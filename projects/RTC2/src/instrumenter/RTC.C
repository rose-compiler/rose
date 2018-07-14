/*
 * SAM: funDecl.C assertion current being suppressed. Has my name next to it.
 * Currently, need to update nodeposition relative to annotated list, not nodestotraverse.
 */

#include <list>
#include "RTC.h"

namespace sb = SageBuilder;
namespace si = SageInterface;

//CheckBase* checkbaseobject;
//NodeContainer TraversalBase::MallocRefs;
//NodeContainer TraversalBase::VarNames;
//NodeContainer TraversalBase::VarRefs;

std::list<Instr::TempNameAttr*> annotatedList;
//std::list<SgExpression*> annotatedList;
//std::list<SgExpression*> annotatedOriginals;

void rebuildTable(SgSourceFile*, SgProject* proj)
{
   Rose_STL_Container <SgScopeStatement*> scopes = si::querySubTree<SgScopeStatement>(proj, V_SgScopeStatement);

   for (size_t i = 0; i < scopes.size(); ++i)
   {
      //((SgSourceFile*)project->get_fileList()[0])->get_globalScope()->set_symbol_table(NULL);
     scopes[i]->set_symbol_table(NULL);
     si::rebuildSymbolTable(scopes[i]);
   }
}

void myRoutine(SgProject* project)
{
  struct ToolVisitor : AstSimpleProcessing
  {
    void atTraversalStart()
    {
      // Any preliminary work that needs to be done can go here.
    }

    void atTraversalEnd()
    {
      // Any work that needs to be done after the traversal can go here.
    }

    void visit (SgNode* node)
    {
      std::string attrname = Instr::TempNameAttr::attributeName();

      // Visitor logic here.
      SgStatement* v = isSgStatement(node);

      if (v && v->attributeExists(attrname))
      {
        annotatedList.push_back(dynamic_cast<Instr::TempNameAttr*>(v->getAttribute(attrname)));
      }
    }
  };

  ToolVisitor visitor;

  visitor.traverse(project, preorder);
}

void insertTimingSupport(SgProject* project)
{
  SgFilePtrList file_list = project->get_fileList();

  for(SgFilePtrList::iterator iter = file_list.begin(); iter!=file_list.end(); ++iter)
  {
    SgSourceFile* cur_file = isSgSourceFile(*iter);
    SgGlobal* global_scope = cur_file->get_globalScope();
    si::insertHeader("foo2_c.h", PreprocessingInfo::after,false,global_scope);
  }

  SgFunctionDeclaration* mainFn     = si::findMain(project);
  SgScopeStatement*      scope      = mainFn->get_scope();
  SgFunctionDefinition*  mainFnDef  = mainFn->get_definition();
  SgBasicBlock*          firstBlock = mainFnDef->get_body();
  SgStatementPtrList&    stmts      = firstBlock->get_statements();
  SgName                 s_name("StartClock");
  SgExprListExp*         paramLst   = sb::buildExprListExp();
  SgExprStatement*       newFunc    = sb::buildFunctionCallStmt( s_name,
                                                                 SgTypeVoid::createType(),
                                                                 paramLst,
                                                                 scope
                                                               );

  SgStatementPtrList::iterator firstStmt = stmts.begin();
  // Equivalent to above, I guess
  //   SgStatement* firstStmt = si::getFirstStatement(scope);
  si::insertStatementBefore(*firstStmt, newFunc);

  #if 1
  SgName                 s_name2 ("EndClock");
  SgExprListExp*         paramLst2 = sb::buildExprListExp();
  SgExprStatement*       newFunc2 = sb::buildFunctionCallStmt( s_name2,
                                                               SgTypeVoid::createType(),
                                                               paramLst2,
                                                               scope
                                                             );
  SgStatement*           lastStmt = stmts.back();

  si::insertStatementBefore(lastStmt, newFunc2);
  #endif
}

