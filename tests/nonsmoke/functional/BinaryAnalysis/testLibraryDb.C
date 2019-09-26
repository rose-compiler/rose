#include "conditionalDisable.h"
#ifdef ROSE_BINARY_TEST_DISABLED
#include <iostream>
int main() { std::cout <<"disabled for " <<ROSE_BINARY_TEST_DISABLED <<"\n"; return 1; }
#else


// This is now included in libraryIdentification.h
// #include "functionIdentification.h"

#include "rose.h"

// At some point this will be put into rose.h.
#include "libraryIdentification.h"

using namespace   LibraryIdentification;


int main()
{

  FunctionIdDatabaseInterface ident("test.db");
  
  std::cerr << "Testing insert library Alexandria" << std::endl;
  //ISA with 1 on the high bit to test sign extension
  LibraryInfo inLibInfo("Axexandria", "1.0", "libhashlibhash", "x86" ); 

  ident.addLibraryToDB(inLibInfo);
  
  std::cerr << "Testing insert function foo" << std::endl;
  FunctionInfo inFuncInfo("foo", "hashhashhashhash", inLibInfo.libHash);
  
  ident.addFunctionToDB(inFuncInfo);
  

  
  std::cerr << "Testing get function foo" << std::endl;
  
  FunctionInfo outFuncInfo("hashhashhashhash");
  
  if( !ident.matchOneFunction(outFuncInfo) )  //TODO: Also test exactMatchFunction?
      {
          std::cerr << "Function not found. Exiting." << std::endl;
          exit(1);
      } 
  
  std::cout << "Found function " << outFuncInfo.funcName << std::endl;
  
  LibraryInfo outLibInfo(outFuncInfo.libHash);
  
  if( !ident.matchLibrary(outLibInfo) )
      {
      std::cerr << "Library not found. Exiting." << std::endl;
      exit(1);
      } 
  
  std::cout << "Found library " << outLibInfo.libName << " : " << outLibInfo.libVersion << std::endl;


  //Now check that all the data made it in and out of the database
  //successfully (hashes don't need to be checked, they were checked
  //via the lookup.)
  if(inLibInfo.libName != outLibInfo.libName) 
      {
          std::cerr << "ERROR: Names put into the database don't match those pulled out." << std::endl;
          std::cerr << "       in: " << inLibInfo.libName << " out: " << outLibInfo.libName <<  std::endl;
          exit(1);
      }
  
  if(inLibInfo.libVersion != outLibInfo.libVersion) 
      {
          std::cerr << "ERROR: Versions put into the database don't match those pulled out." << std::endl;
          std::cerr << "       in: " << inLibInfo.libVersion << " out: " << outLibInfo.libVersion << std::endl;
          exit(1);
      }
  
  if(inLibInfo.architecture != outLibInfo.architecture) 
      {
          std::cerr << "ERROR: Versions put into the database don't match those pulled out." << std::endl;
          std::cerr << "       in: " << inLibInfo.architecture << " out: " << outLibInfo.architecture << std::endl;
          exit(1);
      }

  if(inLibInfo.analysisTime != outLibInfo.analysisTime) 
      {
          std::cerr << "ERROR: Versions put into the database don't match those pulled out." << std::endl;
          std::cerr << "       in: " << inLibInfo.analysisTime << " out: " << outLibInfo.analysisTime << std::endl;
          exit(1);
      } 

  if(inFuncInfo.funcName != outFuncInfo.funcName) 
      {
          std::cerr << "ERROR: Names put into the database don't match those pulled out." << std::endl;
          std::cerr << "       in: " << inFuncInfo.funcName << " out: " << outFuncInfo.funcName <<  std::endl;
          exit(1);
      }

};

#endif
