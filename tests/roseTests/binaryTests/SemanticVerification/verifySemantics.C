/* See README. The basic idea is that ROSE parses the executable and then attaches to a debugger, single stepping through the
 * program and comparing the real execution to ROSE's semantic analysis. */
#include "rose.h"
#include "Debugger.h"
#include "x86InstructionSemantics.h"

static const char *trace_prefix = "    ";

/* Registers names, etc. for x86 32-bit */
static const int x86_reg_names[] = {X86_REG_eip, X86_REG_ebp, X86_REG_esp, X86_REG_orig_eax, X86_REG_eax, X86_REG_ebx,
                                    X86_REG_ecx, X86_REG_edx, X86_REG_xcs, X86_REG_xds, X86_REG_xes, X86_REG_xfs,
                                    X86_REG_xgs, X86_REG_xss, X86_REG_esi, X86_REG_edi};
static const char *x86_reg_str[] = {"eip", "ebp", "esp", "orig_eax", "eax", "ebx", "ecx", "edx", "xcs", "xds", "xes", "xfs", 
                                    "xgs", "xss", "esi", "edi"};
static const char x86_reg_size[] = {32,    32,    32,    32,         32,    32,    32,    32,    16,    16,    16,    16,
                                    16,    16,    32,    32};


/** Values used by X86InstructionsSemantics with the Verifier policy defined below. */
template<size_t Nbits>
class VerifierValue {
public:
    VerifierValue(): v_(0) {
        assert(Nbits<=8*sizeof v_);
        mask = (uint64_t)-1 >> (8*sizeof(v_)-Nbits);
    }
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
        :debugger(dbg), regdict(NULL), registers_current(false), trace_file(NULL) {
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
        assert(insn->get_size()<=sizeof remote);
        size_t remote_size = debugger->memory(insn->get_address(), insn->get_size(), remote);
        if (remote_size!=insn->get_size()) {
            throw Exception("could not read entire instruction");
        } else if (memcmp(&(insn->get_raw_bytes()[0]), remote, remote_size)) {
            throw Exception("instruction mismatch");
        }
    }

    /* Called after each instruction is processed. */
    void finishInstruction(SgAsmx86Instruction*) {}

    /* Called for the HLT instruction */
    void hlt() {} /*FIXME*/

    /* Called for the CPUID instruction */
    void cpuid() {} /*FIXME*/

    /* Called for the RDTSC instruction */
    VerifierValue<64> rdtsc() {return 0;} /*FIXME*/

    /* Called for the INT instruction */
    void interrupt(uint8_t num) {} /*FIXME*/

    /* Called for the SYSENTER instruction */
    void sysenter() {} /*FIXME*/
    
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

