// Put here code used to construct SgOmp* nodes
// Liao 10/8/2010

#include "sage3basic.h"

#include "rose_paths.h"

#include "astPostProcessing.h"
#include "sageBuilder.h"
#include "OmpAttribute.h"
#include "ompAstConstruction.h"
//void processOpenMP(SgSourceFile* sageFilePtr);

//Liao, 10/27/2008: parsing OpenMP pragma here
//Handle OpenMP pragmas. This should be called after preprocessing information is attached since macro calls may exist within pragmas, Liao, 3/31/2009
extern int omp_parse();
extern OmpSupport::OmpAttribute* getParsedDirective();
extern void omp_parser_init(SgNode* aNode, const char* str);
//Fortran OpenMP parser interface
void parse_fortran_openmp(SgSourceFile *sageFilePtr);

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace OmpSupport;

// Liao 4/23/2011, special function to copy file info of the original SgPragma or Fortran comments
static bool copyStartFileInfo (SgNode* src, SgNode* dest, OmpAttribute* oa)
{
  bool result = false;
  ROSE_ASSERT (src && dest);
  // same src and dest, no copy is needed
  if (src == dest) return true;

  SgLocatedNode* lsrc = isSgLocatedNode(src);
  ROSE_ASSERT (lsrc);
  SgLocatedNode* ldest= isSgLocatedNode(dest);
  ROSE_ASSERT (ldest);
  // ROSE_ASSERT (lsrc->get_file_info()->isTransformation() == false);
  // already the same, no copy is needed
  if (lsrc->get_startOfConstruct()->get_filename() == ldest->get_startOfConstruct()->get_filename()
      && lsrc->get_startOfConstruct()->get_line() == ldest->get_startOfConstruct()->get_line()
      && lsrc->get_startOfConstruct()->get_col() == ldest->get_startOfConstruct()->get_col())
    return true; 

  Sg_File_Info* copy = new Sg_File_Info (*(lsrc->get_startOfConstruct())); 
  ROSE_ASSERT (copy != NULL);

   // delete old start of construct
  Sg_File_Info *old_info = ldest->get_startOfConstruct();
  if (old_info) delete (old_info);

  ldest->set_startOfConstruct(copy);
  copy->set_parent(ldest);
//  cout<<"debug: set ldest@"<<ldest <<" with file info @"<< copy <<endl;

  ROSE_ASSERT (lsrc->get_startOfConstruct()->get_filename() == ldest->get_startOfConstruct()->get_filename());
  ROSE_ASSERT (lsrc->get_startOfConstruct()->get_line() == ldest->get_startOfConstruct()->get_line());
  ROSE_ASSERT (lsrc->get_startOfConstruct()->get_col() == ldest->get_startOfConstruct()->get_col());

  ROSE_ASSERT (lsrc->get_startOfConstruct()->get_filename() == ldest->get_file_info()->get_filename());
  ROSE_ASSERT (lsrc->get_startOfConstruct()->get_line() == ldest->get_file_info()->get_line());
  ROSE_ASSERT (lsrc->get_startOfConstruct()->get_col() == ldest->get_file_info()->get_col());

  ROSE_ASSERT (ldest->get_file_info() == copy);
// Adjustment for Fortran, the AST node attaching the Fortran comment will not actual give out the accurate line number for the comment
  if (is_Fortran_language())
  {
    ROSE_ASSERT (oa != NULL);
    PreprocessingInfo *currentPreprocessingInfoPtr = oa->getPreprocessingInfo();
    ROSE_ASSERT (currentPreprocessingInfoPtr != NULL);
    int commentLine = currentPreprocessingInfoPtr->getLineNumber(); 
    ldest->get_file_info()->set_line(commentLine);
  }
    
  return result;
}
// Liao 3/11/2013, special function to copy end file info of the original SgPragma or Fortran comments (src) to OpenMP node (dest)
// If the OpenMP node is a body statement, we have to use the body's end file info as the node's end file info.
static bool copyEndFileInfo (SgNode* src, SgNode* dest, OmpAttribute* oa)
{
  bool result = false;
  ROSE_ASSERT (src && dest);
  
  if (isSgOmpBodyStatement(dest))
    src = isSgOmpBodyStatement(dest)->get_body();

  // same src and dest, no copy is needed
  if (src == dest) return true;

  SgLocatedNode* lsrc = isSgLocatedNode(src);
  ROSE_ASSERT (lsrc);
  SgLocatedNode* ldest= isSgLocatedNode(dest);
  ROSE_ASSERT (ldest);
  // ROSE_ASSERT (lsrc->get_file_info()->isTransformation() == false);
  // already the same, no copy is needed
  if    (lsrc->get_endOfConstruct()->get_filename() == ldest->get_endOfConstruct()->get_filename()
      && lsrc->get_endOfConstruct()->get_line()     == ldest->get_endOfConstruct()->get_line()
      && lsrc->get_endOfConstruct()->get_col()      == ldest->get_endOfConstruct()->get_col())
    return true; 

  Sg_File_Info* copy = new Sg_File_Info (*(lsrc->get_endOfConstruct())); 
  ROSE_ASSERT (copy != NULL);

   // delete old start of construct
  Sg_File_Info *old_info = ldest->get_endOfConstruct();
  if (old_info) delete (old_info);

  ldest->set_endOfConstruct(copy);
  copy->set_parent(ldest);
//  cout<<"debug: set ldest@"<<ldest <<" with file info @"<< copy <<endl;

  ROSE_ASSERT (lsrc->get_endOfConstruct()->get_filename() == ldest->get_endOfConstruct()->get_filename());
  ROSE_ASSERT (lsrc->get_endOfConstruct()->get_line()     == ldest->get_endOfConstruct()->get_line());
  ROSE_ASSERT (lsrc->get_endOfConstruct()->get_col()      == ldest->get_endOfConstruct()->get_col());


  ROSE_ASSERT (ldest->get_endOfConstruct() == copy);
// Adjustment for Fortran, the AST node attaching the Fortran comment will not actual give out the accurate line number for the comment
  if (is_Fortran_language())
  { //TODO fortran support of end file info
//    cerr<<"Error. ompAstConstruction.cpp: copying end file info is not yet implemented for Fortran."<<endl;
//    ROSE_ASSERT (false);
//    ROSE_ASSERT (oa != NULL);
//    PreprocessingInfo *currentPreprocessingInfoPtr = oa->getPreprocessingInfo();
//    ROSE_ASSERT (currentPreprocessingInfoPtr != NULL);
//    int commentLine = currentPreprocessingInfoPtr->getLineNumber(); 
//    ldest->get_file_info()->set_line(commentLine);
  }
    
  return result;
}


namespace OmpSupport
{ 
  // an internal data structure to avoid redundant AST traversal to find OpenMP pragmas
  static std::list<SgPragmaDeclaration* > omp_pragma_list; 

  // a similar list to save encountered Fortran comments which are OpenMP directives
  std::list<OmpAttribute* > omp_comment_list; 
  // A pragma list to store the dangling pragmas for Fortran end directives. 
  // There are stored to ensure correct unparsing after converting Fortran comments into pragmas
  // But they should be immediately removed during the OpenMP lowering phase
 //  static std::list<SgPragmaDeclaration* > omp_end_pragma_list; 

  // find all SgPragmaDeclaration nodes within a file and parse OpenMP pragmas into OmpAttribute info.
  void attachOmpAttributeInfo(SgSourceFile *sageFilePtr)
  {
    ROSE_ASSERT(sageFilePtr != NULL);
    if (sageFilePtr->get_openmp() == false)
      return;
    // For Fortran, search comments for OpenMP directives
    if (sageFilePtr->get_Fortran_only()||sageFilePtr->get_F77_only()||sageFilePtr->get_F90_only()||
        sageFilePtr->get_F95_only() || sageFilePtr->get_F2003_only())
    {
      parse_fortran_openmp(sageFilePtr);
    } //end if (fortran)
    else
    {
      // For C/C++, search pragma declarations for OpenMP directives 
      std::vector <SgNode*> all_pragmas = NodeQuery::querySubTree (sageFilePtr, V_SgPragmaDeclaration);
      std::vector<SgNode*>::iterator iter;
      for(iter=all_pragmas.begin();iter!=all_pragmas.end();iter++)
      {
        SgPragmaDeclaration* pragmaDeclaration = isSgPragmaDeclaration(*iter);
        ROSE_ASSERT(pragmaDeclaration != NULL);
#if 0 // We should not enforce this since the pragma may come from transformation-generated node
        if ((pragmaDeclaration->get_file_info()->isTransformation()
            && pragmaDeclaration->get_file_info()->get_filename()==string("transformation")))
        {
          cout<<"Found a pragma which is transformation generated. @"<< pragmaDeclaration;
          cout<<pragmaDeclaration->unparseToString()<<endl;
          pragmaDeclaration->get_file_info()->display("debug transformation generated pragma declaration.");
          // Liao 4/23/2011
          // #pragma omp task can shown up before a single statement body of a for loop, 
          // In this case, the frontend will insert a basic block under the loop
          // and put both the pragma and the single statement into the block.
          // AstPostProcessing() will reset the transformation flag for the pragma
          // since its parent(the block) is transformation generated, not in the original code
          ROSE_ASSERT(pragmaDeclaration->get_file_info()->isTransformation() ==false  || pragmaDeclaration->get_file_info()->get_filename()!=string("transformation"));
        }
#endif  
        SageInterface::replaceMacroCallsWithExpandedStrings(pragmaDeclaration);
        string pragmaString = pragmaDeclaration->get_pragma()->get_pragma();
        istringstream istr(pragmaString);
        std::string key;
        istr >> key;
        if (key == "omp")
        {
          // Liao, 3/12/2009
          // Outliner may move pragma statements to a new file
          // after the pragma has been attached OmpAttribute.
          // We have to skip generating the attribute again in the new file
          OmpAttributeList* previous = getOmpAttributeList(pragmaDeclaration);
          // store them into a buffer, reused by build_OpenMP_AST()
          omp_pragma_list.push_back(pragmaDeclaration);

          if (previous == NULL )
          {
            // Call parser
#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
            omp_parser_init(pragmaDeclaration,pragmaString.c_str());
            omp_parse();
#endif
            OmpAttribute* attribute = getParsedDirective();
            //cout<<"sage_gen_be.C:23758 debug:\n"<<pragmaString<<endl;
            //attribute->print();//debug only for now
            addOmpAttribute(attribute,pragmaDeclaration);
            //cout<<"debug: attachOmpAttributeInfo() for a pragma:"<<pragmaString<<"at address:"<<pragmaDeclaration<<endl;
            //cout<<"file info for it is:"<<pragmaDeclaration->get_file_info()->get_filename()<<endl;

#if 1 // Liao, 2/12/2010, this could be a bad idea. It causes trouble in comparing 
            //user-defined and compiler-generated OmpAttribute.
            // We attach the attribute redundantly on affected loops also
            // for easier loop handling later on in autoTuning's outlining step (reproducing lost pragmas)
            if (attribute->getOmpDirectiveType() ==e_for ||attribute->getOmpDirectiveType() ==e_parallel_for)
            {
              SgForStatement* forstmt = isSgForStatement(getNextStatement(pragmaDeclaration));
              ROSE_ASSERT(forstmt != NULL);
              //forstmt->addNewAttribute("OmpAttribute",attribute);
              addOmpAttribute(attribute,forstmt);
            }
#endif
          }
        }
      }// end for
    }
  }
  // Clause node builders
  //----------------------------------------------------------

  //! Build SgOmpDefaultClause from OmpAttribute, if any
  SgOmpDefaultClause * buildOmpDefaultClause(OmpAttribute* att)
  {
    ROSE_ASSERT(att != NULL);
    if (!att->hasClause(e_default))
      return NULL;
    //grab default option  
    omp_construct_enum dv = att->getDefaultValue();
    SgOmpClause::omp_default_option_enum sg_dv;
    switch (dv)
    {
      case e_default_none:
        sg_dv = SgOmpClause::e_omp_default_none;
        break;
      case e_default_shared:
        sg_dv = SgOmpClause::e_omp_default_shared;
        break;
      case e_default_private:
        sg_dv = SgOmpClause::e_omp_default_private;
        break;
      case e_default_firstprivate:
        sg_dv = SgOmpClause::e_omp_default_firstprivate;
        break;
      default:
        {
          cerr<<"error: buildOmpDefaultClase() Unacceptable default option from OmpAttribute:"
            <<OmpSupport::toString(dv)<<endl;
          ROSE_ASSERT(false) ;  
        }
    }//end switch
    SgOmpDefaultClause* result = new SgOmpDefaultClause(sg_dv);
    setOneSourcePositionForTransformation(result);
    ROSE_ASSERT(result);
    return result;
  }
    
