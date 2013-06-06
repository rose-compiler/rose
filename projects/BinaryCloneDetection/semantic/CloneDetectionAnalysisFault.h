#ifndef RSIM_AnalysisFault_H
#define RSIM_AnalysisFault_H

#include <cassert>
#include <cstdlib>

namespace CloneDetection {

/** Special output values for when something goes wrong. */
class AnalysisFault {
public:
    enum Fault {
        NONE        = 0,
        DISASSEMBLY = 911000001,     /**< Disassembly failed possibly due to bad address. */
        INSN_LIMIT  = 911000002,     /**< Maximum number of instructions executed. */
        HALT        = 911000003,     /**< x86 HLT instruction executed. */
        INTERRUPT   = 911000004,     /**< x86 INT instruction executed. */
        SEMANTICS   = 911000005,     /**< Some fatal problem with instruction semantics, such as a not-handled instruction. */
        SMTSOLVER   = 911000006,     /**< Some fault in the SMT solver. */
    };
    
    /** Return the short name of a fault ID. */
    static const char *fault_name(Fault fault) {
        switch (fault) {
            case NONE:          return "";
            case DISASSEMBLY:   return "FAULT_DISASSEMBLY";
            case INSN_LIMIT:    return "FAULT_INSN_LIMIT";
            case HALT:          return "FAULT_HALT";
            case INTERRUPT:     return "FAULT_INTERRUPT";
            case SEMANTICS:     return "FAULT_SEMANTICS";
            case SMTSOLVER:     return "FAULT_SMTSOLVER";
            default:
                assert(!"fault not handled");
                abort();
        }
    }
};

} // namespace
#endif
