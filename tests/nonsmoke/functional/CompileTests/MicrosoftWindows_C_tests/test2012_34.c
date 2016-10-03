extern struct eproto
   {
     const char *s;
     short p;
   } eproto_db[];


#if 0
/* Locate the VLA declaration in a function scope. (not allowed in Microsoft MSVC) */
void foobar()
   {
#if 0
     const struct eproto
        {
          const char *s;
          short p;
        } eproto_db[];
#else
     extern struct eproto
        {
          const char *s;
          short p;
        } eproto_db[];
#endif
   }
#endif

