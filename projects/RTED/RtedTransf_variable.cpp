/*
 * RtedTransf_variable.cpp
 *
 *  Created on: Jul 7, 2009
 *      Author: panas2
 */

#include <rose.h>

// DQ (2/9/2010): Testing use of ROE to compile ROSE.
#ifndef USE_ROSE

#include <string>
#include <boost/foreach.hpp>
#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"

#include "rosez.hpp"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

// ------------------------ VARIABLE SPECIFIC CODE --------------------------

bool RtedTransformation::isVarInCreatedVariables(SgInitializedName* n) {
        bool ret = false;
        ROSE_ASSERT(n);
        SgInitializedNamePtrList::const_iterator it=variable_declarations.begin();
        for (;it!=variable_declarations.end();++it) {
                SgInitializedName* initName = *it;
                if (initName==n) {
                        ret=true;
                        break;
                }
        }
        return ret;
}

bool isFileIOVariable(SgType* type)
{
        std::string name = type->unparseToString();

        return name == "std::fstream";
}

void RtedTransformation::visit_isSgVariableDeclaration(SgNode* n) {
        SgVariableDeclaration* varDecl = isSgVariableDeclaration(n);

        // FIXME 2 djh: perhaps n->get_parent should be n->get_scope ?
        // don't track members of user types (structs, classes)
        if (isSgClassDefinition(varDecl -> get_parent()))
                return;
        SgInitializedNamePtrList vars = varDecl->get_variables();
        SgInitializedNamePtrList::const_iterator it = vars.begin();
        cerr << " ...... CHECKING Variable Declaration " << endl;
        for (;it!=vars.end();++it) {
                SgInitializedName* initName = *it;
                ROSE_ASSERT(initName);

                SgType* var_type = initName -> get_type();
                // FIXME 2: probably need to handle typedef to reference, recursive
                // typedef/reference, &c.
                // reference types don't create more memory as far as the RTS is
                // concerned (in that &foo == &bar for bar a ref. of foo)
                if( isSgReferenceType( var_type ))
                continue;

                // need to get the type and the possible value that it is initialized with
                cerr << "      Detected initName : " << initName->unparseToString();
                cerr <<"  type : " << initName->get_type()->unparseToString() << endl;
                variable_declarations.push_back(initName);

        }
}

void RtedTransformation::insertCreateObjectCall(RtedClassDefinition* rcdef) {
        SgClassDefinition*            cdef = rcdef -> classDef;
        SgDeclarationStatementPtrList constructors;

        appendConstructors(cdef, constructors);

        BOOST_FOREACH( SgDeclarationStatement* decl, constructors )
        {
            SgMemberFunctionDeclaration* constructor = isSgMemberFunctionDeclaration( decl );
            // validate the postcondition of appendConstructors
            ROSE_ASSERT( constructor );

            // FIXME 2: Probably the thing to do in this case is simply to bail and
            // trust that the constructor will get transformed when its definition
            // is processed
            SgFunctionDefinition* def = constructor -> get_definition();
            ROSE_ASSERT( def );

            SgBasicBlock* body = def -> get_body();

            // We need the symbol to build a this expression
            SgSymbolTable* sym_tab
            = cdef -> get_declaration() -> get_scope() -> get_symbol_table();

            SgClassSymbol* csym
            = isSgClassSymbol(
                            sym_tab -> find_class(
                                            cdef -> get_declaration() -> get_name() ));
            ROSE_ASSERT( csym );

            SgType* type = cdef -> get_declaration() -> get_type();

            // build arguments to roseCreateObject
            SgExprListExp* arg_list = buildExprListExp();

            appendExpression( arg_list, ctorTypeDesc(mkTypeInformation(type)) );

            appendAddressAndSize( arg_list,
                                  Whole,
                                  buildPointerDerefExp( buildThisExp( csym )), // we want &(*this), sizeof(*this)
                                  type,
                                  NULL
                                );

            appendFileInfo( arg_list, body );

            // create the function call and prepend it to the constructor's body
            ROSE_ASSERT( symbols.roseCreateObject );
            SgExprStatement* fncall =
            buildExprStatement(
                            buildFunctionCallExp(
                                            buildFunctionRefExp( symbols.roseCreateObject ),
                                            arg_list ));
            attachComment( fncall, "", PreprocessingInfo::before );
            attachComment(
                            fncall,
                            "RS: Create Variable, parameters: "
                            "type, basetype, indirection_level, "
                            "address, size, filename, line, linetransformed",
                            PreprocessingInfo::before );

            ROSE_ASSERT( fncall );
            body -> prepend_statement( fncall );
        }
}


