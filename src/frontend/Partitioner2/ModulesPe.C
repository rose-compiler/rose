#include "sage3basic.h"
#include <Partitioner2/ModulesPe.h>
#include <Partitioner2/Partitioner.h>
#include <Partitioner2/Utility.h>

#include <boost/foreach.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace ModulesPe {

using namespace Rose::Diagnostics;

// convert "EncodePointer@KERNEL32.dll" (ROSE's canonical form) to "KERNEL32.dll:EncodePointer" (typical of Windows)
std::string
systemFunctionName(const std::string &name) {
    size_t atsign = name.find_last_of('@');
    if (atsign != std::string::npos && atsign >= 1 &&
        atsign + 4 < name.size() && name[atsign+1]!='@' && boost::ends_with(name, ".dll")) {
        std::string library = name.substr(atsign+1);
        std::string function = name.substr(0, atsign);
        return library + ":" + function;
    }
    return name;
}

// Scan PE import sections to build an index
size_t
getImportIndex(const Partitioner &partitioner, SgAsmPEFileHeader *peHeader, ImportIndex &index /*in,out*/) {
    size_t nInserted = 0;
    if (peHeader!=NULL) {
        BOOST_FOREACH (SgAsmGenericSection *section, peHeader->get_sections()->get_sections()) {
            if (SgAsmPEImportSection *importSection = isSgAsmPEImportSection(section)) {
                BOOST_FOREACH (SgAsmPEImportDirectory *importDir, importSection->get_import_directories()->get_vector()) {
                    BOOST_FOREACH (SgAsmPEImportItem *import, importDir->get_imports()->get_vector()) {
                        rose_addr_t va = import->get_hintname_rva().get_va();
                        if (index.insertMaybe(va, import))
                            ++nInserted;
                    }
                }
            }
        }
    }
    return nInserted;
}

ImportIndex
getImportIndex(const Partitioner &partitioner, SgAsmPEFileHeader *peHeader) {
    ImportIndex index;
    getImportIndex(partitioner, peHeader, index);
    return index;
}

ImportIndex
getImportIndex(const Partitioner &partitioner, SgAsmInterpretation *interp) {
    ImportIndex index;
    if (interp!=NULL) {
        BOOST_FOREACH (SgAsmGenericHeader *fileHeader, interp->get_headers()->get_headers())
            getImportIndex(partitioner, isSgAsmPEFileHeader(fileHeader), index);
    }
    return index;
}

size_t
findExportFunctions(const Partitioner &partitioner, SgAsmPEFileHeader *peHeader, std::vector<Function::Ptr> &functions) {
    size_t nInserted = 0;
    if (peHeader!=NULL) {
        BOOST_FOREACH (SgAsmGenericSection *section, peHeader->get_sections()->get_sections()) {
            if (SgAsmPEExportSection *exportSection = isSgAsmPEExportSection(section)) {
                BOOST_FOREACH (SgAsmPEExportEntry *exportEntry, exportSection->get_exports()->get_exports()) {
                    rose_addr_t va = exportEntry->get_export_rva().get_va();
                    if (partitioner.discoverInstruction(va)) {
                        Function::Ptr function = Function::instance(va, exportEntry->get_name()->get_string(),
                                                                    SgAsmFunction::FUNC_EXPORT);
                        if (insertUnique(functions, function, sortFunctionsByAddress))
                            ++nInserted;
                    }
                }
            }
        }
    }
    return nInserted;
}

std::vector<Function::Ptr>
findExportFunctions(const Partitioner &partitioner, SgAsmPEFileHeader *peHeader) {
    std::vector<Function::Ptr> functions;
    findExportFunctions(partitioner, peHeader, functions);
    return functions;
}

std::vector<Function::Ptr>
findExportFunctions(const Partitioner &partitioner, SgAsmInterpretation *interp) {
    std::vector<Function::Ptr> functions;
    if (interp!=NULL) {
        BOOST_FOREACH (SgAsmGenericHeader *fileHeader, interp->get_headers()->get_headers())
            findExportFunctions(partitioner, isSgAsmPEFileHeader(fileHeader), functions);
    }
    return functions;
}

