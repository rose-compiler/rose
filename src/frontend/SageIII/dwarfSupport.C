// This file represents initial support in the ROSE AST for Dwarf debug information.
// Dwarf is generated when source code is compiled using a compiler's debug mode.
// the Dwarf information is represented a special sections in the file format of 
// the binary executable.  These sections are read using libdwarf (open source
// library used for reading and writing dwarf sections).  In ROSE we read the
// Dwarf information and build special IR nodes into the AST.  This work effects
// only AST for binary executable files (the ROSE Binary AST).

#include "rose.h"

// This is controled by using the --with-dwarf configure command line option.
#if USE_ROSE_DWARF_SUPPORT

#define DIE_STACK_SIZE 300
static Dwarf_Die die_stack[DIE_STACK_SIZE];

#define PUSH_DIE_STACK(x) { die_stack[indent_level] = x; }
#define POP_DIE_STACK { die_stack[indent_level] = 0; }

int indent_level = 0;
bool local_symbols_already_began = false;

bool check_tag_tree = false;

int check_error;

#define DWARF_CHECK_ERROR(str) {\
        printf("*** DWARF CHECK: %s ***\n", str);\
        check_error ++; \
}

#define DWARF_CHECK_ERROR2(str1, str2) {\
        printf("*** DWARF CHECK: %s: %s ***\n", str1, str2);\
        check_error ++; \
}

#define DWARF_CHECK_ERROR3(str1, str2,strexpl) {\
        printf("*** DWARF CHECK: %s -> %s: %s ***\n", str1, str2,strexpl);\
        check_error ++; \
}

typedef struct {
    int checks;
    int errors;
} Dwarf_Check_Result;

Dwarf_Check_Result tag_tree_result;
Dwarf_Check_Result type_offset_result;
Dwarf_Check_Result decl_file_result;

bool info_flag = true;

// Dwarf variables
Dwarf_Debug rose_dwarf_dbg;
Dwarf_Error rose_dwarf_error;

int verbose = 1;
// bool dense = false;

bool ellipsis            = false;

int break_after_n_units = INT_MAX;

char cu_name[BUFSIZ];
bool cu_name_flag = false;
Dwarf_Unsigned cu_offset = 0;

std::string program_name = "roseDwarfReader";


// Allow "string" to be used.
using namespace std;


void
print_error(Dwarf_Debug dbg, string msg, int dwarf_code, Dwarf_Error err)
   {
  // Simple error message function
     printf ("Error: %s \n",msg.c_str());
     ROSE_ASSERT(false);
   }

void
print_source_intro(Dwarf_Die cu_die)
   {
     Dwarf_Off off = 0;
     int ores = dwarf_dieoffset(cu_die, &off, &rose_dwarf_error);

     if (ores == DW_DLV_OK)
        {
          printf("Source lines (from CU-DIE at .debug_info offset %llu):\n",(unsigned long long) off);
        }
       else
        {
          printf("Source lines (for the CU-DIE at unknown location):\n");
        }
   }


char *
makename(char *s)
{
    char *newstr;

    if (!s) {
        return strdup("");
    }

    newstr = strdup(s);
    if (newstr == 0) {
        fprintf(stderr, "Out of memory mallocing %d bytes\n",
                (int) strlen(s));
        exit(1);
    }
    return newstr;
}

string
get_TAG_name (Dwarf_Debug dbg, Dwarf_Half val)
{
	switch (val) {
	case DW_TAG_array_type:
		if (ellipsis)
			return "array_type";
		else
			return "DW_TAG_array_type";
	case DW_TAG_class_type:
		if (ellipsis)
			return "class_type";
		else
			return "DW_TAG_class_type";
	case DW_TAG_entry_point:
		if (ellipsis)
			return "entry_point";
		else
			return "DW_TAG_entry_point";
	case DW_TAG_enumeration_type:
		if (ellipsis)
			return "enumeration_type";
		else
			return "DW_TAG_enumeration_type";
	case DW_TAG_formal_parameter:
		if (ellipsis)
			return "formal_parameter";
		else
			return "DW_TAG_formal_parameter";
	case DW_TAG_imported_declaration:
		if (ellipsis)
			return "imported_declaration";
		else
			return "DW_TAG_imported_declaration";
	case DW_TAG_label:
		if (ellipsis)
			return "label";
		else
			return "DW_TAG_label";
	case DW_TAG_lexical_block:
		if (ellipsis)
			return "lexical_block";
		else
			return "DW_TAG_lexical_block";
	case DW_TAG_member:
		if (ellipsis)
			return "member";
		else
			return "DW_TAG_member";
	case DW_TAG_pointer_type:
		if (ellipsis)
			return "pointer_type";
		else
			return "DW_TAG_pointer_type";
	case DW_TAG_reference_type:
		if (ellipsis)
			return "reference_type";
		else
			return "DW_TAG_reference_type";
	case DW_TAG_compile_unit:
		if (ellipsis)
			return "compile_unit";
		else
			return "DW_TAG_compile_unit";
	case DW_TAG_string_type:
		if (ellipsis)
			return "string_type";
		else
			return "DW_TAG_string_type";
	case DW_TAG_structure_type:
		if (ellipsis)
			return "structure_type";
		else
			return "DW_TAG_structure_type";
	case DW_TAG_subroutine_type:
		if (ellipsis)
			return "subroutine_type";
		else
			return "DW_TAG_subroutine_type";
	case DW_TAG_typedef:
		if (ellipsis)
			return "typedef";
		else
			return "DW_TAG_typedef";
	case DW_TAG_union_type:
		if (ellipsis)
			return "union_type";
		else
			return "DW_TAG_union_type";
	case DW_TAG_unspecified_parameters:
		if (ellipsis)
			return "unspecified_parameters";
		else
			return "DW_TAG_unspecified_parameters";
	case DW_TAG_variant:
		if (ellipsis)
			return "variant";
		else
			return "DW_TAG_variant";
	case DW_TAG_common_block:
		if (ellipsis)
			return "common_block";
		else
			return "DW_TAG_common_block";
	case DW_TAG_common_inclusion:
		if (ellipsis)
			return "common_inclusion";
		else
			return "DW_TAG_common_inclusion";
	case DW_TAG_inheritance:
		if (ellipsis)
			return "inheritance";
		else
			return "DW_TAG_inheritance";
	case DW_TAG_inlined_subroutine:
		if (ellipsis)
			return "inlined_subroutine";
		else
			return "DW_TAG_inlined_subroutine";
	case DW_TAG_module:
		if (ellipsis)
			return "module";
		else
			return "DW_TAG_module";
	case DW_TAG_ptr_to_member_type:
		if (ellipsis)
			return "ptr_to_member_type";
		else
			return "DW_TAG_ptr_to_member_type";
	case DW_TAG_set_type:
		if (ellipsis)
			return "set_type";
		else
			return "DW_TAG_set_type";
	case DW_TAG_subrange_type:
		if (ellipsis)
			return "subrange_type";
		else
			return "DW_TAG_subrange_type";
	case DW_TAG_with_stmt:
		if (ellipsis)
			return "with_stmt";
		else
			return "DW_TAG_with_stmt";
	case DW_TAG_access_declaration:
		if (ellipsis)
			return "access_declaration";
		else
			return "DW_TAG_access_declaration";
	case DW_TAG_base_type:
		if (ellipsis)
			return "base_type";
		else
			return "DW_TAG_base_type";
	case DW_TAG_catch_block:
		if (ellipsis)
			return "catch_block";
		else
			return "DW_TAG_catch_block";
	case DW_TAG_const_type:
		if (ellipsis)
			return "const_type";
		else
			return "DW_TAG_const_type";
	case DW_TAG_constant:
		if (ellipsis)
			return "constant";
		else
			return "DW_TAG_constant";
	case DW_TAG_enumerator:
		if (ellipsis)
			return "enumerator";
		else
			return "DW_TAG_enumerator";
	case DW_TAG_file_type:
		if (ellipsis)
			return "file_type";
		else
			return "DW_TAG_file_type";
	case DW_TAG_friend:
		if (ellipsis)
			return "friend";
		else
			return "DW_TAG_friend";
	case DW_TAG_namelist:
		if (ellipsis)
			return "namelist";
		else
			return "DW_TAG_namelist";
	case DW_TAG_namelist_item:
		if (ellipsis)
			return "namelist_item";
		else
			return "DW_TAG_namelist_item";
	case DW_TAG_packed_type:
		if (ellipsis)
			return "packed_type";
		else
			return "DW_TAG_packed_type";
	case DW_TAG_subprogram:
		if (ellipsis)
			return "subprogram";
		else
			return "DW_TAG_subprogram";
	case DW_TAG_template_type_parameter:
		if (ellipsis)
			return "template_type_parameter";
		else
			return "DW_TAG_template_type_parameter";
	case DW_TAG_template_value_parameter:
		if (ellipsis)
			return "template_value_parameter";
		else
			return "DW_TAG_template_value_parameter";
	case DW_TAG_thrown_type:
		if (ellipsis)
			return "thrown_type";
		else
			return "DW_TAG_thrown_type";
	case DW_TAG_try_block:
		if (ellipsis)
			return "try_block";
		else
			return "DW_TAG_try_block";
	case DW_TAG_variant_part:
		if (ellipsis)
			return "variant_part";
		else
			return "DW_TAG_variant_part";
	case DW_TAG_variable:
		if (ellipsis)
			return "variable";
		else
			return "DW_TAG_variable";
	case DW_TAG_volatile_type:
		if (ellipsis)
			return "volatile_type";
		else
			return "DW_TAG_volatile_type";
	case DW_TAG_dwarf_procedure:
		if (ellipsis)
			return "dwarf_procedure";
		else
			return "DW_TAG_dwarf_procedure";
	case DW_TAG_restrict_type:
		if (ellipsis)
			return "restrict_type";
		else
			return "DW_TAG_restrict_type";
	case DW_TAG_interface_type:
		if (ellipsis)
			return "interface_type";
		else
			return "DW_TAG_interface_type";
	case DW_TAG_namespace:
		if (ellipsis)
			return "namespace";
		else
			return "DW_TAG_namespace";
	case DW_TAG_imported_module:
		if (ellipsis)
			return "imported_module";
		else
			return "DW_TAG_imported_module";
	case DW_TAG_unspecified_type:
		if (ellipsis)
			return "unspecified_type";
		else
			return "DW_TAG_unspecified_type";
	case DW_TAG_partial_unit:
		if (ellipsis)
			return "partial_unit";
		else
			return "DW_TAG_partial_unit";
	case DW_TAG_imported_unit:
		if (ellipsis)
			return "imported_unit";
		else
			return "DW_TAG_imported_unit";
	case DW_TAG_mutable_type:
		if (ellipsis)
			return "mutable_type";
		else
			return "DW_TAG_mutable_type";
	case DW_TAG_condition:
		if (ellipsis)
			return "condition";
		else
			return "DW_TAG_condition";
	case DW_TAG_shared_type:
		if (ellipsis)
			return "shared_type";
		else
			return "DW_TAG_shared_type";
	case DW_TAG_lo_user:
		if (ellipsis)
			return "lo_user";
		else
			return "DW_TAG_lo_user";
	case DW_TAG_MIPS_loop:
		if (ellipsis)
			return "MIPS_loop";
		else
			return "DW_TAG_MIPS_loop";
	case DW_TAG_HP_array_descriptor:
		if (ellipsis)
			return "HP_array_descriptor";
		else
			return "DW_TAG_HP_array_descriptor";
	case DW_TAG_format_label:
		if (ellipsis)
			return "format_label";
		else
			return "DW_TAG_format_label";
	case DW_TAG_function_template:
		if (ellipsis)
			return "function_template";
		else
			return "DW_TAG_function_template";
	case DW_TAG_class_template:
		if (ellipsis)
			return "class_template";
		else
			return "DW_TAG_class_template";
	case DW_TAG_GNU_BINCL:
		if (ellipsis)
			return "GNU_BINCL";
		else
			return "DW_TAG_GNU_BINCL";
	case DW_TAG_GNU_EINCL:
		if (ellipsis)
			return "GNU_EINCL";
		else
			return "DW_TAG_GNU_EINCL";
	case DW_TAG_ALTIUM_circ_type:
		if (ellipsis)
			return "ALTIUM_circ_type";
		else
			return "DW_TAG_ALTIUM_circ_type";
	case DW_TAG_ALTIUM_mwa_circ_type:
		if (ellipsis)
			return "ALTIUM_mwa_circ_type";
		else
			return "DW_TAG_ALTIUM_mwa_circ_type";
	case DW_TAG_ALTIUM_rev_carry_type:
		if (ellipsis)
			return "ALTIUM_rev_carry_type";
		else
			return "DW_TAG_ALTIUM_rev_carry_type";
	case DW_TAG_ALTIUM_rom:
		if (ellipsis)
			return "ALTIUM_rom";
		else
			return "DW_TAG_ALTIUM_rom";
	case DW_TAG_upc_shared_type:
		if (ellipsis)
			return "upc_shared_type";
		else
			return "DW_TAG_upc_shared_type";
	case DW_TAG_upc_strict_type:
		if (ellipsis)
			return "upc_strict_type";
		else
			return "DW_TAG_upc_strict_type";
	case DW_TAG_upc_relaxed_type:
		if (ellipsis)
			return "upc_relaxed_type";
		else
			return "DW_TAG_upc_relaxed_type";
	case DW_TAG_PGI_kanji_type:
		if (ellipsis)
			return "PGI_kanji_type";
		else
			return "DW_TAG_PGI_kanji_type";
	case DW_TAG_PGI_interface_block:
		if (ellipsis)
			return "PGI_interface_block";
		else
			return "DW_TAG_PGI_interface_block";
	case DW_TAG_SUN_function_template:
		if (ellipsis)
			return "SUN_function_template";
		else
			return "DW_TAG_SUN_function_template";
	case DW_TAG_SUN_class_template:
		if (ellipsis)
			return "SUN_class_template";
		else
			return "DW_TAG_SUN_class_template";
	case DW_TAG_SUN_struct_template:
		if (ellipsis)
			return "SUN_struct_template";
		else
			return "DW_TAG_SUN_struct_template";
	case DW_TAG_SUN_union_template:
		if (ellipsis)
			return "SUN_union_template";
		else
			return "DW_TAG_SUN_union_template";
	case DW_TAG_SUN_indirect_inheritance:
		if (ellipsis)
			return "SUN_indirect_inheritance";
		else
			return "DW_TAG_SUN_indirect_inheritance";
	case DW_TAG_SUN_codeflags:
		if (ellipsis)
			return "SUN_codeflags";
		else
			return "DW_TAG_SUN_codeflags";
	case DW_TAG_SUN_memop_info:
		if (ellipsis)
			return "SUN_memop_info";
		else
			return "DW_TAG_SUN_memop_info";
	case DW_TAG_SUN_omp_child_func:
		if (ellipsis)
			return "SUN_omp_child_func";
		else
			return "DW_TAG_SUN_omp_child_func";
	case DW_TAG_SUN_rtti_descriptor:
		if (ellipsis)
			return "SUN_rtti_descriptor";
		else
			return "DW_TAG_SUN_rtti_descriptor";
	case DW_TAG_SUN_dtor_info:
		if (ellipsis)
			return "SUN_dtor_info";
		else
			return "DW_TAG_SUN_dtor_info";
	case DW_TAG_SUN_dtor:
		if (ellipsis)
			return "SUN_dtor";
		else
			return "DW_TAG_SUN_dtor";
	case DW_TAG_SUN_f90_interface:
		if (ellipsis)
			return "SUN_f90_interface";
		else
			return "DW_TAG_SUN_f90_interface";
	case DW_TAG_SUN_fortran_vax_structure:
		if (ellipsis)
			return "SUN_fortran_vax_structure";
		else
			return "DW_TAG_SUN_fortran_vax_structure";
	case DW_TAG_SUN_hi:
		if (ellipsis)
			return "SUN_hi";
		else
			return "DW_TAG_SUN_hi";
	case DW_TAG_hi_user:
		if (ellipsis)
			return "hi_user";
		else
			return "DW_TAG_hi_user";
	default:
		{ 
		  char buf[100]; 
		  char *n; 
		  snprintf(buf,sizeof(buf),"<Unknown TAG value 0x%x>",(int)val);
        fprintf(stderr,"TAG of %d (0x%x) is unknown to dwarfdump. " "Continuing. \n",(int)val,(int)val );  
		  n = makename(buf);
		  return n;
		}
	}
/*NOTREACHED*/
}

