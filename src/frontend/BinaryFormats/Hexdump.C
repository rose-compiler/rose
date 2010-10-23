/* Functions that produce output reminiscent of the Unix "hexdump" command. */
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <stdarg.h>

/* Helper function that safely sprintfs to a buffer, allocation the buffer as needed. Note that the buffer is never freed (so
 * we don't have to allocate/free each time in, but that the conversion to std::string copies it. */
static std::string
str_printf(const char *fmt, ...)
{
    va_list ap;
    static size_t bufsz = 128;
    static char *buf = new char[bufsz];
    va_start(ap, fmt);

    size_t need;
    while ((need=vsnprintf(buf, bufsz, fmt, ap))>bufsz) {
        delete[] buf;
        bufsz = need+1;
        buf = new char[bufsz];
    }
    va_end(ap);
    return buf;
}

/*************************************************************************************************************************
 *                                                      C++ Stream Output
 *************************************************************************************************************************/

/** Prints binary data similar to the Unix hexdump command.  The output is configurable via the HexdumpFormat argument.  This
 *  is the most general hexdump function. All others should be implemented in terms of this one. */
void
SgAsmExecutableFileFormat::hexdump(std::ostream &f, rose_addr_t base_addr, const unsigned char *data,
                                   size_t n, const HexdumpFormat &fmt)
{
    /* Provide default formats. This is done here so that the header file doesn't depend on <inttypes.h> */
    const char *addr_fmt = fmt.addr_fmt ? fmt.addr_fmt : "0x%08"PRIx64":";
    const char *numeric_fmt = fmt.numeric_fmt ? fmt.numeric_fmt : " %02x";
    const char *prefix = fmt.prefix ? fmt.prefix : "";

    char s[1024];
    sprintf(s, numeric_fmt, 0u);
    int numeric_width = strlen(s);

    if (fmt.multiline)
        f <<prefix;

    for (size_t i=0; i<n; i+=fmt.width) {
        /* Prefix and/or address */
        if (i>0)
            f <<"\n" <<prefix;
        f <<str_printf(addr_fmt, base_addr+i);

        /* Numeric byte values */
        if (fmt.show_numeric) {
            for (size_t j=0; j<fmt.width; j++) {
                if (i+j<n) {
                    if (j>0 && 0 == j % fmt.colsize)
                        f <<" ";
                    f <<str_printf(numeric_fmt, data[i+j]);
                } else if (fmt.pad_numeric) {
                    if (j>0 && 0 == j % fmt.colsize)
                        f <<" ";
                    f <<str_printf("%*s", numeric_width, "");
                }
            }
        }

        if (fmt.show_numeric && fmt.show_chars)
            f <<" |";
        
        /* Character byte values */
        if (fmt.show_chars) {
            for (size_t j=0; j<fmt.width; j++) {
                if (i+j>=n) {
                    if (fmt.pad_chars)
                        f <<" ";
                } else if (isprint(data[i+j])) {
                    f <<data[i+j];
                } else {
                    f <<".";
                }
            }
            f <<"|";
        }
    }

    if (fmt.multiline)
        f <<"\n";
}

/* Stream output for old-style arguments. */
void
SgAsmExecutableFileFormat::hexdump(std::ostream &f, rose_addr_t base_addr, const std::string &prefix,
                                   const SgUnsignedCharList &data, bool multiline)
{
    if (!data.empty()) {
        HexdumpFormat fmt;
        fmt.multiline = multiline;
        fmt.prefix = prefix.c_str();
        hexdump(f, base_addr, &(data[0]), data.size(), fmt);
    }
}

/* Stream output for SgFileContentList */
void
SgAsmExecutableFileFormat::hexdump(std::ostream &f, rose_addr_t base_addr, const std::string &prefix,
                                   const SgFileContentList &data, bool multiline)
{

    if (!data.empty()) {
        HexdumpFormat fmt;
        fmt.multiline = multiline;
        fmt.prefix = prefix.c_str();
        hexdump(f, base_addr, &(data[0]), data.size(), fmt);
    }
}

/*************************************************************************************************************************
 *                                                      C++ String Output
 *************************************************************************************************************************/

/* String output with new-style arguments. */
std::string
SgAsmExecutableFileFormat::hexdump(rose_addr_t base_addr, const unsigned char *data, size_t n, const HexdumpFormat &fmt)
{
    std::ostringstream s;
    hexdump(s, base_addr, data, n, fmt);
    return s.str();
}

/* String output with old-style arguments. */
std::string
SgAsmExecutableFileFormat::hexdump(rose_addr_t base_addr, const std::string &prefix, const SgUnsignedCharList &data,
                                   bool multiline)
{
    if (data.empty()) return "";
    HexdumpFormat fmt;
    fmt.multiline = multiline;
    fmt.prefix = prefix.c_str();
    return hexdump(base_addr, &(data[0]), data.size(), fmt);
}

/* String output for SgFileContentList */
std::string
SgAsmExecutableFileFormat::hexdump(rose_addr_t base_addr, const std::string &prefix, const SgFileContentList &data,
                                   bool multiline)
{
    if (data.empty()) return "";
    HexdumpFormat fmt;
    fmt.multiline = multiline;
    fmt.prefix = prefix.c_str();
    return hexdump(base_addr, &(data[0]), data.size(), fmt);
}

/*************************************************************************************************************************
 *                                                      C File Output
 *************************************************************************************************************************/

/* File output with new-style arguments */
void
SgAsmExecutableFileFormat::hexdump(FILE *f, rose_addr_t base_addr, const unsigned char *data, size_t n, const HexdumpFormat &fmt)
{
    fputs(hexdump(base_addr, data, n, fmt).c_str(), f);
}

/* File output with old-style arguments */    
void
SgAsmExecutableFileFormat::hexdump(FILE *f, rose_addr_t base_addr, const std::string &prefix, const SgUnsignedCharList &data, 
                                   bool multiline)
{
    fputs(hexdump(base_addr, prefix, data, multiline).c_str(), f);
}

/* File output for SgFileContentList */
void
SgAsmExecutableFileFormat::hexdump(FILE *f, rose_addr_t base_addr, const std::string &prefix, const SgFileContentList &data, 
                                   bool multiline)
{
    fputs(hexdump(base_addr, prefix, data, multiline).c_str(), f);
}
