#ifndef Rose_SMTSolver_H
#define Rose_SMTSolver_H

#include "InsnSemanticsExpr.h"

/** Interface to Satisfiability Modulo Theory (SMT) solvers.
 *
 *  The purpose of an SMT solver is to determine if an expression is satisfiable. Although the SMTSolver class was originally
 *  designed to be used by SymbolicExpressionSemantics policy (see SymbolicExpressionSemantics::Policy::set_solver()), but it
 *  can also be used independently. */
class SMTSolver {
public:
    struct Exception {
        Exception(const std::string &mesg): mesg(mesg) {}
        friend std::ostream& operator<<(std::ostream&, const SMTSolver::Exception&);
        std::string mesg;
    };
    
    typedef std::set<uint64_t> Definitions;     /**< Free variables that have been defined. */

    SMTSolver(): debug(NULL) {}

    virtual ~SMTSolver() {}

    /** Determines if the specified expression is satisfiable. Throws Exception if satisfiability cannot be determined. */
    virtual bool satisfiable(const InsnSemanticsExpr::TreeNode *expr);

    /** Determines if the specified collection of expressions is satisfiable.  Throws Exception if satisfiability cannot be
     *  determined. */
    virtual bool satisfiable(const std::vector<const InsnSemanticsExpr::TreeNode*> &exprs);

    /** Evidence of satisfiability.  If an expression is satisfiable, this function will return information about which values
     *  should be bound to variables to make the expression satisfiable.  Not all SMT solvers can return this information.
     *  @{ */
    virtual InsnSemanticsExpr::TreeNode *get_definition(uint64_t varno) { return NULL; }
    virtual InsnSemanticsExpr::TreeNode *get_definition(const InsnSemanticsExpr::LeafNode *var) {
        assert(var && !var->is_known());
        return get_definition(var->get_name());
    }
    /** @} */

    /** Turns debugging on or off. */
    void set_debug(FILE *f) { debug = f; }

    /** Obtain current debugging setting. */
    FILE *get_debug() const { return debug; }

    /** Returns the number of times satisfiable() was called.  This is a class method that returns the total number of SMT
     * solver calls across all SMT solvers. */
    static size_t get_ncalls() const { return total_calls; }

protected:
    /** Generates an input file for for the solver. Usually the input file will be SMT-LIB format, but subclasses might
     *  override this to generate some other kind of input. Throws Excecption if the solver does not support an operation that
     *  is necessary to determine the satisfiability. */
    virtual void generate_file(std::ostream&, const std::vector<const InsnSemanticsExpr::TreeNode*> &exprs, Definitions*) = 0;

    /** Given the name of a configuration file, return the command that is needed to run the solver. The first line
     *  of stdout emitted by the solver should be the word "sat" or "unsat". */
    virtual std::string get_command(const std::string &config_name) = 0;

    /** Parses evidence of satisfiability.  Some solvers can emit information about what variable bindings satisfy the
     *  expression.  This information is parsed by this function and added to a mapping of variable to value. */
    virtual void parse_evidence() {};

    /** Additional output obtained by satisfiable(). */
    std::vector<std::string> output_text;
    
    /** Tracks the number of times an SMT solver was called. Actually, the number of calls to satisfiable() */
    static size_t total_calls;

private:
    FILE *debug;
};

#endif
