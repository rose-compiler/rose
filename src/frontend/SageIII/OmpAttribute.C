// Refined OmpAttribute connected to a parser using Bison
//Liao, 9/18/2008
//

// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "sageBuilder.h"
#include "OmpAttribute.h"
#include <algorithm>
#include <sstream>
#include <iostream>
#include <string>

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

namespace OmpSupport
{
  //! A builder for OmpAttribute
  OmpAttribute* buildOmpAttribute(omp_construct_enum directive_type, SgNode* node, bool userDefined)
  {
    OmpAttribute* result = new OmpAttribute(directive_type,node);
    result->isUserDefined = userDefined; 
    ROSE_ASSERT(result);
    return result;
  }

  //! Add OmpAttribute to a SgNode
  //SgNode could be a for loop node or a pragma in front of a for loop
  void addOmpAttribute(OmpAttribute* ompattribute, SgNode* node)
  {
    ROSE_ASSERT(node);
    ROSE_ASSERT(ompattribute);
    OmpAttributeList* cur_list =  getOmpAttributeList(node);
    if (!cur_list)
    {
      cur_list = new OmpAttributeList();
      node->addNewAttribute("OmpAttributeList",cur_list);
      // cout<<"OmpSupport::addOmpAttribute() tries to the first one attribute to "<<endl;
      SgLocatedNode * lnode = isSgLocatedNode(node);
      ROSE_ASSERT (lnode != NULL);
      //  cout<<" memory address: "<<lnode<<endl;
      //  cout<<lnode->get_file_info()->get_filename()<<"@" <<lnode->get_file_info()->get_line()<<endl;
    }
    else
    {
      // cout<<"Warning: OmpSupport::addOmpAttribute() tries to add more than one attribute to "<<endl;
      SgLocatedNode * lnode = isSgLocatedNode(node);
      ROSE_ASSERT (lnode != NULL);
      // cout<<" memory address: "<<lnode<<endl;
      // cout<<lnode->get_file_info()->get_filename()<<"@" <<lnode->get_file_info()->get_line()<<endl;
    }
#if 0
    if (ompattribute->getNode () == NULL)
      ompattribute->setNode (node); // This may violate some rules: not from both proprocessingInfo and SgPragmaDeclaration
    else
    {
      ROSE_ASSERT (ompattribute->getNode () == node);
    }
#endif
    //TODO avoid duplicated ompattributes for the same OpenMP directive
    cur_list->ompAttriList.push_back(ompattribute);
  }

  //! Remove OmpAttribute from a SgNode's OmpAttributeList
  void removeOmpAttribute(OmpAttribute* ompattribute, SgNode* node)
  {
    ROSE_ASSERT(node);
    ROSE_ASSERT(ompattribute);
    ROSE_ASSERT (ompattribute->getNode() == node);

    OmpAttributeList* cur_list =  getOmpAttributeList(node);
    ROSE_ASSERT (cur_list != NULL);
    (cur_list->ompAttriList);
    vector <OmpAttribute* >::iterator h_pos = find ((cur_list->ompAttriList).begin(), (cur_list->ompAttriList).end(), ompattribute);
    ROSE_ASSERT (h_pos != (cur_list->ompAttriList).end());
    (cur_list->ompAttriList).erase (h_pos);
    if ((cur_list->ompAttriList).size() ==0)
    {
      delete cur_list;
      node->removeAttribute("OmpAttributeList");
    }
  }

