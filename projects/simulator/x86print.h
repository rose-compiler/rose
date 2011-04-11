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
 *    Translate t[] = { TE(PROT_READ), TE(PROT_WRITE), TE(PROT_EXEC), TE(PROT_NONE), T_END};
 *  \endcode
 *
 *  This data structure supports a number of cases:
 *
 *  <ul>
 *    <li>It can interpret an integer value as either an enumerated constant (specified with the "TE" family of macros) or
 *        a vector of single- or multi-bit flags (specified with the "TF" family of macros).</li>
 *    <li>The symbol can be defined before the macro that adds it to the Translate array, or it can be defined as it's
 *        added to the array by using the macros that have a "V" (value) argument.</li>
 *    <li>Flag symbols look only for the bits that are set in the symbol's value.  However, a mask can be supplied by
 *        using macros that have an "M" (mask) argument.  When checking for a symbol, we look at all the bits specified
 *        in the mask.  Under the covers, the only difference between a vector of flags and an enumeration value is that the
 *        mask for the enumeration value has all bits set.</li>
 *    <li>For a bit vector, bits that don't correspond to any known symbols can be formatted as an unsigned integer using
 *        a user-supplied printf format string.  This is useful, for instance, for printing permission bits as an octal
 *        number.</li>
 *  </ul>
 *
 *  Note: doxygen doesn't seem to document C preprocessor symbols, so you'll have to look at the source code. See x86print.h
 *  for the macro definitions; see RSIM_Common.h for example definitions; see system call handlers for example usage.
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
    typedef int (*StructPrinter)(RTS_Message*, const uint8_t*, size_t);
    StructPrinter struct_printer;/**< Prints a pointer to something, usually a struct. */
    uint8_t *struct_buf;        /**< Memory to be printed by struct_printer. */
    size_t struct_size;         /**< Desired size of struct_buf in bytes. */
    size_t struct_nread;        /**< Size of data actually read into struct_buf. */
};


std::string flags_to_str(const Translate*, uint32_t value);
std::string hex_to_str(uint32_t value);

void print_flags(RTS_Message*, const Translate*, uint32_t value);
void print_enum(RTS_Message*, const Translate*, uint32_t value);
void print_signed(RTS_Message*, uint32_t value);
void print_pointer(RTS_Message*, uint32_t value);
void print_hex(RTS_Message*, uint32_t value);
void print_string(RTS_Message*, const std::string &value, bool str_fault=false, bool str_trunc=false);
void print_buffer(RTS_Message*, uint32_t addr, const uint8_t *buffer, size_t sz, size_t print_sz);
void print_time(RTS_Message*, uint32_t value);
void print_single(RTS_Message*, char fmt, const ArgInfo *info);
void print_struct(RTS_Message*, uint32_t value, ArgInfo::StructPrinter printer, const uint8_t *buf, size_t need, size_t have);
void print_leave(RTS_Message*, char fmt, const ArgInfo *info);

#endif
