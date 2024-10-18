// SgAsmPowerpcInstruction member definitions.
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>

using namespace Rose;                                   // temporary until this lives in "rose"
using namespace Rose::BinaryAnalysis;

// class method
RegisterDictionary::Ptr
SgAsmPowerpcInstruction::registersForWidth(size_t nBits) {
    switch (nBits) {
        case 32: return Architecture::findByName("ppc32-be").orThrow()->registerDictionary();
        case 64: return Architecture::findByName("ppc64-be").orThrow()->registerDictionary();
        default: ASSERT_not_reachable("invalid PowerPC instruction size");
    }
}

unsigned
SgAsmPowerpcInstruction::get_anyKind() const {
    return p_kind;
}

std::string
SgAsmPowerpcInstruction::conditionalBranchDescription() const {
    std::string retval;
    unsigned atBits = 0;
    SgAsmExpression *arg0 = get_operandList()->get_operands().empty() ? NULL : get_operandList()->get_operands()[0];
    if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(arg0)) {
        unsigned bo = ival->get_absoluteValue();
        if ((bo & ~1u) == 0) {       // 0000z
            retval = "if --CTR != 0 && condition clear";
        } else if ((bo & ~1u) == 2) { // 0001z
            retval = "if --CTR == 0 && condition clear";
        } else if ((bo & ~3u) == 4) { // 001at
            retval = "if condition clear";
            atBits = bo & 3u;
        } else if ((bo & ~1u) == 8) { // 0100z
            retval = "if --CTR != 0 && condition set";
        } else if ((bo & ~1u) == 10) { // 0101z
            retval = "if --CTR == 0 && condition set";
        } else if ((bo & ~3u) == 12) { // 011at
            retval = "if condition set";
            atBits = bo & 3u;
        } else if ((bo & ~9u) == 16) { // 1a00t
            retval = "if --CTR != 0";
            atBits = ((bo & 8u) >> 2) | (bo & 1u);
        } else if ((bo & ~9u) == 18) { // 1a01t
            retval = "if --CTR == 0";
            atBits = ((bo & 8u) >> 2) | (bo & 1u);
        } else if ((bo & ~11u) == 20) { // 1z1zz
            retval = "unconditionally";
        }

        switch (atBits) {
            case 0: break; // no hint
            case 1: break; // invalid, reserved
            case 2: retval += " (unlikely)"; break;
            case 3: retval += " (likely)"; break;
        }
    }
    return retval;
}

#endif
