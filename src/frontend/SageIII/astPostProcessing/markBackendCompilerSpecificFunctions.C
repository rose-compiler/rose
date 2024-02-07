// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "markBackendCompilerSpecificFunctions.h"
#include "FileUtility.h"

using namespace std;
using namespace Rose;

void
markBackendSpecificFunctionsAsCompilerGenerated(SgNode*)
   {
  // This simplifies how the traversal is called!
     MarkBackendSpecificFunctionsAsCompilerGenerated astFixupTraversal;
     astFixupTraversal.traverseMemoryPool();
   }

MarkBackendSpecificFunctionsAsCompilerGenerated::MarkBackendSpecificFunctionsAsCompilerGenerated()
   {
     targetFileName = "rose_edg_required_macros_and_functions.h";
     targetFile = nullptr;

  // Build a traversal for us on just the SgInitializedName memory pool so that we can find the filename (with complete path)
  // to the targetFileName and build a Sg_File_Info object which will permit more efficent testing of IR nodes later.
     class FindFileInfo : public ROSE_VisitTraversal
        {
          public:
               Sg_File_Info* targetFileInfo;
               std::string targetFileName;

               FindFileInfo(std::string & filename ) : targetFileInfo(nullptr), targetFileName(filename) {};
               virtual ~FindFileInfo() {}

               void visit (SgNode* node)
                  {
                    if (targetFileInfo == nullptr)
                       {
                         string rawFileName         = node->get_file_info()->get_raw_filename();
                         string filenameWithoutPath = StringUtility::stripPathFromFileName(rawFileName);

#ifndef USE_ROSE
                      // DQ (3/6/2006): Note that SgGlobal will not have an associated
                      // filename and so will not trigger the targetFile to be set.
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

     if (targetFile == nullptr &&
             (     SageInterface::is_Fortran_language() == false
                && SageInterface::is_Ada_language() == false
                && SageInterface::is_Java_language() == false
                && SageInterface::is_Jovial_language() == false
                && SageInterface::is_Jvm_language() == false
                && SageInterface::is_Python_language() == false
                && SageInterface::is_binary_executable() == false ) )
        {
          printf ("Lookup of Sg_File_Info referencing targetFileName = %s was unsuccessful \n",targetFileName.c_str());
        }
   }

MarkBackendSpecificFunctionsAsCompilerGenerated::~MarkBackendSpecificFunctionsAsCompilerGenerated()
   {
  // This should be true if the file was generated with a modified commandline that includes the
  // edg option to force the rose_edg_required_macros_and_functions.h to be read first.
  // but Qing's mechanism for building some AST fragments does not do this, so it is not always
  // valid.  This might be fixed up later.
     delete targetFile;
     targetFile = nullptr;
   }

void
MarkBackendSpecificFunctionsAsCompilerGenerated::visit(SgNode* node)
   {
     ROSE_ASSERT(node != NULL);

  // DQ (5/8/2006): This should have been setup bu now!
     if (targetFile == NULL)
        {
          return;
        }
     ROSE_ASSERT(targetFile != NULL);

     Sg_File_Info* fileInfo = node->get_file_info();
     if (fileInfo != NULL)
        {
          if (fileInfo->isSameFile(targetFile) == true)
             {
            // DQ (12/21/2006): Added to support uniformally marking IR nodes with such information.
               SgLocatedNode* locatedNode = isSgLocatedNode(node);
               if (locatedNode != NULL)
                  {
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
                 // DQ (5/6/2006): Added new classification so distinguish IR nodes from functions, variable,
                 // typedefs, etc. that are placed into "rose_edg_required_macros_and_functions.h" to
                 // support the GNU compatability mode that is incompletely implemented by EDG).
                    node->get_file_info()->setFrontendSpecific();
                    node->get_file_info()->setCompilerGenerated();
                  }

            // DQ (11/1/2007): Skip marking these this way so that we can focus on having then marked by the code above!
            // If they are first marked as compiler generated then they will not pass the test for if they are in the
            // same file (Sg_File_Info::isSameFile()).
             }
        }

  // DQ (5/11/2006): Also mark the Sg_File_Info objects (such as those that are in comments and directives)
     Sg_File_Info* currentNodeFileInfo = isSg_File_Info(node);
     if (currentNodeFileInfo != NULL)
        {
          if (currentNodeFileInfo->isSameFile(targetFile) == true)
             {
               currentNodeFileInfo->setFrontendSpecific();
               currentNodeFileInfo->setCompilerGenerated();
             }
        }
   }

