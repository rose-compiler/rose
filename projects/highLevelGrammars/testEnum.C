
enum ijk_enum { truth = 1, falsehood = 7 };

class A
   {
     public:
          enum abc_enum { truth = ::truth, falsehood = 7 };
   };

class X
   {
     public:
          enum xyz_enum { truth = 1, falsehood = 7 };
   };

int main ()
   {
     return 0;
   }
