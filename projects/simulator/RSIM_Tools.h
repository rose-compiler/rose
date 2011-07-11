#ifndef ROSE_RSIM_Tools_H
#define ROSE_RSIM_Tools_H

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

/** Traverses the AST to find a symbol for a global function with the specified name. */
class FunctionFinder: public AstSimpleProcessing {
private:
    std::string fname;          /**< Holds address() function for use during traversal. */

public:
    /** Search for a function.  Searches for a function named @p fname in the specified @p ast and returns its entry address.
     * If the function cannot be found, then the null address is returned. */
    rose_addr_t address(SgNode *ast, const std::string &fname) {
        this->fname = fname;
        try {
            traverse(ast, preorder);
        } catch (rose_addr_t addr) {
            return addr;
        }
        return 0;
    }

private:
    /** Traversal callback. */
    void visit(SgNode *node) {
        SgAsmElfSymbol *sym = isSgAsmElfSymbol(node);
        if (sym &&
            sym->get_def_state() == SgAsmGenericSymbol::SYM_DEFINED &&
            sym->get_binding()   == SgAsmGenericSymbol::SYM_GLOBAL &&
            sym->get_type()      == SgAsmGenericSymbol::SYM_FUNC &&
            sym->get_name()->get_string() == fname)
            throw sym->get_value();
    }
};

/** Prints the name of the currently executing function.
 *
 *  In order for this to work, instructions must be assigned to functions.  This can be done by the MemoryDisassembler
 *  tool. The function name is printed to the TRACE_MISC facility whenever the current function changes. */
class FunctionReporter: public RSIM_Callbacks::InsnCallback {
public:
    bool show_call_stack;                       /**< Show a stack trace rather than just a function name. */
    FunctionReporter(bool show_call_stack=false)
        : show_call_stack(show_call_stack) {}

    virtual FunctionReporter *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        RSIM_Process *process = args.thread->get_process();
        SgAsmBlock *basic_block = isSgAsmBlock(args.insn->get_parent());
        SgAsmFunctionDeclaration *func = basic_block ? basic_block->get_enclosing_function() : NULL;
        std::string new_name = func ? func->get_name() : "";
        if (new_name!=name) {
            name = new_name;
            if (show_call_stack) {
                /* Check for the case when we recently executed a CALL instruction, but the called function has not yet pushed
                 * the old EBP register onto the stack.  In this case, the word at ss:[esp] will be two or five bytes past the
                 * address of a CALL instruction in executable memory.  This only handles CALLs encoded in two or five
                 * bytes. */
                bool bp_not_pushed = false;
                uint32_t esp = args.thread->policy.readGPR(x86_gpr_sp).known_value();
                uint32_t top_word;
                SgAsmx86Instruction *call_insn;
                try {
                    if (4==process->mem_read(&top_word, esp, 4)) {
                        if (NULL!=(call_insn=isSgAsmx86Instruction(process->get_instruction(top_word-5))) &&
                            (x86_call==call_insn->get_kind() || x86_farcall==call_insn->get_kind())) {
                            bp_not_pushed = true;
                        } else if (NULL!=(call_insn=isSgAsmx86Instruction(process->get_instruction(top_word-2))) &&
                                   (x86_call==call_insn->get_kind() || x86_farcall==call_insn->get_kind())) {
                            bp_not_pushed = true;
                        } else if (NULL!=(call_insn=isSgAsmx86Instruction(process->get_instruction(top_word-6))) &&
                                   (x86_call==call_insn->get_kind() || x86_farcall==call_insn->get_kind())) {
                            bp_not_pushed = true;
                        }
                    }
                } catch (const Disassembler::Exception&) {
                    /* ignored -- it just means the top of stack probably doesn't even point to executable memory */
                }
                args.thread->report_stack_frames(args.thread->tracing(TRACE_MISC), "FunctionReporter: stack frames",
                                                 bp_not_pushed);
            } else if (name.empty()) {
                args.thread->tracing(TRACE_MISC)->mesg("FunctionReporter: in unknown function");
            } else {
                args.thread->tracing(TRACE_MISC)->mesg("FunctionReporter: in function \"%s\"", name.c_str());
            }
        }
        return enabled;
    }

