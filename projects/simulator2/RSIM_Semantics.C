#include <rose.h>
#include <Diagnostics.h>

#include "RSIM_Semantics.h"
#include "RSIM_Thread.h"

using namespace rose::Diagnostics;
using namespace rose::BinaryAnalysis::InstructionSemantics2;

namespace RSIM_Semantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Dispatcher
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// We only handle a few special cases for this complex instruction
struct IP_cpuid: public X86::InsnProcessor {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        BaseSemantics::SValuePtr codeExpr = d->readRegister(d->REG_EAX);
        unsigned code = codeExpr->get_number();

        // Return value based on an Intel model "Xeon X5680 @ 3.33GHz"; 3325.017GHz; stepping 2
        uint32_t dwords[4];
        dwords[0] = 0x0000000b;
        dwords[1] = 0x756e6547;
        dwords[2] = 0x6c65746e;
        dwords[3] = 0x49656e69;

        // Change "GenuineIntel" to "Genuine ROSE". Doing so should cause the caller to not execute any further CPUID
        // instructions since there's no well-known definition for the rest of our CPUID semantics.
        if (0==code) {
            dwords[3] &= 0x00ffffff; dwords[3] |= 0x20000000;           /* ' ' */
            dwords[2] =  0x45534f52;                                    /* "ROSE" */
        } else {
            std::cerr <<"x86 CPUID-" <<codeExpr <<" instruction ignored\n";
        }

        ops->writeRegister(d->REG_EAX, ops->number_(32, dwords[0]));
        ops->writeRegister(d->REG_EBX, ops->number_(32, dwords[1]));
        ops->writeRegister(d->REG_ECX, ops->number_(32, dwords[2]));
        ops->writeRegister(d->REG_EDX, ops->number_(32, dwords[3]));
    }
};

// On linux, SYSENTER is followed by zero or more NOPs, followed by a JMP back to just before the SYSENTER in order to restart
// interrupted system calls, followed by POPs for the callee-saved registers. A non-interrupted system call should return to
// the first POP instruction, which happens to be 9 bytes after the end of the SYSENTER.
struct IP_sysenter: public X86::InsnProcessor {
    void p(D d, Ops ops_, I insn, A args) {
        assert_args(insn, args, 0);
        RiscOperators *ops = dynamic_cast<RiscOperators*>(ops_);
        ASSERT_not_null(ops);
        ops->thread()->emulate_syscall();
        ops->writeRegister(d->REG_anyIP,
                           ops->add(ops->readRegister(d->REG_anyIP),
                                    ops->number_(d->REG_anyIP.get_nbits(), 9)));
    }
};

