#include <rose.h>


// These two lines are typically not needed in user projects. They're here so that this demo is conditionally compiled when
// distributed as part of the ROSE source tree, which must compile on a wide variety of platforms.
#include <RSIM_Private.h>
#ifdef ROSE_ENABLE_SIMULATOR


#include <arpa/inet.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The following stuff is mostly cut-n-pasted from x86sim.C with minor modifications to the documentation.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <CommandLine.h>                                // ROSE command-line
#include <Diagnostics.h>                                // ROSE diagnostics
#include <RSIM_Debugger.h>                              // Simulator interactive debugger
#include <RSIM_Linux32.h>                               // Simulator architecture: Linux-x86
#include <RSIM_Linux64.h>                               // Simulator architecture: Linux-amd64
#include <Sawyer/Map.h>                                 // Sawyer maps distributed with ROSE

using namespace Rose;
using namespace StringUtility;
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;

Sawyer::Message::Facility mlog;

enum GuestOs {
    GUEST_OS_NONE,
    GUEST_OS_LINUX_x86,
    GUEST_OS_LINUX_amd64
};

struct Settings {
    GuestOs guestOs;
    bool catchingSignals;
    bool usingDebugger;
    RSIM_Simulator::Settings simSettings;
    Settings()
        : guestOs(GUEST_OS_NONE), catchingSignals(false), usingDebugger(false) {}
};

