#ifndef ROSE_BinaryAnalysis_SmtSolver_H
#define ROSE_BinaryAnalysis_SmtSolver_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include <Rose/BinaryAnalysis/SymbolicExpression.h>
#include <Rose/Exception.h>
#include <Rose/Progress.h>

#include <boost/chrono.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
#include <boost/serialization/access.hpp>
#endif

#include <inttypes.h>
#include <unordered_map>

namespace Rose {
namespace BinaryAnalysis {

class CompareLeavesByName {
public:
    bool operator()(const SymbolicExpression::LeafPtr&, const SymbolicExpression::LeafPtr&) const;
};

class CompareRawLeavesByName {
public:
    bool operator()(const SymbolicExpression::Leaf*, const SymbolicExpression::Leaf*) const;
};

/** Interface to Satisfiability Modulo Theory (SMT) solvers.
 *
 *  The purpose of an SMT solver is to determine if an expression is satisfiable. Solvers are reference counted objects that
 *  are allocated with @c instance static methods or @c create virtual constructors and should not be explicitly deleted. */
class SmtSolver: private boost::noncopyable {
public:
    /** Ordered list of expressions. */
    using ExprList = std::vector<SymbolicExpression::Ptr>;

    /** Reference counting pointer for SMT solvers. */
    using Ptr = SmtSolverPtr;

    /** Solver availability map. */
    using Availability = std::map<std::string, bool>;

    /** Bit flags to indicate the kind of solver interface. */
    enum LinkMode {
        LM_NONE       = 0x0000,                         /**< No available linkage. */
        LM_LIBRARY    = 0x0001,                         /**< A runtime library is available. */
        LM_EXECUTABLE = 0x0002,                         /**< An executable is available. */
        LM_ANY        = 0x0003,                         /**< Any available mode. */
    };

    /** Type (sort) of expression.
     *
     *  ROSE uses bit constants "#b1" and "#b0" (in SMT-LIB syntax) to represent Boolean true and false, but most solvers
     *  distinguish between bit vector and Boolean types and don't allow them to be mixed (e.g., "(and #b1 true)" is an
     *  error). */
    enum Type { NO_TYPE, BOOLEAN, BIT_VECTOR, MEM_STATE };

    /** Maps expression nodes to term names.  This map is populated for common subexpressions. */
    using StringTypePair = std::pair<std::string, Type>;
    using TermNames = Sawyer::Container::Map<SymbolicExpression::Ptr, StringTypePair>;

    /** Maps one symbolic expression to another. */
    using ExprExprMap = Sawyer::Container::Map<SymbolicExpression::Ptr, SymbolicExpression::Ptr>;

    /** Exceptions for all things SMT related. */
    struct Exception: Rose::Exception {
        Exception(const std::string &mesg): Rose::Exception(mesg) {}
        ~Exception() throw () {}
    };

    /** Exception for parse errors when reading SMT solver output. */
    struct ParseError: Exception {
        ParseError(const std::pair<size_t /*line*/, size_t /*col*/> &loc, const std::string &mesg)
            : Exception("input line " + boost::lexical_cast<std::string>(loc.first+1) +
                        " column " + boost::lexical_cast<std::string>(loc.second+1) + ": " + mesg) {}
        ~ParseError() throw () {}
    };

    /** Satisfiability constants. */
    enum Satisfiable { SAT_NO=0,                        /**< Provably unsatisfiable. */
                       SAT_YES,                         /**< Satisfiable and evidence of satisfiability may be available. */
                       SAT_UNKNOWN                      /**< Could not be proved satisfiable or unsatisfiable. */
    };

