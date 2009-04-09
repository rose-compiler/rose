#ifndef H_POINTSTO_IMPLEMENTATION
#define H_POINTSTO_IMPLEMENTATION

// This header declares internal details of the points-to analysis
// implementation. These details are meant to be compiled into the analysis,
// but are not part of the external interface of the analysis. Therefore,
// this header need not be provided to users, which is why it is not in the
// satire subdirectory.

#include <boost/pending/disjoint_sets.hpp>
#include "pointsto.h"

namespace SATIrE {

namespace Analyses {

struct PointsToAnalysis::Location
{
private:
 // Denotes the pointed-to location, if any.
    Location *base_location;

public:
    void pointTo(Location *base);
    Location *baseLocation() const;

 // List of function argument/parameter locations. Also abused as the
 // container of locations inside a SgExprListExp.
    std::vector<Location *> arg_locations;
 // The location corresponding to the function's ellipsis parameter, if any.
 // If this exists, it is a pointer equal to the last element of
 // arg_locations.
    Location *ellipsis_location;
 // Denotes the return location of this function location, that is, the
 // location containing the return value; non-NULL iff this node is a
 // function node. (FIXME: Is that really true for void functions?)
    Location *return_location;
 // Mapping associating struct locations with their member locations.
    std::map<SgSymbol *, Location *> struct_members;

    std::list<SgSymbol *> symbols;
    std::list<SgFunctionSymbol *> func_symbols;
    unsigned long id;
 // Some locations are "dummies" used only as temporary containers for a
 // sequence of locations, or for some object's address. As a program
 // invariant (TODO: enforce this using assertions), a dummy location can
 // never be another location's base location.
    bool dummy;
 // Some locations refer to constant literals in the source code --
 // integers, floats, strings. These are marked with the literal flag.
    bool literal;
 // Some locations denote allocation functions, or functions we want to
 // treat specially (polymorphically, i.e., in a context-sensitive way).
 // Each call to a special function creates a new set of locations for each
 // call site; this keeps allocation sites apart, as well as memcpy calls etc.
    bool special_function;
 // This field keeps track of the C type of the first dereference or member
 // access for this node. If a pointer is ever dereferenced/accessed as some
 // type, and there is another access of some other type, and at least one
 // of these is a struct type not compatible with the other type, then we
 // must treat this node as a collapsed structure.
    SgType *first_access_type;
 // This flag denotes whether we are dealing with a "collapsed" struct. This
 // is a structure that at some point is accessed in a way that we can't
 // tell which members are accessed/modified; this can be the case if the
 // structure is casted and accessed byte-wise, for instance. Once a struct
 // is collapsed, it is treated as a single pointer to whatever any member
 // may point to. In this case, all members are collapsed and unified with
 // the containing structure.
    bool collapsed;
 // Points upwards from a structure member node to the structure containing
 // the member.
    Location *parent_struct;
 // If this flag is true, whenever this location is dereferenced, the base
 // location must be collapsed; this flag is set when casting a structure to
 // some incompatible type, and actual collapsing is only caused by
 // dereferencing the pointer. FIXME: Is this still used, since we have
 // first_access_type?
    bool collapseBase;
 // This flag is set if there is at least one incoming points-to edge to
 // this node.
    bool mayBeAliased;

 // The list of "pending" locations: When assigning a non-pointer value, we
 // add the value to the pending list rather than forcing unification; a
 // pending edge from a to b means that a and b may contain the same value
 // at some point, of which value we assume that it is not a pointer.
 // However, should a ever become a pointer value -- i.e., be dereferenced
 // somewhere -- b must also become a pointer that points to the same
 // location as a.
    std::list<Location *> pending;

 // The info this location belongs to.
    PointsToAnalysis::PointsToInformation *ownerInfo;

    Location(Location *b = NULL, Location *r = NULL);
};


class PointsToAnalysis::PointsToInformation
{
public:
    PointsToInformation();

private:
    friend class PointsToAnalysis::Implementation;

    typedef boost::associative_property_map<std::map<Location *, int> >
            RankMap;
    typedef boost::associative_property_map<std::map<Location *, Location *> >
            ParentMap;
    typedef boost::disjoint_sets<RankMap, ParentMap> DisjointSets;
    std::map<Location *, int> rankMap;
    std::map<Location *, Location *> parentMap;
    DisjointSets disjointSets;

