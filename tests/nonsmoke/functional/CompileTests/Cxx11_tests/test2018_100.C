#include <tuple>

#include <ext/alloc_traits.h>

// DQ (9/22/2020): This file fails with Intel v17 because the #include <ext/aligned_buffer.h> can 
// not be resolved by the Intel v17.0.4 compiler as it is installed on Jenkins. The issues seem 
// to be that the Jenkins installation of Intel v17 is using the GNU headers and the version
// of GNU headers is 4.8.5 (which is too old).  Installing GNU 6.1 and maybe a later version 
// of boost than 1.57 (I'm using Boost 1.60) should fix the problem.  Nathan is fixing this
// issue and so this file can be added back in later after Jenkins's use of GNU by Intel v17 
// is updated.
// This file is: test2018_100.C
// This is the header file that is not found if the GNU version where Intel v17 is installed 
// is too old. This file compiles fine on my REL7 desktop machine because GNU 6.1 is the GNU
// compiler available in when the Intel v17 compiler is installed.
#include <ext/aligned_buffer.h>

#include <bits/hashtable_policy.h>



namespace std
{

  template<

typename _Tp,

typename _Hash>

    using __cache_default

      =  __not_<__and_<// Do not cache for fast hasher.

                  __is_fast_hash<_Hash>,

                  // Mandatory to have erase not throwing.

                  __detail::__is_noexcept_hash<_Tp, _Hash>>>;

 

  template<

typename _Key,

typename _Value,

typename _Alloc,

typename _ExtractKey,

typename _Equal,

typename _H1,

typename _H2,

typename _Hash,

typename _RehashPolicy,

typename _Traits>

    class _Hashtable

    : public __detail::_Hashtable_base<_Key, _Value, _ExtractKey, _Equal,

                            _H1, _H2, _Hash, _Traits>,

      public __detail::_Map_base<_Key, _Value, _Alloc, _ExtractKey, _Equal,

                     _H1, _H2, _Hash, _RehashPolicy, _Traits>,

      public __detail::_Insert<_Key, _Value, _Alloc, _ExtractKey, _Equal,

                       _H1, _H2, _Hash, _RehashPolicy, _Traits>,

      public __detail::_Rehash_base<_Key, _Value, _Alloc, _ExtractKey, _Equal,

                         _H1, _H2, _Hash, _RehashPolicy, _Traits>,

      public __detail::_Equality<_Key, _Value, _Alloc, _ExtractKey, _Equal,

                     _H1, _H2, _Hash, _RehashPolicy, _Traits>
#if 0
,

      private __detail::_Hashtable_alloc<

     typename __alloctr_rebind<_Alloc,

       __detail::_Hash_node<_Value,

                       _Traits::__hash_cached::value> >::__type>
#endif
    {

      using __hash_cached = typename _Traits::__hash_cached;

      using __node_type = __detail::_Hash_node<_Value, __hash_cached::value>;

#if 0
      using __node_alloc_type =

      typename __alloctr_rebind<_Alloc, __node_type>::__type;

      using __hashtable_alloc = __detail::_Hashtable_alloc<__node_alloc_type>;
 

      using __value_alloc_traits =

     typename __hashtable_alloc::__value_alloc_traits;

      using __node_alloc_traits =

     typename __hashtable_alloc::__node_alloc_traits;
#endif

 

    public:

      typedef _Key                              key_type;

      typedef _Value                            value_type;

      typedef _Alloc                            allocator_type;

      typedef _Equal                            key_equal;

 

      // mapped_type, if present, comes from _Map_base.

      // hasher, if present, comes from _Hash_code_base/_Hashtable_base.

      // typedef typename __value_alloc_traits::pointer        pointer;

 

    private:

      using __hashtable_base = __detail::

                       _Hashtable_base<_Key, _Value, _ExtractKey,

                                 _Equal, _H1, _H2, _Hash, _Traits>;

    public:

      using const_iterator = typename __hashtable_base::const_iterator;

 

  // _Hashtable& operator=(_Hashtable&& __ht) noexcept(__node_alloc_traits::_S_nothrow_move()) {}

    };

 

 

 

_GLIBCXX_END_NAMESPACE_VERSION

} // namespace std

 

 

  template<bool _Cache>

    using __umap_traits = std::__detail::_Hashtable_traits<_Cache, false, true>;

 

  template<

typename _Key,

typename _Tp>

    using __umap_hashtable = std::_Hashtable<_Key, std::pair<const _Key, _Tp>,

                                        std::allocator<std::pair<const _Key, _Tp>>,

                                        std::__detail::_Select1st,

                             std::equal_to<_Key>,

                             std::hash<_Key>,

                             std::__detail::_Mod_range_hashing,

                             std::__detail::_Default_ranged_hash,

                             std::__detail::_Prime_rehash_policy,

                                        __umap_traits<std::__cache_default<_Key, std::hash<_Key>>::value>>;

 

  template<

class _Key,

class _Tp >

    class unordered_map

    {

     __umap_hashtable<_Key, _Tp>  _M_h;

    public:

      typedef typename __umap_hashtable<_Key, _Tp>::const_iterator     const_iterator;

      unordered_map&

      operator=(const unordered_map&) = default;

      unordered_map&

      operator=(unordered_map&&) = default;

    };

 

namespace namespace_1 {

class Class_1
{
public:
   typedef unordered_map<int, int>::const_iterator typedef_1;

};

}

 
