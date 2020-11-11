#ifndef ROSE_BinaryAnalysis_Disassembler_Null_H
#define ROSE_BinaryAnalysis_Disassembler_Null_H

#include <featureTests.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <Disassembler.h>
#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>

namespace Rose {
namespace BinaryAnalysis {

/** Disassembler for nothing.
 *
 *  This disassembler is a stub that can be used when no ISA is specified.  It's never chosen automatically. It has unknown
 *  byte order. It always decodes to an unknown instruction that's one byte long. */
class DisassemblerNull: public Disassembler {

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Disassembler);
    }
#endif

public:
    DisassemblerNull();
    virtual ~DisassemblerNull();
    virtual Disassembler* clone() const ROSE_OVERRIDE;
    virtual bool canDisassemble(SgAsmGenericHeader*) const ROSE_OVERRIDE;
    virtual Unparser::BasePtr unparser() const ROSE_OVERRIDE;
    virtual SgAsmInstruction* disassembleOne(const MemoryMap::Ptr&, rose_addr_t va, AddressSet *successors = NULL) ROSE_OVERRIDE;
    virtual SgAsmInstruction* makeUnknownInstruction(const Exception&) ROSE_OVERRIDE;
};

} // namespace
} // namespace

#endif
#endif
