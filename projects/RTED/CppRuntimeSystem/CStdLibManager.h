#ifndef CSTDLIBMANAGER_H
#define CSTDLIBMANAGER_H


#include <string>
#include <cstring>

// using namespace std;


/** Provides checks for functions in the C Standard Library.  Each function @c f
 * for which a check is provided will be named @c check_@em f and take exactly the
 * same arguments as @c f.  If a check function for @c f does not report a
 * violation, then an immediate subsequent call to @c f with the same arguments
 * will not result in undefined behaviour.
 */
class CStdLibManager {

    public:
        /* Listed below (in comments) are the functions in the C standard
         * library that have not yet been considered.  Most of them probably do
         * not have any checks that need to be performed and can simply be
         * ignored.
         *
         * Those that have functions which must be checked to avoid unspecified
         * behaviour have corresponding check_ functions.
         *
         * For example, check_memcpy ensures that an immediately subsequent call
         * to memcpy with the same arguments will not result in unspecified
         * behaviour.
         */

        // cstdlib functions {{{
        //
        //  assert.h
        //  ctypes.h
        //     isalnum
        //     isalpha
        //     iscntrl
        //     isdigit
        //     isgraph
        //     islower
        //     isprint
        //     ispunct
        //     isspace
        //     isupper
        //     isxdigit
        //     tolower
        //     toupper
        //  errno.h
        //  float.h
        //  iso646.h
        //  limits.h
        //  locale.h
        //      setlocale
        //      localeconv
        //  math.h
        //      cos
        //      sin
        //      tan
        //      acos
        //      asin
        //      atan
        //      atan2
        //      cosh
        //      sinh
        //      tanh
        //      exp
        //      frexp
        //      ldexp
        //      log
        //      log10
        //      modf
        //      pow
        //      sqrt
        //      ceil
        //      fabs
        //      floor
        //      fmod
        //  setjmp.h
        //      longjmp
        //      setjmp
        //      jmp_buf
        //  signal.h
        //      signal
        //      raise
        //  stdarg.h
        //      va_start
        //      va_arg
        //      va_end
        //  stddef.h
        //  stdio.h
        //      remove
        //      rename
        //      tmpfile
        //      tmpnam
        //      fclose
        //      fflush
        //      fopen
        //      freopn
        //      setbuf
        //      setvbuf
        //      fprintf
        //      fscanf
        //      printf
        //      scanf
        //      vfprintf
        //      vprintf
        //      vsprintf
        //      fgetc
        //      fgets
        //      fputc
        //      fputs
        //      getc
        //      getchar
        //      gets
        //      putc
        //      putchar
        //      puts
        //      ungetc
        //      fread
        //      fwrite
        //      fgetpos
        //      fseek
        //      fsetpos
        //      ftell
        //      rewind
        //      clearerr
        //      feof
        //      ferror
        //      perror
        //  stdlib.h
        //      atof
        //      atoi
        //      atol
        //      strtod
        //      strtol
        //      strtoul
        //      rand
        //      srand
        //      calloc
        //      free
        //      malloc
        //      realloc
        //      abort
        //      atexit
        //      exit
        //      getenv
        //      system
        //      bsearch
        //      qsort
        //      abs
        //      div
        //      labs
        //      ldiv
        //      mblen
        //      mbtowc
        //      wctomb
        //      mbstowcs
        //      wcstombs
        //  string.h
                void check_memcpy( const void* destination, const void* source, size_t num);
                void check_memmove( const void* destination, const void* source, size_t num);
                void check_strcpy( const char* destination, const char* source);
                void check_strncpy( const char* destination, const char* source, size_t num);
                void check_strcat( const char* destination, const char* source);
                void check_strncat( const char* destination, const char* source, size_t num);
        //      memcmp
        //      strcmp
        //      strcoll
        //      strncmp
        //      strxfrm
        //      memchr
                void check_strchr( const char* str, int character);
        //      strcspn
        //      strpbrk
                void check_strpbrk( const char* str1, const char* str2);
        //      strrchr
                void check_strspn( const char* str1, const char* str2);
                void check_strstr( const char* str1, const char* str2);
        //      strtok
        //      memset
        //      strerror
                void check_strlen( const char* str);
        //  time.h
        //      clock
        //      difftime
        //      mktime
        //      time
        //      asctime
        //      ctime
        //      gmtime
        //      localtime
        //      strftime
        // }}}

    private:

        /** Check that the memory region (@c ptr1 : @c ptr1 + @c num) does not overlap
         * with memory region (@c ptr2 : @c ptr2 + num)
         */
        void check_overlap(
            const char* ptr1,
            const char* ptr2,
            size_t num,
            const std::string& description = ""
        );
        void check_allocation_overlap(
            const char* ptr1,
            const char* ptr2,
            const std::string& description = ""
        );
        void check_overlap(
            const char* ptr1,
            size_t size1,
            const char* ptr2,
            size_t size2,
            const std::string& description = ""
        );

        /** Checks that str is initialized, and that it contains a null
         * terminator.
         *
         * Returns the length of the string if the null terminator was found.
         * If the null terminator was not found, -1 is returned unless reporting
         * violations results in aborted execution.
         */
        size_t check_string( const char* str);
};

#endif
// vim:sw=4 ts=4 tw=80 et sta fdm=marker:
