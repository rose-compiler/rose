#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

#include <Rose/BinaryAnalysis/Disassembler/Exception.h>
#include <Rose/StringUtility/NumberToString.h>

#include <AsmUnparser_compat.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Disassembler {

void
Exception::print(std::ostream &o) const {
    if (insn) {
        o <<"disassembly failed at " <<StringUtility::addrToString(ip)
          <<" [" <<unparseInstruction(insn) <<"]"
          <<": " <<what();
    } else if (ip>0) {
        o <<"disassembly failed at " <<StringUtility::addrToString(ip);
        if (!bytes.empty()) {
            for (size_t i=0; i<bytes.size(); i++) {
                o <<(i>0?", ":"[")
                  <<std::hex <<std::setfill('0') <<std::setw(2)
                  <<"0x" <<bytes[i]
                  <<std::dec <<std::setfill(' ') <<std::setw(1);
            }
            o <<"] at bit " <<bit;
        }
    } else {
        o <<what();
    }
}

std::ostream &
operator<<(std::ostream &o, const Exception &e)
{
    e.print(o);
    return o;
}

} // namespace
} // namespace
} // namespace
#endif