/// \brief              adjusts scopes for some variable declaration
/// \param stmt         current statement, might be adjusted
/// \param scope        current statement, might be adjusted
/// \param was_compgen  this output variable is set to true
///                     when stmt could not be adjusted b/c it was compiler generated.
/// \param mainfirst    the first statement in main (typically RtedTransformation::mainfirst)
///
/// \todo
/// \note  I refactored a number of code repetitions into this functions.
///        A was_compgen typically leads into a ROSE_ASSERT, but not always.
///        Possibly, the difference behavior might just reflect clones
///        that have run apart...
static
void adjustStmtAndScopeIfNeeded(SgStatement*& stmt, SgScopeStatement* scope, bool& was_compgen, SgStatement* mainfirst)
{
    // what if there is an array creation within a ClassDefinition
    if (isSgClassDefinition(scope)) {
            // new stmt = the classdef scope
            SgClassDeclaration* decl = isSgClassDeclaration(scope->get_parent());
            ROSE_ASSERT(decl);

            stmt = isSgVariableDeclaration(decl->get_parent());
            if (stmt)
            {
              scope = scope->get_scope();
              // We want to insert the stmt before this classdefinition, if its still in a valid block
              cerr << " ....... Found ClassDefinition Scope. New Scope is : "
                            << scope->class_name() << "  stmt:" << stmt->class_name()
                            << endl;
            }
            else
            {
              was_compgen = true;
              cerr << " Error . stmt is unknown : "
                   << decl->get_parent()->class_name() << endl;

              ROSE_ASSERT( decl->get_file_info()->isCompilerGenerated() );
            }
    }
    // what is there is an array creation in a global scope
    else if (isSgGlobal(scope))
    {
            cerr << "RuntimeInstrumentation :: WARNING - Scope not handled!!! : "
                 << " : " << scope->class_name()
                 << endl;

            // We need to add this new statement to the beginning of main
            // get the first statement in main as stmt
            stmt = mainfirst;
            scope = stmt->get_scope();
    }
}

static
void adjustStmtAndScopeIfNeeded_NoCompilerGen(SgStatement*& stmt, SgScopeStatement* scope, SgStatement* mainfirst)
{
  bool compgen = false;

  adjustStmtAndScopeIfNeeded(stmt, scope, compgen, mainfirst);
  ROSE_ASSERT(!compgen);
}



