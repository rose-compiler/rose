// A hand-crafted OpenMP parser for Fortran comments within a SgSourceFile
// It only exposes a few interface functions: 
//   omp_fortran_parse()
// void parse_fortran_openmp(SgSourceFile *sageFilePtr) // preferred
//
// All other supporting functions should be declared with "static" (file scope only)
// Liao, 5/24/2009

// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "sageBuilder.h"
#include "OmpAttribute.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

using namespace std;

// maximum length for a buffer for a variable, constant, or OpenMP construct name
#define OFS_MAX_LEN 256
using namespace OmpSupport;
// Parse a SgNode's OpenMP style comments (str) into OmpAttribute
OmpSupport::OmpAttribute* omp_fortran_parse(SgNode* locNode, const char* str);
void parse_fortran_openmp(SgSourceFile *sageFilePtr);

// A file scope char* to avoid passing and returning target c string for every and each function
static const char* c_char = NULL; // current characters being scanned
static SgNode* c_sgnode = NULL; // current SgNode associated with the OpenMP directive
static OmpSupport::OmpAttribute* ompattribute = NULL; // the current attribute (directive) being built
static omp_construct_enum omptype= e_unknown; // the current clause (or reduction operation type) being filled out
static SgExpression * current_exp = NULL; // the current expression AST tree being built

//--------------omp fortran scanner (ofs) functions------------------
//
// note: ofs_skip_xxx() optional skip 0 or more patterns
//       ofs_match_xxx() try to match a pattern, undo side effect if failed.

//!Skip 0 or more whitespace or tabs
static bool ofs_skip_whitespace()
{
  bool result = false;
  while ((*c_char)==' '||(*c_char)=='\t')
  {
    c_char++;
    result= true;
  }
  return result;
}

//! Match a given character after skip any possible space/tab
//  advance the header if successful, undo side effect if fails
static bool ofs_match_char(char c)
{
  bool result = false;
  const char* old_char = c_char;

  if (strlen(c_char)==0)
    result = false;
  else
  {
    ofs_skip_whitespace();
    if (*c_char == c)
      result = true;
    else
      result = false;
  }

  // advance once matched
  if (result)
    c_char++;
  else // revert the possible side effect for skip_whitespace()
    c_char = old_char;    
  return result;
}

//! Match a given sub c string from the input c string, again skip heading space/tabs if any
//  checkTrail: Check the immediate following character after the match, it must be one of
//      whitespace, end of str, newline, tab, or '!'
//      Set to true by default, used to ensure the matched substr is a full identifier/keywords.
//
//      But Fortran OpenMP allows blanks/tabs to be ignored between certain pair of keywords:
//      e.g: end critical == endcritical  , parallel do == paralleldo
//      to match the 'end' and 'parallel', we have to skip trail checking.
// return values: 
//    true: find a match, the current char is pointed to the next char after the substr
//    false: no match, the current char is intact

static bool ofs_match_substr(const char* substr, bool checkTrail = true)
{
  bool result = true;
  const char* old_char = c_char;
  // we skip leading space from the target string
  ofs_skip_whitespace();
  size_t len =strlen(substr);
  for (size_t i =0; i<len; i++)
  {
    if ((*c_char)==substr[i])
    {
      c_char++;
    }
    else
    {
      result = false;
      c_char = old_char;
      break;
    }
  }
  // handle the next char after the substr match: 
  // could only be either space or \n, \0, \t, !comments
  // or the match is revoked, e.g: "parallel1" match sub str "parallel" but 
  // the trail is not legal
  // TODO: any other characters?
  if (checkTrail)
  {
    if (*c_char!=' '&&*c_char!='\0'&&*c_char!='\n'&&*c_char!='\t' &&*c_char!='!')
    {
      result = false;
      c_char = old_char;
    }
  }
  return result;
}

//! Check if the current Fortran SgFile has fixed source form
static bool isFixedSourceForm()
{
  bool result = false;
  SgFile * file = SageInterface::getEnclosingFileNode(c_sgnode);
  ROSE_ASSERT(file != NULL);

  // Only make sense for Fortran files
  ROSE_ASSERT (file->get_Fortran_only());
  if (file->get_inputFormat()==SgFile::e_unknown_output_format )
  { // default case: only f77 has fixed form
    if (file->get_F77_only())
      result = true;
    else 
      result = false;
  }
  else // explicit case: any Fortran could be set to fixed form 
  {
    if (file->get_inputFormat()==SgFile::e_fixed_form_output_format)
      result = true;
    else
      result = false;
  }
  return result;
}

//Check if the current c string starts with one of the legal OpenMP directive sentinels.
// Two cases: 
//    fixed source form: !$omp | c$omp | *$omp , then whitespace, continuation apply to the rest
//    free source form: !$omp only
static bool ofs_is_omp_sentinels()
{
  bool result = false;
  // two additional case for fixed form
  if (isFixedSourceForm())
  {
    if (ofs_match_substr("c$omp")||ofs_match_substr("*$omp"))
      result = true;
  }
  // a common case for all situations
  if (ofs_match_substr("!$omp"))
    result = true;
  return result;
}

//! Copy an input Fortran-style comment str and convert all characters to lower case if possible
// We do this to easy scanning and parsing since Fortran is case-insensitive. 
// TODO: how to handle symbol match later on?
static char* ofs_copy_lower(const char* str)
{
  char* strlower = (char*)malloc(sizeof(char)*strlen(str)+1);
  strcpy(strlower, str);
  size_t i;
  for (i=0; i< strlen(strlower); i++)
  {
    strlower[i]= tolower(strlower[i]);
  }
  strlower[strlen(str)]='\0';
  return strlower;
}

// A set of helper functions for recognizing legal Fortran identifier
//! Check if the head char is a digit
static bool ofs_is_digit()
{
  return(( *c_char>='0') &&(*c_char<='9'));
}

static bool ofs_is_lower_letter()
{
  return (( *c_char>='a') &&(*c_char<='z'));
}

