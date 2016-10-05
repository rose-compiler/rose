// This example is collected and corrected from http://msdn.microsoft.com/msdnmag/issues/02/02/ModernC/
// It shows the use of covariant return types in virtual functions.
// The support for this in ROSE is in the recording of the "SgType* p_orig_return_type;"
// data member in the SgFunctionType IR node.  The for this to work the symbol table
// for function types is used to store all function type.  The idea is that all covariant
// function types would match to the same symbol in the function type table.  To do this
// either imposes some constraints on the type mangling (as I currently understand this subject),
// or requires some special analysis to identify overloaded virtual functions (this later approach
// might be easier to maintain long-term; decoupled from the objectives of name mangling).

// If the mangled name support were to support identification of covariant return type 
// virtual functions then we would require:
//  1) mangled names of virtual functions should resolve to the same name (which would seem to imply \
//     that return types should not be included within the name mangling).
//  2) mangled names of virtual functions should be expressed in there base class for scope information 
//     to match.  This seems to be counter to the idea behind name mangling. So this is why I think
//     that this sort of support for covariant return types should be a separate piece of work.


// This code work fine with ROSE if USING_COVARIANT_RETURN_TYPE is set to 0, it fails
// in the compilation of the generated code if USING_COVARIANT_RETURN_TYPE is set to 1.
#define USING_COVARIANT_RETURN_TYPE 0

class X
   {
     public:
          virtual X *copy() = 0;
   };

class Xprime : public X
   {
     public:
#if USING_COVARIANT_RETURN_TYPE
       // Version using covariant return type (invariant)
          virtual Xprime* copy();
#else
       // Version not using covariant return type (invariant)
          virtual X* copy();
#endif
   };

// Build a Xprime
Xprime* x = new Xprime;

#if USING_COVARIANT_RETURN_TYPE
// Using covariant return types the following is possible
// ok: implicit conversion ...
X *var1 = x->copy();

// ok: no conversion necessary ...
Xprime *var2 = x->copy();
#else
// error: illegal assignment ...  (without covaiant return type)
// Xprime *var3 = x->copy();

// ok: required explicit cast ... (work around to handle invariant return type)
Xprime *var4 = static_cast<Xprime*>(x->copy());
#endif