void RtedTransformation::insertVariableCreateCall(SgInitializedName* initName)
{
        SgStatement* stmt = getSurroundingStatement(initName);
        ROSE_ASSERT(stmt);

        // make sure there is no extern in front of stmt
        if (isGlobalExternVariable(stmt)) {
                cerr
                                << "Skipping this insertVariableCreateCall because it probably occurs multiple times (with and without extern)."
                                << endl;
                return;
        }

        SgScopeStatement* scope = stmt->get_scope();
        ROSE_ASSERT(scope);

        adjustStmtAndScopeIfNeeded_NoCompilerGen(stmt, scope, mainFirst);

        // for( int i =0;
        // ForStmt .. ForInitStmt .. <stmt>
        // upc_forall (int i = 9;
        // ...
        if (isSgForInitStatement(stmt -> get_parent())) {
                // we have to handle for statements separately, because of parsing
                // issues introduced by variable declarations in the for loop's
                // init statement
                SgFunctionCallExp* buildVar = buildVariableCreateCallExpr(initName, stmt);
                ROSE_ASSERT(buildVar != NULL);

                SgStatement* const for_loop = GeneralizdFor::is( stmt -> get_parent() -> get_parent() );
                ROSE_ASSERT(for_loop != NULL);

                prependPseudoForInitializerExpression(buildVar, isSgStatement(for_loop));
        } else if (isSgIfStmt(scope)) {
                SgExprStatement* exprStmt = buildVariableCreateCallStmt(initName,
                                stmt);
                if (exprStmt) {
                        SgStatement* trueb = isSgIfStmt(scope)->get_true_body();
                        SgStatement* falseb = isSgIfStmt(scope)->get_false_body();
                        bool partOfTrue = traverseAllChildrenAndFind(initName, trueb);
                        bool partOfFalse = traverseAllChildrenAndFind(initName, falseb);
                        bool partOfCondition = false;
                        if (partOfTrue == false && partOfFalse == false)
                                partOfCondition = true;
                        if (trueb && (partOfTrue || partOfCondition)) {
                                if (!isSgBasicBlock(trueb)) {
                                        removeStatement(trueb);
                                        SgBasicBlock* bb = buildBasicBlock();
                                        bb->set_parent(isSgIfStmt(scope));
                                        isSgIfStmt(scope)->set_true_body(bb);
                                        bb->prepend_statement(trueb);
                                        trueb = bb;
                                }
                                prependStatement(exprStmt, isSgScopeStatement(trueb));
                        }
                        if (falseb && (partOfFalse || partOfCondition)) {
                                if (!isSgBasicBlock(falseb)) {
                                        removeStatement(falseb);
                                        SgBasicBlock* bb = buildBasicBlock();
                                        bb->set_parent(isSgIfStmt(scope));
                                        isSgIfStmt(scope)->set_false_body(bb);
                                        bb->prepend_statement(falseb);
                                        falseb = bb;
                                }
                                prependStatement(exprStmt, isSgScopeStatement(falseb));
                        } else if (partOfCondition) {
                                // create false statement, this is sometimes needed
                                SgBasicBlock* bb = buildBasicBlock();
                                bb->set_parent(isSgIfStmt(scope));
                                isSgIfStmt(scope)->set_false_body(bb);
                                falseb = bb;
                                prependStatement(exprStmt, isSgScopeStatement(falseb));
                        }
                }

//              } else if (isNormalScope(scope) || !isSgIfStmt(scope)) {
        } else if (isNormalScope(scope)) {
                // insert new stmt (exprStmt) after (old) stmt
                SgExprStatement* exprStmt = buildVariableCreateCallStmt(initName,
                                stmt);

                ROSE_ASSERT(exprStmt);
                //~ cerr << "++++++++++++ stmt :" << stmt << " mainFirst:"
                                //~ << mainFirst << "   initName->get_scope():"
                                //~ << initName->get_scope()
                                //~ << "   mainFirst->get_scope():"
                                //~ << mainFirst->get_scope() << endl;
                // FIXME 2: stmt == mainFirst is probably wrong for cases where the
                // statment we want to instrument really is the first one in main (and not
                // merely one in the global scope)
                if (stmt == mainFirst && initName->get_scope()
                                != mainFirst->get_scope()) {
                        mainBody -> prepend_statement(exprStmt);
                        cerr << "+++++++ insert Before... " << endl;
                } else {
                        // insert new stmt (exprStmt) after (old) stmt
                        insertStatementAfter(stmt, exprStmt);
                        cerr << "+++++++ insert After... " << endl;
                }
        }
        else
        {
                cerr
                                << "RuntimeInstrumentation :: WARNING - unhandled (unexpected) scope!!! : "
                                << initName->get_mangled_name().str() << " : " << scope->class_name() << endl;

                 // crash on unexpected scopes
                 ROSE_ASSERT(isSgNamespaceDefinitionStatement(scope));
        }
}

void RtedTransformation::insertVariableCreateCall(SgInitializedName* initName, SgExpression* varRefExp)
{
        SgStatement* stmt = getSurroundingStatement(initName);
        ROSE_ASSERT(stmt);

        SgScopeStatement* scope = stmt->get_scope();
        string            name = initName->get_mangled_name().str();

        ROSE_ASSERT(isNormalScope(scope));

        // insert new stmt (exprStmt) after (old) stmt
        string             debug_name = initName -> get_name();
        SgFunctionCallExp* fn_call = buildVariableCreateCallExpr(varRefExp, debug_name, true);
        SgExprStatement*   exprStmt = buildVariableCreateCallStmt(fn_call);

        ROSE_ASSERT(exprStmt);
        insertStatementAfter(stmt, exprStmt);
}

// convenience function
SgFunctionCallExp*
RtedTransformation::buildVariableCreateCallExpr(SgInitializedName* initName,
                SgStatement* /*stmt*/, bool forceinit) {

        SgInitializer* initializer = initName->get_initializer();

        // FIXME 2: We don't handle initializer clauses in constructors.
        // E.g.
        // class A {
        //  int x, y;
        //  A : x( 1 ), y( 200 ) { }
        // };
        //
        /* For non objects, any initializer is enough for the variable to be
         * fully initialized, e.g.
         *      int y = 200;    // safe to read y
         * However, all objects will have some initialzer, even if it's nothing more
         * than a constructor initializer.  Which members are initialized is up to
         * the constructor.
         */
        bool initb = (initializer && !(isSgConstructorInitializer(initializer)))
                        || forceinit;

        string debug_name = initName -> get_name();

        return buildVariableCreateCallExpr(genVarRef(initName), debug_name, initb);
}