   //! Build SgOmpProcBindClause from OmpAttribute, if any
  SgOmpProcBindClause * buildOmpProcBindClause(OmpAttribute* att)
  {
    ROSE_ASSERT(att != NULL);
    if (!att->hasClause(e_proc_bind))
      return NULL;

    //grab policy
    omp_construct_enum dv = att->getProcBindPolicy();
    SgOmpClause::omp_proc_bind_policy_enum sg_dv;
    switch (dv)
    {
      case e_proc_bind_close:
        sg_dv = SgOmpClause::e_omp_proc_bind_policy_close;
        break;
      case e_proc_bind_master:
        sg_dv = SgOmpClause::e_omp_proc_bind_policy_master;
        break;
      case e_proc_bind_spread:
        sg_dv = SgOmpClause::e_omp_proc_bind_policy_spread;
        break;
      default:
        {
          cerr<<"error: buildOmpProcBindClause () Unacceptable default option from OmpAttribute:"
            <<OmpSupport::toString(dv)<<endl;
          ROSE_ASSERT(false) ;  
        }
    }//end switch
    SgOmpProcBindClause* result = new SgOmpProcBindClause(sg_dv);
    setOneSourcePositionForTransformation(result);
    ROSE_ASSERT(result);
    return result;
  }

   SgOmpAtomicClause * buildOmpAtomicClause(OmpAttribute* att)
  {
    ROSE_ASSERT(att != NULL);
    if (!att->hasClause(e_atomic_clause))
      return NULL;

    //grab value
    omp_construct_enum dv = att->getAtomicAtomicity();
    SgOmpClause::omp_atomic_clause_enum sg_dv;
    switch (dv)
    {
      case e_atomic_read:
        sg_dv = SgOmpClause::e_omp_atomic_clause_read;
        break;
      case e_atomic_write:
        sg_dv = SgOmpClause::e_omp_atomic_clause_write;
        break;
      case e_atomic_update:
        sg_dv = SgOmpClause::e_omp_atomic_clause_update;
        break;
      case e_atomic_capture:
        sg_dv = SgOmpClause::e_omp_atomic_clause_capture;
        break;
     default:
        {
          cerr<<"error: "<<__FUNCTION__ << " Unacceptable default option from OmpAttribute:"
            <<OmpSupport::toString(dv)<<endl;
          ROSE_ASSERT(false) ;  
        }
    }//end switch

    SgOmpAtomicClause* result = new SgOmpAtomicClause(sg_dv);
    setOneSourcePositionForTransformation(result);
    ROSE_ASSERT(result);
    return result;
  }
  
  // Sara Royuela ( Nov 2, 2012 ): Check for clause parameters that can be defined in macros
  // This adds support for the use of macro definitions in OpenMP clauses
  // We need a traversal over SgExpression to support macros in any position of an "assignment_expr"
  // F.i.:   #define THREADS_1 16
  //         #define THREADS_2 8
  //         int main( int arg, char** argv ) {
  //         #pragma omp parallel num_threads( THREADS_1 + THREADS_2 )
  //           {}
  //         }
  SgVarRefExpVisitor::SgVarRefExpVisitor()
        : expressions()
  {}
  
  std::vector<SgExpression*> SgVarRefExpVisitor::get_expressions()
  {
      return expressions;
  }
  
  void SgVarRefExpVisitor::visit(SgNode* node)
  {
      SgExpression* expr = isSgVarRefExp(node);
      if(expr != NULL)
      {
          expressions.push_back(expr);
      }
  }
  
  SgExpression* replace_expression_with_macro_value( std::string define_macro, SgExpression* old_exp, 
                                                     bool& macro_replaced, omp_construct_enum clause_type )
  {
      SgExpression* newExp = old_exp;
      // Parse the macro: we are only interested in macros with the form #define MACRO_NAME MACRO_VALUE, the constant macro
      size_t parenthesis = define_macro.find("(");
      if(parenthesis == string::npos)
      {   // Non function macro, constant macro
          unsigned int macroNameInitPos = (unsigned int)(define_macro.find("define")) + 6;
          while(macroNameInitPos<define_macro.size() && define_macro[macroNameInitPos]==' ')
              macroNameInitPos++;
          unsigned int macroNameEndPos = define_macro.find(" ", macroNameInitPos);
          std::string macroName = define_macro.substr(macroNameInitPos, macroNameEndPos-macroNameInitPos);
                                  
          if(macroName == isSgVarRefExp(old_exp)->get_symbol()->get_name().getString())
          {   // Clause is defined in a macro
              size_t comma = define_macro.find(",");
              if(comma == string::npos)       // Macros like "#define MACRO_NAME VALUE1, VALUE2" are not accepted
              {   // We create here an expression with the value of the clause defined in the macro
                  unsigned int macroValueInitPos = macroNameEndPos + 1;
                  while(macroValueInitPos<define_macro.size() && define_macro[macroValueInitPos]==' ')
                      macroValueInitPos++;
                  unsigned int macroValueEndPos = macroValueInitPos; 
                  while(macroValueEndPos<define_macro.size() && 
                        define_macro[macroValueEndPos]!=' ' && define_macro[macroValueEndPos]!='\n')
                      macroValueEndPos++;        
                  std::string macroValue = define_macro.substr(macroValueInitPos, macroValueEndPos-macroValueInitPos);
                  
                  // Check whether the value is a valid integer
                  std::string::const_iterator it = macroValue.begin();
                  while (it != macroValue.end() && std::isdigit(*it)) 
                      ++it;
                  ROSE_ASSERT(!macroValue.empty() && it == macroValue.end());
                  
                  newExp = buildIntVal(atoi(macroValue.c_str()));
                  if(!isSgPragmaDeclaration(old_exp->get_parent()))
                      replaceExpression(old_exp, newExp);
                  macro_replaced = true;
              }
          }
      }
      return newExp;
  }
  
  SgExpression* checkOmpExpressionClause( SgExpression* clause_expression, SgGlobal* global, omp_construct_enum clause_type )
  {
      SgExpression* newExp = clause_expression;
      // ordered (n): optional (n)
      if (clause_expression == NULL && clause_type == e_ordered_clause)
         return NULL; 
      ROSE_ASSERT(clause_expression != NULL);
      bool returnNewExpression = false;
      if( isSgTypeUnknown( clause_expression->get_type( ) ) )
      {
          SgVarRefExpVisitor v;
          v.traverse(clause_expression, preorder);
          std::vector<SgExpression*> expressions = v.get_expressions();
          if( !expressions.empty() )
          {
              if( expressions.size() == 1 )
              {   // create the new expression and return it
                  // otherwise, replace the expression and return the original, which is now modified 
                  returnNewExpression = true;
              }
              
              bool macroReplaced;
              SgDeclarationStatementPtrList& declarations = global->get_declarations();
              while( !expressions.empty() )
              {
                  macroReplaced = false;
                  SgExpression* oldExp = expressions.back();
                  for(SgDeclarationStatementPtrList::iterator declIt = declarations.begin(); declIt != declarations.end() && !macroReplaced; ++declIt) 
                  {
                      SgDeclarationStatement * declaration = *declIt;
                      AttachedPreprocessingInfoType * preprocInfo = declaration->getAttachedPreprocessingInfo();
                      if( preprocInfo != NULL )
                      {   // There is preprocessed info attached to the current node
                          for(AttachedPreprocessingInfoType::iterator infoIt = preprocInfo->begin(); 
                              infoIt != preprocInfo->end() && !macroReplaced; infoIt++)
                          {
                              if((*infoIt)->getTypeOfDirective() == PreprocessingInfo::CpreprocessorDefineDeclaration)
                              {
                                  newExp = replace_expression_with_macro_value( (*infoIt)->getString(), oldExp, macroReplaced, clause_type );
                              }
                          }
                      }
                  }
                  
                  // When a macro is defined in a header without any statement, the preprocessed information is attached to the SgFile
                  if(!macroReplaced)
                  {
                      SgProject* project = SageInterface::getProject();
                      int nFiles = project->numberOfFiles();
                      for(int fileIt=0; fileIt<nFiles && !macroReplaced; fileIt++)
                      {
                          SgFile& file = project->get_file(fileIt);
                          ROSEAttributesListContainerPtr filePreprocInfo = file.get_preprocessorDirectivesAndCommentsList();
                          if( filePreprocInfo != NULL )
                          {
                              std::map<std::string, ROSEAttributesList*> preprocInfoMap =  filePreprocInfo->getList();
                              for(std::map<std::string, ROSEAttributesList*>::iterator mapIt=preprocInfoMap.begin(); 
                                  mapIt!=preprocInfoMap.end() && !macroReplaced; mapIt++)
                              {
                                  std::vector<PreprocessingInfo*> preprocInfoList = mapIt->second->getList();
                                  for(std::vector<PreprocessingInfo*>::iterator infoIt=preprocInfoList.begin(); 
                                      infoIt!=preprocInfoList.end() && !macroReplaced; infoIt++)
                                  {
                                      if((*infoIt)->getTypeOfDirective() == PreprocessingInfo::CpreprocessorDefineDeclaration)
                                      {
                                          newExp = replace_expression_with_macro_value( (*infoIt)->getString(), oldExp, macroReplaced, clause_type );
                                      }
                                  }
                              }
                          }
                      }
                  }
                  
                  expressions.pop_back();
              }
          }
          else
          {
              printf("error in checkOmpExpressionClause(): no expression found in an expression clause\n");
              ROSE_ASSERT(false);
          }
      }
      
      return (returnNewExpression ? newExp : clause_expression);
  }

  //Build expression clauses
  SgOmpExpressionClause* buildOmpExpressionClause(OmpAttribute* att, omp_construct_enum clause_type)
  {
    ROSE_ASSERT(att != NULL);
    if (!att->hasClause(clause_type))
      return NULL;
    SgOmpExpressionClause * result = NULL ;
    
    SgGlobal* global = SageInterface::getGlobalScope( att->getNode() );
    switch (clause_type)
    {
      case e_ordered_clause:
        {
          SgExpression* param = checkOmpExpressionClause( att->getExpression(e_ordered_clause).second, global, e_ordered_clause);
          result = new SgOmpOrderedClause(param);
          break;
        }
 
      case e_collapse:
        {
          SgExpression* collapseParam = checkOmpExpressionClause( att->getExpression(e_collapse).second, global, e_collapse );
          result = new SgOmpCollapseClause(collapseParam);
          break;
        }
      case e_if:
        {
          SgExpression* ifParam = checkOmpExpressionClause( att->getExpression(e_if).second, global, e_if );
          result = new SgOmpIfClause(ifParam);
          break;
        }
      case e_num_threads:
        {
          SgExpression* numThreadsParam = checkOmpExpressionClause( att->getExpression(e_num_threads).second, global, e_num_threads );
          result = new SgOmpNumThreadsClause(numThreadsParam);
          break;
        }
      case e_device:
        {
          SgExpression* param = checkOmpExpressionClause( att->getExpression(e_device).second, global, e_device );
          result = new SgOmpDeviceClause(param);
          break;
        }
      case e_safelen:
        {
          SgExpression* param = checkOmpExpressionClause( att->getExpression(e_safelen).second, global, e_safelen );
          result = new SgOmpSafelenClause(param);
          break;
        }
       case e_simdlen:
        {
          SgExpression* param = checkOmpExpressionClause( att->getExpression(e_simdlen).second, global, e_simdlen );
          result = new SgOmpSimdlenClause(param);
          break;
        }
       case e_final:
        {
          SgExpression* Param = checkOmpExpressionClause( att->getExpression(e_final).second, global, e_final );
          result = new SgOmpFinalClause(Param);
          break;
        }
       case e_priority:
        {
          SgExpression* Param = checkOmpExpressionClause( att->getExpression(e_priority).second, global, e_priority );
          result = new SgOmpPriorityClause(Param);
          break;
        }
 
      default:
        {
          printf("error in buildOmpExpressionClause(): unacceptable clause type:%s\n",
              OmpSupport::toString(clause_type).c_str());
          ROSE_ASSERT(false);
        }
    }

    ROSE_ASSERT(result != NULL);
    setOneSourcePositionForTransformation(result);
    return result;
  }

  SgOmpNowaitClause * buildOmpNowaitClause(OmpAttribute* att)
  {
    ROSE_ASSERT(att != NULL);
    if (!att->hasClause(e_nowait))
      return NULL;
    SgOmpNowaitClause* result = new SgOmpNowaitClause();
    ROSE_ASSERT(result);
    setOneSourcePositionForTransformation(result);
    return result;
  }
  //TODO: move this builder functions to SageBuilder namespace
  SgOmpEndClause * buildOmpEndClause(OmpAttribute* att)
  {
    ROSE_ASSERT(att != NULL);
    // check if input attribute has e_end clause
    if (!att->hasClause(e_end))
      return NULL;
    SgOmpEndClause* result = new SgOmpEndClause();
    ROSE_ASSERT(result);
    setOneSourcePositionForTransformation(result);
    return result;
  }

  SgOmpBeginClause * buildOmpBeginClause(OmpAttribute* att)
  {
    ROSE_ASSERT(att != NULL);
    // check if input attribute has e_end clause
    if (!att->hasClause(e_begin))
      return NULL;
    SgOmpBeginClause* result = new SgOmpBeginClause();
    ROSE_ASSERT(result);
    setOneSourcePositionForTransformation(result);
    return result;
  }

// This becomes an expression clause since OpenMP 4.5
#if 0
  SgOmpOrderedClause * buildOmpOrderedClause(OmpAttribute* att)
  {
    ROSE_ASSERT(att != NULL);
    if (!att->hasClause(e_ordered_clause))
      return NULL;
    SgOmpOrderedClause* result = new SgOmpOrderedClause();
    ROSE_ASSERT(result);
    setOneSourcePositionForTransformation(result);
    return result;
  }
#endif
  SgOmpUntiedClause * buildOmpUntiedClause(OmpAttribute* att)
  {
    ROSE_ASSERT(att != NULL);
    if (!att->hasClause(e_untied))
      return NULL;
    SgOmpUntiedClause* result = new SgOmpUntiedClause();
    ROSE_ASSERT(result);
    setOneSourcePositionForTransformation(result);
    return result;
  }

  SgOmpMergeableClause * buildOmpMergeableClause(OmpAttribute* att)
  {
    ROSE_ASSERT(att != NULL);
    if (!att->hasClause(e_mergeable))
      return NULL;
    SgOmpMergeableClause* result = new SgOmpMergeableClause();
    ROSE_ASSERT(result);
    setOneSourcePositionForTransformation(result);
    return result;
  }


  SgOmpInbranchClause * buildOmpInbranchClause(OmpAttribute* att)
  {
    ROSE_ASSERT(att != NULL);
    if (!att->hasClause(e_inbranch))
      return NULL;
    SgOmpInbranchClause* result = new SgOmpInbranchClause();
    ROSE_ASSERT(result);
    setOneSourcePositionForTransformation(result);
    return result;
  }

  SgOmpNotinbranchClause * buildOmpNotinbranchClause(OmpAttribute* att)
  {
    ROSE_ASSERT(att != NULL);
    if (!att->hasClause(e_notinbranch))
      return NULL;
    SgOmpNotinbranchClause* result = new SgOmpNotinbranchClause();
    ROSE_ASSERT(result);
    setOneSourcePositionForTransformation(result);
    return result;
  }

  //Build SgOmpScheduleClause from OmpAttribute, if any
  SgOmpScheduleClause* buildOmpScheduleClause(OmpAttribute* att)
  {
    ROSE_ASSERT(att != NULL);
    if (!att->hasClause(e_schedule))
      return NULL;
    // convert OmpAttribute schedule kind to SgOmpClause schedule kind
    omp_construct_enum oa_kind = att->getScheduleKind();
    SgOmpClause::omp_schedule_kind_enum sg_kind;
    switch (oa_kind)
    {
      case   e_schedule_static:
        sg_kind = SgOmpClause::e_omp_schedule_static;
        break;
      case   e_schedule_dynamic:
        sg_kind = SgOmpClause::e_omp_schedule_dynamic;
        break;
      case   e_schedule_guided:
        sg_kind = SgOmpClause::e_omp_schedule_guided;
        break;
      case   e_schedule_auto:
        sg_kind = SgOmpClause::e_omp_schedule_auto;
        break;
      case   e_schedule_runtime:
        sg_kind = SgOmpClause::e_omp_schedule_runtime;
        break;
      default:
        {
          cerr<<"error: buildOmpScheduleClause() Unacceptable schedule kind from OmpAttribute:"
            <<OmpSupport::toString(oa_kind)<<endl;
          ROSE_ASSERT(false) ;  
        }
    }
    SgExpression* chunksize_exp = att->getExpression(e_schedule).second;
    // ROSE_ASSERT(chunksize_exp != NULL); // chunk size is optional
    // finally build the node
    SgOmpScheduleClause* result = new SgOmpScheduleClause(sg_kind, chunksize_exp);
    //  setOneSourcePositionForTransformation(result);
    ROSE_ASSERT(result != NULL);
    return  result;
  }

  static   SgOmpClause::omp_map_operator_enum toSgOmpClauseMapOperator(omp_construct_enum at_op)
  {
    SgOmpClause::omp_map_operator_enum result = SgOmpClause::e_omp_map_unknown;
    switch (at_op)
    {
      case e_map_tofrom: 
        {
          result = SgOmpClause::e_omp_map_tofrom;
          break;
        }
      case e_map_to: 
        {
          result = SgOmpClause::e_omp_map_to;
          break;
        }
      case e_map_from: 
        {
          result = SgOmpClause::e_omp_map_from;
          break;
        }
      case e_map_alloc: 
        {
          result = SgOmpClause::e_omp_map_alloc;
          break;
        }
      default:
        {
          printf("error: unacceptable omp construct enum for map operator conversion:%s\n", OmpSupport::toString(at_op).c_str());
          ROSE_ASSERT(false);
          break;
        }
    }
    ROSE_ASSERT(result != SgOmpClause::e_omp_map_unknown);
    return result;
  }

  //! A helper function to convert OmpAttribute reduction operator to SgClause reduction operator
  //TODO move to sageInterface?
  static   SgOmpClause::omp_reduction_operator_enum toSgOmpClauseReductionOperator(omp_construct_enum at_op)
  {
    SgOmpClause::omp_reduction_operator_enum result = SgOmpClause::e_omp_reduction_unknown;
    switch (at_op)
    {
      case e_reduction_plus: //+
        {
          result = SgOmpClause::e_omp_reduction_plus;
          break;
        }
      case e_reduction_mul:  //*
        {
          result = SgOmpClause::e_omp_reduction_mul;
          break;
        }
      case e_reduction_minus: // -
        {
          result = SgOmpClause::e_omp_reduction_minus;
          break;
        }
        // C/C++ only
      case e_reduction_bitand: // &
        {
          result = SgOmpClause::e_omp_reduction_bitand;
          break;
        }
      case e_reduction_bitor:  // |
        {
          result = SgOmpClause::e_omp_reduction_bitor;
          break;
        }
      case e_reduction_bitxor:  // ^
        {
          result = SgOmpClause::e_omp_reduction_bitxor;
          break;
        }
      case e_reduction_logand:  // &&
        {
          result = SgOmpClause::e_omp_reduction_logand;
          break;
        }
      case e_reduction_logor:   // ||
        {
          result = SgOmpClause::e_omp_reduction_logor;
          break;
        }

        // fortran operator
      case e_reduction_and: // .and.
        {
          result = SgOmpClause::e_omp_reduction_and;
          break;
        }
      case e_reduction_or: // .or.
        {
          result = SgOmpClause::e_omp_reduction_or;
          break;
        }
      case e_reduction_eqv:   // fortran .eqv.
        {
          result = SgOmpClause::e_omp_reduction_eqv;
          break;
        }
      case e_reduction_neqv:   // fortran .neqv.
        // reduction intrinsic procedure name for Fortran
        {
          result = SgOmpClause::e_omp_reduction_neqv;
          break;
        }
      case e_reduction_max:
        {
          result = SgOmpClause::e_omp_reduction_max;
          break;
        }
      case e_reduction_min:
        {
          result = SgOmpClause::e_omp_reduction_min;
          break;
        }
      case e_reduction_iand:
        {
          result = SgOmpClause::e_omp_reduction_iand;
          break;
        }
      case e_reduction_ior:
        {
          result = SgOmpClause::e_omp_reduction_ior;
          break;
        }
      case e_reduction_ieor:
        {
          result = SgOmpClause::e_omp_reduction_ieor;
          break;
        }
      default:
        {
          printf("error: unacceptable omp construct enum for reduction operator conversion:%s\n", OmpSupport::toString(at_op).c_str());
          ROSE_ASSERT(false);
          break;
        }
    }
    ROSE_ASSERT(result != SgOmpClause::e_omp_reduction_unknown);
    return result;
  }
  //A helper function to set SgVarRefExpPtrList  from OmpAttribute's construct-varlist map
  static void setClauseVariableList(SgOmpVariablesClause* target, OmpAttribute* att, omp_construct_enum key)
  {
    ROSE_ASSERT(target&&att);
    // build variable list
    std::vector<std::pair<std::string,SgNode* > > varlist = att->getVariableList(key);
#if 0  
    // Liao 6/10/2010 we relax this assertion to workaround 
    //  shared(num_threads),  a clause keyword is used as a variable 
    //  we skip variable list of shared() for now so shared clause will have empty variable list
#endif  
    ROSE_ASSERT(varlist.size()!=0);
    std::vector<std::pair<std::string,SgNode* > >::iterator iter;
    for (iter = varlist.begin(); iter!= varlist.end(); iter ++)
    {
//      cout<<"debug setClauseVariableList: " << target <<":"<<(*iter).second->class_name()  <<endl;
      // We now start to use SgExpression* to store variables showing up in a varlist
      if (SgInitializedName* iname = isSgInitializedName((*iter).second))
      {
        //target->get_variables().push_back(iname);
        // Liao 1/27/2010, fix the empty parent pointer of the SgVarRefExp here
        SgVarRefExp * var_ref = buildVarRefExp(iname);
        target->get_variables()->get_expressions().push_back(var_ref);
        var_ref->set_parent(target);
      }
      else if (SgPntrArrRefExp* aref= isSgPntrArrRefExp((*iter).second))
      {
        target->get_variables()->get_expressions().push_back(aref);
        aref->set_parent(target);
      }
      else if (SgVarRefExp* vref = isSgVarRefExp((*iter).second))
      {
        target->get_variables()->get_expressions().push_back(vref);
        vref->set_parent(target);
      }
      else
      {
          cerr<<"error: unhandled type of variable within a list:"<< ((*iter).second)->class_name();
          ROSE_ASSERT(false);
      }
    }
  }

  //! Try to build a reduction clause with a given operation type from OmpAttribute
  SgOmpReductionClause* buildOmpReductionClause(OmpAttribute* att, omp_construct_enum reduction_op)
  {
    ROSE_ASSERT(att !=NULL);
    if (!att->hasReductionOperator(reduction_op))
      return NULL;
    SgOmpClause::omp_reduction_operator_enum  sg_op = toSgOmpClauseReductionOperator(reduction_op); 
    SgExprListExp* explist=buildExprListExp();
    SgOmpReductionClause* result = new SgOmpReductionClause(explist, sg_op);
    ROSE_ASSERT(result != NULL);
    explist->set_parent(result);
    setOneSourcePositionForTransformation(result);
    
    // build variable list
    setClauseVariableList(result, att, reduction_op); 
    return result;
  }
  //! A helper function to convert OmpAttribute depend type operator to SgClause's one 
  //TODO move to sageInterface?
  static   SgOmpClause::omp_dependence_type_enum toSgOmpClauseDependenceType(omp_construct_enum at_op)
  {
    SgOmpClause::omp_dependence_type_enum result = SgOmpClause::e_omp_depend_unknown;
    switch (at_op)
    {
      case e_depend_in: 
        {
          result = SgOmpClause::e_omp_depend_in;
          break;
        }
      case e_depend_out: 
        {
          result = SgOmpClause::e_omp_depend_out;
          break;
        }
      case e_depend_inout:
        {
          result = SgOmpClause::e_omp_depend_inout;
          break;
        }
     default:
        {
          printf("error: unacceptable omp construct enum for dependence type conversion:%s\n", OmpSupport::toString(at_op).c_str());
          ROSE_ASSERT(false);
          break;
        }
    }
    ROSE_ASSERT(result != SgOmpClause::e_omp_depend_unknown);
    return result;
  }
  //! Try to build a depend clause with a given operation type from OmpAttribute
  SgOmpDependClause* buildOmpDependClause(OmpAttribute* att, omp_construct_enum dep_type)
  {
    ROSE_ASSERT(att !=NULL);
    if (!att->hasDependenceType(dep_type))
      return NULL;
    SgOmpClause::omp_dependence_type_enum  sg_op = toSgOmpClauseDependenceType(dep_type); 
    SgExprListExp* explist=buildExprListExp();
    SgOmpDependClause* result = new SgOmpDependClause(explist, sg_op);
    ROSE_ASSERT(result != NULL);
    explist->set_parent(result);
    setOneSourcePositionForTransformation(result);
    
    // build variable list
    setClauseVariableList(result, att, dep_type); 

    //this is somewhat inefficient. 
    // since the attribute has dimension info for all map clauses
    //But we don't want to move the dimension info to directive level 
    result->set_array_dimensions(att->array_dimensions);

    return result;
  }

