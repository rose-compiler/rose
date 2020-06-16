/* Demonstrates how to perform symbolic analysis on a function that isn't called.  Uses grep_sample_1.c as input and follows a
 * predetermined control flow path, adding constraints to the function input variables in the process.
 *
 * Documentation can be found by running:
 *    $ doxygen RSIM.dxy
 *    $ $WEB_BROWSER docs/html/index.html
 *
 *
 * This demo does the following:
 *
 * 1. Allow the specimen to execute up to a certain point in order to resolve dynamic linking.  This demo parses the ELF file
 *    to find the address of "main" and stops when it is reached.  By executing to main(), we allow the dynamic linker to run,
 *    giving us more information about the executable.
 *
 * 2. When main is reached, we will do some symbolic analysis on the "guess_type" function.  This function takes a pointer to a
 *    buffer and the number of bytes in the buffer and returns an enum constant describing line termination.
 *
 * 3. A symbolic analysis is performed.  Whenever the analysis reaches a conditional branch where the condition depends on an
 *    unknown value, it consults a vector of user-supplied Booleans to indicate whether the branch should be taken or fall
 *    through.  Optionally, we invoke the SMT solver to see if the user's choice is even possible.  In any case, the choice
 *    made by the user adds constraints to the final solutions.
 *
 * 4. When the function returns, obtain an expression for the function's output in terms of its unknown inputs.
 *
 * 5. Choose values for the unknown inputs and evaluate the function's symbolic expression.  We simply choose 120 (ASCII 'x')
 *    for each unknown.  If the expression has no unknowns, then skip this step -- the function returns a constant value.
 *
 * 6. Choose a value for the function return and invoke the SMT solver to see if the expression is satisfiable for some
 *    comination of concrete values for inputs.  If so, print a set of input values that would give the desired output.  This
 *    step is skipped if the function's expression is constant.
 */

#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR /* protects this whole file */

#include "RSIM_Linux32.h"
#include "BinaryYicesSolver.h"
#include "SymbolicSemantics.h"

// Monitors the CPU instruction pointer.  When it reaches a specified value analyze the function at the specified location.
class Analysis: public RSIM_Callbacks::InsnCallback {
public:
    rose_addr_t trigger_addr, analysis_addr;
    std::vector<bool> take_branch;                      // whether abstract branches should be taken or not taken

    Analysis(rose_addr_t trigger_addr, rose_addr_t analysis_addr, const std::vector<bool> &take_branch)
        : trigger_addr(trigger_addr), analysis_addr(analysis_addr), take_branch(take_branch) {}

    // This analysis is intended to run in a single thread, so clone is a no-op.
    virtual Analysis *clone() { return this; }

