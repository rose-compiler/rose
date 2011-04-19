/* This file contains Linux-32 system call emulation.  Most of these functions are callbacks and have names like:
 *
 *    RSIM_Linux32::syscall_FOO_enter           -- prints syscall tracing info when the call is entered
 *    RSIM_Linux32::syscall_FOO                 -- implements the system call
 *    RSIM_Linux32::syscall_FOO_leave           -- prints syscall tracing info when the call returns
 */
#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR

#include "RSIM_Linux32.h"

#include <errno.h>
#include <syscall.h>
#include <sys/mman.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>
#include <utime.h>

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <linux/types.h>
#include <linux/dirent.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <linux/unistd.h>
#include <sys/sysinfo.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <grp.h>

/* This leave callback just prints using the "d" format and is used by lots of system calls. */
static void syscall_default_leave(RSIM_Thread *t, int callno);

static void syscall_exit_enter(RSIM_Thread *t, int callno);
static void syscall_exit(RSIM_Thread *t, int callno);
static void syscall_exit_leave(RSIM_Thread *t, int callno);
static void syscall_read_enter(RSIM_Thread *t, int callno);
static void syscall_read(RSIM_Thread *t, int callno);
static void syscall_read_leave(RSIM_Thread*, int callno);
static void syscall_write_enter(RSIM_Thread *t, int callno);
static void syscall_write(RSIM_Thread *t, int callno);
static void syscall_open_enter(RSIM_Thread *t, int callno);
static void syscall_open(RSIM_Thread *t, int callno);
static void syscall_close_enter(RSIM_Thread *t, int callno);
static void syscall_close(RSIM_Thread *t, int callno);
static void syscall_waitpid_enter(RSIM_Thread *t, int callno);
static void syscall_waitpid(RSIM_Thread *t, int callno);
static void syscall_waitpid_leave(RSIM_Thread *t, int callno);
static void syscall_creat_enter(RSIM_Thread *t, int callno);
static void syscall_creat(RSIM_Thread *t, int callno);
static void syscall_link_enter(RSIM_Thread *t, int callno);
static void syscall_link(RSIM_Thread *t, int callno);
static void syscall_unlink_enter(RSIM_Thread *t, int callno);
static void syscall_unlink(RSIM_Thread *t, int callno);
static void syscall_execve_enter(RSIM_Thread *t, int callno);
static void syscall_execve(RSIM_Thread *t, int callno);
static void syscall_chdir_enter(RSIM_Thread *t, int callno);
static void syscall_chdir(RSIM_Thread *t, int callno);
static void syscall_time_enter(RSIM_Thread *t, int callno);
static void syscall_time(RSIM_Thread *t, int callno);
static void syscall_time_leave(RSIM_Thread *t, int callno);
static void syscall_mknod_enter(RSIM_Thread *t, int callno);
static void syscall_mknod(RSIM_Thread *t, int callno);
static void syscall_chmod_enter(RSIM_Thread *t, int callno);
static void syscall_chmod(RSIM_Thread *t, int callno);
static void syscall_lseek_enter(RSIM_Thread *t, int callno);
static void syscall_lseek(RSIM_Thread *t, int callno);
static void syscall_getpid_enter(RSIM_Thread *t, int callno);
static void syscall_getpid(RSIM_Thread *t, int callno);
static void syscall_getuid_enter(RSIM_Thread *t, int callno);
static void syscall_getuid(RSIM_Thread *t, int callno);
static void syscall_alarm_enter(RSIM_Thread *t, int callno);
static void syscall_alarm(RSIM_Thread *t, int callno);
static void syscall_pause_enter(RSIM_Thread *t, int callno);
static void syscall_pause(RSIM_Thread *t, int callno);
static void syscall_pause_leave(RSIM_Thread *t, int callno);
static void syscall_utime_enter(RSIM_Thread *t, int callno);
static void syscall_utime(RSIM_Thread *t, int callno);
static void syscall_access_enter(RSIM_Thread *t, int callno);
static void syscall_access(RSIM_Thread *t, int callno);
static void syscall_sync_enter(RSIM_Thread *t, int callno);
static void syscall_sync(RSIM_Thread *t, int callno);
static void syscall_kill_enter(RSIM_Thread *t, int callno);
static void syscall_kill(RSIM_Thread *t, int callno);
static void syscall_rename_enter(RSIM_Thread *t, int callno);
static void syscall_rename(RSIM_Thread *t, int callno);
static void syscall_mkdir_enter(RSIM_Thread *t, int callno);
static void syscall_mkdir(RSIM_Thread *t, int callno);
static void syscall_rmdir_enter(RSIM_Thread *t, int callno);
static void syscall_rmdir(RSIM_Thread *t, int callno);
static void syscall_dup_enter(RSIM_Thread *t, int callno);
static void syscall_dup(RSIM_Thread *t, int callno);
static void syscall_pipe_enter(RSIM_Thread *t, int callno);
static void syscall_pipe(RSIM_Thread *t, int callno);
static void syscall_brk_enter(RSIM_Thread *t, int callno);
static void syscall_brk(RSIM_Thread *t, int callno);
static void syscall_brk_leave(RSIM_Thread *t, int callno);
static void syscall_getgid_enter(RSIM_Thread *t, int callno);
static void syscall_getgid(RSIM_Thread *t, int callno);
static void syscall_geteuid_enter(RSIM_Thread *t, int callno);
static void syscall_geteuid(RSIM_Thread *t, int callno);
static void syscall_getegid_enter(RSIM_Thread *t, int callno);
static void syscall_getegid(RSIM_Thread *t, int callno);
static void syscall_ioctl_enter(RSIM_Thread *t, int callno);
static void syscall_ioctl(RSIM_Thread *t, int callno);
static void syscall_ioctl_leave(RSIM_Thread *t, int callno);
static void syscall_setpgid_enter(RSIM_Thread *t, int callno);
static void syscall_setpgid(RSIM_Thread *t, int callno);
static void syscall_umask_enter(RSIM_Thread *t, int callno);
static void syscall_umask(RSIM_Thread *t, int callno);
static void syscall_dup2_enter(RSIM_Thread *t, int callno);
static void syscall_dup2(RSIM_Thread *t, int callno);
static void syscall_getppid_enter(RSIM_Thread *t, int callno);
static void syscall_getppid(RSIM_Thread *t, int callno);
static void syscall_getpgrp_enter(RSIM_Thread *t, int callno);
static void syscall_getpgrp(RSIM_Thread *t, int callno);
static void syscall_setrlimit_enter(RSIM_Thread *t, int callno);
static void syscall_setrlimit(RSIM_Thread *t, int callno);
static void syscall_ugetrlimit_enter(RSIM_Thread *t, int callno);
static void syscall_ugetrlimit(RSIM_Thread *t, int callno);
static void syscall_ugetrlimit_leave(RSIM_Thread *t, int callno);
static void syscall_getrlimit_enter(RSIM_Thread *t, int callno);
static void syscall_getrlimit(RSIM_Thread *t, int callno);
static void syscall_getrlimit_leave(RSIM_Thread *t, int callno);
static void syscall_gettimeofday_enter(RSIM_Thread *t, int callno);
static void syscall_gettimeofday(RSIM_Thread *t, int callno);
static void syscall_gettimeofday_leave(RSIM_Thread *t, int callno);
static void syscall_symlink_enter(RSIM_Thread *t, int callno);
static void syscall_symlink(RSIM_Thread *t, int callno);
static void syscall_readlink_enter(RSIM_Thread *t, int callno);
static void syscall_readlink(RSIM_Thread *t, int callno);
static void syscall_munmap_enter(RSIM_Thread *t, int callno);
static void syscall_munmap(RSIM_Thread *t, int callno);
static void syscall_ftruncate_enter(RSIM_Thread *t, int callno);
static void syscall_ftruncate(RSIM_Thread *t, int callno);
static void syscall_fchmod_enter(RSIM_Thread *t, int callno);
static void syscall_fchmod(RSIM_Thread *t, int callno);
static void syscall_fchown_enter(RSIM_Thread *t, int callno);
static void syscall_fchown(RSIM_Thread *t, int callno);
static void syscall_statfs_enter(RSIM_Thread *t, int callno);
static void syscall_statfs(RSIM_Thread *t, int callno);
static void syscall_statfs_leave(RSIM_Thread *t, int callno);
static void syscall_fstatfs_enter(RSIM_Thread *t, int callno);
static void syscall_fstatfs(RSIM_Thread *t, int callno);
static void syscall_fstatfs_leave(RSIM_Thread *t, int callno);
static void syscall_socketcall_enter(RSIM_Thread *t, int callno);
static void syscall_socketcall(RSIM_Thread *t, int callno);
static void syscall_wait4_enter(RSIM_Thread *t, int callno);
static void syscall_wait4(RSIM_Thread *t, int callno);
static void syscall_wait4_leave(RSIM_Thread *t, int callno);
static void syscall_sysinfo_enter(RSIM_Thread *t, int callno);
static void syscall_sysinfo(RSIM_Thread *t, int callno);
static void syscall_ipc_enter(RSIM_Thread *t, int callno);
static void syscall_ipc(RSIM_Thread *t, int callno);
static void syscall_ipc_leave(RSIM_Thread *t, int callno);
static void syscall_fsync_enter(RSIM_Thread *t, int callno);
static void syscall_fsync(RSIM_Thread *t, int callno);
static void syscall_sigreturn_enter(RSIM_Thread *t, int callno);
static void syscall_sigreturn(RSIM_Thread *t, int callno);
static void syscall_sigreturn_leave(RSIM_Thread *t, int callno);
static void syscall_clone_enter(RSIM_Thread *t, int callno);
static void syscall_clone(RSIM_Thread *t, int callno);
static void syscall_clone_leave(RSIM_Thread *t, int callno);
static void syscall_uname_enter(RSIM_Thread *t, int callno);
static void syscall_uname(RSIM_Thread *t, int callno);
static void syscall_fchdir_enter(RSIM_Thread *t, int callno);
static void syscall_fchdir(RSIM_Thread *t, int callno);
static void syscall_mprotect_enter(RSIM_Thread *t, int callno);
static void syscall_mprotect(RSIM_Thread *t, int callno);
static void syscall_mprotect_leave(RSIM_Thread *t, int callno);
static void syscall_llseek_enter(RSIM_Thread *t, int callno);
static void syscall_llseek(RSIM_Thread *t, int callno);
static void syscall_getdents_enter(RSIM_Thread *t, int callno);
static void syscall_getdents(RSIM_Thread *t, int callno);
static void syscall_getdents_leave(RSIM_Thread *t, int callno);
static void syscall_select_enter(RSIM_Thread *t, int callno);
static void syscall_select(RSIM_Thread *t, int callno);
static void syscall_select_leave(RSIM_Thread *t, int callno);
static void syscall_msync_enter(RSIM_Thread *t, int callno);
static void syscall_msync(RSIM_Thread *t, int callno);
static void syscall_writev_enter(RSIM_Thread *t, int callno);
static void syscall_writev(RSIM_Thread *t, int callno);
static void syscall_sched_setparam_enter(RSIM_Thread *t, int callno);
static void syscall_sched_setparam(RSIM_Thread *t, int callno);
static void syscall_sched_getscheduler_enter(RSIM_Thread *t, int callno);
static void syscall_sched_getscheduler(RSIM_Thread *t, int callno);
static void syscall_sched_getscheduler_leave(RSIM_Thread *t, int callno);
static void syscall_sched_get_priority_max_enter(RSIM_Thread *t, int callno);
static void syscall_sched_get_priority_max(RSIM_Thread *t, int callno);
static void syscall_sched_get_priority_min_enter(RSIM_Thread *t, int callno);
static void syscall_sched_get_priority_min(RSIM_Thread *t, int callno);
static void syscall_nanosleep_enter(RSIM_Thread *t, int callno);
static void syscall_nanosleep(RSIM_Thread *t, int callno);
static void syscall_nanosleep_leave(RSIM_Thread *t, int callno);
static void syscall_rt_sigreturn_enter(RSIM_Thread *t, int callno);
static void syscall_rt_sigreturn(RSIM_Thread *t, int callno);
static void syscall_rt_sigreturn_leave(RSIM_Thread *t, int callno);
static void syscall_rt_sigaction_enter(RSIM_Thread *t, int callno);
static void syscall_rt_sigaction(RSIM_Thread *t, int callno);
static void syscall_rt_sigaction_leave(RSIM_Thread *t, int callno);
static void syscall_rt_sigprocmask_enter(RSIM_Thread *t, int callno);
static void syscall_rt_sigprocmask(RSIM_Thread *t, int callno);
static void syscall_rt_sigprocmask_leave(RSIM_Thread *t, int callno);
static void syscall_rt_sigpending_enter(RSIM_Thread *t, int callno);
static void syscall_rt_sigpending(RSIM_Thread *t, int callno);
static void syscall_rt_sigpending_leave(RSIM_Thread *t, int callno);
static void syscall_rt_sigsuspend_enter(RSIM_Thread *t, int callno);
static void syscall_rt_sigsuspend(RSIM_Thread *t, int callno);
static void syscall_rt_sigsuspend_leave(RSIM_Thread *t, int callno);
static void syscall_getcwd_enter(RSIM_Thread *t, int callno);
static void syscall_getcwd(RSIM_Thread *t, int callno);
static void syscall_getcwd_leave(RSIM_Thread *t, int callno);
static void syscall_sigaltstack_enter(RSIM_Thread *t, int callno);
static void syscall_sigaltstack(RSIM_Thread *t, int callno);
static void syscall_sigaltstack_leave(RSIM_Thread *t, int callno);
static void syscall_mmap2_enter(RSIM_Thread *t, int callno);
static void syscall_mmap2(RSIM_Thread *t, int callno);
static void syscall_mmap2_leave(RSIM_Thread *t, int callno);
static void syscall_stat64_enter(RSIM_Thread *t, int callno);
static void syscall_stat64(RSIM_Thread *t, int callno);
static void syscall_stat64_leave(RSIM_Thread *t, int callno);
static void syscall_getuid32_enter(RSIM_Thread *t, int callno);
static void syscall_getuid32(RSIM_Thread *t, int callno);
static void syscall_getgid32_enter(RSIM_Thread *t, int callno);
static void syscall_getgid32(RSIM_Thread *t, int callno);
static void syscall_geteuid32_enter(RSIM_Thread *t, int callno);
static void syscall_geteuid32(RSIM_Thread *t, int callno);
static void syscall_getegid32_enter(RSIM_Thread *t, int callno);
static void syscall_getegid32(RSIM_Thread *t, int callno);
static void syscall_fchown32_enter(RSIM_Thread *t, int callno);
static void syscall_fchown32(RSIM_Thread *t, int callno);
static void syscall_chown_enter(RSIM_Thread *t, int callno);
static void syscall_chown(RSIM_Thread *t, int callno);
static void syscall_getdents64_enter(RSIM_Thread *t, int callno);
static void syscall_getdents64(RSIM_Thread *t, int callno);
static void syscall_getdents64_leave(RSIM_Thread *t, int callno);
static void syscall_fcntl_enter(RSIM_Thread *t, int callno);
static void syscall_fcntl(RSIM_Thread *t, int callno);
static void syscall_fcntl_leave(RSIM_Thread *t, int callno);
static void syscall_gettid_enter(RSIM_Thread *t, int callno);
static void syscall_gettid(RSIM_Thread *t, int callno);
static void syscall_futex_enter(RSIM_Thread *t, int callno);
static void syscall_futex(RSIM_Thread *t, int callno);
static void syscall_sched_getaffinity_enter(RSIM_Thread *t, int callno);
static void syscall_sched_getaffinity(RSIM_Thread *t, int callno);
static void syscall_sched_getaffinity_leave(RSIM_Thread *t, int callno);
static void syscall_set_thread_area_enter(RSIM_Thread *t, int callno);
static void syscall_set_thread_area(RSIM_Thread *t, int callno);
static void syscall_exit_group_enter(RSIM_Thread *t, int callno);
static void syscall_exit_group(RSIM_Thread *t, int callno);
static void syscall_exit_group_leave(RSIM_Thread *t, int callno);
static void syscall_set_tid_address_enter(RSIM_Thread *t, int callno);
static void syscall_set_tid_address(RSIM_Thread *t, int callno);
static void syscall_clock_settime_enter(RSIM_Thread *t, int callno);
static void syscall_clock_settime(RSIM_Thread *t, int callno);
static void syscall_clock_gettime_enter(RSIM_Thread *t, int callno);
static void syscall_clock_gettime(RSIM_Thread *t, int callno);
static void syscall_clock_gettime_leave(RSIM_Thread *t, int callno);
static void syscall_clock_getres_enter(RSIM_Thread *t, int callno);
static void syscall_clock_getres(RSIM_Thread *t, int callno);
static void syscall_clock_getres_leave(RSIM_Thread *t, int callno);
static void syscall_statfs64_enter(RSIM_Thread *t, int callno);
static void syscall_statfs64(RSIM_Thread *t, int callno);
static void syscall_statfs64_leave(RSIM_Thread *t, int callno);
static void syscall_tgkill_enter(RSIM_Thread *t, int callno);
static void syscall_tgkill(RSIM_Thread *t, int callno);
static void syscall_utimes_enter(RSIM_Thread *t, int callno);
static void syscall_utimes(RSIM_Thread *t, int callno);
static void syscall_fchmodat_enter(RSIM_Thread *t, int callno);
static void syscall_fchmodat(RSIM_Thread *t, int callno);
static void syscall_set_robust_list_enter(RSIM_Thread *t, int callno);
static void syscall_set_robust_list(RSIM_Thread *t, int callno);

void
RSIM_Linux32::ctor()
{
#   define SC_REG(NUM, NAME, LEAVE)                                                                                            \
        syscall_define((NUM), syscall_##NAME##_enter, syscall_##NAME, syscall_##LEAVE##_leave);

    /* Warning: use hard-coded values here rather than the __NR_* constants from <sys/unistd.h> because the latter varies
     * according to whether ROSE is compiled for 32- or 64-bit.  We always want the 32-bit syscall numbers here. */
    SC_REG(1,   exit,                           exit);
    SC_REG(3,   read,                           read);
    SC_REG(4,   write,                          default);
    SC_REG(5,   open,                           default);
    SC_REG(6,   close,                          default);
    SC_REG(7,   waitpid,                        waitpid);
    SC_REG(8,   creat,                          default);
    SC_REG(9,   link,                           default);
    SC_REG(10,  unlink,                         default);
    SC_REG(11,  execve,                         default);
    SC_REG(12,  chdir,                          default);
    SC_REG(13,  time,                           time);
    SC_REG(14,  mknod,                          default);
    SC_REG(15,  chmod,                          default);
    SC_REG(19,  lseek,                          default);
    SC_REG(20,  getpid,                         default);
    SC_REG(24,  getuid,                         default);
    SC_REG(27,  alarm,                          default);
    SC_REG(29,  pause,                          pause);
    SC_REG(30,  utime,                          default);
    SC_REG(33,  access,                         default);
    SC_REG(36,  sync,                           default);
    SC_REG(37,  kill,                           default);
    SC_REG(38,  rename,                         default);
    SC_REG(39,  mkdir,                          default);
    SC_REG(40,  rmdir,                          default);
    SC_REG(41,  dup,                            default);
    SC_REG(42,  pipe,                           default);
    SC_REG(45,  brk,                            brk);
    SC_REG(47,  getgid,                         default);
    SC_REG(49,  geteuid,                        default);
    SC_REG(50,  getegid,                        default);
    SC_REG(54,  ioctl,                          ioctl);
    SC_REG(57,  setpgid,                        default);
    SC_REG(60,  umask,                          default);
    SC_REG(63,  dup2,                           default);
    SC_REG(64,  getppid,                        default);
    SC_REG(65,  getpgrp,                        default);
    SC_REG(75,  setrlimit,                      default);
    SC_REG(76,  getrlimit,                      getrlimit);
    SC_REG(78,  gettimeofday,                   gettimeofday);
    SC_REG(83,  symlink,                        default);
    SC_REG(85,  readlink,                       default);               // FIXME: probably needs an explicit leave
    SC_REG(91,  munmap,                         default);
    SC_REG(93,  ftruncate,                      default);
    SC_REG(94,  fchmod,                         default);
    SC_REG(95,  fchown,                         default);
    SC_REG(99,  statfs,                         statfs);
    SC_REG(100, fstatfs,                        fstatfs);
    SC_REG(102, socketcall,                     default);
    SC_REG(114, wait4,                          wait4);
    SC_REG(116, sysinfo,                        default);               // FIXME: probably needs an explicit leave
    SC_REG(117, ipc,                            ipc);
    SC_REG(118, fsync,                          default);
    SC_REG(119, sigreturn,                      sigreturn);
    SC_REG(120, clone,                          clone);
    SC_REG(122, uname,                          default);               // FIXME: probably needs an explicit leave
    SC_REG(125, mprotect,                       mprotect);
    SC_REG(133, fchdir,                         default);
    SC_REG(140, llseek,                         default);
    SC_REG(141, getdents,                       getdents);
    SC_REG(142, select,                         select);
    SC_REG(144, msync,                          default);
    SC_REG(146, writev,                         default);
    SC_REG(155, sched_setparam,                 default);
    SC_REG(157, sched_getscheduler,             sched_getscheduler);
    SC_REG(159, sched_get_priority_max,         default);
    SC_REG(160, sched_get_priority_min,         default);
    SC_REG(162, nanosleep,                      nanosleep);
    SC_REG(173, rt_sigreturn,                   rt_sigreturn);
    SC_REG(174, rt_sigaction,                   rt_sigaction);
    SC_REG(175, rt_sigprocmask,                 rt_sigprocmask);
    SC_REG(176, rt_sigpending,                  rt_sigpending);
    SC_REG(179, rt_sigsuspend,                  rt_sigsuspend);
    SC_REG(183, getcwd,                         getcwd);
    SC_REG(186, sigaltstack,                    sigaltstack);
    SC_REG(191, ugetrlimit,                     ugetrlimit);
    SC_REG(192, mmap2,                          mmap2);
    SC_REG(195, stat64,                         stat64);
    SC_REG(196, stat64,                         stat64);        // lstat64
    SC_REG(197, stat64,                         stat64);        // fstat64
    SC_REG(199, getuid32,                       default);
    SC_REG(200, getgid32,                       default);
    SC_REG(201, geteuid32,                      default);
    SC_REG(202, getegid32,                      default);
    SC_REG(207, fchown32,                       default);
    SC_REG(212, chown,                          default);
    SC_REG(220, getdents64,                     getdents64);
    SC_REG(221, fcntl,                          fcntl);
    SC_REG(224, gettid,                         default);
    SC_REG(240, futex,                          default);
    SC_REG(242, sched_getaffinity,              sched_getaffinity);
    SC_REG(243, set_thread_area,                default);
    SC_REG(252, exit_group,                     exit_group);
    SC_REG(258, set_tid_address,                default);
    SC_REG(264, clock_settime,                  default);
    SC_REG(265, clock_gettime,                  clock_gettime);
    SC_REG(266, clock_getres,                   clock_getres);
    SC_REG(268, statfs64,                       statfs64);
    SC_REG(270, tgkill,                         default);
    SC_REG(271, utimes,                         default);
    SC_REG(306, fchmodat,                       default);
    SC_REG(311, set_robust_list,                default);


#undef SC_REG
}

static void
syscall_default_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave("d");
}

