namespace std
{
  template<bool, typename _Tp = void>
    struct enable_if
    { };

  template<typename _From, typename _To>
    struct is_convertible
    { };

  template<class _T1, class _T2>
  struct pair
    {
      constexpr pair(const _T1& __a, const _T2& __b) { }

   // template<class _U2, class = typename enable_if<is_convertible<_U2, _T2>::value>::type>
   // constexpr pair(const _T1& __x, _U2&& __y) { }

   // template<class _U2>
   // template<class _U2, class = typename enable_if<is_convertible<_U2, _T2>::value>::type>
   // template<class _U2>
   // template<class _U2, class X = typename _U2::type>
      template<class _U2, class = typename enable_if<is_convertible<_U2, _T2>::value>::type>
      constexpr pair(const _T1& __x, _U2&& __y) { }
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
          void insert(const value_type& __x);
   };
 
void func1()
   {
     map<basic_string<char>, unsigned int> local1;
     local1.insert( map<basic_string<char>, unsigned int>::value_type("", 1) );
   }

namespace namespace1 
   {
     enum enumType1 
        {
          enum_val1
        };

  // const map<basic_string<char>, unsigned int>::value_type const1 = map<basic_string<char>, unsigned int>::value_type("", enum_val1);
  // map<basic_string<char>, unsigned int>::value_type("", enum_val1);

  // const map<basic_string<char>, unsigned int>::value_type const1 = map<basic_string<char>, unsigned int>::value_type("", enum_val1);
  // const map<int, unsigned int>::value_type const1 = map<int, unsigned int>::value_type(42, enum_val1);
  // const map<basic_string<char>, int>::value_type const1 = map<basic_string<char>, int>::value_type("", enum_val1);
     const map<basic_string<char>, unsigned int>::value_type const1 = map<basic_string<char>, unsigned int>::value_type("", enum_val1);

  // enumType1 xxx;

   } // end namespace1

