// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "markBackendCompilerSpecificFunctions.h"
using namespace std;

void
markBackendSpecificFunctionsAsCompilerGenerated( SgNode* node )
   {
  // This simplifies how the traversal is called!
     MarkBackendSpecificFunctionsAsCompilerGenerated astFixupTraversal;

  // printf ("Inside of markBackendSpecificFunctionsAsCompilerGenerated() \n");

  // I think the default should be preorder so that the interfaces would be more uniform
  // astFixupTraversal.traverse(node,preorder);
     astFixupTraversal.traverseMemoryPool();

  // printf ("Leaving markBackendSpecificFunctionsAsCompilerGenerated() \n");
   }

MarkBackendSpecificFunctionsAsCompilerGenerated::MarkBackendSpecificFunctionsAsCompilerGenerated()
   {
     targetFileName = "rose_edg_required_macros_and_functions.h";

  // printf ("In MarkBackendSpecificFunctionsAsCompilerGenerated constructor: targetFileName = %s \n",targetFileName.c_str());

  // targetFile = new Sg_File_Info(targetFileName.c_str());
     targetFile = NULL; // new Sg_File_Info(targetFileName.c_str());
  // ROSE_ASSERT(targetFile != NULL);

  // Build a traversal for us on just the SgInitializedName memory pool so that we can find the filename (with complete path)
  // to the targetFileName and build a Sg_File_Info object which will permit more efficent testing of IR nodes later.
     class FindFileInfo : public ROSE_VisitTraversal
        {
          public:
               Sg_File_Info* targetFileInfo;
               std::string targetFileName;

               FindFileInfo(std::string & filename ) : targetFileInfo(NULL), targetFileName(filename) {};
               virtual ~FindFileInfo() {}

               void visit (SgNode* node)
                  {
                    if (targetFileInfo == NULL)
                       {
                      // printf ("Setup the targetFile Sg_File_Info \n");
                         string rawFileName         = node->get_file_info()->get_raw_filename();
                         string filenameWithoutPath = StringUtility::stripPathFromFileName(rawFileName);

#ifndef USE_ROSE
                      // DQ (3/6/2006): Note that SgGlobal will not have an associated 
                      // filename and so will not trigger the targetFile to be set.
                      // printf ("targetFileName      = %s \n",targetFileName.c_str());
                      // printf ("filenameWithoutPath = %s \n",filenameWithoutPath.c_str());
                         if (filenameWithoutPath == targetFileName)
                            {
                              targetFileInfo = new Sg_File_Info(rawFileName.c_str());
                              ROSE_ASSERT(targetFileInfo != NULL);
                            }
#endif
                       }

                  }
              
        };

     FindFileInfo visitor (targetFileName);
     SgInitializedName::traverseMemoryPoolNodes(visitor);

     targetFile = visitor.targetFileInfo;

  // DQ (12/6/2007): Skip the output of this message for Fortran applications 
  // (since we handle Fortran intrinsit functions more directly in the front-end)
  // if (targetFile == NULL)
     if (targetFile == NULL && ( SageInterface::is_Fortran_language() == false && SageInterface::is_Java_language() == false && SageInterface::is_binary_executable() == false ) )
        {
          printf ("Lookup of Sg_File_Info referencing targetFileName = %s was unsuccessful \n",targetFileName.c_str());
        }
#if 0
       else
        {
          printf ("Found Sg_File_Info for targetFileName = %s \n",targetFileName.c_str());
        }
#endif
     
  // ROSE_ASSERT(targetFile != NULL);
   }

MarkBackendSpecificFunctionsAsCompilerGenerated::~MarkBackendSpecificFunctionsAsCompilerGenerated()
   {
  // This should be true if the file was generated with a modified commandline that includes the
  // edg option to force the rose_edg_required_macros_and_functions.h to be read first.
  // but Qing's mechanism for building some AST fragments does not do this, so it is not always
  // valid.  This might be fixed up later.
  // ROSE_ASSERT(targetFile != NULL);
     delete targetFile;
     targetFile = NULL;
   }

