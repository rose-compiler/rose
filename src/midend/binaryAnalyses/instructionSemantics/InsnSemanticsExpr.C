#include "rose.h"
#include "InsnSemanticsExpr.h"
#include "SMTSolver.h"
#include "stringify.h"
#include "integerOps.h"
#include "Combinatorics.h"

#ifdef _MSC_VER
#define xor ^
#endif

namespace InsnSemanticsExpr {

uint64_t
LeafNode::name_counter = 0;


const char *
to_str(Operator o)
{
    static char buf[64];
    std::string s = stringifyInsnSemanticsExprOperator(o, "OP_");
    assert(s.size()<sizeof buf);
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
        void operator()(const TreeNodePtr &node) {
            LeafNodePtr l_node = node->isLeafNode();
            if (l_node && !l_node->is_known())
                vars.insert(l_node);
        }
    } t1;
    depth_first_visit(&t1);
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

/*******************************************************************************************************************************
 *                                      InternalNode methods
 *******************************************************************************************************************************/

void
InternalNode::add_child(const TreeNodePtr &child)
{
    ROSE_ASSERT(child!=0);
    children.push_back(child);
}

void
InternalNode::print(std::ostream &o, Formatter &formatter) const
{
    o <<"(" <<to_str(op) <<"[" <<nbits;
    if (formatter.show_comments!=Formatter::CMT_SILENT && !comment.empty())
        o <<"," <<comment;
    o <<"]";
    for (size_t i=0; i<children.size(); i++) {
        o <<" ";
        children[i]->print(o, formatter);
    }
    o <<")";
}

bool
InternalNode::must_equal(const TreeNodePtr &other_, SMTSolver *solver/*NULL*/) const
{
    bool retval = false;
    if (this==other_.get()) {
        retval = true;
    } else if (equivalent_to(other_)) {
        // This is probably faster than using an SMT solver. It also serves as the naive approach when an SMT solver
        // is not available.
        retval = true;
    } else if (solver) {
        TreeNodePtr assertion = InternalNode::create(1, OP_NE, shared_from_this(), other_);
        retval = SMTSolver::SAT_NO==solver->satisfiable(assertion); /*equal if there is no solution for inequality*/
    }
    return retval;
}

bool
InternalNode::may_equal(const TreeNodePtr &other, SMTSolver *solver/*NULL*/) const
{
    bool retval = false;
    if (this==other.get()) {
        return true;
    } else if (equivalent_to(other)) {
        // This is probably faster than using an SMT solver.  It also serves as the naive approach when an SMT solver
        // is not available.
        retval = true;
    } else if (solver) {
        TreeNodePtr assertion = InternalNode::create(1, OP_EQ, shared_from_this(), other);
        retval = SMTSolver::SAT_YES==solver->satisfiable(assertion);
    }
    return retval;
}

bool
InternalNode::equivalent_to(const TreeNodePtr &other_) const
{
    bool retval = false;
    InternalNodePtr other = other_->isInternalNode();
    if (this==other.get()) {
        retval = true;
    } else if (other==NULL || get_nbits()!=other->get_nbits()) {
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
                assert(hashval==other->hashval);
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
    assert(from!=NULL && to!=NULL && from->get_nbits()==to->get_nbits());
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
        return shared_from_this();
    return InternalNode::create(get_nbits(), get_operator(), newnodes, get_comment());
}

void
InternalNode::depth_first_visit(Visitor *v) const
{
    assert(v!=NULL);
    for (std::vector<TreeNodePtr>::const_iterator ci=children.begin(); ci!=children.end(); ++ci)
        (*ci)->depth_first_visit(v);
    (*v)(shared_from_this());
}

InternalNodePtr
InternalNode::nonassociative() const
{
    InternalNode *retval = new InternalNode(get_nbits(), op, get_comment());
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
            retval->add_child(child);
        }
    }
    if (modified)
        return InternalNodePtr(retval);
    delete retval;
    return shared_from_this()->isInternalNode();
}

