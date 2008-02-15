// t0152.cc
// example from nsAtomTable involving operator[]

// this failure is caused by incomplete overload resolution in the
// presence of templates

typedef unsigned int PRUint32;
typedef unsigned short PRUint16;
typedef PRUint16 PRUnichar;

template <class CharT>
class nsReadingIterator {
};

class nsAString {
public:
  typedef PRUnichar                         char_type;
  typedef nsReadingIterator<char_type>      const_iterator;
};

class nsASingleFragmentString : public nsAString {
public:
  typedef const char_type*  const_char_iterator;

  // this one appears first, and involves a template, so overload
  // resolution of the BeginReading function call does not happen;
  // the AST continues to look like a call to this one
  const_iterator& BeginReading( const_iterator& I ) const;

  // but this is the one that the operator[] implementation is
  // expecting to use
  const_char_iterator&
  BeginReading( const_char_iterator& aResult ) const;

  char_type operator[]( PRUint32 i ) const {
    const_char_iterator temp;
    return BeginReading(temp)[ i ];
  }
};
