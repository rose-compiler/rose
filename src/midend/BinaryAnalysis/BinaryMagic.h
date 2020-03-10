#ifndef ROSE_BinaryAnalysis_MagicNumber_H
#define ROSE_BinaryAnalysis_MagicNumber_H

#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <MemoryMap.h>

namespace Rose {
namespace BinaryAnalysis {

/** Identifies magic numbers in binaries.
 *
 *  The analysis constructor parses and stores the system's magic(5) files, which is then reused for each query. */
class MagicNumberDetails;

class MagicNumber {
public:
    enum Mechanism { FAST, SLOW, NONE };

private:
    MagicNumberDetails *details_;
    Mechanism mechanism_;
    size_t maxBytes_;

public:
    /** Create a magic number analyzer. */
    MagicNumber(): details_(NULL), maxBytes_(256) {
        init();
    }

    ~MagicNumber();

    /** Property: The mechanism being used to find magic numbers.
     *
     *  Multiple machanisms are available:
     *
     *  @li If the libmagic library is available then that mechanism is used the the return value is @ref FAST.
     *
     *  @li If libmagic is not available and this is a Unix machine, then the file(1) command is invoked on a temporary
     *  file. The return value in this case is @ref SLOW.
     *
     *  @li If this is Windows and the libmagic library is not available then the return value is NONE. In this case, all calls
     *  to identify will throw a <code>std::runtime_error</code>.
     *
     *  This property is read-only. */
    Mechanism mechanism() const { return mechanism_; }

    /** Property: Max number of bytes to check at once.
     *
     *  This property is the maximum number of bytes that should be passed at once to the magic-number checking funtions.  The
     *  library imposes a hard-coded limit, but the user may set this to a lower value to gain speed. Trying to set this to a
     *  higher value than the hard-coded limit will result in using the hard-coded limit.
     *
     * @{ */
    size_t maxBytesToCheck() const { return maxBytes_; }
    void maxBytesToCheck(size_t n) { maxBytes_ = n; }
    /** @} */

    /** Identify the magic number at the specified address. */
    std::string identify(const MemoryMap::Ptr&, rose_addr_t va) const;

private:
    void init();
};

} // namespace
} // namespace

#endif
#endif