SgFunctionCallExp*
RtedTransformation::buildVariableCreateCallExpr(SgExpression* var_ref, string& debug_name, bool initb)
{
        ROSE_ASSERT( var_ref );


        // tps: I am not sure yet if this is needed
#if 0
        // if the variable is called "this", then we want to take the
        // right hand side value
        if (debug_name=="this") {
                return NULL;
#if 0
                SgArrowExp* arrowOp = isSgArrowExp(var_ref->get_parent());
                ROSE_ASSERT(arrowOp);
                SgVarRefExp* newVarRef = isSgVarRefExp(arrowOp->get_rhs_operand());
                cerr << " ++++++ This FOUND! ++  Changing this : " << var_ref << "  to : " << newVarRef << endl;
                ROSE_ASSERT(newVarRef);
                var_ref=newVarRef;
#endif
        }
#endif

        // build the function call : runtimeSystem-->createArray(params); ---------------------------
        SgExprListExp* arg_list = buildExprListExp();

        appendExpression(arg_list, ctorTypeDesc(mkTypeInformation(var_ref -> get_type())) );

        SgVarRefExp*      theVarRef = isSgVarRefExp(var_ref);
        SgScopeStatement* scope = NULL;

        if (theVarRef)
        {
            SgInitializedName* initName = theVarRef -> get_symbol() -> get_declaration();
            scope = get_scope(initName);

            appendAddressAndSize(arg_list, Whole, scope, theVarRef, NULL);
        }
        else
        {
            SgStatement* varref_stmt = getSurroundingStatement(var_ref);

            ROSE_ASSERT( varref_stmt );
            appendAddressAndSize(arg_list, Whole, var_ref, var_ref -> get_type(), NULL);
            scope = varref_stmt->get_scope();
        }

        ROSE_ASSERT( scope );

        SgExpression*  callName = buildStringVal(debug_name);
        SgExpression*  callNameExp = buildStringVal(debug_name);
        const bool     var_init = initb && isFileIOVariable(var_ref->get_type());
        SgExpression*  initBool = buildIntVal(var_init ? 1 : 0);

        appendExpression(arg_list, callName);
        appendExpression(arg_list, callNameExp);
        appendExpression(arg_list, initBool);

        appendClassName(arg_list, var_ref -> get_type());
        appendFileInfo(arg_list, scope, var_ref->get_file_info());

        ROSE_ASSERT(symbols.roseCreateVariable);
        SgFunctionRefExp* memRef_r = buildFunctionRefExp(symbols.roseCreateVariable);

        return buildFunctionCallExp(memRef_r, arg_list);
}

// convenience function
SgExprStatement*
RtedTransformation::buildVariableCreateCallStmt(SgInitializedName* initName,
                SgStatement* stmt, bool forceinit) {

        SgFunctionCallExp* fn_call = buildVariableCreateCallExpr(initName, stmt,
                        forceinit);
        if (fn_call == NULL)
                return NULL;
        return buildVariableCreateCallStmt(fn_call);
}

SgExprStatement*
RtedTransformation::buildVariableCreateCallStmt(SgFunctionCallExp* funcCallExp)
{
  SgExprStatement* exprStmt = buildExprStatement(funcCallExp);
  string           comment = "RS : Create Variable, paramaters : (name, mangl_name, type, basetype, address, sizeof, initialized, fileOpen, classname, filename, linenr, linenrTransformed)";

  attachComment(exprStmt, "", PreprocessingInfo::before);
  attachComment(exprStmt, comment, PreprocessingInfo::before);

  return exprStmt;
}

