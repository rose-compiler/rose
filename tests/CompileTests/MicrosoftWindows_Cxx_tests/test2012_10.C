template <class T>
class A
   {
     public:
          class B
             {
               public:
#if 1
                    B(int x)
                       {
                       }
                   
                    B operator++( int x)
                       {
                         return *this;
                       }

                    bool operator!=(const B & X)
                       {
                         return false;
                       }
#else
                    B(int x);
#if 1
                    B operator++( int x);
                    bool operator!=(const B & X);
#endif
#endif
             };
   };

// This should be: A<int>::B x = 0;
A<int>::B x = 0;

#if 1
int main()
   {
     A<int>::B x = 0;

#if 1
  // DQ (11/19/2004): Temporarily commented out since this is a demonstrated bug now that we qualify everything!
     A<int> integerList;

     for (A<int>::B i = 0; i != 0; i++)
        {
       // sumOverList += *i;
        }
#endif
   }
#endif
