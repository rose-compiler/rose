#define _GLIBCXX_BEGIN_NAMESPACE(X) namespace X {
#define _GLIBCXX_END_NAMESPACE }

_GLIBCXX_BEGIN_NAMESPACE(std)

   class string {};

   template<class _T1, class _T2> struct pair {};

_GLIBCXX_END_NAMESPACE


_GLIBCXX_BEGIN_NAMESPACE(__gnu_cxx)

  template<bool _Cond, typename _Iftrue, typename _Iffalse>
  struct __conditional_type
  { typedef _Iftrue __type; };

  template<typename _Tp>
    class new_allocator
    {
#if 1
    public:
      typedef long  difference_type;
      typedef _Tp        value_type;
#endif
    };

_GLIBCXX_END_NAMESPACE

#define __glibcxx_base_allocator  __gnu_cxx::new_allocator

_GLIBCXX_BEGIN_NAMESPACE(std)

  template<typename _Tp> class allocator: public __glibcxx_base_allocator<_Tp> {};

_GLIBCXX_END_NAMESPACE


_GLIBCXX_BEGIN_NAMESPACE(std)

  template <class _Tp> struct equal_to {};
  template <class _Tp> struct _Identity {};
  template <class _Pair> struct _Select1st {};

_GLIBCXX_END_NAMESPACE


#define HASH_MAP_CLASS unordered_map
// #define HASH_NAMESPACE std::tr1
#define HASH_SET_CLASS unordered_set

namespace std
{ 
_GLIBCXX_BEGIN_NAMESPACE(tr1)
namespace __detail
{
  template<typename _Value, bool __cache_hash_code> struct _Hash_node;

  template<typename _Value, bool __constant_iterators, bool __cache> struct _Node_iterator {};

  template<typename _Value, bool __cache> struct _Hashtable_iterator_base {};

  template<typename _Value, bool __constant_iterators, bool __cache> struct _Hashtable_iterator {};

  template<typename _Value, bool __constant_iterators, bool __cache> struct _Hashtable_const_iterator {};

  struct _Mod_range_hashing {};

  struct _Default_ranged_hash {};

  struct _Prime_rehash_policy {};

  template<typename _Key, typename _Value, typename _Ex, bool __unique, typename _Hashtable> struct _Map_base {};

  template<typename _RehashPolicy, typename _Hashtable> struct _Rehash_base {};
  template<typename _Hashtable> struct _Rehash_base<_Prime_rehash_policy, _Hashtable> {};

  template<typename _Key, typename _Value,typename _ExtractKey, typename _Equal,typename _H1, typename _H2, typename _Hash,bool __cache_hash_code>
  struct _Hash_code_base;

  template<typename _Key, typename _Value,typename _ExtractKey, typename _Equal,typename _H1, typename _H2, typename _Hash>
  struct _Hash_code_base<_Key, _Value, _ExtractKey, _Equal, _H1, _H2,_Hash, false>
    {
      protected:
           _Hash_code_base(const _ExtractKey& __ex, const _Equal& __eq,const _H1&, const _H2&, const _Hash& __h) {}
    };

} // namespace __detail
_GLIBCXX_END_NAMESPACE
} // namespace std::tr1


namespace std
{ 
_GLIBCXX_BEGIN_NAMESPACE(tr1)

  template<typename _Key, typename _Value, typename _Allocator,typename _ExtractKey, typename _Equal,typename _H1, typename _H2, typename _Hash, typename _RehashPolicy,bool __cache_hash_code,bool __constant_iterators,bool __unique_keys>
    class _Hashtable
    {
    public:
      typedef _Value                                      value_type;
      typedef typename _Allocator::difference_type        difference_type;

      typedef __detail::_Node_iterator<value_type, __constant_iterators,__cache_hash_code> local_iterator;

      typedef __detail::_Hashtable_iterator<value_type, __constant_iterators, __cache_hash_code> iterator;

    private:			// Find, insert and erase helper functions

