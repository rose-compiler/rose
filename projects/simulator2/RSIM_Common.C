#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR

#include "integerOps.h"
#include "Diagnostics.h"
#include <arpa/inet.h>
#include <sys/vfs.h>
#include <sys/wait.h>
#include <sys/socket.h>

using namespace rose::Diagnostics;
using namespace StringUtility;

unsigned
tracingFacilityBit(TracingFacility tf)
{
    if (tf == TRACE_NFACILITIES)
        return IntegerOps::GenMask<unsigned, (unsigned)TRACE_NFACILITIES>::value;
    return IntegerOps::shl1<unsigned>((unsigned)tf);
}

void
print_SegmentDescriptor(Sawyer::Message::Stream &m, const uint8_t *_ud, size_t sz)
{
    const SegmentDescriptor *ud = (const SegmentDescriptor*)_ud;
    assert(sizeof(*ud)==sz);

    const char *content_type = "unknown";
    switch (ud->contents) {
        case 0: content_type = "data"; break;
        case 1: content_type = "stack"; break;
        case 2: content_type = "code"; break;
    }

    mfprintf(m)("entry=%d, base=0x%08lx, limit=0x%08lx, %s, %s, %s, %s, %s, %s",
                (int)ud->entry_number, (unsigned long)ud->base_addr, (unsigned long)ud->limit,
                ud->seg_32bit ? "32bit" : "16bit",
                content_type, ud->read_exec_only ? "read_exec" : "writable",
                ud->limit_in_pages ? "page_gran" : "byte_gran",
                ud->seg_not_present ? "not_present" : "present",
                ud->usable ? "usable" : "not_usable");
}

void
print_int_32(Sawyer::Message::Stream &m, const uint8_t *_v, size_t sz)
{
    const uint32_t *v = (const uint32_t*)_v;
    assert(sz % 4 == 0);
    size_t nelmts = sz/4;

    if (1==nelmts) {
        mfprintf(m)("%"PRId32, v[0]);
    } else {
        m <<"[";
        for (size_t i=0; i<nelmts; i++)
            mfprintf(m)("%s%"PRId32, i?",":"", v[i]);
        m <<"]";
    }
}

void
print_hex_64(Sawyer::Message::Stream &m, const uint8_t *_v, size_t sz)
{
    const uint64_t *v = (const uint64_t*)_v;
    assert(sz % 8 == 0);
    size_t nelmts = sz/8;

    if (1==nelmts) {
        m <<toHex2(v[0], 64);
    } else {
        m <<"[";
        for (size_t i=0; i<nelmts; i++)
            m <<(i?",":"") <<toHex2(v[i], 64);
        m <<"]";
    }
}

void
print_rlimit_32(Sawyer::Message::Stream &m, const uint8_t *ptr, size_t sz)
{
    assert(8==sz); /* two 32-bit unsigned integers */
    if (0==~((const uint32_t*)ptr)[0]) {
        m <<"rlim_cur=unlimited";
    } else {
        mfprintf(m)("rlim_cur=%"PRIu32, ((const uint32_t*)ptr)[0]);
    }
    if (0==~((const uint32_t*)ptr)[1]) {
        m <<", rlim_max=unlimited";
    } else {
        mfprintf(m)(", rlim_max=%"PRIu32, ((const uint32_t*)ptr)[1]);
    }
}

void
print_rlimit_64(Sawyer::Message::Stream &m, const uint8_t *ptr, size_t sz)
{
    assert(16==sz); /* two 64-bit unsigned integers */
    if (0==~((const uint64_t*)ptr)[0]) {
        m <<"rlim_cur=unlimited";
    } else {
        m <<"rlim_cur=" <<((const uint64_t*)ptr)[0];
    }
    if (0==~((const uint64_t*)ptr)[1]) {
        m <<", rlim_max=unlimited";
    } else {
        m <<", rlim_max=" <<((const uint64_t*)ptr)[1];
    }
}

void
print_kernel_stat_32(Sawyer::Message::Stream &m, const uint8_t *_sb, size_t sz)
{
    assert(sz==sizeof(kernel_stat_32));
    const kernel_stat_32 *sb = (const kernel_stat_32*)_sb;
    mfprintf(m)("dev=%"PRIu64", ino=%"PRIu64", mode=", sb->dev, sb->ino);
    Printer::print_flags(m, file_mode_flags, sb->mode);
    mfprintf(m)(", nlink=%"PRIu32", uid=%"PRIu32", gid=%"PRIu32", rdev=%"PRIu64", size=%"PRIu64
                ", blksz=%"PRIu32", blocks=%"PRIu64", ...",
                sb->nlink, sb->user, sb->group, sb->rdev, sb->size,
                sb->blksize, sb->nblocks);
}

