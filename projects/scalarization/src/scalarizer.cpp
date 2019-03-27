/*
 Scalarization transformation for paraDyn

 Pei-Hung Lin (06/29/2018)

 */

#include "rose.h"
#include "rose_config.h" // obtain macros defining backend compiler names, etc.
#include "SgNodeHelper.h"
#include "AstMatching.h"
#include "AstTerm.h"

using namespace std;
using namespace SageInterface;

vector<string> getFortranTargetnameList(SgNode* root);
vector<string> getTargetnameList(SgNode* root);
void transformType(SgVariableSymbol* sym, SgType* newType);
void transformArrayType(SgBasicBlock* funcBody, SgVariableSymbol* sym, SgType* newType);
bool isFortran = false;

int main(int argc, char** argv)
{
  // Build the AST used by ROSE
  SgProject* project = frontend(argc, argv);

  isFortran = is_Fortran_language();
  // Run internal consistency tests on AST
  AstTests::runAllTests(project);

  // get variable list
  vector<string> namelist;

  // For each source file in the project
  SgFilePtrList & ptr_list = project->get_fileList();
  for (SgFilePtrList::iterator iter = ptr_list.begin(); iter!=ptr_list.end(); iter++)
  {
    SgFile* sageFile = (*iter);
    SgSourceFile * sfile = isSgSourceFile(sageFile);
    ROSE_ASSERT(sfile);
    SgGlobal *root = sfile->get_globalScope();

    // find the function list
    Rose_STL_Container<SgNode*> defList = NodeQuery::querySubTree(sfile, V_SgFunctionDefinition); 
    // for each function
    for (Rose_STL_Container<SgNode*>::iterator p = defList.begin(); p != defList.end(); ++p) 
    {
      map<SgVariableSymbol*, int> varRefList;
      
      SgFunctionDefinition *defn = isSgFunctionDefinition(*p);
      ROSE_ASSERT (defn != NULL);

      if(isFortran)
        namelist = getFortranTargetnameList(defn);  
      else
        namelist = getTargetnameList(defn);
     
      SgBasicBlock* funcBody = defn->get_body(); 
      // Process each variable name and perform the transformation
      for(vector<string>::iterator it=namelist.begin(); it != namelist.end(); it++)
      {
        cout << "Processing name: " << *it << endl;
        SgVariableSymbol* sym = lookupVariableSymbolInParentScopes(*it, funcBody);
        ROSE_ASSERT(sym);
        SgArrayType* symType = isSgArrayType(sym->get_type());
        if(symType == NULL)
        {
          cout << "Not a variable with array type" << endl;
          break;
        }
        //  Assuming single dimension array only
        SgType* baseType = symType->get_base_type();
        // Change type in variable declartion
        transformType(sym, baseType);

        // Follow the design in typeForge by Markus
        transformArrayType(funcBody, sym, baseType);

      } 
    }
  }


  generateDOT(*project);

  // Output preprocessed source file.
  unparseProject(project);
  return 0;
}

// Find variable name lsit from the pragma statement. works for Fortran only
//
vector<string> getFortranTargetnameList(SgNode* root)
{
  vector<string> resultlist;
  ROSE_ASSERT(root);

  // Fortran AST does not support SgPragmaNode.  Need to look for every comment and check if it has valid pragma
  vector<SgLocatedNode*> LocatedNodeList = SageInterface::querySubTree<SgLocatedNode> (root,V_SgLocatedNode);

  for (vector<SgLocatedNode*>::iterator i = LocatedNodeList.begin(); i != LocatedNodeList.end(); i++)
  {
    AttachedPreprocessingInfoType* comments = (*i)->getAttachedPreprocessingInfo();
    if(comments)
    {
      for (AttachedPreprocessingInfoType::iterator j = comments->begin(); j != comments->end(); j++)
      {
        PreprocessingInfo * pinfo = *j;
        if(pinfo->getTypeOfDirective() == PreprocessingInfo::FortranStyleComment)
        {
           string buffer = pinfo->getString();
           if(buffer.compare(0,21,"!pragma privatization") == 0)
           {
             cout << "found pragma!!" << endl;
             SgVariableDeclaration* varDeclStmt = isSgVariableDeclaration(*i);
             ROSE_ASSERT(varDeclStmt);
             SgInitializedNamePtrList varList = varDeclStmt->get_variables();

             for(vector<SgInitializedName*>::iterator i=varList.begin(); i<varList.end(); ++i)
             {
               SgVariableSymbol* symbol = isSgVariableSymbol((*i)->search_for_symbol_from_symbol_table());
               ROSE_ASSERT(symbol);
               SgName varname = symbol->get_name();
               resultlist.push_back(varname.getString());
             }
           }
        }
      }
    }
  }
  return resultlist;
}


