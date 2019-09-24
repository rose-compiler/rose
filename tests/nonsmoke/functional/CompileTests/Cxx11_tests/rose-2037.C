template<typename _Tp>
struct remove_reference
{
    typedef _Tp   type;
};

template<typename _Tp>
constexpr _Tp&&
forward(typename remove_reference<_Tp>::type& __t) {
  return static_cast<_Tp&&>(__t);
}

template<typename _Tp>
constexpr typename remove_reference<_Tp>::type&&
move(_Tp&& __t) {
  return static_cast<typename remove_reference<_Tp>::type &&>(__t);
}

template <typename T>
struct return_type
        : return_type<decltype(&T::operator())>
{};

template <typename ClassType, typename ReturnType, typename... Args>
struct return_type<ReturnType(ClassType::*)(Args...) const>
{
    using type = ReturnType;
};

template < typename func_type >
class Class_1
{
public:
    Class_1(int len, func_type&& func);
private:
    func_type m_func;
};


template < typename func_type >
Class_1<func_type>::Class_1(int len, func_type&& func)
            : m_func(move(func))
    { }

template <typename func_type,
          typename func_value_type = typename remove_reference<func_type>::type,
          typename return_type = typename return_type<func_value_type>::type >
typename ::Class_1<func_value_type>
method_2(func_type&& func)
{
    return {0, forward<func_type>(func)};
}

void method_1()
{
auto local_1 = ::method_2< >( [&] (int ns)
{
});
}