void
print_kernel_stat_64(Sawyer::Message::Stream &m, const uint8_t *_sb, size_t sz) {
    ASSERT_require(sz == sizeof(kernel_stat_64));
    const kernel_stat_64 *sb = (const kernel_stat_64*)_sb;
    m <<"dev=" <<sb->dev <<", ino=" <<sb->ino <<", mode=";
    Printer::print_flags(m, file_mode_flags, sb->mode);
    m <<", nlink=" <<sb->nlink <<", uid=" <<sb->user <<", gid=" <<sb->group <<", rdev=" <<sb->rdev
      <<", size=" <<sb->size <<", blksz=" <<sb->blksize <<", blocks=" <<sb->nblocks <<"...";
}

void
print_timespec_32(Sawyer::Message::Stream &m, const uint8_t *_ts, size_t sz)
{
    assert(sz==sizeof(timespec_32));
    const timespec_32 *ts = (const timespec_32*)_ts;
    m <<"sec=";
    Printer::print_time(m, ts->tv_sec);
    m <<", nsec=" <<ts->tv_nsec;
}

void
print_timespec_64(Sawyer::Message::Stream &m, const uint8_t *_ts, size_t sz)
{
    assert(sz==sizeof(timespec_32));
    const timespec_64 *ts = (const timespec_64*)_ts;
    m <<"sec=";
    Printer::print_time(m, ts->tv_sec);
    m <<", nsec=" <<ts->tv_nsec;
}

void
print_timeval_32(Sawyer::Message::Stream &m, const uint8_t *_tv, size_t sz)
{
    assert(sz==sizeof(timeval_32));
    const timeval_32 *tv = (const timeval_32*)_tv;
    m <<"sec=" <<tv->tv_sec <<", usec=" <<tv->tv_usec;
}

void
print_timeval(Sawyer::Message::Stream &m, const uint8_t *_tv, size_t sz)
{
    assert(sz==sizeof(timeval));
    const timeval *tv = (const timeval*)_tv;
    m <<"sec=" <<tv->tv_sec <<", usec=" <<tv->tv_usec;
}

void
print_sigaction_32(Sawyer::Message::Stream &m, const uint8_t *_sa, size_t sz)
{
    assert(sz==sizeof(sigaction_32));
    const sigaction_32 *sa = (const sigaction_32*)_sa;
    m <<"handler=" <<addrToString(sa->handler_va) <<", flags=";
    Printer::print_flags(m, signal_flags, sa->flags);
    m <<", restorer=" <<addrToString(sa->restorer_va);
    m <<", mask=" <<addrToString(sa->mask);
}

void
print_sigaction_64(Sawyer::Message::Stream &m, const uint8_t *_sa, size_t sz)
{
    assert(sz==sizeof(sigaction_64));
    const sigaction_64 *sa = (const sigaction_64*)_sa;
    m <<"handler=" <<addrToString(sa->handler_va) <<", flags=";
    Printer::print_flags(m, signal_flags, sa->flags);
    m <<", restorer=" <<addrToString(sa->restorer_va);
    m <<", mask=" <<addrToString(sa->mask);
}

void
print_dentries_32(Sawyer::Message::Stream &m, const uint8_t *sa, size_t sz)
{
    if (0 == sz) {
        m <<"empty";
    } else {
        m <<"\n";
        for (size_t i=0; (i+1)*sizeof(dirent_32) <= sz; ++i) {
            const dirent_32 *d = (const dirent_32*)(sa + i*sizeof(dirent_32));
            int type = ((const uint8_t*)d)[d->d_reclen - 1];
            const char *name = (const char*)d + d->d_reclen;
            m <<"        dentry[" <<i <<"]: ino=" <<d->d_ino <<", off=" <<d->d_off
              <<", reclen=" <<d->d_reclen <<", type=" <<type <<", name=\"" <<StringUtility::cEscape(name) <<"\"\n";
        }
    }
}

void
print_dentries_64(Sawyer::Message::Stream &m, const uint8_t *sa, size_t sz)
{
    if (0 == sz) {
        m <<"empty";
    } else {
        m <<"\n";
        for (size_t i=0; (i+1)*sizeof(dirent_64) <= sz; ++i) {
            const dirent_64 *d = (const dirent_64*)(sa + i*sizeof(dirent_64));
            int type = ((const uint8_t*)d)[d->d_reclen - 1];
            const char *name = (const char*)d + d->d_reclen;
            m <<"        dentry[" <<i <<"]: ino=" <<d->d_ino <<", off=" <<d->d_off
              <<", reclen=" <<d->d_reclen <<", type=" <<type <<", name=\"" <<StringUtility::cEscape(name) <<"\"\n";
        }
    }
}