std::vector<std::string>
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;
    Parser parser;
    parser
        .purpose("concrete simulation of an executable and monitor file descriptors")
        .version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE)
        .chapter(1, "ROSE Command-line Tools")
        .doc("Synopsis",
             "@prop{programName} [@v{switches}] [--] @v{specimen} [@v{specimen_args}...]\n\n"
             "@prop{programName} [@v{switches}] [--] @v{pid}\n\n")

        .doc("Description", "a",
             "This tool simulates concrete execution of an executable specimen in an unsafe manner. Any system calls made by "
             "the specimen are passed along by the underlying operating system after possible translation by this tool. The "
             "tool emulates various services typically provided by the operating system, such as memory management, signal "
             "handling, and thread support. It also monitors and reports certain operations on file descriptors.\n\n"

             "This tool is not intended to be a full-fledged virtual machine, but rather a dynamic analysis platform. "
             "The dynamic nature comes from ROSE's ability to \"execute\" an instruction in a concrete domain, and the "
             "tool's ability to simulate certain aspects of an operating system, including process and thread creation, "
             "signal delivery, memory management, and system call emulation. There may be other alternatives more "
             "suitable to your situation:"

             "@bullet{ROSE can analyze a natively running process.  It does so by attaching to the process as if it "
             "were a debugger and querying the process's state (memory and registers) from the operating system. This is "
             "much less complicated than trying to simulate execution by emulating substantial features of the operating "
             "system.}"

             "@bullet{ROSE has a built-in debugger. ROSE can execute a specimen natively within its own debugger, which "
             "includes the ability to set breakpoints for entire regions of memory (e.g., all addresses). This allows "
             "user-written tools to easily customize execution within the debugger. This can be combined with the previous "
             "bullet to analyze the program both dynamically and staticlly is and is significantly faster than "
             "simulating each instruction.}")

        .doc("Specimen", "x",
             "The specimen can be specified by name or process ID.  If a name is given then the simulator can either "
             "emulate the Linux \"exec\" system call to load the specimen into a simulated process; or it can use Linux "
             "directly to load the specimen into a temporary native process, initialize the simulator from that process, "
             "and then kill that process. The @s{native-load} switch causes the latter behavior.\n\n"

             "If the specimen is specified with a process ID then the simulator attaches to that process, initializes its "
             "memory and registers from that process, and then detaches.  One should note that only memory and registers "
             "are initialized and not the simulated Linux kernel state. In particular, file descriptors open in the process "
             "will not be present in the simulator; parent/child relationships will be broken since the simulator is not "
             "related to the process; the simulated process will have only one thread; any process ID stored in the "
             "simulated memory will be incorrect; pending signals and signal masks will not be initialized from the process; "
             "process real and effective uid and gid may be different; etc.  This method is best used only with a process "
             "that has been just created and then stopped.")

        .doc("Caveats", "z",
             "Speed of simulation is not a primary goal of this tool. ROSE is mostly a static analysis library "
             "whose capabilities happen to include the ability to write a concrete simulation tool.\n\n"

             "Specimen memory is managed in a container within the simulating ROSE process. Therefore operations like "
             "interprocess communication via shared memory will probably never work.  This limitation may also include "
             "intra-process communication with shared memory mapped at two different addresses, and other \"tricks\" that "
             "specimens sometimes do with their memory maps.\n\n"

             "The specimen process and threads are simulated inside this tool's process and threads. Certain operations that "
             "modify process and thread properties will end up modifying the tool's process and threads. For instance, reading "
             "from the Linux \"/proc/@v{n}\" filesystem will return information about the simulator tool rather than "
             "the specimen being simulated. Sending a signal to a process or thread will cause the tool to forward the "
             "signal to the simulated process or thread only if this behavior is enabled at runtime.\n\n"

             "Emulating the fork (clone) system call is not practically possible because the simulator is multi-threaded "
             "even when simulating a single-threaded guest, and the simulator needs to do more than call only async-signal "
             "safe functions after the fork returns in the child. A guest that forks cannot be simulated past the fork. This "
             "includes guests that call C library functions like @man{system}{3} and @man{popen}{3}.\n\n"

             "See the README file in the simulator source code directory for more caveats.");

    SwitchGroup sg("Tool-specific switches");

    sg.insert(Switch("arch")
              .argument("architecture", enumParser<GuestOs>(settings.guestOs)
                        ->with("linux-x86", GUEST_OS_LINUX_x86)
                        ->with("linux-amd64", GUEST_OS_LINUX_amd64))
              .doc("Simulated host architecture.  The supported architectures are:"
                   "@named{linux-x86}{Linux operating system running on 32-bit x86-compatible hardware.}"
                   "@named{linux-amd64}{Linux operating system running on 64-bit amd64-compatible hardware.}"));

    sg.insert(Switch("signals")
              .intrinsicValue(true, settings.catchingSignals)
              .doc("Causes the simulator to catch signals sent by other processes and deliver them to the "
                   "specimen by emulating the operating system's entire signal delivery mechanism.  The "
                   "@s{no-signals} switch disables this feature, in which case signals sent to this tool "
                   "will cause the default action to occur (e.g., pressing Control-C will likely terminate "
                   "the tool directly, whereas when signal handling is emulated it will cause a simulated SIGINT "
                   "to be sent to the specimen possibly causing the an emulated termination). Signals raised by "
                   "the specimen to be delivered to itself are always emulated; signals raised by the specimen to "
                   "be delivered to some other process are handled by system call emulation.  The default is that "
                   "signals generated by some other process and delivered to the simulator process will " +
                   std::string(settings.catchingSignals ? "be forwarded to the specimen." :
                               "cause the default signal action within the simulator tool.")));
    sg.insert(Switch("no-signals")
              .key("signals")
              .intrinsicValue(false, settings.catchingSignals)
              .hidden(true));

    sg.insert(Switch("debugger")
              .intrinsicValue(true, settings.usingDebugger)
              .doc("Invokes a simple interactive debugger.  See RSIM_Tools.C for the commands; this isn't really "
                   "intended for end users yet."));

    return parser
        .errorStream(::mlog[FATAL])
        .with(Rose::CommandLine::genericSwitches())
        .with(sg)                                       // tool-specific
        .with(RSIM_Simulator::commandLineSwitches(settings.simSettings))
        .parse(argc, argv).apply().unreachedArgs();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Socket-related callbacks specific to this tool (not copied from x86sim.C). We need two versions because linux-x86 and
// linux-amd64 handle socket calls in entirely different ways.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Architecture-independent part of socket handling
class BaseSocketSyscallHandler: public RSIM_Callbacks::SyscallCallback {
protected:
    Sawyer::Container::Map<int /*fd*/, std::string /*address*/> sockets;

public:
    // Keep it simple--don't worry about multiple threads.
    virtual BaseSocketSyscallHandler* clone() ROSE_OVERRIDE { return this; }
    