/*******************************************************************************************************************************/

static void
syscall_exit_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("exit", "d");
}

static void
syscall_exit(RSIM_Thread *t, int callno)
{
    if (t->clear_child_tid) {
        uint32_t zero = 0;
        size_t n = t->get_process()->mem_write(&zero, t->clear_child_tid, sizeof zero);
        ROSE_ASSERT(n==sizeof zero);
        int nwoke = t->futex_wake(t->clear_child_tid);
        ROSE_ASSERT(nwoke>=0);
    }

    /* Throwing an Exit will cause the thread main loop to terminate (and perhaps the real thread terminates as
     * well). The simulated thread is effectively dead at this point. */
    t->tracing(TRACE_SYSCALL)->more(" = <throwing Exit>\n");
    throw RSIM_Process::Exit(__W_EXITCODE(t->syscall_arg(0), 0), false); /* false=>exit only this thread */
}

static void
syscall_exit_leave(RSIM_Thread *t, int callno)
{
    /* This should not be reached, but might be reached if the exit system call body was skipped over. */
    t->tracing(TRACE_SYSCALL)->more(" = <should not have returned>\n");
}

/*******************************************************************************************************************************/

static void
syscall_read_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("read", "dpd");
}

static void
syscall_read(RSIM_Thread *t, int callno)
{
    int fd=t->syscall_arg(0);
    uint32_t buf_va=t->syscall_arg(1), size=t->syscall_arg(2);
    char buf[size];
    ssize_t nread = read(fd, buf, size);
    if (-1==nread) {
        t->syscall_return(-errno);
    } else if (t->get_process()->mem_write(buf, buf_va, (size_t)nread)!=(size_t)nread) {
        t->syscall_return(-EFAULT);
    } else {
        t->syscall_return(nread);
    }
}

static void
syscall_read_leave(RSIM_Thread *t, int callno)
{
    ssize_t nread = t->syscall_arg(-1);
    t->syscall_leave("d-b", nread>0?nread:0);
}

/*******************************************************************************************************************************/

static void
syscall_write_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("write", "dbd", t->syscall_arg(2));
}

static void
syscall_write(RSIM_Thread *t, int callno)
{
    int fd=t->syscall_arg(0);
    uint32_t buf_va=t->syscall_arg(1);
    size_t size=t->syscall_arg(2);
    uint8_t buf[size];
    size_t nread = t->get_process()->mem_read(buf, buf_va, size);
    if (nread!=size) {
        t->syscall_return(-EFAULT);
    } else {
        ssize_t nwritten = write(fd, buf, size);
        if (-1==nwritten) {
            t->syscall_return(-errno);
        } else {
            t->syscall_return(nwritten);
        }
    }
}

/*******************************************************************************************************************************/

static void
syscall_open_enter(RSIM_Thread *t, int callno)
{
    if (t->syscall_arg(1) & O_CREAT) {
        t->syscall_enter("open", "sff", open_flags, file_mode_flags);
    } else {
        t->syscall_enter("open", "sf-", open_flags);
    }
}

static void
syscall_open(RSIM_Thread *t, int callno)
{
    uint32_t filename_va=t->syscall_arg(0);
    bool error;
    std::string filename = t->get_process()->read_string(filename_va, 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }
    uint32_t flags=t->syscall_arg(1), mode=(flags & O_CREAT)?t->syscall_arg(2):0;
    int fd = open(filename.c_str(), flags, mode);
    if (-1==fd) {
        t->syscall_return(-errno);
        return;
    }

    t->syscall_return(fd);
}

/*******************************************************************************************************************************/

static void
syscall_close_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("close", "d");
}

static void
syscall_close(RSIM_Thread *t, int callno)
{
    int fd=t->syscall_arg(0);
    if (1==fd || 2==fd) {
        /* ROSE is using these */
        t->syscall_return(-EPERM);
    } else {
        int status = close(fd);
        t->syscall_return(status<0 ? -errno : status);
    }
}

/*******************************************************************************************************************************/

static void
syscall_waitpid_enter(RSIM_Thread *t, int callno)
{
    static const Translate wflags[] = { TF(WNOHANG), TF(WUNTRACED), T_END };
    t->syscall_enter("waitpid", "dpf", wflags);
}

static void
syscall_waitpid(RSIM_Thread *t, int callno)
{
    pid_t pid=t->syscall_arg(0);
    uint32_t status_va=t->syscall_arg(1);
    int options=t->syscall_arg(2);
    int sys_status;
    int result = waitpid(pid, &sys_status, options);
    if (result == -1) {
        result = -errno;
    } else if (status_va) {
        uint32_t status_le;
        SgAsmExecutableFileFormat::host_to_le(sys_status, &status_le);
        size_t nwritten = t->get_process()->mem_write(&status_le, status_va, 4);
        ROSE_ASSERT(4==nwritten);
    }
    t->syscall_return(result);
}

static void
syscall_waitpid_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave("d-P", 4, print_exit_status_32);
}

/*******************************************************************************************************************************/

static void
syscall_creat_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("creat", "sd");
}

static void
syscall_creat(RSIM_Thread *t, int callno)
{
    uint32_t filename = t->syscall_arg(0);
    bool error;
    std::string sys_filename = t->get_process()->read_string(filename, 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }
    mode_t mode = t->syscall_arg(1);

    int result = creat(sys_filename.c_str(), mode);
    if (result == -1) {
        t->syscall_return(-errno);
        return;
    }

    t->syscall_return(result);
}

/*******************************************************************************************************************************/

static void
syscall_link_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("link", "ss");
}

static void
syscall_link(RSIM_Thread *t, int callno)
{
    bool error;

    std::string oldpath = t->get_process()->read_string(t->syscall_arg(0), 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
	return;
    }
    std::string newpath = t->get_process()->read_string(t->syscall_arg(1), 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }

    int result = syscall(SYS_link,oldpath.c_str(), newpath.c_str());
    t->syscall_return(-1==result?-errno:result);
}

/*******************************************************************************************************************************/

static void
syscall_unlink_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("unlink", "s");
}

static void
syscall_unlink(RSIM_Thread *t, int callno)
{
    uint32_t filename_va = t->syscall_arg(0);
    bool error;
    std::string filename = t->get_process()->read_string(filename_va, 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }

    int result = unlink(filename.c_str());
    if (result == -1) {
        t->syscall_return(-errno);
        return;
    }
                
    t->syscall_return(result);
}

/*******************************************************************************************************************************/

static void
syscall_execve_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("execve", "spp");
}

static void
syscall_execve(RSIM_Thread *t, int callno)
{
    bool error;

    /* Name of executable */
    std::string filename = t->get_process()->read_string(t->syscall_arg(0), 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }

    /* Argument vector */
    std::vector<std::string> argv = t->get_process()->read_string_vector(t->syscall_arg(1), &error);
    if (!argv.empty()) {
        for (size_t i=0; i<argv.size(); i++) {
            t->tracing(TRACE_SYSCALL)->more("    argv[%zu] = ", i);
            print_string(t->tracing(TRACE_SYSCALL), argv[i], false, false);
            t->tracing(TRACE_SYSCALL)->more("\n");
        }
    }
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }
    std::vector<char*> sys_argv;
    for (size_t i = 0; i < argv.size(); ++i)
        sys_argv.push_back(&argv[i][0]);
    sys_argv.push_back(NULL);

    /* Environment vector */
    std::vector<std::string> envp = t->get_process()->read_string_vector(t->syscall_arg(2), &error);
    if (!envp.empty()) {
        for (size_t i=0; i<envp.size(); i++) {
            t->tracing(TRACE_SYSCALL)->more("    envp[%zu] = ", i);
            print_string(t->tracing(TRACE_SYSCALL), envp[i], false, false);
            t->tracing(TRACE_SYSCALL)->more("\n");
        }
    }
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }
    std::vector<char*> sys_envp;
    for (unsigned int i = 0; i < envp.size(); ++i)
        sys_envp.push_back(&envp[i][0]);
    sys_envp.push_back(NULL);

    /* The real system call */
    int result = execve(&filename[0], &sys_argv[0], &sys_envp[0]);
    ROSE_ASSERT(-1==result);
    t->syscall_return(-errno);
}

/*******************************************************************************************************************************/

static void
syscall_chdir_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("chdir", "s");
}

static void
syscall_chdir(RSIM_Thread *t, int callno)
{
    uint32_t path = t->syscall_arg(0);
    bool error;
    std::string sys_path = t->get_process()->read_string(path, 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }

    int result = chdir(sys_path.c_str());
    if (result == -1) {
        t->syscall_return(-errno);
        return;
    }

    t->syscall_return(result);
}

/*******************************************************************************************************************************/

static void
syscall_time_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("time", "p");
}

static void
syscall_time(RSIM_Thread *t, int callno)
{
    time_t result = time(NULL);
    if (t->syscall_arg(0)) {
        uint32_t t_le;
        SgAsmExecutableFileFormat::host_to_le(result, &t_le);
        size_t nwritten = t->get_process()->mem_write(&t_le, t->syscall_arg(0), 4);
        ROSE_ASSERT(4==nwritten);
    }
    t->syscall_return(result);
}

static void
syscall_time_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave("t");
}

/*******************************************************************************************************************************/

static void
syscall_mknod_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("mknod", "sfd", file_mode_flags);
}

static void
syscall_mknod(RSIM_Thread *t, int callno)
{
    uint32_t path_va = t->syscall_arg(0);
    int mode = t->syscall_arg(1);
    unsigned dev = t->syscall_arg(2);
    bool error;
    std::string path = t->get_process()->read_string(path_va, 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }
    int result = mknod(path.c_str(), mode, dev);
    t->syscall_return(-1==result ? -errno : result);
}

/*******************************************************************************************************************************/

static void
syscall_chmod_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("chmod", "sd");
}

static void
syscall_chmod(RSIM_Thread *t, int callno)
{
    uint32_t filename = t->syscall_arg(0);
    bool error;
    std::string sys_filename = t->get_process()->read_string(filename, 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }
    mode_t mode = t->syscall_arg(1);
    int result = chmod(sys_filename.c_str(), mode);
    if (result == -1) result = -errno;
    t->syscall_return(result);
}

/*******************************************************************************************************************************/

static void
syscall_lseek_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("lseek", "ddf", seek_whence);
}

static void
syscall_lseek(RSIM_Thread *t, int callno)
{
    off_t result = lseek(t->syscall_arg(0), t->syscall_arg(1), t->syscall_arg(2));
    t->syscall_return(-1==result?-errno:result);
}

/*******************************************************************************************************************************/

static void
syscall_getpid_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("getpid", "");
}

static void
syscall_getpid(RSIM_Thread *t, int callno)
{
    t->syscall_return(getpid());
}

/*******************************************************************************************************************************/

static void
syscall_getuid_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("getuid", "");
}

static void
syscall_getuid(RSIM_Thread *t, int callno)
{
    t->syscall_return(getuid());
}

/*******************************************************************************************************************************/

static void
syscall_alarm_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("alarm", "d");
}

static void
syscall_alarm(RSIM_Thread *t, int callno)
{
    int result = alarm(t->syscall_arg(0));
    t->syscall_return(result);
}

/*******************************************************************************************************************************/

static void
syscall_pause_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("pause", "");
}

static void
syscall_pause(RSIM_Thread *t, int callno)
{
    t->syscall_info.signo = t->sys_sigsuspend(NULL);
    t->syscall_return(-EINTR);
}

static void
syscall_pause_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave("d");
    if (t->syscall_info.signo>0) {
        t->tracing(TRACE_SYSCALL)->multipart("", "    retured due to ");
        print_enum(t->tracing(TRACE_SYSCALL), signal_names, t->syscall_info.signo);
        t->tracing(TRACE_SYSCALL)->more("(%d)", t->syscall_info.signo);
        t->tracing(TRACE_SYSCALL)->multipart_end();
    }
}

/*******************************************************************************************************************************/

static void
syscall_utime_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("utime", "sp");
}

static void
syscall_utime(RSIM_Thread *t, int callno)
{
    bool error;
    std::string filename = t->get_process()->read_string(t->syscall_arg(0), 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }

    // Check to see if times is NULL
    uint8_t byte;
    size_t nread = t->get_process()->mem_read(&byte, t->syscall_arg(1), 1);
    ROSE_ASSERT(1==nread); /*or we've read past the end of the mapped memory*/

    int result;
    if( byte) {
        struct kernel_utimebuf {
            uint32_t actime;
            uint32_t modtime;
        };

        kernel_utimebuf ubuf;
        size_t nread = t->get_process()->mem_read(&ubuf, t->syscall_arg(1), sizeof(kernel_utimebuf));
        ROSE_ASSERT(nread == sizeof(kernel_utimebuf));

        utimbuf ubuf64;
        ubuf64.actime  = ubuf.actime;
        ubuf64.modtime = ubuf.modtime;

        result = utime(filename.c_str(), &ubuf64);

    } else {
        result = utime(filename.c_str(), NULL);
    }
    t->syscall_return(result);
}

/*******************************************************************************************************************************/

static void
syscall_access_enter(RSIM_Thread *t, int callno)
{
    static const Translate flags[] = { TF(R_OK), TF(W_OK), TF(X_OK), TF(F_OK), T_END };
    t->syscall_enter("access", "sf", flags);
}

static void
syscall_access(RSIM_Thread *t, int callno)
{
    uint32_t name_va=t->syscall_arg(0);
    bool error;
    std::string name = t->get_process()->read_string(name_va, 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }
    int mode=t->syscall_arg(1);
    int result = access(name.c_str(), mode);
    if (-1==result) result = -errno;
    t->syscall_return(result);
}

/*******************************************************************************************************************************/

static void
syscall_sync_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("sync", "");
}

static void
syscall_sync(RSIM_Thread *t, int callno)
{
    sync();
    t->syscall_return(0);
}

/*******************************************************************************************************************************/

static void
syscall_kill_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("kill", "df", signal_names);
}

static void
syscall_kill(RSIM_Thread *t, int callno)
{
    pid_t pid=t->syscall_arg(0);
    int signo=t->syscall_arg(1);
    int result = t->sys_kill(pid, RSIM_SignalHandling::mk_kill(signo, SI_USER));
    t->syscall_return(result);
}

/*******************************************************************************************************************************/

static void
syscall_rename_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("rename", "ss");
}

static void
syscall_rename(RSIM_Thread *t, int callno)
{
    bool error;

    std::string oldpath = t->get_process()->read_string(t->syscall_arg(0), 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
	return;
    }
    std::string newpath = t->get_process()->read_string(t->syscall_arg(1), 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
	return;
    }

    int result = syscall(SYS_rename,oldpath.c_str(), newpath.c_str());
    t->syscall_return(-1==result?-errno:result);
}

/*******************************************************************************************************************************/

static void
syscall_mkdir_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("mkdir", "sd");
}

static void
syscall_mkdir(RSIM_Thread *t, int callno)
{
    uint32_t pathname = t->syscall_arg(0);
    bool error;
    std::string sys_pathname = t->get_process()->read_string(pathname, 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }
    mode_t mode = t->syscall_arg(1);

    int result = mkdir(sys_pathname.c_str(), mode);
    if (result == -1) result = -errno;
    t->syscall_return(result);
}

/*******************************************************************************************************************************/

static void
syscall_rmdir_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("rmdir", "s");
}

static void
syscall_rmdir(RSIM_Thread *t, int callno)
{
    uint32_t pathname = t->syscall_arg(0);
    bool error;
    std::string sys_pathname = t->get_process()->read_string(pathname, 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }

    int result = rmdir(sys_pathname.c_str());
    if (result == -1) result = -errno;
    t->syscall_return(result);
}

/*******************************************************************************************************************************/

static void
syscall_dup_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("dup", "d");
}

static void
syscall_dup(RSIM_Thread *t, int callno)
{
    uint32_t fd = t->syscall_arg(0);
    int result = dup(fd);
    if (-1==result) result = -errno;
    t->syscall_return(result);
}

/*******************************************************************************************************************************/

static void
syscall_pipe_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("pipe", "p");
}

static void
syscall_pipe(RSIM_Thread *t, int callno)
{
    int32_t filedes_kernel[2];
    size_t  size_filedes = sizeof(int32_t)*2;

    int filedes[2];
    int result = pipe(filedes);

    filedes_kernel[0] = filedes[0];
    filedes_kernel[1] = filedes[1];

    t->get_process()->mem_write(filedes_kernel, t->syscall_arg(0), size_filedes);

    if (-1==result) result = -errno;
    t->syscall_return(result);
}

/*******************************************************************************************************************************/

static void
syscall_brk_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("brk", "x");
}

static void
syscall_brk(RSIM_Thread *t, int callno)
{
    uint32_t newbrk = t->syscall_arg(0);
    t->syscall_return(t->get_process()->mem_setbrk(newbrk, t->tracing(TRACE_MMAP)));
}

static void
syscall_brk_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave("p");
}

/*******************************************************************************************************************************/

static void
syscall_getgid_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("getgid", "");
}

static void
syscall_getgid(RSIM_Thread *t, int callno)
{
    t->syscall_return(getgid());
}

/*******************************************************************************************************************************/

static void
syscall_geteuid_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("geteuid", "");
}

static void
syscall_geteuid(RSIM_Thread *t, int callno)
{
    t->syscall_return(geteuid());
}

/*******************************************************************************************************************************/

static void
syscall_getegid_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("getegid", "");
}

static void
syscall_getegid(RSIM_Thread *t, int callno)
{
    t->syscall_return(getegid());
}

/*******************************************************************************************************************************/

static void
syscall_ioctl_enter(RSIM_Thread *t, int callno)
{
    uint32_t cmd=t->syscall_arg(1);
    switch (cmd) {
        case TCGETS:
            t->syscall_enter("ioctl", "dfp", ioctl_commands);
            break;
        case TCSETSW:
        case TCSETSF:
        case TCSETS:
            t->syscall_enter("ioctl", "dfP", ioctl_commands, sizeof(termios_32), print_termios_32);
            break;
        case TCGETA:
            t->syscall_enter("ioctl", "dfd", ioctl_commands);
            break;
        case TIOCGPGRP:
            t->syscall_enter("ioctl", "dfd", ioctl_commands);
            break;
        case TIOCSPGRP:
            t->syscall_enter("ioctl", "dfd", ioctl_commands);
            break;
        case TIOCSWINSZ:
            t->syscall_enter("ioctl", "dfP", ioctl_commands, sizeof(winsize_32), print_winsize_32);
            break;
        case TIOCGWINSZ:
            t->syscall_enter("ioctl", "dfp", ioctl_commands);
            break;
        default:
            t->syscall_enter("ioctl", "dfd", ioctl_commands);
            break;
    }
}

