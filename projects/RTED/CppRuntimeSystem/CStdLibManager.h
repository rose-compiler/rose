#ifndef CSTDLIBMANAGER_H
#define CSTDLIBMANAGER_H


#include <string>
#include <cstring>

using namespace std;


class CStdLibManager {

    public:
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
                void check_memcpy( void* destination, const void* source, size_t num);
                void check_memmove( void* destination, const void* source, size_t num);
                void check_strcpy( char* destination, const char* source);
                void check_strncpy( char* destination, const char* source, size_t num);
                void check_strcat( char* destination, const char* source);
                void check_strncat( char* destination, const char* source, size_t num);
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
       
        // Check that the memory region `ptr1 .. ptr1 + num` does not overlap
        // with memory region `ptr2 .. ptr2 + num`
        void check_overlap(
            const void* ptr1, 
            const void* ptr2, 
            size_t num, 
            const string& description = ""
        );
        void check_allocation_overlap(
            const void* ptr1, 
            const void* ptr2, 
            const string& description = ""
        );
        void check_overlap(
            const void* ptr1, 
            size_t size1, 
            const void* ptr2, 
            size_t size2, 
            const string& description = ""
        );

        // Checks that str is initialized, and that it contains a null
        // terminator.
        //
        // Returns the length of the string if the null terminator was found.
        // If the null terminator was not found, -1 is returned unless reporting
        // violations results in aborted execution.
        size_t check_string( const char* str);
};

#endif
// vim:sw=4 ts=4 tw=80 et sta fdm=marker:
