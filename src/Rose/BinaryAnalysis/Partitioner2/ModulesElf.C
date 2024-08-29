#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Partitioner2/ModulesElf.h>

#include <Rose/AST/Traversal.h>
#include <Rose/BinaryAnalysis/InstructionEnumsX86.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/Partitioner2/Utility.h>
#include <Rose/BinaryAnalysis/RelativeVirtualAddress.h>
#include <rose_getline.h>
#include <Sawyer/FileSystem.h>
#include <x86InstructionProperties.h>
#include <ROSE_UNUSED.h>

#include <SgAsmAarch32Instruction.h>
#include <SgAsmAarch64Instruction.h>
#include <SgAsmBinaryAdd.h>
#include <SgAsmBinaryPreupdate.h>
#include <SgAsmBinaryRor.h>
#include <SgAsmDirectRegisterExpression.h>
#include <SgAsmElfEHFrameEntryFD.h>
#include <SgAsmElfFileHeader.h>
#include <SgAsmElfRelocEntry.h>
#include <SgAsmElfRelocEntryList.h>
#include <SgAsmElfRelocSection.h>
#include <SgAsmElfSymbol.h>
#include <SgAsmElfSymbolList.h>
#include <SgAsmElfSymbolSection.h>
#include <SgAsmFunction.h>
#include <SgAsmGenericHeaderList.h>
#include <SgAsmGenericSectionList.h>
#include <SgAsmGenericString.h>
#include <SgAsmIntegerValueExpression.h>
#include <SgAsmInterpretation.h>
#include <SgAsmMemoryReferenceExpression.h>
#include <SgAsmType.h>
#include <SgAsmX86Instruction.h>

#include <Cxx_GrammarDowncast.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace ModulesElf {

using namespace Rose::Diagnostics;

size_t
findErrorHandlingFunctions(SgAsmElfFileHeader *elfHeader, std::vector<Function::Ptr> &functions) {
    size_t nInserted = 0;
    if (elfHeader) {
        AST::Traversal::forwardPre<SgAsmElfEHFrameEntryFD>(elfHeader, [&nInserted, &functions](SgAsmElfEHFrameEntryFD *fde) {
            Function::Ptr function = Function::instance(fde->get_begin_rva().rva(), SgAsmFunction::FUNC_EH_FRAME);
            function->reasonComment("from EhFrameEntry " + fde->get_begin_rva().toString());
            if (insertUnique(functions, function, sortFunctionsByAddress))
                ++nInserted;
        });
    }
    return nInserted;
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
        for (SgAsmGenericHeader *fileHeader: interp->get_headers()->get_headers())
            findErrorHandlingFunctions(isSgAsmElfFileHeader(fileHeader), functions);
    }
    return functions;
}

SgAsmInstruction*
PltEntryMatcher::matchNop(const Partitioner::ConstPtr &partitioner, rose_addr_t va) {
    ASSERT_not_null(partitioner);
    if (SgAsmX86Instruction *insn = isSgAsmX86Instruction(partitioner->discoverInstruction(va))) {
        if (insn->get_kind() == x86_nop)
            return insn;
    }
    return NULL;
}