bool checkPntrArrRefExp(SgPntrArrRefExp* arrRef)
{
  SgExpression* lhs = arrRef->get_lhs_operand();

  if (isSgBinaryOp(lhs) && !isSgPntrArrRefExp(lhs))
  {
    lhs = isSgBinaryOp(lhs)->get_lhs_operand();
  }

  SgExpression* rhs = arrRef->get_rhs_operand();

  if(isSgPntrArrRefExp(lhs))
  {
    if(!checkPntrArrRefExp(isSgPntrArrRefExp(lhs)))
    {
      return false;
    }

    lhs = si::copyExpression(isSgPntrArrRefExp(lhs)->get_lhs_operand());

    if(isSgPntrArrRefExp(lhs))
    {
      isSgPntrArrRefExp(lhs)->set_rhs_operand(SB::buildIntVal(0));
    }
  }

  /*
  std::cout << "-------------" << std::endl;
  Util::printNode(arrRef);
  Util::printNode(lhs);
  Util::printNode(rhs);
  */

  if(isSgVarRefExp(lhs) ){
          SgVarRefExp* varRef = isSgVarRefExp(lhs);
                SgInitializedName* initName = isSgInitializedName(varRef->get_symbol()->get_declaration());

                SgArrayType* arrType = isSgArrayType(initName->get_type());


                if(arrType){

                        SgExpression* szExpr = arrType->get_index();
                        si::const_int_expr_t szValue =  si::evaluateConstIntegerExpression(szExpr);
                        si::const_int_expr_t accessIndexValue =  si::evaluateConstIntegerExpression(rhs);
                        if (  szValue.hasValue_
                           && szValue.value_ > 0
                           && szValue.value_ < INT_MAX
                           && accessIndexValue.hasValue_
                           && szValue.value_ > accessIndexValue.value_
                           // && accessIndexValue.value_ >= 0
                           )
                           return true;

                        else if(szValue.hasValue_  && szValue.value_ > 0 && szValue.value_ < INT_MAX &&   isSgVarRefExp(rhs)) {
                                SgInitializedName* origin = isSgInitializedName(isSgVarRefExp(rhs)->get_symbol()->get_declaration());

        SgForStatement* forstmt = isSgForStatement(si::findEnclosingLoop(si::getEnclosingStatement(arrRef)));

        if(forstmt && si::getScope(origin) == forstmt && si::isLoopIndexVariable(origin,arrRef))
        {
          SgExpression* lb;
          SgExpression* ub;
          if (si::isCanonicalForLoop(forstmt,NULL,&lb,&ub))
          {
            si::const_int_expr_t lbValue = si::evaluateConstIntegerExpression(lb);
            si::const_int_expr_t ubValue = si::evaluateConstIntegerExpression(ub);

            if (lbValue.hasValue_ && ubValue.hasValue_)
            {
              bool lbSafe = /*(0 <= lbValue.value_) &&*/ (lbValue.value_ < szValue.value_);
              bool ubSafe = /*(0 <= ubValue.value_) &&*/ (ubValue.value_ < szValue.value_);

              if (lbSafe && ubSafe) return true;
            }
          }
        }
      }

      //Note: This part is very unsafe.
      SgStatement* eStmt = si::getEnclosingStatement(arrRef);
      if(eStmt){
        SgExpression* lb;
                                SgExpression* ub;
        SgInitializedName* ivar;
        SgScopeStatement* scope =  si::findEnclosingLoop(eStmt);
        if( scope &&   si::isCanonicalForLoop(scope,&ivar,&lb,&ub )  && isSgVarRefExp(rhs) && isSgVarRefExp(rhs)->get_symbol()->get_declaration() == ivar ){


          std::cout << "HAI!\n" << std::endl;
          /*
          SgExpression* lhsCopy0 = si::copyExpression(arrRef->get_lhs_operand());
          SgExpression* lhsCopy1 = si::copyExpression(arrRef->get_lhs_operand());
          SgPntrArrRefExp* lbCheck = SB::buildPntrArrRefExp(lhsCopy0,si::copyExpression(lb));
          SgPntrArrRefExp* ubCheck = SB::buildPntrArrRefExp(lhsCopy1,SB::buildSubtractOp(si::copyExpression(ub),SB::buildIntVal(1)) );
          SgExprStatement* lbStmt = SB::buildExprStatement(SB::buildAssignOp(lbCheck,si::copyExpression(lbCheck)));
          SgExprStatement* ubStmt = SB::buildExprStatement(SB::buildAssignOp(ubCheck,si::copyExpression(ubCheck)));
          lbStmt->set_isModified(true);
          ubStmt->set_isModified(true);
          si::insertStatement(scope,lbStmt);
          si::insertStatement(scope,ubStmt);
          Util::printNode(lbStmt);
          Util::printNode(ubStmt);
          */


          SgExpression* lbCheck =  Instr::createArrayBoundCheckFn(NULL,si::copyExpression(lhs),si::copyExpression(lb),arrRef->get_type(),scope,scope,true);
          SgExprStatement* lbStmt = SB::buildExprStatement(lbCheck);
          si::insertStatement(scope,lbStmt);

          Util::printNode(ub);
          SgExpression* ubCheck =  Instr::createArrayBoundCheckFn(NULL,si::copyExpression(lhs),SB::buildSubtractOp(si::copyExpression(ub),SB::buildIntVal(1)),arrRef->get_type(),scope,scope,true);
          SgExprStatement* ubStmt = SB::buildExprStatement(ubCheck);
          si::insertStatement(scope,ubStmt);

          //Trav::NodesToInstrument.insert(Trav::NodesToInstrument.begin(),lbCheck);
          //Trav::NodesToInstrument.insert(Trav::NodesToInstrument.begin(),ubCheck);
          //Trav::NodesToInstrument.push_back(lbCheck);
          //Trav::NodesToInstrument.push_back(ubCheck);
          return true;
        }
      }






                }


  }

  return false;

}

