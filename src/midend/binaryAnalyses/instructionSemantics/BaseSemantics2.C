#include "sage3basic.h"
#include "BaseSemantics2.h"
#include "AsmUnparser_compat.h"
#include "Diagnostics.h"

namespace rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {

using namespace Sawyer::Message::Common;

Sawyer::Message::Facility mlog;

void
initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        mlog = Sawyer::Message::Facility("rose::BinaryAnalysis::InstructionSemantics2", Diagnostics::destination);
        Diagnostics::mfacilities.insertAndAdjust(mlog);
    }
}

namespace BaseSemantics {

/*******************************************************************************************************************************
 *                                      Printing operator<<
 *******************************************************************************************************************************/

std::ostream& operator<<(std::ostream &o, const Exception &x) {
    x.print(o);
    return o;
}

std::ostream& operator<<(std::ostream &o, const SValue &x) {
    x.print(o);
    return o;
}

std::ostream& operator<<(std::ostream &o, const SValue::WithFormatter &x)
{
    x.print(o);
    return o;
}

std::ostream& operator<<(std::ostream &o, const MemoryCell &x) {
    x.print(o);
    return o;
}

std::ostream& operator<<(std::ostream &o, const MemoryCell::WithFormatter &x)
{
    x.print(o);
    return o;
}

std::ostream& operator<<(std::ostream &o, const MemoryState &x) {
    x.print(o);
    return o;
}

std::ostream& operator<<(std::ostream &o, const MemoryState::WithFormatter &x)
{
    x.print(o);
    return o;
}

std::ostream& operator<<(std::ostream &o, const RegisterState &x) {
    x.print(o);
    return o;
}

std::ostream& operator<<(std::ostream &o, const RegisterState::WithFormatter &x)
{
    x.print(o);
    return o;
}

std::ostream& operator<<(std::ostream &o, const State &x) {
    x.print(o);
    return o;
}

std::ostream& operator<<(std::ostream &o, const State::WithFormatter &x)
{
    x.print(o);
    return o;
}

std::ostream& operator<<(std::ostream &o, const RiscOperators &x) {
    x.print(o);
    return o;
}

std::ostream& operator<<(std::ostream &o, const RiscOperators::WithFormatter &x)
{
    x.print(o);
    return o;
}

/*******************************************************************************************************************************
 *                                      Exceptions
 *******************************************************************************************************************************/

void
Exception::print(std::ostream &o) const
{
    o <<"rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::Exception: " <<what();
    if (insn)
        o <<": " <<unparseInstructionWithAddress(insn);
    o <<"\n";
}

/*******************************************************************************************************************************
 *                                      RegisterStateGeneric
 *******************************************************************************************************************************/

void
RegisterStateGeneric::clear()
{
    registers.clear();
    eraseWriters();
}

void
RegisterStateGeneric::zero()
{
    // We're initializing with a concrete value, so it really doesn't matter whether we initialize large registers
    // or small registers.  Constant folding will adjust things as necessary when we start reading registers.
    std::vector<RegisterDescriptor> regs = regdict->get_largest_registers();
    initialize_nonoverlapping(regs, true);
}

void
RegisterStateGeneric::initialize_large()
{
    std::vector<RegisterDescriptor> regs = regdict->get_largest_registers();
    initialize_nonoverlapping(regs, false);
}

void
RegisterStateGeneric::initialize_small()
{
    std::vector<RegisterDescriptor> regs = regdict->get_smallest_registers();
    initialize_nonoverlapping(regs, false);
}

void
RegisterStateGeneric::initialize_nonoverlapping(const std::vector<RegisterDescriptor> &regs, bool initialize_to_zero)
{
    clear();
    for (size_t i=0; i<regs.size(); ++i) {
        std::string name = regdict->lookup(regs[i]);
        SValuePtr val;
        if (initialize_to_zero) {
            val = get_protoval()->number_(regs[i].get_nbits(), 0);
        } else {
            val = get_protoval()->undefined_(regs[i].get_nbits());
            if (!name.empty())
                val->set_comment(name+"_0");
        }
        registers[RegStore(regs[i])].push_back(RegPair(regs[i], val));
    }
}

static bool
has_null_value(const RegisterStateGeneric::RegPair &rp)
{
    return rp.value == NULL;
}

void
RegisterStateGeneric::get_nonoverlapping_parts(const Extent &overlap, const RegPair &rp, RiscOperators *ops,
                                               RegPairs *pairs/*out*/)
{
    if (overlap.first() > rp.desc.get_offset()) { // LSB part of existing register does not overlap
        RegisterDescriptor nonoverlap_desc(rp.desc.get_major(), rp.desc.get_minor(),
                                           rp.desc.get_offset(),
                                           overlap.first() - rp.desc.get_offset());
        SValuePtr nonoverlap_val = ops->extract(rp.value, 0, nonoverlap_desc.get_nbits());
        pairs->push_back(RegPair(nonoverlap_desc, nonoverlap_val));
    }
    if (overlap.last()+1 < rp.desc.get_offset() + rp.desc.get_nbits()) { // MSB part of existing reg does not overlap
        size_t nonoverlap_first = overlap.last() + 1; // bit offset for register
        size_t nonoverlap_nbits = (rp.desc.get_offset() + rp.desc.get_nbits()) - (overlap.last() + 1);
        RegisterDescriptor nonoverlap_desc(rp.desc.get_major(), rp.desc.get_minor(), nonoverlap_first, nonoverlap_nbits);
        size_t lo_bit = nonoverlap_first - rp.desc.get_offset(); // lo bit of value to extract
        SValuePtr nonoverlap_val = ops->extract(rp.value, lo_bit, lo_bit+nonoverlap_nbits);
        pairs->push_back(RegPair(nonoverlap_desc, nonoverlap_val));
    }
}

SValuePtr
RegisterStateGeneric::readRegister(const RegisterDescriptor &reg, RiscOperators *ops)
{
    ASSERT_require(reg.is_valid());

    // Fast case: the state does not store this register or any register that might overlap with this register.
    Registers::iterator ri = registers.find(reg);
    if (ri==registers.end()) {
        size_t nbits = reg.get_nbits();
        SValuePtr newval = get_protoval()->undefined_(nbits);
        std::string regname = regdict->lookup(reg);
        if (!regname.empty())
            newval->set_comment(regname + "_0");
        registers[reg].push_back(RegPair(reg, newval));
        return newval;
    }

    // Get a list of all the (parts of) registers that might overlap with this register.
    const Extent need_extent(reg.get_offset(), reg.get_nbits());
    std::vector<SValuePtr> overlaps(reg.get_nbits()); // overlapping parts of other registers by destination bit offset
    std::vector<RegPair> nonoverlaps; // non-overlapping parts of overlapping registers
    for (RegPairs::iterator rvi=ri->second.begin(); rvi!=ri->second.end(); ++rvi) {
        Extent have_extent(rvi->desc.get_offset(), rvi->desc.get_nbits());
        if (need_extent==have_extent) {
            // exact match; no need to traverse further because a RegisterStateGeneric never stores overlapping values
            ASSERT_require(nonoverlaps.empty());
            return rvi->value;
        } else {
            const Extent overlap = need_extent.intersect(have_extent);
            if (overlap==need_extent) {
                // The stored register contains all the bits we need to read, and then some.  Extract only what we need.
                // No need to traverse further because we never store overlapping values.
                ASSERT_require(nonoverlaps.empty());
                const size_t lo_bit = need_extent.first() - have_extent.first();
                return ops->extract(rvi->value, lo_bit, lo_bit+need_extent.size());
            } else if (!overlap.empty()) {
                ASSERT_require(overlap.first() >= have_extent.first());
                size_t extractBegin = overlap.first() - have_extent.first();
                size_t extractEnd = extractBegin + overlap.size();
                ASSERT_require(extractEnd > extractBegin);
                ASSERT_require(extractEnd <= rvi->value->get_width());
                const SValuePtr overlap_val = ops->extract(rvi->value, extractBegin, extractEnd);
                ASSERT_require(overlap_val->get_width() == overlap.size());
                ASSERT_require(overlap.first() >= need_extent.first());
                size_t offsetWrtResult = overlap.first() - need_extent.first();
                overlaps[offsetWrtResult] = overlap_val;

                if (coalesceOnRead) {
                    // If any part of the existing register is not represented by the register being read, then we need to save
                    // that part.
                    get_nonoverlapping_parts(overlap, *rvi, ops, &nonoverlaps /*out*/);

                    // Mark the overlapping register by setting it to null so we can remove it later.
                    rvi->value = SValuePtr();
                }
            }
        }
    }

    // Remove the overlapping registers that we marked above, and replace them with their non-overlapping parts.
    if (coalesceOnRead) {
        ri->second.erase(std::remove_if(ri->second.begin(), ri->second.end(), has_null_value), ri->second.end());
        ri->second.insert(ri->second.end(), nonoverlaps.begin(), nonoverlaps.end());
    }

    // Compute the return value by concatenating the overlapping parts and filling in any missing parts.
    SValuePtr retval;
    size_t offset = 0;
    while (offset<reg.get_nbits()) {
        SValuePtr part;
        if (overlaps[offset]!=NULL) {
            part = overlaps[offset];
        } else {
            size_t next_offset = offset+1;
            while (next_offset<reg.get_nbits() && overlaps[next_offset]==NULL) ++next_offset;
            size_t nbits = next_offset - offset;
            part = get_protoval()->undefined_(nbits);
            if (!coalesceOnRead) {
                // This part of the return value doesn't exist in the register state, so we need to add it.  When
                // coalesceOnRead is set we'll insert the whole value at once at the very end.
                RegisterDescriptor subreg(reg.get_major(), reg.get_minor(), reg.get_offset()+offset, nbits);
                ri->second.push_back(RegPair(subreg, part));
            }
        }
        retval = retval==NULL ? part : ops->concat(retval, part);
        offset += part->get_width();
    }

    // Insert the return value.  When coalesceOnRead is set then no part of the register we just read overlaps with anything
    // already stored because we've removed those parts of registers that overlap.  In other words, there's already a hole in
    // which we can insert the value we're about to return.
    if (coalesceOnRead)
        ri->second.push_back(RegPair(reg, retval));

    ASSERT_require(reg.get_nbits()==retval->get_width());
    return retval;
}

void
RegisterStateGeneric::writeRegister(const RegisterDescriptor &reg, const SValuePtr &value, RiscOperators *ops)
{
    ASSERT_not_null(value);
    ASSERT_require2(reg.get_nbits()==value->get_width(), "value written to register must be the same width as the register");
    erase_register(reg, ops);
    Registers::iterator ri = registers.find(reg);
    if (ri == registers.end()) {
        registers[reg].push_back(RegPair(reg, value));
    } else {
        ri->second.push_back(RegPair(reg, value));
    }
}

void
RegisterStateGeneric::erase_register(const RegisterDescriptor &reg, RiscOperators *ops)
{
    ASSERT_require(reg.is_valid());

    // Fast case: the state does not store this register or any register that might overlap with this register
    Registers::iterator ri = registers.find(reg);
    if (ri == registers.end())
        return;                                         // no part of register is stored in this state

    // Look for existing registers that overlap with this register and remove them.  If the overlap was only partial, then we
    // need to eventually add the non-overlapping part back into the list.
    RegPairs nonoverlaps; // the non-overlapping parts of overlapping registers
    Extent need_extent(reg.get_offset(), reg.get_nbits());
    for (RegPairs::iterator rvi=ri->second.begin(); rvi!=ri->second.end(); ++rvi) {
        Extent have_extent(rvi->desc.get_offset(), rvi->desc.get_nbits());
        Extent overlap = need_extent.intersect(have_extent);
        if (!overlap.empty()) {
            get_nonoverlapping_parts(overlap, *rvi, ops, &nonoverlaps);
            rvi->value = SValuePtr(); // mark pair for removal by setting it to null
        }
    }

    // Remove marked pairs, then add the non-overlapping parts.
    ri->second.erase(std::remove_if(ri->second.begin(), ri->second.end(), has_null_value), ri->second.end());
    ri->second.insert(ri->second.end(), nonoverlaps.begin(), nonoverlaps.end());
}

RegisterStateGeneric::RegPairs
RegisterStateGeneric::get_stored_registers() const
{
    RegPairs retval;
    for (Registers::const_iterator ri=registers.begin(); ri!=registers.end(); ++ri)
        retval.insert(retval.end(), ri->second.begin(), ri->second.end());
    return retval;
}

void
RegisterStateGeneric::traverse(Visitor &visitor)
{
    for (Registers::iterator ri=registers.begin(); ri!=registers.end(); ++ri) {
        for (RegPairs::iterator rpi=ri->second.begin(); rpi!=ri->second.end(); ++rpi) {
            SValuePtr newval = (visitor)(rpi->desc, rpi->value);
            if (newval!=NULL) {
                ASSERT_require(newval->get_width()==rpi->desc.get_nbits());
                rpi->value = newval;
            }
        }
    }
}

void
RegisterStateGeneric::deep_copy_values()
{
    for (Registers::iterator ri=registers.begin(); ri!=registers.end(); ++ri) {
        for (RegPairs::iterator pi=ri->second.begin(); pi!=ri->second.end(); ++pi)
            pi->value = pi->value->copy();
    }
}

bool
RegisterStateGeneric::is_partly_stored(const RegisterDescriptor &desc) const
{
    Extent want(desc.get_offset(), desc.get_nbits());
    Registers::const_iterator ri = registers.find(RegStore(desc));
    if (ri!=registers.end()) {
        for (RegPairs::const_iterator pi=ri->second.begin(); pi!=ri->second.end(); ++pi) {
            const RegisterDescriptor &desc = pi->desc;
            Extent have(desc.get_offset(), desc.get_nbits());
            if (want.overlaps(have))
                return true;
        }
    }
    return false;
}

bool
RegisterStateGeneric::is_wholly_stored(const RegisterDescriptor &desc) const
{
    Extent want(desc.get_offset(), desc.get_nbits());
    ExtentMap have = stored_parts(desc);
    return 1==have.nranges() && have.minmax()==want;
}

bool
RegisterStateGeneric::is_exactly_stored(const RegisterDescriptor &desc) const
{
    Registers::const_iterator ri = registers.find(RegStore(desc));
    if (ri!=registers.end()) {
        for (RegPairs::const_iterator pi=ri->second.begin(); pi!=ri->second.end(); ++pi) {
            if (desc==pi->desc)
                return true;
        }
    }
    return false;
}

ExtentMap
RegisterStateGeneric::stored_parts(const RegisterDescriptor &desc) const
{
    ExtentMap retval;
    Extent want(desc.get_offset(), desc.get_nbits());
    Registers::const_iterator ri = registers.find(RegStore(desc));
    if (ri!=registers.end()) {
        for (RegPairs::const_iterator pi=ri->second.begin(); pi!=ri->second.end(); ++pi) {
            const RegisterDescriptor &desc = pi->desc;
            Extent have(desc.get_offset(), desc.get_nbits());
            retval.insert(want.intersect(have));
        }
    }
    return retval;
}

void
RegisterStateGeneric::insertWriter(const RegisterDescriptor &desc, rose_addr_t writerVa) {
    WrittenParts &parts = writers.insertMaybe(desc, WrittenParts());
    BitRange where = BitRange::baseSize(desc.get_offset(), desc.get_nbits());
    parts.insert(where, writerVa);
}

void
RegisterStateGeneric::eraseWriter(const RegisterDescriptor &desc, rose_addr_t writerVa) {
    if (!writers.exists(desc))
        return;
    WrittenParts &parts = writers[desc];
    BitRange where = BitRange::baseSize(desc.get_offset(), desc.get_nbits());
    parts.erase(where, writerVa);
    if (parts.isEmpty())
        writers.erase(desc);
}

void
RegisterStateGeneric::setWriters(const RegisterDescriptor &desc, rose_addr_t writerVa) {
    AddressSet writersSet;
    writersSet.insert(writerVa);
    setWriters(desc, writersSet);
}

void
RegisterStateGeneric::setWriters(const RegisterDescriptor &desc, const AddressSet &writerVas) {
    WrittenParts &parts = writers.insertMaybe(desc, WrittenParts());
    BitRange where = BitRange::baseSize(desc.get_offset(), desc.get_nbits());
    parts.replace(where, writerVas);
}

// [Robb P. Matzke 2015-08-07]: deprecated
void
RegisterStateGeneric::set_latest_writer(const RegisterDescriptor &desc, rose_addr_t writer_va)
{
    setWriters(desc, writer_va);
}

void
RegisterStateGeneric::eraseWriters(const RegisterDescriptor &desc) {
    writers.erase(desc);
}

// [Robb P. Matzke 2015-08-07]: deprecated
void
RegisterStateGeneric::clear_latest_writer(const RegisterDescriptor &desc)
{
    eraseWriters(desc);
}

void
RegisterStateGeneric::eraseWriters() {
    writers.clear();
}

// [Robb P. Matzke 2015-08-07]: deprecated
void
RegisterStateGeneric::clear_latest_writers()
{
    eraseWriters();
}

RegisterStateGeneric::AddressSet
RegisterStateGeneric::getWritersUnion(const RegisterDescriptor &desc) const {
    if (!writers.exists(desc))
        return AddressSet();
    const WrittenParts &parts = writers[desc];
    BitRange where = BitRange::baseSize(desc.get_offset(), desc.get_nbits());
    return parts.getUnion(where);
}

RegisterStateGeneric::AddressSet
RegisterStateGeneric::getWritersIntersection(const RegisterDescriptor &desc) const {
    if (!writers.exists(desc))
        return AddressSet();
    const WrittenParts &parts = writers[desc];
    BitRange where = BitRange::baseSize(desc.get_offset(), desc.get_nbits());
    return parts.getIntersection(where);
}

// [Robb P. Matzke 2015-08-07]: deprecated, use getWritersUnion instead
std::set<rose_addr_t>
RegisterStateGeneric::get_latest_writers(const RegisterDescriptor &desc) const
{
    AddressSet writerVas = getWritersUnion(desc);
    std::set<rose_addr_t> retval(writerVas.values().begin(), writerVas.values().end());
    return retval;
}

static bool
sortByOffset(const RegisterStateGeneric::RegPair &a, const RegisterStateGeneric::RegPair &b) {
    return a.desc.get_offset() < b.desc.get_offset();
}

void
RegisterStateGeneric::print(std::ostream &stream, Formatter &fmt) const
{
    const RegisterDictionary *regdict = fmt.get_register_dictionary();
    if (!regdict)
        regdict = get_register_dictionary();
    RegisterNames regnames(regdict);

    // First pass is to get the maximum length of the register names; second pass prints
    FormatRestorer oflags(stream);
    size_t maxlen = 6; // use at least this many columns even if register names are short.
    for (int i=0; i<2; ++i) {
        for (Registers::const_iterator ri=registers.begin(); ri!=registers.end(); ++ri) {
            RegPairs regPairs = ri->second;
            std::sort(regPairs.begin(), regPairs.end(), sortByOffset);
            for (RegPairs::const_iterator rvi=regPairs.begin(); rvi!=regPairs.end(); ++rvi) {
                std::string regname = regnames(rvi->desc);
                if (!fmt.get_suppress_initial_values() || rvi->value->get_comment().empty() ||
                    0!=rvi->value->get_comment().compare(regname+"_0")) {
                    if (0==i) {
                        maxlen = std::max(maxlen, regname.size());
                    } else {
                        stream <<fmt.get_line_prefix() <<std::setw(maxlen) <<std::left <<regname;
                        oflags.restore();
                        if (fmt.get_show_latest_writers()) {
                            Sawyer::Container::Set<rose_addr_t> writers = getWritersUnion(rvi->desc);
                            if (writers.size()==1) {
                                stream <<" [writer=" <<StringUtility::addrToString(*writers.values().begin()) <<"]";
                            } else if (!writers.isEmpty()) {
                                stream <<" [writers={";
                                for (Sawyer::Container::Set<rose_addr_t>::ConstIterator wi=writers.values().begin();
                                     wi!=writers.values().end(); ++wi) {
                                    stream <<(wi==writers.values().begin()?"":", ") <<StringUtility::addrToString(*wi);
                                }
                                stream <<"}]";
                            }
                        }
                        stream <<" = ";
                        rvi->value->print(stream, fmt);
                        stream <<"\n";
                    }
                }
            }
        }
    }
}


/*******************************************************************************************************************************
 *                                      RegisterStateX86
 *******************************************************************************************************************************/

std::string
RegisterStateX86::initialValueName(const RegisterDescriptor &reg) const {
    std::string s;
    if (regdict!=NULL) {
        s = regdict->lookup(reg);
        if (!s.empty())
            s += "_0";
    }
    return s;
}

void
RegisterStateX86::clear()
{
    ip = protoval->undefined_(32);
    for (size_t i=0; i<n_gprs; ++i) {
        const RegisterDescriptor reg(x86_regclass_gpr, i, 0, 32);
        gpr[i] = protoval->undefined_(32);
        gpr[i]->set_comment(initialValueName(reg));
    }
    for (size_t i=0; i<n_segregs; ++i) {
        const RegisterDescriptor reg(x86_regclass_segment, i, 0, 16);
        segreg[i] = protoval->undefined_(16);
        segreg[i]->set_comment(initialValueName(reg));
    }
    for (size_t i=0; i<n_flags; ++i) {
        const RegisterDescriptor reg(x86_regclass_flags, x86_flags_status, i, 1);
        flag[i] = protoval->undefined_(1);
        flag[i]->set_comment(initialValueName(reg));
    }
    for (size_t i=0; i<n_st; ++i) {
        const RegisterDescriptor reg(x86_regclass_st, i, 0, 80);
        st[i] = protoval->undefined_(80);
        st[i]->set_comment(initialValueName(reg));
    }
    for (size_t i=0; i<n_xmm; ++i) {
        const RegisterDescriptor reg(x86_regclass_xmm, i, 0, 128);
        xmm[i] = protoval->undefined_(128);
        xmm[i]->set_comment(initialValueName(reg));
    }
    
    const RegisterDescriptor reg(x86_regclass_flags, x86_flags_fpstatus, 0, 16);
    fpstatus = protoval->undefined_(16);
    fpstatus->set_comment(initialValueName(reg));
}

void
RegisterStateX86::zero()
{
    ip = protoval->number_(32, 0);
    for (size_t i=0; i<n_gprs; ++i)
        gpr[i] = protoval->number_(32, 0);
    for (size_t i=0; i<n_segregs; ++i)
        segreg[i] = protoval->number_(16, 0);
    for (size_t i=0; i<n_flags; ++i)
        flag[i] = protoval->number_(1, 0);
    for (size_t i=0; i<n_st; ++i)
        st[i] = protoval->number_(80, 0);
    for (size_t i=0; i<n_xmm; ++i)
        xmm[i] = protoval->number_(128, 0);
    fpstatus = protoval->number_(16, 0);
}

SValuePtr
RegisterStateX86::readRegister(const RegisterDescriptor &reg, RiscOperators *ops)
{
    switch (reg.get_major()) {
        case x86_regclass_gpr:
            return readRegisterGpr(reg, ops);
        case x86_regclass_flags:
            if (reg.get_minor()==x86_flags_status)
                return readRegisterFlag(reg, ops);
            if (reg.get_minor()==x86_flags_fpstatus)
                return readRegisterFpStatus(reg, ops);
            throw Exception("invalid flags minor number: " + StringUtility::numberToString(reg.get_minor()),
                            ops->get_insn());
        case x86_regclass_segment:
            return readRegisterSeg(reg, ops);
        case x86_regclass_ip:
            return readRegisterIp(reg, ops);
        case x86_regclass_st:
            return readRegisterSt(reg, ops);
        case x86_regclass_xmm:
            return readRegisterXmm(reg, ops);
        default:
            throw Exception("invalid register major number: "+StringUtility::numberToString(reg.get_major())+
                            " (wrong RegisterDictionary?)", ops->get_insn());
    }
}

SValuePtr
RegisterStateX86::readRegisterGpr(const RegisterDescriptor &reg, RiscOperators *ops)
{
    using namespace StringUtility;
    ASSERT_not_null(ops);
    ASSERT_require(reg.get_major()==x86_regclass_gpr);
    if (reg.get_minor()>=n_gprs)
        throw Exception("invalid general purpose register minor number: "+numberToString(reg.get_minor()), ops->get_insn());
    if (reg.get_nbits()!=8 && reg.get_nbits()!=16 && reg.get_nbits()!=32)
        throw Exception("invalid general purpose register width: "+numberToString(reg.get_nbits()), ops->get_insn());
    if ((reg.get_offset()!=0 && (reg.get_offset()!=8 || reg.get_nbits()!=8)) ||
        (reg.get_offset() + reg.get_nbits() > 32))
        throw Exception("invalid general purpose sub-register: offset="+numberToString(reg.get_offset())+
                        " width="+numberToString(reg.get_nbits()), ops->get_insn());
    SValuePtr result = gpr[reg.get_minor()];
    if (reg.get_offset()!=0 || reg.get_nbits()!=32)
        result = ops->extract(result, reg.get_offset(), reg.get_offset()+reg.get_nbits());
    ASSERT_require(result->get_width()==reg.get_nbits());
    return result;
}

SValuePtr
RegisterStateX86::readRegisterFlag(const RegisterDescriptor &reg, RiscOperators *ops)
{
    using namespace StringUtility;
    ASSERT_not_null(ops);
    ASSERT_require(reg.get_major()==x86_regclass_flags);
    if (reg.get_minor()!=0)
        throw Exception("invalid flags register minor numbr: "+numberToString(reg.get_minor()), ops->get_insn());
    if (reg.get_nbits()!=1 && reg.get_nbits()!=16 && reg.get_nbits()!=32)
        throw Exception("invalid flag register width: "+numberToString(reg.get_nbits()), ops->get_insn());
    if ((reg.get_nbits()>1 && reg.get_offset()!=0) ||
        (reg.get_offset() + reg.get_nbits() > 32))
        throw Exception("invalid flag subregister: offset="+numberToString(reg.get_offset())+
                        " width="+numberToString(reg.get_nbits()), ops->get_insn());
    SValuePtr result = flag[reg.get_offset()];
    for (size_t i=1; i<reg.get_nbits(); ++i)
        result = ops->concat(result, flag[reg.get_offset()+i]);
    ASSERT_require(result->get_width()==reg.get_nbits());
    return result;
}

SValuePtr
RegisterStateX86::readRegisterSeg(const RegisterDescriptor &reg, RiscOperators *ops)
{
    using namespace StringUtility;
    ASSERT_not_null(ops);
    ASSERT_require(reg.get_major()==x86_regclass_segment);
    if (reg.get_minor()>=n_segregs)
        throw Exception("invalid segment register minor number: "+numberToString(reg.get_minor()), ops->get_insn());
    if (reg.get_offset()!=0 || reg.get_nbits()!=16)
        throw Exception("invalid segment subregister: offset="+numberToString(reg.get_offset())+
                        " width="+numberToString(reg.get_nbits()), ops->get_insn());
    SValuePtr result = segreg[reg.get_minor()];
    ASSERT_require(result->get_width()==reg.get_nbits());
    return result;
}

SValuePtr
RegisterStateX86::readRegisterIp(const RegisterDescriptor &reg, RiscOperators *ops)
{
    using namespace StringUtility;
    ASSERT_not_null(ops);
    ASSERT_require(reg.get_major()==x86_regclass_ip);
    if (reg.get_minor()!=0)
        throw Exception("invalid instruction pointer register minor number: "+numberToString(reg.get_minor()), ops->get_insn());
    if (reg.get_offset()!=0 || reg.get_nbits()!=32)
        throw Exception("invalid instruction pointer subregister: offset="+numberToString(reg.get_offset())+
                        " width="+numberToString(reg.get_nbits()), ops->get_insn());
    ASSERT_require(ip->get_width()==reg.get_nbits());
    return ip;
}

SValuePtr
RegisterStateX86::readRegisterSt(const RegisterDescriptor &reg, RiscOperators *ops)
{
    ASSERT_require(reg.get_major()==x86_regclass_st);
    ASSERT_require(reg.get_minor()<8);
    ASSERT_require(reg.get_offset()==0);
    SValuePtr retval = st[reg.get_minor()];
    ASSERT_require(retval!=NULL && retval->get_width()==80);
    if (reg.get_nbits()==64)
        retval = ops->extract(retval, 0, 64);           // reading MM register, low-order 64 bits of ST
    return retval;
}

SValuePtr
RegisterStateX86::readRegisterXmm(const RegisterDescriptor &reg, RiscOperators *ops)
{
    ASSERT_require(reg.get_major()==x86_regclass_xmm);
    ASSERT_require(reg.get_minor()<8);
    ASSERT_require(reg.get_offset()==0);
    ASSERT_require(reg.get_nbits()==128);
    SValuePtr retval = xmm[reg.get_minor()];
    ASSERT_require(retval!=NULL && retval->get_width()==128);
    return retval;
}

SValuePtr
RegisterStateX86::readRegisterFpStatus(const RegisterDescriptor &reg, RiscOperators *ops)
{
    ASSERT_require(reg.get_major()==x86_regclass_flags);
    ASSERT_require(reg.get_minor()==x86_flags_fpstatus);
    ASSERT_require(reg.get_offset() + reg.get_nbits() <= 16);
    ASSERT_require(fpstatus!=NULL && fpstatus->get_width()==16);
    if (16==reg.get_nbits())
        return fpstatus;
    return ops->extract(fpstatus, reg.get_offset(), reg.get_offset()+reg.get_nbits());
}

void
RegisterStateX86::writeRegister(const RegisterDescriptor &reg, const SValuePtr &value, RiscOperators *ops)
{
    switch (reg.get_major()) {
        case x86_regclass_gpr:
            return writeRegisterGpr(reg, value, ops);
        case x86_regclass_flags:
            if (reg.get_minor()==x86_flags_status)
                return writeRegisterFlag(reg, value, ops);
            if (reg.get_minor()==x86_flags_fpstatus)
                return writeRegisterFpStatus(reg, value, ops);
            throw Exception("invalid register minor number: " + StringUtility::numberToString(reg.get_minor()),
                            ops->get_insn());
        case x86_regclass_segment:
            return writeRegisterSeg(reg, value, ops);
        case x86_regclass_ip:
            return writeRegisterIp(reg, value, ops);
        case x86_regclass_st:
            return writeRegisterSt(reg, value, ops);
        case x86_regclass_xmm:
            return writeRegisterXmm(reg, value, ops);
        default:
            throw Exception("invalid register major number: "+StringUtility::numberToString(reg.get_major())+
                            " (wrong RegisterDictionary?)", ops->get_insn());
    }
}

void
RegisterStateX86::writeRegisterGpr(const RegisterDescriptor &reg, const SValuePtr &value, RiscOperators *ops)
{
    using namespace StringUtility;
    ASSERT_require(reg.get_major()==x86_regclass_gpr);
    ASSERT_require(value!=NULL && value->get_width()==reg.get_nbits());
    ASSERT_not_null(ops);
    if (reg.get_minor()>=n_gprs)
        throw Exception("invalid general purpose register minor number: "+numberToString(reg.get_minor()), ops->get_insn());
    if (reg.get_nbits()!=8 && reg.get_nbits()!=16 && reg.get_nbits()!=32)
        throw Exception("invalid general purpose register width: "+numberToString(reg.get_nbits()), ops->get_insn());
    if ((reg.get_offset()!=0 && (reg.get_offset()!=8 || reg.get_nbits()!=8)) ||
        (reg.get_offset() + reg.get_nbits() > 32))
        throw Exception("invalid general purpose sub-register: offset="+numberToString(reg.get_offset())+
                        " width="+numberToString(reg.get_nbits()), ops->get_insn());
    SValuePtr towrite = value;
    if (reg.get_offset()!=0)
        towrite = ops->concat(ops->extract(gpr[reg.get_minor()], 0, reg.get_offset()), towrite);
    if (reg.get_offset() + reg.get_nbits()<32)
        towrite = ops->concat(towrite, ops->extract(gpr[reg.get_minor()], reg.get_offset()+reg.get_nbits(), 32));
    ASSERT_require(towrite->get_width()==32);
    gpr[reg.get_minor()] = towrite;
}

void
RegisterStateX86::writeRegisterFlag(const RegisterDescriptor &reg, const SValuePtr &value, RiscOperators *ops)
{
    using namespace StringUtility;
    ASSERT_require(reg.get_major()==x86_regclass_flags);
    ASSERT_require(value!=NULL && value->get_width()==reg.get_nbits());
    ASSERT_not_null(ops);
    if (reg.get_minor()!=0)
        throw Exception("invalid flags register minor numbr: "+numberToString(reg.get_minor()), ops->get_insn());
    if (reg.get_nbits()!=1 && reg.get_nbits()!=16 && reg.get_nbits()!=32)
        throw Exception("invalid flag register width: "+numberToString(reg.get_nbits()), ops->get_insn());
    if ((reg.get_nbits()>1 && reg.get_offset()!=0) ||
        (reg.get_offset() + reg.get_nbits() > 32))
        throw Exception("invalid flag subregister: offset="+numberToString(reg.get_offset())+
                        " width="+numberToString(reg.get_nbits()), ops->get_insn());
    if (reg.get_nbits()==1) {
        flag[reg.get_offset()] = value;
    } else {
        for (size_t i=0; i<reg.get_nbits(); ++i)
            flag[reg.get_offset()+i] = ops->extract(value, i, 1);
    }
}

void
RegisterStateX86::writeRegisterSeg(const RegisterDescriptor &reg, const SValuePtr &value, RiscOperators *ops)
{
    using namespace StringUtility;
    ASSERT_require(reg.get_major()==x86_regclass_segment);
    ASSERT_require(value!=NULL && value->get_width()==reg.get_nbits());
    ASSERT_not_null(ops);
    if (reg.get_minor()>=n_segregs)
        throw Exception("invalid segment register minor number: "+numberToString(reg.get_minor()), ops->get_insn());
    if (reg.get_offset()!=0 || reg.get_nbits()!=16)
        throw Exception("invalid segment subregister: offset="+numberToString(reg.get_offset())+
                        " width="+numberToString(reg.get_nbits()), ops->get_insn());
    segreg[reg.get_minor()] = value;
}

void
RegisterStateX86::writeRegisterIp(const RegisterDescriptor &reg, const SValuePtr &value, RiscOperators *ops)
{
    using namespace StringUtility;
    ASSERT_require(reg.get_major()==x86_regclass_ip);
    ASSERT_require(value!=NULL && value->get_width()==reg.get_nbits());
    ASSERT_not_null(ops);
    if (reg.get_minor()!=0)
        throw Exception("invalid instruction pointer register minor number: "+numberToString(reg.get_minor()), ops->get_insn());
    if (reg.get_offset()!=0 || reg.get_nbits()!=32)
        throw Exception("invalid instruction pointer subregister: offset="+numberToString(reg.get_offset())+
                        " width="+numberToString(reg.get_nbits()), ops->get_insn());
    ip = value;
}

void
RegisterStateX86::writeRegisterSt(const RegisterDescriptor &reg, const SValuePtr &value, RiscOperators *ops)
{
    ASSERT_require(reg.get_major()==x86_regclass_st);
    ASSERT_require(reg.get_minor()<8);
    ASSERT_require(reg.get_offset()==0);
    ASSERT_require(reg.get_nbits()==80);
    ASSERT_not_null(value);
    ASSERT_require(value->get_width()==80);
    st[reg.get_minor()] = value;
}

void
RegisterStateX86::writeRegisterXmm(const RegisterDescriptor &reg, const SValuePtr &value, RiscOperators *ops)
{
    ASSERT_require(reg.get_major()==x86_regclass_xmm);
    ASSERT_require(reg.get_minor()<8);
    ASSERT_require(reg.get_offset()==0);
    ASSERT_require(reg.get_nbits()==128);
    ASSERT_not_null(value);
    ASSERT_require(value->get_width()==128);
    xmm[reg.get_minor()] = value;
}

void
RegisterStateX86::writeRegisterFpStatus(const RegisterDescriptor &reg, const SValuePtr &value, RiscOperators *ops)
{
    ASSERT_require(reg.get_major()==x86_regclass_flags);
    ASSERT_require(reg.get_minor()==x86_flags_fpstatus);
    ASSERT_require(reg.get_offset()+reg.get_nbits() <= 16);
    ASSERT_require(value!=NULL && value->get_width()==reg.get_nbits());

    SValuePtr towrite = value;
    if (reg.get_offset()!=0)
        towrite = ops->concat(ops->extract(fpstatus, 0, reg.get_offset()), towrite);
    if (reg.get_offset() + reg.get_nbits()<32)
        towrite = ops->concat(towrite, ops->extract(fpstatus, reg.get_offset()+reg.get_nbits(), 16));
    ASSERT_require(towrite->get_width()==16);
    fpstatus = towrite;
}

void
RegisterStateX86::print(std::ostream &stream, Formatter &fmt) const 
{
    const RegisterDictionary *regdict = fmt.get_register_dictionary();
    if (!regdict)
        regdict = get_register_dictionary();
    RegisterNames regnames(regdict);

    struct ShouldShow {
        Formatter &fmt;
        ShouldShow(Formatter &fmt): fmt(fmt) {}
        bool operator()(const std::string &regname, const BaseSemantics::SValuePtr &value) {
            return (!fmt.get_suppress_initial_values() || value->get_comment().empty() ||
                    0!=value->get_comment().compare(regname+"_0"));
        }
    } should_show(fmt);

    // Two passes: first figure out column widths for the register names, then print
    size_t namewidth = 7;
    FormatRestorer save_fmt(stream);
    for (int pass=0; pass<2; ++pass) {
        for (size_t i=0; i<n_gprs; ++i) {
            std::string regname = regnames(RegisterDescriptor(x86_regclass_gpr, i, 0, 32));
            if (should_show(regname, gpr[i])) {
                if (0==pass) {
                    namewidth = std::max(namewidth, regname.size());
                } else {
                    stream <<fmt.get_line_prefix() <<std::setw(namewidth) <<std::left <<regname
                           <<" = { " <<(*gpr[i]+fmt) <<" }\n";
                }
            }
        }
        for (size_t i=0; i<n_segregs; ++i) {
            std::string regname = regnames(RegisterDescriptor(x86_regclass_segment, i, 0, 16));
            if (should_show(regname, segreg[i])) {
                if (0==pass) {
                    namewidth = std::max(namewidth, regname.size());
                } else {
                    stream <<fmt.get_line_prefix() <<std::setw(namewidth) <<std::left <<regname
                           <<" = { " <<(*segreg[i]+fmt) <<" }\n";
                }
            }
        }
        for (size_t i=0; i<n_flags; ++i) {
            std::string regname = regnames(RegisterDescriptor(x86_regclass_flags, 0, i, 1)); // flags are all part of minor #0
            if (should_show(regname, flag[i])) {
                if (0==pass) {
                    namewidth = std::max(namewidth, regname.size());
                } else {
                    stream <<fmt.get_line_prefix() <<std::setw(namewidth) <<std::left <<regname
                           <<" = { " <<(*flag[i]+fmt) <<" }\n";
                }
            }
        }
        for (size_t i=0; i<n_st; ++i) {
            std::string regname = regnames(RegisterDescriptor(x86_regclass_st, i, 0, 80));
            if (should_show(regname, st[i])) {
                if (0==pass) {
                    namewidth = std::max(namewidth, regname.size());
                } else {
                    stream <<fmt.get_line_prefix() <<std::setw(namewidth) <<std::left <<regname
                           <<" = { " <<(*st[i]+fmt) <<" }\n";
                }
            }
        }
        for (size_t i=0; i<n_xmm; ++i) {
            std::string regname = regnames(RegisterDescriptor(x86_regclass_xmm, i, 0, 128));
            if (should_show(regname, xmm[i])) {
                if (0==pass) {
                    namewidth = std::max(namewidth, regname.size());
                } else {
                    stream <<fmt.get_line_prefix() <<std::setw(namewidth) <<std::left <<regname
                           <<" = { " <<(*xmm[i]+fmt) <<" }\n";
                }
            }
        }
        {
            std::string regname = regnames(RegisterDescriptor(x86_regclass_flags, x86_flags_fpstatus, 0, 16));
            if (should_show(regname, fpstatus)) {
                if (0==pass) {
                    namewidth = std::max(namewidth, regname.size());
                } else {
                    stream <<fmt.get_line_prefix() <<std::setw(namewidth) <<std::left <<regname
                           <<" = { " <<(*fpstatus+fmt) <<" }\n";
                }
            }
        }
        {
            std::string regname = regnames(RegisterDescriptor(x86_regclass_ip, 0, 0, 32));
            if (should_show(regname, ip)) {
                if (0==pass) {
                    namewidth = std::max(namewidth, regname.size());
                } else {
                    stream <<fmt.get_line_prefix() <<std::setw(namewidth) <<std::left <<regname
                           <<" = { " <<(*ip+fmt) <<" }\n";
                }
            }
        }
    }
}

/*******************************************************************************************************************************
 *                                      MemoryState
 *******************************************************************************************************************************/

bool
MemoryCell::may_alias(const MemoryCellPtr &other, RiscOperators *addrOps) const
{
    // Check for the easy case:  two one-byte cells may alias one another if their addresses may be equal.
    if (8==value_->get_width() && 8==other->get_value()->get_width())
        return address_->may_equal(other->get_address(), addrOps->get_solver());

    size_t addr_nbits = address_->get_width();
    ASSERT_require(other->get_address()->get_width()==addr_nbits);

    ASSERT_require(value_->get_width() % 8 == 0);       // memory is byte addressable, so values must be multiples of a byte
    SValuePtr lo1 = address_;
    SValuePtr hi1 = addrOps->add(lo1, addrOps->number_(lo1->get_width(), value_->get_width() / 8));

    ASSERT_require(other->get_value()->get_width() % 8 == 0);
    SValuePtr lo2 = other->get_address();
    SValuePtr hi2 = addrOps->add(lo2, addrOps->number_(lo2->get_width(), other->get_value()->get_width() / 8));

    // Two cells may_alias iff we can prove that they are not disjoint.  The two cells are disjoint iff lo2 >= hi1 or lo1 >=
    // hi2. Two things complicate this: first, the values might not be known quantities, depending on the semantic domain.
    // Second, the RiscOperators does not define a greater-than-or-equal operation, so we need to write it in terms of a
    // subtraction. See x86 CMP and JG instructions for examples. ("sf" is sign flag, "of" is overflow flag.)
    SValuePtr carries;
    SValuePtr diff = addrOps->addWithCarries(lo2, addrOps->invert(hi1), addrOps->boolean_(true), carries/*out*/);
    SValuePtr sf = addrOps->extract(diff, addr_nbits-1, addr_nbits);
    SValuePtr of = addrOps->xor_(addrOps->extract(carries, addr_nbits-1, addr_nbits),
                                 addrOps->extract(carries, addr_nbits-2, addr_nbits-1));
    SValuePtr cond1 = addrOps->invert(addrOps->xor_(sf, of));
    diff = addrOps->addWithCarries(lo1, addrOps->invert(hi2), addrOps->boolean_(true), carries/*out*/);
    sf = addrOps->extract(diff, addr_nbits-1, addr_nbits);
    of = addrOps->xor_(addrOps->extract(carries, addr_nbits-1, addr_nbits),
                       addrOps->extract(carries, addr_nbits-2, addr_nbits-1));
    SValuePtr cond2 = addrOps->invert(addrOps->xor_(sf, of));
    SValuePtr disjoint = addrOps->or_(cond1, cond2);
    if (disjoint->is_number() && disjoint->get_number()!=0)
        return false;
    return true;
}

bool
MemoryCell::must_alias(const MemoryCellPtr &other, RiscOperators *addrOps) const
{
    // Check the easy case: two one-byte cells must alias one another if their address must be equal.
    if (8==value_->get_width() && 8==other->get_value()->get_width())
        return address_->must_equal(other->get_address(), addrOps->get_solver());

    size_t addr_nbits = address_->get_width();
    ASSERT_require(other->get_address()->get_width()==addr_nbits);

    ASSERT_require(value_->get_width() % 8 == 0);
    SValuePtr lo1 = address_;
    SValuePtr hi1 = addrOps->add(lo1, addrOps->number_(lo1->get_width(), value_->get_width() / 8));

    ASSERT_require(other->get_value()->get_width() % 8 == 0);
    SValuePtr lo2 = other->get_address();
    SValuePtr hi2 = addrOps->add(lo2, addrOps->number_(lo2->get_width(), other->get_value()->get_width() / 8));

    // Two cells must_alias iff hi2 >= lo1 and hi1 >= lo2. Two things complicate this: first, the values might not be known
    // quantities, depending on the semantic domain.  Second, the RiscOperators does not define a greater-than-or-equal
    // operation, so we need to write it in terms of a subtraction. See x86 CMP and JG instructions for examples. ("sf" is sign
    // flag, "of" is overflow flag.)
    SValuePtr carries;
    SValuePtr diff = addrOps->addWithCarries(hi2, addrOps->invert(lo1), addrOps->boolean_(true), carries/*out*/);
    SValuePtr sf = addrOps->extract(diff, addr_nbits-1, addr_nbits);
    SValuePtr of = addrOps->xor_(addrOps->extract(carries, addr_nbits-1, addr_nbits),
                                 addrOps->extract(carries, addr_nbits-2, addr_nbits-1));
    SValuePtr cond1 = addrOps->invert(addrOps->xor_(sf, of));
    diff = addrOps->addWithCarries(hi1, addrOps->invert(lo2), addrOps->boolean_(true), carries/*out*/);
    sf = addrOps->extract(diff, addr_nbits-1, addr_nbits);
    of = addrOps->xor_(addrOps->extract(carries, addr_nbits-1, addr_nbits),
                       addrOps->extract(carries, addr_nbits-2, addr_nbits-1));
    SValuePtr cond2 = addrOps->invert(addrOps->xor_(sf, of));
    SValuePtr overlap = addrOps->and_(cond1, cond2);
    if (overlap->is_number() && overlap->get_number()!=0)
        return true;
    return false;
}

void
MemoryCell::print(std::ostream &stream, Formatter &fmt) const
{
    stream <<"addr=" <<(*address_+fmt);
    if (fmt.get_show_latest_writers() && latestWriter_)
        stream <<" writer=" <<StringUtility::addrToString(*latestWriter_);
    stream <<" value=" <<(*value_+fmt);
}

void
MemoryCellList::clearNonWritten() {
    for (CellList::iterator ci=cells.begin(); ci!=cells.end(); ++ci) {
        if (!(*ci)->latestWriter())
            *ci = MemoryCellPtr();
    }
    cells.erase(std::remove(cells.begin(), cells.end(), MemoryCellPtr()), cells.end());
}

SValuePtr
MemoryCellList::readMemory(const SValuePtr &addr, const SValuePtr &dflt, RiscOperators *addrOps, RiscOperators *valOps)
{
    ASSERT_not_null(addr);
    ASSERT_require(dflt!=NULL && (!byte_restricted || dflt->get_width()==8));
    bool short_circuited;
    CellList found = scan(addr, dflt->get_width(), addrOps, valOps, short_circuited/*out*/);
    size_t nfound = found.size();

    SValuePtr retval;
    if (1!=nfound || !short_circuited) {
        retval = dflt; // found no matches, multiple matches, or we fell off the end of the cell list
        cells.push_front(protocell->create(addr, dflt));
    } else {
        retval = found.front()->get_value();
        if (retval->get_width()!=dflt->get_width()) {
            ASSERT_require(!byte_restricted); // can't happen if memory state stores only byte values
            retval = valOps->unsignedExtend(retval, dflt->get_width()); // extend or truncate
        }
    }

    ASSERT_require(retval->get_width()==dflt->get_width());
    return retval;
}

std::set<rose_addr_t>
MemoryCellList::get_latest_writers(const SValuePtr &addr, size_t nbits, RiscOperators *addrOps, RiscOperators *valOps)
{
    ASSERT_not_null(addr);
    std::set<rose_addr_t> retval;
    bool short_circuited;
    CellList found = scan(addr, nbits, addrOps, valOps, short_circuited/*out*/);
    for (CellList::iterator fi=found.begin(); fi!=found.end(); ++fi) {
        MemoryCellPtr cell = *fi;
        if (cell->latestWriter())
            retval.insert(cell->latestWriter().get());
    }
    return retval;
}

void
MemoryCellList::writeMemory(const SValuePtr &addr, const SValuePtr &value, RiscOperators *addrOps, RiscOperators *valOps)
{
    ASSERT_not_null(addr);
    ASSERT_require(!byte_restricted || value->get_width()==8);
    MemoryCellPtr newCell = protocell->create(addr, value);

    // Prune away all cells that must-alias this new one since they will be occluded by this new one.
    if (occlusionsErased_) {
        for (CellList::iterator cli=cells.begin(); cli!=cells.end(); /*void*/) {
            MemoryCellPtr oldCell = *cli;
            if (newCell->must_alias(oldCell, addrOps)) {
                cli = cells.erase(cli);
            } else {
                ++cli;
            }
        }
    }

    // Insert the new cell
    cells.push_front(newCell);
    latest_written_cell = newCell;
}

void
MemoryCellList::print(std::ostream &stream, Formatter &fmt) const
{
    for (CellList::const_iterator ci=cells.begin(); ci!=cells.end(); ++ci)
        stream <<fmt.get_line_prefix() <<(**ci+fmt) <<"\n";
}

MemoryCellList::CellList
MemoryCellList::scan(const BaseSemantics::SValuePtr &addr, size_t nbits, RiscOperators *addrOps, RiscOperators *valOps,
                     bool &short_circuited/*out*/) const
{
    ASSERT_not_null(addr);
    short_circuited = false;
    CellList retval;
    MemoryCellPtr tmpcell = protocell->create(addr, valOps->undefined_(nbits));
    for (CellList::const_iterator ci=cells.begin(); ci!=cells.end(); ++ci) {
        if (tmpcell->may_alias(*ci, addrOps)) {
            retval.push_back(*ci);
            if ((short_circuited = tmpcell->must_alias(*ci, addrOps)))
                break;
        }
    }
    return retval;
}

void
MemoryCellList::traverse(Visitor &visitor)
{
    for (CellList::iterator ci=cells.begin(); ci!=cells.end(); ++ci)
        (visitor)(*ci);
}

/*******************************************************************************************************************************
 *                                      State
 *******************************************************************************************************************************/

void
State::print(std::ostream &stream, Formatter &fmt) const
{
    std::string prefix = fmt.get_line_prefix();
    Indent indent(fmt);
    stream <<prefix <<"registers:\n" <<(*registers+fmt) <<prefix <<"memory:\n" <<(*memory+fmt);
}

/*******************************************************************************************************************************
 *                                      RiscOperators
 *******************************************************************************************************************************/

void
RiscOperators::startInstruction(SgAsmInstruction *insn) {
    ASSERT_not_null(insn);
    SAWYER_MESG(mlog[TRACE]) <<"starting instruction " <<unparseInstructionWithAddress(insn) <<"\n";
    cur_insn = insn;
    ++ninsns;
};

SValuePtr
RiscOperators::subtract(const SValuePtr &minuend, const SValuePtr &subtrahend) {
    return add(minuend, negate(subtrahend));
}

SValuePtr
RiscOperators::equal(const SValuePtr &a, const SValuePtr &b) {
    return isEqual(a, b);
}

SValuePtr
RiscOperators::isEqual(const SValuePtr &a, const SValuePtr &b) {
    return equalToZero(xor_(a, b));
}

SValuePtr
RiscOperators::isNotEqual(const SValuePtr &a, const SValuePtr &b) {
    return invert(isEqual(a, b));
}

SValuePtr
RiscOperators::isUnsignedLessThan(const SValuePtr &a, const SValuePtr &b) {
    SValuePtr wideA = unsignedExtend(a, a->get_width()+1);
    SValuePtr wideB = unsignedExtend(b, b->get_width()+1);
    SValuePtr diff = subtract(wideA, wideB);
    return extract(diff, diff->get_width()-1, diff->get_width()); // A < B iff sign(wideA - wideB) == -1
}

SValuePtr
RiscOperators::isUnsignedLessThanOrEqual(const SValuePtr &a, const SValuePtr &b) {
    return or_(isUnsignedLessThan(a, b), isEqual(a, b));
}

SValuePtr
RiscOperators::isUnsignedGreaterThan(const SValuePtr &a, const SValuePtr &b) {
    return invert(isUnsignedLessThanOrEqual(a, b));
}

SValuePtr
RiscOperators::isUnsignedGreaterThanOrEqual(const SValuePtr &a, const SValuePtr &b) {
    return invert(isUnsignedLessThan(a, b));
}

SValuePtr
RiscOperators::isSignedLessThan(const SValuePtr &a, const SValuePtr &b) {
    ASSERT_require(a->get_width() == b->get_width());
    size_t nbits = a->get_width();
    SValuePtr aIsNeg = extract(a, nbits-1, nbits);
    SValuePtr bIsNeg = extract(b, nbits-1, nbits);
    SValuePtr diff = subtract(signExtend(a, nbits+1), signExtend(b, nbits+1));
    SValuePtr diffIsNeg = extract(diff, nbits, nbits+1); // sign bit
    SValuePtr negPos = and_(aIsNeg, invert(bIsNeg));     // A is negative and B is non-negative?
    SValuePtr sameSigns = invert(xor_(aIsNeg, bIsNeg));  // A and B are both negative or both non-negative?
    SValuePtr result = or_(negPos, and_(sameSigns, diffIsNeg));
    return result;
}

SValuePtr
RiscOperators::isSignedLessThanOrEqual(const SValuePtr &a, const SValuePtr &b) {
    return or_(isSignedLessThan(a, b), isEqual(a, b));
}

SValuePtr
RiscOperators::isSignedGreaterThan(const SValuePtr &a, const SValuePtr &b) {
    return invert(isSignedLessThanOrEqual(a, b));
}

SValuePtr
RiscOperators::isSignedGreaterThanOrEqual(const SValuePtr &a, const SValuePtr &b) {
    return invert(isSignedLessThan(a, b));
}

SValuePtr
RiscOperators::fpFromInteger(const SValuePtr &intValue, SgAsmFloatType *fpType) {
    ASSERT_not_null(fpType);
    throw NotImplemented("fpFromInteger is not implemented", get_insn());
}

SValuePtr
RiscOperators::fpToInteger(const SValuePtr &fpValue, SgAsmFloatType *fpType, const SValuePtr &dflt) {
    ASSERT_not_null(fpType);
    throw NotImplemented("fpToInteger is not implemented", get_insn());
}

SValuePtr
RiscOperators::fpConvert(const SValuePtr &a, SgAsmFloatType *aType, SgAsmFloatType *retType) {
    ASSERT_not_null(a);
    ASSERT_not_null(aType);
    ASSERT_not_null(retType);
    if (aType == retType)
        return a->copy();
    throw NotImplemented("fpConvert is not implemented", get_insn());
}

SValuePtr
RiscOperators::fpIsNan(const SValuePtr &a, SgAsmFloatType *aType) {
    // Value is NAN iff exponent bits are all set and significand is not zero.
    ASSERT_not_null(a);
    ASSERT_not_null(aType);
    SValuePtr exponent = extract(a, aType->exponentBits().least(), aType->exponentBits().greatest()+1);
    SValuePtr significand = extract(a, aType->significandBits().least(), aType->significandBits().greatest()+1);
    return and_(equalToZero(invert(exponent)), invert(equalToZero(significand)));
}

SValuePtr
RiscOperators::fpIsDenormalized(const SValuePtr &a, SgAsmFloatType *aType) {
    // Value is denormalized iff exponent is zero and significand is not zero.
    ASSERT_not_null(a);
    ASSERT_not_null(aType);
    if (!aType->gradualUnderflow())
        return boolean_(false);
    SValuePtr exponent = extract(a, aType->exponentBits().least(), aType->exponentBits().greatest()+1);
    SValuePtr significand = extract(a, aType->significandBits().least(), aType->significandBits().greatest()+1);
    return and_(equalToZero(exponent), invert(equalToZero(significand)));
}

SValuePtr
RiscOperators::fpIsZero(const SValuePtr &a, SgAsmFloatType *aType) {
    // Value is zero iff exponent and significand are both zero.
    ASSERT_not_null(a);
    ASSERT_not_null(aType);
    SValuePtr exponent = extract(a, aType->exponentBits().least(), aType->exponentBits().greatest()+1);
    SValuePtr significand = extract(a, aType->significandBits().least(), aType->significandBits().greatest()+1);
    return and_(equalToZero(exponent), equalToZero(significand));
}

SValuePtr
RiscOperators::fpIsInfinity(const SValuePtr &a, SgAsmFloatType *aType) {
    // Value is infinity iff exponent bits are all set and significand is zero.
    ASSERT_not_null(a);
    ASSERT_not_null(aType);
    SValuePtr exponent = extract(a, aType->exponentBits().least(), aType->exponentBits().greatest()+1);
    SValuePtr significand = extract(a, aType->significandBits().least(), aType->significandBits().greatest()+1);
    return and_(equalToZero(invert(exponent)), equalToZero(significand));
}

SValuePtr
RiscOperators::fpSign(const SValuePtr &a, SgAsmFloatType *aType) {
    ASSERT_not_null(a);
    ASSERT_not_null(aType);
    return extract(a, aType->signBit(), aType->signBit()+1);
}

SValuePtr
RiscOperators::fpEffectiveExponent(const SValuePtr &a, SgAsmFloatType *aType) {
    ASSERT_not_null(a);
    ASSERT_not_null(aType);
    size_t expWidth = aType->exponentBits().size() + 1; // add a sign bit to the beginning
    SValuePtr storedExponent = extract(a, aType->exponentBits().least(), aType->exponentBits().greatest()+1);
    SValuePtr significand = extract(a, aType->significandBits().least(), aType->significandBits().greatest()+1);
    SValuePtr retval = ite(equalToZero(storedExponent),
                           ite(equalToZero(significand),
                               // Stored exponent and significand are both zero, therefore value is zero
                               number_(expWidth, 0),    // value is zero, therefore exponent is zero

                               // Stored exponent is zero but significand is not, therefore denormalized number.
                               // effective exponent is 1 - bias - (significandWidth - mssb(significand))
                               add(number_(expWidth, 1 - aType->exponentBias() - aType->significandBits().size()),
                                   unsignedExtend(mostSignificantSetBit(significand), expWidth))),

                           // Stored exponent is non-zero so significand is normalized. Effective exponent is the stored
                           // exponent minus the bias.
                           subtract(unsignedExtend(storedExponent, expWidth),
                                    number_(expWidth, aType->exponentBias())));
    return retval;
}

SValuePtr
RiscOperators::fpAdd(const SValuePtr &a, const SValuePtr &b, SgAsmFloatType *fpType) {
    throw NotImplemented("fpAdd is not implemented", get_insn());
}

SValuePtr
RiscOperators::fpSubtract(const SValuePtr &a, const SValuePtr &b, SgAsmFloatType *fpType) {
    throw NotImplemented("fpSubtract is not implemented", get_insn());
}

SValuePtr
RiscOperators::fpMultiply(const SValuePtr &a, const SValuePtr &b, SgAsmFloatType *fpType) {
    throw NotImplemented("fpMultiply is not implemented", get_insn());
}

SValuePtr
RiscOperators::fpDivide(const SValuePtr &a, const SValuePtr &b, SgAsmFloatType *fpType) {
    throw NotImplemented("fpDivide is not implemented", get_insn());
}

SValuePtr
RiscOperators::fpSquareRoot(const SValuePtr &a, SgAsmFloatType *aType) {
    throw NotImplemented("fpSquareRoot is not implemented", get_insn());
}

SValuePtr
RiscOperators::fpRoundTowardZero(const SValuePtr &a, SgAsmFloatType *aType) {
    throw NotImplemented("fpRoundTowardZero is not implemented", get_insn());
}


/*******************************************************************************************************************************
 *                                      Dispatcher
 *******************************************************************************************************************************/

void
Dispatcher::advanceInstructionPointer(SgAsmInstruction *insn) {
    RegisterDescriptor ipReg = instructionPointerRegister();
    size_t nBits = ipReg.get_nbits();
    BaseSemantics::SValuePtr ipValue;
    if (!autoResetInstructionPointer_ && operators->get_state() && operators->get_state()->get_register_state()) {
        BaseSemantics::RegisterStateGenericPtr grState =
            boost::dynamic_pointer_cast<BaseSemantics::RegisterStateGeneric>(operators->get_state()->get_register_state());
        if (grState && grState->is_partly_stored(ipReg))
            ipValue = operators->readRegister(ipReg);
    }
    if (!ipValue)
        ipValue = operators->number_(nBits, insn->get_address());
    ipValue = operators->add(ipValue, operators->number_(nBits, insn->get_size()));
    operators->writeRegister(ipReg, ipValue);
}

void
Dispatcher::addressWidth(size_t nBits) {
    ASSERT_require2(nBits==addrWidth_ || addrWidth_==0, "address width cannot be changed once it is set");
    addrWidth_ = nBits;
}

void
Dispatcher::processInstruction(SgAsmInstruction *insn)
{
    operators->startInstruction(insn);
    InsnProcessor *iproc = iproc_lookup(insn);
    try {
        if (!iproc)
            throw Exception("no dispatch ability for instruction", insn);
        iproc->process(shared_from_this(), insn);
    } catch (Exception &e) {
        // If the exception was thrown by something that didn't have an instruction available, then add the instruction
        if (!e.insn)
            e.insn = insn;
        throw e;
    }
    operators->finishInstruction(insn);
}

InsnProcessor *
Dispatcher::iproc_lookup(SgAsmInstruction *insn)
{
    int key = iproc_key(insn);
    ASSERT_require(key>=0);
    return iproc_get(key);
}

void
Dispatcher::iproc_replace(SgAsmInstruction *insn, InsnProcessor *iproc)
{
    iproc_set(iproc_key(insn), iproc);
}

void
Dispatcher::iproc_set(int key, InsnProcessor *iproc)
{
    ASSERT_require(key>=0);
    if ((size_t)key>=iproc_table.size())
        iproc_table.resize(key+1, NULL);
    iproc_table[key] = iproc;
}
    
InsnProcessor *
Dispatcher::iproc_get(int key)
{
    if (key<0 || (size_t)key>=iproc_table.size())
        return NULL;
    return iproc_table[key];
}

const RegisterDescriptor &
Dispatcher::findRegister(const std::string &regname, size_t nbits/*=0*/, bool allowMissing) const
{
    const RegisterDictionary *regdict = get_register_dictionary();
    if (!regdict)
        throw Exception("no register dictionary", get_insn());

    const RegisterDescriptor *reg = regdict->lookup(regname);
    if (!reg) {
        if (allowMissing) {
            static RegisterDescriptor invalidRegister;
            return invalidRegister;
        }
        std::ostringstream ss;
        ss <<"Invalid register \"" <<regname <<"\" in dictionary \"" <<regdict->get_architecture_name() <<"\"";
        throw Exception(ss.str(), get_insn());
    }

    if (nbits>0 && reg->get_nbits()!=nbits) {
        std::ostringstream ss;
        ss <<"Invalid " <<nbits <<"-bit register: \"" <<regname <<"\" is "
           <<reg->get_nbits() <<" " <<(1==reg->get_nbits()?"byte":"bytes");
        throw Exception(ss.str(), get_insn());
    }
    return *reg;
}

void
Dispatcher::decrementRegisters(SgAsmExpression *e)
{
    struct T1: AstSimpleProcessing {
        RiscOperatorsPtr ops;
        T1(const RiscOperatorsPtr &ops): ops(ops) {}
        void visit(SgNode *node) {
            if (SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(node)) {
                const RegisterDescriptor &reg = rre->get_descriptor();
                if (rre->get_adjustment() < 0) {
                    SValuePtr adj = ops->number_(64, (int64_t)rre->get_adjustment());
                    if (reg.get_nbits() <= 64) {
                        adj = ops->unsignedExtend(adj, reg.get_nbits());  // truncate
                    } else {
                        adj = ops->signExtend(adj, reg.get_nbits());      // extend
                    }
                    ops->writeRegister(reg, ops->add(ops->readRegister(reg), adj));
                }
            }
        }
    } t1(operators);
    t1.traverse(e, preorder);
}

void
Dispatcher::incrementRegisters(SgAsmExpression *e)
{
    struct T1: AstSimpleProcessing {
        RiscOperatorsPtr ops;
        T1(const RiscOperatorsPtr &ops): ops(ops) {}
        void visit(SgNode *node) {
            if (SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(node)) {
                const RegisterDescriptor &reg = rre->get_descriptor();
                if (rre->get_adjustment() > 0) {
                    SValuePtr adj = ops->unsignedExtend(ops->number_(64, (int64_t)rre->get_adjustment()), reg.get_nbits());
                    ops->writeRegister(reg, ops->add(ops->readRegister(reg), adj));
                }
            }
        }
    } t1(operators);
    t1.traverse(e, preorder);
}

SValuePtr
Dispatcher::effectiveAddress(SgAsmExpression *e, size_t nbits/*=0*/)
{
    BaseSemantics::SValuePtr retval;
#if 1 // DEBUGGING [Robb P. Matzke 2015-08-04]
    ASSERT_always_require(retval==NULL);
#endif
    if (0==nbits)
        nbits = addressWidth();

    if (SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(e)) {
        retval = effectiveAddress(mre->get_address(), nbits);
    } else if (SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(e)) {
        const RegisterDescriptor &reg = rre->get_descriptor();
        retval = operators->readRegister(reg);
    } else if (SgAsmBinaryAdd *op = isSgAsmBinaryAdd(e)) {
        BaseSemantics::SValuePtr lhs = effectiveAddress(op->get_lhs(), nbits);
        BaseSemantics::SValuePtr rhs = effectiveAddress(op->get_rhs(), nbits);
        retval = operators->add(lhs, rhs);
    } else if (SgAsmBinaryMultiply *op = isSgAsmBinaryMultiply(e)) {
        BaseSemantics::SValuePtr lhs = effectiveAddress(op->get_lhs(), nbits);
        BaseSemantics::SValuePtr rhs = effectiveAddress(op->get_rhs(), nbits);
        retval = operators->unsignedMultiply(lhs, rhs);
    } else if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(e)) {
        retval = operators->number_(ival->get_significantBits(), ival->get_value());
    }