void
print_dentries64_32(Sawyer::Message::Stream &m, const uint8_t *sa, size_t sz)
{
    if (0 == sz) {
        m <<"empty";
    } else {
        m <<"\n";
        for (size_t i=0; (i+1)*sizeof(dirent64_32) <= sz; ++i) {
            const dirent64_32 *d = (const dirent64_32*)(sa + i*sizeof(dirent64_32));
            const char *name = (const char*)d + d->d_reclen;
            m <<"        dentry[" <<i <<"]: ino=" <<d->d_ino <<", off=" <<d->d_off
              <<", reclen=" <<d->d_reclen <<", type=" <<d->d_type <<", name=\"" <<StringUtility::cEscape(name) <<"\"\n";
        }
    }
}

void
print_bitvec(Sawyer::Message::Stream &m, const uint8_t *vec, size_t sz)
{
    size_t nset=0;
    for (size_t i=0; i<sz; i++) {
        for (size_t j=0; j<8; j++) {
            if (vec[i] & (1<<j)) {
                mfprintf(m)("%s%zu", nset++?",":"", i*8+j);
            }
        }
    }
}

void
print_SigSet(Sawyer::Message::Stream &m, const uint8_t *vec, size_t sz)
{
    int nsigs=0;
    for (size_t i=0; i<sz; i++) {
        for (size_t j=0; j<8; j++) {
            if (vec[i] & (1<<j)) {
                uint32_t signo = i*8+j + 1;
                if (nsigs++)
                    m <<",";
                Printer::print_enum(m, signal_names, signo);
            }
        }
    }
}

void
print_stack_32(Sawyer::Message::Stream &m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(stack_32)==sz);
    const stack_32 *v = (const stack_32*)_v;
    mfprintf(m)("sp=0x%08"PRIx32", flags=", v->ss_sp);
    Printer::print_flags(m, stack_flags, v->ss_flags);
    mfprintf(m)(", sz=%"PRIu32, v->ss_size);
}

void
print_flock_32(Sawyer::Message::Stream &m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(flock_32)==sz);
    const flock_32 *v = (const flock_32*)_v;
    m <<"type=";
    Printer::print_enum(m, flock_types, v->l_type);
    m <<", whence=";
    Printer::print_enum(m, seek_whence, v->l_whence);
    m <<", start=" <<v->l_start <<", len=" <<v->l_len <<", pid=" <<v->l_pid;
}

void
print_flock_64(Sawyer::Message::Stream &m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(flock_64)==sz);
    const flock_64 *v = (const flock_64*)_v;
    m <<"type=";
    Printer::print_enum(m, flock_types, v->l_type);
    m <<", whence=";
    Printer::print_enum(m, seek_whence, v->l_whence);
    m <<", start=" <<v->l_start <<", len=" <<v->l_len <<", pid=" <<v->l_pid;
}

void
print_flock64_32(Sawyer::Message::Stream &m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(flock64_32)==sz);
    const flock64_32 *v = (const flock64_32*)_v;
    m <<"type=";
    Printer::print_enum(m, flock_types, v->l_type);
    m <<", whence=";
    Printer::print_enum(m, seek_whence, v->l_whence);
    m <<", start=" <<v->l_start <<", len=" <<v->l_len <<", pid=" <<v->l_pid;
}

void
print_statfs_32(Sawyer::Message::Stream &m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(statfs_32)==sz);
    const statfs_32 *v = (const statfs_32*)_v;
    mfprintf(m)("type=%"PRIu32", bsize=%"PRIu32", blocks=%"PRIu32", bfree=%"PRIu32", bavail=%"PRIu32", files=%"PRIu32
                ", ffree=%"PRIu32", fsid=[%"PRIu32",%"PRIu32"], namelen=%"PRIu32", frsize=%"PRIu32", flags=%"PRIu32
                ", spare=[%"PRIu32",%"PRIu32",%"PRIu32",%"PRIu32"]",
                v->f_type, v->f_bsize, v->f_blocks, v->f_bfree, v->f_bavail, v->f_files,
                v->f_ffree, v->f_fsid[0], v->f_fsid[1], v->f_namelen, v->f_frsize, v->f_flags,
                v->f_spare[0], v->f_spare[1], v->f_spare[2], v->f_spare[3]);
}

