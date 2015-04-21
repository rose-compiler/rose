#ifndef ROSE_RSIM_Templates_H
#define ROSE_RSIM_Templates_H

#include "Diagnostics.h"
#include <errno.h>
#include <syscall.h>

/* NOTE: not yet tested for guest_dirent_t == dirent64_t; i.e., the getdents64() syscall. [RPM 2010-11-17] */
template<class guest_dirent_t> /* either dirent32_t or dirent64_t */
int RSIM_Thread::getdents_syscall(int fd, uint32_t dirent_va, size_t sz)
{
    ROSE_ASSERT(sizeof(dirent64_t)>=sizeof(guest_dirent_t));

    size_t at = 0; /* position when filling specimen's buffer */
    uint8_t guest_buf[sz];
    uint8_t host_buf[sz];

    /* Read dentries from host kernel and copy to specimen's buffer. We must do this one dentry at a time because we don't want
     * to over read (there's no easy way to back up).  In other words, we read a dentry (but not more than what would fit in
     * the specimen) and if successful we copy to the specimen, translating from 64- to 32-bit.  The one-at-a-time requirement
     * is due to the return buffer value being run-length encoded. */
    long status = -EINVAL; /* buffer too small */
    while (at+sizeof(guest_dirent_t)<sz) {

        /* Read one dentry from host if possible */
        dirent64_t *host_dirent = (dirent64_t*)host_buf;
        size_t limit = sizeof(*host_dirent);
        status = -EINVAL; /* buffer too small */
        while (limit<=sz-at && -EINVAL==status) {
            status = syscall(SYS_getdents64, fd, host_buf, limit++);
            if (-1==status)
                status = -errno;
        }

        /* Convert and copy the host dentry into the specimen memory. */
        if (status>0) {
            ROSE_ASSERT(status>(long)sizeof(*host_dirent));
            guest_dirent_t *guest_dirent = (guest_dirent_t*)(guest_buf+at);

            /* name */
            ROSE_ASSERT(host_dirent->d_reclen > sizeof(*host_dirent));
            char *name_src = (char*)host_dirent + sizeof(*host_dirent);
            char *name_dst = (char*)guest_dirent + sizeof(*guest_dirent);
            size_t name_sz = host_dirent->d_reclen - sizeof(*host_dirent);
            memcpy(name_dst, name_src, name_sz);

            /* inode */
            ROSE_ASSERT(host_dirent->d_ino == (host_dirent->d_ino & 0xffffffff));
            guest_dirent->d_ino = host_dirent->d_ino;

            /* record length */
            guest_dirent->d_reclen = host_dirent->d_reclen - sizeof(*host_dirent)
                                     + sizeof(*guest_dirent) + 2/*padding and d_type*/;

            /* type */
            if (sizeof(guest_dirent_t)==sizeof(dirent32_t)) {
                ROSE_ASSERT(host_dirent->d_type == (host_dirent->d_type & 0xff));
                ((uint8_t*)guest_dirent)[guest_dirent->d_reclen-1] = host_dirent->d_type;
            } else {
                ROSE_ASSERT(sizeof(guest_dirent_t)==sizeof(dirent64_t));
                ((uint8_t*)guest_dirent)[sizeof(*guest_dirent)-1] = host_dirent->d_type;
            }

            /* offset to next dentry */
            at += guest_dirent->d_reclen;
            guest_dirent->d_off = at;
        }

        /* Termination conditions */
        if (status<=0) break;
    }

    if ((size_t)at!=get_process()->mem_write(guest_buf, dirent_va, at))
        return -EFAULT;

    return at>0 ? at : status;
}

template<
    template <template <size_t> class> class State,
    template <size_t> class ValueType
    >
