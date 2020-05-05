#ifndef Rose_BinaryAnalysis_RegisterDescriptor_H
#define Rose_BinaryAnalysis_RegisterDescriptor_H

#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <boost/serialization/access.hpp>

namespace Rose {
namespace BinaryAnalysis {

/** Describes (part of) a physical CPU register.
 *
 *  Some architectures have multiple names for physical registers. For example, amd64 has a 64-bit integer register, parts
 *  of which are referred to as "rax", "eax", "ax", "al", and "ah".  The purpose of a RegisterDescriptor is to describe what
 *  physical register (major and minor) is being referenced, and the part (offset and nbits) that's being referenced.
 *
 *  The reason for having a major and minor number to specify a register is to allow for different types of registers. For
 *  instance, an i686 has a set of 64-bit integer registers and a set of 80-bit floating point registers (among others).
 *  Having major and minor numbers allows the physical register set (such as defined by an instruction semantics policy) to
 *  be implemented as an array of 64-bit integers and an array of 80-bit floating points (among others). The array is selected
 *  by the major number while the element of the array is selected by the minor number.
 *
 *  The RegisterDescriptor type is part of a SgAsmRegisterReferenceExpression and also appears in the register dictionaries
 *  (RegisterDictionary) used in various places including the disassembler. RegisterDescriptor objects are small and are
 *  intended to be passed by value. Although these objects are currently four bytes, we reserve the right to make them larger
 *  (but probably never more than eight bytes). */
class RegisterDescriptor {
    // The major, minor, offset, and size are packed together into this single data member.
    //
    //   + The major number is the high order four bits at position 28-31 (inclusive) with bit #0 being the LSB.
    //   + The minor number is the 10 bits at position 18-27.
    //   + The offset is the 9 bits at position 9-17, but see note below and offset in [0,511].
    //   + The width-1 is the 9 bits at position 0-8, but see note below and width in [1,512].
    //
    // We need to be able to store a width of zero in order to distinguish between valid descriptors and invalid (default
    // constructed) descriptors, but there aren't enough bits in the width field by itself to do that.  We also need to be able
    // to store and retrive the full range of major, minor, and offset values for zero-width descriptors. To accomplish that,
    // we use the invarant that the offset+width of a valid descriptor cannot exceed 512 bits. Therefore, if the width field
    // plus offset field equals 512 then the actual offset is as specified in the range [1,511] and the actual offset is zero;
    // and if the offset field is 2 and the width field is 511 (the EMPTY_PATTERN constant) then the actual offset and actual
    // width are both zero.
    //
    // There are still unused bit combinations that we could use to extend the ranges of any of the four fields. For instance,
    // we don't necessarily need to align the fields on bit boundaries. We could use the fact that registers of size R bits need
    // (R+1)*R/2 values to represent all combinations of offset and size, although this requires integer division to retrieve
    // the values.
    uint32_t data_;

    // This pattern represents major=0, minor=0, offset=0, width=0 (offset field = 2, width field = 511)
    static const uint32_t EMPTY_PATTERN     = 0x000005ff; // EMPTY_PATTERN & ~OFFSET_WIDTH_MASK == 0
    static const uint32_t OFFSET_WIDTH_MASK = 0x0003ffff; // 9-bit offset and 9-bit width

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(data_);
    }
#endif

public:
    /** Create an empty register descriptor.
     *
     *  A default constructed register descriptor has major number zero, minor number zero, offset zero, and size zero. */
    RegisterDescriptor()
        : data_(EMPTY_PATTERN) {}

    /** Construct a descriptor from its constituent parts.
     *
     *  The major number, minor number, offset, and width must satisfy the requirements described for those various
     *  properties. */
    RegisterDescriptor(unsigned majr, unsigned minr, size_t offset, size_t width)
        : data_(0) {
        majorNumber(majr);
        minorNumber(minr);
        setOffsetWidth(offset, width);
    }
    
    /** Property: Major number.
     *
     *  Each family of registers is identified by a major number. Major numbers are in the range 0 through 15, inclusive.
     *
     * @{ */
    unsigned majorNumber() const {
        return data_ >> 28;                             // bits 28-31 (4 bits)
    }
    void majorNumber(unsigned);
    /** @} */

    /** Property: Minor number.
     *
     *  Within a family of registers related by their @ref majorNumber, the @p minorNumber identifies individuals. Minor
     *  numbers are in the range 0 through 1023, inclusive.
     *
     * @{ */
    unsigned minorNumber() const {
        return (data_ >> 18) & 0x3ff;                   // bits 18-27 (10 bits)
    }
    void minorNumber(unsigned);
    /** @} */

