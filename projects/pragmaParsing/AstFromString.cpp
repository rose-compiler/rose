/*
 Chunhua (Leo) Liao" <liao6@llnl.gov>
 */
#include "AstFromString.h"
#include <string>
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
  bool afs_skip_whitespace()
  {
    bool result = false;
    while ((*c_char)==' '||(*c_char)=='\t')
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
  //      whitespace, end of str, newline, tab, or '!'
  //      Set to true by default, used to ensure the matched substr is a full identifier/keywords.
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
    // TODO other cases??
    if (afs_is_letter())
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
  /*
     IDENTIFIER
     :     LETTER (LETTER|'0'..'9')*
     ;

     You need symbol table info to parse C
     IDENTIFIERS are either types or plain IDs
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
    SgSymbol* sym = lookupSymbolInParentScopes(buffer, c_scope);

    // type?
    SgTypeTable * gtt = SgNode::get_globalTypeTable();
    assert (gtt != NULL);
    SgType* t = gtt->lookup_type(SgName(buffer)); 
    assert (!(sym &&t)); // can be both a type or a variable?
    if (sym) 
    {
      assert (sym!=NULL);
      SgExpression* ref_exp = NULL;
      switch (sym->variantT())
      {
	case V_SgVariableSymbol:
	  ref_exp = buildVarRefExp(isSgVariableSymbol(sym));
	  break;
	case V_SgFunctionSymbol:
	  ref_exp = buildFunctionRefExp(isSgFunctionSymbol(sym));
	  break;
	default:
	  {
	    cerr<<"error: unhandled symbol type in afs_match_identifier():"<<sym->class_name()<<endl;
	    assert(false);
	  }
      }
      assert (ref_exp != NULL);
      c_parsed_node = ref_exp;  
    } else  if (t)
    {
      c_parsed_node = t;
    }
    else
    {
      //printf("cannot recoginize an identifier:^%s^ not a variable ref, not a type ref.\n",buffer);
      //c_parsed_node = NULL;
      //assert(0);
      c_char = old_char;
      return false;
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
    const char* old_char = c_char;
    if (afs_match_integer_const (&int_result))
    {
      result = true;
      //   cout<<"debug:building int val exp:"<<int_result<<endl;
      c_parsed_node = buildIntVal (int_result);
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
      result = true;
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
      result = true;
    else if (afs_match_substr("++"))
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
//	printf ("error. afs_match_unary_expression() expects unary_exp after matching ++\n");
//	assert (0);
	c_char = old_char;
      }
    } 
    else if (afs_match_substr("--"))
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
  bool decode_list (const std::vector<SgNode*>& sq_list)
  {
    bool result = false;
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
      result = true;
    }
    else
    { 
      // 5 integer types signed (optional)/ unsigned
      //   char, 
      //   int, short int, long int, long long int
      // other types: void, float , double, TODO: struct/union, enum, TYPE_NAME
      // const/volatile modifier (optional)

      // 2 or more items
      printf("error: AstFromString::decode_list(): not handled case.\n");
      assert (0);
    }  

    return result;
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
      assert (rt);
    }
    else
    {
      c_char = old_char;
      return false;
    }

    // TODO 
#if 0    
    if (afs_match_abstract_declarator())
    {
    }
#endif

    return true;
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
    //    	{System.out.println($IDENTIFIER.text+" is a type");}
        ;
 
  */
  bool afs_match_type_specifier()
  {
    bool result = false;
    const char* old_char = c_char;

    if (afs_match_substr("void"))
    {
      c_parsed_node = buildVoidType();
      result = true;
    }
    else if (afs_match_substr("char"))
    {
      c_parsed_node = buildCharType();
      result = true;
    }
    else if (afs_match_substr("short"))
    {
      c_parsed_node = buildShortType();
      result = true;
    }
    else if (afs_match_substr("int"))
    {
      c_parsed_node = buildIntType();
      result = true;
    }
    else if (afs_match_substr("long"))
    {
      c_parsed_node = buildLongType();
      result = true;
    }
    else if (afs_match_substr("float"))
    {
      c_parsed_node = buildFloatType();
      result = true;
    }
    else if (afs_match_substr("double"))
    {
      c_parsed_node = buildDoubleType();
      result = true;
    }
    else if (afs_match_substr("signed"))
    {
      // ROSE does not have a dedicated node for signed or unsigned. 
      // we abuse SgTypeSignedLongLong SgTypeUnSignedLongLong to represent them
      // caller of this function is responsible for decode the return type and 
      // assemble the accurate type.
      c_parsed_node = buildSignedLongLongType();
      result = true;
    }
    else if (afs_match_substr("unsigned"))
    {
      c_parsed_node = buildUnsignedLongLongType();
      result = true;
    } //TODO struct_or_union_specifier
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
	result = true; // one occurence is sufficient for a successful match
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
      result = true;
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
    bool result = false;
    const char* old_char = c_char;

    SgExpression * lhs = NULL;
    if (afs_match_cast_expression())
    {
      lhs = isSgExpression(c_parsed_node);
      assert (lhs != NULL);
      // c_parsed_node = lhs; 
      result = true; // sufficient enough
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
	result = true;
      }
      else 
      {
	result = false;
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
    bool result = false;
    const char* old_char = c_char;

    SgExpression * lhs = NULL;
    if (afs_match_multiplicative_expression())
    {
      lhs = isSgExpression(c_parsed_node);
      assert (lhs != NULL);
      // c_parsed_node = lhs; 
      result = true; // sufficient enough
    }
    else
    { // immediate return if first term match fails
      c_char = old_char; // not really necessary here
      return false;
    } 

    // try to match optional one or more +/- multi_exp
    old_char = c_char;
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
	//printf("error in afs_match_additive_expression(): expects multiplicative_expression after matching '+' or '-'\n");
	//assert (0);
	break;
      }

      // start the next round
      old_char = c_char;
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

   ? means 0 or 1 occurence
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
    bool result = false;    
    const char* old_char = c_char;
    // match the required first term
    if (afs_match_logical_or_expression())
    {
      result = true;
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
    bool result = false;
    const char* old_char = c_char;

    SgExpression* lhs = NULL;
    if (afs_match_logical_and_expression())
    {
      lhs = isSgExpression(c_parsed_node);
      assert (lhs != NULL);
      result = true; // sufficient enough
    }
    else
    { // immediate return if first term match fails
      c_char = old_char; // not really necessary here
      return false;
    }

    // try to match optional one or more +/- multi_exp
    old_char = c_char;
    SgExpression* rhs = NULL;
    while (afs_match_substr("||"))
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
    bool result = false;
    const char* old_char = c_char;

    SgExpression* lhs = NULL;
    if (afs_match_inclusive_or_expression())
    {
      lhs = isSgExpression(c_parsed_node);
      assert (lhs != NULL);
      result = true; // sufficient enough
    }
    else
    { // immediate return if first term match fails
      c_char = old_char; // not really necessary here
      return false;
    }

    // try to match optional one or more +/- multi_exp
    old_char = c_char;
    SgExpression* rhs = NULL;
    while (afs_match_substr("&&"))
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
    bool result = false;
    const char* old_char = c_char;

    SgExpression* lhs = NULL;
    if (afs_match_exclusive_or_expression())
    {
      lhs = isSgExpression(c_parsed_node);
      assert (lhs != NULL);
      result = true; // sufficient enough
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
    bool result = false;
    const char* old_char = c_char;

    SgExpression* lhs = NULL;
    if (afs_match_and_expression())
    {
      lhs = isSgExpression(c_parsed_node);
      assert (lhs != NULL);
      result = true; // sufficient enough
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
    bool result = false;
    const char* old_char = c_char;

    SgExpression* lhs = NULL;
    if (afs_match_equality_expression())
    {
      lhs = isSgExpression(c_parsed_node);
      assert (lhs != NULL);
      result = true; // sufficient enough
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
    bool result = false;
    const char* old_char = c_char;

    SgExpression * lhs = NULL;
    if (afs_match_relational_expression())
    {
      lhs = isSgExpression(c_parsed_node);
      assert (lhs != NULL);
      // c_parsed_node = lhs;
      result = true; // sufficient enough
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
    is_equal = afs_match_substr("==");
    if (!is_equal )
      is_not_equal = afs_match_substr("!=");

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
      is_equal = afs_match_substr("==");
      if (!is_equal )
	is_not_equal = afs_match_substr("!=");
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
    bool result = false;
    const char* old_char = c_char;

    SgExpression * lhs = NULL;
    if (afs_match_shift_expression())
    {
      lhs = isSgExpression(c_parsed_node);
      assert (lhs != NULL);
      // c_parsed_node = lhs;
      result = true; // sufficient enough
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

    is_less_equal = afs_match_substr("<=");
    if (!is_less_equal)
      is_larger_equal = afs_match_substr(">="); 
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

      is_less_equal = afs_match_substr("<=");
      if (!is_less_equal)
	is_larger_equal = afs_match_substr(">=");
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
    bool result = false;
    const char* old_char = c_char;

    SgExpression * lhs = NULL;
    if (afs_match_additive_expression())
    {
      lhs = isSgExpression(c_parsed_node);
      assert (lhs != NULL);
      // c_parsed_node = lhs;
      result = true; // sufficient enough
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
    is_left = afs_match_substr("<<");
    if (!is_left)
      is_right = afs_match_substr(">>");

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
      is_left = afs_match_substr("<<");
      if (!is_left)
        is_right= afs_match_substr(">>");
    }

    return true;
}

  /*
     ANTLR and YACC
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

    if (afs_match_conditional_expression())
      result = true;
    else if (afs_match_lvalue())
    {
      assert (c_parsed_node != NULL);
      SgExpression* lhs = isSgExpression(c_parsed_node);
      assert (lhs != NULL);

      VariantT op_type; 
      bool b_match_op = true;
      if (afs_match_substr("="))
	op_type = V_SgAssignOp;
      else if (afs_match_substr("*="))
	op_type = V_SgMultAssignOp;
      else if (afs_match_substr("/="))
	op_type = V_SgDivAssignOp;
      else if (afs_match_substr("%="))
	op_type = V_SgModAssignOp;
      else if (afs_match_substr("+="))
	op_type = V_SgPlusAssignOp;
      else if (afs_match_substr("-="))
	op_type = V_SgMinusAssignOp;
      else if (afs_match_substr("<<="))
	op_type = V_SgLshiftAssignOp;
      else if (afs_match_substr(">>="))
	op_type = V_SgRshiftAssignOp;
      else if (afs_match_substr("&="))
	op_type = V_SgAndAssignOp;
      else if (afs_match_substr("^="))
	op_type = V_SgXorAssignOp;
      else if (afs_match_substr("|="))
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

    bool result =false; 
    const char* old_char = c_char;

    SgExprListExp* parameters = NULL;
    if (afs_match_assignment_expression())
    {
      SgExpression* arg1 = isSgExpression(c_parsed_node);
      assert (arg1 != NULL);
      parameters = buildExprListExp(arg1);
      c_parsed_node = parameters;
      result = true;
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
//	printf("error. afs_match_argument_expression_list() expects assignment_expression after matching ','\n");
//	assert (0);
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
    bool result = false;
    const char* old_char = c_char;

    SgExpression * post_exp = NULL; // previous postfix expression,
    if (afs_match_primary_expression()) 
    {
      result = true; // match rule1 , but don't stop or branch out here. Need to match max length rules
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
      is_arrow = afs_match_substr("->");
    if (!is_arrow)
      is_plusplus = afs_match_substr("++");
    if (!is_plusplus)
      is_minusminus = afs_match_substr("--");

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
	    result =true;
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
	is_arrow = afs_match_substr("->");
      if (!is_arrow)
	is_plusplus = afs_match_substr("++");
      if (!is_plusplus)
	is_minusminus = afs_match_substr("--");
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
    bool result = false;
    const char* old_char = c_char;

    SgExpression* lhs = NULL;
    if (afs_match_assignment_expression())
    {
      assert (c_parsed_node!= NULL);
      lhs = isSgExpression(c_parsed_node);
      assert (lhs != NULL);
      result = true;
    }
    else
    { // immediate return if first term is not matched
      result = false;
      c_char = old_char;
      return false;
    }

    //TODO : 
    // this is ambiguous when dealing with 'clause (exp, exp)'
    // it will generate one single CommaOpExp instead of one exp as expected
#if 0 

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
//	printf("error: afs_match_expression(), expecting assignment_expression after ','\n");
//	assert (0);
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
  // statements
  bool afs_match_statement()
  {
    bool result = false;
    if (afs_match_labeled_statement())
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

  bool afs_match_labeled_statement()
  {
    //TODO 
    return false;
  }
  bool afs_match_compound_statement()
  {
    //TODO 
    return false;
  }
  bool afs_match_expression_statement()
  {
    //TODO 
    return false;
  }
  bool afs_match_selection_statement()
  {
    //TODO 
    return false;
  }
  bool afs_match_iteration_statement()
  {
    //TODO 
    return false;
  }
  bool afs_match_jump_statement()
  {
    //TODO 
    return false;
  }

} // end namespace AstFromString
