#ifndef ROSE_RSIM_Process_H
#define ROSE_RSIM_Process_H

class RSIM_Thread;
class RSIM_Simulator;

/** Represents a single simulated process. The process object holds resources that are shared among its threads. Some of the
 *  properties of a simulated process (such as PID) are shared with the real process (the process running the simulator).
 *
 *  Thread safety: Since an RSIM_Process may contain multiple RSIM_Thread objects and each RSIM_Thread is matched by a real
 *  thread, many of the RSIM_Process methods must be thread safe. */
class RSIM_Process {
public:
    /** Creates an empty process containing no threads. */
    explicit RSIM_Process(RSIM_Simulator *simulator)
        : simulator(simulator), tracing_file(NULL), tracing_flags(0),
          map(NULL), brk_va(0), mmap_start(0x40000000ul), mmap_recycle(false), disassembler(NULL),
          interpretation(NULL), ep_orig_va(0), ep_start_va(0),
          terminated(false), termination_status(0), core_flags(0), btrace_file(NULL),
          vdso_mapped_va(0), vdso_entry_va(0),
          core_styles(CORE_ELF), core_base_name("x-core.rose"), ld_linux_base_va(0x40000000) {
        RTS_rwlock_init(&instance_rwlock, RTS_LAYER_RSIM_PROCESS_OBJ, NULL);
        ctor();
    }

    RSIM_Simulator *get_simulator() const {
        return simulator;
    }

private:
    RSIM_Simulator *simulator;
    void ctor();


    /**************************************************************************************************************************
     *                                  Thread synchronization
     **************************************************************************************************************************/
private:
    mutable RTS_rwlock_t instance_rwlock;       /**< One read-write lock per object.  See rwlock(). */

public:

    //@{
    /** Returns the read-write lock for this object.
     *
     *  Although most RSIM_Process methods are already thread safe, it is sometimes necessary to protect access to data members
     *  This method returns a per-object read-write lock that can be used with the usual ROSE Thread Support macros, RTS_READ
     *  and RTS_WRITE.  The returned lock is the same lock as the inherently thread-safe methods of this class already use.
     *  See RTS_rwlock_rdlock() and RTS_rwlock_wrlock() for a description of the semantics.
     *
     *  These locks should be held for as little time as possible, and certainly not over a system call that might block. */
    RTS_rwlock_t &rwlock() {
        return instance_rwlock;
    }
    RTS_rwlock_t &rwlock() const {
        return instance_rwlock;
    }
    //@}



    /**************************************************************************************************************************
     *                                  Tracing and debugging
     **************************************************************************************************************************/
private:
    std::string tracing_file_name;      /**< Pattern for trace file names. May include %d for thread ID. */
    FILE *tracing_file;                 /**< Stream to which debugging output is sent (or NULL to suppress it) */
    unsigned tracing_flags;             /**< Bit vector of what to trace. See TraceFlags. */
    RSIM_Callbacks callbacks;           /**< Callbacks for a process, and to initialize callbacks of new threads. */

public:

    /** Sets the pattern to use to generate a file name for trace output.  The name should be a printf-style format string with
     *  an optional %d conversion which is replaced by the process ID. */
    void set_tracing_name(const std::string &s) {
        tracing_file_name = s;
    }

    /** Returns the pattern used to generate trace file names. */
    std::string get_tracing_name(void) const {
        return tracing_file_name;
    }

    /** Initialize tracing by (re)opening the trace file with the name pattern that was specified with set_trace_name().  The
     *  pattern should be a printf-style format with an optional integer specifier for the thread ID. */
    void open_tracing_file();

    /** Returns a file for tracing, or NULL if tracing is disabled. All trace facilities use the same file.
     *
     *  Thread safety:  This method is thread safe; it can be invoked on a single object by multiple threads concurrently. */
    FILE *get_tracing_file() const;

    /** Sets tracing file and facilities. */
    void set_tracing(FILE*, unsigned flags);
    
