#include "sage3basic.h"
#include "RSIM_Common.h"

int
print_user_desc_32(FILE *f, const uint8_t *_ud, size_t sz)
{
    const user_desc_32 *ud = (const user_desc_32*)_ud;
    assert(sizeof(*ud)==sz);

    const char *content_type = "unknown";
    switch (ud->contents) {
        case 0: content_type = "data"; break;
        case 1: content_type = "stack"; break;
        case 2: content_type = "code"; break;
    }

    return fprintf(f, "entry=%d, base=0x%08lx, limit=0x%08lx, %s, %s, %s, %s, %s, %s",
                   (int)ud->entry_number, (unsigned long)ud->base_addr, (unsigned long)ud->limit,
                   ud->seg_32bit ? "32bit" : "16bit",
                   content_type, ud->read_exec_only ? "read_exec" : "writable",
                   ud->limit_in_pages ? "page_gran" : "byte_gran",
                   ud->seg_not_present ? "not_present" : "present",
                   ud->useable ? "usable" : "not_usable");
}

int
print_int_32(FILE *f, const uint8_t *ptr, size_t sz)
{
    assert(4==sz);
    return fprintf(f, "%"PRId32, *(const int32_t*)ptr);
}

int
print_rlimit(FILE *f, const uint8_t *ptr, size_t sz)
{
    assert(8==sz); /* two 32-bit unsigned integers */
    int retval = 0;
    if (0==~((const uint32_t*)ptr)[0]) {
        retval += fprintf(f, "rlim_cur=unlimited");
    } else {
        retval += fprintf(f, "rlim_cur=%"PRIu32, ((const uint32_t*)ptr)[0]);
    }
    if (0==~((const uint32_t*)ptr)[1]) {
        retval += fprintf(f, ", rlim_max=unlimited");
    } else {
        retval += fprintf(f, ", rlim_max=%"PRIu32, ((const uint32_t*)ptr)[1]);
    }
    return retval;
}

int
print_kernel_stat_32(FILE *f, const uint8_t *_sb, size_t sz)
{
    assert(sz==sizeof(kernel_stat_32));
    const kernel_stat_32 *sb = (const kernel_stat_32*)_sb;
    int retval = 0;
    retval += fprintf(f, "dev=%"PRIu64", ino=%"PRIu64", mode=", sb->dev, sb->ino);
    retval += print_flags(f, file_mode_flags, sb->mode);
    retval += fprintf(f, ", nlink=%"PRIu32", uid=%"PRIu32", gid=%"PRIu32", rdev=%"PRIu64", size=%"PRIu64
                      ", blksz=%"PRIu32", blocks=%"PRIu64", ...",
                      sb->nlink, sb->user, sb->group, sb->rdev, sb->size,
                      sb->blksize, sb->nblocks);
    return retval;
}

int
print_timespec_32(FILE *f, const uint8_t *_ts, size_t sz)
{
    assert(sz==sizeof(timespec_32));
    const timespec_32 *ts = (const timespec_32*)_ts;
    return fprintf(f, "sec=%"PRId32", nsec=%"PRId32, ts->tv_sec, ts->tv_nsec);
}

int
print_timeval_32(FILE *f, const uint8_t *_tv, size_t sz)
{
    assert(sz==sizeof(timeval_32));
    const timeval_32 *tv = (const timeval_32*)_tv;
    return fprintf(f, "sec=%"PRIu32", usec=%"PRIu32, tv->tv_sec, tv->tv_usec);
}

int
print_sigaction_32(FILE *f, const uint8_t *_sa, size_t sz)
{
    assert(sz==sizeof(sigaction_32));
    const sigaction_32 *sa = (const sigaction_32*)_sa;
    return (fprintf(f, "handler=0x%08"PRIx32", flags=", sa->handler_va) +
            print_flags(f, signal_flags, sa->flags) +
            fprintf(f, ", restorer=0x%08"PRIx32", mask=0x%016"PRIx64, sa->restorer_va, sa->mask));
}

