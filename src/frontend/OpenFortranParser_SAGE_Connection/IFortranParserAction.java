/******************************************************************************
 * Copyright (c) 2005, 2006 Los Alamos National Security, LLC.  This
 * material was produced under U.S. Government contract
 * DE-AC52-06NA25396 for Los Alamos National Laboratory (LANL), which
 * is operated by the Los Alamos National Security, LLC (LANS) for the
 * U.S. Department of Energy. The U.S. Government has rights to use,
 * reproduce, and distribute this software. NEITHER THE GOVERNMENT NOR
 * LANS MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY
 * LIABILITY FOR THE USE OF THIS SOFTWARE. If software is modified to
 * produce derivative works, such modified software should be clearly
 * marked, so as not to confuse it with the version available from
 * LANL.
 *
 * Additionally, this program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *****************************************************************************/

package fortran.ofp.parser.java;

import org.antlr.runtime.Token;

import fortran.ofp.parser.java.IActionEnums;

public abstract interface IFortranParserAction {

   /** R102 list
    * generic_name (xyz-name)
    * generic_name_list (xyz-list R101)
    *   :   T_IDENT ( T_COMMA T_IDENT )*
    * 
    * @param count The number of items in the list.
    * @param ident The name of the item placed in the list.
    */
   public abstract void generic_name_list__begin();
   public abstract void generic_name_list(int count);
   public void generic_name_list_part(Token ident);

   /** R204
    * specification_part
    *    is [ use-stmt ] ... 
    *       [ import-stmt ] ... 
    *       [ implicit-part ] 
    *       [ declaration-construct ] ... 
    *
    * @param numUseStmts  Number of use statements.
    * @param numImportStmts  Number of import statements.
    * @param numImplStmts Number of implicit statements
    * @param numDeclConstructs  Number of declaration constructs.
    */
   public abstract void
   specification_part(int numUseStmts, int numImportStmts,
                      int numImplStmts, int numDeclConstructs);

   /** R205, R206, R207
    * declaration_construct
    */
   public abstract void declaration_construct();

   /** R208
    * execution_part
    */
   public abstract void execution_part();

   /** R209
    * execution_part_construct
    */
   public abstract void execution_part_construct();

   /** R210
    * internal_subprogram_part
    *   :   contains_stmt internal_subprogram (internal_subprogram)*
    *
    * @param count The number of internal subprograms
    */
   public abstract void internal_subprogram_part(int count);

   /** R211
    * internal_subprogram
    */
   public abstract void internal_subprogram();

   /** R212
    * specification_stmt
    */
   public abstract void specification_stmt();

   /** R213
    * executable_construct
    */
   public abstract void executable_construct();

   /** R214
    * action_stmt
    */
   public abstract void action_stmt();

   /** R215
    * keyword
    */
   public abstract void keyword();

   /** R304
    * name
    *
    * @param id T_IDENT token for the name.
    */
   public abstract void name(Token id);

   /** R305
    * constant
    *   :   literal_constant
    *   |   T_IDENT
    * 
    * ERR_CHK 305 named_constant replaced by T_IDENT
    * 
    * @param id The identifier representing the named constant if
    * present, otherwise is a literal-constant
    */
   public abstract void constant(Token id);

   /**
    * scalar_constant
    * 
    */
   public abstract void scalar_constant();

   /**
    * R306
    * literal_constant
    *
    */
   public abstract void literal_constant();

   /** R308
    * int_constant
    *   :   int_literal_constant
    *   |   T_IDENT
    * 
    * ERR_CHK 308 named_constant replaced by T_IDENT
    * C302 R308 int_constant shall be of type integer
    * inlined integer portion of constant
    * 
    * @param id The identifier representing the named constant if
    * present, otherwise is a literal-constant
    */
   public abstract void int_constant(Token id);

   /** R309
    * char_constant
    *   :   char_literal_constant
    *   |   T_IDENT
    * 
    * ERR_CHK 309 named_constant replaced by T_IDENT
    * C303 R309 char_constant shall be of type character
    * inlined character portion of constant
    * 
    * @param id The identifier representing the named constant if
    * present, otherwise is a literal-constant
    */
   public abstract void char_constant(Token id);

   /**
    * R310
    * intrinsic_operator
    *
    */
   public abstract void intrinsic_operator();

   /** R311
    * defined_operator
    *   :   T_DEFINED_OP
    *   |   extended_intrinsic_op
    *
    * @param definedOp The operator (either a defined-unary-op,
    * defined-binary-op, or extended-intrinsic-op).
    * @param isExtended True if the token is an extended-intrinsic-op,
    * otherwise is a defined operator.
    */
   public abstract void defined_operator(Token definedOp, boolean isExtended);

   /**
    * R312
    * extended_intrinsic_op
    *
    */
   public abstract void extended_intrinsic_op();

   /** R313
    * label
    *   :   T_DIGIT_STRING
    *
    * @param lbl The token containing the label
    */
   public abstract void label(Token lbl);

   /** R313 list
    * label   :   T_DIGIT_STRING
    * label_list
    *   :   label ( T_COMMA label )*
    *
    * // ERR_CHK 313 five characters or less
    *
    * @param count The number of items in the list.
    */
   public abstract void label_list__begin();
   public abstract void label_list(int count);

   /**
    * R401
    * type_spec
    *
    */
   public abstract void type_spec();
	
   /** R402
    * type-param-value
    *   :   expr | T_ASTERISK | T_COLON
    *
    * @param hasExpr True if an expr is present
    * @param hasAsterisk True if an '*' is present
    * @param hasColon True if a ':' is present
    */
   public abstract void
   type_param_value(boolean hasExpr, boolean hasAsterisk, boolean hasColon);

   /** R403
    * intrinsic_type_spec
    *   :   T_INTEGER ( kind_selector )?
    *   |   T_REAL ( kind_selector )?
    *   |   T_DOUBLE T_PRECISION | T_DOUBLEPRECISION
    *   |   T_COMPLEX ( kind_selector )?
    *   |   T_DOUBLE T_COMPLEX | T_DOUBLECOMPLEX
    *   |   T_CHARACTER ( char_selector )?
    *   |   T_LOGICAL ( kind_selector )?
    *
    * @param keyword1 The type keyword token.
    * @param keyword2 The optional keyword token (i.e., T_PRECISION)
    * @param type The type specified (i.e., INTEGER)
    * @param hasKindSelector True if a kind_selector
    * (scalar_int_initialization_expr) is present
    */
   public abstract void
   intrinsic_type_spec(Token keyword1, Token keyword2, int type, boolean hasKindSelector);

   /** R404
    * kind_selector
    *   :   T_LPAREN (T_KIND T_EQUALS)? expr T_RPAREN
    *   |   T_ASTERISK T_DIGIT_STRING	
    *
    * @param token1 KIND keyword token (or *, nonstandard usage)
    * @param token2 = token (or size of type, nonstandard usage)
    * @param hasExpression True if an expr is present
    * (standard-confirming option)
    */
   public abstract void
   kind_selector(Token token1, Token token2, boolean hasExpression);

   /** R405
    * signed_int_literal_constant
    *   :   (T_PLUS|T_MINUS)? int_literal_constant
    *
    * @param sign The sign: positive, negative, or null.
    */
   public abstract void signed_int_literal_constant(Token sign);

   /** R406
    * int_literal_constant
    *   :   T_DIGIT_STRING (T_UNDERSCORE kind_param)?
    *
    * @param digitString The digit string representing the constant
    * @param kindParam The kind parameter
    */
   public abstract void
   int_literal_constant(Token digitString, Token kindParam);

   /**
    * R407
    * kind_param
    *
    * @param kind T_DIGIT_STRING or T_IDENT token which is the kind_param.
    */
   public abstract void kind_param(Token kind);

   /**
    * R411
    * boz_literal_constant
    *
    */
   public abstract void boz_literal_constant(Token constant);

   /** R416
    * signed_real_literal_constant
    *   :   (T_PLUS|T_MINUS)? real_literal_constant
    *
    * @param sign The sign: positive, negative, or null.
    */
   public abstract void signed_real_literal_constant(Token sign);

   /** R417
    * real_literal_constant
    *   :   REAL_CONSTANT ( T_UNDERSCORE kind_param )?
    *   |   DOUBLE_CONSTANT ( T_UNDERSCORE kind_param )?
    *
    * Replaced by
    *   :   T_DIGIT_STRING T_PERIOD_EXPONENT (T_UNDERSCORE kind_param)?
    *   |   T_DIGIT_STRING T_PERIOD (T_UNDERSCORE kind_param)?
    *   |   T_PERIOD_EXPONENT (T_UNDERSCORE kind_param)?
    *
    * @param digits The integral part
    * @param fractionExp The fractional part and exponent
    * @param kindParam The kind parameter
    */
   public abstract void real_literal_constant(Token realConstant, Token kindParam);

   /**
    * R421
    * complex_literal_constant
    *
    */
   public abstract void complex_literal_constant();

   /** R422
    * real_part
    * 
    * ERR_CHK 422 named_constant replaced by T_IDENT
    * 
    * @param hasIntConstant True if signed-int-literal-constant is present
    * @param hasRealConstant True if signed-real-literal-constant is present
    * @param id The named-constant (optional)
    */
   public abstract void
   real_part(boolean hasIntConstant, boolean hasRealConstant, Token id);

   /** R423
    * imag_part
    * 
    * ERR_CHK 423 named_constant replaced by T_IDENT
    * 
    * @param hasIntConstant True if signed-int-literal-constant is present
    * @param hasRealConstant True if signed-real-literal-constant is present
    * @param id The named-constant (optional)
    */
   public abstract void
   imag_part(boolean hasIntConstant, boolean hasRealConstant, Token id);

   /** R424
    * char-selector
    *   :   T_ASTERISK char_length (T_COMMA)?
    *   |   T_LPAREN (T_KIND | T_LEN) T_EQUALS type_param_value
    *          ( T_COMMA (T_KIND | T_LEN) T_EQUALS type_param_value )? 
    *       T_RPAREN
    *   |   T_LPAREN type_param_value ( T_COMMA (T_KIND T_EQUALS)? expr )? 
    *       T_RPAREN
    *
    * @param tk1 Either T_KIND or T_LEN if given; otherwise null.
    * @param tk2 Either T_KIND or T_LEN if given; otherwise null.
    * @param kindOrLen1 Specifies whether the first kind or len
    * type-param-value is present
    * @param kindOrLen2 Specifies whether the second kind or len
    * type-param-value is present
    * @param hasAsterisk True if a '*' char-selector is specified
    */
   public abstract void
   char_selector(Token tk1, Token tk2, int kindOrLen1, int kindOrLen2, boolean hasAsterisk);

   /** R425
    * length-selector
    *   :   T_LPAREN ( T_LEN T_EQUALS )? type_param_value T_RPAREN
    *   |   T_ASTERISK char_length (T_COMMA)?
    *
    * @param lenKeyword T_LEN token if given; null otherwise.
    * @param kindOrLen Specifies whether a kind or len type-param-value is present
    * @param hasAsterisk True if a '*' char-selector is specified
    */
   public abstract void
   length_selector(Token len, int kindOrLen, boolean hasAsterisk);

   /** R426
    * char_length
    *   :   T_LPAREN type_param_value T_RPAREN
    *   |   scalar_int_literal_constant
    *
    * @param hasTypeParamValue True if a type-param-value is
    * specified, otherwise is a scalar-int-literal-constant
    */
   public abstract void char_length(boolean hasTypeParamValue);

   /**
    * scalar_int_literal_constant
    *
    */
   public abstract void scalar_int_literal_constant();

   /** R427
    * char_literal_constant
    * 	:	T_DIGIT_STRING T_UNDERSCORE T_CHAR_CONSTANT
    *       // T_UNDERSCORE as one token (T_IDENT).
    *	    |   T_IDENT T_CHAR_CONSTANT
    *	    |   T_CHAR_CONSTANT
    * 
    * @param digitString Optional digit-string representing the kind parameter
    * @param id Optional identifier representing the kind parameter variable AND the '_'
    * @param str The token containing the literal character constant
    */
   public abstract void
   char_literal_constant(Token digitString, Token id, Token str);

   /** R428
    * logical_literal_constant
    *   :   T_TRUE | T_FALSE
    *
    * @param logicalValue T_TRUE or T_FALSE token.
    * @param isTrue True if logical constant is true, false otherwise
    * @param kindParam The kind parameter
    */
   public abstract void
   logical_literal_constant(Token logicalValue, boolean isTrue, Token kindParam);

   /**
    * Generated rule.
    * hollerith_literal_constant
    *
    * Note: Hollerith constants were deleted in F77; Hollerith edit descriptors
    * deleted in F95.
    *
    * @param hollerithConstant T_HOLLERITH token.
    */
   public abstract void hollerith_literal_constant(Token hollerithConstant);

   /**
    * R429
    * derived_type_def
    *
    */
   public abstract void derived_type_def();

   /** R429
    * type_param_or_comp_def_stmt
    *   :   type_param_attr_spec T_COLON_COLON type_param_decl_list T_EOS 
    *   |   component_attr_spec_list T_COLON_COLON component_decl_list T_EOS
    *
    * @param eos Token for T_EOS.
    * @param type typeParam if a typeParam. compDef if a compDef.
    */
   public abstract void type_param_or_comp_def_stmt(Token eos, int type);

   /**
    * R429
    *
    * type_param_or_comp_def_stmt_list
    */
   public abstract void type_param_or_comp_def_stmt_list();

   /** R430 
    * derived_type_stmt
    *   :   T_TYPE ( ( T_COMMA type_attr_spec_list )? T_COLON_COLON )? T_IDENT
    *       ( T_LPAREN generic_name_list T_RPAREN )? T_EOS
    *
    * @param label The label.
    * @param keyword The TYPE keyword token.
    * @param id The identifier.
    * @param eos End of statement token
    * @param hasTypeAttrSpecList True if a type-attr-spec-list is present.
    * @param hasGenericNameList True if a generic-name-list is present.
    */
   public abstract void
   derived_type_stmt(Token label, Token keyword, Token id, Token eos,
                     boolean hasTypeAttrSpecList, boolean hasGenericNameList);

   /** R431
    * type_attr_spec
    *   :   access_spec
    *   |   T_EXTENDS T_LPAREN T_IDENT T_RPAREN
    *   |   T_ABSTRACT
    *   |   T_BIND T_LPAREN T_IDENT T_RPAREN
    *
    * @param keyword The type-attr-spec keyword token (null if an access_spec)
    * @param id Identifier if extends or bind. Otherwise, null.
    * @param specType "Enum" on type: access_spec, extnds, abstrct, or
    * bind. (Weird spelling of extnds and abstrct avoids overrriding
    * java keywords.)
    */
   public abstract void type_attr_spec(Token keyword, Token id, int specType);

   /** R431 list
    * type_attr_spec
    * type_attr_spec_list
    *   :   type_attr_spec ( T_COMMA type_attr_spec )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void type_attr_spec_list__begin();
   public abstract void type_attr_spec_list(int count);

   /**
    * R432
    * private_or_sequence
    *
    */
   public abstract void private_or_sequence();

   /** R433
    * end_type_stmt
    *   :   (label)? T_END T_TYPE (T_IDENT)? T_EOS
    *   |   (label)? T_ENDTYPE (T_IDENT)? T_EOS
    * 
    * @param label The label.
    * @param endKeyword Token for T_END or T_ENDTYPE.
    * @param typeKeyword Token for T_TYPE, if given as a separate token.  null 
    * if not provided. 
    * @param id The identifier.
    * @param eos T_EOS token.
    */
   public abstract void
   end_type_stmt(Token label, Token endKeyword, Token typeKeyword, Token id, Token eos);

   /** R434
    * sequence_stmt
    *   :   (label)? T_SEQUENCE T_EOS
    * 
    * @param label The label.
    * @param sequenceKeyword T_SEQUENCE token.
    * @param eos T_EOS token.
    */
   public abstract void
   sequence_stmt(Token label, Token sequenceKeyword, Token eos);

   /** R436
    * type_param_decl
    *   :   T_IDENT ( T_EQUALS expr )?
    *
    * @param id Identifier equal to the parameter.
    * @param hasInit True if is initialized.
    */
   public abstract void type_param_decl(Token id, boolean hasInit);

   /** R436 list
    * type_param_decl
    * type_param_decl_list
    *   :   type_param_decl ( T_COMMA type_param_decl )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void type_param_decl_list__begin();
   public abstract void type_param_decl_list(int count);

   /** R437
    * type_param_attr_spec
    *
    * @param kindOrLen T_IDENT token for T_KIND or T_LEN.
    */
   public abstract void type_param_attr_spec(Token kindOrLen);

   /** R439
    * component_def_stmt
    *   :   data_component_def_stmt
    *   |   proc_component_def_stmt
    *
    * @param type Type of definition: data or procedure.
    */
   public abstract void component_def_stmt(int type);

   /** R440
    * data_component_def_stmt
    *   :   (label)? declaration_type_spec 
    *       ( ( T_COMMA component_attr_spec_list {hasSpecList=true;})?
    *       T_COLON_COLON )? component_decl_list T_EOS
    * 
    * @param label The label.
    * @param eos T_EOS token.
    * @param hasSpecList Boolean true if has a component_attr_spec(_list).
    */
   public abstract void
   data_component_def_stmt(Token label, Token eos, boolean hasSpec);

