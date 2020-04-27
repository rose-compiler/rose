#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"
#include "Registers.h"

// These are here temporarily until the classes in this file can be moved into Rose::BinaryAnalysis
using namespace Rose;
using namespace Rose::BinaryAnalysis;

std::ostream&
operator<<(std::ostream &o, const RegisterDictionary &dict) {
    dict.print(o);
    return o;
}


/*******************************************************************************************************************************
 *                                      RegisterNames
 *******************************************************************************************************************************/

std::string
RegisterNames::operator()(RegisterDescriptor rdesc, const RegisterDictionary *dict_/*=NULL*/) const {
    if (rdesc.isEmpty())
        return prefix + (prefix==""?"":"_") + "NONE";

    const RegisterDictionary *dict = dict_ ? dict_ : dflt_dict;
    if (dict) {
        std::string name = dict->lookup(rdesc);
        if (!name.empty())
            return name;
    }

    std::ostringstream ss;
    ss <<prefix <<rdesc.majorNumber() <<"." <<rdesc.minorNumber();
    if (show_offset>0 || (show_offset<0 && rdesc.offset()!=0))
        ss <<offset_prefix <<rdesc.offset() <<offset_suffix;
    if (show_size>0 || (show_size<0 && rdesc.offset()!=0))
        ss <<size_prefix <<rdesc.nBits() <<size_suffix;
    ss <<suffix;
    return ss.str();
}


/*******************************************************************************************************************************
 *                                      RegisterDictionary
 *******************************************************************************************************************************/

void
RegisterDictionary::insert(const std::string &name, RegisterDescriptor rdesc) {
    /* Erase the name from the reverse lookup map, indexed by the old descriptor. */
    Entries::iterator fi = forward.find(name);
    if (fi!=forward.end()) {
        Reverse::iterator ri = reverse.find(fi->second);
        ROSE_ASSERT(ri!=reverse.end());
        std::vector<std::string>::iterator vi=std::find(ri->second.begin(), ri->second.end(), name);
        ROSE_ASSERT(vi!=ri->second.end());
        ri->second.erase(vi);
    }

    /* Insert or replace old descriptor with a new one and insert reverse lookup info. */
    forward[name] = rdesc;
    reverse[rdesc].push_back(name);
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

RegisterDescriptor
RegisterDictionary::find(const std::string &name) const {
    Entries::const_iterator fi = forward.find(name);
    return forward.end() == fi ? RegisterDescriptor() : fi->second;
}

RegisterDescriptor
RegisterDictionary::findOrThrow(const std::string &name) const {
    Entries::const_iterator fi = forward.find(name);
    if (forward.end() == fi)
        throw std::domain_error("register " + name + " not found");
    return fi->second;
}

// deprecated 2020-04-17
const RegisterDescriptor *
RegisterDictionary::lookup(const std::string &name) const {
    Entries::const_iterator fi = forward.find(name);
    if (fi==forward.end())
        return NULL;
    return &(fi->second);
}

const std::string &
RegisterDictionary::lookup(RegisterDescriptor rdesc) const {
    Reverse::const_iterator ri = reverse.find(rdesc);
    if (ri!=reverse.end()) {
        for (size_t i=ri->second.size(); i>0; --i) {
            const std::string &name = ri->second[i-1];
            Entries::const_iterator fi = forward.find(name);
            ROSE_ASSERT(fi!=forward.end());
            if (fi->second==rdesc)
                return name;
        }
    }

    static const std::string empty;
    return empty;
}

const RegisterDescriptor*
RegisterDictionary::exists(RegisterDescriptor rdesc) const {
    Reverse::const_iterator found = reverse.find(rdesc);
    if (found == reverse.end())
        return NULL;
    return &found->first;
}

RegisterDescriptor
RegisterDictionary::findLargestRegister(unsigned major, unsigned minor, size_t maxWidth) const {
    RegisterDescriptor retval;
    for (Entries::const_iterator iter=forward.begin(); iter!=forward.end(); ++iter) {
        RegisterDescriptor reg = iter->second;
        if (major == reg.majorNumber() && minor == reg.minorNumber()) {
            if (maxWidth > 0 && reg.nBits() > maxWidth) {
                // ignore
            } else if (retval.isEmpty()) {
                retval = reg;
            } else if (retval.nBits() < reg.nBits()) {
                retval = reg;
            }
        }
    }
    return retval;
}

void
RegisterDictionary::resize(const std::string &name, unsigned new_nbits) {
    RegisterDescriptor reg = findOrThrow(name);
    reg.nBits(new_nbits);
    insert(name, reg);
}

RegisterParts
RegisterDictionary::getAllParts() const {
    RegisterParts retval;
    BOOST_FOREACH (const Entries::value_type &node, forward)
        retval.insert(node.second);
    return retval;
}

const RegisterDictionary::Entries &
RegisterDictionary::get_registers() const {
    return forward;
}

RegisterDictionary::Entries &
RegisterDictionary::get_registers() {
    return forward;
}

RegisterDictionary::RegisterDescriptors
RegisterDictionary::get_descriptors() const
{
    const Entries &entries = get_registers();
    RegisterDescriptors retval;
    retval.reserve(entries.size());
    for (Entries::const_iterator ei=entries.begin(); ei!=entries.end(); ++ei)
        retval.push_back(ei->second);
    return retval;
}

// Returns lowest possible non-negative value not present in v
static unsigned
firstUnused(std::vector<unsigned> &v /*in,out*/) {
    if (v.empty())
        return 0;
    std::sort(v.begin(), v.end());
    v.erase(std::unique(v.begin(), v.end()), v.end());
    if (v.back() + 1 == v.size())
        return v.size();
    for (size_t i=0; i<v.size(); ++i) {
        if (v[i] != i)
            return i;
    }
    ASSERT_not_reachable("should have returned by now");
}

unsigned
RegisterDictionary::firstUnusedMajor() const {
    std::vector<unsigned> used;
    BOOST_FOREACH (const Entries::value_type &entry, forward) {
        if (used.empty() || used.back()!=entry.second.majorNumber())
            used.push_back(entry.second.majorNumber());
    }
    return firstUnused(used);
}

unsigned
RegisterDictionary::firstUnusedMinor(unsigned majr) const {
    std::vector<unsigned> used;
    BOOST_FOREACH (const Entries::value_type &entry, forward) {
        if (entry.second.majorNumber() == majr)
            used.push_back(entry.second.minorNumber());
    }
    return firstUnused(used);
}

RegisterDictionary::RegisterDescriptors
RegisterDictionary::get_largest_registers() const {
    SortBySize order(SortBySize::ASCENDING);
    return filter_nonoverlapping(get_descriptors(), order, true);
}

RegisterDictionary::RegisterDescriptors
RegisterDictionary::get_smallest_registers() const {
    SortBySize order(SortBySize::DESCENDING);
    return filter_nonoverlapping(get_descriptors(), order, true);
}

void
RegisterDictionary::print(std::ostream &o) const {
    o <<"RegisterDictionary \"" <<name <<"\" contains " <<forward.size() <<" " <<(1==forward.size()?"entry":"entries") <<"\n";
    for (Entries::const_iterator ri=forward.begin(); ri!=forward.end(); ++ri)
        o <<"  \"" <<ri->first <<"\" " <<ri->second <<"\n";

    for (Reverse::const_iterator ri=reverse.begin(); ri!=reverse.end(); ++ri) {
        o <<"  " <<ri->first;
        for (std::vector<std::string>::const_iterator vi=ri->second.begin(); vi!=ri->second.end(); ++vi) {
            o <<" " <<*vi;
        }
        o <<"\n";
    }
}

// class method
const RegisterDictionary *
RegisterDictionary::dictionary_for_isa(SgAsmExecutableFileFormat::InsSetArchitecture isa) {
    typedef SgAsmExecutableFileFormat EFF;
    switch (isa & EFF::ISA_FAMILY_MASK) {
        case EFF::ISA_IA32_Family:
            switch (isa) {
                case EFF::ISA_IA32_286:         return dictionary_i286();
                case EFF::ISA_IA32_386:         return dictionary_i386_387(); // assume '387 coprocessor is present
                case EFF::ISA_IA32_486:         return dictionary_i486();
                case EFF::ISA_IA32_Pentium:     return dictionary_pentium();
                case EFF::ISA_IA32_Pentium4:    return dictionary_pentium4();
                default:                        return dictionary_pentium4();
            }
            break;

        case EFF::ISA_X8664_Family:
            return dictionary_amd64();

        case EFF::ISA_MIPS_Family:
            return dictionary_mips32_altnames(); // disassembler assumes only dictionary_mips32()

        case EFF::ISA_ARM_Family:
            return dictionary_arm7();

        case EFF::ISA_PowerPC:
            return dictionary_powerpc32();

        case EFF::ISA_PowerPC_64bit:
            return dictionary_powerpc64();

        case EFF::ISA_M68K_Family:
            return dictionary_coldfire();

        default:
            return NULL;
    }
}

// class method
const RegisterDictionary *
RegisterDictionary::dictionary_for_isa(SgAsmInterpretation *interp) {
    const SgAsmGenericHeaderPtrList &hdrs = interp->get_headers()->get_headers();
    return hdrs.empty() ? NULL : dictionary_for_isa(hdrs.front()->get_isa());
}

const RegisterDictionary *
RegisterDictionary::dictionary_i8086() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

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

        /* Flags with official names. */
        regs->insert("flags", x86_regclass_flags, x86_flags_status,  0, 16); /* all flags */
        regs->insert("cf",    x86_regclass_flags, x86_flags_status,  0,  1); /* carry status flag */
        regs->insert("pf",    x86_regclass_flags, x86_flags_status,  2,  1); /* parity status flag */
        regs->insert("af",    x86_regclass_flags, x86_flags_status,  4,  1); /* adjust status flag */
        regs->insert("zf",    x86_regclass_flags, x86_flags_status,  6,  1); /* zero status flag */
        regs->insert("sf",    x86_regclass_flags, x86_flags_status,  7,  1); /* sign status flag */
        regs->insert("tf",    x86_regclass_flags, x86_flags_status,  8,  1); /* trap system flag */
        regs->insert("if",    x86_regclass_flags, x86_flags_status,  9,  1); /* interrupt enable system flag */
        regs->insert("df",    x86_regclass_flags, x86_flags_status, 10,  1); /* direction control flag */
        regs->insert("of",    x86_regclass_flags, x86_flags_status, 11,  1); /* overflow status flag */
        regs->insert("nt",    x86_regclass_flags, x86_flags_status, 14,  1); /* nested task system flag */

        /* Flags without names */
        regs->insert("f1",    x86_regclass_flags, x86_flags_status,  1,  1);
        regs->insert("f3",    x86_regclass_flags, x86_flags_status,  3,  1);
        regs->insert("f5",    x86_regclass_flags, x86_flags_status,  5,  1);
        regs->insert("f12",   x86_regclass_flags, x86_flags_status, 12,  1);
        regs->insert("f13",   x86_regclass_flags, x86_flags_status, 13,  1);
        regs->insert("f15",   x86_regclass_flags, x86_flags_status, 15,  1);
    }
    return regs;
}

