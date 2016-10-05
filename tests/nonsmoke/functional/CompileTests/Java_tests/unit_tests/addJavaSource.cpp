// Expressions can be built using both bottomup (recommended ) and topdown orders.
// Bottomup: build operands first, operation later
// Topdown: build operation first, set operands later on.

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
    SgClassDefinition *p_package_definition = findOrInsertJavaPackage(project, "p", true /* create associated directory if package p is inserted */);
    ROSE_ASSERT(p_package_definition);

    //
    // Create a new interface I in package p.
    //
    string package_directory_name = p_package_definition -> get_qualified_name().getString();
    replace(package_directory_name.begin(), package_directory_name.end(), '.', '/');
    preprocessCompilationUnit(project,
                              package_directory_name + "/I",
                              (string) "package p;\n" +
                                       "interface I {\n" +
                                       "    int one();\n" +
                                       "}\n"
                             );

    //
    // Create a new class A in package p that implements p.I.
    //
    preprocessCompilationUnit(project,
                              package_directory_name + "/A",
                              (string) "package p;\n" +
                                       "import java.lang.Class;\n" +
                                       "import java.awt.*;\n" +
                                       "import java.io.File;\n" +
                                       "public class A implements I {\n" +
                                       "    public A(java.lang.String x) {\n" +
                                       "        super();\n" +
                                       "    }\n" +
                                       "    public void m(int i) {\n" +
                                       "    }\n" +
                                       "    public static int zero() {\n" +
                                       "        return 0;\n" +
                                       "    }\n" +
                                       "    public int one() {\n" +
                                       "        return 1;\n" +
                                       "    }\n" +
                                       "    final private static int a;\n" +
                                       "    static {\n" +
                                       "        a = 5;\n" +
                                       "    }\n" +
                                       "    public A cheat() {\n" +
                                       "        return new A(\"\") {\n" +
                                       "                   public int one() {\n" +
                                       "                       return 2;\n" +
                                       "                   }\n" +
                                       "               };\n" +
                                       "    };\n" +
                                       "}\n"
                             );

    //
    // Create a new class Test that uses the new class A.
    //
    preprocessCompilationUnit(project,
                              package_directory_name + "/Test",
                              (string) "package p;\n" +
                                       "class Test {\n" +
                                       "    static String lines(String... lines) { return null; }\n" +
                                       "    static public void main(String args[]) {\n" +
                                       "        String output = lines(\"Once \", \"upon \", \"a \", \"time ...\");\n"+ 
                                       "        A a = new A(\"\");\n" +
                                       "        A b = a.cheat();\n" +
                                       "        System.out.println(\"The value of One is \" + b.one());\n" +
                                       "    }\n" +
                                       "}\n"
                             );

    setSourcePositionClassificationMode(e_sourcePositionTransformation);
    AstTests::runAllTests(project);

    int return_code = backend(project); // invoke backend compiler to generate object/binary files

    destroyTempDirectory(directory_name); // Get rid of the temporary directory and all its included temporary files.

    return return_code;
}
