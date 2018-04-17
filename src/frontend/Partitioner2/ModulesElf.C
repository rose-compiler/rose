#include "sage3basic.h"
#include <Partitioner2/ModulesElf.h>
#include <Partitioner2/Partitioner.h>
#include <Partitioner2/Utility.h>
#include <x86InstructionProperties.h>

#include <boost/foreach.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace ModulesElf {

using namespace Rose::Diagnostics;

size_t
findErrorHandlingFunctions(SgAsmElfFileHeader *elfHeader, std::vector<Function::Ptr> &functions) {
    struct T1: AstSimpleProcessing {
        size_t nInserted;
        std::vector<Function::Ptr> &functions;
        T1(std::vector<Function::Ptr> &functions): nInserted(0), functions(functions) {}
        void visit(SgNode *node) {
            if (SgAsmElfEHFrameEntryFD *fde = isSgAsmElfEHFrameEntryFD(node)) {
                Function::Ptr function = Function::instance(fde->get_begin_rva().get_rva(), SgAsmFunction::FUNC_EH_FRAME);
                if (insertUnique(functions, function, sortFunctionsByAddress))
                    ++nInserted;
            }
        }
    } t1(functions);
    if (elfHeader!=NULL)
        t1.traverse(elfHeader, preorder);
    return t1.nInserted;
}

std::vector<Function::Ptr>
findErrorHandlingFunctions(SgAsmElfFileHeader *elfHeader) {
    std::vector<Function::Ptr> functions;
    findErrorHandlingFunctions(elfHeader, functions);
    return functions;
}

std::vector<Function::Ptr>
findErrorHandlingFunctions(SgAsmInterpretation *interp) {
    std::vector<Function::Ptr> functions;
    if (interp!=NULL) {
        BOOST_FOREACH (SgAsmGenericHeader *fileHeader, interp->get_headers()->get_headers())
            findErrorHandlingFunctions(isSgAsmElfFileHeader(fileHeader), functions);
    }
    return functions;
}


bool
PltEntryMatcher::match(const Partitioner &partitioner, rose_addr_t anchor) {
    nBytesMatched_ = 0;
    gotEntryVa_ = 0;
    gotEntry_ = 0;

    const RegisterDescriptor REG_IP = partitioner.instructionProvider().instructionPointerRegister();
    SgAsmInstruction *insn = partitioner.discoverInstruction(anchor);
    SgAsmX86Instruction *insnX86 = isSgAsmX86Instruction(insn);

    // Look for the PLT entry.
    if (insnX86) {
        if (!x86InstructionIsUnconditionalBranch(insnX86) || 1!=insn->get_operandList()->get_operands().size())
            return false;

        SgAsmMemoryReferenceExpression *mref = isSgAsmMemoryReferenceExpression(insn->get_operandList()->get_operands()[0]);
        if (SgAsmExpression *addr = mref ? mref->get_address() : NULL) {
            ASSERT_not_null(mref->get_type());
            gotEntryNBytes_ = mref->get_type()->get_nBytes();
            if (SgAsmBinaryAdd *binExpr = isSgAsmBinaryAdd(addr)) {
                SgAsmDirectRegisterExpression *rre = isSgAsmDirectRegisterExpression(binExpr->get_lhs());
                SgAsmIntegerValueExpression *offset = isSgAsmIntegerValueExpression(binExpr->get_rhs());
                if (rre && offset) {
                    if (rre->get_descriptor()==REG_IP) {
                        gotEntryVa_ = baseVa_ + offset->get_absoluteValue() + insn->get_address() + insn->get_size();
                        nBytesMatched_ = insn->get_size();
                    } else if (rre->get_descriptor().get_major()==x86_regclass_gpr) {
                        gotEntryVa_ = baseVa_ + offset->get_absoluteValue();
                        nBytesMatched_ = insn->get_size();
                    }
                }
            } else if (SgAsmIntegerValueExpression *offset = isSgAsmIntegerValueExpression(addr)) {
                gotEntryVa_ = offset->get_absoluteValue();      // do not add baseVa_ to these
                nBytesMatched_ = insn->get_size();
            }
        }
    } else {
        // FIXME[Robb P. Matzke 2014-08-23]: Architecture is not supported yet
        static bool warned = false;
        if (!warned) {
            mlog[WARN] <<"ModulesElf::pltEntryMatcher does not yet support this ISA\n";
            warned = true;
        }
    }
        

    // Read the GOT entry if it's mapped
    if (nBytesMatched_ > 0 && partitioner.memoryMap() != NULL) {
        switch (gotEntryNBytes_) {
            case 0:
                gotEntry_ = 0;
                break;
            case 1:
                gotEntry_ = partitioner.memoryMap()->readUnsigned<uint8_t>(gotEntryVa_).orElse(0);
                break;
            case 2:
                gotEntry_ = partitioner.memoryMap()->readUnsigned<uint16_t>(gotEntryVa_).orElse(0);
                break;
            case 4:
                gotEntry_ = partitioner.memoryMap()->readUnsigned<uint32_t>(gotEntryVa_).orElse(0);
                break;
            case 8:
                gotEntry_ = partitioner.memoryMap()->readUnsigned<uint64_t>(gotEntryVa_).orElse(0);
                break;
            default:
                ASSERT_not_reachable("invalid GOT entry size: " + StringUtility::numberToString(gotEntryNBytes_));
        }
    }

    return nBytesMatched_ > 0;
}

