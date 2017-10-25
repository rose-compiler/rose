#ifndef Rose_BinaryAnalysis_Z3Solver_H
#define Rose_BinaryAnalysis_Z3Solver_H

#include <rosePublicConfig.h>
#include <BinarySmtlibSolver.h>

namespace Rose {
namespace BinaryAnalysis {

/** Interface to the Z3 SMT solver.
 *
 *  This interface has to modes: it can either talk to a "z3" executable using the SMT-LIB2 format, or it can connect directly
 *  to the z3 shared library. The former is easier to debug, but the latter is much faster since it avoids translating to an
 *  intermediate text representation both when sending data to the solver and when getting data from the solver.  The mode is
 *  selected at runtime with the @ref linkage property. */
class Z3Solver: public SmtlibSolver {
public:
    /**  Construct Z3 solver preferring library linkage. */
    explicit Z3Solver(unsigned linkages = LM_ANY)
        : SmtlibSolver(ROSE_Z3) {
        name("Z3");
        linkage_ = bestLinkage(linkages & availableLinkages());
    }
    
    /** Returns a bit vector of linkage capabilities.
     *
     *  Returns a vector of @ref LinkMode bits that say what possible modes of communicating with the Z3 SMT solver are
     *  available. A return value of zero means the Z3 solver is not supported in this configuration of ROSE. */
    static unsigned availableLinkages();

    virtual SymbolicExpr::Ptr evidenceForName(const std::string&) ROSE_OVERRIDE;
    virtual std::vector<std::string> evidenceNames() ROSE_OVERRIDE;
    virtual void clearEvidence() ROSE_OVERRIDE;

protected:
    virtual void parseEvidence() ROSE_OVERRIDE;
    virtual void outputBvxorFunctions(std::ostream&, const std::vector<SymbolicExpr::Ptr>&) ROSE_OVERRIDE;
    virtual void outputComparisonFunctions(std::ostream&, const std::vector<SymbolicExpr::Ptr>&) ROSE_OVERRIDE;

private:
    virtual void outputExpression(std::ostream&, const SymbolicExpr::Ptr&) ROSE_OVERRIDE;
    virtual void outputArithmeticShiftRight(std::ostream&, const SymbolicExpr::InteriorPtr&) ROSE_OVERRIDE;
};

} // namespace
} // namespace

#endif
