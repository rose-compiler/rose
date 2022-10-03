#include <rose.h>

#include <Rose/StringUtility/StringToNumber.h>

#include <boost/lexical_cast.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <regex>

#define assert_eq(X, Y) assert_equal((X), (Y), __FILE__, __LINE__)
#define assert_fail(X, Y) assert_exception<T>((X), (Y), __FILE__, __LINE__)

template<class T>
static std::string
typeName() {
    return (std::numeric_limits<T>::is_signed ? "int" : "uint") +
        boost::lexical_cast<std::string>(8*sizeof(T)) + "_t";
}

template<class T>
static std::string
toString(const T& n_, size_t radix = 10, size_t nBits = 8*sizeof(T)) {
    using Unsigned = typename std::make_unsigned<T>::type;
    Unsigned n = (Unsigned)n_;
    std::ostringstream ss;

    if (std::numeric_limits<T>::is_signed) {
        const Unsigned mask = Unsigned{1} << (8*sizeof(T)-1);
        if (n & mask) {
            ss <<'-';
            n = ~n + 1;
        }
    }

    switch (radix) {
        case 10:
            ss <<n;
            break;
        case 16:
            ss <<"0x";
            assert(0 == nBits % 4);
            for (size_t i = nBits; i > 0; i -= 4) {
                const uint16_t nybble = (n >> (i - 4)) & 0x0f;
                ss <<std::hex <<nybble;
            }
            break;
        case 2:
            ss <<"0b";
            for (size_t i = nBits; i > 0; --i) {
                const T mask = Unsigned{1} << (i-1);
                ss <<(n & mask ? '1' : '0');
            }
            break;
        default:
            ASSERT_not_reachable("invalid radix " + boost::lexical_cast<std::string>(radix));
    }
    return ss.str();
}

template<>
std::string
toString<signed char>(const signed char& n, size_t radix, size_t nBits) {
    return toString<int16_t>(n, radix, nBits);
}

template<>
std::string
toString<unsigned char>(const unsigned char& n, size_t radix, size_t nBits) {
    return toString<uint16_t>(n, radix, nBits);
}

template<class T>
static void
assert_equal(const std::string &s, T y, const char *file, const int line) {
    auto parsed = Rose::StringUtility::toNumber<T>(s);
    if (!parsed) {
        std::cerr <<"test failed at " <<file <<":" <<line <<"\n"
                  <<"  for type " <<typeName<T>() <<"\n"
                  <<"  input string \"" <<Rose::StringUtility::cEscape(s) <<"\"\n"
                  <<"  " <<*parsed.error() <<"\n"
                  <<"  expected " <<toString(y) <<"\n";
        exit(1);
    } else if (*parsed != y) {
        std::cerr <<"test failed at " <<file <<":" <<line <<"\n"
                  <<"  for type " <<typeName<T>() <<"\n"
                  <<"  input string \"" <<Rose::StringUtility::cEscape(s) <<"\"\n"
                  <<"  parsed as " <<toString(*parsed) <<"\n"
                  <<"  expected " <<toString(y) <<"\n";
        exit(1);
    }
}

template<class T>
static void
assert_exception(const std::string &s, const std::string &reStr, const char *file, const int line) {
    auto parsed = Rose::StringUtility::toNumber<T>(s);
    if (parsed) {
        std::cerr <<"test failed at " <<file <<":" <<line <<"\n"
                  <<"  for type " <<typeName<T>() <<"\n"
                  <<"  input string \"" <<Rose::StringUtility::cEscape(s) <<"\"\n"
                  <<"  parsed as " <<toString(*parsed) <<"\n"
                  <<"  expected exception matching /" <<Rose::StringUtility::cEscape(reStr) <<"/\n";
        exit(1);
    } else {
        std::regex re(reStr);
        if (!std::regex_search(*parsed.error(), re)) {
            std::cerr <<"test failed at " <<file <<":" <<line <<"\n"
                      <<"  for type " <<typeName<T>() <<"\n"
                      <<"  input string \"" <<Rose::StringUtility::cEscape(s) <<"\"\n"
                      <<"  expected exception matching /" <<Rose::StringUtility::cEscape(reStr) <<"/\n"
                      <<"  got exception \"" <<Rose::StringUtility::cEscape(*parsed.error()) <<"\"\n";
            exit(1);
        }
    }
}

