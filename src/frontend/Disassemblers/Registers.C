#include "sage3basic.h"
#include "Registers.h"

std::ostream& operator<<(std::ostream &o, const RegisterDictionary &dict) {
    dict.print(o);
    return o;
}

std::ostream& operator<<(std::ostream &o, const RegisterDescriptor &reg) {
    reg.print(o);
    return o;
}

/* class method */
uint64_t
RegisterDictionary::hash(const RegisterDescriptor &d)
{
    uint64_t h = d.get_major() << 24;
    h ^= d.get_minor() << 16;
    h ^= d.get_offset() << 8;
    h ^= d.get_nbits();
    return h;
}

void
RegisterDictionary::insert(const std::string &name, const RegisterDescriptor &rdesc) {
    /* Erase the name from the reverse lookup map, indexed by the old descriptor. */
    Entries::iterator fi = forward.find(name);
    if (fi!=forward.end()) {
        Reverse::iterator ri = reverse.find(hash(fi->second));
        ROSE_ASSERT(ri!=reverse.end());
        std::vector<std::string>::iterator vi=std::find(ri->second.begin(), ri->second.end(), name);
        ROSE_ASSERT(vi!=ri->second.end());
        ri->second.erase(vi);
    }

    /* Insert or replace old descriptor with a new one and insert reverse lookup info. */
    forward[name] = rdesc;
    reverse[hash(rdesc)].push_back(name);
}

void
RegisterDictionary::insert(const std::string &name, unsigned majr, unsigned minr, unsigned offset, unsigned nbits) {
    insert(name, RegisterDescriptor(majr, minr, offset, nbits));
}

void
RegisterDictionary::insert(const RegisterDictionary &other) {
    const Entries &entries = other.get_registers();
    for (Entries::const_iterator ei=entries.begin(); ei!=entries.end(); ++ei)
        insert(ei->first, ei->second);
}

void
RegisterDictionary::insert(const RegisterDictionary *other) {
    if (other)
        insert(*other);
}

const RegisterDescriptor *
RegisterDictionary::lookup(const std::string &name) const {
    Entries::const_iterator fi = forward.find(name);
    if (fi==forward.end())
        return NULL;
    return &(fi->second);
}

const std::string &
RegisterDictionary::lookup(const RegisterDescriptor &rdesc) const {
    Reverse::const_iterator ri = reverse.find(hash(rdesc));
    if (ri!=reverse.end()) {
        for (size_t i=ri->second.size(); i>0; --i) {
            const std::string &name = ri->second[i-1];
            Entries::const_iterator fi = forward.find(name);
            ROSE_ASSERT(fi!=forward.end());
            if (fi->second.equal(rdesc))
                return name;
        }
    }
    
    static const std::string empty;
    return empty;
}

void
RegisterDictionary::resize(const std::string &name, unsigned new_nbits) {
    const RegisterDescriptor *old_desc = lookup(name);
    ROSE_ASSERT(old_desc!=NULL);
    RegisterDescriptor new_desc = *old_desc;
    new_desc.set_nbits(new_nbits);
    insert(name, new_desc);
}

const RegisterDictionary::Entries &
RegisterDictionary::get_registers() const {
    return forward;
}

RegisterDictionary::Entries &
RegisterDictionary::get_registers() {
    return forward;
}

void
RegisterDictionary::print(std::ostream &o) const {
    o <<"RegisterDictionary \"" <<name <<"\" contains " <<forward.size() <<" " <<(1==forward.size()?"entry":"entries") <<"\n";
    for (Entries::const_iterator ri=forward.begin(); ri!=forward.end(); ++ri)
        o <<"  \"" <<ri->first <<"\" " <<StringUtility::addrToString(hash(ri->second)) <<" " <<ri->second <<"\n";

    for (Reverse::const_iterator ri=reverse.begin(); ri!=reverse.end(); ++ri) {
        o <<"  " <<StringUtility::addrToString(ri->first);
        for (std::vector<std::string>::const_iterator vi=ri->second.begin(); vi!=ri->second.end(); ++vi) {
            o <<" " <<*vi;
        }
        o <<"\n";
    }
}