size_t
findImportFunctions(const Partitioner &partitioner, SgAsmPEFileHeader *peHeader, const ImportIndex &imports,
                    std::vector<Function::Ptr> &functions) {
    size_t nInserted = 0;
    if (peHeader) {
        BOOST_FOREACH (const ImportIndex::Node &import, imports.nodes()) {
            std::string name = import.value()->get_name()->get_string();
            SgAsmPEImportDirectory *importDir = SageInterface::getEnclosingNode<SgAsmPEImportDirectory>(import.value());
            if (importDir && !importDir->get_dll_name()->get_string().empty())
                name += "@" + importDir->get_dll_name()->get_string();
            Function::Ptr function = Function::instance(import.key(), name, SgAsmFunction::FUNC_IMPORT);
            if (insertUnique(functions, function, sortFunctionsByAddress))
                ++nInserted;
        }
    }
    return nInserted;
}

std::vector<Function::Ptr>
findImportFunctions(const Partitioner &partitioner, SgAsmPEFileHeader *peHeader) {
    ImportIndex imports = getImportIndex(partitioner, peHeader);
    std::vector<Function::Ptr> functions;
    findImportFunctions(partitioner, peHeader, imports, functions);
    return functions;
}

std::vector<Function::Ptr>
findImportFunctions(const Partitioner &partitioner, SgAsmInterpretation *interp) {
    std::vector<Function::Ptr> functions;
    if (interp!=NULL) {
        ImportIndex imports = getImportIndex(partitioner, interp);
        BOOST_FOREACH (SgAsmGenericHeader *fileHeader, interp->get_headers()->get_headers())
            findImportFunctions(partitioner, isSgAsmPEFileHeader(fileHeader), imports, functions);
    }
    return functions;
}

void
rebaseImportAddressTables(Partitioner &partitioner, const ImportIndex &index) {
    const size_t wordSize = partitioner.instructionProvider().instructionPointerRegister().nBits() / 8;
    if (wordSize > 8) {
        mlog[WARN] <<"ModulesPe::rebaseImportAddressTable does not support a word size of "
                   <<StringUtility::plural(wordSize, "bytes") <<"\n";
        return;
    }

    // FIXME[Robb P. Matzke 2014-08-24]: we should probably check that monkeying with memory won't affect existing instructions

    // First, aggregate neighboring IAT entries so we don't need to create so many map segments (easier for users to debug
    // their code if we don't introduce hundreds of segments).
    AddressIntervalSet iatAddresses;
    BOOST_FOREACH (SgAsmPEImportItem *import, index.values())
        iatAddresses.insert(AddressInterval::baseSize(import->get_iat_entry_va(), wordSize));

    // Add segments to the memory map.
    BOOST_FOREACH (const AddressInterval &iatExtent, iatAddresses.intervals()) {
        partitioner.memoryMap()->insert(iatExtent,
                                        MemoryMap::Segment::anonymousInstance(iatExtent.size(), MemoryMap::READABLE,
                                                                              "partitioner-adjusted IAT"));
    }

    // Write IAT entries into the newly mapped IATs
    BOOST_FOREACH (const ImportIndex::Node &node, index.nodes()) {
        // First, pack it as little-endian
        uint8_t packed[8];
        memset(packed, 0, 8);
        for (size_t i=0; i<wordSize; ++i)
            packed[i] = (node.key() >> (8*i)) & 0xff;

        // Then reorder bytes for other sexes
        switch (partitioner.instructionProvider().defaultByteOrder()) {
            case ByteOrder::ORDER_LSB:
                break;

            case ByteOrder::ORDER_MSB:
                std::reverse(packed+0, packed+wordSize);
                break;

            case ByteOrder::ORDER_UNSPECIFIED:
                ASSERT_not_reachable("unknown default byte order");
        }
        
        rose_addr_t iatVa = node.value()->get_iat_entry_va();
        if (wordSize != partitioner.memoryMap()->at(iatVa).limit(wordSize).write(packed).size())
            ASSERT_not_reachable("write failed to map we just created");
    }
}

