#ifndef ROSE_Sarif_Location_H
#define ROSE_Sarif_Location_H
#include <Rose/Sarif/Node.h>

#include <Rose/SourceLocation.h>

namespace Rose {
namespace Sarif {

/** Location for a result.
 *
 *  A location is either a source location or a binary location.
 *
 *  A source location has an artifact (the source file), a starting line number (1 origin), and an optional starting column number
 *  (also 1 origin).
 *
 *  A binary location has an artifact (the binary specimen), and a memory address. */
class Location: public Node {
public:
    /** Shared-ownership pointer to a @ref Location object.
     *
     * @{ */
    using Ptr = LocationPtr;
    using ConstPtr = LocationConstPtr;
    /** @} */

private:
    // The location is stored in either the sourceLocation_ or the other data members.
    SourceLocation sourceLocation_;
    std::string binaryArtifact_;
    rose_addr_t address_ = 0;

    // Each location can have a message
    std::string message_;

public:
    ~Location();
protected:
    explicit Location(const SourceLocation&, const std::string &mesg);
    Location(const std::string &binaryArtifact, rose_addr_t addr, const std::string &mesg);
public:
    /** Allocating constructor for a source location. */
    static Ptr instance(const SourceLocation&, const std::string &mesg = "");

    /** Allocating constructor for a binary location. */
    static Ptr instance(const std::string &binaryArtifact, rose_addr_t, const std::string &mesg = "");

    /** Source location.
     *
     *  Returns the source location. If this is a binary location then returns an empty source location. */
    const SourceLocation& sourceLocation() const;

    /** Binary location.
     *
     *  Returns the binary location as a pair consisting of the binary artifact name and the address. If this is not a binary
     *  location then an empty string and the address zero is returned. Since the empty string and zero address are a valid binary
     *  location, the correct way to tell whether a location is a binary location is to test whether it is a @ref sourceLocation
     *  "source location". A location is a binary location if and only if it is not a source location. */
    std::pair<std::string, rose_addr_t> binaryLocation() const;

    /** Property: Text message.
     *
     * @{ */
    const std::string& message() const;
    void message(const std::string&);
    /** @} */

public:
    void emitYaml(std::ostream&, const std::string&) override;
    std::string emissionPrefix() override;

private:
    void emitMessage(std::ostream&, const std::string &prefix);
};

} // namespace
} // namespace
#endif
