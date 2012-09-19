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
vector<SgVariableDeclaration*> variableDeclList;
vector<SgPntrArrRefExp*> pntrArrRefList;
vector<SgEquivalenceStatement*> equivalenceList;
map<SgVariableSymbol*,SgExpression*> parameterSymbolList;
vector<SgStatement*> statementList;
vector<SgNode*> removeList;

class variableDeclTraversal : public ROSE_VisitorPattern
{
  public:
    void visit(SgVariableDeclaration* varDecl);
};

void variableDeclTraversal::visit(SgVariableDeclaration* varDecl)
{
  variableDeclList.push_back(varDecl);
}

class pntrArrRefTraversal : public ROSE_VisitorPattern
{
  public:
    void visit(SgPntrArrRefExp* pntrArrRefExp);
};

void pntrArrRefTraversal::visit(SgPntrArrRefExp* pntrArrRefExp)
{
  pntrArrRefList.push_back(pntrArrRefExp);
}

class equivalencelTraversal : public ROSE_VisitorPattern
{
  public:
    void visit(SgEquivalenceStatement* equivalence);
};

void equivalencelTraversal::visit(SgEquivalenceStatement* equivalence)
{
  equivalenceList.push_back(equivalence);
}

class arrayTypeTraversal : public ROSE_VisitorPattern
{
  public:
    void visit(SgArrayType* type);
};

void arrayTypeTraversal::visit(SgArrayType* type)
{
  arrayTypeList.push_back(type);
}

class f2cTraversal : public AstSimpleProcessing
{
  public:
    virtual void visit(SgNode* n);
};

void f2cTraversal::visit(SgNode* n)
{
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
//    case V_SgAttributeSpecificationStatement:
//      {
//        SgAttributeSpecificationStatement* attributeSpecificationStatement = isSgAttributeSpecificationStatement(n);
//        ROSE_ASSERT(attributeSpecificationStatement);
//        translateAttributeSpecificationStatement(attributeSpecificationStatement);
//        statementList.push_back(attributeSpecificationStatement);
//        removeList.push_back(attributeSpecificationStatement);
//        break;
//      }
    case V_SgFunctionCallExp:
      {
        SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(n);
        ROSE_ASSERT(functionCallExp);
        translateImplicitFunctionCallExp(functionCallExp);
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
//        PreprocessingInfo* defMaxInfo = new PreprocessingInfo(PreprocessingInfo::CpreprocessorDefineDeclaration,
//                                                              "#define max(a,b) (((a)>(b))?(a):(b))", 
//                                                              "Transformation generated",
//                                                              0, 0, 0, PreprocessingInfo::before);
//        defMaxInfo->set_file_info(global->get_file_info());
//        global->addToAttachedPreprocessingInfo(defMaxInfo,PreprocessingInfo::before);
//        PreprocessingInfo* defMinInfo = new PreprocessingInfo(PreprocessingInfo::CpreprocessorDefineDeclaration,
//                                                              "#define min(a,b) (((a)<(b))?(a):(b))", 
//                                                              "Transformation generated",
//                                                              0, 0, 0, PreprocessingInfo::before);
//        defMinInfo->set_file_info(global->get_file_info());
//        global->addToAttachedPreprocessingInfo(defMinInfo,PreprocessingInfo::before);
        break;
      }
    default:
      break;
  }
}

