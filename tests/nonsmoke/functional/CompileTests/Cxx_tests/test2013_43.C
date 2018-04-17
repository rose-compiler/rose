
class X
   {
     public:
          X& operator<<(bool b);
   };

X x;

int main() 
   {
  // This is unparsed incorrectly as: "x << true == false;"
     x << ( true == false );
   }
