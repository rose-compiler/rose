/* See README. The basic idea is that ROSE parses the executable and then attaches to a debugger, single stepping through the
 * program and comparing the real execution to ROSE's semantic analysis. */
#include "rose.h"
#include "Debugger.h"
#include "x86InstructionSemantics.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

static const char *trace_prefix = "    ";

/* Registers names, etc. for x86 32-bit */
static const int x86_reg_names[] = {X86_REG_eip, X86_REG_ebp, X86_REG_esp, X86_REG_orig_eax, X86_REG_eax, X86_REG_ebx,
                                    X86_REG_ecx, X86_REG_edx, X86_REG_xcs, X86_REG_xds, X86_REG_xes, X86_REG_xfs,
                                    X86_REG_xgs, X86_REG_xss, X86_REG_esi, X86_REG_edi};
static const char *x86_reg_str[] = {"eip", "ebp", "esp", "orig_eax", "eax", "ebx", "ecx", "edx", "xcs", "xds", "xes", "xfs", 
                                    "xgs", "xss", "esi", "edi"};
static const char x86_reg_size[] = {32,    32,    32,    32,         32,    32,    32,    32,    16,    16,    16,    16,
                                    16,    16,    32,    32};


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
    size_t memory(rose_addr_t addr, size_t request_size, unsigned char *buffer) {
        size_t return_size;
        const unsigned char *bytes = (const unsigned char*)execute(CMD_MEM, addr, request_size, &return_size);
        assert(request_size>=return_size);
        memcpy(buffer, bytes, return_size);
        memset(buffer+return_size, 0, request_size-return_size);
        return return_size;
    }
    
    /** Obtain the four-byte word at the specified memory address. Returns zero if the word is not available. */
    uint64_t memory(rose_addr_t addr) {
        size_t size;
        const unsigned char *bytes = (const unsigned char*)execute(CMD_MEM, addr, 4, &size);
        if (size!=4)
            return 0;
        uint64_t retval = 0;
        for (size_t j=0; j<size; j++) {
            retval |= (uint64_t)(bytes[j]) << (8*j);
        }
        return retval;
    }

    /** Obtain registers from the debugger. The registers are cached in the client, so this is an efficient operation. */
    const RegisterSet& registers() {
        if (!regs_current) {
	    size_t size;
	    const void *x = execute(CMD_REGS, &size);
	    assert(size==sizeof(regs));
	    memcpy(&regs, x, size);
	    regs_current = true;
	}
	return regs;
    }

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
    void ctor(const std::string &hostname, short port) {
        /* Initialize data members. */
        server = -1;
        status = 0;
        result = NULL;
        result_nalloc = 0;
	memset(&regs, 0, sizeof regs);
	regs_current = false;

        struct hostent *he = gethostbyname2(hostname.c_str(), AF_INET);
        assert(he);

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = my_htons(port);
        assert(he->h_length==sizeof(addr.sin_addr.s_addr));
        memcpy(&addr.sin_addr.s_addr, he->h_addr, he->h_length);

        server = socket(AF_INET, SOCK_STREAM, 0);
        assert(server>=0);
        if (connect(server, (struct sockaddr*)&addr, sizeof addr)<0) {
            fprintf(stderr, "cannot connect to server at %s:%hd: %s\n", hostname.c_str(), port, strerror(errno));
            exit(1);
        }
    }

    /** Replacement for system's htons(). Calling htons() on OSX gives an error "'exp' was not declared in this scope" even
     *  though we've included <netinet/in.h>. Therefore we write our own version here. Furthermore, OSX apparently doesn't
     *  define __BYTE_ORDER in <sys/param.h> so we have to figure it out ourselves. */
    static short my_htons(short n) {
        static unsigned u = 1;
        if (*((char*)&u)) {
            /* Little endian */
            return (((unsigned short)n & 0x00ff)<<8) | (((unsigned short)n & 0xff00)>>8);
        } else {
            return n;
        }
    }

    /** Sends zero-argument command to the server */
    void send(DebuggerCommand cmd) {
        regs_current = false;
	unsigned char byte = cmd;
	ssize_t n = write(server, &byte, 1);
	if (n<0) {
	    fprintf(stderr, "write command: %s\n", strerror(errno));
	    exit(1);
	} else if (n!=1) {
 	    fprintf(stderr, "write command: short write\n");
	    exit(1);
	}
    }

    /** Sends a one-argument command to the server. */
    void send(DebuggerCommand cmd, uint64_t arg1) {
        send(cmd);
        ssize_t n = write(server, &arg1, sizeof arg1);
        if (n<0) {
	    fprintf(stderr, "write arg1: %s\n", strerror(errno));
	    exit(1);
	} else if (n!=sizeof arg1) {
 	    fprintf(stderr, "write arg1: short write\n");
	    exit(1);
	}
    }

    /** Sends a two-argument command to the server. */
    void send(DebuggerCommand cmd, uint64_t arg1, uint64_t arg2) {
        send(cmd, arg1);
        ssize_t n = write(server, &arg2, sizeof arg2);
        if (n<0) {
	    fprintf(stderr, "write arg2: %s\n", strerror(errno));
	    exit(1);
	} else if (n!=sizeof arg2) {
 	    fprintf(stderr, "write arg2: short write\n");
	    exit(1);
	}
    }

    /** Reads the status word from the server and returns it, also saving it in the "status" data member. */
    uint64_t recv() {
        ssize_t n = read(server, &status, sizeof status);
	if (n<0) {
 	    fprintf(stderr, "read status: %s\n", strerror(errno));
	    exit(1);
	} else if (n!=sizeof status) {
	    fprintf(stderr, "read status: short read\n");
	    exit(1);
	}
	return status;
    }

    /** Reads the status word (as a size) and the following data. The arguments and return value are like the GNU getline()
     *  function. */
    size_t recv(void **bufp, size_t *sizep) {
        size_t need = recv();
	if (need > *sizep) {
            *sizep = need;
	    *bufp = realloc(*bufp, *sizep);
	    assert(*bufp);
	}
	ssize_t n = read(server, *bufp, need);
	if (n<0) {
	    fprintf(stderr, "read data: %s\n", strerror(errno));
	    exit(1);
	} else if ((size_t)n!=need) {
	    fprintf(stderr, "read data: short read\n");
	    exit(1);
	}
	return need;
    }

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

