%option noyywrap
%option prefix="Rose_Fortran_free_format_"
%option outfile="lex.yy.c"
%{
/* 
Version 0.2.1 of FORTRAN scanner.
03-09-2007: Written by Ramakrishna Upadrasta and Dan Quinlan.

In an input free formatted FORTRAN file, this scanner recognizes the following
the ROSE sequence point in a FORTRAN file
     comments (beginning with ! and extendig till the end of the line)
     string literals (enclosed by single quotes '')
     Keywords (the list is provided in this code)
*/


/*
-------------------------------------------------------------------------------------
New in the current release:
    Better data structures for the keywords
    scripts to generate the header files automatically
    preliminary macro processing
-------------------------------------------------------------------------------------
*/

/*
-------------------------------------------------------------------------------------
Acknowledgements:
-------------------------------------------------------------------------------------
The authors (Dan and Rama) looked at the source codes of GFORTRAN as well as G95. 
We gratefully acknowledge the authors of GFORTRAN and G95.
*/

/*
-------------------------------------------------------------------------------------
Limitations: 
-------------------------------------------------------------------------------------
The authors (Rama and Dan) recognize many of the limitations. 
They will be tabulated later.
*/

/*
There are three scanners in the ROSE frontend. One for C/C++, two for fixed and free 
format FORTRAN languages. So that the flex generated C code and definitions (yylex, 
yytext, etc.) of these does not interfere with any other, we are adding 
     the prefix 'Rose_Fortran_C_Cxx_'        for the C/C++ scanner,
     the prefix 'Rose_Fortran_Fixed_Format_' for the Fixed Format FORTRAN scanner and
     the prefix 'Rose_Fortran_Free_Format_'  for the Free Format FORTRAN scanner
by using the -P<prefix options>
Note: We could have done this in an arguably more elegant way by changing the individual uses of these 
functions/variables.
*/
#define yytext Rose_Fortran_free_format_text
#define yylex Rose_Fortran_free_format_lex 



/* DQ (12/10/2016): This is a technique to suppress warnings in generated code that we want to be an error elsewhere in ROSE. 
   See https://gcc.gnu.org/onlinedocs/gcc/Diagnostic-Pragmas.html for more detail.
 */
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)
#pragma GCC diagnostic ignored "-Wsign-compare"
#endif

#include "sage3basic.h"

#include <iostream>
#include <stdio.h>
#include <ctype.h>
#include <string>
#include <list>



using namespace std;

// namespace Rose_Fortran_free_format_namespace {

#include "general_token_defs.h"
#include "./rose_fortran_token_maps.h"

#if 0
// These are now defined in general_defs.h
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

// DQ (1/21/2008): Modified this to be a pointer so it could be built and returned to ROSE.
LexTokenStreamTypePointer ROSE_Fortran_free_format_token_stream_pointer = NULL;

typedef LexTokenStreamType::iterator SE_ITR;

static struct file_pos_info curr_beginning;