  //! Check if two OmpAttributes are semantically equivalent to each other 
  // It should tolerate different order of variables within variable lists
  bool isEquivalentOmpAttribute (OmpAttribute* a1, OmpAttribute* a2)
  {
    // Arguments check
    // We allow NULL parameters
    //ROSE_ASSERT (a1 != NULL); 
    //ROSE_ASSERT (a2 != NULL); 
    if (a1 == a2) // could be both NULL
    {
      cout<<"Warning: isEquivalentOmpAttribute() tries to compare an OmpAttribute to itself."<<endl;
      return true;
    }  
    else
    {
      // One of them is NULL, definitely not equivalent
      if ((a1 == NULL ) || (a2 == NULL))
        return false;
    }
    // now none of them is NULL
    //check directive type
    if (a1->getOmpDirectiveType()!= a2->getOmpDirectiveType())
      return false;

    // check clauses
    vector <omp_construct_enum> clauseList1 = a1->getClauses(); 
    vector <omp_construct_enum> clauseList2 = a2->getClauses(); 
    // check clause types
    sort (clauseList1.begin(), clauseList1.end());
    sort (clauseList2.begin(), clauseList2.end());
    if (!equal(clauseList1.begin(), clauseList1.end(), clauseList2.begin()))
      return false;
    // For each clause, further check the following ...
    for (size_t i = 0; i < clauseList1.size(); i++)
    {
      // check variable list associated with each clause
      std::vector<std::pair<std::string,SgNode* > > varList1  = a1->getVariableList(clauseList1[i]);
      std::vector<std::pair<std::string,SgNode* > > varList2  = a2->getVariableList(clauseList2[i]);
      sort (varList1.begin(), varList1.end());
      sort (varList2.begin(), varList2.end());
#if 0      
      // debug here
      cout<<"var list 1"<<endl; 
      for (std::vector<std::pair<std::string,SgNode* > >::iterator i1 = varList1.begin();
          i1 != varList1.end(); i1++)
      {
        cout<<"string = "<<(*i1).first<<" SgNode* = " <<(*i1).second <<endl;
      }
      cout<<"var list 2"<<endl; 
      for (std::vector<std::pair<std::string,SgNode* > >::iterator i2 = varList2.begin();
          i2 != varList2.end(); i2++)
      {
        cout<<"string = "<<(*i2).first<<" SgNode* = " <<(*i2).second <<endl;
      }
#endif 
      // The assumption here is variable name and SgInitializedName are used as pairs
      // names and SgInitializedNames should be unique for each variable
      if (!equal(varList1.begin(), varList1.end(), varList2.begin()))
        return false;

      // check expressions associated with each clause
      // This is tricky, we need a SageInterface function to do this: TODO
      // right now, we only compare the unparsed text of expression for simplicity
      std::pair<std::string, SgExpression*> exp1 = a1->getExpression(clauseList1[i]);
      std::pair<std::string, SgExpression*> exp2 = a2->getExpression(clauseList2[i]);

      //do nothing if both are NULL, this is the equal case
      if (!(exp1.second== NULL && exp2.second == NULL ))
      {
        // if both expressions exist
        if (exp1.second && exp2.second)
        {
          if (exp1.second->unparseToString() != exp2.second->unparseToString())
            return false;
        }
        else
          // if only one of the expressions exist
          // must be different
          return false;
      }  
    }

    //Similar handling for reduction clauses of different reduction operator
    // TODO need to check if the omp type is one of reduction types
    vector <omp_construct_enum> reductionList1 = a1->getReductionOperators(); 
    vector <omp_construct_enum> reductionList2 = a2->getReductionOperators(); 
    sort (reductionList1.begin(), reductionList1.end());
    sort (reductionList2.begin(), reductionList2.end());
    if (!equal(reductionList1.begin(), reductionList1.end(), reductionList2.begin()))
      return false;
    for (size_t i = 0; i < reductionList1.size(); i++)
    {
      // check variable list associated with each reduction clause
      std::vector<std::pair<std::string,SgNode* > > varList1  = a1->getVariableList(reductionList1[i]);
      std::vector<std::pair<std::string,SgNode* > > varList2  = a2->getVariableList(reductionList2[i]);
      sort (varList1.begin(), varList1.end());
      sort (varList2.begin(), varList2.end());
      // The assumption here is variable name and SgInitializedName are used as pairs
      // names and SgInitializedNames should be unique for each variable
      if (!equal(varList1.begin(), varList1.end(), varList2.begin()))
        return false;
    }

    // other misc things
    if (a1->hasClause(e_default)) 
      if (a1->getDefaultValue() != a2->getDefaultValue()) 
        return false;
    if (a1->hasClause(e_schedule))  
      if (a1->getScheduleKind() != a2->getScheduleKind())  
        return false;
    if (a1->getOmpDirectiveType() == e_critical)    
      if (a1->isNamedCritical())  
        if (a1->getCriticalName() != a2->getCriticalName())  
          return false;
    // all things are checked to be equal, return true
    return true; 
  }

  //! get omp enum from an OpenMP pragma attached with OmpAttribute
  omp_construct_enum getOmpConstructEnum(SgPragmaDeclaration* decl)
  {
    ROSE_ASSERT (decl != NULL);
    OmpAttributeList* oattlist= getOmpAttributeList(decl);
    ROSE_ASSERT (oattlist != NULL) ;
    vector <OmpAttribute* > ompattlist = oattlist->ompAttriList;
    // There should be only one OmpAttribute attached to each pragma
    ROSE_ASSERT (ompattlist.size() == 1) ;
    vector <OmpAttribute* >::iterator i = ompattlist.begin();
    OmpAttribute* oa = *i;
    omp_construct_enum omp_type = oa->getOmpDirectiveType();
    //   The first attribute should always be the directive type
    ROSE_ASSERT(isDirective(omp_type));
    return omp_type;
  }

  //! Get OmpAttribute from a SgNode
  OmpAttributeList* getOmpAttributeList(SgNode* node)
  {
    OmpAttributeList* result = NULL;
    AstAttribute* astattribute=node->getAttribute("OmpAttributeList");
    if (astattribute)
      result = dynamic_cast<OmpAttributeList* > (astattribute);
    return result;  
  }

   //! Get the first OmpAttribute from a SgNode, return NULL if not found
   OmpAttribute* getOmpAttribute(SgNode* node)
   {
     ROSE_ASSERT (node != NULL );
     OmpAttribute* result = NULL;
     OmpAttributeList* oattlist= getOmpAttributeList(node);
     if (oattlist)
      {
        vector <OmpAttribute* > ompattlist = oattlist->ompAttriList;
        result = ompattlist[0];
      } 
     return result;
   }

  //!Add a clause into an OpenMP directive, the content of the clause is set by other interface, such as addVariable(), addExpression() , setReductionOperator() etc.
  void OmpAttribute::addClause(omp_construct_enum clause_type)
  {
    if (isClause(clause_type))
    {
      //We only store a clause type once
      //Logically, the content will be merged into the first occurence.
      if (!hasClause(clause_type))
      { 
        clause_map[clause_type]=true;
        clauses.push_back(clause_type);
        //        cout<<"adding a clause:"<< OmpSupport::toString(clause_type)<<" to attr:"<< this<<endl;
        //	cout<<"clauses have member count="<<clauses.size()<<endl;
        ROSE_ASSERT(clause_type == clauses[clauses.size()-1]);
      }
    }
    else
    {
      cerr<<"OmpAttribute::addClause(): Unrecognized clause type:"<<OmpSupport::toString(clause_type)<<endl;
      ROSE_ASSERT(false);
    }
  }

