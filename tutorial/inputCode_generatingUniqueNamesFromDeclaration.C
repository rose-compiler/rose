// Input file to test mangling of SgInitializedName objects.

int x;

// Global class
class A
   {
     private:
          int x;
       // Nested class
          class B
             {
               private:
                    int x;
               public:
                    void foo (int x_arg) { int x; }
             };
   };

template <typename T>
void
foo (T x_arg)
   {
     T x;
     for (x = 0; x < 10; x++)
        {
          T x = 0;
          do {
            // Local class
               class A
                  {
                    private:
                      // Nested class
                         class B
                            {
                              T x;
                            };
                    public:
                         void foo (T x) {}
                  };
               T x = 0;
             }
          while (x > 0);

          do {
               T x = 0;
             }
          while (x > 0);

       // Nested scope
             {
               T x = 0;
             }
        }
   }

template void foo<int> (int x);
template void foo<double> (double x);

void bar (void)
   {
     for (int x = 0; x != 0; x++)
          for (int x = 0; x != 0; x++)
               for (long x = 0; x != 0; x++)
                    ;
     try {
       for (int x = 0; x != 0; x++) ;
     }
     catch (int) {}
     catch (char x) {}
   }


