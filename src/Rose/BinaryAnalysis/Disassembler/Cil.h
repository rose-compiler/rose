#ifndef ROSE_BinaryAnalysis_Disassembler_Cil_H
#define ROSE_BinaryAnalysis_Disassembler_Cil_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Disassembler/Base.h>
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
    Cil();

public:
    /** Allocating constructor. */
    static Ptr instance();

    virtual ~Cil();

    virtual Base::Ptr clone() const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Public methods
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    virtual bool canDisassemble(SgAsmGenericHeader*) const override;

    virtual Unparser::BasePtr unparser() const override;

    virtual SgAsmInstruction* disassembleOne(const MemoryMap::Ptr&, rose_addr_t va,
                                             AddressSet* successors=nullptr) override;

    virtual SgAsmInstruction* makeUnknownInstruction(const Disassembler::Exception&) override;

private:
    SgAsmCilInstruction* makeUnknownInstruction(rose_addr_t address);
    SgAsmCilInstruction* makeInstruction(rose_addr_t, CilInstructionKind,
                                         const std::string&, SgAsmExpression* operand=nullptr) const;
    SgAsmCilInstruction* makeInstruction(rose_addr_t, SgUnsignedCharList& bytes/*in,out*/, size_t, CilInstructionKind,
                                         const std::string&, SgAsmExpression* operand=nullptr) const;

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
