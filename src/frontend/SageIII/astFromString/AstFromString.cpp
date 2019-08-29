/*
 Chunhua (Leo) Liao" <liao6@llnl.gov>
 */

#include "sage3basic.h"
#include "AstFromString.h"
#include <string>
#include <algorithm>

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

namespace AstFromString 
{
  const char* c_char = NULL;
  // current anchor SgNode associated with parsing. It will serve as a start point to find enclosing scopes for
  // resolving identifiers/symbols 
  SgNode* c_sgnode = NULL; 
  SgNode* c_parsed_node = NULL; // the generated SgNode from a parsing function.

  // note: afs_skip_xxx() optional skip 0 or more patterns
  //       afs_match_xxx() try to match a pattern, undo side effect if failed.
  // afs means Ast From String 
  //!Skip 0 or more whitespace or tabs
  /*
   WS  :  (' '|'\r'|'\t'|'\u000C'|'\n') {$channel=HIDDEN;}
       ;
  */
  bool afs_skip_whitespace()
  {
    bool result = false;
    while ((*c_char)==' '||(*c_char)=='\t' ||(*c_char)=='\r' ||(*c_char)=='\n')
    {
      c_char++;
      result= true;
    }
    return result;
  }
  //! Peak the next character to be scanned after skip any possible space/tab
  //  No side effect in any way
  char afs_peak_next()
  {
    char result = '\0';
    const char* old_char = c_char;

    if (strlen(c_char)==0)
      return result;
    else
    {
      afs_skip_whitespace();
      result = *c_char;
      c_char = old_char; // undo side effect here
    }
    return result;
  }

  //! Match a given character after skip any possible space/tab
  //  advance the header if successful, undo side effect if fails
  bool afs_match_char(char c)
  {
    bool result = false;
    const char* old_char = c_char;

    if (strlen(c_char)==0)
      result = false;
    else
    {
      afs_skip_whitespace();
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
  //      whitespace, end of str, newline, tab, (, ), *, or '!', etc.
  //      Set to true by default, used to ensure the matched substr is a full identifier/keywords.
  //      If try to match operators (+=, etc), please set checkTrail to false!!
  //
  //      But Fortran OpenMP allows blanks/tabs to be ignored between certain pair of keywords:
  //      e.g: end critical == endcritical  , parallel do == paralleldo
  //      to match the 'end' and 'parallel', we have to skip trail checking.
  // return values: 
  //    true: find a match, the current char is pointed to the next char after the substr
  //    false: no match, the current char is intact

  bool afs_match_substr(const char* substr, bool checkTrail/* = true */)
  {
    bool result = true;
    const char* old_char = c_char;
    // we skip leading space from the target string
    afs_skip_whitespace();
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
    //     TV (12/22/2013) : added ',' as legal trail.
    //     Liao (9/21/2015) : added '*' as legal trail, e.g. int* j; 
    if (checkTrail)
    {
      if (*c_char!=' '&&*c_char!='\0'&&*c_char!='\n'&&*c_char!='\t' &&*c_char!='!' &&*c_char!='(' &&*c_char!=')' &&*c_char!=',' &&*c_char!='*')
      {
        result = false;
        c_char = old_char;
      }
    }
    return result;
  }
  // A set of helper functions for recognizing legal Fortran identifier
  //! Check if the head char is a digit
  bool afs_is_digit()
  {
    return(( *c_char>='0') &&(*c_char<='9'));
  }

  bool afs_is_lower_letter()
  {
    return (( *c_char>='a') &&(*c_char<='z'));
  }

  bool afs_is_upper_letter()
  {
    return (( *c_char>='A') &&(*c_char<='Z'));
  }

  bool afs_is_letter()
  {
    return (afs_is_lower_letter()|| afs_is_upper_letter());
  }

  //Check if the current char is a legal character for a Fortran identifier,
  // we allow '_' and '$' in addition to letters and digits
  bool afs_is_identifier_char()
  {
    return (afs_is_letter()||afs_is_digit()||(*c_char =='_')||(*c_char =='$'));
  }

  //Scan input c str to match an integer constant, 
  //return true if successful and save the value in result
  // return false otherwise, and undo side effects. 
  // TODO handle sign, binary, hex format?
  bool afs_match_integer_const(int * result)
  {
    char buffer[OFS_MAX_LEN];
    const char* old_char = c_char;
    afs_skip_whitespace();
    // return false if the first char is not a digit
    if (!afs_is_digit())
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

    } while (afs_is_digit());
    buffer[i]='\0';

    // check tail to ensure digit sequence is independent (not part of another identifier)
    // but it can be followed by space ',' '[' '{' etc.
    // cannot be followed by '.' => floating point
    // TODO other cases??
    if (afs_is_letter() || *c_char == '.')
    {
      c_char = old_char;
      return false;
    }
    // convert buffer to an integer value and return
    // printf("int buffer is %s\n",buffer);
    *result = atoi(buffer);
    return true;
  }

  bool afs_match_double_const(double * result) {
    char buffer[OFS_MAX_LEN];
    const char* old_char = c_char;

    afs_skip_whitespace();

    if (!afs_is_digit()) {
      c_char = old_char;
      return false;
    }

    int i=0;
    do {
      buffer[i] = *c_char;
      i++;
      c_char++;
    } while (afs_is_digit());
    buffer[i]='\0';

    *result = atoi(buffer);

    if (*c_char == '.') {
      c_char++;

      double decimals = 1;
      i=0;
      do {
        buffer[i] = *c_char;
        i++;
        c_char++;
        decimals /= 10;
      } while (afs_is_digit());
      buffer[i]='\0';

      if (i > 0) *result += (atoi(buffer) * decimals);
    }

    if (*c_char == 'e' || *c_char == 'E') {
      c_char++;

      double exponent = 1;
      if (*c_char == '-') {
        exponent = -1;
        c_char++;
      }

      if (!afs_is_digit()) {
        c_char = old_char;
        return false;
      }

      i=0;
      do {
        buffer[i] = *c_char;
        i++;
        c_char++;
      } while (afs_is_digit());
      buffer[i]='\0';

      exponent *= pow(10., atoi(buffer));

      *result *= exponent;
    }

    return true;
  }

  // Try to retrieve a possible name identifier from the head
  // store the result in buffer
  /*
     IDENTIFIER
     :     LETTER (LETTER|'0'..'9')*
     ;

     You need symbol table info to parse C
     IDENTIFIERS are 
     1. types 
     2. variable IDs
     3. names:  label statement
     */
  bool afs_match_identifier()
  {
    //bool result = false;
    char buffer[OFS_MAX_LEN];
    const char* old_char = c_char;
    afs_skip_whitespace();
    // check for the first char
    // Must be either of letter or _ (extended to support this)
    if (!(afs_is_letter()||(*c_char=='_')))
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
    } while (afs_is_identifier_char());

    buffer[i]= '\0';

    // resolve the identifier here
    SgScopeStatement* c_scope = getScope(c_sgnode);
    assert (c_scope != NULL);

 // DQ (8/16/2013): Updated use of new API for symbol table support.
 // SgSymbol* sym = lookupSymbolInParentScopes(buffer, c_scope);
    SgSymbol* sym = lookupSymbolInParentScopes(buffer, c_scope,NULL,NULL);
/* TV (02/25/2014) : remove type as when it find something it asserts (I had an issue with 'i' recognized as 'int'...)
    // type?
    SgTypeTable * gtt = SgNode::get_globalTypeTable();
    assert (gtt != NULL);
    SgType* t = gtt->lookup_type(SgName(buffer)); 
    //assert (!(sym &&t)); // can be both a type or a variable? TODO global type table stores variables and their type info.!!??
*/
    if (sym) 
    {
      assert (sym!=NULL);
      SgExpression* ref_exp = NULL;
      switch (sym->variantT())
      {
        case V_SgVariableSymbol:
          ref_exp = buildVarRefExp(isSgVariableSymbol(sym));
          c_parsed_node = ref_exp;  
          break;
        case V_SgFunctionSymbol:
          ref_exp = buildFunctionRefExp(isSgFunctionSymbol(sym));
          c_parsed_node = ref_exp;  
          break;
        case V_SgEnumFieldSymbol:
          c_parsed_node = SageBuilder::buildEnumVal(isSgEnumFieldSymbol(sym));
          break;
        case V_SgLabelSymbol:
          c_parsed_node = isSgLabelSymbol(sym);
          break;
        default:
          {
            cerr<<"error: unhandled symbol type in afs_match_identifier():"<<sym->class_name()<<endl;
            assert(false);
          }
      }
    }
/* TV (02/25/2014) : remove type as when it find something it asserts (I had an issue with 'i' recognized as 'int'...)
    else  if (t)
    {
      assert (0); // TODO global type table stores variables and their type info.It does not store type names !!!!??
      c_parsed_node = t;
    }
*/
    else
    {
// we may just see a variable name when parsing int a; 
// It is totally fine to return a name
//      printf("cannot recognize an identifier:^%s^ not a variable ref, not a type ref.\n",buffer);
      //c_parsed_node = NULL;
      //assert(0);
      //c_char = old_char;
      //return false;
      c_parsed_node = new SgName(buffer);
      assert (c_parsed_node != NULL);
    }
    return true;
  }

  /*
     constant
     :   HEX_LITERAL
     |   OCTAL_LITERAL
     |   DECIMAL_LITERAL
     |   CHARACTER_LITERAL
     |   STRING_LITERAL
     |   FLOATING_POINT_LITERAL
     ;

   * */
  bool afs_match_constant()
  {
    bool result = false;
    int int_result;
    double dbl_result;
    const char* old_char = c_char;
    if (afs_match_integer_const (&int_result))
    {
      result = true;
      //   cout<<"debug:building int val exp:"<<int_result<<endl;
      c_parsed_node = buildIntVal (int_result);
    }
    else if (afs_match_double_const(&dbl_result)) {
      result = true;
      c_parsed_node = buildDoubleVal (dbl_result);
    }    
    // TODO add other types of C constant

    if (result == false)
      c_char = old_char;
    return result;
  }

