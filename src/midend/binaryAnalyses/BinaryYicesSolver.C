#include "sage3basic.h"
#include <rosePublicConfig.h>

#include "rose_strtoull.h"
#include "BinaryYicesSolver.h"

#include <boost/regex.hpp>
#include <boost/thread/locks.hpp>
#include <errno.h>

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

#ifdef ROSE_HAVE_LIBYICES
    ++stats.ncalls;
    {
        boost::lock_guard<boost::mutex> lock(classStatsMutex);
        ++classStats.ncalls;
    }

    if (!context) {
        context = yices_mk_context();
        ASSERT_not_null(context);
    } else {
        yices_reset(context);
    }

#ifndef NDEBUG
    yices_enable_type_checker(true);
#endif

    std::vector<SymbolicExpr::Ptr> exprs = assertions();
    Definitions defns;
    termExprs.clear();
    ctx_define(exprs, &defns);
    ctx_common_subexpressions(exprs);
    for (std::vector<SymbolicExpr::Ptr>::const_iterator ei=exprs.begin(); ei!=exprs.end(); ++ei)
        ctx_assert(*ei);
    switch (yices_check(context)) {
        case l_false: return SAT_NO;
        case l_true:  return SAT_YES;
        case l_undef: return SAT_UNKNOWN;
    }
    ASSERT_not_reachable("switch statement is incomplete");
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
    return SymbolicExpr::makeInteger(found->second.first/*nbits*/, found->second.second/*value*/);
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
                if (leaf->isVariable()) {
                    if (defns->find(leaf->nameId())==defns->end()) {
                        defns->insert(leaf->nameId());
                        o <<"\n";
                        if (!leaf->comment().empty())
                            o <<StringUtility::prefixLines(leaf->comment(), "; ") <<"\n";
                        o <<"(define v" <<leaf->nameId() <<"::" <<get_typename(leaf) <<")\n";
                    }
                } else if (leaf->isMemory()) {
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
                    SymbolicExpr::LeafPtr var = SymbolicExpr::makeVariable(32, "set")->isLeafNode();
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
        if (cses[i]->isLeafNode() && cses[i]->isLeafNode()->isMemory()) {
            out_expr(o, cses[i], MEM_STATE);
        } else {
            out_expr(o, cses[i], BIT_VECTOR);
        }
        o <<")\n";
        termNames_.insert(cses[i], termName);
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
                if ((leaf->isVariable() || leaf->isMemory()) && !leaf->comment().empty()) {
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
    if (tn->isNumber() && 1==tn->nBits()) {
        if (tn->toInt()) {
            o <<"true";
        } else {
            o <<"false";
        }
    } else {
        out_expr(o, tn, BOOLEAN);
    }
    o <<")\n";
}

/* Output a decimal number. */
void
YicesSolver::out_number(std::ostream &o, const SymbolicExpr::Ptr &tn)
{
    SymbolicExpr::LeafPtr ln = tn->isLeafNode();
    ASSERT_require(ln && ln->isNumber());
    o <<ln->toInt();
}

/* Output for one expression. */
void
YicesSolver::out_expr(std::ostream &o, const SymbolicExpr::Ptr &tn, Type needType)
{
    SymbolicExpr::LeafPtr ln = tn->isLeafNode();
    SymbolicExpr::InteriorPtr in = tn->isInteriorNode();
    std::string subExprName;
    if (termNames_.getOptional(tn).assignTo(subExprName)) {
        if (BOOLEAN == needType) {
            o <<"(= " <<subExprName <<" 0b1)";
        } else {
            o <<subExprName;
        }
        
    } else if (ln) {
        if (ln->isNumber()) {
            if (BOOLEAN == needType) {
                ASSERT_require(ln->nBits() == 1);
                o <<(ln->toInt() ? "0b1" : "0b0");
            } else if (ln->nBits() <= 64) {
                ASSERT_require(BIT_VECTOR == needType);
                o <<"(mk-bv " <<ln->nBits() <<" " <<ln->toInt() <<")";
            } else {
                ASSERT_require(BIT_VECTOR == needType);
                o <<"0b" <<ln->bits().toBinary();
            }
        } else if (ln->isVariable()) {
            if (BOOLEAN == needType) {
                ASSERT_require(ln->nBits() == 1);
                o <<"(= " <<ln->toString() <<" 0b1)";
            } else {
                o <<ln->toString();
            }
        } else {
            ASSERT_require(ln->isMemory());
            ASSERT_require(MEM_STATE == needType);
            o <<ln->toString();
        }
    } else {
        ASSERT_not_null(in);
        switch (in->getOperator()) {
            case SymbolicExpr::OP_ADD:
                ASSERT_require(BIT_VECTOR == needType);
                out_la(o, "bv-add", in, false, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_AND:
                ASSERT_require(BOOLEAN == needType);
                out_la(o, "and", in, true, BOOLEAN);
                break;
            case SymbolicExpr::OP_ASR:
                ASSERT_require(BIT_VECTOR == needType);
                out_asr(o, in);
                break;
            case SymbolicExpr::OP_BV_AND:
                ASSERT_require(BIT_VECTOR == needType);
                out_la(o, "bv-and", in, true, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_BV_OR:
                ASSERT_require(BIT_VECTOR == needType);
                out_la(o, "bv-or", in, false, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_BV_XOR:
                ASSERT_require(BIT_VECTOR == needType);
                out_la(o, "bv-xor", in, false, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_EQ:
                ASSERT_require(BOOLEAN == needType);
                out_binary(o, "=", in, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_CONCAT:
                ASSERT_require(BIT_VECTOR == needType);
                out_la(o, "bv-concat", in, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_EXTRACT:
                ASSERT_require(BIT_VECTOR == needType);
                out_extract(o, in);
                break;
            case SymbolicExpr::OP_INVERT:
                out_unary(o, (BOOLEAN==needType?"not":"bv-not"), in, needType);
                break;
            case SymbolicExpr::OP_ITE:
                out_ite(o, in, needType);
                break;
            case SymbolicExpr::OP_LSSB:
                throw Exception("OP_LSSB not implemented");
            case SymbolicExpr::OP_MSSB:
                throw Exception("OP_MSSB not implemented");
            case SymbolicExpr::OP_NE:
                ASSERT_require(BOOLEAN == needType);
                out_binary(o, "/=", in, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_NEGATE:
                ASSERT_require(BIT_VECTOR == needType);
                out_unary(o, "bv-neg", in, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_NOOP:
                out_expr(o, SymbolicExpr::makeInteger(in->nBits(), 0), needType);
                break;
            case SymbolicExpr::OP_OR:
                ASSERT_require(BOOLEAN == needType);
                out_la(o, "or", in, false, BOOLEAN);
                break;
            case SymbolicExpr::OP_READ:
                ASSERT_require(BIT_VECTOR == needType);
                out_read(o, in);
                break;
            case SymbolicExpr::OP_ROL:
                ASSERT_require(BIT_VECTOR == needType);
                throw Exception("OP_ROL not implemented");
            case SymbolicExpr::OP_ROR:
                ASSERT_require(BIT_VECTOR == needType);
                throw Exception("OP_ROR not implemented");
            case SymbolicExpr::OP_SDIV:
                ASSERT_require(BIT_VECTOR == needType);
                throw Exception("OP_SDIV not implemented");
            case SymbolicExpr::OP_SET:
                ASSERT_require(BIT_VECTOR == needType);
                out_set(o, in);
                break;
            case SymbolicExpr::OP_SEXTEND:
                ASSERT_require(BIT_VECTOR == needType);
                out_sext(o, in);
                break;
            case SymbolicExpr::OP_SLT:
                ASSERT_require(BOOLEAN == needType);
                out_binary(o, "bv-slt", in, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_SLE:
                ASSERT_require(BOOLEAN == needType);
                out_binary(o, "bv-sle", in, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_SHL0:
                ASSERT_require(BIT_VECTOR == needType);
                out_shift(o, "bv-shift-left", in, false);
                break;
            case SymbolicExpr::OP_SHL1:
                ASSERT_require(BIT_VECTOR == needType);
                out_shift(o, "bv-shift-left", in, true);
                break;
            case SymbolicExpr::OP_SHR0:
                ASSERT_require(BIT_VECTOR == needType);
                out_shift(o, "bv-shift-right", in, false);
                break;
            case SymbolicExpr::OP_SHR1:
                ASSERT_require(BIT_VECTOR == needType);
                out_shift(o, "bv-shift-right", in, true);
                break;
            case SymbolicExpr::OP_SGE:
                ASSERT_require(BOOLEAN == needType);
                out_binary(o, "bv-sge", in, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_SGT:
                ASSERT_require(BOOLEAN == needType);
                out_binary(o, "bv-sgt", in, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_SMOD:
                ASSERT_require(BIT_VECTOR == needType);
                throw Exception("OP_SMOD not implemented");
            case SymbolicExpr::OP_SMUL:
                ASSERT_require(BIT_VECTOR == needType);
                out_mult(o, in);
                break;
            case SymbolicExpr::OP_UDIV:
                ASSERT_require(BIT_VECTOR == needType);
                throw Exception("OP_UDIV not implemented");
            case SymbolicExpr::OP_UEXTEND:
                ASSERT_require(BIT_VECTOR == needType);
                out_uext(o, in);
                break;
            case SymbolicExpr::OP_UGE:
                ASSERT_require(BOOLEAN == needType);
                out_binary(o, "bv-ge", in, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_UGT:
                ASSERT_require(BOOLEAN == needType);
                out_binary(o, "bv-gt", in, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_ULE:
                ASSERT_require(BOOLEAN == needType);
                out_binary(o, "bv-le", in, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_ULT:
                ASSERT_require(BOOLEAN == needType);
                out_binary(o, "bv-lt", in, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_UMOD:
                ASSERT_require(BIT_VECTOR == needType);
                throw Exception("OP_UMOD not implemented");
            case SymbolicExpr::OP_UMUL:
                ASSERT_require(BIT_VECTOR == needType);
                out_mult(o, in);
                break;
            case SymbolicExpr::OP_WRITE:
                ASSERT_require(MEM_STATE == needType);
                out_write(o, in);
                break;
            case SymbolicExpr::OP_ZEROP:
                ASSERT_require(BOOLEAN == needType);
                out_zerop(o, in);
                break;
        }
    }
}

/* Output for unary operators. */
void
YicesSolver::out_unary(std::ostream &o, const char *opname, const SymbolicExpr::InteriorPtr &in, Type needType)
{
    ASSERT_require(opname && *opname);
    ASSERT_require(in && 1==in->nChildren());

    o <<"(" <<opname <<" ";
    out_expr(o, in->child(0), needType);
    o <<")";
}

/* Output for binary operators. */
void
YicesSolver::out_binary(std::ostream &o, const char *opname, const SymbolicExpr::InteriorPtr &in, Type needType)
{
    ASSERT_require(opname && *opname);
    ASSERT_require(in && 2==in->nChildren());

    o <<"(" <<opname <<" ";
    out_expr(o, in->child(0), needType);
    o <<" ";
    out_expr(o, in->child(1), needType);
    o <<")";
}

/* Output for if-then-else operator.  The condition must be cast from a 1-bit vector to a number, therefore, the input
 *     (OP_ITE COND S1 S2)
 *
 * will be rewritten as
 *     (ite (= COND 0b1) S1 S2)
 */
void
YicesSolver::out_ite(std::ostream &o, const SymbolicExpr::InteriorPtr &in, Type needType)
{
    ASSERT_require(in && 3==in->nChildren());
    ASSERT_require(in->child(0)->nBits()==1);
    o <<"(ite ";
    out_expr(o, in->child(0), BOOLEAN);
    o <<" ";
    out_expr(o, in->child(1), needType);
    o <<" ";
    out_expr(o, in->child(2), needType);
    o <<")";
}

// Output for "set" operator. (set a b c) gets translated to the symbolic expression (ite v1 a (ite v1 b c)), which
// in turn gets translated to Yices output.
void
YicesSolver::out_set(std::ostream &o, const SymbolicExpr::InteriorPtr &in) {
    ASSERT_not_null(in);
    ASSERT_require(in->getOperator() == SymbolicExpr::OP_SET);
    ASSERT_require(in->nChildren() >= 2);
    SymbolicExpr::LeafPtr var = varForSet(in);
    SymbolicExpr::Ptr ite = SymbolicExpr::setToIte(in, var);
    ite->comment(in->comment());
    out_expr(o, ite, BIT_VECTOR);
}

/* Output for left-associative, binary operators. The identity_element is sign-extended and used as the second operand
 * if only one operand is supplied. */
void
YicesSolver::out_la(std::ostream &o, const char *opname, const SymbolicExpr::InteriorPtr &in, bool identity_element,
                    Type needType)
{
    ASSERT_require(opname && *opname);
    ASSERT_require(in && in->nChildren()>=1);

    for (size_t i=1; i<std::max((size_t)2, in->nChildren()); i++)
        o <<"(" <<opname <<" ";
    out_expr(o, in->child(0), needType);

    if (in->nChildren()>1) {
        for (size_t i=1; i<in->nChildren(); i++) {
            o <<" ";
            out_expr(o, in->child(i), needType);
            o <<")";
        }
    } else {
        SymbolicExpr::Ptr ident = SymbolicExpr::makeInteger(in->child(0)->nBits(), identity_element ? (uint64_t)(-1) : 0);
        out_expr(o, ident, needType);
        o <<")";
    }
}

/* Output for left-associative operators. */
void
YicesSolver::out_la(std::ostream &o, const char *opname, const SymbolicExpr::InteriorPtr &in, Type needType)
{
    if (in->nChildren()==1) {
        out_unary(o, opname, in, needType);
    } else {
        out_la(o, opname, in, false, needType);
    }
}

/* Output for extract. Yices bv-extract first two arguments must be constants. */
void
YicesSolver::out_extract(std::ostream &o, const SymbolicExpr::InteriorPtr &in)
{
    ASSERT_require(in && 3==in->nChildren());
    ASSERT_require(in->child(0)->isNumber());
    ASSERT_require(in->child(1)->isNumber());
    ASSERT_require(in->child(0)->toInt() < in->child(1)->toInt());
    size_t lo = in->child(0)->toInt();
    size_t hi = in->child(1)->toInt() - 1;          /*inclusive*/
    o <<"(bv-extract " <<hi <<" " <<lo <<" ";
    out_expr(o, in->child(2), BIT_VECTOR);
    o <<")";
}

/* Output for sign-extend. The second argument for yices' bv-sign-extend function is the number of bits by which the first
 * argument should be extended.  We compute that from the first argument of the OP_SEXTEND operator (the new size) and the
 * size of the second operand (the bit vector to be extended). */
void
YicesSolver::out_sext(std::ostream &o, const SymbolicExpr::InteriorPtr &in)
{
    ASSERT_require(in && 2==in->nChildren());
    ASSERT_require(in->child(0)->isNumber()); /*Yices bv-sign-extend needs a number for the second operand*/
    ASSERT_require(in->child(0)->toInt() > in->child(1)->nBits());
    size_t extend_by = in->child(0)->toInt() - in->child(1)->nBits();
    o <<"(bv-sign-extend  ";
    out_expr(o, in->child(1), BIT_VECTOR); /*vector*/
    o <<" " <<extend_by <<")";
}

/* Output for unsigned-extend.  ROSE's (OP_UEXT NewSize Vector) is rewritten to
 *
 * (bv-concat (mk-bv [NewSize-OldSize] 0) Vector)
 */
void
YicesSolver::out_uext(std::ostream &o, const SymbolicExpr::InteriorPtr &in)
{
    ASSERT_require(in && 2==in->nChildren());
    ASSERT_require(in->child(0)->isNumber()); /*Yices mk-bv needs a number for the size operand*/
    ASSERT_require(in->child(0)->toInt() > in->child(1)->nBits());
    size_t extend_by = in->child(0)->toInt() - in->child(1)->nBits();

    o <<"(bv-concat (mk-bv " <<extend_by <<" 0) ";
    out_expr(o, in->child(1), BIT_VECTOR);
    o <<")";
}

/* Output for shift operators. */
void
YicesSolver::out_shift(std::ostream &o, const char *opname, const SymbolicExpr::InteriorPtr &in,
                       bool newbits)
{
    ASSERT_require(opname && *opname);
    ASSERT_require(in && 2==in->nChildren());
    ASSERT_require(in->child(0)->isNumber()); /*Yices' bv-shift-* operators need a constant for the shift amount*/

    o <<"(" <<opname <<(newbits?"1":"0") <<" ";
    out_expr(o, in->child(1), BIT_VECTOR);
    o <<" " <<in->child(0)->toInt() <<")";
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
void
YicesSolver::out_asr(std::ostream &o, const SymbolicExpr::InteriorPtr &in)
{
    ASSERT_require(in && 2==in->nChildren());
    SymbolicExpr::Ptr vector = in->child(1);
    uint64_t vector_size = vector->nBits();
    ASSERT_require(in->child(0)->isNumber());
    uint64_t shift_amount = in->child(0)->toInt();

    o <<"(ite (= (mk-bv 1 1) (bv-extract " <<(vector_size-1) <<" " <<(vector_size-1) <<" ";
    out_expr(o, vector, BIT_VECTOR);
    o <<")) (bv-shift-right1 ";
    out_expr(o, vector, BIT_VECTOR);
    o <<" " <<shift_amount <<") (bv-shift-right0 ";
    out_expr(o, vector, BIT_VECTOR);
    o <<" " <<shift_amount <<"))";
}

/* Output for zero comparison. Result should be a single bit:
 *      (OP_ZEROP X)
 *
 *  becomes
 *      (ite (= (mk-bv [sizeof(X)] 0) [X]) 0b1 0b0)
 */
void
YicesSolver::out_zerop(std::ostream &o, const SymbolicExpr::InteriorPtr &in)
{
    ASSERT_require(in && 1==in->nChildren());
    o <<"(= (mk-bv " <<in->child(0)->nBits() <<" 0) ";
    out_expr(o, in->child(0), BIT_VECTOR);
    o <<")";
}

/* Output for multiply. The OP_SMUL and OP_UMUL nodes of SymbolicExpr define the result width to be the sum of the input
 * widths. Yices' bv-mul operator requires that both operands are the same size and the result is the size of each operand.
 * Therefore, we rewrite (OP_SMUL A B) to become, in Yices:
 *    (bv-mul (bv-sign-extend A |B|) (bv-sign-extend B |A|))
 */
void
YicesSolver::out_mult(std::ostream &o, const SymbolicExpr::InteriorPtr &in)
{
    ASSERT_require(in->nBits() == in->child(0)->nBits() + in->child(1)->nBits());
    size_t extend0 = in->child(1)->nBits(); // amount by which to extend arg0
    size_t extend1 = in->child(0)->nBits(); // amount by which to extend arg1

    o <<"(bv-mul (bv-sign-extend ";
    out_expr(o, in->child(0), BIT_VECTOR);
    o <<" " <<extend0 <<") (bv-sign-extend ";
    out_expr(o, in->child(1), BIT_VECTOR);
    o <<" " <<extend1 <<"))";
}

/* Output for write. */
void
YicesSolver::out_write(std::ostream &o, const SymbolicExpr::InteriorPtr &in)
{
    o <<"(update ";
    out_expr(o, in->child(0), MEM_STATE);  // previous memory state
    o <<" (";
    out_expr(o, in->child(1), BIT_VECTOR);  // address to which we are writing
    o <<") ";
    out_expr(o, in->child(2), BIT_VECTOR);  // value we are writing
    o <<")";
}

/* Output for read. */
void
YicesSolver::out_read(std::ostream &o, const SymbolicExpr::InteriorPtr &in)
{
    o <<"(";
    out_expr(o, in->child(0), MEM_STATE);  // memory state from which we are reading
    o <<" ";
    out_expr(o, in->child(1), BIT_VECTOR);  // address from which we are reading
    o <<")";
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
    BOOST_FOREACH (const SymbolicExpr::Ptr &cse, cses) {
        yices_expr cseExpr = ctx_expr(cse, BIT_VECTOR);
        termExprs.insert(cse, cseExpr);
    }
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/* Assert a constraint in the logical context. */
void
YicesSolver::ctx_assert(const SymbolicExpr::Ptr &tn)
{
    if (tn->isNumber() && tn->nBits()==1) {
        ASSERT_not_implemented("[Robb Matzke 2015-10-15]"); // see out_assert for similar code
    } else {
        yices_expr e = ctx_expr(tn, BOOLEAN);
        ASSERT_not_null(e);
        yices_assert(context, e);
    }
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/* Builds a Yices expression from a ROSE expression. */
yices_expr
YicesSolver::ctx_expr(const SymbolicExpr::Ptr &tn, Type needType)
{
    yices_expr retval = 0;
    SymbolicExpr::LeafPtr ln = tn->isLeafNode();
    SymbolicExpr::InteriorPtr in = tn->isInteriorNode();
    if (termExprs.getOptional(tn).assignTo(retval)) {
        if (BOOLEAN == needType) {
            TODO("[Robb Matzke 2017-11-09]");
        } else {
            return retval;
        }
    } else if (ln) {
        if (ln->isNumber()) {
            if (BOOLEAN == needType) {
                ASSERT_require(ln->nBits() == 1);
                TODO("[Robb Matzke 2017-11-09]");
            } else if (ln->nBits() <= 64) {
                ASSERT_require(BIT_VECTOR == needType);
                retval = yices_mk_bv_constant(context, ln->nBits(), ln->toInt());
            } else {
                ASSERT_require(BIT_VECTOR == needType);
                int tmp[ln->nBits()];
                for (size_t i=0; i<ln->nBits(); ++i)
                    tmp[i] = ln->bits().get(i) ? 1 : 0;
                retval = yices_mk_bv_constant_from_array(context, ln->nBits(), tmp);
            }
        } else if (ln->isVariable()) {
            if (BOOLEAN == needType) {
                TODO("[Robb Matzke 2017-11-09]");
            } else {
                yices_var_decl vdecl = yices_get_var_decl_from_name(context, ln->toString().c_str());
                ASSERT_not_null(vdecl);
                retval = yices_mk_var_from_decl(context, vdecl);
            }
        } else {
            ASSERT_require(ln->isMemory());
            yices_var_decl vdecl = yices_get_var_decl_from_name(context, ln->toString().c_str());
            ASSERT_not_null(vdecl);
            retval = yices_mk_var_from_decl(context, vdecl);
        }
    } else {
        ASSERT_not_null(in);
        switch (in->getOperator()) {
            case SymbolicExpr::OP_ADD:
                ASSERT_require(BIT_VECTOR == needType);
                retval = ctx_la(yices_mk_bv_add, in, false, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_AND:
                ASSERT_require(BOOLEAN == needType);
                retval = ctx_la(yices_mk_and, in, true, BOOLEAN);
                break;
            case SymbolicExpr::OP_ASR:
                ASSERT_require(BIT_VECTOR == needType);
                retval = ctx_asr(in);
                break;
            case SymbolicExpr::OP_BV_AND:
                ASSERT_require(BIT_VECTOR == needType);
                retval = ctx_la(yices_mk_bv_and, in, true, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_BV_OR:
                ASSERT_require(BIT_VECTOR == needType);
                retval = ctx_la(yices_mk_bv_or, in, false, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_BV_XOR:
                ASSERT_require(BIT_VECTOR == needType);
                retval = ctx_la(yices_mk_bv_xor, in, false, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_EQ:
                ASSERT_require(BOOLEAN == needType);
                retval = ctx_binary(yices_mk_eq, in, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_CONCAT:
                ASSERT_require(BIT_VECTOR == needType);
                retval = ctx_la(yices_mk_bv_concat, in, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_EXTRACT:
                ASSERT_require(BIT_VECTOR == needType);
                retval = ctx_extract(in);
                break;
            case SymbolicExpr::OP_INVERT:
                if (BOOLEAN == needType) {
                    retval = ctx_unary(yices_mk_not, in, BOOLEAN);
                } else {
                    retval = ctx_unary(yices_mk_bv_not, in, BIT_VECTOR);
                }
                break;
            case SymbolicExpr::OP_ITE:
                retval = ctx_ite(in, needType);
                break;
            case SymbolicExpr::OP_LSSB:
                ASSERT_require(BIT_VECTOR == needType);
                throw Exception("OP_LSSB not implemented");
            case SymbolicExpr::OP_MSSB:
                ASSERT_require(BIT_VECTOR == needType);
                throw Exception("OP_MSSB not implemented");
            case SymbolicExpr::OP_NE:
                ASSERT_require(BOOLEAN == needType);
                retval = ctx_binary(yices_mk_diseq, in, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_NEGATE:
                ASSERT_require(BIT_VECTOR == needType);
                retval = ctx_unary(yices_mk_bv_minus, in, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_NOOP:
                throw Exception("OP_NOOP not implemented");
            case SymbolicExpr::OP_OR:
                ASSERT_require(BOOLEAN == needType);
                retval = ctx_la(yices_mk_or, in, false, BOOLEAN);
                break;
            case SymbolicExpr::OP_READ:
                ASSERT_require(BIT_VECTOR == needType);
                retval = ctx_read(in);
                break;
            case SymbolicExpr::OP_ROL:
                ASSERT_require(BIT_VECTOR == needType);
                throw Exception("OP_ROL not implemented");
            case SymbolicExpr::OP_ROR:
                ASSERT_require(BIT_VECTOR == needType);
                throw Exception("OP_ROR not implemented");
            case SymbolicExpr::OP_SET:
                ASSERT_require(BIT_VECTOR == needType);
                retval = ctx_set(in);
                break;
            case SymbolicExpr::OP_SDIV:
                ASSERT_require(BIT_VECTOR == needType);
                throw Exception("OP_SDIV not implemented");
            case SymbolicExpr::OP_SEXTEND:
                ASSERT_require(BIT_VECTOR == needType);
                retval = ctx_sext(in);
                break;
            case SymbolicExpr::OP_SLT:
                ASSERT_require(BOOLEAN == needType);
                retval = ctx_binary(yices_mk_bv_slt, in, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_SLE:
                ASSERT_require(BOOLEAN == needType);
                retval = ctx_binary(yices_mk_bv_sle, in, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_SHL0:
                ASSERT_require(BIT_VECTOR == needType);
                retval = ctx_shift(yices_mk_bv_shift_left0, in);
                break;
            case SymbolicExpr::OP_SHL1:
                ASSERT_require(BIT_VECTOR == needType);
                retval = ctx_shift(yices_mk_bv_shift_left1, in);
                break;
            case SymbolicExpr::OP_SHR0:
                ASSERT_require(BIT_VECTOR == needType);
                retval = ctx_shift(yices_mk_bv_shift_right0, in);
                break;
            case SymbolicExpr::OP_SHR1:
                ASSERT_require(BIT_VECTOR == needType);
                retval = ctx_shift(yices_mk_bv_shift_right1, in);
                break;
            case SymbolicExpr::OP_SGE:
                ASSERT_require(BOOLEAN == needType);
                retval = ctx_binary(yices_mk_bv_sge, in, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_SGT:
                ASSERT_require(BOOLEAN == needType);
                retval = ctx_binary(yices_mk_bv_sgt, in, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_SMOD:
                ASSERT_require(BIT_VECTOR == needType);
                throw Exception("OP_SMOD not implemented");
            case SymbolicExpr::OP_SMUL:
                ASSERT_require(BIT_VECTOR == needType);
                retval = ctx_mult(in);
                break;
            case SymbolicExpr::OP_UDIV:
                ASSERT_require(BIT_VECTOR == needType);
                throw Exception("OP_UDIV not implemented");
            case SymbolicExpr::OP_UEXTEND:
                ASSERT_require(BIT_VECTOR == needType);
                retval = ctx_uext(in);
                break;
            case SymbolicExpr::OP_UGE:
                ASSERT_require(BOOLEAN == needType);
                retval = ctx_binary(yices_mk_bv_ge, in, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_UGT:
                ASSERT_require(BOOLEAN == needType);
                retval = ctx_binary(yices_mk_bv_gt, in, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_ULE:
                ASSERT_require(BOOLEAN == needType);
                retval = ctx_binary(yices_mk_bv_le, in, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_ULT:
                ASSERT_require(BOOLEAN == needType);
                retval = ctx_binary(yices_mk_bv_lt, in, BIT_VECTOR);
                break;
            case SymbolicExpr::OP_UMOD:
                ASSERT_require(BIT_VECTOR == needType);
                throw Exception("OP_UMOD not implemented");
            case SymbolicExpr::OP_UMUL:
                ASSERT_require(BIT_VECTOR == needType);
                retval = ctx_mult(in);
                break;
            case SymbolicExpr::OP_WRITE:
                ASSERT_require(MEM_STATE == needType);
                retval = ctx_write(in);
                break;
            case SymbolicExpr::OP_ZEROP:
                ASSERT_require(BOOLEAN == needType);
                retval = ctx_zerop(in);
                break;
        }
    }
    ASSERT_not_null(retval);
    return retval;
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/* Create Yices expression from unary ROSE expression. */
yices_expr
YicesSolver::ctx_unary(UnaryAPI f, const SymbolicExpr::InteriorPtr &in, Type needType) {
    ASSERT_not_null(f);
    ASSERT_require(in && 1==in->nChildren());
    yices_expr retval = (f)(context, ctx_expr(in->child(0), needType));
    ASSERT_not_null(retval);
    return retval;
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/* Create Yices epxression from binary ROSE expression. */
yices_expr
YicesSolver::ctx_binary(BinaryAPI f, const SymbolicExpr::InteriorPtr &in, Type needType) {
    ASSERT_not_null(f);
    ASSERT_require(in && 2==in->nChildren());
    yices_expr retval = (f)(context, ctx_expr(in->child(0), needType), ctx_expr(in->child(1), needType));
    ASSERT_not_null(retval);
    return retval;
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/* Create Yices expression for if-then-else operator. */
yices_expr
YicesSolver::ctx_ite(const SymbolicExpr::InteriorPtr &in, Type needType) {
    ASSERT_require(in && 3==in->nChildren());
    ASSERT_require(in->child(0)->nBits()==1);
    yices_expr cond = ctx_expr(in->child(0), BOOLEAN);
    yices_expr retval = yices_mk_ite(context, cond, ctx_expr(in->child(1), needType), ctx_expr(in->child(2), needType));
    ASSERT_not_null(retval);
    return retval;
}
#endif

#ifdef ROSE_HAVE_LIBYICES
// Create Yices expression for "set" operator.
yices_expr
YicesSolver::ctx_set(const SymbolicExpr::InteriorPtr &in) {
    ASSERT_not_null(in);
    ASSERT_require(in->getOperator() == SymbolicExpr::OP_SET);
    ASSERT_require(in->nChildren() >= 2);
    SymbolicExpr::LeafPtr var = varForSet(in);
    ASSERT_not_null(var);
    SymbolicExpr::Ptr ite = SymbolicExpr::setToIte(in, var);
    return ctx_expr(ite, BIT_VECTOR);
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/* Create Yices expression for left-associative, binary operators. The @p identity is sign-extended and used as the
 * second operand if only one operand is supplied. */
yices_expr
YicesSolver::ctx_la(BinaryAPI f, const SymbolicExpr::InteriorPtr &in, bool identity, Type needType)
{
    ASSERT_not_null(f);
    ASSERT_require(in && in->nChildren()>=1);

    yices_expr retval = ctx_expr(in->child(0), needType);

    for (size_t i=1; i<in->nChildren(); i++) {
        retval = (f)(context, retval, ctx_expr(in->child(i), needType));
        ASSERT_not_null(retval);
    }
    
    if (1==in->nChildren()) {
        yices_expr ident = yices_mk_bv_constant(context, 1, identity ? (uint64_t)(-1) : 0);
        retval = (f)(context, retval, ident);
        ASSERT_not_null(retval);
    }
    
    return retval;
}
#endif

#ifdef ROSE_HAVE_LIBYICES
yices_expr
YicesSolver::ctx_la(NaryAPI f, const SymbolicExpr::InteriorPtr &in, bool identity, Type needType)
{
    ASSERT_not_null(f);
    ASSERT_require(in && in->nChildren()>=1);
    yices_expr *operands = new yices_expr[in->nChildren()];
    for (size_t i=0; i<in->nChildren(); i++) {
        operands[i] = ctx_expr(in->child(i), needType);
        ASSERT_not_null(operands[i]);
    }
    yices_expr retval = (f)(context, operands, in->nChildren());
    ASSERT_not_null(retval);
    delete[] operands;
    return retval;
}
#endif

#ifdef ROSE_HAVE_LIBYICES
yices_expr
YicesSolver::ctx_la(BinaryAPI f, const SymbolicExpr::InteriorPtr &in, Type needType)
{
    ASSERT_require(in->nChildren()>1);
    return ctx_la(f, in, false, needType);
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/* Generate a Yices expression for extract. The yices_mk_bv_extract() second two arguments must be constants. */
yices_expr
YicesSolver::ctx_extract(const SymbolicExpr::InteriorPtr &in)
{
    ASSERT_require(in && 3==in->nChildren());
    ASSERT_require(in->child(0)->isNumber());
    ASSERT_require(in->child(1)->isNumber());
    ASSERT_require(in->child(0)->toInt() < in->child(1)->toInt());
    size_t lo = in->child(0)->toInt();
    size_t hi = in->child(1)->toInt() - 1; /*inclusive*/
    yices_expr retval = yices_mk_bv_extract(context, hi, lo, ctx_expr(in->child(2), BIT_VECTOR));
    ASSERT_not_null(retval);
    return retval;
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/* Generate a Yices expression for sign-extend. The third argument for yices_mk_bv_sign_extend() is the number of bits by which
 * the second argument should be extended.  We compute that from the first argument of the OP_SEXTEND operator (the new size)
 * and the size of the second operand (the bit vector to be extended). */
yices_expr
YicesSolver::ctx_sext(const SymbolicExpr::InteriorPtr &in)
{
    ASSERT_require(in && 2==in->nChildren());
    ASSERT_require(in->child(0)->isNumber());
    ASSERT_require(in->child(0)->toInt() > in->child(1)->nBits());
    unsigned extend_by = in->child(0)->toInt() - in->child(1)->nBits();
    yices_expr retval = yices_mk_bv_sign_extend(context, ctx_expr(in->child(1), BIT_VECTOR), extend_by);
    ASSERT_not_null(retval);
    return retval;
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/* Generate a Yices expression for unsigned-extend.  ROSE's (OP_UEXT NewSize Vector) is rewritten to
 *
 *  (bv-concat (mk-bv [NewSize-OldSize] 0) Vector)
 */
yices_expr
YicesSolver::ctx_uext(const SymbolicExpr::InteriorPtr &in)
{
    ASSERT_require(in && 2==in->nChildren());
    ASSERT_require(in->child(0)->isNumber()); /*Yices mk-bv needs a number for the size operand*/
    ASSERT_require(in->child(0)->toInt() > in->child(1)->nBits());
    size_t extend_by = in->child(0)->toInt() - in->child(1)->nBits();
    yices_expr retval = yices_mk_bv_concat(context,
                                           yices_mk_bv_constant(context, extend_by, 0),
                                           ctx_expr(in->child(1), BIT_VECTOR));
    ASSERT_not_null(retval);
    return retval;
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/* Generates a Yices expression for shift operators. */
yices_expr
YicesSolver::ctx_shift(ShiftAPI f, const SymbolicExpr::InteriorPtr &in)
{
    ASSERT_require(in && 2==in->nChildren());
    ASSERT_require(in->child(0)->isNumber()); /*Yices' bv-shift-* operators need a constant for the shift amount*/
    unsigned shift_amount = in->child(0)->toInt();
    yices_expr retval = (f)(context, ctx_expr(in->child(1), BIT_VECTOR), shift_amount);
    ASSERT_not_null(retval);
    return retval;
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
yices_expr
YicesSolver::ctx_asr(const SymbolicExpr::InteriorPtr &in)
{
    ASSERT_require(in && 2==in->nChildren());
    SymbolicExpr::Ptr vector = in->child(1);
    unsigned vector_size = vector->nBits();
    ASSERT_require(in->child(0)->isNumber());
    unsigned shift_amount = in->child(0)->toInt();
    yices_expr retval = yices_mk_ite(context, 
                                     yices_mk_eq(context, 
                                                 yices_mk_bv_constant(context, 1, 1), 
                                                 yices_mk_bv_extract(context, vector_size-1, vector_size-1,
                                                                     ctx_expr(vector, BIT_VECTOR))),
                                     yices_mk_bv_shift_right1(context, ctx_expr(vector, BIT_VECTOR), shift_amount), 
                                     yices_mk_bv_shift_right0(context, ctx_expr(vector, BIT_VECTOR), shift_amount));
    ASSERT_not_null(retval);
    return retval;
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/* Output for zero comparison. Result should be a single bit:
 *      (OP_ZEROP X)
 *
 *  becomes
 *      (ite (= (mk-bv [sizeof(X)] 0) [X]) 0b1 0b0)
 */
yices_expr
YicesSolver::ctx_zerop(const SymbolicExpr::InteriorPtr &in)
{
    ASSERT_require(in && 1==in->nChildren());
    yices_expr retval = yices_mk_eq(context, yices_mk_bv_constant(context, in->child(0)->nBits(), 0), 
                                    ctx_expr(in->child(0), BIT_VECTOR)); 
    ASSERT_not_null(retval);
    return retval;
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/* Generate a Yices expression for multiply. The OP_SMUL and OP_UMUL nodes of SymbolicExpr define the result width to be
 * the sum of the input widths. Yices' bv-mul operator requires that both operands are the same size and the result is the
 * size of each operand. Therefore, we rewrite (OP_SMUL A B) to become, in Yices:
 *    (bv-mul (bv-sign-extend A |B|]) (bv-sign-extend B |A|))
 */
yices_expr
YicesSolver::ctx_mult(const SymbolicExpr::InteriorPtr &in)
{
    ASSERT_require(in->nBits() == in->child(0)->nBits() + in->child(1)->nBits());
    size_t extend0 = in->child(1)->nBits(); // amount by which to extend arg0
    size_t extend1 = in->child(0)->nBits(); // amount by which to extend arg1

    yices_expr retval = yices_mk_bv_mul(context, 
                                        yices_mk_bv_sign_extend(context, ctx_expr(in->child(0), BIT_VECTOR), extend0), 
                                        yices_mk_bv_sign_extend(context, ctx_expr(in->child(1), BIT_VECTOR), extend1));
    ASSERT_not_null(retval);
    return retval;
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/* Write to memory. */
yices_expr
YicesSolver::ctx_write(const SymbolicExpr::InteriorPtr &in)
{
    yices_expr func = ctx_expr(in->child(0), MEM_STATE);
    yices_expr arg  = ctx_expr(in->child(1), BIT_VECTOR);
    yices_expr val  = ctx_expr(in->child(2), BIT_VECTOR);
    ASSERT_require(func && arg && val);
    yices_expr retval = yices_mk_function_update(context, func, &arg, 1, val);
    ASSERT_not_null(retval);
    return retval;
}
#endif

#ifdef ROSE_HAVE_LIBYICES
/* Read from memory. */
yices_expr
YicesSolver::ctx_read(const SymbolicExpr::InteriorPtr &in)
{
    yices_expr func = ctx_expr(in->child(0), MEM_STATE);
    yices_expr arg  = ctx_expr(in->child(1), BIT_VECTOR);
    ASSERT_require(func && arg);
    yices_expr retval = yices_mk_app(context, func, &arg, 1);
    ASSERT_not_null(retval);
    return retval;
}
#endif

} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::YicesSolver);
#endif
