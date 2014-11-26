/*
 * Unit tests for the SageBuilder build type functions.
 */
#include "testSupport.h"

TEST(BuildIntTypeTest, ReturnsNotNull){
  ::SgTypeInt* t = SageBuilder::buildIntType();
  EXPECT_EQ(isNull(t), false);
}

TEST(BuildIntTypeTest, IsIntTypeNotNull){
  ::SgTypeInt* t = SageBuilder::buildIntType();
  EXPECT_EQ(is<SgTypeInt>(t), true);
}
