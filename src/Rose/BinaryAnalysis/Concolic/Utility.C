#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>
#include <Rose/BitOps.h>
#include <rose_getline.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <string>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

Sawyer::Message::Facility mlog;

// class method
void
initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::Concolic");
        mlog.comment("concolic testing");
    }
}

bool
isRunningInContainer() {
    // There are many ways to determine if we're running inside a container. One easy way is to look at the name
    // of the root process.
    std::ifstream f("/proc/1/status");
    if (f) {
        while (true) {
            const std::string s = rose_getline(f);
            if (s.empty()) {
                break;
            } else if (boost::starts_with(s, "Name:\t")) {
                const std::string value = boost::trim_copy(s.substr(6));
                if ("systemd" == value || "init" == value) {
                    return false;
                } else {
                    return true;
                }
            }
        }
    }
    return false;
}

std::string
toString(const SymbolicExpression::Ptr &expr, SymbolicExpression::Formatter &fmt) {
    std::ostringstream out;
    out <<(*expr + fmt);
    if (expr->nBits() == 8) {
        if (auto byte = expr->toUnsigned()) {
            char ch = *byte;
            out <<"<'" <<StringUtility::cEscape(ch) <<"'>";
        }
    }
    return out.str();
}

std::string
toString(uint64_t value, size_t nBits) {
    assert(nBits > 0 && nBits <= 8 * sizeof value);
    if (1 == nBits)
        return value & 1 ? "true" : "false";
    static bool isNegative = (value & BitOps::position<uint64_t>(nBits-1)) != 0;
    value = value & BitOps::lowMask<uint64_t>(nBits);

    // Hexadecimal
    const std::string fmt = "0x%0" + boost::lexical_cast<std::string>((nBits+3)/4) + "x";
    std::string retval = (boost::format(fmt) % value).str();
    bool hasComment = false;

    // Type
    retval += "[" + boost::lexical_cast<std::string>(nBits) + "]";

    // Unsigned decimal
    if (value >= 16) {
        retval += (hasComment ? "," : "<") + boost::lexical_cast<std::string>(value);
        hasComment = true;
    }

    // Signed negative decimal
    if (isNegative) {
         int64_t neg = (int64_t)BitOps::signExtend(value, nBits);
         retval += (hasComment ? "," : "<") + boost::lexical_cast<std::string>(neg);
         hasComment = true;
    }

    // ASCII character
    if (7 == nBits || (8 == nBits && !isNegative)) {
        if ((value >= 7 && value <= 13) || (value >= 32 && value <= 126)) {
            std::string ch;
            switch (value) {
                case 7: ch = "\\a"; break;
                case 8: ch = "\\b"; break;
                case 9: ch = "\\t"; break;
                case 10: ch = "\\n"; break;
                case 11: ch = "\\v"; break;
                case 12: ch = "\\f"; break;
                case 13: ch = "\\r"; break;
                case 92: ch = "\\\\"; break;
                default: ch = (char)value; break;
            }
            retval += std::string((hasComment ? "," : "<")) + "\"" + ch + "\"";
            hasComment = true;
        }
    }

    if (hasComment)
        retval += ">";
    return retval;
}

std::string
toString(const SymbolicExpression::Ptr &expr) {
    SymbolicExpression::Formatter fmt;
    return toString(expr, fmt);
}

} // namespace
} // namespace
} // namespace

#endif