    ASSERT_not_null(retval);
    if (retval->get_width() < nbits) {
        retval = operators->signExtend(retval, nbits);
    } else if (retval->get_width() > nbits) {
        retval = operators->extract(retval, 0, nbits);
    }
    return retval;
}

RegisterDescriptor
Dispatcher::segmentRegister(SgAsmMemoryReferenceExpression *mre)
{
    if (mre!=NULL && mre->get_segment()!=NULL) {
        if (SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(mre->get_segment())) {
            return rre->get_descriptor();
        }
    }
    return RegisterDescriptor();
}

void
Dispatcher::write(SgAsmExpression *e, const SValuePtr &value, size_t addr_nbits/*=0*/)
{
    ASSERT_not_null(e);
    ASSERT_not_null(value);
    if (SgAsmDirectRegisterExpression *re = isSgAsmDirectRegisterExpression(e)) {
        operators->writeRegister(re->get_descriptor(), value);
    } else if (SgAsmIndirectRegisterExpression *re = isSgAsmIndirectRegisterExpression(e)) {
        SValuePtr offset = operators->readRegister(re->get_offset());
        if (!offset->is_number()) {
            std::string offset_name = get_register_dictionary()->lookup(re->get_offset());
            offset_name = offset_name.empty() ? "" : "(" + offset_name + ") ";
            throw Exception("indirect register offset " + offset_name + "must have a concrete value", NULL);
        }
        size_t idx = (offset->get_number() + re->get_index()) % re->get_modulus();
        RegisterDescriptor reg = re->get_descriptor();
        reg.set_major(reg.get_major() + re->get_stride().get_major() * idx);
        reg.set_minor(reg.get_minor() + re->get_stride().get_minor() * idx);
        reg.set_offset(reg.get_offset() + re->get_stride().get_offset() * idx);
        reg.set_nbits(reg.get_nbits() + re->get_stride().get_nbits() * idx);
        operators->writeRegister(reg, value);
    } else if (SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(e)) {
        SValuePtr addr = effectiveAddress(mre, addr_nbits);
        ASSERT_require(0==addrWidth_ || addr->get_width()==addrWidth_);
        operators->writeMemory(segmentRegister(mre), addr, value, operators->boolean_(true));
    } else {
        ASSERT_not_implemented("[Robb P. Matzke 2014-10-07]");
    }
}

