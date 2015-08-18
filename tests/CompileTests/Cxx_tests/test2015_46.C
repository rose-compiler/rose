// This class is required.
template < typename _Tp >
class new_allocator
   {
   };

template < typename _Tp >
class allocator : public new_allocator<_Tp>
   {
   };

// Force the generation of a template instantiation using "char"
// then using the testTemplates translator these instantations 
// will be output in the generated code.  Using the testTranslator
// the instantiations will not be output and just the error using
// "template<>" instead of "template" will be noticed.

// This unparses as: "extern template<> class allocator<char>;"
// which should not include the extra "<>".
extern template class allocator< char  > ;

