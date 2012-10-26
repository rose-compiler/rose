

%option noyywrap
%option prefix="Rose_C_Cxx_"
%option outfile="lex.yy.c"
%{
/* 
    preproc.lex: This lexer currently recognizes C style comments, C++ style 
    comments, preprocessor directives, and c/c++ linkage specifications.  
    line/column/text information for each recognized directive is stored in 
    ROSEAttributesList.  This information is used by the unparser.
*/

/*
-------------------------------------------------------------------------------------
Originally written and modified by Markus Schordan and Dan Quinlan.
(see ChangeLog).

08-05-2006: Rewritten significantly by Ramakrishna Upadrasta.
Reason: For supporting additional features and bug-fixing (see summary below).
Note: The old preproc.lex is available in the same directory as preproc.original.lex
------------------------------------------------------------------------------------
*/
//Ramakrishna Upadrasta
//01-12-07: This is only version 0 of this file


/*
Summary of the changes:
=========================================================================================
    The regular expressions (and the action code) for C++ comments and 
    C style comments has been cleaned up and are (hopefully:) upto the standard.
=========================================================================================
    Naming of variables: 
        changed the lex variable names (states/start conditions) so that 
            CXX_COMMENT to corresponds to C++ style comments (beginning with //) and 
            C_COMMENT to correspond to C-style comments (enclosed by / * and * / ).
        Earlier they were
            C_COMMENT corresponded to C++ style comments
            CPP_COMMENT corresponded to C-style comments.
        It was felt that the names were confusing.
        The other choice was to name the C-style comments as C_COMMENT and C++ style 
        comments as CPP_COMMENT as in Wave. That was also found to be confusing, as CPP usually 
        means C Pre Processor). The new terminology is (partly) taken from makefiles which 
        allows 3 types of flags (C_FLAGS/CPP_FLAGS/CXX_FLAGS).

=========================================================================================
        Use STL types for instead of fixed size buffers for the following data structures
        commentString, macroString and bracestack.  (earlier they were 
                char commentString[10000000] for comments 
                char macroString[10000000] for macro strings
                and char bracestack[500] for matching of braces)
=========================================================================================
    Exact Position information for literals
        The previous method to get the position information from EDG is incorrect in 
        most of the cases. A second scan (like this lexer does) of the input was thought 
        to be necessary to attach additional info.
=========================================================================================
    Support for char literals
        including the '\e' '\U' which would be part of 2008 standard
        refer to ???? for details
=========================================================================================
    Semi colons
        We have to ensure that semi colons for that donot actually end 
        statements are taken care of carefully.
        class A {
        f(); <-- this semi colon should be taken care of carefully.
        }
=========================================================================================
    Double colons (::'s)
        They can only occur in  .........
        we identify them with 
=========================================================================================
trigraphs
    
=========================================================================================
    backslashes followed by newlines
        Either inside or outside of comments
=========================================================================================
    Whitespace info:
        The 

=========================================================================================
    Line number information:
        A better line count

=========================================================================================
    Trigraphs/Digraphs
=========================================================================================
    Proper identification and matching of different varieties of brackets
        Parenthesis ()
        Curly braces {}: The support for this existed earlier
        Square brackets []
=========================================================================================
    Support for keywords
        C++
        C
        C99
        C++Ox 
            Ones only in 
=========================================================================================
    To be done in CHAR literals
        wierd char literals
        testing for octal and hex char literals
        testing for backslashes in char literals.
    To be done in INT literals
        testing for 
=========================================================================================
    Limitations/open questions: 
        This lexer does not handle lots of important cases, like 
                character literals etc.  
                Trigraph sequences, alternative tokens (lex.trigraph and lex.digraph of the standard).  
                hexadecimal floating point literals and similar arcane stuff 
                        (HOW DOES EDG handle them? How does Wave handle them?)
    The user should probably use the wave workaround (does it support these?????), if she needs such things.
=========================================================================================
    Use of standard variable names like ([:space:] and [:blank:]) that come with FLEX. 
    Read the flex http://www.gnu.org/software/flex/manual/ for details. They work like C functions
    isspace and isblank respectively.
=========================================================================================
=========================================================================================
    A ROSE-sequence point in the original program is a position in the input program where 
    synchronization of the AST happens with the input program. For now, the sequence points
    are specific lexical tokens in the input program. In specific, they are 
        parenthesis: () 
        curly braces: {} 
        semi colons: ;
        new lines: \n
    The possible are contenders for being sequence points
        keywords???


    Sequence points of the program so that exact code generation is done
        If the code is not transformed? we synchronize the code generation
        If the code is transformed, we synchronize the code generation immediately after the next sequence point.

    An extension to the above token/character based sequence-points is to use parsing-constructs in the
    the grammar of the source language, like
        end of statement-sequence
        end of function/class/namespace definition
=========================================================================================
    ellipsis????

=========================================================================================
=========================================================================================
=========================================================================================
Notes about coding style:
    There is a unsaid tradition of separating the patterns and the actions in LEX files by tabs.
    I am using that, though Dan prefers that whitespace be used for indentation.
*/

/*
Test cases these (atleast fix) and effect:
    test2005_170.C about the buffer size of (C and C++ style) comments.
    test2001_18.C (and its related files: test2001_18A.h and test2001_18B.h): which is about the includes having code segments??????????????
    test2006_15.C: which is about the handling of the character literals.  
    test2005_184.C: which is about handling of backslashes at the end of C++ style comments
-------------------
    test2005_131.C
The ones that have singlequotes in a IF(0) and were earlier preprocessed silently, but are being flagged (incorrectly??) because we identify character literrals
Cxx tests
    test2004_154.C
    test2005_178.C
    test2005_191.C
    test2005_194.C
    test2005_196.C
    test2005_198.C
    test2005_199.C
    test2006_16.C
    test2006_17.C
    test2006_18.C
    test2006_26.C
-------------------------------------------------
write test cases so that 
    comments and preprocssor stuff etc.
    
*/
/*
       CHANGE LINES LIKE THIS TO ACCEPT std::string instead of char*, which makes us give it a c_str
                    preprocessorList.addElement(macrotype,macroString_str.c_str(),globalFileName,preproc_start_line_num,preproc_start_column_num,preproc_line_num-preproc_start_line_num); 
*/

#include "sage3basic.hhh"
#include <iostream>
#include <stdio.h>
#include <ctype.h>
#include <string>
#include <string.h>
#include <list>

/* DQ (1/21/2008): This now has a single definition in the header file: ROSE/src/frontend/SageIII/general_defs.h */
#include "general_token_defs.h"


#if 0
// DQ (1/21/2008): This definition is now redundent with what is in general_token_defs.h
struct token_element
{
    std::string token_lexeme;
    int token_id;
};
#endif

#if 0
// These are now defined in general_defs.h
//Ideally this should inherit from Sg_File_Info
//But for now, this will work
struct file_pos_info
{
    int line_num;
    int column_num;
};

struct stream_element
{ 
    struct token_element * p_tok_elem;
    struct file_pos_info beginning_fpi;
    struct file_pos_info ending_fpi;
};
#endif


using namespace std;

// list <stream_element*> ROSE_token_stream;
// typedef list <stream_element*>:: iterator SE_ITR;
LexTokenStreamTypePointer ROSE_token_stream_pointer = NULL;
typedef LexTokenStreamType::iterator SE_ITR;

struct file_pos_info curr_beginning;


//Rama
//#ifdef ROSE_TOKEN_IDENTIFIERS_H
//#define ROSE_TOKEN_IDENTIFIERS_H

//These would be used in the file INFO
//File info
namespace ROSE_token_ids
{
    //whitespace token IDs
    enum whitespace
    { 
        T_NOTKNOWN,
        //The following are non UNIX whitespace
    };

#if 0
    //I brazenly copied from the interface of WAVE for these enums
    enum ROSE_C_CXX_keywords
    {
        ROSE_ASM /* WAVE equivalent is T_ASM */
        ROSE_AUTO /* WAVE equivalent is T_AUTO */
        ROSE_BOOL /* WAVE equivalent is T_BOOL */
        ROSE_BREAK /* WAVE equivalent is T_BREAK */
        ROSE_CASE /* WAVE equivalent is T_CASE */
        ROSE_CATCH /* WAVE equivalent is T_CATCH */
        ROSE_CHAR /* WAVE equivalent is T_CHAR */
        ROSE_CLASS /* WAVE equivalent is T_CLASS */
        ROSE_CONST /* WAVE equivalent is T_CONST */
        ROSE_CONSTCAST /* WAVE equivalent is T_CONSTCAST */
        ROSE_CONTINUE /* WAVE equivalent is T_CONTINUE */
        ROSE_DEFAULT /* WAVE equivalent is T_DEFAULT */
        ROSE_DEFINED /* WAVE equivalent is T_DEFINED */
        ROSE_DELETE /* WAVE equivalent is T_DELETE */
        ROSE_DO /* WAVE equivalent is T_DO */
        ROSE_DOUBLE /* WAVE equivalent is T_DOUBLE */
        ROSE_DYNAMICCAST /* WAVE equivalent is T_DYNAMICCAST */
        ROSE_ELSE /* WAVE equivalent is T_ELSE */
        ROSE_ENUM /* WAVE equivalent is T_ENUM */
        ROSE_EXPLICIT /* WAVE equivalent is T_EXPLICIT */
        ROSE_EXPORT /* WAVE equivalent is T_EXPORT */
        ROSE_EXTERN /* WAVE equivalent is T_EXTERN */
        ROSE_FLOAT /* WAVE equivalent is T_FLOAT */
        ROSE_FOR /* WAVE equivalent is T_FOR */
        ROSE_FRIEND /* WAVE equivalent is T_FRIEND */
        ROSE_GOTO /* WAVE equivalent is T_GOTO */
        ROSE_IF /* WAVE equivalent is T_IF */
        ROSE_INLINE /* WAVE equivalent is T_INLINE */
        ROSE_INT /* WAVE equivalent is T_INT */
        ROSE_LONG /* WAVE equivalent is T_LONG */
        ROSE_MUTABLE /* WAVE equivalent is T_MUTABLE */
        ROSE_NAMESPACE /* WAVE equivalent is T_NAMESPACE */
        ROSE_NEW /* WAVE equivalent is T_NEW */
        ROSE_OPERATOR /* WAVE equivalent is T_OPERATOR */
        ROSE_PRIVATE /* WAVE equivalent is T_PRIVATE */
        ROSE_PROTECTED /* WAVE equivalent is T_PROTECTED */
        ROSE_PUBLIC /* WAVE equivalent is T_PUBLIC */
        ROSE_REGISTER /* WAVE equivalent is T_REGISTER */
        ROSE_REINTERPRETCAST /* WAVE equivalent is T_REINTERPRETCAST */
        ROSE_RETURN /* WAVE equivalent is T_RETURN */
        ROSE_SHORT /* WAVE equivalent is T_SHORT */
        ROSE_SIGNED /* WAVE equivalent is T_SIGNED */
        ROSE_SIZEOF /* WAVE equivalent is T_SIZEOF */
        ROSE_STATIC /* WAVE equivalent is T_STATIC */
        ROSE_STATICCAST /* WAVE equivalent is T_STATICCAST */
        ROSE_STRUCT /* WAVE equivalent is T_STRUCT */
        ROSE_SWITCH /* WAVE equivalent is T_SWITCH */
        ROSE_TEMPLATE /* WAVE equivalent is T_TEMPLATE */
        ROSE_THIS /* WAVE equivalent is T_THIS */
        ROSE_THROW /* WAVE equivalent is T_THROW */
        ROSE_TRY /* WAVE equivalent is T_TRY */
        ROSE_TYPEDEF /* WAVE equivalent is T_TYPEDEF */
        ROSE_TYPEID /* WAVE equivalent is T_TYPEID */
        ROSE_TYPENAME /* WAVE equivalent is T_TYPENAME */
        ROSE_UNION /* WAVE equivalent is T_UNION */
        ROSE_UNSIGNED /* WAVE equivalent is T_UNSIGNED */
        ROSE_USING /* WAVE equivalent is T_USING */
        ROSE_VIRTUAL /* WAVE equivalent is T_VIRTUAL */
        ROSE_VOID /* WAVE equivalent is T_VOID */
        ROSE_VOLATILE /* WAVE equivalent is T_VOLATILE */
        ROSE_WCHART /* WAVE equivalent is T_WCHART */
        ROSE_WHILE /* WAVE equivalent is T_WHILE */
    };
#endif

    enum ROSE_C_CXX_keywords 
    {
        C_CXX_ASM = 0 + 500,
        C_CXX_AUTO = 1 + 500,
        C_CXX_BOOL = 2 + 500,
        C_CXX_BREAK = 3 + 500,
        C_CXX_CASE = 4 + 500,
        C_CXX_CATCH = 5 + 500,
        C_CXX_CHAR = 6 + 500,
        C_CXX_CLASS = 7 + 500,
        C_CXX_CONST = 8 + 500,
        C_CXX_CONSTCAST = 9 + 500,
        C_CXX_CONTINUE = 10 + 500,
        C_CXX_DEFAULT = 11 + 500,
        C_CXX_DEFINED = 12 + 500,
        C_CXX_DELETE = 13 + 500,
        C_CXX_DO = 14 + 500,
        C_CXX_DOUBLE = 15 + 500,
        C_CXX_DYNAMICCAST = 16 + 500,
        C_CXX_ELSE = 17 + 500,
        C_CXX_ENUM = 18 + 500,
        C_CXX_EXPLICIT = 19 + 500,
        C_CXX_EXPORT = 20 + 500,
        C_CXX_EXTERN = 21 + 500,
        C_CXX_FALSE = 22 + 500,
        C_CXX_FLOAT = 23 + 500,
        C_CXX_FOR = 24 + 500,
        C_CXX_FRIEND = 25 + 500,
        C_CXX_GOTO = 26 + 500,
        C_CXX_IF = 27 + 500,
        C_CXX_INLINE = 28 + 500,
        C_CXX_INT = 29 + 500,
        C_CXX_LONG = 30 + 500,
        C_CXX_MUTABLE = 31 + 500,
        C_CXX_NAMESPACE = 32 + 500,
        C_CXX_NEW = 33 + 500,
        C_CXX_OPERATOR = 34 + 500,
        C_CXX_PRIVATE = 35 + 500,
        C_CXX_PROTECTED = 36 + 500,
        C_CXX_PUBLIC = 37 + 500,
        C_CXX_REGISTER = 38 + 500,
        C_CXX_REINTERPRETCAST = 39 + 500,
        C_CXX_RETURN = 40 + 500,
        C_CXX_SHORT = 41 + 500,
        C_CXX_SIGNED = 42 + 500,
        C_CXX_SIZEOF = 43 + 500,
        C_CXX_STATIC = 44 + 500,
        C_CXX_STATICCAST = 45 + 500,
        C_CXX_STRUCT = 46 + 500,
        C_CXX_SWITCH = 47 + 500,
        C_CXX_TEMPLATE = 48 + 500,
        C_CXX_THIS = 49 + 500,
        C_CXX_THROW = 50 + 500,
        C_CXX_TRY = 51 + 500,
        C_CXX_TRUE = 52 + 500,
        C_CXX_TYPEDEF = 53 + 500,
        C_CXX_TYPEID = 54 + 500,
        C_CXX_TYPENAME = 55 + 500,
        C_CXX_UNION = 56 + 500,
        C_CXX_UNSIGNED = 57 + 500,
        C_CXX_USING = 58 + 500,
        C_CXX_VIRTUAL = 59 + 500,
        C_CXX_VOID = 60 + 500,
        C_CXX_VOLATILE = 61 + 500,
        C_CXX_WCHART = 62 + 500,
        C_CXX_WHILE = 63 + 500
   };

      //the following are alternate tokens.
      //Refer to page 12 of C++ STD

enum ROSE_C_CXX_alternate_tok
    {
        C_CXX_LEFT_CURLY_ALT, // The <% which is the same as {
        C_CXX_RIGHT_CURLY_ALT, //The %> which is the same as }
        C_CXX_LEFT_SQUARE_ALT, //The <: which is the same as [
        C_CXX_RIGHT_SQUARE_ALT,//The :> which is the same as ]
        C_CXX_HASH_ALT, // The #
        C_CXX_HASH_HASH_ALT, // The 
        C_CXX_and_ALT, // The #
    };

enum ROSE_C_CXX_Additional_Info
{
      C_CXX_COMMENTS = 0 + 100000,
      C_CXX_STRING_LITERALS = 1 + 100000,
      C_CXX_IDENTIFIER = 2 + 100000,
      C_CXX_UNIDENTIFIED_TOKEN = 3 + 100000,
      C_CXX_ERROR = 4 + 100000
};



};

using namespace ROSE_token_ids;

token_element ROSE_C_CXX_keyword_map[] = 
{
        {"asm",              C_CXX_ASM }, 
        {"auto",             C_CXX_AUTO },
        {"bool",             C_CXX_BOOL },
        {"break",            C_CXX_BREAK },
        {"case",             C_CXX_CASE },
        {"catch",            C_CXX_CATCH },
        {"char",             C_CXX_CHAR },
        {"class",            C_CXX_CLASS },
        {"const",            C_CXX_CONST },
        {"const_cast",       C_CXX_CONSTCAST },
        {"continue",         C_CXX_CONTINUE },
        {"default",          C_CXX_DEFAULT },
        //{"defined",        C_CXX_DEFINED },
        {"delete",           C_CXX_DELETE },
        {"do",               C_CXX_DO },
        {"double",           C_CXX_DOUBLE },
        {"dynamic_cast",     C_CXX_DYNAMICCAST },
        {"else",             C_CXX_ELSE },
        {"enum",             C_CXX_ENUM },
        {"explicit",         C_CXX_EXPLICIT },
        {"export",           C_CXX_EXPORT },
        {"extern",           C_CXX_EXTERN },
        {"false",            C_CXX_FALSE },
        {"float",            C_CXX_FLOAT },
        {"for",              C_CXX_FOR },
        {"friend",           C_CXX_FRIEND },
        {"goto",             C_CXX_GOTO },
        {"if",               C_CXX_IF },
        {"inline",           C_CXX_INLINE },
        {"int",              C_CXX_INT },
        {"long",             C_CXX_LONG },
        {"mutable",          C_CXX_MUTABLE },
        {"namespace",        C_CXX_NAMESPACE },
        {"new",              C_CXX_NEW },
        {"operator",         C_CXX_OPERATOR },
        {"private",          C_CXX_PRIVATE },
        {"protected",        C_CXX_PROTECTED },
        {"public",           C_CXX_PUBLIC },
        {"register",         C_CXX_REGISTER },
        {"reinterpret_cast", C_CXX_REINTERPRETCAST },
        {"return",           C_CXX_RETURN },
        {"short",            C_CXX_SHORT },
        {"signed",           C_CXX_SIGNED },
        {"sizeof",           C_CXX_SIZEOF },
        {"static",           C_CXX_STATIC },
        {"static_cast",      C_CXX_STATICCAST },
        {"struct",           C_CXX_STRUCT },
        {"switch",           C_CXX_SWITCH },
        {"template",         C_CXX_TEMPLATE },
        {"this",             C_CXX_THIS },
        {"throw",            C_CXX_THROW },
        {"true",             C_CXX_TRUE },
        {"try",              C_CXX_TRY },
        {"typedef",          C_CXX_TYPEDEF },
        {"typeid",           C_CXX_TYPEID },
        {"typename",         C_CXX_TYPENAME },
        {"union",            C_CXX_UNION },
        {"unsigned",         C_CXX_UNSIGNED },
        {"using",            C_CXX_USING },
        {"virtual",          C_CXX_VIRTUAL },
        {"void",             C_CXX_VOID },
        {"volatile",         C_CXX_VOLATILE },
        {"wchar_t",          C_CXX_WCHART },
        {"while",            C_CXX_WHILE }
};



//We know that this is a magic number. 
//We also know how to fix it.
#define NUM_C_CXX_KEYWORDS 63

//returns -1 if the string is not a keyword
//otherwise returns the token ID
int identify_if_C_CXX_keyword(std::string str)
{
    //In FORTRAN, the identifiers are case insensitive.
    //So, variable 'ArEa' is the same as 'aReA'

    string lowered_str;
    for(unsigned int i = 0; i < str.size(); i++)
    {
        lowered_str += (unsigned char)tolower(str[i]);
    }

    //printf("got called with %s. Converted to %s\n", str.c_str(), lowered_str.c_str());
    for(int i = 0; i < NUM_C_CXX_KEYWORDS; i++)
    {
        if(lowered_str == ROSE_C_CXX_keyword_map[i].token_lexeme)
        {
            return (ROSE_C_CXX_keyword_map[i].token_id);
        }
    }
    return -1;
}



//#endif



#include "sage3.h"

// DQ (11/17/2004): Avoid compiler warning about unused function
#define YY_NO_UNPUT

#ifdef BOOL_IS_BROKEN
// If BOOL_IS_BROKEN then we can assume that there is no definition for "true" and "false"
#ifndef false
#define false 0
#endif
#ifndef true
#define true  1
#endif
#ifndef bool
typedef int bool;
#endif
#endif

#ifndef ROSE_ASSERT
#define ROSE_ASSERT assert
#endif

#include "rose_attributes_list.h"
#define yytext Rose_C_Cxx_text
#define yylex  Rose_C_Cxx_lex

#define HANDLEMACROSTART preproc_start_line_num=preproc_line_num; preproc_start_column_num=1; preproc_column_num+=strlen(yytext); macroString = yytext; BEGIN MACRO;

int i = 0;

std::string commentString;
std::string macroString;
//remove these _st's later
std::stack<int> curly_brace_stack_st;
std::stack<int> parenthesis_stack;
std::stack<int> square_bracket;

//All the above should be "matching constructs"
//create new for various casts and templates


// DQ (4/19/2006):
// Store the name of the current file so that we can generate Sg_File_Info objects
// for each comment.  Later we might want starting and ending fileInfo for each 
// comments and CPP directive.
std::string globalFileName;

int curr_brace_depth=0;
int ls_startingline_no; 
int ls_startingpreproc_column_num; 
int brace_counting_on=0;
PreprocessingInfo::DirectiveType macrotype = PreprocessingInfo::CpreprocessorUnknownDeclaration;

int topcurlybracestack();
void pushbracestack(int);
int popbracestack();
bool isemptystack();

int num_of_newlines(char*);

ROSEAttributesList preprocessorList;


//add stuff for char literals???????????

/*
whitespacenl            [\t\n ]+
whitespace              [\t ]+
lineprefix              ^{whitespace}*"#"{whitespace}*
macrokeyword            "include"|"define"|"undef"|"line"|"error"|"warning"|"if"|"ifdef"|"ifndef"|"elif"|"else"|"endif"
mlinkagespecification   ^{whitespace}*"extern"{whitespace}*(("\"C\"")|("\"C++\"")){whitespacenl}*"{"
*/

//refresher: blank and space function exactly as the STDLIB functions isblank and isspace respectively.
//blank matches only the various blank characters (\b,\t). It does NOT match new line. (and its variants).
//space matches blanks, newlines (various flavours like formfeed etc) as well as other stuff like vertical tab '\v' etc.
//refer to http://www.gatago.com/comp/std/c/18426853.html among others for difference between these two.
/*
The following was the original 
whitespacenl            [\t\n ]+
whitespace              [\t ]+

I changed it to
whitespacenl            [:space:]+
whitespace              [:blank:]+
This does not work.

What worked was the following:
whitespacenl            [[:space:]]+
whitespace              [[:blank:]]+

*/

/*
I am not sure if the whitespacenl would count the number of newlines in code that has backslashes properly.
*/
%}
whitespacenl            [[:space:]]+
whitespace              [[:blank:]]+
lineprefix              ^{whitespace}*"#"{whitespace}*
macrokeyword            "include"|"define"|"undef"|"line"|"error"|"warning"|"if"|"ifdef"|"ifndef"|"elif"|"else"|"endif"
mlinkagespecification   ^{whitespace}*"extern"{whitespace}*(("\"C\"")|("\"C++\"")){whitespacenl}*"{"
%s NORMAL CXX_COMMENT C_COMMENT STRING_LIT CHAR_LIT MACRO C_COMMENT_INMACRO
%%
%{
int preproc_line_num = 1;
        /*bad way to initialize*/
int preproc_start_line_num = preproc_line_num;
int preproc_column_num = 1;
int preproc_start_column_num = preproc_column_num;
                        /*Do we need this  ???*/
BEGIN NORMAL;
%}
<NORMAL>{mlinkagespecification} { 
                                    preprocessorList.addElement(PreprocessingInfo::ClinkageSpecificationStart, 
                                            yytext,globalFileName,preproc_line_num,preproc_column_num,0); 
                                    preproc_line_num+=num_of_newlines(yytext); 
                                    preproc_column_num+=strlen(yytext); 

                                    curr_brace_depth++; 
                                    pushbracestack(curr_brace_depth);
                                }
        /*Handle the braces (left and right). Push and Pop the bracestack accordingly*/
