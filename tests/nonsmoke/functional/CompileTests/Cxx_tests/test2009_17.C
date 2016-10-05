// DQ (7/26/2009): Note that the test2009_17_WholeAST.dot file does not appear 
// to display the expressions in the expression statements, I don't know why.
// However the dot file for the AST shows all the details of the expression 
// statement AST's just fine. This may be yet another bug? 

// A base class to hold the function pointer type
class X
   {
     public:
          typedef void(X::*MemberFunctionPointerType) (int);
   };

class Y : public X
   {
     public:
       // This is a bug independent of the member function being virtual
       // virtual void MemberFunctionPointer_A (int);
          void MemberFunctionPointer_A (int);

          void foo();
   };


void Y::foo()
   {
     X::MemberFunctionPointerType functionHandlers[10];

  // Initialize array of pointers to member functions

  // A value of type "void (Y::*)(int)" cannot be assigned to an entity of type "X::MemberFunctionPointerType" unless there is an explicit cast.
  // functionHandlers[1] = Y::MemberFunctionPointer_A;

  // This works and is an intermediate step in fixing this bug.
     functionHandlers[0] = (X::MemberFunctionPointerType) 0L;

  // These work but they are not a part of the cast (so this is a simpler case: function pointer expressions).
     Y::MemberFunctionPointer_A;
     &Y::MemberFunctionPointer_A;

  // Simple cast expression
     (X::MemberFunctionPointerType) Y::MemberFunctionPointer_A;

  // This is the harder cases that originally failed 
  // Note: the cast type will be the base type of the typedef, so we could maybe still do better...
     functionHandlers[0] = (X::MemberFunctionPointerType) &Y::MemberFunctionPointer_A;

  // This is a semantically equivalent example to the case above.
     functionHandlers[0] = (X::MemberFunctionPointerType) Y::MemberFunctionPointer_A;
   }

