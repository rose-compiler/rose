// This code attempts to use every modifier in C++
// #ifdef __GNUC__
#if ( defined(__clang__) || defined(__GNUC__) )
// GNU specific case
   #define RESTRICT __restrict__
   #define EXPORT 
#else
// Other compilers
   #define RESTRICT restrict
   #define EXPORT export
#endif

#define TEST_INLINING       1
#define TEST_CONST_MEMBERS  1
#define TEST_STATIC_MEMBERS 1

#if 1
// working
const float* RESTRICT constRestrictIntegerPointer = 0;
float* RESTRICT restrictIntegerPointer;
volatile int volatileIntegerValue = 0;
int integerValue = 0;
static int staticIntegerValue;
extern int externIntegerValue;

void voidFunction() {}
inline void voidInlineFunction() {}
const void constVoidFunction() {}
static void staticFunctionEmptyBody() {}

void functionWithRegisterLocalVariable()
   {
     register int registerIntegerValue;
   }

static void staticFunctionWithStaticLocalVariable()
   {
     static double x = 0;
     register int registerIntegerValue;
   }

const void constVoidFunctionThrow() throw() { }

void functionRegisterIntegerParameter ( int j );
void functionRegisterIntegerParameter ( register int i ) {};
#endif

#if 1
EXPORT template < class T > void templateFunction () {};
#endif

#if 1
class classType
   {
     public:
#if 1
          double publicDoubleValue;
#endif
#if TEST_STATIC_MEMBERS
          static double publicStaticDoubleValue;
#endif
#if TEST_CONST_MEMBERS
          const double publicConstDoubleValue;
#endif
#if 1
          volatile double publicVolatileDoubleValue;
          mutable double publicMutableDoubleValue;
#endif
#if 1
       // Error: "inline" only placed on function declaration appearing in the class 
       // and not to the function declaration that appears with the member function 
       // definition.  I think this is an error. Also, non explicitly inlined member 
       // functions declared in the class are output with explicit inlining by the 
       // unparser.  It could be that the modifiers are not copied (possible copy 
       // constructor bug?).
          void publicMemberFunction () {};
          void publicMemberFunctionConst () const {};
          void publicMemberFunctionVolatile () volatile {};
          void publicMemberFunctionThrow () throw() {};
#endif

#if TEST_INLINING
          void noninlinePublicMemberFunctionWithDefinition () {};
          void noninlinePublicMemberFunctionWithoutDefinition ();
       // "inline" specification is optional here if it appears 
       // in the declaration where the function is defined
          inline void inlinePublicMemberFunctionWithoutDefinition ();
          void inlinePublicMemberFunctionWithoutDefinitionSpecifiedInDefnOnly ();
          inline void inlinePublicMemberFunctionWithoutDefinitionSpecifiedInDeclOnly ();
          inline void inlinePublicMemberFunction () {};
#endif

#if 1
       // void publicMemberFunctionIntegerParameter ( register int i ) {};
       // classType ( register int* i ) {};
#if TEST_CONST_MEMBERS
          classType()
            // g++ does not require the initializers of const variables, EDG does
             : publicConstDoubleValue(42),
               protectedConstDoubleValue(43),
               privateConstDoubleValue(44)
             {};
#endif
#endif


#if TEST_CONST_MEMBERS
          explicit classType(register int integerValueParameter)
            // g++ does not require the initializers of const variables, EDG does
             : publicConstDoubleValue(1),
               protectedConstDoubleValue(2),
               privateConstDoubleValue(3)
             {};
#endif

#if 1
          void publicMemberFunctionIntegerPointerParamter ( int* integerPointerParameter ) {};
          void publicMemberFunctionIntegerRestrictPointerParameter 
               ( int* RESTRICT integerRestrictPointerParameter ) {};

       // Note that the storage specifier is only represented in the function 
       // definition and will be dropped from the function declaration
          void publicMemberFunctionIntegerRegisterPointerParamter ( register int* integerPointerParameter ) {};
          void publicMemberFunctionIntegerReferenceParameter ( int & referenceIntegerValueParameter ) {};
          void publicMemberFunctionIntegerConstReferenceParameter 
               ( const int & constReferenceIntegerValueParameter ) {};
          void publicMemberFunctionIntegerConstParameter  ( const int constIntegerValueParameter ) {};
          virtual void publicVirtualMemberFunction () {};
          virtual void publicPureVirtualMemberFunction () = 0;
          friend  void publicFriendMemberFunction () {};
          friend  inline void publicFriendInlineMemberFunction () {};
#endif

#if 1
       // Error: Templated member functions are unparsed outside of their class
          EXPORT template < class T > void publicTemplateMemberFunction () {};
#endif
#if 1
          typedef long* longPointer;
#endif
      protected:
#if 1
          double protectedDoubleValue;
#endif
#if TEST_CONST_MEMBERS
          const double protectedConstDoubleValue;
#endif
#if TEST_STATIC_MEMBERS
          static double protectedStaticDoubleValue;
#endif

#if 1
          void protectedMemberFunction () {};
          void protectedMemberFunctionConst () const {};
          void protectedMemberFunctionVolatile () volatile {};
#endif

      private:
#if 1
          double privateDoubleValue;
#endif
#if TEST_CONST_MEMBERS
          const double privateConstDoubleValue;
#endif
#if TEST_STATIC_MEMBERS
          static double privateStaticDoubleValue;
#endif

#if 1
          void privateMemberFunction () {};
          void privateMemberFunctionConst () const {};
          void privateMemberFunctionVolatile () volatile {};         
#endif
   };

