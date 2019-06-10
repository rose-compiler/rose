%{
#include <stdio.h>
#include "y.tab.h"
%}

%%
open                    return OPEN;
close                   return CLOSE;
export                  return EXPORT;
suite                   return TESTSUITE;
test                    return TESTCASE;
exit                    return EXIT;
[0-9]+                  {
                          yylval.m_num = atoi(yytext);
                          return NUM;
                        }
[a-zA-Z_][a-zA-Z_0-9]*  {
                          yylval.m_str = strdup(yytext);
                          return IDENT;
                        }
[a-zA-Z0-9\/.-]+        {
                          yylval.m_str = strdup(yytext);
                          return STRING;
                        }

\n                      /* ignore end of line */;
[ \t]+                  /* ignore whitespace */;
%%