template <size_t Len>
ValueType<Len>
RSIM_Semantics::InnerPolicy<State, ValueType>::readMemory(X86SegmentRegister sr, const ValueType<32> &addr,
                                                          const ValueType<1> &cond) {
    using namespace rose::Diagnostics;

    ROSE_ASSERT(0==Len % 8 && Len<=64);
    RSIM_Process *process = thread->get_process();
    uint32_t offset = addr.known_value();
    ROSE_ASSERT(sr_shadow[sr].present);
    ROSE_ASSERT(offset <= sr_shadow[sr].limit);
    ROSE_ASSERT(offset + (Len/8) - 1 <= sr_shadow[sr].limit);

    Sawyer::Message::Stream &mesg = tracing(TRACE_MEM);

    ROSE_ASSERT(cond.is_known());
    if (cond.known_value()) {
        uint8_t buf[Len/8];
        size_t nread = process->mem_read(buf, sr_shadow[sr].base+offset, Len/8);
        if (nread!=Len/8) {
            bool ismapped = process->mem_is_mapped(sr_shadow[sr].base+offset+nread);
            throw RSIM_SignalHandling::mk_sigfault(SIGSEGV, ismapped?SEGV_ACCERR:SEGV_MAPERR, sr_shadow[sr].base+offset+nread);
        }
        uint64_t result = 0;
        for (size_t i=0, j=0; i<Len; i+=8, j++)
            result |= buf[j] << i;

        mfprintf(mesg)("  readMemory<%zu>(0x%08"PRIx32"+0x%08"PRIx32"=0x%08"PRIx32") -> 0x%08"PRIx64"\n",
                       Len, sr_shadow[sr].base, offset, sr_shadow[sr].base+offset,
                       ValueType<Len>(result).known_value());

        return ValueType<Len>(result);
    } else {
        return ValueType<Len>(0);
    }
}

/* Writes memory to the memory map rather than the super class. */
template<
    template <template <size_t> class> class State,
    template <size_t> class ValueType
    >
template <size_t Len> void
RSIM_Semantics::InnerPolicy<State, ValueType>::writeMemory(X86SegmentRegister sr, const ValueType<32> &addr,
                                                           const ValueType<Len> &data, const ValueType<1> &cond) {
    using namespace rose::Diagnostics;

    ROSE_ASSERT(0==Len % 8 && Len<=64);
    RSIM_Process *process = thread->get_process();
    uint32_t offset = addr.known_value();
    ROSE_ASSERT(sr_shadow[sr].present);
    ROSE_ASSERT(offset <= sr_shadow[sr].limit);
    ROSE_ASSERT(offset + (Len/8) - 1 <= sr_shadow[sr].limit);
    ROSE_ASSERT(data.is_known());
    ROSE_ASSERT(cond.is_known());

    Sawyer::Message::Stream &mesg = tracing(TRACE_MEM);

    if (cond.known_value()) {
        mfprintf(mesg)("  writeMemory<%zu>(0x%08"PRIx32"+0x%08"PRIx32"=0x%08"PRIx32", 0x%08"PRIx64")\n",
                       Len, sr_shadow[sr].base, offset, sr_shadow[sr].base+offset, data.known_value());
            
        uint8_t buf[Len/8];
        for (size_t i=0, j=0; i<Len; i+=8, j++)
            buf[j] = (data.known_value() >> i) & 0xff;

        size_t nwritten = process->mem_write(buf, sr_shadow[sr].base+offset, Len/8);
        if (nwritten!=Len/8) {
            bool ismapped = process->mem_is_mapped(sr_shadow[sr].base+offset+nwritten);
#if 0   /* First attempt, according to Section 24.2.1 "Program Error Signals" of glibc documentation */
            /* Writing to mem that's not mapped results in SIGBUS; writing to mem that's mapped without write permission
             * results in SIGSEGV. */
            if (ismapped) {
                throw RSIM_SignalHandling::mk_sigfault(SIGSEGV, SEGV_ACCERR, sr_shadow[sr].base+offset+nwritten);
            } else {
                throw RSIM_SignalHandling::mk_sigfault(SIGBUS, SEGV_MAPERR, sr_shadow[sr].base+offset+nwritten);
            }
#else   /* Second attempt, according to actual experience */
            /* The syscall_tst.117.shmdt.01 shows that mapping a shared memory segment, then unmapping it, then trying to
             * write to it will result in a SIGSEGV, not a SIGBUS. */
            throw RSIM_SignalHandling::mk_sigfault(SIGSEGV,
                                                   ismapped ? SEGV_ACCERR : SEGV_MAPERR,
                                                   sr_shadow[sr].base+offset+nwritten);
#endif
        }
    }
}

template<
    template <template <size_t> class> class State,
    template <size_t> class ValueType
    >
