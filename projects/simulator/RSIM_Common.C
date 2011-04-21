#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR

#include <sys/wait.h>

unsigned
tracingFacilityBit(TracingFacility tf)
{
    return (unsigned)1 << (unsigned)tf;
}

void
print_user_desc_32(RTS_Message *m, const uint8_t *_ud, size_t sz)
{
    const user_desc_32 *ud = (const user_desc_32*)_ud;
    assert(sizeof(*ud)==sz);

    const char *content_type = "unknown";
    switch (ud->contents) {
        case 0: content_type = "data"; break;
        case 1: content_type = "stack"; break;
        case 2: content_type = "code"; break;
    }

    m->more("entry=%d, base=0x%08lx, limit=0x%08lx, %s, %s, %s, %s, %s, %s",
            (int)ud->entry_number, (unsigned long)ud->base_addr, (unsigned long)ud->limit,
            ud->seg_32bit ? "32bit" : "16bit",
            content_type, ud->read_exec_only ? "read_exec" : "writable",
            ud->limit_in_pages ? "page_gran" : "byte_gran",
            ud->seg_not_present ? "not_present" : "present",
            ud->useable ? "usable" : "not_usable");
}

void
print_int_32(RTS_Message *m, const uint8_t *ptr, size_t sz)
{
    assert(4==sz);
    m->more("%"PRId32, *(const int32_t*)ptr);
}

void
print_rlimit(RTS_Message *m, const uint8_t *ptr, size_t sz)
{
    assert(8==sz); /* two 32-bit unsigned integers */
    if (0==~((const uint32_t*)ptr)[0]) {
        m->more("rlim_cur=unlimited");
    } else {
        m->more("rlim_cur=%"PRIu32, ((const uint32_t*)ptr)[0]);
    }
    if (0==~((const uint32_t*)ptr)[1]) {
        m->more(", rlim_max=unlimited");
    } else {
        m->more(", rlim_max=%"PRIu32, ((const uint32_t*)ptr)[1]);
    }
}

void
print_kernel_stat_32(RTS_Message *m, const uint8_t *_sb, size_t sz)
{
    assert(sz==sizeof(kernel_stat_32));
    const kernel_stat_32 *sb = (const kernel_stat_32*)_sb;
    m->more("dev=%"PRIu64", ino=%"PRIu64", mode=", sb->dev, sb->ino);
    print_flags(m, file_mode_flags, sb->mode);
    m->more(", nlink=%"PRIu32", uid=%"PRIu32", gid=%"PRIu32", rdev=%"PRIu64", size=%"PRIu64
            ", blksz=%"PRIu32", blocks=%"PRIu64", ...",
            sb->nlink, sb->user, sb->group, sb->rdev, sb->size,
            sb->blksize, sb->nblocks);
}

void
print_timespec_32(RTS_Message *m, const uint8_t *_ts, size_t sz)
{
    assert(sz==sizeof(timespec_32));
    const timespec_32 *ts = (const timespec_32*)_ts;
    m->more("sec=%"PRId32", nsec=%"PRId32, ts->tv_sec, ts->tv_nsec);
}

void
print_timeval_32(RTS_Message *m, const uint8_t *_tv, size_t sz)
{
    assert(sz==sizeof(timeval_32));
    const timeval_32 *tv = (const timeval_32*)_tv;
    m->more("sec=%"PRIu32", usec=%"PRIu32, tv->tv_sec, tv->tv_usec);
}

void
print_sigaction_32(RTS_Message *m, const uint8_t *_sa, size_t sz)
{
    assert(sz==sizeof(sigaction_32));
    const sigaction_32 *sa = (const sigaction_32*)_sa;
    m->more("handler=0x%08"PRIx32", flags=", sa->handler_va);
    print_flags(m, signal_flags, sa->flags);
    m->more(", restorer=0x%08"PRIx32", mask=0x%016"PRIx64, sa->restorer_va, sa->mask);
}

void
print_dentries_helper(RTS_Message *m, const uint8_t *_sa, size_t sz, size_t wordsize)
{
    if (0==sz) {
        m->more("empty");
    } else {
        m->more("\n");
        size_t offset = 0;
        for (size_t i=0; offset<sz; i++) {
            uint64_t d_ino, d_off;
            int d_reclen, d_type;
            const char *d_name;
            if (4==wordsize) {
                const dirent32_t *sa = (const dirent32_t*)(_sa+offset);
                d_ino = sa->d_ino;
                d_off = sa->d_off;
                d_reclen = sa->d_reclen;
                d_type = _sa[offset+sa->d_reclen-1];
                d_name = (const char*)_sa + offset + sizeof(*sa);
            } else {
                assert(8==wordsize);
                const dirent64_t *sa = (const dirent64_t*)(_sa+offset);
                d_ino = sa->d_ino;
                d_off = sa->d_off;
                d_reclen = sa->d_reclen;
                d_type = sa->d_type;
                d_name = (const char*)_sa + offset + sizeof(*sa);
            }
            m->more("        dentry[%3zu]: ino=%-8"PRIu64" next_offset=%-8"PRIu64" reclen=%-3d"
                    " type=%-3d name=\"%s\"\n", i, d_ino, d_off, d_reclen, d_type, d_name);
            offset = d_off;
            if (0==offset) break;
        }
        m->more(" ");
    }
}