    /** SMT solver statistics.
     *
     *  Solver statistics get accumulted into the class statistics only when the solver is destroyed or the solver's @ref
     *  resetStatistics method is invoked. */
    struct Stats {
        size_t ncalls = 0;                              /**< Number of times satisfiable() was called. */
        size_t input_size = 0;                          /**< Bytes of input generated for satisfiable(). */
        size_t output_size = 0;                         /**< Amount of output produced by the SMT solver. */
        size_t memoizationHits = 0;                     /**< Number of times memoization supplied a result. */
        size_t nSolversCreated = 0;                     /**< Number of solvers created. Only for class statistics. */
        size_t nSolversDestroyed = 0;                   /**< Number of solvers destroyed. Only for class statistics. */
        double prepareTime = 0.0;                       /**< Total time in seconds spent creating assertions before solving. */
        double longestPrepareTime = 0.0;                /**< Longest of times added to prepareTime. */
        double solveTime = 0.0;                         /**< Total time in seconds spent in solver's solve function. */
        double longestSolveTime = 0.0;                  /**< Longest of times added to the solveTime total. */
        double evidenceTime = 0.0;                      /**< Total time in seconds to retrieve evidence of satisfiability. */
        double longestEvidenceTime = 0.0;               /**< Longest of times added to evidenceTime. */
        size_t nSatisfied = 0;                          /**< Number of times the solver returned "satisified". */
        size_t nUnsatisfied = 0;                        /**< Number of times the solver returned "unsatisfied". */
        size_t nUnknown = 0;                            /**< Number of times the solver returned "unknown". */
        // Remember to add all data members to SmtSolver::resetStatistics() and SmtSolver::Stats::print()

        void print(std::ostream&, const std::string &prefix = "") const;
    };

    /** RAII guard for solver stack.
     *
     *  This object implements a rudimentary form of SMT transactions. The constructor starts a new transaction by pushing
     *  a new level onto the specified solver (if the solver is non-null). The destructor pops transactions until it gets
     *  back to the same number of levels as originally. It is undefined behavior if the solver has fewer transactions than
     *  originally when the destructor is called. If @ref commit has been called on this object then the destructor does
     *  nothing.
     *
     *  This guard object makes no attempt to ensure that after popping levels we end up at the same level we started at.
     *  In other words, although the number of levels is back to where we started, it might have been possible that between
     *  the constructor and destructor we popped past this transaction and then pushed new transactions to replace it with
     *  a different transaction. */
    class Transaction {
        SmtSolver::Ptr solver_;
        size_t nLevels_;
        bool committed_;
    public:
        /** Constructor pushes level if solver is non-null.
         *
         *  It is safe to call this with a null solver. */
        explicit Transaction(const SmtSolver::Ptr &solver)
            : solver_(solver), committed_(false) {
            if (solver) {
                nLevels_ = solver->nLevels();
                solver->push();
            } else {
                nLevels_ = 0;
            }
        }

        /** Destructor pops level unless canceled. */
        ~Transaction() {
            if (solver_ && !committed_) {
                ASSERT_require2(solver_->nLevels() > nLevels_, "something popped this transaction already");
                while (solver_->nLevels() > nLevels_) {
                    if (solver_->nLevels() > 1) {
                        solver_->pop();
                    } else {
                        solver_->reset();
                    }
                }
            }
        }

        /** Cancel the popping during the destructor. */
        void commit(bool b = true) {
            committed_ = b;
        }

        /** Whether the guard is canceled. */
        bool isCommitted() const {
            return committed_;
        }

        /** Solver being protected. */
        SmtSolver::Ptr solver() const {
            return solver_;
        }
    };

    /** Set of variables. */
    using VariableSet = Sawyer::Container::Set<SymbolicExpression::LeafPtr, CompareLeavesByName>;

    using Definitions = std::set<uint64_t>;             /**< Free variables that have been defined. */

    /** S-Expr parsed from SMT solver text output. */
    class SExpr: public Sawyer::SmallObject, public Sawyer::SharedObject {
    public:
        using Ptr = Sawyer::SharedPointer<SExpr>;
    private:
        explicit SExpr(const std::string &content): content_(content) {}
        std::string content_;
        std::vector<Ptr> children_;
    public:
        static Ptr instance(const std::string &content); // leaf node
        static Ptr instance(size_t);                    // integer leaf node
        static Ptr instance(const Ptr &a = Ptr(), const Ptr &b = Ptr(), const Ptr &c = Ptr(), const Ptr &d = Ptr());

        const std::string name() const { return content_; }
        const std::vector<Ptr>& children() const { return children_; }
        std::vector<Ptr>& children() { return children_; }
        void append(const std::vector<Ptr>&);
        void print(std::ostream&) const;
    };

    using SExprTypePair = std::pair<SExpr::Ptr, Type>;

