//This is the old header file that is not generated.
//#include "./rose_token_defs.h"
// #include "./rose_token_defs_gen.h"

#ifndef ROSE_FORTRAN_TOKEN_MAPS
#define ROSE_FORTRAN_TOKEN_MAPS 1

// #include "general_defs.h"

/*
Some assumptions:
We know that keywords in FORTRAN can be separated by whitespace
Our lexer handles end if as two identifiers with lexemes "end", "if"
A later pass can coalese these two identifers into one token 
   of type ROSE_IDENTIFIER
   of lexeme "end if"
Look at the the coalese tokens phase/clean up tokens phase
*/

// using namespace ROSE_Fortran_defs;

static token_element ROSE_Fortran_keyword_map[] = 
{
      {"abstract",             SgToken::FORTRAN_ABSTRACT}, 
      {"access",               SgToken::FORTRAN_ACCESS}, 
      {"action",               SgToken::FORTRAN_ACTION}, 
      {"abstractinterface",    SgToken::FORTRAN_INTERFACE},           /*   GFORTRAN/G95 equivalent is ST_INTERFACE       */
      {"allocate",             SgToken::FORTRAN_ALLOCATE},            /*   GFORTRAN/G95 equivalent is ST_ALLOCATE        */
      {"allocatable",          SgToken::FORTRAN_ALLOCATABLE},         /*   GFORTRAN/G95 equivalent is ST_ALLOCATABLE     */
      {"assign",               SgToken::FORTRAN_ASSIGN},              /*   GFORTRAN/G95 equivalent is ST_ASSIGN          */
      {"associate",            SgToken::FORTRAN_ASSOCIATE}, 
      {"asynchronous",         SgToken::FORTRAN_ASYNCHRONOUS},        /*   GFORTRAN/G95 equivalent is ST_ASYNCHRONOUS    */
      {"backspace",            SgToken::FORTRAN_BACKSPACE},           /*   GFORTRAN/G95 equivalent is ST_BACKSPACE       */
      {"bind",                 SgToken::FORTRAN_BIND},                /*   GFORTRAN/G95 equivalent is ST_BIND            */
      {"blank",                SgToken::FORTRAN_BLANK},         
      {"blockdata",            SgToken::FORTRAN_BLOCK_DATA},          /*   GFORTRAN/G95 equivalent is ST_BLOCK_DATA      */
      {"call",                 SgToken::FORTRAN_CALL},                /*   GFORTRAN/G95 equivalent is ST_CALL            */
      {"character",            SgToken::FORTRAN_CHARACTER}, 
      {"class",                SgToken::FORTRAN_CLASS},
      {"close",                SgToken::FORTRAN_CLOSE},               /*   GFORTRAN/G95 equivalent is ST_CLOSE           */
      {"continue",             SgToken::FORTRAN_CONTINUE},            /*   GFORTRAN/G95 equivalent is ST_CONTINUE        */
      {"cycle",                SgToken::FORTRAN_CYCLE},               /*   GFORTRAN/G95 equivalent is ST_CYCLE           */
      {"case",                 SgToken::FORTRAN_CASE},                /*   GFORTRAN/G95 equivalent is ST_CASE            */
      {"common",               SgToken::FORTRAN_COMMON},              /*   GFORTRAN/G95 equivalent is ST_COMMON          */
      {"complex",              SgToken::FORTRAN_COMPLEX},
      {"contains",             SgToken::FORTRAN_CONTAINS},            /*   GFORTRAN/G95 equivalent is ST_CONTAINS        */
      {"deallocate",           SgToken::FORTRAN_DEALLOCATE},          /*   GFORTRAN/G95 equivalent is ST_DEALLOCATE      */
      {"data",                 SgToken::FORTRAN_DATA},                /*   GFORTRAN/G95 equivalent is ST_DATA            */
      {"deferred",             SgToken::FORTRAN_DEFERRED},  
      {"delim",                SgToken::FORTRAN_DELIM},  
      {"dimension",            SgToken::FORTRAN_DIMENSION},           /*   GFORTRAN/G95 equivalent is ST_DIMENSION       */
      {"do",                   SgToken::FORTRAN_DO},
      {"dt",                   SgToken::FORTRAN_DT},
      {"doubleprecision",      SgToken::FORTRAN_DOUBLEPRECISION},
      {"encoding",             SgToken::FORTRAN_ENCODING},            /*   GFORTRAN/G95 equivalent is ST_END_FILE        */
      {"endcase",              SgToken::FORTRAN_END_CASE},            /*   GFORTRAN/G95 equivalent is ST_END_FILE        */
      {"enddo",                SgToken::FORTRAN_ENDDO},               /*   GFORTRAN/G95 equivalent is ST_END_FILE        */
      {"endfile",              SgToken::FORTRAN_END_FILE},            /*   GFORTRAN/G95 equivalent is ST_END_FILE        */
      {"endenum",              SgToken::FORTRAN_END_ENUM}, 
      {"endinterface",         SgToken::FORTRAN_END_INTERFACE}, 
      {"endtype",              SgToken::FORTRAN_END_TYPE}, 
      {"err",                  SgToken::FORTRAN_ERR}, 
      {"errmsg",               SgToken::FORTRAN_ERRMSG}, 
      {"exit",                 SgToken::FORTRAN_EXIT},                /*   GFORTRAN/G95 equivalent is ST_EXIT            */
      {"else",                 SgToken::FORTRAN_ELSE},                /*   GFORTRAN/G95 equivalent is ST_ELSE            */
      {"elsewhere",            SgToken::FORTRAN_ELSEWHERE},           /*   GFORTRAN/G95 equivalent is ST_ELSEWHERE       */
      {"elseif",               SgToken::FORTRAN_ELSEIF},              /*   GFORTRAN/G95 equivalent is ST_ELSEIF          */
      {"endif",                SgToken::FORTRAN_ENDIF},
      {"entry% ",              SgToken::FORTRAN_ENTRY},               /*   GFORTRAN/G95 equivalent is ST_ENTRY           */
      {"end",                  SgToken::FORTRAN_END},             
      {"enum",                 SgToken::FORTRAN_ENUM},                /*   GFORTRAN/G95 equivalent is ST_ENUM            */
      {"enumerator",           SgToken::FORTRAN_ENUMERATOR},          /*   GFORTRAN/G95 equivalent is ST_ENUMERATOR      */
      {"equivalence",          SgToken::FORTRAN_EQUIVALENCE},         /*   GFORTRAN/G95 equivalent is ST_EQUIVALENCE     */
      {"external",             SgToken::FORTRAN_EXTERNAL},            /*   GFORTRAN/G95 equivalent is ST_EXTERNAL        */
      {"extends",              SgToken::FORTRAN_EXTENDS}, 
      {"file",                 SgToken::FORTRAN_FILE}, 
      {"final",                SgToken::FORTRAN_FINAL},               /*   GFORTRAN/G95 equivalent is ST_FORALL          */
      {"fmt",                  SgToken::FORTRAN_FMT},                 /*   GFORTRAN/G95 equivalent is ST_FORALL          */
      {"forall",               SgToken::FORTRAN_FORALL},              /*   GFORTRAN/G95 equivalent is ST_FORALL          */
      {"form",                 SgToken::FORTRAN_FORM},  
      {"formatted",            SgToken::FORTRAN_FORMATTED},  
      {"format",               SgToken::FORTRAN_FORMAT},              /*   GFORTRAN/G95 equivalent is ST_FORMAT          */
      {"flush",                SgToken::FORTRAN_FLUSH},               /*   GFORTRAN/G95 equivalent is ST_FLUSH           */
      {"function",             SgToken::FORTRAN_FUNCTION},         
      {"generic",              SgToken::FORTRAN_GENERIC},             /*   GFORTRAN/G95 equivalent is ST_GOTO            */
      {"goto",                 SgToken::FORTRAN_GOTO},                /*   GFORTRAN/G95 equivalent is ST_GOTO            */
      {"id",                   SgToken::FORTRAN_ID},                  /*   GFORTRAN/G95 equivalent is ST_IF,             */
      {"if",                   SgToken::FORTRAN_IF},                  /*   GFORTRAN/G95 equivalent is ST_IF,             */
      {"inquire",              SgToken::FORTRAN_INQUIRE},             /*   GFORTRAN/G95 equivalent is ST_INQUIRE         */
      {"integer",              SgToken::FORTRAN_INTEGER},             /*   GFORTRAN/G95 equivalent is ST_INQUIRE         */
      {"iomsg",                SgToken::FORTRAN_IOMSG}, 
      {"iostat",               SgToken::FORTRAN_IOSTAT}, 
      {"implicit",             SgToken::FORTRAN_IMPLICIT},            /*   GFORTRAN/G95 equivalent is ST_IMPLICIT        */
      {"implicitnone",         SgToken::FORTRAN_IMPLICIT_NONE},       /*   GFORTRAN/G95 equivalent is ST_IMPLICIT_NONE   */
      {"import",               SgToken::FORTRAN_IMPORT},              /*   GFORTRAN/G95 equivalent is ST_IMPORT          */
      {"interface",            SgToken::FORTRAN_INTERFACE},           /*   GFORTRAN/G95 equivalent is ST_INTERFACE       */
      {"intent",               SgToken::FORTRAN_INTENT},              /*   GFORTRAN/G95 equivalent is ST_INTENT          */
      {"intrinsic",            SgToken::FORTRAN_INTRINSIC},           /*   GFORTRAN/G95 equivalent is ST_INTRINSIC       */
      {"len",                  SgToken::FORTRAN_LEN},         
      {"logical",              SgToken::FORTRAN_LOGICAL},          
      {"kind",                 SgToken::FORTRAN_KIND}, 
      {"moduleprocedure",      SgToken::FORTRAN_MODULE_PROC},         /*   GFORTRAN/G95 equivalent is ST_E_MODULE_PROC   */
      {"module",               SgToken::FORTRAN_MODULE},              /*   GFORTRAN/G95 equivalent is ST_MODULE          */
      {"non_intrinsic",        SgToken::FORTRAN_NON_INTRINSIC},       /*   GFORTRAN/G95 equivalent is ST_NULLIFY         */
      {"non_overridable",      SgToken::FORTRAN_NON_OVERRIDABLE},     /*   GFORTRAN/G95 equivalent is ST_NULLIFY         */
      {"null",                 SgToken::FORTRAN_NULL},
      {"nullify",              SgToken::FORTRAN_NULLIFY},             /*   GFORTRAN/G95 equivalent is ST_NULLIFY         */
      {"namelist",             SgToken::FORTRAN_NAMELIST},            /*   GFORTRAN/G95 equivalent is ST_NAMELIST        */
      {"nml",                  SgToken::FORTRAN_NML},
      {"none",                 SgToken::FORTRAN_NONE},                /*   GFORTRAN/G95 equivalent is ST_NONE            */
      {"nopass",               SgToken::FORTRAN_NOPASS},  
      {"only",                 SgToken::FORTRAN_ONLY},                /*   GFORTRAN/G95 equivalent is ST_OPEN            */
      {"open",                 SgToken::FORTRAN_OPEN},                /*   GFORTRAN/G95 equivalent is ST_OPEN            */
      {"optional",             SgToken::FORTRAN_OPTIONAL},            /*   GFORTRAN/G95 equivalent is ST_OPTIONAL        */
      {"parameter",            SgToken::FORTRAN_PARAMETER},           /*   GFORTRAN/G95 equivalent is ST_PARAMETER       */
      {"pass",                 SgToken::FORTRAN_PASS}, 
      {"pause",                SgToken::FORTRAN_PAUSE},               /*   GFORTRAN/G95 equivalent is ST_PAUSE           */
      {"pointer",              SgToken::FORTRAN_POINTER},             /*   GFORTRAN/G95 equivalent is ST_POINTER         */
      {"print",                SgToken::FORTRAN_PRINT},               /*   GFORTRAN/G95 equivalent is ST_PRINT           */
      {"private",              SgToken::FORTRAN_PRIVATE},             /*   GFORTRAN/G95 equivalent is ST_PRINT           */
      {"procedure",            SgToken::FORTRAN_PROCEDURE},           /*   GFORTRAN/G95 equivalent is ST_DATA_DECL       */
      {"program",              SgToken::FORTRAN_PROGRAM},             /*   GFORTRAN/G95 equivalent is ST_PROGRAM         */
      {"protected",            SgToken::FORTRAN_PROTECTED},           /*   GFORTRAN/G95 equivalent is ST_DATA_DECL       */
      {"read",                 SgToken::FORTRAN_READ},                /*   GFORTRAN/G95 equivalent is ST_READ            */
      {"real",                 SgToken::FORTRAN_REAL}, 
      {"return",               SgToken::FORTRAN_RETURN},              /*   GFORTRAN/G95 equivalent is ST_RETURN          */
      {"rewind",               SgToken::FORTRAN_REWIND},              /*   GFORTRAN/G95 equivalent is ST_REWIND          */
      {"round",                SgToken::FORTRAN_ROUND},  
      {"selectcase",           SgToken::FORTRAN_SELECTCASE}, 
      {"selecttype",           SgToken::FORTRAN_SELECTTYPE}, 
      {"sequence",             SgToken::FORTRAN_SEQUENCE},            /*   GFORTRAN/G95 equivalent is ST_SEQUENCE        */
      {"save",                 SgToken::FORTRAN_SAVE},                /*   GFORTRAN/G95 equivalent is ST_SAVE            */
      {"sign",                 SgToken::FORTRAN_SIGN}, 
      {"size",                 SgToken::FORTRAN_SIZE}, 
      {"source",               SgToken::FORTRAN_SOURCE},              /*   GFORTRAN/G95 equivalent is ST_SAVE            */
      {"stat",                 SgToken::FORTRAN_STAT}, 
      {"stop",                 SgToken::FORTRAN_STOP},                /*   GFORTRAN/G95 equivalent is ST_STOP            */
      {"subroutine",           SgToken::FORTRAN_SUBROUTINE},
      {"target",               SgToken::FORTRAN_TARGET},              /*   GFORTRAN/G95 equivalent is ST_TARGET          */
      {"then",                 SgToken::FORTRAN_THEN}, 
      {"type",                 SgToken::FORTRAN_DERIVED_DECL},        /*   GFORTRAN/G95 equivalent is ST_DERIVED_DECL    */
      {"typeis",               SgToken::FORTRAN_TYPEIS}, 
      {"unformatted",          SgToken::FORTRAN_UNFORMATTED}, 
      {"unit",                 SgToken::FORTRAN_UNIT}, 
      {"use",                  SgToken::FORTRAN_USE},                 /*   GFORTRAN/G95 equivalent is ST_USE             */
      {"value",                SgToken::FORTRAN_VALUE},               /*   GFORTRAN/G95 equivalent is ST_VALUE           */
      {"volatile",             SgToken::FORTRAN_VOLATILE},            /*   GFORTRAN/G95 equivalent is ST_VOLATILE        */
      {"wait",                 SgToken::FORTRAN_WAIT},                /*   GFORTRAN/G95 equivalent is ST_WAIT            */
      {"where",                SgToken::FORTRAN_WHERE},               /*   GFORTRAN/G95 equivalent is ST_WHERE,          */
      {"write",                SgToken::FORTRAN_WRITE}                /*   GFORTRAN/G95 equivalent is ST_WRITE           */
};


#endif
