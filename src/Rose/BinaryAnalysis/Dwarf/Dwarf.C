// This file represents support in the ROSE AST for Dwarf debug information.
// Dwarf is generated when source code is compiled using a compiler's debug mode.
// the Dwarf information is represented a special sections in the file format of
// the binary executable.  These sections are read using libdwarf (open source
// library used for reading and writing dwarf sections).  In ROSE we read the
// Dwarf information and build special IR nodes into the AST.  This work effects
// only AST for binary executable files (the ROSE Binary AST).
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>                                 // needed for Sg_File_Info
#include <Rose/BinaryAnalysis/Dwarf.h>

#include <Rose/Diagnostics.h>
#include <Rose/StringUtility/Diagnostics.h>

#include <ROSE_UNUSED.h>

#include <SgAsmDwarfCompilationUnit.h>
#include <SgAsmDwarfConstruct.h>
#include <SgAsmDwarfConstructList.h>
#include <SgAsmDwarfLine.h>
#include <SgAsmDwarfLineList.h>

#include <Cxx_GrammarDowncast.h>

#include <stringify.h>

#ifdef ROSE_HAVE_LIBDWARF
#include <dwarf.h>
#include <libdwarf.h>
#endif

using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {
namespace Dwarf {

Sawyer::Message::Facility mlog;

void
initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::Dwarf");
        mlog.comment("parsing DWARF debug information");
    }
}

// DONE: Dwarf support handles sections: .debug_info, .debug_line
// TODO: Add support for sections: .debug_pubnames, .debug_pubtypes, and .debug_macinfo

// This is controled by using the --with-dwarf configure command line option.
#ifdef ROSE_HAVE_LIBDWARF


#define DIE_STACK_SIZE 300
static Dwarf_Die die_stack[DIE_STACK_SIZE];

#define PUSH_DIE_STACK(x) { ASSERT_require(indent_level < DIE_STACK_SIZE); die_stack[indent_level] = x; }
#define POP_DIE_STACK { ASSERT_require(indent_level < DIE_STACK_SIZE); die_stack[indent_level] = 0; }

int indent_level = 0;
bool local_symbols_already_began = false;

bool check_tag_tree = false;

int check_error;

static void
reportParseError(const std::string &str) {
    mlog[ERROR] <<"DWARF: " <<str <<" ***\n";
    ++check_error;
}

static void
reportParseError(const std::string &str1, const std::string &str2) {
    mlog[ERROR] <<"DWARF: " <<str1 <<": " <<str2 <<" ***\n";
    ++check_error;
}

typedef struct {
    int checks;
    int errors;
} Dwarf_Check_Result;

Dwarf_Check_Result tag_tree_result;
Dwarf_Check_Result type_offset_result;
Dwarf_Check_Result decl_file_result;

// Dwarf variables
Dwarf_Debug rose_dwarf_dbg;
Dwarf_Error rose_dwarf_error;

int verbose = 1;

char cu_name[BUFSIZ];
bool cu_name_flag = false;
Dwarf_Unsigned cu_offset = 0;

// This is mostly copied from the original `print_error` global function (now removed) which ignored all its arguments except the
// `mesg` string.  However, unlike `print_error` which unconditionally aborted, this one throws an exception so that user code can
// (hopefully) recover. It may or may not leave the AST in a consistent state since most of SageIII/dwarfSupport was not written
// with this possibility in mind.
static void
throwError(Dwarf_Debug, const std::string &mesg, int /*dwarf_code*/, Dwarf_Error) [[noreturn]] {
    throw Rose::BinaryAnalysis::Dwarf::Exception(mesg);
}

// An easier way to handle the results from a dwarf C function.
//
// If `result` (dwarf function return value) is DW_DLV_OK then simply return it.
//
// If `result` is DW_DLV_ERROR then throw an error using the specified name and debug context.
//
// If `result` is DW_DLV_NO_ENTRY then assign the `dflt` value to the `save` location, which is usually also one of the arguments
// for the dwarf function call that produced the result, and return DW_DLV_NO_ENTRY.
//
// For any other result, do nothing but return the result.
//
template<typename T>
static int presentOr(const int result,
                     const char *name, T &save, const T dflt, const Dwarf_Debug debug, const Dwarf_Error error) {
    switch (result) {
        case DW_DLV_ERROR:
            throwError(debug, name, DW_DLV_ERROR, error);
            ASSERT_not_reachable("throwError should not have returned");

        case DW_DLV_NO_ENTRY:
            save = dflt;
            break;
    }
    return result;
}

char *
makename(char *s) {
    if (char *newstr = s ? strdup(s) : strdup(""))
        return newstr;
    throw Rose::BinaryAnalysis::Dwarf::Exception("out of memory in makename for " +
                                                 Rose::StringUtility::plural(strlen(s)+1, "bytes"));
}

std::string
get_TAG_name (Dwarf_Debug dbg, Dwarf_Half val) {
    return std::string(stringify::Rose::BinaryAnalysis::Dwarf::DWARF_TAG(val));
}

std::string
get_AT_name (Dwarf_Debug dbg, Dwarf_Half val) {
  return std::string(stringify::Rose::BinaryAnalysis::Dwarf::DWARF_AT(val));
}

std::string
get_LANG_name (Dwarf_Debug dbg, Dwarf_Half val) {
  return std::string(stringify::Rose::BinaryAnalysis::Dwarf::DWARF_LANG(val));
}