private:
    std::string name;
};

/** Watches for memory access.
 *
 *  This memory callback watches for access to certain memory locations and prints a message to the specified facility when
 *  such an access occurs.
 *
 *  See also, MemoryChecker, which checks the contents of memory after every instruction. */
class MemoryAccessWatcher: public RSIM_Callbacks::MemoryCallback {
public:
    RTS_Message *mesg;                          /**< Tracing facility, since no thread is available. */
    rose_addr_t va;                             /**< Starting address for watched memory region. */
    size_t nbytes;                              /**< Size of watched memory region. */
    unsigned how;                               /**< What kind of access we are watching. This should be a mask of
                                                 *   MemoryMap::Protection bits. */

    MemoryAccessWatcher(rose_addr_t va, size_t nbytes, unsigned how, RTS_Message *mesg=NULL)
        : mesg(mesg), va(va), nbytes(nbytes), how(how) {
        if (!mesg)
            this->mesg = new RTS_Message(stderr, NULL);
    }

    virtual MemoryAccessWatcher *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        if (enabled && 0!=(args.how & how) && args.va<va+nbytes && args.va+args.nbytes>=va) {
            mesg->mesg("MemoryAccessWatcher: triggered for access at 0x%08"PRIx64" for %zu byte%s\n",
                       args.va, args.nbytes, 1==args.nbytes?"":"s");
        }
        return enabled;
    }
};

/** Checks whether specimen memory matches a known value.
 *
 *  This instruction callback reads from the specified memory area and verifies that the conetnts of memory at that location
 *  match the expected value.  If not, a message is printed to the TRACE_MISC facility and the callback is disabled.
 *
 *  See also, MemoryAccessWatcher, which is a memory callback to look for memory access.
 *
 *  Here's an example of how to use this tool:
 *  @code
 *  uint8_t valid_mem = {0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0xef,
 *                       0x67, 0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x03};
 *  MemoryChecker mcheck(0x7c402740, sizeof valid_mem, valid_mem);
 *  simulator.install_callback(&mcheck);
 *  @endcode
 */
class MemoryChecker: public RSIM_Callbacks::InsnCallback {
    uint8_t *buffer;
public:
    rose_addr_t va;                             /**< Starting address of memory to check. */
    size_t nbytes;                              /**< Number of bytes to check. */
    uint8_t *answer;                            /**< Valid memory values to check against. User-supplied buffer. */
    bool report_short;                          /**< Treat short read as a difference. */
    bool update_answer;                         /**< If true, update answer when memory changes, and re-arm. */
    bool show_stack_frames;                     /**< If true, show stack frames when memory changes. */
    bool armed;                                 /**< Is this callback armed? */

    MemoryChecker(rose_addr_t va, size_t nbytes, uint8_t *answer, bool update_answer)
        : va(va), nbytes(nbytes), answer(answer), report_short(false), update_answer(update_answer),
          show_stack_frames(true), armed(true) {
        buffer = new uint8_t[nbytes];
    }

    ~MemoryChecker() {
        delete[] buffer;
    }

    virtual MemoryChecker *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        if (armed) {
            size_t nread = args.thread->get_process()->mem_read(buffer, va, nbytes);
            if (nread<nbytes && report_short) {
                args.thread->tracing(TRACE_MISC)->mesg("MemoryChecker: read failed at 0x%08"PRIx64, va+nread);
                armed = false;
            } else {
                for (size_t i=0; i<nread && i<nbytes; i++) {
                    if (answer[i]!=buffer[i]) {
                        args.thread->tracing(TRACE_MISC)->mesg("MemoryChecker: memory changed at 0x%08"PRIx64
                                                               " from 0x%02x to 0x%02x", va+i, answer[i], buffer[i]);
                        if (show_stack_frames)
                            args.thread->report_stack_frames(args.thread->tracing(TRACE_MISC));
                        if (update_answer) {
                            memcpy(answer, buffer, std::min(nread, nbytes));
                        } else {
                            armed = false;
                        }
                        break;
                    }
                }
            }
        }
        return enabled;
    }
};

