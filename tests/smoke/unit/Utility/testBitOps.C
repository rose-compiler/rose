#include <BitOps.h>

#include <boost/lexical_cast.hpp>
#include <Sawyer/Assert.h>
#include <string>

#define check(FUNC, ANS) do {                                                                                                  \
    ASSERT_always_require2(FUNC == ANS, "expected " + binary(ANS) + " but got " + binary(FUNC));                               \
    ASSERT_always_require2(sizeof(FUNC) == sizeof(ANS),                                                                        \
                           "expected result is " + boost::lexical_cast<std::string>(8*sizeof(ANS)) +                           \
                           " bits but actual result is " + boost::lexical_cast<std::string>(8*sizeof(FUNC)) + " bits");        \
} while(0);

using namespace Rose::BitOps;

template<typename T>
static std::string
binary(T x) {
    std::string s = "0b";
    for (size_t i = 8*sizeof(T); i > 0; --i)
        s += x & (T(1) << (i-1)) ? '1' : '0';
    return s;
}

static void
testNBits() {
    check(nBits<uint8_t>(),  size_t( 8));
    check(nBits<uint16_t>(), size_t(16));
    check(nBits<uint32_t>(), size_t(32));
    check(nBits<uint64_t>(), size_t(64));

    uint8_t u8 = 0;
    uint16_t u16 = 0;
    uint32_t u32 = 0;
    uint64_t u64 = 0;

    check(nBits(u8),  size_t( 8));
    check(nBits(u16), size_t(16));
    check(nBits(u32), size_t(32));
    check(nBits(u64), size_t(64));
}

static void
testAll() {
    check(all<uint8_t>(),  uint8_t(0xff));
    check(all<uint16_t>(), uint16_t(0xffff));
    check(all<uint32_t>(), uint32_t(0xffffffff));
    check(all<uint64_t>(), uint64_t(0xffffffffffffffffuLL));

    check(all<uint8_t>(false),  uint8_t(0));
    check(all<uint16_t>(false), uint16_t(0));
    check(all<uint32_t>(false), uint32_t(0));
    check(all<uint64_t>(false), uint64_t(0));
}

static void
testLowMask() {
    check(lowMask<uint8_t>(0), uint8_t(0b00000000));
    check(lowMask<uint8_t>(1), uint8_t(0b00000001));
    check(lowMask<uint8_t>(2), uint8_t(0b00000011));
    check(lowMask<uint8_t>(3), uint8_t(0b00000111));
    check(lowMask<uint8_t>(4), uint8_t(0b00001111));
    check(lowMask<uint8_t>(5), uint8_t(0b00011111));
    check(lowMask<uint8_t>(6), uint8_t(0b00111111));
    check(lowMask<uint8_t>(7), uint8_t(0b01111111));
    check(lowMask<uint8_t>(8), uint8_t(0b11111111));
    check(lowMask<uint8_t>(9), uint8_t(0b11111111));

    check(lowMask<uint64_t>(0),  uint64_t(0));
    check(lowMask<uint64_t>(1),  uint64_t(1));
    check(lowMask<uint64_t>(63), uint64_t(0x7fffffffffffffffuLL));
    check(lowMask<uint64_t>(64), uint64_t(0xffffffffffffffffuLL));
    check(lowMask<uint64_t>(65), uint64_t(0xffffffffffffffffuLL));
}

static void
testHighMask() {
    check(highMask<uint8_t>(0), uint8_t(0b00000000));
    check(highMask<uint8_t>(1), uint8_t(0b10000000));
    check(highMask<uint8_t>(2), uint8_t(0b11000000));
    check(highMask<uint8_t>(3), uint8_t(0b11100000));
    check(highMask<uint8_t>(4), uint8_t(0b11110000));
    check(highMask<uint8_t>(5), uint8_t(0b11111000));
    check(highMask<uint8_t>(6), uint8_t(0b11111100));
    check(highMask<uint8_t>(7), uint8_t(0b11111110));
    check(highMask<uint8_t>(8), uint8_t(0b11111111));
    check(highMask<uint8_t>(9), uint8_t(0b11111111));

    check(highMask<uint64_t>(0),  uint64_t(0));
    check(highMask<uint64_t>(1),  uint64_t(0x8000000000000000uLL));
    check(highMask<uint64_t>(63), uint64_t(0xfffffffffffffffeuLL));
    check(highMask<uint64_t>(64), uint64_t(0xffffffffffffffffuLL));
    check(highMask<uint64_t>(65), uint64_t(0xffffffffffffffffuLL));
}

