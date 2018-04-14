namespace std
{

  template<typename _Tp>
    struct remove_reference
    { typedef _Tp   type; };

 

  template<typename _Tp>
    constexpr _Tp&&
    forward(typename std::remove_reference<_Tp>::type& __t) noexcept;

 

  template<bool, typename _Tp = void>
    struct enable_if
    { };

 

  template<typename _From, typename _To>
    struct is_convertible
    { };


  template<class _T1, class _T2>
    struct pair
    {
      _T1 first;
      _T2 second;


      constexpr pair(const _T1& __a, const _T2& __b)
     { }


      constexpr pair(const pair&) = default;
      constexpr pair(pair&&) = default;


      template<class _U1, class = typename
            enable_if<is_convertible<_U1, _T1>::value>::type>
     constexpr pair(_U1&& __x, const _T2& __y)
     : first(std::forward<_U1>(__x)), second(__y) { }

      template<class _U2, class = typename
            enable_if<is_convertible<_U2, _T2>::value>::type>
     constexpr pair(const _T1& __x, _U2&& __y)   { }
    };

} // namespace std

 

 

  template<typename _CharT>
    class basic_string
    {
     public:
      basic_string();
      basic_string(const _CharT* __s);
  };

 

 

  template <typename _Key, typename _Tp>
    class map
    {
    public:
      typedef std::pair<const _Key, _Tp> value_type;
      void
      insert(const value_type& __x);
    };

 
void func1()
{
  map<basic_string<char>, unsigned int> local1;
  local1.insert( map<basic_string<char>, unsigned int>::value_type("", 1) );

}

 

namespace namespace1 {

namespace namespace2 {

  class Class1;

}


template<class t_parm1> class t_Class1;

 

typedef t_Class1<namespace2::Class1> typedef1;

 

enum enumType1 {
  enum_val1
};

 

const map<basic_string<char>, unsigned int>::value_type const1 =
      map<basic_string<char>, unsigned int>::value_type("", enum_val1);

 

} // end namespace1