/** Disassemble memory when an address is executed.
 *
 *  Runs the disassembler the first time we hit the specified execution address. */
class MemoryDisassembler: public RSIM_Callbacks::InsnCallback {
public:
    rose_addr_t when;                   /**< IP value when this callback is to be triggered. */
    bool triggered;                     /**< Set once this callback has been triggered. */
    bool show;                          /**< Controls whether result is emitted. If true, then emit the results to stdout. */

    MemoryDisassembler(rose_addr_t when, bool show)
        : when(when), triggered(false), show(show) {}

    virtual MemoryDisassembler *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        if (enabled && !triggered && args.insn->get_address()==when) {
            triggered = true;
            RTS_Message *m = args.thread->tracing(TRACE_MISC);
            m->mesg("MemoryDisassembler triggered: disassembling now...");
            SgAsmBlock *block = args.thread->get_process()->disassemble();
            if (show)
                AsmUnparser().unparse(std::cout, block);
        }
        return enabled;
    }
};

/** Show memory contents. */
class MemoryDumper: public RSIM_Callbacks::InsnCallback {
public:
    rose_addr_t when;                   /**< IP value when this callback is to be triggered. */
    rose_addr_t va;                     /**< Starting address to dump. */
    size_t nbytes;                      /**< Number of bytes to dump. */
    HexdumpFormat fmt;                  /**< Format to use for hexdump. */

    MemoryDumper(rose_addr_t when, rose_addr_t va, size_t nbytes)
        : when(when), va(va), nbytes(nbytes) {
        fmt.prefix = "  ";
        fmt.multiline = true;
    }

    virtual MemoryDumper *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        if (enabled && args.insn->get_address()==when) {
            RTS_Message *m = args.thread->tracing(TRACE_MISC);
            m->multipart("MemoryDumper", "MemoryDumper triggered: dumping %zu byte%s at 0x%08"PRIx64"\n",
                         nbytes, 1==nbytes?"":"s", va);
            uint8_t *buffer = new uint8_t[nbytes];
            size_t nread = args.thread->get_process()->mem_read(buffer, va, nbytes);
            if (nread < nbytes)
                m->mesg("MemoryDumper: read failed at 0x%08"PRIx64, va+nread);
            std::string s = SgAsmExecutableFileFormat::hexdump(va, buffer, nbytes, fmt);
            m->more("%s", s.c_str());
            m->multipart_end();
            delete[] buffer;
        }
        return enabled;
    }
};

/** Initialize memory.
 *
 *  When a certain instruction address is hit, memory is initialized by reading from a file or using a user-supplied buffer.
 *  This callback is triggered only one time.
 *
 *  The memory which is being written to should have write permission in order for this callback to succeed.  You can try
 *  changing need_write_perm to false, but if the underlying memory (in the simulator) was mapped without write permission
 *  you'll get a segmentation fault.
 *
 *  Example:
 *  @code
 *  InitializeMemory stack_initializer("x.data", 0xbfffe000, 0x7c4000a);
 *  RSIM_Linux32 simulator;
 *  simulator.get_callbacks().add_insn_callback(RSIM_Callbacks::BEFORE, &stack_initializer);
 *  @endcode
 */
class MemoryInitializer: public RSIM_Callbacks::InsnCallback {
public:
    std::string filename;               /**< Optional name of file containing memory image. */
    const uint8_t *new_value;           /**< Optional pointer to bytes to be written. */
    size_t nbytes;                      /**< Number of bytes pointed to by new_value. */
    rose_addr_t memaddr;                /**< Address where file contents are loaded. */
    rose_addr_t when;                   /**< IP value when this callback is to be triggered. */
    bool triggered;                     /**< Set once this callback has been triggered. */
    bool need_write_perm;               /**< Is write permission needed on the memory? */

