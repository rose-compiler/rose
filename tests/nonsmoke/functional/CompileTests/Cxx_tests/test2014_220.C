// This example demonstrated the use of temple temple parameters.
// More importantly it demonstrates the unparsing the the SgTemplateMemberFunctionDeclaration.

template <typename S> class YYY {};

template <template < typename ABC_TYPE> class DATACLASS, typename TYPE>
class XXX 
   {
     public:
          XXX() {};
   };

void foobar()
   {
  // Force the constructor to be called so that we will force the output 
  // of the SgTemplateMemberFunctionDeclaration in the generated code.
     XXX< YYY , long> x;
   }