static void
syscall_ioctl(RSIM_Thread *t, int callno)
{
    int fd=t->syscall_arg(0);
    uint32_t cmd=t->syscall_arg(1);

    switch (cmd) {
        case TCGETS: { /* 0x00005401, tcgetattr*/
            termios_native host_ti;
            int result = syscall(SYS_ioctl, fd, cmd, &host_ti);
            if (-1==result) {
                t->syscall_return(-errno);
                break;
            }
            termios_32 guest_ti;
            guest_ti.c_iflag = host_ti.c_iflag;
            guest_ti.c_oflag = host_ti.c_oflag;
            guest_ti.c_cflag = host_ti.c_cflag;
            guest_ti.c_lflag = host_ti.c_lflag;
            guest_ti.c_line = host_ti.c_line;
            for (int i=0; i<19; i++)
                guest_ti.c_cc[i] = host_ti.c_cc[i];
            if (sizeof(guest_ti)!=t->get_process()->mem_write(&guest_ti, t->syscall_arg(2), sizeof guest_ti)) {
                t->syscall_return(-EFAULT);
                break;
            }
            t->syscall_return(result);
            break;
        }

        case TCSETSW:   /* 0x00005403 */
        case TCSETSF:
        case TCSETS: {  /* 0x00005402 */
            termios_32 guest_ti;
            if (sizeof(guest_ti)!=t->get_process()->mem_read(&guest_ti, t->syscall_arg(2), sizeof guest_ti)) {
                t->syscall_return(-EFAULT);
                break;
            }
            termios_native host_ti;
            host_ti.c_iflag = guest_ti.c_iflag;
            host_ti.c_oflag = guest_ti.c_oflag;
            host_ti.c_cflag = guest_ti.c_cflag;
            host_ti.c_lflag = guest_ti.c_lflag;
            host_ti.c_line = guest_ti.c_line;
            for (int i=0; i<19; i++)
                host_ti.c_cc[i] = guest_ti.c_cc[i];
            int result = syscall(SYS_ioctl, fd, cmd, &host_ti);
            t->syscall_return(-1==result?-errno:result);
            break;
        }

        case TCGETA: { /* 0x,00005405 */
            /* gets a data structure of type 
                   struct termio * 

               struct termio {
                 unsigned short c_iflag;     // input mode flags 
                 unsigned short c_oflag;     // output mode flags 
                 unsigned short c_cflag;     // control mode flags 
                 unsigned short c_lflag;     // local mode flags 
                 unsigned char c_line;       // line discipline 
                 unsigned char c_cc[NCC];    // control characters 
               };

             */

            termio to;

            int result = ioctl(fd, TCGETA, &to);
            if (-1==result) {
                result = -errno;
            } else {
                size_t nwritten = t->get_process()->mem_write(&to, t->syscall_arg(2), sizeof to);
                ROSE_ASSERT(nwritten==sizeof to);
            }

            t->syscall_return(result);
            break;
        }

        case TIOCGPGRP: { /* 0x0000540F, tcgetpgrp*/
            /* equivalent to 
                pid_t tcgetpgrp(int fd);
               The  function tcgetpgrp() returns the process group ID of the foreground process group 
               on the terminal associated to fd, which must be the controlling terminal of the calling 
               process.
            */

            pid_t pgrp = tcgetpgrp(fd);
            if (-1==pgrp) {
                t->syscall_return(-errno);
                break;
            }
            uint32_t pgrp_le;
            SgAsmExecutableFileFormat::host_to_le(pgrp, &pgrp_le);
            size_t nwritten = t->get_process()->mem_write(&pgrp_le, t->syscall_arg(2), 4);
            ROSE_ASSERT(4==nwritten);
            t->syscall_return(pgrp);
            break;
        }

        case TIOCSPGRP: { /* 0x5410, tcsetpgrp*/
            uint32_t pgid_le;
            size_t nread = t->get_process()->mem_read(&pgid_le, t->syscall_arg(2), 4);
            ROSE_ASSERT(4==nread);
            pid_t pgid = SgAsmExecutableFileFormat::le_to_host(pgid_le);
            int result = tcsetpgrp(fd, pgid);
            if (-1==result)
                result = -errno;
            t->syscall_return(result);
            break;
        }

        case TIOCSWINSZ: { /* 0x5413, the winsize is const */
            winsize_32 guest_ws;
            if (sizeof(guest_ws)!=t->get_process()->mem_read(&guest_ws, t->syscall_arg(2), sizeof guest_ws)) {
                t->syscall_return(-EFAULT);
                break;
            }

            winsize_native host_ws;
            host_ws.ws_row = guest_ws.ws_row;
            host_ws.ws_col = guest_ws.ws_col;
            host_ws.ws_xpixel = guest_ws.ws_xpixel;
            host_ws.ws_ypixel = guest_ws.ws_ypixel;

            int result = syscall(SYS_ioctl, fd, cmd, &host_ws);
            t->syscall_return(-1==result?-errno:result);
            break;
        }

        case TIOCGWINSZ: /* 0x5414, */ {
            winsize_native host_ws;
            int result = syscall(SYS_ioctl, fd, cmd, &host_ws);
            if (-1==result) {
                t->syscall_return(-errno);
                break;
            }

            winsize_32 guest_ws;
            guest_ws.ws_row = host_ws.ws_row;
            guest_ws.ws_col = host_ws.ws_col;
            guest_ws.ws_xpixel = host_ws.ws_xpixel;
            guest_ws.ws_ypixel = host_ws.ws_ypixel;
            if (sizeof(guest_ws)!=t->get_process()->mem_write(&guest_ws, t->syscall_arg(2), sizeof guest_ws)) {
                t->syscall_return(-EFAULT);
                break;
            }

            t->syscall_return(result);
            break;
        }

        default: {
            fprintf(stderr, "  unhandled ioctl: %u\n", cmd);
            abort();
        }
    }
}

static void
syscall_ioctl_leave(RSIM_Thread *t, int callno)
{
    uint32_t cmd=t->syscall_arg(1);
    switch (cmd) {
        case TCGETS:
            t->syscall_leave("d--P", sizeof(termios_32), print_termios_32);
            break;
        case TIOCGWINSZ:
            t->syscall_leave("d--P", sizeof(winsize_32), print_winsize_32);
            break;
        default:
            t->syscall_leave("d");
            break;
    }
}

/*******************************************************************************************************************************/

static void
syscall_setpgid_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("setpgid", "dd");
}

static void
syscall_setpgid(RSIM_Thread *t, int callno)
{
    pid_t pid=t->syscall_arg(0), pgid=t->syscall_arg(1);
    int result = setpgid(pid, pgid);
    if (-1==result) { result = -errno; }
    t->syscall_return(result);
}

/*******************************************************************************************************************************/

static void
syscall_umask_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("umask", "d");
}

static void
syscall_umask(RSIM_Thread *t, int callno)
{
    mode_t mode = t->syscall_arg(0);
    int result = syscall(SYS_umask, mode); 
    if (result == -1) result = -errno;
    t->syscall_return(result);
}

/*******************************************************************************************************************************/

static void
syscall_dup2_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("dup2", "dd");
}

static void
syscall_dup2(RSIM_Thread *t, int callno)
{
    int result = dup2(t->syscall_arg(0), t->syscall_arg(1));
    t->syscall_return(-1==result?-errno:result);
}

/*******************************************************************************************************************************/

static void
syscall_getppid_enter(RSIM_Thread *t, int callno)
{
            t->syscall_enter("getppid", "");
}

static void
syscall_getppid(RSIM_Thread *t, int callno)
{
    t->syscall_return(getppid());
}

/*******************************************************************************************************************************/

static void
syscall_getpgrp_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("getpgrp", "");
}

static void
syscall_getpgrp(RSIM_Thread *t, int callno)
{
    t->syscall_return(getpgrp());
}

/*******************************************************************************************************************************/

static void
syscall_setrlimit_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("setrlimit", "fP", rlimit_resources, 8, print_rlimit);
}

static void
syscall_setrlimit(RSIM_Thread *t, int callno)
{
    int resource = t->syscall_arg(0);
    uint32_t rlimit_va = t->syscall_arg(1);
    uint32_t rlimit_guest[2];
    size_t nread = t->get_process()->mem_read(rlimit_guest, rlimit_va, sizeof rlimit_guest);
    ROSE_ASSERT(nread==sizeof rlimit_guest);
    struct rlimit rlimit_native;
    rlimit_native.rlim_cur = rlimit_guest[0];
    rlimit_native.rlim_max = rlimit_guest[1];
    int result = setrlimit(resource, &rlimit_native);
    t->syscall_return(-1==result ? -errno : result);
}

/*******************************************************************************************************************************/

static void
syscall_ugetrlimit_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("ugetrlimit", "fp", rlimit_resources);
}

static void
syscall_ugetrlimit(RSIM_Thread *t, int callno)
{
    syscall_getrlimit(t, callno);
}

static void
syscall_ugetrlimit_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave("d-P", 8, print_rlimit);
}

/*******************************************************************************************************************************/

static void
syscall_getrlimit_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("getrlimit", "fp", rlimit_resources);
}

static void
syscall_getrlimit(RSIM_Thread *t, int callno)
{
    int resource = t->syscall_arg(0);
    uint32_t rlimit_va = t->syscall_arg(1);
    struct rlimit rlimit_native;
    int result = getrlimit(resource, &rlimit_native);
    if (-1==result) {
        t->syscall_return(-errno);
        return;
    }

    uint32_t rlimit_guest[2];
    rlimit_guest[0] = rlimit_native.rlim_cur;
    rlimit_guest[1] = rlimit_native.rlim_max;
    if (8!=t->get_process()->mem_write(rlimit_guest, rlimit_va, 8)) {
        t->syscall_return(-EFAULT);
        return;
    }

    t->syscall_return(result);
}

static void
syscall_getrlimit_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave("d-P", 8, print_rlimit);
}

/*******************************************************************************************************************************/

static void
syscall_gettimeofday_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("gettimeofday", "p");
}

static void
syscall_gettimeofday(RSIM_Thread *t, int callno)
{
    uint32_t tp = t->syscall_arg(0);
    struct timeval host_time;
    struct timeval_32 guest_time;

    int result = gettimeofday(&host_time, NULL);
    if (result == -1) {
        result = -errno;
    } else {
        guest_time.tv_sec = host_time.tv_sec;
        guest_time.tv_usec = host_time.tv_usec;
        if (sizeof(guest_time) != t->get_process()->mem_write(&guest_time, tp, sizeof guest_time))
            result = -EFAULT;
    }

    t->syscall_return(result);
}

static void
syscall_gettimeofday_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave("dP", sizeof(timeval_32), print_timeval_32);
}

/*******************************************************************************************************************************/

static void
syscall_symlink_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("symlink", "ss");
}

static void
syscall_symlink(RSIM_Thread *t, int callno)
{
    uint32_t oldpath=t->syscall_arg(0), newpath=t->syscall_arg(1);
    bool error;
    std::string sys_oldpath = t->get_process()->read_string(oldpath, 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }
    std::string sys_newpath = t->get_process()->read_string(newpath, 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }
    int result = symlink(sys_oldpath.c_str(),sys_newpath.c_str());
    if (result == -1) result = -errno;
    t->syscall_return(result);
}

/*******************************************************************************************************************************/

static void
syscall_readlink_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("readlink", "spd");
}

static void
syscall_readlink(RSIM_Thread *t, int callno)
{
    uint32_t path=t->syscall_arg(0), buf_va=t->syscall_arg(1), bufsize=t->syscall_arg(2);
    char sys_buf[bufsize];
    bool error;
    std::string sys_path = t->get_process()->read_string(path, 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }
    int result = readlink(sys_path.c_str(), sys_buf, bufsize);
    if (result == -1) {
        result = -errno;
    } else {
        size_t nwritten = t->get_process()->mem_write(sys_buf, buf_va, result);
        ROSE_ASSERT(nwritten == (size_t)result);
    }
    t->syscall_return(result);
}

/*******************************************************************************************************************************/

static void
syscall_munmap_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("munmap", "pd");
}

static void
syscall_munmap(RSIM_Thread *t, int callno)
{
    uint32_t va=t->syscall_arg(0);
    uint32_t sz=t->syscall_arg(1);
    uint32_t aligned_va = ALIGN_DN(va, PAGE_SIZE);
    uint32_t aligned_sz = ALIGN_UP(sz+va-aligned_va, PAGE_SIZE);

    /* Check ranges */
    if (aligned_va+aligned_sz <= aligned_va) { /* FIXME: not sure if sz==0 is an error */
        t->syscall_return(-EINVAL);
        return;
    }

    int status = t->get_process()->mem_unmap(aligned_va, aligned_sz, t->tracing(TRACE_MMAP));
    t->syscall_return(status);
}

/*******************************************************************************************************************************/

static void
syscall_ftruncate_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("ftruncate", "dd");
}

static void
syscall_ftruncate(RSIM_Thread *t, int callno)
{
    int fd = t->syscall_arg(0);
    off_t len = t->syscall_arg(1);
    int result = ftruncate(fd, len);
    t->syscall_return(-1==result ? -errno : result);
}

/*******************************************************************************************************************************/

static void
syscall_fchmod_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("fchmod", "dd");
}

static void
syscall_fchmod(RSIM_Thread *t, int callno)
{
    uint32_t fd = t->syscall_arg(0);
    mode_t mode = t->syscall_arg(1);

    int result = fchmod(fd, mode);
    if (result == -1) result = -errno;
    t->syscall_return(result);
}

/*******************************************************************************************************************************/

static void
syscall_fchown_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("fchown", "ddd");
}

static void
syscall_fchown(RSIM_Thread *t, int callno)
{
    uint32_t fd = t->syscall_arg(0);
    int user = t->syscall_arg(1);
    int group = t->syscall_arg(2);
    int result = syscall(SYS_fchown, fd, user, group);
    t->syscall_return(-1==result?-errno:result);
}

/*******************************************************************************************************************************/

static void
syscall_statfs_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("statfs", "sp");
}

static void
syscall_statfs(RSIM_Thread *t, int callno)
{
    int result;
    statfs_32 guest_statfs;
    bool error;
    std::string path = t->get_process()->read_string(t->syscall_arg(0), 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }

#ifdef SYS_statfs64 /* host is 32-bit machine */
    static statfs64_native host_statfs;
    result = syscall(SYS_statfs64 , path.c_str(), sizeof host_statfs, &host_statfs);
    convert(&guest_statfs, &host_statfs);
#else               /* host is 64-bit machine */
    static statfs_native host_statfs;
    result = syscall(SYS_statfs, path.c_str(), &host_statfs);
    convert(&guest_statfs, &host_statfs);
#endif

    if (-1==result) {
        t->syscall_return(-errno);
        return;
    }

    if (sizeof(guest_statfs)!=t->get_process()->mem_write(&guest_statfs, t->syscall_arg(1), sizeof guest_statfs)) {
        t->syscall_return(-EFAULT);
        return;
    }

    t->syscall_return(result);
}

static void
syscall_statfs_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave("d-P", sizeof(statfs_32), print_statfs_32);
}

/*******************************************************************************************************************************/

static void
syscall_fstatfs_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("fstatfs", "dp");
}

static void
syscall_fstatfs(RSIM_Thread *t, int callno)
{
    int result;
    statfs_32 guest_statfs;
#ifdef SYS_statfs64 /* host is 32-bit machine */
    static statfs64_native host_statfs;
    result = syscall(SYS_fstatfs64 ,t->syscall_arg(0), sizeof host_statfs, &host_statfs);
    convert(&guest_statfs, &host_statfs);
#else               /* host is 64-bit machine */
    static statfs_native host_statfs;
    result = syscall(SYS_fstatfs, t->syscall_arg(0), &host_statfs);
    convert(&guest_statfs, &host_statfs);
#endif

    if (-1==result) {
        t->syscall_return(-errno);
        return;
    }

    if (sizeof(guest_statfs)!=t->get_process()->mem_write(&guest_statfs, t->syscall_arg(1), sizeof guest_statfs)) {
        t->syscall_return(-EFAULT);
        return;
    }

    t->syscall_return(result);
}

static void
syscall_fstatfs_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave("d-P", sizeof(statfs_32), print_statfs_32);
}

/*******************************************************************************************************************************/

static void
syscall_socketcall_enter(RSIM_Thread *t, int callno)
{
    uint32_t a[6];
    switch (t->syscall_arg(0)) {
        case 1: /* SYS_SOCKET */
            if (12==t->get_process()->mem_read(a, t->syscall_arg(1), 12)) {
                t->syscall_enter(a, "socket", "fff", protocol_families, socket_types, socket_protocols);
            } else {
                t->syscall_enter("socketcall", "fp", socketcall_commands);
            }
            break;
        case 2: /* SYS_BIND */
            if (12==t->get_process()->mem_read(a, t->syscall_arg(1), 12)) {
                t->syscall_enter(a, "bind", "dpd");    /* FIXME: we could do a better job printing the address [RPM 2011-01-04] */
            } else {
                t->syscall_enter("socketcall", "fp", socketcall_commands);
            }
            break;
        case 4: /* SYS_LISTEN */
            if (8==t->get_process()->mem_read(a, t->syscall_arg(1), 8)) {
                t->syscall_enter(a, "listen", "dd");
            } else {
                t->syscall_enter("socketcall", "fp", socketcall_commands);
            }
            break;
        default:
            t->syscall_enter("socketcall", "fp", socketcall_commands);
            break;
    }
}

static void
sys_socket(RSIM_Thread *t, int family, int type, int protocol)
{
#ifdef SYS_socketcall /* i686 */
    ROSE_ASSERT(4==sizeof(int));
    int a[3];
    a[0] = family;
    a[1] = type;
    a[2] = protocol;
    int result = syscall(SYS_socketcall, 1/*SYS_SOCKET*/, a);
#else /* amd64 */
    int result = syscall(SYS_socket, family, type, protocol);
#endif
    t->syscall_return(-1==result?-errno:result);
}

static void
sys_bind(RSIM_Thread *t, int fd, uint32_t addr_va, uint32_t addrlen)
{
    if (addrlen<1 || addrlen>4096) {
        t->syscall_return(-EINVAL);
        return;
    }
    uint8_t *addrbuf = new uint8_t[addrlen];
    if (addrlen!=t->get_process()->mem_read(addrbuf, addr_va, addrlen)) {
        t->syscall_return(-EFAULT);
        delete[] addrbuf;
        return;
    }

#ifdef SYS_socketcall /* i686 */
    ROSE_ASSERT(4==sizeof(int));
    ROSE_ASSERT(4==sizeof(void*));
    int a[3];
    a[0] = fd;
    a[1] = (int)addrbuf;
    a[2] = addrlen;
    int result = syscall(SYS_socketcall, 2/*SYS_BIND*/, a);
#else /* amd64 */
    int result = syscall(SYS_bind, fd, addrbuf, addrlen);
#endif
    t->syscall_return(-1==result?-errno:result);
    delete[] addrbuf;
}

static void
sys_listen(RSIM_Thread *t, int fd, int backlog)
{
#ifdef SYS_socketcall /* i686 */
    ROSE_ASSERT(4==sizeof(int));
    int a[2];
    a[0] = fd;
    a[1] = backlog;
    int result = syscall(SYS_socketcall, 4/*SYS_LISTEN*/, a);
#else /* amd64 */
    int result = syscall(SYS_listen, fd, backlog);
#endif
    t->syscall_return(-1==result?-errno:result);
}

