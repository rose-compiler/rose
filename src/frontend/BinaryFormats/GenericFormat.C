/* Classes describing basic features of a binary file that are in common to all file formats. (SgAsmGenericFormat class) */
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
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
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    fprintf(f, "%s%-*s = %s\n", p, w, "family", to_string(get_family()).c_str());
    fprintf(f, "%s%-*s = %s\n", p, w, "purpose", to_string(get_purpose()).c_str());
    fprintf(f, "%s%-*s = %s\n", p, w, "sex", stringifyByteOrderEndianness(get_sex()).c_str());

    fprintf(f, "%s%-*s = %u (%scurrent)\n", p, w, "version", get_version(), get_is_current_version() ? "" : "not-" );
    
    fprintf(f, "%s%-*s = %s\n",  p, w, "ABI",      to_string(get_abi()).c_str());
    fprintf(f, "%s%-*s = %u\n",  p, w, "ABIvers",  get_abi_version());
    fprintf(f, "%s%-*s = %" PRIuPTR "\n", p, w, "wordsize", get_word_size());
}

#endif
