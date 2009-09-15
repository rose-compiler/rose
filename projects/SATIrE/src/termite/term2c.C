/* -*- C++ -*-
Copyright 2006 Christoph Bonitz <christoph.bonitz@gmail.com>
          2007-2008 Adrian Prantl <adrian@complang.tuwien.ac.at>
*/
#include "termite.h"
#include <iostream>
#include <stdio.h>
#include <satire_rose.h>
#include "TermToRose.h"
#include <getopt.h>

using namespace std;

void usage(const char* me) 
{
  cerr << "Usage: " << me
       << " [OPTION]... [FILE.term]\n"
       << "Unparse a term file to its original source representation.\n\n"

       << "Options:\n"
       << "  -o, --output sourcefile.c\n"
       << "    If specified, the contents of all files will be concatenated\n"
       << "    into the sourcefile.\n\n"

       << "  -s, --suffix '.suffix'  Default: '.unparsed'\n"

       << "    Use the original file names with the additional suffix.\n\n"

       << "  -d, --dir DIRECTORY\n"
       << "    Create the unparsed files in DIRECTORY.\n\n"

       << "  --dot\n"
       << "    Create a dotty graph of the syntax tree.\n\n"

       << "  --pdf\n"
       << "    Create a PDF printout of the syntax tree.\n\n"

       << "This program was built against "<<PACKAGE_STRING<<",\n"
       << "please report bugs to "<<PACKAGE_BUGREPORT<<"."

       << endl;

}

int main(int argc, char** argv) {
  //  cout << prote->getRepresentation();
  const char* infile = "";
  const char* outfile= "";
  const char* outdir = ".";
  const char* suffix = ".unparsed";
  int dot_flag = 0;
  int pdf_flag = 0;

  while (1) {
    static struct option long_options[] = {
      /* These options set a flag. */
      {"dot", no_argument, &dot_flag, 1},
      {"pdf", no_argument, &pdf_flag, 1},
      /* These don't */
      {"dir",    required_argument, 0, 'd'},
      {"output", required_argument, 0, 'o'},
      {"suffix", required_argument, 0, 's'},
      {0, 0, 0, 0}
    };
    /* getopt_long stores the option index here. */
    int option_index = 0;
     
    int c = getopt_long(argc, argv, "d:o:s:", long_options, &option_index);
     
    /* Detect the end of the options. */
    if (c == -1)
      break;
     
    switch (c) {
    case 'd': outdir  = optarg; break;
    case 'o': outfile = optarg; break;
    case 's': suffix  = optarg; break;
     
    default:
      usage(argv[0]);
      return 1; 
    }
  }
  if (optind < argc) {
    infile = argv[optind];
  } else {
    usage(argv[0]);
    return 1;
  }

  init_termite(argc, argv);

  TermToRose conv;
  SgNode* p = conv.toRose(infile);

  if (dot_flag) {
    //  Create dot and pdf files
    //  DOT generation (numbering:preoder)
    AstDOTGeneration dotgen;
    dotgen.generateInputFiles((SgProject*)p,AstDOTGeneration::PREORDER);
  }
  if (pdf_flag) {
    //  PDF generation
    AstPDFGeneration pdfgen;
    pdfgen.generateInputFiles((SgProject*)p);
  }
  conv.unparse(outfile, outdir, suffix, p);
  return 0;
}
