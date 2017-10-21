/*
   Copyright (C) 2001-2017 Free Software Foundation, Inc.

   Authors:
   Keisuke Nishida, Roger While, Ron Norman, Simon Sobisch, Brian Tiffin,
   Edward Hart, Dave Pitts

   This file is part of GnuCOBOL.

   The GnuCOBOL compiler is free software: you can redistribute it
   and/or modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   GnuCOBOL is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GnuCOBOL.  If not, see <http://www.gnu.org/licenses/>.
*/

/* #define DEBUG_REPLACE */

#include "config.h"
#include "defaults.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#ifdef	HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef	HAVE_SIGNAL_H
#include <signal.h>
#endif

#ifdef	_WIN32
#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef MOUSE_MOVED
#include <io.h>
#include <fcntl.h>
#endif

#ifdef	HAVE_LOCALE_H
#include <locale.h>
#endif

#include "tarstamp.h"

#include "cobc.h"
#include "tree.h"

#include "lib/gettext.h"

#include "libcob/cobgetopt.h"

#if defined (COB_EXPERIMENTAL) || 1
#define COB_INTERNAL_XREF
enum xref_type {
	XREF_FIELD,
	XREF_FILE,
	XREF_LABEL,
	XREF_FUNCTION
};
#endif

struct strcache {
	struct strcache	*next;
	void		*val;
};

/* Compile level */
#define	CB_LEVEL_PREPROCESS	1
#define	CB_LEVEL_TRANSLATE	2
#define	CB_LEVEL_COMPILE	3
#define	CB_LEVEL_ASSEMBLE	4
#define	CB_LEVEL_MODULE		5
#define	CB_LEVEL_LIBRARY	6
#define	CB_LEVEL_EXECUTABLE	7

/* Info display limits */
#define	CB_IMSG_SIZE		24
#define	CB_IVAL_SIZE		(74 - CB_IMSG_SIZE - 4)

#define	COBC_ADD_STR(v,x,y,z)	cobc_add_str (&v, &v##_size, x, y, z);
#define	COBC_INV_PAR		_("invalid parameter: %s")

/* C version info */
#ifdef	__VERSION__
#define OC_C_VERSION_PRF	""
#define OC_C_VERSION	CB_XSTRINGIFY(__VERSION__)
#elif	defined(__xlc__)
#define OC_C_VERSION_PRF	"(IBM XL C/C++) "
#define OC_C_VERSION	CB_XSTRINGIFY(__xlc__)
#elif	defined(__SUNPRO_CC)
#define OC_C_VERSION_PRF	"(Sun C++) "
#define OC_C_VERSION	CB_XSTRINGIFY(__SUNPRO_CC)
#elif	defined(_MSC_VER)
#define OC_C_VERSION_PRF	"(Microsoft) "
#define OC_C_VERSION	CB_XSTRINGIFY(_MSC_VER)
#elif	defined(__BORLANDC__)
#define OC_C_VERSION_PRF	"(Borland) "
#define OC_C_VERSION	CB_XSTRINGIFY(__BORLANDC__)
#elif	defined(__WATCOMC__)
#define OC_C_VERSION_PRF	"(Watcom) "
#define OC_C_VERSION	CB_XSTRINGIFY(__WATCOMC__)
#elif	defined(__INTEL_COMPILER)
#define OC_C_VERSION_PRF	"(Intel) "
#define OC_C_VERSION	CB_XSTRINGIFY(__INTEL_COMPILER)
#else
#define OC_C_VERSION_PRF	""
#define OC_C_VERSION	_("unknown")
#endif

#define	CB_TEXT_LIST_ADD(y,z)	y = cb_text_list_add (y, z)
#define	CB_TEXT_LIST_CHK(y,z)	y = cb_text_list_chk (y, z)

#ifdef	_MSC_VER
#define	CB_COPT_1	" /O1"
#define	CB_COPT_2	" /O2"
#define	CB_COPT_3	" /Ox"
#define	CB_COPT_S	" /Os"
#elif   defined(__BORLANDC__)
#define	CB_COPT_1	" -O"
#define	CB_COPT_2	" -O2"
#define	CB_COPT_3	" -O3"
#define	CB_COPT_S	" -O1"
#elif defined(__hpux) && !defined(__GNUC__)
#define	CB_COPT_1	" -O"
#define	CB_COPT_2	" +O2"
#define	CB_COPT_3	" +O3"
#define	CB_COPT_S	" +O2 +Osize"
#elif   defined(__WATCOMC__)
#define	CB_COPT_1	" -ot"
#define	CB_COPT_2	" -ox"
#define	CB_COPT_3	" -ox -oh"
#define	CB_COPT_S	" -os"
#elif   defined(__SUNPRO_C)
#define	CB_COPT_1	" -xO1"
#define	CB_COPT_2	" -xO2"
#define	CB_COPT_3	" -xO2"	/* Oracle docs are confusing, is -xO3 working? */
#define	CB_COPT_S	" -xO1 -xspace"
#elif	defined(__xlc__)
#define	CB_COPT_1	" -O"
#define	CB_COPT_2	" -O2"
#define	CB_COPT_2	" -O3"
#define	CB_COPT_S	" -O"
#else
#define	CB_COPT_1	" -O"
#define	CB_COPT_2	" -O2"
#define	CB_COPT_3	" -O3"
#define	CB_COPT_S	" -Os"
#endif

/* Replacement for the GNUCobol codegen function (to build ROSE IR nodes) */
rose_cobol_codegen (struct cb_program *prog, const int subsequent_call);

/* Global variables */

const char		*cb_source_file = NULL;
const char		*cb_cobc_build_stamp = NULL;
const char		*demangle_name = NULL;
const char		*cb_storage_file_name = NULL;
struct cb_text_list	*cb_include_list = NULL;
struct cb_text_list	*cb_intrinsic_list = NULL;
struct cb_text_list	*cb_extension_list = NULL;
struct cb_text_list	*cb_static_call_list = NULL;
struct cb_text_list	*cb_early_exit_list = NULL;
char			**cb_saveargv = NULL;
const char		*cob_config_dir = NULL;
FILE			*cb_storage_file = NULL;
FILE			*cb_listing_file = NULL;

/* Listing structures and externals */

#define CB_LINE_LENGTH	1024 /* hint: we only read PPLEX_BUF_LEN bytes */
#define CB_READ_AHEAD	800 /* lines to read ahead */

/* TODO: add new compiler configuration flags for this*/
#define CB_INDICATOR	6
#define CB_MARGIN_A	7
#define CB_MARGIN_B	11	/* careful, for COBOL 85 this would be 11,
						   for COBOL 2002 (removed it) would be 7 */
#define CB_SEQUENCE	cb_text_column /* the only configuration available...*/
#define CB_ENDLINE	cb_text_column + 8

#define CB_MAX_LINES	55
#define CB_LIST_PICSIZE 80
#define CB_PRINT_LEN	132

char	print_data[CB_PRINT_LEN + 1];
int	pd_off;

#define IS_DEBUG_LINE(line) ((line)[CB_INDICATOR] == 'D')
#define IS_CONTINUE_LINE(line) ((line)[CB_INDICATOR] == '-')
#define IS_COMMENT_LINE(line) \
   ((line)[CB_INDICATOR] == '*' || (line)[CB_INDICATOR] == '/')

FILE			*cb_src_list_file = NULL;
int			cb_listing_page = 0;
int			cb_listing_wide = 0;
unsigned int		cb_lines_per_page = CB_MAX_LINES;
int			cb_listing_symbols = 0;
int			cb_listing_xref = 0;
#define			CB_LISTING_DATE_BUFF 26
#define			CB_LISTING_DATE_MAX (CB_LISTING_DATE_BUFF - 1)
char			cb_listing_date[CB_LISTING_DATE_BUFF]; /* Date/Time buffer for listing */
struct list_files	*cb_current_file = NULL;
#define			LCL_NAME_LEN 80
#define			LCL_NAME_MAX (LCL_NAME_LEN - 1)

/* compilation date/time of current source file */
struct cob_time		current_compile_time = { 0 };
struct tm		current_compile_tm = { 0 };

#if	0	/* RXWRXW - source format */
char			*source_name = NULL;
#endif

int			cb_source_format = CB_FORMAT_FIXED;
int			cb_text_column;
int			cb_id = 0;
int			cb_pic_id = 0;
int			cb_attr_id = 0;
int			cb_literal_id = 0;
int			cb_field_id = 0;
int			cobc_flag_main = 0;
int			cb_flag_main = 0;
int			cobc_wants_debug = 0;
int			cb_flag_functions_all = 0;
int			cobc_seen_stdin = 0;
int			cb_unix_lf = 0;

int			errorcount = 0;
int			warningcount = 0;
int			warningopt = 0;
int			fatal_errors_flag = 0;
int			no_physical_cancel = 0;
int			cb_source_line = 0;
int			cb_saveargc = 0;
unsigned int		cobc_gen_listing = 0;

cob_u32_t		optimize_defs[COB_OPTIM_MAX] = { 0 };

#define	COB_EXCEPTION(code,tag,name,critical) {name, 0x##code, 0},
struct cb_exception cb_exception_table[] = {
	{NULL, 0, 0},		/* CB_EC_ZERO */
#include "libcob/exception.def"
	{NULL, 0, 0}		/* CB_EC_MAX */
};
#undef	COB_EXCEPTION

#define	CB_FLAG(var,pdok,name,doc)	int var = 0;
#define	CB_FLAG_ON(var,pdok,name,doc)	int var = 1;
#define	CB_FLAG_RQ(var,pdok,name,def,opt,doc,vdoc,ddoc)	int var = def;
#define	CB_FLAG_NQ(pdok,name,opt,doc,vdoc)
#include "flag.def"
#undef	CB_FLAG
#undef	CB_FLAG_ON
#undef	CB_FLAG_RQ
#undef	CB_FLAG_NQ

#define	CB_WARNDEF(var,name,doc)	int var = 0;
#define	CB_ONWARNDEF(var,name,doc)	int var = 1;
#define	CB_NOWARNDEF(var,name,doc)	int var = 0;
#include "warning.def"
#undef	CB_WARNDEF
#undef	CB_ONWARNDEF
#undef	CB_NOWARNDEF

/* Local variables */

static struct cb_define_struct	*cb_define_list = NULL;

static struct cobc_mem_struct	*cobc_mainmem_base = NULL;
static struct cobc_mem_struct	*cobc_parsemem_base = NULL;
static struct cobc_mem_struct	*cobc_plexmem_base = NULL;

static const char	*cobc_cc;		/* C compiler */
static char		*cobc_cflags;		/* C compiler flags */
static char		*cobc_libs;		/* -l... */
static char		*cobc_lib_paths;	/* -L... */
static char		*cobc_include;		/* -I... */
static char		*cobc_ldflags;		/* -Q / COB_LDFLAGS */

static size_t		cobc_cflags_size;
static size_t		cobc_libs_size;
static size_t		cobc_lib_paths_size;
static size_t		cobc_include_size;
static size_t		cobc_ldflags_size;

static size_t		cobc_cc_len;
static size_t		cobc_cflags_len;
static size_t		cobc_libs_len;
static size_t		cobc_lib_paths_len;
static size_t		cobc_include_len;
static size_t		cobc_ldflags_len;
static size_t		cobc_export_dyn_len;
static size_t		cobc_shared_opt_len;
static size_t		cobc_pic_flags_len;

static char		*save_temps_dir = NULL;
static struct strcache	*base_string;

static char		*cobc_list_dir = NULL;
static char		*cobc_list_file = NULL;

static char		*output_name = NULL;
static char		*cobc_buffer;
static char		*cobc_objects_buffer;
static char		*output_name_buff;
static char		*basename_buffer;

static size_t		cobc_objects_len;
static size_t		basename_len;
static size_t		cobc_buffer_size;

static struct filename	*file_list;

static unsigned int	cb_compile_level = 0;

static int		iargs;

static size_t		wants_nonfinal = 0;
static size_t		cobc_flag_module = 0;
static size_t		cobc_flag_library = 0;
static size_t		cobc_flag_run = 0;
static char		*cobc_run_args = NULL;
static size_t		save_temps = 0;
static size_t		save_all_src = 0;
static size_t		save_c_src = 0;
static signed int	verbose_output = 0;
static size_t		cob_optimize = 0;

static unsigned int		cb_listing_linecount;
static int		cb_listing_eject = 0;
static char		cb_listing_filename[FILENAME_MAX];
static char		*cb_listing_outputfile = NULL;
static char		cb_listing_title[81];	/* Listing title (defaults to PACKAGE_NAME + Version */
static char		cb_listing_header[133];	/* Listing header */
static struct list_files	*cb_listing_file_struct = NULL;
static struct list_error	*cb_listing_error_head = NULL;
static struct list_error	*cb_listing_error_tail = NULL;

#ifdef	_MSC_VER
static const char	*manicmd;
static const char	*manilink;
static size_t		manilink_len;
#define PATTERN_DELIM '|'
#endif

static size_t		strip_output = 0;
static size_t		gflag_set = 0;
static size_t		aflag_set = 0;

static const char	*const cob_csyns[] = {
#ifndef	COB_EBCDIC_MACHINE
	"NULL",
	"P_cancel",
	"P_initialize",
	"P_ret_initialize",
	"P_switch",
#endif
#ifdef	COB_EBCDIC_MACHINE
	"_float128",
#endif
	"_Bool",
	"_Complex",
	"_Imaginary",
#ifndef	COB_EBCDIC_MACHINE
	"_float128",
#endif
	"alignof",
	"asm",
	"auto",
	"bool",
	"break",
	"case",
	"catch",
	"char",
	"class",
	"const",
	"const_cast",
	"continue",
	"default",
	"delete",
	"do",
	"double",
	"dynamic_cast",
	"else",
	"enum",
	"exit_program",
	"explicit",
	"extern",
	"false",
	"float",
	"for",
	"frame_pointer",
	"frame_stack",
	"friend",
	"goto",
	"if",
	"inline",
	"int",
	"long",
	"mutable",
	"namespace",
	"new",
	"offsetof",
	"operator",
	"private",
	"protected",
	"public",
	"register",
	"reinterpret_cast",
	"restrict",
	"return",
	"short",
	"signed",
	"sizeof",
	"static",
	"static_cast",
	"struct",
	"switch",
	"template",
	"this",
	"throw",
	"true",
	"try",
	"typedef",
	"typeid",
	"typename",
	"typeof",
	"union",
	"unsigned",
	"using",
	"virtual",
	"void",
	"volatile",
#ifndef	COB_EBCDIC_MACHINE
	"wchar_t"
#else
	"wchar_t",
	"NULL",
	"P_cancel",
	"P_initialize",
	"P_ret_initialize",
	"P_switch"
#endif
};

#define COB_NUM_CSYNS	sizeof(cob_csyns) / sizeof(char *)

static const char short_options[] = "hVivqECScbmxjdFROPgwo:t:T:I:L:l:D:K:k:";

#define	CB_NO_ARG	no_argument
#define	CB_RQ_ARG	required_argument
#define	CB_OP_ARG	optional_argument

static const struct option long_options[] = {
	{"help",		CB_NO_ARG, NULL, 'h'},
	{"version",		CB_NO_ARG, NULL, 'V'},
	{"verbose",		CB_OP_ARG, NULL, 'v'},
	{"brief",		CB_NO_ARG, NULL, 'q'},
	{"###",			CB_NO_ARG, NULL, '#'},
	{"info",		CB_NO_ARG, NULL, 'i'},
	{"list-reserved",	CB_NO_ARG, NULL, '5'},
	{"list-intrinsics",	CB_NO_ARG, NULL, '6'},
	{"list-mnemonics",	CB_NO_ARG, NULL, '7'},
	{"list-system",		CB_NO_ARG, NULL, '8'},
	{"list-registers",		CB_NO_ARG, NULL, '9'},
	{"O2",			CB_NO_ARG, NULL, '2'},
	{"O3",			CB_NO_ARG, NULL, '3'},
	{"Os",			CB_NO_ARG, NULL, 's'},
	{"save-temps",		CB_OP_ARG, NULL, '_'},
	{"std",			CB_RQ_ARG, NULL, '$'},
	{"conf",		CB_RQ_ARG, NULL, '&'},
	{"debug",		CB_NO_ARG, NULL, 'd'},
	{"ext",			CB_RQ_ARG, NULL, 'e'},
	{"free",		CB_NO_ARG, &cb_source_format, CB_FORMAT_FREE},
	{"fixed",		CB_NO_ARG, &cb_source_format, CB_FORMAT_FIXED},
	{"static",		CB_NO_ARG, &cb_flag_static_call, 1},
	{"dynamic",		CB_NO_ARG, &cb_flag_static_call, 0},
	{"job",			CB_OP_ARG, NULL, 'j'},
	{"j",			CB_OP_ARG, NULL, 'j'},
	{"Q",			CB_RQ_ARG, NULL, 'Q'},
	{"A",			CB_RQ_ARG, NULL, 'A'},
	{"P",			CB_OP_ARG, NULL, 'P'},
	{"Xref",		CB_NO_ARG, NULL, 'X'},
	{"Wall",		CB_NO_ARG, NULL, 'W'},
	{"Werror",		CB_OP_ARG, NULL, 'Y'},
	{"W",			CB_NO_ARG, NULL, 'Z'},
	{"tlines", 		CB_RQ_ARG, NULL, '*'},
	{"tsymbols", 		CB_NO_ARG, &cb_listing_symbols, 1},

#define	CB_FLAG(var,pdok,name,doc)			\
	{"f" name,		CB_NO_ARG, &var, 1},	\
	{"fno-" name,		CB_NO_ARG, &var, 0},
#define	CB_FLAG_ON(var,pdok,name,doc)		\
	{"f" name,		CB_NO_ARG, &var, 1},	\
	{"fno-" name,		CB_NO_ARG, &var, 0},
#define	CB_FLAG_RQ(var,pdok,name,def,opt,doc,vdoc,ddoc)		\
	{"f" name,		CB_RQ_ARG, NULL, opt},
#define	CB_FLAG_NQ(pdok,name,opt,doc,vdoc)			\
	{"f" name,		CB_RQ_ARG, NULL, opt},
#include "flag.def"
#undef	CB_FLAG
#undef	CB_FLAG_ON
#undef	CB_FLAG_RQ
#undef	CB_FLAG_NQ

#define	CB_CONFIG_ANY(type,var,name,doc)	\
	{"f" name,		CB_RQ_ARG, NULL, '%'},
#define	CB_CONFIG_INT(var,name,min,max,odoc,doc)	\
	{"f" name,		CB_RQ_ARG, NULL, '%'},
#define	CB_CONFIG_STRING(var,name,doc)		\
	{"f" name,		CB_RQ_ARG, NULL, '%'},
#define	CB_CONFIG_BOOLEAN(var,name,doc)		\
	{"f" name,		CB_NO_ARG, &var, 1},	\
	{"fno-" name,		CB_NO_ARG, &var, 0},
#define	CB_CONFIG_SUPPORT(var,name,doc)		\
	{"f" name,		CB_RQ_ARG, NULL, '%'},
#include "config.def"
#undef	CB_CONFIG_ANY
#undef	CB_CONFIG_INT
#undef	CB_CONFIG_STRING
#undef	CB_CONFIG_BOOLEAN
#undef	CB_CONFIG_SUPPORT
	{"freserved",	CB_RQ_ARG, NULL, '%'},
	{"fnot-reserved",	CB_RQ_ARG, NULL, '%'},
	{"fintrinsic-function",	CB_RQ_ARG, NULL, '%'},
	{"fnot-intrinsic-function",	CB_RQ_ARG, NULL, '%'},
	{"fsystem-name",	CB_RQ_ARG, NULL, '%'},
	{"fnot-system-name",	CB_RQ_ARG, NULL, '%'},
	{"fregister",	CB_RQ_ARG, NULL, '%'},
	{"fnot-register",	CB_RQ_ARG, NULL, '%'},

#define	CB_WARNDEF(var,name,doc)			\
	{"W" name,		CB_NO_ARG, &var, 1},	\
	{"Wno-" name,		CB_NO_ARG, &var, 0},
#define	CB_ONWARNDEF(var,name,doc)			\
	{"W" name,		CB_NO_ARG, &var, 1},	\
	{"Wno-" name,		CB_NO_ARG, &var, 0},
#define	CB_NOWARNDEF(var,name,doc)			\
	{"W" name,		CB_NO_ARG, &var, 1},	\
	{"Wno-" name,		CB_NO_ARG, &var, 0},
#include "warning.def"
#undef	CB_WARNDEF
#undef	CB_ONWARNDEF
#undef	CB_NOWARNDEF
	{"Wfatal-errors",	CB_NO_ARG, &fatal_errors_flag, 1},
	{"Wno-fatal-errors",	CB_NO_ARG, &fatal_errors_flag, 0},

	{NULL,			0, NULL, 0}
};

#undef	CB_ARG_NO
#undef	CB_ARG_RQ
#undef	CB_ARG_NQ
#undef	CB_ARG_OP

/* Prototype */
DECLNORET static void COB_A_NORETURN	cobc_abort_terminate (void);
static void	free_list_file		(struct list_files *);
static void	print_program_code	(struct list_files *, int);
static void	set_standard_title	(void);
static void	print_program_header	(void);
static void	print_program_data	(const char *);
static void	print_program_trailer	(void);
static void	print_program_listing	(void);
static int	process			(const char *);

/* cobc functions */

static void
cobc_free_mem (void)
{
	struct cobc_mem_struct	*reps;
	struct cobc_mem_struct	*repsl;

	if (save_temps_dir) {
		cobc_free (save_temps_dir);
		save_temps_dir = NULL;
	}
	if (cobc_list_dir) {
		cobc_free (cobc_list_dir);
		cobc_list_dir = NULL;
	}
	if (cobc_list_file) {
		cobc_free (cobc_list_file);
		cobc_list_file = NULL;
	}
	if (cb_listing_file_struct) {
		free_list_file (cb_listing_file_struct);
		cb_listing_file_struct = NULL;
	}
	if (cobc_run_args) {
		cobc_free (cobc_run_args);
		cobc_run_args = NULL;
	}
	for (reps = cobc_plexmem_base; reps; ) {
		repsl = reps;
		reps = reps->next;
		cobc_free (repsl);
	}
	cobc_plexmem_base = NULL;
	for (reps = cobc_parsemem_base; reps; ) {
		repsl = reps;
		reps = reps->next;
		cobc_free (repsl);
	}
	cobc_parsemem_base = NULL;
	for (reps = cobc_mainmem_base; reps; ) {
		repsl = reps;
		reps = reps->next;
		cobc_free (repsl);
	}
	cobc_mainmem_base = NULL;
	ppp_clear_lists ();
}

static const char *
cobc_enum_explain (const enum cb_tag tag)
{
	switch (tag) {
	case CB_TAG_CONST:
		return "CONSTANT";
	case CB_TAG_INTEGER:
		return "INTEGER";
	case CB_TAG_STRING:
		return "STRING";
	case CB_TAG_ALPHABET_NAME:
		return "ALPHABET";
	case CB_TAG_CLASS_NAME:
		return "CLASS";
	case CB_TAG_LOCALE_NAME:
		return "LOCALE";
	case CB_TAG_SYSTEM_NAME:
		return "SYSTEM";
	case CB_TAG_LITERAL:
		return "LITERAL";
	case CB_TAG_DECIMAL:
		return "DECIMAL";
	case CB_TAG_DECIMAL_LITERAL:
		return "DECIMAL_LITERAL";
	case CB_TAG_FIELD:
		return "FIELD";
	case CB_TAG_FILE:
		return "FILE";
	case CB_TAG_REPORT:
		return "REPORT";
	case CB_TAG_REFERENCE:
		return "REFERENCE";
	case CB_TAG_BINARY_OP:
		return "BINARY OP";
	case CB_TAG_FUNCALL:
		return "FUNCTION CALL";
	case CB_TAG_CAST:
		return "CAST";
	case CB_TAG_INTRINSIC:
		return "INTRINSIC";
	case CB_TAG_LABEL:
		return "LABEL";
	case CB_TAG_ASSIGN:
		return "ASSIGN";
	case CB_TAG_INITIALIZE:
		return "INITIALIZE";
	case CB_TAG_SEARCH:
		return "SEARCH";
	case CB_TAG_CALL:
		return "CALL";
	case CB_TAG_GOTO:
		return "GO TO";
	case CB_TAG_IF:
		return "IF";
	case CB_TAG_PERFORM:
		return "PERFORM";
	case CB_TAG_STATEMENT:
		return "STATEMENT";
	case CB_TAG_CONTINUE:
		return "CONTINUE";
	case CB_TAG_CANCEL:
		return "CANCEL";
	case CB_TAG_ALTER:
		return "ALTER";
	case CB_TAG_SET_ATTR:
		return "SET ATTRIBUTE";
	case CB_TAG_PERFORM_VARYING:
		return "PERFORM";
	case CB_TAG_PICTURE:
		return "PICTURE";
	case CB_TAG_LIST:
		return "LIST";
	case CB_TAG_DIRECT:
		return "DIRECT";
	case CB_TAG_DEBUG:
		return "DEBUG";
	case CB_TAG_DEBUG_CALL:
		return "DEBUG CALL";
	default:
		break;
	}
	return "UNKNOWN";
}

static const char *
cobc_enum_explain_storage (const enum cb_storage storage)
{
	switch (storage) {
	case CB_STORAGE_CONSTANT:
		return "Constants";
	case CB_STORAGE_FILE:
		return "FILE SECTION";
	case CB_STORAGE_WORKING:
		return "WORKING-STORAGE SECTION";
	case CB_STORAGE_LOCAL:
		return "LOCAL-STORAGE SECTION";
	case CB_STORAGE_LINKAGE:
		return "LINKAGE SECTION";
	case CB_STORAGE_SCREEN:
		return "SCREEN SECTION";
	case CB_STORAGE_REPORT:
		return "REPORT SECTION";
	case CB_STORAGE_COMMUNICATION:
		return "COMMUNICATION SECTION";
	default:
		break;
	}
	return "UNKNOWN";
}

static void
free_error_list (struct list_error *err)
{
	struct list_error	*next;

	do {
		if (err->file) {
			cobc_free (err->file);
		}
		if (err->prefix) {
			cobc_free (err->prefix);
		}
		if (err->msg) {
			cobc_free (err->msg);
		}

		next = err->next;
		cobc_free (err);
		err = next;
	} while (err);
}

static void
free_replace_list (struct list_replace *replace)
{
	struct list_replace	*next;

	do {
		if (replace->from) {
			cobc_free (replace->from);
		}
		if (replace->to) {
			cobc_free (replace->to);
		}

		next = replace->next;
		cobc_free (replace);
		replace = next;
	} while (replace);
}

static void
free_list_skip (struct list_skip *skip)
{
	struct list_skip	*next;

	do {
		next = skip->next;
		cobc_free (skip);
		skip = next;
	} while (skip);

}

static void
free_list_file (struct list_files *list_files_struct)
{
	struct list_files	*next;

	do {
		/* Delete the resources held by the struct. */
		if (list_files_struct->copy_head) {
			free_list_file (list_files_struct->copy_head);
		}
		if (list_files_struct->err_head) {
			free_error_list (list_files_struct->err_head);
		}
		if (list_files_struct->replace_head) {
			free_replace_list (list_files_struct->replace_head);
		}
		if (list_files_struct->skip_head) {
			free_list_skip (list_files_struct->skip_head);
		}
		if (list_files_struct->name) {
			cobc_free ((char *) list_files_struct->name);
		}

		/* Delete the struct itself */
		next = list_files_struct->next;
		cobc_free (list_files_struct);
		list_files_struct = next;
	} while (list_files_struct);
}

/* Global functions */

/* Output a formatted message to stderr */
void
cobc_err_msg (const char *fmt, ...)
{
	va_list		ap;

	fprintf (stderr, "cobc: ");
	va_start (ap, fmt);
	vfprintf (stderr, fmt, ap);
	va_end (ap);
	putc ('\n', stderr);
	fflush (stderr);
}

void
cobc_too_many_errors (void)
{
	cobc_err_msg (_("too many errors"));
	cobc_abort_terminate ();
}

/* Output cobc source/line where an internal error occurs and exit */
/* LCOV_EXCL_START */
void
cobc_abort (const char * filename, const int line_num)
{
	cobc_err_msg (_("%s: %d: internal compiler error"), filename, line_num);
	cobc_err_msg (_("Please report this!"));
	cobc_abort_terminate ();
}
/* LCOV_EXCL_STOP */

/* Output cobc source/line where a tree cast error occurs and exit */
/* LCOV_EXCL_START */
void
cobc_tree_cast_error (const cb_tree x, const char * filename, const int line_num,
		      const enum cb_tag tagnum)
{
	cobc_err_msg (_("%s: %d: invalid cast from '%s' type %s to type %s"),
		filename, line_num,
		x ? cb_name (x) : "NULL",
		x ? cobc_enum_explain (CB_TREE_TAG(x)) : "None",
		cobc_enum_explain (tagnum));
	cobc_abort_terminate ();
}
/* LCOV_EXCL_STOP */

#if	!defined(__GNUC__) && defined(COB_TREE_DEBUG)
/* LCOV_EXCL_START */
cb_tree
cobc_tree_cast_check (const cb_tree x, const char * file,
		      const int line, const enum cb_tag tag)
{
	if (!x || x == cb_error_node || CB_TREE_TAG (x) != tag) {
		cobc_tree_cast_error (x, file, line, tag);
	}
	return x;
}
/* LCOV_EXCL_STOP */
#endif

void *
cobc_malloc (const size_t size)
{
	void	*mptr;

	mptr = calloc ((size_t)1, size);
	if (unlikely(!mptr)) {
		/* LCOV_EXCL_START */
		cobc_err_msg (_("cannot allocate %d bytes of memory"),
				(int)size);
		cobc_abort_terminate ();
		/* LCOV_EXCL_STOP */
	}
	return mptr;
}

void
cobc_free (void * mptr)
{
	if (unlikely(!mptr)) {
		/* LCOV_EXCL_START */
		cobc_err_msg (_("call to %s with NULL pointer"), "cobc_free");
		cobc_abort_terminate ();
		/* LCOV_EXCL_STOP */
	}
	free(mptr);
}

