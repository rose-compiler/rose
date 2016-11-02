
void foobar_1() throw();

void foobar_2()  __attribute__((constructor));

void foobar_3()  __attribute__((destructor));

void foobar_4()  __attribute__((pure));

void foobar_5()  __attribute__((weak));

void foobar_6()  __attribute__((unused));

void foobar_7()  __attribute__((used));

void foobar_8()  __attribute__((deprecated));

void foobar_9()  __attribute__((malloc));

void foobar_10()  __attribute__((naked));

// Note that most attributes can use either of two names (with or without leading a trailing underscores).
void foobar_11a()  __attribute__((no_instrument_function));
void foobar_11b()  __attribute__((__no_instrument_function__));

void foobar_12a()  __attribute__((no_check_memory_usage));
void foobar_12b()  __attribute__((__no_check_memory_usage__));

void foobar_13()  __attribute__((noinline));

void foobar_14()  __attribute__((always_inline));

void foobar_15()  __attribute__((nothrow));

// Note that weakref does not apply to extern variables or function declarations.
#ifdef __clang__
// DQ (4/16/2016): See test2016_20.C for an example of this for testing clang backend with ROSE.
// void foobar_16a();
// static void foobar_16()  __attribute__((weakref)) __attribute__ ((alias("foobar_16a")));
// static void foobar_16() __attribute__ ((alias("foobar_16a"))) __attribute__((weakref));
#else
static void foobar_16()  __attribute__((weakref));
#endif

// Example of type attribute.
// Note that zero alignment is no alignment.
typedef int foo_integer_1 __attribute__((aligned(0)));
typedef int foo_integer_2 __attribute__((aligned(1)));
typedef int foo_integer_3 __attribute__((aligned(2)));
typedef int foo_integer_4 __attribute__((aligned(4)));
typedef int foo_integer_5 __attribute__((aligned(8)));
typedef int foo_integer_6 __attribute__((aligned(16)));
typedef int foo_integer_7 __attribute__((aligned(32)));
typedef int foo_integer_8 __attribute__((aligned(64)));
typedef int foo_integer_9 __attribute__((aligned(128)));

// These are not legal values for alignment specification.
// typedef int foo_integer_10 __attribute__((aligned(256)));
// typedef int foo_integer_11 __attribute__((aligned(512)));

// These will both output (unparsed) the same (without the alignment specification), 
// the EDG front-end does not proved any choice here.
int x_0;
int x_1 __attribute__((aligned(0)));

int x_2 __attribute__((aligned(1)));
int x_3 __attribute__((aligned(2)));
int x_4 __attribute__((aligned(4)));

// Alignment directives can be associated with different variables in a single declaration.
// So it needs to be associated with the SgInitializedName instead of the SgVariableDeclaration.
int a_1 __attribute__((aligned(2))), b_1 __attribute__((aligned(4)));

// Use of multiple attributes value will generate multiple attribute specifications 
// which appear to be semantically equivalent but is a normalization of the input code.
void foobar_multiattribute_1()  __attribute__((noinline,no_instrument_function));

struct X
   {
     int field_x_0;
     int field_x_1 __attribute__((aligned(0)));
     int field_x_2 __attribute__((aligned(1)));
     int field_x_3 __attribute__((aligned(2)));
     int field_x_4 __attribute__((aligned(4)));
   };

// We don't have to check defining functions since "attributes are not permitted in a function definition"
// void foobar_2() __attribute__((constructor)) {}