    std::map<SgSymbol *, Location *> symbol_locations;
    std::vector<Location *> locations;
 // This vector contains all the procedure locations created for procedures
 // in the ICFG, such that it can be indexed by procnum.
    std::vector<Location *> procedureLocations;

    std::map<std::string, Location *> function_locations;

 // Location constants
    Location *integerConstantLocation;
    Location *stringConstantLocation;
 // The set of functions that will get special handling using function
 // summaries.
    std::set<std::string> specialFunctionNames;

 // Some data structures we need so we can deal with non-unique function
 // symbols in the ROSE AST.
    std::map<std::string, SgFunctionSymbol *> canonicalFSymbols;
    std::map<std::string, std::set<SgFunctionSymbol *> >
                                              redundantFunctionSymbols;

 // This is the call graph, which is computed incrementally during the
 // traversal.
    CallGraph callGraph;
 // This map associates call site function expressions with their location
 // nodes. We use it after the points-to analysis to extract the function
 // symbols each call site may refer to.
    typedef std::map<SgExpression *, Location *> CallLocationMap;
    CallLocationMap callLocationMap;

 // Some random stuff that we need in some places.
    SgType *ptr_to_void_type;
 // the number of "auxiliary traversals" that have this info
    unsigned int auxctr;

 // context-sensitive stuff
    ContextInformation::Context *context;
    Location *procedureLocation;
    std::string prefix;
};


// This is the implementation class for the points-to analysis. It is pretty
// big and should probably be factored. Not just because of its size, but
// also because of its use of multiple inheritance...
class PointsToAnalysis::Implementation
  : private IcfgTraversal,
    private AstBottomUpProcessing<Location *>
{
public:
 // user interface
    void run(Program *program);
    void run(CFG *icfg);
    void print();
    void doDot(std::string filename);

    Location *expressionLocation(SgExpression *expr);
    Location *expressionLocation(SgExpression *expr,
                                 const ContextInformation::Context &context);
    std::string locationAttributeString(Location *location);

    const std::vector<Location *> &get_locations() const;
    bool mayBeAliased(Location *loc) const;
    const std::list<SgSymbol *> &location_symbols(Location *) const;
    unsigned long location_id(Location *loc) const;
    Location *location_representative(Location *loc);

    const CallGraph &getCallGraph();

 // unused
 // PointsToAnalysis::PointsToInformation *getPointsToInformation() const;

    Implementation(bool contextSensitive = false);

private:
 // The info field stores all the points-to information; via the second
 // constructor, we can share our info with other instances of the
 // implementation class, in particular with the auxiliary traversal
 // instance. This instance is used to start nested traversals.
    Implementation(PointsToAnalysis::PointsToInformation *info,
                   Implementation *icfgTraversal,
                   PointsToAnalysis::PointsToInformation *mainInfo);
    PointsToAnalysis::PointsToInformation *info;
    Implementation *auxiliaryTraversal;
 // This latter thing is something I'm not proud of. We use this member to
 // access stuff that is inherited from the IcfgTraversal class; this is
 // used by the auxiliaryTraversal to make sure it accesses the correct
 // ICFG. It is, essentially, a parent pointer from the auxiliaryTraversal
 // to the implementation instance it belongs to.
    Implementation *icfgTraversal;

 // Support for context sensitivity
    bool contextSensitive;
    std::map<ContextInformation::Context,
             PointsToAnalysis::PointsToInformation *> allInfos;
    PointsToInformation::PointsToInformation *mainInfo;

    Location *symbol_location(SgSymbol *sym);
    Location *symbol_location(SgSymbol *sym,
                              const ContextInformation::Context &context);
    Location *function_location(SgFunctionDeclaration *fd,
                                Location *argDummy = NULL);
    Location *functionSymbol_location(SgFunctionSymbol *fsym);
    Location *functionLocationForProcnum(int procnum);
    void determineSpecialFunction(std::string name, Location *func_location);

    Location *createLocation(Location *t, Location *return_location = NULL);
    Location *createLocation();
    Location *createFunctionLocation();
    Location *createLiteralLocation();
    Location *createDummyLocation(Location *base = NULL);
    void freeDummyLocation(Location *t);

    void print(std::ostream &stream, std::string pre, std::string post,
               std::string lineend);

 // ICFG traversal interface implementation function
    void atIcfgTraversalStart();
    void icfgVisit(SgNode *node);

    Location *evaluateSynthesizedAttribute(SgNode *node,
                                           SynthesizedAttributesList synlist);
    Location *handleIcfgStatement(IcfgStmt *icfgStmt,
                                  SynthesizedAttributesList synlist);

    Location *assign(Location *a, Location *b);
    Location *join(Location *a, Location *b);
    Location *cjoin(Location *a, Location *b);
    void join_pending_locations(Location *a);
    void unify(Location *a, Location *b);

 // helper functions
    void fillDummyLocation(Location *a, SynthesizedAttributesList synlist);
    void unifyArgListLocations(Location *a, Location *b,
                               bool functionCall = false);
    void debugDump(SgNode *node);
 // FIXME: fix the name of this function
    Location *pickThePointer(SgBinaryOp *binOp, Location *a, Location *b);
    Location *newAllocationSite();
    Location *newSpecialFunctionContext(std::string funcname,
                                        SynthesizedAttributesList synlist);
    bool isStructMemberReference(SgVarRefExp *varRef);
    Location *structMemberLocation(Location *structure,
                                   SgExpression *lhsExpr, SgVarRefExp *varRef);
    Location *structMemberLocation(Location *structure, SgSymbol *member);
    void collapseStructure(Location *structure);
    bool isNonCollapsedStructure(Location *structure);
 // FIXME: are these two still used?
 // bool isCastFromStructPtrToIncompatibleType(SgCastExp *cast);
 // bool isCastFromIncompatibleTypeToStructPtr(SgCastExp *cast);
    bool isUnionReference(SgExpression *expr);
    bool hasPointerMembers(Location *structure);
    Location *createStructMemberLocation(Location *structure, SgSymbol *sym);
    SgDeclarationStatementPtrList &membersOfExpressionClass(
            SgExpression *expr);
    std::vector<SgSymbol *> getVariableSymbolsFromDeclaration(
            SgDeclarationStatement *d);
    std::vector<SgSymbol *> classTypeMemberVariables(
            SgClassType *class_type);
    Location *initializerAssignment(SgType *type, Location *a, Location *b);
    Location *initializeClass(SgClassType *class_type,
                              Location *a, Location *b);
    Location *initializeArray(SgArrayType *array_type,
                              Location *a, Location *b);
    void materializeAllStructMembers(Location *structure,
                                     SgClassType *class_type);
    void checkOrSetFirstAccessType(Location *a, SgType *access_type);
    SgSymbol *findSymbolForInitializedName(SgInitializedName *initname);
#if 0 && DEPRECATED
    void unifyReturnLocation(Location *return_location);
#endif
    void performFunctionEndBookkeeping(SgFunctionDeclaration *fdecl);
    SgType *stripOnePointerLayer(SgType *type);
    SgFunctionSymbol *findFunctionSymbolFromDeclaration(
            SgFunctionDeclaration *fd);
    void ensurePointerLocation(Location * &location);
    void ensureArgumentCount(Location *, size_t n);
    SgFunctionSymbol *canonicalSymbol(SgFunctionSymbol *fsym);

#if 0
 // These instance variables pass information upwards in the AST (from the
 // leaves towards the root): In functions using varargs, va_list variables
 // must be unified with the enclosing function's ellipsis location. But the
 // function declaration is usually far away, and I don't want to trace
 // through parent pointers. This is, essentially, a synthesized attribute,
 // but hardly any node's action looks at it, and passing a structure around
 // as synthesized attribute would be a little clumsy.
 // IMPORTANT INVARIANT: This location may not escape functions, i.e., it MUST
 // be set to NULL at function declaration nodes (when leaving a function).
    Location *current_ellipsis_location;
 // This is the list of location that can be returned by this function. May not
 // escape the function.
    std::list<Location *> return_locations;
 // This is the list of call site function expressions in the current
 // function. May not escape the function.
    std::list<SgExpression *> callSiteExpressions;
#endif

    friend class PointsToAnalysis;
};

} // namespace Analyses

} // namespace SATIrE

#endif
