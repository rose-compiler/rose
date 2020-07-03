#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include <sage3basic.h>
#include <DisassemblerPowerpc.h>

#include <Partitioner2/ModulesElf.h>
#include <Partitioner2/ModulesPowerpc.h>
#include <Partitioner2/Partitioner.h>
#include <Sawyer/Message.h>

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace ModulesPowerpc {

std::vector<Function::Ptr>
MatchStwuPrologue::functions() const {
    std::vector<Function::Ptr> retval;
    if (function_)
        retval.push_back(function_);
    return retval;
}

bool
MatchStwuPrologue::match(const Partitioner &partitioner, rose_addr_t anchor) {
    if (partitioner.instructionExists(anchor))
        return false;                                   // already in the CFG/AUM
    SgAsmPowerpcInstruction *insn = isSgAsmPowerpcInstruction(partitioner.discoverInstruction(anchor));
    if (!insn)
        return false;                                   // cannot decode an instruction at this address

    // Look for "stwu r1, u32 [r1 + DELTA]" where DELTA is a negative value
    if (insn->get_kind() == powerpc_stwu && insn->nOperands() == 2) {
        // First operand must be "r1"
        SgAsmDirectRegisterExpression *reg = isSgAsmDirectRegisterExpression(insn->operand(0));
        RegisterDescriptor r1 = reg ? reg->get_descriptor() : RegisterDescriptor();
        if (r1.majorNumber() != powerpc_regclass_gpr || r1.minorNumber() != 1 ||
            r1.offset() != 0 || (r1.nBits() != 32 && r1.nBits() != 64))
            return false;                               // not the usual frame pointer register, r1

        // Second operand must be either:
        //    'u32 [r1 + DELTA]' where DELTA is negative, or
        //    'u64 [r1 + DELTA]' where DELTA is negative
        SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(insn->operand(1));
        SgAsmBinaryAdd *addr = mre ? isSgAsmBinaryAdd(mre->get_address()) : NULL;
        SgAsmDirectRegisterExpression *dre = addr ? isSgAsmDirectRegisterExpression(addr->get_lhs()) : NULL;
        RegisterDescriptor r2 = dre ? dre->get_descriptor() : RegisterDescriptor();
        if (r2 != r1)
            return false;
        int64_t delta = addr->get_rhs()->asSigned().orElse(0);
        if (delta >= 0)
            return false;

        function_ = Function::instance(anchor, SgAsmFunction::FUNC_PATTERN);
        function_->reasonComment("matched STWU r1, [r1 + <delta>] s.t. delta < 0");
        return true;
    }

    return false;
}

Sawyer::Optional<rose_addr_t>
matchElfDynamicStub(const Partitioner &partitioner, const Function::Ptr &function, const AddressIntervalSet &pltAddresses) {
    ASSERT_not_null(function);

    if (function->basicBlockAddresses().size() != 1)
        return Sawyer::Nothing();
    BasicBlock::Ptr bb = partitioner.basicBlockExists(function->address());
    ASSERT_not_null(bb);
    if (bb->nInstructions() != 4)
        return Sawyer::Nothing();

    // addis r11, A, B
    SgAsmPowerpcInstruction *addis = isSgAsmPowerpcInstruction(bb->instructions()[0]);
    if (!addis || addis->get_kind() != powerpc_addis || addis->get_operandList()->get_operands().size() != 3)
        return Sawyer::Nothing();
    SgAsmDirectRegisterExpression *rre = isSgAsmDirectRegisterExpression(addis->get_operandList()->get_operands()[0]);
    const RegisterDescriptor REG_R11 = partitioner.instructionProvider().registerDictionary()->findOrThrow("r11");
    if (!rre || rre->get_descriptor() != REG_R11)
        return Sawyer::Nothing();
    SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(addis->get_operandList()->get_operands()[1]);
    if (!ival)
        return Sawyer::Nothing();
    rose_addr_t target = ival->get_absoluteValue();
    ival = isSgAsmIntegerValueExpression(addis->get_operandList()->get_operands()[2]);
    if (!ival)
        return Sawyer::Nothing();
    target += ival->get_absoluteValue() << 16;
    
    // lwz r11, [r11 + C]
    SgAsmPowerpcInstruction *lwz = isSgAsmPowerpcInstruction(bb->instructions()[1]);
    if (!lwz || lwz->get_kind() != powerpc_lwz || lwz->get_operandList()->get_operands().size() != 2)
        return Sawyer::Nothing();
    rre = isSgAsmDirectRegisterExpression(lwz->get_operandList()->get_operands()[0]);
    if (!rre || rre->get_descriptor() != REG_R11)
        return Sawyer::Nothing();
    SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(lwz->get_operandList()->get_operands()[1]);
    SgAsmBinaryAdd *add = mre ? isSgAsmBinaryAdd(mre->get_address()) : NULL;
    rre = add ? isSgAsmDirectRegisterExpression(add->get_lhs()) : NULL;
    if (!rre || rre->get_descriptor() != REG_R11)
        return Sawyer::Nothing();
    ival = isSgAsmIntegerValueExpression(add->get_rhs());
    if (!ival)
        return Sawyer::Nothing();
    target += ival->get_absoluteValue();

    // Target address must be in the ".plt" section.
    if (!pltAddresses.contains(target))
        return Sawyer::Nothing();
    
    // mtspr ctr, r11
    SgAsmPowerpcInstruction *mtspr = isSgAsmPowerpcInstruction(bb->instructions()[2]);
    if (!mtspr || mtspr->get_kind() != powerpc_mtspr || mtspr->get_operandList()->get_operands().size() != 2)
        return Sawyer::Nothing();
    rre = isSgAsmDirectRegisterExpression(mtspr->get_operandList()->get_operands()[0]);
    const RegisterDescriptor REG_CTR = partitioner.instructionProvider().registerDictionary()->findOrThrow("ctr");
    if (!rre || rre->get_descriptor() != REG_CTR)
        return Sawyer::Nothing();
    rre = isSgAsmDirectRegisterExpression(mtspr->get_operandList()->get_operands()[1]);
    if (!rre || rre->get_descriptor() != REG_R11)
        return Sawyer::Nothing();

    // bcctr 0x14, X, 0   (bits 0x14 must be set, other are irrelevant)
    SgAsmPowerpcInstruction *bcctr = isSgAsmPowerpcInstruction(bb->instructions()[3]);
    if (!bcctr || bcctr->get_kind() != powerpc_bcctr || bcctr->get_operandList()->get_operands().size() != 3)
        return Sawyer::Nothing();
    ival = isSgAsmIntegerValueExpression(bcctr->get_operandList()->get_operands()[0]);
    if (!ival || (ival->get_absoluteValue() & 0x14) != 0x14)
        return Sawyer::Nothing();
    ival = isSgAsmIntegerValueExpression(bcctr->get_operandList()->get_operands()[2]);
    if (!ival || ival->get_absoluteValue() != 0)
        return Sawyer::Nothing();

    return target;
}

void
nameImportThunks(const Partitioner &partitioner, SgAsmInterpretation *interp) {
    if (!dynamic_cast<DisassemblerPowerpc*>(partitioner.instructionProvider().disassembler()))
        return;

    // Find the locations of all the PLTs
    AddressIntervalSet pltAddresses;
    std::vector<SgAsmElfSection*> pltSections = ModulesElf::findSectionsByName(interp, ".plt");
    BOOST_FOREACH (SgAsmElfSection *section, pltSections) {
        if (section->is_mapped())
            pltAddresses |= AddressInterval::baseSize(section->get_mapped_actual_va(), section->get_mapped_size());
    }

    // Look at all the functions that don't have names yet. If the function looks like a PowerPC ELF dynamic function
    // stub that goes through a particular PLT slot, then add "@plt" to the base name. If the PLT slot has a corresponding
    // entry in the .rela.plt section, then use that section's symbol table to get the base name of the function.
    BOOST_FOREACH (const Function::Ptr &function, partitioner.functions()) {
        if (!function->name().empty())
            continue;

        Sawyer::Optional<rose_addr_t> pltSlotVa = matchElfDynamicStub(partitioner, function, pltAddresses);
        if (!pltSlotVa)
            continue;
        
        std::vector<SgAsmElfSection*> relocSections = ModulesElf::findSectionsByName(interp, ".rela.plt");
        BOOST_FOREACH (SgAsmGenericSection *genericRelocSection, relocSections) {
            SgAsmElfRelocSection *relocs = isSgAsmElfRelocSection(genericRelocSection);
            if (!relocs || !relocs->get_entries())
                break;
            SgAsmElfSymbolSection *symtab = isSgAsmElfSymbolSection(relocs->get_linked_section());
            if (!symtab)
                break;

            BOOST_FOREACH (SgAsmElfRelocEntry *reloc, relocs->get_entries()->get_entries()) {
                if (reloc->get_r_offset() == *pltSlotVa) {
                    size_t symbolIdx = reloc->get_sym();
                    if (symbolIdx < symtab->get_symbols()->get_symbols().size()) {
                        SgAsmGenericSymbol *symbol = symtab->get_symbols()->get_symbols()[symbolIdx];
                        function->name(symbol->get_name()->get_string() + "@plt");
                        break;
                    }
                }
            }

            if (!function->name().empty())
                break;
        }

        if (function->name().empty())
            function->name("F" + StringUtility::addrToString(function->address()).substr(2) + "@plt");
    }
}

boost::logic::tribool
isFunctionReturn(const Partitioner &partitioner, const BasicBlock::Ptr &bb) {
    ASSERT_not_null(bb);
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    BasicBlockSemantics sem = bb->semantics();
    BaseSemantics::StatePtr state = sem.finalState();
    if (!state)
        return boost::logic::indeterminate;

    ASSERT_not_null(sem.dispatcher);
    ASSERT_not_null(sem.operators);
    SAWYER_MESG(debug) <<"  block has semantic information\n";
    const RegisterDescriptor REG_IP = partitioner.instructionProvider().instructionPointerRegister();
    const RegisterDescriptor REG_LR = partitioner.instructionProvider().callReturnRegister();
    ASSERT_forbid(REG_IP.isEmpty());
    ASSERT_forbid(REG_LR.isEmpty());

    BaseSemantics::SValuePtr mask = sem.operators->number_(REG_IP.nBits(), 0xfffffffc);
    BaseSemantics::SValuePtr retAddr = sem.operators->and_(sem.operators->peekRegister(REG_LR), mask);
    BaseSemantics::SValuePtr ip = sem.operators->and_(sem.operators->peekRegister(REG_IP), mask);
    BaseSemantics::SValuePtr isEqual = sem.operators->isEqual(retAddr, ip);
    bool isReturn = isEqual->is_number() ? (isEqual->get_number() != 0) : false;

    if (debug) {
        debug <<"    retAddr      = " <<*retAddr <<"\n";
        debug <<"    ip           = " <<*ip <<"\n";
        debug <<"    retAddr==ip? = " <<*isEqual <<"\n";
        debug <<"    returning " <<(isReturn ? "true" : "false") <<"\n";
        //debug <<"    state:" <<*state; // produces lots of output!
    }

    return isReturn;
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
