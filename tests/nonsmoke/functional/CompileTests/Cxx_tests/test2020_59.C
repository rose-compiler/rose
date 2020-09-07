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



