#ifndef RSIM_Adapter_H
#define RSIM_Adapter_H

/** Adapters.
 *
 *  An adapter is an object that can be attached to a simulator to change the simulator's behavior, and then detached to
 *  restore the simulator to its original behavior.
 *
 *  An example of an adapter is TraceIO, which monitors data transfers on specified file descriptors and reports them using the
 *  tracing facility.  In order to accomplish this, the adapter augments a number of system calls by registering additional
 *  callbacks for those system calls.  Users can manually augment system calls, but using an adapter is a good way for the user
 *  to ensure that all system calls relevant to a certain kind of analysis are properly agumented.
 *
 *  Various subclasses of TraceIO add additional features, such as automatically enabling and disabling file descriptors of
 *  various types as they are created and closed.  For instance, RSIM_Adapter::TraceFileIO traces normal files by tracking
 *  sys_open(), sys_creat(), sys_close(), etc.
 *
 *  More than one adapter can be attached to a simulator, and a a single adapter can be attached to multiple simulators.  But
 *  the adapter should not be destroyed until after it's detached from all simulators.  See RSIM_Adapter::AdapterBase::attach() and
 *  RSIM_Adapter::AdapterBase::detach() for details.
 *
 *  @section RSIM_Adapter_Example1 Example: Using an adapter
 *
 *  @code
 *  RSIM_Linux32 simulator;
 *  RSIM_Adapter::TraceFileIO tracer;
 *
 *  // Activate tracing for files that are already open; other files
 *  // will be discovered when they're opened.
 *  tracer.trace_fd(0);  // standard input
 *  tracer.trace_fd(1);  // standard output
 *  tracer.trace_fd(2);  // standard error
 *
 *  // Attach the tracing adapter to the simulator in order to modify
 *  // the simulator's behavior.
 *  tracer.attach(&simulator);
 *  ...
 *
 *  // Detach the tracing adapter to restore the simulator to its
 *  // previous behavior.
 *  tracer.detach(&simulator);
 *  @endcode
 */
namespace RSIM_Adapter {

    /** Base class for all adapters. See the RSIM_Adapter namespace for details. */
    class AdapterBase {
    public:
        AdapterBase(): NOT_IMPLEMENTED(this) {}
        virtual ~AdapterBase() {}

        /** Attach this adapter to a simulator.  The adapter must not be deleted until after it's detached or the simulator has
         *  been destroyed.  In general, it is possible to attach more than one adapter to a simulator, or to attach a single
         *  adapter to multiple simulators.
         *
         *  Thread safety: Thread safety is determined by the subclasses. */
        virtual void attach(RSIM_Simulator*) = 0;

        /** Detach this adapter from a simulator.  An adapter should be detached the same number of times it was attached. When
         *  multiple adapters are attached to a simulator they can typically be detached in any order.  Detaching an adapter
         *  that isn't attached isn't usually a problem--it just doesn't do anything.
         *
         *  Thread safety: Thread safety is determined by the subclasses. */
        virtual void detach(RSIM_Simulator*) = 0;

        /** Returns a string that should be printed at the beginning of each line of output.  The default string is typically
         *  the name of the adapter followed by a colon and space.
         *
         *  Thread safety:  Thread safe. */
        std::string prefix() const {
            return prefix_str;
        }

        /** Sets the string to print at the beginning of each line of output.  All adapters should provide a reasonable default
         *  so users only need to do this if they have attached the same kind of adapter more than once and want to distinguish
         *  between the outputs.
         *
         *  Thread safety:  Not thread safe.  Do not call if other threads are concurrently accessing this adapter. */
        void prefix(const std::string &prefix) {
            prefix_str = prefix;
        }

    protected:
        /* Callback for syscalls that should be traced but aren't.  These are mostly esoteric system calls that are not yet
         * implemented in the simulator and so which would be pretty useless to implement for tracing. */
        class NotImplemented: public RSIM_Simulator::SystemCall::Callback {
        private:
            AdapterBase *adapter;
        public:
            explicit NotImplemented(AdapterBase *adapter)
                : adapter(adapter) {}
            bool operator()(bool b, const Args&);
        };

    protected:
        std::string prefix_str;
        NotImplemented NOT_IMPLEMENTED;
    };



    /**************************************************************************************************************************
     *                                  Selective Syscall Disabling Adapter
     **************************************************************************************************************************/

