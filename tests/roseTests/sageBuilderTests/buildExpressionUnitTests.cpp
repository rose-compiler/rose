/*
 * Unit tests for SageBuilder::build*Expression
 */

#include "rose.h"
#include "gtest/gtest.h"

TEST(BuildIntValTest, HandlesDefaultParameter){
  ::SgIntVal* _nullptr = NULL;
  ::SgIntVal* t = SageBuilder::buildIntVal();
  ASSERT_NE(_nullptr, t);
  EXPECT_NE(_nullptr, isSgTypeInt(t));
  EXPECT_EQ(0, t->get_value());
}

TEST(BuildIntValTest, HandlesParameter){
  ::SgIntVal* _nullptr = NULL;
  ::SgIntVal* t = SageBuilder::buildIntVal(42);
  ASSERT_NE(_nullptr, t);
  EXPECT_NE(_nullptr, isSgTypeInt(t));
  EXPECT_EQ(42, t->get_value());
}
