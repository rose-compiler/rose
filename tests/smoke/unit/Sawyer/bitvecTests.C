// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#include <Sawyer/BitVector.h>
#include <Sawyer/Assert.h>

#include <iostream>

#define check(COND) ASSERT_always_require((COND))

using namespace Sawyer::Container;
typedef BitVector::BitRange BitRange;

template<class Vec>
static void show(const Vec &vector, const std::string &note = "") {
    std::cout <<"    vector[" <<vector.size() <<"] = { " <<vector.toHex() <<" }"
              <<(note.empty() ? "":"\t\t// ") <<note <<"\n";
}

template<class Vec>
static void showBin(const Vec &vector, const std::string &note = "") {
    std::cout <<"    vector[" <<vector.size() <<"] = { ";
    for (size_t i=vector.size(); i>0; --i) {
        if (0 == i%4 && i!=vector.size())
            std::cout <<"_";
        std::cout <<(vector.get(i-1) ? "1" : "0");
    }
    std::cout <<" }" <<(note.empty() ? "" : "\t\t// ") <<note <<"\n";
}

static void ctor_tests() {
    std::cout <<"constructors\n";

    std::cout <<"  default constructor\n";
    BitVector v1;
    show(v1);
    check(v1.size()==0);
    check(v1.isEmpty());

    std::cout <<"  initialized to zero\n";
    BitVector v2(40);
    show(v2);
    check(v2.size()==40);
    check(!v2.isEmpty());

    std::cout <<"  initialized to one\n";
    BitVector v3(40, true);
    show(v3);
    check(v3.size()==40);
    check(!v2.isEmpty());
}

static void string_tests() {
    std::cout <<"string conversion\n";
    BitVector v1(40);
    v1.fromHex("fedc_ba98_7654_3210");
    show(v1);
    check(v1.toHex()=="9876543210");
    check(v1.toOctal()=="11416625031020");
    check(v1.toBinary()=="1001100001110110010101000011001000010000");
    check(v1.toHex(BitRange::hull(10, 25)) == "950c");

    BitVector v2 = BitVector::parse("0x12345");
    check(v2.size()==20);
    check(v2.toInteger() == 0x12345);

    BitVector v3 = BitVector::parse("10");
    check(v3.size()==7);
    check(v3.toInteger() == 10);
}

static void copy_ctor_tests() {
    std::cout <<"copy constructor\n";
    BitVector v1(40);
    v1.fromHex("9876543210");
    BitVector v2(v1);
    show(v2);
    check(v2.toHex()=="9876543210");
}

static void assignment_tests() {
    std::cout <<"assignment tests\n";
    BitVector v1(40);
    v1.fromHex("9876543210");
    BitVector v2(10);
    v2.fromBinary("1011010010");
    v2 = v1;
    show(v2);
    check(v2.toHex()=="9876543210");
}

static void resize_tests() {
    std::cout <<"resize tests\n";
    BitVector v1(40);
    v1.fromHex("9876543210");
    show(v1, "initial value");

    v1.resize(44);
    show(v1, "resized to 44");
    check(v1.size()==44);
    check(v1.toHex()=="09876543210");

    v1.resize(40);
    show(v1, "resized to 40");
    check(v1.size()==40);
    check(v1.toHex()=="9876543210");

    v1.resize(48, true);
    show(v1, "resized to 88 with true");
    check(v1.size()==48);
    check(v1.toHex()=="ff9876543210");
}

static void getbit_tests() {
    std::cout <<"get-bit tests\n";
    BitVector v1(40);
    v1.fromHex("9876543210");
    std::string answer = v1.toBinary();
    std::reverse(answer.begin(), answer.end());
    std::cout <<"  bits:     " <<answer <<"\t// reversed\n  checking: ";
    for (size_t i=0; i<v1.size(); ++i) {
        std::cout <<answer[i];
        check(v1.get(i) == (answer[i]=='1'));
    }
    std::cout <<"\n";
}

static void clearbit_tests() {
    std::cout <<"clear bits\n";
    BitVector v1(40);
    v1.fromHex("9876543210");
    showBin(v1, "initial value");

    v1.clear(BitRange::baseSize(20, 9));
    showBin(v1, "cleared [20+9]");
    check(v1.toHex()=="9860043210");

    v1.clear(0);
    showBin(v1, "clear bit zero");
    check(v1.toHex()=="9860043210");

    v1.clear();
    showBin(v1, "cleared all");
    check(v1.toHex()=="0000000000");
}

