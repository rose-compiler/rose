#include <numeric>
#include <boost/foreach.hpp>

#include "rose.h"

#include "Utils.h"
#include "APIDepFinder.h"
#define foreach BOOST_FOREACH

static const std::string APIDep("APIDep");

/** Is node @n@ marked with an attribute that contains @attr@? */
static bool marked(SgNode *n, const APIDepAttribute *attr) {
    if( n->getAttribute(APIDep) != NULL ){
      const APIDepAttribute * const t = dynamic_cast<const APIDepAttribute * const>(n->getAttribute(APIDep));
      ROSE_ASSERT( t != NULL );
      return t->contains(attr);
    }
    return false;
}

/** Create a new attribute similar to @attr@ but with dependence form
 * including @depForm@. */
static APIDepAttribute *copyDep(const APIDepAttribute *attr,
                                APIDepAttribute::DependenceForm depForm) {
    APIDepAttribute * tData;
    const APIDepAttribute * const tCopy = dynamic_cast<const APIDepAttribute * const>(attr);
    ROSE_ASSERT( tCopy != NULL );
    if(tCopy->getDepForm() != depForm) {
        // NB: throwing away 'const' info to work w/ copy.
        tData = const_cast<APIDepAttribute *>(tCopy->copy());
        ROSE_ASSERT( tData != NULL );
        tData->joinForm(depForm);
    } else {
        // NB: throwing away 'const' info to work w/ copy.
        tData = const_cast<APIDepAttribute *>(tCopy);
    }
    return tData;
}

/** Create an attribute similar to @attr@ but marked as a data
 * dependency. */
static APIDepAttribute *dataDep(const APIDepAttribute *attr) {
    return copyDep(attr, APIDepAttribute::DATA);
}

/** Create an attribute similar to @attr@ but marked as a control
 * dependency. */
static APIDepAttribute *ctrlDep(const APIDepAttribute *attr) {
    return copyDep(attr, APIDepAttribute::CONTROL);
}

/** Mark node @n@ with attribute @attr@, if it's not already marked
 * with a matching attribute. Then add necessary children of @n@,
 * along with the parents of @n@, to approximate control
 * dependencies. */
void APIDepFinder::mark(SgNode *n, const APIDepAttribute *attr) {
    APIDepAttribute *newAttr;
    if( n->getAttribute(APIDep) != NULL ){
      const APIDepAttribute * const oldAttr = dynamic_cast<const APIDepAttribute *>(n->getAttribute(APIDep));
      ROSE_ASSERT( oldAttr != NULL );
      if(oldAttr->matches(attr)) return;
      newAttr = oldAttr->copy();
      newAttr->join(attr);
    } else {
        newAttr = attr->copy(); // Make a copy to avoid throwing away const
    }
    if(debug) {
        std::cout << "Marking: "
                  << n->unparseToString()
                  << " (" <<  newAttr->toString() << ")"
                  << std::endl;
    }
    n->setAttribute(APIDep, newAttr);

    APIDepAttribute * const attrCtrl = ctrlDep(newAttr);
    markNeededChildren(n, attrCtrl);
    markParents(n, attrCtrl);
}

/** For each child of @n@ that must be included for @n@ to be
 * well-formed, add attribute @attr@. */
void APIDepFinder::markNeededChildren ( SgNode *n
                                      , const APIDepAttribute *attr)
{
    SgForStatement    * const fs = isSgForStatement(n);
    SgWhileStmt       * const ws = isSgWhileStmt(n);
    SgDoWhileStmt     * const ds = isSgDoWhileStmt(n);
    SgIfStmt          * const is = isSgIfStmt(n);
    SgSwitchStatement * const ss = isSgSwitchStatement(n);

    if(fs) {
        SgForInitStatement * const init = fs->get_for_init_stmt();
        mark(init, attr);
        SgStatementPtrList inits = init->get_init_stmt();
        SgStatementPtrList::iterator ii = inits.begin();
        while(ii != inits.end()) {
            mark(*ii, attr);
            ++ii;
        }
        mark(fs->get_test(), attr);
        mark(fs->get_increment(), attr);
        mark(fs->get_loop_body(), attr);
    } else if(ws) {
        mark(ws->get_condition(), attr);
        mark(ws->get_body(), attr);
    } else if(ds) {
        mark(ds->get_condition(), attr);
        mark(ds->get_body(), attr);
    } else if(is) {
        mark(is->get_conditional(), attr);
    } else if(ss) {
        mark(ss->get_item_selector(), attr);
    } else {
        if(debug) {
            std::cout << "No needed children for "
                      << n->unparseToString()
                      << std::endl;
        }
    }
}

/** Mark @n@ and all of its parents with attribute @attr@. */
void APIDepFinder::markParents(SgNode *n, const APIDepAttribute *attr) {
    SgStatement *stmt = isSgStatement(n);
    if(stmt && !marked(stmt, attr)) {
        if (debug)
            std::cout << "Adding "
                      << stmt->unparseToString()
                      << std::endl;
        mark(stmt, attr);
    }
    if(n) markParents(n->get_parent(), attr);
}

/** Does @n@ represent an API call, as defined by the @APISpecs@ for
 * this instance? */
APISpec *APIDepFinder::isAPICall(SgNode *n) {
    SgSymbol *fsym = findFunctionHead(n);
    if(fsym) {
        return isAPICall(fsym->get_name());
    }
    return NULL;
}

APISpec *APIDepFinder::isAPICall(const std::string & name) const {
    APISpec *spec = lookupFunction(apiSpecs, name);
    return spec;
}