  /*
     primary_expression
     : IDENTIFIER
     | constant
     | '(' expression ')'
     ;

*/
  bool afs_match_primary_expression()
  {
    bool result = false;
    const char* old_char = c_char;

    if (afs_match_identifier())
    {
      // identifier can return named label, which is not an expression
      // Depending on if the label statement is already created or not,
      // the afs_match_identifier() may return the label symbol or a SgName
      //if (!isSgName(c_parsed_node) && ! isSgLabelSymbol(c_parsed_node))
      if (isSgExpression(c_parsed_node))
        result = true;
    }
    else if (afs_match_constant())
      result = true;
    else if( afs_match_char('('))
    {
      if(!afs_match_expression())
        result = false;
      else if (!afs_match_char(')'))
      {
        result = false;
      }
      else
        result = true;
    }

    if (result == false)
    { // no match
      c_char = old_char;
    }

    return result;
  }

  /*
     ANTLR and YACC

     unary_expression
     : postfix_expression
     | INC_OP unary_expression
     | DEC_OP unary_expression
     | unary_operator cast_expression
     | SIZEOF unary_expression
     | SIZEOF '(' type_name ')'
     ;

     unary_operator
     : '&' // SgAddressOfOp
     | '*'  // SgPointerDerefExp
     | '+'   //SgUnaryAddOp
     | '-'  //SgMinusOp
     | '~'   // Sg
     | '!'   //Sg
     ;
     */
  bool afs_match_unary_expression ()
  {
    bool result = false;
    const char* old_char = c_char;

    if (afs_match_postfix_expression())
    {
      if (isSgExpression(c_parsed_node))
        result = true;
    }
    else if (afs_match_substr("++", false))
    {
      if (afs_match_unary_expression())
      {
        assert (c_parsed_node != NULL);
        SgExpression* exp = isSgExpression(c_parsed_node);
        assert (exp != NULL);
        c_parsed_node = buildPlusPlusOp (exp,SgUnaryOp::prefix );
        result = true;
      }
      else 
      {
        //    printf ("error. afs_match_unary_expression() expects unary_exp after matching ++\n");
        //    assert (0);
        c_char = old_char;
      }
    } 
    else if (afs_match_substr("--", false))
    {
      if (afs_match_unary_expression())
      {
        assert (c_parsed_node != NULL);
        SgExpression* exp = isSgExpression(c_parsed_node);
        assert (exp != NULL);
        c_parsed_node = buildMinusMinusOp (exp,SgUnaryOp::prefix );
        result = true;
      }
      else 
      {
        //printf ("error. afs_match_unary_expression() expects unary_exp after matching --\n");
        //assert (0);
        c_char = old_char;
      }

    } else if (afs_peak_next() == '&' || afs_peak_next() == '*'|| afs_peak_next() == '+'
        ||afs_peak_next() == '-'||afs_peak_next() == '~' || afs_peak_next() == '!' )
    {
      VariantT op_type ;
      if (afs_match_char('&'))
        op_type = V_SgAddressOfOp;
      else if (afs_match_char('*'))
        op_type = V_SgPointerDerefExp;
      else if (afs_match_char('-'))
        op_type = V_SgMinusOp;
      else if (afs_match_char('+'))
        op_type = V_SgUnaryAddOp;
      else if (afs_match_char('~'))
        op_type = V_SgBitComplementOp;
      else if (afs_match_char('!'))
        op_type = V_SgNotOp;
      else
      {
        printf("error. afs_match_unary_expression(): unimplemented unary operator type:\n");
        assert(0);
      }

      if (afs_match_cast_expression())
      {
        SgExpression* exp = isSgExpression(c_parsed_node);
        assert (exp != NULL);
        switch (op_type)
        {
          case V_SgAddressOfOp:
            c_parsed_node = buildAddressOfOp(exp);
            break;
          case V_SgPointerDerefExp:
            c_parsed_node = buildPointerDerefExp(exp);
            break;
          case V_SgMinusOp:
            c_parsed_node = buildMinusOp(exp);
            break;
          case V_SgUnaryAddOp:
            c_parsed_node = buildUnaryAddOp(exp);
            break;
          case V_SgBitComplementOp:
            c_parsed_node = buildBitComplementOp(exp);
            break;
          case V_SgNotOp:
            c_parsed_node = buildNotOp(exp);
            break;
          default:
            printf("afs_match_unary_expression(): unhandled unary operator type\n");
            assert (0);
        }  

        result = true;
      }
    } else if (afs_match_substr("sizeof"))
    {
      /*
         | SIZEOF unary_expression
         */
      if (afs_match_unary_expression())
      {
        SgExpression* exp = isSgExpression(c_parsed_node);
        assert (exp != NULL);
        c_parsed_node = buildSizeOfOp(exp);
        result = true;  
      } else if (afs_match_char('('))
        /*
           | SIZEOF '(' type_name ')'
           */
      {
        if (afs_match_type_name())
        {
          SgType* t = isSgType(c_parsed_node);
          assert (t != NULL);
          if (afs_match_char(')'))
          {
            c_parsed_node = buildSizeOfOp(t);
            result = true;
          }
        }
      }    
    }

    if (result == false)   c_char = old_char;
    return result;
  }

  // decode the simplest first
  // the qualifer_specifier_list needs special attention
  // **The original grammar is too naive*** 
  // We can actual avoid much of this trouble if we used a better grammar. **