  //! Get a vector of clauses existing in the directive
  // We only maintain a map internally, so generated the vector on the fly   
  // This implementation detail is hidden from users, can be changed any time   
  std::vector<omp_construct_enum> OmpAttribute::getClauses()
  {
    /*
       if (hasClause(e_default))     result.push_back(e_default);
       if (hasClause(e_shared))    result.push_back(e_shared);
       if (hasClause(e_private))    result.push_back(e_private);
       if (hasClause(e_firstprivate))    result.push_back(e_firstprivate);
       if (hasClause(e_lastprivate))    result.push_back(e_lastprivate);
       if (hasClause(e_copyin))    result.push_back(e_copyin);
       if (hasClause(e_copyprivate))    result.push_back(e_copyprivate);

       if (hasClause(e_if)) result.push_back(e_if);
       if (hasClause(e_num_threads)) result.push_back(e_num_threads);
       if (hasClause(e_nowait)) result.push_back(e_nowait);
       if (hasClause(e_ordered_clause)) result.push_back(e_ordered_clause);
       if (hasClause(e_reduction)) result.push_back(e_reduction);
       if (hasClause(e_schedule)) result.push_back(e_schedule);
       if (hasClause(e_collapse)) result.push_back(e_collapse);
       if (hasClause(e_untied))  result.push_back(e_untied);
       */
    return clauses;
  }

  //! Get the associated SgPragmaDeclaration
  SgPragmaDeclaration* OmpAttribute::getPragmaDeclaration()
  {
    SgPragmaDeclaration* result = NULL;
    if (mNode)
      result = isSgPragmaDeclaration(mNode);
    return result;
  }


  //! Insert a variable into a variable list for clause "targetConstruct", maintain the reversed variable-clause mapping also.
  void OmpAttribute::addVariable(omp_construct_enum targetConstruct, const std::string& varString, SgInitializedName* sgvar/*=NULL*/)
  {
    //Special handling for reduction clauses
    if (targetConstruct == e_reduction)
    {
      cerr<<"Fatal: cannot add variables into e_reduction, You have to specify e_reduction_operatorX instead!"<<endl;
      assert(false);
    } 
    if (isReductionOperator(targetConstruct))
    {
      addClause(e_reduction);
      setReductionOperator(targetConstruct);
    }  
    // Try to resolve the variable if SgInitializedName is not provided
    if ((sgvar == NULL)&&(mNode!=NULL))
    {
      SgScopeStatement* scope = SageInterface::getScope(mNode);
      ROSE_ASSERT(scope!=NULL);
      //resolve the variable here
      SgVariableSymbol* symbol = lookupVariableSymbolInParentScopes (varString, scope);
      if (symbol == NULL)          
      {
        cerr<<"Error: OmpAttribute::addVariable() cannot find symbol for variable:"<<varString<<endl;
        ROSE_ASSERT(symbol!= NULL);
      }
      else 
        sgvar = symbol->get_declaration();
    }  
    //debug clause var_list
    // if (targetConstruct== e_copyin) cout<<"debug: adding variable to copyin()"<<endl;
    variable_lists[targetConstruct].push_back(make_pair(varString, sgvar));
    // maintain the var-clause map also
    var_clauses[varString].push_back(targetConstruct);
    // Don't forget this! But directive like threadprivate could have variable list also
    if (isClause(targetConstruct)) 
      addClause(targetConstruct);
  }

  //! Set name for named critical section
  void OmpAttribute::setCriticalName(const std::string & varname)
  {
    hasName = true;
    name = varname;
  }

  //! Expression 
  void OmpAttribute::addExpression(omp_construct_enum targetConstruct, const std::string& expString, SgExpression* sgexp/* =NULL */)
  {
    expressions[targetConstruct]=make_pair(expString,sgexp);
    if (sgexp!=NULL)
      sgexp->set_parent(mNode); // a little hack here, we not yet extend the SgPragmaDeclaration to have expression children.
  }

  std::pair<std::string, SgExpression*>
    OmpAttribute::getExpression(omp_construct_enum targetConstruct)
    {
      return expressions[targetConstruct];
    }

  // default () value
  void OmpAttribute::setDefaultValue(omp_construct_enum valuex)
  {
    switch (valuex)
    {
      case  e_default_none:
      case  e_default_shared:
      case  e_default_private:
      case  e_default_firstprivate:
        default_scope = valuex;
        break;
      default:
        cerr<<"OmpAttribute::setDefaultValue() Illegal default scoping value:"<<valuex<<endl;
        ROSE_ASSERT(false);
    }
  }

  enum omp_construct_enum OmpAttribute::getDefaultValue()
  {
    // It does not make sense to get the default value
    // if there is no default clause
    ROSE_ASSERT(hasClause(e_default));
    return default_scope;
  }

  // Reduction clause's operator, 
  // we store reduction clauses of the same operators into a single entity
  void OmpAttribute::setReductionOperator(omp_construct_enum operatorx)
  {
    assert(isReductionOperator(operatorx));
    std::vector<omp_construct_enum>::iterator hit = 
      find(reduction_operators.begin(),reduction_operators.end(), operatorx); 
    if (hit == reduction_operators.end())   
      reduction_operators.push_back(operatorx);
  }
  // 
  std::vector<omp_construct_enum> OmpAttribute::getReductionOperators()
  {
    return reduction_operators;
  }

  bool OmpAttribute::hasReductionOperator(omp_construct_enum operatorx)
  {
    return (find(reduction_operators.begin(), reduction_operators.end(),operatorx) != reduction_operators.end());
  }