void
print_dentries_32(RTS_Message *m, const uint8_t *sa, size_t sz)
{
    print_dentries_helper(m, sa, sz, 4);
}

void
print_dentries_64(RTS_Message *m, const uint8_t *sa, size_t sz)
{
    print_dentries_helper(m, sa, sz, 8);
}

void
print_bitvec(RTS_Message *m, const uint8_t *vec, size_t sz)
{
    size_t nset=0;
    for (size_t i=0; i<sz; i++) {
        for (size_t j=0; j<8; j++) {
            if (vec[i] & (1<<j)) {
                m->more("%s%zu", nset++?",":"", i*8+j);
            }
        }
    }
}

void
print_sigmask_32(RTS_Message *m, const uint8_t *vec, size_t sz)
{
    int nsigs=0;
    for (size_t i=0; i<sz; i++) {
        for (size_t j=0; j<8; j++) {
            if (vec[i] & (1<<j)) {
                uint32_t signo = i*8+j + 1;
                if (nsigs++)
                    m->more(",");
                print_enum(m, signal_names, signo);
            }
        }
    }
}

void
print_stack_32(RTS_Message *m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(stack_32)==sz);
    const stack_32 *v = (const stack_32*)_v;
    m->more("sp=0x%08"PRIx32", flags=", v->ss_sp);
    print_flags(m, stack_flags, v->ss_flags);
    m->more(", sz=%"PRIu32, v->ss_size);
}

void
print_flock_32(RTS_Message *m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(flock_32)==sz);
    const flock_32 *v = (const flock_32*)_v;
    m->more("type=");
    print_enum(m, flock_types, v->l_type);
    m->more(", whence=");
    print_enum(m, seek_whence, v->l_whence);
    m->more(", start=%"PRId32", len=%"PRId32", pid=%"PRId32, v->l_start, v->l_len, v->l_pid);
}

void
print_flock64_32(RTS_Message *m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(flock64_32)==sz);
    const flock64_32 *v = (const flock64_32*)_v;
    m->more("type=");
    print_enum(m, flock_types, v->l_type);
    m->more(", whence=");
    print_enum(m, seek_whence, v->l_whence);
    m->more(", start=%"PRId64", len=%"PRId64", pid=%"PRId32, v->l_start, v->l_len, v->l_pid);
}

void
print_statfs_32(RTS_Message *m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(statfs_32)==sz);
    const statfs_32 *v = (const statfs_32*)_v;
    m->more("type=%"PRIu32", bsize=%"PRIu32", blocks=%"PRIu32", bfree=%"PRIu32", bavail=%"PRIu32", files=%"PRIu32
            ", ffree=%"PRIu32", fsid=[%"PRIu32",%"PRIu32"], namelen=%"PRIu32", frsize=%"PRIu32", flags=%"PRIu32
            ", spare=[%"PRIu32",%"PRIu32",%"PRIu32",%"PRIu32"]",
            v->f_type, v->f_bsize, v->f_blocks, v->f_bfree, v->f_bavail, v->f_files,
            v->f_ffree, v->f_fsid[0], v->f_fsid[1], v->f_namelen, v->f_frsize, v->f_flags,
            v->f_spare[0], v->f_spare[1], v->f_spare[2], v->f_spare[3]);
}

void
print_statfs64_32(RTS_Message *m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(statfs64_32)==sz);
    const statfs64_32 *v = (const statfs64_32*)_v;
    m->more("type=%"PRIu32", bsize=%"PRIu32", blocks=%"PRIu64", bfree=%"PRIu64", bavail=%"PRIu64", files=%"PRIu64
            ", ffree=%"PRIu64", fsid=[%"PRIu32",%"PRIu32"], namelen=%"PRIu32", frsize=%"PRIu32", flags=%"PRIu32
            ", spare=[%"PRIu32",%"PRIu32",%"PRIu32",%"PRIu32"]",
            v->f_type, v->f_bsize, v->f_blocks, v->f_bfree, v->f_bavail, v->f_files,
            v->f_ffree, v->f_fsid[0], v->f_fsid[1], v->f_namelen, v->f_frsize, v->f_flags,
            v->f_spare[0], v->f_spare[1], v->f_spare[2], v->f_spare[3]);
}

void
print_robust_list_head_32(RTS_Message *m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(robust_list_head_32)==sz);
    const robust_list_head_32 *v = (const robust_list_head_32*)_v;
    m->more("next_va=0x%08"PRIx32", futex_offset=%"PRId32", pending_va=0x%08"PRIx32,
            v->next_va, v->futex_offset, v->pending_va);
}

void
print_ipc64_perm_32(RTS_Message *m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(ipc64_perm_32)==sz);
    const ipc64_perm_32 *v = (const ipc64_perm_32*)_v;
    m->more("key=%"PRId32", uid=%"PRId32", gid=%"PRId32", cuid=%"PRId32", cgid=%"PRId32
            ", mode=0%03"PRIo16", seq=%"PRIu16,
            v->key, v->uid, v->gid, v->cuid, v->cgid, v->mode, v->seq);
}

void
print_msqid64_ds_32(RTS_Message *m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(msqid64_ds_32)==sz);
    const msqid64_ds_32 *v = (const msqid64_ds_32*)_v;
    
    m->more("msg_perm={");
    print_ipc64_perm_32(m, _v, sizeof(v->msg_perm));
    m->more("}, stime=%"PRId32", rtime=%"PRId32", ctime=%"PRId32
            ", cbytes=%"PRIu32", qnum=%"PRIu32", qbytes=%"PRIu32
            ", lspid=%"PRId32", lrpid=%"PRId32,
            v->msg_stime, v->msg_rtime, v->msg_ctime,
            v->msg_cbytes, v->msg_qnum, v->msg_qbytes,
            v->msg_lspid, v->msg_lrpid);
}

void
print_ipc_kludge_32(RTS_Message *m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(ipc_kludge_32)==sz);
    const ipc_kludge_32 *v = (const ipc_kludge_32*)_v;
    m->more("msgp=0x%08"PRIx32", msgtype=%"PRId32, v->msgp, v->msgtyp);
}

void
print_shmid64_ds_32(RTS_Message *m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(shmid64_ds_32)==sz);
    const shmid64_ds_32 *v = (const shmid64_ds_32*)_v;

    m->more("shm_perm={");
    print_ipc64_perm_32(m, _v, sizeof(v->shm_perm));
    m->more("}, segsz=%"PRIu32
            ", atime=%"PRId32", dtime=%"PRId32", ctime=%"PRId32
            ", cpid=%"PRId32", lpid=%"PRId32", nattch=%"PRIu32,
            v->shm_segsz,
            v->shm_atime, v->shm_dtime, v->shm_ctime,
            v->shm_cpid, v->shm_lpid, v->shm_nattch);
}

void
print_shm_info_32(RTS_Message *m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(shm_info_32)==sz);
    const shm_info_32 *v = (const shm_info_32*)_v;

    m->more("used_ids=%"PRId32", shm_tot=%"PRIu32", shm_rss=%"PRIu32", shm_swp=%"PRIu32
            ", swap_attempts=%"PRIu32", swap_successes=%"PRIu32,
            v->used_ids, v->shm_tot, v->shm_rss, v->shm_swp,
            v->swap_attempts, v->swap_successes);
}

void
print_shminfo64_32(RTS_Message *m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(shminfo64_32)==sz);
    const shminfo64_32 *v = (const shminfo64_32*)_v;
    m->more("shmmax=%"PRIu32", shmmin=%"PRIu32", shmmni=%"PRIu32", shmseg=%"PRIu32", shmall=%"PRIu32,
            v->shmmax, v->shmmin, v->shmmni, v->shmseg, v->shmall);
}

void
print_pt_regs_32(RTS_Message *m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(pt_regs_32)==sz);
    const pt_regs_32 *v = (const pt_regs_32*)_v;
    m->more("bx=0x%"PRIx32", cx=0x%"PRIx32", dx=0x%"PRIx32", si=0x%"PRIx32", di=0x%"PRIx32
            ", bp=0x%"PRIx32", ax=0x%"PRIx32", ds=0x%"PRIx32", es=0x%"PRIx32", fs=0x%"PRIx32
            ", gs=0x%"PRIx32", orig_ax=0x%"PRIx32", ip=0x%"PRIx32", cs=0x%"PRIx32", flags=0x%"PRIx32
            ", sp=0x%"PRIx32", ss=0x%"PRIx32,
            v->bx, v->cx, v->dx, v->si, v->di,
            v->bp, v->ax, v->ds, v->es, v->fs,
            v->gs, v->orig_ax, v->ip, v->cs, v->flags,
            v->sp, v->ss);
}

