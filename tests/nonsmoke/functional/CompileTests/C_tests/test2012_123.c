
extern int tolower (int __c) __attribute__ ((__nothrow__));
static int posixly_correct;
extern char *getenv (__const char *__name) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1))) ;
typedef long unsigned int size_t;
typedef unsigned long int wctype_t;


static int
internal_fnmatch (const char *pattern, const char *string, const char *string_end, _Bool no_leading_period, int flags)
{
  register const char *p = pattern, *n = string;
  register unsigned char c;
// # 41 "fnmatch_loop.c"
  while ((c = *p++) != '\0')
    {
      _Bool new_no_leading_period = 0;

      switch (c)
        {

        case '[':
          {
            const char *p_init = p;
            const char *n_init = n;
            register _Bool not;
            char cold;
            unsigned char fn;

            for (;;)
              {
#if 0
                if (!(flags & (1 << 1)) && c == '\\')
                  {
                    goto normal_bracket;
                  }
                else if (c == '[' && *p == ':')
                  {

                    char str[256 + 1];
                    size_t c1 = 0;

                    wctype_t wt;

                    const char *startp = p;

                    for (;;)
                      {
                        if (c < 'a' || c >= 'z')
                          {
                            goto normal_bracket;
                          }
                      }

// # 287 "fnmatch_loop.c"
                    if (iswctype (btowc ((unsigned char) *n), wt))
                      goto matched;
// # 305 "fnmatch_loop.c"
                    c = *p++;
                  }
// # 413 "fnmatch_loop.c"
                else if (c == '\0')
                  {

                    p = p_init;
                    n = n_init;
                    c = '[';
                    goto normal_match;
                  }
                else
                  {
                    _Bool is_range = 0;
// # 599 "fnmatch_loop.c"
                      {
                        c = ((flags & (1 << 4)) ? tolower (c) : (c));
                      normal_bracket:
                        is_range = (*p == '-' && p[1] != '\0'
                                    && p[1] != ']');

                        if (!is_range && c == fn)
                          goto matched;
                        cold = c;
                        c = *p++;
                      }
                  }
#endif
                if (c == ']')
                  break;
              }

            if (!not)
              return 1;
            break;

          matched:

            do
              {
              ignore_next:
                c = *p++;
              }
            while (c != ']');
            if (not)
              return 1;
          }
          break;


        default:
        normal_match:
          if (n == string_end || c != ((flags & (1 << 4)) ? tolower ((unsigned char) *n) : ((unsigned char) *n)))
            return 1;
        }

      no_leading_period = new_no_leading_period;
      ++n;
    }

  return 1;
}