void pruneNodesToInstrument(){
  int count = 0;
    NodeContainer::iterator i =  Trav::NodesToInstrument.begin();
  for ( ; i != Trav::NodesToInstrument.end()  ; ){
    //std::cout << Trav::NodesToInstrument[i]->sage_class_name() << std::endl;
    SgNode* node = *i;
    if(isSgPntrArrRefExp(node) && !node->get_isModified() && checkPntrArrRefExp(isSgPntrArrRefExp(node))) {
       Trav::NodesToInstrument.erase(i);
       count++;
    } else
      i++;
  }
  std::cout << "Nodes pruned : " << count << std::endl;

}

void flipNodes(SgProject* project)
{
  myRoutine(project);
  if(annotatedList.size() == 0) return;

  std::cout << annotatedList.size() << std::endl;

  for( std::list<Instr::TempNameAttr*>::iterator replacementIterator = annotatedList.begin();
       replacementIterator != annotatedList.end();
       ++replacementIterator
     )
  {
    Instr::TempNameAttr* current = (*replacementIterator);

    current->swap();
  }

  int counter = 0;

  for( std::list<Instr::TempNameAttr*>::reverse_iterator replacementIterator = annotatedList.rbegin();
       replacementIterator != annotatedList.rend();
       ++replacementIterator
     )
  {
    Instr::TempNameAttr* current = (*replacementIterator);

    if (Instr::nodePosition == counter)
    {
      current->swap();
      break;
    }

    ++counter;
  }
}

//Sam Collie: Write to a file the current position
void writeToFile(int position){
  std::ofstream myfile;
  myfile.open ("example.txt");
  myfile << position << "\n";
  myfile.close();
//./RTC  -rose:RTC:instrIndex:1 -rose:RTC:staggered:"anystringwilldo"
}

static inline
SgScopeStatement* getFileScope(SgProject* proj, size_t /* filenum */)
{
  SgFile*       file = proj->get_fileList()[0];
  SgSourceFile* srcfile = isSgSourceFile(file);
  ROSE_ASSERT(srcfile);

  return srcfile->get_globalScope();
}