void
nameImportThunks(const Partitioner &partitioner, SgAsmInterpretation *interp) {
    if (interp==NULL)
        return;
    std::vector<Function::Ptr> functions = partitioner.functions();

    // Get the addresses for the PE Import Address Tables
    AddressIntervalSet iatExtent;
    BOOST_FOREACH (SgAsmGenericHeader *fileHeader, interp->get_headers()->get_headers()) {
        SgAsmGenericSectionPtrList iatSections = fileHeader->get_sections_by_name("Import Address Table");
        BOOST_FOREACH (SgAsmGenericSection *section, iatSections) {
            if (section->get_id()==-1 && section->is_mapped())
                iatExtent.insert(AddressInterval::baseSize(section->get_mapped_actual_va(), section->get_mapped_size()));
        }
    }

    // Build an index that maps addresses to entries in the import tables.  The addresses are the addresses where the imported
    // functions are expected to be mapped.
    ImportIndex importIndex = getImportIndex(partitioner, interp);

    // Process each function that's attached to the CFG/AUM
    BOOST_FOREACH (const Function::Ptr &function, functions) {
        if (!function->name().empty())
            continue;                                   // no need to name functions that already have a name
        if (function->basicBlockAddresses().size()!=1)
            continue;                                   // thunks have only one basic block...
        BasicBlock::Ptr bblock = partitioner.basicBlockExists(function->address());
        ASSERT_not_null(bblock);
        if (bblock->nInstructions()!=1)
            continue;                                   // ...that contains only one instruction...
        SgAsmX86Instruction *insn = isSgAsmX86Instruction(bblock->instructions().front());
        if (!insn) {
            static bool warned;
            if (!warned) {
                mlog[WARN] <<"ModulesPe::nameImportThunks does not yet support this ISA\n";
                warned = true;
            }
            return;
        }
        if (insn->get_kind()!=x86_jmp || insn->nOperands() != 1)
            continue;                                   // ...that is a JMP...
        SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(insn->operand(0));
        SgAsmIntegerValueExpression *addr = mre ? isSgAsmIntegerValueExpression(mre->get_address()) : NULL;
        if (!addr)
            continue;                                   // ...with addressing mode [C] where C is a constant...
        AddressInterval indirectionAddress = AddressInterval::baseSize(addr->get_absoluteValue(), 4);
        if (!iatExtent.contains(indirectionAddress))
            continue;                                   // ...and where C is inside an Import Address Table...
        bool isComplete = true;
        std::vector<rose_addr_t> successors = partitioner.basicBlockConcreteSuccessors(bblock, &isComplete);
        if (!isComplete || successors.size()!=1)
            continue;                                   // ...and the JMP has a single successor that is concrete...
        SgAsmPEImportItem *importItem = NULL;
        if (!importIndex.getOptional(successors.front()).assignTo(importItem))
            continue;                                   // ...and is a known address for an imported function.
        
        // Merge the new name into the function
        std::string importName = importItem->get_name()->get_string();
        SgAsmPEImportDirectory *importDir = SageInterface::getEnclosingNode<SgAsmPEImportDirectory>(importItem);
        if (importDir && !importDir->get_dll_name()->get_string().empty())
            importName += "@" + importDir->get_dll_name()->get_string();
        function->name(importName);
        function->insertReasons(SgAsmFunction::FUNC_THUNK | SgAsmFunction::FUNC_IMPORT);
    }
}

void
buildMayReturnLists(Partitioner &partitioner) {
    // The following functions never return to their caller
    // FIXME[Robb P. Matzke 2014-11-20]: This list needs to be expanded; I'm only listing those I've actually seen in the wild
    //      because I haven't been able to find a good list anywhere.
    Configuration &c = partitioner.configuration();
    c.insertMaybeFunction("TerminateThread@KERNEL32.dll").mayReturn(false);
    c.insertMaybeFunction("ExitProcess@KERNEL32.dll").mayReturn(false);

    // These may return to the caller.
    c.insertMaybeFunction("RaiseException@KERNEL32.dll").mayReturn(true);
    c.insertMaybeFunction("TerminateProcess@KERNEL32.dll").mayReturn(true);
}

void
PeDescrambler::nameKeyAddresses(Partitioner &partitioner) {
    if (partitioner.addressName(dispatcherVa_).empty())
        partitioner.addressName(dispatcherVa_, "PEScrambler_dispatcher");
    if (partitioner.addressName(dispatchTableVa_).empty())
        partitioner.addressName(dispatchTableVa_, "PEScrambler_dispatch_table");
}

