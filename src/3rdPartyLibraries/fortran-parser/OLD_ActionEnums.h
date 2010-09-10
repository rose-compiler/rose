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

#ifndef FORTRANPARSERENUMSHEAD
#define FORTRANPARSERENUMSHEAD

#ifdef __cplusplus
extern "C" {
#endif

#include "token.h"

/**
 * This file replaces integers with enums. It was getting to be a problem 
 * passing enums from java to C.
 */

#define LiteralConstantBase 0
#define KindParamBase 100
#define KindSelectorBase 200
#define KindLenParamBase 300
#define IntrinsicTypeSpecBase 400
#define DeclarationTypeSpecBase 500
#define IntentSpecBase 600
#define ArraySpecElementBase 700
#define AttrSpecBase 800
#define BindingStatementTypeBase 900
#define ComponentDefTypeBase 1000
#define TypeParamOrCompDefBase 1100
#define TypeAttrSpecBase 1200
#define ComponentAttrSpecBase 1300
#define ProcComponentAttrSpecBase 1400
#define GenericSpecBase 1500
#define DTIOGenericSpecBase 1600

static const int LiteralConstant_int_literal_constant=LiteralConstantBase+0;
static const int LiteralConstant_real_literal_constant=LiteralConstantBase+1;
static const int LiteralConstant_complex_literal_constant=LiteralConstantBase+2;
static const int LiteralConstant_logical_literal_constant=LiteralConstantBase+3;
static const int LiteralConstant_char_literal_constant=LiteralConstantBase+4;
static const int LiteralConstant_boz_literal_constant=LiteralConstantBase+5;

static const int KindParam_none= KindParamBase+0;
static const int KindParam_literal= KindParamBase+1;
static const int KindParam_id= KindParamBase+2;

static const int KindSelector_none= KindSelectorBase+0;
static const int KindSelector_expression= KindSelectorBase+1;

static const int KindLenParam_none= KindLenParamBase+0;
static const int KindLenParam_kind= KindLenParamBase+1;
static const int KindLenParam_len= KindLenParamBase+2;

static const int IntrinsicTypeSpec_INTEGER=IntrinsicTypeSpecBase+0;
static const int IntrinsicTypeSpec_REAL=IntrinsicTypeSpecBase+1;
static const int IntrinsicTypeSpec_DOUBLEPRECISION=IntrinsicTypeSpecBase+2;
static const int IntrinsicTypeSpec_DOUBLECOMPLEX=IntrinsicTypeSpecBase+3;
static const int IntrinsicTypeSpec_COMPLEX=IntrinsicTypeSpecBase+4;
static const int IntrinsicTypeSpec_CHARACTER=IntrinsicTypeSpecBase+5;
static const int IntrinsicTypeSpec_LOGICAL=IntrinsicTypeSpecBase+6;

static const int DeclarationTypeSpec_INTRINSIC=DeclarationTypeSpecBase+0;
static const int DeclarationTypeSpec_TYPE=DeclarationTypeSpecBase+1;
static const int DeclarationTypeSpec_CLASS=DeclarationTypeSpecBase+2;
static const int DeclarationTypeSpec_unlimited=DeclarationTypeSpecBase+3;

static const int IntentSpec_IN=IntentSpecBase+0;
static const int IntentSpec_OUT=IntentSpecBase+1;
static const int IntentSpec_INOUT=IntentSpecBase+2;

static const int ArraySpecElement_expr=ArraySpecElementBase+0;
static const int ArraySpecElement_expr_colon=ArraySpecElementBase+1;
static const int ArraySpecElement_expr_colon_expr=ArraySpecElementBase+2;
static const int ArraySpecElement_expr_colon_asterisk=ArraySpecElementBase+3;
static const int ArraySpecElement_asterisk=ArraySpecElementBase+4;
static const int ArraySpecElement_colon=ArraySpecElementBase+5;

static const int AttrSpec_none=AttrSpecBase+0;
static const int AttrSpec_access=AttrSpecBase+1;
static const int AttrSpec_language_binding=AttrSpecBase+2;
static const int AttrSpec_PUBLIC=AttrSpecBase+3;
static const int AttrSpec_PRIVATE=AttrSpecBase+4;
static const int AttrSpec_ALLOCATABLE=AttrSpecBase+5;
static const int AttrSpec_ASYNCHRONOUS=AttrSpecBase+6;
static const int AttrSpec_DIMENSION=AttrSpecBase+7;
static const int AttrSpec_EXTERNAL=AttrSpecBase+8;
static const int AttrSpec_INTENT=AttrSpecBase+9;
static const int AttrSpec_INTRINSIC=AttrSpecBase+10;
static const int AttrSpec_BINDC=AttrSpecBase+11;
static const int AttrSpec_OPTIONAL=AttrSpecBase+12;
static const int AttrSpec_PARAMETER=AttrSpecBase+13;
static const int AttrSpec_POINTER=AttrSpecBase+14;
static const int AttrSpec_PROTECTED=AttrSpecBase+15;
static const int AttrSpec_SAVE=AttrSpecBase+16;
static const int AttrSpec_TARGET=AttrSpecBase+17;
static const int AttrSpec_VALUE=AttrSpecBase+18;
static const int AttrSpec_VOLATILE=AttrSpecBase+19;
static const int AttrSpec_PASS=AttrSpecBase+20;
static const int AttrSpec_NOPASS=AttrSpecBase+21;
static const int AttrSpec_NON_OVERRIDABLE=AttrSpecBase+22;
static const int AttrSpec_DEFERRED=AttrSpecBase+23;
static const int AttrSpec_KIND=AttrSpecBase+24;
static const int AttrSpec_LEN=AttrSpecBase+25;

static const int BindingStatementType_specific=BindingStatementTypeBase+0;
static const int BindingStatementType_generic=BindingStatementTypeBase+1;
static const int BindingStatementType_const=BindingStatementTypeBase+2;

static const int ComponentDefType_data=ComponentDefTypeBase+0;
static const int ComponentDefType_procedure=ComponentDefTypeBase+1;

static const int TypeParamOrCompDef_typeParam=TypeParamOrCompDefBase+0;
static const int TypeParamOrCompDef_compDef=TypeParamOrCompDefBase+1;

static const int TypeAttrSpec_access_spec=TypeAttrSpecBase+0;
static const int TypeAttrSpec_extends=TypeAttrSpecBase+1;
static const int TypeAttrSpec_abstract=TypeAttrSpecBase+2;
static const int TypeAttrSpec_bind=TypeAttrSpecBase+3;

// DQ (8/29/2010): This is supposed to be an automatically generated file and it appears 
// to not be generated correctly. Could there be other values that are similarly wrong!
#if ROSE_OFP_MINOR_VERSION_NUMBER == 7
static const int ComponentAttrSpec_pointer=ComponentAttrSpecBase+0;
static const int ComponentAttrSpec_dimension_paren=ComponentAttrSpecBase+1;
static const int ComponentAttrSpec_dimension_bracket=ComponentAttrSpecBase+2;
static const int ComponentAttrSpec_allocatable=ComponentAttrSpecBase+3;
static const int ComponentAttrSpec_access_spec=ComponentAttrSpecBase+4;
static const int ComponentAttrSpec_kind=ComponentAttrSpecBase+5;
static const int ComponentAttrSpec_len=ComponentAttrSpecBase+6;
#else
// DQ (8/29/2010): Newer values not automatically generated properly.
static const int ComponentAttrSpec_access_spec=ComponentAttrSpecBase+0;
static const int ComponentAttrSpec_allocatable=ComponentAttrSpecBase+1;
static const int ComponentAttrSpec_codimension=ComponentAttrSpecBase+2;
static const int ComponentAttrSpec_contiguous=ComponentAttrSpecBase+3;
static const int ComponentAttrSpec_dimension=ComponentAttrSpecBase+4;
static const int ComponentAttrSpec_pointer=ComponentAttrSpecBase+5;
static const int ComponentAttrSpec_kind=ComponentAttrSpecBase+6;
static const int ComponentAttrSpec_len=ComponentAttrSpecBase+7;
#endif


static const int ProcComponentAttrSpec_pointer=ProcComponentAttrSpecBase+0;
static const int ProcComponentAttrSpec_pass=ProcComponentAttrSpecBase+1;
static const int ProcComponentAttrSpec_nopass=ProcComponentAttrSpecBase+2;
static const int ProcComponentAttrSpec_access_spec=ProcComponentAttrSpecBase+3;

static const int GenericSpec_generic_name=GenericSpecBase+0;
static const int GenericSpec_OPERATOR=GenericSpecBase+1;
static const int GenericSpec_ASSIGNMENT=GenericSpecBase+2;
static const int GenericSpec_dtio_generic_spec=GenericSpecBase+3;

static const int DTIOGenericSpec_READ_FORMATTED=DTIOGenericSpecBase+0;
static const int DTIOGenericSpec_READ_UNFORMATTED=DTIOGenericSpecBase+1;
static const int DTIOGenericSpec_WRITE_FORMATTED=DTIOGenericSpecBase+2;
static const int DTIOGenericSpec_WRITE_UNFORMATTED=DTIOGenericSpecBase+3;



#ifdef __cplusplus
} /* End extern C. */
#endif

#endif
