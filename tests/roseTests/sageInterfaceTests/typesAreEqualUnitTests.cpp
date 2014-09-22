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
}
