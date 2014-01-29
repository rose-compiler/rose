#include "sage3basic.h"
#include "sageBuilder.h"
#include "failSafePragma.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

string FailSafe::toString(failsafe_construct_enum fs_type)
{
  string result; 
  switch (fs_type)
  { 
     // directives
     case e_region: result = "region" ; break;
     case e_status_predicate: result = "status"; break;
     case e_data_predicate: result = "data" ; break;
     case e_tolerance: result = "tolerance" ; break;
     case e_double_redundancy: result = "double_redundancy"; break;
     case e_triple_redundancy: result = "triple_redundancy"; break;
     case e_save:   result = "save"; break;

     // clauses
     case e_assert:   result = "assert"; break;
//     case e_specifier: result = "specifier"; break;
     case e_region_reference:   result = "in"; break;
     case e_violation:   result = "violation"; break;
     case e_recover:   result = "recover"; break;
     
     // special valued clause: 
     case e_pre:   result = "pre"; break;
     case e_post:   result = "post"; break;

    // values of some clauses
     case e_type_ET1:   result = "ET1"; break;
     case e_type_ET2:   result = "ET2"; break;
  
     
     case e_type_NaN:   result = "NaN"; break;
     case e_type_SECDED:   result = "SECDED"; break;
     case e_type_SEGFAULT:   result = "SEGFAULT"; break;
     case e_type_ANY:   result = "ANY"; break;
  
     default: 
       cerr<<"Error: unhandled failsafe construct within FailSafe::toString()."<<endl;
       ROSE_ASSERT (false);
       break;
  }
  return result; 
}

bool FailSafe::isDirective (failsafe_construct_enum fs_type)
{
  boll result = false;
  switch (fs_type)
  {
    case e_region:
    case e_e_status_predicate:
    case e_data_predicate:
    case e_tolerance:
    case e_double_redundancy:
    case e_triple_redundancy:
    case e_save:
      result = true;
      break;
    default:
      result = false;
      break;
  }
  return result;
}

bool FailSafe::isClause(failsafe_construct_enum fs_type)
{
  boll result = false;
  switch (fs_type)
  {
    case e_assert:
    case e_region_reference:
    case e_violation:
    case e_recover:
    case e_pre:
    case e_post:
      result = true;
      break;
    default:
      result = false;
      break;
  }
  return result;
}
void FailSafe::process_failsafe_directives (SgSourceFile *sageFilePtr)
{
  if (SgProject::get_verbose() > 1)
  {
    printf ("Processing FAIL-SAFE directives \n");
  }
  ROSE_ASSERT(sageFilePtr != NULL);
  
  //1. Parse directives and generate attribute information

  //2. Build dedicate AST nodes for failsafe directives 

  //3. Translate failsafe AST nodes 

}

void FailSafe::FailSafeAttribute::init()
{
  hasSpecifier = false;
  hasName = false;
}

FailSafeAttribute * FailSafe::buildAttribute (fail_safe_construct_enum fstype, SgNode* node)
{
  FailSafeAttribute* result = new FailSafeAttribute (fstype, node);
  ROSE_ASSERT (result);
  return result;
}

void FailSafe::addAttribute (FailSafeAttribute* attribute, SgNode* node)
{
  ROSE_ASSERT(node);
  ROSE_ASSERT(attribute);
  SgLocatedNode * lnode = isSgLocatedNode(node);
  ROSE_ASSERT (lnode != NULL);

  FailSafeAttribute* cur_list = getAttributeList (node);
  if (! cur_list)
  {
    cur_list = new AttributeList();
    node->addNewAttribute("FailSafeAttributeList",cur_list);
  }

  cur_list->attriList.push_back(attribute);
}