   /** R441
    * component_attr_spec
    *   :   T_POINTER
    *   |   T_DIMENSION T_LPAREN component_array_spec T_RPAREN
    *   |   T_DIMENSION T_LBRACKET coarray_spec T_RBRACKET
    *   |   T_ALLOCATABLE
    *   |   access_spec
    *   |   T_KIND
    *   |   T_LEN
    *
    * @param attrKeyword T_POINTER, T_DIMENSION, T_ALLOCATABLE, T_KIND, or T_LEN if given; null otherwise.
    * @param specType Type of spec in enum form: pointer, dimension_paren, dimension_bracket, allocable, access_spec, kind, or len.
    */
   public abstract void component_attr_spec(Token attrKeyword, int specType);

   /** R441 list
    * component_attr_spec
    * component_attr_spec_list
    *   :   component_attr_spec ( T_COMMA component_attr_spec )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void component_attr_spec_list__begin();
   public abstract void component_attr_spec_list(int count);

   /** R442
    * component_decl
    *   :   T_IDENT ( T_LPAREN component_array_spec T_RPAREN )?
    *       ( T_LBRACKET coarray_spec T_RBRACKET )?
    *       ( T_ASTERISK char_length )? ( component_initialization )?
    *
    * @param id Component identifier.
    * @param hasComponentArraySpec True if has component array spec.
    * @param hasCoarraySpec True if has coarray spec.
    * @param hasCharLength True if has char length.
    * @param hasComponentInitialization True if has component initialization.
    */
   public abstract void
   component_decl(Token id, boolean hasComponentArraySpec, boolean hasCoarraySpec,
                  boolean hasCharLength, boolean hasComponentInitialization);

   /** R442 list
    * component_decl
    * component_decl_list
    *   :   component_decl ( T_COMMA component_decl )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void component_decl_list__begin();
   public abstract void component_decl_list(int count);

   /** R443
    * component_array_spec
    *   :   explicit_shape_spec_list
    *   |   deferred_shape_spec_list
    *
    * @param isExplicit True if this is an explicit shape spec
    * list. false if it is a deferred shape spec list.
    */
   public abstract void component_array_spec(boolean isExplicit);

   /** R443 list
    * deferred_shape_spec_list
    *   : T_COLON {count++;} ( T_COMMA T_COLON {count++;} )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void deferred_shape_spec_list__begin();
   public abstract void deferred_shape_spec_list(int count);

   /**
    * R444
    * component_initialization
    *
    */
   public abstract void component_initialization();

   /** R445
    * proc_component_def_stmt
    *   :   (label)? T_PROCEDURE T_LPAREN (proc_interface)? T_RPAREN T_COMMA
    *       proc_component_attr_spec_list T_COLON_COLON proc_decl_list T_EOS
    * 
    * @param label The label.
    * @param procedureKeyword T_PROCEDURE token.
    * @param eos T_EOS token.
    * @param hasInterface Boolean true if has a nonempty interface.
    */
   public abstract void
   proc_component_def_stmt(Token label, Token procedureKeyword,
                           Token eos, boolean hasInterface);

   /** R446
    * proc_component_attr_spec
    *   :   T_POINTER
    *   |   T_PASS ( T_LPAREN T_IDENT T_RPAREN {id=$T_IDENT;} )?
    *   |   T_NOPASS
    *   |   access_spec
    *
    * @param attrSpecKeyword T_POINTER, T_PASS, or T_NOPASS token if given; null otherwise.
    * @param id Identifier if present in pass. 
    * @param specType "Enum" to specify type of spec: pointer, pass, nopass, or access_spec
    */
   public abstract void
   proc_component_attr_spec(Token attrSpecKeyword, Token id, int specType);

   /** R446 list
    * proc_component_attr_spec_list
    * 		proc_component_attr_spec ( T_COMMA proc_component_attr_spec )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void proc_component_attr_spec_list__begin();
   public abstract void proc_component_attr_spec_list(int count);

   /** R447
    * private_components_stmt
    *   :   (label)? T_PRIVATE T_EOS
    * 
    * @param label The label.
    * @param privateKeyword T_PRIVATE token.
    * @param eos T_EOS token.
    */
   public abstract void
   private_components_stmt(Token label, Token privateKeyword, Token eos);

   /** R448
    * type_bound_procedure_part
    *   :   T_CONTAINS  T_EOS ( binding_private_stmt )? proc_binding_stmt 
    *       ( proc_binding_stmt )*
    *
    * @param containsKeyword T_CONTAINS token.
    * @param eos T_EOS token.
    * @param count  Number of procedure binding statements.
    * @param hasBindingPrivateStmt True if has a keyword "private".
    */
   public abstract void
   type_bound_procedure_part(int count, boolean hasBindingPrivateStmt);

   /** R449
    * binding_private_stmt
    *   :   (label)? T_PRIVATE T_EOS
    * 
    * @param label The label.
    * @param privateKeyword T_PRIVATE token.
    * @param eos T_EOS token.
    */
   public abstract void
   binding_private_stmt(Token label, Token privateKeyword, Token eos);

   /** R450
    * proc_binding_stmt
    *   :    (label)? specific_binding T_EOS
    *   |    (label)? generic_binding T_EOS
    *   |    (label)? final_binding T_EOS
    * 
    * @param label The label.
    * @param type Binding statement type (either final or generic).
    * @param eos T_EOS token.
    */
   public abstract void proc_binding_stmt(Token label, int type, Token eos);

   /** R451
    * specific_binding
    *   :   T_PROCEDURE ( T_LPAREN T_IDENT T_RPAREN )?
    *       ( ( T_COMMA binding_attr_list )? T_COLON_COLON )?
    *       T_IDENT ( T_EQ_GT T_IDENT )?
    *
    * @param procedureKeyword T_PROCEDURE token.
    * @param interfaceName Optional interface name.
    * @param bindingName Required binding name.
    * @param procedureName Optional procedure name.
    * @param hasBindingAttributeList True if has a binding-attr-list.
    */
   public abstract void
   specific_binding(Token procedureKeyword, Token interfaceName, Token bindingName,
                    Token procedureName, boolean hasBindingAttrList);

   /** R452
    * generic_binding
    *   :   T_GENERIC ( T_COMMA access_spec )? T_COLON_COLON 
    *       generic_spec T_EQ_GT generic_name_list
    *
    * @param genericKeyword T_GENERIC token.
    * @param hasAccessSpec True if has public or private access spec.
    */
   public abstract void generic_binding(Token genericKeyword, boolean hasAccessSpec);

   /** R453
    * binding_attr
    *   :   T_PASS ( T_LPAREN T_IDENT T_RPAREN )?
    *   |   T_NOPASS
    *   |   T_NON_OVERRIDABLE
    *   |   T_DEFERRED
    *   |   access_spec
    *
    * @param bindingAttr T_PASS, T_NOPASS, T_NON_OVERRIDABLE, T_DEFERRED, or null.
    * @param attr The binding attribute.
    * @param id Optional identifier in pass attribute.
    */
   public abstract void binding_attr(Token bindingAttr, int attr, Token id);

   /** R453 list
    * binding_attr_list
    *   :   binding_attr ( T_COMMA binding_attr )*
    *
    * @param count The number of items in the list.
    */
   public abstract void binding_attr_list__begin();
   public abstract void binding_attr_list(int count);

   /**
    * R454
    * final_binding
    *
    * @param finalKeyword T_FINAL token.
    */
   public abstract void final_binding(Token finalKeyword);

   /** R455
    * derived_type_spec
    *   :   T_IDENT ( T_LPAREN type_param_spec_list T_RPAREN )?
    *
    * @param typeName The name of the derived type or class.
    * @param hasTypeParamSpecList True if type-param-spec-list is present.
    */
   public abstract void
   derived_type_spec(Token typeName, boolean hasTypeParamSpecList);

   /** R456
    * type_param_spec
    *   :   (keyword T_EQUALS)? type_param_value
    *
    * @param keyword Keyword if of the form kw = foo. Null otherwise.
    */
   public abstract void type_param_spec(Token keyword);

   /** R456 list
    * type_param_spec_list
    *    :   type_param_spec ( T_COMMA type_param_spec )*
    *
    * @param count The number of items in the list.
    */
   public abstract void type_param_spec_list__begin();
   public abstract void type_param_spec_list(int count);

   /**
    * R457
    * structure_constructor
    *
    * @param id T_IDENT token for strucure.
    */
   public abstract void structure_constructor(Token id);

   /**
    * R458 
    * component_spec
    *
    * @param id T_IDENT token for "keyword=" part of component-spec.
    */
   public abstract void component_spec(Token id);

   /** R458 list
    * component_spec_list
    *   :   component_spec ( T_COMMA component_spec )*
    *
    * @param count The number of items in the list.
    */
   public abstract void component_spec_list__begin();
   public abstract void component_spec_list(int count);

   /**
    * R459
    * component_data_source
    *
    */
   public abstract void component_data_source();

   /** R460
    * enum_def
    *   :   enum_def_stmt enumerator_def_stmt (enumerator_def_stmt)*
    *       end_enum_stmt
    *
    * @param numEls Number of elements in the enum.
    */
   public abstract void enum_def(int numEls);

   /** R461
    * enum_def_stmt
    *   :   (label)? T_ENUM T_COMMA T_BIND T_LPAREN T_IDENT T_RPAREN T_EOS
    *
    * @param label The label.
    * @param enumKeyword T_ENUM token.
    * @param bindKeyword T_BIND token.
    * @param id The identifier.
    * @param eos T_EOS token.
    */
   public abstract void
   enum_def_stmt(Token label, Token enumKeyword, Token bindKeyword, Token id, Token eos);

   /** R462
    * enumerator_def_stmt
    *   :   (label)? T_ENUMERATOR ( T_COLON_COLON )? enumerator_list T_EOS
    * @param label The label.
    * @param enumeratorKeyword T_ENUMERATOR token.
    * @param eos T_EOS token.
    */
   public abstract void
   enumerator_def_stmt(Token label, Token enumeratorKeyword, Token eos);

   /** R463
    * enumerator
    *
    * @param id T_IDENT for enumerator.
    * @param hasExpr Boolean specifying if the optional "= expr" was given.
    */
   public abstract void enumerator(Token id, boolean hasExpr);

   /** R463 list
    * enumerator_list
    *   :   enumerator ( T_COMMA enumerator )*
    *
    * @param count The number of items in the list.
    */
   public abstract void enumerator_list__begin();
   public abstract void enumerator_list(int count);

   /** R464
    * end_enum_stmt
    *   :   (label)? T_END T_ENUM T_EOS
    *   |   (label)? T_ENDENUM T_EOS
    *
    * @param label The label.
    * @param endKeyword T_END or T_ENDENUM token.
    * @param enumKeyword T_ENUM token or null if not given.
    * @param eos T_EOS token.
    */
   public abstract void
   end_enum_stmt(Token label, Token endKeyword, Token enumKeyword, Token eos);

   /** R465
    * array_constructor
    *   :   T_LPAREN T_SLASH ac_spec T_SLASH T_RPAREN
    *   |   T_LBRACKET ac_spec T_RBRACKET
    */
   public abstract void array_constructor();
	
   /** R466
    * ac_spec
    */
   public abstract void ac_spec();

   /** R469
    * ac_value
    */
   public abstract void ac_value();

   /** R469 list
    * ac_value_list
    *   :   ac_value ( T_COMMA ac_value )*
    *
    * @param count The number of items in the list.
    */
   public abstract void ac_value_list__begin();
   public abstract void ac_value_list(int count);

   /** R470
    * ac_implied_do
    *   :   T_LPAREN ac_value_list T_COMMA ac_implied_do_control T_RPAREN
    *
    */
   public abstract void ac_implied_do();

   /** R471
    * ac_implied_do_control
    *   :   T_IDENT T_EQUALS expr T_COMMA expr (T_COMMA expr)?
    *
    * @param hasStride True if is of the form a = b,c,d where d is optional stride
    */
   public abstract void ac_implied_do_control(boolean hasStride);

   /** R472
    * scalar_int_variable
    */
   public abstract void scalar_int_variable();

   /** R501
    * type_declaration_stmt
    *   :   (label)? declaration_type_spec ( (T_COMMA attr_spec)* 
    *       T_COLON_COLON )?
    *       entity_decl_list T_EOS
    *
    * @param label Optional statement label
    * @param numAttributes The number of attributes present
    * @param eos Token for the end of the statement.
    */
   public abstract void
   type_declaration_stmt(Token label, int numAttributes, Token eos);

   /** R502
    * declaration_type_spec
    *   :   intrinsic_type_spec
    *   |   T_TYPE T_LPAREN derived_type_spec T_RPAREN
    *   |   T_CLASS T_LPAREN derived_type_spec T_RPAREN
    *   |   T_CLASS T_LPAREN T_ASTERISK T_RPAREN
    *
    * @param udtKeyword Token for the T_TYPE or T_CLASS and null for intrinsic_type_spec.
    * @param type The type of declaration-type-spec {INTRINSIC,TYPE, CLASS,POLYMORPHIC}.
    */
   public abstract void declaration_type_spec(Token udtKeyword, int type);

   /** R503
    * attr_spec
    *   :   access_spec
    *   |   T_ALLOCATABLE
    *   |   T_ASYNCHRONOUS
    *   |   T_DIMENSION T_LPAREN array_spec T_RPAREN
    *   |   T_EXTERNAL
    *   |   T_INTENT T_LPAREN intent_spec T_RPAREN
    *   |   T_INTRINSIC
    *   |   language_binding_spec      
    *   |   T_OPTIONAL
    *   |   T_PARAMETER
    *   |   T_POINTER
    *   |   T_PROTECTED
    *   |   T_SAVE
    *   |   T_TARGET
    *   |   T_VALUE
    *   |   T_VOLATILE
    *
    * @param attrKeyword Token for the keyword of the given attribute.  Will 
    * be null in the cases of access_sepc and language_binding_spec.
    * @param attr The attribute specification
    */
   public abstract void attr_spec(Token attrKeyword, int attr);

   /** R503-F08, R504-F03
    * entity_decl
    *   : T_IDENT ( T_LPAREN array_spec T_RPAREN )?
    *             ( T_LBRACKET coarray_spec T_RBRACKET )?
    *             ( T_ASTERISK char_length )?
    *             ( initialization )?
    *
    * @param id The identifier token
    * @param hasArraySpec True if array-spec is present, false otherwise
    * @param hasCoarraySpec True if coarray-spec is present, false otherwise
    * @param hasCharLength True if char-length is present, false otherwise
    * @param hasInitialization True if initialization is present, false otherwise
    */
   public abstract void
   entity_decl(Token id, boolean hasArraySpec, boolean hasCoarraySpec,
               boolean hasCharLength, boolean hasInitialization);

   /** R503-F08 list
    * entity_decl
    * entity_decl_list
    *   :   entity_decl ( T_COMMA entity_decl )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void entity_decl_list__begin();
   public abstract void entity_decl_list(int count);

   /** R506
    * initialization
    *   :   T_EQUALS expr
    *   |   T_EQ_GT null_init
    *
    * ERR_CHK 506 initialization_expr replaced by expr
    *
    * @param hasExpr True if expr is present
    * @param hasNullInit True if null-init is present
    */
   public abstract void initialization(boolean hasExpr, boolean hasNullInit);

   /** R507
    * null_init
    *   :   T_IDENT //'NULL'// T_LPAREN T_RPAREN
    *
    * C506 The function-reference shall be a reference to the NULL
    * intrinsic function with no arguments.
    *
    * @param id The function-reference
    */
   public abstract void null_init(Token id);

   /** R508
    * access_spec
    *   :   T_PUBLIC
    *   |   T_PRIVATE
    *
    * @param keyword The access-spec keyword token.
    * @param type The type of the access-spec.
    */
   public abstract void access_spec(Token keyword, int type);

   /** R509
    * language_binding_spec
    *   :   T_BIND T_LPAREN T_IDENT (T_COMMA name T_EQUALS expr)? T_RPAREN
    *
    * @param keyword The BIND keyword token.
    * @param id The identifier representing the language binding, must be 'C' or 'c'.
    * @param hasName True if the language-binding-spec has a name expression.	 
    */
   public abstract void
   language_binding_spec(Token keyword, Token id, boolean hasName);

   /** R509-F08
    * coarray_spec
    *   :   deferred_coshape_spec_list
    *   |   explicit_coshape_spec
    *
    * @param count The number of items in the list of coarray specifications.
    */
   public abstract void coarray_spec(int count);

   /** R510
    * array_spec
    *   :   array_spec_element (T_COMMA array_spec_element)* array_spec_element
    *   :   expr ( T_COLON (expr | T_ASTERISK)? )?
    *   |   T_ASTERISK
    *   |   T_COLON
    *
    * @param count The number of items in the list of array specifications.
    * @param type The type of the array-spec element.
    */
   public abstract void array_spec(int count);
   public abstract void array_spec_element(int type);

   /** R511
    * explicit_shape_spec
    * expr ( T_COLON expr )?
    * 
    * @param hasUpperBound Whether the shape spec is of the form x:y.
    */
   public abstract void explicit_shape_spec(boolean hasUpperBound);

   /** R511 list
    * explicit_shape_spec_list
    *   :   explicit_shape_spec ( T_COMMA explicit_shape_spec )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void explicit_shape_spec_list__begin();
   public abstract void explicit_shape_spec_list(int count);

   /** R517
    * intent_spec
    *   :   T_IN | T_OUT | T_IN T_OUT | T_INOUT
    *
    * @param intentKeyword1 The first of two possible intent keyword tokens
    * (e.g., T_IN, T_OUT, T_INOUT).
    * @param intentKeyword2 The second of two possible intent keyword tokens.  
    * This token can ONLY be T_OUT in the case of "intent(in out)", and must 
    * be null for all other intents.
    * @param intent The type of intent-spec.
    */
   public abstract void
   intent_spec(Token intentKeyword1, Token intentKeyword2, int intent);

