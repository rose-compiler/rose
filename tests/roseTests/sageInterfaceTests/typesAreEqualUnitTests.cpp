/*
 * Unit tests for the type Equivalence check
 * TODO:
 *       All built-in types
 *       Reference types
 *       Named types
 *       Template types
 *       Anonymous types
 *       Modifier types
 *       Modified types
 *       Function types
 *       Opaque types
 *       XXX
 *       What about Java types? They should be covered as well..?
 *       Fortran types..?
 *       Python types...?
 */

#include "rose.h"
#include "gtest/gtest.h"

TEST(SageInterfaceTypeEquivalence, HandlesEmptyInput){
  ::SgTypeInt* int_t1 = SageBuilder::buildIntType();
  bool tcRes = SageInterface::checkTypesAreEqual(int_t1, NULL);
  ASSERT_EQ(tcRes, false);

  tcRes = SageInterface::checkTypesAreEqual(NULL, int_t1);
  ASSERT_EQ(tcRes, false);
}

/*
 * We rely on the build mechanism that SageBuilder provides
 * We construct all (yet a lot have to be added) built-int types
 * and check pairs of the same type
 */
TEST(SageInterfaceTypeEquivalence, BuiltInTypesAreEqual) {
  ::SgTypeInt *int_t1 = SageBuilder::buildIntType();
  ::SgTypeInt *int_t2 = SageBuilder::buildIntType();

  bool tcRes = SageInterface::checkTypesAreEqual(int_t1, int_t2);
  ASSERT_EQ(tcRes, true);

  ::SgTypeShort* short_t1 = SageBuilder::buildShortType();
  ::SgTypeShort* short_t2 = SageBuilder::buildShortType();
  tcRes = SageInterface::checkTypesAreEqual(short_t1, short_t2);
  ASSERT_EQ(tcRes, true);

  ::SgTypeBool* bool_t1 = SageBuilder::buildBoolType();
  ::SgTypeBool* bool_t2 = SageBuilder::buildBoolType();
  tcRes = SageInterface::checkTypesAreEqual(bool_t1, bool_t2);
  ASSERT_EQ(tcRes, true);

  ::SgTypeVoid* void_t1 = SageBuilder::buildVoidType();
  ::SgTypeVoid* void_t2 = SageBuilder::buildVoidType();
  tcRes = SageInterface::checkTypesAreEqual(void_t1, void_t2);
  ASSERT_EQ(tcRes, true);
}

/*
 * We construct pairs of different types and check them.
 * The result for each test should of course be false
 */
TEST(SageInterfaceTypeEquivalence, BuiltInTypesAreUnequal){
  ::SgTypeInt* int_t1 = SageBuilder::buildIntType();
  ::SgTypeBool* bool_t2 = SageBuilder::buildBoolType();
  bool tcRes = SageInterface::checkTypesAreEqual(int_t1, bool_t2);
  ASSERT_EQ(tcRes, false);

  ::SgTypeShort* short_t1 = SageBuilder::buildShortType();
  ::SgTypeInt* int_t2 = SageBuilder::buildIntType();
  tcRes = SageInterface::checkTypesAreEqual(short_t1, int_t2);
  ASSERT_EQ(tcRes, false);

  ::SgTypeInt* int_t3 = SageBuilder::buildIntType();
  ::SgTypeVoid* void_t2 = SageBuilder::buildVoidType();
  tcRes = SageInterface::checkTypesAreEqual(int_t3, void_t2);
  ASSERT_EQ(tcRes, false);
}

/*
 * Pointer types to the same base type.
 * A lot of things need to be added here. But this is meant as a first
 * safety net.
 */
TEST(SageInterfaceTypeEquivalence, PointerTypesAreEqual){
  ::SgPointerType* p_1 = SageBuilder::buildPointerType(SageBuilder::buildIntType());
  ::SgPointerType* p_2 = SageBuilder::buildPointerType(SageBuilder::buildIntType());
  bool tcRes = SageInterface::checkTypesAreEqual(p_1, p_2);
  ASSERT_EQ(tcRes, true);

  ::SgPointerType* p_3 = SageBuilder::buildPointerType(SageBuilder::buildVoidType());
  ::SgPointerType* p_4 = SageBuilder::buildPointerType(SageBuilder::buildVoidType());
  tcRes = SageInterface::checkTypesAreEqual(p_3, p_4);
  ASSERT_EQ(tcRes, true);

  ::SgPointerType* p_5 = SageBuilder::buildPointerType(p_3);
  ::SgPointerType* p_6 = SageBuilder::buildPointerType(p_4);
  tcRes = SageInterface::checkTypesAreEqual(p_5, p_6);
  ASSERT_EQ(tcRes, true);
}

/*
 * We construct pointer types to different base types.
 */
TEST(SageInterfaceTypeEquivalence, PointerTypesAreUnequal){
  ::SgPointerType* p_1 = SageBuilder::buildPointerType(SageBuilder::buildIntType());
  ::SgPointerType* p_2 = SageBuilder::buildPointerType(SageBuilder::buildCharType());
  bool tcRes = SageInterface::checkTypesAreEqual(p_1, p_2);
  ASSERT_EQ(tcRes, false);

  ::SgPointerType* p_3 = SageBuilder::buildPointerType(SageBuilder::buildVoidType());
  ::SgPointerType* p_4 = SageBuilder::buildPointerType(SageBuilder::buildIntType());
  tcRes = SageInterface::checkTypesAreEqual(p_3, p_4);
  ASSERT_EQ(tcRes, false);

  ::SgPointerType* p_5 = SageBuilder::buildPointerType(p_3);
  ::SgPointerType* p_6 = SageBuilder::buildPointerType(p_4);
  tcRes = SageInterface::checkTypesAreEqual(p_5, p_6);
  ASSERT_EQ(tcRes, false);
}