size_t
findPltFunctions(const Partitioner &partitioner, SgAsmElfFileHeader *elfHeader, std::vector<Function::Ptr> &functions) {
    // Find important sections
    SgAsmGenericSection *plt = elfHeader ? elfHeader->get_section_by_name(".plt") : NULL;
    SgAsmGenericSection *gotplt = elfHeader ? elfHeader->get_section_by_name(".got.plt") : NULL;
    if (!plt || !plt->is_mapped() || !gotplt || !gotplt->is_mapped())
        return 0;

    // Find all relocation sections
    std::set<SgAsmElfRelocSection*> relocSections;
    BOOST_FOREACH (SgAsmGenericSection *section, elfHeader->get_sections()->get_sections()) {
        if (SgAsmElfRelocSection *relocSection = isSgAsmElfRelocSection(section))
            relocSections.insert(relocSection);
        
    }
    if (relocSections.empty())
        return 0;

    // Look at each instruction in the .plt section. If the instruction is a computed jump to an address stored in the .got.plt
    // then we've found the beginning of a plt trampoline.
    size_t nInserted = 0;
    rose_addr_t pltOffset = 14; /* skip the first entry (PUSH ds:XXX; JMP ds:YYY; 0x00; 0x00)--the JMP is not a function*/
    while (pltOffset<plt->get_mapped_size()) {
#if 0 // [Robb P Matzke 2016-10-14]: it seems the base address needs to be zero most of the time, at least for amd64 gcc
        PltEntryMatcher matcher(elfHeader->get_base_va() + gotplt->get_mapped_preferred_rva());
#else
        PltEntryMatcher matcher(elfHeader->get_base_va());
#endif
        rose_addr_t pltEntryVa = plt->get_mapped_actual_va() + pltOffset;
        if (!matcher.match(partitioner, pltEntryVa)) {
            ++pltOffset;
            continue;
        }
        rose_addr_t gotpltVa = matcher.memAddress();    // address that was read by indirect branch
        if (gotpltVa <  elfHeader->get_base_va() + gotplt->get_mapped_preferred_rva() ||
            gotpltVa >= elfHeader->get_base_va() + gotplt->get_mapped_preferred_rva() + gotplt->get_mapped_size()) {
            ++pltOffset;
            continue;                                   // jump is not indirect through the .got.plt section
        }

        // Find the relocation entry whose offset is the gotpltVa and use that entry's symbol for the function name
        std::string name;
        BOOST_FOREACH (SgAsmElfRelocSection *relocSection, relocSections) {
            SgAsmElfSymbolSection *symbolSection = isSgAsmElfSymbolSection(relocSection->get_linked_section());
            if (SgAsmElfSymbolList *symbols = symbolSection ? symbolSection->get_symbols() : NULL) {
                BOOST_FOREACH (SgAsmElfRelocEntry *rel, relocSection->get_entries()->get_entries()) {
                    if (rel->get_r_offset()==gotpltVa) {
                        unsigned long symbolIdx = rel->get_sym();
                        if (symbolIdx < symbols->get_symbols().size()) {
                            SgAsmElfSymbol *symbol = symbols->get_symbols()[symbolIdx];
                            name = symbol->get_name()->get_string() + "@plt";
                            goto foundName;
                        }
                    }
                }
            }
        }
    foundName:

        Function::Ptr function = Function::instance(pltEntryVa, name, SgAsmFunction::FUNC_IMPORT);
        if (insertUnique(functions, function, sortFunctionsByAddress))
            ++nInserted;
        pltOffset += matcher.nBytesMatched();

        // FIXME[Robb P. Matzke 2014-08-23]: we can assume that some functions don't ever return, or they return to the call
        // site: abort, execl, execlp, execv, execvp, exit, _exit, fexecve, longjmp, __longjmp, siglongjmp.
    }

    // The first entry of the .plt section is the call to the function that resolves dynamic links
    Function::Ptr linkerTrampoline = Function::instance(plt->get_mapped_actual_va(), "DYNAMIC_LINKER_TRAMPOLINE",
                                                        SgAsmFunction::FUNC_IMPORT);
    if (insertUnique(functions, linkerTrampoline, sortFunctionsByAddress))
        ++nInserted;

    return nInserted;
}

bool
isImport(const Partitioner &partitioner, const Function::Ptr &function) {
    if (!function)
        return false;
    if (0 == (function->reasons() & SgAsmFunction::FUNC_IMPORT))
        return false;
    PltEntryMatcher matcher(0);
    return matcher.match(partitioner, function->address());
}

