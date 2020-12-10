#ifndef Rose_BinaryAnalysis_Z3Solver_H
#define Rose_BinaryAnalysis_Z3Solver_H
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include <BinarySmtlibSolver.h>
#ifdef ROSE_HAVE_Z3
#include <z3++.h>
#endif
#ifdef ROSE_HAVE_Z3_VERSION_H
#include <z3_version.h>
#endif

#ifndef ROSE_Z3
#define ROSE_Z3 ""
#endif

#include <boost/serialization/access.hpp>

namespace Rose {
namespace BinaryAnalysis {

/** Interface to the Z3 SMT solver.
 *
 *  This interface has two modes: it can either talk to a "z3" executable using the SMT-LIB2 format, or it can connect directly
 *  to the z3 shared library. The former is easier to debug, but the latter is much faster since it avoids translating to an
 *  intermediate text representation both when sending data to the solver and when getting data from the solver.  The mode is
 *  selected at runtime with the @ref linkage property.
 *
 *  If memoization is enabled, then the Z3 state may lag behind the ROSE state in order to avoid making any calls to Z3 until
 *  after the memoization check.  If the caller wants to make the Z3 state up-to-date with the ROSE state then he should invoke
 *  the @ref z3Update function. */
class Z3Solver: public SmtlibSolver {
#ifdef ROSE_HAVE_Z3
public:
    typedef std::pair<z3::expr, Type> Z3ExprTypePair;
private:
    z3::context *ctx_;
    z3::solver *solver_;
    std::vector<std::vector<z3::expr> > z3Stack_;       // lazily parallel with parent class' "stack_" data member
    typedef Sawyer::Container::Map<SymbolicExpr::Ptr, Z3ExprTypePair> CommonSubexpressions;
    CommonSubexpressions ctxCses_; // common subexpressions
    typedef Sawyer::Container::Map<SymbolicExpr::LeafPtr, z3::func_decl, CompareLeavesByName> VariableDeclarations;
    VariableDeclarations ctxVarDecls_;
#endif

private:
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SmtSolver);
        // ctx_         -- not serialized
        // solver_      -- not serialized
        // z3Stack_     -- not serialized
        // ctxCses_     -- not serialized
        // ctxVarDecls_ -- not serialized
    }
#endif

protected:
    // Reference counted object. Use instance or create instead.
    explicit Z3Solver(unsigned linkages = LM_ANY)
        : SmtlibSolver("z3", ROSE_Z3, "", linkages & availableLinkages())
#ifdef ROSE_HAVE_Z3
          , ctx_(NULL), solver_(NULL)
#endif
    {
#ifdef ROSE_HAVE_Z3
        ctx_ = new z3::context;
        solver_ = new z3::solver(*ctx_);
        z3Stack_.push_back(std::vector<z3::expr>());
#endif
    }

public:
    /**  Construct Z3 solver preferring library linkage.
     *
     *   If executable (@c LM_EXECUTABLE) linkage is specified then the executable is that which was detected by the ROSE
     *   configuration script. */
    static Ptr instance(unsigned linkages = LM_ANY) {
        return Ptr(new Z3Solver(linkages));
    }

    /** Virtual constructor.
     *
     *  Create a new solver just like this one. */
    virtual Ptr create() const ROSE_OVERRIDE {
        return Ptr(instance(linkage()));
    }

    /** Construct Z3 solver using a specified executable.
     *
     *  The @p exe should be only the name of the Z3 executable. The @p shellArgs are the rest of the command-line, all of
     *  which will be passed through a shell. The caller is responsible for appropriately escaping shell meta characters. */
    explicit Z3Solver(const boost::filesystem::path &exe, const std::string &shellArgs = "")
        : SmtlibSolver("Z3", exe, shellArgs) {}
    
    /** Returns a bit vector of linkage capabilities.
     *
     *  Returns a vector of @ref LinkMode bits that say what possible modes of communicating with the Z3 SMT solver are
     *  available. A return value of zero means the Z3 solver is not supported in this configuration of ROSE. */
    static unsigned availableLinkages();

#ifdef ROSE_HAVE_Z3
    /** Context used for Z3 library.
     *
     *  Returns the context object being used for the Z3 solver API. A solver running with @c LM_LIBRARY @ref linkage mode
     *  always has a non-null context pointer. The object is owned by this solver and is reallocated whenever this solver
     *  is @ref reset, which also happens implicitly for certain high-level functions like @ref satisfiable.
     *
     *  Warning: The Z3 state may lag behind the ROSE state since ROSE tries to optimize calls to Z3.  If you need the Z3 state
     *  to be updated to match the ROSE state, call @ref z3Update. */
    virtual z3::context *z3Context() const;

    /** Solver used for Z3 library.
     *
     *  Returns the solver object being used for the Z3 solver API. A solver running with @c LM_LIBRARY @ref linkage mode
     *  always has a non-null solver pointer. The object is owned by this server and reallocated whenever this solver is @ref
     *  reset, which also happens implicitly for certain high-level functions like @ref satisfiable.
     *
     *  Warning: The Z3 state may lag behind the ROSE state since ROSE tries to optimize calls to Z3.  If you need the Z3 state
     *  to be updated to match the ROSE state, call @ref z3Update. */
    virtual z3::solver *z3Solver() const;