#if TEST_INLINING
// No inline specifier was used in the declaration in the class so none appears here
void classType::noninlinePublicMemberFunctionWithoutDefinition () {}

// No inline specifier in the in class declaration (but it should appear in the unparsed code)
inline void classType::inlinePublicMemberFunctionWithoutDefinitionSpecifiedInDefnOnly () {}

// No inline specifier in the in class definition (but it should appear in the declaration of the unparsed code)
void classType::inlinePublicMemberFunctionWithoutDefinitionSpecifiedInDeclOnly () {}

// "inline" specification is optional here if it appears in the declaration in the class
inline void classType::inlinePublicMemberFunctionWithoutDefinition () {}
#endif

#if TEST_STATIC_MEMBERS
// intializers for const members (not required by g++)
double classType::publicStaticDoubleValue    = 0;
double classType::protectedStaticDoubleValue = 0;
double classType::privateStaticDoubleValue   = 0;
#endif
#endif

#if 1
class classTypeWithPureVirtualMember
   {
     public:
          virtual void publicPureVirtualMemberFunction () = 0;
   };

class classTypeWithoutBaseClass
   {
     public:
          int x;
   };

class classTypeWithPublicBaseClass : public classTypeWithoutBaseClass
   {
     public:
          int y;
   };

class classTypeWithProtectedBaseClass : protected classTypeWithoutBaseClass
   {
     public:
          int y;
   };

class classTypeWithPrivateBaseClass : private classTypeWithoutBaseClass
   {
     public:
          int y;
   };

class classTypeWithVirtualPublicBaseClass : virtual public classTypeWithoutBaseClass
   {
     public:
          int y;
   };

class classTypeWithVirtualProtectedBaseClass : virtual protected classTypeWithoutBaseClass
   {
     public:
          int y;
   };

class classTypeWithVirtualPrivateBaseClass : virtual private classTypeWithoutBaseClass
   {
     public:
          int y;
   };

class classTypeWithoutBaseClassA { public: int x; };
class classTypeWithoutBaseClassB { public: int y; };
class classTypeWithoutBaseClassC { public: int z; };
class classTypeWithMultipleVirtualBaseClasses : virtual public    classTypeWithoutBaseClassA,
                                                virtual protected classTypeWithoutBaseClassB,
                                                virtual private   classTypeWithoutBaseClassC
   {
     public:
          int a;
   };
#endif

#if 1
template < class T >
class templateClassType
   {
     typename T::X x;
   };
#endif


#if 1
extern int externVariable;
extern const int externConstVariable;
const int externConstVariableWithInitializer = 42;

// No other string recognized in g++, that I could discover
extern "C"   int externCvariable;
extern "C++" int externCppvariable;

extern "C"   void externCfunction () {}
extern "C++" void externCppfunction () {}

// Not permited without string
extern "C"
   {
     int externCscopeVariable;
   }

extern "C++"
   {
     int externCppscopeVariable;
   }
#endif

#if 1
// Make this a valid linkable program
int main()
   {
     return 0;
   }
#endif






