/* Demonstrates a simulator capability:  calls a function that is not normally called.
 *
 * 1. Allow the specimen to execute up to a certain point in order to resolve dynamic linking.  This demo parses the ELF file
 *    to find the address of "main" and stops when it is reached.
 *
 * 2. Change the execution point to be a function which is not normally called.  This demo calls a function named "payload".
 */

#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR /* protects this whole file */

#include "RSIM_Linux32.h"

/* Traverses the AST to find a symbol for a global function with specified name. */
class FunctionFinder: public AstSimpleProcessing {
public:
    SgNode *ast;
    std::string fname;
    FunctionFinder(SgNode *ast, const std::string &fname)
        : ast(ast), fname(fname) {}

    rose_addr_t address() {
        try {
            this->traverse(ast, preorder);
        } catch (rose_addr_t addr) {
            return addr;
        }
        return 0;
    }

    void visit(SgNode *node) {
        SgAsmElfSymbol *sym = isSgAsmElfSymbol(node);
        if (sym &&
            sym->get_def_state() == SgAsmGenericSymbol::SYM_DEFINED &&
            sym->get_binding()   == SgAsmGenericSymbol::SYM_GLOBAL &&
            sym->get_type()      == SgAsmGenericSymbol::SYM_FUNC &&
            sym->get_name()->get_string() == fname)
            throw sym->get_value();
    }
};

/* Monitors the CPU instruction pointer.  When it reaches a specified value, replace it with some other value. */
class ExecutionTransfer: public RSIM_Callbacks::InsnCallback {
public:
    rose_addr_t from_addr, to_addr;
    ExecutionTransfer(rose_addr_t from_addr, rose_addr_t to_addr)
        :from_addr(from_addr), to_addr(to_addr) {
        assert(from_addr!=to_addr);
    }
    virtual ExecutionTransfer *clone() { return this; }
    virtual bool operator()(bool enabled, const Args &args) {
        if (args.thread->policy.readIP().known_value()==from_addr) {
            std::cerr <<"transfering control from " <<StringUtility::addrToString(from_addr)
                      <<" to " <<StringUtility::addrToString(to_addr) <<"\n";
            // we could push a return address onto the stack here...
            args.thread->policy.writeIP(args.thread->policy.number<32>(to_addr));
            enabled = false; /* do not execute instruction at from_addr */
        }
        return enabled;
    }
};

int main(int argc, char *argv[], char *envp[])
{
    /* Configure the simulator by parsing command-line switches. The return value is the index of the executable name in argv. */
    RSIM_Linux32 sim;
    int n = sim.configure(argc, argv, envp);

    /* Parse the ELF container so we can get to the symbol table. */
    char *rose_argv[4];
    int rose_argc=0;
    rose_argv[rose_argc++] = argv[0];
    rose_argv[rose_argc++] = strdup("-rose:read_executable_file_format_only");
    rose_argv[rose_argc++] = argv[n];
    rose_argv[rose_argc] = NULL;
    SgProject *project = frontend(rose_argc, rose_argv);

    /* Find the address of "main" and "payload" functions. */
    rose_addr_t main_addr = FunctionFinder(project, "main").address();
    assert(main_addr!=0);
    rose_addr_t payload_addr = FunctionFinder(project, "payload").address();
    assert(payload_addr!=0);

    /* Set a transfer point from "main" to "payload" */
    ExecutionTransfer xfer(main_addr, payload_addr);
    sim.get_callbacks().add_insn_callback(RSIM_Callbacks::BEFORE, &xfer);

    /* Create the initial process object by loading a program and initializing the stack.   This also creates the main thread,
     * but does not start executing it. */
    sim.exec(argc-n, argv+n);

    /* Get ready to execute by making the specified simulator active. This sets up signal handlers, etc. */
    sim.activate();

    /* Allow executor threads to run and return when the simulated process terminates. The return value is the termination
     * status of the simulated program. */
    sim.main_loop();

    /* Not really necessary since we're not doing anything else. */
    sim.deactivate();

    /* Describe termination status, and then exit ourselves with that same status. */
    sim.describe_termination(stderr);
    sim.terminate_self(); // probably doesn't return
    return 0;
}





#else
int main(int, char *argv[])
{
    std::cerr <<argv[0] <<": not supported on this platform" <<std::endl;
    return 0;
}

#endif /* ROSE_ENABLE_SIMULATOR */