   /** R518
    * access_stmt
    *   :   (label)? access_spec ((T_COLON_COLON)? access_id_list)? T_EOS
    *
    * @param label The label.
    * @param eos End of statement token.
    * @param hasList True if access-id-list is present.
    */
   public abstract void access_stmt(Token label, Token eos, boolean hasList);

   /** R519
    * access_id
    */
   public abstract void access_id();

   /** R519 list
    * access_id_list
    *   :   access_id ( T_COMMA access_id )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void access_id_list__begin();
   public abstract void access_id_list(int count);

   /** R520
    * allocatable_stmt
    *   :   (label)? T_ALLOCATABLE ( T_COLON_COLON )? allocatable_decl 
    *       ( T_COMMA allocatable_decl )* T_EOS
    *
    * @param label The label.
    * @param keyword The allocatable keyword token.
    * @param eos End of statement token.
    */
   public abstract void
      allocatable_stmt(Token label, Token keyword, Token eos);

   /** R527-F08
    * allocatable_decl
    *   :   T_IDENT ( T_LPAREN array_spec T_RPAREN )?
    *               ( T_LBRACKET coarray_spec T_RBRACKET )?
    * 
    * @param id The name of the object
    * @param hasArraySpec True if an array_spec is present.
    * @param hasCoarraySpec True if a coarray_spec is present.
    */
   public abstract void
   allocatable_decl(Token id, boolean hasArraySpec, boolean hasCoarraySpec);
	
   /** R527-F08 list
    * allocatable_decl_list
    *   :   allocatable_decl ( T_COMMA allocatable_decl )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void allocatable_decl_list__begin();
   public abstract void allocatable_decl_list(int count);

   /** R521
    * asynchronous_stmt
    *   :   (label)? T_ASYNCHRONOUS ( T_COLON_COLON )?  generic_name_list T_EOS
    *
    * @param label The label.
    * @param keyword The ASYNCHRONOUS keyword token.
    * @param eos End of statement token.
    */
   public abstract void asynchronous_stmt(Token label, Token keyword, Token eos);

   /** R522
    * bind_stmt
    *   :   (label)? language_binding_spec (T_COLON_COLON)? bind_entity_list T_EOS
    *
    * @param label Optional statement label
    * @param eos End of statement token
    */
   public abstract void bind_stmt(Token label, Token eos);

   /** R523
    * bind_entity
    *   :   T_IDENT | T_SLASH T_IDENT T_SLASH
    *
    * @param entity The thing to bind.
    * @param isCommonBlockName True if the entity is the name of a common block
    */
   public abstract void bind_entity(Token entity, boolean isCommonBlockName);

   /** R523 list
    * bind_entity_list
    *   :   bind_entity ( T_COMMA bind_entity )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void bind_entity_list__begin();
   public abstract void bind_entity_list(int count);

   /** R531-F08
    * codimension-stmt
    *    :   CODIMENSION [ :: ] codimension-decl-list
    *
    * @param label Optional statement label
    * @param keyword The CODIMENSION keyword token
    * @param eos End of statement token
    */
   public abstract void codimension_stmt(Token label, Token keyword, Token eos);

   /** R532-F08
    * codimension-decl
    *    :   coarray-name lbracket coarray-spec rbracket
    *
    * @param coarrayName The coarray-name token
    * @param lbracket The T_LBRACKET token
    * @param rbracket The T_RBRACKET token
    */
   public abstract void codimension_decl(Token coarrayName, Token lbracket, Token rbracket);

   /** R532-F08 list
    * codimension_decl_list
    *   :   codimension_decl ( T_COMMA codimension_decl )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void codimension_decl_list__begin();
   public abstract void codimension_decl_list(int count);

   /** R524
    * data_stmt
    *   :   (label)? T_DATA data_stmt_set ((T_COMMA)? data_stmt_set)* T_EOS
    * 
    * @param label The label.
    * @param keyword The DATA keyword token.
    * @param eos End of statement token.
    * @param count The number of data statement sets.
    */
   public abstract void
   data_stmt(Token label, Token keyword, Token eos, int count);

   /** R525
    * data_stmt_set
    *
    */
   public abstract void data_stmt_set();

   /**
    * R526
    * data_stmt_object
    *
    */
   public abstract void data_stmt_object();

   /** R526 list
    * data_stmt_object_list
    *   :   data_stmt_object ( T_COMMA data_stmt_object )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void data_stmt_object_list__begin();
   public abstract void data_stmt_object_list(int count);

   /** R527
    * data_implied_do
    *
    * @param id T_IDENT token.
    * @param hasThirdExpr Flag to specify if optional third expression was 
    * given.  True if expression given; false if not.
    */
   public abstract void data_implied_do(Token id, boolean hasThirdExpr);

   /** R528
    * data_i_do_object
    *
    */
   public abstract void data_i_do_object();

   /** R528 list
    * data_i_do_object_list
    *   :   data_i_do_object ( T_COMMA data_i_do_object )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void data_i_do_object_list__begin();
   public abstract void data_i_do_object_list(int count);

   /**
    * R530
    * data_stmt_value
    *
    * @param asterisk The T_ASTERISK token (null if no data-stmt-repeat)
    */
   public abstract void data_stmt_value(Token asterisk);

   /** R530 list
    * data_stmt_value_list
    *   :   data_stmt_value ( T_COMMA data_stmt_value )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void data_stmt_value_list__begin();
   public abstract void data_stmt_value_list(int count);

   /** R531
    * scalar_int_constant
    */
   public abstract void scalar_int_constant();

   /** R532
    * data_stmt_constant
    */
   public abstract void data_stmt_constant();

   /** R535
    * dimension_stmt
    * (label)? T_DIMENSION ( T_COLON_COLON )? dimension_decl 
    *                      ( T_COMMA dimension_decl {count++;})* T_EOS
    * 
    * @param label The label.
    * @param keyword The DIMENSION keyword token.
    * @param eos End of statement token.
    * @param count The number of dimension declarations.
    */
   public abstract void
   dimension_stmt(Token label, Token keyword, Token eos, int count);

   /** R535-subrule
    * dimension_decl
    *   :   T_IDENT T_LPAREN array_spec T_RPAREN
    *
    * @param id Identifier (e.g., array name).
    */
   public abstract void dimension_decl(Token id);

   /** R536
    * intent_stmt
    *   :   (label)? T_INTENT T_LPAREN intent_spec T_RPAREN ( T_COLON_COLON )? 
    *       generic_name_list T_EOS
    *
    * @param label The label.
    * @param keyword The INTENT keyword token.
    * @param eos End of statement token.
    */
   public abstract void intent_stmt(Token label, Token keyword, Token eos);

   /** R537
    * optional_stmt
    *   :   (label)? T_OPTIONAL ( T_COLON_COLON )? generic_name_list T_EOS
    *
    * @param label The label.
    * @param keyword The OPTIONAL keyword token.
    * @param eos End of statement token.
    */
   public abstract void optional_stmt(Token label, Token keyword, Token eos);

   /** R538
    * parameter_stmt
    *   :   (label)? T_PARAMETER T_LPAREN named_constant_def_list T_RPAREN T_EOS
    *
    * @param label The label.
    * @param keyword The PARAMETER keyword token.
    * @param eos End of statement token.
    */
   public abstract void parameter_stmt(Token label, Token keyword, Token eos);

   /** R539 list
    * named_constant_def_list
    *   :   named_constant_def ( T_COMMA named_constant_def )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void named_constant_def_list__begin();
   public abstract void named_constant_def_list(int count);

   /** R539
    * named_constant_def
    *   :   T_IDENT T_EQUALS expr
    *
    * @param id Identifier if present.
    */
   public abstract void named_constant_def(Token id);

   /** R540
    * pointer_stmt
    *   :   (label)? T_POINTER ( T_COLON_COLON )? pointer_decl_list T_EOS
    *
    * @param label The label.
    * @param keyword The POINTER keyword token.
    * @param eos End of statement token.
    */
   public abstract void pointer_stmt(Token label, Token keyword, Token eos);

   /** R541 list
    * pointer_decl_list
    *   :   pointer_decl ( T_COMMA pointer_decl )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void pointer_decl_list__begin();
   public abstract void pointer_decl_list(int count);

   /** R541
    * pointer_decl
    *	:  T_IDENT ( T_LPAREN deferred_shape_spec_list T_RPAREN )?
    *
    * @param id Identifier.
    * @param hasSpecList True if has a deferred shape spec list.
    */
   public abstract void pointer_decl(Token id, boolean hasSpecList);

   /** R540-extention
    * cray_pointer_stmt
    *   :   (label)? T_POINTER (T_LPAREN T_IDENT T_COMMA T_IDENT T_RPAREN)+ T_EOS
    *
    * @param label The label.
    * @param keyword The POINTER keyword token.
    * @param eos End of statement token.
    */
   public abstract void cray_pointer_stmt(Token label, Token keyword, Token eos);

   /** R541-extension list
    * cray_pointer_assoc_list
    *   :   cray_pointer_decl ( T_COMMA cray_pointer_decl )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void cray_pointer_assoc_list__begin();
   public abstract void cray_pointer_assoc_list(int count);

   /** R541-extension
    * cray_pointer_assoc
    *	:  T_LPAREN T_IDENT T_COMMA T_IDENT T_RPAREN
    *
    * @param pointer Pointer token.
    * @param pointer Pointee token.
    */
   public abstract void cray_pointer_assoc(Token pointer, Token pointee);

   /** R542
    * protected_stmt
    *   :   (label)? T_PROTECTED ( T_COLON_COLON )? generic_name_list T_EOS
    *
    * @param label The label.
    * @param keyword The PROTECTED keyword token.
    * @param eos End of statement token.
    */
   public abstract void protected_stmt(Token label, Token keyword, Token eos);

   /** R543
    * save_stmt
    *   :   (label T_SAVE ( ( T_COLON_COLON )? saved_entity_list )? T_EOS
    *
    * @param label The label.
    * @param keyword The SAVE keyword token.
    * @param eos End of statement token.
    * @param hasSavedEntityList True if has saved-entity-list.
    */
   public abstract void
   save_stmt(Token label, Token keyword, Token eos, boolean hasSavedEntityList);

   /** R544 list
    * saved_entity_list
    *   :   saved_entity ( T_COMMA saved_entity )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void saved_entity_list__begin();
   public abstract void saved_entity_list(int count);

   /** R544
    * saved_entity
    *   :   T_IDENT
    *   |   T_SLASH T_IDENT T_SLASH
    *
    * @param id Identifier.
    * @param isCommonBlockHame True if identifier is a common block name.
    */
   public abstract void saved_entity(Token id, boolean isCommonBlockName);

   /** R556-F08
    * target_stmt
    *   :   (label)? T_TARGET ( T_COLON_COLON )? target_decl_list
    * 
    * @param label The label.
    * @param keyword The TARGET keyword token.
    * @param eos End of statement token.
    */
   public abstract void target_stmt(Token label, Token keyword, Token eos);

   /** R557-F08
    * target_decl
    *   :   T_IDENT (T_LPAREN array_spec T_RPAREN)?
    *               (T_LBRACKET coarray_spec T_RBRACKET)?
    *
    * @param objName The name of the object
    * @param hasArraySpec True if has an array spec.
    * @param hasCoarraySpec True if has a co array spec.
    */
   public abstract void
   target_decl(Token objName, boolean hasArraySpec, boolean hasCoarraySpec);

   /** R557-F08 list
    * target_decl_list
    *   :   target_decl ( T_COMMA target_decl )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void target_decl_list__begin();
   public abstract void target_decl_list(int count);

   /** R547
    * value_stmt
    *   :   (label)? T_VALUE ( T_COLON_COLON )? generic_name_list T_EOS
    *
    * @param label The label.
    * @param keyword The VALUE keyword token.
    * @param eos End of statement token.
    */
   public abstract void value_stmt(Token label, Token keyword, Token eos);

   /** R548
    * volatile_stmt
    *   :   (label)? T_VOLATILE ( T_COLON_COLON )? generic_name_list T_EOS
    *
    * @param label The label.
    * @param keyword The VOLATILE keyword token.
    * @param eos End of statement token.
    */
   public abstract void volatile_stmt(Token label, Token keyword, Token eos);

   /** R549
    * implicit_stmt
    *   :    (label)? T_IMPLICIT implicit_spec_list T_EOS
    *   |    (label)? T_IMPLICIT T_NONE T_EOS
    *
    * @param label The label.
    * @param implicitKeyword Token for the T_IMPLICIT.
    * @param noneKeyword Token for T_NONE, if applicable; null otherwise.
    * @param eos T_EOS.
    * @param hasImplicitSpecList Could be none, or it could have a list.
    */
   public abstract void
   implicit_stmt(Token label, Token implicitKeyword, Token noneKeyword,
                 Token eos, boolean hasImplicitSpecList);

   /** R550
    * implict-spec
    * : declaration-type-spec ( letter-spec-list )
    */
   public abstract void implicit_spec();

   /** R550 list
    * implicit_spec_list
    *   :   implicit_spec ( T_COMMA implicit_spec )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void implicit_spec_list__begin();
   public abstract void implicit_spec_list(int count);

	
   /** R551
    * letter-spec
    *   :   letter [ - letter ]
    *
    * @param id1 Token for the required T_IDENT
    * @param id2 Token for the optional T_IDENT; null if wasn't provided.
    */
   public abstract void letter_spec(Token id1, Token id2);

   /** R551 list
    * letter_spec_list
    *   :   letter_spec ( T_COMMA letter_spec )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void letter_spec_list__begin();
   public abstract void letter_spec_list(int count);

   /** R552
    * namelist_stmt
    *   :   (label)? T_NAMELIST T_SLASH T_IDENT T_SLASH namelist_group_object_list
    *       ( ( T_COMMA )?
    *         T_SLASH T_IDENT T_SLASH namelist_group_object_list)* T_EOS
    *
    * @param label The label.
    * @param keyword The NAMELIST keyword token.
    * @param eos End of statement token.
    * @param count Number of namelist group object lists. 
    */
   public abstract void
   namelist_stmt(Token label, Token keyword, Token eos, int count);

   /** R552
    * namelist_group_name
    *   :   T_SLASH T_IDENT T_SLASH 
    *
    * @param id Identifier (i.e., actual group name).
    */
   public abstract void namelist_group_name(Token id);

   /** R553
    * namelist_group_object
    *   :   T_IDENT
    * 
    * @param id Identifier (variable-name).
    */
   public abstract void namelist_group_object(Token id);

   /** R553 list
    * namelist_group_object_list
    *   :   T_IDENT ( T_COMMA T_IDENT )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void namelist_group_object_list__begin();
   public abstract void namelist_group_object_list(int count);

   /** R554
    * equivalence_stmt
    *   :   (label)? T_EQUIVALENCE equivalence_set_list T_EOS
    *
    * @param label The label.
    * @param equivalenceKeyword T_EQUIVALENCE token.
    * @param eos T_EOS token.
    */
   public abstract void
   equivalence_stmt(Token label, Token equivalenceKeyword, Token eos);

   /** R555
    * equivalence_set
    */
   public abstract void equivalence_set();

   /** R555 list
    * equivalence_set_list
    *   :   equivalence_set ( T_COMMA equivalence_set )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void equivalence_set_list__begin();
   public abstract void equivalence_set_list(int count);

   /** R556
    *
    * equivalence_object
    *
    */
   public abstract void equivalence_object();

   /** R556 list
    * equivalence_object_list
    *   :   equivalence_object ( T_COMMA equivalence_object )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void equivalence_object_list__begin();
   public abstract void equivalence_object_list(int count);

   /** R557
    * common_stmt
    *   :   (label)? T_COMMON ( common_block_name )?  common_block_object_list 
    *       ( ( T_COMMA )? common_block_name common_block_object_list )* T_EOS
    *
    * @param label The label.
    * @param commonKeyword T_COMMON token.
    * @param eos T_EOS token.
    * @param numBlocks Number of common blocks.
    */
   public abstract void
   common_stmt(Token label, Token commonKeyword, Token eos, int numBlocks);

   /** R557
    * common_block_name
    *   :   T_SLASH ( T_IDENT )? T_SLASH
    *
    * @param id Identifier if present. Otherwise, null.
    */
   public abstract void common_block_name(Token id);

   /** R558 list
    * common_block_object_list
    *   :   common_block_object ( T_COMMA common_block_object )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void common_block_object_list__begin();
   public abstract void common_block_object_list(int count);

   /** R558
    * common_block_object
    *   :   T_IDENT ( T_LPAREN explicit_shape_spec_list T_RPAREN )?
    *
    * @param id Identifier.
    * @param hasShapeSpecList True if has an explicit shape spec list.
    */
   public abstract void common_block_object(Token id, boolean hasShapeSpecList);

   /** R601
    * variable
    *   :   designator
    */
   public abstract void variable();

   // R602 variable_name was name inlined as T_IDENT

   /** R603
    * designator
    *   :   object-name             // T_IDENT (data-ref isa T_IDENT)
    *   |   array-element           // R616 is data-ref
    *   |   array-section           // R617 is data-ref [ (substring-range) ] 
    *   |   structure-component     // R614 is data-ref
    *   |   substring
    *
    *@param hasSubstringRange True if substring-range is present.
    */
   public abstract void designator(boolean hasSubstringRange);

   /**
    * Internal rule.
    * designator_or_func_ref
    */
   public abstract void designator_or_func_ref();

   /**
    * Internal rule.
    * substring_range_or_arg_list
    */
   public abstract void substring_range_or_arg_list();

