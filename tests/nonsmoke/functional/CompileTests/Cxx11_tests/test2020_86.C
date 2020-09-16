// ROSE-1687

namespace std
{
    typedef __SIZE_TYPE__ 	size_t;

    template<typename _Tp>
    _Tp&&
    forward(_Tp& __t) noexcept
    { }

    template<typename _Tp>
    _Tp&&
    move(_Tp&& __t) noexcept
    { }

    struct Struct_1 { };

    template<typename...>
    class tuple;

    template<std::size_t...>
    struct _Index_tuple;

    template<class _T1, class _T2>
    struct pair
    {
        typedef _T1 first_type;    /// @c first_type is the first bound type
        typedef _T2 second_type;   /// @c second_type is the second bound type
        _T1 first;                 /// @c first is a copy of the first object
        _T2 second;                /// @c second is a copy of the second object
        template<typename... _Args1, typename... _Args2>
        pair(Struct_1, tuple<_Args1...>, tuple<_Args2...>);
    private:
        template<typename... _Args1, std::size_t... _Indexes1,
                typename... _Args2, std::size_t... _Indexes2>
        pair(tuple<_Args1...>&, tuple<_Args2...>&,
             _Index_tuple<_Indexes1...>, _Index_tuple<_Indexes2...>);
    };

    template<size_t... _Indexes>
    struct _Index_tuple
    {
        typedef _Index_tuple<_Indexes..., sizeof...(_Indexes)> __next;
    };

    template<size_t _Num>
    struct _Build_index_tuple
    {
        typedef typename _Build_index_tuple<_Num - 1>::__type::__next __type;
    };

//NEEDED to avoid error: excessive recursion at instantiation of class "std::_Build_index_tuple:
    template<>
    struct _Build_index_tuple<0>
    {
        typedef _Index_tuple<> __type;
    };

} // namespace

inline void* operator new(std::size_t, void* __p)
{ }

namespace __gnu_cxx {

    template<typename _Tp>
    class new_allocator {
    public:
        template<typename _Tp1>
        struct rebind {
            typedef new_allocator<_Tp1> other;
        };

        template<typename _Up, typename... _Args>
        void
        construct(_Up *__p, _Args &&... __args) {
//NEEDED:
            ::new((void *) __p) _Up(std::forward<_Args>(__args)...);
        }
    };
}

namespace std {

    template<typename _Tp>
    using __allocator_base = __gnu_cxx::new_allocator<_Tp>;

    template<typename... _Elements>
    class tuple {
    };

//NEEDED:
    template<>
    class tuple<> {
    };

    template<std::size_t __i, typename _Tp>
    struct tuple_element;

    template<typename _Head, typename... _Tail>
    struct tuple_element<0, tuple<_Head, _Tail...> > {
        typedef _Head type;
    };

    template<typename _Tp>
    struct __add_ref {
        typedef _Tp &type;
    };

    template<std::size_t __i, typename... _Elements>
    typename __add_ref<
            typename tuple_element<__i, tuple<_Elements...>>::type
    >::type
    get(tuple<_Elements...> &__t) noexcept { }

//NEEDED:
    template<typename... _Elements>
    tuple<_Elements &&...>
    forward_as_tuple(_Elements &&... __args) noexcept { }

//NEEDED:
    template<class _T1, class _T2>
    template<typename... _Args1, typename... _Args2>
    inline
    pair<_T1, _T2>::
    pair(Struct_1,
         tuple<_Args1...> __first, tuple<_Args2...> __second)
            : pair(__first, __second,
                   typename _Build_index_tuple<sizeof...(_Args1)>::__type(),
                   typename _Build_index_tuple<sizeof...(_Args2)>::__type()) { }

//NEEDED:
    template<class _T1, class _T2>
    template<typename... _Args1, std::size_t... _Indexes1,
            typename... _Args2, std::size_t... _Indexes2>
    inline
    pair<_T1, _T2>::
    pair(tuple<_Args1...> &__tuple1, tuple<_Args2...> &__tuple2,
         _Index_tuple<_Indexes1...>, _Index_tuple<_Indexes2...>)
            : first(std::forward<_Args1>(std::get<_Indexes1>(__tuple1))...),
              second(std::forward<_Args2>(std::get<_Indexes2>(__tuple2))...) { }
}

template<typename _Alloc, typename _Tp>
struct __alloctr_rebind {
    typedef typename _Alloc::template rebind<_Tp>::other __type;
};

template<typename _Alloc>
struct allocator_traits {
    template<typename _Tp>
    using rebind_alloc = typename __alloctr_rebind<_Alloc, _Tp>::__type;
private:
    template<typename _Tp, typename... _Args>
    static _Tp _S_construct(_Alloc &__a, _Tp *__p, _Args &&... __args) {
//NEEDED:
        __a.construct(__p, std::forward<_Args>(__args)...);
    }

public:
    template<typename _Tp, typename... _Args>
    static auto construct(_Alloc &__a, _Tp *__p, _Args &&... __args)
    -> decltype(_S_construct(__a, __p, std::forward<_Args>(__args)...)) {
//NEEDED:
        _S_construct(__a, __p, std::forward<_Args>(__args)...);
    }

    template<typename _Tp>
    struct rebind {
        typedef typename allocator_traits<_Alloc>::template rebind_alloc<_Tp> other;
    };
};

typedef void (*func_1)();

class _Rb_tree {
    typedef typename allocator_traits<__gnu_cxx::new_allocator<std::pair<const int, func_1>>>::template rebind<int>::other _Node_allocator;
    typedef allocator_traits<_Node_allocator> _Alloc_traits;
public:
    _Node_allocator &_M_get_Node_allocator() { }

    template<typename... _Args>
    void _M_emplace_hint_unique(_Args &&... __args) {
// NEEDED:
        std::pair<const int, func_1> *some_val_ptr;
        _Alloc_traits::construct(
                _M_get_Node_allocator(),
                some_val_ptr,
                __args...);
    }
};

_Rb_tree _M_t;


constexpr std::Struct_1 global1 = std::Struct_1();

void func_3() {
    _M_t._M_emplace_hint_unique(
            global1,
            std::forward_as_tuple(std::move(0)),
            std::tuple<>());
}

void func_2() {
    func_3();
}


// Gives this error:
// identityTranslator: Cxx_Grammar.C:165091: SgName SgFunctionDeclaration::get_name() const: Assertion `this != __null' failed.

