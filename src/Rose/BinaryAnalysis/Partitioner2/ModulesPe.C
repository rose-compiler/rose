#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Partitioner2/ModulesPe.h>

#include <Rose/As.h>
#include <Rose/AST/Traversal.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/Function.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/Partitioner2/Utility.h>
#include <Rose/BinaryAnalysis/RelativeVirtualAddress.h>

#include <SgAsmFunction.h>
#include <SgAsmGenericHeaderList.h>
#include <SgAsmGenericSection.h>
#include <SgAsmGenericSectionList.h>
#include <SgAsmGenericString.h>
#include <SgAsmIntegerType.h>
#include <SgAsmIntegerValueExpression.h>
#include <SgAsmInterpretation.h>
#include <SgAsmMemoryReferenceExpression.h>
#include <SgAsmPEExportEntry.h>
#include <SgAsmPEExportEntryList.h>
#include <SgAsmPEExportSection.h>
#include <SgAsmPEFileHeader.h>
#include <SgAsmPEImportDirectory.h>
#include <SgAsmPEImportDirectoryList.h>
#include <SgAsmPEImportItem.h>
#include <SgAsmPEImportItemList.h>
#include <SgAsmPEImportSection.h>
#include <SgAsmX86Instruction.h>

#include <Cxx_GrammarDowncast.h>

#include <Sawyer/StaticBuffer.h>

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
getImportIndex(const Partitioner::ConstPtr&, SgAsmPEFileHeader *peHeader, ImportIndex &index /*in,out*/) {
    size_t nInserted = 0;
    if (peHeader != nullptr) {
        for (SgAsmGenericSection *section: peHeader->get_sections()->get_sections()) {
            if (SgAsmPEImportSection *importSection = isSgAsmPEImportSection(section)) {
                for (SgAsmPEImportDirectory *importDir: importSection->get_importDirectories()->get_vector()) {
                    for (SgAsmPEImportItem *import: importDir->get_imports()->get_vector()) {
                        if (import->get_hintname_rva() != 0 || import->get_hint() != 0 ||
                            !import->get_name()->get_string().empty()) {
                            Address va = *import->get_hintname_rva().va();
                            if (index.insertMaybe(va, import))
                                ++nInserted;
                        }
                    }
                }
            }
        }
    }
    return nInserted;
}

ImportIndex
getImportIndex(const Partitioner::ConstPtr &partitioner, SgAsmPEFileHeader *peHeader) {
    ImportIndex index;
    getImportIndex(partitioner, peHeader, index);
    return index;
}

ImportIndex
getImportIndex(const Partitioner::ConstPtr &partitioner, SgAsmInterpretation *interp) {
    ImportIndex index;
    if (interp != nullptr) {
        for (SgAsmGenericHeader *fileHeader: interp->get_headers()->get_headers())
            getImportIndex(partitioner, isSgAsmPEFileHeader(fileHeader), index);
    }
    return index;
}

size_t
findExportFunctions(const Partitioner::ConstPtr &partitioner, SgAsmPEFileHeader *peHeader, std::vector<Function::Ptr> &functions) {
    ASSERT_not_null(partitioner);
    size_t nInserted = 0;
    if (peHeader != nullptr) {
        for (SgAsmGenericSection *section: peHeader->get_sections()->get_sections()) {
            if (SgAsmPEExportSection *exportSection = isSgAsmPEExportSection(section)) {
                for (SgAsmPEExportEntry *exportEntry: exportSection->get_exports()->get_exports()) {
                    if (const auto va = exportEntry->get_exportRva().va()) {
                        if (partitioner->discoverInstruction(*va)) {
                            Function::Ptr function = Function::instance(*va, exportEntry->get_name()->get_string(),
                                                                        SgAsmFunction::FUNC_EXPORT);
                            if (insertUnique(functions, function, sortFunctionsByAddress))
                                ++nInserted;
                        }
                    }
                }
            }
        }
    }
    return nInserted;
}

std::vector<Function::Ptr>
findExportFunctions(const Partitioner::ConstPtr &partitioner, SgAsmPEFileHeader *peHeader) {
    std::vector<Function::Ptr> functions;
    findExportFunctions(partitioner, peHeader, functions);
    return functions;
}