    /* Undefined value. */
    template<size_t Len>
    VerifierValue<Len> undefined_() {
        return 0;
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
            case x86_flag_pf:    bitidx =  2; s = "pf"; break;
            case x86_flag_af:    bitidx =  4; s = "af"; break;
            case x86_flag_zf:    bitidx =  6; s = "zf"; break;
            case x86_flag_sf:    bitidx =  7; s = "sf"; break;
            case x86_flag_tf:    bitidx =  8; s = "tf"; break;
            case x86_flag_if:    bitidx =  9; s = "if"; break;
            case x86_flag_df:    bitidx = 10; s = "df"; break;
            case x86_flag_of:    bitidx = 11; s = "of"; break;
            case x86_flag_nt:    bitidx = 14; s = "nt"; break;
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
            case x86_flag_pf:    bitidx =  2; s = "pf"; break;
            case x86_flag_af:    bitidx =  4; s = "af"; break;
            case x86_flag_zf:    bitidx =  6; s = "zf"; break;
            case x86_flag_sf:    bitidx =  7; s = "sf"; break;
            case x86_flag_tf:    bitidx =  8; s = "tf"; break;
            case x86_flag_if:    bitidx =  9; s = "if"; break;
            case x86_flag_df:    bitidx = 10; s = "df"; break;
            case x86_flag_of:    bitidx = 11; s = "of"; break;
            case x86_flag_nt:    bitidx = 14; s = "nt"; break;
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

    /** Finds a register by name. */
    const RegisterDescriptor& findRegister(const std::string &regname, size_t nbits=0) {
        const RegisterDescriptor *reg = get_register_dictionary()->lookup(regname);
        if (!reg) {
            std::ostringstream ss;
            ss <<"Invalid register: \"" <<regname <<"\"";
            throw Exception(ss.str());
        }
        if (nbits>0 && reg->get_nbits()!=nbits) {
            std::ostringstream ss;
            ss <<"Invalid " <<nbits <<"-bit register: \"" <<regname <<"\" is "
               <<reg->get_nbits() <<" " <<(1==reg->get_nbits()?"byte":"bytes");
            throw Exception(ss.str());
        }
        return *reg;
    }

    /** Reads from a named register. */
    template<size_t Len/*bits*/>
    VerifierValue<Len> readRegister(const char *regname) {
        return readRegister<Len>(findRegister(regname, Len));
    }

    /** Writes to a named register. */
    template<size_t Len/*bits*/>
    void writeRegister(const char *regname, const VerifierValue<Len> &value) {
        writeRegister<Len>(findRegister(regname, Len), value);
    }

    /* Generic register read. */
    template<size_t Len>
    VerifierValue<Len> readRegister(const RegisterDescriptor &reg) {
        assert(!"Use only specialized versions.");
        abort();
    }

    /* Generic register write. */
    template<size_t Len>
    void writeRegister(const RegisterDescriptor &reg, const VerifierValue<Len> &value) {
        assert(!"Use only specialized versions.");
        abort();
    }

    /** Returns the register dictionary. */
    const RegisterDictionary *get_register_dictionary() const {
        return regdict ? regdict : RegisterDictionary::dictionary_pentium4();
    }

    /** Sets the register dictionary. */
    void set_register_dictionary(const RegisterDictionary *regdict) {
        this->regdict = regdict;
    }

    /* Reads the contents of some memory location. */
    template <size_t Len>
    VerifierValue<Len> readMemory(X86SegmentRegister segreg, VerifierValue<32> addrval, VerifierValue<1> cond) {
        assert(0==Len%8);
        if (cond.uv()) {
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
        } else {
            /* Semantics layer will ultimately discard this value, so it's arbitrary. However, the semantics layer might
             * perform a few operations with it first, so we'll return a non-zero value in case one of those operations happens
             * to be a divide. */
            return VerifierValue<Len>(1);
        }
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
    const RegisterDictionary *regdict;
    bool registers_current;                     /* Is the register_set up to date? */
    std::map<uint64_t, unsigned char> memory;   /* Modified memory values */
    std::ostream *trace_file;
};

/******************************************************************************************************************************
 * The X86InstructionSemantics calls readRegister() and writeRegister() now rather than the multitude of read* and write*
 * methods. However, since the Verifier class was written before this change, these new readRegister() and writeRegister()
 * methods just dispatch to the old ones.
 ******************************************************************************************************************************/
template<>
VerifierValue<1> Verifier::readRegister<1>(const RegisterDescriptor &reg)
{
    assert(reg.get_nbits()==1);
    assert(reg.get_major()==x86_regclass_flags);
    assert(reg.get_minor()==0);
    return readFlag((X86Flag)reg.get_offset());
}

template<>
VerifierValue<8> Verifier::readRegister<8>(const RegisterDescriptor &reg)
{
    assert(reg.get_nbits()==8);
    assert(reg.get_major()==x86_regclass_gpr);
    if (0==reg.get_offset())
        return extract<0, 8>(readGPR((X86GeneralPurposeRegister)reg.get_minor()));
    assert(8==reg.get_offset());
    return extract<8, 16>(readGPR((X86GeneralPurposeRegister)reg.get_minor()));
}

template<>
VerifierValue<16> Verifier::readRegister<16>(const RegisterDescriptor &reg)
{
    assert(reg.get_nbits()==16);
    assert(reg.get_offset()==0);
    if (reg.get_major()==x86_regclass_segment) {
        return readSegreg((X86SegmentRegister)reg.get_minor());
    } else if (reg.get_major()==x86_regclass_gpr) {
        return extract<0, 16>(readGPR((X86GeneralPurposeRegister)reg.get_minor()));
    } else {
        assert(reg.get_major()==x86_regclass_flags);
        return concat(readFlag((X86Flag)0),
               concat(readFlag((X86Flag)1),
               concat(readFlag((X86Flag)2),
               concat(readFlag((X86Flag)3),
               concat(readFlag((X86Flag)4),
               concat(readFlag((X86Flag)5),
               concat(readFlag((X86Flag)6),
               concat(readFlag((X86Flag)7),
               concat(readFlag((X86Flag)8),
               concat(readFlag((X86Flag)9),
               concat(readFlag((X86Flag)10),
               concat(readFlag((X86Flag)11),
               concat(readFlag((X86Flag)12),
               concat(readFlag((X86Flag)13),
               concat(readFlag((X86Flag)14),
                      readFlag((X86Flag)15))))))))))))))));
    }
}

template<>
VerifierValue<32> Verifier::readRegister<32>(const RegisterDescriptor &reg)
{
    assert(reg.get_nbits()==32);
    assert(reg.get_offset()==0);

    if (reg.get_major()==x86_regclass_gpr) {
        return readGPR((X86GeneralPurposeRegister)reg.get_minor());
    } else if (reg.get_major()==x86_regclass_ip) {
        assert(reg.get_minor()==0);
        return readIP();
    } else {
        assert(reg.get_major()==x86_regclass_flags);
        const RegisterDescriptor *reg_flags16 = get_register_dictionary()->lookup("flags");
        assert(reg_flags16!=NULL);
        return concat(readRegister<16>(*reg_flags16),
               concat(readFlag((X86Flag)16),
               concat(readFlag((X86Flag)17),
               concat(readFlag((X86Flag)18),
               concat(readFlag((X86Flag)19),
               concat(readFlag((X86Flag)20),
               concat(readFlag((X86Flag)21),
               concat(readFlag((X86Flag)22),
               concat(readFlag((X86Flag)23),
               concat(readFlag((X86Flag)24),
               concat(readFlag((X86Flag)25),
               concat(readFlag((X86Flag)26),
               concat(readFlag((X86Flag)27),
               concat(readFlag((X86Flag)28),
               concat(readFlag((X86Flag)29),
               concat(readFlag((X86Flag)30),
                      readFlag((X86Flag)31)))))))))))))))));
    }
}

template<>
void Verifier::writeRegister<1>(const RegisterDescriptor &reg, const VerifierValue<1> &value)
{
    /* Flags are identified by their bit offset within the containing FLAGS register. */
    assert(reg.get_nbits()==1);
    assert(reg.get_major()==x86_regclass_flags);
    assert(reg.get_minor()==0);
    writeFlag((X86Flag)reg.get_offset(), value);
}

template<>
void Verifier::writeRegister<8>(const RegisterDescriptor &reg, const VerifierValue<8> &value)
{
    assert(reg.get_nbits()==8);
    assert(reg.get_major()==x86_regclass_gpr);
    if (0==reg.get_offset()) {
        writeGPR((X86GeneralPurposeRegister)reg.get_minor(),
                 concat(value, extract<8, 32>(readGPR((X86GeneralPurposeRegister)reg.get_minor()))));
    } else {
        assert(reg.get_offset()==8);
        writeGPR((X86GeneralPurposeRegister)reg.get_minor(),
                 concat(extract<0, 8>(readGPR((X86GeneralPurposeRegister)reg.get_minor())),
                        concat(value, extract<16, 32>(readGPR((X86GeneralPurposeRegister)reg.get_minor())))));
    }
}

template<>
void Verifier::writeRegister<16>(const RegisterDescriptor &reg, const VerifierValue<16> &value)
{
    assert(reg.get_nbits()==16);
    assert(reg.get_offset()==0);
    if (reg.get_major()==x86_regclass_segment) {
        writeSegreg((X86SegmentRegister)reg.get_minor(), value);
    } else {
        assert(reg.get_major()==x86_regclass_gpr);
        writeGPR((X86GeneralPurposeRegister)reg.get_minor(), concat(value, number<16>(0)));
    }
}

template<>
void Verifier::writeRegister<32>(const RegisterDescriptor &reg, const VerifierValue<32> &value)
{
    assert(reg.get_nbits()==32);
    assert(reg.get_offset()==0);
    if (reg.get_major()==x86_regclass_gpr) {
        writeGPR((X86GeneralPurposeRegister)reg.get_minor(), value);
    } else {
        assert(reg.get_major()==x86_regclass_ip);
        assert(0==reg.get_minor());
        writeIP(value);
    }
}















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

#if 1
    Disassembler *disassembler = Disassembler::lookup(interp)->clone();
    assert(disassembler!=NULL);
#else
    /* Get the instruction mapping (and some other stuff we don't really need) */
    fprintf(stderr, "Calculating call graph...\n");
    VirtualBinCFG::AuxiliaryInformation aux_info(interp);
#endif

    /* Set breakpoints in every executable segment. These breakpoints correspond to the instructions we want to analyze. We set
     * breakpoints by section granularity for efficiency. */
    fprintf(stderr, "Setting break points...\n");
#if 1 /*DEBUGGING: Set breakpoint at all addresses. [RPM 2010-08-20]*/
    {
        int status = dbg.setbp(0, 0xffffffff);
        assert(status>=0);
    }
#else
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
#endif

    /* Initialize verifier */
    fprintf(stderr, "Starting executable...\n");
    uint64_t nprocessed = 0, nerrors = 0;
    Verifier verifier(&dbg);

#ifndef USE_ROSE
    BinaryAnalysis::InstructionSemantics::X86InstructionSemantics<Verifier, VerifierValue> semantics(verifier);
#endif

    dbg.cont(); /* Advance to the first breakpoint. */

    /* Each time we hit a breakpoint, find the instruction at that address and run it through the verifier. */
    while (1) {
        if (0 == ++nprocessed % 100)
            fprintf(stderr, "verified %"PRIu64" instructions; %"PRIu64" error%s so far.\n",
                    nprocessed, nerrors, 1==nerrors?"":"s");
#if 1
        unsigned char insn_buf[15];
        size_t nread = dbg.memory(dbg.rip(), sizeof insn_buf, insn_buf);
        SgAsmInstruction *insn = NULL;
        try {
            insn = disassembler->disassembleOne(insn_buf, dbg.rip(), nread, dbg.rip(), NULL);
        } catch(const Disassembler::Exception &e) {
            std::cerr <<"disassembler exception: " <<e <<"\n";
        }
#else
        /* Look up instruction. Failure to find an instruction results from one of these two conditions:
         *     1. Rose was unable to disassemble the instruction
         *     2. We single-stepped to an address that's not known to the static analysis
         * In either case of failure we just continue until the next breakpoint. */
        SgAsmInstruction *insn = aux_info.getInstructionAtAddress(dbg.rip());
#endif
        if (!insn) {
            dbg.cont();
            continue;
        }
        SgAsmx86Instruction *insn_x86 = isSgAsmx86Instruction(insn);
        assert(insn_x86);
#if 1
        /* Trace instructions */
        fprintf(stderr, "[%07"PRIu64"] 0x%08"PRIx64": %s\n", nprocessed, insn->get_address(), unparseInstruction(insn).c_str());
#endif

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
        } catch (const BinaryAnalysis::InstructionSemantics::X86InstructionSemantics<Verifier, VerifierValue>::Exception &e) {
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

