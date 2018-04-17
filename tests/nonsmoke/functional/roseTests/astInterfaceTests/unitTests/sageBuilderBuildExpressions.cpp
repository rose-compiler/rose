
#include "testSupport.h"
TEST(GeneratedSageBuilderBuildExpressionTestSuite, buildSgIntVal0) {
  SgIntVal *p = SageBuilder::buildIntVal(0);
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgIntVal>(p), true);
  EXPECT_EQ(p->get_value(), 0);
}

TEST(GeneratedSageBuilderBuildExpressionTestSuite, buildSgIntVal1) {
  SgIntVal *p = SageBuilder::buildIntValHex(0);
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgIntVal>(p), true);
  EXPECT_EQ(p->get_value(), 0);
}

TEST(GeneratedSageBuilderBuildExpressionTestSuite, buildSgLongIntVal2) {
  SgLongIntVal *p = SageBuilder::buildLongIntVal(0);
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgLongIntVal>(p), true);
  EXPECT_EQ(p->get_value(), 0);
}
/*
TEST(GeneratedSageBuilderBuildExpressionTestSuite, buildSgLongIntVal3) {
  SgLongIntVal *p = SageBuilder::buildLongIntValHex(0);
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgLongIntVal>(p), true);
  EXPECT_EQ(p->get_value(), 0);
}
*/
TEST(GeneratedSageBuilderBuildExpressionTestSuite, buildSgShortVal4) {
  SgShortVal *p = SageBuilder::buildShortVal(0);
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgShortVal>(p), true);
  EXPECT_EQ(p->get_value(), 0);
}

TEST(GeneratedSageBuilderBuildExpressionTestSuite, buildSgShortVal5) {
  SgShortVal *p = SageBuilder::buildShortValHex(0);
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgShortVal>(p), true);
  EXPECT_EQ(p->get_value(), 0);
}

TEST(GeneratedSageBuilderBuildExpressionTestSuite, buildSgUnsignedShortVal6) {
  SgUnsignedShortVal *p = SageBuilder::buildUnsignedShortVal(0);
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgUnsignedShortVal>(p), true);
  EXPECT_EQ(p->get_value(), 0);
}

TEST(GeneratedSageBuilderBuildExpressionTestSuite, buildSgUnsignedShortVal7) {
  SgUnsignedShortVal *p = SageBuilder::buildUnsignedShortValHex(0);
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgUnsignedShortVal>(p), true);
  EXPECT_EQ(p->get_value(), 0);
}

TEST(GeneratedSageBuilderBuildExpressionTestSuite, buildSgUnsignedIntVal8) {
  SgUnsignedIntVal *p = SageBuilder::buildUnsignedIntVal(0);
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgUnsignedIntVal>(p), true);
  EXPECT_EQ(p->get_value(), 0);
}

TEST(GeneratedSageBuilderBuildExpressionTestSuite, buildSgUnsignedIntVal9) {
  SgUnsignedIntVal *p = SageBuilder::buildUnsignedIntValHex(0);
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgUnsignedIntVal>(p), true);
  EXPECT_EQ(p->get_value(), 0);
}

TEST(GeneratedSageBuilderBuildExpressionTestSuite, buildSgUnsignedLongVal10) {
  SgUnsignedLongVal *p = SageBuilder::buildUnsignedLongVal(0);
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgUnsignedLongVal>(p), true);
  EXPECT_EQ(p->get_value(), 0);
}

TEST(GeneratedSageBuilderBuildExpressionTestSuite, buildSgUnsignedLongVal11) {
  SgUnsignedLongVal *p = SageBuilder::buildUnsignedLongValHex(0);
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgUnsignedLongVal>(p), true);
  EXPECT_EQ(p->get_value(), 0);
}

TEST(GeneratedSageBuilderBuildExpressionTestSuite, buildSgUnsignedLongLongIntVal12) {
  SgUnsignedLongLongIntVal *p = SageBuilder::buildUnsignedLongLongIntVal(0);
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgUnsignedLongLongIntVal>(p), true);
  EXPECT_EQ(p->get_value(), 0);
}

TEST(GeneratedSageBuilderBuildExpressionTestSuite, buildSgUnsignedLongLongIntVal13) {
  SgUnsignedLongLongIntVal *p = SageBuilder::buildUnsignedLongLongIntValHex(0);
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgUnsignedLongLongIntVal>(p), true);
  EXPECT_EQ(p->get_value(), 0);
}

