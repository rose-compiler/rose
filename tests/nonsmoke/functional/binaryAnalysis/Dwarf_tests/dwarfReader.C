/*
A bit of info abour Dward versions numbers and sections 
available in each version number. Copied from:
http://wiki.dwarfstd.org/index.php?title=Dwarf_Version_Numbers

Dwarf Version Numbers:
   As DWARF4 is not released the list of entries for DWARF4 and 
indeed the existence of DWARF4 must be considered speculative.
The versions applicable by section are, so far: 

                      DWARF2    DWARF3 DWARF4
.debug_info             2         3     4
.debug_abbrev           -         -     -
.debug_frame            1         3     3
.debug_str              -         -     -
.debug_loc              -         -     -
.debug_line             2         3     3
.debug_aranges          2         2     2
.debug_ranges           x         -     -
.debug_pubtypes         x         2     2
.debug_pubnames         2         2     2
.debug_macinfo          -         -     -

Where - means there is no version number in the section, and 
where x means the section did not exist in that version.

*/


// We must have available the EXACT version of the dwarf library, otherwise things become undefined. Therefore, only compile
// this when the user defines COMPILE_DWARF_READER.
#ifndef COMPILE_DWARF_READER

#include <iostream>
int main() {
    std::cerr <<"Not tested unless COMPILE_DWARF_READER is defined.\n";
}
#else


#include <rose.h>

#include <dwarf.h>
#include <libdwarf.h>

// This is a header file from dwarfdump that defines "dwconf_s"
#include "../dwarfdump/dwconf.h"
#include "../dwarfdump/_tag_attr_table.c"
#include "../dwarfdump/_tag_tree_table.c"


using namespace std;

Dwarf_Debug rose_dwarf_dbg;
Dwarf_Error rose_dwarf_error;

// Required variables:
int break_after_n_units = INT_MAX;

struct dwconf_s global_config_file_data;

char cu_name[BUFSIZ];
bool cu_name_flag = false;
Dwarf_Unsigned cu_offset = 0;

// values to control level or output
int verbose = 1;
bool dense = false;

bool ellipsis            = false;
bool dst_format          = false;
bool show_global_offsets = false;

int indent_level = 0;
bool local_symbols_already_began = false;

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

typedef string(*encoding_type_func) (Dwarf_Debug dbg, Dwarf_Half val);


// See comments in dwarfdump about this fix.
bool suppress_nested_name_search = false;






// Not clearly required variables:
// extern Dwarf_Error err;

extern void print_error (Dwarf_Debug dbg, string msg, int res, Dwarf_Error err);

string program_name = "dwarfReader";

bool info_flag = false;
bool use_old_dwarf_loclist = false;  /* This so both dwarf_loclist() 
                                        and dwarf_loclist_n() can be
                                        tested. Defaults to new
                                        dwarf_loclist_n() */

bool line_flag = false;
bool abbrev_flag = false;
bool frame_flag = false;      /* .debug_frame section. */
bool eh_frame_flag = false;   /* GNU .eh_frame section. */
bool pubnames_flag = false;
bool macinfo_flag = false;
bool loc_flag = false;
bool aranges_flag = false;
bool string_flag = false;
bool reloc_flag = false;
bool static_func_flag = false;
bool static_var_flag = false;
bool type_flag = false;
bool weakname_flag = false;


// string process_args(int argc, char *argv[]);
void print_infos(Dwarf_Debug dbg);
// void print_usage_message(void);

extern void print_frames (Dwarf_Debug dbg, int print_debug_frame, int print_eh_frame,struct dwconf_s *);

extern void print_pubnames (Dwarf_Debug dbg);
extern void print_macinfo (Dwarf_Debug dbg);
extern void print_locs (Dwarf_Debug dbg);
extern void print_abbrevs (Dwarf_Debug dbg);
extern void print_strings (Dwarf_Debug dbg);
extern void print_aranges (Dwarf_Debug dbg);
extern void print_relocinfo (Dwarf_Debug dbg);
extern void print_static_funcs(Dwarf_Debug dbg);
extern void print_static_vars(Dwarf_Debug dbg);
enum type_type_e {SGI_TYPENAME, DWARF_PUBTYPES} ;
extern void print_types(Dwarf_Debug dbg,enum type_type_e type_type);
extern void print_weaknames(Dwarf_Debug dbg);
extern void print_exception_tables(Dwarf_Debug dbg);
extern string get_fde_proc_name(Dwarf_Debug dbg, Dwarf_Addr low_pc);
extern void print_die_and_children(
        Dwarf_Debug dbg, 
        Dwarf_Die in_die,
        char **srcfiles,
        Dwarf_Signed cnt);
extern void print_one_die(
        Dwarf_Debug dbg, 
        Dwarf_Die die, 
        bool print_information,
        char **srcfiles,
        Dwarf_Signed cnt);

Dwarf_Die current_cu_die_for_print_frames; /* This is
        an awful hack, making this public. But it enables
        cleaning up (doing all dealloc needed). */


bool dwarf_check = false;
bool check_pubname_attr = false;

typedef struct {
    int checks;
    int errors;
} Dwarf_Check_Result;

Dwarf_Check_Result pubname_attr_result;
Dwarf_Check_Result attr_tag_result;
Dwarf_Check_Result tag_tree_result;
Dwarf_Check_Result type_offset_result;
Dwarf_Check_Result decl_file_result;

#define PRINT_CHECK_RESULT(str,result)  {\
    fprintf(stderr, "%-24s %8d %8d\n", str, result.checks, result.errors); \
}

bool check_attr_tag = false;
bool check_tag_tree = false;
bool check_type_offset = false;
bool check_decl_file = false;







Dwarf_Off fde_offset_for_cu_low = DW_DLV_BADOFFSET;
Dwarf_Off fde_offset_for_cu_high = DW_DLV_BADOFFSET;

/* Dwarf_Half list_of_attrs[] */
/*#include "at_list.i" unreferenced */

#define DIE_STACK_SIZE 300
static Dwarf_Die die_stack[DIE_STACK_SIZE];

#define PUSH_DIE_STACK(x) { die_stack[indent_level] = x; }
#define POP_DIE_STACK { die_stack[indent_level] = 0; }


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


void
print_line_numbers_this_cu(Dwarf_Debug dbg, Dwarf_Die cu_die)
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

     printf("\n.debug_line: line number info for a single cu\n");

  // printf ("Setting verbose > 1 (verbose==2) \n");
  // verbose = 2;

     if (verbose > 1)
        {
          print_source_intro(cu_die);
          print_one_die(dbg, cu_die, /* print_information= */ 1,/* srcfiles= */ 0, /* cnt= */ 0);

          lres = dwarf_print_lines(cu_die, &rose_dwarf_error);
          if (lres == DW_DLV_ERROR)
             {
               print_error(dbg, "dwarf_srclines details", lres, rose_dwarf_error);
             }

       // printf ("Exiting print_line_numbers_this_cu prematurely! \n");
       // return;
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
               print_source_intro(cu_die);
               if (verbose)
                  {
                    print_one_die(dbg, cu_die, /* print_information= */ 1,/* srcfiles= */ 0, /* cnt= */ 0);
                  }

               printf("<source>\t[row,column]\t<pc>\t//<new statement or basic block\n");

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
                    printf("%s:\t[%3llu,%2lld]\t%#llx", filename, lineno,column, pc);

                    if (sres == DW_DLV_OK)
                         dwarf_dealloc(dbg, filename, DW_DLA_STRING);

                    nsres = dwarf_linebeginstatement(line, &newstatement, &rose_dwarf_error);
                    if (nsres == DW_DLV_OK)
                       {
                         if (newstatement)
                            {
                              printf("\t// new statement");
                            }
                       }
                      else
                         if (nsres == DW_DLV_ERROR)
                            {
                              print_error(dbg, "linebeginstatment failed", nsres,rose_dwarf_error);
                            }

                    nsres = dwarf_lineblock(line, &new_basic_block, &rose_dwarf_error);
                    if (nsres == DW_DLV_OK)
                       {
                         if (new_basic_block)
                            {
                              printf("\t// new basic block");
                            }
                       }
                      else
                         if (nsres == DW_DLV_ERROR)
                            {
                              print_error(dbg, "lineblock failed", nsres, rose_dwarf_error);
                            }

                    nsres = dwarf_lineendsequence(line, &lineendsequence, &rose_dwarf_error);
                    if (nsres == DW_DLV_OK)
                       {
                         if (lineendsequence)
                            {
                              printf("\t// end of text sequence");
                            }
                       }
                      else
                         if (nsres == DW_DLV_ERROR)
                            {
                              print_error(dbg, "lineblock failed", nsres, rose_dwarf_error);
                            }
                    printf("\n");
                  }
               dwarf_srclines_dealloc(dbg, linebuf, linecount);
             }
        }
   }


/* process each compilation unit in .debug_info */
void
print_infos(Dwarf_Debug dbg)
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

     printf("\n.debug_info\n");

    /* Loop until it fails.  */
     while ((nres =
            dwarf_next_cu_header(dbg, &cu_header_length, &version_stamp,
                                 &abbrev_offset, &address_size,
                                 &next_cu_offset, &rose_dwarf_error)) == DW_DLV_OK)
        {
#if 0
          printf ("In loop over the headers: nres = %d \n",nres);
          printf ("next_cu_offset   = %lu \n",(unsigned long) next_cu_offset);
          printf ("cu_header_length = %lu \n",(unsigned long) cu_header_length);
#endif

          printf ("loop count: cu_count = %d \n",cu_count);

          if(cu_count >=  break_after_n_units)
             {
               printf("Break at %d\n",cu_count);
               break;
             }

          int sres = 0;
#if 1
          printf ("cu_name_flag = %s \n",cu_name_flag ? "true" : "false");
#endif
          if (cu_name_flag)
             {
               int tres = 0;
               Dwarf_Half tag = 0;
               Dwarf_Attribute attrib = 0;
               Dwarf_Half theform = 0;
               int fres = 0;
               int ares = 0;

               sres = dwarf_siblingof(dbg, NULL, &cu_die, &rose_dwarf_error);
               if (sres != DW_DLV_OK) {
                    print_error(dbg, "siblingof cu header", sres, rose_dwarf_error);
                  }

               tres = dwarf_tag(cu_die, &tag, &rose_dwarf_error);
               if (tres != DW_DLV_OK) {
                    print_error(dbg, "tag of cu die", tres, rose_dwarf_error);
                  }

               ares = dwarf_attr(cu_die, DW_AT_name, &attrib, &rose_dwarf_error);
               if (ares != DW_DLV_OK) {
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
                         if (strres != DW_DLV_OK) {
                              print_error(dbg,"formstring failed unexpectedly",strres, rose_dwarf_error);
                            }
                         if (cu_name[0] != '/') {
                              p = strrchr(temps, '/');
                              if (p == NULL) {
                                 p = temps;
                               } else {
                                 p++;
                               }
                            }
                         if (strcmp(cu_name, p)) {
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

#if 1
          printf ("verbose = %s \n",verbose ? "true" : "false");
          printf ("dense   = %s \n",dense ? "true" : "false");
#endif

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

        /* process a single compilation unit in .debug_info. */
        sres = dwarf_siblingof(dbg, NULL, &cu_die, &rose_dwarf_error);

     // printf ("status of call to dwarf_siblingof(): sres = %d \n",sres);

          if (sres == DW_DLV_OK)
             {
#if 0
               printf ("Processing sibling information \n");
               printf ("info_flag    = %s \n",info_flag ? "true" : "false");
               printf ("cu_name_flag = %s \n",cu_name_flag ? "true" : "false");
#endif
               printf ("Explicitly setting info_flag == true \n");
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

                    printf ("In print_infos(): Calling print_die_and_children() \n");
                    print_die_and_children(dbg, cu_die, srcfiles, cnt);

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

               printf ("Explicitly setting line_flag == true \n");
               line_flag = true;

               if (line_flag)
                    print_line_numbers_this_cu(dbg, cu_die);

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
          fprintf(stderr, "attempting to continue.\n");

          printf ("Error: nres = %d \n",nres);
          ROSE_ASSERT(false);
        }

  // printf ("Exiting print_infos() \n");
   }




#if 1
void
print_error(Dwarf_Debug dbg, string msg, int dwarf_code,
            Dwarf_Error err)
{
    fflush(stdout);
    fflush(stderr);
    if (dwarf_code == DW_DLV_ERROR) {
        string errmsg = dwarf_errmsg(err);
        Dwarf_Unsigned myerr = dwarf_errno(err);

        fprintf(stderr, "%s ERROR:  %s:  %s (%lu)\n",
                program_name.c_str(), msg.c_str(), errmsg.c_str(), (unsigned long) myerr);
    } else if (dwarf_code == DW_DLV_NO_ENTRY) {
        fprintf(stderr, "%s NO ENTRY:  %s: \n", program_name.c_str(), msg.c_str());
    } else if (dwarf_code == DW_DLV_OK) {
        fprintf(stderr, "%s:  %s \n", program_name.c_str(), msg.c_str());
    } else {
        fprintf(stderr, "%s InternalError:  %s:  code %d\n",
                program_name.c_str(), msg.c_str(), dwarf_code);
    }
    fflush(stderr);
    exit(1);
}
#endif

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
		 fprintf(stderr,"TAG of %d (0x%x) is unknown to dwarfdump. " 
 		 "Continuing. \n",(int)val,(int)val );  
		    n = makename(buf);
		    return n; 
		} 
	}
/*NOTREACHED*/
}


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

extern string
get_children_name (Dwarf_Debug dbg, Dwarf_Half val)
{
	switch (val) {
	case DW_children_no:
		if (ellipsis)
			return "children_no";
		else
			return "DW_children_no";
	case DW_children_yes:
		if (ellipsis)
			return "children_yes";
		else
			return "DW_children_yes";
	default:
		{ 
		    char buf[100]; 
		    char *n; 
		    snprintf(buf,sizeof(buf),"<Unknown children value 0x%x>",(int)val);
		 fprintf(stderr,"children of %d (0x%x) is unknown to dwarfdump. " 
 		 "Continuing. \n",(int)val,(int)val );  
		    n = makename(buf);
		    return n; 
		} 
	}
/*NOTREACHED*/
}

/* ARGSUSED */
extern string
get_FORM_name (Dwarf_Debug dbg, Dwarf_Half val)
{
	switch (val) {
	case DW_FORM_addr:
		if (ellipsis)
			return "addr";
		else
			return "DW_FORM_addr";
	case DW_FORM_block2:
		if (ellipsis)
			return "block2";
		else
			return "DW_FORM_block2";
	case DW_FORM_block4:
		if (ellipsis)
			return "block4";
		else
			return "DW_FORM_block4";
	case DW_FORM_data2:
		if (ellipsis)
			return "data2";
		else
			return "DW_FORM_data2";
	case DW_FORM_data4:
		if (ellipsis)
			return "data4";
		else
			return "DW_FORM_data4";
	case DW_FORM_data8:
		if (ellipsis)
			return "data8";
		else
			return "DW_FORM_data8";
	case DW_FORM_string:
		if (ellipsis)
			return "string";
		else
			return "DW_FORM_string";
	case DW_FORM_block:
		if (ellipsis)
			return "block";
		else
			return "DW_FORM_block";
	case DW_FORM_block1:
		if (ellipsis)
			return "block1";
		else
			return "DW_FORM_block1";
	case DW_FORM_data1:
		if (ellipsis)
			return "data1";
		else
			return "DW_FORM_data1";
	case DW_FORM_flag:
		if (ellipsis)
			return "flag";
		else
			return "DW_FORM_flag";
	case DW_FORM_sdata:
		if (ellipsis)
			return "sdata";
		else
			return "DW_FORM_sdata";
	case DW_FORM_strp:
		if (ellipsis)
			return "strp";
		else
			return "DW_FORM_strp";
	case DW_FORM_udata:
		if (ellipsis)
			return "udata";
		else
			return "DW_FORM_udata";
	case DW_FORM_ref_addr:
		if (ellipsis)
			return "ref_addr";
		else
			return "DW_FORM_ref_addr";
	case DW_FORM_ref1:
		if (ellipsis)
			return "ref1";
		else
			return "DW_FORM_ref1";
	case DW_FORM_ref2:
		if (ellipsis)
			return "ref2";
		else
			return "DW_FORM_ref2";
	case DW_FORM_ref4:
		if (ellipsis)
			return "ref4";
		else
			return "DW_FORM_ref4";
	case DW_FORM_ref8:
		if (ellipsis)
			return "ref8";
		else
			return "DW_FORM_ref8";
	case DW_FORM_ref_udata:
		if (ellipsis)
			return "ref_udata";
		else
			return "DW_FORM_ref_udata";
	case DW_FORM_indirect:
		if (ellipsis)
			return "indirect";
		else
			return "DW_FORM_indirect";
	default:
		{ 
		    char buf[100]; 
		    char *n; 
		    snprintf(buf,sizeof(buf),"<Unknown FORM value 0x%x>",(int)val);
		 fprintf(stderr,"FORM of %d (0x%x) is unknown to dwarfdump. " 
 		 "Continuing. \n",(int)val,(int)val );  
		    n = makename(buf);
		    return n; 
		} 
	}
/*NOTREACHED*/
}

extern string
get_OP_name (Dwarf_Debug dbg, Dwarf_Half val)
{
	switch (val) {
	case DW_OP_addr:
		if (ellipsis)
			return "addr";
		else
			return "DW_OP_addr";
	case DW_OP_deref:
		if (ellipsis)
			return "deref";
		else
			return "DW_OP_deref";
	case DW_OP_const1u:
		if (ellipsis)
			return "const1u";
		else
			return "DW_OP_const1u";
	case DW_OP_const1s:
		if (ellipsis)
			return "const1s";
		else
			return "DW_OP_const1s";
	case DW_OP_const2u:
		if (ellipsis)
			return "const2u";
		else
			return "DW_OP_const2u";
	case DW_OP_const2s:
		if (ellipsis)
			return "const2s";
		else
			return "DW_OP_const2s";
	case DW_OP_const4u:
		if (ellipsis)
			return "const4u";
		else
			return "DW_OP_const4u";
	case DW_OP_const4s:
		if (ellipsis)
			return "const4s";
		else
			return "DW_OP_const4s";
	case DW_OP_const8u:
		if (ellipsis)
			return "const8u";
		else
			return "DW_OP_const8u";
	case DW_OP_const8s:
		if (ellipsis)
			return "const8s";
		else
			return "DW_OP_const8s";
	case DW_OP_constu:
		if (ellipsis)
			return "constu";
		else
			return "DW_OP_constu";
	case DW_OP_consts:
		if (ellipsis)
			return "consts";
		else
			return "DW_OP_consts";
	case DW_OP_dup:
		if (ellipsis)
			return "dup";
		else
			return "DW_OP_dup";
	case DW_OP_drop:
		if (ellipsis)
			return "drop";
		else
			return "DW_OP_drop";
	case DW_OP_over:
		if (ellipsis)
			return "over";
		else
			return "DW_OP_over";
	case DW_OP_pick:
		if (ellipsis)
			return "pick";
		else
			return "DW_OP_pick";
	case DW_OP_swap:
		if (ellipsis)
			return "swap";
		else
			return "DW_OP_swap";
	case DW_OP_rot:
		if (ellipsis)
			return "rot";
		else
			return "DW_OP_rot";
	case DW_OP_xderef:
		if (ellipsis)
			return "xderef";
		else
			return "DW_OP_xderef";
	case DW_OP_abs:
		if (ellipsis)
			return "abs";
		else
			return "DW_OP_abs";
	case DW_OP_and:
		if (ellipsis)
			return "and";
		else
			return "DW_OP_and";
	case DW_OP_div:
		if (ellipsis)
			return "div";
		else
			return "DW_OP_div";
	case DW_OP_minus:
		if (ellipsis)
			return "minus";
		else
			return "DW_OP_minus";
	case DW_OP_mod:
		if (ellipsis)
			return "mod";
		else
			return "DW_OP_mod";
	case DW_OP_mul:
		if (ellipsis)
			return "mul";
		else
			return "DW_OP_mul";
	case DW_OP_neg:
		if (ellipsis)
			return "neg";
		else
			return "DW_OP_neg";
	case DW_OP_not:
		if (ellipsis)
			return "not";
		else
			return "DW_OP_not";
	case DW_OP_or:
		if (ellipsis)
			return "or";
		else
			return "DW_OP_or";
	case DW_OP_plus:
		if (ellipsis)
			return "plus";
		else
			return "DW_OP_plus";
	case DW_OP_plus_uconst:
		if (ellipsis)
			return "plus_uconst";
		else
			return "DW_OP_plus_uconst";
	case DW_OP_shl:
		if (ellipsis)
			return "shl";
		else
			return "DW_OP_shl";
	case DW_OP_shr:
		if (ellipsis)
			return "shr";
		else
			return "DW_OP_shr";
	case DW_OP_shra:
		if (ellipsis)
			return "shra";
		else
			return "DW_OP_shra";
	case DW_OP_xor:
		if (ellipsis)
			return "xor";
		else
			return "DW_OP_xor";
	case DW_OP_bra:
		if (ellipsis)
			return "bra";
		else
			return "DW_OP_bra";
	case DW_OP_eq:
		if (ellipsis)
			return "eq";
		else
			return "DW_OP_eq";
	case DW_OP_ge:
		if (ellipsis)
			return "ge";
		else
			return "DW_OP_ge";
	case DW_OP_gt:
		if (ellipsis)
			return "gt";
		else
			return "DW_OP_gt";
	case DW_OP_le:
		if (ellipsis)
			return "le";
		else
			return "DW_OP_le";
	case DW_OP_lt:
		if (ellipsis)
			return "lt";
		else
			return "DW_OP_lt";
	case DW_OP_ne:
		if (ellipsis)
			return "ne";
		else
			return "DW_OP_ne";
	case DW_OP_skip:
		if (ellipsis)
			return "skip";
		else
			return "DW_OP_skip";
	case DW_OP_lit0:
		if (ellipsis)
			return "lit0";
		else
			return "DW_OP_lit0";
	case DW_OP_lit1:
		if (ellipsis)
			return "lit1";
		else
			return "DW_OP_lit1";
	case DW_OP_lit2:
		if (ellipsis)
			return "lit2";
		else
			return "DW_OP_lit2";
	case DW_OP_lit3:
		if (ellipsis)
			return "lit3";
		else
			return "DW_OP_lit3";
	case DW_OP_lit4:
		if (ellipsis)
			return "lit4";
		else
			return "DW_OP_lit4";
	case DW_OP_lit5:
		if (ellipsis)
			return "lit5";
		else
			return "DW_OP_lit5";
	case DW_OP_lit6:
		if (ellipsis)
			return "lit6";
		else
			return "DW_OP_lit6";
	case DW_OP_lit7:
		if (ellipsis)
			return "lit7";
		else
			return "DW_OP_lit7";
	case DW_OP_lit8:
		if (ellipsis)
			return "lit8";
		else
			return "DW_OP_lit8";
	case DW_OP_lit9:
		if (ellipsis)
			return "lit9";
		else
			return "DW_OP_lit9";
	case DW_OP_lit10:
		if (ellipsis)
			return "lit10";
		else
			return "DW_OP_lit10";
	case DW_OP_lit11:
		if (ellipsis)
			return "lit11";
		else
			return "DW_OP_lit11";
	case DW_OP_lit12:
		if (ellipsis)
			return "lit12";
		else
			return "DW_OP_lit12";
	case DW_OP_lit13:
		if (ellipsis)
			return "lit13";
		else
			return "DW_OP_lit13";
	case DW_OP_lit14:
		if (ellipsis)
			return "lit14";
		else
			return "DW_OP_lit14";
	case DW_OP_lit15:
		if (ellipsis)
			return "lit15";
		else
			return "DW_OP_lit15";
	case DW_OP_lit16:
		if (ellipsis)
			return "lit16";
		else
			return "DW_OP_lit16";
	case DW_OP_lit17:
		if (ellipsis)
			return "lit17";
		else
			return "DW_OP_lit17";
	case DW_OP_lit18:
		if (ellipsis)
			return "lit18";
		else
			return "DW_OP_lit18";
	case DW_OP_lit19:
		if (ellipsis)
			return "lit19";
		else
			return "DW_OP_lit19";
	case DW_OP_lit20:
		if (ellipsis)
			return "lit20";
		else
			return "DW_OP_lit20";
	case DW_OP_lit21:
		if (ellipsis)
			return "lit21";
		else
			return "DW_OP_lit21";
	case DW_OP_lit22:
		if (ellipsis)
			return "lit22";
		else
			return "DW_OP_lit22";
	case DW_OP_lit23:
		if (ellipsis)
			return "lit23";
		else
			return "DW_OP_lit23";
	case DW_OP_lit24:
		if (ellipsis)
			return "lit24";
		else
			return "DW_OP_lit24";
	case DW_OP_lit25:
		if (ellipsis)
			return "lit25";
		else
			return "DW_OP_lit25";
	case DW_OP_lit26:
		if (ellipsis)
			return "lit26";
		else
			return "DW_OP_lit26";
	case DW_OP_lit27:
		if (ellipsis)
			return "lit27";
		else
			return "DW_OP_lit27";
	case DW_OP_lit28:
		if (ellipsis)
			return "lit28";
		else
			return "DW_OP_lit28";
	case DW_OP_lit29:
		if (ellipsis)
			return "lit29";
		else
			return "DW_OP_lit29";
	case DW_OP_lit30:
		if (ellipsis)
			return "lit30";
		else
			return "DW_OP_lit30";
	case DW_OP_lit31:
		if (ellipsis)
			return "lit31";
		else
			return "DW_OP_lit31";
	case DW_OP_reg0:
		if (ellipsis)
			return "reg0";
		else
			return "DW_OP_reg0";
	case DW_OP_reg1:
		if (ellipsis)
			return "reg1";
		else
			return "DW_OP_reg1";
	case DW_OP_reg2:
		if (ellipsis)
			return "reg2";
		else
			return "DW_OP_reg2";
	case DW_OP_reg3:
		if (ellipsis)
			return "reg3";
		else
			return "DW_OP_reg3";
	case DW_OP_reg4:
		if (ellipsis)
			return "reg4";
		else
			return "DW_OP_reg4";
	case DW_OP_reg5:
		if (ellipsis)
			return "reg5";
		else
			return "DW_OP_reg5";
	case DW_OP_reg6:
		if (ellipsis)
			return "reg6";
		else
			return "DW_OP_reg6";
	case DW_OP_reg7:
		if (ellipsis)
			return "reg7";
		else
			return "DW_OP_reg7";
	case DW_OP_reg8:
		if (ellipsis)
			return "reg8";
		else
			return "DW_OP_reg8";
	case DW_OP_reg9:
		if (ellipsis)
			return "reg9";
		else
			return "DW_OP_reg9";
	case DW_OP_reg10:
		if (ellipsis)
			return "reg10";
		else
			return "DW_OP_reg10";
	case DW_OP_reg11:
		if (ellipsis)
			return "reg11";
		else
			return "DW_OP_reg11";
	case DW_OP_reg12:
		if (ellipsis)
			return "reg12";
		else
			return "DW_OP_reg12";
	case DW_OP_reg13:
		if (ellipsis)
			return "reg13";
		else
			return "DW_OP_reg13";
	case DW_OP_reg14:
		if (ellipsis)
			return "reg14";
		else
			return "DW_OP_reg14";
	case DW_OP_reg15:
		if (ellipsis)
			return "reg15";
		else
			return "DW_OP_reg15";
	case DW_OP_reg16:
		if (ellipsis)
			return "reg16";
		else
			return "DW_OP_reg16";
	case DW_OP_reg17:
		if (ellipsis)
			return "reg17";
		else
			return "DW_OP_reg17";
	case DW_OP_reg18:
		if (ellipsis)
			return "reg18";
		else
			return "DW_OP_reg18";
	case DW_OP_reg19:
		if (ellipsis)
			return "reg19";
		else
			return "DW_OP_reg19";
	case DW_OP_reg20:
		if (ellipsis)
			return "reg20";
		else
			return "DW_OP_reg20";
	case DW_OP_reg21:
		if (ellipsis)
			return "reg21";
		else
			return "DW_OP_reg21";
	case DW_OP_reg22:
		if (ellipsis)
			return "reg22";
		else
			return "DW_OP_reg22";
	case DW_OP_reg23:
		if (ellipsis)
			return "reg23";
		else
			return "DW_OP_reg23";
	case DW_OP_reg24:
		if (ellipsis)
			return "reg24";
		else
			return "DW_OP_reg24";
	case DW_OP_reg25:
		if (ellipsis)
			return "reg25";
		else
			return "DW_OP_reg25";
	case DW_OP_reg26:
		if (ellipsis)
			return "reg26";
		else
			return "DW_OP_reg26";
	case DW_OP_reg27:
		if (ellipsis)
			return "reg27";
		else
			return "DW_OP_reg27";
	case DW_OP_reg28:
		if (ellipsis)
			return "reg28";
		else
			return "DW_OP_reg28";
	case DW_OP_reg29:
		if (ellipsis)
			return "reg29";
		else
			return "DW_OP_reg29";
	case DW_OP_reg30:
		if (ellipsis)
			return "reg30";
		else
			return "DW_OP_reg30";
	case DW_OP_reg31:
		if (ellipsis)
			return "reg31";
		else
			return "DW_OP_reg31";
	case DW_OP_breg0:
		if (ellipsis)
			return "breg0";
		else
			return "DW_OP_breg0";
	case DW_OP_breg1:
		if (ellipsis)
			return "breg1";
		else
			return "DW_OP_breg1";
	case DW_OP_breg2:
		if (ellipsis)
			return "breg2";
		else
			return "DW_OP_breg2";
	case DW_OP_breg3:
		if (ellipsis)
			return "breg3";
		else
			return "DW_OP_breg3";
	case DW_OP_breg4:
		if (ellipsis)
			return "breg4";
		else
			return "DW_OP_breg4";
	case DW_OP_breg5:
		if (ellipsis)
			return "breg5";
		else
			return "DW_OP_breg5";
	case DW_OP_breg6:
		if (ellipsis)
			return "breg6";
		else
			return "DW_OP_breg6";
	case DW_OP_breg7:
		if (ellipsis)
			return "breg7";
		else
			return "DW_OP_breg7";
	case DW_OP_breg8:
		if (ellipsis)
			return "breg8";
		else
			return "DW_OP_breg8";
	case DW_OP_breg9:
		if (ellipsis)
			return "breg9";
		else
			return "DW_OP_breg9";
	case DW_OP_breg10:
		if (ellipsis)
			return "breg10";
		else
			return "DW_OP_breg10";
	case DW_OP_breg11:
		if (ellipsis)
			return "breg11";
		else
			return "DW_OP_breg11";
	case DW_OP_breg12:
		if (ellipsis)
			return "breg12";
		else
			return "DW_OP_breg12";
	case DW_OP_breg13:
		if (ellipsis)
			return "breg13";
		else
			return "DW_OP_breg13";
	case DW_OP_breg14:
		if (ellipsis)
			return "breg14";
		else
			return "DW_OP_breg14";
	case DW_OP_breg15:
		if (ellipsis)
			return "breg15";
		else
			return "DW_OP_breg15";
	case DW_OP_breg16:
		if (ellipsis)
			return "breg16";
		else
			return "DW_OP_breg16";
	case DW_OP_breg17:
		if (ellipsis)
			return "breg17";
		else
			return "DW_OP_breg17";
	case DW_OP_breg18:
		if (ellipsis)
			return "breg18";
		else
			return "DW_OP_breg18";
	case DW_OP_breg19:
		if (ellipsis)
			return "breg19";
		else
			return "DW_OP_breg19";
	case DW_OP_breg20:
		if (ellipsis)
			return "breg20";
		else
			return "DW_OP_breg20";
	case DW_OP_breg21:
		if (ellipsis)
			return "breg21";
		else
			return "DW_OP_breg21";
	case DW_OP_breg22:
		if (ellipsis)
			return "breg22";
		else
			return "DW_OP_breg22";
	case DW_OP_breg23:
		if (ellipsis)
			return "breg23";
		else
			return "DW_OP_breg23";
	case DW_OP_breg24:
		if (ellipsis)
			return "breg24";
		else
			return "DW_OP_breg24";
	case DW_OP_breg25:
		if (ellipsis)
			return "breg25";
		else
			return "DW_OP_breg25";
	case DW_OP_breg26:
		if (ellipsis)
			return "breg26";
		else
			return "DW_OP_breg26";
	case DW_OP_breg27:
		if (ellipsis)
			return "breg27";
		else
			return "DW_OP_breg27";
	case DW_OP_breg28:
		if (ellipsis)
			return "breg28";
		else
			return "DW_OP_breg28";
	case DW_OP_breg29:
		if (ellipsis)
			return "breg29";
		else
			return "DW_OP_breg29";
	case DW_OP_breg30:
		if (ellipsis)
			return "breg30";
		else
			return "DW_OP_breg30";
	case DW_OP_breg31:
		if (ellipsis)
			return "breg31";
		else
			return "DW_OP_breg31";
	case DW_OP_regx:
		if (ellipsis)
			return "regx";
		else
			return "DW_OP_regx";
	case DW_OP_fbreg:
		if (ellipsis)
			return "fbreg";
		else
			return "DW_OP_fbreg";
	case DW_OP_bregx:
		if (ellipsis)
			return "bregx";
		else
			return "DW_OP_bregx";
	case DW_OP_piece:
		if (ellipsis)
			return "piece";
		else
			return "DW_OP_piece";
	case DW_OP_deref_size:
		if (ellipsis)
			return "deref_size";
		else
			return "DW_OP_deref_size";
	case DW_OP_xderef_size:
		if (ellipsis)
			return "xderef_size";
		else
			return "DW_OP_xderef_size";
	case DW_OP_nop:
		if (ellipsis)
			return "nop";
		else
			return "DW_OP_nop";
	case DW_OP_push_object_address:
		if (ellipsis)
			return "push_object_address";
		else
			return "DW_OP_push_object_address";
	case DW_OP_call2:
		if (ellipsis)
			return "call2";
		else
			return "DW_OP_call2";
	case DW_OP_call4:
		if (ellipsis)
			return "call4";
		else
			return "DW_OP_call4";
	case DW_OP_call_ref:
		if (ellipsis)
			return "call_ref";
		else
			return "DW_OP_call_ref";
	case DW_OP_form_tls_address:
		if (ellipsis)
			return "form_tls_address";
		else
			return "DW_OP_form_tls_address";
	case DW_OP_call_frame_cfa:
		if (ellipsis)
			return "call_frame_cfa";
		else
			return "DW_OP_call_frame_cfa";
	case DW_OP_bit_piece:
		if (ellipsis)
			return "bit_piece";
		else
			return "DW_OP_bit_piece";
	case DW_OP_GNU_push_tls_address:
		if (ellipsis)
			return "GNU_push_tls_address";
		else
			return "DW_OP_GNU_push_tls_address";
	case DW_OP_HP_is_value:
		if (ellipsis)
			return "HP_is_value";
		else
			return "DW_OP_HP_is_value";
	case DW_OP_HP_fltconst4:
		if (ellipsis)
			return "HP_fltconst4";
		else
			return "DW_OP_HP_fltconst4";
	case DW_OP_HP_fltconst8:
		if (ellipsis)
			return "HP_fltconst8";
		else
			return "DW_OP_HP_fltconst8";
	case DW_OP_HP_mod_range:
		if (ellipsis)
			return "HP_mod_range";
		else
			return "DW_OP_HP_mod_range";
	case DW_OP_HP_unmod_range:
		if (ellipsis)
			return "HP_unmod_range";
		else
			return "DW_OP_HP_unmod_range";
	case DW_OP_HP_tls:
		if (ellipsis)
			return "HP_tls";
		else
			return "DW_OP_HP_tls";
	case DW_OP_hi_user:
		if (ellipsis)
			return "hi_user";
		else
			return "DW_OP_hi_user";
	default:
		{ 
		    char buf[100]; 
		    char *n; 
		    snprintf(buf,sizeof(buf),"<Unknown OP value 0x%x>",(int)val);
		 fprintf(stderr,"OP of %d (0x%x) is unknown to dwarfdump. " 
 		 "Continuing. \n",(int)val,(int)val );  
		    n = makename(buf);
		    return n; 
		} 
	}
/*NOTREACHED*/
}

