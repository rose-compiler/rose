// #include <boost/wave.hpp>

// #include <boost/wave/cpp_context.hpp>
// #include <boost/wave/util/cpp_include_paths.hpp>
// #include <boost/multi_index_container.hpp>
// #include <boost/multi_index/member.hpp>

#include <boost/multi_index/detail/access_specifier.hpp>
#include <boost/multi_index/detail/base_type.hpp>
#include <boost/utility/base_from_member.hpp>
#include <boost/multi_index/detail/archive_constructed.hpp>

namespace boost{
namespace multi_index{

template<typename Value,typename IndexSpecifierList,typename Allocator>
class multi_index_container:
  private ::boost::base_from_member<
    typename boost::detail::allocator::rebind_to<
      Allocator,
      typename detail::multi_index_node_type<
        Value,IndexSpecifierList,Allocator>::type
    >::type>,
  BOOST_MULTI_INDEX_PRIVATE_IF_MEMBER_TEMPLATE_FRIENDS detail::header_holder<
    typename detail::prevent_eti<
      Allocator,
      typename boost::detail::allocator::rebind_to<
        Allocator,
        typename detail::multi_index_node_type<
          Value,IndexSpecifierList,Allocator>::type
      >::type
    >::type::pointer,
    multi_index_container<Value,IndexSpecifierList,Allocator> >,
  public detail::multi_index_base_type<
    Value,IndexSpecifierList,Allocator>::type
   {};

} /* namespace multi_index */
} /* namespace boost */

// **********************************************************
namespace boost{
namespace multi_index{
namespace detail{

template<class Class,typename Type,Type Class::*PtrToMember>
struct const_member_base
{
  typedef Type result_type;
};

template<class Class,typename Type,Type Class::*PtrToMember>
struct non_const_member_base
{
  typedef Type result_type;
};

} /* namespace multi_index::detail */

#if 1
template<class Class,typename Type,Type Class::*PtrToMember>
struct member:
  mpl::if_c<
    is_const<Type>::value,
    detail::const_member_base<Class,Type,PtrToMember>,
    detail::non_const_member_base<Class,Type,PtrToMember>
  >::type
{
};
#endif

} /* namespace multi_index */
} /* namespace boost */



// *******************************************
namespace boost{
namespace multi_index{
namespace detail{

template<typename Node>
class bidir_node_iterator :
  public bidirectional_iterator_helper<
    bidir_node_iterator<Node>,
    typename Node::value_type,
    std::ptrdiff_t,
    const typename Node::value_type*,
    const typename Node::value_type&>
{
};

template<int N>struct uintptr_candidates;
template<>struct uintptr_candidates<-1>{typedef unsigned int           type;};
template<>struct uintptr_candidates<0> {typedef unsigned int           type;};
template<>struct uintptr_candidates<1> {typedef unsigned short         type;};
template<>struct uintptr_candidates<2> {typedef unsigned long          type;};

template<>struct uintptr_candidates<3> {typedef boost::ulong_long_type type;};
template<>struct uintptr_candidates<4> {typedef unsigned int           type;};

#if 1
struct uintptr_aux
{
#if 1
  BOOST_STATIC_CONSTANT(int,index=
    sizeof(void*)==sizeof(uintptr_candidates<0>::type)?0:
    sizeof(void*)==sizeof(uintptr_candidates<1>::type)?1:
    sizeof(void*)==sizeof(uintptr_candidates<2>::type)?2:
    sizeof(void*)==sizeof(uintptr_candidates<3>::type)?3:
    sizeof(void*)==sizeof(uintptr_candidates<4>::type)?4:-1);

  BOOST_STATIC_CONSTANT(bool,has_uintptr_type=(index>=0));
  typedef uintptr_candidates<index>::type type;
#endif
};

typedef mpl::bool_<uintptr_aux::has_uintptr_type> has_uintptr_type;
typedef uintptr_aux::type                         uintptr_type;
#endif


enum ordered_index_color{red=false,black=true};
enum ordered_index_side{to_left=false,to_right=true};

template<typename Allocator> struct ordered_index_node_impl; /* fwd decl. */

template<typename Allocator> struct ordered_index_node_std_base
{
  typedef typename prevent_eti<
    Allocator,
    typename boost::detail::allocator::rebind_to<
      Allocator,
      ordered_index_node_impl<Allocator>
    >::type
  >::type::pointer                                pointer;
  typedef typename prevent_eti<
    Allocator,
    typename boost::detail::allocator::rebind_to<
      Allocator,
      ordered_index_node_impl<Allocator>
    >::type
  >::type::const_pointer                          const_pointer;

  typedef ordered_index_color&                    color_ref;
  typedef pointer&                                parent_ref;
};

template<typename Allocator>
struct ordered_index_node_compressed_base
{
  typedef ordered_index_node_impl<Allocator>*       pointer;
  typedef const ordered_index_node_impl<Allocator>* const_pointer;
};

template<typename Allocator>
struct ordered_index_node_impl_base:

#if 1
  mpl::if_c<
    !(has_uintptr_type::value)||
    (alignment_of<ordered_index_node_compressed_base<Allocator> >::value%2)||
    !(is_same<
      typename prevent_eti<
        Allocator,
        typename boost::detail::allocator::rebind_to<
          Allocator,
          ordered_index_node_impl<Allocator>
        >::type
      >::type::pointer,
      ordered_index_node_impl<Allocator>*>::value),
    ordered_index_node_std_base<Allocator>,
    ordered_index_node_compressed_base<Allocator>
  >::type
{};
#endif

#if 1
template<typename Allocator>
struct ordered_index_node_impl:ordered_index_node_impl_base<Allocator>
{
};
#endif

#if 1
template<typename Super>
struct ordered_index_node_trampoline:
  prevent_eti<
    Super,
    ordered_index_node_impl<
      typename boost::detail::allocator::rebind_to<
        typename Super::allocator_type,
        char
      >::type
    >
  >::type
{
  typedef typename prevent_eti<
    Super,
    ordered_index_node_impl<
      typename boost::detail::allocator::rebind_to<
        typename Super::allocator_type,
        char
      >::type
    >
  >::type impl_type;
};
#endif

#if 1
template<typename Super>
struct ordered_index_node : Super , ordered_index_node_trampoline<Super>
{
private:
  typedef ordered_index_node_trampoline<Super> trampoline;

public:
  typedef typename trampoline::parent_ref    impl_parent_ref;
  typedef typename trampoline::pointer       impl_pointer;

