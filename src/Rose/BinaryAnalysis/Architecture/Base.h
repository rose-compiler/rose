#ifndef ROSE_BinaryAnalysis_Architecture_Base_H
#define ROSE_BinaryAnalysis_Architecture_Base_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Architecture/BasicTypes.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

/** Base class for architecture definitions. */
class Base {
public:
    using Ptr = BasePtr;

private:
    std::string name_;                                  // name of architecture

public:
    /** Property: Architecture definition name.
     *
     *  The name is used for lookups, but it need not be unique since lookups prefer the latest registered architecture. I.e., if
     *  two architectures A, and B, have the same name, and B was registered after A, then lookup by the name will return
     *  architecture B.
     *
     *  A best practice is to use only characters that are not special in shell scripts since architecture names often appear as
     *  arguments to command-line switches. Also, try to use only lower-case letters, decimal digits and hyphens for consistency
     *  across all architecture names. See the list of ROSE built-in architecture names for ideas (this list can be obtained from
     *  many binary analysis tools, or the @ref Architecture::registeredNames function).
     *
     *  Thread safety: Not thread safe.
     *
     * @{ */
    const std::string &name() const;
    void name(const std::string&);
    /** @} */
};

} // namespace
} // namespace
} // namespace

#endif
#endif
