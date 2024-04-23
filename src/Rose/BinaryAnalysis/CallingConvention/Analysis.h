#ifndef ROSE_BinaryAnalysis_CallingConvention_Analysis_H
#define ROSE_BinaryAnalysis_CallingConvention_Analysis_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/CallingConvention/BasicTypes.h>

#include <Rose/BinaryAnalysis/RegisterParts.h>
#include <Rose/BinaryAnalysis/Variables.h>

#include <Sawyer/Optional.h>

namespace Rose {
namespace BinaryAnalysis {
namespace CallingConvention {

/** Function calling convention.
 *
 *  This class encapsulates all information about calling conventions including the analysis functions and the data types. */
class Analysis {
private:
    InstructionSemantics::BaseSemantics::DispatcherPtr cpu_;
    RegisterDictionaryPtr regDict_;                     // Names for the register parts
    DefinitionPtr defaultCc_;                           // Default calling convention for called functions

    bool hasResults_;                                   // Are the following data members initialized?
    bool didConverge_;                                  // Are the following data members valid (else only approximations)?
    RegisterParts restoredRegisters_;                   // Registers accessed but restored
    RegisterParts inputRegisters_;                      // Registers that serve as possible input parameters
    RegisterParts outputRegisters_;                     // Registers that hold possible return values
    Variables::StackVariables inputStackParameters_;    // Stack variables serving as function inputs
    Variables::StackVariables outputStackParameters_;   // Stack variables serving as possible return values
    Sawyer::Optional<int64_t> stackDelta_;              // Change in stack across entire function
    // Don't forget to update clearResults() and serialize() if you add more.

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(cpu_);
        s & BOOST_SERIALIZATION_NVP(regDict_);
        s & BOOST_SERIALIZATION_NVP(defaultCc_);
        s & BOOST_SERIALIZATION_NVP(hasResults_);
        s & BOOST_SERIALIZATION_NVP(didConverge_);
        s & BOOST_SERIALIZATION_NVP(restoredRegisters_);
        s & BOOST_SERIALIZATION_NVP(inputRegisters_);
        s & BOOST_SERIALIZATION_NVP(outputRegisters_);
        s & BOOST_SERIALIZATION_NVP(inputStackParameters_);
        s & BOOST_SERIALIZATION_NVP(outputStackParameters_);
        s & BOOST_SERIALIZATION_NVP(stackDelta_);
    }
#endif

public:
    /** Default constructor.
     *
     *  This creates an analyzer that is not suitable for analysis since it doesn't know anything about the architecture it
     *  would be analyzing. This is mostly for use in situations where an analyzer must be constructed as a member of another
     *  class's default constructor, in containers that initialize their contents with a default constructor, etc. */
    Analysis();
    ~Analysis();

    /** Construct an analyzer using a specified disassembler.
     *
     *  This constructor chooses a symbolic domain and a dispatcher appropriate for the disassembler's architecture. */
    explicit Analysis(const Disassembler::BasePtr&);

    /** Construct an analysis using a specified dispatcher.
     *
     *  This constructor uses the supplied dispatcher and associated semantic domain. For best results, the semantic domain
     *  should be a symbolic domain that uses @ref InstructionSemantics::BaseSemantics::MemoryCellList "MemoryCellList" and
     *  @ref InstructionSemantics::BaseSemantics::RegisterStateGeneric "RegisterStateGeneric". These happen to also be the
     *  defaults used by @ref InstructionSemantics::SymbolicSemantics. */
    explicit Analysis(const InstructionSemantics::BaseSemantics::DispatcherPtr&);

    /** Property: Default calling convention.
     *
     *  The data-flow portion of the analysis uses analysis results previously computed for called functions. If a called
     *  function has no previous analysis result then a default calling convention can be specified for this property and that
     *  convention's definition determines how the called function modifies the current function's data-flow state.
     *
     * @{ */
    DefinitionPtr defaultCallingConvention() const;
    void defaultCallingConvention(const DefinitionPtr&);
    /** @} */

    /** Analyze one function.
     *
     *  This analysis method uses @ref Partitioner2 data structures which are generally faster than using the AST. The
     *  specified function need not be attached to the partitioner. Results of the analysis are stored in this analysis
     *  object to be queried after the analysis completes. */
    void analyzeFunction(const Partitioner2::PartitionerConstPtr&, const Sawyer::SharedPointer<Partitioner2::Function>&);

    /** Whether a function has been analyzed.
     *
     *  Returns true if this analysis object holds results from analyzing a function. The results might be only approximations
     *  depending on whether @ref didConverge also returns true. */
    bool hasResults() const { return hasResults_; }

