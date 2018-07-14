template <typename... Ts>

struct list 
   {
     using type = list;
   };

template <typename T>
struct as_list_s;

template <typename T>
using as_list = typename as_list_s<T>::type;

#if __GNUC__ == 4 && __GNUC_MINOR__ > 8 || __GNUC__ > 4
template <typename T>
using as_list = typename as_list_s<T>::type;
#endif