#if 0
void
print_attribute(Dwarf_Debug dbg, Dwarf_Die die, Dwarf_Half attr,Dwarf_Attribute attr_in,bool print_information,char **srcfiles, Dwarf_Signed cnt, SgAsmDwarfConstruct* asmDwarfConstruct)
   {
  // This function is not implemented!

     printf ("Error: print_attribute is not implemented! \n");
  // ROSE_ASSERT(false);
   }
#else

string
get_AT_name (Dwarf_Debug dbg, Dwarf_Half val)
{
	switch (val) {
	case DW_AT_sibling:
		if (ellipsis)
			return "sibling";
		else
			return "DW_AT_sibling";
	case DW_AT_location:
		if (ellipsis)
			return "location";
		else
			return "DW_AT_location";
	case DW_AT_name:
		if (ellipsis)
			return "name";
		else
			return "DW_AT_name";
	case DW_AT_ordering:
		if (ellipsis)
			return "ordering";
		else
			return "DW_AT_ordering";
	case DW_AT_subscr_data:
		if (ellipsis)
			return "subscr_data";
		else
			return "DW_AT_subscr_data";
	case DW_AT_byte_size:
		if (ellipsis)
			return "byte_size";
		else
			return "DW_AT_byte_size";
	case DW_AT_bit_offset:
		if (ellipsis)
			return "bit_offset";
		else
			return "DW_AT_bit_offset";
	case DW_AT_bit_size:
		if (ellipsis)
			return "bit_size";
		else
			return "DW_AT_bit_size";
	case DW_AT_element_list:
		if (ellipsis)
			return "element_list";
		else
			return "DW_AT_element_list";
	case DW_AT_stmt_list:
		if (ellipsis)
			return "stmt_list";
		else
			return "DW_AT_stmt_list";
	case DW_AT_low_pc:
		if (ellipsis)
			return "low_pc";
		else
			return "DW_AT_low_pc";
	case DW_AT_high_pc:
		if (ellipsis)
			return "high_pc";
		else
			return "DW_AT_high_pc";
	case DW_AT_language:
		if (ellipsis)
			return "language";
		else
			return "DW_AT_language";
	case DW_AT_member:
		if (ellipsis)
			return "member";
		else
			return "DW_AT_member";
	case DW_AT_discr:
		if (ellipsis)
			return "discr";
		else
			return "DW_AT_discr";
	case DW_AT_discr_value:
		if (ellipsis)
			return "discr_value";
		else
			return "DW_AT_discr_value";
	case DW_AT_visibility:
		if (ellipsis)
			return "visibility";
		else
			return "DW_AT_visibility";
	case DW_AT_import:
		if (ellipsis)
			return "import";
		else
			return "DW_AT_import";
	case DW_AT_string_length:
		if (ellipsis)
			return "string_length";
		else
			return "DW_AT_string_length";
	case DW_AT_common_reference:
		if (ellipsis)
			return "common_reference";
		else
			return "DW_AT_common_reference";
	case DW_AT_comp_dir:
		if (ellipsis)
			return "comp_dir";
		else
			return "DW_AT_comp_dir";
	case DW_AT_const_value:
		if (ellipsis)
			return "const_value";
		else
			return "DW_AT_const_value";
	case DW_AT_containing_type:
		if (ellipsis)
			return "containing_type";
		else
			return "DW_AT_containing_type";
	case DW_AT_default_value:
		if (ellipsis)
			return "default_value";
		else
			return "DW_AT_default_value";
	case DW_AT_inline:
		if (ellipsis)
			return "inline";
		else
			return "DW_AT_inline";
	case DW_AT_is_optional:
		if (ellipsis)
			return "is_optional";
		else
			return "DW_AT_is_optional";
	case DW_AT_lower_bound:
		if (ellipsis)
			return "lower_bound";
		else
			return "DW_AT_lower_bound";
	case DW_AT_producer:
		if (ellipsis)
			return "producer";
		else
			return "DW_AT_producer";
	case DW_AT_prototyped:
		if (ellipsis)
			return "prototyped";
		else
			return "DW_AT_prototyped";
	case DW_AT_return_addr:
		if (ellipsis)
			return "return_addr";
		else
			return "DW_AT_return_addr";
	case DW_AT_start_scope:
		if (ellipsis)
			return "start_scope";
		else
			return "DW_AT_start_scope";
	case DW_AT_bit_stride:
		if (ellipsis)
			return "bit_stride";
		else
			return "DW_AT_bit_stride";
	case DW_AT_upper_bound:
		if (ellipsis)
			return "upper_bound";
		else
			return "DW_AT_upper_bound";
	case DW_AT_abstract_origin:
		if (ellipsis)
			return "abstract_origin";
		else
			return "DW_AT_abstract_origin";
	case DW_AT_accessibility:
		if (ellipsis)
			return "accessibility";
		else
			return "DW_AT_accessibility";
	case DW_AT_address_class:
		if (ellipsis)
			return "address_class";
		else
			return "DW_AT_address_class";
	case DW_AT_artificial:
		if (ellipsis)
			return "artificial";
		else
			return "DW_AT_artificial";
	case DW_AT_base_types:
		if (ellipsis)
			return "base_types";
		else
			return "DW_AT_base_types";
	case DW_AT_calling_convention:
		if (ellipsis)
			return "calling_convention";
		else
			return "DW_AT_calling_convention";
	case DW_AT_count:
		if (ellipsis)
			return "count";
		else
			return "DW_AT_count";
	case DW_AT_data_member_location:
		if (ellipsis)
			return "data_member_location";
		else
			return "DW_AT_data_member_location";
	case DW_AT_decl_column:
		if (ellipsis)
			return "decl_column";
		else
			return "DW_AT_decl_column";
	case DW_AT_decl_file:
		if (ellipsis)
			return "decl_file";
		else
			return "DW_AT_decl_file";
	case DW_AT_decl_line:
		if (ellipsis)
			return "decl_line";
		else
			return "DW_AT_decl_line";
	case DW_AT_declaration:
		if (ellipsis)
			return "declaration";
		else
			return "DW_AT_declaration";
	case DW_AT_discr_list:
		if (ellipsis)
			return "discr_list";
		else
			return "DW_AT_discr_list";
	case DW_AT_encoding:
		if (ellipsis)
			return "encoding";
		else
			return "DW_AT_encoding";
	case DW_AT_external:
		if (ellipsis)
			return "external";
		else
			return "DW_AT_external";
	case DW_AT_frame_base:
		if (ellipsis)
			return "frame_base";
		else
			return "DW_AT_frame_base";
	case DW_AT_friend:
		if (ellipsis)
			return "friend";
		else
			return "DW_AT_friend";
	case DW_AT_identifier_case:
		if (ellipsis)
			return "identifier_case";
		else
			return "DW_AT_identifier_case";
	case DW_AT_macro_info:
		if (ellipsis)
			return "macro_info";
		else
			return "DW_AT_macro_info";
	case DW_AT_namelist_item:
		if (ellipsis)
			return "namelist_item";
		else
			return "DW_AT_namelist_item";
	case DW_AT_priority:
		if (ellipsis)
			return "priority";
		else
			return "DW_AT_priority";
	case DW_AT_segment:
		if (ellipsis)
			return "segment";
		else
			return "DW_AT_segment";
	case DW_AT_specification:
		if (ellipsis)
			return "specification";
		else
			return "DW_AT_specification";
	case DW_AT_static_link:
		if (ellipsis)
			return "static_link";
		else
			return "DW_AT_static_link";
	case DW_AT_type:
		if (ellipsis)
			return "type";
		else
			return "DW_AT_type";
	case DW_AT_use_location:
		if (ellipsis)
			return "use_location";
		else
			return "DW_AT_use_location";
	case DW_AT_variable_parameter:
		if (ellipsis)
			return "variable_parameter";
		else
			return "DW_AT_variable_parameter";
	case DW_AT_virtuality:
		if (ellipsis)
			return "virtuality";
		else
			return "DW_AT_virtuality";
	case DW_AT_vtable_elem_location:
		if (ellipsis)
			return "vtable_elem_location";
		else
			return "DW_AT_vtable_elem_location";
	case DW_AT_allocated:
		if (ellipsis)
			return "allocated";
		else
			return "DW_AT_allocated";
	case DW_AT_associated:
		if (ellipsis)
			return "associated";
		else
			return "DW_AT_associated";
	case DW_AT_data_location:
		if (ellipsis)
			return "data_location";
		else
			return "DW_AT_data_location";
	case DW_AT_byte_stride:
		if (ellipsis)
			return "byte_stride";
		else
			return "DW_AT_byte_stride";
	case DW_AT_entry_pc:
		if (ellipsis)
			return "entry_pc";
		else
			return "DW_AT_entry_pc";
	case DW_AT_use_UTF8:
		if (ellipsis)
			return "use_UTF8";
		else
			return "DW_AT_use_UTF8";
	case DW_AT_extension:
		if (ellipsis)
			return "extension";
		else
			return "DW_AT_extension";
	case DW_AT_ranges:
		if (ellipsis)
			return "ranges";
		else
			return "DW_AT_ranges";
	case DW_AT_trampoline:
		if (ellipsis)
			return "trampoline";
		else
			return "DW_AT_trampoline";
	case DW_AT_call_column:
		if (ellipsis)
			return "call_column";
		else
			return "DW_AT_call_column";
	case DW_AT_call_file:
		if (ellipsis)
			return "call_file";
		else
			return "DW_AT_call_file";
	case DW_AT_call_line:
		if (ellipsis)
			return "call_line";
		else
			return "DW_AT_call_line";
	case DW_AT_description:
		if (ellipsis)
			return "description";
		else
			return "DW_AT_description";
	case DW_AT_binary_scale:
		if (ellipsis)
			return "binary_scale";
		else
			return "DW_AT_binary_scale";
	case DW_AT_decimal_scale:
		if (ellipsis)
			return "decimal_scale";
		else
			return "DW_AT_decimal_scale";
	case DW_AT_small:
		if (ellipsis)
			return "small";
		else
			return "DW_AT_small";
	case DW_AT_decimal_sign:
		if (ellipsis)
			return "decimal_sign";
		else
			return "DW_AT_decimal_sign";
	case DW_AT_digit_count:
		if (ellipsis)
			return "digit_count";
		else
			return "DW_AT_digit_count";
	case DW_AT_picture_string:
		if (ellipsis)
			return "picture_string";
		else
			return "DW_AT_picture_string";
	case DW_AT_mutable:
		if (ellipsis)
			return "mutable";
		else
			return "DW_AT_mutable";
	case DW_AT_threads_scaled:
		if (ellipsis)
			return "threads_scaled";
		else
			return "DW_AT_threads_scaled";
	case DW_AT_explicit:
		if (ellipsis)
			return "explicit";
		else
			return "DW_AT_explicit";
	case DW_AT_object_pointer:
		if (ellipsis)
			return "object_pointer";
		else
			return "DW_AT_object_pointer";
	case DW_AT_endianity:
		if (ellipsis)
			return "endianity";
		else
			return "DW_AT_endianity";
	case DW_AT_elemental:
		if (ellipsis)
			return "elemental";
		else
			return "DW_AT_elemental";
	case DW_AT_pure:
		if (ellipsis)
			return "pure";
		else
			return "DW_AT_pure";
	case DW_AT_recursive:
		if (ellipsis)
			return "recursive";
		else
			return "DW_AT_recursive";
	case DW_AT_HP_block_index:
		if (ellipsis)
			return "HP_block_index";
		else
			return "DW_AT_HP_block_index";
	case DW_AT_MIPS_fde:
		if (ellipsis)
			return "MIPS_fde";
		else
			return "DW_AT_MIPS_fde";
	case DW_AT_MIPS_loop_begin:
		if (ellipsis)
			return "MIPS_loop_begin";
		else
			return "DW_AT_MIPS_loop_begin";
	case DW_AT_MIPS_tail_loop_begin:
		if (ellipsis)
			return "MIPS_tail_loop_begin";
		else
			return "DW_AT_MIPS_tail_loop_begin";
	case DW_AT_MIPS_epilog_begin:
		if (ellipsis)
			return "MIPS_epilog_begin";
		else
			return "DW_AT_MIPS_epilog_begin";
	case DW_AT_MIPS_loop_unroll_factor:
		if (ellipsis)
			return "MIPS_loop_unroll_factor";
		else
			return "DW_AT_MIPS_loop_unroll_factor";
	case DW_AT_MIPS_software_pipeline_depth:
		if (ellipsis)
			return "MIPS_software_pipeline_depth";
		else
			return "DW_AT_MIPS_software_pipeline_depth";
	case DW_AT_MIPS_linkage_name:
		if (ellipsis)
			return "MIPS_linkage_name";
		else
			return "DW_AT_MIPS_linkage_name";
	case DW_AT_MIPS_stride:
		if (ellipsis)
			return "MIPS_stride";
		else
			return "DW_AT_MIPS_stride";
	case DW_AT_MIPS_abstract_name:
		if (ellipsis)
			return "MIPS_abstract_name";
		else
			return "DW_AT_MIPS_abstract_name";
	case DW_AT_MIPS_clone_origin:
		if (ellipsis)
			return "MIPS_clone_origin";
		else
			return "DW_AT_MIPS_clone_origin";
	case DW_AT_MIPS_has_inlines:
		if (ellipsis)
			return "MIPS_has_inlines";
		else
			return "DW_AT_MIPS_has_inlines";
	case DW_AT_MIPS_stride_byte:
		if (ellipsis)
			return "MIPS_stride_byte";
		else
			return "DW_AT_MIPS_stride_byte";
	case DW_AT_MIPS_stride_elem:
		if (ellipsis)
			return "MIPS_stride_elem";
		else
			return "DW_AT_MIPS_stride_elem";
	case DW_AT_MIPS_ptr_dopetype:
		if (ellipsis)
			return "MIPS_ptr_dopetype";
		else
			return "DW_AT_MIPS_ptr_dopetype";
	case DW_AT_MIPS_allocatable_dopetype:
		if (ellipsis)
			return "MIPS_allocatable_dopetype";
		else
			return "DW_AT_MIPS_allocatable_dopetype";
	case DW_AT_MIPS_assumed_shape_dopetype:
		if (ellipsis)
			return "MIPS_assumed_shape_dopetype";
		else
			return "DW_AT_MIPS_assumed_shape_dopetype";
	case DW_AT_MIPS_assumed_size:
		if (ellipsis)
			return "MIPS_assumed_size";
		else
			return "DW_AT_MIPS_assumed_size";
	case DW_AT_HP_raw_data_ptr:
		if (ellipsis)
			return "HP_raw_data_ptr";
		else
			return "DW_AT_HP_raw_data_ptr";
	case DW_AT_HP_pass_by_reference:
		if (ellipsis)
			return "HP_pass_by_reference";
		else
			return "DW_AT_HP_pass_by_reference";
	case DW_AT_HP_opt_level:
		if (ellipsis)
			return "HP_opt_level";
		else
			return "DW_AT_HP_opt_level";
	case DW_AT_HP_prof_version_id:
		if (ellipsis)
			return "HP_prof_version_id";
		else
			return "DW_AT_HP_prof_version_id";
	case DW_AT_HP_opt_flags:
		if (ellipsis)
			return "HP_opt_flags";
		else
			return "DW_AT_HP_opt_flags";
	case DW_AT_HP_cold_region_low_pc:
		if (ellipsis)
			return "HP_cold_region_low_pc";
		else
			return "DW_AT_HP_cold_region_low_pc";
	case DW_AT_HP_cold_region_high_pc:
		if (ellipsis)
			return "HP_cold_region_high_pc";
		else
			return "DW_AT_HP_cold_region_high_pc";
	case DW_AT_HP_all_variables_modifiable:
		if (ellipsis)
			return "HP_all_variables_modifiable";
		else
			return "DW_AT_HP_all_variables_modifiable";
	case DW_AT_HP_linkage_name:
		if (ellipsis)
			return "HP_linkage_name";
		else
			return "DW_AT_HP_linkage_name";
	case DW_AT_HP_prof_flags:
		if (ellipsis)
			return "HP_prof_flags";
		else
			return "DW_AT_HP_prof_flags";
	case DW_AT_sf_names:
		if (ellipsis)
			return "sf_names";
		else
			return "DW_AT_sf_names";
	case DW_AT_src_info:
		if (ellipsis)
			return "src_info";
		else
			return "DW_AT_src_info";
	case DW_AT_mac_info:
		if (ellipsis)
			return "mac_info";
		else
			return "DW_AT_mac_info";
	case DW_AT_src_coords:
		if (ellipsis)
			return "src_coords";
		else
			return "DW_AT_src_coords";
	case DW_AT_body_begin:
		if (ellipsis)
			return "body_begin";
		else
			return "DW_AT_body_begin";
	case DW_AT_body_end:
		if (ellipsis)
			return "body_end";
		else
			return "DW_AT_body_end";
	case DW_AT_GNU_vector:
		if (ellipsis)
			return "GNU_vector";
		else
			return "DW_AT_GNU_vector";
	case DW_AT_ALTIUM_loclist:
		if (ellipsis)
			return "ALTIUM_loclist";
		else
			return "DW_AT_ALTIUM_loclist";
	case DW_AT_PGI_lbase:
		if (ellipsis)
			return "PGI_lbase";
		else
			return "DW_AT_PGI_lbase";
	case DW_AT_PGI_soffset:
		if (ellipsis)
			return "PGI_soffset";
		else
			return "DW_AT_PGI_soffset";
	case DW_AT_PGI_lstride:
		if (ellipsis)
			return "PGI_lstride";
		else
			return "DW_AT_PGI_lstride";
	case DW_AT_upc_threads_scaled:
		if (ellipsis)
			return "upc_threads_scaled";
		else
			return "DW_AT_upc_threads_scaled";
	case DW_AT_SUN_template:
		if (ellipsis)
			return "SUN_template";
		else
			return "DW_AT_SUN_template";
	case DW_AT_SUN_alignment:
		if (ellipsis)
			return "SUN_alignment";
		else
			return "DW_AT_SUN_alignment";
	case DW_AT_SUN_vtable:
		if (ellipsis)
			return "SUN_vtable";
		else
			return "DW_AT_SUN_vtable";
	case DW_AT_SUN_count_guarantee:
		if (ellipsis)
			return "SUN_count_guarantee";
		else
			return "DW_AT_SUN_count_guarantee";
	case DW_AT_SUN_command_line:
		if (ellipsis)
			return "SUN_command_line";
		else
			return "DW_AT_SUN_command_line";
	case DW_AT_SUN_vbase:
		if (ellipsis)
			return "SUN_vbase";
		else
			return "DW_AT_SUN_vbase";
	case DW_AT_SUN_compile_options:
		if (ellipsis)
			return "SUN_compile_options";
		else
			return "DW_AT_SUN_compile_options";
	case DW_AT_SUN_language:
		if (ellipsis)
			return "SUN_language";
		else
			return "DW_AT_SUN_language";
	case DW_AT_SUN_browser_file:
		if (ellipsis)
			return "SUN_browser_file";
		else
			return "DW_AT_SUN_browser_file";
	case DW_AT_SUN_vtable_abi:
		if (ellipsis)
			return "SUN_vtable_abi";
		else
			return "DW_AT_SUN_vtable_abi";
	case DW_AT_SUN_func_offsets:
		if (ellipsis)
			return "SUN_func_offsets";
		else
			return "DW_AT_SUN_func_offsets";
	case DW_AT_SUN_cf_kind:
		if (ellipsis)
			return "SUN_cf_kind";
		else
			return "DW_AT_SUN_cf_kind";
	case DW_AT_SUN_vtable_index:
		if (ellipsis)
			return "SUN_vtable_index";
		else
			return "DW_AT_SUN_vtable_index";
	case DW_AT_SUN_omp_tpriv_addr:
		if (ellipsis)
			return "SUN_omp_tpriv_addr";
		else
			return "DW_AT_SUN_omp_tpriv_addr";
	case DW_AT_SUN_omp_child_func:
		if (ellipsis)
			return "SUN_omp_child_func";
		else
			return "DW_AT_SUN_omp_child_func";
	case DW_AT_SUN_func_offset:
		if (ellipsis)
			return "SUN_func_offset";
		else
			return "DW_AT_SUN_func_offset";
	case DW_AT_SUN_memop_type_ref:
		if (ellipsis)
			return "SUN_memop_type_ref";
		else
			return "DW_AT_SUN_memop_type_ref";
	case DW_AT_SUN_profile_id:
		if (ellipsis)
			return "SUN_profile_id";
		else
			return "DW_AT_SUN_profile_id";
	case DW_AT_SUN_memop_signature:
		if (ellipsis)
			return "SUN_memop_signature";
		else
			return "DW_AT_SUN_memop_signature";
	case DW_AT_SUN_obj_dir:
		if (ellipsis)
			return "SUN_obj_dir";
		else
			return "DW_AT_SUN_obj_dir";
	case DW_AT_SUN_obj_file:
		if (ellipsis)
			return "SUN_obj_file";
		else
			return "DW_AT_SUN_obj_file";
	case DW_AT_SUN_original_name:
		if (ellipsis)
			return "SUN_original_name";
		else
			return "DW_AT_SUN_original_name";
	case DW_AT_SUN_hwcprof_signature:
		if (ellipsis)
			return "SUN_hwcprof_signature";
		else
			return "DW_AT_SUN_hwcprof_signature";
	case DW_AT_SUN_amd64_parmdump:
		if (ellipsis)
			return "SUN_amd64_parmdump";
		else
			return "DW_AT_SUN_amd64_parmdump";
	case DW_AT_SUN_part_link_name:
		if (ellipsis)
			return "SUN_part_link_name";
		else
			return "DW_AT_SUN_part_link_name";
	case DW_AT_SUN_link_name:
		if (ellipsis)
			return "SUN_link_name";
		else
			return "DW_AT_SUN_link_name";
	case DW_AT_SUN_pass_with_const:
		if (ellipsis)
			return "SUN_pass_with_const";
		else
			return "DW_AT_SUN_pass_with_const";
	case DW_AT_SUN_return_with_const:
		if (ellipsis)
			return "SUN_return_with_const";
		else
			return "DW_AT_SUN_return_with_const";
	case DW_AT_SUN_import_by_name:
		if (ellipsis)
			return "SUN_import_by_name";
		else
			return "DW_AT_SUN_import_by_name";
	case DW_AT_SUN_f90_pointer:
		if (ellipsis)
			return "SUN_f90_pointer";
		else
			return "DW_AT_SUN_f90_pointer";
	case DW_AT_SUN_pass_by_ref:
		if (ellipsis)
			return "SUN_pass_by_ref";
		else
			return "DW_AT_SUN_pass_by_ref";
	case DW_AT_SUN_f90_allocatable:
		if (ellipsis)
			return "SUN_f90_allocatable";
		else
			return "DW_AT_SUN_f90_allocatable";
	case DW_AT_SUN_f90_assumed_shape_array:
		if (ellipsis)
			return "SUN_f90_assumed_shape_array";
		else
			return "DW_AT_SUN_f90_assumed_shape_array";
	case DW_AT_SUN_c_vla:
		if (ellipsis)
			return "SUN_c_vla";
		else
			return "DW_AT_SUN_c_vla";
	case DW_AT_SUN_return_value_ptr:
		if (ellipsis)
			return "SUN_return_value_ptr";
		else
			return "DW_AT_SUN_return_value_ptr";
	case DW_AT_SUN_dtor_start:
		if (ellipsis)
			return "SUN_dtor_start";
		else
			return "DW_AT_SUN_dtor_start";
	case DW_AT_SUN_dtor_length:
		if (ellipsis)
			return "SUN_dtor_length";
		else
			return "DW_AT_SUN_dtor_length";
	case DW_AT_SUN_dtor_state_initial:
		if (ellipsis)
			return "SUN_dtor_state_initial";
		else
			return "DW_AT_SUN_dtor_state_initial";
	case DW_AT_SUN_dtor_state_final:
		if (ellipsis)
			return "SUN_dtor_state_final";
		else
			return "DW_AT_SUN_dtor_state_final";
	case DW_AT_SUN_dtor_state_deltas:
		if (ellipsis)
			return "SUN_dtor_state_deltas";
		else
			return "DW_AT_SUN_dtor_state_deltas";
	case DW_AT_SUN_import_by_lname:
		if (ellipsis)
			return "SUN_import_by_lname";
		else
			return "DW_AT_SUN_import_by_lname";
	case DW_AT_SUN_f90_use_only:
		if (ellipsis)
			return "SUN_f90_use_only";
		else
			return "DW_AT_SUN_f90_use_only";
	case DW_AT_SUN_namelist_spec:
		if (ellipsis)
			return "SUN_namelist_spec";
		else
			return "DW_AT_SUN_namelist_spec";
	case DW_AT_SUN_is_omp_child_func:
		if (ellipsis)
			return "SUN_is_omp_child_func";
		else
			return "DW_AT_SUN_is_omp_child_func";
	case DW_AT_SUN_fortran_main_alias:
		if (ellipsis)
			return "SUN_fortran_main_alias";
		else
			return "DW_AT_SUN_fortran_main_alias";
	case DW_AT_SUN_fortran_based:
		if (ellipsis)
			return "SUN_fortran_based";
		else
			return "DW_AT_SUN_fortran_based";
	case DW_AT_hi_user:
		if (ellipsis)
			return "hi_user";
		else
			return "DW_AT_hi_user";
	default:
		{ 
		    char buf[100]; 
		    char *n; 
		    snprintf(buf,sizeof(buf),"<Unknown AT value 0x%x>",(int)val);
		 fprintf(stderr,"AT of %d (0x%x) is unknown to dwarfdump. " 
 		 "Continuing. \n",(int)val,(int)val );  
		    n = makename(buf);
		    return n; 
		} 
	}
/*NOTREACHED*/
}