//Operators
//This includes the "new" as well as the "old" operators
token_element ROSE_Fortran_Operator_map[] = 
{
      {"+",       SgToken::FORTRAN_INTRINSIC_PLUS},            /*   GFORTRAN/G95 equivalent is INTRINSIC_PLUS      */
      {"-",       SgToken::FORTRAN_INTRINSIC_MINUS},           /*   GFORTRAN/G95 equivalent is INTRINSIC_MINUS     */
      {"**",      SgToken::FORTRAN_INTRINSIC_POWER},           /*   GFORTRAN/G95 equivalent is INTRINSIC_POWER     */
      {"//",      SgToken::FORTRAN_INTRINSIC_CONCAT},          /*   GFORTRAN/G95 equivalent is INTRINSIC_CONCAT    */
      {"*",       SgToken::FORTRAN_INTRINSIC_TIMES},           /*   GFORTRAN/G95 equivalent is INTRINSIC_TIMES     */
      {"/",       SgToken::FORTRAN_INTRINSIC_DIVIDE},          /*   GFORTRAN/G95 equivalent is INTRINSIC_DIVIDE    */
      {".and.",   SgToken::FORTRAN_INTRINSIC_AND},             /*   GFORTRAN/G95 equivalent is INTRINSIC_AND       */
      {".or.",    SgToken::FORTRAN_INTRINSIC_OR},              /*   GFORTRAN/G95 equivalent is INTRINSIC_OR        */
      {".eqv.",   SgToken::FORTRAN_INTRINSIC_EQV},             /*   GFORTRAN/G95 equivalent is INTRINSIC_EQV       */
      {".neqv.",  SgToken::FORTRAN_INTRINSIC_NEQV},            /*   GFORTRAN/G95 equivalent is INTRINSIC_NEQV      */
      //{".true.",  SgToken::FORTRAN_TRUE},            /*   GFORTRAN/G95 equivalent is INTRINSIC_NEQV      */
      //{".false.",  SgToken::FORTRAN_FALSE},            /*   GFORTRAN/G95 equivalent is INTRINSIC_NEQV      */
      {"==",      SgToken::FORTRAN_INTRINSIC_EQ},              /*   GFORTRAN/G95 equivalent is INTRINSIC_EQ        */
      {"/=",      SgToken::FORTRAN_INTRINSIC_NE},              /*   GFORTRAN/G95 equivalent is INTRINSIC_NE        */
      {">=",      SgToken::FORTRAN_INTRINSIC_GE},              /*   GFORTRAN/G95 equivalent is INTRINSIC_GE        */
      {"<=",      SgToken::FORTRAN_INTRINSIC_LE},              /*   GFORTRAN/G95 equivalent is INTRINSIC_LE        */
      {"<",       SgToken::FORTRAN_INTRINSIC_LT},              /*   GFORTRAN/G95 equivalent is INTRINSIC_LT        */
      {">",       SgToken::FORTRAN_INTRINSIC_GT},              /*   GFORTRAN/G95 equivalent is INTRINSIC_GT        */
      {".not.",   SgToken::FORTRAN_INTRINSIC_NOT},             /*   GFORTRAN/G95 equivalent is INTRINSIC_NOT       */
      {".eq.",    SgToken::FORTRAN_INTRINSIC_OLDEQ},           /*   GFORTRAN/G95 equivalent is INTRINSIC_OLDEQ     */
      {".ne.",    SgToken::FORTRAN_INTRINSIC_OLDNE},           /*   GFORTRAN/G95 equivalent is INTRINSIC_OLDNE     */
      {".ge.",    SgToken::FORTRAN_INTRINSIC_OLDGE},           /*   GFORTRAN/G95 equivalent is INTRINSIC_OLDGE     */
      {".le.",    SgToken::FORTRAN_INTRINSIC_OLDLE},           /*   GFORTRAN/G95 equivalent is INTRINSIC_OLDLE     */
      {".lt.",    SgToken::FORTRAN_INTRINSIC_OLDLT},           /*   GFORTRAN/G95 equivalent is INTRINSIC_OLDLT     */
      {".gt.",    SgToken::FORTRAN_INTRINSIC_OLDGT}            /*   GFORTRAN/G95 equivalent is INTRINSIC_OLDGT     */
};


//We know that this is a magic number. 
//We also know how to fix it.
#define NUM_KEYWORDS 67

//returns -1 if the string is not a keyword
//otherwise returns the token ID
static int identify_if_keyword(string str)
{
    //In FORTRAN, the identifiers are case insensitive.
    //So, variable 'ArEa' is the same as 'aReA'

    string lowered_str;
    for(unsigned int i = 0; i < strlen(str.c_str()); i++)
    {
        lowered_str += (unsigned char)tolower(char((str.c_str())[i]));
    }

    //printf("got called with %s. Converted to %s\n", str.c_str(), lowered_str.c_str());
    for(int i = 0; i < NUM_KEYWORDS; i++)
    {
        if(lowered_str == ROSE_Fortran_keyword_map[i].token_lexeme)
        {
            return (ROSE_Fortran_keyword_map[i].token_id);
        }
    }
    return -1;
}

//We know that this is a magic number. 
//It will be fixed.
#define NUM_OPERATORS 23

#if 0
static int identify_operator_matching(string str)
{
    //printf("got called with %s. \n", str.c_str());

    for(int i = 0; i < NUM_OPERATORS; i++)
    {
        if(str == ROSE_Fortran_Operator_map[i].token_lexeme)
        {
            return (ROSE_Fortran_Operator_map[i].token_id);
        }
    }
    return -1;
}
#endif

static void process_operator(string op) 
{

}


