#include <Rose/StringUtility/NumberToString.h>

#include <Rose/StringUtility/Diagnostics.h>

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

namespace Rose {
namespace StringUtility {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functions for converting numbers to strings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string
numberToString(long long x) {
    return boost::lexical_cast<std::string>(x);
}

std::string
numberToString(unsigned long long x) {
    return boost::lexical_cast<std::string>(x);
}

std::string
numberToString(long x) {
    return boost::lexical_cast<std::string>(x);
}

std::string
numberToString(unsigned long x) {
    return boost::lexical_cast<std::string>(x);
}

std::string
numberToString(int x) {
    return boost::lexical_cast<std::string>(x);
}

std::string
numberToString(unsigned int x) {
    return boost::lexical_cast<std::string>(x);
}

std::string
numberToString(const void* x) {
    char numberString[128];
    sprintf(numberString, "%p", x);
    return std::string(numberString);
}

std::string
numberToString(double x) {
    char numberString[128];
    sprintf(numberString, "%2.2f", x);
    return std::string(numberString);
}

#if !defined(_MSC_VER) &&                                                                                                      \
    defined(BACKEND_CXX_IS_GNU_COMPILER) &&                                                                                    \
    ((BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER == 4 && BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER > 6) ||                      \
     BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER > 4) &&                                                                         \
    __WORDSIZE == 64
std::string
numberToString( __int128 x) {
    // DQ (2/22/2014): I don't think that the boost::lexical_cast can support __int128 yet.
    long long temp_x = (long long) x;
    return boost::lexical_cast<std::string>(temp_x);
}

std::string
numberToString(unsigned __int128 x) {
    // DQ (2/22/2014): I don't think that the boost::lexical_cast can support __int128 yet.
    unsigned long long temp_x = (unsigned long long) x;
    return boost::lexical_cast<std::string>(temp_x);
}
#endif

std::string
intToHex(uint64_t i) {
    std::ostringstream os;
    os << "0x" << std::hex << i;
    return os.str();
}

std::string
toHex2(uint64_t value, size_t nbits, bool show_unsigned_decimal, bool show_signed_decimal, uint64_t decimal_threshold) {
    assert(nbits>0 && nbits<=8*sizeof value);
    uint64_t sign_bit = ((uint64_t)1 << (nbits-1));

    // Hexadecimal value
    std::string retval;
    int nnibbles = (nbits+3)/4;
    std::string fmt = "0x%0" + boost::lexical_cast<std::string>(nnibbles) + "x";
    retval = (boost::format(fmt) % value).str();

    // unsigned decimal
    bool showed_unsigned = false;
    if (show_unsigned_decimal && value >= decimal_threshold) {
        retval = appendAsmComment(retval, numberToString(value));
        showed_unsigned = true;
    }

    // signed decimal
    if (show_signed_decimal) {
        if (0 == (value & sign_bit)) {
            // This is a positive value. Don't show it if we did already.
            if (!showed_unsigned && value >= decimal_threshold)
                retval = appendAsmComment(retval, numberToString(value));
        } else {
            // This is a negative value, so show it as negative.  We have to manually sign extend it first.
            int64_t signed_value = (value | (-1ll & ~(sign_bit-1)));
            retval = appendAsmComment(retval, numberToString(signed_value));
        }
    }
    return retval;
}

std::string
signedToHex2(uint64_t value, size_t nbits) {
    return toHex2(value, nbits, false, true);
}

std::string
unsignedToHex2(uint64_t value, size_t nbits) {
    return toHex2(value, nbits, true, false);
}

std::string
addrToString(uint64_t value, size_t nbits) {
    if (0 == nbits)
        nbits = value > 0xffffffff ? 64 : 32;
    return toHex2(value, nbits, false, false);
}

std::string
addrToString(const Sawyer::Optional<uint64_t> &value, size_t nbits) {
    return value ? addrToString(*value, nbits) : "none";
}

std::string
addrToString(const Sawyer::Container::Interval<uint64_t> &interval, size_t nbits) {
    if (interval.isEmpty()) {
        return "[empty]";
    } else {
        return "[" + addrToString(interval.least(), nbits) + ", " + addrToString(interval.greatest(), nbits) + "]";
    }
}

std::string
addrToString(const Sawyer::Container::IntervalSet<Sawyer::Container::Interval<uint64_t> > &iset, size_t nbits) {
    if (iset.isEmpty()) {
        return "{empty}";
    } else {
        std::string retval = "{";
        size_t i = 0;
        for (const Sawyer::Container::Interval<uint64_t> &interval: iset.intervals()) {
            if (++i > 1)
                retval += ", ";
            retval += addrToString(interval, nbits);
        }
        retval += "}";
        return retval;
    }
}

} // namespace
} // namespace
