#include "rose.h"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include "../Slicing.h"  // is this ok?


int main(int argc, char *argv[])
{
   SgProject *sgproject = frontend(argc,argv);
 
  //copying the sgproject so that the code isn't destructive
   // OBS: THIS COPYING HAS NO EFFECT!!!
   // SgProject *sgproject = sgproject_original;  
  ROSE_ASSERT(sgproject != NULL);

  char* filename = sgproject->get_file(0).getFileName(); 
  char* outputname = sgproject->get_file(0).get_unparse_output_filename();
  char* interface;
  if(argv[2] == NULL){
    cout << "Please choose an interface number (1, (2) or 3)." << endl;
    cout << "usage: ./slice inputfile interface_number\n\n" << endl;
    return 0;
  }
  else{
    interface = argv[2];
    //cout << "interface = " << interface << endl; 

    // COMPLETE SLICE
    if(*interface == '1'){  
      //slice the code
      Slicing::completeSlice(sgproject);
      cout << "RESULT OF SLICING:" << endl;
      printf("Slicing of %s went OK. The result is in the file %s.\n\n",filename,outputname);
    }

    // This slicing is slightly meaningless
    else if(*interface == '2'){
      set<SgNode*> stmt;
      Slicing::sliceOnlyStmtWithControl(sgproject, stmt);
      cout << "RESULT OF SLICING;" << endl;
      // meaningless output...? includes return statements...
      set<SgNode*>::const_iterator s;
      for(s=stmt.begin();s!=stmt.end();++s){
        cout <<"    - " << (*s)->unparseToString() << endl;
      }  
      cout << "\n(No rose_filename written. Only the above list is the output.)\n\n" <<endl;
    }

    // ONLY STATEMENTS
    else if(*interface == '3'){
      set<SgNode*> stmt_in_slice;
      Slicing::sliceOnlyStmts(sgproject, stmt_in_slice);
      
      cout << "RESULT OF SLICING:" << endl;
      set<SgNode*>::const_iterator s;
      for(s=stmt_in_slice.begin();s!=stmt_in_slice.end();++s){
        cout <<"    - " << (*s)->unparseToString() << endl;
      }  
      cout << "\n(No rose_filename written. Only the above list is the output.)\n\n" <<endl;
    }
  }



  //  backend(sgproject);  
  /* The reason for not calling backend:
     
  Calling prelink utility ...
  sh: line 1: edg_prelink: command not found
sl  slice: /home/hauge2/ROSE/src/roseSupport/templateSupport.C:236: void instantiateTemplates(SgProject*): Assertion `errorCode == 0' failed.
  */
  //delete sgproject;
  
  return 0;
}
