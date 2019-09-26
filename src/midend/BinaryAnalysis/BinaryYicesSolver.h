#ifndef Rose_BinaryAnalysis_YicesSolver_H
#define Rose_BinaryAnalysis_YicesSolver_H

#include "rosePublicConfig.h"
#include "BinarySmtSolver.h"
#include <Sawyer/Map.h>
#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>

#ifdef ROSE_HAVE_LIBYICES
#  include <yices_c.h>
#endif

namespace Rose {
namespace BinaryAnalysis {


/** Interface to the Yices Satisfiability Modulo Theory (SMT) Solver.  ROSE should be configured with --with-yices in order
 *  for the satisfiable() virtual method to work (otherwise, the YicesSolver class is still available but will fail an
 *  assertion when instantiated).
 *
 *  Yices provides two interfaces: an executable named "yices", and a library. The choice of which linkage to use to answer
 *  satisfiability questions is made at runtime (see set_linkage()).
 */
class YicesSolver: public SmtSolver {
protected:
    typedef std::map<std::string/*name or hex-addr*/, std::pair<size_t/*nbits*/, uint64_t/*value*/> > Evidence;

private:
#ifdef ROSE_HAVE_LIBYICES
    yices_context context;
#endif
    ExprExprMap varsForSets_;                           // variables to use for sets
protected:
    Evidence evidence;

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SmtSolver);
        // varsForSets_ -- not saved
        // evidence     -- not saved
        // context      -- not saved
    }
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Construction-related things
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
    explicit YicesSolver(unsigned linkages = LM_ANY)
        : SmtSolver("yices", (LinkMode)(linkages & availableLinkages()))
#ifdef ROSE_HAVE_LIBYICES
          , context(NULL)
#endif
        {
        memoization(false);                             // not supported in this solver
    }

public:
    /** Constructs object to communicate with Yices solver.
     *
     *  The solver will be named "Yices" (see @ref name property) and will use the library linkage if the Yices library
     *  is present, otherwise the executable linkage. If neither is available then an @c SmtSolver::Exception is thrown. */
    static Ptr instance(unsigned linkages = LM_ANY) {
        return Ptr(new YicesSolver(linkages));
    }

    /** Virtual constructor.
     *
     *  Create a new solver just like this one. */
    virtual Ptr create() const ROSE_OVERRIDE {
        return instance(linkage());
    }
    
    /** Returns a bit vector of linkage capabilities.
     *
     *  Returns a vector of @ref LinkMode bits that say what possible modes of communicating with the Yices SMT solver are
     *  available. A return value of zero means the Yices solver is not supported in this configuration of ROSE. */
    static unsigned availableLinkages();