/* ARGSUSED */
extern string
get_ATE_name (Dwarf_Debug dbg, Dwarf_Half val)
{
	switch (val) {
	case DW_ATE_address:
		if (ellipsis)
			return "address";
		else
			return "DW_ATE_address";
	case DW_ATE_boolean:
		if (ellipsis)
			return "boolean";
		else
			return "DW_ATE_boolean";
	case DW_ATE_complex_float:
		if (ellipsis)
			return "complex_float";
		else
			return "DW_ATE_complex_float";
	case DW_ATE_float:
		if (ellipsis)
			return "float";
		else
			return "DW_ATE_float";
	case DW_ATE_signed:
		if (ellipsis)
			return "signed";
		else
			return "DW_ATE_signed";
	case DW_ATE_signed_char:
		if (ellipsis)
			return "signed_char";
		else
			return "DW_ATE_signed_char";
	case DW_ATE_unsigned:
		if (ellipsis)
			return "unsigned";
		else
			return "DW_ATE_unsigned";
	case DW_ATE_unsigned_char:
		if (ellipsis)
			return "unsigned_char";
		else
			return "DW_ATE_unsigned_char";
	case DW_ATE_imaginary_float:
		if (ellipsis)
			return "imaginary_float";
		else
			return "DW_ATE_imaginary_float";
	case DW_ATE_packed_decimal:
		if (ellipsis)
			return "packed_decimal";
		else
			return "DW_ATE_packed_decimal";
	case DW_ATE_numeric_string:
		if (ellipsis)
			return "numeric_string";
		else
			return "DW_ATE_numeric_string";
	case DW_ATE_edited:
		if (ellipsis)
			return "edited";
		else
			return "DW_ATE_edited";
	case DW_ATE_signed_fixed:
		if (ellipsis)
			return "signed_fixed";
		else
			return "DW_ATE_signed_fixed";
	case DW_ATE_unsigned_fixed:
		if (ellipsis)
			return "unsigned_fixed";
		else
			return "DW_ATE_unsigned_fixed";
	case DW_ATE_decimal_float:
		if (ellipsis)
			return "decimal_float";
		else
			return "DW_ATE_decimal_float";
	case DW_ATE_ALTIUM_fract:
		if (ellipsis)
			return "ALTIUM_fract";
		else
			return "DW_ATE_ALTIUM_fract";
	case DW_ATE_ALTIUM_accum:
		if (ellipsis)
			return "ALTIUM_accum";
		else
			return "DW_ATE_ALTIUM_accum";
	case DW_ATE_HP_float128:
		if (ellipsis)
			return "HP_float128";
		else
			return "DW_ATE_HP_float128";
	case DW_ATE_HP_complex_float128:
		if (ellipsis)
			return "HP_complex_float128";
		else
			return "DW_ATE_HP_complex_float128";
	case DW_ATE_HP_floathpintel:
		if (ellipsis)
			return "HP_floathpintel";
		else
			return "DW_ATE_HP_floathpintel";
	case DW_ATE_HP_imaginary_float80:
		if (ellipsis)
			return "HP_imaginary_float80";
		else
			return "DW_ATE_HP_imaginary_float80";
	case DW_ATE_HP_imaginary_float128:
		if (ellipsis)
			return "HP_imaginary_float128";
		else
			return "DW_ATE_HP_imaginary_float128";
	case DW_ATE_SUN_interval_float:
		if (ellipsis)
			return "SUN_interval_float";
		else
			return "DW_ATE_SUN_interval_float";
	case DW_ATE_SUN_imaginary_float:
		if (ellipsis)
			return "SUN_imaginary_float";
		else
			return "DW_ATE_SUN_imaginary_float";
	case DW_ATE_hi_user:
		if (ellipsis)
			return "hi_user";
		else
			return "DW_ATE_hi_user";
	default:
		{ 
		    char buf[100]; 
		    char *n; 
		    snprintf(buf,sizeof(buf),"<Unknown ATE value 0x%x>",(int)val);
		 fprintf(stderr,"ATE of %d (0x%x) is unknown to dwarfdump. " 
 		 "Continuing. \n",(int)val,(int)val );  
		    n = makename(buf);
		    return n; 
		} 
	}
/*NOTREACHED*/
}

/* ARGSUSED */
extern string
get_DS_name (Dwarf_Debug dbg, Dwarf_Half val)
{
	switch (val) {
	case DW_DS_unsigned:
		if (ellipsis)
			return "unsigned";
		else
			return "DW_DS_unsigned";
	case DW_DS_leading_overpunch:
		if (ellipsis)
			return "leading_overpunch";
		else
			return "DW_DS_leading_overpunch";
	case DW_DS_trailing_overpunch:
		if (ellipsis)
			return "trailing_overpunch";
		else
			return "DW_DS_trailing_overpunch";
	case DW_DS_leading_separate:
		if (ellipsis)
			return "leading_separate";
		else
			return "DW_DS_leading_separate";
	case DW_DS_trailing_separate:
		if (ellipsis)
			return "trailing_separate";
		else
			return "DW_DS_trailing_separate";
	default:
		{ 
		    char buf[100]; 
		    char *n; 
		    snprintf(buf,sizeof(buf),"<Unknown DS value 0x%x>",(int)val);
		 fprintf(stderr,"DS of %d (0x%x) is unknown to dwarfdump. " 
 		 "Continuing. \n",(int)val,(int)val );  
		    n = makename(buf);
		    return n; 
		} 
	}
/*NOTREACHED*/
}

/* ARGSUSED */
extern string
get_END_name (Dwarf_Debug dbg, Dwarf_Half val)
{
	switch (val) {
	case DW_END_default:
		if (ellipsis)
			return "default";
		else
			return "DW_END_default";
	case DW_END_big:
		if (ellipsis)
			return "big";
		else
			return "DW_END_big";
	case DW_END_little:
		if (ellipsis)
			return "little";
		else
			return "DW_END_little";
	case DW_END_lo_user:
		if (ellipsis)
			return "lo_user";
		else
			return "DW_END_lo_user";
	case DW_END_hi_user:
		if (ellipsis)
			return "hi_user";
		else
			return "DW_END_hi_user";
	default:
		{ 
		    char buf[100]; 
		    char *n; 
		    snprintf(buf,sizeof(buf),"<Unknown END value 0x%x>",(int)val);
		 fprintf(stderr,"END of %d (0x%x) is unknown to dwarfdump. " 
 		 "Continuing. \n",(int)val,(int)val );  
		    n = makename(buf);
		    return n; 
		} 
	}
/*NOTREACHED*/
}

/* ARGSUSED */
extern string
get_ATCF_name (Dwarf_Debug dbg, Dwarf_Half val)
{
	switch (val) {
	case DW_ATCF_lo_user:
		if (ellipsis)
			return "lo_user";
		else
			return "DW_ATCF_lo_user";
	case DW_ATCF_SUN_mop_bitfield:
		if (ellipsis)
			return "SUN_mop_bitfield";
		else
			return "DW_ATCF_SUN_mop_bitfield";
	case DW_ATCF_SUN_mop_spill:
		if (ellipsis)
			return "SUN_mop_spill";
		else
			return "DW_ATCF_SUN_mop_spill";
	case DW_ATCF_SUN_mop_scopy:
		if (ellipsis)
			return "SUN_mop_scopy";
		else
			return "DW_ATCF_SUN_mop_scopy";
	case DW_ATCF_SUN_func_start:
		if (ellipsis)
			return "SUN_func_start";
		else
			return "DW_ATCF_SUN_func_start";
	case DW_ATCF_SUN_end_ctors:
		if (ellipsis)
			return "SUN_end_ctors";
		else
			return "DW_ATCF_SUN_end_ctors";
	case DW_ATCF_SUN_branch_target:
		if (ellipsis)
			return "SUN_branch_target";
		else
			return "DW_ATCF_SUN_branch_target";
	case DW_ATCF_SUN_mop_stack_probe:
		if (ellipsis)
			return "SUN_mop_stack_probe";
		else
			return "DW_ATCF_SUN_mop_stack_probe";
	case DW_ATCF_SUN_func_epilog:
		if (ellipsis)
			return "SUN_func_epilog";
		else
			return "DW_ATCF_SUN_func_epilog";
	case DW_ATCF_hi_user:
		if (ellipsis)
			return "hi_user";
		else
			return "DW_ATCF_hi_user";
	default:
		{ 
		    char buf[100]; 
		    char *n; 
		    snprintf(buf,sizeof(buf),"<Unknown ATCF value 0x%x>",(int)val);
		 fprintf(stderr,"ATCF of %d (0x%x) is unknown to dwarfdump. " 
 		 "Continuing. \n",(int)val,(int)val );  
		    n = makename(buf);
		    return n; 
		} 
	}
/*NOTREACHED*/
}

/* ARGSUSED */
extern string
get_ACCESS_name (Dwarf_Debug dbg, Dwarf_Half val)
{
	switch (val) {
	case DW_ACCESS_public:
		if (ellipsis)
			return "public";
		else
			return "DW_ACCESS_public";
	case DW_ACCESS_protected:
		if (ellipsis)
			return "protected";
		else
			return "DW_ACCESS_protected";
	case DW_ACCESS_private:
		if (ellipsis)
			return "private";
		else
			return "DW_ACCESS_private";
	default:
		{ 
		    char buf[100]; 
		    char *n; 
		    snprintf(buf,sizeof(buf),"<Unknown ACCESS value 0x%x>",(int)val);
		 fprintf(stderr,"ACCESS of %d (0x%x) is unknown to dwarfdump. " 
 		 "Continuing. \n",(int)val,(int)val );  
		    n = makename(buf);
		    return n; 
		} 
	}
/*NOTREACHED*/
}

/* ARGSUSED */
extern string
get_VIS_name (Dwarf_Debug dbg, Dwarf_Half val)
{
	switch (val) {
	case DW_VIS_local:
		if (ellipsis)
			return "local";
		else
			return "DW_VIS_local";
	case DW_VIS_exported:
		if (ellipsis)
			return "exported";
		else
			return "DW_VIS_exported";
	case DW_VIS_qualified:
		if (ellipsis)
			return "qualified";
		else
			return "DW_VIS_qualified";
	default:
		{ 
		    char buf[100]; 
		    char *n; 
		    snprintf(buf,sizeof(buf),"<Unknown VIS value 0x%x>",(int)val);
		 fprintf(stderr,"VIS of %d (0x%x) is unknown to dwarfdump. " 
 		 "Continuing. \n",(int)val,(int)val );  
		    n = makename(buf);
		    return n; 
		} 
	}
/*NOTREACHED*/
}

/* ARGSUSED */
extern string
get_VIRTUALITY_name (Dwarf_Debug dbg, Dwarf_Half val)
{
	switch (val) {
	case DW_VIRTUALITY_none:
		if (ellipsis)
			return "none";
		else
			return "DW_VIRTUALITY_none";
	case DW_VIRTUALITY_virtual:
		if (ellipsis)
			return "virtual";
		else
			return "DW_VIRTUALITY_virtual";
	case DW_VIRTUALITY_pure_virtual:
		if (ellipsis)
			return "pure_virtual";
		else
			return "DW_VIRTUALITY_pure_virtual";
	default:
		{ 
		    char buf[100]; 
		    char *n; 
		    snprintf(buf,sizeof(buf),"<Unknown VIRTUALITY value 0x%x>",(int)val);
		 fprintf(stderr,"VIRTUALITY of %d (0x%x) is unknown to dwarfdump. " 
 		 "Continuing. \n",(int)val,(int)val );  
		    n = makename(buf);
		    return n; 
		} 
	}
/*NOTREACHED*/
}
extern string
get_ID_name (Dwarf_Debug dbg, Dwarf_Half val)
{
	switch (val) {
	case DW_ID_case_sensitive:
		if (ellipsis)
			return "case_sensitive";
		else
			return "DW_ID_case_sensitive";
	case DW_ID_up_case:
		if (ellipsis)
			return "up_case";
		else
			return "DW_ID_up_case";
	case DW_ID_down_case:
		if (ellipsis)
			return "down_case";
		else
			return "DW_ID_down_case";
	case DW_ID_case_insensitive:
		if (ellipsis)
			return "case_insensitive";
		else
			return "DW_ID_case_insensitive";
	default:
		{ 
		    char buf[100]; 
		    char *n; 
		    snprintf(buf,sizeof(buf),"<Unknown ID value 0x%x>",(int)val);
		 fprintf(stderr,"ID of %d (0x%x) is unknown to dwarfdump. " 
 		 "Continuing. \n",(int)val,(int)val );  
		    n = makename(buf);
		    return n; 
		} 
	}
/*NOTREACHED*/
}

/* ARGSUSED */
extern string
get_CC_name (Dwarf_Debug dbg, Dwarf_Half val)
{
	switch (val) {
	case DW_CC_normal:
		if (ellipsis)
			return "normal";
		else
			return "DW_CC_normal";
	case DW_CC_program:
		if (ellipsis)
			return "program";
		else
			return "DW_CC_program";
	case DW_CC_nocall:
		if (ellipsis)
			return "nocall";
		else
			return "DW_CC_nocall";
	case DW_CC_lo_user:
		if (ellipsis)
			return "lo_user";
		else
			return "DW_CC_lo_user";
	case DW_CC_ALTIUM_interrupt:
		if (ellipsis)
			return "ALTIUM_interrupt";
		else
			return "DW_CC_ALTIUM_interrupt";
	case DW_CC_ALTIUM_near_system_stack:
		if (ellipsis)
			return "ALTIUM_near_system_stack";
		else
			return "DW_CC_ALTIUM_near_system_stack";
	case DW_CC_ALTIUM_near_user_stack:
		if (ellipsis)
			return "ALTIUM_near_user_stack";
		else
			return "DW_CC_ALTIUM_near_user_stack";
	case DW_CC_ALTIUM_huge_user_stack:
		if (ellipsis)
			return "ALTIUM_huge_user_stack";
		else
			return "DW_CC_ALTIUM_huge_user_stack";
	case DW_CC_hi_user:
		if (ellipsis)
			return "hi_user";
		else
			return "DW_CC_hi_user";
	default:
		{ 
		    char buf[100]; 
		    char *n; 
		    snprintf(buf,sizeof(buf),"<Unknown CC value 0x%x>",(int)val);
		 fprintf(stderr,"CC of %d (0x%x) is unknown to dwarfdump. " 
 		 "Continuing. \n",(int)val,(int)val );  
		    n = makename(buf);
		    return n; 
		} 
	}
/*NOTREACHED*/
}

/* ARGSUSED */
extern string
get_INL_name (Dwarf_Debug dbg, Dwarf_Half val)
{
	switch (val) {
	case DW_INL_not_inlined:
		if (ellipsis)
			return "not_inlined";
		else
			return "DW_INL_not_inlined";
	case DW_INL_inlined:
		if (ellipsis)
			return "inlined";
		else
			return "DW_INL_inlined";
	case DW_INL_declared_not_inlined:
		if (ellipsis)
			return "declared_not_inlined";
		else
			return "DW_INL_declared_not_inlined";
	case DW_INL_declared_inlined:
		if (ellipsis)
			return "declared_inlined";
		else
			return "DW_INL_declared_inlined";
	default:
		{ 
		    char buf[100]; 
		    char *n; 
		    snprintf(buf,sizeof(buf),"<Unknown INL value 0x%x>",(int)val);
		 fprintf(stderr,"INL of %d (0x%x) is unknown to dwarfdump. " 
 		 "Continuing. \n",(int)val,(int)val );  
		    n = makename(buf);
		    return n; 
		} 
	}
/*NOTREACHED*/
}

/* ARGSUSED */
extern string
get_ORD_name (Dwarf_Debug dbg, Dwarf_Half val)
{
	switch (val) {
	case DW_ORD_row_major:
		if (ellipsis)
			return "row_major";
		else
			return "DW_ORD_row_major";
	case DW_ORD_col_major:
		if (ellipsis)
			return "col_major";
		else
			return "DW_ORD_col_major";
	default:
		{ 
		    char buf[100]; 
		    char *n; 
		    snprintf(buf,sizeof(buf),"<Unknown ORD value 0x%x>",(int)val);
		 fprintf(stderr,"ORD of %d (0x%x) is unknown to dwarfdump. " 
 		 "Continuing. \n",(int)val,(int)val );  
		    n = makename(buf);
		    return n; 
		} 
	}
/*NOTREACHED*/
}

/* ARGSUSED */
extern string
get_DSC_name (Dwarf_Debug dbg, Dwarf_Half val)
{
	switch (val) {
	case DW_DSC_label:
		if (ellipsis)
			return "label";
		else
			return "DW_DSC_label";
	case DW_DSC_range:
		if (ellipsis)
			return "range";
		else
			return "DW_DSC_range";
	default:
		{ 
		    char buf[100]; 
		    char *n; 
		    snprintf(buf,sizeof(buf),"<Unknown DSC value 0x%x>",(int)val);
		 fprintf(stderr,"DSC of %d (0x%x) is unknown to dwarfdump. " 
 		 "Continuing. \n",(int)val,(int)val );  
		    n = makename(buf);
		    return n; 
		} 
	}
/*NOTREACHED*/
}