    /** Returns a bit mask describing what should be traced by new threads created for this process. The return value is the
     * bitwise OR of the facilityBitMask() for each enabled facility. */
    unsigned get_tracing_flags() const;

    //@{
    /** Obtain the set of callbacks for this object.  Many of the process callbacks are used to initialize thread callbacks
     *  when a new thread is created.
     *
     *  Thread safety:  This method is thread safe.  Furthermore, most methods of the returned object are also thread safe. */
    RSIM_Callbacks &get_callbacks() {
        return callbacks;
    }
    const RSIM_Callbacks &get_callbacks() const {
        return callbacks;
    }
    //@}

    /** Set all callbacks for this process.  Note that callbacks can be added or removed individually by invoking methods on
     *  the callback object return by get_callbacks().
     *
     *  Thread safety:  This method is thread safe. */
    void set_callbacks(const RSIM_Callbacks &cb);

    /**************************************************************************************************************************
     *                                  Process memory
     **************************************************************************************************************************/
private:
    MemoryMap *map;                             /**< Describes how specimen's memory is mapped to simulator memory */
    rose_addr_t brk_va;                         /**< Current value for brk() syscall; initialized by load() */
    rose_addr_t mmap_start;                     /**< Minimum address to use when looking for mmap free space */
    bool mmap_recycle;                          /**< If false, then never reuse mmap addresses */

public:

    /** Returns the memory map for the simulated process.  MemoryMap is not thread safe [as of 2011-03-31], so all access to
     *  the map should be protected by the process-wide read-write lock returned by the rwlock() method. */
    MemoryMap *get_memory() const {
        return map;
    }

    /** Add a memory mapping to a specimen.  The new mapping starts at specimen address @p va (zero causes this method to
     *  choose an appropriate address) for @p size bytes.  The @p rose_perms are the MemoryMap::Protection bits, @p flags are
     *  the same as for mmap() and are defined in <sys/mman.h>, and @p offset and @p fd are as for mmap().
     *
     *  Thread safety:  This method is thread safe; it can be invoked on a single object by multiple threads concurrently. */
    rose_addr_t mem_map(rose_addr_t va, size_t size, unsigned rose_perms, unsigned flags, size_t offset, int fd);

    /** Set the process brk value and adjust the specimen's memory map accordingly.  The return value is either a negative
     *  error number (such as -ENOMEM) or the new brk value.  The optional @p mesg pointer will be used to show the memory map
     *  after it is adjusted.
     *
     *  Thread safety:  This method is thread safe; it can be invoked on a single object by multiple threads concurrently. */
    int mem_setbrk(rose_addr_t newbrk, RTS_Message *mesg=NULL);

    /** Unmap some specimen memory.  The starting virtual address, @p va, and number of bytes, @p sz, need not be page
     *  aligned, but if they are then the real munmap() is also called, substituting the real address for @p va.  The return
     *  value is a negative error number on failure, or zero on success.  The optional @p mesg pointer will be used to show the
     *  memory map after it is adjusted.
     *
     *  Thread safety:  This method is thread safe; it can be invoked on a single object by multiple threads concurrently. */
    int mem_unmap(rose_addr_t va, size_t sz, RTS_Message *mesg=NULL);

    /** Change protection bits on part of the specimen virtual memory.  The @p rose_perms specify the desired permission bits
     *  to set in the specimen's memory map (in terms of MemoryMap::Protection bits) while the @p real_perms are the desired
     *  permissions of the real underlying memory in the simulator itself (using constants from <sys/mman.h>.
     * 
     *  The return value is a negative error number on failure, or zero on success.
     *
     *  Thread safety:  This method is thread safe; it can be invoked on a single object by multiple threads concurrently. */
    int mem_protect(rose_addr_t va, size_t sz, unsigned rose_perms, unsigned real_perms);
    