//! Remove Attribute from a SgNode's attributeList
void FailSafe::removepAttribute(FailSafeAttribute* attribute, SgNode* node)
{
  ROSE_ASSERT(node);
  ROSE_ASSERT(attribute);
  ROSE_ASSERT (attribute->getNode() == node);

  OmpAttributeList* cur_list =  getAttributeList(node);
  ROSE_ASSERT (cur_list != NULL);
  (cur_list->attriList);
  vector <FailSafeAttribute* >::iterator h_pos = find ((cur_list->attriList).begin(), (cur_list->attriList).end(), attribute);
  ROSE_ASSERT (h_pos != (cur_list->attriList).end());
  (cur_list->attriList).erase (h_pos);
  if ((cur_list->attriList).size() ==0)
  {
    delete cur_list;
    node->removeAttribute("FailSafeAttributeList");
  }
}

//! get omp enum from an OpenMP pragma attached with OmpAttribute
fail_safe_construct_enum FailSafe::getConstructEnum(SgPragmaDeclaration* decl)
{
  ROSE_ASSERT (decl != NULL);
  AttributeList* oattlist= getAttributeList(decl);
  ROSE_ASSERT (oattlist != NULL) ;
  vector <FailSafeAttribute* > ompattlist = oattlist->attriList;
  // There should be only one Attribute attached to each pragma
  ROSE_ASSERT (ompattlist.size() == 1) ;
  vector <FailSafeAttribute* >::iterator i = ompattlist.begin();
  FailSafeAttribute* oa = *i;
  fail_safe_construct_enum fs_type= oa->getDirectiveType();
  //   The first attribute should always be the directive type
  ROSE_ASSERT(isDirective(fs_type));
  return fs_type;
}

//! Get Attribute from a SgNode
AttributeList* FailSafe::getAttributeList(SgNode* node)
{
  AttributeList* result = NULL;
  AstAttribute* astattribute=node->getAttribute("FailSafeAttributeList");
  if (astattribute)
    result = dynamic_cast<AttributeList* > (astattribute);
  return result;
}

//! Get the first Attribute from a SgNode, return NULL if not found
FailSafeAttribute* FailSafe::getAttribute(SgNode* node)
{
  ROSE_ASSERT (node != NULL );
  FailSafeAttribute* result = NULL;
  AttributeList* oattlist= getAttributeList(node);
  if (oattlist)
  {
    vector <FailSafeAttribute* > ompattlist = oattlist->attriList;
    result = ompattlist[0];
  }
  return result;
}

void FailSafe::Attribute::addClause(fail_safe_construct_enum clause_type)
{ 
  if (isClause(clause_type))
  {
    //We only store a clause type once
    //Logically, the content will be merged into the first occurrence.
    if (!hasClause(clause_type))
    { 
      clause_map[clause_type]=true;
      clauses.push_back(clause_type);
      //        cout<<"adding a clause:"<< OmpSupport::toString(clause_type)<<" to attr:"<< this<<endl;
      //      cout<<"clauses have member count="<<clauses.size()<<endl;
      ROSE_ASSERT(clause_type == clauses[clauses.size()-1]);
    }
  }
  else
  {
    cerr<<"FailSafe::Attribute::addClause(): Unrecognized clause type:"<<FailSafe::toString(clause_type)<<endl;
    ROSE_ASSERT(false);
  }
}

//! Get the associated SgPragmaDeclaration
SgPragmaDeclaration* FailSafe::Attribute::getPragmaDeclaration()
{
  SgPragmaDeclaration* result = NULL;
  if (mNode)
    result = isSgPragmaDeclaration(mNode);
  return result;
}