InternalNodePtr
InternalNode::commutative() const
{
    InternalNode *retval = new InternalNode(get_nbits(), op, get_comment());
    TreeNodes internal, leaf;
    bool modified = false;
    for (TreeNodes::const_iterator ci=children.begin(); ci!=children.end(); ++ci) {
        if (InternalNodePtr ichild = (*ci)->isInternalNode()) {
            internal.push_back(ichild);
            modified = !leaf.empty();
        } else if (LeafNodePtr lchild = (*ci)->isLeafNode()) {
            leaf.push_back(lchild);
        } else {
            assert(!"unhandled expression node type");
            abort();
        }
    }
    if (!modified) {
        delete retval;
        return shared_from_this()->isInternalNode();
    }
    retval->children.insert(retval->children.end(), internal.begin(), internal.end());
    retval->children.insert(retval->children.end(), leaf.begin(), leaf.end());
    return InternalNodePtr(retval);
}

TreeNodePtr
InternalNode::involutary() const
{
    if (InternalNodePtr inode = isInternalNode()) {
        if (1==inode->size()) {
            if (InternalNodePtr sub1 = inode->child(0)->isInternalNode()) {
                if (sub1->get_operator() == inode->get_operator() && 1==sub1->size()) {
                    return sub1->child(0);
                }
            }
        }
    }
    return shared_from_this();
}

TreeNodePtr
InternalNode::identity(uint64_t ident) const
{
    TreeNodes args;
    bool modified = false;
    for (TreeNodes::const_iterator ci=children.begin(); ci!=children.end(); ++ci) {
        LeafNodePtr leaf = (*ci)->isLeafNode();
        if (leaf && leaf->is_known() && leaf->get_value()==(ident & IntegerOps::genMask<uint64_t>(leaf->get_nbits()))) {
            // skip this arg
            modified = true;
        } else {
            args.push_back(*ci);
        }
    }
    if (!modified)
        return shared_from_this();
    if (args.empty())
        return LeafNode::create_integer(get_nbits(), ident, get_comment());
    if (1==args.size())
        return args.front();
    InternalNode *retval = new InternalNode(get_nbits(), get_operator(), get_comment());
    retval->children.insert(retval->children.end(), args.begin(), args.end());
    return InternalNodePtr(retval);
}

TreeNodePtr
InternalNode::unaryNoOp() const
{
    return 1==size() ? child(0) : shared_from_this();
}

TreeNodePtr
InternalNode::rewrite(const Simplifier &simplifier) const
{
    if (TreeNodePtr simplified = simplifier.rewrite(this))
        return simplified;
    return shared_from_this();
}

TreeNodePtr
InternalNode::constant_folding(const Simplifier &simplifier) const
{
    InternalNode *retval = new InternalNode(get_nbits(), op, get_comment());
    bool modified = false;
    TreeNodes::const_iterator ci1 = children.begin();
    while (ci1!=children.end()) {
        TreeNodes::const_iterator ci2 = ci1;
        LeafNodePtr leaf;
        while (ci2!=children.end() && (leaf=(*ci2)->isLeafNode()) && leaf->is_known()) ++ci2;
        if (ci1==ci2 || ci1+1==ci2) {                           // arg is not a constant, or we had only one constant by itself
            retval->add_child(*ci1);
            ++ci1;
        } else if (TreeNodePtr folded = simplifier.fold(ci1, ci2)) { // able to fold all these constants into a new node
            retval->add_child(folded);
            modified = true;
            ci1 = ci2;
        } else {                                                // multiple constants, but unable to fold
            retval->children.insert(retval->children.end(), ci1, ci2);
            ci1 = ci2;
        }
    }
    if (!modified) {
        delete retval;
        return shared_from_this()->isInternalNode();
    }
    if (1==retval->size()) {
        TreeNodePtr tmp = TreeNodePtr(retval->child(0)); // need to hold this pointer while we delete
        delete retval;
        return tmp;
    }
    return InternalNodePtr(retval);
}

TreeNodePtr
AddSimplifier::fold(TreeNodes::const_iterator begin, TreeNodes::const_iterator end) const
{
    size_t nbits = (*begin)->get_nbits();
    uint64_t result = 0;
    for (/*void*/; begin!=end; ++begin)
        result += (*begin)->isLeafNode()->get_value();
    return LeafNode::create_integer(nbits, result);
}

