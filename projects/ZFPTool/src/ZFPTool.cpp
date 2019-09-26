/*
 Scalarization transformation for paraDyn

 Pei-Hung Lin (06/29/2018)

 */
#include "ZFPTool.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

bool isFortran = false;

void replaceFuncArgument(SgExpressionPtrList expList, int idx)
{
  SgExpression* firstExp = expList[idx];
  Rose_STL_Container<SgNode*> dotExprList = NodeQuery::querySubTree(firstExp, V_SgDotExp);
  for (Rose_STL_Container<SgNode*>::iterator p = dotExprList.begin(); p != dotExprList.end(); ++p)
  {
    cout << "find address of expr" << endl;
    SgDotExp* dotExp = isSgDotExp(*p);
    SgAddressOfOp* addrOfExp = buildAddressOfOp(deepCopy(dotExp->get_lhs_operand()));
    if(isSgCastExp(dotExp->get_parent()))
    {
      SgCastExp* castExp = isSgCastExp(dotExp->get_parent());
      replaceExpression(castExp, addrOfExp, false);
    }
    else
      replaceExpression(dotExp, addrOfExp, false);
  }

}

int main(int argc, char** argv)
{
  // Build the AST used by ROSE
  SgProject* project = frontend(argc, argv);

  AstTests::runAllTests(project);

  generateDOT(*project);

  // insert Fortran header
   SgFilePtrList & ptr_list = project->get_fileList();
  for (SgFilePtrList::iterator iter = ptr_list.begin(); iter!=ptr_list.end(); iter++)
  {
    SgFile* sageFile = (*iter);
    SgSourceFile * sfile = isSgSourceFile(sageFile);
    ROSE_ASSERT(sfile);
    std::cout << "processing intput" << sfile->get_sourceFileNameWithoutPath() << " arg[1]:" << argv[1] << std::endl;
    if(sfile->get_sourceFileNameWithPath() != argv[1])
      continue;

    // inserting Fortran API header file
    insertHeader(sfile, "zFORp.h", false);
    
    SgGlobal* globalScope = getFirstGlobalScope(project);
//    SgStatement* firstStmt = getFirstStatement(globalScope, false);
//    ROSE_ASSERT(firstStmt);
    //PreprocessingInfo* hashdef = attachArbitraryText(firstStmt,"#ifdef TEST_FORTRAN",PreprocessingInfo::before); 
    //PreprocessingInfo* newheader = new PreprocessingInfo(PreprocessingInfo::CpreprocessorIncludeDeclaration,"zFORp.h", "transformation-generated", 0, 0, 0, PreprocessingInfo::before);
    //insertHeader(firstStmt, newheader, true);
    //PreprocessingInfo* hashelse = attachArbitraryText(firstStmt,"#else ",PreprocessingInfo::before); 
  
  // replace container
  Rose_STL_Container<SgNode*> classDeclList = NodeQuery::querySubTree(sfile, V_SgClassDeclaration);
  // for each class
  for (Rose_STL_Container<SgNode*>::iterator p = classDeclList.begin(); p != classDeclList.end(); ++p)
     {
        SgClassDeclaration *classDecl = isSgClassDeclaration(*p);
        if(classDecl->get_parent() == globalScope)
        {
           // look for defintion	
           SgClassDefinition* classDef = isSgClassDefinition(classDecl->get_definition());
           if (classDef)
             std::cout << "found " << classDecl->get_name() <<  " in file " << sfile->get_sourceFileNameWithoutPath() << std::endl;
// Place to insert macro definition, check if there is rule to handle this part
          if ( classDecl->get_name() == "zfp_struct_container")
             {
                // new #define added to be before the forward declaration  
                if (classDecl->isForward())
                {
                  bool surroundingStatementPreceedsTargetStatement = true;
                  SgStatement* neighborStmt = findSurroundingStatementFromSameFile(classDecl, surroundingStatementPreceedsTargetStatement);
                  PreprocessingInfo* newdef = attachArbitraryText (neighborStmt, "#define zfp_struct_container zFORp_structs", PreprocessingInfo::before );
                }
                removeStatement(classDecl);
             }
        }
     }

  Rose_STL_Container<SgNode*> typedefDeclList = NodeQuery::querySubTree(sfile, V_SgTypedefDeclaration);
  // for each class
  for (Rose_STL_Container<SgNode*>::iterator p = typedefDeclList.begin(); p != typedefDeclList.end(); ++p)
     {
        SgTypedefDeclaration *typedefDecl = isSgTypedefDeclaration(*p);
        if(typedefDecl->get_parent() == globalScope)
        {
          if ( typedefDecl->get_name() == "zfp_struct_container")
             {
               std::cout << "found zfp_struct_container typedef" << std::endl;
               removeStatement(typedefDecl);
             }
        }
     }


  // memory pool searching for function call expression
  Rose_STL_Container<SgNode*> funcCallList = NodeQuery::querySubTree(sfile, V_SgFunctionCallExp);
  // for each class
  for (Rose_STL_Container<SgNode*>::iterator p = funcCallList.begin(); p != funcCallList.end(); ++p)
     {
        SgFunctionCallExp* funcCallExp = isSgFunctionCallExp(*p);
        SgFunctionRefExp* funcRefExp = isSgFunctionRefExp(funcCallExp->get_function());
        SgFunctionSymbol* functionSymbol = funcRefExp->get_symbol();
//        SgFunctionSymbol* functionSymbol = funcCallExp->getAssociatedFunctionSymbol();
        string funcName = functionSymbol->get_name().getString();
        if(APIInfo.find(funcName) != APIInfo.end())
        {
            // Step 1: repalcing funciton call name
            string newname = funcName;

            // switch if _prefixFOR is needed  
            bool withPrefix = false;
            // switch if _prefixFOR is needed  
            bool isStreamOpen = false;
            // This should be bitstream prefix API
            if(newname.find("zfp",0) == string::npos)
            {
              newname.insert(0,"zforp_bitstream_");
            }
            else
            {
              newname.replace(newname.begin(),newname.begin()+3,"zforp");
              withPrefix = true;
              if(newname == "zforp_stream_open")
                isStreamOpen = true;
               
            }
            cout << "processing and repalce " << funcName << " to " << newname << endl;

//            // Step 2: repalcing funciton call parameter
            SgScopeStatement* scope = getScope(funcCallExp);
            SgExprListExp* newExprListExp = deepCopy(funcCallExp->get_args());
            SgExpressionPtrList expList = newExprListExp->get_expressions();
            if(isStreamOpen)
            {
              SgStatement* stmt = getEnclosingStatement(funcCallExp);
              ROSE_ASSERT(stmt);
              SgType* voidType = buildVoidType ();
              SgPointerType* voidPtrType = buildPointerType(voidType);
              SgAssignInitializer* initializer = buildAssignInitializer(buildIntVal(),NULL);
              SgVariableDeclaration* newVarDecl = buildVariableDeclaration("bs",voidPtrType, initializer, scope);
              insertStatement(stmt,newVarDecl,true,true);
//              SgInitializedNamePtrList varList = newVarDecl->get_variables();
//              ROSE_ASSERT(varList.size() == 1);
//              SgInitializedName initName = varList[0];
//              SgSymbol* symbol = initName->get_symbol_from_symbol_table();
              SgAddressOfOp* addrOfExp = buildAddressOfOp(buildVarRefExp("bs",scope));
              replaceExpression(expList[0], addrOfExp, false);

            }
            else
            {
               if(APIInfo.find(funcName)->second == onesub)
               {
                 replaceFuncArgument(expList, 0);
               }
               else if(APIInfo.find(funcName)->second == twosub)
               {
                 replaceFuncArgument(expList, 0);
                 replaceFuncArgument(expList, 1);
               }
            }
            // Step 3: perform function call repalcement
            SgFunctionCallExp* newCallExp = buildFunctionCallExp(newname, funcCallExp->get_type() , newExprListExp,scope);
           
            if(withPrefix) 
            {
              SgExpression* funcNameExp = newCallExp->get_function();
              SgNodeHelper::replaceAstWithString(funcNameExp,"_prefixFOR("+newname+")");
            }

            replaceExpression(funcCallExp, newCallExp,true);
        }
     }

  }

  // Output preprocessed source file.
  unparseProject(project);
  return 0;
}


