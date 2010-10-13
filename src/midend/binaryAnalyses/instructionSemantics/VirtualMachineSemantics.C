#include "rose.h"
#include "VirtualMachineSemantics.h"
#include <ostream>

#ifdef HAVE_GCRYPT_H
#include <gcrypt.h>
#endif

namespace VirtualMachineSemantics {


uint64_t name_counter;

/*************************************************************************************************************************
 *                                                         ValueType
 *************************************************************************************************************************/


template<size_t Len> ValueType<Len>
ValueType<Len>::rename(RenameMap *rmap) const
{
    ValueType<Len> retval = *this;
    if (rmap && name>0) {
        RenameMap::iterator found = rmap->find(name);
        if (found==rmap->end()) {
            retval.name = rmap->size()+1;
            rmap->insert(std::make_pair(name, retval.name));
        } else {
            retval.name = found->second;
        }
    }
    return retval;
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
    return this->address == other.address;
}

void
MemoryCell::print(std::ostream &o, RenameMap *rmap/*=NULL*/) const
{
    o <<address.rename(rmap) <<": " <<data.rename(rmap) <<" " <<nbytes <<" byte" <<(1==nbytes?"":"s");
    if (!written) o <<" read-only";
    if (clobbered) o <<" clobbered";
}


/*************************************************************************************************************************
 *                                                         State
 *************************************************************************************************************************/

void
State::print(std::ostream &o, RenameMap *rmap/*=NULL*/) const
{
    std::string prefix = "    ";

    /* Print registers in columns of minimal width */
    size_t ssi=0;
    std::stringstream *ss = new std::stringstream[n_gprs + n_segregs + n_flags + 1]; /*1 for IP register*/
    for (size_t i=0; i<n_gprs; ++i)
        ss[ssi++] <<gprToString((X86GeneralPurposeRegister)i) <<"=" <<gpr[i].rename(rmap);
    for (size_t i=0; i<n_segregs; ++i)
        ss[ssi++] <<segregToString((X86SegmentRegister)i) <<"=" <<segreg[i].rename(rmap);
    for (size_t i=0; i<n_flags; ++i)
        ss[ssi++] <<flagToString((X86Flag)i) <<"=" <<flag[i].rename(rmap);
    ss[ssi++] <<"ip=" <<ip.rename(rmap);
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

    /* Print memory contents. */
    o <<prefix << "memory:\n";
    for (Memory::const_iterator mi=mem.begin(); mi!=mem.end(); ++mi) {
        o <<prefix <<"    ";
        (*mi).print(o, rmap);
        o <<"\n";
    }
}

void
State::print_diff_registers(std::ostream &o, const State &other, RenameMap *rmap/*=NULL*/) const
{
#ifndef CXX_IS_ROSE_ANALYSIS
 // DQ (5/22/2010): This code does not compile using ROSE, it needs to be investigated to be reduced to an bug report.

    std::string prefix = "    ";

    for (size_t i=0; i<n_gprs; ++i) {
        if (gpr[i]!=other.gpr[i]) {
            o <<prefix <<gprToString((X86GeneralPurposeRegister)i) <<": "
              <<gpr[i].rename(rmap) <<" -> " <<other.gpr[i].rename(rmap) <<"\n";
        }
    }
    for (size_t i=0; i<n_segregs; ++i) {
        if (segreg[i]!=other.segreg[i]) {
            o <<prefix <<segregToString((X86SegmentRegister)i) <<": "
              <<segreg[i].rename(rmap) <<" -> " <<other.segreg[i].rename(rmap) <<"\n";
        }
    }
    for (size_t i=0; i<n_flags; ++i) {
        if (flag[i]!=other.flag[i]) {
            o <<prefix <<flagToString((X86Flag)i) <<": "
              <<flag[i].rename(rmap) <<" -> " <<other.flag[i].rename(rmap) <<"\n";
        }
    }
    if (ip!=other.ip) {
        o <<prefix <<"ip: " <<ip.rename(rmap) <<" -> " <<other.ip.rename(rmap) <<"\n";
    }
#endif
}

bool
State::equal_registers(const State &other) const
{
#ifndef CXX_IS_ROSE_ANALYSIS
    for (size_t i=0; i<n_gprs; ++i)
        if (gpr[i]!=other.gpr[i]) return false;
    for (size_t i=0; i<n_segregs; ++i)
        if (segreg[i]!=other.segreg[i]) return false;
    for (size_t i=0; i<n_flags; ++i)
        if (flag[i]!=other.flag[i]) return false;
    if (ip!=other.ip) return false;
#endif
    return true;
}

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

/* Returns memory that needs to be compared by equal_states() */
Memory
Policy::memory_for_equality(const State &state) const
{
    State tmp_state = state;
    Memory retval;
#ifndef CXX_IS_ROSE_ANALYSIS
    for (Memory::const_iterator mi=state.mem.begin(); mi!=state.mem.end(); ++mi) {
        if ((*mi).is_written() && (*mi).data!=mem_read<32>(orig_state, (*mi).address))
            retval.push_back(*mi);
    }
#endif
    return retval;
}

bool
Policy::equal_states(const State &s1, const State &s2) const
{
#ifndef CXX_IS_ROSE_ANALYSIS
    if (!s1.equal_registers(s2))
        return false;
    Memory m1 = memory_for_equality(s1);
    Memory m2 = memory_for_equality(s2);
    if (m1.size()!=m2.size())
        return false;
    for (size_t i=0; i<m1.size(); ++i) {
        if (m1[i].nbytes != m2[i].nbytes ||
            m1[i].address!= m2[i].address ||
            m1[i].data   != m2[i].data)
            return false;
    }
#endif
    return true;
}

void
Policy::print(std::ostream &o, RenameMap *rmap/*=NULL*/) const
{
    cur_state.print(o, rmap);
}

void
Policy::print_diff(std::ostream &o, const State &s1, const State &s2, RenameMap *rmap/*=NULL*/) const
{
#ifndef CXX_IS_ROSE_ANALYSIS
    s1.print_diff_registers(o, s2, rmap);

    /* Get all addresses that have been written and are not currently clobbered. */
    std::set<ValueType<32> > addresses;
    for (Memory::const_iterator mi=s1.mem.begin(); mi!=s1.mem.end(); ++mi) {
        if (!(*mi).is_clobbered() && (*mi).is_written())
            addresses.insert((*mi).address);
    }
    for (Memory::const_iterator mi=s2.mem.begin(); mi!=s2.mem.end(); ++mi) {
        if (!(*mi).is_clobbered() && (*mi).is_written())
            addresses.insert((*mi).address);
    }

    State tmp_s1 = s1;
    State tmp_s2 = s2;
    size_t nmemdiff = 0;
    for (std::set<ValueType<32> >::const_iterator ai=addresses.begin(); ai!=addresses.end(); ++ai) {
        ValueType<32> v1 = mem_read<32>(tmp_s1, *ai);
        ValueType<32> v2 = mem_read<32>(tmp_s2, *ai);
        if (v1 != v2) {
            if (0==nmemdiff++) o <<"    memory:\n";
            o <<"      " <<(*ai).rename(rmap) <<": " <<v1.rename(rmap) <<" -> " <<v2.rename(rmap) <<"\n";
        }
    }
#endif
}

bool
Policy::on_stack(const ValueType<32> &value) const
{
#ifndef CXX_IS_ROSE_ANALYSIS
    const ValueType<32> sp_inverted = invert(cur_state.gpr[x86_gpr_sp]);
    for (Memory::const_iterator mi=cur_state.mem.begin(); mi!=cur_state.mem.end(); ++mi) {
        if ((*mi).nbytes!=4 || !((*mi).data==value)) continue;
        const ValueType<32> &addr = (*mi).address;

        /* Is addr >= sp? */
        ValueType<32> carries = 0;
        ValueType<32> diff = addWithCarries(addr, sp_inverted, true_(), carries/*out*/);
        ValueType<1> sf = extract<31,32>(diff);
        ValueType<1> of = xor_(extract<31,32>(carries), extract<30,31>(carries));
        if (sf==of) return true;
    }
#endif
    return false;
}

bool
Policy::SHA1(unsigned char digest[20]) const
{
#ifdef HAVE_GCRYPT_H
    /* libgcrypt requires gcry_check_version() to be called "before any other function in the library", but doesn't include an
     * API function for determining if this has already been performed. It also doesn't indicate what happens when it's called
     * more than once, or how expensive the call is.  Therefore, instead of calling it every time through this function, we'll
     * just call it the first time. */
    static bool initialized = false;
    if (!initialized) {
        gcry_check_version(NULL);
        initialized = true;
    }

    std::stringstream s;
    RenameMap rmap;
    print_diff(s, &rmap);
    ROSE_ASSERT(gcry_md_get_algo_dlen(GCRY_MD_SHA1)==20);
    gcry_md_hash_buffer(GCRY_MD_SHA1, digest, s.str().c_str(), s.str().size());
    return true;
#else
    memset(digest, 0, 20);
    return false;
#endif
}

std::string
Policy::SHA1() const
{
    std::string digest_str;
    unsigned char digest[20];
    if (SHA1(digest)) {
        for (size_t i=sizeof digest; i>0; --i) {
            digest_str += "0123456789abcdef"[(digest[i-1] >> 4) & 0xf];
            digest_str += "0123456789abcdef"[digest[i-1] & 0xf];
        }
    }
    return digest_str;
}

} /*namespace*/
