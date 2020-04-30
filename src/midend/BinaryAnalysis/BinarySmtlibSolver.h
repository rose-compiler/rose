#ifndef Rose_BinaryAnalysis_SmtlibSolver_H
#define Rose_BinaryAnalysis_SmtlibSolver_H
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <BinarySmtSolver.h>
#include <boost/filesystem.hpp>
#include <boost/unordered_map.hpp>

namespace Rose {
namespace BinaryAnalysis {

/** Wrapper around solvers that speak SMT-LIB. */
class SmtlibSolver: public SmtSolver {
private:
    boost::filesystem::path executable_;                // solver program
    std::string shellArgs_;                             // extra arguments for command (passed through shell)
    ExprExprMap varsForSets_;                           // variables to use for sets

protected:
    ExprExprMap evidence;
    typedef boost::unordered_map<SymbolicExpr::Hash, ExprExprMap> MemoizedEvidence;
    MemoizedEvidence memoizedEvidence;
    Sawyer::Optional<boost::chrono::duration<double> > timeout_; // max time for solving a single set of equations in seconds

protected:
    // Reference counted. Use instance() or create() instead.
    explicit SmtlibSolver(const std::string &name, const boost::filesystem::path &executable, const std::string &shellArgs = "",
                          unsigned linkages = LM_EXECUTABLE)
        : SmtSolver(name, linkages), executable_(executable), shellArgs_(shellArgs) {}

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
    static Ptr instance(const std::string &name, const boost::filesystem::path &executable, const std::string &shellArgs = "",
                        unsigned linkages = LM_EXECUTABLE) {
        return Ptr(new SmtlibSolver(name, executable, shellArgs, linkages));
    }

    /** Virtual constructor.
     *
     *  Creates a new solver like this one. */
    virtual Ptr create() const ROSE_OVERRIDE {
        return instance(name(), executable_, shellArgs_, linkage());
    }
    
public:
    virtual void reset() ROSE_OVERRIDE;
    virtual void generateFile(std::ostream&, const std::vector<SymbolicExpr::Ptr> &exprs, Definitions*) ROSE_OVERRIDE;
    virtual std::string getCommand(const std::string &configName) ROSE_OVERRIDE;
    virtual std::string getErrorMessage(int exitStatus) ROSE_OVERRIDE;
    virtual void findVariables(const SymbolicExpr::Ptr&, VariableSet&) ROSE_OVERRIDE;
    virtual SymbolicExpr::Ptr evidenceForName(const std::string&) ROSE_OVERRIDE;
    virtual std::vector<std::string> evidenceNames() ROSE_OVERRIDE;
    virtual void clearEvidence() ROSE_OVERRIDE;
    virtual void clearMemoization() ROSE_OVERRIDE;
    virtual void timeout(boost::chrono::duration<double>) ROSE_OVERRIDE;

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
    // Return the most common type (arbitrarily if tied). Returns NO_TYPE when there are no inputs.
    virtual Type mostType(const std::vector<SExprTypePair>&);

    // Cast an SMT expression(s) to some other type.
    virtual SExprTypePair outputCast(const SExprTypePair&, Type to);
    virtual std::vector<SExprTypePair> outputCast(const std::vector<SExprTypePair>&, Type to);

    virtual std::string typeName(const SymbolicExpr::Ptr&);

    // Convert a ROSE symbolic expression to an SMT solver expression. The return value is a pair consisting of an SExpr::Ptr
    // (ROSE internal representation of an SMT solver expression) and a type indicating whether the SMT solver expression is a
    // bit vector, a Boolean, or a memory state. Although ROSE uses a bit to represent Booleans, SMT solvers often distinguish
    // betwen a single bit and a Boolean.
    virtual SExprTypePair outputExpression(const SymbolicExpr::Ptr&);
    virtual std::vector<SmtSolver::SExprTypePair> outputExpressions(const std::vector<SymbolicExpr::Ptr>&);

    // Create an SMT expression from a ROSE symbolic leaf node (constant, variable, or memory state).
    virtual SExprTypePair outputLeaf(const SymbolicExpr::LeafPtr&);

