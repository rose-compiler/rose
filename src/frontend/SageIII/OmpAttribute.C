// Refined OmpAttribute connected to a parser using Bison
//Liao, 9/18/2008
//

#include "rose.h"

#include "OmpAttribute.h"
#include <algorithm>
using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

namespace OmpSupport{
  //! A builder for OmpAttribute
  OmpAttribute* buildOmpAttribute(omp_construct_enum directive_type, SgNode* node)
  {
    OmpAttribute* result = new OmpAttribute(directive_type,node);
    ROSE_ASSERT(result);
    return result;
  }

    //! Add OmpAttribute to a SgNode
  void addOmpAttribute(OmpAttribute* ompattribute, SgNode* node)
  {
    ROSE_ASSERT(node);
    ROSE_ASSERT(ompattribute);
    node->addNewAttribute("OmpAttribute",ompattribute);
  }

  //! Get OmpAttribute from a SgNode
  OmpAttribute* getOmpAttribute(SgNode* node)
  {
     OmpAttribute* result = NULL;
     AstAttribute* astattribute=node->getAttribute("OmpAttribute");
     if (astattribute)
       result = dynamic_cast<OmpAttribute* > (astattribute);
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
       }
     }
     else
     {
       cerr<<"OmpAttribute::addClause(): Unrecognized clause type:"<<clause_type<<endl;
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
     SgVariableSymbol* symbol = lookupVariableSymbolInParentScopes 
               (varString, scope);
     ROSE_ASSERT(symbol!= NULL);
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
    case e_parallel_sections: result = "parallel sections"; break;
    case e_task: result = "task"; break;
    case e_taskwait: result = "taskwait"; break;
    case e_ordered_directive: result = "ordered"; break;

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

    case e_schedule_none: result = "not-specified"; break;
    case e_schedule_static: result = "static"; break;
    case e_schedule_dynamic: result = "dynamic"; break;
    case e_schedule_guided: result = "guided"; break;
    case e_schedule_auto: result = "auto"; break;
    case e_schedule_runtime: result = "runtime"; break;

    case e_not_omp: result = "not_omp"; break;
  }

  if (isDirective(omp_type))
    result= "omp " + result;
  return result;
}
bool isDirective(omp_construct_enum omp_type)
{
  bool result = false;
  switch (omp_type)
  { // 16 directives as OpenMP 3.0
    case e_parallel:
    case e_for:
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
    case e_parallel_sections:
    case e_task:
    case e_taskwait:

    case e_ordered_directive:
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
  { //common string for all directives
    // atomic, barrier, master, section, ordered, taskwait
   result += OmpSupport::toString(omp_type);
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
          result+=","; // a ',' between each clause
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
      result+= ",";
    result+= toOpenMPString(*iter);
  }
  return result;
}

//! pretty print of OmpAttribute, ideally should reproduce a legal OpenMP pragma
void OmpAttribute::print()
{ 
  std::cout<<"----------------------"<<std::endl;
  SgPragmaDeclaration* pragma = getPragmaDeclaration();
  if (pragma)
   std::cout<<"Original OpenMP pragma is:\n\t\t#pragma "<<pragma->get_pragma()->get_pragma()<<std::endl;
  std::cout<<"Reproduced OpenMP pragma is:\n";
  // Generate OpenMP pragma from OmpAttribute
  cout<<"\t\t#pragma "<<toOpenMPString();
  cout<<std::endl;

}//end print

//! It does not set omp_type and pragma, leaving constructors to do it
void OmpAttribute::init()
    {   
      //TODO, complete this!
        parent = NULL;

	isOrphaned = false;
        hasName = false;

        schedule_kind = e_schedule_none;
        wrapperCount=0;
    }


} //end namespace OmpSupport
