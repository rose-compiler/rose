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
    x.print(o, NULL);
    return o;
}

std::ostream& operator<<(std::ostream &o, const MemoryCell &x) {
    x.print(o, NULL);
    return o;
}

std::ostream& operator<<(std::ostream &o, const MemoryState &x) {
    x.print(o, "", NULL);
    return o;
}

std::ostream& operator<<(std::ostream &o, const RegisterState &x) {
    x.print(o, "", NULL);
    return o;
}

std::ostream& operator<<(std::ostream &o, const State &x) {
    x.print(o, "", NULL);
    return o;
}

std::ostream& operator<<(std::ostream &o, const RiscOperators &x) {
    x.print(o, "", NULL);
    return o;
}

/*******************************************************************************************************************************
 *                                      Exceptions
 *******************************************************************************************************************************/

void
Exception::print(std::ostream &o) const
{
    o <<"BinaryAnalysis::InstructionSemantics::BaseSemantics::Exception: " <<mesg;
    if (insn)
        o <<": " <<unparseInstructionWithAddress(insn);
    o <<"\n";
}

/*******************************************************************************************************************************
 *                                      Semantic Values
 *******************************************************************************************************************************/

Allocator SValue::allocator;

/*******************************************************************************************************************************
 *                                      RegisterStateX86
 *******************************************************************************************************************************/

RegisterStatePtr
RegisterStateX86::create(const SValuePtr &protoval) const
{
    return RegisterStatePtr(new RegisterStateX86(protoval));
}

RegisterStatePtr
RegisterStateX86::clone() const
{
    return RegisterStatePtr(new RegisterStateX86(*this));
}

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
    assert(towrite->get_width()==reg.get_nbits());
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
RegisterStateX86::print(std::ostream &o, const std::string prefix, PrintHelper *ph) const 
{
    FormatRestorer save_fmt(o);
    for (size_t i=0; i<n_gprs; ++i) {
        o <<prefix <<std::setw(7) <<std::left <<gprToString((X86GeneralPurposeRegister)i) <<" = { ";
        gpr[i]->print(o, ph);
        o <<" }\n";
    }
    for (size_t i=0; i<n_segregs; ++i) {
        o <<prefix <<std::setw(7) <<std::left <<segregToString((X86SegmentRegister)i) <<" = { ";
        segreg[i]->print(o, ph);
        o <<" }\n";
    }
    for (size_t i=0; i<n_flags; ++i) {
        o <<prefix <<std::setw(7) <<std::left <<flagToString((X86Flag)i) <<" = { ";
        flag[i]->print(o, ph);
        o <<" }\n";
    }
    o <<prefix <<std::setw(7) <<std::left <<"ip" <<" = { ";
    ip->print(o, ph);
    o <<" }\n";
}

/*******************************************************************************************************************************
 *                                      MemoryState
 *******************************************************************************************************************************/

bool
MemoryCell::may_alias(const MemoryCellPtr &other, RiscOperators *ops) const
{
    // Check for the easy case:  two one-byte cells may alias one another if their addresses may be equal.
    if (8==value->get_width() && 8==other->get_value()->get_width())
        return address->may_equal(other->get_address(), ops->get_solver());

    size_t addr_nbits = address->get_width();
    assert(other->get_address()->get_width()==addr_nbits);

    assert(value->get_width() % 8 == 0);                // memory is byte addressable, so values must be multiples of a byte
    SValuePtr lo1 = address;
    SValuePtr hi1 = ops->add(lo1, ops->number_(lo1->get_width(), value->get_width() / 8));

    assert(other->get_value()->get_width() % 8 == 0);
    SValuePtr lo2 = other->get_address();
    SValuePtr hi2 = ops->add(lo2, ops->number_(lo2->get_width(), other->get_value()->get_width() / 8));

    // Two cells may_alias iff we can prove that they are not disjoint.  The two cells are disjoint iff lo2 >= hi1 or lo1 >=
    // hi2. Two things complicate this: first, the values might not be known quantities, depending on the semantic domain.
    // Second, the RiscOperators does not define a greater-than-or-equal operation, so we need to write it in terms of a
    // subtraction. See x86 CMP and JG instructions for examples. ("sf" is sign flag, "of" is overflow flag.)
    SValuePtr carries;
    SValuePtr diff = ops->addWithCarries(lo2, ops->invert(hi1), ops->true_(), carries/*out*/);
    SValuePtr sf = ops->extract(diff, addr_nbits-1, addr_nbits);
    SValuePtr of = ops->xor_(ops->extract(carries, addr_nbits-1, addr_nbits), ops->extract(carries, addr_nbits-2, addr_nbits-1));
    SValuePtr cond1 = ops->invert(ops->xor_(sf, of));
    diff = ops->addWithCarries(lo1, ops->invert(hi2), ops->true_(), carries/*out*/);
    sf = ops->extract(diff, addr_nbits-1, addr_nbits);
    of = ops->xor_(ops->extract(carries, addr_nbits-1, addr_nbits), ops->extract(carries, addr_nbits-2, addr_nbits-1));
    SValuePtr cond2 = ops->invert(ops->xor_(sf, of));
    SValuePtr disjoint = ops->or_(cond1, cond2);
    if (disjoint->is_number() && disjoint->get_number()!=0)
        return false;
    return true;
}