    /** Dump a memory map description to the specified message object.  If @p mesg is a null pointer then this method does
     *  nothing.  The @p intro is an optional message to be printed before the map (it should include a newline character), and
     *  @p prefix is an optional string to print before each line of the map (defaulting to four spaces).
     *
     *  Thread safety:  This method is thread safe; it can be invoked on a single object by multiple threads concurrently. */
    void mem_showmap(RTS_Message *mesg, const char *intro=NULL, const char *prefix=NULL);

    /** Returns true if the specified specimen virtual address is mapped; false otherwise.
     *
     *  Thread safety:  This method is thread safe; it can be invoked on a single object by multiple threads concurrently. */
    bool mem_is_mapped(rose_addr_t va) const;

    /** Returns the memory address in ROSE where the specified specimen address is located.
     *
     *  Thread safety: This method is thread safe; it can be invoked on a single object by multiple threads
     *  concurrently. However, the address that is returned might be unmapped before the caller can do anything with it. */
    void *my_addr(uint32_t va, size_t size);

    /** Does the opposite, more or less, of my_addr(). Return a specimen virtual address that maps to the specified address in
     *  the simulator.  There may be more than one, in which case we return the lowest.
     *
     *  Thread safety:  This method is thread safe; it can be invoked on a single object by multiple threads concurrently. */
    uint32_t guest_va(void *addr, size_t nbytes);

    /** Copies data into the specimen address space.  Copies up to @p size bytes from @p buf into specimen memory beginning at
     *  virtual address @p va.  If the requested number of bytes cannot be copied because (part of) the destination address
     *  space is not mapped or because (part of) the destination address space does not have write permission, the this method
     *  will write as much as possible up to the first invalid address.  The return value is the number of bytes copied.
     *
     *  Thread safety:  This method is thread safe; it can be invoked on a single object by multiple threads concurrently. */
    size_t mem_write(const void *buf, rose_addr_t va, size_t size);

    /** Copies data from specimen address space.  Copies up to @p size bytes from the specimen memory beginning at virtual
     *  address @p va into the beginning of @p buf.  If the requested number of bytes cannot be copied because (part of) the
     *  destination address space is not mapped or because (part of) the destination address space does not have read
     *  permission, then this method will read as much as possible up to the first invalid address.  The return value is the
     *  number of bytes copied.
     *
     *  Thread safety:  This method is thread safe; it can be invoked on a single object by multiple threads
     *  concurrently. */
    size_t mem_read(void *buf, rose_addr_t va, size_t size);

    /** Reads a NUL-terminated string from specimen memory. The NUL is not included in the string.  If a limit is specified
     *  then the returned string will contain at most this many characters (a value of zero implies no limit).  If the string
     *  cannot be read, then "error" (if non-null) will point to a true value and the returned string will include the
     *  characters up to the error.
     *
     *  Thread safety:  This method is thread safe; it can be invoked on a single object by multiple threads concurrently. */
    std::string read_string(uint32_t va, size_t limit=0, bool *error=NULL);

    /** Reads a null-terminated vector of pointers to NUL-terminated strings from specimen memory.  If some sort of
     *  segmentation fault or bus error would occur, then set *error to true and return all that we read, otherwise set it to
     *  false.
     *
     *  Thread safety:  This method is thread safe; it can be invoked on a single object by multiple threads concurrently. */
    std::vector<std::string> read_string_vector(uint32_t va, bool *error=NULL);



    /**************************************************************************************************************************
     *                                  Segment registers
     **************************************************************************************************************************/
public:
    /** Set a global descriptor table entry.  This should only be called via RSIM_Thread::set_gdt(). In Linux, three of the GDT
     *  entries (GDT_ENTRY_TLS_MIN through GDT_ENTRY_TLS_MAX) are updated from the thread_struct every time a thread is
     *  scheduled.  The simulator works a bit differently since all threads are effectively always running. The simulator keeps
     *  a single GDT in the RSIM_Process but threads always access it through an RSIM_Thread object. This allows each
     *  RSIM_Thread to override the TLS-related entries.
     *
     *  Thread safety:  This method is thread safe; it can be invoked on a single object by multiple threads concurrently. */
    void set_gdt(const user_desc_32 *ud);