SValuePtr
Dispatcher::read(SgAsmExpression *e, size_t value_nbits/*=0*/, size_t addr_nbits/*=0*/)
{
    ASSERT_not_null(e);
    if (0 == value_nbits) {
        SgAsmType *expr_type = e->get_type();
        ASSERT_not_null(expr_type);
        value_nbits = expr_type->get_nBits();
        ASSERT_require(value_nbits != 0);
    }

    SValuePtr retval;
    if (SgAsmDirectRegisterExpression *re = isSgAsmDirectRegisterExpression(e)) {
        retval = operators->readRegister(re->get_descriptor());
    } else if (SgAsmIndirectRegisterExpression *re = isSgAsmIndirectRegisterExpression(e)) {
        SValuePtr offset = operators->readRegister(re->get_offset());
        if (!offset->is_number()) {
            std::string offset_name = get_register_dictionary()->lookup(re->get_offset());
            offset_name = offset_name.empty() ? "" : "(" + offset_name + ") ";
            throw Exception("indirect register offset " + offset_name + "must have a concrete value", NULL);
        }
        size_t idx = (offset->get_number() + re->get_index()) % re->get_modulus();
        RegisterDescriptor reg = re->get_descriptor();
        reg.set_major(reg.get_major() + re->get_stride().get_major() * idx);
        reg.set_minor(reg.get_minor() + re->get_stride().get_minor() * idx);
        reg.set_offset(reg.get_offset() + re->get_stride().get_offset() * idx);
        reg.set_nbits(reg.get_nbits() + re->get_stride().get_nbits() * idx);
        retval = operators->readRegister(reg);
    } else if (SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(e)) {
        BaseSemantics::SValuePtr addr = effectiveAddress(mre, addr_nbits);
        ASSERT_require(0==addrWidth_ || addr->get_width()==addrWidth_);
        BaseSemantics::SValuePtr dflt = undefined_(value_nbits);
        retval = operators->readMemory(segmentRegister(mre), addr, dflt, operators->boolean_(true));
    } else if (SgAsmValueExpression *ve = isSgAsmValueExpression(e)) {
        uint64_t val = SageInterface::getAsmSignedConstant(ve);
        retval = operators->number_(value_nbits, val);
    } else if (SgAsmBinaryAdd *sum = isSgAsmBinaryAdd(e)) {
        SgAsmExpression *lhs = sum->get_lhs();
        SgAsmExpression *rhs = sum->get_rhs();
        size_t nbits = std::max(lhs->get_nBits(), rhs->get_nBits());
        retval = operators->add(operators->signExtend(read(lhs, lhs->get_nBits(), addr_nbits), nbits),
                                operators->signExtend(read(rhs, rhs->get_nBits(), addr_nbits), nbits));
    } else if (SgAsmBinaryMultiply *product = isSgAsmBinaryMultiply(e)) {
        SgAsmExpression *lhs = product->get_lhs();
        SgAsmExpression *rhs = product->get_rhs();
        retval = operators->unsignedMultiply(read(lhs, lhs->get_nBits()), read(rhs, rhs->get_nBits()));
    } else {
        ASSERT_not_implemented(e->class_name());
    }

    // Make sure the return value is the requested width. The unsignedExtend() can expand or shrink values.
    ASSERT_not_null(retval);
    if (retval->get_width()!=value_nbits)
        retval = operators->unsignedExtend(retval, value_nbits);
    return retval;
}
    
} // namespace
} // namespace
} // namespace
} // namespace
