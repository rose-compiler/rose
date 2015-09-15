#include <rose.h>
#include <RSIM_Private.h>
#ifdef ROSE_ENABLE_SIMULATOR

#include <arpa/inet.h>
#include <Diagnostics.h>
#include <RSIM_Linux32.h>
#include <Sawyer/Map.h>

using namespace rose;
using namespace StringUtility;
using namespace rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;

Sawyer::Message::Facility mlog;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Socket-related stuff
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Registered with simulator to handle various network-related syscalls.
class SocketSyscallHandler: public RSIM_Callbacks::SyscallCallback {
    Sawyer::Container::Map<int /*fd*/, std::string /*address*/> sockets;

    // Keep it simple--don't worry about multiple threads.
    SocketSyscallHandler* clone() ROSE_OVERRIDE { return this; }
    
    // Assume addr_va points to a struct sockaddr and decode and return the IP port number.
    std::string decode_ip_addr(RSIM_Process *proc, uint32_t addr_va, uint32_t addrlen) {
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
    
    bool operator()(bool enabled, const Args &args) ROSE_OVERRIDE {
        if (!enabled)
            return false;

        // We're expecting to be called after the system call returns.
        const RegisterDescriptor &EAX = args.thread->policy.reg_eax;
        int32_t sysresult = args.thread->policy.readRegister<32>(EAX).known_value();
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
                        std::string addrStr = decode_ip_addr(args.thread->get_process(), addr_va, addrlen);
                        sockets.insert(fd, addrStr);
                        mlog[INFO] <<"bind socket=" <<fd <<" " <<addrStr <<"\n";
                    }
                    break;
                case 3:                                 // SYS_CONNECT
                    if (12!=args.thread->get_process()->mem_read(a, args.thread->syscall_arg(1), 12)) {
                        mlog[ERROR] <<"SocketSyscallHandler: could not read 3 words for connect struct\n";
                    } else {
                        int fd = a[0];
                        uint32_t addr_va = a[1];
                        uint32_t addrlen = a[2];
                        std::string addrStr = decode_ip_addr(args.thread->get_process(), addr_va, addrlen);
                        sockets.insert(fd, addrStr);
                        mlog[INFO] <<"connect socket=" <<fd <<" " <<addrStr <<"\n";
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
                        std::string addrStr = decode_ip_addr(args.thread->get_process(), addr_va, addrlen);
                        sockets.insert(newfd, addrStr);
                        mlog[INFO] <<"accept socket=" <<newfd <<" " <<addrStr <<" (from socket " <<oldfd <<")\n";
                    }
                    break;
            }
        } else if (args.callno == 4 /*SYS_write*/) {
            // We should also be watching for writev, send, sendto, sendmsg, etc.  Similarly if you want to monitor reads.
            int fd = args.thread->syscall_arg(0);
            if (!sockets.exists(fd))
                return true;                            // something other than a socket, perhaps a file or other device
            uint32_t buf_va = args.thread->syscall_arg(1);
            uint32_t buf_sz = args.thread->syscall_arg(2);
            mlog[INFO] <<"write socket=" <<fd <<" " <<sockets[fd]
                       <<" buffer=" <<addrToString(buf_va) <<", size=" <<buf_sz <<"\n";
            unsigned char *buf = new unsigned char[buf_sz];
            size_t nread = args.thread->get_process()->mem_read(buf, buf_va, buf_sz);
            if (buf_sz != nread) {
                mlog[ERROR] <<"SocketSyscallHandler: short read from specimen memory\n";
            } else {
                HexdumpFormat fmt;
                fmt.prefix = "  ";
                mlog[INFO] <<fmt.prefix;
                SgAsmExecutableFileFormat::hexdump(mlog[INFO], buf_va, buf, nread, fmt);
                mlog[INFO] <<"\n";
            }
            delete buf;
        } else if (args.callno == 6 /*SYS_close*/) {
            int fd = args.thread->syscall_arg(0);
            std::string addrStr;
            if (sockets.getOptional(fd).assignTo(addrStr)) {
                mlog[INFO] <<"close socket=" <<fd <<" " <<addrStr <<"\n";
                sockets.erase(fd);
            }
        }
        
        return true;
    }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Filesystem-related stuff
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Registered with simulator to handle various filesystem-related syscalls.
class FilesystemSyscallHandler: public RSIM_Callbacks::SyscallCallback {
    // Keep it simple-don't worry about multiple threads.
    typedef Sawyer::Container::Map<int /*fd*/, std::string /*filename*/> Files;
    Files files;