/* ARGSUSED */
extern string
get_LNS_name (Dwarf_Debug dbg, Dwarf_Half val)
{
	switch (val) {
	case DW_LNS_copy:
		if (ellipsis)
			return "copy";
		else
			return "DW_LNS_copy";
	case DW_LNS_advance_pc:
		if (ellipsis)
			return "advance_pc";
		else
			return "DW_LNS_advance_pc";
	case DW_LNS_advance_line:
		if (ellipsis)
			return "advance_line";
		else
			return "DW_LNS_advance_line";
	case DW_LNS_set_file:
		if (ellipsis)
			return "set_file";
		else
			return "DW_LNS_set_file";
	case DW_LNS_set_column:
		if (ellipsis)
			return "set_column";
		else
			return "DW_LNS_set_column";
	case DW_LNS_negate_stmt:
		if (ellipsis)
			return "negate_stmt";
		else
			return "DW_LNS_negate_stmt";
	case DW_LNS_set_basic_block:
		if (ellipsis)
			return "set_basic_block";
		else
			return "DW_LNS_set_basic_block";
	case DW_LNS_const_add_pc:
		if (ellipsis)
			return "const_add_pc";
		else
			return "DW_LNS_const_add_pc";
	case DW_LNS_fixed_advance_pc:
		if (ellipsis)
			return "fixed_advance_pc";
		else
			return "DW_LNS_fixed_advance_pc";
	case DW_LNS_set_prologue_end:
		if (ellipsis)
			return "set_prologue_end";
		else
			return "DW_LNS_set_prologue_end";
	case DW_LNS_set_epilogue_begin:
		if (ellipsis)
			return "set_epilogue_begin";
		else
			return "DW_LNS_set_epilogue_begin";
	case DW_LNS_set_isa:
		if (ellipsis)
			return "set_isa";
		else
			return "DW_LNS_set_isa";
	default:
		{ 
		    char buf[100]; 
		    char *n; 
		    snprintf(buf,sizeof(buf),"<Unknown LNS value 0x%x>",(int)val);
		 fprintf(stderr,"LNS of %d (0x%x) is unknown to dwarfdump. " 
 		 "Continuing. \n",(int)val,(int)val );  
		    n = makename(buf);
		    return n; 
		} 
	}
/*NOTREACHED*/
}

/* ARGSUSED */
extern string
get_LNE_name (Dwarf_Debug dbg, Dwarf_Half val)
{
	switch (val) {
	case DW_LNE_end_sequence:
		if (ellipsis)
			return "end_sequence";
		else
			return "DW_LNE_end_sequence";
	case DW_LNE_set_address:
		if (ellipsis)
			return "set_address";
		else
			return "DW_LNE_set_address";
	case DW_LNE_define_file:
		if (ellipsis)
			return "define_file";
		else
			return "DW_LNE_define_file";
	case DW_LNE_HP_negate_is_UV_update:
		if (ellipsis)
			return "HP_negate_is_UV_update";
		else
			return "DW_LNE_HP_negate_is_UV_update";
	case DW_LNE_HP_push_context:
		if (ellipsis)
			return "HP_push_context";
		else
			return "DW_LNE_HP_push_context";
	case DW_LNE_HP_pop_context:
		if (ellipsis)
			return "HP_pop_context";
		else
			return "DW_LNE_HP_pop_context";
	case DW_LNE_HP_set_file_line_column:
		if (ellipsis)
			return "HP_set_file_line_column";
		else
			return "DW_LNE_HP_set_file_line_column";
	case DW_LNE_HP_set_routine_name:
		if (ellipsis)
			return "HP_set_routine_name";
		else
			return "DW_LNE_HP_set_routine_name";
	case DW_LNE_HP_set_sequence:
		if (ellipsis)
			return "HP_set_sequence";
		else
			return "DW_LNE_HP_set_sequence";
	case DW_LNE_HP_negate_post_semantics:
		if (ellipsis)
			return "HP_negate_post_semantics";
		else
			return "DW_LNE_HP_negate_post_semantics";
	case DW_LNE_HP_negate_function_exit:
		if (ellipsis)
			return "HP_negate_function_exit";
		else
			return "DW_LNE_HP_negate_function_exit";
	case DW_LNE_HP_negate_front_end_logical:
		if (ellipsis)
			return "HP_negate_front_end_logical";
		else
			return "DW_LNE_HP_negate_front_end_logical";
	case DW_LNE_HP_define_proc:
		if (ellipsis)
			return "HP_define_proc";
		else
			return "DW_LNE_HP_define_proc";
	case DW_LNE_lo_user:
		if (ellipsis)
			return "lo_user";
		else
			return "DW_LNE_lo_user";
	case DW_LNE_hi_user:
		if (ellipsis)
			return "hi_user";
		else
			return "DW_LNE_hi_user";
	default:
		{ 
		    char buf[100]; 
		    char *n; 
		    snprintf(buf,sizeof(buf),"<Unknown LNE value 0x%x>",(int)val);
		 fprintf(stderr,"LNE of %d (0x%x) is unknown to dwarfdump. " 
 		 "Continuing. \n",(int)val,(int)val );  
		    n = makename(buf);
		    return n; 
		} 
	}
/*NOTREACHED*/
}

/* ARGSUSED */
extern string
get_MACINFO_name (Dwarf_Debug dbg, Dwarf_Half val)
{
	switch (val) {
	case DW_MACINFO_define:
		if (ellipsis)
			return "define";
		else
			return "DW_MACINFO_define";
	case DW_MACINFO_undef:
		if (ellipsis)
			return "undef";
		else
			return "DW_MACINFO_undef";
	case DW_MACINFO_start_file:
		if (ellipsis)
			return "start_file";
		else
			return "DW_MACINFO_start_file";
	case DW_MACINFO_end_file:
		if (ellipsis)
			return "end_file";
		else
			return "DW_MACINFO_end_file";
	case DW_MACINFO_vendor_ext:
		if (ellipsis)
			return "vendor_ext";
		else
			return "DW_MACINFO_vendor_ext";
	default:
		{ 
		    char buf[100]; 
		    char *n; 
		    snprintf(buf,sizeof(buf),"<Unknown MACINFO value 0x%x>",(int)val);
		 fprintf(stderr,"MACINFO of %d (0x%x) is unknown to dwarfdump. " 
 		 "Continuing. \n",(int)val,(int)val );  
		    n = makename(buf);
		    return n; 
		} 
	}
/*NOTREACHED*/
}

/* ARGSUSED */
extern string
get_EH_name (Dwarf_Debug dbg, Dwarf_Half val)
{
	switch (val) {
	case DW_EH_PE_absptr:
		if (ellipsis)
			return "PE_absptr";
		else
			return "DW_EH_PE_absptr";
	case DW_EH_PE_uleb128:
		if (ellipsis)
			return "PE_uleb128";
		else
			return "DW_EH_PE_uleb128";
	case DW_EH_PE_udata2:
		if (ellipsis)
			return "PE_udata2";
		else
			return "DW_EH_PE_udata2";
	case DW_EH_PE_udata4:
		if (ellipsis)
			return "PE_udata4";
		else
			return "DW_EH_PE_udata4";
	case DW_EH_PE_udata8:
		if (ellipsis)
			return "PE_udata8";
		else
			return "DW_EH_PE_udata8";
	case DW_EH_PE_sleb128:
		if (ellipsis)
			return "PE_sleb128";
		else
			return "DW_EH_PE_sleb128";
	case DW_EH_PE_sdata2:
		if (ellipsis)
			return "PE_sdata2";
		else
			return "DW_EH_PE_sdata2";
	case DW_EH_PE_sdata4:
		if (ellipsis)
			return "PE_sdata4";
		else
			return "DW_EH_PE_sdata4";
	case DW_EH_PE_sdata8:
		if (ellipsis)
			return "PE_sdata8";
		else
			return "DW_EH_PE_sdata8";
	case DW_EH_PE_pcrel:
		if (ellipsis)
			return "PE_pcrel";
		else
			return "DW_EH_PE_pcrel";
	case DW_EH_PE_textrel:
		if (ellipsis)
			return "PE_textrel";
		else
			return "DW_EH_PE_textrel";
	case DW_EH_PE_datarel:
		if (ellipsis)
			return "PE_datarel";
		else
			return "DW_EH_PE_datarel";
	case DW_EH_PE_funcrel:
		if (ellipsis)
			return "PE_funcrel";
		else
			return "DW_EH_PE_funcrel";
	case DW_EH_PE_aligned:
		if (ellipsis)
			return "PE_aligned";
		else
			return "DW_EH_PE_aligned";
	case DW_EH_PE_omit:
		if (ellipsis)
			return "PE_omit";
		else
			return "DW_EH_PE_omit";
	default:
		{ 
		    char buf[100]; 
		    char *n; 
		    snprintf(buf,sizeof(buf),"<Unknown EH value 0x%x>",(int)val);
		 fprintf(stderr,"EH of %d (0x%x) is unknown to dwarfdump. " 
 		 "Continuing. \n",(int)val,(int)val );  
		    n = makename(buf);
		    return n; 
		} 
	}
/*NOTREACHED*/
}

/* ARGSUSED */
extern string
get_FRAME_name (Dwarf_Debug dbg, Dwarf_Half val)
{
	switch (val) {
	case DW_FRAME_CFA_COL:
		if (ellipsis)
			return "CFA_COL";
		else
			return "DW_FRAME_CFA_COL";
	case DW_FRAME_REG1:
		if (ellipsis)
			return "REG1";
		else
			return "DW_FRAME_REG1";
	case DW_FRAME_REG2:
		if (ellipsis)
			return "REG2";
		else
			return "DW_FRAME_REG2";
	case DW_FRAME_REG3:
		if (ellipsis)
			return "REG3";
		else
			return "DW_FRAME_REG3";
	case DW_FRAME_REG4:
		if (ellipsis)
			return "REG4";
		else
			return "DW_FRAME_REG4";
	case DW_FRAME_REG5:
		if (ellipsis)
			return "REG5";
		else
			return "DW_FRAME_REG5";
	case DW_FRAME_REG6:
		if (ellipsis)
			return "REG6";
		else
			return "DW_FRAME_REG6";
	case DW_FRAME_REG7:
		if (ellipsis)
			return "REG7";
		else
			return "DW_FRAME_REG7";
	case DW_FRAME_REG8:
		if (ellipsis)
			return "REG8";
		else
			return "DW_FRAME_REG8";
	case DW_FRAME_REG9:
		if (ellipsis)
			return "REG9";
		else
			return "DW_FRAME_REG9";
	case DW_FRAME_REG10:
		if (ellipsis)
			return "REG10";
		else
			return "DW_FRAME_REG10";
	case DW_FRAME_REG11:
		if (ellipsis)
			return "REG11";
		else
			return "DW_FRAME_REG11";
	case DW_FRAME_REG12:
		if (ellipsis)
			return "REG12";
		else
			return "DW_FRAME_REG12";
	case DW_FRAME_REG13:
		if (ellipsis)
			return "REG13";
		else
			return "DW_FRAME_REG13";
	case DW_FRAME_REG14:
		if (ellipsis)
			return "REG14";
		else
			return "DW_FRAME_REG14";
	case DW_FRAME_REG15:
		if (ellipsis)
			return "REG15";
		else
			return "DW_FRAME_REG15";
	case DW_FRAME_REG16:
		if (ellipsis)
			return "REG16";
		else
			return "DW_FRAME_REG16";
	case DW_FRAME_REG17:
		if (ellipsis)
			return "REG17";
		else
			return "DW_FRAME_REG17";
	case DW_FRAME_REG18:
		if (ellipsis)
			return "REG18";
		else
			return "DW_FRAME_REG18";
	case DW_FRAME_REG19:
		if (ellipsis)
			return "REG19";
		else
			return "DW_FRAME_REG19";
	case DW_FRAME_REG20:
		if (ellipsis)
			return "REG20";
		else
			return "DW_FRAME_REG20";
	case DW_FRAME_REG21:
		if (ellipsis)
			return "REG21";
		else
			return "DW_FRAME_REG21";
	case DW_FRAME_REG22:
		if (ellipsis)
			return "REG22";
		else
			return "DW_FRAME_REG22";
	case DW_FRAME_REG23:
		if (ellipsis)
			return "REG23";
		else
			return "DW_FRAME_REG23";
	case DW_FRAME_REG24:
		if (ellipsis)
			return "REG24";
		else
			return "DW_FRAME_REG24";
	case DW_FRAME_REG25:
		if (ellipsis)
			return "REG25";
		else
			return "DW_FRAME_REG25";
	case DW_FRAME_REG26:
		if (ellipsis)
			return "REG26";
		else
			return "DW_FRAME_REG26";
	case DW_FRAME_REG27:
		if (ellipsis)
			return "REG27";
		else
			return "DW_FRAME_REG27";
	case DW_FRAME_REG28:
		if (ellipsis)
			return "REG28";
		else
			return "DW_FRAME_REG28";
	case DW_FRAME_REG29:
		if (ellipsis)
			return "REG29";
		else
			return "DW_FRAME_REG29";
	case DW_FRAME_REG30:
		if (ellipsis)
			return "REG30";
		else
			return "DW_FRAME_REG30";
	case DW_FRAME_REG31:
		if (ellipsis)
			return "REG31";
		else
			return "DW_FRAME_REG31";
	case DW_FRAME_FREG0:
		if (ellipsis)
			return "FREG0";
		else
			return "DW_FRAME_FREG0";
	case DW_FRAME_FREG1:
		if (ellipsis)
			return "FREG1";
		else
			return "DW_FRAME_FREG1";
	case DW_FRAME_FREG2:
		if (ellipsis)
			return "FREG2";
		else
			return "DW_FRAME_FREG2";
	case DW_FRAME_FREG3:
		if (ellipsis)
			return "FREG3";
		else
			return "DW_FRAME_FREG3";
	case DW_FRAME_FREG4:
		if (ellipsis)
			return "FREG4";
		else
			return "DW_FRAME_FREG4";
	case DW_FRAME_FREG5:
		if (ellipsis)
			return "FREG5";
		else
			return "DW_FRAME_FREG5";
	case DW_FRAME_FREG6:
		if (ellipsis)
			return "FREG6";
		else
			return "DW_FRAME_FREG6";
	case DW_FRAME_FREG7:
		if (ellipsis)
			return "FREG7";
		else
			return "DW_FRAME_FREG7";
	case DW_FRAME_FREG8:
		if (ellipsis)
			return "FREG8";
		else
			return "DW_FRAME_FREG8";
	case DW_FRAME_FREG9:
		if (ellipsis)
			return "FREG9";
		else
			return "DW_FRAME_FREG9";
	case DW_FRAME_FREG10:
		if (ellipsis)
			return "FREG10";
		else
			return "DW_FRAME_FREG10";
	case DW_FRAME_FREG11:
		if (ellipsis)
			return "FREG11";
		else
			return "DW_FRAME_FREG11";
	case DW_FRAME_FREG12:
		if (ellipsis)
			return "FREG12";
		else
			return "DW_FRAME_FREG12";
	case DW_FRAME_FREG13:
		if (ellipsis)
			return "FREG13";
		else
			return "DW_FRAME_FREG13";
	case DW_FRAME_FREG14:
		if (ellipsis)
			return "FREG14";
		else
			return "DW_FRAME_FREG14";
	case DW_FRAME_FREG15:
		if (ellipsis)
			return "FREG15";
		else
			return "DW_FRAME_FREG15";
	case DW_FRAME_FREG16:
		if (ellipsis)
			return "FREG16";
		else
			return "DW_FRAME_FREG16";
	case DW_FRAME_FREG17:
		if (ellipsis)
			return "FREG17";
		else
			return "DW_FRAME_FREG17";
	case DW_FRAME_FREG18:
		if (ellipsis)
			return "FREG18";
		else
			return "DW_FRAME_FREG18";
	case DW_FRAME_FREG19:
		if (ellipsis)
			return "FREG19";
		else
			return "DW_FRAME_FREG19";
	case DW_FRAME_FREG20:
		if (ellipsis)
			return "FREG20";
		else
			return "DW_FRAME_FREG20";
	case DW_FRAME_FREG21:
		if (ellipsis)
			return "FREG21";
		else
			return "DW_FRAME_FREG21";
	case DW_FRAME_FREG22:
		if (ellipsis)
			return "FREG22";
		else
			return "DW_FRAME_FREG22";
	case DW_FRAME_FREG23:
		if (ellipsis)
			return "FREG23";
		else
			return "DW_FRAME_FREG23";
	case DW_FRAME_FREG24:
		if (ellipsis)
			return "FREG24";
		else
			return "DW_FRAME_FREG24";
	case DW_FRAME_FREG25:
		if (ellipsis)
			return "FREG25";
		else
			return "DW_FRAME_FREG25";
	case DW_FRAME_FREG26:
		if (ellipsis)
			return "FREG26";
		else
			return "DW_FRAME_FREG26";
	case DW_FRAME_FREG27:
		if (ellipsis)
			return "FREG27";
		else
			return "DW_FRAME_FREG27";
	case DW_FRAME_FREG28:
		if (ellipsis)
			return "FREG28";
		else
			return "DW_FRAME_FREG28";
	case DW_FRAME_FREG29:
		if (ellipsis)
			return "FREG29";
		else
			return "DW_FRAME_FREG29";
	case DW_FRAME_FREG30:
		if (ellipsis)
			return "FREG30";
		else
			return "DW_FRAME_FREG30";
	case DW_FRAME_FREG31:
		if (ellipsis)
			return "FREG31";
		else
			return "DW_FRAME_FREG31";
	case DW_FRAME_LAST_REG_NUM:
		if (ellipsis)
			return "LAST_REG_NUM";
		else
			return "DW_FRAME_LAST_REG_NUM";
	default:
		{ 
		    char buf[100]; 
		    char *n; 
		    snprintf(buf,sizeof(buf),"<Unknown FRAME value 0x%x>",(int)val);
		 fprintf(stderr,"FRAME of %d (0x%x) is unknown to dwarfdump. " 
 		 "Continuing. \n",(int)val,(int)val );  
		    n = makename(buf);
		    return n; 
		} 
	}
/*NOTREACHED*/
}

/* ARGSUSED */
extern string
get_CHILDREN_name (Dwarf_Debug dbg, Dwarf_Half val)
{
	switch (val) {
	case DW_CHILDREN_no:
		if (ellipsis)
			return "CHILDREN_no";
		else
			return "DW_CHILDREN_no";
	case DW_CHILDREN_yes:
		if (ellipsis)
			return "CHILDREN_yes";
		else
			return "DW_CHILDREN_yes";
	default:
		{ 
		    char buf[100]; 
		    char *n; 
		    snprintf(buf,sizeof(buf),"<Unknown CHILDREN value 0x%x>",(int)val);
		 fprintf(stderr,"CHILDREN of %d (0x%x) is unknown to dwarfdump. " 
 		 "Continuing. \n",(int)val,(int)val );  
		    n = makename(buf);
		    return n; 
		} 
	}
/*NOTREACHED*/
}

/* ARGSUSED */
extern string
get_ADDR_name (Dwarf_Debug dbg, Dwarf_Half val)
{
	switch (val) {
	case DW_ADDR_none:
		if (ellipsis)
			return "ADDR_none";
		else
			return "DW_ADDR_none";
	default:
		print_error(dbg, "get_ADDR_name unexpected value",DW_DLV_OK, rose_dwarf_error);
	}
	 return "unknown-name-dwarf-error";
}


int
get_small_encoding_integer_and_name(Dwarf_Debug dbg,Dwarf_Attribute attrib,Dwarf_Unsigned * uval_out,char *attr_name,string * string_out,encoding_type_func val_as_string,Dwarf_Error * err)
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



int
tag_attr_combination(Dwarf_Half tag, Dwarf_Half attr)
{
    unsigned tabrows = sizeof(tag_attr_combination_table)/(4*sizeof(unsigned));
    if(tag > tabrows) {
        return false;
    }
    if (attr > 0 && attr < 0x60) {
        return ((tag_attr_combination_table[tag][attr / 0x20]
                 & (1 << (attr % 0x20))) > 0 ? true : false);
    } else if (attr == DW_AT_MIPS_fde) {
        /* no check now */
        return (true);
    } else
        return (false);
}

/*
   Look only at valid table entries
   The check here must match the building-logic in
   tag_tree.c
   And must match the tags defined in dwarf.h
*/
#define MAX_CHECKED_TAG_ID 0x35
static int
tag_tree_combination(Dwarf_Half tag_parent, Dwarf_Half tag_child)
{
    unsigned tabrows = sizeof(tag_tree_combination_table)/(2*sizeof(unsigned));
    if(tag_parent > tabrows) {
        return (false);
    }
    if (tag_parent > 0 && tag_parent <= MAX_CHECKED_TAG_ID
        && tag_child > 0 && tag_child <= MAX_CHECKED_TAG_ID) {
        return ((tag_tree_combination_table[tag_parent]
                 [tag_child / 0x20]
                 & (1 << (tag_child % 0x20))) > 0 ? true : false);
    }
    return (false);
}


/* recursively follow the die tree */
void
print_die_and_children(Dwarf_Debug dbg, Dwarf_Die in_die_in,char **srcfiles, Dwarf_Signed cnt)
   {
     Dwarf_Die child;
     Dwarf_Die sibling;
     Dwarf_Error err;
     int tres;
     int cdres;
     Dwarf_Die in_die = in_die_in;

     printf ("Inside of print_die_and_children() \n");

     for (;;)
        {
          printf ("Top of loop in print_die_and_children() \n");

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
                        if (tag_tree_combination(tag_parent, tag_child))
                           {
                          /* OK */
                           }
                          else
                           {
                             DWARF_CHECK_ERROR3(get_TAG_name(dbg, tag_parent).c_str(),get_TAG_name(dbg, tag_child).c_str(),"tag-tree relation is not standard.");
                           }
                      }
                  }
             }

       /* here to pre-descent processing of the die */
          print_one_die(dbg, in_die, info_flag, srcfiles, cnt);

          printf ("Process children \n");

          cdres = dwarf_child(in_die, &child, &err);
       /* child first: we are doing depth-first walk */
          if (cdres == DW_DLV_OK)
             {
               indent_level++;
               if(indent_level >= DIE_STACK_SIZE )
                  {
                    print_error(dbg,"compiled in DIE_STACK_SIZE limit exceeded",DW_DLV_OK,err);
                  }
               print_die_and_children(dbg, child, srcfiles, cnt);
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

          printf ("Process siblings \n");
          cdres = dwarf_siblingof(dbg, in_die, &sibling, &err);
          if (cdres == DW_DLV_OK)
             {
            /* print_die_and_children(dbg, sibling, srcfiles, cnt); We
               loop around to actually print this, rather than
               recursing. Recursing is horribly wasteful of stack
               space. */
             }
            else 
               if (cdres == DW_DLV_ERROR)
                  {
                    print_error(dbg, "dwarf_siblingof", cdres, err);
                  }

       /* Here do any post-descent (ie post-dwarf_child) processing of the in_die. */

          printf ("Process post-dwarf_child \n");

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

        }  /* end for loop on siblings */

     return;
   }

// int _dwarf_print_one_expr_op(Dwarf_Debug dbg,Dwarf_Loc* expr,int index, struct esb_s *string_out)
int _dwarf_print_one_expr_op(Dwarf_Debug dbg,Dwarf_Loc* expr,int index, string* string_out)
   {
  /* local_space_needed is intended to be 'more than big enough' for a short group of loclist entries.  */
     char small_buf[100];
     Dwarf_Small op;
     Dwarf_Unsigned opd1;  
     Dwarf_Unsigned opd2;
     string op_name;

    if (index > 0)
     // esb_append(string_out, " ");
        *string_out += " ";

    op = expr->lr_atom;
    if (op > DW_OP_nop) {
        print_error(dbg, "dwarf_op unexpected value", DW_DLV_OK,rose_dwarf_error);
        return DW_DLV_ERROR;
    }
    op_name = get_OP_name(dbg, op);
 // esb_append(string_out, op_name);
    *string_out += op_name;

    opd1 = expr->lr_number;
    if (op >= DW_OP_breg0 && op <= DW_OP_breg31) {
            snprintf(small_buf, sizeof(small_buf),"%+lld", (Dwarf_Signed) opd1);
         // esb_append(string_out, small_buf);
            *string_out += small_buf;
    } else {
        switch (op) {
        case DW_OP_addr:
                snprintf(small_buf, sizeof(small_buf), " %#llx", opd1);
             // esb_append(string_out, small_buf);
                *string_out += small_buf;
                break;
        case DW_OP_const1s:
        case DW_OP_const2s:
        case DW_OP_const4s:
        case DW_OP_const8s:
        case DW_OP_consts:
        case DW_OP_skip:
        case DW_OP_bra:
        case DW_OP_fbreg:
                snprintf(small_buf, sizeof(small_buf)," %lld", (Dwarf_Signed) opd1);
             // esb_append(string_out, small_buf);
                *string_out += small_buf;
                break;
        case DW_OP_const1u:
        case DW_OP_const2u:
        case DW_OP_const4u:
        case DW_OP_const8u:
        case DW_OP_constu:
        case DW_OP_pick:
        case DW_OP_plus_uconst:
        case DW_OP_regx:
        case DW_OP_piece:
        case DW_OP_deref_size:
        case DW_OP_xderef_size:
            snprintf(small_buf, sizeof(small_buf), " %llu", opd1);
         // esb_append(string_out, small_buf);
            *string_out += small_buf;
                break;
        case DW_OP_bregx:
            snprintf(small_buf, sizeof(small_buf), "%llu", opd1);
         // esb_append(string_out, small_buf);
            *string_out += small_buf;

            opd2 = expr->lr_number2;
            snprintf(small_buf, sizeof(small_buf),"%+lld", (Dwarf_Signed) opd2);
         // esb_append(string_out, small_buf);
            *string_out += small_buf;
            break;

        default:
            break;
        }
    }

    return DW_DLV_OK;
}

// int dwarfdump_print_one_locdesc(Dwarf_Debug dbg, Dwarf_Locdesc * llbuf, int skip_locdesc_header, struct esb_s *string_out)
int dwarfdump_print_one_locdesc(Dwarf_Debug dbg, Dwarf_Locdesc * llbuf, int skip_locdesc_header, string* string_out)
   {

    Dwarf_Locdesc *locd = 0;
    Dwarf_Half no_of_ops = 0;
    int i = 0;
    char small_buf[100];


    if (!skip_locdesc_header && (verbose || llbuf->ld_from_loclist)) {
        snprintf(small_buf, sizeof(small_buf), "<lowpc=0x%llx>",(unsigned long long) llbuf->ld_lopc);
     // esb_append(string_out, small_buf);
        *string_out += small_buf;

        snprintf(small_buf, sizeof(small_buf), "<highpc=0x%llx>",(unsigned long long) llbuf->ld_hipc);
     // esb_append(string_out, small_buf);
        *string_out += small_buf;

        if (verbose) {
            snprintf(small_buf, sizeof(small_buf),"<from %s offset 0x%llx>",llbuf->ld_from_loclist ? ".debug_loc" : ".debug_info",(unsigned long long) llbuf->ld_section_offset);
         // esb_append(string_out, small_buf);
            *string_out += small_buf;
        }
    }


    locd = llbuf;
    no_of_ops = llbuf->ld_cents;
    for (i = 0; i < no_of_ops; i++) {
        Dwarf_Loc * op = &locd->ld_s[i];

        int res = _dwarf_print_one_expr_op(dbg,op,i,string_out);
        if(res == DW_DLV_ERROR) {
          return res;
        }
    }
    return DW_DLV_OK;
   }


