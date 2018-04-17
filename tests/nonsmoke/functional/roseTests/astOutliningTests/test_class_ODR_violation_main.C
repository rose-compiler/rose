void OUT__1__7421__(void **__out_argv);

namespace N
   {
     class A
        {
          public:
       // private:
               int foo() const
                  {
                    return 7;
                  }

               inline int bar() const
                  {
                    biz();
                    return (this) -> foo() / 2;
                  }

          public:
               int biz () const
                  {
                 // Construct an ODR violation.
                    biz();

                    int result = 0;
                    const class A *this__ptr__ = this;
                    void *__out_argv1__1527__[2];
                    *(__out_argv1__1527__ + 0) = ((void *)(&this__ptr__));
                    *(__out_argv1__1527__ + 1) = ((void *)(&result));
                    OUT__1__7421__(__out_argv1__1527__);

                 // Make a recursive reference to "biz()"
                    this__ptr__->biz();
                    return 0;
                  }
        };
   }

int
main ()
   {
     N::A x;
     x.biz();
     return 0;
   }
