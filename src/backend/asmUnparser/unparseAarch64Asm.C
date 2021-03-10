#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH64
#include <sage3basic.h>
#include <BinaryUnparserBase.h>
#include <BinaryUnparserAarch64.h>
#include <AsmUnparser.h>
#include <Partitioner2/Partitioner.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;

std::string unparseAarch64Mnemonic(SgAsmAarch64Instruction *insn) {
    ASSERT_not_null(insn);
    return insn->get_mnemonic();
}

std::string unparseAarch64Expression(SgAsmExpression *expr, const AsmUnparser::LabelMap *labels,
                                 const RegisterDictionary *registers) {
    if (!registers)
        registers = RegisterDictionary::dictionary_aarch64();
    auto unparser = Unparser::Aarch64::instance(Unparser::Aarch64Settings());
    std::ostringstream ss;
    Partitioner2::Partitioner p;
    Unparser::State state(p, registers, unparser->settings(), *unparser);
    unparser->emitOperand(ss, expr, state);
    return ss.str();
}

#endif
