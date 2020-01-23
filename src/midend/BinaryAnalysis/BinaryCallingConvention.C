#include <sage3basic.h>
#include <BinaryCallingConvention.h>

#include <BinaryDataFlow.h>                             // Dataflow engine
#include <boost/foreach.hpp>
#include <CommandLine.h>
#include <Diagnostics.h>
#include <MemoryCellList.h>
#include <Partitioner2/DataFlow.h>                      // Dataflow components that we can re-use
#include <Partitioner2/Partitioner.h>                   // Fast binary analysis data structures
#include <Partitioner2/Function.h>                      // Fast function data structures
#include <Sawyer/ProgressBar.h>

using namespace Rose::Diagnostics;
using namespace Rose::BinaryAnalysis::InstructionSemantics2;
using namespace Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

namespace Rose {
namespace BinaryAnalysis {
namespace CallingConvention {

Sawyer::Message::Facility mlog;

void
initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::CallingConvention");
        mlog.comment("computing function calling conventions");
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Dictionaries
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const Dictionary&
dictionaryArm() {
    static Dictionary dict;
    // FIXME[Robb P. Matzke 2015-08-21]: none defind yet
    return dict;
}

const Dictionary&
dictionaryM68k() {
    static Dictionary dict;
    // FIXME[Robb P. Matzke 2015-08-20]: none defined yet
    return dict;
}

const Dictionary&
dictionaryMips() {
    static Dictionary dict;
    // FIXME[Robb P. Matzke 2015-08-21]: none defind yet
    return dict;
}

const Dictionary&
dictionaryPowerpc32() {
    static Dictionary dict;
    if (dict.empty())
        dict.push_back(Definition::ppc_32bit_ibm());
    return dict;
}

const Dictionary&
dictionaryPowerpc64() {
    static Dictionary dict;
    // FIXME[Robb Matzke 2019-08-07]: none defined yet
    return dict;
}

const Dictionary&
dictionaryAmd64() {
    static Dictionary dict;
    if (dict.empty()) {
        //--------
        // 64-bit
        //--------

        // Listed first because it's the most common
        dict.push_back(Definition::x86_64bit_sysv());

#if 0 // [Robb P. Matzke 2015-08-21]: don't bother distinguishing because alignment is not used yet.
        // cdecl: gcc < 4.5 uses 4-byte stack alignment
        cc = Definition::x86_64bit_cdecl();
        cc.comment(cc.comment() + " 4-byte alignment");
        cc.stackAlignment(4);
        dict.push_back(cc);

        // cdecl: gcc >= 4.5 uses 16-byte stack alignment
        cc = Definition::x86_64bit_cdecl();
        cc.comment(cc.comment() + " 16-byte alignment");
        cc.stackAlignment(16);
        dict.push_back(cc);
#else
        dict.push_back(Definition::x86_64bit_cdecl());
#endif
        dict.push_back(Definition::x86_64bit_stdcall());

        //--------
        // 32-bit
        //--------

#if 0 // [Robb P. Matzke 2015-08-21]: don't bother distinguishing because alignment is not used yet.
        // cdecl: gcc < 4.5 uses 4-byte stack alignment
        Definition cc = Definition::x86_32bit_cdecl();
        cc.comment(cc.comment() + " 4-byte alignment");
        cc.stackAlignment(4);
        dict.push_back(cc);

        // cdecl: gcc >= 4.5 uses 16-byte stack alignment
        cc = Definition::x86_32bit_cdecl();
        cc.comment(cc.comment() + " 16-byte alignment");
        cc.stackAlignment(16);
        dict.push_back(cc);
#else
        dict.push_back(Definition::x86_32bit_cdecl());
#endif

        // other conventions
        dict.push_back(Definition::x86_32bit_stdcall());
        dict.push_back(Definition::x86_32bit_fastcall());
    }
    return dict;
}

const Dictionary&
dictionaryX86() {
    static Dictionary dict;
    if (dict.empty()) {
        //--------
        // 32-bit
        //--------

#if 0 // [Robb P. Matzke 2015-08-21]: don't bother distinguishing because alignment is not used yet.
        // cdecl: gcc < 4.5 uses 4-byte stack alignment
        Definition cc = Definition::x86_32bit_cdecl();
        cc.comment(cc.comment() + " 4-byte alignment");
        cc.stackAlignment(4);
        dict.push_back(cc);

        // cdecl: gcc >= 4.5 uses 16-byte stack alignment
        cc = Definition::x86_32bit_cdecl();
        cc.comment(cc.comment() + " 16-byte alignment");
        cc.stackAlignment(16);
        dict.push_back(cc);
#else
        dict.push_back(Definition::x86_32bit_cdecl());
#endif

        // other conventions
        dict.push_back(Definition::x86_32bit_stdcall());
        dict.push_back(Definition::x86_32bit_fastcall());
    }
    return dict;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Definition
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// class method
Definition::Ptr
Definition::x86_32bit_cdecl() {
    static Ptr cc;
    if (!cc)
        cc = x86_cdecl(RegisterDictionary::dictionary_pentium4());
    return cc;
}

// class method
Definition::Ptr
Definition::x86_64bit_cdecl() {
    static Ptr cc;
    if (!cc)
        cc = x86_cdecl(RegisterDictionary::dictionary_amd64());
    return cc;
}

// class method
Definition::Ptr
Definition::x86_cdecl(const RegisterDictionary *regDict) {
    ASSERT_not_null(regDict);
    const RegisterDescriptor SP = regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_sp);
    Ptr cc = instance(SP.nBits(), "cdecl",
                      "x86-" + StringUtility::numberToString(SP.nBits()) + " cdecl",
                      regDict);

    // Stack characteristics
    cc->stackPointerRegister(SP);
    cc->stackDirection(GROWS_DOWN);
    cc->nonParameterStackSize(cc->wordWidth() >> 3);    // return address

    // All parameters are passed on the stack.
    cc->stackParameterOrder(RIGHT_TO_LEFT);
    cc->stackCleanup(CLEANUP_BY_CALLER);

    // Other inputs
    cc->appendInputParameter(*regDict->lookup("df"));   // direction flag is always assumed to be valid

    // Return values
    cc->appendOutputParameter(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_ax));
    cc->appendOutputParameter(regDict->findLargestRegister(x86_regclass_st, x86_st_0));
    cc->appendOutputParameter(SP);                      // final value is usually one word greater than initial value