    /** Returns a pointer to the segment descriptor in the GDT. */
    user_desc_32 *gdt_entry(int idx);

    static const int GDT_ENTRIES = 8192;                     /**< Number of GDT entries. */
    static const int GDT_ENTRY_TLS_MIN = 6;                  /**< First TLS entry */
    static const int GDT_ENTRY_TLS_ENTRIES = 3;              /**< Number of TLS entries */
    static const int GDT_ENTRY_TLS_MAX = GDT_ENTRY_TLS_MIN + GDT_ENTRY_TLS_ENTRIES - 1; /**< Last TLS entry */

private:
    /**< Global descriptor table. Entries GDT_ENTRY_TLS_MIN through GDT_ENTRY_TLS_MAX are unused. */
    user_desc_32 gdt[GDT_ENTRIES];


    
    /**************************************************************************************************************************
     *                                  Instructions and disassembly
     **************************************************************************************************************************/
private:
    Disassembler *disassembler;                 /**< Disassembler to use for obtaining instructions */
    Disassembler::InstructionMap icache;        /**< Cache of disassembled instructions */

public:
    /** Disassembles the instruction at the specified virtual address. For efficiency, instructions are cached by the
     *  process. Instructions are removed from the cache (but not deleted) when the memory at the instruction address changes.
     *
     *  Thread safety:  This method is thread safe; it can be invoked on a single object by multiple threads concurrently. */
    SgAsmInstruction *get_instruction(rose_addr_t va);

    /** Disassembles an entire process based on the current memory map, returning a pointer to a SgAsmBlock AST node and
     *  inserting all new instructions into the instruction cache used by get_instruction().
     *
     *  Thread safety:  This method is thread safe; it can be invoked on a single object by multiple threaads concurrently.
     *  The callers are serialized and each caller will generate a new AST that does not share nodes with any AST returned by
     *  any previous call by this thread or any other. */
    SgAsmBlock *disassemble();

    /** Returns the disassembler that is being used to obtain instructions. This disassembler is chosen automatically when the
     *  specimen is loaded.
     *
     *  Thread safety:  This method is thread safe; it can be invoked on a single object by multiple threads
     *  concurrently. However, the disassembler object which is returned can probably not be used concurrently by multiple
     *  threads. See documentation for Disassembler for thread safety details. */
    Disassembler *get_disassembler() const {
        return disassembler;
    }

    /** Returns the total number of instructions processed across all threads.
     *
     *  Thread safety:  This method is thread safe; it can be invoked on a single object by multiple threads concurrently. */
    size_t get_ninsns() const;



    /**************************************************************************************************************************
     *                                  Signal handling
     **************************************************************************************************************************/
private:
    /** Simulated actions for signal handling. Array element N is signal N+1. All threads of a process share signal
     *  dispositions (see CLONE_SIGHAND bit). */
    sigaction_32 signal_action[_NSIG];

    /** The asynchronous signal reception queue.  Signals are pushed onto the tail of this queue asynchronously by
     *  signal_enqueue(), and removed synchronously from the head by signal_dequeue(). */
    struct AsyncSignalQueue {
        AsyncSignalQueue()
            : head(0), tail(0) {
            memset(info, 0, sizeof info); /* only for debugging */
        }
        static const size_t size = 128; /**< Up to 127 signals can be on the queue, plus one guard entry */
        RSIM_SignalHandling::siginfo_32 info[size];
        size_t head;                    /**< Index of oldest signal */
        size_t tail;                    /**< One beyond index of youngest signal (incremented asynchronously) */
    } sq;

public:
    /** Signals that have arrived for the process-as-a-whole which cannot be delivered to any thread because all threads have
     *  these signals masked. Signals are placed onto this queue by sys_kill() (when necessary) and removed from it by each
     *  thread using RSIM_Thread::signal_dequeue(). */
    RSIM_SignalHandling sighand;

    /** Simulates a sigaction() system call.  Returns zero on success; negative errno on failure.
     *
     *  Thread safety: This method is thread safe. */
    int sys_sigaction(int signo, const sigaction_32 *new_action, sigaction_32 *old_action);