const RegisterDictionary *
RegisterDictionary::dictionary_i8088() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary *regs = NULL;
    if (!regs) {
        regs = new RegisterDictionary("i8088");
        regs->insert(dictionary_i8086());
    }
    return regs;
}

const RegisterDictionary *
RegisterDictionary::dictionary_i286() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary *regs = NULL;
    if (!regs) {
        regs = new RegisterDictionary("i286");
        regs->insert(dictionary_i8086());
        regs->insert("iopl", x86_regclass_flags, x86_flags_status, 12, 2); /*  I/O privilege level flag */
        regs->insert("nt",   x86_regclass_flags, x86_flags_status, 14, 1); /*  nested task system flag */
    }
    return regs;
}

const RegisterDictionary *
RegisterDictionary::dictionary_i386() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary *regs = NULL;
    if (!regs) {
        regs = new RegisterDictionary("i386");
        regs->insert(dictionary_i286());

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
        regs->insert("eflags", x86_regclass_flags, x86_flags_status, 0, 32);

        /* Additional 16-bit segment registers */
        regs->insert("fs", x86_regclass_segment, x86_segreg_fs, 0, 16);
        regs->insert("gs", x86_regclass_segment, x86_segreg_gs, 0, 16);

        /* Additional flags */
        regs->insert("rf", x86_regclass_flags, x86_flags_status, 16, 1); /* resume system flag */
        regs->insert("vm", x86_regclass_flags, x86_flags_status, 17, 1); /* virtual 8086 mode flag */

        /* Additional flag bits that have no official names */
        for (unsigned i=18; i<32; ++i)
            regs->insert("f"+StringUtility::numberToString(i), x86_regclass_flags, x86_flags_status, i, 1);
        
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

const RegisterDictionary *
RegisterDictionary::dictionary_i386_387() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary *regs = NULL;
    if (!regs) {
        regs = new RegisterDictionary("i386 w/387");
        regs->insert(dictionary_i386());

        // The 387 contains eight floating-point registers that have no names (we call them "st0" through "st7"), and defines
        // expressions of the form "st(n)" to refer to the current nth register from the top of a circular stack.  These
        // expressions are implemented usng SgAsmIndexedRegisterExpression IR nodes, which have a base register which is
        // "st0", a stride which increments the minor number, an offset which is the current top-of-stack value, an index
        // which is the value "n" in the expression "st(n)", and a modulus of eight.  The current top-of-stack value is held in
        // the three-bit register "fpstatus_top", which normally has a concrete value.
        regs->insert("st0",     x86_regclass_st, x86_st_0,   0, 80);
        regs->insert("st1",     x86_regclass_st, x86_st_1,   0, 80);
        regs->insert("st2",     x86_regclass_st, x86_st_2,   0, 80);
        regs->insert("st3",     x86_regclass_st, x86_st_3,   0, 80);
        regs->insert("st4",     x86_regclass_st, x86_st_4,   0, 80);
        regs->insert("st5",     x86_regclass_st, x86_st_5,   0, 80);
        regs->insert("st6",     x86_regclass_st, x86_st_6,   0, 80);
        regs->insert("st7",     x86_regclass_st, x86_st_7,   0, 80);

        // Floating-point tag registers, two bits per ST register.
        regs->insert("fptag",     x86_regclass_flags, x86_flags_fptag,  0, 16); // all tags
        regs->insert("fptag_st0", x86_regclass_flags, x86_flags_fptag,  0,  2); // tag for st0
        regs->insert("fptag_st1", x86_regclass_flags, x86_flags_fptag,  2,  2); // tag for st1
        regs->insert("fptag_st2", x86_regclass_flags, x86_flags_fptag,  4,  2); // tag for st2
        regs->insert("fptag_st3", x86_regclass_flags, x86_flags_fptag,  6,  2); // tag for st3
        regs->insert("fptag_st4", x86_regclass_flags, x86_flags_fptag,  8,  2); // tag for st4
        regs->insert("fptag_st5", x86_regclass_flags, x86_flags_fptag, 10,  2); // tag for st5
        regs->insert("fptag_st6", x86_regclass_flags, x86_flags_fptag, 12,  2); // tag for st6
        regs->insert("fptag_st7", x86_regclass_flags, x86_flags_fptag, 14,  2); // tag for st7

        // Floating-point status register
        regs->insert("fpstatus",     x86_regclass_flags, x86_flags_fpstatus,  0, 16);
        regs->insert("fpstatus_ie",  x86_regclass_flags, x86_flags_fpstatus,  0,  1); // invalid operation
        regs->insert("fpstatus_de",  x86_regclass_flags, x86_flags_fpstatus,  1,  1); // denormalized operand
        regs->insert("fpstatus_ze",  x86_regclass_flags, x86_flags_fpstatus,  2,  1); // zero divide
        regs->insert("fpstatus_oe",  x86_regclass_flags, x86_flags_fpstatus,  3,  1); // overflow
        regs->insert("fpstatus_ue",  x86_regclass_flags, x86_flags_fpstatus,  4,  1); // underflow
        regs->insert("fpstatus_pe",  x86_regclass_flags, x86_flags_fpstatus,  5,  1); // precision
        regs->insert("fpstatus_ir",  x86_regclass_flags, x86_flags_fpstatus,  7,  1); // interrupt request
        regs->insert("fpstatus_c4",  x86_regclass_flags, x86_flags_fpstatus,  8,  1); // condition code
        regs->insert("fpstatus_c1",  x86_regclass_flags, x86_flags_fpstatus,  9,  1); // condition code
        regs->insert("fpstatus_c2",  x86_regclass_flags, x86_flags_fpstatus, 10,  1); // condition code
        regs->insert("fpstatus_top", x86_regclass_flags, x86_flags_fpstatus, 11,  3); // top of stack
        regs->insert("fpstatus_c3",  x86_regclass_flags, x86_flags_fpstatus, 14,  1); // condition code
        regs->insert("fpstatus_b",   x86_regclass_flags, x86_flags_fpstatus, 15,  1); // busy

        // Floating-point control register
        regs->insert("fpctl",    x86_regclass_flags, x86_flags_fpctl,  0, 16);
        regs->insert("fpctl_im", x86_regclass_flags, x86_flags_fpctl,  0,  1); // invalid operation
        regs->insert("fpctl_dm", x86_regclass_flags, x86_flags_fpctl,  1,  1); // denormalized operand
        regs->insert("fpctl_zm", x86_regclass_flags, x86_flags_fpctl,  2,  1); // zero divide
        regs->insert("fpctl_om", x86_regclass_flags, x86_flags_fpctl,  3,  1); // overflow
        regs->insert("fpctl_um", x86_regclass_flags, x86_flags_fpctl,  4,  1); // underflow
        regs->insert("fpctl_pm", x86_regclass_flags, x86_flags_fpctl,  5,  1); // precision
        regs->insert("fpctl_m",  x86_regclass_flags, x86_flags_fpctl,  7,  1); // interrupt mask
        regs->insert("fpctl_pc", x86_regclass_flags, x86_flags_fpctl,  8,  2); // precision control
        regs->insert("fpctl_rc", x86_regclass_flags, x86_flags_fpctl, 10,  2); // rounding control
        regs->insert("fpctl_ic", x86_regclass_flags, x86_flags_fpctl, 12,  1); // infinity control
    }
    return regs;
}
        
