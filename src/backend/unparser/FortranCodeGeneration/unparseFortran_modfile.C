#include "rose.h"
#include "unparseFortran_modfile.h"

using namespace std;

void
generateModFile(SgFile *sfile)
  {
     ROSE_ASSERT(sfile != NULL);

     // file name
     string  sfilename = sfile->get_file_info()->get_filenameString();

     Rose_STL_Container<SgNode*> moduleDeclarationList = NodeQuery::querySubTree (sfile,V_SgModuleStatement);

     if (moduleDeclarationList.empty()) { //no module in the file
         return ;
     }

     for (Rose_STL_Container<SgNode*>::iterator i = moduleDeclarationList.begin(); i != moduleDeclarationList.end(); i++) {
        // For a module named "xx" generate a file "xx.rose_mod" which contains 
        // all the variable definitions and function declarations 
        SgModuleStatement* module_stmt = isSgModuleStatement(*i);

        ROSE_ASSERT(module_stmt != NULL);

        string outputFilename = module_stmt->get_name() + MOD_FILE_SUFFIX;

        fstream Module_OutputFile(outputFilename.c_str(),ios::out);

        if (!Module_OutputFile) {
           cout << "Error detected in opening file " << outputFilename.c_str()
                << "for output" << endl;
           ROSE_ASSERT(false);
           }

        Module_OutputFile <<  endl
             << "! =================================================================================== \n"
             << "! <<Automatically generated for Rose Fortran Separate Compilation, DO NOT MODIFY IT>> \n"
             << "! =================================================================================== \n"
             <<endl;
        SgUnparse_Info ninfo;

        ninfo.set_current_scope((SgScopeStatement*)module_stmt);

        ninfo.set_SkipFormatting();
         
        // set the flag bit "outputFortranModFile" 
        ninfo.set_outputFortranModFile();

        ostringstream outputString;
        Unparser_Opt options(false, false,false,false,true,
                             false,false,false,false,false);

        Unparser unp(&Module_OutputFile, sfilename,options,NULL,NULL);

        FortranCodeGeneration_locatedNode myunp(&unp, outputFilename);

        myunp.unparseClassDeclStmt_module((SgStatement*)module_stmt,(SgUnparse_Info&)ninfo);

        Module_OutputFile.flush();
        Module_OutputFile.close();

     }
     return ;
}
