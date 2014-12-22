// Example from C++11 Wiki web page:
// C++ has always had the concept of constant expressions. These are expressions such as 3+4 that will 
// always yield the same results, at compile time and at run time. Constant expressions are optimization 
// opportunities for compilers, and compilers frequently execute them at compile time and hardcode the 
// results in the program. Also, there are a number of places where the C++ specification requires the 
// use of constant expressions. Defining an array requires a constant expression, and enumerator values 
// must be constant expressions.

// However, constant expressions have always ended whenever a function call or object constructor was 
// encountered. So a piece of code as simple as this is illegal:

#if 0
int get_five() {return 5;}
 
int some_value[get_five() + 7]; // Create an array of 12 integers. Ill-formed C++
#endif

// This was not legal in C++03, because get_five() + 7 is not a constant expression. A C++03 compiler 
// has no way of knowing if get_five() actually is constant at runtime. In theory, this function could 
// affect a global variable, call other non-runtime constant functions, etc.

// C++11 introduced the keyword constexpr, which allows the user to guarantee that a function or object 
// constructor is a compile-time constant [8]. The above example can be rewritten as follows:

constexpr int get_five() {return 5;}
 
int some_value[get_five() + 7]; // Create an array of 12 integers. Legal C++11

// This allows the compiler to understand, and verify, that get_five is a compile-time constant.

// The use of constexpr on a function imposes some limitations on what that function can do. First, the 
// function must have a non-void return type. Second, the function body cannot declare variables or define 
// new types. Third, the body may contain only declarations, null statements and a single return statement. 
// There must exist argument values such that, after argument substitution, the expression in the return 
// statement produces a constant expression.

// Prior to C++11, the values of variables could be used in constant expressions only if the variables are 
// declared const, have an initializer which is a constant expression, and are of integral or enumeration 
// type. C++11 removes the restriction that the variables must be of integral or enumeration type if they 
// are defined with the constexpr keyword:

constexpr double earth_gravitational_acceleration = 9.8;
constexpr double moon_gravitational_acceleration = earth_gravitational_acceleration / 6.0;

// Such data variables are implicitly const, and must have an initializer which must be a constant expression.

