// t0386.cc
// qualified nondependent lookup of inherited member

struct QGArray
{
  void duplicate (const QGArray & a);
  void duplicate (const char *d, unsigned len);
};


template < class type > 
class QArray : public QGArray {
public:
  void duplicate (const type * a, unsigned n)
  {
    char const *s;
    QGArray::duplicate (s, n);
  }
};


void foo(QArray<char> &a, char const *s)
{
  a.duplicate(s, 5);
}
