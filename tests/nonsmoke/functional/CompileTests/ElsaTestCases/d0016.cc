// this test was isolated from nsAtomTable.i; a change was made in
// nsAtomTable.i to remove the failure here, but the fix was NOT that
// which I present here; the fix was to just comment out the line
// corresponding to the line at the bottom here where the ctor is
// actually called; this was done because attempting to fix it the way
// I have here somehow allowed another failure, where as the way I
// fixed it removed all failures

struct nsACString;
struct nsAString;
template <class CharT> struct nsStringTraits {
  typedef nsAString abstract_string_type;
};
template <> struct nsStringTraits<char> {
  typedef nsACString abstract_string_type;
};
template <class CharT> struct nsReadingIterator {
  // if you replace this line
  typedef typename nsStringTraits <CharT>::abstract_string_type abstract_string_type;
  // with this line
//    typedef nsACString abstract_string_type;
  // then the test passes; this seems to be due to the way implicit
  // copy ctors are added to classes; the line marked below is calling
  // a copy ctor that seems to be missing

  const abstract_string_type & string () const {}
};
struct nsACString {
  typedef nsACString abstract_string_type;
  typedef nsReadingIterator<char> const_iterator;
};
struct nsDependentCSubstring:nsACString {
  const abstract_string_type & mString;
  nsDependentCSubstring
    (const const_iterator & aStart,
     const const_iterator & aEnd)
     // it seems that the copy ctor missing here for this initializer
     // call
      : mString (aStart.string ()) {}
};