/** Intel 8086 registers.
 *
 *  The Intel 8086 has fourteen 16-bit registers. Four of them (AX, BX, CX, DX) are general registers (although each may have
 *  an additional purpose; for example only CX can be used as a counter with the loop instruction). Each can be accessed as two
 *  separate bytes (thus BX's high byte can be accessed as BH and low byte as BL). Four segment registers (CS, DS, SS and ES)
 *  are used to form a memory address. There are two pointer registers. SP points to the bottom of the stack and BP which is
 *  used to point at some other place in the stack or the memory(Offset).  Two registers (SI and DI) are for array
 *  indexing. The FLAGS register contains flags such as carry flag, overflow flag and zero flag. Finally, the instruction
 *  pointer (IP) points to the next instruction that will be fetched from memory and then executed. */
const RegisterDictionary *
RegisterDictionary::i8086() {
    static RegisterDictionary *regs = NULL;
    if (!regs) {
        regs = new RegisterDictionary("i8086");

        /*  16-bit general purpose registers. Each has three names depending on which bytes are reference. */
        regs->insert("al", x86_regclass_gpr, x86_gpr_ax, 0, 8);
        regs->insert("ah", x86_regclass_gpr, x86_gpr_ax, 8, 8);
        regs->insert("ax", x86_regclass_gpr, x86_gpr_ax, 0, 16);
        
        regs->insert("bl", x86_regclass_gpr, x86_gpr_bx, 0, 8);
        regs->insert("bh", x86_regclass_gpr, x86_gpr_bx, 8, 8);
        regs->insert("bx", x86_regclass_gpr, x86_gpr_bx, 0, 16);
        
        regs->insert("cl", x86_regclass_gpr, x86_gpr_cx, 0, 8);
        regs->insert("ch", x86_regclass_gpr, x86_gpr_cx, 8, 8);
        regs->insert("cx", x86_regclass_gpr, x86_gpr_cx, 0, 16);
        
        regs->insert("dl", x86_regclass_gpr, x86_gpr_dx, 0, 8);
        regs->insert("dh", x86_regclass_gpr, x86_gpr_dx, 8, 8);
        regs->insert("dx", x86_regclass_gpr, x86_gpr_dx, 0, 16);
        
        /*  16-bit segment registers */
        regs->insert("cs", x86_regclass_segment, x86_segreg_cs, 0, 16);
        regs->insert("ds", x86_regclass_segment, x86_segreg_ds, 0, 16);
        regs->insert("ss", x86_regclass_segment, x86_segreg_ss, 0, 16);
        regs->insert("es", x86_regclass_segment, x86_segreg_es, 0, 16);
        
        /* 16-bit pointer registers */
        regs->insert("sp", x86_regclass_gpr, x86_gpr_sp, 0, 16);        /* stack pointer */
        regs->insert("spl", x86_regclass_gpr, x86_gpr_sp, 0, 8);

        regs->insert("bp", x86_regclass_gpr, x86_gpr_bp, 0, 16);        /* base pointer */
        regs->insert("bpl", x86_regclass_gpr, x86_gpr_bp, 0, 8);

        regs->insert("ip", x86_regclass_ip, 0, 0, 16);                  /* instruction pointer */
        regs->insert("ipl", x86_regclass_ip, 0, 0, 8);
        
        /* Array indexing registers */
        regs->insert("si", x86_regclass_gpr, x86_gpr_si, 0, 16);
        regs->insert("sil", x86_regclass_gpr, x86_gpr_si, 0, 8);

        regs->insert("di", x86_regclass_gpr, x86_gpr_di, 0, 16);
        regs->insert("dil", x86_regclass_gpr, x86_gpr_di, 0, 8);

        /* Flags. Reserved flags have no names but can be accessed by reading the entire "flags" register. */
        regs->insert("flags", x86_regclass_flags, 0,  0, 16);           /* all flags */
        regs->insert("cf",    x86_regclass_flags, 0,  0,  1);           /* carry status flag */
        regs->insert("pf",    x86_regclass_flags, 0,  2,  1);           /* parity status flag */
        regs->insert("af",    x86_regclass_flags, 0,  4,  1);           /* adjust status flag */
        regs->insert("zf",    x86_regclass_flags, 0,  6,  1);           /* zero status flag */
        regs->insert("sf",    x86_regclass_flags, 0,  7,  1);           /* sign status flag */
        regs->insert("tf",    x86_regclass_flags, 0,  8,  1);           /* trap system flag */
        regs->insert("if",    x86_regclass_flags, 0,  9,  1);           /* interrupt enable system flag */
        regs->insert("df",    x86_regclass_flags, 0, 10,  1);           /* direction control flag */
        regs->insert("of",    x86_regclass_flags, 0, 11,  1);           /* overflow status flag */
        regs->insert("nt",    x86_regclass_flags, 0, 14,  1);           /* nested task system flag */
    }
    return regs;
}