//This could be handled in a fashion better than a MACRO, but for now this will work
/*
#define process_operator(op) {  \
    token_element *p_tok_elem = new token_element;  \
    p_tok_elem->token_lexeme = yytext; \
    int rr = identify_if_keyword(yytext); \
    stream_element *p_se = new stream_element;  \
    p_se->p_tok_elem = p_tok_elem; \
    p_se->beginning_fpi.line_num = line_no; \
    p_se->beginning_fpi.column_num = column_no; \
    column_no+=strlen(yytext); \
    p_se->ending_fpi.line_num = line_no; \
    p_se->ending_fpi.column_num = column_no-1; \
    ROSE_Fortran_free_format_token_stream.push_back(p_se);  \
} \

*/

static void process_operator(string op);

#define YY_NO_UNPUT

// static int i = 0;
static std::string currentBuffer;

// DQ (4/19/2006):
// Store the name of the current file so that we can generate Sg_File_Info objects
// for each comment.  Later we might want starting and ending fileInfo for each 
// comments and CPP directive.
static std::string globalFileName;

#define FORTRAN_LEXICAL_SUPPORT 0

//struct matching_construct
static int braces_no=0;
// static int brace_counting_on=0;

// static int topbracestack();
static void pushbracestack(int);
// static int popbracestack();
// static bool isemptystack();

static int num_of_newlines(char*);
static int adjust_new_line_counter();

using namespace std;
//#undef yywrap


%}

whitespacenl                  [\t\n ]+
whitespace                    [\t ]+
alphanum                      [0-9A-Za-z]  

lineprefix                   ^{whitespace}*"#"{whitespace}*
macrokeyword                 "include"|"define"|"undef"|"line"|"error"|"warning"|"if"|"ifdef"|"ifndef"|"elif"|"else"|"endif"

mlinkagespecification        ^{whitespace}*"extern"{whitespace}*(("\"C\"")|("\"C++\"")){whitespacenl}*"{"

%s NORMAL FORT_COMMENT STRING_LIT MACRO 
%%

%{
#undef Rose_Fortran_Free_Format_wrap
          int line_no = 1;

       /* DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable. */
       /* int start_line_no = line_no; */

          int column_no = 1;

       /* DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable. */
       /* int start_column_no = column_no; */

          BEGIN NORMAL;
%}

<NORMAL>{mlinkagespecification} { 
                                    /*preprocessorList.addElement(PreprocessingInfo::ClinkageSpecificationStart, 
                                            yytext,globalFileName,line_no,column_no,0); 
                                            */
                                    braces_no++; 
                                    line_no+=num_of_newlines(yytext); 
                                    column_no+=strlen(yytext); 
                                    pushbracestack(braces_no); 
                   }
<NORMAL>"!"    {
                 /* DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable. */
                 /* start_line_no=line_no; */
                 /* start_column_no=column_no; */
                    curr_beginning.line_num = line_no;
                    curr_beginning.column_num = column_no;
                    column_no+=1; 
                    currentBuffer = yytext; 
                    BEGIN FORT_COMMENT; 
               }
<NORMAL>"'"    {
                 /* DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable. */
                 /* This begins a STRING LITERAL */
                 /* start_line_no=line_no; */
                 /* start_column_no=column_no; */
                    curr_beginning.line_num = line_no;
                    curr_beginning.column_num = column_no;
                    column_no+=1; 
                    currentBuffer = yytext; 
                    BEGIN STRING_LIT;
               }
<NORMAL>\n                           { line_no++; column_no=1; } 
<NORMAL>[a-zA-Z_][a-zA-Z0-9_]*       { 
                                          
                                          token_element *p_tok_elem = new token_element; 
                                          p_tok_elem->token_lexeme = yytext; 

                                          int is_keyword = identify_if_keyword(yytext); 
                                          //found a keyword?
                                          if(is_keyword != -1)
                                          {
                                              //printf("%s is a keyword\n", yytext); 
                                              p_tok_elem->token_id = is_keyword; 
                                          }
                                          else
                                          {
                                              //printf("%s is not a keyword\n", yytext); 
                                              p_tok_elem->token_id = SgToken::FORTRAN_IDENTIFIER;
                                          }
                                          
                                          stream_element *p_se = new stream_element; 
                                          p_se->p_tok_elem = p_tok_elem; 
                                          
                                          p_se->beginning_fpi.line_num = line_no;
                                          p_se->beginning_fpi.column_num = column_no;
                                          column_no+=strlen(yytext); 
                                          p_se->ending_fpi.line_num = line_no; 
                                          p_se->ending_fpi.column_num = column_no-1; 
                                          //push the element onto the token stream
                                          ROSE_Fortran_free_format_token_stream_pointer->push_back(p_se); 
                                    }
