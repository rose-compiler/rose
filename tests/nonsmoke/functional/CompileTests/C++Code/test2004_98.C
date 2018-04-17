// This test code shows the importance of having defining and non-defining declarations
// (and there are of course numerous other reasons as well, simplified unparsing, 
// simpler analysis, simpler specification of transformations, etc.).

// Note that in the traversal, we don't see the template declaration first
// so there are limits on the order in which some error checking can be done 
// within the AST. I think this may be a bug since I would want to see the 
// template declarations represented in the order that they appear within 
// the source code! The template declaration appears in the AST as an instantiation
// without any parameters and with the class name set to the template name.
// These are the options that we have set in EDG to force this to happen!
// Thus the first thing that we see is the place-holder default instatiation 
// associated with the template.  Any additional instatiations will follow
// and appear in ther order of declaration.  All instantiations will reference
// a SgTemplateDeclaration IR node which contains the string associated with the
// template.  The advantage of this design is that we have all instatiations in the 
// AST along with the original templat in the AST as well.
template<typename T> class X
   {
     public:
       // note qualified name X::X_int (which is a type alias) does not conflict 
       // with ::X_int (which is a variable) it would not be a problem if they 
       // were both type aliases or both variable names.
          typedef X<int> X_int;
   };

// If this is uncommented then we get a SgTemplateInstantiation with two parents
// The solution is to not share the declaration in the typedef with the declaration for this 
// variable declaration!  In this case there would be perhaps two non-defining declarations
// (likely only one) but still only one defining declaration.  All would reference the
// single definition which has only the defining declaration as an associated declaration.
X<int> X_int;

// Specializations
template<> class X<char> {};
template<> class X<long> {};

X<char> X_char;
X<long> X_long;


#if 1
X<float> X_float;
X<double> X_double;
X<short> X_short;
#endif