const RegisterDictionary *
RegisterDictionary::dictionary_i486() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary *regs = NULL;
    if (!regs) {
        regs = new RegisterDictionary("i486");
        regs->insert(dictionary_i386_387());
        regs->insert("ac", x86_regclass_flags, x86_flags_status, 18, 1); /* alignment check system flag */
    }
    return regs;
}

const RegisterDictionary *
RegisterDictionary::dictionary_pentium() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary *regs = NULL;
    if (!regs) {
        regs = new RegisterDictionary("pentium");
        regs->insert(dictionary_i486());

        /* Additional flags */
        regs->insert("vif", x86_regclass_flags, x86_flags_status, 19, 1); /* virtual interrupt flag */
        regs->insert("vip", x86_regclass_flags, x86_flags_status, 20, 1); /* virt interrupt pending */
        regs->insert("id",  x86_regclass_flags, x86_flags_status, 21, 1); /* ident system flag */

        /* The MMi registers are aliases for the ST(i) registers but are absolute rather than relative to the top of the
         * stack. We're creating the static definitions, so MMi will point to the same storage as ST(i) for 0<=i<=7. Note that
         * a write to one of the 64-bit MMi registers causes the high-order 16 bits of the corresponding ST(j) register to be
         * set to all ones to indicate a NaN value. */
        regs->insert("mm0", x86_regclass_st, x86_st_0, 0, 64);
        regs->insert("mm1", x86_regclass_st, x86_st_1, 0, 64);
        regs->insert("mm2", x86_regclass_st, x86_st_2, 0, 64);
        regs->insert("mm3", x86_regclass_st, x86_st_3, 0, 64);
        regs->insert("mm4", x86_regclass_st, x86_st_4, 0, 64);
        regs->insert("mm5", x86_regclass_st, x86_st_5, 0, 64);
        regs->insert("mm6", x86_regclass_st, x86_st_6, 0, 64);
        regs->insert("mm7", x86_regclass_st, x86_st_7, 0, 64);
    }
    return regs;
}

const RegisterDictionary *
RegisterDictionary::dictionary_pentiumiii() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary *regs = NULL;
    if (!regs) {
        regs = new RegisterDictionary("pentiumiii");
        regs->insert(dictionary_pentium());
        regs->insert("xmm0", x86_regclass_xmm, 0, 0, 128);
        regs->insert("xmm1", x86_regclass_xmm, 1, 0, 128);
        regs->insert("xmm2", x86_regclass_xmm, 2, 0, 128);
        regs->insert("xmm3", x86_regclass_xmm, 3, 0, 128);
        regs->insert("xmm4", x86_regclass_xmm, 4, 0, 128);
        regs->insert("xmm5", x86_regclass_xmm, 5, 0, 128);
        regs->insert("xmm6", x86_regclass_xmm, 6, 0, 128);
        regs->insert("xmm7", x86_regclass_xmm, 7, 0, 128);

        /** SSE status and control register. */
        regs->insert("mxcsr",     x86_regclass_flags, x86_flags_mxcsr,  0, 32);
        regs->insert("mxcsr_ie",  x86_regclass_flags, x86_flags_mxcsr,  0,  1); // invalid operation flag
        regs->insert("mxcsr_de",  x86_regclass_flags, x86_flags_mxcsr,  1,  1); // denormal flag
        regs->insert("mxcsr_ze",  x86_regclass_flags, x86_flags_mxcsr,  2,  1); // divide by zero flag
        regs->insert("mxcsr_oe",  x86_regclass_flags, x86_flags_mxcsr,  3,  1); // overflow flag
        regs->insert("mxcsr_ue",  x86_regclass_flags, x86_flags_mxcsr,  4,  1); // underflow flag
        regs->insert("mxcsr_pe",  x86_regclass_flags, x86_flags_mxcsr,  5,  1); // precision flag
        regs->insert("mxcsr_daz", x86_regclass_flags, x86_flags_mxcsr,  6,  1); // denormals are zero
        regs->insert("mxcsr_im",  x86_regclass_flags, x86_flags_mxcsr,  7,  1); // invalid operation mask
        regs->insert("mxcsr_dm",  x86_regclass_flags, x86_flags_mxcsr,  8,  1); // denormal mask
        regs->insert("mxcsr_zm",  x86_regclass_flags, x86_flags_mxcsr,  9,  1); // divide by zero mask
        regs->insert("mxcsr_om",  x86_regclass_flags, x86_flags_mxcsr, 10,  1); // overflow mask
        regs->insert("mxcsr_um",  x86_regclass_flags, x86_flags_mxcsr, 11,  1); // underflow mask
        regs->insert("mxcsr_pm",  x86_regclass_flags, x86_flags_mxcsr, 12,  1); // precision mask
        regs->insert("mxcsr_r",   x86_regclass_flags, x86_flags_mxcsr, 13,  2); // rounding mode
        regs->insert("mxcsr_fz",  x86_regclass_flags, x86_flags_mxcsr, 15,  1); // flush to zero
    }
    return regs;
}

const RegisterDictionary *
RegisterDictionary::dictionary_pentium4() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary *regs = NULL;
    if (!regs) {
        regs = new RegisterDictionary("pentium4");
        regs->insert(dictionary_pentiumiii());
    }
    return regs;
}

