typedef __SIZE_TYPE__ 	size_t;

template<typename T, typename Y = char>
class make_array_helper;
        
template<typename T, size_t N>
class make_array_helper<T> {

    template<typename U>
    struct rebind {
        typedef make_array_helper<T[N]> other;
    };
};