static void
syscall_socketcall(RSIM_Thread *t, int callno)
{
    /* Return value is written to eax by these helper functions. The struction of this code closely follows that in the
     * Linux kernel. See linux/net/socket.c. */
    uint32_t a[6];
    switch (t->syscall_arg(0)) {
        case 1: { /* SYS_SOCKET */
            if (12!=t->get_process()->mem_read(a, t->syscall_arg(1), 12)) {
                t->syscall_return(-EFAULT);
            } else {
                sys_socket(t, a[0], a[1], a[2]);
            }
            break;
        }
                    
        case 2: { /* SYS_BIND */
            if (12!=t->get_process()->mem_read(a, t->syscall_arg(1), 12)) {
                t->syscall_return(-EFAULT);
            } else {
                sys_bind(t, a[0], a[1], a[2]);
            }
            break;
        }

        case 4: { /* SYS_LISTEN */
            if (8!=t->get_process()->mem_read(a, t->syscall_arg(1), 8)) {
                t->syscall_return(-EFAULT);
            } else {
                sys_listen(t, a[0], a[1]);
            }
            break;
        }
                    
        case 3: /* SYS_CONNECT */
        case 5: /* SYS_ACCEPT */
        case 6: /* SYS_GETSOCKNAME */
        case 7: /* SYS_GETPEERNAME */
        case 8: /* SYS_SOCKETPAIR */
        case 9: /* SYS_SEND */
        case 10: /* SYS_RECV */
        case 11: /* SYS_SENDTO */
        case 12: /* SYS_RECVFROM */
        case 13: /* SYS_SHUTDOWN */
        case 14: /* SYS_SETSOCKOPT */
        case 15: /* SYS_GETSOCKOPT */
        case 16: /* SYS_SENDMSG */
        case 17: /* SYS_RECVMSG */
        case 18: /* SYS_ACCEPT4 */
        case 19: /* SYS_RECVMMSG */
            t->syscall_return(-ENOSYS);
            break;
        default:
            t->syscall_return(-EINVAL);
            break;
    }
}

/*******************************************************************************************************************************/

static void
syscall_wait4_enter(RSIM_Thread *t, int callno)
{
    static const Translate wflags[] = { TF(WNOHANG), TF(WUNTRACED), T_END };
    t->syscall_enter("wait4", "dpfp", wflags);
}

static void
syscall_wait4(RSIM_Thread *t, int callno)
{
    pid_t pid=t->syscall_arg(0);
    uint32_t status_va=t->syscall_arg(1), rusage_va=t->syscall_arg(3);
    int options=t->syscall_arg(2);
    int status;
    struct rusage rusage;
    int result = wait4(pid, &status, options, &rusage);
    if( result == -1) {
        result = -errno;
    } else {
        if (status_va != 0) {
            size_t nwritten = t->get_process()->mem_write(&status, status_va, 4);
            ROSE_ASSERT(nwritten == 4);
        }
        if (rusage_va != 0) {
            struct rusage_32 {
                uint32_t utime_sec;     /* user time used; seconds */
                uint32_t utime_usec;    /* user time used; microseconds */
                uint32_t stime_sec;     /* system time used; seconds */
                uint32_t stime_usec;    /* system time used; microseconds */
                uint32_t maxrss;        /* maximum resident set size */
                uint32_t ixrss;         /* integral shared memory size */
                uint32_t idrss;         /* integral unshared data size */
                uint32_t isrss;         /* integral unshared stack size */
                uint32_t minflt;        /* page reclaims */
                uint32_t majflt;        /* page faults */
                uint32_t nswap;         /* swaps */
                uint32_t inblock;       /* block input operations */
                uint32_t oublock;       /* block output operations */
                uint32_t msgsnd;        /* messages sent */
                uint32_t msgrcv;        /* messages received */
                uint32_t nsignals;      /* signals received */
                uint32_t nvcsw;         /* voluntary context switches */
                uint32_t nivcsw;        /* involuntary " */
            } __attribute__((packed));
            struct rusage_32 out;
            ROSE_ASSERT(18*4==sizeof(out));
            out.utime_sec = rusage.ru_utime.tv_sec;
            out.utime_usec = rusage.ru_utime.tv_usec;
            out.stime_sec = rusage.ru_stime.tv_sec;
            out.stime_usec = rusage.ru_stime.tv_usec;
            out.maxrss = rusage.ru_maxrss;
            out.ixrss = rusage.ru_ixrss;
            out.idrss = rusage.ru_idrss;
            out.isrss = rusage.ru_isrss;
            out.minflt = rusage.ru_minflt;
            out.majflt = rusage.ru_majflt;
            out.nswap = rusage.ru_nswap;
            out.inblock = rusage.ru_inblock;
            out.msgsnd = rusage.ru_msgsnd;
            out.msgrcv = rusage.ru_msgrcv;
            out.nsignals = rusage.ru_nsignals;
            out.nvcsw = rusage.ru_nvcsw;
            out.nivcsw = rusage.ru_nivcsw;
            size_t nwritten = t->get_process()->mem_write(&out, rusage_va, sizeof out);
            ROSE_ASSERT(nwritten == sizeof out);
        }
    }
    t->syscall_return(result);
}

static void
syscall_wait4_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave("d-P", 4, print_exit_status_32);
}

/*******************************************************************************************************************************/

static void
syscall_sysinfo_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("sysinfo", "p");
}

static void
syscall_sysinfo(RSIM_Thread *t, int callno)
{
    static const size_t guest_extra = 20 - 2*sizeof(uint32_t) - sizeof(int32_t);
    static const size_t host_extra  = 20 - 2*sizeof(long)     - sizeof(int);

    struct guest_sysinfo {      /* Sysinfo to be written into the specimen's memory */
        int32_t uptime;         /* Seconds since boot */
        uint32_t loads[3];      /* 1, 5, and 15 minute load averages */
        uint32_t totalram;      /* Total usable main memory size */
        uint32_t freeram;       /* Available memory size */
        uint32_t sharedram;     /* Amount of shared memory */
        uint32_t bufferram;     /* Memory used by buffers */
        uint32_t totalswap;     /* Total swap space size */
        uint32_t freeswap;      /* swap space still available */
        uint16_t procs;         /* Number of current processes */
        uint16_t pad;           /* explicit padding for m68k */
        uint32_t totalhigh;     /* Total high memory size */
        uint32_t freehigh;      /* Available high memory size */
        uint32_t mem_unit;      /* Memory unit size in bytes */
        char _f[guest_extra];   /* Padding for libc5 */
    } __attribute__((__packed__));

    struct host_sysinfo {
        long uptime;
        unsigned long loads[3];
        unsigned long totalram;
        unsigned long freeram;
        unsigned long sharedram;
        unsigned long bufferram;
        unsigned long totalswap;
        unsigned long freeswap;
        unsigned short procs;
        unsigned short pad;      
        unsigned long totalhigh;
        unsigned long freehigh;
        unsigned int mem_unit;
        char _f[host_extra];
    };

    host_sysinfo host_sys;
    int result  = syscall(SYS_sysinfo, &host_sys);

    if (-1==result) {
        t->syscall_return(-errno);
    } else {
        guest_sysinfo guest_sys;
        guest_sys.uptime = host_sys.uptime;
        for(int i = 0 ; i < 3 ; i++)
            guest_sys.loads[i] = host_sys.loads[i];
        guest_sys.totalram      = host_sys.totalram;
        guest_sys.freeram       = host_sys.freeram;
        guest_sys.sharedram     = host_sys.sharedram;
        guest_sys.bufferram     = host_sys.bufferram;
        guest_sys.totalswap     = host_sys.totalswap;
        guest_sys.freeswap      = host_sys.freeswap;
        guest_sys.procs         = host_sys.procs;
        guest_sys.pad           = host_sys.pad;
        guest_sys.totalhigh     = host_sys.totalhigh;
        guest_sys.mem_unit      = host_sys.mem_unit;
        memset(guest_sys._f, 0, sizeof(guest_sys._f));
        memcpy(guest_sys._f, host_sys._f, std::min(guest_extra, host_extra));

        size_t nwritten = t->get_process()->mem_write(&guest_sys, t->syscall_arg(0), sizeof(guest_sys));
        if (nwritten!=sizeof(guest_sys)) {
            t->syscall_return(-EFAULT);
        } else {
            t->syscall_return(result);
        }
    }
}

/*******************************************************************************************************************************/

static void
syscall_ipc_enter(RSIM_Thread *t, int callno)
{
    unsigned call = t->syscall_arg(0) & 0xffff;
    int version = t->syscall_arg(0) >> 16;
    uint32_t second=t->syscall_arg(2), third=t->syscall_arg(3);
    switch (call) {
        case 1: /*SEMOP*/
            t->syscall_enter("ipc", "fdd-p", ipc_commands);
            break;
        case 2: /*SEMGET*/
            t->syscall_enter("ipc", "fddf", ipc_commands, ipc_flags);
            break;
        case 3: /*SEMCTL*/
            switch (third & 0xff) {
                case 16: /*SETVAL*/
                    t->syscall_enter("ipc", "fddfP", ipc_commands, sem_control, 4, print_int_32);
                    break;
                default:
                    t->syscall_enter("ipc", "fddfp", ipc_commands, sem_control);
                    break;
            }
            break;
        case 4: /*SEMTIMEDOP*/
            t->syscall_enter("ipc", "fdd-pP", ipc_commands, sizeof(timespec_32), print_timespec_32);
            break;
        case 11: /*MSGSND*/
            t->syscall_enter("ipc", "fddfb", ipc_commands, ipc_flags, (size_t)(4+t->syscall_arg(2)));
            break;
        case 12: /*MSGRCV*/
            if (0==version) {
                t->syscall_enter("ipc", "fddfP", ipc_commands, ipc_flags, sizeof(ipc_kludge_32), print_ipc_kludge_32);
            } else {
                t->syscall_enter("ipc", "fddfpd", ipc_commands, ipc_flags);
            }
            break;
        case 13: /*MSGGET*/
            t->syscall_enter("ipc", "fpf", ipc_commands, ipc_flags); /* arg1 "p" for consistency with strace and ipcs */
            break;
        case 14: /*MSGCTL*/
            switch (second & 0xff) {
                case 0: /* IPC_RMID */
                    t->syscall_enter("ipc", "fdf", ipc_commands, msg_control);
                    break;
                case 1: /* IPC_SET */
                    t->syscall_enter("ipc", "fdf-P", ipc_commands, msg_control, sizeof(msqid64_ds_32), print_msqid64_ds_32);
                    break;
                default:
                    t->syscall_enter("ipc", "fdf-p", ipc_commands, msg_control);
                    break;
            }
            break;
        case 21: /*SHMAT*/
            if (1==version) {
                t->syscall_enter("ipc", "fdddpd", ipc_commands);
            } else {
                t->syscall_enter("ipc", "fdfpp", ipc_commands, ipc_flags);
            }
            break;
        case 22: /*SHMDT*/
            t->syscall_enter("ipc", "f---p", ipc_commands);
            break;
        case 23: /*SHMGET*/
            t->syscall_enter("ipc", "fpdf", ipc_commands, ipc_flags); /* arg1 "p" for consistency with strace and ipcs */
            break;
        case 24: /*SHMCTL*/
            switch (second & 0xff) {
                case 0:         /* IPC_RMID */
                    t->syscall_enter("ipc", "fdf", ipc_commands, shm_control);
                    break;
                case 1:         /* IPC_SET */
                    t->syscall_enter("ipc", "fdf-P", ipc_commands, shm_control, sizeof(shmid64_ds_32), print_shmid64_ds_32);
                    break;
                case 11:        /* SHM_LOCK */
                case 12:        /* SHM_UNLOCK */
                    t->syscall_enter("ipc", "fdf", ipc_commands, shm_control);
                    break;
                default:
                    t->syscall_enter("ipc", "fdf-p", ipc_commands, shm_control);
                    break;
            }
            break;
        default:
            t->syscall_enter("ipc", "fdddpd", ipc_commands);
            break;
    }
}

static void
sys_semtimedop(RSIM_Thread *t, uint32_t semid, uint32_t sops_va, uint32_t nsops, uint32_t timeout_va)
{
    RTS_Message *strace = t->tracing(TRACE_SYSCALL);

    static const Translate sem_flags[] = {
        TF(IPC_NOWAIT), TF(SEM_UNDO), T_END
    };

    if (nsops<1) {
        t->syscall_return(-EINVAL);
        return;
    }

    /* struct sembuf is the same on both 32- and 64-bit platforms */
    sembuf sops[nsops * sizeof(sembuf)];
    if (nsops*sizeof(sembuf)!=t->get_process()->mem_read(sops, sops_va, nsops*sizeof(sembuf))) {
        t->syscall_return(-EFAULT);
        return;
    }
    for (uint32_t i=0; i<nsops; i++) {
        strace->more("    sops[%"PRIu32"] = { num=%"PRIu16", op=%"PRId16", flg=",
                     i, sops[i].sem_num, sops[i].sem_op);
        print_flags(strace, sem_flags, sops[i].sem_flg);
        strace->more(" }\n");
    }

    timespec host_timeout;
    if (timeout_va) {
        timespec_32 guest_timeout;
        if (sizeof(guest_timeout)!=t->get_process()->mem_read(&guest_timeout, timeout_va, sizeof guest_timeout)) {
            t->syscall_return(-EFAULT);
            return;
        }
        host_timeout.tv_sec = guest_timeout.tv_sec;
        host_timeout.tv_nsec = guest_timeout.tv_nsec;
    }

    int result = semtimedop(semid, sops, nsops, timeout_va?&host_timeout:NULL);
    t->syscall_return(-1==result?-errno:result);
}

static void
sys_semget(RSIM_Thread *t, uint32_t key, uint32_t nsems, uint32_t semflg)
{
#ifdef SYS_ipc /* i686 */
    int result = syscall(SYS_ipc, 2, key, nsems, semflg);
#else
    int result = syscall(SYS_semget, key, nsems, semflg);
#endif
    t->syscall_return(-1==result?-errno:result);
}

static void
sys_semctl(RSIM_Thread *t, uint32_t semid, uint32_t semnum, uint32_t cmd, uint32_t semun_va)
{
    RTS_Message *strace = t->tracing(TRACE_SYSCALL);

    int version = cmd & 0x0100/*IPC_64*/;
    cmd &= ~0x0100;

    ROSE_ASSERT(version!=0);

    union semun_32 {
        uint32_t val;
        uint32_t ptr;
    };

    union semun_native {
        int val;
        void *ptr;
    };

    semun_32 guest_semun;
    if (sizeof(guest_semun)!=t->get_process()->mem_read(&guest_semun, semun_va, sizeof guest_semun)) {
        t->syscall_return(-EFAULT);
        return;
    }
    

    switch (cmd) {
        case 3:         /* IPC_INFO */
        case 19: {      /* SEM_INFO */
            seminfo host_seminfo;
#ifdef SYS_ipc /* i686 */
            ROSE_ASSERT(version!=0);
            semun_native host_semun;
            host_semun.ptr = &host_seminfo;
            int result = syscall(SYS_ipc, 3/*SEMCTL*/, semid, semnum, cmd|version, &host_semun);
#else
            int result = syscall(SYS_semctl, semid, semnum, cmd, &host_seminfo);
#endif
            if (-1==result) {
                t->syscall_return(-errno);
                return;
            }

            seminfo_32 guest_seminfo;
            guest_seminfo.semmap = host_seminfo.semmap;
            guest_seminfo.semmni = host_seminfo.semmni;
            guest_seminfo.semmns = host_seminfo.semmns;
            guest_seminfo.semmnu = host_seminfo.semmnu;
            guest_seminfo.semmsl = host_seminfo.semmsl;
            guest_seminfo.semopm = host_seminfo.semopm;
            guest_seminfo.semume = host_seminfo.semume;
            guest_seminfo.semusz = host_seminfo.semusz;
            guest_seminfo.semvmx = host_seminfo.semvmx;
            guest_seminfo.semaem = host_seminfo.semaem;
            if (sizeof(guest_seminfo)!=t->get_process()->mem_write(&guest_seminfo, guest_semun.ptr, sizeof guest_seminfo)) {
                t->syscall_return(-EFAULT);
                return;
            }

            t->syscall_return(result);
            break;
        }

        case 2:         /* IPC_STAT */
        case 18: {      /* SEM_STAT */
            semid_ds host_ds;
#ifdef SYS_ipc /* i686 */
            ROSE_ASSERT(version!=0);
            semun_native host_semun;
            host_semun.ptr = &host_ds;
            int result = syscall(SYS_ipc, 3/*SEMCTL*/, semid, semnum, cmd|version, &host_semun);
#else
            int result = syscall(SYS_semctl, semid, semnum, cmd, &host_ds);
#endif
            if (-1==result) {
                t->syscall_return(-errno);
                return;
            }

            semid64_ds_32 guest_ds;
            guest_ds.sem_perm.key = host_ds.sem_perm.__key;
            guest_ds.sem_perm.uid = host_ds.sem_perm.uid;
            guest_ds.sem_perm.gid = host_ds.sem_perm.gid;
            guest_ds.sem_perm.cuid = host_ds.sem_perm.cuid;
            guest_ds.sem_perm.cgid = host_ds.sem_perm.cgid;
            guest_ds.sem_perm.mode = host_ds.sem_perm.mode;
            guest_ds.sem_perm.pad1 = host_ds.sem_perm.__pad1;
            guest_ds.sem_perm.seq = host_ds.sem_perm.__seq;
            guest_ds.sem_perm.pad2 = host_ds.sem_perm.__pad2;
            guest_ds.sem_perm.unused1 = host_ds.sem_perm.__unused1;
            guest_ds.sem_perm.unused2 = host_ds.sem_perm.__unused1;
            guest_ds.sem_otime = host_ds.sem_otime;
            guest_ds.unused1 = host_ds.__unused1;
            guest_ds.sem_ctime = host_ds.sem_ctime;
            guest_ds.unused2 = host_ds.__unused2;
            guest_ds.sem_nsems = host_ds.sem_nsems;
            guest_ds.unused3 = host_ds.__unused3;
            guest_ds.unused4 = host_ds.__unused4;
            if (sizeof(guest_ds)!=t->get_process()->mem_write(&guest_ds, guest_semun.ptr, sizeof guest_ds)) {
                t->syscall_return(-EFAULT);
                return;
            }
                        
            t->syscall_return(result);
            break;
        };

        case 1: {       /* IPC_SET */
            semid64_ds_32 guest_ds;
            if (sizeof(guest_ds)!=t->get_process()->mem_read(&guest_ds, guest_semun.ptr, sizeof(guest_ds))) {
                t->syscall_return(-EFAULT);
                return;
            }
#ifdef SYS_ipc  /* i686 */
            ROSE_ASSERT(version!=0);
            semun_native semun;
            semun.ptr = &guest_ds;
            int result = syscall(SYS_ipc, 3/*SEMCTL*/, semid, semnum, cmd|version, &semun);
#else           /* amd64 */
            semid_ds host_ds;
            host_ds.sem_perm.__key = guest_ds.sem_perm.key;
            host_ds.sem_perm.uid = guest_ds.sem_perm.uid;
            host_ds.sem_perm.gid = guest_ds.sem_perm.gid;
            host_ds.sem_perm.cuid = guest_ds.sem_perm.cuid;
            host_ds.sem_perm.cgid = guest_ds.sem_perm.cgid;
            host_ds.sem_perm.mode = guest_ds.sem_perm.mode;
            host_ds.sem_perm.__pad1 = guest_ds.sem_perm.pad1;
            host_ds.sem_perm.__seq = guest_ds.sem_perm.seq;
            host_ds.sem_perm.__pad2 = guest_ds.sem_perm.pad2;
            host_ds.sem_perm.__unused1 = guest_ds.sem_perm.unused1;
            host_ds.sem_perm.__unused1 = guest_ds.sem_perm.unused2;
            host_ds.sem_otime = guest_ds.sem_otime;
            host_ds.__unused1 = guest_ds.unused1;
            host_ds.sem_ctime = guest_ds.sem_ctime;
            host_ds.__unused2 = guest_ds.unused2;
            host_ds.sem_nsems = guest_ds.sem_nsems;
            host_ds.__unused3 = guest_ds.unused3;
            host_ds.__unused4 = guest_ds.unused4;
            int result = syscall(SYS_semctl, semid, semnum, cmd, &host_ds);
#endif
            t->syscall_return(-1==result?-errno:result);
            break;
        }

        case 13: {      /* GETALL */
            semid_ds host_ds;
            int result = semctl(semid, -1, IPC_STAT, &host_ds);
            if (-1==result) {
                t->syscall_return(-errno);
                return;
            }
            if (host_ds.sem_nsems<1) {
                t->syscall_return(-EINVAL);
                return;
            }
            size_t nbytes = 2 * host_ds.sem_nsems;
            if (NULL==t->get_process()->my_addr(guest_semun.ptr, nbytes)) {
                t->syscall_return(-EFAULT);
                return;
            }
            uint16_t *sem_values = new uint16_t[host_ds.sem_nsems];
#ifdef SYS_ipc  /* i686 */
            semun_native semun;
            semun.ptr = sem_values;
            result = syscall(SYS_ipc, 3/*SEMCTL*/, semid, semnum, cmd|version, &semun);
#else
            result = syscall(SYS_semctl, semid, semnum, cmd, sem_values);
#endif
            if (-1==result) {
                delete[] sem_values;
                t->syscall_return(-errno);
                return;
            }
            if (nbytes!=t->get_process()->mem_write(sem_values, guest_semun.ptr, nbytes)) {
                delete[] sem_values;
                t->syscall_return(-EFAULT);
                return;
            }
            if (host_ds.sem_nsems>0) {
                for (size_t i=0; i<host_ds.sem_nsems; i++) {
                    strace->mesg("    value[%zu] = %"PRId16"\n", i, sem_values[i]);
                }
            }
            delete[] sem_values;
            t->syscall_return(result);
            break;
        }
            
        case 17: {      /* SETALL */
            semid_ds host_ds;
            int result = semctl(semid, -1, IPC_STAT, &host_ds);
            if (-1==result) {
                t->syscall_return(-errno);
                return;
            }
            if (host_ds.sem_nsems<1) {
                t->syscall_return(-EINVAL);
                return;
            }
            uint16_t *sem_values = new uint16_t[host_ds.sem_nsems];
            size_t nbytes = 2 * host_ds.sem_nsems;
            if (nbytes!=t->get_process()->mem_read(sem_values, guest_semun.ptr, nbytes)) {
                delete[] sem_values;
                t->syscall_return(-EFAULT);
                return;
            }
            for (size_t i=0; i<host_ds.sem_nsems; i++) {
                strace->more("    value[%zu] = %"PRId16"\n", i, sem_values[i]);
            }
#ifdef SYS_ipc  /* i686 */
            semun_native semun;
            semun.ptr = sem_values;
            result = syscall(SYS_ipc, 3/*SEMCTL*/, semid, semnum, cmd|version, &semun);
#else
            result = syscall(SYS_semctl, semid, semnum, cmd, sem_values);
#endif
            t->syscall_return(-1==result?-errno:result);
            delete[] sem_values;
            break;
        }

        case 11:        /* GETPID */
        case 12:        /* GETVAL */
        case 15:        /* GETZCNT */
        case 14: {      /* GETNCNT */
            int result = semctl(semid, semnum, cmd, NULL);
            t->syscall_return(-1==result?-errno:result);
            break;
        }

        case 16: {      /* SETVAL */
#ifdef SYS_ipc  /* i686 */
            int result = syscall(SYS_ipc, 3/*SEMCTL*/, semid, semnum, cmd|version, &guest_semun);
#else
            int result = syscall(SYS_semctl, semid, semnum, cmd, guest_semun.val);
#endif
            t->syscall_return(-1==result?-errno:result);
            break;
        }

        case 0: {       /* IPC_RMID */
#ifdef SYS_ipc /* i686 */
            semun_native host_semun;
            memset(&host_semun, 0, sizeof host_semun);
            int result = syscall(SYS_ipc, 3/*SEMCTL*/, semid, semnum, cmd|version, &host_semun);
#else
            int result = semctl(semid, semnum, cmd, NULL);
#endif
            t->syscall_return(-1==result?-errno:result);
            break;
        }

        default:
            t->syscall_return(-EINVAL);
            return;
    }
}