  //! Build a map clause with a given operation type from OmpAttribute
  // map may have several variants: tofrom, to, from, and alloc. 
  // the variables for each may have dimension info 
  SgOmpMapClause* buildOmpMapClause(OmpAttribute* att, omp_construct_enum map_op)
  {
    ROSE_ASSERT(att !=NULL);
    ROSE_ASSERT (att->isMapVariant(map_op));
    if (!att->hasMapVariant(map_op))
      return NULL;
    SgOmpClause::omp_map_operator_enum  sg_op = toSgOmpClauseMapOperator(map_op); 
    SgExprListExp* explist=buildExprListExp();
    SgOmpMapClause* result = new SgOmpMapClause(explist, sg_op);
    ROSE_ASSERT(result != NULL);
    setOneSourcePositionForTransformation(result);
    explist->set_parent(result);

    // build variable list
    setClauseVariableList(result, att, map_op); 

    //this is somewhat inefficient. 
    // since the attribute has dimension info for all map clauses
    //But we don't want to move the dimension info to directive level 
    result->set_array_dimensions(att->array_dimensions);

   //A translation from OmpSupport::omp_construct_enum to SgOmpClause::omp_map_dist_data_enum is needed here.
   std::map<SgSymbol*, std::vector<std::pair<OmpSupport::omp_construct_enum, SgExpression*> > > attDistMap = att->dist_data_policies;
   std::map<SgSymbol*, std::vector<std::pair<OmpSupport::omp_construct_enum, SgExpression*> > >::iterator iter;

   std::map<SgSymbol*, std::vector<std::pair<SgOmpClause::omp_map_dist_data_enum, SgExpression*> > > convertedDistMap;
   for (iter= attDistMap.begin(); iter!=attDistMap.end(); iter++)
   {
     SgSymbol* s = (*iter).first; 
     std::vector<std::pair<OmpSupport::omp_construct_enum, SgExpression*> > src_vec = (*iter).second; 
     std::vector<std::pair<OmpSupport::omp_construct_enum, SgExpression*> >::iterator iter2;

     std::vector<std::pair<SgOmpClause::omp_map_dist_data_enum, SgExpression*> > converted_vec;
     for (iter2=src_vec.begin(); iter2!=src_vec.end(); iter2 ++ )
     {
       std::pair<OmpSupport::omp_construct_enum, SgExpression*>  src_pair = *iter2; 
       if (src_pair.first == OmpSupport::e_duplicate)
       {
         converted_vec.push_back(make_pair(SgOmpClause::e_omp_map_dist_data_duplicate, src_pair.second) );
       } else 
       if (src_pair.first == OmpSupport::e_cyclic)
       {
         converted_vec.push_back(make_pair(SgOmpClause::e_omp_map_dist_data_cyclic, src_pair.second) );
       } else 
       if (src_pair.first == OmpSupport::e_block)
       {
         converted_vec.push_back(make_pair(SgOmpClause::e_omp_map_dist_data_block, src_pair.second) );
       } else 
       {
         cerr<<"error. buildOmpMapClause() :unrecognized source dist data policy enum:"<<src_pair.first <<endl;
         ROSE_ASSERT (false);
      } // end for iter2
     } // end for iter
     convertedDistMap[s]= converted_vec;
   }
    result->set_dist_data_policies(convertedDistMap);
    return result;
  }

  //Build one of the clauses with a variable list
  SgOmpVariablesClause * buildOmpVariableClause(OmpAttribute* att, omp_construct_enum clause_type)
  {
    ROSE_ASSERT(att != NULL);
    if (!att->hasClause(clause_type))
      return NULL;
    SgOmpVariablesClause* result = NULL;  
    SgExprListExp * explist = buildExprListExp(); 
    ROSE_ASSERT(explist != NULL);
    switch (clause_type) 
    {
      case e_copyin:
        {
          result = new SgOmpCopyinClause(explist);
          break;
        }
      case e_copyprivate:
        {
          result = new SgOmpCopyprivateClause(explist);
          break;
        }
      case e_firstprivate:
        {
          result = new SgOmpFirstprivateClause(explist);
          break;
        }
      case e_lastprivate:
        {
          result = new SgOmpLastprivateClause(explist);
          break;
        }
      case e_private:
        {
          result = new SgOmpPrivateClause(explist);
          break;
        }
      case e_shared:
        {
          result = new SgOmpSharedClause(explist);
          break;
        }
     case e_linear: // TODO: need better solution for clauses with both variable list and expression. 
        { // TODO checkOmpExpressionClause() to handle macro
          SgExpression* stepExp= att->getExpression(e_linear).second;
          result = new SgOmpLinearClause(explist, stepExp);
          break;
        }
     case e_aligned:
        {
          SgExpression* alignExp= att->getExpression(e_aligned).second;
          result = new SgOmpAlignedClause(explist, alignExp);
          break;
        }
     case e_uniform:
        {
          result = new SgOmpUniformClause(explist);
          break;
        }
     case e_reduction:
        {
          printf("error: buildOmpVariableClause() does not handle reduction\n");
          ROSE_ASSERT(false);
        }
      default:
        {
          cerr<<"error: buildOmpVariableClause() Unacceptable clause type:"
            <<OmpSupport::toString(clause_type)<<endl;
          ROSE_ASSERT(false) ;  
        }
    } //end switch

    ROSE_ASSERT(result != NULL);
    explist->set_parent(result);
    //build varlist
    setClauseVariableList(result, att, clause_type);
    return result;
  }

  // Build a single SgOmpClause from OmpAttribute for type c_clause_type, excluding reduction clauses
  SgOmpClause* buildOmpNonReductionClause(OmpAttribute* att, omp_construct_enum c_clause_type)
  {
    SgOmpClause* result = NULL;
    ROSE_ASSERT(att != NULL);
    ROSE_ASSERT(isClause(c_clause_type));
    if (!att->hasClause(c_clause_type))
      return NULL;
    switch (c_clause_type) 
    {
      case e_default:
        {
          result = buildOmpDefaultClause(att); 
          break;
        }
      case e_proc_bind:
        {
          result = buildOmpProcBindClause(att); 
          break;
        }
       case e_atomic_clause:
        {
          result = buildOmpAtomicClause(att); 
          break;
        }
      case e_nowait:
        {
          result = buildOmpNowaitClause(att); 
          break;
        }
#if 0  // this becames an expression clause since OpenMP 4.5       
      case e_ordered_clause:
        {
          result = buildOmpOrderedClause(att); 
          break;
        }
#endif        
      case e_schedule:
        {
          result = buildOmpScheduleClause(att);
          break;
        }
      case e_untied:
        {
          result = buildOmpUntiedClause(att); 
          break;
        }
      case e_mergeable:
        {
          result = buildOmpMergeableClause(att); 
          break;
        }
      case e_inbranch:
        {
          result = buildOmpInbranchClause(att); 
          break;
        }
       case e_notinbranch:
        {
          result = buildOmpNotinbranchClause(att); 
          break;
        }
      case e_if:
      case e_final:
      case e_priority:
      case e_collapse:
      case e_num_threads:
      case e_device:
      case e_safelen:
      case e_simdlen:
      case e_ordered_clause:
        {
          result = buildOmpExpressionClause(att, c_clause_type);
          break;
        }
      case e_copyin:  
      case e_copyprivate:  
      case e_firstprivate:  
      case e_lastprivate:
      case e_private:
      case e_shared:
      case e_linear:
      case e_aligned:
        {
          result = buildOmpVariableClause(att, c_clause_type);
          break;
        }
     case e_reduction:
        {
          printf("error: buildOmpNonReductionClause() does not handle reduction. Please use buildOmpReductionClause().\n");
          ROSE_ASSERT(false);
          break;
        }
      case e_begin:
        {
          result = buildOmpBeginClause(att);
          break;
        }
      case e_end:
        {
          result = buildOmpEndClause(att);
          break;
        }
      default:
        {
          printf("Warning: buildOmpNoReductionClause(): unhandled clause type: %s\n", OmpSupport::toString(c_clause_type).c_str());
          ROSE_ASSERT(false);
          break;
        }

    }
    ROSE_ASSERT(result != NULL);
    setOneSourcePositionForTransformation(result);
    return result;
  }

  //! Get the affected structured block from an OmpAttribute
  SgStatement* getOpenMPBlockFromOmpAttribute (OmpAttribute* att)
  {
    SgStatement* result = NULL;
    ROSE_ASSERT(att != NULL);
    omp_construct_enum c_clause_type = att->getOmpDirectiveType();

    // Some directives have no followed statements/blocks 
    if (!isDirectiveWithBody(c_clause_type))
      return NULL;

    SgNode* snode = att-> getNode ();
    ROSE_ASSERT(snode != NULL); //? not sure for Fortran
    // Liao 10/19/2010 We convert Fortran comments into SgPragmaDeclarations
    // So we can reuse the same code to generate OpenMP AST from pragmas
#if 0     
    SgFile * file = getEnclosingFileNode (snode);
    if (file->get_Fortran_only()||file->get_F77_only()||file->get_F90_only()||
        file->get_F95_only() || file->get_F2003_only())
    { //Fortran check setNode()
      //printf("buildOmpParallelStatement() Fortran is not handled yet\n");
      //ROSE_ASSERT(false);
    }
    else // C/C++ must be pragma declaration statement
    {
      SgPragmaDeclaration* pragmadecl = att->getPragmaDeclaration();
      result = getNextStatement(pragmadecl);
    }
#endif
    SgPragmaDeclaration* pragmadecl = att->getPragmaDeclaration();
    result = getNextStatement(pragmadecl);
    // Not all pragma decl has a structured body. We check those which do have one
    // TODO: more types to be checked
    if (c_clause_type == e_task || 
        c_clause_type == e_parallel||
        c_clause_type == e_for||
        c_clause_type == e_do||
        c_clause_type == e_workshare||
        c_clause_type == e_sections||
        c_clause_type == e_section||
        c_clause_type == e_single||
        c_clause_type == e_master||
        c_clause_type == e_critical||
        c_clause_type == e_parallel_for||
        c_clause_type == e_parallel_for_simd||
        c_clause_type == e_parallel_do||
        c_clause_type == e_simd||
        c_clause_type == e_atomic
       )
    {
      ROSE_ASSERT(result!=NULL);
    }
    return result;
  }