static void setbit_tests() {
    std::cout <<"set bits\n";
    BitVector v1(40);
    v1.fromHex("9876543210");
    showBin(v1, "initial value");

    v1.set(BitRange::baseSize(20, 9));
    showBin(v1, "set [20+9]");
    check(v1.toHex()=="987ff43210");

    v1.set(1);
    showBin(v1, "set bit 1");
    check(v1.toHex()=="987ff43212");

    v1.set();
    showBin(v1, "set all");
    check(v1.toHex()=="ffffffffff");

    v1.setValue(BitRange::baseSize(20, 9), false);
    showBin(v1, "set [20+9] false");
    check(v1.toHex()=="ffe00fffff");
}

static void copybit_tests() {
    std::cout <<"copy bits\n";
    std::cout <<"  initializing\n";
    BitVector v1(40), v2(20);
    v1.fromHex("9876543210");
    v2.fromHex("abcde");
    showBin(v1, "initial target");
    showBin(v2, "initial source");

    std::cout <<"  copy from one vector to another\n";
    v1.copy(BitRange::hull(20, 28), v2, BitRange::hull(1, 9));
    showBin(v1, "copied [1-9] to [20-28]");
    check(v1.toHex()=="9866f43210");
    check(v2.toHex()=="abcde");

    std::cout <<"  non-overlapping copy within one vector\n";
    v1.copy(BitRange::baseSize(0, 11), BitRange::baseSize(25, 11));
    showBin(v1, "copied [25+11] to [0+11]");
    check(v1.toHex()=="9866f43433");

    std::cout <<"  overlapping copy right\n";
    v1.copy(BitRange::hull(3, 38), BitRange::hull(4, 39));
    showBin(v1, "copied [4-39] to [3-38]");
    check(v1.toHex()=="cc337a1a1b");

    std::cout <<"  overlapping copy left\n";
    v1.copy(BitRange::hull(3, 38), BitRange::hull(2, 37));
    showBin(v1, "copied [2-37] to [3-38]");
    check(v1.toHex()=="9866f43433");
}

static void swap_tests() {
    std::cout <<"swap tests\n";

    std::cout <<"  initializing\n";
    BitVector v1(40), v2(40);
    v1.fromHex("9876543210");
    v2.fromHex("6789abcdef");
    showBin(v1, "v1");
    showBin(v2, "v2");

    std::cout <<"  swap between buffers\n";
    v1.swap(BitRange::hull(4, 35), v2, BitRange::hull(4, 35));
    showBin(v1, "v1");
    showBin(v2, "v2");
    check(v1.toHex()=="9789abcde0");
    check(v2.toHex()=="687654321f");

    std::cout <<"  non-overlapping swap within one vector\n";
    v1.swap(BitRange::baseSize(4, 12), BitRange::baseSize(20, 12));
    showBin(v1, "v1 swap [4+12] with [20+12]");
    check(v1.toHex()=="97cdeb89a0");
}

static void bitsearch_tests() {
    std::cout <<"bit searching\n";
    Sawyer::Optional<size_t> idx;

    std::cout <<"  initializing\n";
    BitVector v1(40);
    v1.fromHex("369cf_258be");
    showBin(v1, "initial");

    //---- Least significant set

    std::cout <<"  least set in empty range\n";
    idx = v1.leastSignificantSetBit(BitRange());
    check(!idx);

    std::cout <<"  least set in [33-38]\n";
    idx = v1.leastSignificantSetBit(BitRange::hull(33, 38));
    check(idx);
    check(*idx == 33);

    std::cout <<"  least set in [35-39]\n";
    idx = v1.leastSignificantSetBit(BitRange::hull(35, 39));
    check(idx);
    check(*idx == 36);

    std::cout <<"  least set in [8-10]\n";
    idx = v1.leastSignificantSetBit(BitRange::hull(8, 10));
    check(!idx);

    std::cout <<"  least set in hull\n";
    idx = v1.leastSignificantSetBit();
    check(idx);
    check(*idx == 1);

    //---- Most significant set

    std::cout <<"  most set in [33-38]\n";
    idx = v1.mostSignificantSetBit(BitRange::hull(33, 38));
    check(idx);
    check(*idx == 37);

    std::cout <<"  most set in [0-10]\n";
    idx = v1.mostSignificantSetBit(BitRange::hull(0, 10));
    check(idx);
    check(*idx == 7);

    std::cout <<"  most set in [8-10]\n";
    idx = v1.mostSignificantSetBit(BitRange::hull(8, 10));
    check(!idx);

    std::cout <<"  most set in hull\n";
    idx = v1.mostSignificantSetBit();
    check(idx);
    check(*idx == 37);

    //---- Least significant clear

    std::cout <<"  least clear in [33-38]\n";
    idx = v1.leastSignificantClearBit(BitRange::hull(33, 38));
    check(idx);
    check(*idx == 35);

    std::cout <<"  least clear in [1-39]\n";
    idx = v1.leastSignificantClearBit(BitRange::hull(1, 39));
    check(idx);
    check(*idx == 6);

    std::cout <<"  least clear in [20-23]\n";
    idx = v1.leastSignificantClearBit(BitRange::hull(20, 23));
    check(!idx);

    std::cout <<"  least clear in hull\n";
    idx = v1.leastSignificantClearBit();
    check(idx);
    check(*idx == 0);

    //----  Most significant clear

    std::cout <<"  most clear in [33-38]\n";
    idx = v1.mostSignificantClearBit(BitRange::hull(33, 38));
    check(idx);
    check(*idx == 38);

    std::cout <<"  most clear in [0-28]\n";
    idx = v1.mostSignificantClearBit(BitRange::hull(0, 28));
    check(idx);
    check(*idx == 25);

    std::cout <<"  most clear in [20-23]\n";
    idx = v1.mostSignificantClearBit(BitRange::hull(20, 23));
    check(!idx);
}

