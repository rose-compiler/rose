// non-defining declaration before the definition
template<typename T> class X;

template<typename T> class X
   {
     public:
       // note qualified name X::X_int (which is a type alias) does not conflict 
       // with ::X_int (which is a variable) it would not be a problem if they 
       // were both type aliases or both variable names.
          typedef X<int> X_int;
   };

// non-defining declaration after the definition
template<typename T> class X;

#if 0
// If this is uncommented then we get a SgTemplateInstantiation with two parents
// The solution is to not share the declaration in the typedef with the declaration for this 
// variable declaration!  In this case there would be perhaps two non-defining declarations
// (likely only one) but still only one defining declaration.  All would reference the
// single definition which has only the defining declaration as an associated declaration.
X<int> X_int;
#endif

#if 1
// Specializations
template<> class X<char> {};
template<> class X<long> {};

X<char> X_char;
X<long> X_long;
#endif
