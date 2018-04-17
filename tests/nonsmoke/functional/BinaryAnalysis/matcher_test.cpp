#include "matcher.hpp"
#include <gtest/gtest.h>

using namespace matcher;

namespace {

using rse::OP_ADD;
using rse::OP_SET;
using rse::OP_ITE;

struct SimpleMatchers : ::testing::Test {
  using interior = rse::Interior;
  using leaf = rse::Leaf;
  using Ptr = rse::Ptr;

  SimpleMatchers() {
    var_1 = simple_var()->newFlags(1);
    bvar_1 = simple_var(1);
    mem_1 = simple_mem()->newFlags(2);
    int_1 = simple_int(1)->newFlags(3);
    int_2 = simple_int(2)->newFlags(4);
    expr_1 = interior::create(32, OP_ADD, var_1, int_1);
    expr_2 = interior::create(32, OP_ITE, bvar(), var_1, int_1);
    expr_3 = interior::create(32, OP_ITE, bvar_1, expr_1, expr_2);
    expr_4 = interior::create(32, OP_SET, var_1, int_1);
  }

  Ptr empty;
  Ptr var_1;
  Ptr bvar_1;
  Ptr mem_1;
  Ptr int_1;
  Ptr int_2;
  Ptr expr_1;
  Ptr expr_2;
  Ptr expr_3;
  Ptr expr_4;

  static Ptr bvar() {
    return simple_var(1);
  }

  static Ptr simple_mem(size_t addrw=32, size_t bits=32) {
    return leaf::createMemory(addrw, bits);
  }

  static Ptr simple_var(size_t bits=32) {
    return leaf::createVariable(bits);
  }