bool
MemoryCell::must_alias(const MemoryCellPtr &other, RiscOperators *ops) const
{
    // Check the easy case: two one-byte cells must alias one another if their address must be equal.
    if (8==value->get_width() && 8==other->get_value()->get_width())
        return address->must_equal(other->get_address(), ops->get_solver());

    size_t addr_nbits = address->get_width();
    assert(other->get_address()->get_width()==addr_nbits);

    assert(value->get_width() % 8 == 0);
    SValuePtr lo1 = address;
    SValuePtr hi1 = ops->add(lo1, ops->number_(lo1->get_width(), value->get_width() / 8));

    assert(other->get_value()->get_width() % 8 == 0);
    SValuePtr lo2 = other->get_address();
    SValuePtr hi2 = ops->add(lo2, ops->number_(lo2->get_width(), other->get_value()->get_width() / 8));

    // Two cells must_alias iff hi2 >= lo1 and hi1 >= lo2. Two things complicate this: first, the values might not be known
    // quantities, depending on the semantic domain.  Second, the RiscOperators does not define a greater-than-or-equal
    // operation, so we need to write it in terms of a subtraction. See x86 CMP and JG instructions for examples. ("sf" is sign
    // flag, "of" is overflow flag.)
    SValuePtr carries;
    SValuePtr diff = ops->addWithCarries(hi2, ops->invert(lo1), ops->true_(), carries/*out*/);
    SValuePtr sf = ops->extract(diff, addr_nbits-1, addr_nbits);
    SValuePtr of = ops->xor_(ops->extract(carries, addr_nbits-1, addr_nbits), ops->extract(carries, addr_nbits-2, addr_nbits-1));
    SValuePtr cond1 = ops->invert(ops->xor_(sf, of));
    diff = ops->addWithCarries(hi1, ops->invert(lo2), ops->true_(), carries/*out*/);
    sf = ops->extract(diff, addr_nbits-1, addr_nbits);
    of = ops->xor_(ops->extract(carries, addr_nbits-1, addr_nbits), ops->extract(carries, addr_nbits-2, addr_nbits-1));
    SValuePtr cond2 = ops->invert(ops->xor_(sf, of));
    SValuePtr overlap = ops->and_(cond1, cond2);
    if (overlap->is_number() && overlap->get_number()!=0)
        return true;
    return false;
}

SValuePtr
MemoryCellList::readMemory(const SValuePtr &addr, const SValuePtr &dflt, size_t nbits, RiscOperators *ops)
{
    assert(addr!=NULL);
    assert(dflt!=NULL && (!byte_restricted || dflt->get_width()==8));
    bool short_circuited;
    CellList found = scan(addr, nbits, ops, short_circuited/*out*/);
    size_t nfound = found.size();

    SValuePtr retval;
    if (1!=nfound || !short_circuited) {
        retval = dflt; // found no matches, multiple matches, or we fell off the end of the cell list
        cells.push_front(protocell->create(addr, dflt));
    } else {
        retval = found.front()->get_value();
    }
    return retval;
}

void
MemoryCellList::writeMemory(const SValuePtr &addr, const SValuePtr &value, RiscOperators *ops)
{
    assert(!byte_restricted || value->get_width()==8);
    MemoryCellPtr cell = protocell->create(addr, value);
    cells.push_front(cell);
}

void
MemoryCellList::print(std::ostream &o, const std::string prefix, PrintHelper *helper/*=NULL*/) const
{
    for (CellList::const_iterator ci=cells.begin(); ci!=cells.end(); ++ci) {
        o <<prefix;
        (*ci)->print(o, helper);
        o <<"\n";
    }
}

MemoryCellList::CellList
MemoryCellList::scan(const BaseSemantics::SValuePtr &addr, size_t nbits, RiscOperators *ops, bool &short_circuited/*out*/) const
{
    short_circuited = false;
    CellList retval;
    MemoryCellPtr tmpcell = protocell->create(addr, ops->undefined_(nbits));
    for (CellList::const_iterator ci=cells.begin(); ci!=cells.end(); ++ci) {
        if (tmpcell->may_alias(*ci, ops)) {
            retval.push_back(*ci);
            if ((short_circuited = tmpcell->must_alias(*ci, ops)))
                break;
        }
    }
    return retval;
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
    if (!iproc)
        throw Exception("no dispatch ability for instruction", insn);
    try {
        iproc->process(shared_from_this(), insn);
    } catch (Exception &e) {
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

} // namespace
} // namespace
} // namespace
