#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <Rose/Sarif/Node.h>

#include <Rose/BinaryAnalysis/AddressInterval.h>
#include <Rose/SourceLocation.h>

namespace Rose {
namespace Sarif {

/** Location or region for a result.
 *
 *  A location is either a source location or a binary location and defines a particular point in an artifact. Source locations
 *  have a file name, a 1-origin line number, and an optional 1-origin column number. A binary location is a binary specimen name
 *  and an address within the specimen. Binary locations are only supported when ROSE is configured to support binary anslysis.
 *
 *  A region consists of two locations: a begin location (inclusive) and an end location (exclusive). The two locations must refer
 *  to the same artifact, and the end location must be greater than the begin location. For source regions, if a begin location
 *  lacks a column number, then column 1 is assumed and if an end location lacks a column number then one past the last column is
 *  assumed.
 *
 *  Each location may also have a @ref message "text message" property.
 *
 *  Example:
 *
 *  @snippet{trimleft} sarifUnitTests.C sarif_location */
class Location: public Node {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    // Info for a source location or region. If the end location is valid, then the begin location is also valid.
    SourceLocation sourceBegin_, sourceEnd_;

    // Info for a binary location or region. These are valid only if source is not valid.
    std::string binaryArtifact_;
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
    BinaryAnalysis::AddressInterval binaryRegion_;
#endif

public:
    /** Property: Text message. */
    [[Rosebud::property]]
    std::string message;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Allocating constructor for a source location. */
    static Ptr instance(const SourceLocation&, const std::string &mesg = "");

    /** Allocating constructor for a source region.
     *
     *  See the @ref Location "class documentation" for how @p begin and @p end relate to each other. If the relationship
     *  constraints are violated then an exception is thrown. */
    static Ptr instance(const SourceLocation &begin, const SourceLocation &end, const std::string &mesg = "");

#ifdef ROSE_ENABLE_BINARY_ANALYSIS
    /** Allocating constructor for a binary location.
     *
     *  This is only available when ROSE is configured to support binary analysis. */
    static Ptr instance(const std::string &binaryArtifact, rose_addr_t, const std::string &mesg = "");
#endif

#ifdef ROSE_ENABLE_BINARY_ANALYSIS
    /** Allocating constructor for a binary region.
     *
     *  The region must not be empty or an exception is thrown.
     *
     *  @note Although this API is able to represent a region containing an entire address space, the SARIF design has a flaw that
     *  makes it impossible to represent such a region. Therefore, during output to the SARIF file, such a region will have its size
     *  decreased by one, thereby excluding the maximum address but resulting in a size that can be represented in the same number
     *  of bits as the address.
     *
     *  This is only available when ROSE is configured to support binary analysis. */
    static Ptr instance(const std::string &binaryArtifact, const BinaryAnalysis::AddressInterval&, const std::string &mesg = "");
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Public functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Source location.
     *
     *  Returns the source location. If this is a source region, then the beginning location of the region is returned. If this is a
     *  binary location then an empty source location is returned instead. */
    const SourceLocation& sourceLocation() const;

    /** Source region.
     *
     *  Returns the source region as a pair of locations: the begin (inclusive) and end (exclusive) locations. See the @ref Location
     *  "class documentation" for how these locations relate to each other. If this object represents only a single location instead
     *  of a region, then the second return value is empty. If this is a binary location or region, then both members of the
     *  returned pair will be empty. */
    std::pair<SourceLocation, SourceLocation> sourceRegion() const;

#ifdef ROSE_ENABLE_BINARY_ANALYSIS
    /** Binary location.
     *
     *  Returns the binary location as a pair consisting of the binary artifact name and the address. If this is not a binary
     *  location then an empty string and the address zero is returned. Since the empty string and zero address are a valid binary
     *  location, the correct way to tell whether a location is a binary location is to test whether it is not a @ref sourceLocation
     *  "source location". A location is a binary location if and only if it is not a source location.
     *
     *  This is only available if ROSE is configured to support binary analysis. */
    std::pair<std::string, rose_addr_t> binaryLocation() const;
#endif

#ifdef ROSE_ENABLE_BINARY_ANALYSIS
    /** Binary region.
     *
     *  Returns the binary region as pair consisting of the binary artifact name, and the address interval. If this object stores
     *  only a binary location (not a region) then the address interval is a singleton. If this object stores a source location or
     *  region, then the return value is an empty string and an empty address interval.
     *
     *  This is only available if ROSE is configured to support binary analysis. */
    std::pair<std::string, BinaryAnalysis::AddressInterval> binaryRegion() const;
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Private functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    void checkConsistency() const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Overrides
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    bool emit(std::ostream&) override;
};

} // namespace
} // namespace
#endif