void *
cobc_strdup (const char *dupstr)
{
	void	*p;
	size_t	n;

#ifdef	COB_TREE_DEBUG
	if (unlikely(!dupstr)) {
		/* LCOV_EXCL_START */
		cobc_err_msg (_("call to %s with NULL pointer"), "cobc_strdup");
		cobc_abort_terminate ();
		/* LCOV_EXCL_STOP */
	}
#endif
	n = strlen (dupstr);
	p = cobc_malloc (n + 1);
	memcpy (p, dupstr, n);
	return p;
}

void *
cobc_realloc (void *prevptr, const size_t size)
{
	void	*mptr;

	mptr = realloc (prevptr, size);
	if (unlikely(!mptr)) {
		/* LCOV_EXCL_START */
		cobc_err_msg (_("cannot reallocate %d bytes of memory"),
				(int)size);
		cobc_abort_terminate ();
		/* LCOV_EXCL_STOP */
	}
	return mptr;
}

/* Memory allocate/strdup/reallocate/free for complete execution */
void *
cobc_main_malloc (const size_t size)
{
	struct cobc_mem_struct	*m;

	m = calloc ((size_t)1, sizeof(struct cobc_mem_struct) + size);
	if (unlikely(!m)) {
		/* LCOV_EXCL_START */
		cobc_err_msg (_("cannot allocate %d bytes of memory"),
				(int)size);
		cobc_abort_terminate ();
		/* LCOV_EXCL_STOP */
	}
	m->next = cobc_mainmem_base;
	m->memptr = (char *)m + sizeof (struct cobc_mem_struct);
	m->memlen = size;
	cobc_mainmem_base = m;
	return m->memptr;
}

void *
cobc_main_strdup (const char *dupstr)
{
	void	*p;
	size_t	n;

	if (unlikely(!dupstr)) {
		/* LCOV_EXCL_START */
		cobc_err_msg (_("call to %s with NULL pointer"), "cobc_main_strdup");
		cobc_abort_terminate ();
		/* LCOV_EXCL_STOP */
	}
	n = strlen (dupstr);
	p = cobc_main_malloc (n + 1);
	memcpy (p, dupstr, n);
	return p;
}

void *
cobc_main_realloc (void *prevptr, const size_t size)
{
	struct cobc_mem_struct	*m;
	struct cobc_mem_struct	*curr;
	struct cobc_mem_struct	*prev;

	m = calloc ((size_t)1, sizeof(struct cobc_mem_struct) + size);
	if (unlikely(!m)) {
		/* LCOV_EXCL_START */
		cobc_err_msg (_("cannot allocate %d bytes of memory"),
				(int)size);
		cobc_abort_terminate ();
		/* LCOV_EXCL_STOP */
	}
	m->memptr = (char *)m + sizeof (struct cobc_mem_struct);
	m->memlen = size;

	prev = NULL;
	for (curr = cobc_mainmem_base; curr; curr = curr->next) {
		if (curr->memptr == prevptr) {
			break;
		}
		prev = curr;
	}
	if (unlikely(!curr)) {
		/* LCOV_EXCL_START */
		cobc_err_msg (_("attempt to reallocate non-allocated memory"));
		cobc_abort_terminate ();
		/* LCOV_EXCL_STOP */
	}
	m->next = curr->next;
	if (prev) {
		prev->next = m;
	} else {
		/* At mainmem_base */
		cobc_mainmem_base = m;
	}
	memcpy (m->memptr, curr->memptr, curr->memlen);
	cobc_free (curr);

	return m->memptr;
}

void
cobc_main_free (void *prevptr)
{
	struct cobc_mem_struct	*curr;
	struct cobc_mem_struct	*prev;

	prev = NULL;
	for (curr = cobc_mainmem_base; curr; curr = curr->next) {
		if (curr->memptr == prevptr) {
			break;
		}
		prev = curr;
	}
	if (unlikely(!curr)) {
		/* LCOV_EXCL_START */
#ifdef	COB_TREE_DEBUG
		cobc_err_msg (_("call to %s with invalid pointer, as it is missing in list"),
			"cobc_main_free");
		cobc_abort_terminate ();
#else
		return;
#endif
		/* LCOV_EXCL_STOP */
	}
	if (prev) {
		prev->next = curr->next;
	} else {
		/* At mainmem_base */
		cobc_mainmem_base = curr->next;
	}
	cobc_free (curr);
}

/* Memory allocate/strdup/reallocate/free for parser */
void *
cobc_parse_malloc (const size_t size)
{
	struct cobc_mem_struct	*m;

	m = calloc ((size_t)1, sizeof(struct cobc_mem_struct) + size);
	if (unlikely(!m)) {
		/* LCOV_EXCL_START */
		cobc_err_msg (_("cannot allocate %d bytes of memory"),
				(int)size);
		cobc_abort_terminate ();
		/* LCOV_EXCL_STOP */
	}
	m->next = cobc_parsemem_base;
	m->memptr = (char *)m + sizeof(struct cobc_mem_struct);
	m->memlen = size;
	cobc_parsemem_base = m;
	return m->memptr;
}

void *
cobc_parse_strdup (const char *dupstr)
{
	void	*p;
	size_t	n;

	if (unlikely(!dupstr)) {
		/* LCOV_EXCL_START */
		cobc_err_msg (_("call to %s with NULL pointer"), "cobc_parse_strdup");
		cobc_abort_terminate ();
		/* LCOV_EXCL_STOP */
	}
	n = strlen (dupstr);
	p = cobc_parse_malloc (n + 1);
	memcpy (p, dupstr, n);
	return p;
}

void *
cobc_parse_realloc (void *prevptr, const size_t size)
{
	struct cobc_mem_struct	*m;
	struct cobc_mem_struct	*curr;
	struct cobc_mem_struct	*prev;

	m = calloc ((size_t)1, sizeof(struct cobc_mem_struct) + size);
	if (unlikely(!m)) {
		/* LCOV_EXCL_START */
		cobc_err_msg (_("cannot allocate %d bytes of memory"),
				(int)size);
		cobc_abort_terminate ();
		/* LCOV_EXCL_STOP */
	}
	m->memptr = (char *)m + sizeof(struct cobc_mem_struct);
	m->memlen = size;

	prev = NULL;
	for (curr = cobc_parsemem_base; curr; curr = curr->next) {
		if (curr->memptr == prevptr) {
			break;
		}
		prev = curr;
	}
	if (unlikely(!curr)) {
		/* LCOV_EXCL_START */
		cobc_err_msg (_("attempt to reallocate non-allocated memory"));
		cobc_abort_terminate ();
		/* LCOV_EXCL_STOP */
	}
	m->next = curr->next;
	if (prev) {
		prev->next = m;
	} else {
		/* At parsemem_base */
		cobc_parsemem_base = m;
	}
	memcpy (m->memptr, curr->memptr, curr->memlen);
	cobc_free (curr);

	return m->memptr;
}

void
cobc_parse_free (void *prevptr)
{
	struct cobc_mem_struct	*curr;
	struct cobc_mem_struct	*prev;

	prev = NULL;
	for (curr = cobc_parsemem_base; curr; curr = curr->next) {
		if (curr->memptr == prevptr) {
			break;
		}
		prev = curr;
	}
	if (unlikely(!curr)) {
		/* LCOV_EXCL_START */
#ifdef	COB_TREE_DEBUG
		cobc_err_msg (_("call to %s with invalid pointer, as it is missing in list"),
			"cobc_parse_free");
		cobc_abort_terminate ();
#else
		return;
#endif
		/* LCOV_EXCL_STOP */
	}
	if (prev) {
		prev->next = curr->next;
	} else {
		/* At parsemem_base */
		cobc_parsemem_base = curr->next;
	}
	cobc_free (curr);
}

/* Memory allocate/strdup/reallocate/free for preprocessor */
void *
cobc_plex_malloc (const size_t size)
{
	struct cobc_mem_struct	*m;

	m = calloc ((size_t)1, sizeof(struct cobc_mem_struct) + size);
	if (unlikely(!m)) {
		/* LCOV_EXCL_START */
		cobc_err_msg (_("cannot allocate %d bytes of memory"),
				(int)size);
		cobc_abort_terminate ();
		/* LCOV_EXCL_STOP */
	}
	m->memptr = (char *)m + sizeof (struct cobc_mem_struct);
	m->next = cobc_plexmem_base;
	cobc_plexmem_base = m;
	return m->memptr;
}

void *
cobc_plex_strdup (const char *dupstr)
{
	void	*p;
	size_t	n;

	if (unlikely(!dupstr)) {
		/* LCOV_EXCL_START */
		cobc_err_msg (_("call to %s with NULL pointer"), "cobc_plex_strdup");
		cobc_abort_terminate ();
		/* LCOV_EXCL_STOP */
	}
	n = strlen (dupstr);
	p = cobc_plex_malloc (n + 1);
	memcpy (p, dupstr, n);
	return p;
}

void *
cobc_check_string (const char *dupstr)
{
	struct strcache	*s;

	if (unlikely(!dupstr)) {
		/* LCOV_EXCL_START */
		cobc_err_msg (_("call to %s with NULL pointer"), "cobc_check_string");
		cobc_abort_terminate ();
		/* LCOV_EXCL_STOP */
	}
	for (s = base_string; s; s = s->next) {
		if (!strcmp (dupstr, (const char *)s->val)) {
			return s->val;
		}
	}
	s = cobc_main_malloc (sizeof(struct strcache));
	s->next = base_string;
	s->val = cobc_main_strdup (dupstr);
	base_string = s;
	return s->val;
}

static struct cb_text_list *
cb_text_list_add (struct cb_text_list *list, const char *text)
{
	struct cb_text_list	*p;

	p = cobc_main_malloc (sizeof (struct cb_text_list));
	p->text = cobc_main_strdup (text);
	if (!list) {
		p->last = p;
		return p;
	}
	list->last->next = p;
	list->last = p;
	return list;
}

static struct cb_text_list *
cb_text_list_chk (struct cb_text_list *list, const char *text)
{
	struct cb_text_list	*p;

	for (p = list; p; p = p->next) {
		if (!strcmp (text, p->text)) {
			return list;
		}
	}
	return cb_text_list_add (list, text);
}

static unsigned int
cobc_set_value (struct cb_define_struct *p, const char *value)
{
	const char	*s;
	size_t		size;
	unsigned int	dot_seen;
	unsigned int	sign_seen;

	if (!value) {
		p->deftype = PLEX_DEF_NONE;
		p->value = NULL;
		p->sign = 0;
		p->int_part = 0;
		p->dec_part = 0;
		return 0;
	}

	/* Quoted value */
	if (*value == '"' || *value == '\'') {
		size = strlen (value) - 1U;
		if (value[0] != value[size]) {
			p->value = NULL;
			p->deftype = PLEX_DEF_NONE;
			return 1;
		}
		p->value = cobc_main_strdup (value);

		p->deftype = PLEX_DEF_LIT;
		p->sign = 0;
		p->int_part = 0;
		p->dec_part = 0;
		return 0;
	}

	/* Non-quoted value - Check if possible numeric */
	dot_seen = 0;
	sign_seen = 0;
	size = 0;
	s = value;
	if (*s == '+' || *s == '-') {
		sign_seen = 1;
		size++;
		s++;
	}
	for (; *s; ++s) {
		if (*s == '.') {
			if (dot_seen) {
				break;
			}
			dot_seen = 1;
			size++;
			continue;
		}
		if (*s > '9' || *s < '0') {
			break;
		}
		size++;
	}

	if (*s || size <= (dot_seen + sign_seen)) {
		/* Not numeric */
#if	0	/* RXWRXW - Lit warn */
		cb_warning (COBC_WARN_FILLER, _("assuming literal for unquoted '%s'"),
				value);
#endif
		size = strlen (value);
		p->value = cobc_main_malloc (size + 4U);
		sprintf (p->value, "'%s'", value);
		p->deftype = PLEX_DEF_LIT;
		p->sign = 0;
		p->int_part = 0;
		p->dec_part = 0;
		return 0;
	}

	p->value = cobc_main_strdup (value);
	p->deftype = PLEX_DEF_NUM;
	p->sign = 0;
	p->int_part = 0;
	p->dec_part = 0;
	return 0;
}

static int
cobc_bcompare (const void *p1, const void *p2)
{
	const void	**tptr;

	tptr = (const void **)p2;
	return strcmp (p1, *tptr);
}

enum name_error_reason {
	INVALID_LENGTH = 1,
	SPACE_UNDERSCORE_FIRST_CHAR,
	GNUCOBOL_PREFIX,
	C_KEYWORD,
	CONTAINS_DIRECTORY_SEPARATOR
};

static void
cobc_error_name (const char *name, const enum cobc_name_type type,
		 const enum name_error_reason reason)
{
	const char	*s;

	switch (reason) {
	case INVALID_LENGTH:
		s = _(" - length is < 1 or > 31");
		break;
	case SPACE_UNDERSCORE_FIRST_CHAR:
		s = _(" - name cannot begin with space or underscore");
		break;
	case GNUCOBOL_PREFIX:
		s = _(" - name cannot begin with 'cob_' or 'COB_'");
		break;
	case C_KEYWORD:
		s = _(" - name duplicates a 'C' keyword");
		break;
	case CONTAINS_DIRECTORY_SEPARATOR:
		s = _(" - name cannot contain a directory separator");
		break;
	default:
		s = "";
		break;
	}

	switch (type) {
	case FILE_BASE_NAME:
		cobc_err_msg (_("invalid file base name '%s'%s"),
			name, s);
		break;
	case ENTRY_NAME:
		cb_error (_("invalid ENTRY '%s'%s"), name, s);
		break;
	case PROGRAM_ID_NAME:
		cb_error (_("invalid PROGRAM-ID '%s'%s"), name, s);
		break;
	default:
		/* LCOV_EXCL_START */
		cobc_err_msg (_("unknown name error '%s'%s"),
			name, s);
		break;
		/* LCOV_EXCL_STOP */
	}
}

size_t
cobc_check_valid_name (const char *name, const enum cobc_name_type prechk)
{
	const char	*p;
	size_t		len;

	/* Check name doesn't contain path separator. */
	for (p = name, len = 0; *p; p++, len++) {
		if (*p == '/' || *p == '\\') {
			cobc_error_name (name, prechk,
					 CONTAINS_DIRECTORY_SEPARATOR);
			return 1;
		}
	}

	/* Check name is of valid length. */
	if (len < 1) {
		cobc_error_name (name, prechk, INVALID_LENGTH);
		return 1;
	}
	if (!cb_relaxed_syntax_checks && len > 31) {
		cobc_error_name (name, prechk, INVALID_LENGTH);
		return 1;
	}

	if (*name == '_' || *name == ' ') {
		cobc_error_name (name, prechk, SPACE_UNDERSCORE_FIRST_CHAR);
		return 1;
	}

	/* Check name does not begin with the libcob prefixes cob_ and COB_. */
	if (prechk && len > 3 &&
	    (!memcmp (name, "cob_", (size_t)4) ||
	     !memcmp (name, "COB_", (size_t)4))) {
		cobc_error_name (name, prechk, GNUCOBOL_PREFIX);
		return 1;
	}

	/* Check name is not a C keyword. */
	if (bsearch (name, cob_csyns, COB_NUM_CSYNS,
		     sizeof (char *), cobc_bcompare)) {
		cobc_error_name (name, prechk, C_KEYWORD);
		return 1;
	}

	return 0;
}

/* Local functions */

static void
cobc_chk_buff_size (const size_t bufflen)
{
	if (bufflen >= cobc_buffer_size) {
		cobc_buffer_size = bufflen + 32;
		cobc_buffer = cobc_main_realloc (cobc_buffer, cobc_buffer_size);
	}
}

/* decipher a positive int from option argument,
   if allow_quote is set and quotes are used set int from char,
   returns -1 on error */
static int
cobc_deciph_optarg (const char *p, const int allow_quote)
{
	const unsigned char	*s;
	size_t			len;
	size_t			i;
	int			n;

	len = strlen (p);
	if (!len) {
		return -1;
	}
	s = (const unsigned char *)p;
	if (allow_quote) {
		if (*s == '"' || *s == '\'') {
			if (len != 3 || *(s + 2) != *s) {
				return -1;
			}
			return (int)(*(s + 1));
		}
		if (*s < '0' || *s > '9') {
			if (len != 1) {
				return -1;
			}
			return (int)*s;
		}
	}
	n = 0;
	for (i = 0; i < len; ++i) {
		if (s[i] < '0' || s[i] > '9') {
			return -1;
		}
		n *= 10;
		n += (s[i] & 0x0F);
	}
	return n;
}

DECLNORET static void COB_A_NORETURN
cobc_err_exit (const char *fmt, ...)
{
	va_list		ap;

	fputs ("cobc: ", stderr);
	fputs (_("error: "), stderr);
	va_start (ap, fmt);
	vfprintf (stderr, fmt, ap);
	va_end (ap);
	putc ('\n', stderr);
	fflush (stderr);
	cobc_free_mem ();
	exit (1);
}

static struct cb_define_struct *
cb_define_list_add (struct cb_define_struct *list, const char *text)
{
	struct cb_define_struct	*p;
	struct cb_define_struct	*l;
	char			*s;
	char			*x;

	x = cobc_strdup (text);
	s = strtok (x, "=");

	/* Check duplicate */
	for (l = list; l; l = l->next) {
		if (!strcasecmp (s, l->name)) {
			cobc_err_msg (_("duplicate DEFINE '%s' - ignored"), s);
			cobc_free (x);
			return list;
		}
	}

	p = cobc_main_malloc (sizeof (struct cb_define_struct));
	p->next = NULL;
	p->name = cobc_check_string (s);
	p->deftype = PLEX_DEF_NONE;
	s = strtok (NULL, "");
	if (cobc_set_value (p, s)) {
		cobc_free (x);
		return NULL;
	}

	cobc_free (x);

	if (!list) {
		p->last = p;
		return p;
	}
	list->last->next = p;
	list->last = p;
	return list;
}

static char *
cobc_stradd_dup (const char *str1, const char *str2)
{
	char	*p;
	size_t	m, n;

	if (unlikely(!str1 || !str2)) {
		/* LCOV_EXCL_START */
		cobc_err_msg (_("call to %s with NULL pointer"), "cobc_stradd_dup");
		cobc_abort_terminate ();
		/* LCOV_EXCL_STOP */
	}
	m = strlen (str1);
	n = strlen (str2);
	p = cobc_main_malloc (m + n + 1);
	memcpy (p, str1, m);
	memcpy (p + m, str2, n);
	return p;
}

static char *
cobc_getenv (const char *env)
{
	char	*p;

	p = getenv (env);
	if (!p || *p == 0 || *p == ' ') {
		return NULL;
	}
	return cobc_main_strdup (p);
}

/*
 * Like cobc_getenv, except value is not allowed to hold any PATHSEP_CHAR
 */
static char *
cobc_getenv_path (const char *env)
{
	char	*p;

	p = getenv (env);
	if (!p || *p == 0 || *p == ' ') {
		return NULL;
	}
	if (strchr (p, PATHSEP_CHAR) != NULL) {
		cobc_err_msg (_("environment variable '%s' is '%s'; should not contain '%c'"), env, p, PATHSEP_CHAR);
		cobc_abort_terminate ();
	}
	return cobc_main_strdup (p);
}

static void
cobc_add_str (char **var, size_t *cursize, const char *s1, const char *s2,
	      const char *s3)
{
	size_t	calcsize;

	if (!s1) {
		return;
	}

	calcsize = strlen (*var);
	calcsize += strlen (s1);
	if (s2) {
		calcsize += strlen (s2);
	}
	if (s3) {
		calcsize += strlen (s3);
	}
	if (calcsize >= 131072) {
		/* Arbitrary limit */
		/* LCOV_EXCL_START */
		cobc_err_exit (_("parameter buffer size exceeded"));
		/* LCOV_EXCL_STOP */
	}
	if (calcsize >= *cursize) {
		while (*cursize <= calcsize) {
			*cursize *= 2;
		}
		*var = cobc_main_realloc (*var, *cursize);
	}
	strcat (*var, s1);
	if (s2) {
		strcat (*var, s2);
	}
	if (s3) {
		strcat (*var, s3);
	}
}

static void
cobc_check_action (const char *name)
{
	if (!name || access (name, F_OK)) {
		return;
	}
	if (!save_temps) {
		(void)unlink (name);
		return;
	}
	if (save_temps_dir) {
		char	temp_buff[COB_MEDIUM_BUFF];

		snprintf (temp_buff, (size_t)COB_MEDIUM_MAX,
			  "%s%s%s", save_temps_dir, SLASH_STR, name);
		temp_buff[COB_MEDIUM_MAX] = 0;
		/* Remove possible target file - ignore return */
		(void)unlink (temp_buff);
		if (rename (name, temp_buff)) {
			/* LCOV_EXCL_START */
			cobc_err_msg (_("warning: could not move temporary file to %s"),
					temp_buff);
			/* LCOV_EXCL_STOP */
		}
	}
}

static void
cobc_clean_up (const int status)
{
	struct filename		*fn;
	struct local_filename	*lf;
	cob_u32_t		i;

	if (cb_src_list_file) {
		fclose (cb_src_list_file);
		cb_src_list_file = NULL;
	}
	if (cb_listing_file) {
		fclose (cb_listing_file);
		cb_listing_file = NULL;
	}
	if (cb_storage_file) {
		fclose (cb_storage_file);
		cb_storage_file = NULL;
	}
	if (ppin) {
		fclose (ppin);
		ppin = NULL;
	}

	if (ppout) {
		fclose (ppout);
		ppout = NULL;
	}
	if (yyin) {
		fclose (yyin);
		yyin = NULL;
	}

	if (yyout) {
		fclose (yyout);
		yyout = NULL;
	}

	for (fn = file_list; fn; fn = fn->next) {
		for (lf = fn->localfile; lf; lf = lf->next) {
			if (unlikely(lf->local_fp)) {
				fclose (lf->local_fp);
				lf->local_fp = NULL;
			}
		}
		if (fn->need_assemble &&
		    (status || cb_compile_level > CB_LEVEL_ASSEMBLE ||
		     (cb_compile_level == CB_LEVEL_ASSEMBLE && save_temps))) {
			cobc_check_action (fn->object);
		}
		if (save_all_src) {
			continue;
		}
		if (fn->need_preprocess &&
		    (status || cb_compile_level > CB_LEVEL_PREPROCESS ||
		     (cb_compile_level == CB_LEVEL_PREPROCESS && save_temps))) {
			cobc_check_action (fn->preprocess);
		}
		if (save_c_src) {
			continue;
		}
		if (fn->need_translate &&
		    (status || cb_compile_level > CB_LEVEL_TRANSLATE ||
		     (cb_compile_level == CB_LEVEL_TRANSLATE && save_temps))) {
			cobc_check_action (fn->translate);
			cobc_check_action (fn->trstorage);
			if (fn->localfile) {
				for (lf = fn->localfile; lf; lf = lf->next) {
					cobc_check_action (lf->local_name);
				}
			} else if (fn->translate) {
				/* If we get syntax errors, we do not
				   know the number of local include files */
				snprintf (cobc_buffer, cobc_buffer_size,
					 "%s.l.h", fn->translate);
				cobc_buffer[cobc_buffer_size] = 0;
				for (i = 0; i < 30U; ++i) {
					if (i) {
						snprintf (cobc_buffer, cobc_buffer_size,
							 "%s.l%u.h", fn->translate, i);
						cobc_buffer[cobc_buffer_size] = 0;
					}
					if (!access (cobc_buffer, F_OK)) {
						unlink (cobc_buffer);
					} else if (i) {
						break;
					}
				}
			}
		}
	}
	cobc_free_mem ();
	file_list = NULL;
}
static void
set_listing_date (void)
{
	char	*time_buff;
	if (!current_compile_time.year) {
		current_compile_time = cob_get_current_date_and_time();
	}

	/* the following code is likely to get replaced by a self-written format */
	current_compile_tm.tm_sec = current_compile_time.second;
	current_compile_tm.tm_min = current_compile_time.minute;
	current_compile_tm.tm_hour = current_compile_time.hour;
	current_compile_tm.tm_mday = current_compile_time.day_of_month;
	current_compile_tm.tm_mon = current_compile_time.month - 1;
	current_compile_tm.tm_year = current_compile_time.year - 1900;
	if (current_compile_time.day_of_week == 7) {
		current_compile_tm.tm_wday = 0;
	} else {
		current_compile_tm.tm_wday = current_compile_time.day_of_week;
	}
	current_compile_tm.tm_yday = current_compile_time.day_of_year;
	current_compile_tm.tm_isdst = current_compile_time.is_daylight_saving_time;
	time_buff = asctime (&current_compile_tm);
	if (!time_buff) {
		/* LCOV_EXCL_START */
		strncpy (cb_listing_date, "DATE BUG, PLEASE REPORT", CB_LISTING_DATE_MAX);
		return;
		/* LCOV_EXCL_STOP */
	}
	*strchr (time_buff, '\n') = '\0';
	strncpy (cb_listing_date, time_buff, CB_LISTING_DATE_MAX);
}


DECLNORET static void COB_A_NORETURN
cobc_terminate (const char *str)
{
	if (cb_src_list_file) {
		set_listing_date ();
		set_standard_title ();
		cb_listing_linecount = cb_lines_per_page;
		strcpy (cb_listing_filename, str);
		print_program_header ();
	}
	cb_perror (0, "cobc: %s: %s", str, cb_get_strerror ());
	if (cb_src_list_file) {
		print_program_trailer ();
	}
	cobc_clean_up (1);
	exit (1);
}

static void
cobc_abort_msg (void)
{
	char *prog_id;
	const char *prog_type;

	if (cb_source_file) {
		if (current_program) {
			if (current_program->prog_type == CB_FUNCTION_TYPE) {
				prog_type = "FUNCTION-ID";
			} else {
				prog_type = "PROGRAM-ID";
			}
			if (current_program->program_id) {
				prog_id = (char *)current_program->program_id;
			} else {
				prog_id = (char *)_("unknown");
			}
		} else {
			prog_type = prog_id = (char *)_("unknown");
		}
		if (!cb_source_line) {
			cobc_err_msg (_("aborting codegen for %s (%s: %s)"),
				cb_source_file, prog_type, prog_id);
		} else {
			cobc_err_msg (_("aborting compile of %s at line %d (%s: %s)"),
				cb_source_file, cb_source_line, prog_type, prog_id);
		}
	} else {
		cobc_err_msg (_("aborting"));
	}
}

DECLNORET static void COB_A_NORETURN
cobc_abort_terminate (void)
{
	/* note we returned 99 for aborts earlier but autotest will
	   "recognize" status 99 as failure (you cannot "expect" the return 99 */
	const int ret_code = 97;

	if (cb_src_list_file) {
		print_program_listing ();
	}
	cobc_abort_msg ();
	cobc_clean_up (ret_code);
	exit (ret_code);
}

static void
cobc_sig_handler (int sig)
{
#if defined (SIGINT) ||defined (SIGQUIT) || defined (SIGTERM) || defined (SIGPIPE)
	int ret = 0;
#endif

	cobc_abort_msg ();
#if defined (SIGINT) ||defined (SIGQUIT) || defined (SIGTERM) || defined (SIGPIPE)
#ifdef SIGINT
	if (sig == SIGINT) ret = 1;
#endif
#ifdef SIGQUIT
	if (sig == SIGQUIT) ret = 1;
#endif
#ifdef SIGTERM
	if (sig == SIGTERM) ret = 1;
#endif
#ifdef SIGPIPE
	if (sig == SIGPIPE) ret = 1;
#endif
	if (!ret) {
		cobc_err_msg (_("Please report this!"));	/* LCOV_EXCL_LINE */
	}
#else
	COB_UNUSED (sig);
#endif
	save_temps = 0;
	cobc_clean_up (1);
}

/* Command line */

static void
cobc_print_version (void)
{
	printf ("cobc (%s) %s.%d\n",
		PACKAGE_NAME, PACKAGE_VERSION, PATCH_LEVEL);
	puts ("Copyright (C) 2017 Free Software Foundation, Inc.");
	puts (_("License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>"));
	puts (_("This is free software; see the source for copying conditions.  There is NO\n"
	        "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE."));
	printf (_("Written by %s\n"), "Keisuke Nishida, Roger While, Ron Norman, Simon Sobisch, Edward Hart");
	printf (_("Built     %s"), cb_cobc_build_stamp);
	putchar ('\n');
	printf (_("Packaged  %s"), COB_TAR_DATE);
	putchar ('\n');
	printf (_("C version %s%s"), OC_C_VERSION_PRF, OC_C_VERSION);
	putchar ('\n');
}

static void
cobc_print_shortversion (void)
{
	printf ("cobc (%s) %s.%d\n",
		PACKAGE_NAME, PACKAGE_VERSION, PATCH_LEVEL);
	printf (_("Built     %s"), cb_cobc_build_stamp);
	putchar ('\t');
	printf (_("Packaged  %s"), COB_TAR_DATE);
	putchar ('\n');
	printf (_("C version %s%s"), OC_C_VERSION_PRF, OC_C_VERSION);
	putchar ('\n');
}

static void
cobc_cmd_print (const char *cmd)
{
	char	*p;
	char	*token;
	size_t	n;
	size_t	toklen;

	if (verbose_output >= 0) {
		fputs (_("executing:"), stderr);
	} else {
		fputs (_("to be executed:"), stderr);
	}
	/* Check if it fits in 80 characters */
	if (strlen (cmd) < 64) {
		fprintf (stderr, "\t%s\n", (char *)cmd);
		fflush (stderr);
		return;
	}
	putc ('\t', stderr);
	p = cobc_strdup (cmd);
	n = 0;
	token = strtok (p, " ");
	for (; token; token = strtok (NULL, " ")) {
		toklen = strlen (token) + 1;
		if ((n + toklen) > 63) {
			fprintf(stderr, "\n\t\t");
			n = 0;
		}
		fprintf (stderr, "%s%s", (n ? " " : ""), token);
		n += toklen;
	}
	cobc_free (p);
	putc ('\n', stderr);
	fflush (stderr);
}