    // Scratch registers (i.e., modified, not callee-saved, not return registers)
    cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_cx));
    cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_dx));
    cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_ip, 0));
    cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_flags, x86_flags_status));
    cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_flags, x86_flags_fpstatus));

    // Callee-saved registers (everything else)
    RegisterParts regParts = regDict->getAllParts() - cc->getUsedRegisterParts();
    std::vector<RegisterDescriptor> registers = regParts.extract(regDict);
    cc->calleeSavedRegisters().insert(registers.begin(), registers.end());

    return cc;
}

// class method
Definition::Ptr
Definition::ppc_32bit_ibm() {
    static Ptr cc;
    if (!cc)
        cc = ppc_ibm(RegisterDictionary::dictionary_powerpc32());
    return cc;
}

// class method
Definition::Ptr
Definition::ppc_ibm(const RegisterDictionary *regDict) {
    // See https://www.ibm.com/support/knowledgecenter/en/ssw_aix_72/com.ibm.aix.alangref/idalangref_reg_use_conv.htm
    ASSERT_not_null(regDict);
    const RegisterDescriptor SP = regDict->findLargestRegister(powerpc_regclass_gpr, 1);
    Ptr cc = instance(SP.nBits(), "IBM", "PowerPC-" + StringUtility::numberToString(SP.nBits()) + " IBM", regDict);

    // Stack characteristics
    cc->stackPointerRegister(SP);
    cc->stackDirection(GROWS_DOWN);
    cc->nonParameterStackSize(0);                       // return address is in link register

    // Function arguments are passed in registers
    cc->appendInputParameter(regDict->findLargestRegister(powerpc_regclass_gpr, 3));
    cc->appendInputParameter(regDict->findLargestRegister(powerpc_regclass_gpr, 4));
    cc->appendInputParameter(regDict->findLargestRegister(powerpc_regclass_gpr, 5));
    cc->appendInputParameter(regDict->findLargestRegister(powerpc_regclass_gpr, 6));
    cc->appendInputParameter(regDict->findLargestRegister(powerpc_regclass_gpr, 7));
    cc->appendInputParameter(regDict->findLargestRegister(powerpc_regclass_gpr, 8));
    cc->appendInputParameter(regDict->findLargestRegister(powerpc_regclass_gpr, 9));
    cc->appendInputParameter(regDict->findLargestRegister(powerpc_regclass_gpr, 10));

    cc->appendInputParameter(regDict->findLargestRegister(powerpc_regclass_fpr, 1));
    cc->appendInputParameter(regDict->findLargestRegister(powerpc_regclass_fpr, 2));
    cc->appendInputParameter(regDict->findLargestRegister(powerpc_regclass_fpr, 3));
    cc->appendInputParameter(regDict->findLargestRegister(powerpc_regclass_fpr, 4));
    cc->appendInputParameter(regDict->findLargestRegister(powerpc_regclass_fpr, 5));
    cc->appendInputParameter(regDict->findLargestRegister(powerpc_regclass_fpr, 6));
    cc->appendInputParameter(regDict->findLargestRegister(powerpc_regclass_fpr, 7));
    cc->appendInputParameter(regDict->findLargestRegister(powerpc_regclass_fpr, 8));
    cc->appendInputParameter(regDict->findLargestRegister(powerpc_regclass_fpr, 9));
    cc->appendInputParameter(regDict->findLargestRegister(powerpc_regclass_fpr, 10));
    cc->appendInputParameter(regDict->findLargestRegister(powerpc_regclass_fpr, 11));
    cc->appendInputParameter(regDict->findLargestRegister(powerpc_regclass_fpr, 12));
    cc->appendInputParameter(regDict->findLargestRegister(powerpc_regclass_fpr, 13));

    // Stack is generally not used for passing arguments
    cc->stackParameterOrder(RIGHT_TO_LEFT);
    cc->stackCleanup(CLEANUP_BY_CALLER);

    // Return values
    cc->appendOutputParameter(regDict->findLargestRegister(powerpc_regclass_gpr, 3)); // primary return
    cc->appendOutputParameter(regDict->findLargestRegister(powerpc_regclass_gpr, 4)); // secondary return

    cc->appendOutputParameter(regDict->findLargestRegister(powerpc_regclass_fpr, 1));
    cc->appendOutputParameter(regDict->findLargestRegister(powerpc_regclass_fpr, 2));
    cc->appendOutputParameter(regDict->findLargestRegister(powerpc_regclass_fpr, 3));
    cc->appendOutputParameter(regDict->findLargestRegister(powerpc_regclass_fpr, 4));

    // Scratch registers (function arguments that are not return values, plus others)
    cc->scratchRegisters().insert(regDict->findLargestRegister(powerpc_regclass_gpr, 0));
    cc->scratchRegisters().insert(regDict->findLargestRegister(powerpc_regclass_gpr, 5));
    cc->scratchRegisters().insert(regDict->findLargestRegister(powerpc_regclass_gpr, 6));
    cc->scratchRegisters().insert(regDict->findLargestRegister(powerpc_regclass_gpr, 7));
    cc->scratchRegisters().insert(regDict->findLargestRegister(powerpc_regclass_gpr, 8));
    cc->scratchRegisters().insert(regDict->findLargestRegister(powerpc_regclass_gpr, 9));
    cc->scratchRegisters().insert(regDict->findLargestRegister(powerpc_regclass_gpr, 10));
    cc->scratchRegisters().insert(regDict->findLargestRegister(powerpc_regclass_gpr, 11));
    cc->scratchRegisters().insert(regDict->findLargestRegister(powerpc_regclass_gpr, 12));

    cc->scratchRegisters().insert(regDict->findLargestRegister(powerpc_regclass_fpr, 0));
    cc->scratchRegisters().insert(regDict->findLargestRegister(powerpc_regclass_fpr, 5));
    cc->scratchRegisters().insert(regDict->findLargestRegister(powerpc_regclass_fpr, 6));
    cc->scratchRegisters().insert(regDict->findLargestRegister(powerpc_regclass_fpr, 7));
    cc->scratchRegisters().insert(regDict->findLargestRegister(powerpc_regclass_fpr, 8));
    cc->scratchRegisters().insert(regDict->findLargestRegister(powerpc_regclass_fpr, 9));
    cc->scratchRegisters().insert(regDict->findLargestRegister(powerpc_regclass_fpr, 10));
    cc->scratchRegisters().insert(regDict->findLargestRegister(powerpc_regclass_fpr, 11));
    cc->scratchRegisters().insert(regDict->findLargestRegister(powerpc_regclass_fpr, 12));
    cc->scratchRegisters().insert(regDict->findLargestRegister(powerpc_regclass_fpr, 13));

    cc->scratchRegisters().insert(regDict->findLargestRegister(powerpc_regclass_cr, 0));
    cc->scratchRegisters().insert(regDict->findLargestRegister(powerpc_regclass_fpscr, 0));
    cc->scratchRegisters().insert(regDict->findLargestRegister(powerpc_regclass_iar, 0));

    // Callee-saved registers (everything else)
    RegisterParts regParts = regDict->getAllParts() - cc->getUsedRegisterParts();
    std::vector<RegisterDescriptor> registers = regParts.extract(regDict);
    cc->calleeSavedRegisters().insert(registers.begin(), registers.end());

    return cc;
}