<NORMAL>"+"        {process_operator(yytext);column_no+=strlen(yytext);}
<NORMAL>"-"        {process_operator(yytext);column_no+=strlen(yytext);}
<NORMAL>"**"       {process_operator(yytext);column_no+=strlen(yytext);}
<NORMAL>"//"       {process_operator(yytext);column_no+=strlen(yytext);}
<NORMAL>"*"        {process_operator(yytext);column_no+=strlen(yytext);}
<NORMAL>"/"        {process_operator(yytext);column_no+=strlen(yytext);}
<NORMAL>".and."    {process_operator(yytext);column_no+=strlen(yytext);}
<NORMAL>".or."     {process_operator(yytext);column_no+=strlen(yytext);}
<NORMAL>".eqv."    {process_operator(yytext);column_no+=strlen(yytext);}
<NORMAL>".neqv."   {process_operator(yytext);column_no+=strlen(yytext);}
<NORMAL>"=="       {process_operator(yytext);column_no+=strlen(yytext);}
<NORMAL>"/="       {process_operator(yytext);column_no+=strlen(yytext);}
<NORMAL>">="       {process_operator(yytext);column_no+=strlen(yytext);}
<NORMAL>"<="       {process_operator(yytext);column_no+=strlen(yytext);}
<NORMAL>"<"        {process_operator(yytext);column_no+=strlen(yytext);}
<NORMAL>">"        {process_operator(yytext);column_no+=strlen(yytext);}
<NORMAL>".not."    {process_operator(yytext);column_no+=strlen(yytext);}
<NORMAL>".eq."     {process_operator(yytext);column_no+=strlen(yytext);}
<NORMAL>".ne."     {process_operator(yytext);column_no+=strlen(yytext);}
<NORMAL>".ge."     {process_operator(yytext);column_no+=strlen(yytext);}
<NORMAL>".le."     {process_operator(yytext);column_no+=strlen(yytext);}
<NORMAL>".lt."     {process_operator(yytext);column_no+=strlen(yytext);}
<NORMAL>".gt."     {process_operator(yytext);column_no+=strlen(yytext);}
<NORMAL>".true."     {process_operator(yytext);column_no+=strlen(yytext);} 
<NORMAL>".false."     {process_operator(yytext);column_no+=strlen(yytext);}
<NORMAL>";"        {/*Do stuff to store semi colons*/ /*process_operator(yytext);*/column_no+=strlen(yytext);}
<NORMAL>","        {/*Do stuff to store commas*/ /*process_operator(yytext);*/column_no+=strlen(yytext);}
<NORMAL>"::"       {/*Do stuff to store colons*/ /*process_operator(yytext);*/column_no+=strlen(yytext);}
               /*
<NORMAL>^[:blank:]*\n          {
                                       //According to the STD, a line which has whitespaces is a comment line
                                       //We will store it
                                       printf("came across a blank line\n");
                                       currentBuffer += yytext;

                                       token_element *p_tok_elem = new token_element;
                                       p_tok_elem->token_lexeme = currentBuffer;
                                       p_tok_elem->token_id = SgToken::FORTRAN_COMMENTS;
           
                                       stream_element *p_se = new stream_element;
                                       p_se->p_tok_elem = p_tok_elem;

                                       p_se->beginning_fpi.line_num = line_no; 
                                       p_se->beginning_fpi.column_num = column_no;

                                       p_se->ending_fpi.line_num = line_no;
                                       p_se->ending_fpi.column_num = column_no;
                                       ROSE_Fortran_free_format_token_stream_pointer->push_back(p_se);

                                       line_no++; column_no=1;
                                  }
                                  */
<NORMAL>.               {column_no++;}
<STRING_LIT>[^']        {
                            column_no++; 
                            currentBuffer += yytext;
                        }
<STRING_LIT>''          {
                            column_no+=2; 
                            currentBuffer += yytext;
                        }