/** Intel 8088 registers.
 *
 *  Intel 8088 has the same set of registers as Intel 8086. */
const RegisterDictionary *
RegisterDictionary::i8088()
{
    static RegisterDictionary *regs = NULL;
    if (!regs) {
        regs = new RegisterDictionary("i8088");
        regs->insert(i8086());
    }
    return regs;
}

/** Intel 80286 registers.
 *
 *  The 80286 has the same registers as the 8086 but adds two new flags to the "flags" register. */
const RegisterDictionary *
RegisterDictionary::i286()
{
    static RegisterDictionary *regs = NULL;
    if (!regs) {
        regs = new RegisterDictionary("i286");
        regs->insert(i8086());
        regs->insert("iopl", x86_regclass_flags, 0, 12, 2);             /*  I/O privilege level flag */
        regs->insert("nt",   x86_regclass_flags, 0, 14, 1);             /*  nested task system flag */
    }
    return regs;
}

/** Intel 80386 registers.
 *
 *  The 80386 has the same registers as the 80286 but extends the general-purpose registers, base registers, index registers,
 *  instruction pointer, and flags register to 32 bits.  Register names from the 80286 refer to the same offsets and sizes while
 *  the full 32 bits are accessed by names prefixed with "e" as in "eax" (the "e" means "extended"). Two new segment registers
 *  (FS and GS) were added and all segment registers remain 16 bits. */
const RegisterDictionary *
RegisterDictionary::i386()
{
    static RegisterDictionary *regs = NULL;
    if (!regs) {
        regs = new RegisterDictionary("i386");
        regs->insert(i286());

        /* Additional 32-bit registers */
        regs->insert("eax", x86_regclass_gpr, x86_gpr_ax, 0, 32);
        regs->insert("ebx", x86_regclass_gpr, x86_gpr_bx, 0, 32);
        regs->insert("ecx", x86_regclass_gpr, x86_gpr_cx, 0, 32);
        regs->insert("edx", x86_regclass_gpr, x86_gpr_dx, 0, 32);
        regs->insert("esp", x86_regclass_gpr, x86_gpr_sp, 0, 32);
        regs->insert("ebp", x86_regclass_gpr, x86_gpr_bp, 0, 32);
        regs->insert("eip", x86_regclass_ip, 0, 0, 32);
        regs->insert("esi", x86_regclass_gpr, x86_gpr_si, 0, 32);
        regs->insert("edi", x86_regclass_gpr, x86_gpr_di, 0, 32);
        regs->insert("eflags", x86_regclass_flags, 0, 0, 32);

        /* Additional 16-bit segment registers */
        regs->insert("fs", x86_regclass_segment, x86_segreg_fs, 0, 16);
        regs->insert("gs", x86_regclass_segment, x86_segreg_gs, 0, 16);

        /* Additional flags */
        regs->insert("rf", x86_regclass_flags, 0, 16, 1);               /* resume system flag */
        regs->insert("vm", x86_regclass_flags, 0, 17, 1);               /* virtual 8086 mode flag */

        /* Control registers */
        regs->insert("cr0", x86_regclass_cr, 0, 0, 32);
        regs->insert("cr1", x86_regclass_cr, 1, 0, 32);
        regs->insert("cr2", x86_regclass_cr, 2, 0, 32);
        regs->insert("cr3", x86_regclass_cr, 3, 0, 32);
        regs->insert("cr4", x86_regclass_cr, 4, 0, 32);

        /* Debug registers */
        regs->insert("dr0", x86_regclass_dr, 0, 0, 32);
        regs->insert("dr1", x86_regclass_dr, 1, 0, 32);
        regs->insert("dr2", x86_regclass_dr, 2, 0, 32);
        regs->insert("dr3", x86_regclass_dr, 3, 0, 32);                 /* dr4 and dr5 are reserved */
        regs->insert("dr6", x86_regclass_dr, 6, 0, 32);
        regs->insert("dr7", x86_regclass_dr, 7, 0, 32);
        
    }
    return regs;
}

