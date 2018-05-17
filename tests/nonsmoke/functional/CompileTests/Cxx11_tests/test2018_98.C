template <typename... Ts>

struct list 
   {
     using type = list;
   };

template <typename T>
struct as_list_s;

template <typename T>
using as_list = typename as_list_s<T>::type;

template <typename T>
using as_list = typename as_list_s<T>::type;