      typedef typename __gnu_cxx::__conditional_type<__unique_keys,std::pair<iterator, bool>, iterator>::__type _Insert_Return_Type;

      typedef typename __gnu_cxx::__conditional_type<__unique_keys,std::_Select1st<_Insert_Return_Type>,std::_Identity<_Insert_Return_Type> >::__type _Insert_Conv_Type;
    };

_GLIBCXX_END_NAMESPACE
} // namespace std::tr1


namespace std
{
_GLIBCXX_BEGIN_NAMESPACE(tr1)
  template<typename T> struct hash;

_GLIBCXX_END_NAMESPACE
}



namespace std
{
_GLIBCXX_BEGIN_NAMESPACE(tr1)

  template<class _Key, class _Tp,class _Hash = hash<_Key>,class _Pred = std::equal_to<_Key>,class _Alloc = std::allocator<std::pair<const _Key, _Tp> >,bool __cache_hash_code = false>
    class unordered_map
    : public _Hashtable<_Key, std::pair<const _Key, _Tp>, _Alloc,std::_Select1st<std::pair<const _Key, _Tp> >, _Pred, _Hash, __detail::_Mod_range_hashing,__detail::_Default_ranged_hash,__detail::_Prime_rehash_policy,__cache_hash_code, false, true>
    {};

_GLIBCXX_END_NAMESPACE
}

namespace std
{
_GLIBCXX_BEGIN_NAMESPACE(tr1)

// template<class _Value,class _Hash = hash<_Value>,class _Pred = std::equal_to<_Value>,class _Alloc = std::allocator<_Value>,bool __cache_hash_code = false>
   template<class _Value,class _Hash = hash<_Value>,class _Pred = std::equal_to<_Value>,class _Alloc = std::allocator<_Value>,bool __cache_hash_code = false>
    class unordered_set
    : public _Hashtable<_Value, _Value, _Alloc,std::_Identity<_Value>, _Pred,_Hash, __detail::_Mod_range_hashing,__detail::_Default_ranged_hash,__detail::_Prime_rehash_policy,__cache_hash_code, true, true>
    {};

_GLIBCXX_END_NAMESPACE
}

namespace google {
namespace protobuf {

// template <typename Key> struct hash : public HASH_NAMESPACE::hash<Key> {};
   template <typename Key> struct hash : public std::tr1::hash<Key> {};

template <typename Key> struct hash<const Key*> {};

template <typename Key, typename Data, typename HashFcn = hash<Key>, typename EqualKey = std::equal_to<Key> > 
class hash_map 
// : public HASH_NAMESPACE::HASH_MAP_CLASS<Key, Data, HashFcn, EqualKey> 
   : public std::tr1::HASH_MAP_CLASS<Key, Data, HashFcn, EqualKey> 
   {};

template <typename Key, typename HashFcn = hash<Key>, typename EqualKey = std::equal_to<Key> > 
// template <typename Key > 
class hash_set 
// : public HASH_NAMESPACE::HASH_SET_CLASS<Key, HashFcn, EqualKey> 
   : public std::tr1::HASH_SET_CLASS<Key, HashFcn, EqualKey> 
   {};

struct streq {};

}  // namespace protobuf
}  // namespace google

template <typename T> class vector { T x; };
   
namespace google {
namespace protobuf {

class Descriptor;

class FileDescriptor;

class DescriptorPool 
   {
     public:
          class Tables;
   };

}  // namespace protobuf
}  // namespace google


namespace google {
namespace protobuf {

namespace 
   {
     typedef hash_map<const char*, long,hash<const char*>, streq> SymbolsByNameMap;
     typedef hash_map<const char*, const FileDescriptor*,hash<const char*>, streq> FilesByNameMap;

   }  // anonymous namespace

class DescriptorPool::Tables 
   {
     public:
          hash_set<std::string> known_bad_files_;

          hash_set<const Descriptor*> extensions_loaded_from_db_;

     private:
          SymbolsByNameMap      symbols_by_name_;
          FilesByNameMap        files_by_name_;
   };

}  // namespace protobuf
}  // namespace google
