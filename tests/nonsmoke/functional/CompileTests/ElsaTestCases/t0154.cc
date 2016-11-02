// in/t0154.cc
// attempting to minimize nsAtomTable.i failure:
// mString(aStart.string())


class nsAString {
};

class nsACString;


template <class CharT>
struct nsStringTraits {
  typedef nsAString                 abstract_string_type;
};

template <>
struct nsStringTraits<char> {
  typedef nsACString                abstract_string_type;
};


template <class CharT>
class nsReadingIterator {
public:
  // type: nsACString
  typedef typename nsStringTraits<CharT>::abstract_string_type abstract_string_type;

  const abstract_string_type& string() const;
};


class nsACString {
public:
  typedef char                              char_type;

  typedef nsACString                        abstract_string_type;
  typedef nsReadingIterator<char_type>      const_iterator;
};


class nsDependentCSubstring : public nsACString {
public:
  nsDependentCSubstring( const const_iterator& aStart,
                         const const_iterator& aEnd )
    : mString(aStart.string() /* type: nsACString& */)
  {}

  const abstract_string_type&  mString;    // type: nsACString&
};
