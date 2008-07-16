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

public class FortranParserActionPrint implements IFortranParserAction {
	private boolean verbose = true;
	private boolean printKeywords = false;
	private int deletedFeature = -1;
	private int generatedRule = -2;
	private int unknownRule = -3;
	private int f08Rule = -4;
	
	FortranParserActionPrint(String[] args, FortranParser parser, 
									 String filename) {
		super();
	}
	
	private void printRuleHeader(int rule, String name) {
		printRuleHeader(rule, name, "");
	}
	
	private void printRuleHeader(int rule, String name, String addendum) {
		boolean printYear = false;
		if (verbose) {
			System.out.print("R");
			if (rule < 1000) {
				System.out.print(" ");
			} else if (rule > 2000) {
				rule = rule/1000;
				printYear = true;
			}
		}
		System.out.print(rule);
		if (printYear) System.out.print("-F2008");
		if (verbose) {
			System.out.print(":" + name + ":");
		} else {
			if (addendum.length() > 0) System.out.print(":" + addendum);
		}
	}
	
	private void printRuleTrailer() {
		System.out.println();
	}
	
	private void printParameter(Object param, String name) {
		System.out.print(" ");
		if (verbose) System.out.print(name + "=");
		System.out.print(param);				
	}
	
	private void printParameter(Token param, String name) {
		System.out.print(" ");
		if (verbose) {
			System.out.print(name + "=");
			System.out.print(param);
		} else {
			if (param != null) System.out.print(param.getText());
			else System.out.print("null");
		}
	}
	
	public void setVerbose(boolean flag) {
		verbose = flag;
	}
	
	public void setPrintKeywords(boolean flag) {
		printKeywords = flag;
	}
	
	public static String toString(int kp) {
	    switch (kp) {
		case 0:		return "none";
		case 1:	return "literal";
		case 2:		return "id";
		}
	    return "";
	}


