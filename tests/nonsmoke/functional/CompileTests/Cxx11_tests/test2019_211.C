// Name qualification using a single qualified name for the type is not
// sufficient when the name qualificaion is on a teplate instantiation 
// that has template arguments that require name qualification.  I think
// that the computation of the qualified name is robust, but the storage 
// is what is an issue.  We can only store one qualifier where we actaully 
// need many (for the template arguments).

namespace Iterators_A
   {
     typedef int Integer;
   }

namespace Iterators_B 
   {
     typedef float Integer;
     template<typename Type1> class Numeric_Iterator {};
   }

template<typename Type1,typename Type2> class Numeric_Iterator {};

// Numeric_Iterator<Iterators_A::Integer,Iterators_B::Integer> foobar ();
Numeric_Iterator<Iterators_A::Integer,Iterators_B::Numeric_Iterator<Iterators_A::Integer> > foobarA ();

Numeric_Iterator<Iterators_A::Integer,Iterators_B::Numeric_Iterator<Iterators_B::Integer> > foobarB ();