static void
cobc_var_print (const char *msg, const char *val, const unsigned int env)
{
	char	*p;
	char	*token;
	size_t	n;
	int 	lablen;
	size_t	toklen;

	if (!env) {
		printf ("%-*.*s : ", CB_IMSG_SIZE, CB_IMSG_SIZE, msg);
	} else {
		printf ("  %s: ", _("env"));
		lablen = CB_IMSG_SIZE - 2 - (int)strlen (_("env")) - 2;
		printf ("%-*.*s : ", lablen, lablen, msg);
	}
	if (strlen (val) <= CB_IVAL_SIZE) {
		printf("%s\n", val);
		return;
	}
	p = cobc_strdup (val);
	n = 0;
	token = strtok (p, " ");
	for (; token; token = strtok (NULL, " ")) {
		toklen = (int)strlen (token) + 1;
		if ((n + toklen) > CB_IVAL_SIZE) {
			if (n) {
				printf ("\n%*.*s", CB_IMSG_SIZE + 3,
					CB_IMSG_SIZE + 3, " ");
			}
			n = 0;
		}
		printf ("%s%s", (n ? " " : ""), token);
		n += toklen;
	}
	putchar ('\n');
	cobc_free (p);
}

static void
cobc_print_info (void)
{
	char	buff[16];
	char	versbuff[56];
	char	*s;

	cobc_print_version ();
	putchar ('\n');
	puts (_("build information"));
	cobc_var_print (_("build environment"),	COB_BLD_BUILD, 0);
	snprintf (versbuff, 55, "%s\tC version %s%s", COB_BLD_CC, OC_C_VERSION_PRF, OC_C_VERSION);
	cobc_var_print ("CC", versbuff, 0);
	cobc_var_print ("CPPFLAGS",		COB_BLD_CPPFLAGS, 0);
	cobc_var_print ("CFLAGS",		COB_BLD_CFLAGS, 0);
	cobc_var_print ("LD",			COB_BLD_LD, 0);
	cobc_var_print ("LDFLAGS",		COB_BLD_LDFLAGS, 0);
	putchar ('\n');
	puts (_("GnuCOBOL information"));
	cobc_var_print ("COB_CC",		COB_CC, 0);
	if ((s = getenv ("COB_CC")) != NULL) {
		cobc_var_print ("COB_CC",	s, 1);
	}
	cobc_var_print ("COB_CFLAGS",		COB_CFLAGS, 0);
	if ((s = getenv ("COB_CFLAGS")) != NULL) {
		cobc_var_print ("COB_CFLAGS",	s, 1);
	}
	cobc_var_print ("COB_LDFLAGS",		COB_LDFLAGS, 0);
	if ((s = getenv ("COB_LDFLAGS")) != NULL) {
		cobc_var_print ("COB_LDFLAGS",	s, 1);
	}
	cobc_var_print ("COB_LIBS",		COB_LIBS, 0);
	if ((s = getenv ("COB_LIBS")) != NULL) {
		cobc_var_print ("COB_LIBS",	s, 1);
	}
	cobc_var_print ("COB_CONFIG_DIR",	COB_CONFIG_DIR, 0);
	if ((s = getenv ("COB_CONFIG_DIR")) != NULL) {
		cobc_var_print ("COB_CONFIG_DIR",	s, 1);
	}
	cobc_var_print ("COB_COPY_DIR",		COB_COPY_DIR, 0);
	if ((s = getenv ("COB_COPY_DIR")) != NULL) {
		cobc_var_print ("COB_COPY_DIR",	s, 1);
	}
	if ((s = getenv ("COBCPY")) != NULL) {
		cobc_var_print ("COBCPY",	s, 1);
	}
#if defined (_MSC_VER)
	cobc_var_print ("COB_MSG_FORMAT",	"MSC", 0);
#else
	cobc_var_print ("COB_MSG_FORMAT",	"GCC", 0);
#endif
	if ((s = getenv ("COB_MSG_FORMAT")) != NULL) {
		cobc_var_print ("COB_MSG_FORMAT",	s, 1);
	}
	cobc_var_print ("COB_OBJECT_EXT",	COB_OBJECT_EXT, 0);
	cobc_var_print ("COB_MODULE_EXT",	COB_MODULE_EXT, 0);
	cobc_var_print ("COB_EXE_EXT",		COB_EXE_EXT, 0);

#ifdef COB_64_BIT_POINTER
	cobc_var_print ("64bit-mode",	_("yes"), 0);
#else
	cobc_var_print ("64bit-mode",	_("no"), 0);
#endif

#ifdef	COB_LI_IS_LL
	cobc_var_print ("BINARY-C-LONG",	_("8 bytes"), 0);
#else
	cobc_var_print ("BINARY-C-LONG",	_("4 bytes"), 0);
#endif

	cobc_var_print (_("extended screen I/O"),	WITH_CURSES, 0);

	snprintf (buff, sizeof(buff), "%d", WITH_VARSEQ);
	cobc_var_print (_("variable format"),	buff, 0);
	if ((s = getenv ("COB_VARSEQ_FORMAT")) != NULL) {
		cobc_var_print ("COB_VARSEQ_FORMAT", s, 1);
	}

#ifdef	WITH_SEQRA_EXTFH
	cobc_var_print (_("sequential handler"),	"EXTFH", 0);
#else
	cobc_var_print (_("sequential handler"),	_("built-in"), 0);
#endif

#if defined	(WITH_INDEX_EXTFH)
	cobc_var_print (_("ISAM handler"),		"EXTFH", 0);
#elif defined	(WITH_DB)
	cobc_var_print (_("ISAM handler"),		"BDB", 0);
#elif defined	(WITH_CISAM)
	cobc_var_print (_("ISAM handler"),		"C-ISAM", 0);
#elif defined	(WITH_DISAM)
	cobc_var_print (_("ISAM handler"),		"D-ISAM", 0);
#elif defined	(WITH_VBISAM)
	cobc_var_print (_("ISAM handler"),		"VBISAM", 0);
#else
	cobc_var_print (_("ISAM handler"),		_("disabled"), 0);
#endif

#if defined(__MPIR_VERSION)
	cobc_var_print (_("mathematical library"),		"MPIR - GMP", 0);
#else
	cobc_var_print (_("mathematical library"),		"GMP", 0);
#endif
}

static void
cobc_print_warn (const char *name, const char *doc, const int warnopt)
{
	switch (warnopt) {
	case 0:
		printf ("  -W%-19s\t%s\n", name, doc);
		fputs ("\t\t\t", stdout);
		fputs (_("- NOT set with -Wall"), stdout);
		putchar ('\n');
		break;
	case 1:
		printf ("  -W%-19s\t%s\n", name, doc);
		break;
	case 2:
		printf ("  -Wno-%-16s\t%s\n", name, doc);
		fputs ("\t\t\t", stdout);
		fputs (_("- ALWAYS active"), stdout);
		putchar ('\n');
		break;
	default:
		/* LCOV_EXCL_START */
		cobc_err_msg (_("call to '%s' with invalid parameter '%s'"),
			"cobc_print_warn", "warnopt");
		COBC_ABORT ();
		break;
		/* LCOV_EXCL_STOP */
	}
}

static void
cobc_print_flag (const char *name, const char *doc,
		 const int pdok, const char *odoc, const char *def)
{
	char		buff[78];

	if (!pdok || !doc) {
		return;
	}
	if (!odoc) {
		snprintf (buff, sizeof (buff) - 1, "-f%s", name);
	} else if (!strcmp(odoc, "no")) {
		snprintf (buff, sizeof (buff) - 1, "-fno-%s", name);
	} else {
		snprintf (buff, sizeof (buff) - 1, "-f%s=%s", name, odoc);
	}
	printf ("  %-21s\t%s\n", buff, doc);
	if (def) {
		printf ("\t\t\t- %s: %s\n", _("default"), def);
	}
}

static void
cobc_print_usage (char * prog)
{
	puts (_("GnuCOBOL compiler for most COBOL dialects with lots of extensions"));
	putchar ('\n');
	printf (_("Usage: %s [options]... file..."), prog);
	putchar ('\n');
	putchar ('\n');
	puts (_("Options:"));
	puts (_("  -h, -help             display this help and exit"));
	puts (_("  -V, -version          display compiler version and exit"));
	puts (_("  -i, -info             display compiler information (build/environment)\n" \
	        "                        and exit"));
	puts (_("  -v, -verbose          display compiler version and the commands\n" \
	        "                        invoked by the compiler"));
	puts (_("  -vv, -verbose=2       like -v but additional pass verbose option\n" \
	        "                        to assembler/compiler"));
	puts (_("  -vvv, -verbose=3      like -vv but additional pass verbose option\n" \
	        "                        to linker"));
	puts (_("  -q, -brief            reduced displays, commands invoked not shown"));
	puts (_ ("  -###                 like -v but commands not executed"));
	puts (_("  -x                    build an executable program"));
	puts (_("  -m                    build a dynamically loadable module (default)"));
	puts (_("  -j [<args>], -job[=<args>]\trun program after build, passing <args>"));
	puts (_("  -std=<dialect>        warnings/features for a specific dialect\n"
			"                        <dialect> can be one of:\n"
			"                        default, cobol2014, cobol2002, cobol85, xopen,\n"
			"                        ibm-strict, ibm, mvs-strict, mvs,\n"
			"                        mf-strict, mf, bs2000-strict, bs2000,\n"
			"                        acu-strict, acu, rm-strict, rm;\n"
			"                        see configuration files in directory config"));
	puts (_("  -F, -free             use free source format"));
	puts (_("  -fixed                use fixed source format (default)"));
	puts (_("  -O, -O2, -O3, -Os     enable optimization"));
	puts (_("  -g                    enable C compiler debug / stack check / trace"));
	puts (_("  -d, -debug            enable all run-time error checking"));
	puts (_("  -o <file>             place the output into <file>"));
	puts (_("  -b                    combine all input files into a single\n"
			"                        dynamically loadable module"));
	puts (_("  -E                    preprocess only; do not compile or link"));
	puts (_("  -C                    translation only; convert COBOL to C"));
	puts (_("  -S                    compile only; output assembly file"));
	puts (_("  -c                    compile and assemble, but do not link"));
	puts (_("  -T <file>             generate and place a wide program listing into <file>"));
	puts (_("  -t <file>             generate and place a program listing into <file>"));
	puts (_("  --tlines=<lines>      specify lines per page in listing, default = 55"));
	puts (_("  --tsymbols            specify symbols in listing"));
	puts (_("  -P[=<dir or file>]    generate preprocessed program listing (.lst)"));
#ifndef COB_INTERNAL_XREF
	puts (_("  -Xref                 generate cross reference through 'cobxref'\n"
			"                        (V. Coen's 'cobxref' must be in path)"));
#else
	puts (_("  -Xref                 specify cross reference in listing"));
#endif
	puts (_("  -I <directory>        add <directory> to copy/include search path"));
	puts (_("  -L <directory>        add <directory> to library search path"));
	puts (_("  -l <lib>              link the library <lib>"));
	puts (_("  -A <options>          add <options> to the C compile phase"));
	puts (_("  -Q <options>          add <options> to the C link phase"));
	puts (_("  -D <define>           define <define> for COBOL compilation"));
	puts (_("  -K <entry>            generate CALL to <entry> as static"));
	puts (_("  -conf=<file>          user-defined dialect configuration; see -std"));
	puts (_("  -list-reserved        display reserved words"));
	puts (_("  -list-intrinsics      display intrinsic functions"));
	puts (_("  -list-mnemonics       display mnemonic names"));
	puts (_("  -list-system          display system routines"));
	puts (_("  -save-temps[=<dir>]   save intermediate files\n"
			"                        - default: current directory"));
	puts (_("  -ext <extension>      add file extension for resolving COPY"));

	putchar ('\n');

	puts (_("  -W                    enable all warnings"));
	puts (_("  -Wall                 enable most warnings (all except as noted below)"));
	puts (_("  -Wno-<warning>        disable warning enabled by -W or -Wall"));
#define	CB_WARNDEF(var,name,doc)		\
	cobc_print_warn (name, doc, 1);
#define	CB_ONWARNDEF(var,name,doc)		\
	cobc_print_warn (name, doc, 2);
#define	CB_NOWARNDEF(var,name,doc)		\
	cobc_print_warn (name, doc, 0);
#include "warning.def"
#undef	CB_WARNDEF
#undef	CB_ONWARNDEF
#undef	CB_NOWARNDEF
	puts (_("  -Werror               treat all warnings as errors"));
	puts (_("  -Werror=<warning>     treat specified <warning> as error"));

	putchar ('\n');

#define	CB_FLAG(var,pdok,name,doc)		\
	cobc_print_flag (name, doc, pdok, NULL, NULL);
#define	CB_FLAG_ON(var,pdok,name,doc)		\
	cobc_print_flag (name, doc, pdok, "no", NULL);
#define	CB_FLAG_RQ(var,pdok,name,def,opt,doc,vdoc,ddoc)	\
	cobc_print_flag (name, doc, pdok, vdoc, ddoc);
#define	CB_FLAG_NQ(pdok,name,opt,doc,vdoc)		\
	cobc_print_flag (name, doc, pdok, vdoc, NULL);
#include "flag.def"
#undef	CB_FLAG
#undef	CB_FLAG_ON
#undef	CB_FLAG_RQ
#undef	CB_FLAG_NQ

	putchar ('\n');

#define	CB_CONFIG_STRING(var,name,doc)		\
	cobc_print_flag (name, doc, 1, _("<value>"), NULL);
#define	CB_CONFIG_INT(var,name,min,max,odoc,doc)		\
	cobc_print_flag (name, doc, 1, odoc, NULL);
#define	CB_CONFIG_ANY(type,var,name,doc)		\
	cobc_print_flag (name, doc, 1, _("<value>"), NULL);
#define	CB_CONFIG_BOOLEAN(var,name,doc)		\
	cobc_print_flag (name, doc, 1, NULL, NULL);
#define	CB_CONFIG_SUPPORT(var,name,doc)		\
	cobc_print_flag (name, doc, 1, _("<support>"), NULL);
#include "config.def"
#undef	CB_CONFIG_ANY
#undef	CB_CONFIG_INT
#undef	CB_CONFIG_STRING
#undef	CB_CONFIG_BOOLEAN
#undef	CB_CONFIG_SUPPORT
	putchar ('\t');
	puts (_("where <support> is one of the following:"));
	puts ("\t'ok', 'warning', 'archaic', 'obsolete', 'skip', 'ignore', 'error', 'unconformable'");
	putchar ('\t');
	cobc_print_flag ("not-reserved", _("word to be taken out of the reserved words list"), 1, _("<word>"), NULL);
	cobc_print_flag ("reserved", _("word to be added to reserved words list"), 1, _("<word>"), NULL);
	cobc_print_flag ("reserved", _("word to be added to reserved words list as alias"), 1, _("<word>:<alias>"), NULL);

	putchar ('\n');

	putchar ('\n');
	printf (_("Report bugs to: %s\n"
			  "or (preferably) use the issue tracker via the home page."), "bug-gnucobol@gnu.org");
	putchar ('\n');
	puts (_("GnuCOBOL home page: <http://www.gnu.org/software/gnucobol/>"));
	puts (_("General help using GNU software: <http://www.gnu.org/gethelp/>"));
}

static void
cobc_options_error_nonfinal (void)
{
	cobc_err_exit (_("only one of options 'E', 'S', 'C', 'c' may be specified"));
}

static void
cobc_options_error_build (void)
{
	cobc_err_exit (_("only one of options 'm', 'x', 'b' may be specified"));
}

static void
cobc_deciph_funcs (const char *opt)
{
	char	*p;
	char	*q;

	if (!strcasecmp (opt, "ALL")) {
		cb_flag_functions_all = 1;
		return;
	}

	p = cobc_strdup (opt);
	q = strtok (p, ",");
	while (q) {
		if (!lookup_intrinsic (q, 1)) {
			cobc_err_exit (_("'%s' is not an intrinsic function"), q);
		}
		CB_TEXT_LIST_ADD (cb_intrinsic_list, q);
		q = strtok (NULL, ",");
	}
	cobc_free (p);
}

static int
process_command_line (const int argc, char **argv)
{
	struct cb_define_struct	*p;
	size_t			osize;
	int			c;
	int			idx;
	int			n;
	int			exit_option = 0;
	int			list_reserved = 0;
	int			list_registers = 0;
	int			list_intrinsics = 0;
	int			list_system_names = 0;
	int			list_system_routines = 0;
#ifdef _WIN32
	int 			argnum;
#endif
	enum cob_exception_id	i;
	struct stat		st;
	char			ext[COB_MINI_BUFF];
	char			*conf_label;	/* we want a dynamic address for erroc.c, not a static one */
	char			*conf_entry;

	int			conf_ret = 0;
	int			error_all_warnings = 0;

#ifdef _WIN32
	/* Translate command line arguments from WIN to UNIX style */
	argnum = 1;
	while (++argnum <= argc) {
		if (strrchr(argv[argnum - 1], '/') == argv[argnum - 1]) {
			argv[argnum - 1][0] = '-';
		}
	}
#endif

	/* First run of getopt: handle std/conf and all listing options
	   We need to postpone single configuration flags as we need
	   a full configuration to be loaded before */
	cob_optind = 1;
	while ((c = cob_getopt_long_long (argc, argv, short_options,
					  long_options, &idx, 1)) >= 0) {
		switch (c) {

		case '?':
			/* Unknown option or ambiguous */
			cobc_free_mem ();
			exit (1);

		case 'h':
			/* --help */
			cobc_print_usage (argv[0]);
			if (verbose_output) {
				puts ("\n");
				fflush (stdout);
#ifdef _MSC_VER
				process ("cl.exe /help");
				puts ("\n");
				fflush (stdout);
				process ("link.exe /help");
#else
				cobc_buffer_size = strlen (cobc_cc) + 11;
				cobc_buffer = cobc_malloc (cobc_buffer_size);
				snprintf (cobc_buffer, cobc_buffer_size, "%s --help", cobc_cc);
#if defined(__GNUC__) && !defined(__INTEL_COMPILER)
				if (verbose_output > 1) {
					snprintf (cobc_buffer, cobc_buffer_size, "%s -v --help", cobc_cc);
				}
#endif
				cobc_buffer[cobc_buffer_size] = 0;
				process (cobc_buffer);
				cobc_free (cobc_buffer);
				cobc_buffer = NULL;
#endif
			}
			cobc_free_mem ();
			exit (0);

		case 'V':
			/* --version */
			cobc_print_version ();
			cobc_free_mem ();
			exit (0);

		case 'i':
			/* --info */
			cobc_print_info ();
			exit (0);

		/*
			The following list options are postponed until
			until the configuration and exceptions are processed.
		*/
		case '5':
			/* --list-reserved */
			list_reserved = 1;
			exit_option = 1;
			break;

		case '6':
			/* --list-intrinsics */
			list_intrinsics = 1;
			exit_option = 1;
			break;

		case '7':
			/* --list-mnemonics */
			list_system_names = 1;
			exit_option = 1;
			break;

		case '8':
			/* --list-system */
			list_system_routines = 1;
			exit_option = 1;
			break;

		case '9':
			/* --list-registers */
			list_system_names = 1;
			exit_option = 1;
			break;

		case 'q':
			/* --brief : reduced reporting */
			/* resets -verbose and removes the path to cobc in argv[0] */
			verbose_output = 0;
			strcpy (argv[0], "cobc");	/* set for simple compare in test suite
										   and other static output */
			break;

		case '#':
			/* --### : verbose output of commands, but don't execute them */
			if (!verbose_output) {
				cobc_print_shortversion ();
			}
			verbose_output = -1;
			break;

		case 'v':
			/* --verbose : Verbose reporting */
			/* VERY special case as we set different level by mutliple calls */
			/* output version information when running very verbose -vv */
			/* pass verbose switch to invoked commands when running very very verbose -vvv */
			if (cob_optarg) {
				n = cobc_deciph_optarg (cob_optarg, 0);
				if (n == -1) {
					cobc_err_exit (COBC_INV_PAR, "-verbose");
				}
				verbose_output = n;
				if (verbose_output >= 1) {
					cobc_print_shortversion ();
				}
			} else {
				verbose_output++;
				if (verbose_output == 1) {
					cobc_print_shortversion ();
				}
			}
			break;

		case '$':
			/* -std=<xx> : Specify dialect */
			snprintf (ext, (size_t)COB_MINI_MAX, "%s.conf", cob_optarg);
			if (cb_load_std (ext) != 0) {
				cobc_err_exit (_("invalid parameter -std=%s"), cob_optarg);
			}
			break;

		case '&':
			/* -conf=<xx> : Specify dialect configuration file */
			if (strlen (cob_optarg) > COB_SMALL_MAX) {
				cobc_err_exit (COBC_INV_PAR, "-conf");
			}
			conf_ret |= cb_load_conf (cob_optarg, 0);
			break;

		default:
			/* as we postpone most options simply skip everything other here */
			break;
		}
	}

	/* Load default configuration file if necessary */
	if (cb_config_name == NULL) {
		if (verbose_output) {
			fputs (_("loading standard configuration file 'default.conf'"), stderr);
			fputc ('\n', stderr);
		}
		conf_ret |= cb_load_std ("default.conf");
	}

	/* Exit for configuration errors resulting from -std/-conf/default.conf */
	if (conf_ret != 0) {
		cobc_free_mem ();
		exit (1);
	}

	cob_optind = 1;
	while ((c = cob_getopt_long_long (argc, argv, short_options,
					  long_options, &idx, 1)) >= 0) {
		switch (c) {
		case 0:
			/* Defined flag */
			break;

		case 'h':
			/* --help */
		case 'V':
			/* --version */
		case 'i':
			/* --info */
		case '5':
			/* --list-reserved */
		case '6':
			/* --list-intrinsics */
		case '7':
			/* --list-mnemonics */
		case '8':
			/* --list-system */
		case '9':
			/* --list-registers */
			/* These options were all processed in the first getopt-run */
			break;

		case 'E':
			/* -E : Preprocess */
			if (wants_nonfinal) {
				cobc_options_error_nonfinal ();
			}
			wants_nonfinal = 1;
			cb_compile_level = CB_LEVEL_PREPROCESS;
			break;

		case 'C':
			/* -C : Generate C code */
			if (wants_nonfinal) {
				cobc_options_error_nonfinal ();
			}
			save_c_src = 1;
			wants_nonfinal = 1;
			cb_compile_level = CB_LEVEL_TRANSLATE;
			break;

		case 'S':
			/* -S : Generate assembler code */
			if (wants_nonfinal) {
				cobc_options_error_nonfinal ();
			}
			wants_nonfinal = 1;
			cb_compile_level = CB_LEVEL_COMPILE;
			break;

		case 'c':
			/* -c : Generate C object code */
			if (wants_nonfinal) {
				cobc_options_error_nonfinal ();
			}
			wants_nonfinal = 1;
			cb_compile_level = CB_LEVEL_ASSEMBLE;
			break;

		case 'b':
			/* -b : Generate combined library module */
			if (cobc_flag_main || cobc_flag_module) {
				cobc_options_error_build ();
			}
			cobc_flag_library = 1;
			no_physical_cancel = 1;
			cb_flag_implicit_init = 1;
			break;

		case 'm':
			/* -m : Generate loadable module (default) */
			if (cobc_flag_main || cobc_flag_library) {
				cobc_options_error_build ();
			}
			cobc_flag_module = 1;
			break;

		case 'x':
			/* -x : Generate executable */
			if (cobc_flag_module || cobc_flag_library) {
				cobc_options_error_build ();
			}
			cobc_flag_main = 1;
			cb_flag_main = 1;
			no_physical_cancel = 1;
			break;

		case 'j':
			/* -j : Run job; compile, link and go, either by ./ or cobcrun */
			/* allows optional arguments, passed to program */
			cobc_flag_run = 1;
			if (cobc_run_args) {
				cobc_free (cobc_run_args);
			}
			if (cob_optarg) {
				cobc_run_args = cobc_strdup (cob_optarg);
			}
			break;

		case 'F':
			/* -F : short option for -free */
			cb_source_format = CB_FORMAT_FREE;
			break;

		case 'q':
			/* --brief : reduced reporting */
		case '#':
			/* --### : verbose output of commands, but don't execute them */
		case 'v':
			/* --verbose : Verbose reporting */
			/* these options were processed in the first getopt-run */
			break;

		case 'o':
			/* -o : Output file */
			osize = strlen (cob_optarg);
			if (osize > COB_SMALL_MAX) {
				cobc_err_exit (_("invalid output file name"));
			}
			if (output_name) {
				cobc_main_free (output_name);
				cobc_main_free (output_name_buff);
			}
			output_name = cobc_main_strdup (cob_optarg);
			/* Allocate buffer plus extension reserve */
			output_name_buff = cobc_main_malloc (osize + 32U);
			break;

		case 'O':
			/* -O : Optimize */
			cob_optimize = 1;
			COBC_ADD_STR (cobc_cflags, CB_COPT_1, NULL, NULL);
			break;

		case '2':
			/* -O2 : Optimize */
			cob_optimize = 1;
			strip_output = 1;
			COBC_ADD_STR (cobc_cflags, CB_COPT_2, NULL, NULL);
			break;

		case '3':
			/* -O3 : Optimize */
			cob_optimize = 1;
			strip_output = 1;
			COBC_ADD_STR (cobc_cflags, CB_COPT_3, NULL, NULL);
			break;

		case 's':
			/* -Os : Optimize */
			cob_optimize = 1;
			strip_output = 1;
			COBC_ADD_STR (cobc_cflags, CB_COPT_S, NULL, NULL);
			break;

		case 'g':
			/* -g : Generate C debug code */
			save_all_src = 1;
			gflag_set = 1;
			cb_flag_stack_check = 1;
			cb_flag_source_location = 1;
#ifndef	_MSC_VER
			COBC_ADD_STR (cobc_cflags, " -g", NULL, NULL);
#endif
			break;

		case '$':
			/* -std=<xx> : Specify dialect */
		case '&':
			/* -conf=<xx> : Specify dialect configuration file */
			/* These options were all processed in the first getopt-run */
			break;

		case '%':
			/* -f<tag>=<value> : Override configuration entry */
			/* hint: -f[no-]<tag> sets the var directly */
			conf_label = cobc_main_malloc (COB_MINI_BUFF);
			conf_entry = cobc_malloc (COB_MINI_BUFF - 2);
			snprintf (conf_label, COB_MINI_MAX, "-%s=%s",
				long_options[idx].name, cob_optarg);
			strncpy(conf_entry, conf_label + 2, COB_MINI_MAX - 2);
			conf_ret |= cb_config_entry (conf_entry, conf_label, 0);
			cobc_free (conf_entry);
			break;

		case 'd':
			/* -debug : Turn on all runtime checks */
			cb_flag_source_location = 1;
			cb_flag_trace = 1;
			cb_flag_stack_check = 1;
			cobc_wants_debug = 1;
			break;

		case '_':
			/* --save-temps : Save intermediary files */
			save_temps = 1;
			if (cob_optarg) {
				if (stat (cob_optarg, &st) != 0 ||
				    !(S_ISDIR (st.st_mode))) {
					cobc_err_msg (_("warning: '%s' is not a directory, defaulting to current directory"),
						cob_optarg);
				} else {
					if (save_temps_dir) {
						cobc_free (save_temps_dir);
					}
					save_temps_dir = cobc_strdup (cob_optarg);
				}
			}
			break;

		case 'T':
			/* -T : Generate wide listing */
			cb_listing_wide = 1;
			/* fall through */

		case 't':
			/* -t : Generate listing */
			if (cb_listing_outputfile) {
				cobc_main_free (cb_listing_outputfile);
			}
			/* FIXME: add option to place each source in a single listing 
			          by specifying a directory (similar to -P) */
			cb_listing_outputfile = cobc_main_strdup (cob_optarg);
			break;

		case '*':
			/* --tlines=nn : Lines per page */
			cb_lines_per_page = atoi (cob_optarg);
			break;

		case 'P':
			/* -P : Generate preproc listing */
			if (cob_optarg) {
				if (cobc_list_dir) {
					cobc_free (cobc_list_dir);
					cobc_list_dir = NULL;
				}
				if (cobc_list_file) {
					cobc_free (cobc_list_file);
					cobc_list_file = NULL;
				}
				if (!stat (cob_optarg, &st) && S_ISDIR (st.st_mode)) {
					cobc_list_dir = cobc_strdup (cob_optarg);
				} else {
					cobc_list_file = cobc_strdup (cob_optarg);
				}
			}
			if (!cobc_gen_listing) {
				cobc_gen_listing = 1;
			}
			break;

		case 'X':
#ifndef COB_INTERNAL_XREF
			/* -Xref : Generate listing through 'cobxref' */
			cobc_gen_listing = 2;
			/* temporary: check if we run the testsuite and skip
			   the run if we don't have the internal xref */
			if (getenv ("COB_IS_RUNNING_IN_TESTMODE")) {
				cobc_free_mem ();
				exit (77);
			}
#else
			/* -Xref : Generate internal listing */
			cb_listing_xref = 1;
#endif
			break;

		case 'D':
			/* -D xx(=yy) : Define variables */
			if (strlen (cob_optarg) > 64U) {
				cobc_err_exit (COBC_INV_PAR, "-D");
			}
			if (!strcasecmp (cob_optarg, "ebug")) {
				cobc_err_msg (_("warning: assuming '%s' is a DEFINE - did you intend to use -debug?"),
						cob_optarg);
			}
			p = cb_define_list_add (cb_define_list, cob_optarg);
			if (!p) {
				cobc_err_exit (COBC_INV_PAR, "-D");
			}
			cb_define_list = p;
			break;

		case 'I':
			/* -I <xx> : Include/copy directory */
			if (strlen (cob_optarg) > COB_SMALL_MAX) {
				cobc_err_exit (COBC_INV_PAR, "-I");
			}
			if (stat (cob_optarg, &st) != 0 ||
			    !(S_ISDIR (st.st_mode))) {
				break;
			}
#ifdef	_MSC_VER
			COBC_ADD_STR (cobc_include, " /I \"", cob_optarg, "\"");
#elif	defined (__WATCOMC__)
			COBC_ADD_STR (cobc_include, " -i\"", cob_optarg, "\"");
#else
			COBC_ADD_STR (cobc_include, " -I\"", cob_optarg, "\"");
#endif
			CB_TEXT_LIST_ADD (cb_include_list, cob_optarg);
			break;

		case 'L':
			/* -L <xx> : Directory for library search */
			if (strlen (cob_optarg) > COB_SMALL_MAX) {
				cobc_err_exit (COBC_INV_PAR, "-L");
			}
			if (stat (cob_optarg, &st) != 0 ||
			    !(S_ISDIR (st.st_mode))) {
				break;
			}
#ifdef	_MSC_VER
			COBC_ADD_STR (cobc_lib_paths, " /LIBPATH:\"", cob_optarg, "\"");
#else
			COBC_ADD_STR (cobc_lib_paths, " -L\"", cob_optarg, "\"");
#endif
			break;

		case 'l':
			/* -l <xx> : Add library to link phase */
			if (strlen (cob_optarg) > COB_SMALL_MAX) {
				cobc_err_exit (COBC_INV_PAR, "-l");
			}
#ifdef	_MSC_VER
			COBC_ADD_STR (cobc_libs, " \"", cob_optarg, ".lib\"");
#else
			COBC_ADD_STR (cobc_libs, " -l\"", cob_optarg, "\"");
#endif
			break;

		case 'e':
			/* -e <xx> : Add an extension suffix */
			if (strlen (cob_optarg) > 15U) {
				cobc_err_exit (COBC_INV_PAR, "--ext");
			}
			snprintf (ext, (size_t)COB_MINI_MAX, ".%s", cob_optarg);
			CB_TEXT_LIST_ADD (cb_extension_list, ext);
			break;

		case 'K':
			/* -K <xx> : Define literal CALL to xx as static */
			if (strlen (cob_optarg) > 32U) {
				cobc_err_exit (COBC_INV_PAR, "-K");
			}
			CB_TEXT_LIST_ADD (cb_static_call_list, cob_optarg);
			break;

		case 'k':
			/* -k <xx> : Check for exit after CALL to xx  */
			/* This is to cater for legacy German DIN standard */
			/* Check after CALL if an exit program required */
			/* Not in --help as subject to change and highly specific */
			if (strlen (cob_optarg) > 32U) {
				cobc_err_exit (COBC_INV_PAR, "-k");
			}
			CB_TEXT_LIST_ADD (cb_early_exit_list, cob_optarg);
			break;

		case 1:
			/* -fstack-size=<xx> : Specify stack (perform) size */
			n = cobc_deciph_optarg (cob_optarg, 0);
			if (n < 16 || n > 512) {
				cobc_err_exit (COBC_INV_PAR, "-fstack-size");
			}
			cb_stack_size = n;
			break;

		case 2:
			/* -fif-cutoff=<xx> : Specify IF cutoff level */
			n = cobc_deciph_optarg (cob_optarg, 0);
			if (n < 1 || n > 512) {
				cobc_err_exit (COBC_INV_PAR, "-fif-cutoff");
			}
			cb_if_cutoff = n;
			break;

		case 3:
			/* -fsign=<ASCII/EBCDIC> : Specify display sign */
			if (!strcasecmp (cob_optarg, "EBCDIC")) {
				cb_ebcdic_sign = 1;
			} else if (!strcasecmp (cob_optarg, "ASCII")) {
				cb_ebcdic_sign = 0;
			} else {
				cobc_err_exit (COBC_INV_PAR, "-fsign");
			}
			break;

		case 4:
			/* -ffold-copy=<UPPER/LOWER> : COPY fold case */
			if (!strcasecmp (cob_optarg, "UPPER")) {
				cb_fold_copy = COB_FOLD_UPPER;
			} else if (!strcasecmp (cob_optarg, "LOWER")) {
				cb_fold_copy = COB_FOLD_LOWER;
			} else {
				cobc_err_exit (COBC_INV_PAR, "-ffold-copy");
			}
			break;

		case 5:
			/* -ffold-call=<UPPER/LOWER> : CALL/PROG-ID fold case */
			if (!strcasecmp (cob_optarg, "UPPER")) {
				cb_fold_call = COB_FOLD_UPPER;
			} else if (!strcasecmp (cob_optarg, "LOWER")) {
				cb_fold_call = COB_FOLD_LOWER;
			} else {
				cobc_err_exit (COBC_INV_PAR, "-ffold-call");
			}
			break;

		case 6:
			/* -fdefaultbyte=<xx> : Default initialization byte */
			n = cobc_deciph_optarg (cob_optarg, 1);
			if (n < 0 || n > 255) {
				cobc_err_exit (COBC_INV_PAR, "-fdefaultbyte");
			}
			cb_default_byte = n;
			break;

		case 7:
			/* -fmax-errors=<xx> : maximum errors until abort */
			n = cobc_deciph_optarg (cob_optarg, 0);
			if (n < 0 || n > 99999) {
				cobc_err_exit (COBC_INV_PAR, "-max-errors");
			}
			cb_max_errors = n;
			break;

		case 10:
			/* -fintrinsics=<xx> : Intrinsic name or ALL */
			cobc_deciph_funcs (cob_optarg);
			break;

		case 'A':
			/* -A <xx> : Add options to C compile phase */
			COBC_ADD_STR (cobc_cflags, " ", cob_optarg, NULL);
			aflag_set = 1;
			break;

		case 'Q':
			/* -Q <xx> : Add options to C link phase */
			COBC_ADD_STR (cobc_ldflags, " ", cob_optarg, NULL);
			break;

		case 'w':
			/* -w : Turn off all warnings (disables -W/-Wall if passed later) */
			warningopt = 0;
#define	CB_WARNDEF(var,name,doc)	var = 0;
#define	CB_ONWARNDEF(var,name,doc)	var = 0;
#define	CB_NOWARNDEF(var,name,doc)	var = 0;
#include "warning.def"
#undef	CB_WARNDEF
#undef	CB_ONWARNDEF
#undef	CB_NOWARNDEF
			break;

		case 'W':
			/* -Wall : Turn on most warnings */
			warningopt = 1;
#define	CB_WARNDEF(var,name,doc)	var = 1;
#define	CB_ONWARNDEF(var,name,doc)
#define	CB_NOWARNDEF(var,name,doc)
#include "warning.def"
#undef	CB_WARNDEF
#undef	CB_ONWARNDEF
#undef	CB_NOWARNDEF
			break;

		case 'Y':
			/* -Werror[=warning] : Treat all/single warnings as errors */
			if (cob_optarg) {
#define CB_CHECK_WARNING(var,name)  \
				if (strcmp(cob_optarg,name) == 0) {	\
					var = COBC_WARN_AS_ERROR;		\
				} else
#define	CB_WARNDEF(var,name,doc)	CB_CHECK_WARNING(var,name)
#define	CB_ONWARNDEF(var,name,doc)	CB_CHECK_WARNING(var,name)
#define	CB_NOWARNDEF(var,name,doc)	CB_CHECK_WARNING(var,name)
#include "warning.def"
#undef	CB_CHECK_WARNING
#undef	CB_WARNDEF
#undef	CB_ONWARNDEF
#undef	CB_NOWARNDEF
				/* note: ends block from last CB_CHECK_WARNING */
				/* else */ if (verbose_output) {
					cobc_err_msg (_("unknown warning option '%s'"),
						cob_optarg);
				}
			} else {
				error_all_warnings = 1;
			}
			break;

		case 'Z':
			/* -W : Turn on every warning */
			warningopt = 1;
#define	CB_WARNDEF(var,name,doc)	var = 1;
#define	CB_ONWARNDEF(var,name,doc)
#define	CB_NOWARNDEF(var,name,doc)	var = 1;
#include "warning.def"
#undef	CB_WARNDEF
#undef	CB_ONWARNDEF
#undef	CB_NOWARNDEF
			break;

		default:
			cobc_err_exit (_("invalid option detected"));
		}
	}
	
	/* Load reserved words from fixed word-list if specified */
	if (cb_reserved_words != NULL) {
		cb_load_words();
	}

	/* Exit for configuration errors resulting from -f<conf-tag>[=<value>] */
	if (conf_ret != 0) {
		cobc_free_mem ();
		exit (1);
	}

	/* Set relaxed syntax configuration options if requested */
	/* part 1: relaxed syntax compiler configuration option */
	if (cb_relaxed_syntax_checks) {
		if (cb_reference_out_of_declaratives > CB_WARNING) {
			cb_reference_out_of_declaratives = CB_WARNING;
		}
		/* fixme - the warning was only raised if not relaxed */
		cb_warn_ignored_initial_val = 0;
	}
#if 0 /* deactivated as -frelaxed-syntax-checks and other compiler configurations
		 are available at command line - maybe re-add with another name */
	/* 2: relaxed syntax group option from command line */
	if (cb_flag_relaxed_syntax_group) {
		cb_relaxed_syntax_checks = 1;
		cb_larger_redefines_ok = 1;
		cb_relax_level_hierarchy = 1;
		cb_top_level_occurs_clause = CB_OK;
	}
#endif

	if (list_reserved) {
		/* includes register listing */
		cb_list_reserved ();
	} else if (list_registers) {
		cb_list_registers ();
	}
	if (list_intrinsics) {
		cb_list_intrinsics ();
	}
	if (list_system_names) {
		cb_list_system_names ();
	}
	if (list_system_routines) {
		cb_list_system_routines ();
	}

	/* Exit if list options were specified */
	if (exit_option) {
		cobc_free_mem ();
		exit (0);
	}

	/* Set active warnings to errors, if requested */
	if (error_all_warnings) {
#define CB_CHECK_WARNING(var)  \
		if (var == COBC_WARN_ENABLED) {	\
				var = COBC_WARN_AS_ERROR;		\
		}
#define	CB_WARNDEF(var,name,doc)	CB_CHECK_WARNING(var)
#define	CB_ONWARNDEF(var,name,doc)	CB_CHECK_WARNING(var)
#define	CB_NOWARNDEF(var,name,doc)	CB_CHECK_WARNING(var)
#include "warning.def"
#undef	CB_CHECK_WARNING
#undef	CB_WARNDEF
#undef	CB_ONWARNDEF
#undef	CB_NOWARNDEF
	}
	if (fatal_errors_flag) {
		cb_max_errors = 0;
	}

	/* Set implied options */
	if (cb_title_statement <= CB_OBSOLETE
	&&  cb_listing_statements > CB_OBSOLETE) {
		cb_listing_statements = cb_title_statement;
	}

	/* debug: Turn on all exception conditions */
	if (cobc_wants_debug) {
		for (i = (enum cob_exception_id)1; i < COB_EC_MAX; ++i) {
			CB_EXCEPTION_ENABLE (i) = 1;
		}
		if (verbose_output > 1) {
			fputs (_("all runtime checks are enabled"), stderr);
			fputc ('\n', stderr);
		}
	}

	/* If C debug, do not strip output */
	if (gflag_set) {
		strip_output = 0;
	}

	if (cb_flag_traceall) {
		cb_flag_trace = 1;
		cb_flag_source_location = 1;
	}

	return cob_optind;
}