int
print_dentries_helper(FILE *f, const uint8_t *_sa, size_t sz, size_t wordsize)
{
    if (0==sz)
        return fprintf(f, "empty");

    int retval = fprintf(f, "\n");
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
        retval += fprintf(f,
                          "        dentry[%3zu]: ino=%-8"PRIu64" next_offset=%-8"PRIu64" reclen=%-3d"
                          " type=%-3d name=\"%s\"\n", i, d_ino, d_off, d_reclen, d_type, d_name);
        offset = d_off;
        if (0==offset) break;
    }
    return retval + fprintf(f, "    ");
}

int
print_dentries_32(FILE *f, const uint8_t *sa, size_t sz)
{
    return print_dentries_helper(f, sa, sz, 4);
}

int
print_dentries_64(FILE *f, const uint8_t *sa, size_t sz)
{
    return print_dentries_helper(f, sa, sz, 8);
}

int
print_bitvec(FILE *f, const uint8_t *vec, size_t sz)
{
    int result=0;
    for (size_t i=0; i<sz; i++) {
        for (size_t j=0; j<8; j++) {
            if (vec[i] & (1<<j)) {
                result += fprintf(f, "%s%zu", result?",":"", i*8+j);
            }
        }
    }
    return result;
}

int
print_sigmask(FILE *f, const uint8_t *vec, size_t sz)
{
    int result=0, nsigs=0;
    for (size_t i=0; i<sz; i++) {
        for (size_t j=0; j<8; j++) {
            if (vec[i] & (1<<j)) {
                uint32_t signo = i*8+j + 1;
                if (nsigs++) result += fprintf(f, ",");
                result += print_enum(f, signal_names, signo);
            }
        }
    }
    return result;
}

int
print_stack_32(FILE *f, const uint8_t *_v, size_t sz)
{
    assert(sizeof(stack_32)==sz);
    const stack_32 *v = (const stack_32*)_v;
    int result = fprintf(f, "sp=0x%08"PRIx32", flags=", v->ss_sp);
    result += print_flags(f, stack_flags, v->ss_flags);
    result += fprintf(f, ", sz=%"PRIu32, v->ss_size);
    return result;
}

int
print_flock_32(FILE *f, const uint8_t *_v, size_t sz)
{
    assert(sizeof(flock_32)==sz);
    const flock_32 *v = (const flock_32*)_v;
    int retval = fprintf(f, "type=");
    retval += print_enum(f, flock_types, v->l_type);
    retval += fprintf(f, ", whence=");
    retval += print_enum(f, seek_whence, v->l_whence);
    retval += fprintf(f, ", start=%"PRId32", len=%"PRId32", pid=%"PRId32, v->l_start, v->l_len, v->l_pid);
    return retval;
}

int
print_flock64_32(FILE *f, const uint8_t *_v, size_t sz)
{
    assert(sizeof(flock64_32)==sz);
    const flock64_32 *v = (const flock64_32*)_v;
    int retval = fprintf(f, "type=");
    retval += print_enum(f, flock_types, v->l_type);
    retval += fprintf(f, ", whence=");
    retval += print_enum(f, seek_whence, v->l_whence);
    retval += fprintf(f, ", start=%"PRId64", len=%"PRId64", pid=%"PRId32, v->l_start, v->l_len, v->l_pid);
    return retval;
}

int
print_statfs_32(FILE *f, const uint8_t *_v, size_t sz)
{
    assert(sizeof(statfs_32)==sz);
    const statfs_32 *v = (const statfs_32*)_v;
    return fprintf(f, "type=%"PRIu32", bsize=%"PRIu32", blocks=%"PRIu32", bfree=%"PRIu32", bavail=%"PRIu32", files=%"PRIu32
                   ", ffree=%"PRIu32", fsid=[%"PRIu32",%"PRIu32"], namelen=%"PRIu32", frsize=%"PRIu32", flags=%"PRIu32
                   ", spare=[%"PRIu32",%"PRIu32",%"PRIu32",%"PRIu32"]",
                   v->f_type, v->f_bsize, v->f_blocks, v->f_bfree, v->f_bavail, v->f_files,
                   v->f_ffree, v->f_fsid[0], v->f_fsid[1], v->f_namelen, v->f_frsize, v->f_flags,
                   v->f_spare[0], v->f_spare[1], v->f_spare[2], v->f_spare[3]);
}