// static void get_location_list(Dwarf_Debug dbg, Dwarf_Die die, Dwarf_Attribute attr, struct esb_s *esbp)
void get_location_list(Dwarf_Debug dbg, Dwarf_Die die, Dwarf_Attribute attr, string* esbp)
{
    Dwarf_Locdesc *llbuf = 0;
    Dwarf_Locdesc **llbufarray = 0;
    Dwarf_Signed no_of_elements;
    Dwarf_Error err;
    int i;
    int lres  = 0;
    int llent = 0;
    int skip_locdesc_header = 0;

    if (use_old_dwarf_loclist) {

        lres = dwarf_loclist(attr, &llbuf, &no_of_elements, &err);
        if (lres == DW_DLV_ERROR)
            print_error(dbg, "dwarf_loclist", lres, err);
        if (lres == DW_DLV_NO_ENTRY)
            return;

        dwarfdump_print_one_locdesc(dbg, llbuf,skip_locdesc_header,esbp);
        dwarf_dealloc(dbg, llbuf->ld_s, DW_DLA_LOC_BLOCK);
        dwarf_dealloc(dbg, llbuf, DW_DLA_LOCDESC);
        return;
    }

    lres = dwarf_loclist_n(attr, &llbufarray, &no_of_elements, &err);
    if (lres == DW_DLV_ERROR)
        print_error(dbg, "dwarf_loclist", lres, err);
    if (lres == DW_DLV_NO_ENTRY)
        return;

    for (llent = 0; llent < no_of_elements; ++llent) {
        char small_buf[100];

        llbuf = llbufarray[llent];

        if (!dense && llbuf->ld_from_loclist) {
            if (llent == 0) {
                snprintf(small_buf, sizeof(small_buf),
                         "<loclist with %ld entries follows>",
                         (long) no_of_elements);
             // esb_append(esbp, small_buf);
                *esbp += small_buf;
            }
         // esb_append(esbp, "\n\t\t\t");
            *esbp += "\n\t\t\t";
            snprintf(small_buf, sizeof(small_buf), "[%2d]", llent);
         // esb_append(esbp, small_buf);
            *esbp += small_buf;
        }
        lres = dwarfdump_print_one_locdesc(dbg,
              llbuf, 
              skip_locdesc_header,
              esbp);
        if (lres == DW_DLV_ERROR) {
            return;
        } else {
            /* DW_DLV_OK so we add follow-on at end, else is
               DW_DLV_NO_ENTRY (which is impossible, treat like
               DW_DLV_OK). */
        }
    }
    for (i = 0; i < no_of_elements; ++i) {
        dwarf_dealloc(dbg, llbufarray[i]->ld_s, DW_DLA_LOC_BLOCK);
        dwarf_dealloc(dbg, llbufarray[i], DW_DLA_LOCDESC);
    }
    dwarf_dealloc(dbg, llbufarray, DW_DLA_LIST);
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
        if(tempud == tempsd) {
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
    case DW_FORM_block:
    case DW_FORM_block1:
    case DW_FORM_block2:
    case DW_FORM_block4:
        fres = dwarf_formblock(attrib, &tempb, &rose_dwarf_error);
        if (fres == DW_DLV_OK) {
            for (i = 0; i < tempb->bl_len; i++) {
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
                        if (srcfiles && tempud > 0 && tempud <= cnt) {
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
                       if(check_decl_file) {
                           decl_file_result.checks++;
                           /* Zero is always a legal index, it means
                              no source name provided. */
                           if(tempud > cnt) {
                               decl_file_result.errors++;
                               DWARF_CHECK_ERROR2(get_AT_name(dbg,attr).c_str(),"does not point to valid file info");
                           }
                       }
                    }
                } else {
                    print_error(dbg, "Cannot get encoding attribute ..",wres, rose_dwarf_error);
                }
                break;
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
        *esbp += get_FORM_name(dbg, theform);
        break;
    default:
        print_error(dbg, "dwarf_whatform unexpected value", DW_DLV_OK,rose_dwarf_error);
    }
    if (verbose && direct_form && direct_form == DW_FORM_indirect) {
        char *form_indir = " (used DW_FORM_indirect) ";

     // esb_append(esbp, form_indir);
        *esbp += form_indir;
    }
}


void
get_FLAG_BLOCK_string(Dwarf_Debug dbg, Dwarf_Attribute attrib, string & esb_base)
{
    int fres = 0;
    Dwarf_Block *tempb = 0;
    __uint32_t * array = 0;
    Dwarf_Unsigned array_len = 0;
    __uint32_t * array_ptr;
    Dwarf_Unsigned array_remain = 0;
    char linebuf[100];

 // esb_empty_string(&esb_base);
 // esb_append(&esb_base, "\n");
    esb_base += "\n";

    /* first get compressed block data */
    fres = dwarf_formblock (attrib,&tempb, &rose_dwarf_error);
    if (fres != DW_DLV_OK) {
        print_error(dbg,"DW_FORM_blockn cannot get block\n",fres,rose_dwarf_error);
        return;
    }

    /* uncompress block into int array */
/*
void *
dwarf_uncompress_integer_block(
    Dwarf_Debug      dbg,
    Dwarf_Bool       unit_is_signed,
    Dwarf_Small      unit_length_in_bits,
    void*            input_block,
    Dwarf_Unsigned   input_length_in_bytes,
    Dwarf_Unsigned*  output_length_in_units_ptr,
    Dwarf_Error*     error
)
*/
    array = (__uint32_t*) dwarf_uncompress_integer_block(dbg,
                           (Dwarf_Bool) 1, /* 'true' (meaning signed ints)*/
                           (Dwarf_Small) 32, /* bits per unit */
                           (Dwarf_Ptr) (tempb->bl_data),
                           (Dwarf_Unsigned) (tempb->bl_len),
                           &array_len, /* len of out array */
                           &rose_dwarf_error);
    if (array == (void*) DW_DLV_BADOFFSET) {
        print_error(dbg,"DW_AT_SUN_func_offsets cannot uncompress data\n",0,rose_dwarf_error);
        return;
    }
    if (array_len == 0) {
        print_error(dbg,"DW_AT_SUN_func_offsets has no data\n",0,rose_dwarf_error);
        return;
    }
    
    /* fill in string buffer */
    array_remain = array_len;
    array_ptr = array;
    while (array_remain > 8) {
        /* print a full line */
        /* if you touch this string, update the magic number 78 below! */
        snprintf(linebuf, sizeof(linebuf), 
                "\n  %8x %8x %8x %8x %8x %8x %8x %8x",
                array_ptr[0],           array_ptr[1],
                array_ptr[2],           array_ptr[3],
                array_ptr[4],           array_ptr[5],
                array_ptr[6],           array_ptr[7]);
        array_ptr += 8;
        array_remain -= 8;
     // esb_append(&esb_base, linebuf);
        esb_base += linebuf;
    }

    /* now do the last line */
    if (array_remain > 0) {
     // esb_append(&esb_base, "\n ");
        esb_base += "\n";
        while (array_remain > 0) {
            snprintf(linebuf, sizeof(linebuf), " %8x", *array_ptr);
            array_remain--;
            array_ptr++;
         // esb_append(&esb_base, linebuf);
            esb_base += linebuf;
        }
    }
    
    /* free array buffer */
    dwarf_dealloc_uncompressed_block(dbg, array);

}

#define SPACE(x) { register int i; for (i=0;i<x;i++) putchar(' '); }


void
print_attribute(Dwarf_Debug dbg, Dwarf_Die die, Dwarf_Half attr,
                Dwarf_Attribute attr_in,
                bool print_information,
                char **srcfiles, Dwarf_Signed cnt)
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

    /* the following gets the real attribute, even in the face of an 
       incorrect doubling, or worse, of attributes */
    attrib = attr_in;
    /* do not get attr via dwarf_attr: if there are (erroneously) 
       multiple of an attr in a DIE, dwarf_attr will not get the
       second, erroneous one and dwarfdump will print the first one
       multiple times. Oops. */

    tres = dwarf_tag(die, &tag, &rose_dwarf_error);
    if (tres == DW_DLV_ERROR) {
        tag = 0;
    } else if (tres == DW_DLV_NO_ENTRY) {
        tag = 0;
    } else {
        /* ok */
    }
    if (check_attr_tag) {
        string tagname = "<tag invalid>";

        attr_tag_result.checks++;
        if (tres == DW_DLV_ERROR) {
            attr_tag_result.errors++;
            DWARF_CHECK_ERROR3(tagname.c_str(),
                               get_AT_name(dbg, attr).c_str(),
                               "check the tag-attr combination.");
        } else if (tres == DW_DLV_NO_ENTRY) {
            attr_tag_result.errors++;
            DWARF_CHECK_ERROR3(tagname.c_str(),
                               get_AT_name(dbg, attr).c_str(),
                               "check the tag-attr combination..")
        } else if (tag_attr_combination(tag, attr)) {
            /* OK */
        } else {
            attr_tag_result.errors++;
            tagname = get_TAG_name(dbg, tag);
            DWARF_CHECK_ERROR3(tagname.c_str(),
                               get_AT_name(dbg, attr).c_str(),
                               "check the tag-attr combination")
        }
    }

    switch (attr) {
    case DW_AT_language:
        get_small_encoding_integer_and_name(dbg, attrib, &uval,
                                            "DW_AT_language", &valname,
                                            get_LANG_name, &rose_dwarf_error);
        break;
    case DW_AT_accessibility:
        get_small_encoding_integer_and_name(dbg, attrib, &uval,
                                            "DW_AT_accessibility",
                                            &valname, get_ACCESS_name,
                                            &rose_dwarf_error);
        break;
    case DW_AT_visibility:
        get_small_encoding_integer_and_name(dbg, attrib, &uval,
                                            "DW_AT_visibility",
                                            &valname, get_VIS_name,
                                            &rose_dwarf_error);
        break;
    case DW_AT_virtuality:
        get_small_encoding_integer_and_name(dbg, attrib, &uval,
                                            "DW_AT_virtuality",
                                            &valname,
                                            get_VIRTUALITY_name, &rose_dwarf_error);
        break;
    case DW_AT_identifier_case:
        get_small_encoding_integer_and_name(dbg, attrib, &uval,
                                            "DW_AT_identifier",
                                            &valname, get_ID_name,
                                            &rose_dwarf_error);
        break;
    case DW_AT_inline:
        get_small_encoding_integer_and_name(dbg, attrib, &uval,
                                            "DW_AT_inline", &valname,
                                            get_INL_name, &rose_dwarf_error);
        break;
    case DW_AT_encoding:
        get_small_encoding_integer_and_name(dbg, attrib, &uval,
                                            "DW_AT_encoding", &valname,
                                            get_ATE_name, &rose_dwarf_error);
        break;
    case DW_AT_ordering:
        get_small_encoding_integer_and_name(dbg, attrib, &uval,
                                            "DW_AT_ordering", &valname,
                                            get_ORD_name, &rose_dwarf_error);
        break;
    case DW_AT_calling_convention:
        get_small_encoding_integer_and_name(dbg, attrib, &uval,
                                            "DW_AT_calling_convention",
                                            &valname, get_CC_name,
                                            &rose_dwarf_error);
        break;
    case DW_AT_discr_list:      /* DWARF3 */
        get_small_encoding_integer_and_name(dbg, attrib, &uval,
                                            "DW_AT_discr_list",
                                            &valname, get_DSC_name,
                                            &rose_dwarf_error);
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
        }
        break;

    case DW_AT_upper_bound:
        {
            Dwarf_Half theform;
            int rv;
            rv = dwarf_whatform(attrib,&theform,&rose_dwarf_error);
            /* depending on the form and the attribute, process the form */
            if(rv == DW_DLV_ERROR) {
                print_error(dbg, "dwarf_whatform cannot find attr form",
                            rv, rose_dwarf_error);
            } else if (rv == DW_DLV_NO_ENTRY) {
                break;
            }

            switch (theform) {
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
            if(rv == DW_DLV_ERROR) {
                print_error(dbg, "dwarf_whatform cannot find attr form",
                            rv, rose_dwarf_error);
            } else if (rv == DW_DLV_NO_ENTRY) {
                break;
            }

         // esb_empty_string(&esb_base);
            get_attr_value(dbg, tag, attrib, srcfiles, cnt, &esb_base);
            if( theform != DW_FORM_addr) {
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
    if (print_information) {
        if (dense)
            printf(" %s<%s>", atname.c_str(), valname.c_str());
        else
            printf("\t\t%-28s%s\n", atname.c_str(), valname.c_str());
    }
}


/* print info about die */
void
print_one_die(Dwarf_Debug dbg, Dwarf_Die die, bool print_information, char **srcfiles, Dwarf_Signed cnt)
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

     if (!dst_format && print_information)
        {
          if (indent_level == 0)
            {
              if (dense)
                   printf("\n");
                else
                 {
                   printf("\nCOMPILE_UNIT<header overall offset = %llu>:\n",overall_offset - offset);
                 }
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

     printf ("Handle attributes: atcnt = %d \n",atcnt);
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

    for (i = 0; i < atcnt; i++) {
        dwarf_dealloc(dbg, atlist[i], DW_DLA_ATTR);
    }
    if (atres == DW_DLV_OK) {
        dwarf_dealloc(dbg, atlist, DW_DLA_LIST);
    }

    if (dense && print_information) {
        printf("\n\n");
    }
    return;
}

// ************************************************************************

/* The new (April 2005) dwarf_get_section_max_offsets()
   in libdwarf returns all max-offsets, but we only
   want one of those offsets. This function returns 
   the one we want from that set,
   making functions needing this offset as readable as possible.
   (avoiding code duplication).
*/
Dwarf_Unsigned
get_info_max_offset(Dwarf_Debug dbg)
   {
     Dwarf_Unsigned debug_info_size = 0;
     Dwarf_Unsigned debug_abbrev_size = 0;
     Dwarf_Unsigned debug_line_size = 0;
     Dwarf_Unsigned debug_loc_size = 0;
     Dwarf_Unsigned debug_aranges_size = 0;
     Dwarf_Unsigned debug_macinfo_size = 0;
     Dwarf_Unsigned debug_pubnames_size = 0;
     Dwarf_Unsigned debug_str_size = 0;
     Dwarf_Unsigned debug_frame_size = 0;
     Dwarf_Unsigned debug_ranges_size = 0;
     Dwarf_Unsigned debug_pubtypes_size = 0;

     dwarf_get_section_max_offsets(dbg,
                                  &debug_info_size,
                                  &debug_abbrev_size,
                                  &debug_line_size,
                                  &debug_loc_size,
                                  &debug_aranges_size,
                                  &debug_macinfo_size,
                                  &debug_pubnames_size,
                                  &debug_str_size,
                                  &debug_frame_size,
                                  &debug_ranges_size,
                                  &debug_pubtypes_size);

     return debug_info_size;
   }

void
deal_with_name_offset_err(Dwarf_Debug dbg,
                          char *err_loc,
                          char *name, Dwarf_Unsigned die_off,
                          int nres, Dwarf_Error err)
{
    if (nres == DW_DLV_ERROR) {
        Dwarf_Unsigned myerr = dwarf_errno(err);

        if (myerr == DW_DLE_OFFSET_BAD) {
            printf("Error: bad offset %s, %s %lld (0x%llx)\n",err_loc,name,(long long) die_off, (unsigned long long) die_off);
        }
        print_error(dbg, err_loc, nres, err);
    }
}


/* This unifies the code for some error checks to
   avoid code duplication.
*/
void
check_info_offset_sanity(char *sec,
                         char *field,
                         char *global,
                         Dwarf_Unsigned offset, Dwarf_Unsigned maxoff)
{
    if (maxoff == 0) {
        /* Lets make a heuristic check. */
        if (offset > 0xffffffff) {
            printf("Warning: section %s %s %s offset 0x%llx " "exceptionally large \n",sec, field, global, (unsigned long long) offset);
        }
    }
    if (offset >= maxoff) {
        printf("Warning: section %s %s %s offset 0x%llx " "larger than max of 0x%llx\n",sec, field, global, (unsigned long long) offset,(unsigned long long) maxoff);
    }
}


/* Unified pubnames style output.
   The error checking here against maxoff may be useless
   (in that libdwarf may return an error if the offset is bad
   and we will not get called here).
   But we leave it in nonetheless as it looks sensible.
   In at least one gigantic executable such offsets turned out wrong.
*/
void
print_pubname_style_entry(Dwarf_Debug dbg,
                          char *line_title,
                          char *name,
                          Dwarf_Unsigned die_off,
                          Dwarf_Unsigned cu_off,
                          Dwarf_Unsigned global_cu_offset,
                          Dwarf_Unsigned maxoff)
{
    Dwarf_Die die = NULL;
    Dwarf_Die cu_die = NULL;
    Dwarf_Off die_CU_off = 0;
    int dres = 0;
    int ddres = 0;
    int cudres = 0;

    /* get die at die_off */
    dres = dwarf_offdie(dbg, die_off, &die, &rose_dwarf_error);
    if (dres != DW_DLV_OK)
        print_error(dbg, "dwarf_offdie", dres, rose_dwarf_error);

    /* get offset of die from its cu-header */
    ddres = dwarf_die_CU_offset(die, &die_CU_off, &rose_dwarf_error);
    if (ddres != DW_DLV_OK) {
        print_error(dbg, "dwarf_die_CU_offset", ddres, rose_dwarf_error);
    }

    /* get die at offset cu_off */
    cudres = dwarf_offdie(dbg, cu_off, &cu_die, &rose_dwarf_error);
    if (cudres != DW_DLV_OK) {
        dwarf_dealloc(dbg, die, DW_DLA_DIE);
        print_error(dbg, "dwarf_offdie", cudres, rose_dwarf_error);
    }
    printf("%s %-15s die-in-sect %lld, cu-in-sect %lld,"
           " die-in-cu %lld, cu-header-in-sect %lld",
           line_title, name, (long long) die_off, (long long) cu_off,
           /* the cu die offset */
           (long long) die_CU_off,
           /* following is absolute offset of the ** beginning of the
              cu */
           (long long) (die_off - die_CU_off));

    if ((die_off - die_CU_off) != global_cu_offset) {
        printf(" error: real cuhdr %llu", global_cu_offset);
        exit(1);
    }
    if (verbose) {
        printf(" cuhdr %llu", global_cu_offset);
    }


    dwarf_dealloc(dbg, die, DW_DLA_DIE);
    dwarf_dealloc(dbg, cu_die, DW_DLA_DIE);


    printf("\n");

    check_info_offset_sanity(line_title,
                             "die offset", name, die_off, maxoff);
    check_info_offset_sanity(line_title,
                             "die cu offset", name, die_CU_off, maxoff);
    check_info_offset_sanity(line_title,
                             "cu offset", name,
                             (die_off - die_CU_off), maxoff);

}

/* get all the data in .debug_pubnames */
void
print_pubnames(Dwarf_Debug dbg)
{
    Dwarf_Global *globbuf = NULL;
    Dwarf_Signed count = 0;
    Dwarf_Signed i = 0;
    Dwarf_Off die_off = 0;
    Dwarf_Off cu_off = 0;
    char *name = 0;
    int res = 0;

    printf("\n.debug_pubnames\n");

    res = dwarf_get_globals(dbg, &globbuf, &count, &rose_dwarf_error);
    if (res == DW_DLV_ERROR) {
        print_error(dbg, "dwarf_get_globals", res, rose_dwarf_error);
    } else if (res == DW_DLV_NO_ENTRY) {
        /* (err == 0 && count == DW_DLV_NOCOUNT) means there are no
           pubnames.  */
    } else {
        Dwarf_Unsigned maxoff = get_info_max_offset(dbg);

        for (i = 0; i < count; i++) {
            int nres;
            int cures3;
            Dwarf_Off global_cu_off = 0;

            nres = dwarf_global_name_offsets(globbuf[i],&name, &die_off, &cu_off,&rose_dwarf_error);
            deal_with_name_offset_err(dbg, "dwarf_global_name_offsets",name, die_off, nres, rose_dwarf_error);

            cures3 = dwarf_global_cu_offset(globbuf[i],&global_cu_off, &rose_dwarf_error);
            if (cures3 != DW_DLV_OK) {
                print_error(dbg, "dwarf_global_cu_offset", cures3, rose_dwarf_error);
            }

            print_pubname_style_entry(dbg,"global",name, die_off, cu_off,global_cu_off, maxoff);

            /* print associated die too? */

            if (check_pubname_attr) {
                Dwarf_Bool has_attr;
                int ares;
                int dres;
                Dwarf_Die die;

                /* get die at die_off */
                dres = dwarf_offdie(dbg, die_off, &die, &rose_dwarf_error);
                if (dres != DW_DLV_OK) {
                    print_error(dbg, "dwarf_offdie", dres, rose_dwarf_error);
                }

                ares = dwarf_hasattr(die, DW_AT_external, &has_attr, &rose_dwarf_error);
                if (ares == DW_DLV_ERROR) {
                    print_error(dbg, "hassattr on DW_AT_external", ares,rose_dwarf_error);
                }
                pubname_attr_result.checks++;
                if (ares == DW_DLV_OK && has_attr) {
                    /* Should the value of flag be examined? */
                } else {
                    pubname_attr_result.errors++;
                    DWARF_CHECK_ERROR2(name,"pubname does not have DW_AT_external")
                }
                dwarf_dealloc(dbg, die, DW_DLA_DIE);
            }
        }
        dwarf_globals_dealloc(dbg, globbuf, count);
    }
}                               /* print_pubnames() */

// ************************************************************************

struct macro_counts_s {
    long mc_start_file;
    long mc_end_file;
    long mc_define;
    long mc_undef;
    long mc_extension;
    long mc_code_zero;
    long mc_unknown;
};

void
print_one_macro_entry_detail(long i,
                             char *type,
                             struct Dwarf_Macro_Details_s *mdp)
{
    /* "DW_MACINFO_*: section-offset file-index [line] string\n" */
    if (mdp->dmd_macro) {
        printf("%3ld %s: %6llu %2lld [%4lld] \"%s\" \n",
               i,
               type,
               mdp->dmd_offset,
               mdp->dmd_fileindex, mdp->dmd_lineno, mdp->dmd_macro);
    } else {
        printf("%3ld %s: %6llu %2lld [%4lld] 0\n",
               i,
               type,
               mdp->dmd_offset, mdp->dmd_fileindex, mdp->dmd_lineno);
    }

}

void
print_one_macro_entry(long i,
                      struct Dwarf_Macro_Details_s *mdp,
                      struct macro_counts_s *counts)
{

    switch (mdp->dmd_type) {
    case 0:
        counts->mc_code_zero++;
        print_one_macro_entry_detail(i, "DW_MACINFO_type-code-0", mdp);
        break;

    case DW_MACINFO_start_file:
        counts->mc_start_file++;
        print_one_macro_entry_detail(i, "DW_MACINFO_start_file", mdp);
        break;

    case DW_MACINFO_end_file:
        counts->mc_end_file++;
        print_one_macro_entry_detail(i, "DW_MACINFO_end_file  ", mdp);
        break;

    case DW_MACINFO_vendor_ext:
        counts->mc_extension++;
        print_one_macro_entry_detail(i, "DW_MACINFO_vendor_ext", mdp);
        break;

    case DW_MACINFO_define:
        counts->mc_define++;
        print_one_macro_entry_detail(i, "DW_MACINFO_define    ", mdp);
        break;

    case DW_MACINFO_undef:
        counts->mc_undef++;
        print_one_macro_entry_detail(i, "DW_MACINFO_undef     ", mdp);
        break;

    default:
        {
            char create_type[50];       /* More than large enough. */

            counts->mc_unknown++;
            snprintf(create_type, sizeof(create_type),
                     "DW_MACINFO_0x%x", mdp->dmd_type);
            print_one_macro_entry_detail(i, create_type, mdp);
        }
        break;
    }
}

/* print data in .debug_macinfo */
/* FIXME: should print name of file whose index is in macro data
   here  --  somewhere.
*/
void
print_macinfo(Dwarf_Debug dbg)
{
    Dwarf_Off offset = 0;
    Dwarf_Unsigned max = 0;
    Dwarf_Signed count = 0;
    long group = 0;
    Dwarf_Macro_Details *maclist = NULL;
    int lres = 0;

    printf("\n.debug_macinfo\n");

    while ((lres = dwarf_get_macro_details(dbg, offset,max, &count, &maclist, &rose_dwarf_error)) == DW_DLV_OK)
       {
        long i;
        struct macro_counts_s counts;


        memset(&counts, 0, sizeof(counts));

        printf("\n");
        printf("compilation-unit .debug_macinfo # %ld\n", group);
        printf
            ("num name section-offset file-index [line] \"string\"\n");
        for (i = 0; i < count; i++) {
            struct Dwarf_Macro_Details_s *mdp = &maclist[i];

            print_one_macro_entry(i, mdp, &counts);
        }

        if (counts.mc_start_file == 0) {
            printf
                ("DW_MACINFO file count of zero is invalid DWARF2/3\n");
        }
        if (counts.mc_start_file != counts.mc_end_file) {
            printf("Counts of DW_MACINFO file (%ld) end_file (%ld) "
                   "do not match!.\n",
                   counts.mc_start_file, counts.mc_end_file);
        }
        if (counts.mc_code_zero < 1) {
            printf("Count of zeros in macro group should be non-zero "
                   "(1 preferred), count is %ld\n",
                   counts.mc_code_zero);
        }
        printf("Macro counts: start file %ld, "
               "end file %ld, "
               "define %ld, "
               "undef %ld "
               "ext %ld, "
               "code-zero %ld, "
               "unknown %ld\n",
               counts.mc_start_file,
               counts.mc_end_file,
               counts.mc_define,
               counts.mc_undef,
               counts.mc_extension,
               counts.mc_code_zero, counts.mc_unknown);


        /* int type= maclist[count - 1].dmd_type; */
        /* ASSERT: type is zero */

        offset = maclist[count - 1].dmd_offset + 1;
        dwarf_dealloc(dbg, maclist, DW_DLA_STRING);
        ++group;
    }
    if (lres == DW_DLV_ERROR) {
        print_error(dbg, "dwarf_get_macro_details", lres, rose_dwarf_error);
    }
}

// ************************************************************************

/* print data in .debug_loc */
extern void
print_locs(Dwarf_Debug dbg)
   {
     Dwarf_Unsigned offset = 0;
     Dwarf_Addr hipc_offset = 0;
     Dwarf_Addr lopc_offset = 0;
     Dwarf_Ptr data = 0;
     Dwarf_Unsigned entry_len = 0;
     Dwarf_Unsigned next_entry = 0;
     int lres = 0;

     printf("\n.debug_loc format <o b e l> means " "section-offset begin-addr end-addr length-of-block-entry\n");

     while ((lres = dwarf_get_loclist_entry(dbg, offset,
                                           &hipc_offset, &lopc_offset,
                                           &data, &entry_len,
                                           &next_entry,
                                           &rose_dwarf_error)) == DW_DLV_OK)
        {
          printf("\t <obel> 0x%08llx 0x%09llx " "0x%08llx " "%8lld\n",(long long) offset, (long long) lopc_offset,(long long) hipc_offset, (long long) entry_len);
          offset = next_entry;
        }

     if (lres == DW_DLV_ERROR)
        {
          print_error(dbg, "dwarf_get_loclist_entry", lres, rose_dwarf_error);
        }
   }


// ************************************************************************

/* print data in .debug_abbrev */
extern void
print_abbrevs(Dwarf_Debug dbg)
{
    Dwarf_Abbrev ab;
    Dwarf_Unsigned offset = 0;
    Dwarf_Unsigned length = 0;
    Dwarf_Unsigned attr_count = 0;
    /* Maximum defined tag is 0xffff, DW_TAG_hi_user. */
    Dwarf_Half tag = 0;
    Dwarf_Half attr = 0;
    Dwarf_Signed form = 0;
    Dwarf_Off off = 0;
    Dwarf_Unsigned i = 0;
    string child_name;
    Dwarf_Unsigned abbrev_num = 1;
    Dwarf_Signed child_flag = 0;
    int abres = 0;
    int tres = 0;
    int acres = 0;
    Dwarf_Unsigned abbrev_code = 0;

    printf("\n.debug_abbrev\n");

    while ((abres = dwarf_get_abbrev(dbg, offset, &ab,
                                     &length, &attr_count,
                                     &rose_dwarf_error)) == DW_DLV_OK) {

        if (attr_count == 0) {
            /* Simple innocuous zero : null abbrev entry */
            if (dense) {
                printf("<%lld><%lld><%lld><%s>\n", abbrev_num, offset, (signed long long)       /* abbrev_code 
                                                                                                 */ 0,
                       "null .debug_abbrev entry");
            } else {
                printf("<%4lld><%5lld><code: %2lld> %-20s\n", abbrev_num, offset, (signed long long)    /* abbrev_code 
                                                                                                         */ 0,
                       "null .debug_abbrev entry");
            }

            offset += length;
            ++abbrev_num;
            dwarf_dealloc(dbg, ab, DW_DLA_ABBREV);
            continue;
        }
        tres = dwarf_get_abbrev_tag(ab, &tag, &rose_dwarf_error);
        if (tres != DW_DLV_OK) {
            dwarf_dealloc(dbg, ab, DW_DLA_ABBREV);
            print_error(dbg, "dwarf_get_abbrev_tag", tres, rose_dwarf_error);
        }
        tres = dwarf_get_abbrev_code(ab, &abbrev_code, &rose_dwarf_error);
        if (tres != DW_DLV_OK) {
            dwarf_dealloc(dbg, ab, DW_DLA_ABBREV);
            print_error(dbg, "dwarf_get_abbrev_code", tres, rose_dwarf_error);
        }
        if (dense)
            printf("<%lld><%lld><%lld><%s>", abbrev_num,offset, abbrev_code, get_TAG_name(dbg, tag).c_str());
        else
            printf("<%4lld><%5lld><code: %2lld> %-20s", abbrev_num,offset, abbrev_code, get_TAG_name(dbg, tag).c_str());
        ++abbrev_num;
        acres = dwarf_get_abbrev_children_flag(ab, &child_flag, &rose_dwarf_error);
        if (acres == DW_DLV_ERROR) {
            dwarf_dealloc(dbg, ab, DW_DLA_ABBREV);
            print_error(dbg, "dwarf_get_abbrev_children_flag", acres,rose_dwarf_error);
        }
        if (acres == DW_DLV_NO_ENTRY) {
            child_flag = 0;
        }
        child_name = get_children_name(dbg, child_flag);
        if (dense)
            printf(" %s", child_name.c_str());
        else
            printf("%s\n", child_name.c_str());
        /* Abbrev just contains the format of a die, which debug_info
           then points to with the real data. So here we just print the 
           given format. */
        for (i = 0; i < attr_count; i++) {
            int aeres;

            aeres =
                dwarf_get_abbrev_entry(ab, i, &attr, &form, &off, &rose_dwarf_error);
            if (aeres == DW_DLV_ERROR) {
                dwarf_dealloc(dbg, ab, DW_DLA_ABBREV);
                print_error(dbg, "dwarf_get_abbrev_entry", aeres, rose_dwarf_error);
            }
            if (aeres == DW_DLV_NO_ENTRY) {
                attr = -1LL;
                form = -1LL;
            }
            if (dense)
                printf(" <%ld>%s<%s>", (unsigned long) off, get_AT_name(dbg, attr).c_str(),get_FORM_name(dbg, (Dwarf_Half) form).c_str());
            else
                printf("      <%5ld>\t%-28s%s\n",(unsigned long) off, get_AT_name(dbg, attr).c_str(),get_FORM_name(dbg, (Dwarf_Half) form).c_str());
        }
        dwarf_dealloc(dbg, ab, DW_DLA_ABBREV);
        offset += length;
        if (dense)
            printf("\n");
    }
    if (abres == DW_DLV_ERROR) {
        print_error(dbg, "dwarf_get_abbrev", abres, rose_dwarf_error);
    }
}

// ************************************************************************

/* print data in .debug_string */
extern void
print_strings(Dwarf_Debug dbg)
   {
     Dwarf_Signed length = 0;
     char* name;
     Dwarf_Off offset = 0;
     int sres = 0;

     printf("\n.debug_string\n");

     while ((sres = dwarf_get_str(dbg, offset, &name, &length, &rose_dwarf_error)) == DW_DLV_OK)
        {
          printf("name at offset %lld, length %lld is %s\n",offset, length, name);
          offset += length + 1;
        }
    /* An inability to find the section is not necessarily
       a real error, so do not report error unless we've
       seen a real record. */
     if(sres == DW_DLV_ERROR && offset != 0)
        {
          print_error(dbg, "dwarf_get_str failure", sres, rose_dwarf_error);
        }
   }

// ************************************************************************

/* get all the data in .debug_aranges */
extern void
print_aranges(Dwarf_Debug dbg)
{
    Dwarf_Signed count = 0;
    Dwarf_Signed i = 0;
    Dwarf_Arange *arange_buf = NULL;
    Dwarf_Addr start = 0;
    Dwarf_Unsigned length = 0;
    Dwarf_Off cu_die_offset = 0;
    Dwarf_Die cu_die = NULL;
    int ares = 0;
    int aires = 0;

    printf("\n.debug_aranges\n");

    ares = dwarf_get_aranges(dbg, &arange_buf, &count, &rose_dwarf_error);
    if (ares == DW_DLV_ERROR) {
        print_error(dbg, "dwarf_get_aranges", ares, rose_dwarf_error);
    } else if (ares == DW_DLV_NO_ENTRY) {
        /* no arange is included */
    } else {
        for (i = 0; i < count; i++) {
            aires = dwarf_get_arange_info(arange_buf[i],
                                          &start, &length,
                                          &cu_die_offset, &rose_dwarf_error);
            if (aires != DW_DLV_OK) {
                print_error(dbg, "dwarf_get_arange_info", aires, rose_dwarf_error);
            } else {
                int dres;

                dres = dwarf_offdie(dbg, cu_die_offset, &cu_die, &rose_dwarf_error);
                if (dres != DW_DLV_OK) {
                    print_error(dbg, "dwarf_offdie", dres, rose_dwarf_error);
                } else {
                    if (cu_name_flag) {
                        Dwarf_Half tag;
                        Dwarf_Attribute attrib;
                        Dwarf_Half theform;
                        int tres;
                        int dares;
                        int fres;

                        tres = dwarf_tag(cu_die, &tag, &rose_dwarf_error);
                        if (tres != DW_DLV_OK) {
                            print_error(dbg, "dwarf_tag in aranges",
                                        tres, rose_dwarf_error);
                        }
                        dares =
                            dwarf_attr(cu_die, DW_AT_name, &attrib,
                                       &rose_dwarf_error);
                        if (dares != DW_DLV_OK) {
                            print_error(dbg, "dwarf_attr arange"
                                        " derived die has no name",
                                        dres, rose_dwarf_error);
                        }
                        fres = dwarf_whatform(attrib, &theform, &rose_dwarf_error);
                        if (fres == DW_DLV_OK) {
                            if (theform == DW_FORM_string
                                || theform == DW_FORM_strp) {
                                char* temps;
                                int sres;

                                sres = dwarf_formstring(attrib, &temps,&rose_dwarf_error);
                                if (sres == DW_DLV_OK) {
                                    char* p = temps;

                                    if (cu_name[0] != '/') {
                                        p = strrchr(temps, '/');
                                        if (p == NULL) {
                                            p = temps;
                                        } else {
                                            p++;
                                        }
                                    }
                                    if (!strcmp(cu_name, p)) {
                                    } else {
                                        continue;
                                    }
                                } else {
                                    print_error(dbg,"arange: string missing",sres, rose_dwarf_error);
                                }
                            }
                        } else {
                            print_error(dbg,"dwarf_whatform unexpected value",fres, rose_dwarf_error);
                        }
                        dwarf_dealloc(dbg, attrib, DW_DLA_ATTR);
                    }
                    printf("\narange starts at %llx, " "length of %lld, cu_die_offset = %lld",start, length, cu_die_offset);
                    /* get the offset of the cu header itself in the
                       section */
                    {
                        Dwarf_Off off = 0;
                        int cures3 =
                            dwarf_get_arange_cu_header_offset(arange_buf
                                                              [i],
                                                              &off,
                                                              &rose_dwarf_error);

                        if (cures3 != DW_DLV_OK) {
                            print_error(dbg, "dwarf_get_cu_hdr_offset",
                                        cures3, rose_dwarf_error);
                        }
                        if (verbose)
                            printf(" cuhdr %llu", off);
                    }
                    printf("\n");
                    print_one_die(dbg, cu_die, (bool) true,/* srcfiles= */ 0,/* cnt= */ 0);

                    dwarf_dealloc(dbg, cu_die, DW_DLA_DIE);
                }
            }
            /* print associated die too? */
            dwarf_dealloc(dbg, arange_buf[i], DW_DLA_ARANGE);
        }
        dwarf_dealloc(dbg, arange_buf, DW_DLA_LIST);
    }
}

// ************************************************************************

/* Get all the data in .debug_static_funcs 
   On error, this allows some dwarf memory leaks.
*/
extern void
print_static_funcs(Dwarf_Debug dbg)
{
    Dwarf_Func *funcbuf = NULL;
    Dwarf_Signed count = 0;
    Dwarf_Signed i = 0;
    Dwarf_Off die_off = 0;
    Dwarf_Off cu_off = 0;
    int gfres = 0;

    printf("\n.debug_static_func\n");

    gfres = dwarf_get_funcs(dbg, &funcbuf, &count, &rose_dwarf_error);
    if (gfres == DW_DLV_ERROR) {
        print_error(dbg, "dwarf_get_funcs", gfres, rose_dwarf_error);
    } else if (gfres == DW_DLV_NO_ENTRY) {
        /* no static funcs */
    } else {
        Dwarf_Unsigned maxoff = get_info_max_offset(dbg);

        for (i = 0; i < count; i++) {
            int fnres;
            int cures3;
            Dwarf_Unsigned global_cu_off = 0;
            char *name = 0;

            fnres =
                dwarf_func_name_offsets(funcbuf[i], &name, &die_off,
                                        &cu_off, &rose_dwarf_error);
            deal_with_name_offset_err(dbg, "dwarf_func_name_offsets",
                                      name, die_off, fnres, rose_dwarf_error);

            cures3 = dwarf_func_cu_offset(funcbuf[i],
                                          &global_cu_off, &rose_dwarf_error);
            if (cures3 != DW_DLV_OK) {
                print_error(dbg, "dwarf_global_cu_offset", cures3, rose_dwarf_error);
            }

            print_pubname_style_entry(dbg,
                                      "static-func", name, die_off,
                                      cu_off, global_cu_off, maxoff);


            /* print associated die too? */
            if (check_pubname_attr) {
                Dwarf_Bool has_attr;
                int ares;
                int dres;
                Dwarf_Die die;

                /* get die at die_off */
                dres = dwarf_offdie(dbg, die_off, &die, &rose_dwarf_error);
                if (dres != DW_DLV_OK) {
                    print_error(dbg, "dwarf_offdie", dres, rose_dwarf_error);
                }


                ares =
                    dwarf_hasattr(die, DW_AT_external, &has_attr, &rose_dwarf_error);
                if (ares == DW_DLV_ERROR) {
                    print_error(dbg, "hassattr on DW_AT_external", ares,
                                rose_dwarf_error);
                }
                pubname_attr_result.checks++;
                if (ares == DW_DLV_OK && has_attr) {
                    /* Should the value of flag be examined? */
                } else {
                    pubname_attr_result.errors++;
                    DWARF_CHECK_ERROR2(name,
                                       "pubname does not have DW_AT_external")
                }
                dwarf_dealloc(dbg, die, DW_DLA_DIE);
            }
        }
        dwarf_funcs_dealloc(dbg, funcbuf, count);
    }
}                               /* print_static_funcs() */

// ************************************************************************

/* get all the data in .debug_static_vars */
extern void
print_static_vars(Dwarf_Debug dbg)
{
    Dwarf_Var *varbuf = NULL;
    Dwarf_Signed count = 0;
    Dwarf_Signed i = 0;
    Dwarf_Off die_off = 0;
    Dwarf_Off cu_off = 0;
    char *name = 0;
    int gvres = 0;

    printf("\n.debug_static_vars\n");

    gvres = dwarf_get_vars(dbg, &varbuf, &count, &rose_dwarf_error);
    if (gvres == DW_DLV_ERROR) {
        print_error(dbg, "dwarf_get_vars", gvres, rose_dwarf_error);
    } else if (gvres == DW_DLV_NO_ENTRY) {
        /* no static vars */
    } else {
        Dwarf_Unsigned maxoff = get_info_max_offset(dbg);

        for (i = 0; i < count; i++) {
            int vnres;
            int cures3;
            Dwarf_Off global_cu_off = 0;

            vnres =
                dwarf_var_name_offsets(varbuf[i], &name, &die_off,
                                       &cu_off, &rose_dwarf_error);
            deal_with_name_offset_err(dbg,
                                      "dwarf_var_name_offsets",
                                      name, die_off, vnres, rose_dwarf_error);

            cures3 = dwarf_var_cu_offset(varbuf[i],
                                         &global_cu_off, &rose_dwarf_error);
            if (cures3 != DW_DLV_OK) {
                print_error(dbg, "dwarf_global_cu_offset", cures3, rose_dwarf_error);
            }

            print_pubname_style_entry(dbg,
                                      "static-var",
                                      name, die_off, cu_off,
                                      global_cu_off, maxoff);

            /* print associated die too? */
        }
        dwarf_vars_dealloc(dbg, varbuf, count);
    }
}                               /* print_static_vars */

// ************************************************************************

/* get all the data in .debug_types */
extern void
print_types(Dwarf_Debug dbg, enum type_type_e type_type)
{
    Dwarf_Type *typebuf = NULL;
    Dwarf_Signed count = 0;
    Dwarf_Signed i = 0;
    Dwarf_Off die_off = 0;
    Dwarf_Off cu_off = 0;
    char *name = NULL;
    int gtres = 0;

    char *section_name      = NULL;
    char *offset_err_name   = NULL;
    char *section_open_name = NULL;
    char *print_name_prefix = NULL;

    int (*get_types) (Dwarf_Debug, Dwarf_Type **, Dwarf_Signed *, Dwarf_Error *) = 0;
    int (*get_offset) (Dwarf_Type, char **, Dwarf_Off *, Dwarf_Off *, Dwarf_Error *) = NULL;
    int (*get_cu_offset) (Dwarf_Type, Dwarf_Off *, Dwarf_Error *) = NULL;
    void (*dealloctype) (Dwarf_Debug, Dwarf_Type *, Dwarf_Signed) = NULL;

    printf("\n.debug_types\n");

    if (type_type == DWARF_PUBTYPES) {
        section_name = ".debug_pubtypes";
        offset_err_name = "dwarf_pubtype_name_offsets";
        section_open_name = "dwarf_get_pubtypes";
        print_name_prefix = "pubtype";
        get_types = dwarf_get_pubtypes;
        get_offset = dwarf_pubtype_name_offsets;
        get_cu_offset = dwarf_pubtype_cu_offset;
        dealloctype = dwarf_pubtypes_dealloc;
    } else {
        /* SGI_TYPENAME */
        section_name = ".debug_typenames";
        offset_err_name = "dwarf_type_name_offsets";
        section_open_name = "dwarf_get_types";
        print_name_prefix = "type";
        get_types = dwarf_get_types;
        get_offset = dwarf_type_name_offsets;
        get_cu_offset = dwarf_type_cu_offset;
        dealloctype = dwarf_types_dealloc;
    }



    gtres = get_types(dbg, &typebuf, &count, &rose_dwarf_error);
    if (gtres == DW_DLV_ERROR) {
        print_error(dbg, section_open_name, gtres, rose_dwarf_error);
    } else if (gtres == DW_DLV_NO_ENTRY) {
        /* no types */
    } else {
        Dwarf_Unsigned maxoff = get_info_max_offset(dbg);

        /* Before July 2005, the section name was printed
           unconditionally, now only prints if non-empty section really 
           exists. */
        printf("\n%s\n", section_name);

        for (i = 0; i < count; i++) {
            int tnres;
            int cures3;
            Dwarf_Off global_cu_off = 0;

            tnres = get_offset(typebuf[i], &name, &die_off, &cu_off, &rose_dwarf_error);
            deal_with_name_offset_err(dbg, offset_err_name, name, die_off, tnres, rose_dwarf_error);

            cures3 = get_cu_offset(typebuf[i], &global_cu_off, &rose_dwarf_error);

            if (cures3 != DW_DLV_OK) {
                print_error(dbg, "dwarf_var_cu_offset", cures3, rose_dwarf_error);
            }
            print_pubname_style_entry(dbg,
                                      print_name_prefix,
                                      name, die_off, cu_off,
                                      global_cu_off, maxoff);

            /* print associated die too? */
        }
        dealloctype(dbg, typebuf, count);
    }
}                               /* print_types() */

// ************************************************************************

/* get all the data in .debug_weaknames */
extern void
print_weaknames(Dwarf_Debug dbg)
{
    Dwarf_Weak *weaknamebuf = NULL;
    Dwarf_Signed count = 0;
    Dwarf_Signed i = 0;
    Dwarf_Off die_off = 0;
    Dwarf_Off cu_off = 0;
    char *name = NULL;
    int wkres = 0;

    printf("\n.debug_weaknames\n");
    wkres = dwarf_get_weaks(dbg, &weaknamebuf, &count, &rose_dwarf_error);
    if (wkres == DW_DLV_ERROR) {
        print_error(dbg, "dwarf_get_weaks", wkres, rose_dwarf_error);
    } else if (wkres == DW_DLV_NO_ENTRY) {
        /* no weaknames */
    } else {
        Dwarf_Unsigned maxoff = get_info_max_offset(dbg);

        for (i = 0; i < count; i++) {
            int tnres;
            int cures3;

            Dwarf_Unsigned global_cu_off = 0;

            tnres = dwarf_weak_name_offsets(weaknamebuf[i],
                                            &name, &die_off, &cu_off,
                                            &rose_dwarf_error);
            deal_with_name_offset_err(dbg,"dwarf_weak_name_offsets",name, die_off, tnres, rose_dwarf_error);

            cures3 = dwarf_weak_cu_offset(weaknamebuf[i], &global_cu_off, &rose_dwarf_error);

            if (cures3 != DW_DLV_OK) {
                print_error(dbg, "dwarf_weakname_cu_offset",cures3, rose_dwarf_error);
            }
            print_pubname_style_entry(dbg,
                                      "weakname",
                                      name, die_off, cu_off,
                                      global_cu_off, maxoff);

            /* print associated die too? */
        }
        dwarf_weaks_dealloc(dbg, weaknamebuf, count);
    }
}                               /* print_weaknames() */


// ************************************************************************

/*  Print the 'right' string for the register we are given.
    Deal sensibly with the special regs as well as numbers
    we know and those we have not been told about.

*/
void
print_reg_from_config_data(Dwarf_Signed reg, struct dwconf_s *config_data)
{
    char *name = 0;

    if (reg == config_data->cf_cfa_reg) {
        fputs("cfa",stdout);
        return;
    }
    if (reg == DW_FRAME_CFA_COL3) {
        /* This should not be necessary, but sometimes one forgets to
           do the cfa_reg: command in dwarfdump.conf */
        fputs("cfa",stdout);
        return;
    }
    if (reg == DW_FRAME_UNDEFINED_VAL) {
        fputs("u",stdout);
        return;
    }
    if (reg == DW_FRAME_SAME_VAL) {
        fputs("s",stdout);
        return;
    }
    if (config_data->cf_regs == 0 ||
        reg < 0 || 
        reg >= config_data->cf_named_regs_table_size) {
        printf("r%lld", (signed long long) reg);
        return;
    }
    name = config_data->cf_regs[reg];
    if (!name) {
        /* Can happen, the reg names table can be sparse. */
        printf("r%lld", (signed long long) reg);
        return;
    }
    fputs(name,stdout);
    return;
}

/*
    decode ULEB
*/
Dwarf_Unsigned
local_dwarf_decode_u_leb128(unsigned char *leb128,
                            unsigned int *leb128_length)
{
    unsigned char byte = 0;
    Dwarf_Unsigned number = 0;
    unsigned int shift = 0;
    unsigned int byte_length = 1;

    byte = *leb128;
    for (;;) {
        number |= (byte & 0x7f) << shift;
        shift += 7;

        if ((byte & 0x80) == 0) {
            if (leb128_length != NULL)
                *leb128_length = byte_length;
            return (number);
        }

        byte_length++;
        byte = *(++leb128);
    }
}

#define BITSINBYTE 8
Dwarf_Signed
local_dwarf_decode_s_leb128(unsigned char *leb128,
                            unsigned int *leb128_length)
{
    Dwarf_Signed number = 0;
    Dwarf_Bool sign = 0;
    Dwarf_Signed shift = 0;
    unsigned char byte = *leb128;
    Dwarf_Signed byte_length = 1;

    /* byte_length being the number of bytes of data absorbed so far in
       turning the leb into a Dwarf_Signed. */

    for (;;) {
        sign = byte & 0x40;
        number |= ((Dwarf_Signed) ((byte & 0x7f))) << shift;
        shift += 7;

        if ((byte & 0x80) == 0) {
            break;
        }
        ++leb128;
        byte = *leb128;
        byte_length++;
    }

    if ((shift < sizeof(Dwarf_Signed) * BITSINBYTE) && sign) {
        number |= -((Dwarf_Signed) 1 << shift);
    }

    if (leb128_length != NULL)
        *leb128_length = byte_length;
    return (number);
}


/* Print our register names for the cases we have a name.
   Delegate to the configure code to actually do the print.
*/
void
printreg(Dwarf_Signed reg, struct dwconf_s *config_data)
{
    print_reg_from_config_data(reg, config_data);
}

/* Dumping a dwarf-expression as a byte stream. */
void
dump_block(char *prefix, char *data, Dwarf_Signed len)
{
    char *end_data = data + len;
    char *cur = data;
    int i = 0;

    printf("%s", prefix);
    for (; cur < end_data; ++cur, ++i) {
        if (i > 0 && i % 4 == 0)
            printf(" ");
        printf("%02x", 0xff & *cur);

    }
}


/* Print the frame instructions in detail for a glob of instructions.
*/

void
print_frame_inst_bytes(Dwarf_Debug dbg,
                       Dwarf_Ptr cie_init_inst, Dwarf_Signed len,
                       Dwarf_Signed data_alignment_factor,
                       int code_alignment_factor, Dwarf_Half addr_size,
                       struct dwconf_s *config_data)
{
    unsigned char *instp = (unsigned char *) cie_init_inst;
    Dwarf_Unsigned uval;
    Dwarf_Unsigned uval2;
    unsigned int uleblen;
    unsigned int off = 0;
    unsigned int loff = 0;
    unsigned short u16;
    unsigned int u32;
    unsigned long long u64;

    for (; len > 0;) {
        unsigned char ibyte = *instp;
        int top = ibyte & 0xc0;
        int bottom = ibyte & 0x3f;
        int delta;
        int reg;

        switch (top) {
        case DW_CFA_advance_loc:
            delta = ibyte & 0x3f;
            printf("\t%2u DW_CFA_advance_loc %d", off,
                   (int) (delta * code_alignment_factor));
            if (verbose) {
                printf("  (%d * %d)", (int) delta,
                       (int) code_alignment_factor);
            }
            printf("\n");
            break;
        case DW_CFA_offset:
            loff = off;
            reg = ibyte & 0x3f;
            uval = local_dwarf_decode_u_leb128(instp + 1, &uleblen);
            instp += uleblen;
            len -= uleblen;
            off += uleblen;
            printf("\t%2u DW_CFA_offset ", loff);
            printreg((Dwarf_Signed) reg, config_data);
            printf(" %lld", (signed long long)
                   (((Dwarf_Signed) uval) * data_alignment_factor));
            if (verbose) {
                printf("  (%llu * %d)", (unsigned long long) uval,
                       (int) data_alignment_factor);
            }
            printf("\n");
            break;

        case DW_CFA_restore:
            reg = ibyte & 0x3f;
            printf("\t%2u DW_CFA_restore \n", off);
            printreg((Dwarf_Signed) reg, config_data);
            printf("\n");
            break;

        default:
            loff = off;
            switch (bottom) {
            case DW_CFA_set_loc:
                /* operand is address, so need address size */
                /* which will be 4 or 8. */
                switch (addr_size) {
                case 4:
                    {
                        __uint32_t v32;

                        memcpy(&v32, instp + 1, addr_size);
                        uval = v32;
                    }
                    break;
                case 8:
                    {
                        __uint64_t v64;

                        memcpy(&v64, instp + 1, addr_size);
                        uval = v64;
                    }
                    break;
                default:
                    printf
                        ("Error: Unexpected address size %d in DW_CFA_set_loc!\n",
                         addr_size);
                    uval = 0;
                }

                instp += addr_size;
                len -= (Dwarf_Signed) addr_size;
                off += addr_size;
                printf("\t%2u DW_CFA_set_loc %llu\n",
                       loff, (unsigned long long) uval);
                break;
            case DW_CFA_advance_loc1:
                delta = (unsigned char) *(instp + 1);
                uval2 = delta;
                instp += 1;
                len -= 1;
                off += 1;
                printf("\t%2u DW_CFA_advance_loc1 %llu\n",
                       loff, (unsigned long long) uval2);
                break;
            case DW_CFA_advance_loc2:
                memcpy(&u16, instp + 1, 2);
                uval2 = u16;
                instp += 2;
                len -= 2;
                off += 2;
                printf("\t%2u DW_CFA_advance_loc2 %llu\n",
                       loff, (unsigned long long) uval2);
                break;
            case DW_CFA_advance_loc4:
                memcpy(&u32, instp + 1, 4);
                uval2 = u32;
                instp += 4;
                len -= 4;
                off += 4;
                printf("\t%2u DW_CFA_advance_loc4 %llu\n",
                       loff, (unsigned long long) uval2);
                break;
            case DW_CFA_MIPS_advance_loc8:
                memcpy(&u64, instp + 1, 8);
                uval2 = u64;
                instp += 8;
                len -= 8;
                off += 8;
                printf("\t%2u DW_CFA_MIPS_advance_loc8 %llu\n",
                       loff, (unsigned long long) uval2);
                break;
            case DW_CFA_offset_extended:
                uval = local_dwarf_decode_u_leb128(instp + 1, &uleblen);
                instp += uleblen;
                len -= uleblen;
                off += uleblen;
                uval2 =
                    local_dwarf_decode_u_leb128(instp + 1, &uleblen);
                instp += uleblen;
                len -= uleblen;
                off += uleblen;
                printf("\t%2u DW_CFA_offset_extended ", loff);
                printreg((Dwarf_Signed) uval, config_data);
                printf(" %lld", (signed long long)
                       (((Dwarf_Signed) uval2) *
                        data_alignment_factor));
                if (verbose) {
                    printf("  (%llu * %d)", (unsigned long long) uval2,
                           (int) data_alignment_factor);
                }
                printf("\n");
                break;

            case DW_CFA_restore_extended:
                uval = local_dwarf_decode_u_leb128(instp + 1, &uleblen);
                instp += uleblen;
                len -= uleblen;
                off += uleblen;
                printf("\t%2u DW_CFA_restore_extended ", loff);
                printreg((Dwarf_Signed) uval, config_data);
                printf("\n");
                break;
            case DW_CFA_undefined:
                uval = local_dwarf_decode_u_leb128(instp + 1, &uleblen);
                instp += uleblen;
                len -= uleblen;
                off += uleblen;
                printf("\t%2u DW_CFA_undefined ", loff);
                printreg((Dwarf_Signed) uval, config_data);
                printf("\n");
                break;
            case DW_CFA_same_value:
                uval = local_dwarf_decode_u_leb128(instp + 1, &uleblen);
                instp += uleblen;
                len -= uleblen;
                off += uleblen;
                printf("\t%2u DW_CFA_same_value ", loff);
                printreg((Dwarf_Signed) uval, config_data);
                printf("\n");
                break;
            case DW_CFA_register:
                uval = local_dwarf_decode_u_leb128(instp + 1, &uleblen);
                instp += uleblen;
                len -= uleblen;
                off += uleblen;
                uval2 =
                    local_dwarf_decode_u_leb128(instp + 1, &uleblen);
                instp += uleblen;
                len -= uleblen;
                off += uleblen;
                printf("\t%2u DW_CFA_register ", loff);
                printreg((Dwarf_Signed) uval, config_data);
                printf(" = ");
                printreg((Dwarf_Signed) uval2, config_data);
                printf("\n");
                break;
            case DW_CFA_remember_state:
                printf("\t%2u DW_CFA_remember_state\n", loff);
                break;
            case DW_CFA_restore_state:
                printf("\t%2u DW_CFA_restore_state\n", loff);
                break;
            case DW_CFA_def_cfa:
                uval = local_dwarf_decode_u_leb128(instp + 1, &uleblen);
                instp += uleblen;
                len -= uleblen;
                off += uleblen;
                uval2 =
                    local_dwarf_decode_u_leb128(instp + 1, &uleblen);
                instp += uleblen;
                len -= uleblen;
                off += uleblen;
                printf("\t%2u DW_CFA_def_cfa ", loff);
                printreg((Dwarf_Signed) uval, config_data);
                printf(" %llu", (unsigned long long) uval2);
                printf("\n");
                break;
            case DW_CFA_def_cfa_register:
                uval = local_dwarf_decode_u_leb128(instp + 1, &uleblen);
                instp += uleblen;
                len -= uleblen;
                off += uleblen;
                printf("\t%2u DW_CFA_def_cfa_register ", loff);
                printreg((Dwarf_Signed) uval, config_data);
                printf("\n");
                break;
            case DW_CFA_def_cfa_offset:
                uval = local_dwarf_decode_u_leb128(instp + 1, &uleblen);
                instp += uleblen;
                len -= uleblen;
                off += uleblen;
                printf("\t%2u DW_CFA_def_cfa_offset %llu\n",
                       loff, (unsigned long long) uval);
                break;

            case DW_CFA_nop:
                printf("\t%2u DW_CFA_nop\n", loff);
                break;

            case DW_CFA_def_cfa_expression:     /* DWARF3 */
                {
                    Dwarf_Unsigned block_len =
                        local_dwarf_decode_u_leb128(instp + 1,
                                                    &uleblen);

                    instp += uleblen;
                    len -= uleblen;
                    off += uleblen;
                    printf
                        ("\t%2u DW_CFA_def_cfa_expression expr block len %lld\n",
                         loff, (unsigned long long)
                         block_len);
                    dump_block("\t\t", (char *) instp+1,
                               (Dwarf_Signed) block_len);
                    printf("\n");
                    if(verbose) {
#if 1
                      printf ("Use C++ string instead, not implemented! \n");
                      ROSE_ASSERT(false);
#else
                      struct esb_s exprstring;
                      esb_constructor(&exprstring);
                      get_string_from_locs(dbg,
                            instp+1,block_len,&exprstring);
                      printf("\t\t%s\n",esb_get_string(&exprstring));
                      esb_destructor(&exprstring);
#endif
                    }
                    instp += block_len;
                    len -= block_len;
                    off += block_len;
                }
                break;
            case DW_CFA_expression:     /* DWARF3 */
                uval = local_dwarf_decode_u_leb128(instp + 1, &uleblen);
                instp += uleblen;
                len -= uleblen;
                off += uleblen;
                {
                    /* instp is always 1 byte back, so we need +1
                        when we use it. See the final increment
                        of this for loop. */
                    Dwarf_Unsigned block_len =
                        local_dwarf_decode_u_leb128(instp + 1,
                                                    &uleblen);

                    instp += uleblen;
                    len -= uleblen;
                    off += uleblen;
                    printf
                        ("\t%2u DW_CFA_expression %llu expr block len %lld\n",
                         loff, (unsigned long long) uval,
                         (unsigned long long)
                         block_len);
                    dump_block("\t\t", (char *) instp+1,
                               (Dwarf_Signed) block_len);
                    printf("\n");
                    if(verbose) {
#if 1
                      printf ("Use C++ string instead, not implemented! \n");
                      ROSE_ASSERT(false);
#else
                      struct esb_s exprstring;
                      esb_constructor(&exprstring);
                      get_string_from_locs(dbg,
                            instp+1,block_len,&exprstring);
                      printf("\t\t%s\n",esb_get_string(&exprstring));
                      esb_destructor(&exprstring);
#endif
                    }
                    instp += block_len;
                    len -= block_len;
                    off += block_len;
                }

                break;
            case DW_CFA_cfa_offset_extended_sf: /* DWARF3 */
                uval = local_dwarf_decode_u_leb128(instp + 1, &uleblen);
                instp += uleblen;
                len -= uleblen;
                off += uleblen;
                {
                    /* instp is always 1 byte back, so we need +1
                        when we use it. See the final increment
                        of this for loop. */
                    Dwarf_Signed sval2 =
                        local_dwarf_decode_s_leb128(instp + 1,
                                                    &uleblen);

                    instp += uleblen;
                    len -= uleblen;
                    off += uleblen;
                    printf("\t%2u DW_CFA_offset_extended_sf ", loff);
                    printreg((Dwarf_Signed) uval, config_data);
                    printf(" %lld", (signed long long)
                           ((sval2) * data_alignment_factor));
                    if (verbose) {
                        printf("  (%lld * %d)", (long long) sval2,
                               (int) data_alignment_factor);
                    }
                }
                printf("\n");
                break;
            case DW_CFA_def_cfa_sf:     /* DWARF3 */
                    /* instp is always 1 byte back, so we need +1
                        when we use it. See the final increment
                        of this for loop. */
                uval = local_dwarf_decode_u_leb128(instp + 1, &uleblen);
                instp += uleblen;
                len -= uleblen;
                off += uleblen;
                {
                    Dwarf_Signed sval2 =
                        local_dwarf_decode_s_leb128(instp + 1,
                                                    &uleblen);

                    instp += uleblen;
                    len -= uleblen;
                    off += uleblen;
                    printf("\t%2u DW_CFA_def_cfa_sf ", loff);
                    printreg((Dwarf_Signed) uval, config_data);
                    printf(" %lld", (long long) sval2); 
                    printf(" (*data alignment factor=>%lld)",
                     (long long)(sval2*data_alignment_factor));
                }
                printf("\n");
                break;
            case DW_CFA_def_cfa_offset_sf:      /* DWARF3 */
                {
                    /* instp is always 1 byte back, so we need +1
                        when we use it. See the final increment
                        of this for loop. */
                    Dwarf_Signed sval =
                        local_dwarf_decode_s_leb128(instp + 1,
                                                    &uleblen);

                    instp += uleblen;
                    len -= uleblen;
                    off += uleblen;
                    printf("\t%2u DW_CFA_def_cfa_offset_sf %lld (*data alignment factor=> %lld)\n",
                           loff, (long long) sval,
                           (long long)(data_alignment_factor*sval));

                }
                break;
            case DW_CFA_val_offset:     /* DWARF3 */
                    /* instp is always 1 byte back, so we need +1
                        when we use it. See the final increment
                        of this for loop. */
                uval = local_dwarf_decode_u_leb128(instp + 1, &uleblen);
                instp += uleblen;
                len -= uleblen;
                off += uleblen;
                {
                    uval2 =
                        local_dwarf_decode_s_leb128(instp + 1,
                                                    &uleblen);
                    instp += uleblen;
                    len -= uleblen;
                    off += uleblen;
                    printf("\t%2u DW_CFA_val_offset ", loff);
                    printreg((Dwarf_Signed) uval, config_data);
                    printf(" %lld", (unsigned long long)
                           (((Dwarf_Signed) uval2) *
                            data_alignment_factor));
                    if (verbose) {
                        printf("  (%lld * %d)", (long long) uval2,
                               (int) data_alignment_factor);
                    }
                }
                printf("\n");

                break;
            case DW_CFA_val_offset_sf:  /* DWARF3 */
                    /* instp is always 1 byte back, so we need +1
                        when we use it. See the final increment
                        of this for loop. */
                uval = local_dwarf_decode_u_leb128(instp + 1, &uleblen);
                instp += uleblen;
                len -= uleblen;
                off += uleblen;
                {
                    Dwarf_Signed sval2 =
                        local_dwarf_decode_s_leb128(instp + 1,
                                                    &uleblen);

                    instp += uleblen;
                    len -= uleblen;
                    off += uleblen;
                    printf("\t%2u DW_CFA_val_offset_sf ", loff);
                    printreg((Dwarf_Signed) uval, config_data);
                    printf(" %lld", (signed long long)
                           ((sval2) * data_alignment_factor));
                    if (verbose) {
                        printf("  (%lld * %d)", (long long) sval2,
                               (int) data_alignment_factor);
                    }
                }
                printf("\n");

                break;
            case DW_CFA_val_expression: /* DWARF3 */
                    /* instp is always 1 byte back, so we need +1
                        when we use it. See the final increment
                        of this for loop. */
                uval = local_dwarf_decode_u_leb128(instp + 1, &uleblen);
                instp += uleblen;
                len -= uleblen;
                off += uleblen;
                {
                    Dwarf_Unsigned block_len =
                        local_dwarf_decode_u_leb128(instp + 1,
                                                    &uleblen);

                    instp += uleblen;
                    len -= uleblen;
                    off += uleblen;
                    printf
                        ("\t%2u DW_CFA_val_expression %llu expr block len %lld\n",
                         loff, (unsigned long long) uval,
                         (unsigned long long)
                         block_len);
                    dump_block("\t\t", (char *) instp+1,
                               (Dwarf_Signed) block_len);
                    printf("\n");
                    if(verbose) {
#if 1
                      printf ("Use C++ string instead, not implemented! \n");
                      ROSE_ASSERT(false);
#else
                      struct esb_s exprstring;
                      esb_constructor(&exprstring);
                      get_string_from_locs(dbg,
                            instp+1,block_len,&exprstring);
                      printf("\t\t%s\n",esb_get_string(&exprstring));
                      esb_destructor(&exprstring);
#endif
                    }
                    instp += block_len;
                    len -= block_len;
                    off += block_len;
                }


                break;


#ifdef DW_CFA_GNU_window_save
            case DW_CFA_GNU_window_save:{
                    /* no information: this just tells unwinder to
                       restore the window registers from the previous
                       frame's window save area */
                    printf("\t%2u DW_CFA_GNU_window_save \n", loff);
                    break;
                }
#endif
#ifdef DW_CFA_GNU_negative_offset_extended
            case DW_CFA_GNU_negative_offset_extended:{
                    printf
                        ("\t%2u DW_CFA_GNU_negative_offset_extended \n",
                         loff);
                }
#endif
#ifdef  DW_CFA_GNU_args_size
                /* single uleb128 is the current arg area size in
                   bytes. no register exists yet to save this in */
            case DW_CFA_GNU_args_size:{
                    Dwarf_Unsigned lreg;

                    /* instp is always 1 byte back, so we need +1
                        when we use it. See the final increment
                        of this for loop. */
                    lreg =
                        local_dwarf_decode_u_leb128(instp + 1,
                                                    &uleblen);
                    printf
                        ("\t%2u DW_CFA_GNU_args_size arg size: %llu\n",
                         loff, (unsigned long long) lreg);
                    instp += uleblen;
                    len -= uleblen;
                    off += uleblen;

                    break;
                }
#endif

            default:
                printf("\t%u Unexpected op 0x%x: \n",
                       loff, (unsigned int) bottom);
                len = 0;
                break;
            }
        }
        instp++;
        len--;
        off++;
    }
}

/*
   Actually does the printing of a rule in the table.
   This may print something or may print nothing!
*/

static void
print_one_frame_reg_col(Dwarf_Debug dbg,
                        Dwarf_Unsigned rule_id,
                        Dwarf_Small value_type,
                        Dwarf_Unsigned reg_used,
                        struct dwconf_s *config_data,
                        Dwarf_Signed offset_relevant,
                        Dwarf_Signed offset, 
                        Dwarf_Ptr block_ptr)
{
    char *type_title = "";
    int print_type_title = 1;

    if (config_data->cf_interface_number == 2)
        print_type_title = 0;

    switch (value_type) {
    case DW_EXPR_OFFSET:
        type_title = "off";
        goto preg2;
    case DW_EXPR_VAL_OFFSET:
        type_title = "valoff";
      preg2:
        if (reg_used == config_data->cf_initial_rule_value) {
            break;
        }
        if (print_type_title)
            printf("<%s ", type_title);
        printreg((Dwarf_Signed) rule_id, config_data);
        printf("=");
        if (offset_relevant == 0) {
            printreg((Dwarf_Signed) reg_used, config_data);
            printf(" ");
        } else {
            printf("%02lld", offset);
            printf("(");
            printreg((Dwarf_Signed) reg_used, config_data);
            printf(") ");
        }
        if (print_type_title)
            printf("%s", "> ");
        break;
    case DW_EXPR_EXPRESSION:
        type_title = "expr";
        goto pexp2;
    case DW_EXPR_VAL_EXPRESSION:
        type_title = "valexpr";
      pexp2:
        if (print_type_title)
            printf("<%s ", type_title);
        printreg((Dwarf_Signed) rule_id, config_data);
        printf("=");
        printf("expr-block-len=%lld", (long long) offset);
        if (print_type_title)
            printf("%s", "> ");
        if (verbose) {
            char pref[40];

            strcpy(pref, "<");
            strcat(pref, type_title);
            strcat(pref, "bytes:");
            dump_block(pref, (char*) block_ptr, offset);
            printf("%s", "> ");
            if(verbose) {
#if 1
                      printf ("This should use the C++ string class, not implemented yet! \n");
                      ROSE_ASSERT(false);
#else
                      struct esb_s exprstring;
                      esb_constructor(&exprstring);
                      get_string_from_locs(dbg,
                            block_ptr,offset,&exprstring);
                      printf("<expr:%s>",esb_get_string(&exprstring));
                      esb_destructor(&exprstring);
#endif
            }
        }
        break;
    default:
        printf("Internal error in libdwarf, value type %d\n",
               value_type);
        exit(1);
    }
    return;
}

/*

A strcpy which ensures NUL terminated string
and never overruns the output.

*/
static void
safe_strcpy(char *out, long outlen, char *in, long inlen)
{
    if (inlen >= (outlen - 1)) {
        strncpy(out, in, outlen - 1);
        out[outlen - 1] = 0;
    } else {
        strcpy(out, in);
    }
}


/*
        Returns 1 if a proc with this low_pc found.
        Else returns 0.


*/
int
get_proc_name(Dwarf_Debug dbg, Dwarf_Die die, Dwarf_Addr low_pc,char *proc_name_buf, int proc_name_buf_len)
{
    Dwarf_Signed atcnt = 0;
    Dwarf_Signed i = 0;
    Dwarf_Attribute *atlist = NULL;
    Dwarf_Addr low_pc_die = 0;
    int atres = 0;
    int funcres = 1;
    int funcpcfound = 0;
    int funcnamefound = 1;

    proc_name_buf[0] = 0;       /* always set to something */
    atres = dwarf_attrlist(die, &atlist, &atcnt, &rose_dwarf_error);
    if (atres == DW_DLV_ERROR) {
        print_error(dbg, "dwarf_attrlist", atres, rose_dwarf_error);
        return 0;
    }
    if (atres == DW_DLV_NO_ENTRY) {
        return 0;
    }
    for (i = 0; i < atcnt; i++) {
        Dwarf_Half attr;
        int ares;
        char* temps;
        int sres;
        int dres;

        if (funcnamefound == 1 && funcpcfound == 1) {
            /* stop as soon as both found */
            break;
        }
        ares = dwarf_whatattr(atlist[i], &attr, &rose_dwarf_error);
        if (ares == DW_DLV_ERROR) {
            print_error(dbg, "get_proc_name whatattr error", ares, rose_dwarf_error);
        } else if (ares == DW_DLV_OK) {
            switch (attr) {
            case DW_AT_name:
                sres = dwarf_formstring(atlist[i], &temps, &rose_dwarf_error);
                if (sres == DW_DLV_ERROR) {
                    print_error(dbg,"formstring in get_proc_name failed",sres, rose_dwarf_error);
                    /* 50 is safe wrong length since is bigger than the 
                       actual string */
                    safe_strcpy(proc_name_buf, proc_name_buf_len,"ERROR in dwarf_formstring!", 50);
                } else if (sres == DW_DLV_NO_ENTRY) {
                    /* 50 is safe wrong length since is bigger than the 
                       actual string */
                    safe_strcpy(proc_name_buf, proc_name_buf_len,"NO ENTRY on dwarf_formstring?!", 50);
                } else {
                    long len = (long) strlen(temps);

                    safe_strcpy(proc_name_buf, proc_name_buf_len, temps, len);
                }
                funcnamefound = 1;      /* FOUND THE NAME */
                break;
            case DW_AT_low_pc:
                dres = dwarf_formaddr(atlist[i], &low_pc_die, &rose_dwarf_error);
                if (dres == DW_DLV_ERROR) {
                    print_error(dbg, "formaddr in get_proc_name failed", dres, rose_dwarf_error);
                    low_pc_die = ~low_pc;
                    /* ensure no match */
                }
                funcpcfound = 1;

                break;
            default:
                break;
            }
        }
    }

    for (i = 0; i < atcnt; i++) {
        dwarf_dealloc(dbg, atlist[i], DW_DLA_ATTR);
    }
    dwarf_dealloc(dbg, atlist, DW_DLA_LIST);
    if (funcnamefound == 0 || funcpcfound == 0 || low_pc != low_pc_die) {
        funcres = 0;
    }
    return (funcres);
}



/*
        Modified Depth First Search looking for the procedure:
        a) only looks for children of subprogram.
        b) With subprogram looks at current die *before* looking
           for a child.
        
        Needed since some languages, including MP Fortran,
        have nested functions.
        Return 0 on failure, 1 on success.
*/
static int
get_nested_proc_name(Dwarf_Debug dbg, Dwarf_Die die, Dwarf_Addr low_pc,
                     char *ret_name_buf, int ret_name_buf_len)
{
    char name_buf[BUFSIZ];
    Dwarf_Die curdie = die;
    int die_locally_gotten = 0;
    Dwarf_Die prev_child = 0;
    Dwarf_Die newchild = 0;
    Dwarf_Die newsibling = 0;
    Dwarf_Half tag;
    Dwarf_Error err = 0;
    int chres = DW_DLV_OK;

    ret_name_buf[0] = 0;
    while (chres == DW_DLV_OK) {
        int tres;

        tres = dwarf_tag(curdie, &tag, &err);
        newchild = 0;
        err = 0;
        if (tres == DW_DLV_OK) {
            int lchres;

            if (tag == DW_TAG_subprogram) {
                int proc_name_v;

                proc_name_v = get_proc_name(dbg, curdie, low_pc,name_buf, BUFSIZ);
                if (proc_name_v) {
                    /* this is it */
                    safe_strcpy(ret_name_buf, ret_name_buf_len,name_buf, (long) strlen(name_buf));
                    if (die_locally_gotten) {
                        /* If we got this die from the parent, we do
                           not want to dealloc here! */
                        dwarf_dealloc(dbg, curdie, DW_DLA_DIE);
                    }
                    return 1;
                }
                /* check children of subprograms recursively should
                   this really be check children of anything? */

                lchres = dwarf_child(curdie, &newchild, &rose_dwarf_error);
                if (lchres == DW_DLV_OK) {
                    /* look for inner subprogram */
                    int newprog =
                        get_nested_proc_name(dbg, newchild, low_pc,name_buf, BUFSIZ);

                    dwarf_dealloc(dbg, newchild, DW_DLA_DIE);
                    if (newprog) {
                        /* Found it.  We could just take this name or
                           we could concatenate names together For now, 
                           just take name */
                        if (die_locally_gotten) {
                            /* If we got this die from the parent, we
                               do not want to dealloc here! */
                            dwarf_dealloc(dbg, curdie, DW_DLA_DIE);
                        }
                        safe_strcpy(ret_name_buf, ret_name_buf_len,name_buf, (long) strlen(name_buf));
                        return 1;
                    }
                } else if (lchres == DW_DLV_NO_ENTRY) {
                    /* nothing to do */
                } else {
                    print_error(dbg,
                                "get_nested_proc_name dwarf_child() failed ",chres, rose_dwarf_error);
                    if (die_locally_gotten) {
                        /* If we got this die from the parent, we do
                           not want to dealloc here! */
                        dwarf_dealloc(dbg, curdie, DW_DLA_DIE);
                    }
                    return 0;
                }
            }                   /* end if TAG_subprogram */
        } else {
            print_error(dbg, "no tag on child read ", tres, rose_dwarf_error);
            if (die_locally_gotten) {
                /* If we got this die from the parent, we do not want
                   to dealloc here! */
                dwarf_dealloc(dbg, curdie, DW_DLA_DIE);
            }
            return 0;
        }
        /* try next sibling */
        prev_child = curdie;
        chres = dwarf_siblingof(dbg, curdie, &newsibling, &rose_dwarf_error);
        if (chres == DW_DLV_ERROR) {
            print_error(dbg, "dwarf_cu_header On Child read ", chres,
                        err);
            if (die_locally_gotten) {
                /* If we got this die from the parent, we do not want
                   to dealloc here! */
                dwarf_dealloc(dbg, curdie, DW_DLA_DIE);
            }
            return 0;
        } else if (chres == DW_DLV_NO_ENTRY) {
            if (die_locally_gotten) {
                /* If we got this die from the parent, we do not want
                   to dealloc here! */
                dwarf_dealloc(dbg, prev_child, DW_DLA_DIE);
            }
            return 0;           /* proc name not at this level */
        } else {                /* DW_DLV_OK */
            curdie = newsibling;
            if (die_locally_gotten) {
                /* If we got this die from the parent, we do not want
                   to dealloc here! */
                dwarf_dealloc(dbg, prev_child, DW_DLA_DIE);
            }
            prev_child = 0;
            die_locally_gotten = 1;
        }

    }
    if (die_locally_gotten) {
        /* If we got this die from the parent, we do not want to
           dealloc here! */
        dwarf_dealloc(dbg, curdie, DW_DLA_DIE);
    }
    return 0;
}


/*
  For MP Fortran and possibly other languages, functions 
  nest!  As a result, we must dig thru all functions, 
  not just the top level.


  This remembers the CU die and restarts each search at the start
  of  the current cu.

  
*/
string
get_fde_proc_name(Dwarf_Debug dbg, Dwarf_Addr low_pc)
{
    static char proc_name[BUFSIZ];
    Dwarf_Unsigned cu_header_length = 0;
    Dwarf_Unsigned abbrev_offset = 0;
    Dwarf_Half version_stamp = 0;
    Dwarf_Half address_size = 0;
    Dwarf_Unsigned next_cu_offset = 0;
    int cures = DW_DLV_OK;
    int dres = DW_DLV_OK;
    int chres = DW_DLV_OK;
    int looping = 0;

    if (current_cu_die_for_print_frames == NULL) {
        /* Call depends on dbg->cu_context to know what to do. */
        cures = dwarf_next_cu_header(dbg, &cu_header_length,
                                   &version_stamp, &abbrev_offset,
                                   &address_size, &next_cu_offset,
                                   &rose_dwarf_error);
        if (cures == DW_DLV_ERROR) {
            return NULL;
        } else if (cures == DW_DLV_NO_ENTRY) {
            /* loop thru the list again */
            current_cu_die_for_print_frames = 0;
            ++looping;
        } else {                /* DW_DLV_OK */
            dres = dwarf_siblingof(dbg, NULL,
                                   &current_cu_die_for_print_frames,&rose_dwarf_error);
            if (dres == DW_DLV_ERROR) {
                return NULL;
            }
        }
    }
    if (dres == DW_DLV_OK) {
        Dwarf_Die child = 0;

        if (current_cu_die_for_print_frames == 0) {
            /* no information. Possibly a stripped file */
            return NULL;
        }
        chres = dwarf_child(current_cu_die_for_print_frames, &child, &rose_dwarf_error);
        if (chres == DW_DLV_ERROR) {
            print_error(dbg, "dwarf_cu_header on child read ", chres,rose_dwarf_error);
        } else if (chres == DW_DLV_NO_ENTRY) {
        } else {                /* DW_DLV_OK */
            int gotname = get_nested_proc_name(dbg, child, low_pc, proc_name,BUFSIZ);

            dwarf_dealloc(dbg, child, DW_DLA_DIE);
            if (gotname) {
                return (proc_name);
            }
            child = 0;
        }
    }
    for (;;) {
        Dwarf_Die ldie;

        cures = dwarf_next_cu_header(dbg, &cu_header_length,
                                     &version_stamp, &abbrev_offset,
                                     &address_size, &next_cu_offset,
                                     &rose_dwarf_error);

        if (cures != DW_DLV_OK) {
            break;
        }


        dres = dwarf_siblingof(dbg, NULL, &ldie, &rose_dwarf_error);

        if (current_cu_die_for_print_frames) {
            dwarf_dealloc(dbg, current_cu_die_for_print_frames,
                          DW_DLA_DIE);
        }
        current_cu_die_for_print_frames = 0;
        if (dres == DW_DLV_ERROR) {
            print_error(dbg,
                        "dwarf_cu_header Child Read finding proc name for .debug_frame",
                        chres, rose_dwarf_error);
            continue;
        } else if (dres == DW_DLV_NO_ENTRY) {
            ++looping;
            if (looping > 1) {
                print_error(dbg, "looping  on cu headers!", dres, rose_dwarf_error);
                return NULL;
            }
            continue;
        }
        /* DW_DLV_OK */
        current_cu_die_for_print_frames = ldie;
        {
            int chres;
            Dwarf_Die child;

            chres = dwarf_child(current_cu_die_for_print_frames, &child,&rose_dwarf_error);
            if (chres == DW_DLV_ERROR) {
                print_error(dbg, "dwarf Child Read ", chres, rose_dwarf_error);
            } else if (chres == DW_DLV_NO_ENTRY) {

                ;               /* do nothing, loop on cu */
            } else {            /* DW_DLV_OK) */

                int gotname = get_nested_proc_name(dbg, child, low_pc, proc_name,BUFSIZ);

                dwarf_dealloc(dbg, child, DW_DLA_DIE);
                if (gotname) {
                    return (proc_name);
                }
            }
        }
    }
    return (NULL);
}

/*
    Gather the fde print logic here so the control logic
    determining what FDE to print is clearer.
*/
int
print_one_fde(Dwarf_Debug dbg, Dwarf_Fde fde,
              Dwarf_Unsigned fde_index,
              Dwarf_Cie * cie_data,
              Dwarf_Signed cie_element_count,
              Dwarf_Half address_size, int is_eh,
              struct dwconf_s *config_data)
{
    Dwarf_Addr j = 0;
    Dwarf_Addr low_pc = 0;
    Dwarf_Unsigned func_length = 0;
    Dwarf_Ptr fde_bytes = NULL;
    Dwarf_Unsigned fde_bytes_length = 0;
    Dwarf_Off cie_offset = 0;
    Dwarf_Signed cie_index = 0;
    Dwarf_Off fde_offset = 0;
    Dwarf_Signed eh_table_offset = 0;
    int fres = 0;
    int offres = 0;
    string temps;
    Dwarf_Error err = 0;
    int printed_intro_addr = 0;

    fres = dwarf_get_fde_range(fde,
                               &low_pc, &func_length,
                               &fde_bytes,
                               &fde_bytes_length,
                               &cie_offset, &cie_index,
                               &fde_offset, &rose_dwarf_error);
    if (fres == DW_DLV_ERROR) {
        print_error(dbg, "dwarf_get_fde_range", fres, rose_dwarf_error);
    }
    if (fres == DW_DLV_NO_ENTRY) {
        return DW_DLV_NO_ENTRY;
    }
    if (cu_name_flag &&
        fde_offset_for_cu_low != DW_DLV_BADOFFSET &&
        (fde_offset < fde_offset_for_cu_low ||
         fde_offset > fde_offset_for_cu_high)) {
        return DW_DLV_NO_ENTRY;
    }
    /* eh_table_offset is IRIX ONLY. */
    fres = dwarf_get_fde_exception_info(fde, &eh_table_offset, &rose_dwarf_error);
    if (fres == DW_DLV_ERROR) {
        print_error(dbg, "dwarf_get_fde_exception_info", fres, rose_dwarf_error);
    }
    if(suppress_nested_name_search) {
        temps = "";
    } else {
        temps = get_fde_proc_name(dbg, low_pc);
    }
    printf
        ("<%3lld><%#llx:%#llx><%s><fde offset 0x%llx length: 0x%llx>",
         cie_index, low_pc, (low_pc + func_length),
         (temps.empty() == false) ? temps : "", fde_offset, fde_bytes_length);

    if (!is_eh) {
        /* IRIX uses eh_table_offset. */
        if (eh_table_offset == DW_DLX_NO_EH_OFFSET) {
            printf("<eh offset %s>\n", "none");
        } else if (eh_table_offset == DW_DLX_EH_OFFSET_UNAVAILABLE) {
            printf("<eh offset %s>\n", "unknown");
        } else {
            printf("<eh offset 0x%llx>\n", eh_table_offset);
        }
    } else {
        int ares = 0;
        Dwarf_Small *data = 0;
        Dwarf_Unsigned len = 0;

        ares = dwarf_get_fde_augmentation_data(fde, &data, &len, &rose_dwarf_error);
        if (ares == DW_DLV_NO_ENTRY) {
            /* do nothing. */
        } else if (ares == DW_DLV_OK) {
            int k2;

            printf("<eh aug data len 0x%llx", (long long) len);
            for (k2 = 0; k2 < len; ++k2) {
                if (k2 == 0) {
                    printf(" bytes 0x");
                }
                printf("%02x ", (unsigned char) data[k2]);
            }
            printf(">");
        }                       /* else DW_DLV_ERROR, do nothing */
        printf("\n");
    }
    /* call dwarf_get_fde_info_for_reg() to get whole matrix */

    for (j = low_pc; j < low_pc + func_length; j++) {
        Dwarf_Half k;

        if (config_data->cf_interface_number == 3) {
            Dwarf_Signed reg = 0;
            Dwarf_Signed offset_relevant = 0;
            Dwarf_Small value_type = 0;
            Dwarf_Signed offset_or_block_len = 0;
            Dwarf_Signed offset = 0;
            Dwarf_Ptr block_ptr = 0;
            Dwarf_Addr row_pc = 0;

            int fires = dwarf_get_fde_info_for_cfa_reg3(fde,
                                                        j,
                                                        &value_type,
                                                        &offset_relevant,
                                                        &reg,
                                                        &offset_or_block_len,
                                                        &block_ptr,
                                                        &row_pc,
                                                        &rose_dwarf_error);

            offset = offset_or_block_len;
            if (fires == DW_DLV_ERROR) {
                print_error(dbg,
                            "dwarf_get_fde_info_for_reg", fires, rose_dwarf_error);
            }
            if (fires == DW_DLV_NO_ENTRY) {
                continue;
            }
            if (row_pc != j) {
                /* duplicate row */
                continue;
            }
            if (!printed_intro_addr) {
                printf("    %08llx:\t", j);
                printed_intro_addr = 1;
            }
            print_one_frame_reg_col(dbg, config_data->cf_cfa_reg,
                                    value_type,
                                    reg,
                                    config_data,
                                    offset_relevant, offset, block_ptr);
        }
        for (k = 0; k < config_data->cf_table_entry_count; k++) {
            Dwarf_Signed reg = 0;
            Dwarf_Signed offset_relevant = 0;
            int fires = 0;
            Dwarf_Small value_type = 0;
            Dwarf_Ptr block_ptr = 0;
            Dwarf_Signed offset_or_block_len = 0;
            Dwarf_Signed offset = 0;
            Dwarf_Addr row_pc = 0;

            if (config_data->cf_interface_number == 3) {

                fires = dwarf_get_fde_info_for_reg3(fde,
                                                    k,
                                                    j,
                                                    &value_type,
                                                    &offset_relevant,
                                                    &reg,
                                                    &offset_or_block_len,
                                                    &block_ptr,
                                                    &row_pc, &rose_dwarf_error);
                offset = offset_or_block_len;
            } else {            /* ASSERT:
                                   config_data->cf_interface_number ==
                                   2 */


                value_type = DW_EXPR_OFFSET;
                fires = dwarf_get_fde_info_for_reg(fde,
                                                   k,
                                                   j,
                                                   &offset_relevant,
                                                   &reg,
                                                   &offset, &row_pc,
                                                   &rose_dwarf_error);
            }
            if (fires == DW_DLV_ERROR) {
                printf("\n");
                print_error(dbg,
                            "dwarf_get_fde_info_for_reg", fires, rose_dwarf_error);
            }
            if (fires == DW_DLV_NO_ENTRY) {
                continue;
            }
            if (row_pc != j) {
                /* duplicate row */
                break;
            }
            if (!printed_intro_addr) {
                printf("    %08llx:\t", j);
                printed_intro_addr = 1;
            }
            print_one_frame_reg_col(dbg,k,
                                    value_type,
                                    reg,
                                    config_data,
                                    offset_relevant, offset, block_ptr);

        }
        if (printed_intro_addr) {
            printf("\n");
            printed_intro_addr = 0;
        }
    }
    if (verbose > 1) {
        Dwarf_Off fde_off;
        Dwarf_Off cie_off;

        /* get the fde instructions and print them in raw form, just
           like cie instructions */
        Dwarf_Ptr instrs;
        Dwarf_Unsigned ilen;
        int res;

        res = dwarf_get_fde_instr_bytes(fde, &instrs, &ilen, &rose_dwarf_error);
        offres =
            dwarf_fde_section_offset(dbg, fde, &fde_off, &cie_off,
                                      &rose_dwarf_error);
        if (offres == DW_DLV_OK) {
            printf("\tfde sec. offset %llu 0x%llx"
                   " cie offset for fde: %llu 0x%llx\n",
                   (unsigned long long) fde_off,
                   (unsigned long long) fde_off,
                   (unsigned long long) cie_off,
                   (unsigned long long) cie_off);

        }


        if (res == DW_DLV_OK) {
            int cires = 0;
            Dwarf_Unsigned cie_length = 0;
            Dwarf_Small version = 0;
            char* augmenter;
            Dwarf_Unsigned code_alignment_factor = 0;
            Dwarf_Signed data_alignment_factor = 0;
            Dwarf_Half return_address_register_rule = 0;
            Dwarf_Ptr initial_instructions = 0;
            Dwarf_Unsigned initial_instructions_length = 0;

            if (cie_index >= cie_element_count) {
                printf("Bad cie index %lld with fde index %lld! "
                       "(table entry max %lld)\n",
                       (long long) cie_index, (long long) fde_index,
                       (long long) cie_element_count);
                exit(1);
            }

            cires = dwarf_get_cie_info(cie_data[cie_index],
                                       &cie_length,
                                       &version,
                                       &augmenter,
                                       &code_alignment_factor,
                                       &data_alignment_factor,
                                       &return_address_register_rule,
                                       &initial_instructions,
                                       &initial_instructions_length,
                                       &rose_dwarf_error);
            if (cires == DW_DLV_ERROR) {
                printf
                    ("Bad cie index %lld with fde index %lld!\n",
                     (long long) cie_index, (long long) fde_index);
                print_error(dbg, "dwarf_get_cie_info", cires, rose_dwarf_error);
            }
            if (cires == DW_DLV_NO_ENTRY) {
                ;               /* ? */
            } else {

                print_frame_inst_bytes(dbg, instrs,
                                       (Dwarf_Signed) ilen,
                                       data_alignment_factor,
                                       (int) code_alignment_factor,
                                       address_size, config_data);
            }
        } else if (res == DW_DLV_NO_ENTRY) {
            printf
                ("Impossible: no instr bytes for fde index %d?\n",
                 (int) fde_index);
        } else {
            /* DW_DLV_ERROR */
            printf
                ("Error: on gettinginstr bytes for fde index %d?\n",
                 (int) fde_index);
            print_error(dbg, "dwarf_get_fde_instr_bytes", res, rose_dwarf_error);
        }

    }
    return DW_DLV_OK;
}

/* Print a cie.  Gather the print logic here so the
   control logic deciding what to print
   is clearer.
*/
int
print_one_cie(Dwarf_Debug dbg, Dwarf_Cie cie,
              Dwarf_Unsigned cie_index, Dwarf_Half address_size,
              struct dwconf_s *config_data)
{

    int cires = 0;
    Dwarf_Unsigned cie_length = 0;
    Dwarf_Small version = 0;
    char* augmenter = "";
    Dwarf_Unsigned code_alignment_factor = 0;
    Dwarf_Signed data_alignment_factor = 0;
    Dwarf_Half return_address_register_rule = 0;
    Dwarf_Ptr initial_instructions = 0;
    Dwarf_Unsigned initial_instructions_length = 0;
    Dwarf_Off cie_off = 0;
    Dwarf_Error err = 0;

    cires = dwarf_get_cie_info(cie,
                               &cie_length,
                               &version,
                               &augmenter,
                               &code_alignment_factor,
                               &data_alignment_factor,
                               &return_address_register_rule,
                               &initial_instructions,
                               &initial_instructions_length, &err);
    if (cires == DW_DLV_ERROR) {
        print_error(dbg, "dwarf_get_cie_info", cires, err);
    }
    if (cires == DW_DLV_NO_ENTRY) {
        ;                       /* ? */
        printf("Impossible DW_DLV_NO_ENTRY on cie %d\n",
               (int) cie_index);
        return DW_DLV_NO_ENTRY;
    }
    {
        printf("<%3lld>\tversion\t\t\t\t%d\n", cie_index, version);
        cires = dwarf_cie_section_offset(dbg, cie, &cie_off, &err);
        if (cires == DW_DLV_OK) {
            printf("\tcie sec. offset %llu 0x%llx\n",
                   (unsigned long long) cie_off,
                   (unsigned long long) cie_off);

        }

        printf("\taugmentation\t\t\t%s\n", augmenter);
        printf("\tcode_alignment_factor\t\t%llu\n",
               (unsigned long long) code_alignment_factor);
        printf("\tdata_alignment_factor\t\t%lld\n",
               (long long) data_alignment_factor);
        printf("\treturn_address_register\t\t%d\n",
               (int) return_address_register_rule);
        {
            int ares = 0;
            Dwarf_Small *data = 0;
            Dwarf_Unsigned len = 0;

            ares =
                dwarf_get_cie_augmentation_data(cie, &data, &len, &err);
            if (ares == DW_DLV_NO_ENTRY) {
                /* do nothing. */
            } else if (ares == DW_DLV_OK && len > 0) {
                int k2;

                printf("\teh aug data len 0x%llx", (long long) len);
                for (k2 = 0; data && k2 < len; ++k2) {
                    if (k2 == 0) {
                        printf(" bytes 0x");
                    }
                    printf("%02x ", (unsigned char) data[k2]);
                }
                printf("\n");
            }                   /* else DW_DLV_ERROR or no data, do
                                   nothing */
        }

        printf
            ("\tbytes of initial instructions:\t%lld\n",
             (long long) initial_instructions_length);
        printf("\tcie length :\t\t\t%lld\n", (long long) cie_length);
        print_frame_inst_bytes(dbg, initial_instructions, (Dwarf_Signed)
                               initial_instructions_length,
                               data_alignment_factor,
                               (int) code_alignment_factor,
                               address_size, config_data);
    }
    return DW_DLV_OK;
}

/* get all the data in .debug_frame (or .eh_frame). 
 The '3' versions mean print using the dwarf3 new interfaces.
 The non-3 mean use the old interfaces.
 All combinations of requests are possible.
*/
extern void
print_frames(Dwarf_Debug dbg, int print_debug_frame, int print_eh_frame, struct dwconf_s *config_data)
{
    Dwarf_Cie *cie_data = NULL;
    Dwarf_Signed cie_element_count = 0;
    Dwarf_Fde *fde_data = NULL;
    Dwarf_Signed fde_element_count = 0;
    Dwarf_Signed i;
    int fres = 0;
    Dwarf_Half address_size = 0;
    int framed = 0;
    int frame_count = 0;
    int cie_count = 0;

    fres = dwarf_get_address_size(dbg, &address_size, &rose_dwarf_error);
    if (fres != DW_DLV_OK) {
        print_error(dbg, "dwarf_get_address_size", fres, rose_dwarf_error);
    }
    for (framed = 0; framed < 2; ++framed) {
        char *framename = 0;
        int silent_if_missing = 0;
        int is_eh = 0;

        if (framed == 0) {
            if (!print_debug_frame) {
                continue;
            }
            framename = ".debug_frame";
            /* 
             * Big question here is how to print all the info?
             * Can print the logical matrix, but that is huge,
             * though could skip lines that don't change.
             * Either that, or print the instruction statement program
             * that describes the changes.
             */
            fres =
                dwarf_get_fde_list(dbg, &cie_data, &cie_element_count,
                                   &fde_data, &fde_element_count, &rose_dwarf_error);
        } else {
            if (!print_eh_frame) {
                continue;
            }
            is_eh = 1;
            /* This is gnu g++ exceptions in a .eh_frame section. Which 
               is just like .debug_frame except that the empty, or
               'special' CIE_id is 0, not -1 (to distinguish fde from
               cie). And the augmentation is "eh". As of egcs-1.1.2
               anyway. A non-zero cie_id is in a fde and is the
               difference between the fde address and the beginning of
               the cie it belongs to. This makes sense as this is
               intended to be referenced at run time, and is part of
               the running image. For more on augmentation strings, see 
               libdwarf/dwarf_frame.c.  */

            /* 
             * Big question here is how to print all the info?
             * Can print the logical matrix, but that is huge,
             * though could skip lines that don't change.
             * Either that, or print the instruction statement program
             * that describes the changes.
             */
            silent_if_missing = 1;
            framename = ".eh_frame";
            fres =
                dwarf_get_fde_list_eh(dbg, &cie_data,
                                      &cie_element_count, &fde_data,
                                      &fde_element_count, &rose_dwarf_error);
        }
        if (fres == DW_DLV_ERROR) {
            printf("\n%s\n", framename);
            print_error(dbg, "dwarf_get_fde_list", fres, rose_dwarf_error);
        } else if (fres == DW_DLV_NO_ENTRY) {
            if (!silent_if_missing)
                printf("\n%s\n", framename);
            /* no frame information */
        } else {                /* DW_DLV_OK */

            printf("\n%s\n", framename);
            printf("\nfde:\n");

            for (i = 0; i < fde_element_count; i++) {
                print_one_fde(dbg, fde_data[i],i, cie_data, cie_element_count,address_size, is_eh, config_data);
                ++frame_count;
                if(frame_count >= break_after_n_units) {
                    break;
                }
            }
            /* 
               Print the cie set. */
            if (verbose) {
                printf("\ncie:\n");
                for (i = 0; i < cie_element_count; i++) {
                    print_one_cie(dbg, cie_data[i], i, address_size,
                                  config_data);
                    ++cie_count;
                    if(cie_count >= break_after_n_units) {
                        break;
                    }
                }
            }
            dwarf_fde_cie_list_dealloc(dbg, cie_data, cie_element_count,
                                       fde_data, fde_element_count);
        }
    }
    if (current_cu_die_for_print_frames) {
        dwarf_dealloc(dbg, current_cu_die_for_print_frames, DW_DLA_DIE);
        current_cu_die_for_print_frames = 0;
    }
}


// ************************************************************************

#include <libelf.h>


#define DW_SECTION_REL_DEBUG_INFO    0
#define DW_SECTION_REL_DEBUG_LINE    1
#define DW_SECTION_REL_DEBUG_PUBNAME 2
#define DW_SECTION_REL_DEBUG_ABBREV  3
#define DW_SECTION_REL_DEBUG_ARANGES 4
#define DW_SECTION_REL_DEBUG_FRAME   5
#define DW_SECTION_REL_DEBUG_NUM     6

#define DW_SECTNAME_REL_DEBUG_INFO    ".rel.debug_info"
#define DW_SECTNAME_REL_DEBUG_LINE    ".rel.debug_line"
#define DW_SECTNAME_REL_DEBUG_PUBNAME ".rel.debug_pubname"
#define DW_SECTNAME_REL_DEBUG_ABBREV  ".rel.debug_abbrev"
#define DW_SECTNAME_REL_DEBUG_ARANGES ".rel.debug_aranges"
#define DW_SECTNAME_REL_DEBUG_FRAME   ".rel.debug_frame"

#define STRING_FOR_DUPLICATE " duplicate"
#define STRING_FOR_NULL      " null"

static char *sectnames[] = {
    DW_SECTNAME_REL_DEBUG_INFO,
    DW_SECTNAME_REL_DEBUG_LINE,
    DW_SECTNAME_REL_DEBUG_PUBNAME,
    DW_SECTNAME_REL_DEBUG_ABBREV,
    DW_SECTNAME_REL_DEBUG_ARANGES,
    DW_SECTNAME_REL_DEBUG_FRAME,
};

static char *error_msg_duplicate[] = {
    DW_SECTNAME_REL_DEBUG_INFO STRING_FOR_DUPLICATE,
    DW_SECTNAME_REL_DEBUG_LINE STRING_FOR_DUPLICATE,
    DW_SECTNAME_REL_DEBUG_PUBNAME STRING_FOR_DUPLICATE,
    DW_SECTNAME_REL_DEBUG_ABBREV STRING_FOR_DUPLICATE,
    DW_SECTNAME_REL_DEBUG_ARANGES STRING_FOR_DUPLICATE,
    DW_SECTNAME_REL_DEBUG_FRAME STRING_FOR_DUPLICATE,
};

static char *error_msg_null[] = {
    DW_SECTNAME_REL_DEBUG_INFO STRING_FOR_NULL,
    DW_SECTNAME_REL_DEBUG_LINE STRING_FOR_NULL,
    DW_SECTNAME_REL_DEBUG_PUBNAME STRING_FOR_NULL,
    DW_SECTNAME_REL_DEBUG_ABBREV STRING_FOR_NULL,
    DW_SECTNAME_REL_DEBUG_ARANGES STRING_FOR_NULL,
    DW_SECTNAME_REL_DEBUG_FRAME STRING_FOR_NULL,
};

#define SECT_DATA_SET(x) { \
            if (sect_data[(x)].buf != NULL) { \
                print_error(dbg, error_msg_duplicate[(x)],DW_DLV_OK, rose_dwarf_error); \
            } \
            if ((data = elf_getdata(scn, 0)) == NULL || data->d_size == 0) { \
                print_error(dbg, error_msg_null[(x)],DW_DLV_OK, rose_dwarf_error); \
            } \
            sect_data[(x)].buf = (Dwarf_Small*) data -> d_buf; \
            sect_data[(x)].size = data -> d_size; \
            }

static char *reloc_type_names[] = {
    "R_MIPS_NONE", "R_MIPS_16", "R_MIPS_32", "R_MIPS_REL32",
    "R_MIPS_26", "R_MIPS_HI16", "R_MIPS_LO16", "R_MIPS_GPREL16",
    "R_MIPS_LITERAL", "R_MIPS_GOT16", "R_MIPS_PC16", "R_MIPS_CALL16",
    "R_MIPS_GPREL32",           /* 12 */
    "reloc type 13?", "reloc type 14?", "reloc type 15?",
    "R_MIPS_SHIFT5",            /* 16 */
    "R_MIPS_SHIFT6",            /* 17 */
    "R_MIPS_64",                /* 18 */
    "R_MIPS_GOT_DISP",          /* 19 */
    "R_MIPS_GOT_PAGE",          /* 20 */
    "R_MIPS_GOT_OFST",          /* 21 */
    "R_MIPS_GOT_HI16",          /* 22 */
    "R_MIPS_GOT_LO16",          /* 23 */
    "R_MIPS_SUB",               /* 24 */
    "R_MIPS_INSERT_A",          /* 25 */
    "R_MIPS_INSERT_B",          /* 26 */
    "R_MIPS_DELETE",            /* 27 */
    "R_MIPS_HIGHER",            /* 28 */
    "R_MIPS_HIGHEST",           /* 29 */
    "R_MIPS_CALL_HI16",         /* 30 */
    "R_MIPS_CALL_LO16",         /* 31 */
    "R_MIPS_SCN_DISP",          /* 32 */
    "R_MIPS_REL16",             /* 33 */
    "R_MIPS_ADD_IMMEDIATE",     /* 34 */
};

static struct {
    Dwarf_Small *buf;
    Dwarf_Unsigned size;
} sect_data[DW_SECTION_REL_DEBUG_NUM];

#ifndef HAVE_ELF64_GETEHDR
#define Elf64_Addr  long
#define Elf64_Word  unsigned long
#define Elf64_Xword unsigned long
#define Elf64_Sym   long
#endif

typedef size_t indx_type;

typedef struct {
    indx_type indx;
    char *name;
    Elf32_Addr value;
    Elf32_Word size;
    int type;
    int bind;
    unsigned char other;
    Elf32_Half shndx;
} SYM;


typedef struct {
    indx_type indx;
    char *name;
    Elf64_Addr value;
    Elf64_Xword size;
    int type;
    int bind;
    unsigned char other;
    unsigned short shndx;
} SYM64;

static SYM   *sym_data;
static SYM64 *sym_data_64;
static long   sym_data_entry_count;
static long   sym_data_64_entry_count;


static SYM *
readsyms(Elf32_Sym * data, size_t num, Elf * elf, Elf32_Word link)
{
    SYM *s, *buf;
    indx_type i;

    if ((buf = (SYM *) calloc(num, sizeof(SYM))) == NULL) {
        return NULL;
    }
    s = buf;                    /* save pointer to head of array */
    for (i = 1; i < num; i++, data++, buf++) {
        buf->indx = i;
        buf->name = (char *) elf_strptr(elf, link, data->st_name);
        buf->value = data->st_value;
        buf->size = data->st_size;
        buf->type = ELF32_ST_TYPE(data->st_info);
        buf->bind = ELF32_ST_BIND(data->st_info);
        buf->other = data->st_other;
        buf->shndx = data->st_shndx;
    }                           /* end for loop */
    return (s);
}

static SYM64 *
read_64_syms(Elf64_Sym * data, size_t num, Elf * elf, Elf64_Word link)
{
#ifdef HAVE_ELF64_GETEHDR

    SYM64 *s, *buf;
    indx_type i;

    if ((buf = (SYM64 *) calloc(num, sizeof(SYM64))) == NULL) {
        return NULL;
    }
    s = buf;                    /* save pointer to head of array */
    for (i = 1; i < num; i++, data++, buf++) {
        buf->indx = i;
        buf->name = (char *) elf_strptr(elf, link, data->st_name);
        buf->value = data->st_value;
        buf->size = data->st_size;
        buf->type = ELF64_ST_TYPE(data->st_info);
        buf->bind = ELF64_ST_BIND(data->st_info);
        buf->other = data->st_other;
        buf->shndx = data->st_shndx;
    }                           /* end for loop */
    return (s);
#else
    return 0;
#endif /* HAVE_ELF64_GETEHDR */
}



/*
        return valid reloc type names. 
        if buf is used, it is static, so beware it
        will be overrwritten by the next call.
*/
char *
get_reloc_type_names(int index)
{
    static char buf[100];
    int arysiz = sizeof(reloc_type_names) / sizeof(char *);
    char *retval;

    if (index < 0 || index >= arysiz) {
        sprintf(buf, "reloc type %d unknown", (int) index);
        retval = buf;
    } else {
        retval = reloc_type_names[index];
    }
    return retval;
}

void *
get_scndata(Elf_Scn * fd_scn, size_t * scn_size)
{
    Elf_Data *p_data;

    p_data = 0;
    if ((p_data = elf_getdata(fd_scn, p_data)) == 0 ||
        p_data->d_size == 0) {
        return NULL;
    }
    *scn_size = p_data->d_size;
    return (p_data->d_buf);
}


static void
print_reloc_information_64(int section_no, Dwarf_Small * buf,
                           Dwarf_Unsigned size)
{
    Dwarf_Unsigned off;

    printf("\n%s:\n", sectnames[section_no]);
#if HAVE_ELF64_GETEHDR
    for (off = 0; off < size; off += sizeof(Elf64_Rel)) {
#if HAVE_ELF64_R_INFO
        /* This works for the Elf64_Rel in linux */
        Elf64_Rel *p = (Elf64_Rel *) (buf + off);
        char *name = "<no name>";
        if(sym_data ) {
           size_t index = ELF64_R_SYM(p->r_info) - 1;
           if(index < sym_data_entry_count) {
              name = sym_data[index].name;
           }
        } else if (sym_data_64) {
           size_t index = ELF64_R_SYM(p->r_info) - 1;
           if(index < sym_data_64_entry_count) {
              name = sym_data_64[index].name;
           }
        }

        printf("%5lu\t<%3ld> %-34s%s\n",
            (unsigned long int) (p->r_offset),
            (long)ELF64_R_SYM(p->r_info),
            name,
            get_reloc_type_names(ELF64_R_TYPE(p->r_info)));
#else
        /* sgi/mips -64 does not have r_info in the 64bit relocations,
           but seperate fields, with 3 types, actually. Only one of
           which prints here, as only one really used with dwarf */
        Elf64_Rel *p = (Elf64_Rel *) (buf + off);
        char *name = "<no name>";
        if(sym_data ) {
           size_t index = p->r_sym - 1;
           if(index < sym_data_entry_count) {
              name = sym_data[index].name;
           }
        } else if (sym_data_64) {
           size_t index = p->r_sym - 1;
           if(index < sym_data_64_entry_count) {
               name = sym_data_64[index].name;
           }
        }

        printf("%5llu\t<%3ld> %-34s%s\n",
            (unsigned long long int) (p->r_offset),
            (long)p->r_sym,
            name,
            get_reloc_type_names(p->r_type));
#endif
    }
#endif /* HAVE_ELF64_GETEHDR */
}

static void
print_reloc_information_32(int section_no, Dwarf_Small * buf,
                           Dwarf_Unsigned size)
{
    Dwarf_Unsigned off;

    printf("\n%s:\n", sectnames[section_no]);
    for (off = 0; off < size; off += sizeof(Elf32_Rel)) {
        Elf32_Rel *p = (Elf32_Rel *) (buf + off);
        char *name = "<no name>";
        if(sym_data) {
           size_t index = ELF32_R_SYM(p->r_info) - 1;
           if(index < sym_data_entry_count) {
               name = sym_data[index].name;
           }
        }

        printf("%5lu\t<%3d> %-34s%s\n",
               (unsigned long int) (p->r_offset),
               ELF32_R_SYM(p->r_info),
               name,
               get_reloc_type_names(ELF32_R_TYPE(p->r_info)));
    }
}

void
print_relocinfo_64(Dwarf_Debug dbg, Elf * elf)
{
#ifdef HAVE_ELF64_GETEHDR
    Elf_Scn *scn = NULL;
    Elf_Data *data;
    Elf64_Ehdr *ehdr64;
    Elf64_Shdr *shdr64;
    char *scn_name;
    int i;
    Elf64_Sym *sym_64 = 0;

    if ((ehdr64 = elf64_getehdr(elf)) == NULL) {
        print_error(dbg, "DW_ELF_GETEHDR_ERROR", DW_DLV_OK, rose_dwarf_error);
    }

    while ((scn = elf_nextscn(elf, scn)) != NULL) {

        if ((shdr64 = elf64_getshdr(scn)) == NULL) {
            print_error(dbg, "DW_ELF_GETSHDR_ERROR", DW_DLV_OK, rose_dwarf_error);
        }
        if ((scn_name =
             elf_strptr(elf, ehdr64->e_shstrndx, shdr64->sh_name))
            == NULL) {
            print_error(dbg, "DW_ELF_STRPTR_ERROR", DW_DLV_OK, rose_dwarf_error);
        }
        if (shdr64->sh_type == SHT_SYMTAB) {
            size_t sym_size = 0;
            size_t count = 0;

            if ((sym_64 =
                 (Elf64_Sym *) get_scndata(scn, &sym_size)) == NULL) {
                print_error(dbg, "no symbol table data", DW_DLV_OK,rose_dwarf_error);
            }
            count = sym_size / sizeof(Elf64_Sym);
            sym_64++;
            free(sym_data_64);
            sym_data_64 = read_64_syms(sym_64, count, elf, shdr64->sh_link);
            sym_data_64_entry_count = count;
            if (sym_data_64  == NULL) {
                print_error(dbg, "problem reading symbol table data",
                            DW_DLV_OK, err);
            }
        } else if (strncmp(scn_name, ".rel.debug_", 11))
            continue;
        else if (strcmp(scn_name, ".rel.debug_info") == 0) {
            SECT_DATA_SET(DW_SECTION_REL_DEBUG_INFO)
        } else if (strcmp(scn_name, ".rel.debug_line") == 0) {
            SECT_DATA_SET(DW_SECTION_REL_DEBUG_LINE)
        } else if (strcmp(scn_name, ".rel.debug_pubname") == 0) {
            SECT_DATA_SET(DW_SECTION_REL_DEBUG_PUBNAME)
        } else if (strcmp(scn_name, ".rel.debug_aranges") == 0) {
            SECT_DATA_SET(DW_SECTION_REL_DEBUG_ARANGES)
        } else if (strcmp(scn_name, ".rel.debug_abbrev") == 0) {
            SECT_DATA_SET(DW_SECTION_REL_DEBUG_ABBREV)
        } else if (strcmp(scn_name, ".rel.debug_frame") == 0) {
            SECT_DATA_SET(DW_SECTION_REL_DEBUG_FRAME)
        }
    }                           /* while */

    for (i = 0; i < DW_SECTION_REL_DEBUG_NUM; i++) {
        if (sect_data[i].buf != NULL && sect_data[i].size > 0) {
            print_reloc_information_64(i, sect_data[i].buf,
                                       sect_data[i].size);
        }
    }
#endif
}

void
print_relocinfo_32(Dwarf_Debug dbg, Elf * elf)
{
    Elf_Scn *scn = NULL;
    Elf_Data *data;
    Elf32_Ehdr *ehdr32;
    Elf32_Shdr *shdr32;
    char *scn_name;
    int i;
    Elf32_Sym  *sym = 0;

    if ((ehdr32 = elf32_getehdr(elf)) == NULL) {
        print_error(dbg, "DW_ELF_GETEHDR_ERROR", DW_DLV_OK, rose_dwarf_error);
    }
    while ((scn = elf_nextscn(elf, scn)) != NULL) {
        if ((shdr32 = elf32_getshdr(scn)) == NULL) {
            print_error(dbg, "DW_ELF_GETSHDR_ERROR", DW_DLV_OK, rose_dwarf_error);
        }
        if ((scn_name =
             elf_strptr(elf, ehdr32->e_shstrndx, shdr32->sh_name)
            ) == NULL) {
            print_error(dbg, "DW_ELF_STRPTR_ERROR", DW_DLV_OK, rose_dwarf_error);
        }
        if (shdr32->sh_type == SHT_SYMTAB) {
            size_t sym_size = 0;
            size_t count = 0;

            if ((sym =
                 (Elf32_Sym *) get_scndata(scn, &sym_size)) == NULL) {
                print_error(dbg, "no symbol table data", DW_DLV_OK, rose_dwarf_error);
            }
            sym = (Elf32_Sym *) get_scndata(scn, &sym_size);
            count = sym_size / sizeof(Elf32_Sym);
            sym++;
            free(sym_data);
            sym_data = readsyms(sym, count, elf, shdr32->sh_link);
            sym_data_entry_count = count;
            if (sym_data  == NULL) {
                print_error(dbg, "problem reading symbol table data", DW_DLV_OK, rose_dwarf_error);
            }
        } else if (strncmp(scn_name, ".rel.debug_", 11))
            continue;
        else if (strcmp(scn_name, ".rel.debug_info") == 0) {
            SECT_DATA_SET(DW_SECTION_REL_DEBUG_INFO)
        } else if (strcmp(scn_name, ".rel.debug_line") == 0) {
            SECT_DATA_SET(DW_SECTION_REL_DEBUG_LINE)
        } else if (strcmp(scn_name, ".rel.debug_pubname") == 0) {
            SECT_DATA_SET(DW_SECTION_REL_DEBUG_PUBNAME)
        } else if (strcmp(scn_name, ".rel.debug_aranges") == 0) {
            SECT_DATA_SET(DW_SECTION_REL_DEBUG_ARANGES)
        } else if (strcmp(scn_name, ".rel.debug_abbrev") == 0) {
            SECT_DATA_SET(DW_SECTION_REL_DEBUG_ABBREV)
        } else if (strcmp(scn_name, ".rel.debug_frame") == 0) {
            SECT_DATA_SET(DW_SECTION_REL_DEBUG_FRAME)
        }
    }                           /* while */

    for (i = 0; i < DW_SECTION_REL_DEBUG_NUM; i++) {
        if (sect_data[i].buf != NULL && sect_data[i].size > 0) {
            print_reloc_information_32(i, sect_data[i].buf,sect_data[i].size);
        }
    }
}


void
print_relocinfo(Dwarf_Debug dbg)
{
    Elf *elf;
    char *endr_ident;
    int is_64bit;
    int res;
    int i;
    Elf32_Sym *sym = 0;

    printf("\n.debug_realoc\n");
    for (i = 0; i < DW_SECTION_REL_DEBUG_NUM; i++) {
        sect_data[i].buf = 0;
        sect_data[i].size = 0;
    }
    res = dwarf_get_elf(dbg, &elf, &rose_dwarf_error);
    if (res != DW_DLV_OK) {
        print_error(dbg, "dwarf_get_elf error", res, rose_dwarf_error);
    }
    if ((endr_ident = elf_getident(elf, NULL)) == NULL) {
        print_error(dbg, "DW_ELF_GETIDENT_ERROR", res, rose_dwarf_error);
    }
    is_64bit = (endr_ident[EI_CLASS] == ELFCLASS64);
    if (is_64bit) {
        print_relocinfo_64(dbg, elf);
    } else {
        print_relocinfo_32(dbg, elf);
    }
}


// ************************************************************************
// ************************************************************************

int
get_file_descriptor (SgProject* project)
   {
  // vector<SgNode*> asmFileList = NodeQuery::querySubTree (project,V_SgAsmGenericFile);
  // vector<SgNode*> asmFileList = NodeQuery::querySubTree (project,V_SgAsmPEFileHeader | V_SgAsmElfFileHeader | V_SgAsmDOSFileHeader | V_SgAsmLEFileHeader | V_SgAsmNEFileHeader);
  // VariantVector(V_SgClassDefinition) + VariantVector(V_SgType)
     vector<SgNode*> asmHeaderList = NodeQuery::querySubTree (project,V_SgAsmGenericHeader);
     ROSE_ASSERT(asmHeaderList.empty() == false);

     printf ("asmHeaderList = %zu \n",asmHeaderList.size());

     SgAsmGenericFile* genericFile = NULL;
     for (size_t i = 0; i < asmHeaderList.size(); i++)
        {
          printf ("asmHeaderList[i] = %p = %s \n",asmHeaderList[i],asmHeaderList[i]->class_name().c_str());

          SgAsmGenericHeader* genericHeader = isSgAsmGenericHeader(asmHeaderList[i]);
          ROSE_ASSERT(genericHeader != NULL);

          if (genericFile != NULL)
             {
            // Note that all the SgAsmGenericHeader should have a pointer to the same SgAsmGenericFile IR node.
               ROSE_ASSERT(genericFile == genericHeader->get_file());
             }
            else
             {
               genericFile = genericHeader->get_file();
             }

          printf ("genericFile = %p = %s \n",genericFile,genericFile->class_name().c_str());
        }

     ROSE_ASSERT(genericFile != NULL);

     return genericFile->get_fd();
   }


SgAsmGenericFile*
get_asmFile (SgProject* project)
   {
     SgAsmGenericFile*    asmFile    = NULL;
     SgBinaryComposite* binary = NULL;
     int numberOfFiles = (int) project->get_fileList().size();
     for (int i = 0; i < numberOfFiles; i++)
        {
          binary = isSgBinaryComposite(project->get_fileList()[0]);
          if (binary != NULL)
               asmFile    = binary->get_binaryFile();
        }

     ROSE_ASSERT(asmFile != NULL);

     return asmFile;
   }

SgBinaryComposite*
get_binaryFile (SgProject* project)
   {
     SgBinaryComposite* binary = NULL;
     int numberOfFiles = (int) project->get_fileList().size();
     for (int i = 0; i < numberOfFiles; i++)
        {
          binary = isSgBinaryComposite(project->get_fileList()[0]);
        }

     ROSE_ASSERT(binary != NULL);

     return binary;
   }

SgAsmInterpretation*
get_primaryInterpretation(SgProject* project)
   {
     SgAsmGenericFile*    asmFile    = NULL;
     SgBinaryComposite* binary = NULL;
     int numberOfFiles = (int) project->get_fileList().size();
     for (int i = 0; i < numberOfFiles && binary==NULL; i++) {
         if (isSgBinaryComposite(project->get_fileList()[0])!=NULL)
             binary = isSgBinaryComposite(project->get_fileList()[0]);
     }

     ROSE_ASSERT(binary != NULL);

     const SgAsmInterpretationPtrList &interps = binary->get_interpretations()->get_interpretations();
     int numberOfInterpretations = interps.size();

     int lastInterpretation = numberOfInterpretations - 1;

     SgAsmInterpretation* primaryInterpretation = interps[lastInterpretation];

     ROSE_ASSERT(primaryInterpretation != NULL);
     return primaryInterpretation;
   }


// ************************************************************************


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

     printf ("Inside of build_dwarf_IR_node_from_die_and_children() \n");

     for (;;)
        {
          printf ("Top of loop in build_dwarf_IR_node_from_die_and_children() \n");

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
                        if (tag_tree_combination(tag_parent, tag_child))
                           {
                          /* OK */
                           }
                          else
                           {
                             DWARF_CHECK_ERROR3(get_TAG_name(dbg, tag_parent).c_str(),get_TAG_name(dbg, tag_child).c_str(),"tag-tree relation is not standard.");
                           }
                      }
                  }
             }

       // DQ (11/4/2008): This is the location were we will have to generate IR nodes using each debug info entry (die)
          printf ("Calling print_one_die to output the information about each specific debug info entry (die) \n");

       /* here to pre-descent processing of the die */
          print_one_die(dbg, in_die, info_flag, srcfiles, cnt);

          printf ("Process children \n");

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

          printf ("Process siblings \n");
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

          printf ("Process post-dwarf_child \n");

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

          printf ("Bottom of loop in build_dwarf_IR_node_from_die_and_children() \n");
        }  /* end for loop on siblings */

     return;
   }