  //! Find the relevant clauses for a variable 
  std::vector<enum omp_construct_enum> 
    OmpAttribute::get_clauses(const std::string& variable)
    {
      return var_clauses[variable];
    }

  // Schedule kind
  void OmpAttribute::setScheduleKind(omp_construct_enum kindx)
  {
    //validating the parameter here
    switch (kindx)
    {
      case  e_schedule_none:
      case  e_schedule_static:
      case  e_schedule_dynamic:
      case  e_schedule_guided:
      case  e_schedule_auto:
      case  e_schedule_runtime:
        schedule_kind = kindx;
        break;
      default:
        cerr<<"OmpAttribute::setScheduleKind() Illegal schedule kind:"<<kindx<<endl;
        ROSE_ASSERT(false);
        break;
    }
  }

  omp_construct_enum OmpAttribute::getScheduleKind()
  {
    ROSE_ASSERT(hasClause(e_schedule)==true);
    return schedule_kind;
  }
  // OmpSupport::toString()
  string toString(omp_construct_enum omp_type)
  {
    string result;
    switch (omp_type)
    {
      /*
         case e_: result = ""; break;
         */
      //directives
      case e_unknown: result ="unknown" ; break;
      case e_parallel: result = "parallel" ; break;
      case e_for: result = "for"; break;
      case e_do: result = "do"; break;
      case e_workshare: result = "workshare"; break;
      case e_sections: result = "sections"; break;
      case e_section: result = "section"; break;
      case e_single: result = "single"; break;

      case e_master: result = "master"; break;
      case e_critical: result = "critical"; break;
      case e_barrier: result = "barrier"; break;
      case e_atomic: result = "atomic"; break;
      case e_flush: result = "flush"; break;


      case e_threadprivate: result = "threadprivate"; break;
      case e_parallel_for: result = "parallel for"; break;
      case e_parallel_do: result = "parallel do"; break;
      case e_parallel_sections: result = "parallel sections"; break;
      case e_parallel_workshare: result = "parallel workshare"; break;
      case e_task: result = "task"; break;
      case e_taskwait: result = "taskwait"; break;
      case e_ordered_directive: result = "ordered"; break;

                                // Fortran only end directives
      case e_end_critical: result = "end critical"; break;
      case e_end_do: result = "end do"; break;
      case e_end_master: result = "end master"; break;
      case e_end_ordered:result = "end ordered"; break;
      case e_end_parallel_do:result = "end parallel do"; break;
      case e_end_parallel_sections:result = "end parallel sections"; break;
      case e_end_parallel_workshare:result = "end parallel workshare"; break;
      case e_end_parallel:result = "end parallel"; break;
      case e_end_sections:result = "end sections"; break;
      case e_end_single:result = "end single"; break;
      case e_end_task:result = "end task"; break;
      case e_end_workshare:result = "end workshare"; break;

                           // clauses
      case e_default: result = "default"; break;
      case e_shared: result = "shared"; break;
      case e_private: result = "private"; break;
      case e_firstprivate: result = "firstprivate"; break;
      case e_lastprivate: result = "lastprivate"; break;
      case e_copyin: result = "copyin"; break;
      case e_copyprivate: result = "copyprivate"; break;


      case e_if: result = "if"; break;
      case e_num_threads: result = "num_threads"; break;
      case e_nowait: result = "nowait"; break;
      case e_ordered_clause: result = "ordered"; break;
      case e_reduction: result = "reduction"; break;
      case e_schedule: result = "schedule"; break;
      case e_collapse: result = "collapse"; break;
      case e_untied: result = "untied"; break;

                     // values
      case e_default_none: result = "none"; break;
      case e_default_shared: result = "shared"; break;
      case e_default_private: result = "private"; break;
      case e_default_firstprivate: result = "firstprivate"; break;

      case e_reduction_plus: result = "+"; break;
      case e_reduction_minus: result = "-"; break;
      case e_reduction_mul: result = "*"; break;
      case e_reduction_bitand: result = "&"; break;
      case e_reduction_bitor: result = "|"; break;

      case e_reduction_bitxor: result = "^"; break;
      case e_reduction_logand: result = "&&"; break;
      case e_reduction_logor: result = "||"; break;

      case e_reduction_min: result = "min"; break;
      case e_reduction_max: result = "max"; break;

      case e_reduction_and: result = ".and."; break;
      case e_reduction_or: result = ".or."; break;
      case e_reduction_eqv: result = ".eqv."; break;

      case e_reduction_neqv: result = ".neqv."; break;
      case e_reduction_iand: result = "iand"; break;

      case e_reduction_ior: result = "ior"; break;
      case e_reduction_ieor: result = "ieor"; break;


      case e_schedule_none: result = "not-specified"; break;
      case e_schedule_static: result = "static"; break;
      case e_schedule_dynamic: result = "dynamic"; break;
      case e_schedule_guided: result = "guided"; break;
      case e_schedule_auto: result = "auto"; break;
      case e_schedule_runtime: result = "runtime"; break;

      case e_not_omp: result = "not_omp"; break;
    }

    // Not true for Fortran!!
    //    if (isDirective(omp_type))
    //      result= "omp " + result;
    return result;
  }
  bool isDirective(omp_construct_enum omp_type)
  {
    bool result = false;
    switch (omp_type)
    { // 16 directives as OpenMP 3.0
      //+2 for Fortran
      case e_parallel:
      case e_for:
      case e_do:
      case e_workshare:
      case e_sections:
      case e_section:
      case e_single:

      case e_master: 
      case e_critical:
      case e_barrier:
      case e_atomic:
      case e_flush:

      case e_threadprivate:
      case e_parallel_for:
      case e_parallel_do: //fortran
      case e_parallel_sections:
      case e_parallel_workshare://fortran
      case e_task:
      case e_taskwait:

      case e_ordered_directive:

        // Fortran only end directives
      case e_end_critical:
      case e_end_do:
      case e_end_master:
      case e_end_ordered:
      case e_end_parallel_do:
      case e_end_parallel_sections:
      case e_end_parallel_workshare:
      case e_end_parallel:
      case e_end_sections:
      case e_end_single:
      case e_end_task:
      case e_end_workshare:

        result = true;
        break;
      default:
        result = false;
        break;
    }
    return result;
  }