void
print_statfs_64(Sawyer::Message::Stream &m, const uint8_t *_v, size_t sz) {
    assert(sizeof(statfs_64)==sz);
    const statfs_64 *v = (const statfs_64*)_v;
    m <<"type=" <<v->f_type
      <<", bsize=" <<v->f_bsize
      <<", blocks=" <<v->f_blocks
      <<", bfree=" <<v->f_bfree
      <<", bavail=" <<v->f_bavail
      <<", files=" <<v->f_files
      <<", ffree=" <<v->f_ffree
      <<", fsid=[" <<v->f_fsid[0] <<", " <<v->f_fsid[1] <<"]"
      <<", namelen=" <<v->f_namelen
      <<", frsize=" <<v->f_frsize
      <<", flags=" <<v->f_flags
      <<", spare=[" <<v->f_spare[0] <<", " <<v->f_spare[1] <<", " <<v->f_spare[2] <<", " <<v->f_spare[3] <<"]";
}

void
print_statfs64_32(Sawyer::Message::Stream &m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(statfs64_32)==sz);
    const statfs64_32 *v = (const statfs64_32*)_v;
    mfprintf(m)("type=%"PRIu32", bsize=%"PRIu32", blocks=%"PRIu64", bfree=%"PRIu64", bavail=%"PRIu64", files=%"PRIu64
                ", ffree=%"PRIu64", fsid=[%"PRIu32",%"PRIu32"], namelen=%"PRIu32", frsize=%"PRIu32", flags=%"PRIu32
                ", spare=[%"PRIu32",%"PRIu32",%"PRIu32",%"PRIu32"]",
                v->f_type, v->f_bsize, v->f_blocks, v->f_bfree, v->f_bavail, v->f_files,
                v->f_ffree, v->f_fsid[0], v->f_fsid[1], v->f_namelen, v->f_frsize, v->f_flags,
                v->f_spare[0], v->f_spare[1], v->f_spare[2], v->f_spare[3]);
}

void
print_statfs(Sawyer::Message::Stream &m, const uint8_t *_v, size_t sz) {
    assert(sizeof(struct statfs)==sz);
    const struct statfs *v = (const struct statfs*)_v;
    m <<"type=" <<v->f_type
      <<", bsize=" <<v->f_bsize
      <<", blocks=" <<v->f_blocks
      <<", bfree=" <<v->f_bfree
      <<", bavail=" <<v->f_bavail
      <<", files=" <<v->f_files
      <<", ffree=" <<v->f_ffree
      <<", fsid=[" <<v->f_fsid.__val[0] <<", " <<v->f_fsid.__val[1] <<"]"
      <<", namelen=" <<v->f_namelen
      <<", frsize=" <<v->f_frsize
      <<", flags=" <<v->f_flags
      <<", spare=[" <<v->f_spare[0] <<", " <<v->f_spare[1] <<", " <<v->f_spare[2] <<", " <<v->f_spare[3] <<"]";
}

void
print_robust_list_head_32(Sawyer::Message::Stream &m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(robust_list_head_32)==sz);
    const robust_list_head_32 *v = (const robust_list_head_32*)_v;
    m <<"next_va=" <<addrToString(v->next_va)
      <<", futex_offset=" <<toHex(v->futex_offset)
      <<", pending_va=" <<addrToString(v->pending_va);
}

void
print_robust_list_head_64(Sawyer::Message::Stream &m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(robust_list_head_32)==sz);
    const robust_list_head_64 *v = (const robust_list_head_64*)_v;
    m <<"next_va=" <<addrToString(v->next_va)
      <<", futex_offset=" <<toHex(v->futex_offset)
      <<", pending_va=" <<addrToString(v->pending_va);
}

void
print_ipc64_perm_32(Sawyer::Message::Stream &m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(ipc64_perm_32)==sz);
    const ipc64_perm_32 *v = (const ipc64_perm_32*)_v;
    mfprintf(m)("key=%"PRId32", uid=%"PRId32", gid=%"PRId32", cuid=%"PRId32", cgid=%"PRId32
                ", mode=0%03"PRIo16", seq=%"PRIu16,
                v->key, v->uid, v->gid, v->cuid, v->cgid, v->mode, v->seq);
}