#if 0
/*
    This structure defines a row of the line table.
    All of the fields except li_offset have the exact 
    same meaning that is defined in Section 6.2.2 
    of the Libdwarf Document. 

    li_offset is used by _dwarf_addr_finder() which is called
    by rqs(1), an sgi utility for 'moving' shared libraries
    as if the static linker (ld) had linked the shared library
    at the newly-specified address.  Most libdwarf-using 
    apps will ignore li_offset and _dwarf_addr_finder().
    
*/
struct Dwarf_Line_s
   {
     Dwarf_Addr li_address;	/* pc value of machine instr */
     union addr_or_line_s
        {
          struct li_inner_s
             {
               Dwarf_Sword li_file;	/* int identifying src file */
            /* li_file is a number 1-N, indexing into a conceptual
               source file table as described in dwarf2/3 spec line
               table doc. (see Dwarf_File_Entry lc_file_entries; and
               Dwarf_Sword lc_file_entry_count;) */

               Dwarf_Sword li_line;	/* source file line number. */
               Dwarf_Half li_column;	/* source file column number */
               Dwarf_Small li_isa;

            /* To save space, use bit flags. */
            /* indicate start of stmt */
               unsigned char li_is_stmt:1;

            /* indicate start basic block */
               unsigned char li_basic_block:1;

            /* first post sequence instr */
               unsigned char li_end_sequence:1;

               unsigned char li_prologue_end:1;
               unsigned char li_epilogue_begin:1;
             } li_l_data;
          Dwarf_Off li_offset;	/* for rqs */
        } li_addr_line;
     Dwarf_Line_Context li_context;	/* assoc Dwarf_Line_Context_s */
   };
