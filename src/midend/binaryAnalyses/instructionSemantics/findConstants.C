// DQ (10/5/2014): This is more strict now that we include rose_config.h in the sage3basic.h.
// #include "rose.h"
#include "sage3basic.h"

#include "findConstants.h"

namespace rose {
namespace BinaryAnalysis {
namespace FindConstants {

uint64_t xvarNameCounter = 0;
SgAsmX86Instruction* currentInstruction = NULL;

/** Returns true if the contents of memory location @p a could possibly overlap with @p b. In other words, returns false only
 *  if memory location @p a cannot overlap with memory location @p b. */
bool mayAlias(const MemoryWrite& a, const MemoryWrite& b) {
    LatticeElement<32> addr1 = a.address;
    LatticeElement<32> addr2 = b.address;

    if (addr1.isTop || addr2.isTop)
        return false;

    /* Two different unknown values or offsets from two different unknown values. */
    if (addr1.name != addr2.name)
        return true;

    /* Same unknown base values but inverses (any offset). */
    if (addr1.name != 0 && addr1.negate != addr2.negate)
        return true;

    /* If they have the same base values (or are both constant) then check the offsets. The 32-bit casts are purportedly
     * necessary to wrap propertly, but I'm not sure this will work for addresses (LatticeElements) that have a length other
     * than 32 bits. [FIXME RPM 2009-02-03]. */
    uint32_t offsetDiff = (uint32_t)(addr1.offset - addr2.offset);
    if (offsetDiff < a.len || offsetDiff > (uint32_t)(-b.len))
        return true;

    return false;
}

/** Returns true if memory locations @p a and @p b are the same (note that "same" is more strict than "overlap"). */
bool mustAlias(const MemoryWrite& a, const MemoryWrite& b) {
    if (!mayAlias(a, b)) return false;
    return a.address.name==b.address.name && a.address.offset==b.address.offset;
}

std::ostream&
operator<<(std::ostream& o, const RegisterSet& rs)
{
    std::string prefix = "    ";
    for (size_t i = 0; i < 8; ++i)
        o <<prefix << gprToString((X86GeneralPurposeRegister)i) << " = " << rs.registers.gpr[i] << std::endl;
    for (size_t i = 0; i < 6; ++i)
        o <<prefix << segregToString((X86SegmentRegister)i) << " = " << rs.registers.segreg[i] << std::endl;
    for (size_t i = 0; i < 16; ++i)
        o <<prefix << flagToString((X86Flag)i) << " = " << rs.registers.flag[i] << std::endl;
    o <<prefix << "memory = ";
    if (rs.memoryWrites->get().isTop) {
        o <<prefix << "<top>\n";
    } else if (rs.memoryWrites->get().writes.empty()) {
        o <<"{}\n";
    } else {
        o <<"{\n";
        for (size_t i = 0; i < rs.memoryWrites->get().writes.size(); ++i) {
            o <<prefix <<"    "
              <<"size=" <<rs.memoryWrites->get().writes[i].len
              << "; addr=" <<rs.memoryWrites->get().writes[i].address
              << "; value=" <<rs.memoryWrites->get().writes[i].data
              <<"\n";
        }
        o <<prefix << "}\n";
    }
    return o;
}

} // namespace
} // namespace
} // namespace