    FilesystemSyscallHandler* clone() ROSE_OVERRIDE { return this; }
    
    bool operator()(bool enabled, const Args &args) ROSE_OVERRIDE {
        if (!enabled)
            return false;
        static int ncalls = 0;
        if (1==++ncalls) {
            mlog[INFO] <<"NOTE: Operations on file descriptors will be reported as both a file descriptor and\n"
                       <<"      a file name.  The file name should be taken with a grain of salt since it is\n"
                       <<"      possible (even common) for a file to be opened and then unlinked from the fs.\n"
                       <<"      In fact, after unlinking, a new file can be created having the same name and will\n"
                       <<"      not be the file accessed by the original file descriptor.  Also, the names reported\n"
                       <<"      here are the names supplied by the specimen and are not canonical.\n";
        }

        // We're expecting to be called after the system call returns.
        const RegisterDescriptor &EAX = args.thread->policy.reg_eax;
        int32_t sysresult = args.thread->policy.readRegister<32>(EAX).known_value();
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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Process-related stuff
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Reports when a new process has been created.
class ProcessCreationHandler: public RSIM_Callbacks::ProcessCallback {
    ProcessCreationHandler* clone() ROSE_OVERRIDE { return this; }
    
    bool operator()(bool enabled, const Args &args) ROSE_OVERRIDE {
        if (!enabled)
            return false;

        RSIM_Thread *t = args.process->get_main_thread();
        switch (args.reason) {
            case FORK:
                mlog[INFO] <<"process " <<t->get_tid() <<" is forking\n";
                break;
            case START:
                mlog[INFO] <<"process " <<t->get_tid() <<" is starting\n";
                break;
            case FINISH:
                mlog[INFO] <<"process " <<t->get_tid() <<" is exiting\n";
                break;
            case COREDUMP:
                mlog[INFO] <<"process " <<t->get_tid() <<" is dumping core\n";
                break;
        }
        return true;
    }
};

int
main(int argc, char *argv[], char *envp[]) {
    // Initialize ROSE diagnostics since we're not calling ROSE frontend()
    Diagnostics::initialize();
    mlog = Sawyer::Message::Facility("tool", Diagnostics::destination);
    Diagnostics::mfacilities.insert(mlog);

    // Configure the simulator
    RSIM_Linux32 sim;
    int n = sim.configure(argc, argv, envp);
    sim.install_callback(new RSIM_Tools::UnhandledInstruction);
    sim.install_callback(new SocketSyscallHandler, RSIM_Callbacks::AFTER);
    sim.install_callback(new FilesystemSyscallHandler, RSIM_Callbacks::AFTER);
    sim.install_callback(new ProcessCreationHandler, RSIM_Callbacks::BEFORE);

    // Load the specimen to create the initial process and main thread (which does not start executing yet).
    sim.exec(argc-n, argv+n);

    // Set up signal handlers, etc.  This is optional and can be commented to make debugging easier.
    static const bool doActivate = false;
    if (doActivate)
        sim.activate();

    // Allow main thread to start running.  This returns when the simulated process terminates.
    sim.main_loop();
    sim.describe_termination(stdout);

    // Deregister signal handlers if we activated them above.  This step isn't strictly necessary here since the specimen has
    // already terminated and we're about to do the same.
    if (doActivate)
        sim.deactivate();

    sim.terminate_self();
}
#else
int main() { return 0; }
#endif