#endif


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

     printf ("Inside of build_dwarf_line_numbers_this_cu() (using .debug_line section) \n");

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
          print_one_die(dbg, cu_die, /* print_information= */ 1,/* srcfiles= */ 0, /* cnt= */ 0);

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
               print_source_intro(cu_die);
               if (verbose)
                  {
                    print_one_die(dbg, cu_die, /* print_information= */ 1,/* srcfiles= */ 0, /* cnt= */ 0);
                  }

            // Output a header for the data
               printf("<source>\t[row,column]\t<pc>\t//<new statement or basic block\n");

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

                    printf("%s:\t[%3llu,%2lld]\t%#llx", filename, lineno,column, pc);

                 // Build an IR node to represent the instruction address for each line.
#if 0
                 // build a Sg_File_Info so that we can use the filename mapping to integers
                    Sg_File_Info* fileInfo = new Sg_File_Info(filename,0,0);
                    int filename_id = fileInfo->get_file_id();
                    delete fileInfo;
                    fileInfo = NULL;
#else
                 // This uses the static maps in the Sg_File_Info to support a table similar 
                 // to Dwarf's and which maps filenames to integers and back.  This avoids
                 // building and deleting a Sg_File_Info object.
                    int filename_id = Sg_File_Info::addFilenameToMap(filename);
