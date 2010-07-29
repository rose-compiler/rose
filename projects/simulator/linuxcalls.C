#include "rose.h"
#include "simulate.h"

#ifdef ROSE_ENABLE_SIMULATOR /*protects this whole file*/

#include "x86InstructionSemantics.h"
#include <stdint.h>
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>
#include <boost/static_assert.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <linux/types.h>
#include <linux/dirent.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/mman.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <asm/ldt.h>
#include <linux/unistd.h>

#undef DEBUG

extern void simulate_signal(int sig);

using namespace std;

static void do_mmap(const char *fname, LinuxMachineState& ms, uint32_t start, uint32_t length, int prot, int flags, int fd, uint32_t offset) {
#ifdef DO_SIMULATION_TRACING
    fprintf(stderr,
            "  %s(start=0x%08"PRIx32", size=0x%08"PRIx32", prot=%04"PRIo32", flags=0x%"PRIx32
            ", fd=%d, offset=0x%08"PRIx32")\n", 
            fname, start, length, prot, flags, fd, offset);
#endif
    length = (length + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    size_t lengthInPages = length / PAGE_SIZE;
    // if (start - ms.brk < 0x10000000UL && !(flags & MAP_FIXED)) start = 0;
    bool found_addr = start ? true : false;
    if (!start) {
        start = 0x40000000UL;
        for (unsigned int i = 0; i + lengthInPages - 1 < ms.memory.pages.size(); ++i) {
            bool okMapping = true;
            for (size_t j = 0; j < lengthInPages; ++j) {
                if (ms.memory.pages[(start / PAGE_SIZE) + i + j].in_use) {
                    okMapping = false;
                    break;
                }
            }
            if (okMapping) {
                found_addr = true;
                start = start + i * PAGE_SIZE;
                break;
            }
        }
#ifdef DO_SIMULATION_TRACING
        if (found_addr) {
            fprintf(stderr, "  start = 0x%08"PRIx32"\n", start);
        } else {
            fprintf(stderr, "Failed to find address for mapping\n");
        }
#endif
    }

  if (!found_addr) {
    ms.gprs[x86_gpr_ax] = (uint32_t)(-ENOMEM);
  } else {
    if (flags & MAP_ANONYMOUS) {
      uint8_t* data = (uint8_t*)mmap(NULL, length, prot, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
      if (data == MAP_FAILED) {
        ms.gprs[x86_gpr_ax] = (uint32_t)(-errno);
      } else {
        for (uint32_t i = 0; i < length; i += PAGE_SIZE) {
          Page& p = ms.memory.findPage(start + i);
          p.clear();
          p = Page((prot & PROT_READ), (prot & PROT_WRITE), (prot & PROT_EXEC), data + i);
        }
        ms.gprs[x86_gpr_ax] = start;
      }
    } else {
      uint8_t* data = (uint8_t*)mmap(NULL, length, prot, flags & ~MAP_FIXED, fd, offset);
      if (data == MAP_FAILED) {
        ms.gprs[x86_gpr_ax] = (uint32_t)(-errno);
      } else {
        for (uint32_t i = 0; i < length; i += PAGE_SIZE) {
          Page& p = ms.memory.findPage(start + i);
          p.clear();
          p = Page((prot & PROT_READ), (prot & PROT_WRITE), (prot & PROT_EXEC), data + i);
        }
        ms.gprs[x86_gpr_ax] = start;
      }
    }
  }
}

static int simulate_ioctl(LinuxMachineState& ms, int fd, uint32_t cmd, uint32_t arg_ptr) {
#ifdef DO_SIMULATION_TRACING
    fprintf(stderr, "  ioctl(fd=%d, cmd=%"PRIu32", arg_va=0x%08"PRIx32")\n", fd, cmd, arg_ptr);
#endif
  int result = -ENOSYS;
  switch (cmd) {
    case TCGETS: { // 0x5401 TCGETS tcgetattr
      uint32_t termios_ptr = arg_ptr; // st.memory.read_dword(arg_ptr);
#ifdef DEBUG
      fprintf(stdout, "termios_ptr = 0x%08X\n", termios_ptr);
#endif
      struct termios ti;
      result = tcgetattr(fd, &ti);
      if (result == -1) {
	result = -errno;
      } else {
	ms.memory.writeMultiple((uint8_t*)&ti, 39, termios_ptr); // The Linux kernel and glibc have different definitions for termios, with very different sizes (39 bytes vs 60)
      }
      break;
    }

    case TIOCGPGRP: { // 0x540F TIOCGPGRP tcgetpgrp
      uint32_t pgrp_ptr = arg_ptr;
      pid_t pgrp = tcgetpgrp(fd);
      if (pgrp == -1) {
	result = -errno;
      } else {
	ms.memory.write<4>(pgrp_ptr, pgrp);
	result = 0;
      }
      break;
    }

    case TIOCSPGRP: { // 0x5410 TIOCSPGRP tcsetpgrp
      pid_t pgid_id = ms.memory.read<4>(arg_ptr);
      // fprintf(stdout, "TIOCSPGRP %lu (my pgid = %lu)\n", (unsigned long)pgid_id, (unsigned long)getpgid(getpid()));
      result = tcsetpgrp(fd, pgid_id);
      if (result == -1) {
	result = -errno;
      }
      break;
    }

    case TIOCGWINSZ: { // 0x5413 TIOCGWINSZ
      uint32_t win_ptr = arg_ptr;
      struct winsize sys_win;
      result = ioctl(fd, TIOCGWINSZ, &sys_win);
      if (result == -1) {
	result = (uint32_t)(-errno);
      } else {
	ms.memory.writeMultiple((const uint8_t*)&sys_win, sizeof(struct winsize), win_ptr);
      }
      break;
    }

    default: {
      fprintf(stderr, "Unhandled ioctl\n");
      abort();
      break; // Return ENOSYS
    }
  }
  // fprintf(stdout, "%d\n", result);
  return result;
}

static void copyInStat(MachineState& ms, const struct stat64& st, uint32_t statPtr) {
  ms.writeMemory<16>(x86_segreg_ds, statPtr + 0, st.st_dev);
  ms.writeMemory<32>(x86_segreg_ds, statPtr + 12, st.st_ino);
  ms.writeMemory<32>(x86_segreg_ds, statPtr + 16, st.st_mode);
  ms.writeMemory<32>(x86_segreg_ds, statPtr + 20, st.st_nlink);
  ms.writeMemory<32>(x86_segreg_ds, statPtr + 24, st.st_uid);
  ms.writeMemory<32>(x86_segreg_ds, statPtr + 28, st.st_gid);
  ms.writeMemory<16>(x86_segreg_ds, statPtr + 32, st.st_rdev);
  ms.writeMemory<64>(x86_segreg_ds, statPtr + 44, st.st_size);
  ms.writeMemory<32>(x86_segreg_ds, statPtr + 52, st.st_blksize);
  ms.writeMemory<32>(x86_segreg_ds, statPtr + 56, st.st_blocks);
#ifdef FIXME
  ms.writeMemory<32>(x86_segreg_ds, statPtr + 64, st.st_atime);
#else
  ms.writeMemory<32>(x86_segreg_ds, statPtr + 64, 1279897465ul); /*use same time always for consistency when debugging*/
#endif
  ms.writeMemory<32>(x86_segreg_ds, statPtr + 72, st.st_mtime);
  ms.writeMemory<32>(x86_segreg_ds, statPtr + 80, st.st_ctime);
  ms.writeMemory<64>(x86_segreg_ds, statPtr + 88, st.st_ino);
}

static string sigsetToString(sigset_t* s) {
  string result = "{";
  bool first = true;
  for (int i = 0; i < _NSIG; ++i) {
    if (sigismember(s, i)) {
      if (!first) result += ", ";
      first = false;
      result += strsignal(i);
    }
  }
  result += "}";
  return result;
}

void linuxSyscall(LinuxMachineState& ms) {
#ifdef DEBUG
  fprintf(stdout, "%lu System call %u\n", (unsigned long)getpid(), ms.gprs[x86_gpr_ax]); ms.dumpRegs();
#endif

  // static int fileCount = 0;

  switch (ms.gprs[x86_gpr_ax]) {
    case 2: { // fork
      int result = fork();
      if (result == -1) result = -errno;
      ms.gprs[x86_gpr_ax] = (uint32_t)result;
      break;
    }

    case 3: { // read
      int fd = (int)ms.gprs[x86_gpr_bx];
      uint32_t buf = ms.gprs[x86_gpr_cx];
      uint32_t count = ms.gprs[x86_gpr_dx];
#ifdef DO_SIMULATION_TRACING
      fprintf(stderr, "  read(fd=%d, buf=0x%08"PRIx32", size=%"PRIu32")\n", fd, buf, count);
#endif
      char sysbuf[count];
      ssize_t result = read(fd, sysbuf, count);
      ms.gprs[x86_gpr_ax] = result;
      if (result == -1) {
	ms.gprs[x86_gpr_ax] = (uint32_t)(-errno);
      } else {
	ms.memory.writeMultiple((const uint8_t*)sysbuf, result, buf);
      }
      break;
    }

    case 4: { // write
      int fd = (int)ms.gprs[x86_gpr_bx];
      uint32_t buf = ms.gprs[x86_gpr_cx];
      uint32_t count = ms.gprs[x86_gpr_dx];
#ifdef DO_SIMULATION_TRACING
      fprintf(stderr, "  write(fd=%d, buf=0x%08"PRIx32", size=%"PRIu32")\n", fd, buf, count);
#endif
      char sysbuf[count];
      ms.memory.readMultiple((uint8_t*)sysbuf, count, buf);
      ssize_t result = write(fd, sysbuf, count);
      if (result == -1) result = -errno;
      ms.gprs[x86_gpr_ax] = (uint32_t)result;
      break;
    }

    case 5: { // open
      uint32_t filename = ms.gprs[x86_gpr_bx];
      string fn = ms.memory.readString(filename);
      uint32_t flags = ms.gprs[x86_gpr_cx];
      uint32_t mode = 0;
      if (flags & O_CREAT) {
	mode = ms.gprs[x86_gpr_dx];
      }
#ifdef DO_SIMULATION_TRACING
      fprintf(stderr, "  open(name=0x%08"PRIx32" \"%s\", flags=0x%"PRIx32", mode=%04"PRIo32")\n", 
              filename, fn.c_str(), flags, mode);
#endif
      int fd = open(&fn[0], flags, mode);
      if (fd == -1) {
	  ms.gprs[x86_gpr_ax] = (uint32_t)(-errno);
      } else {
	  ms.gprs[x86_gpr_ax] = fd;
      }
      break;
    }

    case 6: { // close
      int fd = (int)(ms.gprs[x86_gpr_bx]);
#ifdef DO_SIMULATION_TRACING
      fprintf(stderr, "  close(%d)\n", fd);
#endif
      if (fd == 2) { // Don't allow closing stderr since we use it for debugging
	ms.gprs[x86_gpr_ax] = (uint32_t)(-EPERM);
      } else {
	int result = close(fd);
	ms.gprs[x86_gpr_ax] = result;
	if (result == -1) ms.gprs[x86_gpr_ax] = (uint32_t)(-errno);
      }
      break;
    }

    case 7: { // waitpid
      pid_t pid = ms.gprs[x86_gpr_bx];
      uint32_t status = ms.gprs[x86_gpr_cx];
      int options = ms.gprs[x86_gpr_dx];
      int sys_status;
      int result = waitpid(pid, &sys_status, options);
      if (result == -1) {
	result = -errno;
      } else {
	if (status) ms.memory.write<4>(status, sys_status);
      }
      ms.gprs[x86_gpr_ax] = (uint32_t)result;
      break;
    }

    case 11: { // execve
      string filename = ms.memory.readString(ms.gprs[x86_gpr_bx]);
      vector<string > argv = ms.memory.readStringVector(ms.gprs[x86_gpr_cx]);
      vector<string > envp = ms.memory.readStringVector(ms.gprs[x86_gpr_dx]);
      vector<char*> sys_argv;
      for (unsigned int i = 0; i < argv.size(); ++i) sys_argv.push_back(&argv[i][0]);
      sys_argv.push_back(NULL);
      vector<char*> sys_envp;
      for (unsigned int i = 0; i < envp.size(); ++i) sys_envp.push_back(&envp[i][0]);
      sys_envp.push_back(NULL);
      int result;
      if (string(&filename[0]) == "/usr/bin/man") {
	result = -EPERM;
      } else {
	result = execve(&filename[0], &sys_argv[0], &sys_envp[0]);
	if (result == -1) result = -errno;
      }
      ms.gprs[x86_gpr_ax] = (uint32_t)result;
      break;
    }

    case 13: { // time
      assert (sizeof(time_t) == 4);
      uint32_t t = ms.gprs[x86_gpr_bx];
      time_t result = time(NULL);
      if (t) ms.memory.write<4>(t, result);
      ms.gprs[x86_gpr_ax] = result;
      break;
    }

    case 20: { // getpid
      pid_t id = getpid();
      ms.gprs[x86_gpr_ax] = id;
      break;
    }

    case 24: { // getuid
      uid_t id = getuid();
      ms.gprs[x86_gpr_ax] = id;
      break;
    }

    case 33: { // access
      uint32_t pathname = ms.gprs[x86_gpr_bx];
      int mode = (int)ms.gprs[x86_gpr_cx];
      string sys_pathname = ms.memory.readString(pathname);
#ifdef DO_SIMULATION_TRACING
      fprintf(stderr, "  access(name=0x%08"PRIx32" \"%s\", mode=%04o)\n", pathname, sys_pathname.c_str(), mode);
#endif
      int result = access(&sys_pathname[0], mode);
      if (result == -1) result = -errno;
      ms.gprs[x86_gpr_ax] = (uint32_t)result;
      break;
    }

    case 37: { // kill
      pid_t pid = ms.gprs[x86_gpr_bx];
      int sig = ms.gprs[x86_gpr_cx];
#ifdef DEBUG
      fprintf(stdout, "Kill process %ld with signal %d\n", (long)pid, sig);
#endif
      int result = kill(pid, sig);
      if (result == -1) result = -errno;
      ms.gprs[x86_gpr_ax] = (uint32_t)result;
      break;
    }

    case 41: { // dup
      int fd = ms.gprs[x86_gpr_bx];
      int result = dup(fd);
      if (result == -1) result = -errno;
      ms.gprs[x86_gpr_ax] = (uint32_t)result;
      break;
    }

    case 45: { // brk
      uint32_t end_data_segment = ms.gprs[x86_gpr_bx];
#ifdef DO_SIMULATION_TRACING
      fprintf(stderr, "  brk(0x%08x) -- old brk is 0x%08x\n", end_data_segment, ms.brk);
#endif
      if (end_data_segment == 0) {
	ms.gprs[x86_gpr_ax] = ms.brk;
      } else if (end_data_segment >= 0xB0000000UL) {
	ms.gprs[x86_gpr_ax] = (uint32_t)(-ENOMEM);
      } else {
        if (end_data_segment >= ms.brk) {
          for (uint32_t i = ms.brk; i < end_data_segment; i += PAGE_SIZE) {
            ms.memory.mapZeroPageIfNeeded(i);
          }
        } else {
          for (uint32_t i = end_data_segment; i < ms.brk; i += PAGE_SIZE) {
            ms.memory.findPage(i).clear();
          }
        }
        ms.gprs[x86_gpr_ax] = end_data_segment;
      }
      break;
    }

    case 47: { // getgid
      uid_t id = getgid();
      ms.gprs[x86_gpr_ax] = id;
      break;
    }

    case 49: { // geteuid
      uid_t id = geteuid();
      ms.gprs[x86_gpr_ax] = id;
      break;
    }

    case 50: { // getegid
      uid_t id = getegid();
      ms.gprs[x86_gpr_ax] = id;
      break;
    }

    case 54: { // ioctl
      int fd = (int)ms.gprs[x86_gpr_bx];
      uint32_t cmd = ms.gprs[x86_gpr_cx];
      uint32_t arg_ptr = ms.gprs[x86_gpr_dx];
      int result = simulate_ioctl(ms, fd, cmd, arg_ptr);
      ms.gprs[x86_gpr_ax] = (uint32_t)result;
      break;
    }

    case 57: { // setpgid
      pid_t pid = (pid_t)ms.gprs[x86_gpr_bx];
      pid_t pgid = (pid_t)ms.gprs[x86_gpr_cx];
      int result = setpgid(pid, pgid);
      if (result == -1) result = -errno;
      ms.gprs[x86_gpr_ax] = (uint32_t)result;
      break;
    }

    case 63: { // dup2
      int fd = ms.gprs[x86_gpr_bx];
      int fd2 = ms.gprs[x86_gpr_cx];
      int result = dup2(fd, fd2);
      if (result == -1) result = -errno;
      ms.gprs[x86_gpr_ax] = (uint32_t)result;
      break;
    }

    case 64: { // getppid
      pid_t id = getppid();
      ms.gprs[x86_gpr_ax] = id;
      break;
    }

    case 65: { // getpgrp
      pid_t id = getpgrp();
      ms.gprs[x86_gpr_ax] = id;
      break;
    }

    case 78: { // gettimeofday
      uint32_t tp = ms.gprs[x86_gpr_bx];
      struct timeval sys_t;
      int result = gettimeofday(&sys_t, NULL);
      if (result == -1) {
	ms.gprs[x86_gpr_ax] = (uint32_t)(-errno);
      } else {
	ms.gprs[x86_gpr_ax] = result;
        ms.writeMemory<32>(x86_segreg_ds, tp, sys_t.tv_sec);
        ms.writeMemory<32>(x86_segreg_ds, tp + 4, sys_t.tv_usec);
      }
      break;
    }

    case 83: { // symlink
      uint32_t path1 = ms.gprs[x86_gpr_bx];
      uint32_t path2 = ms.gprs[x86_gpr_cx];
      string sys_path1 = ms.memory.readString(path1);
      string sys_path2 = ms.memory.readString(path2);
      int result = symlink(&sys_path1[0], &sys_path2[0]);
      if (result == -1) result = -errno;
      ms.gprs[x86_gpr_ax] = (uint32_t)result;
      break;
    }

    case 85: { // readlink
      uint32_t path = ms.gprs[x86_gpr_bx];
      uint32_t buf = ms.gprs[x86_gpr_cx];
      size_t bufsiz = ms.gprs[x86_gpr_dx];
      string sys_path = ms.memory.readString(path);
      char sys_buf[bufsiz];
      int result = readlink(&sys_path[0], sys_buf, bufsiz);
      if (result == -1) {
	ms.gprs[x86_gpr_ax] = (uint32_t)(-errno);
      } else {
	ms.memory.writeMultiple((const uint8_t*)sys_buf, result, buf);
	ms.gprs[x86_gpr_ax] = result;
      }
      break;
    }

    case 90: { // old_mmap
      uint32_t start = ms.memory.read<4>(ms.gprs[x86_gpr_bx] + 0);
      uint32_t length = ms.memory.read<4>(ms.gprs[x86_gpr_bx] + 4);
      uint32_t prot = ms.memory.read<4>(ms.gprs[x86_gpr_bx] + 8);
      uint32_t flags = ms.memory.read<4>(ms.gprs[x86_gpr_bx] + 12);
      uint32_t fd = ms.memory.read<4>(ms.gprs[x86_gpr_bx] + 16);
      uint32_t offset = ms.memory.read<4>(ms.gprs[x86_gpr_bx] + 20);
      do_mmap("mmap", ms, start, length, prot, flags, fd, offset);
      break;
    }

    case 91: { // munmap
      uint32_t start = ms.gprs[x86_gpr_bx];
#ifdef DO_SIMULATION_TRACING
      uint32_t length = ms.gprs[x86_gpr_cx];
      fprintf(stderr, "  munmap(va=0x%08"PRIx32", size=0x%08"PRIx32")\n", start, length);
#endif
      Page& p = ms.memory.findPage(start);
      int result = munmap(p.real_base, PAGE_SIZE);
      if (result == -1) result = -errno;
      ms.gprs[x86_gpr_ax] = (uint32_t)result;
      break;
    }

    case 102: { // socketcall
      int call = ms.gprs[x86_gpr_bx];
      uint32_t args = ms.gprs[x86_gpr_cx];
      fprintf(stderr, "socketcall(%d, 0x%08X)\n", call, args);
      ms.gprs[x86_gpr_ax] = (uint32_t)(-ENOSYS);
      break;
    }

    case 114: { // wait4
      pid_t pid = ms.gprs[x86_gpr_bx];
      uint32_t status_ptr = ms.gprs[x86_gpr_cx];
      int options = (int)ms.gprs[x86_gpr_dx];
      uint32_t rusage_ptr = ms.gprs[x86_gpr_si];
      int status = (int)ms.memory.read<4>(status_ptr);
      struct rusage sys_rusage;
      pid_t result = wait4(pid, &status, options, &sys_rusage);
      if (result == (pid_t)(-1)) {
	ms.gprs[x86_gpr_ax] = (uint32_t)(-errno);
      } else {
	ms.gprs[x86_gpr_ax] = (uint32_t)result;
	if (status_ptr != 0) ms.memory.writeMultiple((const uint8_t*)&status, 4, status_ptr);
	if (rusage_ptr != 0) ms.memory.writeMultiple((const uint8_t*)&sys_rusage, sizeof(struct rusage), rusage_ptr);
      }
      break;
    }

    case 122: { // uname
#ifdef DO_SIMULATION_TRACING
      fprintf(stderr, "  uname(0x%"PRIx32")\n", ms.gprs[x86_gpr_bx]);
#endif
      ms.memory.writeMultiple( // The next 6 strings are exactly 65 characters each
          (const uint8_t*)
          "Linux\0                                                           " // sysname
          "mymachine.example.com\0                                           " // nodename
          "2.6.9\0                                                           " // release
          "#1 SMP Wed Jun 18 12:35:02 EDT 2008\0                             " // version
          "i386\0                                                            " // machine
          "example.com\0                                                     " // domainname
          ,
          6 * 65,
          ms.gprs[x86_gpr_bx]);
      ms.gprs[x86_gpr_ax] = 0;
      break;
    }

    case 125: { // mprotect
      uint32_t addr = ms.gprs[x86_gpr_bx];
      uint32_t len = ms.gprs[x86_gpr_cx];
      uint32_t perms = ms.gprs[x86_gpr_dx];
#ifdef DO_SIMULATION_TRACING
      fprintf(stderr, "  mprotect(va=0x%08"PRIx32", size=0x%08"PRIx32", perm=%04"PRIo32")\n", addr, len, perms);
#endif
      for (uint32_t i = 0; i < len; i += PAGE_SIZE) {
        Page& p = ms.memory.findPage(addr + i);
        assert (p.real_base);
        p.allow_read = (perms & PROT_READ);
        p.allow_write = (perms & PROT_WRITE);
        p.allow_execute = (perms & PROT_EXEC);
        mprotect(p.real_base, PAGE_SIZE, perms);
      }
      ms.gprs[x86_gpr_ax] = 0;
      break;
    }

    case 140: { // _llseek
      int fd = ms.gprs[x86_gpr_bx];
      uint32_t offset_high = ms.gprs[x86_gpr_cx];
      uint32_t offset_low = ms.gprs[x86_gpr_dx];
      uint32_t result = ms.gprs[x86_gpr_si];
      unsigned int whence = ms.gprs[x86_gpr_di];
      off_t r = lseek(fd, (uint64_t)offset_high << 32 | offset_low, whence);
      if (r == -1) {
	ms.gprs[x86_gpr_ax] = (uint32_t)(-errno);
      } else {
	if (result) ms.memory.write<8>(result, r);
	ms.gprs[x86_gpr_ax] = 0;
      }
      break;
    }

    case 142: { // _newselect
      int n = ms.gprs[x86_gpr_bx];
      uint32_t inp = ms.gprs[x86_gpr_cx];
      uint32_t outp = ms.gprs[x86_gpr_dx];
      uint32_t exp = ms.gprs[x86_gpr_si];
      uint32_t tvp = ms.gprs[x86_gpr_di];
      fd_set sys_readset, sys_writeset, sys_exceptset;
      fd_set* arg_readset = inp ? &sys_readset : NULL;
      if (inp) ms.memory.readMultiple((uint8_t*)arg_readset, sizeof(fd_set), inp);
      fd_set* arg_writeset = outp ? &sys_writeset : NULL;
      if (outp) ms.memory.readMultiple((uint8_t*)arg_readset, sizeof(fd_set), outp);
      fd_set* arg_exceptset = exp ? &sys_exceptset : NULL;
      if (exp) ms.memory.readMultiple((uint8_t*)arg_readset, sizeof(fd_set), exp);
      struct timeval sys_tv;
      struct timeval* arg_tv = tvp ? &sys_tv : NULL;
      if (tvp) ms.memory.readMultiple((uint8_t*)arg_tv, sizeof(struct timeval), tvp);
      int result = select(n, arg_readset, arg_writeset, arg_exceptset, arg_tv);
      if (result == -1) {
	ms.gprs[x86_gpr_ax] = (uint32_t)(-errno);
      } else {
	if (inp) ms.memory.writeMultiple((const uint8_t*)arg_readset, sizeof(fd_set), inp);
	if (outp) ms.memory.writeMultiple((const uint8_t*)arg_writeset, sizeof(fd_set), outp);
	if (exp) ms.memory.writeMultiple((const uint8_t*)arg_exceptset, sizeof(fd_set), exp);
	if (tvp) ms.memory.writeMultiple((const uint8_t*)arg_tv, sizeof(struct timeval), tvp);
	ms.gprs[x86_gpr_ax] = result;
      }
      break;
    }

    case 146: { // writev
      int fd = (int)ms.gprs[x86_gpr_bx];
      uint32_t iov = ms.gprs[x86_gpr_cx];
      int iovcnt = (int)ms.gprs[x86_gpr_dx];
#ifdef DO_SIMULATION_TRACING
      fprintf(stderr, "  writev(fd=%d, iov=0x%08"PRIx32", nentries=%d\n", fd, iov, iovcnt);
#endif
      uint32_t user_result = 0;
      for (int i = 0; i < iovcnt; ++i) {
	uint32_t buf = ms.memory.read<4>(iov + 8 * i);
	uint32_t len = ms.memory.read<4>(iov + 8 * i + 4);
#ifdef DO_SIMULATION_TRACING
        fprintf(stderr, "    #%d: va=0x%08"PRIx32", size=0x%08"PRIx32"\n", i, buf, len);
#endif
	char my_buf[len];
	ms.memory.readMultiple((uint8_t*)my_buf, len, buf);
	int result = write(fd, my_buf, len);
	if (result == -1) {
	  user_result = -errno;
	  break;
	} else if ((uint32_t)result < len) {
	  user_result += result;
	  break;
	} else {
	  user_result += result;
	}
      }
      ms.gprs[x86_gpr_ax] = (uint32_t)user_result;
      break;
    }

    case 149: { // _sysctl
      ms.gprs[x86_gpr_ax] = (uint32_t)(-ENOSYS);
      break;
    }

    case 163: { // mremap
      uint32_t old_address = ms.gprs[x86_gpr_bx];
      size_t old_size = ms.gprs[x86_gpr_cx];
      size_t new_size = ms.gprs[x86_gpr_dx];
      unsigned long flags = ms.gprs[x86_gpr_si];
      fprintf(stderr, "mremap(0x%08X, 0x%08zX, 0x%08zX, 0x%08lX)\n", old_address, old_size, new_size, flags);
#if 0
      Segment* seg = st.memory.find_segment_ending_at(old_address + old_size);
      if (!seg) {
	ms.gprs[x86_gpr_ax] = (uint32_t)(-EINVAL);
      } else {
	seg->expand_up(new_size);
	ms.gprs[x86_gpr_ax] = old_address;
      }
#else
      ms.gprs[x86_gpr_ax] = (uint32_t)(-ENOSYS);
#endif
      break;
    }

    case 174: { // rt_sigaction
      int signum = (int)ms.gprs[x86_gpr_bx];
      int result = -ENOSYS;
      uint32_t act = ms.gprs[x86_gpr_cx];
      uint32_t oldact = ms.gprs[x86_gpr_dx];
      fprintf(stdout, "rt_sigaction('%s', 0x%08X, 0x%08X);\n", strsignal(signum), act, oldact);
      struct sigaction sys_act, sys_oldact;
      if (act != 0) {
        sys_act.sa_flags = ms.readMemory<32>(x86_segreg_ds, act + 132);
	if (sys_act.sa_flags & SA_SIGINFO) {
	  ms.gprs[x86_gpr_ax] = (uint32_t)(-ENOSYS);
	  goto sigaction_done;
	}
	if (sys_act.sa_flags & SA_ONSTACK) {
	  ms.gprs[x86_gpr_ax] = (uint32_t)(-ENOSYS);
	  goto sigaction_done;
	}
        ms.memory.readMultiple((uint8_t*)&sys_act.sa_mask, 128, act + 4); // Same size on 32 and 64 bits
        uint32_t handler = ms.readMemory<32>(x86_segreg_ds, act);
	if (handler == 0) {
          sys_act.sa_handler = SIG_DFL;
	  // fprintf(stdout, "New action has sa_handler=SIG_DFL sa_mask=%s sa_flags=%08X\n", sigsetToString(&sys_act.sa_mask).c_str(), sys_act.sa_flags);
	} else if (handler == 1) {
          sys_act.sa_handler = SIG_IGN;
	  // fprintf(stdout, "New action has sa_handler=SIG_IGN sa_mask=%s sa_flags=%08X\n", sigsetToString(&sys_act.sa_mask).c_str(), sys_act.sa_flags);
	} else {
	  // fprintf(stdout, "New action has sa_handler=%p sa_sigaction=%p sa_mask=%s sa_flags=%08X\n", sys_act.sa_handler, sys_act.sa_sigaction, sigsetToString(&sys_act.sa_mask).c_str(), sys_act.sa_flags);
	  ms.signalHandlers[signum] = handler;
	  sys_act.sa_handler = &simulate_signal;
	}
      }
      result = sigaction(signum, (act ? &sys_act : NULL), (oldact ? &sys_oldact : NULL));
      if (result == -1) {
	result = -errno;
      } else {
        if (oldact != 0) {
          ms.memory.writeMultiple((const uint8_t*)&sys_oldact.sa_mask, 128, oldact + 4);
          ms.writeMemory<32>(x86_segreg_ds, oldact + 132, sys_oldact.sa_flags);
          if (sys_oldact.sa_handler == &simulate_signal)
            ms.writeMemory<32>(x86_segreg_ds, oldact, ms.signalHandlers[signum]);
          else if (sys_oldact.sa_handler == SIG_DFL) 
            ms.writeMemory<32>(x86_segreg_ds, oldact, 0);
          else if (sys_oldact.sa_handler == SIG_IGN)
            ms.writeMemory<32>(x86_segreg_ds, oldact, 1);
          else
            ms.writeMemory<32>(x86_segreg_ds, oldact, (uint32_t)(-1));
        }
      }
      ms.gprs[x86_gpr_ax] = (uint32_t)result;
sigaction_done:
      break;
    }

    case 175: { // sigprocmask
      int how = (int)ms.gprs[x86_gpr_bx];
      uint32_t set = ms.gprs[x86_gpr_cx];
      uint32_t oldset = ms.gprs[x86_gpr_dx];
      fprintf(stdout, "sigprocmask(%d, 0x%08X, 0x%08X);\n", how, set, oldset);
      sigset_t sys_set, sys_oldset;
      if (set != 0) {
	ms.memory.readMultiple((uint8_t*)&sys_set, sizeof(sigset_t), set);
	fprintf(stdout, "New mask is %s\n", sigsetToString(&sys_set).c_str());
      }
      int result = sigprocmask(how, set ? &sys_set : NULL, oldset ? &sys_oldset : NULL);
      if (result == -1) {
	result = -errno;
      } else {
	if (oldset != 0) {
	  ms.memory.writeMultiple((const uint8_t*)&sys_oldset, sizeof(sigset_t), oldset);
	}
      }
      ms.gprs[x86_gpr_ax] = (uint32_t)result;
      break;
    }

    case 183: { // getcwd
      uint32_t buf = ms.gprs[x86_gpr_bx];
      unsigned long size = ms.gprs[x86_gpr_cx];
      char sysbuf[size];
      memset(sysbuf, 0, size);
      char* result = getcwd(sysbuf, size);
      if (result == NULL) {
	ms.gprs[x86_gpr_ax] = (uint32_t)(-errno);
      } else {
	size_t length = strnlen(sysbuf, size);
	ms.gprs[x86_gpr_ax] = length >= size ? size : length + 1;
	ms.memory.writeMultiple((const uint8_t*)sysbuf, length, buf);
      }
      break;
    }

    case 191: { // ugetrlimit
      int resource = ms.gprs[x86_gpr_bx];
      uint32_t rlp = ms.gprs[x86_gpr_cx];
      struct rlimit sys_rlp;
      int result = getrlimit(resource, &sys_rlp);
      if (result == 0 && rlp != 0) ms.memory.writeMultiple((const uint8_t*)&sys_rlp, sizeof(struct rlimit), rlp);
      if (result == 0) {
	ms.gprs[x86_gpr_ax] = 0;
      } else {
	ms.gprs[x86_gpr_ax] = (uint32_t)(-errno);
      }
      break;
    }

    case 192: { // mmap2
      uint32_t start = ms.gprs[x86_gpr_bx];
      uint32_t length = ms.gprs[x86_gpr_cx];
      uint32_t prot = ms.gprs[x86_gpr_dx];
      uint32_t flags = ms.gprs[x86_gpr_si];
      uint32_t fd = ms.gprs[x86_gpr_di];
      uint32_t offset = ms.gprs[x86_gpr_bp];
      do_mmap("mmap2", ms, start, length, prot, flags, fd, offset * PAGE_SIZE);
      break;
    }

    case 195: { // stat64
      uint32_t filename = (int)ms.gprs[x86_gpr_bx];
      uint32_t buf = ms.gprs[x86_gpr_cx];
      struct stat64 sys_buf;
      string sys_filename = ms.memory.readString(filename);
#ifdef DO_SIMULATION_TRACING
      fprintf(stderr, "  stat64(name=0x%08"PRIx32" \"%s\", statbuf=0x%08"PRIx32")\n",
              filename, &sys_filename[0], buf);
#endif
      int result = stat64(&sys_filename[0], &sys_buf);
      if (result == -1) result = -errno;
      if (result >= 0) {
	copyInStat(ms, sys_buf, buf);
      }
      ms.gprs[x86_gpr_ax] = (uint32_t)result;
      break;
    }

    case 196: { // lstat64
      uint32_t filename = (int)ms.gprs[x86_gpr_bx];
      uint32_t buf = ms.gprs[x86_gpr_cx];
      struct stat64 sys_buf;
      string sys_filename = ms.memory.readString(filename);
      int result = lstat64(&sys_filename[0], &sys_buf);
      if (result == -1) result = -errno;
      if (result >= 0) {
	copyInStat(ms, sys_buf, buf);
      }
      fprintf(stdout, "lstat64(\"%s\") -> %d = %s\n", &sys_filename[0], result, (result < 0 ? strerror(-result) : ""));
      ms.gprs[x86_gpr_ax] = (uint32_t)result;
      break;
    }

    case 197: { // fstat64
      int fildes = (int)ms.gprs[x86_gpr_bx];
      uint32_t buf = ms.gprs[x86_gpr_cx];
#ifdef DO_SIMULATION_TRACING
      fprintf(stderr, "  fstat64(fd=%d, statbuf=0x%08"PRIx32")\n", fildes, buf);
#endif
      struct stat64 sys_buf;
      int result = fstat64(fildes, &sys_buf);
      if (result == -1) result = -errno;
      if (result >= 0) {
	copyInStat(ms, sys_buf, buf);
      }
      ms.gprs[x86_gpr_ax] = (uint32_t)result;
      break;
    }

    case 199: { // getuid32
      uid_t id = getuid();
      ms.gprs[x86_gpr_ax] = id;
      break;
    }

    case 200: { // getgid32
      uid_t id = getgid();
      ms.gprs[x86_gpr_ax] = id;
      break;
    }

    case 201: { // geteuid32
      uid_t id = geteuid();
      ms.gprs[x86_gpr_ax] = id;
      break;
    }

    case 202: { // getegid32
      uid_t id = getegid();
      ms.gprs[x86_gpr_ax] = id;
      break;
    }

    case 220: { // getdents64
      int fd = ms.gprs[x86_gpr_bx];
      uint32_t dirp = ms.gprs[x86_gpr_cx];
      unsigned int count = ms.gprs[x86_gpr_dx];
      struct dirent64 sys_dirp[count];
      int result = syscall(__NR_getdents64, fd, sys_dirp, count);
      if (result == -1) {
	ms.gprs[x86_gpr_ax] = (uint32_t)(-errno);
      } else {
	ms.memory.writeMultiple((const uint8_t*)sys_dirp, result, dirp);
	ms.gprs[x86_gpr_ax] = result;
      }
      break;
    }

    case 221: { // fcntl
      int fd = (int)ms.gprs[x86_gpr_bx];
      int cmd = (int)ms.gprs[x86_gpr_cx];
      uint32_t other_arg = ms.gprs[x86_gpr_dx];
      int result = -EINVAL;
      // fprintf(stdout, "fcntl(%d, %d, %lu) --> ", fd, cmd, other_arg);
      switch (cmd) {
	case F_DUPFD: {
	  result = fcntl(fd, cmd, (long)other_arg);
	  if (result == -1) result = -errno;
	  break;
	}

	case F_SETFD: {
	  result = fcntl(fd, cmd, (long)other_arg);
	  if (result == -1) result = -errno;
	  break;
	}

	default: {
	  fprintf(stdout, "Unhandled fcntl %d on fd %d\n", cmd, fd);
	  result = -EINVAL;
	  break;
	}
      }
      // fprintf(stdout, "%d\n", result);
      ms.gprs[x86_gpr_ax] = (uint32_t)result;
      break;
    }

    case 224: { // gettid
      ms.gprs[x86_gpr_ax] = getpid();
      break;
    }

    case 229: { // getxattr
      ms.gprs[x86_gpr_ax] = (uint32_t)(-ENOSYS);
      break;
    }

    case 240: { // futex
#if 0
      uint32_t futex = ms.gprs[x86_gpr_bx];
      int op = ms.gprs[x86_gpr_cx];
      int val = ms.gprs[x86_gpr_dx];
      uint32_t timeout = ms.gprs[x86_gpr_si];
#endif
      ms.gprs[x86_gpr_ax] = (uint32_t)(-ENOSYS);
      break;
    }

    case 243: { // set_thread_area
      uint32_t u_info_ptr = ms.gprs[x86_gpr_bx];
      user_desc ud;
      ms.memory.readMultiple((uint8_t*)&ud, sizeof(user_desc), u_info_ptr);
      fprintf(stderr, "  set_thread_area({%d, 0x%08x, 0x%08x, %s, %u, %s, %s, %s, %s})\n",
              (int)ud.entry_number, ud.base_addr, ud.limit,
              ud.seg_32bit ? "32bit" : "16bit",
              ud.contents, ud.read_exec_only ? "read_exec" : "writable",
              ud.limit_in_pages ? "page_gran" : "byte_gran",
              ud.seg_not_present ? "not_present" : "present",
              ud.useable ? "usable" : "not_usable");
      if (ud.entry_number == (unsigned)-1) {
        for (ud.entry_number = (0x33 >> 3); ud.entry_number < 8192; ++ud.entry_number) {
          if (!ms.gdt[ud.entry_number].useable) break;
        }
        ROSE_ASSERT (ud.entry_number != 8192);
        fprintf(stderr, "  assigned entry number = %d\n", (int)ud.entry_number);
      }
      ms.gdt[ud.entry_number] = ud;
      ms.memory.writeMultiple((const uint8_t*)&ud, sizeof(user_desc), u_info_ptr);
      ms.gprs[x86_gpr_ax] = 0;
      for (size_t i = 0; i < 6; ++i) {
        ms.writeSegreg((X86SegmentRegister)i, ms.segregs[i]); // Reload for descriptor changes
      }
      break;
    }

    case 252: { // exit_group
      int status = (int)ms.gprs[x86_gpr_bx];
#ifdef DO_SIMULATION_TRACING
      fprintf(stderr, "  exit_group(%d)\n", status);
#endif
      _exit(status);
      break;
    }

    case 258: { // set_tid_address
      uint32_t addr = ms.gprs[x86_gpr_bx];
      pid_t val = ms.memory.read<4>(addr);
      if (val != getpid()) {
	ms.gprs[x86_gpr_ax] = (uint32_t)(-EINVAL);
      } else {
	ms.gprs[x86_gpr_ax] = 0;
      }
      break;
    }

    case 265: { // clock_gettime
      ms.gprs[x86_gpr_ax] = (uint32_t)(-ENOSYS);
      break;
    }

    default:
      fprintf(stderr, "Unhandled system call %lu\n", (unsigned long)ms.gprs[x86_gpr_ax]);
      abort();
  }
}


#endif /*ROSE_ENABLE_SIMULATOR*/
