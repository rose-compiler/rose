#ifndef Rose_BinaryAnalysis_SmtSolver_H
#define Rose_BinaryAnalysis_SmtSolver_H

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include <BinarySymbolicExpr.h>
#include <boost/lexical_cast.hpp>
#include <boost/serialization/access.hpp>
#include <boost/thread/mutex.hpp>
#include <inttypes.h>

namespace Rose {
namespace BinaryAnalysis {

/** Interface to Satisfiability Modulo Theory (SMT) solvers.
 *
 *  The purpose of an SMT solver is to determine if an expression is satisfiable. */
class SmtSolver {
public:
    /** Solver availability map. */
    typedef std::map<std::string, bool> Availability;

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
    typedef std::pair<std::string, Type> StringTypePair;
    typedef Sawyer::Container::Map<SymbolicExpr::Ptr, StringTypePair> TermNames;

    /** Maps one symbolic expression to another. */
    typedef Sawyer::Container::Map<SymbolicExpr::Ptr, SymbolicExpr::Ptr> ExprExprMap;

    /** Exceptions for all things SMT related. */
    struct Exception: std::runtime_error {
        Exception(const std::string &mesg): std::runtime_error(mesg) {}
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

    /** SMT solver statistics. */
    struct Stats {
        Stats(): ncalls(0), input_size(0), output_size(0) {}
        size_t ncalls;                                  /**< Number of times satisfiable() was called. */
        size_t input_size;                              /**< Bytes of input generated for satisfiable(). */
        size_t output_size;                             /**< Amount of output produced by the SMT solver. */
    };

    /** Set of variables. */
    typedef Sawyer::Container::Set<SymbolicExpr::LeafPtr> VariableSet;

    typedef std::set<uint64_t> Definitions;             /**< Free variables that have been defined. */

    /** S-Expr parsed from SMT solver text output. */
    class SExpr: public Sawyer::SmallObject, public Sawyer::SharedObject {
    public:
        typedef Sawyer::SharedPointer<SExpr> Ptr;
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

    typedef std::pair<SExpr::Ptr, Type> SExprTypePair;

private:
    std::string name_;
    std::vector<std::vector<SymbolicExpr::Ptr> > stack_;

protected:
    LinkMode linkage_;
    std::string outputText_;                            /**< Additional output obtained by satisfiable(). */
    std::vector<SExpr::Ptr> parsedOutput_;              // the evidence output
    TermNames termNames_;                               // maps ROSE exprs to SMT exprs and their basic type


