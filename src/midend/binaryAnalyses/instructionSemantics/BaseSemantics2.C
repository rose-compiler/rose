#include "sage3basic.h"
#include "BaseSemantics2.h"
#include "AsmUnparser_compat.h"

namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace BaseSemantics {

/*******************************************************************************************************************************
 *                                      Printing operator<<
 *******************************************************************************************************************************/

std::ostream& operator<<(std::ostream &o, const RiscOperators::Exception &x) {
    o <<"semantics exception: " <<x.mesg;
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

std::ostream& operator<<(std::ostream &o, const Dispatcher::Exception &x) {
    x.print(o);
    return o;
}

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
RegisterStateX86::readRegister(const RegisterDescriptor &reg)
{
    assert(!"FIXME");
    abort(); 
}

void
RegisterStateX86::writeRegister(const RegisterDescriptor &reg, const SValuePtr &value)
{
    assert(!"FIXME");
    abort();
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

SValuePtr
MemoryCellList::readMemory(const SValuePtr &addr)
{
    assert(addr!=NULL);
    for (std::list<MemoryCell>::iterator ci=cells.begin(); ci!=cells.end(); ++ci) {
        if (ci->get_address()->must_equal(addr))
            return ci->get_value();
    }
    return protoval->undefined_(8);
}

void
MemoryCellList::writeMemory(const SValuePtr &addr, const SValuePtr &value)
{
    cells.push_front(MemoryCell(addr, value));
}

void
MemoryCellList::print(std::ostream &o, const std::string prefix, PrintHelper *helper/*=NULL*/) const
{
    for (std::list<MemoryCell>::const_iterator ci=cells.begin(); ci!=cells.end(); ++ci) {
        o <<prefix;
        ci->print(o, helper);
        o <<"\n";
    }
}

/*******************************************************************************************************************************
 *                                      RiscOperators
 *******************************************************************************************************************************/

const RegisterDescriptor &
RiscOperators::findRegister(const std::string &regname, size_t nbits/*=0*/)
{
    const RegisterDictionary *regdict = get_register_dictionary();
    if (!regdict)
        throw Exception("no register dictionary");

    const RegisterDescriptor *reg = regdict->lookup(regname);
    if (!reg) {
        std::ostringstream ss;
        ss <<"Invalid register: \"" <<regname <<"\"";
        throw Exception(ss.str());
    }

    if (nbits>0 && reg->get_nbits()!=nbits) {
        std::ostringstream ss;
        ss <<"Invalid " <<nbits <<"-bit register: \"" <<regname <<"\" is "
           <<reg->get_nbits() <<" " <<(1==reg->get_nbits()?"byte":"bytes");
        throw Exception(ss.str());
    }
    return *reg;
}

/*******************************************************************************************************************************
 *                                      Dispatcher
 *******************************************************************************************************************************/

void
Dispatcher::Exception::print(std::ostream &o) const
{
    o <<"Dispatcher::Exception: " <<mesg;
    if (insn)
        o <<": " <<unparseInstructionWithAddress(insn);
    o <<"\n";
}

void
Dispatcher::processInstruction(SgAsmInstruction *insn)
{
    const RegisterDescriptor &REG_EIP = operators->findRegister("eip");
    operators->writeRegister(REG_EIP, operators->add(operators->number_(32, insn->get_address()),
                                                     operators->number_(32, insn->get_size())));
    operators->startInstruction(insn);
    InsnProcessor *iproc = iproc_lookup(insn);
    if (!iproc)
        throw Exception("no dispatch ability for instruction", insn);
    try {
        iproc->process(shared_from_this(), insn);
    } catch (Exception e) {
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

} // namespace
} // namespace
} // namespace
