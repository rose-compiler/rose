/*
 * DOT file format parser
 */

%{
#include "parse_header.h"
#include <sstream>
#include <queue>
#include <cstring>
using namespace std;

extern ofstream GML;
std::string edge_str, crt_options;
unsigned long buf_len = 0;
int yyerror (char *s);
int yylex (void);
int crt_subgraph = 1, in_edge_stmt = 0;
std::queue<std::string> edges;
std::string graphics;
char *buf;
%}

%union {
  int iVal;
  char cVal;
  char *sVal;
};

%token <sVal> STRING UNDS VNULL NAME
%token <sVal> ID
%token <cVal> LBRACE RBRACE
%type <sVal> graph stmt_list graph_name stmt node_stmt edge_stmt label_list option_stmt

%token LBRK RBRK LBRACE RBRACE ASSGN LABEL LARROW STRING DIGRAPH COLUMN
%token SHAPE SUBGRAPH COLOR NONE BOTH DIAMOND DIR

%start input

%%
input:          {cout << "STARTED\n";} graph {cout << "ENDED\n";};

graph:          DIGRAPH graph_name LBRACE
                {
                  GML << "graph [\n\tlabel \"" << $2 << "\"";
                }
                // write nodes, gather edges to the end of the file
                stmt_list RBRACE
                {
                  buf = (char *)malloc(buf_len);
                  if (!(buf > 0))
                  {
                    cerr << "ERROR: could not allocate memory for the edges!";
                  }
                  while (!edges.empty())
                  {
                    strcat(buf, edges.front().c_str());
                    edges.pop();
                  }
                  GML << buf << "\n]\n";
                }

graph_name:     STRING

stmt_list:      
                | stmt stmt_list

stmt:           subgraph_stmt 
                | node_stmt COLUMN
                | edge_stmt COLUMN
                | option_stmt COLUMN

subgraph_stmt:  subgraph
                | subgraph subgraph_stmt

subgraph:       SUBGRAPH NAME LBRACE
                {
                  crt_subgraph++;
                }
                stmt_list RBRACE

node_stmt:      ID
                {
                  GML << "\n\tnode [\n\t\tid " << $1; // << "\"";
                }
                label_list
                {
                  GML << "\n\t]";
                }

edge_stmt:      ID LARROW ID
                {
                  in_edge_stmt = 1;
                  edge_str = "\n\tedge [\n\t\tsource " + (std::string)$1 + "\n\t\ttarget " + (std::string)$3;
                }
                label_list
                {
                  edge_str += "\n\t]";
                  edges.push(edge_str);
                  buf_len += edge_str.length();
                  in_edge_stmt = 0;
                }

label_list:     LBRK LABEL ASSGN ID
                {
                  // write node labels and graphical options, save edge labels and options
                  if (in_edge_stmt)
                    edge_str += "\n\t\tlabel \"" + (std::string)$4 + "\"";
                  else
                    GML << "\n\t\tlabel \"" << $4 << "\"";
                  crt_options = "\n\t\tgraphics [";
                }
                option_list
                {
                  // all nodes of the subgraph have the same color
                  std::ostringstream cl;
                  cl << crt_subgraph;
                  crt_options += "\n\t\t\tfill " + cl.str() + "\n\t\t]";

                  // save the options for edges, write the options for nodes
                  if (in_edge_stmt)
                    edge_str += crt_options;
                  else
                    GML << crt_options;
                }
                RBRK
                | LBRK LABEL ASSGN STRING
                {
                  if (in_edge_stmt)
                  {
                    std::string tmp = $4;
                    edge_str += "\n\t\tlabel \"" + tmp  + "\"";
                  }
                  else
                    GML << "\n\t\tlabel \"" << $4 << "\"";
                  crt_options = "\n\t\tgraphics [";
                }
                option_list
                {
                  std::ostringstream cl;
                  cl << crt_subgraph;
                  crt_options += "\n\t\t\tfill " + cl.str() + "\n\t\t]";

                  if (in_edge_stmt)
                    edge_str += crt_options;
                  else
                    GML << crt_options;
                }
                RBRK

option_list:    
                | option option_list

option:         DIR ASSGN NONE
                {
                  crt_options += "\n\t\t\tarrow \"none\"";
                }
                | DIR ASSGN BOTH
                {
                  crt_options += "\n\t\t\tarrow \"both\"";
                }
                | SHAPE ASSGN DIAMOND
                {
                  crt_options += "\n\t\t\ttype \"oval\"";
                }

                // TODO: label and color are now only associated with the whole graph...
                //... not with the nodes or inner graphs (which don't exist yet)
option_stmt:    LABEL ASSGN STRING
                {
                  GML << "\n\tLabelGraphics [\n\t\ttype \"" << $3 << "\"\n\t]";
                }
                | COLOR ASSGN NAME
                {
                  GML << "\n\tgraphics [\n\t\toutline \"" << $3 << "\"\n\t]";
                }
%%

int yyerror(char* s)
{
  extern int yylineno;
  extern char *yytext;
  
  cerr << "ERROR: " << s << " at symbol \"" << yytext;
  cerr << "\" on line " << yylineno << endl;

  exit(1);
}