    MemoryInitializer(const std::string &filename, rose_addr_t memaddr, rose_addr_t when)
        : filename(filename), new_value(NULL), nbytes(0),
          memaddr(memaddr), when(when), triggered(false), need_write_perm(true) {}
    MemoryInitializer(const uint8_t *new_value, size_t nbytes, rose_addr_t memaddr, rose_addr_t when)
        : new_value(new_value), nbytes(nbytes),
          memaddr(memaddr), when(when), triggered(false), need_write_perm(true) {}

    virtual MemoryInitializer *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        if (enabled && !triggered && args.insn->get_address()==when) {
            triggered = true;
            size_t total_written=0;
            unsigned perms = need_write_perm ? MemoryMap::MM_PROT_WRITE : MemoryMap::MM_PROT_NONE;

            if (new_value) {
                total_written = args.thread->get_process()->mem_write(new_value, memaddr, nbytes, perms);
                if (total_written!=nbytes)
                    fprintf(stderr, "MemoryInitializer write failed at 0x%08"PRIx64"\n", memaddr+total_written);
            } else {
                int fd = open(filename.c_str(), O_RDONLY);
                if (fd<0) {
                    perror(filename.c_str());
                    return enabled;
                }

                uint8_t buf[4096];
                ssize_t nread;
                rose_addr_t va = memaddr;
                while ((nread=read(fd, buf, sizeof buf))>0) {
                    size_t nwrite = args.thread->get_process()->mem_write(buf, va, (size_t)nread, perms);
                    total_written += nwrite;
                    va += nwrite;
                    if (nwrite!=(size_t)nread) {
                        fprintf(stderr, "MemoryInitializer write failed at 0x%08"PRIx64"\n", va);
                        break;
                    }
                }
                if (nread<0) {
                    close(fd);
                    perror(filename.c_str());
                    return enabled;
                }
                close(fd);
            }
            RTS_Message *m = args.thread->tracing(TRACE_MISC);
            m->mesg("MemoryInitializer triggered: wrote 0x%zx bytes at 0x%08"PRIx64, total_written, memaddr);
        }
        return enabled;
    }
};

/** Show memory mapping when an address is executed. */
class MemoryMapDumper: public RSIM_Callbacks::InsnCallback {
public:
    rose_addr_t when;                   /**< IP value when this callback is to be triggered. */

    MemoryMapDumper(rose_addr_t when)
        : when(when) {}

    virtual MemoryMapDumper *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        if (enabled && args.insn->get_address()==when)
            args.thread->get_process()->mem_showmap(args.thread->tracing(TRACE_MISC));
        return enabled;
    }
};

/** Prints register contents.
 *
 *  Every time execution hits a specified address, registers are dumped to the TRACE_MISC facility. */
class RegisterDumper: public RSIM_Callbacks::InsnCallback {
public:
    rose_addr_t when;                   /**< IP value when this callback is to be triggered. */

    RegisterDumper(rose_addr_t when)
        : when(when) {}

    virtual RegisterDumper *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        if (enabled && args.insn->get_address()==when)
            args.thread->policy.dump_registers(args.thread->tracing(TRACE_MISC));
        return enabled;
    }
};

/** Generates a stack trace when a signal arrives. */
class SignalStackTrace: public RSIM_Callbacks::SignalCallback {
public:
    bool disassembled;                  /**< Controls disassembly.  If clear, then disassemble memory and set. */

    SignalStackTrace()
        : disassembled(false) {}

    virtual SignalStackTrace *clone() { return this; }

    virtual bool operator()(bool enabled, const Args &args) {
        if (args.reason == ARRIVAL) {
            if (!disassembled) {
                args.thread->get_process()->disassemble(); /* so stack trace has function names */
                disassembled = true;
            }
            args.thread->report_stack_frames(args.thread->tracing(TRACE_MISC));
        }
        return enabled;
    }
};

/** Provides implementations for functions not in ROSE.
 *
 *  These few functions are sometimes encountered in ld-linux.so and are important for its correct operation. */
class UnhandledInstruction: public RSIM_Callbacks::InsnCallback {
public:
    struct MmxValue {
        VirtualMachineSemantics::ValueType<32> lo, hi;
    };

    MmxValue mmx[8];                    // MMX registers 0-7