/** Values used by X86InstructionsSemantics with the Verifier policy defined below. */
template<size_t Nbits>
class VerifierValue {
public:
    VerifierValue(uint64_t n): v_(n) {
        assert(Nbits<=8*sizeof v_);
        mask = (uint64_t)-1 >> (8*sizeof(v_)-Nbits);
        v_ = n & mask;
    }

    /* Returns an extended (64-bit) version of the value without sign extension. */
    uint64_t uv() {
        return v_ & mask;
    }

    /* Returns an extended (64-bit) version of the value with sign extension. */
    int64_t sv() {
        uint64_t retval = v_ & mask;
        if (v_ & ((uint64_t)1<<(Nbits-1)))
            retval |= ~mask; /*sign extend*/
        return (int64_t)retval;
    }
private:
    uint64_t mask;
    uint64_t v_;
};

/** Instruction semantics policy verifies ROSE's instruction semantics against a running program. */
class Verifier {
public:
    class Exception {
    public:
        Exception(const std::string &s): mesg(s) {}
        std::string mesg;
    private:
        Exception() {}
    };

    Verifier(Debugger *dbg)
        :debugger(dbg), registers_current(false), trace_file(NULL) {
        memset(&register_set, 0, sizeof register_set);
    }

    /* Causes semantic actions to be output to the specified file. */
    void trace(std::ostream *f) {
        trace_file = f;
    }

    /* Returns true if the verifier has cached register values. */
    bool has_registers() const {
        return registers_current;
    }

    /* Returns the value of a cached register. */
    uint64_t registers(RegisterName r) const {
        assert(registers_current);
        return register_set.reg[r];
    }

    /* Returns entire cached register set. */
    const RegisterSet& registers() const {
        return register_set;
    }

    /* Compares the cached register set with the actual register set returned by the debugger and throws an exception if there
     * is a mismatch. The exception message is a multi-line string containing all the register differences. */
    void assert_registers(const char *prefix="") {
        assert(registers_current);
        std::string mesg;
        const RegisterSet &rset = debugger->registers();
        for (size_t i=0; i<NELMTS(x86_reg_names); i++) {
            unsigned long mask = (unsigned long)-1 >> (8*sizeof(long)-x86_reg_size[i]);
            unsigned long v1 = register_set.reg[x86_reg_names[i]] & mask;
            unsigned long v2 = rset.reg[x86_reg_names[i]] & mask;

            if (v1!=v2) {
                char buf[256];
                int w = x86_reg_size[i]/4;
                sprintf(buf, "%s%s:  0x%0*lx (simulated) != 0x%0*lx (actual)", prefix, x86_reg_str[i], w, v1, w, v2);
                mesg = mesg + (mesg=="" ? "" : "\n") + buf;
            }
        }
        if (mesg!="")
            throw Exception(mesg);
    }

    /* Compares the cached memory values with the actual memory values returned by the debugger and throws an exception if
     * there is a mismatch.  The exception is a multi-line string containing all the memory differences. */
    void assert_memory(const char *prefix="") {
        std::string mesg;
        for (std::map<uint64_t, unsigned char>::iterator mi=memory.begin(); mi!=memory.end(); mi++) {
            uint64_t addr = mi->first;
            unsigned char simulated = mi->second;
            unsigned char actual = debugger->memory(addr) & 0xff;
            if (simulated!=actual) {
                char buf[256];
                sprintf(buf, "%smem[0x%016"PRIx64"]: 0x%02x (simulated) != 0x%02x (actual)",
                        prefix, addr, simulated, actual);
                mesg = mesg + (mesg=="" ? "" : "\n") + buf;
            }
        }
        if (mesg!="")
            throw Exception(mesg);
    }

