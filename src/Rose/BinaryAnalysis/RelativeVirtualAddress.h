#ifndef ROSE_BinaryAnalysis_RelativeVirtualAddress_H
#define ROSE_BinaryAnalysis_RelativeVirtualAddress_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Address.h>

#include <ROSE_DEPRECATED.h>
#include <Cxx_GrammarDeclarations.h>

#include <Sawyer/Optional.h>

namespace Rose {
namespace BinaryAnalysis {

/** Optionally bound relative virtual address.
 *
 *  A relative virtual address (RVA) is an address which is an offset from the file's base address. The file's base address is
 *  normally found in a file header structure.
 *
 *  This object stores an RVA as either the offset from the file's base address, or as an offset from a specified section's
 *  preferred mapping virtual address. The latter case is useful when a section's address might change later. */
class RelativeVirtualAddress {
    // Offset relative to a section's preferred mapping address (if `section_` is not null) or relative to a file's base address (if
    // `section_` is null). The offset is treated as a signed quantity by virtue of unsigned 64-bit overflow.
    rose_addr_t rva_ = 0;

    // Optional base section. If present, then it must have a preferred mapping address, and the `rva_` is an offset to the
    // section's preferred address.
    SgAsmGenericSection *section_ = nullptr;

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(rva_);
        s & BOOST_SERIALIZATION_NVP(section_);
    }
#endif

public:
    /** Create a zero RVA not linked to any section. */
    RelativeVirtualAddress();

    /** Conversion from numeric RVA.
     *
     *  The new instance is constructed with the specified RVA and section.  The @p rva is relative to the base address of the
     *  file header.  The section is optional, and if present then the new instance is bound to that section. */
    RelativeVirtualAddress(const rose_addr_t rva, SgAsmGenericSection* = nullptr); //implicit

    /** Copy constructor. */
    RelativeVirtualAddress(const RelativeVirtualAddress&);

    /** Assignment operator. */
    RelativeVirtualAddress operator=(const RelativeVirtualAddress&);

    /** Constructs a new instance from a section and offset.
     *
     *  The RVA is computed by adding the specified offset to the section's preferred mapping address. The returned instance is
     *  bound to the section, which must not be null. */
    static RelativeVirtualAddress sectionRelative(SgAsmGenericSection*, rose_addr_t sectionOffset);

    /** Determines whether this instance is associated with a file section. */
    bool isBound() const;

    /** Returns the offset.
     *
     *  The returned value is an address relative to a base address stored in a (implied) file header.  The file header is not
     *  actually referenced, thus this instance may be unbound. */
    rose_addr_t rva() const;

    /** Assign a new RVA without adjusting the bound section.
     *
     *  This object will store the new RVA. The binding to a section is not modified by this function. */
    RelativeVirtualAddress& rva(rose_addr_t rva);

    /** Returns the section to which this RVA is bound. */
    SgAsmGenericSection *boundSection() const;

    /** Changes the section binding.
     *
     *  This RVA is unbound from its previous section and bound to the new section. The numeric value of this RVA does not change as
     *  a result. If not null, the @p new_section must have a preferred mapping address. */
    RelativeVirtualAddress& bindSection(SgAsmGenericSection*);

    /** Binds this RVA to the best available section from the specified file header.
     *
     *  The numeric value of the RVA is not changed by this operation. The section is selected to be the mapped section that most
     *  specifically includes this RVA. */
    RelativeVirtualAddress& bindBestSection(SgAsmGenericHeader*);

    /** Return the absolute address if known.
     *
     *  The absolute address is created by adding this RVA's numeric value to the base address associated with the bound section. */
    Sawyer::Optional<rose_addr_t> va() const;

    /** Returns an offset from the currently bound section.
     *
     *  Returns this address with respect to the beginning of the bound section. If no section is bound then nothing is returned. */
    Sawyer::Optional<rose_addr_t> boundOffset() const;

    /** Returns an offset relative to the specified section.
     *
     *  The specified section must be mapped to a VA. */
    rose_addr_t offsetFrom(SgAsmGenericSection*) const;

    /** Increment the address by the specified amount, keeping it attached to the same (if any) section. */
    void increment(rose_addr_t amount);

    /** Convert to a string representation.
     *
     *  If this RVA is unbound, then the string representation is the numeric RVA value (in hexadecimal and decimal). Otherwise the
     * string representation contains information about the bound section. */
    std::string toString() const;

    // [Robb Matzke 2024-02-05]: deprecated
public:
    static RelativeVirtualAddress section_relative(SgAsmGenericSection*, rose_addr_t) ROSE_DEPRECATED("use sectionRelative instead");
    bool is_bound() const ROSE_DEPRECATED("use isBound instead");
    rose_addr_t get_rva() const ROSE_DEPRECATED("use rva instead");
    RelativeVirtualAddress& set_rva(rose_addr_t rva) ROSE_DEPRECATED("use rva instead");
    SgAsmGenericSection *get_section() const ROSE_DEPRECATED("use boundSection instead");
    RelativeVirtualAddress& set_section(SgAsmGenericSection*) ROSE_DEPRECATED("use bindSection instead");
    RelativeVirtualAddress& bind(SgAsmGenericHeader*) ROSE_DEPRECATED("use bindBestSection instead");
    rose_addr_t get_va() const ROSE_DEPRECATED("use va instead");
    rose_addr_t get_rel() const ROSE_DEPRECATED("use boundOffset instead");
    rose_addr_t get_rel(SgAsmGenericSection*) ROSE_DEPRECATED("use offsetFrom instead");
    std::string to_string() const ROSE_DEPRECATED("use toString instead");
};

} // namespace
} // namespace

// Old name used from 2008-2024
using rose_rva_t = Rose::BinaryAnalysis::RelativeVirtualAddress;

std::ostream &operator<<(std::ostream&, const Rose::BinaryAnalysis::RelativeVirtualAddress&);

rose_addr_t operator-(const Rose::BinaryAnalysis::RelativeVirtualAddress &a1,
                      const Rose::BinaryAnalysis::RelativeVirtualAddress &a2);
rose_addr_t operator+(const Rose::BinaryAnalysis::RelativeVirtualAddress &a1,
                      const Rose::BinaryAnalysis::RelativeVirtualAddress &a2);

bool operator< (const Rose::BinaryAnalysis::RelativeVirtualAddress &a1, const Rose::BinaryAnalysis::RelativeVirtualAddress &a2);
bool operator<=(const Rose::BinaryAnalysis::RelativeVirtualAddress &a1, const Rose::BinaryAnalysis::RelativeVirtualAddress &a2);
bool operator> (const Rose::BinaryAnalysis::RelativeVirtualAddress &a1, const Rose::BinaryAnalysis::RelativeVirtualAddress &a2);
bool operator>=(const Rose::BinaryAnalysis::RelativeVirtualAddress &a1, const Rose::BinaryAnalysis::RelativeVirtualAddress &a2);
bool operator==(const Rose::BinaryAnalysis::RelativeVirtualAddress &a1, const Rose::BinaryAnalysis::RelativeVirtualAddress &a2);
bool operator!=(const Rose::BinaryAnalysis::RelativeVirtualAddress &a1, const Rose::BinaryAnalysis::RelativeVirtualAddress &a2);

#endif
#endif
