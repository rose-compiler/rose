#include "rose.h"
#include "InsnSemanticsExpr.h"
#include "SMTSolver.h"
#include "stringify.h"

uint64_t
InsnSemanticsExpr::LeafNode::name_counter = 0;


const char *
InsnSemanticsExpr::to_str(Operator o)
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

std::set<InsnSemanticsExpr::LeafNodePtr>
InsnSemanticsExpr::TreeNode::get_variables() const
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

void
InsnSemanticsExpr::InternalNode::add_child(const TreeNodePtr &child)
{
    ROSE_ASSERT(child!=0);
    children.push_back(child);
}

void
InsnSemanticsExpr::InternalNode::print(std::ostream &o, RenameMap *rmap/*NULL*/) const
{
    o <<"(" <<to_str(op) <<"[" <<nbits;
    if (!comment.empty())
        o <<"," <<comment;
    o <<"]";
    for (size_t i=0; i<children.size(); i++) {
        o <<" ";
        children[i]->print(o, rmap);
    }
    o <<")";
}

bool
InsnSemanticsExpr::InternalNode::equal_to(const TreeNodePtr &other_, SMTSolver *solver/*NULL*/) const
{
    bool retval = false;
    if (solver) {
        InternalNodePtr assertion = InternalNode::create(1, OP_NE, shared_from_this(), other_);
        retval = SMTSolver::SAT_NO==solver->satisfiable(assertion); /*equal if there is no solution for inequality*/
    } else {
        /* The naive approach uses structural equality */
        InternalNodePtr other = other_->isInternalNode();
        if (this==other.get()) {
            retval = true;
        } else if (other && op==other->op && children.size()==other->children.size()) {
            retval = true;
            for (size_t i=0; i<children.size() && retval; ++i)
                retval = children[i]->equal_to(other->children[i], NULL);
        }
    }
    return retval;
}

bool
InsnSemanticsExpr::InternalNode::equivalent_to(const TreeNodePtr &other_) const
{
    bool retval = false;
    InternalNodePtr other = other_->isInternalNode();
    if (this==other.get()) {
        retval = true;
    } else if (other && get_nbits()==other->get_nbits() && op==other->op && children.size()==other->children.size()) {
        retval = true;
        for (size_t i=0; i<children.size() && retval; ++i)
            retval = children[i]->equivalent_to(other->children[i]);
    }
    return retval;
}

void
InsnSemanticsExpr::InternalNode::depth_first_visit(Visitor *v) const
{
    assert(v!=NULL);
    for (std::vector<TreeNodePtr>::const_iterator ci=children.begin(); ci!=children.end(); ++ci)
        (*ci)->depth_first_visit(v);
    (*v)(shared_from_this());
}

        


/* class method */
InsnSemanticsExpr::LeafNodePtr
InsnSemanticsExpr::LeafNode::create_variable(size_t nbits, std::string comment)
{
    LeafNode *node = new LeafNode(comment);
    node->nbits = nbits;
    node->leaf_type = BITVECTOR;
    node->name = name_counter++;
    LeafNodePtr retval(node);
    return retval;
}

/* class method */
InsnSemanticsExpr::LeafNodePtr
InsnSemanticsExpr::LeafNode::create_integer(size_t nbits, uint64_t n, std::string comment)
{
    LeafNode *node = new LeafNode(comment);
    node->nbits = nbits;
    node->leaf_type = CONSTANT;
    node->ival = n & (((uint64_t)1<<nbits)-1);
    LeafNodePtr retval(node);
    return retval;
}

/* class method */
InsnSemanticsExpr::LeafNodePtr
InsnSemanticsExpr::LeafNode::create_memory(size_t nbits, std::string comment)
{
    LeafNode *node = new LeafNode(comment);
    node->nbits = nbits;
    node->leaf_type = MEMORY;
    node->name = name_counter++;
    LeafNodePtr retval(node);
    return retval;
}

bool
InsnSemanticsExpr::LeafNode::is_known() const
{
    return CONSTANT==leaf_type;
}

uint64_t
InsnSemanticsExpr::LeafNode::get_value() const
{
    assert(is_known());
    return ival;
}

bool
InsnSemanticsExpr::LeafNode::is_variable() const
{
    return BITVECTOR==leaf_type;
}

bool
InsnSemanticsExpr::LeafNode::is_memory() const
{
    return MEMORY==leaf_type;
}

uint64_t
InsnSemanticsExpr::LeafNode::get_name() const
{
    assert(is_variable() || is_memory());
    return name;
}

void
InsnSemanticsExpr::LeafNode::print(std::ostream &o, RenameMap *rmap/*NULL*/) const
{
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
    } else {
        uint64_t renamed = name;
        if (rmap) {
            RenameMap::iterator found = rmap->find(name);
            if (found==rmap->end()) {
                renamed = rmap->size();
                rmap->insert(std::make_pair(name, renamed));
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
    if (!comment.empty())
        o <<"," <<comment;
    o <<"]";
}

bool
InsnSemanticsExpr::LeafNode::equal_to(const TreeNodePtr &other_, SMTSolver *solver) const
{
    bool retval = false;
    if (solver) {
        InternalNodePtr assertion = InternalNode::create(1, OP_NE, shared_from_this(), other_);
        retval = SMTSolver::SAT_NO==solver->satisfiable(assertion); /*equal if there is no solution for inequality*/
    } else {
        LeafNodePtr other = other_->isLeafNode();
        if (this==other.get()) {
            retval = true;
        } else if (other) {
            if (is_known()) {
                retval = other->is_known() && ival==other->ival;
            } else {
                retval = !other->is_known() && name==other->name;
            }
        }
    }
    return retval;
}

bool
InsnSemanticsExpr::LeafNode::equivalent_to(const TreeNodePtr &other_) const
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

void
InsnSemanticsExpr::LeafNode::depth_first_visit(Visitor *v) const
{
    assert(v!=NULL);
    (*v)(shared_from_this());
}

std::ostream&
InsnSemanticsExpr::operator<<(std::ostream &o, const TreeNode &node) {
    node.print(o);
    return o;
}

