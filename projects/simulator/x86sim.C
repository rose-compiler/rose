/* Emulates an executable. */
#include "rose.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
#include "rose_config.h"

/* Define one CPP symbol to determine whether this simulator can be compiled.  The definition of this one symbol depends on
 * all the header file prerequisites. */
#if defined(HAVE_ASM_LDT_H) && defined(HAVE_ELF_H) &&                                                                          \
    defined(HAVE_LINUX_TYPES_H) && defined(HAVE_LINUX_DIRENT_H) && defined(HAVE_LINUX_UNISTD_H)
#  define ROSE_ENABLE_SIMULATOR
#else
#  undef ROSE_ENABLE_SIMULATOR
#endif

#ifdef ROSE_ENABLE_SIMULATOR /* protects this whole file */


#include "RSIM_Simulator.h"
#include "x86print.h"
#include <stdarg.h>

/* These are necessary for the system call emulation */
#include <errno.h>
#include <syscall.h>
#include <sys/mman.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>
#include <utime.h>


/* AS extra required headers for system call simulation */
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
#include <sys/mman.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <linux/unistd.h>
#include <sys/sysinfo.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <grp.h>


#define CONV_FIELD(var1, var2, field) var1.field = var2.field

void
RSIM_Thread::emulate_syscall()
{
    /* Warning: use hard-coded values here rather than the __NR_* constants from <sys/unistd.h> because the latter varies
     *          according to whether ROSE is compiled for 32- or 64-bit.  We always want the 32-bit syscall numbers. */
    unsigned callno = policy.readGPR(x86_gpr_ax).known_value();




    switch (callno) {
        //case 1: /* exit: see syscall 252, exit_group */

        case 3: { /*read*/
            syscall_enter("read", "dpd");
            int fd=arg(0);
            uint32_t buf_va=arg(1), size=arg(2);
            char buf[size];
            ssize_t nread = read(fd, buf, size);
            if (-1==nread) {
                sys_return(-errno);
            } else if (process->get_memory()->write(buf, buf_va, (size_t)nread)!=(size_t)nread) {
                sys_return(-EFAULT);
            } else {
                sys_return(nread);
            }
            syscall_leave("d-b", nread>0?nread:0);
            break;
        }

        case 4: { /*write*/
            syscall_enter("write", "dbd", arg(2));
            int fd=arg(0);
            uint32_t buf_va=arg(1);
            size_t size=arg(2);
            uint8_t buf[size];
            size_t nread = process->get_memory()->read(buf, buf_va, size);
            if (nread!=size) {
                sys_return(-EFAULT);
            } else {
                ssize_t nwritten = write(fd, buf, size);
                if (-1==nwritten) {
                    sys_return(-errno);
                } else {
                    sys_return(nwritten);
                }
            }
            syscall_leave("d");
            break;
        }

        case 5: { /*open*/
            syscall_enter("open", "sf", open_flags);
            do {
                uint32_t filename_va=arg(0);
                bool error;
                std::string filename = read_string(filename_va, 0, &error);
                if (error) {
                    sys_return(-EFAULT);
                    break;
                }
                uint32_t flags=arg(1), mode=(flags & O_CREAT)?arg(2):0;
                int fd = open(filename.c_str(), flags, mode);
                if (-1==fd) {
                    sys_return(-errno);
                    break;
                }

                sys_return(fd);
            } while (0);
            syscall_leave("d");
            break;
        }

        case 6: { /*close*/
            syscall_enter("close", "d");
            int fd=arg(0);
            if (1==fd || 2==fd) {
                /* ROSE is using these */
                sys_return(-EPERM);
            } else {
                int status = close(fd);
                sys_return(status<0 ? -errno : status);
            }
            syscall_leave("d");
            break;
        }

        case 7: { // waitpid
            static const Translate wflags[] = { TF(WNOHANG), TF(WUNTRACED), T_END };
            syscall_enter("waitpid", "dpf", wflags);
            pid_t pid=arg(0);
            uint32_t status_va=arg(1);
            int options=arg(2);
            int sys_status;
            int result = waitpid(pid, &sys_status, options);
            if (result == -1) {
                result = -errno;
            } else if (status_va) {
                uint32_t status_le;
                SgAsmExecutableFileFormat::host_to_le(sys_status, &status_le);
                size_t nwritten = process->get_memory()->write(&status_le, status_va, 4);
                ROSE_ASSERT(4==nwritten);
            }
            sys_return(result);
            syscall_leave("d");
            break;
        }

        case 8: { /* 0x8, creat */
            syscall_enter("creat", "sd");
            do {
                uint32_t filename = arg(0);
                bool error;
                std::string sys_filename = read_string(filename, 0, &error);
                if (error) {
                    sys_return(-EFAULT);
                    break;
                }
	        mode_t mode = arg(1);

	        int result = creat(sys_filename.c_str(), mode);
                if (result == -1) {
                    sys_return(-errno);
                    break;
                }

                sys_return(result);
            } while (0);
            syscall_leave("d");
            break;
        }

        case 9: { /* 0x9, link */
            syscall_enter("link", "ss");
            bool error;

            std::string oldpath = read_string(arg(0), 0, &error);
	    if (error) {
		    sys_return(-EFAULT);
		    break;
	    }
            std::string newpath = read_string(arg(1), 0, &error);
	    if (error) {
		    sys_return(-EFAULT);
		    break;
	    }

            int result = syscall(SYS_link,oldpath.c_str(), newpath.c_str());
            sys_return(-1==result?-errno:result);

            syscall_leave("d");
            break;
        }

        case 10: { /*0xa, unlink*/
            syscall_enter("unlink", "s");
            do {
                uint32_t filename_va = arg(0);
                bool error;
                std::string filename = read_string(filename_va, 0, &error);
                if (error) {
                    sys_return(-EFAULT);
                    break;
                }

                int result = unlink(filename.c_str());
                if (result == -1) {
                    sys_return(-errno);
                    break;
                }
                
                sys_return(result);
            } while (0);
            syscall_leave("d");
            break;
        }

	case 11: { /* 0xb, execve */
            syscall_enter("execve", "spp");
            do {
                bool error;

                /* Name of executable */
                std::string filename = read_string(arg(0), 0, &error);
                if (error) {
                    sys_return(-EFAULT);
                    break;
                }

                /* Argument vector */
                std::vector<std::string> argv = read_string_vector(arg(1), &error);
                if (tracing(TRACE_SYSCALL)) {
                    if (!argv.empty())
                        fputs("continued below...\n", tracing(TRACE_SYSCALL));
                    for (size_t i=0; i<argv.size(); i++) {
                        fprintf(tracing(TRACE_SYSCALL), "    argv[%zu] = ", i);
                        print_string(tracing(TRACE_SYSCALL), argv[i], false, false);
                        fputc('\n', tracing(TRACE_SYSCALL));
                    }
                }
                if (error) {
                    sys_return(-EFAULT);
                    break;
                }
                std::vector<char*> sys_argv;
                for (size_t i = 0; i < argv.size(); ++i)
                    sys_argv.push_back(&argv[i][0]);
                sys_argv.push_back(NULL);

                /* Environment vector */
                std::vector<std::string> envp = read_string_vector(arg(2), &error);
                if (tracing(TRACE_SYSCALL)) {
                    if (argv.empty() && !envp.empty())
                        fputs("continued below...\n", tracing(TRACE_SYSCALL));
                    for (size_t i=0; i<envp.size(); i++) {
                        fprintf(tracing(TRACE_SYSCALL), "    envp[%zu] = ", i);
                        print_string(tracing(TRACE_SYSCALL), envp[i], false, false);
                        fputc('\n', tracing(TRACE_SYSCALL));
                    }
                }
                if (error) {
                    sys_return(-EFAULT);
                    break;
                }
                std::vector<char*> sys_envp;
                for (unsigned int i = 0; i < envp.size(); ++i)
                    sys_envp.push_back(&envp[i][0]);
                sys_envp.push_back(NULL);

                /* The real system call */
                int result = execve(&filename[0], &sys_argv[0], &sys_envp[0]);
                ROSE_ASSERT(-1==result);
                sys_return(-errno);
                if (tracing(TRACE_SYSCALL) && (!argv.empty() || !envp.empty()))
                    fputs("execve failed with ", tracing(TRACE_SYSCALL));
            } while (0);
            syscall_leave("d");
            break;
        }

	case 12: { /* 0xc, chdir */
            syscall_enter("chdir", "s");
            do {
                uint32_t path = arg(0);
                bool error;
                std::string sys_path = read_string(path, 0, &error);
                if (error) {
                    sys_return(-EFAULT);
                    break;
                }

                int result = chdir(sys_path.c_str());
                if (result == -1) {
                    sys_return(-errno);
                    break;
                }

                sys_return(result);
            } while (0);
            syscall_leave("d");
            break;
	}

        case 13: { /*0xd, time */
            syscall_enter("time", "p");
            time_t result = time(NULL);
            if (arg(0)) {
                uint32_t t_le;
                SgAsmExecutableFileFormat::host_to_le(result, &t_le);
                size_t nwritten = process->get_memory()->write(&t_le, arg(0), 4);
                ROSE_ASSERT(4==nwritten);
            }
            sys_return(result);
            syscall_leave("t");
            break;
        }

        case 14: { /*0xe, mknod*/
            syscall_enter("mknod", "sfd", file_mode_flags);
            do {
                uint32_t path_va = arg(0);
                int mode = arg(1);
                unsigned dev = arg(2);
                bool error;
                std::string path = read_string(path_va, 0, &error);
                if (error) {
                    sys_return(-EFAULT);
                    break;
                }
                int result = mknod(path.c_str(), mode, dev);
                sys_return(-1==result ? -errno : result);
            } while (0);
            syscall_leave("d");
            break;
        }

	case 15: { /* 0xf, chmod */
            syscall_enter("chmod", "sd");
            do {
                uint32_t filename = arg(0);
                bool error;
                std::string sys_filename = read_string(filename, 0, &error);
                if (error) {
                    sys_return(-EFAULT);
                    break;
                }
                mode_t mode = arg(1);
                int result = chmod(sys_filename.c_str(), mode);
                if (result == -1) result = -errno;
                sys_return(result);
            } while (0);
            syscall_leave("d");
            break;
	}

        case 19: { /* 0x13, lseek(int fd, off_t offset, int whence) */
            syscall_enter("lseek", "ddf", seek_whence);
            off_t result = lseek(arg(0), arg(1), arg(2));
            sys_return(-1==result?-errno:result);
            syscall_leave("d");
            break;
        }

        case 20: { /*0x14, getpid*/
            syscall_enter("getpid", "");
            sys_return(getpid());
            syscall_leave("d");
            break;
        }

        case 24: { /*0x18, getuid*/
            syscall_enter("getuid", "");
            sys_return(getuid());
            syscall_leave("d");
            break;
        }

        case 27: { /* 0x1b, alarm */
            syscall_enter("alarm", "d");
            int result = alarm(arg(0));
            sys_return(result);
            syscall_leave("d");
            break;
        }

        case 29: { /* 0x1d, pause */
            syscall_enter("pause", "");
            signal_pause();
            sys_return(-EINTR);
            syscall_leave("d");
            break;
        }
            
        case 30: { /* 0x1e, utime */

            /*
               int utime(const char *filename, const struct utimbuf *times);

               The utimbuf structure is:

               struct utimbuf {
               time_t actime;       // access time 
                   time_t modtime;  // modification time 
                 };

               The utime() system call changes the access and modification times of the inode
               specified by filename to the actime and modtime fields of times respectively.

               If times is NULL, then the access and modification times of the file are set
               to the current time.
            */
            syscall_enter("utime", "sp");
            do {
                bool error;
                std::string filename = read_string(arg(0), 0, &error);
                if (error) {
                    sys_return(-EFAULT);
                    break;
                }

                //Check to see if times is NULL
                uint8_t byte;
                size_t nread = process->get_memory()->read(&byte, arg(1), 1);
                ROSE_ASSERT(1==nread); /*or we've read past the end of the mapped memory*/

                int result;
                if( byte) {
                    struct kernel_utimebuf {
                        uint32_t actime;
                        uint32_t modtime;
                    };

                    kernel_utimebuf ubuf;
                    size_t nread = process->get_memory()->read(&ubuf, arg(1), sizeof(kernel_utimebuf));
                    ROSE_ASSERT(nread == sizeof(kernel_utimebuf));

                    utimbuf ubuf64;
                    ubuf64.actime  = ubuf.actime;
                    ubuf64.modtime = ubuf.modtime;

                    result = utime(filename.c_str(), &ubuf64);

                } else {
                    result = utime(filename.c_str(), NULL);
                }
                sys_return(result);
            } while (0);
            syscall_leave("d");
            break;
        };

        case 33: { /*0x21, access*/
            static const Translate flags[] = { TF(R_OK), TF(W_OK), TF(X_OK), TF(F_OK), T_END };
            syscall_enter("access", "sf", flags);
            do {
                uint32_t name_va=arg(0);
                bool error;
                std::string name = read_string(name_va, 0, &error);
                if (error) {
                    sys_return(-EFAULT);
                    break;
                }
                int mode=arg(1);
                int result = access(name.c_str(), mode);
                if (-1==result) result = -errno;
                sys_return(result);
            } while (0);
            syscall_leave("d");
            break;
        }
        case 36: { /*0x24, sync*/
            //  void sync(void);
            syscall_enter("sync", "");
            sync();
            sys_return(0);
            syscall_leave("d");
            break;
        }

	case 37: { /* 0x25, kill */
            syscall_enter("kill", "df", signal_names);
            pid_t pid=arg(0);
            int sig=arg(1);
            int result = kill(pid, sig);
            if (result == -1) result = -errno;
            sys_return(result);
            syscall_leave("d");
            break;
        }

	case 38: { /* 0x26, rename */
            syscall_enter("rename", "ss");
            bool error;

            std::string oldpath = read_string(arg(0), 0, &error);
	    if (error) {
		    sys_return(-EFAULT);
		    break;
	    }
            std::string newpath = read_string(arg(1), 0, &error);
	    if (error) {
		    sys_return(-EFAULT);
		    break;
	    }

            int result = syscall(SYS_rename,oldpath.c_str(), newpath.c_str());
            sys_return(-1==result?-errno:result);

            syscall_leave("d");
            break;
        }


	case 39: { /* 0x27, mkdir */
            syscall_enter("mkdir", "sd");
            do {
                uint32_t pathname = arg(0);
                bool error;
                std::string sys_pathname = read_string(pathname, 0, &error);
                if (error) {
                    sys_return(-EFAULT);
                    break;
                }
                mode_t mode = arg(1);

                int result = mkdir(sys_pathname.c_str(), mode);
                if (result == -1) result = -errno;
                sys_return(result);
            } while (0);
            syscall_leave("d");
            break;
	}

	case 40: { /* 0x28, rmdir */
            syscall_enter("rmdir", "s");
            do {
                uint32_t pathname = arg(0);
                bool error;
                std::string sys_pathname = read_string(pathname, 0, &error);
                if (error) {
                    sys_return(-EFAULT);
                    break;
                }

                int result = rmdir(sys_pathname.c_str());
                if (result == -1) result = -errno;
                sys_return(result);
            } while (0);
            syscall_leave("d");
            break;
	}

        case 41: { /*0x29, dup*/
            syscall_enter("dup", "d");
            uint32_t fd = arg(0);
            int result = dup(fd);
            if (-1==result) result = -errno;
            sys_return(result);
            syscall_leave("d");
            break;
        }

        case 42: { /*0x2a, pipe*/
            /*
               int pipe(int filedes[2]); 

               pipe() creates a pair of file descriptors, pointing to a pipe inode, and 
               places them in the array pointed to by filedes. filedes[0] is for reading, 
               filedes[1] is for writing. 

            */
            syscall_enter("pipe", "p");


            int32_t filedes_kernel[2];
            size_t  size_filedes = sizeof(int32_t)*2;


            int filedes[2];
            int result = pipe(filedes);

            filedes_kernel[0] = filedes[0];
            filedes_kernel[1] = filedes[1];

            process->get_memory()->write(filedes_kernel, arg(0), size_filedes);


            if (-1==result) result = -errno;
            sys_return(result);
            syscall_leave("d");
            break;
        }

        case 45: { /*0x2d, brk*/
            syscall_enter("brk", "x");
            uint32_t newbrk = arg(0);
            int retval = 0;

            if (newbrk >= 0xb0000000ul) {
                retval = -ENOMEM;
            } else {
                if (newbrk > process->brk_va) {
                    MemoryMap::MapElement melmt(process->brk_va, newbrk-process->brk_va,
                                                MemoryMap::MM_PROT_READ|MemoryMap::MM_PROT_WRITE);
                    melmt.set_name("[heap]");
                    process->get_memory()->insert(melmt);
                    process->brk_va = newbrk;
                } else if (newbrk>0 && newbrk<process->brk_va) {
                    process->get_memory()->erase(MemoryMap::MapElement(newbrk, process->brk_va-newbrk));
                    process->brk_va = newbrk;
                }
                retval= process->brk_va;
            }
            if (newbrk!=0 && tracing(TRACE_MMAP)) {
                fprintf(tracing(TRACE_MMAP), "  memory map after brk syscall:\n");
                process->get_memory()->dump(tracing(TRACE_MMAP), "    ");
            }

            sys_return(retval);
            syscall_leave("p");
            break;
        }

        case 47: { /*0x2f, getgid*/
            syscall_enter("getgid", "");
            sys_return(getgid());
            syscall_leave("d");
            break;
        }

        case 49: { /*0x31, geteuid*/
            syscall_enter("geteuid", "");
            sys_return(geteuid());
            syscall_leave("d");
            break;
        }

        case 50: { /*0x32, getegid*/
            syscall_enter("getegid", "");
            sys_return(getegid());
            syscall_leave("d");
            break;
        }

        case 54: { /*0x36, ioctl*/
            int fd=arg(0);
            uint32_t cmd=arg(1);

            switch (cmd) {
                case TCGETS: { /* 0x00005401, tcgetattr*/
                    syscall_enter("ioctl", "dfp", ioctl_commands);
                    do {
                        termios_native host_ti;
                        int result = syscall(SYS_ioctl, fd, cmd, &host_ti);
                        if (-1==result) {
                            sys_return(-errno);
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
                        if (sizeof(guest_ti)!=process->get_memory()->write(&guest_ti, arg(2), sizeof guest_ti)) {
                            sys_return(-EFAULT);
                            break;
                        }
                        sys_return(result);
                    } while (0);
                    syscall_leave("d--P", sizeof(termios_32), print_termios_32);
                    break;
                }

                case TCSETSW:   /* 0x00005403 */
                case TCSETSF:
                case TCSETS: {  /* 0x00005402 */
                    syscall_enter("ioctl", "dfP", ioctl_commands, sizeof(termios_32), print_termios_32);
                    do {
                        termios_32 guest_ti;
                        if (sizeof(guest_ti)!=process->get_memory()->read(&guest_ti, arg(2), sizeof guest_ti)) {
                            sys_return(-EFAULT);
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
                        sys_return(-1==result?-errno:result);
                    } while (0);
                    syscall_leave("d");
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

                    syscall_enter("ioctl", "dfd", ioctl_commands);
                    termio to;

                    int result = ioctl(fd, TCGETA, &to);
                    if (-1==result) {
                        result = -errno;
                    } else {
                        size_t nwritten = process->get_memory()->write(&to, arg(2), sizeof to);
                        ROSE_ASSERT(nwritten==sizeof to);
                    }

                    sys_return(result);
                    syscall_leave("d");
                    break;
                }

                case TIOCGPGRP: { /* 0x0000540F, tcgetpgrp*/
                    /* equivalent to 
                        pid_t tcgetpgrp(int fd);
                       The  function tcgetpgrp() returns the process group ID of the foreground process group 
                       on the terminal associated to fd, which must be the controlling terminal of the calling 
                       process.
                    */

                    syscall_enter("ioctl", "dfd", ioctl_commands);
                    do {
                        pid_t pgrp = tcgetpgrp(fd);
                        if (-1==pgrp) {
                            sys_return(-errno);
                            break;
                        }
                        uint32_t pgrp_le;
                        SgAsmExecutableFileFormat::host_to_le(pgrp, &pgrp_le);
                        size_t nwritten = process->get_memory()->write(&pgrp_le, arg(2), 4);
                        ROSE_ASSERT(4==nwritten);
                        sys_return(pgrp);
                    } while (0);
                    syscall_leave("d");
                    break;
                }
                    
                case TIOCSPGRP: { /* 0x5410, tcsetpgrp*/
                    syscall_enter("ioctl", "dfd", ioctl_commands);
                    uint32_t pgid_le;
                    size_t nread = process->get_memory()->read(&pgid_le, arg(2), 4);
                    ROSE_ASSERT(4==nread);
                    pid_t pgid = SgAsmExecutableFileFormat::le_to_host(pgid_le);
                    int result = tcsetpgrp(fd, pgid);
                    if (-1==result)
                        result = -errno;
                    sys_return(result);
                    syscall_leave("d");
                    break;
                }

                case TIOCSWINSZ: { /* 0x5413, the winsize is const */
                    syscall_enter("ioctl", "dfP", ioctl_commands, sizeof(winsize_32), print_winsize_32);
                    do {
                        winsize_32 guest_ws;
                        if (sizeof(guest_ws)!=process->get_memory()->read(&guest_ws, arg(2), sizeof guest_ws)) {
                            sys_return(-EFAULT);
                            break;
                        }

                        winsize_native host_ws;
                        host_ws.ws_row = guest_ws.ws_row;
                        host_ws.ws_col = guest_ws.ws_col;
                        host_ws.ws_xpixel = guest_ws.ws_xpixel;
                        host_ws.ws_ypixel = guest_ws.ws_ypixel;

                        int result = syscall(SYS_ioctl, fd, cmd, &host_ws);
                        sys_return(-1==result?-errno:result);
                    } while (0);
                    syscall_leave("d");
                    break;
                }

                case TIOCGWINSZ: /* 0x5414, */ {
                    syscall_enter("ioctl", "dfp", ioctl_commands);
                    do {
                        winsize_native host_ws;
                        int result = syscall(SYS_ioctl, fd, cmd, &host_ws);
                        if (-1==result) {
                            sys_return(-errno);
                            break;
                        }

                        winsize_32 guest_ws;
                        guest_ws.ws_row = host_ws.ws_row;
                        guest_ws.ws_col = host_ws.ws_col;
                        guest_ws.ws_xpixel = host_ws.ws_xpixel;
                        guest_ws.ws_ypixel = host_ws.ws_ypixel;
                        if (sizeof(guest_ws)!=process->get_memory()->write(&guest_ws, arg(2), sizeof guest_ws)) {
                            sys_return(-EFAULT);
                            break;
                        }

                        sys_return(result);
                    } while (0);
                    syscall_leave("d--P", sizeof(winsize_32), print_winsize_32);
                    break;
                }

                default: {
                    syscall_enter("ioctl", "dfd", ioctl_commands);
                    fprintf(stderr, "  unhandled ioctl: %u\n", cmd);
                    abort();
                }
            }
            break;
        }

        case 57: { /*0x39, setpgid*/
            syscall_enter("setpgid", "dd");
            pid_t pid=arg(0), pgid=arg(1);
            int result = setpgid(pid, pgid);
            if (-1==result) { result = -errno; }
            sys_return(result);
            syscall_leave("d");
            break;
        }

        case 60: { /* 0x3C, umask */
            /* mode_t umask(mode_t mask);

               umask() sets the calling process' file mode creation mask (umask) to mask & 0777.
 
               This system call always succeeds and the previous value of the mask is returned.
            */
            syscall_enter("umask", "d");
	    mode_t mode = arg(0);

	    int result = syscall(SYS_umask, mode); 
            if (result == -1) result = -errno;
            sys_return(result);

            syscall_leave("d");
            break;
	    }  

        case 63: { /* 0x3f, dup2 */
            syscall_enter("dup2", "dd");
            int result = dup2(arg(0), arg(1));
            sys_return(-1==result?-errno:result);
            syscall_leave("d");
            break;
        }
            
        case 64: { /*0x40, getppid*/
            syscall_enter("getppid", "");
            sys_return(getppid());
            syscall_leave("d");
            break;
        }

        case 65: { /*0x41, getpgrp*/
            syscall_enter("getpgrp", "");
            sys_return(getpgrp());
            syscall_leave("d");
            break;
        }

        case 75: { /*0x4B, setrlimit */
            syscall_enter("setrlimit", "fP", rlimit_resources, 8, print_rlimit);
            int resource = arg(0);
            uint32_t rlimit_va = arg(1);
            uint32_t rlimit_guest[2];
            size_t nread = process->get_memory()->read(rlimit_guest, rlimit_va, sizeof rlimit_guest);
            ROSE_ASSERT(nread==sizeof rlimit_guest);
            struct rlimit rlimit_native;
            rlimit_native.rlim_cur = rlimit_guest[0];
            rlimit_native.rlim_max = rlimit_guest[1];
            int result = setrlimit(resource, &rlimit_native);
            sys_return(-1==result ? -errno : result);
            syscall_leave("d");
            break;
        }

        case 191:
            syscall_enter("ugetrlimit", "fp", rlimit_resources);
            if (tracing(TRACE_SYSCALL))
                fputs("delegated to getrlimit (syscall 76); see next line\n", tracing(TRACE_SYSCALL));
            /* fall through to 76; note that syscall trace will still show syscall 191 */
            
        case 76: {  /*0x4c, getrlimit*/
            syscall_enter("getrlimit", "fp", rlimit_resources);
            do {
                int resource = arg(0);
                uint32_t rlimit_va = arg(1);
                struct rlimit rlimit_native;
                int result = getrlimit(resource, &rlimit_native);
                if (-1==result) {
                    sys_return(-errno);
                    break;
                }

                uint32_t rlimit_guest[2];
                rlimit_guest[0] = rlimit_native.rlim_cur;
                rlimit_guest[1] = rlimit_native.rlim_max;
                if (8!=process->get_memory()->write(rlimit_guest, rlimit_va, 8)) {
                    sys_return(-EFAULT);
                    break;
                }

                sys_return(result);
            } while (0);
            syscall_leave("d-P", 8, print_rlimit);
            break;
        }

        case 78: { /*0x4e, gettimeofday*/       
            syscall_enter("gettimeofday", "p");
            uint32_t tp = arg(0);
            struct timeval host_time;
            struct timeval_32 guest_time;

            int result = gettimeofday(&host_time, NULL);
            if (result == -1) {
                result = -errno;
            } else {
                guest_time.tv_sec = host_time.tv_sec;
                guest_time.tv_usec = host_time.tv_usec;
                if (sizeof(guest_time) != process->get_memory()->write(&guest_time, tp, sizeof guest_time))
                    result = -EFAULT;
            }

            sys_return(result);
            syscall_leave("dP", sizeof guest_time, print_timeval_32);
            break;
        }

        case 83: { /*0x53, symlink*/
            syscall_enter("symlink", "ss");
            do {
                uint32_t oldpath=arg(0), newpath=arg(1);
                bool error;
                std::string sys_oldpath = read_string(oldpath, 0, &error);
                if (error) {
                    sys_return(-EFAULT);
                    break;
                }
                std::string sys_newpath = read_string(newpath, 0, &error);
                if (error) {
                    sys_return(-EFAULT);
                    break;
                }
                int result = symlink(sys_oldpath.c_str(),sys_newpath.c_str());
                if (result == -1) result = -errno;
                sys_return(result);
            } while (0);
            syscall_leave("d");
            break;
        }

        case 85: { /*0x55, readlink*/
            syscall_enter("readlink", "spd");
            do {
                uint32_t path=arg(0), buf_va=arg(1), bufsize=arg(2);
                char sys_buf[bufsize];
                bool error;
                std::string sys_path = read_string(path, 0, &error);
                if (error) {
                    sys_return(-EFAULT);
                    break;
                }
                int result = readlink(sys_path.c_str(), sys_buf, bufsize);
                if (result == -1) {
                    result = -errno;
                } else {
                    size_t nwritten = process->get_memory()->write(sys_buf, buf_va, result);
                    ROSE_ASSERT(nwritten == (size_t)result);
                }
                sys_return(result);
            } while (0);
            syscall_leave("d");
            break;
        }
            
        case 91: { /*0x5b, munmap*/
            syscall_enter("munmap", "pd");
            do {
                uint32_t va=arg(0);
                uint32_t sz=arg(1);
                uint32_t aligned_va = ALIGN_DN(va, PAGE_SIZE);
                uint32_t aligned_sz = ALIGN_UP(sz+va-aligned_va, PAGE_SIZE);
                void *rose_addr = my_addr(aligned_va, aligned_sz);

                /* Check ranges */
                if (aligned_va+aligned_sz <= aligned_va) { /* FIXME: not sure if sz==0 is an error */
                    sys_return(-EINVAL);
                    break;
                }

                /* Make sure that the specified memory range is actually mapped, or return -ENOMEM. */
                ExtentMap extents;
                extents.insert(ExtentPair(aligned_va, aligned_sz));
                extents.erase(process->get_memory()->va_extents());
                if (!extents.empty()) {
                    sys_return(-ENOMEM);
                    break;
                }

                /* Erase the mapping from the simulation */
                process->get_memory()->erase(MemoryMap::MapElement(aligned_va, aligned_sz));

                /* Also unmap for real, because if we don't, and the mapping was not anonymous, and the file that was mapped is
                 * unlinked, and we're on NFS, an NFS temp file is created in place of the unlinked file. */
                if (rose_addr && ALIGN_UP((uint64_t)rose_addr, (uint64_t)PAGE_SIZE)==(uint64_t)rose_addr)
                    (void)munmap(rose_addr, aligned_sz);

                sys_return(0);
            } while (0);
            if (tracing(TRACE_MMAP)) {
                fprintf(tracing(TRACE_MMAP), " memory map after munmap syscall:\n");
                process->get_memory()->dump(tracing(TRACE_MMAP), "    ");
            }
            syscall_leave("d");
            break;
        }

        case 93: { /* 0x5c, ftruncate */
            syscall_enter("ftruncate", "dd");
            int fd = arg(0);
            off_t len = arg(1);
            int result = ftruncate(fd, len);
            sys_return(-1==result ? -errno : result);
            syscall_leave("d");
            break;
        }

        case 94: { /* 0x5d, fchmod */

            /*
                int fchmod(int fd, mode_t mode);

                fchmod() changes the permissions of the file referred to by the open file
                         descriptor fd.
            */
            syscall_enter("fchmod", "dd");
	        uint32_t fd = arg(0);
	        mode_t mode = arg(1);

	        int result = fchmod(fd, mode);
            if (result == -1) result = -errno;
            sys_return(result);

            syscall_leave("d");
            break;
	    }

     	case 95: { /*0x5f, fchown */
            /* int fchown(int fd, uid_t owner, gid_t group);
             * typedef unsigned short  __kernel_old_uid_t;
             * typedef unsigned short  __kernel_old_gid_t;
             *
             * fchown() changes the ownership of the file referred to by the open file descriptor fd. */
            syscall_enter("fchown", "ddd");
            uint32_t fd = arg(0);
            int user = arg(1);
            int group = arg(2);
            int result = syscall(SYS_fchown, fd, user, group);
            sys_return(-1==result?-errno:result);
            syscall_leave("d");
            break;
        }

        case 99:    /* 0x63, statfs */
        case 100: { /* 0x63, fstatfs */

            if( 99 == callno )
              syscall_enter("statfs", "sp");
            else
              syscall_enter("fstatfs", "dp");

            do {
                int result;
                static statfs_64_native host_statfs;

                if(99==callno){

                  bool error;
                  std::string path = read_string(arg(0), 0, &error);
                  if (error) {
                      sys_return(-EFAULT);
                      break;
                  }

#ifdef SYS_statfs64 /* host is 32-bit machine */
                  result = syscall(SYS_statfs64 , path.c_str(), sizeof host_statfs, &host_statfs);
#else             /* host is 64-bit machine */
                  result = syscall(SYS_statfs, path.c_str(), &host_statfs);
#endif
                }else{

#ifdef SYS_statfs64 /* host is 32-bit machine */
                  result = syscall(SYS_fstatfs64 ,arg(0), sizeof host_statfs, &host_statfs);
#else             /* host is 64-bit machine */
                  result = syscall(SYS_fstatfs, arg(0), &host_statfs);
#endif

                }

                if (-1==result) {
                    sys_return(-errno);
                    break;
                }

                statfs_32 guest_statfs;
                guest_statfs.f_type = host_statfs.f_type;
                guest_statfs.f_bsize = host_statfs.f_bsize;
                guest_statfs.f_blocks = host_statfs.f_blocks;
                guest_statfs.f_bfree = host_statfs.f_bfree;
                guest_statfs.f_bavail = host_statfs.f_bavail;
                guest_statfs.f_files = host_statfs.f_files;
                guest_statfs.f_ffree = host_statfs.f_ffree;
                guest_statfs.f_fsid[0] = host_statfs.f_fsid[0];
                guest_statfs.f_fsid[1] = host_statfs.f_fsid[1];
                guest_statfs.f_namelen = host_statfs.f_namelen;
                guest_statfs.f_frsize = host_statfs.f_frsize;
                guest_statfs.f_flags = host_statfs.f_flags;
                guest_statfs.f_spare[0] = host_statfs.f_spare[0];
                guest_statfs.f_spare[1] = host_statfs.f_spare[1];
                guest_statfs.f_spare[2] = host_statfs.f_spare[2];
                guest_statfs.f_spare[3] = host_statfs.f_spare[3];
                if (sizeof(guest_statfs)!=process->get_memory()->write(&guest_statfs, arg(1), sizeof guest_statfs)) {
                    sys_return(-EFAULT);
                    break;
                }

                sys_return(result);
            } while (0);
            syscall_leave("d-P", sizeof(statfs_32), print_statfs_32);
            break;
        }

        case 102: { /* 0x66, socketcall */
            /* Return value is written to eax by these helper functions. The struction of this code closely follows that in the
             * Linux kernel. See linux/net/socket.c. */
            static const Translate socketcall_commands[] = {
                TE2(1, SYS_SOCKET),
                TE2(2, SYS_BIND),
                TE2(3, SYS_CONNECT),
                TE2(4, SYS_LISTEN),
                TE2(5, SYS_ACCEPT),
                TE2(6, SYS_GETSOCKNAME),
                TE2(7, SYS_GETPEERNAME),
                TE2(8, SYS_SOCKETPAIR),
                TE2(9, SYS_SEND),
                TE2(10, SYS_RECV),
                TE2(11, SYS_SENDTO),
                TE2(12, SYS_RECVFROM),
                TE2(13, SYS_SHUTDOWN),
                TE2(14, SYS_SETSOCKOPT),
                TE2(15, SYS_GETSOCKOPT),
                TE2(16, SYS_SENDMSG),
                TE2(17, SYS_RECVMSG),
                TE2(18, SYS_ACCEPT4),
                TE2(19, SYS_RECVMMSG),
                T_END
            };

            uint32_t a[6];
            switch (arg(0)) {
                case 1: { /* SYS_SOCKET */
                    if (12!=process->get_memory()->read(a, arg(1), 12)) {
                        sys_return(-EFAULT);
                        goto socketcall_error;
                    }
                    syscall_enter(a, "socket", "fff", protocol_families, socket_types, socket_protocols);
                    sys_socket(a[0], a[1], a[2]);
                    break;
                }
                    
                case 2: { /* SYS_BIND */
                    if (12!=process->get_memory()->read(a, arg(1), 12)) {
                        sys_return(-EFAULT);
                        goto socketcall_error;
                    }
                    syscall_enter(a, "bind", "dpd");    /* FIXME: we could do a better job printing the address [RPM 2011-01-04] */
                    sys_bind(a[0], a[1], a[2]);
                    break;
                }

                case 4: { /* SYS_LISTEN */
                    if (8!=process->get_memory()->read(a, arg(1), 8)) {
                        sys_return(-EFAULT);
                        goto socketcall_error;
                    }
                    syscall_enter(a, "listen", "dd");
                    sys_listen(a[0], a[1]);
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
                    sys_return(-ENOSYS);
                    goto socketcall_error;
                default:
                    sys_return(-EINVAL);
                    goto socketcall_error;
            }
            syscall_leave("d");
            break;

            socketcall_error:
            syscall_enter("socketcall", "fp", socketcall_commands);
            syscall_leave("d");
            break;
        }

        case 114: { /*0x72, wait4*/
            static const Translate wflags[] = { TF(WNOHANG), TF(WUNTRACED), T_END };
            syscall_enter("wait4", "dpfp", wflags);
            pid_t pid=arg(0);
            uint32_t status_va=arg(1), rusage_va=arg(3);
            int options=arg(2);
            int status;
            struct rusage rusage;
            int result = wait4(pid, &status, options, &rusage);
            if( result == -1) {
                result = -errno;
            } else {
                if (status_va != 0) {
                    size_t nwritten = process->get_memory()->write(&status, status_va, 4);
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
                    size_t nwritten = process->get_memory()->write(&out, rusage_va, sizeof out);
                    ROSE_ASSERT(nwritten == sizeof out);
                }
            }
            sys_return(result);
            syscall_leave("d");
            break;
        }

        case 116: { /* 0x74, sysinfo*/
            syscall_enter("sysinfo", "p");

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
                sys_return(-errno);
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

                size_t nwritten = process->get_memory()->write(&guest_sys, arg(0), sizeof(guest_sys));
                if (nwritten!=sizeof(guest_sys)) {
                    sys_return(-EFAULT);
                } else {
                    sys_return(result);
                }
            }
            syscall_leave("d");

            break;
        };

        case 117: { /* 0x75, ipc */
            /* Return value is written to eax by these helper functions. The structure of this code closely follows that in the
             * Linux kernel. */
            unsigned call = arg(0) & 0xffff;
            int version = arg(0) >> 16;
            uint32_t first=arg(1), second=arg(2), third=arg(3), ptr=arg(4), fifth=arg(5);
            switch (call) {
                case 1: /* SEMOP */
                    syscall_enter("ipc", "fdd-p", ipc_commands);
                    sys_semtimedop(first, ptr, second, 0);
                    syscall_leave("d");
                    break;
                case 2: /* SEMGET */
                    syscall_enter("ipc", "fddf", ipc_commands, ipc_flags);
                    sys_semget(first, second, third);
                    syscall_leave("d");
                    break;
                case 3: /* SEMCTL */
                    switch (third & 0xff) {
                        case 16: /*SETVAL*/
                            syscall_enter("ipc", "fddfP", ipc_commands, sem_control, 4, print_int_32);
                            break;
                        default:
                            syscall_enter("ipc", "fddfp", ipc_commands, sem_control);
                            break;
                    }
                    sys_semctl(first, second, third, ptr);
                    syscall_leave("d");
                    break;
                case 4: /* SEMTIMEDOP */
                    syscall_enter("ipc", "fdd-pP", ipc_commands, sizeof(timespec_32), print_timespec_32);
                    sys_semtimedop(first, ptr, second, fifth);
                    syscall_leave("d");
                    break;
                case 11: /* MSGSND */
                    syscall_enter("ipc", "fddfb", ipc_commands, ipc_flags, (size_t)(4+arg(2)));
                    sys_msgsnd(first, ptr, second, third);
                    syscall_leave("d");
                    break;
                case 12: /* MSGRCV */
                    if (0==version) {
                        syscall_enter("ipc", "fddfP", ipc_commands, ipc_flags, sizeof(ipc_kludge_32), print_ipc_kludge_32);
                        ipc_kludge_32 kludge;
                        if (8!=process->get_memory()->read(&kludge, arg(4), 8)) {
                            sys_return(-ENOSYS);
                        } else {
                            sys_msgrcv(first, kludge.msgp, second, kludge.msgtyp, third);
                        }
                    } else {
                        syscall_enter("ipc", "fddfpd", ipc_commands, ipc_flags);
                        sys_msgrcv(first, ptr, second, fifth, third);
                    }
                    syscall_leave("d");
                    break;
                case 13: /* MSGGET */ {
                    syscall_enter("ipc", "fpf", ipc_commands, ipc_flags); /* arg1 "p" for consistency with strace and ipcs */
                    sys_msgget(first, second);
                    syscall_leave("d");
                    break;
                }
                case 14: /* MSGCTL */ {
                    switch (second & 0xff) {
                        case 0:         /* IPC_RMID */
                            syscall_enter("ipc", "fdf", ipc_commands, msg_control);
                            break;
                        case 1:         /* IPC_SET */
                            syscall_enter("ipc", "fdf-P", ipc_commands, msg_control, sizeof(msqid64_ds_32), print_msqid64_ds_32);
                            break;
                        default:
                            syscall_enter("ipc", "fdf-p", ipc_commands, msg_control);
                            break;
                    }

                    sys_msgctl(first, second, ptr);

                    switch (second & 0xff) {
                        case 3:         /* IPC_INFO */
                        case 12:        /* MSG_INFO */
                            ROSE_ASSERT(!"not handled");
                        case 2:         /* IPC_STAT */
                        case 11:        /* MSG_STAT */
                            syscall_leave("d----P", sizeof(msqid64_ds_32), print_msqid64_ds_32);
                            break;
                        default:
                            syscall_leave("d");
                            break;
                    }
                    break;
                }
                case 21: /* SHMAT */
                    if (1==version) {
                        /* This was the entry point for kernel-originating calls from iBCS2 in 2.2 days */
                        syscall_enter("ipc", "fdddpd", ipc_commands);
                        sys_return(-EINVAL);
                        syscall_leave("d");
                    } else {
                        syscall_enter("ipc", "fdfpp", ipc_commands, ipc_flags);
                        sys_shmat(first, second, third, ptr);
                        syscall_leave("p");
                        if (tracing(TRACE_MMAP)) {
                            fprintf(tracing(TRACE_MMAP), "  memory map after shmat:\n");
                            process->get_memory()->dump(tracing(TRACE_MMAP), "    ");
                        }
                    }
                    break;
                case 22: /* SHMDT */
                    syscall_enter("ipc", "f---p", ipc_commands);
                    sys_shmdt(ptr);
                    syscall_leave("d");
                    if (tracing(TRACE_MMAP)) {
                        fprintf(tracing(TRACE_MMAP), "  memory map after shmdt:\n");
                        process->get_memory()->dump(tracing(TRACE_MMAP), "    ");
                    }
                    break;
                case 23: /* SHMGET */
                    syscall_enter("ipc", "fpdf", ipc_commands, ipc_flags); /* arg1 "p" for consistency with strace and ipcs */
                    sys_shmget(first, second, third);
                    syscall_leave("d");
                    break;
                case 24: { /* SHMCTL */
                    switch (second & 0xff) {
                        case 0:         /* IPC_RMID */
                            syscall_enter("ipc", "fdf", ipc_commands, shm_control);
                            break;
                        case 1:         /* IPC_SET */
                            syscall_enter("ipc", "fdf-P", ipc_commands, shm_control, sizeof(shmid64_ds_32), print_shmid64_ds_32);
                            break;
                        case 11:        /* SHM_LOCK */
                        case 12:        /* SHM_UNLOCK */
                            syscall_enter("ipc", "fdf", ipc_commands, shm_control);
                            break;
                        default:
                            syscall_enter("ipc", "fdf-p", ipc_commands, shm_control);
                            break;
                    }
                    
                    sys_shmctl(first, second, ptr);

                    switch (second & 0xff) {
                        case 2:         /* IPC_STAT */
                        case 13:        /* SHM_STAT */
                            syscall_leave("d----P", sizeof(shmid64_ds_32), print_shmid64_ds_32);
                            break;
                        case 14:        /* SHM_INFO */
                            syscall_leave("d----P", sizeof(shm_info_32), print_shm_info_32);
                            break;
                        case 3:         /* IPC_INFO */
                            syscall_leave("d----P", sizeof(shminfo64_32), print_shminfo64_32);
                            break;
                        default:
                            syscall_leave("d");
                            break;
                    }
                    break;
                }
                default:
                    syscall_enter("ipc", "fdddpd", ipc_commands);
                    sys_return(-ENOSYS);
                    syscall_leave("d");
                    break;
            }
            break;
        }

        case 118: { /* 0x76, fsync */
            /*
                int fsync(int fd);
            */
            syscall_enter("fsync", "d");
            int result = fsync( arg(0));
            sys_return(-1==result?-errno:result);
            syscall_leave("d");
            break;
        }

        case 120: { /* 0x78, clone */
            /* From linux arch/x86/kernel/process.c:
             *    long sys_clone(unsigned long clone_flags, unsigned long newsp,
             *                   void __user *parent_tid, void __user *child_tid, struct pt_regs *regs)
             */
            syscall_enter("clone", "fpppp", clone_flags);
            do {
                unsigned flags = arg(0);
                unsigned newsp = arg(1);
                unsigned parent_tid_va = arg(2);
                unsigned child_tid_va = arg(3);
                unsigned regs_va = arg(4);
                
                /* We cannot handle multiple threads yet. */
                if (newsp || parent_tid_va || child_tid_va || (flags & (CLONE_VM|CLONE_THREAD))) {
                    sys_return(-EINVAL);
                    break;
                }

                /* ROSE simulates signal handling, therefore signal handlers cannot be shared. */
                if (flags & CLONE_SIGHAND) {
                    sys_return(-EINVAL);
                    break;
                }

                /* Flush some files so buffered content isn't output twice. */
                fflush(stdout);
                fflush(stderr);
                if (tracing(TRACE_ALL))
                    fflush(tracing(TRACE_ALL));

                /* We cannot use clone() because it's a wrapper around the clone system call and we'd need to provide a
                 * function for it to execute. We want fork-like semantics. */
                pid_t pid = fork();
                if (-1==pid) {
                    sys_return(-errno);
                    break;
                }

                if (0==pid) {
                    /* Pending signals are only for the parent */
                    signal_pending = 0;

                    /* Open new log files if necessary */
                    process->open_trace_file();
                    if (process->btrace_file) {
                        fclose(process->btrace_file);
                        process->btrace_file = NULL;
                    }

                    /* Thread-related things. ROSE isn't multi-threaded and the simulator doesn't support multi-threading, but
                     * we still have to initialize a few data structures because the specimen may be using a thread-aware
                     * library. */
                    if (0!=(flags & CLONE_CHILD_SETTID)) {
                        set_child_tid = child_tid_va;
                        if (set_child_tid) {
                            uint32_t pid32 = getpid();
                            size_t nwritten = process->get_memory()->write(&pid32, set_child_tid, 4);
                            ROSE_ASSERT(4==nwritten);
                        }
                    }
                    if (0!=(flags & CLONE_CHILD_CLEARTID)) {
                        clear_child_tid = child_tid_va;
                    }

                    /* Return register values in child */
                    pt_regs_32 regs;
                    regs.bx = policy.readGPR(x86_gpr_bx).known_value();
                    regs.cx = policy.readGPR(x86_gpr_cx).known_value();
                    regs.dx = policy.readGPR(x86_gpr_dx).known_value();
                    regs.si = policy.readGPR(x86_gpr_si).known_value();
                    regs.di = policy.readGPR(x86_gpr_di).known_value();
                    regs.bp = policy.readGPR(x86_gpr_bp).known_value();
                    regs.sp = policy.readGPR(x86_gpr_sp).known_value();
                    regs.cs = policy.readSegreg(x86_segreg_cs).known_value();
                    regs.ds = policy.readSegreg(x86_segreg_ds).known_value();
                    regs.es = policy.readSegreg(x86_segreg_es).known_value();
                    regs.fs = policy.readSegreg(x86_segreg_fs).known_value();
                    regs.gs = policy.readSegreg(x86_segreg_gs).known_value();
                    regs.ss = policy.readSegreg(x86_segreg_ss).known_value();
                    uint32_t flags = 0;
                    for (size_t i=0; i<VirtualMachineSemantics::State::n_flags; i++) {
                        if (policy.readFlag((X86Flag)i).known_value()) {
                            flags |= (1u<<i);
                        }
                    }
                    if (sizeof(regs)!=process->get_memory()->write(&regs, regs_va, sizeof regs)) {
                        sys_return(-EFAULT);
                        break;
                    }
                }

                sys_return(pid);
            } while (0);

            if (policy.readGPR(x86_gpr_ax).known_value()) {
                syscall_leave("d");
            } else {
                /* Child */
                syscall_enter("child's clone", "fpppp", clone_flags);
                syscall_leave("d----P", sizeof(pt_regs_32), print_pt_regs_32);
            }
            break;
        }

        case 122: { /*0x7a, uname*/
            syscall_enter("uname", "p");
            uint32_t dest_va=arg(0);
            char buf[6*65];
            memset(buf, ' ', sizeof buf);
            strcpy(buf+0*65, "Linux");                                  /*sysname*/
            strcpy(buf+1*65, "mymachine.example.com");                  /*nodename*/
            strcpy(buf+2*65, "2.6.9");                                  /*release*/
            strcpy(buf+3*65, "#1 SMP Wed Jun 18 12:35:02 EDT 2008");    /*version*/
            strcpy(buf+4*65, "i386");                                   /*machine*/
            strcpy(buf+5*65, "example.com");                            /*domainname*/
            size_t nwritten = process->get_memory()->write(buf, dest_va, sizeof buf);
            if( nwritten <= 0 ) {
              sys_return(-EFAULT);
              break;
            }

            ROSE_ASSERT(nwritten==sizeof buf);
            sys_return(0);
            syscall_leave("d");
            break;
        }

	case 133: { /* 0x85, fchdir */
            syscall_enter("fchdir", "d");
	    uint32_t file_descriptor = arg(0);

	    int result = fchdir(file_descriptor);
            if (result == -1) result = -errno;
            sys_return(result);

            syscall_leave("d");
            break;
	}

        case 125: { /*0x7d, mprotect*/
            static const Translate pflags[] = { TF(PROT_READ), TF(PROT_WRITE), TF(PROT_EXEC), TF(PROT_NONE), T_END };
            syscall_enter("mprotect", "pdf", pflags);
            uint32_t va=arg(0), size=arg(1), perms=arg(2);
            unsigned rose_perms = ((perms & PROT_READ) ? MemoryMap::MM_PROT_READ : 0) |
                                  ((perms & PROT_WRITE) ? MemoryMap::MM_PROT_WRITE : 0) |
                                  ((perms & PROT_EXEC) ? MemoryMap::MM_PROT_EXEC : 0);
            if (va % PAGE_SIZE) {
                sys_return(-EINVAL);
            } else {
                uint32_t aligned_sz = ALIGN_UP(size, PAGE_SIZE);

                /* Set protection in the underlying real memory (to catch things like trying to add write permission to memory
                 * that's mapped from a read-only file), then also set the protection in the simulated memory map so the simulator
                 * can make queries about memory access.  Some of the underlying memory points to parts of an ELF file that was
                 * read into ROSE's memory in such a way that segments are not aligned on page boundaries. We cannot change
                 * protections on these non-aligned sections. */
                if (-1==mprotect(my_addr(va, size), size, perms) && EINVAL!=errno) {
                    sys_return(-errno);
                } else {
                    try {
                        process->get_memory()->mprotect(MemoryMap::MapElement(va, aligned_sz, rose_perms));
                        sys_return(0);
                    } catch (const MemoryMap::NotMapped &e) {
                        sys_return(-ENOMEM);
                    }
                }
            }

            syscall_leave("d");
            if (tracing(TRACE_MMAP)) {
                fprintf(tracing(TRACE_MMAP), "  memory map after mprotect syscall:\n");
                process->get_memory()->dump(tracing(TRACE_MMAP), "    ");
            }
            break;
        }

       case 140: { /* 0x8c, llseek */
            /* From the linux kernel, arguments are:
             *      unsigned int fd,                // file descriptor
             *      unsigned long offset_high,      // high 32 bits of 64-bit offset
             *      unsigned long offset_low,       // low 32 bits of 64-bit offset
             *      loff_t __user *result,          // 64-bit user area to write resulting position
             *      unsigned int origin             // whence specified offset is measured
             */
            syscall_enter("llseek","dddpf", seek_whence);
            int fd = arg(0);
            off64_t offset = ((off64_t)arg(1) << 32) | arg(2);
            uint32_t result_va = arg(3);
            int whence = arg(4);

            off64_t result = lseek64(fd, offset, whence);
            if (-1==result) {
                sys_return(-errno);
            } else {
                sys_return(0);
                size_t nwritten = process->get_memory()->write(&result, result_va, sizeof result);
                ROSE_ASSERT(nwritten==sizeof result);
            }
            syscall_leave("d");
            break;
        };
 	case 141: {     /* 0xdc, getdents(int fd, struct linux_dirent*, unsigned int count) */
            syscall_enter("getdents", "dpd");
            int fd = arg(0), sz = arg(2);
            uint32_t dirent_va = arg(1);
            int status = getdents_syscall<dirent32_t>(fd, dirent_va, sz);
            sys_return(status);
            syscall_leave("d-P", status>0?status:0, print_dentries_32);
            break;
        }

        case 142: { /*0x8e , select */
            /* From the Linux kernel (fs/select.c):
             *    SYSCALL_DEFINE5(select, int, n, fd_set __user *, inp, fd_set __user *, outp,
             *                    fd_set __user *, exp, struct timeval __user *, tvp)
             * where:
             *    fd_set is enough "unsigned long" data to contain 1024 bits. Regardless of the size of "unsigned long",
             *    the file bits will be in the same order (we are the host is little endian), and the fd_set is the same size. */
            syscall_enter("select", "dPPPP",
                          sizeof(fd_set), print_bitvec,
                          sizeof(fd_set), print_bitvec,
                          sizeof(fd_set), print_bitvec,
                          sizeof(timeval_32), print_timeval_32);
            do {
                int fd = arg(0);
                uint32_t in_va=arg(1), out_va=arg(2), ex_va=arg(3), tv_va=arg(4);

                fd_set in, out, ex;
                fd_set *inp=NULL, *outp=NULL, *exp=NULL;

                ROSE_ASSERT(128==sizeof(fd_set)); /* 128 bytes = 1024 file descriptor bits */
                if (in_va && sizeof(in)==process->get_memory()->read(&in, in_va, sizeof in))
                    inp = &in;
                if (out_va && sizeof(out)==process->get_memory()->read(&out, out_va, sizeof out))
                    outp = &out;
                if (ex_va && sizeof(ex)==process->get_memory()->read(&ex, ex_va, sizeof ex))
                    exp = &ex;

                timeval_32 guest_timeout;
                timeval host_timeout, *tvp=NULL;
                if (tv_va) {
                    if (sizeof(guest_timeout)!=process->get_memory()->read(&guest_timeout, tv_va, sizeof guest_timeout)) {
                        sys_return(-EFAULT);
                        break;
                    } else {
                        host_timeout.tv_sec = guest_timeout.tv_sec;
                        host_timeout.tv_usec = guest_timeout.tv_usec;
                        tvp = &host_timeout;
                    }
                }

                int result = select(fd, inp, outp, exp, tvp);
                if (-1==result) {
                    sys_return(-errno);
                    break;
                }

                if ((in_va  && sizeof(in) !=process->get_memory()->write(inp,  in_va,  sizeof in))  ||
                    (out_va && sizeof(out)!=process->get_memory()->write(outp, out_va, sizeof out)) ||
                    (ex_va  && sizeof(ex) !=process->get_memory()->write(exp,  ex_va,  sizeof ex))) {
                    sys_return(-EFAULT);
                    break;
                }

                if (tvp) {
                    guest_timeout.tv_sec = tvp->tv_sec;
                    guest_timeout.tv_usec = tvp->tv_usec;
                    if (sizeof(guest_timeout)!=process->get_memory()->write(&guest_timeout, tv_va, sizeof guest_timeout)) {
                        sys_return(-EFAULT);
                        break;
                    }
                }
            } while (0);
            syscall_leave("d-PPPP",
                          sizeof(fd_set), print_bitvec,
                          sizeof(fd_set), print_bitvec,
                          sizeof(fd_set), print_bitvec,
                          sizeof(timeval_32), print_timeval_32);
            break;
        }

        case 144: { /* 0x90, int msync(void *addr, size_t length, int flags) */
            static const Translate msync_flags[] = { TF(MS_ASYNC), TF(MS_SYNC), TF(MS_INVALIDATE), T_END };
            syscall_enter("msync", "pdf", msync_flags);
            do {
                if (arg(0) % 4096) {
                    sys_return(-EINVAL);
                    break;
                }

                void *addr = my_addr(arg(0), arg(1));
                if (!addr) {
                    sys_return(-ENOMEM);
                    break;
                }
                
                int result = msync(addr, arg(1), arg(2));
                sys_return(-1==result?-errno:result);
            } while (0);
            syscall_leave("d");
            break;
        }

        case 146: { /*0x92, writev*/
            syscall_enter("writev", "dpd");
            uint32_t fd=arg(0), iov_va=arg(1);
            int niov=arg(2), idx=0;
            uint32_t retval = 0;
            if (niov<0 || niov>1024) {
                retval = -EINVAL;
            } else {
                for (idx=0; idx<niov; idx++) {
                    /* Obtain buffer address and size */
                    uint32_t buf_va;
                    if (4 != process->get_memory()->read(&buf_va, iov_va+idx*8+0, 4)) {
                        if (0==idx)
                            retval = -EFAULT;
                        if (tracing(TRACE_SYSCALL))
                            fprintf(tracing(TRACE_SYSCALL), "    #%d: segmentation fault reading address\n", idx);
                        break;
                    }

                    uint32_t buf_sz;
                    if (4 != process->get_memory()->read(&buf_sz, iov_va+idx*8+4, 4)) {
                        if (0==idx)
                            retval = -EFAULT;
                        if (tracing(TRACE_SYSCALL))
                            fprintf(tracing(TRACE_SYSCALL), "    #%d: segmentation fault reading size\n", idx);
                        break;
                    }

                    if (tracing(TRACE_SYSCALL)) {
                        if (0==idx) fprintf(tracing(TRACE_SYSCALL), "<see below>\n"); /* return value is delayed */
                        fprintf(tracing(TRACE_SYSCALL), "    #%d: va=0x%08"PRIx32", size=0x%08"PRIx32, idx, buf_va, buf_sz);
                    }

                    /* Make sure total size doesn't overflow a ssize_t */
                    if ((buf_sz & 0x80000000) || (retval+buf_sz) & 0x80000000) {
                        if (0==idx)
                            retval = -EINVAL;
                        if (tracing(TRACE_SYSCALL))
                            fprintf(tracing(TRACE_SYSCALL), " size overflow\n");
                        break;
                    }

                    /* Copy data from guest to host because guest memory might not be contiguous in the host. Perhaps a more
                     * efficient way to do this would be to copy chunks of host-contiguous data in a loop instead. */
                    uint8_t buf[buf_sz];
                    if (buf_sz != process->get_memory()->read(buf, buf_va, buf_sz)) {
                        if (0==idx)
                            retval = -EFAULT;
                        if (tracing(TRACE_SYSCALL))
                            fprintf(tracing(TRACE_SYSCALL), " segmentation fault\n");
                        break;
                    }

                    /* Write data to the file */
                    ssize_t nwritten = write(fd, buf, buf_sz);
                    if (-1==nwritten) {
                        if (0==idx)
                            retval = -errno;
                        if (tracing(TRACE_SYSCALL))
                            fprintf(tracing(TRACE_SYSCALL), " write failed (%s)\n", strerror(errno));
                        break;
                    }
                    retval += nwritten;
                    if ((uint32_t)nwritten<buf_sz) {
                        if (tracing(TRACE_SYSCALL))
                            fprintf(tracing(TRACE_SYSCALL), " short write (%zd bytes)\n", nwritten);
                        break;
                    }
                    if (tracing(TRACE_SYSCALL))
                        fputc('\n', tracing(TRACE_SYSCALL));
                }
            }
            sys_return(retval);
            if (tracing(TRACE_SYSCALL) && niov>0 && niov<=1024)
                fprintf(tracing(TRACE_SYSCALL), "%*s = ", 51, ""); /* align for return value */
            syscall_leave("d");
            break;
        }

        case 162: { /* 0xa2, nanosleep */
            syscall_enter("nanosleep", "Pp", sizeof(timespec_32), print_timespec_32);
            do {
                timespec_32 guest_ts;
                timespec host_ts_in, host_ts_out;
                if (sizeof(guest_ts)!=process->get_memory()->read(&guest_ts, arg(0), sizeof guest_ts)) {
                    sys_return(-EFAULT);
                    break;
                }
                if (guest_ts.tv_sec<0 || (unsigned long)guest_ts.tv_nsec >= 1000000000L) {
                    sys_return(-EINVAL);
                    break;
                }
                host_ts_in.tv_sec = guest_ts.tv_sec;
                host_ts_in.tv_nsec = guest_ts.tv_nsec;

                int result = nanosleep(&host_ts_in, &host_ts_out);
                if (arg(1) && -1==result && EINTR==errno) {
                    guest_ts.tv_sec = host_ts_out.tv_sec;
                    guest_ts.tv_nsec = host_ts_out.tv_nsec;
                    if (sizeof(guest_ts)!=process->get_memory()->write(&guest_ts, arg(1), sizeof guest_ts)) {
                        sys_return(-EFAULT);
                        break;
                    }
                }
                sys_return(-1==result?-errno:result);
            } while (0);
            syscall_leave("d-P", sizeof(timespec_32), print_timespec_32);
            break;
        }

        case 174: { /*0xae, rt_sigaction*/
            syscall_enter("rt_sigaction", "fPpd", signal_names, sizeof(sigaction_32), print_sigaction_32);
            do {
                int signum=arg(0);
                uint32_t action_va=arg(1), oldact_va=arg(2);
                size_t sigsetsize=arg(3);

                if (sigsetsize!=8 || signum<1 || signum>_NSIG) {
                    sys_return(-EINVAL);
                    break;
                }


                sigaction_32 tmp;
                if (action_va && sizeof(tmp) != process->get_memory()->read(&tmp, action_va, sizeof tmp)) {
                    sys_return(-EFAULT);
                    break;
                }
                if (oldact_va && sizeof(tmp) != process->get_memory()->write(signal_action+signum-1, oldact_va, sizeof tmp)) {
                    sys_return(-EFAULT);
                    break;
                }
                if (action_va)
                    signal_action[signum-1] = tmp;
                sys_return(0);
            } while (0);
            syscall_leave("d--P", sizeof(sigaction_32), print_sigaction_32);
            break;
        }

        case 175: { /*0xaf, rt_sigprocmask*/
            static const Translate flags[] = { TF(SIG_BLOCK), TF(SIG_UNBLOCK), TF(SIG_SETMASK), T_END };
            syscall_enter("rt_sigprocmask", "fPp", flags, (size_t)8, print_sigmask);

            int how=arg(0);
            uint32_t set_va=arg(1), get_va=arg(2);
            //size_t sigsetsize=arg(3);

            uint64_t saved=signal_mask, sigset=0;
            if ( set_va != 0 ) {

                size_t nread = process->get_memory()->read(&sigset, set_va, sizeof sigset);
                ROSE_ASSERT(nread==sizeof sigset);

                if (0==how) {
                    /* SIG_BLOCK */
                    signal_mask |= sigset;
                } else if (1==how) {
                    /* SIG_UNBLOCK */
                    signal_mask &= ~sigset;
                } else if (2==how) {
                    /* SIG_SETMASK */
                    signal_mask = sigset;
                } else {
                    sys_return(-EINVAL);
                    break;
                }
                if (signal_mask!=saved)
                    signal_reprocess = true;
            }

            if (get_va) {
                size_t nwritten = process->get_memory()->write(&saved, get_va, sizeof saved);
                ROSE_ASSERT(nwritten==sizeof saved);
            }
            sys_return(0);
            syscall_leave("d--P", (size_t)8, print_sigmask);
            break;
        }

        case 176: { /* 0xb0, rt_sigpending */
            syscall_enter("rt_sigpending", "p");
            uint32_t sigset_va=arg(0);
            ROSE_ASSERT(8==sizeof(signal_pending));
            if (8!=process->get_memory()->write(&signal_pending, sigset_va, 8)) {
                sys_return(-EFAULT);
            } else {
                sys_return(0);
            }
            syscall_leave("dP", sizeof(uint64_t), print_sigmask);
            break;
        }

        case 179: { /* 0xb3, rt_sigsuspend */
            syscall_enter("rt_sigsuspend", "Pd", (size_t)8, print_sigmask);
            do {
                ROSE_ASSERT(8==arg(1));
                ROSE_ASSERT(8==sizeof(signal_pending));
                uint64_t new_signal_mask;
                if (8!=process->get_memory()->read(&new_signal_mask, arg(0), 8)) {
                    sys_return(-EFAULT);
                    break;
                }
                uint64_t old_signal_mask = signal_mask;
                signal_mask = new_signal_mask;
                signal_pause();
                signal_mask = old_signal_mask;
                sys_return(-EINTR);
            } while (0);
            syscall_leave("d");
            break;
        }

	case 183: { /* 0xb7, getcwd */
            syscall_enter("getcwd", "pd");
            do {
                static char buf[4096]; /* page size in kernel */
                int result = syscall(SYS_getcwd, buf, sizeof buf);
                if (-1==result) {
                    sys_return(-errno);
                    break;
                }

                size_t len = strlen(buf) + 1;
                if (len > arg(1)) {
                    sys_return(-ERANGE);
                    break;
                }

                if (len!=process->get_memory()->write(buf, arg(0), len)) {
                    sys_return(-EFAULT);
                    break;
                }

                sys_return(result);
            } while (0);
            syscall_leave("ds");
            break;
        }

        case 186: { /* 0xba, sigaltstack*/
            syscall_enter("sigaltstack", "Pp", sizeof(stack_32), print_stack_32);
            do {
                uint32_t new_stack_va=arg(0), old_stack_va=arg(1);

                /* Are we currently executing on the alternate stack? */
                uint32_t sp = policy.readGPR(x86_gpr_sp).known_value();
                bool on_stack = (0==(signal_stack.ss_flags & SS_DISABLE) &&
                                 sp >= signal_stack.ss_sp &&
                                 sp < signal_stack.ss_sp + signal_stack.ss_size);

                if (old_stack_va) {
                    stack_32 tmp = signal_stack;
                    tmp.ss_flags &= ~SS_ONSTACK;
                    if (on_stack) tmp.ss_flags |= SS_ONSTACK;
                    if (sizeof(tmp)!=process->get_memory()->write(&tmp, old_stack_va, sizeof tmp)) {
                        sys_return(-EFAULT);
                        break;
                    }
                }

                if (new_stack_va) {
                    stack_32 tmp;
                    tmp.ss_flags &= ~SS_ONSTACK;
                    if (sizeof(tmp)!=process->get_memory()->read(&tmp, new_stack_va, sizeof tmp)) {
                        sys_return(-EFAULT);
                        break;
                    }
                    if (on_stack) {
                        sys_return(-EINVAL);  /* can't set alt stack while we're using it */
                        break;
                    } else if ((tmp.ss_flags & ~(SS_DISABLE|SS_ONSTACK))) {
                        sys_return(-EINVAL);  /* invalid flags */
                        break;
                    } else if (0==(tmp.ss_flags & SS_DISABLE) && tmp.ss_size < 4096) {
                        sys_return(-ENOMEM);  /* stack must be at least one page large */
                        break;
                    }
                    signal_stack = tmp;
                }

                sys_return(0);
            } while (0);
            syscall_leave("d-P", sizeof(stack_32), print_stack_32);
            break;
        }

        // case 191 (0xbf, ugetrlimit). See case 76. I think they're the same. [RPM 2010-11-12]

        case 192: { /*0xc0, mmap2*/
            static const Translate pflags[] = { TF(PROT_READ), TF(PROT_WRITE), TF(PROT_EXEC), TF(PROT_NONE), T_END };
            static const Translate mflags[] = { TF(MAP_SHARED), TF(MAP_PRIVATE), TF(MAP_ANONYMOUS), TF(MAP_DENYWRITE),
                                                TF(MAP_EXECUTABLE), TF(MAP_FILE), TF(MAP_FIXED), TF(MAP_GROWSDOWN),
                                                TF(MAP_LOCKED), TF(MAP_NONBLOCK), TF(MAP_NORESERVE),
#ifdef MAP_32BIT
                                                TF(MAP_32BIT),
#endif
                                                TF(MAP_POPULATE), T_END };
            syscall_enter("mmap2", "pdffdd", pflags, mflags);
            uint32_t start=arg(0), size=arg(1), prot=arg(2), flags=arg(3), offset=arg(5)*PAGE_SIZE;
            int fd=arg(4);
            size_t aligned_size = ALIGN_UP(size, PAGE_SIZE);
            void *buf = NULL;
            unsigned rose_perms = ((prot & PROT_READ) ? MemoryMap::MM_PROT_READ : 0) |
                                  ((prot & PROT_WRITE) ? MemoryMap::MM_PROT_WRITE : 0) |
                                  ((prot & PROT_EXEC) ? MemoryMap::MM_PROT_EXEC : 0);
            //prot |= PROT_READ | PROT_WRITE | PROT_EXEC; /* ROSE takes care of permissions checking */

            if (!start) {
                try {
                    start = process->get_memory()->find_free(process->mmap_start, aligned_size, PAGE_SIZE);
                } catch (const MemoryMap::NoFreeSpace &e) {
                    sys_return(-ENOMEM);
                    goto mmap2_done;
                }
            }
            if (!process->mmap_recycle)
                process->mmap_start = std::max(process->mmap_start, start);

            if (flags & MAP_ANONYMOUS) {
                buf = mmap(NULL, size, prot, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
            } else {
                buf = mmap(NULL, size, prot, flags & ~MAP_FIXED, fd, offset);
            }
            if (MAP_FAILED==buf) {
                sys_return(-errno);
            } else {
                /* Try to figure out a reasonable name for the map element. If we're mapping a file, we can get the file name
                 * from the proc filesystem. The name is only used to aid debugging. */
                std::string melmt_name = "anonymous";
                if (fd>=0 && 0==(flags & MAP_ANONYMOUS)) {
                    char fd_namebuf[4096];
                    ssize_t nread = readlink(("/proc/self/fd/"+StringUtility::numberToString(fd)).c_str(),
                                             fd_namebuf, sizeof(fd_namebuf)-1);
                    if (nread>45) {
                        fd_namebuf[nread] = '\0';
                        char *slash = strrchr(fd_namebuf, '/');
                        melmt_name = slash ? slash+1 : fd_namebuf;
                    } else if (nread>0) {
                        fd_namebuf[nread] = '\0';
                        melmt_name = fd_namebuf;
                    } else {
                        melmt_name = "fd=" + StringUtility::numberToString(fd);
                    }
                }

                MemoryMap::MapElement melmt(start, aligned_size, buf, 0, rose_perms);
                melmt.set_name("mmap2("+melmt_name+")");
                process->get_memory()->erase(melmt); /*clear space space first to avoid MemoryMap::Inconsistent exception*/
                process->get_memory()->insert(melmt);
                sys_return(start);
            }

        mmap2_done:
            syscall_leave("p");
            if (tracing(TRACE_MMAP)) {
                fprintf(tracing(TRACE_MMAP), "  memory map after mmap2 syscall:\n");
                process->get_memory()->dump(tracing(TRACE_MMAP), "    ");
            }

            break;
        }

        case 195:       /*0xc3, stat64*/
        case 196:       /*0xc4, lstat64*/
        case 197: {     /*0xc5, fstat64*/
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
            if (195==callno || 196==callno) {
                syscall_enter(195==callno?"stat64":"lstat64", "sp");
            } else {
                syscall_enter("fstat64", "dp");
            }

            do {
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
                    std::string name = read_string(arg(0), 0, &error);
                    if (error) {
                        sys_return(-EFAULT);
                        break;
                    }
                    result = syscall(host_callno, (unsigned long)name.c_str(), (unsigned long)kernel_stat);
                } else {
                    result = syscall(host_callno, (unsigned long)arg(0), (unsigned long)kernel_stat);
                }
                if (-1==result) {
                    sys_return(-errno);
                    break;
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
                    if (tracing(TRACE_SYSCALL))
                        fprintf(tracing(TRACE_SYSCALL), "[64-to-32] ");
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
                    process->get_memory()->write(&out, arg(1), sizeof out);
                } else {
                    process->get_memory()->write(kernel_stat, arg(1), kernel_stat_size);
                }

                sys_return(result);
            } while (0);
            syscall_leave("d-P", sizeof(kernel_stat_32), print_kernel_stat_32);
            break;
        }

	case 199: { /*0xc7, getuid32 */
            syscall_enter("getuid32", "");
            uid_t id = getuid();
            sys_return(id);
            syscall_leave("d");
	    break;
	}

	case 200: { /*0xc8, getgid32 */
            syscall_enter("getgid32", "");
            uid_t id = getgid();
            sys_return(id);
            syscall_leave("d");
            break;
        }

	case 201: { /*0xc9, geteuid32 */
            syscall_enter("geteuid32", "");
            uid_t id = geteuid();
            sys_return(id);
            syscall_leave("d");
            break;
        }

        case 202: { /*0xca, getegid32 */
            syscall_enter("getegid32", "");
            uid_t id = getegid();
            sys_return(id);
            syscall_leave("d");
            break;
        }
#if 0
        case 205: { /* 0xCD, getgroups32 */
            /*
               int getgroups(int size, gid_t list[]); 
            */
            syscall_enter("getgroups32", "d-");
            gid_t list[arg(0)];
            int result = getgroups( arg(0), list);
            size_t nwritten = process->get_memory()->write(list, arg(1), arg(0));
            ROSE_ASSERT(nwritten == (size_t)arg(0));

            sys_return(result);
            syscall_leave("d");
            break;
        }

        case 206: { /* 0xCD, setgroups32 */
            /*
               int setgroups(size_t size, const gid_t *list);
            */
            syscall_enter("setgroups32", "d-");
            gid_t list[arg(0)];
            size_t nread = process->get_memory()->write(&list, arg(1), arg(0));
            ROSE_ASSERT(nread == arg(0));
            int result = setgroups( arg(0), list);
            sys_return(-1==result?-errno:result);
            syscall_leave("d");
            break;
        }
#endif

        case 207: { /*0xcf, fchown32 */
            /* int fchown(int fd, uid_t owner, gid_t group);
             * typedef unsigned short  __kernel_old_uid_t;
             * typedef unsigned short  __kernel_old_gid_t;
             *
             * fchown() changes the ownership of the file referred to by the open file descriptor fd. */
            syscall_enter("fchown32", "ddd");
            int fd=arg(0), user=arg(1), group=arg(2);
            int result = syscall(SYS_fchown, fd, user, group);
            sys_return(-1==result?-errno:result);
            syscall_leave("d");
            break;
        }

        case 212: { /*0xd4, chown */
            syscall_enter("chown", "sdd");
            do {
                bool error;
                std::string filename = read_string(arg(0), 0, &error);
                if (error) {
                    sys_return(-EFAULT);
                    break;
                }
                uid_t user = arg(1);
                gid_t group = arg(2);
                int result = chown(filename.c_str(),user,group);
                sys_return(result);
            } while (0);
            syscall_leave("d");
            break;
        }

 	case 220: {     /* 0xdc, getdents64(int fd, struct linux_dirent*, unsigned int count) */
            syscall_enter("getdents64", "dpd");
            int fd = arg(0), sz = arg(2);
            uint32_t dirent_va = arg(1);
            int status = getdents_syscall<dirent64_t>(fd, dirent_va, sz);
            sys_return(status);
            syscall_leave("d-P", status>0?status:0, print_dentries_64);
            break;
        }

        case 221: { // 0xdd fcntl(int fd, int cmd, [long arg | struct flock*])
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
            int fd=arg(0), cmd=arg(1), other=arg(2), result=-EINVAL;
            switch (cmd) {
                case F_DUPFD:
#ifdef F_DUPFD_CLOEXEC
                case F_DUPFD_CLOEXEC:
#endif
                case F_GETFD:
                case F_GETFL:
                case F_GETOWN:
                case F_GETSIG: {
                    syscall_enter("fcntl64", "df", fcntl_cmds);
                    result = fcntl(fd, cmd, other);
                    sys_return(-1==result?-errno:result);
                    syscall_leave("d");
                    break;
                }
                case F_SETFD:
                case F_SETOWN: {
                    syscall_enter("fcntl64", "dfd", fcntl_cmds);
                    result = fcntl(fd, cmd, other);
                    sys_return(-1==result?-errno:result);
                    syscall_leave("d");
                    break;
                }
                case F_SETFL: {
                    syscall_enter("fcntl64", "dff", fcntl_cmds, open_flags);
                    result = fcntl(fd, cmd, other);
                    sys_return(-1==result?-errno:result);
                    syscall_leave("d");
                    break;
                }
                case F_SETSIG: {
                    syscall_enter("fcntl64", "dff", fcntl_cmds, signal_names);
                    result = fcntl(fd, cmd, other);
                    sys_return(-1==result?-errno:result);
                    syscall_leave("d");
                    break;
                }
                case F_GETLK:
                case F_SETLK:
                case F_SETLKW: {
                    syscall_enter("fcntl64", "dfP", fcntl_cmds, sizeof(flock_32), print_flock_32);
                    do {
                        flock_32 guest_fl;
                        static flock_native host_fl;
                        if (sizeof(guest_fl)!=process->get_memory()->read(&guest_fl, arg(2), sizeof guest_fl)) {
                            sys_return(-EFAULT);
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
                            sys_return(-errno);
                            break;
                        }
                        if (F_GETLK==cmd) {
                            guest_fl.l_type = host_fl.l_type;
                            guest_fl.l_whence = host_fl.l_whence;
                            guest_fl.l_start = host_fl.l_start;
                            guest_fl.l_len = host_fl.l_len;
                            guest_fl.l_pid = host_fl.l_pid;
                            if (sizeof(guest_fl)!=process->get_memory()->write(&guest_fl, arg(2), sizeof guest_fl)) {
                                sys_return(-EFAULT);
                                break;
                            }
                        }

                        sys_return(result);
                    } while (0);
                    if (F_GETLK==cmd) {
                        syscall_leave("d--P", sizeof(flock_32), print_flock_32);
                    } else {
                        syscall_leave("d");
                    }
                    break;
                }
                default:
                    syscall_enter("fcntl64", "dfd", fcntl_cmds);
                    sys_return(-EINVAL);
                    syscall_leave("d");
                    break;
            }
            break;
        }

        case 224: { /*0xe0, gettid*/
            // We have no concept of threads
            syscall_enter("gettid", "");
            sys_return(getpid());
            syscall_leave("d");
            break;
       }

        case 240: { /*0xf0, futex*/
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

            /* Variable arguments */
            unsigned arg1 = arg(1);
            arg1 &= 0x7f;
            switch (arg1) {
                case 0: /*FUTEX_WAIT*/
                    syscall_enter("futex", "PfdP", 4, print_int_32, opflags, sizeof(timespec_32), print_timespec_32);
                    break;
                case 1: /*FUTEX_WAKE*/
                    syscall_enter("futex", "Pfd", 4, print_int_32, opflags);
                    break;
                case 2: /*FUTEX_FD*/
                    syscall_enter("futex", "Pfd", 4, print_int_32, opflags);
                    break;
                case 3: /*FUTEX_REQUEUE*/
                    syscall_enter("futex", "Pfd-P", 4, print_int_32, opflags, 4, print_int_32);
                    break;
                case 4: /*FUTEX_CMP_REQUEUE*/
                    syscall_enter("futex", "Pfd-Pd", 4, print_int_32, opflags, 4, print_int_32);
                    break;
                default:
                    syscall_enter("futex", "PfdPPd", 4, print_int_32, opflags, sizeof(timespec_32), print_timespec_32, 
                                  4, print_int_32);
                    break;
            }

            uint32_t futex1_va=arg(0), op=arg(1), val1=arg(2), timeout_va=arg(3), futex2_va=arg(4), val3=arg(5);
            uint32_t *futex1 = (uint32_t*)my_addr(futex1_va, 4);
            uint32_t *futex2 = (uint32_t*)my_addr(futex2_va, 4);

            struct timespec timespec_buf, *timespec=NULL;
            if (timeout_va) {
                timespec_32 ts;
                size_t nread = process->get_memory()->read(&ts, timeout_va, sizeof ts);
                ROSE_ASSERT(nread==sizeof ts);
                timespec_buf.tv_sec = ts.tv_sec;
                timespec_buf.tv_nsec = ts.tv_nsec;
                timespec = &timespec_buf;
            }

#if 0 /* DEBUGGING [RPM 2011-01-13] */
            if (process->debug) {
                fprintf(process->debug,
                        "\nROBB: futex1=%p, op=%"PRIu32", val1=%"PRIu32", timeout_va=0x%"PRIx32", futex2=%p, val3=%"PRIu32"\n",
                        futex1, op, val1, timeout_va, futex2, val3);
                if (futex1)
                    fprintf(process->debug, "      *futex1 = %"PRIu32"\n", *futex1);
                if (futex2)
                    fprintf(process->debug, "      *futex2 = %"PRIu32"\n", *futex2);
            }
#endif
            int result = syscall(SYS_futex, futex1, op, val1, timespec, futex2, val3);
            if (-1==result) result = -errno;
            sys_return(result);
            syscall_leave("d");
            break;
        }

        case 243: { /*0xf3, set_thread_area*/
            syscall_enter("set_thread_area", "P", sizeof(user_desc_32), print_user_desc_32);
            user_desc_32 ud;
            size_t nread = process->get_memory()->read(&ud, arg(0), sizeof ud);
            ROSE_ASSERT(nread==sizeof ud);
            if (ud.entry_number==(unsigned)-1) {
                for (ud.entry_number=0x33>>3; ud.entry_number<process->n_gdt; ud.entry_number++) {
                    if (!process->gdt[ud.entry_number].useable) break;
                }
                ROSE_ASSERT(ud.entry_number<8192);
                if (tracing(TRACE_SYSCALL))
                    fprintf(tracing(TRACE_SYSCALL), "[entry #%d] ", (int)ud.entry_number);
            }
            process->gdt[ud.entry_number] = ud;
            size_t nwritten = process->get_memory()->write(&ud, arg(0), sizeof ud);
            ROSE_ASSERT(nwritten==sizeof ud);
            sys_return(0);
            /* Reload all the segreg shadow values from the (modified) descriptor table */
            for (size_t i=0; i<6; i++)
                policy.writeSegreg((X86SegmentRegister)i, policy.readSegreg((X86SegmentRegister)i));
            syscall_leave("d");
            break;
        }

        case 1: /*exit*/
            syscall_enter("exit", "d");
            if (tracing(TRACE_SYSCALL))
                fprintf(tracing(TRACE_SYSCALL), "falls through to exit_group...\n");
            /* fall through */

        case 252: { /*0xfc, exit_group*/
            syscall_enter("exit_group", "d");
            if (clear_child_tid) {
                /* From the set_tid_address(2) man page:
                 *      When clear_child_tid is set, and the process exits, and the process was sharing memory with other
                 *      processes or threads, then 0 is written at this address, and a futex(child_tidptr, FUTEX_WAKE, 1, NULL,
                 *      NULL, 0) call is done. (That is, wake a single process waiting on this futex.) Errors are ignored. */
                if (tracing(TRACE_SYSCALL))
                    fprintf(tracing(TRACE_SYSCALL), "[FIXME: skiping clear_child_tid]");
                //FIXME [RPM 2010-11-13]
            }

            if (tracing(TRACE_SYSCALL)) fputs("(throwing...)\n", tracing(TRACE_SYSCALL));
            int status=arg(0);
            throw Exit(__W_EXITCODE(status, 0));
        }

        case 258: { /*0x102, set_tid_address*/
            syscall_enter("set_tid_address", "p");
            clear_child_tid = arg(0);
            sys_return(getpid());
            syscall_leave("d");
            break;
        }

        case 264: { /* 0x108, clock_settime */
            syscall_enter("clock_settime", "eP", clock_names, sizeof(timespec_32), print_timespec_32);
            do {
                timespec_32 guest_ts;
                if (sizeof(guest_ts)!=process->get_memory()->read(&guest_ts, arg(1), sizeof guest_ts)) {
                    sys_return(-EFAULT);
                    break;
                }

                static timespec host_ts;
                host_ts.tv_sec = guest_ts.tv_sec;
                host_ts.tv_nsec = guest_ts.tv_nsec;
                int result = syscall(SYS_clock_settime, arg(0), &host_ts);
                sys_return(-1==result?-errno:result);
            } while (0);
            syscall_leave("d");
            break;
        }

        case 265: { /* 0x109, clock_gettime */
            syscall_enter("clock_gettime", "ep", clock_names);
            do {
                static timespec host_ts;
                int result = syscall(SYS_clock_gettime, arg(0), &host_ts);
                if (-1==result) {
                    sys_return(-errno);
                    break;
                }

                timespec_32 guest_ts;
                guest_ts.tv_sec = host_ts.tv_sec;
                guest_ts.tv_nsec = host_ts.tv_nsec;
                if (sizeof(guest_ts)!=process->get_memory()->write(&guest_ts, arg(1), sizeof guest_ts)) {
                    sys_return(-EFAULT);
                    break;
                }

                sys_return(result);
            } while (0);
            syscall_leave("d-P", sizeof(timespec_32), print_timespec_32);
            break;
        }

        case 266: { /* 0x10a, clock_getres */
            syscall_enter("clock_getres", "ep", clock_names);
            do {
                static timespec host_ts;
                timespec *host_tsp = arg(1) ? &host_ts : NULL;
                int result = syscall(SYS_clock_getres, arg(0), host_tsp);
                if (-1==result) {
                    sys_return(-errno);
                    break;
                }

                if (arg(1)) {
                    timespec_32 guest_ts;
                    guest_ts.tv_sec = host_ts.tv_sec;
                    guest_ts.tv_nsec = host_ts.tv_nsec;
                    if (sizeof(guest_ts)!=process->get_memory()->write(&guest_ts, arg(1), sizeof guest_ts)) {
                        sys_return(-EFAULT);
                        break;
                    }
                }

                sys_return(result);
            } while (0);
            syscall_leave("d-P", sizeof(timespec_32), print_timespec_32);
            break;
        }

        case 268: { /* 0x10c, statfs64 */
            syscall_enter("statfs64", "sdp");
            ROSE_ASSERT(arg(1)==sizeof(statfs64_32));
            do {
                bool error;
                std::string path = read_string(arg(0), 0, &error);
                if (error) {
                    sys_return(-EFAULT);
                    break;
                }

                static statfs_64_native host_statfs;
#ifdef SYS_statfs64 /* host is 32-bit machine */
                int result = syscall(SYS_statfs64, path.c_str(), sizeof host_statfs, &host_statfs);
#else           /* host is 64-bit machine */
                int result = syscall(SYS_statfs, path.c_str(), &host_statfs);
#endif
                if (-1==result) {
                    sys_return(-errno);
                    break;
                }

                statfs64_32 guest_statfs;
                guest_statfs.f_type = host_statfs.f_type;
                guest_statfs.f_bsize = host_statfs.f_bsize;
                guest_statfs.f_blocks = host_statfs.f_blocks;
                guest_statfs.f_bfree = host_statfs.f_bfree;
                guest_statfs.f_bavail = host_statfs.f_bavail;
                guest_statfs.f_files = host_statfs.f_files;
                guest_statfs.f_ffree = host_statfs.f_ffree;
                guest_statfs.f_fsid[0] = host_statfs.f_fsid[0];
                guest_statfs.f_fsid[1] = host_statfs.f_fsid[1];
                guest_statfs.f_namelen = host_statfs.f_namelen;
                guest_statfs.f_frsize = host_statfs.f_frsize;
                guest_statfs.f_flags = host_statfs.f_flags;
                guest_statfs.f_spare[0] = host_statfs.f_spare[0];
                guest_statfs.f_spare[1] = host_statfs.f_spare[1];
                guest_statfs.f_spare[2] = host_statfs.f_spare[2];
                guest_statfs.f_spare[3] = host_statfs.f_spare[3];
                if (sizeof(guest_statfs)!=process->get_memory()->write(&guest_statfs, arg(2), sizeof guest_statfs)) {
                    sys_return(-EFAULT);
                    break;
                }

                sys_return(result);
            } while (0);
            syscall_leave("d--P", sizeof(statfs64_32), print_statfs64_32);
            break;
        }

        case 270: { /*0x10e tgkill*/
            syscall_enter("tgkill", "ddf", signal_names);
            uint32_t /*tgid=arg(0), pid=arg(1),*/ sig=arg(2);
            // TODO: Actually check thread group and kill properly
            if (tracing(TRACE_SYSCALL)) fputs("(throwing...)\n", tracing(TRACE_SYSCALL));
            throw Exit(__W_EXITCODE(0, sig));
            break;

        }

        case 271: { /* 0x10f, utimes */
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
            syscall_enter("utimes", "s");
            do {
                bool error;
                std::string filename = read_string(arg(0), 0, &error);
                if (error) {
                    sys_return(-EFAULT);
                    break;
                }

                //Check to see if times is NULL
                uint8_t byte;
                size_t nread = process->get_memory()->read(&byte, arg(1), 1);
                ROSE_ASSERT(1==nread); /*or we've read past the end of the mapped memory*/

                int result;
                if( byte ) {

                    size_t size_timeval_sample = sizeof(timeval_32)*2;

                    timeval_32 ubuf[1];

                    size_t nread = process->get_memory()->read(&ubuf, arg(1), size_timeval_sample);


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

                sys_return(result);
            } while (0);
            syscall_leave("d");
            break;

        }

	case 306: { /* 0x132, fchmodat */
            syscall_enter("fchmodat", "dsdd");
            do {
                int dirfd = arg(0);
                uint32_t path = arg(1);
                bool error;
                std::string sys_path = read_string(path, 0, &error);
                if (error) {
                    sys_return(-EFAULT);
                    break;
                }
                mode_t mode = arg(2);
                int flags = arg(3);

                int result = syscall( 306, dirfd, (long) sys_path.c_str(), mode, flags);
                if (result == -1) result = -errno;
                sys_return(result);
            } while (0);
            syscall_leave("d");
            break;
	}

        case 311: { /*0x137, set_robust_list*/
            syscall_enter("set_robust_list", "Pd", sizeof(robust_list_head_32), print_robust_list_head_32);
            do {
                uint32_t head_va=arg(0), len=arg(1);
                if (len!=sizeof(robust_list_head_32)) {
                    sys_return(-EINVAL);
                    break;
                }

                robust_list_head_32 guest_head;
                if (sizeof(guest_head)!=process->get_memory()->read(&guest_head, head_va, sizeof(guest_head))) {
                    sys_return(-EFAULT);
                    break;
                }

                /* The robust list is maintained in user space and accessed by the kernel only when we a thread dies. Since the
                 * simulator handles thread death, we don't need to tell the kernel about the specimen's list until later. In
                 * fact, we can't tell the kernel because that would cause our own list (set by libc) to be removed from the
                 * kernel. */
                robust_list_head_va = head_va;
                sys_return(0);
            } while (0);
            syscall_leave("d");
            break;
        }

        default: {
            fprintf(stderr, "syscall_%u(", callno);
            for (int i=0; i<6; i++)
                fprintf(stderr, "%s0x%08"PRIx32, i?", ":"", arg(i));
            fprintf(stderr, ") is not implemented yet\n\n");
            process->dump_core(SIGSYS);
            abort();
        }
    }
    ROSE_ASSERT( this != NULL  );
}




int
main(int argc, char *argv[], char *envp[])
{
    RSIM_Simulator simulator(argc, argv, envp);
    simulator.activate();
    simulator.main_loop();
    return 0;
}

#else
int main(int, char *argv[])
{
    std::cerr <<argv[0] <<": not supported on this platform" <<std::endl;
    return 0;
}

#endif /* ROSE_ENABLE_SIMULATOR */
