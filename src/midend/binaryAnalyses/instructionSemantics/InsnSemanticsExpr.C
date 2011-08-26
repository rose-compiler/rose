#include "rose.h"
#include "InsnSemanticsExpr.h"
#include "SMTSolver.h"
#include "stringify.h"

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

std::set<const InsnSemanticsExpr::LeafNode*>
InsnSemanticsExpr::TreeNode::get_variables() const
{
    struct T1: public Visitor {
        std::set<const LeafNode*> vars;
        void operator()(const TreeNode *node) {
            const LeafNode *l_node = dynamic_cast<const LeafNode*>(node);
            if (l_node && !l_node->is_known())
                vars.insert(l_node);
        }
    } t1;
    depth_first_visit(&t1);
    return t1.vars;
}

/* Shallow delete: delete this node if it has no parents, but not its children */
InsnSemanticsExpr::InternalNode::~InternalNode()
{
    ROSE_ASSERT(0==get_nrefs());
    for (std::vector<const TreeNode*>::iterator ci=children.begin(); ci!=children.end(); ++ci)
        (*ci)->dec_nrefs();
    children.clear();
}

void
InsnSemanticsExpr::TreeNode::deleteDeeply()
{
    if (0==nrefs)
        delete this;
}

/* Delete this node and its children if this node has no parents. The children of a node are pointers to const objects, but we
 * can delete them if their reference counts are zero since it is only ever the user that calls this method. The user
 * presumably knows when its safe to delete a whole expression tree. */
void
InsnSemanticsExpr::InternalNode::deleteDeeply()
{
    if (0==get_nrefs()) {
        for (std::vector<const TreeNode*>::iterator ci=children.begin(); ci!=children.end(); ++ci) {
            if (0==(*ci)->dec_nrefs()) {
                TreeNode *child = const_cast<TreeNode*>(*ci);
                child->deleteDeeply();
            }
        }
        children.clear();
        delete this;
    }
}

void
InsnSemanticsExpr::InternalNode::add_child(const TreeNode *child)
{
    ROSE_ASSERT(child!=0);
    child->inc_nrefs();
    children.push_back(child);
}

void
InsnSemanticsExpr::InternalNode::print(std::ostream &o, RenameMap *rmap/*NULL*/) const
{
    o <<"(" <<to_str(op) <<"[" <<nbits <<"]";
    for (size_t i=0; i<children.size(); i++) {
        o <<" ";
        children[i]->print(o, rmap);
    }
    o <<")";
}

bool
InsnSemanticsExpr::InternalNode::equal_to(const TreeNode *other_, SMTSolver *solver/*NULL*/) const
{
    bool retval = false;
    if (solver) {
        InternalNode *assertion = new InternalNode(1, OP_NE, this, other_);
        retval = !solver->satisfiable(assertion); /*equal if we cannot find a solution for inequality*/
        assertion->deleteDeeply();
    } else {
        /* The naive approach uses structural equality */
        const InternalNode *other = dynamic_cast<const InternalNode*>(other_);
        if (this==other) {
            retval = true;
        } else if (other && op==other->op && children.size()==other->children.size()) {
            retval = true;
            for (size_t i=0; i<children.size() && retval; ++i)
                retval = children[i]->equal_to(other->children[i], NULL);
        }
    }
    return retval;
}

void
InsnSemanticsExpr::InternalNode::depth_first_visit(Visitor *v) const
{
    assert(v!=NULL);
    for (std::vector<const TreeNode*>::const_iterator ci=children.begin(); ci!=children.end(); ++ci)
        (*ci)->depth_first_visit(v);
    (*v)(this);
}

        


/* class method */
InsnSemanticsExpr::LeafNode *
InsnSemanticsExpr::LeafNode::create_variable(size_t nbits)
{
    static uint64_t name_counter = 0;

    LeafNode *retval = new LeafNode();
    retval->nbits = nbits;
    retval->known = false;
    retval->name = name_counter++;
    return retval;
}

/* class method */
InsnSemanticsExpr::LeafNode *
InsnSemanticsExpr::LeafNode::create_integer(size_t nbits, uint64_t n)
{
    LeafNode *retval = new LeafNode();
    retval->nbits = nbits;
    retval->known = true;
    retval->ival = n & (((uint64_t)1<<nbits)-1);
    return retval;
}

bool
InsnSemanticsExpr::LeafNode::is_known() const
{
    return known;
}

uint64_t
InsnSemanticsExpr::LeafNode::get_value() const
{
    assert(known);
    return ival;
}

uint64_t
InsnSemanticsExpr::LeafNode::get_name() const
{
    assert(!known);
    return name;
}

void
InsnSemanticsExpr::LeafNode::print(std::ostream &o, RenameMap *rmap/*NULL*/) const
{
    if (known) {
        if (nbits>1 && (ival & ((uint64_t)1<<(nbits-1)))) {
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
        o <<"v" <<renamed;
    }
    o <<"[" <<nbits <<"]";
}

bool
InsnSemanticsExpr::LeafNode::equal_to(const TreeNode *other_, SMTSolver *solver) const
{
    bool retval = false;
    if (solver) {
        InternalNode *assertion = new InternalNode(1, OP_NE, this, other_);
        retval = !solver->satisfiable(assertion); /*equal if we cannot find a solution for inequality*/
        assertion->deleteDeeply();
    } else {
        const LeafNode *other = dynamic_cast<const LeafNode*>(other_);
        if (this==other) {
            retval = true;
        } else if (other) {
            if (known) {
                retval = other->known && ival==other->ival;
            } else {
                retval = !other->known && name==other->name;
            }
        }
    }
    return retval;
}

void
InsnSemanticsExpr::LeafNode::depth_first_visit(Visitor *v) const
{
    assert(v!=NULL);
    (*v)(this);
}

std::ostream&
operator<<(std::ostream &o, const InsnSemanticsExpr::TreeNode *node) {
    node->print(o);
    return o;
}

