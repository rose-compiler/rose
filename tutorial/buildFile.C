/*! \brief  Demonstrate how to build a new file
*/
#include "rose.h"
#include <iostream>

using namespace std;

using namespace SageInterface;
using namespace SageBuilder;

int main (int argc, char *argv[])
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

  // Read in the input program (construct initial AST).
     SgProject *project = frontend (argc, argv);

  // Build the SgFile, will be a SgSourceFile which is derived from SgFile.
  // If the input file is not present in the current directory then the input file is ignored.
  // It is a bit strange that a filename is required (empty string is not allowed), it also
  // appears that the language specification for the file is taken from the suffix of the 
  // input file. This can be fixed later.
#if 1
     string source_suffix = ".C";
     string newFilename_output = "new_source_file";
     string newFilename_input  = "temp_dummy_file_" + newFilename_output;

  // This current API requires an input filename, the language selection is done based on the input filename.
  // the input file will be constructed as an empty file in the current working directory.
     SgFile* file = buildFile(newFilename_input+source_suffix,newFilename_output+source_suffix,project);
     ROSE_ASSERT(file != NULL);

     SgSourceFile* sourceFile = isSgSourceFile(file);
#else
  // New API function to simplify new file construction, this hides the defails of the input filename handling.
     SgSourceFile* sourceFile = buildSourceFile("my_header_out.C",project);
#endif
     ROSE_ASSERT(sourceFile != NULL);

  // There should now be 2 SgFile objects in the SgProject.
     ROSE_ASSERT(project->get_fileList().size() == 2);

#if 0
  // This will also call display for each SgFile in the project.
     project->display("project");
#endif

  // Find the pointer to the global scope.
     SgGlobal* globalScopeOfNewFile = sourceFile->get_globalScope();
     ROSE_ASSERT(globalScopeOfNewFile != NULL);

  // Build a new function in the global scope (specify the name).
     string functionName = "foobar";

  // Build the new function's return type.
     SgType* return_type = buildVoidType();

  // Build a simple empty function parameter list.
     SgFunctionParameterList* parameter_list = buildFunctionParameterList();

  // Note that specification of scope determins the scope of the function and not where it could be located (must be explicitly added to the target scope).
     SgFunctionDeclaration* functionDeclaration = buildDefiningFunctionDeclaration(functionName,return_type,parameter_list,globalScopeOfNewFile);
     ROSE_ASSERT(functionDeclaration != NULL);

  // Add the function declearation to the target scope (global scope).
     SageInterface::appendStatement(functionDeclaration,globalScopeOfNewFile);

#if 0
  // This function is specified as a transformation, so it will be unparsed.
     functionDeclaration->get_file_info()->display("function declaration");
#endif

  // Find the associated nondefining declaration to unparse seperately as a function prototype in a header file.
     SgFunctionDeclaration* nondefiningFunctionDeclaration = isSgFunctionDeclaration(functionDeclaration->get_firstNondefiningDeclaration());
     ROSE_ASSERT(nondefiningFunctionDeclaration != NULL);

  // Generate the string for the function prototype.
     string functionPrototypeString = nondefiningFunctionDeclaration->unparseToString();
     printf ("functionPrototypeString = %s \n",functionPrototypeString.c_str());

  // Generate the header file and output the function prototype into the header file.
     string header_suffix = ".h";
     string headerFileName = newFilename_output + header_suffix;
     ofstream outputfile(headerFileName.c_str(),ios::out); 
     outputfile << "// Output for generated header file" << endl;
     outputfile << functionPrototypeString << endl;
     outputfile.close();

  // Run all diabnostics tests on the generated AST.
     AstTests::runAllTests(project);

#if 1
  // Output an optional graph of the AST (just the tree, when active)
     generateDOT(*project);

  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 10000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"");
#endif

  // Outout the unparsed code from the input AST.
     project->unparse();
   }