void
print_msqid64_ds_32(Sawyer::Message::Stream &m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(msqid64_ds_32)==sz);
    const msqid64_ds_32 *v = (const msqid64_ds_32*)_v;
    
    m <<"msg_perm={";
    print_ipc64_perm_32(m, _v, sizeof(v->msg_perm));
    mfprintf(m)("}, stime=%"PRId32", rtime=%"PRId32", ctime=%"PRId32
                ", cbytes=%"PRIu32", qnum=%"PRIu32", qbytes=%"PRIu32
                ", lspid=%"PRId32", lrpid=%"PRId32,
                v->msg_stime, v->msg_rtime, v->msg_ctime,
                v->msg_cbytes, v->msg_qnum, v->msg_qbytes,
                v->msg_lspid, v->msg_lrpid);
}

void
print_ipc_kludge_32(Sawyer::Message::Stream &m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(ipc_kludge_32)==sz);
    const ipc_kludge_32 *v = (const ipc_kludge_32*)_v;
    mfprintf(m)("msgp=0x%08"PRIx32", msgtype=%"PRId32, v->msgp, v->msgtyp);
}

void
print_shmid64_ds_32(Sawyer::Message::Stream &m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(shmid64_ds_32)==sz);
    const shmid64_ds_32 *v = (const shmid64_ds_32*)_v;

    m <<"shm_perm={";
    print_ipc64_perm_32(m, _v, sizeof(v->shm_perm));
    mfprintf(m)("}, segsz=%"PRIu32
                ", atime=%"PRId32", dtime=%"PRId32", ctime=%"PRId32
                ", cpid=%"PRId32", lpid=%"PRId32", nattch=%"PRIu32,
                v->shm_segsz,
                v->shm_atime, v->shm_dtime, v->shm_ctime,
                v->shm_cpid, v->shm_lpid, v->shm_nattch);
}

void
print_shm_info_32(Sawyer::Message::Stream &m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(shm_info_32)==sz);
    const shm_info_32 *v = (const shm_info_32*)_v;

    mfprintf(m)("used_ids=%"PRId32", shm_tot=%"PRIu32", shm_rss=%"PRIu32", shm_swp=%"PRIu32
                ", swap_attempts=%"PRIu32", swap_successes=%"PRIu32,
                v->used_ids, v->shm_tot, v->shm_rss, v->shm_swp,
                v->swap_attempts, v->swap_successes);
}

void
print_shminfo64_32(Sawyer::Message::Stream &m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(shminfo64_32)==sz);
    const shminfo64_32 *v = (const shminfo64_32*)_v;
    mfprintf(m)("shmmax=%"PRIu32", shmmin=%"PRIu32", shmmni=%"PRIu32", shmseg=%"PRIu32", shmall=%"PRIu32,
                v->shmmax, v->shmmin, v->shmmni, v->shmseg, v->shmall);
}

void
print_pt_regs_32(Sawyer::Message::Stream &m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(pt_regs_32)==sz);
    const pt_regs_32 *v = (const pt_regs_32*)_v;
    mfprintf(m)("bx=0x%"PRIx32", cx=0x%"PRIx32", dx=0x%"PRIx32", si=0x%"PRIx32", di=0x%"PRIx32
                ", bp=0x%"PRIx32", ax=0x%"PRIx32", ds=0x%"PRIx32", es=0x%"PRIx32", fs=0x%"PRIx32
                ", gs=0x%"PRIx32", orig_ax=0x%"PRIx32", ip=0x%"PRIx32", cs=0x%"PRIx32", flags=0x%"PRIx32
                ", sp=0x%"PRIx32", ss=0x%"PRIx32,
                v->bx, v->cx, v->dx, v->si, v->di,
                v->bp, v->ax, v->ds, v->es, v->fs,
                v->gs, v->orig_ax, v->ip, v->cs, v->flags,
                v->sp, v->ss);
}

void
print_pt_regs_64(Sawyer::Message::Stream &m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(pt_regs_64)==sz);
    const pt_regs_64 *v = (const pt_regs_64*)_v;
    mfprintf(m)("r15=0x%"PRIx64", r14=0x%"PRIx64", r13=0x%"PRIx64", r12=0x%"PRIx64", bp=0x%"PRIx64
                ", bx=0x%"PRIx64", r11=0x%"PRIx64", r10=0x%"PRIx64", r9=0x%"PRIx64", r8=0x%"PRIx64
                ", ax=0x%"PRIx64", cx=0x%"PRIx64", dx=0x%"PRIx64", si=0x%"PRIx64", di=0x%"PRIx64
                ", orig_ax=0x%"PRIx64", ip=0x%"PRIx64", cs=0x%"PRIx64", flags=0x%"PRIx64", sp=0x%"PRIx64
                ", ss=0x%"PRIx64,
                v->r15, v->r14, v->r13, v->r12, v->bp,
                v->bx, v->r11, v->r10, v->r9, v->r8,
                v->ax, v->cx, v->dx, v->si, v->di,
                v->orig_ax, v->ip, v->cs, v->flags, v->sp,
                v->ss);
}

