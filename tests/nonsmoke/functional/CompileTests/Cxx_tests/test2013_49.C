// Need to generate a test code to demonstrate where typedef types can't be 
// resolved and cause instantiated functions to be generated incorrectly.

template <typename T>
class map
   {
     class private_struct;

     public:
      // This case fails in the unparsed code because map::iterator is translated to the private typedef base type.
         typedef private_struct* iterator;
   };

class foobar_class
   {
     public:

          template <class T>
          void foobar( T x)
             {
             }

          void foobar_member_function()
             {
            // This will fail (seems it has to be a part of a template).
            // It should output: "map<int>::iterator it;"
               map<int>::iterator it;
            // map::iterator it;

            // Call the templated function.
               map<int>::iterator x;
               foobar(x);
             }
   };