static void
sys_msgsnd(RSIM_Thread *t, uint32_t msqid, uint32_t msgp_va, uint32_t msgsz, uint32_t msgflg)
{
    if (msgsz>65535) { /* 65535 >= MSGMAX; smaller limit errors are detected in actual syscall */
        t->syscall_return(-EINVAL);
        return;
    }

    /* Read the message buffer from the specimen. */
    uint8_t *buf = new uint8_t[msgsz+8]; /* msgsz does not include "long mtype", only "char mtext[]" */
    if (!buf) {
        t->syscall_return(-ENOMEM);
        return;
    }
    if (4+msgsz!=t->get_process()->mem_read(buf, msgp_va, 4+msgsz)) {
        delete[] buf;
        t->syscall_return(-EFAULT);
        return;
    }

    /* Message type must be positive */
    if (*(int32_t*)buf <= 0) {
        delete[] buf;
        t->syscall_return(-EINVAL);
        return;
    }

    /* Convert message type from four to eight bytes if necessary */
    if (4!=sizeof(long)) {
        ROSE_ASSERT(8==sizeof(long));
        memmove(buf+8, buf+4, msgsz);
        memset(buf+4, 0, 4);
    }

    /* Try to send the message */
    int result = msgsnd(msqid, buf, msgsz, msgflg);
    if (-1==result) {
        delete[] buf;
        t->syscall_return(-errno);
        return;
    }

    delete[] buf;
    t->syscall_return(result);
}

static void
sys_msgrcv(RSIM_Thread *t, uint32_t msqid, uint32_t msgp_va, uint32_t msgsz, uint32_t msgtyp, uint32_t msgflg)
{
    if (msgsz>65535) { /* 65535 >= MSGMAX; smaller limit errors are detected in actual syscall */
        t->syscall_return(-EINVAL);
        return;
    }

    uint8_t *buf = new uint8_t[msgsz+8]; /* msgsz does not include "long mtype", only "char mtext[]" */
    int result = msgrcv(msqid, buf, msgsz, msgtyp, msgflg);
    if (-1==result) {
        delete[] buf;
        t->syscall_return(-errno);
        return;
    }

    if (4!=sizeof(long)) {
        ROSE_ASSERT(8==sizeof(long));
        uint64_t type = *(uint64_t*)buf;
        ROSE_ASSERT(0 == (type >> 32));
        memmove(buf+4, buf+8, msgsz);
    }

    if (4+msgsz!=t->get_process()->mem_write(buf, msgp_va, 4+msgsz)) {
        delete[] buf;
        t->syscall_return(-EFAULT);
        return;
    }

    delete[] buf;
    t->syscall_return(result);
}

static void
sys_msgget(RSIM_Thread *t, uint32_t key, uint32_t msgflg)
{
    int result = msgget(key, msgflg);
    t->syscall_return(-1==result?-errno:result);
}

static void
sys_msgctl(RSIM_Thread *t, uint32_t msqid, uint32_t cmd, uint32_t buf_va)
{
    int version = cmd & 0x0100/*IPC_64*/;
    cmd &= ~0x0100;

    switch (cmd) {
        case 3:    /* IPC_INFO */
        case 12: { /* MSG_INFO */
            t->syscall_return(-ENOSYS);              /* FIXME */
            break;
        }

        case 2:    /* IPC_STAT */
        case 11: { /* MSG_STAT */
            ROSE_ASSERT(0x0100==version); /* we're assuming ipc64_perm and msqid_ds from the kernel */
            static msqid_ds host_ds;
            int result = msgctl(msqid, cmd, &host_ds);
            if (-1==result) {
                t->syscall_return(-errno);
                break;
            }

            msqid64_ds_32 guest_ds;
            guest_ds.msg_perm.key = host_ds.msg_perm.__key;
            guest_ds.msg_perm.uid = host_ds.msg_perm.uid;
            guest_ds.msg_perm.gid = host_ds.msg_perm.gid;
            guest_ds.msg_perm.cuid = host_ds.msg_perm.cuid;
            guest_ds.msg_perm.cgid = host_ds.msg_perm.cgid;
            guest_ds.msg_perm.mode = host_ds.msg_perm.mode;
            guest_ds.msg_perm.pad1 = host_ds.msg_perm.__pad1;
            guest_ds.msg_perm.seq = host_ds.msg_perm.__seq;
            guest_ds.msg_perm.pad2 = host_ds.msg_perm.__pad2;
            guest_ds.msg_perm.unused1 = host_ds.msg_perm.__unused1;
            guest_ds.msg_perm.unused2 = host_ds.msg_perm.__unused2;
            guest_ds.msg_stime = host_ds.msg_stime;
#if 4==SIZEOF_LONG
            guest_ds.unused1 = host_ds.__unused1;
#endif
            guest_ds.msg_rtime = host_ds.msg_rtime;
#if 4==SIZEOF_LONG
            guest_ds.unused2 = host_ds.__unused2;
#endif
            guest_ds.msg_ctime = host_ds.msg_ctime;
#if 4==SIZEOF_LONG
            guest_ds.unused3 = host_ds.__unused3;
#endif
            guest_ds.msg_cbytes = host_ds.__msg_cbytes;
            guest_ds.msg_qnum = host_ds.msg_qnum;
            guest_ds.msg_qbytes = host_ds.msg_qbytes;
            guest_ds.msg_lspid = host_ds.msg_lspid;
            guest_ds.msg_lrpid = host_ds.msg_lrpid;
            guest_ds.unused4 = host_ds.__unused4;
            guest_ds.unused5 = host_ds.__unused5;

            if (sizeof(guest_ds)!=t->get_process()->mem_write(&guest_ds, buf_va, sizeof guest_ds)) {
                t->syscall_return(-EFAULT);
                break;
            }

            t->syscall_return(result);
            break;
        }

        case 0: { /* IPC_RMID */
            /* NOTE: syscall tracing will not show "IPC_RMID" if the IPC_64 flag is also present */
            int result = msgctl(msqid, cmd, NULL);
            t->syscall_return(-1==result?-errno:result);
            break;
        }

        case 1: { /* IPC_SET */
            msqid64_ds_32 guest_ds;
            if (sizeof(guest_ds)!=t->get_process()->mem_read(&guest_ds, buf_va, sizeof guest_ds)) {
                t->syscall_return(-EFAULT);
                break;
            }

            static msqid_ds host_ds;
            host_ds.msg_perm.__key = guest_ds.msg_perm.key;
            host_ds.msg_perm.uid = guest_ds.msg_perm.uid;
            host_ds.msg_perm.gid = guest_ds.msg_perm.gid;
            host_ds.msg_perm.cuid = guest_ds.msg_perm.cuid;
            host_ds.msg_perm.cgid = guest_ds.msg_perm.cgid;
            host_ds.msg_perm.mode = guest_ds.msg_perm.mode;
            host_ds.msg_perm.__seq = guest_ds.msg_perm.seq;
            host_ds.msg_stime = guest_ds.msg_stime;
            host_ds.msg_rtime = guest_ds.msg_rtime;
            host_ds.msg_ctime = guest_ds.msg_ctime;
            host_ds.__msg_cbytes = guest_ds.msg_cbytes;
            host_ds.msg_qnum = guest_ds.msg_qnum;
            host_ds.msg_qbytes = guest_ds.msg_qbytes;
            host_ds.msg_lspid = guest_ds.msg_lspid;
            host_ds.msg_lrpid = guest_ds.msg_lrpid;

            int result = msgctl(msqid, cmd, &host_ds);
            t->syscall_return(-1==result?-errno:result);
            break;
        }

        default: {
            t->syscall_return(-EINVAL);
            break;
        }
    }
}

static void
sys_shmdt(RSIM_Thread *t, uint32_t shmaddr_va)
{
    int result = -ENOSYS;

    RTS_WRITE(t->get_process()->rwlock()) {
        const MemoryMap::MapElement *me = t->get_process()->get_memory()->find(shmaddr_va);
        if (!me || me->get_va()!=shmaddr_va || me->get_offset()!=0 || me->is_anonymous()) {
            result = -EINVAL;
            break;
        }

        result = shmdt(me->get_base());
        if (-1==result) {
            result = -errno;
            break;
        }

        t->get_process()->mem_unmap(me->get_va(), me->get_size(), t->tracing(TRACE_MMAP));
        result = 0;
    } RTS_WRITE_END;
    t->syscall_return(result);
}

static void
sys_shmget(RSIM_Thread *t, uint32_t key, uint32_t size, uint32_t shmflg)
{
    int result = shmget(key, size, shmflg);
    t->syscall_return(-1==result?-errno:result);
}

static void
sys_shmctl(RSIM_Thread *t, uint32_t shmid, uint32_t cmd, uint32_t buf_va)
{
    int version = cmd & 0x0100/*IPC_64*/;
    cmd &= ~0x0100;

    switch (cmd) {
        case 13:  /* SHM_STAT */
        case 2: { /* IPC_STAT */
            ROSE_ASSERT(0x0100==version); /* we're assuming ipc64_perm and shmid_ds from the kernel */
            static shmid_ds host_ds;
            int result = shmctl(shmid, cmd, &host_ds);
            if (-1==result) {
                t->syscall_return(-errno);
                break;
            }

            shmid64_ds_32 guest_ds;
            guest_ds.shm_perm.key = host_ds.shm_perm.__key;
            guest_ds.shm_perm.uid = host_ds.shm_perm.uid;
            guest_ds.shm_perm.gid = host_ds.shm_perm.gid;
            guest_ds.shm_perm.cuid = host_ds.shm_perm.cuid;
            guest_ds.shm_perm.cgid = host_ds.shm_perm.cgid;
            guest_ds.shm_perm.mode = host_ds.shm_perm.mode;
            guest_ds.shm_perm.pad1 = host_ds.shm_perm.__pad1;
            guest_ds.shm_perm.seq = host_ds.shm_perm.__seq;
            guest_ds.shm_perm.pad2 = host_ds.shm_perm.__pad2;
            guest_ds.shm_perm.unused1 = host_ds.shm_perm.__unused1;
            guest_ds.shm_perm.unused2 = host_ds.shm_perm.__unused2;
            guest_ds.shm_segsz = host_ds.shm_segsz;
            guest_ds.shm_atime = host_ds.shm_atime;
#if 4==SIZEOF_LONG
            guest_ds.unused1 = host_ds.__unused1;
#endif
            guest_ds.shm_dtime = host_ds.shm_dtime;
#if 4==SIZEOF_LONG
            guest_ds.unused2 = host_ds.__unused2;
#endif
            guest_ds.shm_ctime = host_ds.shm_ctime;
#if 4==SIZEOF_LONG
            guest_ds.unused3 = host_ds.__unused3;
#endif
            guest_ds.shm_cpid = host_ds.shm_cpid;
            guest_ds.shm_lpid = host_ds.shm_lpid;
            guest_ds.shm_nattch = host_ds.shm_nattch;
            guest_ds.unused4 = host_ds.__unused4;
            guest_ds.unused5 = host_ds.__unused5;

            if (sizeof(guest_ds)!=t->get_process()->mem_write(&guest_ds, buf_va, sizeof guest_ds)) {
                t->syscall_return(-EFAULT);
                break;
            }

            t->syscall_return(result);
            break;
        }

        case 14: { /* SHM_INFO */
            shm_info host_info;
            int result = shmctl(shmid, cmd, (shmid_ds*)&host_info);
            if (-1==result) {
                t->syscall_return(-errno);
                break;
            }

            shm_info_32 guest_info;
            guest_info.used_ids = host_info.used_ids;
            guest_info.shm_tot = host_info.shm_tot;
            guest_info.shm_rss = host_info.shm_rss;
            guest_info.shm_swp = host_info.shm_swp;
            guest_info.swap_attempts = host_info.swap_attempts;
            guest_info.swap_successes = host_info.swap_successes;

            if (sizeof(guest_info)!=t->get_process()->mem_write(&guest_info, buf_va, sizeof guest_info)) {
                t->syscall_return(-EFAULT);
                break;
            }

            t->syscall_return(result);
            break;
        }

        case 3: { /* IPC_INFO */
            shminfo64_native host_info;
            int result = shmctl(shmid, cmd, (shmid_ds*)&host_info);
            if (-1==result) {
                t->syscall_return(-errno);
                return;
            }

            shminfo64_32 guest_info;
            guest_info.shmmax = host_info.shmmax;
            guest_info.shmmin = host_info.shmmin;
            guest_info.shmmni = host_info.shmmni;
            guest_info.shmseg = host_info.shmseg;
            guest_info.shmall = host_info.shmall;
            guest_info.unused1 = host_info.unused1;
            guest_info.unused2 = host_info.unused2;
            guest_info.unused3 = host_info.unused3;
            guest_info.unused4 = host_info.unused4;
            if (sizeof(guest_info)!=t->get_process()->mem_write(&guest_info, buf_va, sizeof guest_info)) {
                t->syscall_return(-EFAULT);
                return;
            }

            t->syscall_return(result);
            break;
        }

        case 11:   /* SHM_LOCK */
        case 12: { /* SHM_UNLOCK */
            int result = shmctl(shmid, cmd, NULL);
            t->syscall_return(-1==result?-errno:result);
            break;
        }

        case 1: { /* IPC_SET */
            ROSE_ASSERT(version!=0);
            shmid64_ds_32 guest_ds;
            if (sizeof(guest_ds)!=t->get_process()->mem_read(&guest_ds, buf_va, sizeof guest_ds)) {
                t->syscall_return(-EFAULT);
                return;
            }
            shmid_ds host_ds;
            host_ds.shm_perm.__key = guest_ds.shm_perm.key;
            host_ds.shm_perm.uid = guest_ds.shm_perm.uid;
            host_ds.shm_perm.gid = guest_ds.shm_perm.gid;
            host_ds.shm_perm.cuid = guest_ds.shm_perm.cuid;
            host_ds.shm_perm.cgid = guest_ds.shm_perm.cgid;
            host_ds.shm_perm.mode = guest_ds.shm_perm.mode;
            host_ds.shm_perm.__pad1 = guest_ds.shm_perm.pad1;
            host_ds.shm_perm.__seq = guest_ds.shm_perm.seq;
            host_ds.shm_perm.__pad2 = guest_ds.shm_perm.pad2;
            host_ds.shm_perm.__unused1 = guest_ds.shm_perm.unused1;
            host_ds.shm_perm.__unused2 = guest_ds.shm_perm.unused2;
            host_ds.shm_segsz = guest_ds.shm_segsz;
            host_ds.shm_atime = guest_ds.shm_atime;
#if 4==SIZEOF_LONG
            host_ds.__unused1 = guest_ds.unused1;
#endif
            host_ds.shm_dtime = guest_ds.shm_dtime;
#if 4==SIZEOF_LONG
            host_ds.__unused2 = guest_ds.unused2;
#endif
            host_ds.shm_ctime = guest_ds.shm_ctime;
#if 4==SIZEOF_LONG
            host_ds.__unused3 = guest_ds.unused3;
#endif
            host_ds.shm_cpid = guest_ds.shm_cpid;
            host_ds.shm_lpid = guest_ds.shm_lpid;
            host_ds.shm_nattch = guest_ds.shm_nattch;
            host_ds.__unused4 = guest_ds.unused4;
            host_ds.__unused5 = guest_ds.unused5;

            int result = shmctl(shmid, cmd, &host_ds);
            t->syscall_return(-1==result?-errno:result);
            break;
        }

        case 0: { /* IPC_RMID */
            int result = shmctl(shmid, cmd, NULL);
            t->syscall_return(-1==result?-errno:result);
            break;
        }

        default: {
            t->syscall_return(-EINVAL);
            break;
        }
    }
}

static void
sys_shmat(RSIM_Thread *t, uint32_t shmid, uint32_t shmflg, uint32_t result_va, uint32_t shmaddr)
{
    int result = -ENOSYS;

    RTS_WRITE(t->get_process()->rwlock()) {
        if (0==shmaddr) {
            shmaddr = t->get_process()->get_memory()->find_last_free();
        } else if (shmflg & SHM_RND) {
            shmaddr = ALIGN_DN(shmaddr, SHMLBA);
        } else if (ALIGN_DN(shmaddr, 4096)!=shmaddr) {
            result = -EINVAL;
            break;
        }

        /* We don't handle SHM_REMAP */
        if (shmflg & SHM_REMAP) {
            result = -EINVAL;
            break;
        }

        /* Map shared memory into the simulator. It's OK to hold the write lock here because this syscall doesn't block. */
        void *buf = shmat(shmid, NULL, shmflg);
        if (!buf) {
            result = -errno;
            break;
        }

        /* Map simulator's shared memory into the specimen */
        shmid_ds ds;
        int status = shmctl(shmid, IPC_STAT, &ds); // does not block
        ROSE_ASSERT(status>=0);
        ROSE_ASSERT(ds.shm_segsz>0);
        unsigned perms = MemoryMap::MM_PROT_READ | ((shmflg & SHM_RDONLY) ? 0 : MemoryMap::MM_PROT_WRITE);
        MemoryMap::MapElement shm(shmaddr, ds.shm_segsz, buf, 0, perms);
        shm.set_name("shmat("+StringUtility::numberToString(shmid)+")");
        t->get_process()->get_memory()->insert(shm);

        /* Return values */
        if (4!=t->get_process()->mem_write(&shmaddr, result_va, 4)) {
            result = -EFAULT;
            break;
        }
        t->syscall_return(shmaddr);
        result = 0;
    } RTS_WRITE_END;
    if (result)
        t->syscall_return(result);
}