static void
testShiftLeft() {
    check(shiftLeft(uint8_t(0b00001000), 0, false), uint8_t(0b00001000));
    check(shiftLeft(uint8_t(0b00001000), 1, false), uint8_t(0b00010000));
    check(shiftLeft(uint8_t(0b00001000), 2, false), uint8_t(0b00100000));
    check(shiftLeft(uint8_t(0b00001000), 3, false), uint8_t(0b01000000));
    check(shiftLeft(uint8_t(0b00001000), 4, false), uint8_t(0b10000000));
    check(shiftLeft(uint8_t(0b00001000), 5, false), uint8_t(0b00000000));
    check(shiftLeft(uint8_t(0b00001000), 8, false), uint8_t(0b00000000));

    check(shiftLeft(uint8_t(0b00001000), 0,  true), uint8_t(0b00001000));
    check(shiftLeft(uint8_t(0b00001000), 1,  true), uint8_t(0b00010001));
    check(shiftLeft(uint8_t(0b00001000), 2,  true), uint8_t(0b00100011));
    check(shiftLeft(uint8_t(0b00001000), 3,  true), uint8_t(0b01000111));
    check(shiftLeft(uint8_t(0b00001000), 4,  true), uint8_t(0b10001111));
    check(shiftLeft(uint8_t(0b00001000), 5,  true), uint8_t(0b00011111));
    check(shiftLeft(uint8_t(0b00001000), 8,  true), uint8_t(0b11111111));
}

static void
testShiftRight() {
    check(shiftRight(uint8_t(0b00001000), 0, false), uint8_t(0b00001000));
    check(shiftRight(uint8_t(0b00001000), 1, false), uint8_t(0b00000100));
    check(shiftRight(uint8_t(0b00001000), 2, false), uint8_t(0b00000010));
    check(shiftRight(uint8_t(0b00001000), 3, false), uint8_t(0b00000001));
    check(shiftRight(uint8_t(0b00001000), 4, false), uint8_t(0b00000000));
    check(shiftRight(uint8_t(0b00001000), 5, false), uint8_t(0b00000000));
    check(shiftRight(uint8_t(0b00001000), 8, false), uint8_t(0b00000000));

    check(shiftRight(uint8_t(0b00001000), 0,  true), uint8_t(0b00001000));
    check(shiftRight(uint8_t(0b00001000), 1,  true), uint8_t(0b10000100));
    check(shiftRight(uint8_t(0b00001000), 2,  true), uint8_t(0b11000010));
    check(shiftRight(uint8_t(0b00001000), 3,  true), uint8_t(0b11100001));
    check(shiftRight(uint8_t(0b00001000), 4,  true), uint8_t(0b11110000));
    check(shiftRight(uint8_t(0b00001000), 5,  true), uint8_t(0b11111000));
    check(shiftRight(uint8_t(0b00001000), 8,  true), uint8_t(0b11111111));
}

static void
testPosition() {
    check(position<uint8_t>(0), uint8_t(0b00000001));
    check(position<uint8_t>(1), uint8_t(0b00000010));
    check(position<uint8_t>(2), uint8_t(0b00000100));
    check(position<uint8_t>(3), uint8_t(0b00001000));
    check(position<uint8_t>(4), uint8_t(0b00010000));
    check(position<uint8_t>(5), uint8_t(0b00100000));
    check(position<uint8_t>(6), uint8_t(0b01000000));
    check(position<uint8_t>(7), uint8_t(0b10000000));
    check(position<uint8_t>(8), uint8_t(0b00000000));
}

static void
testMask() {
    check(mask<uint8_t>(0, 7), uint8_t(0b11111111));
    check(mask<uint8_t>(1, 7), uint8_t(0b11111110));
    check(mask<uint8_t>(1, 6), uint8_t(0b01111110));
    check(mask<uint8_t>(2, 6), uint8_t(0b01111100));
    check(mask<uint8_t>(2, 5), uint8_t(0b00111100));
    check(mask<uint8_t>(3, 5), uint8_t(0b00111000));
    check(mask<uint8_t>(3, 4), uint8_t(0b00011000));
    check(mask<uint8_t>(4, 4), uint8_t(0b00010000));
}

