#include <type_traits>

#if 0
struct OwnershipReceiver
{
  template <typename T>
  void receive_ownership(T&& t)
  {
     // taking file descriptor of t, and clear t
  }
};
#endif


struct OwnershipReceiver
{
  template <typename T,
            class = typename std::enable_if
            <
                !std::is_lvalue_reference<T>::value
            >::type
           >
  void receive_ownership(T&& t)
  {
     // taking file descriptor of t, and clear t
  }
};