string
get_LANG_name (Dwarf_Debug dbg, Dwarf_Half val)
{
	switch (val) {
	case DW_LANG_C89:
		if (ellipsis)
			return "C89";
		else
			return "DW_LANG_C89";
	case DW_LANG_C:
		if (ellipsis)
			return "C";
		else
			return "DW_LANG_C";
	case DW_LANG_Ada83:
		if (ellipsis)
			return "Ada83";
		else
			return "DW_LANG_Ada83";
	case DW_LANG_C_plus_plus:
		if (ellipsis)
			return "C_plus_plus";
		else
			return "DW_LANG_C_plus_plus";
	case DW_LANG_Cobol74:
		if (ellipsis)
			return "Cobol74";
		else
			return "DW_LANG_Cobol74";
	case DW_LANG_Cobol85:
		if (ellipsis)
			return "Cobol85";
		else
			return "DW_LANG_Cobol85";
	case DW_LANG_Fortran77:
		if (ellipsis)
			return "Fortran77";
		else
			return "DW_LANG_Fortran77";
	case DW_LANG_Fortran90:
		if (ellipsis)
			return "Fortran90";
		else
			return "DW_LANG_Fortran90";
	case DW_LANG_Pascal83:
		if (ellipsis)
			return "Pascal83";
		else
			return "DW_LANG_Pascal83";
	case DW_LANG_Modula2:
		if (ellipsis)
			return "Modula2";
		else
			return "DW_LANG_Modula2";
	case DW_LANG_Java:
		if (ellipsis)
			return "Java";
		else
			return "DW_LANG_Java";
	case DW_LANG_C99:
		if (ellipsis)
			return "C99";
		else
			return "DW_LANG_C99";
	case DW_LANG_Ada95:
		if (ellipsis)
			return "Ada95";
		else
			return "DW_LANG_Ada95";
	case DW_LANG_Fortran95:
		if (ellipsis)
			return "Fortran95";
		else
			return "DW_LANG_Fortran95";
	case DW_LANG_PLI:
		if (ellipsis)
			return "PLI";
		else
			return "DW_LANG_PLI";
	case DW_LANG_ObjC:
		if (ellipsis)
			return "ObjC";
		else
			return "DW_LANG_ObjC";
	case DW_LANG_ObjC_plus_plus:
		if (ellipsis)
			return "ObjC_plus_plus";
		else
			return "DW_LANG_ObjC_plus_plus";
	case DW_LANG_UPC:
		if (ellipsis)
			return "UPC";
		else
			return "DW_LANG_UPC";
	case DW_LANG_D:
		if (ellipsis)
			return "D";
		else
			return "DW_LANG_D";
	case DW_LANG_lo_user:
		if (ellipsis)
			return "lo_user";
		else
			return "DW_LANG_lo_user";
	case DW_LANG_Mips_Assembler:
		if (ellipsis)
			return "Mips_Assembler";
		else
			return "DW_LANG_Mips_Assembler";
	case DW_LANG_Upc:
		if (ellipsis)
			return "Upc";
		else
			return "DW_LANG_Upc";
	case DW_LANG_ALTIUM_Assembler:
		if (ellipsis)
			return "ALTIUM_Assembler";
		else
			return "DW_LANG_ALTIUM_Assembler";
	case DW_LANG_SUN_Assembler:
		if (ellipsis)
			return "SUN_Assembler";
		else
			return "DW_LANG_SUN_Assembler";
	case DW_LANG_hi_user:
		if (ellipsis)
			return "hi_user";
		else
			return "DW_LANG_hi_user";
	default:
		{ 
		    char buf[100]; 
		    char *n; 
		    snprintf(buf,sizeof(buf),"<Unknown LANG value 0x%x>",(int)val);
		 fprintf(stderr,"LANG of %d (0x%x) is unknown to dwarfdump. " 
 		 "Continuing. \n",(int)val,(int)val );  
		    n = makename(buf);
		    return n; 
		} 
	}
/*NOTREACHED*/
}

typedef string(*encoding_type_func) (Dwarf_Debug dbg, Dwarf_Half val);

