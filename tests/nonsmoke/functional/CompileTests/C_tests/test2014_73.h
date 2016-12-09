
// Application specific macro to build a GCC version number (non-standard).

// However, it does appear to be predefined in ROSE.
#ifdef GCC_VERSION
   #warning "GCC_VERSION appears to already be defined in ROSE"
#endif

// #ifdef GCC_VERSION
#define GCC_VERSION (__GNUC__ * 1000 + __GNUC_MINOR__)
// #endif /* GCC_VERSION */

/* This macro allows casting away const-ness to pass -Wcast-qual
   warnings.  DO NOT USE THIS UNLESS YOU REALLY HAVE TO!  It should
   only be used in certain specific cases.  One valid case is where
   the C standard definitions or prototypes force you to.  E.g. if you
   need to free a const object, or if you pass a const string to
   execv, et al.  Another valid use would be in an allocation function
   that creates const objects that need to be initialized.  In some
   cases we have non-const functions that return the argument
   (e.g. next_nonnote_insn).  Rather than create const shadow
   functions, we can cast away const-ness in calling these interfaces
   if we're careful to verify that the called function does indeed not
   modify its argument and the return value is only used in a const
   context.  (This can be somewhat dangerous as these assumptions can
   change after the fact).  Beyond these uses, most other cases of
   using this macro should be viewed with extreme caution.  */

#ifdef __cplusplus
#warning "CONST_CAST2 for C++ will be defined"
#define CONST_CAST2(TOTYPE,FROMTYPE,X) (const_cast<TOTYPE> (X))
#else
#if defined(__GNUC__) && GCC_VERSION > 4000
/* GCC 4.0.x has a bug where it may ICE on this expression,
   so does GCC 3.4.x (PR17436).  */
#warning "CONST_CAST2 for GNUC version > 4.0.0 will be defined"
#define CONST_CAST2(TOTYPE,FROMTYPE,X) ((__extension__(union {FROMTYPE _q; TOTYPE _nq;})(X))._nq)
#elif defined(__GNUC__)
static inline char *
helper_const_non_const_cast (const char *p)
{
  union {
    const char *const_c;
    char *c;
  } val;
  val.const_c = p;
  return val.c;
}

#warning "CONST_CAST2 for GNUC version <= 4.0.0 will be defined"
#define CONST_CAST2(TOTYPE,FROMTYPE,X) ((TOTYPE) helper_const_non_const_cast ((const char *) (FROMTYPE) (X)))
#else
#warning "CONST_CAST2 devault version will be defined"
#define CONST_CAST2(TOTYPE,FROMTYPE,X) ((TOTYPE)(FROMTYPE)(X))
#endif
#endif

#warning "GCC_VERSION = "GCC_VERSION

#define CONST_CAST(TYPE,X) CONST_CAST2(TYPE, const TYPE, (X))