  // a bit hack since declare simd is an outlier statement with clauses. 
  /*
     clause:
      simdlen(length)
      linear(linear-list[ : linear-step])
      aligned(argument-list[ : alignment])
      uniform(argument-list)
      inbranch
      notinbranch 
   * */
  static void appendOmpClauses(SgOmpDeclareSimdStatement* target, OmpAttribute* att)
  {
    ROSE_ASSERT(target && att);
    // must copy those clauses here, since they will be deallocated later on
    vector<omp_construct_enum> clause_vector = att->getClauses();
    std::vector<omp_construct_enum>::iterator citer;
    for (citer = clause_vector.begin(); citer != clause_vector.end(); citer++)
    {
      omp_construct_enum c_clause = *citer;
      if (!isClause(c_clause))
      {
        //      printf ("Found a construct which is not a clause:%s\n within attr:%p\n", OmpSupport::toString(c_clause).c_str(), att);
        ROSE_ASSERT(isClause(c_clause));
        continue;
      }

      SgOmpClause* result = NULL; 
      //------------------ 

      if (!att->hasClause(c_clause))
        continue; 
      switch (c_clause) 
      {
        case e_inbranch:
          {
            result = buildOmpInbranchClause(att); 
            break;
          }
        case e_notinbranch:
          {
            result = buildOmpNotinbranchClause(att); 
            break;
          }
        case e_simdlen:
          {
            result = buildOmpExpressionClause(att, c_clause);
            break;
          }
        case e_linear:
        case e_aligned:
        case e_uniform: 
          {
            result = buildOmpVariableClause(att, c_clause);
            break;
          }
        default:
          {
            printf("Warning: buildOmpNoReductionClause(): unhandled clause type: %s\n", OmpSupport::toString(c_clause).c_str());
            ROSE_ASSERT(false);
            break;
          }
      }
      ROSE_ASSERT(result != NULL);
      setOneSourcePositionForTransformation(result);

      //cout<<"push a clause "<< result->class_name() <<endl;
      target->get_clauses().push_back(result);
      result->set_parent(target); // is This right?
    }
  }


  //add clauses to target based on OmpAttribute
  static void appendOmpClauses(SgOmpClauseBodyStatement* target, OmpAttribute* att)
  {
    ROSE_ASSERT(target && att);
    // for Omp statements with clauses
    // must copy those clauses here, since they will be deallocated later on
    vector<omp_construct_enum> clause_vector = att->getClauses();
    std::vector<omp_construct_enum>::iterator citer;
    for (citer = clause_vector.begin(); citer != clause_vector.end(); citer++)
    {
      omp_construct_enum c_clause = *citer;
      if (!isClause(c_clause))
      {
        //      printf ("Found a construct which is not a clause:%s\n within attr:%p\n", OmpSupport::toString(c_clause).c_str(), att);
        ROSE_ASSERT(isClause(c_clause));
        continue;
      }
      else
      {
        // printf ("Found a clause construct:%s\n", OmpSupport::toString(c_clause).c_str());
      }
      // special handling for reduction
      if (c_clause == e_reduction) 
      {
        std::vector<omp_construct_enum> rops  = att->getReductionOperators();
        ROSE_ASSERT(rops.size()!=0);
        std::vector<omp_construct_enum>::iterator iter;
        for (iter=rops.begin(); iter!=rops.end();iter++)
        {
          omp_construct_enum rop = *iter;
          SgOmpClause* sgclause = buildOmpReductionClause(att, rop);
          target->get_clauses().push_back(sgclause);
          sgclause->set_parent(target);
        }
      }
      // special handling for depend(type:varlist)
      else if (c_clause == e_depend) 
      {
        std::vector<omp_construct_enum> rops  = att->getDependenceTypes();
        ROSE_ASSERT(rops.size()!=0);
        std::vector<omp_construct_enum>::iterator iter;
        for (iter=rops.begin(); iter!=rops.end();iter++)
        {
          omp_construct_enum rop = *iter;
          SgOmpClause* sgclause = buildOmpDependClause(att, rop);
          target->get_clauses().push_back(sgclause);
          sgclause->set_parent(target);
        }
      }
      else if (c_clause == e_map)
      {
        std::vector<omp_construct_enum> rops  = att->getMapVariants();
        ROSE_ASSERT(rops.size()!=0);
        std::vector<omp_construct_enum>::iterator iter;
        for (iter=rops.begin(); iter!=rops.end();iter++)
        {
          omp_construct_enum rop = *iter;
          SgOmpClause* sgclause = buildOmpMapClause(att, rop);
          target->get_clauses().push_back(sgclause);
          sgclause->set_parent(target);
        }
      }
      else 
      {
        SgOmpClause* sgclause = buildOmpNonReductionClause(att, c_clause);
        target->get_clauses().push_back(sgclause);
        sgclause->set_parent(target); // is This right?
      }
    }
  }

  // Directive statement builders
  //----------------------------------------------------------
  //! Build a SgOmpBodyStatement
  // handle body and optional clauses for it
  SgOmpBodyStatement * buildOmpBodyStatement(OmpAttribute* att)
  {
    SgStatement* body = getOpenMPBlockFromOmpAttribute(att);
    //Must remove the body from its previous parent first before attaching it 
    //to the new parent statement.
    // We want to keep its preprocessing information during this relocation
    // so we don't auto keep preprocessing information in its original places.
    removeStatement(body,false);

    if (body==NULL)
    {
      cerr<<"error: buildOmpBodyStatement() found empty body for "<<att->toOpenMPString()<<endl;
      ROSE_ASSERT(body != NULL);
    }
    SgOmpBodyStatement* result = NULL;
    switch (att->getOmpDirectiveType())
    {
      case e_atomic:
        result = new SgOmpAtomicStatement(NULL, body); 
        break;
      case e_critical:
        result = new SgOmpCriticalStatement(NULL, body, SgName(att->getCriticalName())); 
        break;
      case e_master:
        result = new SgOmpMasterStatement(NULL, body); 
        break;
      case e_ordered_directive:
        result = new SgOmpOrderedStatement(NULL, body); 
        break;
      case e_section:
        result = new SgOmpSectionStatement(NULL, body); 
        break;
      case e_parallel:
        result = new SgOmpParallelStatement(NULL, body); 
        break;
      case e_for:  
        result = new SgOmpForStatement(NULL, body); 
        break;
      case e_for_simd:  
        result = new SgOmpForSimdStatement(NULL, body); 
        break;
      case e_single:
        result = new SgOmpSingleStatement(NULL, body); 
        break;
      case e_sections:
        result = new SgOmpSectionsStatement(NULL, body); 
        break;
      case e_task:
        result = new SgOmpTaskStatement(NULL, body); 
        break;
      case e_target:
        result = new SgOmpTargetStatement(NULL, body); 
        ROSE_ASSERT (result != NULL);
        break;
       case e_target_data:
        result = new SgOmpTargetDataStatement(NULL, body); 
        ROSE_ASSERT (result != NULL);
        break;
      case e_simd:
        result = new SgOmpSimdStatement(NULL, body); 
        ROSE_ASSERT (result != NULL);
        break;
 
       //Fortran  
      case e_do:
        result = new SgOmpDoStatement(NULL, body); 
        break;
      case e_workshare:
        result = new SgOmpWorkshareStatement(NULL, body); 
        break;
      default:
        {
          cerr<<"error: unacceptable omp construct for buildOmpBodyStatement():"<<OmpSupport::toString(att->getOmpDirectiveType())<<endl;
          ROSE_ASSERT(false);
        }
    }
    ROSE_ASSERT(result != NULL);
    setOneSourcePositionForTransformation(result);
    copyStartFileInfo (att->getNode(), result, att); 
    copyEndFileInfo (att->getNode(), result, att); 
//    body->get_startOfConstruct()->display();
//    body->get_endOfConstruct()->display();
    //set the current parent
    body->set_parent(result);
    // add clauses for those SgOmpClauseBodyStatement
    if (isSgOmpClauseBodyStatement(result))
      appendOmpClauses(isSgOmpClauseBodyStatement(result), att);
      
   // Liao 1/9/2013, ensure the body is a basic block for some OpenMP constructs
   if (isSgOmpSingleStatement(result)) 
    ensureBasicBlockAsBodyOfOmpBodyStmt (result); 
//    result->get_file_info()->display("debug after building ..");
    return result;
  }

  SgOmpFlushStatement* buildOmpFlushStatement(OmpAttribute* att)
  {
    ROSE_ASSERT(att != NULL);
    SgOmpFlushStatement* result = new SgOmpFlushStatement();
    ROSE_ASSERT(result !=NULL);
    setOneSourcePositionForTransformation(result);
    // build variable list
    std::vector<std::pair<std::string,SgNode* > > varlist = att->getVariableList(e_flush);
    // ROSE_ASSERT(varlist.size()!=0); // can have empty variable list
    std::vector<std::pair<std::string,SgNode* > >::iterator iter;
    for (iter = varlist.begin(); iter!= varlist.end(); iter ++)
    {
      SgInitializedName* iname = isSgInitializedName((*iter).second);
      ROSE_ASSERT(iname !=NULL);
      SgVarRefExp* varref = buildVarRefExp(iname);
      result->get_variables().push_back(varref);
      varref->set_parent(result);
    }
    setOneSourcePositionForTransformation(result);
    copyStartFileInfo (att->getNode(), result, att); 
    copyEndFileInfo (att->getNode(), result, att);
    return result;
  }

  SgOmpDeclareSimdStatement* buildOmpDeclareSimdStatement(OmpAttribute* att)
  {
    ROSE_ASSERT(att != NULL);
    SgOmpDeclareSimdStatement* result = new SgOmpDeclareSimdStatement();
    result->set_firstNondefiningDeclaration(result);
    ROSE_ASSERT(result !=NULL);
    setOneSourcePositionForTransformation(result);

    appendOmpClauses(isSgOmpDeclareSimdStatement(result), att);
    setOneSourcePositionForTransformation(result);
    copyStartFileInfo (att->getNode(), result, att); 
    copyEndFileInfo (att->getNode(), result, att);
    return result;
  }