int
get_small_encoding_integer_and_name( Dwarf_Debug dbg, Dwarf_Attribute attrib, Dwarf_Unsigned * uval_out, char *attr_name, string * string_out, encoding_type_func val_as_string, Dwarf_Error * err)
   {
     Dwarf_Unsigned uval = 0;
     char buf[100];              /* The strings are small. */
     int vres = dwarf_formudata(attrib, &uval, err);

     if (vres != DW_DLV_OK)
        {
          Dwarf_Signed sval = 0;

          vres = dwarf_formsdata(attrib, &sval, err);
          if (vres != DW_DLV_OK)
             {
               if (string_out != 0)
                  {
                    snprintf(buf, sizeof(buf),"%s has a bad form.", attr_name);
                    *string_out = makename(buf);
                  }
               return vres;
             }
          *uval_out = (Dwarf_Unsigned) sval;
        }
       else
        {
          *uval_out = uval;
        }

     if (string_out)
          *string_out = val_as_string(dbg, (Dwarf_Half) uval);

     return DW_DLV_OK;
   }

// static void formx_unsigned(Dwarf_Unsigned u, struct esb_s *esbp)
static void formx_unsigned(Dwarf_Unsigned u, string *esbp)
   {
     char small_buf[40];
     snprintf(small_buf, sizeof(small_buf),
      "%llu", (unsigned long long)u);
  // esb_append(esbp, small_buf);
     *esbp += small_buf;
   }

// static void formx_signed(Dwarf_Signed u, struct esb_s *esbp)
void formx_signed(Dwarf_Signed u, string *esbp)
   {
     char small_buf[40];
     snprintf(small_buf, sizeof(small_buf),
      "%lld", (long long)u);
  // esb_append(esbp, small_buf);
     *esbp += small_buf;
   }

/* We think this is an integer. Figure out how to print it.
   In case the signedness is ambiguous (such as on 
   DW_FORM_data1 (ie, unknown signedness) print two ways.
*/
// static int formxdata_print_value(Dwarf_Attribute attrib, struct esb_s *esbp, Dwarf_Error * err)
int formxdata_print_value(Dwarf_Attribute attrib, string *esbp, Dwarf_Error * err)
   {
    Dwarf_Signed tempsd = 0;
    Dwarf_Unsigned tempud = 0;
    int sres = 0;
    int ures = 0;
    Dwarf_Error serr = 0;
    ures = dwarf_formudata(attrib, &tempud, err);
    sres = dwarf_formsdata(attrib, &tempsd, &serr);
    if(ures == DW_DLV_OK) {
      if(sres == DW_DLV_OK) {

     // if(tempud == tempsd)
        if((Dwarf_Signed)tempud == tempsd)
        {
           /* Data is the same value, so makes no difference which
                we print. */
           formx_unsigned(tempud,esbp);
        } else {
           formx_unsigned(tempud,esbp);
        // esb_append(esbp,"(as signed = ");
           *esbp += "(as signed = ";
           formx_signed(tempsd,esbp);
        // esb_append(esbp,")");
           *esbp += ")";
        }
      } else if (sres == DW_DLV_NO_ENTRY) {
        formx_unsigned(tempud,esbp);
      } else /* DW_DLV_ERROR */{
        formx_unsigned(tempud,esbp);
      }
      return DW_DLV_OK;
    } else  if (ures == DW_DLV_NO_ENTRY) {
      if(sres == DW_DLV_OK) {
        formx_signed(tempsd,esbp);
        return sres;
      } else if (sres == DW_DLV_NO_ENTRY) {
        return sres;
      } else /* DW_DLV_ERROR */{
        *err = serr;
        return sres;
      }
    } 
    /* else ures ==  DW_DLV_ERROR */ 
    if(sres == DW_DLV_OK) {
        formx_signed(tempsd,esbp);
    } else if (sres == DW_DLV_NO_ENTRY) {
        return ures;
    } 
    /* DW_DLV_ERROR */
    return ures;
}



/* Fill buffer with attribute value.
   We pass in tag so we can try to do the right thing with
   broken compiler DW_TAG_enumerator 

   We append to esbp's buffer.

*/
// static void get_attr_value(Dwarf_Debug dbg, Dwarf_Half tag, Dwarf_Attribute attrib,char **srcfiles, Dwarf_Signed cnt, struct esb_s *esbp)
void get_attr_value(Dwarf_Debug dbg, Dwarf_Half tag, Dwarf_Attribute attrib,char **srcfiles, Dwarf_Signed cnt, string *esbp)
{
    Dwarf_Half theform;
    char* temps;
    Dwarf_Block *tempb;
    Dwarf_Signed tempsd = 0;
    Dwarf_Unsigned tempud = 0;
    int i;
    Dwarf_Half attr;
    Dwarf_Off off;
    Dwarf_Die die_for_check;
    Dwarf_Half tag_for_check;
    Dwarf_Bool tempbool;
    Dwarf_Addr addr = 0;
    int fres;
    int bres;
    int wres;
    int dres;
    Dwarf_Half direct_form = 0;
    char small_buf[100];

  // DQ: Added these here instead of in global scope
     Dwarf_Off fde_offset_for_cu_low = DW_DLV_BADOFFSET;
     Dwarf_Off fde_offset_for_cu_high = DW_DLV_BADOFFSET;


    fres = dwarf_whatform(attrib, &theform, &rose_dwarf_error);
    /* depending on the form and the attribute, process the form */
    if (fres == DW_DLV_ERROR) {
        print_error(dbg, "dwarf_whatform cannot find attr form", fres,rose_dwarf_error);
    } else if (fres == DW_DLV_NO_ENTRY) {
        return;
    }

    dwarf_whatform_direct(attrib, &direct_form, &rose_dwarf_error);
    /* ignore errors in dwarf_whatform_direct() */


    switch (theform) {
    case DW_FORM_addr:
        bres = dwarf_formaddr(attrib, &addr, &rose_dwarf_error);
        if (bres == DW_DLV_OK) {
            snprintf(small_buf, sizeof(small_buf), "%#llx",
                     (unsigned long long) addr);
         // esb_append(esbp, small_buf);
            *esbp += small_buf;
        } else {
            print_error(dbg, "addr formwith no addr?!", bres, rose_dwarf_error);
        }
        break;
    case DW_FORM_ref_addr:
        /* DW_FORM_ref_addr is not accessed thru formref: ** it is an
           address (global section offset) in ** the .debug_info
           section. */
        bres = dwarf_global_formref(attrib, &off, &rose_dwarf_error);
        if (bres == DW_DLV_OK) {
            snprintf(small_buf, sizeof(small_buf),
                     "<global die offset %llu>",
                     (unsigned long long) off);
         // esb_append(esbp, small_buf);
            *esbp += small_buf;
        } else {
            print_error(dbg,
                        "DW_FORM_ref_addr form with no reference?!",
                        bres, rose_dwarf_error);
        }
        break;
    case DW_FORM_ref1:
    case DW_FORM_ref2:
    case DW_FORM_ref4:
    case DW_FORM_ref8:
    case DW_FORM_ref_udata:
       {
        bres = dwarf_formref(attrib, &off, &rose_dwarf_error);
        if (bres != DW_DLV_OK) {
            print_error(dbg, "ref formwith no ref?!", bres, rose_dwarf_error);
        }
        /* do references inside <> to distinguish them ** from
           constants. In dense form this results in <<>>. Ugly for
           dense form, but better than ambiguous. davea 9/94 */
        snprintf(small_buf, sizeof(small_buf), "<%llu>", off);
     // esb_append(esbp, small_buf);
        *esbp += small_buf;

     // DQ: Added bool directly
        bool check_type_offset = false;

        if (check_type_offset) {
            wres = dwarf_whatattr(attrib, &attr, &rose_dwarf_error);
            if (wres == DW_DLV_ERROR) {

            } else if (wres == DW_DLV_NO_ENTRY) {
            }
            if (attr == DW_AT_type) {
                dres = dwarf_offdie(dbg, cu_offset + off,
                                    &die_for_check, &rose_dwarf_error);
                type_offset_result.checks++;
                if (dres != DW_DLV_OK) {
                    type_offset_result.errors++;
                    DWARF_CHECK_ERROR
                        ("DW_AT_type offset does not point to type info")
                } else {
                    int tres2;

                    tres2 =
                        dwarf_tag(die_for_check, &tag_for_check, &rose_dwarf_error);
                    if (tres2 == DW_DLV_OK) {
                        switch (tag_for_check) {
                        case DW_TAG_array_type:
                        case DW_TAG_class_type:
                        case DW_TAG_enumeration_type:
                        case DW_TAG_pointer_type:
                        case DW_TAG_reference_type:
                        case DW_TAG_string_type:
                        case DW_TAG_structure_type:
                        case DW_TAG_subroutine_type:
                        case DW_TAG_typedef:
                        case DW_TAG_union_type:
                        case DW_TAG_ptr_to_member_type:
                        case DW_TAG_set_type:
                        case DW_TAG_subrange_type:
                        case DW_TAG_base_type:
                        case DW_TAG_const_type:
                        case DW_TAG_file_type:
                        case DW_TAG_packed_type:
                        case DW_TAG_thrown_type:
                        case DW_TAG_volatile_type:
                            /* OK */
                            break;
                        default:
                            type_offset_result.errors++;
                            DWARF_CHECK_ERROR
                                ("DW_AT_type offset does not point to type info")
                                break;
                        }
                        dwarf_dealloc(dbg, die_for_check, DW_DLA_DIE);
                    } else {
                        type_offset_result.errors++;
                        DWARF_CHECK_ERROR
                            ("DW_AT_type offset does not exist")
                    }
                }
            }
        }
        break;
       }
    case DW_FORM_block:
    case DW_FORM_block1:
    case DW_FORM_block2:
    case DW_FORM_block4:
        fres = dwarf_formblock(attrib, &tempb, &rose_dwarf_error);
        if (fres == DW_DLV_OK) {
         // for (i = 0; i < tempb->bl_len; i++) {
            for (i = 0; i < (int) tempb->bl_len; i++) {
                snprintf(small_buf, sizeof(small_buf), "%02x",
                         *(i + (unsigned char *) tempb->bl_data));
             // esb_append(esbp, small_buf);
                *esbp += small_buf;
            }
            dwarf_dealloc(dbg, tempb, DW_DLA_BLOCK);
        } else {
            print_error(dbg, "DW_FORM_blockn cannot get block\n", fres,
                        rose_dwarf_error);
        }
        break;
    case DW_FORM_data1:
    case DW_FORM_data2:
    case DW_FORM_data4:
    case DW_FORM_data8:
        fres = dwarf_whatattr(attrib, &attr, &rose_dwarf_error);
        if (fres == DW_DLV_ERROR) {
            print_error(dbg, "FORM_datan cannot get attr", fres, rose_dwarf_error);
        } else if (fres == DW_DLV_NO_ENTRY) {
            print_error(dbg, "FORM_datan cannot get attr", fres, rose_dwarf_error);
        } else {
            switch (attr) {
            case DW_AT_ordering:
            case DW_AT_byte_size:
            case DW_AT_bit_offset:
            case DW_AT_bit_size:
            case DW_AT_inline:
            case DW_AT_language:
            case DW_AT_visibility:
            case DW_AT_virtuality:
            case DW_AT_accessibility:
            case DW_AT_address_class:
            case DW_AT_calling_convention:
            case DW_AT_discr_list:      /* DWARF3 */
            case DW_AT_encoding:
            case DW_AT_identifier_case:
            case DW_AT_MIPS_loop_unroll_factor:
            case DW_AT_MIPS_software_pipeline_depth:
            case DW_AT_decl_column:
            case DW_AT_decl_file:
            case DW_AT_decl_line:
            case DW_AT_call_column:
            case DW_AT_call_file:
            case DW_AT_call_line:
            case DW_AT_start_scope:
            case DW_AT_byte_stride:
            case DW_AT_bit_stride:
            case DW_AT_count:
            case DW_AT_stmt_list:
            case DW_AT_MIPS_fde:
               {
                wres = get_small_encoding_integer_and_name(dbg,
                                                           attrib,
                                                           &tempud,
                                                           /* attrname */
                                                           (char *) NULL,
                                                           /* err_string 
                                                            */ 
                                                           (string*)NULL,
                                                           (encoding_type_func) 0,
                                                           &rose_dwarf_error);

                if (wres == DW_DLV_OK) {
                    snprintf(small_buf, sizeof(small_buf), "%llu",tempud);
                 // esb_append(esbp, small_buf);
                    *esbp += small_buf;

                    if (attr == DW_AT_decl_file || attr == DW_AT_call_file) {
                     // if (srcfiles && tempud > 0 && tempud <= cnt)
                        if (srcfiles && (int)tempud > 0 && (int)tempud <= cnt) {
                            /* added by user request */
                            /* srcfiles is indexed starting at 0, but
                               DW_AT_decl_file defines that 0 means no
                               file, so tempud 1 means the 0th entry in
                               srcfiles, thus tempud-1 is the correct
                               index into srcfiles.  */
                            char *fname = srcfiles[tempud - 1];

                         // esb_append(esbp, " ");
                         // esb_append(esbp, fname);
                            *esbp += " ";
                            *esbp += fname;
                       }

                       bool check_decl_file = false;

                       if(check_decl_file) {
                           decl_file_result.checks++;
                           /* Zero is always a legal index, it means
                              no source name provided. */
                        // if(tempud > cnt)
                           if( (int)tempud > cnt) {
                               decl_file_result.errors++;
                               DWARF_CHECK_ERROR2(get_AT_name(dbg,attr).c_str(),"does not point to valid file info");
                           }
                       }
                    }
                } else {
                    print_error(dbg, "Cannot get encoding attribute ..",wres, rose_dwarf_error);
                }
                break;
               }

            case DW_AT_const_value:
                wres = formxdata_print_value(attrib,esbp, &rose_dwarf_error);
                if(wres == DW_DLV_OK){
                    /* String appended already. */
                } else if (wres == DW_DLV_NO_ENTRY) {
                    /* nothing? */
                } else {
                   print_error(dbg,"Cannot get DW_AT_const_value ",wres,rose_dwarf_error);
                }
  
                
                break;
            case DW_AT_upper_bound:
            case DW_AT_lower_bound:
            default:
                wres = formxdata_print_value(attrib,esbp, &rose_dwarf_error);
                if (wres == DW_DLV_OK) {
                    /* String appended already. */
                } else if (wres == DW_DLV_NO_ENTRY) {
                    /* nothing? */
                } else {
                    print_error(dbg, "Cannot get formsdata..", wres,rose_dwarf_error);
                }
                break;
            }
        }
        if (cu_name_flag) {
            if (attr == DW_AT_MIPS_fde) {
                if (fde_offset_for_cu_low == DW_DLV_BADOFFSET) {
                    fde_offset_for_cu_low
                        = fde_offset_for_cu_high = tempud;
                } else if (tempud < fde_offset_for_cu_low) {
                    fde_offset_for_cu_low = tempud;
                } else if (tempud > fde_offset_for_cu_high) {
                    fde_offset_for_cu_high = tempud;
                }
            }
        }
        break;
    case DW_FORM_sdata:
        wres = dwarf_formsdata(attrib, &tempsd, &rose_dwarf_error);
        if (wres == DW_DLV_OK) {
            snprintf(small_buf, sizeof(small_buf), "%lld", tempsd);
         // esb_append(esbp, small_buf);
            *esbp += small_buf;
        } else if (wres == DW_DLV_NO_ENTRY) {
            /* nothing? */
        } else {
            print_error(dbg, "Cannot get formsdata..", wres, rose_dwarf_error);
        }
        break;
    case DW_FORM_udata:
        wres = dwarf_formudata(attrib, &tempud, &rose_dwarf_error);
        if (wres == DW_DLV_OK) {
            snprintf(small_buf, sizeof(small_buf), "%llu", tempud);
         // esb_append(esbp, small_buf);
            *esbp += small_buf;
        } else if (wres == DW_DLV_NO_ENTRY) {
            /* nothing? */
        } else {
            print_error(dbg, "Cannot get formudata....", wres, rose_dwarf_error);
        }
        break;
    case DW_FORM_string:
    case DW_FORM_strp:
        wres = dwarf_formstring(attrib, &temps, &rose_dwarf_error);
        if (wres == DW_DLV_OK) {
         // esb_append(esbp, temps);
            *esbp += temps;
        } else if (wres == DW_DLV_NO_ENTRY) {
            /* nothing? */
        } else {
            print_error(dbg, "Cannot get a formstr (or a formstrp)....",wres, rose_dwarf_error);
        }

        break;
    case DW_FORM_flag:
        wres = dwarf_formflag(attrib, &tempbool, &rose_dwarf_error);
        if (wres == DW_DLV_OK) {
            if (tempbool) {
                snprintf(small_buf, sizeof(small_buf), "yes(%d)",tempbool);
             // esb_append(esbp, small_buf);
                *esbp += small_buf;
            } else {
                snprintf(small_buf, sizeof(small_buf), "no");
             // esb_append(esbp, small_buf);
                *esbp += small_buf;
            }
        } else if (wres == DW_DLV_NO_ENTRY) {
            /* nothing? */
        } else {
            print_error(dbg, "Cannot get formflag/p....", wres, rose_dwarf_error);
        }
        break;
    case DW_FORM_indirect:
        /* We should not ever get here, since the true form was
           determined and direct_form has the DW_FORM_indirect if it is
           used here in this attr. */
     // esb_append(esbp, get_FORM_name(dbg, theform));

       printf ("Error: If we should never get here then make this an error! \n");
       ROSE_ASSERT(false);

     // *esbp += get_FORM_name(dbg, theform);
        break;
    default:
        print_error(dbg, "dwarf_whatform unexpected value", DW_DLV_OK,rose_dwarf_error);
    }
    if (verbose && direct_form && direct_form == DW_FORM_indirect)
    {
     // char *form_indir = " (used DW_FORM_indirect) ";
        char *form_indir = strdup(" (used DW_FORM_indirect) ");

     // esb_append(esbp, form_indir);
        *esbp += form_indir;
    }
}