#endif
                 // Now build the IR node to store the raw information from dwarf.
                    SgAsmDwarfLine* lineInfo = new SgAsmDwarfLine(pc, filename_id, lineno, column);

                    asmDwarfLineList->get_line_list().push_back(lineInfo);

                    if (sres == DW_DLV_OK)
                         dwarf_dealloc(dbg, filename, DW_DLA_STRING);

                    nsres = dwarf_linebeginstatement(line, &newstatement, &rose_dwarf_error);
                    if (nsres == DW_DLV_OK)
                       {
                         if (newstatement)
                            {
                              printf("\t// new statement");
                            }
                       }
                      else
                         if (nsres == DW_DLV_ERROR)
                            {
                              print_error(dbg, "linebeginstatment failed", nsres,rose_dwarf_error);
                            }

                    nsres = dwarf_lineblock(line, &new_basic_block, &rose_dwarf_error);
                    if (nsres == DW_DLV_OK)
                       {
                         if (new_basic_block)
                            {
                              printf("\t// new basic block");
                            }
                       }
                      else
                         if (nsres == DW_DLV_ERROR)
                            {
                              print_error(dbg, "lineblock failed", nsres, rose_dwarf_error);
                            }

                    nsres = dwarf_lineendsequence(line, &lineendsequence, &rose_dwarf_error);
                    if (nsres == DW_DLV_OK)
                       {
                         if (lineendsequence)
                            {
                              printf("\t// end of text sequence");
                            }
                       }
                      else
                         if (nsres == DW_DLV_ERROR)
                            {
                              print_error(dbg, "lineblock failed", nsres, rose_dwarf_error);
                            }
                    printf("\n");
                  }
               dwarf_srclines_dealloc(dbg, linebuf, linecount);
             }
        }

     printf ("Now generate the maps to use to lookup the instruction address to source position mappings \n");
     asmDwarfLineList->buildInstructionAddressSourcePositionMaps(asmDwarfCompilationUnit);
     printf ("DONE: Now generate the maps to use to lookup the instruction address to source position mappings \n");

