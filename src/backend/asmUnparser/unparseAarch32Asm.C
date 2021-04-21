#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH32
#include <sage3basic.h>
#include <BinaryUnparserBase.h>
#include <BinaryUnparserAarch32.h>
#include <AsmUnparser.h>
#include <Partitioner2/Partitioner.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;

std::string unparseAarch32Mnemonic(SgAsmAarch32Instruction *insn) {
    ASSERT_not_null(insn);
    return insn->get_mnemonic();
}

std::string unparseAarch32Expression(SgAsmExpression *expr, const AsmUnparser::LabelMap *labels,
                                     const RegisterDictionary *registers) {
    if (!registers)
        registers = RegisterDictionary::dictionary_aarch32();
    auto unparser = Unparser::Aarch32::instance(Unparser::Aarch32Settings());
    std::ostringstream ss;
    Partitioner2::Partitioner p;
    Unparser::State state(p, registers, unparser->settings(), *unparser);
    unparser->emitOperand(ss, expr, state);
    return ss.str();
}

#endif
