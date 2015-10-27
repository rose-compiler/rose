#include "sage3basic.h"
#include "sageBuilder.h"
#include "failSafePragma.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace AstFromString;

namespace FailSafe{
  // an internal data structure to avoid redundant AST traversal to find failsafe pragmas
  static std::list<SgPragmaDeclaration* > pragma_list;
}

string FailSafe::toString(fail_safe_enum fs_type)
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
     case e_error:   result = "error"; break;
     
     // special valued clause: 
     case e_pre:   result = "pre"; break;
     case e_post:   result = "post"; break;

    // values of some clauses
     case e_type_ET1:   result = "ET1"; break;
     case e_type_ET2:   result = "ET2"; break;
  
     
     case e_violation_NaN:   result = "NaN"; break;
     case e_violation_SECDED:   result = "SECDED"; break;
     case e_violation_SEGFAULT:   result = "SEGFAULT"; break;
     case e_violation_ANY:   result = "ANY"; break;
  
     default: 
       cerr<<"Error: unhandled failsafe construct within FailSafe::toString()."<<endl;
       ROSE_ASSERT (false);
       break;
  }
  return result; 
}

bool FailSafe::isDirective (fail_safe_enum fs_type)
{
  bool result = false;
  switch (fs_type)
  {
    case e_region:
    case e_status_predicate:
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

bool FailSafe::isClause(fail_safe_enum fs_type)
{
  bool result = false;
  switch (fs_type)
  {
    case e_assert:
    case e_specifier:
    case e_error:
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
void FailSafe::process_fail_safe_directives (SgSourceFile *sageFilePtr)
{
  if (SgProject::get_verbose() > 1)
  {
    printf ("Processing FAIL-SAFE directives \n");
  }
  ROSE_ASSERT(sageFilePtr != NULL);
  
  //1. Parse directives and generate attribute information
   parse_directives_to_attributes(sageFilePtr);
  //2. Build dedicate AST nodes for failsafe directives 

  //3. Translate failsafe AST nodes 

}

void FailSafe::Attribute::init()
{
  bName = false;
}

FailSafe::Attribute * FailSafe::buildAttribute (FailSafe::fail_safe_enum fstype, SgNode* node)
{
  Attribute* result = new Attribute (fstype, node);
  ROSE_ASSERT (result);
  return result;
}

void FailSafe::addAttribute (FailSafe::Attribute* attribute, SgNode* node)
{
  ROSE_ASSERT(node);
  ROSE_ASSERT(attribute);
  SgLocatedNode * lnode = isSgLocatedNode(node);
  ROSE_ASSERT (lnode != NULL);

  AttributeList* cur_list = getAttributeList (node);
  if (! cur_list)
  {
    cur_list = new AttributeList();
    node->addNewAttribute("FailSafeAttributeList",cur_list);
  }

  cur_list->attriList.push_back(attribute);
}

//! Remove Attribute from a SgNode's attributeList
void FailSafe::removeAttribute(FailSafe::Attribute* attribute, SgNode* node)
{
  ROSE_ASSERT(node);
  ROSE_ASSERT(attribute);
  ROSE_ASSERT (attribute->getNode() == node);

  AttributeList* cur_list =  getAttributeList(node);
  ROSE_ASSERT (cur_list != NULL);
  // (cur_list->attriList);
  vector <Attribute* >::iterator h_pos = find ((cur_list->attriList).begin(), (cur_list->attriList).end(), attribute);
  ROSE_ASSERT (h_pos != (cur_list->attriList).end());
  (cur_list->attriList).erase (h_pos);
  if ((cur_list->attriList).size() ==0)
  {
    delete cur_list;
    node->removeAttribute("FailSafeAttributeList");
  }
}

//! get omp enum from an OpenMP pragma attached with OmpAttribute
FailSafe::fail_safe_enum FailSafe::getConstructEnum(SgPragmaDeclaration* decl)
{
  ROSE_ASSERT (decl != NULL);
  AttributeList* oattlist= getAttributeList(decl);
  ROSE_ASSERT (oattlist != NULL) ;
  vector <Attribute* > ompattlist = oattlist->attriList;
  // There should be only one Attribute attached to each pragma
  ROSE_ASSERT (ompattlist.size() == 1) ;
  vector <Attribute* >::iterator i = ompattlist.begin();
  Attribute* oa = *i;
  fail_safe_enum fs_type= oa->getDirectiveType();
  //   The first attribute should always be the directive type
  ROSE_ASSERT(isDirective(fs_type));
  return fs_type;
}

//! Get Attribute from a SgNode
FailSafe::AttributeList* FailSafe::getAttributeList(SgNode* node)
{
  AttributeList* result = NULL;
  AstAttribute* astattribute=node->getAttribute("FailSafeAttributeList");
  if (astattribute)
    result = dynamic_cast<AttributeList* > (astattribute);
  return result;
}

//! Get the first Attribute from a SgNode, return NULL if not found
FailSafe::Attribute* FailSafe::getAttribute(SgNode* node)
{
  ROSE_ASSERT (node != NULL );
  Attribute* result = NULL;
  AttributeList* oattlist= getAttributeList(node);
  if (oattlist)
  {
    vector <Attribute* > ompattlist = oattlist->attriList;
    result = ompattlist[0];
  }
  return result;
}

void FailSafe::Attribute::addClause(fail_safe_enum clause_type)
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

SgVariableSymbol* FailSafe::Attribute::addVariable(fail_safe_enum targetConstruct, const std::string& varString, SgInitializedName* sgvar/*=NULL*/)
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
  bName = true;
  name = varname;
}

//! Expression 
void FailSafe::Attribute::addExpression(fail_safe_enum targetConstruct, const std::string& expString, SgExpression* sgexp/* =NULL */)
{
  expressions[targetConstruct]=make_pair(expString,sgexp);
  if (sgexp!=NULL)
    sgexp->set_parent(mNode); // a little hack here, we not yet extend the SgPragmaDeclaration to have expression children.
}

std::pair<std::string, SgExpression*> FailSafe::Attribute::getExpression(fail_safe_enum targetConstruct)
{
  return expressions[targetConstruct];
}

void FailSafe::Attribute::setSpecifierValue (FailSafe::fail_safe_enum valuex)
{
  switch (valuex)
  { 
    case e_pre:
    case e_post:
      specifier = valuex;
      break;
    default: 
     cerr<<"FailSafe::setSpecifier() Illegal specifier value:"<<valuex<<endl;
     ROSE_ASSERT(false);
  }
}

void FailSafe::Attribute::setErrorType(FailSafe::fail_safe_enum valuex)
{
  switch (valuex)
  { 
    case e_type_ET1:
    case e_type_ET2:
      error_type= valuex;
      break;
    default: 
     cerr<<"FailSafe::setErrorType() Illegal value:"<<valuex<<endl;
     ROSE_ASSERT(false);
  }
}

void FailSafe::Attribute::setViolationType(FailSafe::fail_safe_enum valuex)
{
  switch (valuex)
  { 
    case e_violation_NaN:
    case e_violation_SECDED:
    case e_violation_SEGFAULT:
    case e_violation_ANY:
      violation_type= valuex;
      break;
    default: 
     cerr<<"FailSafe::setViolationType() Illegal value:"<<valuex<<endl;
     ROSE_ASSERT(false);
  }
}

  //! Find the relevant clauses for a variable 
std::vector<enum FailSafe::fail_safe_enum>
    FailSafe::Attribute::get_clauses(const std::string& variable)
{
  return var_clauses[variable];
}

bool FailSafe::Attribute::hasClause(FailSafe::fail_safe_enum fs_type)
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
  bool FailSafe::Attribute::isInConstruct(const string & varName, enum FailSafe::fail_safe_enum fstype)
  {
    bool result =false;
    ROSE_ASSERT(varName.size()!= 0);
    vector <fail_safe_enum> construct_vec = get_clauses(varName);
    vector <fail_safe_enum>::iterator iter;
    iter = find (construct_vec.begin(), construct_vec.end(),fstype);
    if (iter != construct_vec.end())
      result = true;
    return result;
  }

//! Convert individual directives and clauses to string 
// This is called by FailSafe::Attribute::toFailSafeString() 
// Note that the input parameter may be totally different from the directive type of the current attribute!
  std::string FailSafe::Attribute::toFailSafeString( FailSafe::fail_safe_enum in_type)
  { 
    string result;
    //Directives ------------------
    if (isDirective(in_type))
    {
      //common string for all directives
      // region, status, data, tolerance, double/triple_redundancy, save, 
      result += FailSafe::toString(in_type);

      // extra stuff associated with a directive, but not clauses
      if (in_type == e_region) //TODO prepare for named region later
      {
        if (isNamed())
          result+=" ("+ getName()+")";
      }
      else
        if (in_type == e_save)
        {
          if(hasVariableList(in_type))
          {
            string varListString = toFailSafeString(getVariableList(in_type));
            result+=" (" + varListString + ")";
          }
        }
    } // end if directive
     else if (in_type == e_assert) // handle clauses one by one
     {
        result += FailSafe::toString(in_type);
        SgExpression* exp = getExpression(FailSafe::e_assert).second;
        ROSE_ASSERT (exp != NULL);
        result += " (";
        result += exp->unparseToString();
        result += ") ";
     }
     else if (in_type == e_specifier)
     {
        result += FailSafe::toString (specifier);
     }
     else if (in_type == e_error)
     {
        result += " error(";
        result +=  FailSafe::toString (getErrorType() );
        result += ") ";
     }
     else if (in_type == e_recover)
     {
         result += " recover(";
         result += isSgFunctionRefExp(getRecoveryFunc())->unparseToString();
         result += ",";
         result += isSgExprListExp(getRecoveryArgList())->unparseToString();
        result += ") ";
     }
     else
     {
       cerr<<"Error. Unhandled enum type in FailSafe::Attribute::toFailSafeString (intype):"<<in_type<<endl;
       ROSE_ASSERT (false);
     }

    return result;
  }
  //! Convert a variable list to x,y,z ,without parenthesis.
  std::string FailSafe::Attribute::toFailSafeString(std::vector<std::pair<std::string,SgNode* > >var_list)
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
FailSafe::Attribute::getVariableList(FailSafe::fail_safe_enum targetConstruct)
{
//  std::vector<std::pair<std::string,SgNode* > > * result = new std::vector<std::pair<std::string,SgNode* > >;
  return variable_lists[targetConstruct];
}

  //! Check if a variable list is associated with a construct
  bool FailSafe::Attribute::hasVariableList(fail_safe_enum fs_type)
  {
    vector<std::pair<std::string,SgNode* > > var_list = variable_lists[fs_type];
    return (var_list.size()!=0);
  }


  // interface to convert Attribute to FailSafe string format, different from Attribute::toString()
  std::string FailSafe::Attribute::toFailSafeString()
  {
    string result; // Should not add "#pragma ", which is automatically generated by SgPragmaDeclaration
    // Convert directive first
    result += toFailSafeString(fs_type);

    // Convert clauses then
    vector<fail_safe_enum> clause_vector = getClauses();
    vector<fail_safe_enum>::iterator iter;
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
  std::vector<Attribute*>::const_iterator citer;
  for (citer= attriList.begin(); citer != attriList.end(); citer++)
  {
    Attribute* attribute = *citer;
    result += attribute->toFailSafeString();
  }
  return result;
}

FailSafe::AttributeList::~AttributeList()
{
  std::vector<Attribute*>::const_iterator citer;
  for (citer= attriList.begin(); citer != attriList.end(); citer++)
  {
    Attribute* attribute = *citer;
    delete attribute;
  }
}


//TODO  bool isDirectiveWithBody(omp_construct_enum omp_type


//! Parse failsafe directives and generate/attach attributes
// follow the code in void attachOmpAttributeInfo(SgSourceFile *sageFilePtr) of ompAstConstruction.cpp
void FailSafe::parse_directives_to_attributes(SgSourceFile* sageFilePtr)
{
  ROSE_ASSERT(sageFilePtr != NULL);
  //TODO check language extension flag for turning on this support
  //if (sageFilePtr->get_failsafe() == false) return

  if (sageFilePtr->get_Fortran_only()||sageFilePtr->get_F77_only()||sageFilePtr->get_F90_only()|
    sageFilePtr->get_F95_only() || sageFilePtr->get_F2003_only())
  {
     //TODO handle Fortran later
  }
  else
  {
    // For C/C++, search pragma declarations for failsafe directives
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
        if (key == "failsafe")
        {
          FailSafe::AttributeList * previous = FailSafe::getAttributeList (pragmaDeclaration);
          FailSafe::pragma_list.push_back(pragmaDeclaration);
          if (previous == NULL)
          {
             FailSafe::Attribute* attribute = parse_fail_safe_directive(pragmaDeclaration);
             FailSafe::addAttribute(attribute, pragmaDeclaration);
          } // end if NULL
        } // end if key is failsafe

       } // end  for all_pragmas   
  } // end if 
}


//! Internal helper function to match assert(expression), no side effects if no match is found
static bool afs_match_assert (SgExpression** e1)
{
  if (!afs_match_substr("assert"))
    return false;

  if (!afs_match_char('('))
  {
    printf ("Error: expecting ( after parsing assert \n");
    assert (false);
  }
  // mandatory expression
  if (!afs_match_assignment_expression())
  {
    printf ("Error: expecting an expression after parsing assert(\n");
    assert (false);
  }

  // retrieve the obtained expression AST piece
  assert (c_parsed_node != NULL);
  assert (isSgExpression(c_parsed_node) != NULL);
  *e1 = isSgExpression(c_parsed_node);

  if (!afs_match_char(')'))
  {
    printf ("Error: expecting ) after parsing assert(exp \n");
    assert (false);
  }
  return true;
}

//! Internal helper function to match error(error_type), no side effects if no match is found
static bool afs_match_error(FailSafe::fail_safe_enum* etype)
{
  if (!afs_match_substr("error"))
    return false;

  if (!afs_match_char('('))
  {
    printf ("Error: expecting ( after parsing error \n");
    assert (false);
  }

  //parse error_type
  //TODO: use real type names later
  if (afs_match_substr("ET1"))
    *etype = FailSafe::e_type_ET1; 
  else if (afs_match_substr("ET2"))  
    *etype = FailSafe::e_type_ET1; 
  else
  {
    printf ("Error: expecting error_type after parsing error( \n");
    assert (false);
  }  

  if (!afs_match_char(')'))
  {
    printf ("Error: expecting ) after parsing assert(ETYPE \n");
    assert (false);
  }
  return true;
}

//! Internal helper function to match recover (Func,para1,para2), no side effects if no match is found
static bool afs_match_recover(FailSafe::Attribute *result)
{
  SgFunctionRefExp * func_ref = NULL; 
  SgExprListExp* arg_list = NULL; 

  if (!afs_match_substr("recover"))
    return false;

  if (!afs_match_char('('))
  {
    printf ("Error: expecting ( after parsing recover \n");
    assert (false);
  }

  //parse function name 
  if (afs_match_identifier())
  {
    ROSE_ASSERT(isSgFunctionRefExp(c_parsed_node)); // expect a function ref expression here
    func_ref = isSgFunctionRefExp(c_parsed_node); 
    if (!afs_match_char(','))
    {
      printf ("Error: expecting ( after parsing recover \n");
      assert (false);
    }
    // actual parameter list
    if (afs_match_argument_expression_list())
    {
      ROSE_ASSERT(isSgExprListExp(c_parsed_node));
      arg_list = isSgExprListExp(c_parsed_node); 
    }  
  }
  else
  {
    printf ("Error: expecting a function name after parsing recover ( \n");
    assert (false);
  }  

  if (!afs_match_char(')'))
  {
    printf ("Error: expecting ) after parsing recover (func, .. \n");
    assert (false);
  }
  // save parsed information to result only when completing the whole thing
  result->addClause (FailSafe::e_recover);
  result->setRecoveryFunc(func_ref);
  func_ref->set_parent( c_sgnode); // set a fake parent here to enable correct unparsing
  result->setRecoveryArgList(arg_list);
  return true;
}

//! Match the part from assert to the rest for either status or data predicate
// assert-clause specifier_opt region-reference-clause_opt error-classification_opt recovery-specification_opt 
static bool afs_match_predicate_and_more (FailSafe::Attribute * result)
{
  // try to match assert (exp)  clause
  SgExpression * assert_exp = NULL; 
  if (afs_match_assert(&assert_exp))
  {
    result->addClause (FailSafe::e_assert);
    result->addExpression(FailSafe::e_assert, assert_exp->unparseToString(), assert_exp);
    // match optional specifier clause
    if (afs_match_substr("pre"))
    {
      result->addClause(FailSafe::e_specifier);
      result->setSpecifierValue (FailSafe::e_pre);
    }
    else if (afs_match_substr("post"))
    {
      result->addClause(FailSafe::e_specifier);
      result->setSpecifierValue (FailSafe::e_post);
    }  

    //match optional region-reference-clause
    //TODO, list of labels

    // match error-classification-clause
    FailSafe::fail_safe_enum etype;
    if (afs_match_error (& etype))
    {
      result->addClause (FailSafe::e_error);
      result->setErrorType(etype);
    }
    // match optional recovery clause
    afs_match_recover (result);
  }
  else
    return false;

  return true; 
}


//! A recursive descendent parser for the pragma string
// Follow the example of projects/pragmaParsing/hcpragma.C
FailSafe::Attribute* FailSafe::parse_fail_safe_directive (SgPragmaDeclaration* pragmaDecl)
{
  Attribute* result = NULL;
  
  assert (pragmaDecl != NULL);
  assert (pragmaDecl->get_pragma() != NULL);
  string pragmaString = pragmaDecl->get_pragma()->get_pragma();
  // make sure it is side effect free
  const char* old_char = c_char;
  SgNode* old_node = c_sgnode;

  c_sgnode = getNextStatement(pragmaDecl);
  //A pragma can show up in front of the structured block affected 
  // or follow declarations it relies on (as data predicate)
  // MS: 11/22/2015 removed ';' from "if (c_sgnode == NULL);"
  if (c_sgnode == NULL)
    c_sgnode = pragmaDecl; // use the pragma as the anchor AST to enable scope search
  assert (c_sgnode != NULL);
    
  c_char = pragmaString.c_str();

  // start real parsing
  if (afs_match_substr("failsafe"))
  {
     if (afs_match_substr("region"))
     {
       result = buildAttribute(e_region, pragmaDecl); 
     }
     else if (afs_match_substr("status")) 
     {
       result = buildAttribute(e_status_predicate, pragmaDecl); 
       if (!afs_match_predicate_and_more (result))
       {
         cerr<<"Error: FailSafe::parse_fail_safe_directive() expect status assert() etc. but facing:"<< c_char<<endl;
         ROSE_ASSERT (false);
       }
     }
     else if (afs_match_substr("data")) 
     {
       result = buildAttribute(e_data_predicate, pragmaDecl); 
       if (!afs_match_predicate_and_more (result))
       {
         cerr<<"Error: FailSafe::parse_fail_safe_directive() expect data assert() etc. but facing:"<< c_char<<endl;
         ROSE_ASSERT (false);
       }
     }
     else if (afs_match_substr("tolerance") ) 
     {
       result = buildAttribute(e_tolerance, pragmaDecl); 
     }
     else if (afs_match_substr("double_redundancy")) 
     {
       result = buildAttribute(e_double_redundancy, pragmaDecl); 
     }
     else if (afs_match_substr("triple_redundancy")) 
     {
       result = buildAttribute(e_triple_redundancy, pragmaDecl); 
     }
     else if (afs_match_substr("save")) 
     {
       result = buildAttribute(e_save, pragmaDecl); 
     }
     else
     {
       cerr<<"Error: FailSafe::parse_fail_safe_directive() expect keyword failsafe but facing:"<< c_char<<endl;
       ROSE_ASSERT (false);
     }
     
  }
  else
  {
    cerr<<"Error: FailSafe::parse_fail_safe_directive() expect keyword failsafe but facing:"<< c_char <<endl;
    ROSE_ASSERT (false);
  } 

  // undo side effects
  c_char = old_char;
  c_sgnode = old_node;
  ROSE_ASSERT (result != NULL);
  return result;

}
