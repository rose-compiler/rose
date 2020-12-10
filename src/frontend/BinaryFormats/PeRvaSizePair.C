#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"

// In order to efficiently (in terms of amount of code) parse a file format that's defined for a different architecture, we
// need to occassionally take addresses of structs that don't follow alignment rules for this architecture.
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"

void
SgAsmPERVASizePair::ctor(SgAsmPERVASizePairList *parent, const RVASizePair_disk *disk) {
    p_e_rva  = ByteOrder::le_to_host(disk->e_rva);
    p_e_size = ByteOrder::le_to_host(disk->e_size);
    set_parent(parent);
}

void
SgAsmPERVASizePair::ctor(SgAsmPERVASizePairList *parent, rose_addr_t rva, rose_addr_t size)
{
    p_e_rva = rva;
    p_e_size = size;
    set_parent(parent);
}

void*
SgAsmPERVASizePair::encode(RVASizePair_disk *disk) const {
    ByteOrder::host_to_le(p_e_rva,  &(disk->e_rva));
    ByteOrder::host_to_le(p_e_size, &(disk->e_size));
    return disk;
}

void
SgAsmPERVASizePair::set_section(SgAsmGenericSection *section)
{
    if (section!=p_section)
        set_isModified(true);
    p_section = section;
    if (section) {
        set_e_rva(rose_rva_t(section->get_mapped_preferred_rva(), section));
        set_e_size(section->get_mapped_size());
    } else {
        set_e_rva(0);
        set_e_size(0);
    }
}

SgAsmGenericSection *
SgAsmPERVASizePair::get_section() const
{
    return p_section;
}

#endif