//std::vector<SgExpression*>
SgExpression*
RtedTransformation::buildVariableInitCallExpr( SgInitializedName* initName,
                                               SgExpression* varRefE,
                                               SgStatement* stmt,
                                               AllocKind allockind
                                             )
{
        // with
        //    arr[ ix ] = value;
        // we want the type of (arr[ ix ]), not arr, as that is the type being
        // written
        // tps (09/14/2009): We need to get all dereferences on the way up
        SgExpression*       exp = getExprBelowAssignment(varRefE);
        cerr << " getExprBelowAssignment : " << exp->class_name() << "   "
                        << stmt->unparseToString() << endl;

        if (isSgVarRefExp(exp) && (allockind == akCHeap)) {
                SgType* thetype = initName->get_type();
                cerr << "$$$$$ Found the AssignInitializer : "
                                << isSgVarRefExp(exp)->get_parent() << endl;
                cerr << "$$$$$ Found the InitName : " << initName->unparseToString()
                                << endl;
                cerr << "$$$$$ Found the InitName Type: " << thetype->class_name()
                                << endl;
                //if (isSgPointerType(thetype))
                //      exp_list.push_back(buildPointerDerefExp(exp));
        }

        // build the function call : runtimeSystem-->createArray(params); ---------------------------
        SgExprListExp*    arg_list = buildExprListExp();
        SgScopeStatement* scope = get_scope(initName);

        appendExpression(arg_list, ctorTypeDesc(mkTypeInformation(NULL, exp -> get_type())) );
        appendAddressAndSize(arg_list, Whole, scope, exp, NULL);
        appendClassName(arg_list, exp -> get_type());

        // \pp \todo use allockind in the signature directly
        SgIntVal*         ismallocV = buildIntVal(allockind == akCHeap);

        appendExpression(arg_list, ismallocV);

        // with
        //    int* p;
        // this is a pointer change
        //    p = (int*) malloc(sizeof(int));
        // but this is not
        //    *p = 10;
        int is_pointer_change =0;
#if 1
        SgExpression* lval = getExprBelowAssignment(varRefE);
        if (lval)
          is_pointer_change = isSgExprStatement(stmt) && isSgPointerType(
                                                                         lval-> get_type());
#else
          is_pointer_change = isSgExprStatement(stmt) && isSgPointerType(
                        isSgExprStatement(stmt) -> get_expression() -> get_type());
#endif

        cerr << "@@@@ varrefe = " << initName->get_scope()->class_name() << endl;
        appendExpression(arg_list, buildIntVal(is_pointer_change));

        appendFileInfo(arg_list, stmt);

        //appendExpression(arg_list, buildStringVal(removeSpecialChar(stmt->unparseToString())));

        ROSE_ASSERT(symbols.roseInitVariable);
        string symbolName2 = symbols.roseInitVariable->get_name().str();
        SgFunctionRefExp* memRef_r = buildFunctionRefExp(symbols.roseInitVariable);
        SgExpression* result = buildFunctionCallExp(memRef_r, arg_list);
#if 0
        resultSet.push_back(result);
}
return resultSet;
#endif
        return result;
}

void RtedTransformation::insertInitializeVariable(SgInitializedName* initName, SgExpression* varRefE, AllocKind allocKind)
{
        ROSE_ASSERT(initName && varRefE);

        SgStatement* stmt = NULL;
        if (varRefE->get_parent()) // we created a verRef for AssignInitializers which do not have a parent
                stmt = getSurroundingStatement(varRefE);
        else
                stmt = getSurroundingStatement(initName);

        ROSE_ASSERT(stmt);

        SgScopeStatement* scope = stmt->get_scope();
        bool              compgen = false;

        ROSE_ASSERT(scope);
        adjustStmtAndScopeIfNeeded(stmt, scope, compgen, mainFirst);

        if (compgen) return;

        string name = initName->get_mangled_name().str();
        cerr << "          ... running insertInitializeVariable :  " << name
                        << "   scope: " << scope->class_name() << endl;

        if (isSgForInitStatement(stmt -> get_parent())) {
                // we have to handle for statements separately, because of parsing
                // issues introduced by variable declarations in the for loop's
                // init statement
                SgExpression*      funcCallExp_vec = buildVariableInitCallExpr(initName, varRefE, stmt, allocKind);
                SgStatement* const for_stmt = GeneralizdFor::is(stmt -> get_parent() -> get_parent());

                ROSE_ASSERT(for_stmt != NULL);
                prependPseudoForInitializerExpression(funcCallExp_vec, for_stmt);
        } else if (isSgIfStmt(scope)) {
                SgExpression*      funcCallExp = buildVariableInitCallExpr(initName, varRefE, stmt, allocKind);
                SgExprStatement*   exprStmt = buildExprStatement(funcCallExp);
                cerr << "If Statment : inserting initvar" << endl;
                ROSE_ASSERT(exprStmt);

                SgIfStmt*          ifStmt = isSgIfStmt(scope);
                SgStatement*       trueb = ifStmt->get_true_body();
                SgStatement*       falseb = ifStmt->get_false_body();
                // find out if the varRefE is part of the true, false or
                // the condition
                const bool         partOfTrue = traverseAllChildrenAndFind(varRefE, trueb);
                const bool         partOfFalse = traverseAllChildrenAndFind(varRefE, falseb);
                const bool         partOfCondition = (!partOfTrue && !partOfFalse);

                cerr << "@@@@ If cond : partOfTrue: " << partOfTrue
                                << "   partOfFalse:" << partOfFalse
                                << "  partOfCondition:" << partOfCondition << endl;

                // \pp \todo simplify the branches below by using the
                //           SageInterface::ensureBasicBlockAs* function family.
                if (trueb && (partOfTrue || partOfCondition)) {
                        if (!isSgBasicBlock(trueb)) {
                                removeStatement(trueb);
                                SgBasicBlock* bb = buildBasicBlock();
                                bb->set_parent(ifStmt);
                                ifStmt->set_true_body(bb);
                                bb->prepend_statement(trueb);
                                trueb = bb;
                        }
                        prependStatement(exprStmt, isSgScopeStatement(trueb));
                }
                if (falseb && (partOfFalse || partOfCondition)) {
                        if (!isSgBasicBlock(falseb)) {
                                removeStatement(falseb);
                                SgBasicBlock* bb = buildBasicBlock();
                                bb->set_parent(ifStmt);
                                ifStmt->set_false_body(bb);
                                bb->prepend_statement(falseb);
                                falseb = bb;
                        }
                        prependStatement(exprStmt, isSgScopeStatement(falseb));
                } else if (partOfCondition) {
                        // create false statement, this is sometimes needed
                        SgBasicBlock* bb = buildBasicBlock();
                        bb->set_parent(ifStmt);
                        ifStmt->set_false_body(bb);
                        falseb = bb;
                        prependStatement(exprStmt, isSgScopeStatement(falseb));
                }
        }
        else if (isNormalScope(scope))
        {
                SgExpression*    funcCallExp = buildVariableInitCallExpr(initName, varRefE, stmt, allocKind);
                SgExprStatement* exprStmt = buildExprStatement(funcCallExp);
                string           empty_comment;
                string           comment = "RS : Init Variable, paramaters : (tpye, basetype, class_name, address, size, ismalloc, is_pointer_change, filename, line, linenrTransformed, error line)";

                attachComment(exprStmt, empty_comment, PreprocessingInfo::before);
                attachComment(exprStmt, comment, PreprocessingInfo::before);

                // insert new stmt (exprStmt) before (old) stmt
                insertStatementAfter(stmt, exprStmt);
        } // basic block
        else
        {
                cerr
                                << " -----------> RuntimeInstrumentation :: unhandled (unexpected) block! : "
                                << name << " : " << scope->class_name() << "  - "
                                << stmt->unparseToString() << endl;

                // crash for unexpected scopes
                ROSE_ASSERT(isSgNamespaceDefinitionStatement(scope));
        }
}

