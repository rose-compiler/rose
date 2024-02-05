#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"
#include <Rose/BinaryAnalysis/RelativeVirtualAddress.h>

// In order to efficiently (in terms of amount of code) parse a file format that's defined for a different architecture, we
// need to occassionally take addresses of structs that don't follow alignment rules for this architecture.
#if defined(__GNUC__) && __GNUC__ >= 9
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
#endif

SgAsmPERVASizePairList::SgAsmPERVASizePairList(SgAsmPEFileHeader *parent) {
    initializeProperties();
    set_parent(parent);
}

SgAsmPERVASizePair::SgAsmPERVASizePair(SgAsmPERVASizePairList *parent, const RVASizePair_disk *disk) {
    initializeProperties();

    ASSERT_not_null(disk);
    p_e_rva  = Rose::BinaryAnalysis::ByteOrder::leToHost(disk->e_rva);
    p_e_size = Rose::BinaryAnalysis::ByteOrder::leToHost(disk->e_size);
    set_parent(parent);
}

SgAsmPERVASizePair::SgAsmPERVASizePair(SgAsmPERVASizePairList *parent, rose_addr_t rva, rose_addr_t size) {
    initializeProperties();

    p_e_rva = rva;
    p_e_size = size;
    set_parent(parent);
}

void*
SgAsmPERVASizePair::encode(RVASizePair_disk *disk) const {
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_rva,  &(disk->e_rva));
    Rose::BinaryAnalysis::ByteOrder::hostToLe(p_e_size, &(disk->e_size));
    return disk;
}

void
SgAsmPERVASizePair::set_section(SgAsmGenericSection *section)
{
    if (section!=p_section)
        set_isModified(true);
    p_section = section;
    if (section) {
        set_e_rva(Rose::BinaryAnalysis::RelativeVirtualAddress(section->get_mappedPreferredRva(), section));
        set_e_size(section->get_mappedSize());
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