// Find variable name lsit from the pragma statement. works for C only
//
vector<string> getTargetnameList(SgNode* root)
{
  vector<string> resultlist;
  ROSE_ASSERT(root);

  // Search for SgPragmaDeclaration to find C pragma list
  Rose_STL_Container<SgNode*> pragmaList = NodeQuery::querySubTree(root, V_SgPragmaDeclaration); 
  for(Rose_STL_Container<SgNode*>::iterator it = pragmaList.begin(); it != pragmaList.end(); it++)
  {
    ROSE_ASSERT(*it);
    SgPragmaDeclaration* pragmaStmt = isSgPragmaDeclaration(*it);
    SgPragma* pragma = isSgPragma(pragmaStmt->get_pragma());
    ROSE_ASSERT(pragma); 
    string srcString = pragma->get_pragma();
    if(srcString.compare(0,21,"!pragma privatization") == 0)
    {
      cout << "found pragma!!" << endl;
      SgVariableDeclaration* varDeclStmt = isSgVariableDeclaration(*it);
      ROSE_ASSERT(varDeclStmt);
      SgInitializedNamePtrList varList = varDeclStmt->get_variables();

      for(vector<SgInitializedName*>::iterator i=varList.begin(); i<varList.end(); ++i)
      {
        SgVariableSymbol* symbol = isSgVariableSymbol((*i)->search_for_symbol_from_symbol_table());
        ROSE_ASSERT(symbol);
        SgName varname = symbol->get_name();
        resultlist.push_back(varname.getString());
      }
    }
  }
  return resultlist;
}

// Change the type of a variable symbol
void transformType(SgVariableSymbol* sym, SgType* newType)
{
  SgInitializedName* initName = sym->get_declaration();
  initName->set_type(newType);
}

// Change the type of a variable symbol
void transformArrayType(SgBasicBlock* funcBody, SgVariableSymbol* sym, SgType* newType)
{
  RoseAst ast(funcBody);
  std::string matchexpression;
  // $ARR[$IDX1,$IDX2]=$RHS
  matchexpression+="$Root=SgPntrArrRefExp($ARR,$IDX1)";
  AstMatching m;
  MatchResult r=m.performMatching(matchexpression,funcBody);
  for(MatchResult::iterator i=r.begin();i!=r.end();++i) {
    std::cout << "MATCH-LHS: \n"; 
    //SgNode* n=(*i)["X"];
    for(SingleMatchVarBindings::iterator vars_iter=(*i).begin();vars_iter!=(*i).end();++vars_iter) {
      SgNode* matchedTerm=(*vars_iter).second;
      std::cout << "  VAR: " << (*vars_iter).first << "=" << AstTerm::astTermWithNullValuesToString(matchedTerm) << " @" << matchedTerm << std::endl;
    }
    SgNode* root=(*i)["$Root"];
    SgPntrArrRefExp* arrayRef = isSgPntrArrRefExp(root);
    if((*i)["$ARR"]) {
      SgVarRefExp* lhsVarRef = isSgVarRefExp((*i)["$ARR"]);
      ROSE_ASSERT(lhsVarRef);
      SgVariableSymbol* arrayNameSymbol = lhsVarRef->get_symbol();
      if(arrayNameSymbol==sym) {
        SgNodeHelper::replaceExpression(arrayRef,lhsVarRef,false);
      } else {
        cout<<"DEBUG: lhs-matches, but symbol does not. skipping."<<arrayNameSymbol->get_name()<<"!="<<sym->get_name()<<endl;
        continue;
      }
    }
  }
}

