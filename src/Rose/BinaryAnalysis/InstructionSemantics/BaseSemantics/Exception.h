#ifndef ROSE_BinaryAnalysis_InstructionSemantics_BaseSemantics_Exception_H
#define ROSE_BinaryAnalysis_InstructionSemantics_BaseSemantics_Exception_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/BasicTypes.h>
#include <Rose/Exception.h>

class SgAsmInstruction;

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace BaseSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Exceptions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Base class for exceptions thrown by instruction semantics. */
class Exception: public Rose::Exception {
public:
    SgAsmInstruction *insn;
    Exception(const std::string &mesg, SgAsmInstruction *insn): Rose::Exception(mesg), insn(insn) {}
    void print(std::ostream&) const;
};

class NotImplemented: public Exception {
public:
    NotImplemented(const std::string &mesg, SgAsmInstruction *insn)
        : Exception(mesg, insn) {}
};

std::ostream& operator<<(std::ostream&, const Exception&);

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
