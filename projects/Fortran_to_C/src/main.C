/*
  Author: Pei-Hung Lin
  Contact: lin32@llnl.gov, phlin@cs.umn.edu

  Date Created       : July 5th, 2012
 
 This is a source to source compiler for Fortran

  Fortran to C translator 
  * input  : Fortran 77 source code
  * output : C source code

*/  

#include "rose.h"
#include "f2c.h"
#include "CommandOptions.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace Fortran_to_C;

bool isLinearlizeArray = false;
vector<SgArrayType*> arrayTypeList;
vector<SgVarRefExp*> parameterRefList;
vector<SgForStatement*> forStmtList;
vector<SgVariableDeclaration*> variableDeclList;
vector<SgPntrArrRefExp*> pntrArrRefList;
vector<SgEquivalenceStatement*> equivalenceList;
map<SgVariableSymbol*,SgExpression*> parameterSymbolList;
vector<SgStatement*> statementList;
vector<SgNode*> removeList;
stack<SgStatement*> insertList;

// memory pool traversal for variable declaration
class variableDeclTraversal : public ROSE_VisitorPattern
{
  public:
    void visit(SgVariableDeclaration* varDecl);
};

void variableDeclTraversal::visit(SgVariableDeclaration* varDecl)
{
  variableDeclList.push_back(varDecl);
}

// memory pool traversal for pointer Array Reference 
class pntrArrRefTraversal : public ROSE_VisitorPattern
{
  public:
    void visit(SgPntrArrRefExp* pntrArrRefExp);
};

void pntrArrRefTraversal::visit(SgPntrArrRefExp* pntrArrRefExp)
{
  pntrArrRefList.push_back(pntrArrRefExp);
}

// memory pool traversal for equivalence 
class equivalencelTraversal : public ROSE_VisitorPattern
{
  public:
    void visit(SgEquivalenceStatement* equivalence);
};

void equivalencelTraversal::visit(SgEquivalenceStatement* equivalence)
{
  equivalenceList.push_back(equivalence);
}

// memory pool traversal for arrayType 
class arrayTypeTraversal : public ROSE_VisitorPattern
{
  public:
    void visit(SgArrayType* type);
};

void arrayTypeTraversal::visit(SgArrayType* type)
{
  arrayTypeList.push_back(type);
}

// memory pool traversal for parameter 
class parameterTraversal : public ROSE_VisitorPattern
{
  public:
    void visit(SgVarRefExp* parameterRef);
};

void parameterTraversal::visit(SgVarRefExp* parameterRef)
{
  parameterRefList.push_back(parameterRef);
}

// memory pool traversal for forStatement 
class forloopTraversal : public ROSE_VisitorPattern
{
  public:
    void visit(SgForStatement* forStmt);
};

void forloopTraversal::visit(SgForStatement* forStmt)
{
  forStmtList.push_back(forStmt);
}

// simple traversal for general translation
class f2cTraversal : public AstSimpleProcessing
{
  public:
    virtual void visit(SgNode* n);
};

