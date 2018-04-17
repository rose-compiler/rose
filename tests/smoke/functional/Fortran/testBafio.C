#include "aterm1.h"
#include "rose_config.h"
#include <string>
#include <stdio.h>

int main(int argc, char **argv){
   ATerm bottomOfStack;
   ATerm table;
   FILE * file;

   ATinit(argc, argv, &bottomOfStack);
  	
   std::string path_to_table = OFP_BIN_PATH;
   path_to_table += "/Fortran.tbl";

   file = fopen(path_to_table.c_str(), "rb");
   if (!file) {
      printf("ERROR: in opening parse table in test ./tests/smoke/functional/Fortran/testBafio, path is %s\n", path_to_table.c_str());
      ATerror("cannot open parse table, Fortran.tbl");
   }

   table = ATreadFromBinaryFile(file);
   if (!table) {
      printf("ERROR: in reading parse table in test ./tests/smoke/functional/Fortran/testBafio, path is %s\n", path_to_table.c_str());
      ATerror("cannot read parse table, Fortran.tbl");
   }

   return 0;
}