<NORMAL>"{"     { 
                    if(!isemptystack()) 
                    { 
                        /* we only count braces within a linkage specification. */ 
                        curr_brace_depth++; 
                    } 
                    //printf("Ram: found left brace at preproc_line_num = %d, preproc_column_num = %d\n", preproc_line_num, preproc_column_num);
                    preproc_column_num++; 
                }

<NORMAL>"}"     { 
                    if(!isemptystack()) 
                    { 
                        if(curr_brace_depth==topcurlybracestack()) 
                        { 
                            popbracestack(); 
                            preprocessorList.addElement(PreprocessingInfo::ClinkageSpecificationEnd, 
                                                            yytext, globalFileName, preproc_line_num, preproc_column_num, 0); 
                        }
                        curr_brace_depth--; 
                    } 
                    //printf("Ram: found right brace at preproc_line_num = %d, preproc_column_num = %d\n", preproc_line_num, preproc_column_num);
                    preproc_column_num++; 
                }

<NORMAL>"("     { 
                    /*if(!isemptystack()) 
                    { 
                        //we only count braces within a linkage specification. 
                        curr_brace_depth++; 
                    }
                    */
                    preproc_column_num++; 
                }

<NORMAL>")"     { 
                    /*
                    if(!isemptystack()) 
                    { 
                        if(curr_brace_depth==topcurlybracestack()) 
                        { 
                            popbracestack(); 
                            preprocessorList.addElement(PreprocessingInfo::ClinkageSpecificationEnd, 
                                                            yytext, globalFileName, preproc_line_num, preproc_column_num, 0); 
                        }
                        curr_brace_depth--; 
                    } 
                    */
                    preproc_column_num++; 
                }

