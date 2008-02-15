#!/usr/bin/perl -w
###############################################
##HERE COMES ALL THE TOKEN IDENTIFIERS
###############################################

$keyword_initial_mask = 0;
$keyword_c_cxx_initial_mask= 500;
$operator_fortran_initial_mask = 10000;
$operator_c_cxx_initial_mask = 50000;
$misc_initial_mask = 100000;

###############################################
#Print the PROLOGUE
###############################################
###############################################
#add stuff to create namespaces for FORTRAN and C/CXX
###############################################
print "
//Ramakrishna Upadrasta
//**************************************************
//CAUTION: This is a generated file!!!
//**************************************************
//
#include \"general_defs.h\"
#ifndef ROSE_FORTRAN_TOKEN_DEFS
#define ROSE_FORTRAN_TOKEN_DEFS 1
//
//
namespace ROSE_Fortran_defs
{
enum ROSE_Fortran_keywords
{
";

#Initialization of the Keyword data structures
@fortran_keyword_token_ids = 
(
      "FORTRAN_ABSTRACT",
      "FORTRAN_ACCESS",  
      "FORTRAN_ACTION",  
      "FORTRAN_ALLOCATE",        
      "FORTRAN_ALLOCATABLE",     
      "FORTRAN_ASSIGN",          
      "FORTRAN_ASSOCIATE",  
      "FORTRAN_ASYNCHRONOUS",    
      "FORTRAN_BACKSPACE",       
      "FORTRAN_BIND",            
      "FORTRAN_BLANK",          
      "FORTRAN_BLOCK_DATA",      
      "FORTRAN_CALL",            
      "FORTRAN_CHARACTER",  
      "FORTRAN_CLASS", 
      "FORTRAN_CLOSE",           
      "FORTRAN_CONTINUE",        
      "FORTRAN_CYCLE",           
      "FORTRAN_CASE",            
      "FORTRAN_COMMON",          
      "FORTRAN_COMPLEX", 
      "FORTRAN_CONTAINS",        
      "FORTRAN_DEALLOCATE",      
      "FORTRAN_DATA",            
      "FORTRAN_DEFERRED",   
      "FORTRAN_DELIM",   
      "FORTRAN_DIMENSION",       
      "FORTRAN_DO", 
      "FORTRAN_DT", 
      "FORTRAN_DOUBLEPRECISION", 
      "FORTRAN_ENCODING",        
      "FORTRAN_END_CASE",        
      "FORTRAN_ENDDO",           
      "FORTRAN_END_FILE",        
      "FORTRAN_END_ENUM",  
      "FORTRAN_END_INTERFACE",  
      "FORTRAN_END_TYPE",  
      "FORTRAN_ERR",  
      "FORTRAN_ERRMSG",  
      "FORTRAN_EXIT",            
      "FORTRAN_ELSE",            
      "FORTRAN_ELSEWHERE",       
      "FORTRAN_ELSEIF",          
      "FORTRAN_ENDIF", 
      "FORTRAN_ENTRY",           
      "FORTRAN_END", 
      "FORTRAN_ENUM",            
      "FORTRAN_ENUMERATOR",      
      "FORTRAN_EQUIVALENCE",     
      "FORTRAN_EXTERNAL",        
      "FORTRAN_EXTENDS",  
      "FORTRAN_FILE",  
      "FORTRAN_FINAL",           
      "FORTRAN_FMT",             
      "FORTRAN_FORALL",          
      "FORTRAN_FORM",   
      "FORTRAN_FORMATTED",   
      "FORTRAN_FORMAT",          
      "FORTRAN_FLUSH",           
      "FORTRAN_FUNCTION",        
      "FORTRAN_GENERIC",         
      "FORTRAN_GOTO",            
      "FORTRAN_ID",              
      "FORTRAN_IF",              
      "FORTRAN_INQUIRE",         
      "FORTRAN_INTEGER",         
      "FORTRAN_IOMSG",  
      "FORTRAN_IOSTAT",  
      "FORTRAN_IMPLICIT",        
      "FORTRAN_IMPLICIT_NONE",   
      "FORTRAN_IMPORT",          
      "FORTRAN_INTERFACE",       
      "FORTRAN_INTENT",          
      "FORTRAN_INTRINSIC",       
      "FORTRAN_LEN",          
      "FORTRAN_LOGICAL",         
      "FORTRAN_KIND",  
      "FORTRAN_MODULE_PROC",     
      "FORTRAN_MODULE",          
      "FORTRAN_NON_INTRINSIC",   
      "FORTRAN_NON_OVERRIDABLE", 
      "FORTRAN_NULL", 
      "FORTRAN_NULLIFY",         
      "FORTRAN_NAMELIST",        
      "FORTRAN_NML", 
      "FORTRAN_NONE",            
      "FORTRAN_NOPASS",   
      "FORTRAN_ONLY",            
      "FORTRAN_OPEN",            
      "FORTRAN_OPTIONAL",        
      "FORTRAN_PARAMETER",       
      "FORTRAN_PASS",  
      "FORTRAN_PAUSE",           
      "FORTRAN_POINTER",         
      "FORTRAN_PRINT",           
      "FORTRAN_PRIVATE",         
      "FORTRAN_PROCEDURE",       
      "FORTRAN_PROGRAM",         
      "FORTRAN_PROTECTED",       
      "FORTRAN_READ",            
      "FORTRAN_REAL",  
      "FORTRAN_RETURN",          
      "FORTRAN_REWIND",          
      "FORTRAN_ROUND",   
      "FORTRAN_SELECTCASE",  
      "FORTRAN_SELECTTYPE",  
      "FORTRAN_SEQUENCE",        
      "FORTRAN_SAVE",            
      "FORTRAN_SIGN",  
      "FORTRAN_SIZE",  
      "FORTRAN_SOURCE",          
      "FORTRAN_STAT",  
      "FORTRAN_STOP",            
      "FORTRAN_SUBROUTINE", 
      "FORTRAN_TARGET",          
      "FORTRAN_THEN",  
      "FORTRAN_DERIVED_DECL",    
      "FORTRAN_TYPEIS",  
      "FORTRAN_UNFORMATTED",  
      "FORTRAN_UNIT",  
      "FORTRAN_USE",             
      "FORTRAN_VALUE",           
      "FORTRAN_VOLATILE",        
      "FORTRAN_WAIT",            
      "FORTRAN_WHERE", 
      "FORTRAN_WRITE"
);

#Print the FORTRAN KEYWORDS, with their offset
$len_key = @fortran_keyword_token_ids;
#print $len_key;
for($i = 0; $i < $len_key-1; $i++)
{ 
   print("      $fortran_keyword_token_ids[$i] = $i + $keyword_initial_mask,");
   print("\n");
}
print("      $fortran_keyword_token_ids[$i] = $i + $keyword_initial_mask");
print("\n};\n");

###########################
#Rama: 03/30
#HERE gegins the support to generate the C CXX keywords 
#They are however commented, as there are conflicts of the names.
#A better design is probably needed and I have to talk to Dan about it.
#So until then, I have decided to if 0 the code
###########################
#if(0)
#{
print("
//namespace ROSE_C_CXX_defs
//{
");

print("\n");
print("enum ROSE_C_CXX_keywords\n{\n");

#Initialization of the Keyword data structures
@C_CXX_keyword_token_ids = 
(
      "C_CXX_ASM", 
      "C_CXX_AUTO", 
      "C_CXX_BOOL", 
      "C_CXX_BREAK",
      "C_CXX_CASE", 
      "C_CXX_CATCH",
      "C_CXX_CHAR", 
      "C_CXX_CLASS",
      "C_CXX_CONST",
      "C_CXX_CONSTCAST",
      "C_CXX_CONTINUE", 
      "C_CXX_DEFAULT", 
      "C_CXX_DEFINED", 
      "C_CXX_DELETE",
      "C_CXX_DO", 
      "C_CXX_DOUBLE", 
      "C_CXX_DYNAMICCAST",
      "C_CXX_ELSE", 
      "C_CXX_ENUM", 
      "C_CXX_EXPLICIT",
      "C_CXX_EXPORT", 
      "C_CXX_EXTERN",
      "C_CXX_FALSE", 
      "C_CXX_FLOAT",
      "C_CXX_FOR", 
      "C_CXX_FRIEND",
      "C_CXX_GOTO",
      "C_CXX_IF", 
      "C_CXX_INLINE",
      "C_CXX_INT", 
      "C_CXX_LONG", 
      "C_CXX_MUTABLE",
      "C_CXX_NAMESPACE",
      "C_CXX_NEW", 
      "C_CXX_OPERATOR",
      "C_CXX_PRIVATE",
      "C_CXX_PROTECTED",
      "C_CXX_PUBLIC",
      "C_CXX_REGISTER", 
      "C_CXX_REINTERPRETCAST", 
      "C_CXX_RETURN", 
      "C_CXX_SHORT", 
      "C_CXX_SIGNED", 
      "C_CXX_SIZEOF", 
      "C_CXX_STATIC", 
      "C_CXX_STATICCAST", 
      "C_CXX_STRUCT",
      "C_CXX_SWITCH",
      "C_CXX_TEMPLATE",
      "C_CXX_THIS", 
      "C_CXX_THROW", 
      "C_CXX_TRY", 
      "C_CXX_TRUE",
      "C_CXX_TYPEDEF", 
      "C_CXX_TYPEID", 
      "C_CXX_TYPENAME",
      "C_CXX_UNION", 
      "C_CXX_UNSIGNED",
      "C_CXX_USING", 
      "C_CXX_VIRTUAL",
      "C_CXX_VOID", 
      "C_CXX_VOLATILE", 
      "C_CXX_WCHART", 
      "C_CXX_WHILE" 
);



#Print the FORTRAN KEYWORDS, with their offset
$len_C_CXX_key = @C_CXX_keyword_token_ids;
#print $len_key;
for($i = 0; $i < $len_C_CXX_key-1; $i++)
{ 
   print("      $C_CXX_keyword_token_ids[$i] = $i + $keyword_c_cxx_initial_mask,");
   print("\n");
}
print("      $C_CXX_keyword_token_ids[$i] = $i + $keyword_c_cxx_initial_mask");
print("\n};\n");

###########################
#HERE ends the C CXX keyword if 0
###########################
#};


#Initialization FORTRAN of Operator data structures

print("\n");
print("enum ROSE_Fortran_Operators\n{\n");

@fortran_operator_token_ids = 
(
     "FORTRAN_INTRINSIC_PLUS",   
     "FORTRAN_INTRINSIC_MINUS", 
     "FORTRAN_INTRINSIC_POWER", 
     "FORTRAN_INTRINSIC_CONCAT", 
     "FORTRAN_INTRINSIC_TIMES", 
     "FORTRAN_INTRINSIC_DIVIDE", 
     "FORTRAN_INTRINSIC_AND",   
     "FORTRAN_INTRINSIC_OR",    
     "FORTRAN_INTRINSIC_EQV",   
     "FORTRAN_INTRINSIC_NEQV",  
     "FORTRAN_INTRINSIC_EQ",   
     "FORTRAN_INTRINSIC_NE",   
     "FORTRAN_INTRINSIC_GE",    
     "FORTRAN_INTRINSIC_LE",    
     "FORTRAN_INTRINSIC_LT",    
     "FORTRAN_INTRINSIC_GT",    
     "FORTRAN_INTRINSIC_NOT",   
     "FORTRAN_INTRINSIC_OLDEQ", 
     "FORTRAN_INTRINSIC_OLDNE", 
     "FORTRAN_INTRINSIC_OLDGE", 
     "FORTRAN_INTRINSIC_OLDLE", 
     "FORTRAN_INTRINSIC_OLDLT", 
     "FORTRAN_INTRINSIC_OLDGT", 
);


#Print the OPERATORS, with their offset
$len_op = @fortran_operator_token_ids;
#print $len_op;
for($i = 0; $i < $len_op - 1; $i++)
{ 
   print("      $fortran_operator_token_ids[$i] = $i + $operator_fortran_initial_mask,");
   print("\n");
}
print("      $fortran_operator_token_ids[$i] = $i + $operator_fortran_initial_mask");
print("\n};\n");



print("\n");
print("enum ROSE_C_CXX_operators\n{\n");

@c_cxx_operator_token_ids = 
(
      "C_CXX_AND",
      "C_CXX_ANDAND",
      "C_CXX_ASSIGN",
      "C_CXX_ANDASSIGN",
      "C_CXX_OR",
      "C_CXX_ORASSIGN",
      "C_CXX_XOR",
      "C_CXX_XORASSIGN",
      "C_CXX_COMMA",
      "C_CXX_COLON",
      "C_CXX_DIVIDE",
      "C_CXX_DIVIDEASSIGN",
      "C_CXX_DOT",
      "C_CXX_DOTSTAR",
      "C_CXX_ELLIPSIS",
      "C_CXX_EQUAL",
      "C_CXX_GREATER",
      "C_CXX_GREATEREQUAL",
      "C_CXX_LEFTBRACE",
      "C_CXX_LESS",
      "C_CXX_LESSEQUAL",
      "C_CXX_LEFTPAREN",
      "C_CXX_LEFTBRACKET",
      "C_CXX_MINUS",
      "C_CXX_MINUSASSIGN",
      "C_CXX_MINUSMINUS",
      "C_CXX_PERCENT",
      "C_CXX_PERCENTASSIGN",
      "C_CXX_NOT",
      "C_CXX_NOTEQUAL",
      "C_CXX_OROR",
      "C_CXX_PLUS",
      "C_CXX_PLUSASSIGN",
      "C_CXX_PLUSPLUS",
      "C_CXX_ARROW",
      "C_CXX_ARROWSTAR",
      "C_CXX_QUESTION_MARK",
      "C_CXX_RIGHTBRACE",
      "C_CXX_RIGHTPAREN",
      "C_CXX_RIGHTBRACKET",
      "C_CXX_COLON_COLON",
      "C_CXX_SEMICOLON",
      "C_CXX_SHIFTLEFT",
      "C_CXX_SHIFTLEFTASSIGN",
      "C_CXX_SHIFTRIGHT",
      "C_CXX_SHIFTRIGHTASSIGN",
      "C_CXX_STAR",
      "C_CXX_COMPL",
      "C_CXX_STARASSIGN",
      "C_CXX_POUND_POUND",
      "C_CXX_POUND",
      "C_CXX_AND_ALT",
      "C_CXX_ANDASSIGN_ALT",
      "C_CXX_OR_ALT",
      "C_CXX_ORASSIGN_ALT",
      "C_CXX_XOR_ALT",
      "C_CXX_XORASSIGN_ALT",
      "C_CXX_LEFTBRACE_ALT",
      "C_CXX_LEFTBRACKET_ALT",
      "C_CXX_NOT_ALT",
      "C_CXX_NOTEQUAL_ALT",
      "C_CXX_RIGHTBRACE_ALT",
      "C_CXX_RIGHTBRACKET_ALT",
      "C_CXX_COMPL_ALT",
      "C_CXX_POUND_POUND_ALT",
      "C_CXX_POUND_ALT",
      "C_CXX_OR_TRIGRAPH",
      "C_CXX_XOR_TRIGRAPH",
      "C_CXX_LEFTBRACE_TRIGRAPH",
      "C_CXX_LEFTBRACKET_TRIGRAPH",
      "C_CXX_RIGHTBRACE_TRIGRAPH",
      "C_CXX_RIGHTBRACKET_TRIGRAPH",
      "C_CXX_COMPL_TRIGRAPH",
      "C_CXX_POUND_POUND_TRIGRAPH",
      "C_CXX_POUND_TRIGRAPH"
);

#Print the OPERATORS, with their offset
$len_op = @c_cxx_operator_token_ids;
#print $len_op;
for($i = 0; $i < $len_op - 1; $i++)
{ 
   print("      $c_cxx_operator_token_ids[$i] = $i + $operator_c_cxx_initial_mask,");
   print("\n");
}
print("      $c_cxx_operator_token_ids[$i] = $i + $operator_c_cxx_initial_mask");
print("\n};\n");





print("\n");
print("enum ROSE_Fortran_Additional_Info\n{\n");

@fortran_misc_token_ids = 
(

    "FORTRAN_COMMENTS",
    "FORTRAN_STRING_LITERALS",
    "FORTRAN_IDENTIFIER",
    "FORTRAN_UNIDENTIFIED_TOKEN",
    "FORTRAN_ERROR"
);

#Print the Misc. stuff, with their offset
$len_misc = @fortran_misc_token_ids;
#print $len_misc;
for($i = 0; $i < $len_misc - 1; $i++)
{ 
   print("      $fortran_misc_token_ids[$i] = $i + $misc_initial_mask,");
   print("\n");
}
print("      $fortran_misc_token_ids[$i] = $i + $misc_initial_mask
};
");

print "

} // end of namespace ROSE_Fortran_defs
#endif
";

###############################################
#print the token values in hex or