    /** Memoizes calls to an SMT solver.
     *
     *  This class memoizes calls to the @c check function for a particular SMT solver, or across some collection of solvers. For
     *  every non-trivial call to @c check, the memoizer caches the input set of assertions and the output satisfiability and, if
     *  satisfiable, the output evidence of satisfiability. Moreover, it does this in such a way that assertions that only vary in
     *  their variable names can be matched to previously cached calls.
     *
     *  In order to use memoization, a non-null memoizer should be passed to the @ref SmtSolver::instance factory, or a non-null
     *  memoizer pointer should be assigned to the @c memoizer property of the @ref SmtSolver. To disable memoization, assign
     *  a null pointer to the @c memoizer property.  Multiple @ref SmtSolver objects can share the same memoizer, but this should
     *  generally only be done when all such solvers are guaranteed to return the same values for any given inputs, which is not
     *  always the case due to differences in implementation.
     *
     *  In order to match calls whose inputs vary only in order of assertions and/or variable names, the assertions are sorted and
     *  normalized before being stored in the cache. First, to sort the assertions each assertion is independently normalized by
     *  renaming its variables starting at "v0" in the order they're encountered in a depth-first-search traversal. Then the input
     *  assertions are sorted according to the hashes of their corresponding normalized versions. Finally, the variables in the
     *  sorted assertions are collectively renamed, producing a sorted-normalized set of assertions and a renaming map.  When a new
     *  result is added to the cache, the variables in the evidence of satisfiability are renamed according to renaming map. The
     *  original assertions, original evidence of satisfiability, and renaming map are discarded.  When a cache hit occurs and the
     *  evidence needs to be returned, the cached evidence is de-normalized using the inverse of the temporary renaming map for the
     *  current input assertions.
     *
     *  Thread safety: All member functions are thread safe unless otherwise noted. */
    class Memoizer: public Sawyer::SharedObject {
    public:
        /** Reference counting pointer. */
        using Ptr = Sawyer::SharedPointer<Memoizer>;

        /* Return value from @ref find function. */
        struct Found {
            Sawyer::Optional<Satisfiable> satisfiable;      /**< If found, whether satisfiable. */
            ExprList sortedNormalized;                      /**< Sorted and normalized assertions, regardless if found. */
            SymbolicExpression::ExprExprHashMap rewriteMap; /**< Mapping from provided to sortedNormalized assertions. */
            ExprExprMap evidence;                           /**< Normalized evidence if found and satisfiable. */

            /** True if lookup was a cache hit. */
            explicit operator bool() const {
                return satisfiable;
            }
        };

    private:
        using ExprExpr = std::pair<SymbolicExpression::Ptr, SymbolicExpression::Ptr>;

        // The thing that is memoized
        struct Record {
            ExprList assertions;                        // "find" inputs, sorted and normalized
            Satisfiable satisfiable;                    // main output of SMT solver
            ExprExprMap evidence;                       // output if satisfied: sorted and normalized
        };

        // Mapping from hash of sorted-normalized assertions to the memoization record.
        using Map = std::multimap<SymbolicExpression::Hash, Record>;

    private:
        mutable SAWYER_THREAD_TRAITS::Mutex mutex_;     // protects the following data members
        Map map_;                                       // memoization records indexed by hash of sorted-normalized assertions

    protected:
        Memoizer() {}

    public:
        /** Allocating constructor. */
        static Ptr instance();

        /** Clear the entire cache as if it was just constructed. */
        void clear();

        /** Search for the specified assertions in the cache.
         *
         *  If this is a cache hit, then the return value evaluates to true in Boolean context and contains the satisfiability and
         *  normalized evidence. The evidence needs to be de-normalized by the @ref evidence function before it can be used.  If
         *  this is a cache miss, then the return value evaluates to false in Boolean context, but it contains enough information
         *  for the SMT solver results to be inserted into the cache later by the @ref insert function.
         *
         *  The documentation for this class describes how the search works by sorting and normalizing the input assertions. */
        Found find(const ExprList &assertions);

        /** Returns evidence of satisfiability.
         *
         *  The argument is the result from @ref find. If the argument evaluates to true in Boolean context (i.e., the @ref find
         *  was a cache hit) then this function will de-normalize the cached evidence of satisfiability and return it. This function
         *  should not be called if the argument evaluates to false in a Boolean context (i.e., the @ref find was a cache miss). */
        ExprExprMap evidence(const Found&) const;

