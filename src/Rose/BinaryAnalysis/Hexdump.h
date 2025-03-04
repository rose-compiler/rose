#ifndef ROSE_BinaryAnalysis_Hexdump_H
#define ROSE_BinaryAnalysis_Hexdump_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Address.h>

#include <sageContainer.h>

#include <map>
#include <ostream>
#include <string>
#include <stdio.h>

namespace Rose {
namespace BinaryAnalysis {

/** Settings that control how the lowest-level @ref hexdump function behaves. */
struct HexdumpFormat {
    HexdumpFormat()
        : width(16), colsize(8), multiline(false), show_numeric(true), pad_numeric(true),
          show_chars(true), pad_chars(false) {}

    /* General settings */
    std::string prefix;         /**< Characters to emit after internal linefeeds. */
    size_t width;               /**< Bytes per line of output (default 16). */
    size_t colsize;             /**< Bytes per column group (default 8). */
    std::string addr_fmt;       /**< Printf format for addresses (default "0x%08llx: "). */
    bool multiline;             /**< Emit prefix at beginning and line feed at end of output. */

    /* Settings for numeric output of bytes */
    bool show_numeric;          /**< Show numeric format of bytes (default true). */
    std::string numeric_fmt;    /**< Printf format for bytes (default "%02x"). */
    std::map<unsigned char,std::string> numeric_fmt_special; /**< Special formatting for certain values. */
    std::string numeric_sep;    /**< String to print between numeric values (default " "). */
    bool pad_numeric;           /**< Pad numeric part of output with spaces for partial final line (default true). */

    /* Settings for ASCII output of bytes */
    bool show_chars;            /**< Show ASCII characters after bytes (default true). */
    bool pad_chars;             /**< Pad character part of output with spaces for partial final line (default true). */
};

/** Display binary data.
 *
 *  This function displays binary data in a fashion similar to the "hexdump -C" command in Unix: an address, numeric
 *  byte values, character byte values.  The format of the output is configurable through the HexdumpFormat
 *  argument. There are other versions that output containers of data.  The hexdump comes in three flavors: output to a
 *  C++ stream, output to a C FILE, and output to an std::string.  The FILE and string versions are implemented in
 *  terms of the stream version.
 *
 * @{ */
void hexdump(std::ostream&, Address base_addr, const unsigned char *data, size_t data_sz, const HexdumpFormat&);
void hexdump(std::ostream&, Address base_addr, const std::string &prefix, const SgUnsignedCharList& data, bool multiline=true);
void hexdump(std::ostream&, Address base_addr, const std::string &prefix, const SgFileContentList& data, bool multiline=true);

// Same, but returning a string instead.
std::string hexdump(Address base_addr, const unsigned char *data, size_t data_sz, const HexdumpFormat&);
std::string hexdump(Address base_addr, const std::string &prefix, const SgUnsignedCharList& data, bool multiline=true);
std::string hexdump(Address base_addr, const std::string &prefix, const SgFileContentList& data, bool multiline=true);

// Same, but output to a FILE* instead.
void hexdump(FILE*, Address base_addr, const unsigned char *data, size_t data_sz, const HexdumpFormat&);
void hexdump(FILE*, Address base_addr, const std::string &prefix, const SgUnsignedCharList& data, bool multiline=true);
void hexdump(FILE*, Address base_addr, const std::string &prefix, const SgFileContentList& data, bool multiline=true);
/** @} */

} // namespace
} // namespace

#endif
#endif
