// -*- mode: C++; truncate-lines: true; -*-
#include "RoseEnums.h"

/* This is needed for the SgBitVector translation
 *
 * Ideally, the contents of most of this file 
 * would be generated from the ROSE sources! 
 *
 * see "rose/src/ROSETTA/Grammar/Support.code"
 *
 * 2009 Adrian Prantl <adrian@complang.tuwien.ac.at>
 */


static const char* e_function_modifier[] = {
 "unknown", // = 0, /*!< error value */
 "default", // = 1, /*!< default value */
 "inline",  // = 2, /*!< inlined function */
 "virtual", // = 3, /*!< virtual function */
 "pure_virtual", // = 4, /*!< pure virtual function */
 "explicit",// = 5, /*!< explicit function (avoids type resolution through auto type promotion of function arguments) */
 "bind",    // = 6, /*!< Fortran bind attribute for functions (see also type modifier) */
 "pure",    // = 7, /*!< Fortran pure attribute for functions (see also type modifier) */
 "elemental", // = 8, /*!< Fortran elemental attribute for functions (see also type modifier) */
 "recursive", // = 9, /*!< Fortran recursive attribute for functions (see also type modifier) */
 "gnu_attribute__constructor__", // = 10,/*!< GNU specific attribute for (GNU extension) */
 "gnu_attribute__destructor__",  // = 11,/*!< GNU specific attribute for (GNU extension) */
 "gnu_attribute__pure__", // = 12,/*!< GNU specific attribute for (GNU extension) */
 "gnu_attribute__weak__", // = 13,/*!< GNU specific attribute for (GNU extension) */
 "gnu_attribute__unused__", // = 14,/*!< GNU specific attribute for (GNU extension) */
 "gnu_attribute__used__", // = 15,/*!< GNU specific attribute for (GNU extension) */
 "gnu_attribute__deprecated__", // = 16,/*!< GNU specific attribute for (GNU extension) */
 "gnu_attribute__malloc__", // = 17,/*!< GNU specific attribute for (GNU extension) */
 "gnu_attribute__naked__",  // = 18,/*!< GNU specific attribute for (GNU extension) */
 "gnu_attribute__no_instrument_function__", // = 19,/*!< GNU specific attribute for (GNU extension) */
 "gnu_attribute__no_check_memory_usage__",   // = 20,/*!< GNU specific attribute for (GNU extension) */
 "gnu_attribute__noinline__", //              = 21,/*!< GNU specific attribute for (GNU extension) */
 "gnu_attribute__always_inline__", //         = 22,/*!< GNU specific attribute for (GNU extension) */
 "gnu_attribute__nothrow__", //               = 23,/*!< GNU specific attribute for (GNU extension) */
 "gnu_attribute__weakref__", //               = 24,/*!< GNU specific attribute for (GNU extension) */
 //"last_modifier"  /*!< last value (upper bound on range of values, used in error checking) */
};
static const char* e_special_function_modifier[] = {
  "unknown", // = 0,         /*!< error value */
  "default", // = 1,         /*!< Not a special function (default) */
  "none", // ="default, /*!< Not a special function (default) */
  "constructor", // = 2,         /*!< A constructor */
  "destructor", // = 3,         /*!< A destructor */
  "conversion", // = 4,         /*!< A conversion operator function */
  "operator" // = 5,         /*!< Any other operator function */
  //"last_modifier"             /*!< last value (upper bound on range of values, used in error checking) */
};
static const char* e_type_modifier[] = {
  "unknown",  // = 0, /*!< Unknown value (error) */
  "default",  // = 1, /*!< Unknown value (default) */
  "restrict", // = 2, /*!< Restrict qualifier (for C/C++) */
  // Fortran specific modifiers (in Fortran these are called attribute specifiers)
  // Note that public and private access attributes are mapped to the C/C++ access modifiers in SgDeclarationModifier
  "public_access",  // = 3, /*!< public qualifier (for Fortran 90) */
  "private_access", // = 4, /*!< private qualifier (for Fortran 90) */
  "allocatable",    // = 3, /*!< allocatable attribute specifier (for Fortran 90) */
  "asynchronous",   // = 4, /*!< asynchronous attribute specifier (for Fortran 2003) */
  "bind", // = 5, /*!< bind attribute specifier (for Fortran 2003, this is closely related to the SgLinkageModifier) */
  "data", // = 6, /*!< data attribute specifier (for Fortran 77) */
  "dimension", // = 7, /*!< dimension attribute specifier (for Fortran 77) */
  // This should map to the C/C++ extern modifier in SgStorageModifier
  //"external       ", // = x, /*!< external attribute specifier (for Fortran 77, this is less related to the SgLinkageModifier than the SgStorageModifier) */
  "intent_in",    // = 8, /*!< intent(in) attribute specifier (for Fortran 90) */
  "intent_out",   // = 9, /*!< intent(out) attribute specifier (for Fortran 90) */
  "intent_inout", // = 10,/*!< intent(inout) attribute specifier (for Fortran 90) */
  "intrinsic",    // = 11,/*!< intrinsic attribute specifier (for Fortran 90) */
  "optional",     // = 12,/*!< optional attribute specifier (for Fortran 90) */
  // This should map to const in the SgConstVolatileModifier
  //"parameter      ", // = xx,/*!< parameter attribute specifier (for Fortran 77) */

  // This should be mapped to the SgPointerType in C/C++
  //"pointer        ", // = xx,/*!< pointer attribute specifier (for Fortran 90) */
  // Note that protected access attribute is mapped to the C/C++ protected access modifiers in SgDeclarationModifier
  //"protected      ", // = xx,/*!< protected attribute specifier (for Fortran 2003) */
  "save",   // = 13,/*!< save attribute specifier (for Fortran 77) */
  "target", // = 14,/*!< target attribute specifier (for Fortran 90) */
  "value",  // = 15,/*!< value attribute specifier (for Fortran 2003) */
  // This should map to volatile in the SgConstVolatileModifier
  //"volatile       ", // = xx,/*!< volatile attribute specifier (for Fortran 2003) */

  // DQ (12/4/2007): Added support for GNU specific attributes
  "gnu_attribute__unused__",            // = 16,/*!< GNU specific attribute for (GNU extension) */
  "gnu_attribute__packed__",            // = 17,/*!< GNU specific attribute for (GNU extension) */
  "gnu_attribute__deprecated__",        // = 18,/*!< GNU specific attribute for (GNU extension) */
  "gnu_attribute__transparent_union__", // = 19,/*!< GNU specific attribute for (GNU extension) */
  "gnu_attribute__noreturn__",          // = 20,/*!< GNU specific attribute for function type (GNU extension) */
  "gnu_attribute__const__",             // = 21,/*!< GNU specific attribute for function type (GNU extension) */
  "gnu_attribute__cdecl__",             // = 22,/*!< GNU specific attribute for function type (GNU extension) */
  "gnu_attribute__stdcall__",           // = 23,/*!< GNU specific attribute for function type (GNU extension) */
 "gnu_attribute__warn_unused_result__", //= 24,/*!< GNU specific attribute for function type (GNU extension) */
 "gnu_attribute__nonnull__", //           = 25,/*!< GNU specific attribute for function type (GNU extension) */
 "gnu_attribute__sentinel__", //          = 26,/*!< GNU specific attribute for function type (GNU extension) */
  //"last_modifier" /*!< last value (upper bound on range of values, used in error checking) */
};