void
MarkBackendSpecificFunctionsAsCompilerGenerated::visit(SgNode* node)
   {
     ROSE_ASSERT(node != NULL);

  // DQ (5/8/2006): This should have been setup bu now!
     if (targetFile == NULL)
        {
       // printf ("Associated Sg_File_Info referencing targetFileName = %s has not been found (skipping test) \n",targetFileName.c_str());
          return;
        }
     ROSE_ASSERT(targetFile != NULL);

  // printf ("In MarkBackendSpecificFunctionsAsCompilerGenerated::visit(%s) \n",node->class_name().c_str());

     Sg_File_Info* fileInfo = node->get_file_info();
     if (fileInfo != NULL)
        {
#if 0
           printf ("From file: file id = %d filename = %s \n",fileInfo->get_file_id(),fileInfo->get_filename());
          if (isSgInitializedName(node) != NULL)
             {
               fileInfo->display("In MarkBackendSpecificFunctionsAsCompilerGenerated::visit(): SgInitializedName debug");
             }
#endif
          if (fileInfo->isSameFile(targetFile) == true)
             {
#if 0
               printf ("Found a node from targetFile->get_filename() = %s (mark it as compiler generated) \n",targetFile->get_filename());
#endif
            // DQ (12/21/2006): Added to support uniformally marking IR nodes with such information.
               SgLocatedNode* locatedNode = isSgLocatedNode(node);
               if (locatedNode != NULL)
                  {
#if 0
                    printf ("Calling Sg_File_Info::setFrontendSpecific() for case #1 \n");
#endif
#if 0
                    targetFile->display("targetFile: debug");
                    fileInfo->display("fileInfo: debug");
#endif
                 // There may be more the mark than a single Sg_File_Info in this case.
                    locatedNode->get_file_info()->setFrontendSpecific();
                    locatedNode->setCompilerGenerated();
                  }
                 else
                  {
#if 0
                    printf ("Calling Sg_File_Info::setFrontendSpecific() for case #2 \n");
#endif
                 // DQ (5/6/2006): Added new classification so distinguish IR nodes from functions, variable,
                 // typedefs, etc. that are placed into "rose_edg_required_macros_and_functions.h" to
                 // support the GNU compatability mode that is incompletely implemented by EDG).
                    node->get_file_info()->setFrontendSpecific();

                    node->get_file_info()->setCompilerGenerated();
                  }

            // DQ (11/1/2007): Skip marking these this way so that we can focus on having then marked by the code above!
            // If they are first makred as compiler generated then they will not pass the test for if they are in the 
            // same file (Sg_File_Info::isSameFile()).
            // Now mark the subtree
            // markAsCompilerGenerated(node);
             }
        }

  // DQ (5/11/2006): Also mark the Sg_File_Info objects (such as those that are in comments and directives)
     Sg_File_Info* currentNodeFileInfo = isSg_File_Info(node);
     if (currentNodeFileInfo != NULL)
        {
          if (currentNodeFileInfo->isSameFile(targetFile) == true)
             {
#if 0
               printf ("Calling Sg_File_Info::setFrontendSpecific() for case #3 \n");
#endif
               currentNodeFileInfo->setFrontendSpecific();
               currentNodeFileInfo->setCompilerGenerated();
             }
        }

#if 0
  // Mark all SgFunctionSymbol IR nodes that are associated with the a frontend specific function
     SgFunctionSymbol* functionSymbol = isSgFunctionSymbol(node);
     if (functionSymbol != NULL)
        {
          SgDeclarationStatement* declaration = functionSymbol->get_declaration();
          if (declaration->get_file_info()->isSameFile(targetFile) == true)
             {
            // printf ("Found a node from targetFile->get_filename() = %s (mark it as compiler generated) \n",targetFile->get_filename());

            // DQ (5/6/2006): Added new classification so distinguish IR nodes from functions, variable, 
            // typedefs, etc. that are placed into "rose_edg_required_macros_and_functions.h" to 
            // support the GNU compatability mode that is incompletely implemented by EDG).
               functionSymbol->get_file_info()->setFrontendSpecific();

               functionSymbol->get_file_info()->setCompilerGenerated();
            // markAsCompilerGenerated(functionSymbol);
             }

          returnValue.addToGraph = false;
        }
#endif

   }