    // Assume addr_va points to a struct sockaddr and decode and return the IP port number.
    std::string decode_ip_addr(RSIM_Process *proc, rose_addr_t addr_va, size_t addrlen) {
        std::string retval = "addr_va=" + addrToString(addr_va) + " addrlen=" + numberToString(addrlen);
        uint8_t addr[4096];
        if (0==addr_va || addrlen<2 || addrlen>sizeof addr)
            return retval;
        size_t nread = proc->mem_read(addr, addr_va, addrlen);
        if (nread != addrlen)
            return retval + " (short read)";
        uint16_t family = *(uint16_t*)addr;
        if (family != AF_INET && family != AF_INET6)
            return retval + " (not INET)";
        int port = ntohs(*(uint16_t*)(addr+2));
        char s[INET_ADDRSTRLEN];
        if (inet_ntop(family, addr+4, s, sizeof s))
            return std::string(s) + ":" + numberToString(port);
        return retval + " port=" + numberToString(port);
    }

    void handle_bind(const Args &args, int fd, rose_addr_t addrVa, size_t addrLen) {
        std::string addrStr = decode_ip_addr(args.thread->get_process(), addrVa, addrLen);
        sockets.insert(fd, addrStr);
        mlog[INFO] <<"bind socket=" <<fd <<" " <<addrStr <<"\n";
    }

    void handle_connect(const Args &args, int fd, rose_addr_t addrVa, size_t addrLen) {
        std::string addrStr = decode_ip_addr(args.thread->get_process(), addrVa, addrLen);
        sockets.insert(fd, addrStr);
        mlog[INFO] <<"connect socket=" <<fd <<" " <<addrStr <<"\n";
    }

    void handle_accept(const Args &args, int oldFd, int newFd, rose_addr_t addrVa, size_t addrLen) {
        std::string addrStr = decode_ip_addr(args.thread->get_process(), addrVa, addrLen);
        sockets.insert(newFd, addrStr);
        mlog[INFO] <<"accept socket=" <<newFd <<" " <<addrStr <<" (from socket " <<oldFd <<")\n";
    }

    void handle_write(const Args &args, int fd, rose_addr_t bufVa, size_t bufSize) {
        if (sockets.exists(fd)) {
            mlog[INFO] <<"write socket=" <<fd <<" " <<sockets[fd]
                       <<" buffer=" <<addrToString(bufVa) <<", size=" <<bufSize <<"\n";
            unsigned char *buf = new unsigned char[bufSize];
            size_t nread = args.thread->get_process()->mem_read(buf, bufVa, bufSize);
            if (bufSize != nread) {
                mlog[ERROR] <<"SocketSyscallHandler: short read from specimen memory\n";
            } else {
                HexdumpFormat fmt;
                fmt.prefix = "  ";
                mlog[INFO] <<fmt.prefix;
                SgAsmExecutableFileFormat::hexdump(mlog[INFO], bufVa, buf, nread, fmt);
                mlog[INFO] <<"\n";
            }
            delete[] buf;
        }
    }

    void handle_close(const Args &args, int fd) {
        std::string addrStr;
        if (sockets.getOptional(fd).assignTo(addrStr)) {
            mlog[INFO] <<"close socket=" <<fd <<" " <<addrStr <<"\n";
            sockets.erase(fd);
        }
    }
};

// Generic
template<class Simulator>
class SocketSyscallHandler: public BaseSocketSyscallHandler {};

// Specialization for linux-x86
template<>
class SocketSyscallHandler<RSIM_Linux32>: public BaseSocketSyscallHandler {