  // the match function only saves the last matched qualifier/specifier in c_parsed_node
  // we need to adjust the real c_parsed_node according to the combination in sq_list
  //
  // for example "long int" will result int type in c_parsed_node,
  // we need to search sq_list for long and adjust c_parsed_node to be "long int" 
  //TODO consider more complex point type case!!
  bool decode_list (std::vector<SgNode*>& sq_list) // const vector <> & will not work with std::find() !!!
  {
    if (sq_list.size() ==0)
    {
      printf("error in AstFromString::decode_list(): empty list!\n");  
      assert (0);
    }
    else if (sq_list.size() == 1)
    {
      // Integer: char, int, 
      // Others: void, float, double, TODO: struct/union, enum, TYPE_NAME
      // const/volatile modifier
      //c_parsed_node is the result
      return true;
    }

    // 2 or more items

    // step 1: has one of 5 integer types? 
    //--------------------------
    //   char, 
    //   int, short int, long int, long long int
    //    with optional:  signed (optional)/ unsigned
    //  we also treat long double here for convenience.
    bool has_char = false;
    bool has_int= false;
    bool has_double= false;

    vector<SgNode*>::iterator iter;
    SgNode * target;
    target = buildCharType();
    iter  = find (sq_list.begin(), sq_list.end(), target);
    if (iter != sq_list.end())
      has_char = true;
    target = buildIntType();
    iter  = find(sq_list.begin(), sq_list.end(), target);
    if (iter != sq_list.end())
      has_int = true; 
    target = buildDoubleType();
    iter  = find(sq_list.begin(), sq_list.end(), target);
    if (iter != sq_list.end())
      has_double = true; 
    assert ( int(has_char) + int(has_int) + int(has_double) <=1); // cannot have more than one base types

    // handle "short int": optional last match of 'int' only return int type to us
    if (find(sq_list.begin(), sq_list.end(), buildShortType()) != sq_list.end())
    {
      //assert (isSgTypeInt(c_parsed_node) != NULL);
      c_parsed_node = buildShortType();
    }

    // Handle  "long int", "long double"  "long long int"
    target = buildLongType();
    iter  = find(sq_list.begin(), sq_list.end(),target);
    if (iter != sq_list.end())
    {
      // long long? followed by optional "int"
      if ((*(iter++)) == buildLongType())
      {
        //assert (isSgTypeInt(c_parsed_node) != NULL);
        c_parsed_node = buildLongLongType();
      }
      else // single "long"
      {
        //could be long , long int, or long double
        if (has_int)
        {
          assert (isSgTypeInt(c_parsed_node) != NULL);
          c_parsed_node = buildLongType();
        }
        else if (has_double)
        {
          assert (isSgTypeDouble(c_parsed_node) != NULL);
          c_parsed_node = buildLongDoubleType();
        }
        else
        {
          printf("Error: AstFromString::decode_list(), found 'long' without companying 'int' or 'double'\n"); 
          assert (0);
        }

      } 
    }

    // handle optional signed /unsigned 
    bool has_signed = false;
    bool has_unsigned = false;
    if ( find(sq_list.begin(), sq_list.end(), buildSignedLongLongType())!= sq_list.end())
      has_signed = true; 
    if ( find(sq_list.begin(), sq_list.end(), buildUnsignedLongLongType())!= sq_list.end())
      has_unsigned = true; 
    assert (! (has_signed && has_unsigned));

    // This has to be done after the adjustment of short int, long int, etc
    if (has_int || has_char)
    {
      // we internally use signed long long to indicate a match of "signed"
      switch (c_parsed_node ->variantT())
      {
        case V_SgTypeChar:
          {
            if (has_signed)
              c_parsed_node = buildSignedCharType();
            else if (has_unsigned)
              c_parsed_node = buildUnsignedCharType();
            // nothing further if no adjustment is needed
            break;
          }
        case V_SgTypeInt:
          {
            if (has_signed)
              c_parsed_node = buildSignedIntType();
            else if (has_unsigned)
              c_parsed_node = buildUnsignedIntType();
            break;
          }
        case V_SgTypeLong:
          {
            if (has_signed)
              c_parsed_node = buildSignedLongType();
            else if (has_unsigned)
              c_parsed_node = buildUnsignedLongType();
            break;
          }
        case V_SgTypeLongLong:
          {
            if (has_signed)
              c_parsed_node = buildSignedLongLongType();
            else if (has_unsigned)
              c_parsed_node = buildUnsignedLongLongType();
            break;
          }
        case V_SgTypeShort:
          {
            if (has_signed)
              c_parsed_node = buildSignedShortType();
            else if (has_unsigned)
              c_parsed_node = buildUnsignedShortType();
            break;
          }
#if 1
        case V_SgTypeUnsignedLongLong:
        case V_SgTypeSignedLongLong:
          {
            // we don't need additional sign/unsign on top of these two types
            break;
          }
#endif
        default:
          {
            if (has_signed || has_unsigned)
            {
              cerr<<"Error: AstFromString::decode_list(), illegal use  of 'signed' with type: "<<c_parsed_node->class_name()<<endl;
              assert (0);
            }
          }
      }
    } // end of if (has_int || has_char)

    // step 2: other types, no special treatment so far: 
    //--------------------------
    // void, float , double (treated as part for long in step 1 already), TODO: struct/union, enum, TYPE_NAME

    // step 3: handle qualifier: const/volatile SgConstVolatileModifier
    // c_parsed_node only keep the last match: "const char" will return char type only 
    //--------------------------
    assert (sq_list.size() > 1);
    bool has_const = false;
    bool has_volatile= false;
    for (iter = sq_list.begin(); iter != sq_list.end(); iter ++)
    {
      SgNode * cur_node = *iter;
      if (SgConstVolatileModifier* mod = isSgConstVolatileModifier(cur_node))
      {
        if (mod->isConst())
        {
          assert (has_const == false); // can only match once
          has_const = true;
        }
        else if (mod->isVolatile())
        {
          assert (has_volatile== false); // can only match once
          has_volatile= true;
        }
      }// end if
    } // end for 
    // it is legal to have both const and volatile modifiers
    if (has_const)
    {
      SgType* base_type = isSgType(c_parsed_node);
      assert (base_type != NULL);
      c_parsed_node = buildConstType(base_type);
    } 
    if (has_volatile)
    {
      SgType* base_type = isSgType(c_parsed_node);
      assert (base_type != NULL);
      c_parsed_node = buildVolatileType(base_type);
    } 


    return true;
  }
  /*
     Yacc Grammar: 
     type_name
     : specifier_qualifier_list
     | specifier_qualifier_list abstract_declarator
     ;

     ANTLR grammar: ? means 0 or 1 occurence
     type_name
     : specifier_qualifier_list abstract_declarator?
     ;

*/
  bool afs_match_type_name()
  {
    bool result = false;
    const char* old_char = c_char;
    std::vector<SgNode*> sq_list;
    if (afs_match_specifier_qualifier_list(sq_list))
    {
      result = true;
      // now decode the sq_list to create real type
      //signed

      //unsigned
      // others
      bool rt = decode_list (sq_list);
      ASSERT_always_require (rt);
    }
    else
    {
      c_char = old_char;
      result = false;
    }

    // TODO 
#if 0    
    if (afs_match_abstract_declarator())
    {
    }
#endif

    return result;
  }
  /*
     type_qualifier
     : 'const'
     | 'volatile'
     ;

  */
  bool afs_match_type_qualifier()
  {
    bool result = false;
    const char* old_char = c_char;
    if (afs_match_substr("const"))
    {
      c_parsed_node = buildConstVolatileModifier (SgConstVolatileModifier::e_const);
      result = true;
    }
    else if (afs_match_substr("volatile"))
    {
      c_parsed_node = buildConstVolatileModifier (SgConstVolatileModifier::e_volatile);
      result = true;
    }
    if (result == false)   c_char = old_char;
    return result; 
  }  
  /* Yacc grammar
     type_specifier
     : VOID
     | CHAR
     | SHORT
     | INT
     | LONG
     | FLOAT
     | DOUBLE
     | SIGNED
     | UNSIGNED
     | struct_or_union_specifier
     | enum_specifier
     | TYPE_NAME
     ;
     ANTLR grammar:
     type_specifier
     : 'void'
     | 'char'
     | 'short'
     | 'int'
     | 'long'
     | 'float'
     | 'double'
     | 'signed'
     | 'unsigned'
     | struct_or_union_specifier
     | enum_specifier
     | type_id
     ;

     type_id
     :   {isTypeName(input.LT(1).getText())}? IDENTIFIER
  //        {System.out.println($IDENTIFIER.text+" is a type");}
  ;

*/
  bool afs_match_type_specifier(bool checkTrail)
  {
    bool result = false;
    const char* old_char = c_char;

    if (afs_match_substr("void", checkTrail))
    {
      c_parsed_node = buildVoidType();
      result = true;
    }
    else if (afs_match_substr("char", checkTrail))
    {
      c_parsed_node = buildCharType();
      result = true;
    }
    else if (afs_match_substr("short", checkTrail))
    {
      c_parsed_node = buildShortType();
      result = true;
    }
    else if (afs_match_substr("int", checkTrail))
    {
      c_parsed_node = buildIntType();
      result = true;
    }
    else if (afs_match_substr("long", checkTrail))
    {
      c_parsed_node = buildLongType();
      result = true;
    }
    else if (afs_match_substr("float", checkTrail))
    {
      c_parsed_node = buildFloatType();
      result = true;
    }
    else if (afs_match_substr("double", checkTrail))
    {
      c_parsed_node = buildDoubleType();
      result = true;
    }
    else if (afs_match_substr("signed", checkTrail))
    {
      // ROSE does not have a dedicated node for signed or unsigned. 
      // we abuse SgTypeSignedLongLong SgTypeUnSignedLongLong to represent them
      // caller of this function is responsible for decode the return type and 
      // assemble the accurate type.
      c_parsed_node = buildSignedLongLongType();
      result = true;
    }
    else if (afs_match_substr("unsigned", checkTrail))
    {
      c_parsed_node = buildUnsignedLongLongType();
      result = true;
    } 
    //TODO struct_or_union_specifier
    //TODO num_specifier
    //TODO TYPE_NAME

    if (result == false)   c_char = old_char;
    return result;
  }
  /*
     Yacc grammar  
     specifier_qualifier_list
     : type_specifier specifier_qualifier_list
     | type_specifier
     | type_qualifier specifier_qualifier_list
     | type_qualifier
     ;
     ANTLR grammar: + means 1 or more occurence 
     specifier_qualifier_list
     : ( type_qualifier | type_specifier )+
     ;

*/
  bool afs_match_specifier_qualifier_list(std::vector<SgNode*> & sq_list)
  {
    bool result = false;  // result of the entire function
    bool cur_result = false; // result of match one occurence
    const char* old_char = c_char;

    bool match_qualifier = false;
    bool match_specifier = false;

    do {
      old_char= c_char;
      match_qualifier = afs_match_type_qualifier();
      if (!match_qualifier)
        match_specifier = afs_match_type_specifier ();
      if (match_qualifier  || match_specifier)
      {
        cur_result = true;
        result = true; // one occurrence is sufficient for a successful match
        sq_list.push_back(c_parsed_node);
      }
      else
      {
        cur_result = false;
        c_char = old_char;
      }
    } while (cur_result);

    return result;
  }

  /*
     YACC
     cast_expression
     : unary_expression
     | '(' type_name ')' cast_expression
     ;

     ANTLR
     cast_expression
     : '(' type_name ')' cast_expression
     | unary_expression
     ;

*/
  bool afs_match_cast_expression()
  {
    bool result = false;
    const char* old_char = c_char;

    if (afs_match_unary_expression())
    {
      if (isSgExpression(c_parsed_node))
      result = true;
    }
    else if (afs_match_char('('))
    {
      if (afs_match_type_name())
      {
        SgType* t = isSgType(c_parsed_node);
        assert (t!= NULL);
        if (afs_match_char(')'))
        {
          if (afs_match_cast_expression())
          {
            SgExpression* operand = isSgExpression(c_parsed_node);
            c_parsed_node = buildCastExp(operand, t);
            result = true; // must set this!!
          }
          else
          {
            c_char = old_char; 
            // printf("error. afs_match_cast_expression() expects cast_exp after matching (type_name) \n");
            // assert(0);
          }
        }
        else
        {
          c_char = old_char; 
          //printf("error. afs_match_cast_expression() expects ) after matching (type_name \n");
          //assert(0);
        }

      }
      else 
      {
        //printf("error. afs_match_cast_expression() expects type_name after matching ( \n");
        //assert(0);
        // should not assert here since unary_expression may start with '(' also!
        c_char = old_char; 
        result = false;
      }
    }

    if (result == false)   c_char = old_char;
    return result;
  }
  /*

     ANTLR grammar 
     multiplicative_expression
     : (cast_expression) ('*' cast_expression | '/' cast_expression | '%' cast_expression)*
     ;
     YACC grammar

     multiplicative_expression
     : cast_expression
     | multiplicative_expression '*' cast_expression // SgMultiplyOp
     | multiplicative_expression '/' cast_expression  // SgDivideOp
     | multiplicative_expression '%' cast_expression   //SgModOp

   * */
  bool afs_match_multiplicative_expression()
  {
 // DQ (12/9/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
 // bool result = false;
    const char* old_char = c_char;

    SgExpression * lhs = NULL;
    if (afs_match_cast_expression())
    {
      lhs = isSgExpression(c_parsed_node);
      assert (lhs != NULL);
   // c_parsed_node = lhs; 

   // DQ (12/9/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
   // result = true; // sufficient enough
    }
    else // immediate return if the first term match fails
    {
      c_char = old_char;
      return false;
    } 
    // later failure should not invalid previous success

    // try to match optional one or more +/- multi_exp
    // optional match may fail, set a rollback point first
    old_char = c_char; // rollback point
    bool is_multiply = false; 
    bool is_divide = false; 
    bool is_mod = false; 
    is_multiply = afs_match_char('*');
    if (!is_multiply )
      is_divide = afs_match_char('/');
    if (!is_divide)
      is_mod= afs_match_char('%');

    while (is_multiply || is_divide || is_mod)
    {
      if (afs_match_cast_expression())
      {
        SgExpression* rhs = isSgExpression(c_parsed_node);
        assert (rhs != NULL);
        if (is_multiply)
        {
          c_parsed_node = buildMultiplyOp(lhs, rhs);
          lhs = isSgExpression(c_parsed_node); // reset lhs to prepare for the next round
        }
        else if (is_divide)
        {
          c_parsed_node = buildDivideOp(lhs, rhs);
          lhs = isSgExpression(c_parsed_node); 
        }
        else if (is_mod)
        {
          c_parsed_node = buildModOp(lhs, rhs);
          lhs = isSgExpression(c_parsed_node); 
        }
        else
        {
          printf("error. afs_match_multiplicative_expression() illegal branch reached.\n");
          assert (0);
        }
     // DQ (12/9/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
     // result = true;
      }
      else 
      {
     // DQ (12/9/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
     // result = false;
        c_char = old_char; // this round failed, restore to the lastes rollback point
        //printf("error. afs_match_multiplicative_expression() expects cast_expression after matching * / or percentage character\n");
        //assert (0);
        break;
      }

      // start the next round
      old_char = c_char; // new roolback point
      is_multiply = false;
      is_divide = false;
      is_mod = false;
      is_multiply = afs_match_char('*');
      if (!is_multiply )
        is_divide = afs_match_char('/');
      if (!is_divide)
        is_mod= afs_match_char('%');
    } // end while  

    return true; // always true if program can reach this point

  }