SgVariableSymbol* FailSafe::Attribute::addVariable(fail_safe_construct_enum targetConstruct, const std::string& varString, SgInitializedName* sgvar/*=NULL*/)
{
  SgVariableSymbol* symbol = NULL;

  // Try to resolve the variable if SgInitializedName is not provided
  if ((sgvar == NULL)&&(mNode!=NULL))
  {
    SgScopeStatement* scope = SageInterface::getScope(mNode);
    ROSE_ASSERT(scope!=NULL);
    //resolve the variable here
    symbol = lookupVariableSymbolInParentScopes (varString, scope);
    if (symbol == NULL)
    {
      cerr<<"Error: FailSafe::Attribute::addVariable() cannot find symbol for variable:"<<varString<<endl;
      ROSE_ASSERT(symbol!= NULL);
    }
    else
      sgvar = symbol->get_declaration();
  }

  if (sgvar != NULL)
  {
    symbol = isSgVariableSymbol(sgvar->get_symbol_from_symbol_table());
    // Liao, 3/7/2013. this may end up with infinite search through cyclic graph.
    //symbol = isSgVariableSymbol(sgvar->search_for_symbol_from_symbol_table ());
  }

  //debug clause var_list
  // if (targetConstruct== e_copyin) cout<<"debug: adding variable to copyin()"<<endl;
  variable_lists[targetConstruct].push_back(make_pair(varString, sgvar));
  // maintain the var-clause map also
  var_clauses[varString].push_back(targetConstruct);
  // Don't forget this! 
  // variable lists are mostly associated with clauses.
   //But directive like threadprivate could have variable list also
  if (isClause(targetConstruct))
    addClause(targetConstruct);
  return symbol;
}

//! Set name for named critical section
void FailSafe::Attribute::setName(const std::string & varname)
{
  hasName = true;
  name = varname;
}

//! Expression 
void FailSafe::Attribute::addExpression(fail_safe_construct_enum targetConstruct, const std::string& expString, SgExpression* sgexp/* =NULL */)
{
  expressions[targetConstruct]=make_pair(expString,sgexp);
  if (sgexp!=NULL)
    sgexp->set_parent(mNode); // a little hack here, we not yet extend the SgPragmaDeclaration to have expression children.
}

std::pair<std::string, SgExpression*> FailSafe::Attribute::getExpression(fail_safe_construct_enum targetConstruct)
{
  return expressions[targetConstruct];
}

void FailSafe::setSpecifier (fail_safe_construct_enum valuex)
{
  switch (valuex)
  { 
    case e_pre:
    case e_post:
      hasSpecifier = true;
      specifier = valuex;
      break;
    default: 
     cerr<<"FailSafe::setSpecifier() Illegal specifier value:"<<valuex<<endl
     ROSE_ASSERT(false)
  }
}

void FailSafe::setErrorType(fail_safe_construct_enum valuex)
{
  switch (valuex)
  { 
    case e_ET1:
    case e_ET2:
      error_type= valuex;
      break;
    default: 
     cerr<<"FailSafe::setErrorType() Illegal value:"<<valuex<<endl
     ROSE_ASSERT(false)
  }
}

void FailSafe::setViolationType(fail_safe_construct_enum valuex)
{
  switch (valuex)
  { 
    case e_NaN:
    case e_SECDED:
    case e_SEGFAULT:
    case e_ANY:
      violation_type= valuex;
      break;
    default: 
     cerr<<"FailSafe::setViolationType() Illegal value:"<<valuex<<endl
     ROSE_ASSERT(false)
  }
}

  //! Find the relevant clauses for a variable 
std::vector<enum omp_construct_enum>
    FailSafe::Attribute::get_clauses(const std::string& variable)
{
  return var_clauses[variable];
}

bool FailSafe::Attribute::hasClause(fail_safe_construct_enum fs_type)
{
  bool result = false;
  if (isClause(fs_type))
    result =clause_map[fs_type];
  else
  {        
    cerr<<"FailSafe::Attribute::hasClause(): Unrecognized clause type as a parameter:"<<fs_type<<endl;
    ROSE_ASSERT(false);
  }
  return result;
}

  //Judge if a name is in a clause's variable list: like save(x,y,z), 
  bool FailSafe::Attribute::isInConstruct(const string & varName, enum fail_safe_construct_enum fstype)
  {
    bool result =false;
    ROSE_ASSERT(varName.size()!= 0);
    vector <fail_safe_construct_enum> construct_vec = get_clauses(varName);
    vector <fail_safe_construct_enum>::iterator iter;
    iter = find (construct_vec.begin(), construct_vec.end(),fstype);
    if (iter != construct_vec.end())
      result = true;
    return result;
  }

