#if 0

#include "sage3basic.h"

#include "templateSupport.h"

#include <fstream>

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

#error "DEAD CODE"

// DQ (10/11/2007): This is no longer used!


// DQ (9/6/2005): Make these static so that we can make sure they are only used in this file
static
bool fileExists ( string filename )
   {
     bool found = false;
     ifstream inputFile;
     inputFile.open( filename.c_str(), ios::binary );
     found = inputFile.good();
     inputFile.close();

  // system("ls -l");
  // printf ("File %s: %s \n",filename.c_str(),(found) ? "FOUND" : "NOT FOUND");

     return found;
   }

// DQ (9/6/2005): Make these static so that we can make sure they are only used in this file
static
string
getCurrentDirectory()
   {
     int i = 0;  // index variable declaration

     const int maxPathNameLength = 1024;
     char* currentDirectory = new char [maxPathNameLength];
     for (i=0; i < maxPathNameLength; i++)
          currentDirectory[i] = '\0';  // set to NULL string

     return getcwd(currentDirectory,maxPathNameLength);
   }

void
buildTemplateInstantiationSupportFile( SgProject* project )
   {
  // DQ (7/12/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("AST Object Code Generation (buildTemplateInstantiationSupportFile):");

  // DQ (9/6/2005): I think these operations have been superseded
  // by the AST post processing mechanism which is more complete.
  // this was previously called in:
  //      SgProject* frontend (int argc, char** argv); 
  // located in ROSE/src/roseSupport/utility_functions.C.
     printf ("In buildTemplateInstantiationSupportFile(): I think this is now dead code! \n");
     ROSE_ASSERT(false);

  // After frontend processing with EDG, if there are templates, a template 
  // instantiation file (*.ti) is built.  We have to modify that file
  // with additional information that will be used by the EDG pre_link 
  // utility (EDG/util/edg_prelink).  The prelinking phase will look
  // at the object files produced by the compilation of each source file and
  // assign template instantiations to each file.  The template instantiations 
  // are generated when the assigned file is recompiled.  This function
  // modifies the *.ti file produced and rewrites it, if it exists, to include 
  // information required to recompile the asociated source file.

  // Search the current directory for any *.ti file that are 
  // associated with the files in this project. Modify the *.ti files.

     string currentDirectory = getCurrentDirectory();

#if 0
  // See what files are present going into the template support
     string lsCurrentDir = string("ls -l ") + currentDirectory;
     printf ("See what files are present going into the template support \n");
     system (lsCurrentDir.c_str());
#endif

  // get list of source files and find associated *.ti files
     Rose_STL_Container<string> sourceFilenameList = project->get_sourceFileNameList();
     Rose_STL_Container<string>::iterator fileIndex = sourceFilenameList.begin();
     int fileCounter = 0;

  // DQ (7/22/2004): This prevents a translator from being called with no commandline options (which should work)
  // DQ (3/17/2004): I assume that there is at least one file!
  // ROSE_ASSERT(sourceFilenameList.size() > 0);

     for (fileIndex = sourceFilenameList.begin(); fileIndex != sourceFilenameList.end(); fileIndex++)
        {
          string filenameWithPath = *fileIndex;
          string filename         = rose::utility_stripPathFromFileName(filenameWithPath.c_str());

          int filenameLength = filename.length();
       // printf ("In buildTemplateInstantiationSupportFile: filename = %s \n",filename.c_str());

          string nameWithoutSuffix = filename.substr(0,filenameLength-2);
          string templateInstantiationFile = nameWithoutSuffix + ".ti";

       // printf ("In buildTemplateInstantiationSupportFile: templateInstantiationFile = %s \n",templateInstantiationFile.c_str());

          if (fileExists(templateInstantiationFile) == true)
             {
            // printf ("Found templateInstantiationFile = %s \n",templateInstantiationFile.c_str());
            // system("ls -l");

            // Read the filename.ti file (where the filename excludes the "rose_" prefix.
               string templateData = StringUtility::readFile(templateInstantiationFile);
            // static void  writeFile ( outputString, fileNameString, directoryName );
            // printf ("templateData (size = %" PRIuPTR ") from %s = %s \n",templateData.size(),templateInstantiationFile.c_str(),templateData.c_str());

            // DQ (5/16/2005): Always put the commandline information into the *.ti files even when they don't reference symbols,
            // the prelink mechanism requires it.

            // if (templateData.size() > 0)
               if (true)
                  {
#if 0
                 // Remove the original *.ti file generated by EDG
                    remove(templateInstantiationFile.c_str());
#endif
                 // string modifiedTemplateInstantiationFile = string("rose_") + templateInstantiationFile;
                 // string newTemplateDataComment = "Modified Template Instantiation File \n";
                 // newTemplateData  = newTemplateDataComment;

                 // Add "-rose:prelink" option to command line so that we avoid recursive calls to the prelink mechanism
                 // string edgCommandLine   = project->get_file(fileCounter).get_savedFrontendCommandLine() + "-rose:prelink";
                    string edgCommandLine   = project->get_file(fileCounter).get_savedFrontendCommandLine();

                 // DQ (5/19/2005): Only add the "-rose:prelink" option if we are not already in the prelink phase
                    if (project->get_prelink() == false)
                       {
                         edgCommandLine   += string("-rose:prelink");
                       }

                 // File names supporting where templates will be instatiated
                 // This approach gets the names correct but does not instantiate templates correctly!
                 // printf ("Non-working naming scheme (builds templates wrong, but names the files correctly) \n");

                    string pathInFilename   = rose::getPathFromFileName(filenameWithPath.c_str());
                 // string newFileName = "rose_" + filename;
                 // string modifiedTemplateInstantiationFile = string("rose_") + templateInstantiationFile;
                    string modifiedTemplateInstantiationFile;
                    string newFileName;
                    if (project->get_prelink() == true)
                       {
                      // templateInstantiationFile already has "rose_" prefix so don't add it again!
#ifndef USE_ORIGINAL_SOURCE_FILE_NAME_IN_PRELINKING
                         ROSE_ASSERT(templateInstantiationFile.find("rose_") != string::npos);
#endif
                         modifiedTemplateInstantiationFile = templateInstantiationFile;
                         newFileName = filename;
                       }
                      else
                       {
                      // If we use the original file name test.C then the EDG will be looking for the test.ii 
                      // file when recompiling during prelinking, then forces us to use test.ti as a filename 
                      // when setting up the template information file (the "*.ti" file).
#ifdef USE_ORIGINAL_SOURCE_FILE_NAME_IN_PRELINKING
                         newFileName = filename;
                         modifiedTemplateInstantiationFile = templateInstantiationFile;
#else
                         modifiedTemplateInstantiationFile = string("rose_") + templateInstantiationFile;
                         newFileName = "rose_" + filename;
#endif
                       }

                 // printf ("edgCommandLine = %s \n",edgCommandLine.c_str());

                    string newTemplateData;
                    newTemplateData += string("cmd:") + edgCommandLine + "\n";
                    newTemplateData += string("dir:") + currentDirectory + "\n";

#ifdef USE_ORIGINAL_SOURCE_FILE_NAME_IN_PRELINKING
                 // DQ (5/16/2005): Now that we have a lot of stuff debugged, try again to use the original file 
                 // instead of the ROSE generated souce file.
                 // DQ (5/12/2005): Only the original source file should be recompiled, not the ROSE generated souce file.
                 // DQ (5/13/2005): This works to a point (see below).
                    newTemplateData += string("fnm:") + filenameWithPath + "\n";
#else
                 // DQ (5/13/2005): If we want to avoid the repeated specification of undefined symbols to be instatiated 
                 // (in the definition_list_file used by the compiler (front-end) to communicated with the pre-linker), 
                 // then we have to recompile the evolving file (which will have any new templates instatiated after each 
                 // prelink recompile).  However then means that names used in the prelink mechanism might have to be 
                 // constructed differently (to exclude any additional "rose_" prefix).
                    newTemplateData += string("fnm:") + currentDirectory + "/" + newFileName + "\n";
#endif

                    newTemplateData += templateData;

                 // Remaining possible arguments to ti file: 
                 //      idn, ifn, stu, tnm, dep (documentation in EDG manual, page 381)

                 // printf ("In buildTemplateInstantiationSupportFile: newTemplateData = \n%s \n",newTemplateData.c_str());

                    string dir = currentDirectory + "/";
                 // printf ("Modified file written to %s%s \n",dir.c_str(),modifiedTemplateInstantiationFile.c_str());
                    StringUtility::writeFile(newTemplateData.c_str(),modifiedTemplateInstantiationFile.c_str(),dir.c_str());

                    if (project->get_prelink() == false)
                       {
#ifndef USE_ORIGINAL_SOURCE_FILE_NAME_IN_PRELINKING
                      // Remove the EDG generated *.ti file (since it has the wrong name, without the rose prefix)
                      // printf ("Removing the original EDG generated file (%s) \n",templateInstantiationFile.c_str());
                         remove(templateInstantiationFile.c_str());
#endif
                       }
                      else
                       {
                      // if prelinking then these names are the same (which is why we can't remove the file)
                         ROSE_ASSERT(templateInstantiationFile == modifiedTemplateInstantiationFile);
                       }
                  }
                 else
                  {
                 // DQ (5/16/2005): I previously removed this file!
                 // printf ("An empty *.ti file was generated by the frontend, leaving it in place (in case prelinker needs it) \n");
                 // remove(templateInstantiationFile.c_str());
                  }
             }
            else
             {
            // printf ("Note: %s not found (no templates support required) \n",templateInstantiationFile.c_str());
            // printf ("ERROR: Could NOT find templateInstantiationFile = %s \n",templateInstantiationFile.c_str());
            // ROSE_ASSERT (1 == 2);
             }

       // printf ("Bottom of loop over files in SgProject \n");
       // ROSE_ASSERT (1 == 2);
        }

   // printf ("Bottom of buildTemplateInstantiationSupportFile() \n");
   // ROSE_ASSERT (1 == 2);
   }
