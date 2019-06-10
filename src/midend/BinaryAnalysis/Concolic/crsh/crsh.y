%{
#include <stdio.h>
#include <string.h>

#include "crsh.h"

void yyerror(const char *str)
{
  fprintf(stderr,"error: %s\n",str);
}

int yywrap()
{
  return 1;
}

%}

%error-verbose
%start prog

%union {
  int   m_num;
  char* m_str;
}

%token OPEN
%token EXIT
%token RUN
%token SPECIMEN
%token TESTCASE
%token TESTSUITE
%token <m_int> NUM
%token <m_str> IDENT
%token <m_str> STRING

%type <int>    num
%type <char*>  filename ident



%start crush

%%
filename         : STRING                    { $$ = $1; }
                 ;

src              : filename
                 ;

ident            : IDENT;                    { $$ = $1; }

string           : STRING                    { $$ = $1; }
              /* | '"' words_opt '"';        { $$ = $2; } */
                 ;

export           : EXPORT ident '=' string   { crsh_envvar($2, $4); }

environ_opt      : /* empty */
                 | environ_opt export
                 ;

argument         : string                    { crsh_cmdlarg($1); }
                 ;

arguments_opt    : /* empty */
                 | argument arguments_opt
                 ;

testcmd          : specimen arguments_opt    { crsh_invoke($1); }
                 ;

testcase         : TESTCASE ident '{'        { crsh_test($2); }
                     environ_opt
                     testcmd
                   '}'                       { crsh_test_end(); }
                 ;

testcases_opt    : /* empty */
                 | testcase testcases_opt
                 ;

testsuite_cmd    : TESTSUITE ident '{'       { crsh_suite($2); }
                     testcases_opt
                   '}'                       { crsh_suite_end(); }
                 ;

testsuite_opt    : /* empty */               { $$ = NULL; }
                 | ident                     { $$ = $1; }
                 ;

num_opt          : /* empty */               { $$ = -1; }
                 | num                       { $$ = $1; }
                 ;

exec_cmd         : RUN testsuite_opt num_opt { crsh_run($2, $3); }
                 ;

open_cmd         : OPEN filename             { crsh_db($2); }
                 ;

exit_cmd         : EXIT
                 ;

cmd              : testsuite_cmds
                 | exec_cmd
                 | open_cmd
                 | exit_cmd
                 ;

cmds_opt         : /* empty */
                 | cmd cmds_opt
                 ;

crush            : cmds_opt
                 ;
