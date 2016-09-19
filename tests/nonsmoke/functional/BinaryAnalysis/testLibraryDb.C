
// This is now included in libraryIdentification.h
// #include "functionIdentification.h"

#include "rose.h"

// At some point this will be put into rose.h.
#include "libraryIdentification.h"

using namespace   LibraryIdentification;


int main()
{

  FunctionIdentification ident("test.db");


  std::string testString = "test instr seq" ;
  {
    std::cerr << "Testing insert " << std::endl;
    library_handle handle;

    handle.filename= "foo";
    handle.function_name="bar";
    handle.begin=1;
    handle.end=10;
    ident.set_function_match(handle,testString);

  }

  {
    std::cerr << "Testing get " << std::endl;

    library_handle handle;

    if( !ident.get_function_match(handle,testString) )
    {
      std::cerr << "Function not found. Exiting." << std::endl;
      exit(1);
    }
      
      ;
    std::cout << "Found function " << handle.filename << " " << handle.function_name 
      << " " << handle.begin << " " << handle.end << std::endl;
  }



};