    bool operator()(bool enabled, const Args &args) ROSE_OVERRIDE {
        if (!enabled)
            return false;

        // We're expecting to be called after the system call returns.
        int32_t sysresult = args.thread->get_regs().ax;
        if (sysresult<0)
            return true;                                // don't worry about syscalls which failed

        if (args.callno == 102 /*SYS_socketcall*/) {
            // The structure of this code closely follows that in the Linux kernel. See linux/net/socket.c.  We're not handling
            // everything, just the stuff we need for analyze which ports are opened to connect or listen. This syscall is a
            // little weird compared to others because Linux handles all these socket-related operations as a single system
            // call that passes a pointer to a union of structs that hold the parameters, some of whose members are themselves
            // pointers.
            uint32_t socket_command = args.thread->syscall_arg(0);
            uint32_t a[6];
            switch (socket_command) {
                case 2:                                 // SYS_BIND
                    if (12!=args.thread->get_process()->mem_read(a, args.thread->syscall_arg(1), 12)) {
                        mlog[ERROR] <<"SocketSyscallHandler: could not read 3 words for bind struct\n";
                    } else {
                        int fd = a[0];
                        uint32_t addr_va = a[1];
                        uint32_t addrlen = a[2];
                        handle_bind(args, fd, addr_va, addrlen);
                    }
                    break;
                case 3:                                 // SYS_CONNECT
                    if (12!=args.thread->get_process()->mem_read(a, args.thread->syscall_arg(1), 12)) {
                        mlog[ERROR] <<"SocketSyscallHandler: could not read 3 words for connect struct\n";
                    } else {
                        int fd = a[0];
                        uint32_t addr_va = a[1];
                        uint32_t addrlen = a[2];
                        handle_connect(args, fd, addr_va, addrlen);
                    }
                    break;
                case 5:                                 // SYS_ACCEPT
                    if (12!=args.thread->get_process()->mem_read(a, args.thread->syscall_arg(1), 12)) {
                        mlog[ERROR] <<"SocketSyscallHandler: could not read 3 words for accept struct\n";
                    } else {
                        int oldfd = a[0];
                        int newfd = sysresult;
                        uint32_t addr_va = a[1];
                        uint32_t addrlen_va = a[2];
                        uint32_t addrlen;
                        if (4 != args.thread->get_process()->mem_read(&addrlen, addrlen_va, 4))
                            addrlen = addr_va = 0;
                        handle_accept(args, oldfd, newfd, addr_va, addrlen);
                    }
                    break;
            }
        } else if (args.callno == 4 /*SYS_write*/) {
            // We should also be watching for writev, send, sendto, sendmsg, etc.  Similarly if you want to monitor reads.
            int fd = args.thread->syscall_arg(0);
            uint32_t buf_va = args.thread->syscall_arg(1);
            uint32_t buf_sz = args.thread->syscall_arg(2);
            handle_write(args, fd, buf_va, buf_sz);
        } else if (args.callno == 6 /*SYS_close*/) {
            int fd = args.thread->syscall_arg(0);
            handle_close(args, fd);
        }
        
        return true;
    }
};

