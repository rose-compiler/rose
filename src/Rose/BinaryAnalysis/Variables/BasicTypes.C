#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Variables/BasicTypes.h>

#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BitOps.h>
#include <Rose/Diagnostics.h>
#include <Rose/StringUtility/NumberToString.h>

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <mutex>

namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace Rose {
namespace BinaryAnalysis {
namespace Variables {

Sawyer::Message::Facility mlog;
Sawyer::Attribute::Id ATTR_FRAME_SIZE = Sawyer::Attribute::INVALID_ID;
Sawyer::Attribute::Id ATTR_LOCAL_VARS = Sawyer::Attribute::INVALID_ID;
Sawyer::Attribute::Id ATTR_GLOBAL_VARS = Sawyer::Attribute::INVALID_ID;

void
initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::Variables");
        mlog.comment("local and global variable detection");
    }
}

// Initialize our own global variables
void
initNamespace() {
    static std::once_flag onceFlag;
    std::call_once(onceFlag, []() {
        ATTR_FRAME_SIZE = Sawyer::Attribute::declare("function frame size");
        ATTR_LOCAL_VARS = Sawyer::Attribute::declare("local variables");
        ATTR_GLOBAL_VARS = Sawyer::Attribute::declare("global variables");
    });
}

std::string
offsetStr(int64_t n) {
    std::string sign = "+";
    if ((uint64_t)n == BitOps::position<uint64_t>(63)) {
        return "-0x8000000000000000<-9223372036854775808>";
    } else if (n < 0) {
        sign = "-";
        n = -n;
    }

    if (n <= 9) {
        return sign + boost::lexical_cast<std::string>(n);
    } else {
        std::string h = StringUtility::addrToString(n).substr(2);
        boost::trim_left_if(h, boost::is_any_of("0_"));
        ASSERT_forbid(h.empty());
        return sign + "0x" + h + "<" + sign + boost::lexical_cast<std::string>(n) + ">";
    }
}

std::string
sizeStr(uint64_t n) {
    if (n <= 9) {
        return boost::lexical_cast<std::string>(n);
    } else {
        std::string h = StringUtility::addrToString(n).substr(2);
        boost::trim_left_if(h, boost::is_any_of("0_"));
        ASSERT_forbid(h.empty());
        return "0x" + h + "<" + boost::lexical_cast<std::string>(n) + ">";
    }
}

void
print(const StackVariables &lvars, const P2::Partitioner::ConstPtr &partitioner,
      std::ostream &out, const std::string &prefix) {
    ASSERT_not_null(partitioner);
    for (const StackVariable &lvar: lvars.values()) {
        out <<prefix <<lvar <<"\n";
        for (const InstructionAccess &ia: lvar.instructionsAccessing())
            out <<prefix <<"  " <<ia.toString() <<"\n";
    }
}

void
print(const GlobalVariables &gvars, const P2::Partitioner::ConstPtr &partitioner,
      std::ostream &out, const std::string &prefix) {
    ASSERT_not_null(partitioner);
    for (const GlobalVariable &gvar: gvars.values()) {
        out <<prefix <<gvar <<"\n";
        for (const InstructionAccess &ia: gvar.instructionsAccessing())
            out <<prefix <<" " <<ia.toString() <<"\n";
    }
}

} // namespace
} // namespace
} // namespace

#endif
