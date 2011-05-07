#include <stdlib.h>
#include <string.h>
#include "jni.h"
#include "token.h"
#include "jni_token_funcs.h"
#include "FortranParserAction.h"

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_name(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "name");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_name(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_substring(JNIEnv *env, jobject this, jboolean jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "substring");
#endif

	carg_0 = (int)jarg_0;

	c_action_substring(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_format(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "format");
#endif


	c_action_format();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_rename(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jobject jarg_4, jobject jarg_5)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	Token_t *carg_4 = NULL;
	Token_t *carg_5 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "rename");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	if(jarg_4 != NULL)
		carg_4 = convert_Java_token(env, jarg_4);
	if(jarg_5 != NULL)
		carg_5 = convert_Java_token(env, jarg_5);

	c_action_rename(carg_0, carg_1, carg_2, carg_3, carg_4, carg_5);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
	if(carg_4 != NULL)
		register_token(carg_4);
	if(carg_5 != NULL)
		register_token(carg_5);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_prefix(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "prefix");
#endif

	carg_0 = (int)jarg_0;

	c_action_prefix(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_block(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "block");
#endif


	c_action_block();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_suffix(JNIEnv *env, jobject this, jobject jarg_0, jboolean jarg_1)
{
	Token_t *carg_0 = NULL;
	int carg_1;

#ifdef DEBUG
	printf("hello from %s\n", "suffix");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	carg_1 = (int)jarg_1;

	c_action_suffix(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_keyword(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "keyword");
#endif


	c_action_keyword();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_expr(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "expr");
#endif


	c_action_expr();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_cleanUp(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "cleanUp");
#endif


	c_action_cleanUp();

	free_token_list();
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_generic_1name_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "generic_name_list__begin");
#endif


	c_action_generic_name_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_generic_1name_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "generic_name_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_generic_name_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_generic_1name_1list_1part(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "generic_name_list_part");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_generic_name_list_part(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_specification_1part(JNIEnv *env, jobject this, jint jarg_0, jint jarg_1, jint jarg_2, jint jarg_3)
{
	int carg_0;
	int carg_1;
	int carg_2;
	int carg_3;

#ifdef DEBUG
	printf("hello from %s\n", "specification_part");
#endif

	carg_0 = (int)jarg_0;
	carg_1 = (int)jarg_1;
	carg_2 = (int)jarg_2;
	carg_3 = (int)jarg_3;

	c_action_specification_part(carg_0, carg_1, carg_2, carg_3);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_declaration_1construct(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "declaration_construct");
#endif


	c_action_declaration_construct();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_execution_1part(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "execution_part");
#endif


	c_action_execution_part();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_execution_1part_1construct(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "execution_part_construct");
#endif


	c_action_execution_part_construct();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_internal_1subprogram_1part(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "internal_subprogram_part");
#endif

	carg_0 = (int)jarg_0;

	c_action_internal_subprogram_part(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_internal_1subprogram(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "internal_subprogram");
#endif


	c_action_internal_subprogram();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_specification_1stmt(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "specification_stmt");
#endif


	c_action_specification_stmt();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_executable_1construct(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "executable_construct");
#endif


	c_action_executable_construct();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_action_1stmt(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "action_stmt");
#endif


	c_action_action_stmt();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_constant(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "constant");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_constant(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_scalar_1constant(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "scalar_constant");
#endif


	c_action_scalar_constant();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_literal_1constant(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "literal_constant");
#endif


	c_action_literal_constant();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_int_1constant(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "int_constant");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_int_constant(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_char_1constant(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "char_constant");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_char_constant(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_intrinsic_1operator(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "intrinsic_operator");
#endif


	c_action_intrinsic_operator();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_defined_1operator(JNIEnv *env, jobject this, jobject jarg_0, jboolean jarg_1)
{
	Token_t *carg_0 = NULL;
	int carg_1;

#ifdef DEBUG
	printf("hello from %s\n", "defined_operator");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	carg_1 = (int)jarg_1;

	c_action_defined_operator(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_extended_1intrinsic_1op(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "extended_intrinsic_op");
#endif


	c_action_extended_intrinsic_op();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_label(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "label");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_label(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_label_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "label_list__begin");
#endif


	c_action_label_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_label_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "label_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_label_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_type_1spec(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "type_spec");
#endif


	c_action_type_spec();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_type_1param_1value(JNIEnv *env, jobject this, jboolean jarg_0, jboolean jarg_1, jboolean jarg_2)
{
	int carg_0;
	int carg_1;
	int carg_2;

#ifdef DEBUG
	printf("hello from %s\n", "type_param_value");
#endif

	carg_0 = (int)jarg_0;
	carg_1 = (int)jarg_1;
	carg_2 = (int)jarg_2;

	c_action_type_param_value(carg_0, carg_1, carg_2);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_intrinsic_1type_1spec(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jint jarg_2, jboolean jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	int carg_2;
	int carg_3;

#ifdef DEBUG
	printf("hello from %s\n", "intrinsic_type_spec");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	carg_2 = (int)jarg_2;
	carg_3 = (int)jarg_3;

	c_action_intrinsic_type_spec(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_kind_1selector(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jboolean jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	int carg_2;

#ifdef DEBUG
	printf("hello from %s\n", "kind_selector");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	carg_2 = (int)jarg_2;

	c_action_kind_selector(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_signed_1int_1literal_1constant(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "signed_int_literal_constant");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_signed_int_literal_constant(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_int_1literal_1constant(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "int_literal_constant");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);

	c_action_int_literal_constant(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_kind_1param(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "kind_param");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_kind_param(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_boz_1literal_1constant(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "boz_literal_constant");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_boz_literal_constant(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_signed_1real_1literal_1constant(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "signed_real_literal_constant");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_signed_real_literal_constant(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_real_1literal_1constant(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "real_literal_constant");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);

	c_action_real_literal_constant(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_complex_1literal_1constant(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "complex_literal_constant");
#endif


	c_action_complex_literal_constant();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_real_1part(JNIEnv *env, jobject this, jboolean jarg_0, jboolean jarg_1, jobject jarg_2)
{
	int carg_0;
	int carg_1;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "real_part");
#endif

	carg_0 = (int)jarg_0;
	carg_1 = (int)jarg_1;
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_real_part(carg_0, carg_1, carg_2);

	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_imag_1part(JNIEnv *env, jobject this, jboolean jarg_0, jboolean jarg_1, jobject jarg_2)
{
	int carg_0;
	int carg_1;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "imag_part");
#endif

	carg_0 = (int)jarg_0;
	carg_1 = (int)jarg_1;
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_imag_part(carg_0, carg_1, carg_2);

	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_char_1selector(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jint jarg_2, jint jarg_3, jboolean jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	int carg_2;
	int carg_3;
	int carg_4;

#ifdef DEBUG
	printf("hello from %s\n", "char_selector");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	carg_2 = (int)jarg_2;
	carg_3 = (int)jarg_3;
	carg_4 = (int)jarg_4;

	c_action_char_selector(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_length_1selector(JNIEnv *env, jobject this, jobject jarg_0, jint jarg_1, jboolean jarg_2)
{
	Token_t *carg_0 = NULL;
	int carg_1;
	int carg_2;

#ifdef DEBUG
	printf("hello from %s\n", "length_selector");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	carg_1 = (int)jarg_1;
	carg_2 = (int)jarg_2;

	c_action_length_selector(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_char_1length(JNIEnv *env, jobject this, jboolean jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "char_length");
#endif

	carg_0 = (int)jarg_0;

	c_action_char_length(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_scalar_1int_1literal_1constant(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "scalar_int_literal_constant");
#endif


	c_action_scalar_int_literal_constant();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_char_1literal_1constant(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "char_literal_constant");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_char_literal_constant(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_logical_1literal_1constant(JNIEnv *env, jobject this, jobject jarg_0, jboolean jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	int carg_1;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "logical_literal_constant");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	carg_1 = (int)jarg_1;
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_logical_literal_constant(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_derived_1type_1def(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "derived_type_def");
#endif


	c_action_derived_type_def();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_type_1param_1or_1comp_1def_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jint jarg_1)
{
	Token_t *carg_0 = NULL;
	int carg_1;

#ifdef DEBUG
	printf("hello from %s\n", "type_param_or_comp_def_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	carg_1 = (int)jarg_1;

	c_action_type_param_or_comp_def_stmt(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_type_1param_1or_1comp_1def_1stmt_1list(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "type_param_or_comp_def_stmt_list");
#endif


	c_action_type_param_or_comp_def_stmt_list();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_derived_1type_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jboolean jarg_4, jboolean jarg_5)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	int carg_4;
	int carg_5;

#ifdef DEBUG
	printf("hello from %s\n", "derived_type_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	carg_4 = (int)jarg_4;
	carg_5 = (int)jarg_5;

	c_action_derived_type_stmt(carg_0, carg_1, carg_2, carg_3, carg_4, carg_5);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_type_1attr_1spec(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jint jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	int carg_2;

#ifdef DEBUG
	printf("hello from %s\n", "type_attr_spec");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	carg_2 = (int)jarg_2;

	c_action_type_attr_spec(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_type_1attr_1spec_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "type_attr_spec_list__begin");
#endif


	c_action_type_attr_spec_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_type_1attr_1spec_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "type_attr_spec_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_type_attr_spec_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_private_1or_1sequence(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "private_or_sequence");
#endif


	c_action_private_or_sequence();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_end_1type_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jobject jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	Token_t *carg_4 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "end_type_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	if(jarg_4 != NULL)
		carg_4 = convert_Java_token(env, jarg_4);

	c_action_end_type_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
	if(carg_4 != NULL)
		register_token(carg_4);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_sequence_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "sequence_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_sequence_stmt(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_type_1param_1decl(JNIEnv *env, jobject this, jobject jarg_0, jboolean jarg_1)
{
	Token_t *carg_0 = NULL;
	int carg_1;

#ifdef DEBUG
	printf("hello from %s\n", "type_param_decl");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	carg_1 = (int)jarg_1;

	c_action_type_param_decl(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_type_1param_1decl_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "type_param_decl_list__begin");
#endif


	c_action_type_param_decl_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_type_1param_1decl_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "type_param_decl_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_type_param_decl_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_type_1param_1attr_1spec(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "type_param_attr_spec");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_type_param_attr_spec(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_component_1def_1stmt(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "component_def_stmt");
#endif

	carg_0 = (int)jarg_0;

	c_action_component_def_stmt(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_data_1component_1def_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jboolean jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	int carg_2;

#ifdef DEBUG
	printf("hello from %s\n", "data_component_def_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	carg_2 = (int)jarg_2;

	c_action_data_component_def_stmt(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_component_1attr_1spec(JNIEnv *env, jobject this, jobject jarg_0, jint jarg_1)
{
	Token_t *carg_0 = NULL;
	int carg_1;

#ifdef DEBUG
	printf("hello from %s\n", "component_attr_spec");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	carg_1 = (int)jarg_1;

	c_action_component_attr_spec(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_component_1attr_1spec_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "component_attr_spec_list__begin");
#endif


	c_action_component_attr_spec_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_component_1attr_1spec_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "component_attr_spec_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_component_attr_spec_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_component_1decl(JNIEnv *env, jobject this, jobject jarg_0, jboolean jarg_1, jboolean jarg_2, jboolean jarg_3, jboolean jarg_4)
{
	Token_t *carg_0 = NULL;
	int carg_1;
	int carg_2;
	int carg_3;
	int carg_4;

#ifdef DEBUG
	printf("hello from %s\n", "component_decl");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	carg_1 = (int)jarg_1;
	carg_2 = (int)jarg_2;
	carg_3 = (int)jarg_3;
	carg_4 = (int)jarg_4;

	c_action_component_decl(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_component_1decl_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "component_decl_list__begin");
#endif


	c_action_component_decl_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_component_1decl_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "component_decl_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_component_decl_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_component_1array_1spec(JNIEnv *env, jobject this, jboolean jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "component_array_spec");
#endif

	carg_0 = (int)jarg_0;

	c_action_component_array_spec(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_deferred_1shape_1spec_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "deferred_shape_spec_list__begin");
#endif


	c_action_deferred_shape_spec_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_deferred_1shape_1spec_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "deferred_shape_spec_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_deferred_shape_spec_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_component_1initialization(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "component_initialization");
#endif


	c_action_component_initialization();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_proc_1component_1def_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jboolean jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	int carg_3;

#ifdef DEBUG
	printf("hello from %s\n", "proc_component_def_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	carg_3 = (int)jarg_3;

	c_action_proc_component_def_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_proc_1component_1attr_1spec(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jint jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	int carg_2;

#ifdef DEBUG
	printf("hello from %s\n", "proc_component_attr_spec");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	carg_2 = (int)jarg_2;

	c_action_proc_component_attr_spec(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_proc_1component_1attr_1spec_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "proc_component_attr_spec_list__begin");
#endif


	c_action_proc_component_attr_spec_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_proc_1component_1attr_1spec_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "proc_component_attr_spec_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_proc_component_attr_spec_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_private_1components_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "private_components_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_private_components_stmt(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_type_1bound_1procedure_1part(JNIEnv *env, jobject this, jint jarg_0, jboolean jarg_1)
{
	int carg_0;
	int carg_1;

#ifdef DEBUG
	printf("hello from %s\n", "type_bound_procedure_part");
#endif

	carg_0 = (int)jarg_0;
	carg_1 = (int)jarg_1;

	c_action_type_bound_procedure_part(carg_0, carg_1);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_binding_1private_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "binding_private_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_binding_private_stmt(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_proc_1binding_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jint jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	int carg_1;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "proc_binding_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	carg_1 = (int)jarg_1;
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_proc_binding_stmt(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_specific_1binding(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jboolean jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	int carg_4;

#ifdef DEBUG
	printf("hello from %s\n", "specific_binding");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	carg_4 = (int)jarg_4;

	c_action_specific_binding(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_generic_1binding(JNIEnv *env, jobject this, jobject jarg_0, jboolean jarg_1)
{
	Token_t *carg_0 = NULL;
	int carg_1;

#ifdef DEBUG
	printf("hello from %s\n", "generic_binding");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	carg_1 = (int)jarg_1;

	c_action_generic_binding(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_binding_1attr(JNIEnv *env, jobject this, jobject jarg_0, jint jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	int carg_1;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "binding_attr");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	carg_1 = (int)jarg_1;
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_binding_attr(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_binding_1attr_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "binding_attr_list__begin");
#endif


	c_action_binding_attr_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_binding_1attr_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "binding_attr_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_binding_attr_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_final_1binding(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "final_binding");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_final_binding(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_derived_1type_1spec(JNIEnv *env, jobject this, jobject jarg_0, jboolean jarg_1)
{
	Token_t *carg_0 = NULL;
	int carg_1;

#ifdef DEBUG
	printf("hello from %s\n", "derived_type_spec");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	carg_1 = (int)jarg_1;

	c_action_derived_type_spec(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_type_1param_1spec(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "type_param_spec");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_type_param_spec(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_type_1param_1spec_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "type_param_spec_list__begin");
#endif


	c_action_type_param_spec_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_type_1param_1spec_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "type_param_spec_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_type_param_spec_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_structure_1constructor(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "structure_constructor");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_structure_constructor(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_component_1spec(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "component_spec");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_component_spec(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_component_1spec_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "component_spec_list__begin");
#endif


	c_action_component_spec_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_component_1spec_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "component_spec_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_component_spec_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_component_1data_1source(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "component_data_source");
#endif


	c_action_component_data_source();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_enum_1def(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "enum_def");
#endif

	carg_0 = (int)jarg_0;

	c_action_enum_def(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_enum_1def_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jobject jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	Token_t *carg_4 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "enum_def_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	if(jarg_4 != NULL)
		carg_4 = convert_Java_token(env, jarg_4);

	c_action_enum_def_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
	if(carg_4 != NULL)
		register_token(carg_4);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_enumerator_1def_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "enumerator_def_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_enumerator_def_stmt(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_enumerator(JNIEnv *env, jobject this, jobject jarg_0, jboolean jarg_1)
{
	Token_t *carg_0 = NULL;
	int carg_1;

#ifdef DEBUG
	printf("hello from %s\n", "enumerator");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	carg_1 = (int)jarg_1;

	c_action_enumerator(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_enumerator_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "enumerator_list__begin");
#endif


	c_action_enumerator_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_enumerator_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "enumerator_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_enumerator_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_end_1enum_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "end_enum_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);

	c_action_end_enum_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_array_1constructor(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "array_constructor");
#endif


	c_action_array_constructor();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_ac_1spec(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "ac_spec");
#endif


	c_action_ac_spec();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_ac_1value(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "ac_value");
#endif


	c_action_ac_value();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_ac_1value_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "ac_value_list__begin");
#endif


	c_action_ac_value_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_ac_1value_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "ac_value_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_ac_value_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_ac_1implied_1do(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "ac_implied_do");
#endif


	c_action_ac_implied_do();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_ac_1implied_1do_1control(JNIEnv *env, jobject this, jboolean jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "ac_implied_do_control");
#endif

	carg_0 = (int)jarg_0;

	c_action_ac_implied_do_control(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_scalar_1int_1variable(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "scalar_int_variable");
#endif


	c_action_scalar_int_variable();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_type_1declaration_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jint jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	int carg_1;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "type_declaration_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	carg_1 = (int)jarg_1;
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_type_declaration_stmt(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_declaration_1type_1spec(JNIEnv *env, jobject this, jobject jarg_0, jint jarg_1)
{
	Token_t *carg_0 = NULL;
	int carg_1;

#ifdef DEBUG
	printf("hello from %s\n", "declaration_type_spec");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	carg_1 = (int)jarg_1;

	c_action_declaration_type_spec(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_attr_1spec(JNIEnv *env, jobject this, jobject jarg_0, jint jarg_1)
{
	Token_t *carg_0 = NULL;
	int carg_1;

#ifdef DEBUG
	printf("hello from %s\n", "attr_spec");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	carg_1 = (int)jarg_1;

	c_action_attr_spec(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_entity_1decl(JNIEnv *env, jobject this, jobject jarg_0, jboolean jarg_1, jboolean jarg_2, jboolean jarg_3, jboolean jarg_4)
{
	Token_t *carg_0 = NULL;
	int carg_1;
	int carg_2;
	int carg_3;
	int carg_4;

#ifdef DEBUG
	printf("hello from %s\n", "entity_decl");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	carg_1 = (int)jarg_1;
	carg_2 = (int)jarg_2;
	carg_3 = (int)jarg_3;
	carg_4 = (int)jarg_4;

	c_action_entity_decl(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_entity_1decl_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "entity_decl_list__begin");
#endif


	c_action_entity_decl_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_entity_1decl_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "entity_decl_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_entity_decl_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_initialization(JNIEnv *env, jobject this, jboolean jarg_0, jboolean jarg_1)
{
	int carg_0;
	int carg_1;

#ifdef DEBUG
	printf("hello from %s\n", "initialization");
#endif

	carg_0 = (int)jarg_0;
	carg_1 = (int)jarg_1;

	c_action_initialization(carg_0, carg_1);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_null_1init(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "null_init");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_null_init(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_access_1spec(JNIEnv *env, jobject this, jobject jarg_0, jint jarg_1)
{
	Token_t *carg_0 = NULL;
	int carg_1;

#ifdef DEBUG
	printf("hello from %s\n", "access_spec");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	carg_1 = (int)jarg_1;

	c_action_access_spec(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_language_1binding_1spec(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jboolean jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	int carg_2;

#ifdef DEBUG
	printf("hello from %s\n", "language_binding_spec");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	carg_2 = (int)jarg_2;

	c_action_language_binding_spec(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_coarray_1spec(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "coarray_spec");
#endif

	carg_0 = (int)jarg_0;

	c_action_coarray_spec(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_array_1spec(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "array_spec");
#endif

	carg_0 = (int)jarg_0;

	c_action_array_spec(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_array_1spec_1element(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "array_spec_element");
#endif

	carg_0 = (int)jarg_0;

	c_action_array_spec_element(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_explicit_1shape_1spec(JNIEnv *env, jobject this, jboolean jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "explicit_shape_spec");
#endif

	carg_0 = (int)jarg_0;

	c_action_explicit_shape_spec(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_explicit_1shape_1spec_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "explicit_shape_spec_list__begin");
#endif


	c_action_explicit_shape_spec_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_explicit_1shape_1spec_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "explicit_shape_spec_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_explicit_shape_spec_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_intent_1spec(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jint jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	int carg_2;

#ifdef DEBUG
	printf("hello from %s\n", "intent_spec");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	carg_2 = (int)jarg_2;

	c_action_intent_spec(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_access_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jboolean jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	int carg_2;

#ifdef DEBUG
	printf("hello from %s\n", "access_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	carg_2 = (int)jarg_2;

	c_action_access_stmt(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_access_1id(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "access_id");
#endif


	c_action_access_id();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_access_1id_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "access_id_list__begin");
#endif


	c_action_access_id_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_access_1id_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "access_id_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_access_id_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_allocatable_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "allocatable_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_allocatable_stmt(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_allocatable_1decl(JNIEnv *env, jobject this, jobject jarg_0, jboolean jarg_1, jboolean jarg_2)
{
	Token_t *carg_0 = NULL;
	int carg_1;
	int carg_2;

#ifdef DEBUG
	printf("hello from %s\n", "allocatable_decl");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	carg_1 = (int)jarg_1;
	carg_2 = (int)jarg_2;

	c_action_allocatable_decl(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_allocatable_1decl_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "allocatable_decl_list__begin");
#endif


	c_action_allocatable_decl_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_allocatable_1decl_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "allocatable_decl_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_allocatable_decl_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_asynchronous_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "asynchronous_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_asynchronous_stmt(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_bind_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "bind_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);

	c_action_bind_stmt(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_bind_1entity(JNIEnv *env, jobject this, jobject jarg_0, jboolean jarg_1)
{
	Token_t *carg_0 = NULL;
	int carg_1;

#ifdef DEBUG
	printf("hello from %s\n", "bind_entity");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	carg_1 = (int)jarg_1;

	c_action_bind_entity(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_bind_1entity_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "bind_entity_list__begin");
#endif


	c_action_bind_entity_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_bind_1entity_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "bind_entity_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_bind_entity_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_codimension_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "codimension_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_codimension_stmt(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_codimension_1decl(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "codimension_decl");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_codimension_decl(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_codimension_1decl_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "codimension_decl_list__begin");
#endif


	c_action_codimension_decl_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_codimension_1decl_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "codimension_decl_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_codimension_decl_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_data_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jint jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	int carg_3;

#ifdef DEBUG
	printf("hello from %s\n", "data_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	carg_3 = (int)jarg_3;

	c_action_data_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_data_1stmt_1set(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "data_stmt_set");
#endif


	c_action_data_stmt_set();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_data_1stmt_1object(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "data_stmt_object");
#endif


	c_action_data_stmt_object();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_data_1stmt_1object_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "data_stmt_object_list__begin");
#endif


	c_action_data_stmt_object_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_data_1stmt_1object_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "data_stmt_object_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_data_stmt_object_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_data_1implied_1do(JNIEnv *env, jobject this, jobject jarg_0, jboolean jarg_1)
{
	Token_t *carg_0 = NULL;
	int carg_1;

#ifdef DEBUG
	printf("hello from %s\n", "data_implied_do");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	carg_1 = (int)jarg_1;

	c_action_data_implied_do(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_data_1i_1do_1object(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "data_i_do_object");
#endif


	c_action_data_i_do_object();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_data_1i_1do_1object_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "data_i_do_object_list__begin");
#endif


	c_action_data_i_do_object_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_data_1i_1do_1object_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "data_i_do_object_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_data_i_do_object_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_data_1stmt_1value(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "data_stmt_value");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_data_stmt_value(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_data_1stmt_1value_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "data_stmt_value_list__begin");
#endif


	c_action_data_stmt_value_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_data_1stmt_1value_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "data_stmt_value_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_data_stmt_value_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_scalar_1int_1constant(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "scalar_int_constant");
#endif


	c_action_scalar_int_constant();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_hollerith_1constant(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "hollerith_constant");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_hollerith_constant(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_data_1stmt_1constant(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "data_stmt_constant");
#endif


	c_action_data_stmt_constant();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_dimension_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jint jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	int carg_3;

#ifdef DEBUG
	printf("hello from %s\n", "dimension_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	carg_3 = (int)jarg_3;

	c_action_dimension_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_dimension_1decl(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "dimension_decl");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_dimension_decl(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_intent_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "intent_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_intent_stmt(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_optional_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "optional_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_optional_stmt(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_parameter_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "parameter_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_parameter_stmt(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_named_1constant_1def_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "named_constant_def_list__begin");
#endif


	c_action_named_constant_def_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_named_1constant_1def_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "named_constant_def_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_named_constant_def_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_named_1constant_1def(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "named_constant_def");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_named_constant_def(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_pointer_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "pointer_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_pointer_stmt(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_pointer_1decl_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "pointer_decl_list__begin");
#endif


	c_action_pointer_decl_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_pointer_1decl_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "pointer_decl_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_pointer_decl_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_pointer_1decl(JNIEnv *env, jobject this, jobject jarg_0, jboolean jarg_1)
{
	Token_t *carg_0 = NULL;
	int carg_1;

#ifdef DEBUG
	printf("hello from %s\n", "pointer_decl");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	carg_1 = (int)jarg_1;

	c_action_pointer_decl(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_cray_1pointer_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "cray_pointer_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_cray_pointer_stmt(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_cray_1pointer_1assoc_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "cray_pointer_assoc_list__begin");
#endif


	c_action_cray_pointer_assoc_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_cray_1pointer_1assoc_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "cray_pointer_assoc_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_cray_pointer_assoc_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_cray_1pointer_1assoc(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "cray_pointer_assoc");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);

	c_action_cray_pointer_assoc(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_protected_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "protected_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_protected_stmt(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_save_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jboolean jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	int carg_3;

#ifdef DEBUG
	printf("hello from %s\n", "save_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	carg_3 = (int)jarg_3;

	c_action_save_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_saved_1entity_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "saved_entity_list__begin");
#endif


	c_action_saved_entity_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_saved_1entity_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "saved_entity_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_saved_entity_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_saved_1entity(JNIEnv *env, jobject this, jobject jarg_0, jboolean jarg_1)
{
	Token_t *carg_0 = NULL;
	int carg_1;

#ifdef DEBUG
	printf("hello from %s\n", "saved_entity");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	carg_1 = (int)jarg_1;

	c_action_saved_entity(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_target_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "target_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_target_stmt(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_target_1decl(JNIEnv *env, jobject this, jobject jarg_0, jboolean jarg_1, jboolean jarg_2)
{
	Token_t *carg_0 = NULL;
	int carg_1;
	int carg_2;

#ifdef DEBUG
	printf("hello from %s\n", "target_decl");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	carg_1 = (int)jarg_1;
	carg_2 = (int)jarg_2;

	c_action_target_decl(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_target_1decl_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "target_decl_list__begin");
#endif


	c_action_target_decl_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_target_1decl_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "target_decl_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_target_decl_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_value_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "value_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_value_stmt(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_volatile_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "volatile_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_volatile_stmt(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_implicit_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jboolean jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	int carg_4;

#ifdef DEBUG
	printf("hello from %s\n", "implicit_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	carg_4 = (int)jarg_4;

	c_action_implicit_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_implicit_1spec(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "implicit_spec");
#endif


	c_action_implicit_spec();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_implicit_1spec_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "implicit_spec_list__begin");
#endif


	c_action_implicit_spec_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_implicit_1spec_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "implicit_spec_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_implicit_spec_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_letter_1spec(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "letter_spec");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);

	c_action_letter_spec(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_letter_1spec_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "letter_spec_list__begin");
#endif


	c_action_letter_spec_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_letter_1spec_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "letter_spec_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_letter_spec_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_namelist_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jint jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	int carg_3;

#ifdef DEBUG
	printf("hello from %s\n", "namelist_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	carg_3 = (int)jarg_3;

	c_action_namelist_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_namelist_1group_1name(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "namelist_group_name");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_namelist_group_name(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_namelist_1group_1object(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "namelist_group_object");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_namelist_group_object(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_namelist_1group_1object_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "namelist_group_object_list__begin");
#endif


	c_action_namelist_group_object_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_namelist_1group_1object_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "namelist_group_object_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_namelist_group_object_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_equivalence_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "equivalence_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_equivalence_stmt(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_equivalence_1set(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "equivalence_set");
#endif


	c_action_equivalence_set();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_equivalence_1set_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "equivalence_set_list__begin");
#endif


	c_action_equivalence_set_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_equivalence_1set_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "equivalence_set_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_equivalence_set_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_equivalence_1object(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "equivalence_object");
#endif


	c_action_equivalence_object();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_equivalence_1object_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "equivalence_object_list__begin");
#endif


	c_action_equivalence_object_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_equivalence_1object_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "equivalence_object_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_equivalence_object_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_common_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jint jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	int carg_3;

#ifdef DEBUG
	printf("hello from %s\n", "common_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	carg_3 = (int)jarg_3;

	c_action_common_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_common_1block_1name(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "common_block_name");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_common_block_name(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_common_1block_1object_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "common_block_object_list__begin");
#endif


	c_action_common_block_object_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_common_1block_1object_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "common_block_object_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_common_block_object_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_common_1block_1object(JNIEnv *env, jobject this, jobject jarg_0, jboolean jarg_1)
{
	Token_t *carg_0 = NULL;
	int carg_1;

#ifdef DEBUG
	printf("hello from %s\n", "common_block_object");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	carg_1 = (int)jarg_1;

	c_action_common_block_object(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_variable(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "variable");
#endif


	c_action_variable();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_designator(JNIEnv *env, jobject this, jboolean jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "designator");
#endif

	carg_0 = (int)jarg_0;

	c_action_designator(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_designator_1or_1func_1ref(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "designator_or_func_ref");
#endif


	c_action_designator_or_func_ref();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_substring_1range_1or_1arg_1list(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "substring_range_or_arg_list");
#endif


	c_action_substring_range_or_arg_list();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_substr_1range_1or_1arg_1list_1suffix(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "substr_range_or_arg_list_suffix");
#endif


	c_action_substr_range_or_arg_list_suffix();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_logical_1variable(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "logical_variable");
#endif


	c_action_logical_variable();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_default_1logical_1variable(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "default_logical_variable");
#endif


	c_action_default_logical_variable();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_scalar_1default_1logical_1variable(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "scalar_default_logical_variable");
#endif


	c_action_scalar_default_logical_variable();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_char_1variable(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "char_variable");
#endif


	c_action_char_variable();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_default_1char_1variable(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "default_char_variable");
#endif


	c_action_default_char_variable();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_scalar_1default_1char_1variable(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "scalar_default_char_variable");
#endif


	c_action_scalar_default_char_variable();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_int_1variable(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "int_variable");
#endif


	c_action_int_variable();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_substring_1range(JNIEnv *env, jobject this, jboolean jarg_0, jboolean jarg_1)
{
	int carg_0;
	int carg_1;

#ifdef DEBUG
	printf("hello from %s\n", "substring_range");
#endif

	carg_0 = (int)jarg_0;
	carg_1 = (int)jarg_1;

	c_action_substring_range(carg_0, carg_1);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_data_1ref(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "data_ref");
#endif

	carg_0 = (int)jarg_0;

	c_action_data_ref(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_part_1ref(JNIEnv *env, jobject this, jobject jarg_0, jboolean jarg_1, jboolean jarg_2)
{
	Token_t *carg_0 = NULL;
	int carg_1;
	int carg_2;

#ifdef DEBUG
	printf("hello from %s\n", "part_ref");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	carg_1 = (int)jarg_1;
	carg_2 = (int)jarg_2;

	c_action_part_ref(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_section_1subscript(JNIEnv *env, jobject this, jboolean jarg_0, jboolean jarg_1, jboolean jarg_2, jboolean jarg_3)
{
	int carg_0;
	int carg_1;
	int carg_2;
	int carg_3;

#ifdef DEBUG
	printf("hello from %s\n", "section_subscript");
#endif

	carg_0 = (int)jarg_0;
	carg_1 = (int)jarg_1;
	carg_2 = (int)jarg_2;
	carg_3 = (int)jarg_3;

	c_action_section_subscript(carg_0, carg_1, carg_2, carg_3);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_section_1subscript_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "section_subscript_list__begin");
#endif


	c_action_section_subscript_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_section_1subscript_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "section_subscript_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_section_subscript_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_vector_1subscript(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "vector_subscript");
#endif


	c_action_vector_subscript();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_allocate_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jboolean jarg_3, jboolean jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	int carg_3;
	int carg_4;

#ifdef DEBUG
	printf("hello from %s\n", "allocate_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	carg_3 = (int)jarg_3;
	carg_4 = (int)jarg_4;

	c_action_allocate_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_image_1selector(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "image_selector");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);

	c_action_image_selector(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_alloc_1opt(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "alloc_opt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_alloc_opt(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_alloc_1opt_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "alloc_opt_list__begin");
#endif


	c_action_alloc_opt_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_alloc_1opt_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "alloc_opt_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_alloc_opt_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_cosubscript_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "cosubscript_list__begin");
#endif


	c_action_cosubscript_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_cosubscript_1list(JNIEnv *env, jobject this, jint jarg_0, jobject jarg_1)
{
	int carg_0;
	Token_t *carg_1 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "cosubscript_list");
#endif

	carg_0 = (int)jarg_0;
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);

	c_action_cosubscript_list(carg_0, carg_1);

	if(carg_1 != NULL)
		register_token(carg_1);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_allocation(JNIEnv *env, jobject this, jboolean jarg_0, jboolean jarg_1)
{
	int carg_0;
	int carg_1;

#ifdef DEBUG
	printf("hello from %s\n", "allocation");
#endif

	carg_0 = (int)jarg_0;
	carg_1 = (int)jarg_1;

	c_action_allocation(carg_0, carg_1);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_allocation_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "allocation_list__begin");
#endif


	c_action_allocation_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_allocation_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "allocation_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_allocation_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_allocate_1object(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "allocate_object");
#endif


	c_action_allocate_object();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_allocate_1object_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "allocate_object_list__begin");
#endif


	c_action_allocate_object_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_allocate_1object_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "allocate_object_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_allocate_object_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_allocate_1shape_1spec(JNIEnv *env, jobject this, jboolean jarg_0, jboolean jarg_1)
{
	int carg_0;
	int carg_1;

#ifdef DEBUG
	printf("hello from %s\n", "allocate_shape_spec");
#endif

	carg_0 = (int)jarg_0;
	carg_1 = (int)jarg_1;

	c_action_allocate_shape_spec(carg_0, carg_1);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_allocate_1shape_1spec_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "allocate_shape_spec_list__begin");
#endif


	c_action_allocate_shape_spec_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_allocate_1shape_1spec_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "allocate_shape_spec_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_allocate_shape_spec_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_nullify_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "nullify_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_nullify_stmt(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_pointer_1object(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "pointer_object");
#endif


	c_action_pointer_object();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_pointer_1object_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "pointer_object_list__begin");
#endif


	c_action_pointer_object_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_pointer_1object_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "pointer_object_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_pointer_object_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_deallocate_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jboolean jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	int carg_3;

#ifdef DEBUG
	printf("hello from %s\n", "deallocate_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	carg_3 = (int)jarg_3;

	c_action_deallocate_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_dealloc_1opt(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "dealloc_opt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_dealloc_opt(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_dealloc_1opt_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "dealloc_opt_list__begin");
#endif


	c_action_dealloc_opt_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_dealloc_1opt_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "dealloc_opt_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_dealloc_opt_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_allocate_1coarray_1spec(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "allocate_coarray_spec");
#endif


	c_action_allocate_coarray_spec();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_allocate_1coshape_1spec(JNIEnv *env, jobject this, jboolean jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "allocate_coshape_spec");
#endif

	carg_0 = (int)jarg_0;

	c_action_allocate_coshape_spec(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_allocate_1coshape_1spec_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "allocate_coshape_spec_list__begin");
#endif


	c_action_allocate_coshape_spec_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_allocate_1coshape_1spec_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "allocate_coshape_spec_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_allocate_coshape_spec_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_primary(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "primary");
#endif


	c_action_primary();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_level_11_1expr(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "level_1_expr");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_level_1_expr(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_defined_1unary_1op(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "defined_unary_op");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_defined_unary_op(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_power_1operand(JNIEnv *env, jobject this, jboolean jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "power_operand");
#endif

	carg_0 = (int)jarg_0;

	c_action_power_operand(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_power_1operand_1_1power_1op(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "power_operand__power_op");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_power_operand__power_op(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_mult_1operand(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "mult_operand");
#endif

	carg_0 = (int)jarg_0;

	c_action_mult_operand(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_mult_1operand_1_1mult_1op(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "mult_operand__mult_op");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_mult_operand__mult_op(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_signed_1operand(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "signed_operand");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_signed_operand(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_add_1operand(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "add_operand");
#endif

	carg_0 = (int)jarg_0;

	c_action_add_operand(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_add_1operand_1_1add_1op(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "add_operand__add_op");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_add_operand__add_op(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_level_12_1expr(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "level_2_expr");
#endif

	carg_0 = (int)jarg_0;

	c_action_level_2_expr(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_power_1op(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "power_op");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_power_op(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_mult_1op(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "mult_op");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_mult_op(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_add_1op(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "add_op");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_add_op(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_level_13_1expr(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "level_3_expr");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_level_3_expr(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_concat_1op(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "concat_op");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_concat_op(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_rel_1op(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "rel_op");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_rel_op(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_and_1operand(JNIEnv *env, jobject this, jboolean jarg_0, jint jarg_1)
{
	int carg_0;
	int carg_1;

#ifdef DEBUG
	printf("hello from %s\n", "and_operand");
#endif

	carg_0 = (int)jarg_0;
	carg_1 = (int)jarg_1;

	c_action_and_operand(carg_0, carg_1);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_and_1operand_1_1not_1op(JNIEnv *env, jobject this, jboolean jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "and_operand__not_op");
#endif

	carg_0 = (int)jarg_0;

	c_action_and_operand__not_op(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_or_1operand(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "or_operand");
#endif

	carg_0 = (int)jarg_0;

	c_action_or_operand(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_equiv_1operand(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "equiv_operand");
#endif

	carg_0 = (int)jarg_0;

	c_action_equiv_operand(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_equiv_1operand_1_1equiv_1op(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "equiv_operand__equiv_op");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_equiv_operand__equiv_op(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_level_15_1expr(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "level_5_expr");
#endif

	carg_0 = (int)jarg_0;

	c_action_level_5_expr(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_level_15_1expr_1_1defined_1binary_1op(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "level_5_expr__defined_binary_op");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_level_5_expr__defined_binary_op(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_not_1op(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "not_op");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_not_op(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_and_1op(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "and_op");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_and_op(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_or_1op(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "or_op");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_or_op(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_equiv_1op(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "equiv_op");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_equiv_op(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_defined_1binary_1op(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "defined_binary_op");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_defined_binary_op(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_assignment_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "assignment_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);

	c_action_assignment_stmt(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_pointer_1assignment_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jboolean jarg_2, jboolean jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	int carg_2;
	int carg_3;

#ifdef DEBUG
	printf("hello from %s\n", "pointer_assignment_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	carg_2 = (int)jarg_2;
	carg_3 = (int)jarg_3;

	c_action_pointer_assignment_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_data_1pointer_1object(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "data_pointer_object");
#endif


	c_action_data_pointer_object();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_bounds_1spec(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "bounds_spec");
#endif


	c_action_bounds_spec();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_bounds_1spec_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "bounds_spec_list__begin");
#endif


	c_action_bounds_spec_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_bounds_1spec_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "bounds_spec_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_bounds_spec_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_bounds_1remapping(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "bounds_remapping");
#endif


	c_action_bounds_remapping();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_bounds_1remapping_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "bounds_remapping_list__begin");
#endif


	c_action_bounds_remapping_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_bounds_1remapping_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "bounds_remapping_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_bounds_remapping_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_proc_1pointer_1object(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "proc_pointer_object");
#endif


	c_action_proc_pointer_object();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_where_1stmt_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "where_stmt__begin");
#endif


	c_action_where_stmt__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_where_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "where_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);

	c_action_where_stmt(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_where_1construct(JNIEnv *env, jobject this, jint jarg_0, jboolean jarg_1, jboolean jarg_2)
{
	int carg_0;
	int carg_1;
	int carg_2;

#ifdef DEBUG
	printf("hello from %s\n", "where_construct");
#endif

	carg_0 = (int)jarg_0;
	carg_1 = (int)jarg_1;
	carg_2 = (int)jarg_2;

	c_action_where_construct(carg_0, carg_1, carg_2);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_where_1construct_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "where_construct_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_where_construct_stmt(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_where_1body_1construct(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "where_body_construct");
#endif


	c_action_where_body_construct();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_masked_1elsewhere_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jobject jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	Token_t *carg_4 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "masked_elsewhere_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	if(jarg_4 != NULL)
		carg_4 = convert_Java_token(env, jarg_4);

	c_action_masked_elsewhere_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
	if(carg_4 != NULL)
		register_token(carg_4);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_masked_1elsewhere_1stmt_1_1end(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "masked_elsewhere_stmt__end");
#endif

	carg_0 = (int)jarg_0;

	c_action_masked_elsewhere_stmt__end(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_elsewhere_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jobject jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	Token_t *carg_4 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "elsewhere_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	if(jarg_4 != NULL)
		carg_4 = convert_Java_token(env, jarg_4);

	c_action_elsewhere_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
	if(carg_4 != NULL)
		register_token(carg_4);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_elsewhere_1stmt_1_1end(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "elsewhere_stmt__end");
#endif

	carg_0 = (int)jarg_0;

	c_action_elsewhere_stmt__end(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_end_1where_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jobject jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	Token_t *carg_4 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "end_where_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	if(jarg_4 != NULL)
		carg_4 = convert_Java_token(env, jarg_4);

	c_action_end_where_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
	if(carg_4 != NULL)
		register_token(carg_4);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_forall_1construct(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "forall_construct");
#endif


	c_action_forall_construct();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_forall_1construct_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "forall_construct_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);

	c_action_forall_construct_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_forall_1header(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "forall_header");
#endif


	c_action_forall_header();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_forall_1triplet_1spec(JNIEnv *env, jobject this, jobject jarg_0, jboolean jarg_1)
{
	Token_t *carg_0 = NULL;
	int carg_1;

#ifdef DEBUG
	printf("hello from %s\n", "forall_triplet_spec");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	carg_1 = (int)jarg_1;

	c_action_forall_triplet_spec(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_forall_1triplet_1spec_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "forall_triplet_spec_list__begin");
#endif


	c_action_forall_triplet_spec_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_forall_1triplet_1spec_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "forall_triplet_spec_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_forall_triplet_spec_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_forall_1body_1construct(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "forall_body_construct");
#endif


	c_action_forall_body_construct();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_forall_1assignment_1stmt(JNIEnv *env, jobject this, jboolean jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "forall_assignment_stmt");
#endif

	carg_0 = (int)jarg_0;

	c_action_forall_assignment_stmt(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_end_1forall_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jobject jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	Token_t *carg_4 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "end_forall_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	if(jarg_4 != NULL)
		carg_4 = convert_Java_token(env, jarg_4);

	c_action_end_forall_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
	if(carg_4 != NULL)
		register_token(carg_4);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_forall_1stmt_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "forall_stmt__begin");
#endif


	c_action_forall_stmt__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_forall_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "forall_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);

	c_action_forall_stmt(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_if_1construct(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "if_construct");
#endif


	c_action_if_construct();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_if_1then_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jobject jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	Token_t *carg_4 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "if_then_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	if(jarg_4 != NULL)
		carg_4 = convert_Java_token(env, jarg_4);

	c_action_if_then_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
	if(carg_4 != NULL)
		register_token(carg_4);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_else_1if_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jobject jarg_4, jobject jarg_5)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	Token_t *carg_4 = NULL;
	Token_t *carg_5 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "else_if_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	if(jarg_4 != NULL)
		carg_4 = convert_Java_token(env, jarg_4);
	if(jarg_5 != NULL)
		carg_5 = convert_Java_token(env, jarg_5);

	c_action_else_if_stmt(carg_0, carg_1, carg_2, carg_3, carg_4, carg_5);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
	if(carg_4 != NULL)
		register_token(carg_4);
	if(carg_5 != NULL)
		register_token(carg_5);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_else_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "else_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);

	c_action_else_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_end_1if_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jobject jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	Token_t *carg_4 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "end_if_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	if(jarg_4 != NULL)
		carg_4 = convert_Java_token(env, jarg_4);

	c_action_end_if_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
	if(carg_4 != NULL)
		register_token(carg_4);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_if_1stmt_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "if_stmt__begin");
#endif


	c_action_if_stmt__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_if_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "if_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);

	c_action_if_stmt(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_block_1construct(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "block_construct");
#endif


	c_action_block_construct();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_specification_1part_1and_1block(JNIEnv *env, jobject this, jint jarg_0, jint jarg_1, jint jarg_2)
{
	int carg_0;
	int carg_1;
	int carg_2;

#ifdef DEBUG
	printf("hello from %s\n", "specification_part_and_block");
#endif

	carg_0 = (int)jarg_0;
	carg_1 = (int)jarg_1;
	carg_2 = (int)jarg_2;

	c_action_specification_part_and_block(carg_0, carg_1, carg_2);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_block_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "block_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);

	c_action_block_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_end_1block_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jobject jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	Token_t *carg_4 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "end_block_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	if(jarg_4 != NULL)
		carg_4 = convert_Java_token(env, jarg_4);

	c_action_end_block_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
	if(carg_4 != NULL)
		register_token(carg_4);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_critical_1construct(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "critical_construct");
#endif


	c_action_critical_construct();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_critical_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "critical_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);

	c_action_critical_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_end_1critical_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jobject jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	Token_t *carg_4 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "end_critical_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	if(jarg_4 != NULL)
		carg_4 = convert_Java_token(env, jarg_4);

	c_action_end_critical_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
	if(carg_4 != NULL)
		register_token(carg_4);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_case_1construct(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "case_construct");
#endif


	c_action_case_construct();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_select_1case_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jobject jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	Token_t *carg_4 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "select_case_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	if(jarg_4 != NULL)
		carg_4 = convert_Java_token(env, jarg_4);

	c_action_select_case_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
	if(carg_4 != NULL)
		register_token(carg_4);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_case_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "case_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);

	c_action_case_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_end_1select_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jobject jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	Token_t *carg_4 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "end_select_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	if(jarg_4 != NULL)
		carg_4 = convert_Java_token(env, jarg_4);

	c_action_end_select_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
	if(carg_4 != NULL)
		register_token(carg_4);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_case_1selector(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "case_selector");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_case_selector(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_case_1value_1range(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "case_value_range");
#endif


	c_action_case_value_range();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_case_1value_1range_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "case_value_range_list__begin");
#endif


	c_action_case_value_range_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_case_1value_1range_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "case_value_range_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_case_value_range_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_case_1value_1range_1suffix(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "case_value_range_suffix");
#endif


	c_action_case_value_range_suffix();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_case_1value(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "case_value");
#endif


	c_action_case_value();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_associate_1construct(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "associate_construct");
#endif


	c_action_associate_construct();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_associate_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "associate_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);

	c_action_associate_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_association_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "association_list__begin");
#endif


	c_action_association_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_association_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "association_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_association_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_association(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "association");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_association(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_selector(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "selector");
#endif


	c_action_selector();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_end_1associate_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jobject jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	Token_t *carg_4 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "end_associate_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	if(jarg_4 != NULL)
		carg_4 = convert_Java_token(env, jarg_4);

	c_action_end_associate_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
	if(carg_4 != NULL)
		register_token(carg_4);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_select_1type_1construct(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "select_type_construct");
#endif


	c_action_select_type_construct();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_select_1type_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "select_type_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);

	c_action_select_type_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_select_1type(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "select_type");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);

	c_action_select_type(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_type_1guard_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jobject jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	Token_t *carg_4 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "type_guard_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	if(jarg_4 != NULL)
		carg_4 = convert_Java_token(env, jarg_4);

	c_action_type_guard_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
	if(carg_4 != NULL)
		register_token(carg_4);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_end_1select_1type_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jobject jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	Token_t *carg_4 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "end_select_type_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	if(jarg_4 != NULL)
		carg_4 = convert_Java_token(env, jarg_4);

	c_action_end_select_type_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
	if(carg_4 != NULL)
		register_token(carg_4);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_do_1construct(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "do_construct");
#endif


	c_action_do_construct();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_block_1do_1construct(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "block_do_construct");
#endif


	c_action_block_do_construct();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_do_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jobject jarg_4, jboolean jarg_5)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	Token_t *carg_4 = NULL;
	int carg_5;

#ifdef DEBUG
	printf("hello from %s\n", "do_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	if(jarg_4 != NULL)
		carg_4 = convert_Java_token(env, jarg_4);
	carg_5 = (int)jarg_5;

	c_action_do_stmt(carg_0, carg_1, carg_2, carg_3, carg_4, carg_5);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
	if(carg_4 != NULL)
		register_token(carg_4);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_label_1do_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jobject jarg_4, jboolean jarg_5)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	Token_t *carg_4 = NULL;
	int carg_5;

#ifdef DEBUG
	printf("hello from %s\n", "label_do_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	if(jarg_4 != NULL)
		carg_4 = convert_Java_token(env, jarg_4);
	carg_5 = (int)jarg_5;

	c_action_label_do_stmt(carg_0, carg_1, carg_2, carg_3, carg_4, carg_5);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
	if(carg_4 != NULL)
		register_token(carg_4);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_loop_1control(JNIEnv *env, jobject this, jobject jarg_0, jint jarg_1, jboolean jarg_2)
{
	Token_t *carg_0 = NULL;
	int carg_1;
	int carg_2;

#ifdef DEBUG
	printf("hello from %s\n", "loop_control");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	carg_1 = (int)jarg_1;
	carg_2 = (int)jarg_2;

	c_action_loop_control(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_do_1variable(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "do_variable");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_do_variable(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_end_1do(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "end_do");
#endif


	c_action_end_do();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_end_1do_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jobject jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	Token_t *carg_4 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "end_do_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	if(jarg_4 != NULL)
		carg_4 = convert_Java_token(env, jarg_4);

	c_action_end_do_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
	if(carg_4 != NULL)
		register_token(carg_4);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_do_1term_1action_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jobject jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	Token_t *carg_4 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "do_term_action_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	if(jarg_4 != NULL)
		carg_4 = convert_Java_token(env, jarg_4);

	c_action_do_term_action_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
	if(carg_4 != NULL)
		register_token(carg_4);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_cycle_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "cycle_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);

	c_action_cycle_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_exit_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "exit_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);

	c_action_exit_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_goto_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "goto_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);

	c_action_goto_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_computed_1goto_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "computed_goto_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);

	c_action_computed_goto_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_assign_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jobject jarg_4, jobject jarg_5)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	Token_t *carg_4 = NULL;
	Token_t *carg_5 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "assign_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	if(jarg_4 != NULL)
		carg_4 = convert_Java_token(env, jarg_4);
	if(jarg_5 != NULL)
		carg_5 = convert_Java_token(env, jarg_5);

	c_action_assign_stmt(carg_0, carg_1, carg_2, carg_3, carg_4, carg_5);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
	if(carg_4 != NULL)
		register_token(carg_4);
	if(carg_5 != NULL)
		register_token(carg_5);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_assigned_1goto_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jobject jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	Token_t *carg_4 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "assigned_goto_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	if(jarg_4 != NULL)
		carg_4 = convert_Java_token(env, jarg_4);

	c_action_assigned_goto_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
	if(carg_4 != NULL)
		register_token(carg_4);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_stmt_1label_1list(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "stmt_label_list");
#endif


	c_action_stmt_label_list();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_pause_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "pause_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);

	c_action_pause_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_arithmetic_1if_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jobject jarg_4, jobject jarg_5)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	Token_t *carg_4 = NULL;
	Token_t *carg_5 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "arithmetic_if_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	if(jarg_4 != NULL)
		carg_4 = convert_Java_token(env, jarg_4);
	if(jarg_5 != NULL)
		carg_5 = convert_Java_token(env, jarg_5);

	c_action_arithmetic_if_stmt(carg_0, carg_1, carg_2, carg_3, carg_4, carg_5);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
	if(carg_4 != NULL)
		register_token(carg_4);
	if(carg_5 != NULL)
		register_token(carg_5);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_continue_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "continue_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_continue_stmt(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_stop_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jboolean jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	int carg_3;

#ifdef DEBUG
	printf("hello from %s\n", "stop_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	carg_3 = (int)jarg_3;

	c_action_stop_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_stop_1code(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "stop_code");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_stop_code(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_errorstop_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jboolean jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	int carg_4;

#ifdef DEBUG
	printf("hello from %s\n", "errorstop_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	carg_4 = (int)jarg_4;

	c_action_errorstop_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_sync_1all_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jboolean jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	int carg_4;

#ifdef DEBUG
	printf("hello from %s\n", "sync_all_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	carg_4 = (int)jarg_4;

	c_action_sync_all_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_sync_1stat(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "sync_stat");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_sync_stat(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_sync_1stat_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "sync_stat_list__begin");
#endif


	c_action_sync_stat_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_sync_1stat_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "sync_stat_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_sync_stat_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_sync_1images_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jboolean jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	int carg_4;

#ifdef DEBUG
	printf("hello from %s\n", "sync_images_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	carg_4 = (int)jarg_4;

	c_action_sync_images_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_image_1set(JNIEnv *env, jobject this, jobject jarg_0, jboolean jarg_1)
{
	Token_t *carg_0 = NULL;
	int carg_1;

#ifdef DEBUG
	printf("hello from %s\n", "image_set");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	carg_1 = (int)jarg_1;

	c_action_image_set(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_sync_1memory_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jboolean jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	int carg_4;

#ifdef DEBUG
	printf("hello from %s\n", "sync_memory_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	carg_4 = (int)jarg_4;

	c_action_sync_memory_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_lock_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jboolean jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	int carg_3;

#ifdef DEBUG
	printf("hello from %s\n", "lock_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	carg_3 = (int)jarg_3;

	c_action_lock_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_lock_1stat(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "lock_stat");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_lock_stat(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_lock_1stat_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "lock_stat_list__begin");
#endif


	c_action_lock_stat_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_lock_1stat_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "lock_stat_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_lock_stat_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_unlock_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jboolean jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	int carg_3;

#ifdef DEBUG
	printf("hello from %s\n", "unlock_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	carg_3 = (int)jarg_3;

	c_action_unlock_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_lock_1variable(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "lock_variable");
#endif


	c_action_lock_variable();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_scalar_1char_1constant(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "scalar_char_constant");
#endif


	c_action_scalar_char_constant();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_io_1unit(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "io_unit");
#endif


	c_action_io_unit();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_file_1unit_1number(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "file_unit_number");
#endif


	c_action_file_unit_number();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_open_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "open_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_open_stmt(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_connect_1spec(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "connect_spec");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_connect_spec(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_connect_1spec_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "connect_spec_list__begin");
#endif


	c_action_connect_spec_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_connect_1spec_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "connect_spec_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_connect_spec_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_close_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "close_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_close_stmt(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_close_1spec(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "close_spec");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_close_spec(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_close_1spec_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "close_spec_list__begin");
#endif


	c_action_close_spec_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_close_1spec_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "close_spec_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_close_spec_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_read_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jboolean jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	int carg_3;

#ifdef DEBUG
	printf("hello from %s\n", "read_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	carg_3 = (int)jarg_3;

	c_action_read_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_write_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jboolean jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	int carg_3;

#ifdef DEBUG
	printf("hello from %s\n", "write_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	carg_3 = (int)jarg_3;

	c_action_write_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_print_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jboolean jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	int carg_3;

#ifdef DEBUG
	printf("hello from %s\n", "print_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	carg_3 = (int)jarg_3;

	c_action_print_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_io_1control_1spec(JNIEnv *env, jobject this, jboolean jarg_0, jobject jarg_1, jboolean jarg_2)
{
	int carg_0;
	Token_t *carg_1 = NULL;
	int carg_2;

#ifdef DEBUG
	printf("hello from %s\n", "io_control_spec");
#endif

	carg_0 = (int)jarg_0;
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	carg_2 = (int)jarg_2;

	c_action_io_control_spec(carg_0, carg_1, carg_2);

	if(carg_1 != NULL)
		register_token(carg_1);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_io_1control_1spec_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "io_control_spec_list__begin");
#endif


	c_action_io_control_spec_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_io_1control_1spec_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "io_control_spec_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_io_control_spec_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_input_1item(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "input_item");
#endif


	c_action_input_item();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_input_1item_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "input_item_list__begin");
#endif


	c_action_input_item_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_input_1item_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "input_item_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_input_item_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_output_1item(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "output_item");
#endif


	c_action_output_item();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_output_1item_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "output_item_list__begin");
#endif


	c_action_output_item_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_output_1item_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "output_item_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_output_item_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_io_1implied_1do(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "io_implied_do");
#endif


	c_action_io_implied_do();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_io_1implied_1do_1object(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "io_implied_do_object");
#endif


	c_action_io_implied_do_object();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_io_1implied_1do_1control(JNIEnv *env, jobject this, jboolean jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "io_implied_do_control");
#endif

	carg_0 = (int)jarg_0;

	c_action_io_implied_do_control(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_dtv_1type_1spec(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "dtv_type_spec");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_dtv_type_spec(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_wait_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "wait_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_wait_stmt(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_wait_1spec(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "wait_spec");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_wait_spec(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_wait_1spec_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "wait_spec_list__begin");
#endif


	c_action_wait_spec_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_wait_1spec_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "wait_spec_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_wait_spec_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_backspace_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jboolean jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	int carg_3;

#ifdef DEBUG
	printf("hello from %s\n", "backspace_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	carg_3 = (int)jarg_3;

	c_action_backspace_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_endfile_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jboolean jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	int carg_4;

#ifdef DEBUG
	printf("hello from %s\n", "endfile_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	carg_4 = (int)jarg_4;

	c_action_endfile_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_rewind_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jboolean jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	int carg_3;

#ifdef DEBUG
	printf("hello from %s\n", "rewind_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	carg_3 = (int)jarg_3;

	c_action_rewind_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_position_1spec(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "position_spec");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_position_spec(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_position_1spec_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "position_spec_list__begin");
#endif


	c_action_position_spec_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_position_1spec_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "position_spec_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_position_spec_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_flush_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jboolean jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	int carg_3;

#ifdef DEBUG
	printf("hello from %s\n", "flush_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	carg_3 = (int)jarg_3;

	c_action_flush_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_flush_1spec(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "flush_spec");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_flush_spec(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_flush_1spec_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "flush_spec_list__begin");
#endif


	c_action_flush_spec_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_flush_1spec_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "flush_spec_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_flush_spec_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_inquire_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jboolean jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	int carg_4;

#ifdef DEBUG
	printf("hello from %s\n", "inquire_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	carg_4 = (int)jarg_4;

	c_action_inquire_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_inquire_1spec(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "inquire_spec");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_inquire_spec(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_inquire_1spec_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "inquire_spec_list__begin");
#endif


	c_action_inquire_spec_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_inquire_1spec_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "inquire_spec_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_inquire_spec_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_format_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "format_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_format_stmt(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_format_1specification(JNIEnv *env, jobject this, jboolean jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "format_specification");
#endif

	carg_0 = (int)jarg_0;

	c_action_format_specification(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_format_1item(JNIEnv *env, jobject this, jobject jarg_0, jboolean jarg_1)
{
	Token_t *carg_0 = NULL;
	int carg_1;

#ifdef DEBUG
	printf("hello from %s\n", "format_item");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	carg_1 = (int)jarg_1;

	c_action_format_item(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_format_1item_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "format_item_list__begin");
#endif


	c_action_format_item_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_format_1item_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "format_item_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_format_item_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_v_1list_1part(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "v_list_part");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);

	c_action_v_list_part(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_v_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "v_list__begin");
#endif


	c_action_v_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_v_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "v_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_v_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_main_1program_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "main_program__begin");
#endif


	c_action_main_program__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_main_1program(JNIEnv *env, jobject this, jboolean jarg_0, jboolean jarg_1, jboolean jarg_2)
{
	int carg_0;
	int carg_1;
	int carg_2;

#ifdef DEBUG
	printf("hello from %s\n", "main_program");
#endif

	carg_0 = (int)jarg_0;
	carg_1 = (int)jarg_1;
	carg_2 = (int)jarg_2;

	c_action_main_program(carg_0, carg_1, carg_2);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_ext_1function_1subprogram(JNIEnv *env, jobject this, jboolean jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "ext_function_subprogram");
#endif

	carg_0 = (int)jarg_0;

	c_action_ext_function_subprogram(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_program_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "program_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);

	c_action_program_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_end_1program_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jobject jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	Token_t *carg_4 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "end_program_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	if(jarg_4 != NULL)
		carg_4 = convert_Java_token(env, jarg_4);

	c_action_end_program_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
	if(carg_4 != NULL)
		register_token(carg_4);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_module(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "module");
#endif


	c_action_module();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_module_1stmt_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "module_stmt__begin");
#endif


	c_action_module_stmt__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_module_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "module_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);

	c_action_module_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_end_1module_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jobject jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	Token_t *carg_4 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "end_module_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	if(jarg_4 != NULL)
		carg_4 = convert_Java_token(env, jarg_4);

	c_action_end_module_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
	if(carg_4 != NULL)
		register_token(carg_4);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_module_1subprogram_1part(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "module_subprogram_part");
#endif

	carg_0 = (int)jarg_0;

	c_action_module_subprogram_part(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_module_1subprogram(JNIEnv *env, jobject this, jboolean jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "module_subprogram");
#endif

	carg_0 = (int)jarg_0;

	c_action_module_subprogram(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_use_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jobject jarg_4, jboolean jarg_5, jboolean jarg_6, jboolean jarg_7)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	Token_t *carg_4 = NULL;
	int carg_5;
	int carg_6;
	int carg_7;

#ifdef DEBUG
	printf("hello from %s\n", "use_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	if(jarg_4 != NULL)
		carg_4 = convert_Java_token(env, jarg_4);
	carg_5 = (int)jarg_5;
	carg_6 = (int)jarg_6;
	carg_7 = (int)jarg_7;

	c_action_use_stmt(carg_0, carg_1, carg_2, carg_3, carg_4, carg_5, carg_6, carg_7);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
	if(carg_4 != NULL)
		register_token(carg_4);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_module_1nature(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "module_nature");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_module_nature(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_rename_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "rename_list__begin");
#endif


	c_action_rename_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_rename_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "rename_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_rename_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_only(JNIEnv *env, jobject this, jboolean jarg_0, jboolean jarg_1, jboolean jarg_2)
{
	int carg_0;
	int carg_1;
	int carg_2;

#ifdef DEBUG
	printf("hello from %s\n", "only");
#endif

	carg_0 = (int)jarg_0;
	carg_1 = (int)jarg_1;
	carg_2 = (int)jarg_2;

	c_action_only(carg_0, carg_1, carg_2);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_only_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "only_list__begin");
#endif


	c_action_only_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_only_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "only_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_only_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_submodule(JNIEnv *env, jobject this, jboolean jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "submodule");
#endif

	carg_0 = (int)jarg_0;

	c_action_submodule(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_submodule_1stmt_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "submodule_stmt__begin");
#endif


	c_action_submodule_stmt__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_submodule_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "submodule_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);

	c_action_submodule_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_parent_1identifier(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "parent_identifier");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);

	c_action_parent_identifier(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_end_1submodule_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jobject jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	Token_t *carg_4 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "end_submodule_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	if(jarg_4 != NULL)
		carg_4 = convert_Java_token(env, jarg_4);

	c_action_end_submodule_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
	if(carg_4 != NULL)
		register_token(carg_4);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_block_1data(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "block_data");
#endif


	c_action_block_data();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_block_1data_1stmt_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "block_data_stmt__begin");
#endif


	c_action_block_data_stmt__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_block_1data_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jobject jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	Token_t *carg_4 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "block_data_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	if(jarg_4 != NULL)
		carg_4 = convert_Java_token(env, jarg_4);

	c_action_block_data_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
	if(carg_4 != NULL)
		register_token(carg_4);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_end_1block_1data_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jobject jarg_4, jobject jarg_5)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	Token_t *carg_4 = NULL;
	Token_t *carg_5 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "end_block_data_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	if(jarg_4 != NULL)
		carg_4 = convert_Java_token(env, jarg_4);
	if(jarg_5 != NULL)
		carg_5 = convert_Java_token(env, jarg_5);

	c_action_end_block_data_stmt(carg_0, carg_1, carg_2, carg_3, carg_4, carg_5);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
	if(carg_4 != NULL)
		register_token(carg_4);
	if(carg_5 != NULL)
		register_token(carg_5);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_interface_1block(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "interface_block");
#endif


	c_action_interface_block();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_interface_1specification(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "interface_specification");
#endif


	c_action_interface_specification();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_interface_1stmt_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "interface_stmt__begin");
#endif


	c_action_interface_stmt__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_interface_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jboolean jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	int carg_4;

#ifdef DEBUG
	printf("hello from %s\n", "interface_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	carg_4 = (int)jarg_4;

	c_action_interface_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_end_1interface_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jboolean jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	int carg_4;

#ifdef DEBUG
	printf("hello from %s\n", "end_interface_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	carg_4 = (int)jarg_4;

	c_action_end_interface_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_interface_1body(JNIEnv *env, jobject this, jboolean jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "interface_body");
#endif

	carg_0 = (int)jarg_0;

	c_action_interface_body(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_procedure_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "procedure_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);

	c_action_procedure_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_generic_1spec(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jint jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	int carg_2;

#ifdef DEBUG
	printf("hello from %s\n", "generic_spec");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	carg_2 = (int)jarg_2;

	c_action_generic_spec(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_dtio_1generic_1spec(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jint jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	int carg_2;

#ifdef DEBUG
	printf("hello from %s\n", "dtio_generic_spec");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	carg_2 = (int)jarg_2;

	c_action_dtio_generic_spec(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_import_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jboolean jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	int carg_3;

#ifdef DEBUG
	printf("hello from %s\n", "import_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	carg_3 = (int)jarg_3;

	c_action_import_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_external_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "external_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_external_stmt(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_procedure_1declaration_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jboolean jarg_3, jint jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	int carg_3;
	int carg_4;

#ifdef DEBUG
	printf("hello from %s\n", "procedure_declaration_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	carg_3 = (int)jarg_3;
	carg_4 = (int)jarg_4;

	c_action_procedure_declaration_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_proc_1interface(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "proc_interface");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_proc_interface(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_proc_1attr_1spec(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jint jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	int carg_2;

#ifdef DEBUG
	printf("hello from %s\n", "proc_attr_spec");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	carg_2 = (int)jarg_2;

	c_action_proc_attr_spec(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_proc_1decl(JNIEnv *env, jobject this, jobject jarg_0, jboolean jarg_1)
{
	Token_t *carg_0 = NULL;
	int carg_1;

#ifdef DEBUG
	printf("hello from %s\n", "proc_decl");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	carg_1 = (int)jarg_1;

	c_action_proc_decl(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_proc_1decl_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "proc_decl_list__begin");
#endif


	c_action_proc_decl_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_proc_1decl_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "proc_decl_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_proc_decl_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_intrinsic_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "intrinsic_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_intrinsic_stmt(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_function_1reference(JNIEnv *env, jobject this, jboolean jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "function_reference");
#endif

	carg_0 = (int)jarg_0;

	c_action_function_reference(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_call_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jboolean jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	int carg_3;

#ifdef DEBUG
	printf("hello from %s\n", "call_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	carg_3 = (int)jarg_3;

	c_action_call_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_procedure_1designator(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "procedure_designator");
#endif


	c_action_procedure_designator();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_actual_1arg_1spec(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "actual_arg_spec");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_actual_arg_spec(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_actual_1arg_1spec_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "actual_arg_spec_list__begin");
#endif


	c_action_actual_arg_spec_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_actual_1arg_1spec_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "actual_arg_spec_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_actual_arg_spec_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_actual_1arg(JNIEnv *env, jobject this, jboolean jarg_0, jobject jarg_1)
{
	int carg_0;
	Token_t *carg_1 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "actual_arg");
#endif

	carg_0 = (int)jarg_0;
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);

	c_action_actual_arg(carg_0, carg_1);

	if(carg_1 != NULL)
		register_token(carg_1);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_function_1subprogram(JNIEnv *env, jobject this, jboolean jarg_0, jboolean jarg_1)
{
	int carg_0;
	int carg_1;

#ifdef DEBUG
	printf("hello from %s\n", "function_subprogram");
#endif

	carg_0 = (int)jarg_0;
	carg_1 = (int)jarg_1;

	c_action_function_subprogram(carg_0, carg_1);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_function_1stmt_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "function_stmt__begin");
#endif


	c_action_function_stmt__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_function_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jboolean jarg_4, jboolean jarg_5)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	int carg_4;
	int carg_5;

#ifdef DEBUG
	printf("hello from %s\n", "function_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	carg_4 = (int)jarg_4;
	carg_5 = (int)jarg_5;

	c_action_function_stmt(carg_0, carg_1, carg_2, carg_3, carg_4, carg_5);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_proc_1language_1binding_1spec(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "proc_language_binding_spec");
#endif


	c_action_proc_language_binding_spec();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_t_1prefix(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "t_prefix");
#endif

	carg_0 = (int)jarg_0;

	c_action_t_prefix(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_prefix_1spec(JNIEnv *env, jobject this, jboolean jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "prefix_spec");
#endif

	carg_0 = (int)jarg_0;

	c_action_prefix_spec(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_t_1prefix_1spec(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "t_prefix_spec");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_t_prefix_spec(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_result_1name(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "result_name");
#endif


	c_action_result_name();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_end_1function_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jobject jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	Token_t *carg_4 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "end_function_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	if(jarg_4 != NULL)
		carg_4 = convert_Java_token(env, jarg_4);

	c_action_end_function_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
	if(carg_4 != NULL)
		register_token(carg_4);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_subroutine_1stmt_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "subroutine_stmt__begin");
#endif


	c_action_subroutine_stmt__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_subroutine_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jboolean jarg_4, jboolean jarg_5, jboolean jarg_6, jboolean jarg_7)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	int carg_4;
	int carg_5;
	int carg_6;
	int carg_7;

#ifdef DEBUG
	printf("hello from %s\n", "subroutine_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	carg_4 = (int)jarg_4;
	carg_5 = (int)jarg_5;
	carg_6 = (int)jarg_6;
	carg_7 = (int)jarg_7;

	c_action_subroutine_stmt(carg_0, carg_1, carg_2, carg_3, carg_4, carg_5, carg_6, carg_7);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_dummy_1arg(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "dummy_arg");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_dummy_arg(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_dummy_1arg_1list_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "dummy_arg_list__begin");
#endif


	c_action_dummy_arg_list__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_dummy_1arg_1list(JNIEnv *env, jobject this, jint jarg_0)
{
	int carg_0;

#ifdef DEBUG
	printf("hello from %s\n", "dummy_arg_list");
#endif

	carg_0 = (int)jarg_0;

	c_action_dummy_arg_list(carg_0);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_end_1subroutine_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jobject jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	Token_t *carg_4 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "end_subroutine_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	if(jarg_4 != NULL)
		carg_4 = convert_Java_token(env, jarg_4);

	c_action_end_subroutine_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
	if(carg_4 != NULL)
		register_token(carg_4);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_entry_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jboolean jarg_4, jboolean jarg_5)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	int carg_4;
	int carg_5;

#ifdef DEBUG
	printf("hello from %s\n", "entry_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	carg_4 = (int)jarg_4;
	carg_5 = (int)jarg_5;

	c_action_entry_stmt(carg_0, carg_1, carg_2, carg_3, carg_4, carg_5);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_return_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jboolean jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	int carg_3;

#ifdef DEBUG
	printf("hello from %s\n", "return_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	carg_3 = (int)jarg_3;

	c_action_return_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_contains_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "contains_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_contains_stmt(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_separate_1module_1subprogram(JNIEnv *env, jobject this, jboolean jarg_0, jboolean jarg_1)
{
	int carg_0;
	int carg_1;

#ifdef DEBUG
	printf("hello from %s\n", "separate_module_subprogram");
#endif

	carg_0 = (int)jarg_0;
	carg_1 = (int)jarg_1;

	c_action_separate_module_subprogram(carg_0, carg_1);

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_separate_1module_1subprogram_1_1begin(JNIEnv *env, jobject this)
{

#ifdef DEBUG
	printf("hello from %s\n", "separate_module_subprogram__begin");
#endif


	c_action_separate_module_subprogram__begin();

}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_mp_1subprogram_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jobject jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	Token_t *carg_4 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "mp_subprogram_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	if(jarg_4 != NULL)
		carg_4 = convert_Java_token(env, jarg_4);

	c_action_mp_subprogram_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
	if(carg_4 != NULL)
		register_token(carg_4);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_end_1mp_1subprogram_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jobject jarg_3, jobject jarg_4)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	Token_t *carg_3 = NULL;
	Token_t *carg_4 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "end_mp_subprogram_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	if(jarg_3 != NULL)
		carg_3 = convert_Java_token(env, jarg_3);
	if(jarg_4 != NULL)
		carg_4 = convert_Java_token(env, jarg_4);

	c_action_end_mp_subprogram_stmt(carg_0, carg_1, carg_2, carg_3, carg_4);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
	if(carg_3 != NULL)
		register_token(carg_3);
	if(carg_4 != NULL)
		register_token(carg_4);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_stmt_1function_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jboolean jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	int carg_3;

#ifdef DEBUG
	printf("hello from %s\n", "stmt_function_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	carg_3 = (int)jarg_3;

	c_action_stmt_function_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_end_1of_1stmt(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "end_of_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_end_of_stmt(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_start_1of_1file(JNIEnv *env, jobject this, jstring jarg_0)
{
	const char *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "start_of_file");
#endif

	if(jarg_0 != NULL)
	{
		if(env == NULL)
		{
			fprintf(stderr, "env is NULL!\n");
			exit(1);
		}
		carg_0 = (*env)->GetStringUTFChars(env, jarg_0, NULL);
	}

	c_action_start_of_file(carg_0);

	(*env)->ReleaseStringUTFChars(env, jarg_0, carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_end_1of_1file(JNIEnv *env, jobject this, jstring jarg_0)
{
	const char *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "end_of_file");
#endif

	if(jarg_0 != NULL)
	{
		if(env == NULL)
		{
			fprintf(stderr, "env is NULL!\n");
			exit(1);
		}
		carg_0 = (*env)->GetStringUTFChars(env, jarg_0, NULL);
	}

	c_action_end_of_file(carg_0);

	(*env)->ReleaseStringUTFChars(env, jarg_0, carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_rice_1image_1selector(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "rice_image_selector");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_rice_image_selector(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_rice_1co_1dereference_1op(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "rice_co_dereference_op");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);

	c_action_rice_co_dereference_op(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_rice_1allocate_1coarray_1spec(JNIEnv *env, jobject this, jint jarg_0, jobject jarg_1)
{
	int carg_0;
	Token_t *carg_1 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "rice_allocate_coarray_spec");
#endif

	carg_0 = (int)jarg_0;
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);

	c_action_rice_allocate_coarray_spec(carg_0, carg_1);

	if(carg_1 != NULL)
		register_token(carg_1);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_rice_1co_1with_1team_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "rice_co_with_team_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);

	c_action_rice_co_with_team_stmt(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_rice_1end_1with_1team_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "rice_end_with_team_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_rice_end_with_team_stmt(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_rice_1finish_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "rice_finish_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);

	c_action_rice_finish_stmt(carg_0, carg_1, carg_2);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_rice_1end_1finish_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "rice_end_finish_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);

	c_action_rice_end_finish_stmt(carg_0, carg_1);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_rice_1spawn_1stmt(JNIEnv *env, jobject this, jobject jarg_0, jobject jarg_1, jobject jarg_2, jboolean jarg_3)
{
	Token_t *carg_0 = NULL;
	Token_t *carg_1 = NULL;
	Token_t *carg_2 = NULL;
	int carg_3;

#ifdef DEBUG
	printf("hello from %s\n", "rice_spawn_stmt");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);
	if(jarg_1 != NULL)
		carg_1 = convert_Java_token(env, jarg_1);
	if(jarg_2 != NULL)
		carg_2 = convert_Java_token(env, jarg_2);
	carg_3 = (int)jarg_3;

	c_action_rice_spawn_stmt(carg_0, carg_1, carg_2, carg_3);

	if(carg_0 != NULL)
		register_token(carg_0);
	if(carg_1 != NULL)
		register_token(carg_1);
	if(carg_2 != NULL)
		register_token(carg_2);
}

void Java_fortran_ofp_parser_c_jni_FortranParserActionJNI_next_1token(JNIEnv *env, jobject this, jobject jarg_0)
{
	Token_t *carg_0 = NULL;

#ifdef DEBUG
	printf("hello from %s\n", "next_token");
#endif

	if(jarg_0 != NULL)
		carg_0 = convert_Java_token(env, jarg_0);

	c_action_next_token(carg_0);

	if(carg_0 != NULL)
		register_token(carg_0);
}