    // Reference counted object. Do not explicitly delete.
    virtual ~YicesSolver();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Overrides of the parent class
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    virtual void reset() ROSE_OVERRIDE;
    virtual void clearEvidence() ROSE_OVERRIDE;
    virtual std::vector<std::string> evidenceNames() ROSE_OVERRIDE;
    virtual SymbolicExpr::Ptr evidenceForName(const std::string&) ROSE_OVERRIDE;

protected:
    virtual Satisfiable checkLib() ROSE_OVERRIDE;
    virtual void generateFile(std::ostream&, const std::vector<SymbolicExpr::Ptr> &exprs, Definitions*) ROSE_OVERRIDE;
    virtual std::string getCommand(const std::string &config_name) ROSE_OVERRIDE;
    virtual void parseEvidence() ROSE_OVERRIDE;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Miscellaneous
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    void varForSet(const SymbolicExpr::InteriorPtr &set, const SymbolicExpr::LeafPtr &var);
    SymbolicExpr::LeafPtr varForSet(const SymbolicExpr::InteriorPtr &set);
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Convert a SymbolicExpr into Yices text input
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
    Type most_type(const std::vector<SExprTypePair>&);
    std::vector<SExprTypePair> out_exprs(const std::vector<SymbolicExpr::Ptr>&);
    std::vector<SExprTypePair> out_cast(const std::vector<SExprTypePair>&, Type toType);
    void out_comments(std::ostream&, const std::vector<SymbolicExpr::Ptr>&);
    void out_common_subexpressions(std::ostream&, const std::vector<SymbolicExpr::Ptr>&);
    void out_define(std::ostream&, const std::vector<SymbolicExpr::Ptr>&, Definitions*);
    void out_assert(std::ostream&, const SymbolicExpr::Ptr&);
    void out_number(std::ostream&, const SymbolicExpr::Ptr&);
    SExprTypePair out_cast(const SExprTypePair&, Type toType);
    SExprTypePair out_expr(const SymbolicExpr::Ptr&);
    SExprTypePair out_unary(const char *opname, const SExprTypePair&, Type rettype = NO_TYPE);
    SExprTypePair out_binary(const char *opname, const SymbolicExpr::InteriorPtr&, Type rettype = NO_TYPE);
    SExprTypePair out_ite(const SymbolicExpr::InteriorPtr&);
    SExprTypePair out_set(const SymbolicExpr::InteriorPtr&);
    SExprTypePair out_la(const char *opname, const SymbolicExpr::InteriorPtr&, Type rettype = NO_TYPE);
    SExprTypePair out_la(const char *opname, const std::vector<SExprTypePair>&, Type rettype = NO_TYPE);
    SExprTypePair out_extract(const SymbolicExpr::InteriorPtr&);
    SExprTypePair out_sext(const SymbolicExpr::InteriorPtr&);
    SExprTypePair out_uext(const SymbolicExpr::InteriorPtr&);
    SExprTypePair out_shift(const char *opname, const SymbolicExpr::InteriorPtr&, bool newbits);
    SExprTypePair out_asr(const SymbolicExpr::InteriorPtr&);
    SExprTypePair out_zerop(const SymbolicExpr::InteriorPtr&);
    SExprTypePair out_mult(const SymbolicExpr::InteriorPtr&);
    SExprTypePair out_read(const SymbolicExpr::InteriorPtr&);
    SExprTypePair out_write(const SymbolicExpr::InteriorPtr&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Convert a SymbolicExpr to Yices IR using the Yices API
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef ROSE_HAVE_LIBYICES
    typedef std::pair<yices_expr, Type> YExprTypePair;
    typedef Sawyer::Container::Map<SymbolicExpr::Ptr, YExprTypePair> TermExprs;
    TermExprs termExprs;                                // for common subexpressions

    /* These ctx_*() functions build a Yices context object if Yices is linked into this executable. */
    typedef yices_expr (*UnaryAPI)(yices_context, yices_expr operand);
    typedef yices_expr (*BinaryAPI)(yices_context, yices_expr operand1, yices_expr operand2);
    typedef yices_expr (*NaryAPI)(yices_context, yices_expr *operands, unsigned n_operands);
    typedef yices_expr (*ShiftAPI)(yices_context, yices_expr, unsigned amount);

    Type most_type(const std::vector<YExprTypePair>&);
    void ctx_common_subexpressions(const std::vector<SymbolicExpr::Ptr>&);
    void ctx_define(const std::vector<SymbolicExpr::Ptr>&, Definitions*);
    void ctx_assert(const SymbolicExpr::Ptr&);
    std::vector<YExprTypePair> ctx_exprs(const std::vector<SymbolicExpr::Ptr>&);
    YExprTypePair ctx_cast(const YExprTypePair&, Type toType);
    std::vector<YExprTypePair> ctx_cast(const std::vector<YExprTypePair>&, Type toType);
    YExprTypePair ctx_expr(const SymbolicExpr::Ptr&);
    YExprTypePair ctx_unary(UnaryAPI, const YExprTypePair&, Type rettype = NO_TYPE);
    YExprTypePair ctx_binary(BinaryAPI, const SymbolicExpr::InteriorPtr&, Type rettype = NO_TYPE);
    YExprTypePair ctx_ite(const SymbolicExpr::InteriorPtr&);
    YExprTypePair ctx_set(const SymbolicExpr::InteriorPtr&);
    YExprTypePair ctx_la(BinaryAPI, const SymbolicExpr::InteriorPtr&, Type rettype = NO_TYPE);
    YExprTypePair ctx_la(BinaryAPI, const std::vector<YExprTypePair>&, Type rettype = NO_TYPE);
    YExprTypePair ctx_la(NaryAPI, const SymbolicExpr::InteriorPtr&, Type rettype = NO_TYPE);
    YExprTypePair ctx_la(NaryAPI, const std::vector<YExprTypePair>&, Type rettype = NO_TYPE);
    YExprTypePair ctx_extract(const SymbolicExpr::InteriorPtr&);
    YExprTypePair ctx_sext(const SymbolicExpr::InteriorPtr&);
    YExprTypePair ctx_uext(const SymbolicExpr::InteriorPtr&);
    YExprTypePair ctx_shift(ShiftAPI, const SymbolicExpr::InteriorPtr&);
    YExprTypePair ctx_asr(const SymbolicExpr::InteriorPtr&);
    YExprTypePair ctx_zerop(const SymbolicExpr::InteriorPtr&);
    YExprTypePair ctx_mult(const SymbolicExpr::InteriorPtr&);
    YExprTypePair ctx_read(const SymbolicExpr::InteriorPtr&);
    YExprTypePair ctx_write(const SymbolicExpr::InteriorPtr&);
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // FIXME[Robb Matzke 2017-10-17]: these are all deprecated
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    static unsigned available_linkage() ROSE_DEPRECATED("use availableLinkages");
private:
    static std::string get_typename(const SymbolicExpr::Ptr&);
};

} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::YicesSolver);
#endif

#endif
