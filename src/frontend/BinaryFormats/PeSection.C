/* Windows PE Sections (SgAsmPESection and related classes) */

#include "rose.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/* Pre-unparsing updates */
bool
SgAsmPESection::reallocate()
{
    bool reallocated = false;

    SgAsmPESectionTableEntry *shdr = get_section_entry();
    if (shdr)
        shdr->update_from_section(this);
    
    return reallocated;
}

/* Print some debugging info. */
void
SgAsmPESection::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sPESection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sPESection.", prefix);
    }

    SgAsmGenericSection::dump(f, p, -1);
    if (p_section_entry)
        p_section_entry->dump(f, p, -1);

    if (variantT() == V_SgAsmPESection) //unless a base class
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}