  /*
   *
   Yacc grammar
   additive_expression
   : multiplicative_expression
   | additive_expression '+' multiplicative_expression
   | additive_expression '-' multiplicative_expression

   ANTLR format
   additive_expression
   : (multiplicative_expression) ('+' multiplicative_expression | '-' multiplicative_expression)*
   ;
   * */
  bool afs_match_additive_expression()
  {
 // DQ (12/9/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
 // bool result = false;
    const char* old_char = c_char;

    SgExpression * lhs = NULL;
    if (afs_match_multiplicative_expression())
    {
      lhs = isSgExpression(c_parsed_node);
      assert (lhs != NULL);
   // c_parsed_node = lhs; 

   // DQ (12/9/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
   // result = true; // sufficient enough
    }
    else
    { // immediate return if first term match fails
      c_char = old_char; // not really necessary here
      return false;
    } 

    // try to match optional one or more +/- multi_exp
    old_char = c_char;
    SgNode * old_parsed_node = c_parsed_node;

    bool is_plus = false; 
    bool is_minus = false; 
    is_plus = afs_match_char('+');
    if (!is_plus )
      is_minus = afs_match_char('-');

    SgExpression* rhs = NULL;
    while (is_plus || is_minus)
    {
      if (afs_match_multiplicative_expression())
      {
        rhs = isSgExpression(c_parsed_node);
        assert (rhs != NULL);
        assert (lhs != NULL);
        if (is_plus)
        {
          c_parsed_node = buildAddOp(lhs, rhs);
          lhs = isSgExpression(c_parsed_node); // reset lhs to prepare for the next round
        }
        else if (is_minus)
        {
          c_parsed_node = buildSubtractOp(lhs, rhs);
          lhs = isSgExpression(c_parsed_node); 
        }
        else
        {
          assert (0);
        }
        //result = true;
      }
      else 
      {
        c_char = old_char;
        c_parsed_node = old_parsed_node;
        //printf("error in afs_match_additive_expression(): expects multiplicative_expression after matching '+' or '-'\n");
        //assert (0);
        break;
      }

      // start the next round
      old_char = c_char;
      old_parsed_node = c_parsed_node;
      is_plus = false; 
      is_minus = false; 
      is_plus = afs_match_char('+');
      if (!is_plus ) 
        is_minus = afs_match_char('-');
    }

    return true;
  }

  bool afs_match_lvalue()
  {
    if (afs_match_unary_expression())
      return true;
    else
      return false; 
  }
  /*
     ANTLR
     conditional_expression
     : logical_or_expression ('?' expression ':' conditional_expression)?
     ;

     ? means 0 or 1 occurrences
     */
  bool afs_match_conditional_expression()
  {
#if 0
    //TODO implement anything between condition_expression and additive_expression
    if (afs_match_additive_expression())
      return true;
    else
      return false;
#endif
 // DQ (12/9/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
 // bool result = false;    
    const char* old_char = c_char;
    // match the required first term
    if (afs_match_logical_or_expression())
    {
   // DQ (12/9/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
   // result = true;
    }
    else 
    {
      c_char = old_char;
      return false;
    }

    // match the optional 2nd term
    // to compose a SgConditionalExp (conditional_exp, true_exp, false_exp)
    // preserve context first
    SgExpression* cond_exp = isSgExpression(c_parsed_node);
    assert (cond_exp != NULL);
    old_char = c_char;
    bool result2 = false;

    if (afs_match_char('?'))
    {
      if (afs_match_expression())
      {
        SgExpression* true_exp = isSgExpression(c_parsed_node);
        assert (true_exp != NULL);
        if (afs_match_char(':'))
        {
          if (afs_match_conditional_expression())
          {
            SgExpression* false_exp = isSgExpression(c_parsed_node);
            assert (false_exp != NULL);
            c_parsed_node = buildConditionalExp(cond_exp, true_exp, false_exp);
            result2 = true;
          }
        }       
      }
    }
    if (!result2) // store contex if term2 is not matched.
    {
      c_char = old_char;
      c_parsed_node = cond_exp;
    }

    return true;
  }
  /*
     logical_or_expression
     : logical_and_expression ('||' logical_and_expression)*
     ;

*/
  bool afs_match_logical_or_expression()
  {
 // DQ (12/9/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
 // bool result = false;
    const char* old_char = c_char;

    SgExpression* lhs = NULL;
    if (afs_match_logical_and_expression())
    {
      lhs = isSgExpression(c_parsed_node);
      assert (lhs != NULL);

   // DQ (12/9/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
   // result = true; // sufficient enough
    }
    else
    { // immediate return if first term match fails
      c_char = old_char; // not really necessary here
      return false;
    }

    // try to match optional one or more +/- multi_exp
    old_char = c_char;
    SgExpression* rhs = NULL;
    while (afs_match_substr("||", false))
    {
      if (afs_match_logical_and_expression())
      {
        rhs = isSgExpression(c_parsed_node);
        assert (rhs != NULL);
        assert (lhs != NULL);
        c_parsed_node = buildOrOp(lhs, rhs);
        lhs = isSgExpression(c_parsed_node);
      }
      else
      {
        c_char = old_char;
        //printf("error in afs_match_logical_or_expression(): expects logical_and_expression after matching '||' \n");
        //assert (0);
        break;
      }
      // start the next round
      old_char = c_char;
    }
    return true;   
  }

  /*
     logical_and_expression
     : inclusive_or_expression ('&&' inclusive_or_expression)*
     ;
     */
  bool afs_match_logical_and_expression()
  {
 // DQ (12/9/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
 // bool result = false;
    const char* old_char = c_char;

    SgExpression* lhs = NULL;
    if (afs_match_inclusive_or_expression())
    {
      lhs = isSgExpression(c_parsed_node);
      assert (lhs != NULL);
   // DQ (12/9/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
   // result = true; // sufficient enough
    }
    else
    { // immediate return if first term match fails
      c_char = old_char; // not really necessary here
      return false;
    }

    // try to match optional one or more +/- multi_exp
    old_char = c_char;
    SgExpression* rhs = NULL;
    while (afs_match_substr("&&", false))
    {
      if (afs_match_inclusive_or_expression())
      {
        rhs = isSgExpression(c_parsed_node);
        assert (rhs != NULL);
        assert (lhs != NULL);
        c_parsed_node = buildAndOp(lhs, rhs);
        lhs = isSgExpression(c_parsed_node);
      }
      else
      {
        c_char = old_char;
        // printf("error in afs_match_logical_and_expression(): expects inclusive_or_expression after matching '&&'\n");
        // assert (0);
        break;
      }
      // start the next round
      old_char = c_char;
    }
    return true;
  }

  /*
     inclusive_or_expression
     : exclusive_or_expression ('|' exclusive_or_expression)*
     ;
     */
  bool afs_match_inclusive_or_expression()
  {
 // DQ (12/9/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
 // bool result = false;
    const char* old_char = c_char;

    SgExpression* lhs = NULL;
    if (afs_match_exclusive_or_expression())
    {
      lhs = isSgExpression(c_parsed_node);
      assert (lhs != NULL);

   // DQ (12/9/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
   // result = true; // sufficient enough
    }
    else
    { // immediate return if first term match fails
      c_char = old_char; // not really necessary here
      return false;
    }

    // try to match optional one or more +/- multi_exp
    old_char = c_char;
    SgExpression* rhs = NULL;
    while (afs_match_char('|'))
    {
      if (afs_match_exclusive_or_expression())
      {
        rhs = isSgExpression(c_parsed_node);
        assert (rhs != NULL);
        assert (lhs != NULL);
        c_parsed_node = buildBitOrOp(lhs, rhs); //TODO verify those operator types!!
        lhs = isSgExpression(c_parsed_node);
      }
      else
      {
        c_char = old_char;
        //printf("error in afs_match_inclusive_or_expression(): expects exclusive_or_expression after matching '|'\n");
        //assert (0);
        break;
      }
      // start the next round
      old_char = c_char;
    }
    return true;
  }

  /*
     exclusive_or_expression
     : and_expression ('^' and_expression)*
     ;
     */
  bool afs_match_exclusive_or_expression()
  {
 // DQ (12/9/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
 // bool result = false;
    const char* old_char = c_char;

    SgExpression* lhs = NULL;
    if (afs_match_and_expression())
    {
      lhs = isSgExpression(c_parsed_node);
      assert (lhs != NULL);
   // DQ (12/9/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
   // result = true; // sufficient enough
    }
    else
    { // immediate return if first term match fails
      c_char = old_char; // not really necessary here
      return false;
    }

    // try to match optional one or more +/- multi_exp
    old_char = c_char;
    SgExpression* rhs = NULL;
    while (afs_match_char('^'))
    {
      if (afs_match_and_expression())
      {
        rhs = isSgExpression(c_parsed_node);
        assert (rhs != NULL);
        assert (lhs != NULL);
        c_parsed_node = buildBitXorOp(lhs, rhs); // bitwise XOR a^b
        lhs = isSgExpression(c_parsed_node);
      }
      else
      {
        c_char = old_char;
        //printf("error in afs_match_exclusive_expression(): expects and_expression after matching '^'\n");
        //assert (0);
        break;
      }
      // start the next round
      old_char = c_char;
    }
    return true;
  }

