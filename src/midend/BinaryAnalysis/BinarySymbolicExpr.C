#define __STDC_LIMIT_MACROS
#include <sage3basic.h>
#include <BinarySymbolicExpr.h>

#include <BinarySmtSolver.h>
#include <boost/foreach.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <Combinatorics.h>
#include <CommandLine.h>
#include <integerOps.h>
#include <stringify.h>
#include <sstream>

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::SymbolicExpr::Interior);
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::SymbolicExpr::Leaf);
#endif

namespace Rose {
namespace BinaryAnalysis {
namespace SymbolicExpr {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Supporting functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// A mutex that's used by various methods in this namespace
static boost::mutex symbolicExprMutex;

typedef Sawyer::Container::BitVector::BitRange BitRange;

const uint64_t
MAX_NNODES = UINT64_MAX;

std::string
toStr(Operator o) {
    char buf[64];
    std::string s = stringifyBinaryAnalysisSymbolicExprOperator(o, "OP_");
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

// Escape control and non-printing characters
static std::string
escapeCharacter(char ch) {
    switch (ch) {
        case '\a': return "\\a";
        case '\b': return "\\b";
        case '\t': return "\\t";
        case '\n': return "\\n";
        case '\v': return "\\v";
        case '\f': return "\\f";
        case '\r': return "\\r";
        default:
            if (!isprint(ch)) {
                char buf[16];
                sprintf(buf, "\\%03o", (unsigned)ch);
                return buf;
            }
            return std::string(1, ch);
    }
}

// Escape a string when it appears where a symbol name could appear.
static std::string
nameEscape(const std::string &s) {
    std::string retval;
    BOOST_FOREACH (char ch, s) {
        switch (ch) {
            case '(':
            case ')':
            case '[':
            case ']':
            case '<':
            case '>':
                retval += std::string("\\") + ch;
                break;
            default:
                retval += escapeCharacter(ch);
                break;
        }
    }
    return retval;
}

// Escape text that appears inside a "<...>" style comment.
static std::string
commentEscape(const std::string &s) {
    std::string retval;

    // Escape angle brackets if they're not balanced.  We could be smarter and escape only the unbalanced angle brackets. We'll
    // leave that as an exercise for the reader. ;-)
    bool escapeAngleBrackets = false;
    int angleBracketDepth = 0;
    BOOST_FOREACH (char ch, s) {
        if ('<' == ch) {
            ++angleBracketDepth;
        } else if ('>' == ch && --angleBracketDepth < 0) {
            escapeAngleBrackets = true;
            break;
        }
    }

    BOOST_FOREACH (char ch, s) {
        switch (ch) {
            case '<':
            case '>':
                if (escapeAngleBrackets) {
                    retval += std::string("\\") + ch;
                } else {
                    retval += ch;
                }
                break;
            default:
                retval += ch;
                break;
        }
    }
    return retval;
}

bool
ExpressionLessp::operator()(const Ptr &a, const Ptr &b) {
    if (a == NULL || b == NULL)
        return a == NULL && b != NULL;
    return a->hash() < b->hash();
}

Ptr
setToIte(const Ptr &set, const SmtSolverPtr &solver, const LeafPtr &var) {
    ASSERT_not_null(set);
    InteriorPtr iset = set->isInteriorNode();
    if (!iset || iset->getOperator() != OP_SET)
        return set;
    ASSERT_require(iset->nChildren() >= 1);
    LeafPtr condVar = var==NULL ? makeIntegerVariable(32)->isLeafNode() : var;
    Ptr retval;
    for (size_t i=iset->nChildren(); i>0; --i) {
        Ptr member = iset->child(i-1);
        if (!retval) {
            retval = member;
        } else {
            Ptr cond = makeEq(condVar, makeIntegerConstant(32, i), solver);
            retval = makeIte(cond, member, retval, solver);
        }
    }
    return retval;
}

Hash
hash(const std::vector<Ptr> &exprs) {
    Hash retval = 0;
    BOOST_FOREACH (const Ptr &expr, exprs)
        retval ^= expr->hash();
    return retval;
}

struct VariableRenamer {
    ExprExprHashMap &index;
    size_t &nextVariableId;
    SmtSolverPtr solver;                                // may be null
    ExprExprHashMap seen;

    VariableRenamer(ExprExprHashMap &index, size_t &nextVariableId, const SmtSolverPtr &solver)
        : index(index), nextVariableId(nextVariableId), solver(solver) {}

    Ptr rename(const Ptr &input) {
        ASSERT_not_null(input);
        Ptr retval;

        ExprExprHashMap::iterator found = seen.find(input);
        if (found != seen.end()) {
            return found->second;
        } else if (InteriorPtr inode = input->isInteriorNode()) {
            bool anyChildRenamed = false;
            Nodes newChildren;
            newChildren.reserve(inode->nChildren());
            BOOST_FOREACH (const Ptr &child, inode->children()) {
                Ptr newChild = rename(child);
                if (newChild != child)
                    anyChildRenamed = true;
                newChildren.push_back(newChild);
            }
            if (!anyChildRenamed) {
                retval = input;
            } else {
                retval = Interior::instance(inode->getOperator(), newChildren, solver, inode->comment(), inode->flags());
            }
            seen.insert(std::make_pair(input, retval));
        } else {
            LeafPtr leaf = input->isLeafNode();
            ASSERT_not_null(leaf);
            if (leaf->isVariable2()) {
                ExprExprHashMap::iterator found = index.find(input);
                if (found != index.end()) {
                    retval = found->second;
                } else {
                    retval = makeVariable(leaf->type(), nextVariableId++, leaf->comment(), leaf->flags());
                    index[input] = retval;
                }
            } else {
                retval = leaf;
            }
            seen.insert(std::make_pair(input, retval));
        }
        ASSERT_not_null(retval);
        return retval;
    }
};

// Perform substitutions by looking for a single expression. Preserves sharing of subexpressions. Creates new
// expressions only when necessary. Comparison for substitution is by isEquivalentTo method.
struct SingleSubstituter {
    Ptr from;                                           // replace this
    Ptr to;                                             // with this
    SmtSolverPtr solver;                                // for simplifications (optional)
    ExprExprHashMap seen;                               // things we've already seen

    SingleSubstituter(const Ptr &from, const Ptr &to, const SmtSolverPtr &solver)
        : from(from), to(to), solver(solver) {}

    Ptr substitute(const Ptr &input) {
        ASSERT_not_null(input);
        Ptr retval;

        ExprExprHashMap::iterator found = seen.find(input);
        if (found != seen.end()) {
            retval = found->second;
        } else if (input->isEquivalentTo(from)) {
            retval = to;
            seen.insert(std::make_pair(input, retval));
        } else if (InteriorPtr inode = input->isInteriorNode()) {
            bool anyChildChanged = false;
            Nodes newChildren;
            newChildren.reserve(inode->nChildren());
            BOOST_FOREACH (const Ptr &child, inode->children()) {
                Ptr newChild = substitute(child);
                if (newChild != child)
                    anyChildChanged = true;
                newChildren.push_back(newChild);
            }
            if (anyChildChanged) {
                retval = Interior::instance(inode->getOperator(), newChildren, solver, inode->comment(), inode->flags());
            } else {
                retval = input;
            }
            seen.insert(std::make_pair(input, retval));
        } else {
            retval = input;
            seen.insert(std::make_pair(input, retval));
        }
        ASSERT_not_null(retval);
        return retval;
    }
};

struct MultiSubstituter {
    const ExprExprHashMap &substitutions;
    SmtSolverPtr solver;
    ExprExprHashMap seen;

    MultiSubstituter(const ExprExprHashMap &substitutions, const SmtSolverPtr &solver)
        : substitutions(substitutions), solver(solver) {}

    Ptr substitute(const Ptr &input) {
        ASSERT_not_null(input);
        Ptr retval;
        ExprExprHashMap::iterator previous = seen.find(input);
        if (previous != seen.end())
            return previous->second;

        ExprExprHashMap::const_iterator found = substitutions.find(input);
        if (found != substitutions.end()) {
            retval = found->second;
        } else if (InteriorPtr inode = input->isInteriorNode()) {
            bool anyChildChanged = false;
            Nodes newChildren;
            newChildren.reserve(inode->nChildren());
            BOOST_FOREACH (const Ptr &child, inode->children()) {
                Ptr newChild = substitute(child);
                if (newChild != child)
                    anyChildChanged = true;
                newChildren.push_back(newChild);
            }
            if (!anyChildChanged) {
                retval = input;
            } else {
                retval = Interior::instance(inode->getOperator(), newChildren, solver, inode->comment(), inode->flags());
            }
        } else {
            retval = input;
        }
        ASSERT_not_null(retval);
        seen.insert(std::make_pair(input, retval));
        return retval;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Types
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool
Type::operator<(const Type &other) const {
    if (typeClass_ != other.typeClass_)
        return typeClass_ < other.typeClass_;
    if (totalWidth_ != other.totalWidth_)
        return totalWidth_ < other.totalWidth_;
    return secondaryWidth_ < other.secondaryWidth_;
}

void
Type::print(std::ostream &out, TypeStyle::Flag style) const {
    switch (typeClass()) {
        case INTEGER:
            switch (style) {
                case TypeStyle::ABBREVIATED:
                    out <<"u" <<nBits();
                    break;
                case TypeStyle::FULL:
                    out <<nBits() <<"-bit integer";
                    break;
            }
            break;
        case FP:
            switch (style) {
                case TypeStyle::ABBREVIATED:
                    if (exponentWidth() == 11 && significandWidth() == 53) {
                        out <<"f64";
                        break;
                    } else if (exponentWidth() == 8 && significandWidth() == 24) {
                        out <<"f32";
                        break;
                    }
                    // fall through
                case TypeStyle::FULL:
                    out <<nBits() <<"-bit float(e=" <<exponentWidth() <<" s=" <<significandWidth() <<")";
                    break;
            }
            break;
        case MEMORY:
            switch (style) {
                case TypeStyle::ABBREVIATED:
                    out <<"m" <<nBits() <<"[" <<addressWidth() <<"]";
                    break;
                case TypeStyle::FULL:
                    out <<nBits() <<"-bit memory(a=" <<addressWidth() <<")";
                    break;
            }
            break;
        case INVALID:
            out <<"invalid";
            break;
    }
}

std::string
Type::toString(TypeStyle::Flag style) const {
    std::ostringstream ss;
    print(ss, style);
    return ss.str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Base node
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

boost::logic::tribool (*Node::mayEqualCallback)(const Ptr&, const Ptr&, const SmtSolver::Ptr&) = NULL;

InteriorPtr
Node::isInteriorNode() const {
    return InteriorPtr(dynamic_cast<Interior*>(const_cast<Node*>(this)));
}

LeafPtr
Node::isLeafNode() const {
    return LeafPtr(dynamic_cast<Leaf*>(const_cast<Node*>(this)));
}

Ptr
Node::newFlags(unsigned newFlags) const {
    if (newFlags == flags_)
        return Ptr(const_cast<Node*>(this));
    if (InteriorPtr inode = isInteriorNode()) {
        // No SMT solver is necessary since changing the flags won't cause the new expression to simplify any differently than
        // it already is. In fact, it there might be a faster way to create the new expression given this fact.
        SmtSolverPtr solver;
        return Interior::instance(inode->getOperator(), inode->children(), solver, comment(), newFlags);
    }
    LeafPtr lnode = isLeafNode();
    ASSERT_not_null(lnode);
    if (lnode->isConstant()) {
        return makeConstant(type(), lnode->bits(), comment(), newFlags);
    } else {
        ASSERT_require(lnode->isVariable2());
        return makeVariable(type(), lnode->nameId(), comment(), newFlags);
    }
}

std::set<LeafPtr>
Node::getVariables() const {
    struct T1: public Visitor {
        std::set<LeafPtr> vars;
        std::set<const Node*> seen;

        VisitAction preVisit(const Ptr &node) {
            if (seen.insert(getRawPointer(node)).second) {
                return CONTINUE;
            } else {
                return TRUNCATE;
            }
        }

        VisitAction postVisit(const Ptr &node) {
            LeafPtr l_node = node->isLeafNode();
            if (l_node && l_node->isVariable2())
                vars.insert(l_node);
            return CONTINUE;
        }
    } t1;
    depthFirstTraversal(t1);
    return t1.vars;
}

Ptr
Node::renameVariables(ExprExprHashMap &index /*in,out*/, size_t &nextVariableId, const SmtSolverPtr &solver) {
    VariableRenamer renamer(index, nextVariableId, solver);
    return renamer.rename(this->sharedFromThis());
}

Ptr
Node::substituteMultiple(const ExprExprHashMap &substitutions, const SmtSolverPtr &solver) {
    MultiSubstituter substituter(substitutions, solver);
    return substituter.substitute(this->sharedFromThis());
}

struct Hasher: Visitor {
    virtual VisitAction preVisit(const Ptr &node) ROSE_OVERRIDE {
        return 0 == node->isHashed() ? CONTINUE : TRUNCATE;
    }

    virtual VisitAction postVisit(const Ptr &node) ROSE_OVERRIDE {
        if (!node->isHashed()) {                        // probably true, but some other thread may have beaten us here.
            Hash h = hash(hash(node->isMemoryExpr() ? node->domainWidth() : 0, node->nBits()), node->flags());
            if (LeafPtr leaf = node->isLeafNode()) {
                if (leaf->isConstant()) {
                    if (leaf->isIntegerConstant() && leaf->nBits() <= 64) {
                        h = hash(h, leaf->toUnsigned().get());
                    } else {
                        for (size_t i=0; i<leaf->nBits(); i+=64) {
                            size_t j = std::min(i+64, leaf->nBits());
                            h = hash(h, leaf->bits().toInteger(BitRange::hull(i, j-1)));
                        }
                    }
                } else {
                    // It's okay to hash only the variable ID and not the type because one variable shouldn't never be more
                    // than one type.
                    h = hash(h, leaf->nameId());
                }
            } else {
                InteriorPtr inode = node->isInteriorNode();
                ASSERT_not_null(inode);
                h = hash(h, inode->getOperator());
                BOOST_FOREACH (const Ptr &child, inode->children()) {
                    ASSERT_require(child->isHashed());
                    h = hash(h, child->hash());
                }
            }
            node->hash(h);
        }
        return CONTINUE;
    }

    // Incorporates data into the existing hash, h, and returns a new hash. This is no particular well-known algorithm, but
    // testing showed that it gives pretty well-distributed results for close values, particularly when called on two or more
    // pieces of data.
    Hash hash(Hash h, uint64_t data) {
        ASSERT_require(sizeof(h) == 8);
        for (size_t i=0; i<64-6; i += 6) {
            unsigned sa = ((data >> i) ^ h ^ i) & 0x3f;
            h = (h >> (64-sa)) | (h << sa);
            h ^= data;
        }
        return h;
    }
};

Hash
Node::hash() const {
    if (0==hashval_) {
        Hasher hasher;
        depthFirstTraversal(hasher);
    }
    boost::unique_lock<boost::mutex> lock(symbolicExprMutex);
    return hashval_;
}

void
Node::hash(Hash h) {
    boost::unique_lock<boost::mutex> lock(symbolicExprMutex);
    hashval_ = h;
}

void
Node::assertAcyclic() const {
#ifndef NDEBUG
    struct T1: Visitor {
        std::vector<const Node*> ancestors;
        VisitAction preVisit(const Ptr &node) {
            ASSERT_require(std::find(ancestors.begin(), ancestors.end(), getRawPointer(node))==ancestors.end());
            ancestors.push_back(getRawPointer(node));
            return CONTINUE;
        }
        VisitAction postVisit(const Ptr &node) {
            ASSERT_require(!ancestors.empty() && ancestors.back()==getRawPointer(node));
            ancestors.pop_back();
            return CONTINUE;
        }
    } t1;
    depthFirstTraversal(t1);
#endif
}

uint64_t
Node::nNodesUnique() const {
    std::vector<Ptr> exprs(1, Ptr(const_cast<Node*>(this)));
    return SymbolicExpr::nNodesUnique(exprs.begin(), exprs.end());
}

std::vector<Ptr>
Node::findCommonSubexpressions() const {
    std::vector<Ptr> exprs(1, Ptr(const_cast<Node*>(this)));
    return SymbolicExpr::findCommonSubexpressions(exprs);
}

void
Node::printFlags(std::ostream &o, unsigned flags, char &bracket) const {
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
    if ((flags & BOTTOM) != 0) {
        o <<bracket <<"bottom";
        bracket = ',';
        flags &= ~BOTTOM;
    }
    if (flags != 0) {
        o <<bracket <<"f=" <<std::hex <<flags <<std::dec;
        bracket = ',';
    }
}

InteriorPtr
Node::isOperator(Operator op) const {
    if (InteriorPtr inode = isInteriorNode()) {
        if (inode->getOperator() == op)
            return inode;
    }
    return InteriorPtr();
}

bool
Node::matchAddVariableConstant(LeafPtr &variable/*out*/, LeafPtr &constant/*out*/) const {
    if (InteriorPtr inode = isOperator(OP_ADD)) {
        if (inode->nChildren() == 2) {
            LeafPtr arg0 = inode->child(0)->isLeafNode();
            LeafPtr arg1 = inode->child(1)->isLeafNode();
            if (arg0 && arg1) {
                if (arg0->isIntegerVariable() && arg1->isIntegerConstant()) {
                    variable = arg0;
                    constant = arg1;
                    return true;
                } else if (arg0->isIntegerConstant() && arg1->isIntegerVariable()) {
                    variable = arg1;
                    constant = arg0;
                    return true;
                }
            }
        }
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Interior node
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Interior::Interior()
    : op_(OP_ADD), nnodes_(1) {}

Interior::Interior(const Type &type, Operator op, const Nodes &children, const std::string &comment, unsigned flags)
    : Node(comment), op_(op), nnodes_(1) {
    for (size_t i=0; i<children.size(); ++i)
        addChild(children[i]);
    adjustWidth(type);
    adjustBitFlags(flags);
    if (type.isValid() && type != this->type())
        throw Exception("operator type mismatch");
}

Ptr
Interior::instance(Operator op, const Ptr &a,
                   const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    return instance(Type(), op, a, solver, comment, flags);
}

Ptr
Interior::instance(const Type &type, Operator op, const Ptr &a,
                   const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    Nodes args;
    args.push_back(a);
    return instance(type, op, args, solver, comment, flags);
}

Ptr
Interior::instance(Operator op, const Ptr &a, const Ptr &b,
                   const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    return instance(Type(), op, a, b, solver, comment, flags);
}

Ptr
Interior::instance(const Type &type, Operator op, const Ptr &a, const Ptr &b,
                   const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    Nodes args;
    args.push_back(a);
    args.push_back(b);
    return instance(type, op, args, solver, comment, flags);
}

Ptr
Interior::instance(Operator op, const Ptr &a, const Ptr &b, const Ptr &c,
                   const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    return instance(Type(), op, a, b, c, solver, comment, flags);
}

Ptr
Interior::instance(const Type &type, Operator op, const Ptr &a, const Ptr &b, const Ptr &c,
                   const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    Nodes args;
    args.push_back(a);
    args.push_back(b);
    args.push_back(c);
    return instance(type, op, args, solver, comment, flags);
}

Ptr
Interior::instance(Operator op, const Nodes &arguments,
                   const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    return instance(Type(), op, arguments, solver, comment, flags);
}

Ptr
Interior::instance(const Type &type, Operator op, const Nodes &arguments,
                   const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    InteriorPtr retval(new Interior(type, op, arguments, comment, flags));
    return retval->simplifyTop(solver);
}

// deprecated [Robb Matzke 2019-10-01]
Ptr
Interior::create(size_t nbits, Operator op, const Ptr &a,
                 const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    if (nbits == 0) {
        return instance(op, a, solver, comment, flags);
    } else {
        return instance(Type::integer(nbits), op, a, solver, comment, flags);
    }
}

// deprecated [Robb Matzke 2019-10-01]
Ptr
Interior::create(size_t nbits, Operator op, const Ptr &a, const Ptr &b,
                 const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    if (nbits == 0) {
        return instance(op, a, b, solver, comment, flags);
    } else {
        return instance(Type::integer(nbits), op, a, b, solver, comment, flags);
    }
}

// deprecated [Robb Matzke 2019-10-01]
Ptr
Interior::create(size_t nbits, Operator op, const Ptr &a, const Ptr &b, const Ptr &c,
                 const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    if (nbits == 0) {
        return instance(op, a, b, c, solver, comment, flags);
    } else {
        return instance(Type::integer(nbits), op, a, b, c, solver, comment, flags);
    }
}

// deprecated [Robb Matzke 2019-10-02]
Ptr
Interior::create(size_t nbits, Operator op, const Nodes &arguments,
                 const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    if (nbits == 0) {
        return instance(op, arguments, solver, comment, flags);
    } else {
        return instance(Type::integer(nbits), op, arguments, solver, comment, flags);
    }
}

void
Interior::addChild(const Ptr &child)
{
    ASSERT_not_null(child);
    children_.push_back(child);
    if (nnodes_ != MAX_NNODES) {
        if (nnodes_ + child->nNodes() < nnodes_) {
            nnodes_ = MAX_NNODES;                       // overflow
        } else {
            nnodes_ += child->nNodes();
        }
    }
}

void
Interior::adjustWidth(const Type &type) {
    if (children_.empty())
        throw Exception(toStr(op_) + " operator requires argument(s)");
    switch (op_) {
        case OP_ASR:
        case OP_ROL:
        case OP_ROR:
        case OP_SHL0:
        case OP_SHL1:
        case OP_SHR0:
        case OP_SHR1: {
            if (nChildren() != 2)
                throw Exception(toStr(op_) + " operator expects two arguments");
            if (!child(0)->isIntegerExpr())
                throw Exception(toStr(op_) + " operator's first argument (shift amount) must be integer");
            if (!child(1)->isIntegerExpr())
                throw Exception(toStr(op_) + " operator's second argument (value to shift) must be integer");
            type_ = Type::integer(child(1)->nBits());
            break;
        }
        case OP_CONCAT: {
            size_t totalWidth = 0;
            BOOST_FOREACH (const Ptr &child, children_) {
                if (!child->isIntegerExpr())
                    throw Exception(toStr(op_) + " operator's arguments must be integer");
                totalWidth += child->nBits();
            }
            type_ = Type::integer(totalWidth);
            break;
        }
        case OP_CONVERT: {
            if (nChildren() != 1)
                throw Exception(toStr(op_) + " operator expects one argument");
            type_ = type;
            break;
        }
        case OP_REINTERPRET: {
            if (nChildren() != 1)
                throw Exception(toStr(op_) + " operator expects one argument");
            if (child(0)->type().nBits() == type.nBits())
                type_ = type;
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
            if (nChildren() != 2)
                throw Exception(toStr(op_) + " operator expects two arguments");
            if (!child(0)->isIntegerExpr())
                throw Exception(toStr(op_) + " operator's first argument (shift amount) must be integer");
            if (!child(1)->isIntegerExpr())
                throw Exception(toStr(op_) + " operator's second argument (value to shift) must be integer");
            if (child(0)->nBits() != child(1)->nBits())
                throw Exception(toStr(op_) + " operator's arguments must both be the same width");
            type_ = Type::integer(1);
            break;
        }
        case OP_EXTRACT: {
            if (nChildren() != 3)
                throw Exception(toStr(op_) + " operator expects three arguments");
            if (!child(0)->isIntegerConstant())
                throw Exception(toStr(op_) + " operator's first argument (begin bit) must be an integer constant");
            if (!child(1)->isIntegerConstant())
                throw Exception(toStr(op_) + " operator's second argument (end bit) must be an integer constant");
            if (child(0)->nBits() > 64)
                throw Exception(toStr(op_) + " operator's first argument (begin bit) must be 64 bits or narrower");
            if (child(1)->nBits() > 64)
                throw Exception(toStr(op_) + " operator's second argument (end bit) must be 64 bits or narrower");
            if (!child(2)->isIntegerExpr() && !child(2)->isFloatingPointExpr())
                throw Exception(toStr(op_) + " operator's third argument must be integer or floating-point");
            if (*child(0)->toUnsigned() >= *child(1)->toUnsigned())
                throw Exception(toStr(op_) + " operator's first argument must be less than the second");
            size_t totalSize = *child(1)->toUnsigned() - *child(0)->toUnsigned();
            // Result will be integer even if arg is floating-point. Example: extracting the bytes of a floating-point value in
            // order to store it in memory results in integer-valued bytes since it doesn't make sense to interpret the bytes
            // as floating-point values.
            type_ = Type::integer(totalSize);
            break;
        }
        case OP_ITE: {
            if (nChildren() != 3)
                throw Exception(toStr(op_) + " operator expects three arguments");
            if (!child(0)->isIntegerExpr())
                throw Exception(toStr(op_) + " operator's first argument (condition) must be integer");
            if (child(0)->nBits() != 1)
                throw Exception(toStr(op_) + " operator's first argument (condition) must be Boolean");
            if (child(1)->type() != child(2)->type())
                throw Exception(toStr(op_) + " operator's second and third arguments must have the same type");
            type_ = child(1)->type();
            break;
        }
        case OP_LET: {
            if (nChildren() != 3)
                throw Exception(toStr(op_) + " operator expects three arguments");
            type_ = child(2)->type();
            break;
        }
        case OP_LSSB:
        case OP_MSSB:
        case OP_NEGATE: {
            if (nChildren() != 1)
                throw Exception(toStr(op_) + " operator expects one argument");
            if (!child(0)->isIntegerExpr())
                throw Exception(toStr(op_) + " argument must be integer");
            type_ = child(0)->type();
            break;
        }
        case OP_READ: {
            if (nChildren() != 2)
                throw Exception(toStr(op_) + " operator expects two arguments");
            if (!child(0)->isMemoryExpr())
                throw Exception(toStr(op_) + " operator's first argument must be a memory state");
            if (!child(1)->isIntegerExpr())
                throw Exception(toStr(op_) + " operator's second argument (address) must be integer");
            if (child(0)->domainWidth() != child(1)->nBits())
                throw Exception(toStr(op_) + " operator's arguments have mismatched sizes");
            type_ = Type::integer(child(0)->nBits());   // size of values stored in memory
            break;
        }
        case OP_SDIV:
        case OP_UDIV: {
            if (nChildren() != 2)
                throw Exception(toStr(op_) + " operator expects two arguments");
            if (!child(0)->isIntegerExpr())
                throw Exception(toStr(op_) + " operator's first argument must be integer");
            if (!child(1)->isIntegerExpr())
                throw Exception(toStr(op_) + " operator's second argument must be integer");
            type_ = child(0)->type();
            break;
        }
        case OP_SEXTEND:
        case OP_UEXTEND: {
            if (nChildren() != 2)
                throw Exception(toStr(op_) + " operator expects two arguments");
            if (!child(0)->isIntegerConstant())
                throw Exception(toStr(op_) + " operator's first argument (size) must be an integer constant");
            if (child(0)->nBits() > 64)
                throw Exception(toStr(op_) + " operator's first argument (size) must be 64 bits or narrower");
            if (!child(1)->isIntegerExpr())
                throw Exception(toStr(op_) + " operator's second argument must be integer");
            type_ = Type::integer(child(0)->toUnsigned().get());
            break;
        }
        case OP_SMOD:
        case OP_UMOD: {
            if (nChildren() != 2)
                throw Exception(toStr(op_) + " operator expects two arguments");
            if (!child(0)->isIntegerExpr())
                throw Exception(toStr(op_) + " operator's first argument must be integer");
            if (!child(1)->isIntegerExpr())
                throw Exception(toStr(op_) + " operator's second argument must be integer");
            type_ = child(1)->type();
            break;
        }
        case OP_SMUL:
        case OP_UMUL: {
            if (nChildren() < 1)
                throw Exception(toStr(op_) + " operator expects at least one argument");
            size_t totalWidth = 0;
            for (size_t i=0; i<nChildren(); ++i) {
                if (!child(i)->isIntegerExpr())
                    throw Exception(toStr(op_) + " operator's arguments must all be integer");
                totalWidth += child(i)->nBits();
            }
            type_ = Type::integer(totalWidth);
            break;
        }
        case OP_WRITE: {
            if (nChildren() != 3)
                throw Exception(toStr(op_) + " operator expects three arguments");
            if (!child(0)->isMemoryExpr())
                throw Exception(toStr(op_) + " operator's first operand must be a memory state");
            if (!child(1)->isIntegerExpr())
                throw Exception(toStr(op_) + " operator's second argument (address) must be integer");
            if (!child(2)->isIntegerExpr())
                throw Exception(toStr(op_) + " operator's third argument (value to write) must be integer");
            if (child(1)->nBits() != child(0)->domainWidth())
                throw Exception(toStr(op_) + " operator's second argument (address) has incorrect width");
            if (child(2)->nBits() != child(0)->nBits())
                throw Exception(toStr(op_) + " operator's third argument (value to write) has incorrect width");
            type_ = child(0)->type();
            break;
        }
        case OP_ZEROP: {
            if (nChildren() != 1)
                throw Exception(toStr(op_) + " operator expects one argument");
            if (!child(0)->isIntegerExpr())
                throw Exception(toStr(op_) + " operator's argument must be integer");
            type_ = Type::integer(1);
            break;
        }
        case OP_FP_LE:
        case OP_FP_LT:
        case OP_FP_GE:
        case OP_FP_GT:
        case OP_FP_EQ: {
            if (nChildren() != 2)
                throw Exception(toStr(op_) + " operator expects two arguments");
            if (!child(0)->isFloatingPointExpr())
                throw Exception(toStr(op_) + " operator's first argument must be floating-point");
            if (!child(1)->isFloatingPointExpr())
                throw Exception(toStr(op_) + " operator's second argument must be floating-point");
            type_ = Type::integer(1);
            break;
        }
        case OP_FP_ISNORM:
        case OP_FP_ISSUBNORM:
        case OP_FP_ISZERO:
        case OP_FP_ISINFINITE:
        case OP_FP_ISNAN:
        case OP_FP_ISNEG:
        case OP_FP_ISPOS: {
            if (nChildren() != 1)
                throw Exception(toStr(op_) + " operator expects one argument");
            if (!child(0)->isFloatingPointExpr())
                throw Exception(toStr(op_) + " operator's argument must be floating-point");
            type_ = Type::integer(1);
            break;
        }
        default: {
            // All children must have the same type, which is the type of this expression. This is suitable for things like
            // bitwise operators, add, etc.
            ASSERT_require(nChildren() > 0);
            for (size_t i = 1; i < nChildren(); ++i) {
                if (child(i)->type() != child(0)->type())
                    throw Exception(toStr(op_) + " operator's arguments must all be the same type");
            }
            type_ = child(0)->type();
            break;
        }
    }
    ASSERT_require(nBits() != 0);
}

void
Interior::adjustBitFlags(unsigned flags) {
    flags_ = flags;
    BOOST_FOREACH (const Ptr &child, children_)
        flags_ |= child->flags();
}

void
Interior::print(std::ostream &o, Formatter &fmt) const {
    struct FormatGuard {
        Formatter &fmt;
        FormatGuard(Formatter &fmt): fmt(fmt) {
            ++fmt.cur_depth;
        }
        ~FormatGuard() {
            --fmt.cur_depth;
        }
    } formatGuard(fmt);

    o <<"(" <<toStr(op_);

    // The width of an operator is not normally too useful since it can also be inferred from the width of its operands, but we
    // print it anyway for the benefit of mere humans.
    char bracket = '[';
    if (fmt.show_type) {
        o <<bracket <<type().toString(TypeStyle::ABBREVIATED);
        bracket = ',';
    }
    if (fmt.show_flags)
        printFlags(o, flags(), bracket /*in,out*/);
    if (fmt.show_comments!=Formatter::CMT_SILENT && !comment_.empty()) {
        o <<bracket <<comment_;
        bracket = ',';
    }
    if (bracket != '[')
        o <<"]";

    // Print the operand list.
    if (fmt.max_depth!=0 && fmt.cur_depth>=fmt.max_depth && 0!=nChildren()) {
        o <<" ...";
    } else {
        for (size_t i=0; i<children_.size(); i++) {
            bool printed = false;
            LeafPtr child_leaf = children_[i]->isLeafNode();
            o <<" ";
            switch (op_) {
                case OP_ASR:
                case OP_ROL:
                case OP_ROR:
                case OP_UEXTEND:
                    if (0==i && child_leaf) {
                        child_leaf->printAsUnsigned(o, fmt);
                        printed = true;
                    }
                    break;

                case OP_EXTRACT:
                    if ((0==i || 1==i) && child_leaf) {
                        child_leaf->printAsUnsigned(o, fmt);
                        printed = true;
                    }
                    break;

                case OP_AND:
                case OP_OR:
                case OP_XOR:
                case OP_CONCAT:
                case OP_UDIV:
                case OP_UGE:
                case OP_UGT:
                case OP_ULE:
                case OP_ULT:
                case OP_UMOD:
                case OP_UMUL:
                case OP_LET:
                case OP_FP_LT:
                case OP_FP_LE:
                case OP_FP_GT:
                case OP_FP_GE:
                case OP_FP_EQ:
                case OP_FP_ISNORM:
                case OP_FP_ISSUBNORM:
                case OP_FP_ISZERO:
                case OP_FP_ISINFINITE:
                case OP_FP_ISNAN:
                case OP_FP_ISNEG:
                case OP_FP_ISPOS:
                    if (child_leaf) {
                        child_leaf->printAsUnsigned(o, fmt);
                        printed = true;
                    }
                    break;

                default:
                    break;
            }

            if (!printed)
                children_[i]->print(o, fmt);
        }
    }
    o <<")";

    if (!comment().empty())
        o <<"<" <<commentEscape(comment()) <<">";
}

bool
Interior::mustEqual(const Ptr &other_, const SmtSolverPtr &solver/*NULL*/) {
    bool retval = false;
    if (this == getRawPointer(other_)) {
        retval = true;
    } else if (isEquivalentTo(other_)) {
        // This is probably faster than using an SMT solver. It also serves as the naive approach when an SMT solver
        // is not available.
        retval = true;
    } else if (solver) {
        SmtSolver::Transaction transaction(solver);
        Ptr assertion = makeNe(sharedFromThis(), other_, solver);
        solver->insert(assertion);
        retval = SmtSolver::SAT_NO==solver->check(); /*equal if there is no solution for inequality*/
    }
    return retval;
}

bool
Interior::mayEqual(const Ptr &other, const SmtSolverPtr &solver/*NULL*/) {
    // Fast comparison of literally the same expression pointer
    if (this == getRawPointer(other))
        return true;

    // Give the user a chance to decide.
    if (mayEqualCallback) {
        boost::logic::tribool result = (mayEqualCallback)(sharedFromThis(), other, solver);
        if (true == result || false == result)
            return result ? true : false;
    }

    // Two addition operations of the form V + C1 and V + C2 where V is a variable and C1 and C2 are constants, are equal if
    // and only if C1 = C2.
    LeafPtr variableA, variableB, constantA, constantB;
    if (matchAddVariableConstant(variableA/*out*/, constantA/*out*/)) {
        if (other->matchAddVariableConstant(variableB/*out*/, constantB/*out*/)) {
            // Comparing V + C1 with V + C2; return true iff C1 == C2
            if (variableA->nameId() == variableB->nameId()) {
                ASSERT_require(variableA->nBits() == variableB->nBits());
                ASSERT_require(constantA->nBits() == constantB->nBits());
                return constantA->bits().compare(constantB->bits()) == 0;
            }
        } else if ((variableB = other->isLeafNode()) && variableB->isIntegerVariable()) {
            // Comparing V + C with V; return true iff C == 0 (which it shouldn't or else the additive identity rule would have
            // already kicked in and removed it.
            if (variableA->nameId() == variableB->nameId()) {
                ASSERT_require(variableA->nBits() == variableB->nBits());
                ASSERT_require(constantA->nBits() == variableA->nBits());
                ASSERT_forbid2(constantA->bits().isEqualToZero(), "additive identity should have been simplified");
                return false;
            }
        }
    }

    // Two expressions that are structurally equivalent are also equal.
    if (isEquivalentTo(other))
        return true;

    // It is difficult to prove that arbitrary expressions are equal or not equal.  Ask the solver to find a solution where
    // they are equal.  If the solver finds a solution, then obviously they can be equal.  If the solver can't prove the
    // equality assertion, we want to assume that they can be equal.  Thus only if the solver can prove that they cannot be
    // equal do we have anything to return here.
    if (solver) {
        SmtSolver::Transaction transaction(solver);
        Ptr assertion = makeEq(sharedFromThis(), other, solver);
        solver->insert(assertion);
        if (SmtSolver::SAT_NO == solver->check())
            return false;
    }

    // If we couldn't prove that the two expressions are unequal, then assume they could be equal.
    return true;
}

int
Interior::compareStructure(const Ptr &other_) {
    InteriorPtr other = other_->isInteriorNode();
    if (this==getRawPointer(other)) {
        return 0;
    } else if (other==NULL) {
        return 1;                                       // leaf nodes < internal nodes
    } else if (op_ != other->op_) {
        return op_ < other->op_ ? -1 : 1;
    } else if (type() != other->type()) {
        return type() < other->type() ? -1 : 1;
    } else if (children_.size() != other->children_.size()) {
        return children_.size() < other->children_.size() ? -1 : 1;
    } else if (flags() != other->flags()) {
        return flags() < other->flags() ? -1 : 1;
    } else {
        // compare children
        ASSERT_require(children_.size()==other->children_.size());
        for (size_t i=0; i<children_.size(); ++i) {
            if (int cmp = children_[i]->compareStructure(other->children_[i]))
                return cmp;
        }
    }
    return 0;
}

bool
Interior::isEquivalentTo(const Ptr &other_) {
    bool retval = false;
    InteriorPtr other = other_->isInteriorNode();
    if (this == getRawPointer(other)) {
        retval = true;
    } else if (NULL == other || type() != other->type() || flags() != other->flags()) {
        retval = false;
    } else if (hashval_ != 0 && other->hashval_ != 0 && hashval_ != other->hashval_) {
        // Unequal hashvals imply non-equivalent expressions.  The converse is not necessarily true due to possible
        // collisions.
        retval = false;
    } else if (op_ == other->op_ && children_.size() == other->children_.size()) {
        retval = true;
        for (size_t i=0; i < children_.size() && retval; ++i)
            retval = children_[i]->isEquivalentTo(other->children_[i]);
        // Cache hash values. There's no need to compute a hash value if we've determined that the two expressions are
        // equivalent because it wouldn't save us any work--two equal hash values doesn't necessarily mean that two expressions
        // are equivalent.  However, if we already know one of the hash values then we can cache that hash value in the other
        // expression too.
        if (retval) {
            if (hashval_ != 0 && other->hashval_ == 0) {
                other->hashval_ = hashval_;
            } else if (hashval_ == 0 && other->hashval_ != 0) {
                hashval_ = other->hashval_;
            } else {
                ASSERT_require(hashval_ == other->hashval_);
            }
        } else {
#ifdef InsnInstructionExpr_USE_HASHES
            hashval_ = hash();
            other->hashval_ = other->hash();
#endif
        }
    }
    return retval;
}

Ptr
Interior::substitute(const Ptr &from, const Ptr &to, const SmtSolverPtr &solver) {
    ASSERT_require(from!=NULL && to!=NULL && from->nBits()==to->nBits());
    if (isEquivalentTo(from))
        return to;
    SingleSubstituter subber(from, to, solver);
    return subber.substitute(sharedFromThis());
}

VisitAction
Interior::depthFirstTraversal(Visitor &v) const {
    InteriorPtr self(const_cast<Interior*>(this));
    VisitAction action = v.preVisit(self);
    if (CONTINUE==action) {
        for (std::vector<Ptr>::const_iterator ci=children_.begin(); ci!=children_.end(); ++ci) {
            action = (*ci)->depthFirstTraversal(v);
            if (TERMINATE==action)
                break;
        }
    }
    if (TERMINATE!=action)
        action = v.postVisit(self);
    return action;
}

InteriorPtr
Interior::associative() {
    Nodes newOperands;
    std::list<Ptr> worklist(children_.begin(), children_.end());
    bool modified = false;
    while (!worklist.empty()) {
        Ptr child = worklist.front();
        worklist.pop_front();
        InteriorPtr ichild = child->isInteriorNode();
        if (ichild && ichild->op_ == op_) {
            worklist.insert(worklist.begin(), ichild->children_.begin(), ichild->children_.end());
            modified = true;
        } else {
            newOperands.push_back(child);
        }
    }
    if (!modified)
        return isInteriorNode();

    // Return the new expression without simplifying it again.
    return InteriorPtr(new Interior(type(), op_, newOperands, comment(), 0));
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Simplification
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// compare expressions for sorting operands of commutative operators. Returns -1, 0, 1
static int
expr_cmp(const Ptr &a, const Ptr &b)
{
    ASSERT_not_null(a);
    ASSERT_not_null(b);
    InteriorPtr ai = a->isInteriorNode();
    InteriorPtr bi = b->isInteriorNode();
    LeafPtr al = a->isLeafNode();
    LeafPtr bl = b->isLeafNode();
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

        // Floating point constants are greater than all other nodes
        if (al->isFloatingPointConstant() != bl->isFloatingPointConstant()) {
            return al->isFloatingPointConstant() ? 1 : -1;
        } else if (al->isFloatingPointConstant() && bl->isFloatingPointConstant()) {
            return al->bits().compare(bl->bits()); // compare bits, not actual FP values
        }
        ASSERT_forbid(al->isFloatingPointConstant());
        ASSERT_forbid(bl->isFloatingPointConstant());

        // Integer constants are greater than remaining nodes
        if (al->isIntegerConstant() != bl->isIntegerConstant()) {
            return al->isIntegerConstant() ? 1 : -1;
        } else if (al->isIntegerConstant() && bl->isIntegerConstant()) {
            return al->bits().compare(bl->bits());
        }
        ASSERT_forbid(al->isIntegerConstant());
        ASSERT_forbid(bl->isIntegerConstant());

        ASSERT_require(al->isVariable2());
        ASSERT_require(bl->isVariable2());

        // Memory variables are greater than all other variables
        if (al->isMemoryVariable() != bl->isMemoryVariable()) {
            return al->isMemoryVariable() ? 1 : -1;
        } else if (al->isMemoryVariable() && bl->isMemoryVariable()) {
            if (al->nameId() == bl->nameId()) {
                return 0;
            } else {
                return al->nameId() < bl->nameId() ? -1 : 1;
            }
        }
        ASSERT_forbid(al->isMemoryVariable());
        ASSERT_forbid(bl->isMemoryVariable());

        // Floating-point variables are greater than all remaining variable types
        if (al->isFloatingPointVariable() != bl->isFloatingPointVariable()) {
            return al->isFloatingPointVariable() ? 1 : -1;
        } else if (al->isFloatingPointVariable() && b->isFloatingPointVariable()) {
            if (al->nameId() == bl->nameId()) {
                return 0;
            } else {
                return al->nameId() < bl->nameId() ? -1 : 1;
            }
        }
        ASSERT_forbid(al->isFloatingPointVariable());
        ASSERT_forbid(bl->isFloatingPointVariable());

        ASSERT_require(al->isIntegerVariable());
        ASSERT_require(bl->isIntegerVariable());
        if (al->nameId() == bl->nameId()) {
            return 0;
        } else {
            return al->nameId() < bl->nameId() ? -1 : 1;
        }
    } else {
        // both are internal nodes
        ASSERT_not_null(ai);
        ASSERT_not_null(bi);
        if (ai->getOperator() != bi->getOperator())
            return ai->getOperator() < bi->getOperator() ? -1 : 1;
        for (size_t i=0; i<std::min(ai->nChildren(), bi->nChildren()); ++i) {
            if (int cmp = expr_cmp(ai->child(i), bi->child(i)))
                return cmp;
        }
        if (ai->nChildren() != bi->nChildren())
            return ai->nChildren() < bi->nChildren() ? -1 : 1;
        return 0;
    }
}

static bool
commutative_order(const Ptr &a, const Ptr &b)
{
    if (int cmp = expr_cmp(a, b))
        return cmp<0;
    return getRawPointer(a) < getRawPointer(b); // make it a strict ordering
}

InteriorPtr
Interior::commutative() {
    const Nodes &orig_operands = children();
    Nodes sorted_operands = orig_operands;
    std::sort(sorted_operands.begin(), sorted_operands.end(), commutative_order);
    if (std::equal(sorted_operands.begin(), sorted_operands.end(), orig_operands.begin()))
        return isInteriorNode();

    // construct the new node but don't simplify it yet (i.e., don't use Interior::create())
    Interior *inode = new Interior(type(), getOperator(), sorted_operands, comment(), 0);
    return InteriorPtr(inode);
}

InteriorPtr
Interior::idempotent(const SmtSolverPtr &solver) {
    Nodes newArgs;
    bool isSimplified = false;
    BOOST_FOREACH (const Ptr &arg, children()) {
        if (!newArgs.empty() && newArgs.back()->mustEqual(arg, solver)) {
            isSimplified = true;
        } else {
            newArgs.push_back(arg);
        }
    }

    if (isSimplified) {
        // Construct the new node but don't simplify it yet (i.e., don't use Interior::create())
        Interior *inode = new Interior(type(), getOperator(), newArgs, comment(), 0);
        return InteriorPtr(inode);
    } else {
        return sharedFromThis().dynamicCast<Interior>();
    }
}
        
Ptr
Interior::involutary() {
    if (InteriorPtr inode = isInteriorNode()) {
        if (1==inode->nChildren()) {
            if (InteriorPtr sub1 = inode->child(0)->isInteriorNode()) {
                if (sub1->getOperator() == inode->getOperator() && 1==sub1->nChildren()) {
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
Ptr
Interior::additiveNesting(const SmtSolverPtr &solver) {
    InteriorPtr nested = child(1)->isInteriorNode();
    if (nested!=NULL && nested->getOperator()==getOperator()) {
        ASSERT_require(nested->nChildren()==nChildren());
        ASSERT_require(nested->nBits()==nBits());
        size_t additive_nbits = std::max(child(0)->nBits(), nested->child(0)->nBits());

        // The two addends must be the same width, so zero-extend them if necessary (or should we sign extend?)
        // Note that the first argument (new width) of the UEXTEND operator is not actually used.
        Ptr a = child(0)->nBits()==additive_nbits ? child(0) :
                makeExtend(makeIntegerConstant(8, additive_nbits), child(0), solver);
        Ptr b = nested->child(0)->nBits()==additive_nbits ? nested->child(0) :
                makeExtend(makeIntegerConstant(8, additive_nbits), nested->child(0), solver);

        // construct the new node but don't simplify it yet (i.e., don't use Interior::create())
        Nodes newArgs;
        newArgs.push_back(makeAdd(a, b, solver));
        newArgs.push_back(nested->child(1));
        Interior *inode = new Interior(type(), getOperator(), newArgs, comment(), 0);
        return InteriorPtr(inode);
    }
    return isInteriorNode();
}

Ptr
Interior::identity(uint64_t ident, const SmtSolverPtr &solver) {
    Nodes args;
    bool modified = false;
    for (Nodes::const_iterator ci=children_.begin(); ci!=children_.end(); ++ci) {
        LeafPtr leaf = (*ci)->isLeafNode();
        if (leaf && leaf->isIntegerConstant()) {
            Sawyer::Container::BitVector identBv = Sawyer::Container::BitVector(leaf->nBits()).fromInteger(ident);
            if (0==leaf->bits().compare(identBv)) {
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
        return makeIntegerConstant(nBits(), ident, comment());
    if (1==args.size()) {
        if (args.front()->nBits()!=nBits())
            return makeExtend(makeIntegerConstant(8, nBits()), args.front(), solver);
        return args.front();
    }

    // Don't simplify the return value recursively
    Interior *inode = new Interior(Type::none(), getOperator(), args, comment(), 0);
    if (inode->nBits() != nBits()) {
        delete inode;
        return sharedFromThis();                        // don't simplify if width changed.
    }
    return InteriorPtr(inode);
}

Ptr
Interior::poisonNan(const SmtSolverPtr &solver) {
    BOOST_FOREACH (Ptr child, children()) {
        LeafPtr leaf = child->isLeafNode();
        if (leaf && leaf->isFloatingPointNan())
            return makeFloatingPointNan(leaf->type().exponentWidth(), leaf->type().significandWidth(), comment(), flags());
    }
    return sharedFromThis();
}

Ptr
Interior::unaryNoOp() {
    return 1==nChildren() ? child(0) : sharedFromThis();
}

Ptr
Interior::rewrite(const Simplifier &simplifier, const SmtSolverPtr &solver) {
    if (Ptr simplified = simplifier.rewrite(this, solver))
        return simplified;
    return sharedFromThis();
}

Ptr
Interior::foldConstants(const Simplifier &simplifier) {
    Nodes newOperands;
    bool modified = false;
    Nodes::const_iterator ci1 = children_.begin();
    while (ci1!=children_.end()) {
        Nodes::const_iterator ci2 = ci1;
        LeafPtr leaf;
        while (ci2 != children_.end() && (leaf = (*ci2)->isLeafNode()) && leaf->isIntegerConstant())
            ++ci2;
        if (ci1==ci2 || ci1+1==ci2) {                           // arg is not a constant, or we had only one constant by itself
            newOperands.push_back(*ci1);
            ++ci1;
        } else if (Ptr folded = simplifier.fold(ci1, ci2)) { // able to fold all these constants into a new node
            newOperands.push_back(folded);
            modified = true;
            ci1 = ci2;
        } else {                                                // multiple constants, but unable to fold
            newOperands.insert(newOperands.end(), ci1, ci2);
            ci1 = ci2;
        }
    }
    if (!modified)
        return isInteriorNode();
    if (1==newOperands.size())
        return newOperands.front();

    // Do not simplify again (i.e., don't use Interior::create())
    return InteriorPtr(new Interior(type(), op_, newOperands, comment(), 0));
}

Ptr
AddSimplifier::fold(Nodes::const_iterator begin, Nodes::const_iterator end) const {
    Sawyer::Container::BitVector accumulator((*begin)->nBits());
    unsigned flags = 0;
    for (/*void*/; begin!=end; ++begin) {
        accumulator.add((*begin)->isLeafNode()->bits());
        flags |= (*begin)->flags();
    }
    return makeIntegerConstant(accumulator, "", flags);
}

Ptr
AddSimplifier::rewrite(Interior *inode, const SmtSolverPtr &solver) const {
    // Simplify these expressions by hoisting the ite (SEI):
    //   (add (ite C X Y) Z)
    //   (add Z (ite C X Y)
    // where Z is not an ite expression,
    // to this expression:
    //   (ite C (add X Z) (add Y Z))
    // and simplify the add operations
    if (inode->nChildren() == 2) {
        if (InteriorPtr ite = inode->child(0)->isOperator(OP_ITE)) {
            // (add (ite C X Y) Z) => (ite (add X Z) (add Y Z))
            if (!inode->child(1)->isOperator(OP_ITE)) {
                return makeIte(ite->child(0),
                               makeAdd(ite->child(1), inode->child(1), solver),
                               makeAdd(ite->child(2), inode->child(1), solver),
                               solver);
            }
        } else if (InteriorPtr ite = inode->child(1)->isOperator(OP_ITE)) {
            // (add Z (ite C X Y)) => (ite (add Z X) (add Z Y))
            ASSERT_forbid(inode->child(0)->isOperator(OP_ITE));
            return makeIte(ite->child(0),
                           makeAdd(inode->child(0), ite->child(1), solver),
                           makeAdd(inode->child(0), ite->child(2), solver),
                           solver);
        }
    }

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
        bool operator()(Ptr a, Ptr b, Sawyer::Container::BitVector &adjustment/*in,out*/) {
            ASSERT_not_null(a);
            ASSERT_not_null(b);
            ASSERT_require(a->nBits()==b->nBits());

            // swap A and B if necessary so we have form (1) or (2).
            if (b->isInteriorNode()==NULL)
                std::swap(a, b);
            if (b->isInteriorNode()==NULL)
                return false;

            InteriorPtr bi = b->isInteriorNode();
            if (bi->getOperator()==OP_NEGATE) {
                // form (3)
                ASSERT_require(1==bi->nChildren());
                return a->isEquivalentTo(bi->child(0));
            } else if (bi->getOperator()==OP_INVERT) {
                // form (4) and ninverts is small enough
                if (a->isEquivalentTo(bi->child(0))) {
                    adjustment.decrement();
                    return true;
                }
            }
            return false;
        }
    };

    // Rewrite (add ... (negate (add a b)) ...) => (add ... (negate a) (negate b) ...)
    struct distributeNegations {
        Ptr operator()(Interior *add, const SmtSolverPtr &solver) {
            Nodes children;
            bool distributed = false;
            for (size_t i=0; i<add->nChildren(); ++i) {
                bool pushed = false;
                InteriorPtr addArg = add->child(i)->isInteriorNode();
                if (addArg && addArg->getOperator()==OP_NEGATE && addArg->nChildren()==1) {
                    if (InteriorPtr negateArg = addArg->child(0)->isInteriorNode()) {
                        if (negateArg && negateArg->getOperator()==OP_ADD && negateArg->nChildren()>0) {
                            for (size_t j=0; j<negateArg->nChildren(); ++j)
                                children.push_back(makeNegate(negateArg->child(j), solver));
                            pushed = distributed = true;
                        }
                    }
                }
                if (!pushed)
                    children.push_back(add->child(i));
            }
            if (!distributed)
                return Ptr();
            return Interior::instance(OP_ADD, children, solver, add->comment());
        }
    };

    // Arguments that are negated cancel out similar arguments that are not negated
    bool had_duals = false;
    Sawyer::Container::BitVector adjustment(inode->nBits());
    Nodes children = inode->children();
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

    // Otherwise distribute negations across adds:
    //   (add ... (negate (add a b)) ...) => (add ... (negate a) (negate b) ...)
    if (!had_duals) {
        if (Ptr distributed = distributeNegations()(inode, solver))
            return distributed;
        return Ptr();
    }

    // Build the new expression
    children.erase(std::remove(children.begin(), children.end(), Ptr()), children.end());
    if (!adjustment.isEqualToZero())
        children.push_back(makeIntegerConstant(adjustment));
    if (children.empty())
        return makeIntegerConstant(inode->nBits(), 0, inode->comment());
    if (children.size()==1)
        return children[0];
    return Interior::instance(OP_ADD, children, solver, inode->comment());
}

Ptr
AndSimplifier::fold(Nodes::const_iterator begin, Nodes::const_iterator end) const {
    Sawyer::Container::BitVector accumulator((*begin)->nBits(), true);
    unsigned flags = 0;
    for (/*void*/; begin!=end; ++begin) {
        accumulator.bitwiseAnd((*begin)->isLeafNode()->bits());
        flags |= (*begin)->flags();
    }
    return makeIntegerConstant(accumulator, "", flags);
}

Ptr
AndSimplifier::rewrite(Interior *inode, const SmtSolverPtr &solver) const {
    // Result is zero if any argument is zero
    for (size_t i=0; i<inode->nChildren(); ++i) {
        LeafPtr child = inode->child(i)->isLeafNode();
        if (child && child->isIntegerConstant() && child->bits().isEqualToZero())
            return makeIntegerConstant(inode->nBits(), 0, inode->comment(), child->flags());
    }

    // (and X X) => X (for any number of arguments that are all the same)
    bool allSameArgs = true;
    for (size_t i=1; i<inode->nChildren() && allSameArgs; ++i) {
        if (!inode->child(0)->isEquivalentTo(inode->child(i)))
            allSameArgs = false;
    }
    if (allSameArgs)
        return inode->child(0);

    return Ptr();
}

Ptr
OrSimplifier::fold(Nodes::const_iterator begin, Nodes::const_iterator end) const {
    Sawyer::Container::BitVector accumulator((*begin)->nBits());
    unsigned flags = 0;
    for (/*void*/; begin!=end; ++begin) {
        accumulator.bitwiseOr((*begin)->isLeafNode()->bits());
        flags |= (*begin)->flags();
    }
    return makeIntegerConstant(accumulator, "", flags);
}

Ptr
OrSimplifier::rewrite(Interior *inode, const SmtSolverPtr &solver) const {
    // Result has all bits set if any argument has all bits set.
    // Zeros have no effect on the result
    std::vector<bool> removed(inode->nChildren(), false);
    bool modified = false;
    for (size_t i=0; i<inode->nChildren(); ++i) {
        LeafPtr child = inode->child(i)->isLeafNode();
        if (child && child->isIntegerConstant()) {
            if (child->bits().isEqualToZero()) {
                removed[i] = modified = true;
            } else if (child->bits().isAllSet()) {
                return makeIntegerConstant(child->bits(), inode->comment(), child->flags());
            }
        }
    }
    Nodes newargs;
    for (size_t i=0; i<inode->nChildren(); ++i) {
        if (!removed[i])
            newargs.push_back(inode->child(i));
    }

    // If we removed all the arguments, return 0. I.e., (or 0) => (or) => 0
    if (newargs.empty())
        return makeIntegerConstant(inode->nBits(), 0, inode->comment());

    // If there's only one argument left, return it. I.e., (or 0 x 0) => (or x) => x
    if (newargs.size() == 1)
        return newargs[0];

    // Return original or modified expression
    if (!modified)
        return Ptr();
    return Interior::instance(inode->getOperator(), newargs, solver, inode->comment());
}

Ptr
XorSimplifier::fold(Nodes::const_iterator begin, Nodes::const_iterator end) const {
    Sawyer::Container::BitVector accumulator((*begin)->isLeafNode()->bits());
    unsigned flags = 0;
    for (++begin; begin!=end; ++begin) {
        accumulator.bitwiseXor((*begin)->isLeafNode()->bits());
        flags |= (*begin)->flags();
    }
    return makeIntegerConstant(accumulator, "", flags);
}

Ptr
XorSimplifier::rewrite(Interior *inode, const SmtSolverPtr &solver) const {
    // If any pairs of arguments are equal, then they don't contribute to the final answer.
    // If any argument is zero, then it doesn't contribute to the final answer.
    std::vector<bool> removed(inode->nChildren(), false);
    bool modified = false;
    for (size_t i=0; i<inode->nChildren(); ++i) {
        if (removed[i])
            continue;
        LeafPtr leaf = inode->child(i)->isLeafNode();
        if (leaf && leaf->isIntegerConstant() && leaf->bits().isEqualToZero()) {
            removed[i] = modified = true;
        } else {
            for (size_t j=i+1; j<inode->nChildren(); ++j) {
                if (!removed[j] && inode->child(i)->mustEqual(inode->child(j), solver)) {
                    removed[i] = removed[j] = modified = true;
                    break;
                }
            }
        }
    }
    Nodes newargs;
    for (size_t i=0; i<inode->nChildren(); ++i) {
        if (!removed[i])
            newargs.push_back(inode->child(i));
    }

    // If we removed all the arguments, return 0. I.e., (xor X X) = (xor) = 0
    if (newargs.empty())
        return makeIntegerConstant(inode->nBits(), 0, inode->comment());

    // If there's only one argument left, return it. I.e., (xor X) == (xor X Y Y) == (xor X 0) == X
    if (newargs.size() == 1)
        return newargs[0];

    // Return original or modified expression
    if (!modified)
        return Ptr();
    return Interior::instance(inode->getOperator(), newargs, solver, inode->comment());
}

Ptr
SmulSimplifier::fold(Nodes::const_iterator begin, Nodes::const_iterator end) const {
    // FIXME[Robb P. Matzke 2014-05-05]: Constant folding is not currently possible when the operands are wider than 64 bits
    // because Sawyer::Container::BitVector does not provide a multiplication method.
    size_t totalWidth = 0;
    int64_t product = 1;
    unsigned flags = 0;
    for (/*void*/; begin!=end; ++begin) {
        size_t nbits = (*begin)->nBits();
        totalWidth += nbits;
        if (totalWidth > 8*sizeof(product))
            return Ptr();
        LeafPtr leaf = (*begin)->isLeafNode();
        product *= leaf->toSigned().get();
        flags |= (*begin)->flags();
    }
    return makeIntegerConstant(totalWidth, product, "", flags);
}

Ptr
UmulSimplifier::fold(Nodes::const_iterator begin, Nodes::const_iterator end) const {
    // FIXME[Robb P. Matzke 2014-05-05]: Constant folding is not currently possible when the operands are wider than 64 bits
    // because Sawyer::Container::BitVector does not provide a multiplication method.
    size_t totalWidth = 0;
    uint64_t product = 1;
    unsigned flags = 0;
    for (/*void*/; begin!=end; ++begin) {
        size_t nbits = (*begin)->nBits();
        totalWidth += nbits;
        if (totalWidth > 8*sizeof(product))
            return Ptr();
        LeafPtr leaf = (*begin)->isLeafNode();
        product *= leaf->toUnsigned().get();
        flags |= (*begin)->flags();
    }
    return makeIntegerConstant(totalWidth, product, "", flags);
}

Ptr
ConcatSimplifier::fold(Nodes::const_iterator begin, Nodes::const_iterator end) const {
    // first arg is high-order bits. Although this is nice to look at, it makes the operation a bit more difficult.
    size_t resultSize = 0;
    for (Nodes::const_iterator ti=begin; ti!=end; ++ti)
        resultSize += (*ti)->nBits();
    Sawyer::Container::BitVector accumulator(resultSize);

    // Copy bits into wherever they belong in the accumulator
    unsigned flags = 0;
    for (size_t sa=resultSize; begin!=end; ++begin) {
        LeafPtr leaf = (*begin)->isLeafNode();
        sa -= leaf->nBits();
        BitRange destination = BitRange::baseSize(sa, leaf->nBits());
        accumulator.copy(destination, leaf->bits(), leaf->bits().hull());
        flags |= (*begin)->flags();
    }
    return makeIntegerConstant(accumulator, "", flags);
}

Ptr
ConcatSimplifier::rewrite(Interior *inode, const SmtSolverPtr &solver) const {
    // Hoist "ite" if possible. In other words, simplify this:
    //   (concat (ite C X1 Y1) ... (ite C Xn Yn))
    // to this:
    //   (ite C (concat X1 ... Xn) (concat Y1 ... Yn))
    // and simplify the concat operations
    Ptr condition;
    Nodes trueValues, falseValues;
    BOOST_FOREACH (const Ptr &child, inode->children()) {
        InteriorPtr ite = child->isInteriorNode();
        if (!ite || ite->getOperator() != OP_ITE) {
            condition = Ptr();
            break;
        } else if (!condition) {
            condition = ite->child(0);
        } else if (!ite->child(0)->mustEqual(condition, solver)) {
            condition = Ptr();
            break;
        }
        trueValues.push_back(ite->child(1));
        falseValues.push_back(ite->child(2));
    }
    if (condition) {
        Ptr trueConcat = Interior::instance(OP_CONCAT, trueValues, solver);
        Ptr falseConcat = Interior::instance(OP_CONCAT, falseValues, solver);
        return Interior::instance(OP_ITE, condition, trueConcat, falseConcat, solver);
    }

    // If all the concatenated expressions are extract expressions, all extracting bits from the same expression and
    // in the correct order, then we can simplify this to that expression.  For instance:
    //   (concat[32]
    //       (extract[8] 24[32] 32[32] v2[32])
    //       (extract[8] 16[32] 24[32] v2[32])
    //       (extract[8] 8[32] 16[32] v2[32])
    //       (extract[8] 0[32] 8[32] v2[32]))
    // can be simplified to
    //   v2
    //
    // What follows is an even better implementation that looks at subsequences of arguments instead of all arguments at
    // once. For instance:
    //   (concat[32]
    //       0x00[5]
    //       (extract[19] 0x0d[32] 0x20[32] v2[32])
    //       (extract[8] 0x05[32] 0x0d[32] v2[32]))
    // can be simplified to:
    //   (concat[32]
    //       0x00[5]
    //       (extract[27] 0x05[32] 0x20[32] v2[32]))
    Nodes newArgs;
    InteriorPtr isPrevExtract;                          // is newArgs.back a suitable (extract X Y X)?
    rose_addr_t prevLoOffset = 0;                          // valid only if isPrevExtract non-null
    for (size_t argno=0; argno<inode->nChildren(); ++argno) {

        // Does the argument have the form (extract X Y EXPR) where X and Y are known integers. If so, make isExtract point to
        // this argument.
        InteriorPtr isCurExtract = inode->child(argno)->isInteriorNode();
        rose_addr_t curLoOffset=0, curHiOffset=0;
        if (isCurExtract!=NULL && isCurExtract->getOperator()==OP_EXTRACT &&
            isCurExtract->child(0)->nBits() <= 8*sizeof(prevLoOffset) &&
            isCurExtract->child(1)->nBits() <= 8*sizeof(prevLoOffset) &&
            isCurExtract->child(0)->isLeafNode() && isCurExtract->child(0)->isLeafNode()->isIntegerConstant() &&
            isCurExtract->child(1)->isLeafNode() && isCurExtract->child(1)->isLeafNode()->isIntegerConstant()) {
            curLoOffset = isCurExtract->child(0)->isLeafNode()->toUnsigned().get();
            curHiOffset = isCurExtract->child(1)->isLeafNode()->toUnsigned().get();
            ASSERT_require(curLoOffset < curHiOffset);
        } else {
            isCurExtract = InteriorPtr();
        }

        // Can this argument be joined with the previous one?
        if (isPrevExtract && isCurExtract && curHiOffset==prevLoOffset &&
            isPrevExtract->child(2)->mustEqual(isCurExtract->child(2), solver)) {
            newArgs.back() = makeExtract(isCurExtract->child(0), isPrevExtract->child(1), isCurExtract->child(2), solver);
            isPrevExtract = newArgs.back()->isInteriorNode(); // merged arg is still a valid extract expression
        } else {
            newArgs.push_back(inode->child(argno));
            isPrevExtract = isCurExtract;
        }
        prevLoOffset = curLoOffset;                     // valid only if isPrevExtract is non-null
    }

    // Identity
    // (concat x) => x (flags from both)
    if (newArgs.size() == 1)
        return newArgs[0]->newFlags(inode->flags());

    // Construct a new, simplified expression
    if (newArgs.size() == inode->nChildren())
        return Ptr();                                   // no simplification possible
    return Interior::instance(inode->getOperator(), newArgs, solver, inode->comment(), inode->flags());
}

Ptr
ExtractSimplifier::rewrite(Interior *inode, const SmtSolverPtr &solver) const {
    LeafPtr from_node = inode->child(0)->isLeafNode();
    LeafPtr to_node   = inode->child(1)->isLeafNode();
    Ptr operand   = inode->child(2);
    ASSERT_require(!from_node->isIntegerConstant() || from_node->nBits() <= 8*sizeof(size_t));
    ASSERT_require(!to_node->isIntegerConstant()   || to_node->nBits() <= 8*sizeof(size_t));
    size_t from = from_node && from_node->isIntegerConstant() ? from_node->toUnsigned().get() : 0;
    size_t to = to_node && to_node->isIntegerConstant() ? to_node->toUnsigned().get() : 0;

    // If limits are backward or extend beyond the operand size, don't simplify
    if (from_node && to_node && from_node->isIntegerConstant() && to_node->isIntegerConstant() && (from>=to || to>operand->nBits()))
        return Ptr();

    // Constant folding
    if (from_node && to_node && from_node->isIntegerConstant() && to_node->isIntegerConstant() &&
        operand->isLeafNode() && operand->isLeafNode()->isIntegerConstant()) {
        Sawyer::Container::BitVector result(to-from);
        BitRange source = BitRange::hull(from, to-1);
        result.copy(result.hull(), operand->isLeafNode()->bits(), source);
        return makeIntegerConstant(result, inode->comment(), inode->flags());
    }

    // Extracting the whole thing is a no-op
    if (from_node && to_node && from_node->isIntegerConstant() && from==0 && to==operand->nBits())
        return operand;

    // Hoist concat operations to the outside of the extract
    // If the operand is a concat operation then take only the parts we need.  Some examples:
    // (extract 0 24 (concat X[24] Y[8]))  ==> (concat (extract 0 16 X) Y)
    Nodes newChildren;
    InteriorPtr ioperand = operand->isInteriorNode();
    if (from_node && to_node && from_node->isIntegerConstant() && to_node->isIntegerConstant() &&
        ioperand && OP_CONCAT==ioperand->getOperator()) {
        size_t partAt = 0;                              // starting bit number in child
        BOOST_REVERSE_FOREACH (const Ptr part, ioperand->children()) { // concatenated parts
            size_t partEnd = partAt + part->nBits();
            if (partEnd <= from) {
                // Part is entirely left of what we need
                partAt = partEnd;
            } else if (partAt >= to) {
                // Part is entirely right of what we need
                break;
            } else if (partAt < from && partEnd > to) {
                // We need the middle of this part, and then we're done
                size_t need = to-from;                  // number of bits we need
                newChildren.push_back(makeExtract(makeIntegerConstant(32, from-partAt), makeIntegerConstant(32, to-partAt), part, solver));
                partAt = partEnd;
                from += need;
            } else if (partAt < from) {
                // We need the end of the part
                ASSERT_require(partEnd <= to);
                size_t need = partEnd - from;
                newChildren.push_back(makeExtract(makeIntegerConstant(32, from-partAt), makeIntegerConstant(32, part->nBits()), part, solver));
                partAt = partEnd;
                from += need;
            } else if (partEnd > to) {
                // We need the beginning of the part
                ASSERT_require(partAt == from);
                size_t need = to-from;
                newChildren.push_back(makeExtract(makeIntegerConstant(32, 0), makeIntegerConstant(32, need), part, solver));
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
            return Interior::instance(OP_CONCAT, newChildren, solver, inode->comment());
        }
        newChildren[0]->comment(inode->comment());
        return newChildren[0];
    }

    // If the operand is another extract operation and we know all the limits then they can be replaced with a single extract.
    if (from_node && to_node && from_node->isIntegerConstant() && to_node->isIntegerConstant() &&
        ioperand && OP_EXTRACT==ioperand->getOperator()) {
        LeafPtr from2_node = ioperand->child(0)->isLeafNode();
        LeafPtr to2_node = ioperand->child(1)->isLeafNode();
        if (from2_node && to2_node && from2_node->isIntegerConstant() && to2_node->isIntegerConstant()) {
            size_t from2 = from2_node->toUnsigned().get();
            return makeExtract(makeIntegerConstant(32, from2+from), makeIntegerConstant(32, from2+to), ioperand->child(2),
                               solver, inode->comment());
        }
    }

    // Simplifications for (extract 0 a (uextend b c))
    if (from_node && to_node && from_node->isIntegerConstant() && 0==from && to_node->isIntegerConstant()) {
        size_t a=to, b=operand->nBits();
        // (extract[a] 0 a (uextend[b] b c[a])) => c when b>=a
        if (ioperand && OP_UEXTEND==ioperand->getOperator() && b>=a && ioperand->child(1)->nBits()==a)
            return ioperand->child(1);
    }


    return Ptr();
}

Ptr
AsrSimplifier::rewrite(Interior *inode, const SmtSolverPtr &solver) const {
    ASSERT_require(2==inode->nChildren());

    // Constant folding
    LeafPtr shift_leaf   = inode->child(0)->isLeafNode();
    LeafPtr operand_leaf = inode->child(1)->isLeafNode();
    if (shift_leaf!=NULL && operand_leaf!=NULL && shift_leaf->isIntegerConstant() && operand_leaf->isIntegerConstant()) {
        size_t sa = shift_leaf->toUnsigned().get();
        Sawyer::Container::BitVector result = operand_leaf->bits();
        result.shiftRightArithmetic(sa);
        return makeIntegerConstant(result, inode->comment(), inode->flags());
    }
    return Ptr();
}

Ptr
InvertSimplifier::rewrite(Interior *inode, const SmtSolverPtr &solver) const {
    // Constant folding
    LeafPtr operand_node = inode->child(0)->isLeafNode();
    if (operand_node==NULL || !operand_node->isIntegerConstant())
        return Ptr();
    Sawyer::Container::BitVector result = operand_node->bits();
    result.invert();
    return makeIntegerConstant(result, inode->comment(), inode->flags());
}

Ptr
NegateSimplifier::rewrite(Interior *inode, const SmtSolverPtr &solver) const {
    // Constant folding
    LeafPtr operand_node = inode->child(0)->isLeafNode();
    if (operand_node==NULL || !operand_node->isIntegerConstant())
        return Ptr();
    Sawyer::Container::BitVector result = operand_node->bits();
    result.negate();
    return makeIntegerConstant(result, inode->comment(), inode->flags());
}

Ptr
IteSimplifier::rewrite(Interior *inode, const SmtSolverPtr &solver) const {
    // Is the condition known?
    LeafPtr cond_node = inode->child(0)->isLeafNode();
    if (cond_node!=NULL && cond_node->isIntegerConstant()) {
        if (cond_node->nBits() != 1)
            throw Exception(toStr(inode->getOperator()) + " operator's first argument (condition) should be one bit wide");
        bool condition = ! cond_node->bits().isAllClear();
        return condition ? inode->child(1) : inode->child(2);
    }

    // Are both operands the same? Then the condition doesn't matter
    if (inode->child(1)->isEquivalentTo(inode->child(2)))
        return inode->child(1);

    // Are both extracts of the same offsets?
    // convert this: (ite cond (extract x y expr1) (extract x y expr2))
    // to this:      (extract x y (ite cond expr1 expr2))
    InteriorPtr in1 = inode->child(1)->isInteriorNode();
    InteriorPtr in2 = inode->child(2)->isInteriorNode();
    if (in1 && in2 &&
        in1->getOperator() == OP_EXTRACT && in2->getOperator() == OP_EXTRACT &&
        in1->nBits() == in2->nBits() &&
        in1->child(0)->mustEqual(in2->child(0), solver) &&      // both extracts have same "from" bit offset
        in1->child(1)->mustEqual(in2->child(1), solver) &&      // both extracts have same "to" bit offset
        in1->child(2)->nBits() == in2->child(2)->nBits()) {     // both extracted-from values are same width

        Ptr newIte = makeIte(inode->child(0), in1->child(2), in2->child(2), solver);
        Ptr newExtract = makeExtract(in1->child(0), in1->child(1), newIte, solver);
        return newExtract;
    }

    // Convert a negative condition to a positive condition
    //   (ite (invert X) A B) => (ite X B A)
    if (InteriorPtr invert = inode->child(0)->isOperator(OP_INVERT))
        return makeIte(invert->child(0), inode->child(2), inode->child(1), solver, inode->comment(), inode->flags());

    return Ptr();
}

Ptr
NoopSimplifier::rewrite(Interior *inode, const SmtSolverPtr &solver) const {
    if (1==inode->nChildren())
        return inode->child(0);
    return Ptr();
}

template<typename T>
static Sawyer::Optional<size_t>
isPowerOfTwo(T n) {
    for (size_t i=0; i<8*sizeof(T); ++i) {
        if (T(1) << i == n)
            return i;
    }
    return Sawyer::Nothing();
}

Ptr
RolSimplifier::rewrite(Interior *inode, const SmtSolverPtr &solver) const {
    LeafPtr sa_leaf = inode->child(0)->isLeafNode();
    LeafPtr val_leaf = inode->child(1)->isLeafNode();

    // Get the effective shift amount
    Sawyer::Optional<uint64_t> sa;
    if (sa_leaf && sa_leaf->isIntegerConstant()) {
        if (sa_leaf->nBits() <= 64) {
            sa = sa_leaf->toUnsigned();
        } else {
            Sawyer::Optional<size_t> msbSetIdx = sa_leaf->bits().mostSignificantSetBit();
            size_t twoPow = 0;
            if (!msbSetIdx) {
                sa = 0;
            } else if (*msbSetIdx < 64) {
                // Only low-order bits are set, so we can truncate to 64 bits.
                sa = sa_leaf->bits().toInteger();
            } else if (isPowerOfTwo(inode->nBits()).assignTo(twoPow)) {
                // Shift count modulo number of bits in result.
                sa = sa_leaf->bits().toInteger(BitRange::hull(0, twoPow));
            }
        }
    }

    // Constant folding
    if (val_leaf && val_leaf->isIntegerConstant() && sa) {
        Sawyer::Container::BitVector result = val_leaf->bits();
        result.rotateLeft(*sa);
        return makeIntegerConstant(result, inode->comment(), inode->flags());
    }

    // If the shift amount is zero then this is a no-op
    if (sa && *sa % inode->nBits() == 0)
        return inode->child(1);

    // If the shift amount is greater than the value width, replace it with modulo
    if (sa && *sa > inode->nBits()) {
        return Interior::instance(inode->getOperator(),
                                  makeIntegerConstant(inode->child(0)->nBits(), *sa % inode->nBits(),
                                                      inode->child(0)->comment(), inode->child(0)->flags()),
                                  inode->child(1),
                                  solver, inode->comment(), inode->flags());
    }

    return Ptr();
}
Ptr
RorSimplifier::rewrite(Interior *inode, const SmtSolverPtr &solver) const {
    LeafPtr sa_leaf = inode->child(0)->isLeafNode();
    LeafPtr val_leaf = inode->child(1)->isLeafNode();

    // Get the effective shift amount
    Sawyer::Optional<uint64_t> sa;
    if (sa_leaf && sa_leaf->isIntegerConstant()) {
        if (sa_leaf->nBits() <= 64) {
            sa = sa_leaf->toUnsigned();
        } else {
            Sawyer::Optional<size_t> msbSetIdx = sa_leaf->bits().mostSignificantSetBit();
            size_t twoPow = 0;
            if (!msbSetIdx) {
                sa = 0;
            } else if (*msbSetIdx < 64) {
                // Only low-order bits are set, so we can truncate to 64 bits.
                sa = sa_leaf->bits().toInteger();
            } else if (isPowerOfTwo(inode->nBits()).assignTo(twoPow)) {
                // Shift count modulo number of bits in result.
                sa = sa_leaf->bits().toInteger(BitRange::hull(0, twoPow));
            }
        }
    }

    // Constant folding
    if (val_leaf && val_leaf->isIntegerConstant() && sa) {
        Sawyer::Container::BitVector result = val_leaf->bits();
        result.rotateRight(*sa);
        return makeIntegerConstant(result, inode->comment(), inode->flags());
    }

    // If the shift amount is zero then this is a no-op
    if (sa && *sa % inode->nBits() == 0)
        return inode->child(1);

    // If the shift amount is greater than the value width, replace it with modulo
    if (sa && *sa > inode->nBits()) {
        return Interior::instance(inode->getOperator(),
                                  makeIntegerConstant(inode->child(0)->nBits(), *sa % inode->nBits(),
                                                      inode->child(0)->comment(), inode->child(0)->flags()),
                                  inode->child(1),
                                  solver, inode->comment(), inode->flags());
    }

    return Ptr();
}

Ptr
UextendSimplifier::rewrite(Interior *inode, const SmtSolverPtr &solver) const {
    // Noop case
    size_t oldsize = inode->child(1)->nBits();
    size_t newsize = inode->nBits();

    // Identity
    // (uextend newsize arg[oldsize])
    //   and newsize = oldsize
    // => arg[oldsize]
    if (oldsize == newsize)
        return inode->child(1);

    // Constant folding
    LeafPtr val_leaf = inode->child(1)->isLeafNode();
    if (val_leaf && val_leaf->isIntegerConstant()) {
        Sawyer::Container::BitVector result = val_leaf->bits();
        result.resize(newsize);
        return makeIntegerConstant(result, inode->comment(), inode->flags());
    }

    // Extending an extend
    // (uextend newsize (uextend oldsize arg[k]))
    //   and newsize >= oldsize
    //   and oldsize >= k
    // => (uextend newsize arg[k])
    InteriorPtr arg = inode->child(1)->isInteriorNode();
    if (arg && arg->getOperator() == OP_UEXTEND && newsize >= oldsize && oldsize >= arg->child(1)->nBits())
        return makeExtend(inode->child(0), arg->child(1), solver, inode->comment());

    // Shrinking an extend
    // (uextend newsize (uextend oldsize arg[k]))
    //   and k <= newsize <= oldsize
    // => (uextend newsize arg[k])
    if (arg && arg->getOperator() == OP_UEXTEND && newsize >= arg->child(1)->nBits() && newsize <= oldsize)
        return makeExtend(inode->child(0), arg->child(1), solver, inode->comment());
    
    // Shrinking
    // (uextend newsize arg[oldsize])
    //   and newsize < oldsize
    // => (extract 0 newsize arg[oldsize])
    if (newsize < oldsize)
        return makeExtract(makeIntegerConstant(32, 0), makeIntegerConstant(32, newsize), inode->child(1), solver, inode->comment());

    return Ptr();
}

Ptr
SextendSimplifier::rewrite(Interior *inode, const SmtSolverPtr &solver) const {
    // Noop case
    size_t oldsize = inode->child(1)->nBits();
    size_t newsize = inode->nBits();
    if (oldsize==newsize)
        return inode->child(1);

    // Constant folding
    LeafPtr val_leaf = inode->child(1)->isLeafNode();
    if (val_leaf && val_leaf->isIntegerConstant()) {
        Sawyer::Container::BitVector result(inode->nBits());
        result.signExtend(val_leaf->bits());
        return makeIntegerConstant(result, inode->comment(), inode->flags());
    }

    // Downsizing should be represented as an extract operation
    if (newsize < oldsize) {
        return makeExtract(makeIntegerConstant(32, 0), makeIntegerConstant(32, newsize), inode->child(1), solver, inode->comment());
    }

    return Ptr();
}

Ptr
EqSimplifier::rewrite(Interior *inode, const SmtSolverPtr &solver) const {
    // Constant folding
    LeafPtr a_leaf = inode->child(0)->isLeafNode();
    LeafPtr b_leaf = inode->child(1)->isLeafNode();
    if (a_leaf && b_leaf && a_leaf->isIntegerConstant() && b_leaf->isIntegerConstant()) {
        int cmp = a_leaf->bits().compare(b_leaf->bits());
        return makeBooleanConstant(0==cmp, inode->comment(), inode->flags());
    }

    // (eq x x) => 1
    if (inode->child(0)->mustEqual(inode->child(1), SmtSolverPtr()))
        return makeBooleanConstant(true, inode->comment(), inode->flags());

    return Ptr();
}

Ptr
SgeSimplifier::rewrite(Interior *inode, const SmtSolverPtr &solver) const {
    // Constant folding
    LeafPtr a_leaf = inode->child(0)->isLeafNode();
    LeafPtr b_leaf = inode->child(1)->isLeafNode();
    if (a_leaf && b_leaf && a_leaf->isIntegerConstant() && b_leaf->isIntegerConstant()) {
        int cmp = a_leaf->bits().compareSigned(b_leaf->bits());
        return makeBooleanConstant(cmp>=0, inode->comment(), inode->flags());
    }

    // (sge x x) => 1
    if (inode->child(0)->mustEqual(inode->child(1), SmtSolverPtr()))
        return makeBooleanConstant(true, inode->comment(), inode->flags());

    return Ptr();
}

Ptr
SgtSimplifier::rewrite(Interior *inode, const SmtSolverPtr &solver) const {
    // Constant folding
    LeafPtr a_leaf = inode->child(0)->isLeafNode();
    LeafPtr b_leaf = inode->child(1)->isLeafNode();
    if (a_leaf && b_leaf && a_leaf->isIntegerConstant() && b_leaf->isIntegerConstant()) {
        int cmp = a_leaf->bits().compareSigned(b_leaf->bits());
        return makeBooleanConstant(cmp>0, inode->comment(), inode->flags());
    }

    return Ptr();
}

Ptr
SleSimplifier::rewrite(Interior *inode, const SmtSolverPtr &solver) const {
    // Constant folding
    LeafPtr a_leaf = inode->child(0)->isLeafNode();
    LeafPtr b_leaf = inode->child(1)->isLeafNode();
    if (a_leaf && b_leaf && a_leaf->isIntegerConstant() && b_leaf->isIntegerConstant()) {
        int cmp = a_leaf->bits().compareSigned(b_leaf->bits());
        return makeBooleanConstant(cmp<=0, inode->comment(), inode->flags());
    }

    // (sle x x) => 1
    if (inode->child(0)->mustEqual(inode->child(1), SmtSolverPtr()))
        return makeBooleanConstant(true, inode->comment(), inode->flags());

    return Ptr();
}

Ptr
SltSimplifier::rewrite(Interior *inode, const SmtSolverPtr &solver) const {
    // Constant folding
    LeafPtr a_leaf = inode->child(0)->isLeafNode();
    LeafPtr b_leaf = inode->child(1)->isLeafNode();
    if (a_leaf && b_leaf && a_leaf->isIntegerConstant() && b_leaf->isIntegerConstant()) {
        int cmp = a_leaf->bits().compareSigned(b_leaf->bits());
        return makeBooleanConstant(cmp<0, inode->comment(), inode->flags());
    }

    return Ptr();
}

Ptr
UgeSimplifier::rewrite(Interior *inode, const SmtSolverPtr &solver) const {
    // Constant folding
    LeafPtr a_leaf = inode->child(0)->isLeafNode();
    LeafPtr b_leaf = inode->child(1)->isLeafNode();
    if (a_leaf && b_leaf && a_leaf->isIntegerConstant() && b_leaf->isIntegerConstant()) {
        int cmp = a_leaf->bits().compare(b_leaf->bits());
        return makeBooleanConstant(cmp>=0, inode->comment(), inode->flags());
    }

    // (uge x x) => 1
    if (inode->child(0)->mustEqual(inode->child(1), SmtSolverPtr()))
        return makeBooleanConstant(true, inode->comment(), inode->flags());

   return Ptr();
}

Ptr
UgtSimplifier::rewrite(Interior *inode, const SmtSolverPtr &solver) const {
    // Constant folding
    LeafPtr a_leaf = inode->child(0)->isLeafNode();
    LeafPtr b_leaf = inode->child(1)->isLeafNode();
    if (a_leaf && b_leaf && a_leaf->isIntegerConstant() && b_leaf->isIntegerConstant()) {
        int cmp = a_leaf->bits().compare(b_leaf->bits());
        return makeBooleanConstant(cmp>0, inode->comment(), inode->flags());
    }

    return Ptr();
}

Ptr
UleSimplifier::rewrite(Interior *inode, const SmtSolverPtr &solver) const {
    // Constant folding
    LeafPtr a_leaf = inode->child(0)->isLeafNode();
    LeafPtr b_leaf = inode->child(1)->isLeafNode();
    if (a_leaf && b_leaf && a_leaf->isIntegerConstant() && b_leaf->isIntegerConstant()) {
        int cmp = a_leaf->bits().compare(b_leaf->bits());
        return makeBooleanConstant(cmp<=0, inode->comment(), inode->flags());
    }

    // (ule x x) => 1
    if (inode->child(0)->mustEqual(inode->child(1), SmtSolverPtr()))
        return makeBooleanConstant(true, inode->comment(), inode->flags());

    return Ptr();
}

Ptr
UltSimplifier::rewrite(Interior *inode, const SmtSolverPtr &solver) const {
    // Constant folding
    LeafPtr a_leaf = inode->child(0)->isLeafNode();
    LeafPtr b_leaf = inode->child(1)->isLeafNode();
    if (a_leaf && b_leaf && a_leaf->isIntegerConstant() && b_leaf->isIntegerConstant()) {
        int cmp = a_leaf->bits().compare(b_leaf->bits());
        return makeBooleanConstant(cmp<0, inode->comment(), inode->flags());
    }

    return Ptr();
}

Ptr
ZeropSimplifier::rewrite(Interior *inode, const SmtSolverPtr &solver) const {
    // Constant folding
    LeafPtr a_leaf = inode->child(0)->isLeafNode();
    if (a_leaf && a_leaf->isIntegerConstant())
        return makeBooleanConstant(a_leaf->bits().isEqualToZero(), inode->comment(), inode->flags());

    return Ptr();
}

Ptr
SdivSimplifier::rewrite(Interior *inode, const SmtSolverPtr &solver) const {
    // Constant folding
    LeafPtr a_leaf = inode->child(0)->isLeafNode();
    LeafPtr b_leaf = inode->child(1)->isLeafNode();
    if (a_leaf && b_leaf && a_leaf->isIntegerConstant() && b_leaf->isIntegerConstant() && !b_leaf->bits().isEqualToZero()) {
        if (a_leaf->nBits() <= 64 && b_leaf->nBits() <= 64) {
            int64_t a = a_leaf->toSigned().get();
            int64_t b = b_leaf->toSigned().get();
            return makeIntegerConstant(a_leaf->nBits(), a/b, inode->comment(), inode->flags());
        } else {
            // FIXME[Robb P. Matzke 2014-05-05]: not folding constants larger than 64 bits because Sawyer::Container::BitVector
            // does not currently define division.
        }
    }
    return Ptr();
}

Ptr
SmodSimplifier::rewrite(Interior *inode, const SmtSolverPtr &solver) const {
    // Constant folding
    LeafPtr a_leaf = inode->child(0)->isLeafNode();
    LeafPtr b_leaf = inode->child(1)->isLeafNode();
    if (a_leaf && b_leaf && a_leaf->isIntegerConstant() && b_leaf->isIntegerConstant() && !b_leaf->bits().isEqualToZero()) {
        if (a_leaf->nBits() <= 64 && b_leaf->nBits() <= 64) {
            int64_t a = a_leaf->toSigned().get();
            int64_t b = b_leaf->toSigned().get();
            return makeIntegerConstant(b_leaf->nBits(), a%b, inode->comment(), inode->flags());
        } else {
            // FIXME[Robb P. Matzke 2014-05-05]: not folding constants larger than 64 bits because Sawyer::Container::BitVector
            // does not currently define division.
        }
    }

    return Ptr();
}

Ptr
UdivSimplifier::rewrite(Interior *inode, const SmtSolverPtr &solver) const {
    // Constant folding
    LeafPtr a_leaf = inode->child(0)->isLeafNode();
    LeafPtr b_leaf = inode->child(1)->isLeafNode();
    if (a_leaf && b_leaf && a_leaf->isIntegerConstant() && b_leaf->isIntegerConstant() && !b_leaf->bits().isEqualToZero()) {
        if (a_leaf->nBits() <= 64 && b_leaf->nBits() <= 64) {
            uint64_t a = a_leaf->toUnsigned().get();
            uint64_t b = b_leaf->toUnsigned().get();
            return makeIntegerConstant(a_leaf->nBits(), a/b, inode->comment(), inode->flags());
        } else {
            // FIXME[Robb P. Matzke 2014-05-05]: not folding constants larger than 64 bits because Sawyer::Container::BitVector
            // does not currently define division.
        }
    }

    return Ptr();
}

Ptr
UmodSimplifier::rewrite(Interior *inode, const SmtSolverPtr &solver) const {
    // Constant folding
    LeafPtr a_leaf = inode->child(0)->isLeafNode();
    LeafPtr b_leaf = inode->child(1)->isLeafNode();
    if (a_leaf && b_leaf && a_leaf->isIntegerConstant() && b_leaf->isIntegerConstant() && !b_leaf->bits().isEqualToZero()) {
        if (a_leaf->nBits() <= 64 && b_leaf->nBits() <= 64) {
            uint64_t a = a_leaf->toUnsigned().get();
            uint64_t b = b_leaf->toUnsigned().get();
            return makeIntegerConstant(b_leaf->nBits(), a%b, inode->comment(), inode->flags());
        } else {
            // FIXME[Robb P. Matzke 2014-05-05]: not folding constants larger than 64 bits because Sawyer::Container::BitVector
            // does not currently define division.
        }
    }

    return Ptr();
}

Ptr
ShiftSimplifier::combine_strengths(Ptr strength1, Ptr strength2, size_t value_width, const SmtSolverPtr &solver) const {
    if (!strength1 || !strength2)
        return Ptr();

    // Calculate the width for the sum of the strengths.  If the width of the value being shifted isn't a power of two then we
    // need to avoid overflow in the sum, otherwise overflow doesn't matter.  The sum should be wide enough to hold a shift
    // amount that's the same as the width of the value, otherwise we wouldn't be able to distinguish between the case where
    // modulo addition produced a shift amount that's large enough to decimate the value, as opposed to a shift count of zero
    // which is a no-op.
    size_t sum_width = std::max(strength1->nBits(), strength2->nBits());
    if (IntegerOps::isPowerOfTwo(value_width)) {
        sum_width = std::max(sum_width, IntegerOps::log2max(value_width)+1);
    } else {
        sum_width = std::max(sum_width+1, IntegerOps::log2max(value_width)+1);
    }
    if (sum_width > 64)
        return Ptr();

    // Zero-extend the strengths if they're not as wide as the sum.  This is because the ADD operator requires that its
    // operands are the same width, and the result will also be that width.
    if (strength1->nBits() < sum_width)
        strength1 = makeExtend(makeIntegerConstant(32, sum_width), strength1, solver);
    if (strength2->nBits() < sum_width)
        strength2 = makeExtend(makeIntegerConstant(32, sum_width), strength2, solver);

    return makeAdd(strength1, strength2, solver);
}

Ptr
ShlSimplifier::rewrite(Interior *inode, const SmtSolverPtr &solver) const {
    LeafPtr sa_leaf = inode->child(0)->isLeafNode();
    LeafPtr val_leaf = inode->child(1)->isLeafNode();

    // Effective shift amount
    Sawyer::Optional<uint64_t> sa;
    if (sa_leaf && sa_leaf->isIntegerConstant()) {
        if (sa_leaf->nBits() <= 64) {
            sa = sa_leaf->toUnsigned();
        } else {
            Sawyer::Optional<size_t> msbSetIdx = sa_leaf->bits().mostSignificantSetBit();
            if (!msbSetIdx) {
                sa = 0;
            } else if (*msbSetIdx < 64) {
                sa = sa_leaf->bits().toInteger();       // truncate to 64 bits
            } else {
                sa = inode->nBits();
            }
        }
    }
    if (sa)
        sa = std::min((uint64_t)inode->nBits(), *sa);

    // Constant folding
    if (val_leaf && val_leaf->isIntegerConstant() && sa) {
        Sawyer::Container::BitVector result = val_leaf->bits();
        result.shiftLeft(*sa, newbits);
        return makeIntegerConstant(result, inode->comment(), inode->flags());
    }

    // If the shifted operand is itself a shift of the same kind, then simplify by combining the strengths:
    // (shl AMT1 (shl AMT2 X)) ==> (shl (add AMT1 AMT2) X)
    InteriorPtr val_inode = inode->child(1)->isInteriorNode();
    if (val_inode && val_inode->getOperator()==inode->getOperator()) {
        if (Ptr strength = combine_strengths(inode->child(0), val_inode->child(0), inode->child(1)->nBits(), solver)) {
            return Interior::instance(inode->getOperator(), strength, val_inode->child(1), solver);
        }
    }

    // If the shift amount is known to be at least as large as the value, then replace the value with a constant.
    if (sa && *sa >= inode->nBits()) {
        Sawyer::Container::BitVector result(inode->nBits(), newbits);
        return makeIntegerConstant(result, inode->comment());
    }

    // If the shift amount is zero then this is a no-op
    if (sa && 0==*sa)
        return inode->child(1);

    // If the shift amount is a constant, then:
    // (shl0[N] AMT X) ==> (concat (extract 0 N-AMT X)<hiBits> 0[AMT]<loBits>)
    // (shl1[N] AMT X) ==> (concat (extract 0 N-AMT X)<hiBits> -1[AMT]<loBits>)
    if (sa) {
        ASSERT_require(*sa > 0 && *sa < inode->nBits());// handled above
        size_t nHiBits = inode->nBits() - *sa;
        Ptr hiBits = makeExtract(makeIntegerConstant(32, 0), makeIntegerConstant(32, nHiBits), inode->child(1), solver);
        Ptr loBits = makeIntegerConstant(*sa, newbits?uint64_t(-1):uint64_t(0));
        return makeConcat(hiBits, loBits, solver);
    }

    return Ptr();
}

Ptr
ShrSimplifier::rewrite(Interior *inode, const SmtSolverPtr &solver) const {
    LeafPtr sa_leaf = inode->child(0)->isLeafNode();
    LeafPtr val_leaf = inode->child(1)->isLeafNode();

    // Effective shift amount
    Sawyer::Optional<uint64_t> sa;
    if (sa_leaf && sa_leaf->isIntegerConstant()) {
        if (sa_leaf->nBits() <= 64) {
            sa = sa_leaf->toUnsigned();
        } else {
            Sawyer::Optional<size_t> msbSetIdx = sa_leaf->bits().mostSignificantSetBit();
            if (!msbSetIdx) {
                sa = 0;
            } else if (*msbSetIdx < 64) {
                sa = sa_leaf->bits().toInteger();       // truncate to 64 bits
            } else {
                sa = inode->nBits();
            }
        }
    }
    if (sa)
        sa = std::min((uint64_t)inode->nBits(), *sa);

    // Constant folding
    if (val_leaf && val_leaf->isIntegerConstant() && sa) {
        Sawyer::Container::BitVector result = val_leaf->bits();
        result.shiftRight(*sa, newbits);
        return makeIntegerConstant(result, inode->comment(), inode->flags());
    }

    // If the shifted operand is itself a shift of the same kind, then simplify by combining the strengths:
    //   (shr0 AMT1 (shr0 AMT2 X)) ==> (shr0 (add AMT1 AMT2) X)
    InteriorPtr val_inode = inode->child(1)->isInteriorNode();
    if (val_inode && val_inode->getOperator()==inode->getOperator()) {
        if (Ptr strength = combine_strengths(inode->child(0), val_inode->child(0), inode->child(1)->nBits(), solver)) {
            return Interior::instance(inode->getOperator(), strength, val_inode->child(1), solver);
        }
    }

    // If the shift amount is known to be at least as large as the value, then replace the value with a constant.
    if (sa && *sa >= inode->nBits()) {
        Sawyer::Container::BitVector result(inode->nBits(), newbits);
        return makeIntegerConstant(result, inode->comment(), inode->flags());
    }

    // If the shift amount is zero then this is a no-op
    if (sa && 0 == *sa)
        return inode->child(1);

    // If the shift amount is a constant, then:
    // (shr0[N] AMT X) ==> (concat 0[AMT]  (extract AMT N X))
    // (shr1[N] AMT X) ==> (concat -1[AMT] (extract AMT N X))
    if (sa) {
        ASSERT_require(*sa > 0 && *sa < inode->nBits());// handled above
        Ptr loBits = makeExtract(makeIntegerConstant(32, *sa), makeIntegerConstant(32, inode->nBits()), inode->child(1), solver);
        Ptr hiBits = makeIntegerConstant(*sa, newbits?uint64_t(-1):uint64_t(0));
        return makeConcat(hiBits, loBits, solver);
    }

    return Ptr();
}

Ptr
LssbSimplifier::rewrite(Interior *inode, const SmtSolverPtr &solver) const {
    // Constant folding
    LeafPtr a_leaf = inode->child(0)->isLeafNode();
    if (a_leaf && a_leaf->isIntegerConstant()) {
        if (Sawyer::Optional<size_t> idx = a_leaf->bits().leastSignificantSetBit())
            return makeIntegerConstant(inode->nBits(), *idx, inode->comment());
        return makeIntegerConstant(inode->nBits(), 0, inode->comment(), inode->flags());
    }

    return Ptr();
}

Ptr
MssbSimplifier::rewrite(Interior *inode, const SmtSolverPtr &solver) const {
    // Constant folding
    LeafPtr a_leaf = inode->child(0)->isLeafNode();
    if (a_leaf && a_leaf->isIntegerConstant()) {
        if (Sawyer::Optional<size_t> idx = a_leaf->bits().mostSignificantSetBit())
            return makeIntegerConstant(inode->nBits(), *idx, inode->comment());
        return makeIntegerConstant(inode->nBits(), 0, inode->comment(), inode->flags());
    }

    return Ptr();
}

Ptr
SetSimplifier::rewrite(Interior *inode, const SmtSolverPtr &solver) const {
    // (set x) => x
    if (1 == inode->nChildren())
        return inode->child(0);

    // Remove duplicate arguments
    bool removedDuplicate = false;
    Nodes elements;
    BOOST_FOREACH (const Ptr &elmt1, inode->children()) {
        bool isDuplicate = false;
        BOOST_FOREACH (const Ptr &elmt2, elements) {
            if (elmt1->mustEqual(elmt2, solver)) {
                isDuplicate = true;
                break;
            }
        }
        if (!isDuplicate) {
            elements.push_back(elmt1);
        } else {
            removedDuplicate = true;
        }
    }
    if (!removedDuplicate)
        return Ptr();
    if (1==elements.size())
        return elements[0];
    return Interior::instance(inode->getOperator(), elements, solver, inode->comment());
}

Ptr
Interior::simplifyTop(const SmtSolverPtr &solver) {
    Ptr node = sharedFromThis();
    while (InteriorPtr inode = node->isInteriorNode()) {
        Ptr newnode = node;
        switch (inode->getOperator()) {
            case OP_NONE:
                ASSERT_not_reachable("not possible for an interior node");
            case OP_ADD:
                newnode = inode->rewrite(AddSimplifier(), solver);
                if (newnode==node)
                    newnode = inode->associative()->commutative()->identity(0, solver);
                if (newnode==node)
                    newnode = inode->unaryNoOp();
                if (newnode==node)
                    newnode = inode->foldConstants(AddSimplifier());
                break;
            case OP_AND:
                newnode = inode->associative()->commutative()->idempotent(solver)->identity((uint64_t)-1, solver);
                if (newnode==node)
                    newnode = inode->foldConstants(AndSimplifier());
                if (newnode==node)
                    newnode = inode->rewrite(AndSimplifier(), solver);
                break;
            case OP_ASR:
                newnode = inode->additiveNesting(solver);
                if (newnode==node)
                    newnode = inode->rewrite(AsrSimplifier(), solver);
                break;
            case OP_XOR:
                newnode = inode->associative()->commutative()->identity(0, solver);
                if (newnode==node)
                    newnode = inode->foldConstants(XorSimplifier());
                if (newnode==node)
                    newnode = inode->rewrite(XorSimplifier(), solver);
                break;
            case OP_CONCAT:
                newnode = inode->associative()->foldConstants(ConcatSimplifier());
                if (newnode==node)
                    newnode = inode->rewrite(ConcatSimplifier(), solver);
                break;
            case OP_EQ:
                newnode = inode->commutative();
                if (newnode==node)
                    newnode = inode->rewrite(EqSimplifier(), solver);
                break;
            case OP_EXTRACT:
                newnode = inode->rewrite(ExtractSimplifier(), solver);
                break;
            case OP_INVERT:
                newnode = inode->involutary();
                if (newnode==node)
                    newnode = inode->rewrite(InvertSimplifier(), solver);
                break;
            case OP_ITE:
                newnode = inode->rewrite(IteSimplifier(), solver);
                break;
            case OP_LET:
                // No simplifications
                break;
            case OP_LSSB:
                newnode = inode->rewrite(LssbSimplifier(), solver);
                break;
            case OP_MSSB:
                newnode = inode->rewrite(MssbSimplifier(), solver);
                break;
            case OP_NE:
                newnode = inode->commutative();
                break;
            case OP_NEGATE:
                newnode = inode->involutary();
                if (newnode==node)
                    newnode = inode->rewrite(NegateSimplifier(), solver);
                break;
            case OP_NOOP:
                newnode = inode->rewrite(NoopSimplifier(), solver);
                break;
            case OP_OR:
                newnode = inode->associative()->commutative()->idempotent(solver)->identity(0, solver);
                if (newnode==node)
                    newnode = inode->foldConstants(OrSimplifier());
                if (newnode==node)
                    newnode = inode->rewrite(OrSimplifier(), solver);
                break;
            case OP_READ:
                // no simplifications
                break;
            case OP_ROL:
                newnode = inode->rewrite(RolSimplifier(), solver);
                break;
            case OP_ROR:
                newnode = inode->rewrite(RorSimplifier(), solver);
                break;
            case OP_SDIV:
                newnode = inode->rewrite(SdivSimplifier(), solver);
                break;
            case OP_SET:
                newnode = inode->associative()->commutative();
                if (newnode==node)
                    newnode = inode->rewrite(SetSimplifier(), solver);
                break;
            case OP_SEXTEND:
                newnode = inode->rewrite(SextendSimplifier(), solver);
                break;
            case OP_SGE:
                newnode = inode->rewrite(SgeSimplifier(), solver);
                break;
            case OP_SGT:
                newnode = inode->rewrite(SgtSimplifier(), solver);
                break;
            case OP_SHL0:
                newnode = inode->additiveNesting(solver);
                if (newnode==node)
                    newnode = inode->rewrite(ShlSimplifier(false), solver);
                break;
            case OP_SHL1:
                newnode = inode->additiveNesting(solver);
                if (newnode==node)
                    newnode = inode->rewrite(ShlSimplifier(true), solver);
                break;
            case OP_SHR0:
                newnode = inode->additiveNesting(solver);
                if (newnode==node)
                    newnode = inode->rewrite(ShrSimplifier(false), solver);
                break;
            case OP_SHR1:
                newnode = inode->additiveNesting(solver);
                if (newnode==node)
                    newnode = inode->rewrite(ShrSimplifier(true), solver);
                break;
            case OP_SLE:
                newnode = inode->rewrite(SleSimplifier(), solver);
                break;
            case OP_SLT:
                newnode = inode->rewrite(SltSimplifier(), solver);
                break;
            case OP_SMOD:
                newnode = inode->rewrite(SmodSimplifier(), solver);
                break;
            case OP_SMUL:
                newnode = inode->associative()->commutative()->foldConstants(SmulSimplifier());
                break;
            case OP_UDIV:
                newnode = inode->rewrite(UdivSimplifier(), solver);
                break;
            case OP_UEXTEND:
                newnode = inode->rewrite(UextendSimplifier(), solver);
                break;
            case OP_UGE:
                newnode = inode->rewrite(UgeSimplifier(), solver);
                break;
            case OP_UGT:
                newnode = inode->rewrite(UgtSimplifier(), solver);
                break;
            case OP_ULE:
                newnode = inode->rewrite(UleSimplifier(), solver);
                break;
            case OP_ULT:
                newnode = inode->rewrite(UltSimplifier(), solver);
                break;
            case OP_UMOD:
                newnode = inode->rewrite(UmodSimplifier(), solver);
                break;
            case OP_UMUL:
                newnode = inode->associative()->commutative()->identity(1, solver);
                if (newnode==node)
                    newnode = inode->foldConstants(UmulSimplifier());
                break;
            case OP_WRITE:
                // no simplifications
                break;
            case OP_ZEROP:
                newnode = inode->rewrite(ZeropSimplifier(), solver);
                break;
            case OP_FP_ABS:
            case OP_FP_NEGATE:
            case OP_FP_ADD:
            case OP_FP_MUL:
            case OP_FP_DIV:
            case OP_FP_MULADD:
            case OP_FP_SQRT:
            case OP_FP_MOD:
            case OP_FP_ROUND:
            case OP_FP_MIN:
            case OP_FP_MAX:
                newnode = inode->poisonNan(solver);
                break;
            case OP_FP_LE:
            case OP_FP_LT:
            case OP_FP_GE:
            case OP_FP_GT:
            case OP_FP_EQ:
            case OP_FP_ISNORM:
            case OP_FP_ISSUBNORM:
            case OP_FP_ISZERO:
            case OP_FP_ISINFINITE:
            case OP_FP_ISNAN:
            case OP_FP_ISNEG:
            case OP_FP_ISPOS:
            case OP_CONVERT:
            case OP_REINTERPRET:
                // no simplification
                break;
        }
        if (newnode==node)
            break;
        node = newnode;
    }
    return node;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Leaf nodes
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// class method
LeafPtr
Leaf::createVariable(const Type &type, const std::string &comment, unsigned flags) {
    return createVariable(type, nextNameCounter(), comment, flags);
}

// class method
LeafPtr
Leaf::createVariable(const Type &type, const uint64_t id, const std::string &comment, unsigned flags) {
    if (type.nBits() == 0)
        throw Exception("variables must have positive width");
    Leaf *node = new Leaf(comment, flags);
    node->type_ = type;
    node->name_ = id;
    return LeafPtr(node);
}

// class method
LeafPtr
Leaf::createConstant(const Type &type, const Sawyer::Container::BitVector &bits, const std::string &comment, unsigned flags) {
    if (type.nBits() != bits.size())
        throw Exception("bit vector is wrong size for type");
    if (bits.size() == 0)
        throw Exception("zero-sized constants are not allowed"); // bits.isEmpty implies leaf is a variable
    Leaf *node = new Leaf(comment, flags);
    node->type_ = type;
    node->bits_ = bits;
    return LeafPtr(node);
}

// deprecated [Robb Matzke 2019-09-30]
LeafPtr
Leaf::createInteger(size_t nBits, uint64_t value, const std::string &comment, unsigned flags) {
    return makeIntegerConstant(nBits, value, comment, flags)->isLeafNode();
}

// deprecated [Robb Matzke 2019-09-30]
LeafPtr
Leaf::createBoolean(bool b, const std::string &comment, unsigned flags) {
    return makeBooleanConstant(b, comment, flags);
}

// class method
uint64_t
Leaf::nextNameCounter(uint64_t useThis) {
    static boost::mutex mutex;
    static uint64_t counter = 0;
    boost::lock_guard<boost::mutex> lock(mutex);
    if (useThis == (uint64_t)(-1))
        return ++counter;
    counter = std::max(counter, useThis);
    return useThis;
}

Sawyer::Optional<uint64_t>
Leaf::toUnsigned() const {
    if (!isIntegerConstant())
        return Sawyer::Nothing();
    if (nBits() <= 64)
        return bits_.toInteger();
    size_t mssb = bits_.mostSignificantSetBit().orElse(0);
    if (mssb < 64)
        return bits_.toInteger(mssb+1);
    return Sawyer::Nothing();
}

Sawyer::Optional<int64_t>
Leaf::toSigned() const {
    using namespace Sawyer::Container;
    if (!isIntegerConstant())
        return Sawyer::Nothing();
    if (nBits() <= 64)
        return bits_.toSignedInteger();
    bool isNegative = bits_.get(bits_.size()-1);
    if (isNegative) {
        size_t mscb = bits_.mostSignificantClearBit().orElse(0);
        if (mscb >= 63)
            return Sawyer::Nothing();
    } else {
        size_t mssb = bits_.mostSignificantSetBit().orElse(0);
        if (mssb >= 63)
            return Sawyer::Nothing();
    }
    return bits_.toSignedInteger();
}

const Sawyer::Container::BitVector&
Leaf::bits() const {
    ASSERT_require(isConstant());
    return bits_;
}

uint64_t
Leaf::nameId() const {
    ASSERT_require(isVariable2());
    return name_;
}

std::string
Leaf::toString() const {
    if (isConstant()) {                                 // integers or floating-point
        return "0x" + bits().toHex();
    } else if (isMemoryVariable()) {
        return "m" + StringUtility::numberToString(nameId());
    } else if (isVariable2()) {
        return "v" + StringUtility::numberToString(nameId());
    }
    ASSERT_not_reachable("invalid leaf type");
    return "";
}

void
Leaf::print(std::ostream &o, Formatter &formatter) const {
    printAsSigned(o, formatter);
}

void
Leaf::printAsSigned(std::ostream &o, Formatter &formatter, bool as_signed) const {
    bool showed_comment = false;
    if (isIntegerConstant()) {
        if (bits_.size() == 1) {
            // Boolean values
            if (bits_.toInteger()) {
                o <<"true";
            } else {
                o <<"false";
            }
        } else if (bits_.size() <= 64) {
            // Integer values that are small enough to use the machine's native type.
            uint64_t ival = bits_.toInteger();
            if ((32==nBits() || 64==nBits()) && 0!=(ival & 0xffff0000) && 0xffff0000!=(ival & 0xffff0000)) {
                // The value is probably an address, so print it like one.
                if (formatter.use_hexadecimal) {
                    o <<StringUtility::unsignedToHex2(ival, nBits());
                } else {
                    // The old behavior (which is enabled when formatter.use_hexadecimal is false) was to print only the
                    // hexadecimal format and not the decimal format, so we'll emulate that. [Robb P. Matzke 2013-12-26]
                    o <<StringUtility::addrToString(ival, nBits());
                }
            } else if (as_signed) {
                if (formatter.use_hexadecimal) {
                    o <<StringUtility::toHex2(ival, nBits()); // show as signed and unsigned
                } else if (IntegerOps::signBit2(ival, nBits())) {
                    o <<(int64_t)IntegerOps::signExtend2(ival, nBits(), 64);
                } else {
                    o <<ival;
                }
            } else {
                if (formatter.use_hexadecimal) {
                    o <<StringUtility::unsignedToHex2(ival, nBits()); // show only as unsigned
                } else {
                    o <<ival;
                }
            }
        } else {
            // Integers that are too wide -- use bit vector support instead.
            // FIXME[Robb P. Matzke 2014-05-05]: we should change StringUtility functions to handle BitVector arguments also.
            o <<"0x" <<bits_.toHex();
        }
    } else if (isFloatingPointConstant()) {
        o <<"0x" <<bits_.toHex();
        
    } else if (formatter.show_comments == Formatter::CMT_INSTEAD && !comment_.empty()) {
        // Use the comment as the variable name.
        ASSERT_require(isVariable2());
        o <<nameEscape(comment_);
        showed_comment = true;

    } else {
        // Show the variable name.
        ASSERT_require(isVariable2());
        uint64_t renamed = name_;
        if (formatter.do_rename) {
            RenameMap::iterator found = formatter.renames.find(name_);
            if (found==formatter.renames.end() && formatter.add_renames) {
                renamed = formatter.renames.size();
                formatter.renames.insert(std::make_pair(name_, renamed));
            } else {
                renamed = found->second;
            }
        }
        if (isMemoryVariable()) {
            o <<"m" <<renamed;
        } else {
            o <<"v" <<renamed;
        }
    }

    // Bit width of variable.  All variables have this otherwise there's no way for the parser to tell how wide a variable is
    // when reading it back in.
    if (formatter.show_type) {
        o <<'[' <<type().toString(TypeStyle::ABBREVIATED) <<']';
    }

    // Comment stuff
    char bracket='<';
    if (formatter.show_flags)
        printFlags(o, flags(), bracket /*in,out*/);
    if (!showed_comment && formatter.show_comments!=Formatter::CMT_SILENT && !comment_.empty()) {
        o <<bracket <<commentEscape(comment_);
        bracket = ',';
    }
    if (bracket != '<')
        o <<">";
}

bool
Leaf::mustEqual(const Ptr &other_, const SmtSolverPtr &solver) {
    bool retval = false;
    LeafPtr other = other_->isLeafNode();
    if (this == getRawPointer(other)) {
        retval = true;
    } else if (flags() != other_->flags()) {
        // Expressions with different flags are not considered mustEqual
        retval = false;
    } else if (other == NULL) {
        // We need an SMT solver to figure this out.  This handles things like "x mustEqual (not (not x))" which is true.
        if (solver) {
            SmtSolver::Transaction transaction(solver);
            Ptr assertion = makeNe(sharedFromThis(), other_, solver);
            solver->insert(assertion);
            retval = SmtSolver::SAT_NO == solver->check(); // must equal if there is no soln for inequality
        }
    } else if (isConstant() && other->isConstant()) {
        if (isIntegerConstant() && other->isIntegerConstant()) {
            // Both are integers
            retval = 0 == bits_.compare(other->bits());
        } else {
            // At least one is something other than an integer constant
            if (solver) {
                SmtSolver::Transaction transaction(solver);
                Ptr assertion = makeNe(sharedFromThis(), other_, solver);
                solver->insert(assertion);
                retval = SmtSolver::SAT_NO == solver->check(); // must equal if there is no soln for inequality
            }
        }

    } else if (solver && solver->nAssertions() > 0) {
        // At least one is a variable and the solver has some context that might affect the comparison
        SmtSolver::Transaction transaction(solver);
        Ptr assertion = makeNe(sharedFromThis(), other_, solver);
        solver->insert(assertion);
        retval = SmtSolver::SAT_NO == solver->check(); // must equal if there is no soln for inequality

    } else if (isVariable2() && other->isVariable2()) {
        // Both are variables, and no solver context. Variables must be equal if they have the same name.
        ASSERT_require2(name_ != other->name_ || type_ == other->type_, "vars with same name must have same type");
        retval = name_ == other->name_;
    }
    return retval;
}

bool
Leaf::mayEqual(const Ptr &other, const SmtSolverPtr &solver) {
    LeafPtr otherLeaf = other->isLeafNode();

    // Fast comparison of literally the same expression pointer
    if (this == getRawPointer(other))
        return true;

    // The may-equal operator is symmetric, therefore if other is an interior node use that method instead (which is where the
    // more complicated logic lives).
    if (other->isInteriorNode())
        return other->mayEqual(sharedFromThis(), solver);

    // Give the user a chance to decide.
    if (mayEqualCallback) {
        boost::logic::tribool result = (mayEqualCallback)(sharedFromThis(), other, solver);
        if (true == result || false == result)
            return result ? true : false;
    }

    // If both expressions are variables of the same type then they might be equal.
    if (isVariable2() && other->isVariable2() && type() == other->type())
        return true;

    // If both expressions are integer constants, they're equal if the integer interpretation of their bits is equal. The
    // integer constants don't have to be the same width in order to be equal (e.g., 42[8] == 42[16]).
    if (isIntegerConstant() && other->isIntegerConstant())
        return bits().compare(otherLeaf->bits()) == 0;

    // If both expressions are floating point constants, we can reach a conclusion without a solver. Two floating point
    // constants are equal if they have the same bit pattern and neither one is a NaN bit pattern.
    if (isFloatingPointConstant() && other->isFloatingPointConstant())
        return !isFloatingPointNan() && !otherLeaf->isFloatingPointNan() && bits().equalTo(otherLeaf->bits());

    // When comparing V with V+C where V is an integer variable and C is an integer constant, then V may-equal V+C is true iff
    // C is zero.
    LeafPtr variableB, constantB;
    if (isIntegerVariable() && matchAddVariableConstant(variableB /*out*/, constantB /*out*/)) {
        ASSERT_require(nBits() == variableB->nBits());
        ASSERT_require(variableB->nBits() == constantB->nBits());
        ASSERT_forbid2(constantB->bits().isEqualToZero(), "additive identity should have been simplified");
        return false;
    }

    // Use an SMT solver (if there is one) for all remaining cases
    if (solver) {
        ASSERT_require(isFloatingPointConstant() != other->isFloatingPointConstant());
        ASSERT_require(isIntegerConstant() != other->isIntegerConstant());
        SmtSolver::Transaction transaction(solver);
        Ptr assertion = makeEq(sharedFromThis(), other, solver);
        solver->insert(assertion);
        return SmtSolver::SAT_NO == solver->check();
    }

    // If all else fails, assume that two expressions might be equal.
    return true;
}

int
Leaf::compareStructure(const Ptr &other_) {
    LeafPtr other = other_->isLeafNode();
    if (this==getRawPointer(other)) {
        return 0;
    } else if (other==NULL) {
        return -1;                                      // leaf nodes < internal nodes
    } else if (nBits() != other->nBits()) {
        return nBits() < other->nBits() ? -1 : 1;
    } else if (flags() != other->flags()) {
        return flags() < other->flags() ? -1 : 1;
    } else if (isIntegerConstant() != other->isIntegerConstant()) {
        return isIntegerConstant() ? -1 : 1;            // integer constants less than non-integer constants
    } else if (isFloatingPointConstant() != other->isFloatingPointConstant()) {
        return isFloatingPointConstant() ? -1 : 1;      // FP constants less than remaining types
    } else if (isConstant()) {
        ASSERT_require(other->isConstant());
        ASSERT_require(nBits() == other->nBits());
        // compare constants (even FP) as unsigned integers since all we need is an order for the bits without
        // any particular interpretation.
        return bits().compare(other->bits());
    } else {
        ASSERT_require(isVariable2());
        ASSERT_require(other->isVariable2());
        return name_ < other->name_ ? -1 : 1;
    }
    return 0;
}

bool
Leaf::isEquivalentTo(const Ptr &other_) {
    bool retval = false;
    LeafPtr other = other_->isLeafNode();
    if (this==getRawPointer(other)) {
        retval = true;
    } else if (other && nBits() == other->nBits() && flags() == other->flags()) {
        if (isConstant()) {
            retval = other->isConstant() && bits().equalTo(other->bits());
        } else {
            retval = !other->isConstant() && name_ == other->name_;
        }
    }
    return retval;
}

Ptr
Leaf::substitute(const Ptr &from, const Ptr &to, const SmtSolverPtr &solver) {
    ASSERT_require(from!=NULL && to!=NULL && from->nBits()==to->nBits());
    if (isEquivalentTo(from))
        return to;
    return sharedFromThis();
}

VisitAction
Leaf::depthFirstTraversal(Visitor &v) const {
    LeafPtr self(const_cast<Leaf*>(this));
    VisitAction retval = v.preVisit(self);
    if (TERMINATE!=retval)
        retval = v.postVisit(self);
    return retval;
}

const Nodes&
Leaf::children() const {
    static const Nodes empty;
    return empty;
}

bool
Leaf::isFloatingPointNan() const {
    using namespace Sawyer::Container;
    if (!isFloatingPointExpr() || !isConstant())
        return false;
    BitVector::BitRange significandRange = BitVector::BitRange::baseSize(0, type().significandWidth() - 1);
    BitVector::BitRange exponentRange = BitVector::BitRange::baseSize(type().significandWidth() - 1, type().exponentWidth());
    return bits_.isAllSet(exponentRange) && !bits_.isAllClear(significandRange);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      ExprExprHashMap
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ExprExprHashMap
ExprExprHashMap::invert() const {
    ExprExprHashMap retval;
    BOOST_FOREACH (const ExprExprHashMap::value_type &node, *this)
        retval[node.second] = node.first;
    return retval;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Free functions of the API
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::ostream&
operator<<(std::ostream &o, Node &node) {
    Formatter fmt;
    node.print(o, fmt);
    return o;
}

std::ostream&
operator<<(std::ostream &o, const Node::WithFormatter &w)
{
    w.print(o);
    return o;
}

std::vector<Ptr>
findCommonSubexpressions(const std::vector<Ptr> &exprs) {
    return findCommonSubexpressions(exprs.begin(), exprs.end());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Factory functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LeafPtr
makeVariable(const Type &type, const std::string &comment, unsigned flags) {
    return Leaf::createVariable(type, comment, flags);
}

LeafPtr
makeVariable(const Type &type, uint64_t id, const std::string &comment, unsigned flags) {
    return Leaf::createVariable(type, id, comment, flags);
}

LeafPtr
makeConstant(const Type &type, const Sawyer::Container::BitVector &bits, const std::string &comment, unsigned flags) {
    return Leaf::createConstant(type, bits, comment, flags);
}

LeafPtr
makeIntegerVariable(size_t nBits, const std::string &comment, unsigned flags) {
    return Leaf::createVariable(Type::integer(nBits), comment, flags);
}

LeafPtr
makeIntegerVariable(size_t nBits, uint64_t id, const std::string &comment, unsigned flags) {
    return Leaf::createVariable(Type::integer(nBits), id, comment, flags);
}

LeafPtr
makeIntegerConstant(size_t nBits, uint64_t value, const std::string &comment, unsigned flags) {
    Sawyer::Container::BitVector bits(nBits);
    bits.fromInteger(value);
    return Leaf::createConstant(Type::integer(nBits), bits, comment, flags);
}

LeafPtr
makeIntegerConstant(const Sawyer::Container::BitVector &bits, const std::string &comment, unsigned flags) {
    return Leaf::createConstant(Type::integer(bits.size()), bits, comment, flags);
}

LeafPtr
makeBooleanConstant(bool b, const std::string &comment, unsigned flags) {
    Sawyer::Container::BitVector bits(1);
    if (b)
        bits.set(0);
    return Leaf::createConstant(Type::integer(1), bits, comment, flags);
}

LeafPtr
makeMemoryVariable(size_t addressWidth, size_t valueWidth, const std::string &comment, unsigned flags) {
    return Leaf::createVariable(Type::memory(addressWidth, valueWidth), comment, flags);
}

LeafPtr
makeMemoryVariable(size_t addressWidth, size_t valueWidth, uint64_t id, const std::string &comment, unsigned flags) {
    return Leaf::createVariable(Type::memory(addressWidth, valueWidth), id, comment, flags);
}

LeafPtr
makeFloatingPointVariable(size_t eb, size_t sb, const std::string &comment, unsigned flags) {
    return Leaf::createVariable(Type::floatingPoint(eb, sb), comment, flags);
}

LeafPtr
makeFloatingPointVariable(size_t eb, size_t sb, uint64_t id, const std::string &comment, unsigned flags) {
    return Leaf::createVariable(Type::floatingPoint(eb, sb), id, comment, flags);
}

LeafPtr
makeFloatingPointConstant(float f, const std::string &comment, unsigned flags) {
    using namespace Sawyer::Container;
    ASSERT_require(4 == sizeof f);
    union {
        float f;
        uint8_t b[4];
    } u;
    u.f = f;
    BitVector bv(4);
    switch (ByteOrder::host_order()) {
        case ByteOrder::ORDER_LSB:
            bv.fromInteger(BitVector::BitRange::baseSize(0, 8),  u.b[0]);
            bv.fromInteger(BitVector::BitRange::baseSize(8, 8),  u.b[1]);
            bv.fromInteger(BitVector::BitRange::baseSize(16, 8), u.b[2]);
            bv.fromInteger(BitVector::BitRange::baseSize(24, 8), u.b[3]);
            break;
        case ByteOrder::ORDER_MSB:
            bv.fromInteger(BitVector::BitRange::baseSize(0, 8),  u.b[3]);
            bv.fromInteger(BitVector::BitRange::baseSize(8, 8),  u.b[2]);
            bv.fromInteger(BitVector::BitRange::baseSize(16, 8), u.b[1]);
            bv.fromInteger(BitVector::BitRange::baseSize(24, 8), u.b[0]);
            break;
        default:
            ASSERT_not_reachable("invalid host byte order");
    }
    return Leaf::createConstant(Type::floatingPoint(8, 24), bv, comment, flags);
}

LeafPtr
makeFloatingPointConstant(double d, const std::string &comment, unsigned flags) {
    using namespace Sawyer::Container;
    ASSERT_require(8 == sizeof d);
    union {
        double d;
        uint8_t b[8];
    } u;
    u.d = d;
    BitVector bv(8);
    switch (ByteOrder::host_order()) {
        case ByteOrder::ORDER_LSB:
            bv.fromInteger(BitVector::BitRange::baseSize(0, 8),  u.b[0]);
            bv.fromInteger(BitVector::BitRange::baseSize(8, 8),  u.b[1]);
            bv.fromInteger(BitVector::BitRange::baseSize(16, 8), u.b[2]);
            bv.fromInteger(BitVector::BitRange::baseSize(24, 8), u.b[3]);
            bv.fromInteger(BitVector::BitRange::baseSize(32, 8), u.b[4]);
            bv.fromInteger(BitVector::BitRange::baseSize(40, 8), u.b[5]);
            bv.fromInteger(BitVector::BitRange::baseSize(48, 8), u.b[6]);
            bv.fromInteger(BitVector::BitRange::baseSize(56, 8), u.b[7]);
            break;
        case ByteOrder::ORDER_MSB:
            bv.fromInteger(BitVector::BitRange::baseSize(0, 8),  u.b[7]);
            bv.fromInteger(BitVector::BitRange::baseSize(8, 8),  u.b[6]);
            bv.fromInteger(BitVector::BitRange::baseSize(16, 8), u.b[5]);
            bv.fromInteger(BitVector::BitRange::baseSize(24, 8), u.b[4]);
            bv.fromInteger(BitVector::BitRange::baseSize(32, 8), u.b[3]);
            bv.fromInteger(BitVector::BitRange::baseSize(40, 8), u.b[2]);
            bv.fromInteger(BitVector::BitRange::baseSize(48, 8), u.b[1]);
            bv.fromInteger(BitVector::BitRange::baseSize(56, 8), u.b[0]);
            break;
        default:
            ASSERT_not_reachable("invalid host byte order");
    }
    return Leaf::createConstant(Type::floatingPoint(11, 53), bv, comment, flags);
}

LeafPtr
makeFloatingPointNan(size_t eb, size_t sb, const std::string &comment, unsigned flags) {
    using namespace Sawyer::Container;
    BitVector bits(eb+sb);

    // There are many NaN bit patterns, we use just one.  The sign bit can be set or clear, the exponent bits must all
    // be set, and the significand bits must not be all clear. It's easiest for us to just set all bits, plus it's an
    // easy bit pattern to recognize in the output.
    bits.set(bits.hull());
    return Leaf::createConstant(Type::floatingPoint(eb, sb), bits, comment, flags);
}

// deprecated [Robb Matzke 2019-09-30]
Ptr
makeVariable(size_t nBits, const std::string &comment, unsigned flags) {
    return makeIntegerVariable(nBits, comment, flags);
}

// deprecated [Robb Matzke 2019-09-30]
Ptr
makeExistingVariable(size_t nBits, uint64_t id, const std::string &comment, unsigned flags) {
    return makeIntegerVariable(nBits, id, comment, flags);
}

// deprecated [Robb Matzke 2019-09-30]
Ptr
makeInteger(size_t nBits, uint64_t value, const std::string &comment, unsigned flags) {
    return makeIntegerConstant(nBits, value, comment, flags);
}

// deprecated [Robb Matzke 2019-09-30]
Ptr
makeConstant(const Sawyer::Container::BitVector &bits, const std::string &comment, unsigned flags) {
    return makeIntegerConstant(bits, comment, flags);
}

// deprecated [Robb Matzke 2019-09-30]
Ptr
makeBoolean(bool b, const std::string &comment, unsigned flags) {
    return makeBooleanConstant(b, comment, flags);
}

// deprecated [Robb Matzke 2019-09-30]
Ptr
makeMemory(size_t addressWidth, size_t valueWidth, const std::string &comment, unsigned flags) {
    return makeMemoryVariable(addressWidth, valueWidth, comment, flags);
}

// deprecated [Robb Matzke 2019-09-30]
Ptr
makeExistingMemory(size_t addressWidth, size_t valueWidth, uint64_t id, const std::string &comment, unsigned flags) {
    return makeMemoryVariable(addressWidth, valueWidth, id, comment, flags);
}

Ptr
makeAdd(const Ptr&a, const Ptr &b, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    if (a->isFloatingPointExpr() && b->isFloatingPointExpr()) {
        return Interior::instance(OP_FP_ADD, a, b, solver, comment, flags);
    } else {
        return Interior::instance(OP_ADD, a, b, solver, comment, flags);
    }
}

Ptr
makeBooleanAnd(const Ptr &a, const Ptr &b, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    return Interior::create(0, OP_AND, a, b, solver, comment, flags);
}

Ptr
makeAsr(const Ptr &sa, const Ptr &a, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    return Interior::instance(OP_ASR, sa, a, solver, comment, flags);
}

Ptr
makeAnd(const Ptr &a, const Ptr &b, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    return Interior::instance(OP_AND, a, b, solver, comment, flags);
}

Ptr
makeOr(const Ptr &a, const Ptr &b, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    return Interior::instance(OP_OR, a, b, solver, comment, flags);
}

Ptr
makeXor(const Ptr &a, const Ptr &b, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    return Interior::instance(OP_XOR, a, b, solver, comment, flags);
}

Ptr
makeConcat(const Ptr &hi, const Ptr &lo, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    return Interior::instance(OP_CONCAT, hi, lo, solver, comment, flags);
}

Ptr
makeConvert(const Ptr &a, const Type &dstType, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    return Interior::instance(dstType, OP_CONVERT, a, solver, comment, flags);
}

Ptr
makeEq(const Ptr &a, const Ptr &b, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    if (a->isFloatingPointExpr() && b->isFloatingPointExpr()) {
        return Interior::instance(OP_FP_EQ, a, b, solver, comment, flags);
    } else {
        return Interior::instance(OP_EQ, a, b, solver, comment, flags);
    }
}

Ptr
makeExtract(const Ptr &begin, const Ptr &end, const Ptr &a, const SmtSolverPtr &solver, const std::string &comment,
            unsigned flags) {
    return Interior::instance(OP_EXTRACT, begin, end, a, solver, comment, flags);
}

Ptr
makeInvert(const Ptr &a, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    return Interior::instance(OP_INVERT, a, solver, comment, flags);
}

Ptr
makeIsInfinite(const Ptr &a, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    return Interior::instance(OP_FP_ISINFINITE, a, solver, comment, flags);
}

Ptr
makeIsNan(const Ptr &a, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    return Interior::instance(OP_FP_ISNAN, a, solver, comment, flags);
}

Ptr
makeIsNeg(const Ptr &a, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    if (a->isFloatingPointExpr()) {
        return Interior::instance(OP_FP_ISNEG, a, solver, comment, flags);
    } else {
        return makeSignedLt(a, makeIntegerConstant(a->nBits(), 0), solver, comment, flags);
    }
}

Ptr
makeIsNorm(const Ptr &a, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    return Interior::instance(OP_FP_ISNORM, a, solver, comment, flags);
}

Ptr
makeIsPos(const Ptr &a, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    if (a->isFloatingPointExpr()) {
        return Interior::instance(OP_FP_ISPOS, a, solver, comment, flags);
    } else {
        return makeGt(a, makeIntegerConstant(a->nBits(), 0), solver, comment, flags);
    }
}

Ptr
makeIsSubnorm(const Ptr &a, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    return Interior::instance(OP_FP_ISSUBNORM, a, solver, comment, flags);
}

Ptr
makeIte(const Ptr &cond, const Ptr &a, const Ptr &b, const SmtSolverPtr &solver, const std::string &comment,
        unsigned flags) {
    return Interior::instance(OP_ITE, cond, a, b, solver, comment, flags);
}

Ptr
makeLet(const Ptr &a, const Ptr &b, const Ptr &c, const SmtSolverPtr &solver, const std::string &comment,
        unsigned flags) {
    return Interior::instance(OP_LET, a, b, c, solver, comment, flags);
}

Ptr
makeLssb(const Ptr &a, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    return Interior::instance(OP_LSSB, a, solver, comment, flags);
}

Ptr
makeMax(const Ptr &a, const Ptr &b, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    if (a->isFloatingPointExpr() && b->isFloatingPointExpr()) {
        return Interior::instance(OP_FP_MAX, a, b, solver, comment, flags);
    } else {
        return makeIte(makeGe(a, b, solver, "", 0), a, b, solver, comment, flags);
    }
}

Ptr
makeMin(const Ptr &a, const Ptr &b, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    if (a->isFloatingPointExpr() && b->isFloatingPointExpr()) {
        return Interior::instance(OP_FP_MIN, a, b, solver, comment, flags);
    } else {
        return makeIte(makeLe(a, b, solver, "", 0), a, b, solver, comment, flags);
    }
}

Ptr
makeMssb(const Ptr &a, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    return Interior::instance(OP_MSSB, a, solver, comment, flags);
}

Ptr
makeMultiplyAdd(const Ptr &a, const Ptr &b, const Ptr &c, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    if (a->isFloatingPointExpr() && b->isFloatingPointExpr() && c->isFloatingPointExpr()) {
        return Interior::instance(OP_FP_MULADD, a, b, c, solver, comment, flags);
    } else {
        return makeAdd(makeSignedMul(a, b, solver, "", 0), c, solver, comment, flags);
    }
}

Ptr
makeNe(const Ptr &a, const Ptr &b, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    if (a->isFloatingPointExpr() && b->isFloatingPointExpr()) {
        return makeInvert(Interior::instance(OP_FP_EQ, a, b, solver), solver, comment, flags);
    } else {
        return Interior::instance(OP_NE, a, b, solver, comment, flags);
    }
}

Ptr
makeNegate(const Ptr &a, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    if (a->isFloatingPointExpr()) {
        return Interior::instance(OP_FP_NEGATE, a, solver, comment, flags);
    } else {
        return Interior::instance(OP_NEGATE, a, solver, comment, flags);
    }
}

Ptr
makeBooleanOr(const Ptr &a, const Ptr &b, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    return Interior::instance(OP_OR, a, b, solver, comment, flags);
}

Ptr
makeRead(const Ptr &mem, const Ptr &addr, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    return Interior::instance(OP_READ, mem, addr, solver, comment, flags);
}

Ptr
makeReinterpret(const Ptr &a, const Type &dstType, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    return Interior::instance(dstType, OP_REINTERPRET, a, solver, comment, flags);
}

Ptr
makeRol(const Ptr &sa, const Ptr &a, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    return Interior::instance(OP_ROL, sa, a, solver, comment, flags);
}

Ptr
makeRor(const Ptr &sa, const Ptr &a, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    return Interior::instance(OP_ROR, sa, a, solver, comment, flags);
}

Ptr
makeRound(const Ptr &a, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    return Interior::instance(OP_FP_ROUND, a, solver, comment, flags);
}

Ptr
makeSet(const Ptr &a, const Ptr &b, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    return Interior::instance(OP_SET, a, b, solver, comment, flags);
}

Ptr
makeSet(const Ptr &a, const Ptr &b, const Ptr &c, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    return Interior::instance(OP_SET, a, b, c, solver, comment, flags);
}

Ptr
makeSignedDiv(const Ptr &a, const Ptr &b, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    if (a->isFloatingPointExpr() && b->isFloatingPointExpr()) {
        return Interior::instance(OP_FP_DIV, a, b, solver, comment, flags);
    } else {
        return Interior::instance(OP_SDIV, a, b, solver, comment, flags);
    }
}

Ptr
makeSignExtend(const Ptr &newSize, const Ptr &a, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    return Interior::instance(OP_SEXTEND, newSize, a, solver, comment, flags);
}

Ptr
makeSignedGe(const Ptr &a, const Ptr &b, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    if (a->isFloatingPointExpr() && b->isFloatingPointExpr()) {
        return Interior::instance(OP_FP_GE, a, b, solver, comment, flags);
    } else {
        return Interior::instance(OP_SGE, a, b, solver, comment, flags);
    }
}

Ptr
makeSignedGt(const Ptr &a, const Ptr &b, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    if (a->isFloatingPointExpr() && b->isFloatingPointExpr()) {
        return Interior::instance(OP_FP_GT, a, b, solver, comment, flags);
    } else {
        return Interior::instance(OP_SGT, a, b, solver, comment, flags);
    }
}

Ptr
makeShl0(const Ptr &sa, const Ptr &a, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    return Interior::instance(OP_SHL0, sa, a, solver, comment, flags);
}

Ptr
makeShl1(const Ptr &sa, const Ptr &a, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    return Interior::instance(OP_SHL1, sa, a, solver, comment, flags);
}

Ptr
makeShr0(const Ptr &sa, const Ptr &a, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    return Interior::instance(OP_SHR0, sa, a, solver, comment, flags);
}

Ptr
makeShr1(const Ptr &sa, const Ptr &a, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    return Interior::instance(OP_SHR1, sa, a, solver, comment, flags);
}

Ptr
makeSignedIsPos(const Ptr &a, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    if (a->isFloatingPointExpr()) {
        return Interior::instance(OP_FP_ISPOS, a, solver, comment, flags);
    } else {
        return makeSignedGt(a, makeIntegerConstant(a->nBits(), 0), solver, comment, flags);
    }
}

Ptr
makeSignedLe(const Ptr &a, const Ptr &b, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    if (a->isFloatingPointExpr() && b->isFloatingPointExpr()) {
        return Interior::instance(OP_FP_LE, a, b, solver, comment, flags);
    } else {
        return Interior::instance(OP_SLE, a, b, solver, comment, flags);
    }
}

Ptr
makeSignedLt(const Ptr &a, const Ptr &b, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    if (a->isFloatingPointExpr() && b->isFloatingPointExpr()) {
        return Interior::instance(OP_FP_LT, a, b, solver, comment, flags);
    } else {
        return Interior::instance(OP_SLT, a, b, solver, comment, flags);
    }
}

Ptr
makeSignedMax(const Ptr &a, const Ptr &b, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    if (a->isFloatingPointExpr() && b->isFloatingPointExpr()) {
        return Interior::instance(OP_FP_MAX, a, b, solver, comment, flags);
    } else {
        return makeIte(makeSignedGe(a, b, solver, "", 0), a, b, solver, comment, flags);
    }
}

Ptr
makeSignedMin(const Ptr &a, const Ptr &b, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    if (a->isFloatingPointExpr() && b->isFloatingPointExpr()) {
        return Interior::instance(OP_FP_MIN, a, b, solver, comment, flags);
    } else {
        return makeIte(makeSignedLe(a, b, solver, "", 0), a, b, solver, comment, flags);
    }
}

Ptr
makeSignedMod(const Ptr &a, const Ptr &b, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    if (a->isFloatingPointExpr() && b->isFloatingPointExpr()) {
        return Interior::instance(OP_FP_MOD, a, b, solver, comment, flags);
    } else {
        return Interior::instance(OP_SMOD, a, b, solver, comment, flags);
    }
}

Ptr
makeSignedMul(const Ptr &a, const Ptr &b, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    if (a->isFloatingPointExpr() && b->isFloatingPointExpr()) {
        return Interior::instance(OP_FP_MUL, a, b, solver, comment, flags);
    } else {
        return Interior::instance(OP_SMUL, a, b, solver, comment, flags);
    }
}

Ptr
makeSqrt(const Ptr &a, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    return Interior::instance(OP_FP_SQRT, a, solver, comment, flags);
}

Ptr
makeDiv(const Ptr &a, const Ptr &b, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    if (a->isFloatingPointExpr() && b->isFloatingPointExpr()) {
        return Interior::instance(OP_FP_DIV, a, b, solver, comment, flags);
    } else {
        return Interior::instance(OP_UDIV, a, b, solver, comment, flags);
    }
}

Ptr
makeExtend(const Ptr &newSize, const Ptr &a, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    return Interior::instance(OP_UEXTEND, newSize, a, solver, comment, flags);
}

Ptr
makeGe(const Ptr &a, const Ptr &b, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    if (a->isFloatingPointExpr() && b->isFloatingPointExpr()) {
        return Interior::instance(OP_FP_GE, a, b, solver, comment, flags);
    } else {
        return Interior::instance(OP_UGE, a, b, solver, comment, flags);
    }
}

Ptr
makeGt(const Ptr &a, const Ptr &b, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    if (a->isFloatingPointExpr() && b->isFloatingPointExpr()) {
        return Interior::instance(OP_FP_GT, a, b, solver, comment, flags);
    } else {
        return Interior::instance(OP_UGT, a, b, solver, comment, flags);
    }
}

Ptr
makeLe(const Ptr &a, const Ptr &b, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    if (a->isFloatingPointExpr() && b->isFloatingPointExpr()) {
        return Interior::instance(OP_FP_LE, a, b, solver, comment, flags);
    } else {
        return Interior::instance(OP_ULE, a, b, solver, comment, flags);
    }
}

Ptr
makeLt(const Ptr &a, const Ptr &b, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    if (a->isFloatingPointExpr() && b->isFloatingPointExpr()) {
        return Interior::instance(OP_FP_LT, a, b, solver, comment, flags);
    } else {
        return Interior::instance(OP_ULT, a, b, solver, comment, flags);
    }
}

Ptr
makeMod(const Ptr &a, const Ptr &b, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    if (a->isFloatingPointExpr() && b->isFloatingPointExpr()) {
        return Interior::instance(OP_FP_MOD, a, b, solver, comment, flags);
    } else {
        return Interior::instance(OP_UMOD, a, b, solver, comment, flags);
    }
}

Ptr
makeMul(const Ptr &a, const Ptr &b, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    if (a->isFloatingPointExpr() && b->isFloatingPointExpr()) {
        return Interior::instance(OP_FP_MUL, a, b, solver, comment, flags);
    } else {
        return Interior::instance(OP_UMUL, a, b, solver, comment, flags);
    }
}

Ptr
makeWrite(const Ptr &mem, const Ptr &addr, const Ptr &a, const SmtSolverPtr &solver, const std::string &comment,
          unsigned flags) {
    return Interior::instance(OP_WRITE, mem, addr, a, solver, comment, flags);
}

Ptr
makeZerop(const Ptr &a, const SmtSolverPtr &solver, const std::string &comment, unsigned flags) {
    if (a->isFloatingPointExpr()) {
        return Interior::instance(OP_FP_ISZERO, a, solver, comment, flags);
    } else {
        return Interior::instance(OP_ZEROP, a, solver, comment, flags);
    }
}

} // namespace
} // namespace
} // namespace