static bool ofs_is_upper_letter()
{
  return (( *c_char>='A') &&(*c_char<='Z'));
}

static bool ofs_is_letter()
{
  return (ofs_is_lower_letter()|| ofs_is_upper_letter());
}

//Check if the current char is a legal character for a Fortran identifier,
// we allow '_' and '$' in addition to letters and digits
static bool ofs_is_identifier_char()
{
  return (ofs_is_letter()||ofs_is_digit()||(*c_char =='_')||(*c_char =='$'));
}

//Scan input c str to match an integer constant, 
//return true if successful and save the value in result
// return false otherwise, and undo side effects. 
// TODO handle sign, binary, hex format?
static bool ofs_match_integer_const(int * result)
{
  char buffer[OFS_MAX_LEN];
  const char* old_char = c_char; 
  ofs_skip_whitespace();
  // return false if the first char is not a digit
  if (!ofs_is_digit())
  {
    c_char = old_char;
    return false; 
  }

  // Now we may have digits
  int i=0; 
  do 
  {
    buffer[i]= *c_char;
    i++;
    c_char++;

  } while (ofs_is_digit()); 
  buffer[i]='\0';
 
  // check tail to ensure digit sequence is independent (not part of another identifier)
  // but it can be followed by space ',' '[' '{' etc.
  // TODO other cases??
  if (ofs_is_letter()) 
  {
    c_char = old_char;
    return false;
  }
  // convert buffer to an integer value and return
 // printf("int buffer is %s\n",buffer);
  *result = atoi(buffer);
  return true; 
}

// Try to retrieve a possible name identifier from the head
// store the result in buffer
// for example: it can be used to match a variable in a list.
static bool ofs_match_anyname(char* buffer)
{
  const char* old_char = c_char;
  ofs_skip_whitespace();
  // check for the first char
  // Must be either of letter or _
  if (!(ofs_is_letter()||(*c_char=='_')))
  {
    c_char = old_char;
    return false;
  }
  // We have a legal identifier now
  int i =0;
  do 
  {
    buffer[i] = *c_char;
    i++;
    c_char++;
  } while (ofs_is_identifier_char());

  buffer[i]= '\0';
  return true;
}

//It creates the current expression AST subtree if successful.
static bool ofs_match_expression()
{
  int resulti;
  char varRef[OFS_MAX_LEN];
  const char * old_char = c_char;

  // Integer constant value
  if (ofs_match_integer_const(&resulti))
  {
    current_exp= SageBuilder::buildIntVal(resulti);
  }
  // Variable reference 
  else if (ofs_match_anyname(varRef))
  {
    current_exp = SageBuilder::buildVarRefExp(varRef, SageInterface::getScope(c_sgnode));
  }
  else 
  {
    //TODO handle more expression types as needed
    printf("Unable to match an expression for %s\n", old_char);
    c_char = old_char;
    return false;
  }

  return true;
}

//!Match schedule(kind, [, chunk_size])
// static, dynamic, guided can have optional chunk_size
// auto and runtime don't have chunk_size
static bool ofs_match_clause_schedule()
{
  const char* old_char = c_char;
  omp_construct_enum matched_kind = e_unknown; 

  if  (ofs_match_substr("schedule",false))// no space is required after the match
  {
    ompattribute->addClause(e_schedule);
    omptype = e_schedule;

    if (!ofs_match_char('('))
    {
      printf("error in schedule(xx) match: no starting '(' is found for %s.\n",old_char);
      assert(false);
    }
    // match kind first
    if  (ofs_match_substr("static",false))// no space is required after the match 
    {
      matched_kind= e_schedule_static;
    } 
    else if  (ofs_match_substr("dynamic",false))// no space is required after the match 
    {
      matched_kind= e_schedule_dynamic;
    } 
    else if  (ofs_match_substr("guided",false))// no space is required after the match 
    {
      matched_kind= e_schedule_guided;
    } 
    else if  (ofs_match_substr("auto",false))// no space is required after the match 
    {
      matched_kind= e_schedule_auto;
    } 
    else if  (ofs_match_substr("runtime",false))// no space is required after the match 
    {
      matched_kind= e_schedule_runtime;
    } 
    else
    { 
      printf("error in schedule clause match: no legal kind is found for %s\n",old_char);
      assert(false);
    }
    //  set schedule kind matched
    ompattribute->setScheduleKind(matched_kind);

    // match optional ",chunk_size"
    if (matched_kind == e_schedule_static || matched_kind == e_schedule_dynamic
        || matched_kind == e_schedule_guided ) 
    {
      if (ofs_match_char(','))
      {
        if (ofs_match_expression())
        {
          assert (current_exp != NULL);
          ompattribute->addExpression(omptype, "", current_exp);
        }
        else
        {
          printf("error in schedule(kind,chunk) match: no chunk is found after , for %s\n",old_char);
        }
      }
    } // end match for ",chunkzise"

    // match end ')'
       if (!ofs_match_char(')'))
    {
      printf("error in schedule() match: no end ')' is found for %s.\n",old_char);
      assert(false);
    }

    // it is successful now
    return true; 
  }

  // no match, undo side effect and return false;
  c_char = old_char;
  return false;
}
// Match something like / A25_COMMON /, return the block name in the buffer
static bool ofs_match_common_block(char* buffer)
{
  const char* old_char = c_char;
  if (ofs_match_char('/'))
  {
    if (!ofs_match_anyname(buffer))
    {
      printf("error in matching a named common block: cannot find block name for %s\n", old_char);
      assert (false);
    } 

    if  (!ofs_match_char('/'))
    {
      printf("error in matching a named common block: cannot the end '/' for %s\n", old_char);
      assert (false);

    } 
    return true;
  }
  // Did not make it, revoke side effects
  c_char= old_char;
  return false;
}

