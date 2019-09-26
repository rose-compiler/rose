typedef enum
   {
     _IEEE_ = -1,	/* According to IEEE 754/IEEE 854.  */
     _SVID_,	/* According to System V, release 4.  */
     _XOPEN_,	/* Nowadays also Unix98.  */
     _POSIX_,
     _ISOC_	/* Actually this is ISO C99.  */
   } math_type;