int
print_statfs64_32(FILE *f, const uint8_t *_v, size_t sz)
{
    assert(sizeof(statfs64_32)==sz);
    const statfs64_32 *v = (const statfs64_32*)_v;
    return fprintf(f, "type=%"PRIu32", bsize=%"PRIu32", blocks=%"PRIu64", bfree=%"PRIu64", bavail=%"PRIu64", files=%"PRIu64
                   ", ffree=%"PRIu64", fsid=[%"PRIu32",%"PRIu32"], namelen=%"PRIu32", frsize=%"PRIu32", flags=%"PRIu32
                   ", spare=[%"PRIu32",%"PRIu32",%"PRIu32",%"PRIu32"]",
                   v->f_type, v->f_bsize, v->f_blocks, v->f_bfree, v->f_bavail, v->f_files,
                   v->f_ffree, v->f_fsid[0], v->f_fsid[1], v->f_namelen, v->f_frsize, v->f_flags,
                   v->f_spare[0], v->f_spare[1], v->f_spare[2], v->f_spare[3]);
}

int
print_robust_list_head_32(FILE *f, const uint8_t *_v, size_t sz)
{
    assert(sizeof(robust_list_head_32)==sz);
    const robust_list_head_32 *v = (const robust_list_head_32*)_v;
    return fprintf(f, "next=0x%08"PRIx32", futex=0x%08"PRIx32", pending=0x%08"PRIx32,
                   v->next, v->futex_va, v->pending_va);
}

int
print_ipc64_perm_32(FILE *f, const uint8_t *_v, size_t sz)
{
    assert(sizeof(ipc64_perm_32)==sz);
    const ipc64_perm_32 *v = (const ipc64_perm_32*)_v;
    return fprintf(f,
                   "key=%"PRId32", uid=%"PRId32", gid=%"PRId32", cuid=%"PRId32", cgid=%"PRId32
                   ", mode=0%03"PRIo16", seq=%"PRIu16,
                   v->key, v->uid, v->gid, v->cuid, v->cgid, v->mode, v->seq);
}

int
print_msqid64_ds_32(FILE *f, const uint8_t *_v, size_t sz)
{
    assert(sizeof(msqid64_ds_32)==sz);
    const msqid64_ds_32 *v = (const msqid64_ds_32*)_v;
    
    int retval = fprintf(f, "msg_perm={");
    retval += print_ipc64_perm_32(f, _v, sizeof(v->msg_perm));
    retval += fprintf(f, "}");

    retval += fprintf(f,
                      ", stime=%"PRId32", rtime=%"PRId32", ctime=%"PRId32
                      ", cbytes=%"PRIu32", qnum=%"PRIu32", qbytes=%"PRIu32
                      ", lspid=%"PRId32", lrpid=%"PRId32,
                      v->msg_stime, v->msg_rtime, v->msg_ctime,
                      v->msg_cbytes, v->msg_qnum, v->msg_qbytes,
                      v->msg_lspid, v->msg_lrpid);
    return retval;
}

int
print_ipc_kludge_32(FILE *f, const uint8_t *_v, size_t sz)
{
    assert(sizeof(ipc_kludge_32)==sz);
    const ipc_kludge_32 *v = (const ipc_kludge_32*)_v;
    return fprintf(f, "msgp=0x%08"PRIx32", msgtype=%"PRId32, v->msgp, v->msgtyp);
}