/*
 * Reference types
 */
TEST(SageInterfaceTypeEquivalence, ReferenceTypesAreEqual){
  ::SgReferenceType* r_1 = SageBuilder::buildReferenceType(SageBuilder::buildIntType());
  ::SgReferenceType* r_2 = SageBuilder::buildReferenceType(SageBuilder::buildIntType());
  bool tcRef = SageInterface::checkTypesAreEqual(r_1, r_2);
  ASSERT_EQ(tcRef, true);

  ::SgReferenceType* r_3 = SageBuilder::buildReferenceType(SageBuilder::buildVoidType());
  ::SgReferenceType* r_4 = SageBuilder::buildReferenceType(SageBuilder::buildVoidType());
  tcRef = SageInterface::checkTypesAreEqual(r_3, r_4);
  ASSERT_EQ(tcRef, true);
}

TEST(SageInterfaceTypeEquivalence, ReferenceTypesAreUnequal){
  ::SgReferenceType* r_1 = SageBuilder::buildReferenceType(SageBuilder::buildIntType());
  ::SgReferenceType* r_2 = SageBuilder::buildReferenceType(SageBuilder::buildVoidType());
  bool tcRef = SageInterface::checkTypesAreEqual(r_1, r_2);
  ASSERT_EQ(tcRef, false);

  ::SgReferenceType* r_3 = SageBuilder::buildReferenceType(SageBuilder::buildPointerType(SageBuilder::buildVoidType()));
  ::SgReferenceType* r_4 = SageBuilder::buildReferenceType(SageBuilder::buildVoidType());
  tcRef = SageInterface::checkTypesAreEqual(r_3, r_4);
  ASSERT_EQ(tcRef, false); 
}

/*
 * Array Types
 */
TEST(SageInterfaceTypeEquivalence, ArrayTypesAreEqual){
  // type(int a[10]) == type(int a[v]) (with v being const int v = 10.
  ::SgArrayType* a_1 = SageBuilder::buildArrayType(SageBuilder::buildIntType(), SageBuilder::buildIntVal(3));
  ::SgArrayType* a_2 = SageBuilder::buildArrayType(SageBuilder::buildIntType(), SageBuilder::buildIntVal(3));
  bool tcRef = SageInterface::checkTypesAreEqual(a_1, a_2);
  ASSERT_EQ(tcRef, true);

  ::SgArrayType* a_3 = SageBuilder::buildArrayType(SageBuilder::buildCharType(), SageBuilder::buildIntVal(100));
  ::SgArrayType* a_4 = SageBuilder::buildArrayType(SageBuilder::buildCharType(), SageBuilder::buildIntVal(100));
  tcRef = SageInterface::checkTypesAreEqual(a_3, a_4);
  ASSERT_EQ(tcRef, true);

  ::SgArrayType* a_5 = SageBuilder::buildArrayType(a_3, SageBuilder::buildIntVal(100));
  ::SgArrayType* a_6 = SageBuilder::buildArrayType(a_4, SageBuilder::buildIntVal(100));
  tcRef = SageInterface::checkTypesAreEqual(a_5, a_6);
  ASSERT_EQ(tcRef, true);

#if 0
  // build assign-initializer, build variable declaration, build varrefexp
  ::SgBasicBlock* bb = SageBuilder::buildBasicBlock();
  ::SgAssignInitializer* init = SageBuilder::buildAssignInitializer(SageBuilder::buildIntVal(42), SageBuilder::buildIntType());
  ::SgVariableDeclaration* vDecl = isSgVariableDeclaration(SageBuilder::buildVariableDeclaration(SgName("refVar"), SageBuilder::buildIntType(), init, bb));
  vDecl->get_declarationModifier().get_typeModifier().get_constVolatileModifier().setConst();
  ::SgVarRefExp* vRef = SageBuilder::buildVarRefExp(vDecl);
  ::SgArrayType* a_7 = SageBuilder::buildArrayType(SageBuilder::buildIntType(), vRef);
  ::SgArrayType* a_8 = SageBuilder::buildArrayType(SageBuilder::buildIntType(), vRef);
  tcRef = SageInterface::checkTypesAreEqual(a_7, a_8);
  ASSERT_EQ(tcRef, true);
#endif
}

TEST(SageInterfaceTypeEquivalence, ArrayTypesAreUnequal){
  ::SgArrayType* a_1 = SageBuilder::buildArrayType(SageBuilder::buildCharType(), SageBuilder::buildIntVal(42));
  ::SgArrayType* a_2 = SageBuilder::buildArrayType(SageBuilder::buildIntType(), SageBuilder::buildIntVal(42));
  bool tcRef = SageInterface::checkTypesAreEqual(a_1, a_2);
  ASSERT_EQ(tcRef, false);

  ::SgArrayType* a_3 = SageBuilder::buildArrayType(SageBuilder::buildCharType(), SageBuilder::buildIntVal(42));
  ::SgArrayType* a_4 = SageBuilder::buildArrayType(SageBuilder::buildCharType(), SageBuilder::buildIntVal(21));
  tcRef = SageInterface::checkTypesAreEqual(a_3, a_4);
  ASSERT_EQ(tcRef, false);
}