  /*
     and_expression
     : equality_expression ('&' equality_expression)*
     ;
     */
  bool afs_match_and_expression()
  {
 // DQ (12/9/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
 // bool result = false;
    const char* old_char = c_char;

    SgExpression* lhs = NULL;
    if (afs_match_equality_expression())
    {
      lhs = isSgExpression(c_parsed_node);
      assert (lhs != NULL);
   // DQ (12/9/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
   // result = true; // sufficient enough
    }
    else
    { // immediate return if first term match fails
      c_char = old_char; // not really necessary here
      return false;
    }

    // try to match optional one or more +/- multi_exp
    old_char = c_char;
    SgExpression* rhs = NULL;
    while (afs_match_char('&'))
    {
      if (afs_match_equality_expression())
      {
        rhs = isSgExpression(c_parsed_node);
        assert (rhs != NULL);
        assert (lhs != NULL);
        c_parsed_node = buildBitAndOp(lhs, rhs);
        lhs = isSgExpression(c_parsed_node);
      }
      else
      {
        c_char = old_char;
        //  printf("error in afs_match_and_expression(): expects equality_expression after matching '&'\n");
        // assert (0);
        break;
      }
      // start the next round
      old_char = c_char;
    }
    return true;
  }

  /*
     equality_expression
     : relational_expression (('=='|'!=') relational_expression)*
     ;
     */
  bool afs_match_equality_expression()
  {
 // DQ (12/9/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
 // bool result = false;
    const char* old_char = c_char;

    SgExpression * lhs = NULL;
    if (afs_match_relational_expression())
    {
      lhs = isSgExpression(c_parsed_node);
      assert (lhs != NULL);
   // c_parsed_node = lhs;
   // DQ (12/9/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
   // result = true; // sufficient enough
    }
    else
    { // immediate return if first term match fails
      c_char = old_char; // not really necessary here
      return false;
    }

    // try to match optional one or more +/- multi_exp
    old_char = c_char;
    bool is_equal= false;
    bool is_not_equal = false;
    is_equal = afs_match_substr("==", false);
    if (!is_equal )
      is_not_equal = afs_match_substr("!=", false);

    SgExpression* rhs = NULL;
    while (is_equal || is_not_equal )
    {
      if (afs_match_relational_expression())
      {
        rhs = isSgExpression(c_parsed_node);
        assert (rhs != NULL);
        assert (lhs != NULL);
        if (is_equal)
        {
          c_parsed_node = buildEqualityOp(lhs, rhs);
          lhs = isSgExpression(c_parsed_node); // reset lhs to prepare for the next round
        }
        else if (is_not_equal)
        {
          c_parsed_node = buildNotEqualOp(lhs, rhs);
          lhs = isSgExpression(c_parsed_node);
        }
        else
        {
          assert (0);
        }
        //result = true;
      }
      else
      {
        c_char = old_char;
        //  printf("error in afs_match_additive_expression(): expects multiplicative_expression after matching '+' or '-'\n");
        // assert (0);
        break;
      }

      // start the next round
      old_char = c_char;
      is_equal= false;
      is_not_equal = false;
      is_equal = afs_match_substr("==", false);
      if (!is_equal )
        is_not_equal = afs_match_substr("!=", false);
    }

    return true;
  }


  /*
     relational_expression
     : shift_expression (('<'|'>'|'<='|'>=') shift_expression)*
     ;
     */
  bool afs_match_relational_expression()
  {
 // DQ (12/9/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
 // bool result = false;
    const char* old_char = c_char;

    SgExpression * lhs = NULL;
    if (afs_match_shift_expression())
    {
      lhs = isSgExpression(c_parsed_node);
      assert (lhs != NULL);
   // c_parsed_node = lhs;
   // DQ (12/9/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
   // result = true; // sufficient enough
    }
    else
    { // immediate return if first term match fails
      c_char = old_char; // not really necessary here
      return false;
    }

    // try to match optional one or more +/- multi_exp
    old_char = c_char;
    bool is_less_equal = false;
    bool is_larger_equal = false;
    bool is_less = false;
    bool is_larger = false;

    is_less_equal = afs_match_substr("<=", false);
    if (!is_less_equal)
      is_larger_equal = afs_match_substr(">=", false); 
    if (!is_larger_equal)
      is_less = afs_match_char('<');
    if (!is_less)
      is_larger = afs_match_char('>');

    SgExpression* rhs = NULL;
    while (is_less_equal || is_larger_equal || is_less || is_larger)
    {
      if (afs_match_shift_expression())
      {
        rhs = isSgExpression(c_parsed_node);
        assert (rhs != NULL);
        assert (lhs != NULL);
        if (is_less_equal)
        {
          c_parsed_node = buildLessOrEqualOp(lhs, rhs);
          lhs = isSgExpression(c_parsed_node); // reset lhs to prepare for the next round
        }
        else if (is_larger_equal)
        {
          c_parsed_node = buildGreaterOrEqualOp(lhs, rhs);
          lhs = isSgExpression(c_parsed_node);
        } 
        else if (is_less)
        {
          c_parsed_node = buildLessThanOp(lhs, rhs);
          lhs = isSgExpression(c_parsed_node); // reset lhs to prepare for the next round
        }
        else if (is_larger)
        {
          c_parsed_node = buildGreaterThanOp(lhs, rhs);
          lhs = isSgExpression(c_parsed_node);
        }
        else
        { // impossible branch
          assert (0);
        }
        //result = true;
      }
      else
      {
        // no match , rollback
        c_char = old_char;
        //printf("error in afs_match_additive_expression(): expects multiplicative_expression after matching '+' or '-'\n");
        //assert (0);
        break;
      }

      // start the next round
      old_char = c_char;
      is_less_equal = false;
      is_larger_equal = false;
      is_less = false;
      is_larger = false;

      is_less_equal = afs_match_substr("<=", false);
      if (!is_less_equal)
        is_larger_equal = afs_match_substr(">=", false);
      if (!is_larger_equal)
        is_less = afs_match_char('<');
      if (!is_less)
        is_larger = afs_match_char('>');
    }

    return true;
  }

  /*
     shift_expression
     : additive_expression (('<<'|'>>') additive_expression)*
     ;
     */
  bool afs_match_shift_expression()
  {
 // DQ (12/9/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
 // bool result = false;
    const char* old_char = c_char;

    SgExpression * lhs = NULL;
    if (afs_match_additive_expression())
    {
      lhs = isSgExpression(c_parsed_node);
      assert (lhs != NULL);
   // c_parsed_node = lhs;
   // DQ (12/9/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
   // result = true; // sufficient enough
    }
    else
    { // immediate return if first term match fails
      c_char = old_char; // not really necessary here
      return false;
    }

    // try to match optional one or more +/- multi_exp
    old_char = c_char;
    bool is_left = false;
    bool is_right = false;
    is_left = afs_match_substr("<<", false);
    if (!is_left)
      is_right = afs_match_substr(">>", false);

    SgExpression* rhs = NULL;
    while (is_left || is_right)
    {
      if (afs_match_additive_expression())
      {
        rhs = isSgExpression(c_parsed_node);
        assert (rhs != NULL);
        assert (lhs != NULL);
        if (is_left)
        {
          c_parsed_node = buildLshiftOp(lhs, rhs);
          lhs = isSgExpression(c_parsed_node); // reset lhs to prepare for the next round
        }
        else if (is_right)
        {
          c_parsed_node = buildRshiftOp(lhs, rhs);
          lhs = isSgExpression(c_parsed_node);
        }
        else
        {
          assert (0);
        }
        //result = true;
      }
      else
      {
        c_char = old_char;
        // printf("error in afs_match_shift_expression(): expects additive_expression after matching '<<' or '>>'\n");
        // assert (0);
        break;
      }

      // start the next round
      old_char = c_char;
      is_left = false;
      is_right = false;
      is_left = afs_match_substr("<<", false);
      if (!is_left)
        is_right= afs_match_substr(">>", false);
    }

    return true;
  }

  /*
     ANTLR grammar // this grammar is more right. We should try to match the longer rule first
     assignment_expression
      : lvalue assignment_operator assignment_expression
      | conditional_expression
      ;

     
     YACC grammar , this is ambiguous since conditional_expression could also a kind of unary_expression
     assignment_expression
     : conditional_expression
     | unary_expression assignment_operator assignment_expression

     assignment_operator
     : '='
     | MUL_ASSIGN  // *=
     | DIV_ASSIGN // /=
     | MOD_ASSIGN  // %=
     | ADD_ASSIGN  // +=
     | SUB_ASSIGN  // -=
     | LEFT_ASSIGN  // <<=
     | RIGHT_ASSIGN // >>=
     | AND_ASSIGN // &=
     | XOR_ASSIGN // ^= 
     | OR_ASSIGN // |=

     ">>="                   { count(); return(RIGHT_ASSIGN); }
     "<<="                   { count(); return(LEFT_ASSIGN); }
     "+="                    { count(); return(ADD_ASSIGN); }
     "-="                    { count(); return(SUB_ASSIGN); }
     "*="                    { count(); return(MUL_ASSIGN); }
     "/="                    { count(); return(DIV_ASSIGN); }
     "%="                    { count(); return(MOD_ASSIGN); }
     "&="                    { count(); return(AND_ASSIGN); }
     "^="                    { count(); return(XOR_ASSIGN); }
     "|="                    { count(); return(OR_ASSIGN); }

*/