void
RSIM_Semantics::InnerPolicy<State, ValueType>::ctor(RSIM_Thread *thread)
{
    assert(thread!=NULL);
    this->thread = thread;

    reg_eax = this->findRegister("eax", 32);
    reg_ebx = this->findRegister("ebx", 32);
    reg_ecx = this->findRegister("ecx", 32);
    reg_edx = this->findRegister("edx", 32);
    reg_esi = this->findRegister("esi", 32);
    reg_edi = this->findRegister("edi", 32);
    reg_eip = this->findRegister("eip", 32);
    reg_esp = this->findRegister("esp", 32);
    reg_ebp = this->findRegister("ebp", 32);

    reg_cs  = this->findRegister("cs", 16);
    reg_ds  = this->findRegister("ds", 16);
    reg_es  = this->findRegister("es", 16);
    reg_fs  = this->findRegister("fs", 16);
    reg_gs  = this->findRegister("gs", 16);
    reg_ss  = this->findRegister("ss", 16);

    reg_eflags = this->findRegister("eflags", 32);
    reg_df  = this->findRegister("df", 1);
    reg_tf  = this->findRegister("tf", 1);

    // See also: RSIM_Process::load
#if 0 // [Robb P. Matzke 2015-04-21]
    this->writeRegister(reg_eip,    ValueType<32>(0));
    this->writeRegister(reg_eax,    ValueType<32>(0));
    this->writeRegister(reg_ebx,    ValueType<32>(0));
    this->writeRegister(reg_ecx,    ValueType<32>(0));
    this->writeRegister(reg_edx,    ValueType<32>(0));
    this->writeRegister(reg_esi,    ValueType<32>(0));
    this->writeRegister(reg_edi,    ValueType<32>(0));
    this->writeRegister(reg_ebp,    ValueType<32>(0));
    this->writeRegister(reg_esp,    ValueType<32>(0xc0000000ul)); // high end of stack, exclusive
    this->writeRegister(reg_eflags, ValueType<32>(0x2)); // flag bit 1 is set to one, although this is a reserved register

    this->writeRegister(reg_cs,     ValueType<16>(0x23));
    this->writeRegister(reg_ds,     ValueType<16>(0x2b));
    this->writeRegister(reg_es,     ValueType<16>(0x2b));
    this->writeRegister(reg_ss,     ValueType<16>(0x2b));
    this->writeRegister(reg_fs,     ValueType<16>(0));
    this->writeRegister(reg_gs,     ValueType<16>(0));
#endif
}

template<
    template <template <size_t> class> class State,
    template <size_t> class ValueType
    >
Sawyer::Message::Stream&
RSIM_Semantics::InnerPolicy<State, ValueType>::tracing(TracingFacility what) const
{
    return thread->tracing(what);
}

template<
    template <template <size_t> class> class State,
    template <size_t> class ValueType
    >
void
RSIM_Semantics::InnerPolicy<State, ValueType>::interrupt(uint8_t num)
{
    if (num != 0x80)
        throw Interrupt(this->get_insn()->get_address(), num);
    thread->emulate_syscall();
}

template<
    template <template <size_t> class> class State,
    template <size_t> class ValueType
    >
void
RSIM_Semantics::InnerPolicy<State, ValueType>::cpuid()
{
    int code = this->template readRegister<32>(reg_eax).known_value();

    uint32_t dwords[4];
#if 0
    /* Prone to not compile */
    asm volatile("cpuid"
                 :
                 "=a"(*(dwords+0)),
                 "=b"(*(dwords+1)),
                 "=c"(*(dwords+2)),
                 "=d"(*(dwords+3))
                 :
                 "0"(code));
#else
    /* Return value based on an Intel model "Xeon X5680 @ 3.33GHz"; 3325.017GHz; stepping 2 */
    dwords[0] = 0x0000000b;
    dwords[1] = 0x756e6547;
    dwords[2] = 0x6c65746e;
    dwords[3] = 0x49656e69;
#endif

    /* Change "GenuineIntel" to "Genuine ROSE". Doing so should cause the caller to not execute any further CPUID
     * instructions since there's no well-known definition for the rest of our CPUID semantics. */
    if (0==code) {
        dwords[3] &= 0x00ffffff; dwords[3] |= 0x20000000;           /* ' ' */
        dwords[2] =  0x45534f52;                                    /* "ROSE" */
    } else {
        fprintf(stderr, "CPUID-%d probably should not have been executed!", code);
    }

    this->writeRegister(reg_eax, ValueType<32>(dwords[0]));
    this->writeRegister(reg_ebx, ValueType<32>(dwords[1]));
    this->writeRegister(reg_ecx, ValueType<32>(dwords[2]));
    this->writeRegister(reg_edx, ValueType<32>(dwords[3]));
}

template<
    template <template <size_t> class> class State,
    template <size_t> class ValueType
    >
void
RSIM_Semantics::InnerPolicy<State, ValueType>::sysenter()
{
    thread->emulate_syscall();

    /* On linux, SYSENTER is followed by zero or more NOPs, followed by a JMP back to just before the SYSENTER in order to
     * restart interrupted system calls, followed by POPs for the callee-saved registers. A non-interrupted system call should
     * return to the first POP instruction, which happens to be 9 bytes after the end of the SYSENTER. */
    this->writeRegister(reg_eip, add(this->template readRegister<32>(reg_eip), ValueType<32>(9)));
}