const RegisterDictionary *
RegisterDictionary::dictionary_amd64() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary *regs = NULL;
    if (!regs) {
        regs = new RegisterDictionary("amd64");
        regs->insert(dictionary_pentium4());

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
        regs->insert("rflags", x86_regclass_flags, x86_flags_status, 0, 64);

        for (unsigned i=8; i<16; i++) {
            /* New general purpose registers in various widths */
            std::string name = "r" + StringUtility::numberToString(i);
            regs->insert(name,     x86_regclass_gpr, i, 0, 64);
            regs->insert(name+"b", x86_regclass_gpr, i, 0,  8);
            regs->insert(name+"w", x86_regclass_gpr, i, 0, 16);
            regs->insert(name+"d", x86_regclass_gpr, i, 0, 32);

            /* New media XMM registers */
            regs->insert(std::string("xmm")+StringUtility::numberToString(i),
                         x86_regclass_xmm, i, 0, 128);
        }

        /* Additional flag bits with no official names */
        for (unsigned i=32; i<64; ++i)
            regs->insert("f"+StringUtility::numberToString(i), x86_regclass_flags, x86_flags_status, i, 1);

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

const RegisterDictionary *
RegisterDictionary::dictionary_arm7() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

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

const RegisterDictionary *
RegisterDictionary::dictionary_powerpc32() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // WARNING: PowerPC documentation numbers register bits in reverse of their power-of-two position. ROSE numbers bits
    //          according to their power of two, so that the bit corresponding to 2^i is said to be at position i in the
    //          reigster.  In PowerPC documentation, the bit for 2^i is at N - (i+1) where N is the total number of bits in the
    //          reigster.  All PowerPC bit position numbers need to be converted to the ROSE numbering when they appear here.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    static RegisterDictionary *regs = NULL;
    if (!regs) {
        regs = new RegisterDictionary("powerpc-32");

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

        // Condition Register. This register is grouped into eight fields, where each field is 4 bits. Many PowerPC
        // instructions define the least significant bit of the instruction encoding as the Rc bit, and some instructions imply
        // an Rc value equal to 1. When Rc is equal to 1 for integer operations, the CR0 field is set to reflect the result of
        // the instruction's operation: Less than zero (LT), greater than zero (GT), equal to zero (EQ), and summary overflow
        // (SO). When Rc is equal to 1 for floating-point operations, the CR1 field is set to reflect the state of the
        // exception status bits in the FPSCR: FX, FEX, VX, and OX. Any CR field can be the target of an integer or
        // floating-point comparison instruction. The CR0 field is also set to reflect the result of a conditional store
        // instruction (stwcx or stdcx). There is also a set of instructions that can manipulate a specific CR bit, a specific
        // CR field, or the entire CR, usually to combine several conditions into a single bit for testing.
        regs->insert("cr",  powerpc_regclass_cr, 0,  0, 32);

        regs->insert("cr0", powerpc_regclass_cr, 0, 28,  4);
        regs->insert("cr0.lt", powerpc_regclass_cr, 0, 31, 1);
        regs->insert("cr0.gt", powerpc_regclass_cr, 0, 30, 1);
        regs->insert("cr0.eq", powerpc_regclass_cr, 0, 29, 1);
        regs->insert("cr0.so", powerpc_regclass_cr, 0, 28, 1);

        regs->insert("cr1", powerpc_regclass_cr, 0, 24,  4);
        regs->insert("cr1.lt", powerpc_regclass_cr, 0, 27, 1);
        regs->insert("cr1.gt", powerpc_regclass_cr, 0, 26, 1);
        regs->insert("cr1.eq", powerpc_regclass_cr, 0, 25, 1);
        regs->insert("cr1.so", powerpc_regclass_cr, 0, 24, 1);

        regs->insert("cr2", powerpc_regclass_cr, 0, 20,  4);
        regs->insert("cr2.lt", powerpc_regclass_cr, 0, 23, 1);
        regs->insert("cr2.gt", powerpc_regclass_cr, 0, 22, 1);
        regs->insert("cr2.eq", powerpc_regclass_cr, 0, 21, 1);
        regs->insert("cr2.so", powerpc_regclass_cr, 0, 20, 1);

        regs->insert("cr3", powerpc_regclass_cr, 0, 16,  4);
        regs->insert("cr3.lt", powerpc_regclass_cr, 0, 19, 1);
        regs->insert("cr3.gt", powerpc_regclass_cr, 0, 18, 1);
        regs->insert("cr3.eq", powerpc_regclass_cr, 0, 17, 1);
        regs->insert("cr3.so", powerpc_regclass_cr, 0, 16, 1);

        regs->insert("cr4", powerpc_regclass_cr, 0, 12,  4);
        regs->insert("cr4.lt", powerpc_regclass_cr, 0, 15, 1);
        regs->insert("cr4.gt", powerpc_regclass_cr, 0, 14, 1);
        regs->insert("cr4.eq", powerpc_regclass_cr, 0, 13, 1);
        regs->insert("cr4.so", powerpc_regclass_cr, 0, 12, 1);

        regs->insert("cr5", powerpc_regclass_cr, 0,  8,  4);
        regs->insert("cr5.lt", powerpc_regclass_cr, 0, 11, 1);
        regs->insert("cr5.gt", powerpc_regclass_cr, 0, 10, 1);
        regs->insert("cr5.eq", powerpc_regclass_cr, 0,  9, 1);
        regs->insert("cr5.so", powerpc_regclass_cr, 0,  8, 1);

        regs->insert("cr6", powerpc_regclass_cr, 0,  4,  4);
        regs->insert("cr6.lt", powerpc_regclass_cr, 0,  7, 1);
        regs->insert("cr6.gt", powerpc_regclass_cr, 0,  6, 1);
        regs->insert("cr6.eq", powerpc_regclass_cr, 0,  5, 1);
        regs->insert("cr6.so", powerpc_regclass_cr, 0,  4, 1);

        regs->insert("cr7", powerpc_regclass_cr, 0,  0,  4);
        regs->insert("cr7.lt", powerpc_regclass_cr, 0,  3, 1);
        regs->insert("cr7.gt", powerpc_regclass_cr, 0,  2, 1);
        regs->insert("cr7.eq", powerpc_regclass_cr, 0,  1, 1);
        regs->insert("cr7.so", powerpc_regclass_cr, 0,  0, 1);

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
        regs->insert("xer_so", powerpc_regclass_spr, powerpc_spr_xer, 31, 1); // summary overflow
        regs->insert("xer_ov", powerpc_regclass_spr, powerpc_spr_xer, 30, 1); // overflow
        regs->insert("xer_ca", powerpc_regclass_spr, powerpc_spr_xer, 29, 1); // carry

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

const RegisterDictionary *
RegisterDictionary::dictionary_powerpc64() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // WARNING: PowerPC documentation numbers register bits in reverse of their power-of-two position. ROSE numbers bits
    //          according to their power of two, so that the bit corresponding to 2^i is said to be at position i in the
    //          reigster.  In PowerPC documentation, the bit for 2^i is at N - (i+1) where N is the total number of bits in the
    //          reigster.  All PowerPC bit position numbers need to be converted to the ROSE numbering when they appear here.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    static RegisterDictionary *regs = NULL;
    if (!regs) {
        regs = new RegisterDictionary("powerpc-64");

        /**********************************************************************************************************************
         * General purpose and floating point registers
         **********************************************************************************************************************/
        for (unsigned i=0; i<32; i++) {
            regs->insert("r"+StringUtility::numberToString(i), powerpc_regclass_gpr, i, 0, 64);
            regs->insert("f"+StringUtility::numberToString(i), powerpc_regclass_fpr, i, 0, 64);
        }

        /**********************************************************************************************************************
         * State, status, condition, control registers
         **********************************************************************************************************************/

        /* Machine state register */
        regs->insert("msr", powerpc_regclass_msr, 0, 0, 32);

        /* Floating point status and control register */
        regs->insert("fpscr", powerpc_regclass_fpscr, 0, 0, 32);

        // Condition Register. This register is grouped into eight fields, where each field is 4 bits. Many PowerPC
        // instructions define the least significant bit of the instruction encoding as the Rc bit, and some instructions imply
        // an Rc value equal to 1. When Rc is equal to 1 for integer operations, the CR0 field is set to reflect the result of
        // the instruction's operation: Less than zero (LT), greater than zero (GT), equal to zero (EQ), and summary overflow
        // (SO). When Rc is equal to 1 for floating-point operations, the CR1 field is set to reflect the state of the
        // exception status bits in the FPSCR: FX, FEX, VX, and OX. Any CR field can be the target of an integer or
        // floating-point comparison instruction. The CR0 field is also set to reflect the result of a conditional store
        // instruction (stwcx or stdcx). There is also a set of instructions that can manipulate a specific CR bit, a specific
        // CR field, or the entire CR, usually to combine several conditions into a single bit for testing.
        regs->insert("cr",  powerpc_regclass_cr, 0,  0, 32);

        regs->insert("cr0", powerpc_regclass_cr, 0, 28,  4);
        regs->insert("cr0.lt", powerpc_regclass_cr, 0, 31, 1);
        regs->insert("cr0.gt", powerpc_regclass_cr, 0, 30, 1);
        regs->insert("cr0.eq", powerpc_regclass_cr, 0, 29, 1);
        regs->insert("cr0.so", powerpc_regclass_cr, 0, 28, 1);

        regs->insert("cr1", powerpc_regclass_cr, 0, 24,  4);
        regs->insert("cr1.lt", powerpc_regclass_cr, 0, 27, 1);
        regs->insert("cr1.gt", powerpc_regclass_cr, 0, 26, 1);
        regs->insert("cr1.eq", powerpc_regclass_cr, 0, 25, 1);
        regs->insert("cr1.so", powerpc_regclass_cr, 0, 24, 1);

        regs->insert("cr2", powerpc_regclass_cr, 0, 20,  4);
        regs->insert("cr2.lt", powerpc_regclass_cr, 0, 23, 1);
        regs->insert("cr2.gt", powerpc_regclass_cr, 0, 22, 1);
        regs->insert("cr2.eq", powerpc_regclass_cr, 0, 21, 1);
        regs->insert("cr2.so", powerpc_regclass_cr, 0, 20, 1);

        regs->insert("cr3", powerpc_regclass_cr, 0, 16,  4);
        regs->insert("cr3.lt", powerpc_regclass_cr, 0, 19, 1);
        regs->insert("cr3.gt", powerpc_regclass_cr, 0, 18, 1);
        regs->insert("cr3.eq", powerpc_regclass_cr, 0, 17, 1);
        regs->insert("cr3.so", powerpc_regclass_cr, 0, 16, 1);

        regs->insert("cr4", powerpc_regclass_cr, 0, 12,  4);
        regs->insert("cr4.lt", powerpc_regclass_cr, 0, 15, 1);
        regs->insert("cr4.gt", powerpc_regclass_cr, 0, 14, 1);
        regs->insert("cr4.eq", powerpc_regclass_cr, 0, 13, 1);
        regs->insert("cr4.so", powerpc_regclass_cr, 0, 12, 1);

        regs->insert("cr5", powerpc_regclass_cr, 0,  8,  4);
        regs->insert("cr5.lt", powerpc_regclass_cr, 0, 11, 1);
        regs->insert("cr5.gt", powerpc_regclass_cr, 0, 10, 1);
        regs->insert("cr5.eq", powerpc_regclass_cr, 0,  9, 1);
        regs->insert("cr5.so", powerpc_regclass_cr, 0,  8, 1);

        regs->insert("cr6", powerpc_regclass_cr, 0,  4,  4);
        regs->insert("cr6.lt", powerpc_regclass_cr, 0,  7, 1);
        regs->insert("cr6.gt", powerpc_regclass_cr, 0,  6, 1);
        regs->insert("cr6.eq", powerpc_regclass_cr, 0,  5, 1);
        regs->insert("cr6.so", powerpc_regclass_cr, 0,  4, 1);

        regs->insert("cr7", powerpc_regclass_cr, 0,  0,  4);
        regs->insert("cr7.lt", powerpc_regclass_cr, 0,  3, 1);
        regs->insert("cr7.gt", powerpc_regclass_cr, 0,  2, 1);
        regs->insert("cr7.eq", powerpc_regclass_cr, 0,  1, 1);
        regs->insert("cr7.so", powerpc_regclass_cr, 0,  0, 1);

        /* The processor version register is a 32-bit read-only register that identifies the version and revision level of the
         * processor. Processor versions are assigned by the PowerPC architecture process. Revision levels are implementation
         * defined. Access to the register is privileged, so that an application program can determine the processor version
         * only with the help of an operating system function. */
        regs->insert("pvr", powerpc_regclass_pvr, 0, 0, 32);

        /**********************************************************************************************************************
         * The instruction address register is a pseudo register. It is not directly available to the user other than through a
         * "branch and link" instruction. It is primarily used by debuggers to show the next instruction to be executed.
         **********************************************************************************************************************/
        regs->insert("iar", powerpc_regclass_iar, 0, 0, 64);

        /**********************************************************************************************************************
         * Special purpose registers. There are 1024 of these, some of which have special names.  We name all 1024 consistently
         * and create aliases for the special ones. This allows the disassembler to look them up generically.  Because the
         * special names appear after the generic names, a reverse lookup will return the special name.
         **********************************************************************************************************************/
        /* Generic names for them all */
        for (unsigned i=0; i<1024; i++)
            regs->insert("spr"+StringUtility::numberToString(i), powerpc_regclass_spr, i, 0, 64);

        /* The link register contains the address to return to at the end of a function call.  Each branch instruction encoding
         * has an LK bit. If the LK bit is 1, the branch instruction moves the program counter to the link register. Also, the
         * conditional branch instruction BCLR branches to the value in the link register. */
        regs->insert("lr", powerpc_regclass_spr, powerpc_spr_lr, 0, 64);

        /* The fixed-point exception register contains carry and overflow information from integer arithmetic operations. It
         * also contains carry input to certain integer arithmetic operations and the number of bytes to transfer during load
         * and store string instructions, lswx and stswx. */
        regs->insert("xer", powerpc_regclass_spr, powerpc_spr_xer, 0, 64);
        regs->insert("xer_so", powerpc_regclass_spr, powerpc_spr_xer, 31, 1); // summary overflow
        regs->insert("xer_ov", powerpc_regclass_spr, powerpc_spr_xer, 30, 1); // overflow
        regs->insert("xer_ca", powerpc_regclass_spr, powerpc_spr_xer, 29, 1); // carry

        /* The count register contains a loop counter that is decremented on certain branch operations. Also, the conditional
         * branch instruction bcctr branches to the value in the CTR. */
        regs->insert("ctr", powerpc_regclass_spr, powerpc_spr_ctr, 0, 64);

        /* Other special purpose registers. */
        regs->insert("dsisr", powerpc_regclass_spr, powerpc_spr_dsisr, 0, 64);
        regs->insert("dar", powerpc_regclass_spr, powerpc_spr_dar, 0, 64);
        regs->insert("dec", powerpc_regclass_spr, powerpc_spr_dec, 0, 64);

        /**********************************************************************************************************************
         * Time base registers. There are 1024 of these, some of which have special names. We name all 1024 consistently and
         * create aliases for the special ones. This allows the disassembler to look them up generically.  Because the special
         * names appear after the generic names, a reverse lookup will return the special name.
         **********************************************************************************************************************/
        for (unsigned i=0; i<1024; i++)
            regs->insert("tbr"+StringUtility::numberToString(i), powerpc_regclass_tbr, i, 0, 64);

        regs->insert("tbl", powerpc_regclass_tbr, powerpc_tbr_tbl, 0, 64);      /* time base lower */
        regs->insert("tbu", powerpc_regclass_tbr, powerpc_tbr_tbu, 0, 64);      /* time base upper */
    }
    return regs;
}

const RegisterDictionary *
RegisterDictionary::dictionary_mips32() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary *regs = NULL;
    if (!regs) {
        regs = new RegisterDictionary("mips32");

        // 32 general purpose registers
        for (size_t i=0; i<32; ++i)
            regs->insert("r"+StringUtility::numberToString(i), mips_regclass_gpr, i, 0, 32);

        // Alternate names for the general purpose registers
        regs->insert("zero", mips_regclass_gpr, 0,  0, 32);                     // always equal to zero
        regs->insert("at",   mips_regclass_gpr, 1,  0, 32);                     // assembler temporary
        regs->insert("v0",   mips_regclass_gpr, 2,  0, 32);                     // return value from a function call
        regs->insert("v1",   mips_regclass_gpr, 3,  0, 32);
        regs->insert("a0",   mips_regclass_gpr, 4,  0, 32);                     // first four function arguments
        regs->insert("a1",   mips_regclass_gpr, 5,  0, 32);
        regs->insert("a2",   mips_regclass_gpr, 6,  0, 32);
        regs->insert("a3",   mips_regclass_gpr, 7,  0, 32);
        regs->insert("t0",   mips_regclass_gpr, 8,  0, 32);                     // temporary variables; need not be preserved
        regs->insert("t1",   mips_regclass_gpr, 9,  0, 32);
        regs->insert("t2",   mips_regclass_gpr, 10, 0, 32);
        regs->insert("t3",   mips_regclass_gpr, 11, 0, 32);
        regs->insert("t4",   mips_regclass_gpr, 12, 0, 32);
        regs->insert("t5",   mips_regclass_gpr, 13, 0, 32);
        regs->insert("t6",   mips_regclass_gpr, 14, 0, 32);
        regs->insert("t7",   mips_regclass_gpr, 15, 0, 32);
        regs->insert("s0",   mips_regclass_gpr, 16, 0, 32);                     // temporary variables; must be preserved
        regs->insert("s1",   mips_regclass_gpr, 17, 0, 32);
        regs->insert("s2",   mips_regclass_gpr, 18, 0, 32);
        regs->insert("s3",   mips_regclass_gpr, 19, 0, 32);
        regs->insert("s4",   mips_regclass_gpr, 20, 0, 32);
        regs->insert("s5",   mips_regclass_gpr, 21, 0, 32);
        regs->insert("s6",   mips_regclass_gpr, 22, 0, 32);
        regs->insert("s7",   mips_regclass_gpr, 23, 0, 32);
        regs->insert("t8",   mips_regclass_gpr, 24, 0, 32);                     // two more temporaries; need not be preserved
        regs->insert("t9",   mips_regclass_gpr, 25, 0, 32);
        regs->insert("k0",   mips_regclass_gpr, 26, 0, 32);                     // kernel use; may change unexpectedly
        regs->insert("k1",   mips_regclass_gpr, 27, 0, 32);
        regs->insert("gp",   mips_regclass_gpr, 28, 0, 32);                     // global pointer
        regs->insert("sp",   mips_regclass_gpr, 29, 0, 32);                     // stack pointer
        regs->insert("s8",   mips_regclass_gpr, 30, 0, 32);                     // temp; must be preserved (or "fp")
        regs->insert("fp",   mips_regclass_gpr, 30, 0, 32);                     // stack frame pointer (or "s8")
        regs->insert("ra",   mips_regclass_gpr, 31, 0, 32);                     // return address (link register)

        // Special purpose registers
        regs->insert("hi", mips_regclass_spr, mips_spr_hi, 0, 32);
        regs->insert("lo", mips_regclass_spr, mips_spr_lo, 0, 32);
        regs->insert("pc", mips_regclass_spr, mips_spr_pc, 0, 32);              // program counter

        // 32 floating point registers
        for (size_t i=0; i<32; ++i)
            regs->insert("f"+StringUtility::numberToString(i), mips_regclass_fpr, i, 0, 32);

        // Five FPU control registers are used to identify and control the FPU. The FCCR, FEXR, and FENR are portions
        // (not necessarily contiguous) of the FCSR extended to 32 bits, and therefore all share a major number.
        regs->insert("fir", mips_regclass_spr, mips_spr_fir, 0, 32);            // FP implementation and revision
        regs->insert("fcsr", mips_regclass_fcsr, mips_fcsr_all, 0, 32);         // the entire FCSR register
        regs->insert("fccr", mips_regclass_fcsr, mips_fcsr_fccr, 0, 32);        // condition codes portion of FCSR
        regs->insert("fexr", mips_regclass_fcsr, mips_fcsr_fexr, 0, 32);        // FP exceptions
        regs->insert("fenr", mips_regclass_fcsr, mips_fcsr_fenr, 0, 32);        // FP enables

        // parts of the FIR (only those defined for MIPS32 release 1)
        regs->insert("fir.d", mips_regclass_spr, mips_spr_fir, 17, 1);          // is double-precision implemented?
        regs->insert("fir.s", mips_regclass_spr, mips_spr_fir, 16, 1);          // is single-precision implemented?
        regs->insert("fir.processorid", mips_regclass_spr, mips_spr_fir, 8, 8); // identifies the FP processor
        regs->insert("fir.revision", mips_regclass_spr, mips_spr_fir, 0, 8);    // FP unit revision number

        // Additional registers for coprocessor 0 are not part of this dictionary. They use major number mips_regclass_cp0gpr.

        // Additional implementation-specific coprocessor 2 registers are not part of the dictionary. Coprocessor 2 may have up
        // to 32 general purpose registers and up to 32 control registers.  They use the major numbers mips_regclass_cp2gpr and
        // mips_regclass_cp2spr.
    }
    return regs;
}