    /** An adapter to enable/disable specific system calls.
     *
     *  This adapter, when attached to a simulator, allows a user to enable or disable specific system calls. System calls are
     *  either enabled, disabled, or in a default state (either enabled or disabled depending on the constructor
     *  arguments). System calls are identified by call number and can be found in <asm/unistd_32.h> for the RSIM_Linux32
     *  simulator.
     *
     *  Simply speaking, whenever the specimen attempts to invoke a disabled system call the call is made to appear to fail by
     *  returning a "Function not implemented" error.  However, the adapter can be configured via a callback list which is
     *  executed when a system call is about to be skipped.
     *
     *  Here's what happens in detail when the specimen tries to invoke a disabled system call:
     *
     *  <ol>
     *    <li>That system call's "enter" callbacks are performed, possibly emiting system call tracing messages.</li>
     *    <li>A message is printed to the TRACE_MISC facility indicating that the system call is disabled.</li>
     *    <li>The system call return value is set to -ENOSYS ("Function not implemented"). The callbacks in the next step may
     *        replace this with some other return value.</li>
     *    <li>All user specified callbacks registered with this adapter (see get_callbacks()) are invoked on behalf of the
     *        system call.  The list is invoked with the Boolean value initially set to false. If this callback list returns
     *        true then:
     *        <ol>
     *          <li>the system call is immediately enabled</li>
     *          <li>a message to that effect is emitted to the TRACE_MISC facility</li>
     *          <li>the system call will eventually execute (after the remaining pre-syscall callbacks finish and return
     *              true)</li>
     *        </ol>
     *        otherwise
     *        <ol>
     *          <li>The system call remains disabled</li>
     *          <li>All syscall "leave" callbacks run, possibly tracing the syscall return value.</li>
     *        </ol>
     *        </li>
     *  </ol>
     *
     *  @section SyscallDisabler_Example1 Example: Disabling networking
     *
     *  Here's how to make it impossible for a non-root specimen to make a network connection.  The sys_socketcall()
     *  encapsulates all socket operations on x86 Linux: socket, bind, listen, connect, accept, send, recv, etc.  So disabling
     *  that one system call disables any ability for the specimen to connect to the network.
     *
     *  @code
     *  // Leave all system calls enabled by default
     *  RSIM_Adapter::SyscallDisabler no_network(true);
     *
     *  // Give the adapter a name other than SyscallDisabler. This makes the
     *  // reason-for-disabled more obvious in the tracing output.
     *  no_network.prefix("NoNetwork: ");
     *
     *  // Disable system calls that are required to open a network connection
     *  no_network.disable_syscall(102); // 102 is sys_socketcall
     *
     *  // Attach the adapter to the simulator
     *  no_network.attach(simulator);
     *  @endcode
     *
     *  The output, when the simulator is run with "--debug=syscall", may look something like this:
     *  @verbatim
 2782:1 1.892 0x401162b2[362543]:  socket[102](PF_INET, SOCK_STREAM, IPPROTO_TCP) <socket continued below>
 2782:1 1.892 0x401162b2[362543]:  NoNetwork: syscall 102 is disabled
 2782:1 1.892 0x401162b2[362543]:  <socket resumed>  = -38 ENOSYS (Function not implemented)
 2782:1 1.904 0x401059be[364084]:  write[4](2, 0xbfffb684 [Can't get socket], 16) = 16
 2782:1 1.925 0x401059be[367112]:  write[4](2, 0xbfffb23c [ : Function not implemented\n], 28) = 28
 2782:1 1.934 0x401058d7[368842]:  close[6](-1) = -9 EBADF (Bad file descriptor)
 2782:1 1.957 0x400d1bef[371854]:  exit_group[252](1) = <throwing Exit>
 2782:1 1.957 0x400d1bef[371854]:  this thread is terminating (for entire process) @endverbatim
     *
     *  @section SyscallDisabler_Example2 Example: Interactive enabling
     *
     *  By default, if the specimen attempts to invoke a disabled system call the call is simply skipped and the EAX register
     *  is adjusted to make it appear as if the error "Function not implemented" was returned.  However, the adapter allows
     *  disabled system calls to be enabled. This is accomplished by registering a callback with the adapter. The callback
     *  should return true to enable the system call.
     *
     *  This example initialially disables all system calls, and then queries the user about whether each call should be
     *  invoked as the system call is reached.  This example is not thread safe.
     *
     *  @code
     *  struct Ask: public RSIM_Simulator::SystemCall::Callback {
     *      std::set<int> asked; // syscalls we asked about
     *      bool operator()(bool syscall_enabled, const Args &args) {
     *          if (asked.find(args.callno)==asked.end()) {
     *              asked.insert(args.callno);
     *              fprintf(stderr, "System call %d is currently disabled. "
     *                      "What should I do? (s=skip, e=enable) [s] ", args.callno);
     *              char buf[200];
     *              if (fgets(buf, sizeof buf, stdin) && buf[0]=='e')
     *                  syscall_enabled = true;
     *          }
     *          return syscall_enabled;
     *      }
     *  };
     *
     *  static RSIM_Adapter::SyscallDisabler disabler(false); // disable all system calls
     *  disabler.get_callbacks().append(new Ask);             // callback for disabled syscalls
     *  disabler.attach(&sim);                                // modify the simulator's behavior
     *  @endcode
     */
    class SyscallDisabler: public AdapterBase {
    public:

        /** Constructor that sets default state.  The default state, @p dflt, is used as the state for system calls that have
         *  been neither explicitly enabled nor explicitly disabled. */
        explicit SyscallDisabler(bool dflt)
            :  syscall_cb(NULL), dflt_state(dflt) {
            prefix("SyscallDisabler: ");
        }

        virtual ~SyscallDisabler() {
            delete syscall_cb;
        }

        virtual void attach(RSIM_Simulator *sim);       /**< See base class. */
        virtual void detach(RSIM_Simulator *sim);       /**< See base class. */

        /** Enable a system call.  The optional @p state argument can be used to disable the system call (when false), or a
         *  system call can be disabled with disable_syscall().  The specified system call number, @p callno, need not reference
         *  a system call that's actually defined yet.
         *
         *  Thread safety:  This method is thread safe. */
        void enable_syscall(int callno, bool state=true);

        /** Disable a system call.  A system call can also be disabled by passing false as the @p state argument of the
         *  enable_syscall() method. The specified system call number, @p callno, need not reference a system call that's
         *  actually defined yet.
         *
         *  Thread safety:  This method is thread safe. */
        void disable_syscall(int callno) {
            enable_syscall(callno, false);
        }

        /** Set the default state of system calls.  If a system call has not been explicitly enabled or disabled, then its
         *  state is the default state set by this method or by the constructor.
         *
         *  Thread safety:  This method is thread safe. */
        void set_default(bool state);

        /** Get the state of a system call. This method returns true if the system call is enabled, false if not enabled.
         *
         *  Thread safety:  This method is thread safe. */
        bool is_enabled(int callno) const;

        /** Callbacks for disabled system calls.  This list initially has a single item: a functor that invokes the normal
         *  system call "enter" and "leave" boilerplate but replaces the body with code that causes the system call to return
         *  -ENOSYS ("Function not implemented").  The user can modify this list as they desire, perhaps to return a different
         *  value or to do some other system call.
         *
         *  Thread safety: This method is thread safe and most methods on the returned object are also thread safe. */
        Rose::Callbacks::List<RSIM_Simulator::SystemCall::Callback> &get_callbacks() {
            return cblist;
        }

    protected:
        /** Callback for all system calls. */
        class SyscallCB: public RSIM_Callbacks::SyscallCallback {
        private:
            SyscallDisabler *adapter;
        public:
            explicit SyscallCB(SyscallDisabler *adapter)
                : adapter(adapter) {}
            virtual SyscallCB *clone() { return this; }
            virtual bool operator()(bool b, const Args&);
        };

    protected:
        /* Non mutex-protected data members */
        SyscallCB *syscall_cb;
        mutable SAWYER_THREAD_TRAITS::RecursiveMutex mutex;
        Rose::Callbacks::List<RSIM_Simulator::SystemCall::Callback> cblist;
        
        /* Mutex-protected data members */
        bool dflt_state;
        std::map<int/*callno*/, bool/*state*/> syscall_state;
    };
    

    /**************************************************************************************************************************
     *                                  I/O Tracing Adapter
     **************************************************************************************************************************/