        /** Insert a call record into the cache.
         *
         *  The first argument is the return value from @ref find which must have been a cache miss.  The remaining arguments are
         *  the results from the SMT solver for the same assertions that were used in the @ref find call. The evidence is normalized
         *  using the same variable mapping as was used for the input assertions during the @ref find call, and then stored in the
         *  cache in normalized form. */
        void insert(const Found&, Satisfiable, const ExprExprMap &evidence);

        /** Number of call records cached. */
        size_t size() const;

    public:
        // Non-synchronized search for the sorted-normalized assertions which have the specified hash.
        Map::iterator searchNS(SymbolicExpression::Hash, const ExprList &sortedNormalized);
    };

private:
    std::string name_;
    std::vector<ExprList> stack_;
    bool errorIfReset_;

protected:
    LinkMode linkage_;
    std::string outputText_;                            /**< Additional output obtained by satisfiable(). */
    std::vector<SExpr::Ptr> parsedOutput_;              // the evidence output
    ExprExprMap evidence_;                              // evidence for last check() if satisfiable
    TermNames termNames_;                               // maps ROSE exprs to SMT exprs and their basic type
    Memoizer::Ptr memoizer_;                            // cache of previously computed results
    Progress::Ptr progress_;                            // optional progress reporting

    // Statistics
    static boost::mutex classStatsMutex;
    static Stats classStats;                            // all access must be protected by classStatsMutex
    Stats stats;

public:
    /** Diagnostic facility. */
    static Sawyer::Message::Facility mlog;

private:
#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(name_);
        s & BOOST_SERIALIZATION_NVP(stack_);
        // linkage_                  -- not serialized
        // termNames_                -- not serialized
        // outputText_               -- not serialized
        // parsedOutput_             -- not serialized
        // termNames_                -- not serialized
        // memoizer_                 -- not serialized
        // classStatsMutex           -- not serialized
        // classStats                -- not serialized
        // stats                     -- not serialized
        // mlog                      -- not serialized
    }
#endif

protected:
    /** Construct with name and linkage.
     *
     *  Every solver should have a @p name that will appear in diagnostic messages, such as "z3", and a linkage mode that
     *  describes how ROSE communicates with the solver. The linkage mode is chosen as the least significant set bit of @p
     *  linkages, therefore the subclass should ensure that @p linkages contains only valid bits. If @p linkages is zero then
     *  the constructed object will be useless since it has no way to communicate with the solver. You can check for this
     *  situation by reading the @p linkage property, or just wait for one of the other methods to throw an @ref
     *  SmtSolver::Exception. */
    SmtSolver(const std::string &name, unsigned linkages)
        : name_(name), errorIfReset_(false), linkage_(LM_NONE) {
        init(linkages);
    }
    
public:
    /** Virtual constructor.
     *
     *  The new solver will have the same settings as the source solver. */
    virtual Ptr create() const = 0;

    // Solvers are reference counted and should not be explicitly deleted.
    virtual ~SmtSolver();


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Methods for creating solvers.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Availability of all known solvers.
     *
     *  Returns a map whose keys are the names of the SMT solver APIs and whose value is true if the solver is avilable or
     *  false if not available. */
    static Availability availability();

    /** Allocate a new solver by name.
     *
     *  Create a new solver using one of the names returned by @ref availability. The special name "" means no solver (return
     *  null) and "best" means return @ref bestAvailable (which might also be null). It may be possible to create solvers by
     *  name that are not available, but attempting to use such a solver will fail loudly by calling @ref requireLinkage. If an
     *  invalid name is supplied then an @ref SmtSolver::Exception is thrown. */
    static Ptr instance(const std::string &name);

    /** Best available solver.
     *
     *  Returns a new solver, an instance of the best available solver. If no solver is possible then returns null. */
    static Ptr bestAvailable();

    /** Property: Name of solver for debugging.
     *
     *  This name gets printed in various diagnostic messages. It's initialized to something reasonable by constructors, but
     *  can be changed at any time by the user.
     *
     * @{ */
    const std::string& name() const { return name_; }
    void name(const std::string &s) { name_ = s; }
    /** @} */

    /** Property: How ROSE communicates with the solver.
     *
     *  The linkage is set when the solver object is created, and is read-only. */
    LinkMode linkage() const {
        return linkage_;
    }

    /** Assert required linkage.
     *
     *  If the specified linkage is not available, then throw an exception. */
    void requireLinkage(LinkMode) const;