/** Perform a bottom-up traversal of the AST rooted at @n@,
 * calculating a synthesized attribute that indicates whether the
 * current node contains any API calls. Along the way, mark all data
 * and control dependencies of each API call encountered. */
/* TODO: replace this with something that just looks for
 * calls and propagates attributes from them? */
bool APIDepFinder::evaluateSynthesizedAttribute(SgNode *n,
    SynthesizedAttributesList childAttrs) {

    bool localResult =
        std::accumulate(childAttrs.begin(), childAttrs.end(),
                        false, std::logical_or<bool>());
    SgFunctionCallExp *fc = isFunctionCall(n);

    /* If this is a function call and we haven't already processed it,
     * then go ahead and process it. */
    if(fc && argTable.find(fc->getAssociatedFunctionSymbol()) == argTable.end()) {
        APISpec *spec = isAPICall(n);
        if(spec) {
            recordFuncDefStmts(0, spec->callType(), fc);
            localResult = true;
        } else {
            localResult = handleOtherCall(fc);
        }
    }
    return localResult;
}

/** "other call" means non-API call */
bool APIDepFinder::handleOtherCall(SgFunctionCallExp *fc) {
    Rose_STL_Container<SgFunctionDeclaration *> decls;
    CallTargetSet::getPropertiesForExpression(isSgExpression(fc), chw, decls);
    bool hasAPICalls = false;
    foreach(SgFunctionDeclaration *decl, decls) {
       // 1. Look in the transitive call graph for decl
       if( transitiveCalls == NULL ) return hasAPICalls;
       // FIXME: this should use a qualified name, possibly with mangling
       // The reason is that, now that we are considering the full call graph we
       // need names to be unique across the program -- not just across the compilation unit.
       Signatures::const_iterator c = transitiveCalls->find(decl->get_name());
       if( c != transitiveCalls->end() ){
          // 2. Look for API calls in the list of calls
          if(debug) std::cout << c->first << ": " << c->second << std::endl; 
          foreach(const std::string & callee, c->second.calls){
             const APISpec * const spec = isAPICall(callee);
             if(spec) {
                // 3. We found an API call inside the function so we record fc as
                // as having the same callType as the API call.
                recordFuncDefStmts(0, spec->callType(), fc);
                hasAPICalls = true;
             }
          }
       }
    }
    return hasAPICalls;
}

/** Record data and control dependencies for a function call. */
void APIDepFinder::recordFuncDefStmts( int indent
                                     , const APIDepAttribute *t
                                     , SgFunctionCallExp *fc) {
    SgFunctionSymbol *fsym = fc ? fc->getAssociatedFunctionSymbol() : NULL;
    if ( fsym ) {
        SgExprListExp *args       = fc->get_args();
        mark(fc, t);

        // FIXME[C1]: is the following mark really necessary? Won't it
        // be handled by the control dependency analysis later on?
        //SgStatement *enclosing = SageInterface::getEnclosingStatement(fc);
        //mark(enclosing, t);

        if ( isAPICall(fc) ) {
            if (debug) {
                for (int c = 0; c < indent; c++)
                    std::cout << " ";
                std::cout << "API call: " << fsym->get_name().str()
                                                                << std::endl;
            }

            APISpec *spec             = lookupFunction(apiSpecs, fsym->get_name());
            ArgTreatments *treatments = spec->getFunction(fsym->get_name());

            SgExpressionPtrList exps  = args->get_expressions();

            for ( size_t j = 0 ; j < exps.size() ; j++ ) {
                if(debug) {
                    for (int c = 0; c < indent; c++)
                        std::cout << " ";
                    std::cout << "Processing argument "
                              << exps[j]->unparseToString()
                              << std::endl;
                }
                ROSE_ASSERT( j < treatments->size() );
                APIDepAttribute *at = (APIDepAttribute *) (*treatments)[j];
                mark(exps[j], at);
                APIDepAttribute *tData = dataDep(at);
                recordDefStmts(indent + 2, tData, exps[j]);
            }
        } else {
            APIDepAttribute *tData = dataDep(t);
            recordDefStmts(indent, tData, args);
        }
    }
}

/** Record data and control dependencies for an arbitrary AST node. */
void APIDepFinder::recordDefStmts( int indent
                                 , const APIDepAttribute *t
                                 , SgNode *n) {
    SgFunctionCallExp *fc = isFunctionCall(n);
    if(fc) {
        if(!isAPICall(n)) {
            recordFuncDefStmts(indent, t, fc);
        }
        /* If n is an API call, its dependencies will be calculated
         * separately. */
        return;
    }

    std::set<SgNode *> defs = getNodeVarDefsSSA(ssa, n);
    std::set<SgNode *>::iterator di = defs.begin();
    if (debug) {
        for (int c = 0; c < indent; c++) std::cout << " ";
        std::cout << "Recording defining statements for "
                  << n->unparseToString() << std::endl;
    }
    for(; di != defs.end(); ++di) {
        if(!marked(*di, t) && *di != n) {
            mark(*di, t);
            if (debug) {
                for (int c = 0; c < indent; c++) std::cout << " ";
                std::cout << "Recording defining statement: "
                          << (*di)->unparseToString() << std::endl;
            }
            /* FIXME: should this conditional exist? The way it's
             * written, we'll miss dependencies on initializers within
             * declarations. Will declarations without initializers
             * depend on anything? */
            if(!isSgDeclarationStatement(*di)) {
                APIDepAttribute *tData = dataDep(t);
                recordDefStmts(indent + 2, tData, *di);
            }
        }
    }
}