    /** I/O Tracing Adapter.
     *
     *  This adapter augments system calls related to I/O on file descriptors so that whenever data is transfered over a
     *  descriptor the data is also displayed in the TRACE_MISC tracing facility.  Because this adapter prints data that is
     *  transfered, the underlying system call must be allowed to run before the transfer is logged, otherwise the adapter
     *  would not know how much data was actually transfered.
     *
     *  This adapter has one function to activate/deactivate file descriptors, trace_fd(), but does not manipulate the set of
     *  traced descriptors itself.  Subclasses of TraceIO are expected to activate/deactivate file descriptors based on other
     *  system calls that typically don't perform I/O, such as open(), creat(), and close(). */
    class TraceIO: public AdapterBase {
    public:
        HexdumpFormat hd_format;                                /**< Format to use for hexdump() */

    public:
        TraceIO()
            : read_cb(this, "input"), write_cb(this, "output"),
              readv_cb(this, "input"), writev_cb(this, "output"),
              mmap_cb(this), ftruncate_cb(this) {
            prefix("TraceIO: ");
            hd_format.prefix = "    ";
            hd_format.multiline = true;
        }

        virtual void attach(RSIM_Simulator *sim);       /**< See base class. */
        virtual void detach(RSIM_Simulator *sim);       /**< See base class. */

        /** Determines if a file descriptor is currently being traced.  Returns true if the specified file descriptor is being
         *  traced. That is, if data transfers on that file descriptor are logged.
         *
         *  Thread safety:  This method is thread safe. */
        bool is_tracing_fd(int fd);

        /** Enables or disables tracing for a file descriptor.
         *
         *  Thread safety:  This method is thread safe. */
        void trace_fd(int fd, bool how=true);

    protected:
        /* Callback for sys_read (#3), sys_write (#4), or any other system call that takes a file descriptor as the first
         * argument, a buffer as the second argument, and returns the number of bytes transfered.  This callback checks to see
         * if the file descriptor is one that's being traced. If it is, and the system call transfered data (returned positive)
         * then grab the data and dump it to the tracer's output stream. */
        class ReadWriteSyscall: public RSIM_Simulator::SystemCall::Callback {
        private:
            TraceIO *tracer;
            std::string label;
        public:
            explicit ReadWriteSyscall(TraceIO *tracer, const std::string &label)
                : tracer(tracer), label(label) {}
            bool operator()(bool b, const Args&);
        };

        /* Callback for vector I/O sys_readv (#145) and sys_writev (#146). If the syscall returns a positive value, then we
         * traverse the iov array to read buffer addresses and sizes. We use the iov array to grab each buffer and dump it
         * using hexdump. */
        class ReadWriteVectorSyscall: public RSIM_Simulator::SystemCall::Callback {
        private:
            TraceIO *tracer;
            std::string label;
        public:
            explicit ReadWriteVectorSyscall(TraceIO *tracer, const std::string &label)
                : tracer(tracer), label(label) {}
            bool operator()(bool b, const Args&);
        };

        /* Callback for sys_mmap2 (#90). If the fd argument is non-negative then print a message that we're not tracing mapped
         * I/O for that file. */
        class MmapSyscall: public RSIM_Simulator::SystemCall::Callback {
        private:
            TraceIO *tracer;
        public:
            explicit MmapSyscall(TraceIO *tracer)
                : tracer(tracer) {}
            bool operator()(bool b, const Args&);
        };

        /* Callback for sys_ftruncate prints the file descriptor and the truncation position. */
        class FtruncateSyscall: public RSIM_Simulator::SystemCall::Callback {
        private:
            TraceIO *tracer;
        public:
            explicit FtruncateSyscall(TraceIO *tracer)
                : tracer(tracer) {}
            bool operator()(bool b, const Args&);
        };

    protected:
        std::set<int32_t> tracefd;                              /* Set of file descriptors being traced. */
        SAWYER_THREAD_TRAITS::RecursiveMutex mutex;             /* Protects tracefd */
        ReadWriteSyscall read_cb, write_cb;                     /* Callbacks for sys_read, sys_write, etc. */
        ReadWriteVectorSyscall readv_cb, writev_cb;             /* Callbacks for sys_readv and sys_writev */
        MmapSyscall mmap_cb;                                    /* Callback for sys_mmap and sys_mmap2 */
        FtruncateSyscall ftruncate_cb;                          /* Callback for sys_ftruncate */
    };



    /**************************************************************************************************************************
     *                                  File I/O Tracing Adapter
     **************************************************************************************************************************/