static void
testBit() {
    check(bit(uint8_t(0b10000010), 0), false);
    check(bit(uint8_t(0b10000010), 1),  true);
    check(bit(uint8_t(0b10000010), 2), false);
    check(bit(uint8_t(0b10000010), 3), false);
    check(bit(uint8_t(0b10000010), 4), false);
    check(bit(uint8_t(0b10000010), 5), false);
    check(bit(uint8_t(0b10000010), 6), false);
    check(bit(uint8_t(0b10000010), 7),  true);
    check(bit(uint8_t(0b10000010), 8), false);
}

static void
testMsb() {
    check(msb(uint8_t(0x7f)), false);
    check(msb(uint8_t(0x80)),  true);
    check(msb(uint16_t(0x7fff)), false);
    check(msb(uint16_t(0x8000)),  true);
    check(msb(uint32_t(0x7fffffff)), false);
    check(msb(uint32_t(0x80000000)),  true);
    check(msb(uint64_t(0x7fffffffffffffffuLL)), false);
    check(msb(uint64_t(0x8000000000000000uLL)),  true);
}

static void
testShiftRightSigned() {
    check(shiftRightSigned(uint8_t(0b01100000), 0), uint8_t(0b01100000));
    check(shiftRightSigned(uint8_t(0b01100000), 1), uint8_t(0b00110000));
    check(shiftRightSigned(uint8_t(0b01100000), 2), uint8_t(0b00011000));
    check(shiftRightSigned(uint8_t(0b01100000), 3), uint8_t(0b00001100));
    check(shiftRightSigned(uint8_t(0b01100000), 4), uint8_t(0b00000110));
    check(shiftRightSigned(uint8_t(0b01100000), 5), uint8_t(0b00000011));
    check(shiftRightSigned(uint8_t(0b01100000), 6), uint8_t(0b00000001));
    check(shiftRightSigned(uint8_t(0b01100000), 7), uint8_t(0b00000000));
    check(shiftRightSigned(uint8_t(0b01100000), 8), uint8_t(0b00000000));

    check(shiftRightSigned(uint8_t(0b10110000), 0), uint8_t(0b10110000));
    check(shiftRightSigned(uint8_t(0b10110000), 1), uint8_t(0b11011000));
    check(shiftRightSigned(uint8_t(0b10110000), 2), uint8_t(0b11101100));
    check(shiftRightSigned(uint8_t(0b10110000), 3), uint8_t(0b11110110));
    check(shiftRightSigned(uint8_t(0b10110000), 4), uint8_t(0b11111011));
    check(shiftRightSigned(uint8_t(0b10110000), 5), uint8_t(0b11111101));
    check(shiftRightSigned(uint8_t(0b10110000), 6), uint8_t(0b11111110));
    check(shiftRightSigned(uint8_t(0b10110000), 7), uint8_t(0b11111111));
    check(shiftRightSigned(uint8_t(0b10110000), 8), uint8_t(0b11111111));
}

static void
testBits() {
    check(bits(uint8_t(0b10110011), 0, 7), uint8_t(0b10110011));
    check(bits(uint8_t(0b10110011), 1, 7), uint8_t(0b01011001));
    check(bits(uint8_t(0b10110011), 2, 7), uint8_t(0b00101100));
    check(bits(uint8_t(0b10110011), 3, 7), uint8_t(0b00010110));
    check(bits(uint8_t(0b10110011), 4, 7), uint8_t(0b00001011));
    check(bits(uint8_t(0b10110011), 5, 7), uint8_t(0b00000101));
    check(bits(uint8_t(0b10110011), 6, 7), uint8_t(0b00000010));
    check(bits(uint8_t(0b10110011), 7, 7), uint8_t(0b00000001));

    check(bits(uint8_t(0b10110011), 0, 7), uint8_t(0b10110011));
    check(bits(uint8_t(0b10110011), 0, 6), uint8_t(0b00110011));
    check(bits(uint8_t(0b10110011), 0, 5), uint8_t(0b00110011));
    check(bits(uint8_t(0b10110011), 0, 4), uint8_t(0b00010011));
    check(bits(uint8_t(0b10110011), 0, 3), uint8_t(0b00000011));
    check(bits(uint8_t(0b10110011), 0, 2), uint8_t(0b00000011));
    check(bits(uint8_t(0b10110011), 0, 1), uint8_t(0b00000011));
    check(bits(uint8_t(0b10110011), 0, 0), uint8_t(0b00000001));
}