std::vector<Function::Ptr>
findExportFunctions(const Partitioner::ConstPtr &partitioner, SgAsmInterpretation *interp) {
    std::vector<Function::Ptr> functions;
    if (interp != nullptr) {
        for (SgAsmGenericHeader *fileHeader: interp->get_headers()->get_headers())
            findExportFunctions(partitioner, isSgAsmPEFileHeader(fileHeader), functions);
    }
    return functions;
}

size_t
findImportFunctions(const Partitioner::ConstPtr&, SgAsmPEFileHeader *peHeader, const ImportIndex &imports,
                    std::vector<Function::Ptr> &functions) {
    size_t nInserted = 0;
    if (peHeader) {
        for (const ImportIndex::Node &import: imports.nodes()) {
            std::string name = import.value()->get_name()->get_string();
            SgAsmPEImportDirectory *importDir = AST::Traversal::findParentTyped<SgAsmPEImportDirectory>(import.value());
            if (importDir && !importDir->get_dllName()->get_string().empty())
                name += "@" + importDir->get_dllName()->get_string();
            Function::Ptr function = Function::instance(import.key(), name, SgAsmFunction::FUNC_IMPORT);
            if (insertUnique(functions, function, sortFunctionsByAddress))
                ++nInserted;
        }
    }
    return nInserted;
}

std::vector<Function::Ptr>
findImportFunctions(const Partitioner::ConstPtr &partitioner, SgAsmPEFileHeader *peHeader) {
    ImportIndex imports = getImportIndex(partitioner, peHeader);
    std::vector<Function::Ptr> functions;
    findImportFunctions(partitioner, peHeader, imports, functions);
    return functions;
}

std::vector<Function::Ptr>
findImportFunctions(const Partitioner::ConstPtr &partitioner, SgAsmInterpretation *interp) {
    std::vector<Function::Ptr> functions;
    if (interp != nullptr) {
        ImportIndex imports = getImportIndex(partitioner, interp);
        for (SgAsmGenericHeader *fileHeader: interp->get_headers()->get_headers())
            findImportFunctions(partitioner, isSgAsmPEFileHeader(fileHeader), imports, functions);
    }
    return functions;
}

static void
insertIatMemory(const MemoryMap::Ptr &mem, const AddressInterval &interval) {
    auto newBuffer = MemoryMap::AllocatingBuffer::instance(interval.size());
    mem->insert(interval, MemoryMap::Segment(newBuffer, 0, MemoryMap::READ_WRITE, "IAT modified address"));
}

static void
makeMemoryWritable(const MemoryMap::Ptr &mem, const AddressIntervalSet &where) {
    for (const AddressInterval &interval: where.intervals()) {
        const auto node = mem->find(interval.least());
        if (node != mem->intervals().end()) {
            if (const auto existingBuffer = as<MemoryMap::StaticBuffer>(node->value().buffer())) {
                if (existingBuffer->isReadOnly())
                    insertIatMemory(mem, interval);
            } else if (const auto existingBuffer = as<MemoryMap::AllocatingBuffer>(node->value().buffer())) {
                // these buffers are always writable
            } else {
                // not sure about these, so map new buffers
                insertIatMemory(mem, interval);
            }
        } else {
            insertIatMemory(mem, interval);
        }
    }
}