    /** File I/O tracing adapter.
     *
     *  This adapter is a specialization of TraceIO for tracing data transfers to and from files.  It augments TraceIO by
     *  hooking into sys_open(), sys_creat(), sys_dup(), sys_dup2(), and sys_close() to automatically activate/deactivate
     *  tracing of particular file descriptors.
     *
     *  Since file descriptors 0, 1, and 2 are typically open already, the user will probably need to explicitly activate these
     *  by invoking trace_fd().  Similarly for any other file descriptors that might already be open.
     *
     *  @section TraceFileIO_Example1 Example
     *
     *  The following code causes the simulator to notice when a new file is opened and will produce some I/O tracing on the
     *  TRACE_MISC facility.
     *
     *  @code
     *  RSIM_Linux32 sim;
     *  RSIM_Adapter::TraceFileIO tracer;
     *  tracer.attach(&sim);
     *  @endcode
     *
     *  If the simulator was configured with "--debug=syscall", then the output might look like this:
     *  @verbatim
3621:1 0.147 0x4001759d[22405]:   close[6](7) <close continued below>
3621:1 0.147 0x4001759d[22405]:   TraceFileIO: deactivating fd=7
3621:1 0.147 0x4001759d[22405]:   <close resumed>  = 0
3621:1 0.150 0x400176a1[22670]:   access[33]("/etc/ld.so.nohwcap", 0) = -2 ENOENT (No such file or directory)
3621:1 0.160 0x40017564[24140]:   open[5]("/lib32/libc.so.6", 0, <unused>) <open continued below>
3621:1 0.160 0x40017564[24140]:   TraceFileIO: activating fd=7
3621:1 0.160 0x40017564[24140]:   <open resumed>  = 7
3621:1 0.161 0x400175e4[24161]:   read[3](7, 0xbfffd624, 512) <read continued below>
3621:1 0.161 0x400175e4[24161]:   TraceFileIO: input fd=7, nbytes=512:
    0x00000000: 7f 45 4c 46 01 01 01 00  00 00 00 00 00 00 00 00 |.ELF............|
    0x00000010: 03 00 03 00 01 00 00 00  5e 65 01 00 34 00 00 00 |........^e..4...|
    0x00000020: 74 f6 14 00 00 00 00 00  34 00 20 00 0a 00 28 00 |t.......4. ...(.|
    0x00000030: 43 00 42 00 06 00 00 00  34 00 00 00 34 00 00 00 |C.B.....4...4...|
    0x00000040: 34 00 00 00 40 01 00 00  40 01 00 00 05 00 00 00 |4...@...@.......|
    0x00000050: 04 00 00 00 03 00 00 00  80 79 13 00 80 79 13 00 |.........y...y..|
    0x00000060: 80 79 13 00 13 00 00 00  13 00 00 00 04 00 00 00 |.y..............|
    0x00000070: 01 00 00 00 01 00 00 00  00 00 00 00 00 00 00 00 |................|
    0x00000080: 00 00 00 00 98 b2 14 00  98 b2 14 00 05 00 00 00 |................|
    0x00000090: 00 10 00 00 01 00 00 00  fc c1 14 00 fc c1 14 00 |................|
    0x000000a0: fc c1 14 00 a0 27 00 00  74 54 00 00 06 00 00 00 |.....'..tT......|
    0x000000b0: 00 10 00 00 02 00 00 00  9c dd 14 00 9c dd 14 00 |................|
    0x000000c0: 9c dd 14 00 f0 00 00 00  f0 00 00 00 06 00 00 00 |................|
    0x000000d0: 04 00 00 00 04 00 00 00  74 01 00 00 74 01 00 00 |........t...t...|
    0x000000e0: 74 01 00 00 20 00 00 00  20 00 00 00 04 00 00 00 |t... ... .......|
    0x000000f0: 04 00 00 00 07 00 00 00  fc c1 14 00 fc c1 14 00 |................|
    0x00000100: fc c1 14 00 08 00 00 00  2c 00 00 00 04 00 00 00 |........,.......|
    0x00000110: 04 00 00 00 50 e5 74 64  94 79 13 00 94 79 13 00 |....P.td.y...y..|
    0x00000120: 94 79 13 00 cc 2a 00 00  cc 2a 00 00 04 00 00 00 |.y...*...*......|
    0x00000130: 04 00 00 00 51 e5 74 64  00 00 00 00 00 00 00 00 |....Q.td........|
    0x00000140: 00 00 00 00 00 00 00 00  00 00 00 00 06 00 00 00 |................|
    0x00000150: 04 00 00 00 52 e5 74 64  04 c2 14 00 fc c1 14 00 |....R.td........|
    0x00000160: fc c1 14 00 88 1c 00 00  80 1c 00 00 04 00 00 00 |................|
    0x00000170: 01 00 00 00 04 00 00 00  10 00 00 00 01 00 00 00 |................|
    0x00000180: 47 4e 55 00 00 00 00 00  02 00 00 00 06 00 00 00 |GNU.............|
    0x00000190: 08 00 00 00 f3 03 00 00  0a 00 00 00 00 02 00 00 |................|
    0x000001a0: 0e 00 00 00 a0 30 10 44  80 20 02 01 8c 03 e6 90 |.....0.D. ......|
    0x000001b0: 41 45 88 00 84 00 08 00  41 80 00 40 c0 80 00 0c |AE......A..@....|
    0x000001c0: 02 0c 00 01 30 00 08 40  22 08 a6 04 88 48 36 6c |....0..@"....H6l|
    0x000001d0: a0 16 30 00 26 84 80 8e  04 08 42 24 02 0c a6 a4 |..0.&.....B$....|
    0x000001e0: 1a 06 63 c8 00 c2 20 01  c0 00 52 00 21 81 08 04 |..c... ...R.!...|
    0x000001f0: 0a 20 20 a8 14 00 14 28  60 00 00 50 a0 ca 44 42 |.  ....(`..P..DB|
3621:1 0.161 0x400175e4[24161]:   <read resumed>  = 512
3621:1 0.161 0x400175e4[24161]:       result arg1 = 0xbfffd624 [\177ELF\001\001\001\000\000]...
@endverbatim
     */
    class TraceFileIO: public TraceIO {
    public:
        TraceFileIO()
            : open_cb(this), dup_cb(this), pipe_cb(this), close_cb(this) {
            prefix("TraceFileIO: ");
        }

