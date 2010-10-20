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


namespace OmpSupport
{ 
  // an internal data structure to avoid redundant AST traversal to find OpenMP pragmas
  static std::list<SgPragmaDeclaration* > omp_pragma_list; 

  // a similar list to save encountered Fortran comments which are OpenMP directives
  std::list<OmpAttribute* > omp_comment_list; 
  // A pragma list to store the dangling pragmas for Fortran end directives. 
  // There are stored to ensure correct unparsing using -rose:openmp:ast_only
  // But they should be immediately removed during the OpenMP lowering phase
  static std::list<SgPragmaDeclaration* > omp_end_pragma_list; 

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
            omp_parser_init(pragmaDeclaration,pragmaString.c_str());
            omp_parse();
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

  //Build if clause
  SgOmpExpressionClause* buildOmpExpressionClause(OmpAttribute* att, omp_construct_enum clause_type)
  {
    ROSE_ASSERT(att != NULL);
    if (!att->hasClause(clause_type)) 
      return NULL;
    SgOmpExpressionClause * result = NULL ;   
    switch (clause_type)
    {
      case e_collapse:
        {
          ROSE_ASSERT(att->getExpression(e_collapse).second!=NULL);

          result = new SgOmpCollapseClause(att->getExpression(e_collapse).second);
          break;
        }
      case e_if:
        {
          ROSE_ASSERT(att->getExpression(e_if).second!=NULL);
          result = new SgOmpIfClause(att->getExpression(e_if).second);
          break;
        }
      case e_num_threads:
        {
          ROSE_ASSERT(att->getExpression(e_num_threads).second!=NULL);
          result = new SgOmpNumThreadsClause(att->getExpression(e_num_threads).second);
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

  //! A helper function to convert OmpAttribute reduction operator to SgClause reduction operator
  //TODO move to sageInterface?
  static   SgOmpClause::omp_reduction_operator_enum toSgOmpClauseReductionOperator(omp_construct_enum at_op)
  {
    SgOmpClause::omp_reduction_operator_enum result = SgOmpClause::e_omp_reduction_unkown;
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
    ROSE_ASSERT(result != SgOmpClause::e_omp_reduction_unkown);
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
      SgInitializedName* iname = isSgInitializedName((*iter).second);
      ROSE_ASSERT(iname !=NULL);
      //target->get_variables().push_back(iname);
      target->get_variables().push_back(buildVarRefExp(iname));
    }
  }

  //! Try to build a reduction clause with a given operation type from OmpAttribute
  SgOmpReductionClause* buildOmpReductionClause(OmpAttribute* att, omp_construct_enum reduction_op)
  {
    ROSE_ASSERT(att !=NULL);
    if (!att->hasReductionOperator(reduction_op))
      return NULL;
    SgOmpClause::omp_reduction_operator_enum  sg_op = toSgOmpClauseReductionOperator(reduction_op); 
    SgOmpReductionClause* result = new SgOmpReductionClause(sg_op);
    setOneSourcePositionForTransformation(result);
    ROSE_ASSERT(result != NULL);

    // build variable list
    setClauseVariableList(result, att, reduction_op); 
    return result;
  }

  //Build one of the clauses with a variable list
  SgOmpVariablesClause * buildOmpVariableClause(OmpAttribute* att, omp_construct_enum clause_type)
  {
    ROSE_ASSERT(att != NULL);
    if (!att->hasClause(clause_type))
      return NULL;
    SgOmpVariablesClause* result = NULL;  
    switch (clause_type) 
    {
      case e_copyin:
        {
          result = new SgOmpCopyinClause();
          break;
        }
      case e_copyprivate:
        {
          result = new SgOmpCopyprivateClause();
          break;
        }
      case e_firstprivate:
        {
          result = new SgOmpFirstprivateClause();
          break;
        }
      case e_lastprivate:
        {
          result = new SgOmpLastprivateClause();
          break;
        }
      case e_private:
        {
          result = new SgOmpPrivateClause();
          break;
        }
      case e_shared:
        {
          result = new SgOmpSharedClause();
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

    //build varlist
    ROSE_ASSERT(result != NULL);
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
      case e_nowait:
        {
          result = buildOmpNowaitClause(att); 
          break;
        }
      case e_ordered_clause:
        {
          result = buildOmpOrderedClause(att); 
          break;
        }
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
      case e_if:
      case e_collapse:
      case e_num_threads:
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
  SgStatement* getOpenMPBlockFromOmpAttribte (OmpAttribute* att)
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
    //  ROSE_ASSERT(result!=NULL);
    // Not all pragma decl has a structured body!!
    return result;
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
    SgStatement* body = getOpenMPBlockFromOmpAttribte(att);
    //Must remove the body from its previous parent
    removeStatement(body);

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
      case e_single:
        result = new SgOmpSingleStatement(NULL, body); 
        break;
      case e_sections:
        result = new SgOmpSectionsStatement(NULL, body); 
        break;
      case e_task:
        result = new SgOmpTaskStatement(NULL, body); 
        // cout<<"Debug:sageSupport.C Found an OmpAttribute from a task pragma"<<endl;
        break;
      default:
        {
          cerr<<"error: unacceptable omp construct for buildOmpBodyStatement():"<<OmpSupport::toString(att->getOmpDirectiveType())<<endl;
          ROSE_ASSERT(false);
        }
    }
    ROSE_ASSERT(result != NULL);
    setOneSourcePositionForTransformation(result);
    //set the current parent
    body->set_parent(result);
    // add clauses for those SgOmpClauseBodyStatement
    if (isSgOmpClauseBodyStatement(result))
      appendOmpClauses(isSgOmpClauseBodyStatement(result), att);
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
      result->get_variables().push_back(buildVarRefExp(iname));
    }
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
      result->get_variables().push_back(buildVarRefExp(iname));
    }
    result->set_definingDeclaration(result);
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
    SgStatement* body = getOpenMPBlockFromOmpAttribte(att);
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
      case e_parallel_sections:
        {
          second_stmt = new SgOmpSectionsStatement(NULL, body);
          setOneSourcePositionForTransformation(second_stmt); 
          break;
        }
        //   case e_parallel_workshare: //TODO fortran
      default:
        {
          cerr<<"error: unacceptable directive type in buildOmpParallelStatementFromCombinedDirectives(): "<<OmpSupport::toString(att->getOmpDirectiveType())<<endl;
          ROSE_ASSERT(false);
        }
    } //end switch

    ROSE_ASSERT(second_stmt);
    body->set_parent(second_stmt);

    // build the 1st directive node then
    SgOmpParallelStatement* first_stmt = new SgOmpParallelStatement(NULL, second_stmt); 
    setOneSourcePositionForTransformation(first_stmt);
    second_stmt->set_parent(first_stmt);

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
            break;
          }
          // unique clauses allocated to omp for
        case e_schedule:
        case e_collapse:
        case e_ordered_clause:
          {
            if (!isSgOmpForStatement(second_stmt))
            {
              printf("error: unacceptable clauses in parallel for\n");
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
            }
            break;
          }
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

