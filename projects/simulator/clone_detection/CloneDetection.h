// This header has the high-level clone-detection semantics functions.  One does not typically need a deep understanding of the
// simulator in order to modify this file (Andreas and Earl).
//
// Don't forget to apply the clone_detection/CloneDetection.patch before compiling, or else none of this stuff ever gets
// called!  These functions are mostly called from clone_detection/CloneDetectionTpl.h, but I'm splitting them into this file
// so you guys can find the parts you need to modify.

#include "rose_getline.h"

namespace CloneDetection {
namespace HighLevel {

/** Convert a value to the concrete domain.
 *
 *  Converts @p value to the concrete domain.  This is only called if @p value is not already defined in the concrete domain
 *  and can't be trivially converted to the concrete domain from one of the other domains. */
template <template <size_t> class ValueType, size_t nBits>
CONCRETE_VALUE<nBits>
convert_to_concrete(const ValueType<nBits> &value)
{
    // FIXME: for now, return a random value with the specified number of bits.
    return CONCRETE_VALUE<nBits>(rand());
}


/** Convert a value to the interval domain.
 *
 *  Converts @p value to the interval domain.  This is only called if @p value is not already defined in the interval domain
 *  and can't be trivially converted to the interval domain from one of the other domains. */
template <template <size_t> class ValueType, size_t nBits>
INTERVAL_VALUE<nBits>
convert_to_interval(const ValueType<nBits> &value)
{
    // FIXME: for now, return an interval that covers the entire domain
    return INTERVAL_VALUE<nBits>();
}


    
/** Convert a value to the symbolic domain.
 *
 *  Converts @p value to the symbolic domain.  This is only called if @p value is not already defined in the symbolic domain
 *  and can't be trivially converted to the symbolic domain from one of the other domains. */
template <template <size_t> class ValueType, size_t nBits>
SYMBOLIC_VALUE<nBits>
convert_to_symbolic(const ValueType<nBits> &value)
{
    // FIXME: for now, return an unconstrained value.
    return SYMBOLIC_VALUE<nBits>();
}

/** Return the next input value from the queue of possible inputs. */
template <size_t nBits>
RSIM_SEMANTICS_VTYPE<nBits>
next_input_value(InputValues *inputs, InputValues::Type type, RTS_Message *m)
{
    uint64_t value = 0;
    size_t nvalues = 0;
    const char *type_name = NULL;
    switch (type) {
        case InputValues::POINTER:
            value = inputs->next_pointer();
            nvalues = inputs->pointers_consumed();
            type_name = "pointer";
            break;
        case InputValues::UNKNOWN_TYPE:
        case InputValues::NONPOINTER:
            value = inputs->next_integer();
            nvalues = inputs->integers_consumed();
            type_name = "non-pointer";
            break;
    }
    
    RSIM_SEMANTICS_VTYPE<nBits> retval(value);
    if (m) {
        std::ostringstream ss;
        ss <<retval;
        m->mesg("CloneDetection::HighLevel: using %s input #%zu: %s", type_name, nvalues, ss.str().c_str());
    }
    return retval;
}

/** Called once when analysis starts. This is called at the start of every analyzed function. The @p target_va is the virtual
 *  address for the function's entry point. Registers and memory have already been (partly) initialized by this time. */
template<class Policy>
void
analysis_starting(Policy *policy, rose_addr_t target_va)
{
}

/** Determines domains in which an x86 instruction executes.
 *
 *  Given a mask of currently active domains and an x86 instruction, return the domain(s) in which the instruction should
 *  execute. The @p active argument and return value are bit mask whose bits are: CONCRETE.mask, INTERVAL.mask, and/or
 *  SYMBOLIC.mask. For the first instruction, @p active is all domains; for subsequent instructions it is the value that this
 *  function previously returned.
 *
 *  The @p policy is available if you need access to the current machine state. */
template<class Policy>
unsigned
domains_for_instruction(Policy *policy, SgAsmx86Instruction *insn)
{
    unsigned retval = 0;

    if (true) {
        return CONCRETE.mask;

    } else if (true) {
        // Use a file to determine when to switch from one mode to another.  Each line of the file is an instruction address
        // followed by white space and an integer. The integer is the mask value returned by this function.  Each time we reach
        // an instruction whose address is specified by the current line of the file, we consume that line and advance to the
        // next one.
        static FILE *f = NULL;
        if (!f && NULL==(f = fopen("mixed-modes", "r"))) {
            perror("mixed-modes");
            assert(!"here");
            abort();
        }
        static char *buf = NULL;
        static size_t bufsz = 0;
        static rose_addr_t va = 0;
        static unsigned mask;
        if (!va && rose_getline(&buf, &bufsz, f) > 0) {
            char *rest;
            va = strtoull(buf, &rest, 0);
            mask = strtol(rest, NULL, 0);
        }
        if (insn->get_address()==va)
            va = 0; // force reading of next file line on next call
        return mask;

    } else {
        // You can enable all policies at once if you like.
        retval = CONCRETE.mask | INTERVAL.mask | SYMBOLIC.mask;

        // You can turn off the symbolic domain if you think it's getting too complex.
        if (0!=(retval & SYMBOLIC.mask)) {
            size_t sym_cplx = policy->symbolic_state_complexity();
            if (sym_cplx > 10000)
                retval &= ~SYMBOLIC.mask;
        }

        // You can set the policy based on the kind of x86 instruction.
        if (x86_int == insn->get_kind()) {
            retval = CONCRETE.mask;
        } else if (x86InstructionIsConditionalFlagControlTransfer(insn)) {
            retval = SYMBOLIC.mask;
        }

        // You can hard-code to use a sigle policy, such as when you're debugging
        retval = INTERVAL.mask;

        // You can get access to the thread that's executing this instruction.  This info is stored in the CONCRETE domain, which
        // is the same domain that the normal simulator uses and thus has all the normal simulator's data.  This is also how you
        // can gain access to all the simulator callback lists (which are still executing in the CONCRETE domain).
        RSIM_Thread *thread = policy->get_policy(CONCRETE).thread;
        RSIM_Process *proc = thread->get_process();
        policy->trace()->mesg("This is CloneDetection thread %d of %s\n", thread->get_tid(), proc->get_exename().c_str());


        // You can also use this function to do other per-instruction work, like showing how much the SMT solver's been used.
        if (true) {
            static size_t ncalls = 0;
            static const size_t report_interval = 100;
            if (0==++ncalls % report_interval) {
                SMTSolver *solver = policy->get_policy(SYMBOLIC).get_solver();
                if (solver) {
                    const SMTSolver::Stats &stats = solver->get_stats();
                    if (stats.ncalls>0) {
                        policy->trace()->mesg("SMT solver stats since last report: ncalls=%zu, input=%zu bytes, output=%zu bytes",
                                              stats.ncalls, stats.input_size, stats.output_size);
                        solver->reset_stats();
                    }
                }
            }
        }
    }
    return retval;
}

/** Handles certain if-then-else operations.
 *
 *  This function is called to obtain the result for all if-then-else RISC operations where the caller determined that the
 *  condition could be either true and false.  Normally, if the condition is either true or false but not both, the result
 *  would be either @p a or @p b; this function is to handle the unknown case.  The @p cond is the Boolean condition and is
 *  defined in the symbolic domain (among others). */
template<class Policy, template <size_t nBits> class ValueType, size_t nBits>
ValueType<nBits>
ite_merge(Policy *policy, const ValueType<1> &cond, const ValueType<nBits> &a, const ValueType<nBits> &b)
{
    if (true) {
        // Here's how you might choose one or the other at random.  We use our own linear congruential generator so we get
        // reproducible results.
        static unsigned long long x=0;
        x = (25214903917ull*x+11) % 0x0000ffffffffffffull;  // 48 bits
        bool bit = 0 != (x &        0x0000010000000000ull); // arbitrary bit; higher order bits have longer periods
        std::ostringstream ss;
        ss <<"CloneDetection::HighLevel: ite_merge: choosing " <<(bit?"first":"second") <<" alternative: " <<(bit?a:b);
        policy->trace()->mesg("%s", ss.str().c_str());
        return bit ? a : b;

    } else if (true) {
        // Here's how you might make the decision from a file.  The file contains lines where each line is either zero or
        // non-zero to indicate whether the condition should be considered to be false or true, respectively.  Each time we hit
        // this decision a line is consumed from the file.
        static FILE *f = NULL;
        if (!f) {
            f = fopen("branch-predictions", "r");
            if (!f) {
                perror("branch-predictions");
                assert(!"here");
                abort();
            }
        }
        static char *buf = NULL;
        static size_t bufsz = 0;
        bool take = false;
        if (rose_getline(&buf, &bufsz, f) > 0)
            take = strtol(buf, NULL, 0);

        std::ostringstream ss;
        ss <<"CloneDetection::HighLevel: ite_merge: choosing " <<(take?"first":"second") <<" alternative: " <<(take?a:b);
        policy->trace()->mesg("%s", ss.str().c_str());
        return take ? a : b;

    } else {
        // Here's how you would do the default thing.
        return policy->Policy::Super::ite(cond, a, b);
    }
}

/** Gets called after every x86 instruction.
 *
 *  Allows the HighLevel to do things after an instruction is executed.  The machine state is available through the CONCRETE
 *  component of the multi domain policy passed in as the @p policy argument.  The @p insn argument is the instruction that was
 *  just executed. */
template<class Policy>
void
after_instruction(Policy *policy, SgAsmx86Instruction *insn)
{
#if 0 /*DEBUGGING [Robb Matzke 2013-01-14]*/
    // Print the machine state
    std::ostringstream ss;
    bool abbreviated = false; // if true, then show only the active domains
    policy->print(ss, abbreviated);
    policy->trace()->mesg("%s: complete state after %s\n%s",
                          policy->name, unparseInstruction(insn).c_str(),
                          StringUtility::prefixLines(ss.str(), "    ").c_str());
    
#endif
}

} // namespace
} // namespace
