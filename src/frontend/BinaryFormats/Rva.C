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

// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

rose_addr_t
rose_rva_t::get_rva() const 
{
    rose_addr_t rva = addr;
    if (section) rva += section->get_mapped_preferred_rva();
    return rva;
}

rose_rva_t&
rose_rva_t::set_rva(rose_addr_t rva)
{
    addr = rva;
    if (section)
        addr -= section->get_mapped_preferred_rva();
    return *this;
}

SgAsmGenericSection *
rose_rva_t::get_section() const
{
    return section;
}

rose_rva_t&
rose_rva_t::set_section(SgAsmGenericSection *new_section)
{
    if (section) {
        addr += section->get_mapped_preferred_rva();
        section = NULL;
    }
    if (new_section) {
        addr -= new_section->get_mapped_preferred_rva();
    }
    section = new_section;
    return *this;
}

/* Set the section to the section that best (most specifically) describes the virtual address */
rose_rva_t&
rose_rva_t::bind(SgAsmGenericHeader *fhdr)
{
    rose_addr_t va = get_rva() + fhdr->get_base_va();
    SgAsmGenericSection *secbind = fhdr->get_best_section_by_va(va, true);
    return set_section(secbind);
}

/* Return address relative to currently bound section */
rose_addr_t
rose_rva_t::get_rel() const
{
    return addr;
}

/** Return address relative to specified section */
rose_addr_t
rose_rva_t::get_rel(SgAsmGenericSection *s)
{
    ROSE_ASSERT(s!=NULL);
    ROSE_ASSERT(s->is_mapped());
    ROSE_ASSERT(get_rva() >= s->get_mapped_preferred_rva());
    return get_rva() - s->get_mapped_preferred_rva();
}

/** Return the absolute address if known.  If this object is bound to a section then this method returns
 *  the absolute address (relative address plus section address plus base address). Otherwise it returns the
 *  relative virtual address that was assigned. */
rose_addr_t
rose_rva_t::get_va() const
{
    if (!section)
        return addr;
    ROSE_ASSERT(section->is_mapped());
    return addr + section->get_mapped_preferred_rva() + section->get_base_va();
}

/** Convert to a string representation */
std::string
rose_rva_t::to_string() const
{
    char s[1024];
    sprintf(s, "0x%08"PRIx64" (%"PRIu64")", get_rva(), get_rva());
    std::string ss = s;

    if (get_section()) {
        sprintf(s, " + 0x%08"PRIx64" (%"PRIu64")", get_rel(), get_rel());
        ss += " <" + get_section()->get_name()->get_string() + s + ">";
    }
    return ss;
}

/** Increment the address by the specified amount, keeping it attached to the same (if any) section. */
void
rose_rva_t::increment(rose_addr_t amount)
{
    addr += amount;
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

    