template<class T>
static void
test() {
    using namespace Rose::StringUtility;

    // Common values
    assert_eq("0", T{0});
    assert_eq("1", T{1});
    assert_eq("018", T{18});

    // Minimum and maximum possible values
    const std::string max_value_10 = toString(std::numeric_limits<T>::max(), 10);
    const std::string min_value_10 = toString(std::numeric_limits<T>::min(), 10);
    assert_eq(max_value_10, std::numeric_limits<T>::max());
    assert_eq(min_value_10, std::numeric_limits<T>::min());

    // Leading zeros
    assert_eq("00000000000000000000000000000000000000000000000000", T{0});
    assert_eq("00000000000000000000000000000000000000000000000001", T{1});
    const std::string zeromax = "0000000000" + max_value_10;
    assert_eq(zeromax, std::numeric_limits<T>::max());

    // Separators
    assert_eq("0000_0000_0000_0000_0001", T{1});
    assert_eq("1_2_3", T{123});
    assert_fail("_123", "syntax error: separator not allowed before first digit");
    assert_fail("123_", "syntax error: invalid use of digit separator");
    assert_fail("1__23", "syntax error: invalid use of digit separator");


    // Binary
    assert_eq("0b0000", T{0});
    assert_eq("0b_0000_0000_0000_1011", T{11});
    const std::string max_value_2 = toString(std::numeric_limits<T>::max(), 2);
    const std::string min_value_2 = toString(std::numeric_limits<T>::min(), 2);
    assert_eq(max_value_2, std::numeric_limits<T>::max());
    assert_eq(min_value_2, std::numeric_limits<T>::min());
    assert_fail("0b121", "syntax error: invalid digit");

    // Hexadecimal
    assert_eq("0x7f", T{0x7f});
    assert_eq("0x7F", T{0x7f});
    const std::string max_value_16 = toString(std::numeric_limits<T>::max(), 16);
    const std::string min_value_16 = toString(std::numeric_limits<T>::min(), 16);
    assert_eq(max_value_16, std::numeric_limits<T>::max());
    assert_eq(min_value_16, std::numeric_limits<T>::min());
    assert_fail("7f", "syntax error: invalid digit");

    // Negatives and positives
    if constexpr (std::numeric_limits<T>::is_signed) {
        assert_eq("-000123", T{-123});
        assert_eq("-1_23", T{-123});
        assert_fail("-_123", "syntax error: separator not allowed before first digit");
        assert_eq("-0b_1000_0000", T{-128});

        assert_eq("+0000123", T{123});
        assert_eq("+1_23", T{123});
        assert_fail("+_123", "syntax error: separator not allowed before first digit");
        assert_eq("+0b_0111_1111", T{127});
    }

    // Check overflows
    assert_fail(max_value_10 + "0", "overflow");
    assert_fail(max_value_10 + "1", "overflow");
    if constexpr (sizeof(T) < 8) {
        if constexpr (std::numeric_limits<T>::is_signed) {
            const std::string big_min_10 = toString((int64_t)std::numeric_limits<T>::min() - 1);
            assert_fail(big_min_10, "overflow error: less than minimum value for type");
            const std::string big_max_10 = toString((int64_t)std::numeric_limits<T>::max() + 1);
            assert_fail(big_max_10, "overflow error: greater than maximum value for type");
        } else {
            const std::string big_max_10 = toString((uint64_t)std::numeric_limits<T>::max() + 1);
            assert_fail(big_max_10, "overflow error: greater than maximum value for type");
        }
    }

    // Test certain syntax errors
    assert_fail("", "syntax error: digits expected");
    assert_fail("x", "syntax error: invalid digit after parsing 0 digits");
    assert_fail("0z", "syntax error: invalid digit after parsing 1 digit");
    assert_fail("0x", "syntax error: digits expected");
    assert_fail("0b", "syntax error: digits expected");
    if constexpr (std::numeric_limits<T>::is_signed) {
        assert_fail("-", "syntax error: digits expected");
        assert_fail("+", "syntax error: digits expected");
        assert_fail("- 0", "syntax error: invalid digit after parsing 0 digits");
        assert_fail("+ 0", "syntax error: invalid digit after parsing 0 digits");
    } else {
        assert_fail("-", "syntax error: sign not allowed for unsigned types");
        assert_fail("+", "syntax error: sign not allowed for unsigned types");
        assert_fail("- 0", "syntax error: sign not allowed for unsigned types");
        assert_fail("+ 0", "syntax error: sign not allowed for unsigned types");
    }
}

int main() {
    test<uint8_t>();
    test<uint16_t>();
    test<uint32_t>();
    test<uint64_t>();

    test<int8_t>();
    test<int16_t>();
    test<int32_t>();
    test<int64_t>();
}
