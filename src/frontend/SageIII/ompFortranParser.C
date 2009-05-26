// A hand-crafted OpenMP parser for Fortran comments
// It only exposes one interface function: 
//   omp_fortran_parse()
// All other supporting functions should be declared with "static" (file scope only)
// Liao, 5/24/2009

#include "rose.h"
#include "OmpAttribute.h"
#include <cstdio>
#include <cstring>

// maximum length for a buffer for a variable or OpenMP construct name
#define OFS_MAX_LEN 256
using namespace OmpSupport;
// Parse a SgNode's OpenMP style comments (str) into OmpAttribute
OmpSupport::OmpAttribute* omp_fortran_parse(SgNode* locNode, const char* str);

// A file scope char* to avoid passing and returning target c string for every and each function
static const char* c_char = NULL; // current characters being scanned
static SgNode* c_sgnode = NULL; // current SgNode associated with the OpenMP directive
static OmpSupport::OmpAttribute* ompattribute = NULL; // the current attribute (directive) being built
static omp_construct_enum omptype= e_unknown; // the current clause (or reduction operation type) being filled out
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

//! Match a variable list like  "x,y,z)"
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
    if (ofs_match_anyname(buffer))
    {

      ompattribute->addVariable(omptype, buffer);
      // look for the next variable
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
    } else
      if (ofs_match_char('*'))
      {
        ompattribute->setReductionOperator(e_reduction_mul);
        omptype = e_reduction_mul;
      } else
        if (ofs_match_char('-'))
        {
          ompattribute->setReductionOperator(e_reduction_minus);
          omptype = e_reduction_minus; 
        } else 
          // match multi-char operator/intrinsics 
          // 9 of them in total
          // we tend to not share the enumerate types between C/C++ and Fortran operators
          if (ofs_match_substr(".and.",false)) 
          {
            ompattribute->setReductionOperator(e_reduction_and);
            omptype = e_reduction_and; 
          } else
            if (ofs_match_substr(".or.",false)) 
            {
              ompattribute->setReductionOperator(e_reduction_or);
              omptype = e_reduction_or; 
            } else
              if (ofs_match_substr(".eqv.",false)) 
              {
                ompattribute->setReductionOperator(e_reduction_eqv);
                omptype = e_reduction_eqv; 
              } else
                if (ofs_match_substr(".neqv.",false)) 
                {
                  ompattribute->setReductionOperator(e_reduction_neqv);
                  omptype = e_reduction_neqv; 
                } else
                  if (ofs_match_substr("max",false)) 
                  {
                    ompattribute->setReductionOperator(e_reduction_max);
                    omptype = e_reduction_max; 
                  } else
                    if (ofs_match_substr("min",false)) 
                    {
                      ompattribute->setReductionOperator(e_reduction_min);
                      omptype = e_reduction_min; 
                    } else
                      if (ofs_match_substr("iand",false)) 
                      {
                        ompattribute->setReductionOperator(e_reduction_iand);
                        omptype = e_reduction_iand; 
                      } else
                        if (ofs_match_substr("ior",false)) 
                        {
                          ompattribute->setReductionOperator(e_reduction_ior);
                          omptype = e_reduction_ior; 
                        } else
                          if (ofs_match_substr("ieor",false)) 
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
  while (1) //TODO handle multiple lines directives
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
    // special reduction clause 
    if((bitvector&BV_CLAUSE_REDUCTION)&& (ofs_match_clause_reduction()))
      continue;

    //TODO clauses with expressions, default, and schedule
    
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

