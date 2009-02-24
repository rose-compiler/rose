#include "rose.h"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include "Slicing.h"

int main(int argc, char *argv[]) {
  
  SgProject *sgproject = frontend(argc,argv);
  
  // A complete slice
  Slicing::completeSlice(sgproject);
  cout << "\nRESULT: sliced file written to file with prefix \"rose_\"\n" << endl;

  // make a clean project again, since the last call has changed the sgproject/AST
  sgproject = frontend(argc,argv);

  // A partial slice, returning only directly affecting statements
  set<SgNode*> stmt_in_slice;
  Slicing::sliceOnlyStmts(sgproject, stmt_in_slice);
  
  cout << "Statements directly affecting the slicing criterion:" << endl;
  set<SgNode*>::const_iterator s;
  for(s=stmt_in_slice.begin();s!=stmt_in_slice.end();++s){
    cout  << (*s)->unparseToString() << endl;
  }  
  cout<<"(No rose_filename written. Only the above list is the output.)\n\n"<<endl;
  
  return 0;
}