/** Intel 80486 registers.
 *
 *  The 80486 has the same registers as the 80386 but adds a new flag to the "eflags" register and adds floating-point registers
 *  previously in the 8087, 80287, and 80387 math coprocessors. */
const RegisterDictionary *
RegisterDictionary::i486()
{
    static RegisterDictionary *regs = NULL;
    if (!regs) {
        regs = new RegisterDictionary("i486");
        regs->insert(i386());

        /* Additional flags */
        regs->insert("ac", x86_regclass_flags, 0, 18, 1);               /* alignment check system flag */

        /* The floating point registers names are relative to the current top-of-stack that changes dynamically. The
         * definitions we're creating here are static.  When a floating-point instruction is simulated (e.g., by the
         * instruction semantics analyses) then the simulation will have to make adjustments to the storage descriptors in
         * order for the register names to point to their new storage locations. */
        regs->insert("st(0)", x86_regclass_st, 0, 0, 80);
        regs->insert("st(1)", x86_regclass_st, 1, 0, 80);
        regs->insert("st(2)", x86_regclass_st, 2, 0, 80);
        regs->insert("st(3)", x86_regclass_st, 3, 0, 80);
        regs->insert("st(4)", x86_regclass_st, 4, 0, 80);
        regs->insert("st(5)", x86_regclass_st, 5, 0, 80);
        regs->insert("st(6)", x86_regclass_st, 6, 0, 80);
        regs->insert("st(7)", x86_regclass_st, 7, 0, 80);
    }
    return regs;
}

/** Intel Pentium registers.
 *
 *  The Pentium has the same registers as the 80486 but adds a few flags to the "eflags" register and MMX registers. */
const RegisterDictionary *
RegisterDictionary::pentium()
{
    static RegisterDictionary *regs = NULL;
    if (!regs) {
        regs = new RegisterDictionary("pentium");
        regs->insert(i486());

        /* Additional flags */
        regs->insert("vif", x86_regclass_flags, 0, 19, 1);              /* virtual interrupt flag */
        regs->insert("vip", x86_regclass_flags, 0, 20, 1);              /* virt interrupt pending */
        regs->insert("id",  x86_regclass_flags, 0, 21, 1);              /* ident system flag */

        /* The MMi registers are aliases for the ST(i) registers but are absolute rather than relative to the top of the
         * stack. We're creating the static definitions, so MMi will point to the same storage as ST(i) for 0<=i<=7. Note that
         * a write to one of the 64-bit MMi registers causes the high-order 16 bits of the corresponding ST(j) register to be
         * set to all ones to indicate a NaN value. */
        regs->insert("mm0", x86_regclass_mm, 0, 0, 64);
        regs->insert("mm1", x86_regclass_mm, 1, 0, 64);
        regs->insert("mm2", x86_regclass_mm, 2, 0, 64);
        regs->insert("mm3", x86_regclass_mm, 3, 0, 64);
        regs->insert("mm4", x86_regclass_mm, 4, 0, 64);
        regs->insert("mm5", x86_regclass_mm, 5, 0, 64);
        regs->insert("mm6", x86_regclass_mm, 6, 0, 64);
        regs->insert("mm7", x86_regclass_mm, 7, 0, 64);
    }
    return regs;
}

