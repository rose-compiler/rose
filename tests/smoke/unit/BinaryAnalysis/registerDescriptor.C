// Unit tests for register descriptors
#include <rose.h>
#include <RegisterDescriptor.h>

#include <boost/lexical_cast.hpp>

using namespace Rose::BinaryAnalysis;

int
main() {
    // Check default constructor
    RegisterDescriptor a;
    ASSERT_always_require(a.majorNumber() == 0);
    ASSERT_always_require(a.minorNumber() == 0);
    ASSERT_always_require(a.offset() == 0);
    ASSERT_always_require(a.nBits() == 0);
    ASSERT_always_require(a.isEmpty());
    
    // Check that major numbers can be set
    for (unsigned majr = 0; majr < 16; ++majr) {
        a.majorNumber(majr);
        ASSERT_always_require(a.majorNumber() == majr);
    }

    // Check that minor numbers can be set
    for (unsigned minr = 0; minr < 1024; ++minr) {
        a.minorNumber(minr);
        ASSERT_always_require(a.minorNumber() == minr);
    }

    // Check all combinations of offset and size
    for (size_t offset = 0; offset < 512; ++offset) {
        for (size_t nBits = 0; offset + nBits <= 512; ++nBits) {
            a.setOffsetWidth(offset, nBits);
            ASSERT_always_require2(a.offset() == offset,
                                   "set offset=" + boost::lexical_cast<std::string>(offset) + ", "
                                   "nBits=" + boost::lexical_cast<std::string>(nBits) +
                                   "; got offset=" + boost::lexical_cast<std::string>(a.offset()));
            ASSERT_always_require2(a.nBits() == nBits,
                                   "set offset=" + boost::lexical_cast<std::string>(offset) + ", "
                                   "nBits=" + boost::lexical_cast<std::string>(nBits) +
                                   "; got nBits=" + boost::lexical_cast<std::string>(a.nBits()));
        }
    }

    // Check property constructor
    RegisterDescriptor b(15, 1023, 100, 412);
    ASSERT_always_require(b.majorNumber() == 15);
    ASSERT_always_require(b.minorNumber() == 1023);
    ASSERT_always_require(b.offset() ==100);
    ASSERT_always_require(b.nBits() == 412);

    // Check that offset can be decreased without affecting width
    b.offset(50);
    ASSERT_always_require(b.offset() == 50);
    ASSERT_always_require(b.nBits() == 412);

    // Check that increasing offset adjusts width to satisfy offset + width <= 512
    b.offset(500);
    ASSERT_always_require(b.offset() == 500);
    ASSERT_always_require(b.nBits() == 12);

    // Check that API works for const objects
    const RegisterDescriptor &c = b;
    ASSERT_always_require(c.majorNumber() == 15);
    ASSERT_always_require(c.minorNumber() == 1023);
    ASSERT_always_require(c.offset() == 500);
    ASSERT_always_require(c.nBits() == 12);
    ASSERT_always_require(c.hash() == b.hash());
    ASSERT_always_require(c == b);
    ASSERT_always_forbid(c != b);
    ASSERT_always_forbid(c < b);
    ASSERT_always_forbid(b < c);
}