//! Convert directives and clauses to string 
  // Top down recursive conversion: directive ,directive-optional stuff, 
  // then clauses one by one, plus clause-optional stuff
  std::string FailSafe::FailSafeAttribute::toFailSafeString(fail_safe_construct_enum fs_type)
  { 
    string result;
    //Directives ------------------
    if (isDirective(fs_type))
    {
      //common string for all directives
      // region, status, data, tolerance, double/triple_redundancy, save, 
      result += toString(fs_type);

      // extra stuff associated with a directive, but not clauses
     if (fs_type == e_region) //TODO prepare for named region later
     {
       if (isNamed())
         result+=" ("+ getName()+")";
     }
     else
     if (fs_type == e_save)
     {
       if(hasVariableList(fs_type))
        {
          string varListString = toFailSafeString(getVariableList(fs_type));
          result+=" (" + varListString + ")";
        }
     }
    } // end if directive
    else if () // TODO: handle clauses one by one

    return result;
  }
  //! Convert a variable list to x,y,z ,without parenthesis.
  std::string FailSafe::FailSafeAttribute::toOpenMPString(std::vector<std::pair<std::string,SgNode* > >var_list)
  {
    string result;
    std::vector<std::pair<std::string,SgNode* > >::iterator iter;
    for (iter=var_list.begin();iter!=var_list.end();iter++)
    {
      if (iter != var_list.begin())
        result +=",";
      result+=(*iter).first;
      // For map (a[0:n], b[0:m],c), a variable may have optional list of dimension information
      SgInitializedName* initname = isSgInitializedName((*iter).second);
      if (initname != NULL)
      {
       SgVariableSymbol * sym = isSgVariableSymbol(initname->get_symbol_from_symbol_table());
       ROSE_ASSERT (sym != NULL);
#if 0 //TODO: we should have similar syntax for array variables also
       for ( std::vector < std::pair <SgExpression*, SgExpression*> >::const_iterator citer = dims.begin(); citer!= dims.end(); citer ++)
       {
         result+="[";
         std::pair <SgExpression*, SgExpression*> c_pair = (*citer);
         result += c_pair.first->unparseToString();
         result += ":"; 
         result += c_pair.second->unparseToString();
         result+="]";
       } 
#endif
      }
    }
    return result;
  }

//! Get the variable list associated with a construct
  std::vector<std::pair<std::string,SgNode* > >
FailSafe::FailSafeAttribute::getVariableList(fail_safe_construct_enum targetConstruct)
{
  std::vector<std::pair<std::string,SgNode* > > * result = new std::vector<std::pair<std::string,SgNode* > >;
  return variable_lists[targetConstruct];
}


  // interface to convert FailSafeAttribute to FailSafe string format, different from FailSafeAttribute::toString()
  std::string FailSafe::FailSafeAttribute::toFailSafeString()
  {
    string result; // Should not add "#pragma ", which is automatically generated by SgPragmaDeclaration
    // Convert directive first
    result += toFailSafeString(fs_type);

    // Convert clauses then
    vector<fail_safe_construct_enum> clause_vector = getClauses();
    vector<fail_safe_construct_enum>::iterator iter;
    for (iter=clause_vector.begin();iter!=clause_vector.end();iter++)
    {
      if (iter==clause_vector.begin())
        result+= " "; // space between directive and first clause
      else
        result+= " "; //avoid using ',' since GCC 4.2.x is picky about this
      result+= toFailSafeString(*iter);
    }
    return result;
  }

std::string FailSafe::AttributeList::toFailSafeString()
{
  string result;
  std::vector<FailSafeAttribute*>::const_iterator citer;
  for (citer= attriList.begin(); citer != attriList.end(); citer++)
  {
    FailSafeAttribute* attribute = *citer;
    result += attribute->toFailSafeString();
  }
  return result;
}

FailSafe::AttributeList::~FailSafe::AttributeList()
{
  std::vector<FailSafeAttribute*>::const_iterator citer;
  for (citer= attriList.begin(); citer != attriList.end(); citer++)
  {
    FailSafeAttribute* attribute = *citer;
    delete attribute;
  }
}


//TODO  bool isDirectiveWithBody(omp_construct_enum omp_type