int main(int argc, char* argv[])
{
  std::string tempString;
  std::string staggered = "";
  Rose_STL_Container<std::string> arguments = CommandlineProcessing::generateArgListFromArgcArgv(argc,argv);
  CommandlineProcessing::isOptionWithParameter(arguments, "-rose:RTC:", "instrIndex", tempString, true);
  CommandlineProcessing::isOptionWithParameter(arguments, "-rose:RTC:", "staggered", staggered, true);

  if(staggered != "")
     Instr::STAGGERED = true;

  size_t position = 0;
  std::istringstream(tempString) >> position;
  Rose_STL_Container<std::string> ArgList = CommandlineProcessing::generateArgListFromArgcArgv(argc, argv);

  bool reduction = false;
  for (Rose_STL_Container<std::string>::iterator rit = ArgList.begin(); rit != ArgList.end(); ++rit)
  {
    if (*rit == "-reduction")
    {
      reduction = true;
      break;
    }
  }

  SgProject* project = frontend(arguments);
  ROSE_ASSERT(project != NULL);

  // So here's what we do... three steps...
  // 1. preprocess the ast
  //    this is where all the simplify expressions,
  //    and other modifications before the main traversal
  //    happen
  // 2. the main traversal
  //    this the top down bottom up traversal that
  //    feeds our instrumentation. It fills up the
  //    NodesToInstrument
  // 3. the instrumentation
  //    this instruments the nodes provided in
  //    NodesToInstrument.

  Util::setProject(project);
  //generateDOT(*project); //RMM

  /*
  #ifdef RMM_TYPETRACKER
    //RMM: Gather a list of type nodes to be used in the typechecking instrumentation process.
    Rose_STL_Container < SgType  *> types = si::querySubTree<  SgType  >(isSgNode(project), V_SgType );
  #endif
  */

  // preprocess the ast
  std::cerr << "Preprocessing AST" << std::endl;
  PPA::preProcess(project);

  // the main traversal
  std::cerr << "Main Traversal" << std::endl;
  Trav::traverse(project, project->get_fileList()[0]);

  if (position > Trav::NodesToInstrument.size())
  {
    writeToFile(-1);
    return 0;
  }

  // SgSourceFile* backup = (SgSourceFile*) si::deepCopyNode(project->get_fileList()[0]);
  if (reduction)
  {
    pruneNodesToInstrument();
  }

#if 0
  // Backup Code SC
  for (NodeContainer::iterator i = Trav::NodesToInstrument.begin(); i != Trav::NodesToInstrument.begin(); i++)
  {
    if(isSgScopeStatement(*i) || isSgFunctionParameterList(*i))
    {
      if(i == Trav::NodesToInstrument.end())
      {
        writeToFile(-1); //function for outputting position
        return 0;
      }

      ++i; ++position;
    }
  }
#endif

  Instr::instrument(project, position);

  SageInterface::insertHeader( "metadata/metadata_alt.h",
                               PreprocessingInfo::after,
                               false /* not a system header */,
                               getFileScope(project, 0)
                             );

  writeToFile(++Instr::nodePosition);

  // the instrumentation
  std::cerr << "Instrumentation: position\n"
            << position  << " " << "size: " << Trav::NodesToInstrument.size()
            << std::endl;

  //store nodes to iterate in copy list
  //set value of Trav::NodesToIterate with only metadata and one check.
  //call instrument
  //call unparser
  std::cerr << "Success!" << std::endl;

/*
  #ifdef RMM_TYPETRACKER
  //RMM: Final pass for typechecking instrumentation.

  TypeCheck::instr(project,types);
  #endif
*/


#if 0
  std::cerr << "runAllTests" << std::endl;
  AstTests::runAllTests(project);
  std::cerr << "runAllTests - done" << std::endl;
#endif

  std::cerr << "backend/unparser" << std::endl;

  //~ flipNodes(project);
  //~ writeToFile(++position);
  project->unparse();

  std::cerr << "backend/unparser - done" << std::endl;
  return 0;
}
