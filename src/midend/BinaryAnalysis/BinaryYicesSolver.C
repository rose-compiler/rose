#include "sage3basic.h"
#include <rosePublicConfig.h>

#include "rose_strtoull.h"
#include "BinaryYicesSolver.h"
#include "stringify.h"

#include <boost/regex.hpp>
#include <boost/thread/locks.hpp>
#include <errno.h>
#include <Sawyer/Stopwatch.h>

#ifdef _MSC_VER
#define strtoull _strtoui64
#endif

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {

YicesSolver::~YicesSolver() 
{
#ifdef ROSE_HAVE_LIBYICES
    if (context) {
        yices_del_context(context);
        context = NULL;
    }
#endif
}

void
YicesSolver::reset() {
    SmtSolver::reset();
    varsForSets_.clear();
}

// class method
unsigned
YicesSolver::availableLinkages()
{
    unsigned retval = 0;
#ifdef ROSE_HAVE_LIBYICES
    retval |= LM_LIBRARY;
#endif
#ifdef ROSE_YICES
    if (ROSE_YICES && ROSE_YICES[0] != '\0')
        retval |= LM_EXECUTABLE;
#endif
    return retval;
}

// FIXME[Robb Matzke 2017-10-17]: deprecated
unsigned
YicesSolver::available_linkage() {
    return availableLinkages();
}

/* See YicesSolver.h */
SmtSolver::Satisfiable
YicesSolver::checkLib() {
    requireLinkage(LM_LIBRARY);
    ASSERT_forbid(memoization());

#ifdef ROSE_HAVE_LIBYICES
    if (!context) {
        context = yices_mk_context();
        ASSERT_not_null(context);
    } else {
        yices_reset(context);
    }

#ifndef NDEBUG
    yices_enable_type_checker(true);
#endif

    Sawyer::Stopwatch prepareTimer;
    std::vector<SymbolicExpr::Ptr> exprs = assertions();
    Definitions defns;
    termExprs.clear();
    ctx_define(exprs, &defns);
    ctx_common_subexpressions(exprs);
    for (std::vector<SymbolicExpr::Ptr>::const_iterator ei=exprs.begin(); ei!=exprs.end(); ++ei)
        ctx_assert(*ei);
    stats.prepareTime += prepareTimer.stop();

    Sawyer::Stopwatch timer;
    switch (yices_check(context)) {
        case l_false:
            stats.solveTime += timer.stop();
            return SAT_NO;
        case l_true:
            stats.solveTime += timer.stop();
            return SAT_YES;
        case l_undef:
            stats.solveTime += timer.stop();
            return SAT_UNKNOWN;
    }
    ASSERT_not_reachable("switch statement is incomplete");
#else
    ASSERT_not_reachable("yices library not enabled");
#endif
}

/* See SmtSolver::get_command() */
std::string
YicesSolver::getCommand(const std::string &config_name)
{
#ifdef ROSE_YICES
    requireLinkage(LM_EXECUTABLE);
    return std::string(ROSE_YICES) + " --evidence --type-check " + config_name;
#else
    return "false no yices command";
#endif
}

void
YicesSolver::generateFile(std::ostream &o, const std::vector<SymbolicExpr::Ptr> &exprs, Definitions *defns)
{
    requireLinkage(LM_EXECUTABLE);
    ASSERT_forbid(memoization());
    Definitions *allocated = NULL;
    if (!defns)
        defns = allocated = new Definitions;
    termNames_.clear();
    varsForSets_.clear();

    o <<";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n"
      <<"; Uninterpreted variables\n"
      <<";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n";
    out_define(o, exprs, defns);

    o <<"\n"
      <<";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n"
      <<"; Common subexpressions\n"
      <<";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n";
    out_common_subexpressions(o, exprs);

    out_comments(o, exprs);

    o <<"\n"
      <<";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n"
      <<"; Assertions\n"
      <<";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n";
    for (std::vector<SymbolicExpr::Ptr>::const_iterator ei=exprs.begin(); ei!=exprs.end(); ++ei) {
        o <<"\n";
        const std::string &comment = (*ei)->comment();
        if (!comment.empty())
            o <<StringUtility::prefixLines(comment, "; ") <<"\n";
        o <<"; effective size = " <<StringUtility::plural((*ei)->nNodes(), "nodes")
          <<", actual size = " <<StringUtility::plural((*ei)->nNodesUnique(), "nodes") <<"\n";
        out_assert(o, *ei);
    }

    o <<"\n(check)\n";

    delete allocated;
}

void
YicesSolver::parseEvidence() {
    ASSERT_forbid(memoization());
    Sawyer::Stopwatch evidenceTimer;
    boost::regex varNameRe("v\\d+");
    boost::regex memNameRe("m\\d+");
    boost::regex binaryConstantRe("0b[01]+");
    BOOST_FOREACH (const SExpr::Ptr &sexpr, parsedOutput_) {
        if (sexpr->name() == "sat" || sexpr->name() == "unsat") {
            // "sat" and "unsat" are not evidence
        } else if (sexpr->children().size() == 3 &&
                   sexpr->children()[0]->name() == "=" &&
                   boost::regex_match(sexpr->children()[1]->name(), varNameRe) &&
                   boost::regex_match(sexpr->children()[2]->name(), binaryConstantRe)) {
            // (= v36 0b01101111111111111101011110110100)
            std::string varName = sexpr->children()[1]->name();
            std::string valStr = sexpr->children()[2]->name();
            size_t nBits = valStr.size() - 2;
            uint64_t value = rose_strtoull(valStr.c_str(), NULL, 0);
            ASSERT_require(evidence.find(varName) == evidence.end());
            evidence[varName] = std::make_pair(nBits, value);
            SAWYER_MESG(mlog[DEBUG]) <<"evidence: " <<varName <<" == " <<StringUtility::toHex2(value, nBits) <<"\n";
        } else if (sexpr->children().size() == 3 &&
                   sexpr->children()[0]->name() == "=" &&
                   sexpr->children()[1]->children().size() == 2 &&
                   boost::regex_match(sexpr->children()[1]->children()[0]->name(), memNameRe) &&
                   boost::regex_match(sexpr->children()[1]->children()[1]->name(), binaryConstantRe) &&
                   boost::regex_match(sexpr->children()[2]->name(), binaryConstantRe)) {
            // (= (m95 0b01000000000000011100111111110100) 0b00000000000000000100111100000100)
            std::string addrStr = sexpr->children()[1]->children()[1]->name();
            std::string valStr = sexpr->children()[2]->name();
            std::string addrName = StringUtility::addrToString(rose_strtoull(addrStr.c_str(), NULL, 0));
            size_t nBits = valStr.size() - 2;
            uint64_t value = rose_strtoull(valStr.c_str(), NULL, 0);
            ASSERT_require(evidence.find(addrName) == evidence.end());
            evidence[addrName] = std::make_pair(nBits, value);
            SAWYER_MESG(mlog[DEBUG]) <<"evidence: " <<addrName <<" == " <<StringUtility::toHex2(value, nBits) <<"\n";
        } else {
            mlog[ERROR] <<"malformed evidence: ";
            printSExpression(mlog[ERROR], sexpr);
            mlog[ERROR] <<"\n";
        }
    }
    stats.evidenceTime += evidenceTimer.stop();
}

std::vector<std::string>
YicesSolver::evidenceNames()
{
    std::vector<std::string> retval;
    for (Evidence::const_iterator ei=evidence.begin(); ei!=evidence.end(); ++ei)
        retval.push_back(ei->first);
    return retval;
}

SymbolicExpr::Ptr
YicesSolver::evidenceForName(const std::string &name)
{
    Evidence::const_iterator found = evidence.find(name);
    if (found==evidence.end())
        return SymbolicExpr::Ptr(); // null
    return SymbolicExpr::makeIntegerConstant(found->second.first/*nbits*/, found->second.second/*value*/);
}

void
YicesSolver::clearEvidence() {
    SmtSolver::clearEvidence();
    evidence.clear();
}

/* Emit type name for term. */
std::string
YicesSolver::get_typename(const SymbolicExpr::Ptr &expr) {
    ASSERT_not_null(expr);
    if (expr->isScalar())
        return "(bitvector " + StringUtility::numberToString(expr->nBits()) + ")";
    return ("(-> (bitvector " + StringUtility::numberToString(expr->domainWidth()) + ")"
            " (bitvector " + StringUtility::numberToString(expr->nBits()) + "))");
}

void
YicesSolver::varForSet(const SymbolicExpr::InteriorPtr &set, const SymbolicExpr::LeafPtr &var) {
    ASSERT_not_null(set);
    ASSERT_not_null(var);
    varsForSets_.insert(set, var);
}

SymbolicExpr::LeafPtr
YicesSolver::varForSet(const SymbolicExpr::InteriorPtr &set) {
    ASSERT_not_null(set);
    SymbolicExpr::Ptr expr = varsForSets_.getOrDefault(set);
    ASSERT_not_null(expr);
    SymbolicExpr::LeafPtr var = expr->isLeafNode();
    ASSERT_not_null(var);
    return var;
}

/* Traverse an expression and produce Yices "define" statements for variables. */
void
YicesSolver::out_define(std::ostream &o, const std::vector<SymbolicExpr::Ptr> &exprs, Definitions *defns) {
    ASSERT_not_null(defns);

    struct T1: SymbolicExpr::Visitor {
        YicesSolver *self;
        typedef std::set<const SymbolicExpr::Node*> SeenNodes;
        SeenNodes seen;
        std::ostream &o;
        Definitions *defns;

        T1(YicesSolver *self, std::ostream &o, Definitions *defns)
            : self(self), o(o), defns(defns) {}

        SymbolicExpr::VisitAction preVisit(const SymbolicExpr::Ptr &node) {
            if (!seen.insert(getRawPointer(node)).second)
                return SymbolicExpr::TRUNCATE;          // already processed this subexpression
            if (SymbolicExpr::LeafPtr leaf = node->isLeafNode()) {
                if (leaf->isIntegerVariable()) {
                    if (defns->find(leaf->nameId())==defns->end()) {
                        defns->insert(leaf->nameId());
                        o <<"\n";
                        if (!leaf->comment().empty())
                            o <<StringUtility::prefixLines(leaf->comment(), "; ") <<"\n";
                        o <<"(define v" <<leaf->nameId() <<"::" <<get_typename(leaf) <<")\n";
                    }
                } else if (leaf->isMemoryVariable()) {
                    if (defns->find(leaf->nameId())==defns->end()) {
                        defns->insert(leaf->nameId());
                        o <<"\n";
                        if (!leaf->comment().empty())
                            o <<StringUtility::prefixLines(leaf->comment(), "; ") <<"\n";
                        o <<"(define m" <<leaf->nameId() <<"::" <<get_typename(leaf) <<")\n";
                    }
                }
            } else if (SymbolicExpr::InteriorPtr inode = node->isInteriorNode()) {
                if (inode->getOperator() == SymbolicExpr::OP_SET) {
                    // Sets are ultimately converted to ITEs and therefore each set needs a free variable
                    SymbolicExpr::LeafPtr var = SymbolicExpr::makeIntegerVariable(32, "set")->isLeafNode();
                    self->varForSet(inode, var);
                    defns->insert(var->nameId());
                    o <<"\n";
                    o <<"(define v" <<var->nameId() <<"::" <<get_typename(var) <<")\n";
                }
            }
            return SymbolicExpr::CONTINUE;
        }

        SymbolicExpr::VisitAction postVisit(const SymbolicExpr::Ptr&) {
            return SymbolicExpr::CONTINUE;
        }
    } t1(this, o, defns);

    BOOST_FOREACH (const SymbolicExpr::Ptr &expr, exprs)
        expr->depthFirstTraversal(t1);
}

/* Generate definitions for common subexpressions. */
void
YicesSolver::out_common_subexpressions(std::ostream &o, const std::vector<SymbolicExpr::Ptr> &exprs) {
    std::vector<SymbolicExpr::Ptr> cses = findCommonSubexpressions(exprs);
    for (size_t i=0; i<cses.size(); ++i) {
        o <<"\n";
        if (!cses[i]->comment().empty())
            o <<StringUtility::prefixLines(cses[i]->comment(), "; ") <<"\n";
        o <<"; effective size = " <<StringUtility::plural(cses[i]->nNodes(), "nodes")
          <<", actual size = " <<StringUtility::plural(cses[i]->nNodesUnique(), "nodes") <<"\n";
        std::string termName = "cse_" + StringUtility::numberToString(i);
        o <<"(define " <<termName <<"::" <<get_typename(cses[i]) <<" ";
        if (cses[i]->isLeafNode()) {
            SExprTypePair et = out_expr(cses[i]);
            o <<*et.first;
            termNames_.insert(cses[i], StringTypePair(termName, et.second));
        }
        o <<")\n";
    }
}

//  Generate comments for certain expressions
void
YicesSolver::out_comments(std::ostream &o, const std::vector<SymbolicExpr::Ptr> &exprs) {
    struct T1: SymbolicExpr::Visitor {
        typedef std::set<const SymbolicExpr::Node*> SeenNodes;
        SeenNodes seen;
        std::ostream &o;
        bool commented;

        T1(std::ostream &o)
            : o(o), commented(false) {}

        SymbolicExpr::VisitAction preVisit(const SymbolicExpr::Ptr &node) {
            if (!seen.insert(getRawPointer(node)).second)
                return SymbolicExpr::TRUNCATE;          // already processed this subexpression
            if (SymbolicExpr::LeafPtr leaf = node->isLeafNode()) {
                if (leaf->isVariable2() && !leaf->comment().empty()) {
                    if (!commented) {
                        o <<"\n"
                          <<";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n"
                          <<"; Variable comments\n"
                          <<";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n";
                        commented = true;
                    }
                    o <<"\n; " <<leaf->toString() <<": "
                      <<StringUtility::prefixLines(leaf->comment(), ";    ", false) <<"\n";
                }
            }
            return SymbolicExpr::CONTINUE;
        }

        SymbolicExpr::VisitAction postVisit(const SymbolicExpr::Ptr&) {
            return SymbolicExpr::CONTINUE;
        }
    } t1(o);

    BOOST_FOREACH (const SymbolicExpr::Ptr &expr, exprs)
        expr->depthFirstTraversal(t1);
}

/* Generate a Yices "assert" statement for an expression. */
void
YicesSolver::out_assert(std::ostream &o, const SymbolicExpr::Ptr &tn)
{
    o <<"(assert ";
    if (tn->isIntegerConstant() && 1==tn->nBits()) {
        if (tn->toUnsigned().get()) {
            o <<"true";
        } else {
            o <<"false";
        }
    } else {
        SExpr::Ptr e = out_cast(out_expr(tn), BOOLEAN).first;
        o <<*e;
    }
    o <<")\n";
}

/* Output a decimal number. */
void
YicesSolver::out_number(std::ostream &o, const SymbolicExpr::Ptr &tn)
{
    SymbolicExpr::LeafPtr ln = tn->isLeafNode();
    ASSERT_require(ln && ln->isIntegerConstant());
    o <<ln->toUnsigned().get();
}

SmtSolver::SExprTypePair
YicesSolver::out_cast(const SExprTypePair &et, Type toType) {
    Type fromType = et.second;
    if (fromType == toType) {
        return et;
    } else if (BOOLEAN == fromType && BIT_VECTOR == toType) {
        SExpr::Ptr e =
            SExpr::instance(SExpr::instance("ite"),
                            et.first,
                            SExpr::instance("0b1"),
                            SExpr::instance("0b0"));
        return SExprTypePair(e, toType);
    } else if (BIT_VECTOR == fromType && BOOLEAN == toType) {
        SExpr::Ptr e =
            SExpr::instance(SExpr::instance("="),
                            et.first,
                            SExpr::instance("0b1"));
        return SExprTypePair(e, toType);
    } else {
        Stringifier string(stringifyBinaryAnalysisSmtSolverType);
        ASSERT_not_reachable("invalid cast from " + string(fromType) + " to " + string(toType));
    }
}

std::vector<SmtSolver::SExprTypePair>
YicesSolver::out_cast(const std::vector<SExprTypePair> &ets, Type toType) {
    std::vector<SExprTypePair> retval;
    retval.reserve(ets.size());
    BOOST_FOREACH (const SExprTypePair &et, ets)
        retval.push_back(out_cast(et, toType));
    return retval;
}

SmtSolver::Type
YicesSolver::most_type(const std::vector<SExprTypePair> &ets) {
    typedef Sawyer::Container::Map<Type, size_t> Histogram;
    Histogram histogram;
    BOOST_FOREACH (const SExprTypePair &et, ets)
        ++histogram.insertMaybe(et.second, 0);
    Type bestType = NO_TYPE;
    size_t bestCount = 0;
    BOOST_FOREACH (const Histogram::Node &node, histogram.nodes()) {
        if (node.value() > bestCount) {
            bestType = node.key();
            bestCount = node.value();
        }
    }
    return bestType;
}

#ifdef ROSE_HAVE_LIBYICES
SmtSolver::Type
YicesSolver::most_type(const std::vector<YExprTypePair> &ets) {
    typedef Sawyer::Container::Map<Type, size_t> Histogram;
    Histogram histogram;
    BOOST_FOREACH (const YExprTypePair &et, ets)
        ++histogram.insertMaybe(et.second, 0);
    Type bestType = NO_TYPE;
    size_t bestCount = 0;
    BOOST_FOREACH (const Histogram::Node &node, histogram.nodes()) {
        if (node.value() > bestCount) {
            bestType = node.key();
            bestCount = node.value();
        }
    }
    return bestType;
}
#endif

/* Output for one expression. */
SmtSolver::SExprTypePair
YicesSolver::out_expr(const SymbolicExpr::Ptr &tn) {
    typedef std::vector<SExprTypePair> Etv;

    SymbolicExpr::LeafPtr ln = tn->isLeafNode();
    SymbolicExpr::InteriorPtr in = tn->isInteriorNode();
    StringTypePair nameType;
    if (termNames_.getOptional(tn).assignTo(nameType)) {
        return SExprTypePair(SExpr::instance(nameType.first), nameType.second);
        
    } else if (ln) {
        if (ln->isIntegerConstant()) {
            if (ln->nBits() <= 64) {
                SExpr::Ptr e =
                    SExpr::instance(SExpr::instance("mk-bv"),
                                    SExpr::instance(ln->nBits()),
                                    SExpr::instance(ln->toUnsigned().get()));
                return SExprTypePair(e, BIT_VECTOR);
            } else {
                SExpr::Ptr e = SExpr::instance("0b" + ln->bits().toBinary());
                return SExprTypePair(e, BIT_VECTOR);
            }
        } else if (ln->isMemoryVariable()) {
            return SExprTypePair(SExpr::instance(ln->toString()), MEM_STATE);
        } else {
            ASSERT_require(ln->isIntegerVariable());
            return SExprTypePair(SExpr::instance(ln->toString()), BIT_VECTOR);
        }
    } else {
        ASSERT_not_null(in);
        switch (in->getOperator()) {
            case SymbolicExpr::OP_NONE:
                ASSERT_not_reachable("not possible for an interior node");
            case SymbolicExpr::OP_ADD:
                return out_la("bv-add", in, BIT_VECTOR);
            case SymbolicExpr::OP_AND: {
                Etv children = out_exprs(in->children());
                Type type = most_type(children);
                children = out_cast(children, type);
                if (BOOLEAN == type) {
                    return out_la("and", children);
                } else {
                    ASSERT_require(BIT_VECTOR == type);
                    return out_la("bv-and", children);
                }
            }
            case SymbolicExpr::OP_ASR:
                return out_asr(in);
            case SymbolicExpr::OP_EQ:
                return out_binary("=", in, BOOLEAN);
            case SymbolicExpr::OP_CONCAT: {
                Etv children = out_cast(out_exprs(in->children()), BIT_VECTOR);
                return out_la("bv-concat", children);
            }
            case SymbolicExpr::OP_EXTRACT:
                return out_extract(in);
            case SymbolicExpr::OP_INVERT: {
                SExprTypePair child = out_expr(in->child(0));
                return out_unary((BOOLEAN==child.second?"not":"bv-not"), child);
            }
            case SymbolicExpr::OP_ITE:
                return out_ite(in);
            case SymbolicExpr::OP_LET:
                throw Exception("OP_LET not implemented");
            case SymbolicExpr::OP_LSSB:
                throw Exception("OP_LSSB not implemented");
            case SymbolicExpr::OP_MSSB:
                throw Exception("OP_MSSB not implemented");
            case SymbolicExpr::OP_NE:
                return out_binary("/=", in, BOOLEAN);
            case SymbolicExpr::OP_NEGATE: {
                SExprTypePair child = out_cast(out_expr(in->child(0)), BIT_VECTOR);
                return out_unary("bv-neg", child);
            }
            case SymbolicExpr::OP_NOOP:
                return out_expr(SymbolicExpr::makeIntegerConstant(in->nBits(), 0));
            case SymbolicExpr::OP_OR: {
                Etv children = out_exprs(in->children());
                Type type = most_type(children);
                children = out_cast(children, type);
                if (BOOLEAN == type) {
                    return out_la("or", children);
                } else {
                    ASSERT_require(BIT_VECTOR == type);
                    return out_la("bv-or", children);
                }
            }
            case SymbolicExpr::OP_READ:
                return out_read(in);
            case SymbolicExpr::OP_ROL:
                throw Exception("OP_ROL not implemented");
            case SymbolicExpr::OP_ROR:
                throw Exception("OP_ROR not implemented");
            case SymbolicExpr::OP_SDIV:
                throw Exception("OP_SDIV not implemented");
            case SymbolicExpr::OP_SET:
                return out_set(in);
            case SymbolicExpr::OP_SEXTEND:
                return out_sext(in);
            case SymbolicExpr::OP_SLT:
                return out_binary("bv-slt", in, BOOLEAN);
            case SymbolicExpr::OP_SLE:
                return out_binary("bv-sle", in, BOOLEAN);
            case SymbolicExpr::OP_SHL0:
                return out_shift("bv-shift-left", in, false);
            case SymbolicExpr::OP_SHL1:
                return out_shift("bv-shift-left", in, true);
            case SymbolicExpr::OP_SHR0:
                return out_shift("bv-shift-right", in, false);
            case SymbolicExpr::OP_SHR1:
                return out_shift("bv-shift-right", in, true);
            case SymbolicExpr::OP_SGE:
                return out_binary("bv-sge", in, BOOLEAN);
            case SymbolicExpr::OP_SGT:
                return out_binary("bv-sgt", in, BOOLEAN);
            case SymbolicExpr::OP_SMOD:
                throw Exception("OP_SMOD not implemented");
            case SymbolicExpr::OP_SMUL:
                return out_mult(in);
            case SymbolicExpr::OP_UDIV:
                throw Exception("OP_UDIV not implemented");
            case SymbolicExpr::OP_UEXTEND:
                return out_uext(in);
            case SymbolicExpr::OP_UGE:
                return out_binary("bv-ge", in, BOOLEAN);
            case SymbolicExpr::OP_UGT:
                return out_binary("bv-gt", in, BOOLEAN);
            case SymbolicExpr::OP_ULE:
                return out_binary("bv-le", in, BOOLEAN);
            case SymbolicExpr::OP_ULT:
                return out_binary("bv-lt", in, BOOLEAN);
            case SymbolicExpr::OP_UMOD:
                throw Exception("OP_UMOD not implemented");
            case SymbolicExpr::OP_UMUL:
                return out_mult(in);
            case SymbolicExpr::OP_WRITE:
                return out_write(in);
            case SymbolicExpr::OP_XOR: {
                // Yices doesn't have a Boolean XOR function, so use the bit-vector one instead.
                Etv children = out_cast(out_exprs(in->children()), BIT_VECTOR);
                return out_la("bv-xor", children);
            }
            case SymbolicExpr::OP_ZEROP:
                return out_zerop(in);
        }
    }
    ASSERT_not_reachable("expression type not handled");
}

/* Output for unary operators. */
SmtSolver::SExprTypePair
YicesSolver::out_unary(const char *opname, const SExprTypePair &a, Type rettype) {
    ASSERT_require(opname && *opname);

    SExpr::Ptr e = SExpr::instance(SExpr::instance(opname), a.first);
    if (NO_TYPE == rettype)
        rettype = a.second;
    return SExprTypePair(e, rettype);
}

std::vector<SmtSolver::SExprTypePair>
YicesSolver::out_exprs(const std::vector<SymbolicExpr::Ptr> &exprs) {
    std::vector<SExprTypePair> retval;
    retval.reserve(exprs.size());
    BOOST_FOREACH (const SymbolicExpr::Ptr &expr, exprs)
        retval.push_back(out_expr(expr));
    return retval;
}

/* Output for binary operators. */
SmtSolver::SExprTypePair
YicesSolver::out_binary(const char *opname, const SymbolicExpr::InteriorPtr &in, Type rettype) {
    ASSERT_require(opname && *opname);
    ASSERT_require(in && 2==in->nChildren());

    std::vector<SExprTypePair> operands;
    operands.push_back(out_expr(in->child(0)));
    operands.push_back(out_expr(in->child(1)));
    operands = out_cast(operands, most_type(operands));
    if (NO_TYPE == rettype)
        rettype = operands[0].second;

    SExpr::Ptr e = SExpr::instance(SExpr::instance(opname), operands[0].first, operands[1].first);
    return SExprTypePair(e, rettype);
}

/* Output for if-then-else operator.  The condition must be cast from a 1-bit vector to a number, therefore, the input
 *     (OP_ITE COND S1 S2)
 *
 * will be rewritten as
 *     (ite (= COND 0b1) S1 S2)
 */
SmtSolver::SExprTypePair
YicesSolver::out_ite(const SymbolicExpr::InteriorPtr &in) {
    ASSERT_require(in && 3==in->nChildren());
    ASSERT_require(in->child(0)->nBits()==1);
    std::vector<SExprTypePair> alternatives;
    alternatives.push_back(out_expr(in->child(1)));
    alternatives.push_back(out_expr(in->child(2)));
    alternatives = out_cast(alternatives, most_type(alternatives));

    SExpr::Ptr e =
        SExpr::instance(SExpr::instance("ite"),
                        out_cast(out_expr(in->child(0)), BOOLEAN).first,
                        alternatives[0].first, alternatives[1].first);
    return SExprTypePair(e, alternatives[0].second);
}

// Output for "set" operator. (set a b c) gets translated to the symbolic expression (ite v1 a (ite v1 b c)), which
// in turn gets translated to Yices output.
SmtSolver::SExprTypePair
YicesSolver::out_set(const SymbolicExpr::InteriorPtr &in) {
    ASSERT_not_null(in);
    ASSERT_require(in->getOperator() == SymbolicExpr::OP_SET);
    ASSERT_require(in->nChildren() >= 2);
    SymbolicExpr::LeafPtr var = varForSet(in);
    SymbolicExpr::Ptr ite = SymbolicExpr::setToIte(in, SmtSolverPtr(), var);
    ite->comment(in->comment());
    return out_expr(ite);
}

/* Output for left-associative, binary operators. The identity_element is sign-extended and used as the second operand
 * if only one operand is supplied. */
SmtSolver::SExprTypePair
YicesSolver::out_la(const char *opname, const SymbolicExpr::InteriorPtr &in, Type rettype) {
    std::vector<SExprTypePair> children = out_exprs(in->children());
    return out_la(opname, children, rettype);
}

SmtSolver::SExprTypePair
YicesSolver::out_la(const char *opname, const std::vector<SExprTypePair> &children, Type rettype) {
    ASSERT_require(opname && *opname);
    ASSERT_require(children.size() >= 2);
    Type childType = most_type(children);

    SExpr::Ptr e = out_cast(children[0], childType).first;
    for (size_t i = 1; i < children.size(); ++i) {
        SExpr::Ptr child = out_cast(children[i], childType).first;
        e = SExpr::instance(SExpr::instance(opname), e, child);
    }

    if (NO_TYPE == rettype)
        rettype = childType;
    return SExprTypePair(e, rettype);
}

/* Output for extract. Yices bv-extract first two arguments must be constants. */
SmtSolver::SExprTypePair
YicesSolver::out_extract(const SymbolicExpr::InteriorPtr &in) {
    ASSERT_require(in && 3==in->nChildren());
    ASSERT_require(in->child(0)->isIntegerConstant());
    ASSERT_require(in->child(1)->isIntegerConstant());
    ASSERT_require(in->child(0)->toUnsigned().get() < in->child(1)->toUnsigned().get());
    size_t lo = in->child(0)->toUnsigned().get();
    size_t hi = in->child(1)->toUnsigned().get() - 1;   /*inclusive*/

    SExpr::Ptr e =
        SExpr::instance(SExpr::instance("bv-extract"),
                        SExpr::instance(hi),
                        SExpr::instance(lo),
                        out_cast(out_expr(in->child(2)), BIT_VECTOR).first);

    return SExprTypePair(e, BIT_VECTOR);
}

/* Output for sign-extend. The second argument for yices' bv-sign-extend function is the number of bits by which the first
 * argument should be extended.  We compute that from the first argument of the OP_SEXTEND operator (the new size) and the
 * size of the second operand (the bit vector to be extended). */
SmtSolver::SExprTypePair
YicesSolver::out_sext(const SymbolicExpr::InteriorPtr &in) {
    ASSERT_require(in && 2==in->nChildren());
    ASSERT_require(in->child(0)->isIntegerConstant()); /*Yices bv-sign-extend needs a number for the second operand*/
    ASSERT_require(in->child(0)->toUnsigned().get() > in->child(1)->nBits());
    size_t extend_by = in->child(0)->toUnsigned().get() - in->child(1)->nBits();

    SExpr::Ptr e =
        SExpr::instance(SExpr::instance("bv-sign-extend"),
                        out_cast(out_expr(in->child(1)), BIT_VECTOR).first,
                        SExpr::instance(extend_by));

    return SExprTypePair(e, BIT_VECTOR);
}

/* Output for unsigned-extend.  ROSE's (OP_UEXT NewSize Vector) is rewritten to
 *
 * (bv-concat (mk-bv [NewSize-OldSize] 0) Vector)
 */
SmtSolver::SExprTypePair
YicesSolver::out_uext(const SymbolicExpr::InteriorPtr &in) {
    ASSERT_require(in && 2==in->nChildren());
    ASSERT_require(in->child(0)->isIntegerConstant()); /*Yices mk-bv needs a number for the size operand*/
    ASSERT_require(in->child(0)->toUnsigned().get() > in->child(1)->nBits());
    size_t extend_by = in->child(0)->toUnsigned().get() - in->child(1)->nBits();

    SExpr::Ptr e =
        SExpr::instance(SExpr::instance("bv-concat"),
                        SExpr::instance(SExpr::instance("mk-bv"),
                                        SExpr::instance(extend_by),
                                        SExpr::instance(0)),
                        out_cast(out_expr(in->child(1)), BIT_VECTOR).first);

    return SExprTypePair(e, BIT_VECTOR);
}

/* Output for shift operators. */
SmtSolver::SExprTypePair
YicesSolver::out_shift(const char *opname, const SymbolicExpr::InteriorPtr &in, bool newbits) {
    ASSERT_require(opname && *opname);
    ASSERT_require(in && 2==in->nChildren());
    ASSERT_require(in->child(0)->isIntegerConstant()); /*Yices' bv-shift-* operators need a constant for the shift amount*/

    SExpr::Ptr e =
        SExpr::instance(SExpr::instance(std::string(opname) + (newbits ? "1" : "0")),
                        out_cast(out_expr(in->child(1)), BIT_VECTOR).first,
                        SExpr::instance(in->child(0)->toUnsigned().get()));

    return SExprTypePair(e, BIT_VECTOR);
}

/* Output for arithmetic right shift.  Yices doesn't have a sign-extending right shift, therefore we implement it in terms of
 * other operations.  (OP_ASR ShiftAmount Vector) becomes
 *
 * (ite (= (mk-bv 1 1) (bv-extract [VectorSize-1] [VectorSize-1] Vector)) ;; vector's sign bit
 *      (bv-shift-right1 Vector [ShiftAmount])
 *      (bv-shift-right0 Vector [ShiftAmount]))
 *
 *where [VectorSize], [VectorSize-1], and [ShiftAmount] are numeric constants.
 */
SmtSolver::SExprTypePair
YicesSolver::out_asr(const SymbolicExpr::InteriorPtr &in) {
    ASSERT_require(in && 2==in->nChildren());
    SymbolicExpr::Ptr vector = in->child(1);
    uint64_t vector_size = vector->nBits();
    ASSERT_require(in->child(0)->isIntegerConstant());
    uint64_t shift_amount = in->child(0)->toUnsigned().get();

    SExpr::Ptr e =
        SExpr::instance(SExpr::instance("ite"),
                        SExpr::instance(SExpr::instance("="),
                                        SExpr::instance(SExpr::instance("mk-bv"),
                                                        SExpr::instance(1),
                                                        SExpr::instance(1)),
                                        SExpr::instance(SExpr::instance("bv-extract"),
                                                        SExpr::instance(vector_size-1),
                                                        SExpr::instance(vector_size-1),
                                                        out_cast(out_expr(vector), BIT_VECTOR).first)),
                        SExpr::instance(SExpr::instance("bv-shift-right1"),
                                        out_cast(out_expr(vector), BIT_VECTOR).first,
                                        SExpr::instance(shift_amount)),
                        SExpr::instance(SExpr::instance("bv-shift-right0"),
                                        out_cast(out_expr(vector), BIT_VECTOR).first,
                                        SExpr::instance(shift_amount)));

    return SExprTypePair(e, BIT_VECTOR);
}

/* Output for zero comparison. Result should be a single bit:
 *      (OP_ZEROP X)
 *
 *  becomes
 *      (ite (= (mk-bv [sizeof(X)] 0) [X]) 0b1 0b0)
 */
SmtSolver::SExprTypePair
YicesSolver::out_zerop(const SymbolicExpr::InteriorPtr &in) {
    ASSERT_require(in && 1==in->nChildren());

    SExpr::Ptr e =
        SExpr::instance(SExpr::instance("="),
                        SExpr::instance(SExpr::instance("mk-bv"),
                                        SExpr::instance(in->child(0)->nBits()),
                                        SExpr::instance(0)),
                        out_cast(out_expr(in->child(0)), BIT_VECTOR).first);

    return SExprTypePair(e, BOOLEAN);
}

/* Output for multiply. The OP_SMUL and OP_UMUL nodes of SymbolicExpr define the result width to be the sum of the input
 * widths. Yices' bv-mul operator requires that both operands are the same size and the result is the size of each operand.
 * Therefore, we rewrite (OP_SMUL A B) to become, in Yices:
 *    (bv-mul (bv-sign-extend A |B|) (bv-sign-extend B |A|))
 */
SmtSolver::SExprTypePair
YicesSolver::out_mult(const SymbolicExpr::InteriorPtr &in) {
    ASSERT_require(in->nBits() == in->child(0)->nBits() + in->child(1)->nBits());
    size_t extend0 = in->child(1)->nBits(); // amount by which to extend arg0
    size_t extend1 = in->child(0)->nBits(); // amount by which to extend arg1

    SExpr::Ptr e =
        SExpr::instance(SExpr::instance("bv-mul"),
                        SExpr::instance(SExpr::instance("bv-sign-extend"),
                                        out_cast(out_expr(in->child(0)), BIT_VECTOR).first,
                                        SExpr::instance(extend0)),
                        SExpr::instance(SExpr::instance("bv-sign-extend"),
                                        out_cast(out_expr(in->child(1)), BIT_VECTOR).first,
                                        SExpr::instance(extend1)));

    return SExprTypePair(e, BIT_VECTOR);
}

/* Output for write. */
SmtSolver::SExprTypePair
YicesSolver::out_write(const SymbolicExpr::InteriorPtr &in) {
    SExpr::Ptr e =
        SExpr::instance(SExpr::instance("update"),
                        out_cast(out_expr(in->child(0)), MEM_STATE).first,
                        SExpr::instance(out_cast(out_expr(in->child(1)), BIT_VECTOR).first),
                        out_cast(out_expr(in->child(2)), BIT_VECTOR).first);

    return SExprTypePair(e, MEM_STATE);
}

/* Output for read. */
SmtSolver::SExprTypePair
YicesSolver::out_read(const SymbolicExpr::InteriorPtr &in) {
    SExpr::Ptr e =
        SExpr::instance(out_cast(out_expr(in->child(0)), MEM_STATE).first,
                        out_cast(out_expr(in->child(1)), BIT_VECTOR).first);

    return SExprTypePair(e, BIT_VECTOR);
}

#ifdef ROSE_HAVE_LIBYICES
/* Traverse an expression and define Yices variables. */
void
YicesSolver::ctx_define(const std::vector<SymbolicExpr::Ptr> &exprs, Definitions *defns)
{
    struct T1: SymbolicExpr::Visitor {
        typedef std::set<const SymbolicExpr::Node*> SeenNodes;
        SeenNodes seen;
        YicesSolver *self;
        Definitions *defns;

        T1(YicesSolver *self, Definitions *defns): self(self), defns(defns) {}

        SymbolicExpr::VisitAction preVisit(const SymbolicExpr::Ptr &node) {
            if (!seen.insert(getRawPointer(node)).second)
                return SymbolicExpr::TRUNCATE;          // already processed this subexpression
            if (SymbolicExpr::LeafPtr leaf = node->isLeafNode()) {
                if (leaf->isVariable()) {
                    if (defns->find(leaf->nameId()) == defns->end()) {
                        defns->insert(leaf->nameId());
                        yices_type bvtype = yices_mk_bitvector_type(self->context, leaf->nBits());
                        ASSERT_not_null(bvtype);
                        std::string name = "v" + StringUtility::numberToString(leaf->nameId());
                        yices_var_decl vdecl __attribute__((unused)) = yices_mk_var_decl(self->context, name.c_str(), bvtype);
                        ASSERT_not_null(vdecl);
                    }
                } else if (leaf->isMemory()) {
                    if (defns->find(leaf->nameId()) == defns->end()) {
                        defns->insert(leaf->nameId());
                        yices_type domain = yices_mk_bitvector_type(self->context, leaf->domainWidth());
                        yices_type range = yices_mk_bitvector_type(self->context, leaf->nBits());
                        yices_type ftype = yices_mk_function_type(self->context, &domain, 1, range);
                        ASSERT_not_null(ftype);
                        std::string name = "m" + StringUtility::numberToString(leaf->nameId());
                        yices_var_decl vdecl __attribute__((unused)) = yices_mk_var_decl(self->context, name.c_str(), ftype);
                        ASSERT_not_null(vdecl);
                    }
                }
            } else if (SymbolicExpr::InteriorPtr inode = node->isInteriorNode()) {
                if (inode->getOperator() == SymbolicExpr::OP_SET) {
                    // Sets are ultimately converted to ITEs and therefore each set needs a free variable.
                    SymbolicExpr::LeafPtr var = SymbolicExpr::makeVariable(32, "set")->isLeafNode();
                    defns->insert(var->nameId());
                    yices_type bvtype = yices_mk_bitvector_type(self->context, var->nBits());
                    ASSERT_not_null(bvtype);
                    std::string name = var->toString();
                    yices_var_decl vdecl __attribute__((unused)) = yices_mk_var_decl(self->context, name.c_str(), bvtype);
                    ASSERT_not_null(vdecl);
                    self->varForSet(inode, var);
                }
            }
            return SymbolicExpr::CONTINUE;
        }

        SymbolicExpr::VisitAction postVisit(const SymbolicExpr::Ptr&) {
            return SymbolicExpr::CONTINUE;
        }
    } t1(this, defns);

    BOOST_FOREACH (const SymbolicExpr::Ptr &expr, exprs)
        expr->depthFirstTraversal(t1);
}
#endif

#ifdef ROSE_HAVE_LIBYICES
void
YicesSolver::ctx_common_subexpressions(const std::vector<SymbolicExpr::Ptr> &exprs) {
    std::vector<SymbolicExpr::Ptr> cses = findCommonSubexpressions(exprs);
    BOOST_FOREACH (const SymbolicExpr::Ptr &cse, cses)
        termExprs.insert(cse, ctx_expr(cse));
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/* Assert a constraint in the logical context. */
void
YicesSolver::ctx_assert(const SymbolicExpr::Ptr &tn)
{
    if (tn->isIntegerConstant() && tn->nBits()==1) {
        ASSERT_not_implemented("[Robb Matzke 2015-10-15]"); // see out_assert for similar code
    } else {
        yices_expr e = ctx_cast(ctx_expr(tn), BOOLEAN).first;
        ASSERT_not_null(e);
        yices_assert(context, e);
    }
}
#endif

#ifdef ROSE_HAVE_LIBYICES
YicesSolver::YExprTypePair
YicesSolver::ctx_cast(const YExprTypePair &et, Type toType) {
    Type fromType = et.second;
    if (fromType == toType) {
        return et;
    } else if (BOOLEAN == fromType && BIT_VECTOR == toType) {
        yices_expr e = yices_mk_ite(context,
                                    et.first,
                                    yices_mk_bv_constant(context, 1, 1),
                                    yices_mk_bv_constant(context, 1, 0));
        return YExprTypePair(e, toType);
    } else if (BIT_VECTOR == fromType && BOOLEAN == toType) {
        yices_expr e = yices_mk_eq(context,
                                   et.first,
                                   yices_mk_bv_constant(context, 1, 1));
        return YExprTypePair(e, toType);
    } else {
        Stringifier string(stringifyBinaryAnalysisSmtSolverType);
        ASSERT_not_reachable("invalid cast from " + string(fromType) + " to " + string(toType));
    }
}

std::vector<YicesSolver::YExprTypePair>
YicesSolver::ctx_cast(const std::vector<YExprTypePair> &ets, Type toType) {
    std::vector<YExprTypePair> retval;
    retval.reserve(ets.size());
    BOOST_FOREACH (const YExprTypePair &et, ets)
        retval.push_back(ctx_cast(et, toType));
    return retval;
}

/* Builds a Yices expression from a ROSE expression. */
YicesSolver::YExprTypePair
YicesSolver::ctx_expr(const SymbolicExpr::Ptr &tn) {
    typedef std::vector<YExprTypePair> Etv;
    SymbolicExpr::LeafPtr ln = tn->isLeafNode();
    SymbolicExpr::InteriorPtr in = tn->isInteriorNode();


    YExprTypePair found;
    if (termExprs.getOptional(tn).assignTo(found)) {
        return found;
    } else if (ln) {
        if (ln->isIntegerConstant()) {
            if (ln->nBits() <= 64) {
                return YExprTypePair(yices_mk_bv_constant(context, ln->nBits(), ln->toInt()), BIT_VECTOR);
            } else {
                int tmp[ln->nBits()];
                for (size_t i=0; i<ln->nBits(); ++i)
                    tmp[i] = ln->bits().get(i) ? 1 : 0;
                return YExprTypePair(yices_mk_bv_constant_from_array(context, ln->nBits(), tmp), BIT_VECTOR);
            }
        } else if (ln->isVariable()) {
            yices_var_decl vdecl = yices_get_var_decl_from_name(context, ln->toString().c_str());
            ASSERT_not_null(vdecl);
            return YExprTypePair(yices_mk_var_from_decl(context, vdecl), BIT_VECTOR);
        } else {
            ASSERT_require(ln->isMemory());
            yices_var_decl vdecl = yices_get_var_decl_from_name(context, ln->toString().c_str());
            ASSERT_not_null(vdecl);
            return YExprTypePair(yices_mk_var_from_decl(context, vdecl), MEM_STATE);
        }
    } else {
        ASSERT_not_null(in);
        switch (in->getOperator()) {
            case SymbolicExpr::OP_ADD:
                return ctx_la(yices_mk_bv_add, in, BIT_VECTOR);
            case SymbolicExpr::OP_AND: {
                Etv children = ctx_exprs(in->children());
                Type type = most_type(children);
                children = ctx_cast(children, type);
                if (BOOLEAN == type) {
                    return ctx_la(yices_mk_and, children);
                } else {
                    ASSERT_require(BIT_VECTOR == type);
                    return ctx_la(yices_mk_bv_and, children);
                }
            }
            case SymbolicExpr::OP_ASR:
                return ctx_asr(in);
            case SymbolicExpr::OP_EQ:
                return ctx_binary(yices_mk_eq, in, BOOLEAN);
            case SymbolicExpr::OP_CONCAT:
                return ctx_la(yices_mk_bv_concat, in, BIT_VECTOR);
            case SymbolicExpr::OP_EXTRACT:
                return ctx_extract(in);
            case SymbolicExpr::OP_INVERT: {
                YExprTypePair child = ctx_expr(in->child(0));
                return ctx_unary(BOOLEAN == child.second ? yices_mk_not : yices_mk_bv_not, child);
            }
            case SymbolicExpr::OP_ITE:
                return ctx_ite(in);
            case SymbolicExpr::OP_LSSB:
                throw Exception("OP_LSSB not implemented");
            case SymbolicExpr::OP_MSSB:
                throw Exception("OP_MSSB not implemented");
            case SymbolicExpr::OP_NE:
                return ctx_binary(yices_mk_diseq, in, BOOLEAN);
            case SymbolicExpr::OP_NEGATE: {
                YExprTypePair child = ctx_cast(ctx_expr(in->child(0)), BIT_VECTOR);
                return ctx_unary(yices_mk_bv_minus, child);
            }
            case SymbolicExpr::OP_NOOP:
                throw Exception("OP_NOOP not implemented");
            case SymbolicExpr::OP_OR: {
                Etv children = ctx_exprs(in->children());
                Type type = most_type(children);
                children = ctx_cast(children, type);
                if (BOOLEAN == type) {
                    return ctx_la(yices_mk_or, children);
                } else {
                    ASSERT_require(BIT_VECTOR == type);
                    return ctx_la(yices_mk_bv_or, children);
                }
            }
            case SymbolicExpr::OP_READ:
                return ctx_read(in);
            case SymbolicExpr::OP_ROL:
                throw Exception("OP_ROL not implemented");
            case SymbolicExpr::OP_ROR:
                throw Exception("OP_ROR not implemented");
            case SymbolicExpr::OP_SET:
                return ctx_set(in);
            case SymbolicExpr::OP_SDIV:
                throw Exception("OP_SDIV not implemented");
            case SymbolicExpr::OP_SEXTEND:
                return ctx_sext(in);
            case SymbolicExpr::OP_SLT:
                return ctx_binary(yices_mk_bv_slt, in, BOOLEAN);
            case SymbolicExpr::OP_SLE:
                return ctx_binary(yices_mk_bv_sle, in, BOOLEAN);
            case SymbolicExpr::OP_SHL0:
                return ctx_shift(yices_mk_bv_shift_left0, in);
            case SymbolicExpr::OP_SHL1:
                return ctx_shift(yices_mk_bv_shift_left1, in);
            case SymbolicExpr::OP_SHR0:
                return ctx_shift(yices_mk_bv_shift_right0, in);
            case SymbolicExpr::OP_SHR1:
                return ctx_shift(yices_mk_bv_shift_right1, in);
            case SymbolicExpr::OP_SGE:
                return ctx_binary(yices_mk_bv_sge, in, BOOLEAN);
            case SymbolicExpr::OP_SGT:
                return ctx_binary(yices_mk_bv_sgt, in, BOOLEAN);
            case SymbolicExpr::OP_SMOD:
                throw Exception("OP_SMOD not implemented");
            case SymbolicExpr::OP_SMUL:
                return ctx_mult(in);
            case SymbolicExpr::OP_UDIV:
                throw Exception("OP_UDIV not implemented");
            case SymbolicExpr::OP_UEXTEND:
                return ctx_uext(in);
            case SymbolicExpr::OP_UGE:
                return ctx_binary(yices_mk_bv_ge, in, BOOLEAN);
            case SymbolicExpr::OP_UGT:
                return ctx_binary(yices_mk_bv_gt, in, BOOLEAN);
            case SymbolicExpr::OP_ULE:
                return ctx_binary(yices_mk_bv_le, in, BOOLEAN);
            case SymbolicExpr::OP_ULT:
                return ctx_binary(yices_mk_bv_lt, in, BOOLEAN);
            case SymbolicExpr::OP_UMOD:
                throw Exception("OP_UMOD not implemented");
            case SymbolicExpr::OP_UMUL:
                return ctx_mult(in);
            case SymbolicExpr::OP_WRITE:
                return ctx_write(in);
            case SymbolicExpr::OP_XOR: {
                // Yices doesn't have a Boolean XOR function, so use the bit-vector one instead
                Etv children = ctx_cast(ctx_exprs(in->children()), BIT_VECTOR);
                return ctx_la(yices_mk_bv_xor, children, BIT_VECTOR);
            }
            case SymbolicExpr::OP_ZEROP:
                return ctx_zerop(in);
        }
    }
    ASSERT_not_reachable("expression type not handled");
}

std::vector<YicesSolver::YExprTypePair>
YicesSolver::ctx_exprs(const std::vector<SymbolicExpr::Ptr> &exprs) {
    std::vector<YExprTypePair> retval;
    retval.reserve(exprs.size());
    BOOST_FOREACH (const SymbolicExpr::Ptr &expr, exprs)
        retval.push_back(ctx_expr(expr));
    return retval;
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/* Create Yices expression from unary ROSE expression. */
YicesSolver::YExprTypePair
YicesSolver::ctx_unary(UnaryAPI f, const YExprTypePair &operand, Type rettype) {
    ASSERT_not_null(f);
    yices_expr e = (f)(context, operand.first);
    ASSERT_not_null(e);
    if (NO_TYPE == rettype)
        rettype = operand.second;
    return YExprTypePair(e, rettype);
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/* Create Yices epxression from binary ROSE expression. */
YicesSolver::YExprTypePair
YicesSolver::ctx_binary(BinaryAPI f, const SymbolicExpr::InteriorPtr &in, Type rettype) {
    ASSERT_not_null(f);
    ASSERT_require(in && 2==in->nChildren());

    std::vector<YExprTypePair> operands;
    operands.push_back(ctx_expr(in->child(0)));
    operands.push_back(ctx_expr(in->child(1)));
    operands = ctx_cast(operands, most_type(operands));
    if (NO_TYPE == rettype)
        rettype = operands[0].second;
    yices_expr e = (f)(context, operands[0].first, operands[1].first);
    ASSERT_not_null(e);
    return YExprTypePair(e, rettype);
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/* Create Yices expression for if-then-else operator. */
YicesSolver::YExprTypePair
YicesSolver::ctx_ite(const SymbolicExpr::InteriorPtr &in) {
    ASSERT_require(in && 3==in->nChildren());
    ASSERT_require(in->child(0)->nBits()==1);
    std::vector<YExprTypePair> alternatives;
    alternatives.push_back(ctx_expr(in->child(1)));
    alternatives.push_back(ctx_expr(in->child(2)));
    alternatives = ctx_cast(alternatives, most_type(alternatives));
    yices_expr e = yices_mk_ite(context,
                                ctx_cast(ctx_expr(in->child(0)), BOOLEAN).first,
                                alternatives[0].first,
                                alternatives[1].first);
    return YExprTypePair(e, alternatives[0].second);
}
#endif

#ifdef ROSE_HAVE_LIBYICES
// Create Yices expression for "set" operator.
YicesSolver::YExprTypePair
YicesSolver::ctx_set(const SymbolicExpr::InteriorPtr &in) {
    ASSERT_not_null(in);
    ASSERT_require(in->getOperator() == SymbolicExpr::OP_SET);
    ASSERT_require(in->nChildren() >= 2);
    SymbolicExpr::LeafPtr var = varForSet(in);
    SymbolicExpr::Ptr ite = SymbolicExpr::setToIte(in, var);
    return ctx_expr(ite);
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/* Create Yices expression for left-associative, binary operators. The @p identity is sign-extended and used as the
 * second operand if only one operand is supplied. */
YicesSolver::YExprTypePair
YicesSolver::ctx_la(BinaryAPI f, const SymbolicExpr::InteriorPtr &in, Type rettype) {
    std::vector<YExprTypePair> operands = ctx_exprs(in->children());
    return ctx_la(f, operands, rettype);
}

YicesSolver::YExprTypePair
YicesSolver::ctx_la(BinaryAPI f, const std::vector<YExprTypePair> &operands, Type rettype) {
    ASSERT_not_null(f);
    ASSERT_require(operands.size() >= 2);
    Type childType = most_type(operands);

    yices_expr e = ctx_cast(operands[0], childType).first;
    for (size_t i = 1; i < operands.size(); ++i) {
        yices_expr operand = ctx_cast(operands[i], childType).first;
        e = (f)(context, e, operand);
    }

    if (NO_TYPE == rettype)
        rettype = childType;
    return YExprTypePair(e, rettype);
}

YicesSolver::YExprTypePair
YicesSolver::ctx_la(NaryAPI f, const SymbolicExpr::InteriorPtr &in, Type rettype) {
    std::vector<YExprTypePair> operands = ctx_exprs(in->children());
    return ctx_la(f, operands, rettype);
}

YicesSolver::YExprTypePair
YicesSolver::ctx_la(NaryAPI f, const std::vector<YExprTypePair> &operands, Type rettype) {
    ASSERT_not_null(f);
    ASSERT_require(operands.size() >= 2);
    Type childType = most_type(operands);

    std::vector<yices_expr> exprs;
    BOOST_FOREACH (const YExprTypePair &operand, operands)
        exprs.push_back(ctx_cast(operand, childType).first);
    yices_expr e = (f)(context, &(exprs[0]), exprs.size());
    if (NO_TYPE == rettype)
        rettype = childType;
    return YExprTypePair(e, rettype);
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/* Generate a Yices expression for extract. The yices_mk_bv_extract() second two arguments must be constants. */
YicesSolver::YExprTypePair
YicesSolver::ctx_extract(const SymbolicExpr::InteriorPtr &in) {
    ASSERT_require(in && 3==in->nChildren());
    ASSERT_require(in->child(0)->isIntegerConstant());
    ASSERT_require(in->child(1)->isIntegerConstant());
    ASSERT_require(in->child(0)->toInt() < in->child(1)->toInt());
    size_t lo = in->child(0)->toInt();
    size_t hi = in->child(1)->toInt() - 1; /*inclusive*/
    yices_expr e = yices_mk_bv_extract(context, hi, lo, ctx_cast(ctx_expr(in->child(2)), BIT_VECTOR).first);
    ASSERT_not_null(e);
    return YExprTypePair(e, BIT_VECTOR);
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/* Generate a Yices expression for sign-extend. The third argument for yices_mk_bv_sign_extend() is the number of bits by which
 * the second argument should be extended.  We compute that from the first argument of the OP_SEXTEND operator (the new size)
 * and the size of the second operand (the bit vector to be extended). */
YicesSolver::YExprTypePair
YicesSolver::ctx_sext(const SymbolicExpr::InteriorPtr &in) {
    ASSERT_require(in && 2==in->nChildren());
    ASSERT_require(in->child(0)->isIntegerConstant());
    ASSERT_require(in->child(0)->toInt() > in->child(1)->nBits());
    unsigned extend_by = in->child(0)->toInt() - in->child(1)->nBits();
    yices_expr e = yices_mk_bv_sign_extend(context, ctx_cast(ctx_expr(in->child(1)), BIT_VECTOR).first, extend_by);
    ASSERT_not_null(e);
    return YExprTypePair(e, BIT_VECTOR);
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/* Generate a Yices expression for unsigned-extend.  ROSE's (OP_UEXT NewSize Vector) is rewritten to
 *
 *  (bv-concat (mk-bv [NewSize-OldSize] 0) Vector)
 */
YicesSolver::YExprTypePair
YicesSolver::ctx_uext(const SymbolicExpr::InteriorPtr &in) {
    ASSERT_require(in && 2==in->nChildren());
    ASSERT_require(in->child(0)->isIntegerConstant()); /*Yices mk-bv needs a number for the size operand*/
    ASSERT_require(in->child(0)->toInt() > in->child(1)->nBits());
    size_t extend_by = in->child(0)->toInt() - in->child(1)->nBits();
    yices_expr e = yices_mk_bv_concat(context,
                                      yices_mk_bv_constant(context, extend_by, 0),
                                      ctx_cast(ctx_expr(in->child(1)), BIT_VECTOR).first);
    ASSERT_not_null(e);
    return YExprTypePair(e, BIT_VECTOR);
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/* Generates a Yices expression for shift operators. */
YicesSolver::YExprTypePair
YicesSolver::ctx_shift(ShiftAPI f, const SymbolicExpr::InteriorPtr &in) {
    ASSERT_require(in && 2==in->nChildren());
    ASSERT_require(in->child(0)->isIntegerConstant()); /*Yices' bv-shift-* operators need a constant for the shift amount*/
    unsigned shift_amount = in->child(0)->toInt();
    yices_expr e = (f)(context, ctx_cast(ctx_expr(in->child(1)), BIT_VECTOR).first, shift_amount);
    ASSERT_not_null(e);
    return YExprTypePair(e, BIT_VECTOR);
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/* Generates a Yices expression for arithmetic right shift.  Yices doesn't have a sign-extending right shift, therefore we
 * implement it in terms of other operations.  (OP_ASR ShiftAmount Vector) becomes
 *
 * (ite (= (mk-bv 1 1) (bv-extract [VectorSize-1] [VectorSize-1] Vector)) ;; vector's sign bit
 *      (bv-shift-right1 Vector [ShiftAmount])
 *      (bv-shift-right0 Vector [ShiftAmount]))
 *
 * where [VectorSize], [VectorSize-1], and [ShiftAmount] are numeric constants.
 */
YicesSolver::YExprTypePair
YicesSolver::ctx_asr(const SymbolicExpr::InteriorPtr &in) {
    ASSERT_require(in && 2==in->nChildren());
    SymbolicExpr::Ptr vector = in->child(1);
    unsigned vector_size = vector->nBits();
    ASSERT_require(in->child(0)->isIntegerConstant());
    unsigned shift_amount = in->child(0)->toInt();
    yices_expr e = yices_mk_ite(context, 
                                yices_mk_eq(context, 
                                            yices_mk_bv_constant(context, 1, 1), 
                                            yices_mk_bv_extract(context, vector_size-1, vector_size-1,
                                                                ctx_cast(ctx_expr(vector), BIT_VECTOR).first)),
                                yices_mk_bv_shift_right1(context,
                                                         ctx_cast(ctx_expr(vector), BIT_VECTOR).first, shift_amount),
                                yices_mk_bv_shift_right0(context,
                                                         ctx_cast(ctx_expr(vector), BIT_VECTOR).first, shift_amount));
    ASSERT_not_null(e);
    return YExprTypePair(e, BIT_VECTOR);
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/* Output for zero comparison. Result should be a single bit:
 *      (OP_ZEROP X)
 *
 *  becomes
 *      (= X (mk-bv [sizeof(X)] 0))
 */
YicesSolver::YExprTypePair
YicesSolver::ctx_zerop(const SymbolicExpr::InteriorPtr &in) {
    ASSERT_require(in && 1==in->nChildren());
    yices_expr e = yices_mk_eq(context, 
                               ctx_cast(ctx_expr(in->child(0)), BIT_VECTOR).first,
                               yices_mk_bv_constant(context, in->child(0)->nBits(), 0));
    ASSERT_not_null(e);
    return YExprTypePair(e, BOOLEAN);
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/* Generate a Yices expression for multiply. The OP_SMUL and OP_UMUL nodes of SymbolicExpr define the result width to be
 * the sum of the input widths. Yices' bv-mul operator requires that both operands are the same size and the result is the
 * size of each operand. Therefore, we rewrite (OP_SMUL A B) to become, in Yices:
 *    (bv-mul (bv-sign-extend A |B|]) (bv-sign-extend B |A|))
 */
YicesSolver::YExprTypePair
YicesSolver::ctx_mult(const SymbolicExpr::InteriorPtr &in) {
    ASSERT_require(in->nBits() == in->child(0)->nBits() + in->child(1)->nBits());
    size_t extend0 = in->child(1)->nBits(); // amount by which to extend arg0
    size_t extend1 = in->child(0)->nBits(); // amount by which to extend arg1

    yices_expr e = yices_mk_bv_mul(context, 
                                   yices_mk_bv_sign_extend(context,
                                                           ctx_cast(ctx_expr(in->child(0)), BIT_VECTOR).first,
                                                           extend0), 
                                   yices_mk_bv_sign_extend(context,
                                                           ctx_cast(ctx_expr(in->child(1)), BIT_VECTOR).first,
                                                           extend1));
    ASSERT_not_null(e);
    return YExprTypePair(e, BIT_VECTOR);
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/* Write to memory. */
YicesSolver::YExprTypePair
YicesSolver::ctx_write(const SymbolicExpr::InteriorPtr &in) {
    yices_expr func = ctx_cast(ctx_expr(in->child(0)), MEM_STATE).first;
    yices_expr arg  = ctx_cast(ctx_expr(in->child(1)), BIT_VECTOR).first;
    yices_expr val  = ctx_cast(ctx_expr(in->child(2)), BIT_VECTOR).first;
    ASSERT_require(func && arg && val);
    yices_expr e = yices_mk_function_update(context, func, &arg, 1, val);
    ASSERT_not_null(e);
    return YExprTypePair(e, MEM_STATE);
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/* Read from memory. */
YicesSolver::YExprTypePair
YicesSolver::ctx_read(const SymbolicExpr::InteriorPtr &in) {
    yices_expr func = ctx_cast(ctx_expr(in->child(0)), MEM_STATE).first;
    yices_expr arg  = ctx_cast(ctx_expr(in->child(1)), BIT_VECTOR).first;
    ASSERT_require(func && arg);
    yices_expr e = yices_mk_app(context, func, &arg, 1);
    ASSERT_not_null(e);
    return YExprTypePair(e, BIT_VECTOR);
}
#endif

} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::YicesSolver);
#endif
