/* confdefs.h */
#define PACKAGE_NAME "xerces-c"
#define PACKAGE_TARNAME "xerces-c"
#define PACKAGE_VERSION "3.1.1"
#define PACKAGE_STRING "xerces-c 3.1.1"
#define PACKAGE_BUGREPORT ""
#define PACKAGE_URL ""
#define PACKAGE "xerces-c"
#define VERSION "3.1.1"
#define STDC_HEADERS 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRING_H 1
#define HAVE_MEMORY_H 1
#define HAVE_STRINGS_H 1
#define HAVE_INTTYPES_H 1
#define HAVE_STDINT_H 1
#define HAVE_UNISTD_H 1
#define HAVE_DLFCN_H 1
#define LT_OBJDIR ".libs/"
#define STDC_HEADERS 1
#define TIME_WITH_SYS_TIME 1
#define HAVE_ARPA_INET_H 1
#define HAVE_FCNTL_H 1
#define HAVE_FLOAT_H 1
#define HAVE_INTTYPES_H 1
#define HAVE_LANGINFO_H 1
#define HAVE_LIMITS_H 1
#define HAVE_LOCALE_H 1
#define HAVE_MEMORY_H 1
#define HAVE_NETDB_H 1
#define HAVE_NETINET_IN_H 1
#define HAVE_NL_TYPES_H 1
#define HAVE_STDDEF_H 1
#define HAVE_STDINT_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRING_H 1
#define HAVE_STRINGS_H 1
#define HAVE_SYS_PARAM_H 1
#define HAVE_SYS_SOCKET_H 1
#define HAVE_SYS_TIME_H 1
#define HAVE_SYS_TIMEB_H 1
#define HAVE_UNISTD_H 1
#define HAVE_WCHAR_H 1
#define HAVE_WCTYPE_H 1
#define HAVE_ENDIAN_H 1
#define HAVE_ARPA_NAMESER_COMPAT_H 1
#define HAVE_STDBOOL_H 1
#define SIZEOF_WCHAR_T 4
#define HAVE_INTTYPES_H 1
#define SIZEOF_SHORT 2
#define SIZEOF_INT 4
#define SIZEOF_LONG 8
#define SIZEOF_LONG_LONG 8
#define SIZEOF___INT64 0
#define XERCES_S16BIT_INT int16_t
#define XERCES_U16BIT_INT uint16_t
#define XERCES_S32BIT_INT int32_t
#define XERCES_U32BIT_INT uint32_t
#define XERCES_S64BIT_INT int64_t
#define XERCES_U64BIT_INT uint64_t
#define HAVE_BOOL /**/
#define HAVE_NAMESPACES /**/
#define HAVE_STD_NAMESPACE /**/
#define HAVE_STD_LIBS /**/
#define HAVE_PTHREAD 1
#define HAVE_MBRLEN 1
#define HAVE_WCSRTOMBS 1
#define HAVE_MBSRTOWCS 1
#define ICONV_USES_CONST_POINTER 0
#define XERCES_USE_MUTEXMGR_POSIX 1
#define XERCES_USE_NETACCESSOR_CURL 1
#define HAVE_ICONV_H 1
#define HAVE_WCHAR_H 1
#define HAVE_STRING_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STDIO_H 1
#define HAVE_CTYPE_H 1
#define HAVE_LOCALE_H 1
#define HAVE_ERRNO_H 1
#define HAVE_WCHAR_H 1
/* end confdefs.h.  */
/* Define wcstombs to an innocuous variant, in case <limits.h> declares wcstombs.
   For example, HP-UX 11i <limits.h> declares gettimeofday.  */
#define wcstombs innocuous_wcstombs

/* System header to define __stub macros and hopefully few prototypes,
    which can conflict with char wcstombs (); below.
    Prefer <limits.h> to <assert.h> if __STDC__ is defined, since
    <limits.h> exists even on freestanding compilers.  */

#ifdef __STDC__
# include <limits.h>
#else
# include <assert.h>
#endif

#undef wcstombs

/* Override any GCC internal prototype to avoid an error.
   Use char because int might match the return type of a GCC
   builtin and then its argument prototype would still apply.  */
#ifdef __cplusplus
extern "C"
#endif
char wcstombs ();
/* The GNU C library defines this for functions which it implements
    to always fail with ENOSYS.  Some functions are actually named
    something starting with __ and the normal name is an alias.  */
#if defined __stub_wcstombs || defined __stub___wcstombs
choke me
#endif

int
main ()
{
return wcstombs ();
  ;
  return 0;
}

