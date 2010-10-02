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

void
RegisterDictionary::insert(const std::string &name, const RegisterDescriptor &rdesc) {
    regs[name] = rdesc;
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
    Entries::const_iterator found = regs.find(name);
    if (found==regs.end())
        return NULL;
    return &(found->second);
}

RegisterDescriptor *
RegisterDictionary::lookup(const std::string &name) {
    Entries::iterator found = regs.find(name);
    if (found==regs.end())
        return NULL;
    return &(found->second);
}

const std::string &
RegisterDictionary::lookup(const RegisterDescriptor &rdesc) const {
    /* This might be too slow. We may need to build a reverse lookup map. [RPM 2010-10-01] */
    for (Entries::const_iterator ri=regs.begin(); ri!=regs.end(); ++ri) {
        if (rdesc.equal(ri->second))
            return ri->first;
    }
    static const std::string empty;
    return empty;
}

const RegisterDictionary::Entries &
RegisterDictionary::get_registers() const {
    return regs;
}

RegisterDictionary::Entries &
RegisterDictionary::get_registers() {
    return regs;
}

void
RegisterDictionary::print(std::ostream &o) const {
    o <<"RegisterDictionary \"" <<name <<"\" contains " <<regs.size() <<" " <<(1==regs.size()?"entry":"entries") <<"\n";
    for (Entries::const_iterator ri=regs.begin(); ri!=regs.end(); ++ri)
        o <<"  \"" <<ri->first <<"\" " <<ri->second <<"\n";
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
 *  pointer, and flags register to 64-bits.  Register names from the Pentium architecture still exist and refer to 32-bit
 *  quantities, while the AMD64 adds new names that start with "r" rather than "e" (such as "rax" for the 64-bit register and
 *  "eax" for the 32 low-order bits of the same register).  It also adds eight additional 64-bit general purpose registers
 *  named "r8" through "r15". */
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
    }
    return regs;
}
