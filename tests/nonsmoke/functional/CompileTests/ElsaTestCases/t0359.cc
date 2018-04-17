// t0359.cc
// problem with dependentness of 'this'; the problem only recently
// arose because we recently added the insertion of otherwise
// implicit uses of 'this'

class HashTable
{
};

struct HashTableIter {
  HashTableIter(HashTable &t);
};

template < class T >
class OwnerHashTable
{
  HashTable table;
  void empty (int initSize);
};

template < class T >
void OwnerHashTable
  <T>
  ::empty (int initSize)
{
  HashTableIter iter (table);
}