static void
syscall_ipc(RSIM_Thread *t, int callno)
{
    /* Return value is written to eax by these helper functions. The structure of this code closely follows that in the
     * Linux kernel. */
    unsigned call = t->syscall_arg(0) & 0xffff;
    int version = t->syscall_arg(0) >> 16;
    uint32_t first=t->syscall_arg(1), second=t->syscall_arg(2), third=t->syscall_arg(3), ptr=t->syscall_arg(4), fifth=t->syscall_arg(5);
    switch (call) {
        case 1: /* SEMOP */
            sys_semtimedop(t, first, ptr, second, 0);
            break;
        case 2: /* SEMGET */
            sys_semget(t, first, second, third);
            break;
        case 3: /* SEMCTL */
            sys_semctl(t, first, second, third, ptr);
            break;
        case 4: /* SEMTIMEDOP */
            sys_semtimedop(t, first, ptr, second, fifth);
            break;
        case 11: /* MSGSND */
            sys_msgsnd(t, first, ptr, second, third);
            break;
        case 12: /* MSGRCV */
            if (0==version) {
                ipc_kludge_32 kludge;
                if (8!=t->get_process()->mem_read(&kludge, t->syscall_arg(4), 8)) {
                    t->syscall_return(-ENOSYS);
                } else {
                    sys_msgrcv(t, first, kludge.msgp, second, kludge.msgtyp, third);
                }
            } else {
                sys_msgrcv(t, first, ptr, second, fifth, third);
            }
            break;
        case 13: /* MSGGET */
            sys_msgget(t, first, second);
            break;
        case 14: /* MSGCTL */
            sys_msgctl(t, first, second, ptr);
            break;
        case 21: /* SHMAT */
            if (1==version) {
                /* This was the entry point for kernel-originating calls from iBCS2 in 2.2 days */
                t->syscall_return(-EINVAL);
            } else {
                sys_shmat(t, first, second, third, ptr);
            }
            break;
        case 22: /* SHMDT */
            sys_shmdt(t, ptr);
            break;
        case 23: /* SHMGET */
            sys_shmget(t, first, second, third);
            break;
        case 24: /* SHMCTL */
            sys_shmctl(t, first, second, ptr);
            break;
        default:
            t->syscall_return(-ENOSYS);
            break;
    }
}

static void
syscall_ipc_leave(RSIM_Thread *t, int callno)
{
    RTS_Message *mtrace = t->tracing(TRACE_MMAP);
    unsigned call = t->syscall_arg(0) & 0xffff;
    int version = t->syscall_arg(0) >> 16;
    uint32_t second=t->syscall_arg(2);
    switch (call) {
        case 14: /* MSGCTL */ {
            switch (second & 0xff) {
                case 3:         /* IPC_INFO */
                case 12:        /* MSG_INFO */
                    ROSE_ASSERT(!"not handled");
                case 2:         /* IPC_STAT */
                case 11:        /* MSG_STAT */
                    t->syscall_leave("d----P", sizeof(msqid64_ds_32), print_msqid64_ds_32);
                    break;
                default:
                    t->syscall_leave("d");
                    break;
            }
            break;
        }
        case 21: /* SHMAT */
            if (1==version) {
                t->syscall_leave("d");
            } else {
                t->syscall_leave("p");
                t->get_process()->mem_showmap(mtrace, "  memory map after shmat:\n");
            }
            break;
        case 22: /* SHMDT */
            t->syscall_leave("d");
            t->get_process()->mem_showmap(mtrace, "  memory map after shmdt:\n");
            break;
        case 24: { /* SHMCTL */
            switch (second & 0xff) {
                case 2:         /* IPC_STAT */
                case 13:        /* SHM_STAT */
                    t->syscall_leave("d----P", sizeof(shmid64_ds_32), print_shmid64_ds_32);
                    break;
                case 14:        /* SHM_INFO */
                    t->syscall_leave("d----P", sizeof(shm_info_32), print_shm_info_32);
                    break;
                case 3:         /* IPC_INFO */
                    t->syscall_leave("d----P", sizeof(shminfo64_32), print_shminfo64_32);
                    break;
                default:
                    t->syscall_leave("d");
                    break;
            }
            break;
        }
        default:
            t->syscall_leave("d");
            break;
    }
}

/*******************************************************************************************************************************/

static void
syscall_fsync_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("fsync", "d");
}

static void
syscall_fsync(RSIM_Thread *t, int callno)
{
    int result = fsync( t->syscall_arg(0));
    t->syscall_return(-1==result?-errno:result);
}

/*******************************************************************************************************************************/

static void
syscall_sigreturn_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("sigreturn", "");
}

static void
syscall_sigreturn(RSIM_Thread *t, int callno)
{
    int status = t->sys_sigreturn();
    if (status>=0) {
        t->tracing(TRACE_SYSCALL)->more(" = <does not return>\n");
        throw RSIM_SignalHandling::mk_kill(0, 0);
    }
    t->syscall_return(status); /* ERROR; specimen will likely segfault shortly! */
}

static void
syscall_sigreturn_leave(RSIM_Thread *t, int callno)
{
    /* This should not be reached, but might be reached if the sigreturn system call body was skipped over. */
    t->tracing(TRACE_SYSCALL)->more(" = <should not have returned>\n");
}

/*******************************************************************************************************************************/

static void
syscall_clone_enter(RSIM_Thread *t, int callno)
{
    /* From linux arch/x86/kernel/process.c:
     *    long sys_clone(unsigned long clone_flags, unsigned long newsp,
     *                   void __user *parent_tid, void __user *child_tid, struct pt_regs *regs)
     *
     * The fourth argument, child_tls_va, varies depending on clone_flags. Linux doesn't appear to require that
     * these are mutually exclusive.  It appears as though the CLONE_SETTLS happens before CLONE_CHILD_SETTID.
     *   CLONE_CHILD_SETTID:  it is an address where the child's TID is written when the child is created
     *   CLONE_SETTLS:        it is an address of a user_desc_32 which will be loaded into the GDT.
     */
    unsigned flags = t->syscall_arg(0);
    if (flags & CLONE_SETTLS) {
        t->syscall_enter("clone", "fppPP",
                      clone_flags,
                      sizeof(user_desc_32), print_user_desc_32,
                      sizeof(pt_regs_32), print_pt_regs_32);
    } else {
        t->syscall_enter("clone", "fpppP",
                      clone_flags,
                      sizeof(pt_regs_32), print_pt_regs_32);
    }
}

static int
sys_clone(RSIM_Thread *t, unsigned flags, uint32_t newsp, uint32_t parent_tid_va, uint32_t child_tls_va, uint32_t pt_regs_va)
{
    if (flags == (CLONE_CHILD_CLEARTID | CLONE_CHILD_SETTID | SIGCHLD)) {
        /* This is a fork() */

        /* Flush some files so buffered content isn't output twice. */
        fflush(stdout);
        fflush(stderr);
        if (t->get_process()->get_tracing_file())
            fflush(t->get_process()->get_tracing_file());

        /* We cannot use clone() because it's a wrapper around the clone system call and we'd need to provide a function for it to
         * execute. We want fork-like semantics. */
        pid_t pid = fork();
        if (-1==pid)
            return -errno;

        if (0==pid) {
            /* Kludge for now. FIXME [RPM 2011-02-14] */
            t->post_fork();

            /* Open new log files if necessary */
            t->get_process()->open_tracing_file();
            t->get_process()->btrace_close();

            /* Thread-related things. We have to initialize a few data structures because the specimen may be using a
             * thread-aware library. */
            if (0!=(flags & CLONE_CHILD_SETTID) && child_tls_va) {
                uint32_t pid32 = getpid();
                size_t nwritten = t->get_process()->mem_write(&pid32, child_tls_va, 4);
                ROSE_ASSERT(4==nwritten);
            }
            if (0!=(flags & CLONE_CHILD_CLEARTID))
                t->clear_child_tid = parent_tid_va;

            /* Return register values in child */
            pt_regs_32 regs;
            regs.bx = t->policy.readGPR(x86_gpr_bx).known_value();
            regs.cx = t->policy.readGPR(x86_gpr_cx).known_value();
            regs.dx = t->policy.readGPR(x86_gpr_dx).known_value();
            regs.si = t->policy.readGPR(x86_gpr_si).known_value();
            regs.di = t->policy.readGPR(x86_gpr_di).known_value();
            regs.bp = t->policy.readGPR(x86_gpr_bp).known_value();
            regs.sp = t->policy.readGPR(x86_gpr_sp).known_value();
            regs.cs = t->policy.readSegreg(x86_segreg_cs).known_value();
            regs.ds = t->policy.readSegreg(x86_segreg_ds).known_value();
            regs.es = t->policy.readSegreg(x86_segreg_es).known_value();
            regs.fs = t->policy.readSegreg(x86_segreg_fs).known_value();
            regs.gs = t->policy.readSegreg(x86_segreg_gs).known_value();
            regs.ss = t->policy.readSegreg(x86_segreg_ss).known_value();
            uint32_t flags = 0;
            for (size_t i=0; i<VirtualMachineSemantics::State::n_flags; i++) {
                if (t->policy.readFlag((X86Flag)i).known_value()) {
                    flags |= (1u<<i);
                }
            }
            if (sizeof(regs)!=t->get_process()->mem_write(&regs, pt_regs_va, sizeof regs))
                return -EFAULT;
        }

        return pid;
        
    } else if (flags == (CLONE_VM |
                         CLONE_FS |
                         CLONE_FILES |
                         CLONE_SIGHAND |
                         CLONE_THREAD |
                         CLONE_SYSVSEM |
                         CLONE_SETTLS |
                         CLONE_PARENT_SETTID |
                         CLONE_CHILD_CLEARTID)) {
        /* we are creating a new thread */
        pt_regs_32 regs = t->get_regs();
        regs.sp = newsp;
        regs.ax = 0;

        pid_t tid = t->get_process()->clone_thread(t, flags, parent_tid_va, child_tls_va, regs);
        return tid;
    } else {
        return -EINVAL; /* can't handle this combination of flags */
    }
}

static void
syscall_clone(RSIM_Thread *t, int callno)
{
    unsigned flags = t->syscall_arg(0);
    uint32_t newsp = t->syscall_arg(1);
    uint32_t parent_tid_va = t->syscall_arg(2);
    uint32_t child_tls_va = t->syscall_arg(3);
    uint32_t regs_va = t->syscall_arg(4);
    t->syscall_return(sys_clone(t, flags, newsp, parent_tid_va, child_tls_va, regs_va));
}

static void
syscall_clone_leave(RSIM_Thread *t, int callno)
{
    if (t->syscall_arg(-1)) {
        /* Parent */
        t->syscall_leave("d");
    } else {
        /* Child returns here for fork, but not for thread-clone */
        t->syscall_enter("child's clone", "fpppP", clone_flags, sizeof(pt_regs_32), print_pt_regs_32);
        t->syscall_leave("d----P", sizeof(pt_regs_32), print_pt_regs_32);
    }
}

/*******************************************************************************************************************************/

static void
syscall_uname_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("uname", "p");
}

static void
syscall_uname(RSIM_Thread *t, int callno)
{
    uint32_t dest_va=t->syscall_arg(0);
    char buf[6*65];
    memset(buf, ' ', sizeof buf);
    strcpy(buf+0*65, "Linux");                                  /*sysname*/
    strcpy(buf+1*65, "mymachine.example.com");                  /*nodename*/
    strcpy(buf+2*65, "2.6.9");                                  /*release*/
    strcpy(buf+3*65, "#1 SMP Wed Jun 18 12:35:02 EDT 2008");    /*version*/
    strcpy(buf+4*65, "i386");                                   /*machine*/
    strcpy(buf+5*65, "example.com");                            /*domainname*/
    size_t nwritten = t->get_process()->mem_write(buf, dest_va, sizeof buf);
    if( nwritten <= 0 ) {
        t->syscall_return(-EFAULT);
        return;
    }

    ROSE_ASSERT(nwritten==sizeof buf);
    t->syscall_return(0);
}

/*******************************************************************************************************************************/

static void
syscall_fchdir_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("fchdir", "d");
}

static void
syscall_fchdir(RSIM_Thread *t, int callno)
{
    uint32_t file_descriptor = t->syscall_arg(0);

    int result = fchdir(file_descriptor);
    if (result == -1) result = -errno;
    t->syscall_return(result);
}

/*******************************************************************************************************************************/

static void
syscall_mprotect_enter(RSIM_Thread *t, int callno)
{
    static const Translate pflags[] = { TF(PROT_READ), TF(PROT_WRITE), TF(PROT_EXEC), TF(PROT_NONE), T_END };
    t->syscall_enter("mprotect", "pdf", pflags);
}

static void
syscall_mprotect(RSIM_Thread *t, int callno)
{
    uint32_t va=t->syscall_arg(0), size=t->syscall_arg(1), real_perms=t->syscall_arg(2);
    unsigned rose_perms = ((real_perms & PROT_READ) ? MemoryMap::MM_PROT_READ : 0) |
                          ((real_perms & PROT_WRITE) ? MemoryMap::MM_PROT_WRITE : 0) |
                          ((real_perms & PROT_EXEC) ? MemoryMap::MM_PROT_EXEC : 0);
    if (va % PAGE_SIZE) {
        t->syscall_return(-EINVAL);
    } else {
        uint32_t aligned_sz = ALIGN_UP(size, PAGE_SIZE);
        t->syscall_return(t->get_process()->mem_protect(va, aligned_sz, rose_perms, real_perms));
    }
}

static void
syscall_mprotect_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave("d");
    t->get_process()->mem_showmap(t->tracing(TRACE_MMAP), "  memory map after mprotect syscall:\n");
}

/*******************************************************************************************************************************/

static void
syscall_llseek_enter(RSIM_Thread *t, int callno)
{
    /* From the linux kernel, arguments are:
     *      unsigned int fd,                // file descriptor
     *      unsigned long offset_high,      // high 32 bits of 64-bit offset
     *      unsigned long offset_low,       // low 32 bits of 64-bit offset
     *      loff_t __user *result,          // 64-bit user area to write resulting position
     *      unsigned int origin             // whence specified offset is measured
     */
    t->syscall_enter("llseek","dddpf", seek_whence);
}

static void
syscall_llseek(RSIM_Thread *t, int callno)
{
    int fd = t->syscall_arg(0);
    off64_t offset = ((off64_t)t->syscall_arg(1) << 32) | t->syscall_arg(2);
    uint32_t result_va = t->syscall_arg(3);
    int whence = t->syscall_arg(4);

    off64_t result = lseek64(fd, offset, whence);
    if (-1==result) {
        t->syscall_return(-errno);
    } else {
        t->syscall_return(0);
        size_t nwritten = t->get_process()->mem_write(&result, result_va, sizeof result);
        ROSE_ASSERT(nwritten==sizeof result);
    }
}

/*******************************************************************************************************************************/

static void
syscall_getdents_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("getdents", "dpd");
}

static void
syscall_getdents(RSIM_Thread *t, int callno)
{
    int fd = t->syscall_arg(0), sz = t->syscall_arg(2);
    uint32_t dirent_va = t->syscall_arg(1);
    int status = t->getdents_syscall<dirent32_t>(fd, dirent_va, sz);
    t->syscall_return(status);
}

static void
syscall_getdents_leave(RSIM_Thread *t, int callno)
{
    int status = t->syscall_arg(-1);
    t->syscall_leave("d-P", status>0?status:0, print_dentries_32);
}

/*******************************************************************************************************************************/

static void
syscall_select_enter(RSIM_Thread *t, int callno)
{
    /* From the Linux kernel (fs/select.c):
     *    SYSCALL_DEFINE5(select, int, n, fd_set __user *, inp, fd_set __user *, outp,
     *                    fd_set __user *, exp, struct timeval __user *, tvp)
     * where:
     *    fd_set is enough "unsigned long" data to contain 1024 bits. Regardless of the size of "unsigned long",
     *    the file bits will be in the same order (we are the host is little endian), and the fd_set is the same size. */
    t->syscall_enter("select", "dPPPP",
                  sizeof(fd_set), print_bitvec,
                  sizeof(fd_set), print_bitvec,
                  sizeof(fd_set), print_bitvec,
                  sizeof(timeval_32), print_timeval_32);
}

static void
syscall_select(RSIM_Thread *t, int callno)
{
    int fd = t->syscall_arg(0);
    uint32_t in_va=t->syscall_arg(1), out_va=t->syscall_arg(2), ex_va=t->syscall_arg(3), tv_va=t->syscall_arg(4);

    fd_set in, out, ex;
    fd_set *inp=NULL, *outp=NULL, *exp=NULL;

    ROSE_ASSERT(128==sizeof(fd_set)); /* 128 bytes = 1024 file descriptor bits */
    if (in_va && sizeof(in)==t->get_process()->mem_read(&in, in_va, sizeof in))
        inp = &in;
    if (out_va && sizeof(out)==t->get_process()->mem_read(&out, out_va, sizeof out))
        outp = &out;
    if (ex_va && sizeof(ex)==t->get_process()->mem_read(&ex, ex_va, sizeof ex))
        exp = &ex;

    timeval_32 guest_timeout;
    timeval host_timeout, *tvp=NULL;
    if (tv_va) {
        if (sizeof(guest_timeout)!=t->get_process()->mem_read(&guest_timeout, tv_va, sizeof guest_timeout)) {
            t->syscall_return(-EFAULT);
            return;
        } else {
            host_timeout.tv_sec = guest_timeout.tv_sec;
            host_timeout.tv_usec = guest_timeout.tv_usec;
            tvp = &host_timeout;
        }
    }

    int result = select(fd, inp, outp, exp, tvp);
    if (-1==result) {
        t->syscall_return(-errno);
        return;
    }

    if ((in_va  && sizeof(in) !=t->get_process()->mem_write(inp,  in_va,  sizeof in))  ||
        (out_va && sizeof(out)!=t->get_process()->mem_write(outp, out_va, sizeof out)) ||
        (ex_va  && sizeof(ex) !=t->get_process()->mem_write(exp,  ex_va,  sizeof ex))) {
        t->syscall_return(-EFAULT);
        return;
    }

    if (tvp) {
        guest_timeout.tv_sec = tvp->tv_sec;
        guest_timeout.tv_usec = tvp->tv_usec;
        if (sizeof(guest_timeout)!=t->get_process()->mem_write(&guest_timeout, tv_va, sizeof guest_timeout)) {
            t->syscall_return(-EFAULT);
            return;
        }
    }
}

static void
syscall_select_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave("d-PPPP",
                  sizeof(fd_set), print_bitvec,
                  sizeof(fd_set), print_bitvec,
                  sizeof(fd_set), print_bitvec,
                  sizeof(timeval_32), print_timeval_32);
}

/*******************************************************************************************************************************/

static void
syscall_msync_enter(RSIM_Thread *t, int callno)
{
    static const Translate msync_flags[] = { TF(MS_ASYNC), TF(MS_SYNC), TF(MS_INVALIDATE), T_END };
    t->syscall_enter("msync", "pdf", msync_flags);
}

static void
syscall_msync(RSIM_Thread *t, int callno)
{
    if (t->syscall_arg(0) % 4096) {
        t->syscall_return(-EINVAL);
        return;
    }

    void *addr = t->get_process()->my_addr(t->syscall_arg(0), t->syscall_arg(1));
    if (!addr) {
        t->syscall_return(-ENOMEM);
    } else if (-1==msync(addr, t->syscall_arg(1), t->syscall_arg(2))) {
        t->syscall_return(-errno);
    } else {
        t->syscall_return(0);
    }
}

/*******************************************************************************************************************************/

static void
syscall_writev_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("writev", "dpd");
}

static void
syscall_writev(RSIM_Thread *t, int callno)
{
    uint32_t fd=t->syscall_arg(0), iov_va=t->syscall_arg(1);
    int niov=t->syscall_arg(2), idx=0;
    int retval = 0;
    if (niov<0 || niov>1024) {
        retval = -EINVAL;
    } else {
        if (niov>0)
            t->tracing(TRACE_SYSCALL)->more("\n");
        for (idx=0; idx<niov; idx++) {
            /* Obtain buffer address and size */
            uint32_t buf_va;
            if (4 != t->get_process()->mem_read(&buf_va, iov_va+idx*8+0, 4)) {
                if (0==idx)
                    retval = -EFAULT;
                t->tracing(TRACE_SYSCALL)->more("    #%d: segmentation fault reading address\n", idx);
                break;
            }

            uint32_t buf_sz;
            if (4 != t->get_process()->mem_read(&buf_sz, iov_va+idx*8+4, 4)) {
                if (0==idx)
                    retval = -EFAULT;
                t->tracing(TRACE_SYSCALL)->more("    #%d: segmentation fault reading size\n", idx);
                break;
            }

            t->tracing(TRACE_SYSCALL)->more("    #%d: va=0x%08"PRIx32", size=0x%08"PRIx32, idx, buf_va, buf_sz);

            /* Make sure total size doesn't overflow a ssize_t */
            if ((buf_sz & 0x80000000) || ((uint32_t)retval+buf_sz) & 0x80000000) {
                if (0==idx)
                    retval = -EINVAL;
                t->tracing(TRACE_SYSCALL)->more(" size overflow\n");
                break;
            }

            /* Copy data from guest to host because guest memory might not be contiguous in the host. Perhaps a more
             * efficient way to do this would be to copy chunks of host-contiguous data in a loop instead. */
            uint8_t buf[buf_sz];
            if (buf_sz != t->get_process()->mem_read(buf, buf_va, buf_sz)) {
                if (0==idx)
                    retval = -EFAULT;
                t->tracing(TRACE_SYSCALL)->more(" segmentation fault\n");
                break;
            }

            /* Write data to the file */
            ssize_t nwritten = write(fd, buf, buf_sz);
            if (-1==nwritten) {
                if (0==idx)
                    retval = -errno;
                t->tracing(TRACE_SYSCALL)->more(" write failed (%s)\n", strerror(errno));
                break;
            }
            retval += nwritten;
            if ((uint32_t)nwritten<buf_sz) {
                t->tracing(TRACE_SYSCALL)->more(" short write (%zd bytes)\n", nwritten);
                break;
            }
            t->tracing(TRACE_SYSCALL)->more("\n");
        }
    }
    t->syscall_return(retval);
    if (niov>0 && niov<=1024)
        t->tracing(TRACE_SYSCALL)->more("writev return");
}