   /**
    * Internal rule.
    * substr_range_or_arg_list_suffix
    */
   public abstract void substr_range_or_arg_list_suffix();

   /** R604
    * logical_variable
    */
   public abstract void logical_variable();

   /** R605
    * default_logical_variable
    */
   public abstract void default_logical_variable();

   /**
    * Internal rule.
    * scalar_default_logical_variable
    */
   public abstract void scalar_default_logical_variable();
	
   /** R606
    * char_variable
    */
   public abstract void char_variable();

   /** R607
    * default_char_variable
    */
   public abstract void default_char_variable();

   /**
    * Internal rule.
    * scalar_default_char_variable
    *
    */
   public abstract void scalar_default_char_variable();

   /** R608
    * int_variable
    */
   public abstract void int_variable();

   /** R609
    * substring
    *   :   data_ref (T_LPAREN substring_range T_RPAREN)?
    *   |   char_literal_constant T_LPAREN substring_range T_RPAREN
    *
    * C608 (R610) parent_string shall be of type character fix for
    * ambiguity in data_ref allows it to match T_LPAREN
    * substring_range T_RPAREN, so required T_LPAREN substring_range
    * T_RPAREN made optional ERR_CHK 609 ensure final () is
    * (substring-range)
    * 
    * @param hasSubstringRange True if substring-range is present,
    * otherwise it must be extracted from the data-ref.
    */
   public abstract void substring(boolean hasSubstringRange);
	 
   /** R611
    * substring_range
    *   :   (expr)? T_COLON   (expr)?
    *
    * ERR_CHK 611 scalar_int_expr replaced by expr
    *
    * @param hasLowerBound True if lower bound is present in a
    *  substring-range (lower_bound:upper_bound).
    * @param hasUpperBound True if upper bound is present in a
    * substring-range (lower_bound:upper_bound).
    */
   public abstract void
   substring_range(boolean hasLowerBound, boolean hasUpperBound);

   /** R612
    * data_ref
    *   :   part_ref (T_PERCENT part_ref)*
    *
    * @param numPartRef The number of optional part_refs
    */
   public abstract void data_ref(int numPartRef);

   /** R613, R613-F2008
    * part_ref
    *   :   T_IDENT T_LPAREN section_subscript_list T_RPAREN (image_selector)? 
    *   |   T_IDENT image_selector
    *   |   T_IDENT
    *
    * @param id The identifier (variable name in most cases (all?))
    * @param hasSelectionSubscriptList True if a selection-subscript-list is present
    * @param hasImageSelector Ture if an image-selector is present
    */
   public abstract void
   part_ref(Token id, boolean hasSelectionSubscriptList, boolean hasImageSelector);

   /** R619 (see R1220, actual_arg_spec)
    * section_subscript/actual_arg_spec
    *   :   expr section_subscript_suffix
    *   |   T_COLON (expr)? (T_COLON expr)?
    *   |   T_COLON_COLON expr
    *   |   T_IDENT T_EQUALS (expr | T_ASTERISK label ) 
    *   |   T_ASTERISK label // could be an actual-arg-spec, see R1220 
    *   |   { // empty could be an actual-arg, see R1220 // }
    *
    * R619, section_subscript has been combined with actual_arg_spec
    * (R1220) to reduce backtracking.  Only the first alternative is
    * truly ambiguous.
    * 
    * @param hasLowerBound True if lower bound is present in a
    * section-subscript (lower_bound:upper_bound:stride).
    * @param hasUpperBound True if upper bound is present in a
    * section-subscript (lower_bound:upper_bound:stride).
    * @param hasStride True if stride is present in a
    * section-subscript (lower_bound:upper_bound:stride).
    * @param isAmbiguous True if the third alternative is taken
    */
   public abstract void
   section_subscript(boolean hasLowerBound, boolean hasUpperBound, 
                     boolean hasStride, boolean isAmbiguous);

   /** R619 list
    * section_subscript
    * section_subscript_list
    *   :   section_subscript ( T_COMMA section_subscript )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void section_subscript_list__begin();
   public abstract void section_subscript_list(int count);

   /** R622
    * vector_subscript
    */
   public abstract void vector_subscript();

   /** R623
    * allocate_stmt
    *   :   (label)? T_ALLOCATE_STMT_1 T_ALLOCATE T_LPAREN type_spec T_COLON_COLON
    *       allocation_list (T_COMMA alloc_opt_list)? T_RPAREN T_EOS
    *   |   (label)? T_ALLOCATE T_LPAREN
    *       allocation_list (T_COMMA alloc_opt_list)? T_RPAREN T_EOS
    *
    * @param label Optional statement label
    * @param allocateKeyword T_ALLOCATE token.
    * @param eos T_EOS token.
    * @param hasTypeSpec True if type-spec is present
    * @param hasAllocOptList True if alloc-opt-list is present
    */
   public abstract void
   allocate_stmt(Token label, Token allocateKeyword, Token eos,
                 boolean hasTypeSpec, boolean hasAllocOptList);

   /** R624-F2008
    * image_selector
    *   :   T_LBRACKET cosubscript_list T_RBRACKET
    *
    * @param leftBracket The '[' token
    * @param rightBracket The ']' token
    */
   public abstract void image_selector(Token leftBracket, Token rightBracket);

   /** R624
    * alloc_opt
    *   :   T_IDENT	// {'STAT','ERRMSG'} are variables {SOURCE'} is expr
    *       T_EQUALS expr
    *
    * @param allocOpt Identifier representing {'STAT','ERRMSG','SOURCE'}
    */
   public abstract void alloc_opt(Token allocOpt);

   /** R624 list
    * alloc_opt_list
    *   :   alloc_opt ( T_COMMA alloc_opt )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void alloc_opt_list__begin();
   public abstract void alloc_opt_list(int count);


   /** R625-F08
    * cosubscript_list
    * @param count The number of items in the list.
    */

   public abstract void cosubscript_list__begin();
   public abstract void cosubscript_list(int count, Token team);

   /** R628, R631-F2008
    * allocation
    *   :   ( T_LPAREN allocate_shape_spec_list T_RPAREN )?
    *       ( T_LBRACKET allocate_coarray_spec T_RBRACKET )?
    *
    * NOTE: In current parser, hasAllocateShapeSpecList is always
    * false, appears as R619 section-subscript-list.  In a
    * section-subscript, the stride shall not be present and if
    * hasUpperBound is false, hasLowerBound shall be present and must
    * be interpreted as an upper-bound-expr.
    * 
    * @param hasAllocateShapeSpecList True if allocate-shape-spec-list is present.
    * @param hasAllocateCoarraySpec True if allocate-co-array-spec is present.
    */
   public abstract void
   allocation(boolean hasAllocateShapeSpecList, boolean hasAllocateCoarraySpec);

   /** R628 list
    * allocation_list
    *   :   allocation ( T_COMMA allocation )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void allocation_list__begin();
   public abstract void allocation_list(int count);

   /**
    * R629
    * allocate_object
    *
    */
   public abstract void allocate_object();

   /** R629 list
    * allocate_object_list
    *   :   allocate_object ( T_COMMA allocate_object )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void allocate_object_list__begin();
   public abstract void allocate_object_list(int count);

   /** R630
    * allocate_shape_spec
    *   :   expr (T_COLON expr)?
    *
    * NOTE: not called by current parser, appears as R619
    * section-subscript instead
    *
    * @param hasLowerBound True if optional lower-bound-expr is present.
    * @param hasUpperBound True if upper-bound-expr is present (note
    * always true).
    */
   public abstract void
   allocate_shape_spec(boolean hasLowerBound, boolean hasUpperBound);

   /** R630 list
    * allocate_shape_spec_list
    *   :   allocate_shape_spec ( T_COMMA allocate_shape_spec )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void allocate_shape_spec_list__begin();
   public abstract void allocate_shape_spec_list(int count);

   /** R633
    *	nullify_stmt
    *   :   (label)? T_NULLIFY T_LPAREN pointer_object_list T_RPAREN T_EOS
    *
    * @param label Optional statement label
    * @param nullifyKeyword T_NULLIFY token.
    * @param eos T_EOS token.
    */
   public abstract void
   nullify_stmt(Token label, Token nullifyKeyword, Token eos);

   /** R634
    * pointer_object
    */
   public abstract void pointer_object();
	
   /** R634 list
    * pointer_object_list
    *   :   pointer_object ( T_COMMA pointer_object )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void pointer_object_list__begin();
   public abstract void pointer_object_list(int count);

   /** R635
    * deallocate_stmt
    *   :   (label)? T_DEALLOCATE T_LPAREN allocate_object_list 
    *                ( T_COMMA dealloc_opt_list)? T_RPAREN T_EOS
    *
    * @param label The label.
    * @param deallocateKewyword T_DEALLOCATE token.
    * @param eos T_EOS token.
    * @param hasDeallocOptList True if there is an option list.
    */
   public abstract void
   deallocate_stmt(Token label, Token deallocateKeyword,
                   Token eos, boolean hasDeallocOptList);

   /** R636
    * dealloc_opt
    *
    * @param id T_IDENT for 'STAT' or 'ERRMSG'.
    */
   public abstract void dealloc_opt(Token id);
   
   /** R636 list
    * dealloc_opt_list
    *   :   dealloc_opt ( T_COMMA dealloc_opt )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void dealloc_opt_list__begin();
   public abstract void dealloc_opt_list(int count);

   /** R636-F2008
    * allocate_coarray_spec
    *
    */
   public abstract void allocate_coarray_spec();
   
   /**
    * R637-F08
    * allocate_coshape_spec
    *
    * @param hasExpr Flag specifying whether optional expression was given or 
    * not.  True if given; false if not.
    */
   public abstract void allocate_coshape_spec(boolean hasExpr);

   /** R637-F08 list
    * allocate_coshape_spec_list
    *   :   allocate_coshape_spec ( T_COMMA allocate_co_shape_spec )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void allocate_coshape_spec_list__begin();
   public abstract void allocate_coshape_spec_list(int count);

   /** R701
    * primary
    *   :   designator_or_func_ref
    *   |   literal_constant
    *   |   array_constructor
    *   |   structure_constructor
    *   |   T_LPAREN expr T_RPAREN
    */
   public abstract void primary();

   /** R702
    * level_1_expr
    *   :   (defined_unary_op)? primary
    */
   public abstract void level_1_expr(Token definedUnaryOp);

   /** R703
    * defined_unary_op
    *
    * @param definedOp T_DEFINED_OP token.
    */
   public abstract void defined_unary_op(Token definedOp);

   /** R704: note, inserted as R704 functionality
    * power_operand
    *   :   level_1_expr (power_op power_operand)?
    */
   public abstract void power_operand(boolean hasPowerOperand);
   public abstract void power_operand__power_op(Token powerOp);

   /** R704: note, see power_operand
    * mult_operand
    *   :   power_operand (mult_op power_operand)*
    *  
    *  @param numMults The number of optional mult_ops
    */
   public abstract void mult_operand(int numMultOps);
   public abstract void mult_operand__mult_op(Token multOp);

   /** R705 addition:
    * This rule has been added so the unary plus/minus has the correct
    * precedence when actions are fired.
    *   :   (add_op)? mult_operand
    *  
    * @param addOp Optional add_op for this operand
    */
   public abstract void signed_operand(Token addOp);

   /** R705: note, moved leading optionals to mult_operand
    * add_operand
    *   :   (add_op)? mult_operand (add_op mult_operand)*
    *  
    * @param numAddOps The number of optional add_ops
    */
   public abstract void add_operand(int numAddOps);
   public abstract void add_operand__add_op(Token addOp);

   /** R706: note, moved leading optionals to add_operand
    * level_2_expr
    *   :   add_operand (concat_op add_operand)*
    *  
    *  @param numConcatOps The number of optional numConcatOps
    */
   public abstract void level_2_expr(int numConcatOps);

   /** R707
    * power_op
    *
    * @param powerKeyword T_POWER token.
    */
   public abstract void power_op(Token powerKeyword);

   /** R708
    * mult_op
    *
    * @param multKeyword T_ASTERISK or T_SLASH token.
    */
   public abstract void mult_op(Token multKeyword);

   /** R709
    * add_op
    *
    * @param addKeyword T_PLUS or T_MINUS token.
    */
   public abstract void add_op(Token addKeyword);

   /** R710: note, moved leading optional to level_2_expr
    * level_3_expr
    *   :   level_2_expr (rel_op level_2_expr)?
    *  
    *  @param relOp The rel-op, if present, null otherwise
    */
   public abstract void level_3_expr(Token relOp);

   /** R711
    * concat_op
    *
    * @param concatKeyword T_SLASH_SLASH token.
    */
   public abstract void concat_op(Token concatKeyword);

   /** R713
    * rel_op
    *
    * @param relOp Relational operator token.
    */
   public abstract void rel_op(Token relOp);

   /** R714
    * and_operand
    *   :   (not_op)? level_3_expr (and_op level_3_expr)*
    *
    * @param hasNotOp True if optional not_op is present
    * @param numAndOps The number of optional and_ops
    */
   public abstract void and_operand(boolean hasNotOp, int numAndOps);
   public abstract void and_operand__not_op(boolean hasNotOp);

   /** R715: note, moved leading optional to or_operand
    * or_operand
    *   :   and_operand (or_op and_operand)*
    *
    * @param numOrOps The number of optional or_ops
    */
   public abstract void or_operand(int numOrOps);

   /** R716: note, moved leading optional to or_operand
    * equiv_operand
    *   :   or_operand (equiv_op or_operand)*
    *  
    * @param numEquivOps The number of optional or_operands
    * @param equivOp Optional equiv_op for this operand
    */
   public abstract void equiv_operand(int numEquivOps);
   public abstract void equiv_operand__equiv_op(Token equivOp);

   /** R717: note, moved leading optional to equiv_operand
    * level_5_expr
    *   :   equiv_operand (defined_binary_op equiv_operand)*
    *  
    * @param numDefinedBinaryOps The number of optional equiv_operands
    * @param definedBinaryOp Optional defined_binary_op for this operand
    */
   public abstract void level_5_expr(int numDefinedBinaryOps);
   public abstract void level_5_expr__defined_binary_op(Token definedBinaryOp);

   /** R718
    * not_op
    *
    * @param notOp T_NOT token.
    */
   public abstract void not_op(Token notOp);

   /** R719
    * and_op
    *
    * @param andOp T_AND token.
    */
   public abstract void and_op(Token andOp);

   /** R720
    * or_op
    *
    * @param orOp T_OR token.
    */
   public abstract void or_op(Token orOp);

   /** R721
    * equiv_op
    *
    * @param equivOp T_EQV or T_NEQV token.
    */
   public abstract void equiv_op(Token equivOp);

   /** R722: note, moved leading optional to level_5_expr
    * expr
    *   :   level_5_expr
    */
   public abstract void expr();

   /** R723
    * defined_binary_op
    *
    * @param binaryOp T_DEFINED_OP token.
    */
   public abstract void defined_binary_op(Token binaryOp);

   /** R734
    *	assignment_stmt 
    *   :   (label)? T_ASSIGNMENT_STMT variable	T_EQUALS expr T_EOS
    *
    * @param label Optional statement label
    * @param eos T_EOS token.
    */
   public abstract void assignment_stmt(Token label, Token eos);

   /** R735
    * pointer_assignment_stmt
    *   :   (label)? T_PTR_ASSIGNMENT_STMT data_ref T_EQ_GT expr T_EOS
    *   |   (label)? T_PTR_ASSIGNMENT_STMT data_ref T_LPAREN bounds_spec_list 
    *       T_RPAREN T_EQ_GT expr T_EOS
    *   |   (label)? T_PTR_ASSIGNMENT_STMT data_ref T_LPAREN 
    *       bounds_remapping_list T_RPAREN T_EQ_GT expr T_EOS
    *
    * @param label The label.
    * @param eos T_EOS token.
    * @param hasBoundsSpecList True if has a bounds spec list.
    * @param hasBRList True if has a bounds remapping list.
    */
   public abstract void
   pointer_assignment_stmt(Token label, Token eos,
                           boolean hasBoundsSpecList, boolean hasBRList);

   /** R736
    * data_pointer_object
    */
   public abstract void data_pointer_object();

   /** R737
    * bounds_spec
    */
   public abstract void bounds_spec();

   /** R737 list
    * bounds_spec_list
    *   :   bounds_spec ( T_COMMA bounds_spec )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void bounds_spec_list__begin();
   public abstract void bounds_spec_list(int count);

   /** R738
    * bounds_remapping
    */
   public abstract void bounds_remapping();

   /** R738 list
    * bounds_remapping_list
    *   :   bounds_remapping ( T_COMMA bounds_remapping )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void bounds_remapping_list__begin();
   public abstract void bounds_remapping_list(int count);

   /** R740
    * proc_pointer_object
    */
   public abstract void proc_pointer_object();

   /** R743 
    * where_stmt
    *   :   (label)? T_WHERE_STMT T_WHERE T_LPAREN expr T_RPAREN assignment_stmt
    *
    * @param label The label
    * @param whereKeyword T_WHERE token.
    */
   public abstract void where_stmt__begin();
   public abstract void where_stmt(Token label, Token whereKeyword);

    /** R744 
     * where_construct
     *   :   where_construct_stmt ( where_body_construct )*
     *		( masked_elsewhere_stmt ( where_body_construct )* )*
     *		( elsewhere_stmt ( where_body_construct )* )?
     *        end_where_stmt
     *
     * @param numConstructs The number of where-body-constructs.
     * @param hasMaskedElsewhere True if where-construct has a masked-elsewhere-stmt.
     * @param hasElsewhere True if where-construct has an elsewhere-stmt.
     */
   public abstract void
   where_construct(int numConstructs, boolean hasMaskedElsewhere, boolean hasElsewhere);

