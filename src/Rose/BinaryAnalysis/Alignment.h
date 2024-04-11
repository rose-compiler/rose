#ifndef ROSE_BinaryAnalysis_Alignment_H
#define ROSE_BinaryAnalysis_Alignment_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <RoseFirst.h>
#include <Sawyer/Optional.h>

namespace Rose {
namespace BinaryAnalysis {

/** Information about alignments. */
class Alignment {
private:
    uint64_t value_ = 1;
    size_t nBits_ = 64;

public:
    /** Constructor with specific alignment.
     *
     *  The first argument is the alignment, and the second argument is the width in bits of the values to be aligned. An alignment
     *  of zero is the same as an alignment of one, and will be returned as one by @ref get.  The number of bits must be between
     *  eight and 64, inclusive. */
    Alignment(uint64_t alignment, size_t nBits);

    /** Assignment.
     *
     * @{ */
    Alignment& operator=(const Alignment&) = default;
    Alignment& operator=(const uint64_t);
    /** @} */

    /** Property: Alignment.
     *
     * @{ */
    uint64_t get() const;
    uint64_t operator*() const;
    void set(uint64_t);
    /** @} */

    /** Property: Number of bits in the values being aligned.
     *
     * @{ */
    size_t nBits() const;
    void nBits(size_t);
    /** @} */

    /** Round value down to closest multiple of the alignment. */
    uint64_t alignDown(uint64_t);

    /** Round value up to the closest multiple of the alignment.
     *
     *  If the value is equal to a multiple of the alignment then the value is returned unchanged, otherwise the value is
     *  incremented until it is a multiple of the alignment. If the next multiple of the alignment is too large to represent in the
     *  specified number of bits, then nothing is returned. */
    Sawyer::Optional<uint64_t> alignUp(uint64_t);
};

} // namespace
} // namespace
#endif
#endif