const RegisterDictionary *
RegisterDictionary::dictionary_mips32_altnames() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary *regs = NULL;
    if (!regs) {
        regs = new RegisterDictionary("mips32");
        regs->insert(dictionary_mips32());

        // Alternate names for the general purpose registers
        regs->insert("zero", mips_regclass_gpr, 0,  0, 32);                     // always equal to zero
        regs->insert("at",   mips_regclass_gpr, 1,  0, 32);                     // assembler temporary
        regs->insert("v0",   mips_regclass_gpr, 2,  0, 32);                     // return value from a function call
        regs->insert("v1",   mips_regclass_gpr, 3,  0, 32);
        regs->insert("a0",   mips_regclass_gpr, 4,  0, 32);                     // first four function arguments
        regs->insert("a1",   mips_regclass_gpr, 5,  0, 32);
        regs->insert("a2",   mips_regclass_gpr, 6,  0, 32);
        regs->insert("a3",   mips_regclass_gpr, 7,  0, 32);
        regs->insert("t0",   mips_regclass_gpr, 8,  0, 32);                     // temporary variables; need not be preserved
        regs->insert("t1",   mips_regclass_gpr, 9,  0, 32);
        regs->insert("t2",   mips_regclass_gpr, 10, 0, 32);
        regs->insert("t3",   mips_regclass_gpr, 11, 0, 32);
        regs->insert("t4",   mips_regclass_gpr, 12, 0, 32);
        regs->insert("t5",   mips_regclass_gpr, 13, 0, 32);
        regs->insert("t6",   mips_regclass_gpr, 14, 0, 32);
        regs->insert("t7",   mips_regclass_gpr, 15, 0, 32);
        regs->insert("s0",   mips_regclass_gpr, 16, 0, 32);                     // temporary variables; must be preserved
        regs->insert("s1",   mips_regclass_gpr, 17, 0, 32);
        regs->insert("s2",   mips_regclass_gpr, 18, 0, 32);
        regs->insert("s3",   mips_regclass_gpr, 19, 0, 32);
        regs->insert("s4",   mips_regclass_gpr, 20, 0, 32);
        regs->insert("s5",   mips_regclass_gpr, 21, 0, 32);
        regs->insert("s6",   mips_regclass_gpr, 22, 0, 32);
        regs->insert("s7",   mips_regclass_gpr, 23, 0, 32);
        regs->insert("t8",   mips_regclass_gpr, 24, 0, 32);                     // two more temporaries; need not be preserved
        regs->insert("t9",   mips_regclass_gpr, 25, 0, 32);
        regs->insert("k0",   mips_regclass_gpr, 26, 0, 32);                     // kernel use; may change unexpectedly
        regs->insert("k1",   mips_regclass_gpr, 27, 0, 32);
        regs->insert("gp",   mips_regclass_gpr, 28, 0, 32);                     // global pointer
        regs->insert("sp",   mips_regclass_gpr, 29, 0, 32);                     // stack pointer
        regs->insert("s8",   mips_regclass_gpr, 30, 0, 32);                     // temp; must be preserved (or "fp")
        regs->insert("fp",   mips_regclass_gpr, 30, 0, 32);                     // stack frame pointer (or "s8")
        regs->insert("ra",   mips_regclass_gpr, 31, 0, 32);                     // return address (link register)
    }
    return regs;
}