#endif

#if 0
// DQ (10/11/2007): This is no longer used!

void
instantiateTemplates ( SgProject* project )
   {
  // DQ (7/12/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("AST Object Code Generation (instantiateTemplates): time (sec) = ");

  // DQ (9/6/2005): I think these operations have been superseded
  // by the AST post processing mechanism which is more complete.
     printf ("In instantiateTemplates(): I think this may be dead code! \n");
     ROSE_ASSERT(false);

  // After compilation we can start the process of template instantiation
  //    1) Compiling to object files (building *.ti files)
  //         a) build *.ti files (call instantiateTemplates() function)
  //    2) Linking
  //         a) call prelink utility
  //              * builds instantiation information (*.ii files)
  //              * calls compilation step to force instantiation (using info in *.ii files)
  //              * calls prelink utility iteratively
  //         b) call g++ to do final linking with all templates resolved

     ROSE_ASSERT (project != NULL);
  // printf ("In instantiateTemplates(project = %p) \n",project);

#if 1
  // ***********************
  // Calling prelink utility
  // ***********************
  // printf ("Calling prelink utility ... \n");

     Rose_STL_Container<string> sourceFilenameList = project->get_sourceFileNameList();

     ROSE_ASSERT (sourceFilenameList.size() <= 1);
     if ( project->get_prelink() == false )
        {
          Rose_STL_Container<string> objectFilenameList = project->get_objectFileNameList();

          string prelinkOptions = "-v -d9 -i ";
          string objectFiles;
          Rose_STL_Container<string> argList;
          if ( (sourceFilenameList.size() == 1) && (objectFilenameList.size() == 0) )
             {
               printf ("Handling (debugging) single file case of template instantiation \n");

               Rose_STL_Container<string>::iterator fileIndex = sourceFilenameList.begin();
               ROSE_ASSERT (sourceFilenameList.size() == 1);
               string filenameWithPath = *fileIndex;
               string filename         = rose::utility_stripPathFromFileName(filenameWithPath.c_str());

               int filenameLength = filename.length();
               printf ("In instantiateTemplates(): filename = %s \n",filename.c_str());

            // This would not handle a foo.cpp file name (with ".cpp" as a suffix), I think
               string nameWithoutSuffix = filename.substr(0,filenameLength-2);
#if 0
               objectFiles = nameWithoutSuffix + ".o";
#else
            // Make the prelink phase operate on a rose_<file name>.o file and fix 
            // the object file name generated by the vendor compiler (to generate 
            // the same file name).
               objectFiles = string("rose_") + nameWithoutSuffix + ".o";
#endif
               printf ("In instantiateTemplates(): objectFiles = %s \n",objectFiles.c_str());

            // DQ (9/25/2007): This is the first element to be put into the list, so we can use push_back() inplace of push_front().
            // argList.push_front(objectFiles);
               argList.push_back(objectFiles);
             }
            else
             {
            // linking only (this processing is different than that of a single file)
            // Debug the single file case before addressing multiple files required 
            // for link only processing.

               printf ("Linking only, template instantiation stil being debugged (similar to single file case) \n");
            // ROSE_ASSERT (false);

               objectFiles = CommandlineProcessing::generateStringFromArgList(project->get_objectFileNameList());
               argList = project->get_objectFileNameList();
             }

       // DQ (1/13/2005):
       // We could check to make sure there are valid *.ti (template instantiation files) associated with the objectFiles
       // if not then calling "edg_prelink" will cause an error.  But it is not required!  It is not an error for edg_prelink
       // to not find a corresponding *.ti file for each object file!
       // printf ("Check for corresponding template instantiation files: objectFiles = %s \n",objectFiles.c_str());

       // Build the command line to execute the edg_prelink utility 
       // (user's path must include location of edg_prelink utility).
          string prelinkCommandLine = "edg_prelink " + prelinkOptions + objectFiles;

#if 0
          printf ("\n\n");
          printf ("######################################### \n");
          printf ("######################################### \n");
          printf ("######################################### \n");
          printf ("prelinkCommandLine = %s \n",prelinkCommandLine.c_str());
          printf ("######################################### \n");
          printf ("######################################### \n");
          printf ("######################################### \n");
          printf ("\n\n");

          printf ("Print out rose_test2005_74.C (before prelink) \n");
          system("cat /home/dquinlan2/ROSE/LINUX-3.3.2/developersScratchSpace/Dan/rose_test2005_74.C");
#endif

          int argc    = 0;
          char** argv = NULL;

       // Add the commandline parameters as separate strings (in reverse order)
       // argList.push_front(" -i ");
       // argList.push_front(" -d9 ");
       // argList.push_front(" -a 0 ");

       // DQ (9/25/2007): Move from std::list to std::vector.
       // argList.push_front(" -v ");
          argList.insert(argList.begin()," -v ");

       // Separate list out into form traditional argc and argv form parameters
          CommandlineProcessing::generateArgcArgvFromList (argList,argc,argv);

          printf ("Calling generateArgcArgvFromList: argc = %d compressed argList = %s \n",
               argc,CommandlineProcessing::generateStringFromArgList(argList).c_str());

       // Declaration of prelink_main() function
          int prelink_main(int argc, char *argv[]);

          printf ("############### Before call to prelink_main ... ################ \n");

       // Use original interface (from when it was a command line utility!
          prelink_main(argc,argv);

          printf ("############### After call to prelink_main ... ################ \n");
       // ROSE_ASSERT(false);
        }
       else
        {
       // printf ("############### Prelink option specified (exited compilation!) ################ \n");
        }

  // printf ("Print out rose_test2004_18.C (after prelink) \n");
  // system("cat /home/dquinlan2/ROSE/LINUX-3.3.2/dqDevelopmentDirectory/rose_test2004_18.C");

#else
     printf ("Skipping calls to iterative prelink utility ... \n");
#endif

#if 0
     printf ("Exiting after handling newly instantiated code! \n");
     ROSE_ASSERT (false);
#endif
   }