// class method
Definition::Ptr
Definition::x86_32bit_stdcall() {
    static Ptr cc;
    if (!cc)
        cc = x86_stdcall(RegisterDictionary::dictionary_pentium4());
    return cc;
}

// class method
Definition::Ptr
Definition::x86_64bit_stdcall() {
    static Ptr cc;
    if (!cc)
        cc = x86_stdcall(RegisterDictionary::dictionary_amd64());
    return cc;
}

// class method
Definition::Ptr
Definition::x86_stdcall(const RegisterDictionary *regDict) {
    ASSERT_not_null(regDict);
    const RegisterDescriptor SP = regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_sp);
    Ptr cc = instance(SP.nBits(), "stdcall",
                      "x86-" + StringUtility::numberToString(SP.nBits()) + " stdcall",
                      regDict);

    // Stack characteristics
    cc->stackPointerRegister(SP);
    cc->stackDirection(GROWS_DOWN);
    cc->nonParameterStackSize(cc->wordWidth() >> 3);    // return address

    // All parameters are passed on the stack
    cc->stackParameterOrder(RIGHT_TO_LEFT);
    cc->stackCleanup(CLEANUP_BY_CALLEE);

    // Other inputs
    cc->appendInputParameter(*regDict->lookup("df"));   // direction flag is always assumed to be valid

    // Return values
    cc->appendOutputParameter(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_ax));
    cc->appendOutputParameter(SP);

    // Scratch registers (i.e., modified, not callee-saved, not return registers)
    cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_cx));
    cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_dx));
    cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_ip, 0));
    cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_flags, x86_flags_status));
    cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_flags, x86_flags_fpstatus));

    // Callee-saved registers (everything else)
    RegisterParts regParts = regDict->getAllParts() - cc->getUsedRegisterParts();
    std::vector<RegisterDescriptor> registers = regParts.extract(regDict);
    cc->calleeSavedRegisters().insert(registers.begin(), registers.end());

    return cc;
}

// class method
Definition::Ptr
Definition::x86_32bit_fastcall() {
    static Ptr cc;
    if (!cc)
        cc = x86_fastcall(RegisterDictionary::dictionary_pentium4());
    return cc;
}

// class method
Definition::Ptr
Definition::x86_fastcall(const RegisterDictionary *regDict) {
    ASSERT_not_null(regDict);
    const RegisterDescriptor SP = regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_sp);
    static Ptr cc = instance(SP.nBits(), "fastcall",
                             "x86-" + StringUtility::numberToString(SP.nBits()) + " fastcall",
                             regDict);

    // Stack characteristics
    cc->stackPointerRegister(SP);
    cc->stackDirection(GROWS_DOWN);
    cc->nonParameterStackSize(cc->wordWidth() >> 3);    // return address

    // Uses ECX and EDX for first args that fit; all other parameters are passed on the stack.
    cc->appendInputParameter(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_cx));
    cc->appendInputParameter(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_dx));
    cc->stackParameterOrder(RIGHT_TO_LEFT);
    cc->stackCleanup(CLEANUP_BY_CALLEE);
    cc->appendInputParameter(*regDict->lookup("df"));   // direction flag is always assumed to be valid

    // Return values
    cc->appendOutputParameter(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_ax));
    cc->appendOutputParameter(SP);

    // Scratch registers (i.e., modified, not callee-saved, not return registers)
    cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_cx));
    cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_dx));
    cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_ip, 0));
    cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_flags, x86_flags_status));
    cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_flags, x86_flags_fpstatus));

    // Callee-saved registers (everything else)
    RegisterParts regParts = regDict->getAllParts() - cc->getUsedRegisterParts();
    std::vector<RegisterDescriptor> registers = regParts.extract(regDict);
    cc->calleeSavedRegisters().insert(registers.begin(), registers.end());

    return cc;
}

