#ifndef ROSE_BinaryAnalysis_InstructionSemantics2_BaseSemantics_Exception_H
#define ROSE_BinaryAnalysis_InstructionSemantics2_BaseSemantics_Exception_H
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <BaseSemanticsTypes.h>
#include <RoseException.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
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