    // Create an expression composed of only 2-argument calls to the specified function.  The arguments are all first converted
    // to the most common argument type (which is usually a no-op since arguments are normally all the same type). If rettype
    // is NO_TYPE then the returned type is assumed to be the same as the arguments, otherwise it is set as specified.
    virtual SExprTypePair outputLeftAssoc(const std::string &func, const SymbolicExpr::InteriorPtr&, Type rettype = NO_TYPE);
    virtual SExprTypePair outputLeftAssoc(const std::string &func, const std::vector<SExprTypePair>&, Type rettype = NO_TYPE);

    // Create an expression that does a shift operation.
    virtual SExprTypePair outputArithmeticShiftRight(const SymbolicExpr::InteriorPtr&);
    virtual SExprTypePair outputLogicalShiftRight(const SymbolicExpr::InteriorPtr&);
    virtual SExprTypePair outputShiftLeft(const SymbolicExpr::InteriorPtr&);

    // Create a rotate expression. */
    virtual SExprTypePair outputRotateLeft(const SymbolicExpr::InteriorPtr&);
    virtual SExprTypePair outputRotateRight(const SymbolicExpr::InteriorPtr&);

    // Create an expression that does either a bit-wise or boolean XOR. All arguments must be the same type, either bit vectors
    // or Booleans, and the return value is the same as the argument type.
    virtual SExprTypePair outputXor(const SymbolicExpr::InteriorPtr&);

    // Create a binary expression. This is a special case of outputLeftAssoc.
    virtual SExprTypePair outputBinary(const std::string &func, const SymbolicExpr::InteriorPtr&, Type rettype = NO_TYPE);

    // Create a unary expression.  The return type is the same as the argument type.
    virtual SExprTypePair outputUnary(const std::string &funcName, const SExprTypePair &arg);

    // Create a bit extraction expression, i.e., a bit vector result which is a sub-array of the input bit vector.
    virtual SExprTypePair outputExtract(const SymbolicExpr::InteriorPtr&);

    // Create a widening expression that returns a bit vector type. */
    virtual SExprTypePair outputSignExtend(const SymbolicExpr::InteriorPtr&);
    virtual SExprTypePair outputUnsignedExtend(const SymbolicExpr::InteriorPtr&);

    // Create an if-then-else expression. The arguments should be the same type (one is cast if not) and the return type is the
    // same as the argument types.
    virtual SExprTypePair outputIte(const SymbolicExpr::InteriorPtr&);

    // Create a not-equal expression. The operands can be any type and are cast to a common type before comparing. The return
    // type is Boolean.
    virtual SExprTypePair outputNotEqual(const SymbolicExpr::InteriorPtr&);

    // Create a comparison expression for bit vectors. Return type is Boolean. */
    virtual SExprTypePair outputSignedCompare(const SymbolicExpr::InteriorPtr&);
    virtual SExprTypePair outputUnsignedCompare(const SymbolicExpr::InteriorPtr&);
    virtual SExprTypePair outputZerop(const SymbolicExpr::InteriorPtr&);

    // Create multiplicative expression. */
    virtual SExprTypePair outputMultiply(const SymbolicExpr::InteriorPtr&);
    virtual SExprTypePair outputDivide(const SymbolicExpr::InteriorPtr&, const std::string &operation);
    virtual SExprTypePair outputModulo(const SymbolicExpr::InteriorPtr&, const std::string &operation);

    // Create a memory read expression. The return type is a bit vector. */
    virtual SExprTypePair outputRead(const SymbolicExpr::InteriorPtr&);

    // Create a memory write operation. The return value is a memory state. */
    virtual SExprTypePair outputWrite(const SymbolicExpr::InteriorPtr&);

    // Create a set expression that represents a set of bit vectors all the same size. */
    virtual SExprTypePair outputSet(const SymbolicExpr::InteriorPtr&);

    // Functions that generate SMT-LIB output to a stream when given a Rose::BinaryAnalysis::SymbolicExpr
    virtual void outputVariableDeclarations(std::ostream&, const VariableSet&);
    virtual void outputComments(std::ostream&, const std::vector<SymbolicExpr::Ptr>&);
    virtual void outputCommonSubexpressions(std::ostream&, const std::vector<SymbolicExpr::Ptr>&);
    virtual void outputAssertion(std::ostream&, const SymbolicExpr::Ptr&);
};

} // namespace
} // namespace

#endif
#endif
