// t0168.cc
// in/big/nsAtomTable.i:9857:36: error: class `nsWritingIterator' isn't a template

template <class CharT>
class nsWritingIterator {
public:
  void f() {
    nsWritingIterator<CharT> result(*this);
  }
};

void f()
{
  nsWritingIterator<int> w;
}