    /* Called before each instruction is processed.  It marks cached registers and memory as invalid in order to force them to
     * be read across the network from the debugger. It also checks that the instruction we're verifying matches the
     * instruction on the debugged process.
     *
     * On error, this function prints a message and then throws an exception in order to avoid the analysis. */
    void startInstruction(SgAsmx86Instruction *insn) {
        registers_current = false; /* will cause next register read to come from the debugger rather than a cached value. */
        memory.clear();

        if (trace_file)
            (*trace_file) <<"Tracing 0x" <<std::hex <<insn->get_address() <<": " <<unparseInstruction(insn) <<"\n";

        /* Verify that instructions are identical. */
        unsigned char remote[256];
        assert(insn->get_raw_bytes().size()<=sizeof remote);
        size_t remote_size = debugger->memory(insn->get_address(), insn->get_raw_bytes().size(), remote);
        if (remote_size!=insn->get_raw_bytes().size()) {
            throw Exception("could not read entire instruction");
        } else if (memcmp(&(insn->get_raw_bytes()[0]), remote, remote_size)) {
            throw Exception("instruction mismatch");
        }
    }

    /* Called after each instruction is processed. */
    void finishInstruction(SgAsmx86Instruction*) {}

    /* Called for the HLT instruction */
    void hlt() {} /*FIXME*/

    /* Called for the RDTSC instruction */
    VerifierValue<64> rdtsc() {return 0;} /*FIXME*/

    /* Called for the INT instruction */
    void interrupt(uint8_t num) {} /*FIXME*/
    
    VerifierValue<32> filterCallTarget(VerifierValue<32> x) {return x;}
    VerifierValue<32> filterIndirectJumpTarget(VerifierValue<32> x) {return x;}
    VerifierValue<32> filterReturnTarget(VerifierValue<32> x) {return x;}
    
    /* Constant false */
    VerifierValue<1> false_() {
        return false;
    }
    
    /* Constant true */
    VerifierValue<1> true_() {
        return true;
    }

    /* Flag having an undefined value. */
    VerifierValue<1> undefined_() {
        return false;
    }

    /* A constant */
    template<size_t Len>
    VerifierValue<Len> number(uint64_t n) {
        return n;
    }

    /* Sign extend a value from Len1 to Len2 bits. */
    template<size_t Len1, size_t Len2>
    VerifierValue<Len2> signExtend(VerifierValue<Len1> a) {
        assert(Len1 <= Len2);
        return a.sv();
    }

    /* Extracts bits from value A where "From" is the index of the lowest order bit to extract and "To" is one higher than the
     * highest bit to extract. In other words, the number of bits extracted is "To-From". */
    template<size_t From, size_t To, size_t Len>
    VerifierValue<To-From> extract(VerifierValue<Len> a) {
        assert(From<To);
        assert(To<=Len);
        uint64_t a_shifted = a.uv() >> From;
        uint64_t mask = ((uint64_t)1<<(To-From))-1;
        return a_shifted & mask;
    }

    /* Increases the size of a value by adding (clear) bits to the most significant end. */
    template<size_t From, size_t To>
    VerifierValue<To> extendByMSB(VerifierValue<From> a) {
        return a.uv();
    }
    
    /* Return the current instruction pointer register. */
    VerifierValue<32> readIP() {
        if (!registers_current) {
            register_set = debugger->registers();
            registers_current = true;
        }
        VerifierValue<32> retval = register_set.reg[X86_REG_eip];
        if (trace_file)
            (*trace_file) <<trace_prefix <<"eip -> 0x" <<std::hex <<std::setw(32/4) <<std::setfill('0') <<retval.uv() <<"\n";
        return retval;
    }
    
    /* Give the instruction pointer a new value. */
    void writeIP(VerifierValue<32> value) {
        if (!registers_current) {
            register_set = debugger->registers();
            registers_current = true;
        }
        if (trace_file)
            (*trace_file) <<trace_prefix <<"0x" <<std::hex <<std::setw(32/4) <<std::setfill('0') <<value.uv() <<" -> eip\n";
        register_set.reg[X86_REG_eip] = value.uv();
    }

    /* Read the value of a general purpose register. */
    VerifierValue<32> readGPR(X86GeneralPurposeRegister gpr) {
        /* Convert x86 general purpose register name to a generic register name. */
        const char *s;
        RegisterName name;
        switch (gpr) {
            case x86_gpr_ax: name = X86_REG_eax; s = "eax"; break;
            case x86_gpr_cx: name = X86_REG_ecx; s = "ecx"; break;
            case x86_gpr_dx: name = X86_REG_edx; s = "edx"; break;
            case x86_gpr_bx: name = X86_REG_ebx; s = "ebx"; break;
            case x86_gpr_sp: name = X86_REG_esp; s = "esp"; break;
            case x86_gpr_bp: name = X86_REG_ebp; s = "ebp"; break;
            case x86_gpr_si: name = X86_REG_esi; s = "esi"; break;
            case x86_gpr_di: name = X86_REG_edi; s = "edi"; break;
            default: abort();
        }

        /* Return the most recently written value or get the value from the debugger. */
        if (!registers_current) {
            register_set = debugger->registers();
            registers_current = true;
        }

        VerifierValue<32> retval(register_set.reg[name]);
        if (trace_file)
            (*trace_file) <<trace_prefix <<s <<" -> 0x" <<std::hex <<std::setw(32/4) <<std::setfill('0') <<retval.uv() <<"\n";
        return retval;
    }

