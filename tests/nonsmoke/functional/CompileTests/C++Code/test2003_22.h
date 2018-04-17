typedef struct
{
  int __count;
  union
  {
    wint_t __wch;
    char __wchb[4];
  } __value;		/* Value so far.  */
} __mbstate_t;