static void counting_tests() {
    std::cout <<"counting\n";

    std::cout <<"  initializing\n";
    BitVector v1(40);
    //                 35    30    25    20    15    10    5     0
    v1.fromBinary("00011_11111_10000_11111_00001_11100_01010_11011");
    showBin(v1, "initial");

    std::cout <<"  isAllSet\n";
    check(v1.isAllSet(BitRange()));
    check(v1.isAllSet(BitRange::hull(29, 36)));
    check(!v1.isAllSet(BitRange::hull(29, 37)));
    check(v1.isAllSet(BitRange::hull(20, 24)));
    check(!v1.isAllSet());

    std::cout <<"  isAllClear\n";
    check(v1.isAllClear(BitRange()));
    check(v1.isAllClear(BitRange::hull(25, 28)));
    check(!v1.isAllClear(BitRange::hull(25, 29)));
    check(!v1.isAllClear(BitRange::hull(24, 28)));
    check(v1.isAllClear(10));
    check(!v1.isAllClear(0));
    check(!v1.isAllClear());

    std::cout <<"  nSet\n";
    check(v1.nSet(BitRange()) == 0);
    check(v1.nSet(BitRange::hull(0, 4)) == 4);
    check(v1.nSet(BitRange::hull(30, 35)) == 6);
    check(v1.nSet() == 23);

    std::cout <<"  nClear\n";
    check(v1.nClear(BitRange()) == 0);
    check(v1.nClear(BitRange::hull(0, 4)) == 1);
    check(v1.nClear(BitRange::hull(25, 35)) == 4);
    check(v1.nClear() == 17);
}

static void find_difference_tests() {
    std::cout <<"differencing\n";
    Sawyer::Optional<size_t> idx;

    std::cout <<"  initializing\n";
    BitVector v1(40), v2(40);
    //                 35    30    25    20    15    10    5     0
    v1.fromBinary("00011_11111_10000_11111_00001_11100_01010_11011");
    v2.fromBinary("00001_11101_10000_11111_01001_11100_10101_11011");
    showBin(v1, "v1");
    showBin(v2, "v2");

    //---- Least difference

    std::cout <<"  least difference two vectors\n";
    idx = v1.leastSignificantDifference(BitRange::hull(32, 35), v2, BitRange::hull(32, 35));
    check(!idx);

    idx = v1.leastSignificantDifference(BitRange::hull(32, 39), v2, BitRange::hull(32, 39));
    check(idx);
    check(*idx == 36-32);

    idx = v1.leastSignificantDifference(BitRange::hull(25, 35), v2, BitRange::hull(25, 35));
    check(idx);
    check(*idx == 31-25);

    std::cout <<"  least difference one vector overlap\n";
    idx = v1.leastSignificantDifference(BitRange::hull(12, 24), BitRange::hull(21, 33));
    check(!idx);

    idx = v1.leastSignificantDifference(BitRange::hull(12, 25), BitRange::hull(21, 34));
    check(idx);
    check(*idx == 25-12);

    std::cout <<"  least difference hulls\n";
    idx = v1.leastSignificantDifference(v2);
    check(idx);
    check(*idx == 5);

    //---- Most difference

    std::cout <<"  most difference two vectors\n";
    idx = v1.mostSignificantDifference(BitRange::hull(32, 35), v2, BitRange::hull(32, 35));
    check(!idx);

    idx = v1.mostSignificantDifference(BitRange::hull(20, 35), v2, BitRange::hull(20, 35));
    check(idx);
    check(*idx == 31-20);

    std::cout <<"  most difference one vector overlap\n";
    idx = v1.mostSignificantDifference(BitRange::hull(12, 24), BitRange::hull(21, 33));
    check(!idx);

    idx = v1.mostSignificantDifference(BitRange::hull(11, 24), BitRange::hull(20, 33));
    check(idx);
    check(*idx == 0);

    std::cout <<"  most difference hulls\n";
    idx = v1.mostSignificantDifference(v2);
    check(idx);
    check(*idx == 36);
}

