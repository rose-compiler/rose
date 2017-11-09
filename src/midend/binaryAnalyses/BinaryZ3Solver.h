#ifndef Rose_BinaryAnalysis_Z3Solver_H
#define Rose_BinaryAnalysis_Z3Solver_H

#include <rosePublicConfig.h>
#include <BinarySmtlibSolver.h>
#ifdef ROSE_HAVE_Z3
#include <z3++.h>
#endif

#include <boost/serialization/access.hpp>

namespace Rose {
namespace BinaryAnalysis {

/** Interface to the Z3 SMT solver.
 *
 *  This interface has to modes: it can either talk to a "z3" executable using the SMT-LIB2 format, or it can connect directly
 *  to the z3 shared library. The former is easier to debug, but the latter is much faster since it avoids translating to an
 *  intermediate text representation both when sending data to the solver and when getting data from the solver.  The mode is
 *  selected at runtime with the @ref linkage property. */
class Z3Solver: public SmtlibSolver {
private:
#ifdef ROSE_HAVE_Z3
    z3::context *ctx_;
    z3::solver *solver_;
    std::vector<std::vector<z3::expr> > z3Stack_;       // parallel with parent class' "stack_" data member
    typedef Sawyer::Container::Map<SymbolicExpr::Ptr, z3::expr> CommonSubexpressions;
    CommonSubexpressions ctxCses_; // common subexpressions
    typedef Sawyer::Container::Map<SymbolicExpr::LeafPtr, z3::func_decl> VariableDeclarations;
    VariableDeclarations ctxVarDecls_;
#endif

private:
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned version) {
        // ctx_         -- not serialized
        // solver_      -- not serialized
        // ctxCses_     -- not serialized
        // ctxVarDecls_ -- not serialized
    }
#endif

public:
    /**  Construct Z3 solver preferring library linkage.
     *
     *   If executable (@c LM_EXECUTABLE) linkage is specified then the executable is that which was detected by the ROSE
     *   configuration script. */
    explicit Z3Solver(unsigned linkages = LM_ANY)
        :
#ifdef ROSE_Z3
        SmtlibSolver(ROSE_Z3)
#else
        SmtlibSolver("z3")
#endif
#ifdef ROSE_HAVE_Z3
        , ctx_(NULL), solver_(NULL)
#endif
    {
        name("Z3");
        linkage_ = bestLinkage(linkages & availableLinkages());
#ifdef ROSE_HAVE_Z3
        ctx_ = new z3::context;
        solver_ = new z3::solver(*ctx_);
        z3Stack_.push_back(std::vector<z3::expr>());
#endif
    }

    /** Construct Z3 solver using a specified executable.
     *
     *  The @p exe should be only the name of the Z3 executable. The @p shellArgs are the rest of the command-line, all of
     *  which will be passed through a shell. The caller is responsible for appropriately escaping shell meta characters. */
    explicit Z3Solver(const boost::filesystem::path &exe, const std::string &shellArgs = "")
        : SmtlibSolver(exe, shellArgs) {}
    
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
     *  is @ref reset, which also happens implicitly for certain high-level functions like @ref satisfiable. */
    virtual z3::context *z3Context() const;

    /** Solver used for Z3 library.
     *
     *  Returns the solver object being used for the Z3 solver API. A solver running with @c LM_LIBRARY @ref linkage mode
     *  always has a non-null solver pointer. The object is owned by this server and reallocated whenever this solver is @ref
     *  reset, which also happens implicitly for certain high-level functions like @ref satisfiable. */
    virtual z3::solver *z3Solver() const;

    /** Z3 assertions.
     *
     *  This function is similar to @ref assertions except instead of returning ROSE symbolic expressions it returns Z3
     *  expressions. The return value is parallel with the return value of @ref assertions.
     *
     * @{ */
    virtual std::vector<z3::expr> z3Assertions() const;
    virtual std::vector<z3::expr> z3Assertions(size_t level) const;
    /** @} */
#endif

    // Overrides
public:
    virtual Satisfiable checkLib() ROSE_OVERRIDE;
    virtual void reset() ROSE_OVERRIDE;
    virtual void clearEvidence() ROSE_OVERRIDE;
    virtual SymbolicExpr::Ptr evidenceForName(const std::string&) ROSE_OVERRIDE;
    virtual std::vector<std::string> evidenceNames() ROSE_OVERRIDE;
    virtual void push() ROSE_OVERRIDE;
    virtual void pop() ROSE_OVERRIDE;
    void insert(const SymbolicExpr::Ptr &expr) ROSE_OVERRIDE;
    using SmtlibSolver::insert;
protected:
    virtual void outputBvxorFunctions(std::ostream&, const std::vector<SymbolicExpr::Ptr>&) ROSE_OVERRIDE;
    virtual void outputComparisonFunctions(std::ostream&, const std::vector<SymbolicExpr::Ptr>&) ROSE_OVERRIDE;
    virtual void outputExpression(std::ostream&, const SymbolicExpr::Ptr&, Type need) ROSE_OVERRIDE;
    virtual void outputArithmeticShiftRight(std::ostream&, const SymbolicExpr::InteriorPtr&) ROSE_OVERRIDE;

#ifdef ROSE_HAVE_Z3
protected:
    z3::expr ctxExpression(const SymbolicExpr::Ptr&, Type need);
    void ctxVariableDeclarations(const VariableSet&);
    void ctxCommonSubexpressions(const SymbolicExpr::Ptr&);
    z3::expr ctxArithmeticShiftRight(const SymbolicExpr::InteriorPtr&);
    z3::expr ctxExtract(const SymbolicExpr::InteriorPtr&);
    z3::expr ctxRead(const SymbolicExpr::InteriorPtr&);
    z3::expr ctxRotateLeft(const SymbolicExpr::InteriorPtr&);
    z3::expr ctxRotateRight(const SymbolicExpr::InteriorPtr&);
    z3::expr ctxSet(const SymbolicExpr::InteriorPtr&);
    z3::expr ctxSignExtend(const SymbolicExpr::InteriorPtr&);
    z3::expr ctxShiftLeft(const SymbolicExpr::InteriorPtr&);
    z3::expr ctxShiftRight(const SymbolicExpr::InteriorPtr&);
    z3::expr ctxMultiply(const SymbolicExpr::InteriorPtr&);
    z3::expr ctxUnsignedDivide(const SymbolicExpr::InteriorPtr&);
    z3::expr ctxUnsignedExtend(const SymbolicExpr::InteriorPtr&);
    z3::expr ctxUnsignedModulo(const SymbolicExpr::InteriorPtr&);
    z3::expr ctxWrite(const SymbolicExpr::InteriorPtr&);
    z3::expr ctxZerop(const SymbolicExpr::InteriorPtr&);
#endif
};

} // namespace
} // namespace

#endif