   /** R745
    * where_construct_stmt
    *   :   (T_IDENT T_COLON)? T_WHERE_CONSTRUCT_STMT T_WHERE T_LPAREN
    *       expr T_RPAREN T_EOS
    * 
    * @param id Optional name for the loop. If you use this up front,
    * you have to use it in the end, and vice versa.
    * @param whereKeyword T_WHERE token.
    * @param eos T_EOS token.
    */
   public abstract void
   where_construct_stmt(Token id, Token whereKeyword, Token eos);

   /** R746
    * where_body_construct
    */
   public abstract void where_body_construct();

   /** R749 
    * masked_elsewhere_stmt
    *   :   T_ELSE T_WHERE T_LPAREN expr T_RPAREN ( T_IDENT )? T_EOS
    *   |   T_ELSEWHERE    T_LPAREN expr T_RPAREN ( T_IDENT )? T_EOS
    *
    * @param label Optional label.
    * @param elseKeyword Either T_ELSE or T_ELSEWHERE token.
    * @param whereKeyword Either T_WHERE or null.
    * @param id Optional name for the loop. If you use this up front,
    * you have to use it in the end, and vice versa.
    * @param eos T_EOS token.
    */
   public abstract void
   masked_elsewhere_stmt(Token label, Token elseKeyword, Token whereKeyword,
                         Token id, Token eos);

   /** R749 end
    * masked_elsewhere_stmt__end
    *
    * @param numBodyConstructs The number of where-body-constructs in
    * the masked-elsewhere-stmt (called from R744)
    */
   public abstract void masked_elsewhere_stmt__end(int numBodyConstructs);

   /** R750 
    * elsewhere_stmt
    *   :   (label {lbl=$label.tk;})? T_ELSE T_WHERE (id=T_IDENT)? T_EOS
    *   |   (label {lbl=$label.tk;})? T_ELSEWHERE    (id=T_IDENT)? T_EOS 
    *
    * @param label The label
    * @param elseKeyword Either T_ELSE or T_ELSEWHERE token.
    * @param whereKeyword Either T_WHERE or null.
    * @param id Optional name for the loop. If you use this up front,
    * you have to use it in the end, and vice versa.
    * @param eos T_EOS token.
    */
   public abstract void
   elsewhere_stmt(Token label, Token elseKeyword,
                  Token whereKeyword, Token id, Token eos);

   /** R750 end
    * elsewhere_stmt__end
    *
    * @param numBodyConstructs The number of where-body-constructs in
    * the elsewhere-stmt (called from R744)
    */
   public abstract void elsewhere_stmt__end(int numBodyConstructs);

   /** R751 
    * end_where_stmt
    *   :   T_END T_WHERE ( T_IDENT )? T_EOS
    *   |   T_ENDWHERE ( T_IDENT )? T_EOS
    *
    * @param label The label
    * @param endKeyword T_END or T_ENDWHERE token.
    * @param whereKeyword T_WHERE token, if given; null otherwise.
    * @param id Optional name for the loop. If you use this up front,
    * you have to use it in the end, and vice versa.
    * @param eos T_EOS token.
    */
   public abstract void
   end_where_stmt(Token label, Token endKeyword, Token whereKeyword, Token id, Token eos);

   /** R752
    * forall_construct
    */
   public abstract void forall_construct();

   /** R753 
    * forall_construct_stmt
    *
    * (T_IDENT T_COLON)? T_FORALL_CONSTRUCT_STMT T_FORALL forall_header T_EOS
    * @param label The label
    * @param id Optional name for the forall loop. If you use this up
    * front, you have to use it in the end, and vice versa.
    * @param forallKeyword T_FORALL token.
    * @param eos T_EOS token.
    */
   public abstract void forall_construct_stmt(Token label, Token id, 
                                              Token forallKeyword, Token eos);

   /** R754
    * forall_header
    */
   public abstract void forall_header();
	
   /** R755
    * forall_triplet_spec
    *   :   T_IDENT T_EQUALS expr T_COLON expr ( T_COLON expr )?
    *
    * @param id Identifier on left-hand side, e.g., id = 1:10.
    * @param hasStride True if has a stride element in spec., e.g., 1:100:5.
    */
   public abstract void forall_triplet_spec(Token id, boolean hasStride);

   /** R755 list
    * forall_triplet_spec_list
    *   :   forall_triplet_spec ( T_COMMA forall_triplet_spec )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void forall_triplet_spec_list__begin();
   public abstract void forall_triplet_spec_list(int count);

   /** R756
    * forall_body_construct
    */
   public abstract void forall_body_construct();

   /** R757
    * forall_assignment_stmt
    *   :   assignment_stmt
    *   |   pointer_assignment_stmt
    *
    * @param isPointerAssignment True if this is pointer assignment statement.
    */
   public abstract void forall_assignment_stmt(boolean isPointerAssignment);

   /** R758
    * end_forall_stmt
    *   :   (label)? T_END T_FORALL (T_IDENT)? T_EOS
    *   |   (label)? T_ENDFORALL (T_IDENT)? T_EOS
    *
    * @param label The label.
    * @param endKeyword T_END or T_ENDFORALL token.
    * @param forallToken T_FORALL if given, otherise null.
    * @param id Optional identifier for the loop.
    * @param eos T_EOS token.
    */
   public abstract void
   end_forall_stmt(Token label, Token endKeyword, Token forallKeyword, Token id, Token eos);

   /** R759
    * forall_stmt
    *   :   (label)? T_FORALL_STMT T_FORALL forall_header forall_assignment_stmt
    *
    * @param label The label.
    * @param forallKeyword T_FORALL token.
    */
   public abstract void forall_stmt__begin(); 
   public abstract void forall_stmt(Token label, Token forallKeyword);

   /** R801
    * block
    */
   public abstract void block();

   /** R802
    * if_construct
    */
   public abstract void if_construct();

   /** R803
    * if_then_stmt
    *   :   (label)? ( T_IDENT T_COLON )? T_IF T_LPAREN expr T_RPAREN T_THEN T_EOS
    *
    * @param label The label.
    * @param id Optional identifier used for the statement.
    * @param ifKeyword T_IF token.
    * @param thenKeyword T_THEN token.
    * @param eos T_EOS token.
    */
   public abstract void
   if_then_stmt(Token label, Token id, Token ifKeyword, Token thenKeyword, Token eos);

   /** R804
    * else_if_stmt
    *   :   (label)? T_ELSE T_IF T_LPAREN expr T_RPAREN T_THEN ( T_IDENT )? T_EOS
    *   |   (label)? T_ELSEIF T_LPAREN expr T_RPAREN T_THEN ( T_IDENT )? T_EOS
    *
    * @param label The label.
    * @param elseKeyword T_ELSE or T_ELSEIF token.
    * @param ifKeyword T_IF token if given, otherwise null.
    * @param thenKeyword T_THEN token.
    * @param id Optional identifier used for the statement.
    * @param eos T_EOS token.
    */
   public abstract void
   else_if_stmt(Token label, Token elseKeyword, Token ifKeyword,
                Token thenKeyword, Token id, Token eos);

   /** R805
    * else_stmt
    *   :   (label)? T_ELSE ( T_IDENT )? T_EOS
    *
    * @param label The label.
    * @param elseKeyword T_ELSE token.
    * @param id Optional identifier used for the statement.
    * @param eos T_EOS token.
    */
   public abstract void
   else_stmt(Token label, Token elseKeyword, Token id, Token eos);

   /** R806
    * end_if_stmt
    *   :   (label)? T_END T_IF ( T_IDENT )? T_EOS
    *   |   (label)? T_ENDIF    ( T_IDENT )? T_EOS
    *
    * @param label The label.
    * @param endKeyword T_END or T_ENDIF token.
    * @param ifKeyword T_IF token if given; null otherwise.
    * @param id True if has what you think it has.
    * @param eos T_EOS token.
    */
   public abstract void
   end_if_stmt(Token label, Token endKeyword, Token ifKeyword, Token id, Token eos);
   /** R807
    * if_stmt
    *   :   (label)? T_IF_STMT T_IF T_LPAREN expr T_RPAREN action_stmt
    *
    * @param label The label.
    * @param ifKeyword T_IF token.
    */
   public abstract void if_stmt__begin();
   public abstract void if_stmt(Token label, Token ifKeyword);

   /** R807-F08
    * block-construct
    *    is block-stmt
    *          [ specification-part ]
    *          block
    *       end-block-stmt
    */
   public abstract void block_construct();

   /** R807-F08 suppliment
    *  see R204-F08 specification-part for comparison
    *
    * @param numUseStmts  Number of use statements.
    * @param numImportStmts  Number of import statements.
    * @param numDeclConstructs  Number of declaration constructs.
    */
   public abstract void
   specification_part_and_block(int numUseStmts, int numImportStmts, int numDeclConstructs);

   /** R808-F08 block-stmt is [ block-construct-name : ] BLOCK
    *
    * @param label The label
    * @param id The optional block-construct-name identifier
    * @param keyword The T_BLOCK token
    * @param eos The T_EOS token
    */
   public abstract void
   block_stmt(Token label, Token id, Token keyword, Token eos);

   /** R809-F08
    * end-block-stmt
    *    is END BLOCK [ block-construct-name ]
    *
    * @param label The label
    * @param id The optional block-construct-name identifier
    * @param endKeyword The T_END token
    * @param blockKeyword The T_BLOCK token
    * @param eos The T_EOS token
    */
   public abstract void
   end_block_stmt(Token label, Token id,
                  Token endKeyword, Token blockKeyword, Token eos);

   /** R810-F08
    * critical-construct
    *    is critical-stmt block end-critical-stmt
    */
   public abstract void critical_construct();

   /** R811-F08
    * critical-stmt
    *    is [ critical-construct-name : ] CRITICAL
    *
    * @param label The label
    * @param id The optional critical-construct-name identifier
    * @param keyword The T_CRITICAL token
    * @param eos The T_EOS token
    */
   public abstract void
   critical_stmt(Token label, Token id, Token keyword, Token eos);

   /** R812-F08
    * end-critical-stmt
    *    is END CRITICAL [ critical-construct-name ]
    *
    * @param label The label
    * @param id The optional critical-construct-name identifier
    * @param endKeyword The T_END token
    * @param criticalKeyword The T_CRITICAL token
    * @param eos The T_EOS token
    */
   public abstract void
   end_critical_stmt(Token label, Token id,
                     Token endKeyword, Token criticalKeyword, Token eos);

   /** R808
    * case_construct
    */
   public abstract void case_construct();

   /** R809
    * select_case_stmt
    *   :   (label)? ( T_IDENT T_COLON )?  (T_SELECT T_CASE | T_SELECTCASE)
    *       T_LPAREN expr T_RPAREN T_EOS
    *
    * @param label The label.
    * @param id Identifier if present. Otherwise, null.
    * @param selectKeyword T_SELECT or T_SELECTCASE token.
    * @param caseKeyword T_CASE token if given; otherise null.
    * @param eos T_EOS token.
    */
   public abstract void
   select_case_stmt(Token label, Token id,
                    Token selectKeyword, Token caseKeyword, Token eos);

   /** R810
    * case_stmt
    *    :   (label)? T_CASE case_selector ( T_IDENT )? T_EOS
    *
    * @param label The label.
    * @param caseKeyword T_CASE token.
    * @param id Identifier if present. Otherwise, null.
    * @param eos T_EOS token.
    */
   public abstract void
   case_stmt(Token label, Token caseKeyword, Token id, Token eos);

   /** R811
    * end_select_stmt
    *
    * : (label)? T_END T_SELECT (T_IDENT)? T_EOS
    * | (label)? T_ENDSELECT    (T_IDENT)? T_EOS
    *
    * @param label The label.
    * @param endKeyword T_END or T_ENDSELECT token.
    * @param selectKeyword T_SELECT token if given; null otherwise.
    * @param id Identifier if present. Otherwise, null.
    * @param eos T_EOS token.
    */
   public abstract void
   end_select_stmt(Token label, Token endKeyword,
                   Token selectKeyword, Token id, Token eos);

   /** R813
    * case_selector
    *
    * @param defaultToken T_DEFAULT token or null.
    */
   public abstract void case_selector(Token defaultToken);

   /** R814
    * case_value_range
    *
    */
   public abstract void case_value_range();

   /** R814 list
    * case_value_range_list
    *   :   case_value_range ( T_COMMA case_value_range )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void case_value_range_list__begin();
   public abstract void case_value_range_list(int count);

   /**
    * Internal rule.
    * case_value_range_suffix
    * 
    */
   public abstract void case_value_range_suffix();

   /** R815
    * case_value
    */
   public abstract void case_value();

   /** R816
    * associate_construct
    */
   public abstract void associate_construct();

   /** R817
    * associate_stmt
    *   :   (label)? (T_IDENT T_COLON)? T_ASSOCIATE T_LPAREN association_list 
    *                T_RPAREN T_EOS
    *
    * @param label The label.
    * @param id Identifier if present. Otherwise, null.
    * @param associateKeyword T_ASSOCIATE token.
    * @param eos T_EOS token.
    */
   public abstract void
   associate_stmt(Token label, Token id, Token associateKeyword, Token eos);

   /** R817 list
    * association_list
    *   :   association ( T_COMMA association )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void association_list__begin();
   public abstract void association_list(int count);

   /** R818
    * association
    *
    * @param id T_IDENT for associate_name.
    */
   public abstract void association(Token id);

   /** R819
    * selector
    */
   public abstract void selector();

   /** R820
    * end_associate_stmt
    *   :   (label)? T_END T_ASSOCIATE (T_IDENT)? T_EOS
    *   |   (label)? T_ENDASSOCIATE  (T_IDENT)? T_EOS
    *
    * @param label The label.
    * @param endKeyword T_END or T_ENDASSOCIATE token.
    * @param associateKeyword T_ASSOCIATE token if given; null otherwise.
    * @param id Identifier if present. Otherwise, null.
    * @param eos T_EOS token.
    */
   public abstract void
   end_associate_stmt(Token label, Token endKeyword,
                      Token associateKeyword, Token id, Token eos);
   /** R821
    * select_type_construct
    */
   public abstract void select_type_construct();

   /** R822
    * select_type_stmt
    *
    * @param label The label.
    * @param selectConstructName Name of the select construct.
    * @param associateName Name of association.
    * @param eos T_EOS token.
    */
   public abstract void
   select_type_stmt(Token label, Token selectConstructName, Token associateName, Token eos);
   public abstract void select_type(Token selectKeyword, Token typeKeyword);


   /** R823
    * type_guard_stmt
    *   :   (label)? T_TYPE T_IDENT T_LPAREN type_spec T_RPAREN (T_IDENT)? T_EOS
    *   |   (label)? T_CLASS T_IDENT T_LPAREN type_spec T_RPAREN (T_IDENT)? T_EOS
    *   |   (label)? T_CLASS	T_DEFAULT (T_IDENT)? T_EOS
    *
    * @param label The label.
    * @param typeKeyword T_TYPE or T_CLASS token.
    * @param isOrDefaultKeyword T_IDENT token or T_DEFAULT token.  If it is a 
    * T_IDENT, it must represent the string "is" (for "type is" or "class is" syntax).
    * @param selectConstructName Optional identifier immediately before end.
    * @param eos T_EOS token.
    */
   public abstract void
   type_guard_stmt(Token label, Token typeKeyword, Token isOrDefaultKeyword, 
                   Token selectConstructName, Token eos);

   /** R824
    * end_select_type_stmt
    *   :   (label )? T_END T_SELECT ( T_IDENT )? T_EOS
    *   |   (label )? T_ENDSELECT    ( T_IDENT )? T_EOS
    *
    * @param label The label.
    * @param endKeyword T_END or T_ENDSELECT token.
    * @param selectKeyword T_SELECT token if given; null otherwise.
    * @param id The identifier, if present. Otherwise null.
    * @param eos T_EOS token.
    */
   public abstract void
   end_select_type_stmt(Token label, Token endKeyword,
                        Token selectKeyword, Token id, Token eos);

   /** R825
    * do_construct
    */
   public abstract void do_construct();

   /** R826
    * block_do_construct
    */
   public abstract void block_do_construct();

   /** R827
    * do_stmt
    *
    * @param label The label.
    * @param id Identifier for do construct name, if present. Otherwise, null.
    * @param doKeyword T_DO token.
    * @param digitString The value of the digit string, if there. Otherwise, null.
    * @param eos T_EOS token.
    * @param hasLoopControl True if there is a loop control.
    */
   public abstract void
   do_stmt(Token label, Token id, Token doKeyword,
           Token digitString, Token eos, boolean hasLoopControl);

   /** R828
    * label_do_stmt
    *   :   (label)? ( T_IDENT T_COLON )? 
    *		       T_DO T_DIGIT_STRING ( loop_control )? T_EOS
    *
    * @param label The label.
    * @param id Identifier for do construct name, if present. Otherwise, null.
    * @param doKeyword T_DO token.
    * @param digitString Token for the digit string.
    * @param eos T_EOS token.
    * @param hasLoopControl True if there is a loop control.
    */
   public abstract void
   label_do_stmt(Token label, Token id, Token doKeyword,
                 Token digitString, Token eos, boolean hasLoopControl);

   /** R818-F03, R830-F03
    * loop_control
    *
    * @param keyword T_WHILE, T_CONCURRENT, or null.
    * @param doConstructType The type of do (variable, while, concurrent)
    * @param hasOptExpr Flag specifying if optional expression was given (if type is variable)
    */
   public abstract void loop_control(Token keyword, int doConstructType, boolean hasOptExpr);