int main( int argc, char * argv[] )
{
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

  if (SgProject::get_verbose() > 2)
    generateAstGraph(project,8000,"_orig");
  
  // Traversal with Memory Pool to search for variableDeclaration
  variableDeclTraversal translateVariableDeclaration;
  traverseMemoryPoolVisitorPattern(translateVariableDeclaration);
  for(vector<SgVariableDeclaration*>::iterator dec=variableDeclList.begin(); dec<variableDeclList.end(); ++dec)
  {
    /*
       For the Fortran AST, a single variableDeclaration can be shared by multiple variables.
       This violated the normalization rules for C unparser.  Therefore, we have to transform it.
    */
    SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(*dec);
    ROSE_ASSERT(variableDeclaration);
    if((variableDeclaration->get_variables()).size() != 1)
    {
      SgInitializedNamePtrList varList = variableDeclaration->get_variables();
      SgScopeStatement* scope = variableDeclaration->get_scope(); 
      for(vector<SgInitializedName*>::iterator i=varList.begin(); i<varList.end(); ++i)
      {
        SgVariableDeclaration* newDecl = SageBuilder::buildVariableDeclaration((*i)->get_name(), (*i)->get_type(),(*i)->get_initializer(),scope);
        insertStatementBefore(variableDeclaration,newDecl,true);
        SgVariableSymbol* symbol = isSgVariableSymbol((*i)->search_for_symbol_from_symbol_table());
        ROSE_ASSERT(symbol);
        SgInitializedName* newIntializedName = *((newDecl->get_variables()).begin());
        newIntializedName->set_prev_decl_item(NULL);
        symbol->set_declaration(newIntializedName);
      }
    statementList.push_back(variableDeclaration);
    removeList.push_back(variableDeclaration);
    }
  }

  // replace the parameter 
  Rose_STL_Container<SgNode*> AttributeSpecificationStatement = NodeQuery::querySubTree (project,V_SgAttributeSpecificationStatement);
  for (Rose_STL_Container<SgNode*>::iterator i = AttributeSpecificationStatement.begin(); i != AttributeSpecificationStatement.end(); i++)
  {
    SgAttributeSpecificationStatement* attributeSpecificationStatement = isSgAttributeSpecificationStatement(*i);
    ROSE_ASSERT(attributeSpecificationStatement);
    translateAttributeSpecificationStatement(attributeSpecificationStatement);
    statementList.push_back(attributeSpecificationStatement);
    removeList.push_back(attributeSpecificationStatement);
  }

  // remove the parameter symbol
  Rose_STL_Container<SgNode*> parameterRefList = NodeQuery::querySubTree (project,V_SgVarRefExp);
  for (Rose_STL_Container<SgNode*>::iterator i = parameterRefList.begin(); i != parameterRefList.end(); i++)
  {
    SgVarRefExp* parameterRef = isSgVarRefExp(*i);
    if(parameterSymbolList.find(parameterRef->get_symbol()) != parameterSymbolList.end())
    {
      SgExpression* newExpr = isSgExpression(deepCopy(parameterSymbolList.find(parameterRef->get_symbol())->second));
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

    generateAstGraph(project,8000);

  // Traversal with Memory Pool to search for arrayType
  arrayTypeTraversal translateArrayType;
  traverseMemoryPoolVisitorPattern(translateArrayType);
  for(vector<SgArrayType*>::iterator i=arrayTypeList.begin(); i<arrayTypeList.end(); ++i)
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
  for(vector<SgPntrArrRefExp*>::iterator i=pntrArrRefList.begin(); i<pntrArrRefList.end(); ++i)
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

  // Traversal with Memory Pool to search for equivalenceStatement
  equivalencelTraversal translateEquivalenceStmt;
  traverseMemoryPoolVisitorPattern(translateEquivalenceStmt);
  for(vector<SgEquivalenceStatement*>::iterator i=equivalenceList.begin(); i<equivalenceList.end(); ++i)
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
  for(vector<SgStatement*>::iterator i=statementList.begin(); i<statementList.end(); ++i)
  {
    removeStatement(*i);
    (*i)->set_parent(NULL);
  }
      
  // deepDelete the removed nodes 
  for(vector<SgNode*>::iterator i=removeList.begin(); i<removeList.end(); ++i)
  {
    deepDelete(*i);
  }
      
/*
  1. There should be no Fortran-specific AST nodes in the whole
     AST graph after the translation. 
  
  TODO: make sure translator generating clean AST 
*/
    //generateDOT(*project);
  if (SgProject::get_verbose() > 2)
    generateAstGraph(project,8000);
  return backend(project);
}

