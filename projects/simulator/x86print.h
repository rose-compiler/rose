#ifndef ROSE_X86SIM_PRINT_H
#define ROSE_X86SIM_PRINT_H

#include <stdio.h>
#include <stdint.h>

#include <string>

/** Holds information about how to translate a constant or flag to a string.  Some of the printing functions take an array
 *  of these objects, and the array is terminated with value having a null pointer for the 'str' member. The T() and E_END
 *  macros can be used to build the list succinctly.  For example:
 *
 *  \code
 *    Translate t[] = { T(PROT_READ), T(PROT_WRITE), T(PROT_EXEC), T(PROT_NONE), T_END};
 *  \endcode
 */
struct Translate {
    uint32_t    mask;
    uint32_t    val;
    const char  *str;
};


#define TF(X)           {X, X, #X}              /**< Define a bit flag. */
#define TF2(M,X)        {M, X, #X}              /**< Define a bit vector with possible zero bits. */
#define TF3(M,V,X)      {M, V, #X}              /**< Define a masked flag when X is not defined. */
#define TF_FMT(M,FMT)   {0, M, FMT}             /**< Format remaining bits; FMT is a printf format string. */
#define TE(X)           {(uint32_t)-1, X, #X}   /**< Define an enumerated constant. */
#define TE2(V,X)        {(uint32_t)-1, V, #X}   /**< Define an enumerated constant with no predefined name symbol. */
#define T_END           {0, 0, NULL}            /**< Terminates a Translate table. */

/** Holds information needed for printing */
struct ArgInfo {
    ArgInfo()
        : val(0),
          str_fault(false), str_trunc(false),
          xlate(NULL),
          struct_printer(NULL), struct_buf(NULL), struct_size(0), struct_nread(0)
        {}
    ~ArgInfo() { delete[] struct_buf; }
    uint32_t    val;            /**< Integer value of the argument, straight from a machine register. */
    std::string str;            /**< String when val is the virtual address of a string, buffer, etc. */
    bool str_fault;             /**< True if a segmentation fault occurred while reading the string. */
    bool str_trunc;             /**< True if the string is truncated. */
    const Translate *xlate;     /**< Pointer to a translation table. */
    typedef int (*StructPrinter)(FILE*, const uint8_t*, size_t);
    StructPrinter struct_printer;/**< Prints a pointer to something, usually a struct. */
    uint8_t *struct_buf;        /**< Memory to be printed by struct_printer. */
    size_t struct_size;         /**< Desired size of struct_buf in bytes. */
    size_t struct_nread;        /**< Size of data actually read into struct_buf. */
};


int print_flags(FILE*, const Translate*, uint32_t value);
int print_enum(FILE*, const Translate*, uint32_t value);
int print_signed(FILE*, uint32_t value);
int print_pointer(FILE*, uint32_t value);
int print_hex(FILE*, uint32_t value);
int print_string(FILE*, const std::string &value, bool str_fault=false, bool str_trunc=false);
int print_buffer(FILE*, uint32_t addr, const uint8_t *buffer, size_t sz, size_t print_sz);
int print_time(FILE*, uint32_t value);
int print_single(FILE*, char fmt, const ArgInfo *info);
int print_struct(FILE *f, uint32_t value, ArgInfo::StructPrinter printer, const uint8_t *buf, size_t need, size_t have);
int print_enter(FILE*, const char *name, int syscallnum, const char *format, const ArgInfo *info);
int print_leave(FILE*, char fmt, const ArgInfo *info);

#endif