    /** Simulates a kill() system call. Returns zero on success; negative errno on failure.
     *
     *  Thread safety: This method is thread safe. */
    int sys_kill(pid_t pid, const RSIM_SignalHandling::siginfo_32&);

    /** Signal queue used for asynchronous reception of signals from other processes. Signal numbers are pushed onto the tail
     *  of this queue by signal_enqueue(), called from the RSIM_Simulator::signal_receiver() signal handler.  Therefore, this
     *  function must be async signal safe. */
    void signal_enqueue(const RSIM_SignalHandling::siginfo_32&);

    /** Removes one signal from the queue.  Returns the oldest signal on the queue, or zero if the queue is empty.
     *
     *  Thread safety: This method is thread safe. */
    int signal_dequeue(RSIM_SignalHandling::siginfo_32 *info/*out*/);

    /** Assigns process-wide signals to threads.
     *
     *  Thread safety: This method is thread safe. */
    void signal_dispatch();



    /***************************************************************************************************************************
     *                                  Thread creation/join simulation
     ***************************************************************************************************************************/
private:
    /**< Contains various things that are needed while we clone a new thread to handle a simulated clone call. */
    struct Clone {
        Clone(RSIM_Process *process, unsigned flags, uint32_t parent_tid_va, uint32_t child_tls_va, const pt_regs_32 &regs)
            : process(process), flags(flags), newtid(-1), seq(-1),
              parent_tid_va(parent_tid_va), child_tls_va(child_tls_va), regs(regs) {
            RTS_mutex_init(&mutex, RTS_LAYER_RSIM_PROCESS_CLONE_OBJ, NULL);
            pthread_cond_init(&cond, NULL);
        }
        RTS_mutex_t mutex;                      /**< Protects entire structure. */
        pthread_cond_t  cond;                   /**< For coordinating between creating thread and created thread. */
        RSIM_Process    *process;               /**< Process creating the new thread. */
        unsigned        flags;                  /**< Various CLONE_* flags passed to the clone system call. */
        pid_t           newtid;                 /**< Created thread's TID filled in by clone_thread_helper(); negative on error */
        int             seq;                    /**< Sequence number for new thread, used for debugging. */
        uint32_t        parent_tid_va;          /**< Optional address at which to write created thread's TID; clone() argument */
        uint32_t        child_tls_va;           /**< Address of TLS user_desc_32 to load into GDT; clone() argument */
        pt_regs_32      regs;                   /**< Initial registers for child thread. */
    };
    static Clone clone_info;
    
    /** Helper to create a new simulated thread and corresponding real thread. Do not call this directly; call clone_thread()
     *  instead.  Thread creation is implemented in two parts: clone_thread() is the main entry point and is called by the
     *  thread that wishes to create a new thread, and clone_thread_helper() is the part run by the new thread.
     *
     *  We need to do a little dancing to return the ID of the new thread to the creating thread.  This is where the
     *  clone_mutex, clone_cond, and clone_newtid class data members are used.  The creating thread blocks on the clone_cond
     *  condition variable while the new thread fills in clone_newtid with its own ID and then signals the condition
     *  variable. The clone_mutex is only used to protect the clone_newtid. */
    static void *clone_thread_helper(void *process);

    /** Create a new thread.  This should be called only by the real thread which will be simulating the specimen's
     * thread. Each real thread should simulate a single specimen thread. This is normally invoked by clone_thread_helper().
     *
     *  Thread safety: This method is thread safe; it can be invoked on a single object by multiple threads concurrently. */
    RSIM_Thread *create_thread();

public:
    /** Creates a new simulated thread and corresponding real thread.  Returns the ID of the new thread, or a negative errno.
     *  The @p parent_tid_va and @p child_tid_va are optional addresses at which to write the new thread's TID if the @p flags
     *  contain the CLONE_PARENT_TID and/or CLONE_CHILD_TID bits.  We gaurantee that the TID is written to both before the
     *  simulated child starts executing.  The @p child_tls_va also points to a segment descriptor if the CLONE_SETTLS bit is
     *  set.  The @p regs are the values with which to initialize the new threads registers.
     *
     *  Thread safety: This method is thread safe; it can be invoked on a single object by multiple threads concurrently. Each
     *  calling thread should pass its own RSIM_Thread as the first argument. */
    pid_t clone_thread(RSIM_Thread*, unsigned flags, uint32_t parent_tid_va, uint32_t child_tls_va, const pt_regs_32 &regs);