    virtual UnhandledInstruction *clone() { return this; }
    virtual bool operator()(bool enabled, const Args &args) {
        static const char *fmt = "UnhandledInstruction triggered for %s\n";
        SgAsmx86Instruction *insn = isSgAsmx86Instruction(args.insn);
        if (enabled && insn) {
            RTS_Message *m = args.thread->tracing(TRACE_MISC);
            const SgAsmExpressionPtrList &operands = insn->get_operandList()->get_operands();
            uint32_t newip_va = insn->get_address() + insn->get_raw_bytes().size();
            VirtualMachineSemantics::ValueType<32> newip = args.thread->policy.number<32>(newip_va);
            switch (insn->get_kind()) {
                case x86_movd: {
                    assert(2==operands.size());
                    SgAsmRegisterReferenceExpression *mre = isSgAsmRegisterReferenceExpression(operands[0]);
                    if (mre && mre->get_descriptor().get_major()==x86_regclass_xmm) {
                        int mmx_number = mre->get_descriptor().get_minor();
                        m->mesg(fmt, unparseInstruction(insn).c_str());
                        mmx[mmx_number].lo = args.thread->semantics.read32(operands[1]);
                        mmx[mmx_number].hi = args.thread->policy.number<32>(0);
                        args.thread->policy.writeIP(newip);
                        enabled = false;
                    }
                    break;
                }

                case x86_movq: {
                    assert(2==operands.size());
                    SgAsmRegisterReferenceExpression *mre = isSgAsmRegisterReferenceExpression(operands[1]);
                    if (mre && mre->get_descriptor().get_major()==x86_regclass_xmm) {
                        int mmx_number = mre->get_descriptor().get_minor();
                        m->mesg(fmt, unparseInstruction(insn).c_str());
                        VirtualMachineSemantics::ValueType<32> addr = args.thread->semantics.readEffectiveAddress(operands[0]);
                        args.thread->policy.writeMemory(x86_segreg_ss, addr, mmx[mmx_number].lo, args.thread->policy.true_());
                        addr = args.thread->policy.add<32>(addr, args.thread->policy.number<32>(4));
                        args.thread->policy.writeMemory(x86_segreg_ss, addr, mmx[mmx_number].hi, args.thread->policy.true_());
                        args.thread->policy.writeIP(newip);
                        enabled = false;
                    }
                    break;
                }

                case x86_pause: {
                    /* PAUSE is treated as a CPU hint, and is a no-op on some architectures. */
                    assert(0==operands.size());
                    args.thread->policy.writeIP(newip);
                    enabled = false;
                    break;
                }

                case x86_stmxcsr: {
                    /* Store value of mxcsr register (which we don't actually have) to a doubleword in memory.  The value we
                     * store was obtained by running GDB under "i386 -LRB3", stopping at the first instruction, and looking at
                     * the mxcsr register. */
                    m->mesg(fmt, unparseInstruction(insn).c_str());
                    assert(1==operands.size());
                    VirtualMachineSemantics::ValueType<32> value = args.thread->policy.number<32>(0x1f80); // from GDB
                    VirtualMachineSemantics::ValueType<32> addr = args.thread->semantics.readEffectiveAddress(operands[0]);
                    args.thread->policy.writeMemory(x86_segreg_ss, addr, value, args.thread->policy.true_());
                    args.thread->policy.writeIP(newip);
                    enabled = false;
                    break;
                }

                case x86_ldmxcsr: {
                    /* Load the mxcsr register (which we don't actually have) from a doubleword in memory.  We read the memory
                     * (for possible side effects) but then just throw away the value. */
                    m->mesg(fmt, unparseInstruction(insn).c_str());
                    assert(1==operands.size());
                    VirtualMachineSemantics::ValueType<32> addr = args.thread->semantics.readEffectiveAddress(operands[0]);
                    (void)args.thread->policy.readMemory<32>(x86_segreg_ss, addr, args.thread->policy.true_());
                    args.thread->policy.writeIP(newip);
                    enabled = false;
                    break;
                }

                default:                // to shut up warnings about the zillion instructions we don't handle here
                    break;
            }
        }
        return enabled;
    }
};

#endif
