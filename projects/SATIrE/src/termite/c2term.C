/* -*- C++ -*-
Copyright 2006 Christoph Bonitz <christoph.bonitz@gmail.com>
          2007-2009 Adrian Prantl <adrian@complang.tuwien.ac.at>

 * Purpose: create a TERMITE representation of a given AST
 */

#include <iostream>
#include <fstream>
#include <satire_rose.h>
#include <getopt.h>

// GB (2009-02-25): We want to build c2term without ICFG stuff to avoid
// having to link against libsatire. Also, want to avoid PAG DFI stuff.
#define HAVE_SATIRE_ICFG 0
#undef HAVE_PAG
#include "TermPrinter.h"

using namespace std;

void usage(const char* me) 
{
  cerr << "Usage: " << me
     << " [FRONTEND OPTIONS] [OPTIONS] src1.c src2.cpp ... [-o termfile.pl]\n"
     << "  Parse one or more source files and convert them into a TERMITE file."
       << "\n  Header files will be included in the term representation.\n\n"

       << "Options:\n"
       << "  [FRONTENT OPTIONS] will be passed to the C/C++ frontend.\n\n" 

       << "  -o, --output <termfile.pl>\n"
       << "    Write the output to <termifile.pl> instead of stdout.\n\n"

       << "  --dot\n"
       << "    Create a dotty graph of the syntax tree.\n\n"

       << "  --pdf\n"
       << "    Create a PDF printout of the syntax tree."
       << endl;
}

int main(int argc, char** argv) {
  const char* outfile = NULL;
  int dot_flag = 0;
  int pdf_flag = 0;

  while (1) {
    static struct option long_options[] = {
      /* These options set a flag. */
      {"dot", no_argument, &dot_flag, 1},
      {"pdf", no_argument, &pdf_flag, 1},
      {"output", required_argument, 0, 'o'},
      {0, 0, 0, 0}
    };
    /* getopt_long stores the option index here. */
    int option_index = 0;
     
    int c = getopt_long(argc, argv, "o:", long_options, &option_index);
     
    /* Detect the end of the options. */
    if (c == -1)
      break;
     
    switch (c) {
    case 'o': outfile = optarg; break;
     
    default:
      usage(argv[0]);
      return 1; 
    }
  }
  if (optind < argc) {
    //infile = argv[optind];
  } else {
    usage(argv[0]);
    return 1;
  }

  // Turn off the frontend's warnings; they are distracting in the
  // automated test outputs.
  vector<char*> argv1;
  char warningOpt[] = "-edg:w";
  argv1.push_back(argv[0]);
  argv1.push_back(warningOpt);
  for (int i = 1; i < argc; ++i)
    argv1.push_back(argv[i]);


  // Run the EDG frontend
  SgProject* project = frontend(argc+1,&argv1[0]);

  if (dot_flag) {
    //  Create dot and pdf files
    //  DOT generation (numbering:preoder)
    AstDOTGeneration dotgen;
    dotgen.generateInputFiles(project, AstDOTGeneration::PREORDER);
  }
  if (pdf_flag) {
    //  PDF generation
    AstPDFGeneration pdfgen;
    pdfgen.generateInputFiles(project);
  }

  init_termite(argc, argv);
  
  // Create prolog term
  BasicTermPrinter tp;
  tp.traverse(project); // With headers

  PrologTerm* genTerm = tp.getTerm();
  
  if (outfile) {
    ofstream ofile(outfile);
    ofile << genTerm->getRepresentation() << "." << endl;
    ofile.close();
  } else cout << genTerm->getRepresentation() << "." << endl;
		
  return 0;
}
