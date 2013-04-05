// Example of using BinaryAnalysis::PointerAnalysis::PointerDetection to find pointer variables in a binary specimen.
#include "rose.h"
#include "BinaryPointerDetection.h"
#include "AsmFunctionIndex.h"
#include "YicesSolver.h"

// An object that organizes instructions so they can be looked up in O(log N) time.
class InstructionProvidor {
protected:
    typedef std::map<rose_addr_t, SgAsmInstruction*> InstructionMap;
    InstructionMap insns;
public:
    InstructionProvidor(SgNode *ast) {
        std::vector<SgAsmInstruction*> iv = SageInterface::querySubTree<SgAsmInstruction>(ast);
        for (std::vector<SgAsmInstruction*>::iterator ii=iv.begin(); ii!=iv.end(); ++ii)
            insns[(*ii)->get_address()] = *ii;
    }

    SgAsmInstruction *get_instruction(rose_addr_t va) const {
        InstructionMap::const_iterator ii = insns.find(va);
        return ii==insns.end() ? NULL : ii->second;
    }
};

int
main(int argc, char *argv[])
{
    // Extract command-line switches that we understand.
    std::string func_name;
    rose_addr_t func_va=0;
    bool use_solver=false, do_debug=false;
    for (int i=1; i<argc; ++i) {
        if (!strncmp(argv[i], "--function=", 11)) {
            char *rest;
            func_va = strtoull(argv[i]+11, &rest, 0);
            if (*rest) {
                func_va = 0;
                func_name = argv[i]+11;
            }
            memmove(argv+i, argv+i+1, (argc-- - i)*sizeof(*argv));
            --i;
        } else if (!strcmp(argv[i], "--with-yices")) {
            use_solver = true;
            memmove(argv+i, argv+i+1, (argc-- - i)*sizeof(*argv));
            --i;
        } else if (!strcmp(argv[i], "--debug")) {
            do_debug = true;
            memmove(argv+i, argv+i+1, (argc-- - i)*sizeof(*argv));
            --i;
        }
    }
    if (0==func_va && func_name.empty()) {
        std::cerr <<"usage: " <<argv[0] <<" --function=NAME_OR_ADDR [--with-yices] [ROSE_SWITCHES] BINARY_SPECIMEN\n";
        exit(1);
    }

    // Disassemble the specimen and partition it into functions
    SgProject *project = frontend(argc, argv);
    std::vector<SgAsmFunction*> functions = SageInterface::querySubTree<SgAsmFunction>(project);

    // Find the function in which we're interested.
    SgAsmFunction *func = NULL;
    for (std::vector<SgAsmFunction*>::iterator fi=functions.begin(); !func && fi!=functions.end(); ++fi) {
        if (!func_name.empty()) {
            if ((*fi)->get_name()==func_name)
                func = *fi;
        } else if ((*fi)->get_entry_va()==func_va) {
            func = *fi;
        }
    }
    if (!func) {
        std::cerr <<argv[0] <<": cannot find user-specified function. Functions are:\n" <<AsmFunctionIndex(project);
        exit(1);
    }
    std::cout <<"Anlyzing pointers in this function:\n";
    AsmUnparser().unparse(std::cout, func);
    
    // Perform the analysis on that function
    SMTSolver *solver = use_solver && YicesSolver::available_linkage() ? new YicesSolver : NULL;
    std::cout <<(solver?"Using":"Not using") <<" Yices SMT solver\n";
    InstructionProvidor insns(func);
    typedef BinaryAnalysis::PointerAnalysis::PointerDetection<InstructionProvidor> PointerDetector;
    PointerDetector ptr_detector(&insns, solver);
    if (do_debug)
        ptr_detector.set_debug(stderr);
    ptr_detector.analyze(func);

    // Show the results
    const PointerDetector::Pointers &pointers = ptr_detector.get_pointers();
    for (PointerDetector::Pointers::const_iterator pi=pointers.begin(); pi!=pointers.end(); ++pi) {
        if (pi->type & BinaryAnalysis::PointerAnalysis::DATA_PTR)
            std::cout <<"data ";
        if (pi->type & BinaryAnalysis::PointerAnalysis::CODE_PTR)
            std::cout <<"code ";
        std::cout <<"pointer at " <<pi->address <<"\n";
    }

    return 0;
}