  bool afs_match_assignment_expression()
  {
    bool result = false;
    const char* old_char = c_char;
    SgNode* old_parsed_node = c_parsed_node;

    if (afs_match_lvalue())
    {
      assert (c_parsed_node != NULL);
      SgExpression* lhs = isSgExpression(c_parsed_node);
      assert (lhs != NULL);

      VariantT op_type; 
      bool b_match_op = true;
      if (afs_match_char('='))
        op_type = V_SgAssignOp;
      else if (afs_match_substr("*=", false))
        op_type = V_SgMultAssignOp;
      else if (afs_match_substr("/=", false))
        op_type = V_SgDivAssignOp;
      else if (afs_match_substr("%=", false))
        op_type = V_SgModAssignOp;
      else if (afs_match_substr("+=", false))
        op_type = V_SgPlusAssignOp;
      else if (afs_match_substr("-=", false))
        op_type = V_SgMinusAssignOp;
      else if (afs_match_substr("<<=", false))
        op_type = V_SgLshiftAssignOp;
      else if (afs_match_substr(">>=", false))
        op_type = V_SgRshiftAssignOp;
      else if (afs_match_substr("&=", false))
        op_type = V_SgAndAssignOp;
      else if (afs_match_substr("^=", false))
        op_type = V_SgXorAssignOp;
      else if (afs_match_substr("|=", false))
        op_type = V_SgIorAssignOp;
      else
        b_match_op = false;

      if (b_match_op)
        if (afs_match_assignment_expression())
        {
          SgExpression* rhs = isSgExpression(c_parsed_node);
          assert (rhs != NULL);
          switch (op_type)
          {
            case V_SgAssignOp:
              c_parsed_node = buildAssignOp(lhs, rhs);
              break;
            case V_SgMultAssignOp:
              c_parsed_node = buildMultAssignOp(lhs, rhs);
              break;
            case V_SgDivAssignOp:
              c_parsed_node = buildDivAssignOp(lhs, rhs);
              break;
            case V_SgModAssignOp:
              c_parsed_node = buildModAssignOp(lhs, rhs);
              break;
            case V_SgPlusAssignOp:
              c_parsed_node = buildPlusAssignOp(lhs, rhs);
              break;
            case V_SgMinusAssignOp:
              c_parsed_node = buildMinusAssignOp(lhs, rhs);
              break;
            case V_SgLshiftAssignOp:
              c_parsed_node = buildLshiftAssignOp(lhs, rhs);
              break;
            case V_SgRshiftAssignOp:
              c_parsed_node = buildRshiftAssignOp(lhs, rhs);
              break;
            case V_SgAndAssignOp:
              c_parsed_node = buildAndAssignOp(lhs, rhs);
              break;
            case V_SgXorAssignOp:
              c_parsed_node = buildXorAssignOp(lhs, rhs);
              break;
            case V_SgIorAssignOp:
              c_parsed_node = buildIorAssignOp(lhs, rhs);
              break;
            default:
              assert (false);
          }  
          result = true;
        } 
    } 

   if (!result)
   {
      c_char = old_char; // must restore first
      c_parsed_node = old_parsed_node ;
      // if first term match fails
      if (afs_match_conditional_expression())
      {
        if (isSgExpression(c_parsed_node))
        result = true;
      }
   }

    if (result == false)   c_char = old_char;
    return result;

  }

  /*
     argument_expression_list
     : assignment_expression
     | argument_expression_list ',' assignment_expression


     Or in ANTLR
     argument_expression_list
     : assignment_expression (',' assignment_expression)*
     ;

*/
  bool afs_match_argument_expression_list()
  {
 // DQ (12/9/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
 // bool result =false; 
    const char* old_char = c_char;

    SgExprListExp* parameters = NULL;
    if (afs_match_assignment_expression())
    {
      SgExpression* arg1 = isSgExpression(c_parsed_node);
      assert (arg1 != NULL);
      parameters = buildExprListExp(arg1);
      c_parsed_node = parameters;
   // DQ (12/9/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
   // result = true;
    }
    else
    { // immediate return false when first required term is not matched
      c_char = old_char;
      return false;
    }

    // match optional additional expressions  
    old_char = c_char; // set rollback point
    while (afs_match_char(','))
    {
      if (afs_match_assignment_expression())
      {
        SgExpression* argx = isSgExpression(c_parsed_node);
        assert(argx != NULL); 
        appendExpression(parameters, argx);
        c_parsed_node = parameters; // must set it again since it was rewritten in match_assignment_expression()
      }
      else 
      {
        c_char = old_char;  // optional match fails, rollback
        //    printf("error. afs_match_argument_expression_list() expects assignment_expression after matching ','\n");
        //    assert (0);
        break;             // and break out
      }
      // prepare next round
      old_char = c_char; 
    } 

    assert (parameters != NULL);
    //    c_parsed_node = parameters; // this is necessary since the while loop may rewrite c_parsed_node
    assert (c_parsed_node == parameters);

    return true;
  }
  /*
   * 
YACC: left recursion!!
postfix_expression
: primary_expression
| postfix_expression '[' expression ']'
| postfix_expression '(' ')'
| postfix_expression '(' argument_expression_list ')'
| postfix_expression '.' IDENTIFIER
| postfix_expression PTR_OP IDENTIFIER
| postfix_expression INC_OP
| postfix_expression DEC_OP
;

ANTLR
postfix_expression
:   primary_expression
(   '[' expression ']'
| '(' ')'
| '(' argument_expression_list ')'
| '.' IDENTIFIER
| '->' IDENTIFIER
| '++'
| '--'
)*
; 

In a simpler form
postfix_expression
: primary_expression (postfix_operator) *
;

postfix_operator
: '[' expression ']'
| '(' ')'
| '(' argument_expression_list ')'
| '.' IDENTIFIER
| '->' IDENTIFIER
| '++'
| '--'
;

   * */
  bool afs_match_postfix_expression()
  {
 // DQ (12/9/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
 // bool result = false;
    const char* old_char = c_char;

    SgExpression * post_exp = NULL; // previous postfix expression,
    if (afs_match_primary_expression()) 
    {
   // DQ (12/9/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
   // result = true; // match rule1 , but don't stop or branch out here. Need to match max length rules
      assert (c_parsed_node != NULL);
      post_exp = isSgExpression(c_parsed_node);
      assert (post_exp != NULL);
    }
    else
    { // roll back and return false, since the first term is a must
      c_char = old_char;
      return false;
    }

    // optionally match the rest terms

    old_char = c_char;
    bool is_left_sb = false; // left square bracket [
    bool is_left_paren = false;  // left (
    bool is_dot = false;
    bool is_arrow = false;
    bool is_plusplus = false;
    bool is_minusminus = false;

    // try to match optional one of the postfix operators
    is_left_sb = afs_match_char('[');
    if (!is_left_sb)
      is_left_paren = afs_match_char('(');
    if (!is_left_paren)
      is_dot = afs_match_char('.');
    if (!is_dot)
      is_arrow = afs_match_substr("->", false);
    if (!is_arrow)
      is_plusplus = afs_match_substr("++", false);
    if (!is_plusplus)
      is_minusminus = afs_match_substr("--", false);

    while (is_left_sb||is_left_paren||is_dot||is_arrow||is_plusplus||is_minusminus)
    {
      if (is_left_sb) // '[' expression ']'
      {
        if (afs_match_expression()) // this will overwrite c_parsed_node
        {  
          if (afs_match_char(']'))
          {
            SgExpression* arr_ref = isSgExpression(c_parsed_node);
            assert (arr_ref != NULL);
            assert (post_exp != NULL);
            c_parsed_node = buildPntrArrRefExp(post_exp, arr_ref);
            post_exp = isSgExpression(c_parsed_node);  //  update the previous postfix exp
            //result = true;
          }
          else
          {// optional match fails, rollback and breakout
            c_char = old_char;
            break;
            //result = false;  
          }
        }
        else 
        {
          c_char = old_char;
          break;
          //result = false;  
        }
      }
      else  if (is_left_paren)
      { // function call exp (argument_expression_list) 
        if (afs_match_argument_expression_list())
        {
          assert (c_parsed_node != NULL);
          //cout<<"debug: "<< c_parsed_node->class_name()<<endl;
          SgExprListExp* parameters = isSgExprListExp(c_parsed_node);
          assert (parameters != NULL);
          if (afs_match_char(')'))
          {
            assert (post_exp != NULL);
            c_parsed_node = buildFunctionCallExp (post_exp, parameters);
            post_exp = isSgExpression(c_parsed_node);  

         // DQ (12/9/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
         // result =true;
          }
          else 
          {
            c_char = old_char;
            break;
          }
        }
        else if (afs_match_char(')'))
        {
          assert (post_exp != NULL);
          c_parsed_node = buildFunctionCallExp (post_exp, NULL);
          post_exp = isSgExpression(c_parsed_node);  
          //result = true;
        }
        else // neither of the two cases,something is wrong
        {
          c_char = old_char;
          break;
          //  result = false;
        }
      }
      else if (is_dot)
      {
        if (afs_match_identifier())
        {
          assert (c_parsed_node != NULL);
          assert (isSgExpression(c_parsed_node));
          assert (post_exp != NULL);
          c_parsed_node = buildDotExp(post_exp, isSgExpression(c_parsed_node) );
          post_exp = isSgExpression(c_parsed_node);  
        }
        else 
        {
          c_char = old_char;
          break;
        }
      }  
      else if (is_arrow)
      {
        if (afs_match_identifier())
        {
          assert (c_parsed_node != NULL);
          assert (isSgExpression(c_parsed_node));
          assert (post_exp != NULL);
          c_parsed_node = buildArrowExp(post_exp, isSgExpression(c_parsed_node) );
          post_exp = isSgExpression(c_parsed_node);  
        }
        else 
        {
          c_char = old_char;
          break;
        }
      }  
      else if (is_plusplus)
      {
        assert (post_exp != NULL);
        c_parsed_node = buildPlusPlusOp(post_exp, SgUnaryOp::postfix);
        post_exp = isSgExpression(c_parsed_node);  
        //result = true;
      } 
      else if (is_minusminus)
      {
        assert (post_exp != NULL);
        c_parsed_node = buildMinusMinusOp (post_exp, SgUnaryOp::postfix);
        post_exp = isSgExpression(c_parsed_node);  
        // result = true;
      }
      else
      {
        printf("error. afs_match_postfix_expression() reaches impossible if-else-if branch.\n");
        assert(0);
      }    

      // try next round
      old_char = c_char; // set new rollback point
      is_left_sb = false; // left square bracket [
      is_left_paren = false;  // left (
      is_dot = false;
      is_arrow = false;
      is_plusplus = false;
      is_minusminus = false;

      // try to match optional one of the postfix operators
      is_left_sb = afs_match_char('[');
      if (!is_left_sb)
        is_left_paren = afs_match_char('(');
      if (!is_left_paren)
        is_dot = afs_match_char('.');
      if (!is_dot)
        is_arrow = afs_match_substr("->", false);
      if (!is_arrow)
        is_plusplus = afs_match_substr("++", false);
      if (!is_plusplus)
        is_minusminus = afs_match_substr("--", false);
    }// end while

    return true; 
  }