// Typedef for function pointer type used in get_small_encoding_integer_and_name() function paramter.
typedef std::string(*encoding_type_func) (Dwarf_Debug dbg, Dwarf_Half val);

int
get_small_encoding_integer_and_name( Dwarf_Debug dbg, Dwarf_Attribute attrib, Dwarf_Unsigned * uval_out, char *attr_name,
                                     std::string * string_out, encoding_type_func val_as_string, Dwarf_Error * err) {
    Dwarf_Unsigned uval = 0;
    char buf[100];              /* The strings are small. */
    int vres = dwarf_formudata(attrib, &uval, err);

    if (vres != DW_DLV_OK) {
        Dwarf_Signed sval = 0;

        vres = dwarf_formsdata(attrib, &sval, err);
        if (vres != DW_DLV_OK) {
            if (string_out != 0) {
                snprintf(buf, sizeof(buf),"%s has a bad form.", attr_name);
                *string_out = makename(buf);
            }
            return vres;
        }
        *uval_out = (Dwarf_Unsigned) sval;
    } else {
        *uval_out = uval;
    }

    if (string_out)
        *string_out = val_as_string(dbg, (Dwarf_Half) uval);

    return DW_DLV_OK;
}

void
formx_unsigned(Dwarf_Unsigned u, std::string *esbp) {
    *esbp += std::to_string((unsigned long long)u);
}

void
formx_signed(Dwarf_Signed u, std::string *esbp) {
    *esbp += std::to_string((long long)u);
}