    // The actual analysis, triggered when we reach the specified execution address...
    virtual bool operator()(bool enabled, const Args &args) try {
        using namespace Rose::BinaryAnalysis::InstructionSemantics;

        static const char *name = "Analysis";
        using namespace Rose::BinaryAnalysis::SymbolicExpr;
        if (enabled && args.insn->get_address()==trigger_addr) {
            RTS_Message *trace = args.thread->tracing(TRACE_MISC);
            trace->mesg("%s triggered: analyzing function at 0x%08"PRIx64, name, analysis_addr);

            // An SMT solver is necessary for this example to work correctly. ROSE should have been configured with
            // "--with-yices=/full/path/to/yices/installation".  If not, you'll get a failed assertion when ROSE tries to use
            // the solver.
            Rose::BinaryAnalysis::YicesSolver smt_solver(Rose::BinaryAnalysis::YicesSolver::LM_EXECUTABLE);

            // We deactive the simulator while we're doing this analysis.  If the simulator remains activated, then the SIGCHLD
            // that are generated from running the Yices executable will be sent to the specimen.  That probably wouldn't cause
            // problems for the specimen, but the messages are annoying.
            args.thread->get_process()->get_simulator()->deactivate();

            // Create the policy that holds the analysis state which is modified by each instruction.  Then plug the policy
            // into the X86InstructionSemantics to which we'll feed each instruction.
            SymbolicSemantics::Policy<SymbolicSemantics::State, SymbolicSemantics::ValueType> policy(&smt_solver);
            X86InstructionSemantics<SymbolicSemantics::Policy<SymbolicSemantics::State, SymbolicSemantics::ValueType>,
                                    SymbolicSemantics::ValueType> semantics(policy);

            // The top of the stack contains the (unknown) return address.  The value above that (in memory) is the address of
            // the buffer, to which we give a concrete value, and above that is the size of the buffer, which we also give a
            // concrete value).  The contents of the buffer are unknown.  Process memory is maintained by the policy we created
            // above, so none of these memory writes are actually affecting the specimen's state in the simulator.
            policy.writeRegister("esp", policy.number<32>(4000));
            SymbolicSemantics::ValueType<32> arg1_va = policy.add(policy.readRegister<32>("esp"), policy.number<32>(4));
            SymbolicSemantics::ValueType<32> arg2_va = policy.add(arg1_va, policy.number<32>(4));
            policy.writeMemory<32>(x86_segreg_ss, arg1_va, policy.number<32>(12345), policy.true_());   // ptr to buffer
            policy.writeMemory<32>(x86_segreg_ss, arg2_va, policy.number<32>(2), policy.true_());       // bytes in buffer
            policy.writeRegister("eip", SymbolicSemantics::ValueType<32>(analysis_addr));            // branch to analysis address

#if 1
            {
                // This is a kludge.  If the first instruction is an indirect JMP then assume we're executing through a dynamic
                // linker thunk and execute the instruction concretely to advance the instruction pointer.
                SgAsmX86Instruction *insn = isSgAsmX86Instruction(args.thread->get_process()->get_instruction(analysis_addr));
                if (x86_jmp==insn->get_kind()) {
                    PartialSymbolicSemantics::Policy<PartialSymbolicSemantics::State, PartialSymbolicSemantics::ValueType> p;
                    X86InstructionSemantics<PartialSymbolicSemantics::Policy<PartialSymbolicSemantics::State,
                                                                             PartialSymbolicSemantics::ValueType>,
                                            PartialSymbolicSemantics::ValueType> sem(p);
                    MemoryMap p_map = args.thread->get_process()->get_memory();
                    BOOST_FOREACH (MemoryMap::Segment &segment, p_map.segments())
                        segment.buffer()->copyOnWrite(true);
                    p.set_map(&p_map); // won't be thread safe
                    sem.processInstruction(insn);
                    policy.writeRegister("eip", SymbolicSemantics::ValueType<32>(p.readRegister<32>("eip").known_value()));
                    trace->mesg("%s: dynamic linker thunk kludge triggered: changed eip from 0x%08"PRIx64" to 0x%08"PRIx64,
                                name, analysis_addr, p.readRegister<32>("eip").known_value());
                }
            }
#endif

            // Run the analysis until we can't figure out what instruction is next.  If we set things up correctly, the
            // simulation will stop when we hit the RET instruction to return from this function.
            size_t nbranches = 0;
            std::vector<TreeNodePtr> constraints; // path constraints for the SMT solver
            while (policy.readRegister<32>("eip").is_known()) {
                uint64_t va = policy.readRegister<32>("eip").known_value();
                SgAsmX86Instruction *insn = isSgAsmX86Instruction(args.thread->get_process()->get_instruction(va));
                assert(insn!=NULL);
                trace->mesg("%s: analysing instruction %s", name, unparseInstructionWithAddress(insn).c_str());
                semantics.processInstruction(insn);
                if (policy.readRegister<32>("eip").is_known())
                    continue;
                
                bool complete;
                std::set<rose_addr_t> succs = insn->getSuccessors(complete/*out*/);
                if (complete && 2==succs.size()) {
                    if (nbranches>=take_branch.size()) {
                        std::ostringstream s; s<<policy.readRegister<32>("eip");
                        trace->mesg("%s: EIP = %s", name, s.str().c_str());
                        trace->mesg("%s: analysis cannot continue; out of \"take_branch\" values", name);
                        throw this;
                    }

                    // Decide whether we should take the branch or not.
                    bool take = take_branch[nbranches++];
                    rose_addr_t target = 0;
                    for (std::set<rose_addr_t>::iterator si=succs.begin(); si!=succs.end(); ++si) {
                        if ((take && *si!=insn->get_address()+insn->get_size()) ||
                            (!take && *si==insn->get_address()+insn->get_size()))
                            target = *si;
                    }
                    assert(target!=0);
                    trace->mesg("%s: branch %staken; target=0x%08"PRIx64, name, take?"":"not ", target);

                    // Is this path feasible?  We don't really need to check it now; we could wait until the end.
                    TreeNodePtr c = InternalNode::create(32, OP_EQ, policy.readRegister<32>("eip").get_expression(),
                                                         LeafNode::create_integer(32, target));
                    constraints.push_back(c); // shouldn't really have to do this again if we could save some state
                    if (Rose::BinaryAnalysis::SmtSolver::SAT_YES == smt_solver.satisfiable(constraints)) {
                        policy.writeRegister("eip", SymbolicSemantics::ValueType<32>(target));
                    } else {
                        trace->mesg("%s: chosen control flow path is not feasible (or unknown).", name);
                        break;
                    }
                }
            }

            // Show the value of the EAX register since this is where GCC puts the function's return value.  If we did things
            // right, the return value should depend only on the unknown bytes from the beginning of the buffer.
            SymbolicSemantics::ValueType<32> result = policy.readRegister<32>("eax");
            std::set<Rose::BinaryAnalysis::SymbolicExpr::LeafNodePtr> vars = result.get_expression()->get_variables();
            {
                std::ostringstream s;
                s <<name <<": symbolic return value is " <<result <<"\n"
                  <<name <<": return value has " <<vars.size() <<" variables:";
                for (std::set<Rose::BinaryAnalysis::SymbolicExpr::LeafNodePtr>::iterator vi=vars.begin();
                     vi!=vars.end(); ++vi)
                    s <<" " <<*vi;
                s <<"\n";
                if (!constraints.empty()) {
                    s <<name <<": path constraints:\n";
                    for (std::vector<TreeNodePtr>::iterator ci=constraints.begin(); ci!=constraints.end(); ++ci)
                        s <<name <<":   " <<*ci <<"\n";
                }
                trace->mesg("%s", s.str().c_str());
            }

            // Now give values to those bytes and solve the equation for the result using an SMT solver.
            if (!result.is_known()) {
                trace->mesg("%s: setting variables (buffer bytes) to 'x' and evaluating the function symbolically...", name);
                std::vector<TreeNodePtr> exprs = constraints;
                LeafNodePtr result_var = LeafNode::create_variable(32);
                TreeNodePtr expr = InternalNode::create(32, OP_EQ, result.get_expression(), result_var);
                exprs.push_back(expr);
                for (std::set<LeafNodePtr>::iterator vi=vars.begin(); vi!=vars.end(); ++vi) {
                    expr = InternalNode::create(32, OP_EQ, *vi, LeafNode::create_integer(32, (int)'x'));
                    exprs.push_back(expr);
                }
                if (Rose::BinaryAnalysis::SmtSolver::SAT_YES == smt_solver.satisfiable(exprs)) {
                    LeafNodePtr result_value = smt_solver.evidenceForVariable(result_var)->isLeafNode();
                    if (!result_value) {
                        trace->mesg("%s: evaluation result could not be determined. ERROR!", name);
                    } else if (!result_value->is_known()) {
                        trace->mesg("%s: evaluation result is not constant. ERROR!", name);
                    } else {
                        trace->mesg("%s: evaluation result is 0x%08"PRIx64, name, result_value->get_value());
                    }
                } else {
                    trace->mesg("%s: expression is not satisfiable. (or unknown)", name);
                }
            }

            // Now try going the other direction.  Set the return expression to a value and try to discover what two bytes
            // would satisfy the equation.
            if (!result.is_known()) {
                trace->mesg("%s: setting result equal to 0xff015e7c and trying to find inputs...", name);
                std::vector<TreeNodePtr> exprs = constraints;
                TreeNodePtr expr = InternalNode::create(32, OP_EQ, result.get_expression(),
                                                        LeafNode::create_integer(32, 0xff015e7c));
                exprs.push_back(expr);
                if (Rose::BinaryAnalysis::SmtSolver::SAT_YES == smt_solver.satisfiable(exprs)) {
                    for (std::set<LeafNodePtr>::iterator vi=vars.begin(); vi!=vars.end(); ++vi) {
                        LeafNodePtr var_val = smt_solver.evidenceForVariable(*vi)->isLeafNode();
                        if (var_val && var_val->is_known())
                            trace->mesg("%s:   v%"PRIu64" = %"PRIu64" %c",
                                        name, (*vi)->get_name(), var_val->get_value(),
                                        isprint(var_val->get_value())?(char)var_val->get_value():' ');
                    }
                } else {
                    trace->mesg("%s:   expression is not satisfiable (or unknown).  No solutions.", name);
                }
            }

            // Reactivate the simulator in case we want to continue simulating.
            args.thread->get_process()->get_simulator()->activate();
            throw this; // Optional: will exit simulator, caught in main(), which then deactivates the simulator
        }
        return enabled;
    } catch (const Analysis*) {
        args.thread->get_process()->get_simulator()->activate();
        throw;
    }
};