void
print_termios_32(Sawyer::Message::Stream &m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(termios_32)==sz);
    const termios_32 *v = (const termios_32*)_v;
    m <<"c_iflag=";
    Printer::print_flags(m, termios_iflags, v->c_iflag);
    m <<", c_oflag=";
    Printer::print_flags(m, termios_oflags, v->c_oflag);
    m <<", c_cflag=";
    Printer::print_flags(m, termios_cflags, v->c_cflag);
    m <<", c_lflag=";
    Printer::print_flags(m, termios_lflags, v->c_lflag);
    mfprintf(m)(", c_line=%u, c_cc=[", v->c_line);
    for (int i=0; i<19; i++)
        mfprintf(m)("%s%u", i?",":"", v->c_cc[i]);
    m <<"]";
}

void
print_winsize_32(Sawyer::Message::Stream &m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(winsize_32)==sz);
    const winsize_32 *v = (const winsize_32*)_v;
    mfprintf(m)("ws_row=%"PRIu16", ws_col=%"PRIu16", ws_xpixel=%"PRIu16", ws_ypixel=%"PRIu16,
                v->ws_row, v->ws_col, v->ws_xpixel, v->ws_ypixel);
}

void
print_exit_status_32(Sawyer::Message::Stream &m, const uint8_t *_v, size_t sz)
{
    assert(4==sz);
    int status = *(int32_t*)_v;

    if (WIFEXITED(status)) {
        mfprintf(m)("exit %d", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        m <<"terminated by ";
        Printer::print_enum(m, signal_names, WTERMSIG(status));
        mfprintf(m)("%s", WCOREDUMP(status)?" with core dump":"");
    } else if (WIFSTOPPED(status)) {
        m <<"stopped by ";
        Printer::print_enum(m, signal_names, WSTOPSIG(status));
    } else {
        mfprintf(m)("0x%08x", status);
    }
}

void
print_SigInfo(Sawyer::Message::Stream &m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(RSIM_SignalHandling::SigInfo)==sz);
    const RSIM_SignalHandling::SigInfo *info = (const RSIM_SignalHandling::SigInfo*)_v;

    m <<"signo=";
    Printer::print_enum(m, signal_names, info->si_signo);
    mfprintf(m)(", errno=%d", info->si_errno);
    switch (info->si_signo) {
        case SIGSEGV:
            m <<", code=";
            Printer::print_enum(m, siginfo_sigsegv_codes, info->si_code);
            m <<", addr=" <<addrToString(info->sigfault.addr);
            break;
        case SIGBUS:
            m <<", code=";
            Printer::print_enum(m, siginfo_sigbus_codes, info->si_code);
            m <<", addr=" <<addrToString(info->sigfault.addr);
            break;
        case SIGILL:
            m <<", code=";
            Printer::print_enum(m, siginfo_sigill_codes, info->si_code);
            m <<", addr=" <<addrToString(info->sigfault.addr);
            break;
        case SIGFPE:
            m <<", code=";
            Printer::print_enum(m, siginfo_sigfpe_codes, info->si_code);
            m <<", addr=" <<addrToString(info->sigfault.addr);
            break;
        case SIGTRAP:
            m <<", code=";
            Printer::print_enum(m, siginfo_sigtrap_codes, info->si_code);
            break;
        case SIGCHLD:
            m <<", code=";
            Printer::print_enum(m, siginfo_sigchld_codes, info->si_code);
            m <<", pid=" <<info->sigchld.pid <<", uid=" <<info->sigchld.uid <<", status=" <<info->sigchld.status
              <<", utime=" <<info->sigchld.utime <<", stime=" <<info->sigchld.stime;
            break;
        case SIGPOLL:
            m <<", code=";
            Printer::print_enum(m, siginfo_sigpoll_codes, info->si_code);
            m <<", band=" <<info->sigpoll.band <<", fd=" <<info->sigpoll.fd;
            break;
        default:
            m <<", code=";
            Printer::print_enum(m, siginfo_generic_codes, info->si_code);
            switch (info->si_code) {
                case SI_TKILL:
                    m <<", pid=" <<info->rt.pid <<", uid=" <<info->rt.uid
                      <<", sigval=" <<addrToString(info->rt.sigval);
                    break;
                case SI_USER:
                    m <<", pid=" <<info->kill.pid <<", uid=" <<info->kill.uid;
                    break;
                default:
                    // no other info?
                    break;
            }
            break;
    }
}

