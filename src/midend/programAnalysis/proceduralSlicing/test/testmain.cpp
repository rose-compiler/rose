#include "rose.h"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif



#include "GlobalIndexing.h"
#include "InsertPragma.h"

#include "Slicing.h"

#include <stdio.h>
#include <stdlib.h>



int main(int argc, char *argv[]){

  SgProject* sgproject_original = frontend(argc,argv);
  SgProject* sgproject = frontend(argc, argv);

  SgFile &file = sgproject_original->get_file(0);

  // Travering the sgproject to find the number of statements.  
  GlobalIndexing g;
  g.set_global_index(0);
  g.traverse(sgproject, preorder);
  list<SgNode*> list_of_statements = g.get_list_of_statements();
  int no_statements = list_of_statements.size();
  
  
  char file_w_pragmas[256]; 
  char sliced_file[256];
  char move[256];
  char compile[256];
  char run[256];
  char diff[256];
  char out_original[256];
  char out_sliced[256];
  char diff_file[] = "diff_file";
  char slice[256];
  char rm[256];
  char *filename;

  // For all the statements:
  // - insert pragma declarations and printf statements. Pragma declarations define the
  //   slicing criterion
  // - compile and run unsliced code
  // - run the slicing application to slice the code according to the slicing criterion
  // - compile and run the sliced code
  // - compare the output of the two applications: Should be identical!

  for(int i=1; i<=no_statements; i++){
    // need to set the global attributes for each copy of the sgproject (for all the other
    // for-loop traversals apart from the first one...)
    if(i!=1){    
      g.set_global_index(0);
      g.traverse(sgproject, preorder);
    }
    
    InsertPragma p;
    p.set_statement(1);
    p.traverseInputFiles(sgproject, preorder);
    
    sgproject->unparse();
    filename =file.get_unparse_output_filename();
    
    // move the file name to a new name
    sprintf(file_w_pragmas,"%d_%s",i,filename);
    sprintf(move, "mv %s %s", filename, file_w_pragmas);
    
    if(system(move)){
      cerr << "Failed moving the file" << filename << endl;
      return -1;
      }
    
    //Compile and run output file before slicing
    sprintf(compile,"g++ %s", file_w_pragmas);
      if(system(compile)){
      cerr << "failed compiling " << file_w_pragmas << endl; return -1;
    }
    sprintf(out_original,"%s.out", file_w_pragmas);
    sprintf(run,"./a.out > %s",out_original);
    if(system(run)){
      cerr << "failed running the compiled file of "<< file_w_pragmas << endl; return -1;
      }

    // Do slicing
    //Slicing::completeSlice(sgproject);  // this doesn't work.. :(
    sprintf(slice, "./slice %s 1", file_w_pragmas);  // number 1 since we are doing the whole slice...
    //fprintf(stderr, "executing ... [%s]\n", slice);
    if(system(slice)){
      cerr << "slicing of " << file_w_pragmas << " failed." << endl; return -1;
    }
    // move the output file to a name with prefix "sliced_"
    sprintf(sliced_file,"sliced_%s", file_w_pragmas);
    sprintf(move, "mv rose_%s %s", file_w_pragmas, sliced_file);
    if(system(move)){
      cerr << "Failed moving the file to the filename" << file_w_pragmas << endl; return -1;
    }
    // Compile and run output file after slicing
    sprintf(compile, "g++ %s", sliced_file);
    if(system(compile)){
      cerr << "Failed compiling" << endl; return -1;
    }
    sprintf(out_sliced, "%s.out", sliced_file);
    sprintf(run,"./a.out | tee %s",out_sliced);
    if(system(run)){
      cerr << "running sliced code failed." << endl;
      return -1;
    }
 
     // Compare the two output files. They should be identical! diff_file should be empty
    sprintf(diff,"diff %s %s | tee %s", out_sliced, out_original, diff_file);
    if(system(diff)){
      cerr << "running diff on the " << out_sliced << " and " << out_original << " failed." << endl;
      return -1;
    }
    char c;
    ifstream diff_stream(diff_file);
    diff_stream >> c;  // Read/extracts the first character of the file
    if(!diff_stream.eof()){
      cerr << "\nThe two files " << out_sliced << " and " << out_original
           << " are not identical." << endl;
      return -1;
    }
    else{
      cout << "\nThe two files are identical." << endl;
    }
    
    // Delete the output files
    sprintf(rm, "rm %s %s", out_sliced, out_original);
    if(system(rm)){
      cerr <<"Removing the files " << out_sliced << " and " << out_original << " failed." << endl;
      return -1;
      }
    // Delete the produced files
    sprintf(rm, "rm %s %s %s", file_w_pragmas,sliced_file, diff_file);
    if(system(rm)){
      cerr << "Removing the files " << file_w_pragmas << ", " << sliced_file  << " and "
           << diff_file << " failed." << endl;
      return -1;
      }

    // get a clean sage project again...
    sgproject = frontend(argc,argv);
    
  }
  
  cout <<"Slicing every statement in the file " <<  file.getFileName() << " finished OK." << endl;  
  
  return 0;
}

