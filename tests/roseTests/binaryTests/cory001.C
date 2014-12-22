#include <iostream>
#include "rose.h"
#include "SymbolicSemantics.h"

// ROSE is slowly organizing all binary analysis into name spaces.  SymbolicSemantics is now inside
// BinaryAnalysis::InstructionSemantics.
using namespace rose::BinaryAnalysis::InstructionSemantics;

typedef SymbolicSemantics::Policy<SymbolicSemantics::State,SymbolicSemantics::ValueType> Policy;
typedef SymbolicSemantics::State<SymbolicSemantics::ValueType> State;
typedef X86InstructionSemantics<Policy, SymbolicSemantics::ValueType> Semantics;

#if 1   // deprecated

// This old method of using hard-coded register numbers, although fast, has some problems.  Two big ones are that it doesn't
// allow a software layer (SymbolicSemantics::State) to safely and easily implement a subset of all available registers, and it
// isn't able to easily describe parts of registers (like the x86 OF flag vs. the entire EFLAGS register, or EIP vs. RIP, etc.)
#define REG_ESP registers.gpr[x86_gpr_sp]

#elif 1

// ROSE is moving toward a more dynamic way of referring to registers.  See the doxygen documentation for the
// RegisterDictionary and RegisterDescriptor classes.  All recently implemented policies have a findRegister() method that
// returns a register descriptor (or an exception).  For SymbolicSemantics::State you'd have to know that it uses the register
// major number as an array index.  You could also cache the major number since it shouldn't change for the life of the
// SymbolicSemantics::Policy<> object.
#define REG_ESP registers.gpr[policy.findRegister("esp", 32).get_major()]

#else

// If you're not copying states around and just need the value of a register in the policy's current state, you can use this
// code.  There's also a faster version that takes a register descriptor.
#define REG_ESP policy.readRegister<32>("esp")

#endif

int
main(int argc, char* argv[])
{
    SgProject *project = frontend(argc, argv);
    ROSE_ASSERT (project != NULL);
  
    Policy policy;
    Semantics semantics(policy);

    std::vector<SgNode*> tree =NodeQuery::querySubTree(project, V_SgAsmBlock);
    for (size_t x = 0; x < tree.size(); x++) {
        SgAsmBlock* bblock = isSgAsmBlock(tree[x]);
        const SgAsmStatementPtrList &insns = bblock->get_statementList();
        for (size_t i=0; i<insns.size(); ++i) {
            SgAsmX86Instruction *insn = isSgAsmX86Instruction(insns[i]);
            if (insn) {
                try {
                    std::cout << unparseInstructionWithAddress(insn) << std::endl;

                    // Get the state before processing the instruction.
                    State& state = policy.get_state();
                    std::cout << "ESP1=" << *state.REG_ESP.get_expression() << std::endl;

                    // Most instruction semantic policies require that the instruction being processed has the same address as
                    // that which is currently stored in the instruction pointer register.  SymbolicSemantics is one of them,
                    // although it only does the check when the EIP register has a known (not variable) value.  So we just make
                    // sure EIP has the right value before we process the instruction. This is only needed for the first
                    // instruction of any block provided we can guarantee that the semantics ROSE used for assembling the basic
                    // block are similar to the semantics we're using here.  We'll just do it for all instructions. ;-)
                    policy.writeRegister("eip", policy.number<32>(insn->get_address()));
	  
                    semantics.processInstruction(insn);

#ifdef WORKS
                    // These two lines are just mean to show that it's possible to get the state
                    // again, just not by re-using the same reference.
                    State& state2 = policy.get_state();
                    std::cout << "ESP2=" << *state2.REG_ESP.get_expression() << std::endl;
#else
                    // Here's where the segmentation fault occurs...
                    // This next line causes the segmentation fault... (and the one after uses the variable)
                    state = policy.get_state();
                    std::cout << "ESP3=" << *state.REG_ESP.get_expression() << std::endl;
#endif
                }
                // If processing the instruction failed, report the error!
                catch (const Semantics::Exception &e) {
                    std::cerr << e << " (instruction skipped)" << std::endl;
                }
            }
        }
    }
}