static const char* e_elaborated_type_modifier[] = {
  // Only one value can be specified ???
  "unknown", // = 0, /*!< error value */
  "default", //  = 1, /*!< default value (default) */
  "class", //    = 2, /*!< class type */
  "struct", //   = 3, /*!< struct type */
  "union", //    = 4, /*!< union type */
  "enum", //     = 5, /*!< enum type */
  "typename" // = 6, /*!< typename type */
  //e_last_modifier /*!< last value (upper bound on range of values, used in error checking) */
};


static const char* e_declaration_modifier[] = {
  "unknown", // = 0,  /*!< Unknown value (error) */
  "default", // = 1,  /*!< Default value */
  "friend",  // = 2,  /*!< Friend qualifier */
  "typedef", // = 3,  /*!< Typedef qualifier (not clear if this is used, I don't think typedef is a modifier) */
  "export",  // = 4,  /*!< Export qualifier */
  "throw",   // = 5,  /*!< Exception handling support (throw) */ 
  "bind",    // = 6,  /*!< Fortran bind attribute for declarations (functions, variable, and classes) */
  "final"    // = 7,  /*!< PHP support for specification to not be extended (should be useful for Java, if required in the future) */
  //"last_modifier" /*!< last modifier value (upper bound on range of values, used in error checking) */
};