void RtedTransformation::insertCheckIfThisNull(SgThisExp* texp)
{
        cerr << "access to this : " << texp->unparseToString() << endl;

        SgStatement*      stmt = getSurroundingStatement(texp);
        ROSE_ASSERT(stmt);

        SgScopeStatement* scope = stmt->get_scope();
        ROSE_ASSERT(scope);

        adjustStmtAndScopeIfNeeded_NoCompilerGen(stmt, scope, mainFirst);

        if (isNormalScope(scope)) {
                // build the function call : runtimeSystem-->createArray(params); ---------------------------
                SgExprListExp* arg_list = buildExprListExp();

                appendExpression(arg_list, buildThisExp(texp->get_class_symbol()));
                appendFileInfo(arg_list, stmt);

                ROSE_ASSERT(symbols.roseCheckIfThisNULL);
                checkBeforeStmt( stmt,
                                 symbols.roseCheckIfThisNULL,
                                 arg_list,
                                 "RS : roseCheckIfThisNULL, parameters : (ThisExp, filename, line, line transformed, error Str)"
                               );

        } // basic block
        else
        {
                cerr
                                << " -----------> RuntimeInstrumentation :: unhandled (unexpected) block! : "
                                << " : " << scope->class_name() << "  - "
                                << stmt->unparseToString() << endl;
                ROSE_ASSERT(isSgNamespaceDefinitionStatement(scope));
        }
}

void RtedTransformation::insertAccessVariable(SgThisExp* varRefE, SgExpression* derefExp)
{
        SgStatement* stmt = getSurroundingStatement(varRefE);
        SgClassDeclaration* decl = varRefE->get_class_symbol()->get_declaration();
        ROSE_ASSERT(decl);
        SgScopeStatement* scope = decl->get_scope();
        insertAccessVariable(scope, derefExp, stmt, varRefE);
}

void RtedTransformation::insertAccessVariable(SgVarRefExp* varRefE, SgExpression* derefExp)
{
        SgStatement* stmt = getSurroundingStatement(varRefE);
        // make sure there is no extern in front of stmt
        SgInitializedName* initName = varRefE->get_symbol()->get_declaration();
        SgScopeStatement* initNamescope = initName->get_scope();

        SgDotExp* parent_dot = isSgDotExp(varRefE -> get_parent());
        if (parent_dot && parent_dot -> get_lhs_operand() == varRefE) {
                //      x = s.y
                // does not need a var ref to y, only to s
                return;
        }
        insertAccessVariable(initNamescope, derefExp, stmt, varRefE);
}

void RtedTransformation::insertAccessVariable( SgScopeStatement* initscope,
                                               SgExpression* derefExp,
                                               SgStatement* stmt,
                                               SgExpression* varRefE
                                             )
{
    ROSE_ASSERT(stmt && varRefE);

    if (stmt->get_file_info()->isCompilerGenerated())
      return;

    if (isSgVarRefExp(varRefE) && !isInInstrumentedFile(
                    isSgVarRefExp(varRefE)->get_symbol()->get_declaration()))
       return;

    if (isFileIOVariable(varRefE->get_type()))
      return;

    SgScopeStatement* scope = stmt->get_scope();
    ROSE_ASSERT(scope);

    cerr << "          ... running insertAccessVariable :  " //<< name
                    << "   scope: " << scope->class_name() << endl;

    adjustStmtAndScopeIfNeeded_NoCompilerGen(stmt, scope, mainFirst);

    if (isNormalScope(scope))
    {
            // build the function call : runtimeSystem-->createArray(params); ---------------------------

            int read_write_mask = Read;
            SgExpression* accessed_exp = varRefE;
            SgExpression* write_location_exp = varRefE;
            if (derefExp) {
                    SgPointerDerefExp* deref_op = isSgPointerDerefExp(derefExp);
                    SgArrowExp* arrow_op = isSgArrowExp(derefExp);
                    ROSE_ASSERT( deref_op || arrow_op );

                    if (arrow_op) {
                            // with
                            //    p -> b = 2
                            // we need to be able to read
                            //    *p
                            if (isUsedAsLvalue(arrow_op)) {
                                    bool isReadOnly =
                                                    isthereAnotherDerefOpBetweenCurrentAndAssign(
                                                                    derefExp);
                                    if (isSgThisExp(arrow_op->get_lhs_operand())) {
                                            return;

                                    } else {
                                            accessed_exp = arrow_op->get_lhs_operand();
                                            if (!isReadOnly)
                                                    read_write_mask |= Write;
                                    }
                                    write_location_exp = arrow_op;
                            } else {
                                    // not a l-value
                                    write_location_exp = 0;
                                    if (isSgMemberFunctionType(
                                                    arrow_op -> get_rhs_operand() -> get_type()))
                                            // for member function invocations, we just want to
                                            // check that the pointer is good
                                            accessed_exp = arrow_op -> get_lhs_operand();
                                    else
                                            // normally we'll be reading the member itself
                                            accessed_exp = arrow_op;
                            }
                    } else {
                            // consider
                            //    int *p;
                            //    *p = 24601;
                            //  It is necessary that &p, sizeof(p) is readable, but not
                            //  &(*p), sizeof(*p).
                            if (isUsedAsLvalue(derefExp)) {
                                    bool isReadOnly =
                                                    isthereAnotherDerefOpBetweenCurrentAndAssign(
                                                                    derefExp);
                                    accessed_exp = deref_op -> get_operand();
                                    write_location_exp = deref_op;
                                    if (!isReadOnly)
                                            read_write_mask |= Write;
                            } else {
                                    accessed_exp = deref_op;
                                    write_location_exp = 0;
                            }
                    }
            }

            SgExprListExp* arg_list = buildExprListExp();

            appendAddressAndSize(arg_list, Elem, initscope, accessed_exp, NULL);
            appendAddressAndSize(arg_list, Elem, initscope, write_location_exp, NULL);
            appendExpression(arg_list, buildIntVal(read_write_mask));
            appendFileInfo(arg_list, stmt);

            // appendExpression(arg_list, buildStringVal(removeSpecialChar(stmt->unparseToString())));

            ROSE_ASSERT(symbols.roseAccessVariable);
            checkBeforeStmt( stmt,
                             symbols.roseAccessVariable,
                             arg_list,
                             "RS : Access Variable, paramaters : (address_r, sizeof(type)_r, address_w, sizeof(type)_w, r/w, filename, line, line transformed, error Str)"
                           );
    } // basic block
    else
    {
            cerr
                            << " -----------> RuntimeInstrumentation :: unexpected (unhandled) scope! : "
            //<< name
                            << " : " << scope->class_name() << "  - "
                            << stmt->unparseToString() << endl;
            ROSE_ASSERT(isSgNamespaceDefinitionStatement(scope));
    }
}


struct AllocInfo
{
  SgType*   newtype;
  AllocKind allocKind;

  AllocInfo()
  : newtype(NULL), allocKind(akStack)
  {}

  void handle(SgNode&) { ROSE_ASSERT(false); }

