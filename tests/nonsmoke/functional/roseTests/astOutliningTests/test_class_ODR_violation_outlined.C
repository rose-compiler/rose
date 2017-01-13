// This is part of a test code (with test_class_ODR_violation_main.C) to 
// understand how to build outlined code for classes (to avoid ODR violations).

// From this test code we have learned:
//   1) member function defined in the class must be defined in the copied class, 
//      else the member function (e.g. "foo()") will not be defined at link time.
//   2) Oddly enough "biz()" does not need to be defined, I don't know why.

/* OUTLINED FUNCTION PROTOTYPE */
// extern "C" void OUT__1__7421__(void **__out_argv);
void OUT__1__7421__(void **__out_argv);

// This class has only non-defining declarations
namespace N
   {
     class A
        {
       // Make the outlined function a friend so that it will have access to "foo()" and "bar()"
          friend void ::OUT__1__7421__(void **__out_argv);

       // public or private does not make a difference.
          public:
#if 1
            // This will compile and link just fine.
               int foo() const { return 0; };
#else
            // This is an error at link time since "foo()" is not defined.
               int foo() const;
#endif
#if 1
            // This will compile and link just fine.
               int bar() const { return 0; };
#else
            // This is an error at link time since "bar()" is not defined.
               int bar() const;
#endif
          public:
            // I don't know why "biz()" need not be defined!
               int biz () const; // { return 0; };
        };
   }

// extern "C" void OUT__1__7421__(void **__out_argv)
void OUT__1__7421__(void **__out_argv)
   {
     int &result =  *((int *)(__out_argv[1]));
     const class N::A *&this__ptr__ =  *((const class N::A **)(__out_argv[0]));
     for (int i = 1; i <= this__ptr__ -> foo(); i++) 
          for (int j = 1; j <= this__ptr__ -> bar(); j++) 
               result += (i * j);

  // This is now an indirectly recursive reference to "biz()"
     this__ptr__->biz();
   }
