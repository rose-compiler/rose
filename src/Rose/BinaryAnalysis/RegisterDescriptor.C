#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/RegisterDescriptor.h>

#include <sstream>

namespace Rose {
namespace BinaryAnalysis {

unsigned
RegisterDescriptor::maxMajor() {
    return 16;
}

unsigned
RegisterDescriptor::maxMinor() {
    return 1023;
}

unsigned
RegisterDescriptor::maxOffset() {
    return 511;
}

unsigned
RegisterDescriptor::maxNBits() {
    return 512;
}

void
RegisterDescriptor::majorNumber(unsigned majr) {
    ASSERT_require(majr <= maxMajor());
    data_ &= 0x0fffffff;
    data_ |= (majr << 28);
}

void
RegisterDescriptor::minorNumber(unsigned minr) {
    ASSERT_require(minr <= maxMinor());
    data_ &= 0xf003ffff;
    data_ |= (minr << 18);
}

void
RegisterDescriptor::setOffsetWidth(size_t offset, size_t width) {
    ASSERT_require(offset < 512);
    ASSERT_require(offset + width <= 512);

    data_ &= ~OFFSET_WIDTH_MASK;
    if (0 == offset && 0 == width) {
        data_ |= EMPTY_PATTERN;
    } else if (0 == width) {
        unsigned widthField = 512 - offset;
        data_ |= (offset << 9) | widthField;
    } else {
        data_ |= (offset << 9) | (width - 1);
    }

    ASSERT_require(this->offset() == offset);
    ASSERT_require(this->nBits() == width);
}

void
RegisterDescriptor::offset(size_t offset) {
    ASSERT_require(offset < 512);
    size_t width = std::min(nBits(), 512-offset);
    setOffsetWidth(offset, width);
}

void
RegisterDescriptor::nBits(size_t width) {
    ASSERT_require(width <= 512);
    setOffsetWidth(offset(), width);
}

RegisterDescriptor
RegisterDescriptor::operator&(RegisterDescriptor other) const {
    if (majorNumber() == other.majorNumber() && minorNumber() == other.minorNumber()) {
        const size_t begin1 = offset();
        const size_t end1 = begin1 + nBits();
        const size_t begin2 = other.offset();
        const size_t end2 = begin2 + other.nBits();
        const size_t begin = std::max(begin1, begin2);
        const size_t end = std::min(end1, end2);
        if (end <= begin) {
            return RegisterDescriptor();
        } else {
            const size_t width = end - begin;
            return RegisterDescriptor(majorNumber(), minorNumber(), begin, width);
        }
    } else {
        return RegisterDescriptor();
    }
}

bool
RegisterDescriptor::isSubsetOf(RegisterDescriptor other) const {
    if (const RegisterDescriptor intersection = *this & other) {
        return intersection.offset() == offset() && intersection.nBits() == nBits();
    } else {
        return false;
    }
}

std::string
RegisterDescriptor::toString() const {
    std::ostringstream ss;
    ss <<*this;
    return ss.str();
}

std::ostream&
operator<<(std::ostream &o, RegisterDescriptor reg) {
    reg.print(o);
    return o;
}

} // namespace
} // namespace

#endif
