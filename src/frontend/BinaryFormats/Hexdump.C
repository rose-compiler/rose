/* Functions that produce output reminiscent of the Unix "hexdump" command. */
#include "rose.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

void
SgAsmExecutableFileFormat::hexdump(FILE *f, addr_t base_addr, const unsigned char *data, size_t n, const HexdumpFormat &fmt)
{
    /* Provide default formats. This is done here so that the header file doesn't depend on <inttypes.h> */
    const char *addr_fmt = fmt.addr_fmt ? fmt.addr_fmt : "0x%08"PRIx64":";
    const char *numeric_fmt = fmt.numeric_fmt ? fmt.numeric_fmt : " %02x";
    const char *prefix = fmt.prefix ? fmt.prefix : "";

    char s[1024];
    sprintf(s, numeric_fmt, 0u);
    int numeric_width = strlen(s);

    if (fmt.multiline)
        fputs(prefix, f);

    for (size_t i=0; i<n; i+=fmt.width) {
        /* Prefix and/or address */
        if (i>0) {
            fputc('\n', f);
            fputs(prefix, f);
        }
        fprintf(f, addr_fmt, base_addr+i);

        /* Numeric byte values */
        if (fmt.show_numeric) {
            for (size_t j=0; j<fmt.width; j++) {
                if (i+j<n) {
                    if (j>0 && 0 == j % fmt.colsize)
                        fputc(' ', f);
                    fprintf(f, numeric_fmt, data[i+j]);
                } else if (fmt.pad_numeric) {
                    if (j>0 && 0 == j % fmt.colsize)
                        fputc(' ', f);
                    fprintf(f, "%*s", numeric_width, "");
                }
            }
        }

        if (fmt.show_numeric && fmt.show_chars)
            fputs("  |", f);
        
        /* Character byte values */
        if (fmt.show_chars) {
            for (size_t j=0; j<fmt.width; j++) {
                if (i+j>=n) {
                    if (fmt.pad_chars)
                        fputc(' ', f);
                } else if (isprint(data[i+j])) {
                    fputc(data[i+j], f);
                } else {
                    fputc('.', f);
                }
            }
            fputc('|', f);
        }
    }

    if (fmt.multiline)
        fputc('\n', f);
}

// DQ (11/8/2008): Alternative interface that works better for ROSE IR nodes
void
SgAsmExecutableFileFormat::hexdump(FILE *f, addr_t base_addr, const std::string &prefix, const SgUnsignedCharList &data, 
                                   bool multiline)
{
    if (!data.empty()) {
        HexdumpFormat fmt;
        fmt.multiline = multiline;
        fmt.prefix = prefix.c_str();
        hexdump(f, base_addr, &(data[0]), data.size(), fmt);
    }
}

// DQ (8/31/2008): This is the newest interface function (could not remove the one based on SgUnsignedCharList since it
// is used in the symbol support).
void
SgAsmExecutableFileFormat::hexdump(FILE *f, addr_t base_addr, const std::string &prefix, const SgFileContentList &data, 
                                   bool multiline)
{
    if (!data.empty()) {
        HexdumpFormat fmt;
        fmt.multiline = multiline;
        fmt.prefix = prefix.c_str();
        hexdump(f, base_addr, &(data[0]), data.size(), fmt);

    }
}