/* Reverse the list of programs */
static struct cb_program *
program_list_reverse (struct cb_program *p)
{
	struct cb_program	*next;
	struct cb_program	*last;

	last = NULL;
	for (; p; p = next) {
		next = p->next_program;
		p->next_program = last;
		last = p;
	}
	return last;
}

static void
process_env_copy_path (const char *p)
{
	char		*value;
	char		*token;
	struct stat	st;

	if (p == NULL || !*p || *p == ' ') {
		return;
	}

	/* Clone value for destructive strtok */
	value = cobc_strdup (p);

	/* Tokenize for path sep. */
	token = strtok (value, PATHSEP_STR);
	while (token) {
		if (!stat (token, &st) && (S_ISDIR (st.st_mode))) {
			CB_TEXT_LIST_CHK (cb_include_list, token);
		}
		token = strtok (NULL, PATHSEP_STR);
	}

	cobc_free (value);
	return;
}

#if	defined (_MSC_VER) || defined(__OS400__) || defined(__WATCOMC__) || defined(__BORLANDC__)
static void
file_stripext (char *buff)
{
	char	*endp;

	endp = buff + strlen (buff) - 1U;
	while (endp > buff) {
		if (*endp == '/' || *endp == '\\') {
			break;
		}
		if (*endp == '.') {
			*endp = 0;
		}
		--endp;
	}
}
#endif

static char *
file_basename (const char *filename)
{
	const char	*p;
	const char	*startp;
	const char	*endp;
	size_t		len;

	if (!filename) {
		/* LCOV_EXCL_START */
		cobc_err_msg (_("call to '%s' with invalid parameter '%s'"),
			"file_basename", "filename");
		COBC_ABORT ();
		/* LCOV_EXCL_STOP */
	}

	/* Remove directory name */
	startp = NULL;
	for (p = filename; *p; p++) {
		if (*p == '/' || *p == '\\') {
			startp = p;
		}
	}
	if (startp) {
		startp++;
	} else {
		startp = filename;
	}

	/* Remove extension */
	endp = strrchr (filename, '.');
	if (endp > startp) {
		len = endp - startp;
	} else {
		len = strlen (startp);
	}

	if (len >= basename_len) {
		basename_len = len + 16;
		basename_buffer = cobc_main_realloc (basename_buffer, basename_len);
	}
	/* Copy base name (possiby done before -> memmove) */
	memmove (basename_buffer, startp, len);
	basename_buffer[len] = 0;
	return basename_buffer;
}

static const char *
file_extension (const char *filename)
{
	const char *p;

	p = strrchr (filename, '.');
	if (p) {
		return p + 1;
	}
	return "";
}

static struct filename *
process_filename (const char *filename)
{
	const char	*extension;
	struct filename	*fn;
	struct filename	*ffn;
	char		*fbasename;
	char		*listptr;
	size_t		fsize;
	int		file_is_stdin;

	if (strcmp(filename, COB_DASH) == 0) {
		if (cobc_seen_stdin == 0) {
			cobc_seen_stdin = 1;
			file_is_stdin = 1;
			filename = COB_DASH_NAME;
		} else {
			cobc_err_msg (_("only one stdin input allowed"));
			return NULL;
		}
	} else {
		file_is_stdin = 0;
	}

	fsize = strlen (filename);
	if (fsize > COB_NORMAL_MAX) {
		/* LCOV_EXCL_START */
		cobc_err_msg (_("invalid file name parameter (length > %d)"), COB_NORMAL_MAX);
		return NULL;
		/* LCOV_EXCL_STOP */
	}

#ifdef	__OS400__
	if (strchr (filename, '.') != NULL) {
#endif

	if (!file_is_stdin && access (filename, R_OK) != 0) {
		cobc_terminate (filename);
	}

#ifdef	__OS400__
	}
#endif

	fbasename = file_basename (filename);
	extension = file_extension (filename);
	if (strcmp(extension, "lib") && strcmp(extension, "a") &&
		strcmp(extension, COB_OBJECT_EXT)) {
		if (cobc_check_valid_name (fbasename, 0)) {
			return NULL;
		}
	}
	fn = cobc_main_malloc (sizeof (struct filename));
	fn->need_preprocess = 1;
	fn->need_translate = 1;
	fn->need_assemble = 1;
	fn->file_is_stdin = file_is_stdin;
	fn->next = NULL;

	if (!file_list) {
		file_list = fn;
	} else {
		for (ffn = file_list; ffn->next; ffn = ffn->next)
			;
		ffn->next = fn;
	}

	fn->demangle_source = cb_encode_program_id (fbasename);

	/* Check input file type */
	if (strcmp (extension, "i") == 0) {
		/* Already preprocessed */
		fn->need_preprocess = 0;
	} else if (strcmp (extension, "c") == 0
#if	defined(_WIN32)
			|| strcmp(extension, "asm") == 0
#endif
			|| strcmp (extension, "s") == 0) {
		/* Already compiled */
		fn->need_preprocess = 0;
		fn->need_translate = 0;
	} else if (
#if	defined(__OS400__)
			extension[0] == 0
#else
			strcmp (extension, COB_OBJECT_EXT) == 0
#if	defined(_WIN32)
			|| strcmp(extension, "lib") == 0
#endif
#if	!defined(_WIN32) || defined(__MINGW32__) || defined(__MINGW64__)
			|| strcmp(extension, "a") == 0
			|| strcmp(extension, "so") == 0
			|| strcmp(extension, "dylib") == 0
			|| strcmp(extension, "sl") == 0
#endif
#endif
	) {
		/* Already assembled */
		fn->need_preprocess = 0;
		fn->need_translate = 0;
		fn->need_assemble = 0;
	}

	/* Set source filename */
	fn->source = cobc_main_strdup (filename);

	/* Set preprocess filename */
	if (!fn->need_preprocess) {
		fn->preprocess = cobc_main_strdup (fn->source);
	} else if (output_name && cb_compile_level == CB_LEVEL_PREPROCESS) {
		fn->preprocess = cobc_main_strdup (output_name);
	} else if (save_all_src || save_temps ||
		   cb_compile_level == CB_LEVEL_PREPROCESS) {
		fn->preprocess = cobc_stradd_dup (fbasename, ".i");
	} else {
		fn->preprocess = cobc_main_malloc(COB_FILE_MAX);
		cob_temp_name ((char *)fn->preprocess, ".cob");
	}

	/* Set translate filename */
	if (!fn->need_translate) {
		fn->translate = cobc_main_strdup (fn->source);
	} else if (output_name && cb_compile_level == CB_LEVEL_TRANSLATE) {
		fn->translate = cobc_main_strdup (output_name);
	} else if (save_all_src || save_temps || save_c_src ||
		   cb_compile_level == CB_LEVEL_TRANSLATE) {
		fn->translate = cobc_stradd_dup (fbasename, ".c");
	} else {
		fn->translate = cobc_main_malloc(COB_FILE_MAX);
		cob_temp_name ((char *)fn->translate, ".c");
	}
	fn->translate_len = strlen (fn->translate);

	/* Set storage filename */
	if (fn->need_translate) {
		fn->trstorage = cobc_stradd_dup (fn->translate, ".h");
	}

	/* Set object filename */
	if (!fn->need_assemble) {
		fn->object = cobc_main_strdup (fn->source);
	} else if (output_name && cb_compile_level == CB_LEVEL_ASSEMBLE) {
		fn->object = cobc_main_strdup (output_name);
	} else if (save_temps || cb_compile_level == CB_LEVEL_ASSEMBLE) {
		fn->object = cobc_stradd_dup(fbasename, "." COB_OBJECT_EXT);
	} else {
		fn->object = cobc_main_malloc(COB_FILE_MAX);
		cob_temp_name ((char *)fn->object, "." COB_OBJECT_EXT);
	}
	fn->object_len = strlen (fn->object);
	cobc_objects_len += fn->object_len + 8U;

	/* Set listing filename */
	if (cobc_gen_listing == 1) {
		if (cobc_list_file) {
			fn->listing_file = cobc_list_file;
		} else if (cobc_list_dir) {
			fsize = strlen (cobc_list_dir) + strlen (fbasename) + 8U;
			listptr = cobc_main_malloc (fsize);
			snprintf (listptr, fsize, "%s%c%s.lst",
				  cobc_list_dir, SLASH_CHAR, fbasename);
			fn->listing_file = listptr;
		} else {
			fn->listing_file = cobc_stradd_dup (fbasename, ".lst");
		}
#ifndef COB_INTERNAL_XREF
	} else if (cobc_gen_listing == 2) {
	/* LCOV_EXCL_START */
		fn->listing_file = cobc_stradd_dup (fbasename, ".xrf");
	/* LCOV_EXCL_STOP */
#endif
	}

	cob_incr_temp_iteration();
	return fn;
}

#ifdef _MSC_VER
/*
 * search_pattern can contain one or more search strings separated by '|'
 * search_patterns must have a final '|'
 */
static int
line_contains (char* line_start, char* line_end, char* search_patterns) {
	int pattern_end, pattern_start, pattern_length, full_length;
	char* line_pos;

	if (search_patterns == NULL) return 0;

	pattern_start = 0;
	full_length = (int)strlen (search_patterns) - 1;
	for (pattern_end = 0; pattern_end < (int)strlen (search_patterns); pattern_end++) {
		if (search_patterns[pattern_end] == PATTERN_DELIM) {
			pattern_length = pattern_end - pattern_start;
			for (line_pos = line_start; line_pos + pattern_length <= line_end; line_pos++) {
				/* Find matching substring */
				if (memcmp (line_pos, search_patterns + pattern_start, pattern_length) == 0) {
					/* Exit if all patterns found, skip to next pattern otherwise */
					if (pattern_start + pattern_length == full_length) {
					return 1;
					} else {
						break;
				}
			}
			}
			pattern_start = pattern_end + 1;
		}
	}

	return 0;
}
#endif

/** -j run job after build */
static int
process_run (const char *name) {
	int ret, status;

	if (cb_compile_level < CB_LEVEL_MODULE) {
		fputs (_("nothing for -j to run"), stderr);
		fflush (stderr);
		return 0;
	}

	if (cb_compile_level == CB_LEVEL_MODULE ||
	    cb_compile_level == CB_LEVEL_LIBRARY) {
		if (cobc_run_args) {
			snprintf (cobc_buffer, cobc_buffer_size, "cobcrun%s %s %s",
				COB_EXE_EXT, file_basename(name), cobc_run_args);
		} else {
			snprintf (cobc_buffer, cobc_buffer_size, "cobcrun%s %s",
				COB_EXE_EXT, file_basename(name));
		}
	} else {  /* executable */
		if (cobc_run_args) {
			snprintf (cobc_buffer, cobc_buffer_size, ".%c%s%s %s",
				SLASH_CHAR, name, COB_EXE_EXT, cobc_run_args);
		} else {
			snprintf (cobc_buffer, cobc_buffer_size, ".%c%s%s",
				SLASH_CHAR, name, COB_EXE_EXT);
		}
	}
	cobc_buffer[cobc_buffer_size] = 0;
	if (verbose_output) {
		cobc_cmd_print (cobc_buffer);
	}
	if (verbose_output < 0) {
		return 0;
	}
	status = system (cobc_buffer);
#ifdef WEXITSTATUS
	if (WIFEXITED(status)) {
		ret = WEXITSTATUS(status);
	} else {
		ret = status;
	}
#else
	ret = status;
#endif
	if (verbose_output) {
		fputs (_("return status:"), stderr);
		fprintf (stderr, "\t%d\n", ret);
		fflush (stderr);
	}
	return ret;
}

#ifdef	__OS400__
static int
process (char *cmd)
{
	char	*buffptr;
	char	*name = NULL;
	char	*objname = NULL;
	char	*cobjname = NULL;
	char	*token;
	char	*incl[100];
	char	*defs[100];
	char	*objs[100];
	char	*libs[100];
	char	*optc[100];
	char	*optl[100];
	int	nincl = 0;
	int	ndefs = 0;
	int	nobjs = 0;
	int	nlibs = 0;
	int	noptc = 0;
	int	noptl = 0;
	int	dbg = 0;
	int	comp_only = 0;
	int	shared = 0;
	int	optimize = 0;
	int	i;
	int	len;
	int	ret;

	if (verbose_output) {
		cobc_cmd_print (cmd);
	}
	if (gflag_set) {
		dbg = 1;
	}
	token = strtok (cmd, " ");
	if (token != NULL) {
		/* Skip C compiler */
		token = strtok (NULL, " ");
	}
	for (; token; token = strtok (NULL, " ")) {
		if (*token != '-') {
			len = strlen (token);
			if (*token == '"') {
				len -= 2;
				++token;
				token[len] = 0;
			}
			if (token[len-2] == '.' && token[len - 1] == 'c') {
				/* C source */
				name = token;
				continue;
			}
			/* Assuming module */
			objs[nobjs++] = token;
			continue;
		}
		++token;
		switch (*token) {
		case 'c':
			comp_only = 1;
			break;
		case 'I':
			++token;
			if (*token == 0) {
				token = strtok (NULL, " ");
			}
			if (*token == '"') {
				++token;
				token[strlen (token) - 1] = 0;
			}
			incl[nincl++] = token;
			break;
		case 'D':
			++token;
			if (*token == 0) {
				token = strtok (NULL, " ");
			}
			if (*token == '"') {
				++token;
				token[strlen (token) - 1] = 0;
			}
			defs[ndefs++] = token;
			break;
		case 'A':
			++token;
			optc[noptc++] = token;
			break;
		case 'Q':
			++token;
			optl[noptl++] = token;
			break;
		case 'o':
			++token;
			if (*token == 0) {
				token = strtok (NULL, " ");
			}
			if (*token == '"') {
				++token;
				token[strlen (token) - 1] = 0;
			}
			objname = token;
			break;
		case 'l':
			++token;
			if (*token == 0) {
				token = strtok (NULL, " ");
			}
			libs[nlibs++] = token;
			break;
		case 'G':
			shared = 1;
			break;
		case 'g':
			dbg = 1;
			break;
		case 'O':
			optimize = 1;
			break;
		default:
			cobc_err_msg (_("unknown option ignored:\t%s"),
				 token - 1);
		}
	}

	buffptr = cobc_malloc (COB_LARGE_BUFF);
	if (name != NULL) {
		/* Requires compilation */
		if (objname == NULL) {
			cobjname = file_basename (name);
		} else {
			cobjname = objname;
		}
		sprintf(buffptr, "CRTCMOD MODULE(%s) SRCSTMF('%s') ",
			cobjname, name);
		if (nincl > 0) {
			strcat (buffptr, "INCDIR(");
			for (i = 0; i < nincl; ++i) {
				if (i != 0) {
					strcat (buffptr, " ");
				}
				strcat (buffptr, "'");
				strcat (buffptr, incl[i]);
				strcat (buffptr, "' ");
			}
			strcat (buffptr, ") ");
		}
		if (ndefs > 0) {
			strcat (buffptr, "DEFINE(");
			for (i = 0; i < ndefs; ++i) {
				if (i != 0) {
					strcat (buffptr, " ");
				}
				strcat (buffptr, "'");
				strcat (buffptr, defs[i]);
				strcat (buffptr, "' ");
			}
			strcat (buffptr, ") ");
		}
		strcat (buffptr, "SYSIFCOPT(*IFSIO)");
		for (i = 0; i < noptc; ++i) {
			strcat (buffptr, " ");
			strcat (buffptr, optc[i]);
		}
		if (optimize) {
			strcat (buffptr, " OPTIMIZE(40)");
		}
		if (dbg) {
			strcat (buffptr, " DBGVIEW(*ALL)");
		}
		if (cobc_gen_listing) {
			strcat (buffptr, " OUTPUT(*PRINT)");
		}
		if (verbose_output) {
			cobc_cmd_print (buffptr);
		}
		if (verbose_output >= 0) {
			ret = system (buffptr);
			if (verbose_output) {
				fputs (_("return status:"), stderr);
				fprintf (stderr, "\t%d\n", ret);
				fflush (stderr);
			}
		} else {
			ret = 0;
		}
		if (comp_only || ret != 0) {
			cobc_free (buffptr);
			return ret;
		}
	}
	if (objname == NULL) {
		if (name != NULL) {
			objname = cobjname;
		} else if (nobjs > 0) {
			objname = objs[0];
		} else {
			objname = (char *)"AOUT";
		}
	}
	if (shared) {
		sprintf(buffptr, "CRTSRVPGM SRVPGM(%s) MODULE(", objname);
	} else {
		sprintf(buffptr, "CRTPGM PGM(%s) MODULE(", objname);
	}
	if (name != NULL) {
		strcat (buffptr, cobjname);
	}
	for (i = 0; i < nobjs; ++i) {
		if (i != 0 || name != NULL) {
			strcat (buffptr, " ");
		}
		strcat (buffptr, objs[i]);
	}
	strcat (buffptr, ")");
	if (nlibs > 0) {
		strcat (buffptr, " BNDSRVPGM(");
		for (i = 0; i < nlibs; ++i) {
			if (i != 0) {
				strcat (buffptr, " ");
			}
			strcat (buffptr, libs[i]);
		}
		strcat (buffptr, ")");
	}
	for (i = 0; i < noptl; ++i) {
		strcat (buffptr, " ");
		strcat (buffptr, optl[i]);
	}
	if (shared) {
		strcat (buffptr, " EXPORT(*ALL)");
	}
	if (verbose_output) {
		cobc_cmd_print (buffptr);
	}
	if (verbose_output >= 0) {
		ret = system (buffptr);
		if (verbose_output) {
			fputs (_("return status:"), stderr);
			fprintf (stderr, "\t%d\n", ret);
			fflush (stderr);
		}
	} else {
		ret = 0;
	}
	cobc_free (buffptr);

	if ((ret == 0) && cobc_flag_run) {
		ret = process_run (name);
	}
	return ret;
}

#elif defined(_MSC_VER)
static int
process (const char *cmd)
{
	int ret;

	if (verbose_output) {
		cobc_cmd_print (cmd);
	}
	if (verbose_output < 0) {
		return 0;
	}
	ret = system (cmd);
	if (verbose_output) {
		fputs (_("return status:"), stderr);
		fprintf (stderr, "\t%d\n", ret);
		fflush (stderr);
	}
	return !!ret;
}

static int
process_filtered (const char *cmd, struct filename *fn)
{
	FILE* pipe;
	char* read_buffer;
	char *line_start, *line_end;
	char* search_pattern, *search_pattern2 = NULL;
	char* output_name_temp;
	int i;
	int ret;

	if (verbose_output) {
		cobc_cmd_print (cmd);
	}
	if (verbose_output < 0) {
		return 0;
	}

	/* Open pipe to catch output of cl.exe */
	pipe = _popen(cmd, "r");

	if (!pipe) {
		return 1; /* checkme */
	}

	/* building search_patterns */
	if (output_name) {
		output_name_temp = file_basename(output_name);
	} else {
		/* demangle_source is encoded and cannot be used
		   -> set to file.something and strip at point
		*/
		output_name_temp = cobc_strdup (fn->source);
		file_stripext(output_name_temp);
	}

	/* check for last path seperator as we only need the file name */
	for (i = fn->translate_len; i > 0; i--) {
		if (fn->translate[i - 1] == '\\' || fn->translate[i - 1] == '/') break;
	}

	search_pattern = (char*)cobc_malloc((fn->translate_len - i + 2) + 1);
	sprintf(search_pattern, "%s\n%c", fn->translate + i, PATTERN_DELIM);
	if (cb_compile_level > CB_LEVEL_ASSEMBLE) {
		search_pattern2 = (char*)cobc_malloc (2 * (strlen (output_name_temp) + 5) + 1);
		sprintf (search_pattern2, "%s.lib%c%s.exp%c", file_basename(output_name_temp), PATTERN_DELIM,
			file_basename(output_name_temp), PATTERN_DELIM);
	}

	/* prepare buffer and read from pipe */
	read_buffer = (char*) cobc_malloc(COB_FILE_BUFF);
	line_start = fgets(read_buffer, COB_FILE_BUFF - 1, pipe);

	while (line_start != NULL) {
		/* read one line from buffer, returning line end position */
		line_end = line_start + strlen (line_start);

		/* if non of the patterns was found, print line */
		if (line_start == line_end
			|| (!line_contains(line_start, line_end, search_pattern)
				&& !line_contains(line_start, line_end, search_pattern2)))
		{
			fprintf(stdout, "%*s", (int)(line_end - line_start + 2), line_start);
		}
		line_start = fgets(read_buffer, COB_FILE_BUFF - 1, pipe);
	}
	fflush (stdout);

	cobc_free (read_buffer);
	cobc_free (search_pattern);
	cobc_free (search_pattern2);

	if (!output_name) cobc_free (output_name_temp);

	/* close pipe and get return code of cl.exe */
	ret = !!_pclose (pipe);

	if (verbose_output) {
		fputs (_("return status:"), stderr);
		fprintf (stderr, "\t%d\n", ret);
		fflush (stderr);
	}
	return ret;
}

