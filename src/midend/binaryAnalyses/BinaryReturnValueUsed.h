#ifndef ROSE_BinaryAnalysis_ReturnValueUsed_H
#define ROSE_BinaryAnalysis_ReturnValueUsed_H

#include <Partitioner2/BasicTypes.h>
#include <Partitioner2/ControlFlowGraph.h>

namespace rose {
namespace BinaryAnalysis {

/** Contains functions that analyze whether a function returns a value which is used by the caller.
 *
 *  Currently only able to handle 32-bit x86 code that returns values via one of the AX registers. [Robb P. Matzke 2014-02-18] */
namespace ReturnValueUsed {

/** Initialize diagnostics.
 *
 *  This is normally called as part of ROSE's diagnostics initialization,  but it doesn't hut to call it often. */
void initDiagnostics();

/** Facility for diagnostic output.
 *
 *  The facility can be controlled directly or via ROSE's command-line. */
extern Sawyer::Message::Facility mlog;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Miscellaneous small types
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CallSiteResults {
    std::vector<Partitioner2::Function::Ptr> callees_;
    RegisterParts returnRegistersUsed_;
    RegisterParts returnRegistersUnused_;

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S & s, const unsigned version) {
        s & BOOST_SERIALIZATION_NVP(callees_);
        s & BOOST_SERIALIZATION_NVP(returnRegistersUsed_);
        s & BOOST_SERIALIZATION_NVP(returnRegistersUnused_);
    }
#endif

public:
    /** Property: Functions called at this site. */
    const std::vector<Partitioner2::Function::Ptr> callees() const { return callees_; }

    /** Property: Return registers that are used in the caller.
     *
     *  These represent the values returned by the callee in registers which are subsequently used in the caller.
     *
     * @{ */
    const RegisterParts& returnRegistersUsed() const { return returnRegistersUsed_; }
    RegisterParts& returnRegistersUsed() { return returnRegistersUsed_; }
    void returnRegistersUsed(const RegisterParts &regs) { returnRegistersUsed_ = regs; }
    /** @} */

    /** Property: Return registers that are unused in the caller.
     *
     *  These represent the values returned by the callee in registers which are not subsequently used in the caller.
     *
     * @{ */
    const RegisterParts& returnRegistersUnused() const { return returnRegistersUnused_; }
    RegisterParts& returnRegistersUnused() { return returnRegistersUnused_; }
    void returnRegistersUnused(const RegisterParts &regs) { returnRegistersUnused_ = regs; }
    /** @} */

private:
    friend class Analysis;
    void callees(const std::vector<Partitioner2::Function::Ptr> &functions) { callees_ = functions; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Main analysis class
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Analysis {
private:
    typedef Sawyer::Container::Map<rose_addr_t /*call_site*/, CallSiteResults> CallSiteMap;

private:
    CallingConvention::Definition::Ptr defaultCallingConvention_;
    CallSiteMap callSites_;

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned version) {
        s & BOOST_SERIALIZATION_NVP(defaultCallingConvention_);
        s & BOOST_SERIALIZATION_NVP(callSites_);
    }
#endif

public:
    /** Default constructor.
     *
     *  This creates an analyzer that is not suitable for analysis since it doesn't know anything about the architecture it
     *  would be analyzing. This is mostly for use in situations where an analyzer must be constructed as a member of another
     *  class's default constructor, in containers that initialize their contents with default constructors, etc. */
    Analysis() {}

    /** Property: Default calling convention.
     *
     *  Use this calling convention any time we encounter a function that doesn't have a known calling convention.  This is a
     *  calling convention definition that's used during data-flow. Even if the default calling convention is specified, the
     *  functions being analyzed (that function which is called at the analyzed call site) must still have already had a
     *  calling convention analysis run and have calling convention charactertics. The characterstics describe, among other
     *  things, which register and memory locations are <em>actually</em> outputs, whereas a calling convention definition only
     *  states which locations are <em>permitted</em> to be outputs.
     *
     * @{ */
    CallingConvention::Definition::Ptr defaultCallingConvention() const { return defaultCallingConvention_; }
    void defaultCallingConvention(const CallingConvention::Definition::Ptr &defn) { defaultCallingConvention_ = defn; }
    /** @} */
    
    /** Clear analysis results.
     *
     *  Resets the analysis results so it looks like this analyzer is initialized but has not run yet. When this method
     *  returns, @ref hasResults and @ref didConverge will both retun false. */
    void clearResults();

    /** Analyze a call site to see if a function's return value is used. */
    CallSiteResults analyzeCallSite(const Partitioner2::Partitioner&, const Partitioner2::ControlFlowGraph::ConstVertexIterator&);

private:
    // Given a control flow graph vertex, if that vertex is a function call basic block then return a list of the known, called
    // functions.
    std::vector<Partitioner2::Function::Ptr>
    findCallees(const Partitioner2::Partitioner&, const Partitioner2::ControlFlowGraph::ConstVertexIterator &callSite);

    // Given a control flow graph vertex, if that vertex is a function call basic block then return a list of all vertices that
    // are known call-return points for that call.
    std::vector<Partitioner2::ControlFlowGraph::ConstVertexIterator>
    findReturnTargets(const Partitioner2::Partitioner&, const Partitioner2::ControlFlowGraph::ConstVertexIterator &callSite);
};

} // namespace
} // namespace
} // namespace

#endif
