template < typename T > 
class ISpace
   {
     public:
          class iterator
             {
               protected:
                    int _max;

               public:
                    iterator( )
                       {
                         _max = 10;
                       }
             };
   };

int main()
   {
     ISpace<int> space;
     ISpace<int>::iterator space_iterator;
     return 0;
   }
