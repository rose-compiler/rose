
#ifndef __TILEK_DEFS_H__
#define __TILEK_DEFS_H__

#if defined(__TILEK_SPACE__) || defined(__TILEK_USER__) || defined(__TILEK_STATIC__) || defined(__TILEK_KERNEL__)
#error None of __TILEK_*__ macro should be defined.
#endif

#define __TILEK_NONE__     0
#define __TILEK_USER__     1
#define __TILEK_STATIC__   2
#define __TILEK_KERNEL__  3

#if defined(TILEK_FORCED)
#define __TILEK_SPACE__ __TILEK_NONE__
#elif  defined(TILEK_USER) && !defined(TILEK_STATIC) && !defined(TILEK_KERNEL)
#define __TILEK_SPACE__ __TILEK_USER__
#elif !defined(TILEK_USER) &&  defined(TILEK_STATIC) && !defined(TILEK_KERNEL)
#define __TILEK_SPACE__ __TILEK_STATIC__
#elif !defined(TILEK_USER) && !defined(TILEK_STATIC) &&  defined(TILEK_KERNEL)
#define __TILEK_SPACE__ __TILEK_KERNEL__
#elif !defined(TILEK_USER) && !defined(TILEK_STATIC) && !defined(TILEK_KERNEL)
#define __TILEK_SPACE__ __TILEK_NONE__
#else
#error More than one TILEK_* macro are defined.
#endif

#define TILEK_SPACE(s1) (__TILEK_SPACE__ == __TILEK_NONE__ || __TILEK_SPACE__ == s1)
#define TILEK_SPACE_(s1, s2) (__TILEK_SPACE__ == __TILEK_NONE__ || __TILEK_SPACE__ == s1 || __TILEK_SPACE__ == s2)

#endif /* __TILEK_DEFS_H__ */

