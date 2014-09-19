#include "traversal.hpp"
#include "OFPUnparser.hpp"
#include "UntypedASTBuilder.hpp"
//#include "OFPNodes.hpp"

#include <stdlib.h>
#include <stdio.h>

ATbool ofp_traverse_init();

int main(int argc, char * argv[])
{
   OFP::Program Program;
   OFP::UntypedASTBuilder ast;
   OFP::FortranTextUnparser * unparser = NULL;
   std::ofstream * ofs = NULL;
   FILE * file = stdin;
   
   for (int i = 1; i < argc; i++) {
      std::string arg = argv[i];
      if (arg == "-i") {
         file = fopen(argv[++i], "r");
         if (file == NULL) {
            fprintf(stderr, "Failed to open file\n");
            exit(-1);
         }
      }
      else if (arg == "-o") {
         ofs = new std::ofstream(argv[++i], std::ios_base::out);
      }
   }

   if (ofs) unparser = new OFP::FortranTextUnparser(*ofs);
   else     unparser = new OFP::FortranTextUnparser();

   ATinitialize(argc, argv); 

   if (ofp_traverse_init() != ATtrue) exit(1);

   Program.term = ATreadFromTextFile(file);

#ifdef OFP_DEBUG
   printf("\n%s\n\n", ATwriteToString(Program.term));
#endif

   OFP::setASTBuilder(&ast);
   OFP::setUnparser(unparser);

   if (ofp_traverse_Program(Program.term, &Program)) {
      if (ofs == NULL) printf("\nWoot!\n");
   } else return 1;

   if (ofs == NULL) printf("----------------------------\n");
   unparser->unparseNode(Program.getPayload());
   if (ofs == NULL) printf("----------------------------\n\n");

   delete unparser;
   if (ofs) delete ofs;

   return 0;
}