const RegisterDictionary *
RegisterDictionary::dictionary_m68000() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary *regs = NULL;
    if (!regs) {
        regs = new RegisterDictionary("m68000");

        // 32-bit integer data and address registers. When the low-order 16-bits of a data or address register is accessed by a
        // word instruction, or the low-order 8-bits of a data register is accessed by a byte instruction, the size of the
        // access is implied by the suffix of the instruction (e.g., "MOV.B" vs "MOV.W", vs. MOV.L") and the usual m68k
        // assembly listings to not have special names for the register parts.  ROSE on the other hand gives names to each
        // register part.
        for (size_t i=0; i<8; ++i) {
            std::string regnum = StringUtility::numberToString(i);
            regs->insert("d"+regnum,       m68k_regclass_data, i,  0, 32);
            regs->insert("d"+regnum+".w0", m68k_regclass_data, i,  0, 16);
            regs->insert("d"+regnum+".w1", m68k_regclass_data, i, 16, 16);
            regs->insert("d"+regnum+".b0", m68k_regclass_data, i,  0,  8);
            regs->insert("d"+regnum+".b1", m68k_regclass_data, i,  8,  8);
            regs->insert("d"+regnum+".b2", m68k_regclass_data, i, 12,  8);
            regs->insert("d"+regnum+".b3", m68k_regclass_data, i, 16,  8);
            regs->insert("a"+regnum,       m68k_regclass_addr, i,  0, 32);
            regs->insert("a"+regnum+".w0", m68k_regclass_addr, i,  0, 16);
            regs->insert("a"+regnum+".w1", m68k_regclass_addr, i, 16, 16);
        }

        // Special-purpose registers
        regs->insert("pc",    m68k_regclass_spr, m68k_spr_pc, 0,  32);          // program counter
        regs->insert("ccr",   m68k_regclass_spr, m68k_spr_sr, 0,  8);           // condition code register
        regs->insert("ccr_c", m68k_regclass_spr, m68k_spr_sr, 0,  1);           // condition code carry bit
        regs->insert("ccr_v", m68k_regclass_spr, m68k_spr_sr, 1,  1);           // condition code overflow
        regs->insert("ccr_z", m68k_regclass_spr, m68k_spr_sr, 2,  1);           // condition code zero bit
        regs->insert("ccr_n", m68k_regclass_spr, m68k_spr_sr, 3,  1);           // condition code negative
        regs->insert("ccr_x", m68k_regclass_spr, m68k_spr_sr, 4,  1);           // condition code extend
        regs->insert("sr",    m68k_regclass_spr, m68k_spr_sr, 0,  16);          // status register
        regs->insert("sr_i",  m68k_regclass_spr, m68k_spr_sr, 8,  3);           // interrupt priority mask
        regs->insert("sr_s",  m68k_regclass_spr, m68k_spr_sr, 13, 1);           // status register user mode bit
        regs->insert("sr_t",  m68k_regclass_spr, m68k_spr_sr, 14, 2);           // status register trace mode bits

        // Floating point data registers
        // These registers hold 96-bit extended-precision real format ("X") values. However, since the X format has
        // 16 reserved zero bits at positions 64-79, inclusive, the value can be stored in 80 bits.  Therefore, the
        // floating point data registers are only 80-bits wide.
        for (size_t i=0; i<8; ++i)
            regs->insert("fp"+StringUtility::numberToString(i),      m68k_regclass_fpr,  i, 0,  80);

        // Floating point control registers
        regs->insert("fpcr",       m68k_regclass_spr, m68k_spr_fpcr,  0, 32);   // floating-point control register
        regs->insert("fpcr_mctl",  m68k_regclass_spr, m68k_spr_fpcr,  0,  8);   // mode control
        regs->insert("fpcr_xen",   m68k_regclass_spr, m68k_spr_fpcr,  8,  8);   // exception enable
        regs->insert("fpcr_rnd",   m68k_regclass_spr, m68k_spr_fpcr,  4,  2);   // rounding mode
        regs->insert("fpcr_prec",  m68k_regclass_spr, m68k_spr_fpcr,  6,  2);   // rounding precision
        regs->insert("fpcr_inex1", m68k_regclass_spr, m68k_spr_fpcr,  8,  1);   // inexact decimal input
        regs->insert("fpcr_inex2", m68k_regclass_spr, m68k_spr_fpcr,  9,  1);   // inexact operation
        regs->insert("fpcr_dz",    m68k_regclass_spr, m68k_spr_fpcr, 10,  1);   // divide by zero
        regs->insert("fpcr_unfl",  m68k_regclass_spr, m68k_spr_fpcr, 11,  1);   // underflow
        regs->insert("fpcr_ovfl",  m68k_regclass_spr, m68k_spr_fpcr, 12,  1);   // overflow
        regs->insert("fpcr_operr", m68k_regclass_spr, m68k_spr_fpcr, 13,  1);   // operand error
        regs->insert("fpcr_snan",  m68k_regclass_spr, m68k_spr_fpcr, 14,  1);   // signaling not-a-number
        regs->insert("fpcr_bsun",  m68k_regclass_spr, m68k_spr_fpcr, 15,  1);   // branch/set on unordered

        // Floating point status registers
        regs->insert("fpsr",       m68k_regclass_spr, m68k_spr_fpsr,  0, 32);   // floating-point status register
        regs->insert("fpsr_aexc",  m68k_regclass_spr, m68k_spr_fpsr,  0,  8);   // accrued exception status
        regs->insert("aexc_inex",  m68k_regclass_spr, m68k_spr_fpsr,  3,  1);   // inexact
        regs->insert("aexc_dz",    m68k_regclass_spr, m68k_spr_fpsr,  4,  1);   // divide by zero
        regs->insert("aexc_unfl",  m68k_regclass_spr, m68k_spr_fpsr,  5,  1);   // underflow
        regs->insert("aexc_ovfl",  m68k_regclass_spr, m68k_spr_fpsr,  6,  1);   // overflow
        regs->insert("aexc_iop",   m68k_regclass_spr, m68k_spr_fpsr,  7,  1);   // invalid operation
        regs->insert("fpsr_exc",   m68k_regclass_spr, m68k_spr_fpsr,  8,  8);   // exception status
        regs->insert("exc_inex1",  m68k_regclass_spr, m68k_spr_fpsr,  8,  1);   // inexact decimal input (input is denormalized)
        regs->insert("exc_inex2",  m68k_regclass_spr, m68k_spr_fpsr,  9,  1);   // inexact operation (inexact result)
        regs->insert("exc_dz",     m68k_regclass_spr, m68k_spr_fpsr, 10,  1);   // divide by zero
        regs->insert("exc_unfl",   m68k_regclass_spr, m68k_spr_fpsr, 11,  1);   // underflow
        regs->insert("exc_ovfl",   m68k_regclass_spr, m68k_spr_fpsr, 12,  1);   // overflow
        regs->insert("exc_operr",  m68k_regclass_spr, m68k_spr_fpsr, 13,  1);   // operand error
        regs->insert("exc_snan",   m68k_regclass_spr, m68k_spr_fpsr, 14,  1);   // signaling not-a-number
        regs->insert("exc_bsun",   m68k_regclass_spr, m68k_spr_fpsr, 15,  1);   // branch/set on unordered
        regs->insert("fpsr_quot",  m68k_regclass_spr, m68k_spr_fpsr, 16,  8);   // quotient
        regs->insert("fpcc",       m68k_regclass_spr, m68k_spr_fpsr, 24,  8);   // floating-point conditon code
        regs->insert("fpcc_nan",   m68k_regclass_spr, m68k_spr_fpsr, 24,  1);   // not-a-number or unordered
        regs->insert("fpcc_i",     m68k_regclass_spr, m68k_spr_fpsr, 25,  1);   // infinity
        regs->insert("fpcc_z",     m68k_regclass_spr, m68k_spr_fpsr, 25,  1);   // zero
        regs->insert("fpcc_n",     m68k_regclass_spr, m68k_spr_fpsr, 26,  1);   // negative

        // Other floating point registers
        regs->insert("fpiar", m68k_regclass_spr,   m68k_spr_fpiar,   0, 32);    // floating-point instruction address reg
        
        // Supervisor registers (SR register is listed above since its CCR bits are available in user mode)
        regs->insert("ssp",      m68k_regclass_sup, m68k_sup_ssp,       0, 32); // supervisor A7 stack pointer
        regs->insert("vbr",      m68k_regclass_sup, m68k_sup_vbr,       0, 32); // vector base register
    }
    return regs;
}