    /** Returns the thread having the specified thread ID, or null if there is no thread with the specified ID.  Thread objects
     *  are never deleted while a simulator is running, but they are unlinked from their parent RSIM_Process when the simulated
     *  thread dies (and the simulating thread exits).
     *
     *  Thread safety:  This method is thread safe; it can be invoked on a single object by multiple threads concurrently. */
    RSIM_Thread *get_thread(pid_t tid) const;

    /** Remove a thread from this process.  This is normally called by the specified thread when that thread exits.  Calling
     *  this method twice for the same thread will result in a failed assertion.
     *
     *  Thread safety: This method is thread safe; it can be invoked on a single object by multiple threads concurrently. */
    void remove_thread(RSIM_Thread*);



    /***************************************************************************************************************************
     *                                  Process loading, linking, exit, etc.
     ***************************************************************************************************************************/
private:
    SgAsmInterpretation *interpretation;        /**< Chosen by the load() method. */
    std::string exename;                        /**< Name of executable without any path components */
    std::string interpname;                     /**< Name of interpreter from ".interp" section or "--interp=" switch */
    rose_addr_t ep_orig_va;                     /**< Original executable entry point (a specimen virtual address). */
    rose_addr_t ep_start_va;                    /**< Entry point where simulation starts (e.g., the dynamic linker). */
    bool terminated;                            /**< True when the process has finished running. */
    int termination_status;                     /**< As would be returned by the parent's waitpid() call. */

public:
    /** Thrown by exit system calls. */
    struct Exit {
        Exit(int status, bool exit_process): status(status), exit_process(exit_process) {}
        int status;                             /**< Same value as returned by waitpid(). */
        bool exit_process;                      /**< If true, then exit the entire process. */
    };


    /** Returns the name of the executable without any path components. */
    std::string get_exename() const {
        return exename;
    }

    /** Returns the interpreter name for dynamically linked ELF executables.  This name comes from the ".interp" section of the
     *  ELF file and is usually the name of the dynamic linker, ld-linux.so.   The interpreter name is set automatically when
     *  the main executable is parsed, or it can be set manually with set_interpname(). */
    std::string get_interpname() const {
        return interpname;
    }

    /** Overrides the interpreter name that would have been obtained from the ELF ".interp" section.  This is useful when one
     *  has written a custom linker.  Regardless of whether the linker is explicitly specified or comes from the ELF file, it
     *  will be loaded into the specimen address space and executed. */
    void set_interpname(const std::string &s) {
        interpname = s;
    }
    
    /** Loads a new executable image into an existing process.  If the specified name contains no slashes, then the
     *  corresponding file is found by trying each of the directories listed in the PATH environment variable, otherwise the
     *  specified name is used directly.
     *
     *  Once the executable file is located, the ROSE frontend() is invoked to parse the container (ELF, PE, etc) but
     *  instructions are not disassembled yet.  A particular interpretation (SgAsmInterpretation) is chosen if more than one is
     *  available, and this becomes the node returned by get_interpretation().  The file header associated with this
     *  interpretation becomes the return value of this method. The process' memory map is adjusted by loading any necessary
     *  segments from the executable.
     *
     *  Any necessary dynamic linkers are added to the AST by using the interpreter defined in the executable (ELF ".interp"
     *  section) or a user-specified interpreter (see set_interpname()).  The process' memory map is adjusted by loading the
     *  interpreter.
     *
     *  If a virtual dynamic shared object is desired then it is parsed and loaded into the process' memory map.
     *
     *  A disassembler is chosen based on the interpretation. The disassembler can be obtained by calling get_disassembler().
     *
     *  Operating system simulation data is initialized (brk, mmap, etc). */
    SgAsmGenericHeader *load(const char *name);