static void shift_tests() {
    std::cout <<"bit shifting\n";

    std::cout <<"  initializing\n";
    BitVector v1(40);
    v1.fromHex("cf258be147");
    showBin(v1, "initialized");

    std::cout <<"  left shift\n";
    v1.shiftLeft(BitRange::baseSize(28, 8), 2, true);
    showBin(v1, "[28+8] by 2 true");
    check(v1.toHex() == "ccb58be147");

    v1.shiftLeft(1);
    showBin(v1, "hull by 1 false");
    check(v1.toHex() == "996b17c28e");

    std::cout <<"  right shift\n";
    v1.shiftRight(BitRange::baseSize(28, 8), 2);
    showBin(v1, "[28+8] by 2 false");
    check(v1.toHex() == "925b17c28e");

    v1.shiftRight(1, true);
    showBin(v1, "hull by 1 true");
    check(v1.toHex() == "c92d8be147");

    std::cout <<"  right arithmetic\n";
    v1.shiftRightArithmetic(BitRange::baseSize(28, 8), 2);
    showBin(v1, "[28+8] by 2");
    check(v1.toHex() == "ce4d8be147");

    v1.shiftRightArithmetic(BitRange::hull(4, 31), 4);
    showBin(v1, "[4-31] by 4");
    check(v1.toHex() == "ce04d8be17");

    v1.shiftRightArithmetic(16);
    showBin(v1, "hull by 16");
    check(v1.toHex() == "ffffce04d8");
}

static void rotate_tests() {
    std::cout <<"rotating\n";

    std::cout <<"  initializing\n";
    std::cout <<"  initializing\n";
    BitVector v1(40);
    v1.fromHex("cf258be147");
    showBin(v1, "initialized");

    std::cout <<"  rotate left\n";
    v1.rotateLeft(BitRange::baseSize(28, 8), 2);
    showBin(v1, "[28+8] by 2");
    check(v1.toHex() == "ccb58be147");

    v1.rotateLeft(4);
    showBin(v1, "hull by 4");
    check(v1.toHex() == "cb58be147c");

    std::cout <<"  rotate right\n";
    v1.rotateRight(BitRange::baseSize(28, 8), 2);
    showBin(v1, "[28+8] by 2");
    check(v1.toHex() == "c6d8be147c");

    v1.rotateRight(8);
    showBin(v1, "hull by 8");
    check(v1.toHex() == "7cc6d8be14");
}

static void addition_tests() {
    std::cout <<"addition\n";
    bool carry;

    std::cout <<"  initializing\n";
    BitVector v1(40), v2(40);
    v1.fromHex("7cc6d8be14");
    v2.fromHex("cf258be147");
    showBin(v1, "v1");
    showBin(v2, "v2");

    std::cout <<"  from two vectors\n";
    carry = v1.add(BitRange::hull(24, 35), v2, BitRange::hull(4, 15));
    showBin(v1, "v1[24-35] += v2[4-15]");
    check(v1.toHex() == "7adad8be14");
    check(carry);

    carry = v1.add(v2);
    showBin(v1, "v1 += v2");
    check(v1.toHex() == "4a00649f5b");
    check(carry);

    std::cout <<"  from one vector no overlap\n";
    carry = v1.add(BitRange::baseSize(28, 12), BitRange::baseSize(4, 12));
    showBin(v1, "v1[28+12] += v1[4+12]");
    check(v1.toHex() == "e950649f5b");
    check(!carry);

    std::cout <<"  from one vector with overlap\n";
    carry = v1.add(BitRange::baseSize(24, 12), BitRange::baseSize(20, 12));
    showBin(v1, "v1[24+12] += v1[20+12]");
    check(v1.toHex() == "ee56649f5b");

    std::cout <<"  increment\n";
    carry = v1.increment(BitRange::baseSize(28, 12));
    showBin(v1, "v1[28+12] += 1");
    check(v1.toHex() == "ee66649f5b");
    check(!carry);

    carry = v1.increment(BitRange::baseSize(8, 5));
    showBin(v1, "v1[8+5] += 1");
    check(v1.toHex() == "ee6664805b");
    check(carry);
}