// Grab all explicit? variables  declared within a common block and add them into ompattribute
static void ofs_add_block_variables (char* block_name)
{
  std::vector<SgCommonBlock*> block_vec = SageInterface::getSgNodeListFromMemoryPool<SgCommonBlock>();
  SgCommonBlockObject* found_block_object = NULL;
  for (std::vector<SgCommonBlock*>::const_iterator i = block_vec.begin();
       i!= block_vec.end();i++)
  {

    bool innerbreak = false;
    SgCommonBlock* c_block = *i;
    SgCommonBlockObjectPtrList & blockList = c_block->get_block_list();
    SgCommonBlockObjectPtrList::iterator i2 = blockList.begin();
    while (i2 != blockList.end())
    {
      string name = (*i2)->get_block_name();
      if (strcmp(block_name, name.c_str())==0)
      {
        found_block_object = *i2;
        innerbreak = true;
        break;
      }
      i2++;
    }// end while block objects

    if (innerbreak)
      break;
  } // end for all blocks

  if (found_block_object == NULL)
  {
    printf("error: cannot find a common block with a name of %s\n",block_name);
    assert(false);
  }

  // add each variable within the block into ompattribute
  SgExprListExp * explistexp = found_block_object->get_variable_reference_list ();
  assert(explistexp != NULL);
  SgExpressionPtrList& explist = explistexp->get_expressions();

  Rose_STL_Container<SgExpression*>::const_iterator exp_iter = explist.begin();
  assert (explist.size()>0); // must have variable defined 
  while (exp_iter !=explist.end())
  {
    SgVarRefExp * var_exp = isSgVarRefExp(*exp_iter);
    assert (var_exp!=NULL);
    SgVariableSymbol * symbol = isSgVariableSymbol(var_exp->get_symbol());
    assert (symbol!=NULL);
//    cout<<"adding variable:"<< symbol->get_name().getString() <<" to "<<OmpSupport::toString(omptype)<<endl;
    ompattribute->addVariable(omptype,symbol->get_name(), 
                symbol->get_declaration());
    exp_iter++;
  }

}

//! Match a variable list like  "x,y,z)"
// a common block name can also show up sometimes, like /A25_COMMON/
// It is used to collect variable list within 
//  clause(varlist) and reduction(op:varlist)
//
// Since the leading character like '(' or ':'
// has already being matched , we report error and abort if 
//  the rest var list is not matched. 
static bool ofs_match_varlist()
{
  const char* old_char =c_char;

  assert(ompattribute !=NULL);
  assert( omptype != e_unknown);
  while (1)
  {
    char buffer[OFS_MAX_LEN];
    bool isCommonblock =false;

    if (ofs_match_anyname(buffer)||(isCommonblock=ofs_match_common_block(buffer)))
    {
      if (isCommonblock)
        ofs_add_block_variables(buffer);
      else
        ompattribute->addVariable(omptype, buffer);
      // look for the next variable/or common block
      if (ofs_match_char(','))
        continue;
      // end of varlist within ()
      if (ofs_match_char(')'))
        break;
      printf("error: cannot find either , or ) after a variable match for %s\n!",old_char);
      assert(false);
    }
    else
    {
      printf("error: empty variable list not allowed for %s\n!",old_char);
      assert(false);
    }
  } // end while()
  return true;
}

//! Match a clause with a form like clause (varlist)
// 6 allowed 6 clause types are
// 	copyin (var_list)
// 	copyprivate(var-list) 
// 	firstprivate (var_list)
// 	lastprivate (var_list)
// 	private (var_list)
// 	shared (var_list)
// add clause and varlist if successful, 
//otherwise undo any side effects and return false
static bool ofs_match_clause_varlist(omp_construct_enum clausetype)
{
  const char* old_char = c_char;
  char clause_name[OFS_MAX_LEN];
  // verify and convert clause type to clause name string
  switch (clausetype)
  {
    case e_copyin:
    case e_copyprivate:
    case e_firstprivate:
    case e_lastprivate:
    case e_private:
    case e_shared:
      {
        strcpy (clause_name, OmpSupport::toString(clausetype).c_str());
        break;
      }
    default:
      {
        printf("Unaccepted clause type :%s for clause(varlist) match!\n",OmpSupport::toString(clausetype).c_str());
        assert(false);
      }
  } //end switch

  if (ofs_match_substr(clause_name,false)) // We don't check for trail here, they are handled later on
  {
    assert (ompattribute != NULL); 
    ompattribute->addClause(clausetype);
    omptype = clausetype;
    ofs_skip_whitespace();
    if (ofs_match_char('('))
    {
      ofs_match_varlist();
    } // end if '(varlist)'
    else
    {
      printf("error in clause(varlist) match: no starting '(' is found for %s.\n",old_char);
      assert(false);
    }
    return true;
  }
  else
    c_char= old_char;

  return false;
}

//! Match anyname within parenthesis, used for named "critical" and "end critical" 
static bool ofs_match_name_in_parenthesis(char name[])
{
  const char* old_char = c_char;
//  char buffer[OFS_MAX_LEN];
  if (ofs_match_char('('))
  {
    if (ofs_match_anyname(name))
    {
      if (!ofs_match_char(')'))
      {
        printf("error in (name) match: no end ')' is found for %s.\n",old_char);
        assert(false);
      }
      // successful if reach to this point
      return true;
    }
    else
    {
      printf("error in (name) match: no name is found for %s.\n",old_char);
      assert(false);
    }
  }
  c_char= old_char;
  return false;
}

