#include <numeric>
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#include "Utils.h"
#include "APIDepFinder.h"

/** Is node @n@ marked with an attribute that contains @attr@? */
static bool marked(SgNode *n, const APIDepAttribute *attr) {
    APIDepAttribute *t = (APIDepAttribute *)n->getAttribute("APIDep");
    return t ? t->contains(attr) : false;
}

/** Create a new attribute similar to @attr@ but with dependence form
 * including @depForm@. */
static APIDepAttribute *copyDep(const APIDepAttribute *attr,
                                APIDepAttribute::DependenceForm depForm) {
    APIDepAttribute *tData;
    APIDepAttribute *tCopy = (APIDepAttribute *) attr;
    if(tCopy->getDepForm() != depForm) {
        // NB: throwing away 'const' info to work w/ copy.
        tData = (APIDepAttribute *)tCopy->copy();
        tData->joinForm(depForm);
    } else {
        tData = tCopy;
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
    APIDepAttribute *oldAttr = (APIDepAttribute *)n->getAttribute("APIDep");
    APIDepAttribute *newAttr;
    if(oldAttr) {
        if(oldAttr->matches(attr)) return;
        newAttr = oldAttr->copy();
        newAttr->join(attr);
    } else {
        newAttr = (APIDepAttribute *) attr;
    }
    if(debug) {
        std::cout << "Marking: " << n->unparseToString()
                  << " (" <<  newAttr->toString() << ")" << std::endl;
    }
    n->setAttribute("APIDep", newAttr);

    APIDepAttribute *attrCtrl = ctrlDep(newAttr);
    markNeededChildren(n, attrCtrl);
    markParents(n, attrCtrl);
}

/** For each child of @n@ that must be included for @n@ to be
 * well-formed, add attribute @attr@. */
void APIDepFinder::markNeededChildren ( SgNode *n
                                      , const APIDepAttribute *attr)
{
    SgForStatement *fs = isSgForStatement(n);
    SgWhileStmt *ws = isSgWhileStmt(n);
    SgDoWhileStmt *ds = isSgDoWhileStmt(n);
    SgIfStmt *is = isSgIfStmt(n);
    SgSwitchStatement *ss = isSgSwitchStatement(n);

    if(fs) {
        SgForInitStatement *init = fs->get_for_init_stmt();
        mark(init, attr);
        SgStatementPtrList inits = init->get_init_stmt();
        SgStatementPtrList::iterator ii = inits.begin();
        while(ii != inits.end()) {
            mark(*ii, attr);
            ii++;
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
            std::cout << "No needed children for " << n->unparseToString()
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
    SgFunctionCallExp *fc = isSgFunctionCallExp(n);
    SgFunctionRefExp *f =
        fc ? isSgFunctionRefExp(fc->get_function()) : NULL;
    if(f) {
        SgFunctionSymbol *sym = f->get_symbol_i();
        SgName name = sym->get_name();
        APISpec *spec = lookupFunction(apiSpecs, name.str());
        if(spec) return spec;
    }
    return NULL;
}

/** Perform a bottom-up traversal of the AST rooted at @n@,
 * calculating a synthesized attribute that indicates whether the
 * current node contains any API calls. Along the way, mark all data
 * and control dependencies of each API call encountered. */
/* TODO: replace this with something that just looks for
 * calls and propagates attributes from them? */
bool APIDepFinder::evaluateSynthesizedAttribute(SgNode *n,
    SynthesizedAttributesList childAttrs)
{
    bool localResult =
        std::accumulate(childAttrs.begin(), childAttrs.end(),
                        false, std::logical_or<bool>());
    SgFunctionCallExp *fc = isSgFunctionCallExp(n);
    /* If this is a function call and we haven't already processed it,
     * then go ahead and process it. */
    if(fc && argTable.find(getEnclosingSym(fc)) == argTable.end()) {
        APISpec *spec = isAPICall(n);
        if(spec) {
            recordFuncDefStmts(0, spec->callType(), isSgFunctionCallExp(n));
            localResult = true;
        } else {
            localResult = handleOtherCall(fc);
        }
    }
    return localResult;
}

bool APIDepFinder::handleOtherCall(SgFunctionCallExp *fc) {
    Rose_STL_Container<SgFunctionDeclaration *> decls;
    CallTargetSet::getPropertiesForExpression(isSgExpression(fc), chw, decls);
    bool hasAPICalls = false;
    foreach(SgFunctionDeclaration *decl, decls) {
        SgSymbol *sym = decl->search_for_symbol_from_symbol_table();
        if(debug)
            std::cout << "Found declaration for call to "
                      << sym->get_name().getString() << std::endl;
        std::vector<APIDepAttribute *> *argAttrs = NULL;
        SgExprListExp *args = fc->get_args();
        SgExpressionPtrList exps = args->get_expressions();
        if(defTable.find(sym) == defTable.end()) {
            if(debug)
                std::cout << "No def for "
                          << sym->get_name().getString()
                          << std::endl;
            continue;
        } else if(argTable.find(sym) == argTable.end()) {
            followCall(sym);
        }
        argAttrs = argTable[sym];
        /* TODO: better sharing with recordFuncDefStmts */
        /* TODO: check that argAttrs and exps are the same size? */
        for (size_t j = 0; j < argAttrs->size(); j++) {
            APIDepAttribute *at = (*argAttrs)[j];
            if(at) {
                if(debug)
                    std::cout << "Marking argument "
                              << exps[j]->unparseToString()
                              << std::endl;
                mark(exps[j], at);
                APIDepAttribute *tData = dataDep(at);
                recordDefStmts(2, tData, exps[j]);
                APIDepAttribute *tCtrl = ctrlDep(at);
                mark(fc, tCtrl);
                hasAPICalls = true;
            }
        }
    }
    return hasAPICalls;
}

void APIDepFinder::followCall(SgSymbol *sym) {
    SgFunctionDefinition *def = defTable[sym];
    SgFunctionDeclaration *decl = def->get_declaration();
    std::vector<APIDepAttribute *> *attrs = new std::vector<APIDepAttribute *>;
    APIDepFinder *subFinder = new APIDepFinder(ssa, defTable, chw, apiSpecs);
    subFinder->setArgTable(argTable);
    if(debug)
        std::cout << "Following call to "
                  << sym->get_name().getString()
                  << std::endl;
    subFinder->traverse(def);
    foreach(SgInitializedName *n, decl->get_args()) {
        APIDepAttribute *t = (APIDepAttribute *)n->getAttribute("APIDep");
        attrs->push_back(t);
    }
    std::map<SgSymbol *, std::vector<APIDepAttribute *> *> args = subFinder->getArgTable();
    argTable.insert(args.begin(), args.end());
    argTable[sym] = attrs;
    delete subFinder;
}

/** Record data and control dependencies for a function call. */
void APIDepFinder::recordFuncDefStmts( int indent
                                     , const APIDepAttribute *t
                                     , SgFunctionCallExp *fc) {
    SgFunctionRefExp *f = fc ? isSgFunctionRefExp(fc->get_function()) : NULL;
    if(f) {
        SgFunctionSymbol *sym = f->get_symbol_i();
        SgName name = sym->get_name();
        SgExprListExp *args = fc->get_args();
        mark(fc, t);

        // FIXME[C1]: is the following mark really necessary? Won't it
        // be handled by the control dependency analysis later on?
        //SgStatement *enclosing = SageInterface::getEnclosingStatement(fc);
        //mark(enclosing, t);

        if(isAPICall(fc)) {
            if (debug) {
                for (int c = 0; c < indent; c++) std::cout << " ";
                std::cout << "API call: " << name.str() << std::endl;
            }

            APISpec *spec = lookupFunction(apiSpecs, name.str());
            ArgTreatments *treatments = spec->getFunction(name.str());
            SgExpressionPtrList exps = args->get_expressions();
            for (size_t j = 0; j < exps.size(); j++) {
                if(debug) {
                    for (int c = 0; c < indent; c++) std::cout << " ";
                    std::cout << "Processing argument "
                              << exps[j]->unparseToString()
                              << std::endl;
                }
                APIDepAttribute *at = (APIDepAttribute *)(*treatments)[j];
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
    SgFunctionCallExp *fc = isSgFunctionCallExp(n);
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
    for(; di != defs.end(); di++) {
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
