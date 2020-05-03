#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include <sage3basic.h>
#include <RegisterDescriptor.h>

namespace Rose {
namespace BinaryAnalysis {

void
RegisterDescriptor::majorNumber(unsigned majr) {
    ASSERT_require(majr < 16);
    data_ &= 0x0fffffff;
    data_ |= (majr << 28);
}

void
RegisterDescriptor::minorNumber(unsigned minr) {
    ASSERT_require(minr < 1024);
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

std::ostream&
operator<<(std::ostream &o, RegisterDescriptor reg) {
    reg.print(o);
    return o;
}

} // namespace
} // namespace

#endif
