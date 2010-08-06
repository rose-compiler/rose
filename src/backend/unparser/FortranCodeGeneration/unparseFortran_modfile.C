#include "sage3basic.h"
#include "unparser_opt.h"
#include "unparser.h"
#include "unparseFortran_modfile.h"

using namespace std;

string 
get_rmod_dir(SgFile* sfile)
{
  vector<string> args = sfile->get_originalCommandLineArgumentList();
  string  rmodDir;

  if (CommandlineProcessing::isOptionWithParameter(args,"-outputdir","",rmodDir,true)==true) 
       return rmodDir+"/";
  else return "";
}


void
generateModFile(SgFile *sfile)
   {
     ROSE_ASSERT(sfile != NULL);

//FMZ (10/28/2009): don't generate the .rmod for the readin .rmod file
if (sfile->get_skipfinalCompileStep() == true)
       return;


  // file name, with full path.
     string  originalModuleFilenameWithPath = sfile->get_file_info()->get_filenameString();

  // Cause the output of a message with verbose level is turned on.
     if (SgProject::get_verbose() > 0)
        {
          printf ("In generateModFile(): Generating a Fortran 90 specific module (*.rmod file) for file = %s \n",originalModuleFilenameWithPath.c_str());
        }

  // Get the list of SgModuleStatement objects for the current AST.
     Rose_STL_Container<SgNode*> moduleDeclarationList = NodeQuery::querySubTree (sfile,V_SgModuleStatement);

  // DQ: I think this case is not required since the loop (below) would be empty.
     if (moduleDeclarationList.empty()) { //no module in the file
         return ;
     }

     for (Rose_STL_Container<SgNode*>::iterator i = moduleDeclarationList.begin(); i != moduleDeclarationList.end(); i++)
        {
       // For a module named "xx" generate a file "xx.rose_mod" which contains 
       // all the variable definitions and function declarations 
          SgModuleStatement* module_stmt = isSgModuleStatement(*i);

          ROSE_ASSERT(module_stmt != NULL);
string outputDir = get_rmod_dir(sfile);
string outputFilename;
if (outputDir !="")
          outputFilename =outputDir + module_stmt->get_name() + MOD_FILE_SUFFIX;
else
          outputFilename = module_stmt->get_name() + MOD_FILE_SUFFIX;

          string lowerCaseOutputFilename = StringUtility::convertToLowerCase(outputFilename);

       // Cause the output of a message with verbose level is turned on.
          if (SgProject::get_verbose() > 0)
             {
               printf ("In generateModFile() (loop over module declarations): Generating a Fortran 90 specific module file %s for module = %s \n",lowerCaseOutputFilename.c_str(),outputFilename.c_str());
             }

       // Use a lower case generate filename for the generated ROSE mod (or rmod) file. 
       // fstream Module_OutputFile(outputFilename.c_str(),ios::out);
          fstream Module_OutputFile(lowerCaseOutputFilename.c_str(),ios::out);

          if (!Module_OutputFile) {
             cout << "Error detected in opening file " << lowerCaseOutputFilename.c_str()
                  << "for output" << endl;
             ROSE_ASSERT(false);
             }

       // Output header at the top of the generate *.rmod file.
          Module_OutputFile <<  endl
               << "! =================================================================================== \n"
               << "! <<Automatically generated for Rose Fortran Separate Compilation, DO NOT MODIFY IT>> \n"
               << "! =================================================================================== \n"
               << endl;
          SgUnparse_Info ninfo;

          ninfo.set_current_scope((SgScopeStatement*)module_stmt);

          ninfo.set_SkipFormatting();

       // set the flag bit "outputFortranModFile" 
          ninfo.set_outputFortranModFile();

          ostringstream outputString;
          Unparser_Opt options(false, false,false,false,true,false,false,false,false,false);

       // This is a confusing use of originalModuleFilename vs. outputFilename (Oh, the first one has the full path!).
       // The originalModuleFilename will be used to build a FortranCodeGeneration_locatedNode using
       // the originalModuleFilename as a basis.
       // printf ("originalModuleFilenameWithPath = %s outputFilename = %s \n",originalModuleFilenameWithPath.c_str(),outputFilename.c_str());
          Unparser unp(&Module_OutputFile, originalModuleFilenameWithPath,options,NULL,NULL);

       // The outputFilename is the name that will be matched against in the selection of statements to unparse.
       // However, that its suffix is ".rmod" will cause UnparseLanguageIndependentConstructs::statementFromFile() 
       // to always return true.  So use of outputFilename should map to the file from the file constructed.
          FortranCodeGeneration_locatedNode myunp(&unp, outputFilename);

       // This calls the unparser for just the module declaration.
          myunp.unparseClassDeclStmt_module((SgStatement*)module_stmt,(SgUnparse_Info&)ninfo);

          Module_OutputFile.flush();
          Module_OutputFile.close();
        }
   }
