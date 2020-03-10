///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Relative Virtual Addresses (RVA)
// An RVA is always relative to the base virtual address (base_va) defined in an executable file header.
// A rose_rva_t is optionally tied to an SgAsmGenericSection so that if the preferred mapped address of the section is
// modified then the RVA stored in the rose_rva_t object is also adjusted.  The section-relative offset is always treated as
// an unsigned quantity, but negative offsets can be accommodated via integer overflow.
//
// Be careful about adjusting the RVA (the address or section) using ROSETTA's accessors.
//     symbol.p_address.set_section(section);          // this works
//     symbol.get_address().set_section(section);      // using ROSETTA accessor modifies a temporary copy of the RVA
// But if ROSETTA returns a vector then we can modify the RVA:
//     symbol.p_addresses[0].set_section(section);     // this works
//     symbol.get_addresses()[0].set_section(section); // so does this.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"

/** @class rose_rva_t
 *
 * A relative virtual address optionally associated with a SgAsmSection.
 *
 * A relative virtual addresses (RVA) is an offset from the base virtual address (base_va) stored in a file header
 * (SgAsmGenericHeader). RVAs can either be stored as a simple numeric offset or they can be tied to the mapped
 * address of some file section (SgAsmGenericSection).  The rose_addr_t (a typedef for uint64_t) can be used in the former
 * case, and the rose_rva_t class can be used in the latter.
 *
 * Each rose_rva_t instance has an RVA represented by a rose_rva_t which can be adjusted via get_rva() and set_rva(). The
 * rose_rva_t instance can optionally be bound to a generic section so that if the mapping address of the section changes, the
 * value returned by get_rva() will be similarly adjusted.  The RVA is usually a positive offset from the bound section, but is
 * not required to be so. */

/** Create a zero RVA not linked to any section. */
rose_rva_t::rose_rva_t() {
    addr = 0;
    section = NULL;
}

/** Conversion from numeric RVA. The new rose_rva_t is constructed with the specified RVA and section.  The RVA is relative to
 *  the base_va of the (possibly implied) file header.  The section is optional. */
rose_rva_t::rose_rva_t(rose_addr_t rva, SgAsmGenericSection *section/*=NULL*/)
{
    addr = rva;
    this->section = NULL;
    set_section(section);
}


/** Copy constructor. */
rose_rva_t::rose_rva_t(const rose_rva_t &other) 
{
    addr = other.addr;
    section = other.section;
}

/** Assignment. */
rose_rva_t
rose_rva_t::operator=(const rose_rva_t &other) 
{
    addr = other.addr;
    section = other.section;
    return *this;
}

/** Class method to construct a new RVA which is an offset from the beginning of a section. The returned RVA is bound to the
 *  supplied section. */
rose_rva_t
rose_rva_t::section_relative(SgAsmGenericSection *section, rose_addr_t offset)
{
    assert(section!=NULL);
    assert(section->is_mapped());
    rose_addr_t rva =section->get_mapped_preferred_rva() + offset;
    return rose_rva_t(rva, section);
}

/** Determines whether this RVA is associated with a file section. */
bool
rose_rva_t::is_bound() const 
{
    return section!=NULL;
}

/** Returns the numeric RVA.  The returned value is an address relative to a base address stored in a (implied) file header.
 *  The file header is not actually referenced, thus this rose_rva_t may be unbound. */
rose_addr_t
rose_rva_t::get_rva() const
{
    rose_addr_t rva = addr;
    if (section) {
        assert(section->is_mapped());
        rva += section->get_mapped_preferred_rva();
    }
    return rva;
}

/** Resets this RVA to a new value without unbinding from a section. The modified RVA is remains bound to the same section as
 * before this call. The new RVA is allowed to be less than the beginning of the bound section. */
rose_rva_t&
rose_rva_t::set_rva(rose_addr_t rva)
{
    addr = rva;
    if (section) {
        assert(section->is_mapped());
        addr -= section->get_mapped_preferred_rva();
    }
    return *this;
}

/** Returns the section with which this RVA is associated. */
SgAsmGenericSection *
rose_rva_t::get_section() const
{
    return section;
}