  //! Get the corresponding begin construct enum from an end construct enum
  omp_construct_enum getBeginOmpConstructEnum (omp_construct_enum end_enum)
  {
    ROSE_ASSERT (isFortranEndDirective (end_enum));
    omp_construct_enum begin_enum;
    switch (end_enum)
    {
      case e_end_parallel:
        begin_enum = e_parallel;
        break;
      case e_end_do:
        begin_enum = e_do;
        break;
      case e_end_sections:
        begin_enum = e_sections;
        break;
      case e_end_single:
        begin_enum = e_single;
        break;
      case e_end_workshare:
        begin_enum = e_workshare;
        break;
      case e_end_parallel_do:
        begin_enum = e_parallel_do;
        break;
      case e_end_parallel_sections:
        begin_enum = e_parallel_sections;
        break;
      case e_end_parallel_workshare:
        begin_enum = e_parallel_workshare;
        break;
      case e_end_task:
        begin_enum = e_task;
        break;
      case e_end_master:
        begin_enum = e_master;
        break;
      case e_end_critical:
        begin_enum = e_critical;
        break;
      case e_end_ordered:
        begin_enum = e_ordered_directive;
        break;
      default:
        {
          ROSE_ASSERT (false);
          break;
        }
    } // end switch
    return begin_enum;
  }

  //! Check if the construct is a Fortran directive which can (optionally) have a corresponding END directive
  bool isFortranBeginDirective(omp_construct_enum omp_type)
  {
    bool rt = false;
    switch (omp_type)
    {
      case e_parallel:
      case e_do:
      case e_sections:
      case e_single:
      case e_workshare:
      case e_parallel_do:
      case e_parallel_sections:
      case e_parallel_workshare:
      case e_task:
      case e_master:
      case e_critical:
      case e_ordered_directive:
        {
          rt = true;
          break;
        }
      default:
        {
          break;
        }
    }
    return rt;
  }
  //! Get the corresponding end construct enum from a begin construct enum
  omp_construct_enum getEndOmpConstructEnum (omp_construct_enum begin_enum)
  {
    omp_construct_enum rt; 
    switch (begin_enum)
    {
      case e_parallel:
        rt = e_end_parallel;
        break;
      case e_do:
        rt = e_end_do;
        break;
      case e_sections:
        rt = e_end_sections;
        break;
      case e_single:
        rt = e_end_single;
        break;
      case e_workshare:
        rt = e_end_workshare;
        break;
      case e_parallel_do:
        rt = e_end_parallel_do;
        break;
      case e_parallel_sections:
        rt = e_end_parallel_sections;
        break;
      case e_parallel_workshare:
        rt = e_end_parallel_workshare;
        break;
      case e_task:
        rt = e_end_task;
        break; 
      case e_master:
        rt = e_end_master;
        break;
      case e_critical:
        rt = e_end_critical;
        break;
      case e_ordered_directive:
        {
          rt = e_end_ordered;
          break;
        }
      default:
        {
          cerr<<"In getEndOmpConstructEnum(): illegal begin enum is found:"<< toString(begin_enum)<<endl;
          ROSE_ASSERT (false);
          break;
        }
    } // end switch
    return rt;
  }

  //! Check if the construct is a Fortran END ... directive
  bool isFortranEndDirective(omp_construct_enum omp_type)
  {
    bool rt = false;
    switch (omp_type)
    { // 16 directives as OpenMP 3.0
      //+2 for Fortran
      case e_end_critical:
      case e_end_do:
      case e_end_master:
      case e_end_ordered:
      case e_end_parallel_do:
      case e_end_parallel_sections:
      case e_end_parallel_workshare:
      case e_end_parallel:
      case e_end_sections:
      case e_end_single:
      case e_end_task:
      case e_end_workshare:
        rt = true; 
        break; 
      default:
        break;
    } 
    return rt;  
  }

  //! Check if an OpenMP directive has a structured body
  bool isDirectiveWithBody(omp_construct_enum omp_type)
  {
    bool result = false;
    ROSE_ASSERT(isDirective(omp_type));
    switch (omp_type)
    { // 16 directives as OpenMP 3.0
      //+2 for Fortran
      case e_parallel:
      case e_for:
      case e_do:
      case e_workshare:
      case e_sections:
      case e_section:
      case e_single:

      case e_master: 
      case e_critical:
      case e_barrier:
      case e_atomic:
        //    case e_flush:

        //      case e_threadprivate:
      case e_parallel_for:
      case e_parallel_do: //fortran
      case e_parallel_sections:
      case e_parallel_workshare://fortran
      case e_task:
        //      case e_taskwait:

      case e_ordered_directive:

        // Fortran only end directives
        //      case e_end_critical:
        //      case e_end_do:
        //      case e_end_master:
        //      case e_end_ordered:
        //      case e_end_parallel_do:
        //      case e_end_parallel_sections:
        //      case e_end_parallel_workshare:
        //      case e_end_parallel:
        //      case e_end_sections:
        //      case e_end_single:
        //      case e_end_task:
        //      case e_end_workshare:

        result = true;
        break;
      default:
        result = false;
        break;
    }
    return result;

  }

