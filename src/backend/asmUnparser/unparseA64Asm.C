#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_A64
#include <sage3basic.h>
#include <BinaryUnparserBase.h>
#include <BinaryUnparserArm.h>
#include <AsmUnparser.h>
#include <Partitioner2/Partitioner.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;

std::string unparseA64Mnemonic(SgAsmA64Instruction *insn) {
    ASSERT_not_null(insn);
    return insn->get_mnemonic();
}

std::string unparseA64Expression(SgAsmExpression *expr, const AsmUnparser::LabelMap *labels,
                                 const RegisterDictionary *registers) {
    auto unparser = Unparser::Arm::instance(Unparser::ArmSettings());
    std::ostringstream ss;
    Unparser::State state(Partitioner2::Partitioner(), registers, unparser->settings(), *unparser);
    unparser->emitOperand(ss, expr, state);
    return ss.str();
}

#endif
