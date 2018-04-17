
namespace X
{
   int variableVar;
   enum booleanType {TRUE,FALSE};
   class classType {};
   
}

// Test qualification of names of variables defined in a namespace
int variableWithInitializer = X::variableVar;

// DQ (1/7/2007): Temporarily commented out!
#if 1

// Test qualificiation of names of enums defined in a namespace
X::booleanType booleanVar;

// Test qualificiation of names of enums defined in a namespace
X::classType classVar;
#endif

// Cases which require name qualification:
//   enum values, function names, member fuction names, variable ref expressions