    // Statistics
    static boost::mutex classStatsMutex;
    static Stats classStats;                            // all access must be protected by classStatsMutex
    Stats stats;

public:
    /** Diagnostic facility. */
    static Sawyer::Message::Facility mlog;

private:
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned version) {
        s & BOOST_SERIALIZATION_NVP(name_);
        s & BOOST_SERIALIZATION_NVP(stack_);
        // linkage_             -- not serialized
        // termNames_           -- not serialized
        // outputText_          -- not serialized
        // parsedOutput_        -- not serialized
        // classStatsMutex      -- not serialized
        // classStats           -- not serialized
        // stats                -- not serialized
        // mlog                 -- not serialized
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
        : name_(name), linkage_(LM_NONE) {
        init(linkages);
    }


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
    static SmtSolver* instance(const std::string &name);

    /** Best available solver.
     *
     *  Returns a new solver, an instance of the best available solver. If no solver is possible then returns null. */
    static SmtSolver* bestAvailable();

    virtual ~SmtSolver() {}

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
    virtual Satisfiable triviallySatisfiable(const std::vector<SymbolicExpr::Ptr> &exprs);

    /** Determines if the specified expressions are all satisfiable, unsatisfiable, or unknown.
     *
     *  This is a high-level abstraction that starts a new SMT solver session. For text-based interfaces, this solver object is
     *  reset, a temporary text file is created, the solver is run with the file as input, text output is read, and the
     *  evidence of satisfiability is parsed and stored in this object.
     *
     * @{ */
    virtual Satisfiable satisfiable(const SymbolicExpr::Ptr&);
    virtual Satisfiable satisfiable(const std::vector<SymbolicExpr::Ptr>&);
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
     *  Pops the top set of assertions from the solver stack. The stack always contains one set of assertions, so popping the
     *  last set will cause a new, empty set to be created.
     *
     *  See also, @ref push and @ref reset. */
    virtual void pop();

    /** Number of backtracking levels.
     *
     *  This is the number of sets of assertions. The @ref push and @ref pop increment and decrement this number. The return
     *  valued is always positive. */
    virtual size_t nLevels() const { return stack_.size(); }

    /** Insert assertions.
     *
     *  Inserts assertions into the set of assertions at the top of the backtracking stack.
     *
     * @{ */
    virtual void insert(const SymbolicExpr::Ptr&);
    virtual void insert(const std::vector<SymbolicExpr::Ptr>&);
    /** @} */

    /** All assertions.
     *
     *  Returns the list of all assertions across all backtracking points. */
    virtual std::vector<SymbolicExpr::Ptr> assertions() const;

    /** Assertions for a particular level.
     *
     *  Returns the assertions associated with a particular level of the stack. Level zero is the oldest entry in the stack;
     *  all smt objects have a level zero. See also, @ref nLevels. */
    virtual std::vector<SymbolicExpr::Ptr> assertions(size_t level) const;

    /** Check satisfiability of current stack.
     *
     *  Checks whether all assertions in the entire stack of assertion sets are satisfiable.  A set of no assertions is
     *  trivially satisfiable. */
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

    /** Names of items for which satisfiability evidence exists.
     *
     *  Returns a vector of strings (variable names or memory addresses) that can be passed to @ref evidenceForName.  Not all
     *  SMT solvers can return this information, in which case they return an empty vector.
     *
     *  The returned names are only for those variables and addresses whose evidence of satisfiability can be parsed by
     *  ROSE. The subclasses provide additional methods for obtaining more detailed information. */
    virtual std::vector<std::string> evidenceNames() {
        return std::vector<std::string>();
    }

    /** Evidence of satisfiability for a variable or memory address.
     *
     *  If the string starts with the letter 'v' then variable evidence is returned, otherwise the string must be an address.
     *  Valid strings are those returned by the @ref evidenceNames method; other strings result in a null return
     *  value. Subclasses might define additional methods for obtaining evidence of satisfiability. */
    virtual SymbolicExpr::Ptr evidenceForName(const std::string&) {
        return SymbolicExpr::Ptr();
    }


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
    virtual SymbolicExpr::Ptr evidenceForVariable(const SymbolicExpr::Ptr &var) {
        SymbolicExpr::LeafPtr ln = var->isLeafNode();
        ASSERT_require(ln && !ln->isNumber());
        return evidenceForVariable(ln->nameId());
    }
    virtual SymbolicExpr::Ptr evidenceForVariable(uint64_t varno) {
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
    virtual SymbolicExpr::Ptr evidenceForAddress(uint64_t addr);


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
     *  The statistics are not reset by this call, but continue to accumulate. */
    static Stats classStatistics();

    /** Resets statistics for this solver. */
    void resetStatistics() { stats = Stats(); }

    /** Resets statistics for the class.
     *
     *  Statistics are reset to initial values for the class as a whole.  Resetting statistics for the class does not affect
     *  statistics of any particular SMT solver object. */
    static void resetClassStatistics();


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Low-level API used in subclasses.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
    virtual VariableSet findVariables(const SymbolicExpr::Ptr&) { return VariableSet(); }

    /** Print an S-Expr for debugging.
     *
     *  A null pointer is printed as "nil" and an empty list is printed as "()" in order to distinguish the two cases. There
     *  should be no null pointers though in well-formed S-Exprs. */
    static void printSExpression(std::ostream&, const SExpr::Ptr&);

    /** Generates an input file for for the solver. Usually the input file will be SMT-LIB format, but subclasses might
     *  override this to generate some other kind of input. Throws Excecption if the solver does not support an operation that
     *  is necessary to determine the satisfiability. */
    virtual void generateFile(std::ostream&, const std::vector<SymbolicExpr::Ptr> &exprs, Definitions*) = 0;

    /** Given the name of a configuration file, return the command that is needed to run the solver. The first line
     *  of stdout emitted by the solver should be the word "sat" or "unsat". */
    virtual std::string getCommand(const std::string &config_name) = 0;

    /** Parse all SExprs from the specified string. */
    std::vector<SExpr::Ptr> parseSExpressions(const std::string&);

    /** Parses evidence of satisfiability.  Some solvers can emit information about what variable bindings satisfy the
     *  expression.  This information is parsed by this function and added to a mapping of variable to value. */
    virtual void parseEvidence() {};


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



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated junk
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // FIXME[Robb Matzke 2017-10-17]: these are all deprecated
public:
    virtual Satisfiable trivially_satisfiable(const std::vector<SymbolicExpr::Ptr> &exprs)
        ROSE_DEPRECATED("use triviallySatisfiable");
    virtual SymbolicExpr::Ptr evidence_for_variable(uint64_t varno) ROSE_DEPRECATED("use evidenceForVariable");
    virtual SymbolicExpr::Ptr evidence_for_variable(const SymbolicExpr::Ptr &var) ROSE_DEPRECATED("use evidenceForVariable");
    virtual SymbolicExpr::Ptr evidence_for_name(const std::string&) ROSE_DEPRECATED("use evidenceForName");
    virtual SymbolicExpr::Ptr evidence_for_address(uint64_t addr) ROSE_DEPRECATED("use evidenceForAddress");
    virtual std::vector<std::string> evidence_names() ROSE_DEPRECATED("use evidenceNames");
    virtual void clear_evidence() ROSE_DEPRECATED("use clearEvidence");
    const Stats& get_stats() const ROSE_DEPRECATED("use statistics");
    static Stats get_class_stats() ROSE_DEPRECATED("use classStatistics");
    void reset_stats() ROSE_DEPRECATED("use resetStatistics");
    void reset_class_stats() ROSE_DEPRECATED("use resetClassStatistics");
protected:
    virtual void generate_file(std::ostream&, const std::vector<SymbolicExpr::Ptr> &exprs, Definitions*)
        ROSE_DEPRECATED("use generateFile");
    virtual std::string get_command(const std::string &config_name) ROSE_DEPRECATED("use getCommand");
    virtual void parse_evidence() ROSE_DEPRECATED("use parseEvidence");
};

std::ostream& operator<<(std::ostream&, const SmtSolver::SExpr&);

// FIXME[Robb Matzke 2017-10-17]: This typedef is deprecated. Use SmtSolver instead.
typedef SmtSolver SMTSolver;

} // namespace
} // namespace

#endif
