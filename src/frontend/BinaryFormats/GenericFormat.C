/* Classes describing basic features of a binary file that are in common to all file formats. (SgAsmGenericFormat class) */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include "stringify.h"

using namespace Rose;

void
SgAsmGenericFormat::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p),"%sFormat[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p),"%sFormat.", prefix);
    }
    const int w = std::max(size_t{1}, DUMP_FIELD_WIDTH - strlen(p));
    
    fprintf(f, "%s%-*s = %s\n", p, w, "family", toString(get_family()).c_str());
    fprintf(f, "%s%-*s = %s\n", p, w, "purpose", toString(get_purpose()).c_str());
    fprintf(f, "%s%-*s = %s\n", p, w, "sex", stringifyByteOrderEndianness(get_sex()).c_str());

    fprintf(f, "%s%-*s = %u (%scurrent)\n", p, w, "version", get_version(), get_isCurrentVersion() ? "" : "not-" );
    
    fprintf(f, "%s%-*s = %s\n",  p, w, "ABI",      toString(get_abi()).c_str());
    fprintf(f, "%s%-*s = %u\n",  p, w, "ABIvers",  get_abiVersion());
    fprintf(f, "%s%-*s = %" PRIuPTR "\n", p, w, "wordsize", get_wordSize());
}

bool
SgAsmGenericFormat::get_is_current_version() const {
    return get_isCurrentVersion();
}

void
SgAsmGenericFormat::set_is_current_version(bool x) {
    set_isCurrentVersion(x);
}

unsigned
SgAsmGenericFormat::get_abi_version() const {
    return get_abiVersion();
}

void
SgAsmGenericFormat::set_abi_version(unsigned x) {
    set_abiVersion(x);
}

size_t
SgAsmGenericFormat::get_word_size() const {
    return get_wordSize();
}

void
SgAsmGenericFormat::set_word_size(size_t x) {
    set_wordSize(x);
}

#endif
