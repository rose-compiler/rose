#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Alignment.h>

#include <Rose/BitOps.h>
#include <Rose/Exception.h>

#include <Sawyer/Assert.h>

namespace Rose {
namespace BinaryAnalysis {

Alignment::Alignment()
    : value_(1), nBits_(0) {}

Alignment::Alignment(const uint64_t alignment, size_t nBits)
    : value_(std::max(uint64_t{1}, alignment)), nBits_(nBits) {
    ASSERT_require(nBits >= 8 && nBits <= 64);
}

Alignment&
Alignment::operator=(const uint64_t value) {
    set(value);
    return *this;
}

uint64_t
Alignment::get() const {
    return value_;
}

uint64_t
Alignment::operator*() const {
    return get();
}

void
Alignment::set(const uint64_t v) {
    value_ = Alignment(v, nBits()).get();
}

size_t
Alignment::nBits() const {
    return nBits_;
}

void
Alignment::nBits(size_t n) {
    nBits_ = Alignment(1, n).nBits();
}

uint64_t
Alignment::alignDown(const uint64_t x) const {
    if (nBits() == 0)
        throw Exception("default-constructed alignment cannot align values");
    const uint64_t maxValue = BitOps::lowMask<uint64_t>(nBits_);
    return (std::min(x, maxValue) / value_) * value_;
}

Sawyer::Optional<uint64_t>
Alignment::alignUp(const uint64_t x) const {
    if (nBits() == 0)
        throw Exception("default-constructed alignment cannot align values");
    const uint64_t retval = ((x + value_ - 1) / value_) * value_;
    const uint64_t maxValue = BitOps::lowMask<uint64_t>(nBits_);
    if (retval >= x && retval <= maxValue) {
        return retval;
    } else {
        return Sawyer::Nothing();
    }
}

bool
Alignment::isAligned(const uint64_t x) const {
    return alignDown(x) == x;
}

} // namespace
} // namespace
#endif