//! Match clauses with expression, like 
//    collapse (expression) 
//    if(scalar-logical-expression)
//    num_threads (expression)
static bool ofs_match_clause_expression (omp_construct_enum clausetype)
{
  const char* old_char = c_char;
  char clause_name[OFS_MAX_LEN];
  // verify and convert clause type to clause name string
  switch (clausetype)
  {
    case e_collapse:
    case e_if:
    case e_num_threads:
      {
        strcpy (clause_name, OmpSupport::toString(clausetype).c_str());
        break;
      }
    default:
      {
        printf("Unaccepted clause type :%s for clause(expr) match!\n",OmpSupport::toString(clausetype).c_str());
        assert(false);
      }
  } //end switch

  if (ofs_match_substr(clause_name,false)) // We don't check for trail here, they are handled later on
  {
    assert (ompattribute != NULL);
    ompattribute->addClause(clausetype);
    omptype = clausetype;
    ofs_skip_whitespace();
    if (ofs_match_char('('))
    {
      if(ofs_match_expression())
      {
        assert(current_exp != NULL);
        ompattribute->addExpression(omptype,"", current_exp);
      }
      else
      {
        printf("error in clause(expression) match:no expression is found for %s.\n",old_char);
        assert(false);
      }
    } // end if '(expression'
    else
    {
      printf("error in clause(expression) match: no starting '(' is found for %s.\n",old_char);
      assert(false);
    }

    // match end ')'
    if (!ofs_match_char(')'))
    {
      printf("error in clause(expression) match: no end ')' is found for %s.\n",old_char);
      assert(false);
    }

    return true;
  }
  else
    c_char= old_char;

  return false;
}

static bool ofs_match_clause_collapse()
{
  return ofs_match_clause_expression(e_collapse);
}

static bool ofs_match_clause_if()
{
  return ofs_match_clause_expression(e_if);
}

static bool ofs_match_clause_num_threads()
{
  return ofs_match_clause_expression(e_num_threads);
}

//! Match 'naked' clauses: such as ordered_clause, nowait, and untied
// should be side effect free if fails
static bool ofs_match_clause_naked (omp_construct_enum clausetype)
{
  const char* old_char = c_char;
  char clause_name[OFS_MAX_LEN];
  // verify and convert clause type to clause name string
  switch (clausetype)
  {
    case e_ordered_clause:
    case e_nowait:
    case e_untied:
      {
        strcpy (clause_name, OmpSupport::toString(clausetype).c_str());
        break;
      }
    default:
      {
        printf("Unaccepted clause type :%s for a naked clause match!\n",OmpSupport::toString(clausetype).c_str());
        assert(false);
      }
  } //end switch

  // we do need to ensure a legal trail here, a space is needed after them
  if (ofs_match_substr(clause_name,true)) 
  {
    assert (ompattribute != NULL); 
    ompattribute->addClause(clausetype);
    omptype = clausetype;
    return true;
  }
  else
    c_char= old_char;
  return false;
}

//! Match default (private|firstprivate|shared|none)
static bool ofs_match_clause_default()
{
  const char* old_char = c_char;
  if  (ofs_match_substr("default",false))// no space is required after the match
  {
    ompattribute->addClause(e_default);
    if (!ofs_match_char('('))
    {
      printf("error in default(xx) match: no starting '(' is found for %s.\n",old_char);
      assert(false);
    }
    // match values
    if (ofs_match_substr("private",false))
      ompattribute->setDefaultValue(e_default_private);
    else if (ofs_match_substr("firstprivate",false))
      ompattribute->setDefaultValue(e_default_firstprivate);
    else if (ofs_match_substr("shared",false))
      ompattribute->setDefaultValue(e_default_shared);
    else if  (ofs_match_substr("none",false))
      ompattribute->setDefaultValue(e_default_none);
    else
    {
      printf("error in matching default(value):no legal value is found for %s\n", old_char);
      assert("false");
    }
    // match end ')'
    if (!ofs_match_char(')'))
    {
      printf("error in default() match: no end ')' is found for %s.\n",old_char);  
      assert(false); 
    }
    return true;
  }
  // undo side effect if no match has been found so far
  c_char = old_char;
  return false;
}

//! Match a Fortran reduction clause
// reduction({operator|intrinsic_procedure_name}:varlist)
static bool ofs_match_clause_reduction()
{
  const char* old_char = c_char;
  if (ofs_match_substr("reduction",false)) // no space etc. is needed after it
  {
    assert (ompattribute != NULL);
    ompattribute->addClause(e_reduction);
    omptype = e_unknown; // we need the reduction operator type to associate varlist, not just reduction
    if (!ofs_match_char('('))
    {
      printf("error in clause(varlist) match: no starting '(' is found for %s.\n",old_char);  
      assert(false); 
    }
    // match operator/intrinsics
    // match single character operator first
    // 3 of them in total
    if (ofs_match_char('+'))
    {
      ompattribute->setReductionOperator(e_reduction_plus);
      omptype = e_reduction_plus; /*variables are stored for each kind of operators*/
    } 
    else if (ofs_match_char('*'))
    {
      ompattribute->setReductionOperator(e_reduction_mul);
      omptype = e_reduction_mul;
    } 
    else if (ofs_match_char('-'))
    {
      ompattribute->setReductionOperator(e_reduction_minus);
      omptype = e_reduction_minus; 
    } 
    // match multi-char operator/intrinsics 
    // 9 of them in total
    // we tend to not share the enumerate types between C/C++ and Fortran operators
    else if (ofs_match_substr(".and.",false)) 
    {
      ompattribute->setReductionOperator(e_reduction_and);
      omptype = e_reduction_and; 
    } 
    else if (ofs_match_substr(".or.",false)) 
    {
      ompattribute->setReductionOperator(e_reduction_or);
      omptype = e_reduction_or; 
    } 
    else if (ofs_match_substr(".eqv.",false)) 
    {
      ompattribute->setReductionOperator(e_reduction_eqv);
      omptype = e_reduction_eqv; 
    } 
    else if (ofs_match_substr(".neqv.",false)) 
    {
      ompattribute->setReductionOperator(e_reduction_neqv);
      omptype = e_reduction_neqv; 
    } 
    else if (ofs_match_substr("max",false)) 
    {
      ompattribute->setReductionOperator(e_reduction_max);
      omptype = e_reduction_max; 
    } 
    else if (ofs_match_substr("min",false)) 
    {
      ompattribute->setReductionOperator(e_reduction_min);
      omptype = e_reduction_min; 
    } 
    else if (ofs_match_substr("iand",false)) 
    {
      ompattribute->setReductionOperator(e_reduction_iand);
      omptype = e_reduction_iand; 
    } 
    else if (ofs_match_substr("ior",false)) 
    {
      ompattribute->setReductionOperator(e_reduction_ior);
      omptype = e_reduction_ior; 
    }
    else  if (ofs_match_substr("ieor",false)) 
    {
      ompattribute->setReductionOperator(e_reduction_ieor);
      omptype = e_reduction_ieor; 
    } else
    {
      printf("error: cannot find a legal reduction operator for %s\n",old_char);
      assert(false);
    }

    // match ':' in between
    if (!ofs_match_char(':'))
    {
      printf("error in reduction(op:varlist) match: no ':' is found for %s\n",old_char);
      assert(false);
    }	
    // match the rest "varlist)"
    if (!ofs_match_varlist())
    {
      printf("error in reduction(op:valist) match during varlist for %s \n",old_char);
      assert(false);
    }
    else
    {
      omptype = e_unknown; // restore it to unknown
      return true; // all pass! return here!
    }

  } // end if (reduction)

  c_char = old_char;
  return false;
}