void
print_attribute(Dwarf_Debug dbg, Dwarf_Die die, Dwarf_Half attr, Dwarf_Attribute attr_in,bool print_information,char **srcfiles, Dwarf_Signed cnt, SgAsmDwarfConstruct* asmDwarfConstruct)
   {
     Dwarf_Attribute attrib = 0;
     Dwarf_Unsigned uval = 0;
     string atname;
     string valname;
     int tres = 0;
     Dwarf_Half tag = 0;

  // Added C++ string support to replace C style string support
     string esb_base;

     atname = get_AT_name(dbg, attr);

  // printf ("In print_attribute(): attribute type (atname) = %s asmDwarfConstruct = %s \n",atname.c_str(),asmDwarfConstruct->class_name().c_str());

  // Set the name in the SgAsmDwarfConstruct (base class)
  // asmDwarfConstruct->set_name(atname);

  /* the following gets the real attribute, even in the face of an 
     incorrect doubling, or worse, of attributes */
     attrib = attr_in;

  /* do not get attr via dwarf_attr: if there are (erroneously) 
     multiple of an attr in a DIE, dwarf_attr will not get the
     second, erroneous one and dwarfdump will print the first one
     multiple times. Oops. */

     tres = dwarf_tag(die, &tag, &rose_dwarf_error);
     if (tres == DW_DLV_ERROR)
        {
          tag = 0;
        }
       else
        {
          if (tres == DW_DLV_NO_ENTRY)
             {
               tag = 0;
             }
            else
             {
            /* ok */
             }
        }

#if 0
  // This is just Dwarf error checking.

  // DQ: added bool value directly
     bool check_attr_tag = true;

     if (check_attr_tag)
        {
          string tagname = "<tag invalid>";

          attr_tag_result.checks++;
          if (tres == DW_DLV_ERROR)
             {
               attr_tag_result.errors++;
               DWARF_CHECK_ERROR3(tagname.c_str(),get_AT_name(dbg, attr).c_str(),"check the tag-attr combination.");
             }
            else
             {
               if (tres == DW_DLV_NO_ENTRY)
                  {
                    attr_tag_result.errors++;
                    DWARF_CHECK_ERROR3(tagname.c_str(),get_AT_name(dbg, attr).c_str(),"check the tag-attr combination..")
                  }
                 else 
                  {
                    if (tag_attr_combination(tag, attr))
                       {
                      /* OK */
                       }
                      else
                       {
                         attr_tag_result.errors++;
                         tagname = get_TAG_name(dbg, tag);
                         DWARF_CHECK_ERROR3(tagname.c_str(),get_AT_name(dbg, attr).c_str(),"check the tag-attr combination")
                       }
                  }
             }
        }
#endif

     switch (attr)
        {
          case DW_AT_language:
             {
            // get_small_encoding_integer_and_name(dbg, attrib, &uval,"DW_AT_language", &valname,get_LANG_name, &rose_dwarf_error);
               get_small_encoding_integer_and_name(dbg, attrib, &uval,strdup("DW_AT_language"), &valname,get_LANG_name, &rose_dwarf_error);

            // The language is only set in the SgAsmDwarfCompilationUnit IR node.
               SgAsmDwarfCompilationUnit* asmDwarfCompilationUnit = isSgAsmDwarfCompilationUnit(asmDwarfConstruct);
               ROSE_ASSERT(asmDwarfCompilationUnit != NULL);
            // printf ("Setting language string in SgAsmDwarfCompilationUnit valname = %s \n",valname.c_str());
               asmDwarfCompilationUnit->set_language(valname);
               break;
             }

#if 0
       // DQ: Initially, let's limit the number of dependent functions required to make some progress and generating IR nodes from Dwarf.

          case DW_AT_accessibility:
               get_small_encoding_integer_and_name(dbg, attrib, &uval,"DW_AT_accessibility",&valname, get_ACCESS_name,&rose_dwarf_error);
               break;
          case DW_AT_visibility:
               get_small_encoding_integer_and_name(dbg, attrib, &uval,"DW_AT_visibility",&valname, get_VIS_name,&rose_dwarf_error);
               break;
          case DW_AT_virtuality:
               get_small_encoding_integer_and_name(dbg, attrib, &uval,"DW_AT_virtuality",&valname,get_VIRTUALITY_name, &rose_dwarf_error);
               break;
          case DW_AT_identifier_case:
               get_small_encoding_integer_and_name(dbg, attrib, &uval,"DW_AT_identifier",&valname, get_ID_name,&rose_dwarf_error);
               break;
          case DW_AT_inline:
               get_small_encoding_integer_and_name(dbg, attrib, &uval,"DW_AT_inline", &valname,get_INL_name, &rose_dwarf_error);
               break;
          case DW_AT_encoding:
               get_small_encoding_integer_and_name(dbg, attrib, &uval,"DW_AT_encoding", &valname,get_ATE_name, &rose_dwarf_error);
               break;
          case DW_AT_ordering:
               get_small_encoding_integer_and_name(dbg, attrib, &uval,"DW_AT_ordering", &valname,get_ORD_name, &rose_dwarf_error);
               break;
          case DW_AT_calling_convention:
               get_small_encoding_integer_and_name(dbg, attrib, &uval,"DW_AT_calling_convention",&valname, get_CC_name,&rose_dwarf_error);
               break;
          case DW_AT_discr_list:      /* DWARF3 */
               get_small_encoding_integer_and_name(dbg, attrib, &uval,"DW_AT_discr_list",&valname, get_DSC_name,&rose_dwarf_error);
               break;

          case DW_AT_location:
          case DW_AT_data_member_location:
          case DW_AT_vtable_elem_location:
          case DW_AT_string_length:
          case DW_AT_return_addr:
          case DW_AT_use_location:
          case DW_AT_static_link:
          case DW_AT_frame_base:
        /* value is a location description or location list */

            // valname = "Need to use C++ class!!!";
            // printf ("DW_AT_frame_base: This should be replaced by the C++ string object! \n");
            // ROSE_ASSERT(false);

            // esb_empty_string(&esb_base);
            // get_location_list(dbg, die, attrib, &esb_base);
            // valname = esb_get_string(&esb_base);

               get_location_list(dbg, die, attrib, &esb_base);
               valname = esb_base;
               break;

          case DW_AT_SUN_func_offsets:

            // printf ("DW_AT_SUN_func_offsets: This should be replaced by the C++ string object! \n");
            // get_FLAG_BLOCK_string(dbg, attrib);
            // valname = esb_get_string(&esb_base);

               get_FLAG_BLOCK_string(dbg, attrib, esb_base);
               valname = esb_base;
               break;

          case DW_AT_SUN_cf_kind:
             {
               Dwarf_Half kind;
               Dwarf_Unsigned tempud;
               Dwarf_Error err;
               int wres;
               wres = dwarf_formudata (attrib,&tempud, &err);
               if(wres == DW_DLV_OK) {
                   kind = tempud;
                   valname = get_ATCF_name(dbg, kind);
               } else if (wres == DW_DLV_NO_ENTRY) {
                   valname = "?";
               } else {
                   print_error(dbg,"Cannot get formudata....",wres,err);
                   valname = "??";
               }

               break;
             }

          case DW_AT_upper_bound:
             {
               Dwarf_Half theform;
               int rv;
               rv = dwarf_whatform(attrib,&theform,&rose_dwarf_error);
            /* depending on the form and the attribute, process the form */
               if(rv == DW_DLV_ERROR) {
                    print_error(dbg, "dwarf_whatform cannot find attr form",rv, rose_dwarf_error);
               } else if (rv == DW_DLV_NO_ENTRY) {
                   break;
               }

               switch (theform)
                  {
                    case DW_FORM_block1:
                      // valname = "Need to use C++ class!!!";
                      // printf ("This should be replaced by the C++ string object! \n");
                      // ROSE_ASSERT(false);

                      // get_location_list(dbg, die, attrib, &esb_base);
                      // valname = esb_get_string(&esb_base);

                         get_location_list(dbg, die, attrib, &esb_base);
                         valname = esb_base;
                         break;

                     default:
                      // valname = "Need to use C++ class!!!";
                      // printf ("This should be replaced by the C++ string object! \n");
                      // ROSE_ASSERT(false);

                      // esb_empty_string(&esb_base);
                      // get_attr_value(dbg, tag, attrib, srcfiles, cnt, &esb_base);
                      // valname = esb_get_string(&esb_base);

                         get_attr_value(dbg, tag, attrib, srcfiles, cnt, &esb_base);
                         valname = esb_base;
                         break;
                  }
               break;
             }

          case DW_AT_high_pc:
             {
               Dwarf_Half theform;
               int rv;
               rv = dwarf_whatform(attrib,&theform,&rose_dwarf_error);
            /* depending on the form and the attribute, process the form */
               if(rv == DW_DLV_ERROR)
                  {
                    print_error(dbg, "dwarf_whatform cannot find attr form",rv, rose_dwarf_error);
                  }
                 else
                  {
                    if (rv == DW_DLV_NO_ENTRY)
                       {
                         break;
                       }
                  }

            // esb_empty_string(&esb_base);
               get_attr_value(dbg, tag, attrib, srcfiles, cnt, &esb_base);
               if( theform != DW_FORM_addr)
                  {
                 /* New in DWARF4: other forms are not an address
                    but are instead offset from pc.
                    One could test for DWARF4 here before adding
                    this string, but that seems unnecessary as this
                    could not happen with DWARF3 or earlier. 
                    A normal consumer would have to add this value to
                    DW_AT_low_pc to get a true pc. */
                 // esb_append(&esb_base,"<offset-from-lowpc>");
                    esb_base += "<offset-from-lowpc>";
                  }

            // valname = esb_get_string(&esb_base);
               valname = esb_base;
             }
#endif
          default:
            // valname = "Need to use C++ class!!!";
            // printf ("This should be replaced by the C++ string object! \n");
            // ROSE_ASSERT(false);
               esb_base = "";
               get_attr_value(dbg, tag, attrib, srcfiles, cnt, &esb_base);
               valname = esb_base;
            // esb_empty_string(&esb_base);
            // get_attr_value(dbg, tag, attrib, srcfiles, cnt, &esb_base);
            // valname = esb_get_string(&esb_base);
               break;
        }

  // printf ("In print_attribute: setting name for asmDwarfConstruct = %s using valname = %s \n",asmDwarfConstruct->class_name().c_str(),valname.c_str());

  // Set the name in the base class to valname
  // if ( isSgAsmDwarfSubprogram(asmDwarfConstruct) || isSgAsmDwarfvariable(asmDwarfConstruct) )
  //      asmDwarfConstruct->set_name(valname);

  // Set the name in the base clas only when the input attribute is DW_AT_name.
     if (attr == DW_AT_name)
        {
          asmDwarfConstruct->set_name(valname);
        }

#if 0
  // Supress Dwarf output
     if (print_information)
        {
          printf("\t\t%-28s%s\n", atname.c_str(), valname.c_str());
        }
#endif
   }
#endif


/* print info about die */
// void
SgAsmDwarfConstruct* 
build_dwarf_IR_node_from_print_one_die(Dwarf_Debug dbg, Dwarf_Die die, bool print_information, char **srcfiles, Dwarf_Signed cnt /* , SgAsmDwarfCompilationUnit* asmDwarfCompilationUnit */)
   {
     Dwarf_Signed i;
     Dwarf_Off offset, overall_offset;
     string tagname;
     Dwarf_Half tag;
     Dwarf_Signed atcnt;
     Dwarf_Attribute *atlist;
     int tres;
     int ores;
     int atres;

     tres = dwarf_tag(die, &tag, &rose_dwarf_error);
     if (tres != DW_DLV_OK)
        {
          print_error(dbg, "accessing tag of die!", tres, rose_dwarf_error);
        }

     tagname = get_TAG_name(dbg, tag);
  // printf ("In build_dwarf_IR_node_from_print_one_die(): tag = %d tagname = %s \n",tag,tagname.c_str());

     ores = dwarf_dieoffset(die, &overall_offset, &rose_dwarf_error);
     if (ores != DW_DLV_OK)
        {
          print_error(dbg, "dwarf_dieoffset", ores, rose_dwarf_error);
        }

     ores = dwarf_die_CU_offset(die, &offset, &rose_dwarf_error);
     if (ores != DW_DLV_OK)
        {
          print_error(dbg, "dwarf_die_CU_offset", ores, rose_dwarf_error);
        }

  // if (!dst_format && print_information)
     if (print_information)
        {
          if (indent_level == 0)
            {
           // printf("\nCOMPILE_UNIT<header overall offset = %llu>:\n",overall_offset - offset);

           // Initialize the information in the SgAsmDwarfCompilationUnit IR node

            }
           else
            {
           // if (local_symbols_already_began == false && indent_level == 1 && !dense)
              if (local_symbols_already_began == false && indent_level == 1)
                 {
                // printf("\nLOCAL_SYMBOLS:\n");
                   local_symbols_already_began = true;
                 }
            }

      // printf("<%d><%5llu GOFF=%llu>\t%s\n", indent_level, offset,overall_offset, tagname.c_str());
       }

     atres = dwarf_attrlist(die, &atlist, &atcnt, &rose_dwarf_error);
     if (atres == DW_DLV_ERROR)
        {
          print_error(dbg, "dwarf_attrlist", atres, rose_dwarf_error);
        }
       else
        {
          if (atres == DW_DLV_NO_ENTRY)
             {
            /* indicates there are no attrs.  It is not an error. */
            // printf ("No attributes found (not an error) \n");
               atcnt = 0;
             }
        }

     if (indent_level == 0)
        {
       // This is the CU header
        }
       else
        {
       // These are local symbols
        }

  // Build the ROSE dwarf construct IR node (use a factory design to build the appropriate IR nodes
#if 0
  // This is used to limit the Dwarf representation to just the SgAsmDwarfCompilationUnit IR nodes
  // (the test file from BGL has 1200+ CU's so this is useful for debugging).
     SgAsmDwarfConstruct* asmDwarfConstruct = NULL;
     if (tag == DW_TAG_compile_unit)
        {
          asmDwarfConstruct = SgAsmDwarfConstruct::createDwarfConstruct( tag, indent_level, offset, overall_offset );
        }
#else
  // This will cause all Dwarf IR nodes to be properly represented in the AST.
     SgAsmDwarfConstruct* asmDwarfConstruct = SgAsmDwarfConstruct::createDwarfConstruct( tag, indent_level, offset, overall_offset );
#endif
  // printf ("Handle attributes: atcnt = %d \n",(int)atcnt);

  // Setup attribute specific fields in the different kinds of Dwarf nodes
     for (i = 0; i < atcnt; i++)
        {
          Dwarf_Half attr;
          int ares;

          ares = dwarf_whatattr(atlist[i], &attr, &rose_dwarf_error);
          if (ares == DW_DLV_OK)
             {
               if (asmDwarfConstruct != NULL)
                  {
                    print_attribute(dbg, die, attr,atlist[i],print_information, srcfiles, cnt, asmDwarfConstruct);
                  }
                 else
                  {
                 // printf ("Skipping print_attribute since asmDwarfConstruct == NULL \n");
                  }

               if (indent_level == 0)
                  {
                 // This is the CU header
                  }
                 else
                  {
                 // These are local symbols
                  }
             }
            else
             {
               print_error(dbg, "dwarf_whatattr entry missing", ares, rose_dwarf_error);
             }
        }

     for (i = 0; i < atcnt; i++)
        {
          dwarf_dealloc(dbg, atlist[i], DW_DLA_ATTR);
        }

     if (atres == DW_DLV_OK)
        {
          dwarf_dealloc(dbg, atlist, DW_DLA_LIST);
        }

#if 0
  // if (dense && print_information)
     if (false && print_information)
        {
          printf("\n\n");
        }
#endif

     return asmDwarfConstruct;
   }