  SgOmpThreadprivateStatement* buildOmpThreadprivateStatement(OmpAttribute* att)
  {
    ROSE_ASSERT(att != NULL);
    SgOmpThreadprivateStatement* result = new SgOmpThreadprivateStatement();
    ROSE_ASSERT(result !=NULL);
    setOneSourcePositionForTransformation(result);
    // build variable list
    std::vector<std::pair<std::string,SgNode* > > varlist = att->getVariableList(e_threadprivate);
    ROSE_ASSERT(varlist.size()!=0);
    std::vector<std::pair<std::string,SgNode* > >::iterator iter;
    for (iter = varlist.begin(); iter!= varlist.end(); iter ++)
    {
      SgInitializedName* iname = isSgInitializedName((*iter).second);
      ROSE_ASSERT(iname !=NULL);
      SgVarRefExp* varref = buildVarRefExp(iname);
      result->get_variables().push_back(varref);
      varref->set_parent(result);
    }
    result->set_definingDeclaration(result);
    setOneSourcePositionForTransformation(result);
    copyStartFileInfo (att->getNode(), result, att); 
    copyEndFileInfo (att->getNode(), result, att);
    return result;
  }
  //! Build nodes for combined OpenMP directives:
  //    parallel for
  //    parallel sections
  //    parallel workshare //TODO fortran later on
  // We don't provide dedicated Sage node for combined directives, 
  // so we separate them in the AST as 1st and 2nd directive statement
  // the first is always parallel and we return it from the function
  SgOmpParallelStatement* buildOmpParallelStatementFromCombinedDirectives(OmpAttribute* att)
  {
    ROSE_ASSERT(att != NULL);
    SgStatement* body = getOpenMPBlockFromOmpAttribute(att);
    //Must remove the body from its previous parent
    removeStatement(body);
    ROSE_ASSERT(body != NULL);

    // build the 2nd directive node first
    SgStatement * second_stmt = NULL; 
    switch (att->getOmpDirectiveType())  
    {
      case e_parallel_for:
        {
          second_stmt = new SgOmpForStatement(NULL, body);
          setOneSourcePositionForTransformation(second_stmt);
          break;
        }
      case e_parallel_for_simd:
        {
          second_stmt = new SgOmpForSimdStatement(NULL, body);
          setOneSourcePositionForTransformation(second_stmt);
          break;
        }
 
      case e_parallel_sections:
        {
          second_stmt = new SgOmpSectionsStatement(NULL, body);
          setOneSourcePositionForTransformation(second_stmt); 
          break;
        }
        // Fortran
       case e_parallel_do:
        {
          second_stmt = new SgOmpDoStatement(NULL, body);
          setOneSourcePositionForTransformation(second_stmt);
          break;
        }
        case e_parallel_workshare:
        {
          second_stmt = new SgOmpWorkshareStatement(NULL, body);
          setOneSourcePositionForTransformation(second_stmt);
          break;
        }
      default:
        {
          cerr<<"error: unacceptable directive type in buildOmpParallelStatementFromCombinedDirectives(): "<<OmpSupport::toString(att->getOmpDirectiveType())<<endl;
          ROSE_ASSERT(false);
        }
    } //end switch

    ROSE_ASSERT(second_stmt);
    body->set_parent(second_stmt);
    copyStartFileInfo (att->getNode(), second_stmt, att);
    copyEndFileInfo (att->getNode(), second_stmt, att);

    // build the 1st directive node then
    SgOmpParallelStatement* first_stmt = new SgOmpParallelStatement(NULL, second_stmt); 
    setOneSourcePositionForTransformation(first_stmt);
    copyStartFileInfo (att->getNode(), first_stmt, att);
    copyEndFileInfo (att->getNode(), first_stmt, att);
    second_stmt->set_parent(first_stmt);
    ROSE_ASSERT (second_stmt->get_file_info()->get_line() == first_stmt->get_file_info()->get_line());
    // allocate clauses to them, let the 2nd one have higher priority 
    // if a clause can be allocated to either of them
    vector<omp_construct_enum> clause_vector = att->getClauses();
    std::vector<omp_construct_enum>::iterator citer;
    for (citer = clause_vector.begin(); citer != clause_vector.end(); citer++)
    {
      omp_construct_enum c_clause = *citer;
      if (!isClause(c_clause))
      {
        printf ("Found a construct which is not a clause:%s\n within attr:%p\n", OmpSupport::toString(c_clause).c_str(), att);
        ROSE_ASSERT(isClause(c_clause));
        continue;
      }
      else
      {
        // printf ("Found a clause construct:%s\n", OmpSupport::toString(c_clause).c_str());
      }

      switch (c_clause)
      {
        // clauses allocated to omp parallel
        case e_if:
        case e_num_threads:
        case e_default:
        case e_shared:
        case e_copyin:
          {
            SgOmpClause* sgclause = buildOmpNonReductionClause(att, c_clause);
            ROSE_ASSERT(sgclause != NULL);
            first_stmt->get_clauses().push_back(sgclause);
            sgclause->set_parent(first_stmt);
            break;
          }
          // unique clauses allocated to omp for  or omp for simd
        case e_schedule:
        case e_collapse:
        case e_ordered_clause:
        case e_safelen:
        case e_simdlen:
        case e_uniform:
        case e_aligned:
        case e_linear:
          {
            if (!isSgOmpForStatement(second_stmt) && !isSgOmpForSimdStatement(second_stmt) && !isSgOmpDoStatement(second_stmt))
            {
              printf("Error: buildOmpParallelStatementFromCombinedDirectives(): unacceptable clauses for parallel for/do [simd]\n");
              att->print();
              ROSE_ASSERT(false);
            }
          }
        case e_private:
        case e_firstprivate:
        case e_lastprivate:
          // case e_nowait: // nowait should not appear with combined directives
          {
            SgOmpClause* sgclause = buildOmpNonReductionClause(att, c_clause);
            ROSE_ASSERT(sgclause != NULL);
            // TODO parallel workshare 
            isSgOmpClauseBodyStatement(second_stmt)->get_clauses().push_back(sgclause);
            sgclause->set_parent(second_stmt);
            break;
          }
        case e_reduction: //special handling for reduction
          {
            std::vector<omp_construct_enum> rops  = att->getReductionOperators();
            ROSE_ASSERT(rops.size()!=0);
            std::vector<omp_construct_enum>::iterator iter;
            for (iter=rops.begin(); iter!=rops.end();iter++)
            {
              omp_construct_enum rop = *iter;
              SgOmpClause* sgclause = buildOmpReductionClause(att, rop);
              ROSE_ASSERT(sgclause != NULL);
              isSgOmpClauseBodyStatement(second_stmt)->get_clauses().push_back(sgclause);
              sgclause->set_parent(second_stmt);
            }
            break;
          }
#if 0           
        case e_map: //special handling for map , no such thing for combined parallel directives. 
          {
            std::vector<omp_construct_enum> rops  = att->getMapVariants();
            ROSE_ASSERT(rops.size()!=0);
            std::vector<omp_construct_enum>::iterator iter;
            for (iter=rops.begin(); iter!=rops.end();iter++)
            {
              omp_construct_enum rop = *iter;
              SgOmpClause* sgclause = buildOmpMapClause(att, rop);
              ROSE_ASSERT(sgclause != NULL);
              isSgOmpClauseBodyStatement(second_stmt)->get_clauses().push_back(sgclause);
           }
            break;
          }
#endif 
        default:
          {
            cerr<<"error: unacceptable clause for combined parallel for directive:"<<OmpSupport::toString(c_clause)<<endl;
            ROSE_ASSERT(false);
          }
      }
    } // end clause allocations 

    /*
       handle dangling #endif  attached to the loop
       1. original 
#ifdef _OPENMP
#pragma omp parallel for  private(i,k)
#endif 
for () ...

2. after splitting

#ifdef _OPENMP
#pragma omp parallel 
#pragma omp for  private(i,k)
#endif 
for () ...

3. We need to move #endif to omp parallel statement 's after position
transOmpParallel () will take care of it later on

#ifdef _OPENMP
#pragma omp parallel 
#pragma omp for  private(i) reduction(+ : j)
for (i = 1; i < 1000; i++)
if ((key_array[i - 1]) > (key_array[i]))
j++;
#endif
This is no perfect solution until we handle preprocessing information as structured statements in AST
*/
    movePreprocessingInfo(body, first_stmt, PreprocessingInfo::before, PreprocessingInfo::after, true);
    return first_stmt;
  }

  //! For C/C++ replace OpenMP pragma declaration with an SgOmpxxStatement
  void replaceOmpPragmaWithOmpStatement(SgPragmaDeclaration* pdecl, SgStatement* ompstmt)
  {
    ROSE_ASSERT(pdecl != NULL);
    ROSE_ASSERT(ompstmt!= NULL);

    SgScopeStatement* scope = pdecl ->get_scope();
    ROSE_ASSERT(scope !=NULL);
#if 0  
    SgOmpBodyStatement * omp_cb_stmt = isSgOmpBodyStatement(ompstmt);
    // do it within buildOmpBodyStatement()
    // avoid two parents point to the same structured block
    // optionally remove the immediate structured block
    if (omp_cb_stmt!= NULL)
    {
      SgStatement* next_stmt = getNextStatement(pdecl);
      // not true after splitting combined directives, the body becomes the 2nd directive
      // ROSE_ASSERT(next_stmt == omp_cb_stmt->get_body()); // ompstmt's body is set already
      removeStatement(next_stmt);
    }
#endif  
    // replace the pragma
    moveUpPreprocessingInfo(ompstmt, pdecl); // keep #ifdef etc attached to the pragma
    replaceStatement(pdecl, ompstmt);
  }

  //! Convert omp_pragma_list to SgOmpxxx nodes
  void convert_OpenMP_pragma_to_AST (SgSourceFile *sageFilePtr)
  {
    list<SgPragmaDeclaration* >::reverse_iterator iter; // bottom up handling for nested cases
    ROSE_ASSERT (sageFilePtr != NULL);
#if 0    
    // remove the end pragmas within Fortran. They were preserved for debugging (the conversion from comments to pragmas) purpose. 
    list<SgPragmaDeclaration* >::reverse_iterator end_iter;
    for (end_iter=omp_end_pragma_list.rbegin(); end_iter!=omp_end_pragma_list.rend(); end_iter ++)
      removeStatement (*end_iter);
#endif
    for (iter = omp_pragma_list.rbegin(); iter != omp_pragma_list.rend(); iter ++)
    {
      // Liao, 11/18/2009
      // It is possible that several source files showing up in a single compilation line
      // We have to check if the pragma declaration's file information matches the current file being processed
      // Otherwise we will process the same pragma declaration multiple times!!
      SgPragmaDeclaration* decl = *iter; 
      // Liao, 2/8/2010
      // Some pragmas are set to "transformation generated" when we fix scopes for some pragma under single statement block
      // e.g if ()
      //      #pragma
      //        do_sth()
      //  will be changed to
      //     if ()
      //     {
      //       #pragma
      //        do_sth()
      //     }
      // So we process a pragma if it is either within the same file or marked as transformation
      if (decl->get_file_info()->get_filename()!= sageFilePtr->get_file_info()->get_filename()
          && !(decl->get_file_info()->isTransformation()))
        continue;
       // Liao 10/19/2010
       // We now support OpenMP AST construction for both C/C++ and Fortran
       // But we allow Fortran End directives to exist after -rose:openmp:ast_only
       // Otherwise the code unparsed will be illegal Fortran code (No {} blocks in Fortran)
       ROSE_ASSERT(getOmpAttribute(decl) != NULL);    
       if (isFortranEndDirective(getOmpAttribute(decl)->getOmpDirectiveType()))
          continue; 
      ROSE_ASSERT (decl->get_scope() !=NULL);    
      ROSE_ASSERT (decl->get_parent() !=NULL);    
      //cout<<"debug: convert_OpenMP_pragma_to_AST() handling pragma at "<<decl<<endl;  
      //ROSE_ASSERT (decl->get_file_info()->get_filename() != string("transformation"));
      OmpAttributeList* oattlist= getOmpAttributeList(decl);
      ROSE_ASSERT (oattlist != NULL) ;
      vector <OmpAttribute* > ompattlist = oattlist->ompAttriList;
      ROSE_ASSERT (ompattlist.size() != 0) ;
      ROSE_ASSERT (ompattlist.size() == 1) ; // when do we have multiple directives associated with one pragma?

      // Liao 12/21/2015 special handling to support target begin and target end aimed for MPI code generation
      // In this case, we already use postParsingProcessing () to wrap the statements in between into a basic block after "target begin" 
      // The "target end" attribute should be ignored.
      // Skip "target end" here. 
      OmpAttribute* oa = ompattlist[0];
      ROSE_ASSERT (oa!=NULL);
      if (oa->hasClause(e_end))
      {
      // This assertion does not hold. The pragma is removed. But it is still accessible from omp_pragma_list  
      //  cerr<<"Error. unexpected target end directive is encountered in convert_OpenMP_pragma_to_AST(). It should have been removed by postParsingProcessing()."<<endl;
       // ROSE_ASSERT (false);
        //removeStatement(decl);
         continue;
      }

      vector <OmpAttribute* >::iterator i = ompattlist.begin();
      for (; i!=ompattlist.end(); i++)
      {
        OmpAttribute* oa = *i;
        omp_construct_enum omp_type = oa->getOmpDirectiveType();
        ROSE_ASSERT(isDirective(omp_type));
        SgStatement* omp_stmt = NULL;
        switch (omp_type)
        {
          // simplest OMP directives
          case e_barrier:
            {
              omp_stmt = new SgOmpBarrierStatement();
              setOneSourcePositionForTransformation(omp_stmt);
              copyStartFileInfo (oa->getNode(), omp_stmt, oa); 
              copyEndFileInfo (oa->getNode(), omp_stmt, oa);
              break;
            }
          case e_taskwait:
            {
              omp_stmt = new SgOmpTaskwaitStatement();
              setOneSourcePositionForTransformation(omp_stmt);
              copyStartFileInfo (oa->getNode(), omp_stmt, oa); 
              copyEndFileInfo (oa->getNode(), omp_stmt, oa);
              break;
            }
            // with variable list
          case e_threadprivate:
            {
              omp_stmt = buildOmpThreadprivateStatement(oa);
              break;
            }
          case e_flush:
            {
              omp_stmt = buildOmpFlushStatement(oa);
              break;
            }
          case e_declare_simd:
            {
              omp_stmt = buildOmpDeclareSimdStatement(oa);
              break;
            }
            // with a structured block/statement followed
          case e_atomic:
          case e_master:
          case e_section:
          case e_critical:
          case e_ordered_directive:
          case e_parallel:
          case e_for:
          case e_for_simd:
          case e_single:
          case e_task:
          case e_sections: 
          case e_target: // OMP-ACC directive
          case e_target_data: 
          case e_simd:  // OMP 4.0 SIMD directive
            //fortran
          case e_do:
          case e_workshare:
            {
              omp_stmt = buildOmpBodyStatement(oa);
              break;
            }
          case e_parallel_for:
          case e_parallel_for_simd:
          case e_parallel_sections:
          case e_parallel_workshare://fortran
          case e_parallel_do:
            {
              omp_stmt = buildOmpParallelStatementFromCombinedDirectives(oa);
              break;
            }
          default:
            { 
               cerr<<"Error: convert_OpenMP_pragma_to_AST(): unhandled OpenMP directive type:"<<OmpSupport::toString(omp_type)<<endl;
                assert (false);
               break;
            }
        }
        replaceOmpPragmaWithOmpStatement(decl, omp_stmt);

      } // end for (OmpAttribute)
    }// end for (omp_pragma_list)
  }