//! Match an end of an OpenMP directive:
//   find optional whitespace, '\n' , '!', or '\0'
//   since we extract the directive string and try to parse it.
static bool ofs_match_omp_directive_end()
{
  const char* old_char = c_char;
  //skip space
  ofs_skip_whitespace();

  //skip all comments until new line
  if (*c_char =='!')
  {
    do
      c_char++;
    while (*c_char!='\n');
  }

  if (*c_char =='\0')
    return true;

  // must be new line
  if (*c_char== '\n')
  {
    c_char++;
    return true;
  }
  else
  { // undo sideeffect 
    printf("warning: unacceptable end char for an OpenMP directive:%d-%c\n",*c_char, *c_char);
    c_char = old_char;
    return false;
  }
}


//a bit vector for all possible OpenMP Fortran clauses: 
//  15 in total for OpenMP 3.0
// in alphabet order
#define BV_CLAUSE_COLLAPSE	(1<<0)
#define BV_CLAUSE_COPYIN 	(1<<1)
#define BV_CLAUSE_COPYPRIVATE 	(1<<2)
#define BV_CLAUSE_DEFAULT	(1<<3)
#define BV_CLAUSE_FIRSTPRIVATE 	(1<<4)
#define BV_CLAUSE_IF		(1<<5)
#define BV_CLAUSE_LASTPRIVATE	(1<<6)
#define BV_CLAUSE_NOWAIT 	(1<<7)
#define BV_CLAUSE_NUM_THREADS 	(1<<8)
#define BV_CLAUSE_ORDERED	(1<<9)
#define BV_CLAUSE_PRIVATE 	(1<<10)
#define BV_CLAUSE_REDUCTION 	(1<<11)
#define BV_CLAUSE_SCHEDULE	(1<<12)
#define BV_CLAUSE_SHARED 	(1<<13)
#define BV_CLAUSE_UNTIED 	(1<<14)

// common bit vector values for some directive's allowed clauses
#define BV_OMP_PARALLEL_CLAUSES \
( BV_CLAUSE_COPYIN \
| BV_CLAUSE_DEFAULT \
| BV_CLAUSE_FIRSTPRIVATE \
| BV_CLAUSE_IF \
| BV_CLAUSE_NUM_THREADS \
| BV_CLAUSE_PRIVATE \
| BV_CLAUSE_REDUCTION \
| BV_CLAUSE_SHARED ) 

#define BV_OMP_DO_CLAUSES \
( BV_CLAUSE_COLLAPSE \
|BV_CLAUSE_FIRSTPRIVATE \
|BV_CLAUSE_LASTPRIVATE \
|BV_CLAUSE_ORDERED \
|BV_CLAUSE_PRIVATE \
|BV_CLAUSE_REDUCTION \
|BV_CLAUSE_SCHEDULE ) 

#define BV_OMP_SECTIONS_CLAUSES \
(BV_CLAUSE_FIRSTPRIVATE\
|BV_CLAUSE_LASTPRIVATE \
|BV_CLAUSE_PRIVATE \
|BV_CLAUSE_REDUCTION)
 
#define BV_OMP_TASK_CLAUSES \
(BV_CLAUSE_DEFAULT \
|BV_CLAUSE_FIRSTPRIVATE \
|BV_CLAUSE_IF \
|BV_CLAUSE_PRIVATE \
|BV_CLAUSE_SHARED \
|BV_CLAUSE_UNTIED )


//! Try to match a set of clauses indicated by a bit vector
//
// e.g: private(list), firstprivate(list), lastprivate(list)
// reduction({operator|intrinsic_procedure}:list)
// schedule(kinkd[,chunk_size])
// collapse (n)
// ordered, etc.
//
// They might have space or comma in between
static bool ofs_match_omp_clauses(int bitvector)  
{
  const char* old_char = c_char;
  bool firstiter= true;
  //  ofs_match_private_clause();
  while (1) 
  {
#if 1    
    // for the second and after clause, we need a space or ','
    if (!firstiter)
    { 
      if (!ofs_match_char(',')&& ! ofs_skip_whitespace())
      {
        //printf("error: need space or ',' between two clauses!");
        break;
      }
    }
    else     
      firstiter = false;	
#endif    
    ofs_skip_whitespace();

    // simplest clauses without any extra stuff
    if((bitvector&BV_CLAUSE_ORDERED)&&(ofs_match_clause_naked(e_ordered_clause)))
      continue;
    if((bitvector&BV_CLAUSE_NOWAIT)&&(ofs_match_clause_naked(e_nowait)))
      continue;
    if((bitvector&BV_CLAUSE_UNTIED)&&(ofs_match_clause_naked(e_untied)))
      continue;

    // all clauses with a varlist
    if((bitvector&BV_CLAUSE_COPYPRIVATE)&&(ofs_match_clause_varlist(e_copyprivate)))
      continue;
    if((bitvector&BV_CLAUSE_PRIVATE)&&(ofs_match_clause_varlist(e_private)))
      continue;
    if((bitvector&BV_CLAUSE_FIRSTPRIVATE)&&(ofs_match_clause_varlist(e_firstprivate)))
      continue;
    if((bitvector&BV_CLAUSE_SHARED)&&(ofs_match_clause_varlist(e_shared)))
      continue;
    if((bitvector&BV_CLAUSE_COPYIN)&&(ofs_match_clause_varlist(e_copyin)))
      continue;
    if((bitvector&BV_CLAUSE_LASTPRIVATE)&&(ofs_match_clause_varlist(e_lastprivate)))
      continue;

    // reduction clause 
    if((bitvector&BV_CLAUSE_REDUCTION)&& (ofs_match_clause_reduction()))
      continue;

    //match clauses with expressions, 
    if((bitvector&BV_CLAUSE_COLLAPSE)&& (ofs_match_clause_collapse()))
      continue;
    if((bitvector&BV_CLAUSE_IF)&& (ofs_match_clause_if()))
      continue;
    if((bitvector&BV_CLAUSE_NUM_THREADS)&& (ofs_match_clause_num_threads()))
      continue;
    
    //match default () 
    if((bitvector&BV_CLAUSE_DEFAULT)&& (ofs_match_clause_default()))
      continue;

    //match schedule(kind[, chunk_size])
    if((bitvector&BV_CLAUSE_SCHEDULE)&& (ofs_match_clause_schedule()))
      continue;
    
    // allow no clauses matched at all  
    break;
  }
  // match end of directive line
  if (!ofs_match_omp_directive_end())
  {
    printf("ERROR: OpenMP directive text ends abnormally for \n%s\n",old_char);
    assert(false);
  }
  return true;
}