#if 1
  // Run a second time to test that the maps are not regenerated
     printf ("Run a second time to test that the maps are not regenerated ... \n");
     DwarfInstructionSourceMapReturnType returnValue = asmDwarfLineList->buildInstructionAddressSourcePositionMaps();
     printf ("DONE: Run a second time to test that the maps are not regenerated ... \n");
#endif

#if 1
  // Output the line information from the generated maps.
     asmDwarfLineList->display("Inside of build_dwarf_line_numbers_this_cu()");
#endif
   }


/* process each compilation unit in .debug_info */
// void build_dwarf_IR_nodes(Dwarf_Debug dbg, SgBinaryFile* binaryFile)
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

     printf("\n.debug_info\n");
     SgAsmDwarfCompilationUnit* asmDwarfCompilationUnit = new SgAsmDwarfCompilationUnit();

#if 0
  // binaryFile->set_dwarf_info(asmDwarfCompilationUnit);
  // ROSE_ASSERT(binaryFile->get_dwarf_info() != NULL);
     asmInterpretation->set_dwarf_info(asmDwarfCompilationUnit);
     ROSE_ASSERT(asmInterpretation->get_dwarf_info() != NULL);
#endif

    /* Loop until it fails.  */
     while ((nres = dwarf_next_cu_header(dbg, &cu_header_length, &version_stamp,&abbrev_offset, &address_size,&next_cu_offset, &rose_dwarf_error)) == DW_DLV_OK)
        {
#if 0
          printf ("In loop over the headers: nres = %d \n",nres);
          printf ("next_cu_offset   = %lu \n",(unsigned long) next_cu_offset);
          printf ("cu_header_length = %lu \n",(unsigned long) cu_header_length);
#endif

          printf ("loop count: cu_count = %d \n",cu_count);

          if (cu_count >=  break_after_n_units)
             {
               printf("Break at %d\n",cu_count);
               break;
             }

          int sres = 0;
#if 1
          printf ("cu_name_flag = %s \n",cu_name_flag ? "true" : "false");
#endif

          printf ("Setting cu_name_flag == true \n");
       // cu_name_flag = true;
       // cu_name_flag = false;

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

#if 1
          printf ("build_dwarf_IR_nodes(): verbose = %s \n",verbose ? "true" : "false");
          printf ("build_dwarf_IR_nodes(): dense   = %s \n",dense ? "true" : "false");
#endif

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

     /* process a single compilation unit in .debug_info. */
        sres = dwarf_siblingof(dbg, NULL, &cu_die, &rose_dwarf_error);

        printf ("build_dwarf_IR_nodes(): status of call to dwarf_siblingof(): sres = %d \n",sres);

          if (sres == DW_DLV_OK)
             {
#if 0
               printf ("Processing sibling information \n");
               printf ("info_flag    = %s \n",info_flag    ? "true" : "false");
               printf ("cu_name_flag = %s \n",cu_name_flag ? "true" : "false");
#endif
               printf ("Explicitly setting info_flag == true \n");
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

                    printf ("In print_infos(): Calling print_die_and_children() \n");
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

               printf ("Explicitly setting line_flag == true \n");
               line_flag = true;

               if (line_flag)
                  {
                    printf ("\n\nOutput the line information by calling print_line_numbers_this_cu() \n");
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
          fprintf(stderr, "attempting to continue.\n");

          printf ("Error: nres = %d \n",nres);
          ROSE_ASSERT(false);
        }

  // printf ("Exiting print_infos() \n");
   }



// ************************************************************************


// NOTES: Checkout "man objcopy" and the options for:
//         --add-gnu-debuglink=path-to-file
//           Creates a .gnu_debuglink section which contains a reference to path-to-file and adds it to the output file.
//         --only-keep-debug
//           Strip a file, removing any sections that would be stripped by --strip-debug and leaving the debugging sections.
// These allow for a two part binary.  Can libdwarf read such files?  
// We think that a new section is generated, what is its name?


int
main(int argc, char** argv)
   {
     ios::sync_with_stdio();     // Syncs C++ and C I/O subsystems!
    
  // DQ (9/1/2006): Introduce tracking of performance of ROSE at the top most level.
     TimingPerformance timer ("AST binary reader (main): time (sec) = ",true);

     try{
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

  // Just set the project, the report will be generated upon calling the destructor for "timer"
     timer.set_project(project);

  // Internal AST consistancy tests.
     AstTests::runAllTests(project);

#if 0
     printf ("Generate the dot output of the SAGE III AST \n");
  // generateDOT ( *project );
     generateDOTforMultipleFile ( *project );
     printf ("DONE: Generate the dot output of the SAGE III AST \n");
#endif

#if 1
  // Use the file descriptor generated when ROSE read the binary file to generate the AST.
  // alternatively we could have reopended the file, but that would not have elegantly tied
  // the Dwarf handling to the AST.  I would like to tie the use of libdward more tightly
  // to the AST, but I am still learning how to use libdward at the moment.
     int fd = get_file_descriptor(project);

     printf ("file descriptor from ROSE AST: fd = %d \n",fd);

     int dwarf_init_status = dwarf_init (fd, DW_DLC_READ, NULL, NULL, &rose_dwarf_dbg, &rose_dwarf_error);

     printf ("dwarf_init_status = %d \n",dwarf_init_status);

  // Test if the call to dwarf_init worked!
     ROSE_ASSERT(dwarf_init_status == DW_DLV_OK);

  // I am unclear about the functionality of these two functions!
     dwarf_set_frame_rule_inital_value(rose_dwarf_dbg,global_config_file_data.cf_initial_rule_value);
     dwarf_set_frame_rule_table_size(rose_dwarf_dbg,global_config_file_data.cf_table_entry_count);

#if 0
  // SgAsmFile* asmFile = get_asmFile(project);
  // SgBinaryFile* binaryFile = get_binaryFile(project);
     SgAsmInterpretation* asmInterpretation = get_primaryInterpretation(project);

  // build_dwarf_IR_nodes(rose_dwarf_dbg,asmFile);
  // build_dwarf_IR_nodes(rose_dwarf_dbg,binaryFile);
     build_dwarf_IR_nodes(rose_dwarf_dbg,asmInterpretation);
#endif

#if 1
  // Print Dwarf info
     print_infos(rose_dwarf_dbg);

     print_pubnames(rose_dwarf_dbg);

     print_macinfo(rose_dwarf_dbg);

     print_locs(rose_dwarf_dbg);

     print_abbrevs(rose_dwarf_dbg);

     print_strings(rose_dwarf_dbg);

     print_aranges(rose_dwarf_dbg);

     current_cu_die_for_print_frames = 0;
     print_frames(rose_dwarf_dbg, frame_flag, eh_frame_flag, &global_config_file_data);

     print_static_funcs(rose_dwarf_dbg);

     print_static_vars(rose_dwarf_dbg);

     print_types(rose_dwarf_dbg, DWARF_PUBTYPES);
     print_types(rose_dwarf_dbg, SGI_TYPENAME);

     print_weaknames(rose_dwarf_dbg);

     print_relocinfo(rose_dwarf_dbg);
#endif

     printf ("\n\nFinishing Dwarf handling... \n\n");

     int dwarf_finish_status = dwarf_finish( rose_dwarf_dbg, &rose_dwarf_error);
     ROSE_ASSERT(dwarf_finish_status == DW_DLV_OK);
#endif

#if 1
     printf ("Generate the dot output of the SAGE III AST \n");
  // generateDOT ( *project );
     generateDOTforMultipleFile ( *project );
     printf ("DONE: Generate the dot output of the SAGE III AST \n");
#endif

  // Unparse the binary executable (as a binary, as an assembler text file, 
  // and as a dump of the binary executable file format details (sections)).
     return backend(project);

     }catch(SgAsmExecutableFileFormat::FormatError e) 
     {
       std::cerr << e.mesg << std::endl;

       exit(1);
     
     }
   }

#endif
