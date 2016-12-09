template<class U>
class A
   {
     public:
         void f() {}
         
   };

// Skip version 4.x gnu compilers
#if ( __GNUC__ == 3 )

int main()
   {
     A<int> a;

  // DQ (9/5/2005): Commented out to allow tests with AST testing in place. It used to pass just fine!
  // This causes the attribute test to fail because a node is shared (SgTemplateInstantiationMemberFunctionDecl, I think)
     a.f();

     return 0;
   }

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif

