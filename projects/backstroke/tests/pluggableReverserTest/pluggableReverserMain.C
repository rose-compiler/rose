#include <pluggableReverser/eventProcessor.h>
#include <pluggableReverser/expressionProcessor.h>
#include <pluggableReverser/statementProcessor.h>
#include <utilities/Utilities.h>
#include <normalizations/expNormalization.h>
#include <boost/algorithm/string.hpp>
#include <string>

#include "utilities/CPPDefinesAndNamespaces.h"


#include <VariableRenaming.h>

using namespace SageInterface;
using namespace SageBuilder;
using namespace backstroke_util;

int fixVariableReferences2(SgNode* root)
{
    ROSE_ASSERT(root);
    int counter=0;
    Rose_STL_Container<SgNode*> nodeList;

    SgVarRefExp* varRef=NULL;
    Rose_STL_Container<SgNode*> reflist = NodeQuery::querySubTree(root, V_SgVarRefExp);
    for (Rose_STL_Container<SgNode*>::iterator i=reflist.begin();i!=reflist.end();i++)
    {
        //cout << get_name(isSgVarRefExp(*i)) << endl;
        varRef= isSgVarRefExp(*i);
        ROSE_ASSERT(varRef->get_symbol());
        SgInitializedName* initname= varRef->get_symbol()->get_declaration();
        //ROSE_ASSERT(initname);

        if (initname->get_type()==SgTypeUnknown::createType())
            //    if ((initname->get_scope()==NULL) && (initname->get_type()==SgTypeUnknown::createType()))
        {
            SgName varName=initname->get_name();
            SgSymbol* realSymbol = NULL;
            cout << varName << endl;

            // CH (5/7/2010): Before searching SgVarRefExp objects, we should first deal with class/structure
            // members. Or else, it is possible that we assign the wrong symbol to those members if there is another
            // variable with the same name in parent scopes. Those members include normal member referenced using . or ->
            // operators, and static members using :: operators.
            //
            if (SgArrowExp* arrowExp = isSgArrowExp(varRef->get_parent()))
            {
                if (varRef == arrowExp->get_rhs_operand())
                {
                    // make sure the lhs operand has been fixed
                    counter += fixVariableReferences2(arrowExp->get_lhs_operand());

                    SgPointerType* ptrType = isSgPointerType(arrowExp->get_lhs_operand()->get_type());
                    ROSE_ASSERT(ptrType);
                    SgClassType* clsType = isSgClassType(ptrType->get_base_type());
                    ROSE_ASSERT(clsType);
                    SgClassDeclaration* decl = isSgClassDeclaration(clsType->get_declaration());
                    decl = isSgClassDeclaration(decl->get_definingDeclaration());
                    ROSE_ASSERT(decl);
                    realSymbol = lookupSymbolInParentScopes(varName, decl->get_definition());
                }
                else
                    realSymbol = lookupSymbolInParentScopes(varName,getScope(varRef));
            }
            else if (SgDotExp* dotExp = isSgDotExp(varRef->get_parent()))
            {
                if (varRef == dotExp->get_rhs_operand())
                {
                    // make sure the lhs operand has been fixed
                    counter += fixVariableReferences2(dotExp->get_lhs_operand());

                    SgClassType* clsType = isSgClassType(dotExp->get_lhs_operand()->get_type());
                    ROSE_ASSERT(clsType);
                    SgClassDeclaration* decl = isSgClassDeclaration(clsType->get_declaration());
                    decl = isSgClassDeclaration(decl->get_definingDeclaration());
                    ROSE_ASSERT(decl);
                    realSymbol = lookupSymbolInParentScopes(varName, decl->get_definition());
                }
                else
                    realSymbol = lookupSymbolInParentScopes(varName,getScope(varRef));
            }
            else
                realSymbol = lookupSymbolInParentScopes(varName,getScope(varRef));

            // should find a real symbol at this final fixing stage!
            // This function can be called any time, not just final fixing stage
            if (realSymbol==NULL) 
            {
                cerr<<"Error: cannot find a symbol for "<<varName.getString()<<endl;
                //ROSE_ASSERT(realSymbol);
            }
            else {
                // release placeholder initname and symbol
                ROSE_ASSERT(realSymbol!=(varRef->get_symbol()));

                bool flag = false;

                SgSymbol* symbol_to_delete = varRef->get_symbol();
                varRef->set_symbol(isSgVariableSymbol(realSymbol));
                counter ++;

                if (nodeList.empty())
                {
                    VariantVector vv(V_SgVarRefExp);
                    nodeList = NodeQuery::queryMemoryPool(vv);
                }
                foreach(SgNode* node, nodeList)
                {
                    if (SgVarRefExp* var = isSgVarRefExp(node))
                    {
                        if (var->get_symbol() == symbol_to_delete)
                        {
                            flag = true;
                            break;
                        }
                    }
                }
                if (!flag)
                {
                    //cout <<initname->get_name().str() << endl;
                    delete initname; // TODO deleteTree(), release File_Info nodes etc.
                    delete symbol_to_delete;
                }

                cout << "@" << varRef->get_symbol()->get_declaration()->get_prev_decl_item() << endl;
            }
        }
    } // end for
    return counter;
}


int main(int argc, char * argv[])
{
    vector<string> args(argv, argv+argc);
    SgProject* project = frontend(args);


    VariableRenaming var_renaming(project);
    var_renaming.run();
    //var_renaming.toDOT("temp.dot");


    SgGlobal* global = getFirstGlobalScope(project);

    // Prepend includes to test files.
    string includes = "#include <rctypes.h>\n"
        "#include <stdio.h>\n"
        "#include <stdlib.h>\n"
        "#include <time.h>\n"
        "#include <assert.h>\n"
        "#include <memory.h>\n";
    addTextForUnparser(global, includes, AstUnparseAttribute::e_before);


    EventProcessor event_processor(NULL, &var_renaming);

    // Add all expression handlers to the expression pool.
    event_processor.addExpressionProcessor(new NullExpressionProcessor);
    event_processor.addExpressionProcessor(new StoreAndRestoreExpressionProcessor);
    event_processor.addExpressionProcessor(new ConstructiveExpressionProcessor);
    event_processor.addExpressionProcessor(new ConstructiveAssignmentProcessor);

    // Add all statement handlers to the statement pool.
    event_processor.addStatementProcessor(new BasicStatementProcessor);

    pushScopeStack(isSgScopeStatement(global));

    // Get every function declaration and identify if it's an event function.
    vector<SgFunctionDeclaration*> func_decls = backstroke_util::querySubTree<SgFunctionDeclaration > (global);

    foreach(SgFunctionDeclaration* decl, func_decls)
    {
        string func_name = decl->get_name();
        if (!starts_with(func_name, "event") ||
                ends_with(func_name, "reverse") ||
                ends_with(func_name, "forward"))
            continue;


        // First of all, normalize this event function.
        backstroke_norm::normalizeEvent(decl);

        var_renaming.run();

        /*******************************************************/
        // A small test here :)
       // VariableRenaming var_renaming(project);
        //var_renaming.run();
#if 0
        VariableVersionTable var_table(decl, &var_renaming);
        cout << "!!!\n";
        var_table.print();
#endif

#if 1
        // Here reverse the event function into several versions.
        FuncDeclPairs output = event_processor.processEvent(decl);
        foreach (FuncDeclPair func_decl_pair, output)
        {
           appendStatement(func_decl_pair.first); 
           appendStatement(func_decl_pair.second); 
        }
#endif
    }

    // Declare all stack variables on top of the generated file.
    vector<SgVariableDeclaration*> stack_decls = event_processor.getAllStackDeclarations();
    foreach (SgVariableDeclaration* decl, stack_decls)
        prependStatement(decl);

    popScopeStack();

    fixVariableReferences2(global);
    fixVariableReferences(global);

    return backend(project);
}