static void subtraction_tests() {
    std::cout <<"subtraction\n";
    bool carry;

    std::cout <<"  initializing\n";
    BitVector v1(40), v2(40);
    v1.fromHex("7cc6d8be14");
    v2.fromHex("cf258be147");
    showBin(v1, "v1");
    showBin(v2, "v2");

    std::cout <<"  from two vectors\n";
    carry = v1.subtract(BitRange::hull(24, 35), v2, BitRange::hull(4, 15));
    showBin(v1, "v1[24-35] -= v2[4-15]");
    check(v1.toHex() == "7eb2d8be14");
    check(!carry);

    carry = v1.subtract(v2);
    showBin(v1, "v1 -= v2");
    check(v1.toHex() == "af8d4cdccd");
    check(!carry);

    std::cout <<"  from one vector no overlap\n";
    carry = v1.subtract(BitRange::baseSize(28, 12), BitRange::baseSize(4, 12));
    showBin(v1, "v1[28+12] -= v1[4+12]");
    check(v1.toHex() == "d2cd4cdccd");
    check(!carry);

    std::cout <<"  from one vector with overlap\n";
    carry = v1.subtract(BitRange::baseSize(24, 12), BitRange::baseSize(20, 12));
    showBin(v1, "v1[24+12] -= v1[20+12]");
    check(v1.toHex() == "d5f94cdccd");
    check(!carry);

    std::cout <<"  decrement\n";
    carry = v1.decrement(BitRange::baseSize(28, 12));
    showBin(v1, "v1[28+12] -= 1");
    check(v1.toHex() == "d5e94cdccd");
    check(!carry);

    v1.clear(BitRange::baseSize(8, 5));
    showBin(v1, "cleared [8+5]");
    carry = v1.decrement(BitRange::baseSize(8, 5));
    showBin(v1, "v1[8+5] -= 1");
    check(v1.toHex() == "d5e94cdfcd");
    check(carry);
}

static void negate_tests() {
    std::cout <<"negate\n";

    std::cout <<"  initializing\n";
    BitVector v1(40);
    v1.fromHex("7cc6d8be14");
    showBin(v1, "v1");

    std::cout <<"  testing\n";
    v1.negate(BitRange::baseSize(28, 12));
    showBin(v1, "v1[28+12] = -v1[28+12]");
    check(v1.toHex() == "8346d8be14");

    v1.negate();
    showBin(v1, "v1 = -v1");
    check(v1.toHex() == "7cb92741ec");
}