    /* Write a value to a general purpose register. */
    void writeGPR(X86GeneralPurposeRegister gpr, VerifierValue<32> value) {
        /* Convert x86 general purpose register name to a generic register name. */
        const char *s;
        RegisterName name;
        switch (gpr) {
            case x86_gpr_ax: name = X86_REG_eax; s = "eax"; break;
            case x86_gpr_cx: name = X86_REG_ecx; s = "ecx"; break;
            case x86_gpr_dx: name = X86_REG_edx; s = "edx"; break;
            case x86_gpr_bx: name = X86_REG_ebx; s = "ebx"; break;
            case x86_gpr_sp: name = X86_REG_esp; s = "esp"; break;
            case x86_gpr_bp: name = X86_REG_ebp; s = "ebp"; break;
            case x86_gpr_si: name = X86_REG_esi; s = "esi"; break;
            case x86_gpr_di: name = X86_REG_edi; s = "edi"; break;
            default: abort();
        }

        if (!registers_current) {
            register_set = debugger->registers();
            registers_current = true;
        }
        if (trace_file)
            (*trace_file) <<trace_prefix <<"0x" <<std::hex <<std::setw(32/4) <<std::setfill('0') <<value.uv() <<" -> " <<s <<"\n";
        register_set.reg[name] = value.uv();
    }

    /* Read the value of a flag from the eflags register. */
    VerifierValue<1> readFlag(X86Flag f) {
        /* Convert x86 flag name to bit offset in eflags register. */
        const char *s;
        int bitidx;
        switch (f) {
            case x86_flag_cf:    bitidx =  0; s = "cf"; break;
            case x86_flag_1:     bitidx =  1; s = "1f"; break;
            case x86_flag_pf:    bitidx =  2; s = "pf"; break;
            case x86_flag_3:     bitidx =  3; s = "3f"; break;
            case x86_flag_af:    bitidx =  4; s = "af"; break;
            case x86_flag_5:     bitidx =  5; s = "5f"; break;
            case x86_flag_zf:    bitidx =  6; s = "zf"; break;
            case x86_flag_sf:    bitidx =  7; s = "sf"; break;
            case x86_flag_tf:    bitidx =  8; s = "tf"; break;
            case x86_flag_if:    bitidx =  9; s = "if"; break;
            case x86_flag_df:    bitidx = 10; s = "df"; break;
            case x86_flag_of:    bitidx = 11; s = "of"; break;
            case x86_flag_iopl0: bitidx = 12; s = "iopl0"; break;
            case x86_flag_iopl1: bitidx = 13; s = "iopl1"; break;
            case x86_flag_nt:    bitidx = 14; s = "nt"; break;
            case x86_flag_15:    bitidx = 15; s = "15f"; break;
            case x86_flag_rf:    bitidx = 16; s = "rf"; break;
            case x86_flag_vm:    bitidx = 17; s = "vm"; break;
            case x86_flag_ac:    bitidx = 18; s = "ac"; break;
            case x86_flag_vif:   bitidx = 19; s = "vif"; break;
            case x86_flag_vip:   bitidx = 20; s = "vip"; break;
            case x86_flag_id:    bitidx = 21; s = "id"; break;
            default: abort();
        }

        if (!registers_current) {
            register_set = debugger->registers();
            registers_current = true;
        }

        VerifierValue<1> retval = register_set.reg[X86_REG_eflags] & (1lu << bitidx) ? true_() : false_();
        if (trace_file)
            (*trace_file) <<trace_prefix <<s <<" -> "<<(retval.uv()?"true":"false") <<"\n";
        return retval;
    }
    