#else
static int
process (const char *cmd)
{
	char	*p;
	char	*buffptr;
	size_t	clen;
	int	ret;

	if (likely(strchr (cmd, '$') == NULL)) {
		buffptr = (char *)cmd;
	} else {
		clen = strlen (cmd) + 64U;
		clen = clen + 6U;
		buffptr = (char *)cobc_malloc (clen);
		p = buffptr;
		/* Quote '$' */
		for (; *cmd; ++cmd) {
			if (*cmd == '$') {
				p += sprintf (p, "\\$");
			} else {
				*p++ = *cmd;
			}
		}
		*p = 0;
	}

	if (verbose_output) {
		cobc_cmd_print (buffptr);
	}

	ret = system (buffptr);

	if (unlikely(buffptr != cmd)) {
		cobc_free (buffptr);
	}

#ifdef	WIFSIGNALED
	if (WIFSIGNALED(ret)) {
#ifdef	SIGINT
		if (WTERMSIG(ret) == SIGINT) {
			cob_raise (SIGINT);
		}
#endif
#ifdef	SIGQUIT
		if (WTERMSIG(ret) == SIGQUIT) {
			cob_raise (SIGQUIT);
		}
#endif
	}
#endif
	if (verbose_output) {
		fputs (_("return status:"), stderr);
		fprintf (stderr, "\t%d\n", ret);
		fflush (stderr);
	}
	return !!ret;
}
#endif

static COB_INLINE COB_A_INLINE void
force_new_page_for_next_line (void)
{
	cb_listing_linecount = cb_lines_per_page;
}

/* Preprocess source */

static int
preprocess (struct filename *fn)
{
	const char		*sourcename;
	int			save_source_format;
	int			save_fold_copy;
	int			save_fold_call;
#ifndef COB_INTERNAL_XREF
#ifdef	_WIN32
	const char *envname = "%PATH%";
#else
	const char *envname = "$PATH";
#endif
	int			ret;
#endif

	if (cb_unix_lf) {
		ppout = fopen(fn->preprocess, "wb");
	} else {
		ppout = fopen(fn->preprocess, "w");
	}
	if (!ppout) {
		cobc_terminate (fn->preprocess);
	}

	if (fn->file_is_stdin) {
		sourcename = COB_DASH;
	} else {
		sourcename = fn->source;
	}
	if (ppopen (sourcename, NULL) != 0) {
		fclose (ppout);
		ppout = NULL;
		if (fn->preprocess) {
			(void)unlink (fn->preprocess);
		}
		cobc_free_mem ();
		exit (1);
	}

	if (verbose_output) {
		fputs (_("preprocessing:"), stderr);
		fprintf (stderr, "\t%s -> %s\n",
			 sourcename, fn->preprocess);
		fflush (stderr);
	}

	if (cobc_gen_listing && !cobc_list_file) {
		if (cb_unix_lf) {
			cb_listing_file = fopen (fn->listing_file, "wb");
		} else {
			cb_listing_file = fopen (fn->listing_file, "w");
		}
		if (!cb_listing_file) {
			cobc_terminate (fn->listing_file);
		}
	}

	/* Reset pplex/ppparse variables */
	plex_clear_vars ();
	ppparse_clear_vars (cb_define_list);

	/* Save default flags in case program directives change them */
	save_source_format = cb_source_format;
	save_fold_copy = cb_fold_copy;
	save_fold_call = cb_fold_call;

	/* Preprocess */
	ppparse ();

	/* Restore default flags */
	cb_source_format = save_source_format;
	cb_fold_copy = save_fold_copy;
	cb_fold_call = save_fold_call;

	if (ppin) {
		fclose (ppin);
		ppin = NULL;
	}

	if (ppout) {
		if (unlikely(fclose(ppout) != 0)) {
			cobc_terminate(fn->preprocess);
		}
		ppout = NULL;
	}

	/* Release flex buffers - After file close */
	plex_call_destroy ();

	if (cobc_gen_listing && !cobc_list_file) {
		if (unlikely(fclose (cb_listing_file) != 0)) {
			cobc_terminate(fn->listing_file);
		}
#ifndef COB_INTERNAL_XREF
		/* external cross-reference with cobxref */
		if (cobc_gen_listing == 2) {
			/* LCOV_EXCL_START */
			if (cb_src_list_file) {
				fclose (cb_src_list_file);
			}

			snprintf (cobc_buffer, cobc_buffer_size,
				 "cobxref %s -R", fn->listing_file);
			cobc_buffer[cobc_buffer_size] = 0;
			if (verbose_output) {
				cobc_cmd_print (cobc_buffer);
			}
			ret = system (cobc_buffer);
			if (verbose_output) {
				fputs (_("return status:"), stderr);
				fprintf (stderr, "\t%d\n", ret);
				fflush (stderr);
			}
			if (ret) {
				fputs (_("'cobxref' execution unsuccessful"),
					stderr);
				putc ('\n', stderr);
				fprintf (stderr, _("check that 'cobxref' is in %s"), envname);
				putc ('\n', stderr);
				fputs (_("no listing produced"),
					stderr);
				putc ('\n', stderr);
				fflush (stderr);
			}
			if (cb_listing_outputfile) {
				if (cb_unix_lf) {
					cb_src_list_file = fopen (cb_listing_outputfile, "ab");
				} else {
					cb_src_list_file = fopen (cb_listing_outputfile, "a");
				}
				if (!cb_src_list_file) {
					cobc_terminate (cb_listing_outputfile);
				}
				cb_listing_eject = 1;
				force_new_page_for_next_line ();
			}
			unlink (fn->listing_file);
			/* LCOV_EXCL_STOP */
		}
#endif
		cb_listing_file = NULL;
	}

	if (verbose_output) {
		fputs (_("return status:"), stderr);
		fprintf (stderr, "\t%d\n", errorcount);
		fflush (stderr);
	}
	return !!errorcount;
}

/* Routines to generate program listing */


static void
set_listing_header_code (void)
{
	strcpy (cb_listing_header, "LINE    ");
	if (cb_listing_file_struct->source_format != CB_FORMAT_FREE) {
		strcat (cb_listing_header,
			"PG/LN  A...B..............................."
			".............................");
		if (cb_listing_wide) {
			if (cb_listing_file_struct->source_format == CB_FORMAT_FIXED
			    && cb_text_column == 72) {
				strcat (cb_listing_header, "SEQUENCE");
			} else {
				strcat (cb_listing_header,
					"........................................");
			}
		}
	} else {
		if (cb_listing_wide) {
			strcat (cb_listing_header,
				"................................");
		}
		strcat (cb_listing_header,
			".....................SOURCE..................."
			"..........................");
		if (cb_listing_wide) {
			strcat (cb_listing_header, "........");
		}
	}
}

static void
set_listing_header_symbols (void)
{
	strcpy (cb_listing_header,
		"SIZE  TYPE           LVL  NAME                           PICTURE");
}

#ifdef COB_INTERNAL_XREF
/* listing header for internal xref */
static void
set_listing_header_xref (const enum xref_type type)
{
	if (type == XREF_FUNCTION) {
		strcpy (cb_listing_header, "FUNCTION");
	} else if (type == XREF_LABEL) {
		strcpy (cb_listing_header, "LABEL   ");
	} else {
		strcpy (cb_listing_header, "NAME    ");
	}
	if (type == XREF_FUNCTION) {
		strcat (cb_listing_header,
			"                       TYPE                   ");
	} else {
		strcat (cb_listing_header,
			"                       DEFINED                ");
	}
	if (cb_listing_wide) {
		strcat (cb_listing_header, "                    ");
	}
	strcat (cb_listing_header, "REFERENCES");
}
#endif

/* listing header empty */
static void
set_listing_header_none (void)
{
	cb_listing_header[0] = 0;
}

/* standard title for listing
   (TODO: option to set by directive and/or command line option) */
static void
set_standard_title (void)
{
	char		version[20];
	sprintf (version, "%s.%d", PACKAGE_VERSION, PATCH_LEVEL);
	snprintf (cb_listing_title, 80, "%s %s",
		PACKAGE_NAME,
		version);
}

/* print header */
static void
print_program_header (void)
{
	const char	*format_str;

	cb_listing_linecount = 1;

	/* header for print listing (with page breaks) */
	if (cb_lines_per_page != 0) {
		if (cb_listing_eject) {
			fputs ("\f", cb_src_list_file);
		} else {
			cb_listing_eject = 1;
		}
		if (cb_listing_wide) {
			format_str = "%-23.23s %-61.61s %s  Page %04d\n";
		} else {
			format_str = "%-23.23s %-20.20s %s  Page %04d\n";
		}
		fprintf (cb_src_list_file,
			 format_str,
			 cb_listing_title,
			 cb_listing_filename,
			 cb_listing_date,
			 ++cb_listing_page);

	/* header for listing without page breaks: --tlines=0 */
	} else {

		if (cb_listing_page == 0) {
			cb_listing_page = 1;
			if (cb_listing_wide) {
				format_str = "%-28.28s %-66.66s %s\n";
			} else {
				format_str = "%-28.28s %-26.26s %s\n";
			}
			fprintf (cb_src_list_file,
				 format_str,
				 cb_listing_title,
				 cb_listing_filename,
				 cb_listing_date);
		} else {
		}
	}
	fputc ('\n', cb_src_list_file);

	/* print second header if set */
	if (cb_listing_header[0]) {
		print_program_data (cb_listing_header);
		print_program_data ("");
	}
}

static void
print_program_data (const char *data)
{
	/* no check for header if page break is disabled and not forced */
	if (cb_lines_per_page != 0 || cb_listing_linecount == 0) {
		/* increase listing line number and print header if necessary */
		if (++cb_listing_linecount >= cb_lines_per_page) {
			/* empty string - don't print anything */
			if (!data[0]) {
				return;
			}
			print_program_header ();
		}
	}

	/* print data + newline */
	fprintf (cb_src_list_file, "%s\n", data);
}

static char *
check_filler_name (char *name)
{
	if (strlen (name) >= 6 && memcmp (name, "FILLER", 6) == 0) {
		name = (char *)"FILLER";
	}
	return name;
}

static int
set_picture (struct cb_field *field, char *picture, int picture_len)
{
	int usage_len;
	char picture_usage[CB_LIST_PICSIZE];

	memset (picture, 0, CB_LIST_PICSIZE);

	/* Check non-picture information first */
	switch (field->usage) {
	case CB_USAGE_INDEX:
	case CB_USAGE_LENGTH:
	case CB_USAGE_OBJECT:
	case CB_USAGE_POINTER:
	case CB_USAGE_PROGRAM_POINTER:
	case CB_USAGE_LONG_DOUBLE:
	case CB_USAGE_FP_BIN32:
	case CB_USAGE_FP_BIN64:
	case CB_USAGE_FP_BIN128:
	case CB_USAGE_FP_DEC64:
	case CB_USAGE_FP_DEC128:
	case CB_USAGE_SIGNED_CHAR:
	case CB_USAGE_SIGNED_SHORT:
	case CB_USAGE_SIGNED_INT:
	case CB_USAGE_SIGNED_LONG:
	case CB_USAGE_UNSIGNED_CHAR:
	case CB_USAGE_UNSIGNED_SHORT:
	case CB_USAGE_UNSIGNED_INT:
	case CB_USAGE_UNSIGNED_LONG:
		return 0;
	default:
		break;
	}

	/* check for invalid picture next */
	if (field->pic && !field->pic->orig) {
		strcpy (picture, "INVALID");
		return 1;
	}

	/* Get usage for this picture */
	strcpy (picture_usage, cb_get_usage_string (field->usage));
	usage_len = strlen (picture_usage);

	/* set picture for the rest */
	if (field->usage == CB_USAGE_BINARY
		   || field->usage == CB_USAGE_FLOAT
		   || field->usage == CB_USAGE_DOUBLE
		   || field->usage == CB_USAGE_PACKED
		   || field->usage == CB_USAGE_COMP_5
		   || field->usage == CB_USAGE_COMP_6
		   || field->usage == CB_USAGE_COMP_X) {
		if (field->pic) {
			strncpy (picture, field->pic->orig, picture_len - 1 - usage_len);
			picture[CB_LIST_PICSIZE - 1] = 0;
			strcat (picture, " ");
		}
	} else if (field->flag_any_numeric) {
		strncpy (picture, "9 ANY NUMERIC", 14);
		return 1;
	} else if (field->flag_any_length) {
		strncpy (picture, "X ANY LENGTH", 13);
		return 1;
	} else {
		if (!field->pic) {
			return 0;
		}
		strncpy (picture, field->pic->orig, picture_len - 1);
		return 1;
	}

	strcat (picture, picture_usage);
	return 1;
}

static void
set_category_from_usage (int usage, char *type)
{
	switch (usage) {
	case CB_USAGE_INDEX:
		strcpy (type, "INDEX");
		break;
	case CB_USAGE_POINTER:
	case CB_USAGE_PROGRAM_POINTER:
		strcpy (type, "POINTER");
		break;
	case CB_USAGE_DISPLAY:
		strcpy (type, "ALPHANUMERIC");
		break;
	case CB_USAGE_NATIONAL:
		strcpy (type, "NATIONAL");
		break;
	case CB_USAGE_BIT:
		strcpy (type, "BOOLEAN");
		break;
	default:
		strcpy (type, "NUMERIC");
		break;
	}
}

static void
set_category (int category, int usage, char *type)
{
	switch (category) {
	case CB_CATEGORY_UNKNOWN:
		set_category_from_usage (usage, type);
		break;
	case CB_CATEGORY_ALPHABETIC:
		strcpy (type, "ALPHABETIC");
		break;
	case CB_CATEGORY_ALPHANUMERIC:
	case CB_CATEGORY_ALPHANUMERIC_EDITED:
		strcpy (type, "ALPHANUMERIC");
		break;
	case CB_CATEGORY_BOOLEAN:
		strcpy (type, "BOOLEAN");
		break;
	case CB_CATEGORY_INDEX:
		strcpy (type, "INDEX");
		break;
	case CB_CATEGORY_NATIONAL:
	case CB_CATEGORY_NATIONAL_EDITED:
		strcpy (type, "NATIONAL");
		break;
	case CB_CATEGORY_NUMERIC:
	case CB_CATEGORY_NUMERIC_EDITED:
		strcpy (type, "NUMERIC");
		break;
	case CB_CATEGORY_OBJECT_REFERENCE:
		strcpy (type, "OBJECT REF");
		break;
	case CB_CATEGORY_DATA_POINTER:
	case CB_CATEGORY_PROGRAM_POINTER:
		strcpy (type, "POINTER");
		break;
	default:
		strcpy (type, "UNKNOWN");	/* LCOV_EXCL_LINE */
	}
}

static void
terminate_str_at_first_trailing_space (char * const str)
{
	int	i;

	for (i = strlen (str) - 1; i && isspace ((unsigned char)str[i]); i--) {
		str[i] = '\0';
	}
}

static void
print_88_values (struct cb_field *field)
{
	struct cb_field *f;
	char lcl_name[LCL_NAME_LEN] = { '\0' };

	for (f = field->validation; f; f = f->sister) {
		strncpy (lcl_name, (char *)f->name, LCL_NAME_MAX);
		snprintf (print_data, CB_PRINT_LEN,
			"      %-14.14s %02d   %s",
			"CONDITIONAL", f->level, lcl_name);
		print_program_data (print_data);
	}
}

/* print all fields including sister and child elements */
static void
print_fields (struct cb_field *top, int *found)
{
	int	first = 1;
	int	get_cat;
	int	got_picture;
	int	old_level = 0;
	int	picture_len = cb_listing_wide ? 64 : 24;
	char	type[20];
	char	picture[CB_LIST_PICSIZE];
	char	lcl_name[LCL_NAME_LEN];

	for (; top; top = top->sister) {
		if (!top->level) {
			continue;
		}
		if (*found == 0) {
			*found = 1;
			/* MAYBE use a second header line and a forced page break instead */
			snprintf (print_data, CB_PRINT_LEN,
				"      %s", cobc_enum_explain_storage(top->storage));
			print_program_data (print_data);
			print_program_data ("");
		}

		strncpy (lcl_name, check_filler_name ((char *)top->name),
			 LCL_NAME_MAX);
		get_cat = 1;
		got_picture = 1;

		if (top->children) {
			strcpy (type, "GROUP");
			get_cat = 0;
			got_picture = 0;
			if (top->level == 01 && !first) {
				print_program_data ("");
			}
		} else if (top->level == 01) {
			if (!first) {
				print_program_data ("");
			}
		} else if (top->level == 77 && !first
			   && old_level != 77) {
			print_program_data ("");
		}

		if (get_cat) {
			set_category (top->common.category, top->usage, type);
			if (top->flag_any_length) {
				picture[0] = 0;
			}
			got_picture = set_picture (top, picture, picture_len);
		}

		if (top->flag_any_length || top->flag_unbounded) {
			pd_off = sprintf (print_data, "????? ");
		} else if (top->flag_occurs && !got_picture) {
			pd_off = sprintf (print_data, "%05d ", top->size * top->occurs_max);
		} else {
			pd_off = sprintf (print_data, "%05d ", top->size);
		}
		pd_off += sprintf (print_data + pd_off, "%-14.14s %02d   ", type, top->level);
		if (top->flag_occurs && got_picture) {
			pd_off += sprintf (print_data + pd_off, "%-30.30s %s, ", lcl_name, picture);
		} else if (got_picture) {
			pd_off += sprintf (print_data + pd_off, "%-30.30s %s", lcl_name, picture);
		} else if (top->flag_occurs) {
			pd_off += sprintf (print_data + pd_off, "%-30.30s ", lcl_name);
		} else { /* Trailing spaces break testsuite AT_DATA */
			pd_off += sprintf (print_data + pd_off, "%s", lcl_name);
		}
		if (top->flag_occurs) {
			if (top->depending && top->flag_unbounded) {
				pd_off += sprintf (print_data + pd_off, "OCCURS %d TO UNBOUNDED", top->occurs_min);
			} else if (top->depending) {
				pd_off += sprintf (print_data + pd_off, "OCCURS %d TO %d", top->occurs_min, top->occurs_max);
			} else {
				pd_off += sprintf (print_data + pd_off, "OCCURS %d", top->occurs_max);
			}
		}
		if (top->redefines && !top->file) {
			pd_off += sprintf (print_data + pd_off, ", REDEFINES %s", top->redefines->name);
		}
		print_program_data (print_data);

		first = 0;
		old_level = top->level;
		print_88_values (top);

		if (top->children) {
			print_fields (top->children, found);
		}
	}
}

static void
print_files_and_their_records (cb_tree file_list_p)
{
	cb_tree	l;
	int dummy = 1;

	for (l = file_list_p; l; l = CB_CHAIN (l)) {
		snprintf (print_data, CB_PRINT_LEN,
			"%05d %-14.14s      %s",
			 CB_FILE (CB_VALUE (l))->record_max,
			 "FILE",
			 CB_FILE (CB_VALUE (l))->name);
		print_program_data (print_data);
		if (CB_FILE (CB_VALUE (l))->record) {
			print_fields (CB_FILE (CB_VALUE (l))->record, &dummy);
			print_program_data ("");
		}
	}
}

static int
print_fields_in_section (struct cb_field *first_field_in_section)
{
	int found = 0;
	if (first_field_in_section != NULL) {
		print_fields (first_field_in_section, &found);
		if (found) {
			print_program_data ("");
		}
	}
	return found;
}

/* create xref_elem with line number for existing xref entry */
void
cobc_xref_link (struct cb_xref *list, const int line, const int receiving)
{
#ifdef COB_INTERNAL_XREF
	struct cb_xref_elem *elem;

	for (elem = list->head; elem; elem = elem->next) {
		if (elem->line == line) {
			if (receiving) {
				elem->receive = 1;
			}
			return;
		}
	}

	elem = cobc_parse_malloc (sizeof (struct cb_xref_elem));
	elem->line = line;
	elem->receive = receiving;

	/* add xref_elem to head/tail
	   remark: if head == NULL, then tail may contain reference to child's
	   head marking it as "referenced by child" - we don't want to preserve
	   this information but overwrite it with the actual reference */
	if (list->head == NULL) {
		list->head = elem;
	} else if (list->tail != NULL) {
		list->tail->next = elem;
	}
	list->tail = elem;
#else
	COB_UNUSED (list);
	COB_UNUSED (line);
	COB_UNUSED (receiving);
#endif
}

/* set "referenced by child" (including lvl 88 validation) for field's parents */
void
cobc_xref_link_parent (const struct cb_field *field)
{
#ifdef COB_INTERNAL_XREF
	struct cb_field *f;
	const struct cb_xref *f_xref = &field->xref;
	struct cb_xref *p_xref;

	for (f = field->parent; f; f = f->parent) {
		/* parent has own reference already -> exit */
		p_xref = &f->xref;
		if (p_xref->head != NULL) {
			return;
		}
		p_xref->tail = f_xref->tail;
	}
#else
	COB_UNUSED (field);
#endif
}

/* add a "receiving" entry for a given field reference */
void
cobc_xref_set_receiving (const cb_tree target_ext)
{
#ifdef COB_INTERNAL_XREF
	cb_tree	target = target_ext;
	struct cb_field		*target_fld;
	int				xref_line;

	if (CB_CAST_P (target)) {
		target = CB_CAST (target)->val;
	}
	if (CB_REF_OR_FIELD_P (target)) {
		target_fld = CB_FIELD_PTR (target);
	} else {
		return;
	}
	if (CB_REFERENCE_P (target)) {
		xref_line = CB_REFERENCE (target)->common.source_line;
	} else if (current_statement) {
		xref_line = current_statement->common.source_line;
	} else {
		xref_line = cb_source_line;
	}
	cobc_xref_link (&target_fld->xref, xref_line, 1);
#else
	COB_UNUSED (target_ext);
#endif
}

void
cobc_xref_call (const char *name, const int line, const int is_ident, const int is_sys)
{
#ifdef COB_INTERNAL_XREF
	struct cb_call_elem	*elem;

	for (elem = current_program->call_xref.head; elem; elem = elem->next) {
		if (!strcmp (name, elem->name)) {
			cobc_xref_link (&elem->xref, line, 0);
			return;
		}
	}

	elem = cobc_parse_malloc (sizeof (struct cb_call_elem));
	elem->name = cobc_strdup (name);
	elem->is_identifier = is_ident;
	elem->is_system = is_sys;
	cobc_xref_link (&elem->xref, line, 0);

	if (current_program->call_xref.head == NULL) {
		current_program->call_xref.head = elem;
	} else if (current_program->call_xref.tail != NULL) {
		current_program->call_xref.tail->next = elem;
	}
	current_program->call_xref.tail = elem;
#else
	COB_UNUSED (name);
	COB_UNUSED (line);
	COB_UNUSED (is_ident);
#endif
}

#ifdef COB_INTERNAL_XREF
static void
xref_print (struct cb_xref *xref, const enum xref_type type, struct cb_xref *xref_parent)
{
	struct cb_xref_elem	*elem;
	int     		cnt;
	int     		maxcnt = cb_listing_wide ? 10 : 5;

	if (xref->head == NULL) {
		sprintf (print_data + pd_off, "  ");
		if (type == XREF_FIELD) {
			/* check if parent has any reference and use it for the current field */
			if (xref_parent && xref_parent->head) {
				xref->head = xref_parent->head;
			}
			/* references by child only are stored in xref->tail if xref->head was NULL */
			if (xref->head && xref->tail) {
				sprintf (print_data + pd_off, "referenced by parent/child");
			} else if (xref->head) {
				sprintf (print_data + pd_off, "referenced by parent");
			} else if (xref->tail) {
				sprintf (print_data + pd_off, "referenced by child");
			} else {
				sprintf (print_data + pd_off, "not referenced");
			}
		} else {
			sprintf (print_data + pd_off, "not referenced");
		}
		print_program_data (print_data);
		return;
	}

	cnt = 0;
	for (elem = xref->head; elem; elem = elem->next) {
		pd_off += sprintf (print_data + pd_off, "  %-6d", elem->line);
		if (++cnt >= maxcnt) {
			cnt = 0;
			terminate_str_at_first_trailing_space (print_data);
			print_program_data (print_data);
			if (elem->next) {
				pd_off = sprintf (print_data, "%38.38s", " ");
			}
		}
	}
	if (cnt) {
		terminate_str_at_first_trailing_space (print_data);
		print_program_data (print_data);
	}
}

static void
xref_88_values (struct cb_field *field)
{
	struct cb_field *f;
	char lcl_name[LCL_NAME_LEN] = { '\0' };

	for (f = field->validation; f; f = f->sister) {
		strncpy (lcl_name, (char *)f->name, LCL_NAME_MAX);
		pd_off = sprintf (print_data,
			"%-30.30s %-6d ",
			lcl_name, f->common.source_line);
		xref_print (&f->xref, XREF_FIELD, NULL);

	}
}

static int
xref_fields (struct cb_field *top)
{
	char		lcl_name[LCL_NAME_LEN];
	int		found = 0;

	for (; top; top = top->sister) {

		/* no entry for internal generated fields
		   other than used special indexes */
		if (!top->level || (top->special_index && !top->count)) {
			continue;
		}

		strncpy (lcl_name, check_filler_name ((char *)top->name), LCL_NAME_MAX);
		lcl_name[LCL_NAME_MAX] = 0;	/* make sure we always have the trailing NULL */
		if (!strcmp (lcl_name, "FILLER") && !top->validation) {
			if (top->children) {
				found += xref_fields (top->children);
			}
			continue;
		}
		found = 1;
		pd_off = sprintf (print_data, "%-30.30s %-6d ",
			 lcl_name, top->common.source_line);

		/* print xref for field */
		if (top->parent) {
			xref_print (&top->xref, XREF_FIELD, &top->parent->xref);
		} else {
			xref_print (&top->xref, XREF_FIELD, NULL);
		}

		/* print xref for all assigned 88 validation entries */
		if (top->validation) {
			xref_88_values (top);
		}

		/* print xref for all childs */
		if (top->children) {
			(void)xref_fields (top->children);
		}
	}
	return found;
}

static void
xref_files_and_their_records (cb_tree file_list_p)
{
	cb_tree	l;

	for (l = file_list_p; l; l = CB_CHAIN (l)) {
		pd_off = sprintf (print_data, "%-30.30s %-6d ",
			 CB_FILE (CB_VALUE (l))->name,
			 CB_FILE (CB_VALUE (l))->common.source_line);
		xref_print (&CB_FILE (CB_VALUE (l))->xref, XREF_FILE, NULL);
		if (CB_FILE (CB_VALUE (l))->record) {
			(void)xref_fields (CB_FILE (CB_VALUE (l))->record);
		}
		print_program_data ("");
	}
}

static int
xref_fields_in_section (struct cb_field *first_field_in_section)
{
	int found = 0;

	if (first_field_in_section != NULL) {
		found = !!xref_fields (first_field_in_section);
		print_program_data ("");
	}
	return found;
}

static int
xref_labels (cb_tree label_list_p)
{
	cb_tree	l;
	char	label_type = ' ';
	struct cb_label *lab;

	for (l = label_list_p; l; l = CB_CHAIN (l)) {
		if (CB_LABEL_P(CB_VALUE(l))) {
			lab = CB_LABEL (CB_VALUE (l));
			if (lab->xref.skip) {
				continue;
			}
			if (lab->flag_entry) {
				label_type = 'E';
				sprintf (print_data, "E %-28.28s %d",
					lab->name, lab->common.source_line);
				print_program_data (print_data);
				continue;
			} else if (lab->flag_section) {
				label_type = 'S';
			} else {
				label_type = 'P';
			}
			pd_off = sprintf (print_data, "%c %-28.28s %-6d ",
				label_type, lab->name, lab->common.source_line);
			xref_print (&lab->xref, XREF_LABEL, NULL);
		}
	}
	if (label_type == ' ') {
		return 0;
	} else {
		return 1;
	}
}

static int
xref_calls (struct cb_call_xref *list)
{
	struct cb_call_elem *elem;
	int gotone = 0;

	if (list->head) {
		set_listing_header_xref (XREF_FUNCTION);
		force_new_page_for_next_line ();
		print_program_header ();
	}

	for (elem = list->head; elem; elem = elem->next) {
		gotone = 1;
		pd_off = sprintf (print_data, "%c %-28.28s %-6.6s ",
			elem->is_identifier ? 'I' : 'L',
			elem->name,
			elem->is_system ? "SYSTEM" : "EXTERN");
		xref_print (&elem->xref, XREF_FUNCTION, NULL);
	}
	return gotone;
}
#endif

