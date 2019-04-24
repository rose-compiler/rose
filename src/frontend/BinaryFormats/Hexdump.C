/* Functions that produce output reminiscent of the Unix "hexdump" command. */
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include <boost/format.hpp>
#include <stdarg.h>

/*************************************************************************************************************************
 *                                                      C++ Stream Output
 *************************************************************************************************************************/

void
SgAsmExecutableFileFormat::hexdump(std::ostream &f, rose_addr_t base_addr, const unsigned char *data,
                                   size_t n, const HexdumpFormat &fmt)
{
    // Provide default formats. This is done here so that the header file doesn't depend on <inttypes.h>
    std::string addr_fmt = fmt.addr_fmt.empty() ? std::string("0x%|08x|: ") : fmt.addr_fmt;
    std::string numeric_fmt = fmt.numeric_fmt.empty() ? std::string("%|02x|") : fmt.numeric_fmt;
    std::string numeric_sep = fmt.numeric_sep.empty() ? std::string(" ") : fmt.numeric_sep;
    std::string prefix = fmt.prefix;

    size_t numeric_width = (boost::format(numeric_fmt) % (rose_addr_t)0).str().size();

    if (fmt.multiline)
        f <<prefix;

    for (size_t i=0; i<n; i+=fmt.width) {
        // Prefix and/or address
        if (i>0)
            f <<"\n" <<prefix;
        f <<boost::format(addr_fmt) % (base_addr+i);

        /* Numeric byte values */
        if (fmt.show_numeric) {
            for (size_t j=0; j<fmt.width; j++) {
                if (i+j<n) {
                    if (j>0)
                        f <<numeric_sep;
                    if (j>0 && 0 == j % fmt.colsize)
                        f <<" ";
                    std::map<unsigned char, std::string>::const_iterator special = fmt.numeric_fmt_special.find(data[i+j]);
                    if (special == fmt.numeric_fmt_special.end()) {
                        f <<boost::format(numeric_fmt) % (unsigned)data[i+j];
                    } else {
                        try {
                            f <<boost::format(special->second) % (unsigned)(special->first);
                        } catch (...) {
                            f <<special->second;
                        }
                    }
                } else if (fmt.pad_numeric) {
                    if (j>0)
                        f <<numeric_sep;
                    if (j>0 && 0 == j % fmt.colsize)
                        f <<" ";
                    f <<std::string(numeric_width, ' ');
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
    if (f)
        fputs(hexdump(base_addr, data, n, fmt).c_str(), f);
}

/* File output with old-style arguments */    
void
SgAsmExecutableFileFormat::hexdump(FILE *f, rose_addr_t base_addr, const std::string &prefix, const SgUnsignedCharList &data, 
                                   bool multiline)
{
    if (f)
        fputs(hexdump(base_addr, prefix, data, multiline).c_str(), f);
}

/* File output for SgFileContentList */
void
SgAsmExecutableFileFormat::hexdump(FILE *f, rose_addr_t base_addr, const std::string &prefix, const SgFileContentList &data, 
                                   bool multiline)
{
    if (f)
        fputs(hexdump(base_addr, prefix, data, multiline).c_str(), f);
}