    /* Write one of the bits in the eflags register. */
    void writeFlag(X86Flag f, VerifierValue<1> value) {
        /* Convert x86 flag name to bit offset in eflags register. */
        const char *s;
        int bitidx;
        switch (f) {
            case x86_flag_cf:    bitidx =  0; s = "cf"; break;
            case x86_flag_1:     bitidx =  1; s = "1f"; break;
            case x86_flag_pf:    bitidx =  2; s = "pf"; break;
            case x86_flag_3:     bitidx =  3; s = "3f"; break;
            case x86_flag_af:    bitidx =  4; s = "af"; break;
            case x86_flag_5:     bitidx =  5; s = "5f"; break;
            case x86_flag_zf:    bitidx =  6; s = "zf"; break;
            case x86_flag_sf:    bitidx =  7; s = "sf"; break;
            case x86_flag_tf:    bitidx =  8; s = "tf"; break;
            case x86_flag_if:    bitidx =  9; s = "if"; break;
            case x86_flag_df:    bitidx = 10; s = "df"; break;
            case x86_flag_of:    bitidx = 11; s = "of"; break;
            case x86_flag_iopl0: bitidx = 12; s = "iopl0"; break;
            case x86_flag_iopl1: bitidx = 13; s = "iopl1"; break;
            case x86_flag_nt:    bitidx = 14; s = "nt"; break;
            case x86_flag_15:    bitidx = 15; s = "15f"; break;
            case x86_flag_rf:    bitidx = 16; s = "rf"; break;
            case x86_flag_vm:    bitidx = 17; s = "vm"; break;
            case x86_flag_ac:    bitidx = 18; s = "ac"; break;
            case x86_flag_vif:   bitidx = 19; s = "vif"; break;
            case x86_flag_vip:   bitidx = 20; s = "vip"; break;
            case x86_flag_id:    bitidx = 21; s = "id"; break;
            default: abort();
        }

        if (!registers_current) {
            register_set = debugger->registers();
            registers_current = true;
        }

        if (value.uv()) {
            register_set.reg[X86_REG_eflags] |= 1lu<<bitidx;
        } else {
            register_set.reg[X86_REG_eflags] &= ~(1lu<<bitidx);
        }

        if (trace_file)
            (*trace_file) <<trace_prefix <<(value.uv()?"true":"false") <<" -> " <<s <<"\n";
    }

    /* Read the contents of a segment register. */
    VerifierValue<16> readSegreg(X86SegmentRegister sr) {
        /* Convert x86 general purpose register name to a generic register name. */
        const char *s;
        RegisterName name;
        switch (sr) {
            case x86_segreg_es: name = X86_REG_es; s = "es"; break;
            case x86_segreg_cs: name = X86_REG_cs; s = "cs"; break;
            case x86_segreg_ss: name = X86_REG_ss; s = "ss"; break;
            case x86_segreg_ds: name = X86_REG_ds; s = "ds"; break;
            case x86_segreg_fs: name = X86_REG_fs; s = "fs"; break;
            case x86_segreg_gs: name = X86_REG_gs; s = "gs"; break;
            default: abort();
        }

        if (!registers_current) {
            register_set = debugger->registers();
            registers_current = true;
        }

        VerifierValue<16> retval(register_set.reg[name]);
        if (trace_file)
            (*trace_file) <<trace_prefix <<s <<" -> 0x" <<std::hex <<std::setw(16/4) <<std::setfill('0') <<retval.uv() <<"\n";
        return register_set.reg[name];
    }
    
    /* Write a value to a segment register */
    void writeSegreg(X86SegmentRegister sr, VerifierValue<16> val) {
        /* Convert x86 general purpose register name to a generic register name. */
        const char *s;
        RegisterName name;
        switch (sr) {
            case x86_segreg_es: name = X86_REG_es; s = "es"; break;
            case x86_segreg_cs: name = X86_REG_cs; s = "cs"; break;
            case x86_segreg_ss: name = X86_REG_ss; s = "ss"; break;
            case x86_segreg_ds: name = X86_REG_ds; s = "ds"; break;
            case x86_segreg_fs: name = X86_REG_fs; s = "fs"; break;
            case x86_segreg_gs: name = X86_REG_gs; s = "gs"; break;
            default: abort();
        }

        if (!registers_current) {
            register_set = debugger->registers();
            registers_current = true;
        }
        if (trace_file)
            (*trace_file) <<trace_prefix <<"0x" <<std::hex <<std::setw(16/4) <<std::setfill('0') <<val.uv() <<" -> " <<s <<"\n";
        register_set.reg[name] = val.uv();
    }

    /* Reads the contents of some memory location. */
    template <size_t Len>
    VerifierValue<Len> readMemory(X86SegmentRegister segreg, VerifierValue<32> addrval, VerifierValue<1> cond) {
        assert(0==Len%8);
        assert(cond.uv()!=0);
        const size_t nbytes = Len/8;
        (void)readSegreg(segreg); /*emits segreg value if there's a failure*/
        uint64_t addr = addrval.uv();
        unsigned char actual[nbytes];
        size_t nread = debugger->memory(addr, nbytes, actual);
        if (nread!=nbytes) {
            char buf[256];
            sprintf(buf, "unable to read memory at 0x%016"PRIx64, addr);
            throw Exception(buf);
        }

        uint64_t val = 0;
        for (uint64_t i=0; i<nbytes; i++) {
            std::map<uint64_t, unsigned char>::iterator mi = memory.find(addr+i);
            if (mi!=memory.end()) {
                val |= mi->second << (8*i);
            } else {
                val |= actual[i] << (8*i);
            }
        }

        VerifierValue<Len> retval(val);
        if (trace_file)
            (*trace_file) <<trace_prefix <<"mem[0x" <<std::hex <<std::setw(32/4) <<std::setfill('0') <<addr <<"]"
                          <<" -> 0x" <<std::hex <<std::setw(Len/4) <<std::setfill('0') <<retval.uv() <<"\n";
        return retval;
    }
    