//-------------- the implementation for the external interface -------------------
// The entry point for the OpenMP Fortran directive parser
//
OmpSupport::OmpAttribute* omp_fortran_parse(SgNode* locNode, const char* str)
{
  ROSE_ASSERT(locNode != NULL);
  //Initialization for each parsing session
  // set the file scope char* all lower case. 
  c_char = ofs_copy_lower(str);
  c_sgnode = locNode;
  ompattribute = NULL;
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
      ompattribute = buildOmpAttribute(e_atomic, c_sgnode);
    }
    // !$omp barrier 
    else if (ofs_match_substr("barrier"))
    {
      ompattribute = buildOmpAttribute(e_barrier, c_sgnode);
    }
    // !$omp critical
    else if (ofs_match_substr("critical"))
    {
      ompattribute = buildOmpAttribute(e_critical, c_sgnode);
    }
    // !$omp do
    else if (ofs_match_substr("do"))
    {
      ompattribute = buildOmpAttribute(e_do, c_sgnode);
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
      if (ofs_match_substr("critical"))
      {
        ompattribute = buildOmpAttribute(e_end_critical, c_sgnode);
      }else
        //!$omp end do
        if (ofs_match_substr("do")) 
        {
          ompattribute = buildOmpAttribute(e_end_do, c_sgnode);
          ofs_match_omp_clauses(BV_CLAUSE_NOWAIT);
        } else 
          //!$omp end master
          if (ofs_match_substr("master"))
          {
            ompattribute = buildOmpAttribute(e_end_master, c_sgnode);
          } else
            //!$omp end ordered
            if (ofs_match_substr("ordered"))
            {
              ompattribute = buildOmpAttribute(e_end_ordered, c_sgnode);
            } else
              if (ofs_match_substr("parallel", false))
              {
                //!$omp end parallel do
                ofs_skip_whitespace();// additional space etc.
                if (ofs_match_substr("do"))
                {
                  ompattribute = buildOmpAttribute(e_end_parallel_do, c_sgnode);
                } else
                  //!$omp end parallel sections
                  if (ofs_match_substr("sections"))
                  {
                    ompattribute = buildOmpAttribute(e_end_parallel_sections, c_sgnode);
                  } else
                    //!$omp end parallel workshare
                    if (ofs_match_substr("workshare"))
                    {
                      ompattribute = buildOmpAttribute(e_end_parallel_workshare, c_sgnode);
                    } else
                      //!$omp end parallel
                    {
                      ompattribute = buildOmpAttribute(e_end_parallel, c_sgnode);
                    }
              }
            //!$omp end sections
              else if (ofs_match_substr("sections"))
              {
                ompattribute = buildOmpAttribute(e_end_sections, c_sgnode);
              }
            //!$omp end single 
              else if (ofs_match_substr("single"))
              {
                ompattribute = buildOmpAttribute(e_end_single, c_sgnode);
              }
            //!$omp end task
              else if (ofs_match_substr("task"))
              {
                ompattribute = buildOmpAttribute(e_end_task, c_sgnode);
              }
            //!$omp end workshare
              else if (ofs_match_substr("workshare"))
              {
                ompattribute = buildOmpAttribute(e_end_workshare, c_sgnode);
              }
              else
              {
                printf("error: $omp end must be followed by some other keywords! orig is:\n%s\n",str);
                assert(false);
              }
    } // finished handling of "!$omp end ...."
    // !$omp flush 
    else if (ofs_match_substr("flush"))
    {
      ompattribute = buildOmpAttribute(e_flush, c_sgnode);
    }
    // !$omp master
    else if (ofs_match_substr("master"))
    {
      ompattribute = buildOmpAttribute(e_master, c_sgnode);
    }
    // !$omp ordered
    else if (ofs_match_substr("ordered"))
    { // note the difference between ordered directive and ordered clause here
      ompattribute = buildOmpAttribute(e_ordered_directive, c_sgnode);
    }
    else
      if (ofs_match_substr("parallel",false)) //don't have to have a space after it
      {
        ofs_skip_whitespace();// additional space etc.
        // !$omp parallel do
        if (ofs_match_substr("do"))
        {
          ompattribute = buildOmpAttribute(e_parallel_do, c_sgnode);
          ofs_match_omp_clauses(BV_OMP_PARALLEL_CLAUSES|BV_OMP_DO_CLAUSES);
        }
        //!$omp parallel sections	
        else if (ofs_match_substr("sections"))
        {
          ompattribute = buildOmpAttribute(e_parallel_sections,c_sgnode);
        }
        //!$omp parallel workshare
        else if (ofs_match_substr("workshare")) 
        {
          ompattribute = buildOmpAttribute(e_parallel_workshare,c_sgnode);
        }
        else  
          //!$omp parallel , must be the last to be checked.
        {
          ompattribute = buildOmpAttribute(e_parallel,c_sgnode);
        }
      } // end if parallel ..
    // !$omp sections
      else if (ofs_match_substr("sections"))
      {
        ompattribute = buildOmpAttribute(e_sections, c_sgnode);
      }
    //!$omp section
      else if (ofs_match_substr("section"))
      {
        ompattribute = buildOmpAttribute(e_section, c_sgnode);
      }
    //!$omp single
      else if (ofs_match_substr("single"))
      {
        ompattribute = buildOmpAttribute(e_single, c_sgnode);
      }
    //!$omp task
      else if (ofs_match_substr("task"))
      {
        ompattribute = buildOmpAttribute(e_task, c_sgnode);
      }
    //!$omp taskwait
      else if (ofs_match_substr("taskwait"))
      {
        ompattribute = buildOmpAttribute(e_taskwait, c_sgnode);
      }
    //!$omp threadprivate
      else if (ofs_match_substr("threadprivate"))
      {
        ompattribute = buildOmpAttribute(e_threadprivate, c_sgnode);
      }
    //!$omp workshare
      else if (ofs_match_substr("workshare"))
      {
        ompattribute = buildOmpAttribute(e_workshare, c_sgnode);
      }
      else
      {
        printf("error: found an OpenMP sentinel without any legal OpenMP directive followed\n");
        assert(false);
      }
  }
  else
  {
    //    printf("skip a non-OpenMP comment:%s\n",c_char);
  }

  return ompattribute;
}


