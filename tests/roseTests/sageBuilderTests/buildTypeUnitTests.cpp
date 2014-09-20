/*
 * Unit tests for the SageBuilder build type functions.
 */
#include "rose.h"

#include "gtest/gtest.h"

TEST(BuildIntTypeTest, ReturnsNotNull){
  ::SgTypeInt* _nullptr = NULL;
  ::SgTypeInt* t = SageBuilder::buildIntType();
  EXPECT_NE(_nullptr,t);
}

TEST(BuildIntTypeTest, IsIntTypeNotNull){
  ::SgTypeInt* _nullptr = NULL;
  ::SgTypeInt* t = SageBuilder::buildIntType();
  EXPECT_NE(_nullptr, isSgTypeInt(t));
}
