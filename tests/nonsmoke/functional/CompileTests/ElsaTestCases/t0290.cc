// t0290.cc
// problem matching definition and decl

template < typename _CharT >
class basic_string;

class locale { };

struct messages_base
{
  typedef int catalog;
};

template < typename _CharT >
class messages : public messages_base
{
  catalog open (const basic_string < char >&__s, const locale & __loc) const
  {
  }
  catalog open (const basic_string < char >&, const locale &,
                const char *) const;
};

template < typename _CharT >
typename messages < _CharT >::catalog
messages < _CharT >::open (const basic_string <char >&__s,
                           const locale & __loc,
                           const char *__dir) const
{
}