void f2cTraversal::visit(SgNode* n)
{
  Sg_File_Info* fileInfo = n->get_file_info();
  fileInfo->set_isPartOfTransformation(true);

//  fileInfo->set_physical_filename(fileInfo->get_filenameString());
//  cout << n << " sets physical filename to " << fileInfo->get_filenameString() << endl;
  /*
    1. The following switch statement search for the Fortran-specific
       AST nodes and transform them into C nodes. 
    2. The new C nodes are created first.  Attributes and details
       are then copied from original Fortran nodes.  After the 
       copy, original Fortran nodes are deleted. 
  */
  switch(n->variantT())
  {
    case V_SgSourceFile:
      {
        SgFile* fileNode = isSgFile(n);
        translateFileName(fileNode);
        break;
      }
    case V_SgProgramHeaderStatement:
      {
        SgProgramHeaderStatement* ProgramHeaderStatement = isSgProgramHeaderStatement(n);
        ROSE_ASSERT(ProgramHeaderStatement);
        translateProgramHeaderStatement(ProgramHeaderStatement);
        // Deep delete the original Fortran SgProgramHeaderStatement
        removeList.push_back(ProgramHeaderStatement);
        break;
      }
    case V_SgProcedureHeaderStatement:
      {
        SgProcedureHeaderStatement* procedureHeaderStatement = isSgProcedureHeaderStatement(n);
        ROSE_ASSERT(procedureHeaderStatement);
        translateProcedureHeaderStatement(procedureHeaderStatement);
        // Deep delete the original Fortran procedureHeaderStatement.
        removeList.push_back(procedureHeaderStatement);
        break;
      }
    case V_SgFortranDo:
      {
        SgFortranDo* fortranDo = isSgFortranDo(n);
        ROSE_ASSERT(fortranDo);
        translateFortranDoLoop(fortranDo);
        // Deep delete the original fortranDo .
        removeList.push_back(fortranDo);
        break;
      }
    case V_SgFunctionCallExp:
      {
        SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(n);
        ROSE_ASSERT(functionCallExp);
        translateImplicitFunctionCallExp(functionCallExp);
        break;
      }
    case V_SgExponentiationOp:
      {
        SgExponentiationOp* expOp = isSgExponentiationOp(n);
        ROSE_ASSERT(expOp);
        translateExponentiationOp(expOp);
        break;
      }
    case V_SgFloatVal:
      {
        SgFloatVal* floatVal = isSgFloatVal(n);
        ROSE_ASSERT(floatVal);
        translateDoubleVal(floatVal);
        break;
      }
    case V_SgCommonBlock:
      {
        SgCommonBlock* commonBlock = isSgCommonBlock(n);
        ROSE_ASSERT(commonBlock);
        //translateCommonBlock(commonBlock);
        break;
      }
    case V_SgGlobal:
      {
        SgGlobal* global = isSgGlobal(n);
        ROSE_ASSERT(global);
        removeFortranMaxMinFunction(global);
        PreprocessingInfo* defMaxInfo = new PreprocessingInfo(PreprocessingInfo::CpreprocessorDefineDeclaration,
                                                              "#define max(a,b) (((a)>(b))?(a):(b))", 
                                                              "Transformation generated",
                                                              0, 0, 0, PreprocessingInfo::before);
        defMaxInfo->set_file_info(global->get_file_info());
        global->addToAttachedPreprocessingInfo(defMaxInfo,PreprocessingInfo::before);
        PreprocessingInfo* defMinInfo = new PreprocessingInfo(PreprocessingInfo::CpreprocessorDefineDeclaration,
                                                              "#define min(a,b) (((a)<(b))?(a):(b))", 
                                                              "Transformation generated",
                                                              0, 0, 0, PreprocessingInfo::before);
        defMinInfo->set_file_info(global->get_file_info());
        global->addToAttachedPreprocessingInfo(defMinInfo,PreprocessingInfo::before);
        break;
      }
    case V_SgExprStatement:
      {
        SgExprStatement* exprStmt = isSgExprStatement(n);
        SgFunctionCallExp* funcCallExp = isSgFunctionCallExp(exprStmt->get_expression());
        while(insertList.size() > 0 && funcCallExp)
        {
           insertStatement(exprStmt,deepCopy(insertList.top()),true);
           insertList.pop();
        }
        break;
      }
    default:
      break;
  }
}

