typedef struct
{
  int __count;
  union
  {
    int __wch;
    char __wchb[4];
  } __value;		/* Value so far.  */
} __mbstate_t;

typedef struct
{
  int __pos;
  __mbstate_t __state;
} _G_fpos_t;
typedef struct
{
  int __pos;
  __mbstate_t __state;
} _G_fpos64_t;