    /** Given a bit vector of linkages, return the best one.
     *
     *  "Best" is defined as that with the best performance, which is usually direct calls to the solver's API. */
    static LinkMode bestLinkage(unsigned linkages);

    /** Property: Memoizer.
     *
     *  The value of this property is a pointer to the object that caches the memoization for this solver. Setting it to a
     *  non-null pointer turns on memoization (using that object) and setting it to a null pointer turns off
     *  memoization. Memoization can be turned on or off at any time, and multiple SMT solvers can share the same memoizer.
     *
     * @{ */
    Memoizer::Ptr memoizer() const;
    void memoizer(const Memoizer::Ptr&);
    /** @} */

    /** Set the timeout for the solver.
     *
     *  This sets the maximum time that the solver will try to find a solution before returning "unknown". */
    virtual void timeout(boost::chrono::duration<double> seconds) = 0;

    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // High-level abstraction for testing satisfiability.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Determines if expressions are trivially satisfiable or unsatisfiable.
     *
     *  If all expressions are known 1-bit values that are true, then this function returns SAT_YES.  If any expression is a
     *  known 1-bit value that is false, then this function returns SAT_NO.  Otherwise this function returns SAT_UNKNOWN.
     *
     *  This is a high-level abstraction that resets this object state so it contains a single assertion set on its stack, and
     *  clears evidence of satisfiability. */
    virtual Satisfiable triviallySatisfiable(const ExprList &exprs);

    /** Determines if the specified expressions are all satisfiable, unsatisfiable, or unknown.
     *
     *  This is a high-level abstraction that starts a new SMT solver session. For text-based interfaces, this solver object is
     *  reset, a temporary text file is created, the solver is run with the file as input, text output is read, and the
     *  evidence of satisfiability is parsed and stored in this object.
     *
     * @{ */
    virtual Satisfiable satisfiable(const SymbolicExpression::Ptr&);
    virtual Satisfiable satisfiable(const ExprList&);
    /** @} */


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Mid-level abstractions for testing satisfiaiblity.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Reset solver state.
     *
     *  Resets the solver to an initial state containing no assertions.  The evidence of satisfiability is cleared. For
     *  API-based solvers, this function might also create a new solver and/or solver context. */
    virtual void reset();

    /** Property: Throw an exception if the solver is reset.
     *
     *  This is used mostly for debugging solvers that are intending to use transactions. If the solver is ever reset, say by
     *  accidentally invoking its @ref satisfiable method, then an exception is thrown.
     *
     * @{ */
    bool errorIfReset() const {
        return errorIfReset_;
    }
    void errorIfReset(bool b) {
        errorIfReset_ = b;
    }
    /** @} */

    /** Create a backtracking point.
     *
     *  Pushes a new, empty set of assertions onto the solver stack.
     *
     *  Note that although text-based solvers (executables) accept push and pop methods, they have no effect on the speed of
     *  the solver because ROSE invokes the executable in batch mode. In this case the push and pop apply to the stack within
     *  this solver object in ROSE.
     *
     *  See also, @ref pop. */
    virtual void push();

    /** Pop a backtracking point.
     *
     *  Pops the top set of assertions from the solver stack. It is not legal to call @ref pop when the assertion stack is only
     *  one level deep; use @ref reset in that case instead.
     *
     *  See also, @ref push and @ref reset. */
    virtual void pop();

    /** Number of backtracking levels.
     *
     *  This is the number of sets of assertions. The @ref push and @ref pop increment and decrement this number. The return
     *  valued is always positive. */
    virtual size_t nLevels() const { return stack_.size(); }

    /** Number of assertions at a specific backtracking level.
     *
     *  Backtracking levels are numbered starting at zero up to one less than the value returned by @ref nLevels. */
    virtual size_t nAssertions(size_t backtrackingLevel);

    /** Total number of assertions across all backtracking levels. */
    virtual size_t nAssertions() const;
    
    /** Insert assertions.
     *
     *  Inserts assertions into the set of assertions at the top of the backtracking stack.
     *
     * @{ */
    virtual void insert(const SymbolicExpression::Ptr&);
    virtual void insert(const ExprList&);
    /** @} */

    /** All assertions.
     *
     *  Returns the list of all assertions across all backtracking points. */
    virtual ExprList assertions() const;

