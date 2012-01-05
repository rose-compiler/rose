#ifndef __ApiDepFinder_H_LOADED__
#define __ApiDepFinder_H_LOADED__

#include <staticSingleAssignment.h>
#include <map>
#include <set>
#include <vector>

#include "APIDepAttrib.h"
#include "APISpec.h"

/** Find the dependencies of API calls. Some of the functionality
 * overlaps significantly with, but is not quite the same as, the
 * slicing code already included in ROSE. This implementation uses
 * only the def-use analysis of ROSE, and is therefore likely to be a
 * bit more efficient than slicing, which builds on an interprocedural
 * system dependency graph. */
class APIDepFinder : public AstBottomUpProcessing<bool> {
  public:

    APIDepFinder(StaticSingleAssignment *ssaParam, APISpecs *specs) {
        ssa = ssaParam;
        apiSpecs = specs;
        cType = new GenericDepAttribute(&callTable, 0);
        cType->setDepForm(APIDepAttribute::CONTROL);
    }

    /** Determine whether a given node is a function call expression
     * with an API function as its target. */
    APISpec *isAPICall(SgNode *n);

    /** Determine whether the current node is an API call. Returns a
     * boolean synthesized attribute indicating whether this node or
     * any of its children is an API call. If the current node is a
     * function declaration, and the current synthesized attribute is
     * true, then add the name of the function being declared to the
     * set of functions that make API calls. */
    virtual bool evaluateSynthesizedAttribute(SgNode *n,
        SynthesizedAttributesList childAttrs);

    /** Add nodes to the keep set not included in the initial
     * traversal. This includes calls to functions that make API
     * calls, and parent nodes of nodes already in the keep set. */
    void finalize(SgNode *n);

  private:

    StaticSingleAssignment *ssa;
    APISpecs *apiSpecs;
    name_table callTable;
    APIDepAttribute *cType;

    // Used internally for bookkeeping.
    std::set<SgName> users;

    /** Mark a statement with an API dependency attribute. */
    void mark(SgNode *n, const APIDepAttribute *attr);
    /** Is the given node already marked with an API dependency
     * attribute? */

    /** Add all ancestors of the given node to the set of nodes to
     * keep, along with necessary children of those ancestors.
     * FIXME:
     * this approach to determining control dependence only works if
     * control flow is tree structured. If the code contains goto
     * statements, it may be wrong. Ultimately, we want to use the
     * control dependency graph. */
    void markParents(SgNode *n, const APIDepAttribute *attr);

    /** Add the children necessary for well-formedness of the given
     * node. Loops, for instance, need a condition expression. */
    void markNeededChildren(SgNode *n, const APIDepAttribute *attr);

    void recordDefStmts(int indent, const APIDepAttribute *t, SgNode *n);
    void recordFuncDefStmts(int indent, const APIDepAttribute *t,
                            SgFunctionCallExp *fc);
};

#endif