void
rebaseImportAddressTables(const Partitioner::Ptr &partitioner, const ImportIndex &index) {
    ASSERT_not_null(partitioner);
    const size_t wordSize = partitioner->instructionProvider().instructionPointerRegister().nBits() / 8;
    if (wordSize > 8) {
        mlog[WARN] <<"ModulesPe::rebaseImportAddressTable does not support a word size of "
                   <<StringUtility::plural(wordSize, "bytes") <<"\n";
        return;
    }

    // We need to make sure the underlying buffers are writable before we start writing to them. The easiest way to do this is to
    // just make new buffers and map them over the top of any existing read-only buffers. First, gather the addresses so we're
    // creating as few new buffers as possible.
    AddressIntervalSet toWrite;
    for (const ImportIndex::Node &node: index.nodes()) {
        if (node.value()->get_iat_written())
            continue;
        const Address iatVa = node.value()->get_iatEntryVa();
        toWrite |= AddressInterval::baseSize(iatVa, wordSize);
    }
    makeMemoryWritable(partitioner->memoryMap(), toWrite);

    // Write IAT entries into the newly mapped IATs
    for (const ImportIndex::Node &node: index.nodes()) {
        //First check if the loader already set the address
        if (node.value()->get_iat_written())
            continue;
    
        // Pack it as little-endian
        uint8_t packed[8];
        memset(packed, 0, 8);
        for (size_t i=0; i<wordSize; ++i)
            packed[i] = (node.key() >> (8*i)) & 0xff;

        // Then reorder bytes for other sexes
        switch (partitioner->instructionProvider().defaultByteOrder()) {
            case ByteOrder::ORDER_LSB:
                break;

            case ByteOrder::ORDER_MSB:
                std::reverse(packed+0, packed+wordSize);
                break;

            case ByteOrder::ORDER_UNSPECIFIED:
                ASSERT_not_reachable("unknown default byte order");
        }
        
        Address iatVa = node.value()->get_iatEntryVa();
        if (wordSize != partitioner->memoryMap()->at(iatVa).limit(wordSize).write(packed).size()){
            ASSERT_not_reachable("write failed to map we just created");
        }else{
            node.value()->set_iat_written(true);
        }
    }
}

void
nameImportThunks(const Partitioner::ConstPtr &partitioner, SgAsmInterpretation *interp) {
    ASSERT_not_null(partitioner);
    if (interp == nullptr)
        return;
    std::vector<Function::Ptr> functions = partitioner->functions();

    // Get the addresses for the PE Import Address Tables
    AddressIntervalSet iatExtent;
    for (SgAsmGenericHeader *fileHeader: interp->get_headers()->get_headers()) {
        SgAsmGenericSectionPtrList iatSections = fileHeader->get_sectionsByName("Import Address Table");
        for (SgAsmGenericSection *section: iatSections) {
            if (section->get_id()==-1 && section->isMapped())
                iatExtent.insert(AddressInterval::baseSize(section->get_mappedActualVa(), section->get_mappedSize()));
        }
    }
    if (iatExtent.isEmpty())
        return;

    // Build an index that maps addresses to entries in the import tables.  The addresses are the addresses where the imported
    // functions are expected to be mapped.
    ImportIndex importIndex = getImportIndex(partitioner, interp);

    // Process each function that's attached to the CFG/AUM
    for (const Function::Ptr &function: functions) {
        if (!function->name().empty())
            continue;                                   // no need to name functions that already have a name
        if (function->basicBlockAddresses().size()!=1)
            continue;                                   // thunks have only one basic block...
        BasicBlock::Ptr bblock = partitioner->basicBlockExists(function->address());
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
        SgAsmIntegerValueExpression *addr = mre ? isSgAsmIntegerValueExpression(mre->get_address()) : nullptr;
        if (!addr)
            continue;                                   // ...with addressing mode [C] where C is a constant...
        AddressInterval indirectionAddress = AddressInterval::baseSize(addr->get_absoluteValue(), 4);
        if (!iatExtent.contains(indirectionAddress))
            continue;                                   // ...and where C is inside an Import Address Table...
        bool isComplete = true;
        std::vector<Address> successors = partitioner->basicBlockConcreteSuccessors(bblock, &isComplete);
        if (!isComplete || successors.size()!=1)
            continue;                                   // ...and the JMP has a single successor that is concrete...
        SgAsmPEImportItem *importItem = nullptr;
        if (!importIndex.getOptional(successors.front()).assignTo(importItem))
            continue;                                   // ...and is a known address for an imported function.
        
        // Merge the new name into the function
        std::string importName = importItem->get_name()->get_string();
        SgAsmPEImportDirectory *importDir = AST::Traversal::findParentTyped<SgAsmPEImportDirectory>(importItem);
        if (importDir && !importDir->get_dllName()->get_string().empty())
            importName += "@" + importDir->get_dllName()->get_string();
        function->name(importName);
        function->insertReasons(SgAsmFunction::FUNC_THUNK | SgAsmFunction::FUNC_IMPORT);
    }
}