  bool isClause(omp_construct_enum clause_type)
  {
    bool result = false;

    switch (clause_type)
    { //total 15 possible clauses as OpenMP 3.0
      case e_default:
      case e_shared:
      case e_private:
      case e_firstprivate:
      case e_lastprivate:

      case e_copyin:
      case e_copyprivate:
      case e_if:
      case e_num_threads:
      case e_nowait:

      case e_ordered_clause:
      case e_reduction:
      case e_schedule:
      case e_collapse:
      case e_untied:
        result = true; 
        break;
      default:
        result = false;
        break;
    }
    return result; 
  }


  bool isReductionOperator(omp_construct_enum omp_type)
  {
    bool result = false;
    switch (omp_type)
    {
      case e_reduction_plus:
      case e_reduction_minus:
      case e_reduction_mul:
      case e_reduction_bitand:
      case e_reduction_bitor:

      case e_reduction_bitxor:
      case e_reduction_logand:
      case e_reduction_logor: 
        // TODO more reduction intrinsic procedure name for Fortran  
      case e_reduction_min: //?
      case e_reduction_max:
        result = true;
        break;
      default:
        result = false;
        break;
    }
    return result;
  }

  bool OmpAttribute::hasClause(omp_construct_enum omp_type)
  {
    bool result = false;
    if (isClause(omp_type))
      result =clause_map[omp_type];
    else
    {        
      cerr<<"OmpAttribute::hasClause(): Unrecognized clause type as a parameter:"<<omp_type<<endl;
      ROSE_ASSERT(false);
    }
    return result;
  }

  //--------------------------
  //Judge if a name is in a clause's variable list: like private(x,y,z), copyin(), etc.
  bool OmpAttribute::isInConstruct(const string & varName, enum omp_construct_enum omptype)
  {
    bool result =false;
    ROSE_ASSERT(varName.size()!= 0);
    vector <omp_construct_enum> construct_vec = get_clauses(varName);
    vector <omp_construct_enum>::iterator iter;
    iter = find (construct_vec.begin(), construct_vec.end(),omptype);
    if (iter != construct_vec.end())
      result = true;
    return result;
  }

  //! Convert directives and clauses to string 
  // Top down recursive conversion: directive ,directive-optional stuff, 
  // then clauses one by one, plus clause-optional stuff
  std::string OmpAttribute::toOpenMPString(omp_construct_enum omp_type)
  {
    string result;
    //Directives ------------------
    if (isDirective(omp_type))
    { 
      //common string for all directives
      // atomic, barrier, master, section, ordered, taskwait
      result += OmpSupport::toString(omp_type);

      // extra stuff associated with a directive, but not clauses
      // optional: critical's name
      if (omp_type == e_critical)
      {
        if (isNamedCritical())
          result+=" ("+ getCriticalName()+")";
      } 
      // optional: variable lists for flush () and threadprivate()
      else if ((omp_type == e_flush)||(omp_type == e_threadprivate))
      {
        if(hasVariableList(omp_type))
        {
          string varListString = toOpenMPString(getVariableList(omp_type));
          result+=" (" + varListString + ")"; 
        }
      }// end of flush(), threadprivate()
      else if (omp_type == e_end_critical)
      {
        result += " " + getCriticalName(); 
      }
#if 0  // clauses are handled separately      
      // optional nowait for fortran: end do, end sections, end workshare, end single
      else if ((omp_type == e_for) 
          || (omp_type == e_sections)
          || (omp_type == e_single)
          || (omp_type == e_end_do)
          ||(omp_type == e_end_sections)
          ||(omp_type == e_end_workshare
            ||(omp_type == e_end_single)))
      {
        if (hasClause(e_nowait)) 
          result += " "+ OmpSupport::toString(e_nowait);
      }
#endif      

    } // end if directives
    //Clauses ------------------
    else if (isClause(omp_type))
    {
      // optional expressions
      if((omp_type == e_if)||
          (omp_type ==e_num_threads)||
          (omp_type == e_collapse)
        )
      {
        result += OmpSupport::toString(omp_type);
        string expString;
        // We store real SgExpression* in .second now, 
        // No need to save the original string format in .first 
        if (getExpression(omp_type).first.size()>0) 
          expString   = getExpression(omp_type).first;
        else if (getExpression(omp_type).second!=NULL)
          expString = getExpression(omp_type).second->unparseToString();
        result+=" (" + expString+ ")"; 
      } 
      // optional variable lists
      else if ((omp_type == e_copyprivate)||
          (omp_type ==e_private)||
          (omp_type == e_firstprivate)||
          (omp_type == e_shared)||
          (omp_type == e_copyin)||
          (omp_type == e_lastprivate)
          )
      {
        result += OmpSupport::toString(omp_type);
        string varListString = toOpenMPString(getVariableList(omp_type));
        result+=" (" + varListString + ")"; 
      }
      // default scoping values
      else if (omp_type == e_default)
      {
        result += OmpSupport::toString(omp_type);
        result+=" ("+ OmpSupport::toString(getDefaultValue())+")";
      } // reduction (op:var-list)
      // could have multiple reduction clauses 
      else if (omp_type == e_reduction)
      {
        std::vector<omp_construct_enum> operators = getReductionOperators();
        std::vector<omp_construct_enum>::iterator iter = operators.begin();
        for (; iter!=operators.end();iter++)
        {
          if (iter!=operators.begin())
            result+=" "; // a ' ' between each clause
          omp_construct_enum optype = *iter;
          result += OmpSupport::toString(omp_type);
          result +=" ("+ OmpSupport::toString(optype)+":";
          // variable list is associated to each reduction operator
          string varListString = toOpenMPString(getVariableList(optype));
          result += varListString + ")";
        }  
      } // schedule(kind, exp)
      else if (omp_type == e_schedule)
      { 
        result += OmpSupport::toString(omp_type);
        result +=" ("+ OmpSupport::toString(getScheduleKind());
        string expString;
        // We store real SgExpression* in .second now, 
        // No need to save the original string format in .first
        if (getExpression(omp_type).first.size()>0)
          expString = getExpression(omp_type).first;
        else  if (getExpression(omp_type).second !=  NULL)
          expString =  getExpression(omp_type).second->unparseToString();

        if (expString.size()>0)
          result += "," + expString;
        result += ")";
      }
      else // catch all cases
      {
        result += OmpSupport::toString(omp_type);
      }
    }// end clauses
    return result; 
  }