    /* Conditionally write a value to memory */
    template <size_t Len>
    void writeMemory(X86SegmentRegister segreg, VerifierValue<32> addr, VerifierValue<Len> data, VerifierValue<1> cond) {
        writeMemory(segreg, addr, data, VerifierValue<32>(1), cond);
    }

    /* Writes data starting at the specified address and repeating. */
    template <size_t Len>
    void writeMemory(X86SegmentRegister segreg, VerifierValue<32> addr, VerifierValue<Len> data, VerifierValue<32> repeat,
                     VerifierValue<1> cond) {
        if (cond.uv()!=0) {
            assert(0==Len%8);
            size_t nbytes = Len/8;
            uint64_t at = addr.uv();
            for (uint64_t i=0; i<repeat.uv(); i++) {
                if (trace_file)
                    (*trace_file) <<trace_prefix <<"0x" <<std::hex <<std::setw(Len/4) <<std::setfill('0') <<data.uv()
                                  <<" -> mem[0x" <<std::hex <<std::setw(32/4) <<std::setfill('0') <<at <<"]\n";
                for (size_t j=0; j<nbytes; j++) {
                    unsigned char byte = (data.uv() >> (8*j)) & 0xff;
                    memory.insert(std::pair<uint64_t, unsigned char>(at++, byte));
                }
            }
        }
    }
    
    /* Given two values, A and B, concatinates the bits so the return value contains A in the low-order bits and B in the
     * higher order bits just above A. */
    template<size_t Len1, size_t Len2>
    VerifierValue<Len1+Len2> concat(VerifierValue<Len1> a, VerifierValue<Len2> b) {
        return a.uv() | (b.uv() << Len1);
    }

    /* Given two values, A and B, return the Boolean bit-wise AND. */
    template<size_t Len>
    VerifierValue<Len> and_(VerifierValue<Len> a, VerifierValue<Len> b) {
        return a.uv() & b.uv();
    }
    
    /* Given two values, A and B, return the Boolean bit-wise OR. */
    template<size_t Len>
    VerifierValue<Len> or_(VerifierValue<Len> a, VerifierValue<Len> b) {
        return a.uv() | b.uv();
    }
    
    /* Given two values, A and B, return the Boolean bit-wise XOR. */
    template<size_t Len>
    VerifierValue<Len> xor_(VerifierValue<Len> a, VerifierValue<Len> b) {
        return a.uv() ^ b.uv();
    }

    /* Computes three-arg XOR */
    template<size_t Len>
    VerifierValue<Len> xor3(VerifierValue<Len> a, VerifierValue<Len> b, VerifierValue<Len> c) {
        return a.uv() ^ b.uv() ^ c.uv();
    }
    
    /* Adds two values and a carry bit. */
    template<size_t Len>
    VerifierValue<Len> add3(VerifierValue<Len> a, VerifierValue<Len>b, VerifierValue<1> carry) {
        return a.uv() + b.uv() + carry.uv();
    }
    
    /* Given two values, A and B, return the sum. */
    template<size_t Len>
    VerifierValue<Len> add(VerifierValue<Len> a, VerifierValue<Len> b) {
        return a.uv() + b.uv();
    }

    template<size_t Len>
    VerifierValue<Len> addWithCarries(VerifierValue<Len> a, VerifierValue<Len> b,
                                      VerifierValue<1> carryIn, VerifierValue<Len> &carriesOut) {
        VerifierValue<Len+1> aa = extendByMSB<Len, Len+1>(a);
        VerifierValue<Len+1> bb = extendByMSB<Len, Len+1>(b);
        VerifierValue<Len+1> result = add3(aa, bb, carryIn);
        carriesOut = extract<1, Len+1>(xor3(aa, bb, result));
        return extract<0, Len>(result);
    }
    
    /* Return the negative (two's complement) of A */
    template<size_t Len>
    VerifierValue<Len> negate(VerifierValue<Len> a) {
        return add(invert(a), VerifierValue<Len>(1));
    }

    /* Returns the product of signed A and signed B */
    template<size_t Len1, size_t Len2>
    VerifierValue<Len1+Len2> signedMultiply(VerifierValue<Len1> a, VerifierValue<Len2> b) {
        return a.sv() * b.sv();
    }

    /* Returns the product of unsigned A and unsigned B */
    template<size_t Len1, size_t Len2>
    VerifierValue<Len1+Len2> unsignedMultiply(VerifierValue<Len1> a, VerifierValue<Len2> b) {
        return a.uv() * b.uv();
    }

    /* Returns the ratio of signed A to signed B */
    template<size_t Len1, size_t Len2>
    VerifierValue<Len1> signedDivide(VerifierValue<Len1> a, VerifierValue<Len2> b) {
        return a.sv() / b.sv();
    }