    /** Returns the interpretation that is being simulated.  The interpretation was chosen by the load() method. */
    SgAsmInterpretation *get_interpretation() const {
        return interpretation;
    }

    /** Returns the original entry point of the executable. This is the entry point specified in the executable's file header
     *  and might not necessarily be the same address as where the simulation begins.
     *
     *  Thread safety:  This method is thread safe. */
    rose_addr_t get_ep_orig_va() const {
        return ep_orig_va;
    }

    /** Returns the address at which the simulation starts (or started).  This might not be the same as the program entry
     *  address returned by get_ep_orig_va() -- when an ELF executable is dynamically linked, this is the entry point for the
     *  dynamic linker.
     *
     *  Thread safety:  This method is thread safe. */
    rose_addr_t get_ep_start_va() const {
        return ep_start_va;
    }

    /** Initializes the stack for the main thread.  The argc and argv are the command-line of the specimen, not ROSE or the
     *  simulator.   Users generally don't need to call this directly. */
    void initialize_stack(SgAsmGenericHeader*, int argc, char *argv[]);

    /** Exit entire process. Saves the exit status (like that returned from waitpid), then joins all threads except the main
     *  thread.
     *
     *  Thread safety: This method can be called by any thread or multiple threads. This function returns only when called by
     *  the main thread. */
    void sys_exit(int status);

    /** Returns true if simulated process has terminated. */
    bool has_terminated() {
        return terminated;
    }

    /** Returns the process exit status. If the process has not exited, then zero is returned. */
    int get_termination_status() {
        return termination_status;
    }

public:
    void post_fork();

    void btrace_close();

    /** Sets the core dump styles. */
    void set_core_styles(unsigned bitmask) {
        core_flags = bitmask;
    }

    /** Generate an ELF Core Dump on behalf of the specimen.  This is a real core dump that can be used with GDB and contains
     *  the same information as if the specimen had been running natively and dumped its own core. In other words, the core
     *  dump we generate here does not have references to the simulator even though it is being dumped by the simulator. */
    void dump_core(int signo, std::string base_name="");

    /** Start an instruction trace file. No-op if "binary_trace" is null. */
    void binary_trace_start();

    /** Add an instruction to the binary trace file. No-op if "binary_trace" is null. */
    void binary_trace_add(RSIM_Thread*, const SgAsmInstruction*);

private:
    std::map<pid_t, RSIM_Thread*> threads;      /**< All threads associated with this process. */

private:
    unsigned core_flags;                        /**< Bit vector describing how to produce core dumps. */

public: /* FIXME */
    FILE *btrace_file;                          /**< Stream for binary trace. See projects/traceAnalysis/trace.C for details */
    std::vector<std::string> exeargs;           /**< Specimen argv with PATH-resolved argv[0] */

    std::vector<uint32_t> auxv;                 /* Auxv vector pushed onto initial stack; also used when dumping core */
    static const uint32_t brk_base=0x08000000;  /* Lowest possible brk() value */
    std::string vdso_name;                      /* Optional base name of virtual dynamic shared object from kernel */
    std::vector<std::string> vdso_paths;        /* Directories and/or filenames to search for vdso */
    rose_addr_t vdso_mapped_va;                 /* Address where vdso is mapped into specimen, or zero */
    rose_addr_t vdso_entry_va;                  /* Entry address for vdso, or zero */
    unsigned core_styles;                       /* What kind of core dump(s) to make for dump_core() */
    std::string core_base_name;                 /* Name to use for core files ("core") */
    rose_addr_t ld_linux_base_va;               /* Base address for ld-linux.so if no preferred addresss for "LOAD#0" */


};

#endif /* ROSE_RSIM_Process_H */
