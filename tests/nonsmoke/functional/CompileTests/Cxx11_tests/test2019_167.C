// Name qualification using a single qualified name for the type is not
// sufficient when the name qualificaion is on a teplate instantiation 
// that has template arguments that require name qualification.  I think
// that the computation of the qualified name is robust, but the storage 
// is what is an issue.  We can only store one qualifier where we actaully 
// need many (for the template arguments).

namespace Iterators 
   {
     typedef int Integer;
     template<typename Type> class numeric_iterator {};
   }

void foobar()
   {
  // Simple example where name qualification support is not general enough.
  // using iterator = Iterators::numeric_iterator<int>;
     using iterator = Iterators::numeric_iterator<Iterators::Integer>;
   };