// class method
Definition::Ptr
Definition::x86_64bit_sysv() {
    static Ptr cc;
    if (!cc) {
        const RegisterDictionary *regDict = RegisterDictionary::dictionary_amd64();
        const RegisterDescriptor SP = regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_sp);

        cc = instance(64, "sysv", "x86-64 sysv", regDict);

        // Stack characteristics
        cc->stackPointerRegister(SP);
        cc->stackDirection(GROWS_DOWN);
        cc->nonParameterStackSize(cc->wordWidth() >> 3); // return address

        //---- Function arguments ----

        // The first six integer or pointer arguments are passed in registers RDI, RSI, RDX, RCX, R8, and R9.
        // These registers are also not preserved across the call.
        cc->appendInputParameter(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_di));
        cc->appendInputParameter(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_si));
        cc->appendInputParameter(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_dx));
        cc->appendInputParameter(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_cx));
        cc->appendInputParameter(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_r8));
        cc->appendInputParameter(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_r9));

        // The first eight SSE arguments are passed in registers xmm0 through xmm7
        cc->appendInputParameter(regDict->findLargestRegister(x86_regclass_xmm, 0));
        cc->appendInputParameter(regDict->findLargestRegister(x86_regclass_xmm, 1));
        cc->appendInputParameter(regDict->findLargestRegister(x86_regclass_xmm, 2));
        cc->appendInputParameter(regDict->findLargestRegister(x86_regclass_xmm, 3));
        cc->appendInputParameter(regDict->findLargestRegister(x86_regclass_xmm, 4));
        cc->appendInputParameter(regDict->findLargestRegister(x86_regclass_xmm, 5));
        cc->appendInputParameter(regDict->findLargestRegister(x86_regclass_xmm, 6));
        cc->appendInputParameter(regDict->findLargestRegister(x86_regclass_xmm, 7));

        // The AL register is an input register that stores the number of SSE registers used for variable argument calls. (It
        // is also part of the first return value).
        cc->appendInputParameter(RegisterDescriptor(x86_regclass_gpr, x86_gpr_ax, 0, 8)); // for varargs calls

        // direction flag is always assumed to be initialized and is thus often treated as input
        cc->appendInputParameter(*regDict->lookup("df"));

        // Arguments that don't fit in the input registers are passed on the stack
        cc->stackParameterOrder(RIGHT_TO_LEFT);
        cc->stackCleanup(CLEANUP_BY_CALLER);

        //---- Return values ----

        cc->appendOutputParameter(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_ax));
        cc->appendOutputParameter(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_dx)); // second integer return
        cc->appendOutputParameter(SP);                   // final value is usually 8 greater than initial value
        cc->appendOutputParameter(regDict->findLargestRegister(x86_regclass_xmm, 0));
        cc->appendOutputParameter(regDict->findLargestRegister(x86_regclass_xmm, 1));
        //cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_st, 0)); // dynamic st(0), overlaps mm<i>
        //cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_st, 1)); // dynamic st(1), overlaps mm<i+1>

        //---- Scratch registers (modified, not callee-saved, not return values) ----

        // Registers that hold arguments are also scratch registers (as long as they're not return values)
        cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_di));
        cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_si));
        //cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_dx)); this is a return reg
        cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_cx));
        cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_r8));
        cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_r9));

        //cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_xmm, 0)); this is a return reg
        //cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_xmm, 1)); this is a return reg
        cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_xmm, 2));
        cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_xmm, 3));
        cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_xmm, 4));
        cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_xmm, 5));
        cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_xmm, 6));
        cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_xmm, 7));

        // These registers are almost always modified by a function
        cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_ip, 0));
        cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_flags, x86_flags_status));
        cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_flags, x86_flags_fpstatus));

        cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_r10)); //static chain ptr
        cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_gpr, x86_gpr_r11));

        // Floating point registers are pretty much all scratch.
        cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_xmm, 8));
        cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_xmm, 9));
        cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_xmm, 10));
        cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_xmm, 11));
        cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_xmm, 12));
        cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_xmm, 13));
        cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_xmm, 14));
        cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_xmm, 15));

        cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_st, 0)); // i.e., statically mm0
        cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_st, 1)); // mm1, etc. Since mm<i> could
        cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_st, 2)); // overlap with st(0) or st(1),
        cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_st, 3)); // two of these could also be
        cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_st, 4)); // return values.
        cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_st, 5));
        cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_st, 6));
        cc->scratchRegisters().insert(regDict->findLargestRegister(x86_regclass_st, 7));

        // Callee-saved registers (everything else)
        RegisterParts regParts = regDict->getAllParts() - cc->getUsedRegisterParts();
        std::vector<RegisterDescriptor> registers = regParts.extract(regDict);
        cc->calleeSavedRegisters().insert(registers.begin(), registers.end());
    }
    return cc;
}

void
Definition::appendInputParameter(const ParameterLocation &newLocation) {
#ifndef NDEBUG
    BOOST_FOREACH (const ParameterLocation &existingLocation, inputParameters_)
        ASSERT_forbid(newLocation == existingLocation);
#endif
    inputParameters_.push_back(newLocation);
}

void
Definition::appendOutputParameter(const ParameterLocation &newLocation) {
#ifndef NDEBUG
    BOOST_FOREACH (const ParameterLocation &existingLocation, outputParameters_)
        ASSERT_forbid(newLocation == existingLocation);
#endif
    outputParameters_.push_back(newLocation);
}

RegisterParts
Definition::outputRegisterParts() const {
    RegisterParts retval;
    BOOST_FOREACH (const ParameterLocation &loc, outputParameters_) {
        if (loc.type() == ParameterLocation::REGISTER)
            retval.insert(loc.reg());
    }
    return retval;
}

RegisterParts
Definition::inputRegisterParts() const {
    RegisterParts retval;
    BOOST_FOREACH (const ParameterLocation &loc, inputParameters_) {
        if (loc.type() == ParameterLocation::REGISTER)
            retval.insert(loc.reg());
    }
    return retval;
}

