


namespace boost{
template <class T, T val>
struct integral_constant // : public mpl::integral_c<T, val>
{
   typedef integral_constant<T,val> type;
};
}

namespace boost {
template< typename T > struct is_integral : ::boost::integral_constant<bool,false> { };
}

namespace boost {
namespace type_traits {
template <bool b>
struct ice_not
{
    static const bool value = true;
};
template <>
struct ice_not<true>
{
    static const bool value = false;
};
}
}

namespace boost
{
  template <bool B, class T = void>
  struct enable_if_c {
    typedef T type;
  };
}

namespace boost {
  namespace detail {
    namespace function {
      struct useless_clear_type {};
    }
  }

class function_base
{
   public: bool empty() const { return true; }
};

inline bool operator==(const function_base& f, detail::function::useless_clear_type*)
{
  return f.empty();
}

template<typename Functor>
  typename ::boost::enable_if_c<(::boost::type_traits::ice_not< (::boost::is_integral<Functor>::value)>::value), bool>::type
  operator==(const function_base& f, Functor g)
  {
    return true;
  }
}

namespace boost
   {
     template <class Key, class T, class Hash, class Pred, class Alloc>
     class unordered_map
        {
          friend bool operator==<Key, T, Hash, Pred, Alloc>( unordered_map const&, unordered_map const& );
        };
   }