int
print_shmid64_ds_32(FILE *f, const uint8_t *_v, size_t sz)
{
    assert(sizeof(shmid64_ds_32)==sz);
    const shmid64_ds_32 *v = (const shmid64_ds_32*)_v;

    int retval = fprintf(f, "shm_perm={");
    retval += print_ipc64_perm_32(f, _v, sizeof(v->shm_perm));
    retval += fprintf(f, "}");

    retval += fprintf(f,
                      ", segsz=%"PRIu32
                      ", atime=%"PRId32", dtime=%"PRId32", ctime=%"PRId32
                      ", cpid=%"PRId32", lpid=%"PRId32", nattch=%"PRIu32,
                      v->shm_segsz,
                      v->shm_atime, v->shm_dtime, v->shm_ctime,
                      v->shm_cpid, v->shm_lpid, v->shm_nattch);
    return retval;
}

int
print_shm_info_32(FILE *f, const uint8_t *_v, size_t sz)
{
    assert(sizeof(shm_info_32)==sz);
    const shm_info_32 *v = (const shm_info_32*)_v;

    return fprintf(f, "used_ids=%"PRId32", shm_tot=%"PRIu32", shm_rss=%"PRIu32", shm_swp=%"PRIu32
                   ", swap_attempts=%"PRIu32", swap_successes=%"PRIu32,
                   v->used_ids, v->shm_tot, v->shm_rss, v->shm_swp,
                   v->swap_attempts, v->swap_successes);
}

int
print_shminfo64_32(FILE *f, const uint8_t *_v, size_t sz)
{
    assert(sizeof(shminfo64_32)==sz);
    const shminfo64_32 *v = (const shminfo64_32*)_v;
    return fprintf(f, "shmmax=%"PRIu32", shmmin=%"PRIu32", shmmni=%"PRIu32", shmseg=%"PRIu32", shmall=%"PRIu32,
                   v->shmmax, v->shmmin, v->shmmni, v->shmseg, v->shmall);
}

int
print_pt_regs_32(FILE *f, const uint8_t *_v, size_t sz)
{
    assert(sizeof(pt_regs_32)==sz);
    const pt_regs_32 *v = (const pt_regs_32*)_v;
    return fprintf(f, "bx=0x%"PRIx32", cx=0x%"PRIx32", dx=0x%"PRIx32", si=0x%"PRIx32", di=0x%"PRIx32
                   ", bp=0x%"PRIx32", ax=0x%"PRIx32", ds=0x%"PRIx32", es=0x%"PRIx32", fs=0x%"PRIx32
                   ", gs=0x%"PRIx32", orig_ax=0x%"PRIx32", ip=0x%"PRIx32", cs=0x%"PRIx32", flags=0x%"PRIx32
                   ", sp=0x%"PRIx32", ss=0x%"PRIx32,
                   v->bx, v->cx, v->dx, v->si, v->di,
                   v->bp, v->ax, v->ds, v->es, v->fs,
                   v->gs, v->orig_ax, v->ip, v->cs, v->flags,
                   v->sp, v->ss);
}

int
print_termios_32(FILE *f, const uint8_t *_v, size_t sz)
{
    assert(sizeof(termios_32)==sz);
    const termios_32 *v = (const termios_32*)_v;
    int retval = 0;
    retval += fprintf(f, "c_iflag=");
    retval += print_flags(f, termios_iflags, v->c_iflag);
    retval += fprintf(f, ", c_oflag=");
    retval += print_flags(f, termios_oflags, v->c_oflag);
    retval += fprintf(f, ", c_cflag=");
    retval += print_flags(f, termios_cflags, v->c_cflag);
    retval += fprintf(f, ", c_lflag=");
    retval += print_flags(f, termios_lflags, v->c_lflag);
    retval += fprintf(f, ", c_line=%u, c_cc=[", v->c_line);
    for (int i=0; i<19; i++)
        retval += fprintf(f, "%s%u", i?",":"", v->c_cc[i]);
    retval += fprintf(f, "]");
    return retval;
}

int
print_winsize_32(FILE *f, const uint8_t *_v, size_t sz)
{
    assert(sizeof(winsize_32)==sz);
    const winsize_32 *v = (const winsize_32*)_v;
    return fprintf(f, "ws_row=%"PRIu16", ws_col=%"PRIu16", ws_xpixel=%"PRIu16", ws_ypixel=%"PRIu16,
                   v->ws_row, v->ws_col, v->ws_xpixel, v->ws_ypixel);
}