static void
print_program_trailer (void)
{
	struct cb_program	*p;
	struct cb_program	*q;
	struct list_error	*err;
	int			print_names = 0;
	int			print_break = 1;
	int			found;
	char			err_msg[BUFSIZ];

	/* needed for syntax-checks-only as codegen reverses the program list */
	if (cb_flag_syntax_only) {
		p = program_list_reverse (current_program);
	} else {
		p = current_program;
	}

	if (p != NULL) {

	/* Print program in symbol table / cross-reference if more than one program */
	/* MAYBE use a second header line and a forced page break instead */
		if (p->next_program) {
			print_names = 1;
		}
	
		/* Print file/symbol tables if requested */
		if (cb_listing_symbols) {
			set_listing_header_symbols();
			force_new_page_for_next_line ();
			print_program_header ();

			for (q = p; q; q = q->next_program) {
				if (print_names) {
					sprintf (print_data,
						"      %-14s      %s",
			 	 		(q->prog_type == CB_FUNCTION_TYPE ?
				 			"FUNCTION" : "PROGRAM"),
			 	 		q->program_name);
					print_program_data (print_data);
					print_program_data ("");
				}
				found = 0;
				if (q->file_list) {
					print_files_and_their_records (q->file_list);
					found++;
				}
				found += print_fields_in_section (q->working_storage);
				found += print_fields_in_section (q->local_storage);
				found += print_fields_in_section (q->linkage_storage);
				found += print_fields_in_section (q->screen_storage);
				found += print_fields_in_section (q->report_storage);
				if (!found) {
					snprintf (print_data, CB_PRINT_LEN, "      %s",
						_("No fields defined."));
					print_program_data (print_data);
					print_program_data ("");
				}
			}
			print_break = 0;
		}

#ifdef COB_INTERNAL_XREF
		/* Print internal cross reference if requested */
		if (cb_listing_xref) {

			for (q = p; q; q = q->next_program) {

				set_listing_header_xref (XREF_FIELD);
				force_new_page_for_next_line ();
				print_program_header ();

				if (print_names) {
					sprintf (print_data,
						 "%s %s",
			 	 		(q->prog_type == CB_FUNCTION_TYPE ?
				 			"FUNCTION" : "PROGRAM"),
			 	 		q->program_name);
					print_program_data (print_data);
					print_program_data ("");
				}
				found = 0;
				if (q->file_list) {
					xref_files_and_their_records (q->file_list);
					found++;
				}
				found += xref_fields_in_section (q->working_storage);
				found += xref_fields_in_section (q->local_storage);
				found += xref_fields_in_section (q->linkage_storage);
				found += xref_fields_in_section (q->screen_storage);
				found += xref_fields_in_section (q->report_storage);
				if (!found) {
					snprintf (print_data, CB_PRINT_LEN, "      %s",
						_("No fields defined."));
					print_program_data (print_data);
					print_program_data ("");
				}

				set_listing_header_xref (XREF_LABEL);
				force_new_page_for_next_line ();
				print_program_header ();

				if (print_names) {
					sprintf (print_data,
						 "%s %s",
			 	 		(q->prog_type == CB_FUNCTION_TYPE ?
				 			"FUNCTION" : "PROGRAM"),
			 	 		q->program_name);
					print_program_data (print_data);
					print_program_data ("");
				}
				if (!xref_labels (q->exec_list)) {
					snprintf (print_data, CB_PRINT_LEN, "      %s",
						_("No labels defined."));
					print_program_data (print_data);
					print_program_data ("");
				};

				xref_calls (&q->call_xref);
			}
			print_break = 0;
		}
#endif
	}

	set_listing_header_none();
	print_program_data ("");
	if (print_break) {
		print_program_data ("");
	}

	/* Print error/warning summary */
	if (cb_listing_error_head) {
		force_new_page_for_next_line ();
		print_program_data (_("Error/Warning summary:"));
		print_program_data ("");
		err = cb_listing_error_head;
		do {
			snprintf (err_msg, BUFSIZ, "%s: %d: %s%s",
				err->file, err->line, err->prefix, err->msg);
			print_program_data (err_msg);
			err = err->next;
		} while (err);
		print_program_data ("");

		free_error_list (cb_listing_error_head);
		cb_listing_error_head = NULL;
		cb_listing_error_tail = NULL;
	}

	/* Print error counts */

	switch (warningcount) {
	case 0:
		print_program_data (_("0 warnings in compilation group"));
		break;
	case 1:
		/* FIXME: Change to P_, needs changes to Makevars and tests */
		print_program_data (_("1 warning in compilation group"));
		break;
	default:
		snprintf (print_data, CB_PRINT_LEN,
			_("%d warnings in compilation group"), warningcount);
		print_program_data (print_data);
		break;
	}
	switch (errorcount) {
	case 0:
		print_program_data (_("0 errors in compilation group"));
		break;
	case 1:
		/* FIXME: Change to P_, needs changes to Makevars and tests */
		print_program_data (_("1 error in compilation group"));
		break;
	default:
		snprintf (print_data, CB_PRINT_LEN,
			_("%d errors in compilation group"), errorcount);
		print_program_data (print_data);
		break;
	}
	if (errorcount > cb_max_errors) {
		snprintf (print_data, CB_PRINT_LEN,
			_("Too many errors in compilation group: %d maximum errors"),
			cb_max_errors);
		print_program_data (print_data);
	}
	force_new_page_for_next_line ();
}

/*
  return pointer to next non-space character
*/
static COB_INLINE COB_A_INLINE char *
get_next_nonspace (char * pos)
{
	while (*pos != '\0' && isspace ((unsigned char)*pos)) {
		pos++;
	}
	return pos;
}

/*
  Find next token after bp, copy it to token and copy the token terminator to
  term. Return pointer to the character after the terminator.
*/
static char *
get_next_token (char *bp, char *token, char *term)
{
	char	*token_start = token;
	int	in_string = 0;

	/* Repeat until a token is found */
	do {
		bp = get_next_nonspace (bp);

		term[0] = '\0';
		term[1] = '\0';
		if (*bp == '\0') {
			return NULL;
		}

		/* Copy characters into token until a terminator is found. */
		while (*bp) {
			/* Return character strings as a single token */
			if (*bp == '"' || *bp == '\'') {
				in_string = !in_string;
				*token++ = *bp++;
				continue;
			}
			if (in_string) {
				*token++ = *bp++;
				continue;
			}
			if (*bp == '.' && isdigit((unsigned char)*(bp + 1))) {
				;
			} else if (isspace ((unsigned char)*bp) || *bp == ',' || *bp == '.' || *bp == ';') {
				term[0] = *bp++;
				break;
			}
			*token++ = *bp++;
		}
		*token = '\0';
	} while (*token_start == '\0' && *term != '\0');

	return bp;
}

static void
terminate_str_at_first_of_char (const char c, char * const str)
{
	char	*first_instance  = strchr (str, c);

	if (first_instance != NULL) {
		*first_instance = '\0';
	}
}

/*
  Copies the next CB_LINE_LENGTH chars from fd into out_line. If fixed is true,
  out_line is padded with spaces to column CB_ENDLINE. The return value is
  either the length of out_line, or -1 if the end of fd is reached.
*/
static int
get_next_listing_line (FILE *fd, char **pline, int fixed)
{
	char	*in_char, *out_line;
	unsigned int	i = 0;
	char	in_line[CB_LINE_LENGTH + 2];

	if (*pline == NULL) {
	   *pline = cobc_malloc (CB_LINE_LENGTH + 2);
	}
	out_line = *pline;

	if (!fgets (in_line, CB_LINE_LENGTH, fd)) {
		memset (out_line, 0, CB_LINE_LENGTH);
		return -1;
	}

	terminate_str_at_first_of_char ('\n', in_line);
	terminate_str_at_first_of_char ('\r', in_line);

	for (in_char = in_line; i != CB_LINE_LENGTH && *in_char; in_char++) {
		if (*in_char == '\t') {
			out_line[i++] = ' ';
			while (i % cb_tab_width != 0) {
				out_line[i++] = ' ';
				if (i == CB_LINE_LENGTH) {
					break;
				}
			}
		} else {
			out_line[i++] = *in_char;
		}
	}

	if (fixed) {
		while (i < (unsigned int)CB_ENDLINE) {
			out_line[i++] = ' ';
		}
	} else {
		out_line[i++] = ' ';
	}
	out_line[i] = 0;

	return i;
}

/*
  return pointer to first non-space character (ignoring sequence area)
*/
static COB_INLINE COB_A_INLINE char *
get_first_nonspace (char *line, const enum cb_format source_format)
{
	if (source_format != CB_FORMAT_FREE) {
		return get_next_nonspace (line + CB_INDICATOR + 1);
	} else {
		return get_next_nonspace (line);
	}
}

/*
  check for compiler directive indicator and return
  position of compiler instruction or NULL if not found
*/
static char *
get_directive_start (char *line, const enum cb_format source_format)
{
	char	*curr_pos;

	curr_pos = get_first_nonspace (line, source_format);
	if (*curr_pos == '>' && *++curr_pos == '>') {
		curr_pos = get_next_nonspace (++curr_pos);
		if (*curr_pos != 0) {
			return curr_pos;
		}
	}
	return NULL;
}

/*
  check for >> LISTING directive and set on_off value
*/
static int
line_has_listing_directive (char *line, const enum cb_format source_format, int *on_off)
{
	char	*token;

	token = get_directive_start (line, source_format);
	
	if (token != NULL &&
		!strncasecmp (token, "LISTING", 7)) {
		token += 7;
		*on_off = 1;
		token = get_next_nonspace (token);
		if (!strncasecmp (token, "OFF", 3))
			*on_off = 0;
		return 1;
	}
	return 0;
}

/*
  check for >> PAGE directive and page eject indicator
*/
static int
line_has_page_eject (char *line, const enum cb_format source_format)
{
	char	*directive_start;

	if (source_format != CB_FORMAT_FREE && line[CB_INDICATOR] == '/') {
		return 1;
	} else {
		directive_start = get_directive_start (line, source_format);
		return directive_start != NULL
			&& !strncasecmp (directive_start, "PAGE", 4);
	}
}
/*
  check for listing statements in current line and handle them
*/
static int
line_has_listing_statement (char *line, const enum cb_format source_format)
{
	char	*statement_start, *curr_pos;
	int		size;

	/* check if we actually want to process any listing statement */
	if (cb_listing_statements > CB_OBSOLETE) {
		return 0;
	}

	curr_pos = get_first_nonspace (line, source_format);

	if (curr_pos == NULL) {
		return 0;
	}

	statement_start = curr_pos++;

	/* extract first word with max. length of 6 */
	for (size = 1; size < 6 && curr_pos != 0; size++, curr_pos++) {
		if ((*curr_pos == ' ' )
			||  (*curr_pos == '.' )
			||  (*curr_pos == '*' && (*(curr_pos + 1) == '>' ) )) {
			break;
		}
	}

	/* compare word against listing statements */
	if (size != 5) {
		return 0;
	}
	if ((strncasecmp (statement_start, "EJECT", 5))
	&&  (strncasecmp (statement_start, "SKIP1", 5))
	&&  (strncasecmp (statement_start, "SKIP2", 5))
	&&  (strncasecmp (statement_start, "SKIP3", 5))
	&&  (strncasecmp (statement_start, "TITLE", 5))) {
		return 0;
	}

	/* handle statements */
	if (!strncasecmp (statement_start, "TITLE", 5)) {
		/* check if we actually want to process TITLE as a statement 
		   note: the title statement is an extra listing-directive statement */
		if (cb_title_statement > CB_OBSOLETE) {
			return 0;
		}

		/* FIXME: the title should be handled correctly as literal */
		while (*curr_pos != 0) {
			if (*++curr_pos != ' ') {
				curr_pos++; /* skip start of literal */
				break;
			}
		}
		statement_start = curr_pos;
		for (size = 1; size < 80 && curr_pos != 0; size++, curr_pos++) {
			if ((*curr_pos == '.' )
				||  (*curr_pos == '*' && (*(curr_pos + 1) == '>' ) )) {
				break;
			}
		}
		snprintf (print_data, size, "%s", statement_start);
		terminate_str_at_first_trailing_space (print_data);
		size = strlen (print_data);
		snprintf (cb_listing_title, size, "%s", print_data);
		force_new_page_for_next_line ();
	} else {
		if (!strncasecmp (statement_start, "EJECT", 5)) {
			force_new_page_for_next_line ();
		} else if (!strncasecmp (statement_start, "SKIP1", 5))  {
			print_program_data ("\n");
		} else if (!strncasecmp (statement_start, "SKIP2", 5)) {
			print_program_data ("\n\n");
		} else if (!strncasecmp (statement_start, "SKIP3", 5)) {
			print_program_data ("\n\n\n");
		}
	}
	return 1;
}

static void
print_fixed_line (const int line_num, char pch, char *line)
{
	int		i;
	int		len = strlen (line);
	const int	max_chars_on_line = cb_listing_wide ? 112 : 72;
	const char	*format_str;

	if (line[CB_INDICATOR] == '&') {
		line[CB_INDICATOR] = '-';
		pch = '+';
	}

	for (i = 0; len > 0; i += max_chars_on_line, len -= max_chars_on_line) {
		if (cb_listing_wide) {
			format_str = "%06d%c %-112.112s";
		} else {
			format_str = "%06d%c %-72.72s";
		}
		sprintf (print_data, format_str, line_num, pch, line + i);
		terminate_str_at_first_trailing_space (print_data);
		print_program_data (print_data);

		if (cb_text_column == 72) {
			break;
		}
		pch = '+';
	}
}

static void
print_free_line (const int line_num, char pch, char *line)
{
	int		i;
	int		len = strlen (line);
	const int	max_chars_on_line = cb_listing_wide ? 112 : 72;
	const char	*format_str;

	for (i = 0; len > 0; i += max_chars_on_line, len -= max_chars_on_line) {
		if (cb_listing_wide) {
			format_str = "%06d%c %-112.112s";
		} else {
			format_str = "%06d%c %-72.72s";
		}
		sprintf (print_data, format_str, line_num, pch, line + i);
		terminate_str_at_first_trailing_space (print_data);
		print_program_data (print_data);
		pch = '+';
	}
}

static void
print_errors_for_line (const struct list_error * const first_error,
		       const int line_num)
{
	const struct list_error	*err;
	const int	max_chars_on_line = cb_listing_wide ? 120 : 80;
	int msg_off;

	for (err = first_error; err; err = err->next) {
		if (err->line == line_num) {
			pd_off = snprintf (print_data, max_chars_on_line, "%s%s", err->prefix, err->msg);
			if (pd_off == -1) {	/* snprintf returns -1 in MS and on HPUX if max is reached */
				pd_off = max_chars_on_line;
				print_data[max_chars_on_line - 1] = 0;
			}
			if (pd_off >= max_chars_on_line) {
				/* trim on last space */
				pd_off = strlen (print_data) - 1;
				while (pd_off && !isspace (print_data[pd_off])) {
					pd_off--;
				}
				print_data[pd_off] = '\0';
				print_program_data (print_data);
				msg_off = strlen(err->prefix);
				pd_off = strlen (print_data) - msg_off;
				if (msg_off < 2) msg_off = 2;
				memset (print_data, ' ', msg_off - 1);
				snprintf (print_data + msg_off - 2, max_chars_on_line, "%c%s", '+', err->msg + pd_off);
			}
			print_program_data (print_data);
		}
	}
}

static void
print_line (struct list_files *cfile, char *line, int line_num, int in_copy)
{
	struct list_skip	*skip;
	int	do_print;
	int	on_off;
	char	pch;

	do_print = cfile->listing_on;
	if (line_has_listing_directive (line, cfile->source_format, &on_off)) {
		cfile->listing_on = on_off;
		/* always print the directive itself */
		do_print = 1;
	} else if (line_has_page_eject (line, cfile->source_format)) {
		force_new_page_for_next_line ();
	} else if (line_has_listing_statement (line, cfile->source_format)) {
		do_print = 0;
	}

	if (do_print) {
		pch = in_copy ? 'C' : ' ';
		for (skip = cfile->skip_head; skip; skip = skip->next) {
			if (skip->skipline == line_num) {
				pch = 'X';
				break;
			}
		}

		terminate_str_at_first_trailing_space (line);
		if (cfile->source_format == CB_FORMAT_FIXED) {
			print_fixed_line (line_num, pch, line);
		} else { /* CB_FORMAT_FREE */
			print_free_line (line_num, pch, line);
		}
	}

	/* Print errors regardless of LISTING setting */
	if (cfile->err_head) {
		print_errors_for_line (cfile->err_head, line_num);
	}
}

/*
  Copy each token in pline from the start of pline[first_idx] to the end of
  pline[last_idx] into cmp_line, separated by a space. Tokens are copied from
  the first_col of each line and up to the end of line or the sequence area (if
  fixed is true).
  Return the column to which pline[last_idx] was read up to.

  first_col is zero-indexed.
*/
static int
compare_prepare (char *cmp_line, char *pline[CB_READ_AHEAD],
		 int first_idx, int last_idx, int first_col, int fixed)
{
	int	i;
	int	out_pos = 0;
	int	line_idx;
	int	in_string = 0;
	int	last_col = CB_SEQUENCE;
	int	last_nonspace;

	cmp_line[0] = 0;

	/* Collapse pline into a string of tokens separated by spaces */
	for (line_idx = first_idx; line_idx < last_idx; line_idx++) {
		if (!fixed) {
			last_col = strlen (pline[line_idx]) - 1;
		}

		/* Go to the last non-space character */
		for (last_nonspace = last_col;
		     isspace ((unsigned char)pline[line_idx][last_nonspace]) && last_nonspace > first_col;
		     last_nonspace--);
		/* Go to first non-space character */
		for (i = first_col; (i <= last_nonspace) && isspace ((unsigned char)pline[line_idx][i]); i++);

		/* Copy chars between the first and last non-space characters */
		while (i <= last_nonspace) {
			if (isspace ((unsigned char)pline[line_idx][i])) {
				cmp_line[out_pos++] = ' ';
				for (i++; (i <= last_nonspace) && isspace ((unsigned char)pline[line_idx][i]); i++);
				if (i > last_nonspace) {
					break;
				}
			} else if (pline[line_idx][i] == '"') {
				/*
				  Merge multi-part strings into one string,
				  reading another line if necessary to find the
				  end.
				*/
				if (in_string) {
					i++;
				} else {
					cmp_line[out_pos++] = pline[line_idx][i++];
					in_string = 1;
				}

				for (; (i <= last_nonspace) && (pline[line_idx][i] != '"'); ) {
					cmp_line[out_pos++] = pline[line_idx][i++];
				}
				if (pline[line_idx][i] == '"') {
					cmp_line[out_pos++] = pline[line_idx][i++];
					in_string = 0;
				}
				if (i > last_nonspace) {
					break;
				}
			} else {
				cmp_line[out_pos++] = pline[line_idx][i++];
			}
		}
	}
	cmp_line[out_pos] = 0;
#ifdef DEBUG_REPLACE
	fprintf (stdout, "   last_col = %d\n   cmp_line: %s\n", last_col, cmp_line);
#endif
	return last_col;
}

/*
  Add adjust to each line number less than line_num (if appropriate) in cfile's
  copy, replace and error lists.
*/
static void
adjust_line_numbers (struct list_files *cfile, int line_num, int adjust)
{
	struct list_files	*cur;
	struct list_replace	*rep;
	struct list_error	*err;

	for (cur = cfile->copy_head; cur; cur = cur->next) {
		cur->copy_line += adjust;
	}

	for (rep = cfile->replace_head; rep; rep = rep->next) {
		if (rep->firstline > line_num) {
			rep->firstline += adjust;
		}
	}

	for (err = cfile->err_head; err; err = err->next) {
		err->line += adjust;
	}
}

static COB_INLINE COB_A_INLINE int
is_debug_line (char *line, int fixed)
{
	if (line == NULL || line[0] == 0) {
		return 0;
	}
	return !cb_flag_debugging_line
		&& ((fixed && IS_DEBUG_LINE (line))
		    || (!fixed && !strncasecmp (line, "D ", 2)));
}

static COB_INLINE COB_A_INLINE int
is_comment_line (char *line, int fixed)
{
	if (line == NULL || line[0] == 0) {
		return 0;
	}
	return (fixed && IS_COMMENT_LINE (line))
		|| (!fixed && !strncmp (line, "*>", 2));
}

static int
is_continuation_line (char *line, int fixed)
{
	int i;

	if (line == NULL || line[0] == 0) {
		return 0;
	}
	if (fixed) {
		/* check for "-" in column 7 */
		if (IS_CONTINUE_LINE (line)) {
			return 1;
		}
	} else {
		/* check for "&" as last character */
		/* CHECKME: does this work with inline comments after "&"? */
		i = strlen (line) - 1;
		while (i && isspace (line[i])) i--;
		if (line[i] == '&') {
			return 1;
		}
	}

	return 0;
}

static void
abort_if_too_many_continuation_lines (int pline_cnt, const char *filename, int line_num)
{
	if (pline_cnt >= CB_READ_AHEAD) {
		cobc_err_msg (_("%s: %d: Too many continuation lines"),
				filename, line_num);
		cobc_abort_terminate ();
	}
}

static void
make_new_continuation_line (const char *cfile_name, char *pline[CB_READ_AHEAD],
			    int * const pline_cnt, int line_num)
{
	abort_if_too_many_continuation_lines (*pline_cnt + 1, cfile_name,
					      line_num);
	if (pline[*pline_cnt + 1] == NULL) {
		pline[*pline_cnt + 1] = cobc_malloc (CB_LINE_LENGTH + 2);
	}
	strcpy (pline[*pline_cnt + 1], pline[*pline_cnt]);
	strcpy (pline[*pline_cnt], pline[*pline_cnt - 1]);
	memset (&pline[*pline_cnt][CB_MARGIN_A], ' ',
		CB_SEQUENCE - CB_MARGIN_A);
	pline[*pline_cnt][CB_INDICATOR] = '&';

        (*pline_cnt)++;
}

static void
add_token_over_multiple_lines (const char *cfile_name,
			       char *pline[CB_READ_AHEAD],
			       int * const pline_cnt,
			       const int line_num,
			       const char *new_token,
			       const int first_col,
			       int new_token_len,
			       int * const out_line,
			       int * const out_col)
{
	int	tok_char = 0;

#ifdef DEBUG_REPLACE
	fprintf (stdout, "   new_token_len = %d\n", new_token_len);
#endif

	while (new_token_len) {
		/* Copy the token one character at a time. */
		pline[*out_line][(*out_col)++] = new_token[tok_char++];
		new_token_len--;

		/*
		  Move to the next line when reach the end of the current one.
		*/
		if (*out_col == CB_SEQUENCE) {
#ifdef DEBUG_REPLACE
			fprintf (stdout, "   NEW pline[%2d] = %s\n",
				 *out_line, pline[*out_line]);
#endif

			*out_col = first_col;
			(*out_line)++;

			/*
			  Allocate a new out_line if we are on the last
			  out_line.
			*/
			if (*out_line == *pline_cnt) {
				make_new_continuation_line (cfile_name, pline,
							    pline_cnt, line_num);
			}
		}
	}

	pline[*out_line][(*out_col)++] = ' ';
}

static void
reflow_replaced_fixed_format_text (const char *cfile_name, char *pline[CB_READ_AHEAD],
				   int * const pline_cnt, const int line_num,
				   char *newline, int first_col, const int last)
{
	int	first_nonspace;
	char	*new_line_ptr;
	char	new_token[CB_LINE_LENGTH + 2];
	char	token_terminator[2];
	int	out_col;
	int	out_line;
	int	force_next_line;
	int	new_token_len;

	new_line_ptr = get_next_token (newline, new_token, token_terminator);

	/*
	  Start adding tokens from margin B or the first non-space character.
	*/
	for (first_nonspace = first_col;
	     (first_nonspace < last) && isspace (pline[0][first_nonspace]);
	     first_nonspace++);
	if (first_nonspace >= CB_MARGIN_B) {
		first_col = CB_MARGIN_B;
	}

	/* For each line,  */
	for (out_line = 0; out_line < *pline_cnt; out_line++) {
		force_next_line = 0;
		out_col = first_col;

		/* Add as many token as possible to the current line. */
		while (new_line_ptr && !force_next_line) {
			new_token_len = strlen (new_token);
			if (new_token_len >= (CB_SEQUENCE - first_col)) {
				/*
				  If the new token does not fit on this line,
				  reflow it onto the next line.
				*/
			        add_token_over_multiple_lines (cfile_name, pline, pline_cnt, line_num,
							       new_token, first_col, new_token_len,
							       &out_line, &out_col);
			} else if ((out_col + 2 + new_token_len) < last) {
				/*
				  If the new token *and* its terminator fits,
				  copy it all onto the current line.
				*/
				strcpy (&pline[out_line][out_col], new_token);
				out_col += strlen (new_token);

				if (token_terminator[0]) {
					pline[out_line][out_col++] = token_terminator[0];
				} else {
				        pline[out_line][out_col++] = ' ';
				}
				if (token_terminator[0] == '.') {
					pline[out_line][out_col++] = ' ';
				}
			} else {
				force_next_line = 1;
				make_new_continuation_line (cfile_name, pline,
							    pline_cnt, line_num);
				continue;
			}
			new_line_ptr = get_next_token (new_line_ptr, new_token, token_terminator);
		}

		if (out_col == first_col) {
			pline[out_line][CB_INDICATOR] = ' ';
		}
		while (out_col < last) {
			pline[out_line][out_col++] = ' ';
		}

#ifdef DEBUG_REPLACE
		fprintf (stdout, "   NEW pline[%2d] = %s\n", out_line, pline[out_line]);
#endif
	}
}

static void
reflow_replaced_free_format_text (char *pline[CB_READ_AHEAD],
				  const int pline_cnt, char *newline,
				  const int first_col)
{
	char	*new_line_ptr;
	char	new_token[CB_LINE_LENGTH + 2];
	char	token_terminator[2];
	int	i;
	int	j;

	new_line_ptr = get_next_token (newline, new_token, token_terminator);

	for (i = 0; i < pline_cnt; i++) {
		/*
		  Terminate the line at null or the first non-space character.
		*/
		for (j = first_col; pline[i][j] && pline[i][j] == ' '; j++);
		pline[i][j] = '\0';

		/*
		  If the text has not been copied yet, copy it to the start of
		  the line.
		*/
		while (new_line_ptr) {
			/* TO-DO: Replace with strncat? */
			strcat (pline[i], new_token);
			strcat (pline[i], token_terminator);
			j++;
			new_line_ptr = get_next_token (new_line_ptr, new_token,
						       token_terminator);
		}

		if (j == first_col) {
			strcat (pline[i], " ");
		}
	}
}

static int
reflow_replaced_text (const char *cfile_name, char *pline[CB_READ_AHEAD],
		      int pline_cnt, int line_num, char *newline, int first_col,
		      int last_col, int fixed)
{
	if (fixed) {
	        reflow_replaced_fixed_format_text (cfile_name, pline,
						   &pline_cnt, line_num,
						   newline, first_col,
						   last_col);
	} else {
		reflow_replaced_free_format_text (pline, pline_cnt, newline,
						  first_col);
	}

	return pline_cnt;
}

/* TODO: Modularise! */

