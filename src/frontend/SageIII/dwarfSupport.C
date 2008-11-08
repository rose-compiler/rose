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

bool info_flag = true;

// Dwarf variables
Dwarf_Debug rose_dwarf_dbg;
Dwarf_Error rose_dwarf_error;

int verbose = 1;
bool dense = false;

bool show_global_offsets = false;
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
print_attribute(Dwarf_Debug dbg, Dwarf_Die die, Dwarf_Half attr,Dwarf_Attribute attr_in,bool print_information,char **srcfiles, Dwarf_Signed cnt)
   {
  // This function is not implemented!

     printf ("Error: print_attribute is not implemented! \n");
  // ROSE_ASSERT(false);
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



/* print info about die */
void
print_one_die(Dwarf_Debug dbg, Dwarf_Die die, bool print_information, char **srcfiles, Dwarf_Signed cnt, SgAsmDwarfCompilationUnit* asmDwarfCompilationUnit)
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
     printf ("tag = %d tagname = %s \n",tag,tagname.c_str());

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


     if (indent_level == 0)
        {
       // This is the header

        }
       else
        {
       // These are local symbols
        }


  // if (!dst_format && print_information)
     if (print_information)
        {
          if (indent_level == 0)
            {
              if (dense)
                   printf("\n");
                else
                 {
                   printf("\nCOMPILE_UNIT<header overall offset = %llu>:\n",overall_offset - offset);
                 }

           // Initialize the information in the SgAsmDwarfCompilationUnit IR node

            }
           else
            {
              if (local_symbols_already_began == false && indent_level == 1 && !dense)
                 {
                   printf("\nLOCAL_SYMBOLS:\n");
                   local_symbols_already_began = true;
                 }
            }

          if (dense)
             {
               if (show_global_offsets)
                  {
                    if (indent_level == 0)
                       {
                         printf("<%d><%llu+%llu GOFF=%llu><%s>", indent_level,overall_offset - offset, offset,overall_offset, tagname.c_str());
                       }
                      else
                       {
                         printf("<%d><%llu GOFF=%llu><%s>", indent_level,offset, overall_offset, tagname.c_str());
                       }
                  }
                 else
                  {
                    if (indent_level == 0)
                       {
                         printf("<%d><%llu+%llu><%s>", indent_level,overall_offset - offset, offset, tagname.c_str());
                       }
                      else
                       {
                         printf("<%d><%llu><%s>", indent_level, offset, tagname.c_str());
                       }
                  }
             } 
            else
             {
               if (show_global_offsets)
                  {
                    printf("<%d><%5llu GOFF=%llu>\t%s\n", indent_level, offset,overall_offset, tagname.c_str());
                  }
                 else
                  {
                    printf("<%d><%5llu>\t%s\n", indent_level, offset, tagname.c_str());
                  }
             }
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
               printf ("No attributes found (not an error) \n");
               atcnt = 0;
             }
        }

     printf ("Handle attributes: atcnt = %d \n",(int)atcnt);
     for (i = 0; i < atcnt; i++)
        {
          Dwarf_Half attr;
          int ares;

          ares = dwarf_whatattr(atlist[i], &attr, &rose_dwarf_error);
          if (ares == DW_DLV_OK)
             {
               print_attribute(dbg, die, attr,atlist[i],print_information, srcfiles, cnt);
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

     if (dense && print_information)
        {
          printf("\n\n");
        }

     return;
   }



/* recursively follow the die tree */
void
build_dwarf_IR_node_from_die_and_children(Dwarf_Debug dbg, Dwarf_Die in_die_in,char **srcfiles, Dwarf_Signed cnt, SgAsmDwarfCompilationUnit* asmDwarfCompilationUnit)
   {
     Dwarf_Die child;
     Dwarf_Die sibling;
     Dwarf_Error err;
     int tres;
     int cdres;
     Dwarf_Die in_die = in_die_in;

  // printf ("Inside of build_dwarf_IR_node_from_die_and_children() \n");

     for (;;)
        {
       // printf ("Top of loop in build_dwarf_IR_node_from_die_and_children() \n");

          PUSH_DIE_STACK(in_die);

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
                         if (cres == DW_DLV_OK) {
                              ctagname = get_TAG_name(dbg, tag_child);
                            }
                         if (pres == DW_DLV_OK) {
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
#if 1
       // Suppress output!

       // DQ (11/4/2008): This is the location were we will have to generate IR nodes using each debug info entry (die)
          printf ("Calling print_one_die to output the information about each specific debug info entry (die) \n");

       /* here to pre-descent processing of the die */
          print_one_die(dbg, in_die, /* info_flag */ true, srcfiles, cnt, asmDwarfCompilationUnit);

          printf ("Process children \n");
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

            // print_die_and_children(dbg, child, srcfiles, cnt);
               build_dwarf_IR_node_from_die_and_children(dbg, child, srcfiles, cnt, asmDwarfCompilationUnit);

               indent_level--;
               if (indent_level == 0)
                    local_symbols_already_began = false;
               dwarf_dealloc(dbg, child, DW_DLA_DIE);
             }
            else
               if (cdres == DW_DLV_ERROR)
                  {
                    print_error(dbg, "dwarf_child", cdres, err);
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

     return;
   }


void
build_dwarf_line_numbers_this_cu(Dwarf_Debug dbg, Dwarf_Die cu_die, SgAsmDwarfCompilationUnit* asmDwarfCompilationUnit)
   {
     Dwarf_Signed linecount = 0;
     Dwarf_Line *linebuf = NULL;
     Dwarf_Signed i = 0;
     Dwarf_Addr pc = 0;
     Dwarf_Unsigned lineno = 0;
     Dwarf_Signed column = 0;
     char* filename;
     Dwarf_Bool newstatement = 0;
     Dwarf_Bool lineendsequence = 0;
     Dwarf_Bool new_basic_block = 0;
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
     ROSE_ASSERT(asmDwarfCompilationUnit->get_line_info() != NULL);

     if (verbose > 1)
        {
          print_source_intro(cu_die);
          print_one_die(dbg, cu_die, /* print_information= */ 1,/* srcfiles= */ 0, /* cnt= */ 0, asmDwarfCompilationUnit);

          lres = dwarf_print_lines(cu_die, &rose_dwarf_error);
          if (lres == DW_DLV_ERROR)
             {
               print_error(dbg, "dwarf_srclines details", lres, rose_dwarf_error);
             }

          printf ("Exiting print_line_numbers_this_cu prematurely! \n");
          return;
       }

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
#if 1
            // Suppress output!
               print_source_intro(cu_die);
               if (verbose)
                  {
                    print_one_die(dbg, cu_die, /* print_information= */ 1,/* srcfiles= */ 0, /* cnt= */ 0, asmDwarfCompilationUnit);
                  }
#endif
            // Output a header for the data
            // printf("<source>\t[row,column]\t<pc>\t//<new statement or basic block\n");

               for (i = 0; i < linecount; i++)
                  {
                    Dwarf_Line line = linebuf[i];
                    int nsres;

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
#if 0
                 // Suppress output!
                    printf("\n");
#endif
                  }

               dwarf_srclines_dealloc(dbg, linebuf, linecount);
             }
        }

  // printf ("Now generate the maps to use to lookup the instruction address to source position mappings \n");
     asmDwarfLineList->buildInstructionAddressSourcePositionMaps(asmDwarfCompilationUnit);
  // printf ("DONE: Now generate the maps to use to lookup the instruction address to source position mappings \n");

#if 0
  // Run a second time to test that the maps are not regenerated
     printf ("Run a second time to test that the maps are not regenerated ... \n");
     DwarfInstructionSourceMapReturnType returnValue = asmDwarfLineList->buildInstructionAddressSourcePositionMaps();
     printf ("DONE: Run a second time to test that the maps are not regenerated ... \n");
#endif

#if 1
  // Output the line information from the generated maps.
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

  // printf ("Inside of print_infos() \n");
  // if (info_flag)
  //      printf("\n.debug_info\n");

  // printf("\n.debug_info\n");
     SgAsmDwarfCompilationUnit* asmDwarfCompilationUnit = new SgAsmDwarfCompilationUnit();

  // binaryFile->set_dwarf_info(asmDwarfCompilationUnit);
  // ROSE_ASSERT(binaryFile->get_dwarf_info() != NULL);
     asmInterpretation->set_dwarf_info(asmDwarfCompilationUnit);
     ROSE_ASSERT(asmInterpretation->get_dwarf_info() != NULL);

    /* Loop until it fails.  */
     while ((nres = dwarf_next_cu_header(dbg, &cu_header_length, &version_stamp,&abbrev_offset, &address_size,&next_cu_offset, &rose_dwarf_error)) == DW_DLV_OK)
        {
#if 0
          printf ("In loop over the headers: nres = %d \n",nres);
          printf ("next_cu_offset   = %lu \n",(unsigned long) next_cu_offset);
          printf ("cu_header_length = %lu \n",(unsigned long) cu_header_length);
#endif

       // printf ("loop count: cu_count = %d \n",cu_count);

          if (cu_count >=  break_after_n_units)
             {
               printf("Break at %d\n",cu_count);
               break;
             }

          int sres = 0;
#if 0
          printf ("cu_name_flag = %s \n",cu_name_flag ? "true" : "false");
#endif


       // printf ("Setting cu_name_flag == true \n");
       // cu_name_flag = true;
       // cu_name_flag = false;

          ROSE_ASSERT(cu_name_flag == false);

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

#if 0
          printf ("build_dwarf_IR_nodes(): verbose = %s \n",verbose ? "true" : "false");
          printf ("build_dwarf_IR_nodes(): dense   = %s \n",dense ? "true" : "false");
#endif

#if 0
       // Supress output!
          if (verbose)
             {
               if (dense) {
                    printf("<%s>", "cu_header");
                    printf(" %s<%llu>", "cu_header_length",cu_header_length);
                    printf(" %s<%d>", "version_stamp", version_stamp);
                    printf(" %s<%llu>", "abbrev_offset", abbrev_offset);
                    printf(" %s<%d>\n", "address_size", address_size);
               } else {
                    printf("\nCU_HEADER:\n");
                    printf("\t\t%-28s%llu\n", "cu_header_length",cu_header_length);
                    printf("\t\t%-28s%d\n", "version_stamp", version_stamp);
                    printf("\t\t%-28s%llu\n", "abbrev_offset",abbrev_offset);
                    printf("\t\t%-28s%d", "address_size", address_size);
                  }
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

               if (info_flag || cu_name_flag)
                  {
                    Dwarf_Signed cnt = 0;
                    char **srcfiles = 0;
                    int srcf = dwarf_srcfiles(cu_die,&srcfiles, &cnt, &rose_dwarf_error);

                    if (srcf != DW_DLV_OK)
                       {
                         srcfiles = 0;
                         cnt = 0;
                       }

                 // This function call will traverse the list of child debug info entries and 
                 // within this function we will generate the IR nodes specific to Dwarf. This
                 // should define an course view of the AST which can be used to relate the binary
                 // to the source code.

                 // printf ("In print_infos(): Calling print_die_and_children() \n");
                 // print_die_and_children(dbg, cu_die, srcfiles, cnt);
                    build_dwarf_IR_node_from_die_and_children(dbg, cu_die, srcfiles, cnt, asmDwarfCompilationUnit);

                    if (srcf == DW_DLV_OK)
                       {
                         int si;

                         for (si = 0; si < cnt; ++si)
                            {
                              dwarf_dealloc(dbg, srcfiles[si], DW_DLA_STRING);
                            }
                         dwarf_dealloc(dbg, srcfiles, DW_DLA_LIST);
                       }
                  }

            // printf ("Explicitly setting line_flag == true \n");
               bool line_flag = true;

               if (line_flag)
                  {
                 // printf ("\n\nOutput the line information by calling print_line_numbers_this_cu() \n");
                 // print_line_numbers_this_cu(dbg, cu_die);
                    build_dwarf_line_numbers_this_cu(dbg, cu_die, asmDwarfCompilationUnit);
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
   }

// endif for "if USE_ROSE_DWARF_SUPPORT" at top of file.
#endif

