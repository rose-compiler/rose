// Original code:
//      b.foobar(0);
// Unparsed (generated) code:
//      b.foobar(((class B < int > ::BB< B < int > ::privateType  > *)0));
// but this is an error since "B<int>::privateType" is private!

// Solution: I guess we should out output default arguments
// Since the following appears to work:
//      b.foobar(((class B < int > ::BB<> *)0));

template<typename T1>
class B
   {
     private:
  // public:
          typedef int privateType;

     public:
          template<typename T2 = privateType> class BB {};

     public:
          void foobar(BB<> *b) {}
   };


int main()
   {
     B<int> b;

  // This unparses to:
  // b.foobar(((class B < int > ::BB< B < int > ::privateType  > *)0));
  // but this is an error since "B<int>::privateType" is private!
     b.foobar(0);

     return 0;
   }