RegisterParts
Definition::scratchRegisterParts() const {
    RegisterParts retval;
    BOOST_FOREACH (RegisterDescriptor reg, scratchRegisters_)
        retval.insert(reg);
    return retval;
}

RegisterParts
Definition::calleeSavedRegisterParts() const {
    RegisterParts retval;
    BOOST_FOREACH (RegisterDescriptor reg, calleeSavedRegisters_)
        retval.insert(reg);
    return retval;
}

RegisterParts
Definition::getUsedRegisterParts() const {
    RegisterParts retval = inputRegisterParts();
    retval |= outputRegisterParts();
    if (!stackPointerRegister_.isEmpty())
        retval.insert(stackPointerRegister_);
    if (thisParameter_.type() == ParameterLocation::REGISTER)
        retval.insert(thisParameter_.reg());
    retval |= calleeSavedRegisterParts();
    retval |= scratchRegisterParts();
    return retval;
}

void
Definition::print(std::ostream &out, const RegisterDictionary *regDict/*=NULL*/) const {
    using namespace StringUtility;
    RegisterNames regNames(regDict ? regDict : regDict_);

    out <<cEscape(name_);
    if (!comment_.empty())
        out <<" (" <<cEscape(comment_) <<")";
    out <<" = {" <<wordWidth_ <<"-bit words";

    if (!inputParameters_.empty()) {
        out <<", inputs={";
        BOOST_FOREACH (const ParameterLocation &loc, inputParameters_) {
            out <<" ";
            loc.print(out, regNames);
        }
        out <<" }";
    }

    if (stackParameterOrder_ != ORDER_UNSPECIFIED) {
        out <<", implied={";
        switch (stackParameterOrder_) {
            case LEFT_TO_RIGHT: out <<" left-to-right"; break;
            case RIGHT_TO_LEFT: out <<" right-to-left"; break;
            case ORDER_UNSPECIFIED: ASSERT_not_reachable("invalid stack parameter order");
        }

        if (!stackPointerRegister_.isEmpty()) {
            out <<" " <<regNames(stackPointerRegister_) <<"-based stack";
        } else {
            out <<" NO-STACK-REGISTER";
        }

        switch (stackCleanup_) {
            case CLEANUP_BY_CALLER: out <<" cleaned up by caller"; break;
            case CLEANUP_BY_CALLEE: out <<" cleaned up by callee"; break;
            case CLEANUP_UNSPECIFIED: out <<" with UNSPECIFIED cleanup"; break;
        }
        out <<" }";
    }

    if (nonParameterStackSize_ > 0)
        out <<", " <<nonParameterStackSize_ <<"-byte return";

    if (stackParameterOrder_ != ORDER_UNSPECIFIED || nonParameterStackSize_ > 0) {
        switch (stackDirection_) {
            case GROWS_UP: out <<", upward-growing stack"; break;
            case GROWS_DOWN: out <<", downward-growing stack"; break;
        }
    }

    if (thisParameter_.isValid()) {
        out <<", this=";
        thisParameter_.print(out, regNames);
    }

    if (!outputParameters_.empty()) {
        out <<", outputs={";
        BOOST_FOREACH (const ParameterLocation &loc, outputParameters_) {
            out <<" ";
            loc.print(out, regNames);
        }
        out <<" }";
    }

    if (!scratchRegisters_.empty()) {
        out <<", scratch={";
        BOOST_FOREACH (RegisterDescriptor loc, scratchRegisters_)
            out <<" " <<regNames(loc);
        out <<" }";
    }

    if (!calleeSavedRegisters_.empty()) {
        out <<", saved={";
        BOOST_FOREACH (RegisterDescriptor loc, calleeSavedRegisters_)
            out <<" " <<regNames(loc);
        out <<" }";
    }
}

