/*
 * ROSE Fortran Analysis Example
 *
 * This example demonstrates using ROSE to analyze Fortran source code.
 *
 * Note: This program is only built if ROSE was configured with Fortran support.
 */

#ifndef HAVE_ROSE_FORTRAN
#error "This example requires ROSE with Fortran support"
#endif

#include <rose.h>
#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "ROSE Fortran Analyzer" << std::endl;
    std::cout << "ROSE Version: " << ROSE_PACKAGE_VERSION << std::endl;
    std::cout << "=====================" << std::endl;

    // Check for input files
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <fortran_file.f90>" << std::endl;
        std::cerr << "Example: " << argv[0] << " program.f90" << std::endl;
        return 1;
    }

    // Initialize ROSE frontend
    SgProject* project = frontend(argc, argv);

    if (!project) {
        std::cerr << "Error: Failed to parse Fortran file(s)" << std::endl;
        return 1;
    }

    std::cout << "Successfully parsed Fortran file(s)" << std::endl;
    std::cout << "Number of files: " << project->numberOfFiles() << std::endl;

    // Iterate over all files in the project
    SgFilePtrList files = project->get_fileList();
    for (SgFile* file : files) {
        std::cout << "\nFile: " << file->getFileName() << std::endl;

        // Get the source file (contains the AST root)
        SgSourceFile* sourceFile = isSgSourceFile(file);
        if (sourceFile) {
            SgGlobal* globalScope = sourceFile->get_globalScope();

            // Count declarations
            SgDeclarationStatementPtrList decls = globalScope->get_declarations();
            std::cout << "  Top-level declarations: " << decls.size() << std::endl;

            // Count Fortran program units
            Rose_STL_Container<SgNode*> programs = NodeQuery::querySubTree(sourceFile, V_SgProgramHeaderStatement);
            std::cout << "  Fortran PROGRAM units: " << programs.size() << std::endl;

            // Count modules
            Rose_STL_Container<SgNode*> modules = NodeQuery::querySubTree(sourceFile, V_SgModuleStatement);
            std::cout << "  Fortran MODULE units: " << modules.size() << std::endl;

            // Count subroutines and functions
            Rose_STL_Container<SgNode*> subroutines = NodeQuery::querySubTree(sourceFile, V_SgProcedureHeaderStatement);
            std::cout << "  Fortran SUBROUTINE/FUNCTION units: " << subroutines.size() << std::endl;

            // List subroutine/function names
            for (SgNode* node : subroutines) {
                SgProcedureHeaderStatement* procHeader = isSgProcedureHeaderStatement(node);
                if (procHeader) {
                    std::cout << "    - " << procHeader->get_name().getString() << std::endl;
                }
            }
        }
    }

    std::cout << "\nFortran analysis complete!" << std::endl;
    return 0;
}