void
print_sched_param_32(Sawyer::Message::Stream &m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(sched_param_32)==sz);
    const sched_param_32 *v = (const sched_param_32*)_v;
    mfprintf(m)("sched_priority=%"PRId32, v->sched_priority);
}

void
print_msghdr_32(Sawyer::Message::Stream &m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(msghdr_32)==sz);
    const msghdr_32 *v = (const msghdr_32*)_v;
    mfprintf(m)("name=0x%08"PRIx32", namelen=%"PRIu32", iov=0x%08"PRIx32", iovlen=%"PRIu32
                ", control=0x%08"PRIx32", controllen=%"PRIu32", flags=",
                v->msg_name, v->msg_namelen, v->msg_iov, v->msg_iovlen,
                v->msg_control, v->msg_controllen);
    Printer::print_flags(m, msghdr_flags, v->msg_flags);
}

void
print_new_utsname_32(Sawyer::Message::Stream &m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(new_utsname_32)==sz);
    const new_utsname_32 *v = (const new_utsname_32*)_v;
    m <<"sysname=";
    Printer::print_string(m, v->sysname);
    m <<", nodename=";
    Printer::print_string(m, v->nodename);
    m <<", release=";
    Printer::print_string(m, v->release);
    m <<", version=";
    Printer::print_string(m, v->version);
    m <<", machine=";
    Printer::print_string(m, v->machine);
    m <<", domainname=";
    Printer::print_string(m, v->domainname);
}

void
print_new_utsname_64(Sawyer::Message::Stream &m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(new_utsname_64)==sz);
    const new_utsname_64 *v = (const new_utsname_64*)_v;
    m <<"sysname=";
    Printer::print_string(m, v->sysname);
    m <<", nodename=";
    Printer::print_string(m, v->nodename);
    m <<", release=";
    Printer::print_string(m, v->release);
    m <<", version=";
    Printer::print_string(m, v->version);
    m <<", machine=";
    Printer::print_string(m, v->machine);
    m <<", domainname=";
    Printer::print_string(m, v->domainname);
}

void
print_mmap_arg_struct_32(Sawyer::Message::Stream &m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(mmap_arg_struct_32)==sz);
    const mmap_arg_struct_32 *v = (const mmap_arg_struct_32*)_v;
    mfprintf(m)("addr=0x%08"PRIx32", len=%"PRId32", prot=", v->addr, v->len);
    Printer::print_flags(m, mmap_pflags, v->prot);
    m <<", flags=";
    Printer::print_flags(m, mmap_mflags, v->flags);
    mfprintf(m)(", fd=%"PRId32", offset=%"PRId32, v->fd, v->offset);
}

void
convert(statfs_32 *g, const statfs64_native *h)
{
    memset(g, 0, sizeof(*g));
    g->f_type     = h->f_type;
    g->f_bsize    = h->f_bsize;
    g->f_blocks   = h->f_blocks;
    g->f_bfree    = h->f_bfree;
    g->f_bavail   = h->f_bavail;
    g->f_files    = h->f_files;
    g->f_ffree    = h->f_ffree;
    g->f_fsid[0]  = h->f_fsid[0];
    g->f_fsid[1]  = h->f_fsid[1];
    g->f_namelen  = h->f_namelen;
    g->f_frsize   = h->f_frsize;
    g->f_flags    = h->f_flags;
    g->f_spare[0] = h->f_spare[0];
    g->f_spare[1] = h->f_spare[1];
    g->f_spare[2] = h->f_spare[2];
    g->f_spare[3] = h->f_spare[3];
}

void
convert(statfs_32 *g, const statfs_native *h)
{
    memset(g, 0, sizeof(*g));
    g->f_type     = h->f_type;
    g->f_bsize    = h->f_bsize;
    g->f_blocks   = h->f_blocks;
    g->f_bfree    = h->f_bfree;
    g->f_bavail   = h->f_bavail;
    g->f_files    = h->f_files;
    g->f_ffree    = h->f_ffree;
    g->f_fsid[0]  = h->f_fsid[0];
    g->f_fsid[1]  = h->f_fsid[1];
    g->f_namelen  = h->f_namelen;
    g->f_frsize   = h->f_frsize;
    g->f_flags    = h->f_flags;
    g->f_spare[0] = h->f_spare[0];
    g->f_spare[1] = h->f_spare[1];
    g->f_spare[2] = h->f_spare[2];
    g->f_spare[3] = h->f_spare[3];
}