   /** R831
    * do_variable is scalar-int-variable-name
    */
   public abstract void do_variable(Token id);

   /** R833
    * end_do
    */
   public abstract void end_do();

   /** R834
    * end_do_stmt
    *   :   (label)? T_END T_DO ( T_IDENT )? T_EOS
    *   |   (label)? T_ENDDO    ( T_IDENT )? T_EOS
    *
    * @param label The label.
    * @param endKeyword T_END or T_ENDDO token.
    * @param doKeyword T_DO token if given, null otherwise.
    * @param id The do construct name, if present. Otherwise, null.
    * @param eos T_EOS token.
    */
   public abstract void
   end_do_stmt(Token label, Token endKeyword, Token doKeyword, Token id, Token eos);

   /** R838
    * do_term_action_stmt
    *
    * Try requiring an action_stmt and then we can simply insert the new
    * T_LABEL_DO_TERMINAL during the Sale's prepass.  T_EOS is in action_stmt.
    * added the T_END T_DO and T_ENDDO options to this rule because of the
    * token T_LABEL_DO_TERMINAL that is inserted if they end a labeled DO.
    *
    *   :   label T_LABEL_DO_TERMINAL 
    *       (action_stmt | ( (T_END T_DO | T_ENDDO) (T_IDENT)? ) T_EOS)
    *
    * @param label The label, which must be present.
    * @param endKeyword T_END or T_ENDDO if given, null otherwise.
    * @param doKeyword T_DO token if given, null otherwise.
    * @param id The identifier, if present. Otherwise, null.
    * @param eos T_EOS token.
    */
   public abstract void
   do_term_action_stmt(Token label, Token endKeyword,
                       Token doKeyword, Token id, Token eos);

   /** R843
    * cycle_stmt
    *   :   (label)? T_CYCLE (T_IDENT)? T_EOS
    * 
    * T_IDENT inlined for do_construct_name
    * 
    * @param label Optional statement label
    * @param cycleKeyword T_CYCLE token.
    * @param id Optional do-construct-name
    * @param eos T_EOS token.
    */
   public abstract void
   cycle_stmt(Token label, Token cycleKeyword, Token id, Token eos);

   /** R844
    * exit_stmt
    *   :   (label)? T_EXIT (T_IDENT)? T_EOS
    *
    * T_IDENT inlined for do_construct_name
    * 
    * @param label Optional statement label
    * @param exitKeyword T_EXIT token.
    * @param id Optional do-construct-name
    * @param eos T_EOS token.
    */
   public abstract void
   exit_stmt(Token label, Token exitKeyword, Token id, Token eos);

   /** R845
    * goto_stmt
    *   :  (label)? t_go_to target_label T_EOS
    *
    * @param label Optional statement label
    * @param goKeyword T_GO or T_GOTO token.
    * @param toKeyword T_TO token, if given, null otherwise.
    * @param target_label The branch target statement label
    * @param eos T_EOS token.
    */
   public abstract void
   goto_stmt(Token label, Token goKeyword, Token toKeyword, Token target_label, Token eos);

   /** R846
    * computed_goto_stmt
    *   :   (label)? t_go_to T_LPAREN label_list T_RPAREN (T_COMMA)? expr T_EOS
    *
    * ERR_CHK 846 scalar_int_expr replaced by expr
    * 
    * @param label Optional statement label
    * @param goKeyword T_GO or T_GOTO token.
    * @param toKeyword T_TO token, if given, null otherwise.
    * @param eos T_EOS token.
    */
   public abstract void
   computed_goto_stmt(Token label, Token goKeyword, Token toKeyword, Token eos);

   /**
    * assign_stmt
    *
    * @param label1 Optional statement label.
    * @param assignKeyword T_ASSIGN token.
    * @param label2 Required label for assign_stmt.
    * @param toKeyword T_TO token.
    * @param name T_IDENT for name subrule.
    * @param eos T_EOS token.
    * Note: This is a deleted feature.
    */
   public abstract void
   assign_stmt(Token label1, Token assignKeyword,
               Token label2, Token toKeyword, Token name, Token eos);

   /**
    * assigned_goto_stmt
    *
    * @param label Optional statement label.
    * @param goKeyword T_GO or T_GOTO token.
    * @param toKeyword T_TO token if given; null otherwise.
    * @param name T_IDENT token for name subrule.
    * @param eos T_EOS token.
    * Note: This is a deleted feature.
    */
   public abstract void
   assigned_goto_stmt(Token label, Token goKeyword,
                      Token toKeyword, Token name, Token eos);

   /**
    * Internal rule.
    * stmt_label_list
    */
   public abstract void stmt_label_list();

   /**
    * pause_stmt
    *
    * @param label Optional statement label.
    * @param pauseKeyword T_PAUSE token.
    * @param constant T_DIGIT_STRING or null if is a char_literal_constant.
    * @param eos T_EOS token.
    * Note: This is a deleted feature.
    */
   public abstract void
   pause_stmt(Token label, Token pauseKeyword, Token constant, Token eos);

   /** R847
    * arithmetic_if_stmt
    *   :   (label)? T_ARITHMETIC_IF_STMT T_IF
    *       T_LPAREN expr T_RPAREN label T_COMMA label T_COMMA label T_EOS
    *
    * ERR_CHK 847 scalar_numeric_expr replaced by expr
    * 
    * @param label  Optional statement label
    * @param ifKeyword T_IF token.
    * @param label1 The first branch target statement label
    * @param label2 The second branch target statement label
    * @param label3 The third branch target statement label
    * @param eos T_EOS token.
    */
   public abstract void
   arithmetic_if_stmt(Token label, Token ifKeyword,
                      Token label1, Token label2, Token label3, Token eos);

   /** R848
    * continue_stmt
    *   :   (label)? T_CONTINUE
    * 
    * @param label  Optional statement label
    * @param continueKeyword T_CONTINUE token.
    * @param eos T_EOS token.
    */
   public abstract void
   continue_stmt(Token label, Token continueKeyword, Token eos);

   /** R849
    * stop_stmt
    *   :   (label)? T_STOP (stop_code)? T_EOS
    *
    * @param label Optional statement label
    * @param stopKeyword T_STOP token.
    * @param eos T_EOS token.
    * @param hasStopCode True if the stop-code is present, false otherwise
    */
   public abstract void
   stop_stmt(Token label, Token stopKeyword, Token eos, boolean hasStopCode);

   /** R850
    * stop_code
    *   :   scalar_char_constant
    *   |   T_DIGIT_STRING
    * 
    * ERR_CHK 850 T_DIGIT_STRING must be 5 digits or less
    * 
    * @param digitString The stop-code token, otherwise is a
    * scalar-char-constant
    */
   public abstract void stop_code(Token digitString);
	
   /** R856-F08
    * errorstop_stmt
    *   :   (label)? T_ERROR T_STOP (stop_code)? T_EOS
    *
    * @param label Optional statement label
    * @param errorKeyword T_ERROR token.
    * @param stopKeyword T_STOP token.
    * @param eos T_EOS token.
    * @param hasStopCode True if the stop-code is present, false otherwise
    */
   public abstract void
   errorstop_stmt(Token label, Token errorKeyword,
                  Token stopKeyword, Token eos, boolean hasStopCode);

   /** R858-F08
    * sync_all_stmt
    *   :   (label)? T_SYNC T_ALL (sync_stat_list)? T_EOS
    *
    * @param label Optional statement label
    * @param syncKeyword T_SYNC token.
    * @param allKeyword T_ALL token.
    * @param eos T_EOS token.
    * @param hasSyncStatList True if the sync_stat_list is present, false otherwise
    */
   public abstract void
   sync_all_stmt(Token label, Token syncKeyword,
                 Token allKeyword, Token eos, boolean hasSyncStatList);

   /** R859-F08
    * sync_stat
    *   :   T_IDENT            // {'STAT','ERRMSG'} are variables}
    *       T_EQUALS expr   // expr is a stat-variable or an errmsg-variable
    *
    * @param syncStat Identifier representing {'STAT','ERRMSG'}
    */
   public abstract void sync_stat(Token syncStat);

   /** R859-F08 list
    * sync_stat_list
    *    :   sync_stat ( T_COMMA sync_stat )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void sync_stat_list__begin();
   public abstract void sync_stat_list(int count);

   /** R860-F08
    * sync-images-stmt
    *    is SYNC IMAGES ( image-set [, sync-stat-list ] )
    *
    * @param label Optional statement label
    * @param syncKeyword T_SYNC token.
    * @param imagesKeyword T_IMAGES token.
    * @param eos T_EOS token.
    * @param hasSyncStatList True if the sync_stat_list is present, false otherwise
    */
   public abstract void
   sync_images_stmt(Token label, Token syncKeyword,
                    Token imagesKeyword, Token eos, boolean hasSyncStatList);

   /** R861-F08
    * image-set
    *    is int-expr
    *    or *
    * @param asterisk Optional asterisk token T_ASTERISK
    * @param hasIntExpr True if int_expr is present, false otherwise
    */
   public abstract void image_set(Token asterisk, boolean hasIntExpr);

   /** R862-F08
    * sync_memory_stmt
    *   :   (label)? T_SYNC T_MEMORY (sync_stat_list)? T_EOS
    *
    * @param label Optional statement label
    * @param syncKeyword T_SYNC token.
    * @param memoryKeyword T_MEMORY token.
    * @param eos T_EOS token.
    * @param hasSyncStatList True if the sync_stat_list is present, false otherwise
    */
   public abstract void
   sync_memory_stmt(Token label, Token syncKeyword,
                    Token memoryKeyword, Token eos, boolean hasSyncStatList);

   /**
    * R863-F08 lock-stmt
    *    (label)? T_LOCK lock_variable (lock_stat_list)? T_EOS
    *
    * @param label Optional statement label
    * @param lockKeyword T_LOCK token
    * @param eos T_EOS token
    * @param hasLockStatList True if lock_stat_list is present, false otherwise
    */
   public abstract void
   lock_stmt(Token label, Token lockKeyword, Token eos, boolean hasLockStatList);

   /** R864-F08
    * lock_stat
    *   :   T_ACQUIRED_LOCK T_EQUALS scalar_logical_variable
    *   |   sync_stat
    *
    * @param acquiredKeyword T_ACQUIRED_LOCK token
    */
    public abstract void lock_stat(Token acquiredKeyword);

   /** R864-F08 list
    * lock_stat_list
    *   :   lock_stat ( T_COMMA lock_stat )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void lock_stat_list__begin();
   public abstract void lock_stat_list(int count);

   /** R865-F08
    * unlock-stmt
    *   :  (label)? T_UNLOCK lock_variable (sync_stat_list)? T_EOS
    *
    * @param label Optional statement label
    * @param unlockKeyword T_UNLOCK token
    * @param eos T_EOS token
    * @param hasSyncStatList True if sync_stat_list is present, false otherwise
    */
   public abstract void
   unlock_stmt(Token label, Token unlockKeyword, Token eos, boolean hasSyncStatList);

   /** R866-F08
    * lock-variable
    *    is scalar-variable
    */
   public abstract void lock_variable();

   /**
    * Unknown rule.
    * scalar_char_constant
    */
   public abstract void scalar_char_constant();

   /** R901
    * io_unit
    */
   public abstract void io_unit();

   /** R902
    * file_unit_number
    */
   public abstract void file_unit_number();

   /** R904
    * open_stmt
    *   :   (label)? T_OPEN T_LPAREN connect_spec_list T_RPAREN T_EOS
    *
    * @param label The label.
    * @param openKeyword T_OPEN token.
    * @param eos T_EOS token.
    */
   public abstract void open_stmt(Token label, Token openKeyword, Token eos);

   /** R905
    * connect_spec
    *
    * @param id T_IDENT token for second alternative; otherwise null.
    */
   public abstract void connect_spec(Token id);

   /** R905 list
    * connect_spec_list
    *   :   connect_spec ( T_COMMA connect_spec )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void connect_spec_list__begin();
   public abstract void connect_spec_list(int count);

   /** R908
    * close_stmt
    *   :   (label)? T_CLOSE T_LPAREN close_spec_list T_RPAREN T_EOS
    *
    * @param label The label.
    * @param closeKeyword T_CLOSE token.
    * @param eos T_EOS token.
    */
   public abstract void close_stmt(Token label, Token closeKeyword, Token eos);

   /** R909
    * close_spec
    *
    * @param closeSpec T_IDENT for second alternative; null otherwise.
    */
   public abstract void close_spec(Token closeSpec);

   /** R909 list
    * close_spec_list
    *   :   close_spec ( T_COMMA close_spec )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void close_spec_list__begin();
   public abstract void close_spec_list(int count);

   /** R910
    * read_stmt
    *   :   ((label)? T_READ T_LPAREN) => (label)? T_READ T_LPAREN 
    *           io_control_spec_list T_RPAREN ( input_item_list )? T_EOS
    *   |   ((label)? T_READ) => (label)? T_READ format 
    *           ( T_COMMA input_item_list )? T_EOS
    *
    * @param label The label.
    * @param readKeyword T_READ token.
    * @param eos T_EOS token.
    * @param hasInputItemList True if has an input item list.
    */
   public abstract void
   read_stmt(Token label, Token readKeyword, Token eos, boolean hasInputItemList);

   /** R911
    * write_stmt
    *   :   (label)? T_WRITE T_LPAREN io_control_spec_list T_RPAREN 
    * 			(output_item_list)? T_EOS
    *
    * @param label The statement label
    * @param writeKeyword T_WRITE token.
    * @param eos T_EOS token.
    * @param hasOutputItemList True if output-item-list is present
    */
   public abstract void
   write_stmt(Token label, Token writeKeyword, Token eos, boolean hasOutputItemList);

   /** R912
    * print_stmt
    *   :   (label)? T_PRINT format ( T_COMMA output_item_list )? T_EOS
    *
    * @param label The label.
    * @param printKeyword T_PRINT token.
    * @param eos T_EOS token.
    * @param hasOutputItemList True if output-item-list is present
    */
   public abstract void
   print_stmt(Token label, Token printKeyword, Token eos, boolean hasOutputItemList);

   /** R913
    * io_control_spec
    *   :   expr
    *   |   T_ASTERISK
    *   |   T_IDENT // {'UNIT','FMT'}
    *       T_EQUALS T_ASTERISK
    *   |   T_IDENT
    *       // {'UNIT','FMT'} are expr 'NML' is T_IDENT}
    *	    // {'ADVANCE','ASYNCHRONOUS','BLANK','DECIMAL','DELIM'} are expr
    *	    // {'END','EOR','ERR'} are labels
    *	    // {'ID','IOMSG',IOSTAT','SIZE'} are variables
    *	    // {'PAD','POS','REC','ROUND','SIGN'} are expr
    *		T_EQUALS expr
    *
    * ERR_CHK 913 check expr type with identifier
    * io_unit and format are both (expr|'*') so combined
    * 
    * @param hasExpression True if the io-control-spec has an expression
    * @param keyword Represents the keyword if present
    * @param hasAsterisk True if an '*' is present
    */
   public abstract void
   io_control_spec(boolean hasExpression, Token keyword, boolean hasAsterisk);

   /** R913 list
    * io_control_spec_list
    *   :   io_control_spec ( T_COMMA io_control_spec )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void io_control_spec_list__begin();
   public abstract void io_control_spec_list(int count);

   /** R914
    * format
    */
   public abstract void format();

   /** R915
    * input_item
    */
   public abstract void input_item();

   /** R915 list
    * input_item_list
    *   :   input_item ( T_COMMA input_item )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void input_item_list__begin();
   public abstract void input_item_list(int count);

   /** R916
    * output_item
    */
   public abstract void output_item();

   /** R916 list
    * output_item_list
    *   :   output_item ( T_COMMA output_item )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void output_item_list__begin();
   public abstract void output_item_list(int count);

   /** R917
    * io_implied_do
    */
   public abstract void io_implied_do();

   /** R918
    * io_implied_do_object
    */
   public abstract void io_implied_do_object();

   /** R919
    * io_implied_do_control
    *
    * @param hasStride True if is of the form a = b,c,d where d is optional stride
    */
   public abstract void io_implied_do_control(boolean hasStride);

   /** R920
    * dtv_type_spec
    *
    * @param typeKeyword T_TYPE or T_CLASS token.
    */
   public abstract void dtv_type_spec(Token typeKeyword);

   /** R921
    * wait_stmt
    *   :   (label)? T_WAIT T_LPAREN wait_spec_list T_RPAREN T_EOS
    *
    * @param label The label.
    * @param waitKeyword T_WAIT token.
    * @param eos T_EOS token.
    */
   public abstract void wait_stmt(Token label, Token waitKeyword, Token eos);

   /** R922
    * wait_spec
    * 
    * @param id T_IDENT or null.
    */
   public abstract void wait_spec(Token id);

   /** R922 list
    * wait_spec_list
    *   :   wait_spec ( T_COMMA wait_spec )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void wait_spec_list__begin();
   public abstract void wait_spec_list(int count);

   /** R923
    * backspace_stmt
    *   :   ((label)? T_BACKSPACE T_LPAREN) => (label)? T_BACKSPACE T_LPAREN 
    *                 position_spec_list T_RPAREN T_EOS
    *   |   ((label)? T_BACKSPACE) => (label)? T_BACKSPACE file_unit_number T_EOS
    *
    * @param label The label.
    * @param backspaceKeyword T_BACKSPACE token.
    * @param eos T_EOS token.
    * @param hasPositionSpecList True if there is a position spec
    * list. False is there is a file unit number.
    */
   public abstract void
   backspace_stmt(Token label, Token backspaceKeyword,
                  Token eos, boolean hasPositionSpecList);

