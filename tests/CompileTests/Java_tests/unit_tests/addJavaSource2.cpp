// Same as addJavaSource.cpp except with a file

#include <iostream>
#include "rose.h"
#include "ecj.h"
using namespace Rose::Frontend::Java::Ecj;
using namespace SageBuilder;
using namespace SageInterface;
using namespace std;

/**
 * Example of how to manipulate a Java Ast in ROSE.
 */
int main(int argc, char **argv) {
    SgProject *project = frontend(argc, argv); // Run the input example and create an initial AST

    setSourcePositionClassificationMode(e_sourcePositionFrontendConstruction);

    //
    // Allocate a temporary directory to store newly created info.
    //
    string directory_name = getTempDirectory(project); // get a temporary directory.

    //
    // Create a new package "p".
    //
    string packageName = "Number_Handling.CWE_190";
    SgClassDefinition *p_package_definition = findOrInsertJavaPackage(project, packageName,
                                                                      true /* create associated directory if package p is
                                                                            * inserted */); 
    ROSE_ASSERT(p_package_definition);
    string package_directory_name = p_package_definition -> get_qualified_name().getString();
    replace(package_directory_name.begin(), package_directory_name.end(), '.', '/');

    // Load some source code
    std::string snippetFileName = "./snippets/Number_Handling/CWE_190/CWE_190_0.java";
    MemoryMap::BufferPtr buffer = MemoryMap::ByteBuffer::create_from_file(snippetFileName);
    char *charBuf = new char[buffer->size()];
    buffer->read(charBuf, 0, buffer->size());
    std::string sourceCode(charBuf, buffer->size());
    std::cerr <<"loaded this source code:\n" <<sourceCode <<"\n";
    std::string unqualifiedClassName = "CWE_190_0";

    //
    // Create a new class in package
    //
    preprocessCompilationUnit(project,
                              package_directory_name + "/" + unqualifiedClassName,
                              sourceCode);

    setSourcePositionClassificationMode(e_sourcePositionTransformation);
    AstTests::runAllTests(project);

    int return_code = backend(project); // invoke backend compiler to generate object/binary files

    destroyTempDirectory(directory_name); // Get rid of the temporary directory and all its included temporary files.

    return return_code;
}