#endif


#if 0
// DQ (10/11/2007): This is no longer used!

void
fixupInstantiatedTemplates ( SgProject* project )
   {
  // DQ (7/12/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("AST Object Code Generation (fixupInstantiatedTemplates): time (sec) = ");

  // DQ (9/6/2005): I think these operations have been superseded
  // by the AST post processing mechanism which is more complete.
     printf ("In fixupInstantiatedTemplates(): I think this may be dead code! \n");
     ROSE_ASSERT(false);

  // This must be done prior to the unparsing of the SAGE III AST, so that any transformations
  // (new function prototypes for the specialized template function) can be inserted before we 
  // unparse the final code!  Could be done in AST fixup!  Not clear if it should be done in
  // the EDG/SAGE III connection!  It is currently called from the backend() function just 
  // before the unparser!

  // Add forward references for instantiated template functions and member functions 
  // (which are by default defined at the bottom of the file (but should be declared 
  // at the top once we know what instantiations should be built)).  They must be 
  // defined at the bottom since they could call other functions not yet declared in 
  // the file.  Note that this fixup is required since we have skipped the class template 
  // definitions which would contain the declarations that we are generating.  We might 
  // need that as a solution at some point if this fails to be sufficently robust.

  // Build a lists of intatiatied templates
     Rose_STL_Container<SgNode*> classList          = NodeQuery::querySubTree (project,V_SgTemplateInstantiationDecl);
     Rose_STL_Container<SgNode*> functionList       = NodeQuery::querySubTree (project,V_SgTemplateInstantiationFunctionDecl);
     Rose_STL_Container<SgNode*> memberFunctionList = NodeQuery::querySubTree (project,V_SgTemplateInstantiationMemberFunctionDecl);

#if 1
     printf ("In fixupInstantiatedTemplates SgTemplateInstantiationDecl:               classList.size()          = %ld \n",classList.size());
     printf ("In fixupInstantiatedTemplates SgTemplateInstantiationFunctionDecl:       functionList.size()       = %ld \n",functionList.size());
     printf ("In fixupInstantiatedTemplates SgTemplateInstantiationMemberFunctionDecl: memberFunctionList.size() = %ld \n",memberFunctionList.size());
#endif

  // These are not handled yet!
  // ROSE_ASSERT(classList.size()    == 0);
  // ROSE_ASSERT(functionList.size() == 0);

     Rose_STL_Container<SgNode*>::iterator functionIndex = functionList.begin();
     while ( functionIndex != functionList.end() )
        {
       // SgDeclarationStatement* declaration = isSgDeclarationStatement(*i);
          SgTemplateInstantiationFunctionDecl* templateInstantiationFunctionDeclaration = 
               isSgTemplateInstantiationFunctionDecl(*functionIndex);
          ROSE_ASSERT (templateInstantiationFunctionDeclaration != NULL);

       // printf ("SgTemplateInstantiationFunctionDecl: *i = %p = %s \n",*functionIndex,(*functionIndex)->unparseToString().c_str());

       // Mark this function as a specialization, since we have transformed it into one when we converted the
       // name format (e.g.from  __A45_ to A<int>).  The handling of templates in ROSE is one of converting
       // the templae instantiations (generated by EDG) into explicit template specializations so that they
       // can be operated upon within ROSE (for translation/optimization).
          printf ("Error in fixupInstantiatedTemplates (function templates): It is now an error to mark something that was not explicitly a specialization in the original source code as a specialization ... \n");
          templateInstantiationFunctionDeclaration->set_specialization(SgTemplateInstantiationFunctionDecl::e_specialization);

          bool generateForwardDeclarationForFunction = (templateInstantiationFunctionDeclaration->isForward() == false);

          if ( generateForwardDeclarationForFunction == true )
             {
            // This is just a regular member function inside of a templated class (or a class nested in a templated class)
            // it's associated template declaration will not have the template text (appears to be lost in EDG), but it 
            // is not essential if we unparse the instantated template for the member function.

            // This is the instantiate template member function definition, the steps are:
            //    1) Build a forward declaration for the instantiated template (member function).
            //    2) Place the new forward declaration after the class containing the templated member function.
             }

          functionIndex++;
        }

     Rose_STL_Container<SgNode*>::iterator classIndex = classList.begin();
     while ( classIndex != classList.end() )
        {
          SgTemplateInstantiationDecl* templateInstantiationClassDeclaration = 
               isSgTemplateInstantiationDecl(*classIndex);
          ROSE_ASSERT (templateInstantiationClassDeclaration != NULL);

       // printf ("SgTemplateInstantiationDecl: *i = %p = %s \n",*classIndex,(*classIndex)->unparseToString().c_str());

#if 0
          printf ("In fixupInstantiatedTemplates: templateInstantiationClassDeclaration = %p compilerGenerated = %s \n",
                  templateInstantiationClassDeclaration,
                  templateInstantiationClassDeclaration->get_file_info()->isCompilerGenerated() ? "true" : "false");
          templateInstantiationClassDeclaration->get_file_info()->display("In fixupInstantiatedTemplates: templateInstantiationClassDeclaration");
#endif

       // Mark this class as a specialization, since we have transformed it into one when we converted the
       // name format (e.g.from  __A45_ to A<int>).  The handling of templates in ROSE is one of converting
       // the templae instantiations (generated by EDG) into explicit template specializations so that they
       // can be operated upon within ROSE (for translation/optimization).
          printf ("Error in fixupInstantiatedTemplates (class templates): It is now an error to mark something that was not explicitly a specialization in the original source code as a specialization ... \n");
          templateInstantiationClassDeclaration->set_specialization(SgTemplateInstantiationDecl::e_specialization);

          bool generateForwardDeclarationForClass = (templateInstantiationClassDeclaration->isForward() == false);

          if ( generateForwardDeclarationForClass == true )
             {
            // Nothing to do in this case since EDG should have already instered the forward class declaration!
             }

          classIndex++;
        }

  // Loop over the SgTemplateInstantiationMemberFunction objects and insert a function prototype.
  // We need the function prototypes because the template instatiation function definitions appear 
  // at the end of the file!
     Rose_STL_Container<SgNode*>::iterator i = memberFunctionList.begin();
     while ( i != memberFunctionList.end() )
        {
       // SgDeclarationStatement* declaration = isSgDeclarationStatement(*i);
          SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDeclaration = 
               isSgTemplateInstantiationMemberFunctionDecl(*i);
          ROSE_ASSERT (templateInstantiationMemberFunctionDeclaration != NULL);
#if 0
          printf ("templateInstantiationMemberFunctionDeclaration->isTemplateFunction() == %s \n",
               templateInstantiationMemberFunctionDeclaration->isTemplateFunction() ? "true" : "false");
#endif
       // Mark this function as a specialization, since we have transformed it into one when we converted the
       // name formate (e.g.from  __A45_ to A<int>).  The handling of templates in ROSE is one of converting
       // the templae instantiations (generated by EDG) into explicit template specializations so that they
       // can be operated upon within ROSE (for translation/optimization).
       // If this is not a truely templated function then it can't be a specialization!
       // if it is not a templated function then is is likely just a member function of a templated class.
          if (templateInstantiationMemberFunctionDeclaration->isTemplateFunction() == true)
             {
               printf ("Error in fixupInstantiatedTemplates (member function templates): It is now an error to mark something that was not explicitly a specialization in the original source code as a specialization ... \n");
               templateInstantiationMemberFunctionDeclaration->set_specialization(SgTemplateInstantiationMemberFunctionDecl::e_specialization);
             }
#if 0
       // DQ (9/5/2005): Updated comment.
       // DQ (5/31/2005): Commented out since the class declaration itself is the forward declaration 
       // for the member function. I don't think we need another one!  Actually we do but this is now 
       // handled within ROSE/src/frontend/SageIII/astPostProcessing/ files.

          bool generateForwardDeclarationForInlinedMemberFunction = 
               (templateInstantiationMemberFunctionDeclaration->isForward() == false) &&
               (templateInstantiationMemberFunctionDeclaration->isTemplateFunction() == false);

          if ( generateForwardDeclarationForInlinedMemberFunction == true )
             {
            // This is just a regular member function inside of a templated class (or a class nested in a templated class)
            // it's associated template declaration will not have the templae text (appears to be lost in EDG), but it 
            // is not essential if we unparse the instantated template for the member function.

               printf ("For inlined template member functions get the templateDeclaration from the class declaration \n");

            // This is the instantiate template member function definition, the steps are:
            //    1) Build a forward declaration for the instantiated template (member function).
            //    2) Place the new forward declaration after the class containing the templated member function.

               printf ("SgTemplateInstantiationMemberFunctionDecl: *i = %p = %s \n",*i,(*i)->unparseToString().c_str());

            // Call the AST's copy mechanism
               SgShallowCopy shallow;
               SgNode * forwardDeclarationNode = templateInstantiationMemberFunctionDeclaration->copy(shallow);
               SgTemplateInstantiationMemberFunctionDecl* forwardDeclaration = isSgTemplateInstantiationMemberFunctionDecl(forwardDeclarationNode);
               ROSE_ASSERT(forwardDeclaration != NULL);

            // find the template declaration of the class contining the member function
               SgClassDeclaration* classDeclaration = templateInstantiationMemberFunctionDeclaration->get_class_scope()->get_declaration();
               ROSE_ASSERT(classDeclaration != NULL);
               SgTemplateInstantiationDecl* templateInstantiationDeclaration = isSgTemplateInstantiationDecl(classDeclaration);
               ROSE_ASSERT(templateInstantiationDeclaration != NULL);
               SgTemplateDeclaration* templateDeclaration = templateInstantiationDeclaration->get_templateDeclaration();
               ROSE_ASSERT (templateDeclaration != NULL);

            // Reset the file info object so that we can mark this as compiler generated (copy builds a new Sg_File_Info object)
            // ROSE_ASSERT (forwardDeclaration->get_file_info() != NULL);
            // forwardDeclaration->set_file_info(new Sg_File_Info(*(forwardDeclaration->get_file_info())));
               ROSE_ASSERT(forwardDeclaration->get_file_info() != templateInstantiationMemberFunctionDeclaration->get_file_info());

            // Both of these may be set (implemented as bit flags internally)
               forwardDeclaration->get_file_info()->setCompilerGenerated();
               forwardDeclaration->get_file_info()->setTransformation();

            // Remove the shallow copy of the function definition
               forwardDeclaration->set_definition(NULL);

            // Mark the declaration as a forward declarations
               forwardDeclaration->setForward();

            // Mark this function as a specialization (should be done within copy function)
            // forwardDeclaration->set_specialization(SgTemplateInstantiationMemberFunctionDecl::e_specialization);

               ROSE_ASSERT(forwardDeclaration->isSpecialization() == true);
               ROSE_ASSERT(forwardDeclaration->isPartialSpecialization() == false);

            // Now insert the forwardDeclaration after the templateDeclaration!
            // templateDeclaration.insert_statement(forwardDeclaration,true);
               SgScopeStatement* templateDeclarationScope = templateDeclaration->get_scope();
               ROSE_ASSERT (templateDeclarationScope != NULL);
               printf ("BEFORE loop: Insert before: templateDeclarationScope = %p = %s \n",templateDeclarationScope,templateDeclarationScope->sage_class_name());

            // Trace back through the scopes to find a non class declaration scope into which to put the forward declaration
            // Does this then work with nested template classes?????
               while (isSgTemplateInstantiationDefn(templateDeclarationScope) != NULL)
                  {
                    templateDeclarationScope = templateDeclarationScope->get_scope();
                    printf ("In loop templateDeclarationScope = %p = %s \n",templateDeclarationScope,templateDeclarationScope->sage_class_name());
                  }

               ROSE_ASSERT (templateDeclarationScope != NULL);
               printf ("AFTER loop: Insert before: templateDeclarationScope = %p = %s \n",templateDeclarationScope,templateDeclarationScope->sage_class_name());

               templateDeclaration->get_file_info()->display("templateDeclaration");
               templateDeclarationScope->get_file_info()->display("templateDeclarationScope");

               int insertBeforeStatement = false;
            // printf ("Calling templateDeclarationScope->insert_statement() \n");
            // templateDeclaration->insert_statement ( templateDeclaration, forwardDeclaration, insertBeforeStatement );
               SgTemplateInstantiationDefn *templateClassDefinition = isSgTemplateInstantiationDefn(templateDeclarationScope);
               if (templateClassDefinition != NULL)
                  {
                    SgDeclarationStatementPtrList::iterator start = templateClassDefinition->get_members().begin();
                    SgDeclarationStatementPtrList::iterator end   = templateClassDefinition->get_members().end();

                    printf ("templateDeclaration unparsed = %s \n",templateDeclaration->unparseToString().c_str());
                    printf ("templateDeclaration name = %s string = %s \n",
                         templateDeclaration->get_name().str(),templateDeclaration->get_string().str());

                    for (SgDeclarationStatementPtrList::iterator i = start; i != end; i++)
                       {
                         string s = (*i)->unparseToString();
                         printf ("(*i)->unparseToString() = %s \n",s.c_str());
                       }

                    ROSE_ASSERT(find(start,end,templateInstantiationMemberFunctionDeclaration) != end);
                    templateDeclarationScope->insert_statement ( templateInstantiationMemberFunctionDeclaration, forwardDeclaration, insertBeforeStatement );
                  }
                 else
                  {
                 // ROSE_ASSERT(find(templateDeclarationScope->get_members().begin(),templateDeclarationScope->get_members().end(),templateDeclaration) != templateDeclarationScope->get_members().end() );
                    templateDeclarationScope->insert_statement ( templateDeclaration, forwardDeclaration, insertBeforeStatement );
                  }

               printf ("forwardDeclaration = %s \n",forwardDeclaration->unparseToString().c_str());

            // printf ("DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD \n");
            // printf ("Exiting after construction of forward declaration for template instantiation! \n");
            // ROSE_ASSERT (false);
             }
            else
             {
            // This is a forward declaration (does it have the template arguments!)
               printf ("SgTemplateInstantiationMemberFunctionDecl: (forward) *i = %p = %s \n",*i,(*i)->unparseToString().c_str());
             }
#endif
          i++;
        }
   }

#endif