  /*
     YACC grammar
     expression
     : assignment_expression
     | expression ',' assignment_expression
     ;
     ANTLR grammar
     expression
     : assignment_expression (',' assignment_expression)*
     ;


   * */
  bool afs_match_expression()
  {
 // DQ (12/9/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
 // bool result = false;
    const char* old_char = c_char;

    SgExpression* lhs = NULL;
    if (afs_match_assignment_expression())
    {
      assert (c_parsed_node!= NULL);
      lhs = isSgExpression(c_parsed_node);
      assert (lhs != NULL);

   // DQ (12/9/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
   // result = true;
    }
    else
    { // immediate return if first term is not matched
   // DQ (12/9/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
   // result = false;
      c_char = old_char;
      return false;
    }

    // this is ambiguous when dealing with 'clause (exp, exp)'
    // it will generate one single CommaOpExp instead of one exp as expected.
    // Users must use assignmen_expression instead to avoid this conflict.
#if 1 

    // match 0 or more , assignment_expression
    // build SgCommaOpExp(lhs, c_parsed_node)

    old_char = c_char; // preserve rollback point
    while (afs_match_char(','))
    {
      if (afs_match_assignment_expression())
      {
        assert (c_parsed_node!= NULL);
        SgExpression* rhs = isSgExpression(c_parsed_node);
        assert (rhs != NULL);
        lhs = buildCommaOpExp(lhs, rhs);
        c_parsed_node = lhs;
      }
      else
      {
        // no match, rollback
        c_char = old_char;
        //error reporting
        //    printf("error: afs_match_expression(), expecting assignment_expression after ','\n");
        //    assert (0);
        break;
      }
      old_char = c_char; 
    }
#endif
    return true;
  }
  /*
     constant_expression
     : conditional_expression
     ;

*/
  bool afs_match_constant_expression()
  {
    if (afs_match_conditional_expression())
      return true;
    else
      return false;
  }

  /*
    pointer
        : '*'
        | '*' type_qualifier_list
        | '*' pointer
        | '*' type_qualifier_list pointer
        ;
   At this stage, previous parsing should already found a base type stored in c_parsed_node.
   Matching a pointer will generate a new type pointing to the base type
   * */
  bool afs_match_pointer (SgType* orig_type)
  {
    bool result = false;
    const char* old_char = c_char;

//    assert (c_parsed_node != NULL);
    assert (orig_type!= NULL);
    //TODO double check this assumption: declaration_specifiers should be in front of this pointer 
    SgType* base_type = orig_type;
    assert (base_type != NULL);

    if (afs_match_char('*') )
    {
      base_type = buildPointerType(base_type);
      c_parsed_node = base_type; 
      result = true; 

      // match optional one or more pointer
      while (afs_match_char('*'))
      {
        base_type = buildPointerType(base_type);
        c_parsed_node = base_type; 
      }
      //TODO handle type_qualifier_list  const, volatile

      return true; 
    }
    else
      c_char = old_char; // need to restore context

    return result; 
  }
  /*
    direct_declarator
      : IDENTIFIER
      | '(' declarator ')'
      | direct_declarator '[' constant_expression ']'
      | direct_declarator '[' ']'
      | direct_declarator '(' parameter_type_list ')'
      | direct_declarator '(' identifier_list ')'
      | direct_declarator '(' ')'
      ;
   * */
  bool afs_match_direct_declarator()
  {
     bool result = false;
     if (afs_match_identifier())
     {
        // we expect to see SgName, not a symbol yet.  
#ifndef NDEBUG
        SgName* sname = isSgName(c_parsed_node); 
        assert (sname != NULL);
#endif
        result = true;  
     } 
     // TODO other options 
     return result; 
  }
  /*
  declarator
      : pointer direct_declarator
      | direct_declarator
      ;
  */
   bool afs_match_declarator(SgType* orig_type, SgType** mod_type) 
   {
     bool result = false;

     if (afs_match_pointer(orig_type))
     {
       // store the modified type 
       *mod_type = isSgPointerType(c_parsed_node); 
       assert ((*mod_type) != NULL);
       if (afs_match_direct_declarator())
       {
         // c_parsed_node should store the identifier name now 
         result = true; 
       }
     }
     else if (afs_match_direct_declarator())
     {
       // c_parsed_node should store the identifier name now 
       result = true; 
     }  

     return result;  
   } 
   /*
    init_declarator 
        : declarator  // may store both modified type and an identifier like ** a 
        | declarator '=' initializer
        ;    
    * */
  bool afs_match_init_declarator(SgType* orig_type, SgType** mod_type, SgName** sname, SgExpression** initializer) 
  {
    bool result = false; 
    const char* old_char = c_char;

    if (afs_match_declarator(orig_type, mod_type))
    {
      *sname = isSgName(c_parsed_node);  // must preserve it here, later match will overwrite it!
      assert (sname);
      c_parsed_node = NULL; // prep for initializer
      if (afs_match_char('=') && afs_match_initializer())
      {
        *initializer = isSgExpression(c_parsed_node); 
        assert (initializer != NULL);
      }  
      return true; 
    }
    else
    {
      c_char = old_char;
    }
    return result;  
  }

  /*
     initializer
       : assignment_expression
       | '{' initializer_list '}'
       | '{' initializer_list ',' '}'
  ;
   * */
  bool afs_match_initializer()
  {
     bool result = false;
     if (afs_match_assignment_expression())
       result = true; 
     return result; 
  }
  // statements
  /*
    statement
      : labeled_statement
      | compound_statement
      | expression_statement
      | selection_statement
      | iteration_statement
      | jump_statement
      ;
   C89 only allows variable declarations (declarators) in the front section of a function definition.
   To support C++ style variable declarations in any places, we allow match_statement to 
   match declarator
   * */ 
  bool afs_match_statement()
  {
    bool result = false;
    if (afs_match_declaration())
      result = true;
    else if (afs_match_labeled_statement())
      result = true;
    else if (afs_match_compound_statement())  
      result = true;
    else if (afs_match_expression_statement())
      result = true;
    else if (afs_match_selection_statement())
      result = true;
    else if (afs_match_iteration_statement())
      result = true;
    else if (afs_match_jump_statement())
      result = true;
    return result;
  }
  /*
     labeled_statement
     : IDENTIFIER ':' statement
     | 'case' constant_expression ':' statement
     | 'default' ':' statement
     ;

     A case or default label shall appear only in a switch statement. Further
     constraints on such labels are discussed under the switch statement.

     Label names shall be unique within a function.
     */
  bool afs_match_labeled_statement()
  {
    bool result = false;
    const char* old_char = c_char;
    // IDENTIFIER ':' statement
    if (afs_match_identifier())
    {
      SgName* sn = isSgName(c_parsed_node);
      SgLabelSymbol* lsym = isSgLabelSymbol(c_parsed_node);
      if (sn != NULL || lsym != NULL)
      {
        if (afs_match_char(':') && afs_match_statement())
        {
          SgStatement* stmt = isSgStatement(c_parsed_node);
          assert (stmt != NULL);
          SgScopeStatement * scope = getScope(c_sgnode);
          assert (scope != NULL);
          // It is possible that a label statement is prebuilt when parsing goto IDENTIFIER.
          // we use the label statement there
          //SgLabelSymbol * lsym = scope->lookup_label_symbol (*sn);
          if (lsym)
          {
             c_parsed_node = lsym->get_declaration();
             // the prebuilt label statement has NULL pointer as its statement member  by default
             // we have set it to the real statement
             SgLabelStatement* lstmt = isSgLabelStatement(c_parsed_node);
             lstmt->set_statement(stmt);
             stmt->set_parent(lstmt);
          }
          else if (sn)
             c_parsed_node = buildLabelStatement (*sn, stmt, scope);
          else
          {
            //impossible branch reached.
            assert (0);
          }
          assert (isSgLabelStatement(c_parsed_node)!=NULL);   
          assert (isSgLabelStatement(c_parsed_node)->get_statement()!=NULL);   
          result = true;
        }
      }
      //TODO case, default
    }

    if (result == false)   c_char = old_char;
    return result;
  }
/*
Yacc grammar:

compound_statement
  : '{' '}'
  | '{' statement_list '}'
  | '{' declaration_list '}'
  | '{' declaration_list statement_list '}'
  ;


ANTLR grammar: 

compound_statement
scope Symbols; // blocks have a scope of symbols
@init {
  $Symbols::types = new HashSet();
}
  : '{' declaration* statement_list? '}'
  ;
 
 * */
  bool afs_match_compound_statement()
  {
    //TODO 
    return false;
  }
  /*
     expression_statement
     : ';'
     | expression ';'
     ;

   * */

