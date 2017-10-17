#ifndef Rose_BinaryAnalysis_SmtSolver_H
#define Rose_BinaryAnalysis_SmtSolver_H

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include <BinarySymbolicExpr.h>
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
    struct Exception {
        Exception(const std::string &mesg): mesg(mesg) {}
        friend std::ostream& operator<<(std::ostream&, const SmtSolver::Exception&);
        std::string mesg;
    };

    /** Satisfiability constants. */
    enum Satisfiable { SAT_NO=0,                /**< Provably unsatisfiable. */
                       SAT_YES,                 /**< Satisfiable and evidence of satisfiability may be available. */
                       SAT_UNKNOWN              /**< Could not be proved satisfiable or unsatisfiable. */
    };

    /** SMT solver statistics. */
    struct Stats {
        Stats(): ncalls(0), input_size(0), output_size(0) {}
        size_t ncalls;                          /**< Number of times satisfiable() was called. */
        size_t input_size;                      /**< Bytes of input generated for satisfiable(). */
        size_t output_size;                     /**< Amount of output produced by the SMT solver. */
    };

    typedef std::set<uint64_t> Definitions;     /**< Free variables that have been defined. */

private:
    std::string name_;
    FILE *debug;
    void init();

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned version) {
        s & BOOST_SERIALIZATION_NVP(name_);
    }
#endif

public:
    SmtSolver(): debug(NULL) { init(); }

    virtual ~SmtSolver() {}

    /** Property: Name of solver for debugging.
     *
     * @{ */
    const std::string& name() const { return name_; }
    void name(const std::string &s) { name_ = s; }
    /** @} */

    /** Create a solver by name. */
    SmtSolver* instance(const std::string &name);

    /** Determines if expressions are trivially satisfiable or unsatisfiable.  If all expressions are known 1-bit values that
     *  are true, then this function returns SAT_YES.  If any expression is a known 1-bit value that is false, then this
     *  function returns SAT_NO.  Otherwise this function returns SAT_UNKNOWN. */
    virtual Satisfiable triviallySatisfiable(const std::vector<SymbolicExpr::Ptr> &exprs);

    // FIXME[Robb Matzke 2017-10-17]: deprecated
    virtual Satisfiable trivially_satisfiable(const std::vector<SymbolicExpr::Ptr> &exprs)
        ROSE_DEPRECATED("use triviallySatisfiable");

    /** Determines if the specified expressions are all satisfiable, unsatisfiable, or unknown.
     * @{ */
    virtual Satisfiable satisfiable(const SymbolicExpr::Ptr&);
    virtual Satisfiable satisfiable(const std::vector<SymbolicExpr::Ptr>&);
    virtual Satisfiable satisfiable(std::vector<SymbolicExpr::Ptr>, const SymbolicExpr::Ptr&);
    /** @} */

    /** Evidence of satisfiability for a bitvector variable.  If an expression is satisfiable, this function will return
     *  a value for the specified bitvector variable that satisfies the expression in conjunction with the other evidence. Not
     *  all SMT solvers can return this information.  Returns the null pointer if no evidence is available for the variable.
     * @{ */
    virtual SymbolicExpr::Ptr evidenceForVariable(uint64_t varno) {
        char buf[64];
        snprintf(buf, sizeof buf, "v%" PRIu64, varno);
        return evidence_for_name(buf);
    }
    virtual SymbolicExpr::Ptr evidenceForVariable(const SymbolicExpr::Ptr &var) {
        SymbolicExpr::LeafPtr ln = var->isLeafNode();
        ASSERT_require(ln && !ln->isNumber());
        return evidence_for_variable(ln->nameId());
    }
    /** @} */

    // FIXME[Robb Matzke 2017-10-17]: deprecated
    virtual SymbolicExpr::Ptr evidence_for_variable(uint64_t varno) ROSE_DEPRECATED("use evidenceForVariable");
    virtual SymbolicExpr::Ptr evidence_for_variable(const SymbolicExpr::Ptr &var) ROSE_DEPRECATED("use evidenceForVariable");
    
    /** Evidence of satisfiability for a memory address.  If an expression is satisfiable, this function will return
     *  a value for the specified memory address that satisfies the expression in conjunction with the other evidence. Not
     *  all SMT solvers can return this information. Returns the null pointer if no evidence is available for the memory
     *  address. */
    virtual SymbolicExpr::Ptr evidenceForAddress(uint64_t addr);

    // FIXME[Robb Matzke 2017-10-17]: deprecated
    virtual SymbolicExpr::Ptr evidence_for_address(uint64_t addr) ROSE_DEPRECATED("use evidenceForAddress");

    /** Evidence of satisfiability for a variable or memory address.  If the string starts with the letter 'v' then variable
     *  evidence is returned, otherwise the string must be an address.  The strings are those values returned by the
     *  evidence_names() method.  Not all SMT solvers can return this information.  Returns the null pointer if no evidence is
     *  available for the named item. */
    virtual SymbolicExpr::Ptr evidenceForName(const std::string&) {
        return SymbolicExpr::Ptr();
    }

    // FIXME[Robb Matzke 2017-10-17]: deprecated
    virtual SymbolicExpr::Ptr evidence_for_name(const std::string&) ROSE_DEPRECATED("use evidenceForName");

    /** Names of items for which satisfiability evidence exists.  Returns a vector of strings (variable names or memory
     * addresses) that can be passed to @ref evidenceForName.  Not all SMT solvers can return this information. */
    virtual std::vector<std::string> evidenceNames() {
        return std::vector<std::string>();
    }

    // FIXME[Robb Matzke 2017-10-17]: deprecated
    virtual std::vector<std::string> evidence_names() ROSE_DEPRECATED("use evidenceNames");

    /** Clears evidence information. */
    virtual void clearEvidence() {}

    // FIXME[Robb Matzke 2017-10-17]: deprecated
    virtual void clear_evidence() ROSE_DEPRECATED("use clearEvidence");

    /** Turns debugging on or off. */
    void setDebug(FILE *f) { debug = f; }

    // FIXME[Robb Matzke 2017-10-17]: deprecated
    void set_debug(FILE *f) ROSE_DEPRECATED("use setDebug");

    /** Obtain current debugging setting. */
    FILE *getDebug() const { return debug; }

    // FIXME[Robb Matzke 2017-10-17]: deprecated
    FILE *get_debug() const ROSE_DEPRECATED("use getDebug");

    /** Returns statistics for this solver. The statistics are not reset by this call, but continue to accumulate. */
    const Stats& statistics() const { return stats; }

    // FIXME[Robb Matzke 2017-10-17]: deprecated
    const Stats& get_stats() const ROSE_DEPRECATED("use statistics");

    /** Returns statistics for all solvers. The statistics are not reset by this call, but continue to accumulate. */
    static Stats classStatistics();

    // FIXME[Robb Matzke 2017-10-17]: deprecated
    static Stats get_class_stats() ROSE_DEPRECATED("use classStatistics");

    /** Resets statistics for this solver. */
    void resetStatistics() { stats = Stats(); }

    // FIXME[Robb Matzke 2017-10-17]: deprecated
    void reset_stats() ROSE_DEPRECATED("use resetStatistics");

    /** Resets statistics for the class.  Statistics are reset to initial values for the class as a whole.  Resetting
     * statistics for the class does not affect statistics of any particular SMT object. */
    void resetClassStatistics();

    // FIXME[Robb Matzke 2017-10-17]: deprecated
    void reset_class_stats() ROSE_DEPRECATED("use resetClassStatistics");
    
