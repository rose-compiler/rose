/*
 * Unit tests for SageBuilder::build*Expression
 */

#include "testSupport.h"

TEST(BuildIntValTest, HandlesDefaultParameter){
  ::SgIntVal* t = SageBuilder::buildIntVal();
  ASSERT_EQ(isNull(t), false);
  EXPECT_EQ(is<SgIntVal>(t), true);
  EXPECT_EQ(0, t->get_value());
}

TEST(BuildIntValTest, HandlesParameter){
  ::SgIntVal* t = SageBuilder::buildIntVal(42);
  ASSERT_EQ(isNull(t), false);
  EXPECT_EQ(is<SgIntVal>(t), true);
  EXPECT_EQ(42, t->get_value());
}