static const char* e_storage_modifier[] = {
  "unknown",      // = 0,  /*!< error value */
  "default",      // = 1,  /*!< default value */
  "extern",       // = 2,  /*!< extern storage modifier */
  "static",       // = 3,  /*!< static storage modifier */
  "auto",         // = 4,  /*!< auto storage value */
  "unspecified",  // = 5,  /*!< previous (old) default value, not included in C++ grammar definition of storage modifier */
  "register",     // = 6,  /*!< register storage modifier */
  "mutable",      // = 7,  /*!< mutable storage modifier */
  "typedef",      // = 8,  /*!< not used, not included in C++ grammar definition of storage modifier */
  "asm",          // = 9,  /*!< not used, not included in C++ grammar definition of storage modifier */

  // included for later FORTRAN support
  "local",        // = 10, /*!< Auto or static at back end's preference */
  "common",       // = 11, /*!< A COMMON block */
  "associated",   // = 12, /*!< Variable is part of an association */
  "intrinsic",    // = 13, /*!< Intrinsic function or subroutine */
  "pointer_based" // = 14, /*!< Pointee of a POINTER definition */
  
  //e_last_modifier  /*!< last value (upper bound on range of values, used in error checking) */
};

static const char* e_access_modifier[] = {
  "unknown",  // = 0,        /*!< error value */
  "private",  // = 1,        /*!< private access (local to class members) */
  "protected",// = 2,        /*!< protected access (local to class members and membrs of derived classes) */
  "public",   // = 3,        /*!< public access (access within enclosing namespace) */
  "default",  // = e_public, /*!< default value (public access) */
  "undefined" // = 4,        /*!< fortran default value */
    //last_modifier         /*!< last value (upper bound on range of values, used in error checking) */
};

static const char* e_upc_access_modifier[] = {
  // Bit values can support multiple values in a single enum type
  "unknown",       /*!< error value */
  "default",       /*!< default value */
  // DQ and Liao (6/13/2008): "shared" can be used with "strict" and "relaxed" so it must be handled separately.
  // e_upc_shared,    /*!< UPC shared */
  "upc_strict",    /*!< UPC strict */
  "upc_relaxed",   /*!< UPC relaxed */
  //e_last_modifier  /*!< last value (upper bound on range of values, used in error checking) */
};

static const char* e_cv_modifier[] = {
  "unknown",      /*!< Unknown value (error value) */
  "default",      /*!< Default value (default) */
  "const",        /*!< Const qualifier */
  "volatile"      /*!< Volatile qualifier */
  //e_last_modifier /*!< last value (unused, serves as upper bound on values) */
};

static const char* e_class_type[] = { 
  "class", 
  "struct", 
  "union", 
  "template_parameter" 
};

static const char* e_throw_kind[] = { 
  "unknown_throw",
  "throw_expression", 
  "rethrow" 
};

static const char* e_cast_type[] = { 
  "unknown", 
  "default", 
  "C_style_cast", 
  "const_cast", 
  "static_cast", 
  "dynamic_cast", 
  "reinterpret_cast"
};

static const char* e_static_flag[] = { 
  "default", 
  "static"
};

static const char* e_ellipses_flag[] = { 
  "default", 
  "ellipses"
};

static const char* e_RelativePositionType[] = {
  "defaultValue", // = 0, // let the zero value be an error value
  "undef", //        = 1, // Position of the directive is only going to be defined
  // when the preprocessing object is copied into the AST,
  // it remains undefined before that
  "before", //       = 2, // Directive goes before the correponding code segment
  "after", //        = 3, // Directive goes after the correponding code segment
  "inside", //       = 4, // Directive goes inside the correponding code segment (as in between "{" and "}" of an empty basic block)
  "replace", //       = 5, // Support for replacing the IR node in the unparsing of any associated subtree
  "before_syntax", // = 6, // We still have to specify the syntax
  "after_syntax"  //  = 7  // We still have to specify the syntax
};

       // Enum type to help classify the type for string that has been saved.
       // This helps in the unparsing to make sure that line feeds are introduced properly.