TreeNodePtr
AddSimplifier::rewrite(const InternalNode *inode) const
{
    struct are_duals {
        bool operator()(TreeNodePtr a, TreeNodePtr b) {
            if (a==NULL || b==NULL)
                return false;
            InternalNodePtr negate_a = a->isInternalNode();
            if (negate_a!=NULL && OP_NEGATE!=negate_a->get_operator())
                negate_a.reset();
            InternalNodePtr negate_b = b->isInternalNode();
            if (negate_b!=NULL && OP_NEGATE!=negate_b->get_operator())
                negate_b.reset();
            if ((negate_a==NULL) xor (negate_b==NULL)) {
                if (negate_a==NULL)
                    std::swap(negate_a, negate_b);
                assert(1==negate_a->size());
                return negate_a->child(0)->equivalent_to(b);
            }
            return false;
        }
    };
    
    // Arguments that are negated cancel out similar arguments that are not negated
    TreeNodes children = inode->get_children();
    for (size_t i=0; i<children.size(); ++i) {
        for (size_t j=i+1; j<children.size(); ++j) {
            if (are_duals()(children[i], children[j])) {
                children[i].reset();
                children[j].reset();
                break;
            }
        }
    }
    children.erase(std::remove(children.begin(), children.end(), TreeNodePtr()), children.end());

    // Create a new node if we removed any children. */
    if (children.size()!=inode->size()) {
        if (children.empty())
            return LeafNode::create_integer(inode->get_nbits(), 0, inode->get_comment());
        if (children.size()==1)
            return children[0];
        return InternalNode::create(inode->get_nbits(), OP_ADD, children, inode->get_comment());
    }
                  
    return TreeNodePtr();
}

TreeNodePtr
AndSimplifier::fold(TreeNodes::const_iterator begin, TreeNodes::const_iterator end) const
{
    size_t nbits = (*begin)->get_nbits();
    uint64_t result = (*begin)->isLeafNode()->get_value();
    for (++begin; begin!=end; ++begin)
        result &= (*begin)->isLeafNode()->get_value();
    return LeafNode::create_integer(nbits, result);
}

TreeNodePtr
AndSimplifier::rewrite(const InternalNode *inode) const
{
    // Result is zero if any argument is zero
    for (size_t i=0; i<inode->size(); ++i) {
        LeafNodePtr child = inode->child(i)->isLeafNode();
        if (child && child->is_known() && 0==child->get_value())
            return LeafNode::create_integer(inode->get_nbits(), 0, inode->get_comment());
    }
    return TreeNodePtr();
}

TreeNodePtr
OrSimplifier::fold(TreeNodes::const_iterator begin, TreeNodes::const_iterator end) const
{
    size_t nbits = (*begin)->get_nbits();
    uint64_t result = 0;
    for (/*void*/; begin!=end; ++begin)
        result |= (*begin)->isLeafNode()->get_value();
    return LeafNode::create_integer(nbits, result);
}

TreeNodePtr
OrSimplifier::rewrite(const InternalNode *inode) const
{
    // Result has all bits set if any argument has all bits set
    uint64_t allset = IntegerOps::genMask<uint64_t>(inode->get_nbits());
    for (size_t i=0; i<inode->size(); ++i) {
        LeafNodePtr child = inode->child(i)->isLeafNode();
        if (child && child->is_known() && child->get_value()==allset)
            return LeafNode::create_integer(inode->get_nbits(), allset, inode->get_comment());
    }
    return TreeNodePtr();
}

TreeNodePtr
XorSimplifier::fold(TreeNodes::const_iterator begin, TreeNodes::const_iterator end) const
{
    size_t nbits = (*begin)->get_nbits();
    uint64_t result = (*begin)->isLeafNode()->get_value();
    for (++begin; begin!=end; ++begin)
        result ^= (*begin)->isLeafNode()->get_value();
    return LeafNode::create_integer(nbits, result);
}