<STRING_LIT>"'"         { 
                            //This ends the STRING LITERALS
                            currentBuffer += yytext;
                            //printf("\nthe literal string is %s\n", currentBuffer.c_str());
                            //printf("ending in (%d, %d) \n", line_no, column_no);

                            adjust_new_line_counter();

                            token_element *p_tok_elem = new token_element;
                            p_tok_elem->token_lexeme = currentBuffer;
                            p_tok_elem->token_id = SgToken::FORTRAN_STRING_LITERALS;

                            stream_element *p_se = new stream_element;
                            p_se->p_tok_elem = p_tok_elem;

                            p_se->beginning_fpi = curr_beginning;
                            p_se->ending_fpi.line_num = line_no;
                            p_se->ending_fpi.column_num = column_no;

                            ROSE_Fortran_free_format_token_stream_pointer->push_back(p_se);

                            column_no++; 
                            BEGIN NORMAL;
                       }
<FORT_COMMENT>\n       { 
                            //This is also a comment
                            currentBuffer += yytext;
                            //printf("the comment string is %s", currentBuffer.c_str());

                            token_element *p_tok_elem = new token_element;
                            p_tok_elem->token_lexeme = currentBuffer;
                            p_tok_elem->token_id = SgToken::FORTRAN_COMMENTS;

                            stream_element *p_se = new stream_element;
                            p_se->p_tok_elem = p_tok_elem;
                            p_se->beginning_fpi = curr_beginning;
                            p_se->ending_fpi.line_num = line_no;
                            p_se->ending_fpi.column_num = column_no;
                            ROSE_Fortran_free_format_token_stream_pointer->push_back(p_se);

                            line_no++; column_no=1; 
                            BEGIN NORMAL;
                      }
<FORT_COMMENT>.       { 
                            column_no++; 
                            currentBuffer += yytext;
                      }
%%

static const int maxstacksize=500;
static int bracestack[maxstacksize];

static int top=0;
static void pushbracestack(int brace_no) { bracestack[top++]=brace_no; }
// static int topbracestack() { if(top) return bracestack[top-1]; else return -1; }
// static int popbracestack() { return bracestack[--top]; }
// static bool isemptystack() { return top==0; }


static int adjust_new_line_counter()
{
    return 1;
}

static int num_of_newlines(char* s)
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

// This function is called by AttachPreprocessingInfoTreeTrav::evaluateInheritedAttribute()
// function when the input node is a SgFile IR node.
// int getFortranFreeFormatPreprocessorDirectives( std::string fileName )
LexTokenStreamTypePointer
getFortranFreeFormatPreprocessorDirectives( std::string fileName )
   {
     FILE *fp = NULL; 

     ROSE_Fortran_free_format_token_stream_pointer = new LexTokenStreamType;
     assert(ROSE_Fortran_free_format_token_stream_pointer != NULL);

     globalFileName = fileName;

#if 0
     printf ("In getFortranFreeFormatPreprocessorDirectives(): Lexical pass to retrieve the token stream (Opening Free Format Fortran file: %s) \n",fileName.c_str());
#endif

     if(fileName.empty() == false) 
        {
          fp = fopen(fileName.c_str(), "r"); 
          if (fp) 
             { 
            // printf ("opened file %s\n",fileName.c_str()); 
            // exit(0);
               yyin = fp; 
               yylex(); 
               fclose(fp);  
             }
            else 
             {
               printf ("Error: can't find the requested file (%s) \n",fileName.c_str()); 
             }
        }

     assert(ROSE_Fortran_free_format_token_stream_pointer != NULL);
     return ROSE_Fortran_free_format_token_stream_pointer;
   }

