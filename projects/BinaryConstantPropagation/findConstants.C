#include "findConstants.h"

int
main(int argc, char** argv)
{
    SgProject* proj = frontend(argc, argv);

    /* Find all x86 instructions */
    std::vector<SgNode*> instructions = NodeQuery::querySubTree(proj, V_SgAsmx86Instruction);
    ROSE_ASSERT (!instructions.empty());

    /* Find the interpretation and header */
    SgAsmNode* n = isSgAsmNode(instructions[0]);
    while (n && !isSgAsmInterpretation(n))
        n = isSgAsmNode(n->get_parent());
    ROSE_ASSERT (n);
    SgAsmInterpretation* interp = isSgAsmInterpretation(n);
    SgAsmGenericHeader* header = interp->get_header();

#if 0  /*Jeremiah's search for syscalls*/
    /* Initialize semantics with entry address for executable */
    FindConstantsPolicy policy; /*defined above*/
    X86InstructionSemantics<FindConstantsPolicy, XVariablePtr> t(policy);
    uint32_t entry = header->get_entry_rva() + header->get_base_va();
    policy.entryPoint = entry;

    for (size_t i = 0; i < instructions.size(); ++i) {
        SgAsmx86Instruction* insn = isSgAsmx86Instruction(instructions[i]);
        ROSE_ASSERT (insn);
        cerr << "Working on address 0x" << hex << insn->get_address() << dec << endl;
        t.processInstruction(insn);
    }

    /* Build the graph (info) containing information about each instruction calls another. */
    VirtualBinCFG::AuxiliaryInformation info(proj);

    /* Obtain syscall info: name of system call, call number, and names and types for formal arguments.  The *.h file contains
     * the declarations in the kernel for the system calls. The second file is a text file contaiing one line per syscall
     * where each line is an integer followed by the syscall name. */
    vector<linux_syscall> syscalls = getSyscalls("/usr/src/linux-2.6.27-gentoo-r8/include/linux/syscalls.h",
                                                 "syscall_list");

    /* For each INT instruction look at the contents of the AX register, which contains the system call number. If that
     * register contains a known constant value that corresponds to one of the syscall numbers we identified above then we
     * know what system call is being invoked by that INT instruction. (This is x86-specific). */
    for (map<uint64_t, RegisterSet>::const_iterator i = policy.rsets.begin(); i != policy.rsets.end(); ++i) {
        SgAsmx86Instruction* insn = isSgAsmx86Instruction(info.getInstructionAtAddress(i->first));
        if (insn == NULL) continue;
        cout << "Address 0x" << hex << i->first << dec << ": "
             << (policy.isInstructionExternallyVisible(insn) ? "EXTERN " : "") << unparseInstruction(insn) << endl;
        // cout << i->second << endl;
        if (insn->get_kind() == x86_int) {
            const RegisterSet& rset = i->second;
            LatticeElement<32> eax = rset.gpr[x86_gpr_ax]->get();
            if (eax.isTop || eax.name != 0) { // Not a constant
                cerr << "System call number not a constant" << endl;
            } else {
                linux_syscall call;
                bool found = false;
                for (size_t i = 0; i < syscalls.size(); ++i) {
                    if (syscalls[i].number != -1 && syscalls[i].number == eax.offset) {
                        found = true;
                        call = syscalls[i];
                        break;
                    }
                }
                if (!found) {
                    call.name = "unknown_" + boost::lexical_cast<string>(eax.offset);
                    call.number = eax.offset;
                    call.arguments.clear();
                }
                cout << "Found system call " << call.name << endl;

                /* System call arguments are passed in these general purpose registers.  Print the contents of each register
                 * as a LatticeElement which contains a known constant or a named (unknown) constant, or a named constant plus a
                 * known constant addend. */
                for (size_t param = 0; param < call.arguments.size(); ++param) {
                    string name = call.arguments[param].name;
                    X86GeneralPurposeRegister paramRegs[] = {
                        x86_gpr_bx, x86_gpr_cx, x86_gpr_dx, x86_gpr_si, x86_gpr_di, x86_gpr_bp
                    };
                    assert (param < 6);
                    X86GeneralPurposeRegister reg = paramRegs[param];
                    cout << name << " = " << rset.gpr[reg] << endl;
                }
                cout << "End of system call" << endl << endl;
            }
        }
    }
#else /*Robb's search for signal handlers*/

    /* Compute semantics only for specified range of addresses in order to avoid bad instruction assertions. The assertions
     * sometimes fail because we disassembled an instruction at a weird address (like part way into some other instruction). */
    rose_addr_t minaddr = 0x0;
    rose_addr_t maxaddr = 0xffffffff;
    std::cout <<"processing instruction semantics...\n";
    FindConstantsPolicy policy;
    X86InstructionSemantics<FindConstantsPolicy, XVariablePtr> t(policy);
    for (size_t i=0; i<instructions.size(); i++) {
        SgAsmx86Instruction *insn = isSgAsmx86Instruction(instructions[i]);
        ROSE_ASSERT(insn);
        if (insn->get_address()>=minaddr && insn->get_address()<=maxaddr)
            t.processInstruction(insn);
    }

    /* Build the graph (info) containing information about how each instruction calls another. */
    std::cout <<"building auxiliary information graph...\n";
    VirtualBinCFG::AuxiliaryInformation info(proj);

    /* Show each instruction along with its initial register set. */
    for (std::map<uint64_t, RegisterSet>::const_iterator i = policy.rsets.begin(); i != policy.rsets.end(); ++i) {
        SgAsmx86Instruction* insn = isSgAsmx86Instruction(info.getInstructionAtAddress(i->first));
        if (insn == NULL) continue;

        const RegisterSet& rset = i->second;
        std::cout <<std::hex <<i->first <<": initial conditions...\n"
                  <<rset  /*implied endl*/
                  <<"    " <<std::dec <<": " <<unparseInstruction(insn) <<"\n\n";
    }

#endif
    return 0;
}
