#include "sage3basic.h"
#include <Partitioner2/ModulesPe.h>
#include <Partitioner2/Partitioner.h>
#include <Partitioner2/Utility.h>

#include <boost/foreach.hpp>

namespace rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace ModulesPe {

using namespace rose::Diagnostics;

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
    ASSERT_require2(partitioner.instructionProvider().instructionPointerRegister().get_nbits()==32,
                    "FIXME[Robb P. Matzke 2014-08-24]: supports only 32-bit addresses at this time");
    ASSERT_require2(partitioner.instructionProvider().defaultByteOrder()==ByteOrder::ORDER_LSB,
                    "FIXME[Robb P. Matzke 2014-08-24]: supports only little-endian architectures at this time");

    // FIXME[Robb P. Matzke 2014-08-24]: we should probably check that monkeying with memory won't affect existing instructions

    // First, aggregate neighboring IAT entries so we don't need to create so many map segments (easier for users to debug
    // their code if we don't introduce hundreds of segments).
    AddressIntervalSet iatAddresses;
    BOOST_FOREACH (SgAsmPEImportItem *import, index.values())
        iatAddresses.insert(AddressInterval::baseSize(import->get_iat_entry_va(), 4));

    // Add segments to the memory map.
    BOOST_FOREACH (const AddressInterval &iatExtent, iatAddresses.intervals()) {
        partitioner.memoryMap().insert(iatExtent,
                                       MemoryMap::Segment::anonymousInstance(iatExtent.size(), MemoryMap::READABLE,
                                                                             "partitioner-adjusted IAT"));
    }

    // Write IAT entries into the newly mapped IATs
    BOOST_FOREACH (const ImportIndex::Node &node, index.nodes()) {
        uint32_t packed;
        ByteOrder::host_to_le(node.key(), &packed);
        rose_addr_t iatVa = node.value()->get_iat_entry_va();
        if (4!=partitioner.memoryMap().at(iatVa).limit(4).write((uint8_t*)&packed).size())
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
        if (insn->get_kind()!=x86_jmp || insn->get_operandList()->get_operands().size()!=1)
            continue;                                   // ...that is a JMP...
        SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(insn->get_operandList()->get_operands()[0]);
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
    partitioner.setMayReturnBlacklisted("TerminateThread@KERNEL32.dll");
    partitioner.setMayReturnBlacklisted("RaiseException@KERNEL32.dll");
    partitioner.setMayReturnBlacklisted("TerminateProcess@KERNEL32.dll");
    partitioner.setMayReturnBlacklisted("ExitProcess@KERNEL32.dll");
}

} // namespace
} // namespace
} // namespace
} // namespace