/* recursively follow the die tree */
// void build_dwarf_IR_node_from_die_and_children(Dwarf_Debug dbg, Dwarf_Die in_die_in,char **srcfiles, Dwarf_Signed cnt, SgAsmDwarfCompilationUnit* asmDwarfCompilationUnit)
// void build_dwarf_IR_node_from_die_and_children(Dwarf_Debug dbg, Dwarf_Die in_die_in,char **srcfiles, Dwarf_Signed cnt, SgAsmDwarfConstruct* parentDwarfConstruct )
SgAsmDwarfConstruct* 
build_dwarf_IR_node_from_die_and_children(Dwarf_Debug dbg, Dwarf_Die in_die_in,char **srcfiles, Dwarf_Signed cnt, SgAsmDwarfConstruct* parentDwarfConstruct )
   {
  // NOTE: The first time this function is called the parentDwarfConstruct is NULL.

     Dwarf_Die child;
     Dwarf_Die sibling;
     Dwarf_Error err;
  // int tres;
     int cdres;
     Dwarf_Die in_die = in_die_in;

     SgAsmDwarfConstruct* astDwarfConstruct = NULL;

  // printf ("Inside of build_dwarf_IR_node_from_die_and_children() \n");

     for (;;)
        {
       // printf ("Top of loop in build_dwarf_IR_node_from_die_and_children() \n");

          PUSH_DIE_STACK(in_die);

#if 0
       // DQ: This just does error checking on dwarf format

          if (check_tag_tree)
             {
               tag_tree_result.checks++;
               if (indent_level == 0)
                  {
                    Dwarf_Half tag;

                    tres = dwarf_tag(in_die, &tag, &err);
                    if (tres != DW_DLV_OK)
                       {
                         tag_tree_result.errors++;
                         DWARF_CHECK_ERROR("Tag-tree root is not DW_TAG_compile_unit")
                       }
                      else
                         if (tag == DW_TAG_compile_unit)
                            {
                              /* OK */
                            }
                           else
                            {
                              tag_tree_result.errors++;
                              DWARF_CHECK_ERROR("tag-tree root is not DW_TAG_compile_unit")
                            }
                  }
                 else
                  {
                    Dwarf_Half tag_parent, tag_child;
                    int pres;
                    int cres;
                    string ctagname = "<child tag invalid>";
                    string ptagname = "<parent tag invalid>";

                    pres = dwarf_tag(die_stack[indent_level - 1], &tag_parent,&err);
                    cres = dwarf_tag(in_die, &tag_child, &err);

                    if (pres != DW_DLV_OK)
                         tag_parent = 0;

                    if (cres != DW_DLV_OK)
                         tag_child = 0;

                    if (cres != DW_DLV_OK || pres != DW_DLV_OK) 
                       {
                         if (cres == DW_DLV_OK)
                            {
                              ctagname = get_TAG_name(dbg, tag_child);
                            }

                         if (pres == DW_DLV_OK)
                            {
                              ptagname = get_TAG_name(dbg, tag_parent);
                            }

                         DWARF_CHECK_ERROR3(ptagname.c_str(),ctagname.c_str(),"Tag-tree relation is not standard..");
                      }
                     else
                      {
#if 0
                     // DQ (11/7/2008): This is error checking and I want to remove the dependence upon dwarfdump code.
                        if (tag_tree_combination(tag_parent, tag_child))
                           {
                          /* OK */
                           }
                          else
                           {
                             DWARF_CHECK_ERROR3(get_TAG_name(dbg, tag_parent).c_str(),get_TAG_name(dbg, tag_child).c_str(),"tag-tree relation is not standard.");
                           }
#endif
                      }
                  }
             }
#endif

#if 1
       // Suppress output!

       // DQ (11/4/2008): This is the location were we will have to generate IR nodes using each debug info entry (die)
       // printf ("Calling print_one_die to output the information about each specific debug info entry (die) \n");

       /* here to pre-descent processing of the die */
       // SgAsmDwarfConstruct* astDwarfConstruct = build_dwarf_IR_node_from_print_one_die(dbg, in_die, /* info_flag */ true, srcfiles, cnt /* , asmDwarfCompilationUnit */ );
          astDwarfConstruct = build_dwarf_IR_node_from_print_one_die(dbg, in_die, /* info_flag */ true, srcfiles, cnt /* , asmDwarfCompilationUnit */ );

       // parentDwarfConstruct->set_child(astDwarfConstruct);
          if (parentDwarfConstruct != NULL && astDwarfConstruct != NULL)
             {
            // printf ("Push children onto parent IR node! \n");

            // if (parentDwarfConstruct->get_children() == NULL)
            //      parentDwarfConstruct->set_children(new SgAsmDwarfConstructList());
#if 1
            // When this work we know that we have the child support for Dwarf IR nodes in place (in all the right places).
               ROSE_ASSERT(parentDwarfConstruct->get_children() != NULL);
               parentDwarfConstruct->get_children()->get_list().push_back(astDwarfConstruct);
#else
            // Optional support for parents without child support implemented yet!
            // I want to know what IR nodes require it and how many there are.
               if (parentDwarfConstruct->get_children() != NULL)
                  {
                    parentDwarfConstruct->get_children()->get_list().push_back(astDwarfConstruct);
                  }
                 else
                  {
                    printf ("Error: Sorry not implemented (child support on IR nodes %s \n",parentDwarfConstruct->class_name().c_str());
                  }
#endif
               astDwarfConstruct->set_parent(parentDwarfConstruct);
             }

       // printf ("Process children \n");
#endif
          cdres = dwarf_child(in_die, &child, &err);
       /* child first: we are doing depth-first walk */
          if (cdres == DW_DLV_OK)
             {
               indent_level++;
               if(indent_level >= DIE_STACK_SIZE )
                  {
                    print_error(dbg,"compiled in DIE_STACK_SIZE limit exceeded",DW_DLV_OK,err);
                  }

            // printf ("Processing child dwarf nodes: calling build_dwarf_IR_node_from_die_and_children() \n");

            // Old function: print_die_and_children(dbg, child, srcfiles, cnt);

            // Ignore the return result (children are added to the parent).
#if 0
               build_dwarf_IR_node_from_die_and_children(dbg, child, srcfiles, cnt, astDwarfConstruct);
#else
               if (astDwarfConstruct != NULL)
                    build_dwarf_IR_node_from_die_and_children(dbg, child, srcfiles, cnt, astDwarfConstruct);
#endif
               indent_level--;

               if (indent_level == 0)
                    local_symbols_already_began = false;

               dwarf_dealloc(dbg, child, DW_DLA_DIE);
             }
            else
             {
               if (cdres == DW_DLV_ERROR)
                  {
                    print_error(dbg, "dwarf_child", cdres, err);
                  }
             }

       // printf ("Process siblings \n");
          cdres = dwarf_siblingof(dbg, in_die, &sibling, &err);
          if (cdres == DW_DLV_OK)
             {
            /* print_die_and_children(dbg, sibling, srcfiles, cnt); We
               loop around to actually print this, rather than
               recursing. Recursing is horribly wasteful of stack
               space. */
             }
            else 
             {
               if (cdres == DW_DLV_ERROR)
                  {
                    print_error(dbg, "dwarf_siblingof", cdres, err);
                  }
             }

       /* Here do any post-descent (ie post-dwarf_child) processing of the in_die. */

       // printf ("Process post-dwarf_child \n");

          POP_DIE_STACK;
          if (in_die != in_die_in)
             {
            /* Dealloc our in_die, but not the argument die, it belongs 
               to our caller. Whether the siblingof call worked or not. 
             */
               dwarf_dealloc(dbg, in_die, DW_DLA_DIE);
             }

          if (cdres == DW_DLV_OK)
             {
            /* Set to process the sibling, loop again. */
               in_die = sibling;
             }
            else
             {
            /* We are done, no more siblings at this level. */
               break;
             }

       // printf ("Bottom of loop in build_dwarf_IR_node_from_die_and_children() \n");
        }  /* end for loop on siblings */

  // ROSE_ASSERT(astDwarfConstruct != NULL);

     return astDwarfConstruct;
   }


void
build_dwarf_line_numbers_this_cu(Dwarf_Debug dbg, Dwarf_Die cu_die, SgAsmDwarfCompilationUnit* asmDwarfCompilationUnit)
   {
  // This function build the IR nodes in the AST that hold the mappings of instruction addresses 
  // to source line and column numbers.  From this we generate STL maps and higher level interfaces 
  // that make the information easier to use.

     Dwarf_Signed linecount = 0;
     Dwarf_Line *linebuf = NULL;
     Dwarf_Signed i = 0;
     Dwarf_Addr pc = 0;
     Dwarf_Unsigned lineno = 0;
     Dwarf_Signed column = 0;
     char* filename;

     int lres = 0;
     int sres = 0;
     int ares = 0;
     int lires = 0;
     int cores = 0;

  // printf ("Inside of build_dwarf_line_numbers_this_cu() (using .debug_line section) \n");

  // printf("\n.debug_line: line number info for a single cu\n");

  // printf ("Setting verbose > 1 (verbose==2) \n");
  // verbose = 2;

     ROSE_ASSERT(asmDwarfCompilationUnit->get_line_info() == NULL);

     SgAsmDwarfLineList* asmDwarfLineList = new SgAsmDwarfLineList();

     asmDwarfCompilationUnit->set_line_info(asmDwarfLineList);
     asmDwarfLineList->set_parent(asmDwarfCompilationUnit);

     ROSE_ASSERT(asmDwarfCompilationUnit->get_line_info() != NULL);

#if 0
     if (verbose > 1)
        {
          print_source_intro(cu_die);
          build_dwarf_IR_node_from_print_one_die(dbg, cu_die, /* print_information= */ 1,/* srcfiles= */ 0, /* cnt= */ 0 /* , asmDwarfCompilationUnit */ );

          lres = dwarf_print_lines(cu_die, &rose_dwarf_error);
          if (lres == DW_DLV_ERROR)
             {
               print_error(dbg, "dwarf_srclines details", lres, rose_dwarf_error);
             }

          printf ("Exiting print_line_numbers_this_cu prematurely! \n");
          return;
       }
#endif

     lres = dwarf_srclines(cu_die, &linebuf, &linecount, &rose_dwarf_error);
     if (lres == DW_DLV_ERROR)
        {
          print_error(dbg, "dwarf_srclines", lres, rose_dwarf_error);
        }
       else
        {
          if (lres == DW_DLV_NO_ENTRY)
             {
            /* no line information is included */
             }
            else
             {
#if 0
            // Suppress output!
               print_source_intro(cu_die);
               if (verbose)
                  {
                    build_dwarf_IR_node_from_print_one_die(dbg, cu_die, /* print_information= */ 1,/* srcfiles= */ 0, /* cnt= */ 0 /* , asmDwarfCompilationUnit */ );
                  }
#endif
            // Output a header for the data
            // printf("<source>\t[row,column]\t<pc>\t//<new statement or basic block\n");

               for (i = 0; i < linecount; i++)
                  {
                    Dwarf_Line line = linebuf[i];

                    sres = dwarf_linesrc(line, &filename, &rose_dwarf_error);
                    ares = dwarf_lineaddr(line, &pc, &rose_dwarf_error);

                    if (sres == DW_DLV_ERROR)
                       {
                         print_error(dbg, "dwarf_linesrc", sres, rose_dwarf_error);
                       }

                    if (sres == DW_DLV_NO_ENTRY)
                       {
                         filename = strdup("<unknown>");
                       }

                    if (ares == DW_DLV_ERROR)
                       {
                         print_error(dbg, "dwarf_lineaddr", ares, rose_dwarf_error);
                       }

                    if (ares == DW_DLV_NO_ENTRY)
                       {
                         pc = 0;
                       }

                    lires = dwarf_lineno(line, &lineno, &rose_dwarf_error);
                    if (lires == DW_DLV_ERROR)
                       {
                         print_error(dbg, "dwarf_lineno", lires, rose_dwarf_error);
                       }

                    if (lires == DW_DLV_NO_ENTRY)
                       {
                         lineno = -1LL;
                       }

                    cores = dwarf_lineoff(line, &column, &rose_dwarf_error);
                    if (cores == DW_DLV_ERROR)
                       {
                         print_error(dbg, "dwarf_lineoff", cores, rose_dwarf_error);
                       }

                    if (cores == DW_DLV_NO_ENTRY)
                       {
                         column = -1LL;
                       }

                 // printf("%s:\t[%3llu,%2lld]\t%#llx", filename, lineno,column, pc);

                 // Build an IR node to represent the instruction address for each line.
                 // This uses the static maps in the Sg_File_Info to support a table similar 
                 // to Dwarf's and which maps filenames to integers and back.  This avoids
                 // building and deleting a Sg_File_Info object.
                    int filename_id = Sg_File_Info::addFilenameToMap(filename);

                 // Now build the IR node to store the raw information from dwarf.
                    SgAsmDwarfLine* lineInfo = new SgAsmDwarfLine(pc, filename_id, lineno, column);

                    asmDwarfLineList->get_line_list().push_back(lineInfo);

                    if (sres == DW_DLV_OK)
                         dwarf_dealloc(dbg, filename, DW_DLA_STRING);

#if 0
                 // DQ: The remainer of this code block is the output of information that sumarizes the previous entry.

                    Dwarf_Bool newstatement = 0;
                    Dwarf_Bool lineendsequence = 0;
                    Dwarf_Bool new_basic_block = 0;
                    int nsres;

                    nsres = dwarf_linebeginstatement(line, &newstatement, &rose_dwarf_error);
                    if (nsres == DW_DLV_OK)
                       {
#if 0
                      // Suppress output!
                         if (newstatement)
                            {
                              printf("\t// new statement");
                            }
#endif
                       }
                      else
                         if (nsres == DW_DLV_ERROR)
                            {
                              print_error(dbg, "linebeginstatment failed", nsres,rose_dwarf_error);
                            }

                    nsres = dwarf_lineblock(line, &new_basic_block, &rose_dwarf_error);
                    if (nsres == DW_DLV_OK)
                       {
#if 0
                      // Suppress output!
                         if (new_basic_block)
                            {
                              printf("\t// new basic block");
                            }
#endif
                       }
                      else
                         if (nsres == DW_DLV_ERROR)
                            {
                              print_error(dbg, "lineblock failed", nsres, rose_dwarf_error);
                            }

                    nsres = dwarf_lineendsequence(line, &lineendsequence, &rose_dwarf_error);
                    if (nsres == DW_DLV_OK)
                       {
#if 0
                      // Suppress output!
                         if (lineendsequence)
                            {
                              printf("\t// end of text sequence");
                            }
#endif
                       }
                      else
                         if (nsres == DW_DLV_ERROR)
                            {
                              print_error(dbg, "lineblock failed", nsres, rose_dwarf_error);
                            }
#endif

#if 0
                 // Suppress output!
                    printf("\n");
#endif
                  }

               dwarf_srclines_dealloc(dbg, linebuf, linecount);
             }
        }

  // printf ("Now generate the static maps to use to lookup the instruction address to source position mappings \n");
     asmDwarfLineList->buildInstructionAddressSourcePositionMaps(asmDwarfCompilationUnit);
  // printf ("DONE: Now generate the maps to use to lookup the instruction address to source position mappings \n");

#if 0
  // Run a second time to test that the maps are not regenerated (this is just a test)
     printf ("Run a second time to test that the maps are not regenerated ... \n");
     DwarfInstructionSourceMapReturnType returnValue = asmDwarfLineList->buildInstructionAddressSourcePositionMaps();
     printf ("DONE: Run a second time to test that the maps are not regenerated ... \n");
#endif

#if 1
  // Output the line information from the generated maps (debugging information).
  // printf ("SgProject::get_verbose() = %d \n",SgProject::get_verbose());
     if (SgProject::get_verbose() > 0)
        {
          asmDwarfLineList->display("Inside of build_dwarf_line_numbers_this_cu()");
        }
#endif
   }


