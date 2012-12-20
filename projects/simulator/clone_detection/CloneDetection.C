#include "rose.h"
#include "RSIM_Private.h"

// This source file can only be compiled if the simulator is enabled. Furthermore, it only makes sense to compile this file if
// the simulator is using our code from CloneDetectionSemantics.h (i.e., the CloneDetection.patch file has been applied to
// RSIM).
#if defined(ROSE_ENABLE_SIMULATOR) && defined(RSIM_CloneDetectionSemantics_H)

#include "RSIM_Linux32.h"

/******************************************************************************************************************************/

// Runs clone detection over specimen functions.
class CloneDetector {
protected:
    static const char *name;            /**< For --debug output. */
    RSIM_Thread *thread;                /**< Thread where analysis is running. */
public:
    CloneDetector(RSIM_Thread *thread): thread(thread) {}

    void analyze() {
        RSIM_Process *proc = thread->get_process();
        RTS_Message *m = thread->tracing(TRACE_MISC);
        m->mesg("%s triggered; disassembling entire specimen image...\n", name);
        MemoryMap map(proc->get_memory(), MemoryMap::COPY_SHALLOW);
        map.prune(MemoryMap::MM_PROT_READ); // don't let the disassembler read unreadable memory, else it will segfault

        // Removes execute permission for any segment whose debug name does not contain the name of the executable. When
        // comparing two different executables for clones, we probably don't need to compare code that came from dynamically
        // linked libraries since they will be identical in both executables.
        struct Pruner: MemoryMap::Visitor {
            std::string exename;
            Pruner(const std::string &exename): exename(exename) {}
            virtual bool operator()(const MemoryMap*, const Extent&, const MemoryMap::Segment &segment_) {
                MemoryMap::Segment *segment = const_cast<MemoryMap::Segment*>(&segment_);
                if (segment->get_name().find(exename)==std::string::npos) {
                    unsigned p = segment->get_mapperms();
                    p &= ~MemoryMap::MM_PROT_EXEC;
                    segment->set_mapperms(p);
                }
                return true;
            }
        } pruner(proc->get_exename());
        map.traverse(pruner);

        // Disassemble the process image and organize it into functions
        std::ostringstream ss;
        map.dump(ss, "  ");
        m->mesg("%s: using this memory map for disassembly:\n%s", name, ss.str().c_str());
        SgAsmBlock *gblk = proc->disassemble(false/*take no shortcuts*/, &map);
        std::vector<SgAsmFunction*> functions = SageInterface::querySubTree<SgAsmFunction>(gblk);
        m->mesg("%s: fuzz testing %zu function%s", name, functions.size(), 1==functions.size()?"":"s");

        // Perform data type analysis on each function in order to find which things are pointers and which are non-pointers
        // (FIXME: See demos/types_1.C)

        // Choose some input values. (FIXME: eventually we need to make these random)
        CloneDetection::InputValues inputs;
        inputs.add_integer(1);
        inputs.add_integer(50);
        inputs.add_integer(100);
        inputs.add_pointer(true); // non-null pointer
        inputs.add_pointer(false); // null pointer
        inputs.add_pointer(true); // another (different) non-null pointer

        // Fuzz test each function
        for (std::vector<SgAsmFunction*>::iterator fi=functions.begin(); fi!=functions.end(); ++fi)
            fuzz_test(*fi, &inputs);
    }

    void fuzz_test(SgAsmFunction *function, CloneDetection::InputValues *inputs) {
        RSIM_Process *proc = thread->get_process();
        RTS_Message *m = thread->tracing(TRACE_MISC);
        m->mesg("%s", std::string(100, '=').c_str());
        m->mesg("%s: fuzz testing function \"%s\" at 0x%08"PRIx64, name, function->get_name().c_str(), function->get_entry_va());
        m->mesg("%s", std::string(100, '=').c_str());
        proc->mem_transaction_start(name);
        pt_regs_32 saved_regs = thread->get_regs();
        thread->policy.trigger(function->get_entry_va(), inputs);
        try {
            thread->main();
        } catch (const Disassembler::Exception &e) {
            std::ostringstream ss;
            m->mesg("%s: function disassembly failed at 0x%08"PRIx64": %s", name, e.ip, e.mesg.c_str());
        }
        CloneDetection::Outputs<RSIM_SEMANTICS_VTYPE> *outputs = thread->policy.get_outputs();
        outputs->print(m, "Function outputs:", "  ");
        thread->init_regs(saved_regs);
        proc->mem_transaction_rollback(name);
    }
};

const char *CloneDetector::name = "CloneDetector";

/******************************************************************************************************************************/

// Instruction callback to trigger clone detection.  The specimen executable is allowed to run until it hits the specified
// trigger address (in order to cause dynamically linked libraries to be loaded and linked), at which time a CloneDetector
// object is created and thrown.
class Trigger: public RSIM_Callbacks::InsnCallback {
protected:
    rose_addr_t trigger_va;             /**< Address at which a CloneDetector should be created and thrown. */
    bool armed;                         /**< Should this object be monitoring instructions yet? */
    bool triggered;                     /**< Has this been triggered yet? */
public:
    Trigger(): trigger_va(0), armed(false), triggered(false) {}

    // For simplicity, all threads, processes, and simulators will share the same SemanticController object.  This means that
    // things probably won't work correctly when analyzing a multi-threaded specimen, but it keeps this demo more
    // understandable.
    virtual Trigger *clone() /*override*/ { return this; }

    // Arm this callback so it starts monitoring instructions for the trigger address.
    void arm(rose_addr_t trigger_va) {
        this->trigger_va = trigger_va;
        armed = true;
    }

    // Called for every instruction by every thread. When the trigger address is reached for the first time, create a new
    // CloneDetector and throw it.  The main() will catch it and start its analysis.
    virtual bool operator()(bool enabled, const Args &args) /*override*/ {
        if (enabled && armed && !triggered && args.insn->get_address()==trigger_va) {
            triggered = true;
            throw new CloneDetector(args.thread);
        }
        return enabled;
    }
};
            
    
/******************************************************************************************************************************/
int
main(int argc, char *argv[], char *envp[])
{
    std::ios::sync_with_stdio();

    // Our instruction callback.  We can't set its trigger address until after we load the specimen, but we want to register
    // the callback with the simulator before we create the first thread.
    Trigger clone_detection_trigger;

    // All of this is standard boilerplate and documented in the first page of the simulator doxygen.
    RSIM_Linux32 sim;
    sim.install_callback(new RSIM_Tools::UnhandledInstruction); // needed by some versions of ld-linux.so
    sim.install_callback(&clone_detection_trigger);             // it mustn't be destroyed while the simulator's running
    int n = sim.configure(argc, argv, envp);
    sim.exec(argc-n, argv+n);

    // Now that we've loaded the specimen into memory and created its first thread, figure out its original entry point (OEP)
    // and arm the clone_detection_trigger so that it triggers clone detection when the OEP is reached.  This will allow the
    // specimen's dynamic linker to run.
    rose_addr_t trigger_va = sim.get_process()->get_ep_orig_va();
    clone_detection_trigger.arm(trigger_va);

    // Allow the specimen to run until it reaches the clone detection trigger point, at which time a CloneDetector object is
    // thrown for that thread that reaches it first.
    try {
        sim.main_loop();
    } catch (CloneDetector *clone_detector) {
        clone_detector->analyze();
    }

    return 0;
}

#else
int main(int, char *argv[])
{
    std::cerr <<argv[0] <<": not supported on this platform" <<std::endl;
    return 0;
}

#endif /* ROSE_ENABLE_SIMULATOR */
