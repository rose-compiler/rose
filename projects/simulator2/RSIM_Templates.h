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

#endif /* ROSE_RSIM_Templates_H */
