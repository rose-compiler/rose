#ifndef __ApiDepFinder_H_LOADED__
#define __ApiDepFinder_H_LOADED__

#include <staticSingleAssignment.h>
#include <map>
#include <set>
#include <vector>

#include "APIDepAttrib.h"
#include "APISpec.h"
#include "SignatureReader.h"

/** Find the dependencies of API calls. Some of the functionality
 * overlaps significantly with, but is not quite the same as, the
 * slicing code already included in ROSE. This implementation uses
 * only the def-use analysis of ROSE, and is therefore likely to be a
 * bit more efficient than slicing, which builds on an interprocedural
 * system dependency graph. */
class APIDepFinder : public AstBottomUpProcessing<bool> {
  public:

    APIDepFinder(StaticSingleAssignment *ssaParam,
                 std::map<SgSymbol *, SgFunctionDeclaration *> decls,
                 ClassHierarchyWrapper *ch,
                 APISpecs *specs, const Signatures * transitiveCalls)
    : ssa(ssaParam), apiSpecs(specs), chw(ch), declTable(decls)
    , transitiveCalls(transitiveCalls) {
        cType = new GenericDepAttribute(&callTable, 0);
        cType->setDepForm(APIDepAttribute::CONTROL);
    }

    APIDepFinder(StaticSingleAssignment *ssaParam,
                 std::map<SgSymbol *, SgFunctionDeclaration *> decls,
                 ClassHierarchyWrapper *ch,
                 APISpecs *specs)
    : ssa(ssaParam), apiSpecs(specs), chw(ch), declTable(decls)
    , transitiveCalls(NULL) {
        cType = new GenericDepAttribute(&callTable, 0);
        cType->setDepForm(APIDepAttribute::CONTROL);
    }

    /** Determine whether a given node is a function call expression
     * with an API function as its target. */
    APISpec *isAPICall(SgNode *n);
    APISpec *isAPICall(const std::string & name) const;

    /** Determine whether the current node is an API call. Returns a
     * boolean synthesized attribute indicating whether this node or
     * any of its children is an API call. If the current node is a
     * function declaration, and the current synthesized attribute is
     * true, then add the name of the function being declared to the
     * set of functions that make API calls. */
    virtual bool evaluateSynthesizedAttribute(SgNode *n,
        SynthesizedAttributesList childAttrs);

    std::map< SgSymbol *, std::vector<APIDepAttribute *> * > getArgTable()
        { return argTable; }
    void setArgTable(std::map< SgSymbol*, std::vector< APIDepAttribute* >* > at)
        { argTable = at; }
    
  private:

    StaticSingleAssignment *ssa;
    APISpecs *apiSpecs;
    name_table callTable;
    APIDepAttribute *cType;
    ClassHierarchyWrapper *chw;
    std::map< SgSymbol*, SgFunctionDeclaration* > declTable;
    std::map< SgSymbol*, std::vector< APIDepAttribute* >* > argTable;
    // When this isn't defined, don't use it.
    const Signatures * const transitiveCalls;

    /** Mark a statement with an API dependency attribute. */
    void mark(SgNode *n, const APIDepAttribute *attr);

    /** 
     * Add all ancestors of the given node to the set of nodes to
     * keep, along with necessary children of those ancestors.
     * FIXME:
     * this approach to determining control dependence only works if
     * control flow is tree structured. If the code contains goto
     * statements, it may be wrong. Ultimately, we want to use the
     * control dependency graph. 
     */
    void markParents(SgNode *n, const APIDepAttribute *attr);

    /** Add the children necessary for well-formedness of the given
     * node. Loops, for instance, need a condition expression. */
    void markNeededChildren(SgNode *n, const APIDepAttribute *attr);

    void recordDefStmts(int indent, const APIDepAttribute *t, SgNode *n);
    bool handleOtherCall(SgFunctionCallExp *fc);
    void recordFuncDefStmts(int indent, const APIDepAttribute *t,
                            SgFunctionCallExp *fc);
};

#endif