int main(int argc, char *argv[], char *envp[])
{
    // Parse (and remove) switches intended for the analysis.
    std::string trigger_func, analysis_func;
    rose_addr_t trigger_va=0, analysis_va=0;
    std::vector<bool> take_branch;
    for (int i=1; i<argc; i++) {
        if (!strncmp(argv[i], "--trigger=", 10)) {
            // name or address of function triggering analysis
            char *rest;
            trigger_va = strtoull(argv[i]+10, &rest, 0);
            if (*rest) {
                trigger_va = 0;
                trigger_func = argv[i]+10;
            }
            memmove(argv+i, argv+i+1, (argc-- - i)*sizeof(*argv));
            --i;
        } else if (!strncmp(argv[i], "--analysis-func=", 16)) {
            // name or address of function to analyze
            char *rest;
            analysis_va = strtoull(argv[i]+16, &rest, 0);
            if (*rest) {
                analysis_va = 0;
                analysis_func = argv[i]+16;
            }
            memmove(argv+i, argv+i+1, (argc-- - i)*sizeof(*argv));
            --i;
        } else if (!strncmp(argv[i], "--take-branch=", 14)) {
            // --take-branch=STRING.  The STRING is a series of boolean values (t/f or 1/0 or y/n) that say what to do when the
            // analysis hits a conditional branch instruction and cannot decide whether the branch should be taken or not
            // taken.  Whatever choice the user specifies here causes the analysis to add a new constraint to the solver.
            for (size_t j=14; argv[i][j]; j++)
                take_branch.push_back(NULL!=strchr("t1y", argv[i][j]));
            memmove(argv+i, argv+i+1, (argc-- -i)*sizeof(*argv));
            --i;
        }
    }
    if (trigger_func.empty() && 0==trigger_va) {
        std::cerr <<argv[0] <<": --trigger-func=NAME_OR_ADDR is required\n";
        return 1;
    }
    if (analysis_func.empty() && 0==analysis_va) {
        std::cerr <<argv[0] <<": --analysis-func=NAME_OR_ADDR is required\n";
        return 1;
    }

    // Parse switches intended for the simulator.
    RSIM_Linux32 sim;
    int n = sim.configure(argc, argv, envp);

    // Parse the ELF container so we can get to the symbol table.  This is normal ROSE static analysis.
    char *rose_argv[4];
    int rose_argc=0;
    rose_argv[rose_argc++] = argv[0];
    rose_argv[rose_argc++] = strdup("-rose:read_executable_file_format_only");
    rose_argv[rose_argc++] = argv[n];
    rose_argv[rose_argc] = NULL;
    SgProject *project = frontend(rose_argc, rose_argv);

    // Find the address of "main" and analysis functions.
    if (!trigger_func.empty())
        trigger_va = RSIM_Tools::FunctionFinder().address(project, trigger_func);
    assert(trigger_va!=0);
    if (!analysis_func.empty())
        analysis_va = RSIM_Tools::FunctionFinder().address(project, analysis_func);
    assert(analysis_va!=0);

    // Register the analysis callback.
    Analysis analysis(trigger_va, analysis_va, take_branch);
    sim.install_callback(&analysis);

    // The rest is normal boiler plate to run the simulator, except we'll catch the Analysis to terminate the simulation early
    // if desired.
    sim.install_callback(new RSIM_Tools::UnhandledInstruction);
    sim.exec(argc-n, argv+n);
    sim.activate();
    try {
        sim.main_loop();
    } catch (Analysis*) {
    }
    sim.deactivate();
    return 0;
}





#else
int main(int, char *argv[])
{
    std::cerr <<argv[0] <<": not supported on this platform" <<std::endl;
    return 0;
}

#endif /* ROSE_ENABLE_SIMULATOR */