/*******************************************************************************************************************************/

static void
syscall_sched_setparam_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("sched_setparam", "dP", sizeof(sched_param_32), print_sched_param_32);
}

static void
syscall_sched_setparam(RSIM_Thread *t, int callno)
{
    pid_t pid = t->syscall_arg(0);
    rose_addr_t params_va = t->syscall_arg(1);

    sched_param_32 guest;
    if (sizeof(guest)!=t->get_process()->mem_read(&guest, params_va, sizeof guest)) {
        t->syscall_return(-EFAULT);
        return;
    }

    sched_param host;
    host.sched_priority = guest.sched_priority;

    int result = sched_setparam(pid, &host);
    t->syscall_return(-1==result ? -errno : result);
}

/*******************************************************************************************************************************/

static void
syscall_sched_getscheduler_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("sched_getscheduler", "d");
}

static void
syscall_sched_getscheduler(RSIM_Thread *t, int callno)
{
    pid_t pid = t->syscall_arg(0);
    int result = sched_getscheduler(pid);
    t->syscall_return(-1==result ? -errno : result);
}

static void
syscall_sched_getscheduler_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave("Df", scheduler_policies);
}

/*******************************************************************************************************************************/

static void
syscall_sched_get_priority_max_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("sched_get_priority_max", "f", scheduler_policies);
}

static void
syscall_sched_get_priority_max(RSIM_Thread *t, int callno)
{
    int policy = t->syscall_arg(0);
    int result = sched_get_priority_max(policy);
    t->syscall_return(result);
}

/*******************************************************************************************************************************/

static void
syscall_sched_get_priority_min_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("sched_get_priority_min", "f", scheduler_policies);
}

static void
syscall_sched_get_priority_min(RSIM_Thread *t, int callno)
{
    int policy = t->syscall_arg(0);
    int result = sched_get_priority_min(policy);
    t->syscall_return(result);
}

/*******************************************************************************************************************************/

static void
syscall_nanosleep_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("nanosleep", "Pp", sizeof(timespec_32), print_timespec_32);
}

static void
syscall_nanosleep(RSIM_Thread *t, int callno)
{
    timespec_32 guest_ts;
    timespec host_ts_in, host_ts_out;
    if (sizeof(guest_ts)!=t->get_process()->mem_read(&guest_ts, t->syscall_arg(0), sizeof guest_ts)) {
        t->syscall_return(-EFAULT);
        return;
    }
    if (guest_ts.tv_sec<0 || (unsigned long)guest_ts.tv_nsec >= 1000000000L) {
        t->syscall_return(-EINVAL);
        return;
    }
    host_ts_in.tv_sec = guest_ts.tv_sec;
    host_ts_in.tv_nsec = guest_ts.tv_nsec;

    int result = nanosleep(&host_ts_in, &host_ts_out);
    if (t->syscall_arg(1) && -1==result && EINTR==errno) {
        guest_ts.tv_sec = host_ts_out.tv_sec;
        guest_ts.tv_nsec = host_ts_out.tv_nsec;
        if (sizeof(guest_ts)!=t->get_process()->mem_write(&guest_ts, t->syscall_arg(1), sizeof guest_ts)) {
            t->syscall_return(-EFAULT);
            return;
        }
    }
    t->syscall_return(-1==result?-errno:result);
}

static void
syscall_nanosleep_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave("d-P", sizeof(timespec_32), print_timespec_32);
}

/*******************************************************************************************************************************/

static void
syscall_rt_sigreturn_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("rt_sigreturn", "");
}

static void
syscall_rt_sigreturn(RSIM_Thread *t, int callno)
{
    int status = t->sys_rt_sigreturn();
    if (status>=0) {
        t->tracing(TRACE_SYSCALL)->more(" = <does not return>\n");
        throw RSIM_SignalHandling::mk_kill(0, 0);
    }
    t->syscall_return(status); /* ERROR; specimen will likely segfault shortly! */
}

static void
syscall_rt_sigreturn_leave(RSIM_Thread *t, int callno)
{
    /* This should not be reached, but might be reached if the rt_sigreturn system call body was skipped over. */
    t->tracing(TRACE_SYSCALL)->more(" = <should not have returned>\n");
}

/*******************************************************************************************************************************/

static void
syscall_rt_sigaction_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("rt_sigaction", "fPpd", signal_names, sizeof(sigaction_32), print_sigaction_32);
}

static void
syscall_rt_sigaction(RSIM_Thread *t, int callno)
{
    int signum=t->syscall_arg(0);
    uint32_t action_va=t->syscall_arg(1), oldact_va=t->syscall_arg(2);
    size_t sigsetsize=t->syscall_arg(3);

    if (sigsetsize!=8 || signum<1 || signum>_NSIG) {
        t->syscall_return(-EINVAL);
        return;
    }

    sigaction_32 new_action, old_action;
    sigaction_32 *new_action_p=NULL, *old_action_p=NULL;
    if (action_va) {
        if (sizeof(new_action) != t->get_process()->mem_read(&new_action, action_va, sizeof new_action)) {
            t->syscall_return(-EFAULT);
            return;
        }
        new_action_p = &new_action;
    }
    if (oldact_va) {
        old_action_p = &old_action;
    }
                
    int status = t->get_process()->sys_sigaction(signum, new_action_p, old_action_p);

    if (status>=0 && oldact_va &&
        sizeof(old_action) != t->get_process()->mem_write(&old_action, oldact_va, sizeof old_action)) {
        t->syscall_return(-EFAULT);
        return;
    }

    t->syscall_return(status);
}

static void
syscall_rt_sigaction_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave("d--P", sizeof(sigaction_32), print_sigaction_32);
}

/*******************************************************************************************************************************/

static void
syscall_rt_sigprocmask_enter(RSIM_Thread *t, int callno)
{
    static const Translate flags[] = { TE(SIG_BLOCK), TE(SIG_UNBLOCK), TE(SIG_SETMASK), T_END };
    t->syscall_enter("rt_sigprocmask", "ePp", flags, sizeof(RSIM_SignalHandling::sigset_32), print_sigmask_32);
}

static void
syscall_rt_sigprocmask(RSIM_Thread *t, int callno)
{
    int how=t->syscall_arg(0);
    uint32_t in_va=t->syscall_arg(1), out_va=t->syscall_arg(2);
    size_t sigsetsize=t->syscall_arg(3);
    assert(sigsetsize==sizeof(RSIM_SignalHandling::sigset_32));

    RSIM_SignalHandling::sigset_32 in_set, out_set;
    RSIM_SignalHandling::sigset_32 *in_set_p  = in_va ? &in_set  : NULL;
    RSIM_SignalHandling::sigset_32 *out_set_p = out_va? &out_set : NULL;

    if (in_set_p && sizeof(in_set)!=t->get_process()->mem_read(in_set_p, in_va, sizeof in_set)) {
        t->syscall_return(-EFAULT);
        return;
    }

    int result = t->sys_sigprocmask(how, in_set_p, out_set_p);
    t->syscall_return(result);
    if (result<0)
        return;

    if (out_set_p && sizeof(out_set)!=t->get_process()->mem_write(out_set_p, out_va, sizeof out_set)) {
        t->syscall_return(-EFAULT);
        return;
    }
}

static void
syscall_rt_sigprocmask_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave("d--P", sizeof(RSIM_SignalHandling::sigset_32), print_sigmask_32);
}

/*******************************************************************************************************************************/

static void
syscall_rt_sigpending_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("rt_sigpending", "p");
}

static void
syscall_rt_sigpending(RSIM_Thread *t, int callno)
{
    uint32_t sigset_va=t->syscall_arg(0);
    RSIM_SignalHandling::sigset_32 pending;
    int result = t->sys_sigpending(&pending);
    t->syscall_return(result);
    if (result<0)
        return;

    if (sizeof(pending)!=t->get_process()->mem_write(&pending, sigset_va, sizeof pending)) {
        t->syscall_return(-EFAULT);
        return;
    }
}

static void
syscall_rt_sigpending_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave("dP", sizeof(RSIM_SignalHandling::sigset_32), print_sigmask_32);
}

/*******************************************************************************************************************************/

static void
syscall_rt_sigsuspend_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("rt_sigsuspend", "Pd", sizeof(RSIM_SignalHandling::sigset_32), print_sigmask_32);
}

static void
syscall_rt_sigsuspend(RSIM_Thread *t, int callno)
{
    assert(sizeof(RSIM_SignalHandling::sigset_32)==t->syscall_arg(1));
    RSIM_SignalHandling::sigset_32 new_mask;
    if (sizeof(new_mask)!=t->get_process()->mem_read(&new_mask, t->syscall_arg(0), sizeof new_mask)) {
        t->syscall_return(-EFAULT);
        return;
    }
    t->syscall_info.signo = t->sys_sigsuspend(&new_mask);
    t->syscall_return(-EINTR);
}

static void
syscall_rt_sigsuspend_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave("d");
    if (t->syscall_info.signo>0) {
        t->tracing(TRACE_SYSCALL)->multipart("", "    retured due to ");
        print_enum(t->tracing(TRACE_SYSCALL), signal_names, t->syscall_info.signo);
        t->tracing(TRACE_SYSCALL)->more("(%d)", t->syscall_info.signo);
        t->tracing(TRACE_SYSCALL)->multipart_end();
    }
}

/*******************************************************************************************************************************/

static void
syscall_getcwd_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("getcwd", "pd");
}

static void
syscall_getcwd(RSIM_Thread *t, int callno)
{
    static char buf[4096]; /* page size in kernel */
    int result = syscall(SYS_getcwd, buf, sizeof buf);
    if (-1==result) {
        t->syscall_return(-errno);
        return;
    }

    size_t len = strlen(buf) + 1;
    if (len > t->syscall_arg(1)) {
        t->syscall_return(-ERANGE);
        return;
    }

    if (len!=t->get_process()->mem_write(buf, t->syscall_arg(0), len)) {
        t->syscall_return(-EFAULT);
        return;
    }

    t->syscall_return(result);
}

static void
syscall_getcwd_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave("ds");
}

/*******************************************************************************************************************************/

static void
syscall_sigaltstack_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("sigaltstack", "Pp", sizeof(stack_32), print_stack_32);
}

static void
syscall_sigaltstack(RSIM_Thread *t, int callno)
{
    uint32_t in_va=t->syscall_arg(0), out_va=t->syscall_arg(1);

    stack_32 in_stack, out_stack;
    stack_32 *in_stack_p  = in_va  ? &in_stack  : NULL;
    stack_32 *out_stack_p = out_va ? &out_stack : NULL;

    if (in_stack_p && sizeof(in_stack)!=t->get_process()->mem_read(in_stack_p, in_va, sizeof in_stack)) {
        t->syscall_return(-EFAULT);
        return;
    }

    int result = t->sys_sigaltstack(in_stack_p, out_stack_p);
    t->syscall_return(result);
    if (result<0)
        return;

    if (out_stack_p && sizeof(out_stack)!=t->get_process()->mem_write(out_stack_p, out_va, sizeof out_stack)) {
        t->syscall_return(-EFAULT);
        return;
    }
}

static void
syscall_sigaltstack_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave("d-P", sizeof(stack_32), print_stack_32);
}
            
/*******************************************************************************************************************************/

static void
syscall_mmap2_enter(RSIM_Thread *t, int callno)
{
    static const Translate pflags[] = { TF(PROT_READ), TF(PROT_WRITE), TF(PROT_EXEC), TF(PROT_NONE), T_END };
    static const Translate mflags[] = { TF(MAP_SHARED), TF(MAP_PRIVATE), TF(MAP_ANONYMOUS), TF(MAP_DENYWRITE),
                                        TF(MAP_EXECUTABLE), TF(MAP_FILE), TF(MAP_FIXED), TF(MAP_GROWSDOWN),
                                        TF(MAP_LOCKED), TF(MAP_NONBLOCK), TF(MAP_NORESERVE),
#ifdef MAP_32BIT
                                        TF(MAP_32BIT),
#endif
                                        TF(MAP_POPULATE), T_END };
    t->syscall_enter("mmap2", "pdffdd", pflags, mflags);
}

static void
syscall_mmap2(RSIM_Thread *t, int callno)
{
    uint32_t start=t->syscall_arg(0), size=t->syscall_arg(1), prot=t->syscall_arg(2), flags=t->syscall_arg(3);
    uint32_t offset=t->syscall_arg(5)*PAGE_SIZE;
    int fd=t->syscall_arg(4);
    uint32_t result = t->get_process()->mem_map(start, size, prot, flags, offset, fd);
    t->syscall_return(result);
}

static void
syscall_mmap2_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave("p");
    t->get_process()->mem_showmap(t->tracing(TRACE_MMAP), "  memory map after mmap2 syscall:\n");
}

/*******************************************************************************************************************************/

/* Single function for syscalls 195 (stat64), 196 (lstat64), and 197 (fstat64) */
static void
syscall_stat64_enter(RSIM_Thread *t, int callno)
{
    if (195==callno || 196==callno) {
        t->syscall_enter(195==callno?"stat64":"lstat64", "sp");
    } else {
        t->syscall_enter("fstat64", "dp");
    }
}

static void
syscall_stat64(RSIM_Thread *t, int callno)
{
    /* We need to be a bit careful with xstat64 calls. The C library invokes one of the xstat64 system calls, which
     * writes a kernel data structure into a temporary buffer, and which the C library then massages into a struct
     * stat64. When simulating, we don't want the C library to monkey with the data returned from the system call
     * because the simulated C library will do the monkeying (it must only happen once).
     *
     * Therefore, we will invoke the system call directly, bypassing the C library, and then copy the result into
     * specimen memory. If the syscall is made on an amd64 host we need to convert it to an i386 host.
     *
     * For some unknown reason, if we invoke the system call with buf allocated on the stack we'll get -EFAULT (-14)
     * as the result; if we allocate it statically there's no problem.  Also, just in case the size is different than
     * we think, we'll allocate a guard area above the kernel_stat and check that the syscall didn't write into it. */
    ROSE_ASSERT(96==sizeof(kernel_stat_32));
    ROSE_ASSERT(144==sizeof(kernel_stat_64));
#ifdef SYS_stat64       /* x86sim must be running on i386 */
    ROSE_ASSERT(4==sizeof(long));
    int host_callno = 195==callno ? SYS_stat64 : (196==callno ? SYS_lstat64 : SYS_fstat64);
    static const size_t kernel_stat_size = sizeof(kernel_stat_32);
#else                   /* x86sim must be running on amd64 */
    ROSE_ASSERT(8==sizeof(long));
    int host_callno = 195==callno ? SYS_stat : (196==callno ? SYS_lstat : SYS_fstat);
    static const size_t kernel_stat_size = sizeof(kernel_stat_64);
#endif

    static uint8_t kernel_stat[kernel_stat_size+100];
    memset(kernel_stat, 0xff, sizeof kernel_stat);
    int result = 0xdeadbeef;

    /* Make the system call without going through the C library. Well, we go through syscall(), but nothing else. */
    if (195==callno || 196==callno) {
        bool error;
        std::string name = t->get_process()->read_string(t->syscall_arg(0), 0, &error);
        if (error) {
            t->syscall_return(-EFAULT);
            return;
        }
        result = syscall(host_callno, (unsigned long)name.c_str(), (unsigned long)kernel_stat);
    } else {
        result = syscall(host_callno, (unsigned long)t->syscall_arg(0), (unsigned long)kernel_stat);
    }
    if (-1==result) {
        t->syscall_return(-errno);
        return;
    }

    /* Check for overflow */
    for (size_t i=kernel_stat_size; i<sizeof kernel_stat; i++)
        ROSE_ASSERT(0xff==kernel_stat[i]);

    /* Check for underflow.  Check that the kernel initialized as much data as we thought it should.  We
     * initialized the kernel_stat to all 0xff bytes before making the system call.  The last data member of
     * kernel_stat is either an 8-byte inode (i386) or zero (amd64), which in either case the high order byte is
     * almost certainly not 0xff. */
    ROSE_ASSERT(0xff!=kernel_stat[kernel_stat_size-1]);

    /* On amd64 we need to translate the 64-bit struct that we got back from the host kernel to the 32-bit struct
     * that the specimen should get back from the guest kernel. */           
    if (sizeof(kernel_stat_64)==kernel_stat_size) {
        t->tracing(TRACE_SYSCALL)->brief("64-to-32");
        kernel_stat_64 *in = (kernel_stat_64*)kernel_stat;
        kernel_stat_32 out;
        out.dev = in->dev;
        out.pad_1 = (uint32_t)(-1);
        out.ino_lo = in->ino;
        out.mode = in->mode;
        out.nlink = in->nlink;
        out.user = in->user;
        out.group = in->group;
        out.rdev = in->rdev;
        out.pad_2 = (uint32_t)(-1);
        out.size = in->size;
        out.blksize = in->blksize;
        out.nblocks = in->nblocks;
        out.atim_sec = in->atim_sec;
        out.atim_nsec = in->atim_nsec;
        out.mtim_sec = in->mtim_sec;
        out.mtim_nsec = in->mtim_nsec;
        out.ctim_sec = in->ctim_sec;
        out.ctim_nsec = in->ctim_nsec;
        out.ino = in->ino;
        t->get_process()->mem_write(&out, t->syscall_arg(1), sizeof out);
    } else {
        t->get_process()->mem_write(kernel_stat, t->syscall_arg(1), kernel_stat_size);
    }

    t->syscall_return(result);
}

static void
syscall_stat64_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave("d-P", sizeof(kernel_stat_32), print_kernel_stat_32);
}

/*******************************************************************************************************************************/

static void
syscall_getuid32_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("getuid32", "");
}

static void
syscall_getuid32(RSIM_Thread *t, int callno)
{
    uid_t id = getuid();
    t->syscall_return(id);
}

/*******************************************************************************************************************************/

static void
syscall_getgid32_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("getgid32", "");
}

static void
syscall_getgid32(RSIM_Thread *t, int callno)
{
    uid_t id = getgid();
    t->syscall_return(id);
}

/*******************************************************************************************************************************/

static void
syscall_geteuid32_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("geteuid32", "");
}

static void
syscall_geteuid32(RSIM_Thread *t, int callno)
{
    uid_t id = geteuid();
    t->syscall_return(id);
}

/*******************************************************************************************************************************/

static void
syscall_getegid32_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("getegid32", "");
}

static void
syscall_getegid32(RSIM_Thread *t, int callno)
{
    uid_t id = getegid();
    t->syscall_return(id);
}

/*******************************************************************************************************************************/

static void
syscall_fchown32_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("fchown32", "ddd");
}

static void
syscall_fchown32(RSIM_Thread *t, int callno)
{
    int fd=t->syscall_arg(0), user=t->syscall_arg(1), group=t->syscall_arg(2);
    int result = syscall(SYS_fchown, fd, user, group);
    t->syscall_return(-1==result?-errno:result);
}

/*******************************************************************************************************************************/

static void
syscall_chown_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("chown", "sdd");
}

static void
syscall_chown(RSIM_Thread *t, int callno)
{
    bool error;
    std::string filename = t->get_process()->read_string(t->syscall_arg(0), 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }
    uid_t user = t->syscall_arg(1);
    gid_t group = t->syscall_arg(2);
    int result = chown(filename.c_str(),user,group);
    t->syscall_return(result);
}

/*******************************************************************************************************************************/

static void
syscall_getdents64_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("getdents64", "dpd");
}

static void
syscall_getdents64(RSIM_Thread *t, int callno)
{
    int fd = t->syscall_arg(0), sz = t->syscall_arg(2);
    uint32_t dirent_va = t->syscall_arg(1);
    int status = t->getdents_syscall<dirent64_t>(fd, dirent_va, sz);
    t->syscall_return(status);
}

static void
syscall_getdents64_leave(RSIM_Thread *t, int callno)
{
    int status = t->syscall_arg(-1);
    t->syscall_leave("d-P", status>0?status:0, print_dentries_64);
}

