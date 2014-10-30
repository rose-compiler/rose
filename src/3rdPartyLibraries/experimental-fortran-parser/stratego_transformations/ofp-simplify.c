#include <srts/stratego.h>
#include <stratego-lib/stratego-lib.h>
void init_constructors (void)
{
}
static Symbol sym_EquivalenceSet_1;
static Symbol sym_Class_2;
static Symbol sym_DerivedType_2;
static Symbol sym_SpecificationPart_1;
static Symbol sym_InitialSpecPart_1;
static Symbol sym_CommonStmt_3;
static Symbol sym__4;
static Symbol sym__2;
static Symbol sym_Nil_0;
static Symbol sym_Cons_2;
static Symbol sym_derived_type_spec_1;
static Symbol sym_class_type_spec_1;
static Symbol sym_opt_component_array_spec_1;
static Symbol sym_DerivedTypeSpec_2;
static Symbol sym_equals_expr_1;
static Symbol sym_opt_array_spec_1;
static Symbol sym_opt_coarray_spec_1;
static Symbol sym_opt_char_length_1;
static Symbol sym_language_binding_name_1;
static Symbol sym_explicit_coshape_spec_list_2;
static Symbol sym_ExplicitCoshape_2;
static Symbol sym_final_cobounds_entry_1;
static Symbol sym_name_list_entry_list_head_1;
static Symbol sym_name_list_entry_list_tail_2;
static Symbol sym_equivalence_set_list_2;
static Symbol sym_common_stmt_4;
static Symbol sym_DataRef_1;
static Symbol sym_PartRef_3;
static Symbol sym_no_section_subscripts_0;
static Symbol sym_opt_stride_1;
static Symbol sym_ImageSelector_1;
static Symbol sym_no_image_selector_0;
static Symbol sym_no_lower_bound_0;
static Symbol sym_Primary_1;
static Symbol sym_type_spec_colon_1;
static Symbol sym_colon_forall_step_1;
static Symbol sym_opt_expr_1;
static Symbol sym_a_name_1;
static Symbol sym_read_format_1;
static Symbol sym_io_control_spec_list_1;
static Symbol sym_io_control_spec_list_2;
static Symbol sym_io_control_spec_list_3;
static Symbol sym_a_list_1;
static Symbol sym_FMT_1;
static Symbol sym_backspace_unit_stmt_3;
static Symbol sym_BackspaceStmt_3;
static Symbol sym_endfile_unit_stmt_3;
static Symbol sym_EndfileStmt_3;
static Symbol sym_rewind_unit_stmt_3;
static Symbol sym_RewindStmt_3;
static Symbol sym_flush_unit_stmt_3;
static Symbol sym_FlushStmt_3;
static Symbol sym_UNIT_1;
static Symbol sym_format_items_first_1;
static Symbol sym_format_items_2;
static Symbol sym_paren_list_1;
static Symbol sym_initial_spec_part_2;
static Symbol sym_SpecificationPart_4;
static Symbol sym_ImplicitPart_2;
static Symbol sym_no_implicit_part_0;
static Symbol sym_comma_list_1;
static Symbol sym_STAR_0;
static Symbol sym_no_list_0;
static void init_module_constructors (void)
{
sym_EquivalenceSet_1 = ATmakeSymbol("EquivalenceSet", 1, ATfalse);
ATprotectSymbol(sym_EquivalenceSet_1);
sym_Class_2 = ATmakeSymbol("Class", 2, ATfalse);
ATprotectSymbol(sym_Class_2);
sym_DerivedType_2 = ATmakeSymbol("DerivedType", 2, ATfalse);
ATprotectSymbol(sym_DerivedType_2);
sym_SpecificationPart_1 = ATmakeSymbol("SpecificationPart", 1, ATfalse);
ATprotectSymbol(sym_SpecificationPart_1);
sym_InitialSpecPart_1 = ATmakeSymbol("InitialSpecPart", 1, ATfalse);
ATprotectSymbol(sym_InitialSpecPart_1);
sym_CommonStmt_3 = ATmakeSymbol("CommonStmt", 3, ATfalse);
ATprotectSymbol(sym_CommonStmt_3);
sym__4 = ATmakeSymbol("", 4, ATfalse);
ATprotectSymbol(sym__4);
sym__2 = ATmakeSymbol("", 2, ATfalse);
ATprotectSymbol(sym__2);
sym_Nil_0 = ATmakeSymbol("Nil", 0, ATfalse);
ATprotectSymbol(sym_Nil_0);
sym_Cons_2 = ATmakeSymbol("Cons", 2, ATfalse);
ATprotectSymbol(sym_Cons_2);
sym_derived_type_spec_1 = ATmakeSymbol("derived-type-spec", 1, ATfalse);
ATprotectSymbol(sym_derived_type_spec_1);
sym_class_type_spec_1 = ATmakeSymbol("class-type-spec", 1, ATfalse);
ATprotectSymbol(sym_class_type_spec_1);
sym_opt_component_array_spec_1 = ATmakeSymbol("opt-component-array-spec", 1, ATfalse);
ATprotectSymbol(sym_opt_component_array_spec_1);
sym_DerivedTypeSpec_2 = ATmakeSymbol("DerivedTypeSpec", 2, ATfalse);
ATprotectSymbol(sym_DerivedTypeSpec_2);
sym_equals_expr_1 = ATmakeSymbol("equals-expr", 1, ATfalse);
ATprotectSymbol(sym_equals_expr_1);
sym_opt_array_spec_1 = ATmakeSymbol("opt-array-spec", 1, ATfalse);
ATprotectSymbol(sym_opt_array_spec_1);
sym_opt_coarray_spec_1 = ATmakeSymbol("opt-coarray-spec", 1, ATfalse);
ATprotectSymbol(sym_opt_coarray_spec_1);
sym_opt_char_length_1 = ATmakeSymbol("opt-char-length", 1, ATfalse);
ATprotectSymbol(sym_opt_char_length_1);
sym_language_binding_name_1 = ATmakeSymbol("language-binding-name", 1, ATfalse);
ATprotectSymbol(sym_language_binding_name_1);
sym_explicit_coshape_spec_list_2 = ATmakeSymbol("explicit-coshape-spec-list", 2, ATfalse);
ATprotectSymbol(sym_explicit_coshape_spec_list_2);
sym_ExplicitCoshape_2 = ATmakeSymbol("ExplicitCoshape", 2, ATfalse);
ATprotectSymbol(sym_ExplicitCoshape_2);
sym_final_cobounds_entry_1 = ATmakeSymbol("final-cobounds-entry", 1, ATfalse);
ATprotectSymbol(sym_final_cobounds_entry_1);
sym_name_list_entry_list_head_1 = ATmakeSymbol("name-list-entry-list-head", 1, ATfalse);
ATprotectSymbol(sym_name_list_entry_list_head_1);
sym_name_list_entry_list_tail_2 = ATmakeSymbol("name-list-entry-list-tail", 2, ATfalse);
ATprotectSymbol(sym_name_list_entry_list_tail_2);
sym_equivalence_set_list_2 = ATmakeSymbol("equivalence-set-list", 2, ATfalse);
ATprotectSymbol(sym_equivalence_set_list_2);
sym_common_stmt_4 = ATmakeSymbol("common-stmt", 4, ATfalse);
ATprotectSymbol(sym_common_stmt_4);
sym_DataRef_1 = ATmakeSymbol("DataRef", 1, ATfalse);
ATprotectSymbol(sym_DataRef_1);
sym_PartRef_3 = ATmakeSymbol("PartRef", 3, ATfalse);
ATprotectSymbol(sym_PartRef_3);
sym_no_section_subscripts_0 = ATmakeSymbol("no-section-subscripts", 0, ATfalse);
ATprotectSymbol(sym_no_section_subscripts_0);
sym_opt_stride_1 = ATmakeSymbol("opt-stride", 1, ATfalse);
ATprotectSymbol(sym_opt_stride_1);
sym_ImageSelector_1 = ATmakeSymbol("ImageSelector", 1, ATfalse);
ATprotectSymbol(sym_ImageSelector_1);
sym_no_image_selector_0 = ATmakeSymbol("no-image-selector", 0, ATfalse);
ATprotectSymbol(sym_no_image_selector_0);
sym_no_lower_bound_0 = ATmakeSymbol("no-lower-bound", 0, ATfalse);
ATprotectSymbol(sym_no_lower_bound_0);
sym_Primary_1 = ATmakeSymbol("Primary", 1, ATfalse);
ATprotectSymbol(sym_Primary_1);
sym_type_spec_colon_1 = ATmakeSymbol("type-spec-colon", 1, ATfalse);
ATprotectSymbol(sym_type_spec_colon_1);
sym_colon_forall_step_1 = ATmakeSymbol("colon-forall-step", 1, ATfalse);
ATprotectSymbol(sym_colon_forall_step_1);
sym_opt_expr_1 = ATmakeSymbol("opt-expr", 1, ATfalse);
ATprotectSymbol(sym_opt_expr_1);
sym_a_name_1 = ATmakeSymbol("a-name", 1, ATfalse);
ATprotectSymbol(sym_a_name_1);
sym_read_format_1 = ATmakeSymbol("read-format", 1, ATfalse);
ATprotectSymbol(sym_read_format_1);
sym_io_control_spec_list_1 = ATmakeSymbol("io-control-spec-list", 1, ATfalse);
ATprotectSymbol(sym_io_control_spec_list_1);
sym_io_control_spec_list_2 = ATmakeSymbol("io-control-spec-list", 2, ATfalse);
ATprotectSymbol(sym_io_control_spec_list_2);
sym_io_control_spec_list_3 = ATmakeSymbol("io-control-spec-list", 3, ATfalse);
ATprotectSymbol(sym_io_control_spec_list_3);
sym_a_list_1 = ATmakeSymbol("a-list", 1, ATfalse);
ATprotectSymbol(sym_a_list_1);
sym_FMT_1 = ATmakeSymbol("FMT", 1, ATfalse);
ATprotectSymbol(sym_FMT_1);
sym_backspace_unit_stmt_3 = ATmakeSymbol("backspace-unit-stmt", 3, ATfalse);
ATprotectSymbol(sym_backspace_unit_stmt_3);
sym_BackspaceStmt_3 = ATmakeSymbol("BackspaceStmt", 3, ATfalse);
ATprotectSymbol(sym_BackspaceStmt_3);
sym_endfile_unit_stmt_3 = ATmakeSymbol("endfile-unit-stmt", 3, ATfalse);
ATprotectSymbol(sym_endfile_unit_stmt_3);
sym_EndfileStmt_3 = ATmakeSymbol("EndfileStmt", 3, ATfalse);
ATprotectSymbol(sym_EndfileStmt_3);
sym_rewind_unit_stmt_3 = ATmakeSymbol("rewind-unit-stmt", 3, ATfalse);
ATprotectSymbol(sym_rewind_unit_stmt_3);
sym_RewindStmt_3 = ATmakeSymbol("RewindStmt", 3, ATfalse);
ATprotectSymbol(sym_RewindStmt_3);
sym_flush_unit_stmt_3 = ATmakeSymbol("flush-unit-stmt", 3, ATfalse);
ATprotectSymbol(sym_flush_unit_stmt_3);
sym_FlushStmt_3 = ATmakeSymbol("FlushStmt", 3, ATfalse);
ATprotectSymbol(sym_FlushStmt_3);
sym_UNIT_1 = ATmakeSymbol("UNIT", 1, ATfalse);
ATprotectSymbol(sym_UNIT_1);
sym_format_items_first_1 = ATmakeSymbol("format-items-first", 1, ATfalse);
ATprotectSymbol(sym_format_items_first_1);
sym_format_items_2 = ATmakeSymbol("format-items", 2, ATfalse);
ATprotectSymbol(sym_format_items_2);
sym_paren_list_1 = ATmakeSymbol("paren-list", 1, ATfalse);
ATprotectSymbol(sym_paren_list_1);
sym_initial_spec_part_2 = ATmakeSymbol("initial-spec-part", 2, ATfalse);
ATprotectSymbol(sym_initial_spec_part_2);
sym_SpecificationPart_4 = ATmakeSymbol("SpecificationPart", 4, ATfalse);
ATprotectSymbol(sym_SpecificationPart_4);
sym_ImplicitPart_2 = ATmakeSymbol("ImplicitPart", 2, ATfalse);
ATprotectSymbol(sym_ImplicitPart_2);
sym_no_implicit_part_0 = ATmakeSymbol("no-implicit-part", 0, ATfalse);
ATprotectSymbol(sym_no_implicit_part_0);
sym_comma_list_1 = ATmakeSymbol("comma-list", 1, ATfalse);
ATprotectSymbol(sym_comma_list_1);
sym_STAR_0 = ATmakeSymbol("STAR", 0, ATfalse);
ATprotectSymbol(sym_STAR_0);
sym_no_list_0 = ATmakeSymbol("no-list", 0, ATfalse);
ATprotectSymbol(sym_no_list_0);
}
static ATerm term5;
static ATerm term4;
static ATerm term3;
static ATerm term2;
static ATerm term1;
static ATerm term0;
static void init_module_constant_terms (void)
{
ATprotect(&(term0));
term0 = (ATerm) ATmakeAppl(sym_no_lower_bound_0);
ATprotect(&(term1));
term1 = term0;
ATprotect(&(term2));
term2 = (ATerm) ATmakeAppl(sym_STAR_0);
ATprotect(&(term3));
term3 = term2;
ATprotect(&(term4));
term4 = (ATerm) ATmakeAppl(sym_ExplicitCoshape_2, term1, term3);
ATprotect(&(term5));
term5 = term4;
}
#include <srts/init-stratego-application.h>
extern const char * __tracing_table [];
extern unsigned short __tracing_table_counter;
ATerm concat_0_0 (StrSL sl, ATerm t);
ATerm conc_0_0 (StrSL sl, ATerm t);
ATerm io_wrap_1_0 (StrSL sl, StrCL c_95, ATerm t);
ATerm main_0_0 (StrSL sl, ATerm t);
static ATerm c_280 (StrSL sl, ATerm t);
static ATerm lifted0 (StrSL sl, ATerm t);
static ATerm b_280 (StrSL sl, StrCL d_280, ATerm t);
ATerm ofp_simplify_0_0 (StrSL sl, ATerm t);
ATerm main_0_0 (StrSL sl, ATerm t)
{
__tracing_table[__tracing_table_counter++] = "main_0_0";
__tracing_table[__tracing_table_counter] = NULL;
sl_decl(sl);
{
struct str_closure g_280 = { &(ofp_simplify_0_0) , sl };
StrCL lifted1_cl = &(g_280);
t = io_wrap_1_0(sl, lifted1_cl, t);
if((t == NULL))
goto fail4 ;
}
__tracing_table[--__tracing_table_counter] = NULL;
return(t);
fail4 :
--__tracing_table_counter;
return(NULL);
}
ATerm ofp_simplify_0_0 (StrSL sl, ATerm t)
{
__tracing_table[__tracing_table_counter++] = "ofp_simplify_0_0";
__tracing_table[__tracing_table_counter] = NULL;
sl_decl(sl);
sl_funs(1);
{
struct str_closure f_280 = { &(c_280) , &(frame) };
StrCL c_280_cl = &(f_280);
sl_init_fun(0, c_280_cl);
t = b_280(&(frame), c_280_cl, t);
if((t == NULL))
goto fail0 ;
}
__tracing_table[--__tracing_table_counter] = NULL;
return(t);
fail0 :
--__tracing_table_counter;
return(NULL);
}
static ATerm c_280 (StrSL sl, ATerm t)
{
__tracing_table[__tracing_table_counter++] = "c_280";
__tracing_table[__tracing_table_counter] = NULL;
sl_decl(sl);
{
ATerm trm0 = t;
ATerm trm1 = t;
if(!(match_cons(t, sym_no_list_0)))
goto label3 ;
t = (ATerm) ATempty;
goto label2 ;
label3 :
t = trm1;
{
ATerm trm2 = t;
ATerm p_275 = NULL;
if(match_cons(t, sym_a_list_1))
{
p_275 = ATgetArgument(t, 0);
}
else
goto label4 ;
t = p_275;
goto label2 ;
label4 :
t = trm2;
{
ATerm trm3 = t;
ATerm q_275 = NULL;
if(match_cons(t, sym_paren_list_1))
{
q_275 = ATgetArgument(t, 0);
}
else
goto label5 ;
t = q_275;
goto label2 ;
label5 :
t = trm3;
{
ATerm trm4 = t;
ATerm r_275 = NULL;
if(match_cons(t, sym_comma_list_1))
{
r_275 = ATgetArgument(t, 0);
}
else
goto label6 ;
t = r_275;
goto label2 ;
label6 :
t = trm4;
{
ATerm trm5 = t;
ATerm s_275 = NULL;
if(match_cons(t, sym_a_name_1))
{
s_275 = ATgetArgument(t, 0);
}
else
goto label7 ;
t = s_275;
goto label2 ;
label7 :
t = trm5;
{
ATerm trm6 = t;
ATerm t_275 = NULL,u_275 = NULL;
if(match_cons(t, sym_ImplicitPart_2))
{
t_275 = ATgetArgument(t, 0);
u_275 = ATgetArgument(t, 1);
}
else
goto label8 ;
t = (ATerm) ATmakeAppl(sym__2, t_275, (ATerm) ATinsert(ATempty, u_275));
t = conc_0_0(sl_up(sl), t);
if((t == NULL))
goto label8 ;
t = b_280(sl, sl_fun_cl(0, sl), t);
if((t == NULL))
goto label8 ;
goto label2 ;
label8 :
t = trm6;
{
ATerm trm7 = t;
if(!(match_cons(t, sym_no_implicit_part_0)))
goto label9 ;
t = (ATerm) ATempty;
goto label2 ;
label9 :
t = trm7;
{
ATerm trm8 = t;
ATerm v_275 = NULL,w_275 = NULL,x_275 = NULL,y_275 = NULL,z_275 = NULL,a_276 = NULL,b_276 = NULL,c_276 = NULL;
if(match_cons(t, sym_SpecificationPart_4))
{
v_275 = ATgetArgument(t, 0);
w_275 = ATgetArgument(t, 1);
x_275 = ATgetArgument(t, 2);
y_275 = ATgetArgument(t, 3);
}
else
goto label10 ;
a_276 = t;
t = (ATerm) ATmakeAppl(sym__4, v_275, w_275, x_275, y_275);
t = conc_0_0(sl_up(sl), t);
if((t == NULL))
goto label10 ;
z_275 = t;
t = a_276;
c_276 = t;
t = z_275;
t = b_280(sl, sl_fun_cl(0, sl), t);
if((t == NULL))
goto label10 ;
b_276 = t;
t = c_276;
t = (ATerm) ATmakeAppl(sym_SpecificationPart_1, b_276);
goto label2 ;
label10 :
t = trm8;
{
ATerm trm9 = t;
ATerm n_276 = NULL,w_276 = NULL,x_276 = NULL,y_276 = NULL,g_277 = NULL,i_277 = NULL;
if(match_cons(t, sym_initial_spec_part_2))
{
n_276 = ATgetArgument(t, 0);
w_276 = ATgetArgument(t, 1);
}
else
goto label11 ;
y_276 = t;
t = (ATerm) ATmakeAppl(sym__2, n_276, w_276);
t = conc_0_0(sl_up(sl), t);
if((t == NULL))
goto label11 ;
x_276 = t;
t = y_276;
i_277 = t;
t = x_276;
t = b_280(sl, sl_fun_cl(0, sl), t);
if((t == NULL))
goto label11 ;
g_277 = t;
t = i_277;
t = (ATerm) ATmakeAppl(sym_InitialSpecPart_1, g_277);
goto label2 ;
label11 :
t = trm9;
{
ATerm trm10 = t;
ATerm j_277 = NULL,k_277 = NULL;
if(match_cons(t, sym_derived_type_spec_1))
{
ATerm trm11 = ATgetArgument(t, 0);
if(match_cons(trm11, sym_DerivedTypeSpec_2))
{
j_277 = ATgetArgument(trm11, 0);
k_277 = ATgetArgument(trm11, 1);
}
else
goto label12 ;
}
else
goto label12 ;
t = (ATerm) ATmakeAppl(sym_DerivedType_2, j_277, k_277);
goto label2 ;
label12 :
t = trm10;
{
ATerm trm12 = t;
ATerm n_277 = NULL,o_277 = NULL;
if(match_cons(t, sym_class_type_spec_1))
{
ATerm trm13 = ATgetArgument(t, 0);
if(match_cons(trm13, sym_DerivedTypeSpec_2))
{
n_277 = ATgetArgument(trm13, 0);
o_277 = ATgetArgument(trm13, 1);
}
else
goto label13 ;
}
else
goto label13 ;
t = (ATerm) ATmakeAppl(sym_Class_2, n_277, o_277);
goto label2 ;
label13 :
t = trm12;
{
ATerm trm14 = t;
ATerm p_277 = NULL;
if(match_cons(t, sym_opt_component_array_spec_1))
{
p_277 = ATgetArgument(t, 0);
}
else
goto label14 ;
t = p_277;
goto label2 ;
label14 :
t = trm14;
{
ATerm trm15 = t;
ATerm r_277 = NULL;
if(match_cons(t, sym_equals_expr_1))
{
r_277 = ATgetArgument(t, 0);
}
else
goto label15 ;
t = r_277;
goto label2 ;
label15 :
t = trm15;
{
ATerm trm16 = t;
ATerm s_277 = NULL;
if(match_cons(t, sym_opt_array_spec_1))
{
s_277 = ATgetArgument(t, 0);
}
else
goto label16 ;
t = s_277;
goto label2 ;
label16 :
t = trm16;
{
ATerm trm17 = t;
ATerm t_277 = NULL;
if(match_cons(t, sym_opt_coarray_spec_1))
{
t_277 = ATgetArgument(t, 0);
}
else
goto label17 ;
t = t_277;
goto label2 ;
label17 :
t = trm17;
{
ATerm trm18 = t;
ATerm u_277 = NULL;
if(match_cons(t, sym_opt_char_length_1))
{
u_277 = ATgetArgument(t, 0);
}
else
goto label18 ;
t = u_277;
goto label2 ;
label18 :
t = trm18;
{
ATerm trm19 = t;
ATerm w_277 = NULL;
if(match_cons(t, sym_language_binding_name_1))
{
w_277 = ATgetArgument(t, 0);
}
else
goto label19 ;
t = w_277;
goto label2 ;
label19 :
t = trm19;
{
ATerm trm20 = t;
ATerm x_277 = NULL,y_277 = NULL;
if(match_cons(t, sym_explicit_coshape_spec_list_2))
{
x_277 = ATgetArgument(t, 0);
y_277 = ATgetArgument(t, 1);
}
else
goto label20 ;
t = (ATerm) ATmakeAppl(sym__2, x_277, (ATerm) ATinsert(ATempty, y_277));
t = conc_0_0(sl_up(sl), t);
if((t == NULL))
goto label20 ;
t = b_280(sl, sl_fun_cl(0, sl), t);
if((t == NULL))
goto label20 ;
goto label2 ;
label20 :
t = trm20;
{
ATerm trm21 = t;
ATerm z_277 = NULL;
if(match_cons(t, sym_final_cobounds_entry_1))
{
z_277 = ATgetArgument(t, 0);
}
else
goto label21 ;
t = term5;
goto label2 ;
label21 :
t = trm21;
{
ATerm trm22 = t;
ATerm b_278 = NULL;
if(match_cons(t, sym_name_list_entry_list_head_1))
{
b_278 = ATgetArgument(t, 0);
}
else
goto label22 ;
t = (ATerm) ATinsert(ATempty, b_278);
goto label2 ;
label22 :
t = trm22;
{
ATerm trm23 = t;
ATerm c_278 = NULL,f_278 = NULL;
if(match_cons(t, sym_name_list_entry_list_tail_2))
{
c_278 = ATgetArgument(t, 0);
f_278 = ATgetArgument(t, 1);
}
else
goto label23 ;
t = (ATerm) ATmakeAppl(sym__2, c_278, (ATerm) ATinsert(ATempty, f_278));
t = conc_0_0(sl_up(sl), t);
if((t == NULL))
goto label23 ;
t = b_280(sl, sl_fun_cl(0, sl), t);
if((t == NULL))
goto label23 ;
goto label2 ;
label23 :
t = trm23;
{
ATerm trm24 = t;
ATerm g_278 = NULL,i_278 = NULL,j_278 = NULL,k_278 = NULL,l_278 = NULL,m_278 = NULL;
if(match_cons(t, sym_equivalence_set_list_2))
{
g_278 = ATgetArgument(t, 0);
i_278 = ATgetArgument(t, 1);
}
else
goto label24 ;
k_278 = t;
t = (ATerm) ATinsert(ATinsert(ATempty, i_278), (ATerm) ATinsert(ATempty, g_278));
t = concat_0_0(sl_up(sl), t);
if((t == NULL))
goto label24 ;
j_278 = t;
t = k_278;
m_278 = t;
t = j_278;
t = b_280(sl, sl_fun_cl(0, sl), t);
if((t == NULL))
goto label24 ;
l_278 = t;
t = m_278;
t = (ATerm) ATmakeAppl(sym_EquivalenceSet_1, l_278);
goto label2 ;
label24 :
t = trm24;
{
ATerm trm25 = t;
ATerm n_278 = NULL,o_278 = NULL,p_278 = NULL,q_278 = NULL,r_278 = NULL,t_278 = NULL,u_278 = NULL,v_278 = NULL;
if(match_cons(t, sym_common_stmt_4))
{
n_278 = ATgetArgument(t, 0);
o_278 = ATgetArgument(t, 1);
p_278 = ATgetArgument(t, 2);
q_278 = ATgetArgument(t, 3);
}
else
goto label25 ;
t_278 = t;
t = (ATerm) ATinsert(ATinsert(ATempty, p_278), (ATerm) ATinsert(ATempty, o_278));
t = concat_0_0(sl_up(sl), t);
if((t == NULL))
goto label25 ;
r_278 = t;
t = t_278;
v_278 = t;
t = r_278;
t = b_280(sl, sl_fun_cl(0, sl), t);
if((t == NULL))
goto label25 ;
u_278 = t;
t = v_278;
t = (ATerm) ATmakeAppl(sym_CommonStmt_3, n_278, u_278, q_278);
goto label2 ;
label25 :
t = trm25;
{
ATerm trm26 = t;
ATerm w_278 = NULL;
if(match_cons(t, sym_DataRef_1))
{
ATerm trm27 = ATgetArgument(t, 0);
if(((ATgetType(trm27) == AT_LIST) && !(ATisEmpty(trm27))))
{
w_278 = ATgetFirst((ATermList) trm27);
{
ATerm trm28 = (ATerm) ATgetNext((ATermList) trm27);
if(!(((ATgetType(trm28) == AT_LIST) && ATisEmpty(trm28))))
goto label26 ;
}
}
else
goto label26 ;
}
else
goto label26 ;
t = w_278;
goto label2 ;
label26 :
t = trm26;
{
ATerm trm29 = t;
ATerm x_278 = NULL;
if(match_cons(t, sym_PartRef_3))
{
x_278 = ATgetArgument(t, 0);
{
ATerm trm30 = ATgetArgument(t, 1);
if(!(match_cons(trm30, sym_no_section_subscripts_0)))
goto label27 ;
}
{
ATerm trm31 = ATgetArgument(t, 2);
if(!(match_cons(trm31, sym_no_image_selector_0)))
goto label27 ;
}
}
else
goto label27 ;
t = x_278;
goto label2 ;
label27 :
t = trm29;
{
ATerm trm32 = t;
ATerm y_278 = NULL;
if(match_cons(t, sym_opt_stride_1))
{
y_278 = ATgetArgument(t, 0);
}
else
goto label28 ;
t = y_278;
goto label2 ;
label28 :
t = trm32;
{
ATerm trm33 = t;
ATerm a_279 = NULL;
if(match_cons(t, sym_ImageSelector_1))
{
a_279 = ATgetArgument(t, 0);
}
else
goto label29 ;
t = a_279;
goto label2 ;
label29 :
t = trm33;
{
ATerm trm34 = t;
ATerm b_279 = NULL;
if(match_cons(t, sym_Primary_1))
{
b_279 = ATgetArgument(t, 0);
}
else
goto label30 ;
t = b_279;
goto label2 ;
label30 :
t = trm34;
{
ATerm trm35 = t;
ATerm c_279 = NULL;
if(match_cons(t, sym_type_spec_colon_1))
{
c_279 = ATgetArgument(t, 0);
}
else
goto label31 ;
t = c_279;
goto label2 ;
label31 :
t = trm35;
{
ATerm trm36 = t;
ATerm d_279 = NULL;
if(match_cons(t, sym_colon_forall_step_1))
{
d_279 = ATgetArgument(t, 0);
}
else
goto label32 ;
t = d_279;
goto label2 ;
label32 :
t = trm36;
{
ATerm trm37 = t;
ATerm e_279 = NULL;
if(match_cons(t, sym_opt_expr_1))
{
e_279 = ATgetArgument(t, 0);
}
else
goto label33 ;
t = e_279;
goto label2 ;
label33 :
t = trm37;
{
ATerm trm38 = t;
ATerm f_279 = NULL;
if(match_cons(t, sym_read_format_1))
{
f_279 = ATgetArgument(t, 0);
}
else
goto label34 ;
t = (ATerm) ATinsert(ATempty, (ATerm) ATmakeAppl(sym_FMT_1, f_279));
goto label2 ;
label34 :
t = trm38;
{
ATerm trm39 = t;
ATerm g_279 = NULL;
if(match_cons(t, sym_io_control_spec_list_1))
{
g_279 = ATgetArgument(t, 0);
}
else
goto label35 ;
t = g_279;
goto label2 ;
label35 :
t = trm39;
{
ATerm trm40 = t;
ATerm h_279 = NULL,i_279 = NULL;
if(match_cons(t, sym_io_control_spec_list_2))
{
h_279 = ATgetArgument(t, 0);
i_279 = ATgetArgument(t, 1);
}
else
goto label36 ;
t = (ATerm) ATinsert(ATinsert(ATempty, i_279), (ATerm) ATinsert(ATempty, h_279));
t = concat_0_0(sl_up(sl), t);
if((t == NULL))
goto label36 ;
t = b_280(sl, sl_fun_cl(0, sl), t);
if((t == NULL))
goto label36 ;
goto label2 ;
label36 :
t = trm40;
{
ATerm trm41 = t;
ATerm j_279 = NULL,k_279 = NULL,l_279 = NULL;
if(match_cons(t, sym_io_control_spec_list_3))
{
j_279 = ATgetArgument(t, 0);
k_279 = ATgetArgument(t, 1);
l_279 = ATgetArgument(t, 2);
}
else
goto label37 ;
t = (ATerm) ATinsert(ATinsert(ATinsert(ATempty, l_279), (ATerm) ATinsert(ATempty, k_279)), (ATerm) ATinsert(ATempty, j_279));
t = concat_0_0(sl_up(sl), t);
if((t == NULL))
goto label37 ;
t = b_280(sl, sl_fun_cl(0, sl), t);
if((t == NULL))
goto label37 ;
goto label2 ;
label37 :
t = trm41;
{
ATerm trm42 = t;
ATerm m_279 = NULL,n_279 = NULL,o_279 = NULL;
if(match_cons(t, sym_backspace_unit_stmt_3))
{
m_279 = ATgetArgument(t, 0);
n_279 = ATgetArgument(t, 1);
o_279 = ATgetArgument(t, 2);
}
else
goto label38 ;
t = (ATerm) ATmakeAppl(sym_BackspaceStmt_3, m_279, (ATerm)ATinsert(ATempty, (ATerm) ATmakeAppl(sym_UNIT_1, n_279)), o_279);
goto label2 ;
label38 :
t = trm42;
{
ATerm trm43 = t;
ATerm p_279 = NULL,q_279 = NULL,r_279 = NULL;
if(match_cons(t, sym_endfile_unit_stmt_3))
{
p_279 = ATgetArgument(t, 0);
q_279 = ATgetArgument(t, 1);
r_279 = ATgetArgument(t, 2);
}
else
goto label39 ;
t = (ATerm) ATmakeAppl(sym_EndfileStmt_3, p_279, (ATerm)ATinsert(ATempty, (ATerm) ATmakeAppl(sym_UNIT_1, q_279)), r_279);
goto label2 ;
label39 :
t = trm43;
{
ATerm trm44 = t;
ATerm s_279 = NULL,t_279 = NULL,u_279 = NULL;
if(match_cons(t, sym_rewind_unit_stmt_3))
{
s_279 = ATgetArgument(t, 0);
t_279 = ATgetArgument(t, 1);
u_279 = ATgetArgument(t, 2);
}
else
goto label40 ;
t = (ATerm) ATmakeAppl(sym_RewindStmt_3, s_279, (ATerm)ATinsert(ATempty, (ATerm) ATmakeAppl(sym_UNIT_1, t_279)), u_279);
goto label2 ;
label40 :
t = trm44;
{
ATerm trm45 = t;
ATerm v_279 = NULL,w_279 = NULL,x_279 = NULL;
if(match_cons(t, sym_flush_unit_stmt_3))
{
v_279 = ATgetArgument(t, 0);
w_279 = ATgetArgument(t, 1);
x_279 = ATgetArgument(t, 2);
}
else
goto label41 ;
t = (ATerm) ATmakeAppl(sym_FlushStmt_3, v_279, (ATerm)ATinsert(ATempty, (ATerm) ATmakeAppl(sym_UNIT_1, w_279)), x_279);
goto label2 ;
label41 :
t = trm45;
{
ATerm trm46 = t;
ATerm y_279 = NULL;
if(match_cons(t, sym_format_items_first_1))
{
y_279 = ATgetArgument(t, 0);
}
else
goto label42 ;
t = (ATerm) ATinsert(ATempty, y_279);
goto label2 ;
label42 :
t = trm46;
{
ATerm z_279 = NULL,a_280 = NULL;
if(match_cons(t, sym_format_items_2))
{
z_279 = ATgetArgument(t, 0);
a_280 = ATgetArgument(t, 1);
}
else
goto label1 ;
t = (ATerm) ATinsert(ATinsert(ATempty, (ATerm) ATinsert(ATempty, a_280)), z_279);
t = concat_0_0(sl_up(sl), t);
if((t == NULL))
goto label1 ;
t = b_280(sl, sl_fun_cl(0, sl), t);
if((t == NULL))
goto label1 ;
else
goto label2 ;
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
}
label2 :
;
goto label0 ;
label1 :
t = trm0;
goto label0 ;
label0 :
;
}
__tracing_table[--__tracing_table_counter] = NULL;
return(t);
fail3 :
--__tracing_table_counter;
return(NULL);
}
static ATerm b_280 (StrSL sl, StrCL d_280, ATerm t)
{
__tracing_table[__tracing_table_counter++] = "b_280";
__tracing_table[__tracing_table_counter] = NULL;
sl_decl(sl);
sl_funs(1);
sl_init_fun(0, d_280);
{
struct str_closure e_280 = { &(lifted0) , &(frame) };
StrCL lifted0_cl = &(e_280);
t = SRTS_all(sl_up(sl), lifted0_cl, t);
if((t == NULL))
goto fail1 ;
t = cl_fun(d_280)(cl_sl(d_280), t);
if((t == NULL))
goto fail1 ;
}
__tracing_table[--__tracing_table_counter] = NULL;
return(t);
fail1 :
--__tracing_table_counter;
return(NULL);
}
static ATerm lifted0 (StrSL sl, ATerm t)
{
__tracing_table[__tracing_table_counter++] = "lifted0";
__tracing_table[__tracing_table_counter] = NULL;
sl_decl(sl);
t = b_280(sl_up(sl), sl_fun_cl(0, sl), t);
if((t == NULL))
goto fail2 ;
__tracing_table[--__tracing_table_counter] = NULL;
return(t);
fail2 :
--__tracing_table_counter;
return(NULL);
}
static void register_strategies (void)
{
int initial_size = 117;
int max_load = 75;
struct str_closure * closures;
int closures_index = 0;
if((strategy_table == NULL))
strategy_table = ATtableCreate(initial_size, max_load);
closures = (struct str_closure*) malloc((sizeof(struct str_closure) * 8));
if((closures == NULL))
{
perror("malloc error for registration of dynamic strategies");
exit(1);
}
closures[closures_index].fun = &(concat_0_0);
closures[closures_index].sl = NULL;
SRTS_register_function((ATerm)ATmakeAppl0(ATmakeSymbol("concat_0_0", 0, ATtrue)), &(closures[closures_index]));
closures_index++;
closures[closures_index].fun = &(conc_0_0);
closures[closures_index].sl = NULL;
SRTS_register_function((ATerm)ATmakeAppl0(ATmakeSymbol("conc_0_0", 0, ATtrue)), &(closures[closures_index]));
closures_index++;
closures[closures_index].fun = &(io_wrap_1_0);
closures[closures_index].sl = NULL;
SRTS_register_function((ATerm)ATmakeAppl0(ATmakeSymbol("io_wrap_1_0", 0, ATtrue)), &(closures[closures_index]));
closures_index++;
closures[closures_index].fun = &(main_0_0);
closures[closures_index].sl = NULL;
SRTS_register_function((ATerm)ATmakeAppl0(ATmakeSymbol("main_0_0", 0, ATtrue)), &(closures[closures_index]));
closures_index++;
closures[closures_index].fun = &(c_280);
closures[closures_index].sl = NULL;
SRTS_register_function((ATerm)ATmakeAppl0(ATmakeSymbol("c_280", 0, ATtrue)), &(closures[closures_index]));
closures_index++;
closures[closures_index].fun = &(lifted0);
closures[closures_index].sl = NULL;
SRTS_register_function((ATerm)ATmakeAppl0(ATmakeSymbol("lifted0", 0, ATtrue)), &(closures[closures_index]));
closures_index++;
closures[closures_index].fun = &(b_280);
closures[closures_index].sl = NULL;
SRTS_register_function((ATerm)ATmakeAppl0(ATmakeSymbol("b_280", 0, ATtrue)), &(closures[closures_index]));
closures_index++;
closures[closures_index].fun = &(ofp_simplify_0_0);
closures[closures_index].sl = NULL;
SRTS_register_function((ATerm)ATmakeAppl0(ATmakeSymbol("ofp_simplify_0_0", 0, ATtrue)), &(closures[closures_index]));
closures_index++;
}
int main (int argc, char * argv [])
{
ATerm out_term;
ATermList in_term;
int i;
ATinit(argc, argv, &(out_term));
init_constructors();
in_term = ATempty;
for ( i = (argc - 1) ; (i >= 0) ; i-- )
{
in_term = ATinsert(in_term, (ATerm) ATmakeAppl0(ATmakeSymbol(argv[i], 0, ATtrue)));
}
SRTS_stratego_initialize();
memset(__tracing_table, 0, (sizeof(unsigned short) * TRACING_TABLE_SIZE));
__tracing_table_counter = 0;
register_strategies();
out_term = main_0_0(NULL, (ATerm) in_term);
if((out_term != NULL))
{
ATfprintf(stdout, "%t\n", out_term);
exit(0);
}
else
{
ATfprintf(stderr, "%s: rewriting failed, trace:\n", argv[0]);
for ( i = 0 ; (__tracing_table[i] && (i < TRACING_TABLE_SIZE)) ; i++ )
ATfprintf(stderr, "\t%s\n", __tracing_table[i]);
exit(1);
}
}