/* process each compilation unit in .debug_info */
void
build_dwarf_IR_nodes(Dwarf_Debug dbg, SgAsmInterpretation* asmInterpretation)
   {
     Dwarf_Unsigned cu_header_length = 0;
     Dwarf_Unsigned abbrev_offset = 0;
     Dwarf_Half version_stamp = 0;
     Dwarf_Half address_size = 0;
     Dwarf_Die cu_die = NULL;
     Dwarf_Unsigned next_cu_offset = 0;
     int nres = DW_DLV_OK;
     int   cu_count = 0;

  // printf("\n.debug_info\n");

     ROSE_ASSERT(asmInterpretation->get_dwarf_info() == NULL);

#if 0
     SgAsmDwarfCompilationUnit* asmDwarfCompilationUnit = new SgAsmDwarfCompilationUnit();

     asmInterpretation->set_dwarf_info(asmDwarfCompilationUnit);
     asmDwarfCompilationUnit->set_parent(asmInterpretation);

     ROSE_ASSERT(asmInterpretation->get_dwarf_info() != NULL);
#endif

#if 1
     SgAsmDwarfCompilationUnitList* asmDwarfCompilationUnitList = new SgAsmDwarfCompilationUnitList();

     asmInterpretation->set_dwarf_info(asmDwarfCompilationUnitList);
     asmDwarfCompilationUnitList->set_parent(asmInterpretation);

     ROSE_ASSERT(asmInterpretation->get_dwarf_info() != NULL);
#endif

  // This permits restricting number of CU's read so that we can have a 
  // manageable problem to debug the Dwarf represnetation in ROSE.
     int compilationUnitCounter = 0;
  // int compilationUnitLimit   = 10;
     int compilationUnitLimit   = 1000000;

  /* Loop until it fails.  */
     while ( ((nres = dwarf_next_cu_header(dbg, &cu_header_length, &version_stamp,&abbrev_offset, &address_size,&next_cu_offset, &rose_dwarf_error)) == DW_DLV_OK) && (compilationUnitCounter < compilationUnitLimit) )
        {
#if 0
       // printf ("In loop over the headers: nres = %d \n",nres);
       // printf ("next_cu_offset   = %lu \n",(unsigned long) next_cu_offset);
          printf ("cu_header_length = %lu \n",(unsigned long) cu_header_length);
#endif

       // printf ("Processing CU: %d \n",compilationUnitCounter);
          compilationUnitCounter++;

#if 0
       // This permits restricting size of the CU's read so that we can have a 
       // manageable problem to debug the Dwarf represnetation in ROSE.
          if (cu_header_length > 500)
             {
               printf ("Skipping the handling of this CU (too large for debugging): cu_header_length = %lu \n",(unsigned long) cu_header_length);
               continue;
             }
#endif

       // printf ("loop count: cu_count = %d \n",cu_count);

          ROSE_ASSERT(cu_count < break_after_n_units);
#if 0
          if (cu_count >=  break_after_n_units)
             {
               printf("Break at %d\n",cu_count);

               printf ("I think this should be an error for now! \n");
               ROSE_ASSERT(false);

               break;
             }
#endif

#if 0
          printf ("cu_name_flag = %s \n",cu_name_flag ? "true" : "false");
#endif

       // error status variable
          int sres = 0;

       // printf ("Setting cu_name_flag == true \n");
       // cu_name_flag = true;
       // cu_name_flag = false;

          ROSE_ASSERT(cu_name_flag == false);

#if 0
       // DQ: I think that this code is unimportant to the construction or IR nodes!

          if (cu_name_flag)
             {
               int tres = 0;
               Dwarf_Half tag = 0;
               Dwarf_Attribute attrib = 0;
               Dwarf_Half theform = 0;
               int fres = 0;
               int ares = 0;

               sres = dwarf_siblingof(dbg, NULL, &cu_die, &rose_dwarf_error);
               if (sres != DW_DLV_OK)
                  {
                    print_error(dbg, "siblingof cu header", sres, rose_dwarf_error);
                  }

               tres = dwarf_tag(cu_die, &tag, &rose_dwarf_error);
               if (tres != DW_DLV_OK)
                  {
                    print_error(dbg, "tag of cu die", tres, rose_dwarf_error);
                  }

               ares = dwarf_attr(cu_die, DW_AT_name, &attrib, &rose_dwarf_error);
               if (ares != DW_DLV_OK)
                  {
                    print_error(dbg, "dwarf DW_AT_name ", ares, rose_dwarf_error);
                  }

               fres = dwarf_whatform(attrib, &theform, &rose_dwarf_error);
               if (fres != DW_DLV_OK)
                  {
                    print_error(dbg, "dwarf_whatform problem ", fres, rose_dwarf_error);
                  }
                 else
                  {
                    if (theform == DW_FORM_string || theform == DW_FORM_strp) 
                       {
                         char* temps;
                         int strres;
                         char* p;

                         strres = dwarf_formstring(attrib, &temps, &rose_dwarf_error);
                         p = temps;

                         printf ("temps = %s \n",temps);

                         if (strres != DW_DLV_OK)
                            {
                              print_error(dbg,"formstring failed unexpectedly",strres, rose_dwarf_error);
                            }

                         if (cu_name[0] != '/')
                            {
                              p = strrchr(temps, '/');
                              if (p == NULL)
                                 {
                                   p = temps;
                                 }
                                else
                                 {
                                   p++;
                                 }
                            }

                         if (strcmp(cu_name, p))
                            {
                              printf ("Exiting from loop... (continue) \n");
                              continue;
                            }
                       }
                      else
                       {
                         print_error(dbg,"dwarf_whatform unexpected value",fres, rose_dwarf_error);
                       }
                  }

               dwarf_dealloc(dbg, attrib, DW_DLA_ATTR);
               dwarf_dealloc(dbg, cu_die, DW_DLA_DIE);
             }
#endif

#if 0
          printf ("build_dwarf_IR_nodes(): verbose = %s \n",verbose ? "true" : "false");
       // printf ("build_dwarf_IR_nodes(): dense   = %s \n",dense ? "true" : "false");
#endif

#if 0
       // Supress output!
          if (verbose)
             {
               printf("\nCU_HEADER:\n");
               printf("\t\t%-28s%llu\n", "cu_header_length",cu_header_length);
               printf("\t\t%-28s%d\n", "version_stamp", version_stamp);
               printf("\t\t%-28s%llu\n", "abbrev_offset",abbrev_offset);
               printf("\t\t%-28s%d", "address_size", address_size);
             }

          printf ("\nbuild_dwarf_IR_nodes(): Calling dwarf_siblingof() \n");
#endif

       /* process a single compilation unit in .debug_info. */
          sres = dwarf_siblingof(dbg, NULL, &cu_die, &rose_dwarf_error);

       // printf ("build_dwarf_IR_nodes(): status of call to dwarf_siblingof(): sres = %d \n",sres);

          if (sres == DW_DLV_OK)
             {
#if 0
               printf ("Processing sibling information \n");
               printf ("info_flag    = %s \n",info_flag    ? "true" : "false");
               printf ("cu_name_flag = %s \n",cu_name_flag ? "true" : "false");
#endif
            // printf ("Explicitly setting info_flag == true \n");
               info_flag = true;

            // if (info_flag || cu_name_flag)
            //    {
                    Dwarf_Signed cnt = 0;
                    char **srcfiles = 0;

                 // We need to call this function so that we can generate filenames to support the line number mapping below.
                    int srcf = dwarf_srcfiles(cu_die, &srcfiles, &cnt, &rose_dwarf_error);

                    if (srcf != DW_DLV_OK)
                       {
                      // I think we can make this an error.
                         printf ("Error: No source file information found: (dwarf_srcfiles() != DW_DLV_OK) \n");
                         ROSE_ASSERT(false);

                         srcfiles = NULL;
                         cnt = 0;
                       }

                 // This function call will traverse the list of child debug info entries and 
                 // within this function we will generate the IR nodes specific to Dwarf. This
                 // should define an course view of the AST which can be used to relate the binary
                 // to the source code.

                 // printf ("In print_infos(): Calling build_dwarf_IR_node_from_die_and_children() \n");
                 // print_die_and_children(dbg, cu_die, srcfiles, cnt);
                    SgAsmDwarfConstruct* asmDwarfConstruct = build_dwarf_IR_node_from_die_and_children(dbg, cu_die, srcfiles, cnt, NULL);
                    ROSE_ASSERT(asmDwarfConstruct != NULL);

#if 0
                    SgAsmDwarfCompilationUnit* asmDwarfCompilationUnit = isSgAsmDwarfCompilationUnit(asmDwarfConstruct);
                    ROSE_ASSERT(asmDwarfCompilationUnit != NULL);

                    asmInterpretation->set_dwarf_info(asmDwarfCompilationUnit);
                    asmDwarfCompilationUnit->set_parent(asmInterpretation);

                    ROSE_ASSERT(asmInterpretation->get_dwarf_info() != NULL);
#else
                 // Handle the case of many Dwarf Compile Units
                 // asmInterpretation->get_dwarf_info()->get_cu_list().push_back(asmDwarfCompilationUnit);
                    SgAsmDwarfCompilationUnit* asmDwarfCompilationUnit = isSgAsmDwarfCompilationUnit(asmDwarfConstruct);
                    ROSE_ASSERT(asmDwarfCompilationUnit != NULL);

                    asmDwarfCompilationUnitList->get_cu_list().push_back(asmDwarfCompilationUnit);
                    asmDwarfCompilationUnit->set_parent(asmDwarfCompilationUnitList);
#endif

                    if (srcf == DW_DLV_OK)
                       {
                         int si;

                         for (si = 0; si < cnt; ++si)
                            {
                              dwarf_dealloc(dbg, srcfiles[si], DW_DLA_STRING);
                            }
                         dwarf_dealloc(dbg, srcfiles, DW_DLA_LIST);
                       }
            //    }

            // printf ("Explicitly setting line_flag == true \n");
               bool line_flag = true;

               if (line_flag)
                  {
                 // printf ("\n\nOutput the line information by calling print_line_numbers_this_cu() \n");
                 // print_line_numbers_this_cu(dbg, cu_die);
                    build_dwarf_line_numbers_this_cu(dbg, cu_die, asmDwarfCompilationUnit);
                  }
                 else
                  {
                    printf ("Skipping line number <--> instruction address mapping information \n");
                  }

               dwarf_dealloc(dbg, cu_die, DW_DLA_DIE);
             }
            else
             {
               if (sres == DW_DLV_NO_ENTRY)
                  {
                 /* do nothing I guess. */
                  }
                 else
                  {
                    print_error(dbg, "Regetting cu_die", sres, rose_dwarf_error);
                  }
             }

          ++cu_count;
          cu_offset = next_cu_offset;
        }

  // printf ("error checking: nres = %d \n",nres);

     if (nres == DW_DLV_ERROR)
        {
          string errmsg = dwarf_errmsg(rose_dwarf_error);
          Dwarf_Unsigned myerr = dwarf_errno(rose_dwarf_error);

          fprintf(stderr, "%s ERROR:  %s:  %s (%lu)\n",program_name.c_str(), "attempting to print .debug_info",errmsg.c_str(), (unsigned long) myerr);
       // fprintf(stderr, "attempting to continue.\n");

       // printf ("Error: nres = %d \n",nres);
       // ROSE_ASSERT(false);
        }

  // printf ("Exiting print_infos() \n");
   }


// DQ (11/10/2008):
/*! \brief This traversal permits symbols to be commented out from the DOT graphs.

 */