  static Ptr simple_int(uint64_t val, size_t bits=32) {
    return leaf::createInteger(bits, val);
  }
};

TEST_F(SimpleMatchers, match_Any) {
  auto m = Matcher<Any>();
  EXPECT_TRUE(m(var_1));
  EXPECT_TRUE(m(mem_1));
  EXPECT_TRUE(m(int_1));
  EXPECT_TRUE(m(expr_1));
  EXPECT_TRUE(m(expr_2));
  EXPECT_FALSE(m(empty));
}

TEST_F(SimpleMatchers, match_Leaf) {
  auto m = Matcher<Leaf>();
  EXPECT_TRUE(m(var_1));
  EXPECT_TRUE(m(mem_1));
  EXPECT_TRUE(m(int_1));
  EXPECT_FALSE(m(expr_1));
  EXPECT_FALSE(m(expr_2));
  EXPECT_FALSE(m(empty));
}

TEST_F(SimpleMatchers, match_Interior) {
  auto m = Matcher<Interior>();
  EXPECT_FALSE(m(var_1));
  EXPECT_FALSE(m(mem_1));
  EXPECT_FALSE(m(int_1));
  EXPECT_TRUE(m(expr_1));
  EXPECT_TRUE(m(expr_2));
  EXPECT_FALSE(m(empty));
}

TEST_F(SimpleMatchers, match_Int) {
  auto m = Matcher<Int>();
  EXPECT_FALSE(m(var_1));
  EXPECT_FALSE(m(mem_1));
  EXPECT_TRUE(m(int_1));
  EXPECT_TRUE(m(int_2));
  EXPECT_FALSE(m(expr_1));
  EXPECT_FALSE(m(expr_2));
  EXPECT_FALSE(m(empty));
}

TEST_F(SimpleMatchers, match_Const) {
  auto m = Matcher<Const<2>>();
  EXPECT_FALSE(m(var_1));
  EXPECT_FALSE(m(mem_1));
  EXPECT_FALSE(m(int_1));
  EXPECT_TRUE(m(int_2));
  EXPECT_FALSE(m(expr_1));
  EXPECT_FALSE(m(expr_2));
  EXPECT_FALSE(m(empty));
}

TEST_F(SimpleMatchers, match_Var) {
  auto m = Matcher<Var>();
  EXPECT_TRUE(m(var_1));
  EXPECT_FALSE(m(mem_1));
  EXPECT_FALSE(m(int_1));
  EXPECT_FALSE(m(expr_1));
  EXPECT_FALSE(m(expr_2));
  EXPECT_FALSE(m(empty));
}

TEST_F(SimpleMatchers, match_Mem) {
  auto m = Matcher<Mem>();
  EXPECT_FALSE(m(var_1));
  EXPECT_TRUE(m(mem_1));
  EXPECT_FALSE(m(int_1));
  EXPECT_FALSE(m(expr_1));
  EXPECT_FALSE(m(expr_2));
  EXPECT_FALSE(m(empty));
}

TEST_F(SimpleMatchers, match_Expr) {
  auto m = Matcher<Expr<Any, ArgList>>();
  EXPECT_FALSE(m(var_1));
  EXPECT_FALSE(m(mem_1));
  EXPECT_FALSE(m(int_1));
  EXPECT_TRUE(m(expr_1));
  EXPECT_TRUE(m(expr_2));
  EXPECT_FALSE(m(empty));
}

TEST_F(SimpleMatchers, match_ExprOp) {
  auto m = Matcher<Expr<Op<OP_ADD>, ArgList>>();
  EXPECT_TRUE(m(expr_1));
  EXPECT_FALSE(m(expr_2));
}

TEST_F(SimpleMatchers, match_ExprArgs1) {
  auto m = Matcher<Expr<Any, Var, ArgList>>();
  EXPECT_TRUE(m(expr_1));
  EXPECT_TRUE(m(expr_2));
}

TEST_F(SimpleMatchers, match_ExprArgs2) {
  auto m = Matcher<Expr<Any, Any, Var, ArgList>>();
  EXPECT_FALSE(m(expr_1));
  EXPECT_TRUE(m(expr_2));
}

TEST_F(SimpleMatchers, match_ExprSize) {
  auto m1 = Matcher<Expr<Any, Any, Any>>();
  EXPECT_TRUE(m1(expr_1));
  EXPECT_FALSE(m1(expr_2));
  auto m2 = Matcher<Expr<Any, Any, Any, Any>>();
  EXPECT_FALSE(m2(expr_1));
  EXPECT_TRUE(m2(expr_2));
  auto m3 = Matcher<Expr<Any, Any>>();
  EXPECT_FALSE(m3(expr_1));
  EXPECT_FALSE(m3(expr_2));
}

TEST_F(SimpleMatchers, match_ExprNesting) {
  auto m = Matcher<Expr<Any, Var, Expr<Op<OP_ADD>, ArgList>, Expr<Any, Any, Var, ArgList>>>();
  EXPECT_TRUE(m(expr_3));
}

TEST_F(SimpleMatchers, match_Ref) {
  node_t n;
  auto m = Matcher<Ref<Any>>(n);
  EXPECT_TRUE(m(var_1));
  EXPECT_EQ(var_1, n);
  EXPECT_TRUE(m(mem_1));
  EXPECT_EQ(mem_1, n);
  EXPECT_TRUE(m(int_1));
  EXPECT_EQ(int_1, n);
  EXPECT_TRUE(m(expr_1));
  EXPECT_EQ(expr_1, n);
  EXPECT_TRUE(m(expr_2));
  EXPECT_EQ(expr_2, n);
  EXPECT_FALSE(m(empty));
}

TEST_F(SimpleMatchers, match_Ref_Op) {
  operator_t op;
  auto m = Matcher<Expr<Ref<Any>, ArgList>>(op);
  EXPECT_TRUE(m(expr_1));
  EXPECT_EQ(OP_ADD, op);
  EXPECT_TRUE(m(expr_2));
  EXPECT_EQ(OP_ITE, op);
}

TEST_F(SimpleMatchers, match_Ref_nested) {
  node_t n, e;
  operator_t op;
  nodelist_t nl;
  auto m = Matcher<Expr<Any, Ref<Var>, Expr<Ref<Op<OP_ADD>>, ArgList>,
                        Ref<Expr<Any, Any, Var, Ref<ArgList>>>>>(n, op, e, nl);
  EXPECT_TRUE(m(expr_3));
  EXPECT_EQ(bvar_1, n);
  EXPECT_EQ(op, OP_ADD);
  EXPECT_EQ(e, expr_2);
  EXPECT_EQ(1, nl.size());
  EXPECT_EQ(int_1, nl.at(0));
}

TEST_F(SimpleMatchers, match_Ref_convert) {
  leaf_t     lf;
  auto m = Matcher<Ref<Leaf>>(lf);
  EXPECT_TRUE(m(var_1));
  EXPECT_EQ(var_1, lf);
  EXPECT_TRUE(m(mem_1));
  EXPECT_EQ(mem_1, lf);
  EXPECT_TRUE(m(int_1));
  EXPECT_EQ(int_1, lf);
  interior_t in;
  auto m2 = Matcher<Ref<Interior>>(in);
  EXPECT_TRUE(m2(expr_1));
  EXPECT_EQ(expr_1, in);
  EXPECT_TRUE(m2(expr_2));
  EXPECT_EQ(expr_2, in);
  uint64_t i;
  auto m3 = Matcher<Ref<Int>>(i);
  EXPECT_TRUE(m3(int_1));
  EXPECT_EQ(1, i);
  EXPECT_TRUE(m3(int_2));
  EXPECT_EQ(2, i);
}

TEST_F(SimpleMatchers, match_Bits) {
  node_t n1, n2;
  auto m = Matcher<Expr<Any, Ref<Bits<1, Ref<Var>>>, ArgList>>(n1, n2);
  EXPECT_FALSE(m(expr_1));
  EXPECT_TRUE(m(expr_2));
  EXPECT_TRUE(n1 == n2);
  EXPECT_TRUE(m(expr_3));
  EXPECT_EQ(bvar_1, n1);
  EXPECT_EQ(bvar_1, n2);
}

TEST_F(SimpleMatchers, match_FlagsMask) {
  auto m = Matcher<FlagsMask<3, 0, Any>>();
  EXPECT_FALSE(m(var_1));
  EXPECT_TRUE(m(bvar_1));
  EXPECT_FALSE(m(mem_1));
  EXPECT_FALSE(m(int_1));
  EXPECT_TRUE(m(int_2));
  EXPECT_FALSE(m(empty));
  node_t n1, n2;
  auto m2 = Matcher<Ref<FlagsMask<3, 0, Ref<Any>>>>(n1, n2);
  EXPECT_TRUE(m2(int_2));
  EXPECT_EQ(int_2, n1);
  EXPECT_EQ(int_2, n2);
}


TEST_F(SimpleMatchers, match_Flags) {
  auto m = Matcher<Flags<1, Any>>();
  EXPECT_TRUE(m(var_1));
  EXPECT_FALSE(m(bvar_1));
  EXPECT_FALSE(m(mem_1));
  EXPECT_TRUE(m(int_1));
  EXPECT_FALSE(m(int_2));
  EXPECT_FALSE(m(empty));
  node_t n1, n2;
  auto m2 = Matcher<Ref<Flags<1, Ref<Any>>>>(n1, n2);
  EXPECT_TRUE(m2(var_1));
  EXPECT_EQ(var_1, n1);
  EXPECT_EQ(var_1, n2);
}

TEST_F(SimpleMatchers, macth_Or) {
  auto m = Matcher<Or<Int, Mem, Interior>>();
  EXPECT_FALSE(m(var_1));
  EXPECT_TRUE(m(mem_1));
  EXPECT_TRUE(m(int_1));
  EXPECT_TRUE(m(expr_1));
  EXPECT_TRUE(m(expr_2));
  EXPECT_FALSE(m(empty));
  node_t n1, n2;
  auto m2 = Matcher<Ref<Or<Ref<Int>, Ref<Interior>>>>(n1, n2, n2);
  EXPECT_FALSE(m2(var_1));
  EXPECT_TRUE(m2(int_1));
  EXPECT_EQ(int_1, n1);
  EXPECT_EQ(int_1, n2);
  EXPECT_TRUE(m2(expr_1));
  EXPECT_EQ(expr_1, n1);
  EXPECT_EQ(expr_1, n2);
}

TEST_F(SimpleMatchers, match_OrInExpr) {
  auto m = Matcher<Expr<Any, Any, Or<Int, Var>, ArgList>>();
  EXPECT_TRUE(m(expr_1));
  EXPECT_TRUE(m(expr_2));
  EXPECT_FALSE(m(expr_3));
}

TEST_F(SimpleMatchers, match_OrOp) {
  auto m = Matcher<Expr<Or<Op<OP_ADD>, Op<OP_ITE>>, ArgList>>();
  EXPECT_TRUE(m(expr_1));
  EXPECT_TRUE(m(expr_2));
  EXPECT_TRUE(m(expr_3));
  EXPECT_FALSE(m(expr_4));
  operator_t op1, op2;
  auto m2 = Matcher<Expr<Ref<Or<Ref<Op<OP_ADD>>, Ref<Op<OP_ITE>>>>, ArgList>>(op1, op2, op2);
  EXPECT_TRUE(m2(expr_1));
  EXPECT_EQ(OP_ADD, op1);
  EXPECT_EQ(OP_ADD, op2);
  EXPECT_TRUE(m2(expr_2));
  EXPECT_EQ(OP_ITE, op1);
  EXPECT_EQ(OP_ITE, op2);
}

TEST_F(SimpleMatchers, match_OrConvert) {
  interior_t i;
  auto m = Matcher<Ref<Or<Expr<Op<OP_ADD>, ArgList>, Expr<Op<OP_ITE>, ArgList>>>>(i);
  EXPECT_FALSE(m(expr_4));
  EXPECT_TRUE(m(expr_1));
  EXPECT_EQ(expr_1, i);
  EXPECT_TRUE(m(expr_2));
  EXPECT_EQ(expr_2, i);
}

} // anonymous namespace


int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