  //! A helper function to ensure a sequence statements either has only one statement
  //  or all are put under a single basic block.
  //  begin_decl is the begin directive which is immediately in front of the list of statements
  //  Return the single statement or the basic block.
  //  This function is used to wrap all statement between begin and end Fortran directives into a block,
  //  if necessary(more than one statement)
   static SgStatement * ensureSingleStmtOrBasicBlock (SgPragmaDeclaration* begin_decl, const std::vector <SgStatement*>& stmt_vec)
   {
     ROSE_ASSERT (begin_decl != NULL);
     SgStatement * result = NULL; 
     ROSE_ASSERT (stmt_vec.size() > 0);
     if (stmt_vec.size() ==1)
     {
       result = stmt_vec[0];
       ROSE_ASSERT (getNextStatement(begin_decl) == result);
     }
     else
     {  
       result = buildBasicBlock();
       // Have to remove them from their original scope first. 
       // Otherwise they will show up twice in the unparsed code: original place and under the new block
       // I tried to merge this into appendStatement() but it broke other transformations I don't want debug
       for (std::vector <SgStatement*>::const_iterator iter = stmt_vec.begin(); iter != stmt_vec.end(); iter++)
         removeStatement(*iter);
       appendStatementList (stmt_vec, isSgScopeStatement(result));
       insertStatementAfter (begin_decl, result, false);
     }
     return result; 
   }

  //! Merge clauses from end directives to the corresponding begin directives
  // dowait clause:  end do, end sections, end single, end workshare
  // copyprivate clause: end single
 void mergeEndClausesToBeginDirective (SgPragmaDeclaration* begin_decl, SgPragmaDeclaration* end_decl)
 {
   ROSE_ASSERT (begin_decl!=NULL);
   ROSE_ASSERT (end_decl!=NULL);

   // Make sure they match
   omp_construct_enum begin_type, end_type;
   begin_type = getOmpConstructEnum (begin_decl);
   end_type = getOmpConstructEnum (end_decl);
   ROSE_ASSERT (begin_type == getBeginOmpConstructEnum(end_type));

#if 0
   // Make sure they are at the same level ??
   // Fortran do loop may have wrong file info, which cause comments to be attached to another scope
   // Consequently, the end pragma will be in a higher/ different scope
   // A workaround for bug 495: https://outreach.scidac.gov/tracker/?func=detail&atid=185&aid=495&group_id=24
   if (SageInterface::is_Fortran_language() )
   {
     if (begin_decl->get_parent() != end_decl->get_parent())
     {
       ROSE_ASSERT (isAncestor (end_decl->get_parent(), begin_decl->get_parent()));
     }
   }
   else  
#endif     
    ROSE_ASSERT (begin_decl->get_parent() == end_decl->get_parent()); 

   // merge end directive's clause to the begin directive.
   OmpAttribute* begin_att = getOmpAttribute (begin_decl); 
   OmpAttribute* end_att = getOmpAttribute (end_decl); 

   // Merge possible nowait clause
   switch (end_type)
   {
     case e_end_do:
     case e_end_sections:
     case e_end_single:
     case e_end_workshare:
       {
         if (end_att->hasClause(e_nowait))
         {
           begin_att->addClause(e_nowait);
         }
         break;
       }
     default:
       break; // there should be no clause for other cases
   }
   // Merge possible copyrpivate (list) from end single
   if ((end_type == e_end_single) && end_att ->hasClause(e_copyprivate)) 
   {
     begin_att->addClause (e_copyprivate);
     std::vector<std::pair<std::string,SgNode* > > varList = end_att->getVariableList(e_copyprivate);
     std::vector<std::pair<std::string,SgNode* > >::iterator iter;
     for (iter = varList.begin(); iter != varList.end(); iter++)
     {
       std::pair<std::string,SgNode* > element = *iter;
       SgInitializedName* i_name = isSgInitializedName(element.second);
       ROSE_ASSERT (i_name != NULL);
       begin_att->addVariable(e_copyprivate, element.first, i_name);
     }
   }  
 }
  //! This function will Find a (optional) end pragma for an input pragma (decl)
  //  and merge clauses from the end pragma to the beginning pragma
  //  statements in between will be put into a basic block if there are more than one statements
  void merge_Matching_Fortran_Pragma_pairs(SgPragmaDeclaration* decl, omp_construct_enum omp_type)
  {
    ROSE_ASSERT (getOmpConstructEnum(decl)== omp_type );
    ROSE_ASSERT (isFortranBeginDirective(omp_type));
    omp_construct_enum end_omp_type = getEndOmpConstructEnum(omp_type);
    SgPragmaDeclaration* end_decl = NULL; 
    SgStatement* next_stmt = getNextStatement(decl);
  //  SgStatement* prev_stmt = decl;
  //  SgBasicBlock* func_body = getEnclosingProcedure (decl)->get_body();
  //  ROSE_ASSERT (func_body != NULL);

    std::vector<SgStatement*> affected_stmts; // statements which are inside the begin .. end pair

    // Find possible end directives attached to a pragma declaration
    while (next_stmt!= NULL)
    {
      end_decl = isSgPragmaDeclaration (next_stmt);
      if ((end_decl) && (getOmpConstructEnum(end_decl) == end_omp_type))
        break;
      else
        end_decl = NULL; // MUST reset to NULL if not a match
      affected_stmts.push_back(next_stmt);
   //   prev_stmt = next_stmt; // save previous statement
      next_stmt = getNextStatement (next_stmt);
#if 0
      // Liao 1/21/2011
      // A workaround of wrong file info for Do loop body
      // See bug 495 https://outreach.scidac.gov/tracker/?func=detail&atid=185&aid=495&group_id=24
      // Comments will not be attached before ENDDO, but some parent located node instead.
      // SageInterface::getNextStatement() will not climb out current scope and find a matching end directive attached to a parent node.
      //
      // For example 
      //        do i = 1, 10
      //     !$omp task 
      //        call process(item(i))
      //     !$omp end task
      //          enddo
      // The !$omp end task comments will not be attached before ENDDO , but inside SgBasicBlock, which is an ancestor node  
     if (SageInterface::is_Fortran_language() )
     {
      // try to climb up one statement level, until reaching the function body
       SgStatement* parent_stmt  = getEnclosingStatement(prev_stmt->get_parent());
       // getNextStatement() cannot take SgFortranDo's body as input (the body is not a child of its scope's declaration list)
       // So we climb up to the parent do loop
       if (isSgFortranDo(parent_stmt->get_parent()))  
         parent_stmt = isSgFortranDo(parent_stmt->get_parent());
       else if (isSgWhileStmt(parent_stmt->get_parent()))  
         parent_stmt = isSgWhileStmt(parent_stmt->get_parent());

       if (parent_stmt != func_body) 
         next_stmt = getNextStatement (parent_stmt);
     }
#endif     
    }  // end while

    // mandatory end directives for most begin directives, except for two cases:
    // !$omp end do
    // !$omp end parallel do
    if (end_decl == NULL) 
    {
      if ((end_omp_type!=e_end_do) &&  (end_omp_type!=e_end_parallel_do))
      {
        cerr<<"merge_Matching_Fortran_Pragma_pairs(): cannot find required end directive for: "<< endl;
        cerr<<decl->get_pragma()->get_pragma()<<endl;
        ROSE_ASSERT (false);
      }
      else 
        return; // There is nothing further to do if the optional end directives do not exist
    } // end if sanity check

    // at this point, we have found a matching end directive/pragma
    ROSE_ASSERT (end_decl);
    ensureSingleStmtOrBasicBlock(decl, affected_stmts);
    mergeEndClausesToBeginDirective (decl,end_decl);
    // SgBasicBlock is not unparsed in Fortran 
    //
    // To ensure the unparsed Fortran code is correct for debugging -rose:openmp:ast_only
    //  after converting Fortran comments to Pragmas. 
    // x.  We should not tweak the original text for the pragmas. 
    // x.  We should not remove the end pragma declaration since SgBasicBlock is not unparsed.
    // In the end , the pragmas don't matter too much, the OmpAttributes attached to them 
    // are used to guide translations. 
     removeStatement(end_decl);
    // we should save those useless end pragmas to a list
    // and remove them as one of the first steps in OpenMP lowering for Fortran
    // omp_end_pragma_list.push_back(end_decl); 
  } // end merge_Matching_Fortran_Pragma_pairs()
  
  //! This function will 
  //   x. Find matching OpenMP directive pairs
  //      an inside out order is used to handle nested regions
  //   x. Put statements in between into a basic block
  //   x. Merge clauses from the ending directive to the beginning directives
  //  The result is an Fortran OpenMP AST with C/C++ pragmas
  //  so we can simply reuse convert_OpenMP_pragma_to_AST() to generate 
  //  OpenMP AST nodes for Fortran programs
  void convert_Fortran_Pragma_Pairs (SgSourceFile *sageFilePtr)
  {
    ROSE_ASSERT (sageFilePtr != NULL);
    list<SgPragmaDeclaration* >::reverse_iterator iter; // bottom up handling for nested cases
    for (iter = omp_pragma_list.rbegin(); iter != omp_pragma_list.rend(); iter ++)
    {
      // It is possible that several source files showing up in a single compilation line
      // We have to check if the pragma declaration's file information matches the current file being processed
      // Otherwise we will process the same pragma declaration multiple times!!
      SgPragmaDeclaration* decl = *iter;
      // Some pragmas are set to "transformation generated" when we fix scopes for some pragma under single statement block
      // e.g if ()
      //      #pragma
      //        do_sth()
      //  will be changed to
      //     if ()
      //     {
      //       #pragma
      //        do_sth()
      //     }
      // So we process a pragma if it is either within the same file or marked as transformation
      if (decl->get_file_info()->get_filename()!= sageFilePtr->get_file_info()->get_filename()
          && !(decl->get_file_info()->isTransformation()))
        continue;
      omp_construct_enum omp_type = getOmpConstructEnum(decl);
#if 0     
      // skip if the construct is Fortran end directive of any kinds
      // since we always start the conversion from a begin directive
      if (isFortranEndDirective(omp_type))
        continue;
#endif         
      // Now we  
       if (isFortranBeginDirective(omp_type)) 
         merge_Matching_Fortran_Pragma_pairs (decl, omp_type);
    } // end for omp_pragma_list

  } // end convert_Fortran_Pragma_Pairs()