void
convert(statfs64_32 *g, const statfs64_native *h)
{
    memset(g, 0, sizeof(*g));
    g->f_type     = h->f_type;
    g->f_bsize    = h->f_bsize;
    g->f_blocks   = h->f_blocks;
    g->f_bfree    = h->f_bfree;
    g->f_bavail   = h->f_bavail;
    g->f_files    = h->f_files;
    g->f_ffree    = h->f_ffree;
    g->f_fsid[0]  = h->f_fsid[0];
    g->f_fsid[1]  = h->f_fsid[1];
    g->f_namelen  = h->f_namelen;
    g->f_frsize   = h->f_frsize;
    g->f_flags    = h->f_flags;
    g->f_spare[0] = h->f_spare[0];
    g->f_spare[1] = h->f_spare[1];
    g->f_spare[2] = h->f_spare[2];
    g->f_spare[3] = h->f_spare[3];
}

void
convert(statfs64_32 *g, const statfs_native *h)
{
    memset(g, 0, sizeof(*g));
    g->f_type     = h->f_type;
    g->f_bsize    = h->f_bsize;
    g->f_blocks   = h->f_blocks;
    g->f_bfree    = h->f_bfree;
    g->f_bavail   = h->f_bavail;
    g->f_files    = h->f_files;
    g->f_ffree    = h->f_ffree;
    g->f_fsid[0]  = h->f_fsid[0];
    g->f_fsid[1]  = h->f_fsid[1];
    g->f_namelen  = h->f_namelen;
    g->f_frsize   = h->f_frsize;
    g->f_flags    = h->f_flags;
    g->f_spare[0] = h->f_spare[0];
    g->f_spare[1] = h->f_spare[1];
    g->f_spare[2] = h->f_spare[2];
    g->f_spare[3] = h->f_spare[3];
}

void
print_sockaddr(Sawyer::Message::Stream &m, const uint8_t *addr, size_t addrlen) {
    if (addrlen<2) {
        m <<"too short";
    } else {
        uint16_t family = *(uint16_t*)addr;
        if (family != AF_INET && family != AF_INET6) {
            m <<"family=" <<family;
        } else if (addrlen < 8) {
            m <<"too short";
        } else {
            int port = ntohs(*(uint16_t*)(addr+2));
            char s[INET_ADDRSTRLEN];
            if (inet_ntop(family, addr+4, s, sizeof s))
                m <<s <<":" <<port;
        }
    }
}

void
print_sysinfo_32(Sawyer::Message::Stream &m, const uint8_t *_v, size_t sz) {
    ASSERT_require(sizeof(sysinfo_32)==sz);
    const sysinfo_32 *v = (const sysinfo_32*)_v;
    m <<"uptime=" <<v->uptime
      <<", loads=[" <<v->loads[0] <<", " <<v->loads[1] <<", " <<v->loads[2] <<"]"
      <<", totalram=" <<v->totalram
      <<", freeram=" <<v->freeram
      <<", sharedram=" <<v->sharedram
      <<", bufferram=" <<v->bufferram
      <<", totalswap=" <<v->totalswap
      <<", freeswap=" <<v->freeswap
      <<", procs=" <<v->procs
      <<", totalhigh=" <<v->totalhigh
      <<", freehigh=" <<v->freehigh
      <<", mem_unit=" <<v->mem_unit;
}

void
print_sysinfo_64(Sawyer::Message::Stream &m, const uint8_t *_v, size_t sz) {
    ASSERT_require(sizeof(sysinfo_64)==sz);
    const sysinfo_64 *v = (const sysinfo_64*)_v;
    m <<"uptime=" <<v->uptime
      <<", loads=[" <<v->loads[0] <<", " <<v->loads[1] <<", " <<v->loads[2] <<"]"
      <<", totalram=" <<v->totalram
      <<", freeram=" <<v->freeram
      <<", sharedram=" <<v->sharedram
      <<", bufferram=" <<v->bufferram
      <<", totalswap=" <<v->totalswap
      <<", freeswap=" <<v->freeswap
      <<", procs=" <<v->procs
      <<", totalhigh=" <<v->totalhigh
      <<", freehigh=" <<v->freehigh
      <<", mem_unit=" <<v->mem_unit;
}




#endif /* ROSE_ENABLE_SIMULATOR */

