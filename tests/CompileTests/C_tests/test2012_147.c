static struct tz_t
{
  char tzname[5];
  unsigned char zhours;
  unsigned char zminutes;
  unsigned char zoccident;
}
TimeZones[] =
{
  { "aat", 1, 0, 1 },
  { "adt", 4, 0, 0 },
  { "ast", 3, 0, 0 },

  { "bst", 1, 0, 0 },
  { "cat", 1, 0, 0 },
  { "cdt", 5, 0, 1 },
  { "cest", 2, 0, 0 },
  { "cet", 1, 0, 0 },
  { "cst", 6, 0, 1 },


  { "eat", 3, 0, 0 },
  { "edt", 4, 0, 1 },
  { "eest", 3, 0, 0 },
  { "eet", 2, 0, 0 },
  { "egst", 0, 0, 0 },
  { "egt", 1, 0, 1 },
  { "est", 5, 0, 1 },
  { "gmt", 0, 0, 0 },
  { "gst", 4, 0, 0 },
  { "hkt", 8, 0, 0 },
  { "ict", 7, 0, 0 },
  { "idt", 3, 0, 0 },
  { "ist", 2, 0, 0 },

  { "jst", 9, 0, 0 },
  { "kst", 9, 0, 0 },
  { "mdt", 6, 0, 1 },
  { "met", 1, 0, 0 },
  { "msd", 4, 0, 0 },
  { "msk", 3, 0, 0 },
  { "mst", 7, 0, 1 },
  { "nzdt", 13, 0, 0 },
  { "nzst", 12, 0, 0 },
  { "pdt", 7, 0, 1 },
  { "pst", 8, 0, 1 },
  { "sat", 2, 0, 0 },
  { "smt", 4, 0, 0 },
  { "sst", 11, 0, 1 },

  { "utc", 0, 0, 0 },
  { "wat", 0, 0, 0 },
  { "west", 1, 0, 0 },
  { "wet", 0, 0, 0 },
  { "wgst", 2, 0, 1 },
  { "wgt", 3, 0, 1 },
  { "wst", 8, 0, 0 },
};

typedef long unsigned size_t;

extern void *bsearch (__const void *__key, __const void *__base, size_t __nmemb, size_t __size ); // __attribute__ ((__nonnull__ (1, 2, 5))) ;

struct tz_t *tz;

void foobar()
   {
     int x;
     switch (x)
        {
          case 0:
             {
               if (1)
                  {
                  }
                 else
                  {
                    struct tz_t *tz;
                    x = sizeof (struct tz_t);
#if 1
                    tz = bsearch (0L, TimeZones, sizeof TimeZones/sizeof (struct tz_t),sizeof (struct tz_t) );
#endif
                    tz = bsearch (0L, TimeZones, sizeof (struct tz_t),sizeof (struct tz_t) );
                    tz = bsearch (0L, TimeZones, 42,sizeof (struct tz_t) );
                  }
             }
        }
   }
