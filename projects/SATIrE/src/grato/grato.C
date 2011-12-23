// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: grato.C,v 1.3 2008-08-23 13:46:57 gergo Exp $

// Author: Markus Schordan

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <cstring>
#include <fstream>

//#include "parser_decls.h"

class GGrammar;
#include "GProperty.h"
#include "GTextTransformation.h"
#include "GSequenceVisitor.h"
#include "GCocoBackEnd.h"
#include "GDotBackEnd.h"

using namespace std;



unsigned int transformationrulenumber=0;
string auxiliaryNonTerminalPrefix="Aux";
string auxiliaryNonTerminalPostfix="";

extern void printRegisteredAuxRules();
extern GGrammar* synParse(void);

void print_help_text(char* command_name) {
  cout << command_name << " <grammarfile> [-trdph]" << endl;
	 cout << "  -t : text output" << endl;
	 cout << "  -r<num> : apply transformation rule number <num> (requires -t)" << endl;
	 cout << "     num=0: default"<< endl;
	 cout << "     num=1: introduce auxiliary rule and non-terminal"<< endl;
	 cout << "     num=2: introduce base-type class"<< endl;

	 cout << "  -d : dot output" << endl;
	 cout << "  -p : check properties" << endl;
	 cout << "  -v : visitor" << endl;
	 cout << "  -h : help" << endl;
	 cout << "  --help    : print help" << endl;
	 cout << "  --version : print version text" << endl;
}

int main(int argc, char **argv)
{
  char *input_filename = NULL;
  const char *output_filename = "a.c";

  bool dotoutput=false;
  bool textoutput=false;
  bool checkproperties=false;
  bool runvisitor=false;
  bool cocooutput=false;

#if 1
  int i; 
  for(i=1;i<argc;i++) {
	if(!strcmp(argv[i],"--version")) {
	  cout << argv[0] << " (SATIrE) 0.1" << endl;
	  exit(0);
	}
	if(!strcmp(argv[i],"--help")) {
	  print_help_text(argv[0]);
	  exit(0);
	}
    if (*argv[i]=='-') {
       switch(*(argv[i]+1)) {
       /* output option */
       case 'o': 
          output_filename=argv[i]+2;
          break;
       case 'd': 
	 dotoutput=true;
	 break;
       case 't': 
	 textoutput=true;
	 break;
       case 'p':
	 checkproperties=true;
	 break;
       case 'r':
	 transformationrulenumber=*(argv[i]+2)-'0';
	 break;
       case 'h': 
		 print_help_text(argv[0]);
		 exit(0);
	 break;

       case 'v':
	 runvisitor=true;
	 break;
       case 'c': 
	 cocooutput=true;
	 break;
       /********************************/
       /* specify your own option here */ 
       /********************************/
            
       default:
          fprintf(stderr,"%s: unknown argument option\n",argv[0]); 
          exit(1);
       }
    } else {
      if (input_filename != NULL) {
         fprintf(stderr,"%s: only one input file allowed\n",argv[0]); 
         exit(1);
      }
      input_filename = argv[i];
    }
 }

 if (input_filename != NULL) {
    if ((freopen(input_filename, "r",stdin))==NULL) {
      fprintf(stderr,"%s: cannot open input file %s\n",argv[0],input_filename); 
      exit(1);
    }
 } else {
   //fprintf(stderr,"error: no input file specified.\n"); 
   //exit(1);
 }

 //int parseok=!synparse();
 GGrammar* gresult=synParse();
 if(gresult) {
   GTextBackEnd textBackEnd;
   GDotBackEnd dotBackEnd;
   GProperty property;
   //std::cout << "Parsing succeeded. " << gresult << " "<< std::endl;
   property.generate(gresult);

   switch(transformationrulenumber) {
   case 0:
     break;
   case 1:
   case 2:
     {
       GTextTransformation textTransformation;
       textTransformation.transform(gresult);
     }
     break;
   default:
     cout << "Unknown transformation rule number: " << transformationrulenumber << endl;exit(1);
   } // end switch

   if(textoutput) {
     textBackEnd.generate(gresult);
   }

   if(dotoutput) {
     dotBackEnd.generate(gresult);
   }
   if(checkproperties) {
     cout << "Intersection(NonTerminals,Terminals):";
     property.printSet(property.intersectionNonTerminalsAndTerminals());
     cout << "Unused children:";
     property.printSet(property.unusedChildrenNonTerminals());
     //     cout << "Nonterminals:";
     //property.printSet(property.nonTerminals);
     //cout << "Terminals:";
     //property.printSet(property.terminals);
     property.printStatistics();
   }
   if(runvisitor) {
     GVisitor v;
     gresult->accept(v);
     cout << "Visitor finished." << endl;
     cout << "Show Sequence:" << endl;
     GSequenceVisitor seqV;
     gresult->accept(seqV);
     cout << seqV.toString();
     cout << "Sequence finished." << endl;
     {
     GBAVisitor v;
     gresult->baaccept(v);
     cout << "BAVisitor finished." << endl;
     cout << "Show Sequence:" << endl;
     GEnhancedTokenStreamVisitor seqV;
     gresult->baaccept(seqV);
     cout << seqV.toString();
     cout << "Sequence finished." << endl;
     }
   }
   if(cocooutput) {
     GCocoBackEnd cocoBackEnd;
     cocoBackEnd.generate(gresult);
   }

 } else {
   std::cerr << "Parsing failed." << std::endl;
 }
#endif
}
