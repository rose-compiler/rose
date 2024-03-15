#ifndef ROSE_BinaryAnalysis_Disassembler_Exception_H
#define ROSE_BinaryAnalysis_Disassembler_Exception_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/Exception.h>

#include <Cxx_GrammarDeclarations.h>
#include <sageContainer.h>

#include <ostream>
#include <string>

namespace Rose {
namespace BinaryAnalysis {
namespace Disassembler {

/** Exception thrown by the disassemblers. */
class Exception: public Rose::Exception {
public:
    /** A bare exception not bound to any particular instruction. */
    Exception(const std::string &reason)
        : Rose::Exception(reason), ip(0), bit(0), insn(NULL)
        {}

    /** An exception bound to a virtual address but no raw data or instruction. */
    Exception(const std::string &reason, rose_addr_t ip)
        : Rose::Exception(reason), ip(ip), bit(0), insn(NULL)
        {}

    /** An exception bound to a particular instruction being disassembled. */
    Exception(const std::string &reason, rose_addr_t ip, const SgUnsignedCharList &raw_data, size_t bit)
        : Rose::Exception(reason), ip(ip), bytes(raw_data), bit(bit), insn(NULL)
        {}

    /** An exception bound to a particular instruction being assembled. */
    Exception(const std::string &reason, SgAsmInstruction*);

    ~Exception() throw() {}

    void print(std::ostream&) const;
    friend std::ostream& operator<<(std::ostream &o, const Exception &e);

    rose_addr_t ip;                 /**< Virtual address where failure occurred; zero if no associated instruction */
    SgUnsignedCharList bytes;       /**< Bytes (partial) of failed disassembly, including byte at failure. Empty if the
                                     *   exception is not associated with a particular byte sequence, such as if an
                                     *   attempt was made to disassemble at an invalid address. */
    size_t bit;                     /**< Bit offset in instruction byte sequence where disassembly failed (bit/8 is the
                                     *   index into the "bytes" list, while bit%8 is the bit within that byte. */
    SgAsmInstruction *insn;         /**< Instruction associated with an assembly error. */
};

} // namespace
} // namespace
} // namespace
#endif
#endif
