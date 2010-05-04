#include "rose.h"
#include "VirtualMachineSemantics.h"
#include <ostream>
#include <strstream>

namespace VirtualMachineSemantics {

uint64_t name_counter;

/*************************************************************************************************************************
 *                                                      Global Functions
 *************************************************************************************************************************/

template <size_t Len> std::ostream&
operator<<(std::ostream &o, const ValueType<Len> &e) {
    e.print(o);
    return o;
}

std::ostream&
operator<<(std::ostream &o, const State &e)
{
    e.print(o);
    return o;
}

bool
operator==(const State &a, const State &b)
{
    for (size_t i=0; i<a.n_gprs; ++i)
        if (a.gpr[i]!=b.gpr[i]) return false;
    for (size_t i=0; i<a.n_segregs; ++i)
        if (a.segreg[i]!=b.segreg[i]) return false;
    for (size_t i=0; i<a.n_flags; ++i)
        if (a.flag[i]!=b.flag[i]) return false;
    if (a.mem.size()!=b.mem.size()) return false;
    for (size_t i=0; i<a.mem.size(); i++) {
        if (a.mem[i].nbytes!=b.mem[i].nbytes ||
            a.mem[i].address!=b.mem[i].address ||
            a.mem[i].data!=b.mem[i].data)
            return false;
    }
    return true;
}

bool
operator!=(const State &a, const State &b)
{
    return !(a==b);
}


    
/*************************************************************************************************************************
 *                                                         ValueType
 *************************************************************************************************************************/

template <size_t Len> void
ValueType<Len>::print(std::ostream &o) const {
    uint64_t sign_bit = (uint64_t)1 << (Len-1);  /* e.g., 80000000 */
    uint64_t val_mask = sign_bit - 1;            /* e.g., 7fffffff */
    uint64_t negative = Len>1 && (offset & sign_bit) ? (~offset & val_mask) + 1 : 0; /*magnitude of negative value*/

    if (name!=0) {
        /* This is a named value rather than a constant. */
        const char *sign = negate ? "-" : "";
        o <<sign <<"v" <<std::dec <<name;
        if (negative) {
            o <<"-0x" <<std::hex <<negative;
        } else if (offset) {
            o <<"+0x" <<std::hex <<offset;
        }
    } else {
        /* This is a constant */
        ROSE_ASSERT(!negate);
        o  <<"0x" <<std::hex <<offset;
        if (negative)
            o <<" (-0x" <<std::hex <<negative <<")";
    }
}

template<size_t Len> void
ValueType<Len>::rename(RenameMap &rmap)
{
    if (name!=0) {
        RenameMap::iterator ri=rmap.find(name);
        if (ri==rmap.end()) {
            uint64_t new_name = rmap.size()+1;
            rmap.insert(std::make_pair(name, new_name));
            name = new_name;
        } else {
            name = ri->second;
        }
    }
}



/*************************************************************************************************************************
 *                                                          MemoryCell
 *************************************************************************************************************************/

bool
MemoryCell::may_alias(const MemoryCell &other) const {
    const ValueType<32> &addr1 = this->address;
    const ValueType<32> &addr2 = other.address;
    if (addr1.name != addr2.name) return true;

    /* Same unknown values but inverses (any offset). */
    if (addr1.name && addr1.negate!=addr2.negate) return true;

    /* If they have the same base values (or are both constant) then check the offsets. The 32-bit casts are purportedly
     * necessary to wrap propertly, but I'm not sure this will work for addresses (LatticeElements) that have a length other
     * than 32 bits. [FIXME RPM 2009-02-03]. */
    uint32_t offsetDiff = (uint32_t)(addr1.offset - addr2.offset);
    if (offsetDiff < this->nbytes || offsetDiff > (uint32_t)(-other.nbytes))
        return true;
    return false;
}

bool
MemoryCell::must_alias(const MemoryCell &other) const {
    if (!may_alias(other)) return false;
    return this->address == other.address;
}

void
MemoryCell::rename(RenameMap &rmap)
{
    address.rename(rmap);
    data.rename(rmap);
}



/*************************************************************************************************************************
 *                                                         State
 *************************************************************************************************************************/

void
State::print(std::ostream &o) const 
{
    std::string prefix = "    ";

    /* Print registers in columns of minimal width */
    size_t ssi=0;
    std::stringstream *ss = new std::stringstream[n_gprs + n_segregs + n_flags];
    for (size_t i=0; i<n_gprs; ++i)
        ss[ssi++] <<gprToString((X86GeneralPurposeRegister)i) <<"=" <<gpr[i];
    for (size_t i=0; i<n_segregs; ++i)
        ss[ssi++] <<segregToString((X86SegmentRegister)i) <<"=" <<segreg[i];
    for (size_t i=0; i<n_flags; ++i)
        ss[ssi++] <<flagToString((X86Flag)i) <<"=" <<flag[i];
    size_t colwidth = 0;
    for (size_t i=0; i<ssi; i++)
        colwidth = std::max(colwidth, ss[i].str().size());
    for (size_t i=0; i<ssi; i++) {
        if (0==i%4) o <<(i?"\n":"") <<prefix;
        std::string s = ss[i].str();
        if (s.size()<colwidth+1) s.resize(colwidth+1, ' ');
        o <<s;
    }
    o <<"\n";

    /* Print memory contents */
    o <<prefix << "memory = ";
    if (mem.empty()) {
        o <<"{}\n";
    } else {
        o <<"{\n";
        for (Memory::const_iterator mi=mem.begin(); mi!=mem.end(); ++mi)
            o <<prefix <<"    " <<(*mi) <<"\n";
        o <<prefix << "}\n";
    }
}

void
State::rename(RenameMap &rmap) 
{
    for (size_t i=0; i<n_gprs; ++i)
        gpr[i].rename(rmap);
    for (size_t i=0; i<n_segregs; ++i)
        segreg[i].rename(rmap);
    for (size_t i=0; i<n_flags; ++i)
        flag[i].rename(rmap);
    for (Memory::iterator mi=mem.begin(); mi!=mem.end(); ++mi)
        (*mi).rename(rmap);
}

State
State::normalize() const
{
    State retval = *this;
    RenameMap rmap;
    retval.rename(rmap);
    return retval;
}

#if 0
std::string
State::SHA1() const
{
    /* No need to call this every time. */
    static bool did_start_gcry = false;
    if (!did_start_gcry) {
        gcry_check_version(NULL);
        did_start_gcry = true;
    }

    /* Simple version just hashes the print form of the state */
    std::stringstream s; print(s);
    size_t digest_sz = gcry_md_get_algo_dlen(GCRY_MD_SHA1);
    char *digest = new char[digest_sz];
    gcry_md_hash_buffer(GCRY_MD_SHA1, digest, s.str().c_str(), s.str().size());
    
    /* Convert to ASCII string */
    std::string digest_str;
    for (size_t i=digest_sz; i>0; --i) {
        digest_str += "0123456789abcdef"[(digest[i-1] >> 4) & 0xf];
        digest_str += "0123456789abcdef"[digest[i-1] & 0xf];
    }

    delete[] digest;
    return digest_str;
}
#endif

void
State::discard_popped_memory() 
{
    Memory new_mem;
    const ValueType<32> &sp = gpr[x86_gpr_sp];
    for (Memory::const_iterator mi=mem.begin(); mi!=mem.end(); ++mi) {
        const ValueType<32> &addr = (*mi).address;
        if (addr.name!=sp.name || addr.negate!=sp.negate || (int32_t)addr.offset>=(int32_t)sp.offset)
            new_mem.push_back(*mi);
    }
    mem = new_mem;
}

/*************************************************************************************************************************
 *                                                          Policy
 *************************************************************************************************************************/

bool
Policy::on_stack(const ValueType<32> &value)
{
    //std::cerr <<"VirtualMachineSemantics::on_stack(value=" <<value <<"):\n";
    const ValueType<32> sp_inverted = invert(state.gpr[x86_gpr_sp]);
    //std::cerr <<"  stack pointer = " <<state.gpr[x86_gpr_sp] <<"; inverted = " <<sp_inverted <<"\n";
    for (Memory::const_iterator mi=state.mem.begin(); mi!=state.mem.end(); ++mi) {
        //std::cerr <<"  mem entry " <<(*mi) <<":\n";
        if ((*mi).nbytes!=4 || !((*mi).data==value)) continue;
        const ValueType<32> &addr = (*mi).address;

        /* Is addr >= sp? */
        ValueType<32> carries = 0;
        ValueType<32> diff = addWithCarries(addr, sp_inverted, true_(), carries/*out*/);
        //std::cerr <<"    [" <<addr <<"] + [" <<sp_inverted <<"] = [" <<diff <<"] carry=" <<carries <<"\n";
        ValueType<1> sf = extract<31,32>(diff);
        ValueType<1> of = xor_(extract<31,32>(carries), extract<30,31>(carries));
        //std::cerr <<"    sf=" <<sf <<", of=" <<of <<"\n";
        //std::cerr <<"    on stack? "<<(sf==of ? "yes" : "no") <<"\n";
        if (sf==of) return true;
    }
    return false;
}

} /*namespace*/