        virtual void attach(RSIM_Simulator *sim);               /**< See base class. */
        virtual void detach(RSIM_Simulator *sim);               /**< See base class. */

    protected:
        /* Callback for sys_open (#5) and sys_creat (#8), which return a new file descriptor that should be traced.  The file
         * descriptor is added to the set of file descriptors being traced. */
        class OpenSyscall: public RSIM_Simulator::SystemCall::Callback {
        private:
            TraceIO *tracer;
        public:
            explicit OpenSyscall(TraceIO *tracer)
                : tracer(tracer) {}
            bool operator()(bool b, const Args&);
        };

        /* Callback for sys_dup (#41) and sys_dup2 (#63), which return a new file descriptor that should be traced if and only
         * if the file descriptor specified by the first argument is traced. */
        class DupSyscall: public RSIM_Simulator::SystemCall::Callback {
        private:
            TraceIO *tracer;
        public:
            explicit DupSyscall(TraceIO *tracer)
                : tracer(tracer) {}
            bool operator()(bool b, const Args&);
        };

        /* Callback for sys_pipe (#42) which returns two new file descriptors in a 2-element array pointed to by the first
         * argument. */
        class PipeSyscall: public RSIM_Simulator::SystemCall::Callback {
        private:
            TraceIO *tracer;
        public:
            explicit PipeSyscall(TraceIO *tracer)
                : tracer(tracer) {}
            bool operator()(bool b, const Args&);
        };

        /* Callback for sys_close (#6) or any other system call that closes a file descriptor which is provided as the first
         * argument. */
        class CloseSyscall: public RSIM_Simulator::SystemCall::Callback {
        private:
            TraceIO *tracer;
        public:
            explicit CloseSyscall(TraceIO *tracer)
                : tracer(tracer) {}
            bool operator()(bool b, const Args&);
        };

    private:
        OpenSyscall open_cb;
        DupSyscall dup_cb;
        PipeSyscall pipe_cb;
        CloseSyscall close_cb;
    };

    /**************************************************************************************************************************
     *                                  TCP Network Tracing Adapter
     **************************************************************************************************************************/

    /** Not fully implemented yet. [RPM 2011-04-15] */
    class TraceTcpIO: public TraceIO {
    public:
        TraceTcpIO() {
            prefix("TraceTcpIO: ");
        }

        virtual void attach(RSIM_Simulator *sim);               /**< See base class. */
        virtual void detach(RSIM_Simulator *sim);               /**< See base class. */

    protected:
    };
};

#endif /* !RSIM_Adapter_H !*/