    /* Returns the ration of unsigned A to unsigned B */
    template<size_t Len1, size_t Len2>
    VerifierValue<Len1> unsignedDivide(VerifierValue<Len1> a, VerifierValue<Len2> b) {
        return a.uv() / b.uv();
    }

    /* Returns signed A modulo signed B */
    template<size_t Len1, size_t Len2>
    VerifierValue<Len2> signedModulo(VerifierValue<Len1> a, VerifierValue<Len2> b) {
        return a.sv() % b.sv();
    }
    
    /* Returns unsigned A modulo unsigned B */
    template<size_t Len1, size_t Len2>
    VerifierValue<Len2> unsignedModulo(VerifierValue<Len1> a, VerifierValue<Len2> b) {
        return a.uv() % b.uv();
    }
    
    /* Return the one's complement of the supplied value. */
    template<size_t Len>
    VerifierValue<Len> invert(VerifierValue<Len> a) {
        return ~a.uv();
    }
    
    /* Conditional express (not sure where the name "ite" comes from; perhaps "integer triplet expression"?) */
    template<size_t Len>
    VerifierValue<Len> ite(VerifierValue<1> sel, VerifierValue<Len> a, VerifierValue<Len> b) {
        return sel.uv() ? a : b;
    }
    
    /* Return true if the value is equal to zero */
    template<size_t Len>
    VerifierValue<1> equalToZero(VerifierValue<Len> a) {
        return a.uv() ? false_() : true_();
    }

    /* Shift bits of A left by B bits */
    template<size_t Len1, size_t Len2>
    VerifierValue<Len1> shiftLeft(VerifierValue<Len1> a, VerifierValue<Len2> b) {
        return a.uv() << b.uv();
    }

    /* Shift bits of A right by B bits without sign extension. */
    template<size_t Len1, size_t Len2>
    VerifierValue<Len1> shiftRight(VerifierValue<Len1> a, VerifierValue<Len2> b) {
        if (b.uv()>=Len1)
            return VerifierValue<Len1>(0);
        return a.uv() >> b.uv();
    }

    /* Shift bits of A right by B bits with sign extension. */
    template<size_t Len1, size_t Len2>
    VerifierValue<Len1> shiftRightArithmetic(VerifierValue<Len1> a, VerifierValue<Len2> b) {
        if (b.uv()>=Len1)
            return a.sv()<0 ? (uint64_t)-1 : 0;
        return a.sv() >> (int64_t)b.uv();
    }

    /* Rotate bits of A left by B bits. */
    template<size_t Len1, size_t Len2>
    VerifierValue<Len1> rotateLeft(VerifierValue<Len1> a, VerifierValue<Len2> b) {
        size_t count = b.uv() % Len1;
        return (a.uv()<<count) | (a.uv()>>(Len1-count));
    }
    
    /* Rotate bits of A right by B bits. */
    template<size_t Len1, size_t Len2>
    VerifierValue<Len1> rotateRight(VerifierValue<Len1> a, VerifierValue<Len2> b) {
        size_t count = b.uv() % Len1;
        return (a.uv()>>count) | (a.uv()<<(Len1-count));
    }

    /* Return the position of the least significant set bit. Returns zero if all bits are clear. */
    template<size_t Len>
    VerifierValue<Len> leastSignificantSetBit(VerifierValue<Len> a) {
        uint64_t v=a.uv(), i;
        for (i=0; i<(uint64_t)Len; i++) {
            if (v & ((uint64_t)1 << i)) {
                return i;
            }
        }
        return 0;
    }

    /* Returns the position of the most significant set bit. Returns zero if all bits are clear. */
    template<size_t Len>
    VerifierValue<Len> mostSignificantSetBit(VerifierValue<Len> a) {
        uint64_t v=a.uv(), i;
        for (i=0; i<(uint64_t)Len; --i) {
            if (v & ((uint64_t)1 << (i-1))) {
                return i;
            }
        }
        return 0;
    }

private:
    Debugger *debugger;
    RegisterSet register_set;
    bool registers_current;                     /* Is the register_set up to date? */
    std::map<uint64_t, unsigned char> memory;   /* Modified memory values */
    std::ostream *trace_file;
};


/* Prints values of all registers */
static void dump_registers(FILE *f, const RegisterSet &rs) {
    for (size_t i=0; i<NELMTS(x86_reg_names); i++) {
        int w = x86_reg_size[i]/4;
        fprintf(f, "    %-10s = 0x%0*"PRIx64"\n", x86_reg_str[i], w, rs.reg[x86_reg_names[i]]);
    }
}

