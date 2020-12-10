#include <rose.h>
#include <Diagnostics.h>
#include <DisassemblerX86.h>
#include <DisassemblerM68k.h>

#include "RSIM_Semantics.h"
#include "RSIM_Thread.h"

#if 1 // DEBUGGING [Robb P. Matzke 2015-06-01]
#include "TraceSemantics2.h"
#endif

using namespace Rose;
using namespace Rose::Diagnostics;
using namespace Rose::BinaryAnalysis;
using namespace Rose::BinaryAnalysis::InstructionSemantics2;

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
                                    ops->number_(d->REG_anyIP.nBits(), 9)));
    }
};

DispatcherPtr
createDispatcher(RSIM_Thread *owningThread) {
    BinaryAnalysis::Disassembler *disassembler = owningThread->get_process()->disassembler();
    Architecture arch = ARCH_NONE;
    if (dynamic_cast<BinaryAnalysis::DisassemblerX86*>(disassembler)) {
        arch = ARCH_X86;
    } else if (dynamic_cast<BinaryAnalysis::DisassemblerM68k*>(disassembler)) {
        arch = ARCH_M68k;
    } else {
        TODO("architecture not supported");
    }

    const RegisterDictionary *regs = disassembler->registerDictionary();
    RiscOperatorsPtr ops = RiscOperators::instance(arch, owningThread, regs);
    size_t wordSize = disassembler->instructionPointerRegister().nBits();
    ASSERT_require(wordSize == 32 || wordSize == 64);

#if 0 // DEBUGGING [Robb P. Matzke 2015-07-30]
    std::cerr <<"Using TraceSemantics for debugging (" <<__FILE__ <<":" <<__LINE__ <<")\n";
    TraceSemantics::RiscOperatorsPtr traceOps = TraceSemantics::RiscOperators::instance(ops);
    traceOps->stream().disable();                       // turn it on only when we need it
    ops = traceOps;
#endif

    DispatcherPtr dispatcher;
    switch (arch) {
        case ARCH_X86:
            dispatcher = DispatcherX86::instance(ops, wordSize, regs);
            dispatcher->iproc_set(x86_cpuid, new IP_cpuid);
            dispatcher->iproc_set(x86_sysenter, new IP_sysenter);
            ops->allocateOnDemand(false);               // OS controls what memory is available
            break;
        case ARCH_M68k:
            dispatcher = DispatcherM68k::instance(ops, wordSize, regs);
            ops->allocateOnDemand(true);                // No OS, so make all memory available
            break;
        default:
            ASSERT_not_reachable("invalid architecture");
    }

    return dispatcher;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RiscOperators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RiscOperators::loadShadowRegister(X86SegmentRegister sr, unsigned gdtIdx) {
    ASSERT_require(ARCH_X86 == architecture_);
    SegmentInfo segment(thread_->gdt_entry(gdtIdx));
    if (64 == thread_->get_process()->wordSize()) {
        if (sr != x86_segreg_fs && sr != x86_segreg_gs)
            segment.base = 0;
        segment.limit = 0xffffffffffffffffull;
    }
    segmentInfo_.insert(sr, segment);
}

RiscOperators::SegmentInfo&
RiscOperators::segmentInfo(X86SegmentRegister sr) {
    ASSERT_require(ARCH_X86 == architecture_);
    return segmentInfo_[sr];
}

void
RiscOperators::dumpState() {
    Sawyer::Message::Stream out(thread_->tracing(TRACE_STATE));
    out.enable();
    out <<"Semantic state for thread " <<thread_->get_tid() <<":\n";
    if (currentInstruction()) {
        out <<"  instruction #" <<nInsns() <<" at " <<unparseInstructionWithAddress(currentInstruction()) <<"\n";
    } else {
        out <<"  processed " <<StringUtility::plural(nInsns(), "instructions") <<"\n";
    }

    out <<"  registers:\n";
    BaseSemantics::Formatter format;
    format.set_line_prefix("    ");
    out <<(*currentState()->registerState()+format);

    out <<"  memory:\n";
    thread_->get_process()->mem_showmap(out, "memory:", "    ");

    if (ARCH_X86 == architecture_) {
        out <<"  segments:\n";
        RegisterNames regNames(currentState()->registerState()->get_register_dictionary());
        BOOST_FOREACH (const SegmentInfoMap::Node &node, segmentInfo_.nodes()) {
            Rose::BinaryAnalysis::RegisterDescriptor segreg(x86_regclass_segment, node.key(), 0, 16);
            out <<"    " <<regNames(segreg) <<": base=" <<StringUtility::addrToString(node.value().base)
                <<" limit=" <<StringUtility::addrToString(node.value().limit)
                <<" present=" <<(node.value().present?"yes":"no") <<"\n";
        }
    }
}

void
RiscOperators::startInstruction(SgAsmInstruction* insn) {
    Super::startInstruction(insn);
    SAWYER_MESG(thread_->tracing(TRACE_INSN)) <<"#" <<nInsns() <<" " <<unparseInstructionWithAddress(insn) <<"\n";
}

void
RiscOperators::interrupt(int majr, int minr) {
    ASSERT_require(ARCH_X86 == architecture_);
    if (x86_exception_int == majr && 0x80 == minr) {
        thread_->emulate_syscall();
    } else if (x86_exception_sysenter == majr) {
        // because it needs special processing for the return target
        ASSERT_not_reachable("syscall should have been handled by the dispatcher");
    } else if (x86_exception_syscall == majr) {
        thread_->emulate_syscall();
    } else if (x86_exception_int == majr) {
        throw Interrupt(currentInstruction()->get_address(), minr);
    } else {
        FIXME("interrupt/exception type not handled [Robb P. Matzke 2015-04-22]");
    }
}

void
RiscOperators::writeRegister(Rose::BinaryAnalysis::RegisterDescriptor reg, const BaseSemantics::SValuePtr &value) {
    Super::writeRegister(reg, value);
    if (ARCH_X86 == architecture_ && reg.majorNumber() == x86_regclass_segment) {
        ASSERT_require2(0 == value->get_number() || 3 == (value->get_number() & 7), "GDT and privilege level 3");
        loadShadowRegister((X86SegmentRegister)reg.minorNumber(), value->get_number() >> 3);
    }
}

BaseSemantics::SValuePtr
RiscOperators::readMemory(Rose::BinaryAnalysis::RegisterDescriptor segreg, const BaseSemantics::SValuePtr &address,
                          const BaseSemantics::SValuePtr &dflt, const BaseSemantics::SValuePtr &cond) {
    Sawyer::Message::Stream &mesg = thread_->tracing(TRACE_MEM);
    RSIM_Process *process = thread_->get_process();
    rose_addr_t offset = address->get_number();
    rose_addr_t addrMask = IntegerOps::genMask<rose_addr_t>(address->get_width());
    rose_addr_t addr = offset & addrMask;
    if (!cond->get_number())
        return dflt;

    // Check the address against the memory segment information
    ASSERT_require(dflt->get_width() % 8 == 0);
    size_t nBytes = dflt->get_width() / 8;
    if (ARCH_X86 == architecture_) {
        ASSERT_forbid(segreg.isEmpty());
        ASSERT_require(segmentInfo_.exists((X86SegmentRegister)segreg.minorNumber()));
        SegmentInfo &segment = segmentInfo_[(X86SegmentRegister)segreg.minorNumber()];
        ASSERT_require(segment.present);
        ASSERT_require(offset <= segment.limit);
        ASSERT_require(((offset + nBytes - 1) & addrMask) <= segment.limit);
        addr = (segment.base + offset) & addrMask;
        SAWYER_MESG(mesg) <<"  readMemory(" <<StringUtility::addrToString(segment.base) <<"+"
                          <<StringUtility::addrToString(offset) <<"=" <<StringUtility::addrToString(addr) <<") ";
    } else {
        SAWYER_MESG(mesg) <<"  readMemory(" <<StringUtility::addrToString(addr) <<") ";
    }

    // Read the data from memory
    uint8_t buffer[16];
    ASSERT_require(nBytes <= sizeof buffer);
    size_t nRead = process->mem_read(buffer, addr, nBytes);
    if (nRead != nBytes) {
        if (allocateOnDemand_) {
            // Reading from unallocated memory returns zeros rather than allocating anything.
            for (size_t i=0; i<nBytes; ++i) {
                if (process->mem_read(buffer+i, addr+i, 1) != 0)
                    buffer[i] = 0;
            }
        } else {
            bool isMapped = process->mem_is_mapped(addr + nRead);
            throw RSIM_SignalHandling::mk_sigfault(SIGSEGV, isMapped?SEGV_ACCERR:SEGV_MAPERR, addr+nRead);
        }
    }

    // Convert guest bytes read to host byte order
    ASSERT_require(ByteOrder::host_order() == ByteOrder::ORDER_LSB);
    Sawyer::Container::BitVector bv(8*nBytes);
    switch (architecture_) {
        case ARCH_X86:                                  // guest memory is little-endian
            for (size_t i=0; i<nRead; ++i)
                bv.fromInteger(Sawyer::Container::BitVector::BitRange::baseSize(i*8, 8), buffer[i]);
            break;
        case ARCH_M68k:                                 // guest memory is big-endian
            for (size_t i=0; i<nRead; ++i)
                bv.fromInteger(Sawyer::Container::BitVector::BitRange::baseSize(i*8, 8), buffer[nBytes-(i+1)]);
            break;
        default:
            ASSERT_not_reachable("invalid architecture");
    }
    BaseSemantics::SValuePtr retval = svalue_number(bv);

    SAWYER_MESG(mesg) <<"-> " <<*retval <<"\n";
    return retval;
}

void
RiscOperators::writeMemory(Rose::BinaryAnalysis::RegisterDescriptor segreg, const BaseSemantics::SValuePtr &address,
                           const BaseSemantics::SValuePtr &value_, const BaseSemantics::SValuePtr &cond) {
    Sawyer::Message::Stream &mesg = thread_->tracing(TRACE_MEM);
    RSIM_Process *process = thread_->get_process();
    rose_addr_t offset = address->get_number();
    rose_addr_t addrMask = IntegerOps::genMask<rose_addr_t>(address->get_width());
    SValuePtr value = SValue::promote(value_);
    rose_addr_t addr = offset & addrMask;
    if (!cond->get_number())
        return;

    // Check the address against the memory segment information
    ASSERT_require(value->get_width() % 8 == 0);
    size_t nBytes = value->get_width() / 8;
    if (ARCH_X86 == architecture_) {
        ASSERT_forbid(segreg.isEmpty());
        ASSERT_require(segmentInfo_.exists((X86SegmentRegister)segreg.minorNumber()));
        SegmentInfo &segment = segmentInfo_[(X86SegmentRegister)segreg.minorNumber()];
        ASSERT_require(segment.present);
        ASSERT_require(offset <= segment.limit);
        ASSERT_require(((offset + nBytes - 1) & addrMask) <= segment.limit);
        addr = (segment.base + offset) & addrMask;
        SAWYER_MESG(mesg) <<"  writeMemory(" <<StringUtility::addrToString(segment.base) <<"+"
                          <<StringUtility::addrToString(offset) <<"=" <<StringUtility::addrToString(addr)
                          <<", " <<*value <<")\n";
    } else {
        SAWYER_MESG(mesg) <<"  writeMemory(" <<StringUtility::addrToString(addr) <<", " <<*value <<")\n";
    }

    // Convert host bytes to guest memory byte order.
    ASSERT_require(ByteOrder::host_order() == ByteOrder::ORDER_LSB);
    uint8_t buffer[16];
    ASSERT_require(nBytes <= sizeof buffer);
    switch (architecture_) {
        case ARCH_X86:                                  // guest memory is little-endian
            for (size_t i=0; i<nBytes; ++i)
                buffer[i] = value->bits().toInteger(Sawyer::Container::BitVector::BitRange::baseSize(i*8, 8));
            break;
        case ARCH_M68k:
            for (size_t i=0; i<nBytes; ++i)
                buffer[nBytes-(i+1)] = value->bits().toInteger(Sawyer::Container::BitVector::BitRange::baseSize(i*8, 8));
            break;
        default:
            ASSERT_not_reachable("invalid architecture");
    }

    // Write buffer to memory map.
    size_t nWritten = process->mem_write(buffer, addr, nBytes);
    if (nWritten != nBytes) {
        if (allocateOnDemand_) {
            for (size_t i=0; i<nBytes; ++i) {
                if (process->mem_write(buffer+i, addr+i, 1) == 0 && buffer[i] != 0) {
                    // If memory is mapped then it must have no write permission. Treat this like an error.
                    if (process->get_memory()->at(addr+i).exists())
                        throw RSIM_SignalHandling::mk_sigfault(SIGSEGV, SEGV_ACCERR, addr+i);

                    // This address (addr+i) is not mapped, but try to map the whole page being careful to not occlude anything
                    // that's already mapped.
                    static const rose_addr_t pageSize = 8192;
                    rose_addr_t begin = alignDown(addr+i, pageSize);    // candidate first address to map
                    rose_addr_t end = alignUp(addr+i+1, pageSize);      // candidate end (exclusive) address to map
                    Sawyer::Optional<rose_addr_t> loMapped =            // optional last lower address already mapped
                        process->get_memory()->atOrBefore(addr).next(Sawyer::Container::MATCH_BACKWARD);
                    Sawyer::Optional<rose_addr_t> hiMapped =            // optional next higher address already mapped
                        process->get_memory()->atOrAfter(addr).next();
                    if (loMapped)
                        begin = std::max(begin, *loMapped+1);
                    if (hiMapped)
                        end = std::min(end, *hiMapped);
                    AddressInterval newArea = AddressInterval::baseSize(begin, end-begin);
                    ASSERT_forbid(process->get_memory()->isOverlapping(newArea));
                    process->get_memory()->insert(newArea,
                                                  MemoryMap::Segment(MemoryMap::AllocatingBuffer::instance(newArea.size()), 0,
                                                                     MemoryMap::READ_WRITE_EXECUTE, "demand allocated"));
                    process->get_memory()->dump(thread_->tracing(TRACE_MMAP));
                    nWritten = process->mem_write(buffer+i, addr+i, 1);
                    ASSERT_require(nWritten == 1);
                }
            }
        } else {
            // Generate a fault. According to GLibc documentation Section 24.2.1 "Program Error Signals", attempts to write to
            // memory that is not mapped results in SIGBUS, and writing to memory mapped without write permission results in
            // SIGSEGV.  However, actual experience (e.g., syscall_tst.117.shmdt.01) shows that mapping a shared memory
            // segment, then unmapping it, then trying to write to it will result in SIGSEGV rather than SIGBUS.
            bool isMapped = process->mem_is_mapped(addr + nWritten);
            throw RSIM_SignalHandling::mk_sigfault(SIGSEGV, isMapped ? SEGV_ACCERR : SEGV_MAPERR, addr+nWritten);
        }
    }
}


    
} // namespace