    /** Property: Offset to least-significant bit.
     *
     *  Each member of a register family (member identified by major and minor number) is conceptually 512 bits wide. A @p
     *  RegisterDescriptor has an offset and width (@ref nBits property) to describe a contiguous region of that space. The sum
     *  of the offset and width cannot exceed 512. Decreasing the offset preserves the width; increasing the offset may
     *  truncate the width. The offset can be set and queried even for descriptors that are empty (width==0).
     *
     *  See also, @ref setOffsetWidth which sets both offset and size at once.
     *
     * @{ */
    inline size_t offset() const {
        if ((data_ & OFFSET_WIDTH_MASK) == EMPTY_PATTERN)
            return 0;
        unsigned offsetField = (data_ >> 9) & 0x1ff;    // bits 9-17 (9 bits)
        unsigned widthField = data_ & 0x1ff;            // bits 0-8 (9 bits)
        if (offsetField + widthField == 512)
            return offsetField;
        return offsetField;
    }
    void offset(size_t);
    /** @} */

    /** Property: Size in bits.
     *
     *  Each member of a register family (member identified by major and minor number) is conceptually 512 bits wide. A @p
     *  RegisterDescriptor has an @ref offset and this width-in-bits property to describe a contiguous region of that
     *  space. The sum of the offset and width cannot exceed 512 (the @ref setOffsetWidth function sets both at once).
     *
     * @{ */
    inline size_t nBits() const {
        if ((data_ & OFFSET_WIDTH_MASK) == EMPTY_PATTERN)
            return 0;
        unsigned offsetField = (data_ >> 9) & 0x1ff;    // bits 9-17 (9 bits)
        unsigned widthField = data_ & 0x1ff;            // bits 0-8 (9 bits)
        if (offsetField + widthField == 512)
            return 0;
        return widthField + 1;
    }
    void nBits(size_t);
    /** @} */

    /** Set offset and size at the same time.
     *
     *  Adjusting the offset and width individually with the @ref offset and @ref nBits properties can be tricky because you
     *  need to always satisfy the invariant that offset + size <= 512.  Setting the @ref offset individually may change the
     *  width, and setting the width individually might cause an invariant to be violated. Therefore, the recommended way to
     *  adjust both is to use the @p setOffsetWidth method.
     *
     *  The offset and width are set as indicated. Their sum must not exceed 512. */
    void setOffsetWidth(size_t offset, size_t nBits);

    /** Predicate returns true if the width is zero.
     *
     *  Default-constructed register descriptors have an initial width of zero. */
    bool isEmpty() const {
        return 0 == nBits();
    }

    /** Predicate returns true if width is non-zero.
     *
     *  Default-constructed register descriptors have an initial width of zero. */
    bool isValid() const {
        return nBits() != 0;
    }

    // The following trickery is to have something like "explicit operator bool" before C++11
private:
    typedef void(RegisterDescriptor::*unspecified_bool)() const;
    void this_type_does_not_support_comparisons() const {}
public:
    operator unspecified_bool() const {
        return isEmpty() ? 0 : &RegisterDescriptor::this_type_does_not_support_comparisons;
    }

    /** Compare two descriptors.
     *
     *  Descriptors are sorted by major and minor numbers. If two descriptors have the same major and minor numbers then this
     *  less-than operation satisfies the requirements of a strict weak ordering although the exact details are not specified
     *  (it is not a simple comparison of offset and width). */
    bool operator<(RegisterDescriptor other) const {
        return data_ < other.data_;
    }

    /** Check descriptors for equality.
     *
     *  Two descriptors are equal if and only if they have the same major numbers, minor numbers, offsets, and width. Note that
     *  all properties are significant even if the width is zero or @ref isEmpty returns true. */
    bool operator==(RegisterDescriptor other) const {
        return data_ == other.data_;
    }

    /** Check descriptors for inequality.
     *
     *  Two descriptors are unequal if they differ in major numbers, minor numbers, offsets, or widths. Note that all
     *  properties are significant even if the width is zero or @ref isEmpty returns true. */
    bool operator!=(RegisterDescriptor other) const {
        return data_ != other.data_;
    }

    /** Hash value.
     *
     *  Hash a descriptor. This is a perfect hash: every descriptor hashes to a distinct value. */
    unsigned hash() const {
        return data_;
    }

    /** Print all properties. */
    void print(std::ostream &o) const {
        o <<"{" <<majorNumber() <<"," <<minorNumber() <<"," <<offset() <<"," <<nBits() <<"}";
    }

    friend std::ostream& operator<<(std::ostream&, RegisterDescriptor);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Old interface  (not deprecated yet, just implemented in terms of the new interface)
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned get_major() const {
        return majorNumber();
    }
    bool is_valid() const {
        return !isEmpty();
    }
    RegisterDescriptor &set_major(unsigned majr) {
        majorNumber(majr);
        return *this;
    }
    unsigned get_minor() const {
        return minorNumber();
    }
    RegisterDescriptor &set_minor(unsigned minr) {
        this->minorNumber(minr);
        return *this;
    }
    unsigned get_offset() const {
        return offset();
    }
    RegisterDescriptor &set_offset(unsigned offset) {
        this->offset(offset);
        return *this;
    }
    unsigned get_nbits() const {
        return nBits();
    }
    RegisterDescriptor &set_nbits(unsigned nbits) {
        this->nBits(nbits);
        return *this;
    }
};

} // namespace
} // namespace

#endif
#endif
