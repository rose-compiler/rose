#ifndef ROSE_BinaryAnalysis_ReturnValueUsed_H
#define ROSE_BinaryAnalysis_ReturnValueUsed_H
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <Partitioner2/BasicTypes.h>
#include <Partitioner2/ControlFlowGraph.h>

namespace Rose {
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
    bool didConverge_;                                  // Are the following data members valid (else only approximations)?
    std::vector<Partitioner2::Function::Ptr> callees_;
    RegisterParts returnRegistersUsed_;
    RegisterParts returnRegistersUnused_;

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S & s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(didConverge_);
        s & BOOST_SERIALIZATION_NVP(callees_);
        s & BOOST_SERIALIZATION_NVP(returnRegistersUsed_);
        s & BOOST_SERIALIZATION_NVP(returnRegistersUnused_);
    }
#endif

public:
    CallSiteResults()
        : didConverge_(false) {}

    /** Property: Did the analysis converge to a solution.
     *
     *  If the return value used analysis encountered an error then it returns an object whose didConverge property is
     *  false. This object might still contain some information about the call, but the information might not be complete or
     *  might be only an approximation.
     *
     * @{ */
    bool didConverge() const { return didConverge_; }
    void didConverge(bool b) { didConverge_ = b; }
    /** @} */

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
    bool assumeCallerReturnsValue_;

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(defaultCallingConvention_);
        s & BOOST_SERIALIZATION_NVP(callSites_);
        s & BOOST_SERIALIZATION_NVP(assumeCallerReturnsValue_);
    }
#endif

public:
    /** Default constructor.
     *
     *  This creates an analyzer that is not suitable for analysis since it doesn't know anything about the architecture it
     *  would be analyzing. This is mostly for use in situations where an analyzer must be constructed as a member of another
     *  class's default constructor, in containers that initialize their contents with default constructors, etc. */
    Analysis()
        : assumeCallerReturnsValue_(true) {}

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

    /** Property: Assume caller returns value(s).
     *
     *  If true, then assume that the caller returns a value(s) in the location(s) indicated by its primary calling convention
     *  definition.  This property affects whether a call to function B from A followed by a return from A implicitly uses the
     *  value returned from B.  For example, these GCC generates identical code for these two functions:
     *
     *  @code
     *  int test1(void) {
     *      return one();
     *  }
     *
     *  void test2(void) {
     *      one();
     *  }
     *  @endcode
     *
     *  Namely,
     *
     *  @code
     *  L1: push     ebp
     *      mov      ebp, esp
     *      call     function 0x080480de "one" ; returns value in EAX
     *      pop      ebp
     *      ret
     *  @endcode
     *
     *  If this property is set, then the analysis will indicate that the return value of the CALL instruction is used
     *  implicitly by the RET instruction (since there was no intervening write to EAX).  On the other hand, if this property
     *  is clear, then the analysis indicates that the call to function "one" returns a value in EAX which is unused.
     *
     * @{ */
    bool assumeCallerReturnsValue() const { return assumeCallerReturnsValue_; }
    void assumeCallerReturnsValue(bool b) { assumeCallerReturnsValue_ = b; }
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
#endif