  bool afs_match_expression_statement()
  {
    bool result = false;
    const char* old_char = c_char;

    if (afs_match_char(';'))
    {
      c_parsed_node = buildNullStatement();
      result = true; 
    }
    else if (afs_match_expression () && afs_match_char(';')) 
    {
      SgExpression * exp = isSgExpression(c_parsed_node);
      assert (exp != NULL); 
      c_parsed_node = buildExprStatement(exp);
      result = true; 
    }
    if (result == false)   c_char = old_char;
    return result;
  }

/*
ANTLR grammar
selection_statement
    : 'if' '(' expression ')' statement (options {k=1; backtrack=false;}:'else' statement)?
    | 'switch' '(' expression ')' statement
    ;

Yacc grammar

selection_statement
  : IF '(' expression ')' statement
  | IF '(' expression ')' statement ELSE statement
  | SWITCH '(' expression ')' statement
  ;

 */
  bool afs_match_selection_statement()
  {
    bool result = false;
    const char* old_char = c_char;

    if (afs_match_substr("if") && afs_match_char('(') && afs_match_expression() && afs_match_char(')'))
    {
      SgExpression* exp = isSgExpression (c_parsed_node);
      assert (exp !=NULL);
      if (! afs_match_statement())
      {
        printf("error. afs_match_selection_statement() expects statement after matching 'if' '(' expression ')'\n");
        assert(0);
      }
      SgStatement* true_stmt = isSgStatement(c_parsed_node);
      assert (true_stmt!=NULL);

      SgStatement* false_stmt = NULL;
      if (afs_match_substr("else") && afs_match_statement())
      {
         false_stmt = isSgStatement(c_parsed_node);
         assert (false_stmt != NULL);
      }
      c_parsed_node = buildIfStmt (buildExprStatement(exp), true_stmt, false_stmt);
      result = true;
    }  
    // TODO switch

    if (result == false)   c_char = old_char;
    return result;
  }

/*
iteration_statement
  : 'while' '(' expression ')' statement
  | 'do' statement 'while' '(' expression ')' ';'
  | 'for' '(' expression_statement expression_statement expression? ')' statement
  ;

*/
  bool afs_match_iteration_statement()
  {
    bool result = false;
    const char* old_char = c_char;
    
    // 'while' '(' expression ')' statement
    if (afs_match_substr("while") && afs_match_char('(') && afs_match_expression() && afs_match_char(')')) 
    {
      SgExpression* exp = isSgExpression (c_parsed_node);
      assert (exp != NULL);
      if (afs_match_statement())
      {
        SgStatement* stmt = isSgStatement(c_parsed_node);
        c_parsed_node = buildWhileStmt (buildExprStatement(exp), stmt);
        result = true;
      }  
    }
    // 'do' statement 'while' '(' expression ')' ';'
    else if (afs_match_substr("do") && afs_match_statement() && afs_match_substr("while") && afs_match_char('('))
    {
      SgStatement* stmt = isSgStatement(c_parsed_node);
      assert (stmt != NULL);
      if (!afs_match_expression())
      {
        printf("error. afs_match_iteration_statement() expects expression after matching 'do' statement 'while' '('\n");
        assert (0);
      }
      SgExpression* exp = isSgExpression (c_parsed_node);
      assert (exp != NULL);
       if (!afs_match_char(')'))
      {
        printf("error. afs_match_iteration_statement() expects ')' after matching 'do' statement 'while' '(' expression ')' \n");
        assert (0);
      }
 
      if (!afs_match_char(';'))
      {
        printf("error. afs_match_iteration_statement() expects ';' after matching 'do' statement 'while' '(' expression ')' \n");
        assert (0);
      }

      c_parsed_node = buildDoWhileStmt (stmt, buildExprStatement(exp));

      result = true;
    }
    // 'for' '(' expression_statement expression_statement expression? ')' statement
    else if (afs_match_substr("for") && afs_match_char('(') && afs_match_expression_statement()) 
    {
      SgStatement* init_stmt = isSgStatement (c_parsed_node);

      if (! afs_match_expression_statement() )
      {
         printf("error. afs_match_iteration_statement() expects 'expression_statement' after matching 'for' '(' expression_statement\n");
         assert (0);
      }
      SgStatement* test_stmt = isSgStatement (c_parsed_node);
      SgExpression* incr_exp = NULL;
      if (afs_match_expression())
        incr_exp = isSgExpression(c_parsed_node);
      if (! (afs_match_char(')') && afs_match_statement()))
      {
         printf("error. afs_match_iteration_statement() expects ')' statement after matching 'for' '(' expression_statement expression_statement expression? \n");
         assert (0);
      }
      SgStatement * body = isSgStatement(c_parsed_node);
      c_parsed_node = buildForStatement (init_stmt, test_stmt, incr_exp, body);
      result = true;
    }  

    if (result == false)
    {
      c_char = old_char;
    }
    return result;

  }
/*
ANTLR grammar:

jump_statement
  : 'goto' IDENTIFIER ';'
  | 'continue' ';'
  | 'break' ';'
  | 'return' ';'
  | 'return' expression ';'
  ;

*/
  bool afs_match_jump_statement()
  {
    bool result = false;
    const char* old_char = c_char;
    if (afs_match_substr("goto") && afs_match_identifier())
    {
      SgScopeStatement* scope = getScope(c_sgnode);
      assert (scope != NULL);

      // two cases: the label statement is 1) already created or to 2) be created later
      // case 1: the label statement is already created. afs_match_identifier() should return the label symbol
      SgLabelStatement* lstmt = NULL;
      if (SgLabelSymbol * lsym = isSgLabelSymbol(c_parsed_node))
       {
         lstmt = lsym->get_declaration();
       } 
       else if (SgName* sn = isSgName (c_parsed_node))
         //case 2: the label symbol is not yet created.  afs_match_identifier() should return the label SgName
       {
         // prebuild label statement here, fix the exact location later on when parsing the label statement
         lstmt = buildLabelStatement(*sn, NULL, scope);
       }  
       assert (lstmt != NULL);
       c_parsed_node = buildGotoStatement (lstmt);
       result = true;
    }
    else if (afs_match_substr("continue") && afs_match_char(';'))
    {
      c_parsed_node = buildContinueStmt();
      result = true;
    }
    else if (afs_match_substr("break") && afs_match_char(';'))
    {
      c_parsed_node = buildBreakStmt();
      result = true;
    }
    else if (afs_match_substr("return"))
    {
      if (afs_match_char(';'))
      {
        c_parsed_node = buildReturnStmt (NULL);
        result = true;
      } 
      else if (afs_match_expression())
      {
        SgExpression* exp = isSgExpression(c_parsed_node);
        c_parsed_node = buildReturnStmt(exp);
        result = true;
      }
    }

    if (result == false)   c_char = old_char;
    return result;
  }

/*

declaration_specifiers
  : storage_class_specifier
  | storage_class_specifier declaration_specifiers
  | type_specifier
  | type_specifier declaration_specifiers
  | type_qualifier
  | type_qualifier declaration_specifiers
  ;

OR ANTLR grammar

declaration_specifiers
  :   (   storage_class_specifier
        |   type_specifier
        |   type_qualifier
        )+
  ;

   */

bool afs_match_declaration_specifiers(SgType** tt)
{
  bool result = false;
  if (afs_match_type_specifier ())
  {
    *tt = isSgType(c_parsed_node); 
    ROSE_ASSERT ((*tt) != NULL);
    result = true;  
  } 
   //TODO other cases
  return result; 
}


/*
declaration
  : declaration_specifiers ';'
  | declaration_specifiers init_declarator_list ';'
  ;

ROSE only supports  int i;  int j; not int i,j,k;
so we take a short cut to match init_declartor() instead of init_declarator_list 
*/
  bool afs_match_declaration() {
    const char* old_char = c_char;
    bool result = false;

    SgType* orig_type= NULL;
    SgType* mod_type=NULL; 
    SgName* sname=NULL; 
    SgExpression* init_exp = NULL; 

   // common case of int i = 10;  don't forget the ending ; 
    if (afs_match_declaration_specifiers (&orig_type) && afs_match_init_declarator (orig_type, &mod_type, &sname, &init_exp) && afs_match_char(';'))
    {
      assert (orig_type != NULL);
      assert (sname != NULL);

      if (mod_type) // pointer types
        orig_type = mod_type; 

      SgScopeStatement * scope = getScope(c_sgnode);
      assert (scope != NULL);
      SgAssignInitializer *assign_init = NULL;
      if (init_exp)
        assign_init = buildAssignInitializer(init_exp, orig_type);

      c_parsed_node = SageBuilder::buildVariableDeclaration(*sname, orig_type, assign_init, scope);
      result = true; // must set this !! 
    } // TODO handle declaration_specifiers ';', have to roll back the side effects or previous branch
    else
      c_char = old_char;

#if 0
    if (!afs_match_type_name()) {
      c_char = old_char;
      return false;
    }
    SgType * type = isSgType(c_parsed_node);
    assert(type != NULL);

    if (!afs_match_identifier()) {
      c_char = old_char;
      return false;
    }
    SgName * ident = isSgName(c_parsed_node);
    assert(ident != NULL);

    afs_skip_whitespace();

    SgInitializer * init = NULL;
    if (afs_match_char('=')) {

      if (!afs_match_additive_expression()) {
        c_char = old_char;
        assert(false);
        return false;
      }
      SgExpression * expr = isSgExpression(c_parsed_node);
      assert(expr != NULL);
      init = SageBuilder::buildAssignInitializer(expr);
    }

    afs_skip_whitespace();

    if (!afs_match_char(';')) {
      c_char = old_char;
      return false;
    }

    SgScopeStatement * scope = getScope(c_sgnode);
    assert (scope != NULL);
    c_parsed_node = SageBuilder::buildVariableDeclaration(*ident, type, init, scope);
#endif
    return result;
  }

} // end namespace AstFromString

