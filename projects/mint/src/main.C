

/*
  Author: Didem Unat 
  Contact: mintprogmodel@gmail.com
           dunat@cs.ucsd.edu
	   baden@cs.ucsd.edu

  Date Created       : 31 Oct 2009
  Major Modifications: July 6 2010
                       Feb  9 2011
		       Oct 20 2011
 
 This is a source to source compiler for the Mint Programming Model

  Mint to CUDA translator 
  * input  : Mint source code (C with Mint annotations )    
  * output : Cuda source code

*/  

#include "rose.h"

#include "./midend/MintCudaMidend.h"
#include "./midend/mintTools/MintOptions.h"
#include "./midend/arrayProcessing/MintArrayInterface.h"
#include "./midend/mintTools/MintTools.h"

using namespace std;


int main(int argc, char * argv[])
{

  //this adds -rose:openmp:ast_only -linearize args
  MintOptions::setInternalCommandLineArgs(argc, argv);
  
  vector<string> argvList(argv, argv + argc);

  MintOptions::GetInstance()->SetMintOptions(argvList);

  //isCFileNameSuffix ();

  //call the front end with -rose:openmp:ast_only and
  //then call the lowering explicitly

  SgProject *project = frontend (argc, argv);

  //currently we only handle single input file 
  SgSourceFile* file = isSgSourceFile(project->get_fileList()[0]);
  
  MintTools::setOutputFileName(isSgFile(file));

  //insert cuda header files. is there any? 
  MintCudaMidend::insertHeaders(project);

  //Now we have the AST and search for Mint pragmas
  MintCudaMidend::lowerMinttoCuda(file);

  if(MintOptions::GetInstance()->linearize()){
    cout << "  INFO:Mint: flattening the array accesses" << endl;  
    MintArrayInterface::linearizeArrays(file);
  }


  cout << "  INFO:Mint: calling ROSE backend" << endl;
  project->unparse();
  //return backend(project);

  cout << endl <<"  INFO:Mint: Good Job. Translation is done!" << endl << endl;

  return 0;
}


