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

/**
 * This file replaces integers with enums. It was getting to be a problem 
 * passing enums from java to C.
 */
public interface IActionEnums {

	// Here are the offsets.
	static final public int LiteralConstantBase=0;
	static final public int KindParamBase=100;
	static final public int KindSelectorBase=200;
	static final public int KindLenParamBase=300;
	static final public int IntrinsicTypeSpecBase=400;
	static final public int DeclarationTypeSpecBase=500;
	static final public int IntentSpecBase=600;
	static final public int ArraySpecElementBase=700;
	static final public int AttrSpecBase=800;
	static final public int BindingStatementTypeBase=900;
	static final public int ComponentDefTypeBase=1000;
	static final public int TypeParamOrCompDefBase=1100;
	static final public int TypeAttrSpecBase=1200;
	static final public int ComponentAttrSpecBase=1300;
	static final public int ProcComponentAttrSpecBase=1400;
	static final public int GenericSpecBase=1500;
	static final public int DTIOGenericSpecBase=1600;

	// And here are the "enums"
	static final public int LiteralConstant_int_literal_constant=
		LiteralConstantBase+0;
	static final public int LiteralConstant_real_literal_constant=
		LiteralConstantBase+1;
	static final public int LiteralConstant_complex_literal_constant=
		LiteralConstantBase+2;
	static final public int LiteralConstant_logical_literal_constant=
		LiteralConstantBase+3;
	static final public int LiteralConstant_char_literal_constant=
		LiteralConstantBase+4;
	static final public int LiteralConstant_boz_literal_constant=
		LiteralConstantBase+5;

	static final public int KindParam_none= KindParamBase+0;
	static final public int KindParam_literal= KindParamBase+1;
	static final public int KindParam_id= KindParamBase+2;
	
	static final public int KindSelector_none= KindSelectorBase+0;
	static final public int KindSelector_expression= KindSelectorBase+1;
	
	static final public int KindLenParam_none= KindLenParamBase+0;
	static final public int KindLenParam_kind= KindLenParamBase+1;
	static final public int KindLenParam_len= KindLenParamBase+2;

	static final public int IntrinsicTypeSpec_INTEGER=
		IntrinsicTypeSpecBase+0;
	static final public int IntrinsicTypeSpec_REAL=
		IntrinsicTypeSpecBase+1;
	static final public int IntrinsicTypeSpec_DOUBLEPRECISION=
		IntrinsicTypeSpecBase+2;
	static final public int IntrinsicTypeSpec_DOUBLECOMPLEX=
		IntrinsicTypeSpecBase+3;
	static final public int IntrinsicTypeSpec_COMPLEX=
		IntrinsicTypeSpecBase+4;
	static final public int IntrinsicTypeSpec_CHARACTER=
		IntrinsicTypeSpecBase+5;
	static final public int IntrinsicTypeSpec_LOGICAL=
		IntrinsicTypeSpecBase+6;
	
	static final public int DeclarationTypeSpec_INTRINSIC=
		DeclarationTypeSpecBase+0;
	static final public int DeclarationTypeSpec_TYPE=
		DeclarationTypeSpecBase+1;
	static final public int DeclarationTypeSpec_CLASS=
		DeclarationTypeSpecBase+2;
	static final public int DeclarationTypeSpec_unlimited=
		DeclarationTypeSpecBase+3;
	
	static final public int IntentSpec_IN=IntentSpecBase+0;
	static final public int IntentSpec_OUT=IntentSpecBase+1;
	static final public int IntentSpec_INOUT=IntentSpecBase+2;

	static final public int ArraySpecElement_expr=
		ArraySpecElementBase+0;
	static final public int ArraySpecElement_expr_colon=
		ArraySpecElementBase+1;
	static final public int ArraySpecElement_expr_colon_expr=
		ArraySpecElementBase+2;
	static final public int ArraySpecElement_expr_colon_asterisk=
		ArraySpecElementBase+3;
	static final public int ArraySpecElement_asterisk=
		ArraySpecElementBase+4;
	static final public int ArraySpecElement_colon=
		ArraySpecElementBase+5;

