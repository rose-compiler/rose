#include "sage3basic.h"
#include "BaseSemantics2.h"
#include "AsmUnparser_compat.h"

namespace BinaryAnalysis {
namespace InstructionSemantics2 {
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
    o <<"BinaryAnalysis::InstructionSemantics::BaseSemantics::Exception: " <<what();
    if (insn)
        o <<": " <<unparseInstructionWithAddress(insn);
    o <<"\n";
}

/*******************************************************************************************************************************
 *                                      Semantic Values
 *******************************************************************************************************************************/

Allocator SValue::allocator;

/*******************************************************************************************************************************
 *                                      RegisterStateGeneric
 *******************************************************************************************************************************/

void
RegisterStateGeneric::clear()
{
    registers.clear();
    clear_latest_writers();
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
    Extent need_extent(reg.get_offset(), reg.get_nbits());
    std::vector<SValuePtr> overlaps(reg.get_nbits()); // overlapping parts of other registers by bit offset
    std::vector<RegPair> nonoverlaps; // non-overlapping parts of overlapping registers
    for (RegPairs::iterator rvi=ri->second.begin(); rvi!=ri->second.end(); ++rvi) {
        Extent have_extent(rvi->desc.get_offset(), rvi->desc.get_nbits());
        if (need_extent==have_extent) {
            // exact match; no need to traverse further because a RegisterStateGeneric never stores overlapping values
            assert(nonoverlaps.empty());
            return rvi->value;
        } else {
            Extent overlap = need_extent.intersect(have_extent);
            if (overlap==need_extent) {
                // The stored register contains all the bits we need to read, and then some.  Extract only what we need.
                // No need to traverse further because we never store overlapping values.
                assert(nonoverlaps.empty());
                size_t lo_bit = need_extent.first() - have_extent.first();
                return ops->extract(rvi->value, lo_bit, lo_bit+need_extent.size());
            } else if (!overlap.empty()) {
                SValuePtr overlap_val = ops->extract(rvi->value, overlap.first()-have_extent.first(), overlap.size());
                overlaps[overlap.first()] = overlap_val;

                // If any part of the existing register is not represented by the register being read, then we need to save
                // that part.
                get_nonoverlapping_parts(overlap, *rvi, ops, &nonoverlaps);

                // Mark the overlapping register by setting it to null so we can remove it later.
                rvi->value = SValuePtr();
            }
        }
    }

    // Remove the overlapping registers that we marked above, and replace them with their non-overlapping parts.
    ri->second.erase(std::remove_if(ri->second.begin(), ri->second.end(), has_null_value), ri->second.end());
    ri->second.insert(ri->second.end(), nonoverlaps.begin(), nonoverlaps.end());

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
        }
        retval = retval==NULL ? part : ops->concat(retval, part);
        offset += part->get_width();
    }

    // Insert the return value; it does not overlap with any of the other parts.
    ri->second.push_back(RegPair(reg, retval));
    return retval;
}