<NORMAL>[a-zA-Z_][a-zA-Z0-9_]*       {

                                          token_element *p_tok_elem = new token_element;
                                          p_tok_elem->token_lexeme = yytext;

                                          int is_keyword = identify_if_C_CXX_keyword(yytext);
                                          //found a keyword?
                                          if(is_keyword != -1)
                                          {   
                                              //printf("%s is a keyword\n", yytext); 
                                              p_tok_elem->token_id = is_keyword;
                                          }
                                          else
                                          {   
                                              //printf("%s is not a keyword\n", yytext); 
                                              p_tok_elem->token_id = C_CXX_IDENTIFIER;
                                          }

                                          stream_element *p_se = new stream_element;
                                          p_se->p_tok_elem = p_tok_elem;

                                          /*
                                          p_se->beginning_fpi.line_num = preproc_column_num;
                                          p_se->beginning_fpi.column_num = preproc_column_num;
                                          preproc_column_num+=strlen(yytext);
                                          p_se->ending_fpi.line_num = preproc_line_num;
                                          p_se->ending_fpi.column_num = preproc_column_num-1;
                                          //push the element onto the token stream
                                          */
                                          ROSE_token_stream_pointer->push_back(p_se);
                                    }
        /*begin handling the C++ style comments. */
<NORMAL>"\/\/"  {
                    /*Handle the C comments.*/ 
                    preproc_start_line_num=preproc_line_num; 
                    preproc_start_column_num=preproc_column_num; 
                    preproc_column_num+=2; 
                    commentString = yytext; 
                    BEGIN CXX_COMMENT;
                }

        /*The normal mode actions. Handle the preprocessor stuff and anyother characters. */
        /*Do we need the backslash character?????????????*/