    /** Assertions for a particular level.
     *
     *  Returns the assertions associated with a particular level of the stack. Level zero is the oldest entry in the stack;
     *  all smt objects have a level zero. See also, @ref nLevels. */
    virtual const ExprList& assertions(size_t level) const;

    /** Check satisfiability of current stack.
     *
     *  Checks whether all assertions in the entire stack of assertion sets are satisfiable.  A set of no assertions is
     *  trivially satisfiable. Errors are emitted on @ref mlog once per error message and returned as @c SAT_UNKNOWN. */
    virtual Satisfiable check();

    /** Check whether the stack of assertions is trivially satisfiable.
     *
     *  This function returns true if all assertions have already been simplified in ROSE to the single bit "1", and returns
     *  true if so; false otherwise. If no assertions are present, this function returns true.  This function does not invoke
     *  any functions in the underlying SMT solver. */
    virtual Satisfiable checkTrivial();


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // High-level abstraction for solver results.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Evidence of satisfiability. */
    using Evidence = Sawyer::Container::Map<std::string /*variable name*/, SymbolicExpression::Ptr /*value*/>;

    /** Names of items for which satisfiability evidence exists.
     *
     *  Returns a vector of strings (variable names or memory addresses) that can be passed to @ref evidenceForName.  Not all
     *  SMT solvers can return this information, in which case they return an empty vector.
     *
     *  The returned names are only for those variables and addresses whose evidence of satisfiability can be parsed by
     *  ROSE. The subclasses provide additional methods for obtaining more detailed information. */
    virtual std::vector<std::string> evidenceNames() const;

    /** Evidence of satisfiability for a variable or memory address.
     *
     *  If the string starts with the letter 'v' then variable evidence is returned, otherwise the string must be an address.
     *  Valid strings are those returned by the @ref evidenceNames method; other strings result in a null return
     *  value. Subclasses might define additional methods for obtaining evidence of satisfiability. */
    virtual SymbolicExpression::Ptr evidenceForName(const std::string&) const;

    /** All evidence of satisfiability.
     *
     *  The version that returns a map indexed by variable name is the same as calling @ref evidenceNames and then @ref
     *  evidenceForName for each of those names.
     *
     *  The version that returns a map indexed by symbolic expression returns all the evidence in symbolic form. The keys for
     *  that return value are symbolic expressions that are simply variables.
     *
     *  Evidence is only returned if the previous check was satisfiable. Otherwise the return value is an empty map.
     *
     * @{ */
    Evidence evidenceByName() const;
    ExprExprMap evidence() const;
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Mid-level abstraction for solver results.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Clears evidence information.
     *
     *  Evidence of satisfiability is cleared by calling this function or by calling any function that changes the state of the
     *  solver, such as pushing or popping assertion sets, inserting new assertions, or checking satisfiability. Checking
     *  satisfiability re-initializes the evidence. */
    virtual void clearEvidence();

    /** Evidence of satisfiability for a bitvector variable.
     *
     *  If an expression is satisfiable, this function will return a value for the specified bitvector variable that satisfies
     *  the expression in conjunction with the other evidence. Not all SMT solvers can return this information.  Returns the
     *  null pointer if no evidence is available for the variable.
     * @{ */
    virtual SymbolicExpression::Ptr evidenceForVariable(const SymbolicExpression::Ptr &var) {
        SymbolicExpression::LeafPtr ln = var->isLeafNode();
        ASSERT_require(ln && ln->isVariable2());
        return evidenceForVariable(ln->nameId());
    }
    virtual SymbolicExpression::Ptr evidenceForVariable(uint64_t varno) {
        char buf[64];
        snprintf(buf, sizeof buf, "v%" PRIu64, varno);
        return evidenceForName(buf);
    }
    /** @} */

    /** Evidence of satisfiability for a memory address.
     *
     *  If an expression is satisfiable, this function will return a value for the specified memory address that satisfies the
     *  expression in conjunction with the other evidence. Not all SMT solvers can return this information. Returns the null
     *  pointer if no evidence is available for the memory address. */
    virtual SymbolicExpression::Ptr evidenceForAddress(uint64_t addr);


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Statistics
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Property: Statistics for this solver.
     *
     *  The statistics are not reset by this call, but continue to accumulate. */
    const Stats& statistics() const { return stats; }