SgAsmInstruction*
PltEntryMatcher::matchPush(const Partitioner::ConstPtr &partitioner, rose_addr_t va, rose_addr_t &n /*out*/) {
    ASSERT_not_null(partitioner);
    n = 0;

    // match "push C" where C is a constant
    if (SgAsmX86Instruction *insn = isSgAsmX86Instruction(partitioner->discoverInstruction(va))) {
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
PltEntryMatcher::matchDirectJump(const Partitioner::ConstPtr &partitioner, rose_addr_t va) {
    ASSERT_not_null(partitioner);
    if (SgAsmX86Instruction *insn = isSgAsmX86Instruction(partitioner->discoverInstruction(va))) {
        if (insn->nOperands() == 1 && isSgAsmIntegerValueExpression(insn->operand(0)))
            return insn;
    }
    return NULL;
}

SgAsmInstruction*
PltEntryMatcher::matchIndirectJump(const Partitioner::ConstPtr &partitioner, rose_addr_t va,
                                   rose_addr_t &indirectVa /*out*/, size_t &indirectNBytes /*out*/) {
    ASSERT_not_null(partitioner);
    indirectVa = 0;
    indirectNBytes = 0;
    const RegisterDescriptor REG_IP = partitioner->instructionProvider().instructionPointerRegister();
    if (SgAsmX86Instruction *insn = isSgAsmX86Instruction(partitioner->discoverInstruction(va))) {
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
PltEntryMatcher::matchIndirectJumpEbx(const Partitioner::ConstPtr &partitioner, rose_addr_t va,
                                      rose_addr_t &indirectOffset /*out*/, size_t &indirectNBytes /*out*/) {
    ASSERT_not_null(partitioner);
    indirectOffset = 0;
    indirectNBytes = 0;

    SgAsmX86Instruction *insn = isSgAsmX86Instruction(partitioner->discoverInstruction(va));
    if (!insn || !x86InstructionIsUnconditionalBranch(insn) || insn->nOperands() != 1)
        return NULL;

    SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(insn->operand(0));
    SgAsmBinaryAdd *add = mre ? isSgAsmBinaryAdd(mre->get_address()) : NULL;
    if (!add)
        return NULL;

    SgAsmDirectRegisterExpression *dre = NULL;
    SgAsmIntegerValueExpression *ival = NULL;
    if (isSgAsmDirectRegisterExpression(add->get_lhs()) && isSgAsmIntegerValueExpression(add->get_rhs())) {
        dre = isSgAsmDirectRegisterExpression(add->get_lhs());
        ival = isSgAsmIntegerValueExpression(add->get_rhs());
    } else if (isSgAsmIntegerValueExpression(add->get_lhs()) && isSgAsmDirectRegisterExpression(add->get_rhs())) {
        dre = isSgAsmDirectRegisterExpression(add->get_rhs());
        ival = isSgAsmIntegerValueExpression(add->get_lhs());
    }
    if (!ival || !dre)
        return NULL;

    const RegisterDescriptor EBX = partitioner->instructionProvider().registerDictionary()->find("ebx");
    if (dre->get_descriptor() != EBX)
        return NULL;

    indirectOffset = ival->get_absoluteValue();
    indirectNBytes = mre->get_type()->get_nBytes();
    return insn;
}

SgAsmInstruction*
PltEntryMatcher::matchAarch64Adrp(const Partitioner::ConstPtr &partitioner, rose_addr_t va, rose_addr_t &value /*out*/) {
    ASSERT_not_null(partitioner);
    value = 0;
#ifdef ROSE_ENABLE_ASM_AARCH64
    // match "adrp X, I" where I is an integer that will be returned through the "value" argument
    if (SgAsmAarch64Instruction *insn = isSgAsmAarch64Instruction(partitioner->discoverInstruction(va))) {
        if (insn->get_kind() == Aarch64InstructionKind::ARM64_INS_ADRP) {
            SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(insn->operand(1));
            ASSERT_not_null(ival);
            value = ival->get_absoluteValue();
            return insn;
        }
    }
#else
    ROSE_UNUSED(partitioner);
    ROSE_UNUSED(va);
#endif
    return NULL;
}

SgAsmInstruction*
PltEntryMatcher::matchAarch64Ldr(const Partitioner::ConstPtr &partitioner, rose_addr_t va, rose_addr_t &indirectVa /*in,out*/,
                                 rose_addr_t &indirectNBytes /*out*/) {
    ASSERT_not_null(partitioner);
    indirectNBytes = 0;
#ifdef ROSE_ENABLE_ASM_AARCH64
    // match "ldr R1, [ R2 + C ]" where R1 and R2 are registers and C is a constant. The initial value of R2 is
    // provided as "indirectVa", which will be incremented by C upon return.
    if (SgAsmAarch64Instruction *insn = isSgAsmAarch64Instruction(partitioner->discoverInstruction(va))) {
        if (insn->get_kind() != Aarch64InstructionKind::ARM64_INS_LDR || insn->nOperands() != 2)
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
#else
    ROSE_UNUSED(partitioner);
    ROSE_UNUSED(va);
    ROSE_UNUSED(indirectVa);
#endif
    return NULL;
}

SgAsmInstruction*
PltEntryMatcher::matchAarch64Add(const Partitioner::ConstPtr &partitioner, rose_addr_t va) {
    ASSERT_not_null(partitioner);
#ifdef ROSE_ENABLE_ASM_AARCH64
    if (SgAsmAarch64Instruction *insn = isSgAsmAarch64Instruction(partitioner->discoverInstruction(va))) {
        if (insn->get_kind() == Aarch64InstructionKind::ARM64_INS_ADD)
            return insn;
    }
#else
    ROSE_UNUSED(partitioner);
    ROSE_UNUSED(va);
#endif
    return NULL;
}

SgAsmInstruction*
PltEntryMatcher::matchAarch64Br(const Partitioner::ConstPtr &partitioner, rose_addr_t va) {
    ASSERT_not_null(partitioner);
#ifdef ROSE_ENABLE_ASM_AARCH64
    if (SgAsmAarch64Instruction *insn = isSgAsmAarch64Instruction(partitioner->discoverInstruction(va))) {
        if (insn->get_kind() == Aarch64InstructionKind::ARM64_INS_BR)
            return insn;
    }
#else
    ROSE_UNUSED(partitioner);
    ROSE_UNUSED(va);
#endif
    return NULL;
}

SgAsmInstruction*
PltEntryMatcher::matchAarch32CopyPcToIp(const Partitioner::ConstPtr &partitioner, rose_addr_t va, uint32_t &result) {
    ASSERT_not_null(partitioner);
    // Matches: "add ip, pc, ror(X,Y)" where X and Y are constants. The resulting IP value is returned via "value" argument.
#ifdef ROSE_ENABLE_ASM_AARCH32
    auto insn = isSgAsmAarch32Instruction(partitioner->discoverInstruction(va));
    if (!insn || insn->get_kind() != Aarch32InstructionKind::ARM_INS_ADD || insn->nOperands() != 3)
        return nullptr;

    const RegisterDescriptor REG_IP(aarch32_regclass_gpr, aarch32_gpr_ip, 0, 32);
    auto ip = isSgAsmDirectRegisterExpression(insn->operand(0));
    if (!ip || ip->get_descriptor() != REG_IP)
        return nullptr;

    const RegisterDescriptor REG_PC(aarch32_regclass_gpr, aarch32_gpr_pc, 0, 32);
    auto pc = isSgAsmDirectRegisterExpression(insn->operand(1));
    if (!pc || pc->get_descriptor() != REG_PC)
        return nullptr;

    if (auto ror = isSgAsmBinaryRor(insn->operand(2))) {
        auto base = isSgAsmIntegerValueExpression(ror->get_lhs());
        auto amount = isSgAsmIntegerValueExpression(ror->get_rhs());
        if (!base || !amount)
            return nullptr;
        uint32_t b = base->get_absoluteValue();
        uint32_t a = amount->get_absoluteValue();
        result = va + BitOps::rotateRight(b, a);
        return insn;
    } else if (auto ival = isSgAsmIntegerValueExpression(insn->operand(2))) {
        result = va + ival->get_absoluteValue();
        return insn;
    }
#else
    ROSE_UNUSED(partitioner);
    ROSE_UNUSED(va);
    ROSE_UNUSED(result);
#endif
    return nullptr;
}

SgAsmInstruction*
PltEntryMatcher::matchAarch32AddConstToIp(const Partitioner::ConstPtr &partitioner, rose_addr_t va, uint32_t &addend /*out*/) {
    ASSERT_not_null(partitioner);
    // Matches "add ip, ip, ADDEND" where ADDEND is a constant returned via "addend" argument.
#ifdef ROSE_ENABLE_ASM_AARCH32
    auto insn = isSgAsmAarch32Instruction(partitioner->discoverInstruction(va));
    if (!insn || insn->get_kind() != Aarch32InstructionKind::ARM_INS_ADD || insn->nOperands() != 3)
        return nullptr;

    const RegisterDescriptor REG_IP(aarch32_regclass_gpr, aarch32_gpr_ip, 0, 32);
    auto first = isSgAsmDirectRegisterExpression(insn->operand(0));
    if (!first || first->get_descriptor() != REG_IP)
        return nullptr;

    auto second = isSgAsmDirectRegisterExpression(insn->operand(1));
    if (!second || second->get_descriptor() != REG_IP)
        return nullptr;

    auto third = isSgAsmIntegerValueExpression(insn->operand(2));
    if (!third)
        return nullptr;
    addend = third->get_absoluteValue();
    return insn;
#else
    ROSE_UNUSED(partitioner);
    ROSE_UNUSED(va);
    ROSE_UNUSED(addend);
#endif
    return nullptr;
}

SgAsmInstruction*
PltEntryMatcher::matchAarch32IndirectBranch(const Partitioner::ConstPtr &partitioner, rose_addr_t va, uint32_t &addend /*out*/) {
    ASSERT_not_null(partitioner);
    // Matches "ldr pc, u32 [ip += ADDEND]" where ADDEND is a constant returned via "addend" argument.
#ifdef ROSE_ENABLE_ASM_AARCH32
    auto insn = isSgAsmAarch32Instruction(partitioner->discoverInstruction(va));
    if (!insn || insn->get_kind() != Aarch32InstructionKind::ARM_INS_LDR || insn->nOperands() != 2)
        return nullptr;

    const RegisterDescriptor REG_PC(aarch32_regclass_gpr, aarch32_gpr_pc, 0, 32);
    auto first = isSgAsmDirectRegisterExpression(insn->operand(0));
    if (!first || first->get_descriptor() != REG_PC)
        return nullptr;

    // Second argument is a memory reference expression mre(addr=postupdate(lhs=IP, rhs=add(lhs=IP, rhs=ADDEND)))
    const RegisterDescriptor REG_IP(aarch32_regclass_gpr, aarch32_gpr_ip, 0, 32);
    auto second = isSgAsmMemoryReferenceExpression(insn->operand(1));
    auto post = second ? isSgAsmBinaryPreupdate(second->get_address()) : nullptr;
    auto ip = post ? isSgAsmDirectRegisterExpression(post->get_lhs()) : nullptr;
    if (!ip || ip->get_descriptor() != REG_IP)
        return nullptr;
    auto add = isSgAsmBinaryAdd(post->get_rhs());
    auto addLhs = add ? isSgAsmDirectRegisterExpression(add->get_lhs()) : nullptr;
    if (!addLhs || addLhs->get_descriptor() != REG_IP)
        return nullptr;
    auto addRhs = add ? isSgAsmIntegerValueExpression(add->get_rhs()) : nullptr;
    if (!addRhs)
        return nullptr;
    addend = addRhs->get_absoluteValue();
    return insn;
#else
    ROSE_UNUSED(partitioner);
    ROSE_UNUSED(va);
    ROSE_UNUSED(addend);
#endif
    return nullptr;
}

bool
PltEntryMatcher::match(const Partitioner::ConstPtr &partitioner, rose_addr_t anchor) {
    ASSERT_not_null(partitioner);
    nBytesMatched_ = 0;
    gotEntryVa_ = 0;
    gotEntry_ = 0;
    gotEntryNBytes_ = 0;
    functionNumber_ = 0;

    SgAsmInstruction *insn = partitioner->discoverInstruction(anchor);
    if (!insn)
        return false;

    // Look for the PLT entry.
    if (isSgAsmX86Instruction(insn)) {
        bool found = false;
        rose_addr_t indirectVa=0;
        size_t indirectSize=0;

        if (!found) {
            // i386 entries that look like this:
            //    jmp dword [CONST]     ; where CONST is a GOT entry address
            //    push N                ; where N is a small integer, unique for each dynamically linked function
            //    jmp X                 ; where X is the address of the first PLT entry
            SgAsmInstruction *ijmp = matchIndirectJump(partitioner, anchor, indirectVa /*out*/, indirectSize /*out*/);
            SgAsmInstruction *push = ijmp ? matchPush(partitioner, ijmp->get_address() + ijmp->get_size(), functionNumber_) : NULL;
            SgAsmInstruction *djmp = push ? matchDirectJump(partitioner, push->get_address() + push->get_size()) : NULL;
            if (djmp) {
                gotEntryNBytes_ = indirectSize;
                gotEntryVa_ = indirectVa;
                nBytesMatched_ = djmp->get_address() + djmp->get_size() - anchor;
                pltEntryAlignment_ = nBytesMatched_;
                found = true;
            }
        }

        if (!found) {
            // i386 compiled with -fPIC, occuring in the ".plt.sec" section. There's also a .plt section that gets called after
            // lookup up the address in the GOT.
            //    nop                   ; 4 bytes
            //    jmp [ ebx + CONST ]   ; where CONST is the offset into the GOT
            //    nop                   ; 6 bytes
            rose_addr_t offset = 0;
            SgAsmInstruction *nop1 = matchNop(partitioner, anchor);
            SgAsmInstruction *ijmp = nop1 ? matchIndirectJumpEbx(partitioner, nop1->get_address() + nop1->get_size(),
                                                                offset /*out*/, indirectSize /*out*/) : NULL;
            SgAsmInstruction *nop2 = ijmp ? matchNop(partitioner, ijmp->get_address() + ijmp->get_size()) : NULL;
            if (nop2) {
                gotEntryNBytes_ = indirectSize;
                gotEntryVa_ = gotVa_ + offset;
                nBytesMatched_ = nop2->get_address() + nop2->get_size() - anchor;
                pltEntryAlignment_ = nBytesMatched_;
                found = true;
            }
        }

        if (!found) {
            // i386 that look like this:
            //    jmp dword [ ebx + CONST ]; where CONST is the offset into the GOT
            //    push N                ; where N is a small integer, unique for each dynamically linked function
            //    jmp X                 ; where X is the address of the first PLT entry
            rose_addr_t offset = 0;
            SgAsmInstruction *ijmp = matchIndirectJumpEbx(partitioner, anchor, offset /*out*/, indirectSize /*out*/);
            SgAsmInstruction *push = ijmp ? matchPush(partitioner, ijmp->get_address() + ijmp->get_size(), functionNumber_) : NULL;
            SgAsmInstruction *djmp = push ? matchDirectJump(partitioner, push->get_address() + push->get_size()) : NULL;
            if (djmp) {
                gotEntryNBytes_ = indirectSize;
                gotEntryVa_ = gotVa_ + offset;
                nBytesMatched_ = djmp->get_address() + djmp->get_size() - anchor;
                pltEntryAlignment_ = nBytesMatched_;
                found = true;
            }
        }

        if (!found) {
            // Amd64 entries that look like this:
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
                pltEntryAlignment_ = nBytesMatched_;
                found = true;
            }
        }

        if (!found) {
            // i386 and amd64 entries that look like this when compiled with -fPIC
            //    nop      ; 4 bytes
            //    push N   ; where N is a small integer, unique for each dynamically linked function
            //    jmp X    ; where X is the address of the first PLT entry
            //    nop      ; 5 bytes
            SgAsmInstruction *nop1 = matchNop(partitioner, anchor);
            SgAsmInstruction *push = nop1 ? matchPush(partitioner, nop1->get_address() + nop1->get_size(), functionNumber_) : NULL;
            SgAsmInstruction *djmp = push ? matchDirectJump(partitioner, push->get_address() + push->get_size()) : NULL;
            SgAsmInstruction *nop2 = djmp ? matchNop(partitioner, djmp->get_address() + djmp->get_size()) : NULL;
            if (nop2) {
                gotEntryNBytes_ = 0;                    // not present in PLT entry
                gotEntryVa_ = 0;                        // not present in PLT entry
                nBytesMatched_ = nop2->get_address() + nop2->get_size() - anchor;
                pltEntryAlignment_ = nBytesMatched_;
                found = true;
            }
        }

#ifdef ROSE_ENABLE_ASM_AARCH64
    } else if (isSgAsmAarch64Instruction(insn)) {
        if (0 == gotEntryNBytes_) {
            // Aarch64 entries look like this:
            //     adrp     x16, 0x00011000
            //     ldr      x17, u64 [x16 + 0x0000000000000ef8<3832>]
            //     add      x16, x16, 0x0000000000000ef8<3832>
            //     br       x17
            rose_addr_t indirectVa = 0;
            rose_addr_t indirectNBytes = 0;
            SgAsmInstruction *adrp = matchAarch64Adrp(partitioner, anchor, indirectVa /*out*/);
            SgAsmInstruction *ldr = adrp ? matchAarch64Ldr(partitioner, adrp->get_address() + adrp->get_size(),
                                                           indirectVa /*in,out*/, indirectNBytes /*out*/) : NULL;
            SgAsmInstruction *add = ldr ? matchAarch64Add(partitioner, ldr->get_address() + ldr->get_size()) : NULL;
            SgAsmInstruction *br = add ? matchAarch64Br(partitioner, add->get_address() + add->get_size()) : NULL;
            if (br) {
                gotEntryNBytes_ = indirectNBytes;
                gotEntryVa_ = indirectVa;
                nBytesMatched_ = br->get_address() + br->get_size() - anchor;
                pltEntryAlignment_ = nBytesMatched_;
            }
        }
#endif

#ifdef ROSE_ENABLE_ASM_AARCH32
    } else if (isSgAsmAarch32Instruction(insn)) {
        if (0 == gotEntryNBytes_) {
            // AArch32 entries look like this:
            //   add ip, pc, ror(12,0)                  // the ror(12,0) doesn't matter since it's always zero
            //   add ip, ip, 0x00011000                 // higher order bits of offset into the .got section
            //   ldr pc, u32 [ip += 0x000008b4]         // lower order bits of offset into the .got section
            uint32_t addend1=0, addend2=0, addend3=0;

            SgAsmInstruction *add1 = matchAarch32CopyPcToIp(partitioner, anchor, addend1 /*out*/);
            SgAsmInstruction *add2 = add1 ? matchAarch32AddConstToIp(partitioner, anchor+4, addend2 /*out*/) : NULL;
            SgAsmInstruction *ldr = add2 ? matchAarch32IndirectBranch(partitioner, anchor+8, addend3 /*out*/) : NULL;
            if (ldr) {
                gotEntryVa_ = addend1 + addend2 + addend3 - 4;
                gotEntryNBytes_ = 4;
                nBytesMatched_ = 12;
                pltEntryAlignment_ = 4;                 // an alignment of 12 (entry size) is apparently not needed
            }
        }
#endif

    } else if (isSgAsmM68kInstruction(insn)) {
        // m68k entries look like this (from GDB-4.1.6's bfd/elf32-m68k.c file):
        //    4e fb 01 71: jmp ([%pc, symbol@GOTPC])
        //    00 00 00 00: replaced with offset to symbol's .got entry
        //    2f 3c      : move.l #offset, -(%sp)
        //    00 00 00 00: replaced with offset into relocation table
        //    60 ff      : bra.l .plt
        //    00 00 00 00: replaced with offset to start of .plt
        //
        // In practice, the 20-byte entry looks like this on disk:
        //    This example's PLT starts at 8000081c and GOT starts at 80004000.
        //    800008a8: entry #7 for "time" function
        //        00 00 00 00
        //        00 00 37 7a    -- 800008a8 + 377a = 80004022, or two bytes before time's .got entry of 4024
        //        2f 3c
        //        00 00 00 48
        //        60 ff
        //        ff ff ff 64    --  + ffffff64 = 8000080c, or 0x10 bytes before the beginning of this PLT
        //
        // From the same source as above also describes the first PLT entry as being:
        //    2f 3b 01 70: move.l (%pc,addr), -(%sp)
        //    00 00 00 00: replaced with offset to .got+4
        //    4e fb 01 71: jmp ([%pc, addr])
        //    00 00 00 00: replaced with offset to .got+8
        //    00 00 00 00: pad out to 20 bytes
        if (partitioner->memoryMap() != NULL) {
            uint8_t entry[20];
            size_t nRead = partitioner->memoryMap()->at(anchor).limit(sizeof entry).read(entry).size();
            if (nRead == sizeof entry && entry[8] == 0x2f && entry[9] == 0x3c && entry[14] == 0x60 && entry[15] == 0xff) {
                rose_addr_t gotOffset = (entry[4] << 24) | (entry[5] << 16) | (entry[6] << 8) | entry[7];
                gotEntryVa_ = (anchor + gotOffset + 2) & 0xffffffffUL;
                gotEntryNBytes_ = 4;
                nBytesMatched_ = 20;
                pltEntryAlignment_ = nBytesMatched_;
            }
        }

    } else if (isSgAsmNullInstruction(insn)) {
        // Null ISA has no parsable PLT section

    } else {
        // FIXME[Robb P. Matzke 2014-08-23]: Architecture is not supported yet
        static bool warned = false;
        if (!warned) {
            mlog[WARN] <<"ModulesElf::pltEntryMatcher does not yet support this ISA (" <<insn->class_name() <<")\n";
            warned = true;
        }
    }

    // Read the GOT entry if it's mapped
    if (nBytesMatched_ > 0 && partitioner->memoryMap() != NULL) {
        switch (gotEntryNBytes_) {
            case 0:
                gotEntry_ = 0;
                break;
            case 1:
                gotEntry_ = partitioner->memoryMap()->readUnsigned<uint8_t>(gotEntryVa_).orElse(0);
                break;
            case 2:
                gotEntry_ = partitioner->memoryMap()->readUnsigned<uint16_t>(gotEntryVa_).orElse(0);
                break;
            case 4:
                gotEntry_ = partitioner->memoryMap()->readUnsigned<uint32_t>(gotEntryVa_).orElse(0);
                break;
            case 8:
                gotEntry_ = partitioner->memoryMap()->readUnsigned<uint64_t>(gotEntryVa_).orElse(0);
                break;
            default:
                ASSERT_not_reachable("invalid GOT entry size: " + StringUtility::numberToString(gotEntryNBytes_));
        }
    }

    return nBytesMatched_ > 0;
}

PltInfo
findPlt(const Partitioner::ConstPtr &partitioner, SgAsmGenericSection *got, SgAsmElfFileHeader *elfHeader) {
    if (elfHeader) {
        // The procedure lookup table can be in a variety of places.
        bool foundSection = false;
        for (size_t i = 0; i < 3; ++i) {
            SgAsmGenericSection *section = NULL;
            switch (i) {
                case 0:
                    // A .plt.sec is either a subregion of .plt (it excludes the leading bogus entries) or it's a completely
                    // separate table. When it's a separate table, we want to parse it instead of .plt because the .plt.sec is what
                    // has the offsets into the .got, which in turn is what the relocation symbols refer to.  This is used, for
                    // instance, when compiling with -fPIC on i386.
                    section = elfHeader->get_sectionByName(".plt.sec");
                    break;
                case 1:
                    // The normal .plt is used when there is no .plt.sec.
                    section = elfHeader->get_sectionByName(".plt");
                    break;
                case 2:
                    // Not sure what this is.
                    section = elfHeader->get_sectionByName(".plt.got");
                    break;
            }

            // Search for the first valid entry.
            if (section) {
                foundSection = true;
                SAWYER_MESG(mlog[DEBUG]) <<"searching for PLT in \""
                                         <<StringUtility::cEscape(section->get_name()->get_string()) <<"\""
                                         <<" at " <<StringUtility::addrToString(section->get_mappedActualVa())
                                         <<" + " <<StringUtility::addrToString(section->get_mappedSize())
                                         <<" = " <<StringUtility::addrToString(section->get_mappedActualVa() +
                                                                               section->get_mappedSize())
                                         <<"\n";
                for (size_t offset = 0; offset < section->get_size(); ++offset) {
                    PltEntryMatcher matcher(got ? got->get_mappedActualVa() : 0);
                    if (matcher.match(partitioner, section->get_mappedActualVa() + offset)) {
                        SAWYER_MESG(mlog[DEBUG]) <<"  matched PLT " <<matcher.nBytesMatched() <<" byte entry"
                                                 <<" at offset " <<offset <<"\n";
                        if (offset + matcher.nBytesMatched() > section->get_size()) {
                            SAWYER_MESG(mlog[DEBUG]) <<"    discarded; extends past section end\n";
                            break;
                        } else if (offset % matcher.pltEntryAlignment() != 0) {
                            SAWYER_MESG(mlog[DEBUG]) <<"    offset is not a multiple of the  "
                                                     <<matcher.pltEntryAlignment() <<" byte entry size\n";
                            continue;
                        } else {
                            SAWYER_MESG(mlog[DEBUG]) <<"   entries are each "
                                                     <<StringUtility::plural(matcher.nBytesMatched(), "bytes") <<"\n";
                            SAWYER_MESG(mlog[DEBUG]) <<"   entries are each aligned on a "
                                                     <<matcher.pltEntryAlignment() <<"-byte boundary\n";
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
            mlog[WARN] <<"found PLT section(s) but could not parse table or table is empty\n";
    }

    return PltInfo();
}

size_t
findPltFunctions(const Partitioner::Ptr &partitioner, SgAsmElfFileHeader *elfHeader, std::vector<Function::Ptr> &functions) {
    Sawyer::Message::Stream debug(mlog[DEBUG]);

    // Find important sections
    SgAsmGenericSection *got = partitioner->elfGot(elfHeader);
    PltInfo plt = findPlt(partitioner, got, elfHeader);
    if (!plt.section || !plt.section->isMapped() || !got || !got->isMapped() || 0 == plt.entrySize)
        return 0;

    // Find all relocation sections
    std::set<SgAsmElfRelocSection*> relocSections;
    for (SgAsmGenericSection *section: elfHeader->get_sections()->get_sections()) {
        if (SgAsmElfRelocSection *relocSection = isSgAsmElfRelocSection(section))
            relocSections.insert(relocSection);

    }
    if (relocSections.empty())
        return 0;

    // Look at each instruction in the .plt section. If the instruction is a computed jump to an address stored in the .got.plt
    // then we've found the beginning of a plt trampoline.
    size_t nInserted = 0;
    for (rose_addr_t pltOffset = plt.firstOffset;
         pltOffset + plt.entrySize <= plt.section->get_mappedSize();
         pltOffset += plt.entrySize) {
        PltEntryMatcher matcher(got->get_mappedActualVa());
        rose_addr_t pltEntryVa = plt.section->get_mappedActualVa() + pltOffset;
        if (!matcher.match(partitioner, pltEntryVa))
            continue;
        rose_addr_t gotVa = matcher.gotEntryVa();    // address that was read by indirect branch
        if (gotVa <  elfHeader->get_baseVa() + got->get_mappedPreferredRva() ||
            gotVa >= elfHeader->get_baseVa() + got->get_mappedPreferredRva() + got->get_mappedSize()) {
            continue;                                   // jump is not indirect through the .got.plt section
        }
        SAWYER_MESG(debug) <<"symbolizing PLT entry " <<StringUtility::addrToString(pltEntryVa)
                           <<" GOT entry " <<StringUtility::addrToString(gotVa) <<"\n";

        // Find the relocation entry whose offset is the gotVa and use that entry's symbol for the function name
        std::string name;
        for (SgAsmElfRelocSection *relocSection: relocSections) {
            SgAsmElfSymbolSection *symbolSection = isSgAsmElfSymbolSection(relocSection->get_linkedSection());
            if (SgAsmElfSymbolList *symbols = symbolSection ? symbolSection->get_symbols() : NULL) {
                for (SgAsmElfRelocEntry *rel: relocSection->get_entries()->get_entries()) {
                    if (rel->get_r_offset() == gotVa) {
                        unsigned long symbolIdx = rel->get_sym();
                        if (symbolIdx < symbols->get_symbols().size()) {
                            SgAsmElfSymbol *symbol = symbols->get_symbols()[symbolIdx];
                            name = symbol->get_name()->get_string() + "@plt";
                            SAWYER_MESG(debug) <<"  found relocation symbol " <<StringUtility::addrToString(rel->get_r_offset())
                                       <<" \"" <<StringUtility::cEscape(symbol->get_name()->get_string()) <<"\"\n";
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
        Function::Ptr linkerTrampoline = Function::instance(plt.section->get_mappedActualVa(), "DYNAMIC_LINKER_TRAMPOLINE",
                                                            SgAsmFunction::FUNC_IMPORT);
        if (insertUnique(functions, linkerTrampoline, sortFunctionsByAddress))
            ++nInserted;
    }

    return nInserted;
}

bool
isImport(const Partitioner::ConstPtr &partitioner, const Function::Ptr &function) {
    ASSERT_not_null(partitioner);
    if (!function)
        return false;
    if (0 == (function->reasons() & SgAsmFunction::FUNC_IMPORT))
        return false;
    PltEntryMatcher matcher(partitioner->elfGotVa().orElse(0));
    return matcher.match(partitioner, function->address());
}

bool
isLinkedImport(const Partitioner::ConstPtr &partitioner, const Function::Ptr &function) {
    ASSERT_not_null(partitioner);

    // Is function an import?
    if (!function)
        return false;
    if (0 == (function->reasons() & SgAsmFunction::FUNC_IMPORT))
        return false;
    PltEntryMatcher matcher(partitioner->elfGotVa().orElse(0));
    if (!matcher.match(partitioner, function->address()))
        return false;
    if (matcher.gotEntryVa() == 0)
        return false;
    SgAsmInstruction *insn = partitioner->instructionProvider()[function->address()];
    if (!insn)
        return false;

    // Is the import linked?
    rose_addr_t fallthrough = insn->get_address() + insn->get_size();
    return matcher.gotEntry() != 0 && matcher.gotEntry() != fallthrough;
}

bool
isUnlinkedImport(const Partitioner::ConstPtr &partitioner, const Function::Ptr &function) {
    ASSERT_not_null(partitioner);

    // Is function an import?
    if (!function)
        return false;
    if (0 == (function->reasons() & SgAsmFunction::FUNC_IMPORT))
        return false;
    PltEntryMatcher matcher(partitioner->elfGotVa().orElse(0));
    if (!matcher.match(partitioner, function->address()))
        return false;
    if (matcher.gotEntryVa() == 0)
        return false;
    SgAsmInstruction *insn = partitioner->instructionProvider()[function->address()];
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
    for (const boost::filesystem::path &input: inputNames)
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
    for (const std::string &symbol: undefinedSymbols) {
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
findPltFunctions(const Partitioner::Ptr &partitioner, SgAsmElfFileHeader *elfHeader) {
    std::vector<Function::Ptr> functions;
    findPltFunctions(partitioner, elfHeader, functions);
    return functions;
}

std::vector<Function::Ptr>
findPltFunctions(const Partitioner::Ptr &partitioner, SgAsmInterpretation *interp) {
    std::vector<Function::Ptr> functions;
    if (interp!=NULL) {
        for (SgAsmGenericHeader *fileHeader: interp->get_headers()->get_headers())
            findPltFunctions(partitioner, isSgAsmElfFileHeader(fileHeader), functions);
    }
    return functions;
}

std::vector<SgAsmElfSection*>
findSectionsByName(SgAsmInterpretation *interp, const std::string &name) {
    std::vector<SgAsmElfSection*> retval;
    if (interp!=NULL) {
        for (SgAsmGenericHeader *fileHeader: interp->get_headers()->get_headers()) {
            std::vector<SgAsmGenericSection*> sections = fileHeader->get_sectionsByName(name);
            for (SgAsmGenericSection *section: sections) {
                if (SgAsmElfSection *elfSection = isSgAsmElfSection(section))
                    retval.push_back(elfSection);
            }
        }
    }
    return retval;
}

void
buildMayReturnLists(const Partitioner::Ptr &partitioner) {
    ASSERT_not_null(partitioner);

    // Most of these were obtained by searching for "noreturn" in all the header files on Debian Squeeze.
    // Please keep this list alphabetical (ignoring leading underscores).
    Configuration &c = partitioner->configuration();

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
