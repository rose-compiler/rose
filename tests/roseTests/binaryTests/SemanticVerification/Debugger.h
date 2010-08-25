#ifndef ROSE_DEBUGGER_H
#define ROSE_DEBUGGER_H

#include <stdint.h>

/* The communication protocol is very simple and synchronous: the client sends a command, which the server answers with a
 * result. The protocol can be either ASCII-based for human usage, or binary for efficiency and the choice is made depending
 * on the first byte received from the client (a printable ASCII character invokes human mode; all others indicate binary). */

enum DebuggerCommand {
    /* Binary commands */
    CMD_QUIT = 0,               /* Kill subordinate and quit (arg is ignored) */
    CMD_STEP = 1,               /* Single step (arg is ignored) */
    CMD_CONT = 2,               /* Continue until subordinate reaches specified address. */
    CMD_REGS = 3,               /* Return values of all non-fp registers */
    CMD_NOOP = 4,               /* No-op command (still returns a status result) */
    CMD_BREAK = 5,              /* Set break points */
    CMD_MEM = 6,                /* Obtain memory values */
    CMD_SRCH = 7,               /* Search for a particular value in memory */
    CMD_DEBUG = 8,              /* Debugging add; does various things depending on what we're debugging. */
    CMD_NCOMMANDS = 9,          /* This must be last */

    /* ASCII commands */
    CMD_QUIT_A = 0x71,          /* 'q' */
    CMD_STEP_A = 0x73,          /* 's' */
    CMD_CONT_A = 0x63,          /* 'c' */
    CMD_REGS_A = 0x72,          /* 'r' */
    CMD_BREAK_A = 0x62,         /* 'b' */
    CMD_MEM_A = 0x6d,           /* 'm' */
    CMD_SRCH_A = 0x66,          /* 'f' */
    CMD_DEBUG_A = 0x64          /* 'd' */
};


/** A union of all the registers from all the architectures. Reading a register that doesn't exist on the architecture
 *  being debugged simply results in zero. When used for message passing between the debugger and ROSE, the data members
 *  will all be stored in little-endian. The RegisterSet has fairly generic names, so we define alternatives for the
 *  various architectures. */

enum RegisterName {
    /* Total number of registers */
    REG_NREGISTERS   = 27,

    /* x86_64 (these indices are compatible with x86 32-bit) */
    X86_REG_rip      = 0,
    X86_REG_rbp      = 1,
    X86_REG_rsp      = 2,
    X86_REG_orig_rax = 3,
    X86_REG_rax      = 4,
    X86_REG_rbx      = 5,
    X86_REG_rcx      = 6,
    X86_REG_rdx      = 7,
    X86_REG_r8       = 8,
    X86_REG_r9       = 9,
    X86_REG_r10      = 10,
    X86_REG_r11      = 11,
    X86_REG_r12      = 12,
    X86_REG_r13      = 13,
    X86_REG_r14      = 14,
    X86_REG_r15      = 15,
    X86_REG_cs       = 16,
    X86_REG_ds       = 17,
    X86_REG_es       = 18,
    X86_REG_fs       = 19,
    X86_REG_gs       = 20,
    X86_REG_ss       = 21,
    X86_REG_rsi      = 22,
    X86_REG_rdi      = 23,
    X86_REG_eflags   = 24, /*also valid for 32-bit x86*/
    X86_REG_fs_base  = 25,
    X86_REG_gs_base  = 26,

    /* x86 32-bit (these indices are compatible with x86_64) */
    X86_REG_eip      = 0,
    X86_REG_ebp      = 1,
    X86_REG_esp      = 2,
    X86_REG_orig_eax = 3,
    X86_REG_eax      = 4,
    X86_REG_ebx      = 5,
    X86_REG_ecx      = 6,
    X86_REG_edx      = 7,
    X86_REG_xcs      = 16,
    X86_REG_xds      = 17,
    X86_REG_xes      = 18,
    X86_REG_xfs      = 19,
    X86_REG_xgs      = 20,
    X86_REG_xss      = 21,
    X86_REG_esi      = 22,
    X86_REG_edi      = 23
};

struct RegisterSet {
    uint64_t reg[REG_NREGISTERS];       /* Indexed by one of the *_REG_* constants above. */
};

/** Interface to the debugger implemented in Debugger*.c in this directory. */
class Debugger {
public:
    /** Constructors all attach to a debugger via TCP/IP. Note that data member initializations are in ctor() so we don't
     *  need to repeat them for each constructor. */
    Debugger() {
        ctor("localhost", default_port);
    }
    Debugger(const std::string &hostname) {
        ctor(hostname, default_port);
    }
    Debugger(short port) {
        ctor("localhost", port);
    }
    Debugger(const std::string &hostname, short port) {
        ctor(hostname, port);
    }