/*******************************************************************************************************************************/

static void
syscall_fcntl_enter(RSIM_Thread *t, int callno)
{
    static const Translate fcntl_cmds[] = { TE(F_DUPFD),
                                            TE(F_GETFD), TE(F_SETFD),
                                            TE(F_GETFL), TE(F_SETFL),
                                            TE(F_GETLK), TE(F_GETLK64),
                                            TE(F_SETLK), TE(F_SETLK64),
                                            TE(F_SETLKW), TE(F_SETLKW64),
                                            TE(F_SETOWN), TE(F_GETOWN),
                                            TE(F_SETSIG), TE(F_GETSIG),
                                            TE(F_SETLEASE), TE(F_GETLEASE),
                                            TE(F_NOTIFY),
#ifdef F_DUPFD_CLOEXEC
                                            TE(F_DUPFD_CLOEXEC),
#endif
                                            T_END};
    int cmd=t->syscall_arg(1);
    switch (cmd) {
        case F_DUPFD:
#ifdef F_DUPFD_CLOEXEC
        case F_DUPFD_CLOEXEC:
#endif
        case F_GETFD:
        case F_GETFL:
        case F_GETOWN:
        case F_GETSIG:
            t->syscall_enter("fcntl64", "df", fcntl_cmds);
            break;
        case F_SETFD:
        case F_SETOWN:
            t->syscall_enter("fcntl64", "dfd", fcntl_cmds);
            break;
        case F_SETFL:
            t->syscall_enter("fcntl64", "dff", fcntl_cmds, open_flags);
            break;
        case F_SETSIG:
            t->syscall_enter("fcntl64", "dff", fcntl_cmds, signal_names);
            break;
        case F_GETLK:
        case F_SETLK:
        case F_SETLKW:
            t->syscall_enter("fcntl64", "dfP", fcntl_cmds, sizeof(flock_32), print_flock_32);
            break;
        default:
            t->syscall_enter("fcntl64", "dfd", fcntl_cmds);
            break;
    }
}

static void
syscall_fcntl(RSIM_Thread *t, int callno)
{
    int fd=t->syscall_arg(0), cmd=t->syscall_arg(1), other=t->syscall_arg(2), result=-EINVAL;
    switch (cmd) {
        case F_DUPFD:
#ifdef F_DUPFD_CLOEXEC
        case F_DUPFD_CLOEXEC:
#endif
        case F_GETFD:
        case F_GETFL:
        case F_GETOWN:
        case F_GETSIG:
        case F_SETFD:
        case F_SETOWN:
        case F_SETFL:
        case F_SETSIG: {
            result = fcntl(fd, cmd, other);
            t->syscall_return(-1==result?-errno:result);
            break;
        }
        case F_GETLK:
        case F_SETLK:
        case F_SETLKW: {
            flock_32 guest_fl;
            static flock_native host_fl;
            if (sizeof(guest_fl)!=t->get_process()->mem_read(&guest_fl, t->syscall_arg(2), sizeof guest_fl)) {
                t->syscall_return(-EFAULT);
                break;
            }
            host_fl.l_type = guest_fl.l_type;
            host_fl.l_whence = guest_fl.l_whence;
            host_fl.l_start = guest_fl.l_start;
            host_fl.l_len = guest_fl.l_len;
            host_fl.l_pid = guest_fl.l_pid;
#ifdef SYS_fcntl64      /* host is 32-bit */
            result = syscall(SYS_fcntl64, fd, cmd, &host_fl);
#else                   /* host is 64-bit */
            result = syscall(SYS_fcntl, fd, cmd, &host_fl);
#endif
            if (-1==result) {
                t->syscall_return(-errno);
                break;
            }
            if (F_GETLK==cmd) {
                guest_fl.l_type = host_fl.l_type;
                guest_fl.l_whence = host_fl.l_whence;
                guest_fl.l_start = host_fl.l_start;
                guest_fl.l_len = host_fl.l_len;
                guest_fl.l_pid = host_fl.l_pid;
                if (sizeof(guest_fl)!=t->get_process()->mem_write(&guest_fl, t->syscall_arg(2), sizeof guest_fl)) {
                    t->syscall_return(-EFAULT);
                    break;
                }
            }

            t->syscall_return(result);
            break;
        }
        default:
            t->syscall_return(-EINVAL);
            break;
    }
}

static void
syscall_fcntl_leave(RSIM_Thread *t, int callno)
{
    int cmd=t->syscall_arg(1);
    switch (cmd) {
        case F_GETLK:
            t->syscall_leave("d--P", sizeof(flock_32), print_flock_32);
            break;
        default:
            t->syscall_leave("d");
            break;
    }
}
    
/*******************************************************************************************************************************/

static void
syscall_gettid_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("gettid", "");
}

static void
syscall_gettid(RSIM_Thread *t, int callno)
{
    t->syscall_return(t->get_tid());
}

/*******************************************************************************************************************************/

static void
syscall_futex_enter(RSIM_Thread *t, int callno)
{
    /* We cannot include <linux/futex.h> portably across a variety of Linux machines. */
    static const Translate opflags[] = {
        TF3(0xff, 0x80, FUTEX_PRIVATE_FLAG|FUTEX_WAIT),
        TF3(0x80, 0x80, FUTEX_PRIVATE_FLAG),
        TF3(0x7f, 0, FUTEX_WAIT),
        TF3(0x7f, 1, FUTEX_WAKE),
        TF3(0x7f, 2, FUTEX_FD),
        TF3(0x7f, 3, FUTEX_REQUEUE),
        TF3(0x7f, 4, FUTEX_CMP_REQUEUE),
        TF3(0x7f, 5, FUTEX_WAKE_OP),
        TF3(0x7f, 6, FUTEX_LOCK_PI),
        TF3(0x7f, 7, FUTEX_UNLOCK_PI),
        TF3(0x7f, 8, FUTEX_TRYLOCK_PI),
        TF3(0x7f, 9, FUTEX_WAIT_BITSET),
        TF3(0x7f, 10, FUTEX_WAKE_BITSET),
        T_END
    };

    uint32_t op = t->syscall_arg(1);
    switch (op & 0x7f) {
        case 0: /*FUTEX_WAIT*/
            t->syscall_enter("futex", "PfdP", 4, print_int_32, opflags, sizeof(timespec_32), print_timespec_32);
            break;
        case 1: /*FUTEX_WAKE*/
            t->syscall_enter("futex", "Pfd", 4, print_int_32, opflags);
            break;
        case 2: /*FUTEX_FD*/
            t->syscall_enter("futex", "Pfd", 4, print_int_32, opflags);
            break;
        case 3: /*FUTEX_REQUEUE*/
            t->syscall_enter("futex", "Pfd-P", 4, print_int_32, opflags, 4, print_int_32);
            break;
        case 4: /*FUTEX_CMP_REQUEUE*/
            t->syscall_enter("futex", "Pfd-Pd", 4, print_int_32, opflags, 4, print_int_32);
            break;
        default:
            t->syscall_enter("futex", "PfdPPd", 4, print_int_32, opflags, sizeof(timespec_32), print_timespec_32, 
                          4, print_int_32);
            break;
    }
}

static void
syscall_futex(RSIM_Thread *t, int callno)
{
    /* Variable arguments */
    uint32_t futex1_va = t->syscall_arg(0);
    uint32_t op = t->syscall_arg(1);
    uint32_t val1 = t->syscall_arg(2);
    uint32_t timeout_va = 0;                    /* arg 3 when present */
    uint32_t futex2_va = 0;                     /* arg 4 when present */
    uint32_t val3 = 0;                          /* arg 5 when present */
    switch (op & 0x7f) {
        case 0: /*FUTEX_WAIT*/
            timeout_va = t->syscall_arg(3);
            break;
        case 1: /*FUTEX_WAKE*/
        case 2: /*FUTEX_FD*/
            break;
        case 3: /*FUTEX_REQUEUE*/
            futex2_va = t->syscall_arg(4);
            break;
        case 4: /*FUTEX_CMP_REQUEUE*/
            futex2_va = t->syscall_arg(4);
            val3 = t->syscall_arg(5);
            break;
        default:
            timeout_va =t->syscall_arg(3);
            futex2_va = t->syscall_arg(4);
            val3 = t->syscall_arg(5);
            break;
    }

    assert(sizeof(int)==sizeof(uint32_t));
    int *futex1 = futex1_va ? (int*)t->get_process()->my_addr(futex1_va, 4) : NULL;
    int *futex2 = futex2_va ? (int*)t->get_process()->my_addr(futex2_va, 4) : NULL;
    struct timespec timespec_buf, *timespec=NULL;
    if (timeout_va) {
        timespec_32 ts;
        if (sizeof(ts) != t->get_process()->mem_read(&ts, timeout_va, sizeof ts)) {
            t->syscall_return(-EFAULT);
            return;
        }
        timespec_buf.tv_sec = ts.tv_sec;
        timespec_buf.tv_nsec = ts.tv_nsec;
        timespec = &timespec_buf;
    }

    int result = syscall(SYS_futex, futex1, op, val1, timespec, futex2, val3);
    t->syscall_return(-1==result ? -errno : result);
}

/*******************************************************************************************************************************/

static void
syscall_sched_getaffinity_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("sched_getaffinity", "ddp");
}

static void
syscall_sched_getaffinity(RSIM_Thread *t, int callno)
{
    pid_t pid = t->syscall_arg(0);
    
    size_t cpuset_nbits = t->syscall_arg(1);
    size_t cpuset_nbytes = (cpuset_nbits+7) / 8;
    uint8_t buf[cpuset_nbytes+sizeof(long)]; /* overallocated */

    int result = sched_getaffinity(pid, cpuset_nbits, (cpu_set_t*)buf);
    if (-1==result) {
        t->syscall_return(-errno);
        return;
    }

    rose_addr_t cpuset_va = t->syscall_arg(2);
    if (cpuset_nbytes!=t->get_process()->mem_write(buf, cpuset_va, cpuset_nbytes)) {
        t->syscall_return(-EFAULT);
        return;
    }

    t->syscall_return(result);
}

static void
syscall_sched_getaffinity_leave(RSIM_Thread *t, int callno)
{
    size_t cpuset_nbits = t->syscall_arg(1);
    size_t cpuset_nbytes = (cpuset_nbits+7) / 8;
    t->syscall_leave("d--P", cpuset_nbytes, print_bitvec);
}

/*******************************************************************************************************************************/

static void
syscall_set_thread_area_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("set_thread_area", "P", sizeof(user_desc_32), print_user_desc_32);
}

static void
syscall_set_thread_area(RSIM_Thread *t, int callno)
{
    user_desc_32 ud;
    if (sizeof(ud)!=t->get_process()->mem_read(&ud, t->syscall_arg(0), sizeof ud)) {
        t->syscall_return(-EFAULT);
        return;
    }
    int old_idx = ud.entry_number;
    int new_idx = t->set_thread_area(&ud, true);
    if (new_idx<0) {
        t->syscall_return(new_idx);
        return;
    }
    if (old_idx!=new_idx &&
        sizeof(ud)!=t->get_process()->mem_write(&ud, t->syscall_arg(0), sizeof ud)) {
        t->syscall_return(-EFAULT);
        return;
    }
    t->syscall_return(0);
}

/*******************************************************************************************************************************/

static void
syscall_exit_group_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("exit_group", "d");
}

static void
syscall_exit_group(RSIM_Thread *t, int callno)
{
    if (t->clear_child_tid) {
        /* From the set_tid_address(2) man page:
         *      When clear_child_tid is set, and the process exits, and the process was sharing memory with other
         *      processes or threads, then 0 is written at this address, and a futex(child_tidptr, FUTEX_WAKE, 1, NULL,
         *      NULL, 0) call is done. (That is, wake a single process waiting on this futex.) Errors are ignored. */
        uint32_t zero = 0;
        size_t n = t->get_process()->mem_write(&zero, t->clear_child_tid, sizeof zero);
        ROSE_ASSERT(n==sizeof zero);
        int nwoke = t->futex_wake(t->clear_child_tid);
        ROSE_ASSERT(nwoke>=0);
    }

    t->tracing(TRACE_SYSCALL)->more(" = <throwing Exit>\n");
    throw RSIM_Process::Exit(__W_EXITCODE(t->syscall_arg(0), 0), true); /* true=>exit entire process */
}

static void
syscall_exit_group_leave(RSIM_Thread *t, int callno)
{
    /* This should not be reached, but might be reached if the exit_group system call body was skipped over. */
    t->tracing(TRACE_SYSCALL)->more(" = <should not have returned>\n");
}

/*******************************************************************************************************************************/

static void
syscall_set_tid_address_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("set_tid_address", "p");
}

static void
syscall_set_tid_address(RSIM_Thread *t, int callno)
{
    t->clear_child_tid = t->syscall_arg(0);
    t->syscall_return(getpid());
}

/*******************************************************************************************************************************/

static void
syscall_clock_settime_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("clock_settime", "eP", clock_names, sizeof(timespec_32), print_timespec_32);
}

static void
syscall_clock_settime(RSIM_Thread *t, int callno)
{
    timespec_32 guest_ts;
    if (sizeof(guest_ts)!=t->get_process()->mem_read(&guest_ts, t->syscall_arg(1), sizeof guest_ts)) {
        t->syscall_return(-EFAULT);
        return;
    }

    static timespec host_ts;
    host_ts.tv_sec = guest_ts.tv_sec;
    host_ts.tv_nsec = guest_ts.tv_nsec;
    int result = syscall(SYS_clock_settime, t->syscall_arg(0), &host_ts);
    t->syscall_return(-1==result?-errno:result);
}

/*******************************************************************************************************************************/

static void
syscall_clock_gettime_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("clock_gettime", "ep", clock_names);
}

static void
syscall_clock_gettime(RSIM_Thread *t, int callno)
{
    static timespec host_ts;
    int result = syscall(SYS_clock_gettime, t->syscall_arg(0), &host_ts);
    if (-1==result) {
        t->syscall_return(-errno);
        return;
    }

    timespec_32 guest_ts;
    guest_ts.tv_sec = host_ts.tv_sec;
    guest_ts.tv_nsec = host_ts.tv_nsec;
    if (sizeof(guest_ts)!=t->get_process()->mem_write(&guest_ts, t->syscall_arg(1), sizeof guest_ts)) {
        t->syscall_return(-EFAULT);
        return;
    }

    t->syscall_return(result);
}

static void
syscall_clock_gettime_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave("d-P", sizeof(timespec_32), print_timespec_32);
}

/*******************************************************************************************************************************/

static void
syscall_clock_getres_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("clock_getres", "ep", clock_names);
}

static void
syscall_clock_getres(RSIM_Thread *t, int callno)
{
    static timespec host_ts;
    timespec *host_tsp = t->syscall_arg(1) ? &host_ts : NULL;
    int result = syscall(SYS_clock_getres, t->syscall_arg(0), host_tsp);
    if (-1==result) {
        t->syscall_return(-errno);
        return;
    }

    if (t->syscall_arg(1)) {
        timespec_32 guest_ts;
        guest_ts.tv_sec = host_ts.tv_sec;
        guest_ts.tv_nsec = host_ts.tv_nsec;
        if (sizeof(guest_ts)!=t->get_process()->mem_write(&guest_ts, t->syscall_arg(1), sizeof guest_ts)) {
            t->syscall_return(-EFAULT);
            return;
        }
    }

    t->syscall_return(result);
}

static void
syscall_clock_getres_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave("d-P", sizeof(timespec_32), print_timespec_32);
}

/*******************************************************************************************************************************/

static void
syscall_statfs64_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("statfs64", "sdp");
}

static void
syscall_statfs64(RSIM_Thread *t, int callno)
{
    ROSE_ASSERT(t->syscall_arg(1)==sizeof(statfs64_32));
    bool error;
    std::string path = t->get_process()->read_string(t->syscall_arg(0), 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }

    statfs64_32 guest_statfs;
#ifdef SYS_statfs64 /* host is 32-bit machine */
    static statfs64_native host_statfs;
    int result = syscall(SYS_statfs64, path.c_str(), sizeof host_statfs, &host_statfs);
    convert(&guest_statfs, &host_statfs);
#else           /* host is 64-bit machine */
    static statfs_native host_statfs;
    int result = syscall(SYS_statfs, path.c_str(), &host_statfs);
    convert(&guest_statfs, &host_statfs);
#endif
    if (-1==result) {
        t->syscall_return(-errno);
        return;
    }
    if (sizeof(guest_statfs)!=t->get_process()->mem_write(&guest_statfs, t->syscall_arg(2), sizeof guest_statfs)) {
        t->syscall_return(-EFAULT);
        return;
    }

    t->syscall_return(result);
}

static void
syscall_statfs64_leave(RSIM_Thread *t, int callno)
{
    t->syscall_leave("d--P", sizeof(statfs64_32), print_statfs64_32);
}

/*******************************************************************************************************************************/

static void
syscall_tgkill_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("tgkill", "ddf", signal_names);
}

static void
syscall_tgkill(RSIM_Thread *t, int callno)
{
    int tgid=t->syscall_arg(0), tid=t->syscall_arg(1), sig=t->syscall_arg(2);
    int result = t->sys_tgkill(tgid, tid, sig);
    t->syscall_return(result);
}

/*******************************************************************************************************************************/

static void
syscall_utimes_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("utimes", "s");
}

static void
syscall_utimes(RSIM_Thread *t, int callno)
{
    /*
                int utimes(const char *filename, const struct timeval times[2]);

                struct timeval {
                    long tv_sec;        // seconds 
                    long tv_usec;   // microseconds 
                };


                The utimes() system call changes the access and modification times of the inode
                specified by filename to the actime and modtime fields of times respectively.

                times[0] specifies the new access time, and times[1] specifies the new
                modification time.  If times is NULL, then analogously to utime(), the access
                and modification times of the file are set to the current time.

    */
    bool error;
    std::string filename = t->get_process()->read_string(t->syscall_arg(0), 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }

    //Check to see if times is NULL
    uint8_t byte;
    size_t nread = t->get_process()->mem_read(&byte, t->syscall_arg(1), 1);
    ROSE_ASSERT(1==nread); /*or we've read past the end of the mapped memory*/

    int result;
    if( byte ) {

        size_t size_timeval_sample = sizeof(timeval_32)*2;

        timeval_32 ubuf[1];

        size_t nread = t->get_process()->mem_read(&ubuf, t->syscall_arg(1), size_timeval_sample);

        timeval timeval64[1];
        timeval64[0].tv_sec  = ubuf[0].tv_sec;
        timeval64[0].tv_usec = ubuf[0].tv_usec;
        timeval64[1].tv_sec  = ubuf[1].tv_sec;
        timeval64[1].tv_usec = ubuf[1].tv_usec;

        ROSE_ASSERT(nread == size_timeval_sample);

        result = utimes(filename.c_str(), timeval64);

    } else {
        result = utimes(filename.c_str(), NULL);
    }

    t->syscall_return(result);
}

/*******************************************************************************************************************************/

static void
syscall_fchmodat_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("fchmodat", "dsdd");
}

static void
syscall_fchmodat(RSIM_Thread *t, int callno)
{
    int dirfd = t->syscall_arg(0);
    uint32_t path = t->syscall_arg(1);
    bool error;
    std::string sys_path = t->get_process()->read_string(path, 0, &error);
    if (error) {
        t->syscall_return(-EFAULT);
        return;
    }
    mode_t mode = t->syscall_arg(2);
    int flags = t->syscall_arg(3);

    int result = syscall( 306, dirfd, (long) sys_path.c_str(), mode, flags);
    if (result == -1) result = -errno;
    t->syscall_return(result);
}

/*******************************************************************************************************************************/

static void
syscall_set_robust_list_enter(RSIM_Thread *t, int callno)
{
    t->syscall_enter("set_robust_list", "Pd", sizeof(robust_list_head_32), print_robust_list_head_32);
}

static void
syscall_set_robust_list(RSIM_Thread *t, int callno)
{
    uint32_t head_va=t->syscall_arg(0), len=t->syscall_arg(1);
    if (len!=sizeof(robust_list_head_32)) {
        t->syscall_return(-EINVAL);
        return;
    }

    robust_list_head_32 guest_head;
    if (sizeof(guest_head)!=t->get_process()->mem_read(&guest_head, head_va, sizeof(guest_head))) {
        t->syscall_return(-EFAULT);
        return;
    }

    /* The robust list is maintained in user space and accessed by the kernel only when we a thread dies. Since the
     * simulator handles thread death, we don't need to tell the kernel about the specimen's list until later. In
     * fact, we can't tell the kernel because that would cause our own list (set by libc) to be removed from the
     * kernel. */
    t->robust_list_head_va = head_va;
    t->syscall_return(0);
}

/*******************************************************************************************************************************/


#endif /* ROSE_ENABLE_SIMULATOR */
