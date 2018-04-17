
class A
   {
     public:
         template<class T> void g(int y, T u) 
            {
              y = y+u;
            }
   };

int main()
   {
     A a;
     int x,y;
     a.g<int>(x,y);   
   }

