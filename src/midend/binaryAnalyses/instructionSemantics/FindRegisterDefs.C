// DQ (10/5/2014): This is more strict now that we include rose_config.h in the sage3basic.h.
// #include "rose.h"
#include "sage3basic.h"

#include "FindRegisterDefs.h"
#include <ostream>

namespace rose {
namespace BinaryAnalysis {
namespace FindRegisterDefs {

/******************************************************************************************************************************
 *                                      State
 ******************************************************************************************************************************/

void
State::merge(const State &other)
{
    for (size_t i=0; i<n_gprs; ++i)
        gpr[i].defbits |= other.gpr[i].defbits;
    for (size_t i=0; i<n_segregs; ++i)
        segreg[i].defbits |= other.segreg[i].defbits;
    flags.defbits |= other.flags.defbits;
    ip.defbits |= other.ip.defbits;
}

bool
State::equal_registers(const State &other) const
{
    for (size_t i=0; i<n_gprs; ++i)
        if (gpr[i].defbits != other.gpr[i].defbits)
            return false;

    for (size_t i=0; i<n_segregs; ++i)
        if (segreg[i].defbits != other.segreg[i].defbits)
            return false;

    if (flags.defbits!=other.flags.defbits)
        return false;

    if (ip.defbits != other.ip.defbits)
        return false;

    return true;
}

void
State::print(std::ostream &o) const
{
    std::ios_base::fmtflags oflags = o.flags();
    try {
        std::string prefix = "    ";
        for (size_t i=0; i<n_gprs; ++i)
            o <<prefix <<std::setw(5) <<std::left <<gprToString((X86GeneralPurposeRegister)i) <<" = " <<gpr[i] <<"\n";
        for (size_t i=0; i<n_segregs; ++i)
            o <<prefix <<std::setw(5) <<std::left <<segregToString((X86SegmentRegister)i) <<" = " <<segreg[i] <<"\n";
        for (size_t i=0; i<32; ++i)
            o <<prefix <<std::setw(5) <<std::left <<flagToString((X86Flag)i) <<" = " <<ValueType<1>(flags.defbits>>i) <<"\n";
        o <<prefix <<std::setw(5) <<std::left <<"ip" <<" = " <<ip <<"\n";
    } catch (...) {
        o.flags(oflags);
        throw;
    }
    o.flags(oflags);
}

/******************************************************************************************************************************
 *                                      Policy
 ******************************************************************************************************************************/

void
Policy::merge(const Policy &other)
{
    cur_state.merge(other.cur_state);
}

bool
Policy::equal_states(const Policy &other) const
{
    return cur_state.equal_registers(other.cur_state);
}

void
Policy::print(std::ostream &o) const
{
    o <<cur_state;
}

} // namespace
} // namespace
} // namespace
