#include <sage3basic.h>
#include <Partitioner2/ModulesElf.h>
#include <Partitioner2/ModulesPowerpc.h>
#include <Partitioner2/Partitioner.h>

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

bool
matchElfDynamicStub(const Partitioner &partitioner, const Function::Ptr &function, const AddressIntervalSet &pltAddresses) {
    ASSERT_not_null(function);

    if (function->basicBlockAddresses().size() != 1)
        return false;
    BasicBlock::Ptr bb = partitioner.basicBlockExists(function->address());
    ASSERT_not_null(bb);
    if (bb->nInstructions() != 4)
        return false;

    // addis r11, A, B
    SgAsmPowerpcInstruction *addis = isSgAsmPowerpcInstruction(bb->instructions()[0]);
    if (!addis || addis->get_kind() != powerpc_addis || addis->get_operandList()->get_operands().size() != 3)
        return false;
    SgAsmDirectRegisterExpression *rre = isSgAsmDirectRegisterExpression(addis->get_operandList()->get_operands()[0]);
    const RegisterDescriptor REG_R11 = *partitioner.instructionProvider().registerDictionary()->lookup("r11");
    if (!rre || rre->get_descriptor() != REG_R11)
        return false;
    SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(addis->get_operandList()->get_operands()[1]);
    if (!ival)
        return false;
    rose_addr_t target = ival->get_absoluteValue();
    ival = isSgAsmIntegerValueExpression(addis->get_operandList()->get_operands()[2]);
    if (!ival)
        return false;
    target += ival->get_absoluteValue() << 16;
    
    // lwz r11, [r11 + C]
    SgAsmPowerpcInstruction *lwz = isSgAsmPowerpcInstruction(bb->instructions()[1]);
    if (!lwz || lwz->get_kind() != powerpc_lwz || lwz->get_operandList()->get_operands().size() != 2)
        return false;
    rre = isSgAsmDirectRegisterExpression(lwz->get_operandList()->get_operands()[0]);
    if (!rre || rre->get_descriptor() != REG_R11)
        return false;
    SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(lwz->get_operandList()->get_operands()[1]);
    SgAsmBinaryAdd *add = mre ? isSgAsmBinaryAdd(mre->get_address()) : NULL;
    rre = add ? isSgAsmDirectRegisterExpression(add->get_lhs()) : NULL;
    if (!rre || rre->get_descriptor() != REG_R11)
        return false;
    ival = isSgAsmIntegerValueExpression(add->get_rhs());
    if (!ival)
        return false;
    target += ival->get_absoluteValue();

    // Target address must be in the ".plt" section.
    if (!pltAddresses.contains(target))
        return false;
    
    // mtspr r11, ctr
    SgAsmPowerpcInstruction *mtspr = isSgAsmPowerpcInstruction(bb->instructions()[2]);
    if (!mtspr || mtspr->get_kind() != powerpc_mtspr || mtspr->get_operandList()->get_operands().size() != 2)
        return false;
    rre = isSgAsmDirectRegisterExpression(mtspr->get_operandList()->get_operands()[0]);
    if (!rre || rre->get_descriptor() != REG_R11)
        return false;
    rre = isSgAsmDirectRegisterExpression(mtspr->get_operandList()->get_operands()[1]);
    const RegisterDescriptor REG_CTR = *partitioner.instructionProvider().registerDictionary()->lookup("ctr");
    if (!rre || rre->get_descriptor() != REG_CTR)
        return false;

    // bcctr 0x14, X, 0   (bits 0x14 must be set, other are irrelevant)
    SgAsmPowerpcInstruction *bcctr = isSgAsmPowerpcInstruction(bb->instructions()[3]);
    if (!bcctr || bcctr->get_kind() != powerpc_bcctr || bcctr->get_operandList()->get_operands().size() != 3)
        return false;
    ival = isSgAsmIntegerValueExpression(bcctr->get_operandList()->get_operands()[0]);
    if (!ival || (ival->get_absoluteValue() & 0x14) != 0x14)
        return false;
    ival = isSgAsmIntegerValueExpression(bcctr->get_operandList()->get_operands()[2]);
    if (!ival || ival->get_absoluteValue() != 0)
        return false;

    return true;
}

void
nameImportThunks(const Partitioner &partitioner, SgAsmInterpretation *interp) {
    AddressIntervalSet pltAddresses;
    std::vector<SgAsmElfSection*> pltSections = ModulesElf::findSectionsByName(interp, ".plt");
    BOOST_FOREACH (SgAsmElfSection *section, pltSections) {
        if (section->is_mapped())
            pltAddresses |= AddressInterval::baseSize(section->get_mapped_actual_va(), section->get_mapped_size());
    }
    
    BOOST_FOREACH (const Function::Ptr &function, partitioner.functions()) {
        if (function->name() == "" && matchElfDynamicStub(partitioner, function, pltAddresses))
            function->name("F" + StringUtility::addrToString(function->address()).substr(2) + "@plt");
    }
}

} // namespace
} // namespace
} // namespace
} // namespace