std::ostream&
operator<<(std::ostream &out, const Definition &x) {
    x.print(out);
    return out;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Analysis
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void
Analysis::init(Disassembler *disassembler) {
    if (disassembler) {
        const RegisterDictionary *registerDictionary = disassembler->registerDictionary();
        ASSERT_not_null(registerDictionary);
        size_t addrWidth = disassembler->instructionPointerRegister().nBits();

        SmtSolverPtr solver = SmtSolver::instance(Rose::CommandLine::genericSwitchArgs.smtSolver);
        SymbolicSemantics::RiscOperatorsPtr ops = SymbolicSemantics::RiscOperators::instance(registerDictionary, solver);

        cpu_ = disassembler->dispatcher()->create(ops, addrWidth, registerDictionary);
    }
}

void
Analysis::clearResults() {
    hasResults_ = didConverge_ = false;
    restoredRegisters_.clear();
    inputRegisters_.clear();
    outputRegisters_.clear();
    inputStackParameters_.clear();
    outputStackParameters_.clear();
    stackDelta_ = Sawyer::Nothing();
}

void
Analysis::clearNonResults() {
    cpu_ = DispatcherPtr();
}

void
Analysis::analyzeFunction(const P2::Partitioner &partitioner, const P2::Function::Ptr &function) {
    mlog[DEBUG] <<"analyzing " <<function->printableName() <<"\n";
    clearResults();

    // Build the CFG used by the dataflow: dfCfg.  The dfCfg includes only those vertices that are reachable from the entry
    // point for the function we're analyzing and which belong to that function.  All return points in the function will flow
    // into a special CALLRET vertex (which is absent if there are no returns).
    typedef P2::DataFlow::DfCfg DfCfg;
    DfCfg dfCfg = P2::DataFlow::buildDfCfg(partitioner, partitioner.cfg(), partitioner.findPlaceholder(function->address()));
#if 0 // DEBUGGING [Robb P Matzke 2017-02-24]
    {
        boost::filesystem::path debugDir = "./rose-debug/BinaryAnalysis/CallingConvention";
        boost::filesystem::create_directories(debugDir);
        boost::filesystem::path fileName = debugDir /
                                           ("F_" + StringUtility::addrToString(function->address()).substr(2) + ".dot");
        std::ofstream f(fileName.string().c_str());
        P2::DataFlow::dumpDfCfg(f, dfCfg);
    }
#endif
    size_t startVertexId = 0;
    DfCfg::ConstVertexIterator returnVertex = dfCfg.vertices().end();
    BOOST_FOREACH (const DfCfg::Vertex &vertex, dfCfg.vertices()) {
        if (vertex.value().type() == P2::DataFlow::DfCfgVertex::FUNCRET) {
            returnVertex = dfCfg.findVertex(vertex.id());
            break;
        }
    }
    if (returnVertex == dfCfg.vertices().end()) {
        mlog[DEBUG] <<"  function CFG has no return vertex\n";
        return;
    }

    // Build the dataflow engine.  If an instruction dispatcher is already provided then use it, otherwise create one and store
    // it in this analysis object.
    typedef DataFlow::Engine<DfCfg, StatePtr, P2::DataFlow::TransferFunction, DataFlow::SemanticsMerge> DfEngine;
    if (!cpu_ && NULL==(cpu_ = partitioner.newDispatcher(partitioner.newOperators()))) {
        mlog[DEBUG] <<"  no instruction semantics\n";
        return;
    }
    P2::DataFlow::MergeFunction merge(cpu_);
    P2::DataFlow::TransferFunction xfer(cpu_);
    xfer.defaultCallingConvention(defaultCc_);
    DfEngine dfEngine(dfCfg, xfer, merge);
    size_t maxIterations = dfCfg.nVertices() * 5;       // arbitrary
    dfEngine.maxIterations(maxIterations);
    regDict_ = cpu_->get_register_dictionary();

    // Build the initial state
    StatePtr initialState = xfer.initialState();
    RegisterStateGenericPtr initialRegState = RegisterStateGeneric::promote(initialState->registerState());
    initialRegState->initialize_large();
    const RegisterDescriptor SP = partitioner.instructionProvider().stackPointerRegister();
    rose_addr_t initialStackPointer = 0xcf000000;       // arbitrary
    initialRegState->writeRegister(SP, cpu_->get_operators()->number_(SP.nBits(), initialStackPointer),
                                   cpu_->get_operators().get());

    // Run data flow analysis
    bool converged = true;
    try {
        // Use this rather than runToFixedPoint because it lets us show a progress report
        Sawyer::ProgressBar<size_t> progress(maxIterations, mlog[MARCH], function->printableName());
        progress.suffix(" iterations");
        dfEngine.reset(StatePtr());
        dfEngine.insertStartingVertex(startVertexId, initialState);
        while (dfEngine.runOneIteration())
            ++progress;
    } catch (const DataFlow::NotConverging &e) {
        mlog[WARN] <<e.what() <<" for " <<function->printableName() <<"\n";
        converged = false;                              // didn't converge, so just use what we have
    } catch (const BaseSemantics::Exception &e) {
        mlog[WARN] <<e.what() <<" for " <<function->printableName() <<"\n";
        converged = false;
    }

    // Get the final dataflow state
    StatePtr finalState = dfEngine.getInitialState(returnVertex->id());
    if (finalState == NULL) {
        mlog[DEBUG] <<"  data flow analysis did not reach final state\n";
        return;
    }
    if (mlog[DEBUG]) {
        if (!converged) {
            mlog[DEBUG] <<"  data flow analysis did not converge to a solution (using partial solution)\n";
        } else {
            SymbolicSemantics::Formatter fmt;
            fmt.set_line_prefix("    ");
            fmt.expr_formatter.max_depth = 10;          // prevent really long output
            mlog[DEBUG] <<"  final state:\n" <<(*finalState+fmt);
        }
    }
    RegisterStateGenericPtr finalRegs = RegisterStateGeneric::promote(finalState->registerState());

    // Update analysis results
    updateRestoredRegisters(initialState, finalState);
    updateInputRegisters(finalState);
    updateOutputRegisters(finalState);
    updateStackParameters(function, initialState, finalState);
    updateStackDelta(initialState, finalState);
    hasResults_ = true;
    didConverge_ = converged;

    SAWYER_MESG(mlog[DEBUG]) <<"  analysis results for " <<function->printableName() <<": " <<*this <<"\n";
}

void
Analysis::updateRestoredRegisters(const StatePtr &initialState, const StatePtr &finalState) {
    restoredRegisters_.clear();

    RegisterStateGenericPtr initialRegs = RegisterStateGeneric::promote(initialState->registerState());
    RegisterStateGenericPtr finalRegs = RegisterStateGeneric::promote(finalState->registerState());
    ASSERT_not_null2(cpu_, "analyzer is not properly initialized");
    RiscOperatorsPtr ops = cpu_->get_operators();

    InputOutputPropertySet props;
    props.insert(IO_READ_BEFORE_WRITE);
    props.insert(IO_WRITE);
    BOOST_FOREACH (RegisterDescriptor reg, finalRegs->findProperties(props)) {
        SValuePtr initialValue = initialRegs->peekRegister(reg, ops->undefined_(reg.nBits()), ops.get());
        SValuePtr finalValue = finalRegs->peekRegister(reg, ops->undefined_(reg.nBits()), ops.get());
        SymbolicExpr::Ptr initialExpr = SymbolicSemantics::SValue::promote(initialValue)->get_expression();
        SymbolicExpr::Ptr finalExpr = SymbolicSemantics::SValue::promote(finalValue)->get_expression();
        if (finalExpr->flags() == initialExpr->flags() && finalExpr->mustEqual(initialExpr, ops->solver()))
            restoredRegisters_.insert(reg);
    }
}

void
Analysis::updateInputRegisters(const StatePtr &state) {
    inputRegisters_.clear();
    RegisterStateGenericPtr regs = RegisterStateGeneric::promote(state->registerState());
    BOOST_FOREACH (RegisterDescriptor reg, regs->findProperties(IO_READ_BEFORE_WRITE))
        inputRegisters_.insert(reg);
    inputRegisters_ -= restoredRegisters_;
}

void
Analysis::updateOutputRegisters(const StatePtr &state) {
    outputRegisters_.clear();
    RegisterStateGenericPtr regs = RegisterStateGeneric::promote(state->registerState());
    BOOST_FOREACH (RegisterDescriptor reg, regs->findProperties(IO_WRITE))
        outputRegisters_.insert(reg);
    outputRegisters_ -= restoredRegisters_;
}

void
Analysis::updateStackParameters(const P2::Function::Ptr &function, const StatePtr &initialState, const StatePtr &finalState) {
    inputStackParameters_.clear();
    outputStackParameters_.clear();

    ASSERT_not_null2(cpu_, "analyzer is not properly initialized");
    RiscOperatorsPtr ops = cpu_->get_operators();
    RegisterDescriptor SP = cpu_->stackPointerRegister();
    SValuePtr initialStackPointer = initialState->peekRegister(SP, ops->undefined_(SP.nBits()), ops.get());
    ops->currentState(finalState);
    Variables::StackVariables vars = P2::DataFlow::findFunctionArguments(function, ops, initialStackPointer);
    BOOST_FOREACH (const Variables::StackVariable &var, vars.values()) {
        if (var.ioProperties().exists(IO_READ_BEFORE_WRITE)) {
            inputStackParameters_.insert(var.interval(), var);
        } else if (var.ioProperties().exists(IO_WRITE) && var.ioProperties().exists(IO_READ_AFTER_WRITE)) {
            outputStackParameters_.insert(var.interval(), var);
        }
    }
}

void
Analysis::updateStackDelta(const StatePtr &initialState, const StatePtr &finalState) {
    ASSERT_not_null2(cpu_, "analyzer is not properly initialized");
    RiscOperatorsPtr ops = cpu_->get_operators();
    RegisterDescriptor SP = cpu_->stackPointerRegister();
    SValuePtr initialStackPointer = initialState->peekRegister(SP, ops->undefined_(SP.nBits()), ops.get());
    SValuePtr finalStackPointer = finalState->peekRegister(SP, ops->undefined_(SP.nBits()), ops.get());
    SValuePtr stackDelta = ops->subtract(finalStackPointer, initialStackPointer);
    if (stackDelta->is_number() && stackDelta->get_width()<=64) {
        stackDelta_ = IntegerOps::signExtend2(stackDelta->get_number(), stackDelta->get_width(), 64);
    } else {
        stackDelta_ = Sawyer::Nothing();
    }
}

void
Analysis::print(std::ostream &out, bool multiLine) const {
    RegisterNames regName(regDict_);
    std::string separator;

    if (!inputRegisters_.isEmpty() || !inputStackParameters_.isEmpty()) {
        out <<separator <<"inputs={";
        if (!inputRegisters_.isEmpty()) {
            BOOST_FOREACH (RegisterDescriptor reg, inputRegisters_.listAll(regDict_))
                out <<" " <<regName(reg);
        }
        if (!inputStackParameters_.isEmpty()) {
            Variables::StackVariables vars = inputStackParameters();
            BOOST_FOREACH (const Variables::StackVariable &var, vars.values())
                out <<" stack[" <<var.frameOffset() <<"]+" <<var.maxSizeBytes();
        }
        out <<" }";
        separator = multiLine ? "\n" : ", ";
    }

    if (!outputRegisters_.isEmpty() || !outputStackParameters_.isEmpty()) {
        out <<separator <<"outputs={";
        if (!outputRegisters_.isEmpty()) {
            BOOST_FOREACH (RegisterDescriptor reg, outputRegisters_.listAll(regDict_))
                out <<" " <<regName(reg);
        }
        if (!outputStackParameters_.isEmpty()) {
            Variables::StackVariables vars = outputStackParameters();
            BOOST_FOREACH (const Variables::StackVariable &var, vars.values())
                out <<" stack[" <<var.frameOffset() <<"]+" <<var.maxSizeBytes();
        }
        out <<" }";
        separator = multiLine ? "\n" : ", ";
    }

    if (!restoredRegisters_.isEmpty()) {
        out <<separator <<"saved={";
        BOOST_FOREACH (RegisterDescriptor reg, restoredRegisters_.listAll(regDict_))
            out <<" " <<regName(reg);
        out <<" }";
        separator = multiLine ? "\n" : ", ";
    }

    if (stackDelta_) {
        out <<separator <<"stackDelta=" <<(*stackDelta_>=0?"+":"") <<*stackDelta_;
        separator = multiLine ? "\n" : ", ";
    }

    if (separator.empty())
        out <<"no I/O";
}

bool
Analysis::match(const Definition::Ptr &cc) const {
    ASSERT_not_null2(cpu_, "analyzer is not properly initialized");
    ASSERT_not_null(cc);

    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"matching calling convention definition to analysis\n"
                       <<"  definition: " <<*cc <<"\n"
                       <<"  analysis results: " <<*this <<"\n";

    if (!hasResults_) {
        SAWYER_MESG(debug) <<"  mismatch: no analysis results\n";
        return false;
    }

    if (cc->wordWidth() != cpu_->stackPointerRegister().nBits()) {
        SAWYER_MESG(debug) <<"  mismatch: defn word size (" <<cc->wordWidth() <<") != analysis word size ("
                           <<cpu_->stackPointerRegister().nBits() <<")\n";
        return false;
    }

    // Gather up definition's input registers. We always add EIP (or similar) because the analysis will have read it to obtain
    // the function's first instruction before ever writing to it.  Similarly, we add ESP (or similar) because pushing,
    // popping, aligning, and allocating local variable space all read ESP before writing to it.
    RegisterParts ccInputRegisters = cc->inputRegisterParts();
    ccInputRegisters.insert(cpu_->instructionPointerRegister());
    ccInputRegisters.insert(cpu_->stackPointerRegister());
    if (cc->thisParameter().type() == ParameterLocation::REGISTER)
        ccInputRegisters.insert(cc->thisParameter().reg());

    // Gather up definition's output registers.  We always add EIP (or similar) because the final RET instruction will write
    // the return address into the EIP register and not subsequently read it. The stack pointer register is not added by
    // default because not all functions use the stack (e.g., architectures that have link registers); it must be added (or
    // not) when the definition is created.
    RegisterParts ccOutputRegisters = cc->outputRegisterParts() | cc->scratchRegisterParts();
    ccOutputRegisters.insert(cpu_->instructionPointerRegister());

    // Stack delta checks
    if (stackDelta_) {
        int64_t normalization = (cc->stackDirection() == GROWS_UP ? -1 : +1);
        int64_t normalizedStackDelta = *stackDelta_ * normalization; // in bytes

        // All callees must pop the non-parameter area (e.g., return address) of the stack.
        if (normalizedStackDelta < 0 || (uint64_t)normalizedStackDelta < cc->nonParameterStackSize()) {
            SAWYER_MESG(debug) <<"  mismatch: callee did not pop " <<cc->nonParameterStackSize() <<"-byte"
                               <<" non-parameter area from stack\n";
            return false;
        }
        normalizedStackDelta -= cc->nonParameterStackSize();

        // The callee must not pop stack parameters if the caller cleans them up.
        if (cc->stackCleanup() == CLEANUP_BY_CALLER && normalizedStackDelta != 0) {
            SAWYER_MESG(debug) <<"  mismatch: callee popped stack parameters but definition is caller-cleanup\n";
            return false;
        }

        // For callee cleanup, the callee must pop all the stack variables. It may pop more than what it used (i.e., it must
        // pop even unused arguments).
        if (cc->stackCleanup() == CLEANUP_BY_CALLEE) {
            int64_t normalizedEnd = 0; // one-past first-pushed argument normlized for downward-growing stack
            BOOST_FOREACH (const Variables::StackVariable &var, inputStackParameters_.values())
                normalizedEnd = std::max(normalizedEnd, (int64_t)(var.frameOffset() * normalization + var.maxSizeBytes()));
            BOOST_FOREACH (const Variables::StackVariable &var, outputStackParameters_.values())
                normalizedEnd = std::max(normalizedEnd, (int64_t)(var.frameOffset() * normalization + var.maxSizeBytes()));
            if (normalizedStackDelta < normalizedEnd) {
                SAWYER_MESG(debug) <<"  mismatch: callee failed to pop callee-cleanup stack parameters\n";
                return false;
            }
        }
    }

    // All analysis output registers must be a definition's output or scratch register.
    if (!(outputRegisters_ - ccOutputRegisters).isEmpty()) {
        if (debug) {
            RegisterNames regName(registerDictionary());
            debug <<"  mismatch: actual outputs are not defined outputs or scratch registers: ";
            RegisterParts parts = outputRegisters_ - ccOutputRegisters;
            BOOST_FOREACH (RegisterDescriptor reg, parts.listAll(registerDictionary()))
                debug <<" " <<regName(reg);
            debug <<"\n";
        }
        return false;
    }

    // All analysis input registers must be a definition's input or "this" register.
    if (!(inputRegisters_ - ccInputRegisters).isEmpty()) {
        if (debug) {
            RegisterNames regName(registerDictionary());
            debug <<"  mismatch: actual inputs are not defined inputs or \"this\" register: ";
            RegisterParts parts = inputRegisters_ - ccInputRegisters;
            BOOST_FOREACH (RegisterDescriptor reg, parts.listAll(registerDictionary()))
                debug <<" " <<regName(reg);
            debug <<"\n";
        }
        return false;
    }

    // All analysis restored registers must be a definition's callee-saved register.
    if (!(restoredRegisters_ - cc->calleeSavedRegisterParts()).isEmpty()) {
        if (debug) {
            debug <<"  mismatch: restored registers that are not defined as callee-saved:";
            RegisterParts parts = restoredRegisters_ - cc->calleeSavedRegisterParts();
            RegisterNames regName(registerDictionary());
            BOOST_FOREACH (RegisterDescriptor reg, parts.listAll(registerDictionary()))
                debug <<" " <<regName(reg);
            debug <<"\n";
        }
        return false;
    }

    // If the definition has an object pointer ("this" parameter) then it should not be an anlysis output or scratch register,
    // but must be an analysis input register.
    if (cc->thisParameter().type() == ParameterLocation::REGISTER) {
        if (ccOutputRegisters.existsAny(cc->thisParameter().reg())) {
            SAWYER_MESG(debug) <<"  mismatch: actual output defined as \"this\" register: "
                               <<RegisterNames(registerDictionary())(cc->thisParameter().reg()) <<"\n";
            return false;
        }
        if (!ccInputRegisters.existsAll(cc->thisParameter().reg())) {
            SAWYER_MESG(debug) <<"  mismatch: actual input does not include \"this\" register: "
                               <<RegisterNames(registerDictionary())(cc->thisParameter().reg()) <<"\n";
            return false;
        }
    }

    // If the analysis has stack inputs or outputs then the definition must have a valid stack parameter direction.
    if ((!inputStackParameters().isEmpty() || !outputStackParameters().isEmpty()) &&
        cc->stackParameterOrder() == ORDER_UNSPECIFIED) {
        SAWYER_MESG(debug) <<"  mismatch: stack parameters detected but not allowed by definition\n";
        return false;
    }

    SAWYER_MESG(debug) <<"  analysis matches definition\n";
    return true;
}

Dictionary
Analysis::match(const Dictionary &conventions) const {
    Dictionary retval;
    BOOST_FOREACH (const Definition::Ptr &cc, conventions) {
        if (match(cc))
            retval.push_back(cc);
    }
    return retval;
}

std::ostream&
operator<<(std::ostream &out, const Analysis &x) {
    x.print(out);
    return out;
}

} // namespace
} // namespace
} // namespace
