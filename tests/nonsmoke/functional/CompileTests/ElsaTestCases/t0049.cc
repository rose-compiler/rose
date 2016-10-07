// cc.in49
// friend decls

// Restricted to versions of GNU 3.4 and earlier
#if ( (__GNUC__ == 3) && (__GNUC_MINOR__ <= 4) )
class Foo {
  typedef int Integer;
  friend int f(Integer i);
};

int main()
{
  return f(3);
}
#endif
