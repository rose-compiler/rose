class function_base;

#if 0
bool operator==(function_base & f, int)
{
  return true;
}
#else
bool operator==(function_base & f, int);
#endif

#if 0
template<typename Functor>
bool operator==(function_base & f, Functor g)
  {
    return true;
  }
#else
template<typename Functor> bool operator==(function_base & f, Functor g);
#endif

template <class Key, class T, class Hash, class Pred, class Alloc>
class unordered_map
    {
      friend bool operator==<Key, T, Hash, Pred, Alloc>( unordered_map &, int );
    };