   /** R924
    * endfile_stmt
    *   :   ((label)? T_END T_FILE T_LPAREN) => (label)? T_END T_FILE 
    *                 T_LPAREN position_spec_list T_RPAREN T_EOS
    *   |   ((label)? T_ENDFILE T_LPAREN) => (label)? T_ENDFILE T_LPAREN 
    *                 position_spec_list T_RPAREN T_EOS
    *   |   ((label)? T_END T_FILE)=> (label)? T_END T_FILE file_unit_number T_EOS
    *   |   ((label)? T_ENDFILE) => (label)? T_ENDFILE file_unit_number T_EOS
    *
    * @param label The label.
    * @param endKeyword T_END or T_ENDFILE.
    * @param fileKeyword T_FILE or null.
    * @param eos T_EOS token.
    * @param hasPositionSpecList True if there is a position spec
    * list. False is there is a file unit number.
    */
   public abstract void
   endfile_stmt(Token label, Token endKeyword, Token fileKeyword,
                Token eos, boolean hasPositionSpecList);

   /** R925
    * rewind_stmt
    *   :   ((label)? T_REWIND T_LPAREN) => (label)? T_REWIND T_LPAREN 
    *                 position_spec_list T_RPAREN T_EOS
    *   |   ((label)? T_REWIND) => (label)? T_REWIND file_unit_number T_EOS
    *
    * @param label The label.
    * @param rewindKeyword T_REWIND token.
    * @param eos T_EOS token.
    * @param hasPositionSpecList True if there is a position spec
    * list. False is there is a file unit number.
    */
   public abstract void
   rewind_stmt(Token label, Token rewindKeyword, Token eos, boolean hasPositionSpecList);

   /** R926
    * position_spec
    *
    * @param id T_IDENT for the specifier or null.
    */
   public abstract void position_spec(Token id);

   /** R926 list
    * position_spec_list
    *   :   position_spec ( T_COMMA position_spec )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void position_spec_list__begin();
   public abstract void position_spec_list(int count);

   /** R927
    * flush_stmt
    *   :   ((label)? T_FLUSH T_LPAREN) => (label)? T_FLUSH T_LPAREN       \
    *                 flush_spec_list T_RPAREN T_EOS
    *   |   ((label)? T_FLUSH) => (label)? T_FLUSH file_unit_number T_EOS
    *
    * @param label The label.
    * @param flushKeyword T_FLUSH token.
    * @param eos T_EOS token.
    * @param hasFlushSpecList True if there is a flush spec
    * list. False is there is a file unit number.
    */
   public abstract void
   flush_stmt(Token label, Token flushKeyword, Token eos, boolean hasFlushSpecList);

   /** R928
    * flush_spec
    *
    * @param id T_IDENT for specifier or null.
    */
   public abstract void flush_spec(Token id);

   /** R928 list
    * flush_spec_list
    *   :   flush_spec ( T_COMMA flush_spec )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void flush_spec_list__begin();
   public abstract void flush_spec_list(int count);
   
   /** R929
    * inquire_stmt
    *   :   (label)? T_INQUIRE T_LPAREN inquire_spec_list T_RPAREN T_EOS
    *   |   (label)? T_INQUIRE_STMT_2 T_INQUIRE T_LPAREN T_IDENT T_EQUALS 
    *                scalar_int_variable T_RPAREN output_item_list T_EOS
    *
    * @param label Optional statement label.
    * @param inquireKeyword T_INQUIRE token.
    * @param id T_IDENT token for type 2 inquire statement; null for type 1.
    * @param eos T_EOS token.
    * @param isType2 true if is type 2; false otherwise.
    */
   public abstract void
   inquire_stmt(Token label, Token inquireKeyword, Token id, Token eos, boolean isType2);

   /** R930
    * inquire_spec
    *
    * @param id T_IDENT for specifier or null.
    */
   public abstract void inquire_spec(Token id);

   /** R930 list
    * inquire_spec_list
    *   :   inquire_spec ( T_COMMA inquire_spec )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void inquire_spec_list__begin();
   public abstract void inquire_spec_list(int count);

   /** R1001
    * format_stmt
    *   :   (label)? T_FORMAT format_specification T_EOS
    *
    * @param label The label.
    * @param formatKeyword T_FORMAT token.
    * @param eos T_EOS token.
    */
   public abstract void format_stmt(Token label, Token formatKeyword, Token eos);

   /** R1002
    * format_specification
    *
    *   :   T_LPAREN ( format_item_list )? T_RPAREN
    *
    * @param hasFormatItemList True if has a format item list.
    */
   public abstract void format_specification(boolean hasFormatItemList);

   /** R1003
    * format_item
    *   :   T_DATA_EDIT_DESC 
    *   |   T_CONTROL_EDIT_DESC
    *   |   T_CHAR_STRING_EDIT_DESC
    *   |   (T_DIGIT_STRING)? T_LPAREN format_item_list T_RPAREN
    *
    * @param descOrDigit Edit descriptor, unless has a format item
    * list, then either the optinal digit string or null.
    * @param hasFormatItemList True if has a format item list.
    */
   public abstract void format_item(Token descOrDigit, boolean hasFormatItemList);

   /** R1003 list
    * format_item_list
    *   :   format_item ( T_COMMA format_item )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void format_item_list__begin();
   public abstract void format_item_list(int count);

   /** R1010 list
    * v_list_part
    * v_list
    *   :   (T_PLUS|T_MINUS)? T_DIGIT_STRING 
    *       ( T_COMMA (T_PLUS|T_MINUS)? T_DIGIT_STRING )*
    * 
    * @param plus_minus Optional T_PLUSIT_MINUS token.
    * @param digitString The digit string token.
    * @param count The number of items in the list.
    */
   public abstract void v_list_part(Token plus_minus, Token digitString);
   public abstract void v_list__begin();
   public abstract void v_list(int count);

   /** R1101
    * main_program
    *	(program_stmt)?	
    * specification_part (execution_part)? 
    * (internal_subprogram_part)?
    *	end_program_stmt
    * 
    * @param hasProgramStmt Optional program-stmt
    * @param hasExecutionPart Optional execution-part
    * @param hasInternalSubprogramPart Optional internal-subprogram-part
    */
   public abstract void main_program__begin();
   public abstract void main_program(boolean hasProgramStmt,
                                     boolean hasExecutionPart,
                                     boolean hasInternalSubprogramPart);

   /** R1101
    * ext_function_subprogram
    *   :   (prefix)? function_subprogram
    *
    * @param hasPrefix True if has a prefix.
    */
   public abstract void ext_function_subprogram(boolean hasPrefix);

   /** R1102
    * program_stmt
    *   :   (label)? ...
    * 
    * @param label Optional statement label
    * @param programKeyword T_PROGRAM token.
    * @param id Optional program name
    * @param eos T_EOS token.
    */
   public abstract void
   program_stmt(Token label, Token programKeyword, Token id, Token eos);

   /** R1103
    * end_program_stmt
    *   :   (label)? ...
    * 
    * @param label Optional statement label
    * @param endKeyword T_END or T_ENDPROGRAM token.
    * @param programKeyword T_PROGRAM token if given; null otherwise.
    * @param id Optional program name
    * @param eos T_EOS token.
    */
   public abstract void
   end_program_stmt(Token label, Token endKeyword,
                    Token programKeyword, Token id, Token eos);

   /** R1104
    * module
    */
   public abstract void module();
   
   /** R1105
    * module_stmt__begin
    */
   public abstract void module_stmt__begin();

   /** R1105
    * module_stmt
    *   :   (label)? T_MODULE (T_IDENT)? T_EOS
    *
    * @param label The label.
    * @param moduleKeyword T_MODULE token.
    * @param id The identifier, if present, otherwise null.
    * @param eos T_EOS token.
    */
   public abstract void
   module_stmt(Token label, Token moduleKeyword, Token id, Token eos);

   /** R1106
    * end_module_stmt
    *   :   (label)? T_END T_MODULE (T_IDENT)? end_of_stmt
    *   |   (label)? T_ENDMODULE    (T_IDENT)? end_of_stmt
    *   |   (label)? T_END end_of_stmt
    *
    * @param label The label.
    * @param endKeyword T_END or T_ENDMODULE token.
    * @param moduleKeyword T_MODULE token if given; null otherwise.
    * @param id The identifier, if present, otherwise null.
    * @param eos T_EOS token.
    */
   public abstract void
   end_module_stmt(Token label, Token endKeyword, 
                   Token moduleKeyword, Token id, Token eos);

   /** R1107-F08
    * module_subprogram_part
    *     is   contains-stmt
    *          [ module-subprogram ] ...
    *
    * @param count The number of module-subprograms.
    */
   public abstract void module_subprogram_part(int count);
   
   /** R1108
    * module_subprogram
    *   :   (prefix)? function_subprogram
    *   |   subroutine_subprogram
    *
    * @param hasPrefix Boolean true if has a prefix.
    */
   public abstract void module_subprogram(boolean hasPrefix);

   /** R1109
    * use_stmt
    *   :   (label)? T_USE ( (T_COMMA module_nature)? T_COLON_COLON )? T_IDENT 
    *                      ( T_COMMA rename_list)? T_EOS
    *   |   (label)? T_USE ( (T_COMMA module_nature)? T_COLON_COLON )? T_IDENT 
    *                      T_COMMA T_ONLY T_COLON (only_list)? T_EOS
    *
    * @param label The label.
    * @param useKeyword T_USE token.
    * @param id T_IDENT token for module name.
    * @param onlyKeyword T_ONLY token if given; null otherwise.
    * @param eos T_EOS token.
    * @param hasModuleNature True if has a module nature.
    * @param hasRenameList True if has a rename list.
    * @param hasOnly True if has an only statement, regardless of
    * whether a list is present.
    */
   public abstract void
   use_stmt(Token label, Token useKeyword, Token id, Token onlyKeyword, Token eos,
            boolean hasModuleNature, boolean hasRenameList, boolean hasOnly);

   /** R1110
    * module_nature
    * 
    * @param nature T_INTRINSIC or T_NON_INTRINSIC token.
    */
   public abstract void module_nature(Token nature);

   /** R1111 
    * rename
    * 
    * @param id1 First T_IDENT for alt1 or null if alt2.
    * @param id2 Second T_IDENT for alt1 or null if alt2.
    * @param op1 First T_OPERATOR for alt2 or null if alt1.
    * @param defOp1 First T_DEFINED_OP for alt2 or null if alt1.
    * @param op2 Second T_OPERATOR for alt2 or null if alt1.
    * @param defOp2 Second T_DEFINED_OP for alt2 or null if alt1.
    */
   public abstract void
   rename(Token id1, Token id2, Token op1, Token defOp1, Token op2, Token defOp2);

   /** R1111 list
    * rename_list
    *   :   rename ( T_COMMA rename )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void rename_list__begin();
   public abstract void rename_list(int count);
   
   /** R1112
    * only
    *   :   generic-spec 
    *   |   only-use-name 
    *   |   rename 
    *
    * @param hasGenericSpec True if has a generic spec.
    * @param hasRename True if has a rename
    * @param hasOnlyUseName True if has an only use name
    */
   public abstract void
   only(boolean hasGenericSpec, boolean hasRename, boolean hasOnlyUseName);

   /** R1112 list
    * only_list
    *   :   only ( T_COMMA only )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void only_list__begin();
   public abstract void only_list(int count);

   /** R1116-F08
    * submodule
    *     is submodule-stmt
    *           [ specification-part ]
    *           [ module-subprogram-part ]
    *     end-submodule-stmt
    */
   public abstract void submodule(boolean hasModuleSubprogramPart);
   
   /** R1117-F08
    * submodule_stmt__begin
    */
   public abstract void submodule_stmt__begin();

   /** R1117-F08
    * submodule-stmt
    *     is SUBMODULE ( parent-identifier ) submodule-name
    *
    * @param label The label.
    * @param submoduleKeyword T_SUBMODULE token if given; null otherwise.
    * @param name Token for submodule name.
    * @param eos T_EOS token.
    */
   public abstract void
   submodule_stmt(Token label, Token submoduleKeyword, Token name, Token eos);

   /** R1118-F08
    * parent-identifier
    *     is ancestor-module-name [ : parent-submodule-name ]
    *
    * @param ancestor The ancestor-module-name token.
    * @param parent The parent-submodule-name token if given; null otherwise.
    */
   public abstract void parent_identifier(Token ancestor, Token parent);

   /** R1119-F08
    * end-submodule-stmt
    *     is END [ SUBMODULE [ submodule-name ] ]
    *
    * @param label The label.
    * @param endKeyword T_END token.
    * @param submoduleKeyword T_SUBMODULE token if given; null otherwise.
    * @param name T_IDENT token for submodule name if given; null otherwise.
    * @param eos T_EOS token.
    */
   public abstract void
   end_submodule_stmt(Token label, Token endKeyword,
                      Token submoduleKeyword, Token name, Token eos);

   /** R1116
    * block_data
    */
   public abstract void block_data();

   /** R1117
    * block_data_stmt__begin
    */
   public abstract void block_data_stmt__begin();

   /** R1117
    * block_data_stmt
    *   :   (label)? T_BLOCK T_DATA (T_IDENT)? T_EOS
    *   |   (label)? T_BLOCKDATA  (T_IDENT)? T_EOS
    *
    * @param label The label.
    * @param blockKeyword T_BLOCK or T_BLOCKDATA token.
    * @param dataKeyword T_DATA token if given; null otherwise.
    * @param id Identifier if it exists. Otherwise, null.
    * @param eos T_EOS token.
    */
   public abstract void
   block_data_stmt(Token label, Token blockKeyword,
                   Token dataKeyword, Token id, Token eos);

   /** R1118
    * end_block_data_stmt
    *   :   (label)? T_END T_BLOCK T_DATA ( T_IDENT )? end_of_stmt
    *   |   (label)? T_ENDBLOCK T_DATA    ( T_IDENT )? end_of_stmt
    *   |   (label)? T_END T_BLOCKDATA    ( T_IDENT )? end_of_stmt
    *   |   (label)? T_ENDBLOCKDATA       ( T_IDENT )? end_of_stmt
    *   |   (label)? T_END end_of_stmt
    *
    * @param label The label.
    * @param endKeyword T_END, T_ENDBLOCK, or T_ENDBLOCKDATA token.
    * @param blockKeyword T_BLOCK or T_BLOCKDATA token; null otherwise.
    * @param dataKeyword T_DATA token if given; null otherwise.
    * @param id Identifier if it exists. Otherwise, null.
    * @param eos T_EOS token.
    */
   public abstract void
   end_block_data_stmt(Token label, Token endKeyword, Token blockKeyword, 
                       Token dataKeyword, Token id, Token eos);
   
   /** R1201
    * interface_block
    */
   public abstract void interface_block();

   /** R1202
    * interface_specification
    */
   public abstract void interface_specification();

   /** R1203
    * interface_stmt__begin
    */
   public abstract void interface_stmt__begin();

   /** R1203
    * interface_stmt
    *   :   (label)? T_INTERFACE ( generic_spec )? T_EOS
    *   |   (label)? T_ABSTRACT T_INTERFACE T_EOS
    *
    * @param label The label.
    * @param abstract The ABSTRACT keyword token (null if the
    * interface is not abstract).
    * @param keyword The INTERFACE keyword token.
    * @param eos End of statement token.
    * @param hasGenericSpec True if has a generic spec.
    */
   public abstract void
   interface_stmt(Token label, Token abstractToken,
                  Token keyword, Token eos, boolean hasGenericSpec);

   /** R1204
    * end_interface_stmt
    *   :   (label)? T_END T_INTERFACE ( generic_spec )? T_EOS
    *   |   (label)? T_ENDINTERFACE    ( generic_spec )? T_EOS
    *
    * @param label The label.
    * @param kw1 The END keyword token (may be ENDINTERFACE).
    * @param kw2 The INTERFACE keyword token (may be null).
    * @param eos End of statement token.
    * @param hasGenericSpec True if has a generic spec.
    */
   public abstract void
   end_interface_stmt(Token label, Token kw1,
                      Token kw2, Token eos, boolean hasGenericSpec);

   /** R1205
    * interface_body
    * 
    * @param hasPrefix Boolean flag for whether the optional prefix was given.
    * This only applies for alt1 (always false for alt2).
    */
   public abstract void interface_body(boolean hasPrefix);

   /** R1206
    * procedure_stmt
    *   :   (label)? ( T_MODULE )? T_PROCEDURE generic_name_list T_EOS
    *
    * @param label The label.
    * @param module Name of the module, if present.
    * @param procedureKeyword T_PROCEDURE token.
    * @param eos T_EOS token.
    */
   public abstract void
   procedure_stmt(Token label, Token module, Token procedureKeyword, Token eos);

   /** R1207
    * generic_spec
    *   :   T_IDENT
    *   |   T_OPERATOR T_LPAREN defined_operator T_RPAREN
    *   |   T_ASSIGNMENT T_LPAREN T_EQUALS T_RPAREN
    *   |   dtio_generic_spec
    *
    * @param keyword OPERATOR or ASSIGNMENT keyword (null otherwise).
    * @param name The name of the spec (null unless first option T_IDENT).
    * @param type Type of generic-spec.
    */
   public abstract void generic_spec(Token keyword, Token name, int type);