/** Intel Pentium 4 registers.
 *
 *  The Pentium 4 has the same register set as the Pentium but adds the MMX0 through MMX7 registers for the SSE instruction
 *  set. */
const RegisterDictionary *
RegisterDictionary::pentium4()
{
    static RegisterDictionary *regs = NULL;
    if (!regs) {
        regs = new RegisterDictionary("pentium4");
        regs->insert(pentium());
        regs->insert("mmx0", x86_regclass_xmm, 0, 0, 128);
        regs->insert("mmx1", x86_regclass_xmm, 1, 0, 128);
        regs->insert("mmx2", x86_regclass_xmm, 2, 0, 128);
        regs->insert("mmx3", x86_regclass_xmm, 3, 0, 128);
        regs->insert("mmx4", x86_regclass_xmm, 4, 0, 128);
        regs->insert("mmx5", x86_regclass_xmm, 5, 0, 128);
        regs->insert("mmx6", x86_regclass_xmm, 6, 0, 128);
        regs->insert("mmx7", x86_regclass_xmm, 7, 0, 128);
    }
    return regs;
}

        
/** Amd64 registers.
 *
 *  The AMD64 architecture increases the size of the general purpose registers, base registers, index registers, instruction
 *  pointer, and flags register to 64-bits.  Most register names from the Pentium architecture still exist and refer to 32-bit
 *  quantities, while the AMD64 adds new names that start with "r" rather than "e" (such as "rax" for the 64-bit register and
 *  "eax" for the 32 low-order bits of the same register).  It also adds eight additional 64-bit general purpose registers
 *  named "r8" through "r15" along with "b", "w", and "d" suffixes for the low-order 8, 16, and 32 bits, respectively.
 *
 *  The only registers that are not retained are the control registers cr0-cr4, which are replaced by 64-bit registers of the
 *  same name, and debug registers dr0-dr7, which are also replaced by 64-bit registers of the same name. */
const RegisterDictionary *
RegisterDictionary::amd64()
{
    static RegisterDictionary *regs = NULL;
    if (!regs) {
        regs = new RegisterDictionary("amd64");
        regs->insert(pentium4());

        /* Additional 64-bit (and hi-end 32-bit) registers */
        regs->insert("rax", x86_regclass_gpr, x86_gpr_ax, 0, 64);
        regs->insert("rbx", x86_regclass_gpr, x86_gpr_bx, 0, 64);
        regs->insert("rcx", x86_regclass_gpr, x86_gpr_cx, 0, 64);
        regs->insert("rdx", x86_regclass_gpr, x86_gpr_dx, 0, 64);
        regs->insert("rsp", x86_regclass_gpr, x86_gpr_sp, 0, 64);
        regs->insert("rbp", x86_regclass_gpr, x86_gpr_bp, 0, 64);
        regs->insert("rsi", x86_regclass_gpr, x86_gpr_si, 0, 64);
        regs->insert("rdi", x86_regclass_gpr, x86_gpr_di, 0, 64);
        regs->insert("rip", x86_regclass_ip, 0, 0, 64);
        regs->insert("rflags", x86_regclass_flags, 0, 0, 64);

        for (unsigned i=8; i<16; i++) {
            /* New general purpose registers in various widths */
            std::string name = "r" + StringUtility::numberToString(i);
            regs->insert(name,     x86_regclass_gpr, i, 0, 64);
            regs->insert(name+"b", x86_regclass_gpr, i, 0,  8);
            regs->insert(name+"w", x86_regclass_gpr, i, 0, 16);
            regs->insert(name+"d", x86_regclass_gpr, i, 0, 32);

            /* New media MMX registers */
            regs->insert(std::string("mmx")+StringUtility::numberToString(i),
                         x86_regclass_xmm, i, 0, 128);
        }

        /* Control registers become 64 bits, and cr8 is added */
        regs->resize("cr0", 64);
        regs->resize("cr1", 64);
        regs->resize("cr2", 64);
        regs->resize("cr3", 64);
        regs->resize("cr4", 64);
        regs->insert("cr8", x86_regclass_cr, 8, 0, 64);

        /* Debug registers become 64 bits */
        regs->resize("dr0", 64);
        regs->resize("dr1", 64);
        regs->resize("dr2", 64);
        regs->resize("dr3", 64);                                /* dr4 and dr5 are reserved */
        regs->resize("dr6", 64);
        regs->resize("dr7", 64);
    }
    return regs;
}

