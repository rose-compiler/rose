// pointsto.h -- first attempt at a simple, efficient, and powerful
// unification-based flow- and context-insensitive points-to analysis for C.

// The basic abstraction computed by this analysis is the Location, an
// abstract memory region. Variables and functions (identified by their
// respective symbols) live in Locations. Pointers are modeled by points-to
// relations between Locations: each Location may have at most one "base
// location", which is what it may point to. In this unification-based
// analysis, Locations are merged when the same pointer may point to each of
// them; that is, if the program contains the assignment sequence
//      p = &a; p = &b;
// then p's Location will point to a Location that contains both variables a
// and b.
// Each array is treated as a single object, i.e., all members live in the
// same Location. It is assumed that array indexing and pointer arithmetic
// always stay in the same object (as required by the Standard), so these
// are safely ignored.
// In contrast, structs are treated field-sensitively: Each struct instance
// corresponds to a location, and each of its fields has its own location as
// well. Such structs are collapsed when needed (if pointer arithmetic is
// performed on a pointer to the structure or one of its members).

// In principle, the heap can be modeled using the same abstractions.
// However, this requires function summaries for allocation functions, which
// are not here yet.

#ifndef H_POINTSTO
#define H_POINTSTO

#include "cfg_support.h"
#include <satire.h>

namespace SATIrE {

namespace Analyses {

class PointsToAnalysis: public Analysis
{
public:
 // Implementation of the SATIrE::Analysis interface.
    virtual std::string identifier() const;
    virtual std::string description() const;

    virtual void run(Program *program);
    virtual void run(CFG *icfg);

    virtual void processResults(Program *program);

 // Dump a representation of the shape data structure to std::cout.
    void print();
 // Dump a dot representation of the shape data structure to a file with the
 // given name, and invoke dot on it.
    void doDot(std::string filename);

 // Opaque type representing the "abstract location" of an expression. This
 // "abstract location" is a representation of a memory region; expressions
 // referring to the same "location", i.e., region, *may* be aliased.
 // Expressions referring to distinct locations are definitely not aliased.
    struct Location;

 // This function returns an expression's location. For now, the expression
 // may be composed of dereference and member access operations and may use
 // variables (and constants as array subscripts). Two expressions may be
 // aliased iff this function returns the same pointer for both.
    Location *expressionLocation(SgExpression *expr);
    Location *expressionLocation(SgExpression *expr,
                                 const ContextInformation::Context &context);
 // Compute some unique string from the given location. There is a one-to-one
 // correspondence between valid Location pointers and these strings, so again
 // two expressions may be aliased iff the locationAttributeStrings for their
 // locations compare equal.
    std::string locationAttributeString(Location *location);

 // The points-to analysis computes a call graph of the program as it runs.
 // This type, and the associated function, give access to this call graph.
 // The graph is represented by two data structures: The
 // CallSiteExpressionMap maps each function (represented by its symbol) to
 // the list of function expressions at all call sites inside the function.
 // The list is in an order which does not necessarily agree with the the
 // lexical order of function expressions in the source file.
 // The CallCandidateMap maps each of the expressions from the first map to
 // the set of functions that may be referenced by that expression, based on
 // points-to information.
 // Thus, by composing the maps in the obvious way, you can get a mapping
 // from each function to every function it may call.
    struct CallGraph
    {
        typedef std::map<SgFunctionSymbol *, std::list<SgExpression *> >
            CallSiteExpressionMap;
        typedef std::map<SgExpression *, std::set<SgFunctionSymbol *> >
            CallCandidateMap;

        CallSiteExpressionMap callSiteExpressionMap;
        CallCandidateMap callCandidateMap;
    };
 // This function returns a reference to the internal call graph. Users
 // might want to make a copy, as this changes if the analysis is run again,
 // and is lost when the analyzer object is destructed.
    const CallGraph &getCallGraph();

 // The location corresponding to a given variable or function symbol.
    Location *symbol_location(SgSymbol *sym);
    Location *symbol_location(SgSymbol *sym,
                              const ContextInformation::Context &context);
 // Whether the given location may be aliased, i.e. whether some other
 // location may point to it.
    bool mayBeAliased(Location *loc) const;
 // Whether the given location is an array (the array itself, not its
 // address).
    bool isArrayLocation(Location *loc) const;
 // List of all (variable or function) symbols associated with a given
 // location. Any expression referencing this location may access (only) one
 // of these objects.
    const std::list<SgSymbol *> &location_symbols(Location *loc) const;
 // A unique numerical identifier for the given location. Not strictly
 // necessary, as pointers are just as unique.
    unsigned long location_id(Location *loc) const;
 // Locations may contain pointer values, i.e., point to other locations.
 // base_location will give the pointed-to location, if any; valid_location
 // tests whether base_location's return value is indeed a pointed-to
 // location. To test whether a location l is a pointer location, call
 // valid_location(base_location(l)).
    Location *base_location(Location *loc);
    bool valid_location(Location *loc) const;

    PointsToAnalysis(bool contextSensitive = false);
    ~PointsToAnalysis();


 // ----- FOR INTERNAL USE ONLY -----

 // The list of *all* locations. For internal use only, as these also
 // include non-canonical locations.
    const std::vector<Location *> &get_locations() const;
 // The representative of the given location's equivalence class. For
 // internal use only, as users always get representatives anyway.
    Location *location_representative(Location *loc);

private:
 // Forbid copying.
    PointsToAnalysis(const PointsToAnalysis &);
    PointsToAnalysis &operator=(const PointsToAnalysis &);

 // The "pimpl" idiom ensures that clients of this class need not be
 // recompiled if internal details are changed.
    struct Implementation;
    const std::auto_ptr<Implementation> p_impl;

 // This type encapsualtes the "points-to information", i.e., the analysis
 // results. This is of no public use.
    class PointsToInformation;
 // unused
 // PointsToInformation *getPointsToInformation() const;
};

} // namespace Analyses

} // namespace SATIrE

#endif