bool
isLinkedImport(const Partitioner &partitioner, const Function::Ptr &function) {
    // Is function an import?
    if (!function)
        return false;
    if (0 == (function->reasons() & SgAsmFunction::FUNC_IMPORT))
        return false;
    PltEntryMatcher matcher(0);
    if (!matcher.match(partitioner, function->address()))
        return false;
    if (matcher.gotEntryVa() == 0)
        return false;
    SgAsmInstruction *insn = partitioner.instructionProvider()[function->address()];
    if (!insn)
        return false;

    // Is the import linked?
    rose_addr_t fallthrough = insn->get_address() + insn->get_size();
    return matcher.gotEntry() != 0 && matcher.gotEntry() != fallthrough;
}

bool
isUnlinkedImport(const Partitioner &partitioner, const Function::Ptr &function) {
    // Is function an import?
    if (!function)
        return false;
    if (0 == (function->reasons() & SgAsmFunction::FUNC_IMPORT))
        return false;
    PltEntryMatcher matcher(0);
    if (!matcher.match(partitioner, function->address()))
        return false;
    if (matcher.gotEntryVa() == 0)
        return false;
    SgAsmInstruction *insn = partitioner.instructionProvider()[function->address()];
    if (!insn)
        return false;

    // Is function NOT linked?
    rose_addr_t fallthrough = insn->get_address() + insn->get_size();
    return matcher.gotEntry() == 0 || matcher.gotEntry() == fallthrough;
}

std::vector<Function::Ptr>
findPltFunctions(const Partitioner &partitioner, SgAsmElfFileHeader *elfHeader) {
    std::vector<Function::Ptr> functions;
    findPltFunctions(partitioner, elfHeader, functions);
    return functions;
}

std::vector<Function::Ptr>
findPltFunctions(const Partitioner &partitioner, SgAsmInterpretation *interp) {
    std::vector<Function::Ptr> functions;
    if (interp!=NULL) {
        BOOST_FOREACH (SgAsmGenericHeader *fileHeader, interp->get_headers()->get_headers())
            findPltFunctions(partitioner, isSgAsmElfFileHeader(fileHeader), functions);
    }
    return functions;
}

void
buildMayReturnLists(Partitioner &p) {
    // Most of these were obtained by searching for "noreturn" in all the header files on Debian Squeeze.
    // Please keep this list alphabetical (ignoring leading underscores).
    Configuration &c = p.configuration();

    c.insertMaybeFunction("abort@plt").mayReturn(false);
    c.insertMaybeFunction("abort@plt").mayReturn(false);
    c.insertMaybeFunction("__assert@plt").mayReturn(false);
    c.insertMaybeFunction("__assert_fail@plt").mayReturn(false);
    c.insertMaybeFunction("__assert_perror_fail@plt").mayReturn(false);
    c.insertMaybeFunction("err@plt").mayReturn(false);
    c.insertMaybeFunction("errx@plt").mayReturn(false);
    c.insertMaybeFunction("_exit@plt").mayReturn(false);
    c.insertMaybeFunction("_Exit@plt").mayReturn(false);
    c.insertMaybeFunction("exit@plt").mayReturn(false);
    c.insertMaybeFunction("__longjmp_chk@plt").mayReturn(false);
    c.insertMaybeFunction("longjmp@plt").mayReturn(false);
    c.insertMaybeFunction("pthread_exit@plt").mayReturn(false);
    c.insertMaybeFunction("quick_exit@plt").mayReturn(false);
    c.insertMaybeFunction("__pthread_unwind_next@plt").mayReturn(false);
    c.insertMaybeFunction("rethrow_exception@plt").mayReturn(false);
    c.insertMaybeFunction("siglongjmp@plt").mayReturn(false);
    c.insertMaybeFunction("__throw_bad_alloc@plt").mayReturn(false);
    c.insertMaybeFunction("__throw_bad_cast@plt").mayReturn(false);
    c.insertMaybeFunction("__throw_bad_exception@plt").mayReturn(false);
    c.insertMaybeFunction("__throw_bad_typeid@plt").mayReturn(false);
    c.insertMaybeFunction("__throw_domain_error@plt").mayReturn(false);
    c.insertMaybeFunction("__throw_invalid_argument@plt").mayReturn(false);
    c.insertMaybeFunction("__throw_ios_failure@plt").mayReturn(false);
    c.insertMaybeFunction("__throw_length_error@plt").mayReturn(false);
    c.insertMaybeFunction("__throw_logic_error@plt").mayReturn(false);
    c.insertMaybeFunction("__throw_out_of_range@plt").mayReturn(false);
    c.insertMaybeFunction("__throw_overflow_error@plt").mayReturn(false);
    c.insertMaybeFunction("__throw_range_error@plt").mayReturn(false);
    c.insertMaybeFunction("__throw_runtime_error@plt").mayReturn(false);
    c.insertMaybeFunction("__throw_system_error@plt").mayReturn(false);
    c.insertMaybeFunction("__throw_underflow_error@plt").mayReturn(false);
    c.insertMaybeFunction("verr@plt").mayReturn(false);
    c.insertMaybeFunction("verrx@plt").mayReturn(false);
}

} // namespace
} // namespace
} // namespace
} // namespace