	/** R102 list
	 * generic_name (xyz-name)
	 * generic_name_list (xyz-list R101)
	 * generic_name_list_part
	 */
	public void generic_name_list_part(Token id) {
		printRuleHeader(102, "generic-name-list-part", "part");
		printParameter(id, "id");
		printRuleTrailer();
	}
	public void generic_name_list__begin() {
		printRuleHeader(102, "generic-name-list__begin", "list-begin");
		printRuleTrailer();
	}
	public void generic_name_list(int count) {
		printRuleHeader(102, "generic-name-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/** R204
	 * specification_part
	 */
	public void specification_part(int numUseStmts, int numImportStmts, 
											 int numDeclConstructs) {
		printRuleHeader(204, "specification-part");
		printParameter(numUseStmts, "numUseStmts");
		printParameter(numImportStmts, "numImportStmts");
		printParameter(numDeclConstructs, "numDeclConstructs");
		printRuleTrailer();
	}

	/**
	 * R205, R206, R207
	 * declaration_construct
	 *
	 */
	public void declaration_construct() {
		printRuleHeader(207, "declaration-construct");
		printRuleTrailer();
	}

	/**
	 * R208
	 * execution_part
	 *
	 */
	public void execution_part() {
		printRuleHeader(208, "execution-part");
		printRuleTrailer();
	}

	/**
	 * R209
	 * execution_part_construct
	 *
	 */
	public void execution_part_construct() {
		printRuleHeader(209, "execution-part-construct");
		printRuleTrailer();
	}

	/** R210
	 * internal_subprogram_part
	 */
	public void internal_subprogram_part(int count) {
		printRuleHeader(210, "internal-subprogram-part");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/**
	 * R211
	 * internal_subprogram
	 *
	 */
	public void internal_subprogram() {
		printRuleHeader(211, "internal-subprogram");
		printRuleTrailer();
	}

	/**
	 * R212
	 *
	 * specification_stmt
	 */
	public void specification_stmt() {
		printRuleHeader(212, "specification-stmt");
		printRuleTrailer();
	}


	/**
	 * R213
	 * executable_construct
	 *
	 */
	public void executable_construct() {
		printRuleHeader(213, "executable-construct");
		printRuleTrailer();
	}


	/**
	 * R214
	 *
	 * action_stmt
	 */
	public void action_stmt() {
		printRuleHeader(214, "action-stmt");
		printRuleTrailer();
	}

	/**
	 * R215
	 * keyword
	 *
	 */
	public void keyword() {
		printRuleHeader(215, "keyword");
		printRuleTrailer();
	}

	/**
	 * R304
	 * name
	 *
	 * @param id T_IDENT token for the name.
	 */
	public void name(Token id) {
		printRuleHeader(304, "name");
		printParameter(id, "name");
		printRuleTrailer();
	}

	/** R305
	 * constant
	 */
	public void constant(Token id) {
		printRuleHeader(305, "constant");
		printParameter(id, "id");
		printRuleTrailer();
	}

	/**
	 * scalar_constant
	 * 
	 */
	public void scalar_constant() {
		printRuleHeader(generatedRule, "scalar-constant");
		printRuleTrailer();
	}

	/**
	 * R306
	 * literal_constant
	 *
	 */
	public void literal_constant() {
		printRuleHeader(306, "literal-constant");
		printRuleTrailer();
	}

	/** R308
	 * int_constant
	 */
	public void int_constant(Token id) {
		printRuleHeader(308, "int-constant");
		printParameter(id, "id");
		printRuleTrailer();
	}

	/** R309
	 * 	char_constant
	 */
	public void char_constant(Token id) {
		printRuleHeader(309, "char-constant");
		printParameter(id, "id");
		printRuleTrailer();
	}

	/**
	 * R310
	 * intrinsic_operator
	 *
	 */
	public void intrinsic_operator() {
		printRuleHeader(310, "intrinsic-operator");
		printRuleTrailer();
	}

	/** R311
	 * defined_operator
	 */
	public void defined_operator(Token definedOp, boolean isExtended) {
		printRuleHeader(311, "defined-operator");
		printParameter(definedOp, "definedOp");
		printParameter(isExtended, "isExtendedIntrinsicOp");
		printRuleTrailer();
	}

	/**
	 * R312
	 * extended_intrinsic_op
	 *
	 */
	public void extended_intrinsic_op() {
		printRuleHeader(312, "extended-intrinsic-op");
		printRuleTrailer();
	}		

	/** R313
	 * label
     */
	public void label(Token lbl) {
		printRuleHeader(313, "label");
		printParameter(lbl, "lbl");
		printRuleTrailer();
	}

	/** R313 list
	 * label
	 * label_list
     */
	public void label_list__begin() {
		printRuleHeader(313, "label-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void label_list(int count) {
		printRuleHeader(313, "label-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/**
	 * R401
	 * type_spec
	 *
	 */
	public void type_spec() {
		printRuleHeader(401, "type-spec");
		printRuleTrailer();
	}

	/** R402
	 * type-param-value
	 */
	public void type_param_value(boolean hasExpr, boolean hasAsterisk, 
										  boolean hasColon) {
		printRuleHeader(402, "type-param-value");
		printParameter(hasExpr, "hasExpr");
		printParameter(hasAsterisk, "hasAsterisk");
		printParameter(hasColon, "hasColon");
		printRuleTrailer();
	}


	/** R403
	 * intrinsic_type_spec
	 */
	public void intrinsic_type_spec(Token keyword1, Token keyword2, int type, 
											  boolean hasKindSelector) {
		printRuleHeader(403, "intrinsic-type-spec");
		printParameter(keyword1, "keyword1");
		printParameter(keyword2, "keyword2");
		printParameter(type, "type");
		printParameter(hasKindSelector, "hasKindSelector");
		printRuleTrailer();
	}

	/** R404
	 * kind_selector
	 */
	public void kind_selector(Token token1, Token token2, 
									  boolean hasExpression) {
		printRuleHeader(404, "kind-selector");
		printParameter(token1, "token1");
		printParameter(token2, "token2");
		printParameter(hasExpression, "hasExpression");
		printRuleTrailer();
	}

	/** R405
	 * signed_int_literal_constant
	 */
	 public void signed_int_literal_constant(Token sign) {
		 printRuleHeader(405, "signed-int-literal-constant");
		 printParameter(sign, "sign");
		 printRuleTrailer(); 
	 }

	/** R406
	 * int_literal_constant
	 */
	 public void int_literal_constant(Token digitString, Token kindParam) {
		 printRuleHeader(406, "int-literal-constant");
		 printParameter(digitString, "digitString");
		 printParameter(kindParam, "kindParam");
		 printRuleTrailer(); 
	 }

	/**
	 * R407
	 * kind_param
	 *
	 * @param kind T_DIGIT_STRING or T_IDENT token which is the kind_param.
	 */
	public void kind_param(Token kind) {
		printRuleHeader(407, "kind-param");
		printRuleTrailer();
	}

	/**
	 * R411
	 * boz_literal_constant
	 *
	 */
	public void boz_literal_constant(Token constant) {
		printRuleHeader(411, "boz-literal-constant");
		printRuleTrailer();
	}

	/** R416
	 * signed_real_literal_constant
	 */
	 public void signed_real_literal_constant(Token sign) {
		 printRuleHeader(416, "signed-real-literal-constant");
		 printParameter(sign, "sign");
		 printRuleTrailer(); 
	 }

	/** R417
	 * real_literal_constant
	 */
	 public void real_literal_constant(Token realConstant, 
                                           Token kindParam) {
		 printRuleHeader(417, "real-literal-constant");
                 printParameter(realConstant, "realConstant");
		 printParameter(kindParam, "kindParam");
		 printRuleTrailer(); 
	 }

	/**
	 * R421
	 * complex_literal_constant
	 *
	 */
	public void complex_literal_constant() {
		printRuleHeader(421, "complex-literal-constant");
		printRuleTrailer();
	}
	 
	/** R422
	 * real_part
	 */
	public void real_part(boolean hasIntConstant, boolean hasRealConstant, 
								 Token id) {
		printRuleHeader(422, "real-part");
		printParameter(hasIntConstant, "hasIntConstant");
		printParameter(hasRealConstant, "hasRealConstant");
		printParameter(id, "id");
		printRuleTrailer();
	}

	/** R423
	 * imag_part
	 */
	public void imag_part(boolean hasIntConstant, boolean hasRealConstant, 
								 Token id) {
		printRuleHeader(423, "imag-part");
		printParameter(hasIntConstant, "hasIntConstant");
		printParameter(hasRealConstant, "hasRealConstant");
		printParameter(id, "id");
		printRuleTrailer();
	}

	/** R424
	 * char-selector
	 *
	 * @param tk1 Either T_KIND or T_LEN if given; otherwise null.
	 * @param tk2 Either T_KIND or T_LEN if given; otherwise null.
	 * @param kindOrLen1 Specifies whether the first kind or len
	 * type-param-value is present
	 * @param kindOrLen2 Specifies whether the second kind or len
	 * type-param-value is present
	 * @param hasAsterisk True if a '*' char-selector is specified
	 */
	public void char_selector(Token tk1, Token tk2, int kindOrLen1, 
									  int kindOrLen2, boolean hasAsterisk) {
		printRuleHeader(424, "char-selector");
		printParameter(kindOrLen1, "kindOrLen1");
		printParameter(kindOrLen2, "kindOrLen2");
		printParameter(hasAsterisk, "hasAsterisk");
		printRuleTrailer();
	}

	/** R425
	 * length-selector
	 *
	 * @param lenKeyword T_LEN token if given; null otherwise.
	 * @param kindOrLen Specifies whether a kind or len
	 * type-param-value is present
	 * @param hasAsterisk True if a '*' char-selector is specified
	 */
	public void length_selector(Token lenKeyword, int kindOrLen, 
										 boolean hasAsterisk) {
		printRuleHeader(425, "length-selector");
		printParameter(kindOrLen, "kindOrLen");
		printParameter(hasAsterisk, "hasAsterisk");
		printRuleTrailer();
	}

	/** R426
	 * char_length
	 */
	public void char_length(boolean hasTypeParamValue) {
		printRuleHeader(426, "char-length");
		printParameter(hasTypeParamValue, "hasTypeParamValue");
		printRuleTrailer();
	}

	/**
	 * scalar_int_literal_constant
	 *
	 */
	public void scalar_int_literal_constant() {
		printRuleHeader(generatedRule, "scalar-int-literal-constant");
		printRuleTrailer();
	}

	/** R427
	 * char_literal_constant
	 */
	public void char_literal_constant(Token digitString, Token id, Token str) {
		printRuleHeader(427, "char-literal-constant");
		printParameter(digitString, "digitString-kind-param");
		printParameter(id, "identifier-kind-param");
		printParameter(str, "str");
		printRuleTrailer();	
	}

	/** R428
	 * logical_literal_constant
	 *
	 * @param logicalValue T_TRUE or T_FALSE token.
	 * @param isTrue True if logical constant is true, false otherwise
	 * @param kindParam The kind parameter
	 */
	public void logical_literal_constant(Token logicalValue, boolean isTrue, 
													 Token kindParam) {
		printRuleHeader(428, "logical-literal-constant");
		printParameter(isTrue, "isTrue");
		printParameter(kindParam, "kindParam");
		printRuleTrailer(); 
	}

	/**
	 * R429
	 * derived_type_def
	 *
	 */
	public void derived_type_def() {
		printRuleHeader(429, "derived-type-def");
		printRuleTrailer();
	}

	/** R429
	 * type_param_or_comp_def_stmt
	 */
	public void type_param_or_comp_def_stmt(Token eos, int type) {
		printRuleHeader(429, "type-param-or-comp-def-stmt");
		printParameter(type, "type");
		printRuleTrailer();
	}

	/**
	 * R429
	 *
	 * type_param_or_comp_def_stmt_list
	 */
	public void type_param_or_comp_def_stmt_list() {
		printRuleHeader(429, "type-param-or-comp-def-stmt-list");
		printRuleTrailer();
	}

	/** R430 
	 * derived_type_stmt
	 *
	 */
	public void derived_type_stmt(Token label, Token keyword, Token id, 
											Token eos, boolean hasTypeAttrSpecList, 
											boolean hasGenericNameList) {
		printRuleHeader(430, "derived-type-stmt");
		if (label != null) printParameter(label, "label");
		if (printKeywords) printParameter(keyword, "keyword");
		printParameter(id, "id");
		if (printKeywords) printParameter(eos, "eos");
		printParameter(hasTypeAttrSpecList, "hasTypeAttrSpecList");
		printParameter(hasGenericNameList, "hasGenericNameList");
		printRuleTrailer();
	}

	/** R431
	 * type_attr_spec
	 */
	public void type_attr_spec(Token keyword, Token id, int type) {
		printRuleHeader(431, "type-attr-spec");
		printParameter(keyword, "keyword");
		printParameter(id, "id");
		printParameter(type, "type");
		printRuleTrailer();
	}

	/** R431 list
	 * type_attr_spec
	 * type_attr_spec_list
	 */
	public void type_attr_spec_list__begin() {
		printRuleHeader(431, "type-attr-spec-list__begin", "list-begin");
		printRuleTrailer();
	}
	public void type_attr_spec_list(int count) {
		printRuleHeader(431, "type-attr-spec-list", "list");
		printParameter(count, "count");
		printRuleTrailer();	
	}

	/**
	 * R432
	 * private_or_sequence
	 *
	 */
	public void private_or_sequence() {
		printRuleHeader(432, "private-or-sequence");
		printRuleTrailer();
	}

	/** R433 
	 * end_type_stmt
	 *
	 */
	public void end_type_stmt(Token label, Token endKeyword, 
									  Token typeKeyword, Token id, Token eos) {
		printRuleHeader(433, "end-type-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(id, "id");
		printRuleTrailer();
	}

	/** R434 
	 * sequence_stmt
	 *
	 */
	public void sequence_stmt(Token label, Token sequenceKeyword, Token eos) {
		printRuleHeader(434, "sequence-stmt");
		if (label!=null) printParameter(label, "label");
		printRuleTrailer();
	}

	/** R436
	 * type_param_decl
	 */
	public void type_param_decl(Token id, boolean hasInit) {
		printRuleHeader(436, "type-param-decl");
		printParameter(id, "id");
		printParameter(hasInit, "hasInit");
		printRuleTrailer();
	}

	/** R436 list
	 * type_param_decl
	 * type_param_decl_list
	 */
	public void type_param_decl_list__begin() {
		printRuleHeader(436, "type-param-decl-list__begin", "list-begin");
		printRuleTrailer();
	}
	public void type_param_decl_list(int count) {
		printRuleHeader(436, "type-param-decl-list", "list");
		printParameter(count, "count");
		printRuleTrailer();	
	}

	/**
	 * R437
	 * type_param_attr_spec
	 *
	 * @param kindOrLen T_IDENT token for T_KIND or T_LEN.
	 */
	public void type_param_attr_spec(Token kindOrLen) {
		printRuleHeader(437, "type-param-attr-spec");
		printRuleTrailer();
	}

	/** R439
	 * component_def_stmt
	 */
	public void component_def_stmt(int type) {
		printRuleHeader(439, "component-def-stmt");
		printParameter(type, "type");
		printRuleTrailer();
	}

	/** R440
	 *	data_component_def_stmt
	 */
	public void data_component_def_stmt(Token label, Token eos, 
													boolean hasSpec) {
		printRuleHeader(440, "data-component-def-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(hasSpec, "hasSpec");
		printRuleTrailer();
	}

	/** R441
	 * component_attr_spec
	 *
	 * @param attrKeyword T_POINTER, T_DIMENSION, T_ALLOCATABLE, T_KIND, or 
	 * T_LEN if given; null otherwise.
	 * @param specType Type of spec in enum form: pointer,
	 * dimension_paren, dimension_bracket, allocable, access_spec,
	 * kind, or len.
	 */
	public void component_attr_spec(Token attrKeyword, int specType) {
		printRuleHeader(441, "component-attr-spec");
		printParameter(specType, "specType");
		printRuleTrailer();
	}

	/** R441 list
	 * component_attr_spec
	 * component_attr_spec_list
	 */
	public void component_attr_spec_list__begin() {
		printRuleHeader(441, "component-attr-spec-list__begin", "list-begin");
		printRuleTrailer();

	}
	public void component_attr_spec_list(int count) {
		printRuleHeader(441, "component-attr-spec-list", "list");
		printParameter(count, "count");
		printRuleTrailer();	
	
	}

	/** R442
	 * component_decl
	 */
	public void component_decl(Token id, boolean hasComponentArraySpec, 
										boolean hasCoArraySpec, boolean hasCharLength, 
										boolean hasComponentInitialization) {
		printRuleHeader(442, "component-decl");
		printParameter(id, "id");
		printParameter(hasComponentArraySpec, "hasComponentArraySpec");
		printParameter(hasCoArraySpec, "hasCoArraySpec");
		printParameter(hasCharLength, "hasCharLength");
		printParameter(hasComponentInitialization, "hasComponentInitialization");
		printRuleTrailer();
	}

	/** R442 list
	 * component_decl
	 * component_decl_list
	 */
	public void component_decl_list__begin() {
		printRuleHeader(442, "component-decl-list__begin", "list-begin");
		printRuleTrailer();
	
	}
	public void component_decl_list(int count) {
		printRuleHeader(442, "component-decl-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/** R443
	 * component_array_spec
	 */
	public void component_array_spec(boolean isExplicit) {
		printRuleHeader(443, "component-array-spec");
		printParameter(isExplicit, "isExplicit");
		printRuleTrailer();
	}

	/** R443 list
	 * deferred_shape_spec_list
	 */
	public void deferred_shape_spec_list__begin() {
		printRuleHeader(443, "deferred-shape-spec-list__begin", "list-begin");
		printRuleTrailer();
	}
	public void deferred_shape_spec_list(int count) {
		printRuleHeader(443, "deferred-shape-spec-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/**
	 * R444
	 * component_initialization
	 *
	 */
	public void component_initialization() {
		printRuleHeader(444, "component-initialization");
		printRuleTrailer();
	}

	/** R445
	 *	proc_component_def_stmt
	 */
	public void proc_component_def_stmt(Token label, Token procedureKeyword, 
													Token eos, boolean hasInterface) {
		printRuleHeader(445, "proc-component-def-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(hasInterface, "hasInterface");
		printRuleTrailer();
	}

	/** R446
	 * proc_component_attr_spec
	 *
	 * @param attrSpecKeyword T_POINTER, T_PASS, or T_NOPASS token if given; 
	 * null otherwise.
	 * @param id Identifier if present in pass. 
	 * @param specType "Enum" to specify type of spec: pointer, pass,
	 * nopass, or access_spec
	 */
	public void proc_component_attr_spec(Token attrSpecKeyword, Token id, 
													 int specType) {
		printRuleHeader(446, "proc-component-attr-spec");
		printParameter(id, "id");
		printParameter(specType, "specType");
		printRuleTrailer();
	}

	/** R446 list
	 * proc_component_attr_spec_list
	 */
	public void proc_component_attr_spec_list__begin() {
		printRuleHeader(446, "proc-component-attr-spec-list__begin", 
			"list-begin");
		printRuleTrailer();
	}
	public void proc_component_attr_spec_list(int count) {
		printRuleHeader(446, "proc-component-attr-spec-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/** R447
	 * private_components_stmt
	 */
	public void private_components_stmt(Token label, Token privateKeyword, 
													Token eos) {
		printRuleHeader(447, "private-components-stmt");
		if (label!=null) printParameter(label, "label");
		printRuleTrailer();
	}

	/** R448
	 * type_bound_procedure_part
	 */
	public void type_bound_procedure_part(int count, boolean hasBindingPrivateStmt) {
		printRuleHeader(448, "type-bound-procedure-part");
		printParameter(count, "count");
		printParameter(hasBindingPrivateStmt, "hasBindingPrivateStmt");
		printRuleTrailer();
	}

	/** R449
	 * binding_private_stmt
	 */
	public void binding_private_stmt(Token label, Token privateKeyword, 
												Token eos) {
		printRuleHeader(447, "binding-private-stmt");
		if (label!=null) printParameter(label, "label");
		printRuleTrailer();
	}

	/** R450
	 * proc_binding_stmt
	 */
	public void proc_binding_stmt(Token label, int type, Token eos) {
		printRuleHeader(450, "proc-binding-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(type, "type");
		printRuleTrailer();
	}

	/** R451
	 * specific_binding
	 *
	 * @param procedureKeyword T_PROCEDURE token.
	 * @param interfaceName Optional interface name.
	 * @param bindingName Required binding name.
	 * @param procedureName Optional procedure name.
	 * @param hasBindingAttributeList True if has a binding-attr-list.
	 */
	public void specific_binding(Token procedureKeyword, Token interfaceName, 
										  Token bindingName, Token procedureName,
										  boolean hasBindingAttrList) {
		printRuleHeader(451, "specific-binding");
		printParameter(interfaceName, "interfaceName");
		printParameter(bindingName, "bindingName");
		printParameter(procedureName, "procedureName");
		printParameter(hasBindingAttrList, "hasBindingAttrList");
		printRuleTrailer();
	}

	/** R452
	 * generic_binding
	 *
	 * @param genericKeyword T_GENERIC token.
	 * @param hasAccessSpec True if has public or private access spec.
	 */
	public void generic_binding(Token genericKeyword, boolean hasAccessSpec) {
		printRuleHeader(452, "generic-binding");
		printParameter(hasAccessSpec, "hasAccessSpec");
		printRuleTrailer();
	}

	/** R453
	 * binding_attr
	 *
	 * @param bindingAttr T_PASS, T_NOPASS, T_NON_OVERRIDABLE, T_DEFERRED, 
	 * or null.
	 * @param attr The binding attribute.
	 * @param id Optional identifier in pass attribute.
	 */
	public void binding_attr(Token bindingAttr, int attr, Token id) {
		printRuleHeader(453, "binding-attr");
		printParameter(attr, "attr");
		printParameter(id, "id");
		printRuleTrailer();	
	}

	/** R453 list
	 * binding_attr_list
	 */
	public void binding_attr_list__begin() {
		printRuleHeader(453, "binding-attr-list__begin", 
			"list-begin");
		printRuleTrailer();
	}
	public void binding_attr_list(int count) {
		printRuleHeader(453, "binding-attr-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/**
	 * R454
	 * final_binding
	 *
	 * @param finalKeyword T_FINAL token.
	 */
	public void final_binding(Token finalKeyword) {
		printRuleHeader(454, "final-binding");
		printRuleTrailer();
	}

	/** R455
	 * derived_type_spec
	 */
	public void derived_type_spec(Token typeName, 
											boolean hasTypeParamSpecList) {
		printRuleHeader(455, "derived-type-spec");
		printParameter(typeName, "typeName");
		printParameter(hasTypeParamSpecList, "hasTypeParamSpecList");
		printRuleTrailer();			
	}

	/** R456
	 * type_param_spec
	 */
	public void type_param_spec(Token keyWord) {
		printRuleHeader(456, "type-param-spec");
		printParameter(keyWord, "keyWord");
		printRuleTrailer();
	}

	/** R456 list
	 * type_param_spec_list
	 */
	public void type_param_spec_list__begin() {
		printRuleHeader(456, "type-param-spec-list__begin", "list-begin");
		printRuleTrailer();
	}
	public void type_param_spec_list(int count) {
		printRuleHeader(456, "type-param-spec-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/**
	 * R457
	 * structure_constructor
	 *
	 * @param id T_IDENT token for strucure.
	 */
	public void structure_constructor(Token id) {
		printRuleHeader(457, "structure-constructor");
		printParameter(id, "id");
		printRuleTrailer();
	}


	/**
	 * R458 
	 * component_spec
	 *
	 * @param id T_IDENT token for "keyword=" part of component-spec.
	 */
	public void component_spec(Token id) {
		printRuleHeader(458, "component-spec");
		printParameter(id, "id");
		printRuleTrailer();
	}

	/** R458 list
	 * component_spec_list
	 */
	public void component_spec_list__begin() {
		printRuleHeader(458, "component-spec-list__begin", "list-begin");
		printRuleTrailer();
	}
	public void component_spec_list(int count) {
		printRuleHeader(458, "component-spec-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/**
	 * R459
	 * component_data_source
	 *
	 */
	public void component_data_source() {
		printRuleHeader(459, "component-data-source");
		printRuleTrailer();
	}

	/** R460
	 * enum_def
	 */
	public void enum_def(int numEls) {
		printRuleHeader(460, "enum-def");
		printParameter(numEls, "numEls");
		printRuleTrailer();
	}

	/** R461
	 * enum_def_stmt
	 */
	public void enum_def_stmt(Token label, Token enumKeyword, 
									  Token bindKeyword, Token id, Token eos) {
		printRuleHeader(461, "enum-def-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(id, "id");
		printRuleTrailer();
	}

	/** R462
	 * enumerator_def_stmt
	 */
	public void enumerator_def_stmt(Token label, Token enumeratorKeyword, 
											  Token eos) {
		printRuleHeader(462, "enumerator-def-stmt");
		if (label!=null) printParameter(label, "label");
		printRuleTrailer();
	}

	/**
	 * R463
	 * enumerator
	 *
	 * @param id T_IDENT for enumerator.
	 * @param hasExpr Boolean specifying if the optional "= expr" was given.
	 */
	public void enumerator(Token id, boolean hasExpr) {
		printRuleHeader(463, "enumerator");
		printParameter(id, "id");
		printParameter(hasExpr, "hasExpr");
		printRuleTrailer();
	}

	/** R463 list
	 * enumerator_list
	 */
	public void enumerator_list__begin() {
		printRuleHeader(463, "enumerator-list__begin", "list-begin");
		printRuleTrailer();
	}
	public void enumerator_list(int count) {
		printRuleHeader(463, "enumerator-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/** R464
	 * end_enum_stmt
	 */
	public void end_enum_stmt(Token label, Token endKeyword, Token enumKeyword, 
									  Token eos) {
		printRuleHeader(464, "end-enum-stmt");
		if (label!=null) printParameter(label, "label");
		printRuleTrailer();
	}

	/** R465
	 * array_constructor
	 */
	public void array_constructor() {
		printRuleHeader(465, "array-constructor");
		printRuleTrailer();
	}

	/**
	 * R466
	 * ac_spec
	 *
	 */
	public void ac_spec() {
		printRuleHeader(466, "ac-spec");
		printRuleTrailer();
	}		

	/**
	 * R469
	 * ac_value
	 *
	 */
	public void ac_value() {
		printRuleHeader(469, "ac-value");
		printRuleTrailer();
	}

	/** R469 list
	 * ac_value_list
	 */
	public void ac_value_list__begin() {
		printRuleHeader(469, "ac-value-list__begin", "list-begin");
		printRuleTrailer();
	}
	public void ac_value_list(int count) {
		printRuleHeader(469, "ac-value-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/** R470
	 * ac_implied_do
	 */
	public void ac_implied_do() {
		printRuleHeader(470, "ac-implied-do");
		printRuleTrailer();
	}

	/** R471
	 * ac_implied_do_control
	 */
	public void ac_implied_do_control(boolean hasStride) {
		printRuleHeader(471, "ac-implied-do-control");
		printParameter(hasStride, "hasStride");
		printRuleTrailer();
	}

	/**
	 * R472
	 * scalar_int_variable
	 *
	 */
	public void scalar_int_variable() {
		printRuleHeader(472, "scalar-int-variable");
		printRuleTrailer();
	}

	/** R501
	 * type_declaration_stmt
	 */
	public void type_declaration_stmt(Token label, int numAttributes, 
												 Token eos) {
		printRuleHeader(501, "type-declaration-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(numAttributes, "numAttributes");
		printRuleTrailer();
	}

	/** R502
	 * declaration_type_spec
	 *	:	intrinsic_type_spec
	 *	|	T_TYPE T_LPAREN	derived_type_spec T_RPAREN
	 *	|	T_CLASS	T_LPAREN derived_type_spec T_RPAREN
	 *	|	T_CLASS T_LPAREN T_ASTERISK T_RPAREN
	 *
	 * @param udtKeyword Token for the T_TYPE or T_CLASS and null for 
	 * intrinsic_type_spec.
	 * @param type The type of declaration-type-spec {INTRINSIC,TYPE,CLASS,
	 * POLYMORPHIC}.
	 */
	public void declaration_type_spec(Token udtKeyword, int type) {
		printRuleHeader(502, "declaration-type-spec");
		printParameter(type, "type");
		printRuleTrailer();
	}

	/** R503
	 * attr_spec
	 * 
	 * @param attrKeyword Token for the keyword of the given attribute.  Will 
	 * be null in the cases of access_sepc and language_binding_spec.
	 */
	public void attr_spec(Token attrKeyword, int attr) {
		printRuleHeader(503, "attr-spec");
		printParameter(attr, "attr");
		printRuleTrailer();
	}

	/** R504, R503-F2008
	 * entity_decl
	 *  : T_IDENT ( T_LPAREN array_spec T_RPAREN )?
	 *            ( T_LBRACKET co_array_spec T_RBRACKET )?
	 *            ( T_ASTERISK char_length )? ( initialization )? 
	 */
	public void entity_decl(Token id) {
		printRuleHeader(504, "entity-decl");
		printParameter(id, "id");
		printRuleTrailer();
	}

	/** R504 list
	 * entity_decl_list
	 * 	:    entity_decl ( T_COMMA entity_decl )*
	 */
	public void entity_decl_list__begin() {
		printRuleHeader(504, "entity-decl-list__begin", "list-begin");
		printRuleTrailer();
	}
	public void entity_decl_list(int count) {
		printRuleHeader(504, "entity-decl-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/** R506
	 * initialization
	 */
	public void initialization(boolean hasExpr, boolean hasNullInit) {
		printRuleHeader(506, "initialization");
		printParameter(hasExpr, "hasExpr");
		printParameter(hasNullInit, "hasNullInit");
		printRuleTrailer();
	}

	/** R507
	 * null_init
	 */
	public void null_init(Token id) {
		printRuleHeader(507, "null-init");
		printParameter(id, "function-reference");
		printRuleTrailer();	
	}

	/** R508
	 * access_spec
	 */
	public void access_spec(Token keyword, int type) {
		printRuleHeader(508, "access-spec");
		printParameter(keyword, "keyword");
		printParameter(type, "type");
		printRuleTrailer();	
	}

	/** R509
	 * language_binding_spec 
	 */
	public void language_binding_spec(Token keyword, Token id, 
												 boolean hasName) {
		printRuleHeader(509, "language-binding-spec");
		if (printKeywords) printParameter(keyword, "keyword");
		printParameter(id, "language");
		printParameter(hasName, "hasName");
		printRuleTrailer();	
	}

	/** R510
	 * array_spec
	 */
	public void array_spec(int count) {
		printRuleHeader(510, "array-spec");
		printParameter(count, "count");
		printRuleTrailer();
	}
	public void array_spec_element(int type) {
		printRuleHeader(510, "array-spec-element");
		printParameter(type, "type");
		printRuleTrailer();
	}

	/** R511 list
	 * explicit_shape_spec
	 * explicit_shape_spec_list
	 */
	public void explicit_shape_spec(boolean hasUpperBound) {
		printRuleHeader(511, "explicit-shape-spec");
		printParameter(hasUpperBound, "hasUpperBound");
		printRuleTrailer();
	}
	public void explicit_shape_spec_list__begin() {
		printRuleHeader(511, "explicit-shape-spec-list__begin", "list-begin");
		printRuleTrailer();
	}
	public void explicit_shape_spec_list(int count) {
		printRuleHeader(511, "explicit-shape-spec-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/**
	 * R511-F2008
	 * co_array_spec
	 *
	 */
	public void co_array_spec() {
		printRuleHeader(511, "co-array-spec");
		printRuleTrailer();
	}

	/** R517
	 * intent_spec
	 * @param intentKeyword1 The first of two possible intent keyword tokens
	 * (e.g., T_IN, T_OUT, T_INOUT).
	 * @param intentKeyword2 The second of two possible intent keyword tokens.  
	 * This token can ONLY be T_OUT in the case of "intent(in out)", and must 
	 * be null for all other intents.
	 */
	public void intent_spec(Token intentKeyword1, Token intentKeyword2, 
									int intent) {
		printRuleHeader(517, "intent-spec");
		printParameter(intent, "intent");
		printRuleTrailer();
	}

	/**
	 * R519-F2008
	 * deferred_co_shape_spec
	 *
	 */
	public void deferred_co_shape_spec() {
		printRuleHeader(519, "deferred-co-shape-spec");
		printRuleTrailer();
	}		

	/** R519-F2008 list
	 * deferred_co_space_spec_list
	 */
	public void deferred_co_shape_spec_list__begin() {
		printRuleHeader(519, "deferred-co-shape-spec-list__begin", 
			"list-begin-F2008");
		printRuleTrailer();
	}
	public void deferred_co_shape_spec_list(int count) {
		printRuleHeader(519, "deferred-co-shape-spec-list", "list-F2008");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/**
	 * R520-F2008
	 * explicit_co_shape_spec
	 *
	 */
	public void explicit_co_shape_spec() {
		printRuleHeader(520, "explicit-co-shape-spec");
		printRuleTrailer();
	}

	/**
	 * explicit_co_shape_spec_suffix
	 *
	 */
	public void explicit_co_shape_spec_suffix() {
		printRuleHeader(unknownRule, "explicit-co-shape-spec-suffix");
		printRuleTrailer();
	}

	/** R518
	 * access_stmt
	 */
	public void access_stmt(Token label, Token eos, boolean hasList) {
		printRuleHeader(447, "access-stmt");
		if (label != null) printParameter(label, "label");
		if (printKeywords) printParameter(eos, "eos");
		printParameter(hasList, "has-access-id-list");
		printRuleTrailer();
	}

	/**
	 * R519
	 * access_id
	 *
	 */
	public void access_id() {
		printRuleHeader(519, "access-id");
		printRuleTrailer();
	}

	/** R519 list
	 * access_id_list
     */
	public void access_id_list__begin() {
		printRuleHeader(519, "access-id-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void access_id_list(int count) {
		printRuleHeader(519, "access-id-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/** R520
	 * allocatable_stmt
	 */
	public void allocatable_stmt(Token label, Token keyword, Token eos, 
										  int count) {
		printRuleHeader(520, "allocatable-stmt");
		if (label != null) printParameter(label, "label");
		if (printKeywords) printParameter(keyword, "keyword");
		if (printKeywords) printParameter(eos, "eos");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/** R527-F2008
	 * allocatable_decl
	 */
	public void allocatable_decl(Token id, boolean hasArraySpec, 
										  boolean hasCoArraySpec) {
		printRuleHeader(527008, "allocatable-decl");
		printParameter(id, "object_name");
		printParameter(hasArraySpec, "hasArraySpec");
		printParameter(hasCoArraySpec, "hasCoArraySpec");
		printRuleTrailer();
	}
	
	/** R521
	 * asynchronous_stmt
	 */
	public void asynchronous_stmt(Token label, Token keyword, Token eos) {
		printRuleHeader(447, "asynchronous-stmt");
		if (label != null) printParameter(label, "label");
		if (printKeywords) printParameter(keyword, "keyword");
		if (printKeywords) printParameter(eos, "eos");
		printRuleTrailer();
	}

	/** R522
	 * bind_stmt
     */
	public void bind_stmt(Token label, Token eos) {
		printRuleHeader(522, "bind-stmt");
		printParameter(label, "label");
		if (printKeywords) printParameter(eos, "eos");
		printRuleTrailer();
	}

	/** R523 list
	 * bind_entity
	 * bind_entity_list
     */
	public void bind_entity(Token entity, boolean isCommonBlockName) {
		printRuleHeader(523, "bind-entity");
		printParameter(entity, "entity");
		printParameter(isCommonBlockName, "isCommonBlockName");
		printRuleTrailer();
	}
	public void bind_entity_list__begin() {
		printRuleHeader(523, "bind-entity-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void bind_entity_list(int count) {
		printRuleHeader(523, "bind-entity-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/** R524
	 * data_stmt
     */
	public void data_stmt(Token label, Token keyword, Token eos, int count) {
		printRuleHeader(524, "data-stmt");
		if (label != null) printParameter(label, "label");
		if (printKeywords) printParameter(keyword, "keyword");
		if (printKeywords) printParameter(eos, "eos");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/**
	 * R525
	 * data_stmt_set
	 *
	 */
	public void data_stmt_set() {
		printRuleHeader(525, "data-stmt-set");
		printRuleTrailer();
	}

	/**
	 * R526
	 * data_stmt_object
	 *
	 */
	public void data_stmt_object() {
		printRuleHeader(526, "data-stmt-object");
		printRuleTrailer();
	}

	/** R526 list
	 * data_stmt_object_list
     */
	public void data_stmt_object_list__begin() {
		printRuleHeader(526, "data-stmt-object-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void data_stmt_object_list(int count) {
		printRuleHeader(526, "data-stmt-object-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/**
	 * R527
	 * data_implied_do
	 *
	 * @param id T_IDENT token.
	 * @param hasThirdExpr Flag to specify if optional third expression was 
	 * given.  True if expression given; false if not.
	 */
	public void data_implied_do(Token id, boolean hasThirdExpr) {
		printRuleHeader(527, "data-implied-do");
		printParameter(hasThirdExpr, "hasThirdExpr");
		printRuleTrailer();
	}

	/**
	 * R528
	 * data_i_do_object
	 */
	public void data_i_do_object() {
		printRuleHeader(528, "data-i-do-object");
		printRuleTrailer();
	}

	/** R528 list
	 * data_i_do_object_list
     */
	public void data_i_do_object_list__begin() {
		printRuleHeader(528, "data-i-do-object-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void data_i_do_object_list(int count) {
		printRuleHeader(528, "data-i-do-object-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/**
	 * R530
	 * data_stmt_value
	 */
	public void data_stmt_value(Token asterisk) {
		printRuleHeader(530, "data-stmt-value");
		printParameter(asterisk, "asterisk");
		printRuleTrailer();
	}

	/** R530 list
	 * data_stmt_value_list
     */
	public void data_stmt_value_list__begin() {
		printRuleHeader(530, "data-stmt-value-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void data_stmt_value_list(int count) {
		printRuleHeader(530, "data-stmt-value-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/**
	 * R531
	 * scalar_int_constant
	 *
	 */
	public void scalar_int_constant() {
		printRuleHeader(531, "scalar-int-constant");
		printRuleTrailer();
	}

	/**
	 * Generated rule.
	 * hollerith_constant
	 *
	 * @param hollerithConstant T_HOLLERITH token.
	 */
	public void hollerith_constant(Token hollerithConstant) {
		printRuleHeader(454, "final-binding");
		printParameter(hollerithConstant, "hollerithConstant");
		printRuleTrailer();
	}

	/**
	 * R532
	 * data_stmt_constant
	 *
	 */
	public void data_stmt_constant() {
		printRuleHeader(532, "data-stmt-constant");
		printRuleTrailer();
	}

	/** R535
	 * dimension_stmt
     */
	public void dimension_stmt(Token label, Token keyword, Token eos, 
										int count) {
		printRuleHeader(535, "dimension-stmt");
		if (label!=null) printParameter(label, "label");
		if (printKeywords) printParameter(keyword, "keyword");
		if (printKeywords) printParameter(eos, "eos");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/** R544-F2008
	 * dimension_decl
	 */
	public void dimension_decl(Token id, boolean hasArraySpec, 
										boolean hasCoArraySpec) {
		printRuleHeader(544008, "dimension-decl");
		printParameter(id, "id");
		printParameter(hasArraySpec, "hasArraySpec");
		printParameter(hasCoArraySpec, "hasCoArraySpec");
		printRuleTrailer();
	}

	/**
	 * R509-F2008
	 * dimension_spec
	 *
	 * @param dimensionKeyword T_DIMENSION token.
	 */
	public void dimension_spec(Token dimensionKeyword) {
		printRuleHeader(f08Rule, "dimension-spec");
		printParameter(dimensionKeyword, "dimensionKeyword");
		printRuleTrailer();
	}

	/** R536
	 * intent_stmt
     */
	public void intent_stmt(Token label, Token keyword, Token eos) {
		printRuleHeader(536, "intent-stmt");
		if (label!=null) printParameter(label, "label");
		if (printKeywords) printParameter(keyword, "keyword");
		if (printKeywords) printParameter(eos, "eos");
		printRuleTrailer();
	}

	/** R537
	 * optional_stmt
     */
	public void optional_stmt(Token label, Token keyword, Token eos) {
		printRuleHeader(536, "optional-stmt");
		if (label!=null) printParameter(label, "label");
		if (printKeywords) printParameter(keyword, "keyword");
		if (printKeywords) printParameter(eos, "eos");
		printRuleTrailer();
	}

	/** R538
	 * parameter_stmt
     */
	public void parameter_stmt(Token label, Token keyword, Token eos) {
		printRuleHeader(538, "parameter-stmt");
		if (label!=null) printParameter(label, "label");
		if (printKeywords) printParameter(keyword, "keyword");
		if (printKeywords) printParameter(eos, "eos");
		printRuleTrailer();
	}

	/** R539 list
	 * named_constant_def_list
     */
	public void named_constant_def_list__begin() {
		printRuleHeader(539, "named-constant-def-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void named_constant_def_list(int count) {
		printRuleHeader(539, "named-constant-def-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/** R539
	 * named_constant_def
	 */
	public void named_constant_def(Token id) {
		printRuleHeader(539, "named-constant-def");
		printParameter(id, "id");
		printRuleTrailer();
	}

	/** R540
	 * pointer_stmt
     */
	public void pointer_stmt(Token label, Token keyword, Token eos) {
		printRuleHeader(540, "pointer-stmt");
		if (label!=null) printParameter(label, "label");
		if (printKeywords) printParameter(keyword, "keyword");
		if (printKeywords) printParameter(eos, "eos");
		printRuleTrailer();
	}

	/** R541 list
	 * pointer_decl_list
     */
	public void pointer_decl_list__begin() {
		printRuleHeader(541, "pointer-decl-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void pointer_decl_list(int count) {
		printRuleHeader(541, "pointer-decl-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/** R541
	 * pointer_decl
	 */
	public void pointer_decl(Token id, boolean hasSpecList) {
		printRuleHeader(541, "pointer-decl");
		printParameter(id, "id");
		printParameter(hasSpecList, "hasSpecList");
		printRuleTrailer();
	}

	/** R542
	 * protected_stmt
     */
	public void protected_stmt(Token label, Token keyword, Token eos) {
		printRuleHeader(542, "protected-stmt");
		if (label!=null) printParameter(label, "label");
		if (printKeywords) printParameter(keyword, "keyword");
		if (printKeywords) printParameter(eos, "eos");
		printRuleTrailer();
	}

	/** R543
	 * save_stmt
     */
	public void save_stmt(Token label, Token keyword, Token eos, 
								 boolean hasSavedEntityList) {
		printRuleHeader(543, "save-stmt");
		if (label!=null) printParameter(label, "label");
		if (printKeywords) printParameter(keyword, "keyword");
		if (printKeywords) printParameter(eos, "eos");
		printParameter(hasSavedEntityList, "hasSavedEntityList");
		printRuleTrailer();
	}

	/** R544 list
	 * saved_entity_list
     */
	public void saved_entity_list__begin() {
		printRuleHeader(544, "saved-entity-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void saved_entity_list(int count) {
		printRuleHeader(544, "saved-entity-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/** R544
	 * saved_entity
	 */
	public void saved_entity(Token id, boolean isCommonBlockName) {
		printRuleHeader(544, "saved-entity");
		printParameter(id, "id");
		printParameter(isCommonBlockName, "isCommonBlockName");
		printRuleTrailer();
	}

	/** R546
	 * target_stmt
     */
	public void target_stmt(Token label, Token keyword, Token eos, int count) {
		printRuleHeader(546, "target-stmt");
		if (label!=null) printParameter(label, "label");
		if (printKeywords) printParameter(keyword, "keyword");
		if (printKeywords) printParameter(eos, "eos");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/** R556-F2008
	 * target_decl
	 */
	public void target_decl(Token id, boolean hasArraySpec, 
									boolean hasCoArraySpec) {
		printRuleHeader(566008, "target-decl");
		printParameter(id, "id");
		printParameter(hasArraySpec, "hasArraySpec");
		printParameter(hasCoArraySpec, "hasCoArraySpec");
		printRuleTrailer();
	}

	/** R547
	 * value_stmt
     */
	public void value_stmt(Token label, Token keyword, Token eos) {
		printRuleHeader(547, "value-stmt");
		if (label != null) printParameter(label, "label");
		if (printKeywords) printParameter(keyword, "keyword");
		if (printKeywords) printParameter(eos, "eos");
		printRuleTrailer();
	}

	/** R548
	 * volatile_stmt
     */
	public void volatile_stmt(Token label, Token keyword, Token eos) {
		printRuleHeader(548, "volatile-stmt");
		if (label != null) printParameter(label, "label");
		if (printKeywords) printParameter(keyword, "keyword");
		if (printKeywords) printParameter(eos, "eos");
		printRuleTrailer();
	}

	/** R549
	 * implicit_stmt
     */
	public void implicit_stmt(Token label, Token implicitKeyword, 
									  Token noneKeyword, Token eos, 
									  boolean hasImplicitSpecList) {
		printRuleHeader(549, "implicit-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(hasImplicitSpecList, "hasImplicitSpecList");
		printRuleTrailer();
	}


	/** 
	 * R550
	 *
	 * implict-spec
	 * 
	 * : declaration-type-spec ( letter-spec-list )
	 *
	 */
	public void implicit_spec() {
		printRuleHeader(550, "implicit-spec");
		printRuleTrailer();
	}


	/** R550 list
	 * implicit_spec_list
     */
	public void implicit_spec_list__begin() {
		printRuleHeader(550, "implicit-spec-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void implicit_spec_list(int count) {
		printRuleHeader(550, "implicit-spec-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/**
	 * R551
	 * 
	 * letter-spec
	 *
	 * : letter [ - letter ]
	 *
	 * @param id1 Token for the required T_IDENT
	 * @param id2 Token for the optional T_IDENT; null if wasn't provided.
	 */
	public void letter_spec(Token id1, Token id2) {
		printRuleHeader(551, "letter-spec");
		printParameter(id1, "id1");
		printParameter(id2, "id2");
		printRuleTrailer();
	}


	/** R551 list
	 * letter_spec_list
     */
	public void letter_spec_list__begin() {
		printRuleHeader(551, "letter-spec-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void letter_spec_list(int count) {
		printRuleHeader(551, "letter-spec-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/** R552
	 * namelist_stmt
     */
	public void namelist_stmt(Token label, Token keyword, Token eos, 
									  int count) {
		printRuleHeader(552, "namelist-stmt", "list-begin");
		if (label != null) printParameter(label, "label");
		if (printKeywords) printParameter(keyword, "keyword");
		if (printKeywords) printParameter(eos, "eos");
		printParameter(count, "count");
		printRuleTrailer();	
	}

	/** R552
	 * namelist_group_name
	 */
	public void namelist_group_name(Token id) {
		printRuleHeader(552, "namelist-group-name");
		printParameter(id, "id");
		printRuleTrailer();
	}

	/** R553
	 * namelist_group_object
	 */
	public void namelist_group_object(Token id) {
		printRuleHeader(553, "namelist-group-object");
		printParameter(id, "id");
		printRuleTrailer();		
	}

	/** R553 list
	 * namelist_group_object_list
     */
	public void namelist_group_object_list__begin() {
		printRuleHeader(553, "namelist-group-object-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void namelist_group_object_list(int count) {
		printRuleHeader(553, "namelist-group-object-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/** R554
	 * equivalence_stmt
     */
	public void equivalence_stmt(Token label, Token equivalenceKeyword, 
										  Token eos) {
		printRuleHeader(554, "equivalence-stmt");
		if (label!=null) printParameter(label, "label");
		printRuleTrailer();
	}

	/**
	 * R555
	 * 
	 * equivalence_set
	 *
	 */
	public void equivalence_set() {
		printRuleHeader(555, "equivalence-set");
		printRuleTrailer();
	}

	/** R555 list
	 * equivalence_set_list
     */
	public void equivalence_set_list__begin() {
		printRuleHeader(555, "equivalence-set-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void equivalence_set_list(int count) {
		printRuleHeader(555, "equivalence-set-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/**
	 * R556
	 *
	 * equivalence_object
	 *
	 */
	public void equivalence_object() {
		printRuleHeader(556, "equivalence-object");
		printRuleTrailer();
	}


	/** R556 list
	 * equivalence_object_list
     */
	public void equivalence_object_list__begin() {
		printRuleHeader(556, "equivalence-object-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void equivalence_object_list(int count) {
		printRuleHeader(556, "equivalence-object-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/** R557
	 * common_stmt
     */
	public void common_stmt(Token label, Token commonKeyword, Token eos, 
									int numBlocks) {
		printRuleHeader(557, "common-stmt", "list-begin");
		if (label!=null) printParameter(label, "label");
		printParameter(numBlocks, "numBlocks");
		printRuleTrailer();	
	}

	/** R557
	 * common_block_name
	 */
	public void common_block_name(Token id) {
		printRuleHeader(557, "common-block-name");
		printParameter(id, "id");
		printRuleTrailer();
	}

	/** R558 list
	 * common_block_object_list
     */
	public void common_block_object_list__begin() {
		printRuleHeader(558, "common-block-object-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void common_block_object_list(int count) {
		printRuleHeader(558, "common-block-object-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/** R558
	 * common_block_object
	 */
	public void common_block_object(Token id, boolean hasShapeSpecList) {
		printRuleHeader(558, "common-block-object");
		printParameter(id, "id");
		printParameter(hasShapeSpecList, "hasShapeSpecList");
		printRuleTrailer();
	}

	/** R601
	 * variable
	 * :	designator
	 */
	public void variable() {
		printRuleHeader(601, "variable");
		printRuleTrailer();
	}

	// R602 variable_name was name inlined as T_IDENT

	/** R603
	 * designator
	 */
	public void designator(boolean hasSubstringRange) {
		printRuleHeader(603, "designator");
		printParameter(hasSubstringRange, "hasSubstringRange");
		printRuleTrailer();
	}

	/**
	 * Unknown rule.
	 * designator_or_func_ref
	 *
	 */
	public void designator_or_func_ref() {
		printRuleHeader(unknownRule, "designator-or-func-ref");
		printRuleTrailer();
	}

	/**
	 * Unknown rule.
	 * substring_range_or_arg_list
	 *
	 */
	public void substring_range_or_arg_list() {
		printRuleHeader(unknownRule, "substring-range-or-arg-list");
		printRuleTrailer();
	}

	/**
	 * Unknown rule.
	 * substr_range_or_arg_list_suffix
	 *
	 */
	public void substr_range_or_arg_list_suffix() {
		printRuleHeader(unknownRule, "substr-range-or-arg-list-suffix");
		printRuleTrailer();
	}

	/**
	 * R604
	 * logical_variable
	 *
	 */
	public void logical_variable() {
		printRuleHeader(604, "logical-variable");
		printRuleTrailer();
	}

	/**
	 * R605
	 * default_logical_variable
	 *
	 */
	public void default_logical_variable() {
		printRuleHeader(605, "default-logical-variable");
		printRuleTrailer();
	}

	/**
	 * Unknown rule.
	 * scalar_default_logical_variable
	 *
	 */
	public void scalar_default_logical_variable() {
		printRuleHeader(unknownRule, "scalar-default-logical-variable");
		printRuleTrailer();
	}
	
	/**
	 * R606
	 * char_variable
	 *
	 */
	public void char_variable() {
		printRuleHeader(606, "char-variable");
		printRuleTrailer();
	}

	/**
	 * R607
	 * default_char_variable
	 *
	 */
	public void default_char_variable() { 
		printRuleHeader(607, "default-char-variable");
		printRuleTrailer();
	}		

	/**
	 * Unknown rule.
	 * scalar_default_char_variable
	 *
	 */
	public void scalar_default_char_variable() {
		printRuleHeader(unknownRule, "scalar-default-char-variable");
		printRuleTrailer();
	}		

	/**
	 * R608
	 * int_variable
	 *
	 */
	public void int_variable() {
		printRuleHeader(608, "int-variable");
		printRuleTrailer();
	}

	/** R609
	 * substring
	 */
	public void substring(boolean hasSubstringRange) {
		printRuleHeader(609, "substring");
		printParameter(hasSubstringRange, "hasSubstringRange");
		printRuleTrailer();		
	}
	 

	/** R611
	 * substring_range
	 *	:	(expr)? T_COLON	(expr)?
	 *
	 * ERR_CHK 611 scalar_int_expr replaced by expr
	 *
	 * @param hasLowerBound True if lower bound is present in a
	 * substring-range (lower_bound:upper_bound).
	 * @param hasUpperBound True if upper bound is present in a
	 * substring-range (lower_bound:upper_bound).
	 */
	public void substring_range(boolean hasLowerBound, boolean hasUpperBound) {
		printRuleHeader(611, "substring-range");
		printParameter(hasLowerBound, "hasLowerBound");
		printParameter(hasUpperBound, "hasUpperBound");
		printRuleTrailer();
	}
	
	/** R612
	 *	data_ref
	 */
	public void data_ref(int numPartRef) {
		printRuleHeader(612, "data-ref");
		printParameter(numPartRef, "numPartRef");
		printRuleTrailer();
	}

	/** R613, R613-F2008
	 * part_ref
	 */
	public void part_ref(Token id, boolean hasSelectionSubscriptList, 
								boolean hasImageSelector) {
		printRuleHeader(613, "part-ref");
		printParameter(id, "id");
		printParameter(hasSelectionSubscriptList, "hasSelectionSubscriptList");
		printParameter(hasImageSelector, "hasImageSelector");
		printRuleTrailer();
	}

	/** R619  (see R1220, actual_arg_spec)
	 * section_subscript/actual_arg_spec
	 */
	public void section_subscript(boolean hasLowerBound, boolean hasUpperBound, 
											boolean hasStride, boolean isAmbiguous) {
		printRuleHeader(619, "section-subscript");
		printParameter(hasLowerBound, "hasLowerBound");
		printParameter(hasUpperBound, "hasUpperBound");
		printParameter(hasStride, "hasStride");
		printParameter(isAmbiguous, "isAmbiguous");
		printRuleTrailer();
	}

	/** R619 list
	 * section_subscript
	 * section_subscript_list
	 */
	public void section_subscript_list__begin() {
		printRuleHeader(619, "section-subscript-list__begin", "list-begin");
		printRuleTrailer();
	}
	public void section_subscript_list(int count) {
		printRuleHeader(619, "section-subscript-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/**
	 * R622
	 * vector_subscript
	 *
	 */
	public void vector_subscript() {
		printRuleHeader(622, "vector-subscript");
		printRuleTrailer();
	}

	/**
	 * R624-F2008
	 * image_selector
	 *
	 * @param exprCount Count of the optional expressions for co-dimensions.
	 */
	public void image_selector(int exprCount) {
		printRuleHeader(f08Rule, "final-binding");
		printRuleTrailer();
	}

	/** R623
	 * allocate_stmt
	 */
	public void allocate_stmt(Token label, Token allocateKeyword, 
									  Token eos, boolean hasTypeSpec, 
									  boolean hasAllocOptList) {
		printRuleHeader(623, "allocate-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(hasTypeSpec, "hasTypeSpec");
		printParameter(hasAllocOptList, "hasAllocOptList");
		printRuleTrailer();
	}

	/** R624
	 * alloc_opt
	 */
	public void alloc_opt(Token allocOpt) {
		printRuleHeader(624, "alloc-opt");
		printParameter(allocOpt, "allocOpt");
		printRuleTrailer();
	}
	
	/** R624 list
	 * alloc_opt_list
     */
	public void alloc_opt_list__begin() {
		printRuleHeader(624, "alloc-opt-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void alloc_opt_list(int count) {
		printRuleHeader(624, "alloc-opt-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/** R628, R631-F2008
	 * allocation
	 */
	public void allocation(boolean hasAllocateShapeSpecList, 
								  boolean hasAllocateCoArraySpec) {
		printRuleHeader(628, "allocation");
		printParameter(hasAllocateShapeSpecList, "hasAllocateShapeSpecList");
		printParameter(hasAllocateCoArraySpec, "hasAllocateCoArraySpec");
		printRuleTrailer();	
	}

	/** R628 list
	 * allocation_list
     */
	public void allocation_list__begin() {
		printRuleHeader(628, "allocation-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void allocation_list(int count) {
		printRuleHeader(628, "allocation-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/**
	 * R629
	 * allocate_object
	 *
	 */
	public void allocate_object() {
		printRuleHeader(629, "allocate-object");
		printRuleTrailer();
	}

	/** R629 list
	 * allocate_object_list
     */
	public void allocate_object_list__begin() {
		printRuleHeader(629, "allocate-object-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void allocate_object_list(int count) {
		printRuleHeader(629, "allocate-object-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/** R630
	 * allocate_shape_spec
	 */
	public void allocate_shape_spec(boolean hasLowerBound, 
											  boolean hasUpperBound) {
		printRuleHeader(630, "allocate-shape-spec");
		printParameter(hasLowerBound, "hasLowerBound");
		printParameter(hasUpperBound, "hasUpperBound");
		printRuleTrailer();	
	}

	/** R630 list
	 * allocate_shape_spec_list
     */
	public void allocate_shape_spec_list__begin() {
		printRuleHeader(630, "allocate-shape-spec-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void allocate_shape_spec_list(int count) {
		printRuleHeader(630, "allocate-shape-spec-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/** R633
	 *	nullify_stmt
	 */
	public void nullify_stmt(Token label, Token nullifyKeyword, Token eos) {
		printRuleHeader(633, "nullify-stmt");
		if (label!=null) printParameter(label, "label");
		printRuleTrailer();
	}

	/**
	 * R634
	 * pointer_object
	 *
	 */
	public void pointer_object() {
		printRuleHeader(634, "pointer-object");
		printRuleTrailer();
	}

	/** R634 list
	 * pointer_object_list
     */
	public void pointer_object_list__begin() {
		printRuleHeader(634, "pointer-object-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void pointer_object_list(int count) {
		printRuleHeader(634, "pointer-object-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/** R635
	 * deallocate_stmt
     */
	public void deallocate_stmt(Token label, Token deallocateKeyword, 
										 Token eos, boolean hasDeallocOptList) {
		printRuleHeader(635, "deallocate-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(hasDeallocOptList, "hasDeallocOptList");
		printRuleTrailer();
	}

	/**
	 * R636
	 * dealloc_opt
	 *
	 * @param id T_IDENT for 'STAT' or 'ERRMSG'.
	 */
	public void dealloc_opt(Token id) {
		printRuleHeader(636, "dealloc-opt");
		printParameter(id, "id");
		printRuleTrailer();
	}		

	/** R636 list
	 * dealloc_opt_list
     */
	public void dealloc_opt_list__begin() {
		printRuleHeader(636, "dealloc-opt-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void dealloc_opt_list(int count) {
		printRuleHeader(636, "dealloc-opt-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/**
	 * R636-F2008
	 * allocate_co_array_spec
	 *
	 */
	public void allocate_co_array_spec() {
		printRuleHeader(f08Rule, "allocate-co-array-spec");
		printRuleTrailer();
	}

	/**
	 * R637-F2008
	 * allocate_co_shape_spec
	 *
	 * @param hasExpr Flag specifying whether optional expression was given or 
	 * not.  True if given; false if not.
	 */
	public void allocate_co_shape_spec(boolean hasExpr) {
		printRuleHeader(f08Rule, "allocate-co-shape-spec");
		printParameter(hasExpr, "hasExpr");
		printRuleTrailer();
	}

	/** R637-F2008 list
	 * allocate_co_shape_spec_list
     */
	public void allocate_co_shape_spec_list__begin() {
		printRuleHeader(637, "allocate-co-shape-spec-list__begin", 
							 "list-begin-F2008");
		printRuleTrailer();	
	}
	public void allocate_co_shape_spec_list(int count) {
		printRuleHeader(637, "allocate-co-shape-spec-list", "list-F2008");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/** R701
	 * primary
	 */
	public void primary() {
		printRuleHeader(701, "primary");
		printRuleTrailer();
	}
	
	/** R702
	 * level_1_expr
	 */
	public void level_1_expr(Token definedUnaryOp) {
		if (definedUnaryOp != null) {
			printRuleHeader(702, "level-1-expr");
			printParameter(definedUnaryOp, "definedUnaryOp");
			printRuleTrailer();
		}
	}

	/**
	 * R703
	 * defined_unary_op
	 *
	 * @param definedOp T_DEFINED_OP token.
	 */
	public void defined_unary_op(Token definedOp) {
		printRuleHeader(703, "defined-unary-op");
		printParameter(definedOp, "definedOp");
		printRuleTrailer();
	}		
	
	/** R704: note, inserted as R704 functionality
	 * power_operand
	 */
	public void power_operand(boolean hasPowerOperand) {
		if (hasPowerOperand) {
			printRuleHeader(704, "power-operand", "power-operand");
			printParameter(hasPowerOperand, "hasPowerOperand");
			printRuleTrailer();
		}
	}

	public void power_operand__power_op(Token powerOp) {
		printRuleHeader(704, "power-operand__power-op", "power-op");
		printParameter(powerOp, "powerOp");
		printRuleTrailer();
	}

	/** R704: note, see power_operand
	 * mult_operand
	 */
	public void mult_operand(int numMultOps) {
		if (numMultOps > 0) {
			printRuleHeader(704, "mult-operand");
			printParameter(numMultOps, "numMultOps");
			printRuleTrailer();
		}
	}

	public void mult_operand__mult_op(Token multOp) {
		printRuleHeader(704, "mult-operand__mult-op", "mult-op");
		printParameter(multOp, "multOp");
		printRuleTrailer();
	}

	/** R705: note, moved leading optionals to mult_operand
	 * add_operand
	 */
	public void add_operand(Token addOp, int numAddOps) {
		if (addOp != null | numAddOps > 0) {
			printRuleHeader(705, "add-operand");
			printParameter(addOp, "addOp");
			printParameter(numAddOps, "numAddOps");
			printRuleTrailer();
		}
	}

	public void add_operand__add_op(Token addOp) {
		printRuleHeader(705, "add-operand__add-op", "add-op");
		printParameter(addOp, "addOp");
		printRuleTrailer();
	}

	/** R706: note, moved leading optionals to add_operand
	 * level_2_expr
	 */
	public void level_2_expr(int numConcatOps) {
		if (numConcatOps > 0) {
			printRuleHeader(706, "level-2-expr");
			printParameter(numConcatOps, "numConcatOps");
			printRuleTrailer();
		}
	}

	/**
	 * R707
	 * power_op
	 *
	 * @param powerKeyword T_POWER token.
	 */
	public void power_op(Token powerKeyword) {
		printRuleHeader(707, "power-op");
		printParameter(powerKeyword, "powerKeyword");
		printRuleTrailer();
	}		

	/**
	 * R708
	 * mult_op
	 *
	 * @param multKeyword T_ASTERISK or T_SLASH token.
	 */
	public void mult_op(Token multKeyword) {
		printRuleHeader(708, "mult-op");
		printParameter(multKeyword, "multKeyword");
		printRuleTrailer();
	}

	/**
	 * R709
	 * add_op
	 *
	 * @param addKeyword T_PLUS or T_MINUS token.
	 */
	public void add_op(Token addKeyword) {
		printRuleHeader(709, "add-op");
		printParameter(addKeyword, "addKeyword");
		printRuleTrailer();
	}		

	/** R710: note, moved leading optional to level_2_expr
	 * level_3_expr
	 */
	public void level_3_expr(Token relOp) {
		if (relOp != null) {
			printRuleHeader(710, "level-3-expr");
			printParameter(relOp, "relOp");
			printRuleTrailer();
		}
	}

	/**
	 * R711
	 * concat_op
	 *
	 * @param concatKeyword T_SLASH_SLASH token.
	 */
	public void concat_op(Token concatKeyword) {
		printRuleHeader(711, "concat-op");
		printRuleTrailer();
	}		

	/**
	 * R713
	 * rel_op
	 *
	 * @param relOp Relational operator token.
	 */
	public void rel_op(Token relOp) {
		printRuleHeader(713, "rel-op");
		printRuleTrailer();
	}

	/** R714
	 * and_operand
	 */
	public void and_operand(boolean hasNotOp, int numAndOps) {
		if (hasNotOp | numAndOps > 0) {
			printRuleHeader(714, "and-operand");
			printParameter(hasNotOp, "hasNotOp");
			printParameter(numAndOps, "numAndOps");
			printRuleTrailer();
		}
	}
	public void and_operand__not_op(boolean hasNotOp) {
		printRuleHeader(714, "and-operand__not-op", "not-op");
		printParameter(hasNotOp, "hasNotOp");
		printRuleTrailer();
	}

	/** R715: note, moved leading optional to or_operand
	 * or_operand
	 */
	public void or_operand(int numOrOps) {
		if (numOrOps > 0) {
			printRuleHeader(715, "or-operand");
			printParameter(numOrOps, "numOrOps");
			printRuleTrailer();
		}
	}

	/** R716: note, moved leading optional to or_operand
	 * equiv_operand
	 */
	public void equiv_operand(int numEquivOps) {
		if (numEquivOps > 0) {
			printRuleHeader(716, "equiv-operand");
			printParameter(numEquivOps, "numEquivOps");
			printRuleTrailer();
		}
	}
	public void equiv_operand__equiv_op(Token equivOp) {
		printRuleHeader(716, "equiv-operand__equiv-op", "equiv-op");
		printParameter(equivOp, "equivOp");
		printRuleTrailer();
	}

	/** R717: note, moved leading optional to equiv_operand
	 * level_5_expr
	 */
	public void level_5_expr(int numDefinedBinaryOps) {
		if (numDefinedBinaryOps > 0) {
			printRuleHeader(717, "level-5-expr");
			printParameter(numDefinedBinaryOps, "numDefinedBinaryOps");
			printRuleTrailer();
		}
	}
	public void level_5_expr__defined_binary_op(Token definedBinaryOp) {
		printRuleHeader(717, "level-5-expr__defined-binary-op", 
							 "defined-binary-op");
		printParameter(definedBinaryOp, "definedBinaryOp");
		printRuleTrailer();
	}

	/**
	 * R718
	 * not_op
	 *
	 * @param notOp T_NOT token.
	 */
	public void not_op(Token notOp) {
		printRuleHeader(718, "not-op");
		printRuleTrailer();
	}		

	/**
	 * R719
	 * and_op
	 *
	 * @param andOp T_AND token.
	 */
	public void and_op(Token andOp) {
		printRuleHeader(719, "and-op");
		printRuleTrailer();
	}

	/**
	 * R720
	 * or_op
	 *
	 * @param orOp T_OR token.
	 */
	public void or_op(Token orOp) {
		printRuleHeader(720, "or-op");
		printRuleTrailer();
	}

	/**
	 * R721
	 * equiv_op
	 *
	 * @param equivOp T_EQV or T_NEQV token.
	 */
	public void equiv_op(Token equivOp) {
		printRuleHeader(721, "equiv-op");
		printRuleTrailer();
	}

	/** R722: note, moved leading optional to level_5_expr
	 */
	public void expr() {
		// TODO (make optional) printRuleHeader(722, "expr");
		// printRuleTrailer();
	}

	/**
	 * R723
	 * defined_binary_op
	 *
	 * @param binaryOp T_DEFINED_OP token.
	 */
	public void defined_binary_op(Token binaryOp) {
		printRuleHeader(723, "defined-binary-op");
		printParameter(binaryOp, "binaryOp");
		printRuleTrailer();
	}

	/** R734
	 *	assignment_stmt 
	 */
	public void assignment_stmt(Token label, Token eos) {
		printRuleHeader(734, "assignment-stmt");
		if (label!=null) printParameter(label, "label");
		printRuleTrailer();
	}

	/** R735
	 * pointer_assignment_stmt
	 */
	public void pointer_assignment_stmt(Token label, Token eos, 
													boolean hasBoundsSpecList, 
													boolean hasBoundsRemappingList) {
		printRuleHeader(735, "pointer-assignment-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(hasBoundsSpecList, "hasBoundsSpecList");
		printParameter(hasBoundsRemappingList, "hasBoundsRemappingList");
		printRuleTrailer();
	}

	/**
	 * R736
	 * data_pointer_object
	 *
	 */
	public void data_pointer_object() {
		printRuleHeader(736, "data-pointer-object");
		printRuleTrailer();
	}

	/**
	 * R737
	 * bounds_spec
	 *
	 */
	public void bounds_spec() {
		printRuleHeader(737, "bounds-spec");
		printRuleTrailer();
	}

	/** R737 list
	 * bounds_spec_list
     */
	public void bounds_spec_list__begin() {
		printRuleHeader(737, "bounds-spec-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void bounds_spec_list(int count) {
		printRuleHeader(737, "bounds-spec-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/**
	 * R738
	 * bounds_remapping
	 *
	 */
	public void bounds_remapping() {
		printRuleHeader(738, "bounds_remapping");
		printRuleTrailer();
	}

	/** R738 list
	 * bounds_remapping_list
     */
	public void bounds_remapping_list__begin() {
		printRuleHeader(738, "bounds-remapping-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void bounds_remapping_list(int count) {
		printRuleHeader(738, "bounds-remapping-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/**
	 * R740
	 * proc_pointer_object
	 *
	 */
	public void proc_pointer_object() {
		printRuleHeader(740, "proc-pointer-object");
		printRuleTrailer();
	}

	/** R743
	 * where_stmt
     */
	public void where_stmt__begin() {
		printRuleHeader(743, "where-stmt__begin", "begin");
		printRuleTrailer();
	}
	public void where_stmt(Token label, Token eos) {
		printRuleHeader(743, "where-stmt");
		if (label!=null) printParameter(label, "label");
		printRuleTrailer();
	}

    /** R744
     * where_construct
     */
    public void where_construct(int numConstructs, boolean hasMaskedElsewhere, 
										  boolean hasElsewhere) {
            printRuleHeader(744, "where-construct");
            printParameter(numConstructs, "numConstructs");
            printParameter(hasMaskedElsewhere, "hasMaskedElsewhere");
            printParameter(hasElsewhere, "hasElsewhere");
            printRuleTrailer();
    }

	/** R745
	 * where_construct_stmt
     */
	public void where_construct_stmt(Token id, Token whereKeyword, Token eos) {
		printRuleHeader(744, "where-construct-stmt");
		if (id != null) printParameter(id, "id");
		printRuleTrailer();
	}

	/**
	 * R746
	 * where_body_construct
	 *
	 */
	public void where_body_construct() {
		printRuleHeader(746, "where-body-construct");
		printRuleTrailer();
	}

	/** R749
	 * masked_elsewhere_stmt
     */
	public void masked_elsewhere_stmt(Token label, Token elseKeyword, 
												 Token whereKeyword, Token id, Token eos) {
		printRuleHeader(749, "masked-elsewhere-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(id, "id");
		printRuleTrailer();
	}

    /** R749 end
     * masked_elsewhere_stmt__end
     */
    public void masked_elsewhere_stmt__end(int numBodyConstructs) {
            printRuleHeader(749, "masked-elsewhere-stmt__end", "end");
            printParameter(numBodyConstructs, "numBodyConstructs");
            printRuleTrailer();
    }

	/** R750
	 * elsewhere_stmt
     */
	public void elsewhere_stmt(Token label, Token elseKeyword, 
										Token whereKeyword, Token id, Token eos) {
		printRuleHeader(750, "elsewhere-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(id, "id");
		printRuleTrailer();
	}

    /** R750 end
     * elsewhere_stmt__end
     */
    public void elsewhere_stmt__end(int numBodyConstructs) {
            printRuleHeader(750, "elsewhere-stmt__end", "end");
            printParameter(numBodyConstructs, "nnumBodyConstructs");
            printRuleTrailer();
    }

	/** R751
	 * end_where_stmt
     */
	public void end_where_stmt(Token label, Token endKeyword, 
										Token whereKeyword, Token id, Token eos) {
		printRuleHeader(751, "end-where-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(id, "id");
		printRuleTrailer();
	}

	/**
	 * R752
	 * forall_construct
	 *
	 */
	public void forall_construct() {
		printRuleHeader(752, "forall-construct");
		printRuleTrailer();
	}

	/** R753
	 * forall_construct_stmt
     */
	public void forall_construct_stmt(Token label, Token id, 
												 Token forallKeyword, Token eos) {
		printRuleHeader(753, "forall-construct-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(id, "id");
		printRuleTrailer();
	}

	/**
	 * R754
	 * forall_header
	 *
	 */
	public void forall_header() {
		printRuleHeader(754, "forall-header");
		printRuleTrailer();
	}

	/** R755
	 * forall_triplet_spec
	 */
	public void forall_triplet_spec(Token id, boolean hasStride) {
		printRuleHeader(755, "forall-triplet-spec");
		printParameter(id, "id");
		printParameter(hasStride, "hasStride");
		printRuleTrailer();
	}

	/** R755 list
	 * forall_triplet_spec_list
     */
	public void forall_triplet_spec_list__begin() {
		printRuleHeader(755, "forall-triplet-spec-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void forall_triplet_spec_list(int count) {
		printRuleHeader(755, "forall-triplet-spec-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/**
	 * R756
	 * forall_body_construct
	 *
	 */
	public void forall_body_construct() {
		printRuleHeader(756, "forall-body-construct");
		printRuleTrailer();
	}

	/** R757
	 * forall_assignment_stmt
	 */
	public void forall_assignment_stmt(boolean isPointerAssignment) {
		printRuleHeader(757, "forall-assignment-stmt");
		printParameter(isPointerAssignment, "isPointerAssignment");
		printRuleTrailer();
	}

	/** R758
	 * end_forall_stmt
     */
	public void end_forall_stmt(Token label, Token endKeyword, 
										 Token forallKeyword, Token id, Token eos) {
		printRuleHeader(758, "end-forall-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(id, "id");
		printRuleTrailer();
	}

	/** R759
	 * forall_stmt
     */
	public void forall_stmt__begin() {
		printRuleHeader(759, "forall-stmt__begin", "begin");
		printRuleTrailer();
	}
	public void forall_stmt(Token label, Token forallKeyword) {
		printRuleHeader(759, "forall-stmt");
		if (label!=null) printParameter(label, "label");
		printRuleTrailer();
	}

	/**
	 * R801
	 * block
	 *
	 */
	public void block() {
		printRuleHeader(801, "block");
		printRuleTrailer();
	}

	/**
	 * R802
	 * if_construct
	 *
	 */
	public void if_construct() {
		printRuleHeader(802, "if-construct");
		printRuleTrailer();
	}

	/** R803
	 * if_then_stmt
     */
	public void if_then_stmt(Token label, Token id, Token ifKeyword, 
									 Token thenKeyword, Token eos) {
		printRuleHeader(803, "if-then-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(id, "id");
		printRuleTrailer();
	}

	/** R804
	 * else_if_stmt
	 */
	public void else_if_stmt(Token label, Token elseKeyword, 
									 Token ifKeyword, Token thenKeyword, Token id, 
									 Token eos) {
		printRuleHeader(804, "else-if-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(id, "id");
		printRuleTrailer();
	}

	/** R805
	 * else_stmt
	 */
	public void else_stmt(Token label, Token elseKeyword, Token id, Token eos) {
		printRuleHeader(805, "else-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(id, "id");
		printRuleTrailer();
	}

	/** R806
	 * end_if_stmt
	 */
	public void end_if_stmt(Token label, Token endKeyword, Token ifKeyword, 
									Token id, Token eos) {
		printRuleHeader(806, "end-if-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(id, "id");
		printRuleTrailer();
	}

	/** R807
	 * if_stmt
	 */
	public void if_stmt__begin() {
		printRuleHeader(807, "if-stmt__begin", "begin");
		printRuleTrailer();
	}
	public void if_stmt(Token label, Token ifKeyword) {
		printRuleHeader(807, "if-stmt");
		if (label!=null) printParameter(label, "label");
		printRuleTrailer();
	}

	/**
	 * R808
	 * case_construct
	 *
	 */
	public void case_construct() {
		printRuleHeader(808, "case-construct");
		printRuleTrailer();
	}

	/** R809
	 * select_case_stmt
	 */
	public void select_case_stmt(Token label, Token id, Token selectKeyword, 
										  Token caseKeyword, Token eos) {
		printRuleHeader(809, "select-case-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(id, "id");
		printRuleTrailer();
	}

	/** R810
	 * case_stmt
	 */
	public void case_stmt(Token label, Token caseKeyword, Token id, Token eos) {
		printRuleHeader(810, "case-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(id, "id");
		printRuleTrailer();
	}

	/** R811
	 * end_select_stmt
	 */
	public void end_select_stmt(Token label, Token endKeyword, 
										 Token selectKeyword, Token id, Token eos) {
		printRuleHeader(811, "end-select-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(id, "id");
		printRuleTrailer();
	}

	/**
	 * R813
	 * case_selector
	 *
	 * @param defaultToken T_DEFAULT token or null.
	 */
	public void case_selector(Token defaultToken) {
		printRuleHeader(813, "case-selector");
		printRuleTrailer();
	}

	/**
	 * R814
	 * case_value_range
	 *
	 */
	public void case_value_range() {
		printRuleHeader(814, "case-value-range");
		printRuleTrailer();
	}

	/** R814 list
	 * case_value_range_list
	 */
	public void case_value_range_list__begin() {
		printRuleHeader(814, "case-value-range-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void case_value_range_list(int count) {
		printRuleHeader(814, "case-value-range-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/**
	 * Unknown rule.
	 * case_value_range_suffix
	 * 
	 */
	public void case_value_range_suffix() {
		printRuleHeader(unknownRule, "case-value-range-suffix");
		printRuleTrailer();
	}

	/**
	 * R815
	 * case_value
	 *
	 */
	public void case_value() {
		printRuleHeader(815, "case-value");
		printRuleTrailer();
	}

	/**
	 * R816
	 * associate_construct
	 *
	 */
	public void associate_construct() {
		printRuleHeader(816, "associate-construct");
		printRuleTrailer();
	}

	/** R817
	 * associate_stmt
	 */
	public void associate_stmt(Token label, Token id, Token associateKeyword, 
										Token eos) {
		printRuleHeader(817, "associate-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(id, "id");
		printRuleTrailer();
	}

	/** R817 list
	 * association_list
	 */
	public void association_list__begin() {
		printRuleHeader(817, "association-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void association_list(int count) {
		printRuleHeader(817, "association-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/**
	 * R818
	 * association
	 *
	 * @param id T_IDENT for associate_name.
	 */
	public void association(Token id) {
		printRuleHeader(818, "association");
		printParameter(id, "associate-name");
		printRuleTrailer();
	}

	/**
	 * R819
	 * selector
	 *
	 */
	public void selector() {
		printRuleHeader(819, "selector");
		printRuleTrailer();
	}

	/** R820
	 * end_associate_stmt
	 */
	public void end_associate_stmt(Token label, Token endKeyword, 
											 Token associateKeyword, Token id, 
											 Token eos) {
		printRuleHeader(820, "end-associate-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(id, "id");
		printRuleTrailer();
	}

	/**
	 * R821
	 * select_type_construct
	 *
	 */
	public void select_type_construct() {
		printRuleHeader(821, "select-type-construct");
		printRuleTrailer();
	}		

	/** R822
	 * select_type_stmt
	 */
	public void select_type_stmt(Token label, Token selectConstructName, 
										  Token associateName, Token eos) {
		printRuleHeader(822, "select-type-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(selectConstructName, "selectConstructName");
		printParameter(associateName, "associateName");
		printRuleTrailer();
	}

	/* This rule is not part of the draft.  */
	public void select_type(Token selectKeyword, Token typeKeyword) {
		printRuleHeader(822, "select-type");
		printRuleTrailer();
	}

	/** R823
	 * type_guard_stmt
	 */
	public void type_guard_stmt(Token label, Token typeKeyword, 
										 Token isOrDefaultKeyword, 
										 Token selectConstructName, Token eos) {
		printRuleHeader(823, "type-guard-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(selectConstructName, "selectConstructName");
		printRuleTrailer();
	}

	/** R824
	 * end_select_type_stmt
	 */
	public void end_select_type_stmt(Token label, Token endKeyword, 
												Token selectKeyword, Token id, Token eos) {
		printRuleHeader(824, "end-select-type-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(id, "id");
		printRuleTrailer();
	}

	/**
	 * R825
	 * do_construct
	 *
	 */
	public void do_construct() {
		printRuleHeader(825, "do-construct");
		printRuleTrailer();
	}

	/**
	 * R826
	 * block_do_construct
	 * 
	 */
	public void block_do_construct() {
		printRuleHeader(826, "block-do-construct");
		printRuleTrailer();
	}

	/** R827
	 * do_stmt
	 */
	public void do_stmt(Token label, Token id, Token doKeyword, 
							  Token digitString, Token eos, boolean hasLoopControl) {
		printRuleHeader(827, "do-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(id, "id");
		printParameter(digitString, "digitString");
		printParameter(hasLoopControl, "hasLoopControl");
		printRuleTrailer();
	}

	/** R828
	 * label_do_stmt
	 */
	public void label_do_stmt(Token label, Token id, Token doKeyword, 
									  Token digitString, Token eos, 
									  boolean hasLoopControl) {
		printRuleHeader(828, "label-do-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(id, "id");
		printParameter(hasLoopControl, "hasLoopControl");
		printRuleTrailer();
	}

	/**
	 * R830
	 * loop_control
	 *
	 * @param whileKeyword T_WHILE or null.
	 * @param hasOptExpr Flag specifying if optional expression was given.  
	 * This only applies for alternative 2 of the rule.
	 */
	public void loop_control(Token whileKeyword, boolean hasOptExpr) {
		printRuleHeader(830, "loop-control");
		printParameter(hasOptExpr, "hasOptExpr");
		printRuleTrailer();
	}		

	/**
	 * R831
	 * do_variable
	 *
	 */
	public void do_variable() {
		printRuleHeader(831, "do-variable");
		printRuleTrailer();
	}

	/**
	 * R833
	 * end_do
	 *
	 */
	public void end_do() {
		printRuleHeader(833, "end-do");
		printRuleTrailer();
	}

	/** R834
	 * end_do_stmt
	 */
	public void end_do_stmt(Token label, Token endKeyword, Token doKeyword, 
									Token id, Token eos) {
		printRuleHeader(834, "end-do-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(id, "id");
		printRuleTrailer();
	}

	/** R838
	 * do_term_action_stmt
	 */
	public void do_term_action_stmt(Token label, Token endKeyword, 
											  Token doKeyword, Token id, Token eos) {
		printRuleHeader(838, "do-term-action-stmt");
		printParameter(label, "label");
		printParameter(id, "id");
		printRuleTrailer();
	}

	/** R843
	 * cycle_stmt
	 */
	public void cycle_stmt(Token label, Token cycleKeyword, Token id, 
								  Token eos) {
		printRuleHeader(843, "cycle-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(id, "do-construct-name");
		printRuleTrailer();
	}

	/** R844
	 * exit_stmt
	 */
	public void exit_stmt(Token label, Token exitKeyword, Token id, Token eos) {
		printRuleHeader(844, "exit-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(id, "do-construct-name");
		printRuleTrailer();
	}

	/** R845
	 * goto_stmt
	 *	:	t_go_to label T_EOS
	 *
	 * @param goKeyword T_GO or T_GOTO token.
	 * @param toKeyword T_TO token, if given, null otherwise.
	 * @param label The branch target statement label
	 * @param eos T_EOS token.
	 */
	public void goto_stmt(Token goKeyword, Token toKeyword, Token label, 
								 Token eos) {
		printRuleHeader(845, "goto-stmt");
		printParameter(label, "label");
		printRuleTrailer();
	}

	/** R846
	 * computed_goto_stmt
	 * 
	 * @param label The branch target statement label
	 * @param goKeyword T_GO or T_GOTO token.
	 * @param toKeyword T_TO token, if given, null otherwise.
	 * @param eos T_EOS token.
	 */
	public void computed_goto_stmt(Token label, Token goKeyword, 
											 Token toKeyword, Token eos) {
		printRuleHeader(846, "computed-goto-stmt");
		if (label!=null) printParameter(label, "label");
		printRuleTrailer();
	}

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
	public void assign_stmt(Token label1, Token assignKeyword, 
									Token label2, Token toKeyword, Token name, 
									Token eos) {
		printRuleHeader(deletedFeature, "assign-stmt");
		if (label1 != null) printParameter(label1, "label1");
		printParameter(label2, "label2");
		printParameter(name, "name");
		printRuleTrailer();
	}

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
	public void assigned_goto_stmt(Token label, Token goKeyword, 
											 Token toKeyword, Token name, 
											 Token eos) {
		printRuleHeader(deletedFeature, "assigned-goto-stmt");
		if(label != null) 
			printParameter(label, "label");
		printParameter(name, "name");
		printRuleTrailer();
	}

	/**
	 * Unknown rule.
	 * stmt_label_list
	 *
	 */
	public void stmt_label_list() {
		printRuleHeader(unknownRule, "stmt-label-list");
		printRuleTrailer();
	}

	/**
	 * pause_stmt
	 *
	 * @param label Optional statement label.
	 * @param pauseKeyword T_PAUSE token.
	 * @param constant T_DIGIT_STRING or null if is a char_literal_constant.
	 * @param eos T_EOS token.
	 * Note: This is a deleted feature.
	 */
	public void pause_stmt(Token label, Token pauseKeyword, 
								  Token constant, Token eos) {
		printRuleHeader(deletedFeature, "pause-stmt");
		if(label != null)
			printParameter(label, "label");
		printParameter(constant, "constant");
		printRuleTrailer();
	}


	/** R847
	 * arithmetic_if_stmt
	 * @param label  Optional statement label
	 * @param ifKeyword T_IF token.
	 * @param label1 The first branch target statement label
	 * @param label2 The second branch target statement label
	 * @param label3 The third branch target statement label
	 * @param eos T_EOS token.
	 */
	public void arithmetic_if_stmt(Token label, Token ifKeyword, Token label1, 
											 Token label2, Token label3, Token eos) {
		printRuleHeader(847, "arithmetic-if-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(label1, "label1");
		printParameter(label2, "label2");
		printParameter(label3, "label3");
		printRuleTrailer();
	}

	/** R848
	 * continue_stmt
	 *	:	(label)? T_CONTINUE
	 * 
	 * @param label  Optional statement label
	 * @param continueKeyword T_CONTINUE token.
	 * @param eos T_EOS token.
	 */
	public void continue_stmt(Token label, Token continueKeyword, Token eos) {
		printRuleHeader(848, "continue-stmt");
		if (label!=null) printParameter(label, "label");
		printRuleTrailer();
	}

	/** R849
	 * stop_stmt
	 * @param label Optional statement label
	 * @param stopKeyword T_STOP token.
	 * @param eos T_EOS token.
	 * @param hasStopCode True if the stop-code is present, false otherwise
	 */
	public void stop_stmt(Token label, Token stopKeyword, Token eos, 
								 boolean hasStopCode) {
		printRuleHeader(849, "stop-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(hasStopCode, "hasStopCode");
		printRuleTrailer();
	}

	/** R850
	 * stop_code
	 */
	public void stop_code(Token digitString) {
		printRuleHeader(850, "stop-code");
		printParameter(digitString, "digitString");
		printRuleTrailer();
	}

	/**
	 * Unknown rule.
	 * scalar_char_constant
	 *
	 */
	public void scalar_char_constant() {
		printRuleHeader(unknownRule, "scalar-char-constant");
		printRuleTrailer();
	}

	/**
	 * R901
	 * io_unit
	 *
	 */
	public void io_unit() {
		printRuleHeader(901, "io-unit");
		printRuleTrailer();
	}

	/**
	 * R902
	 * file_unit_number
	 *
	 */
	public void file_unit_number() {
		printRuleHeader(902, "file-unit-number");
		printRuleTrailer();
	}

	/** R904
	 * open_stmt
	 * @param label The label.
	 * @param openKeyword T_OPEN token.
	 * @param eos T_EOS token.
	 */
	public void open_stmt(Token label, Token openKeyword, Token eos) {
		printRuleHeader(904, "open-stmt");
		if (label!=null) printParameter(label, "label");
		printRuleTrailer();
	}

	/**
	 * R905
	 * connect_spec
	 *
	 * @param id T_IDENT token for second alternative; otherwise null.
	 */
	public void connect_spec(Token id) {
		printRuleHeader(721, "equiv-op");
		if(id != null) printParameter(id, "connect-spec");
		printRuleTrailer();
	}		

	/** R905 list
	 * connect_spec_list
     */
	public void connect_spec_list__begin() {
		printRuleHeader(905, "connect-spec-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void connect_spec_list(int count) {
		printRuleHeader(905, "connect-spec-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/** R908
	 * close_stmt
	 * @param label The label.
	 * @param closeKeyword T_CLOSE token.
	 * @param eos T_EOS token.
	 */
	public void close_stmt(Token label, Token closeKeyword, Token eos) {
		printRuleHeader(908, "close-stmt");
		if (label!=null) printParameter(label, "label");
		printRuleTrailer();
	}

	/**
	 * R909
	 * close_spec
	 *
	 * @param closeSpec T_IDENT for second alternative; null otherwise.
	 */
	public void close_spec(Token closeSpec) {
		printRuleHeader(721, "equiv-op");
		if(closeSpec != null) printParameter(closeSpec, "closeSpec");
		printRuleTrailer();
	}		

	/** R909 list
	 * close_spec_list
     */
	public void close_spec_list__begin() {
		printRuleHeader(909, "close-spec-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void close_spec_list(int count) {
		printRuleHeader(909, "close-spec-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/** R910
	 * read_stmt
	 * @param label The label.
	 * @param readKeyword T_READ token.
	 * @param eos T_EOS token.
	 * @param hasInputItemList True if has an input item list.
	 */
	public void read_stmt(Token label, Token readKeyword, Token eos, 
								 boolean hasInputItemList) {
		printRuleHeader(910, "read-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(hasInputItemList, "hasInputItemList");
		printRuleTrailer();
	}

	/** R911
	 * write_stmt
	 * 
	 * @param label The statement label
	 * @param writeKeyword T_WRITE token.
	 * @param eos T_EOS token.
	 * @param hasOutputItemList True if output-item-list is present
	 */
	public void write_stmt(Token label, Token writeKeyword, Token eos, 
								  boolean hasOutputItemList) {
		printRuleHeader(911, "write-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(hasOutputItemList, "hasOutputItemList");
		printRuleTrailer();
	}

	/** R912
	 * print_stmt
	 * 
	 * @param label The label.
	 * @param printKeyword T_PRINT token.
	 * @param eos T_EOS token.
	 * @param hasOutputItemList True if output-item-list is present
	 */
	public void print_stmt(Token label, Token printKeyword, Token eos, 
								  boolean hasOutputItemList) {
		printRuleHeader(912, "print-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(hasOutputItemList, "hasOutputItemList");
		printRuleTrailer();
	}

	/** R913
	 * io_control_spec
	 */
	 public void io_control_spec(boolean hasExpression, Token keyword, 
										  boolean hasAsterisk) {
			printRuleHeader(913, "io-control-spec-list");
			printParameter(hasExpression, "hasExpression");
			printParameter(keyword, "keyword");
			printParameter(hasAsterisk, "hasAsterisk");
			printRuleTrailer();		 
	 }
	/** R913 list
	 * io_control_spec_list
     */
	public void io_control_spec_list__begin() {
		printRuleHeader(913, "io-control-spec-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void io_control_spec_list(int count) {
		printRuleHeader(913, "io-control-spec-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/**
	 * R914
	 * format
	 *
	 */
	public void format() {
		printRuleHeader(914, "format");
		printRuleTrailer();
	}

	/**
	 * R915
	 * input_item
	 * 
	 */
	public void input_item() {
		printRuleHeader(915, "input-item");
		printRuleTrailer();
	}

	/** R915 list
	 * input_item_list
     */
	public void input_item_list__begin() {
		printRuleHeader(915, "input-item-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void input_item_list(int count) {
		printRuleHeader(915, "input-item-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/**
	 * R916
	 * output_item
	 *
	 */
	public void output_item() {
		printRuleHeader(916, "output-item");
		printRuleTrailer();
	}

	/** R916 list
	 * output_item_list
     */
	public void output_item_list__begin() {
		printRuleHeader(916, "output-item-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void output_item_list(int count) {
		printRuleHeader(916, "output-item-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/**
	 * R917
	 * io_implied_do
	 *
	 */
	public void io_implied_do() {
		printRuleHeader(917, "io-implied-do");
		printRuleTrailer();
	}

	/**
	 * R918
	 * io_implied_do_object
	 *
	 */
	public void io_implied_do_object() {
		printRuleHeader(918, "io-implied-do-object");
		printRuleTrailer();
	}

	/**
	 * R919
	 * io_implied_do_control
	 *
	 */
	public void io_implied_do_control() {
		printRuleHeader(919, "io-implied-do-control");
		printRuleTrailer();
	}

	/**
	 * R920
	 * dtv_type_spec
	 *
	 * @param typeKeyword T_TYPE or T_CLASS token.
	 */
	public void dtv_type_spec(Token typeKeyword) {
		printRuleHeader(920, "dtv-type-spec");
		printParameter(typeKeyword, "typeKeyword");
		printRuleTrailer();
	}

	/** R921
	 * wait_stmt
	 *
	 * @param label The label.
	 * @param waitKeyword T_WAIT token.
	 * @param eos T_EOS token.
	 */
	public void wait_stmt(Token label, Token waitKeyword, Token eos) {
		printRuleHeader(921, "wait-stmt");
		if (label!=null) printParameter(label, "label");
		printRuleTrailer();
	}

	/**
	 * R922
	 * wait_spec
	 * 
	 * @param id T_IDENT or null.
	 */
	public void wait_spec(Token id) {
		printRuleHeader(922, "wait-spec");
		printRuleTrailer();
	}

	/** R922 list
	 * wait_spec_list
     */
	public void wait_spec_list__begin() {
		printRuleHeader(922, "wait-spec-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void wait_spec_list(int count) {
		printRuleHeader(922, "wait-spec-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/** R923
	 * backspace_stmt
	 *
	 * @param label The label.
	 * @param backspaceKeyword T_BACKSPACE token.
	 * @param eos T_EOS token.
	 * @param hasPositionSpecList True if there is a position spec
	 * list. False is there is a file unit number.
	 */
	public void backspace_stmt(Token label, Token backspaceKeyword, 
										Token eos, boolean hasPositionSpecList) {
		printRuleHeader(923, "backspace-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(hasPositionSpecList, "hasPositionSpecList");
		printRuleTrailer();
	}

	/** R924
	 * endfile_stmt
	 *
	 * @param label The label.
	 * @param endKeyword T_END or T_ENDFILE.
	 * @param fileKeyword T_FILE or null.
	 * @param eos T_EOS token.
	 * @param hasPositionSpecList True if there is a position spec
	 * list. False is there is a file unit number.
	 */
	public void endfile_stmt(Token label, Token endKeyword, Token fileKeyword, 
									 Token eos, boolean hasPositionSpecList) {
		printRuleHeader(924, "endfile-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(hasPositionSpecList, "hasPositionSpecList");
		printRuleTrailer();
	}

	/** R925
	 * rewind_stmt
	 *
	 * @param label The label.
	 * @param rewindKeyword T_REWIND token.
	 * @param eos T_EOS token.
	 * @param hasPositionSpecList True if there is a position spec
	 * list. False is there is a file unit number.
	 */
	public void rewind_stmt(Token label, Token rewindKeyword, Token eos, 
									boolean hasPositionSpecList) {
		printRuleHeader(925, "rewind-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(hasPositionSpecList, "hasPositionSpecList");
		printRuleTrailer();
	}

	/**
	 * R926
	 * position_spec
	 *
	 * @param id T_IDENT for the specifier or null.
	 */
	public void position_spec(Token id) {
		printRuleHeader(926, "position-spec");
		printParameter(id, "id");
		printRuleTrailer();
	}

	/** R926 list
	 * position_spec_list
     */
	public void position_spec_list__begin() {
		printRuleHeader(926, "position-spec-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void position_spec_list(int count) {
		printRuleHeader(926, "position-spec-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/**
	 * R928
	 * flush_spec
	 *
	 * @param id T_IDENT for specifier or null.
	 */
	public void flush_spec(Token id) {
		printRuleHeader(928, "flush-spec");
		printParameter(id, "specifier");
		printRuleTrailer();
	}

	/** R928 list
	 * flush_spec_list
	 */
	public void flush_spec_list__begin() {
		printRuleHeader(928, "flush-spec-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void flush_spec_list(int count) {
		printRuleHeader(928, "flush-spec-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/** R927
	 * flush_stmt
	 *
	 * @param label The label.
	 * @param flushKeyword T_FLUSH token.
	 * @param eos T_EOS token.
	 * @param hasFlushSpecList True if there is a flush spec
	 * list. False is there is a file unit number.
	 */
	public void flush_stmt(Token label, Token flushKeyword, Token eos, 
								  boolean hasFlushSpecList) {
		printRuleHeader(927, "flush-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(hasFlushSpecList, "hasFlushSpecList");
		printRuleTrailer();
	}

	/** R929
	 * inquire_stmt
	 *
	 * @param label Optional statement label.
	 * @param inquireKeyword T_INQUIRE token.
	 * @param id T_IDENT token for type 2 inquire statement; null for type 1.
	 * @param eos T_EOS token.
	 * @param isType2 true if is type 2; false otherwise.
	 */
	public void inquire_stmt(Token label, Token inquireKeyword, Token id, 
									 Token eos, boolean isType2) {
		printRuleHeader(929, "inquire-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(isType2, "isType2");
		printRuleTrailer();
	}

	/**
	 * R930
	 * inquire_spec
	 *
	 * @param id T_IDENT for specifier or null.
	 */
	public void inquire_spec(Token id) {
		printRuleHeader(930, "inquire-spec");
		printParameter(id, "specifier");
		printRuleTrailer();
	}

	/** R930 list
	 * inquire_spec_list
     */
	public void inquire_spec_list__begin() {
		printRuleHeader(930, "inquire-spec-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void inquire_spec_list(int count) {
		printRuleHeader(930, "inquire-spec-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/** R1001
	 * format_stmt
	 *
	 * @param label The label.
	 * @param formatKeyword T_FORMAT token.
	 * @param eos T_EOS token.
	 */
	public void format_stmt(Token label, Token formatKeyword, Token eos) {
		printRuleHeader(1001, "format-stmt");
		if (label!=null) printParameter(label, "label");
		printRuleTrailer();
	}

	/** R1002
	 * format_specification
	 */
	public void format_specification(boolean hasFormatItemList) {
		printRuleHeader(1002, "format-specification");
		printParameter(hasFormatItemList, "hasFormatItemList");
		printRuleTrailer();
	}

	/** R1003
	 * format_item
	 */
	public void format_item(Token descOrDigit, boolean hasFormatItemList) {
		printRuleHeader(1003, "format-item");
		printParameter(descOrDigit, "descOrDigit");
		printParameter(hasFormatItemList, "hasFormatItemList");
		printRuleTrailer();
	}

	/** R1003 list
	 * format_item_list
     */
	public void format_item_list__begin() {
		printRuleHeader(1003, "format-item-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void format_item_list(int count) {
		printRuleHeader(1003, "format-item-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/** R1010 list
	 * v_list_part
	 * v_list
     */
	public void v_list_part(Token plus_minus, Token digitString) {
		printRuleHeader(1010, "v-list-part");
		printParameter(plus_minus, "plus_minus");
		printParameter(digitString, "digitString");
		printRuleTrailer();
	}
	public void v_list__begin() {
		printRuleHeader(1010, "v-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void v_list(int count) {
		printRuleHeader(1010, "v-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/** R1101
	 * main-program
	 * :	(label)? ...
	 * 
	 * @param label Optional statement label
	 * @param id Optional program name
	 */
	public void main_program__begin() {
		printRuleHeader(1101, "main-program__begin", "begin");
		printRuleTrailer();
	}

	/** R1101
	 * ext_function_subprogram
	 */
	public void ext_function_subprogram(boolean hasPrefix) {
		printRuleHeader(1101, "ext-function-subprogram");
		printParameter(hasPrefix, "hasPrefix");
		printRuleTrailer();
	}

	public void main_program(boolean hasProgramStmt, boolean hasExecutionPart, 
									 boolean hasInternalSubprogramPart) {
		printRuleHeader(1101, "main-program");
		printParameter(hasProgramStmt, "hasProgramStmt");
		printParameter(hasExecutionPart, "hasExecutionPart");
		printParameter(hasInternalSubprogramPart, "hasInternalSubprogramPart");
		printRuleTrailer();
	}
	
	/** R1102
	 * program_stmt
	 * :	(label)? ...
	 * 
	 * @param label Optional statement label
	 * @param programKeyword T_PROGRAM token.
	 * @param id Optional program name
	 * @param eos T_EOS token.
	 */
	public void program_stmt(Token label, Token programKeyword, Token id, 
									 Token eos) {
		printRuleHeader(1102, "program-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(id, "id");
		printRuleTrailer();
	}

	/** R1103
	 * end_program_stmt
	 * :	(label)? ...
	 * 
	 * @param label Optional statement label
	 * @param endKeyword T_END or T_ENDPROGRAM token.
	 * @param programKeyword T_PROGRAM token if given; null otherwise.
	 * @param id Optional program name
	 * @param eos T_EOS token.
	 */
	public void end_program_stmt(Token label, Token endKeyword, 
										  Token programKeyword, Token id, Token eos) {
		printRuleHeader(1103, "end-program-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(id, "id");
		printRuleTrailer();
	}

	/**
	 * R1104
	 * module
	 * 
	 */
	public void module() {
		printRuleHeader(1104, "module");
		printRuleTrailer();
	}

	/** R1105
	 * module_stmt__begin
	 */
	public void module_stmt__begin() {
		printRuleHeader(1105, "module-stmt__begin", "begin");
		printRuleTrailer();
	}

	/** R1105
	 * module_stmt
	 *
	 * @param label The label.
	 * @param moduleKeyword T_MODULE token.
	 * @param id The identifier, if present, otherwise null.
	 * @param eos T_EOS token.
	 */
	public void module_stmt(Token label, Token moduleKeyword, Token id, 
									Token eos) {
		printRuleHeader(1105, "module-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(id, "id");
		printRuleTrailer();
	}

	/** R1106
	 * end_module_stmt
	 *
	 * @param label The label.
	 * @param endKeyword T_END or T_ENDMODULE token.
	 * @param moduleKeyword T_MODULE token if given; null otherwise.
	 * @param id The identifier, if present, otherwise null.
	 * @param eos T_EOS token.
	 */
	public void end_module_stmt(Token label, Token endKeyword, 
										 Token moduleKeyword, Token id, Token eos) {
		printRuleHeader(1106, "end-module-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(id, "id");
		printRuleTrailer();
	}

	/**
	 * R1107
	 * module_subprogram_part
	 */
	public void module_subprogram_part() {
		printRuleHeader(1107, "module-subprogram-part");
		printRuleTrailer();
	}

	/** R1108
	 * module_subprogram
	 */
	public void module_subprogram(boolean hasPrefix) {
		printRuleHeader(1108, "module-subprogram");
		printParameter(hasPrefix, "hasPrefix");
		printRuleTrailer();
	}

	/** R1109
	 * use_stmt
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
	public void use_stmt(Token label, Token useKeyword, Token id, 
								Token onlyKeyword, Token eos, boolean hasModuleNature,
								boolean hasRenameList, boolean hasOnly) {
		printRuleHeader(1109, "use-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(hasModuleNature, "hasModuleNature");
		printParameter(hasRenameList, "hasRenameList");
		printParameter(hasOnly, "hasOnly");
		printRuleTrailer();
	}

 	/**
	 * R1110
	 * module_nature
	 * 
	 * @param nature T_INTRINSIC or T_NON_INTRINSIC token.
	 */
	public void module_nature(Token nature) {
		printRuleHeader(1110, "module-nature");
		printRuleTrailer();
	}

	/**
	 * R1111 
	 * rename
	 * 
	 * @param id1 First T_IDENT for alt1 or null if alt2.
	 * @param id2 Second T_IDENT for alt1 or null if alt2.
	 * @param op1 First T_OPERATOR for alt2 or null if alt1.
	 * @param defOp1 First T_DEFINED_OP for alt2 or null if alt1.
	 * @param op2 Second T_OPERATOR for alt2 or null if alt1.
	 * @param defOp2 Second T_DEFINED_OP for alt2 or null if alt1.
	 */
	public void rename(Token id1, Token id2, Token op1, Token defOp1, 
							 Token op2, Token defOp2) {
		printRuleHeader(1111, "rename");
		printParameter(id1, "id1");
		printParameter(id2, "id2");
		printParameter(defOp1, "defOp1");
		printParameter(defOp2, "defOp2");
		printRuleTrailer();
	}

	/** R1111 list
	 * rename_list
     */
	public void rename_list__begin() {
		printRuleHeader(1111, "rename-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void rename_list(int count) {
		printRuleHeader(1111, "rename-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/**
	 * R1112
	 * only
	 *
	 */
	public void only() {
		printRuleHeader(1112, "only");
		printRuleTrailer();
	}

	/** R1112 list
	 * only_list
     */
	public void only_list__begin() {
		printRuleHeader(1112, "only-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void only_list(int count) {
		printRuleHeader(1112, "only-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/**
	 * R1116
	 * block_data
	 *
	 */
	public void block_data() {
		printRuleHeader(1116, "block-data");
		printRuleTrailer();
	}

	/** R1117
	 * block_data_stmt__begin
	 */
	public void block_data_stmt__begin() {
		printRuleHeader(1117, "block-data-stmt__begin", "begin");
		printRuleTrailer();
	}

	/** R1117
	 * block_data_stmt
	 *
	 * @param label The label.
	 * @param blockKeyword T_BLOCK or T_BLOCKDATA token.
	 * @param dataKeyword T_DATA token if given; null otherwise.
	 * @param id Identifier if it exists. Otherwise, null.
	 * @param eos T_EOS token.
	 */
	public void block_data_stmt(Token label, Token blockKeyword, 
										 Token dataKeyword, Token id, Token eos) {
		printRuleHeader(1117, "block-data-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(id, "id");
		printRuleTrailer();
	}

	/** R1118
	 * end_block_data_stmt
	 *
	 * @param label The label.
	 * @param endKeyword T_END, T_ENDBLOCK, or T_ENDBLOCKDATA token.
	 * @param blockKeyword T_BLOCK or T_BLOCKDATA token; null otherwise.
	 * @param dataKeyword T_DATA token if given; null otherwise.
	 * @param id Identifier if it exists. Otherwise, null.
	 * @param eos T_EOS token.
	 */
	public void end_block_data_stmt(Token label, Token endKeyword, 
											  Token blockKeyword, Token dataKeyword, 
											  Token id, Token eos) {
		printRuleHeader(1118, "end-block-data-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(id, "id");
		printRuleTrailer();
	}

	/**
	 * R1201
	 * interface_block
	 *
	 */
	public void interface_block() {
		printRuleHeader(1201, "interface-block");
		printRuleTrailer();
	}

	/**
	 * R1202
	 * interface_specification
	 *
	 */
	public void interface_specification() {
		printRuleHeader(1202, "interface-specification");
		printRuleTrailer();
	}

	/** R1203
	 * interface_stmt__begin
	 */
	public void interface_stmt__begin() {
		printRuleHeader(1203, "interface-stmt__begin", "begin");
		printRuleTrailer();
	}

	/** R1203
	 * interface_stmt
	 */
	public void interface_stmt(Token label, Token abstractToken, Token keyword, 
										Token eos, boolean hasGenericSpec) {
		printRuleHeader(1203, "interface-stmt");
		if (label != null) printParameter(label, "label");
		printParameter(abstractToken, "abstract_token");
		if (printKeywords) printParameter(keyword, "keyword");
		if (printKeywords) printParameter(eos, "eos");
		printParameter(hasGenericSpec, "hasGenericSpec");
		printRuleTrailer();
	}

	/** R1204
	 * end_interface_stmt
	 */
	public void end_interface_stmt(Token label, Token kw1, Token kw2, 
											 Token eos, boolean hasGenericSpec) {
		printRuleHeader(1204, "end-interface-stmt");
		if (label != null) printParameter(label, "label");
		if (printKeywords) printParameter(kw1, "keyword1");
		if (printKeywords) printParameter(kw2, "keyword2");
		if (printKeywords) printParameter(eos, "eos");
		printParameter(hasGenericSpec, "hasGenericSpec");
		printRuleTrailer();
	}

	/**
	 * R1205
	 * interface_body
	 * 
	 * @param hasPrefix Boolean flag for whether the optional prefix was given.
	 * This only applies for alt1 (always false for alt2).
	 */
	public void interface_body(boolean hasPrefix) {
		printRuleHeader(1205, "interface-body");
		printParameter(hasPrefix, "hasPrefix");
		printRuleTrailer();
	}

	/** R1206
	 * procedure_stmt
	 *
	 * @param label The label.
	 * @param module Name of the module, if present.
	 * @param procedureKeyword T_PROCEDURE token.
	 * @param eos T_EOS token.
	 */
	public void procedure_stmt(Token label, Token module, 
										Token procedureKeyword, Token eos) {
		printRuleHeader(1206, "procedure-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(module, "module");
		printRuleTrailer();
	}

	/** R1207
	 * generic_spec
	 */
	 public void generic_spec(Token keyword, Token name, int type) {
		printRuleHeader(1207, "generic_spec");
		printParameter(keyword, "keyword");
		printParameter(name, "name");
		printParameter(type, "type");
		printRuleTrailer();
	 }

	/** R1208
	 * dtio_generic_spec
	 */
	public void dtio_generic_spec(Token rw, Token format, int type) {
		printRuleHeader(1208, "dtio_generic_spec");
		printParameter(rw, "rw");
		printParameter(format, "format");
		printParameter(type, "type");
		printRuleTrailer();		
	}

	/** R1209
	 * import_stmt
	 *
	 * @param label The label.
	 * @param importKeyword T_IMPORT token.
	 * @param eos T_EOS token.
	 * @param hasGenericNameList True if has generic name list.
	 */
	public void import_stmt(Token label, Token importKeyword, Token eos, 
									boolean hasGenericNameList) {
		printRuleHeader(1209, "import-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(hasGenericNameList, "hasGenericNameList");
		printRuleTrailer();
	}

	/** R1210
	 * external_stmt
	 *
	 * @param label The label.
	 * @param externalKeyword T_EXTERNAL token.
	 * @param eos T_EOS token.
	 */
	public void external_stmt(Token label, Token externalKeyword, Token eos) {
		printRuleHeader(1210, "external-stmt");
		if (label!=null) printParameter(label, "label");
		printRuleTrailer();
	}

	/** R1211
	 * procedure_declaration_stmt
	 *
	 * @param label The label.
	 * @param procedureKeyword T_PROCEDURE token.
	 * @param eos T_EOS token.
	 * @param hasProcInterface True if has a procedure interface.
	 * @param count Number of procedure attribute specifications.
	 */
	public void procedure_declaration_stmt(Token label, Token procedureKeyword, 
														Token eos, boolean hasProcInterface, 
														int count) {
		printRuleHeader(1211, "procedure-declaration-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(hasProcInterface, "hasProcInterface");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/** R1212
	 * proc_interface
	 */
	public void proc_interface(Token id) {
		printRuleHeader(1212, "proc-interface");
		printParameter(id, "id");
		printRuleTrailer();		
	}

	/** 
	 * R1213
	 * proc_attr_spec
	 *	:	access_spec
	 *	|	proc_language_binding_spec
	 *	|	T_INTENT T_LPAREN intent_spec T_RPAREN
	 *	|	T_OPTIONAL
	 *	|	T_POINTER
	 *	|	T_SAVE
	 *
	 * @param attrKeyword Token for the attribute keyword if given, or null.
	 * @param id T_IDENT token for the T_PASS alternative; null otherwise.
	 * @param spec The procedure attribute specification.
	 */
	public void proc_attr_spec(Token attrKeyword, Token id, int spec) {
		printRuleHeader(1213, "proc-attr-spec");
		printParameter(id, "id");
		printParameter(spec, "spec");
		printRuleTrailer();		
	}

	/** R1214
	 * proc_decl
	 */
	public void proc_decl(Token id, boolean hasNullInit) {
		printRuleHeader(1214, "proc-decl");
		printParameter(id, "id");
		printParameter(hasNullInit, "hasNullInit");
		printRuleTrailer();		
	}

	/** R1214 list
	 * proc_decl_list
     */
	public void proc_decl_list__begin() {
		printRuleHeader(1214, "proc-decl-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void proc_decl_list(int count) {
		printRuleHeader(1214, "proc-decl-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/** R1216
	 * intrinsic_stmt
	 *
	 * @param label The label.
	 * @param intrinsicKeyword T_INSTRINSIC token.
	 * @param eos T_EOS token.
	 */
	public void intrinsic_stmt(Token label, Token intrinsicKeyword, Token eos) {
		printRuleHeader(1216, "intrinsic-stmt");
		if (label!=null) printParameter(label, "label");
		printRuleTrailer();
	}

	/** R1217
	 * function_reference
	 */
	public void function_reference(boolean hasActualArgSpecList) {
		printRuleHeader(1217, "function-reference");
		printParameter(hasActualArgSpecList, "hasActualArgSpecList");
		printRuleTrailer();
	}

	/** R1218
	 * call_stmt
	 * 
	 * @param label Optional statement label
	 * @param callKeyword T_CALL token.
	 * @param eos T_EOS token.
	 * @param hasActionArgSpecList True if an actual-arg-spec-list is present
	 */
	public void call_stmt(Token label, Token callKeyword, Token eos, 
								 boolean hasActualArgSpecList) {
		printRuleHeader(1218, "call-stmt");
		printParameter(hasActualArgSpecList, "hasActualArgSpecList");
		printRuleTrailer();
	}

	/**
	 * R1219
	 * procedure_designator
	 *
	 */
	public void procedure_designator() {
		printRuleHeader(1219, "procedure-designator");
		printRuleTrailer();
	}

	/** R1220
	 * actual_arg_spec
	 */
	public void actual_arg_spec(Token keyword) {
		printRuleHeader(1220, "actual-arg-spec");
		printParameter(keyword, "keyword");
		printRuleTrailer();
	}

	/** R1220 list
	 * actual_arg_spec_list
	 */
	public void actual_arg_spec_list__begin() {
		printRuleHeader(1220, "actual-arg-spec-list__begin", "list-begin");
		printRuleTrailer();
	}
	public void actual_arg_spec_list(int count) {
		printRuleHeader(1220, "actual-arg-spec-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/** R1221
	 * actual_arg
	 */
	public void actual_arg(boolean hasExpr, Token label) {
		printRuleHeader(1221, "actual-arg");
		printParameter(hasExpr, "hasExpr");
		printParameter(label, "label");
		printRuleTrailer();
	}

	/**
	 * R1223
	 * function_subprogram
	 * 
	 * @param hasExePart Flag specifying if optional execution_part was given.
	 * @param hasIntSubProg Flag specifying if optional 
	 * internal_subprogram_part was given.
	 */
	public void function_subprogram(boolean hasExePart, 
											  boolean hasIntSubProg) {
		printRuleHeader(1223, "function-subprogram");
		printParameter(hasExePart, "hasExePart");
		printParameter(hasIntSubProg, "hasIntSubProg");
		printRuleTrailer();
	}

	/** R1224
	 * function_stmt__begin
	 */
	public void function_stmt__begin() {
		printRuleHeader(1224, "function-stmt__begin", "begin");
		printRuleTrailer();
	}

	/** R1224
	 * function_stmt
	 */
	public void function_stmt(Token label, Token keyword, Token name, 
									  Token eos, boolean hasGenericNameList, 
									  boolean hasSuffix) {
		printRuleHeader(1224, "function-stmt");
		if (label != null) printParameter(label, "label");
		if (printKeywords) printParameter(keyword, "keyword");
		printParameter(name, "name");
		if (printKeywords) printParameter(eos, "eos");
		printParameter(hasGenericNameList, "hasGenericNameList");
		printParameter(hasSuffix, "hasSuffix");
		printRuleTrailer();
	}

	/**
	 * R1225
	 * proc_language_binding_spec
	 *
	 */
	public void proc_language_binding_spec() {
		printRuleHeader(1225, "proc-language-binding-spec");
		printRuleTrailer();
	}

	/** R1227
	 * prefix
	 */
	public void prefix(int specCount) {
		printRuleHeader(1227, "prefix");
		printParameter(specCount, "specCount");
		printRuleTrailer();
	}

	/** R1227
	 * t_prefix
	 */
	public void t_prefix(int specCount) {
		printRuleHeader(1227, "t-prefix");
		printParameter(specCount, "specCount");
		printRuleTrailer();
	}

	/** R1228
	 * prefix_spec
	 */
	public void prefix_spec(boolean isDecTypeSpec) {
		printRuleHeader(1228, "prefix-spec");
		printParameter(isDecTypeSpec, "isDecTypeSpec");
		printRuleTrailer();
	}

	/** R1228
	 * t_prefix_spec
	 */
	public void t_prefix_spec(Token spec) {
		printRuleHeader(1228, "t-prefix-spec");
		printParameter(spec, "spec");
		printRuleTrailer();
	}

	/**
	 * R1229
	 * suffix
	 * 
	 * @param resultKeyword T_RESULT token if given; null otherwise.
	 * @param hasProcLangBindSpec Flag specifying whether a 
	 * proc-language-binding-spec was given.
	 */
	public void suffix(Token resultKeyword, boolean hasProcLangBindSpec) {
		printRuleHeader(1229, "suffix");
		printParameter(resultKeyword, "result");
		printParameter(hasProcLangBindSpec, "hasProcLangBindSpec");
		printRuleTrailer();
	}

	/**
	 * Unknown rule.
	 * result_name
	 *
	 */
	public void result_name() {
		printRuleHeader(unknownRule, "result-name");
		printRuleTrailer();
	}

	/** R1230
	 * end_function_stmt
	 */
	public void end_function_stmt(Token label, Token keyword1, Token keyword2, 
											Token name, Token eos) {
		printRuleHeader(1230, "end-function-stmt");
		if (label != null) printParameter(label, "label");
		if (printKeywords) printParameter(keyword1, "keyword1");
		if (printKeywords) printParameter(keyword2, "keyword2");
		printParameter(name, "name");
		if (printKeywords) printParameter(eos, "eos");
		printRuleTrailer();
	}

	/** R1232
	 * subroutine_stmt__begin
	 */
	public void subroutine_stmt__begin() {
		printRuleHeader(1232, "subroutine-stmt__begin","begin");
		printRuleTrailer();
	}

	/** R1232
	 * subroutine_stmt
	 */
	public void subroutine_stmt(Token label, Token keyword, Token name, 
										 Token eos, boolean hasPrefix, 
										 boolean hasDummyArgList, boolean hasBindingSpec,
										 boolean hasArgSpecifier) {
		printRuleHeader(1232, "subroutine-stmt");
		if (label!=null) printParameter(label, "label");
		if (printKeywords) printParameter(keyword, "keyword");
		printParameter(name, "name");
		if (printKeywords) printParameter(eos, "eos");
		printParameter(hasPrefix, "hasPrefix");
		printParameter(hasDummyArgList, "hasDummyArgList");
		printParameter(hasBindingSpec, "hasBindingSpec");
		printParameter(hasArgSpecifier, "hasArgSpecifier");
		printRuleTrailer();
	}

	/** R1233
	 * dummy_arg
	 */
	public void dummy_arg(Token dummy) {
		printRuleHeader(1233, "dummy-arg");
		printParameter(dummy, "dummy");
		printRuleTrailer();
	}

	/** R1233 list
	 * dummy_arg_list
     */
	public void dummy_arg_list__begin() {
		printRuleHeader(1233, "dummy-arg-list__begin", "list-begin");
		printRuleTrailer();	
	}
	public void dummy_arg_list(int count) {
		printRuleHeader(1233, "dummy-arg-list", "list");
		printParameter(count, "count");
		printRuleTrailer();
	}

	/** R1234
	 * end_subroutine_stmt
	 */
	public void end_subroutine_stmt(Token label, Token keyword1, 
											  Token keyword2, Token name, Token eos) {
		printRuleHeader(1234, "end-subroutine-stmt");
		if (label != null) printParameter(label, "label");
		if (printKeywords) printParameter(keyword1, "keyword1");
		if (printKeywords) printParameter(keyword2, "keyword2");
		printParameter(name, "name");
		if (printKeywords) printParameter(eos, "eos");
		printRuleTrailer();
	}

	/** R1235
	 * entry_stmt
	 */
	public void entry_stmt(Token label, Token keyword, Token id, Token eos, 
								  boolean hasDummyArgList, boolean hasSuffix) {
		printRuleHeader(1235, "entry-stmt");
		if (label != null) printParameter(label, "label");
		if (printKeywords) printParameter(keyword, "keyword");
		printParameter(id, "id");
		if (printKeywords) printParameter(eos, "eos");
		printParameter(hasDummyArgList, "hasDummyArgList");
		printParameter(hasSuffix, "hasSuffix");
		printRuleTrailer();
	}

	/** R1236
	 * return_stmt
	 */
	public void return_stmt(Token label, Token keyword, Token eos, 
									boolean hasScalarIntExpr) {
		printRuleHeader(1236, "return-stmt");
		if (label != null) printParameter(label, "label");
		if (printKeywords) printParameter(keyword, "keyword");
		if (printKeywords) printParameter(eos, "eos");
		printParameter(hasScalarIntExpr, "hasScalarIntExpr");
		printRuleTrailer();
	}

	/** R1237
	 * contains_stmt
	 */
	public void contains_stmt(Token label, Token keyword, Token eos) {
		printRuleHeader(1237, "contains-stmt");
		if (label != null) printParameter(label, "label");
		if (printKeywords) printParameter(keyword, "keyword");
		if (printKeywords) printParameter(eos, "eos");
		printRuleTrailer();
	}

	/** R1238
	 * stmt_function_stmt
	 *
	 * @param label The label.
	 * @param functionName The name of the function.
	 * @param eos T_EOS token.
	 * @param hasGenericNameList True if there is a list in the statement.
	 */
	public void stmt_function_stmt(Token label, Token functionName, Token eos, 
											 boolean hasGenericNameList) {
		printRuleHeader(1238, "stmt-function-stmt");
		if (label!=null) printParameter(label, "label");
		printParameter(functionName, "functionName");
		printParameter(hasGenericNameList, "hasGenericNameList");
		printRuleTrailer();
	}


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
// 	public void end_of_stmt(Token eos, String currStreamName, 
// 									String nextStreamName) {
// 		printRuleHeader(generatedRule, "end-of-stmt");
// 		printParameter(currStreamName, "currStreamName");
// 		printParameter(nextStreamName, "nextStreamName");
// 	}
	public void end_of_stmt(Token eos) {
	}


	public void start_of_file(String fileName) {
		printRuleHeader(generatedRule, "start-of-file");
		printParameter(fileName, "fileName");
		printRuleTrailer();
	}

	/**
	 * Inserted Rule
	 * end_of_file
	 */
	public void end_of_file() {
		printRuleHeader(generatedRule, "end-of-file");
		printRuleTrailer();
	}

	/**
	 * This method should never need to be called from Java.  
	 */
	public void cleanUp() {
		/* Do nothing.  */
	}
}
