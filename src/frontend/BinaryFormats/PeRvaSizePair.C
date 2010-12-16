#include "sage3basic.h"

void
SgAsmPERVASizePair::ctor(SgAsmPERVASizePairList *parent, const RVASizePair_disk *disk) {
    p_e_rva  = le_to_host(disk->e_rva);
    p_e_size = le_to_host(disk->e_size);
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
    host_to_le(p_e_rva,  &(disk->e_rva));
    host_to_le(p_e_size, &(disk->e_size));
    return disk;
}

/** Sets or removes the section associated with an RVA/size pair. Setting or removing the section also updates the RVA and size
 *  according to the preferred mapping address and mapped size of the section. */
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

/** Returns the section associated with an RVA/size pair. This is the same as the ROSETTA-generated accessor, but we need a
 * custom version of set_section(). */
SgAsmGenericSection *
SgAsmPERVASizePair::get_section() const
{
    return p_section;
}

