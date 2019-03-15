#include <boost/foreach.hpp>

#include <rose.h>
#include "APISpec.h"
#include "APIReader.h"
#include "APIDepFinder.h"
#include "DangerousOperationFinder.h"
#include "APIDepChecker.h"
#include "processPragmas.h"
#include "annotatePragmas.h"

#include <staticSingleAssignment.h>
#include <iostream>
#define foreach BOOST_FOREACH

int main(int argc, char **argv) {

    bool debug  = false;   // if true, generate lots of debugging output
    const std::string signature("-signature:");

    //
    // Local Command Line Processing:
    //
    Rose_STL_Container<std::string> l =
      CommandlineProcessing::generateArgListFromArgcArgv (argc,argv);
    if ( CommandlineProcessing::isOption(l,signature,"(d|debug)",true) ) {
        debug = true;
    }
    std::string sig_fname;
    if( CommandlineProcessing::isOptionWithParameter(l, signature,
                                                     "(o|output)",
                                                     sig_fname,
                                                     true) ){
        std::cout << "Using filename for output: " << sig_fname << std::endl;
    } else {
        std::cout << "Warning: No output filename given. Using standard out." << std::endl;
    }

    //
    // parse and create project
    //
    SgProject* const project = frontend(l);
    //
    // Run the SSA analysis
    //
    if(debug) std::cout << "Running SSA analysis"               << std::endl;
    StaticSingleAssignment ssa(project);
    ssa.run(true, true);

    // In an ideal world we would query just for function definitions,
    // but we don't live in an ideal world.
    //
    // Instead we need to either:
    // a) query for SgFunctionDeclarations and filter out the forward
    // declarations, or;
    // b) query for SgFunctionDefinitions and filter out the ones that
    // don't correspond to function declarations (eg., they are some
    // other form of scope like a class or namespace).
    // Here we arbitrarily go with option (a).
    Signatures sigs;
    const std::vector<SgFunctionDeclaration*> funDecls(SageInterface::querySubTree<SgFunctionDeclaration>(project));
    if(debug) std::cout << "Found " << funDecls.size() << " function declarations" << std::endl;
    foreach(SgFunctionDeclaration * const funDecl, funDecls){
       SgFunctionDefinition * const funDef = funDecl->get_definition();
       if( funDef == NULL ) continue; // Must be a forward decl, nothing to see here.
       if(debug) std::cout << funDecl->get_name().getString() << " calls: ";
       Signature sig;
       sig.argCount = funDecl->get_args().size();
       const std::vector<SgCallExpression*> callExprs(SageInterface::querySubTree<SgCallExpression>(funDef));
       if(debug) std::cout << "Found " << callExprs.size() << std::endl;
       foreach(SgCallExpression * const callExpr, callExprs){
         SgFunctionCallExp * const funCall = isSgFunctionCallExp(callExpr);
         ROSE_ASSERT( funCall != NULL );
         const SgExpression          * const fun         = funCall->get_function();
         const SgFunctionSymbol      * const funCallSym  = funCall->getAssociatedFunctionSymbol(); // if it can be resolved statically...
         const SgFunctionDeclaration * const funCallDecl = funCall->getAssociatedFunctionDeclaration();
         std::string funName;
         // Ideally we just use the function symbol or declaration to get the
         // function name.  The ROSE docs say that the symbol and declaration
         // will only be defined when ROSE can statically resolve the function
         // call. So we check for the symbol and declaration. If they are
         // defined we use them, otherwise we fall back on unparsing the
         // function call. FIXME: This really needs better testing. It could
         // be that unparsing is woefully unsound.
         if( funCallSym != NULL ){
           funName = funCallSym->get_name();
         } else if( funCallDecl != NULL) {
           if(debug) std::cout << "Could not statically resolve function symbol. Using function declaration." << std::endl;
           funName = funCallDecl->get_name();
         } else {
           if(debug) std::cout << "Could not statically resolve function symbol or declaration. Using unparseToString() on function call node." << std::endl;
           funName = fun->unparseToString(); // HACK: does this work in general?
         }
         if(debug) std::cout << "Call to " << funName << std::endl;
         sig.calls.insert(funName);
         SgExprListExp * const argList = callExpr->get_args();
         ROSE_ASSERT( argList != NULL );
         // Here we search over the variables used in the function call and try to match
         // up their definition with the parameters of the function we're inside of.
         // If we have a match, we construct an ArgMapsTo and insert it into the
         // signature we're building.
         std::vector<SgExpression*> argListExprs = argList->get_expressions();
         for(size_t calleeArg = 0; calleeArg < argListExprs.size(); ++calleeArg){
           const StaticSingleAssignment::NodeReachingDefTable & nodeReachingDefTable = ssa.getUsesAtNode(argListExprs[calleeArg]);
           foreach( const StaticSingleAssignment::NodeReachingDefTable::value_type & nodeReachingDef, nodeReachingDefTable ){
             std::vector<SgInitializedName *> funArgList = funDecl->get_args();
             for(size_t callerArg = 0; callerArg < funArgList.size(); ++callerArg){
               if( funArgList[callerArg] == nodeReachingDef.second->getDefinitionNode() ){
                 if (debug) std::cout << "Found a match! " << funArgList[callerArg]->unparseToString() << std::endl;
                 insertArgMapping(sig.argMapping, ArgMapsTo(callerArg, funName, calleeArg));
               }
             }
           }
         }
       }
       if(debug) std::cout << std::endl;
       sigs[funDecl->get_name()] = sig;
    }
    if(debug) std::cout << sigs << std::endl;
#if __cplusplus < 201103L
    std::auto_ptr<SExpr> sx(toSExpr(sigs));
#else
    std::unique_ptr<SExpr> sx(toSExpr(sigs));
#endif
    if(debug || sig_fname == "") std::cout << *sx << std::endl;
    if( sig_fname != "" ){
       std::ofstream out(sig_fname.c_str());
       if(out.bad()) exit(-1);
       out << *sx << std::endl;
    }

    return 0;
}