const RegisterDictionary *
RegisterDictionary::dictionary_m68000_altnames() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary *regs = NULL;
    if (!regs) {
        regs = new RegisterDictionary("m68000");
        regs->insert(dictionary_m68000());
        regs->insert("bp", m68k_regclass_addr, 6, 0, 32);                       // a6 is conventionally the stack frame pointer
        regs->insert("sp", m68k_regclass_addr, 7, 0, 32);                       // a7 is conventionally the stack pointer
    }
    return regs;
}

// FIXME[Robb P. Matzke 2014-07-15]: This is fairly generic at this point. Eventually we'll split this function into
// dictionaries for each specific Freescale ColdFire architecture.
const RegisterDictionary *
RegisterDictionary::dictionary_coldfire() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary *regs = NULL;
    if (!regs) {
        regs = new RegisterDictionary("freescale MAC");
        regs->insert(dictionary_m68000());

        // Floating point data registers.
        // The ColdFire processors do not support extended precision real ("X") format values, and therefore don't need
        // full 80-bit floating point data registers.  Their FP data registers are only 64 bits.
        for (int i=0; i<8; ++i)
            regs->resize("fp"+StringUtility::numberToString(i), 64);

        // MAC unit
        regs->insert("macsr",     m68k_regclass_mac, m68k_mac_macsr, 0, 32);    // MAC status register
        regs->insert("macsr_c",   m68k_regclass_mac, m68k_mac_macsr, 0,  1);    //   Carry flag; this field is always zero.
        regs->insert("macsr_v",   m68k_regclass_mac, m68k_mac_macsr, 1,  1);    //   Overflow flag
        regs->insert("macsr_z",   m68k_regclass_mac, m68k_mac_macsr, 2,  1);    //   Zero flag
        regs->insert("macsr_n",   m68k_regclass_mac, m68k_mac_macsr, 3,  1);    //   Negative flag
        regs->insert("macsr_rt",  m68k_regclass_mac, m68k_mac_macsr, 4,  1);    //   Round/truncate mode
        regs->insert("macsr_fi",  m68k_regclass_mac, m68k_mac_macsr, 5,  1);    //   Fraction/integer mode
        regs->insert("macsr_su",  m68k_regclass_mac, m68k_mac_macsr, 6,  1);    //   Signed/unsigned operations mode
        regs->insert("macsr_omc", m68k_regclass_mac, m68k_mac_macsr, 7,  1);    //   Overflow/saturation mode
        regs->insert("acc",       m68k_regclass_mac, m68k_mac_acc0,  0, 32);    // MAC accumulator
        regs->insert("mask",      m68k_regclass_mac, m68k_mac_mask,  0, 32);    // MAC mask register (upper 16 bits are set)

        // Supervisor registers
        regs->insert("cacr",     m68k_regclass_sup, m68k_sup_cacr,      0, 32); // cache control register
        regs->insert("asid",     m68k_regclass_sup, m68k_sup_asid,      0, 32); // address space ID register
        regs->insert("acr0",     m68k_regclass_sup, m68k_sup_acr0,      0, 32); // access control register 0 (data)
        regs->insert("acr1",     m68k_regclass_sup, m68k_sup_acr1,      0, 32); // access control register 1 (data)
        regs->insert("acr2",     m68k_regclass_sup, m68k_sup_acr2,      0, 32); // access control register 2 (instruction)
        regs->insert("acr3",     m68k_regclass_sup, m68k_sup_acr3,      0, 32); // access control register 3 (instruction)
        regs->insert("mmubar",   m68k_regclass_sup, m68k_sup_mmubar,    0, 32); // MMU base address register
        regs->insert("rombar0",  m68k_regclass_sup, m68k_sup_rombar0,   0, 32); // ROM base address register 0
        regs->insert("rombar1",  m68k_regclass_sup, m68k_sup_rombar1,   0, 32); // ROM base address register 1
        regs->insert("rambar0",  m68k_regclass_sup, m68k_sup_rambar0,   0, 32); // RAM base address register 0
        regs->insert("rambar1",  m68k_regclass_sup, m68k_sup_rambar1,   0, 32); // RAM base address register 1
        regs->insert("mbar",     m68k_regclass_sup, m68k_sup_mbar,      0, 32); // module base address register
        regs->insert("mpcr",     m68k_regclass_sup, m68k_sup_mpcr,      0, 32); // multiprocessor control register
        regs->insert("edrambar", m68k_regclass_sup, m68k_sup_edrambar,  0, 32); // embedded DRAM base address register
        regs->insert("secmbar",  m68k_regclass_sup, m68k_sup_secmbar,   0, 32); // secondary module base address register
        regs->insert("pcr1u0",   m68k_regclass_sup, m68k_sup_0_pcr1,   32, 32); // 32 msbs of RAM 0 permutation control reg 1
        regs->insert("pcr1l0",   m68k_regclass_sup, m68k_sup_0_pcr1,    0, 32); // 32 lsbs of RAM 0 permutation control reg 1
        regs->insert("pcr2u0",   m68k_regclass_sup, m68k_sup_0_pcr2,   32, 32); // 32 msbs of RAM 0 permutation control reg 2
        regs->insert("pcr2l0",   m68k_regclass_sup, m68k_sup_0_pcr2,    0, 32); // 32 lsbs of RAM 0 permutation control reg 2
        regs->insert("pcr3u0",   m68k_regclass_sup, m68k_sup_0_pcr3,   32, 32); // 32 msbs of RAM 0 permutation control reg 3
        regs->insert("pcr3l0",   m68k_regclass_sup, m68k_sup_0_pcr3,    0, 32); // 32 lsbs of RAM 0 permutation control reg 3
        regs->insert("pcr1u1",   m68k_regclass_sup, m68k_sup_1_pcr1,   32, 32); // 32 msbs of RAM 1 permutation control reg 1
        regs->insert("pcr1l1",   m68k_regclass_sup, m68k_sup_1_pcr1,    0, 32); // 32 lsbs of RAM 1 permutation control reg 1
        regs->insert("pcr2u1",   m68k_regclass_sup, m68k_sup_1_pcr2,   32, 32); // 32 msbs of RAM 1 permutation control reg 2
        regs->insert("pcr2l1",   m68k_regclass_sup, m68k_sup_1_pcr2,    0, 32); // 32 lsbs of RAM 1 permutation control reg 2
        regs->insert("pcr3u1",   m68k_regclass_sup, m68k_sup_1_pcr3,   32, 32); // 32 msbs of RAM 1 permutation control reg 3
        regs->insert("pcr3l1",   m68k_regclass_sup, m68k_sup_1_pcr3,    0, 32); // 32 lsbs of RAM 1 permutation control reg 3
    }
    return regs;
}