/** ARM7 registers.
 *
 * The CPU has a total of 37 registers, each 32 bits wide: 31 general purpose registers named and six status registers named.
 * At most 16 (8 in Thumb mode) general purpose registers are visible at a time depending on the mode of operation. They have
 * names rN where N is an integer between 0 and 15, inclusive and are mapped onto a subset of the 31 physical general purpose
 * registers. Register r13 and r14 are, by convention, a stack pointer and link register (the link register holds the return
 * address for a function call). Register r15 is the instruction pointer.  Also, at most two status registers are available at
 * a time.
 *
 * The major number of a RegisterDescriptor is used to indicate the type of register: 0=general purpose, 1=status. The minor
 * number indicates the register number: 0-15 for general purpose, 0 or 1 for status. */
const RegisterDictionary *
RegisterDictionary::arm7() {
    /* Documentation of the Nintendo GameBoy Advance is pretty decent. It's located here:
     * http:// nocash.emubase.de/gbatek.htm */
    static RegisterDictionary *regs = NULL;
    if (!regs) {
        regs = new RegisterDictionary("arm7");

        /* The (up-to) 16 general purpose registers available within the current mode. */
        for (unsigned i=0; i<16; i++)
            regs->insert("r"+StringUtility::numberToString(i), arm_regclass_gpr, i, 0, 32);

        /* The (up to) two status registers available within the current mode. */
        regs->insert("cpsr", arm_regclass_psr, arm_psr_current, 0, 32);      /* current program status register */
        regs->insert("spsr", arm_regclass_psr, arm_psr_saved,   0, 32);      /* saved program status register */

        /* Individual parts of the cpsr register */
        regs->insert("cpsr_m", arm_regclass_psr, arm_psr_current,  0, 5);    /* Mode bits indicating current operating mode */
        regs->insert("cpsr_t", arm_regclass_psr, arm_psr_current,  5, 1);    /* State bit (0=>ARM; 1=>THUMB) */
        regs->insert("cpsr_f", arm_regclass_psr, arm_psr_current,  6, 1);    /* FIQ disable (0=>enable; 1=>disable) */
        regs->insert("cpsr_i", arm_regclass_psr, arm_psr_current,  7, 1);    /* IRQ disable (0=>enable; 1=>disable) */
        regs->insert("cpsr_q", arm_regclass_psr, arm_psr_current, 27, 1);    /* sticky overflow (ARMv5TE and up only) */
        regs->insert("cpsr_v", arm_regclass_psr, arm_psr_current, 28, 1);    /* overflow flag (0=>no overflow; 1=overflow) */
        regs->insert("cpsr_c", arm_regclass_psr, arm_psr_current, 29, 1);    /* carry flag (1=>no carry; 1=>carry) */
        regs->insert("cpsr_z", arm_regclass_psr, arm_psr_current, 30, 1);    /* zero flag (0=>not zero; 1=>zero) */
        regs->insert("cpsr_n", arm_regclass_psr, arm_psr_current, 31, 1);    /* sign flag (0=>not signed; 1=>signed) */

        /* The six spsr registers (at most one available depending on the operating mode), have the same bit fields as the cpsr
         * register. When an exception occurs, the current status (in cpsr) is copied to one of the spsr registers. */
        regs->insert("spsr_m", arm_regclass_psr, arm_psr_saved,  0, 5);    /* Mode bits indicating saved operating mode */
        regs->insert("spsr_t", arm_regclass_psr, arm_psr_saved,  5, 1);    /* State bit (0=>ARM; 1=>THUMB) */
        regs->insert("spsr_f", arm_regclass_psr, arm_psr_saved,  6, 1);    /* FIQ disable (0=>enable; 1=>disable) */
        regs->insert("spsr_i", arm_regclass_psr, arm_psr_saved,  7, 1);    /* IRQ disable (0=>enable; 1=>disable) */
        regs->insert("spsr_q", arm_regclass_psr, arm_psr_saved, 27, 1);    /* sticky overflow (ARMv5TE and up only) */
        regs->insert("spsr_v", arm_regclass_psr, arm_psr_saved, 28, 1);    /* overflow flag (0=>no overflow; 1=overflow) */
        regs->insert("spsr_c", arm_regclass_psr, arm_psr_saved, 29, 1);    /* carry flag (1=>no carry; 1=>carry) */
        regs->insert("spsr_z", arm_regclass_psr, arm_psr_saved, 30, 1);    /* zero flag (0=>not zero; 1=>zero) */
        regs->insert("spsr_n", arm_regclass_psr, arm_psr_saved, 31, 1);    /* sign flag (0=>not signed; 1=>signed) */
    }
    return regs;
}