	static final public int AttrSpec_none=AttrSpecBase+0;
	static final public int AttrSpec_access=AttrSpecBase+1;
	static final public int AttrSpec_language_binding=AttrSpecBase+2;
	static final public int AttrSpec_PUBLIC=AttrSpecBase+3;
	static final public int AttrSpec_PRIVATE=AttrSpecBase+4;
	static final public int AttrSpec_ALLOCATABLE=AttrSpecBase+5;
	static final public int AttrSpec_ASYNCHRONOUS=AttrSpecBase+6;
	static final public int AttrSpec_DIMENSION=AttrSpecBase+7;
	static final public int AttrSpec_EXTERNAL=AttrSpecBase+8;
	static final public int AttrSpec_INTENT=AttrSpecBase+9;
	static final public int AttrSpec_INTRINSIC=AttrSpecBase+10;
	static final public int AttrSpec_BINDC=AttrSpecBase+11;
	static final public int AttrSpec_OPTIONAL=AttrSpecBase+12;
	static final public int AttrSpec_PARAMETER=AttrSpecBase+13;
	static final public int AttrSpec_POINTER=AttrSpecBase+14;
	static final public int AttrSpec_PROTECTED=AttrSpecBase+15;
	static final public int AttrSpec_SAVE=AttrSpecBase+16;
	static final public int AttrSpec_TARGET=AttrSpecBase+17;
	static final public int AttrSpec_VALUE=AttrSpecBase+18;
	static final public int AttrSpec_VOLATILE=AttrSpecBase+19;
	static final public int AttrSpec_PASS=AttrSpecBase+20;
	static final public int AttrSpec_NOPASS=AttrSpecBase+21;
	static final public int AttrSpec_NON_OVERRIDABLE=AttrSpecBase+22;
	static final public int AttrSpec_DEFERRED=AttrSpecBase+23;
	// TODO are T_KIND and T_LEN correct?
	static final public int AttrSpec_KIND=AttrSpecBase+24;
	static final public int AttrSpec_LEN=AttrSpecBase+25;

	static final public int BindingStatementType_specific=
		BindingStatementTypeBase+0;
	static final public int BindingStatementType_generic=
		BindingStatementTypeBase+1;
	static final public int BindingStatementType_final=
		BindingStatementTypeBase+2;

	static final public int ComponentDefType_data=ComponentDefTypeBase+0;
	static final public int ComponentDefType_procedure=ComponentDefTypeBase+1;

	static final public int TypeParamOrCompDef_typeParam= 
		TypeParamOrCompDefBase+0;
	static final public int TypeParamOrCompDef_compDef=
		TypeParamOrCompDefBase+1;

	static final public int TypeAttrSpec_access_spec=TypeAttrSpecBase+0;
	static final public int TypeAttrSpec_extends=TypeAttrSpecBase+1;
	static final public int TypeAttrSpec_abstract=TypeAttrSpecBase+2;
	static final public int TypeAttrSpec_bind=TypeAttrSpecBase+3;

	static final public int ComponentAttrSpec_pointer=
		ComponentAttrSpecBase+0;
	static final public int ComponentAttrSpec_dimension_paren=
		ComponentAttrSpecBase+1;
	static final public int ComponentAttrSpec_dimension_bracket=
		ComponentAttrSpecBase+2;
	static final public int ComponentAttrSpec_allocatable=
		ComponentAttrSpecBase+3;
	static final public int ComponentAttrSpec_access_spec=
		ComponentAttrSpecBase+4;
	static final public int ComponentAttrSpec_kind=
		ComponentAttrSpecBase+5;
	static final public int ComponentAttrSpec_len=
		ComponentAttrSpecBase+6;

	static final public int ProcComponentAttrSpec_pointer=
		ProcComponentAttrSpecBase+0;
	static final public int ProcComponentAttrSpec_pass=
		ProcComponentAttrSpecBase+1;
	static final public int ProcComponentAttrSpec_nopass=
		ProcComponentAttrSpecBase+2;
	static final public int ProcComponentAttrSpec_access_spec=
		ProcComponentAttrSpecBase+3;
	
	static final public int GenericSpec_generic_name=
		GenericSpecBase+0;
	static final public int GenericSpec_OPERATOR=
		GenericSpecBase+1;
	static final public int GenericSpec_ASSIGNMENT=
		GenericSpecBase+2;
	static final public int GenericSpec_dtio_generic_spec=
		GenericSpecBase+3;
	
	static final public int DTIOGenericSpec_READ_FORMATTED=
		DTIOGenericSpecBase+0;
	static final public int DTIOGenericSpec_READ_UNFORMATTED=
		DTIOGenericSpecBase+1;
	static final public int DTIOGenericSpec_WRITE_FORMATTED=
		DTIOGenericSpecBase+2;
	static final public int DTIOGenericSpec_WRITE_UNFORMATTED=
		DTIOGenericSpecBase+3;

}
