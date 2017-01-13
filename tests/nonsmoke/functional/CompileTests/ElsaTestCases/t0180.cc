// t0180.cc
// parse-time ambiguity of TemplateArgumentList


// ----------------------
// the following example contains an ambiguous
// TemplateArgumentList, and the ambiguity survives
// into the final parse

// I've commented it out since I now don't allow
// as large a syntax for non-template arguments
// (at least for the moment)

// 2005-03-13: can put it back now?


namespace N {
  // primary
  template <class S, class T>
  struct A { /*...*/ };

  template <class S, int T>
  struct Helper1 {};

  typedef int myint;
  enum foo { enumerator };

  // specialization
  template <>
  struct A<int, Helper1<myint, enumerator<3> >
  {};
}


// -----------------------------
// the following example parses with an ambiguous
// TemplateArgumentList, but the ambiguity is part of
// a parse that then fails


// primary
template <typename A, typename T>
struct Traits { /*...*/ };

template <class S, class T>
struct Helper1 {};

template <int C>
struct Helper2 {};

// specialization
template <typename A, typename B, int C>
struct Traits<A, Helper1<B, Helper2<C> > >
{};

