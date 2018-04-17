// g0026.cc
// gcc-2 header bugs from bastring.cc

template <class charT, class traits,
          class Allocator>
class basic_string
{
private:
  struct Rep {
    inline static Rep* create (unsigned);
  };

  typedef unsigned size_type;
  size_type copy (charT* s, size_type n, size_type pos = 0) const;
};

template <class charT, class traits, class Allocator>
inline /* should say "typename" here */
basic_string <charT, traits, Allocator>::Rep *
basic_string <charT, traits, Allocator>::Rep::
create (unsigned extra)
{
  // ...
}

template <class charT, class traits, class Allocator>
/* should say "typename" here */
basic_string <charT, traits, Allocator>::size_type
basic_string <charT, traits, Allocator>::
copy (charT* s, size_type n, size_type pos) const
{
  // ...
  return n;
}