<NORMAL>"/*"    {
                    preproc_start_line_num=preproc_line_num; 
                    preproc_start_column_num=preproc_column_num; 
                    preproc_column_num+=strlen(yytext);
                    
                    //copy the comment to commentString 
                    commentString = yytext; 
                    BEGIN C_COMMENT; 
                }
    /*These are the alternative tokens. Refer to page 12 of C++ std to understand their representation*/
                  /*<NORMAL>"<%"     {curr_token = ALT_LEFT_CURLY; do_yytext_bookeeping(); }
<NORMAL>"<%"     { }
                       */




<NORMAL>"static_cast"   { 
                    /*
                    if(!isemptystack()) 
                    { 
                        //we only count braces within a linkage specification. 
                        curr_brace_depth++; 
                    
                        */
                    preproc_column_num++; 
                }


<NORMAL>{lineprefix}"include"   { macrotype=PreprocessingInfo::CpreprocessorIncludeDeclaration; HANDLEMACROSTART }
<NORMAL>{lineprefix}"define"    { macrotype=PreprocessingInfo::CpreprocessorDefineDeclaration; HANDLEMACROSTART }
<NORMAL>{lineprefix}"undef"     { macrotype=PreprocessingInfo::CpreprocessorUndefDeclaration; HANDLEMACROSTART }
<NORMAL>{lineprefix}"line"      { macrotype=PreprocessingInfo::CpreprocessorLineDeclaration; HANDLEMACROSTART }
<NORMAL>{lineprefix}"error"     { macrotype=PreprocessingInfo::CpreprocessorErrorDeclaration; HANDLEMACROSTART }
<NORMAL>{lineprefix}"if"        { macrotype=PreprocessingInfo::CpreprocessorIfDeclaration; HANDLEMACROSTART }
<NORMAL>{lineprefix}"ifdef"     { macrotype=PreprocessingInfo::CpreprocessorIfdefDeclaration; HANDLEMACROSTART }
<NORMAL>{lineprefix}"ifndef"    { macrotype=PreprocessingInfo::CpreprocessorIfndefDeclaration; HANDLEMACROSTART }
<NORMAL>{lineprefix}"elif"      { macrotype=PreprocessingInfo::CpreprocessorElifDeclaration; HANDLEMACROSTART }
<NORMAL>{lineprefix}"else"      { macrotype=PreprocessingInfo::CpreprocessorElseDeclaration; HANDLEMACROSTART }
<NORMAL>{lineprefix}"endif"     { macrotype=PreprocessingInfo::CpreprocessorEndifDeclaration; HANDLEMACROSTART }
<NORMAL>{lineprefix}"warning"   { macrotype=PreprocessingInfo::CpreprocessorWarningDeclaration; HANDLEMACROSTART }
        /*Add code here to attach the whitespace before newlines (and general lineprefix code) */