  void handle(SgNewExp& n)
  {
    newtype = n.get_type();
    allocKind = ( isSgArrayType( skip_ModifierType(newtype) ) ? akCxxArrayNew
                                                              : akCxxNew
                );
  }

  void handle(SgExpression& n) {}
};


void RtedTransformation::visit_isAssignInitializer(SgNode* n) {
        SgAssignInitializer* assign = isSgAssignInitializer(n);
        ROSE_ASSERT(assign);
        cerr << "\n\n???????????? Found assign init op : " << n->unparseToString()
                        << endl;
        SgInitializedName* initName = NULL;
        SgNode* ancestor = n;
        while (initName == NULL && ancestor != NULL) {
                initName = isSgInitializedName(ancestor);
                ancestor = ancestor -> get_parent();
        }
        ROSE_ASSERT(initName);

        // ---------------------------------------------
        // we now know that this variable must be initialized
        // if we have not set this variable to be initialized yet,
        // we do so
        cerr << ">>>>>>> Setting this var to be assign initialized : "
                        << initName->unparseToString() << "  and assignInit: "
                        << assign->unparseToString() << endl;
        SgStatement* stmt = getSurroundingStatement(assign);
        ROSE_ASSERT(stmt);
        SgScopeStatement* scope = stmt->get_scope();
        ROSE_ASSERT(scope);
        //      SgType* type = initName->get_type();

        // dont do this if the variable is global or an enum constant
        if (isSgGlobal(initName->get_scope()) || isSgEnumDeclaration(initName->get_parent())) {
        } else {
                SgVarRefExp* const varRef = buildVarRefExp(initName, scope);

                // \pp \todo do we need the following line?
                varRef->get_file_info()->unsetOutputInCodeGeneration();

                const AllocInfo    allocInfo = ez::visitSgNode(AllocInfo(), assign->get_operand());

                cerr << "Adding variable init : " << varRef->unparseToString() << endl;
                variableIsInitialized[varRef] = InitializedVarMap::mapped_type(initName, allocInfo.allocKind);

                // tps (09/15/2009): The following code handles AssignInitializers for SgNewExp
                // e.g. int *p = new int;
#if 1
                // \pp \todo the following block could be pushed into AllocInfo
                if (allocInfo.newtype) {
                        // TODO 2: This handles new assign initializers, but not malloc assign
                        //          initializers.  Consider, e.g:
                        //
                        //          int* x = (int*) malloc( sizeof( int ));
                        SgExpression* sizeExp = buildSizeOfOp(allocInfo.newtype);
                        RtedArray*    array = new RtedArray( initName,
                                                             getSurroundingStatement(initName),
                                                             allocInfo.allocKind,
                                                             sizeExp
                                                           );

                        variablesUsedForArray.push_back(varRef);
                        create_array_define_varRef_multiArray[varRef] = array;

                        cerr << ">> Setting this var to be initialized : "
                             << initName->unparseToString() << endl;

                        // \pp \todo I believe that the following assignment
                        //           can be safely removed as it duplicates
                        //           the previous store to
                        //           variableIsInitialized ...
                        variableIsInitialized[varRef] = InitializedVarMap::mapped_type(initName, allocInfo.allocKind);
                }
#endif

        }

        // ---------------------------------------------
}


//
// Functions added to treat UPC-forall and C/C++ for loops
//   somewhat uniformly
//

namespace GeneralizdFor
{
  SgStatement* is(SgNode* astNode)
  {
    SgStatement* res = NULL;

    switch (astNode->variantT())
    {
      case V_SgForStatement:
      case V_SgUpcForAllStatement:
        res = isSgStatement(astNode);
        break;
      default: ;
    }

    return res;
  }

  SgForInitStatement* initializer(SgStatement* forloop)
  {
    SgForInitStatement* res = NULL;

    switch (forloop->variantT())
    {
      case V_SgForStatement:
        res = isSgForStatement(forloop)->get_for_init_stmt();
        break;
      case V_SgUpcForAllStatement:
        res = isSgUpcForAllStatement(forloop)->get_for_init_stmt();
        break;
      default: ROSE_ASSERT(false);
    }

    ROSE_ASSERT(res != NULL);
    return res;
  }

  SgStatement* test(SgStatement* forloop)
  {
    SgStatement* res = NULL;

    switch (forloop->variantT())
    {
      case V_SgForStatement:
        res = isSgForStatement(forloop)->get_test();
        break;
      case V_SgUpcForAllStatement:
        res = isSgUpcForAllStatement(forloop)->get_test();
        break;
      default: ROSE_ASSERT(false);
    }

    ROSE_ASSERT(res != NULL);
    return res;
  }
}

#endif