TreeNodePtr
XorSimplifier::rewrite(const InternalNode *inode) const
{
    SMTSolver *solver = NULL;   // FIXME

    // If any pairs of arguments are equal, then they don't contribute to the final answer.
    std::vector<bool> removed(inode->size(), false);
    bool modified = false;
    for (size_t i=0; i<inode->size(); ++i) {
        if (removed[i])
            continue;
        for (size_t j=i+1; j<inode->size(); ++j) {
            if (!removed[j] && inode->child(i)->must_equal(inode->child(j), solver)) {
                removed[i] = removed[j] = modified = true;
                break;
            }
        }
    }
    if (!modified)
        return TreeNodePtr();
    TreeNodes newargs;
    for (size_t i=0; i<inode->size(); ++i) {
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
    size_t nbits = (*begin)->get_nbits();
    int64_t result = (*begin)->isLeafNode()->get_value();
    for (++begin; begin!=end; ++begin) {
        LeafNodePtr leaf = (*begin)->isLeafNode();
        result *= (int64_t)leaf->get_value();
        nbits += leaf->get_nbits();
    }
    assert(nbits<=8*sizeof result);
    return LeafNode::create_integer(nbits, result);
}

TreeNodePtr
UmulSimplifier::fold(TreeNodes::const_iterator begin, TreeNodes::const_iterator end) const
{
    size_t nbits = (*begin)->get_nbits();
    uint64_t result = (*begin)->isLeafNode()->get_value();
    for (++begin; begin!=end; ++begin) {
        LeafNodePtr leaf = (*begin)->isLeafNode();
        result *= leaf->get_value();
        nbits += leaf->get_nbits();
    }
    assert(nbits>0 && nbits<=8*sizeof result);
    return LeafNode::create_integer(nbits, result);
}

TreeNodePtr
ConcatSimplifier::fold(TreeNodes::const_iterator begin, TreeNodes::const_iterator end) const
{
    uint64_t result = 0;

    // first arg is high-order bits. Although this is nice to look at, it makes the operation a bit more difficult.
    size_t nbits = 0;
    for (TreeNodes::const_iterator ti=begin; ti!=end; ++ti)
        nbits += (*ti)->get_nbits();

    for (size_t sa=nbits; begin!=end; ++begin) {
        LeafNodePtr leaf = (*begin)->isLeafNode();
        sa -= leaf->get_nbits();
        result |= IntegerOps::shiftLeft2(leaf->get_value(), sa);
    }

    assert(nbits>0 && nbits<=8*sizeof result);
    return LeafNode::create_integer(nbits, result);
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
    for (size_t i=inode->size(); i>0; --i) { // process args in little endian order
        InternalNodePtr extract = inode->child(i-1)->isInternalNode();
        if (!extract || OP_EXTRACT!=extract->get_operator())
            break;
        LeafNodePtr from_node = extract->child(0)->isLeafNode();
        if (!from_node || !from_node->is_known() || from_node->get_value()!=offset ||
            extract->child(2)->get_nbits()!=inode->get_nbits())
            break;
        if (inode->size()==i) {
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
    size_t from = from_node && from_node->is_known() ? from_node->get_value() : 0;
    size_t to = to_node && to_node->is_known() ? to_node->get_value() : 0;

    // If limits are backward or extend beyond the operand size, don't simplify
    if (from_node && to_node && from_node->is_known() && to_node->is_known() && (from>=to || to>operand->get_nbits()))
        return TreeNodePtr();

    // Constant folding
    if (from_node && to_node && from_node->is_known() && to_node->is_known() &&
        operand->isLeafNode() && operand->isLeafNode()->is_known()) {
        uint64_t val = operand->isLeafNode()->get_value();
        val = IntegerOps::shiftRightLogical2(val, from) & IntegerOps::genMask<uint64_t>(to-from);
        return LeafNode::create_integer(to-from, val, inode->get_comment());
    }

    // Extracting the whole thing is a no-op
    if (from_node && to_node && from_node->is_known() && from==0 && to==operand->get_nbits())
        return operand;

    // If the operand is a concat operation and one of the concat operands corresponds to the part we're extracting, then we
    // can return just that part.
    InternalNodePtr ioperand = operand->isInternalNode();
    if (ioperand && OP_CONCAT==ioperand->get_operator()) {
        size_t offset = ioperand->get_nbits(); // most significant bits are in concat's first argument
        for (size_t i=0; i<ioperand->size() && offset>=from; ++i) {
            offset -= ioperand->child(i)->get_nbits();
            if (offset==from && ioperand->child(i)->get_nbits()==to-from)
                return ioperand->child(i);
        }
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

    return TreeNodePtr();
}

TreeNodePtr
AsrSimplifier::rewrite(const InternalNode *inode) const
{
    // Constant folding
    LeafNodePtr shift_node   = inode->child(0)->isLeafNode();
    LeafNodePtr operand_node = inode->child(1)->isLeafNode();
    if (shift_node==NULL || operand_node==NULL || !shift_node->is_known() || !operand_node->is_known())
        return TreeNodePtr();
    size_t sa = shift_node->get_value();
    uint64_t val = operand_node->get_value();
    val = IntegerOps::shiftRightArithmetic2(val, sa, inode->get_nbits());
    return LeafNode::create_integer(inode->get_nbits(), val, inode->get_comment());
}

TreeNodePtr
InvertSimplifier::rewrite(const InternalNode *inode) const
{
    // Constant folding
    LeafNodePtr operand_node = inode->child(0)->isLeafNode();
    if (operand_node==NULL || !operand_node->is_known())
        return TreeNodePtr();
    uint64_t val = ~(operand_node->get_value());
    return LeafNode::create_integer(inode->get_nbits(), val, inode->get_comment());
}

TreeNodePtr
NegateSimplifier::rewrite(const InternalNode *inode) const
{
    // Constant folding
    LeafNodePtr operand_node = inode->child(0)->isLeafNode();
    if (operand_node==NULL || !operand_node->is_known())
        return TreeNodePtr();
    uint64_t val = ~(operand_node->get_value()) + 1;
    return LeafNode::create_integer(inode->get_nbits(), val, inode->get_comment());
}

TreeNodePtr
IteSimplifier::rewrite(const InternalNode *inode) const
{
    // Is the condition known?
    LeafNodePtr cond_node = inode->child(0)->isLeafNode();
    if (cond_node!=NULL && cond_node->is_known()) {
        assert(1==cond_node->get_nbits());
        return cond_node->get_value() ? inode->child(1) : inode->child(2);
    }

    // Are both operands the same? Then the condition doesn't matter
    if (inode->child(0)->equivalent_to(inode->child(1)))
        return inode->child(0);

    return TreeNodePtr();
}

TreeNodePtr
NoopSimplifier::rewrite(const InternalNode *inode) const
{
    if (1==inode->size())
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
        uint64_t val = IntegerOps::rotateLeft2(val_leaf->get_value(), sa_leaf->get_value(), inode->get_nbits());
        return LeafNode::create_integer(inode->get_nbits(), val, inode->get_comment());
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
        uint64_t val = IntegerOps::rotateRight2(val_leaf->get_value(), sa_leaf->get_value(), inode->get_nbits());
        return LeafNode::create_integer(inode->get_nbits(), val, inode->get_comment());
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
    if (val_leaf && val_leaf->is_known())
        return LeafNode::create_integer(newsize, val_leaf->get_value(), inode->get_comment());

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
        uint64_t val = IntegerOps::signExtend2(val_leaf->get_value(), oldsize, newsize);
        return LeafNode::create_integer(newsize, val, inode->get_comment());
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
SgeSimplifier::rewrite(const InternalNode *inode) const
{
    // Constant folding
    LeafNodePtr a_leaf = inode->child(0)->isLeafNode();
    LeafNodePtr b_leaf = inode->child(1)->isLeafNode();
    if (a_leaf && b_leaf && a_leaf->is_known() && b_leaf->is_known()) {
        int64_t a = IntegerOps::signExtend2(a_leaf->get_value(), a_leaf->get_nbits(), 8*sizeof(int64_t));
        int64_t b = IntegerOps::signExtend2(b_leaf->get_value(), b_leaf->get_nbits(), 8*sizeof(int64_t));
        return LeafNode::create_integer(1, a>=b ? 1 : 0, inode->get_comment());
    }

    return TreeNodePtr();
}

TreeNodePtr
SgtSimplifier::rewrite(const InternalNode *inode) const
{
    // Constant folding
    LeafNodePtr a_leaf = inode->child(0)->isLeafNode();
    LeafNodePtr b_leaf = inode->child(1)->isLeafNode();
    if (a_leaf && b_leaf && a_leaf->is_known() && b_leaf->is_known()) {
        int64_t a = IntegerOps::signExtend2(a_leaf->get_value(), a_leaf->get_nbits(), 8*sizeof(int64_t));
        int64_t b = IntegerOps::signExtend2(b_leaf->get_value(), b_leaf->get_nbits(), 8*sizeof(int64_t));
        return LeafNode::create_integer(1, a>b ? 1 : 0, inode->get_comment());
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
        int64_t a = IntegerOps::signExtend2(a_leaf->get_value(), a_leaf->get_nbits(), 8*sizeof(int64_t));
        int64_t b = IntegerOps::signExtend2(b_leaf->get_value(), b_leaf->get_nbits(), 8*sizeof(int64_t));
        return LeafNode::create_integer(1, a<=b ? 1 : 0, inode->get_comment());
    }

    return TreeNodePtr();
}

TreeNodePtr
SltSimplifier::rewrite(const InternalNode *inode) const
{
    // Constant folding
    LeafNodePtr a_leaf = inode->child(0)->isLeafNode();
    LeafNodePtr b_leaf = inode->child(1)->isLeafNode();
    if (a_leaf && b_leaf && a_leaf->is_known() && b_leaf->is_known()) {
        int64_t a = IntegerOps::signExtend2(a_leaf->get_value(), a_leaf->get_nbits(), 8*sizeof(int64_t));
        int64_t b = IntegerOps::signExtend2(b_leaf->get_value(), b_leaf->get_nbits(), 8*sizeof(int64_t));
        return LeafNode::create_integer(1, a<b ? 1 : 0, inode->get_comment());
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
        uint64_t a = a_leaf->get_value();
        uint64_t b = b_leaf->get_value();
        return LeafNode::create_integer(1, a>=b ? 1 : 0, inode->get_comment());
    }

    return TreeNodePtr();
}

TreeNodePtr
UgtSimplifier::rewrite(const InternalNode *inode) const
{
    // Constant folding
    LeafNodePtr a_leaf = inode->child(0)->isLeafNode();
    LeafNodePtr b_leaf = inode->child(1)->isLeafNode();
    if (a_leaf && b_leaf && a_leaf->is_known() && b_leaf->is_known()) {
        uint64_t a = a_leaf->get_value();
        uint64_t b = b_leaf->get_value();
        return LeafNode::create_integer(1, a>b ? 1 : 0, inode->get_comment());
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
        uint64_t a = a_leaf->get_value();
        uint64_t b = b_leaf->get_value();
        return LeafNode::create_integer(1, a<=b ? 1 : 0, inode->get_comment());
    }

    return TreeNodePtr();
}

TreeNodePtr
UltSimplifier::rewrite(const InternalNode *inode) const
{
    // Constant folding
    LeafNodePtr a_leaf = inode->child(0)->isLeafNode();
    LeafNodePtr b_leaf = inode->child(1)->isLeafNode();
    if (a_leaf && b_leaf && a_leaf->is_known() && b_leaf->is_known()) {
        uint64_t a = a_leaf->get_value();
        uint64_t b = b_leaf->get_value();
        return LeafNode::create_integer(1, a<b ? 1 : 0, inode->get_comment());
    }

    return TreeNodePtr();
}

TreeNodePtr
ZeropSimplifier::rewrite(const InternalNode *inode) const
{
    // Constant folding
    LeafNodePtr a_leaf = inode->child(0)->isLeafNode();
    if (a_leaf && a_leaf->is_known())
        return LeafNode::create_integer(1, 0==a_leaf->get_value() ? 1 : 0, inode->get_comment());

    return TreeNodePtr();
}

TreeNodePtr
SdivSimplifier::rewrite(const InternalNode *inode) const
{
    // Constant folding
    LeafNodePtr a_leaf = inode->child(0)->isLeafNode();
    LeafNodePtr b_leaf = inode->child(1)->isLeafNode();
    if (a_leaf && b_leaf && a_leaf->is_known() && b_leaf->is_known() && b_leaf->get_value()!=0) {
        int64_t a = IntegerOps::signExtend2(a_leaf->get_value(), a_leaf->get_nbits(), 8*sizeof(int8_t));
        int64_t b = IntegerOps::signExtend2(b_leaf->get_value(), b_leaf->get_nbits(), 8*sizeof(int8_t));
        return LeafNode::create_integer(a_leaf->get_nbits(), a/b, inode->get_comment());
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
        int64_t a = IntegerOps::signExtend2(a_leaf->get_value(), a_leaf->get_nbits(), 8*sizeof(int8_t));
        int64_t b = IntegerOps::signExtend2(b_leaf->get_value(), b_leaf->get_nbits(), 8*sizeof(int8_t));
        return LeafNode::create_integer(b_leaf->get_nbits(), a%b, inode->get_comment());
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
        uint64_t a = a_leaf->get_value();
        uint64_t b = b_leaf->get_value();
        return LeafNode::create_integer(a_leaf->get_nbits(), a/b, inode->get_comment());
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
        uint64_t a = a_leaf->get_value();
        uint64_t b = b_leaf->get_value();
        return LeafNode::create_integer(b_leaf->get_nbits(), a%b, inode->get_comment());
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
        uint64_t val = val_leaf->get_value();
        val = IntegerOps::shiftLeft2(val, sa, inode->get_nbits());
        if (newbits)
            val |= IntegerOps::genMask<uint64_t>(sa);
        return LeafNode::create_integer(inode->get_nbits(), val, inode->get_comment());
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
        uint64_t val = newbits ? -1 : 0;
        return LeafNode::create_integer(inode->get_nbits(), val, inode->get_comment());
    }

    // If the shift amount is zero then this is a no-op
    if (sa_leaf && sa_leaf->is_known() && sa_leaf->get_value()==0)
        return inode->child(1);

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
        uint64_t val = val_leaf->get_value();
        val = IntegerOps::shiftRightLogical2(val, sa, inode->get_nbits());
        if (newbits)
            val |= IntegerOps::genMask<uint64_t>(sa);
        return LeafNode::create_integer(inode->get_nbits(), val, inode->get_comment());
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
        uint64_t val = newbits ? -1 : 0;
        return LeafNode::create_integer(inode->get_nbits(), val, inode->get_comment());
    }

    // If the shift amount is zero then this is a no-op
    if (sa_leaf && sa_leaf->is_known() && sa_leaf->get_value()==0)
        return inode->child(1);

    return TreeNodePtr();
}

TreeNodePtr
LssbSimplifier::rewrite(const InternalNode *inode) const
{
    // Constant folding
    LeafNodePtr a_leaf = inode->child(0)->isLeafNode();
    if (a_leaf && a_leaf->is_known()) {
        uint64_t a = a_leaf->get_value();
        for (size_t i=0; i<a_leaf->get_nbits(); ++i) {
            if (a & IntegerOps::shl1<uint64_t>(i))
                return LeafNode::create_integer(inode->get_nbits(), i, inode->get_comment());
        }
        return LeafNode::create_integer(inode->get_nbits(), 0, inode->get_comment());
    }

    return TreeNodePtr();
}

TreeNodePtr
MssbSimplifier::rewrite(const InternalNode *inode) const
{
    // Constant folding
    LeafNodePtr a_leaf = inode->child(0)->isLeafNode();
    if (a_leaf && a_leaf->is_known()) {
        uint64_t a = a_leaf->get_value();
        for (size_t i=a_leaf->get_nbits(); i>0; --i) {
            if (a & IntegerOps::shl1<uint64_t>(i-1))
                return LeafNode::create_integer(inode->get_nbits(), i-1, inode->get_comment());
        }
        return LeafNode::create_integer(inode->get_nbits(), 0, inode->get_comment());
    }

    return TreeNodePtr();
}

TreeNodePtr
InternalNode::simplifyTop() const
{
    TreeNodePtr node = shared_from_this();
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
                newnode = inode->rewrite(ShlSimplifier(false));
                break;
            case OP_SHL1:
                newnode = inode->rewrite(ShlSimplifier(true));
                break;
            case OP_SHR0:
                newnode = inode->rewrite(ShrSimplifier(false));
                break;
            case OP_SHR1:
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
                newnode = inode->nonassociative()->commutative()->constant_folding(UmulSimplifier());
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
LeafNode::create_variable(size_t nbits, std::string comment)
{
    LeafNode *node = new LeafNode(comment);
    node->nbits = nbits;
    node->leaf_type = BITVECTOR;
    node->name = name_counter++;
    LeafNodePtr retval(node);
    return retval;
}

/* class method */
LeafNodePtr
LeafNode::create_integer(size_t nbits, uint64_t n, std::string comment)
{
    LeafNode *node = new LeafNode(comment);
    node->nbits = nbits;
    node->leaf_type = CONSTANT;
    node->ival = n & IntegerOps::genMask<uint64_t>(nbits);
    LeafNodePtr retval(node);
    return retval;
}

/* class method */
LeafNodePtr
LeafNode::create_memory(size_t nbits, std::string comment)
{
    LeafNode *node = new LeafNode(comment);
    node->nbits = nbits;
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
    assert(is_known());
    return ival;
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
    assert(is_variable() || is_memory());
    return name;
}

void
LeafNode::print(std::ostream &o, Formatter &formatter) const
{
    bool showed_comment = false;
    if (is_known()) {
        if ((32==nbits || 64==nbits) && 0!=(ival & 0xffff0000) && 0xffff0000!=(ival & 0xffff0000)) {
            // probably an address, so print in hexadecimal.  The comparison with 0 is for positive values, and the comparison
            // with 0xffff0000 is for negative values.
            o <<StringUtility::addrToString(ival);
        } else if (nbits>1 && (ival & ((uint64_t)1<<(nbits-1)))) {
            uint64_t sign_extended = ival | ~(((uint64_t)1<<nbits)-1);
            o <<(int64_t)sign_extended;
        } else {
            o <<ival;
        }
    } else if (formatter.show_comments==Formatter::CMT_INSTEAD && !comment.empty()) {
        o <<comment;
        showed_comment = true;
    } else {
        uint64_t renamed = name;
        if (formatter.do_rename) {
            Formatter::RenameMap::iterator found = formatter.renames.find(name);
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
                assert(!"handled above");
                abort();
        }
        o <<renamed;
    }
    o <<"[" <<nbits;
    if (!showed_comment && formatter.show_comments!=Formatter::CMT_SILENT && !comment.empty())
        o <<"," <<comment;
    o <<"]";
}

bool
LeafNode::must_equal(const TreeNodePtr &other_, SMTSolver *solver) const
{
    bool retval = false;
    LeafNodePtr other = other_->isLeafNode();
    if (this==other.get()) {
        retval = true;
    } else if (other==NULL) {
        // We need an SMT solver to figure this out.  This handles things like "x must_equal (not (not x))" which is true.
        if (solver) {
            TreeNodePtr assertion = InternalNode::create(1, OP_NE, shared_from_this(), other_);
            retval = SMTSolver::SAT_NO==solver->satisfiable(assertion); // must equal if there is no soln for inequality
        }
    } else if (is_known()) {
        retval = other->is_known() && ival==other->ival;
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
    if (this==other.get()) {
        retval = true;
    } else if (other==NULL) {
        // We need an SMT solver to figure out things like "x may_equal (add y 1))", which is true.
        if (solver) {
            TreeNodePtr assertion = InternalNode::create(1, OP_EQ, shared_from_this(), other_);
            retval = SMTSolver::SAT_YES == solver->satisfiable(assertion);
        }
    } else if (!is_known() || !other->is_known() || ival==other->ival) {
        retval = true;
    }
    return retval;
}

bool
LeafNode::equivalent_to(const TreeNodePtr &other_) const
{
    bool retval = false;
    LeafNodePtr other = other_->isLeafNode();
    if (this==other.get()) {
        retval = true;
    } else if (other && get_nbits()==other->get_nbits()) {
        if (is_known()) {
            retval = other->is_known() && ival==other->ival;
        } else {
            retval = !other->is_known() && name==other->name;
        }
    }
    return retval;
}

TreeNodePtr
LeafNode::substitute(const TreeNodePtr &from, const TreeNodePtr &to) const
{
    assert(from!=NULL && to!=NULL && from->get_nbits()==to->get_nbits());
    if (equivalent_to(from))
        return to;
    return shared_from_this();
}

void
LeafNode::depth_first_visit(Visitor *v) const
{
    assert(v!=NULL);
    (*v)(shared_from_this());
}

std::ostream&
operator<<(std::ostream &o, const TreeNode &node) {
    node.print(o);
    return o;
}

std::ostream&
operator<<(std::ostream &o, const TreeNode::WithFormatter &w)
{
    w.print(o);
    return o;
}


} // namespace