// Specialization for linux-amd64
template<>
class SocketSyscallHandler<RSIM_Linux64>: public BaseSocketSyscallHandler {
public:
    bool operator()(bool enabled, const Args &args) ROSE_OVERRIDE {
        if (!enabled)
            return false;

        // We're expecting to be called after the system call returns
        int64_t sysresult = args.thread->get_regs().ax;
        if (sysresult < 0)
            return true;

        switch (args.callno) {
            case 49: {                                  // bind
                int fd = args.thread->syscall_arg(0);
                rose_addr_t addrVa = args.thread->syscall_arg(1);
                size_t addrLen = args.thread->syscall_arg(2);
                handle_bind(args, fd, addrVa, addrLen);
                break;
            }
            case 42: {                                  // connect
                int fd = args.thread->syscall_arg(0);
                rose_addr_t addrVa = args.thread->syscall_arg(1);
                size_t addrLen = args.thread->syscall_arg(2);
                handle_connect(args, fd, addrVa, addrLen);
                break;
            }
            case 43: {                                  // accept
                int oldFd = args.thread->syscall_arg(0);
                int newFd = sysresult;
                rose_addr_t addrVa = args.thread->syscall_arg(1);
                rose_addr_t addrLenVa = args.thread->syscall_arg(2);
                uint32_t addrLen;
                if (4 != args.thread->get_process()->mem_read(&addrLen, addrLenVa, 4))
                    addrLen = addrVa = 0;
                handle_accept(args, oldFd, newFd, addrVa, addrLen);
                break;
            }
            case 1: {                                   // write
                int fd = args.thread->syscall_arg(0);
                rose_addr_t bufVa = args.thread->syscall_arg(1);
                rose_addr_t bufSize = args.thread->syscall_arg(2);
                handle_write(args, fd, bufVa, bufSize);
                break;
            }
            case 3: {                                   // close
                int fd = args.thread->syscall_arg(0);
                handle_close(args, fd);
                break;
            }
        }
        return true;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Filesystem-related callbacks specific to this tool (not copied from x86sim.C).
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Architecture-independent part of filesystem syscall handling.
class BaseFilesystemSyscallHandler: public RSIM_Callbacks::SyscallCallback {
public:
    typedef Sawyer::Container::Map<int /*fd*/, std::string /*filename*/> Files;
    Files files;

    BaseFilesystemSyscallHandler* clone() ROSE_OVERRIDE { return this; }

    void emitWarning() {
        static int ncalls = 0;
        if (1==++ncalls) {
            mlog[INFO] <<"NOTE: Operations on file descriptors will be reported as both a file descriptor and\n"
                       <<"      a file name.  The file name should be taken with a grain of salt since it is\n"
                       <<"      possible (even common) for a file to be opened and then unlinked from the fs.\n"
                       <<"      In fact, after unlinking, a new file can be created having the same name and will\n"
                       <<"      not be the file accessed by the original file descriptor.  Also, the names reported\n"
                       <<"      here are the names supplied by the specimen and are not canonical.\n";
        }
    }

};

// Generic
template<class Simulator>
class FilesystemSyscallHandler: public BaseFilesystemSyscallHandler {};

// Specialization for linux-x86
template<>
class FilesystemSyscallHandler<RSIM_Linux32>: public BaseFilesystemSyscallHandler {
    bool operator()(bool enabled, const Args &args) ROSE_OVERRIDE {
        if (!enabled)
            return false;
        emitWarning();

        // We're expecting to be called after the system call returns.
        int32_t sysresult = args.thread->get_regs().ax;
        if (sysresult<0)
            return true;                                // don't worry about syscalls which failed

        // These are the syscalls that can modify the filesystem (there might be some others too).
        std::string filename;
        int fd = -1;
        switch (args.callno) {
            case 4: {                                   // SYS_write
                fd = args.thread->syscall_arg(0);
                if (files.getOptional(fd).assignTo(filename))
                    mlog[INFO] <<"write fd=" <<fd <<" \"" <<cEscape(filename) <<"\"\n";
                break;
            }
            case 5: {                                   // SYS_open
                fd = sysresult;
                filename = args.thread->get_process()->read_string(args.thread->syscall_arg(0));
                files.insert(fd, filename);
                unsigned flags = args.thread->syscall_arg(1);
                mlog[INFO] <<"open fd=" <<fd <<" \"" <<cEscape(filename) <<"\""
                           <<((flags & (O_CREAT|O_TRUNC)) ? " O_CREAT and/or O_TRUNC" : "") <<"\n";
                break;
            }
            case 6: {                                   // SYS_close
                fd = args.thread->syscall_arg(0);
                if (files.getOptional(fd).assignTo(filename))
                    mlog[INFO] <<"close fd=" <<fd <<" \"" <<cEscape(filename) <<"\"\n";
                files.erase(fd);
                break;
            }
            case 8: {                                   // SYS_creat
                fd = sysresult;
                filename = args.thread->get_process()->read_string(args.thread->syscall_arg(0));
                files.insert(fd, filename);
                mlog[INFO] <<"creat fd=" <<fd <<" \"" <<cEscape(filename) <<"\"\n";
                break;
            }
            case 9: {                                   // SYS_link
                std::string oldpath = args.thread->get_process()->read_string(args.thread->syscall_arg(0));
                std::string newpath = args.thread->get_process()->read_string(args.thread->syscall_arg(1));
                mlog[INFO] <<"link \"" <<cEscape(oldpath) <<"\" to \"" <<cEscape(newpath) <<"\"\n";
                break;
            }
            case 10: {                                  // SYS_unlink
                filename = args.thread->get_process()->read_string(args.thread->syscall_arg(0));
                mlog[INFO] <<"unlink \"" <<cEscape(filename) <<"\"\n";
                break;
            }
            case 14: {                                  // SYS_mknod
                fd = sysresult;
                filename = args.thread->get_process()->read_string(args.thread->syscall_arg(0));
                files.insert(fd, filename);
                mlog[INFO] <<"mknod fd=" <<fd <<" \"" <<cEscape(filename) <<"\"\n";
                break;
            }
            case 38: {                                  // SYS_rename
                std::string oldpath = args.thread->get_process()->read_string(args.thread->syscall_arg(0));
                std::string newpath = args.thread->get_process()->read_string(args.thread->syscall_arg(1));
                mlog[INFO] <<"rename \"" <<cEscape(oldpath) <<"\" to \"" <<cEscape(newpath) <<"\"\n";
                break;
            }
            case 39: {                                  // SYS_mkdir
                filename = args.thread->get_process()->read_string(args.thread->syscall_arg(0));
                mlog[INFO] <<"mkdir \"" <<cEscape(filename) <<"\"\n";
                break;
            }
            case 40: {                                  // SYS_rmdir
                filename = args.thread->get_process()->read_string(args.thread->syscall_arg(0));
                mlog[INFO] <<"rmdir \"" <<cEscape(filename) <<"\"\n";
                break;
            }
            case 83: {                                  // SYS_symlink
                std::string oldpath = args.thread->get_process()->read_string(args.thread->syscall_arg(0));
                std::string newpath = args.thread->get_process()->read_string(args.thread->syscall_arg(1));
                mlog[INFO] <<"symlink \"" <<cEscape(oldpath) <<"\" to \"" <<cEscape(newpath) <<"\"\n";
                break;
            }
            case 90: {                                  // SYS_mmap
                mmap_arg_struct_32 mapArgs;
                size_t nread = args.thread->get_process()->mem_read(&mapArgs, args.thread->syscall_arg(0), sizeof mapArgs);
                if (sizeof(mapArgs) != nread) {
                    mlog[ERROR] <<"FilesystemSyscallHandler: short read of SYS_mmap arguments\n";
                } else if (0 == (mapArgs.flags & (MAP_PRIVATE|MAP_ANONYMOUS)) &&
                           files.getOptional(mapArgs.fd).assignTo(filename)) {
                    // Report mapping regardless of protection since protection can be changed later. However, we don't need to
                    // report private or anonymous mapping since the underlying file can't be changed.
                    mlog[INFO] <<"mmap fd=" <<mapArgs.fd <<" \"" <<cEscape(filename) <<"\"\n";
                }
                break;
            }
            case 93: {                                  // SYS_ftruncate
                fd = args.thread->syscall_arg(0);
                if (files.getOptional(fd).assignTo(filename))
                    mlog[INFO] <<"ftruncate fd=" <<fd <<" \"" <<cEscape(filename) <<"\"\n";
                break;
            }
            case 94: {                                  // SYS_fchmod
                fd = args.thread->syscall_arg(0);
                if (files.getOptional(fd).assignTo(filename))
                    mlog[INFO] <<"fchmod fd=" <<fd <<" \"" <<cEscape(filename) <<"\"\n";
                break;
            }
            case 95: {                                  // SYS_fchown
                fd = args.thread->syscall_arg(0);
                if (files.getOptional(fd).assignTo(filename))
                    mlog[INFO] <<"fchown fd=" <<fd <<" \"" <<cEscape(filename) <<"\"\n";
                break;
            }
            case 146: {                                 // SYS_writev
                fd = args.thread->syscall_arg(0);
                if (files.getOptional(fd).assignTo(filename))
                    mlog[INFO] <<"writev fd=" <<fd <<" \"" <<cEscape(filename) <<"\"\n";
                break;
            }
            case 192: {                                 // SYS_mmap2
                fd = args.thread->syscall_arg(4);
                unsigned flags = args.thread->syscall_arg(3);
                // Report mapping regardless of protection since protection can be changed later.  However, we don't need to
                // report private or anonymous mapping since the underlying file can't be changed.
                if (0 == (flags & (MAP_PRIVATE|MAP_ANONYMOUS)) && files.getOptional(fd).assignTo(filename))
                    mlog[INFO] <<"mmap2 fd=" <<fd <<" \"" <<cEscape(filename) <<"\"\n";
                break;
            }
            case 212: {                                 // SYS_chown
                filename = args.thread->get_process()->read_string(args.thread->syscall_arg(0));
                mlog[INFO] <<"chown \"" <<cEscape(filename) <<"\"\n";
                break;
            }
            case 306: {                                 // SYS_fchmodat
                filename = args.thread->get_process()->read_string(args.thread->syscall_arg(1));
                mlog[INFO] <<"fchmodat \"" <<cEscape(filename) <<"\"\n";
                break;
            }
        }
        return true;
    }
};

// Specialization for linux-x86
template<>
class FilesystemSyscallHandler<RSIM_Linux64>: public BaseFilesystemSyscallHandler {
    bool operator()(bool enabled, const Args &args) ROSE_OVERRIDE {
        if (!enabled)
            return false;
        emitWarning();

        // We're expecting to be called after the system call returns.
        int sysresult = args.thread->get_regs().ax;
        if (sysresult < 0)
            return true;                                // dont' worry about syscalls which failed

        // These are the syscalls that can modify the filesystem (there might be some others too).
        std::string filename;
        int fd = -1;
        switch (args.callno) {
            case 1: {                                   // write
                fd = args.thread->syscall_arg(0);
                if (files.getOptional(fd).assignTo(filename))
                    mlog[INFO] <<"write fd=" <<fd <<" \"" <<cEscape(filename) <<"\"\n";
                break;
            }
            case 2: {                                   // open
                fd = sysresult;
                filename = args.thread->get_process()->read_string(args.thread->syscall_arg(0));
                files.insert(fd, filename);
                unsigned flags = args.thread->syscall_arg(1);
                mlog[INFO] <<"open fd=" <<fd <<" \"" <<cEscape(filename) <<"\""
                           <<((flags & (O_CREAT|O_TRUNC)) ? " O_CREAT and/or O_TRUNC" : "") <<"\n";
                break;
            }
            case 3: {                                   // close
                fd = args.thread->syscall_arg(0);
                if (files.getOptional(fd).assignTo(filename))
                    mlog[INFO] <<"close fd=" <<fd <<" \"" <<cEscape(filename) <<"\"\n";
                files.erase(fd);
                break;
            }
            case 85: {                                  // creat
                fd = sysresult;
                filename = args.thread->get_process()->read_string(args.thread->syscall_arg(0));
                files.insert(fd, filename);
                mlog[INFO] <<"creat fd=" <<fd <<" \"" <<cEscape(filename) <<"\"\n";
                break;
            }
            case 86: {                                  // link
                std::string oldpath = args.thread->get_process()->read_string(args.thread->syscall_arg(0));
                std::string newpath = args.thread->get_process()->read_string(args.thread->syscall_arg(1));
                mlog[INFO] <<"link \"" <<cEscape(oldpath) <<"\" to \"" <<cEscape(newpath) <<"\"\n";
                break;
            }
            case 87: {                                  // unlink
                filename = args.thread->get_process()->read_string(args.thread->syscall_arg(0));
                mlog[INFO] <<"unlink \"" <<cEscape(filename) <<"\"\n";
                break;
            }
            case 133: {                                 // mknod
                fd = sysresult;
                filename = args.thread->get_process()->read_string(args.thread->syscall_arg(0));
                files.insert(fd, filename);
                mlog[INFO] <<"mknod fd=" <<fd <<" \"" <<cEscape(filename) <<"\"\n";
                break;
            }
            case 82: {                                  // rename
                std::string oldpath = args.thread->get_process()->read_string(args.thread->syscall_arg(0));
                std::string newpath = args.thread->get_process()->read_string(args.thread->syscall_arg(1));
                mlog[INFO] <<"rename \"" <<cEscape(oldpath) <<"\" to \"" <<cEscape(newpath) <<"\"\n";
                break;
            }
            case 83: {                                  // mkdir
                filename = args.thread->get_process()->read_string(args.thread->syscall_arg(0));
                mlog[INFO] <<"mkdir \"" <<cEscape(filename) <<"\"\n";
                break;
            }
            case 84: {                                  // rmdir
                filename = args.thread->get_process()->read_string(args.thread->syscall_arg(0));
                mlog[INFO] <<"rmdir \"" <<cEscape(filename) <<"\"\n";
                break;
            }
            case 88: {                                  // symlink
                std::string oldpath = args.thread->get_process()->read_string(args.thread->syscall_arg(0));
                std::string newpath = args.thread->get_process()->read_string(args.thread->syscall_arg(1));
                mlog[INFO] <<"symlink \"" <<cEscape(oldpath) <<"\" to \"" <<cEscape(newpath) <<"\"\n";
                break;
            }
            case 77: {                                  // ftruncate
                fd = args.thread->syscall_arg(0);
                if (files.getOptional(fd).assignTo(filename))
                    mlog[INFO] <<"ftruncate fd=" <<fd <<" \"" <<cEscape(filename) <<"\"\n";
                break;
            }
            case 91: {                                  // fchmod
                fd = args.thread->syscall_arg(0);
                if (files.getOptional(fd).assignTo(filename))
                    mlog[INFO] <<"fchmod fd=" <<fd <<" \"" <<cEscape(filename) <<"\"\n";
                break;
            }
            case 93: {                                  // fchown
                fd = args.thread->syscall_arg(0);
                if (files.getOptional(fd).assignTo(filename))
                    mlog[INFO] <<"fchown fd=" <<fd <<" \"" <<cEscape(filename) <<"\"\n";
                break;
            }
            case 20: {                                  // writev
                fd = args.thread->syscall_arg(0);
                if (files.getOptional(fd).assignTo(filename))
                    mlog[INFO] <<"writev fd=" <<fd <<" \"" <<cEscape(filename) <<"\"\n";
                break;
            }
            case 9: {                                   // mmap
                fd = args.thread->syscall_arg(4);
                unsigned flags = args.thread->syscall_arg(3);
                // Report mapping regardless of protection since protection can be changed later.  However, we don't need to
                // report private or anonymous mapping since the underlying file can't be changed.
                if (0 == (flags & (MAP_PRIVATE|MAP_ANONYMOUS)) && files.getOptional(fd).assignTo(filename))
                    mlog[INFO] <<"mmap fd=" <<fd <<" \"" <<cEscape(filename) <<"\"\n";
                break;
            }
            case 92: {                                  // chown
                filename = args.thread->get_process()->read_string(args.thread->syscall_arg(0));
                mlog[INFO] <<"chown \"" <<cEscape(filename) <<"\"\n";
                break;
            }
            case 268: {                                 // fchmodat
                filename = args.thread->get_process()->read_string(args.thread->syscall_arg(1));
                mlog[INFO] <<"fchmodat \"" <<cEscape(filename) <<"\"\n";
                break;
            }
        }
        return true;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The rest of this is mostly cut-n-pasted from x86sim.C except where the callbacks are registered which are specific to this
// particular tool.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class Simulator>
static void
simulate(const Settings &settings, const std::vector<std::string> &args, char *envp[]) {
    // Create and configure
    Simulator sim;
    if (settings.usingDebugger)
        RSIM_Debugger::attach(sim);
    sim.configure(settings.simSettings, envp);

    // Initialize callbacks
    sim.install_callback(new SocketSyscallHandler<Simulator>, RSIM_Callbacks::AFTER);
    sim.install_callback(new FilesystemSyscallHandler<Simulator>, RSIM_Callbacks::AFTER);

    // Load specimen directly or via debugger
    pid_t existingPid = -1;
    char *rest = NULL;
    errno = 0;
    if (args.size()==1 && (existingPid=strtoul(args.front().c_str(), &rest, 0))>=0 && !errno && !*rest) {
        if (sim.loadSpecimen(existingPid) < 0)
            return;
    } else {
        if (sim.loadSpecimen(args) < 0)
            return;
    }

    // Run the simulator
    if (settings.catchingSignals)
        sim.activate();
    sim.main_loop();
    if (settings.catchingSignals)
        sim.deactivate();
    std::cerr <<sim.describe_termination() <<"\n";
    sim.terminate_self();                               // probably doesn't return
}

int
main(int argc, char *argv[], char *envp[]) {
    // Initialize ROSE diagnostics since we're not calling ROSE frontend()
    ROSE_INITIALIZE;
    ::mlog = Sawyer::Message::Facility("tool", Diagnostics::destination);
    Diagnostics::mfacilities.insert(mlog);

    // Configure the simulator
    Settings settings;
    std::vector<std::string> specimen = parseCommandLine(argc, argv, settings);
    switch (settings.guestOs) {
        case GUEST_OS_LINUX_x86:
            simulate<RSIM_Linux32>(settings, specimen, envp);
            break;
        case GUEST_OS_LINUX_amd64:
            simulate<RSIM_Linux64>(settings, specimen, envp);
            break;
        case GUEST_OS_NONE:
            ::mlog[FATAL] <<"no architecture specified (\"--arch\"); see \"--help\"\n";
            exit(1);
    }
}

#else
int main() {
    std::cerr <<"ROSE simulator is not supported for this configuration\n";
}
#endif