static int
print_replace_text (struct list_files *cfile, FILE *fd,
		    struct list_replace *rep, char *pline[CB_READ_AHEAD],
		    int pline_cnt, int line_num)
{
	char	*rfp = rep->from;
	char	*from_ptr;
	char	*to_ptr;
	const int	fixed = (cfile->source_format == CB_FORMAT_FIXED);
	int	first_col = fixed ? CB_MARGIN_A : 0;
	int	last;
	int	multi_token;
	int	match = 0;
	int	eof = 0;
	int	submatch = 0;
	int	seccount = 0;
	int	overread = 0;
	int	tokmatch = 0;
	int	subword = 0;
	int	ttix, ttlen, from_token_len;
	char	lterm[2];
	char	fterm[2];
	char	ftoken[CB_LINE_LENGTH + 2];
	char	tterm[2];
	char	ttoken[CB_LINE_LENGTH + 2];
	char	cmp_line[CB_LINE_LENGTH + 2];
	char	newline[CB_LINE_LENGTH + 2];
	char	from_line[CB_LINE_LENGTH + 2];

	if (is_comment_line (pline[0], fixed)) {
		return pline_cnt;
	}

	/* Trim the string to search and replace */
	terminate_str_at_first_trailing_space (rfp);
	while (*rfp && isspace (*rfp)) {
		rfp++;
	}
	multi_token = (strchr (rfp, ' ') != NULL);

#ifdef DEBUG_REPLACE
	fprintf (stdout, "print_replace_text: line_num = %d", line_num);
	fprintf (stdout, ", multi_token = %s, fixed = %s\n",
		 multi_token ? "TRUE" : "FALSE", fixed ? "TRUE" : "FALSE");
	fprintf (stdout, "   pline_cnt = %d\n", pline_cnt);
	for (int i = 0; i < pline_cnt; i++) {
		fprintf (stdout, "   pline[%2d]: %s\n", i, pline[i]);
	}
	fprintf (stdout, "   rep: first = %d, last = %d, lead_trail = %d\n",
		 rep->firstline, rep->lastline, rep->lead_trail);
	fprintf (stdout, "   from: '%s'\n", rfp);
	fprintf (stdout, "   to:   '%s'\n", rep->to);
#endif

	last = compare_prepare (cmp_line, pline, 0, pline_cnt, first_col, fixed);

	newline[0] = 0;
	if (multi_token) {
		/*
		  Attempt to match the source text from the beginning of each
		  line (continuing the match to the next line if need be). If a
		  match is found, output the line to newline with the match
		  replaced.
		*/

		strcpy (from_line, rfp);
		from_ptr = get_next_token (from_line, ftoken, fterm);
	force_next_line:
		to_ptr = get_next_token (cmp_line, ttoken, tterm);

		/*
		  Read tokens until the match is complete or until a match
		  fails.
		*/
		while (to_ptr && from_ptr) {
			if (!strcasecmp (ttoken, ftoken)) {
				/*
				  Mark two tokens as matched, then read next
				  pair.
				*/
				submatch = 1;
				if (fterm[0] == tterm[0]) {
					lterm[0] = 0;
				} else {
					lterm[0] = tterm[0];
				}
				lterm[1] = tterm[1];
				to_ptr = get_next_token (to_ptr, ttoken, tterm);
				from_ptr = get_next_token (from_ptr, ftoken, fterm);
			} else {
				/* Discard partial match. */
				if (seccount == 0) {
					strcat (newline, ttoken);
					strcat (newline, tterm);
				}
				submatch = 0;

				/* Start matching from beginning of from_line again. */
				strcpy (from_line, rfp);
				from_ptr = get_next_token (from_line, ftoken, fterm);
				to_ptr = get_next_token (to_ptr, ttoken, tterm);
				break;
			}
		}
		if (!from_ptr && submatch) {
			/*
			  If the match is complete, output the match's
			  replacement.
			*/
			match = 1;
			strcat (newline, rep->to);
			strcat (newline, lterm);
			if (to_ptr) {
				strcat (newline, ttoken);
				strcat (newline, tterm);
				strcat (newline, to_ptr);
			}
		} else if (!to_ptr && submatch) {
			/*
			  If we run out of chars from the original source, get
			  more.
			*/

#ifdef DEBUG_REPLACE
			fprintf (stdout, "   submatch = TRUE\n");
#endif
			if (eof) {
				return pline_cnt;
			}

			/*
			  Overwrite the current line if it is a comment or debug
			  line.
			*/
			if (is_comment_line (pline[pline_cnt], fixed)) {
				adjust_line_numbers (cfile, line_num,  -1);
				overread = 1;
			}
			if (is_debug_line (pline[pline_cnt], fixed)) {
				adjust_line_numbers (cfile, line_num,  -1);
				overread = 1;
			}

			/*
			  Read lines until we find a non-comment, non-debug
			  line.
			 */
		next_rec:
			if (!is_comment_line (pline[pline_cnt], fixed)) {
				pline_cnt++;
			}
			abort_if_too_many_continuation_lines (pline_cnt, cfile->name, line_num);
			if (get_next_listing_line (fd, &pline[pline_cnt], fixed) < 0) {
				pline[pline_cnt][0] = 0;
				eof = 1;
			}
			if (is_debug_line (pline[pline_cnt], fixed)
			    || is_comment_line (pline[pline_cnt], fixed)) {
				adjust_line_numbers (cfile, line_num,  -1);
				goto next_rec;
			}
#ifdef DEBUG_REPLACE
			fprintf (stdout, "   pline[%2d]: %s\n", pline_cnt - 1,
				 pline[pline_cnt - 1]);
#endif
			line_num++;
			seccount++;
			if (overread) {
				overread = 0;
				goto next_rec;
			}
			last = compare_prepare (cmp_line, pline, pline_cnt - 1, pline_cnt,
						first_col, fixed);
			strcat (newline, " ");
			goto force_next_line;
		}
	} else {
		strcpy (from_line, rfp);
		from_ptr = get_next_token (from_line, ftoken, fterm);
		if (ftoken[0] == ':' || ftoken[0] == '(') {
			subword = 1;
		}
		from_token_len = strlen (ftoken);

		/*
		  For each token in cmp_line, try to match it with the token in
		  from_line.
		 */
		for (to_ptr = get_next_token (cmp_line, ttoken, tterm); to_ptr;
		     to_ptr = get_next_token (to_ptr, ttoken, tterm)) {
#ifdef DEBUG_REPLACE
			fprintf (stdout, "   tterm = '%s', ttoken = '%s', ftoken = '%s'\n",
				 tterm, ttoken, ftoken);
#endif
			ttlen = strlen (ttoken);
			ttix = 0;
			if (rep->lead_trail == CB_REPLACE_LEADING) {
				subword = 1;
			} else if (rep->lead_trail == CB_REPLACE_TRAILING) {
				if (ttlen >= from_token_len) {
					subword = 1;
					ttix = ttlen - from_token_len;
					ttlen = ttix;
				}
			}
			if (subword) {
				tokmatch = !strncasecmp (&ttoken[ttix], ftoken, from_token_len);
			} else {
				tokmatch = !strcasecmp (ttoken, ftoken);
			}
			if (tokmatch) {
				if (subword) {
					if (rep->lead_trail == CB_REPLACE_LEADING) {
						strcat (newline, rep->to);
						strcat (newline, &ttoken[from_token_len]);
					} else if (rep->lead_trail == CB_REPLACE_TRAILING) {
						strncat (newline, ttoken, ttlen);
						strcat (newline, rep->to);
					} else {
						strcat (newline, rep->to);
					}
				} else {
					strcat (newline, rep->to);
				}
				strcat (newline, tterm);
				match = 1;
			} else {
				strcat (newline, ttoken);
				strcat (newline, tterm);
			}
		}
	}

	if (match) {
#ifdef DEBUG_REPLACE
		fprintf (stdout, "   match = TRUE\n   newline = %s\n", newline);
#endif
		pline_cnt = reflow_replaced_text (cfile->name, pline, pline_cnt,
						  line_num, newline, first_col,
						  last, fixed);
	}

	return pline_cnt;
}

static void
remove_replace_entries_before_line (struct list_files *cfile, const int line_num)
{
	struct list_replace	*rep;

	while (cfile->replace_head
	       && cfile->replace_head->firstline < line_num) {
		rep = cfile->replace_head;
		cfile->replace_head = rep->next;

		if (rep->from) {
			cobc_free (rep->from);
		}
		if (rep->to) {
			cobc_free (rep->to);
		}
		cobc_free (rep);
	}
}

static void
deep_copy_list_replace (struct list_replace *src, struct list_files *dst_file)
{
	struct list_replace	*copy;

	copy = cobc_malloc (sizeof (struct list_replace));
	memcpy (copy, src, sizeof (struct list_replace));
	copy->next = NULL;
	if (src->to) {
		copy->to = cobc_strdup (src->to);
	}
	if (src->from) {
		copy->from = cobc_strdup (src->from);
	}

	if (dst_file->replace_tail) {
		dst_file->replace_tail->next = copy;
	}
	if (!dst_file->replace_head) {
		dst_file->replace_head = copy;
	}
	dst_file->replace_tail = copy;
}

/* TO-DO: Modularise! */
/*
  Applies active REPLACE statements to the source lines in pline. Returns the
  number of lines after the replacement has been performed.
*/
static int
print_replace_main (struct list_files *cfile, FILE *fd,
		    char *pline[CB_READ_AHEAD], int pline_cnt, int line_num)
{
	static int		active_replace_stmt = 0;
	char			*to_ptr;
	struct list_replace	*rep;
	struct list_files 	*cur;
	int    		i;
	const int	fixed = (cfile->source_format == CB_FORMAT_FIXED);
	const int	first_col = fixed ? CB_MARGIN_A : 0;
	int		is_copy_line;
	int		is_replace_line;
	int		is_replace_off = 0;
	char		tterm[2] = { '\0' };
	char		ttoken[CB_LINE_LENGTH + 2] = { '\0' };
	char		cmp_line[CB_LINE_LENGTH + 2] = { '\0' };

	if (is_comment_line (pline[0], cfile->source_format != CB_FORMAT_FREE)) {
		return pline_cnt;
	}

#ifdef DEBUG_REPLACE
	fprintf (stdout, "print_replace_main: line_num = %d\n", line_num);
	fprintf (stdout, "   pline_cnt = %d\n", pline_cnt);
	for (i = 0; i < pline_cnt; i++) {
		fprintf (stdout, "   pline[%2d]: %s\n", i, pline[i]);
	}
#endif

	compare_prepare (cmp_line, pline, 0, pline_cnt, first_col,
			 cfile->source_format != CB_FORMAT_FREE);

	/* Check whether we're given a COPY or REPLACE statement. */
	to_ptr = get_next_token (cmp_line, ttoken, tterm);
	is_copy_line = !strcasecmp (ttoken, "COPY");
	is_replace_line = !strcasecmp (ttoken, "REPLACE");
	if (is_replace_line) {
		to_ptr = get_next_token (to_ptr, ttoken, tterm);
		is_replace_off = !strcasecmp (ttoken, "OFF");
	}

	/*
	  If no REPLACE is active, print nothing. If one is active, perform
	  replacements on the text.
	*/
	if (!active_replace_stmt && is_replace_line) {
		if (!is_replace_off) {
			active_replace_stmt = 1;
#ifdef DEBUG_REPLACE
			for (i = 0, rep = cfile->replace_head; rep; i++, rep = rep->next) {
				if (rep->firstline < (line_num + 10)) {
					if (i == 0)
						fprintf (stdout, "   replace_list: \n");
					fprintf (stdout, "      line[%d]: %d\n", i, rep->firstline);
					fprintf (stdout, "      from[%d]: '%s'\n", i, rep->from);
					fprintf (stdout, "      to  [%d]: '%s'\n", i, rep->to);
				}
			}
#endif
		}
	} else if (active_replace_stmt) {
		if (is_replace_line && is_replace_off) {
			active_replace_stmt = 0;
			remove_replace_entries_before_line (cfile, line_num);
		} else if (is_copy_line) {
			if (cfile->copy_head) {
				/* List all lines read so far and then discard them. */
				for (i = 0; i < pline_cnt; i++) {
					print_line (cfile, pline[i], line_num + i, 0);
					pline[i][0] = 0;
				}

				/* Print copybook, with REPLACE'd text. */
				cur = cfile->copy_head;
				if (!cur->replace_head) {
					for (rep = cfile->replace_head;
					     rep && rep->firstline <= line_num;
					     rep = rep->next) {
					        deep_copy_list_replace (rep, cur);
					}
				}
				print_program_code (cur, 1);
				cfile->copy_head = cur->next;

				/* Discard copybook reference */
				if (cur->name) {
					cobc_free ((char *)cur->name);
				}
				cobc_free (cur);
			}
		} else {
			/* Print text with replacements */
			for (rep = cfile->replace_head;
			     rep && rep->firstline < line_num;
			     rep = rep->next) {
				pline_cnt = print_replace_text (cfile, fd, rep, pline,
								pline_cnt, line_num);
			}
		}
	}

	return pline_cnt;
}

static struct list_error *
list_error_reverse (struct list_error *p)
{
	struct list_error	*next;
	struct list_error	*last;

	last = NULL;
	for (; p; p = next) {
		next = p->next;
		p->next = last;
		last = p;
	}
	return last;
}

/*
  Print the listing for the file in cfile, with copybooks expanded and
  after text has been REPLACE'd.

  This function also frees contents of cfile's copy_head and replace_head
  members, then sets them to NULL.
*/
static void
print_program_code (struct list_files *cfile, int in_copy)
{
	FILE			*fd = NULL;
	struct list_replace	*rep;
	struct list_files	*cur;
	struct list_error	*err;
	int	i;
	int	line_num = 1;
	const int	fixed = (cfile->source_format == CB_FORMAT_FIXED);
	int	eof = 0;
	int	pline_cnt = 0;
	char	*pline[CB_READ_AHEAD] = { NULL };
	int	lines_read;

	if (cfile->err_head) {
		cfile->err_head = list_error_reverse (cfile->err_head);
	}
	cfile->listing_on = 1;

#ifdef DEBUG_REPLACE
	struct list_skip *skip;

	fprintf (stdout, "print_program_code: in_copy = %s\n",
		 in_copy ? "YES" : "NO");
	fprintf (stdout, "   name: %s\n", cfile->name);
	fprintf (stdout, "   copy_line: %d\n", cfile->copy_line);
	for (i = 0, cur = cfile->copy_head; cur; i++, cur = cur->next) {
		if (i == 0) {
			fprintf (stdout, "   copy_books: \n");
		}
		fprintf (stdout, "      name[%d]: %s\n", i, cur->name);
		fprintf (stdout, "      line[%d]: %d\n", i, cur->copy_line);
	}
	for (i = 0, rep = cfile->replace_head; rep; i++, rep = rep->next) {
		if (i == 0) {
			fprintf (stdout, "   replace_list: \n");
		}
		fprintf (stdout, "      line[%d]: %d\n", i, rep->firstline);
		fprintf (stdout, "      from[%d]: '%s'\n", i, rep->from);
		fprintf (stdout, "      to  [%d]: '%s'\n", i, rep->to);
	}
	for (i = 0, err = cfile->err_head; err; i++, err = err->next) {
		if (i == 0) {
			fprintf (stdout, "   error_list: \n");
		}
		fprintf (stdout, "      line[%d]: %d\n", i, err->line);
		fprintf (stdout, "      pref[%d]: '%s'\n", i, err->prefix);
		fprintf (stdout, "      msg [%d]: '%s'\n", i, err->msg);
	}
	for (i = 0, skip = cfile->skip_head; skip; i++, skip = skip->next) {
		if (i == 0) {
			fprintf (stdout, "   skip_list: \n");
		}
		fprintf (stdout, "      line[%d]: %d\n", i, skip->skipline);
	}
#endif

	if (cfile->name) {
		fd = fopen (cfile->name, "r");
	}
	if (fd != NULL) {
		abort_if_too_many_continuation_lines (pline_cnt, cfile->name, line_num);
		if (get_next_listing_line (fd, &pline[pline_cnt], fixed) >= 0) {
		        do {
				abort_if_too_many_continuation_lines (pline_cnt, cfile->name, line_num);
				if (get_next_listing_line (fd, &pline[pline_cnt + 1], fixed) < 0) {
					eof = 1;
				}
				pline_cnt++;
				lines_read = 0;

				/* Collect all adjacent continuation lines */
				if (is_continuation_line (pline[fixed ? pline_cnt : pline_cnt - 1],
							  cfile->source_format != CB_FORMAT_FREE)) {
					continue;
				}
				/* Set line number as specified by #line directive. */
				if (!strncmp (pline[0], "#line ", 6)) {
					line_num = atoi (&pline[0][6]);
					lines_read = -1;
				}

				/* Perform text replacement on the lines. */
				if (!in_copy) {
					pline_cnt = print_replace_main (cfile, fd, pline, pline_cnt,
								       line_num);
				} else if (cfile->replace_head) {
					rep = cfile->replace_head;
					while (rep) {
						pline_cnt = print_replace_text (cfile, fd, rep, pline,
									       pline_cnt, line_num);
						rep = rep->next;
					}
				}

				/* Print each line except the last. */
				for (i = 0; i < pline_cnt; i++) {
					if (pline[i][0]) {
						if (fixed && pline[i][CB_INDICATOR] == '&') {
							print_line (cfile, pline[i], line_num, in_copy);
						} else {
							print_line (cfile, pline[i], line_num + i, in_copy);
							lines_read++;
						}
					}
				}

				/* Output copybooks which are COPY'd at the current line */
				if (cfile->copy_head
				    && cfile->copy_head->copy_line == line_num) {
					/* Add the current text replacements to the copybook */
					cur = cfile->copy_head;
					for (rep = cfile->replace_head; rep && in_copy;
					     rep = rep->next) {
						deep_copy_list_replace (rep, cur);
					}

					print_program_code (cur, 1);

					/* Delete the copybook reference when done */
					cfile->copy_head = cur->next;
					if (cur->name) {
						cobc_free ((void *)cur->name);
					}
					cobc_free (cur);
				}

				/* Delete all but the last line. */
				strcpy (pline[0], pline[pline_cnt]);
				for (i = 1; i < pline_cnt + 1; i++) {
				   memset (pline[i], 0, CB_LINE_LENGTH);
				}

				line_num += lines_read;
				pline_cnt = 0;
				if (pline[0][0] == 0) {
					eof = 1;
				}
			} while (!eof);
		}
		fclose (fd);

	/* Non-existent file, print errors to listing */
	} else {

		if (cfile->err_head) {
			for (err = cfile->err_head; err; err = err->next) {
				snprintf (print_data, CB_PRINT_LEN, "%s%s", err->prefix, err->msg);
				print_program_data (print_data);
			}
		}
		if (cfile->copy_head) {
			cur = cfile->copy_head;
			print_program_code (cur, 1);
			cfile->copy_head = cur->next;
			if (cur->name) {
				cobc_free ((void *)cur->name);
			}
			cobc_free (cur);
		}
	}

	for (i = 0; i < CB_READ_AHEAD; i++) {
		if (pline[i] == NULL) {
			break;
		}
		cobc_free (pline[i]);
	}

	/* Free replace data */
	if (cfile->replace_head) {
		free_replace_list (cfile->replace_head);
		cfile->replace_head = NULL;
	}

	/* Put errors on summary list */
	while (cfile->err_head) {
		err = cfile->err_head;
		cfile->err_head = err->next;
		if (cb_listing_error_tail) {
			cb_listing_error_tail->next = err;
		}
		if (!cb_listing_error_head) {
			cb_listing_error_head = err;
		}
		cb_listing_error_tail = err;
	}
}


/* Print the listing for the current file */
static void
print_program_listing (void)
{
	print_program_code (cb_listing_file_struct, 0);

	print_program_trailer ();

	/* TO-DO: Should this be here? */
	cobc_free ((void *)cb_listing_file_struct->name);
	cb_listing_file_struct->name = NULL;
}

/* Create single-element C source */

static int
process_translate (struct filename *fn)
{
	struct cb_program	*p;
	struct cb_program	*q;
	struct cb_program	*r;
	struct nested_list	*nlp;
	struct handler_struct	*hstr1;
	struct handler_struct	*hstr2;
	struct local_filename	*lf;
	int			ret;
	int			i;

	/* Initialize */
	cb_source_file = NULL;
	cb_source_line = 0;

	/* Open the input file */
	yyin = fopen (fn->preprocess, "r");
	if (!yyin) {
		cobc_terminate (fn->preprocess);
	}

	if (verbose_output) {
		fputs (_("parsing:"), stderr);
		fprintf (stderr, "\t%s (%s)\n", fn->preprocess, fn->source);
		fflush (stderr);
	}

	current_program = NULL;
	cb_init_constants ();

	/* Parse */
	ret = yyparse ();

	fclose (yyin);
	yyin = NULL;

	/* Release flex buffers - After file close */
	ylex_call_destroy ();

	if (verbose_output) {
		fputs (_("return status:"), stderr);
		fprintf (stderr, "\t%d\n", ret);
		fflush (stderr);
	}

	if (ret) {
		/* If processing raised errors set syntax-only flag to not
		   loose the information "no codegen occurred" */
		cb_flag_syntax_only = 1;
		return 1;
	}
	if (cb_flag_syntax_only) {
		return 0;
	}

	/* Set up USE GLOBAL handlers */
	p = current_program;
	for (q = p; q; q = q->next_program) {
		q->global_file_list = cb_list_reverse (q->global_file_list);
		if (q->nested_level) {
			for (r = q->next_program; r; r = r->next_program) {
				if (r->nested_level >= q->nested_level) {
					continue;
				}
				for (i = COB_OPEN_INPUT; i <= COB_OPEN_EXTEND; ++i) {
					hstr1 = &q->global_handler[i];
					hstr2 = &r->global_handler[i];
					if (!hstr1->handler_label &&
					    hstr2->handler_label &&
					    hstr2->handler_label->flag_global) {
						hstr1->handler_label = hstr2->handler_label;
						hstr1->handler_prog = r;
					}
				}
				if (!r->nested_level) {
					break;
				}
			}
		}
	}

	if (verbose_output) {
		fputs (_("translating:"), stderr);
		fprintf (stderr, "\t%s -> %s (%s)\n",
			 fn->preprocess, fn->translate, fn->source);
		fflush (stderr);
	}

	/* Open the output file */
	if (cb_unix_lf) {
		yyout = fopen (fn->translate, "wb");
	} else {
		yyout = fopen (fn->translate, "w");
	}
	if (!yyout) {
		cobc_terminate (fn->translate);
	}

	/* Open the common storage file */
	cb_storage_file_name = fn->trstorage;
	if (cb_unix_lf) {
		cb_storage_file = fopen (cb_storage_file_name, "wb");
	} else {
		cb_storage_file = fopen (cb_storage_file_name, "w");
	}
	if (!cb_storage_file) {
		cobc_terminate (cb_storage_file_name);
	}

	current_program = program_list_reverse (current_program);
	p = current_program;

	/* Set up local storage files */
	lf = NULL;
	ret = 1;
	for (q = p; q; q = q->next_program, ret++) {
		lf = cobc_main_malloc (sizeof(struct local_filename));
		lf->local_name = cobc_main_malloc (fn->translate_len + 12U);
		if (q == p && !q->next_program) {
			sprintf (lf->local_name, "%s.l.h", fn->translate);
		} else {
			sprintf (lf->local_name, "%s.l%d.h", fn->translate, ret);
		}
		if (cb_unix_lf) {
			lf->local_fp = fopen (lf->local_name, "wb");
		} else {
			lf->local_fp = fopen (lf->local_name, "w");
		}
		if (!lf->local_fp) {
			cobc_terminate (lf->local_name);
		}
		q->local_include = lf;
		lf->next = fn->localfile;
		fn->localfile = lf;
	}

	/* Entries for COMMON programs */
	for (q = p; q; q = q->next_program) {
		i = q->nested_level;
		for (nlp = q->common_prog_list; nlp; nlp = nlp->next) {
			for (r = q->next_program; r; r = r->next_program) {
				if (r->nested_level <= i) {
					break;
				}
				cb_insert_common_prog (r, nlp->nested_prog);
			}
		}
	}

	/* Translate to C */
	current_section = NULL;
	current_paragraph = NULL;
	current_statement = NULL;
	cb_source_line = 0;
	/* Temporarily disable cross-reference during C generation */
	if (cb_listing_xref) {
		cb_listing_xref = 0;
		rose_cobol_codegen (p, 0);
		cb_listing_xref = 1;
	} else {
		rose_cobol_codegen (p, 0);
	}

	/* Close files */
	if(unlikely(fclose (cb_storage_file) != 0)) {
		cobc_terminate (cb_storage_file_name);
	}
	cb_storage_file = NULL;
	if(unlikely(fclose (yyout) != 0)) {
		cobc_terminate (fn->translate);
	}
	yyout = NULL;
	for (q = p; q; q = q->next_program) {
		if (unlikely(!q->local_include->local_fp)) {
			continue;
		}
		if (unlikely(fclose (q->local_include->local_fp) != 0)) {
			cobc_terminate(lf->local_name);
		}
		q->local_include->local_fp = NULL;
	}
	return !!errorcount;
}

/* Create single-element assembly source */

static int
process_compile (struct filename *fn)
{
	char	*name;
	size_t	bufflen;
	size_t	size;

	if (output_name) {
		name = output_name;
	} else {
		name = file_basename (fn->source);
#ifndef	_MSC_VER
		strcat (name, ".s");
#endif
	}
	size = strlen (name);
#ifdef	_MSC_VER
	size *= 2U;
#endif

	bufflen = cobc_cc_len + cobc_cflags_len
			+ size + fn->translate_len
			+ cobc_include_len + 64U;

	cobc_chk_buff_size (bufflen);

#ifdef	_MSC_VER
	sprintf (cobc_buffer, gflag_set ?
		"%s /c %s %s /Od /MDd /Zi /FR /c /Fa\"%s\" /Fo\"%s\" \"%s\"" :
		"%s /c %s %s /MD /c /Fa\"%s\" /Fo\"%s\" \"%s\"",
			cobc_cc, cobc_cflags, cobc_include, name,
			name, fn->translate);
	if (verbose_output > 1) {
		return process (cobc_buffer);
	} else {
		return process_filtered (cobc_buffer, fn);
	}
#elif defined(__WATCOMC__)
	sprintf (cobc_buffer, "%s -fe=\"%s\" -s %s %s %s", cobc_cc, name,
			cobc_cflags, cobc_include, fn->translate);
	return process (cobc_buffer);
#else
	if (!cb_flag_main) {
		sprintf (cobc_buffer, "%s -S -o \"%s\" %s %s %s \"%s\"", cobc_cc, name,
			cobc_cflags, cobc_include, COB_PIC_FLAGS, fn->translate);
	} else {
		sprintf (cobc_buffer, "%s -S -o \"%s\" %s %s \"%s\"", cobc_cc, name,
			cobc_cflags, cobc_include, fn->translate);
	}
	return process(cobc_buffer);
#endif

}

/* Create single-element assembled object */

static int
process_assemble (struct filename *fn)
{
#ifndef _MSC_VER
	int		ret;
#endif
	size_t		bufflen;
#ifdef	__OS400__
	char	*name;
#endif

	bufflen = cobc_cc_len + cobc_cflags_len + fn->object_len
			+ fn->translate_len + cobc_include_len
			+ cobc_pic_flags_len + 64U;

	cobc_chk_buff_size (bufflen);

#ifdef	_MSC_VER
	sprintf (cobc_buffer, gflag_set ?
		"%s /c %s %s /Od /MDd /Zi /FR /Fo\"%s\" \"%s\"" :
		"%s /c %s %s     /MD          /Fo\"%s\" \"%s\"",
			cobc_cc, cobc_cflags, cobc_include,
			fn->object, fn->translate);
	if (verbose_output > 1) {
		return process (cobc_buffer);
	} else {
		return process_filtered (cobc_buffer, fn);
	}
#elif defined(__OS400__)
	name = (char *) fn->translate;
	if (name[0] != '/') {
		char	*p;

		p = cobc_main_malloc (COB_LARGE_BUFF);
		getcwd (p, COB_LARGE_BUFF);
		strcat (p, "/");
		strcat (p, name);
		name = p;
	}
	file_stripext ((char *) fn->object);
	sprintf (cobc_buffer, "%s -c %s %s -o %s %s",
		 cobc_cc, cobc_cflags, cobc_include,
		 fn->object, name);
	ret = process (cobc_buffer);
	if ((ret == 0) && cobc_flag_run) {
		ret = process_run (name);
	}
	return ret;
#elif defined(__WATCOMC__)
	if (cb_compile_level == CB_LEVEL_MODULE ||
	    cb_compile_level == CB_LEVEL_LIBRARY) {
		sprintf (cobc_buffer, "%s -c %s %s %s -fe=\"%s\" \"%s\"",
			 cobc_cc, cobc_cflags, cobc_include,
			 COB_PIC_FLAGS, fn->object, fn->translate);
	} else {
		sprintf (cobc_buffer, "%s -c %s %s -fe=\"%s\" \"%s\"",
			 cobc_cc, cobc_cflags, cobc_include,
			 fn->object, fn->translate);
	}
	ret = process (cobc_buffer);
	if ((ret == 0) && cobc_flag_run) {
		ret = process_run (fn->object);
	}
	return ret;
#else
	if (cb_compile_level == CB_LEVEL_MODULE ||
	    cb_compile_level == CB_LEVEL_LIBRARY ||
	    cb_compile_level == CB_LEVEL_ASSEMBLE) {
		sprintf (cobc_buffer, "%s -c %s %s %s -o \"%s\" \"%s\"",
			 cobc_cc, cobc_cflags, cobc_include,
			 COB_PIC_FLAGS, fn->object, fn->translate);
	} else {
		// Only for CB_LEVEL_EXECUTABLE
		sprintf (cobc_buffer, "%s -c %s %s -o \"%s\" \"%s\"",
			 cobc_cc, cobc_cflags, cobc_include,
			 fn->object, fn->translate);
	}
	ret = process (cobc_buffer);
	return ret;
#endif

}

/* Create single-element loadable object without intermediate stages */

static int
process_module_direct (struct filename *fn)
{
	char	*name;
#ifdef	_MSC_VER
	char	*exename;
#endif
	size_t	bufflen;
	size_t	size;
	int	ret;

	if (output_name) {
		name = output_name_buff;
		strcpy (name, output_name);
#if	defined(_MSC_VER) || defined(__OS400__) || defined(__WATCOMC__) || defined(__BORLANDC__)
		file_stripext (name);
#else
		if (strchr (output_name, '.') == NULL) {
			strcat (name, ".");
			strcat (name, COB_MODULE_EXT);
		}
#endif
	} else {
		name = file_basename (fn->source);
#if	!defined(_MSC_VER) && !defined(__OS400__) && !defined(__WATCOMC__) && !defined(__BORLANDC__)
		strcat (name, ".");
		strcat (name, COB_MODULE_EXT);
#endif
	}
#ifdef	_MSC_VER
	exename = cobc_stradd_dup (name, ".dll");
#endif

	size = strlen (name);
#ifdef	_MSC_VER
	size *= 2U;
#endif

	bufflen = cobc_cc_len + cobc_cflags_len
			+ cobc_include_len + cobc_shared_opt_len
			+ cobc_pic_flags_len + cobc_export_dyn_len
			+ size + fn->translate_len
#ifdef	_MSC_VER
			+ manilink_len
#endif
			+ cobc_ldflags_len + cobc_lib_paths_len + cobc_libs_len
			+ 128U;

	cobc_chk_buff_size (bufflen);

#ifdef	_MSC_VER
	sprintf (cobc_buffer, gflag_set ?
		"%s %s %s /Od /MDd /LDd /Zi /FR /Fe\"%s\" /Fo\"%s\" \"%s\" %s %s %s %s" :
		"%s %s %s     /MD  /LD          /Fe\"%s\" /Fo\"%s\" \"%s\" %s %s %s %s",
			cobc_cc, cobc_cflags, cobc_include, exename, name,
			fn->translate,
			manilink, cobc_ldflags, cobc_lib_paths, cobc_libs);
	if (verbose_output > 1) {
		ret = process (cobc_buffer);
	} else {
		ret = process_filtered (cobc_buffer, fn);
	}
	/* Embedding manifest */
	if (ret == 0) {
		sprintf (cobc_buffer,
			 "%s /manifest \"%s.manifest\" /outputresource:\"%s\";#2",
			 manicmd, exename, exename);
		ret = process (cobc_buffer);
		sprintf (cobc_buffer, "%s.manifest", exename);
		cobc_check_action (cobc_buffer);
	}
	sprintf (cobc_buffer, "%s.exp", name);
	cobc_check_action (cobc_buffer);
	sprintf (cobc_buffer, "%s.lib", name);
	if (strstr(fn->source, cobc_buffer) == NULL)	cobc_check_action (cobc_buffer);
	sprintf (cobc_buffer, "%s.%s", name, COB_OBJECT_EXT);
	if (strstr(fn->source, cobc_buffer) == NULL)	cobc_check_action (cobc_buffer);
#else	/* _MSC_VER */
#ifdef	__OS400__
	if (fn->translate[0] != '/') {
		char	*p;

		p = cobc_main_malloc (COB_LARGE_BUFF);
		getcwd (p, COB_LARGE_BUFF);
		strcat (p, "/");
		strcat (p, fn->translate);
		fn->translate = p;
		fn->translate_len = strlen (p);
	}
#endif
#ifdef	__WATCOMC__
	sprintf (cobc_buffer, "%s %s %s %s %s %s -fe=\"%s\" \"%s\" %s %s %s",
		 cobc_cc, cobc_cflags, cobc_include, COB_SHARED_OPT,
		 COB_PIC_FLAGS, COB_EXPORT_DYN, name,
		 fn->translate, cobc_ldflags, cobc_lib_paths, cobc_libs);
#else
	sprintf (cobc_buffer, "%s %s %s %s %s %s -o \"%s\" \"%s\" %s %s %s",
		 cobc_cc, cobc_cflags, cobc_include, COB_SHARED_OPT,
		 COB_PIC_FLAGS, COB_EXPORT_DYN, name,
		 fn->translate, cobc_ldflags, cobc_lib_paths, cobc_libs);
#endif
	ret = process (cobc_buffer);
#ifdef	COB_STRIP_CMD
	if (strip_output && ret == 0) {
		sprintf (cobc_buffer, "%s \"%s\"", COB_STRIP_CMD, name);
		ret = process (cobc_buffer);
	}
#endif
#endif	/* _MSC_VER */
	if ((ret == 0) && cobc_flag_run) {
		ret = process_run (name);
	}
	return ret;
}