int main( int argc, char * argv[] )
{
// Option to linearize the array.
  Rose_STL_Container<std::string> localCopy_argv = CommandlineProcessing::generateArgListFromArgcArgv(argc, argv);
  int newArgc;
  char** newArgv = NULL;
  vector<string> argList = localCopy_argv;
  if (CommandlineProcessing::isOption(argList,"-f2c:","linearize",true) == true)
  {
    isLinearlizeArray = true;
  }
  CommandlineProcessing::generateArgcArgvFromList(argList,newArgc, newArgv);
// Build the AST used by ROSE
  SgProject* project = frontend(newArgc,newArgv);
  AstTests::runAllTests(project);   

// process comment
  vector<SgLocatedNode*> LocatedNodeList = SageInterface::querySubTree<SgLocatedNode> (project,V_SgLocatedNode);
  for (vector<SgLocatedNode*>::iterator i = LocatedNodeList.begin(); i != LocatedNodeList.end(); i++)
  {
    AttachedPreprocessingInfoType* comments = (*i)->getAttachedPreprocessingInfo();
    AttachedPreprocessingInfoType newComment;
          if (comments != NULL)
             {
               AttachedPreprocessingInfoType::iterator j;
               for (j = comments->begin(); j != comments->end(); j++)
                  {
                    if((*j)->getTypeOfDirective() == PreprocessingInfo::FortranStyleComment)
                    {
                        PreprocessingInfo* cmt = new PreprocessingInfo(PreprocessingInfo::C_StyleComment,"/* "+(*j)->getString()+" */", "transformation-generated", 0, 0, 0, (*j)->getRelativePosition());
                        newComment.push_back(cmt);
                    }
                  }
               comments->clear();
               for (j = newComment.begin(); j != newComment.end(); j++)
                  {
     		    (*i)->addToAttachedPreprocessingInfo(*j);
                  }
             }
  }
  // Traversal with Memory Pool to search for variableDeclaration
  variableDeclTraversal translateVariableDeclaration;
  traverseMemoryPoolVisitorPattern(translateVariableDeclaration);
  for(vector<SgVariableDeclaration*>::iterator dec=variableDeclList.begin(); dec!=variableDeclList.end(); ++dec)
  {
    /*
       For the Fortran AST, a single variableDeclaration can be shared by multiple variables.
       This violated the normalization rules for C unparser.  Therefore, we have to transform it.
    */
    SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(*dec);
    ROSE_ASSERT(variableDeclaration);
    SgFunctionDeclaration* funcDecl = getEnclosingFunctionDeclaration(variableDeclaration,false);
    ROSE_ASSERT(funcDecl);
    SgFunctionParameterList* funcParamList = funcDecl->get_parameterList();
    if((variableDeclaration->get_variables()).size() != 1 && isFuncArg(funcParamList, variableDeclaration) == false)
    {
      updateVariableDeclarationList(variableDeclaration);
      statementList.push_back(variableDeclaration);
      removeList.push_back(variableDeclaration);
    }
  }

  // reset the vector that collects all variable declaration. We need to walk through memory pool again to find types
  
  variableDeclList.clear();
  traverseMemoryPoolVisitorPattern(translateVariableDeclaration);
  for(vector<SgVariableDeclaration*>::iterator dec=variableDeclList.begin(); dec!=variableDeclList.end(); ++dec)
  {
    SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(*dec);
    ROSE_ASSERT(variableDeclaration);
    SgInitializedNamePtrList initializedNameList = variableDeclaration->get_variables();
    for(SgInitializedNamePtrList::iterator i=initializedNameList.begin(); i!=initializedNameList.end();++i)
    {
      SgInitializedName* initializedName = isSgInitializedName(*i);
      SgType* baseType = initializedName->get_type();
      if(baseType->variantT() == V_SgArrayType)
      {
        SgArrayType* arrayBase = isSgArrayType(baseType);
        // At this moment, we are still working on the Fortran-stype AST.  Therefore, there is no nested types for multi-dim array.
        if(arrayBase->findBaseType()->variantT() == V_SgTypeString)
        {
          //arrayBase->reset_base_type(translateType(arrayBase->findBaseType()));
          //arrayBase->set_rank(arrayBase->get_rank()+1);
          SgExpression* stringLength = deepCopy(isSgTypeString(arrayBase->findBaseType())->get_lengthExpression());
          arrayBase->get_dim_info()->prepend_expression(stringLength);
        }
        else
          arrayBase->set_base_type(translateType(arrayBase->findBaseType()));
      }
      else
      {
//cout << initializedName->get_name() << endl;
        initializedName->set_type(translateType(baseType));
      }
    }
  }

  // replace the AttributeSpecificationStatement 
  Rose_STL_Container<SgNode*> AttributeSpecificationStatement = NodeQuery::querySubTree (project,V_SgAttributeSpecificationStatement);
  for (Rose_STL_Container<SgNode*>::iterator i = AttributeSpecificationStatement.begin(); i != AttributeSpecificationStatement.end(); i++)
  {
    SgAttributeSpecificationStatement* attributeSpecificationStatement = isSgAttributeSpecificationStatement(*i);
    ROSE_ASSERT(attributeSpecificationStatement);
    translateAttributeSpecificationStatement(attributeSpecificationStatement);
    statementList.push_back(attributeSpecificationStatement);
    removeList.push_back(attributeSpecificationStatement);
  }

  // replace the parameter reference
  parameterTraversal translateParameterRef;
  traverseMemoryPoolVisitorPattern(translateParameterRef);
  for(vector<SgVarRefExp*>::iterator i=parameterRefList.begin(); i!=parameterRefList.end(); ++i)
  {
    SgVarRefExp* parameterRef = isSgVarRefExp(*i);
    if(parameterSymbolList.find(parameterRef->get_symbol()) != parameterSymbolList.end())
    {
      SgExpression* newExpr = isSgExpression(deepCopy(parameterSymbolList.find(parameterRef->get_symbol())->second));
      ROSE_ASSERT(newExpr);
      newExpr->set_parent(parameterRef->get_parent());
      replaceExpression(parameterRef,
                        newExpr,
                        false);
    }
  }

  /*
     Parameters will be replaced by #define, all the declarations should be removed
  */
  for(map<SgVariableSymbol*,SgExpression*>::iterator i=parameterSymbolList.begin();i!=parameterSymbolList.end();++i)
  {
    SgVariableSymbol* symbol = i->first;
    SgInitializedName* initializedName = symbol->get_declaration();
    SgVariableDeclaration* decl = isSgVariableDeclaration(initializedName->get_parent());
    statementList.push_back(decl);
    removeList.push_back(decl);
  }


  // Traversal with Memory Pool to search for arrayType
  arrayTypeTraversal translateArrayType;
  traverseMemoryPoolVisitorPattern(translateArrayType);
  for(vector<SgArrayType*>::iterator i=arrayTypeList.begin(); i!=arrayTypeList.end(); ++i)
  {
    if(isLinearlizeArray)
    {
      linearizeArrayDeclaration(*i);
    }
    else
    {
      translateArrayDeclaration(*i);
    }
  }

  // Traversal with Memory Pool to search for pntrArrRefExp
  pntrArrRefTraversal translatePntrArrRefExp;
  traverseMemoryPoolVisitorPattern(translatePntrArrRefExp);
  for(vector<SgPntrArrRefExp*>::iterator i=pntrArrRefList.begin(); i!=pntrArrRefList.end(); ++i)
  {
    if(isLinearlizeArray)
    {
      linearizeArraySubscript(*i);
    }
    else
    {
      translateArraySubscript(*i);
    }
  }


  Rose_STL_Container<SgNode*> functionList = NodeQuery::querySubTree (project,V_SgFunctionDeclaration);
  for (Rose_STL_Container<SgNode*>::iterator i = functionList.begin(); i != functionList.end(); i++)
  {
    if((isSgProcedureHeaderStatement(*i) != NULL) ||
       (isSgProgramHeaderStatement(*i) != NULL)){
      SgFunctionDeclaration* functionBody = isSgFunctionDeclaration(*i);
      bool hasReturnVal = false;
      if(isSgProcedureHeaderStatement(functionBody))
      {
        hasReturnVal = isSgProcedureHeaderStatement(functionBody)->isFunction();
      }
      fixFortranSymbolTable(functionBody->get_definition(),hasReturnVal);
    }
  } 

  // Traversal with Memory Pool to search for equivalenceStatement
  equivalencelTraversal translateEquivalenceStmt;
  traverseMemoryPoolVisitorPattern(translateEquivalenceStmt);
  for(vector<SgEquivalenceStatement*>::iterator i=equivalenceList.begin(); i!=equivalenceList.end(); ++i)
  {
    SgEquivalenceStatement* equivalenceStatement = isSgEquivalenceStatement(*i);
    ROSE_ASSERT(equivalenceStatement);
    translateEquivalenceStatement(equivalenceStatement);
    statementList.push_back(equivalenceStatement);
    removeList.push_back(equivalenceStatement);
  }



  // Simple traversal, bottom-up, to translate the rest
  f2cTraversal f2c;
  f2c.traverseInputFiles(project,postorder);

  // removing all the unsed statement from AST
  for(vector<SgStatement*>::iterator i=statementList.begin(); i!=statementList.end(); ++i)
  {
    removeStatement(*i);
    (*i)->set_parent(NULL);
  }

  // The last step to refactor loops to have a 0-based C-style loop
  forloopTraversal translateforLoop;
  traverseMemoryPoolVisitorPattern(translateforLoop);
  for(vector<SgForStatement*>::iterator i=forStmtList.begin(); i!=forStmtList.end(); ++i)
  {
    SgStatementPtrList &init = (*i) ->get_init_stmt();
    if (init.size() !=1) // We only handle one statement case
      {
        cout << "Skipping index shifting: loop contains multiple init statements" << endl;
        continue;
      }
    SgStatement* init1 = init.front();
    SgExprStatement* assign = isSgExprStatement(init1);
    if(assign)
    {
      SgAssignOp* assignOp = isSgAssignOp(assign->get_expression());
      ROSE_ASSERT(assignOp);
      SgExpression* lowBound = isSgExpression(assignOp->get_rhs_operand());
      ROSE_ASSERT(lowBound);
      int offSet = 0;
      SgExpression* newLowerBound;
      if(isSgIntVal(lowBound))
      {
        offSet = isSgIntVal(lowBound)->get_value();
        newLowerBound = buildIntVal(isSgIntVal(lowBound)->get_value()-1);
      }
      else if(isSgAddOp(lowBound))
      {
        SgAddOp* addop = isSgAddOp(lowBound);
        if(!isSgIntVal(addop->get_rhs_operand()))
          continue;
        offSet = isSgIntVal(addop->get_rhs_operand())->get_value();
        newLowerBound = addop->get_lhs_operand();
      }
      else
      {
        cout << "Skipping index shifting: loop init statement is not SgExprStatement" << endl;
        continue;
      }
// convert test statement
      if(offSet%2 != 1)  // Now only test offSet = 1
      {
        cout << "Skipping index shifting: Offset is not 1" << endl;
        continue;
      }
      SgExprStatement* testStmt = isSgExprStatement((*i)->get_test());
      ROSE_ASSERT(testStmt);
      SgExpression* testExpr = testStmt->get_expression();
      SgExpression* oldExp;
      SgExpression* newExp;
      if(isSgLessOrEqualOp(testExpr))
      {
        SgLessOrEqualOp* oldTest = isSgLessOrEqualOp(testExpr);
        SgLessThanOp* newTest = buildLessThanOp(deepCopy(oldTest->get_lhs_operand()),deepCopy(oldTest->get_rhs_operand()));
        oldExp = oldTest;
        newExp = newTest;
      }
      else if(isSgLessThanOp(testExpr))
      {
        SgLessThanOp* oldTest = isSgLessThanOp(testExpr);
        SgSubtractOp* newUpbound = buildSubtractOp(deepCopy(oldTest->get_rhs_operand()), buildIntVal(1));
        oldExp = oldTest->get_rhs_operand();
        newExp = newUpbound;
      }
      else
        continue;
// convert all subscript
      bool validSubscript = true;
      SgInitializedName* indexName = getLoopIndexVariable(*i);
      SgSymbol* indexSym = indexName->get_symbol_from_symbol_table();
      Rose_STL_Container<SgNode*> varRefs = NodeQuery::querySubTree((*i)->get_loop_body(),V_SgVarRefExp);
      for (Rose_STL_Container<SgNode *>::iterator j = varRefs.begin(); j != varRefs.end(); j++)
      {
// need to find the "-n" in subscript
        SgVarRefExp* varRef = isSgVarRefExp(*j);
        SgVariableSymbol* varSym = varRef->get_symbol();
        if(varSym != indexSym)  
          continue;
        SgNode* parentNode = varRef;
        while(!isSgPntrArrRefExp(parentNode->get_parent()))
        {
          if(parentNode->get_parent())
            parentNode = parentNode->get_parent();
          else 
          {
           cout << "Skipping index shifting: index is used not only in subscript" << endl;
            validSubscript = false;
            break;
          }
        }
        if(!isSgSubtractOp(parentNode))
        {
          validSubscript = false;
          break;
        }
        SgSubtractOp* subOp = isSgSubtractOp(parentNode);
        SgIntVal* rhsOp = isSgIntVal(subOp->get_rhs_operand());
        if(!rhsOp)
        {
          validSubscript = false;
          break;
        }
// this might need more consideration
        if(rhsOp->get_value() != 1)
        {
          validSubscript = false;
          break;
        }
      }
// perform replacement
      if(!validSubscript)
      {
        cout << "Skipping index shifting: invalid array subscript" << endl;
        continue;
      }
//      cout << "Perform loop shifting!!" << endl;
      // reset the lowerbound to 0
        assignOp->set_rhs_operand(newLowerBound);
      // replace new test statement
      replaceExpression(oldExp, newExp,false);
      for (Rose_STL_Container<SgNode *>::iterator j = varRefs.begin(); j != varRefs.end(); j++)
      {
// need to find the "-n" in subscript
        SgVarRefExp* varRef = isSgVarRefExp(*j);
        SgVariableSymbol* varSym = varRef->get_symbol();
        if(varSym != indexSym)  
          continue;
        SgNode* parentNode = varRef;
        while(!isSgPntrArrRefExp(parentNode->get_parent()))
        {
          if(parentNode->get_parent())
            parentNode = parentNode->get_parent();
          else 
            break;
        }
        SgSubtractOp* subOp = isSgSubtractOp(parentNode);
        ROSE_ASSERT(subOp);
        SgExpression* lhsOp = isSgExpression(subOp->get_lhs_operand());
        SgIntVal* rhsOp = isSgIntVal(subOp->get_rhs_operand());
        ROSE_ASSERT(lhsOp);
        ROSE_ASSERT(rhsOp);
        replaceExpression(subOp,deepCopy(lhsOp), true);
        deleteAST(rhsOp);
      }
    }
    else
      cout << "Skipping index shifting: loop init statement is not SgExprStatement" << endl;
  }
      
  // deepDelete the removed nodes 
  for(vector<SgNode*>::iterator i=removeList.begin(); i!=removeList.end(); ++i)
  {
    deepDelete(*i);
  }

  project->set_C_only(true);
  project->set_Fortran_only(false);
      
/*
  1. There should be no Fortran-specific AST nodes in the whole
     AST graph after the translation. 
  
  TODO: make sure translator generating clean AST 
*/
//    generateDOT(*project);
  if (SgProject::get_verbose() > 2)
    generateAstGraph(project,8000);
  return backend(project);
}

