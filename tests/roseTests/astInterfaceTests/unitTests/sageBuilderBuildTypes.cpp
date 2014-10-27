
#include "testSupport.h"
TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeInt0) {
  SgTypeInt *p = SageBuilder::buildIntType();
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeInt>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeLong1) {
  SgTypeLong *p = SageBuilder::buildLongType();
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeLong>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeLongLong2) {
  SgTypeLongLong *p = SageBuilder::buildLongLongType();
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeLongLong>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeShort3) {
  SgTypeShort *p = SageBuilder::buildShortType();
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeShort>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeSigned128bitInteger4) {
  SgTypeSigned128bitInteger *p = SageBuilder::buildSigned128bitIntegerType();
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeSigned128bitInteger>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeSignedInt5) {
  SgTypeSignedInt *p = SageBuilder::buildSignedIntType();
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeSignedInt>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeSignedLong6) {
  SgTypeSignedLong *p = SageBuilder::buildSignedLongType();
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeSignedLong>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeSignedLongLong7) {
  SgTypeSignedLongLong *p = SageBuilder::buildSignedLongLongType();
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeSignedLongLong>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeSignedShort8) {
  SgTypeSignedShort *p = SageBuilder::buildSignedShortType();
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeSignedShort>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeUnsigned128bitInteger9) {
  SgTypeUnsigned128bitInteger *p = SageBuilder::buildUnsigned128bitIntegerType();
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeUnsigned128bitInteger>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeUnsignedInt10) {
  SgTypeUnsignedInt *p = SageBuilder::buildUnsignedIntType();
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeUnsignedInt>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeUnsignedLong11) {
  SgTypeUnsignedLong *p = SageBuilder::buildUnsignedLongType();
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeUnsignedLong>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeUnsignedLongLong12) {
  SgTypeUnsignedLongLong *p = SageBuilder::buildUnsignedLongLongType();
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeUnsignedLongLong>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeUnsignedShort13) {
  SgTypeUnsignedShort *p = SageBuilder::buildUnsignedShortType();
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeUnsignedShort>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeChar14) {
  SgTypeChar *p = SageBuilder::buildCharType();
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeChar>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeSignedChar15) {
  SgTypeSignedChar *p = SageBuilder::buildSignedCharType();
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeSignedChar>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeUnsignedChar16) {
  SgTypeUnsignedChar *p = SageBuilder::buildUnsignedCharType();
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeUnsignedChar>(p), true);
}
/*
TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeString17){  SgTypeString *p = SageBuilder::buildStringType();  ASSERT_EQ(isNull(p), false);  EXPECT_EQ(is<SgTypeString>(p), true);}
*/
TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeFloat18) {
  SgTypeFloat *p = SageBuilder::buildFloatType();
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeFloat>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeDouble19) {
  SgTypeDouble *p = SageBuilder::buildDoubleType();
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeDouble>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeLongDouble20) {
  SgTypeLongDouble *p = SageBuilder::buildLongDoubleType();
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeLongDouble>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgPointerType21) {
  SgPointerType *p = SageBuilder::buildPointerType(SageBuilder::buildIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgPointerType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgReferenceType22) {
  SgReferenceType *p = SageBuilder::buildReferenceType(SageBuilder::buildIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgReferenceType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgRvalueReferenceType23) {
  SgRvalueReferenceType *p = SageBuilder::buildRvalueReferenceType(SageBuilder::buildIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgRvalueReferenceType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType24) {
  SgModifierType *p = SageBuilder::buildModifierType(SageBuilder::buildIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType25) {
  SgModifierType *p = SageBuilder::buildConstType(SageBuilder::buildIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType26) {
  SgModifierType *p = SageBuilder::buildVolatileType(SageBuilder::buildIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType27){
  EXPECT_DEATH( SageBuilder::buildRestrictType(SageBuilder::buildIntType()), "");
//  SgModifierType *p = SageBuilder::buildRestrictType(SageBuilder::buildIntType());
//  ASSERT_EQ(isNull(p), false);
//  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType28) {
  SgModifierType *p = SageBuilder::buildUpcStrictType(SageBuilder::buildIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType29) {
  SgModifierType *p = SageBuilder::buildUpcRelaxedType(SageBuilder::buildIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType30) {
  SgModifierType *p = SageBuilder::buildUpcSharedType(SageBuilder::buildIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType31) {
  SgModifierType *p = SageBuilder::buildUpcBlockIndefiniteType(SageBuilder::buildIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType32) {
  SgModifierType *p = SageBuilder::buildUpcBlockStarType(SageBuilder::buildIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeComplex33) {
  SgTypeComplex *p = SageBuilder::buildComplexType(SageBuilder::buildIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeComplex>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeImaginary34) {
  SgTypeImaginary *p = SageBuilder::buildImaginaryType(SageBuilder::buildIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeImaginary>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgPointerType35) {
  SgPointerType *p = SageBuilder::buildPointerType(SageBuilder::buildLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgPointerType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgReferenceType36) {
  SgReferenceType *p = SageBuilder::buildReferenceType(SageBuilder::buildLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgReferenceType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgRvalueReferenceType37) {
  SgRvalueReferenceType *p = SageBuilder::buildRvalueReferenceType(SageBuilder::buildLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgRvalueReferenceType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType38) {
  SgModifierType *p = SageBuilder::buildModifierType(SageBuilder::buildLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType39) {
  SgModifierType *p = SageBuilder::buildConstType(SageBuilder::buildLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType40) {
  SgModifierType *p = SageBuilder::buildVolatileType(SageBuilder::buildLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType41){
  EXPECT_DEATH(SageBuilder::buildRestrictType(SageBuilder::buildLongType()), "");
//  SgModifierType *p = SageBuilder::buildRestrictType(SageBuilder::buildLongType());
//  ASSERT_EQ(isNull(p), false);
//  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType42) {
  SgModifierType *p = SageBuilder::buildUpcStrictType(SageBuilder::buildLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType43) {
  SgModifierType *p = SageBuilder::buildUpcRelaxedType(SageBuilder::buildLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType44) {
  SgModifierType *p = SageBuilder::buildUpcSharedType(SageBuilder::buildLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType45) {
  SgModifierType *p = SageBuilder::buildUpcBlockIndefiniteType(SageBuilder::buildLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType46) {
  SgModifierType *p = SageBuilder::buildUpcBlockStarType(SageBuilder::buildLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeComplex47) {
  SgTypeComplex *p = SageBuilder::buildComplexType(SageBuilder::buildLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeComplex>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeImaginary48) {
  SgTypeImaginary *p = SageBuilder::buildImaginaryType(SageBuilder::buildLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeImaginary>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgPointerType49) {
  SgPointerType *p = SageBuilder::buildPointerType(SageBuilder::buildLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgPointerType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgReferenceType50) {
  SgReferenceType *p = SageBuilder::buildReferenceType(SageBuilder::buildLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgReferenceType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgRvalueReferenceType51) {
  SgRvalueReferenceType *p = SageBuilder::buildRvalueReferenceType(SageBuilder::buildLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgRvalueReferenceType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType52) {
  SgModifierType *p = SageBuilder::buildModifierType(SageBuilder::buildLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType53) {
  SgModifierType *p = SageBuilder::buildConstType(SageBuilder::buildLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType54) {
  SgModifierType *p = SageBuilder::buildVolatileType(SageBuilder::buildLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType55){
  EXPECT_DEATH(SageBuilder::buildRestrictType(SageBuilder::buildLongLongType()), "");
//  SgModifierType *p = SageBuilder::buildRestrictType(SageBuilder::buildLongLongType());
//  ASSERT_EQ(isNull(p), false);
//  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType56) {
  SgModifierType *p = SageBuilder::buildUpcStrictType(SageBuilder::buildLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType57) {
  SgModifierType *p = SageBuilder::buildUpcRelaxedType(SageBuilder::buildLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType58) {
  SgModifierType *p = SageBuilder::buildUpcSharedType(SageBuilder::buildLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType59) {
  SgModifierType *p = SageBuilder::buildUpcBlockIndefiniteType(SageBuilder::buildLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType60) {
  SgModifierType *p = SageBuilder::buildUpcBlockStarType(SageBuilder::buildLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeComplex61) {
  SgTypeComplex *p = SageBuilder::buildComplexType(SageBuilder::buildLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeComplex>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeImaginary62) {
  SgTypeImaginary *p = SageBuilder::buildImaginaryType(SageBuilder::buildLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeImaginary>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgPointerType63) {
  SgPointerType *p = SageBuilder::buildPointerType(SageBuilder::buildShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgPointerType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgReferenceType64) {
  SgReferenceType *p = SageBuilder::buildReferenceType(SageBuilder::buildShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgReferenceType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgRvalueReferenceType65) {
  SgRvalueReferenceType *p = SageBuilder::buildRvalueReferenceType(SageBuilder::buildShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgRvalueReferenceType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType66) {
  SgModifierType *p = SageBuilder::buildModifierType(SageBuilder::buildShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType67) {
  SgModifierType *p = SageBuilder::buildConstType(SageBuilder::buildShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType68) {
  SgModifierType *p = SageBuilder::buildVolatileType(SageBuilder::buildShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType69){
  EXPECT_DEATH(SageBuilder::buildRestrictType(SageBuilder::buildShortType()), "");
//  SgModifierType *p = SageBuilder::buildRestrictType(SageBuilder::buildShortType());
//  ASSERT_EQ(isNull(p), false);
//  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType70) {
  SgModifierType *p = SageBuilder::buildUpcStrictType(SageBuilder::buildShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType71) {
  SgModifierType *p = SageBuilder::buildUpcRelaxedType(SageBuilder::buildShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType72) {
  SgModifierType *p = SageBuilder::buildUpcSharedType(SageBuilder::buildShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType73) {
  SgModifierType *p = SageBuilder::buildUpcBlockIndefiniteType(SageBuilder::buildShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType74) {
  SgModifierType *p = SageBuilder::buildUpcBlockStarType(SageBuilder::buildShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeComplex75) {
  SgTypeComplex *p = SageBuilder::buildComplexType(SageBuilder::buildShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeComplex>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeImaginary76) {
  SgTypeImaginary *p = SageBuilder::buildImaginaryType(SageBuilder::buildShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeImaginary>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgPointerType77) {
  SgPointerType *p = SageBuilder::buildPointerType(SageBuilder::buildSigned128bitIntegerType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgPointerType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgReferenceType78) {
  SgReferenceType *p = SageBuilder::buildReferenceType(SageBuilder::buildSigned128bitIntegerType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgReferenceType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgRvalueReferenceType79) {
  SgRvalueReferenceType *p = SageBuilder::buildRvalueReferenceType(
                               SageBuilder::buildSigned128bitIntegerType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgRvalueReferenceType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType80) {
  SgModifierType *p = SageBuilder::buildModifierType(SageBuilder::buildSigned128bitIntegerType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType81) {
  SgModifierType *p = SageBuilder::buildConstType(SageBuilder::buildSigned128bitIntegerType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType82) {
  SgModifierType *p = SageBuilder::buildVolatileType(SageBuilder::buildSigned128bitIntegerType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType83){
  EXPECT_DEATH(SageBuilder::buildRestrictType(SageBuilder::buildSigned128bitIntegerType()), "");
//  SgModifierType *p = SageBuilder::buildRestrictType(SageBuilder::buildSigned128bitIntegerType());
//  ASSERT_EQ(isNull(p), false);
//  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType84) {
  SgModifierType *p = SageBuilder::buildUpcStrictType(SageBuilder::buildSigned128bitIntegerType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType85) {
  SgModifierType *p = SageBuilder::buildUpcRelaxedType(SageBuilder::buildSigned128bitIntegerType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType86) {
  SgModifierType *p = SageBuilder::buildUpcSharedType(SageBuilder::buildSigned128bitIntegerType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType87) {
  SgModifierType *p = SageBuilder::buildUpcBlockIndefiniteType(
                        SageBuilder::buildSigned128bitIntegerType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType88) {
  SgModifierType *p = SageBuilder::buildUpcBlockStarType(SageBuilder::buildSigned128bitIntegerType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeComplex89) {
  SgTypeComplex *p = SageBuilder::buildComplexType(SageBuilder::buildSigned128bitIntegerType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeComplex>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeImaginary90) {
  SgTypeImaginary *p = SageBuilder::buildImaginaryType(SageBuilder::buildSigned128bitIntegerType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeImaginary>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgPointerType91) {
  SgPointerType *p = SageBuilder::buildPointerType(SageBuilder::buildSignedIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgPointerType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgReferenceType92) {
  SgReferenceType *p = SageBuilder::buildReferenceType(SageBuilder::buildSignedIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgReferenceType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgRvalueReferenceType93) {
  SgRvalueReferenceType *p = SageBuilder::buildRvalueReferenceType(SageBuilder::buildSignedIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgRvalueReferenceType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType94) {
  SgModifierType *p = SageBuilder::buildModifierType(SageBuilder::buildSignedIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType95) {
  SgModifierType *p = SageBuilder::buildConstType(SageBuilder::buildSignedIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType96) {
  SgModifierType *p = SageBuilder::buildVolatileType(SageBuilder::buildSignedIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType97){
  EXPECT_DEATH(SageBuilder::buildRestrictType(SageBuilder::buildSignedIntType()), "");
//  SgModifierType *p = SageBuilder::buildRestrictType(SageBuilder::buildSignedIntType());
//  ASSERT_EQ(isNull(p), false);
//  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType98) {
  SgModifierType *p = SageBuilder::buildUpcStrictType(SageBuilder::buildSignedIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType99) {
  SgModifierType *p = SageBuilder::buildUpcRelaxedType(SageBuilder::buildSignedIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType100) {
  SgModifierType *p = SageBuilder::buildUpcSharedType(SageBuilder::buildSignedIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType101) {
  SgModifierType *p = SageBuilder::buildUpcBlockIndefiniteType(SageBuilder::buildSignedIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType102) {
  SgModifierType *p = SageBuilder::buildUpcBlockStarType(SageBuilder::buildSignedIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeComplex103) {
  SgTypeComplex *p = SageBuilder::buildComplexType(SageBuilder::buildSignedIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeComplex>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeImaginary104) {
  SgTypeImaginary *p = SageBuilder::buildImaginaryType(SageBuilder::buildSignedIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeImaginary>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgPointerType105) {
  SgPointerType *p = SageBuilder::buildPointerType(SageBuilder::buildSignedLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgPointerType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgReferenceType106) {
  SgReferenceType *p = SageBuilder::buildReferenceType(SageBuilder::buildSignedLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgReferenceType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgRvalueReferenceType107) {
  SgRvalueReferenceType *p = SageBuilder::buildRvalueReferenceType(
                               SageBuilder::buildSignedLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgRvalueReferenceType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType108) {
  SgModifierType *p = SageBuilder::buildModifierType(SageBuilder::buildSignedLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType109) {
  SgModifierType *p = SageBuilder::buildConstType(SageBuilder::buildSignedLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType110) {
  SgModifierType *p = SageBuilder::buildVolatileType(SageBuilder::buildSignedLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType111){
  EXPECT_DEATH(SageBuilder::buildRestrictType(SageBuilder::buildSignedLongType()), "");
//  SgModifierType *p = SageBuilder::buildRestrictType(SageBuilder::buildSignedLongType());
//  ASSERT_EQ(isNull(p), false);
//  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType112) {
  SgModifierType *p = SageBuilder::buildUpcStrictType(SageBuilder::buildSignedLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType113) {
  SgModifierType *p = SageBuilder::buildUpcRelaxedType(SageBuilder::buildSignedLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType114) {
  SgModifierType *p = SageBuilder::buildUpcSharedType(SageBuilder::buildSignedLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType115) {
  SgModifierType *p = SageBuilder::buildUpcBlockIndefiniteType(SageBuilder::buildSignedLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType116) {
  SgModifierType *p = SageBuilder::buildUpcBlockStarType(SageBuilder::buildSignedLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeComplex117) {
  SgTypeComplex *p = SageBuilder::buildComplexType(SageBuilder::buildSignedLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeComplex>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeImaginary118) {
  SgTypeImaginary *p = SageBuilder::buildImaginaryType(SageBuilder::buildSignedLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeImaginary>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgPointerType119) {
  SgPointerType *p = SageBuilder::buildPointerType(SageBuilder::buildSignedLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgPointerType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgReferenceType120) {
  SgReferenceType *p = SageBuilder::buildReferenceType(SageBuilder::buildSignedLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgReferenceType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgRvalueReferenceType121) {
  SgRvalueReferenceType *p = SageBuilder::buildRvalueReferenceType(
                               SageBuilder::buildSignedLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgRvalueReferenceType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType122) {
  SgModifierType *p = SageBuilder::buildModifierType(SageBuilder::buildSignedLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType123) {
  SgModifierType *p = SageBuilder::buildConstType(SageBuilder::buildSignedLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType124) {
  SgModifierType *p = SageBuilder::buildVolatileType(SageBuilder::buildSignedLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType125){
  EXPECT_DEATH(SageBuilder::buildRestrictType(SageBuilder::buildSignedLongLongType()),"");
//  SgModifierType *p = SageBuilder::buildRestrictType(SageBuilder::buildSignedLongLongType());
//  ASSERT_EQ(isNull(p), false);
//  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType126) {
  SgModifierType *p = SageBuilder::buildUpcStrictType(SageBuilder::buildSignedLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType127) {
  SgModifierType *p = SageBuilder::buildUpcRelaxedType(SageBuilder::buildSignedLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType128) {
  SgModifierType *p = SageBuilder::buildUpcSharedType(SageBuilder::buildSignedLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType129) {
  SgModifierType *p = SageBuilder::buildUpcBlockIndefiniteType(
                        SageBuilder::buildSignedLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType130) {
  SgModifierType *p = SageBuilder::buildUpcBlockStarType(SageBuilder::buildSignedLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeComplex131) {
  SgTypeComplex *p = SageBuilder::buildComplexType(SageBuilder::buildSignedLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeComplex>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeImaginary132) {
  SgTypeImaginary *p = SageBuilder::buildImaginaryType(SageBuilder::buildSignedLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeImaginary>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgPointerType133) {
  SgPointerType *p = SageBuilder::buildPointerType(SageBuilder::buildSignedShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgPointerType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgReferenceType134) {
  SgReferenceType *p = SageBuilder::buildReferenceType(SageBuilder::buildSignedShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgReferenceType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgRvalueReferenceType135) {
  SgRvalueReferenceType *p = SageBuilder::buildRvalueReferenceType(
                               SageBuilder::buildSignedShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgRvalueReferenceType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType136) {
  SgModifierType *p = SageBuilder::buildModifierType(SageBuilder::buildSignedShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType137) {
  SgModifierType *p = SageBuilder::buildConstType(SageBuilder::buildSignedShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType138) {
  SgModifierType *p = SageBuilder::buildVolatileType(SageBuilder::buildSignedShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType139){
  EXPECT_DEATH(SageBuilder::buildRestrictType(SageBuilder::buildSignedShortType()), "");
//  SgModifierType *p = SageBuilder::buildRestrictType(SageBuilder::buildSignedShortType());
//  ASSERT_EQ(isNull(p), false);
//  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType140) {
  SgModifierType *p = SageBuilder::buildUpcStrictType(SageBuilder::buildSignedShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType141) {
  SgModifierType *p = SageBuilder::buildUpcRelaxedType(SageBuilder::buildSignedShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType142) {
  SgModifierType *p = SageBuilder::buildUpcSharedType(SageBuilder::buildSignedShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType143) {
  SgModifierType *p = SageBuilder::buildUpcBlockIndefiniteType(SageBuilder::buildSignedShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType144) {
  SgModifierType *p = SageBuilder::buildUpcBlockStarType(SageBuilder::buildSignedShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeComplex145) {
  SgTypeComplex *p = SageBuilder::buildComplexType(SageBuilder::buildSignedShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeComplex>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeImaginary146) {
  SgTypeImaginary *p = SageBuilder::buildImaginaryType(SageBuilder::buildSignedShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeImaginary>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgPointerType147) {
  SgPointerType *p = SageBuilder::buildPointerType(SageBuilder::buildUnsigned128bitIntegerType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgPointerType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgReferenceType148) {
  SgReferenceType *p = SageBuilder::buildReferenceType(SageBuilder::buildUnsigned128bitIntegerType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgReferenceType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgRvalueReferenceType149) {
  SgRvalueReferenceType *p = SageBuilder::buildRvalueReferenceType(
                               SageBuilder::buildUnsigned128bitIntegerType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgRvalueReferenceType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType150) {
  SgModifierType *p = SageBuilder::buildModifierType(SageBuilder::buildUnsigned128bitIntegerType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType151) {
  SgModifierType *p = SageBuilder::buildConstType(SageBuilder::buildUnsigned128bitIntegerType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType152) {
  SgModifierType *p = SageBuilder::buildVolatileType(SageBuilder::buildUnsigned128bitIntegerType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType153){
  EXPECT_DEATH(SageBuilder::buildRestrictType(SageBuilder::buildUnsigned128bitIntegerType()), "");
//  SgModifierType *p = SageBuilder::buildRestrictType(SageBuilder::buildUnsigned128bitIntegerType());
//  ASSERT_EQ(isNull(p), false);
//  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType154) {
  SgModifierType *p = SageBuilder::buildUpcStrictType(SageBuilder::buildUnsigned128bitIntegerType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType155) {
  SgModifierType *p = SageBuilder::buildUpcRelaxedType(SageBuilder::buildUnsigned128bitIntegerType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType156) {
  SgModifierType *p = SageBuilder::buildUpcSharedType(SageBuilder::buildUnsigned128bitIntegerType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType157) {
  SgModifierType *p = SageBuilder::buildUpcBlockIndefiniteType(
                        SageBuilder::buildUnsigned128bitIntegerType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType158) {
  SgModifierType *p = SageBuilder::buildUpcBlockStarType(
                        SageBuilder::buildUnsigned128bitIntegerType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeComplex159) {
  SgTypeComplex *p = SageBuilder::buildComplexType(SageBuilder::buildUnsigned128bitIntegerType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeComplex>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeImaginary160) {
  SgTypeImaginary *p = SageBuilder::buildImaginaryType(SageBuilder::buildUnsigned128bitIntegerType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeImaginary>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgPointerType161) {
  SgPointerType *p = SageBuilder::buildPointerType(SageBuilder::buildUnsignedIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgPointerType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgReferenceType162) {
  SgReferenceType *p = SageBuilder::buildReferenceType(SageBuilder::buildUnsignedIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgReferenceType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgRvalueReferenceType163) {
  SgRvalueReferenceType *p = SageBuilder::buildRvalueReferenceType(
                               SageBuilder::buildUnsignedIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgRvalueReferenceType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType164) {
  SgModifierType *p = SageBuilder::buildModifierType(SageBuilder::buildUnsignedIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType165) {
  SgModifierType *p = SageBuilder::buildConstType(SageBuilder::buildUnsignedIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType166) {
  SgModifierType *p = SageBuilder::buildVolatileType(SageBuilder::buildUnsignedIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType167){
  EXPECT_DEATH(SageBuilder::buildRestrictType(SageBuilder::buildUnsignedIntType()), "");
//  SgModifierType *p = SageBuilder::buildRestrictType(SageBuilder::buildUnsignedIntType());
//  ASSERT_EQ(isNull(p), false);
//  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType168) {
  SgModifierType *p = SageBuilder::buildUpcStrictType(SageBuilder::buildUnsignedIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType169) {
  SgModifierType *p = SageBuilder::buildUpcRelaxedType(SageBuilder::buildUnsignedIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType170) {
  SgModifierType *p = SageBuilder::buildUpcSharedType(SageBuilder::buildUnsignedIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType171) {
  SgModifierType *p = SageBuilder::buildUpcBlockIndefiniteType(SageBuilder::buildUnsignedIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType172) {
  SgModifierType *p = SageBuilder::buildUpcBlockStarType(SageBuilder::buildUnsignedIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeComplex173) {
  SgTypeComplex *p = SageBuilder::buildComplexType(SageBuilder::buildUnsignedIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeComplex>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeImaginary174) {
  SgTypeImaginary *p = SageBuilder::buildImaginaryType(SageBuilder::buildUnsignedIntType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeImaginary>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgPointerType175) {
  SgPointerType *p = SageBuilder::buildPointerType(SageBuilder::buildUnsignedLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgPointerType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgReferenceType176) {
  SgReferenceType *p = SageBuilder::buildReferenceType(SageBuilder::buildUnsignedLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgReferenceType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgRvalueReferenceType177) {
  SgRvalueReferenceType *p = SageBuilder::buildRvalueReferenceType(
                               SageBuilder::buildUnsignedLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgRvalueReferenceType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType178) {
  SgModifierType *p = SageBuilder::buildModifierType(SageBuilder::buildUnsignedLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType179) {
  SgModifierType *p = SageBuilder::buildConstType(SageBuilder::buildUnsignedLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType180) {
  SgModifierType *p = SageBuilder::buildVolatileType(SageBuilder::buildUnsignedLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType181){
  EXPECT_DEATH(SageBuilder::buildRestrictType(SageBuilder::buildUnsignedLongType()), "");
//  SgModifierType *p = SageBuilder::buildRestrictType(SageBuilder::buildUnsignedLongType());
//  ASSERT_EQ(isNull(p), false);
//  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType182) {
  SgModifierType *p = SageBuilder::buildUpcStrictType(SageBuilder::buildUnsignedLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType183) {
  SgModifierType *p = SageBuilder::buildUpcRelaxedType(SageBuilder::buildUnsignedLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType184) {
  SgModifierType *p = SageBuilder::buildUpcSharedType(SageBuilder::buildUnsignedLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType185) {
  SgModifierType *p = SageBuilder::buildUpcBlockIndefiniteType(SageBuilder::buildUnsignedLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType186) {
  SgModifierType *p = SageBuilder::buildUpcBlockStarType(SageBuilder::buildUnsignedLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeComplex187) {
  SgTypeComplex *p = SageBuilder::buildComplexType(SageBuilder::buildUnsignedLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeComplex>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeImaginary188) {
  SgTypeImaginary *p = SageBuilder::buildImaginaryType(SageBuilder::buildUnsignedLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeImaginary>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgPointerType189) {
  SgPointerType *p = SageBuilder::buildPointerType(SageBuilder::buildUnsignedLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgPointerType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgReferenceType190) {
  SgReferenceType *p = SageBuilder::buildReferenceType(SageBuilder::buildUnsignedLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgReferenceType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgRvalueReferenceType191) {
  SgRvalueReferenceType *p = SageBuilder::buildRvalueReferenceType(
                               SageBuilder::buildUnsignedLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgRvalueReferenceType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType192) {
  SgModifierType *p = SageBuilder::buildModifierType(SageBuilder::buildUnsignedLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType193) {
  SgModifierType *p = SageBuilder::buildConstType(SageBuilder::buildUnsignedLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType194) {
  SgModifierType *p = SageBuilder::buildVolatileType(SageBuilder::buildUnsignedLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType195){
 EXPECT_DEATH(SageBuilder::buildRestrictType(SageBuilder::buildUnsignedLongLongType()), "");
//  SgModifierType *p = SageBuilder::buildRestrictType(SageBuilder::buildUnsignedLongLongType());
//  ASSERT_EQ(isNull(p), false);
//  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType196) {
  SgModifierType *p = SageBuilder::buildUpcStrictType(SageBuilder::buildUnsignedLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType197) {
  SgModifierType *p = SageBuilder::buildUpcRelaxedType(SageBuilder::buildUnsignedLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType198) {
  SgModifierType *p = SageBuilder::buildUpcSharedType(SageBuilder::buildUnsignedLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType199) {
  SgModifierType *p = SageBuilder::buildUpcBlockIndefiniteType(
                        SageBuilder::buildUnsignedLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType200) {
  SgModifierType *p = SageBuilder::buildUpcBlockStarType(SageBuilder::buildUnsignedLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeComplex201) {
  SgTypeComplex *p = SageBuilder::buildComplexType(SageBuilder::buildUnsignedLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeComplex>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeImaginary202) {
  SgTypeImaginary *p = SageBuilder::buildImaginaryType(SageBuilder::buildUnsignedLongLongType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeImaginary>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgPointerType203) {
  SgPointerType *p = SageBuilder::buildPointerType(SageBuilder::buildUnsignedShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgPointerType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgReferenceType204) {
  SgReferenceType *p = SageBuilder::buildReferenceType(SageBuilder::buildUnsignedShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgReferenceType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgRvalueReferenceType205) {
  SgRvalueReferenceType *p = SageBuilder::buildRvalueReferenceType(
                               SageBuilder::buildUnsignedShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgRvalueReferenceType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType206) {
  SgModifierType *p = SageBuilder::buildModifierType(SageBuilder::buildUnsignedShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType207) {
  SgModifierType *p = SageBuilder::buildConstType(SageBuilder::buildUnsignedShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType208) {
  SgModifierType *p = SageBuilder::buildVolatileType(SageBuilder::buildUnsignedShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType209){
  EXPECT_DEATH(SageBuilder::buildRestrictType(SageBuilder::buildUnsignedShortType()), "");
//  SgModifierType *p = SageBuilder::buildRestrictType(SageBuilder::buildUnsignedShortType());
//  ASSERT_EQ(isNull(p), false);
//  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType210) {
  SgModifierType *p = SageBuilder::buildUpcStrictType(SageBuilder::buildUnsignedShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType211) {
  SgModifierType *p = SageBuilder::buildUpcRelaxedType(SageBuilder::buildUnsignedShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType212) {
  SgModifierType *p = SageBuilder::buildUpcSharedType(SageBuilder::buildUnsignedShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType213) {
  SgModifierType *p = SageBuilder::buildUpcBlockIndefiniteType(SageBuilder::buildUnsignedShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgModifierType214) {
  SgModifierType *p = SageBuilder::buildUpcBlockStarType(SageBuilder::buildUnsignedShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgModifierType>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeComplex215) {
  SgTypeComplex *p = SageBuilder::buildComplexType(SageBuilder::buildUnsignedShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeComplex>(p), true);
}

TEST(GeneratedSageBuilderBuildTypeTestSuite, buildSgTypeImaginary216) {
  SgTypeImaginary *p = SageBuilder::buildImaginaryType(SageBuilder::buildUnsignedShortType());
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgTypeImaginary>(p), true);
}

