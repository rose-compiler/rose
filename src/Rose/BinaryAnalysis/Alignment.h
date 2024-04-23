#ifndef ROSE_BinaryAnalysis_Alignment_H
#define ROSE_BinaryAnalysis_Alignment_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <RoseFirst.h>
#include <Sawyer/Optional.h>

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
#include <boost/serialization/access.hpp>
#endif

namespace Rose {
namespace BinaryAnalysis {

/** Information about alignments. */
class Alignment {
private:
    uint64_t value_ = 1;
    size_t nBits_ = 64;

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(value_);
        s & BOOST_SERIALIZATION_NVP(nBits_);
    }
#endif

public:
    /** Default constructor.
     *
     *  The resulting object is not usable until the @ref nBits property is set. */
    Alignment();

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
     *  This property is zero for default-constructed alignments. It can only be set to positive values between 8 and 64, inclusive.
     *
     * @{ */
    size_t nBits() const;
    void nBits(size_t);
    /** @} */

    /** Round value down to closest multiple of the alignment.
     *
     *  An exception is thrown if this alignment has zero width, such as when it is default constructed. */
    uint64_t alignDown(uint64_t) const;

    /** Round value up to the closest multiple of the alignment.
     *
     *  If the value is equal to a multiple of the alignment then the value is returned unchanged, otherwise the value is
     *  incremented until it is a multiple of the alignment. If the next multiple of the alignment is too large to represent in the
     *  specified number of bits, then nothing is returned.
     *
     *  An exception is thrown if this alignment has zero width, such as when it is default constructed. */
    Sawyer::Optional<uint64_t> alignUp(uint64_t) const;

    /** Test whether a value is aligned.
     *
     *  An exception is thrown if this alignment has zero width, such as when it is default constructed. */
    bool isAligned(uint64_t) const;
};

} // namespace
} // namespace
#endif
#endif