/** PowerPC registers. */
const RegisterDictionary *
RegisterDictionary::powerpc()
{
    static RegisterDictionary *regs = NULL;
    if (!regs) {
        regs = new RegisterDictionary("powerpc");

        /**********************************************************************************************************************
         * General purpose and floating point registers
         **********************************************************************************************************************/
        for (unsigned i=0; i<32; i++) {
            regs->insert("r"+StringUtility::numberToString(i), powerpc_regclass_gpr, i, 0, 32);
            regs->insert("f"+StringUtility::numberToString(i), powerpc_regclass_fpr, i, 0, 64);
        }

        /**********************************************************************************************************************
         * State, status, condition, control registers
         **********************************************************************************************************************/

        /* Machine state register */
        regs->insert("msr", powerpc_regclass_msr, 0, 0, 32);

        /* Floating point status and control register */
        regs->insert("fpscr", powerpc_regclass_fpscr, 0, 0, 32);

        /* Condition Register. This register is grouped into eight fields, where each field is 4 bits. Many PowerPC
         * instructions define bit 31 of the instruction encoding as the Rc bit, and some instructions imply an Rc value equal
         * to 1. When Rc is equal to 1 for integer operations, the CR field 0 is set to reflect the result of the instruction's
         * operation: Equal (EQ), Greater Than (GT), Less Than (LT), and Summary Overflow (SO). When Rc is equal to 1 for
         * floating-point operations, the CR field 1 is set to reflect the state of the exception status bits in the FPSCR: FX,
         * FEX, VX, and OX. Any CR field can be the target of an integer or floating-point comparison instruction. The CR field
         * 0 is also set to reflect the result of a conditional store instruction (stwcx or stdcx). There is also a set of
         * instructions that can manipulate a specific CR bit, a specific CR field, or the entire CR, usually to combine
         * several conditions into a single bit for testing. */
        regs->insert("cr", powerpc_regclass_cr, 0, 0, 32);
        for (unsigned i=0; i<32; i++) {
            switch (i%4) {
                case 0:
                    regs->insert("cr"+StringUtility::numberToString(i/4), powerpc_regclass_cr, 0, i, 4);
                    regs->insert("cr"+StringUtility::numberToString(i/4)+"*4+lt", powerpc_regclass_cr, 0, i, 1);
                    break;
                case 1:
                    regs->insert("cr"+StringUtility::numberToString(i/4)+"*4+gt", powerpc_regclass_cr, 0, i, 1);
                    break;
                case 2:
                    regs->insert("cr"+StringUtility::numberToString(i/4)+"*4+eq", powerpc_regclass_cr, 0, i, 1);
                    break;
                case 3:
                    regs->insert("cr"+StringUtility::numberToString(i/4)+"*4+so", powerpc_regclass_cr, 0, i, 1);
                    break;
            }
        }

        /* The processor version register is a 32-bit read-only register that identifies the version and revision level of the
         * processor. Processor versions are assigned by the PowerPC architecture process. Revision levels are implementation
         * defined. Access to the register is privileged, so that an application program can determine the processor version
         * only with the help of an operating system function. */
        regs->insert("pvr", powerpc_regclass_pvr, 0, 0, 32);

        /**********************************************************************************************************************
         * The instruction address register is a pseudo register. It is not directly available to the user other than through a
         * "branch and link" instruction. It is primarily used by debuggers to show the next instruction to be executed.
         **********************************************************************************************************************/
        regs->insert("iar", powerpc_regclass_iar, 0, 0, 32);

        /**********************************************************************************************************************
         * Special purpose registers. There are 1024 of these, some of which have special names.  We name all 1024 consistently
         * and create aliases for the special ones. This allows the disassembler to look them up generically.  Because the
         * special names appear after the generic names, a reverse lookup will return the special name.
         **********************************************************************************************************************/
        /* Generic names for them all */
        for (unsigned i=0; i<1024; i++)
            regs->insert("spr"+StringUtility::numberToString(i), powerpc_regclass_spr, i, 0, 32);

        /* The link register contains the address to return to at the end of a function call.  Each branch instruction encoding
         * has an LK bit. If the LK bit is 1, the branch instruction moves the program counter to the link register. Also, the
         * conditional branch instruction BCLR branches to the value in the link register. */
        regs->insert("lr", powerpc_regclass_spr, powerpc_spr_lr, 0, 32);

        /* The fixed-point exception register contains carry and overflow information from integer arithmetic operations. It
         * also contains carry input to certain integer arithmetic operations and the number of bytes to transfer during load
         * and store string instructions, lswx and stswx. */
        regs->insert("xer", powerpc_regclass_spr, powerpc_spr_xer, 0, 32);

        /* The count register contains a loop counter that is decremented on certain branch operations. Also, the conditional
         * branch instruction bcctr branches to the value in the CTR. */
        regs->insert("ctr", powerpc_regclass_spr, powerpc_spr_ctr, 0, 32);

        /* Other special purpose registers. */
        regs->insert("dsisr", powerpc_regclass_spr, powerpc_spr_dsisr, 0, 32);
        regs->insert("dar", powerpc_regclass_spr, powerpc_spr_dar, 0, 32);
        regs->insert("dec", powerpc_regclass_spr, powerpc_spr_dec, 0, 32);

        /**********************************************************************************************************************
         * Time base registers. There are 1024 of these, some of which have special names. We name all 1024 consistently and
         * create aliases for the special ones. This allows the disassembler to look them up generically.  Because the special
         * names appear after the generic names, a reverse lookup will return the special name.
         **********************************************************************************************************************/
        for (unsigned i=0; i<1024; i++)
            regs->insert("tbr"+StringUtility::numberToString(i), powerpc_regclass_tbr, i, 0, 32);

        regs->insert("tbl", powerpc_regclass_tbr, powerpc_tbr_tbl, 0, 32);      /* time base lower */
        regs->insert("tbu", powerpc_regclass_tbr, powerpc_tbr_tbu, 0, 32);      /* time base upper */
    }
    return regs;
}
