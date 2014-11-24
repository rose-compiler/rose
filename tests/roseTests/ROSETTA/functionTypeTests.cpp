
#include "testSupport.h"

TEST(FunctionParameterTypeList, DefaultConstructible){
  SgFunctionParameterTypeList *p = new SgFunctionParameterTypeList();
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgFunctionParameterTypeList>(p), true);
  EXPECT_EQ(p->get_arguments().size(), 0);
}

TEST(FunctionParameterTypeList, AddOneArgumentIncreasesSizeByOne){
  SgFunctionParameterTypeList *p = new SgFunctionParameterTypeList();
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgFunctionParameterTypeList>(p), true);
  p->append_argument(SageBuilder::buildIntType());
  EXPECT_EQ(p->get_arguments().size(), 1);
}

TEST(FunctionParameterTypeList, AddTwoArgumentIncreasesSizeByTwo){
  SgFunctionParameterTypeList *p = new SgFunctionParameterTypeList();
  p->append_argument(SageBuilder::buildIntType());
  p->append_argument(SageBuilder::buildIntType());
  EXPECT_EQ(p->get_arguments().size(), 2);
}

TEST(FunctionParameterTypeList, TwoFunctionParameterTypeListsAreUnequalInSize){
  SgFunctionParameterTypeList *p = new SgFunctionParameterTypeList();
  p->append_argument(SageBuilder::buildIntType());
  p->append_argument(SageBuilder::buildIntType());
  EXPECT_EQ(p->get_arguments().size(), 2);
  SgFunctionParameterTypeList *p2 = new SgFunctionParameterTypeList();
  p2->append_argument(SageBuilder::buildIntType());
  EXPECT_EQ(p2->get_arguments().size(), 1);
  EXPECT_NE(p->get_arguments().size(), p2->get_arguments().size());
}
