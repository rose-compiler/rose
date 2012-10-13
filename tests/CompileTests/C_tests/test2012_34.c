extern struct eproto
   {
     const char *s;
     short p;
   } eproto_db[];


#if 0
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