/* We think this is an integer. Figure out how to print it.
   In case the signedness is ambiguous (such as on
   DW_FORM_data1 (ie, unknown signedness) print two ways.
*/
int
formxdata_print_value(Dwarf_Attribute attrib, std::string *esbp, Dwarf_Error * err) {
    Dwarf_Signed tempsd = 0;
    Dwarf_Unsigned tempud = 0;
    int sres = 0;
    int ures = 0;
    Dwarf_Error serr = 0;
    ures = dwarf_formudata(attrib, &tempud, err);
    sres = dwarf_formsdata(attrib, &tempsd, &serr);

    if(ures == DW_DLV_OK) {
        if(sres == DW_DLV_OK) {

            if((Dwarf_Signed)tempud == tempsd) {
                /* Data is the same value, so makes no difference which
                   we print. */
                formx_unsigned(tempud,esbp);
            } else {
                formx_unsigned(tempud,esbp);
                *esbp += "(as signed = ";
                formx_signed(tempsd,esbp);
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
void
get_attr_value(Dwarf_Debug dbg, Dwarf_Half tag, Dwarf_Attribute attrib,char **srcfiles, Dwarf_Signed cnt, std::string *esbp) {
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
    Dwarf_Off fde_offset_for_cu_low = DW_DLE_BADOFF;
    Dwarf_Off fde_offset_for_cu_high = DW_DLE_BADOFF;

    fres = dwarf_whatform(attrib, &theform, &rose_dwarf_error);
    /* depending on the form and the attribute, process the form */
    if (fres == DW_DLV_ERROR) {
        throwError(dbg, "dwarf_whatform cannot find attr form", fres,rose_dwarf_error);
    } else if (fres == DW_DLV_NO_ENTRY) {
        return;
    }

    dwarf_whatform_direct(attrib, &direct_form, &rose_dwarf_error);
    /* ignore errors in dwarf_whatform_direct() */

    switch (theform) {
        case DW_FORM_addr:
            bres = dwarf_formaddr(attrib, &addr, &rose_dwarf_error);
            if (bres == DW_DLV_OK) {
                snprintf(small_buf, sizeof(small_buf), "%#llx",(unsigned long long) addr);
                *esbp += small_buf;
            } else {
                throwError(dbg, "addr formwith no addr?!", bres, rose_dwarf_error);
            }
            break;
        case DW_FORM_ref_addr:
            /* DW_FORM_ref_addr is not accessed thru formref: ** it is an
               address (global section offset) in ** the .debug_info
               section. */
            bres = dwarf_global_formref(attrib, &off, &rose_dwarf_error);
            if (bres == DW_DLV_OK) {
                snprintf(small_buf, sizeof(small_buf),"<global die offset %llu>",(unsigned long long) off);
                *esbp += small_buf;
            } else {
                throwError(dbg,"DW_FORM_ref_addr form with no reference?!",bres, rose_dwarf_error);
            }
            break;
        case DW_FORM_ref1:
        case DW_FORM_ref2:
        case DW_FORM_ref4:
        case DW_FORM_ref8:
        case DW_FORM_ref_udata: {
            Dwarf_Bool isInfo = false;
            bres = dwarf_formref(attrib, &off, &isInfo, &rose_dwarf_error);
            if (bres != DW_DLV_OK) {
                throwError(dbg, "ref formwith no ref?!", bres, rose_dwarf_error);
            }
            /* do references inside <> to distinguish them ** from
               constants. In dense form this results in <<>>. Ugly for
               dense form, but better than ambiguous. davea 9/94 */
            snprintf(small_buf, sizeof(small_buf), "<%llu>", off);
            *esbp += small_buf;

            // DQ: Added bool directly
            bool check_type_offset = false;

            if (check_type_offset) {
                wres = dwarf_whatattr(attrib, &attr, &rose_dwarf_error);
                if (wres == DW_DLV_ERROR) {

                } else if (wres == DW_DLV_NO_ENTRY) {
                }
                if (attr == DW_AT_type) {
                    dres = dwarf_offdie_b(dbg, cu_offset + off, isInfo, &die_for_check, &rose_dwarf_error);
                    type_offset_result.checks++;
                    if (dres != DW_DLV_OK) {
                        type_offset_result.errors++;
                        reportParseError("DW_AT_type offset does not point to type info");
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
                                    reportParseError("DW_AT_type offset does not point to type info");
                                    break;
                            }
                            dwarf_dealloc(dbg, die_for_check, DW_DLA_DIE);
                        } else {
                            type_offset_result.errors++;
                            reportParseError("DW_AT_type offset does not exist");
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
                for (i = 0; i < (int) tempb->bl_len; i++) {
                    snprintf(small_buf, sizeof(small_buf), "%02x",*(i + (unsigned char *) tempb->bl_data));
                    *esbp += small_buf;
                }
                dwarf_dealloc(dbg, tempb, DW_DLA_BLOCK);
            } else {
                throwError(dbg, "DW_FORM_blockn cannot get block\n", fres,rose_dwarf_error);
            }
            break;
        case DW_FORM_data1:
        case DW_FORM_data2:
        case DW_FORM_data4:
        case DW_FORM_data8:
            fres = dwarf_whatattr(attrib, &attr, &rose_dwarf_error);
            if (fres == DW_DLV_ERROR) {
                throwError(dbg, "FORM_datan cannot get attr", fres, rose_dwarf_error);
            } else if (fres == DW_DLV_NO_ENTRY) {
                throwError(dbg, "FORM_datan cannot get attr", fres, rose_dwarf_error);
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
                    case DW_AT_MIPS_fde: {
                        wres = get_small_encoding_integer_and_name(dbg,
                                                                   attrib,
                                                                   &tempud,
                                                                   /* attrname */
                                                                   (char *) NULL,
                                                                   /* err_string
                                                                    */
                                                                   (std::string*)NULL,
                                                                   (encoding_type_func) 0,
                                                                   &rose_dwarf_error);

                        if (wres == DW_DLV_OK) {
                            snprintf(small_buf, sizeof(small_buf), "%llu",tempud);
                            *esbp += small_buf;

                            if (attr == DW_AT_decl_file || attr == DW_AT_call_file) {
                                if (srcfiles && (int)tempud > 0 && (int)tempud <= cnt) {
                                    /* added by user request */
                                    /* srcfiles is indexed starting at 0, but
                                       DW_AT_decl_file defines that 0 means no
                                       file, so tempud 1 means the 0th entry in
                                       srcfiles, thus tempud-1 is the correct
                                       index into srcfiles.  */
                                    char *fname = srcfiles[tempud - 1];
                                    *esbp += " ";
                                    *esbp += fname;
                                }

                                bool check_decl_file = false;

                                if(check_decl_file) {
                                    decl_file_result.checks++;
                                    /* Zero is always a legal index, it means
                                       no source name provided. */
                                    if( (int)tempud > cnt) {
                                        decl_file_result.errors++;
                                        reportParseError(get_AT_name(dbg,attr).c_str(), "does not point to valid file info");
                                    }
                                }
                            }
                        } else {
                            throwError(dbg, "Cannot get encoding attribute ..",wres, rose_dwarf_error);
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
                            throwError(dbg,"Cannot get DW_AT_const_value ",wres,rose_dwarf_error);
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
                            throwError(dbg, "Cannot get formsdata..", wres,rose_dwarf_error);
                        }
                        break;
                }
            }
            if (cu_name_flag) {
                if (attr == DW_AT_MIPS_fde) {
                    if (fde_offset_for_cu_low == DW_DLE_BADOFF) {
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
                *esbp += small_buf;
            } else if (wres == DW_DLV_NO_ENTRY) {
                /* nothing? */
            } else {
                throwError(dbg, "Cannot get formsdata..", wres, rose_dwarf_error);
            }
            break;
        case DW_FORM_udata:
            wres = dwarf_formudata(attrib, &tempud, &rose_dwarf_error);
            if (wres == DW_DLV_OK) {
                snprintf(small_buf, sizeof(small_buf), "%llu", tempud);
                *esbp += small_buf;
            } else if (wres == DW_DLV_NO_ENTRY) {
                /* nothing? */
            } else {
                throwError(dbg, "Cannot get formudata....", wres, rose_dwarf_error);
            }
            break;
        case DW_FORM_string:
        case DW_FORM_strp:
            wres = dwarf_formstring(attrib, &temps, &rose_dwarf_error);
            if (wres == DW_DLV_OK) {
                *esbp += temps;
            } else if (wres == DW_DLV_NO_ENTRY) {
                /* nothing? */
            } else {
                throwError(dbg, "Cannot get a formstr (or a formstrp)....",wres, rose_dwarf_error);
            }

            break;
        case DW_FORM_flag:
            wres = dwarf_formflag(attrib, &tempbool, &rose_dwarf_error);
            if (wres == DW_DLV_OK) {
                if (tempbool) {
                    snprintf(small_buf, sizeof(small_buf), "yes(%d)",tempbool);
                    *esbp += small_buf;
                } else {
                    snprintf(small_buf, sizeof(small_buf), "no");
                    *esbp += small_buf;
                }
            } else if (wres == DW_DLV_NO_ENTRY) {
                /* nothing? */
            } else {
                throwError(dbg, "Cannot get formflag/p....", wres, rose_dwarf_error);
            }
            break;
        case DW_FORM_indirect:
            /* We should not ever get here, since the true form was
               determined and direct_form has the DW_FORM_indirect if it is
               used here in this attr. */
            ASSERT_not_reachable("dquinlan 2008-11-10");

        case DW_FORM_sec_offset:
        case DW_FORM_exprloc:
        case DW_FORM_flag_present:
        case DW_FORM_ref_sig8:
            /* all of these are DWARF4 related and currently unsupported. */
            break;
        default:
            // Failure to parse a DWARF construct must not be a non-recoverable error. [Robb P Matzke 2017-05-16]
            mlog[ERROR] <<"dwarf_whatform_unexpected value\n";
            break;
    }
    if (verbose && direct_form && direct_form == DW_FORM_indirect) {
        char *form_indir = strdup(" (used DW_FORM_indirect) ");
        *esbp += form_indir;
    }
}

void
print_attribute(Dwarf_Debug dbg, Dwarf_Die die, Dwarf_Half attr, Dwarf_Attribute attr_in,bool print_information,char **srcfiles,
                Dwarf_Signed cnt, SgAsmDwarfConstruct* asmDwarfConstruct) {
    Dwarf_Attribute attrib = 0;
    Dwarf_Unsigned  uval = 0;
    std::string          atname;
    std::string          valname;
    int             tres = 0;
    Dwarf_Half      tag = 0;

    // Added C++ string support to replace C style string support
    std::string esb_base;

    atname = get_AT_name(dbg, attr);

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
    if (tres == DW_DLV_ERROR) {
        tag = 0;
    } else {
        if (tres == DW_DLV_NO_ENTRY) {
            tag = 0;
        } else {
            /* ok */
        }
    }

    switch (attr) {
        case DW_AT_language: {
            get_small_encoding_integer_and_name(dbg, attrib, &uval,strdup("DW_AT_language"), &valname,get_LANG_name, &rose_dwarf_error);

            // The language is only set in the SgAsmDwarfCompilationUnit IR node.
            SgAsmDwarfCompilationUnit* asmDwarfCompilationUnit = isSgAsmDwarfCompilationUnit(asmDwarfConstruct);
            ASSERT_not_null(asmDwarfCompilationUnit);
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
                throwError(dbg,"Cannot get formudata....",wres,err);
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
                throwError(dbg, "dwarf_whatform cannot find attr form",rv, rose_dwarf_error);
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
                    throwError(dbg, "dwarf_whatform cannot find attr form",rv, rose_dwarf_error);
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
            esb_base = "";
            get_attr_value(dbg, tag, attrib, srcfiles, cnt, &esb_base);

            /* add the attribute as an AST attribute on the asmDwarfConstruct that we're
               working on */
            const std::string attrName = stringify::Rose::BinaryAnalysis::Dwarf::DWARF_AT(attr);
            Sawyer::Attribute::Id attrId = Sawyer::Attribute::declareMaybe(attrName);
            asmDwarfConstruct->attributes().setAttribute(attrId, esb_base);

            valname = esb_base;
            break;
    }

    // Set the name in the base clas only when the input attribute is DW_AT_name.
    if (attr == DW_AT_name) {
        asmDwarfConstruct->set_name(valname);
    }

}

/* print info about die */
SgAsmDwarfConstruct*
build_dwarf_IR_node_from_print_one_die(Dwarf_Debug dbg, Dwarf_Die die, bool print_information, char **srcfiles,
                                       Dwarf_Signed cnt /* , SgAsmDwarfCompilationUnit* asmDwarfCompilationUnit */) {
    Dwarf_Signed i;
    Dwarf_Off offset, overall_offset;
    std::string tagname;
    Dwarf_Half tag;
    Dwarf_Signed atcnt;
    Dwarf_Attribute *atlist;
    int tres;
    int ores;
    int atres;

    tres = dwarf_tag(die, &tag, &rose_dwarf_error);
    if (tres != DW_DLV_OK) {
        throwError(dbg, "accessing tag of die!", tres, rose_dwarf_error);
    }

    tagname = get_TAG_name(dbg, tag);

    ores = dwarf_dieoffset(die, &overall_offset, &rose_dwarf_error);
    if (ores != DW_DLV_OK) {
        throwError(dbg, "dwarf_dieoffset", ores, rose_dwarf_error);
    }

    ores = dwarf_die_CU_offset(die, &offset, &rose_dwarf_error);
    if (ores != DW_DLV_OK) {
        throwError(dbg, "dwarf_die_CU_offset", ores, rose_dwarf_error);
    }

    if (print_information) {
        if (indent_level == 0) {
            // Initialize the information in the SgAsmDwarfCompilationUnit IR node

        } else {
            if (local_symbols_already_began == false && indent_level == 1) {
                local_symbols_already_began = true;
            }
        }

    }

    atres = dwarf_attrlist(die, &atlist, &atcnt, &rose_dwarf_error);
    if (atres == DW_DLV_ERROR) {
        throwError(dbg, "dwarf_attrlist", atres, rose_dwarf_error);
    } else {
        if (atres == DW_DLV_NO_ENTRY) {
            /* indicates there are no attrs.  It is not an error. */
            atcnt = 0;
        }
    }

    if (indent_level == 0) {
        // This is the CU header
    } else {
        // These are local symbols
    }

    // Build the ROSE dwarf construct IR node (use a factory design to build the appropriate IR nodes

    // This will cause all Dwarf IR nodes to be properly represented in the AST.
    SgAsmDwarfConstruct* asmDwarfConstruct = SgAsmDwarfConstruct::createDwarfConstruct( tag, indent_level, offset, overall_offset );

    // Setup attribute specific fields in the different kinds of Dwarf nodes
    for (i = 0; i < atcnt; i++) {
        Dwarf_Half attr;
        int ares;

        ares = dwarf_whatattr(atlist[i], &attr, &rose_dwarf_error);
        if (ares == DW_DLV_OK) {
            if (asmDwarfConstruct != NULL) {
                print_attribute(dbg, die, attr, atlist[i], print_information, srcfiles, cnt, asmDwarfConstruct);
            }

            if (indent_level == 0) {
                // This is the CU header
            } else {
                // These are local symbols
            }
        } else {
            throwError(dbg, "dwarf_whatattr entry missing", ares, rose_dwarf_error);
        }
    }

    for (i = 0; i < atcnt; i++) {
        dwarf_dealloc(dbg, atlist[i], DW_DLA_ATTR);
    }

    if (atres == DW_DLV_OK) {
        dwarf_dealloc(dbg, atlist, DW_DLA_LIST);
    }

    return asmDwarfConstruct;
}



// recursively follow the die tree
static SgAsmDwarfConstruct*
buildDwarfIrNodeFromDieAndChildren(Dwarf_Debug dbg, Dwarf_Die in_die_in, char **srcfiles, Dwarf_Signed cnt,
                                   SgAsmDwarfConstruct* parentDwarfConstruct, bool isInfo) {
    // NOTE: The first time this function is called the parentDwarfConstruct is null.

    Dwarf_Die child;
    Dwarf_Die sibling;
    Dwarf_Error err;
    int cdres;
    Dwarf_Die in_die = in_die_in;

    SgAsmDwarfConstruct* astDwarfConstruct = nullptr;

    for (;;) {
        PUSH_DIE_STACK(in_die);

        // Suppress output!

        // DQ (11/4/2008): This is the location were we will have to generate IR nodes using each debug info entry (die)

        /* here to pre-descent processing of the die */
        astDwarfConstruct = build_dwarf_IR_node_from_print_one_die(dbg, in_die, isInfo, srcfiles, cnt /* , asmDwarfCompilationUnit */ );

        if (parentDwarfConstruct != NULL && astDwarfConstruct != NULL) {
            // When this work we know that we have the child support for Dwarf IR nodes in place (in all the right places).
            // According to Kewen Meng at uoregon.edu, this assertion fails when the input specimen is compiled with "gcc
            // -O1" (input and gcc version were unspecified by user), but works if -O1 is removed, or if C++ compiler
            // (vendor and version unspecified by user) is used with optimization flag (flag unspecified by user). [Robb
            // Matzke 2016-09-29]
            ASSERT_not_null(parentDwarfConstruct->get_children());
            parentDwarfConstruct->get_children()->get_list().push_back(astDwarfConstruct);
            astDwarfConstruct->set_parent(parentDwarfConstruct->get_children());
        }

        cdres = dwarf_child(in_die, &child, &err);
        /* child first: we are doing depth-first walk */
        if (cdres == DW_DLV_OK) {
            indent_level++;
            if(indent_level >= DIE_STACK_SIZE ) {
                throwError(dbg,"compiled in DIE_STACK_SIZE limit exceeded",DW_DLV_OK,err);
            }

            // Ignore the return result (children are added to the parent).
            if (astDwarfConstruct != NULL)
                buildDwarfIrNodeFromDieAndChildren(dbg, child, srcfiles, cnt, astDwarfConstruct, isInfo);
            indent_level--;

            if (indent_level == 0)
                local_symbols_already_began = false;

            dwarf_dealloc(dbg, child, DW_DLA_DIE);
        } else {
            if (cdres == DW_DLV_ERROR) {
                throwError(dbg, "dwarf_child", cdres, err);
            }
        }

        cdres = dwarf_siblingof_b(dbg, in_die, isInfo, &sibling, &err);
        if (cdres == DW_DLV_OK) {
            /* print_die_and_children(dbg, sibling, srcfiles, cnt); We
               loop around to actually print this, rather than
               recursing. Recursing is horribly wasteful of stack
               space. */
        } else {
            if (cdres == DW_DLV_ERROR) {
                throwError(dbg, "dwarf_siblingof", cdres, err);
            }
        }

        /* Here do any post-descent (ie post-dwarf_child) processing of the in_die. */

        POP_DIE_STACK;
        if (in_die != in_die_in) {
            /* Dealloc our in_die, but not the argument die, it belongs
               to our caller. Whether the siblingof call worked or not.
            */
            dwarf_dealloc(dbg, in_die, DW_DLA_DIE);
        }

        if (cdres == DW_DLV_OK) {
            /* Set to process the sibling, loop again. */
            in_die = sibling;
        } else {
            /* We are done, no more siblings at this level. */
            break;
        }
    }  /* end for loop on siblings */

    return astDwarfConstruct;
}

static void
buildDwarfLineNumbersThisCu(Dwarf_Debug debug, Dwarf_Die cuDie, SgAsmDwarfCompilationUnit *asmDwarfCompilationUnit) {
    ASSERT_not_null(asmDwarfCompilationUnit);

    // Attach an empty line list to the compilation unit.
    ASSERT_require(!asmDwarfCompilationUnit->get_line_info());
    auto asmDwarfLineList = new SgAsmDwarfLineList();
    asmDwarfCompilationUnit->set_line_info(asmDwarfLineList);
    asmDwarfLineList->set_parent(asmDwarfCompilationUnit);

    // Get the line context
    Dwarf_Unsigned version = 0;
    Dwarf_Small tableCount = 0;
    Dwarf_Line_Context lineContext;
    Dwarf_Error error;
    presentOr(dwarf_srclines_b(cuDie, &version, &tableCount, &lineContext, &error),
              "dwarf_srclines_b", tableCount, Dwarf_Small(0), debug, error);

    // Get all the lines for the line context
    Dwarf_Line *lines = nullptr;
    Dwarf_Signed lineCount = 0;
    presentOr(dwarf_srclines_from_linecontext(lineContext, &lines, &lineCount, &error),
              "dwarf_srclines_from_linecontext", lineCount, Dwarf_Signed(0), debug, error);

    // Build the SgAsmDwarfLine objects and attach them to the AST
    for (Dwarf_Signed i = 0; i < lineCount; ++i) {
        // Address
        Dwarf_Addr addr = 0;
        presentOr(dwarf_lineaddr(lines[i], &addr, &error),
                  "dwarf_lineaddr", addr, Dwarf_Addr(0), debug, error);

        // File name
        int fileNameId = -1;
        {
            char *fileName = nullptr;                   // docs say "Do not dealloc or free the string."
            presentOr(dwarf_linesrc(lines[i], &fileName, &error),
                      "dwarf_linesrc", fileName, (char*)"<unknown>", debug, error);
            fileNameId = Sg_File_Info::addFilenameToMap(fileName);
        }

        // Line number
        Dwarf_Unsigned lineNumber = 0;
        presentOr(dwarf_lineno(lines[i], &lineNumber, &error),
                  "dwarf_lineno", lineNumber, Dwarf_Unsigned(0), debug, error);

        // Column number
        Dwarf_Unsigned columnNumber = 0;
        presentOr(dwarf_lineoff_b(lines[i], &columnNumber, &error),
                  "dwarf_lineoff_b", columnNumber, Dwarf_Unsigned(0), debug, error);

        // Create the AST node for the Dwarf line and link it into the AST
        auto asmDwarfLine = new SgAsmDwarfLine(addr, fileNameId, lineNumber, columnNumber);
        asmDwarfLineList->get_line_list().push_back(asmDwarfLine);
        asmDwarfLine->set_parent(asmDwarfLineList);
    }

    dwarf_srclines_dealloc_b(lineContext);

    asmDwarfLineList->buildInstructionAddressSourcePositionMaps(asmDwarfCompilationUnit);

    if (SgProject::get_verbose() > 0)
        asmDwarfLineList->display("Inside of buildDwarfLineNumbersThisCu()");
}

// Process each compilation unit in ".debug_info".
static void
buildDwarfIrNodes(Dwarf_Debug dbg, SgAsmGenericFile* file, const Dwarf_Bool isInfo) {
    ASSERT_require(file->get_dwarf_info() == nullptr);
    SgAsmDwarfCompilationUnitList* asmDwarfCompilationUnitList = new SgAsmDwarfCompilationUnitList();
    file->set_dwarfInfo(asmDwarfCompilationUnitList);
    asmDwarfCompilationUnitList->set_parent(file);

    /* Loop until it fails.  */
    Dwarf_Unsigned cu_header_length = 0;
    Dwarf_Half version_stamp = 0;
    Dwarf_Unsigned abbrev_offset = 0;
    Dwarf_Half address_size = 0;
    Dwarf_Half length_size = 0;
    Dwarf_Half extension_size = 0;
    Dwarf_Sig8 type_signature;
    memset(&type_signature, 0, sizeof type_signature);
    Dwarf_Unsigned type_offset = 0;
    Dwarf_Unsigned next_cu_header_offset = 0;
    Dwarf_Half header_cu_type = 0;
    Dwarf_Error error = 0;
    int nres = DW_DLV_OK;
    while ((nres = dwarf_next_cu_header_d(dbg, isInfo, &cu_header_length, &version_stamp, &abbrev_offset, &address_size,
                                          &length_size, &extension_size, &type_signature, &type_offset, &next_cu_header_offset,
                                          &header_cu_type, &error)) == DW_DLV_OK) {
        /* process a single compilation unit in .debug_info. */
        Dwarf_Die cu_die = nullptr;
        const int sres = dwarf_siblingof_b(dbg, nullptr, isInfo, &cu_die, &error);
        switch (sres) {
            case DW_DLV_OK: {
                Dwarf_Signed cnt = 0;
                char **srcfiles = 0;

                // We need to call this function so that we can generate filenames to support the line number mapping below.  The call
                // to dwarf_srcfiles fails if the compilation unit `cu_die` does not have associated source file information, or if an
                // out-of-memory condition was encountered.
                const int srcf = dwarf_srcfiles(cu_die, &srcfiles, &cnt, &error);
                if (srcf != DW_DLV_OK) {
                    mlog[WARN] <<"dwarf_srcfiles call failed\n";
                    srcfiles = nullptr;
                    cnt = 0;
                }

                // This function call will traverse the list of child debug info entries and
                // within this function we will generate the IR nodes specific to Dwarf. This
                // should define an course view of the AST which can be used to relate the binary
                // to the source code.
                SgAsmDwarfConstruct* asmDwarfConstruct =
                    buildDwarfIrNodeFromDieAndChildren(dbg, cu_die, srcfiles, cnt, nullptr, isInfo);
                ASSERT_not_null(asmDwarfConstruct);

                // Handle the case of many Dwarf Compile Units
                SgAsmDwarfCompilationUnit* asmDwarfCompilationUnit = isSgAsmDwarfCompilationUnit(asmDwarfConstruct);
                ASSERT_not_null(asmDwarfCompilationUnit);

                asmDwarfCompilationUnitList->get_cu_list().push_back(asmDwarfCompilationUnit);
                asmDwarfCompilationUnit->set_parent(asmDwarfCompilationUnitList);

                if (srcf == DW_DLV_OK) {
                    for (int si = 0; si < cnt; ++si) {
                        dwarf_dealloc(dbg, srcfiles[si], DW_DLA_STRING);
                    }
                    dwarf_dealloc(dbg, srcfiles, DW_DLA_LIST);
                }

                bool line_flag = true;

                if (line_flag) {
                    buildDwarfLineNumbersThisCu(dbg, cu_die, asmDwarfCompilationUnit);
                } else {
                    mlog[INFO] << "Skipping line number <--> instruction address mapping information\n";
                }

                dwarf_dealloc(dbg, cu_die, DW_DLA_DIE);
                break;
            }
            case DW_DLV_NO_ENTRY:
                break;

            default:
                throwError(dbg, "Regetting cu_die", sres, error);
        }

        cu_offset = next_cu_header_offset;
    }

    if (nres == DW_DLV_ERROR) {
        std::string errmsg = dwarf_errmsg(error);
        Dwarf_Unsigned myerr = dwarf_errno(error);
        mlog[FATAL] << "DWARF error : " << errmsg << " (" << (unsigned long)myerr << ")\n";
    }
}


// DQ (11/10/2008):
/*! \brief This traversal permits symbols to be commented out from the DOT graphs.

 */
void
commentOutSymbolsFromDotGraph (SgNode* node) {
    // Remove the symbols and related symbol lists so that the generated DOT file will be simpler.
    // To do this we add an attribute that implements the virtual commentOutNodeInGraph() member
    // function and return "true".

    // Define the attribute class containing the virtual function "commentOutNodeInGraph()"
    class SymbolPruningAttribute : public AstAttribute {
        // This is a persistant attribute used to mark locations in the AST where we don't want IR nodes to be generated for the DOT graph.

    public:
        bool commentOutNodeInGraph() { return true; }
        virtual AstAttribute* copy() const { return new SymbolPruningAttribute(*this); }
        virtual ~SymbolPruningAttribute() {}
    };

    // Define the traversal class over the AST
    class CommentOutSymbolsFromDotGraph : public SgSimpleProcessing {
    public:
        void visit ( SgNode* node ) {

            SgAsmGenericSymbolList* genericSymbolList = isSgAsmGenericSymbolList(node);
            if (genericSymbolList != NULL) {
                SymbolPruningAttribute* att = new SymbolPruningAttribute();
                genericSymbolList->addNewAttribute("SymbolPruningAttribute",att);
            }

            SgAsmElfSymbolList* elfSymbolList = isSgAsmElfSymbolList(node);
            if (elfSymbolList != NULL) {
                SymbolPruningAttribute* att = new SymbolPruningAttribute();
                elfSymbolList->addNewAttribute("SymbolPruningAttribute",att);
            }

            SgAsmGenericSymbol* genericSymbol = isSgAsmGenericSymbol(node);
            if (genericSymbol != NULL) {
                SymbolPruningAttribute* att = new SymbolPruningAttribute();
                genericSymbol->addNewAttribute("SymbolPruningAttribute",att);
            }
        }
    };

    CommentOutSymbolsFromDotGraph traversal;
    traversal.traverse(node,preorder);
}

/* This traversal permits non-dwarf IR nodes to be commented out from the DOT graphs. */
void
commentOutEvertythingButDwarf (SgNode* node) {
    // Remove the symbols and related symbol lists so that the generated DOT file will be simpler.
    // To do this we add an attribute that implements the virtual commentOutNodeInGraph() member
    // function and return "true".

    // Define the attribute class containing the virtual function "commentOutNodeInGraph()"
    class SymbolPruningAttribute : public AstAttribute {
        // This is a persistant attribute used to mark locations in the AST where we don't want IR nodes to be generated for the DOT graph.

    public:
        bool commentOutNodeInGraph() { return true; }
        virtual AstAttribute* copy() const { return new SymbolPruningAttribute(*this); }
        virtual ~SymbolPruningAttribute() {}
    };

    // Define the traversal class over the AST
    class CommentOutSymbolsFromDotGraph : public SgSimpleProcessing {
    public:
        void visit ( SgNode* node ) {
            // Remove sections to make the Dwarf graph dominate (assume symbols were alread removed)!

            SgAsmGenericSectionList* genericSectionList = isSgAsmGenericSectionList(node);
            if (genericSectionList != NULL) {
                SymbolPruningAttribute* att = new SymbolPruningAttribute();
                genericSectionList->addNewAttribute("SymbolPruningAttribute",att);
            }

            SgAsmGenericSection* genericSection = isSgAsmGenericSection(node);
            if (genericSection != NULL) {
                SymbolPruningAttribute* att = new SymbolPruningAttribute();
                genericSection->addNewAttribute("SymbolPruningAttribute",att);
            }

            SgAsmElfSectionTableEntry* elfSectionTableEntry = isSgAsmElfSectionTableEntry(node);
            if (elfSectionTableEntry != NULL) {
                SymbolPruningAttribute* att = new SymbolPruningAttribute();
                elfSectionTableEntry->addNewAttribute("SymbolPruningAttribute",att);
            }

            SgAsmElfSegmentTableEntry* elfSegmentTableEntry = isSgAsmElfSegmentTableEntry(node);
            if (elfSegmentTableEntry != NULL) {
                SymbolPruningAttribute* att = new SymbolPruningAttribute();
                elfSegmentTableEntry->addNewAttribute("SymbolPruningAttribute",att);
            }

            SgAsmElfRelocEntry* elfRelocEntry = isSgAsmElfRelocEntry(node);
            if (elfRelocEntry != NULL) {
                SymbolPruningAttribute* att = new SymbolPruningAttribute();
                elfRelocEntry->addNewAttribute("SymbolPruningAttribute",att);
            }

            SgAsmElfRelocEntryList* elfRelocEntryList = isSgAsmElfRelocEntryList(node);
            if (elfRelocEntryList != NULL) {
                SymbolPruningAttribute* att = new SymbolPruningAttribute();
                elfRelocEntryList->addNewAttribute("SymbolPruningAttribute",att);
            }

            SgAsmElfDynamicEntryList* elfDynamicEntryList = isSgAsmElfDynamicEntryList(node);
            if (elfDynamicEntryList != NULL) {
                SymbolPruningAttribute* att = new SymbolPruningAttribute();
                elfDynamicEntryList->addNewAttribute("SymbolPruningAttribute",att);
            }

            SgAsmGenericFormat* genericFormat = isSgAsmGenericFormat(node);
            if (genericFormat != NULL) {
                SymbolPruningAttribute* att = new SymbolPruningAttribute();
                genericFormat->addNewAttribute("SymbolPruningAttribute",att);
            }

            SgAsmGenericDLLList* genericDLLList = isSgAsmGenericDLLList(node);
            if (genericDLLList != NULL) {
                SymbolPruningAttribute* att = new SymbolPruningAttribute();
                genericDLLList->addNewAttribute("SymbolPruningAttribute",att);
            }
        }
    };

    // Comment out the symbols (factored out)
    commentOutSymbolsFromDotGraph(node);

    // Now comment out the rest of the non-dwarf IR nodes.
    CommentOutSymbolsFromDotGraph traversal;
    traversal.traverse(node,preorder);
}

void
readDwarf(SgAsmGenericFile *file) {
    ASSERT_not_null(file);
    const int fileDescriptor = file->get_fd();
    Dwarf_Debug debug;
    Dwarf_Error error;
    const Dwarf_Bool isInfo = true;

    // DQ (3/13/2009): This function gets a lot of surrounding debugging information because it is the first dwarf function call and
    // is a special problem in the compatability between Intel Pin and ROSE over the use of Dwarf.
    const int dwarfInitStatus = dwarf_init_b(fileDescriptor, DW_GROUPNUMBER_ANY, nullptr, nullptr, &debug, &error);
    if (dwarfInitStatus == DW_DLV_OK) {
        // Dwarf information in a specimen is attached to the specimen's file as a whole.  It is not attached to an interpretation
        // since the relationship between SgAsmGenericFile and SgAsmInterpretation is many-to-many.
        buildDwarfIrNodes(debug, file, isInfo);

        const int dwarfFinishStatus = dwarf_finish(debug);
        ASSERT_always_require(dwarfFinishStatus == DW_DLV_OK);
    } else {
        // This might be a PE file (or just non-ELF)
        if (SgProject::get_verbose() > 0) {
            mlog[INFO] << "No dwarf debug sections found\n";
        }
    }

    // DQ (11/10/2008): Added support to permit symbols to be removed from the DOT graph generation.  This makes the DOT files
    // easier to manage since there can be thousands of symbols.  This also makes it easer to debug the ROSE dwarf AST.
    if (SgBinaryComposite* binary = SageInterface::getEnclosingNode<SgBinaryComposite>(file)) {
        // This is used to reduce the size of the DOT file to simplify debugging Dwarf stuff.
        if (binary->get_visualize_executable_file_format_skip_symbols()) {
            mlog[DEBUG] <<"calling commentOutSymbolsFromDotGraph() for visualization of binary file format withouth symbols\n";
            commentOutSymbolsFromDotGraph(file);
        }

        // Nothing but dwarf!
        if (binary->get_visualize_dwarf_only()) {
            mlog[DEBUG] <<"calling commentOutEvertythingButDwarf() for visualization of Dwarf)\n";
            commentOutEvertythingButDwarf(file);
        }
    }
}
#endif // ROSE_HAVE_LIBDWARF

void
parse(SgAsmGenericFile *file) {
#ifdef ROSE_HAVE_LIBDWARF
    return readDwarf(file);
#else
    ROSE_UNUSED(file);
    mlog[INFO] <<"DWARF parsing is not enabled in this ROSE configuration\n";
#endif
}

} // namespace
} // namespace
} // namespace

#endif