    /** Z3 assertions.
     *
     *  This function is similar to @ref assertions except instead of returning ROSE symbolic expressions it returns Z3
     *  expressions. The return value is parallel with the return value of @ref assertions. This function calls @ref z3Update
     *  to make sure the Z3 state matches the ROSE state, therefore this function's return value will parallel the return value
     *  from @ref assertions.
     *
     * @{ */
    virtual std::vector<z3::expr> z3Assertions() const;
    virtual std::vector<z3::expr> z3Assertions(size_t level) const;
    /** @} */
#endif

    /** Updates the Z3 state to match the ROSE state.
     *
     *  ROSE tries to avoid making any Z3 calls until it knows they're necessary. Therefore the Z3 state may lag behind the
     *  ROSE state. This function's purpose is to bring the Z3 state up-to-date with the ROSE state. You may call it as often
     *  as you like, and it is called automatically by some other functions in this API. */
    virtual void z3Update();

protected:
    SExprTypePair outputList(const std::string &name, const SymbolicExpr::InteriorPtr&, Type rettype = NO_TYPE);
    SExprTypePair outputList(const std::string &name, const std::vector<SExprTypePair>&, Type rettype = NO_TYPE);

    // Overrides
public:
    virtual Satisfiable checkLib() ROSE_OVERRIDE;
    virtual void reset() ROSE_OVERRIDE;
    virtual void clearEvidence() ROSE_OVERRIDE;
    virtual void parseEvidence() ROSE_OVERRIDE;
    virtual void pop() ROSE_OVERRIDE;
    virtual void selfTest() ROSE_OVERRIDE;
    virtual void timeout(boost::chrono::duration<double>) ROSE_OVERRIDE;
protected:
    virtual void outputBvxorFunctions(std::ostream&, const std::vector<SymbolicExpr::Ptr>&) ROSE_OVERRIDE;
    virtual void outputComparisonFunctions(std::ostream&, const std::vector<SymbolicExpr::Ptr>&) ROSE_OVERRIDE;
    virtual SExprTypePair outputExpression(const SymbolicExpr::Ptr&) ROSE_OVERRIDE;
    virtual SExprTypePair outputArithmeticShiftRight(const SymbolicExpr::InteriorPtr&) ROSE_OVERRIDE;

#ifdef ROSE_HAVE_Z3
protected:
    virtual Type mostType(const std::vector<Z3ExprTypePair>&);
    using SmtlibSolver::mostType;
    Z3ExprTypePair ctxCast(const Z3ExprTypePair&, Type toType);
    std::vector<Z3Solver::Z3ExprTypePair> ctxCast(const std::vector<Z3ExprTypePair>&, Type toType);
    Z3ExprTypePair ctxLeaf(const SymbolicExpr::LeafPtr&);
    Z3ExprTypePair ctxExpression(const SymbolicExpr::Ptr&);
    std::vector<Z3Solver::Z3ExprTypePair> ctxExpressions(const std::vector<SymbolicExpr::Ptr>&);
    void ctxVariableDeclarations(const VariableSet&);
    void ctxCommonSubexpressions(const SymbolicExpr::Ptr&);
    Z3ExprTypePair ctxArithmeticShiftRight(const SymbolicExpr::InteriorPtr&);
    Z3ExprTypePair ctxExtract(const SymbolicExpr::InteriorPtr&);
    Z3ExprTypePair ctxRead(const SymbolicExpr::InteriorPtr&);
    Z3ExprTypePair ctxRotateLeft(const SymbolicExpr::InteriorPtr&);
    Z3ExprTypePair ctxRotateRight(const SymbolicExpr::InteriorPtr&);
    Z3ExprTypePair ctxSet(const SymbolicExpr::InteriorPtr&);
    Z3ExprTypePair ctxSignExtend(const SymbolicExpr::InteriorPtr&);
    Z3ExprTypePair ctxShiftLeft(const SymbolicExpr::InteriorPtr&);
    Z3ExprTypePair ctxShiftRight(const SymbolicExpr::InteriorPtr&);
    Z3ExprTypePair ctxMultiply(const SymbolicExpr::InteriorPtr&);
    Z3ExprTypePair ctxUnsignedDivide(const SymbolicExpr::InteriorPtr&);
    Z3ExprTypePair ctxSignedDivide(const SymbolicExpr::InteriorPtr&);
    Z3ExprTypePair ctxUnsignedExtend(const SymbolicExpr::InteriorPtr&);
    Z3ExprTypePair ctxUnsignedModulo(const SymbolicExpr::InteriorPtr&);
    Z3ExprTypePair ctxSignedModulo(const SymbolicExpr::InteriorPtr&);
    Z3ExprTypePair ctxWrite(const SymbolicExpr::InteriorPtr&);
    Z3ExprTypePair ctxZerop(const SymbolicExpr::InteriorPtr&);
#endif
};

} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::Z3Solver);
#endif

#endif
#endif