  //! Convert a variable list to x,y,z ,without parenthesis.
  std::string OmpAttribute::toOpenMPString(std::vector<std::pair<std::string,SgNode* > >var_list)
  {
    string result;
    std::vector<std::pair<std::string,SgNode* > >::iterator iter;
    for (iter=var_list.begin();iter!=var_list.end();iter++)
    {
      if (iter != var_list.begin())
        result +=",";
      result+=(*iter).first;
    }
    return result;
  }

  //! Get the variable list associated with a construct
  std::vector<std::pair<std::string,SgNode* > > 
    OmpAttribute::getVariableList(omp_construct_enum targetConstruct)
    {
      std::vector<std::pair<std::string,SgNode* > > * result = new std::vector<std::pair<std::string,SgNode* > >; 
      // e_reduction is a collective concept, 
      // There may have multiple reduction clauses for different operations.
      // return all of them. Return special one if e_reduction_operatorX is used
      if (targetConstruct==e_reduction)
      {
        std::vector<omp_construct_enum> ops = getReductionOperators();
        std::vector<omp_construct_enum>::iterator iter = ops.begin();
        for (;iter!=ops.end(); iter++) // for each reduction operator
        { 
          omp_construct_enum operation = *iter;
          assert(isReductionOperator(operation));
          std::vector<std::pair<std::string,SgNode* > > temp = variable_lists[operation];
          std::vector<std::pair<std::string,SgNode* > > ::iterator iter2 = temp.begin();
          for (;iter2!=temp.end(); iter2++)
            result->push_back(*iter2);
        }  
        return *result;
      } 
      else
        return variable_lists[targetConstruct];
    }
  //! Check if a variable list is associated with a construct
  bool OmpAttribute::hasVariableList(omp_construct_enum omp_type)
  {
    vector<std::pair<std::string,SgNode* > > var_list = variable_lists[omp_type];
    return (var_list.size()!=0);
  }
  // interface to convert OmpAttribute to OpenMP string format, not OmpAttribute::toString()
  std::string OmpAttribute::toOpenMPString()
  {
    string result; // Should not add "#pragma ", which is automatically generated by SgPragmaDeclaration
    // Convert directive first
    result += toOpenMPString(omp_type);

    // Convert clauses then
    vector<omp_construct_enum> clause_vector = getClauses();
    vector<omp_construct_enum>::iterator iter;
    for (iter=clause_vector.begin();iter!=clause_vector.end();iter++)
    {  
      if (iter==clause_vector.begin())
        result+= " "; // space between directive and first clause
      else
        result+= " "; //avoid using ',' since GCC 4.2.x is picky about this
      result+= toOpenMPString(*iter);
    }
    return result;
  }

  std::string OmpAttributeList::toOpenMPString()
  {
    string result;
    std::vector<OmpAttribute*>::const_iterator citer;
    for (citer= ompAttriList.begin(); citer != ompAttriList.end(); citer++)
    {
      OmpAttribute* attribute = *citer;
      result += attribute->toOpenMPString();
    }
    return result;
  }

  OmpAttributeList::~OmpAttributeList()
  {
    std::vector<OmpAttribute*>::const_iterator citer;
    for (citer= ompAttriList.begin(); citer != ompAttriList.end(); citer++)
    {
      OmpAttribute* attribute = *citer;
      delete attribute; 
    }
  }

  //! pretty print of OmpAttribute, ideally should reproduce a legal OpenMP pragma
  void OmpAttribute::print()
  { 
    std::cout<<"----------OmpAttribute::print()------------:\n"<<this<<std::endl;
    SgPragmaDeclaration* pragma = getPragmaDeclaration();
    PreprocessingInfo* info = getPreprocessingInfo();
    // directive can come from only one of two sources, not both. 
    ROSE_ASSERT(pragma == NULL || info == NULL); 
    if (pragma)
      std::cout<<"Original  :\t#pragma "<<pragma->get_pragma()->get_pragma()<<std::endl;
    else if (info)
      std::cout<<"Original  :\t"<<info->getString()<<std::endl;
    else 
      std::cout<<"Warning: Cannot find an associated AST piece to get the original OpenMP directive text." <<std::endl;

    std::cout<<"Reproduced:\t";
    // Generate OpenMP pragma from OmpAttribute
    if (pragma)
      cout<<"#pragma omp "<<toOpenMPString();
    else
      cout<<"!$omp "<<toOpenMPString();
    cout<<std::endl;

  }//end print

