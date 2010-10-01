#include "rose.h"

std::ostream& operator<<(std::ostream &o, const RegisterDescriptor &reg) {
    reg.print(o);
    return o;
}






///** Intel 8086 registers. The Intel 8086 has fourteen 16-bit registers. Four of them (AX, BX, CX, DX) are general registers
// *  (although each may have an additional purpose; for example only CX can be used as a counter with the loop
// *  instruction). Each can be accessed as two separate bytes (thus BX's high byte can be accessed as BH and low byte as
// *  BL). Four segment registers (CS, DS, SS and ES) are used to form a memory address. There are two pointer registers. SP
// *  points to the bottom of the stack and BP which is used to point at some other place in the stack or the memory(Offset).
// *  Two registers (SI and DI) are for array indexing. The FLAGS register contains flags such as carry flag, overflow flag and
// *  zero flag. Finally, the instruction pointer (IP) points to the next instruction that will be fetched from memory and then
// *  executed. */
//const RegisterSet* RegisterSet::i8086() {
//    static RegisterSet *rs = NULL;
//    if (!rs) {
//        rs = new RegisterSet("Intel 8086");
//        size_t sd=0; /*register storage descriptor*/
//
//        /* 16-bit general purpose registers; each has three names depending one which bytes are referenced. */
//        sd = rs->new_storage();
//        rs->insert(new RegisterDefn("al", RegisterDefn::REGCAT_GENERAL, sd, 0,  8));
//        rs->insert(new RegisterDefn("ah", RegisterDefn::REGCAT_GENERAL, sd, 8,  8));
//        rs->insert(new RegisterDefn("ax", RegisterDefn::REGCAT_GENERAL, sd, 0, 16));
//
//        sd = rs->new_storage();
//        rs->insert(new RegisterDefn("bl", RegisterDefn::REGCAT_GENERAL, sd, 0,  8));
//        rs->insert(new RegisterDefn("bh", RegisterDefn::REGCAT_GENERAL, sd, 8,  8));
//        rs->insert(new RegisterDefn("bx", RegisterDefn::REGCAT_GENERAL, sd, 0, 16));
//        
//        sd = rs->new_storage();
//        rs->insert(new RegisterDefn("cl", RegisterDefn::REGCAT_GENERAL, sd, 0,  8));
//        rs->insert(new RegisterDefn("ch", RegisterDefn::REGCAT_GENERAL, sd, 8,  8));
//        rs->insert(new RegisterDefn("cx", RegisterDefn::REGCAT_GENERAL, sd, 0, 16));
//        
//        sd = rs->new_storage();
//        rs->insert(new RegisterDefn("dl", RegisterDefn::REGCAT_GENERAL, sd, 0,  8));
//        rs->insert(new RegisterDefn("dh", RegisterDefn::REGCAT_GENERAL, sd, 8,  8));
//        rs->insert(new RegisterDefn("dx", RegisterDefn::REGCAT_GENERAL, sd, 0, 16));
//
//        /* Segment registers */
//        rs->insert(new RegisterDefn("cs", RegisterDefn::REGCAT_SEGMENT, rs->new_storage(), 0, 16));
//        rs->insert(new RegisterDefn("ds", RegisterDefn::REGCAT_SEGMENT, rs->new_storage(), 0, 16));
//        rs->insert(new RegisterDefn("ss", RegisterDefn::REGCAT_SEGMENT, rs->new_storage(), 0, 16));
//        rs->insert(new RegisterDefn("es", RegisterDefn::REGCAT_SEGMENT, rs->new_storage(), 0, 16));
//        
//        /* Pointer registers */
//        rs->insert(new RegisterDefn("sp", RegisterDefn::REGCAT_POINTER, rs->new_storage(), 0, 16)); /* stack pointer */
//        rs->insert(new RegisterDefn("bp", RegisterDefn::REGCAT_POINTER, rs->new_storage(), 0, 16)); /* base pointer */
//        rs->insert(new RegisterDefn("ip", RegisterDefn::REGCAT_POINTER, rs->new_storage(), 0, 16)); /* instruction pointer */
//        
//        /* Array indexing registers */
//        rs->insert(new RegisterDefn("si", RegisterDefn::REGCAT_INDEX,   rs->new_storage(), 0, 16));
//        rs->insert(new RegisterDefn("di", RegisterDefn::REGCAT_INDEX,   rs->new_storage(), 0, 16));
//
//        /* Flags. Reserved flags have no names but can be accessed by reading the entire "flags" register. */
//        sd = rs->new_storage();
//        rs->insert(new RegisterDefn("flags", RegisterDefn::REGCAT_FLAG, sd,   0, 16));     /* all flags */
//        rs->insert(new RegisterDefn("cf", RegisterDefn::REGCAT_FLAG,    sd,   0,  1));     /* carry status flag */
//        rs->insert(new RegisterDefn("pf", RegisterDefn::REGCAT_FLAG,    sd,   2,  1));     /* parity status flag */
//        rs->insert(new RegisterDefn("af", RegisterDefn::REGCAT_FLAG,    sd,   4,  1));     /* adjust status flag */
//        rs->insert(new RegisterDefn("zf", RegisterDefn::REGCAT_FLAG,    sd,   6,  1));     /* zero status flag */
//        rs->insert(new RegisterDefn("sf", RegisterDefn::REGCAT_FLAG,    sd,   7,  1));     /* sign status flag */
//        rs->insert(new RegisterDefn("tf", RegisterDefn::REGCAT_FLAG,    sd,   8,  1));     /* trap system flag */
//        rs->insert(new RegisterDefn("if", RegisterDefn::REGCAT_FLAG,    sd,   9,  1));     /* interrupt enable system flag */
//        rs->insert(new RegisterDefn("df", RegisterDefn::REGCAT_FLAG,    sd,  10,  1));     /* direction control flag */
//        rs->insert(new RegisterDefn("of", RegisterDefn::REGCAT_FLAG,    sd,  11,  1));     /* overflow status flag */
//        rs->insert(new RegisterDefn("nt", RegisterDefn::REGCAT_FLAG,    sd,  14,  1));     /* nested task system flag */
//
//    }
//    return rs;
//}
//
///** Intel 8088 registers. Intel 8088 has the same set of registers as Intel 8086. */
//const RegisterSet* RegisterSet::i8088()
//{
//    static RegisterSet *rs = NULL;
//    if (!rs) {
//        rs = new RegisterSet("Intel 8086");
//        rs->insert(i8086());
//    }
//    return rs;
//}
//
///** Intel 80286 registers. The 80286 has the same registers as the 8086 but adds two new flags to the "flags" register. */
//const RegisterSet* RegisterSet::i80286()
//{
//    static RegisterSet *rs = NULL;
//    if (!rs) {
//        rs = new RegisterSet("Intel 80286");
//        rs->insert(i8086());
//        RegisterDefn *flags = rs->find("flags");
//        rs->insert(new RegisterDefn("iopl", RegisterDefn::REGCAT_FLAG, flags->get_storage(), 12, 2)); /* I/O privilege level flg */
//        rs->insert(new RegisterDefn("nt",   RegisterDefn::REGCAT_FLAG, flags->get_storage(), 14, 1)); /* nested task system flag */
//    }
//    return rs;
//}
//
///** Intel 80386 registers. The 80386 has the same registers as the 8086 but extends the general-purpose registers, base
// *  registers, index registers, instruction pointer, and flags register to 32 bits.  Register names from the 8086 refer to the
// *  same offsets and sizes while the full 32 bits are accessed by names prefixed with "e" as in "eax" (the "e" means
// *  "extended"). Two new segment registers (FS and GS) were added and all segment registers remain 16 bits. */
//const RegisterSet* RegisterSet::i80386()
//{
//    static RegisterSet *rs = NULL;
//    if (!rs) {
//        rs = new RegisterSet("Intel 80386");
//        rs->insert(i8086());
//
//        /* Add 32-bit versions of most 16-bit registers */
//        for (size_t i=0; i<i8086()->size(); i++) {
//            RegisterDefn *r = i8086()->at(i);
//            if (r->get_category()!=RegisterDefn::REGCAT_SEGMENT && r->get_nbits()==16) {
//                std::string name = "e" + r->get_name();
//                rs->insert(new RegisterDefn(name, r->get_category(), r->get_storage(), 0, 32));
//            }
//        }
//        
//        /* Add new segment registers FS and GS */
//        rs->insert(new RegisterDefn("fs", RegisterDefn::REGCAT_SEGMENT, rs->new_storage(), 0, 16));
//        rs->insert(new RegisterDefn("gs", RegisterDefn::REGCAT_SEGMENT, rs->new_storage(), 0, 16));
//
//        /* Add new flags registers */
//        RegisterDefn *flags = rs->find("flags");
//        rs->insert(new RegisterDefn("eflags", RegisterDefn::REGCAT_FLAG, flags->get_storage(), 0, 32));
//        rs->insert(new RegisterDefn("rf", RegisterDefn::REGCAT_FLAG, flags->get_storage(), 16, 1)); /* resume system flag */
//        rs->insert(new RegisterDefn("vm", RegisterDefn::REGCAT_FLAG, flags->get_storage(), 17, 1)); /* virtual 8086 mode flag */
//    }
//    return rs;
//}
//
///** Intel 80486 registers. The 80486 has the same registers as the 80386 but adds a new flag to the "eflags" register and the
// *  floating-point registers previously in the 8087, 80287, and 80387 math coprocessors. */
//const RegisterSet* RegisterSet::i80486()
//{
//    static RegisterSet *rs = NULL;
//    if (!rs) {
//        rs = new RegisterSet("Intel 80486");
//        rs->insert(i80386());
//        RegisterDefn *flags = rs->find("eflags");
//        rs->insert(new RegisterDefn("ac", RegisterDefn::REGCAT_FLAG, flags->get_storage(), 18, 1)); /* alignment check sys flag */
//
//        /* The floating point registers names are relative to the current top-of-stack that changes dynamically. The
//         * definitions we're creating here are static.  When a floating-point instruction is simulated (e.g., by the
//         * instruction semantics analyses) then the simulation will have to make adjustments to the storage descriptors in
//         * order for the register names to point to their new storage locations. */
//        rs->insert(new RegisterDefn("st(0)", RegisterDefn::REGCAT_FLOAT, rs->new_storage(), 0, 80));
//        rs->insert(new RegisterDefn("st(1)", RegisterDefn::REGCAT_FLOAT, rs->new_storage(), 0, 80));
//        rs->insert(new RegisterDefn("st(2)", RegisterDefn::REGCAT_FLOAT, rs->new_storage(), 0, 80));
//        rs->insert(new RegisterDefn("st(3)", RegisterDefn::REGCAT_FLOAT, rs->new_storage(), 0, 80));
//        rs->insert(new RegisterDefn("st(4)", RegisterDefn::REGCAT_FLOAT, rs->new_storage(), 0, 80));
//        rs->insert(new RegisterDefn("st(5)", RegisterDefn::REGCAT_FLOAT, rs->new_storage(), 0, 80));
//        rs->insert(new RegisterDefn("st(6)", RegisterDefn::REGCAT_FLOAT, rs->new_storage(), 0, 80));
//        rs->insert(new RegisterDefn("st(7)", RegisterDefn::REGCAT_FLOAT, rs->new_storage(), 0, 80));
//    }
//    return rs;
//}
//
///** Intel Pentium registers. The Pentium has the same registers as the 80486 but adds a few flags to the "eflags" register and
// *  MMX registers. */
//const RegisterSet* RegisterSet::pentium()
//{
//    static RegisterSet *rs = NULL;
//    if (!rs) {
//        rs = new RegisterSet("Intel Pentium");
//        rs->insert(i80486());
//        RegisterDefn *flags = rs->find("eflags");
//        rs->insert(new RegisterDefn("vif", RegisterDefn::REGCAT_FLAG, flags->get_storage(), 19, 1)); /* virtual interrupt flag */
//        rs->insert(new RegisterDefn("vip", RegisterDefn::REGCAT_FLAG, flags->get_storage(), 20, 1)); /* virt interrupt pending */
//        rs->insert(new RegisterDefn("id",  RegisterDefn::REGCAT_FLAG, flags->get_storage(), 21, 1)); /* ident system flag */
//
//        /* The MMi registers are aliases for the ST(i) registers but are absolute rather than relative to the top of the
//         * stack. We're creating the static definitions, so MMi will point to the same storage as ST(i) for 0<=i<=7. Note that
//         * a write to one of the 64-bit MMi registers causes the high-order 16 bits of the corresponding ST(j) register to be
//         * set to all ones to indicate a NaN value. */
//        for (size_t i=0; i<8; i++) {
//            char stname[8], mmname[8];
//            sprintf(stname, "st(%zu)", i);
//            sprintf(mmname, "mm%zu", i);
//            RegisterDefn *sti = rs->find(stname);
//            rs->insert(new RegisterDefn(mmname, RegisterDefn::REGCAT_SIMD, sti->get_storage(), 0, 64));
//        }
//    }
//    return rs;
//}
//
///** Intel Pentium 4 registers. The Pentium 4 has the same register set as the Pentium but adds the MMX0 through MMX7 registers
// *  for the SSE instruction set. */
//const RegisterSet* RegisterSet::pentium4()
//{
//    static RegisterSet *rs = NULL;
//    if (!rs) {
//        rs = new RegisterSet("Intel Pentium 4");
//        rs->insert(pentium());
//        for (size_t i=0; i<8; i++) {
//            char name[8];
//            sprintf(name, "mmx%zu", i);
//            rs->insert(new RegisterDefn(name, RegisterDefn::REGCAT_SIMD, rs->new_storage(), 0, 128));
//        }
//    }
//    return rs;
//}
//
//        
///** Amd64 registers. The AMD64 architecture increases the size of the general purpose registers, base registers, index
// *  registers, instruction pointer, and flags register to 64-bits.  Register names from the Pentium architecture still exist
// *  and refer to 32-bit quantities, while the AMD64 adds new names that start with "r" rather than "e" (such as "rax" for the
// *  64-bit register and "eax" for the 32 low-order bits of the same register).  It also adds eight additional 64-bit general
// *  purpose registers named "r8" through "r15". */
//const RegisterSet* RegisterSet::amd64()
//{
//    static RegisterSet *rs = NULL;
//    if (!rs) {
//        rs = new RegisterSet("AMD64");
//        rs->insert(pentium());
//        
//        /* Add 64-bit versions of most 32-bit registers */
//        for (size_t i=0; i<pentium4()->size(); i++) {
//            RegisterDefn *r = pentium4()->at(i);
//            if (r->get_category()!=RegisterDefn::REGCAT_SEGMENT && r->get_nbits()==32) {
//                std::string name = r->get_name();
//                ROSE_ASSERT(name[0]=='e');
//                name[0] = 'r';
//                rs->insert(new RegisterDefn(name, r->get_category(), r->get_storage(), 0, 64));
//            }
//        }
//        
//        /* New 64- and 32-bit general purpose registers */
//        for (size_t i=8; i<16; i++) {
//            int sd = rs->new_storage();
//            char name[8];
//            sprintf(name, "r%zu", i);
//            rs->insert(new RegisterDefn(name, RegisterDefn::REGCAT_GENERAL, sd, 0, 64));
//            strcat(name, "d");
//            rs->insert(new RegisterDefn(name, RegisterDefn::REGCAT_GENERAL, sd, 0, 32));
//        }
//
//        /* Add registers MMX8 through MMX15 */
//        for (size_t i=8; i<16; i++) {
//            char name[8];
//            sprintf(name, "mmx%zu", i);
//            rs->insert(new RegisterDefn(name, RegisterDefn::REGCAT_SIMD, rs->new_storage(), 0, 128));
//        }
//    }
//    return rs;
//}