DispatcherPtr
createDispatcher(RSIM_Thread *owningThread) {
    const RegisterDictionary *regs = owningThread->get_process()->get_disassembler()->get_registers();
    RiscOperatorsPtr ops = RiscOperators::instance(owningThread, regs, NULL);
    size_t wordSize = regs->findLargestRegister(x86_regclass_gpr, x86_gpr_ax).get_nbits();
    ASSERT_require(wordSize == 32 || wordSize == 64);
    DispatcherPtr dispatcher = DispatcherX86::instance(ops, wordSize, regs);
    dispatcher->iproc_set(x86_cpuid, new IP_cpuid);
    dispatcher->iproc_set(x86_sysenter, new IP_sysenter);
    return dispatcher;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RiscOperators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RiscOperators::loadShadowRegister(X86SegmentRegister sr, unsigned gdtIdx) {
    user_desc_32 *segment = thread_->gdt_entry(gdtIdx);
    segmentInfo_.insert(sr, *segment);
}

void
RiscOperators::dumpState() {
    Sawyer::Message::Stream out(thread_->tracing(TRACE_STATE));
    out.enable();
    out <<"Semantic state for thread " <<thread_->get_tid() <<":\n";
    if (get_insn()) {
        out <<"  instruction #" <<get_ninsns() <<" at " <<unparseInstructionWithAddress(get_insn()) <<"\n";
    } else {
        out <<"  processed " <<StringUtility::plural(get_ninsns(), "instructions") <<"\n";
    }

    out <<"  registers:\n";
    BaseSemantics::Formatter format;
    format.set_line_prefix("    ");
    out <<(*get_state()->get_register_state()+format);

    out <<"  memory:\n";
    thread_->get_process()->mem_showmap(out, "memory:", "    ");

    out <<"  segments:\n";
    RegisterNames regNames(get_state()->get_register_state()->get_register_dictionary());
    BOOST_FOREACH (const SegmentInfoMap::Node &node, segmentInfo_.nodes()) {
        RegisterDescriptor segreg(x86_regclass_segment, node.key(), 0, 16);
        out <<"    " <<regNames(segreg) <<": base=" <<StringUtility::addrToString(node.value().base)
            <<" limit=" <<StringUtility::addrToString(node.value().limit)
            <<" present=" <<(node.value().present?"yes":"no") <<"\n";
    }
}

void
RiscOperators::startInstruction(SgAsmInstruction* insn) {
    Super::startInstruction(insn);
    SAWYER_MESG(thread_->tracing(TRACE_INSN)) <<"#" <<get_ninsns() <<" " <<unparseInstructionWithAddress(insn) <<"\n";
}

void
RiscOperators::interrupt(int majr, int minr) {
    if (x86_exception_int == majr && 0x80 == minr) {
        thread_->emulate_syscall();
    } else if (x86_exception_sysenter == majr) {
        ASSERT_not_reachable("syscall should have been handled by the dispatcher");
    } else if (x86_exception_int == majr) {
        throw Interrupt(get_insn()->get_address(), minr);
    } else {
        FIXME("interrupt/exception type not handled [Robb P. Matzke 2015-04-22]");
    }
}

void
RiscOperators::writeRegister(const RegisterDescriptor &reg, const BaseSemantics::SValuePtr &value) {
    Super::writeRegister(reg, value);
    if (reg.get_major() == x86_regclass_segment) {
        ASSERT_require2(0 == value->get_number() || 3 == (value->get_number() & 7), "GDT and privilege level 3");
        loadShadowRegister((X86SegmentRegister)reg.get_minor(), value->get_number() >> 3);
    }
}

BaseSemantics::SValuePtr
RiscOperators::readMemory(const RegisterDescriptor &segreg, const BaseSemantics::SValuePtr &address,
                          const BaseSemantics::SValuePtr &dflt, const BaseSemantics::SValuePtr &cond) {
    Sawyer::Message::Stream &mesg = thread_->tracing(TRACE_MEM);
    RSIM_Process *process = thread_->get_process();
    rose_addr_t offset = address->get_number();
    rose_addr_t addrMask = IntegerOps::genMask<rose_addr_t>(address->get_width());

    // Check the address against the memory segment information
    ASSERT_require(dflt->get_width() % 8 == 0);
    size_t nBytes = dflt->get_width() / 8;
    ASSERT_require(segreg.is_valid());
    ASSERT_require(segmentInfo_.exists((X86SegmentRegister)segreg.get_minor()));
    SegmentInfo &segment = segmentInfo_[(X86SegmentRegister)segreg.get_minor()];
    ASSERT_require(segment.present);
    ASSERT_require(offset <= segment.limit);
    ASSERT_require(((offset + nBytes - 1) & addrMask) <= segment.limit);

    if (!cond->get_number())
        return dflt;

    rose_addr_t addr = (segment.base + offset) & addrMask;

    // Read the data from memory
    uint8_t buffer[16];
    ASSERT_require(nBytes <= sizeof buffer);
    size_t nRead = process->mem_read(buffer, addr, nBytes);
    if (nRead != nBytes) {
        bool isMapped = process->mem_is_mapped(addr + nRead);
        throw RSIM_SignalHandling::mk_sigfault(SIGSEGV, isMapped?SEGV_ACCERR:SEGV_MAPERR, addr+nRead);
    }

    // Convert to a little-endian value
    Sawyer::Container::BitVector bv(8*nBytes);
    for (size_t i=0; i<nRead; ++i)
        bv.fromInteger(Sawyer::Container::BitVector::BitRange::baseSize(i*8, 8), buffer[i]);
    BaseSemantics::SValuePtr retval = svalue_number(bv);

    // Diagnostics
    mesg <<"  readMemory(" <<StringUtility::addrToString(segment.base) <<"+"
         <<StringUtility::addrToString(offset) <<"=" <<StringUtility::addrToString(addr)
         <<") -> " <<retval <<"\n";

    return retval;
}

void
RiscOperators::writeMemory(const RegisterDescriptor &segreg, const BaseSemantics::SValuePtr &address,
                           const BaseSemantics::SValuePtr &value_, const BaseSemantics::SValuePtr &cond) {
    Sawyer::Message::Stream &mesg = thread_->tracing(TRACE_MEM);
    RSIM_Process *process = thread_->get_process();
    rose_addr_t offset = address->get_number();
    rose_addr_t addrMask = IntegerOps::genMask<rose_addr_t>(address->get_width());
    SValuePtr value = SValue::promote(value_);

    // Check the address against the memory segment information
    ASSERT_require(value->get_width() % 8 == 0);
    size_t nBytes = value->get_width() / 8;
    ASSERT_require(segreg.is_valid());
    ASSERT_require(segmentInfo_.exists((X86SegmentRegister)segreg.get_minor()));
    SegmentInfo &segment = segmentInfo_[(X86SegmentRegister)segreg.get_minor()];
    ASSERT_require(segment.present);
    ASSERT_require(offset <= segment.limit);
    ASSERT_require(((offset + nBytes - 1) & addrMask) <= segment.limit);

    if (!cond->get_number())
        return;

    rose_addr_t addr = (segment.base + offset) & addrMask;

    mesg <<"  writeMemory(" <<StringUtility::addrToString(segment.base) <<"+"
         <<StringUtility::addrToString(offset) <<"=" <<StringUtility::addrToString(addr)
         <<", " <<value <<")\n";

    // Convert to a little-endian value
    uint8_t buffer[16];
    ASSERT_require(nBytes <= sizeof buffer);
    for (size_t i=0; i<nBytes; ++i)
        buffer[i] = value->bits().toInteger(Sawyer::Container::BitVector::BitRange::baseSize(i*8, 8));

    // Write buffer to memory map or simulate a segmentation fault. According to GLibc documentation Section 24.2.1 "Program
    // Error Signals", attempts to write to memory that is not mapped results in SIGBUS, and writing to memory mapped without
    // write permission results in SIGSEGV.  However, actual experience (e.g., syscall_tst.117.shmdt.01) shows that mapping a
    // shared memory segment, then unmapping it, then trying to write to it will result in SIGSEGV rather than SIGBUS.
    size_t nWritten = process->mem_write(buffer, addr, nBytes);
    if (nWritten != nBytes) {
        bool isMapped = process->mem_is_mapped(addr + nWritten);
        throw RSIM_SignalHandling::mk_sigfault(SIGSEGV, isMapped ? SEGV_ACCERR : SEGV_MAPERR, addr+nWritten);
    }
}


    
} // namespace