<NORMAL>\n                      { preproc_line_num++; preproc_column_num=1; } 
<NORMAL>"\""                    { /*preproc_line_num+=num_of_newlines(yytext);*/ preproc_column_num+=strlen(yytext); BEGIN STRING_LIT;}
<NORMAL>"'"                     { /*preproc_line_num+=num_of_newlines(yytext);*/ preproc_column_num+=strlen(yytext); BEGIN CHAR_LIT;}
<NORMAL>.                       { preproc_column_num++; }




                        /* Actions for character literals. Since the part between single quote can be more than one
                         * character of input (e.g., '\\', '\'', '\n', '\012', etc) we parse them sort of like strings. */
<CHAR_LIT>\\\n                  {/*eat escaped linefeed*/       preproc_line_num++; preproc_column_num=1; }
<CHAR_LIT>\\.                   {/*eat escaped something*/      preproc_column_num+=strlen(yytext); }
<CHAR_LIT>[^'\n\\]              {/*eat non-special characters*/ preproc_column_num+=strlen(yytext); }
<CHAR_LIT>\n                    {/*eat linefeed*/               preproc_line_num++; preproc_column_num=1; }
<CHAR_LIT>"'"                   {/*end of character literal*/   preproc_column_num+=strlen(yytext); BEGIN NORMAL; }



                        /* Actions for string literals. */
<STRING_LIT>\\\n                {/*eat escaped linefeed*/       preproc_line_num++; preproc_column_num=1; }
<STRING_LIT>\\.                 {/*eat escaped something*/      preproc_column_num+=strlen(yytext); }
<STRING_LIT>[^\"\n\\]           {/*eat non-special characters*/ preproc_column_num+=strlen(yytext); }
<STRING_LIT>\n                  {/*eat linefeed*/               preproc_line_num++; preproc_column_num=1; }
<STRING_LIT>"\""                {/*end of string literal*/      preproc_column_num+=strlen(yytext); BEGIN NORMAL; }




        /*Actions for a C++ style comment.*/
<CXX_COMMENT>[^\\\n]    { /* eat anything that is not a backslash or a newline*/ commentString += yytext;}
<CXX_COMMENT>\\[^\n]    { 
                            /*eat up backslashes not immdediately followed by newlines*/ 
                            commentString += yytext;
                            /*fix the preproc_column_num at other places ????????????????????  ??????????????*/ 
                            preproc_column_num=1; 
                        }

<CXX_COMMENT>\\\n       { 
                            /*if backslash is followed by a newline, then eat them (backslash, newline) up, increment preproc_line_num and continue*/ 
                            commentString += yytext;
                            preproc_line_num++; 
}
<CXX_COMMENT>\n         {
                            preproc_line_num++; preproc_column_num=1; 
                            commentString += yytext;
                            preprocessorList.addElement(PreprocessingInfo::CplusplusStyleComment, 
                                    commentString,globalFileName, preproc_start_line_num, preproc_start_column_num,preproc_line_num-preproc_start_line_num); 

                            BEGIN NORMAL; 
                        }
    /* negara1 (07/25/2011): Added handling of CXX comments that appear at the last line of a file. */
<CXX_COMMENT><<EOF>>    {
                            preprocessorList.addElement(PreprocessingInfo::CplusplusStyleComment, 
                                    commentString,globalFileName, preproc_start_line_num, preproc_start_column_num,preproc_line_num-preproc_start_line_num); 

                            yyterminate();
                        }
        /*Actions while in a C style comment.*/
<C_COMMENT>\n           {
                            commentString += yytext;
                            preproc_line_num++; 
                            preproc_column_num=1; 
                        }
<C_COMMENT>"*/" {
                            commentString += yytext;
                            preproc_column_num+=strlen(yytext); 
                            /*printf("hello Ram: The comment string is ####%s#############. It is of %d lines, the current line number is %d\n",
                                    commentString.c_str(), preproc_line_num-preproc_start_line_num+1, preproc_line_num);
                                    */
                            preprocessorList.addElement(PreprocessingInfo::C_StyleComment,commentString,globalFileName,preproc_start_line_num,preproc_start_column_num,preproc_line_num-preproc_start_line_num+1); 
                            BEGIN(NORMAL); 
                        }
<C_COMMENT>.    { 
                    commentString += yytext;
                    preproc_column_num++; 
                }

        /*Actions while in a MACRO.*/
<MACRO>\\\n     { 
                    macroString += yytext;
                    preproc_line_num++; 
                    preproc_column_num=1;
                }
<MACRO>\n       {
                    macroString += yytext;
                    preproc_line_num++; 
                    preproc_column_num=1; 
                    
                    preprocessorList.addElement(macrotype,macroString,globalFileName,preproc_start_line_num,preproc_start_column_num,preproc_line_num-preproc_start_line_num); 
                    BEGIN NORMAL; 
                }
    /* negara1 (07/25/2011): Added handling of preprocessor directives that end at the last line of a file. */
<MACRO><<EOF>>  {
                    macroString += yytext;
                    
                    preprocessorList.addElement(macrotype,macroString,globalFileName,preproc_start_line_num,preproc_start_column_num,preproc_line_num-preproc_start_line_num); 
                    yyterminate();
                }
<MACRO>"\/*"    {
                    //does this cover all cases?????????
                    preproc_column_num+=2; 
                    macroString += yytext;

                    /*
                                //Do we need to do something like this?
                                commentString = yytext;
                    */
                    BEGIN C_COMMENT_INMACRO;
                }
<MACRO>.        { 
                    macroString += yytext;
                    preproc_column_num++; 
                }
<C_COMMENT_INMACRO>"*/"   { 
                                //??????????????????????????????????????????????????????????????
                                //This code copies the comment into the macrobuffer.
                                //Should we not copy it to comment buffer also?????
                                macroString += yytext;

                                /*
                                commentString += yytext;
                                //should we do something like this??
                                preprocessorList.addElement(PreprocessingInfo::C_StyleComment,commentString.c_str(),globalFileName,preproc_start_line_num,preproc_start_column_num,preproc_line_num-preproc_start_line_num); 
                                */
                                preproc_column_num+=strlen(yytext); 
                                BEGIN MACRO; 
                            }
<C_COMMENT_INMACRO>\n       { 
                                macroString += yytext;

                                //Do we need to do something like this?
                                //commentString += yytext;

                                preproc_line_num++; preproc_column_num=1; 
                            }
<C_COMMENT_INMACRO>.        { 
                                macroString += yytext;

                                //Do we need to do something like this?
                                //commentString += yytext;

                                preproc_column_num++; 
                            }
%%

const int maxstacksize=500;
int bracestack[maxstacksize];

int top=0;
void pushbracestack(int brace_no) { bracestack[top++]=brace_no; }
int topcurlybracestack() { if(top) return bracestack[top-1]; else return -1; }
int popbracestack() { return bracestack[--top]; }
bool isemptystack() { return top==0; }

int num_of_newlines(char* s)
{
     int num = 0;
     while(*s != '\0')
        {
          if(*s == '\n')
               num++;
          s++;
        }
     return num;
}

// DQ (3/30/2006): Modified to use C++ style string instead of C style char* string
// ROSEAttributesList *getPreprocessorDirectives( char *fileName)
//! This function extracts the comments and CPP preprocessor control directives from the input file.
// ROSEAttributesList *getPreprocessorDirectives( std::string fileName )
// ROSEAttributesList *getPreprocessorDirectives( std::string fileName, LexTokenStreamTypePointer & input_token_stream_pointer )
ROSEAttributesList *getPreprocessorDirectives( std::string fileName )
   {
     FILE *fp = NULL;
     ROSEAttributesList *preprocessorInfoList = new ROSEAttributesList; // create a new list
     ROSE_ASSERT (preprocessorInfoList != NULL);

  // printf ("Inside of lex file: getPreprocessorDirectives() \n");
  // ROSE_ASSERT(false);

  // assert(input_token_stream_pointer == NULL);
     ROSE_token_stream_pointer = new LexTokenStreamType;
     assert(ROSE_token_stream_pointer != NULL);

  // Share a pointer to the same LexTokenStreamType just allocated
  // input_token_stream_pointer = ROSE_token_stream_pointer;
  // assert(input_token_stream_pointer != NULL);

  // Attach the token stream to the ROSEAttributesList being returned.
     preprocessorInfoList->set_rawTokenStream(ROSE_token_stream_pointer);

  // Set the global filename to be used in the PreprocessingInfo constructor.
     globalFileName = fileName;

     if ( fileName.empty() == false )
        {
          std::map<std::string,ROSEAttributesList* >::iterator iItr= 
                  mapFilenameToAttributes.find(fileName);
          //std::cout << "Trying to find fileName " << fileName << std::endl;
          if ( iItr != mapFilenameToAttributes.end())
             {
                     //std::cout << "Found requested file: " << fileName << " size: " << iItr->second->size() << std::endl; 
               for(std::vector<PreprocessingInfo*>::iterator jItr = iItr->second->getList().begin(); 
                               jItr != iItr->second->getList().end(); ++jItr){
                     //std::cout << "Inserting element" <<  (*jItr)->getString() << std::endl;
                     preprocessorInfoList->insertElement(**jItr);

               }

             }
          else{
            fp = fopen( fileName.c_str(), "r");
            if (fp)
            {
              yyin = fp;
              yylex();

              // Writes all gathered information to stdout
              // preprocessorList.display("TEST Collection of Comments and CPP Directives");

              // bugfix (9/29/2001)
              // The semantics required here is to move the elements accumulated into the
              // preprocessorList into the preprocessorInfoList and delete them from the
              // preprocessorList (which will be used again to accumulate PreprocessingInfo objects
              // when the next file is processed).  We have to be able to process several files using
              // this getPreprocessorDirectives() function.
              preprocessorInfoList->moveElements( preprocessorList ); // create a copy that we can pass on

              // The accumulator list should now be empty
              assert (preprocessorList.getLength() == 0);
              fclose(fp);  
            }
            else
            {
              // DQ (5/14/2006): Added error checking for collection of comments and CPP directives.
              printf ("Error: can't find the requested file (%s) \n",fileName.c_str());
              // ROSE_ASSERT(false);
            }
          }

        }

  // printf ("In getPreprocessorDirectives(fileName = %s): preprocessorInfoList->size() = %d \n",fileName.c_str(),(int)preprocessorInfoList->size());

     return preprocessorInfoList;
   }
