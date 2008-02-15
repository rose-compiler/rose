// extern "C" void OUT__1__7037__(int *resultp__,const void *this__ptr__p__);
void OUT__1__7037__(int *resultp__,const void *this__ptr__p__);

namespace N
   {
  // If the function is defined local to the namespace then we don't require the "::" qualification in the friend declaration
  // void OUT__1__7037__(int *resultp__,const void *this__ptr__p__);

     class A 
        {
       // public: friend void ::N::OUT__1__7037__(int *resultp__,const void *this__ptr__p__);
          public: friend void ::OUT__1__7037__(int *resultp__,const void *this__ptr__p__);

          private:
               inline int N::A::foo() const
                  {
                    return 7;
                  }


               inline int N::A::bar() const
                  {
                    return (this) -> foo() / 2;
                  }


          public:
               inline int N::A::biz() const
                  {
                    int result = 0;
                       {
                         const class A *this__ptr__ = this;
                         OUT__1__7037__(&result,&this__ptr__);
                       }
                    return result;
                  }

        };
   }

extern "C" { int printf(const char *fmt, ... ); }

int main()
   {
     class N::A x;
  // Prints '168'
     printf("%d\n",x.biz());
     return 0;
   }


// extern "C" void OUT__1__7037__(int *resultp__,const void *this__ptr__p__)
// void N::OUT__1__7037__(int *resultp__,const void *this__ptr__p__)
void OUT__1__7037__(int *resultp__,const void *this__ptr__p__)
   {
     int &result =  *((int *)resultp__);
     const class N::A *&this__ptr__ =  *((const class N::A **)this__ptr__p__);
     for (int i = 1; i <= this__ptr__ -> foo(); i++)
        {
          for (int j = 1; j <= this__ptr__ -> bar(); j++)
             {
               result += (i * j);
             }
        }
   }