void
buildMayReturnLists(const Partitioner::Ptr &partitioner) {
    ASSERT_not_null(partitioner);

    // The following functions never return to their caller
    // FIXME[Robb P. Matzke 2014-11-20]: This list needs to be expanded; I'm only listing those I've actually seen in the wild
    //      because I haven't been able to find a good list anywhere.
    Configuration &c = partitioner->configuration();
    c.insertMaybeFunction("TerminateThread@KERNEL32.dll").mayReturn(false);
    c.insertMaybeFunction("ExitProcess@KERNEL32.dll").mayReturn(false);

    // These may return to the caller.
    c.insertMaybeFunction("RaiseException@KERNEL32.dll").mayReturn(true);
    c.insertMaybeFunction("TerminateProcess@KERNEL32.dll").mayReturn(true);
}

void
PeDescrambler::nameKeyAddresses(const Partitioner::Ptr &partitioner) {
    ASSERT_not_null(partitioner);
    if (partitioner->addressName(dispatcherVa_).empty())
        partitioner->addressName(dispatcherVa_, "PEScrambler_dispatcher");
    if (partitioner->addressName(dispatchTableVa_).empty())
        partitioner->addressName(dispatchTableVa_, "PEScrambler_dispatch_table");
}

bool
PeDescrambler::operator()(bool chain, const Args &args) {
    if (!checkedPreconditions_) {
        if (args.partitioner->instructionProvider().instructionPointerRegister().nBits() != 32)
            throw std::runtime_error("PeDescrambler module only works on 32-bit specimens");
        checkedPreconditions_ = true;
    }

    Address calleeVa = 0;
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
PeDescrambler::basicBlockCallsDispatcher(const Partitioner::ConstPtr &partitioner, const BasicBlock::Ptr &bblock) {
    ASSERT_not_null(partitioner);
    ASSERT_not_null(bblock);
    SgAsmX86Instruction *x86insn = isSgAsmX86Instruction(bblock->instructions().back());
    if (!x86insn || (x86insn->get_kind() != x86_call && x86insn->get_kind() != x86_farcall))
        return false;
    std::vector<Address> successors = partitioner->basicBlockConcreteSuccessors(bblock);
    return 1==successors.size() && 1==partitioner->basicBlockSuccessors(bblock).size() && successors.front() == dispatcherVa_;
}

Sawyer::Optional<Address>
PeDescrambler::findCalleeAddress(const Partitioner::ConstPtr &partitioner, Address returnVa) {
    ASSERT_not_null(partitioner);
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
            Address batchVa = dispatchTableVa_ + dispatchTable_.size() * sizeof(DispatchEntry);
            size_t nReadBytes = partitioner->memoryMap()->at(batchVa).limit(sizeof buf).read((uint8_t*)buf).size();
            reachedEndOfTable_ = nReadBytes < bytesPerEntry;
            for (size_t i=0; 4*(i+1)<nReadBytes; i+=2)
                dispatchTable_.push_back(DispatchEntry(ByteOrder::leToHost(buf[i+0]), ByteOrder::leToHost(buf[i+1])));
            SAWYER_MESG(mlog[DEBUG]) <<"read " <<StringUtility::plural(nReadBytes, "bytes") <<" from PEScrambler dispatch "
                                     <<"table at " <<StringUtility::addrToString(batchVa) <<"\n";
        }
        if (tableIdx >= dispatchTable_.size())
            return Sawyer::Nothing();               // couldn't find a suitable entry
        if (dispatchTable_[tableIdx].returnVa == returnVa) {
            Address va = dispatchTable_[tableIdx].calleeVa;
            if (hitNullEntry) {
                uint32_t va2;
                if (4 != partitioner->memoryMap()->at(va).limit(4).read((uint8_t*)&va2).size())
                    return Sawyer::Nothing();       // couldn't dereference table entry
                va = ByteOrder::leToHost(va2);
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

#endif
