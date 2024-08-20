#ifndef ROSE_BinaryAnalysis_InstructionSemantics_DispatcherMips_H
#define ROSE_BinaryAnalysis_InstructionSemantics_DispatcherMips_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Dispatcher.h>

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_member.hpp>
#endif

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {

namespace Mips {
    struct IP_lwl;
    struct IP_lwr;
}

/** Shared-ownership pointer to a MIPS instruction dispatcher. */
using DispatcherMipsPtr = boost::shared_ptr<class DispatcherMips>;

/** Dispatches MIPS instructions through the semantics layer.
 *
 *  The instruction is lowered to a set of a few dozen operations that operate on values and states in a semantic domain. */
class DispatcherMips: public BaseSemantics::Dispatcher {
public:
    /** Base type. */
    using Super = BaseSemantics::Dispatcher;

    /** Shared-ownership pointer. */
    using Ptr = DispatcherMipsPtr;

public:
    /** Cached register.
     *
     *  This register is cached so that there are not so many calls to look up registers by name.
     *
     * @{ */
    const RegisterDescriptor REG_R0, REG_R1, REG_HI, REG_LO, REG_PC, REG_SP, REG_FP, REG_RA;
    /** @} */

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void save(S &s, const unsigned /*version*/) const {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Super);
    }

    template<class S>
    void load(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Super);
        initializeDispatchTable();
        initializeMemoryState();
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER();
#endif

public:
    ~DispatcherMips();
protected:
    DispatcherMips() = delete;
    explicit DispatcherMips(const Architecture::BaseConstPtr&);
    DispatcherMips(const Architecture::BaseConstPtr&, const BaseSemantics::RiscOperatorsPtr&);

public:
    /** Construct a prototypical dispatcher.
     *
     *  The only thing this dispatcher can be used for is to create another dispatcher with the virtual @ref create method. */
    static Ptr instance(const Architecture::BaseConstPtr&);

    /** Constructor. */
    static Ptr instance(const Architecture::BaseConstPtr&, const BaseSemantics::RiscOperatorsPtr&);

    /** Dynamic cast to DispatcherMipsPtr with assertion. */
    static Ptr promote(const BaseSemantics::DispatcherPtr&);

public:
    // documented in the base class
    virtual BaseSemantics::DispatcherPtr create(const BaseSemantics::RiscOperatorsPtr&) const override;
    virtual int iprocKey(SgAsmInstruction*) const override;

private:
    // Initialize the dispatch table that handles each kind of instruction
    void initializeDispatchTable();

    // Initialize memory state, such as the default byte order
    void initializeMemoryState();


    // Merge unaligned memory with contents of register
    BaseSemantics::SValuePtr mergeLeft(BaseSemantics::SValuePtr reg, BaseSemantics::SValuePtr mem);
    BaseSemantics::SValuePtr mergeRight(BaseSemantics::SValuePtr reg, BaseSemantics::SValuePtr mem);

    // Classes that are allowed to mergeLeft and mergeRight
    friend struct Mips::IP_lwl;
    friend struct Mips::IP_lwr;
};

} // namespace
} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::InstructionSemantics::DispatcherMips);
#endif

#endif
#endif
