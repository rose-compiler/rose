#define __STDC_LIMIT_MACROS

#include "sage3basic.h"

#include "InsnSemanticsExpr.h"
#include "SMTSolver.h"
#include "stringify.h"
#include "integerOps.h"
#include "Combinatorics.h"

#include <boost/foreach.hpp>

namespace rose {
namespace BinaryAnalysis {
namespace InsnSemanticsExpr {

uint64_t
LeafNode::name_counter = 0;

const uint64_t
MAX_NNODES = UINT64_MAX;

const char *
to_str(Operator o)
{
    static char buf[64];
    std::string s = stringifyBinaryAnalysisInsnSemanticsExprOperator(o, "OP_");
    ASSERT_require(s.size()<sizeof buf);
    strcpy(buf, s.c_str());
    for (char *s=buf; *s; s++) {
        if ('_'==*s) {
            *s = '-';
        } else {
            *s = tolower(*s);
        }
    }
    return buf;
}

/*******************************************************************************************************************************
 *                                      TreeNode methods
 *******************************************************************************************************************************/

std::set<LeafNodePtr>
TreeNode::get_variables() const
{
    struct T1: public Visitor {
        std::set<LeafNodePtr> vars;
        VisitAction preVisit(const TreeNodePtr&) {
            return CONTINUE;
        }
        VisitAction postVisit(const TreeNodePtr &node) {
            LeafNodePtr l_node = node->isLeafNode();
            if (l_node && !l_node->is_known())
                vars.insert(l_node);
            return CONTINUE;
        }
    } t1;
    depth_first_traversal(t1);
    return t1.vars;
}

uint64_t
TreeNode::hash() const
{
    if (0==hashval) {
        // FIXME: We could build the hash with a traversal rather than
        // from a string.  But this method is quick and easy. [Robb P. Matzke 2013-09-10]
        std::ostringstream ss;
        Formatter formatter;
        formatter.show_comments = Formatter::CMT_SILENT;
        print(ss, formatter);
        hashval = Combinatorics::fnv1a64_digest(ss.str());
    }
    return hashval;
}

void
TreeNode::assert_acyclic() const
{
#ifndef NDEBUG
    struct T1: Visitor {
        std::vector<const TreeNode*> ancestors;
        VisitAction preVisit(const TreeNodePtr &node) {
            ASSERT_require(std::find(ancestors.begin(), ancestors.end(), getRawPointer(node))==ancestors.end());
            ancestors.push_back(getRawPointer(node));
            return CONTINUE;
        }
        VisitAction postVisit(const TreeNodePtr &node) {
            ASSERT_require(!ancestors.empty() && ancestors.back()==getRawPointer(node));
            ancestors.pop_back();
            return CONTINUE;
        }
    } t1;
    depth_first_traversal(t1);
#endif
}

uint64_t
TreeNode::nnodesUnique() const {
    std::vector<TreeNodePtr> exprs(1, sharedFromThis());
    return InsnSemanticsExpr::nnodesUnique(exprs.begin(), exprs.end());
}

std::vector<TreeNodePtr>
TreeNode::findCommonSubexpressions() const {
    return InsnSemanticsExpr::findCommonSubexpressions(std::vector<TreeNodePtr>(1, sharedFromThis()));
}

void
TreeNode::printFlags(std::ostream &o, unsigned flags, char &bracket) const {
    if ((flags & INDETERMINATE) != 0) {
        o <<bracket <<"indet";
        bracket = ',';
        flags &= ~INDETERMINATE;
    }
    if ((flags & UNSPECIFIED) != 0) {
        o <<bracket <<"unspec";
        bracket = ',';
        flags &= ~UNSPECIFIED;
    }
    if (flags != 0) {
        o <<bracket <<"f=" <<std::hex <<flags <<std::dec;
        bracket = ',';
    }
}

/*******************************************************************************************************************************
 *                                      InternalNode methods
 *******************************************************************************************************************************/

void
InternalNode::add_child(const TreeNodePtr &child)
{
    ASSERT_not_null(child);
    children.push_back(child);
    if (nnodes_ != MAX_NNODES) {
        if (nnodes_ + child->nnodes() < nnodes_) {
            nnodes_ = MAX_NNODES;                       // overflow
        } else {
            nnodes_ += child->nnodes();
        }
    }
}

void
InternalNode::adjustWidth() {
    ASSERT_require(!children.empty());
    switch (op) {
        case OP_ASR:
        case OP_ROL:
        case OP_ROR:
        case OP_SHL0:
        case OP_SHL1:
        case OP_SHR0:
        case OP_SHR1: {
            ASSERT_require(nchildren() == 2);
            ASSERT_require(child(0)->isScalar());       // shift amount
            ASSERT_require(child(1)->isScalar());       // value to shift
            nbits = child(1)->get_nbits();
            domainWidth_ = 0;
            break;
        }
        case OP_CONCAT: {
            size_t totalWidth = 0;
            BOOST_FOREACH (const TreeNodePtr &child, children) {
                ASSERT_require(child->isScalar());
                totalWidth += child->get_nbits();
            }
            nbits = totalWidth;
            domainWidth_ = 0;
            break;
        }
        case OP_EQ:
        case OP_NE:
        case OP_SGE:
        case OP_SGT:
        case OP_SLE:
        case OP_SLT:
        case OP_UGE:
        case OP_UGT:
        case OP_ULE:
        case OP_ULT: {
            ASSERT_require(nchildren() == 2);
            ASSERT_require(child(0)->get_nbits() == child(1)->get_nbits());
            nbits = 1;
            domainWidth_ = 0;
            break;
        }
        case OP_EXTRACT: {
            ASSERT_require(nchildren() == 3);
            ASSERT_require(child(0)->is_known());
            ASSERT_require(child(1)->is_known());
            ASSERT_require(child(2)->isScalar());
            ASSERT_require(child(0)->get_value() < child(1)->get_value());
            size_t totalSize = child(1)->get_value() - child(0)->get_value();
            nbits = totalSize;
            domainWidth_ = 0;
            break;
        }
        case OP_ITE: {
            ASSERT_require(nchildren() == 3);
            ASSERT_require(child(0)->isScalar());
            ASSERT_require(child(0)->get_nbits() == 1);
            ASSERT_require(child(1)->get_nbits() == child(2)->get_nbits());
            ASSERT_require(child(1)->domainWidth() == child(2)->domainWidth());
            nbits = child(1)->get_nbits();
            domainWidth_ = child(1)->domainWidth();
            break;
        }
        case OP_LSSB:
        case OP_MSSB:
        case OP_NEGATE: {
            ASSERT_require(nchildren() == 1);
            ASSERT_require(child(0)->isScalar());
            nbits = child(0)->get_nbits();
            domainWidth_ = 0;
            break;
        }
        case OP_READ: {
            ASSERT_require(nchildren() == 2);
            ASSERT_require2(!child(0)->isScalar(), "memory state expected for first operand");
            ASSERT_require(child(1)->isScalar());
            ASSERT_require2(child(0)->domainWidth() == child(1)->get_nbits(), "invalid address size");
            nbits = child(0)->get_nbits();              // size of values stored in memory
            domainWidth_ = 0;
            break;
        }
        case OP_SDIV:
        case OP_UDIV: {
            ASSERT_require(nchildren() == 2);
            ASSERT_require(child(0)->isScalar());
            ASSERT_require(child(1)->isScalar());
            nbits = child(0)->get_nbits();
            domainWidth_ = 0;
            break;
        }
        case OP_SEXTEND:
        case OP_UEXTEND: {
            ASSERT_require(nchildren() == 2);
            ASSERT_require(child(0)->is_known());       // new size
            ASSERT_require(child(1)->isScalar());       // value to extend
            nbits = child(0)->get_value();
            domainWidth_ = 0;
            break;
        }
        case OP_SMOD:
        case OP_UMOD: {
            ASSERT_require(nchildren() == 2);
            ASSERT_require(child(0)->isScalar());
            ASSERT_require(child(1)->isScalar());
            nbits = child(1)->get_nbits();
            domainWidth_ = 0;
            break;
        }
        case OP_SMUL:
        case OP_UMUL: {
            ASSERT_require(nchildren() == 2);
            ASSERT_require(child(0)->isScalar());
            ASSERT_require(child(1)->isScalar());
            nbits = child(0)->get_nbits() + child(1)->get_nbits();
            domainWidth_ = 0;
            break;
        }
        case OP_WRITE: {
            ASSERT_require(nchildren() == 3);
            ASSERT_require2(!child(0)->isScalar(), "first operand must be memory");
            ASSERT_require(child(1)->isScalar());       // address
            ASSERT_require(child(2)->isScalar());       // value
            ASSERT_require2(child(1)->get_nbits() == child(0)->domainWidth(), "incorrect address width");
            ASSERT_require2(child(2)->get_nbits() == child(0)->get_nbits(), "incorrect value width");
            nbits = child(0)->get_nbits();
            domainWidth_ = child(0)->domainWidth();
            break;
        }
        case OP_ZEROP: {
            ASSERT_require(nchildren() == 1);
            ASSERT_require(child(0)->isScalar());
            nbits = 1;
            domainWidth_ = 0;
            break;
        }
        default: {
            // All children must have the same width, which is the width of this expression. This is suitable for things like
            // bitwise operators, add, etc.
            ASSERT_require(child(0)->isScalar());
            for (size_t i=1; i<nchildren(); ++i) {
                ASSERT_require(child(i)->isScalar());
                ASSERT_require(child(i)->get_nbits() == child(0)->get_nbits());
            }
            nbits = child(0)->get_nbits();
            domainWidth_ = 0;
            break;
        }
    }
    ASSERT_require(nbits != 0);
}

void
InternalNode::adjustBitFlags() {
    flags_ = 0;
    BOOST_FOREACH (const TreeNodePtr &child, children)
        flags_ |= child->get_flags();
}

void
InternalNode::print(std::ostream &o, Formatter &fmt) const
{
    struct FormatGuard {
        Formatter &fmt;
        FormatGuard(Formatter &fmt): fmt(fmt) {
            ++fmt.cur_depth;
        }
        ~FormatGuard() {
            --fmt.cur_depth;
        }
    } formatGuard(fmt);

    o <<"(" <<to_str(op);

    char bracket = '[';
    if (fmt.show_width) {
        o <<bracket <<nbits;
        bracket = ',';
    }
    if (fmt.show_flags)
        printFlags(o, get_flags(), bracket /*in,out*/);
    if (fmt.show_comments!=Formatter::CMT_SILENT && !comment.empty()) {
        o <<bracket <<comment;
        bracket = ',';
    }
    if (bracket != '[')
        o <<"]";

    if (fmt.max_depth!=0 && fmt.cur_depth>=fmt.max_depth && 0!=nchildren()) {
        o <<" ...";
    } else {
        for (size_t i=0; i<children.size(); i++) {
            bool printed = false;
            LeafNodePtr child_leaf = children[i]->isLeafNode();
            o <<" ";
            switch (op) {
                case OP_ASR:
                case OP_ROL:
                case OP_ROR:
                case OP_UEXTEND:
                    if (0==i && child_leaf) {
                        child_leaf->print_as_unsigned(o, fmt);
                        printed = true;
                    }
                    break;

                case OP_EXTRACT:
                    if ((0==i || 1==i) && child_leaf) {
                        child_leaf->print_as_unsigned(o, fmt);
                        printed = true;
                    }
                    break;

                case OP_BV_AND:
                case OP_BV_OR:
                case OP_BV_XOR:
                case OP_CONCAT:
                case OP_UDIV:
                case OP_UGE:
                case OP_UGT:
                case OP_ULE:
                case OP_ULT:
                case OP_UMOD:
                case OP_UMUL:
                    if (child_leaf) {
                        child_leaf->print_as_unsigned(o, fmt);
                        printed = true;
                    }
                    break;

                default:
                    break;
            }

            if (!printed)
                children[i]->print(o, fmt);
        }
    }
    o <<")";
}

bool
InternalNode::must_equal(const TreeNodePtr &other_, SMTSolver *solver/*NULL*/) const
{
    bool retval = false;
    if (this==getRawPointer(other_)) {
        retval = true;
    } else if (equivalent_to(other_)) {
        // This is probably faster than using an SMT solver. It also serves as the naive approach when an SMT solver
        // is not available.
        retval = true;
    } else if (solver) {
        TreeNodePtr assertion = InternalNode::create(1, OP_NE, sharedFromThis(), other_);
        retval = SMTSolver::SAT_NO==solver->satisfiable(assertion); /*equal if there is no solution for inequality*/
    }
    return retval;
}

bool
InternalNode::may_equal(const TreeNodePtr &other, SMTSolver *solver/*NULL*/) const
{
    bool retval = false;
    if (this==getRawPointer(other)) {
        return true;
    } else if (equivalent_to(other)) {
        // This is probably faster than using an SMT solver.  It also serves as the naive approach when an SMT solver
        // is not available.
        retval = true;
    } else if (solver) {
        TreeNodePtr assertion = InternalNode::create(1, OP_EQ, sharedFromThis(), other);
        retval = SMTSolver::SAT_YES==solver->satisfiable(assertion);
    }
    return retval;
}

int
InternalNode::structural_compare(const TreeNodePtr &other_) const
{
    InternalNodePtr other = other_->isInternalNode();
    if (this==getRawPointer(other)) {
        return 0;
    } else if (other==NULL) {
        return 1;                                       // leaf nodes < internal nodes
    } else if (op != other->op) {
        return op < other->op ? -1 : 1;
    } else if (get_nbits() != other->get_nbits()) {
        return get_nbits() < other->get_nbits() ? -1 : 1;
    } else if (children.size() != other->children.size()) {
        return children.size() < other->children.size() ? -1 : 1;
    } else if (get_flags() != other->get_flags()) {
        return get_flags() < other->get_flags() ? -1 : 1;
    } else {
        // compare children
        ASSERT_require(children.size()==other->children.size());
        for (size_t i=0; i<children.size(); ++i) {
            if (int cmp = children[i]->structural_compare(other->children[i]))
                return cmp;
        }
    }
    return 0;
}

bool
InternalNode::equivalent_to(const TreeNodePtr &other_) const
{
    bool retval = false;
    InternalNodePtr other = other_->isInternalNode();
    if (this==getRawPointer(other)) {
        retval = true;
    } else if (other==NULL || get_nbits()!=other->get_nbits() || get_flags()!=other->get_flags()) {
        retval = false;
    } else if (hashval!=0 && other->hashval!=0 && hashval!=other->hashval) {
        // Unequal hashvals imply non-equivalent expressions.  The converse is not necessarily true due to possible
        // collisions.
        retval = false;
    } else if (op==other->op && children.size()==other->children.size()) {
        retval = true;
        for (size_t i=0; i<children.size() && retval; ++i)
            retval = children[i]->equivalent_to(other->children[i]);
        // Cache hash values. There's no need to compute a hash value if we've determined that the two expressions are
        // equivalent because it wouldn't save us any work--two equal hash values doesn't necessarily mean that two expressions
        // are equivalent.  However, if we already know one of the hash values then we can cache that hash value in the other
        // expression too.
        if (retval) {
            if (hashval!=0 && other->hashval==0) {
                other->hashval = hashval;
            } else if (hashval==0 && other->hashval!=0) {
                hashval = other->hashval;
            } else {
                ASSERT_require(hashval==other->hashval);
            }
        } else {
#ifdef InsnInstructionExpr_USE_HASHES
            hashval = hash();
            other->hashval = other->hash();
#endif
        }
    }
    return retval;
}

TreeNodePtr
InternalNode::substitute(const TreeNodePtr &from, const TreeNodePtr &to) const
{
    ASSERT_require(from!=NULL && to!=NULL && from->get_nbits()==to->get_nbits());
    if (equivalent_to(from))
        return to;
    bool substituted = false;
    TreeNodes newnodes;
    for (size_t i=0; i<children.size(); ++i) {
        if (children[i]->equivalent_to(from)) {
            newnodes.push_back(to);
            substituted = true;
        } else {
            newnodes.push_back(children[i]->substitute(from, to));
            if (newnodes.back()!=children[i])
                substituted = true;
        }
    }
    if (!substituted)
        return sharedFromThis();
    return InternalNode::create(get_nbits(), get_operator(), newnodes, get_comment());
}

VisitAction
InternalNode::depth_first_traversal(Visitor &v) const
{
    TreeNodePtr self = sharedFromThis();
    VisitAction action = v.preVisit(self);
    if (CONTINUE==action) {
        for (std::vector<TreeNodePtr>::const_iterator ci=children.begin(); ci!=children.end(); ++ci) {
            action = (*ci)->depth_first_traversal(v);
            if (TERMINATE==action)
                break;
        }
    }
    if (TERMINATE!=action)
        action = v.postVisit(self);
    return action;
}

InternalNodePtr
InternalNode::nonassociative() const
{
    TreeNodes newOperands;
    std::list<TreeNodePtr> worklist(children.begin(), children.end());
    bool modified = false;
    while (!worklist.empty()) {
        TreeNodePtr child = worklist.front();
        worklist.pop_front();
        InternalNodePtr ichild = child->isInternalNode();
        if (ichild && ichild->op == op) {
            worklist.insert(worklist.begin(), ichild->children.begin(), ichild->children.end());
            modified = true;
        } else {
            newOperands.push_back(child);
        }
    }
    if (!modified)
        return isInternalNode();

    // Return the new expression without simplifying it again.
    return InternalNodePtr(new InternalNode(get_nbits(), op, newOperands, get_comment()));
}

// compare expressions for sorting operands of commutative operators. Returns -1, 0, 1
static int
expr_cmp(const TreeNodePtr &a, const TreeNodePtr &b)
{
    ASSERT_not_null(a);
    ASSERT_not_null(b);
    InternalNodePtr ai = a->isInternalNode();
    InternalNodePtr bi = b->isInternalNode();
    LeafNodePtr al = a->isLeafNode();
    LeafNodePtr bl = b->isLeafNode();
    ASSERT_require((ai!=NULL) ^ (al!=NULL));
    ASSERT_require((bi!=NULL) ^ (bl!=NULL));

    if (a == b) {
        return 0;
    } else if ((ai==NULL) != (bi==NULL)) {
        // internal nodes are less than leaf nodes
        return ai!=NULL ? -1 : 1;
    } else if (al!=NULL) {
        // both are leaf nodes
        ASSERT_not_null(bl);
        if (al->is_known() != bl->is_known()) {
            // constants are greater than variables
            return al->is_known() ? 1 : -1;
        } else if (al->is_known()) {
            // both are constants, sort by unsigned value
            ASSERT_require(bl->is_known());
            return al->get_bits().compare(bl->get_bits());
        } else if (al->is_variable() != bl->is_variable()) {
            // variables are less than memory
            return al->is_variable() ? -1 : 1;
        } else {
            // both are variables or both are memory; sort by variable name
            ASSERT_require((al->is_variable() && bl->is_variable()) || (al->is_memory() && bl->is_memory()));
            if (al->get_name() != bl->get_name())
                return al->get_name() < bl->get_name() ? -1 : 1;
            return 0;
        }
    } else {
        // both are internal nodes
        ASSERT_not_null(ai);
        ASSERT_not_null(bi);
        if (ai->get_operator() != bi->get_operator())
            return ai->get_operator() < bi->get_operator() ? -1 : 1;
        for (size_t i=0; i<std::min(ai->nchildren(), bi->nchildren()); ++i) {
            if (int cmp = expr_cmp(ai->child(i), bi->child(i)))
                return cmp;
        }
        if (ai->nchildren() != bi->nchildren())
            return ai->nchildren() < bi->nchildren() ? -1 : 1;
        return 0;
    }
}

static bool
commutative_order(const TreeNodePtr &a, const TreeNodePtr &b)
{
    if (int cmp = expr_cmp(a, b))
        return cmp<0;
    return getRawPointer(a) < getRawPointer(b); // make it a strict ordering
}

InternalNodePtr
InternalNode::commutative() const
{
    const TreeNodes &orig_operands = get_children();
    TreeNodes sorted_operands = orig_operands;
    std::sort(sorted_operands.begin(), sorted_operands.end(), commutative_order);
    if (std::equal(sorted_operands.begin(), sorted_operands.end(), orig_operands.begin()))
        return isInternalNode();

    // construct the new node but don't simplify it yet (i.e., don't use InternalNode::create())
    InternalNode *inode = new InternalNode(get_nbits(), get_operator(), sorted_operands, get_comment());
    return InternalNodePtr(inode);
}

TreeNodePtr
InternalNode::involutary() const
{
    if (InternalNodePtr inode = isInternalNode()) {
        if (1==inode->nchildren()) {
            if (InternalNodePtr sub1 = inode->child(0)->isInternalNode()) {
                if (sub1->get_operator() == inode->get_operator() && 1==sub1->nchildren()) {
                    return sub1->child(0);
                }
            }
        }
    }
    return sharedFromThis();
}

// simplifies things like:
//   (shift a (shift b x)) ==> (shift (add a b) x)
// making sure a and b are extended to the same width
TreeNodePtr
InternalNode::additive_nesting() const
{
    InternalNodePtr nested = child(1)->isInternalNode();
    if (nested!=NULL && nested->get_operator()==get_operator()) {
        ASSERT_require(nested->nchildren()==nchildren());
        ASSERT_require(nested->get_nbits()==get_nbits());
        size_t additive_nbits = std::max(child(0)->get_nbits(), nested->child(0)->get_nbits());

        // The two addends must be the same width, so zero-extend them if necessary (or should we sign extend?)
        // Note that the first argument (new width) of the UEXTEND operator is not actually used.
        TreeNodePtr a = child(0)->get_nbits()==additive_nbits ? child(0) :
                        InternalNode::create(additive_nbits, OP_UEXTEND, LeafNode::create_integer(8, additive_nbits),
                                             child(0));
        TreeNodePtr b = nested->child(0)->get_nbits()==additive_nbits ? nested->child(0) :
                        InternalNode::create(additive_nbits, OP_UEXTEND, LeafNode::create_integer(8, additive_nbits),
                                             nested->child(0));
        
        // construct the new node but don't simplify it yet (i.e., don't use InternalNode::create())
        InternalNode *inode = new InternalNode(get_nbits(), get_operator(),
                                               InternalNode::create(additive_nbits, OP_ADD, a, b),
                                               nested->child(1), get_comment());
        return InternalNodePtr(inode);
    }
    return isInternalNode();
}

TreeNodePtr
InternalNode::identity(uint64_t ident) const
{
    TreeNodes args;
    bool modified = false;
    for (TreeNodes::const_iterator ci=children.begin(); ci!=children.end(); ++ci) {
        LeafNodePtr leaf = (*ci)->isLeafNode();
        if (leaf && leaf->is_known()) {
            Sawyer::Container::BitVector identBv = Sawyer::Container::BitVector(leaf->get_nbits()).fromInteger(ident);
            if (0==leaf->get_bits().compare(identBv)) {
                // skip this arg
                modified = true;
            } else {
                args.push_back(*ci);
            }
        } else {
            args.push_back(*ci);
        }
    }
    if (!modified)
        return sharedFromThis();
    if (args.empty())
        return LeafNode::create_integer(get_nbits(), ident, get_comment());
    if (1==args.size()) {
        if (args.front()->get_nbits()!=get_nbits())
            return InternalNode::create(get_nbits(), OP_UEXTEND, LeafNode::create_integer(8, get_nbits()), args.front());
        return args.front();
    }
    
    // construct the new node but don't simplify it yet (i.e., don't use InternalNode::create())
    return InternalNodePtr(new InternalNode(get_nbits(), get_operator(), args, get_comment()));
}

TreeNodePtr
InternalNode::unaryNoOp() const
{
    return 1==nchildren() ? child(0) : sharedFromThis();
}

TreeNodePtr
InternalNode::rewrite(const Simplifier &simplifier) const
{
    if (TreeNodePtr simplified = simplifier.rewrite(this))
        return simplified;
    return sharedFromThis();
}

TreeNodePtr
InternalNode::constant_folding(const Simplifier &simplifier) const
{
    TreeNodes newOperands;
    bool modified = false;
    TreeNodes::const_iterator ci1 = children.begin();
    while (ci1!=children.end()) {
        TreeNodes::const_iterator ci2 = ci1;
        LeafNodePtr leaf;
        while (ci2!=children.end() && (leaf=(*ci2)->isLeafNode()) && leaf->is_known()) ++ci2;
        if (ci1==ci2 || ci1+1==ci2) {                           // arg is not a constant, or we had only one constant by itself
            newOperands.push_back(*ci1);
            ++ci1;
        } else if (TreeNodePtr folded = simplifier.fold(ci1, ci2)) { // able to fold all these constants into a new node
            newOperands.push_back(folded);
            modified = true;
            ci1 = ci2;
        } else {                                                // multiple constants, but unable to fold
            newOperands.insert(newOperands.end(), ci1, ci2);
            ci1 = ci2;
        }
    }
    if (!modified)
        return isInternalNode();
    if (1==newOperands.size())
        return newOperands.front();

    // Do not simplify again (i.e., don't use InternalNode::create())
    return InternalNodePtr(new InternalNode(get_nbits(), op, newOperands, get_comment()));
}

TreeNodePtr
AddSimplifier::fold(TreeNodes::const_iterator begin, TreeNodes::const_iterator end) const
{
    Sawyer::Container::BitVector accumulator((*begin)->get_nbits());
    unsigned flags = 0;
    for (/*void*/; begin!=end; ++begin) {
        accumulator.add((*begin)->isLeafNode()->get_bits());
        flags |= (*begin)->get_flags();
    }
    return LeafNode::create_constant(accumulator, "", flags);
}

TreeNodePtr
AddSimplifier::rewrite(const InternalNode *inode) const
{
    // A and B are duals if they have one of the following forms:
    //    (1) A = x           AND  B = (negate x)
    //    (2) A = x           AND  B = (invert x)   [adjust constant]
    //    (3) A = (negate x)  AND  B = x
    //    (4) A = (invert x)  AND  B = x            [adjust constant]
    //
    // This makes use of the relationship:
    //   (add (negate x) -1) == (invert x)
    // by decrementing adjustment. The adjustment, whose width is the same as A and B, is allowed to overflow.  For example,
    // consider the expression, where all values are two bits wide:
    //   (add v1 (invert v1) v2 (invert v2) v3 (invert v3))            by substitution for invert:
    //   (add v1 (negate v1) -1 v2 (negate v2) -1 v3 (negate v3) -1)   canceling duals gives:
    //   (add -1 -1 -1)                                                rewriting as 2's complement (2 bits wide):
    //   (add 3 3 3)                                                   constant folding modulo 4:
    //   1
    // compare with v1=0, v2=1, v3=2 (i.e., -2 in two's complement):
    //   (add 0 3 1 2 2 1) == 1 mod 4
    struct are_duals {
        bool operator()(TreeNodePtr a, TreeNodePtr b, Sawyer::Container::BitVector &adjustment/*in,out*/) {
            ASSERT_not_null(a);
            ASSERT_not_null(b);
            ASSERT_require(a->get_nbits()==b->get_nbits());

            // swap A and B if necessary so we have form (1) or (2).
            if (b->isInternalNode()==NULL)
                std::swap(a, b);
            if (b->isInternalNode()==NULL)
                return false;

            InternalNodePtr bi = b->isInternalNode();
            if (bi->get_operator()==OP_NEGATE) {
                // form (3)
                ASSERT_require(1==bi->nchildren());
                return a->equivalent_to(bi->child(0));
            } else if (bi->get_operator()==OP_INVERT) {
                // form (4) and ninverts is small enough
                if (a->equivalent_to(bi->child(0))) {
                    adjustment.decrement();
                    return true;
                }
            }
            return false;
        }
    };

    // Arguments that are negated cancel out similar arguments that are not negated
    bool had_duals = false;
    Sawyer::Container::BitVector adjustment(inode->get_nbits());
    TreeNodes children = inode->get_children();
    for (size_t i=0; i<children.size(); ++i) {
        if (children[i]!=NULL) {
            for (size_t j=i+1; j<children.size() && children[j]!=NULL; ++j) {
                if (children[j]!=NULL && are_duals()(children[i], children[j], adjustment/*in,out*/)) {
                    children[i] = Sawyer::Nothing();
                    children[j] = Sawyer::Nothing();
                    had_duals = true;
                    break;
                }
            }
        }
    }
    if (!had_duals)
        return TreeNodePtr();

    // Build the new expression
    children.erase(std::remove(children.begin(), children.end(), TreeNodePtr()), children.end());
    if (!adjustment.isEqualToZero())
        children.push_back(LeafNode::create_constant(adjustment));
    if (children.empty())
        return LeafNode::create_integer(inode->get_nbits(), 0, inode->get_comment());
    if (children.size()==1)
        return children[0];
    return InternalNode::create(inode->get_nbits(), OP_ADD, children, inode->get_comment());
}

TreeNodePtr
AndSimplifier::fold(TreeNodes::const_iterator begin, TreeNodes::const_iterator end) const
{
    Sawyer::Container::BitVector accumulator((*begin)->get_nbits(), true);
    unsigned flags = 0;
    for (/*void*/; begin!=end; ++begin) {
        accumulator.bitwiseAnd((*begin)->isLeafNode()->get_bits());
        flags |= (*begin)->get_flags();
    }
    return LeafNode::create_constant(accumulator, "", flags);
}

TreeNodePtr
AndSimplifier::rewrite(const InternalNode *inode) const
{
    // Result is zero if any argument is zero
    for (size_t i=0; i<inode->nchildren(); ++i) {
        LeafNodePtr child = inode->child(i)->isLeafNode();
        if (child && child->is_known() && child->get_bits().isEqualToZero())
            return LeafNode::create_integer(inode->get_nbits(), 0, inode->get_comment(), child->get_flags());
    }

    // (and X X) => X (for any number of arguments that are all the same)
    bool allSameArgs = true;
    for (size_t i=1; i<inode->nchildren() && allSameArgs; ++i) {
        if (!inode->child(0)->equivalent_to(inode->child(i)))
            allSameArgs = false;
    }
    if (allSameArgs)
        return inode->child(0);

    return TreeNodePtr();
}

TreeNodePtr
OrSimplifier::fold(TreeNodes::const_iterator begin, TreeNodes::const_iterator end) const
{
    Sawyer::Container::BitVector accumulator((*begin)->get_nbits());
    unsigned flags = 0;
    for (/*void*/; begin!=end; ++begin) {
        accumulator.bitwiseOr((*begin)->isLeafNode()->get_bits());
        flags |= (*begin)->get_flags();
    }
    return LeafNode::create_constant(accumulator, "", flags);
}

TreeNodePtr
OrSimplifier::rewrite(const InternalNode *inode) const
{
    // Result has all bits set if any argument has all bits set
    for (size_t i=0; i<inode->nchildren(); ++i) {
        LeafNodePtr child = inode->child(i)->isLeafNode();
        if (child && child->is_known() && child->get_bits().isAllSet())
            return LeafNode::create_constant(child->get_bits(), inode->get_comment(), child->get_flags());
    }
    return TreeNodePtr();
}

TreeNodePtr
XorSimplifier::fold(TreeNodes::const_iterator begin, TreeNodes::const_iterator end) const
{
    Sawyer::Container::BitVector accumulator((*begin)->get_nbits());
    unsigned flags = 0;
    for (++begin; begin!=end; ++begin) {
        accumulator.bitwiseXor((*begin)->isLeafNode()->get_bits());
        flags |= (*begin)->get_flags();
    }
    return LeafNode::create_constant(accumulator, "", flags);
}

TreeNodePtr
XorSimplifier::rewrite(const InternalNode *inode) const
{
    SMTSolver *solver = NULL;   // FIXME

    // If any pairs of arguments are equal, then they don't contribute to the final answer.
    std::vector<bool> removed(inode->nchildren(), false);
    bool modified = false;
    for (size_t i=0; i<inode->nchildren(); ++i) {
        if (removed[i])
            continue;
        for (size_t j=i+1; j<inode->nchildren(); ++j) {
            if (!removed[j] && inode->child(i)->must_equal(inode->child(j), solver)) {
                removed[i] = removed[j] = modified = true;
                break;
            }
        }
    }
    if (!modified)
        return TreeNodePtr();
    TreeNodes newargs;
    for (size_t i=0; i<inode->nchildren(); ++i) {
        if (!removed[i])
            newargs.push_back(inode->child(i));
    }
    if (newargs.empty())
        return LeafNode::create_integer(inode->get_nbits(), 0, inode->get_comment());
    return InternalNode::create(inode->get_nbits(), inode->get_operator(), newargs, inode->get_comment());
}

TreeNodePtr
SmulSimplifier::fold(TreeNodes::const_iterator begin, TreeNodes::const_iterator end) const
{
    // FIXME[Robb P. Matzke 2014-05-05]: Constant folding is not currently possible when the operands are wider than 64 bits
    // because Sawyer::Container::BitVector does not provide a multiplication method.
    size_t totalWidth = 0;
    int64_t product = 1;
    unsigned flags = 0;
    for (/*void*/; begin!=end; ++begin) {
        size_t nbits = (*begin)->get_nbits();
        totalWidth += nbits;
        if (totalWidth > 8*sizeof(product))
            return TreeNodePtr();
        LeafNodePtr leaf = (*begin)->isLeafNode();
        product *= (int64_t)leaf->get_value();
        flags |= (*begin)->get_flags();
    }
    return LeafNode::create_integer(totalWidth, product, "", flags);
}

TreeNodePtr
UmulSimplifier::fold(TreeNodes::const_iterator begin, TreeNodes::const_iterator end) const
{
    // FIXME[Robb P. Matzke 2014-05-05]: Constant folding is not currently possible when the operands are wider than 64 bits
    // because Sawyer::Container::BitVector does not provide a multiplication method.
    size_t totalWidth = 0;
    uint64_t product = 1;
    unsigned flags = 0;
    for (/*void*/; begin!=end; ++begin) {
        size_t nbits = (*begin)->get_nbits();
        totalWidth += nbits;
        if (totalWidth > 8*sizeof(product))
            return TreeNodePtr();
        LeafNodePtr leaf = (*begin)->isLeafNode();
        product *= (uint64_t)leaf->get_value();
        flags |= (*begin)->get_flags();
    }
    return LeafNode::create_integer(totalWidth, product, "", flags);
}

TreeNodePtr
ConcatSimplifier::fold(TreeNodes::const_iterator begin, TreeNodes::const_iterator end) const
{
    // first arg is high-order bits. Although this is nice to look at, it makes the operation a bit more difficult.
    size_t resultSize = 0;
    for (TreeNodes::const_iterator ti=begin; ti!=end; ++ti)
        resultSize += (*ti)->get_nbits();
    Sawyer::Container::BitVector accumulator(resultSize);

    // Copy bits into wherever they belong in the accumulator
    unsigned flags = 0;
    for (size_t sa=resultSize; begin!=end; ++begin) {
        LeafNodePtr leaf = (*begin)->isLeafNode();
        sa -= leaf->get_nbits();
        typedef Sawyer::Container::BitVector::BitRange BitRange;
        BitRange destination = BitRange::baseSize(sa, leaf->get_nbits());
        accumulator.copy(destination, leaf->get_bits(), leaf->get_bits().hull());
        flags |= (*begin)->get_flags();
    }
    return LeafNode::create_constant(accumulator, "", flags);
}

TreeNodePtr
ConcatSimplifier::rewrite(const InternalNode *inode) const
{
    SMTSolver *solver = NULL; // FIXME

    // If all the concatenated expressions are extract expressions, all extracting bits from the same expression and
    // in the correct order, then we can simplify this to that expression.  For instance:
    //   (concat[32]
    //       (extract[8] 24[32] 32[32] v2[32])
    //       (extract[8] 16[32] 24[32] v2[32])
    //       (extract[8] 8[32] 16[32] v2[32])
    //       (extract[8] 0[32] 8[32] v2[32]))
    // can be simplified to
    //   v2
    TreeNodePtr retval;
    size_t offset = 0;
    for (size_t i=inode->nchildren(); i>0; --i) { // process args in little endian order
        InternalNodePtr extract = inode->child(i-1)->isInternalNode();
        if (!extract || OP_EXTRACT!=extract->get_operator())
            break;
        LeafNodePtr from_node = extract->child(0)->isLeafNode();
        ASSERT_require(from_node->get_nbits() <= 8*sizeof offset);
        if (!from_node || !from_node->is_known() || from_node->get_value()!=offset ||
            extract->child(2)->get_nbits()!=inode->get_nbits())
            break;
        if (inode->nchildren()==i) {
            retval = extract->child(2);
        } else if (!extract->child(2)->must_equal(retval, solver)) {
            break;
        }
        offset += extract->get_nbits();
    }
    if (offset==inode->get_nbits())
        return retval;
    return TreeNodePtr();
}

TreeNodePtr
ExtractSimplifier::rewrite(const InternalNode *inode) const
{
    LeafNodePtr from_node = inode->child(0)->isLeafNode();
    LeafNodePtr to_node   = inode->child(1)->isLeafNode();
    TreeNodePtr operand   = inode->child(2);
    ASSERT_require(!from_node->is_known() || from_node->get_nbits() <= 8*sizeof(size_t));
    ASSERT_require(!to_node->is_known()   || to_node->get_nbits() <= 8*sizeof(size_t));
    size_t from = from_node && from_node->is_known() ? from_node->get_value() : 0;
    size_t to = to_node && to_node->is_known() ? to_node->get_value() : 0;

    // If limits are backward or extend beyond the operand size, don't simplify
    if (from_node && to_node && from_node->is_known() && to_node->is_known() && (from>=to || to>operand->get_nbits()))
        return TreeNodePtr();

    // Constant folding
    if (from_node && to_node && from_node->is_known() && to_node->is_known() &&
        operand->isLeafNode() && operand->isLeafNode()->is_known()) {
        Sawyer::Container::BitVector result(to-from);
        typedef Sawyer::Container::BitVector::BitRange BitRange;
        BitRange source = BitRange::hull(from, to-1);
        result.copy(result.hull(), operand->isLeafNode()->get_bits(), source);
        return LeafNode::create_constant(result, inode->get_comment(), inode->get_flags());
    }

    // Extracting the whole thing is a no-op
    if (from_node && to_node && from_node->is_known() && from==0 && to==operand->get_nbits())
        return operand;

    // Hoist concat operations to the outside of the extract
    // If the operand is a concat operation then take only the parts we need.  Some examples:
    // (extract 0 24 (concat X[24] Y[8]))  ==> (concat (extract 0 16 X) Y)
    TreeNodes newChildren;
    InternalNodePtr ioperand = operand->isInternalNode();
    if (from_node && to_node && from_node->is_known() && to_node->is_known() &&
        ioperand && OP_CONCAT==ioperand->get_operator()) {
        size_t partAt = 0;                              // starting bit number in child
        BOOST_REVERSE_FOREACH (const TreeNodePtr part, ioperand->get_children()) { // concatenated parts
            size_t partEnd = partAt + part->get_nbits();
            if (partEnd <= from) {
                // Part is entirely left of what we need
                partAt = partEnd;
            } else if (partAt >= to) {
                // Part is entirely right of what we need
                break;
            } else if (partAt < from && partEnd > to) {
                // We need the middle of this part, and then we're done
                size_t need = to-from;                  // number of bits we need
                newChildren.push_back(InternalNode::create(need, OP_EXTRACT,
                                                           LeafNode::create_integer(32, from-partAt),
                                                           LeafNode::create_integer(32, to-partAt),
                                                           part));
                partAt = partEnd;
                from += need;
            } else if (partAt < from) {
                // We need the end of the part
                ASSERT_require(partEnd <= to);
                size_t need = partEnd - from;
                newChildren.push_back(InternalNode::create(need, OP_EXTRACT,
                                                           LeafNode::create_integer(32, from-partAt),
                                                           LeafNode::create_integer(32, part->get_nbits()),
                                                           part));
                partAt = partEnd;
                from += need;
            } else if (partEnd > to) {
                // We need the beginning of the part
                ASSERT_require(partAt == from);
                size_t need = to-from;
                newChildren.push_back(InternalNode::create(need, OP_EXTRACT,
                                                           LeafNode::create_integer(32, 0),
                                                           LeafNode::create_integer(32, need),
                                                           part));
                break;
            } else {
                // We need the whole part
                ASSERT_require(partAt >= from);
                ASSERT_require(partEnd <= to);
                newChildren.push_back(part);
                partAt = from = partEnd;
            }
        }

        // Concatenate all the parts.
        if (newChildren.size() > 1) {
            std::reverse(newChildren.begin(), newChildren.end());// high bits must be first
            return InternalNode::create(inode->get_nbits(), OP_CONCAT, newChildren, inode->get_comment());
        }
        newChildren[0]->set_comment(inode->get_comment());
        return newChildren[0];
    }

    // If the operand is another extract operation and we know all the limits then they can be replaced with a single extract.
    if (from_node && to_node && from_node->is_known() && to_node->is_known() &&
        ioperand && OP_EXTRACT==ioperand->get_operator()) {
        LeafNodePtr from2_node = ioperand->child(0)->isLeafNode();
        LeafNodePtr to2_node = ioperand->child(1)->isLeafNode();
        if (from2_node && to2_node && from2_node->is_known() && to2_node->is_known()) {
            size_t from2 = from2_node->get_value();
            return InternalNode::create(inode->get_nbits(), OP_EXTRACT,
                                        LeafNode::create_integer(32, from2+from),
                                        LeafNode::create_integer(32, from2+to),
                                        ioperand->child(2),
                                        inode->get_comment());
        }
    }

    // Simplifications for (extract 0 a (uextend b c))
    if (from_node && to_node && from_node->is_known() && 0==from && to_node->is_known()) {
        size_t a=to, b=operand->get_nbits();
        // (extract[a] 0 a (uextend[b] b c[a])) => c when b>=a
        if (ioperand && OP_UEXTEND==ioperand->get_operator() && b>=a && ioperand->child(1)->get_nbits()==a)
            return ioperand->child(1);
    }


    return TreeNodePtr();
}

TreeNodePtr
AsrSimplifier::rewrite(const InternalNode *inode) const
{
    ASSERT_require(2==inode->nchildren());

    // Constant folding
    LeafNodePtr shift_leaf   = inode->child(0)->isLeafNode();
    LeafNodePtr operand_leaf = inode->child(1)->isLeafNode();
    if (shift_leaf!=NULL && operand_leaf!=NULL && shift_leaf->is_known() && operand_leaf->is_known()) {
        size_t sa = shift_leaf->get_value();
        Sawyer::Container::BitVector result = operand_leaf->get_bits();
        result.shiftRightArithmetic(sa);
        return LeafNode::create_constant(result, inode->get_comment(), inode->get_flags());
    }
    return TreeNodePtr();
}

TreeNodePtr
InvertSimplifier::rewrite(const InternalNode *inode) const
{
    // Constant folding
    LeafNodePtr operand_node = inode->child(0)->isLeafNode();
    if (operand_node==NULL || !operand_node->is_known())
        return TreeNodePtr();
    Sawyer::Container::BitVector result = operand_node->get_bits();
    result.invert();
    return LeafNode::create_constant(result, inode->get_comment(), inode->get_flags());
}

TreeNodePtr
NegateSimplifier::rewrite(const InternalNode *inode) const
{
    // Constant folding
    LeafNodePtr operand_node = inode->child(0)->isLeafNode();
    if (operand_node==NULL || !operand_node->is_known())
        return TreeNodePtr();
    Sawyer::Container::BitVector result = operand_node->get_bits();
    result.negate();
    return LeafNode::create_constant(result, inode->get_comment(), inode->get_flags());
}

TreeNodePtr
IteSimplifier::rewrite(const InternalNode *inode) const
{
    // Is the condition known?
    LeafNodePtr cond_node = inode->child(0)->isLeafNode();
    if (cond_node!=NULL && cond_node->is_known()) {
        ASSERT_require(1==cond_node->get_nbits());
        return cond_node->get_value() ? inode->child(1) : inode->child(2);
    }

    // Are both operands the same? Then the condition doesn't matter
    if (inode->child(1)->equivalent_to(inode->child(2)))
        return inode->child(1);

    return TreeNodePtr();
}

TreeNodePtr
NoopSimplifier::rewrite(const InternalNode *inode) const
{
    if (1==inode->nchildren())
        return inode->child(0);
    return TreeNodePtr();
}

TreeNodePtr
RolSimplifier::rewrite(const InternalNode *inode) const
{
    // Constant folding
    LeafNodePtr sa_leaf = inode->child(0)->isLeafNode();
    LeafNodePtr val_leaf = inode->child(1)->isLeafNode();
    if (sa_leaf && val_leaf && sa_leaf->is_known() && val_leaf->is_known()) {
        Sawyer::Container::BitVector result = val_leaf->get_bits();
        result.rotateLeft(sa_leaf->get_value());
        return LeafNode::create_constant(result, inode->get_comment(), inode->get_flags());
    }

    // If the shift amount is known and is a multiple of the operand size, then this is a no-op
    if (sa_leaf && sa_leaf->is_known() && 0==sa_leaf->get_value() % inode->get_nbits())
        return inode->child(1);

    return TreeNodePtr();
}
TreeNodePtr
RorSimplifier::rewrite(const InternalNode *inode) const
{
    // Constant folding
    LeafNodePtr sa_leaf = inode->child(0)->isLeafNode();
    LeafNodePtr val_leaf = inode->child(1)->isLeafNode();
    if (sa_leaf && val_leaf && sa_leaf->is_known() && val_leaf->is_known()) {
        Sawyer::Container::BitVector result = val_leaf->get_bits();
        result.rotateRight(sa_leaf->get_value());
        return LeafNode::create_constant(result, inode->get_comment(), inode->get_flags());
    }

    // If the shift amount is known and is a multiple of the operand size, then this is a no-op
    if (sa_leaf && sa_leaf->is_known() && 0==sa_leaf->get_value() % inode->get_nbits())
        return inode->child(1);

    return TreeNodePtr();
}

TreeNodePtr
UextendSimplifier::rewrite(const InternalNode *inode) const
{
    // Noop case
    size_t oldsize = inode->child(1)->get_nbits();
    size_t newsize = inode->get_nbits();
    if (oldsize==newsize)
        return inode->child(1);

    // Constant folding
    LeafNodePtr val_leaf = inode->child(1)->isLeafNode();
    if (val_leaf && val_leaf->is_known()) {
        Sawyer::Container::BitVector result = val_leaf->get_bits();
        result.resize(newsize);
        return LeafNode::create_constant(result, inode->get_comment(), inode->get_flags());
    }

    // If the new size is smaller than the old size, use OP_EXTRACT instead.
    if (newsize<oldsize) {
        return InternalNode::create(newsize, OP_EXTRACT,
                                    LeafNode::create_integer(32, 0),
                                    LeafNode::create_integer(32, newsize),
                                    inode->child(1),
                                    inode->get_comment());
    }

    return TreeNodePtr();
}

TreeNodePtr
SextendSimplifier::rewrite(const InternalNode *inode) const
{
    // Noop case
    size_t oldsize = inode->child(1)->get_nbits();
    size_t newsize = inode->get_nbits();
    if (oldsize==newsize)
        return inode->child(1);

    // Constant folding
    LeafNodePtr val_leaf = inode->child(1)->isLeafNode();
    if (val_leaf && val_leaf->is_known()) {
        Sawyer::Container::BitVector result(inode->get_nbits());
        result.signExtend(val_leaf->get_bits());
        return LeafNode::create_constant(result, inode->get_comment(), inode->get_flags());
    }

    // Downsizing should be represented as an extract operation
    if (newsize < oldsize) {
        return InternalNode::create(newsize, OP_EXTRACT,
                                    LeafNode::create_integer(32, 0),
                                    LeafNode::create_integer(32, newsize),
                                    inode->child(1),
                                    inode->get_comment());
    }

    return TreeNodePtr();
}

TreeNodePtr
EqSimplifier::rewrite(const InternalNode *inode) const
{
    // Constant folding
    LeafNodePtr a_leaf = inode->child(0)->isLeafNode();
    LeafNodePtr b_leaf = inode->child(1)->isLeafNode();
    if (a_leaf && b_leaf && a_leaf->is_known() && b_leaf->is_known()) {
        int cmp = a_leaf->get_bits().compare(b_leaf->get_bits());
        return LeafNode::create_boolean(0==cmp, inode->get_comment(), inode->get_flags());
    }

    // (eq x x) => 1
    if (inode->child(0)->must_equal(inode->child(1), NULL))
        return LeafNode::create_boolean(true, inode->get_comment(), inode->get_flags());

    return TreeNodePtr();
}

TreeNodePtr
SgeSimplifier::rewrite(const InternalNode *inode) const
{
    // Constant folding
    LeafNodePtr a_leaf = inode->child(0)->isLeafNode();
    LeafNodePtr b_leaf = inode->child(1)->isLeafNode();
    if (a_leaf && b_leaf && a_leaf->is_known() && b_leaf->is_known()) {
        int cmp = a_leaf->get_bits().compareSigned(b_leaf->get_bits());
        return LeafNode::create_boolean(cmp>=0, inode->get_comment(), inode->get_flags());
    }

    // (sge x x) => 1
    if (inode->child(0)->must_equal(inode->child(1), NULL))
        return LeafNode::create_boolean(true, inode->get_comment(), inode->get_flags());

    return TreeNodePtr();
}

TreeNodePtr
SgtSimplifier::rewrite(const InternalNode *inode) const
{
    // Constant folding
    LeafNodePtr a_leaf = inode->child(0)->isLeafNode();
    LeafNodePtr b_leaf = inode->child(1)->isLeafNode();
    if (a_leaf && b_leaf && a_leaf->is_known() && b_leaf->is_known()) {
        int cmp = a_leaf->get_bits().compareSigned(b_leaf->get_bits());
        return LeafNode::create_boolean(cmp>0, inode->get_comment(), inode->get_flags());
    }

    return TreeNodePtr();
}

TreeNodePtr
SleSimplifier::rewrite(const InternalNode *inode) const
{
    // Constant folding
    LeafNodePtr a_leaf = inode->child(0)->isLeafNode();
    LeafNodePtr b_leaf = inode->child(1)->isLeafNode();
    if (a_leaf && b_leaf && a_leaf->is_known() && b_leaf->is_known()) {
        int cmp = a_leaf->get_bits().compareSigned(b_leaf->get_bits());
        return LeafNode::create_boolean(cmp<=0, inode->get_comment(), inode->get_flags());
    }

    // (sle x x) => 1
    if (inode->child(0)->must_equal(inode->child(1), NULL))
        return LeafNode::create_boolean(true, inode->get_comment(), inode->get_flags());

    return TreeNodePtr();
}

TreeNodePtr
SltSimplifier::rewrite(const InternalNode *inode) const
{
    // Constant folding
    LeafNodePtr a_leaf = inode->child(0)->isLeafNode();
    LeafNodePtr b_leaf = inode->child(1)->isLeafNode();
    if (a_leaf && b_leaf && a_leaf->is_known() && b_leaf->is_known()) {
        int cmp = a_leaf->get_bits().compareSigned(b_leaf->get_bits());
        return LeafNode::create_boolean(cmp<0, inode->get_comment(), inode->get_flags());
    }

    return TreeNodePtr();
}

TreeNodePtr
UgeSimplifier::rewrite(const InternalNode *inode) const
{
    // Constant folding
    LeafNodePtr a_leaf = inode->child(0)->isLeafNode();
    LeafNodePtr b_leaf = inode->child(1)->isLeafNode();
    if (a_leaf && b_leaf && a_leaf->is_known() && b_leaf->is_known()) {
        int cmp = a_leaf->get_bits().compare(b_leaf->get_bits());
        return LeafNode::create_boolean(cmp>=0, inode->get_comment(), inode->get_flags());
    }

    // (uge x x) => 1
    if (inode->child(0)->must_equal(inode->child(1), NULL))
        return LeafNode::create_boolean(true, inode->get_comment(), inode->get_flags());

   return TreeNodePtr();
}

TreeNodePtr
UgtSimplifier::rewrite(const InternalNode *inode) const
{
    // Constant folding
    LeafNodePtr a_leaf = inode->child(0)->isLeafNode();
    LeafNodePtr b_leaf = inode->child(1)->isLeafNode();
    if (a_leaf && b_leaf && a_leaf->is_known() && b_leaf->is_known()) {
        int cmp = a_leaf->get_bits().compare(b_leaf->get_bits());
        return LeafNode::create_boolean(cmp>0, inode->get_comment(), inode->get_flags());
    }

    return TreeNodePtr();
}

TreeNodePtr
UleSimplifier::rewrite(const InternalNode *inode) const
{
    // Constant folding
    LeafNodePtr a_leaf = inode->child(0)->isLeafNode();
    LeafNodePtr b_leaf = inode->child(1)->isLeafNode();
    if (a_leaf && b_leaf && a_leaf->is_known() && b_leaf->is_known()) {
        int cmp = a_leaf->get_bits().compare(b_leaf->get_bits());
        return LeafNode::create_boolean(cmp<=0, inode->get_comment(), inode->get_flags());
    }

    // (ule x x) => 1
    if (inode->child(0)->must_equal(inode->child(1), NULL))
        return LeafNode::create_boolean(true, inode->get_comment(), inode->get_flags());

    return TreeNodePtr();
}

TreeNodePtr
UltSimplifier::rewrite(const InternalNode *inode) const
{
    // Constant folding
    LeafNodePtr a_leaf = inode->child(0)->isLeafNode();
    LeafNodePtr b_leaf = inode->child(1)->isLeafNode();
    if (a_leaf && b_leaf && a_leaf->is_known() && b_leaf->is_known()) {
        int cmp = a_leaf->get_bits().compare(b_leaf->get_bits());
        return LeafNode::create_boolean(cmp<0, inode->get_comment(), inode->get_flags());
    }

    return TreeNodePtr();
}

TreeNodePtr
ZeropSimplifier::rewrite(const InternalNode *inode) const
{
    // Constant folding
    LeafNodePtr a_leaf = inode->child(0)->isLeafNode();
    if (a_leaf && a_leaf->is_known())
        return LeafNode::create_boolean(a_leaf->get_bits().isEqualToZero(), inode->get_comment(), inode->get_flags());
    
    return TreeNodePtr();
}

TreeNodePtr
SdivSimplifier::rewrite(const InternalNode *inode) const
{
    // Constant folding
    LeafNodePtr a_leaf = inode->child(0)->isLeafNode();
    LeafNodePtr b_leaf = inode->child(1)->isLeafNode();
    if (a_leaf && b_leaf && a_leaf->is_known() && b_leaf->is_known() && b_leaf->get_value()!=0) {
        if (a_leaf->get_nbits() <= 64 && b_leaf->get_nbits() <= 64) {
            int64_t a = IntegerOps::signExtend2(a_leaf->get_value(), a_leaf->get_nbits(), 8*sizeof(int8_t));
            int64_t b = IntegerOps::signExtend2(b_leaf->get_value(), b_leaf->get_nbits(), 8*sizeof(int8_t));
            return LeafNode::create_integer(a_leaf->get_nbits(), a/b, inode->get_comment(), inode->get_flags());
        } else {
            // FIXME[Robb P. Matzke 2014-05-05]: not folding constants larger than 64 bits because Sawyer::Container::BitVector
            // does not currently define division.
        }
    }
    return TreeNodePtr();
}

TreeNodePtr
SmodSimplifier::rewrite(const InternalNode *inode) const
{
    // Constant folding
    LeafNodePtr a_leaf = inode->child(0)->isLeafNode();
    LeafNodePtr b_leaf = inode->child(1)->isLeafNode();
    if (a_leaf && b_leaf && a_leaf->is_known() && b_leaf->is_known() && b_leaf->get_value()!=0) {
        if (a_leaf->get_nbits() <= 64 && b_leaf->get_nbits() <= 64) {
            int64_t a = IntegerOps::signExtend2(a_leaf->get_value(), a_leaf->get_nbits(), 8*sizeof(int8_t));
            int64_t b = IntegerOps::signExtend2(b_leaf->get_value(), b_leaf->get_nbits(), 8*sizeof(int8_t));
            return LeafNode::create_integer(b_leaf->get_nbits(), a%b, inode->get_comment(), inode->get_flags());
        } else {
            // FIXME[Robb P. Matzke 2014-05-05]: not folding constants larger than 64 bits because Sawyer::Container::BitVector
            // does not currently define division.
        }
    }

    return TreeNodePtr();
}

TreeNodePtr
UdivSimplifier::rewrite(const InternalNode *inode) const
{
    // Constant folding
    LeafNodePtr a_leaf = inode->child(0)->isLeafNode();
    LeafNodePtr b_leaf = inode->child(1)->isLeafNode();
    if (a_leaf && b_leaf && a_leaf->is_known() && b_leaf->is_known() && b_leaf->get_value()!=0) {
        if (a_leaf->get_nbits() <= 64 && b_leaf->get_nbits() <= 64) {
            uint64_t a = a_leaf->get_value();
            uint64_t b = b_leaf->get_value();
            return LeafNode::create_integer(a_leaf->get_nbits(), a/b, inode->get_comment(), inode->get_flags());
        } else {
            // FIXME[Robb P. Matzke 2014-05-05]: not folding constants larger than 64 bits because Sawyer::Container::BitVector
            // does not currently define division.
        }
    }

    return TreeNodePtr();
}

TreeNodePtr
UmodSimplifier::rewrite(const InternalNode *inode) const
{
    // Constant folding
    LeafNodePtr a_leaf = inode->child(0)->isLeafNode();
    LeafNodePtr b_leaf = inode->child(1)->isLeafNode();
    if (a_leaf && b_leaf && a_leaf->is_known() && b_leaf->is_known() && b_leaf->get_value()!=0) {
        if (a_leaf->get_nbits() <= 64 && b_leaf->get_nbits() <= 64) {
            uint64_t a = a_leaf->get_value();
            uint64_t b = b_leaf->get_value();
            return LeafNode::create_integer(b_leaf->get_nbits(), a%b, inode->get_comment(), inode->get_flags());
        } else {
            // FIXME[Robb P. Matzke 2014-05-05]: not folding constants larger than 64 bits because Sawyer::Container::BitVector
            // does not currently define division.
        }
    }

    return TreeNodePtr();
}

TreeNodePtr
ShiftSimplifier::combine_strengths(TreeNodePtr strength1, TreeNodePtr strength2, size_t value_width) const
{
    if (!strength1 || !strength2)
        return TreeNodePtr();

    // Calculate the width for the sum of the strengths.  If the width of the value being shifted isn't a power of two then we
    // need to avoid overflow in the sum, otherwise overflow doesn't matter.  The sum should be wide enough to hold a shift
    // amount that's the same as the width of the value, otherwise we wouldn't be able to distinguish between the case where
    // modulo addition produced a shift amount that's large enough to decimate the value, as opposed to a shift count of zero
    // which is a no-op.
    size_t sum_width = std::max(strength1->get_nbits(), strength2->get_nbits());
    if (IntegerOps::isPowerOfTwo(value_width)) {
        sum_width = std::max(sum_width, IntegerOps::log2max(value_width)+1);
    } else {
        sum_width = std::max(sum_width+1, IntegerOps::log2max(value_width)+1);
    }
    if (sum_width > 64)
        return TreeNodePtr();

    // Zero-extend the strengths if they're not as wide as the sum.  This is because the ADD operator requires that its
    // operands are the same width, and the result will also be that width.
    if (strength1->get_nbits() < sum_width)
        strength1 = InternalNode::create(sum_width, OP_UEXTEND, LeafNode::create_integer(32, sum_width), strength1);
    if (strength2->get_nbits() < sum_width)
        strength2 = InternalNode::create(sum_width, OP_UEXTEND, LeafNode::create_integer(32, sum_width), strength2);

    return InternalNode::create(sum_width, OP_ADD, strength1, strength2);
}

TreeNodePtr
ShlSimplifier::rewrite(const InternalNode *inode) const
{
    // Constant folding
    LeafNodePtr sa_leaf = inode->child(0)->isLeafNode();
    LeafNodePtr val_leaf = inode->child(1)->isLeafNode();
    if (sa_leaf && val_leaf && sa_leaf->is_known() && val_leaf->is_known()) {
        uint64_t sa = sa_leaf->get_value();
        sa = std::min((uint64_t)inode->get_nbits(), sa);
        Sawyer::Container::BitVector result = val_leaf->get_bits();
        result.shiftLeft(sa, newbits);
        return LeafNode::create_constant(result, inode->get_comment(), inode->get_flags());
    }

    // If the shifted operand is itself a shift of the same kind, then simplify by combining the strengths:
    // (shl AMT1 (shl AMT2 X)) ==> (shl (add AMT1 AMT2) X)
    InternalNodePtr val_inode = inode->child(1)->isInternalNode();
    if (val_inode && val_inode->get_operator()==inode->get_operator()) {
        if (TreeNodePtr strength = combine_strengths(inode->child(0), val_inode->child(0), inode->child(1)->get_nbits())) {
            return InternalNode::create(inode->get_nbits(), inode->get_operator(), strength, val_inode->child(1));
        }
    }

    // If the shift amount is known to be at least as large as the value, then replace the value with a constant.
    if (sa_leaf && sa_leaf->is_known() && sa_leaf->get_value() >= inode->get_nbits()) {
        Sawyer::Container::BitVector result(inode->get_nbits(), newbits);
        return LeafNode::create_constant(result, inode->get_comment());
    }

    // If the shift amount is zero then this is a no-op
    if (sa_leaf && sa_leaf->is_known() && sa_leaf->get_value()==0)
        return inode->child(1);

    // If the shift amount is a constant, then:
    // (shl0[N] AMT X) ==> (concat (extract 0 N-AMT X)<hiBits> 0[AMT]<loBits>)
    // (shl1[N] AMT X) ==> (concat (extract 0 N-AMT X)<hiBits> -1[AMT]<loBits>)
    if (sa_leaf && sa_leaf->is_known()) {
        ASSERT_require(sa_leaf->get_value()>0 && sa_leaf->get_value()<inode->get_nbits());// handled above
        size_t nHiBits = inode->get_nbits() - sa_leaf->get_value();
        TreeNodePtr hiBits = InternalNode::create(nHiBits, OP_EXTRACT,
                                                  LeafNode::create_integer(32, 0), LeafNode::create_integer(32, nHiBits),
                                                  inode->child(1));
        TreeNodePtr loBits = LeafNode::create_integer(sa_leaf->get_value(), newbits?uint64_t(-1):uint64_t(0));
        return InternalNode::create(inode->get_nbits(), OP_CONCAT, hiBits, loBits);
    }
    
    return TreeNodePtr();
}

TreeNodePtr
ShrSimplifier::rewrite(const InternalNode *inode) const
{
    // Constant folding
    LeafNodePtr sa_leaf = inode->child(0)->isLeafNode();
    LeafNodePtr val_leaf = inode->child(1)->isLeafNode();
    if (sa_leaf && val_leaf && sa_leaf->is_known() && val_leaf->is_known()) {
        uint64_t sa = sa_leaf->get_value();
        sa = std::min((uint64_t)inode->get_nbits(), sa);
        Sawyer::Container::BitVector result = val_leaf->get_bits();
        result.shiftRight(sa, newbits);
        return LeafNode::create_constant(result, inode->get_comment(), inode->get_flags());
    }

    // If the shifted operand is itself a shift of the same kind, then simplify by combining the strengths:
    //   (shr0 AMT1 (shr0 AMT2 X)) ==> (shr0 (add AMT1 AMT2) X)
    InternalNodePtr val_inode = inode->child(1)->isInternalNode();
    if (val_inode && val_inode->get_operator()==inode->get_operator()) {
        if (TreeNodePtr strength = combine_strengths(inode->child(0), val_inode->child(0), inode->child(1)->get_nbits())) {
            return InternalNode::create(inode->get_nbits(), inode->get_operator(), strength, val_inode->child(1));
        }
    }
    
    // If the shift amount is known to be at least as large as the value, then replace the value with a constant.
    if (sa_leaf && sa_leaf->is_known() && sa_leaf->get_value() >= inode->get_nbits()) {
        Sawyer::Container::BitVector result(inode->get_nbits(), newbits);
        return LeafNode::create_constant(result, inode->get_comment(), inode->get_flags());
    }

    // If the shift amount is zero then this is a no-op
    if (sa_leaf && sa_leaf->is_known() && sa_leaf->get_value()==0)
        return inode->child(1);

    // If the shift amount is a constant, then:
    // (shr0[N] AMT X) ==> (concat 0[AMT]  (extract AMT N X))
    // (shr1[N] AMT X) ==> (concat -1[AMT] (extract AMT N X))
    if (sa_leaf && sa_leaf->is_known()) {
        ASSERT_require(sa_leaf->get_value()>0 && sa_leaf->get_value()<inode->get_nbits());// handled above
        size_t nLoBits = inode->get_nbits() - sa_leaf->get_value();
        TreeNodePtr loBits = InternalNode::create(nLoBits, OP_EXTRACT,
                                                  LeafNode::create_integer(32, sa_leaf->get_value()),
                                                  LeafNode::create_integer(32, inode->get_nbits()),
                                                  inode->child(1));
        TreeNodePtr hiBits = LeafNode::create_integer(sa_leaf->get_value(), newbits?uint64_t(-1):uint64_t(0));
        return InternalNode::create(inode->get_nbits(), OP_CONCAT, hiBits, loBits);
    }
    
    return TreeNodePtr();
}

TreeNodePtr
LssbSimplifier::rewrite(const InternalNode *inode) const
{
    // Constant folding
    LeafNodePtr a_leaf = inode->child(0)->isLeafNode();
    if (a_leaf && a_leaf->is_known()) {
        if (Sawyer::Optional<size_t> idx = a_leaf->get_bits().leastSignificantSetBit())
            return LeafNode::create_integer(inode->get_nbits(), *idx, inode->get_comment());
        return LeafNode::create_integer(inode->get_nbits(), 0, inode->get_comment(), inode->get_flags());
    }

    return TreeNodePtr();
}

TreeNodePtr
MssbSimplifier::rewrite(const InternalNode *inode) const
{
    // Constant folding
    LeafNodePtr a_leaf = inode->child(0)->isLeafNode();
    if (a_leaf && a_leaf->is_known()) {
        if (Sawyer::Optional<size_t> idx = a_leaf->get_bits().mostSignificantSetBit())
            return LeafNode::create_integer(inode->get_nbits(), *idx, inode->get_comment());
        return LeafNode::create_integer(inode->get_nbits(), 0, inode->get_comment(), inode->get_flags());
    }

    return TreeNodePtr();
}

TreeNodePtr
InternalNode::simplifyTop() const
{
    TreeNodePtr node = sharedFromThis();
    while (InternalNodePtr inode = node->isInternalNode()) {
        TreeNodePtr newnode = node;
        switch (inode->get_operator()) {
            case OP_ADD:
                newnode = inode->nonassociative()->commutative()->identity(0);
                if (newnode==node)
                    newnode = inode->constant_folding(AddSimplifier());
                if (newnode==node)
                    newnode = inode->rewrite(AddSimplifier());
                break;
            case OP_AND:
            case OP_BV_AND:
                newnode = inode->nonassociative()->commutative()->identity((uint64_t)-1);
                if (newnode==node)
                    newnode = inode->constant_folding(AndSimplifier());
                if (newnode==node)
                    newnode = inode->rewrite(AndSimplifier());
                break;
            case OP_ASR:
                newnode = inode->additive_nesting();
                if (newnode==node)
                    newnode = inode->rewrite(AsrSimplifier());
                break;
            case OP_BV_XOR:
                newnode = inode->nonassociative()->commutative()->constant_folding(XorSimplifier());
                if (newnode==node)
                    newnode = inode->rewrite(XorSimplifier());
                break;
            case OP_CONCAT:
                newnode = inode->nonassociative()->constant_folding(ConcatSimplifier());
                if (newnode==node)
                    newnode = inode->rewrite(ConcatSimplifier());
                break;
            case OP_EQ:
                newnode = inode->commutative();
                if (newnode==node)
                    newnode = inode->rewrite(EqSimplifier());
                break;
            case OP_EXTRACT:
                newnode = inode->rewrite(ExtractSimplifier());
                break;
            case OP_INVERT:
                newnode = inode->involutary();
                if (newnode==node)
                    newnode = inode->rewrite(InvertSimplifier());
                break;
            case OP_ITE:
                newnode = inode->rewrite(IteSimplifier());
                break;
            case OP_LSSB:
                newnode = inode->rewrite(LssbSimplifier());
                break;
            case OP_MSSB:
                newnode = inode->rewrite(MssbSimplifier());
                break;
            case OP_NE:
                newnode = inode->commutative();
                break;
            case OP_NEGATE:
                newnode = inode->involutary();
                if (newnode==node)
                    newnode = inode->rewrite(NegateSimplifier());
                break;
            case OP_NOOP:
                newnode = inode->rewrite(NoopSimplifier());
                break;
            case OP_OR:
            case OP_BV_OR:
                newnode = inode->nonassociative()->commutative()->identity(0);
                if (newnode==node)
                    newnode = inode->constant_folding(OrSimplifier());
                if (newnode==node)
                    newnode = inode->rewrite(OrSimplifier());
                break;
            case OP_READ:
                // no simplifications
                break;
            case OP_ROL:
                newnode = inode->rewrite(RolSimplifier());
                break;
            case OP_ROR:
                newnode = inode->rewrite(RorSimplifier());
                break;
            case OP_SDIV:
                newnode = inode->rewrite(SdivSimplifier());
                break;
            case OP_SEXTEND:
                newnode = inode->rewrite(SextendSimplifier());
                break;
            case OP_SGE:
                newnode = inode->rewrite(SgeSimplifier());
                break;
            case OP_SGT:
                newnode = inode->rewrite(SgtSimplifier());
                break;
            case OP_SHL0:
                newnode = inode->additive_nesting();
                if (newnode==node)
                    newnode = inode->rewrite(ShlSimplifier(false));
                break;
            case OP_SHL1:
                newnode = inode->additive_nesting();
                if (newnode==node)
                    newnode = inode->rewrite(ShlSimplifier(true));
                break;
            case OP_SHR0:
                newnode = inode->additive_nesting();
                if (newnode==node)
                    newnode = inode->rewrite(ShrSimplifier(false));
                break;
            case OP_SHR1:
                newnode = inode->additive_nesting();
                if (newnode==node)
                    newnode = inode->rewrite(ShrSimplifier(true));
                break;
            case OP_SLE:
                newnode = inode->rewrite(SleSimplifier());
                break;
            case OP_SLT:
                newnode = inode->rewrite(SltSimplifier());
                break;
            case OP_SMOD:
                newnode = inode->rewrite(SmodSimplifier());
                break;
            case OP_SMUL:
                newnode = inode->nonassociative()->commutative()->constant_folding(SmulSimplifier());
                break;
            case OP_UDIV:
                newnode = inode->rewrite(UdivSimplifier());
                break;
            case OP_UEXTEND:
                newnode = inode->rewrite(UextendSimplifier());
                break;
            case OP_UGE:
                newnode = inode->rewrite(UgeSimplifier());
                break;
            case OP_UGT:
                newnode = inode->rewrite(UgtSimplifier());
                break;
            case OP_ULE:
                newnode = inode->rewrite(UleSimplifier());
                break;
            case OP_ULT:
                newnode = inode->rewrite(UltSimplifier());
                break;
            case OP_UMOD:
                newnode = inode->rewrite(UmodSimplifier());
                break;
            case OP_UMUL:
                newnode = inode->nonassociative()->commutative()->identity(1);
                if (newnode==node)
                    newnode = inode->constant_folding(UmulSimplifier());
                break;
            case OP_WRITE:
                // no simplifications
                break;
            case OP_ZEROP:
                newnode = inode->rewrite(ZeropSimplifier());
                break;
        }
        if (newnode==node)
            break;
        node = newnode;
    }
    return node;
}

/*******************************************************************************************************************************
 *                                      LeafNode methods
 *******************************************************************************************************************************/

/* class method */
LeafNodePtr
LeafNode::create_variable(size_t nbits, std::string comment, unsigned flags)
{
    ASSERT_require(nbits > 0);
    LeafNode *node = new LeafNode(comment, flags);
    node->nbits = nbits;
    node->leaf_type = BITVECTOR;
    node->name = name_counter++;
    LeafNodePtr retval(node);
    return retval;
}

/* class method */
LeafNodePtr
LeafNode::create_integer(size_t nbits, uint64_t n, std::string comment, unsigned flags)
{
    ASSERT_require(nbits > 0);
    LeafNode *node = new LeafNode(comment, flags);
    node->nbits = nbits;
    node->leaf_type = CONSTANT;
    node->bits = Sawyer::Container::BitVector(nbits).fromInteger(n);
    LeafNodePtr retval(node);
    return retval;
}

/* class method */
LeafNodePtr
LeafNode::create_constant(const Sawyer::Container::BitVector &bits, std::string comment, unsigned flags)
{
    LeafNode *node = new LeafNode(comment, flags);
    node->nbits = bits.size();
    node->leaf_type = CONSTANT;
    node->bits = bits;
    LeafNodePtr retval(node);
    return retval;
}

/* class method */
LeafNodePtr
LeafNode::create_memory(size_t addressWidth, size_t valueWidth, std::string comment, unsigned flags)
{
    ASSERT_require(addressWidth > 0);
    ASSERT_require(valueWidth > 0);
    LeafNode *node = new LeafNode(comment, flags);
    node->nbits = valueWidth;
    node->domainWidth_ = addressWidth;
    node->leaf_type = MEMORY;
    node->name = name_counter++;
    LeafNodePtr retval(node);
    return retval;
}

bool
LeafNode::is_known() const
{
    return CONSTANT==leaf_type;
}

uint64_t
LeafNode::get_value() const
{
    ASSERT_require(is_known());
    ASSERT_require(get_nbits() <= 64);
    return bits.toInteger();
}

const Sawyer::Container::BitVector&
LeafNode::get_bits() const
{
    ASSERT_require(is_known());
    return bits;
}

bool
LeafNode::is_variable() const
{
    return BITVECTOR==leaf_type;
}

bool
LeafNode::is_memory() const
{
    return MEMORY==leaf_type;
}

uint64_t
LeafNode::get_name() const
{
    ASSERT_require(is_variable() || is_memory());
    return name;
}

std::string
LeafNode::toString() const {
    if (is_known())
        return "0x" + get_bits().toHex();
    if (is_variable())
        return "v" + StringUtility::numberToString(get_name());
    if (is_memory())
        return "m" + StringUtility::numberToString(get_name());
    ASSERT_not_reachable("invalid leaf type");
    return "";
}

void
LeafNode::print(std::ostream &o, Formatter &formatter) const 
{
    print_as_signed(o, formatter);
}

void
LeafNode::print_as_signed(std::ostream &o, Formatter &formatter, bool as_signed) const
{
    bool showed_comment = false;
    if (is_known()) {
        if (bits.size() == 1) {
            if (bits.toInteger()) {
                o <<"true";
            } else {
                o <<"false";
            }
        } else if (bits.size() <= 64) {
            uint64_t ival = bits.toInteger();
            if ((32==nbits || 64==nbits) && 0!=(ival & 0xffff0000) && 0xffff0000!=(ival & 0xffff0000)) {
                // The value is probably an address, so print it like one.
                if (formatter.use_hexadecimal) {
                    o <<StringUtility::unsignedToHex2(ival, nbits);
                } else {
                    // The old behavior (which is enabled when formatter.use_hexadecimal is false) was to print only the
                    // hexadecimal format and not the decimal format, so we'll emulate that. [Robb P. Matzke 2013-12-26]
                    o <<StringUtility::addrToString(ival, nbits);
                }
            } else if (as_signed) {
                if (formatter.use_hexadecimal) {
                    o <<StringUtility::toHex2(ival, nbits); // show as signed and unsigned
                } else if (IntegerOps::signBit2(ival, nbits)) {
                    o <<(int64_t)IntegerOps::signExtend2(ival, nbits, 64);
                } else {
                    o <<ival;
                }
            } else {
                if (formatter.use_hexadecimal) {
                    o <<StringUtility::unsignedToHex2(ival, nbits); // show only as unsigned
                } else {
                    o <<ival;
                }
            }
        } else {
            // FIXME[Robb P. Matzke 2014-05-05]: we should change StringUtility functions to handle BitVector arguments
            o <<"0x" <<bits.toHex();
        }
    } else if (formatter.show_comments==Formatter::CMT_INSTEAD && !comment.empty()) {
        o <<comment;
        showed_comment = true;
    } else {
        uint64_t renamed = name;
        if (formatter.do_rename) {
            RenameMap::iterator found = formatter.renames.find(name);
            if (found==formatter.renames.end() && formatter.add_renames) {
                renamed = formatter.renames.size();
                formatter.renames.insert(std::make_pair(name, renamed));
            } else {
                renamed = found->second;
            }
        }
        switch (leaf_type) {
            case MEMORY:
                o <<"m";
                break;
            case BITVECTOR:
                o <<"v";
                break;
            case CONSTANT:
                ASSERT_not_reachable("handled above");
        }
        o <<renamed;
    }

    char bracket = '[';
    if (formatter.show_width) {
        o <<bracket <<nbits;
        bracket = ',';
    }
    if (formatter.show_flags)
        printFlags(o, get_flags(), bracket);
    if (!showed_comment && formatter.show_comments!=Formatter::CMT_SILENT && !comment.empty()) {
        o <<bracket <<comment;
        bracket = ',';
    }
    if (bracket != '[')
        o <<"]";
}

bool
LeafNode::must_equal(const TreeNodePtr &other_, SMTSolver *solver) const
{
    bool retval = false;
    LeafNodePtr other = other_->isLeafNode();
    if (this==getRawPointer(other)) {
        retval = true;
    } else if (get_flags() != other_->get_flags()) {
        retval = false;
    } else if (other==NULL) {
        // We need an SMT solver to figure this out.  This handles things like "x must_equal (not (not x))" which is true.
        if (solver) {
            TreeNodePtr assertion = InternalNode::create(1, OP_NE, sharedFromThis(), other_);
            retval = SMTSolver::SAT_NO==solver->satisfiable(assertion); // must equal if there is no soln for inequality
        }
    } else if (is_known()) {
        retval = other->is_known() && 0==bits.compare(other->bits);
    } else {
        retval = !other->is_known() && name==other->name;
    }
    return retval;
}

bool
LeafNode::may_equal(const TreeNodePtr &other_, SMTSolver *solver) const
{
    bool retval = false;
    LeafNodePtr other = other_->isLeafNode();
    if (this==getRawPointer(other)) {
        retval = true;
    } else if (other==NULL) {
        // We need an SMT solver to figure out things like "x may_equal (add y 1))", which is true.
        if (solver) {
            TreeNodePtr assertion = InternalNode::create(1, OP_EQ, sharedFromThis(), other_);
            retval = SMTSolver::SAT_YES == solver->satisfiable(assertion);
        }
    } else if (!is_known() || !other->is_known() || 0==bits.compare(other->bits)) {
        retval = true;
    }
    return retval;
}

int
LeafNode::structural_compare(const TreeNodePtr &other_) const
{
    LeafNodePtr other = other_->isLeafNode();
    if (this==getRawPointer(other)) {
        return 0;
    } else if (other==NULL) {
        return -1;                                      // leaf nodes < internal nodes
    } else if (get_nbits() != other->get_nbits()) {
        return get_nbits() < other->get_nbits() ? -1 : 1;
    } else if (get_flags() != other->get_flags()) {
        return get_flags() < other->get_flags() ? -1 : 1;
    } else if (is_known() != other->is_known()) {
        return is_known() ? -1 : 1;                     // concrete values < non-concrete
    } else if (name != other->name) {
        return name < other->name ? -1 : 1;
    }
    return 0;
}

bool
LeafNode::equivalent_to(const TreeNodePtr &other_) const
{
    bool retval = false;
    LeafNodePtr other = other_->isLeafNode();
    if (this==getRawPointer(other)) {
        retval = true;
    } else if (other && get_nbits()==other->get_nbits() && get_flags()==other->get_flags()) {
        if (is_known()) {
            retval = other->is_known() && 0==bits.compare(other->bits);
        } else {
            retval = !other->is_known() && name==other->name;
        }
    }
    return retval;
}

TreeNodePtr
LeafNode::substitute(const TreeNodePtr &from, const TreeNodePtr &to) const
{
    ASSERT_require(from!=NULL && to!=NULL && from->get_nbits()==to->get_nbits());
    if (equivalent_to(from))
        return to;
    return sharedFromThis();
}

VisitAction
LeafNode::depth_first_traversal(Visitor &v) const
{
    TreeNodePtr self = sharedFromThis();
    VisitAction retval = v.preVisit(self);
    if (TERMINATE!=retval)
        retval = v.postVisit(self);
    return retval;
}

std::ostream&
operator<<(std::ostream &o, const TreeNode &node) {
    Formatter fmt;
    node.print(o, fmt);
    return o;
}

std::ostream&
operator<<(std::ostream &o, const TreeNode::WithFormatter &w)
{
    w.print(o);
    return o;
}

std::vector<TreeNodePtr>
findCommonSubexpressions(const std::vector<TreeNodePtr> &exprs) {
    return findCommonSubexpressions(exprs.begin(), exprs.end());
}

} // namespace
} // namespace
} // namespace
