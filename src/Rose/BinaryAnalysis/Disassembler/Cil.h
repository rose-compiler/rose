#ifndef ROSE_BinaryAnalysis_Disassembler_Cil_H
#define ROSE_BinaryAnalysis_Disassembler_Cil_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Disassembler/Base.h>

#include <Rose/BinaryAnalysis/Architecture/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionEnumsCil.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace Disassembler {

/** CIL Disassembler.
 *
 *  This disassembler decodes CIL instructions.
 */
class Cil: public Base {
public:
    /** Reference counting pointer. */
    using Ptr = Sawyer::SharedPointer<Cil>;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
    explicit Cil(const Architecture::BaseConstPtr&);

public:
    /** Allocating constructor. */
    static Ptr instance(const Architecture::BaseConstPtr&);

    virtual ~Cil();

    virtual Base::Ptr clone() const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Public methods
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    virtual SgAsmInstruction* disassembleOne(const MemoryMap::Ptr&, rose_addr_t va,
                                             AddressSet* successors=nullptr) override;

    virtual SgAsmInstruction* makeUnknownInstruction(const Disassembler::Exception&) override;

private:
    SgAsmCilInstruction* makeUnknownInstruction(rose_addr_t va, SgUnsignedCharList &bytes, size_t size);
    SgAsmCilInstruction* makeInstruction(rose_addr_t, SgUnsignedCharList& bytes/*in,out*/, size_t, CilInstructionKind,
                                         SgAsmExpression* operand=nullptr) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Serialization
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
      void serialize(S &s, const unsigned /*version*/) {
          s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Disassembler::Base);
    }
#endif
};

} // namespace
} // namespace
} // namespace

#endif
#endif