    /** Whether the analysis results are valid.
     *
     *  Returns true if @ref hasResults is true and the analysis converged to a solution.  If the analysis did not converge
     *  then the other results are only approximations. */
    bool didConverge() const { return didConverge_; }

    /** Clear analysis results.
     *
     *  Resets the analysis results so it looks like this analyzer is initialized but has not run yet. When this method
     *  returns, @ref hasResults and @ref didConverge will both retun false. */
    void clearResults();

    /** Clears everything but results.
     *
     *  This resets the virtual CPU to the null pointer, possibly freeing some memory if the CPU isn't being used for other
     *  things. Once the CPU is removed it's no longer possible to do more analysis. */
    void clearNonResults();

    /** Property: Register dictionary.
     *
     *  The register dictionary provides names for register parts.  If a dictionary is provided before @ref analyzeFunction is
     *  called then that dictionary is used if possible, otherwise @ref analyzeFunction assigns a new dictionary. In any case,
     *  this property is non-null after a call to @ref analyzeFunction.
     *
     * @{ */
    RegisterDictionaryPtr registerDictionary() const;
    void registerDictionary(const RegisterDictionaryPtr &d);
    /** @} */

    /** Callee-saved registers.
     *
     *  Returns the set of registers that are accessed by the function but which are also reset to their original values before
     *  the function returns. A callee-saved register will not be included in the set of input or output registers. */
    const RegisterParts& calleeSavedRegisters() const { return restoredRegisters_; }

    /** Input registers.
     *
     *  Returns the set of registers that the function uses as input values. These are the registers that the function reads
     *  without first writing and which the function does not restore before returning. */
    const RegisterParts& inputRegisters() const { return inputRegisters_; }

    /** Output registers.
     *
     *  Returns the set of registers into which the function writes potential return values.  These are the registers to which
     *  the function writes with no following read.  An output register is not also a calle-saved register. */
    const RegisterParts& outputRegisters() const { return outputRegisters_; }

    /** Input stack parameters.
     *
     *  Locations for stack-based parameters that are used as inputs to the function. */
    const Variables::StackVariables& inputStackParameters() const { return inputStackParameters_; }

    /** Output stack parameters.
     *
     *  Locations for stack-based parameters that are used as outputs of the function. */
    const Variables::StackVariables& outputStackParameters() const { return outputStackParameters_; }

    /** Concrete stack delta.
     *
     *  This is the amount added to the stack pointer by the function.  For caller-cleanup this is usually just the size of the
     *  non-parameter area (the return address). Stack deltas are measured in bytes. */
    Sawyer::Optional<int64_t> stackDelta() const { return stackDelta_; }

    /** Determine whether a definition matches.
     *
     *  Returns true if the specified definition is compatible with the results of this analysis. */
    bool match(const DefinitionPtr&) const;

    /** Find matching calling convention definitions.
     *
     *  Given an ordered list of calling convention definitons (a calling convention dictionary) return a list of definitions
     *  that is consistent with the results of this analysis.  The definitions in the returned list are in the same order as
     *  those in the specified dictionary. */
    Dictionary match(const Dictionary&) const;

    /** Print information about the analysis results.
     *
     *  The output is a single line of comma-separated values if @p multiLine is true. Otherwise, the top-level commas are
     *  replaced by linefeeds. */
    void print(std::ostream&, bool multiLine=false) const;

private:
    // Finish constructing
    void init(const Disassembler::BasePtr&);

    // Recompute the restoredRegisters_ data member.
    void updateRestoredRegisters(const InstructionSemantics::BaseSemantics::StatePtr &initialState,
                                 const InstructionSemantics::BaseSemantics::StatePtr &finalState);

    // Recompute the inputRegisters_ data member after updateRestoredRegisters is computed.
    void updateInputRegisters(const InstructionSemantics::BaseSemantics::StatePtr &state);

    // Recompute the outputRegisters_ data member after updateRestoredRegisters is computed.
    void updateOutputRegisters(const InstructionSemantics::BaseSemantics::StatePtr &state);

    // Recompute the input and output stack variables
    void updateStackParameters(const Partitioner2::FunctionPtr &function,
                               const InstructionSemantics::BaseSemantics::StatePtr &initialState,
                               const InstructionSemantics::BaseSemantics::StatePtr &finalState);

    // Recomputes the stack delta
    void updateStackDelta(const InstructionSemantics::BaseSemantics::StatePtr &initialState,
                          const InstructionSemantics::BaseSemantics::StatePtr &finalState);
};

} // namespace
} // namespace
} // namespace

#endif
#endif