protected:
    /** Generates an input file for for the solver. Usually the input file will be SMT-LIB format, but subclasses might
     *  override this to generate some other kind of input. Throws Excecption if the solver does not support an operation that
     *  is necessary to determine the satisfiability. */
    virtual void generateFile(std::ostream&, const std::vector<SymbolicExpr::Ptr> &exprs, Definitions*) = 0;

    // FIXME[Robb Matzke 2017-10-17]: deprecated
    virtual void generate_file(std::ostream&, const std::vector<SymbolicExpr::Ptr> &exprs, Definitions*)
        ROSE_DEPRECATED("use generateFile") = 0;

    /** Given the name of a configuration file, return the command that is needed to run the solver. The first line
     *  of stdout emitted by the solver should be the word "sat" or "unsat". */
    virtual std::string getCommand(const std::string &config_name) = 0;

    // FIXME[Robb Matzke 2017-10-17]: deprecated
    virtual std::string get_command(const std::string &config_name) ROSE_DEPRECATED("use getCommand") = 0;

    /** Parses evidence of satisfiability.  Some solvers can emit information about what variable bindings satisfy the
     *  expression.  This information is parsed by this function and added to a mapping of variable to value. */
    virtual void parseEvidence() {};

    // FIXME[Robb Matzke 2017-10-17]: deprecated
    virtual void parse_evidence() ROSE_DEPRECATED("use parseEvidence");

    /** Additional output obtained by satisfiable(). */
    std::string outputText;

    // Statistics
    static boost::mutex classStatsMutex;
    static Stats classStats;                            // all access must be protected by classStatsMutex
    Stats stats;
};

// FIXME[Robb Matzke 2017-10-17]: This typedef is deprecated. Use SmtSolver instead.
typedef SmtSolver SMTSolver;

} // namespace
} // namespace

#endif
