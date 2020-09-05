#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"
#include <Partitioner2/ModulesElf.h>

#include <Partitioner2/Partitioner.h>
#include <Partitioner2/Utility.h>
#include <rose_getline.h>
#include <Sawyer/FileSystem.h>
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
                function->reasonComment("from EhFrameEntry " + fde->get_begin_rva().to_string());
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

SgAsmInstruction*
PltEntryMatcher::matchNop(const Partitioner &partitioner, rose_addr_t va) {
    if (SgAsmX86Instruction *insn = isSgAsmX86Instruction(partitioner.discoverInstruction(va))) {
        if (insn->get_kind() == x86_nop)
            return insn;
    }
    return NULL;
}

SgAsmInstruction*
PltEntryMatcher::matchPush(const Partitioner &partitioner, rose_addr_t va, rose_addr_t &n /*out*/) {
    n = 0;

    // match "push C" where C is a constant
    if (SgAsmX86Instruction *insn = isSgAsmX86Instruction(partitioner.discoverInstruction(va))) {
        if (insn->get_kind() == x86_push && insn->nOperands() == 1) {
            if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(insn->operand(0))) {
                n = ival->get_absoluteValue();
                return insn;
            }
        }
    }
    return NULL;
}

SgAsmInstruction*
PltEntryMatcher::matchDirectJump(const Partitioner &partitioner, rose_addr_t va) {
    if (SgAsmX86Instruction *insn = isSgAsmX86Instruction(partitioner.discoverInstruction(va))) {
        if (insn->nOperands() == 1 && isSgAsmIntegerValueExpression(insn->operand(0)))
            return insn;
    }
    return NULL;
}

SgAsmInstruction*
PltEntryMatcher::matchIndirectJump(const Partitioner &partitioner, rose_addr_t va,
                                   rose_addr_t &indirectVa /*out*/, size_t &indirectNBytes /*out*/) {
    indirectVa = 0;
    indirectNBytes = 0;
    const RegisterDescriptor REG_IP = partitioner.instructionProvider().instructionPointerRegister();
    if (SgAsmX86Instruction *insn = isSgAsmX86Instruction(partitioner.discoverInstruction(va))) {
        if (x86InstructionIsUnconditionalBranch(insn) && insn->nOperands() == 1 &&
            isSgAsmMemoryReferenceExpression(insn->operand(0))) {
            SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(insn->operand(0));

            // The address could be either a constant, or the IP register plus a constant (or a constant plus IP).
            if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(mre->get_address())) {
                indirectVa = ival->get_absoluteValue();
                indirectNBytes = mre->get_type()->get_nBytes();
                return insn;
            } else if (SgAsmBinaryAdd *add = isSgAsmBinaryAdd(mre->get_address())) {
                SgAsmDirectRegisterExpression *dre = NULL;
                SgAsmIntegerValueExpression *ival = NULL;
                if (isSgAsmDirectRegisterExpression(add->get_lhs()) && isSgAsmIntegerValueExpression(add->get_rhs())) {
                    dre = isSgAsmDirectRegisterExpression(add->get_lhs());
                    ival = isSgAsmIntegerValueExpression(add->get_rhs());
                } else if (isSgAsmIntegerValueExpression(add->get_lhs()) && isSgAsmDirectRegisterExpression(add->get_rhs())) {
                    dre = isSgAsmDirectRegisterExpression(add->get_rhs());
                    ival = isSgAsmIntegerValueExpression(add->get_lhs());
                }
                if (dre && ival && dre->get_descriptor() == REG_IP) {
                    indirectVa = insn->get_address() + insn->get_size() + ival->get_absoluteValue();
                    indirectNBytes = mre->get_type()->get_nBytes();
                    return insn;
                }
            }
        }
    }
    return NULL;
}

SgAsmInstruction*
PltEntryMatcher::matchA64Adrp(const Partitioner &partitioner, rose_addr_t va, rose_addr_t &value /*out*/) {
    value = 0;
#ifdef ROSE_ENABLE_ASM_A64
    // match "adrp X, I" where I is an integer that will be returned through the "value" argument
    if (SgAsmA64Instruction *insn = isSgAsmA64Instruction(partitioner.discoverInstruction(va))) {
        if (insn->get_kind() == A64InstructionKind::ARM64_INS_ADRP) {
            SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(insn->operand(1));
            ASSERT_not_null(ival);
            value = ival->get_absoluteValue();
            return insn;
        }
    }
#endif
    return NULL;
}