void
print_termios_32(RTS_Message *m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(termios_32)==sz);
    const termios_32 *v = (const termios_32*)_v;
    m->more("c_iflag=");
    print_flags(m, termios_iflags, v->c_iflag);
    m->more(", c_oflag=");
    print_flags(m, termios_oflags, v->c_oflag);
    m->more(", c_cflag=");
    print_flags(m, termios_cflags, v->c_cflag);
    m->more(", c_lflag=");
    print_flags(m, termios_lflags, v->c_lflag);
    m->more(", c_line=%u, c_cc=[", v->c_line);
    for (int i=0; i<19; i++)
        m->more("%s%u", i?",":"", v->c_cc[i]);
    m->more("]");
}

void
print_winsize_32(RTS_Message *m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(winsize_32)==sz);
    const winsize_32 *v = (const winsize_32*)_v;
    m->more("ws_row=%"PRIu16", ws_col=%"PRIu16", ws_xpixel=%"PRIu16", ws_ypixel=%"PRIu16,
            v->ws_row, v->ws_col, v->ws_xpixel, v->ws_ypixel);
}

void
print_exit_status_32(RTS_Message *m, const uint8_t *_v, size_t sz)
{
    assert(4==sz);
    int status = *(int32_t*)_v;

    if (WIFEXITED(status)) {
        m->more("exit %d", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        m->more("terminated by ");
        print_enum(m, signal_names, WTERMSIG(status));
        m->more("%s", WCOREDUMP(status)?" with core dump":"");
    } else if (WIFSTOPPED(status)) {
        m->more("stopped by ");
        print_enum(m, signal_names, WSTOPSIG(status));
    } else {
        m->more("0x%08x", status);
    }
}

void
print_siginfo_32(RTS_Message *m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(RSIM_SignalHandling::siginfo_32)==sz);
    const RSIM_SignalHandling::siginfo_32 *info = (const RSIM_SignalHandling::siginfo_32*)_v;

    m->more("signo=");
    print_enum(m, signal_names, info->si_signo);
    m->more(", errno=%d", info->si_errno);
    switch (info->si_signo) {
        case SIGSEGV:
            m->more(", code=");
            print_enum(m, siginfo_sigsegv_codes, info->si_code);
            m->more(", addr=0x%08"PRIx32, info->sigfault.addr);
            break;
        case SIGBUS:
            m->more(", code=");
            print_enum(m, siginfo_sigbus_codes, info->si_code);
            m->more(", addr=0x%08"PRIx32, info->sigfault.addr);
            break;
        case SIGILL:
            m->more(", code=");
            print_enum(m, siginfo_sigill_codes, info->si_code);
            m->more(", addr=0x%08"PRIx32, info->sigfault.addr);
            break;
        case SIGFPE:
            m->more(", code=");
            print_enum(m, siginfo_sigfpe_codes, info->si_code);
            m->more(", addr=0x%08"PRIx32, info->sigfault.addr);
            break;
        case SIGTRAP:
            m->more(", code=");
            print_enum(m, siginfo_sigtrap_codes, info->si_code);
            break;
        case SIGCHLD:
            m->more(", code=");
            print_enum(m, siginfo_sigchld_codes, info->si_code);
            m->more(", pid=%d, uid=%u, status=0x%02x, utime=%d, stime=%d",
                    info->sigchld.pid, info->sigchld.uid, info->sigchld.status, info->sigchld.utime, info->sigchld.stime);
            break;
        case SIGPOLL:
            m->more(", code=");
            print_enum(m, siginfo_sigpoll_codes, info->si_code);
            m->more(", band=%d, fd=%d", info->sigpoll.band, info->sigpoll.fd);
            break;
        default:
            m->more(", code=");
            print_enum(m, siginfo_generic_codes, info->si_code);
            switch (info->si_code) {
                case SI_TKILL:
                    m->more(", pid=%d, uid=%d, sigval=%d", info->rt.pid, info->rt.uid, info->rt.sigval);
                    break;
                case SI_USER:
                    m->more(", pid=%d, uid=%d", info->kill.pid, info->kill.uid);
                    break;
                default:
                    // no other info?
                    break;
            }
            break;
    }
}

void
print_sched_param_32(RTS_Message *m, const uint8_t *_v, size_t sz)
{
    assert(sizeof(sched_param_32)==sz);
    const sched_param_32 *v = (const sched_param_32*)_v;
    m->more("sched_priority=%"PRId32, v->sched_priority);
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

#endif /* ROSE_ENABLE_SIMULATOR */

