// This test code demonstrates that with the testTemplates translator
// the specialization of __gnu_cxx::new_allocator<char> needs to be
// output in the the namespace __gnu_cxx.  This must be handled in
// the unparser, or as a fixup of the AST.

// Think I would like to handle it via a transformation before the unparser.

namespace __gnu_cxx
   {
     template < typename _Tp >
     class new_allocator
        {
        };
   }

template < typename _Tp >
class allocator : public __gnu_cxx::new_allocator<_Tp>
   {
   };

template<> class allocator< char  > : public __gnu_cxx::new_allocator< char> 
   {
   };


extern template class allocator< char  > ;