/** Changes the section binding.  This RVA is unbound from its previous section and bound to the new section. The numeric value
 * of this RVA does not change as a result. If not null, the @p new_section must be mapped to a VA. */
rose_rva_t&
rose_rva_t::set_section(SgAsmGenericSection *new_section)
{
    assert(new_section==NULL || new_section->is_mapped());
    if (section) {
        addr += section->get_mapped_preferred_rva();
        section = NULL;
    }
    if (new_section)
        addr -= new_section->get_mapped_preferred_rva();
    section = new_section;
    return *this;
}

/** Binds this RVA to the best available section from the specified file header.  The numeric value of the RVA is not changed
 *  by this operation. The section is selected to be the mapped section that most specifically includes this RVA. */
rose_rva_t&
rose_rva_t::bind(SgAsmGenericHeader *fhdr)
{
    rose_addr_t va = get_rva() + fhdr->get_base_va();
    SgAsmGenericSection *secbind = fhdr->get_best_section_by_va(va, true);
    return set_section(secbind);
}

/** Return the absolute address if known.  The absolute address is created by adding this RVA's numeric value to the base_va
 *  associated with the bound section.  If no section is bound to this rose_rva_t, then the RVA is returned instead. */
rose_addr_t
rose_rva_t::get_va() const
{
    if (!section)
        return addr;
    assert(section->is_mapped());
    return addr + section->get_mapped_preferred_rva() + section->get_base_va();
}

/** Returns an offset from the currently bound section.  If this rose_rva_t is not bound to a section then the RVA numeric
 *  value is returned (i.e., an offset from the file header's base_va). */
rose_addr_t
rose_rva_t::get_rel() const
{
    return addr;
}

/** Returns an offset relative to the specified section. The specified section must be mapped to a VA. */
rose_addr_t
rose_rva_t::get_rel(SgAsmGenericSection *s)
{
    assert(s!=NULL && s->is_mapped());
    return get_rva() - s->get_mapped_preferred_rva();
}

/** Increment the address by the specified amount, keeping it attached to the same (if any) section. */
void
rose_rva_t::increment(rose_addr_t amount)
{
    addr += amount;
}

/** Convert to a string representation. If this RVA is unbound, then the string representation is the numeric RVA value (in
 * hexadecimal and decimal). Otherwise the string representation contains information about the bound section. */
std::string
rose_rva_t::to_string() const
{
    char s[1024];
    sprintf(s, "0x%08" PRIx64 " (%" PRIu64 ")", get_rva(), get_rva());
    std::string ss = s;

    if (get_section()) {
        sprintf(s, " + 0x%08" PRIx64 " (%" PRIu64 ")", get_rel(), get_rel());
        ss += " <" + get_section()->get_name()->get_string(true) + s + ">";
    }
    return ss;
}


std::ostream &
operator<<(std::ostream &os, const rose_rva_t &rva)
{
    os << rva.to_string();
    return os;
}

/* Arithmetic */
rose_addr_t operator+(const rose_rva_t &a1, const rose_rva_t &a2) {return a1.get_rva() + a2.get_rva();}
rose_addr_t operator-(const rose_rva_t &a1, const rose_rva_t &a2) {return a1.get_rva() - a2.get_rva();}

/* Comparisons */
bool operator< (const rose_rva_t &a1, const rose_rva_t &a2) {return a1.get_rva() <  a2.get_rva();}
bool operator<=(const rose_rva_t &a1, const rose_rva_t &a2) {return a1.get_rva() <= a2.get_rva();}
bool operator> (const rose_rva_t &a1, const rose_rva_t &a2) {return a1.get_rva() >  a2.get_rva();}
bool operator>=(const rose_rva_t &a1, const rose_rva_t &a2) {return a1.get_rva() >= a2.get_rva();}
bool operator==(const rose_rva_t &a1, const rose_rva_t &a2) {return a1.get_rva() == a2.get_rva();}
bool operator!=(const rose_rva_t &a1, const rose_rva_t &a2) {return a1.get_rva() != a2.get_rva();}

#endif