  void OmpAttributeList::print()
  {
    std::vector<OmpAttribute*>::const_iterator citer;
    for (citer= ompAttriList.begin(); citer != ompAttriList.end(); citer++)
    {
      OmpAttribute* attribute = *citer;
      attribute->print();
    }
  }

  //! It does not set omp_type and pragma, leaving constructors to do it
  void OmpAttribute::init()
  {   
    //TODO, complete this!
    parent = NULL;

    isOrphaned = false;
    hasName = false;
    pinfo = NULL;

    default_scope = e_unknown;
    schedule_kind = e_schedule_none;
    wrapperCount=0;
  }

  void generatePragmaFromOmpAttribute(SgNode* sg_node)
  {
    SgStatement* cur_stmt = isSgStatement(sg_node);
    ROSE_ASSERT(cur_stmt != NULL);
    // For C/C++, only one OmpAttribute could be attached to each SgNOde
    // since C/C++ have scopes for each of them
    // They cannot be used together in one scope?
    // But we prepare the worst: it may be allowed?
    OmpAttributeList* attlist = getOmpAttributeList(sg_node);
    if(attlist)
    {
      // No need to duplicate a pragma for an existing OpenMP pragma
      if (isSgPragmaDeclaration(cur_stmt))
        return;
      // Should only insert the pragma statement
      // if there is no existing OpenMP pragma with the same attribute
      SgStatement* prev_stmt = SageInterface::getPreviousStatement(cur_stmt);
      if (prev_stmt)
      {
        SgPragmaDeclaration * prev_pragma = isSgPragmaDeclaration(prev_stmt);
        if (prev_pragma)
        {
          OmpAttributeList* prev_attlist= getOmpAttributeList(prev_pragma);
          if (attlist == prev_attlist)
            return;
        }
      }

      // Now we are safe to append the pragma
      std::vector<OmpAttribute*>::reverse_iterator riter;
      for (riter=attlist->ompAttriList.rbegin(); riter !=attlist->ompAttriList.rend();riter++)
      {
        OmpAttribute* att = *riter; //getOmpAttribute(sg_node);
        if (att->getOmpDirectiveType() ==e_for ||att->getOmpDirectiveType() ==e_parallel_for)
          ROSE_ASSERT(isSgForStatement(cur_stmt) != NULL);

        string pragma_str= att->toOpenMPString();
        SgPragmaDeclaration * pragma = SageBuilder::buildPragmaDeclaration("omp "+ pragma_str);
        SageInterface::insertStatementBefore(cur_stmt, pragma);
      }
    } // if (attlist)
  }

  //! Generate a diff text for the OpenMP attribute attached to a node
  //This is essentially a workaround to generate a translation as a patch
  std::string generateDiffTextFromOmpAttribute(SgNode* sg_node)
  {
    std::string rtxt;

    SgStatement* cur_stmt = isSgStatement(sg_node);
    ROSE_ASSERT(cur_stmt != NULL);
    // For C/C++, only one OmpAttribute could be attached to each SgNOde
    // since C/C++ have scopes for each of them
    // They cannot be used together in one scope?
    // But we prepare the worst: it may be allowed?
    OmpAttributeList* attlist = getOmpAttributeList(sg_node);
    if(attlist)
    {
      // No need to duplicate a pragma for an existing OpenMP pragma
      if (isSgPragmaDeclaration(cur_stmt))
        return rtxt;
      // Should only insert the pragma statement
      // if there is no existing OpenMP pragma with the same attribute
      SgStatement* prev_stmt = SageInterface::getPreviousStatement(cur_stmt);
      if (prev_stmt)
      {
        SgPragmaDeclaration * prev_pragma = isSgPragmaDeclaration(prev_stmt);
        if (prev_pragma)
        {
          OmpAttributeList* prev_attlist= getOmpAttributeList(prev_pragma);
          if (attlist == prev_attlist)
            return rtxt;
        }
      }

      // Now we are safe to generate a diff text chunk, such as
      // 4a5
      // > #pragma omp parallel for
      std::vector<OmpAttribute*>::reverse_iterator riter;
      for (riter=attlist->ompAttriList.rbegin(); riter !=attlist->ompAttriList.rend();riter++)
      {
        if (riter == attlist->ompAttriList.rbegin() )
        {
          SgLocatedNode * lnode = isSgLocatedNode(sg_node);
          ROSE_ASSERT (lnode != NULL);
          int line_no = lnode->get_file_info()->get_line();
          std::ostringstream os ;
          os<< (line_no -1);
          rtxt = os.str()+"a";
          std::ostringstream os2 ;
          os2<< line_no;
          rtxt = rtxt + os2.str()+"\n";
        }
        OmpAttribute* att = *riter; //getOmpAttribute(sg_node);
        if (att->getOmpDirectiveType() ==e_for ||att->getOmpDirectiveType() ==e_parallel_for)
          ROSE_ASSERT(isSgForStatement(cur_stmt) != NULL);

        string pragma_str= att->toOpenMPString();
        rtxt += "> #pragma omp "+ pragma_str + "\n";
      }
    } // if (attlist)
    return rtxt;
  }



} //end namespace OmpSupport