void commentOutSymbolsFromDotGraph (SgNode* node)
   {
  // Define the attribute class containing the virtual function "commentOutNodeInGraph()"
     class SymbolPruningAttribute : public AstAttribute
        {
       // This is a persistant attribute used to mark locations in the AST where we don't want IR nodes to be generated for the DOT graph.

          public:
               bool commentOutNodeInGraph() { return true; }
               virtual AstAttribute* copy() { return new SymbolPruningAttribute(*this); }
               virtual ~SymbolPruningAttribute() {}
        };

  // Define the traversal class over the AST
     class CommentOutSymbolsFromDotGraph : public SgSimpleProcessing
        {
          public:
               void visit ( SgNode* node )
                  {
#if 1
                  // Also remove sections to make the Dwarf graph dominate!
                     SgAsmGenericSectionList* genericSectionList = isSgAsmGenericSectionList(node);
                     if (genericSectionList != NULL)
                        {
                          SymbolPruningAttribute* att = new SymbolPruningAttribute();
                          genericSectionList->addNewAttribute("SymbolPruningAttribute",att);
                        }

                     SgAsmGenericSection* genericSection = isSgAsmGenericSection(node);
                     if (genericSection != NULL)
                        {
                          SymbolPruningAttribute* att = new SymbolPruningAttribute();
                          genericSection->addNewAttribute("SymbolPruningAttribute",att);
                        }

                     SgAsmElfSectionTableEntry* elfSectionTableEntry = isSgAsmElfSectionTableEntry(node);
                     if (elfSectionTableEntry != NULL)
                        {
                          SymbolPruningAttribute* att = new SymbolPruningAttribute();
                          elfSectionTableEntry->addNewAttribute("SymbolPruningAttribute",att);
                        }

                     SgAsmElfSegmentTableEntry* elfSegmentTableEntry = isSgAsmElfSegmentTableEntry(node);
                     if (elfSegmentTableEntry != NULL)
                        {
                          SymbolPruningAttribute* att = new SymbolPruningAttribute();
                          elfSegmentTableEntry->addNewAttribute("SymbolPruningAttribute",att);
                        }

                     SgAsmElfRelaEntry* elfRelaEntry = isSgAsmElfRelaEntry(node);
                     if (elfRelaEntry != NULL)
                        {
                          SymbolPruningAttribute* att = new SymbolPruningAttribute();
                          elfRelaEntry->addNewAttribute("SymbolPruningAttribute",att);
                        }

                     SgAsmElfRelaEntryList* elfRelaEntryList = isSgAsmElfRelaEntryList(node);
                     if (elfRelaEntryList != NULL)
                        {
                          SymbolPruningAttribute* att = new SymbolPruningAttribute();
                          elfRelaEntryList->addNewAttribute("SymbolPruningAttribute",att);
                        }

                     SgAsmElfDynamicEntryList* elfDynamicEntryList = isSgAsmElfDynamicEntryList(node);
                     if (elfDynamicEntryList != NULL)
                        {
                          SymbolPruningAttribute* att = new SymbolPruningAttribute();
                          elfDynamicEntryList->addNewAttribute("SymbolPruningAttribute",att);
                        }

                     SgAsmGenericFormat* genericFormat = isSgAsmGenericFormat(node);
                     if (genericFormat != NULL)
                        {
                          SymbolPruningAttribute* att = new SymbolPruningAttribute();
                          genericFormat->addNewAttribute("SymbolPruningAttribute",att);
                        }

                     SgAsmGenericDLLList* genericDLLList = isSgAsmGenericDLLList(node);
                     if (genericDLLList != NULL)
                        {
                          SymbolPruningAttribute* att = new SymbolPruningAttribute();
                          genericDLLList->addNewAttribute("SymbolPruningAttribute",att);
                        }
#endif
                     SgAsmGenericSymbolList* genericSymbolList = isSgAsmGenericSymbolList(node);
                     if (genericSymbolList != NULL)
                        {
                          SymbolPruningAttribute* att = new SymbolPruningAttribute();
                          genericSymbolList->addNewAttribute("SymbolPruningAttribute",att);
                        }

                     SgAsmElfSymbolList* elfSymbolList = isSgAsmElfSymbolList(node);
                     if (elfSymbolList != NULL)
                        {
                          SymbolPruningAttribute* att = new SymbolPruningAttribute();
                          elfSymbolList->addNewAttribute("SymbolPruningAttribute",att);
                        }

                     SgAsmGenericSymbol* genericSymbol = isSgAsmGenericSymbol(node);
                     if (genericSymbol != NULL)
                        {
                          SymbolPruningAttribute* att = new SymbolPruningAttribute();
                          genericSymbol->addNewAttribute("SymbolPruningAttribute",att);
                        }
                  }
        };

     CommentOutSymbolsFromDotGraph traversal;
     traversal.traverse(node,preorder);
   }


void
readDwarf ( SgAsmFile* asmFile )
   {
     ROSE_ASSERT(asmFile != NULL);

     SgAsmGenericFile* genericFile = asmFile->get_genericFile();
     ROSE_ASSERT(genericFile != NULL);

     int fileDescriptor = genericFile->get_fd();

     int dwarf_init_status = dwarf_init (fileDescriptor, DW_DLC_READ, NULL, NULL, &rose_dwarf_dbg, &rose_dwarf_error);
  // printf ("dwarf_init_status = %d \n",dwarf_init_status);

  // Test if the call to dwarf_init worked!
  // ROSE_ASSERT(dwarf_init_status == DW_DLV_OK);
     if (dwarf_init_status == DW_DLV_OK)
        {
       // I am unclear about the functionality of these two functions!
       // dwarf_set_frame_rule_inital_value(rose_dwarf_dbg,global_config_file_data.cf_initial_rule_value);
       // dwarf_set_frame_rule_table_size(rose_dwarf_dbg,global_config_file_data.cf_table_entry_count);

       // Dwarf information will be attached to the main SgAsmInterpretation for the binary file.
          SgAsmInterpretation* asmInterpretation = SageInterface::getMainInterpretation(asmFile);     

       // Main function to read dwarf information
          build_dwarf_IR_nodes(rose_dwarf_dbg,asmInterpretation);

       // printf ("\n\nFinishing Dwarf handling... \n\n");
          int dwarf_finish_status = dwarf_finish( rose_dwarf_dbg, &rose_dwarf_error);
          ROSE_ASSERT(dwarf_finish_status == DW_DLV_OK);
        }
       else
        {
       // This might be a PE file (or just non-ELF)
          if (SgProject::get_verbose() > 0)
             {
               printf ("No dwarf debug sections found! \n");
             }
        }

  // DQ (11/10/2008): Added support to permit symbols to be removed from the DOT graph generation.
  // This make the DOT files easier to manage since there can be thousands of symbols.  This also
  // makes it easer to debug the ROSE dwarf AST.
     SgBinaryFile* binaryFile = isSgBinaryFile(asmFile->get_parent());
     ROSE_ASSERT (binaryFile != NULL);

  // This is used to reduce the size of the DOT file to simplify debugging Dwarf stuff.
     if (binaryFile->get_read_executable_file_format_only_skip_symbols() == true)
        {
          printf ("Calling commentOutSymbolsFromDotGraph() \n");
          commentOutSymbolsFromDotGraph(asmFile);
        }

   }




SgAsmDwarfConstruct*
SgAsmDwarfConstruct::createDwarfConstruct( int tag, int nesting_level, uint64_t offset, uint64_t overall_offset )
   {
  // This function implements the factory pattern.

     SgAsmDwarfConstruct* returnConstruct = NULL;

  // printf ("In SgAsmDwarfConstruct::createDwarfConstruct(): tag = %d nesting_level = %d \n",tag,nesting_level);

  // This uses the libdwarf tag id values
     switch(tag)
        {
       // case DW_TAG_: { returnConstruct = new SgAsmDwarf(nesting_level,offset,overall_offset); break; }

          case DW_TAG_array_type: { returnConstruct = new SgAsmDwarfArrayType(nesting_level,offset,overall_offset); break; }
          case DW_TAG_class_type: { returnConstruct = new SgAsmDwarfClassType(nesting_level,offset,overall_offset); break; }
          case DW_TAG_entry_point: { returnConstruct = new SgAsmDwarfEntryPoint(nesting_level,offset,overall_offset); break; }
          case DW_TAG_enumeration_type: { returnConstruct = new SgAsmDwarfEnumerationType(nesting_level,offset,overall_offset); break; }
          case DW_TAG_formal_parameter: { returnConstruct = new SgAsmDwarfFormalParameter(nesting_level,offset,overall_offset); break; }
          case DW_TAG_imported_declaration: { returnConstruct = new SgAsmDwarfImportedDeclaration(nesting_level,offset,overall_offset); break; }
          case DW_TAG_label: { returnConstruct = new SgAsmDwarfLabel(nesting_level,offset,overall_offset); break; }
          case DW_TAG_lexical_block: { returnConstruct = new SgAsmDwarfLexicalBlock(nesting_level,offset,overall_offset); break; }
          case DW_TAG_member: { returnConstruct = new SgAsmDwarfMember(nesting_level,offset,overall_offset); break; }
          case DW_TAG_pointer_type: { returnConstruct = new SgAsmDwarfPointerType(nesting_level,offset,overall_offset); break; }
          case DW_TAG_reference_type: { returnConstruct = new SgAsmDwarfReferenceType(nesting_level,offset,overall_offset); break; }
          case DW_TAG_compile_unit: { returnConstruct = new SgAsmDwarfCompilationUnit(nesting_level,offset,overall_offset); break; }
          case DW_TAG_string_type: { returnConstruct = new SgAsmDwarfStringType(nesting_level,offset,overall_offset); break; }
          case DW_TAG_structure_type: { returnConstruct = new SgAsmDwarfStructureType(nesting_level,offset,overall_offset); break; }
          case DW_TAG_subroutine_type: { returnConstruct = new SgAsmDwarfSubroutineType(nesting_level,offset,overall_offset); break; }
          case DW_TAG_typedef: { returnConstruct = new SgAsmDwarfTypedef(nesting_level,offset,overall_offset); break; }
          case DW_TAG_union_type: { returnConstruct = new SgAsmDwarfUnionType(nesting_level,offset,overall_offset); break; }
          case DW_TAG_unspecified_parameters: { returnConstruct = new SgAsmDwarfUnspecifiedParameters(nesting_level,offset,overall_offset); break; }
          case DW_TAG_variant: { returnConstruct = new SgAsmDwarfVariant(nesting_level,offset,overall_offset); break; }
          case DW_TAG_common_block: { returnConstruct = new SgAsmDwarfCommonBlock(nesting_level,offset,overall_offset); break; }
          case DW_TAG_common_inclusion: { returnConstruct = new SgAsmDwarfCommonInclusion(nesting_level,offset,overall_offset); break; }
          case DW_TAG_inheritance: { returnConstruct = new SgAsmDwarfInheritance(nesting_level,offset,overall_offset); break; }
          case DW_TAG_inlined_subroutine: { returnConstruct = new SgAsmDwarfInlinedSubroutine(nesting_level,offset,overall_offset); break; }
          case DW_TAG_module: { returnConstruct = new SgAsmDwarfModule(nesting_level,offset,overall_offset); break; }
          case DW_TAG_ptr_to_member_type: { returnConstruct = new SgAsmDwarfPtrToMemberType(nesting_level,offset,overall_offset); break; }
          case DW_TAG_set_type: { returnConstruct = new SgAsmDwarfSetType(nesting_level,offset,overall_offset); break; }
          case DW_TAG_subrange_type: { returnConstruct = new SgAsmDwarfSubrangeType(nesting_level,offset,overall_offset); break; }
          case DW_TAG_with_stmt: { returnConstruct = new SgAsmDwarfWithStmt(nesting_level,offset,overall_offset); break; }
          case DW_TAG_access_declaration: { returnConstruct = new SgAsmDwarfAccessDeclaration(nesting_level,offset,overall_offset); break; }
          case DW_TAG_base_type: { returnConstruct = new SgAsmDwarfBaseType(nesting_level,offset,overall_offset); break; }
          case DW_TAG_catch_block: { returnConstruct = new SgAsmDwarfCatchBlock(nesting_level,offset,overall_offset); break; }
          case DW_TAG_const_type: { returnConstruct = new SgAsmDwarfConstType(nesting_level,offset,overall_offset); break; }
          case DW_TAG_constant: { returnConstruct = new SgAsmDwarfConstant(nesting_level,offset,overall_offset); break; }
          case DW_TAG_enumerator: { returnConstruct = new SgAsmDwarfEnumerator(nesting_level,offset,overall_offset); break; }
          case DW_TAG_file_type: { returnConstruct = new SgAsmDwarfFileType(nesting_level,offset,overall_offset); break; }
          case DW_TAG_friend: { returnConstruct = new SgAsmDwarfFriend(nesting_level,offset,overall_offset); break; }
          case DW_TAG_namelist: { returnConstruct = new SgAsmDwarfNamelist(nesting_level,offset,overall_offset); break; }
          case DW_TAG_namelist_item: { returnConstruct = new SgAsmDwarfNamelistItem(nesting_level,offset,overall_offset); break; }
          case DW_TAG_packed_type: { returnConstruct = new SgAsmDwarfPackedType(nesting_level,offset,overall_offset); break; }
          case DW_TAG_subprogram: { returnConstruct = new SgAsmDwarfSubprogram(nesting_level,offset,overall_offset); break; }
          case DW_TAG_template_type_parameter: { returnConstruct = new SgAsmDwarfTemplateTypeParameter(nesting_level,offset,overall_offset); break; }
          case DW_TAG_template_value_parameter: { returnConstruct = new SgAsmDwarfTemplateValueParameter(nesting_level,offset,overall_offset); break; }
          case DW_TAG_thrown_type: { returnConstruct = new SgAsmDwarfThrownType(nesting_level,offset,overall_offset); break; }
          case DW_TAG_try_block: { returnConstruct = new SgAsmDwarfTryBlock(nesting_level,offset,overall_offset); break; }
          case DW_TAG_variant_part: { returnConstruct = new SgAsmDwarfVariantPart(nesting_level,offset,overall_offset); break; }
          case DW_TAG_variable: { returnConstruct = new SgAsmDwarfVariable(nesting_level,offset,overall_offset); break; }
          case DW_TAG_volatile_type: { returnConstruct = new SgAsmDwarfVolatileType(nesting_level,offset,overall_offset); break; }
          case DW_TAG_dwarf_procedure: { returnConstruct = new SgAsmDwarfDwarfProcedure(nesting_level,offset,overall_offset); break; }
          case DW_TAG_restrict_type: { returnConstruct = new SgAsmDwarfRestrictType(nesting_level,offset,overall_offset); break; }
          case DW_TAG_interface_type: { returnConstruct = new SgAsmDwarfInterfaceType(nesting_level,offset,overall_offset); break; }
          case DW_TAG_namespace: { returnConstruct = new SgAsmDwarfNamespace(nesting_level,offset,overall_offset); break; }
          case DW_TAG_imported_module: { returnConstruct = new SgAsmDwarfImportedModule(nesting_level,offset,overall_offset); break; }
          case DW_TAG_unspecified_type: { returnConstruct = new SgAsmDwarfUnspecifiedType(nesting_level,offset,overall_offset); break; }
          case DW_TAG_partial_unit: { returnConstruct = new SgAsmDwarfPartialUnit(nesting_level,offset,overall_offset); break; }
          case DW_TAG_imported_unit: { returnConstruct = new SgAsmDwarfImportedUnit(nesting_level,offset,overall_offset); break; }
          case DW_TAG_mutable_type: { returnConstruct = new SgAsmDwarfMutableType(nesting_level,offset,overall_offset); break; }
          case DW_TAG_condition: { returnConstruct = new SgAsmDwarfCondition(nesting_level,offset,overall_offset); break; }
          case DW_TAG_shared_type: { returnConstruct = new SgAsmDwarfSharedType(nesting_level,offset,overall_offset); break; }
          case DW_TAG_format_label: { returnConstruct = new SgAsmDwarfFormatLabel(nesting_level,offset,overall_offset); break; }
          case DW_TAG_function_template: { returnConstruct = new SgAsmDwarfFunctionTemplate(nesting_level,offset,overall_offset); break; }
          case DW_TAG_upc_shared_type: { returnConstruct = new SgAsmDwarfUpcSharedType(nesting_level,offset,overall_offset); break; }
          case DW_TAG_upc_strict_type: { returnConstruct = new SgAsmDwarfUpcStrictType(nesting_level,offset,overall_offset); break; }
          case DW_TAG_upc_relaxed_type: { returnConstruct = new SgAsmDwarfUpcRelaxedType(nesting_level,offset,overall_offset); break; }

          default:
             {
            // Use AsmDwarfVariant as a default dwarf construct (we might want a SgAsmDwarfUnknownConstruct also).
            // returnConstruct = new SgAsmDwarfVariant(nesting_level,offset,overall_offset);
               returnConstruct = new SgAsmDwarfUnknownConstruct(nesting_level,offset,overall_offset);
             }
        }

#if 0
     printf ("createDwarfConstruct(): nesting_level = %d building: %s \n",nesting_level,returnConstruct->class_name().c_str());
#endif

     return returnConstruct;
   }


// endif for "if USE_ROSE_DWARF_SUPPORT" at top of file.
#endif