static void
testConvert() {
    check(convert<uint8_t>(uint32_t(0x9ec48db3)), uint8_t(0xb3));
    check(convert<uint32_t>(uint8_t(0xb3)), uint32_t(0x000000b3));
}

static void
testConvertSigned() {
    check(convertSigned<uint8_t>(uint32_t(0x9ec48db3)), uint8_t(0xb3));
    check(convertSigned<uint32_t>(uint8_t(0xb3)), uint32_t(0xffffffb3));
}

static void
testRotateLeft() {
    check(rotateLeft(uint8_t(0b10110111),  0), uint8_t(0b10110111));
    check(rotateLeft(uint8_t(0b10110111),  1), uint8_t(0b01101111));
    check(rotateLeft(uint8_t(0b10110111),  2), uint8_t(0b11011110));
    check(rotateLeft(uint8_t(0b10110111),  3), uint8_t(0b10111101));
    check(rotateLeft(uint8_t(0b10110111),  4), uint8_t(0b01111011));
    check(rotateLeft(uint8_t(0b10110111),  5), uint8_t(0b11110110));
    check(rotateLeft(uint8_t(0b10110111),  6), uint8_t(0b11101101));
    check(rotateLeft(uint8_t(0b10110111),  7), uint8_t(0b11011011));

    check(rotateLeft(uint8_t(0b10110111),  8), uint8_t(0b10110111));
    check(rotateLeft(uint8_t(0b10110111),  9), uint8_t(0b01101111));
    check(rotateLeft(uint8_t(0b10110111), 10), uint8_t(0b11011110));
    check(rotateLeft(uint8_t(0b10110111), 11), uint8_t(0b10111101));
    check(rotateLeft(uint8_t(0b10110111), 12), uint8_t(0b01111011));
    check(rotateLeft(uint8_t(0b10110111), 13), uint8_t(0b11110110));
    check(rotateLeft(uint8_t(0b10110111), 14), uint8_t(0b11101101));
    check(rotateLeft(uint8_t(0b10110111), 15), uint8_t(0b11011011));
}

static void
testRotateRight() {
    check(rotateRight(uint8_t(0b10110111),  0), uint8_t(0b10110111));
    check(rotateRight(uint8_t(0b10110111),  1), uint8_t(0b11011011));
    check(rotateRight(uint8_t(0b10110111),  2), uint8_t(0b11101101));
    check(rotateRight(uint8_t(0b10110111),  3), uint8_t(0b11110110));
    check(rotateRight(uint8_t(0b10110111),  4), uint8_t(0b01111011));
    check(rotateRight(uint8_t(0b10110111),  5), uint8_t(0b10111101));
    check(rotateRight(uint8_t(0b10110111),  6), uint8_t(0b11011110));
    check(rotateRight(uint8_t(0b10110111),  7), uint8_t(0b01101111));

    check(rotateRight(uint8_t(0b10110111),  8), uint8_t(0b10110111));
    check(rotateRight(uint8_t(0b10110111),  9), uint8_t(0b11011011));
    check(rotateRight(uint8_t(0b10110111), 10), uint8_t(0b11101101));
    check(rotateRight(uint8_t(0b10110111), 11), uint8_t(0b11110110));
    check(rotateRight(uint8_t(0b10110111), 12), uint8_t(0b01111011));
    check(rotateRight(uint8_t(0b10110111), 13), uint8_t(0b10111101));
    check(rotateRight(uint8_t(0b10110111), 14), uint8_t(0b11011110));
    check(rotateRight(uint8_t(0b10110111), 15), uint8_t(0b01101111));
}

int
main() {
    testNBits();
    testAll();
    testLowMask();
    testHighMask();
    testShiftLeft();
    testShiftRight();
    testPosition();
    testMask();
    testBit();
    testMsb();
    testShiftRightSigned();
    testBits();
    testConvert();
    testConvertSigned();
    testRotateLeft();
    testRotateRight();
}
