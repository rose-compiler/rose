// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#ifndef Sawyer_Parse_H
#define Sawyer_Parse_H

#include <Sawyer/Optional.h>
#include <Sawyer/Result.h>

#include <limits>
#include <string>
#include <type_traits>
#include <boost/lexical_cast.hpp>

namespace Sawyer {

/** Convert a character to a numeric digit.
 *
 *  If the character is valid for the specified radix, then its numeric value is returned, otherwise
 *  nothing is returned.  The radix must be less than or equal to 16. */
template<class IntegralType>
Sawyer::Optional<typename std::enable_if<std::is_integral<IntegralType>::value, IntegralType>::type>
toDigit(char ch, IntegralType radix = 10) {
    assert(radix <= 16);
    assert(!std::numeric_limits<IntegralType>::is_signed || radix >= 0); // trait test is to avoid compiler warning
    IntegralType digit;
    if (ch >= '0' && ch <= '9') {
        digit = ch - '0';
    } else if (ch >= 'a' && ch <= 'f') {
        digit = ch - 'a' + 10;
    } else if (ch >= 'A' && ch <= 'F') {
        digit = ch - 'A' + 10;
    } else {
        return Sawyer::Nothing();
    }

    if (digit < radix)
        return digit;
    return Sawyer::Nothing();
}

/** Safely convert a string to a number.
 *
 *  The input string must parse as a integer without leading white space and without trailing characters. For signed integer
 *  types, the integer may be immediately preceded by a minus ("-") or plus ("+") sign. The integer part may be preceded by a
 *  radix indicator: "0x" for hexadecimal or "0b" for binary. Octal and other bases are not supported. Digits may be separated
 *  from one another (or from the radix specifier) by a single underscore, as in "0b_0001_0010_0011_0100_0101_0110_0111_1000".
 *
 *  The @p IntegralType must be a signed or unsigned integer type of any width. The @c uint8_t and @c int8_t types are also
 *  accepted although they're aliases for <code>unsigned char</code> and <code>signed char</code> .
 *
 *  If a syntax error occurs, or if the magnitude of the value is too large to be represented by the specified @p IntegralType,
 *  then an error message is returned. The text of the message begins with either "syntax error:" or "overflow error:". */
template<class IntegralType>
typename std::enable_if<std::is_integral<IntegralType>::value, Sawyer::Result<IntegralType, std::string>>::type
parse(const std::string &s) {
    using UnsignedType = typename std::make_unsigned<IntegralType>::type;

    // No template parameter deduction in constructors before C++17, so make aliases
    using Error = Sawyer::Error<std::string>;
    using Ok = Sawyer::Ok<IntegralType>;

    const char *sptr = s.c_str();

    // Optional plus or minus sign when parsing signed values
    if ('-' == *sptr || '+' == *sptr) {
        if (std::numeric_limits<IntegralType>::is_signed) {
            ++sptr;
        } else {
            return Error("syntax error: sign not allowed for unsigned types");
        }
    }
    if ('_' == *sptr)
        return Error("syntax error: separator not allowed before first digit");

    // Radix specification
    UnsignedType radix = 10;
    if (strncmp("0x", sptr, 2) == 0 || strncmp("0X", sptr, 2) == 0) {
        radix = 16;
        sptr += 2;
    } else if (strncmp("0b", sptr, 2) == 0) {
        radix = 2;
        sptr += 2;
    }

    // Number may have a leading digit separator if it has a radix specifier
    if (radix != 10 && '_' == *sptr)
        ++sptr;

    // Parse the value as unsigned, but be careful of overflows.
    UnsignedType n = 0;
    size_t nDigits = 0;
    for (size_t i = 0; sptr[i]; ++i) {
        if ('_' == sptr[i]) {
            if (0 == i || '_' == sptr[i-1] || !sptr[i+1])
                return Error("syntax error: invalid use of digit separator");

        } else if (Sawyer::Optional<UnsignedType> digit = toDigit(sptr[i], radix)) {
            ++nDigits;

            // Check for overflow
            const UnsignedType shifted = n * radix;
            if ((n != 0 && shifted / n != radix) || *digit > std::numeric_limits<UnsignedType>::max() - shifted) {
                if ('-' == s[0]) {
                    return Error("overflow error: less than minimum value for type");
                } else {
                    return Error("overflow error: greater than maximum value for type");
                }
            }

            n = shifted + *digit;
        } else {
            return Error("syntax error: invalid digit after parsing " + boost::lexical_cast<std::string>(nDigits) +
                         (1==nDigits ? " digit" : " digits"));
        }
    }
    if (0 == nDigits)
        return Error("syntax error: digits expected");

    // Convert the unsigned (positive) value to a signed negative if necessary, checking overflow.
    if (std::numeric_limits<IntegralType>::is_signed) {
        const UnsignedType signBit = (UnsignedType)1 << (8*sizeof(IntegralType) - 1);
        if ('-' == s[0]) {
            if (n & signBit && n != signBit)
                return Error("overflow error: less than minimum value for type");
            return Ok((IntegralType)(~n + 1));
        } else if (n & signBit) {
            return Error("overflow error: greater than maximum value for type");
        } else {
            return Ok((IntegralType)n);
        }
    } else {
        return Ok((IntegralType)n);
    }
}

} // namespace
#endif