  //! Convert OpenMP Fortran comments to pragmas
  //  main purpose is to 
  //     x. Generate pragmas from OmpAttributes and insert them into the right places
  //        since the floating comments are very difficult to work with
  //        we move them to the fake pragmas to ease later translations. 
  //        The backend has been extended to unparse the pragma in order to debug this step.
  //     x. Enclose affected Fortran statement into a basic block
  //     x. Merge clauses from END directives to the begin directive
  // This will temporarily introduce C/C++-like AST with pragmas attaching OmpAttributes.
  // This should be fine since we have SgBasicBlock in Fortran AST also.
  //
  // The benefit is that pragma-to-AST conversion written for C/C++ can 
  // be reused for Fortran after this pass.
  // Liao 10/18/2010
  void convert_Fortran_OMP_Comments_to_Pragmas (SgSourceFile *sageFilePtr)
  {
    // We reuse the pragma list for C/C++ here
    //ROSE_ASSERT  (omp_pragma_list.size() ==0);
    ROSE_ASSERT (sageFilePtr != NULL);
    // step 1: Each OmpAttribute will have a dedicated SgPragmaDeclaration for it
    list <OmpAttribute *>::iterator iter; 
    for (iter = omp_comment_list.begin(); iter != omp_comment_list.end(); iter ++)
    {
      OmpAttribute * att = *iter;
      ROSE_ASSERT (att->getNode() !=NULL);
      ROSE_ASSERT (att->getPreprocessingInfo() !=NULL);
      //cout<<"debug omp attribute @"<<att<<endl;
      SgStatement* stmt = isSgStatement(att->getNode());
      // TODO verify this assertion is true for Fortran OpenMP comments
      ROSE_ASSERT (stmt != NULL);
      //cout<<"debug at ompAstConstruction.cpp:"<<stmt <<" " << stmt->getAttachedPreprocessingInfo ()->size() <<endl;
      ROSE_ASSERT (stmt->getAttachedPreprocessingInfo ()->size() != 0);
      // So we process the directive if it's anchor node is either within the same file or marked as transformation
      if (stmt->get_file_info()->get_filename()!= sageFilePtr->get_file_info()->get_filename()
          && !(stmt->get_file_info()->isTransformation()))
        continue;
      SgScopeStatement * scope = stmt->get_scope();
      ROSE_ASSERT (scope != NULL);
      // the pragma will have string to ease debugging
      string p_name = att->toOpenMPString();
      SgPragmaDeclaration * p_decl = buildPragmaDeclaration("omp "+ p_name, scope);
      //preserve the original source file info ,TODO complex cases , use real preprocessing info's line information !!
      copyStartFileInfo (att->getNode(), p_decl, att);
      omp_pragma_list.push_back(p_decl);

      // move the attribute to the pragma
      //  remove the attribute from the original statement 's OmpAttributeList
      removeOmpAttribute(att, stmt);
      //cout<<"debug at after removeOmpAttribute:"<<stmt <<" " << stmt->getAttachedPreprocessingInfo ()->size() <<endl;
      // remove the getPreprocessingInfo also 
      PreprocessingInfo* info = att->getPreprocessingInfo(); 
      ROSE_ASSERT (info != NULL);
      // We still keep the peprocessingInfo. its line number will be used later to set file info object
      //att->setPreprocessingInfo(NULL);// set this as if the OmpAttribute was from a pragma, not from a comment
      AttachedPreprocessingInfoType *comments = stmt ->getAttachedPreprocessingInfo ();
      ROSE_ASSERT (comments != NULL);
      ROSE_ASSERT (comments->size() !=0);
      AttachedPreprocessingInfoType::iterator m_pos = find (comments->begin(), comments->end(), info);
      if (m_pos == comments->end())
      {
        cerr<<"Cannot find a Fortran comment from a node: "<<endl;
        cerr<<"The comment is "<<info->getString()<<endl;
        cerr<<"The AST Node is "<<stmt->class_name()<<endl;
        stmt->get_file_info()->display("debug here");
        AttachedPreprocessingInfoType::iterator i;
        for (i = comments->begin(); i!= comments->end(); i++)
        {
          cerr<<(*i)->getString()<<endl;
        }
        //cerr<<"The AST Node is at line:"<<stmt->get_file_info().get_line()<<endl;
        ROSE_ASSERT (m_pos != comments->end());
      }
      comments->erase (m_pos);
      att->setNode(p_decl);
      addOmpAttribute(att, p_decl); 

      //cout<<"debug at after addOmpAttribute:"<<stmt <<" " << stmt->getAttachedPreprocessingInfo ()->size() <<endl;
      // two cases for where to insert the pragma, depending on where the preprocessing info is attached to stmt
      //  1. PreprocessingInfo::before
      //     insert the pragma right before the original Fortran statement
      //  2. PreprocessingInfo::inside
      //      insert it as the last statement within stmt
      PreprocessingInfo::RelativePositionType position = info->getRelativePosition ();      
      if (position == PreprocessingInfo::before)
      { 
        // Don't automatically move comments here!
        if (isSgBasicBlock(stmt) && isSgFortranDo (stmt->get_parent()))
        {// special handling for the body of SgFortranDo.  The comments will be attached before the body
         // But we cannot insert the pragma before the body. So we prepend it into the body instead
          prependStatement(p_decl, isSgBasicBlock(stmt));
        }
        else
          insertStatementBefore (stmt, p_decl, false);
      }
      else if (position == PreprocessingInfo::inside)
      {
        SgScopeStatement* scope = isSgScopeStatement(stmt);
        ROSE_ASSERT (scope != NULL);
        appendStatement(p_decl, scope);
      }
      else if (position == PreprocessingInfo::after)
      {
        insertStatementAfter(stmt, p_decl, false);
      }
      else
      {
        cerr<<"ompAstConstruction.cpp , illegal PreprocessingInfo::RelativePositionType:"<<position<<endl;
        ROSE_ASSERT (false);
      }
      //cout<<"debug at after appendStmt:"<<stmt <<" " << stmt->getAttachedPreprocessingInfo ()->size() <<endl;
    } // end for omp_comment_list

    convert_Fortran_Pragma_Pairs(sageFilePtr);
  } // end convert_Fortran_OMP_Comments_to_Pragmas ()


  void build_OpenMP_AST(SgSourceFile *sageFilePtr)
  {
    // build AST for OpenMP directives and clauses 
    // by converting OmpAttributeList to SgOmpxxx Nodes 
    if (sageFilePtr->get_Fortran_only()||sageFilePtr->get_F77_only()||sageFilePtr->get_F90_only()||
        sageFilePtr->get_F95_only() || sageFilePtr->get_F2003_only())
    {
      convert_Fortran_OMP_Comments_to_Pragmas (sageFilePtr);
      // end if (fortran)
    }
    else
    {
      // for  C/C++ pragma's OmpAttributeList --> SgOmpxxx nodes
      if (SgProject::get_verbose() > 1)
      {
        printf ("Calling convert_OpenMP_pragma_to_AST() \n");
      }
    }
    // We can turn this off to debug the convert_Fortran_OMP_Comments_to_Pragmas()
    convert_OpenMP_pragma_to_AST( sageFilePtr);
  }

  // Liao 12/21/2015 special handling to support target begin and target end aimed for MPI code generation
  // In this case, we already use postParsingProcessing () to wrap the statements in between into a basic block after "target begin" 
  // The "target end" attribute should be ignored.
  // Skip "target end" here. 
  void postParsingProcessing (SgSourceFile *sageFilePtr)
  {
    // This experimental support should only happen to C/C++ code for now
    if (sageFilePtr->get_Fortran_only()||sageFilePtr->get_F77_only()||sageFilePtr->get_F90_only()||
        sageFilePtr->get_F95_only() || sageFilePtr->get_F2003_only())
    {
      return; 
    }

    list<SgPragmaDeclaration* >::reverse_iterator iter; // bottom up handling for nested cases
    ROSE_ASSERT (sageFilePtr != NULL);
   for (iter = omp_pragma_list.rbegin(); iter != omp_pragma_list.rend(); iter ++)
    {
      // Liao, 11/18/2009
      // It is possible that several source files showing up in a single compilation line
      // We have to check if the pragma declaration's file information matches the current file being processed
      // Otherwise we will process the same pragma declaration multiple times!!
      SgPragmaDeclaration* decl = *iter;
      // Liao, 2/8/2010
      // Some pragmas are set to "transformation generated" when we fix scopes for some pragma under single statement block
      // e.g if ()
      //      #pragma
      //        do_sth()
      //  will be changed to
      //     if ()
      //     {
      //       #pragma
      //        do_sth()
      //     }
      // So we process a pragma if it is either within the same file or marked as transformation
      if (decl->get_file_info()->get_filename()!= sageFilePtr->get_file_info()->get_filename()
          && !(decl->get_file_info()->isTransformation()))
        continue;
       // Liao 10/19/2010
       // We now support OpenMP AST construction for both C/C++ and Fortran
       // But we allow Fortran End directives to exist after -rose:openmp:ast_only
       // Otherwise the code unparsed will be illegal Fortran code (No {} blocks in Fortran)
      // if (isFortranEndDirective(getOmpAttribute(decl)->getOmpDirectiveType()))
       //    continue;
      ROSE_ASSERT (decl->get_scope() !=NULL);
      ROSE_ASSERT (decl->get_parent() !=NULL);
      //cout<<"debug: convert_OpenMP_pragma_to_AST() handling pragma at "<<decl<<endl;  
      //ROSE_ASSERT (decl->get_file_info()->get_filename() != string("transformation"));
      OmpAttributeList* oattlist= getOmpAttributeList(decl);                                                                               
      ROSE_ASSERT (oattlist != NULL) ;                                                                                                     
      vector <OmpAttribute* > ompattlist = oattlist->ompAttriList;                                                                         
      ROSE_ASSERT (ompattlist.size() != 0) ;                                                                                               
      ROSE_ASSERT (ompattlist.size() == 1) ; // when do we have multiple directives associated with one pragma?                            
                                                                                                                                           
      // Liao 12/21/2015 special handling to support target begin and target end aimed for MPI code generation                             
      // In this case, we already use postParsingProcessing () to wrap the statements in between into a basic block after "target begin"   
      // The "target end" attribute should be ignored.                                                                                     
      // Skip "target end" here.                                                                                                           
      // find omp target begin
      OmpAttribute* oa = getOmpAttribute (decl);
      ROSE_ASSERT (oa != NULL);
      omp_construct_enum omp_type = oa->getOmpDirectiveType();
      //   The first attribute should always be the directive type
      ROSE_ASSERT(isDirective(omp_type));
      
      if ( omp_type == e_target && oa->hasClause(e_begin)) 
      { 
        // find the matching end decl with "target end" attribute
        SgPragmaDeclaration* end_decl = NULL; 
        SgStatement* next_stmt = getNextStatement(decl);
        std::vector<SgStatement*> affected_stmts; // statements which are inside the begin .. end pair
        while (next_stmt!= NULL)
        {
          end_decl = isSgPragmaDeclaration (next_stmt);
          if (end_decl)  // candidate pragma declaration
           if (getOmpConstructEnum(end_decl) == e_target) // It is target directive
           {
             OmpAttribute* oa2 = getOmpAttribute (end_decl);
             ROSE_ASSERT (oa2 != NULL);
             if (oa2->hasClause (e_end))
                break; // found  the matching target end, break out the while loop
           }

          // No match found yet? store the current stmt into the affected stmt list
          end_decl = NULL; // MUST reset to NULL if not a match
          affected_stmts.push_back(next_stmt);
          //   prev_stmt = next_stmt; // save previous statement
          next_stmt = getNextStatement (next_stmt);
        } // end while  

        if (end_decl == NULL) 
        {
          cerr<<"postParsingProcessing(): cannot find required end directive for: "<< endl;
          cerr<<decl->get_pragma()->get_pragma()<<endl;
          ROSE_ASSERT (false);
        } // end if sanity check

        //at this point, we have found a matching end directive/pragma
        ROSE_ASSERT (end_decl);
        ensureSingleStmtOrBasicBlock(decl, affected_stmts);
        removeStatement(end_decl);

      } // end if "target begin"                          
    } // end for 
  } // end postParsingProcessing()

  // Liao, 5/31/2009 an entry point for OpenMP related processing
  // including parsing, AST construction, and later on translation
  void processOpenMP(SgSourceFile *sageFilePtr)
  {
    // DQ (4/4/2010): This function processes both C/C++ and Fortran code.
    // As a result of the Fortran processing some OMP pragmas will cause
    // transformation (e.g. declaration of private variables will add variables
    // to the local scope).  So this function has side-effects for all languages.

    if (SgProject::get_verbose() > 1)
    {
      printf ("Processing OpenMP directives \n");
    }

    ROSE_ASSERT(sageFilePtr != NULL);
    if (sageFilePtr->get_openmp() == false)
    {
      if (SgProject::get_verbose() > 1)
      {
        printf ("Skipping calls to lower OpenMP sageFilePtr->get_openmp() = %s \n",sageFilePtr->get_openmp() ? "true" : "false");
      }
      return;
    }

    // parse OpenMP directives and attach OmpAttributeList to relevant SgNode
    attachOmpAttributeInfo(sageFilePtr);

    // Additional processing of the AST after parsing
    // 
    postParsingProcessing (sageFilePtr);

    // stop here if only OpenMP parsing is requested
    if (sageFilePtr->get_openmp_parse_only())
    {
      if (SgProject::get_verbose() > 1)
      {
        printf ("Skipping calls to lower OpenMP sageFilePtr->get_openmp_parse_only() = %s \n",sageFilePtr->get_openmp_parse_only() ? "true" : "false");
      }
      return;
    }

    // Build OpenMP AST nodes based on parsing results
    build_OpenMP_AST(sageFilePtr);

    // stop here if only OpenMP AST construction is requested
    if (sageFilePtr->get_openmp_ast_only())
    {
      if (SgProject::get_verbose() > 1)
      {
        printf ("Skipping calls to lower OpenMP sageFilePtr->get_openmp_ast_only() = %s \n",sageFilePtr->get_openmp_ast_only() ? "true" : "false");
      }
      return;
    }

    lower_omp(sageFilePtr);
  }

} // end of the namespace
