// Test that serialization works.
#include "conditionalDisable.h"
#ifdef ROSE_BINARY_TEST_DISABLED
#include <iostream>
int main() { std::cout <<"disabled for " <<ROSE_BINARY_TEST_DISABLED <<"\n"; return 1; }
#else

#include <rose.h>

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB

#include <AstSerialization.h>                           // needed for Rose::saveAst and Rose::restoreAst
#include <SageBuilderAsm.h>                             // used to build some IR nodes for testing

// binary archives should work equally well, but we use text here to aid in debugging.
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

using namespace Rose::BinaryAnalysis;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Testing support
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define not_null(IN) ASSERT_always_not_null(IN)
#define equal(IN, OUT, FIELD) ASSERT_always_require(IN->FIELD == OUT->FIELD)
#define check(COND) ASSERT_always_require(COND)

namespace Builder = Rose::SageBuilderAsm;

// Serialize and then unserialize an AST
template<class T>
static void
serunser(SgNode *nodeOut, T *&retval) {
    std::ostringstream oss;
    boost::archive::text_oarchive out(oss);
    Rose::saveAst(out, nodeOut);

    std::istringstream iss(oss.str());
    boost::archive::text_iarchive in(iss);
    SgNode *nodeIn = Rose::restoreAst(in);

    ASSERT_always_require((nodeOut==NULL && nodeIn==NULL) || (nodeOut!=NULL && nodeIn!=NULL));
    retval = dynamic_cast<T*>(nodeIn);
    ASSERT_always_require((nodeOut==NULL && retval==NULL) || (nodeOut!=NULL && retval!=NULL));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void
testSgAsmIntegerType() {
    std::cerr <<"SgAsmIntegerType\n";
    SgAsmIntegerType *out = Builder::buildTypeU32();
    SgAsmIntegerType *in = NULL;
    serunser(out, in);
    equal(in, out, get_isSigned());
    equal(in, out, get_minorOrder());
    equal(in, out, get_majorOrder());
    equal(in, out, get_majorNBytes());
    equal(in, out, get_nBits());
}

static void
testSgAsmVectorType() {
    std::cerr <<"SgAsmVectorType\n";
    SgAsmIntegerType *baseType = Builder::buildTypeU32(); // tested above
    SgAsmVectorType *out = Builder::buildTypeVector(2, baseType);
    SgAsmVectorType *in = NULL;
    serunser(out, in);
    equal(in, out, get_nElmts());
    equal(in, out, get_elmtType()->class_name());
}

static void
testSgAsmFloatType() {
    std::cerr <<"SgAsmFloatType\n";
    SgAsmFloatType *out = Builder::buildIeee754Binary64();
    SgAsmFloatType *in = NULL;
    serunser(out, in);
    equal(in, out, significandBits());
    equal(in, out, signBit());
    equal(in, out, exponentBits());
    equal(in, out, exponentBias());
    equal(in, out, flags());
    equal(in, out, get_minorOrder());
    equal(in, out, get_majorOrder());
    equal(in, out, get_majorNBytes());
    equal(in, out, get_nBits());
}

static void
testSgAsmIntegerValueExpression() {
    std::cerr <<"SgAsmIntegerValueExpression\n";
    SgAsmIntegerValueExpression *out = Builder::buildValueInteger(1234, Builder::buildTypeU32());
    SgAsmIntegerValueExpression *in = NULL;
    serunser(out, in);
    check(in->get_baseNode() == NULL);                  // FIXME[Robb P Matzke 2016-11-02]
    check(in->get_bitVector().compare(out->get_bitVector()) == 0);
    check(in->get_unfolded_expression_tree() == NULL);  // FIXME[Robb P Matzke 2016-11-02]
    equal(in, out, get_bit_offset());
    equal(in, out, get_bit_size());
    check(in->get_symbol() == NULL);                    // FIXME[Robb P Matzke 2016-11-02]
    check(in->get_type() != NULL);
    equal(in, out, get_type()->class_name());
}

static void
testSgAsmFloatValueExpression() {
    std::cerr <<"SgAsmFloatValueExpression\n";
    SgAsmFloatValueExpression *out = Builder::buildValueFloat(3.14, Builder::buildIeee754Binary64());
    SgAsmFloatValueExpression *in = NULL;
    serunser(out, in);
    equal(in, out, get_nativeValue());
    check(in->get_bitVector().compare(out->get_bitVector()) == 0);
    check(in->get_unfolded_expression_tree() == NULL);  // FIXME[Robb P Matzke 2016-11-02]
    equal(in, out, get_bit_offset());
    equal(in, out, get_bit_size());
    check(in->get_symbol() == NULL);                    // FIXME[Robb P Matzke 2016-11-02]
    check(in->get_type() != NULL);
    equal(in, out, get_type()->class_name());
}

static void
testSgAsmRiscOperation() {
    // FIXME[Robb P Matzke 2016-11-02]: test one with arguments
    std::cerr <<"SgAsmRiscOperation\n";
    SgAsmRiscOperation *out = new SgAsmRiscOperation(SgAsmRiscOperation::OP_undefined);
    SgAsmRiscOperation *in = NULL;
    serunser(out, in);
    equal(in, out, get_riscOperator());
}

static void
testSgAsmDirectRegisterExpression() {
    std::cerr <<"SgAsmDirectRegisterExpression\n";
    SgAsmDirectRegisterExpression *out = new SgAsmDirectRegisterExpression(RegisterDescriptor(1, 2, 3, 4));
    out->set_adjustment(2);
    SgAsmDirectRegisterExpression *in = NULL;
    serunser(out, in);
    equal(in, out, get_descriptor());
    equal(in, out, get_adjustment());
}

static void
testSgAsmIndirectRegisterExpression() {
    std::cerr <<"SgAsmIndirectRegisterExpression\n";
    SgAsmIndirectRegisterExpression *out = new SgAsmIndirectRegisterExpression(RegisterDescriptor(1, 2, 3, 4),
                                                                               RegisterDescriptor(2, 3, 4, 5),
                                                                               RegisterDescriptor(3, 4, 5, 6),
                                                                               7, 8);
    SgAsmIndirectRegisterExpression *in = NULL;
    serunser(out, in);
    equal(in, out, get_stride());
    equal(in, out, get_offset());
    equal(in, out, get_index());
    equal(in, out, get_modulus());
    equal(in, out, get_descriptor());
}

static void
testSgAsmUnaryMinus() {
    std::cerr <<"SgAsmUnaryMinus\n";
    SgAsmUnaryMinus *out = new SgAsmUnaryMinus(Builder::buildValueU32(1234));
    //check(out->get_operand()->get_parent() == out);
    SgAsmUnaryMinus *in = NULL;
    serunser(out, in);
    check(in->get_operand() != NULL);
    equal(in, out, get_operand()->class_name());
    //check(in->get_operand()->get_parent() == in);
}

static void
testSgAsmBinaryAdd() {
    std::cerr <<"SgAsmBinaryAdd\n";
    SgAsmBinaryAdd *out = Builder::buildAddExpression(Builder::buildValueU32(1234), Builder::buildValueU32(5678));
    check(out->get_lhs()->get_parent() == out);
    check(out->get_rhs()->get_parent() == out);
    SgAsmBinaryAdd *in = NULL;
    serunser(out, in);
    check(in->get_lhs() != NULL);
    equal(in, out, get_lhs()->class_name());
    check(in->get_rhs() != NULL);
    equal(in, out, get_rhs()->class_name());
}

static void
testSgAsmOperandList() {
    std::cerr <<"SgAsmOperandList\n";
    SgAsmOperandList *out = new SgAsmOperandList;
    out->append_operand(Builder::buildValueU32(1234));
    out->append_operand(Builder::buildValueU32(5678));
    SgAsmOperandList *in = NULL;
    serunser(out, in);
    equal(in, out, get_operands().size());
    equal(in, out, get_operands()[0]->class_name());
    equal(in, out, get_operands()[1]->class_name());
}

static void
testSgAsmRegisterNames() {
    std::cerr <<"SgAsmRegisterNames\n";
    SgAsmRegisterNames *out = new SgAsmRegisterNames;
    out->get_registers().push_back(new SgAsmDirectRegisterExpression(RegisterDescriptor(1, 2, 3, 4)));
    out->get_registers().push_back(new SgAsmDirectRegisterExpression(RegisterDescriptor(5, 6, 7, 8)));
    SgAsmRegisterNames *in = NULL;
    serunser(out, in);
    equal(in, out, get_registers().size());
    equal(in, out, get_registers()[0]->class_name());
    equal(in, out, get_registers()[0]->get_descriptor());
    equal(in, out, get_registers()[1]->class_name());
    equal(in, out, get_registers()[1]->get_descriptor());
}

static void
testSgAsmMemoryReferenceExpression() {
    std::cerr <<"SgAsmMemoryReferenceExpression\n";
    SgAsmMemoryReferenceExpression *out =
        Builder::buildMemoryReferenceExpression(Builder::buildValueU32(1234),
                                                new SgAsmRegisterReferenceExpression(RegisterDescriptor(1, 2, 3, 4)),
                                                Builder::buildTypeU32());
    SgAsmMemoryReferenceExpression *in = NULL;
    serunser(out, in);
    check(in->get_address() != NULL);
    equal(in, out, get_address()->class_name());
    check(in->get_segment() != NULL);
    equal(in, out, get_segment()->class_name());
}

static void
testSgAsmControlFlagsExpression() {
    std::cerr <<"SgAsmControlFlagsExpression\n";
    SgAsmControlFlagsExpression *out = new SgAsmControlFlagsExpression;
    out->set_bit_flags(10);
    SgAsmControlFlagsExpression *in = NULL;
    serunser(out, in);
    equal(in, out, get_bit_flags());
}

static void
testSgAsmCommonSubExpression() {
    std::cerr <<"SgAsmCommonSubExpression\n";
    SgAsmCommonSubExpression *out = new SgAsmCommonSubExpression;
    out->set_subexpression(Builder::buildValueU32(1234));
    SgAsmCommonSubExpression *in = NULL;
    serunser(out, in);
    check(in->get_subexpression() != NULL);
    equal(in, out, get_subexpression()->class_name());
}

static void
testSgAsmExprListExp() {
    std::cerr <<"SgAsmExprListExp\n";
    SgAsmExprListExp *out = new SgAsmExprListExp;
    out->get_expressions().push_back(Builder::buildValueU32(1234));
    out->get_expressions().push_back(Builder::buildValueU32(5678));
    SgAsmExprListExp *in = NULL;
    serunser(out, in);
    equal(in, out, get_expressions().size());
    equal(in, out, get_expressions()[0]->class_name());
    equal(in, out, get_expressions()[1]->class_name());
}

int
main() {
    ROSE_INITIALIZE;

    testSgAsmIntegerType();
    testSgAsmVectorType();
    testSgAsmFloatType();
    testSgAsmIntegerValueExpression();
    testSgAsmFloatValueExpression();
    testSgAsmRiscOperation();
    testSgAsmDirectRegisterExpression();
    testSgAsmIndirectRegisterExpression();
    testSgAsmUnaryMinus();
    testSgAsmBinaryAdd();
    testSgAsmOperandList();
    testSgAsmRegisterNames();
    testSgAsmMemoryReferenceExpression();
    testSgAsmControlFlagsExpression();
    testSgAsmCommonSubExpression();
    testSgAsmExprListExp();
}

#else

int
main() {
    std::cerr <<"Disabled because libboost_serialization is was not enabled at ROSE configure time\n";
}

#endif
#endif