static void sign_extend_tests() {
    std::cout <<"sign extend\n";

    std::cout <<"  initializing\n";
    BitVector v1(40), v2(36);
    v1.fromHex("7cc6d8be14");
    v2.fromHex("741eb825f");
    showBin(v1, "v1");
    showBin(v2, "v2");

    std::cout <<"  from one vector to another\n";
    v1.signExtend(BitRange::baseSize(4, 8), v2, BitRange::baseSize(0, 4));
    showBin(v1, "v1[4+8] = v2[0+4]");
    check(v1.toHex() == "7cc6d8bff4");

    std::cout <<"  overlapping ranges\n";
    v1.signExtend(BitRange::baseSize(0, 40), BitRange::baseSize(0, 28));
    showBin(v1, "v1[0+40] = v1[0+28]");
    check(v1.toHex() == "0006d8bff4");

    std::cout <<"  truncation\n";
    v1.signExtend(BitRange::baseSize(0, 4), BitRange::baseSize(0, 40));
    showBin(v1, "v1[0+4] = v1[0+40]");
    check(v1.toHex() == "0006d8bff4");

    std::cout <<"  whole vector\n";
    v1.signExtend(v2);
    showBin(v1, "v1 = v2");
    check(v1.toHex() == "0741eb825f");
    
}
static void boolean_tests() {
    std::cout <<"Boolean operations\n";

    std::cout <<"  initializing\n";
    BitVector v1(40), v2(40);
    v1.fromHex("7cc6d8be14");
    v2.fromHex("c741eb825f");
    showBin(v1, "v1");
    showBin(v2, "v2");

    std::cout <<"  invert\n";
    v1.invert(BitRange::baseSize(8, 12));
    showBin(v1, "invert v1[8+12]");
    check(v1.toHex() == "7cc6d74114");

    v1.invert();
    showBin(v1, "invert v1");
    check(v1.toHex() == "833928beeb");

    std::cout <<"  bit-wise AND\n";
    v1.bitwiseAnd(BitRange::baseSize(28, 8), v2, BitRange::baseSize(28, 8));
    showBin(v1, "v1[28+8] &= v2[28+8]");
    check(v1.toHex() == "830928beeb");

    v1.bitwiseAnd(BitRange::baseSize(0, 16), BitRange::baseSize(4, 16));
    showBin(v1, "v1[0+16] &= v1[4+16]");
    check(v1.toHex() == "8309288aea");

    v1.bitwiseAnd(v2);
    showBin(v1, "v1 &= v2");
    check(v1.toHex() == "830128824a");

    std::cout <<"  bit-wise OR\n";
    v1.bitwiseOr(BitRange::baseSize(28, 12), v2, BitRange::baseSize(0, 12));
    showBin(v1, "v1[28+12] |= v2[0+12]");
    check(v1.toHex() == "a7f128824a");

    v1.bitwiseOr(BitRange::baseSize(20, 20), BitRange::baseSize(0, 20));
    showBin(v1, "v1[20+20] |= v1[0+20]");
    check(v1.toHex() == "aff5a8824a");

    v1.bitwiseOr(v2);
    showBin(v1, "v1 |= v2");
    check(v1.toHex() == "eff5eb825f");

    std::cout <<"  bit-wise XOR\n";
    v1.bitwiseXor(BitRange::baseSize(28, 8), v2, BitRange::baseSize(28, 8));
    showBin(v1, "v1[28+8] ^= v2[28+8]");
    check(v1.toHex() == "e8b5eb825f");

    v1.bitwiseXor(BitRange::baseSize(4, 16), BitRange::baseSize(0, 16));
    showBin(v1, "v1[4+16] ^= v1[0+16]");
    check(v1.toHex() == "e8b5e3a7af");

    v1.bitwiseXor(v2);
    showBin(v1, "v1 ^= v2");
    check(v1.toHex() == "2ff40825f0");
}

static void numeric_tests() {
    std::cout <<"numeric tests\n";
    std::cout <<"  initializing\n";
    BitVector v1(40), v2(32);
    v1.fromHex("7cc6d8be14");
    v2.fromHex("c741825f");
    showBin(v1, "v1");
    showBin(v2, "v2");

    std::cout <<"  equal to zero\n";
    check(v2.isEqualToZero(BitRange::baseSize(10, 5)));
    check(!v1.isEqualToZero());

    std::cout <<"  compare unsigned\n";
    check(v1.compare(BitRange::hull(8, 39), v2, v2.hull()) < 0);
    check(v1.compare(BitRange::hull(7, 38), v2, v2.hull()) > 0);
    check(v1.compare(BitRange::hull(0, 1), v2, BitRange::hull(10, 14))==0);
    check(v1.compare(BitRange::baseSize(34, 5), BitRange::baseSize(9, 5))==0);
    check(v1.compare(v2) > 0);
    check(v2.compare(v1) < 0);

    std::cout <<"  compare signed\n";
    check(v1.compareSigned(BitRange::hull(0, 4), v2, BitRange::hull(0, 4)) < 0);
    check(v1.compareSigned(BitRange::hull(4, 4), BitRange::baseSize(9, 5)) == 0);
    check(v1.compareSigned(v2) > 0);
    check(v2.compareSigned(v1) < 0);
    check(v1.compareSigned(v1) == 0);
    
    
    
}

int main() {
    Sawyer::initializeLibrary();

    ctor_tests();
    string_tests();
    copy_ctor_tests();
    assignment_tests();
    resize_tests();
    getbit_tests();
    clearbit_tests();
    setbit_tests();
    copybit_tests();
    swap_tests();
    bitsearch_tests();
    counting_tests();
    find_difference_tests();
    shift_tests();
    rotate_tests();
    addition_tests();
    subtraction_tests();
    negate_tests();
    sign_extend_tests();
    boolean_tests();
    numeric_tests();
}