bool
PeDescrambler::operator()(bool chain, const Args &args) {
    if (!checkedPreconditions_) {
        if (args.partitioner.instructionProvider().instructionPointerRegister().nBits() != 32)
            throw std::runtime_error("PeDescrambler module only works on 32-bit specimens");
        checkedPreconditions_ = true;
    }

    rose_addr_t calleeVa = 0;
    if (chain &&
        basicBlockCallsDispatcher(args.partitioner, args.bblock) &&
        findCalleeAddress(args.partitioner, args.bblock->fallthroughVa()).assignTo(calleeVa)) {
        SAWYER_MESG(mlog[DEBUG]) <<"PEScrambler edge rewrite"
                                 <<" from " <<StringUtility::addrToString(args.bblock->address())
                                 <<" -> " <<StringUtility::addrToString(dispatcherVa_)
                                 <<" to " <<StringUtility::addrToString(calleeVa) <<"\n";
        args.bblock->successors().clear();
        args.bblock->insertSuccessor(calleeVa, bitsPerWord); // let partitioner figure out the edge type
    }
    return chain;
}

bool
PeDescrambler::basicBlockCallsDispatcher(const Partitioner &partitioner, const BasicBlock::Ptr &bblock) {
    ASSERT_not_null(bblock);
    SgAsmX86Instruction *x86insn = isSgAsmX86Instruction(bblock->instructions().back());
    if (!x86insn || (x86insn->get_kind() != x86_call && x86insn->get_kind() != x86_farcall))
        return false;
    std::vector<rose_addr_t> successors = partitioner.basicBlockConcreteSuccessors(bblock);
    return 1==successors.size() && 1==partitioner.basicBlockSuccessors(bblock).size() && successors.front() == dispatcherVa_;
}

Sawyer::Optional<rose_addr_t>
PeDescrambler::findCalleeAddress(const Partitioner &partitioner, rose_addr_t returnVa) {
    size_t tableIdx = 0;
    bool hitNullEntry = false;
    while (1) {
        if (tableIdx >= dispatchTable_.size() && !reachedEndOfTable_) {
            // suck in some more table entries if possible
            static const size_t nEntriesToRead = 512; // arbitrary
            static const size_t wordsPerEntry = 2;
            static const size_t bytesPerEntry = wordsPerEntry * 4;
            static const size_t nWordsToRead = nEntriesToRead * wordsPerEntry;
            static uint32_t buf[nWordsToRead];
            rose_addr_t batchVa = dispatchTableVa_ + dispatchTable_.size() * sizeof(DispatchEntry);
            size_t nReadBytes = partitioner.memoryMap()->at(batchVa).limit(sizeof buf).read((uint8_t*)buf).size();
            reachedEndOfTable_ = nReadBytes < bytesPerEntry;
            for (size_t i=0; 4*(i+1)<nReadBytes; i+=2)
                dispatchTable_.push_back(DispatchEntry(ByteOrder::le_to_host(buf[i+0]), ByteOrder::le_to_host(buf[i+1])));
            SAWYER_MESG(mlog[DEBUG]) <<"read " <<StringUtility::plural(nReadBytes, "bytes") <<" from PEScrambler dispatch "
                                     <<"table at " <<StringUtility::addrToString(batchVa) <<"\n";
        }
        if (tableIdx >= dispatchTable_.size())
            return Sawyer::Nothing();               // couldn't find a suitable entry
        if (dispatchTable_[tableIdx].returnVa == returnVa) {
            rose_addr_t va = dispatchTable_[tableIdx].calleeVa;
            if (hitNullEntry) {
                uint32_t va2;
                if (4 != partitioner.memoryMap()->at(va).limit(4).read((uint8_t*)&va2).size())
                    return Sawyer::Nothing();       // couldn't dereference table entry
                va = ByteOrder::le_to_host(va2);
            }
            return va;
        }
        if (dispatchTable_[tableIdx].returnVa == 0)
            hitNullEntry = true;
        ++tableIdx;
    }
}

} // namespace
} // namespace
} // namespace
} // namespace