    /** Property: Statistics across all solvers.
     *
     *  The class statistics are updated whenever a solver is destroyed or its @ref resetStatistics method is invoked. However,
     *  the nSolversCreated member is updated as soon as a solver is created.
     *
     *  The statistics are not reset by this call, but continue to accumulate. */
    static Stats classStatistics();

    /** Resets statistics for this solver. */
    void resetStatistics();

    /** Resets statistics for the class.
     *
     *  Statistics are reset to initial values for the class as a whole.  Resetting statistics for the class does not affect
     *  statistics of any particular SMT solver object. */
    static void resetClassStatistics();

    /** Progress reporting object.
     *
     *  If non-null, certain types of actions are reported to this progress object by pushing subtask progress objects.
     *
     * @{ */
    Progress::Ptr progress() const;
    void progress(const Progress::Ptr &progress);
    /** @} */


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Low-level API used in subclasses and sometimes for debugging
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Generates an input file for for the solver.
     *
     *  Usually the input file will be SMT-LIB format, but subclasses might override this to generate some other kind of
     *  input. Throws Excecption if the solver does not support an operation that is necessary to determine the
     *  satisfiability.
     *
     *  This function is also useful for debugging because it will convert ROSE's symbolic expressions to whatever format
     *  is used by the SMT solver. */
    virtual void generateFile(std::ostream&, const ExprList &exprs, Definitions*) = 0;

protected:
    /** Check satisfiability using text files and an executable. */
    virtual Satisfiable checkExe();

    /** Check satisfiability using a library API. */
    virtual Satisfiable checkLib();

    /** Error message from running a solver executable.
     *
     *  Given the solver exit status and (implicitly) the output of the solver, either return an error message or the empty
     *  string. This can be overridden by subclasses because some solvers exit with non-zero status if you try to get the model
     *  when (check-sat) returns not-satisfiable. */
    virtual std::string getErrorMessage(int exitStatus);

    /** Return all variables that need declarations. */
    virtual void findVariables(const SymbolicExpression::Ptr&, VariableSet&) {}

    /** Print an S-Expr for debugging.
     *
     *  A null pointer is printed as "nil" and an empty list is printed as "()" in order to distinguish the two cases. There
     *  should be no null pointers though in well-formed S-Exprs. */
    static void printSExpression(std::ostream&, const SExpr::Ptr&);

    /** Given the name of a configuration file, return the command that is needed to run the solver. The first line
     *  of stdout emitted by the solver should be the word "sat" or "unsat". */
    virtual std::string getCommand(const std::string &config_name) = 0;

    /** Parse all SExprs from the specified string. */
    std::vector<SExpr::Ptr> parseSExpressions(const std::string&);

    /** Parses evidence of satisfiability.  Some solvers can emit information about what variable bindings satisfy the
     *  expression.  This information is parsed by this function and added to a mapping of variable to value. */
    virtual void parseEvidence() {};

    /** Normalize expressions by renaming variables.
     *
     *  This is used during memoization to rename all the variables. It performs a depth-first search and renames each variable
     *  it encounters. The variables are renumbered starting at zero.  The return value is a vector new new expressions, some
     *  of which may be the unmodified original expressions if there were no variables.  The @p index is also a return value
     *  which indicates how original variables were mapped to new variables. */
    static ExprList normalizeVariables(const ExprList&, SymbolicExpression::ExprExprHashMap &index /*out*/);

    /** Undo the normalizations that were performed earlier.
     *
     *  Each of the specified expressions are rewritten by undoing the variable renaming that was done by @ref
     *  normalizeVariables. The @p index is the same index as returned by @ref normalizeVariables, although the input
     *  expressions need not be those same expressions. For each input expression, the expression is rewritten by substituting
     *  the inverse of the index. That is, a depth first search is performed on the expression and if the subexpression matches
     *  a value of the index, then it's replaced by the corresponding key. */
    static ExprList undoNormalization(const ExprList&, const SymbolicExpression::ExprExprHashMap &index);


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Miscellaneous
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Unit tests. */
    virtual void selfTest();

    /** Initialize diagnostic output facilities.
     *
     *  Called when the ROSE library is initialized. */
    static void initDiagnostics();

private:
    void init(unsigned linkages);                       // Called during construction
};

std::ostream& operator<<(std::ostream&, const SmtSolver::SExpr&);

} // namespace
} // namespace

#endif
#endif