   /** R1208
    * dtio_generic_spec
    *   :   T_READ T_LPAREN T_FORMATTED T_RPAREN
    *   |   T_READ T_LPAREN T_UNFORMATTED T_RPAREN
    *   |   T_WRITE T_LPAREN T_FORMATTED T_RPAREN
    *   |   T_WRITE T_LPAREN T_UNFORMATTED T_RPAREN
    *
    * @param rw Read or write token.
    * @param format Formatted or unformatted token.
    * @param type Type of dtio-generic-spec.
    */
   public abstract void dtio_generic_spec(Token rw, Token format, int type);

   /** R1209
    * import_stmt
    *   :   (label)? T_IMPORT ( ( T_COLON_COLON )? generic_name_list)? T_EOS
    *
    * @param label The label.
    * @param importKeyword T_IMPORT token.
    * @param eos T_EOS token.
    * @param hasGenericNameList True if has generic name list.
    */
   public abstract void
   import_stmt(Token label, Token importKeyword, Token eos, boolean hasGenericNameList);

   /** R1210
    * external_stmt
    *   :   (label)? T_EXTERNAL ( T_COLON_COLON )? generic_name_list T_EOS
    *
    * @param label The label.
    * @param externalKeyword T_EXTERNAL token.
    * @param eos T_EOS token.
    */
   public abstract void
   external_stmt(Token label, Token externalKeyword, Token eos);

   /** R1211
    * procedure_declaration_stmt
    *
    * @param label The label.
    * @param procedureKeyword T_PROCEDURE token.
    * @param eos T_EOS token.
    * @param hasProcInterface True if has a procedure interface.
    * @param count Number of procedure attribute specifications.
    */
   public abstract void
   procedure_declaration_stmt(Token label, Token procedureKeyword, 
			      Token eos, boolean hasProcInterface, int count);

   /** R1212
    * proc_interface
    *   :   T_IDENT
    *   |   declaration_type_spec
    *
    * @param id The interface name.
    */
   public abstract void proc_interface(Token id);

   /** R1213
    * proc_attr_spec
    *   :   access_spec
    *   |   proc_language_binding_spec
    *   |   T_INTENT T_LPAREN intent_spec T_RPAREN
    *   |   T_OPTIONAL
    *   |   T_POINTER
    *   |   T_SAVE
    *
    * @param attrKeyword Token for the attribute keyword if given, or null.
    * @param id T_IDENT token for the T_PASS alternative; null otherwise.
    * @param spec The procedure attribute specification.
    */
   public abstract void proc_attr_spec(Token attrKeyword, Token id, int spec);

   /** R1214
    * proc_decl
    *   :   T_IDENT ( T_EQ_GT null_init {hasNullInit=true;} )?
    *    
    * @param id The name of the procedure.
    * @param hasNullInit True if null-init is present.
    */
   public abstract void proc_decl(Token id, boolean hasNullInit);
	   
   /** R1214 list
    * proc_decl_list
    *   :   proc_decl ( T_COMMA proc_decl )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void proc_decl_list__begin();
   public abstract void proc_decl_list(int count);

   /** R1216
    * intrinsic_stmt
    * 	(label)? T_INTRINSIC ( T_COLON_COLON )?  generic_name_list T_EOS
    *
    * @param label The label.
    * @param intrinsicKeyword T_INSTRINSIC token.
    * @param eos T_EOS token.
    */
   public abstract void
   intrinsic_stmt(Token label, Token intrinsicToken, Token eos);

   /** R1217
    * function_reference
    *   :   procedure-designator LPAREN (actual_arg_spec_list)* RPAREN
    * 
    * Called from designator_or_proc_ref to reduce ambiguities.
    * procedure-designator is replaced by data-ref thus
    * function-reference may also be matched in data-ref as an
    * array-ref, i.e., foo(1) looks like an array
    * 
    * @param hasActualArgSpecList True if an actual-arg-spec-list is present
    */
   public abstract void function_reference(boolean hasActualArgSpecList);

   /** R1218
    * call_stmt
    *   :   (label)? T_CALL procedure_designator
    *                ( T_LPAREN (actual_arg_spec_list)? T_RPAREN )? T_EOS
    * 
    * @param label Optional statement label
    * @param callKeyword T_CALL token.
    * @param eos T_EOS token.
    * @param hasActionArgSpecList True if an actual-arg-spec-list is present
    */
   public abstract void
   call_stmt(Token label, Token callKeyword, Token eos, boolean hasActualArgSpecList);

   /** R1219
    * procedure_designator
    */
   public abstract void procedure_designator();
   
   /** R1220
    * actual_arg_spec
    *   :   (T_IDENT T_EQUALS)? actual_arg
    *
    * R619, section_subscript has been combined with actual_arg_spec (R1220) 
    * to reduce backtracking thus R619 is called from R1220.
    * 
    * @param keyword The keyword is the name of the dummy argument in
    * the explicit interface of the procedure.
    */
   public abstract void actual_arg_spec(Token keyword);

   /** R1220 list
    * actual_arg_spec_list
    * 	:    actual_arg_spec ( T_COMMA actual_arg_spec )*
    *
    * List begin may be called incorrectly from substring_range_or_arg_list. 
    * This will be noted by a count of less than zero.
    *
    * @param count The number of items in the list.  If count is less than 
    * zero, clean up the effects of list begin (as if it had not been called).
    */
   public abstract void actual_arg_spec_list__begin();
   public abstract void actual_arg_spec_list(int count);

   /** R1221
    * actual_arg
    *   :   expr
    *   |   T_ASTERISK label
    *
    * ERR_CHK 1221 ensure ( expr | designator ending in T_PERCENT T_IDENT)
    * T_IDENT inlined for procedure_name
    * 
    * @param hasExpr True if actual-arg is an expression.
    * @param label The label of the alt-return-spec (if not null).
    */
   public abstract void actual_arg(boolean hasExpr, Token label);

   /** R1223
    * function_subprogram
    * 
    * @param hasExePart Flag specifying if optional execution_part was given.
    * @param hasIntSubProg Flag specifying if optional internal_subprogram_part
    * was given.
    */
   public abstract void
   function_subprogram(boolean hasExePart, boolean hasIntSubProg);

   /** R1224
    * function_stmt
    *
    * : (label)? T_FUNCTION T_IDENT
    * 		T_LPAREN ( generic_name_list )? T_RPAREN ( suffix )? T_EOS
    *
    * @param label The label.
    * @param keyword The ENTRY keyword token.
    * @param name The name of the function.
    * @param eos End of statement token.
    * @param hasGenericNameList True if has a generic name list.
    * @param hasSuffix True if has a suffix.
    */
   public abstract void function_stmt__begin();
   public abstract void function_stmt(Token label, Token keyword, Token name, 
                                      Token eos, boolean hasGenericNameList, 
                                      boolean hasSuffix);

   /** R1225
    * proc_language_binding_spec
    *
    */
   public abstract void proc_language_binding_spec();

   /** R1227
    * prefix
    *   :   prefix_spec ( prefix_spec (prefix_spec)? )?
    *
    * @param specCount  Number of specs.
    */
   public abstract void prefix(int specCount);

   /** R1227
    * t_prefix
    *   :   t_prefix_spec (t_prefix_spec)?
    *
    * @param specCount  Number of specifiers.
    */
   public abstract void t_prefix(int specCount);

   /** R1228
    * prefix_spec
    *   :   declaration_type_spec
    *   |   t_prefix_spec
    *
    * @param isDecTypeSpec  True if is a declaration type spec. False if pure,
    * elemental, or recursive.
    */
   public abstract void prefix_spec(boolean isDecTypeSpec);

   /** R1228
    * t_prefix_spec
    *   :   T_RECURSIVE
    *   |   T_PURE
    *   |   T_ELEMENTAL	
    *
    * @param spec  The actual token (pure, elemental, or recursive).
    */
   public abstract void t_prefix_spec(Token spec);

   /** R1229
    * suffix
    * 
    * @param resultKeyword T_RESULT token if given; null otherwise.
    * @param hasProcLangBindSpec Flag specifying whether a 
    * proc-language-binding-spec was given.
    */
   public abstract void
   suffix(Token resultKeyword, boolean hasProcLangBindSpec);
	
   /**
    * Unknown rule.
    * result_name
    *
    */
   public abstract void result_name();

   /** R1230
    * end_function_stmt
    *
    * : (label)? T_END T_FUNCTION ( T_IDENT )? end_of_stmt
    * | (label)? T_ENDFUNCTION    ( T_IDENT )? end_of_stmt
    * | (label)? T_END end_of_stmt
    *
    * @param label The label.
    * @param keyword1 The END or ENDFUNCTION keyword token.
    * @param keyword2 The FUNCTION keyword token (may be null).
    * @param name The name of the function.
    * @param eos End of statement token.
    * @param id The identifier, if present. Otherwise null.
    */
   public abstract void
   end_function_stmt(Token label, Token keyword1,
                     Token keyword2, Token name, Token eos);

   /** R1232
    * subroutine_stmt__begin
    */
   public abstract void subroutine_stmt__begin();

   /** R1232
    * subroutine_stmt
    *   :   (label)? (t_prefix )? T_SUBROUTINE T_IDENT
    * 	             ( T_LPAREN ( dummy_arg_list )? T_RPAREN 
    * 		     ( proc_language_binding_spec )? )? T_EOS
    *
    * @param label The label.
    * @param keyword The SUBROUTINE keyword token.
    * @param name The name of the subroutine.
    * @param eos End of statement token.
    * @param hasPrefix True if has a prefix
    * @param hasDummyArgList True if has an argument list.
    * @param hasBindingSpec True is has a binding spec.
    * @param hasArgSpecifier True if has anything between parentheses
    * following statement.
    */
   public abstract void
   subroutine_stmt(Token label, Token keyword, Token name, Token eos,
                   boolean hasPrefix, boolean hasDummyArgList,
                   boolean hasBindingSpec, boolean hasArgSpecifier);
   
   /** R1233
    * dummy_arg
    *   :   T_IDENT | T_ASTERISK
    *
    * @param dummy The dummy argument token.
    */
   public abstract void dummy_arg(Token dummy);

   /** R1233 list
    * dummy_arg_list
    *   :   dummy_arg ( T_COMMA dummy_arg )*
    * 
    * @param count The number of items in the list.
    */
   public abstract void dummy_arg_list__begin();
   public abstract void dummy_arg_list(int count);

   /** R1234
    * end_subroutine_stmt
    *   :   (label)? T_END T_SUBROUTINE ( T_IDENT )? end_of_stmt
    *   |   (label)? T_ENDSUBROUTINE    ( T_IDENT )? end_of_stmt
    *   |   (label)? T_END end_of_stmt
    *
    * @param label The label.
    * @param keyword1 The END or ENDSUBROUTINE keyword token.
    * @param keyword2 The SUBROUTINE keyword token (may be null).
    * @param name The name of the subroutine (may be null).
    * @param eos End of statement token.
    */
   public abstract void
   end_subroutine_stmt(Token label, Token keyword1, 
                       Token keyword2, Token name, Token eos);

   /** R1235
    * entry_stmt
    *   :   (label)? T_ENTRY T_IDENT
    *       ( T_LPAREN ( dummy_arg_list)? T_RPAREN (suffix)? )? T_EOS
    *
    * @param label The label.
    * @param keyword The ENTRY keyword token.
    * @param id T_IDENT for entry name.
    * @param eos End of statement token.
    * @param hasDummyArgList True if has a dummy argument list.
    * @param hasSuffix True if has a suffix.
    */
   public abstract void
   entry_stmt(Token label, Token keyword, Token id,
              Token eos, boolean hasDummyArgList, boolean hasSuffix);

   /** R1236
    * return_stmt
    *   :   (label)? T_RETURN ( expr )? T_EOS
    *
    * @param label The label.
    * @param keyword The RETURN keyword token.
    * @param eos End of statement token.
    * @param hasScalarIntExpr True if there is a scalar in the return; 
    */
   public abstract void return_stmt(Token label, Token keyword, Token eos, 
                                    boolean hasScalarIntExpr);

   /** R1237
    * contains_stmt
    *   :   (label)? T_CONTAINS ( expr )? T_EOS
    *
    * @param label The label.
    * @param keyword The CONTAINS keyword token.
    * @param eos End of statement token.
    */
   public abstract void contains_stmt(Token label, Token keyword, Token eos);

   /** R1237-F08
    * separate-module-subprogram
    *   is   mp-subprogram-stmt          // NEW_TO_F2008
    *           [ specification-part ]
    *           [ execution-part ]
    *           [ internal-subprogram-part ]
    *        end-mp-subprogram
    *
    * @param hasExecutionPart True if there is an execution-part, false otherwise.
    * @param hasInternalSubprogramPart True if there is an internal-subprogram-part.
    */
   public abstract void
   separate_module_subprogram(boolean hasExecutionPart, boolean hasInternalSubprogramPart);

   public abstract void separate_module_subprogram__begin();

   /** R1238-F08
    * mp-subprogram-stmt
    *     is   MODULE PROCEDURE procedure-name
    *
    * @param label The label.
    * @param moduleKeyword The MODULE keyword token.
    * @param procedureKeyword The PROCEDURE keyword token.
    * @param name The name of the procedure.
    * @param eos T_EOS token.
    */
   public abstract void mp_subprogram_stmt(Token label, Token moduleKeyword,
                                           Token procedureKeyword, Token name, Token eos);

   /** R1239-F08
    * end-mp-subprogram-stmt
    *     is END [ PROCEDURE [ procedure-name ] ]
    *
    * @param label The label.
    * @param keyword1 The END or ENDPROCEDURE keyword token.
    * @param keyword2 The PROCEDURE keyword token (may be null).
    * @param name The name of the procedure (may be null).
    * @param eos End of statement token.
    */
   public abstract void end_mp_subprogram_stmt(Token label, Token keyword1, 
                                               Token keyword2, Token name, Token eos);

   /** R1238
    * stmt_function_stmt
    *   :   (label)? T_STMT_FUNCTION T_IDENT T_LPAREN 
    *                ( generic_name_list )? T_RPAREN T_EQUALS expr T_EOS
    *
    * @param label The label.
    * @param functionName The name of the function.
    * @param eos T_EOS token.
    * @param hasGenericNameList True if there is a list in the statement.
    */
   public abstract void stmt_function_stmt(Token label, Token functionName,
                                           Token eos, boolean hasGenericNameList);

   /**
    * Inserted rule.
    * end_of_stmt
    * 
    * @param eos T_EOS or EOF token.
    * @param currStreamName Name of the current input stream the parser is
    * processing.
    * @param nextStreamName Name of the stream that the parser will switch to 
    * next, starting with the next statement.  This is null if there is no 
    * new stream.  This variable signals to the action method that an include 
    * statement followed the current statement ended by this end_of_stmt.
    */
   // 	public abstract void end_of_stmt(Token eos, String currStreamName, String nextStreamName);

   public abstract void end_of_stmt(Token eos);
   
   public abstract void start_of_file(String filename, String path);
   public abstract void end_of_file(String filename, String path);
   
   public abstract void cleanUp();

   
   //-------------------------------------------------------------------------
   // RICE CO-ARRAY FORTRAN RULES
   // ---------------------------
   // All Rice's rules and actions will prefixed with "rice_" keyword
   //-------------------------------------------------------------------------

   /**
    * rice_image_selector
    * 
    * @param idTeam The token for the id of the team.
    */
   public abstract void rice_image_selector(Token idTeam);

   /**
    * rice_co_dereference_op
    * 
    * @param lbracket The token for the left bracket of the operator.
    * @param rbracket The token for the right bracket of the operator.
    */
   public abstract void rice_co_dereference_op(Token lbracket, Token rbracket);

   /**
    * rice_allocate_coarray_spec
    *
    * @param selection 0 (T_ASTERISK) or 1 (T_AT T_IDENT) or -1
    * @param id AT identifier
    */
   public abstract void rice_allocate_coarray_spec(int selection, Token id);

   /**
    * rice_co_with_team_stmt
    *
    * @param label The label.
    * @param id Team identifier.
    */
   public abstract void rice_co_with_team_stmt(Token label, Token id);

   /**
    * rice_end_with_team_stmt
    *
    * @param label The label.
    * @param id Team identifier.
    * @param eos T_EOS token.
    */
   public abstract void rice_end_with_team_stmt(Token label, Token id, Token eos);

   /**
    * rice_finish_stmt
    *   :   (label)?
    *       T_FINISH (T_IDENT)?
    *       T_EOS
    *
    * @param label The label.
    * @param idTeam Team identifier.
    * @param eos T_EOS token.
    */
   public abstract void rice_finish_stmt(Token label, Token idTeam, Token eos);

  /**
    * rice_end_finish_stmt
    *   :   (label)?
    *       T_END T_FINISH
    *       T_EOS
    *
    * @param label The label.
    * @param eos T_EOS token.
    */
   public abstract void rice_end_finish_stmt(Token label, Token eos);

   /** 
    * spawn_stmt
    *   :   (label)?
    *       T_SPAWN procedure_designator // includes actual parameter list and cosubscript, sigh
    *       // ( T_LPAREN (actual_arg_spec_list)? T_RPAREN )?
    *       // ( T_LBRACKET expr T_AT T_IDENT T_RBRACKET)?
    *       T_EOS
    * 
    * @param label Optional statement label.
    * @param spawn T_SPAWN token.
    * @param eos T_EOS token.
    * @param hasEvent True if a notify-event is present
    */
   public abstract void
   rice_spawn_stmt(Token label, Token spawn, Token eos, boolean hasEvent);
   
   /** 
    * next_token
    * This action is not part of the Fortran grammar.  It is used to output the token stream.
    * 
    * @param tk The next token in the token stream
    */
   public abstract void next_token(Token tk);

}