    /** Destructor detaches from the debugger. */
    ~Debugger() {
        if (server>=0)
            close(server);
    }

    /** Set breakpoint at specified address. */
    int setbp(rose_addr_t addr) {
        return setbp(addr, 1);
    }
    
    /** Set beakpoints at all addresses in specified range. */
    int setbp(rose_addr_t addr, rose_addr_t size) {
        uint64_t status = execute(CMD_BREAK, addr, size);
        return status>=0 ? 0 : -1;
    }

    /** Execute one instruction and then stop. Returns the address where the program stopped. */
    rose_addr_t step() {
        return execute(CMD_STEP);
    }
    
    /** Continue execution until the next breakpoint is reached. Returns the address where the program stopped. */
    rose_addr_t cont() {
        return cont(0);
    }

    /** Continue execution until we reach the specified address, ignoring any breakpoints previously set. Returns the address
     *  where the program stopped. */
    int cont(rose_addr_t addr) {
	return execute(CMD_CONT, addr);
    }

    /** Returns the memory values at the specified address. The caller should a sufficiently large buffer. The return value is
     *  the number of bytes actually returned by the debugger, which might be less than requested if the debugger cannot read
     *  part of the memory. */
    size_t memory(rose_addr_t addr, size_t request_size, unsigned char *buffer);
    
    /** Obtain the four-byte word at the specified memory address. Returns zero if the word is not available. */
    uint64_t memory(rose_addr_t addr);

    /** Obtain registers from the debugger. The registers are cached in the client, so this is an efficient operation. */
    const RegisterSet& registers();

    /** Returns a particular register rather than the whole set. */
    uint64_t registers(int regname) {
        return registers().reg[regname];
    }
    
    /** Return the current address (contents of register "rip"). */
    rose_addr_t rip() {
        return registers(X86_REG_rip);
    }

private:
    /** Open a connection to the debugger. */
    void ctor(const std::string &hostname, short port);

    /** Replacement for system's htons(). Calling htons() on OSX gives an error "'exp' was not declared in this scope" even
     *  though we've included <netinet/in.h>. Therefore we write our own version here. Furthermore, OSX apparently doesn't
     *  define __BYTE_ORDER in <sys/param.h> so we have to figure it out ourselves. */
    static short my_htons(short n);

    /** Sends zero-argument command to the server */
    void send(DebuggerCommand cmd);

    /** Sends a one-argument command to the server. */
    void send(DebuggerCommand cmd, uint64_t arg1);

    /** Sends a two-argument command to the server. */
    void send(DebuggerCommand cmd, uint64_t arg1, uint64_t arg2);

    /** Reads the status word from the server and returns it, also saving it in the "status" data member. */
    uint64_t recv();

    /** Reads the status word (as a size) and the following data. The arguments and return value are like the GNU getline()
     *  function. */
    size_t recv(void **bufp, size_t *sizep);

    /** Run a command with no arguments and no result data. */
    uint64_t execute(DebuggerCommand cmd) {
        send(cmd);
        return recv();
    }

    /** Run a command with one argument and no result data. */
    uint64_t execute(DebuggerCommand cmd, uint64_t arg1) {
        send(cmd, arg1);
        return recv();
    }

    /** Run a command with two arguments and no result data. */
    uint64_t execute(DebuggerCommand cmd, uint64_t arg1, uint64_t arg2) {
        send(cmd, arg1, arg2);
	return recv();
    }

    /** Run a command with no arguments returning result data. */
    const void* execute(DebuggerCommand cmd, size_t *sizeptr) {
        send(cmd);
	recv(&result, &result_nalloc);
	if (sizeptr)
	  *sizeptr = status;
	return result;
    }

    /** Run a command with one argument returning result data. */
    const void* execute(DebuggerCommand cmd, uint64_t arg1, size_t *sizeptr) {
        send(cmd, arg1);
	recv(&result, &result_nalloc);
	if (sizeptr)
	  *sizeptr = status;
	return result;
    }

    /** Run a command with two arguments returning result data. */
    const void* execute(DebuggerCommand cmd, uint64_t arg1, uint64_t arg2, size_t *sizeptr) {
        send(cmd, arg1, arg2);
	recv(&result, &result_nalloc);
	if (sizeptr)
	  *sizeptr = status;
	return result;
    }

private:
    static const short default_port = 32002;
    int server;                                 /* Server socket */
    uint64_t status;		                /* Status from last executed command. */
    void *result;                               /* Result data from last executed command. */
    size_t result_nalloc;                       /* Current allocated size of the result buffer. */
    RegisterSet regs;                           /* Cached values of all registers */
    bool regs_current;                          /* Is the contents of "regs" cache current? */
};

#endif
