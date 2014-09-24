/*
 * Unit tests for SageBuilder::build*Expression
 */

#include "rose.h"
#include "gtest/gtest.h"

TEST(BuildIntValTest, HandlesDefaultParameter){
  ::SgIntVal* _valnullptr = NULL;
  ::SgTypeInt* _typenullptr = NULL;
  ::SgIntVal* t = SageBuilder::buildIntVal();
  ASSERT_NE(_valnullptr, t);
  EXPECT_NE(_typenullptr, isSgTypeInt(t));
  EXPECT_EQ(0, t->get_value());
}

TEST(BuildIntValTest, HandlesParameter){
  ::SgIntVal* _valnullptr = NULL;
  ::SgTypeInt* _typenullptr = NULL;
  ::SgIntVal* t = SageBuilder::buildIntVal(42);
  ASSERT_NE(_valnullptr, t);
  EXPECT_NE(_typenullptr, isSgTypeInt(t));
  EXPECT_EQ(42, t->get_value());
}
