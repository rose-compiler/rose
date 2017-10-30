#ifndef Rose_BinaryAnalysis_Z3Solver_H
#define Rose_BinaryAnalysis_Z3Solver_H

#include <rosePublicConfig.h>
#include <BinarySmtlibSolver.h>
#ifdef ROSE_HAVE_Z3
#include <z3++.h>
#endif

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
    Sawyer::Container::Map<SymbolicExpr::Ptr, z3::expr> ctxCses_; // common subexpressions
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
        SmtlibSolver("/bin/false")
#endif
#ifdef ROSE_HAVE_Z3
        , ctx_(NULL)
#endif
    {
        name("Z3");
        linkage_ = bestLinkage(linkages & availableLinkages());
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

    virtual Satisfiable satisfiable(const std::vector<SymbolicExpr::Ptr> &exprs) ROSE_OVERRIDE;
    virtual Satisfiable satisfiable(const SymbolicExpr::Ptr &tn) ROSE_OVERRIDE {
        std::vector<SymbolicExpr::Ptr> exprs;
        exprs.push_back(tn);
        return satisfiable(exprs);
    }

    virtual SymbolicExpr::Ptr evidenceForName(const std::string&) ROSE_OVERRIDE;
    virtual std::vector<std::string> evidenceNames() ROSE_OVERRIDE;

protected:
    virtual void outputBvxorFunctions(std::ostream&, const std::vector<SymbolicExpr::Ptr>&) ROSE_OVERRIDE;
    virtual void outputComparisonFunctions(std::ostream&, const std::vector<SymbolicExpr::Ptr>&) ROSE_OVERRIDE;

private:
    virtual void outputExpression(std::ostream&, const SymbolicExpr::Ptr&) ROSE_OVERRIDE;
    virtual void outputArithmeticShiftRight(std::ostream&, const SymbolicExpr::InteriorPtr&) ROSE_OVERRIDE;

#ifdef ROSE_HAVE_Z3
    z3::expr ctxExpression(const SymbolicExpr::Ptr&);
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