//! A helper function to remove Fortran '!comments', but not '!$omp ...' from a string
// handle complex cases like:
// ... !... !$omp .. !...
static void removeFortranComments(string &buffer)
{
  size_t pos1 ;
  size_t pos2;
  size_t pos3=string::npos;

  pos1= buffer.rfind("!", pos3);
  while (pos1!=string::npos)
  {
    pos2= buffer.rfind("!$omp",pos3);
    if (pos1!=pos2) // is a real comment if not !$omp
    {
      buffer.erase(pos1);
    }
    else // find "!$omp", cannot stop here since there might have another '!' before it
      //limit the search range
    {
      if (pos2>=1)
        pos3= pos2-1;
      else
        break;
    }
    pos1= buffer.rfind("!", pos3);
  }
}

//!  A helper function to tell if a line has an ending '&', followed by optional space , tab , '\n', 
// "!comments" should be already removed  (call removeFortranComments()) before calling this function
static bool hasFortranLineContinuation(const string& buffer)
{
  // search backwards for '&'
  size_t pos = buffer.rfind("&");
  if (pos ==string::npos)
    return false;
  else
  {
    // make sure the characters after & is legal
    for (size_t i = ++pos; i<buffer.length(); i++)
    {
      char c= buffer[i];
      if ((c!=' ')&&(c!='\t'))
      {
        return false;
      }
    }
  }
  return true;
}


//!Assume two Fortran OpenMP comment lines are just merged, remove" & !$omp [&]" within them
// Be careful about the confusing case 
//   the 2nd & and  the end & as another continuation character
static void postProcessingMergedContinuedLine(std::string & buffer)
{
  size_t first_pos, second_pos, last_pos, next_cont_pos;
  removeFortranComments(buffer);
  // locate the first &
  first_pos = buffer.find("&");
  assert(first_pos!=string::npos);

  // locate the !$omp, must have it for OpenMP directive
  second_pos = buffer.find("$omp",first_pos);
  assert(second_pos!=string::npos);
  second_pos +=3; //shift to the end 'p' of "$omp"
  // search for the optional next '&'
  last_pos = buffer.find("&",second_pos);

  // locate the possible real cont &
  // If it is also the found optional next '&'
  // discard it as the next '&'
  if (hasFortranLineContinuation(buffer))
  {
    next_cont_pos = buffer.rfind("&");
    if (last_pos == next_cont_pos)
      last_pos = string::npos;
  }

  if (last_pos==string::npos)
    last_pos = second_pos;
  // we can now remove from first to last pos from the buffer
  buffer.erase(first_pos, last_pos - first_pos + 1);
}

//-------------- the implementation for the external interface -------------------
//! Check if a line is an OpenMP directive, 
// the associated node is needed to tell the programming language
bool  ofs_is_omp_sentinels(const char* str, SgNode* node)
{
  bool result; 
  assert (node&&str);
  // make sure it is side effect free
  const char* old_char = c_char;
  SgNode* old_node = c_sgnode;

  c_char = str;
  c_sgnode = node;
  result = ofs_is_omp_sentinels();

  c_char = old_char;
  c_sgnode = old_node;

  return result;
} 

