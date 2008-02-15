// dsw: This one passes in g++ but fails in elsa because at the end of
// a translation unit we are instantiating a function member of a
// template class in elsa that need not have been instantiated because
// it was never called

class A;

enum enum0 {kNextFragment};

template < class CharT > struct nsWritableFragment {};

template < class CharT > struct nsStringTraits {
  typedef A abstract_string_type;
};

template < class CharT > class nsWritingIterator {
  typedef nsWritableFragment < CharT > fragment_type;
  typedef typename nsStringTraits <
    CharT >::abstract_string_type abstract_string_type;
  fragment_type mFragment;
  CharT *mPosition;
  abstract_string_type *mOwningString;
  inline void normalize_forward ();
};

template < class CharT >
inline void nsWritingIterator <CharT>::normalize_forward ()
{
  mOwningString->GetWritableFragment (mFragment, kNextFragment);
}

class A {
public:typedef unsigned short char_type;
  typedef nsWritingIterator < char_type > iterator;
};
