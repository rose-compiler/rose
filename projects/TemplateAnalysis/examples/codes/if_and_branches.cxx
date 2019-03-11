
#ifndef USE_CONCEPT
#  define USE_CONCEPT 0
#endif
#ifndef USE_BASIC_CONCEPT
#  define USE_BASIC_CONCEPT 0
#endif

#ifndef INSTANTIATION_TESTS
#  define INSTANTIATION_TESTS 0
#endif

#ifndef TEST_FAILING
#  define TEST_FAILING 0
#endif
#ifndef TEST_FAILING_MISSING_MEMBER_TYPE
#  define TEST_FAILING_MISSING_MEMBER_TYPE TEST_FAILING
#endif
#ifndef TEST_FAILING_NOT_A_BOOLEAN
#  define TEST_FAILING_NOT_A_BOOLEAN TEST_FAILING
#endif

#ifndef TEST_FAILING_INCOMPLETE
#  define TEST_FAILING_INCOMPLETE TEST_FAILING
#endif

#ifndef USE_DEFAULT_FOR_TRUE_BRANCH
#  define USE_DEFAULT_FOR_TRUE_BRANCH 0
#endif

// Define IF: the branching template

#if USE_DEFAULT_FOR_TRUE_BRANCH
template <bool Bcond, typename Ttrue, typename Tfalse>
struct IF {
  typedef typename Ttrue::branch taken;
};
#else
template <bool Bcond, typename Ttrue, typename Tfalse>
struct IF ;

template <typename Ttrue, typename Tfalse>
struct IF<true, Ttrue, Tfalse> {
  typedef typename Ttrue::branch taken;
};
#endif
template <typename Ttrue, typename Tfalse>
struct IF<false, Ttrue, Tfalse> {
  typedef typename Tfalse::branch taken;
};

// Definition of IfStmt with or without concept

#if USE_CONCEPT

#  if USE_BASIC_CONCEPT

template <typename T>
concept bool IfStmt_param = requires (T t) {
  { T::Condition } -> bool;
  typename T::TrueBranch::branch;
  typename T::FalseBranch::branch;
};

#  else

template <typename T>
constexpr bool get_condition() {
  return T::Condition;
}

template <typename T>
concept bool is_valid_true_branch = requires (T t) {
  ( get_condition<T>() );
  typename T::TrueBranch::branch;
};

template <typename T>
concept bool is_valid_false_branch = requires (T t) {
  ( ! get_condition<T>() );
  typename T::FalseBranch::branch;
};

template <typename T>
concept bool IfStmt_param = requires (T t) {
  { T::Condition } -> bool;
  typename T::TrueBranch;
  typename T::FalseBranch;
  is_valid_true_branch<T> || is_valid_false_branch<T>;
};

#  endif

template <IfStmt_param Tif>
struct IfStmt {
  typedef typename IF<Tif::Condition, typename Tif::TrueBranch, typename Tif::FalseBranch>::taken taken;
};

#else

template <typename Tif>
struct IfStmt {
  typedef typename IF<Tif::Condition, typename Tif::TrueBranch, typename Tif::FalseBranch>::taken taken;
};

#endif

#if INSTANTIATION_TESTS

// Two branch structs for testing...

struct branch_0 {
  typedef int branch;
};
struct branch_1 {
  struct branch {};
};
struct branch_2 {};

// Passing Tests

struct if_passing_1 {
  static constexpr bool Condition = true;
  typedef branch_0 TrueBranch;
  typedef branch_1 FalseBranch;
};

IfStmt<if_passing_1> stmt_passing_1;

//   Second: fails with basic concept

struct if_passing_2 {
  enum { Condition = 0 };
  struct TrueBranch {};
  struct FalseBranch {
    struct branch {};
  };
};

IfStmt<if_passing_2> stmt_passing_2;

// Failing Tests

#if TEST_FAILING_MISSING_MEMBER_TYPE

struct if_failing_1 {
  static constexpr bool Condition = true;
  typedef branch_0 TrueBranch;
};

IfStmt<if_failing_1> stmt_failing_1;

#endif

#if TEST_FAILING_NOT_A_BOOLEAN

struct if_failing_2 {
  typedef int Condition;
  typedef branch_0 TrueBranch;
  typedef branch_1 FalseBranch;
};

IfStmt<if_failing_2> stmt_failing_2;

#endif

#if TEST_FAILING_INCOMPLETE

struct if_failing_3 {
  static constexpr bool Condition = false;
  typedef branch_0 TrueBranch;
  typedef branch_2 FalseBranch;
};

IfStmt<if_failing_3> stmt_failing_3;

#endif

#endif