// Parsing OpenMP directive string associated with a node 
OmpSupport::OmpAttribute* omp_fortran_parse(SgNode* locNode, const char* str)
{
  ROSE_ASSERT(locNode != NULL);
  //Initialization for each parsing session
  // set the file scope char* all lower case. 
  c_char = ofs_copy_lower(str);
  //processCombinedContinuedLine(c_char);
  c_sgnode = locNode;
  ompattribute = NULL;
  current_exp = NULL;
  omptype = e_unknown;

  //Ready and go
  ofs_skip_whitespace();
  //  printf("current target c str is:%s\n",c_char);
  if (ofs_is_omp_sentinels())
  {
    ofs_skip_whitespace();
    // printf("Found OpenMP directives:%s\n",c_char);
    // sort directives in alphabetic order here
    // !$omp atomic,  
    // the default trail check is on for those simple directives since a space a like is needed after them
    if (ofs_match_substr("atomic"))
    {
      ompattribute = buildOmpAttribute(e_atomic, c_sgnode, true);
      // nothing further
    }
    // !$omp barrier 
    else if (ofs_match_substr("barrier"))
    {
      ompattribute = buildOmpAttribute(e_barrier, c_sgnode, true);
      // nothing further
    }
    // !$omp critical
    else if (ofs_match_substr("critical",false))
    {
      ompattribute = buildOmpAttribute(e_critical, c_sgnode, true);
      char namebuffer[OFS_MAX_LEN];
      if (ofs_match_name_in_parenthesis(namebuffer))
      {
        ompattribute->setCriticalName(namebuffer);
      }
    }
    // !$omp do
    else if (ofs_match_substr("do"))
    {
      ompattribute = buildOmpAttribute(e_do, c_sgnode, true);
      ofs_match_omp_clauses(BV_OMP_DO_CLAUSES);
    }
    // !$omp end critical/do/master/ordered/ parallel [do|sections|workshare|]
    // !$omp end sections/single/task/workshare
    // Note that the first two keywords can be used together without blanks/tabs
    // in between!! So we skip trail check after matching a substr
    else if (ofs_match_substr("end", false))
    {
      ofs_skip_whitespace();// additional space etc.
      //$$omp end critical 
      if (ofs_match_substr("critical",false))
      {
        ompattribute = buildOmpAttribute(e_end_critical, c_sgnode, true);
        char namebuffer[OFS_MAX_LEN];
        if (ofs_match_name_in_parenthesis(namebuffer))
        {
          ompattribute->setCriticalName(namebuffer);
        }
      }
      //!$omp end do
      else if (ofs_match_substr("do")) 
      {
        ompattribute = buildOmpAttribute(e_end_do, c_sgnode, true);
        ofs_match_omp_clauses(BV_CLAUSE_NOWAIT);
      }  
      //!$omp end master
      else if (ofs_match_substr("master"))
      {
        ompattribute = buildOmpAttribute(e_end_master, c_sgnode, true);
        // nothing further
      } 
      //!$omp end ordered
      else  if (ofs_match_substr("ordered"))
      {
        ompattribute = buildOmpAttribute(e_end_ordered, c_sgnode, true);
        // nothing further
      } 
      else  if (ofs_match_substr("parallel", false))
      {
        //!$omp end parallel do
        ofs_skip_whitespace();// additional space etc.
        if (ofs_match_substr("do"))
        {
          ompattribute = buildOmpAttribute(e_end_parallel_do, c_sgnode, true);
          // nothing further, nowait cannot be used with end parallel 
        } 
        //!$omp end parallel sections
        else  if (ofs_match_substr("sections"))
        {
          ompattribute = buildOmpAttribute(e_end_parallel_sections, c_sgnode, true);
          // nothing further, nowait cannot be used with end parallel 
        } 
        //!$omp end parallel workshare
        else  if (ofs_match_substr("workshare"))
        {
          ompattribute = buildOmpAttribute(e_end_parallel_workshare, c_sgnode, true);
          // nothing further, nowait cannot be used with end parallel 
        } 
        else
          //!$omp end parallel
        {
          ompattribute = buildOmpAttribute(e_end_parallel, c_sgnode, true);
          // nothing further
        }
      }
      //!$omp end sections
      else if (ofs_match_substr("sections"))
      {
        ompattribute = buildOmpAttribute(e_end_sections, c_sgnode, true);
        ofs_match_omp_clauses(BV_CLAUSE_NOWAIT);
      }
      //!$omp end single 
      else if (ofs_match_substr("single"))
      {
        ompattribute = buildOmpAttribute(e_end_single, c_sgnode, true);
        ofs_match_omp_clauses(BV_CLAUSE_NOWAIT|BV_CLAUSE_COPYPRIVATE);
      }
      //!$omp end task
      else if (ofs_match_substr("task"))
      {
        ompattribute = buildOmpAttribute(e_end_task, c_sgnode, true);
        // nothing further
      }
      //!$omp end workshare
      else if (ofs_match_substr("workshare"))
      {
        ompattribute = buildOmpAttribute(e_end_workshare, c_sgnode, true);
        ofs_match_omp_clauses(BV_CLAUSE_NOWAIT);
      }
      else
      {
        printf("error: !$omp end must be followed by some other keywords! orig is:\n%s\n",str);
        assert(false);
      }
    } // finished handling of "!$omp end ...."
    // !$omp flush 
    else if (ofs_match_substr("flush"))
    {
      ompattribute = buildOmpAttribute(e_flush, c_sgnode, true);
      omptype = e_flush;
      //optional (varlist)
      if (ofs_match_char('('))
      {
        if (!ofs_match_varlist())
        {
          printf("error: cannot find a var list after matching 'flush(' for %s\n",str);
        }
      }
    }
    // !$omp master
    else if (ofs_match_substr("master"))
    {
      ompattribute = buildOmpAttribute(e_master, c_sgnode, true);
      // nothing further
    }
    // !$omp ordered
    else if (ofs_match_substr("ordered"))
    { // note the difference between ordered directive and ordered clause here
      ompattribute = buildOmpAttribute(e_ordered_directive, c_sgnode, true);
      // nothing further
    }
    else
      if (ofs_match_substr("parallel",false)) //don't have to have a space after it
      {
        ofs_skip_whitespace();// additional space etc.
        // !$omp parallel do
        if (ofs_match_substr("do"))
        {
          ompattribute = buildOmpAttribute(e_parallel_do, c_sgnode, true);
          ofs_match_omp_clauses(BV_OMP_PARALLEL_CLAUSES|BV_OMP_DO_CLAUSES);
        }
        //!$omp parallel sections	
        else if (ofs_match_substr("sections"))
        {
          ompattribute = buildOmpAttribute(e_parallel_sections,c_sgnode, true);
          ofs_match_omp_clauses(BV_OMP_PARALLEL_CLAUSES|BV_OMP_SECTIONS_CLAUSES);
        }
        //!$omp parallel workshare
        else if (ofs_match_substr("workshare")) 
        {
          ompattribute = buildOmpAttribute(e_parallel_workshare,c_sgnode, true);
          ofs_match_omp_clauses(BV_OMP_PARALLEL_CLAUSES);
        }
        else  
          //!$omp parallel , must be the last to be checked.
        {
          ompattribute = buildOmpAttribute(e_parallel,c_sgnode, true);
          ofs_match_omp_clauses(BV_OMP_PARALLEL_CLAUSES);
        }
      } // end if parallel ..
    // !$omp sections
      else if (ofs_match_substr("sections"))
      {
        ompattribute = buildOmpAttribute(e_sections, c_sgnode, true);
        ofs_match_omp_clauses(BV_OMP_SECTIONS_CLAUSES);
      }
    //!$omp section
      else if (ofs_match_substr("section"))
      {
        ompattribute = buildOmpAttribute(e_section, c_sgnode, true);
        //nothing further
      }
    //!$omp single
      else if (ofs_match_substr("single"))
      {
        ompattribute = buildOmpAttribute(e_single, c_sgnode, true);
        ofs_match_omp_clauses(BV_CLAUSE_PRIVATE|BV_CLAUSE_FIRSTPRIVATE);
      }
    //!$omp task
      else if (ofs_match_substr("task"))
      {
        ompattribute = buildOmpAttribute(e_task, c_sgnode, true);
        ofs_match_omp_clauses(BV_OMP_TASK_CLAUSES);
      }
    //!$omp taskwait
      else if (ofs_match_substr("taskwait"))
      {
        ompattribute = buildOmpAttribute(e_taskwait, c_sgnode, true);
        //nothing further
      }
    //!$omp threadprivate
      else if (ofs_match_substr("threadprivate",false))
      {
        ompattribute = buildOmpAttribute(e_threadprivate, c_sgnode, true);
        //TODO (list) variables, or common blocks
        omptype = e_threadprivate;
        //optional (varlist)
        if (ofs_match_char('('))
        {
          if (!ofs_match_varlist())
          {
            printf("error: cannot find a var list after matching 'threadprivate(' for %s\n",str);
          }
        }
      }
    //!$omp workshare
      else if (ofs_match_substr("workshare"))
      {
        ompattribute = buildOmpAttribute(e_workshare, c_sgnode, true);
        //nothing further
      }
      else
      {
        printf("error: found an OpenMP sentinel without any legal OpenMP directive followed for \n%s\n",str);
        assert(false);
      }
  }
  else
  {
    //    printf("skip a non-OpenMP comment:%s\n",c_char);
  }

  return ompattribute;
}