TEST(GeneratedSageBuilderBuildExpressionTestSuite, buildSgIntVal14) {
  SgIntVal *p = SageBuilder::buildIntVal(42);
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgIntVal>(p), true);
  EXPECT_EQ(p->get_value(), 42);
}

TEST(GeneratedSageBuilderBuildExpressionTestSuite, buildSgIntVal15) {
  SgIntVal *p = SageBuilder::buildIntValHex(42);
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgIntVal>(p), true);
  EXPECT_EQ(p->get_value(), 42);
}

TEST(GeneratedSageBuilderBuildExpressionTestSuite, buildSgLongIntVal16) {
  SgLongIntVal *p = SageBuilder::buildLongIntVal(42);
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgLongIntVal>(p), true);
  EXPECT_EQ(p->get_value(), 42);
}
/*
TEST(GeneratedSageBuilderBuildExpressionTestSuite, buildSgLongIntVal17) {
  SgLongIntVal *p = SageBuilder::buildLongIntValHex(42);
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgLongIntVal>(p), true);
  EXPECT_EQ(p->get_value(), 42);
}
*/
TEST(GeneratedSageBuilderBuildExpressionTestSuite, buildSgShortVal18) {
  SgShortVal *p = SageBuilder::buildShortVal(42);
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgShortVal>(p), true);
  EXPECT_EQ(p->get_value(), 42);
}

TEST(GeneratedSageBuilderBuildExpressionTestSuite, buildSgShortVal19) {
  SgShortVal *p = SageBuilder::buildShortValHex(42);
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgShortVal>(p), true);
  EXPECT_EQ(p->get_value(), 42);
}

TEST(GeneratedSageBuilderBuildExpressionTestSuite, buildSgUnsignedShortVal20) {
  SgUnsignedShortVal *p = SageBuilder::buildUnsignedShortVal(42);
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgUnsignedShortVal>(p), true);
  EXPECT_EQ(p->get_value(), 42);
}

TEST(GeneratedSageBuilderBuildExpressionTestSuite, buildSgUnsignedShortVal21) {
  SgUnsignedShortVal *p = SageBuilder::buildUnsignedShortValHex(42);
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgUnsignedShortVal>(p), true);
  EXPECT_EQ(p->get_value(), 42);
}

TEST(GeneratedSageBuilderBuildExpressionTestSuite, buildSgUnsignedIntVal22) {
  SgUnsignedIntVal *p = SageBuilder::buildUnsignedIntVal(42);
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgUnsignedIntVal>(p), true);
  EXPECT_EQ(p->get_value(), 42);
}

TEST(GeneratedSageBuilderBuildExpressionTestSuite, buildSgUnsignedIntVal23) {
  SgUnsignedIntVal *p = SageBuilder::buildUnsignedIntValHex(42);
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgUnsignedIntVal>(p), true);
  EXPECT_EQ(p->get_value(), 42);
}

TEST(GeneratedSageBuilderBuildExpressionTestSuite, buildSgUnsignedLongVal24) {
  SgUnsignedLongVal *p = SageBuilder::buildUnsignedLongVal(42);
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgUnsignedLongVal>(p), true);
  EXPECT_EQ(p->get_value(), 42);
}

TEST(GeneratedSageBuilderBuildExpressionTestSuite, buildSgUnsignedLongVal25) {
  SgUnsignedLongVal *p = SageBuilder::buildUnsignedLongValHex(42);
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgUnsignedLongVal>(p), true);
  EXPECT_EQ(p->get_value(), 42);
}

TEST(GeneratedSageBuilderBuildExpressionTestSuite, buildSgUnsignedLongLongIntVal26) {
  SgUnsignedLongLongIntVal *p = SageBuilder::buildUnsignedLongLongIntVal(42);
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgUnsignedLongLongIntVal>(p), true);
  EXPECT_EQ(p->get_value(), 42);
}

TEST(GeneratedSageBuilderBuildExpressionTestSuite, buildSgUnsignedLongLongIntVal27) {
  SgUnsignedLongLongIntVal *p = SageBuilder::buildUnsignedLongLongIntValHex(42);
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgUnsignedLongLongIntVal>(p), true);
  EXPECT_EQ(p->get_value(), 42);
}