// FreeScale ColdFire CPUs with EMAC (extended multiply-accumulate) unit.
const RegisterDictionary *
RegisterDictionary::dictionary_coldfire_emac() {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    static RegisterDictionary *regs = NULL;
    if (!regs) {
        regs = new RegisterDictionary("freescale EMAC");
        regs->insert(dictionary_coldfire());
        
        regs->insert("macsr_pav0", m68k_regclass_mac, m68k_mac_macsr,  8,  1);  // overflow flag for accumulator 0
        regs->insert("macsr_pav1", m68k_regclass_mac, m68k_mac_macsr,  9,  1);  // overflow flag for accumulator 1
        regs->insert("macsr_pav2", m68k_regclass_mac, m68k_mac_macsr, 10,  1);  // overflow flag for accumulator 2
        regs->insert("macsr_pav3", m68k_regclass_mac, m68k_mac_macsr, 11,  1);  // overflow flag for accumulator 3

        regs->insert("acc0",       m68k_regclass_mac, m68k_mac_acc0,   0, 32);  // accumulator #0
        regs->insert("acc1",       m68k_regclass_mac, m68k_mac_acc1,   0, 32);  // accumulator #1
        regs->insert("acc2",       m68k_regclass_mac, m68k_mac_acc2,   0, 32);  // accumulator #2
        regs->insert("acc3",       m68k_regclass_mac, m68k_mac_acc3,   0, 32);  // accumulator #3

        regs->insert("accext01",   m68k_regclass_mac, m68k_mac_ext01,  0, 32);  // extensions for acc0 and acc1
        regs->insert("accext0",    m68k_regclass_mac, m68k_mac_ext0,   0, 16);
        regs->insert("accext1",    m68k_regclass_mac, m68k_mac_ext1,  16, 16);
        regs->insert("accext23",   m68k_regclass_mac, m68k_mac_ext23,  0, 32);  // extensions for acc2 and acc3
        regs->insert("accext2",    m68k_regclass_mac, m68k_mac_ext2,   0, 16);
        regs->insert("accext3",    m68k_regclass_mac, m68k_mac_ext3,  16, 16);
    }
    return regs;
}

#endif