template<
    template <template <size_t> class> class State,
    template <size_t> class ValueType
    >
void
RSIM_Semantics::InnerPolicy<State, ValueType>::dump_registers(Sawyer::Message::Stream &_mesg)
{
    /* lock, so entire state is all together in the output */
    Sawyer::Message::Stream mesg(_mesg);
    mesg <<"   "
         <<" eax=" <<StringUtility::toHex2(this->template readRegister<32>(reg_eax).known_value(), 32)
         <<" ebx=" <<StringUtility::toHex2(this->template readRegister<32>(reg_ebx).known_value(), 32)
         <<" ecx=" <<StringUtility::toHex2(this->template readRegister<32>(reg_ecx).known_value(), 32)
         <<" edx=" <<StringUtility::toHex2(this->template readRegister<32>(reg_edx).known_value(), 32) <<"\n";
    mesg <<"   "
         <<" esi=" <<StringUtility::toHex2(this->template readRegister<32>(reg_esi).known_value(), 32)
         <<" edi=" <<StringUtility::toHex2(this->template readRegister<32>(reg_edi).known_value(), 32)
         <<" ebp=" <<StringUtility::toHex2(this->template readRegister<32>(reg_ebp).known_value(), 32)
         <<" esp=" <<StringUtility::toHex2(this->template readRegister<32>(reg_esp).known_value(), 32)
         <<" eip=" <<StringUtility::toHex2(this->get_ip().known_value(), 32) <<"\n";

    for (int i=0; i<6; i++) {
        std::string segreg_name = segregToString((X86SegmentRegister)i);
        mesg <<"   "
             <<" " <<segreg_name
             <<"=" <<StringUtility::toHex2(this->template readRegister<16>(segreg_name.c_str()).known_value(), 16)
             <<" base=" <<StringUtility::toHex2(sr_shadow[i].base, 32)
             <<" limit=" <<StringUtility::toHex2(sr_shadow[i].limit, 32)
             <<" present=" <<(sr_shadow[i].present?"yes":"no") <<"\n";
    }

    uint32_t eflags = get_eflags();
    mesg <<"    flags: " <<StringUtility::toHex2(eflags, 32);
    static const char *flag_name[] = {"cf",  "#1",  "pf",   "#3",    "af",    "#5",  "zf",  "sf",
                                      "tf",  "if",  "df",   "of", "iopl0", "iopl1",  "nt", "#15",
                                      "rf",  "vm",  "ac",  "vif",   "vip",    "id", "#22", "#23",
                                      "#24", "#25", "#26", "#27",   "#28",   "#29", "#30", "#31"};
    for (uint32_t i=0; i<32; i++) {
        if (eflags & (1u<<i))
            mesg <<" " <<flag_name[i];
    }
    mesg <<"\n";
}

template<
    template <template <size_t> class> class State,
    template <size_t> class ValueType
    >
uint32_t
RSIM_Semantics::InnerPolicy<State, ValueType>::get_eflags()
{
    return this->template readRegister<32>(reg_eflags).known_value();
}

template<
    template <template <size_t> class> class State,
    template <size_t> class ValueType
    >
ValueType<32>
RSIM_Semantics::InnerPolicy<State, ValueType>::pop()
{
    ValueType<32> old_sp = this->template readRegister<32>(reg_esp);
    ValueType<32> retval = this->template readMemory<32>(x86_segreg_ss, old_sp, this->true_());
    this->writeRegister(reg_esp, add(old_sp, ValueType<32>(4)));
    return retval;
}

template<
    template <template <size_t> class> class State,
    template <size_t> class ValueType
    >
void
RSIM_Semantics::InnerPolicy<State, ValueType>::startInstruction(SgAsmInstruction* insn)
{
    Sawyer::Message::Stream &mesg = tracing(TRACE_INSN);
    SAWYER_MESG(mesg) <<unparseInstruction(insn) <<"\n";
}

template<
    template <template <size_t> class> class State,
    template <size_t> class ValueType
    >
void
RSIM_Semantics::InnerPolicy<State, ValueType>::load_sr_shadow(X86SegmentRegister sr, unsigned gdt_num)
{
    user_desc_32 *info = thread->gdt_entry(gdt_num);
    sr_shadow[sr] = *info;
    //ROSE_ASSERT(sr_shadow[sr].present); //checked when used
}

#endif /* ROSE_RSIM_Templates_H */
