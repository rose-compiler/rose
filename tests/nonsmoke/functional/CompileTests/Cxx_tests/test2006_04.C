#if 0
When compiling the code
class X{
public:
void foo(){ ::operator new(1); }

};
ROSE gives the following error message:
rose_test.C: In member function "void X::foo()":
rose_test.C:8: error: expected identifier before ";" token

The problem is that the unparser unparses "::operator new(1)" as ":: new(1)".

Andreas
#endif


class X
   {
     public:
          void foo1(){ ::operator   new(1); }
//        void foo2(){ ::new(1); }

   };

#if 1
// This function shows how the two function calls to new are represented differently in the AST
// The first time as a SgNewExp IR node and the second time as a function call.
void
foobar()
   {
  // This has a call to a SgNewExp
     X* ptr = new X; 
     void* voidPointer = 0L;

  // While this has a call to a SgFunctionCallExp (since it is called with "operator new" syntax)
     ::operator new(1);
     ::operator delete(voidPointer);
   }
#endif

#if 1
// See if this also happens for member functions called two different ways!
class Y
   {
     public:
       // DQ (12/4/2006): This is a bug specific to 3.4.x or greater. there are 3 issues: 
       //      1) the the return value of operator new can not be NULL, and 
       //      2) the parameter type much match size_t (though I had to use "unsigned long" to make it work).
       //      3) the error message from GNU is misleading "declaration of `operator new' as non-function"
       // void* operator new (unsigned int size) { return 0L; }
       // void* operator new (std::size_t size) { void* value; return value; }
       // void* operator new (unsigned long size) { void* value; return value; }
          void* operator new (__SIZE_TYPE__ size) { void* value; return value; }

       // void foo2(){ ::new(1); }
   };

void
foobar2()
   {
  // This has a call to a SgNewExp (only yhe scope appears to link it the "Y"
     Y* ptr = new Y;

  // While this has a call to a SgFunctionCallExp using SgMemberFunctionRefExp (since it is called with "operator new" syntax)
     Y::operator new(1);
   }
#endif


