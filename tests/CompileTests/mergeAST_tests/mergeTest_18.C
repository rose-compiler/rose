// BTW, This example yields a really nice figure in color!
// The un-named class declaration is asigned a unique internal
// name which causes the structures not be be merged!
typedef struct
{
  int __count;
  union
  {
    int __wch;
    char __wchb[4];
  } __value_alt;		/* Value so far.  */
} __mbstate_t_alt;


