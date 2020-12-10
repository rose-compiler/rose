// This is a copy of Cxx_tests/test2018_51.C
// It is a simple example of what can fail when we use the fix to test2020_91.C (RC-107).
// The test code test2020_91.C (RC-107) has a more complete explaination of the problem.

template <typename T>
class map
   {
     class private_struct;

     public:
      // This case fails because map::iterator is translated to the private typedef base type.
         typedef private_struct* iterator;
   };

class foobar_class
   {
     public:
          void foobar_member_function()
             {
            // This will fail (seems it has to be a part of a template).
            // It should output: "map<int>::iterator it;"
               map<int>::iterator it;
            // map::iterator it;
             }
   };



