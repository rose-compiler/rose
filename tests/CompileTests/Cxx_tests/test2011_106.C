#define SIMPLE 1
// extern "C" void OUT__1__7816__(void **__out_argv);
// void OUT__1__7816__(void **__out_argv);
// void OUT__1__7816__(void **__out_argv) {}
void OUT__1__7816__(void **__out_argv);

namespace N
   {
     class A 
        {
       // Note that these are private, so the outline function will not 
       // be able to access them. The generated friend function must
       // also be output with global qualification to permit it reference
       // the global scope version of the function instead of generating 
       // a reference to a function that has not been defined with a scope.

       // Error: name qualification does not output global qualifier.
       // The function prototype has been seen, the defining declaration 
       // has not been seen, the unparser generates:
       //    "public: friend void OUT__1__7816__(void **__out_argv);" (missing global qualification).
          friend void ::OUT__1__7816__(void **__out_argv);

       // Bug causes both "protected" and "private" to be a problem.
       // public: inline int foo() const
       // protected: inline int foo() const
          private: inline int foo() const
             {
               return 7;
             }

          public: 
               inline int biz() const
                  {
                 // A declaration for this pointer
                    const class A *this__ptr__ = this;
                 // Build a reference to test the AST copy mechanism.
                    (this) ->  foo ();
                    int result = 0;
                    typedef class A *pointerToA;
                       {
                         void *__out_argv1__1527__[2];
                         __out_argv1__1527__[0] = ((void *)(&this__ptr__));
                         __out_argv1__1527__[1] = ((void *)(&result));
                         OUT__1__7816__(__out_argv1__1527__);
                       }
                     return result;
                  }

        };
   }

#if 1
// I don't think the use of extern "C" is an issue.
// extern "C" void OUT__1__7816__(void **__out_argv)
void OUT__1__7816__(void **__out_argv)
   {
     int &result =  *((int *)__out_argv[1]);
     const class N::A *&this__ptr__ =  *((const class N::A **)__out_argv[0]);
     for (int i = 1; i <= this__ptr__ ->  foo (); i++) 
          result += i;
   }
#endif
