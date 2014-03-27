#include "traversal.h"
#include "OFPUnparser.hpp"
#include "UntypedASTBuilder.hpp"
//#include "OFPNodes.h"

#include <stdlib.h>
#include <stdio.h>

ATbool ofp_traverse_init();

int main(int argc, char * argv[])
{
   OFP::Program Program;
   OFP::UntypedASTBuilder   ast;
   OFP::FortranTextUnparser unparser;
   
   if (argc < 2) {
      fprintf(stderr, "usage: traverse filename\n");
      exit(-1);
   }

   FILE * file = fopen(argv[1], "r");
   if (file == NULL) {
      fprintf(stderr, "Failed to open file\n");
      exit(-1);
   }

   ATinitialize(argc, argv); 

   if (ofp_traverse_init() != ATtrue) exit(1);

   Program.term = ATreadFromTextFile(file);

   printf("\n%s\n\n", ATwriteToString(Program.term));

   OFP::setASTBuilder(&ast);
   OFP::setUnparser(&unparser);

   if (ofp_traverse_Program(Program.term, &Program)) {
      printf("\nWoot!\n");
   } else return 1;

   printf("\n\n----------------------------\n");
   unparser.unparseNode(Program.getPayload());
   printf("----------------------------\n\n");

   return 0;
}