int main(int argc, char *argv[]) {
    std::ios_base::sync_with_stdio(true);
    std::string hostname = "localhost";
    short port = 32002;

    /* Process command-line arguments. ROSE will do most of the work, but we need to look for the --debugger switch. */
    for (int argno=1; argno<argc; argno++) {
        if (!strncmp(argv[argno], "--debugger=", 11)) {
            char *colon = strchr(argv[argno]+11, ':');
            if (colon) {
                hostname = std::string(argv[argno]+11, colon-(argv[argno]+11));
                char *rest;
                port = strtol(colon+1, &rest, 0);
                if (rest && *rest) {
                    fprintf(stderr, "invalid argument for --debugger=HOST:PORT switch\n");
                    exit(1);
                }
            } else {
                hostname = argv[argno]+11;
            }
            memmove(argv+argno, argv+argno+1, (argc-(argno+1))*sizeof(*argv));
            --argc;
            break;
        }
    }
    fprintf(stderr, "Parsing and disassembling executable...\n");
    SgProject *project = frontend(argc, argv);

    /* Connect to debugger */
    fprintf(stderr, "Connecting to debugger at %s:%d\n", hostname.c_str(), port);
    Debugger dbg(hostname, port);

    /* Choose an interpretation */
    SgAsmInterpretation *interp = isSgAsmInterpretation(NodeQuery::querySubTree(project, V_SgAsmInterpretation).back());
    assert(interp);

    /* Get the instruction mapping (and some other stuff we don't really need) */
    fprintf(stderr, "Calculating call graph...\n");
    VirtualBinCFG::AuxiliaryInformation aux_info(interp);

    /* Set breakpoints in every executable segment. These breakpoints correspond to the instructions we want to analyze. We set
     * breakpoints by section granularity for efficiency. */
    fprintf(stderr, "Setting break points...\n");
    const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
    for (size_t i=0; i<headers.size(); i++) {
        std::vector<SgNode*> nodes = NodeQuery::querySubTree(headers[i], V_SgAsmGenericSection);
        assert(nodes.size()>0);
        for (size_t j=0; j<nodes.size(); j++) {
            SgAsmGenericSection *section = isSgAsmGenericSection(nodes[j]);
            assert(section);
            if (section->is_mapped() && section->get_mapped_xperm()) {
                int status = dbg.setbp(section->get_mapped_actual_rva(), section->get_mapped_size());
                assert(status>=0);
            }
        }
    }

    /* Initialize verifier */
    fprintf(stderr, "Starting executable...\n");
    uint64_t nprocessed = 0, nerrors = 0;
    Verifier verifier(&dbg);

#ifndef USE_ROSE
    X86InstructionSemantics<Verifier, VerifierValue> semantics(verifier);
#endif

    dbg.cont(); /* Advance to the first breakpoint. */

    /* Each time we hit a breakpoint, find the instruction at that address and run it through the verifier. */
    while (1) {
        if (0 == ++nprocessed % 100)
            fprintf(stderr, "verified %"PRIu64" instructions; %"PRIu64" error%s so far.\n",
                    nprocessed, nerrors, 1==nerrors?"":"s");

        /* Look up instruction. Failure to find an instruction results from one of these two conditions:
         *     1. Rose was unable to disassemble the instruction
         *     2. We single-stepped to an address that's not known to the static analysis
         * In either case of failure we just continue until the next breakpoint. */
        SgAsmInstruction *insn = aux_info.getInstructionAtAddress(dbg.rip());
        if (!insn) {
            dbg.cont();
            continue;
        }
        SgAsmx86Instruction *insn_x86 = isSgAsmx86Instruction(insn);
        assert(insn_x86);

        /* Process instruction semantics. */
        std::ostringstream trace;

#ifndef USE_ROSE
        try {
            verifier.trace(&trace);
            semantics.processInstruction(insn_x86);
        } catch (const Verifier::Exception &e) {
            nerrors++;
            fprintf(stderr, "Error at 0x%016"PRIx64" (#%"PRIu64"): %s\n%s\n",
                    insn->get_address(), nprocessed, unparseInstruction(insn).c_str(), e.mesg.c_str());
            fputs(trace.str().c_str(), stderr);
            fprintf(stderr, "Actual register set when error was detected:\n");
            dump_registers(stderr, dbg.registers());
            dbg.cont();
            continue;
        } catch (const X86InstructionSemantics<Verifier, VerifierValue>::Exception &e) {
            fprintf(stderr, "%s: %s\n", e.mesg.c_str(), unparseInstructionWithAddress(e.insn).c_str());
        }
#endif

        /* Single step to cause the instruction to be executed remotely. Then compare our state with the remote state. */
        dbg.step();
        try {
            if (verifier.has_registers())
                verifier.assert_registers("    ");
            verifier.assert_memory("   ");
        } catch (const Verifier::Exception &e) {
            nerrors++;
            fprintf(stderr, "Error at 0x%016"PRIx64" (#%"PRIu64"): %s\nstate mismatch:\n%s\n",
                    insn->get_address(), nprocessed, unparseInstruction(insn).c_str(), e.mesg.c_str());
            fputs(trace.str().c_str(), stderr);
            fprintf(stderr, "Actual register set when error was detected:\n");
            dump_registers(stderr, dbg.registers());
            continue;
        }
    }


    exit(1); /*FIXME*/
}

