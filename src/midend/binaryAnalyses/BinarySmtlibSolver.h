#ifndef Rose_BinaryAnalysis_SmtlibSolver_H
#define Rose_BinaryAnalysis_SmtlibSolver_H

#include <BinarySmtSolver.h>
#include <boost/filesystem.hpp>

namespace Rose {
namespace BinaryAnalysis {

/** Wrapper around solvers that speak SMT-LIB. */
class SmtlibSolver: public SmtSolver {
private:
    boost::filesystem::path executable_;                // solver program
    std::string shellArgs_;                             // extra arguments for command (passed through shell)
    typedef Sawyer::Container::Map<SymbolicExpr::Ptr, SymbolicExpr::Ptr> ExprExprMap;
    ExprExprMap varsForSets_;                           // variables to use for sets
    ExprExprMap evidence_;

public:
    /** Construct a solver using the specified program.
     *
     *  This object will communicate with the SMT solver using SMT-LIB version 2 text files, both for input to the solver and
     *  expected output from the solver. Diagnostics from the solver (standard error) are not redirected by the ROSE library. A
     *  separate invocation of the solver is used for each satisfiability check.
     *
     *  The @p executable should be the name of the solver executable, either to be found in $PATH or an absolute file
     *  name. Beware that some tools might change directories as they run, so absolute names are usually best.  The optional @p
     *  shellArgs are the list of extra arguments to pass to the solver. WARNING: the entire command is pass to @c popen, which
     *  will invoke a shell to process the executable name and arguments; appropriate escaping of shell meta characters is the
     *  responsibility of the caller. */
    explicit SmtlibSolver(const boost::filesystem::path &executable, const std::string &shellArgs = "")
        : SmtSolver(executable.filename().string(), LM_EXECUTABLE), executable_(executable), shellArgs_(shellArgs) {}

public:
    virtual void generateFile(std::ostream&, const std::vector<SymbolicExpr::Ptr> &exprs, Definitions*) ROSE_OVERRIDE;
    virtual std::string getCommand(const std::string &configName) ROSE_OVERRIDE;
    virtual std::string getErrorMessage(int exitStatus) ROSE_OVERRIDE;
    virtual VariableSet findVariables(const std::vector<SymbolicExpr::Ptr>&) ROSE_OVERRIDE;
    virtual SymbolicExpr::Ptr evidenceForName(const std::string&) ROSE_OVERRIDE;
    virtual std::vector<std::string> evidenceNames() ROSE_OVERRIDE;
    virtual void clearEvidence() ROSE_OVERRIDE;

protected:
    /** Specify variable to use for OP_SET.
     *
     *  Each OP_SET needs a free variable to choose from the available members of the set.  This function sets (two arguments)
     *  or retrives (one argument) the variable associated with a set.
     *
     * @{ */
    void varForSet(const SymbolicExpr::InteriorPtr &set, const SymbolicExpr::LeafPtr &var);
    SymbolicExpr::LeafPtr varForSet(const SymbolicExpr::InteriorPtr &set);
    /** @} */

    virtual void parseEvidence() ROSE_OVERRIDE;

    /** Generate definitions for bit-wise XOR functions.
     *
     *  This method scans the supplied list of expressiosn and for each bit-wise XOR operation taking arguments of width @em n,
     *  it emits a binary function definition named "bvxor<em>n</em>". */
    virtual void outputBvxorFunctions(std::ostream&, const std::vector<SymbolicExpr::Ptr>&);

    /** Generate functions for comparison of bitvectors. */
    virtual void outputComparisonFunctions(std::ostream&, const std::vector<SymbolicExpr::Ptr>&);

protected:

    virtual std::string typeName(const SymbolicExpr::Ptr&);

    // Functions that generate SMT-LIB output to a stream when given a Rose::BinaryAnalysis::SymbolicExpr
    virtual void outputDefinitions(std::ostream&, const VariableSet&);
    virtual void outputComments(std::ostream&, const std::vector<SymbolicExpr::Ptr>&);
    virtual void outputCommonSubexpressions(std::ostream&, const std::vector<SymbolicExpr::Ptr>&);
    virtual void outputAssertion(std::ostream&, const SymbolicExpr::Ptr&);
    virtual void outputExpression(std::ostream&, const SymbolicExpr::Ptr&, Type need);
    virtual void outputLeaf(std::ostream&, const SymbolicExpr::LeafPtr&, Type need);
    virtual void outputList(std::ostream&, const std::string &funcName, const SymbolicExpr::InteriorPtr&, Type need);
    virtual void outputUnary(std::ostream&, const std::string &funcName, const SymbolicExpr::InteriorPtr&, Type need);
    virtual void outputBinary(std::ostream&, const std::string &funcName, const SymbolicExpr::InteriorPtr&, Type need);
    virtual void outputLeftAssoc(std::ostream&, const std::string &funcName, const SymbolicExpr::InteriorPtr&, Type need);
    virtual void outputXor(std::ostream&, const SymbolicExpr::InteriorPtr&);
    virtual void outputExtract(std::ostream&, const SymbolicExpr::InteriorPtr&);
    virtual void outputIte(std::ostream&, const SymbolicExpr::InteriorPtr&, Type need);
    virtual void outputNotEqual(std::ostream&, const SymbolicExpr::InteriorPtr&);
    virtual void outputUnsignedExtend(std::ostream&, const SymbolicExpr::InteriorPtr&);
    virtual void outputSignExtend(std::ostream&, const SymbolicExpr::InteriorPtr&);
    virtual void outputSet(std::ostream&, const SymbolicExpr::InteriorPtr&);
    virtual void outputLogicalShiftRight(std::ostream&, const SymbolicExpr::InteriorPtr&);
    virtual void outputShiftLeft(std::ostream&, const SymbolicExpr::InteriorPtr&);
    virtual void outputArithmeticShiftRight(std::ostream&, const SymbolicExpr::InteriorPtr&);
    virtual void outputRotateLeft(std::ostream&, const SymbolicExpr::InteriorPtr&);
    virtual void outputRotateRight(std::ostream&, const SymbolicExpr::InteriorPtr&);
    virtual void outputZerop(std::ostream&, const SymbolicExpr::InteriorPtr&);
    virtual void outputMultiply(std::ostream&, const SymbolicExpr::InteriorPtr&);
    virtual void outputUnsignedDivide(std::ostream&, const SymbolicExpr::InteriorPtr&);
    virtual void outputUnsignedModulo(std::ostream&, const SymbolicExpr::InteriorPtr&);
    virtual void outputSignedCompare(std::ostream&, const SymbolicExpr::InteriorPtr&);
    virtual void outputUnsignedCompare(std::ostream&, const SymbolicExpr::InteriorPtr&);
    virtual void outputRead(std::ostream&, const SymbolicExpr::InteriorPtr&);
    virtual void outputWrite(std::ostream&, const SymbolicExpr::InteriorPtr&);
};

} // namespace
} // namespace

#endif