// The entry point for the OpenMP Fortran directive parser
void parse_fortran_openmp(SgSourceFile *sageFilePtr)
{
  std::vector <SgNode*> loc_nodes = NodeQuery::querySubTree (sageFilePtr, V_SgLocatedNode);
  std::vector <SgNode*>::iterator iter;
  for (iter= loc_nodes.begin(); iter!= loc_nodes.end(); iter++)
  {
    SgLocatedNode* locNode= isSgLocatedNode(*iter);
    ROSE_ASSERT(locNode);
    AttachedPreprocessingInfoType *comments = locNode->getAttachedPreprocessingInfo ();
    if (comments)
    {
      //      SageInterface::dumpInfo(locNode);
      AttachedPreprocessingInfoType::iterator iter, previter = comments->end();

      // Handle line continuation among OpenMP directives: 
      //  Two major cases (each line has two lines with '\n' in between):
      //
      //  !$omp ...   & \n !$omp .... 
      //  !$omp ...   & \n !$omp & ....
      //
      //  The handling is complex since : 
      //  the '&' can be used for two purposes: 
      //              the regular line continuation at an end of a line
      //              or the marker for the beginning of a 2nd line.
      //  Also, source comments can be used after &
      //  The continuation can be applied to multiple lines
      //
      // The basic idea is to delay the handling of the current line if it has a line continuation (&): 
      //    (save to previter)
      // The next line will be merged with a previous pending line with & , 
      //    and remove disposable stuff (postProcessingMergedContinuedLine()).
      // The actual parsing is done only if the merged one does not have a line continuation (&).
      //  Otherwise, mark it as pending and go on merging until the last line without a line continuation (&).
      std::list<std::string> comment_list;
      for (iter = comments->begin(); iter!=comments->end(); iter++)
      {
        PreprocessingInfo * pinfo = *iter;
        if (pinfo->getTypeOfDirective()==PreprocessingInfo::FortranStyleComment)
        {
          string buffer = pinfo->getString();
          // Change to lower case
          std::transform(buffer.begin(), buffer.end(), buffer.begin(), ::tolower);
          // We are not interested in other comments
          if (!ofs_is_omp_sentinels(buffer.c_str(),locNode))
          {
            if (previter!= comments->end())
            {
              printf("error: Found a none-OpenMP comment after a pending OpenMP comment with a line continuation\n");
              assert(false);
            }
            continue;
          } 

          // remove possible comments also:
          removeFortranComments(buffer);
          //          cout<<"----------------------"<<endl;
          //          cout<<"current line:"<<buffer<<endl;
          // merge with possible previous line with &
          if (previter!= comments->end())
          {
            //            cout<<"previous line:"<<(*previter)->getString()<<endl;
            buffer = (*previter)->getString()+buffer;
            // remove "& !omp [&]" within the merged line
            postProcessingMergedContinuedLine(buffer);
            //            cout<<"merged line:"<<buffer<<endl;
            (*previter)->setString(""); // erase previous line with & at the end
          }

          pinfo->setString(buffer); //save the changed buffer back 

          if (hasFortranLineContinuation(buffer))
          {
            //delay the handling of the current line to the next line
            previter = iter;
          }
          else
          { // Now we have a line without line-continuation & , we can proceed to parse it
            previter = comments->end(); // clear this flag for a pending line with &
            OmpSupport::OmpAttribute* att= omp_fortran_parse(locNode, pinfo->getString().c_str());
            if (att)
            {
              att->setPreprocessingInfo(pinfo);
              addOmpAttribute(att, locNode);
              ROSE_ASSERT (locNode->getAttachedPreprocessingInfo ()->size() != 0);
             // cout<<"debug at ompFortranParser.C:"<<locNode<<" "<< locNode->getAttachedPreprocessingInfo ()->size() <<endl;
              omp_comment_list.push_back(att);
            }
          }
        }
      } //end for all preprocessing info

    }
  } //end for located nodes
}