void
RegisterStateGeneric::writeRegister(const RegisterDescriptor &reg, const SValuePtr &value, RiscOperators *ops)
{
    // Fast case: the state does not store this register or any register that might overlap with this register
    Registers::iterator ri = registers.find(reg);
    if (ri==registers.end()) {
        registers[reg].push_back(RegPair(reg, value));
        return;
    }

    // Look for existing registers that overlap with this register and remove them.  If the overlap was only partial, then we
    // need to eventually add the non-overlapping part back into the list.
    RegPairs nonoverlaps; // the non-overlapping parts of overlapping registers
    Extent need_extent(reg.get_offset(), reg.get_nbits());
    for (RegPairs::iterator rvi=ri->second.begin(); rvi!=ri->second.end(); ++rvi) {
        Extent have_extent(rvi->desc.get_offset(), rvi->desc.get_nbits());
        if (need_extent==have_extent) {
            // found exact match. No need to traverse further because a RegisterStateGeneric never stores overlapping values.
            assert(nonoverlaps.empty());
            rvi->value = value;
            return;
        } else {
            Extent overlap = need_extent.intersect(have_extent);
            if (!overlap.empty()) {
                get_nonoverlapping_parts(overlap, *rvi, ops, &nonoverlaps);
                rvi->value = SValuePtr(); // mark pair for removal by setting it to null
            }
        }
    }

    // Remove marked pairs, then add the non-overlapping parts.
    ri->second.erase(std::remove_if(ri->second.begin(), ri->second.end(), has_null_value), ri->second.end());
    ri->second.insert(ri->second.end(), nonoverlaps.begin(), nonoverlaps.end());

    // Insert the new value.
    ri->second.push_back(RegPair(reg, value));
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
                assert(newval->get_width()==rpi->desc.get_nbits());
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
RegisterStateGeneric::set_latest_writer(const RegisterDescriptor &desc, rose_addr_t writer_va)
{
    WrittenParts &parts = writers[desc];
    parts.insert(Extent(desc.get_offset(), desc.get_nbits()), writer_va);
}

void
RegisterStateGeneric::clear_latest_writer(const RegisterDescriptor &desc)
{
    WritersMap::iterator wi = writers.find(desc);
    if (wi!=writers.end())
        wi->second.clear();
}

void
RegisterStateGeneric::clear_latest_writers()
{
    writers.clear();
}

std::set<rose_addr_t>
RegisterStateGeneric::get_latest_writers(const RegisterDescriptor &desc) const
{
    std::set<rose_addr_t> retval;
    WritersMap::const_iterator wi = writers.find(desc);
    if (wi!=writers.end()) {
        Extent desc_extent(desc.get_offset(), desc.get_nbits());
        for (WrittenParts::const_iterator pi=wi->second.begin(); pi!=wi->second.end(); ++pi) {
            if (pi->first.overlaps(desc_extent))
                retval.insert(pi->second.get());
        }
    }
    return retval;
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
            for (RegPairs::const_iterator rvi=ri->second.begin(); rvi!=ri->second.end(); ++rvi) {
                std::string regname = regnames(rvi->desc);
                if (!fmt.get_suppress_initial_values() || rvi->value->get_comment().empty() ||
                    0!=rvi->value->get_comment().compare(regname+"_0")) {
                    if (0==i) {
                        maxlen = std::max(maxlen, regname.size());
                    } else {
                        stream <<fmt.get_line_prefix() <<std::setw(maxlen) <<std::left <<regname;
                        oflags.restore();
                        if (fmt.get_show_latest_writers()) {
                            std::set<rose_addr_t> writers = get_latest_writers(rvi->desc);
                            if (writers.size()==1) {
                                stream <<" [writer=" <<StringUtility::addrToString(*writers.begin()) <<"]";
                            } else if (!writers.empty()) {
                                stream <<" [writers={";
                                for (std::set<rose_addr_t>::iterator wi=writers.begin(); wi!=writers.end(); ++wi)
                                    stream <<(wi==writers.begin()?"":", ") <<StringUtility::addrToString(*wi);
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

void
RegisterStateX86::clear()
{
    ip = protoval->undefined_(32);
    for (size_t i=0; i<n_gprs; ++i)
        gpr[i] = protoval->undefined_(32);
    for (size_t i=0; i<n_segregs; ++i)
        segreg[i] = protoval->undefined_(16);
    for (size_t i=0; i<n_flags; ++i)
        flag[i] = protoval->undefined_(1);
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
}

SValuePtr
RegisterStateX86::readRegister(const RegisterDescriptor &reg, RiscOperators *ops)
{
    switch (reg.get_major()) {
        case x86_regclass_gpr:
            return readRegisterGpr(reg, ops);
        case x86_regclass_flags:
            return readRegisterFlag(reg, ops);
        case x86_regclass_segment:
            return readRegisterSeg(reg, ops);
        case x86_regclass_ip:
            return readRegisterIp(reg, ops);
        default:
            throw Exception("invalid register major number: "+StringUtility::numberToString(reg.get_major())+
                            " (wrong RegisterDictionary?)", ops->get_insn());
    }
}

SValuePtr
RegisterStateX86::readRegisterGpr(const RegisterDescriptor &reg, RiscOperators *ops)
{
    using namespace StringUtility;
    assert(ops!=NULL);
    assert(reg.get_major()==x86_regclass_gpr);
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
    assert(result->get_width()==reg.get_nbits());
    return result;
}

SValuePtr
RegisterStateX86::readRegisterFlag(const RegisterDescriptor &reg, RiscOperators *ops)
{
    using namespace StringUtility;
    assert(ops!=NULL);
    assert(reg.get_major()==x86_regclass_flags);
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
    assert(result->get_width()==reg.get_nbits());
    return result;
}

SValuePtr
RegisterStateX86::readRegisterSeg(const RegisterDescriptor &reg, RiscOperators *ops)
{
    using namespace StringUtility;
    assert(ops!=NULL);
    assert(reg.get_major()==x86_regclass_segment);
    if (reg.get_minor()>=n_segregs)
        throw Exception("invalid segment register minor number: "+numberToString(reg.get_minor()), ops->get_insn());
    if (reg.get_offset()!=0 || reg.get_nbits()!=16)
        throw Exception("invalid segment subregister: offset="+numberToString(reg.get_offset())+
                        " width="+numberToString(reg.get_nbits()), ops->get_insn());
    SValuePtr result = segreg[reg.get_minor()];
    assert(result->get_width()==reg.get_nbits());
    return result;
}

SValuePtr
RegisterStateX86::readRegisterIp(const RegisterDescriptor &reg, RiscOperators *ops)
{
    using namespace StringUtility;
    assert(ops!=NULL);
    assert(reg.get_major()==x86_regclass_ip);
    if (reg.get_minor()!=0)
        throw Exception("invalid instruction pointer register minor number: "+numberToString(reg.get_minor()), ops->get_insn());
    if (reg.get_offset()!=0 || reg.get_nbits()!=32)
        throw Exception("invalid instruction pointer subregister: offset="+numberToString(reg.get_offset())+
                        " width="+numberToString(reg.get_nbits()), ops->get_insn());
    assert(ip->get_width()==reg.get_nbits());
    return ip;
}

void
RegisterStateX86::writeRegister(const RegisterDescriptor &reg, const SValuePtr &value, RiscOperators *ops)
{
    switch (reg.get_major()) {
        case x86_regclass_gpr:
            return writeRegisterGpr(reg, value, ops);
        case x86_regclass_flags:
            return writeRegisterFlag(reg, value, ops);
        case x86_regclass_segment:
            return writeRegisterSeg(reg, value, ops);
        case x86_regclass_ip:
            return writeRegisterIp(reg, value, ops);
        default:
            throw Exception("invalid register major number: "+StringUtility::numberToString(reg.get_major())+
                            " (wrong RegisterDictionary?)", ops->get_insn());
    }
}

void
RegisterStateX86::writeRegisterGpr(const RegisterDescriptor &reg, const SValuePtr &value, RiscOperators *ops)
{
    using namespace StringUtility;
    assert(reg.get_major()==x86_regclass_gpr);
    assert(value!=NULL && value->get_width()==reg.get_nbits());
    assert(ops!=NULL);
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
    assert(towrite->get_width()==32);
    gpr[reg.get_minor()] = towrite;
}

void
RegisterStateX86::writeRegisterFlag(const RegisterDescriptor &reg, const SValuePtr &value, RiscOperators *ops)
{
    using namespace StringUtility;
    assert(reg.get_major()==x86_regclass_flags);
    assert(value!=NULL && value->get_width()==reg.get_nbits());
    assert(ops!=NULL);
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
    assert(reg.get_major()==x86_regclass_segment);
    assert(value!=NULL && value->get_width()==reg.get_nbits());
    assert(ops!=NULL);
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
    assert(reg.get_major()==x86_regclass_ip);
    assert(value!=NULL && value->get_width()==reg.get_nbits());
    assert(ops!=NULL);
    if (reg.get_minor()!=0)
        throw Exception("invalid instruction pointer register minor number: "+numberToString(reg.get_minor()), ops->get_insn());
    if (reg.get_offset()!=0 || reg.get_nbits()!=32)
        throw Exception("invalid instruction pointer subregister: offset="+numberToString(reg.get_offset())+
                        " width="+numberToString(reg.get_nbits()), ops->get_insn());
    ip = value;
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
    if (8==value->get_width() && 8==other->get_value()->get_width())
        return address->may_equal(other->get_address(), addrOps->get_solver());

    size_t addr_nbits = address->get_width();
    assert(other->get_address()->get_width()==addr_nbits);

    assert(value->get_width() % 8 == 0);                // memory is byte addressable, so values must be multiples of a byte
    SValuePtr lo1 = address;
    SValuePtr hi1 = addrOps->add(lo1, addrOps->number_(lo1->get_width(), value->get_width() / 8));

    assert(other->get_value()->get_width() % 8 == 0);
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
    if (8==value->get_width() && 8==other->get_value()->get_width())
        return address->must_equal(other->get_address(), addrOps->get_solver());

    size_t addr_nbits = address->get_width();
    assert(other->get_address()->get_width()==addr_nbits);

    assert(value->get_width() % 8 == 0);
    SValuePtr lo1 = address;
    SValuePtr hi1 = addrOps->add(lo1, addrOps->number_(lo1->get_width(), value->get_width() / 8));

    assert(other->get_value()->get_width() % 8 == 0);
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
    stream <<"addr=" <<(*address+fmt);
    if (fmt.get_show_latest_writers() && latest_writer)
        stream <<" writer=" <<StringUtility::addrToString(*latest_writer);
    stream <<" value=" <<(*value+fmt);
}

SValuePtr
MemoryCellList::readMemory(const SValuePtr &addr, const SValuePtr &dflt, size_t nbits,
                           RiscOperators *addrOps, RiscOperators *valOps)
{
    assert(addr!=NULL);
    assert(dflt!=NULL && (!byte_restricted || dflt->get_width()==8));
    bool short_circuited;
    CellList found = scan(addr, nbits, addrOps, valOps, short_circuited/*out*/);
    size_t nfound = found.size();

    SValuePtr retval;
    if (1!=nfound || !short_circuited) {
        retval = dflt; // found no matches, multiple matches, or we fell off the end of the cell list
        cells.push_front(protocell->create(addr, dflt));
    } else {
        retval = found.front()->get_value();
        if (retval->get_width()!=nbits) {
            assert(!byte_restricted); // can't happen if memory state stores only byte values
            retval = valOps->unsignedExtend(retval, nbits); // extend or truncate
        }
    }

    assert(retval->get_width()==nbits);
    return retval;
}

std::set<rose_addr_t>
MemoryCellList::get_latest_writers(const SValuePtr &addr, size_t nbits, RiscOperators *addrOps, RiscOperators *valOps)
{
    assert(addr!=NULL);
    std::set<rose_addr_t> retval;
    bool short_circuited;
    CellList found = scan(addr, nbits, addrOps, valOps, short_circuited/*out*/);
    for (CellList::iterator fi=found.begin(); fi!=found.end(); ++fi) {
        MemoryCellPtr cell = *fi;
        if (cell->get_latest_writer())
            retval.insert(cell->get_latest_writer().get());
    }
    return retval;
}

void
MemoryCellList::writeMemory(const SValuePtr &addr, const SValuePtr &value, RiscOperators *addrOps, RiscOperators *valOps)
{
    assert(!byte_restricted || value->get_width()==8);
    MemoryCellPtr cell = protocell->create(addr, value);
    cells.push_front(cell);
    latest_written_cell = cell;
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


/*******************************************************************************************************************************
 *                                      Dispatcher
 *******************************************************************************************************************************/

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
#if 1 /*DEBUGGING [Robb P. Matzke 2014-01-15]*/
        if (e.insn) {
            std::string what = StringUtility::trim(e.what());
            std::string insn_s = StringUtility::addrToString(e.insn->get_address()) + ": " + unparseInstruction(e.insn);
            if (what.empty()) {
                what = insn_s;
            } else {
                what += " (" + insn_s + ")";
            }
            throw Exception(what, e.insn);
        }
#endif
        throw e;
    }
    operators->finishInstruction(insn);
}

InsnProcessor *
Dispatcher::iproc_lookup(SgAsmInstruction *insn)
{
    int key = iproc_key(insn);
    assert(key>=0);
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
    assert(key>=0);
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
Dispatcher::findRegister(const std::string &regname, size_t nbits/*=0*/)
{
    const RegisterDictionary *regdict = get_register_dictionary();
    if (!regdict)
        throw Exception("no register dictionary", get_insn());

    const RegisterDescriptor *reg = regdict->lookup(regname);
    if (!reg) {
        std::ostringstream ss;
        ss <<"Invalid register: \"" <<regname <<"\"";
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

SValuePtr
Dispatcher::effectiveAddress(SgAsmExpression *e, size_t nbits/*=0*/)
{
    BaseSemantics::SValuePtr retval;
    if (SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(e)) {
        retval = effectiveAddress(mre->get_address(), nbits);
    } else if (SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(e)) {
        retval = operators->readRegister(rre->get_descriptor());
    } else if (SgAsmBinaryAdd *op = isSgAsmBinaryAdd(e)) {
        BaseSemantics::SValuePtr lhs = effectiveAddress(op->get_lhs(), nbits);
        BaseSemantics::SValuePtr rhs = effectiveAddress(op->get_rhs(), nbits);
        if (0==nbits)
            nbits = std::max(lhs->get_width(), rhs->get_width());
        if (lhs->get_width() < nbits)
            lhs = operators->signExtend(lhs, nbits);
        if (rhs->get_width() < nbits)
            rhs = operators->signExtend(rhs, nbits);
        retval = operators->add(lhs, rhs);
    } else if (SgAsmBinaryMultiply *op = isSgAsmBinaryMultiply(e)) {
        BaseSemantics::SValuePtr lhs = effectiveAddress(op->get_lhs(), nbits);
        BaseSemantics::SValuePtr rhs = effectiveAddress(op->get_rhs(), nbits);
        retval = operators->unsignedMultiply(lhs, rhs);
    } else if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(e)) {
        retval = operators->number_(ival->get_significant_bits(), ival->get_value());
    }

    assert(retval!=NULL);
    if (nbits!=0) {
        if (retval->get_width() < nbits) {
            retval = operators->signExtend(retval, nbits);
        } else if (retval->get_width() > nbits) {
            retval = operators->extract(retval, 0, nbits);
        }
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
    assert(e!=NULL && value!=NULL);
    if (SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(e)) {
        operators->writeRegister(rre->get_descriptor(), value);
    } else if (SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(e)) {
        SValuePtr addr = effectiveAddress(mre, addr_nbits);
        operators->writeMemory(segmentRegister(mre), addr, value, operators->boolean_(true));
    } else {
        assert(!"not implemented");
        abort();
    }
}

SValuePtr
Dispatcher::read(SgAsmExpression *e, size_t value_nbits, size_t addr_nbits/*=0*/)
{
    assert(e!=NULL);
    BaseSemantics::SValuePtr retval;
    if (SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(e)) {
        retval = operators->readRegister(rre->get_descriptor());
    } else if (SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(e)) {
        BaseSemantics::SValuePtr addr = effectiveAddress(mre, addr_nbits);
        retval = operators->readMemory(segmentRegister(mre), addr, operators->boolean_(true), value_nbits);
    } else if (SgAsmValueExpression *ve = isSgAsmValueExpression(e)) {
        uint64_t val = SageInterface::getAsmSignedConstant(ve);
        retval = operators->number_(value_nbits, val);
    } else {
        assert(!"not implemented");
        abort();
    }

    // Make sure the return value is the requested width. The unsignedExtend() can expand or shrink values.
    assert(retval!=NULL);
    if (retval->get_width()!=value_nbits)
        retval = operators->unsignedExtend(retval, value_nbits);
    return retval;
}


    
    

} // namespace
} // namespace
} // namespace
