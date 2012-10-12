// Template definitions for Andreas and Earl
// Don't forget to apply the demos/MultiWithConversion.patch before compiling, or else none of this stuff ever gets called!
// These functions are called from demos/MultiWithConversionTpl.h, but I'm splitting them into this file so you guys can find
// the parts you need to modify.

namespace Robin {

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
    using namespace MultiDomainDemo; // where things like SYMBOLIC are defined. See MultiWithConversion.h
    unsigned retval = 0;

    if (true) {
        /* DEBUGGING, but still compile the real stuff [RPM 2012-10-12] */
        static size_t ncalls = 0;
        switch (ncalls++ % 3) {
            case 0: retval = INTERVAL.mask; break;
            case 1: retval = CONCRETE.mask; break;
            case 2: retval = SYMBOLIC.mask; break;
        }
    } else {
        // You can enable all policies at once if you like.
        retval = CONCRETE.mask | INTERVAL.mask | SYMBOLIC.mask;

        // You can use the same policies as for the previous instruction.
        retval = policy->get_active_policies();

        // You can turn off the symbolic domain if you think it's getting too complex.
        if (0!=(retval & SYMBOLIC.mask)) {
            size_t sym_cplx = policy->symbolic_state_complexity();
            if (sym_cplx > 10000)
                retval &= ~SYMBOLIC.mask;
        }

        // You can set the policy based on the kind of x86 instruction.
        if (x86_int == insn->get_kind())
            retval = CONCRETE.mask;

        // You can hard-code to use a sigle policy, such as when you're debugging
        retval = INTERVAL.mask;

        // You can get access to the thread that's executing this instruction.  This info is stored in the CONCRETE domain, which
        // is the same domain that the normal simulator uses and thus has all the normal simulator's data.  This is also how you
        // can gain access to all the simulator callback lists (which are still executing in the CONCRETE domain).
        RSIM_Thread *thread = policy->get_policy(CONCRETE).thread;
        RSIM_Process *proc = thread->get_process();
        policy->trace()->mesg("This is Robin: thread %d of %s\n", thread->get_tid(), proc->get_exename().c_str());


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



/** Gets called after every x86 instruction.
 *
 *  Allows Robin to do things after an instruction is executed.  The machine state is available through the CONCRETE component
 *  of the multi domain policy passed in as the @p policy argument.  The @p insn argument is the instruction that was just
 *  executed. */
template<class Policy>
void
after_instruction(Policy *policy, SgAsmx86Instruction *insn)
{
    using namespace MultiDomainDemo; // where things like SYMBOLIC are defined. See MultiWithConversion.h

    // Print the machine state
    std::ostringstream ss;
    bool abbreviated = false; // if true, then show only the active domains
    policy->print(ss, abbreviated);
    policy->trace()->mesg("%s: complete state after %s\n%s",
                          policy->name, unparseInstruction(insn).c_str(),
                          StringUtility::prefixLines(ss.str(), "    ").c_str());
}
    

    
    
} // namespace