  impl_parent_ref parent();
//  static ordered_index_node* from_impl(impl_pointer x);
};
#endif


template<typename Node,typename KeyFromValue,typename CompatibleKey,typename CompatibleCompare>
inline Node* ordered_index_find(Node* top,Node* y,const KeyFromValue& key,const CompatibleKey& x,const CompatibleCompare& comp) {  return y; }


struct ordered_unique_tag{};
struct ordered_non_unique_tag{};

template<typename KeyFromValue,typename Compare,typename SuperMeta,typename TagList,typename Category >
class ordered_index  : BOOST_MULTI_INDEX_PROTECTED_IF_MEMBER_TEMPLATE_FRIENDS SuperMeta::type
{
  typedef typename SuperMeta::type                   super;

protected:
  typedef ordered_index_node<typename super::node_type> node_type;

public:
  typedef typename node_type::value_type             value_type;
  typedef KeyFromValue                               key_from_value;
  typedef Compare                                    key_compare;
  typedef bidir_node_iterator<node_type>             iterator;
  typedef iterator                                   const_iterator;
  typedef TagList                                    tag_list;

protected:
  typedef typename mpl::push_front<typename super::index_type_list,ordered_index>::type index_type_list;

public:
  iterator               begin();
  iterator               end();
  template<typename CompatibleKey> iterator find(const CompatibleKey& x) const;
  const_iterator make_iterator(node_type* node)const;
};

} /* namespace multi_index::detail */

/* ordered_index specifiers */
#if 1
template<typename Arg1,typename Arg2,typename Arg3>
struct ordered_unique
{
  typedef typename detail::ordered_index_args<Arg1,Arg2,Arg3> index_args;
  typedef typename index_args::tag_list_type::type tag_list_type;
  typedef typename index_args::key_from_value_type key_from_value_type;
  typedef typename index_args::compare_type        compare_type;
#if 1
  template<typename Super>
  struct node_class
  {
    typedef detail::ordered_index_node<Super> type;
  };
#endif
#if 1
  template<typename SuperMeta>
  struct index_class
  {
    typedef detail::ordered_index<
      key_from_value_type,compare_type,
      SuperMeta,tag_list_type,detail::ordered_unique_tag> type;
  };
#endif
};
#endif

#if 1
template<typename Arg1,typename Arg2,typename Arg3>
struct ordered_non_unique
{
  typedef detail::ordered_index_args<Arg1,Arg2,Arg3> index_args;
  typedef typename index_args::tag_list_type::type tag_list_type;
  typedef typename index_args::key_from_value_type key_from_value_type;
  typedef typename index_args::compare_type        compare_type;
#if 1
  template<typename Super>
  struct node_class
  {
    typedef detail::ordered_index_node<Super> type;
  };
#endif
#if 1
  template<typename SuperMeta>
  struct index_class
  {
    typedef detail::ordered_index<key_from_value_type,compare_type,SuperMeta,tag_list_type,detail::ordered_non_unique_tag> type;
  };
#endif
};
#endif

} /* namespace multi_index */
} /* namespace boost */


// **************************************************
namespace boost { namespace wave { namespace util {

//  Tags for accessing both sides of a bidirectional map
struct from {};
struct to {};

template<typename FromType, typename ToType>
struct bidirectional_map
{
    typedef std::pair<FromType, ToType> value_type;
    typedef boost::multi_index::multi_index_container<
      value_type,
      boost::multi_index::indexed_by<
          boost::multi_index::ordered_unique<
              boost::multi_index::tag<from>,
              boost::multi_index::member<value_type, FromType, &value_type::first> 
          >,
          boost::multi_index::ordered_non_unique<
              boost::multi_index::tag<to>,  
              boost::multi_index::member<value_type, ToType, &value_type::second> 
          >
      >
  > type;

};

class include_paths
{
private:
   typedef bidirectional_map<std::string, std::string>::type pragma_once_set_type;

// DQ (8/25/2009): This is required to demonstrate the bug.
   pragma_once_set_type pragma_once_files;
};

}}}   // namespace boost::wave::util





