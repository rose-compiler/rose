/*
 * Parsing pragmas and generating MPI code from input sequential code
 *  
 *  Pragma is OpenMP style, reusing OmpAttribute to store information
 *  As a experiments, a lightweight recursive descendent parser is used to parse the pragmas
 *  Liao 9/22/2015
 * */
#ifndef MPI_Code_Generator_h 
#define MPI_Code_Generator_h

namespace MPI_CODE_GENERATOR {
  
  int generateMPI (SgSourceFile* sfile); 
}


#endif //MPI_Code_Generator_h
