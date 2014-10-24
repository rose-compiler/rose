#include "testSupport.h"

TEST_F(CleanFunctionTypeTableFixture, GetsConstructed){
  SgFunctionTypeTable *ftt = SgNode::get_globalFunctionTypeTable();
  ASSERT_EQ(isNull(ftt), false);
}

TEST(GlobalTypeTableTest, EmptyTypeTableCanPrintItself){
  SgFunctionTypeTable *ftt = SgNode::get_globalFunctionTypeTable();
  ftt->print_functypetable();
  SUCCEED();
}

TEST_F(CleanFunctionTypeTableFixture, ConstructFunctionTypeIncreasesSizeByOne){
  SgFunctionTypeTable *ftt = SgNode::get_globalFunctionTypeTable();
  ASSERT_EQ(isNull(ftt), false);
  EXPECT_EQ(ftt->get_function_type_table()->size(), 0);
  SgFunctionParameterTypeList *params = new SgFunctionParameterTypeList();
  params->append_argument(SageBuilder::buildIntType());
  SgFunctionType *ft = SageBuilder::buildFunctionType(SageBuilder::buildVoidType(), params);
  EXPECT_EQ(ftt->get_function_type_table()->size(), 1);
}

TEST_F(CleanFunctionTypeTableFixture, ConstructFunctionTypeTwiceIncreasesSizeByOne){
  SgFunctionTypeTable *ftt = SgNode::get_globalFunctionTypeTable();
  ASSERT_EQ(isNull(ftt), false);
  EXPECT_EQ(ftt->get_function_type_table()->size(), 0);
  SgFunctionParameterTypeList *params = new SgFunctionParameterTypeList();
  params->append_argument(SageBuilder::buildIntType());
  SgFunctionType *ft = SageBuilder::buildFunctionType(SageBuilder::buildVoidType(), params);
  EXPECT_EQ(ftt->get_function_type_table()->size(), 1);
  SgFunctionType *ft2 = SageBuilder::buildFunctionType(SageBuilder::buildVoidType(), params);
  EXPECT_EQ(ftt->get_function_type_table()->size(), 1);
  EXPECT_EQ(ft, ft2);
}

TEST_F(CleanFunctionTypeTableFixture, ConstructFunctionTypeWithTwoArgumentsTwiceIncreasesSizeByOne){
  SgFunctionTypeTable *ftt = SgNode::get_globalFunctionTypeTable();
  ASSERT_EQ(isNull(ftt), false);
  EXPECT_EQ(ftt->get_function_type_table()->size(), 0);
  SgFunctionParameterTypeList *params = new SgFunctionParameterTypeList();
  params->append_argument(SageBuilder::buildIntType());
  params->append_argument(SageBuilder::buildIntType());
  SgFunctionType *ft = SageBuilder::buildFunctionType(SageBuilder::buildVoidType(), params);
  EXPECT_EQ(ftt->get_function_type_table()->size(), 1);
  SgFunctionType *ft2 = SageBuilder::buildFunctionType(SageBuilder::buildVoidType(), params);
  EXPECT_EQ(ftt->get_function_type_table()->size(), 1);
  EXPECT_EQ(ft, ft2);
}

TEST_F(CleanFunctionTypeTableFixture, ConstructTwoDifferentFunctionsIncreasesSizeByTwo){
  SgFunctionTypeTable *ftt = SgNode::get_globalFunctionTypeTable();
  ASSERT_EQ(isNull(ftt), false);
  EXPECT_EQ(ftt->get_function_type_table()->size(), 0);
  SgFunctionParameterTypeList *params = new SgFunctionParameterTypeList();
  params->append_argument(SageBuilder::buildIntType());
  params->append_argument(SageBuilder::buildIntType());
  SgFunctionType *ft = SageBuilder::buildFunctionType(SageBuilder::buildVoidType(), params);
  EXPECT_EQ(ftt->get_function_type_table()->size(), 1);
  SgFunctionParameterTypeList *params2 = new SgFunctionParameterTypeList();
  params->append_argument(SageBuilder::buildIntType());
  SgFunctionType *ft2 = SageBuilder::buildFunctionType(SageBuilder::buildVoidType(), params2);
  EXPECT_EQ(ftt->get_function_type_table()->size(), 2);
}

TEST(SymbolTableTest, DefaultConstructorWorks){
  SgSymbolTable *p = new SgSymbolTable();
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(is<SgSymbolTable>(p), true);
}

TEST(SymbolTableTest, DefaultConstructorLeadsToEmptySymbolSet){
  SgSymbolTable *p = new SgSymbolTable();
  ASSERT_EQ(isNull(p), false);
  EXPECT_EQ(p->size(), 0);
}

TEST(SymbolTableTest, LookUpOfNameInEmptyTableReturnsFalseNullOrZero){
  SgSymbolTable *p = new SgSymbolTable();
  const SgName foo("foo");
  EXPECT_EQ(p->exists(foo), false);
//  EXPECT_EQ(isNull(p->find_any(foo)),true); error: no matching function for call to ‘SgSymbolTable::find_any(const SgName&)’ FIXME WTF
  EXPECT_EQ(isNull(p->find_variable(foo)), true);
  EXPECT_EQ(isNull(p->find_class(foo)), true);
  EXPECT_EQ(isNull(p->find_function(foo)), true);
  EXPECT_EQ(isNull(p->find_function_type(foo)), true);
  EXPECT_EQ(isNull(p->find_typedef(foo)), true);
  EXPECT_EQ(isNull(p->find_enum(foo)), true);
  EXPECT_EQ(isNull(p->find_enum_field(foo)), true);
  EXPECT_EQ(isNull(p->find_label(foo)), true);
  EXPECT_EQ(isNull(p->find_java_label(foo)), true);
  EXPECT_EQ(isNull(p->find_namespace(foo)), true);
//  EXPECT_EQ(isNull(p->find_aliased_symbol(foo)), true);
  EXPECT_EQ(p->count(foo), 0);
  EXPECT_EQ(p->count_aliases(foo), 0);
  EXPECT_EQ(p->get_symbols().size(), 0);
}

TEST(SymbolTableTest, InsertNameWithNullSymbolAborts){
  SgSymbolTable *p = new SgSymbolTable();
  const SgName foo("foo");
  EXPECT_EQ(p->size(), 0);
  EXPECT_DEATH(p->insert(foo, NULL), "");
}