SgAsmInstruction*
PltEntryMatcher::matchA64Ldr(const Partitioner &partitioner, rose_addr_t va, rose_addr_t &indirectVa /*in,out*/,
                             rose_addr_t &indirectNBytes /*out*/) {
    indirectNBytes = 0;
#ifdef ROSE_ENABLE_ASM_A64
    // match "ldr R1, [ R2 + C ]" where R1 and R2 are registers and C is a constant. The initial value of R2 is
    // provided as "indirectVa", which will be incremented by C upon return.
    if (SgAsmA64Instruction *insn = isSgAsmA64Instruction(partitioner.discoverInstruction(va))) {
        if (insn->get_kind() != A64InstructionKind::ARM64_INS_LDR || insn->nOperands() != 2)
            return NULL;
        SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(insn->operand(1));
        if (!mre)
            return NULL;

        if (SgAsmBinaryAdd *add = isSgAsmBinaryAdd(mre->get_address())) {
            SgAsmIntegerValueExpression *ival = NULL;
            if (isSgAsmDirectRegisterExpression(add->get_lhs())) {
                ival = isSgAsmIntegerValueExpression(add->get_rhs());
            } else if (isSgAsmDirectRegisterExpression(add->get_rhs())) {
                ival = isSgAsmIntegerValueExpression(add->get_lhs());
            } else {
                return NULL;
            }
            indirectVa += ival->get_absoluteValue();
        } else if (isSgAsmDirectRegisterExpression(mre->get_address())) {
            // use indirectVa as is
        } else {
            return NULL;
        }

        indirectNBytes = mre->get_type()->get_nBytes();
        return insn;
    }
#endif
    return NULL;
}

SgAsmInstruction*
PltEntryMatcher::matchA64Add(const Partitioner &partitioner, rose_addr_t va) {
#ifdef ROSE_ENABLE_ASM_A64
    if (SgAsmA64Instruction *insn = isSgAsmA64Instruction(partitioner.discoverInstruction(va))) {
        if (insn->get_kind() == A64InstructionKind::ARM64_INS_ADD)
            return insn;
    }
#endif
    return NULL;
}

SgAsmInstruction*
PltEntryMatcher::matchA64Br(const Partitioner &partitioner, rose_addr_t va) {
#ifdef ROSE_ENABLE_ASM_A64
    if (SgAsmA64Instruction *insn = isSgAsmA64Instruction(partitioner.discoverInstruction(va))) {
        if (insn->get_kind() == A64InstructionKind::ARM64_INS_BR)
            return insn;
    }
#endif
    return NULL;
}

