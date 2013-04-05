// Refer to MultiWithConversion.h for documentation

#include "rose.h"
#include "RSIM_Private.h"

// This source file can only be compiled if the simulator is enabled. Furthermore, it only makes sense to compile this file if
// the simulator is using our code from MultiWithConversion.h (i.e., the patch described in that file has been applied to RSIM).
#if defined(ROSE_ENABLE_SIMULATOR) && defined(RSIM_MultiWithConversion_H)

#include "RSIM_Linux32.h"

// This is a per-instruction callback.  Before the very first instruction executes, we want to disable all but the simulator's
// concrete domain.  When we hit the "trigger" address we will re-enable all our domains and branch to the "target" address.
class SemanticController: public RSIM_Callbacks::InsnCallback {
protected:
    const char *name;                   /**< Name to print in diagnostics. */
    rose_addr_t trigger_va;             /**< Address at which the analysis will be enabled. */
    rose_addr_t target_va;              /**< Address to which the analysis branches when it starts. */
    bool armed;                         /**< Should this callback do anything? */
    bool triggered;                     /**< Has the analysis been triggered yet? */
public:

    SemanticController()
        : name("SemanticController"), trigger_va(0), target_va(0), armed(false), triggered(false) {}

    // Arm this callback so it starts monitoring instructions for the trigger address.
    void arm(rose_addr_t trigger_va, rose_addr_t target_va) {
        this->trigger_va = trigger_va;
        this->target_va = target_va;
        armed = true;
    }

    // For simplicity, all threads, processes, and simulators will share the same SemanticController object.  This means that
    // things probably won't work correctly when analyzing a multi-threaded specimen, but it keeps this demo more
    // understandable.
    virtual SemanticController *clone() /*override*/ { return this; }

    // Gets called before the simulator executes each instruction.  All this does is watch what instructions are being executed
    // and when we hit the right one, enable all our sub-domains and branch to the target virtual address (i.e., "arbitrary
    // offset").  Only do it once.
    virtual bool operator()(bool enabled, const Args &args) /*override*/ {
        if (enabled && armed && !triggered && args.insn->get_address()==trigger_va) {
            args.thread->policy.trigger(target_va);
            triggered = true;
        }
        return enabled;
    }
};

int
main(int argc, char *argv[], char *envp[])
{
    std::ios::sync_with_stdio();

    // Parse command-line switches understood by MultiWithConversion and leave the rest for the simulator.
    rose_addr_t target_va = 0; // analysis address (i.e., the "arbitrary offset"). Zero implies the program's OEP
    for (int i=1; i<argc; ++i) {
        if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h") || !strcmp(argv[i], "-?")) {
            std::cout <<"usage: " <<argv[0] <<" [--target=ADDRESS] [SIMULATOR_SWITCHES] SPECIMEN [SPECIMEN_ARGS...]\n";
            exit(0);
        } else if (!strncmp(argv[i], "--target=", 9)) {
            target_va = strtoull(argv[i]+9, NULL, 0);
            memmove(argv+i, argv+i+1, (argc-- -i)*sizeof(*argv)); // argv has argc+1 elements
            --i;
        } else {
            break;
        }
    }

    // Our instruction callback.  We can't set its trigger address until after we load the specimen, but we want to register
    // the callback with the simulator before we create the first thread.
    SemanticController semantic_controller;

    // All of this (except the part where we register our SemanticController) is standard boilerplate and documented in
    // the first page of doxygen.
    RSIM_Linux32 sim;
    sim.install_callback(new RSIM_Tools::UnhandledInstruction); // needed by some versions of ld-linux.so
    sim.install_callback(&semantic_controller);                 // it mustn't be destroyed while the simulator's running
    int n = sim.configure(argc, argv, envp);
    sim.exec(argc-n, argv+n);

    // Register our instruction callback and tell it to trigger at the specimen's original entry point (OEP).  This will cause
    // our analysis to run as soon as the dynamic linker is finished.  We don't know the OEP until after the sim.exec() call
    // that loads the specimen into memory, so by time we can register our callback, the RSIM_Process has copied 
    rose_addr_t trigger_va = sim.get_process()->get_ep_orig_va();
    semantic_controller.arm(trigger_va, 0==target_va ? trigger_va : target_va);

    //sim.activate();
    sim.main_loop();
    //sim.deactivate();
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
