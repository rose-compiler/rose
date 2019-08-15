// This is reproducer ROSE-36
// 
// This is not directly a problem in Areas, but was identified ando not completely 
// isolated in the process of building a reducer for an issue specific to Ares.

#include <tuple>
#include <ext/alloc_traits.h>

namespace std
{

namespace __detail
{
  template<typename _Value>
    struct _Hash_node
    {
      typedef _Value value_type;
    };

  template<typename _NodeAlloc>
    struct _Hashtable_alloc
    {
    public:
      // Use __gnu_cxx for S_nothrow_move et al.
      using __node_alloc_traits = __gnu_cxx::__alloc_traits<_NodeAlloc>;

      using __value_alloc_type =
    typename __alloctr_rebind<
         _NodeAlloc,
          typename _NodeAlloc::value_type::value_type>::__type;
      using __value_alloc_traits = std::allocator_traits<__value_alloc_type>;
    };
} // namespace __detail

  template<
typename _Value,
typename _Alloc>
    class _Hashtable
    {
      using __node_alloc_type =
    typename __alloctr_rebind<
          _Alloc,
          __detail::_Hash_node<_Value>>::__type;

     public:
      // NEEDED (with __value_alloc_traits and not __node_alloc_traits):
      typedef typename std::allocator_traits<__node_alloc_type>::pointer  pointer;
//      typedef typename __detail::_Hashtable_alloc<__node_alloc_type>::__value_alloc_traits::pointer  pointer;

    public:
      using const_iterator = int;

      // NEEDED:
     _Hashtable&
      operator=(_Hashtable&& __ht)
      noexcept
      // NEEDED:
      (__detail::_Hashtable_alloc<__node_alloc_type>::__node_alloc_traits::_S_nothrow_move())
      {}
    };
} // namespace std

  template<class _Key>
    class unordered_map
    {
      // NEEDED:
      std::_Hashtable<_Key, std::allocator<_Key>>  _M_h;

    public:
      typedef typename std::_Hashtable<_Key, std::allocator<_Key>>::const_iterator    const_iterator;
      // NEEDED:
      unordered_map&
      operator=(unordered_map&&) = default;
    };

// NEEDED:
namespace namespace_1 {
  typedef unordered_map<int>::const_iterator typedef_1;
}

 

#if 0
Error:
identityTranslator[76145] 3.68418s Rose[FATAL]: assertion failed:
identityTranslator[76145] 3.69972s Rose[FATAL]:   /g/g17/charles/code/ROSE/rose-dquinlan/src/frontend/CxxFrontend/EDG/edgRose/edgRose.C:63045
identityTranslator[76145] 3.69982s Rose[FATAL]:   SgDeclarationStatement* EDG_ROSE_Translation::parse_using_declaration(SourceSequenceList&)
identityTranslator[76145] 3.69993s Rose[FATAL]:   required: first_nondefining_declarations_for_functions.find(edg_routine) != first_nondefining_declarations_for_functions.end()
#endif