/* Create single-element loadable object */

static int
process_module (struct filename *fn)
{
	char	*name;
#ifdef	_MSC_VER
	char	*exename;
#endif
	size_t	bufflen;
	size_t	size;
	int	ret;

	if (output_name) {
		name = output_name_buff;
		strcpy (name, output_name);
#if	defined(_MSC_VER) || defined(__OS400__) || defined(__WATCOMC__) || defined(__BORLANDC__)
		file_stripext (name);
#else
		if (strchr (output_name, '.') == NULL) {
			strcat (name, ".");
			strcat (name, COB_MODULE_EXT);
		}
#endif
	} else {
		name = file_basename (fn->source);
#if	!defined(_MSC_VER) && !defined(__OS400__) && !defined(__WATCOMC__) &&! defined(__BORLANDC__)
		strcat (name, ".");
		strcat (name, COB_MODULE_EXT);
#endif
	}
#ifdef	_MSC_VER
	exename = cobc_stradd_dup (name, ".dll");
#endif

	size = strlen (name);
	bufflen = cobc_cc_len
			+ cobc_shared_opt_len
			+ cobc_pic_flags_len + cobc_export_dyn_len
			+ size + fn->object_len
#ifdef	_MSC_VER
			+ manilink_len
#endif
			+ cobc_ldflags_len + cobc_lib_paths_len + cobc_libs_len
			+ 128U;

	cobc_chk_buff_size (bufflen);

#ifdef	_MSC_VER
	sprintf (cobc_buffer, gflag_set ?
		"%s /Od /MDd /LDd /Zi /FR /Fe\"%s\" \"%s\" %s %s %s %s" :
		"%s     /MD  /LD          /Fe\"%s\" \"%s\" %s %s %s %s",
		cobc_cc, exename, fn->object,
		manilink, cobc_ldflags, cobc_libs, cobc_lib_paths);
	if (verbose_output > 1) {
		ret = process (cobc_buffer);
	} else {
		ret = process_filtered (cobc_buffer, fn);
	}
	/* Embedding manifest */
	if (ret == 0) {
		sprintf (cobc_buffer,
			 "%s /manifest \"%s.manifest\" /outputresource:\"%s\";#2",
			 manicmd, exename, exename);
		ret = process (cobc_buffer);
		sprintf (cobc_buffer, "%s.manifest", exename);
		cobc_check_action (cobc_buffer);
	}
	sprintf (cobc_buffer, "%s.exp", name);
	cobc_check_action (cobc_buffer);
	sprintf (cobc_buffer, "%s.lib", name);
	if (strstr(fn->source, cobc_buffer) == NULL)	cobc_check_action (cobc_buffer);
	sprintf (cobc_buffer, "%s.obj", name);
	if (strstr(fn->source, cobc_buffer) == NULL)	cobc_check_action (cobc_buffer);
#else	/* _MSC_VER */
#ifdef	__WATCOMC__
	sprintf (cobc_buffer, "%s %s %s %s -fe=\"%s\" \"%s\" %s %s %s",
		 cobc_cc, COB_SHARED_OPT, COB_PIC_FLAGS, COB_EXPORT_DYN,
		 name, fn->object, cobc_ldflags, cobc_lib_paths, cobc_libs);
#else
	sprintf (cobc_buffer, "%s %s %s %s -o \"%s\" \"%s\" %s %s %s",
		 cobc_cc, COB_SHARED_OPT, COB_PIC_FLAGS, COB_EXPORT_DYN,
		 name, fn->object, cobc_ldflags, cobc_lib_paths, cobc_libs);
#endif
	ret = process (cobc_buffer);
#ifdef	COB_STRIP_CMD
	if (strip_output && ret == 0) {
		sprintf (cobc_buffer, "%s %s", COB_STRIP_CMD, name);
		ret = process (cobc_buffer);
	}
#endif
#endif	/* _MSC_VER */
	/* cobcrun job? */
	if ((ret == 0) && cobc_flag_run) {
		ret = process_run (name);
	}
	return ret;
}

/* Create multi-element loadable object */

static int
process_library (struct filename *l)
{
	struct filename	*f;
	char		*name;
#ifdef	_MSC_VER
	char	*exename;
#endif
	size_t		bufflen;
	size_t		size;
	int		ret;

	if (!l) {
		cobc_err_msg (_("call to '%s' with invalid parameter '%s'"),
			"process_library", "l");
		COBC_ABORT ();
	}

	for (f = l; f; f = f->next) {
		strcat (cobc_objects_buffer, "\"");
		strcat (cobc_objects_buffer, f->object);
		strcat (cobc_objects_buffer, "\" ");
	}

	if (output_name) {
		name = output_name_buff;
		strcpy (name, output_name);
#if	defined(_MSC_VER) || defined(__OS400__) || defined(__WATCOMC__) || defined(__BORLANDC__)
		file_stripext (name);
#else
		if (strchr (output_name, '.') == NULL) {
			strcat (name, ".");
			strcat (name, COB_MODULE_EXT);
		}
#endif
	} else {
		name = file_basename (l->source);
#if	!defined(_MSC_VER) && !defined(__OS400__) && !defined(__WATCOMC__) && !defined(__BORLANDC__)
		strcat (name, ".");
		strcat (name, COB_MODULE_EXT);
#endif
	}
#ifdef	_MSC_VER
	exename = cobc_stradd_dup (name, ".dll");
#endif

	size = strlen (name);
	bufflen = cobc_cc_len + cobc_shared_opt_len
			+ cobc_pic_flags_len + cobc_export_dyn_len
			+ size + cobc_objects_len + cobc_libs_len
#ifdef	_MSC_VER
			+ manilink_len
#endif
			+ cobc_ldflags_len + cobc_lib_paths_len
			+ 64U;

	cobc_chk_buff_size (bufflen);

#ifdef	_MSC_VER
	sprintf (cobc_buffer, gflag_set ?
		"%s /Od /MDd /LDd /Zi /FR /Fe\"%s\" %s %s %s %s %s" :
		"%s     /MD  /LD          /Fe\"%s\" %s %s %s %s %s",
		cobc_cc, exename, cobc_objects_buffer,
		manilink, cobc_ldflags, cobc_lib_paths, cobc_libs);
	if (verbose_output > 1) {
		ret = process (cobc_buffer);
	} else {
		ret = process_filtered (cobc_buffer, l);
	}
	/* Embedding manifest */
	if (ret == 0) {
		sprintf (cobc_buffer,
			 "%s /manifest \"%s.manifest\" /outputresource:\"%s\";#2",
			 manicmd, exename, exename);
		ret = process (cobc_buffer);
		sprintf (cobc_buffer, "%s.manifest", exename);
		cobc_check_action (cobc_buffer);
	}
	sprintf (cobc_buffer, "%s.exp", name);
	cobc_check_action (cobc_buffer);
	sprintf (cobc_buffer, "%s.lib", name);

	for (f = l; f; f = f->next) {
		if (strstr (f->source, cobc_buffer) != NULL) {
			break;
		}
	}
	if (!f)	cobc_check_action (cobc_buffer);
#else	/* _MSC_VER */
#ifdef	__WATCOMC__
	sprintf (cobc_buffer, "%s %s %s %s -fe=\"%s\" %s %s %s %s",
		 cobc_cc, COB_SHARED_OPT, COB_PIC_FLAGS,
		 COB_EXPORT_DYN, name, cobc_objects_buffer,
		 cobc_ldflags, cobc_lib_paths, cobc_libs);
#else
	sprintf (cobc_buffer, "%s %s %s %s -o \"%s\" %s %s %s %s",
		 cobc_cc, COB_SHARED_OPT, COB_PIC_FLAGS,
		 COB_EXPORT_DYN, name, cobc_objects_buffer,
		 cobc_ldflags, cobc_lib_paths, cobc_libs);
#endif
	ret = process (cobc_buffer);
#ifdef	COB_STRIP_CMD
	if (strip_output && ret == 0) {
		sprintf (cobc_buffer, "%s \"%s\"", COB_STRIP_CMD, name);
		ret = process (cobc_buffer);
	}
#endif
#endif	/* _MSC_VER */
	/* cobcrun job? */
	if ((ret == 0) && cobc_flag_run) {
		ret = process_run (name);
	}
	return ret;
}

/* Create executable */

static int
process_link (struct filename *l)
{
	struct filename	*f;
	const char		*name;
#ifdef	_MSC_VER
	const char		*exename;
#endif
	size_t		bufflen;
	size_t		size;
	int		ret;

	if (!l) {
		cobc_err_msg (_("call to '%s' with invalid parameter '%s'"),
			"process_link", "l");
		COBC_ABORT ();
	}

	for (f = l; f; f = f->next) {
#ifdef	__OS400__
		file_stripext ((char *) f->object);
		strcat (cobc_objects_buffer, f->object);
		strcat (cobc_objects_buffer, " ");
#else
		strcat (cobc_objects_buffer, "\"");
		strcat (cobc_objects_buffer, f->object);
		strcat (cobc_objects_buffer, "\" ");
#endif
	}

	if (output_name) {
#if	defined(_MSC_VER) || defined(__OS400__) || defined(__WATCOMC__) || defined(__BORLANDC__)
		name = cobc_main_strdup (output_name);
		file_stripext ((char *)name);
#else
		name = output_name;
#endif
	} else {
		if (l->file_is_stdin) {
			name = COB_DASH_OUT;
		} else {
			name = file_basename (l->source);
		}
	}
#ifdef	_MSC_VER
	exename = cobc_stradd_dup (name, ".exe");
#endif

	size = strlen (name);
	bufflen = cobc_cc_len + cobc_export_dyn_len
			+ size + cobc_objects_len
#ifdef	_MSC_VER
			+ manilink_len
#endif
			+ cobc_ldflags_len + cobc_libs_len + cobc_lib_paths_len
			+ 64U;

	cobc_chk_buff_size (bufflen);

#ifdef	_MSC_VER
	sprintf (cobc_buffer, gflag_set ?
		"%s /Od /MDd /Zi /FR /Fe\"%s\" %s %s %s %s %s" :
		"%s     /MD          /Fe\"%s\" %s %s %s %s %s",
		cobc_cc, exename, cobc_objects_buffer,
		manilink, cobc_ldflags, cobc_lib_paths, cobc_libs);
	if (verbose_output > 1) {
		ret = process (cobc_buffer);
	} else {
		ret = process_filtered (cobc_buffer, l);
	}
	/* Embedding manifest */
	if (ret == 0) {
		sprintf (cobc_buffer,
			 "%s /manifest \"%s.manifest\" /outputresource:\"%s\";#1",
			 manicmd, exename, exename);
		ret = process (cobc_buffer);
		sprintf (cobc_buffer, "%s.manifest", exename);
		cobc_check_action (cobc_buffer);
	}
#else	/* _MSC_VER */
#ifdef	__WATCOMC__
	sprintf (cobc_buffer, "%s %s -fe=\"%s\" %s %s %s %s",
		 cobc_cc, COB_EXPORT_DYN, name, cobc_objects_buffer,
		 cobc_ldflags, cobc_lib_paths, cobc_libs);
#else
	sprintf (cobc_buffer, "%s %s -o \"%s\" %s %s %s %s",
		 cobc_cc, COB_EXPORT_DYN, name, cobc_objects_buffer,
		 cobc_ldflags, cobc_lib_paths, cobc_libs);
#endif

	ret = process (cobc_buffer);

#ifdef	__hpux
	if (ret == 0) {
		sprintf (cobc_buffer, "chatr -s +s enable \"%s%s\" 1>/dev/null 2>&1",
			 name, COB_EXE_EXT);
		process (cobc_buffer);
	}
#endif

#ifdef	COB_STRIP_CMD
	if (strip_output && ret == 0) {
		sprintf (cobc_buffer, "%s \"%s%s\"",
			 COB_STRIP_CMD, name, COB_EXE_EXT);
		ret = process (cobc_buffer);
	}
#endif

#endif	/* _MSC_VER */

	/* run job? */
	if ((ret == 0) && cobc_flag_run) {
		ret = process_run (name);
	}
	return ret;
}

/* Set up build time stamp */
static void
set_const_cobc_build_stamp (void)
{
	int			year;
	int			day;
	char		month[32];

	memset (month, 0, sizeof(month));
	day = 0;
	year = 0;
	if (sscanf (__DATE__, "%s %d %d", month, &day, &year) == 3) {
		snprintf (cobc_buffer, (size_t)COB_MINI_MAX,
			"%s %2.2d %4.4d %s", month, day, year, __TIME__);
	} else {
		snprintf (cobc_buffer, (size_t)COB_MINI_MAX,
			"%s %s", __DATE__, __TIME__);
	}
	cb_cobc_build_stamp = (const char *)cobc_main_strdup (cobc_buffer);
}

/* Set up compiler defaults from environment/builtin */
static void
set_cobc_defaults (void)
{
	char			*p;

	cobc_cc = cobc_getenv_path ("COB_CC");
	if (cobc_cc == NULL) {
		cobc_cc = COB_CC;
	}

	cob_config_dir = cobc_getenv_path ("COB_CONFIG_DIR");
	if (cob_config_dir == NULL) {
		cob_config_dir = COB_CONFIG_DIR;
	}

	p = cobc_getenv ("COB_CFLAGS");
	if (p) {
		COBC_ADD_STR (cobc_cflags, p, NULL, NULL);
	} else {
		COBC_ADD_STR (cobc_cflags, COB_CFLAGS, NULL, NULL);
	}

	p = cobc_getenv ("COB_LDFLAGS");
	if (p) {
		COBC_ADD_STR (cobc_ldflags, p, NULL, NULL);
	} else {
		COBC_ADD_STR (cobc_ldflags, COB_LDFLAGS, NULL, NULL);
	}

	p = cobc_getenv ("COB_LIBS");
	if (p) {
		COBC_ADD_STR (cobc_libs, p, NULL, NULL);
	} else {
		COBC_ADD_STR (cobc_libs, COB_LIBS, NULL, NULL);
	}

	p = cobc_getenv ("COB_LDADD");
	if (p) {
		COBC_ADD_STR (cobc_libs, " ", p, NULL);
	}

	p = cobc_getenv ("COB_LIB_PATHS");
	if (p) {
		COBC_ADD_STR (cobc_lib_paths, p, NULL, NULL);
	} else {
		COBC_ADD_STR (cobc_lib_paths, " ", NULL, NULL);
	}

	/* Different styles for warning/error messages */
	p = cobc_getenv ("COB_MSG_FORMAT");
#if defined (_MSC_VER)
	if (p && strcasecmp(p, "GCC") == 0) {
		cb_msg_style = CB_MSG_STYLE_GCC;
	} else {
		cb_msg_style = CB_MSG_STYLE_MSC;
	}
#else
	if (p && strcasecmp(p, "MSC") == 0) {
		cb_msg_style = CB_MSG_STYLE_MSC;
	} else {
		cb_msg_style = CB_MSG_STYLE_GCC;
	}
#endif
}

/* Main function */
int
main (int argc, char **argv)
{
	struct filename		*fn;
	struct cobc_mem_struct	*mptr;
	struct cobc_mem_struct	*mptrt;
	unsigned int		iparams;
	unsigned int		local_level;
	int			status;
	int			i;

#ifdef	_WIN32
	char			*p;
#endif

#ifdef	ENABLE_NLS
	struct stat	localest;
	const char* localedir;
#endif

	file_list = NULL;
	cb_listing_file = NULL;
	cb_src_list_file = NULL;
	ppin = NULL;
	ppout = NULL;
	yyin = NULL;
	yyout = NULL;


	cob_reg_sighnd (&cobc_sig_handler);

	cb_saveargc = argc;
	cb_saveargv = argv;

	/* General buffers */
	cobc_buffer = cobc_main_malloc ((size_t)COB_LARGE_BUFF);
	cobc_buffer_size = COB_LARGE_MAX;
	basename_buffer = cobc_main_malloc ((size_t)COB_MINI_BUFF);
	basename_len = COB_MINI_MAX - 16;

	cobc_libs = cobc_main_malloc ((size_t)COB_SMALL_BUFF);
	cobc_lib_paths = cobc_main_malloc ((size_t)COB_SMALL_BUFF);
	cobc_cflags = cobc_main_malloc ((size_t)COB_MINI_BUFF);
	cobc_ldflags = cobc_main_malloc ((size_t)COB_MINI_BUFF);
	cobc_include = cobc_main_malloc ((size_t)COB_MINI_BUFF);

	cobc_libs_size = COB_SMALL_MAX;
	cobc_lib_paths_size = COB_SMALL_MAX;
	cobc_cflags_size = COB_MINI_MAX;
	cobc_include_size = COB_MINI_MAX;
	cobc_ldflags_size = COB_MINI_MAX;

	cb_source_file = NULL;
	save_temps_dir = NULL;
	base_string = NULL;
	cobc_objects_len = 0;
	cb_id = 1;
	cb_pic_id = 1;
	cb_attr_id = 1;
	cb_literal_id = 1;
	cb_field_id = 1;
#ifdef	COB_EBCDIC_MACHINE
	cb_ebcdic_sign = 1;
#else
	cb_ebcdic_sign = 0;
#endif

#ifdef	_WIN32
	/* Allows running tests under Win */
	p = getenv ("COB_UNIX_LF");
	if (p && (*p == 'Y' || *p == 'y' ||
		*p == 'O' || *p == 'o' ||
		*p == 'T' || *p == 't' ||
		*p == '1')) {
		cb_unix_lf = 1;
		(void)_setmode (_fileno (stdin), _O_BINARY);
		(void)_setmode (_fileno (stdout), _O_BINARY);
		(void)_setmode (_fileno (stderr), _O_BINARY);
	}
#endif

#ifdef	HAVE_SETLOCALE
	setlocale (LC_ALL, "");
	setlocale (LC_NUMERIC, "C");
#endif

#ifdef	ENABLE_NLS
	localedir = getenv("LOCALEDIR");
	if (localedir != NULL
	&& !stat (localedir, &localest)
	&& (S_ISDIR (localest.st_mode))) {
		bindtextdomain (PACKAGE, localedir);
	} else {
		bindtextdomain (PACKAGE, LOCALEDIR);
	}
	textdomain (PACKAGE);
#endif

	/* Initialize variables */

	set_const_cobc_build_stamp();
	set_cobc_defaults();

	output_name = NULL;

	/* Set default computed goto usage if appropriate */
#if	defined(__GNUC__) && !defined(__clang__)
	cb_flag_computed_goto = 1;
#elif	defined(__SUNPRO_C) && __SUNPRO_C >= 0x570
	cb_flag_computed_goto = 1;
#elif	defined(__xlc__) && defined(__IBMC__) && __IBMC__ >= 700
	cb_flag_computed_goto = 1;
#endif

	/* Enable default I/O exceptions */
	CB_EXCEPTION_ENABLE (COB_EC_I_O) = 1;

	/* Compiler initialization I */
#ifndef	HAVE_DESIGNATED_INITS
	cobc_init_reserved ();
#endif

	/* Process command line arguments */
	iargs = process_command_line (argc, argv);

	cb_text_column = cb_config_text_column;

	/* Check the filename */
	if (iargs == argc) {
		cobc_err_exit (_("no input files"));
	}

	/* compiler specific options for (non/very) verbose output */
#if defined(__GNUC__)
	if (verbose_output > 1) {
		COBC_ADD_STR (cobc_cflags,  " -v", NULL, NULL);
#if	!defined (__INTEL_COMPILER)
		if (verbose_output > 2) {
			COBC_ADD_STR (cobc_ldflags, " -t", NULL, NULL);
		}
#endif
	}
#elif defined(_MSC_VER)
	/* MSC stuff reliant upon verbose option */
	switch (verbose_output) {
	case 0:
	/* -v */
	case 1:
		COBC_ADD_STR (cobc_cflags, " /nologo", NULL, NULL);
		manicmd = "mt /nologo";
		manilink = "/link /manifest /nologo";
		break;
	/* -vv */
	case 2:
		manicmd = "mt";
		manilink = "/link /manifest";
		break;
	/* -vvv */
	default:
		manicmd = "mt /verbose";
		manilink = "/link /manifest /verbose";
	}
	manilink_len = strlen (manilink);
#elif defined(__WATCOMC__)
	if (verbose_output < 2) {
		COBC_ADD_STR (cobc_cflags, " -q", NULL, NULL);
	}
#endif

	/* Append default extensions */
	CB_TEXT_LIST_ADD (cb_extension_list, ".CPY");
	CB_TEXT_LIST_ADD (cb_extension_list, ".CBL");
	CB_TEXT_LIST_ADD (cb_extension_list, ".COB");
	CB_TEXT_LIST_ADD (cb_extension_list, ".cpy");
	CB_TEXT_LIST_ADD (cb_extension_list, ".cbl");
	CB_TEXT_LIST_ADD (cb_extension_list, ".cob");
	CB_TEXT_LIST_ADD (cb_extension_list, "");

	/* Process COB_COPY_DIR and COBCPY environment variables */
	process_env_copy_path (getenv ("COB_COPY_DIR"));
	process_env_copy_path (getenv ("COBCPY"));

	/* Add default COB_COPY_DIR directory */
	CB_TEXT_LIST_CHK (cb_include_list, COB_COPY_DIR);

	/* Defaults are set here */
	if (!cb_flag_syntax_only) {
		if (!wants_nonfinal) {
			if (cobc_flag_main) {
				cb_compile_level = CB_LEVEL_EXECUTABLE;
			} else if (cobc_flag_module) {
				cb_compile_level = CB_LEVEL_MODULE;
			} else if (cobc_flag_library) {
				cb_compile_level = CB_LEVEL_LIBRARY;
			} else if (cb_compile_level == 0) {
				cb_compile_level = CB_LEVEL_MODULE;
				cobc_flag_module = 1;
			}
		}
		if (wants_nonfinal && cb_compile_level != CB_LEVEL_PREPROCESS &&
		    !cobc_flag_main && !cobc_flag_module && !cobc_flag_library) {
			cobc_flag_module = 1;
		}
	} else {
		cb_compile_level = CB_LEVEL_TRANSLATE;
		cobc_flag_main = 0;
		cobc_flag_module = 0;
		cobc_flag_library = 0;
	}

	if (output_name && cb_compile_level < CB_LEVEL_LIBRARY &&
	    (argc - iargs) > 1) {
		cobc_err_exit (_("%s option invalid in this combination"), "-o");
	}

	if (cb_flag_notrunc) {
		cb_binary_truncate = 0;
		cb_pretty_display = 0;
	}

	/* Compiler initialization II */
#ifndef	HAVE_DESIGNATED_INITS
	cobc_init_typeck ();
#endif

	memset (cb_listing_header, 0, sizeof (cb_listing_header));
	/* If -P=file specified, all lists go to this file */
	if (cobc_list_file) {
		if (cb_unix_lf) {
			cb_listing_file = fopen (cobc_list_file, "wb");
		} else {
			cb_listing_file = fopen (cobc_list_file, "w");
		}
		if (!cb_listing_file) {
			cobc_terminate (cobc_list_file);
		}
	}

	/* internal complete source listing file */
#ifdef COB_INTERNAL_XREF
	if (cb_listing_xref && !cb_listing_outputfile) {
		cobc_err_exit (_("%s option requires a listing file"), "-Xref");
	}
#endif
	if (cb_listing_outputfile) {
		if (cb_unix_lf) {
			cb_src_list_file = fopen (cb_listing_outputfile, "wb");
		} else {
			cb_src_list_file = fopen (cb_listing_outputfile, "w");
		}
		if (!cb_src_list_file) {
			cobc_terminate (cb_listing_outputfile);
		}
		cb_listing_file_struct = cobc_malloc (sizeof (struct list_files));
		memset (cb_listing_file_struct, 0, sizeof (struct list_files));
	}

	if (verbose_output) {
		fputs (_("command line:"), stderr);
		putc ('\t', stderr);
		for (i = 0; i < argc; ++i) {
			fprintf (stderr, "%s ", argv[i]);
		}
		putc ('\n', stderr);
		fflush (stderr);
	}

	cobc_cc_len = strlen (cobc_cc);
	cobc_cflags_len = strlen (cobc_cflags);
	cobc_include_len = strlen (cobc_include);
	cobc_shared_opt_len = strlen (COB_SHARED_OPT);
	cobc_pic_flags_len = strlen (COB_PIC_FLAGS);
	cobc_export_dyn_len = strlen (COB_EXPORT_DYN);
	cobc_ldflags_len = strlen (cobc_ldflags);
	cobc_lib_paths_len = strlen (cobc_lib_paths);
	cobc_libs_len = strlen (cobc_libs);

	/* Process input files */
	status = 0;
	iparams = 0;
	local_level = 0;

	/* Set up file parameters, if any are missing: abort */
	while (iargs < argc) {
		fn = process_filename (argv[iargs++]);
		if (!fn) {
			cobc_clean_up (1);
			return 1;
		}
	}

	/* process all files */
	for (fn = file_list; fn; fn = fn->next) {
		current_compile_time = cob_get_current_date_and_time ();

		/* Initialize listing */
		if (cb_src_list_file) {
			set_listing_date ();
			set_standard_title ();

			cb_current_file = cb_listing_file_struct;
			cb_current_file->name = cobc_strdup (fn->source);
			cb_current_file->source_format = cb_source_format;
			force_new_page_for_next_line ();
		}

		/* Initialize general vars */
		errorcount = 0;
		cb_source_file = NULL;
		cb_source_line = 0;
		current_section = NULL;
		current_paragraph = NULL;
		current_program = NULL;
		cb_id = 1;
		cb_pic_id = 1;
		cb_attr_id = 1;
		cb_literal_id = 1;
		cb_field_id = 1;
		demangle_name = fn->demangle_source;
		memset (optimize_defs, 0, sizeof (optimize_defs));

		iparams++;
		if (iparams > 1 && cb_compile_level == CB_LEVEL_EXECUTABLE) {
			local_level = cb_compile_level;
			cb_compile_level = CB_LEVEL_ASSEMBLE;
			cobc_flag_main = 0;
		}

		if (cb_src_list_file) {
			cb_listing_page = 0;
			strcpy (cb_listing_filename, fn->source);
			set_listing_header_code ();
		}


		if (cb_compile_level >= CB_LEVEL_PREPROCESS &&
		    fn->need_preprocess) {
			/* Preprocess */
			fn->has_error = preprocess (fn);
			status |= fn->has_error;
			/* If preprocessing raised errors go on but only check syntax */
			if (fn->has_error) {
				cb_flag_syntax_only = 1;
			}
		}

		if (cobc_list_file) {
			putc ('\n', cb_listing_file);
		}

		if (cb_compile_level < CB_LEVEL_TRANSLATE) {
			if (cb_src_list_file) {
				print_program_listing ();
			}
			continue;
		}
		if (fn->need_translate) {
			/* Parse / Translate (to C code) */
			fn->has_error = process_translate (fn);
			status |= fn->has_error;
			if (cb_src_list_file) {
				print_program_listing ();
			}
			/* Free parse memory */
			for (mptr = cobc_parsemem_base; mptr; ) {
				mptrt = mptr;
				mptr = mptr->next;
				cobc_free (mptrt);
			}
			cobc_parsemem_base = NULL;
		} else {
			if (cb_src_list_file) {
				print_program_listing ();
			}
		}
		if (cb_compile_level < CB_LEVEL_COMPILE ||
		    cb_flag_syntax_only || fn->has_error) {
			continue;
		}
		if (cb_compile_level == CB_LEVEL_COMPILE) {
			/* Compile to assembler code */
			fn->has_error = process_compile (fn);
			status |= fn->has_error;
			continue;
		}

		if (cb_compile_level == CB_LEVEL_MODULE && fn->need_assemble) {
			/* Build module direct */
			fn->has_error = process_module_direct (fn);
			status |= fn->has_error;
		} else {
			/* Compile to object code */
			if (cb_compile_level >= CB_LEVEL_ASSEMBLE &&
			    fn->need_assemble) {
				fn->has_error = process_assemble (fn);
				status |= fn->has_error;
			}
			if (fn->has_error) {
				continue;
			}

			/* Build module */
			if (cb_compile_level == CB_LEVEL_MODULE) {
				fn->has_error = process_module (fn);
				status |= fn->has_error;
			}
		}
	}

	if (cobc_list_file) {
		fclose (cb_listing_file);
		cb_listing_file = NULL;
	}

	/* Clear rest of preprocess stuff */
	plex_clear_all ();

	/* Clear rest of parser stuff */
	ylex_clear_all ();

	if (local_level == CB_LEVEL_EXECUTABLE) {
		cb_compile_level = CB_LEVEL_EXECUTABLE;
	}

	if (cb_compile_level < CB_LEVEL_LIBRARY ||
	    status || cb_flag_syntax_only) {
		/* Finished */
		cobc_clean_up (status);
		return status;
	}

	/* Allocate objects buffer */
	cobc_objects_buffer = cobc_main_malloc (cobc_objects_len);

	if (file_list) {
		/* Link */
		if (cb_compile_level == CB_LEVEL_LIBRARY) {
			/* Multi-program module */
			status = process_library (file_list);
		} else {
			/* Executable */
			status = process_link (file_list);
		}
	}

	/* We have completed */
	cobc_clean_up (status);

	return status;
}
