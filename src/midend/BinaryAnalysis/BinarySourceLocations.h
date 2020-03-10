#ifndef ROSE_BinaryAnalysis_SourceLocations_H
#define ROSE_BinaryAnalysis_SourceLocations_H
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <SourceLocation.h>

#include <boost/filesystem.hpp>
#include <boost/serialization/access.hpp>
#include <Sawyer/IntervalMap.h>
#include <Sawyer/Map.h>
#include <Sawyer/Optional.h>
#include <Sawyer/Synchronization.h>
#include <set>
#include <string>

namespace Rose {
namespace BinaryAnalysis {

/** Bidirectional mapping between addresses and source locations.
 *
 *  This class stores a mapping between virtual addresses and source locations (file name, line number, and optional column number)
 *  and vice versa. Each source location can have multiple virtual addresses, but each address has zero or one source location. */
class SourceLocations {
public:
    typedef Sawyer::Container::IntervalMap<AddressInterval, SourceLocation> AddressToSource;
    typedef Sawyer::Container::Map<SourceLocation, AddressIntervalSet> SourceToAddress;

private:
    mutable SAWYER_THREAD_TRAITS::Mutex mutex_;         // protects the following data members
    AddressToSource addrToSrc_;
    SourceToAddress srcToAddr_;

private:
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(addrToSrc_);
        s & BOOST_SERIALIZATION_NVP(srcToAddr_);
    }
#endif

public:
    /** Default constructor.
     *
     *  Creates a mapping that has no links between source and binary. */
    SourceLocations() {}

    /** Atomic copy.
     *
     * @{ */
    SourceLocations(const SourceLocations&);
    SourceLocations& operator=(const SourceLocations&);
    /** @} */

    /** Insert links between source and adresseses.
     *
     *  If the source location is empty then the address is removed from the mapping.
     *
     * @{ */
    void insert(const SourceLocation&, rose_addr_t);
    void insert(const SourceLocation&, const AddressInterval&);
    void insert(const SourceLocation&, const AddressIntervalSet&);
    /** @} */

    /** Remove links between source and addresses.
     *
     *  Links between the source location and the address (either direction) are removed only if those links exist. No other
     *  links are affected. Since links to empty source locations never exist, calling this with an empty source location has
     *  no effect.
     *
     * @{ */
    void erase(const SourceLocation&, rose_addr_t);
    void erase(const SourceLocation&, const AddressInterval&);
    void erase(const SourceLocation&, const AddressIntervalSet&);
    /** @} */
    
    /** Remove all links for the specified addresses regardless of source.
     *
     * @{ */
    void erase(rose_addr_t);
    void erase(const AddressInterval&);
    void erase(const AddressIntervalSet&);
    /** @} */

    /** Remove all links for the specified source location regardless of address. */
    void erase(const SourceLocation&);

    /** Remove all links. */
    void clear();

    /** Insert information from debug tables.
     *
     *  This method traverses the specified AST to find the ELF DWARF information that specifies mappings between
     *  source code and addresses and adds that information to this object. */
    void insertFromDebug(SgNode *ast);

    /** Fill in small holes in the address space.
     *
     *  The DWARF information stored in a file typically maps source location to only the first virtual address for that source
     *  location. This method fills in the mapping so that any unmapped virtual address within a certain delta of a previous
     *  mapped address will map to the same source location as the previous mapped address. Mappings will be added only for
     *  addresses where the distance between the next lower mapped address and the next higher mapped address is less than or
     *  equal to @p maxHoleSize. */
    void fillHoles(size_t maxHoleSize = 64);

    /** Find the source location for an address.
     *
     * @{ */
    SourceLocation get(rose_addr_t) const;
    SourceLocation operator()(rose_addr_t va) const {
        return get(va);
    }
    /** @} */

    /** Find the addresses associated with a source location. */
    AddressIntervalSet get(const SourceLocation&) const;

    /** Find the first address associated with a source location.
     *
     * @{ */
    Sawyer::Optional<rose_addr_t> firstAddress(const SourceLocation&) const;
    Sawyer::Optional<rose_addr_t> operator()(const SourceLocation &loc) const {
        return firstAddress(loc);
    }
    /** @} */

    /** Get the list of all known source files. */
    std::set<boost::filesystem::path> allFileNames() const;

    /** Given a source location, return the next source location.
     *
     *  Returns the next source location that has a mapping to some address. The order will be first by increaing column
     *  number, then by increasing line number, and finally by different file names.  The file names are not sorted. When
     *  called with no source location then the first source location is returned. When there is no next source location then
     *  an empty source location is returned. */
    SourceLocation nextSourceLocation(const SourceLocation &current = SourceLocation()) const;

    /** Print the source to address mapping.
     *
     *  Each line of output is the specified prefix, the source location, a colon, and a space separated list of addresses.
     *
     * @{ */
    void printSrcToAddr(std::ostream&, const std::string &prefix = "") const;
    void print(std::ostream&, const std::string &prefix = "") const;
    /** @} */

    /** Print the address to source mapping.
     *
     *  Each line of output is the specified prefix, an address or address interval, a colon, and the source location. */
    void printAddrToSrc(std::ostream&, const std::string &prefix = "") const;

private:
    // Non-synchronized erasing of a link between source code and binary address.
    void eraseNS(const SourceLocation&, rose_addr_t);
    void eraseNS(rose_addr_t va);
};

std::ostream& operator<<(std::ostream&, const SourceLocations&);

} // namespace
} // namespace

#endif
#endif
