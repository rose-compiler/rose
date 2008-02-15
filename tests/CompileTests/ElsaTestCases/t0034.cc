// cc.in34
// difficulty making connection with forward-declared
// template class

template <class T> class OwnerHashTableIter;

template <class T>
class OwnerHashTable {
public:
  friend class OwnerHashTableIter<T>;
};
