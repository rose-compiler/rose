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

#endif