  //! for C/C++ replace OpenMP pragma declaration with an SgOmpxxStatement
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
       if (isFortranEndDirective(getOmpAttribute(decl)->getOmpDirectiveType()))
          continue; 
      //cout<<"debug: convert_OpenMP_pragma_to_AST() handling pragma at "<<decl<<endl;  
      OmpAttributeList* oattlist= getOmpAttributeList(decl);
      ROSE_ASSERT (oattlist != NULL) ;
      vector <OmpAttribute* > ompattlist = oattlist->ompAttriList;
      ROSE_ASSERT (ompattlist.size() != 0) ;
      ROSE_ASSERT (ompattlist.size() == 1) ; // when do we have multiple directives associated with one pragma?
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
              break;
            }
          case e_taskwait:
            {
              omp_stmt = new SgOmpTaskwaitStatement();
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
            // with a structured block/statement followed
          case e_atomic:
          case e_master:
          case e_section:
          case e_critical:
          case e_ordered_directive:
            // case e_workshare;// TODO fortran
          case e_parallel:
          case e_for:
          case e_single:
          case e_task:
          case e_sections: 
            {
              omp_stmt = buildOmpBodyStatement(oa);
              break;
            }
          case e_parallel_for:
          case e_parallel_sections:
            //case e_parallel_workshare://TODO fortran
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
        setOneSourcePositionForTransformation(omp_stmt);
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

   // Make sure they are at the same level ??
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

    std::vector<SgStatement*> affected_stmts; // statements which are inside the begin .. end pair

    // Find possible end directives attached to a pragma declaration
    while (next_stmt!= NULL)
    {
      end_decl = isSgPragmaDeclaration (next_stmt);
      if ((end_decl) && (getOmpConstructEnum(end_decl) == end_omp_type))
        break;
      affected_stmts.push_back(next_stmt);
      next_stmt = getNextStatement (next_stmt);
    }  

    // mandatory end directives for most begin directives, except for two cases:
    // !$omp end do
    // !$omp end parallel do
    if (end_decl == NULL) 
    {
      if ((end_omp_type!=e_end_do) &&  (end_omp_type!=e_end_parallel_do))
      {
        cerr<<"merge_Matching_Fortran_Pragma_pairs(): cannot find required end directive for: "<< endl;
        cerr<<decl->unparseToString()<<endl;
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
    // To ensure the unparsed Fortran code is correct for -rose:openmp:ast_only
    // x.  We should not tweak the original text for the pragmas. 
    // x.  We should not remove the end pragma declaration since SgBasicBlock is not unparsed.
    // In the end , the pragmas don't matter too much, the OmpAttributes attached to them 
    // are used to guide translations. 
    // removeStatement(end_decl);
    // we should save those useless end pragmas to a list
    // and remove them as one of the first steps in OpenMP lowering for Fortran
    omp_end_pragma_list.push_back(end_decl); 
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
      omp_pragma_list.push_back(p_decl);

      // move the attribute to the pragma
      //  remove the attribute from the original statement 's OmpAttributeList
      removeOmpAttribute(att, stmt);
      //cout<<"debug at after removeOmpAttribute:"<<stmt <<" " << stmt->getAttachedPreprocessingInfo ()->size() <<endl;
      // remove the getPreprocessingInfo also 
      PreprocessingInfo* info = att->getPreprocessingInfo(); 
      ROSE_ASSERT (info != NULL);
      att->setPreprocessingInfo(NULL);// set this as if the OmpAttribute was from a pragma, not from a comment
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
      // TODO move out of this if false body.
      //convert_OpenMP_pragma_to_AST( sageFilePtr);
    }
    convert_OpenMP_pragma_to_AST( sageFilePtr);
  }

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
