#ifndef ROSE_RSIM_Templates_H
#define ROSE_RSIM_Templates_H

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

template <size_t Len> VirtualMachineSemantics::ValueType<Len>
RSIM_SemanticPolicy::readMemory(X86SegmentRegister sr, const VirtualMachineSemantics::ValueType<32> &addr,
                                const VirtualMachineSemantics::ValueType<1> cond) {
    ROSE_ASSERT(0==Len % 8 && Len<=64);
    RSIM_Process *process = thread->get_process();
    uint32_t offset = addr.known_value();
    ROSE_ASSERT(offset <= sr_shadow[sr].limit);
    ROSE_ASSERT(offset + (Len/8) - 1 <= sr_shadow[sr].limit);

    RTS_Message *mesg = tracing(TRACE_MEM);

    ROSE_ASSERT(cond.is_known());
    if (cond.known_value()) {
        uint8_t buf[Len/8];
        size_t nread = process->mem_read(buf, sr_shadow[sr].base+offset, Len/8);
        if (nread!=Len/8)
            throw Signal(SIGSEGV);
        uint64_t result = 0;
        for (size_t i=0, j=0; i<Len; i+=8, j++)
            result |= buf[j] << i;

        mesg->mesg("  readMemory<%zu>(0x%08"PRIx32"+0x%08"PRIx32"=0x%08"PRIx32") -> 0x%08"PRIx64"\n",
                   Len, sr_shadow[sr].base, offset, sr_shadow[sr].base+offset,
                   VirtualMachineSemantics::ValueType<Len>(result).known_value());

        return result;
    } else {
        return 0;
    }
}

/* Writes memory to the memory map rather than the super class. */
template <size_t Len> void
RSIM_SemanticPolicy::writeMemory(X86SegmentRegister sr, const VirtualMachineSemantics::ValueType<32> &addr,
                                 const VirtualMachineSemantics::ValueType<Len> &data,  VirtualMachineSemantics::ValueType<1> cond) {
    ROSE_ASSERT(0==Len % 8 && Len<=64);
    RSIM_Process *process = thread->get_process();
    uint32_t offset = addr.known_value();
    ROSE_ASSERT(offset <= sr_shadow[sr].limit);
    ROSE_ASSERT(offset + (Len/8) - 1 <= sr_shadow[sr].limit);
    ROSE_ASSERT(data.is_known());
    ROSE_ASSERT(cond.is_known());

    RTS_Message *mesg = tracing(TRACE_MEM);

    if (cond.known_value()) {
        mesg->mesg("  writeMemory<%zu>(0x%08"PRIx32"+0x%08"PRIx32"=0x%08"PRIx32", 0x%08"PRIx64")\n",
                   Len, sr_shadow[sr].base, offset, sr_shadow[sr].base+offset, data.known_value());
            
        uint8_t buf[Len/8];
        for (size_t i=0, j=0; i<Len; i+=8, j++)
            buf[j] = (data.known_value() >> i) & 0xff;

        size_t nwritten = process->mem_write(buf, sr_shadow[sr].base+offset, Len/8);
        if (nwritten!=Len/8) {
#if 0   /* First attempt, according to Section 24.2.1 "Program Error Signals" of glibc documentation */
            /* Writing to mem that's not mapped results in SIGBUS; writing to mem that's mapped without write permission
             * results in SIGSEGV. */
            if (process->mem_is_mapped(base+offset)) {
                throw Signal(SIGSEGV);
            } else {
                throw Signal(SIGBUS);
            }
#else   /* Second attempt, according to actual experience */
            /* The syscall_tst.117.shmdt.01 shows that mapping a shared memory segment, then unmapping it, then trying to
             * write to it will result in a SIGSEGV, not a SIGBUS. */
            throw Signal(SIGSEGV);
#endif
        }
    }
}

#endif /* ROSE_RSIM_Templates_H */
