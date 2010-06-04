#include "rose.h"
#include "SymbolicSemantics.h"

namespace SymbolicSemantics {

std::ostream &
operator<<(std::ostream &o, const State &state)
{
    state.print(o);
    return o;
}
    
std::ostream &
operator<<(std::ostream &o, const MemoryCell &mc)
{
    mc.print(o, NULL);
    return o;
}

bool
MemoryCell::may_alias(const MemoryCell &other) const
{
    return must_alias(other); /*FIXME: need to tighten this up some. [RPM 2010-05-24]*/
}

bool
MemoryCell::must_alias(const MemoryCell &other) const
{
    return address.equal_to(other.address);
}

void
MemoryCell::print(std::ostream &o, RenameMap *rmap/*NULL*/) const
{
    address.print(o, rmap);
    o <<": ";
    data.print(o, rmap);
    o <<" " <<nbytes <<" byte" <<(1==nbytes?"":"s");
    if (!written) o <<" read-only";
    if (clobbered) o <<" clobbered";
}

void
State::print(std::ostream &o, RenameMap *rmap/*NULL*/) const
{
    std::string prefix = "    ";

    for (size_t i=0; i<n_gprs; ++i) {
        o <<prefix <<gprToString((X86GeneralPurposeRegister)i) <<"=";
        gpr[i].print(o, rmap);
        o <<"\n";
    }
    for (size_t i=0; i<n_segregs; ++i) {
        o <<prefix <<segregToString((X86SegmentRegister)i) <<"=";
        segreg[i].print(o, rmap);
        o <<"\n";
    }
    for (size_t i=0; i<n_flags; ++i) {
        o <<prefix <<flagToString((X86Flag)i) <<"=";
        flag[i].print(o, rmap);
        o <<"\n";
    }
    o <<prefix <<"ip=";
    ip.print(o, rmap);
    o <<"\n";

    /* Print memory contents. */
    o <<prefix << "memory:\n";
    for (Memory::const_iterator mi=mem.begin(); mi!=mem.end(); ++mi) {
        o <<prefix <<"    ";
        (*mi).print(o, rmap);
        o <<"\n";
    }
}
    
}