static const char* e_DirectiveType[] = {
  "cpreprocessorUnknownDeclaration",
  "c_StyleComment",
  "cplusplusStyleComment",
  "fortranStyleComment",
  "F90StyleComment",
  "cpreprocessorBlankLine",

  "cpreprocessorIncludeDeclaration",
  "cpreprocessorIncludeNextDeclaration",
  "cpreprocessorDefineDeclaration",
  "cpreprocessorUndefDeclaration",
  "cpreprocessorIfdefDeclaration",
  "cpreprocessorIfndefDeclaration",
  "cpreprocessorIfDeclaration",
  "cpreprocessorDeadIfDeclaration",
  "cpreprocessorElseDeclaration",
  "cpreprocessorElifDeclaration",
  "cpreprocessorEndifDeclaration",
  "cpreprocessorLineDeclaration",
  "cpreprocessorErrorDeclaration",

  // DQ (10/19/2005): Added CPP warning directive
  "cpreprocessorWarningDeclaration",
  "cpreprocessorEmptyDeclaration",

  // AS (11/18/05): Added macro support
  "cSkippedToken",
  "cMacroCall",

  // AS & LIAO (8/12/2008): A PreprocessingInfo that is a 
  // hand made MacroCall that will expand into a valid statement.
  "cMacroCallStatement",

  // A line replacement will replace a sub-tree in the AST
  // after a node with position (filename",line)
  "lineReplacement",
  "clinkageSpecificationStart",
  "clinkageSpecificationEnd",
  // DQ (11/17/2008): Added support for #ident
  "cpreprocessorIdentDeclaration",

  // DQ (11/17/2008): This handles the case CPP declarations (called "linemarkers")
  // (see Google for more details) such as: "# 1 "test2008_05.F90"", "# 1 "<built-in>"", 
  // "# 1 "<command line>"" "# 1 "test2008_05.F90""
  // The first token is the line number,
  // the second token is the filename (or string),
  // the optional tokens (zero or more) are flags:
  //   '1' indicates the start of a new file.
  //   '2' indicates returning to a file (having included another file).
  //   '3' indicates that the following text comes from a system header file, so certain warnings should be supressed. 
  //   '4' indicates that the following text should be treated as being wrapped in an implicit 'extern "C"' block
  "cpreprocessorCompilerGeneratedLinemarker"
};

/////////////////////////////////////////////////////////////////////////

//#include <iostream>
//using namespace std;

#define INIT(SRC,MAP,VEC) \
  for (i = 0; i < sizeof(SRC)/sizeof(char*); ++i) { \
    MAP[SRC[i]] = i; \
    VEC.push_back(SRC[i]); \
  }


RoseEnums::RoseEnums() {
  size_t i;
  INIT(e_function_modifier, function_modifier, function_modifiers)
  INIT(e_special_function_modifier, special_function_modifier, special_function_modifiers)
  INIT(e_type_modifier, type_modifier, type_modifiers)
  INIT(e_elaborated_type_modifier, elaborated_type_modifier, elaborated_type_modifiers)
  INIT(e_declaration_modifier, declaration_modifier, declaration_modifiers)
  INIT(e_access_modifier, access_modifier, access_modifiers)
  INIT(e_upc_access_modifier, upc_access_modifier, upc_access_modifiers)
  INIT(e_storage_modifier, storage_modifier, storage_modifiers)
  INIT(e_cv_modifier, cv_modifier, cv_modifiers)

  INIT(e_class_type, class_type, class_types)
  INIT(e_throw_kind, throw_kind, throw_kinds)
  INIT(e_cast_type, cast_type, cast_types)

  INIT(e_static_flag, static_flag, static_flags)
  INIT(e_ellipses_flag, ellipses_flag, ellipses_flags)

  INIT(e_RelativePositionType, RelativePositionType, RelativePositionTypes)
  INIT(e_DirectiveType, DirectiveType, DirectiveTypes)
}
