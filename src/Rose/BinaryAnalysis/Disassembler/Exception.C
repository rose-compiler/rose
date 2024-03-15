#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Disassembler/Exception.h>

#include <Rose/StringUtility/NumberToString.h>

#include <SgAsmInstruction.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Disassembler {

Exception::Exception(const std::string &reason, SgAsmInstruction *insn)
    : Rose::Exception(reason), ip(insn->get_address()), bit(0), insn(insn) {}

void
Exception::print(std::ostream &o) const {
    if (insn) {
        o <<"disassembly failed at " <<StringUtility::addrToString(ip)
          <<" [" <<insn->toString() <<"]"
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
