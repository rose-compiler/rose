// This class is required.
template < typename _Tp >
class new_allocator
   {
   };

namespace XXX
   {
     template < typename _Tp >
     class allocator : public new_allocator<_Tp>
        {
        };
   }

// This is an error for GNU g++, but works fine for ROSE (EDG).
// However, the generated ROSE code fails for GNU, because the
// original code fails for GNU.
template<> class XXX::allocator<int> ;

// The fix is to do a transforamtion on the AST before unparsing
// to wrap the template declaration in a namespace.

// Note that using the testTemplates translator and additional
// template instantiation is output:
//    "template<> class allocator< int  > ;"
// This would have to be name qualified for EDG to pass it, but
// because of the limitations of GNU g++ it need the same handling
// as the example above (must be output in the namespace "std".

