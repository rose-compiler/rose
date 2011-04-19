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
 *  the adapter should not be destroyed until after it's detached from all simulators.  See RSIM_Adapter::Base::attach() and
 *  RSIM_Adapter::Base::detach() for details.
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
    class Base {
    public:
        Base(): NOT_IMPLEMENTED(this) {}
        virtual ~Base() {}

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
            Base *adapter;
        public:
            NotImplemented(Base *adapter)
                : adapter(adapter) {}
            bool operator()(bool b, const Args&);
        };

    protected:
        std::string prefix_str;
        NotImplemented NOT_IMPLEMENTED;
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
    class TraceIO: public Base {
    public:
        HexdumpFormat hd_format;                                /**< Format to use for hexdump() */

    public:
        TraceIO()
            : read_cb(this, "input"), write_cb(this, "output"),
              readv_cb(this, "input"), writev_cb(this, "output"),
              mmap_cb(this), ftruncate_cb(this) {
            pthread_mutex_init(&mutex, NULL);
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
            ReadWriteSyscall(TraceIO *tracer, const std::string &label)
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
            ReadWriteVectorSyscall(TraceIO *tracer, const std::string &label)
                : tracer(tracer), label(label) {}
            bool operator()(bool b, const Args&);
        };

        /* Callback for sys_mmap2 (#90). If the fd argument is non-negative then print a message that we're not tracing mapped
         * I/O for that file. */
        class MmapSyscall: public RSIM_Simulator::SystemCall::Callback {
        private:
            TraceIO *tracer;
        public:
            MmapSyscall(TraceIO *tracer)
                : tracer(tracer) {}
            bool operator()(bool b, const Args&);
        };

        /* Callback for sys_ftruncate prints the file descriptor and the truncation position. */
        class FtruncateSyscall: public RSIM_Simulator::SystemCall::Callback {
        private:
            TraceIO *tracer;
        public:
            FtruncateSyscall(TraceIO *tracer)
                : tracer(tracer) {}
            bool operator()(bool b, const Args&);
        };

    protected:
        std::set<int32_t> tracefd;                              /* Set of file descriptors being traced. */
        pthread_mutex_t mutex;                                  /* Protects tracefd */
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
     *  by invoking trace_fd().  Similarly for any other file descriptors that might already be open. */
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
            OpenSyscall(TraceIO *tracer)
                : tracer(tracer) {}
            bool operator()(bool b, const Args&);
        };

        /* Callback for sys_dup (#41) and sys_dup2 (#63), which return a new file descriptor that should be traced if and only
         * if the file descriptor specified by the first argument is traced. */
        class DupSyscall: public RSIM_Simulator::SystemCall::Callback {
        private:
            TraceIO *tracer;
        public:
            DupSyscall(TraceIO *tracer)
                : tracer(tracer) {}
            bool operator()(bool b, const Args&);
        };

        /* Callback for sys_pipe (#42) which returns two new file descriptors in a 2-element array pointed to by the first
         * argument. */
        class PipeSyscall: public RSIM_Simulator::SystemCall::Callback {
        private:
            TraceIO *tracer;
        public:
            PipeSyscall(TraceIO *tracer)
                : tracer(tracer) {}
            bool operator()(bool b, const Args&);
        };

        /* Callback for sys_close (#6) or any other system call that closes a file descriptor which is provided as the first
         * argument. */
        class CloseSyscall: public RSIM_Simulator::SystemCall::Callback {
        private:
            TraceIO *tracer;
        public:
            CloseSyscall(TraceIO *tracer)
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