bool
PltEntryMatcher::match(const Partitioner &partitioner, rose_addr_t anchor) {
    nBytesMatched_ = 0;
    gotEntryVa_ = 0;
    gotEntry_ = 0;
    gotEntryNBytes_ = 0;
    functionNumber_ = 0;

    SgAsmInstruction *insn = partitioner.discoverInstruction(anchor);

    // Look for the PLT entry.
    if (SgAsmX86Instruction *insnX86 = isSgAsmX86Instruction(insn)) {
        bool found = false;
        rose_addr_t indirectVa=0;
        size_t indirectSize=0;

        if (!found) {
            // i386 entries that look like this, and are each 16 bytes:
            //    jmp dword [CONST]; where CONST is a GOT entry address
            //    push N;  where N is a small integer, unique for each dynamically linked function
            //    jmp X;   where X is the address of the first PLT entry
            SgAsmInstruction *ijmp = matchIndirectJump(partitioner, anchor, indirectVa /*out*/, indirectSize /*out*/);
            SgAsmInstruction *push = ijmp ? matchPush(partitioner, ijmp->get_address() + ijmp->get_size(), functionNumber_) : NULL;
            SgAsmInstruction *djmp = push ? matchDirectJump(partitioner, push->get_address() + push->get_size()) : NULL;
            if (djmp) {
                gotEntryNBytes_ = indirectSize;
                gotEntryVa_ = indirectVa;
                nBytesMatched_ = djmp->get_address() + djmp->get_size() - anchor;
                found = true;
            }
        }

        if (!found) {
            // Amd64 entries that look like this, and are each 16 bytes:
            //    nop;     4 bytes
            //    jmp qword [rip + CONST]; address of a GOT entry
            //    nop;     5 bytes
            SgAsmInstruction *nop1 = matchNop(partitioner, anchor);
            SgAsmInstruction *ijmp = nop1 ? matchIndirectJump(partitioner, nop1->get_address() + nop1->get_size(),
                                                              indirectVa /*out*/, indirectSize /*out*/) : NULL;
            SgAsmInstruction *nop2 = ijmp ? matchNop(partitioner, ijmp->get_address() + ijmp->get_size()) : NULL;
            if (nop2) {
                gotEntryNBytes_ = indirectSize;
                gotEntryVa_ = indirectVa;
                nBytesMatched_ = nop2->get_address() + nop2->get_size() - anchor;
                found = true;
            }
        }

        if (!found) {
            // Amd64 entries that look like this, and are each 16 bytes:
            //    nop      ; 4 bytes
            //    push N   ; where N is a small integer, unique for each dynamically linked function
            //    jmp X    ; where X is the address of the first PLT entry
            //    nop      ; 5 bytes
            SgAsmInstruction *nop1 = matchNop(partitioner, anchor);
            SgAsmInstruction *push = nop1 ? matchPush(partitioner, nop1->get_address() + nop1->get_size(), functionNumber_) : NULL;
            SgAsmInstruction *djmp = push ? matchDirectJump(partitioner, push->get_address() + push->get_size()) : NULL;
            if (djmp) {
                gotEntryNBytes_ = 0;                    // not present in PLT entry
                gotEntryVa_ = 0;                        // not present in PLT entry
                nBytesMatched_ = djmp->get_address() + djmp->get_size() - anchor;
                found = true;
            }
        }

#ifdef ROSE_ENABLE_ASM_A64
    } else if (SgAsmA64Instruction *insnA64 = isSgAsmA64Instruction(insn)) {
        if (0 == gotEntryNBytes_) {
            // A64 entries look like this:
            //     adrp     x16, 0x00011000
            //     ldr      x17, u64 [x16 + 0x0000000000000ef8<3832>]
            //     add      x16, x16, 0x0000000000000ef8<3832>
            //     br       x17
            rose_addr_t indirectVa = 0;
            size_t indirectNBytes = 0;
            SgAsmInstruction *adrp = matchA64Adrp(partitioner, anchor, indirectVa /*out*/);
            SgAsmInstruction *ldr = adrp ? matchA64Ldr(partitioner, adrp->get_address() + adrp->get_size(),
                                                       indirectVa /*in,out*/, indirectNBytes /*out*/) : NULL;
            SgAsmInstruction *add = ldr ? matchA64Add(partitioner, ldr->get_address() + ldr->get_size()) : NULL;
            SgAsmInstruction *br = add ? matchA64Br(partitioner, add->get_address() + add->get_size()) : NULL;
            if (br) {
                gotEntryNBytes_ = indirectNBytes;
                gotEntryVa_ = indirectVa;
                nBytesMatched_ = br->get_address() + br->get_size() - anchor;
            }
        }
#endif

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

SgAsmGenericSection*
findPltGot(const Partitioner &partitioner, SgAsmElfFileHeader *elfHeader) {
    if (!elfHeader)
        return NULL;

    // Get the section pointed to by the DT_PLTGOT entry of the .dynamic section.
    if (SgAsmElfDynamicSection *dynamic = isSgAsmElfDynamicSection(elfHeader->get_section_by_name(".dynamic"))) {
        if (SgAsmElfDynamicEntryList *dentriesNode = dynamic->get_entries()) {
            BOOST_FOREACH (SgAsmElfDynamicEntry *dentry, dentriesNode->get_entries()) {
                if (dentry->get_d_tag() == SgAsmElfDynamicEntry::DT_PLTGOT) {
                    rose_rva_t rva = dentry->get_d_val();
                    return rva.get_section();
                }
            }
        }
    }

    // If that failed, just try some common names.
    if (SgAsmGenericSection *pltgot = elfHeader->get_section_by_name(".got.plt"))
        return pltgot;
    if (SgAsmGenericSection *pltgot = elfHeader->get_section_by_name(".plt.got"))
        return pltgot;

    return NULL;
}

PltInfo
findPlt(const Partitioner &partitioner, SgAsmElfFileHeader *elfHeader) {
    if (elfHeader) {
        // The procedure lookup table can be in a variety of places.
        //   1. Sometimes the table is in .plt.sec, starting at the beginning of that section.
        //   1. Sometimes the table is in .plt but the first few entries are garbage
        //   3. Sometimes the table is in .plt.got, starting at the beginning of that section.
        bool foundSection = false;
        for (size_t i = 0; i < 3; ++i) {
            SgAsmGenericSection *section = NULL;
            switch (i) {
                case 0:
                    section = elfHeader->get_section_by_name(".plt.sec");
                    break;
                case 1:
                    section = elfHeader->get_section_by_name(".plt");
                    break;
                case 2:
                    section = elfHeader->get_section_by_name(".plt.got");
                    break;
            }

            // Search for the first valid entry.
            if (section) {
                foundSection = true;
                SAWYER_MESG(mlog[DEBUG]) <<"searching for PLT in \""
                                         <<StringUtility::cEscape(section->get_name()->get_string()) <<"\""
                                         <<" at " <<StringUtility::addrToString(section->get_mapped_actual_va())
                                         <<" + " <<StringUtility::addrToString(section->get_mapped_size())
                                         <<" = " <<StringUtility::addrToString(section->get_mapped_actual_va() +
                                                                               section->get_mapped_size())
                                         <<"\n";
                for (size_t offset = 0; offset < section->get_size(); ++offset) {
                    PltEntryMatcher matcher(elfHeader->get_base_va());
                    if (matcher.match(partitioner, section->get_mapped_actual_va() + offset)) {
                        SAWYER_MESG(mlog[DEBUG]) <<"  matched PLT " <<matcher.nBytesMatched() <<" byte entry"
                                                 <<" at offset " <<offset <<"\n";
                        if (offset + matcher.nBytesMatched() > section->get_size()) {
                            SAWYER_MESG(mlog[DEBUG]) <<"    discarded; extends past section end\n";
                            break;
                        } else if (offset % matcher.nBytesMatched() != 0) {
                            SAWYER_MESG(mlog[DEBUG]) <<"    offset is not a multiple of the  "
                                                     <<matcher.nBytesMatched() <<" byte entry size\n";
                            continue;
                        } else {
                            SAWYER_MESG(mlog[DEBUG]) <<"   entries are each "
                                                     <<StringUtility::plural(matcher.nBytesMatched(), "bytes") <<"\n";
                            PltInfo plt;
                            plt.section = section;
                            plt.firstOffset = offset;
                            plt.entrySize = matcher.nBytesMatched();
                            return plt;
                        }
                    }
                }
            }
        }
        if (foundSection)
            mlog[ERROR] <<"found PLT section(s) but could not parse table\n";
    }

    return PltInfo();
}

size_t
findPltFunctions(const Partitioner &partitioner, SgAsmElfFileHeader *elfHeader, std::vector<Function::Ptr> &functions) {
    // Find important sections
    PltInfo plt = findPlt(partitioner, elfHeader);
    SgAsmGenericSection *gotplt = findPltGot(partitioner, elfHeader);
    if (!plt.section || !plt.section->is_mapped() || !gotplt || !gotplt->is_mapped() || 0 == plt.entrySize)
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
    for (rose_addr_t pltOffset = plt.firstOffset;
         pltOffset + plt.entrySize <= plt.section->get_mapped_size();
         pltOffset += plt.entrySize) {
        PltEntryMatcher matcher(elfHeader->get_base_va());
        rose_addr_t pltEntryVa = plt.section->get_mapped_actual_va() + pltOffset;
        if (!matcher.match(partitioner, pltEntryVa))
            continue;
        rose_addr_t gotpltVa = matcher.memAddress();    // address that was read by indirect branch
        if (gotpltVa <  elfHeader->get_base_va() + gotplt->get_mapped_preferred_rva() ||
            gotpltVa >= elfHeader->get_base_va() + gotplt->get_mapped_preferred_rva() + gotplt->get_mapped_size()) {
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

        // FIXME[Robb P. Matzke 2014-08-23]: we can assume that some functions don't ever return, or they return to the call
        // site: abort, execl, execlp, execv, execvp, exit, _exit, fexecve, longjmp, __longjmp, siglongjmp.
    }

    // The first entry of the .plt section is the call to the function that resolves dynamic links
    if (plt.firstOffset > 0) {
        Function::Ptr linkerTrampoline = Function::instance(plt.section->get_mapped_actual_va(), "DYNAMIC_LINKER_TRAMPOLINE",
                                                            SgAsmFunction::FUNC_IMPORT);
        if (insertUnique(functions, linkerTrampoline, sortFunctionsByAddress))
            ++nInserted;
    }

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

bool
isObjectFile(const boost::filesystem::path &fileName) {
    if (!boost::filesystem::exists(fileName))
        return false;                                   // file doesn't exist
    MemoryMap::Ptr map = MemoryMap::instance();
    if (0 == map->insertFile(fileName.string(), 0))
        return false;                                   // file cannot be mmap'd

    uint8_t magic[4];
    if (map->at(0).limit(4).read(magic).size() != 4)
        return false;                                   // file is too short
    if (magic[0] != 0x7f || magic[1] != 'E' || magic[2] != 'L' || magic[3] != 'F')
        return false;                                   // wrong magic number

    uint8_t encoding = 0;
    if (map->at(5).limit(1).read(&encoding).size() != 1)
        return false;                                   // file is too short

    uint8_t elfTypeBuf[2];
    if (map->at(16).limit(2).read(elfTypeBuf).size() != 2)
        return false;                                   // file is too short

    unsigned elfType = 0;
    switch (encoding) {
        case 1: // little endian
            elfType = ((unsigned)elfTypeBuf[1] << 8) | elfTypeBuf[0];
            break;
        case 2: // big endian
            elfType = ((unsigned)elfTypeBuf[0] << 8) | elfTypeBuf[1];
            break;
        default:
            return false;                               // invalid data encoding
    }

    return elfType == SgAsmElfFileHeader::ET_REL;
}

bool
isStaticArchive(const boost::filesystem::path &fileName) {
    if (!boost::filesystem::exists(fileName))
        return false;                                   // file doesn't exist
    MemoryMap::Ptr map = MemoryMap::instance();
    if (0 == map->insertFile(fileName.string(), 0))
        return false;                                   // file cannot be mmap'd
    uint8_t magic[7];
    if (map->at(0).limit(7).read(magic).size() != 7)
        return false;                                   // short read
    return memcmp(magic, "!<arch>", 7) == 0;
}

bool
tryLink(const std::string &commandTemplate, const boost::filesystem::path &outputName,
        std::vector<boost::filesystem::path> inputNames, Sawyer::Message::Stream &errors,
        FixUndefinedSymbols::Boolean fixUndefinedSymbols) {
    if (commandTemplate.empty() || outputName.empty() || inputNames.empty())
        return false;

    std::string allInputs;
    BOOST_FOREACH (const boost::filesystem::path &input, inputNames)
        allInputs += (allInputs.empty()?"":" ") + StringUtility::bourneEscape(input.string());

    std::string cmd;
    bool escaped = false;
    for (size_t i=0; i<commandTemplate.size(); ++i) {
        if ('\\' == commandTemplate[i]) {
            escaped = !escaped;
            cmd += commandTemplate[i];
        } else if (escaped) {
            cmd += commandTemplate[i];
        } else if ('%' == commandTemplate[i] && i+1 < commandTemplate.size() && 'o' == commandTemplate[i+1]) {
            cmd += StringUtility::bourneEscape(outputName.string());
            ++i;                                        // skip the "o"
        } else if ('%' == commandTemplate[i] && i+1 < commandTemplate.size() && 'f' == commandTemplate[i+1]) {
            cmd += allInputs;
            ++i;                                        // skip the "f"
        } else {
            cmd += commandTemplate[i];
        }
    }

    struct Resources {
        FILE *ldOutput;
        char *line;

        Resources()
            : ldOutput(NULL), line(NULL) {}
        ~Resources() {
            if (ldOutput)
                pclose(ldOutput);
            if (line)
                free(line);
        }
    } r;

    // Run the linker the first time and parse the error messages looking for complaints of undefined symbols.
    cmd += " 2>&1";
    mlog[DEBUG] <<"running command: " <<cmd <<"\n";
    r.ldOutput = popen(cmd.c_str(), "r");
    if (!r.ldOutput)
        return false;
    std::set<std::string> undefinedSymbols;
    boost::regex undefReferenceRe("undefined reference to `(.*)'");
    size_t lineSize = 0;
    while (rose_getline(&r.line, &lineSize, r.ldOutput) > 0) {
        std::string line = r.line;
        mlog[DEBUG] <<"command output: " <<line;
        boost::smatch matched;
        if (boost::regex_search(line, matched, undefReferenceRe))
            undefinedSymbols.insert(matched.str(1));
    }
    int exitStatus = pclose(r.ldOutput);
    r.ldOutput = NULL;
    if (0 == exitStatus || undefinedSymbols.empty() || !fixUndefinedSymbols)
        return 0 == exitStatus;

    // Create a .c file that defines those symbols that the linker complained about
    Sawyer::FileSystem::TemporaryFile cFile((boost::filesystem::temp_directory_path() /
                                             boost::filesystem::unique_path()).string() + ".c");
    mlog[DEBUG] <<"defining symbols in a C file\n";
    BOOST_FOREACH (const std::string &symbol, undefinedSymbols) {
        cFile.stream() <<"void " <<symbol <<"() {}\n";
        mlog[DEBUG] <<"  void " <<symbol <<"() {}\n";
    }
    cFile.stream().close();

    // Compile the .c file to get an object file
    Sawyer::FileSystem::TemporaryFile oFile((boost::filesystem::temp_directory_path() /
                                             boost::filesystem::unique_path()).string() + ".o");
    cmd = "cc -o " + oFile.name().string() + " -c " + cFile.name().string();
    mlog[DEBUG] <<"running command: " <<cmd <<"\n";
    if (system(cmd.c_str()) != 0)
        return false;

    // Try the linking again, but with the object file as well
    inputNames.insert(inputNames.begin(), oFile.name().string());
    return tryLink(commandTemplate, outputName, inputNames, errors, FixUndefinedSymbols::NO);
}

std::vector<boost::filesystem::path>
extractStaticArchive(const boost::filesystem::path &directory, const boost::filesystem::path &archive) {
    std::vector<boost::filesystem::path> retval;

    // Create subdirectory
    boost::filesystem::path subdir = directory / archive.filename();
    boost::filesystem::create_directory(subdir);

    boost::filesystem::path absArchive = boost::filesystem::absolute(archive);

    // Run extraction command
    std::string cmd = "cd " + StringUtility::bourneEscape(subdir.string()) +
                      " && ar x " + StringUtility::bourneEscape(absArchive.string());
    SAWYER_MESG(mlog[DEBUG]) <<"extracting members of " <<archive <<" into " <<subdir <<"\n";
    SAWYER_MESG(mlog[DEBUG]) <<"extraction command: " <<cmd <<"\n";
    int status = system(cmd.c_str());
    if (status) {
        SAWYER_MESG(mlog[DEBUG]) <<"command failed\n";
        return retval;
    }

    // Get list of object files
    for (boost::filesystem::directory_iterator dentry(subdir); dentry != boost::filesystem::directory_iterator(); ++dentry) {
        if (isObjectFile(dentry->path())) {
            SAWYER_MESG(mlog[DEBUG]) <<"  extracted object file " <<dentry->path().filename() <<"\n";
            retval.push_back(dentry->path());
        } else {
            mlog[WARN] <<dentry->path().filename() <<" extracted from " <<archive <<" is not an object file\n";
        }
    }
    return retval;
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

std::vector<SgAsmElfSection*>
findSectionsByName(SgAsmInterpretation *interp, const std::string &name) {
    std::vector<SgAsmElfSection*> retval;
    if (interp!=NULL) {
        BOOST_FOREACH (SgAsmGenericHeader *fileHeader, interp->get_headers()->get_headers()) {
            std::vector<SgAsmGenericSection*> sections = fileHeader->get_sections_by_name(name);
            BOOST_FOREACH (SgAsmGenericSection *section, sections) {
                if (SgAsmElfSection *elfSection = isSgAsmElfSection(section))
                    retval.push_back(elfSection);
            }
        }
    }
    return retval;
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

#endif