void
clean_up_stream()
{
    //
    //This "rudimentary" post processing of the token stream helps in correct identification of keywords.

    //int e g e r 
    //is a sequence of identifiers
    //recognize the token_ids and map them.
    //If the tokens are 


#if 0
    for(SE_ITR ii = ROSE_Fortran_free_format_token_stream_pointer->begin(); ii != ROSE_Fortran_free_format_token_stream_pointer->end(); ii++)
    {
        if((*ii)->p_tok_elem->token_id == SgToken::FORTRAN_COMMENTS)
        { 
            cout<<"## COMMENT -->"; 
            cout<<(*ii)->p_tok_elem->token_lexeme; 
            cout<<"<-- From "<<"("<<(*ii)->beginning_fpi.line_num<<","<<(*ii)->beginning_fpi.column_num<<") to " 
                <<"("<<(*ii)->ending_fpi.line_num<<","<<(*ii)->ending_fpi.column_num<<")";
        }
        else if((*ii)->p_tok_elem->token_id == SgToken::FORTRAN_STRING_LITERALS)
        {
            cout<<"## STRING_LIT -->"; 
            cout<<(*ii)->p_tok_elem->token_lexeme; 
            cout<<"<-- From "<<"("<<(*ii)->beginning_fpi.line_num<<","<<(*ii)->beginning_fpi.column_num<<") to " 
                <<"("<<(*ii)->ending_fpi.line_num<<","<<(*ii)->ending_fpi.column_num<<")";
        }
        else if(((*ii)->p_tok_elem->token_id > 2) && ((*ii)->p_tok_elem->token_id < 67))
        {
            cout<<"## KEYWORD -->"; 
            cout<<(*ii)->p_tok_elem->token_lexeme; 
            cout<<"<-- From "<<"("<<(*ii)->beginning_fpi.line_num<<","<<(*ii)->beginning_fpi.column_num<<") to " 
                <<"("<<(*ii)->ending_fpi.line_num<<","<<(*ii)->ending_fpi.column_num<<")";
        } 
        else if((*ii)->p_tok_elem->token_id == SgToken::FORTRAN_IDENTIFIER)
        {
            cout<<"## IDENTIFIER -->";
            cout<<(*ii)->p_tok_elem->token_lexeme; 
            cout<<"<-- From "<<"("<<(*ii)->beginning_fpi.line_num<<","<<(*ii)->beginning_fpi.column_num<<") to " 
                <<"("<<(*ii)->ending_fpi.line_num<<","<<(*ii)->ending_fpi.column_num<<")";
        } 
        else
        {
            cout<<"## ????Unrecognized element in the token stream##"; 
        }
        cout<<"\n"; 
 
    }
#endif
}

#if 0
int main(int argc, char *argv[])
{
    if(argc == 1) 
    {   //The "default" for now
        getFortranFreeFormatPreprocessorDirectives("triangle.f90");
    }
    else
    {
        getFortranFreeFormatPreprocessorDirectives(argv[1]);
    }

    clean_up_stream();

    printf("*****************here is the stream *************\n"); 
    for(SE_ITR ii = ROSE_Fortran_free_format_token_stream.begin(); ii != ROSE_Fortran_free_format_token_stream.end(); ii++)
    {
        if((*ii)->p_tok_elem->token_id == SgToken::FORTRAN_COMMENTS)
        { 
            cout<<"## COMMENT -->"; 
            cout<<(*ii)->p_tok_elem->token_lexeme; 
            cout<<"<-- From "<<"("<<(*ii)->beginning_fpi.line_num<<","<<(*ii)->beginning_fpi.column_num<<") to " 
                <<"("<<(*ii)->ending_fpi.line_num<<","<<(*ii)->ending_fpi.column_num<<")";
        }
        else if((*ii)->p_tok_elem->token_id == SgToken::FORTRAN_STRING_LITERALS)
        {
            cout<<"## STRING_LIT -->"; 
            cout<<(*ii)->p_tok_elem->token_lexeme; 
            cout<<"<-- From "<<"("<<(*ii)->beginning_fpi.line_num<<","<<(*ii)->beginning_fpi.column_num<<") to " 
                <<"("<<(*ii)->ending_fpi.line_num<<","<<(*ii)->ending_fpi.column_num<<")";
        }
        else if(((*ii)->p_tok_elem->token_id > 2) && ((*ii)->p_tok_elem->token_id < 67))
        {
            cout<<"## KEYWORD -->"; 
            cout<<(*ii)->p_tok_elem->token_lexeme; 
            cout<<"<-- From "<<"("<<(*ii)->beginning_fpi.line_num<<","<<(*ii)->beginning_fpi.column_num<<") to " 
                <<"("<<(*ii)->ending_fpi.line_num<<","<<(*ii)->ending_fpi.column_num<<")";
        } 
        else if((*ii)->p_tok_elem->token_id == SgToken::FORTRAN_IDENTIFIER)
        {
            cout<<"## IDENTIFIER -->";
            cout<<(*ii)->p_tok_elem->token_lexeme; 
            cout<<"<-- From "<<"("<<(*ii)->beginning_fpi.line_num<<","<<(*ii)->beginning_fpi.column_num<<") to " 
                <<"("<<(*ii)->ending_fpi.line_num<<","<<(*ii)->ending_fpi.column_num<<")";
        } 
        else
        {
            cout<<"## ????Unrecognized element in the token stream##"; 
        }
        cout<<"\n"; 
 
    }
    printf("*****************the stream is over*************\n"); 

    return 1;
}

#endif
// }//This ends the namespace Rose_Fortran_free_format_namespace

