/****************************************************************************

Remarks on Unparsing and on attaching preprocessing information to AST nodes
----------------------------------------------------------------------------
Markus Kowarschik, 10/2002

The SgFile (always) constructor calls the function
void attachPreprocessingInfo(SgFile *sageFilePtr);
which in turn calls getPreprocessorDirectives (see above) and then 
invokes a tree traversal in order to attach the preprocessor directives
(i.e., the preprocessingInfo objects) to located nodes in the AST.
(Currently, we only attach preprocessingInfo objects to SgStatement
objects.)

For this purpose, I added a new data member
attachedPreprocessingInfoType* attachedPreprocessingInfoPtr;
to the SgLocatedNode class. This is done in ROSETTA/src/node.C.

Furthermore, I added the corresponding access functions:
void addToAttachedPreprocessingInfo(preprocessingInfo *prepInfoPtr);
attachedPreprocessingInfoType* getAttachedPreprocessingInfo(void);
to the SgLocatedNode class. This is done in ROSETTA/Grammar/LocatedNode.code.

The tree traversal works as follows: whenever it hits a located node
(currently: a statement), it checks if there is preprocessing info the
line number of which is less or equal than the line number of the current
located node (currently: of the current statement). If this is the case,
the corresponding preprocessing info is attached to the current
located node (currently: before the current statement), unparse flag: "before".
All this is done in the evaluateInheritedAttribute member function of the
derived tree traversal class.

The evaluateSynthesizedAttribute member function deletes the list of 
preprocessingInfo objects as soon as the traversal returns to a SgFile
object and attaches trailing preprocessing information to the last located
node (currently to the last statement) that has been visited in
the file (unparse flag: "after").

Node that the preprocessingInfo objects are always attached to AST nodes.
By switching the USE_OLD_MECHANISM_OF_HANDLING_PREPROCESSING_INFO flag,
you only change the mechanism which the unparser is based on!
If USE_OLD_MECHANISM_OF_HANDLING_PREPROCESSING_INFO is set to 1, then
the unparser simply ignores the preprocessingInfo objects that have
been attached to the AST nodes.

Problems with the handling of preprocessing information can be
found in the directory ROSE/TESTS/KnownBugs/AttachPreprocessingInfo.

****************************************************************************/

// This file implements the extraction of the attributes (comments and
// preprocessor directives) from the original source file and their insertion
// into the AST data structure.
// The idea is to re-introduce them (as correctly as possible) when the
// transformed AST is unparsed later on.

// #include "attachPreprocessingInfo.h"
// #include "sage3.h"
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
#include "rose_config.h"

#include "stringify.h"
#include "attachPreprocessingInfo.h"
#include "attachPreprocessingInfoTraversal.h"

// DQ (9/15/2018): Associated header file for the class and member function declarations defined in this file.
// NOTE: this has been moved to be a new ROSE IR node.
// #include "headerFileSupportReport.h"

// DQ (9/26/2018): Added so that we can call the display function for TokenStreamSequenceToNodeMapping (for debugging).
#include "tokenStreamMapping.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;
using namespace Rose;

// Debug flag
#define DEBUG_ATTACH_PREPROCESSING_INFO 0

// DQ (6/17/2020): This appears to be required to avoid segfaults that will not print the failing assertion.
// #define ROSE_ASSERT assert

// DQ (8/23/2018): Adding function declaration to generate comments, CPP directives and the token stream.
void buildTokenStreamMapping(SgSourceFile* sourceFile);

// It is needed because otherwise, the default destructor breaks something.

AttachPreprocessingInfoTreeTrav::~AttachPreprocessingInfoTreeTrav() {
    //do nothing
}


namespace EDG_ROSE_Translation
   {
  // DQ (9/18/2018): Declare this map so that we can use it for the unparse header files option.
#if defined(ROSE_BUILD_CXX_LANGUAGE_SUPPORT) && !defined(ROSE_USE_CLANG_FRONTEND)
  // DQ (12/11/2018): Use the definition in the EDG edgRose.C file if C/C++ support IS defined.
     extern std::map<std::string, SgIncludeFile*> edg_include_file_map;
#else
  // DQ (12/11/2018): Allow this to be the definition if C/C++ support is NOT defined.
     extern std::map<std::string, SgIncludeFile*> edg_include_file_map;
#endif
   }

// DQ (11/30/2008): Refactored this code out of the simpler function to isolate the Wave specific handling.

// AttachPreprocessingInfoTreeTrav::AttachPreprocessingInfoTreeTrav( SgSourceFile* file, bool includeDirectivesAndCommentsFromAllFiles )
AttachPreprocessingInfoTreeTrav::AttachPreprocessingInfoTreeTrav( SgSourceFile* file, ROSEAttributesList* listOfAttributes )
   {
     use_Wave = file->get_wave();

  // Wave will get all Preprocessor Diretives by default and it is therefore reasonable that it will attach all

  // DQ (6/5/2020): Adding back the original simile level of support for a single ROSEAttributesList data member.
     start_index = 0;

  // DQ (6/2/2020): This feature is now handled through repeated calls to attach the CPP directives and comments to each file seperately.
  // processAllIncludeFiles = includeDirectivesAndCommentsFromAllFiles;
     processAllIncludeFiles = false;

  // DQ (5/4/2020): This is now handled in a different way.  Each invocation of the AttachPreprocessingInfoTreeTrav
  // traversal will only insert a single file's (header file of source file) comments and CPP directives into the AST.
     ROSE_ASSERT(processAllIncludeFiles == false);

#if 0
     printf ("processAllIncludeFiles = %s \n",processAllIncludeFiles ? "true" : "false");
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

     sourceFile = file;

#if 0
     printf ("In AttachPreprocessingInfoTreeTrav constructor: sourceFile = %p = %s \n",sourceFile,sourceFile->class_name().c_str());
#endif

#if 0
  // DQ (6/8/2020): Testing: This is always false for tests in Cxx_tests directory (no header file unparsing).
  // DQ (4/23/2020): I think this is marked incorrectly.
     if (sourceFile->get_unparseHeaderFiles() == false)
        {
          printf ("ERROR: sourceFile->get_unparseHeaderFiles() == false and should be true for testing unparseHeaderTests test8 \n");
          ROSE_ASSERT(false);
        }
#endif

  // DQ (2/28/2019): We need to return the line that is associated with the source file where this can be a ode shared between multiple ASTs.
     ROSE_ASSERT(sourceFile != NULL);
     ROSE_ASSERT(sourceFile->get_file_info() != NULL);
     source_file_id = sourceFile->get_file_info()->get_physical_file_id();

  // DQ (11/20/2019): Check this.
  // ROSE_ASSERT(sourceFile->get_globalScope() != NULL);

  // ROSEAttributesList* returnListOfAttributes = NULL;
  // ROSEAttributesListContainerPtr filePreprocInfo = sourceFile->get_preprocessorDirectivesAndCommentsList();

     currentListOfAttributes = listOfAttributes;
     ROSE_ASSERT(currentListOfAttributes != NULL);

  // DQ (6/23/2020): Initialize this.
     previousLocatedNode = NULL;

  // DQ (6/23/2020): Initialize this.
  // target_source_file_id = sourceFile->get_file_info()->get_physical_file_id();
     target_source_file_id = sourceFile->get_file_info()->get_physical_file_id();

#if 0
     printf ("In AttachPreprocessingInfoTreeTrav constructor: target_source_file_id = %d \n",target_source_file_id);
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

#if 0
     printf ("Leaving AttachPreprocessingInfoTreeTrav() constructor: sourceFile = %p sourceFile->getFileName() = %s \n",sourceFile,sourceFile->getFileName().c_str());
     printf (" --- sourceFile->get_globalScope() = %p \n",sourceFile->get_globalScope());
#endif
#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

   }


// #ifndef  CXX_IS_ROSE_CODE_GENERATION

// DQ (10/27/2007): Added display function to output information gather durring the collection of 
// comments and CPP directives across all files.
void
AttachPreprocessingInfoTreeTrav::display(const std::string & label) const
   {
  // Output internal information

     printf ("Inside of AttachPreprocessingInfoTreeTrav::display(%s) \n",label.c_str());
     printf ("   use_Wave                      = %s \n",use_Wave ? "true" : "false");
     printf ("   processAllIncludeFiles        = %s \n",processAllIncludeFiles ? "true" : "false");

  // DQ (4/30/2020): Changing the implementation to simplify header file unparsing.
     ROSE_ASSERT(currentListOfAttributes != NULL);
     printf ("currentListOfAttributes = %p list size = %d filename = %s \n",
          currentListOfAttributes,currentListOfAttributes->size(),currentListOfAttributes->getFileName().c_str());

     printf ("previousLocatedNode = %p = %s \n",previousLocatedNode,(previousLocatedNode != NULL) ? previousLocatedNode->class_name().c_str() : "NULL");

     printf ("start_index = %d \n",start_index);
   }

// DQ (8/6/2012): New copy constructor.
AttachPreprocessingInfoTreeTraversalInheritedAttrribute::AttachPreprocessingInfoTreeTraversalInheritedAttrribute(const AttachPreprocessingInfoTreeTraversalInheritedAttrribute & X)
   {
     isPartOfTemplateDeclaration              = X.isPartOfTemplateDeclaration;
     isPartOfTemplateInstantiationDeclaration = X.isPartOfTemplateInstantiationDeclaration;
   }


void
AttachPreprocessingInfoTreeTrav::iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
   ( SgLocatedNode* locatedNode, int lineNumber, PreprocessingInfo::RelativePositionType location, bool reset_start_index, ROSEAttributesList *currentListOfAttributes)
   {
  // DQ (11/23/2008): Added comment.
  // This is the main function called to insert all PreprocessingInfo objects into IR nodes.  This function currently 
  // adds the PreprocessingInfo objects as attributes, but will be modified to insert the CPP directive specific
  // PreprocessingInfo objects as separate IR nodes and leave PreprocessingInfo objects that are comments inserted
  // as attributes.  Note that attributes imply PreprocessingInfo specific atrributes and not the more general 
  // mechanism available in ROSE for user defined attributes to be saved into the AST.

     ROSE_ASSERT(currentListOfAttributes != NULL);

  // DQ (4/29/2020): Introduce test for recursive call.
     static bool isRecursiveCall = false;
     ROSE_ASSERT(isRecursiveCall == false);

     isRecursiveCall = true;

#define DEBUG_IterateOverList 0

#if DEBUG_IterateOverList
  // DQ (8/22/2018): Added debugging information.
     printf ("In AttachPreprocessingInfoTreeTrav::iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber(): currentListOfAttributes->size() = %d \n",currentListOfAttributes->size());
#endif
#if DEBUG_IterateOverList
     currentListOfAttributes->display("Top of iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber()");
#endif

  // DQ (6/8/2020): Adding assertions to debug segfault below.
     ROSE_ASSERT(locatedNode != NULL);

#if DEBUG_IterateOverList
  // DQ (9/17/2019): Added debugging information.
     printf ("In AttachPreprocessingInfoTreeTrav::iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber(): sourceFile = %p = %s \n",sourceFile,sourceFile->class_name().c_str());
     printf (" --- currentListOfAttributes->size() = %d \n",currentListOfAttributes->size());
     printf (" --- iterateOverListAndInsertPrev... locatedNode = %p = %s lineNumber = %d location = %s \n",
          locatedNode,locatedNode->class_name().c_str(),lineNumber,PreprocessingInfo::relativePositionName(location).c_str());
#endif

#if 0
  // Debugging information...
     printf ("\n\nIn iterateOverListAndInsertPrev... locatedNode = %s lineNumber = %d location = %s \n",locatedNode->class_name().c_str(),lineNumber,PreprocessingInfo::relativePositionName(location).c_str());
  // if ( dynamic_cast<SgLocatedNode*>(locatedNode) != NULL )
  // if ( isSgLocatedNode(locatedNode) != NULL )
     if ( locatedNode != NULL )
        {
       // printf ("starting line number = %d \n",locatedNode->get_startOfConstruct()->get_line());
          printf ("starting line number = %d \n",locatedNode->get_startOfConstruct()->get_physical_line(source_file_id));
          if (locatedNode->get_endOfConstruct() != NULL)
             {
            // printf ("ending line number   = %d \n",locatedNode->get_endOfConstruct()->get_line());
               printf ("ending line number   = %d \n",locatedNode->get_endOfConstruct()->get_physical_line(source_file_id));
             }
        }
       else
        {
          printf ("locatedNode is not a SgLocatedNode object \n");

       // DQ (12/16/2008): I think this should be an error.
          ROSE_ASSERT(false);
        }
#endif

  // DQ (6/5/2020): Adding assertions to debug segfault below.
     ROSE_ASSERT(locatedNode != NULL);

#if 0
     printf ("Error checking for locatedNode \n");
     if (locatedNode != NULL)
        {
          printf ("locatedNode = %p \n",locatedNode);
          printf ("locatedNode = %p = %s \n",locatedNode,locatedNode->class_name().c_str());
        }
     if (locatedNode->get_startOfConstruct() == NULL)
        {
          printf ("Error: locatedNode = %p = %s \n",locatedNode,locatedNode->class_name().c_str());
          printf (" --- locatedNode->get_startOfConstruct() = %p \n",locatedNode->get_startOfConstruct());
          printf (" --- locatedNode->get_endOfConstruct()   = %p \n",locatedNode->get_endOfConstruct());
        }
#endif

     ROSE_ASSERT(locatedNode->get_startOfConstruct() != NULL);

#if DEBUG_ATTACH_PREPROCESSING_INFO
  // Debugging information...
        {
       // int line        = locatedNode->get_startOfConstruct()->get_line();
          int line        = locatedNode->get_startOfConstruct()->get_physical_line(source_file_id);
          int col         = locatedNode->get_startOfConstruct()->get_col();
       // int ending_line = locatedNode->get_endOfConstruct()->get_line();
          int ending_line = locatedNode->get_endOfConstruct()->get_physical_line(source_file_id);
          int ending_col  = locatedNode->get_endOfConstruct()->get_col();

       // DQ (8/6/2012): Added support for endOfConstruct().
          cout << "Visiting SgStatement node (starting: " << line << ":" << col << ") (ending " << ending_line << ":" << ending_col << ") -> ";
          cout << getVariantName(locatedNode->variantT()) << endl;
          cout << "-----> Filename: " << locatedNode->get_file_info()->get_filename() << endl;
        }
#if 0
     printf("-----> Address: %p\n", locatedNode);
     cout << "-----> Filename: " << locatedNode->get_file_info()->get_filename() << endl;
     if (locatedNode->getAttachedPreprocessingInfo() == NULL)
          cout << "-----> No PreprocessingInfo objects attached yet" << endl;
     else
          cout << "-----> There are already PreprocessingInfo objects attached to this AST node" << endl;
#endif
  // cout << "Traversing current list of attributes of length " << sizeOfCurrentListOfAttributes << endl;
#endif

  // for ( int i = 0; i < sizeOfCurrentListOfAttributes; i++ )
  // AS(09/21/07) Because the AttachAllPreprocessingInfoTreeTrav can call the evaluateInheritedAttribute(..)
  // which calls this function the start_index can not be static for this function. Instead it is made
  // a class member variable for AttachPreprocessingInfoTreeTrav so that it can be reset by AttachAllPreprocessingInfoTreeTrav
  // when processing a new file.

  // static int start_index = 0;
  // int currentFileId = locatedNode->get_startOfConstruct()->get_file_id();
     Sg_File_Info* locatedFileInfo = locatedNode->get_file_info();

#if 0
     printf ("In iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber(): locatedNode = %p = %s locatedFileInfo = %p \n",locatedNode,locatedNode->class_name().c_str(),locatedFileInfo);
     printf ("In iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber(): sourceFile->get_requires_C_preprocessor() = %s \n",sourceFile->get_requires_C_preprocessor() ? "true" : "false");
#endif

  // DQ (12/18/2012): Switch to using the physical file id now that we support this feature.
     int currentFileId = (sourceFile->get_requires_C_preprocessor() == true) ? 
                         Sg_File_Info::getIDFromFilename(sourceFile->generate_C_preprocessor_intermediate_filename(sourceFile->get_file_info()->get_filename())) : 
                         locatedFileInfo->get_physical_file_id(source_file_id);

#if 0
     printf ("Initial value of currentFileId = %d = %s \n",currentFileId,Sg_File_Info::getFilenameFromID(currentFileId).c_str());
#endif

  // DQ (12/15/2012): Allow equivalent files to be mapped back to the source file.
     if (currentListOfAttributes->get_filenameIdSet().find(currentFileId) != currentListOfAttributes->get_filenameIdSet().end())
        {
       // File name that we want all equivalent files to map to...
       // string filename = sourceFile->generate_C_preprocessor_intermediate_filename(sourceFile->get_file_info()->get_filename());
          string filename = sourceFile->get_file_info()->get_filename();

          currentFileId = Sg_File_Info::getIDFromFilename(filename);
#if 0
          printf ("Reset the currentFileId to currentFileId = %d = %s filename = %s \n",currentFileId,Sg_File_Info::getFilenameFromID(currentFileId).c_str(),filename.c_str());
#endif
       // DQ (12/19/2012): This should map to an existing file.
          ROSE_ASSERT(currentFileId >= 0);
        }

  // DQ (4/30/2020): We no long need this in the new simplified support for CPP directivces and comments and unparsing of header files.
  // int start_index = startIndexMap[currentFileId];
     int sizeOfCurrentListOfAttributes = currentListOfAttributes->size();

#if DEBUG_IterateOverList
     printf ("Initial start_index = %d \n",start_index);
#endif

   // Liao 2/1/2010: SgBasicBlock in Fortran should be ignored for attaching a preprocessing info with a 'before' position.
   // The reason is that there is no  { ..} in Fortran and the preprocessing information should really be associated with 
   // a statement showing up in the source code. 
   // However, we allow a preprocessing info. to be attached to be inside of a SgBasicBlock to get the following special case right:
   // end do does not exist in AST. The comment has to be attached inside the do-loop's body to be unparsed right before 'end do'
   //  do i 1, 10
   //
   // ! comment here
   //  end do
   //
     bool isFortranBlockAndBeforePoisition = false; // should we skip a Fortran basic block when the position is before?
     if (SageInterface::is_Fortran_language() )
        {
          if (isSgBasicBlock (locatedNode) && (location == PreprocessingInfo::before || location == PreprocessingInfo::after))
             {
               isFortranBlockAndBeforePoisition = true; 
             }
        }

  // DQ (12/23/2008): Note: I think that this should be turned into a while loop (starting at start_index,
  // to lineNumber when location == PreprocessingInfo::before, and to the sizeOfCurrentListOfAttributes 
  // when location == PreprocessingInfo::after).
     if (!isFortranBlockAndBeforePoisition)
        {
#if DEBUG_IterateOverList
          printf ("start_index = %d sizeOfCurrentListOfAttributes = %d \n",start_index,sizeOfCurrentListOfAttributes);
#endif
          list<pair<SgIncludeDirectiveStatement*, SgStatement*> > localStatementsToInsertAfter;
          for ( int i = start_index; i < sizeOfCurrentListOfAttributes; i++ )
             {
               PreprocessingInfo *currentPreprocessingInfoPtr = (*currentListOfAttributes)[i];

            // DQ (6/4/2020): Added test.
               ROSE_ASSERT(currentPreprocessingInfoPtr != NULL);
#if DEBUG_IterateOverList
               printf ("TOP OF LOOP: Processing (*currentListOfAttributes)[%3d] = %p string = %s \n",i,currentPreprocessingInfoPtr,currentPreprocessingInfoPtr->getString().c_str());
#endif
            // DQ (8/21/2018): I think we can assert these here.
               ROSE_ASSERT(currentPreprocessingInfoPtr->get_file_info() != NULL);
               ROSE_ASSERT(currentPreprocessingInfoPtr != NULL);
#if 0
               printf ("currentPreprocessingInfoPtr->getLineNumber() = %d lineNumber = %d internalString = %s \n",currentPreprocessingInfoPtr->getLineNumber(),lineNumber,currentPreprocessingInfoPtr->getString().c_str());
               printf ("   --- currentPreprocessingInfoPtr->get_file_info()->filenameString() = %s \n",currentPreprocessingInfoPtr->get_file_info()->get_filenameString().c_str());
#endif
               ROSE_ASSERT(currentPreprocessingInfoPtr != NULL);
               int currentPreprocessingInfoLineNumber = currentPreprocessingInfoPtr->getLineNumber();

               int currentPreprocessingInfoColumnNumber = currentPreprocessingInfoPtr->getColumnNumber();

            // DQ (8/17/2020): Added note detail
               int line        = locatedNode->get_startOfConstruct()->get_physical_line(source_file_id);
               int col         = locatedNode->get_startOfConstruct()->get_col();
               int ending_line = locatedNode->get_endOfConstruct()->get_physical_line(source_file_id);
               int ending_col  = locatedNode->get_endOfConstruct()->get_col();

#if DEBUG_IterateOverList
               printf ("currentPreprocessingInfoLineNumber   = %d lineNumber = %d \n",currentPreprocessingInfoLineNumber,lineNumber);
               printf ("currentPreprocessingInfoColumnNumber = %d lineNumber = %d \n",currentPreprocessingInfoColumnNumber,lineNumber);
               printf ("starting line = %d ending_line = %d starting col = %d ending_col = %d \n",line,ending_line,col,ending_col);
               printf ("location = %s \n",PreprocessingInfo::relativePositionName(location).c_str());
#endif

            // DQ (12/23/2008): So far this is the most reliable way to break out of the loop.
               ROSE_ASSERT(currentPreprocessingInfoPtr != NULL);

#if 0
#if 0
            // DQ (8/17/2020): Original code.
               if ( (currentPreprocessingInfoLineNumber > lineNumber) && (location == PreprocessingInfo::before) )
#else
            // DQ (8/17/2020): Account for the column numbers.
               if ( ( (currentPreprocessingInfoLineNumber > lineNumber) || 
                      ( (currentPreprocessingInfoLineNumber == lineNumber) && (currentPreprocessingInfoColumnNumber < ending_col) ) ) &&
                    ( (location == PreprocessingInfo::before) || (location == PreprocessingInfo::inside) ) )
#endif
                  {
                 // DQ (12/23/2008): I think that under this constraint we could exit this loop!
#if DEBUG_IterateOverList
                    printf ("BREAK OUT OF LOOP: BREAK: Warning: Why are we searching this list of PreprocessingInfo beyond the line number of the current statement (using break) \n");
#endif
                 // DQ (8/17/2020): This line was previously commented out except when DEBUG_IterateOverList was defined to be greater then zero.
                 // DQ (12/23/2008): I don't like the design which forces an exit from the loop here, but this is the most robust implementation so far.
                    break;
                  }
#endif
            // bool attachCommentOrDirective = (currentPreprocessingInfoPtr != NULL) && (currentPreprocessingInfoPtr->getLineNumber() <= lineNumber);
            // bool attachCommentOrDirective = (currentPreprocessingInfoLineNumber <= lineNumber);
                  bool attachCommentOrDirective = 
                       (currentPreprocessingInfoLineNumber < lineNumber) || 
                       ( (currentPreprocessingInfoLineNumber == lineNumber) && (currentPreprocessingInfoColumnNumber < ending_col) );

            // DQ (1/7/2019): Supress comments and CPP directives onto member functions of the generated labda function class.
               SgLambdaExp* lambdaExpression = isSgLambdaExp(locatedNode->get_parent());
               if (lambdaExpression != NULL)
                  {
#if 0
                    printf ("NOTE: Detected lambda expression as parent of the located node = %p = %s: suppress attachment of comments and CPP directives \n",locatedNode,locatedNode->class_name().c_str());
#endif
                    attachCommentOrDirective = false;
                  }

#if DEBUG_IterateOverList
               printf ("@@@@@@@@@@@@@@@@@@ attachCommentOrDirective = %s currentPreprocessingInfoLineNumber = %d lineNumber = %d \n",attachCommentOrDirective ? "true" : "false",currentPreprocessingInfoLineNumber,lineNumber);
#endif
               if ( attachCommentOrDirective == true )
                  {
#if DEBUG_ATTACH_PREPROCESSING_INFO || 0
                    printf ("Attaching \"%s\" (from file = %s file_id = %d line# %d) to %s locatedNode = %p = %s = %s at line %d position = %s \n",
                         currentPreprocessingInfoPtr->getString().c_str(),
                         currentPreprocessingInfoPtr->getFilename().c_str(),
                         currentPreprocessingInfoPtr->getFileId(),
                         currentPreprocessingInfoPtr->getLineNumber(),
                         (locatedNode->get_file_info()->isCompilerGenerated() == true) ? "compiler-generated" : "non-compiler-generated",
                         locatedNode,
                         locatedNode->class_name().c_str(),SageInterface::get_name(locatedNode).c_str(),
                      // (locatedNode->get_file_info()->isCompilerGenerated() == true) ? -1 : locatedNode->get_file_info()->get_line());
                         (locatedNode->get_file_info()->isCompilerGenerated() == true) ? -1 : locatedNode->get_file_info()->get_physical_line(source_file_id),
                         PreprocessingInfo::relativePositionName(location).c_str());
#if 0
                    printf ("locatedNode from file: %s \n",locatedNode->get_file_info()->get_filenameString().c_str());
#endif
#if DEBUG_IterateOverList
                    printf ("Attaching to node from locatedNode->get_file_info()->get_filename() = %s \n",locatedNode->get_file_info()->get_filename());
                    printf (" --- currentListOfAttributes->getFileName()                        = %s \n",currentListOfAttributes->getFileName().c_str());
#endif
                 // DQ (11/4/2019): If we want this assertion then it likely should be based on physical filenames (derived from physical fid ids).
                 // DQ (11/3/2019): Check that the comment or CPP directive is from the same file as the locatedNode.
                 // A variation of this test might be required later, though we should only be attacheing comments and 
                 // CPP directives before possible transformations.
#if DEBUG_IterateOverList
                    printf ("In iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber(): locatedNode->get_file_info()->get_filename() = %s \n",locatedNode->get_file_info()->get_filename());
                    printf ("In iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber(): currentListOfAttributes->getFileName()       = %s \n",currentListOfAttributes->getFileName().c_str());
#endif
                 // ROSE_ASSERT(locatedNode->get_file_info()->get_filename() == currentListOfAttributes->getFileName());

#if 0
                 // DQ (6/25/2020): Adding test for test2020_03.c.
                    if (currentPreprocessingInfoPtr->getString().find("test2020_03.h") != string::npos)
                      {
                        printf ("Exiting as a test! \n");
                        ROSE_ASSERT(false);
                      }
#endif
                    SgNode* parentNode = locatedNode->get_parent();
                    if (parentNode != NULL)
                       {
                         printf ("locatedNode->parent = %p = %s \n",parentNode,parentNode->class_name().c_str());
                         SgClassDefinition* classDefinition = isSgClassDefinition(parentNode);
                         if (classDefinition != NULL)
                            {
                              SgClassDeclaration* classDeclaration = classDefinition->get_declaration();
                              if (classDeclaration != NULL)
                                 {
                                   printf ("parent: classDeclaration->get_name() = %s \n",classDeclaration->get_name().str());
                                 }
                            }
                           else
                            {
                              SgStatement* associatedStatement = isSgStatement(locatedNode);

                           // DQ (11/4/2019): This is not general enough code.
                           // ROSE_ASSERT(associatedStatement != NULL);
                              if (associatedStatement != NULL)
                                 {
                                   SgScopeStatement* associatedScope = isSgScopeStatement(associatedStatement->get_scope());
                                   SgClassDefinition* classDefinition = isSgClassDefinition(associatedScope);
                                   if (classDefinition != NULL)
                                      {
                                        SgClassDeclaration* classDeclaration = classDefinition->get_declaration();
                                        if (classDeclaration != NULL)
                                           {
                                             printf ("associatedScope: classDeclaration->get_name() = %s \n",classDeclaration->get_name().str());
                                           }
                                      }
                                 }
                            }
                       }
                 // printf ("locatedNode->unparseToString() = %s \n",locatedNode->unparseToString().c_str());
#endif
                 // Mark this PreprocessingInfo object as having been placed into the AST
                 // It might make more sense to remove it from the list so it doesn't have 
                 // to be traversed next time.
                 // currentPreprocessingInfoPtr->setHasBeenCopied();

                 // negara1 (08/05/2011): Do not set to NULL such that we can reuse it for multiple inclusions of the same header file.
                 // currentListOfAttributes->getList()[i] = NULL;

                 // DQ (4/30/2020): We no long need this in the new simplified support for CPP directivces and comments and unparsing of header files.
                 // DQ (4/13/2007): If we are going to invalidate the list of accumulated attributes then we can start 
                 // next time at the next index (at least).  This removes the order n^2 complexity of traversing over the whole loop.
                 // start_index = i+1;
                 // ROSE_ASSERT(startIndexMap.find(currentFileId) != startIndexMap.end());
                 // startIndexMap[currentFileId] = i+1;

                    start_index = i+1;
#if DEBUG_IterateOverList
                 // DQ (4/30/2020): We no long need this in the new simplified support for CPP directivces and comments and unparsing of header files.
                 // printf ("Incremented start_index to be %d \n",startIndexMap[currentFileId]);
                    printf ("Incremented start_index to be %d \n",start_index);
#endif
                 // Mark the location relative to the current node where the PreprocessingInfo 
                 // object should be unparsed (before or after) relative to the current locatedNode
                    currentPreprocessingInfoPtr->setRelativePosition(location);
#if DEBUG_IterateOverList
                    printf ("Attaching CPP directives %s to IR nodes as attributes (location = %s) \n",
                            PreprocessingInfo::directiveTypeName(currentPreprocessingInfoPtr->getTypeOfDirective()).c_str(),
                            PreprocessingInfo::relativePositionName(currentPreprocessingInfoPtr->getRelativePosition()).c_str());
#endif
#if 1
                 // This uses the old code to attach comments and CPP directives to the AST as attributes.
                    locatedNode->addToAttachedPreprocessingInfo(currentPreprocessingInfoPtr);
#if DEBUG_IterateOverList
                    printf ("DONE: Attaching CPP directives %s to IR nodes as attributes. \n",PreprocessingInfo::directiveTypeName(currentPreprocessingInfoPtr->getTypeOfDirective()).c_str());
#endif
                 // DQ (12/2/2018): This fails for the C/C++ snippet insertion tests.
                 // DQ (12/2/2018): This fails for Fortran.
                 // DQ (9/5/2018): We should have already set the preprocessorDirectivesAndCommentsList, checked in getTokenStream().
                 // ROSE_ASSERT(sourceFile->get_preprocessorDirectivesAndCommentsList() != NULL);
                 // if (SageInterface::is_Fortran_language() == false)
                    if (SageInterface::is_C_language() == true || SageInterface::is_Cxx_language() == true)
                       {
                      // ROSE_ASSERT(sourceFile->get_preprocessorDirectivesAndCommentsList() != NULL);
                       }
#if DEBUG_IterateOverList
                    printf ("sourceFile->getFileName()                            = %s \n",sourceFile->getFileName().c_str());
                    printf ("sourceFile->get_unparseHeaderFiles()                 = %s \n",sourceFile->get_unparseHeaderFiles() ? "true" : "false");
                    printf ("sourceFile->get_header_file_unparsing_optimization() = %s \n",sourceFile->get_header_file_unparsing_optimization() ? "true" : "false");
                    printf ("currentPreprocessingInfoPtr->getTypeOfDirective() == PreprocessingInfo::CpreprocessorIncludeDeclaration = %s \n",
                            currentPreprocessingInfoPtr->getTypeOfDirective() == PreprocessingInfo::CpreprocessorIncludeDeclaration ? "true" : "false");
#endif

#if 0
                 // DQ (6/8/2020): This is always false for tets in Cxx_tests directory (no header file unparsing).
                 // DQ (4/23/2020): I think this is marked incorrectly.
                    if (sourceFile->get_unparseHeaderFiles() == false)
                       {
                         printf ("ERROR: sourceFile->get_unparseHeaderFiles() == false and should be true for testing unparseHeaderTests test8 \n");
                         ROSE_ASSERT(false);
                       }
#endif

#else

#error "DEAD CODE!"

                 // Removed older equivalent code!
#endif

                 // For now leave the lists unmodified so that we can support debugging.
                 // delete currentPreprocessingInfoPtr;
                 // currentPreprocessingInfoPtr = NULL;

                 // debugging info
                 // printOutComments(locatedNode);
                  }
#if 0
            // Increment the loop index
               i++;

            // Reset the PreprocessingInfo pointer using the incremented value of "i"
               currentPreprocessingInfoPtr = (*currentListOfAttributes)[i];
#endif
#if DEBUG_IterateOverList
               printf ("BOTTOM OF LOOP: Processing (*currentListOfAttributes)[%3d] = %p string = %s \n",i,currentPreprocessingInfoPtr,currentPreprocessingInfoPtr->getString().c_str());
#endif
             }

       // DQ (1/7/2019): This appears to be nearly always an empty list, so we can improve the performance and also simlify the debugging with this test.
          if (localStatementsToInsertAfter.empty() == false)
             {
#if DEBUG_IterateOverList
            // DQ (1/7/2019): Adding debugging support.
               printf ("Calling insert statements: statementsToInsertAfter.size() = %zu localStatementsToInsertAfter.size() = %zu \n",
                    statementsToInsertAfter.size(),localStatementsToInsertAfter.size());
#endif
            // negara1 (08/15/2011): After the iteration is over, add local list of statements to "insert after" to the global list. Two lists are used in order to
            // insert in front of the local list and then, insert the local list in front of the global list such that we preserve the relative order of inserted nodes. 
               statementsToInsertAfter.insert(statementsToInsertAfter.begin(), localStatementsToInsertAfter.begin(), localStatementsToInsertAfter.end());
             }
        }

  // DQ (12/12/2008): We should not need this state, so why support resetting it, unless the traversal needs to be called multiple times.
  // DQ (4/13/2007): The evaluation of the synthesized attribute for a SgFile will trigger the reset of the start index to 0.
     if (reset_start_index == true)
        {
       // DQ (4/30/2020): We no long need this in the new simplified support for CPP directivces and comments and unparsing of header files.
       // Reset all the start_index data members (for each associated file)
       // start_index = 0;
       // for (StartingIndexAttributeMapType::iterator it = startIndexMap.begin(); it != startIndexMap.end(); it++)
       //    {
       //      it->second = 0;
       //    }
          start_index = 0;
        }

#if 0
     string dotgraph_filename = "include_file_graph_from_attach_CPP_directives";
     SgProject* project = SageInterface::getProject(XXX);
     ROSE_ASSERT(project != NULL);
     generateGraphOfIncludeFiles(project,dotgraph_filename);
#endif

#if DEBUG_IterateOverList
  // DQ (10/27/2019): Added debugging information.
     printf ("Leaving AttachPreprocessingInfoTreeTrav::iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber(): currentListOfAttributes->size() = %d \n",currentListOfAttributes->size());
#endif

  // DQ (4/29/2020): Introduce test for recursive call.
     isRecursiveCall = false;

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }


//! Use parent as the previous node to attach preprocessing info since a current node is not unparsed.
void
AttachPreprocessingInfoTreeTrav::setupPointerToPreviousNode (SgLocatedNode* currentLocNodePtr )
   {
  // If we are at a SgCtorInitializerList IR nodes (and a few others) 
  // then since it is visited last (after the definition) leave the 
  // previousLocNodePtr referenced to the function definition.

  // Supports assertions at end of function
     SgLocatedNode* previousLocNodePtr = NULL;

  // DQ (12/12/2008): Newer implementation to support multiple files.
  // int currentFileId = currentLocNodePtr->get_startOfConstruct()->get_file_id();
     Sg_File_Info* locatedFileInfo = currentLocNodePtr->get_file_info();
#if 0
     int currentFileId = (sourceFile->get_requires_C_preprocessor() == true) ? 
                         Sg_File_Info::getIDFromFilename(sourceFile->generate_C_preprocessor_intermediate_filename(sourceFile->get_file_info()->get_filename())) : 
                         locatedFileInfo->get_file_id();
#else
     int currentFileId = (sourceFile->get_requires_C_preprocessor() == true) ? 
                         Sg_File_Info::getIDFromFilename(sourceFile->generate_C_preprocessor_intermediate_filename(sourceFile->get_file_info()->get_filename())) : 
                         locatedFileInfo->get_physical_file_id(source_file_id);
#endif

#if 1
     printf ("setupPointerToPreviousNode: currentFileId = %d currentLocNodePtr = %s \n",currentFileId,currentLocNodePtr->class_name().c_str());
#endif

     if ( (dynamic_cast<SgForInitStatement*>     (currentLocNodePtr) == NULL) &&
          (dynamic_cast<SgTypedefSeq*>           (currentLocNodePtr) == NULL) &&
          (dynamic_cast<SgCatchStatementSeq*>    (currentLocNodePtr) == NULL) &&
          (dynamic_cast<SgFunctionParameterList*>(currentLocNodePtr) == NULL) &&
          (dynamic_cast<SgCtorInitializerList*>  (currentLocNodePtr) == NULL) )
        {
#if 0
       // Debugging output...
          if (previousLocatedNodeMap.find(currentFileId) == previousLocatedNodeMap.end())
             {
               printf ("Note that previousLocatedNodeMap does not have an entry for currentFileId = %d \n",currentFileId);
             }
#endif
#if 0
       // DQ (4/30/2020): We no long need this in the new simplified support for CPP directivces and comments and unparsing of header files.
          previousLocatedNodeMap[currentFileId] = currentLocNodePtr;
          ROSE_ASSERT(previousLocatedNodeMap.find(currentFileId) != previousLocatedNodeMap.end());

       // Supports assertions at end of function
          previousLocNodePtr = currentLocNodePtr;
#else
       // DQ (6/9/2020): Modified to point to currentLocNodePtr.
       // SgLocatedNode* previousLocatedNode;
       // previousLocNodePtr = previousLocatedNode;
          previousLocNodePtr = currentLocNodePtr;
#endif
        }
       else
        {
          SgStatement* currentStatement = dynamic_cast<SgStatement*>(currentLocNodePtr);
          ROSE_ASSERT (currentStatement != NULL);
          SgStatement* parentStatement = isSgStatement(currentStatement->get_parent());

       // We can't enforce this since currentStatement may be SgGlobal and the parent 
       // is SgSourceFile (which is not a SgStatement).
       // ROSE_ASSERT (parentStatement != NULL);
          ROSE_ASSERT ( (parentStatement != NULL) || (isSgGlobal(currentStatement) != NULL) );

#if 0
       // DQ (4/30/2020): We no long need this in the new simplified support for CPP directivces and comments and unparsing of header files.
       // printf ("parentStatement = %s \n",parentStatement->sage_class_name());
          previousLocatedNodeMap[currentFileId] = parentStatement;
#endif
       // Supports assertions at end of function
          previousLocNodePtr = parentStatement;
        }

       // Liao 6/10/2020, special handling for Fortran subroutine init-name, which is compiler generated and cannot be unparsed directly.
       // It cannot be used as an anchor node for preprocessing information.
       // In this case, we use SgBasicBlock of the SgFunctionDefinition as the previous located node.
       // This is to address the lost comment problem as shown in test2020_comment_1.f90 .
       if (SageInterface::is_Fortran_language ())
       {
         if (SgInitializedName * init_name =  isSgInitializedName(currentLocNodePtr))
         {
           if (isSgProcedureHeaderStatement(init_name->get_parent()))
           {
             previousLocNodePtr = init_name->get_scope();

          // DQ (7/3/2020): We no longer support this map (in the new design for comment and CPP directive handling).
          // previousLocatedNodeMap[currentFileId] = previousLocNodePtr;
           }
         }
       }

#if 1
  // Nodes that should not have comments attached (since they are not unparsed directly 
  // within the generation of the source code by the unparser (no associated unparse functions))
     ROSE_ASSERT (dynamic_cast<SgForInitStatement*>     (previousLocNodePtr) == NULL);
     ROSE_ASSERT (dynamic_cast<SgTypedefSeq*>           (previousLocNodePtr) == NULL);
     ROSE_ASSERT (dynamic_cast<SgCatchStatementSeq*>    (previousLocNodePtr) == NULL);
     ROSE_ASSERT (dynamic_cast<SgFunctionParameterList*>(previousLocNodePtr) == NULL);
     ROSE_ASSERT (dynamic_cast<SgCtorInitializerList*>  (previousLocNodePtr) == NULL);
#else
          printf ("Commented out calls to dynamic_cast on previousLocNodePtr \n");
#endif
   }


// ROSEAttributesList* AttachPreprocessingInfoTreeTrav::buildCommentAndCppDirectiveList ( bool use_Wave, std::string fileNameForDirectivesAndComments )
ROSEAttributesList* 
AttachPreprocessingInfoTreeTrav::buildCommentAndCppDirectiveList ( bool use_Wave, SgSourceFile* sourceFile, std::string fileNameForDirectivesAndComments )
   {
  // This function abstracts the collection of comments and CPP directives into a list.  
  // The list is then used to draw from as the AST is traversed and the list elements 
  // are woven into the AST.
#if 0
     printf ("Inside of AttachPreprocessingInfoTreeTrav::buildCommentAndCppDirectiveList(use_Wave = %s) file = %s \n",use_Wave ? "true" : "false",fileNameForDirectivesAndComments.c_str());
#endif

#if 0
  // DQ (8/17/2020): I think this function is no longer used.
     printf ("This function: buildCommentAndCppDirectiveList() is no longer used! \n");
     ROSE_ASSERT(false);
#endif

  // DQ (4/29/2020): Introduce test for recursive call.
     static bool isRecursiveCall = false;
     ROSE_ASSERT(isRecursiveCall == false);

     isRecursiveCall = true;


  // Liao 4/26/2010 support --enable-only-c
#ifdef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
  // DQ (12/10/2007): Declare Fortran specific lexical pass function explicitly.
  // extern int getFortranFixedFormatPreprocessorDirectives( std::string fileName );
  // extern int getFortranFreeFormatPreprocessorDirectives ( std::string fileName );
// #ifdef USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT
     extern std::list <stream_element*>* getFortranFixedFormatPreprocessorDirectives( std::string fileName );
     extern std::list <stream_element*>* getFortranFreeFormatPreprocessorDirectives ( std::string fileName );
// #endif
#endif

     ROSE_ASSERT(sourceFile != NULL);
     string fileNameForTokenStream = fileNameForDirectivesAndComments;

  // DQ (11/2/2019): Avoid redundant calls to getListOfAttributes().
  // ROSEAttributesList* returnListOfAttributes = new ROSEAttributesList();
     ROSEAttributesList* returnListOfAttributes = NULL;
     ROSEAttributesListContainerPtr filePreprocInfo = sourceFile->get_preprocessorDirectivesAndCommentsList();

#if 0
     printf ("filePreprocInfo = %p \n",filePreprocInfo);
     printf ("sourceFile = %p \n",sourceFile);
     printf ("sourceFile->get_file_info() = %p \n",sourceFile->get_file_info());
     printf ("sourceFile->get_file_info()->get_filename() = %s \n",sourceFile->get_file_info()->get_filename());
#endif

  // DQ (12/3/2019): Need to test for filePreprocInfo != NULL when compiling Fortran code.
     if (sourceFile->get_Fortran_only() == false)
        {
     if (filePreprocInfo != NULL)
        {
     if (filePreprocInfo->getList().find(sourceFile->get_file_info()->get_filename()) == filePreprocInfo->getList().end())
        {
#if 0
          int currentFileNameId = sourceFile->get_file_info()->get_file_id();
          printf ("Generating a new ROSEAttributesList: currentFileNameId = %d \n",currentFileNameId);
#endif

#ifdef ROSE_BUILD_CPP_LANGUAGE_SUPPORT
       // DQ (11/2/2019): A call to getListOfAttributes() will generate infinite recursion.
       // returnListOfAttributes = getListOfAttributes(currentFileNameId);
          returnListOfAttributes = getPreprocessorDirectives(fileNameForDirectivesAndComments);
#endif
#if 0
          printf ("DONE: Generating a new ROSEAttributesList: currentFileNameId = %d \n",currentFileNameId);
          printf (" --- returnListOfAttributes->getList().size() = %" PRIuPTR " \n",returnListOfAttributes->getList().size());
#endif

#if 0
       // DQ (11/2/2019): Adding debugging code.
          returnListOfAttributes->display("Display after DONE: Generating a new ROSEAttributesList");
#endif
       // DQ (11/2/2019): Add the new attributes to the list.
          filePreprocInfo->getList()[sourceFile->get_file_info()->get_filename()] = returnListOfAttributes;
        }
       else
        {
#if 0
          printf ("Using the existing ROSEAttributesList (setting the returnListOfAttributes) \n");
#endif
          returnListOfAttributes = filePreprocInfo->getList()[sourceFile->get_file_info()->get_filename()];
#if 0
          printf ("DONE: Using the existing ROSEAttributesList (setting the returnListOfAttributes) \n");
          printf (" --- returnListOfAttributes->getList().size() = %" PRIuPTR " \n",returnListOfAttributes->getList().size());
#endif
        }
        }
        }

  // Build an empty list while we skip the translation of tokens
  // returnListOfAttributes = new ROSEAttributesList();

  // If this is a CPP processed file then modify the name to reflect that the CPP output is
  // to be process and it was assigned a different file name (with "_preprocessed" suffix).
#if 0
     printf ("Inside of buildCommentAndCppDirectiveList(): fileNameForDirectivesAndComments = %s \n",fileNameForDirectivesAndComments.c_str());
     printf ("                                             fileNameForTokenStream           = %s \n",fileNameForTokenStream.c_str());
#endif

  // currentFileNameId = currentFilePtr->get_file_info()->get_file_id();
  // ROSE_ASSERT(currentFileNameId >= 0);

  // Note that we need the SgSourceFile so that we get information about what language type this is to support.
  // SgSourceFile* currentFilePtr = sourceFile;

#if 0
     printf ("In buildCommentAndCppDirectiveList(): use_Wave = %s \n",use_Wave ? "true" : "false");
#endif

     if (use_Wave == false)
        {
       // std::cerr << "Not using wave" << std::endl;
       // DQ (4/12/2007): Introduce tracking of performance of ROSE.
          TimingPerformance timer ("AST evaluateInheritedAttribute (use_Wave == false):");


          //AS(4/3/09): FIXME: We are doing this quick fix because the fileNameForDirectivesAndComments is
          //incorrect for Fortran
          //PC(08/17/2009): Now conditional on the output language, otherwise breaks -rose:collectAllCommentsAndDirectives
          if (sourceFile->get_outputLanguage() == SgFile::e_Fortran_language)
             {
               fileNameForDirectivesAndComments = sourceFile->get_sourceFileNameWithPath();
               fileNameForTokenStream           = fileNameForDirectivesAndComments;
             }

#if 0
          printf ("In buildCommentAndCppDirectiveList(): sourceFile->get_Fortran_only() = %s \n",sourceFile->get_Fortran_only() ? "true" : "false");
#endif

          if (sourceFile->get_Fortran_only() == true)
             {
            // For Fortran CPP code you need to preprocess the code into an intermediate file in order to pass it through
            // the Fortran frontend. This is because for Fortan everything is ONE file. 
               if (sourceFile->get_requires_C_preprocessor() == true)
                  {
                    fileNameForDirectivesAndComments = sourceFile->generate_C_preprocessor_intermediate_filename(fileNameForDirectivesAndComments);
                  }

            // DQ (12/3/2019): I think this is required for Fortran support.
            // ROSE_ASSERT(returnListOfAttributes == NULL);
            // returnListOfAttributes = new ROSEAttributesList();
               if (returnListOfAttributes == NULL)
                  {
#if 0
                    printf ("Building ROSEAttributesList() to initialized NULL returnListOfAttributes \n");
#endif
                    returnListOfAttributes = new ROSEAttributesList();
                  }
               ROSE_ASSERT(returnListOfAttributes != NULL);

#ifdef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT

// #ifdef USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT
            // This is either of two different kinds of Fortran programs: fixed format or free format
            //    * fix format is generally used for older Fortran code, F77 and earlier, and 
            //    * free format is generall used for newer codes, F90 and later
            //    * however this is a general rule, specifically a F03 code can use fixed format.

            // If it is not explicitly fixed form, then assume it is free form input.
            // if (currentFilePtr->get_fixedFormat() == true)
               if (sourceFile->get_inputFormat() == SgFile::e_fixed_form_output_format)
                  {
                    if ( SgProject::get_verbose() > 1 )
                       {
                         printf ("Fortran code assumed to be in fixed format form (skipping translation of tokens) \n");
                       }

                 // For now we call the lexical pass on the fortran file, but we don't yet translate the tokens.
                 // returnListOfAttributes       = getPreprocessorDirectives( Sg_File_Info::getFilenameFromID(currentFileNameId) );
                 // getFortranFixedFormatPreprocessorDirectives( Sg_File_Info::getFilenameFromID(currentFileNameId) );
                 // LexTokenStreamTypePointer lex_token_stream = getFortranFixedFormatPreprocessorDirectives( Sg_File_Info::getFilenameFromID(currentFileNameId) );
                    LexTokenStreamTypePointer lex_token_stream = NULL;
#ifdef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
                    lex_token_stream = getFortranFixedFormatPreprocessorDirectives( fileNameForTokenStream );
#endif
                    ROSE_ASSERT(lex_token_stream != NULL);

                    if ( SgProject::get_verbose() > 1 )
                       {
                         printf ("DONE: getFortranFixedFormatPreprocessorDirectives() \n");
                       }

                 // Attach the token stream to the AST
                    returnListOfAttributes->set_rawTokenStream(lex_token_stream);
#if 1
                 // DQ (11/23/2008): This is the new support to collect CPP directives and comments from Fortran applications.
                 // printf ("Calling collectPreprocessorDirectivesAndCommentsForAST() to collect CPP directives for fileNameForDirectivesAndComments = %s \n",fileNameForDirectivesAndComments.c_str());
                    returnListOfAttributes->collectPreprocessorDirectivesAndCommentsForAST(fileNameForDirectivesAndComments,ROSEAttributesList::e_Fortran77_language);
                 // printf ("DONE: Calling collectPreprocessorDirectivesAndCommentsForAST() to collect CPP directives for fileNameForDirectivesAndComments = %s \n",fileNameForDirectivesAndComments.c_str());
#endif
#if 0
                 // DQ (11/19/2008): This code has been replaced by collectPreprocessorDirectivesAndCommentsForAST().
                 // Process the raw token stream into the PreprocessorDirectives and Comment list required to be inserted into the AST.
                 // returnListOfAttributes->collectFixedFormatPreprocessorDirectivesAndCommentsForAST(currentFilePtr->get_sourceFileNameWithPath());
                    returnListOfAttributes->collectFixedFormatPreprocessorDirectivesAndCommentsForAST(fileNameForDirectivesAndComments);
#endif
                  }
                 else
                  {
                 // int currentFileNameId = currentFilePtr->get_file_info()->get_file_id();
                 // For now we call the lexical pass on the fortran file, but we don't yet translate the tokens.
                 // returnListOfAttributes       = getPreprocessorDirectives( Sg_File_Info::getFilenameFromID(currentFileNameId) );
                 // getFortranFreeFormatPreprocessorDirectives( Sg_File_Info::getFilenameFromID(currentFileNameId) );
                 // string fileNameForTokenStream = Sg_File_Info::getFilenameFromID(currentFileNameId);

                    LexTokenStreamTypePointer lex_token_stream = NULL;
#if 0
                    printf ("fileNameForTokenStream = %s \n",fileNameForTokenStream.c_str());
                    printf (" --- returnListOfAttributes->getList().size() = %" PRIuPTR " \n",returnListOfAttributes->getList().size());
#endif
#ifdef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
                    lex_token_stream = getFortranFreeFormatPreprocessorDirectives( fileNameForTokenStream );
#endif
                    ROSE_ASSERT(lex_token_stream != NULL);

                 // DQ (12/3/2019): Added test to support debugging Fortran support.
                    ROSE_ASSERT(returnListOfAttributes != NULL);

                 // Attach the token stream to the AST
                    returnListOfAttributes->set_rawTokenStream(lex_token_stream);
                    ROSE_ASSERT(returnListOfAttributes->get_rawTokenStream() != NULL);
#if 0
                    printf ("Fortran Token List Size: returnListOfAttributes->get_rawTokenStream()->size() = %" PRIuPTR " \n",returnListOfAttributes->get_rawTokenStream()->size());
                    printf (" --- returnListOfAttributes->getList().size() = %" PRIuPTR " \n",returnListOfAttributes->getList().size());
#endif
                 // DQ (11/23/2008): This is the new support to collect CPP directives and comments from Fortran applications.
                 // printf ("Calling collectPreprocessorDirectivesAndCommentsForAST() to collect CPP directives for fileNameForDirectivesAndComments = %s \n",fileNameForDirectivesAndComments.c_str());
                    returnListOfAttributes->collectPreprocessorDirectivesAndCommentsForAST(fileNameForDirectivesAndComments,ROSEAttributesList::e_Fortran9x_language);

#if 0
                    printf ("Done with processing of separate lexical pass to gather comments and CPP directives \n");
                    ROSE_ASSERT(false);
#endif
#if 0
                 // DQ (11/19/2008): This code has been replaced by collectPreprocessorDirectivesAndCommentsForAST().
                    printf ("Calling generatePreprocessorDirectivesAndCommentsForAST() for fileNameForDirectivesAndComments = %s \n",fileNameForDirectivesAndComments.c_str());
                    returnListOfAttributes->generatePreprocessorDirectivesAndCommentsForAST(fileNameForDirectivesAndComments);
#endif
                  }

#if 0
               printf ("Done with processing of separate lexical pass to gather Fortran specific CPP directives and comments from the token stream \n");
               ROSE_ASSERT(false);
#endif

// #else // for !USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT
//               fprintf(stderr, "Fortran parser not enabled \n");
//               ROSE_ABORT();
// #endif // USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT
#endif // for #ifdef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
             }
            else
             {
            // Else we assume this is a C or C++ program (for which the lexical analysis is identical)
            // The lex token stream is now returned in the ROSEAttributesList object.

            // DQ (8/19/2019): comment this redundant call out!  We are trying to optimize the performance of the header file unparsing.
#if 0
            // DQ (8/19/2019): This is a redundent call and in processing each header (for hundreds to thousands of headers) it will be a performance issue.
            // DQ (11/23/2008): This is part of CPP handling for Fortran, but tested on C and C++ codes additionally, (it is redundant for C and C++).
            // This is a way of testing the extraction of CPP directives (on C and C++ codes, so that it is more agressively tested).
            // Since this is a redundant test, it can be removed in later development (its use is only a performance issue).
            // returnListOfAttributes = new ROSEAttributesList();

            // This call is just a test, this function is defined for use on Fortran.  For C and C++ we have alternative methods to extract the CPP directives and comments.
#if 0
               printf ("REDUNDENT: Call collectPreprocessorDirectivesAndCommentsForAST to test C and C++ preprocessor directive collection: fileNameForDirectivesAndComments = %s \n",
                    fileNameForDirectivesAndComments.c_str());
#endif
               returnListOfAttributes->collectPreprocessorDirectivesAndCommentsForAST(fileNameForDirectivesAndComments,ROSEAttributesList::e_C_language);
#if 0
               printf ("DONE: Call collectPreprocessorDirectivesAndCommentsForAST to test C and C++ preprocessor directive collection \n");
#endif
#endif
            // DQ (8/23/2018): The token stream has not been collected yet (verify).
            // ROSE_ASSERT (returnListOfAttributes->get_rawTokenStream() == NULL);
#if 0
            // DQ (12/4/2019): This fails for the case of a the snippet tests.
            // ROSE_ASSERT (returnListOfAttributes->get_rawTokenStream() != NULL);
               ROSE_ASSERT(returnListOfAttributes != NULL);
               if (returnListOfAttributes->get_rawTokenStream() == NULL)
                  {
                    printf ("NOTE: returnListOfAttributes->get_rawTokenStream() == NULL \n");
                  }
#endif
#if 0
            // DQ (11/202019): comment this redundant call to getPreprocessorDirectives().

#if 0
               printf ("REDUNDENT: Call collectPreprocessorDirectivesAndCommentsForAST to test C and C++ preprocessor directive collection: fileNameForDirectivesAndComments = %s \n",
                    fileNameForDirectivesAndComments.c_str());
#endif

            // This function has been modified to clear any existing list of PreprocessingInfo*
            // objects (so that we can test the function: collectPreprocessorDirectivesAndCommentsForAST()).
            // returnListOfAttributes = getPreprocessorDirectives( Sg_File_Info::getFilenameFromID(currentFileNameId) );
#if 0
               printf ("Calling lex or wave based mechanism for collecting CPP directives, comments, and token stream:  fileNameForDirectivesAndComments = %s \n",
                    fileNameForDirectivesAndComments.c_str());
#endif

            // DQ (8/19/2019): As a performance optimization we should record that we want to collect the comments and
            // CPP directives from this file and only do so later if we discover that we have to unparse this file.
               returnListOfAttributes = getPreprocessorDirectives(fileNameForDirectivesAndComments);
#if 0
               printf ("DONE: Calling lex or wave based mechanism for collecting CPP directives, comments, and token stream \n");
#endif
#endif
#if 0
            // DQ (12/4/2019): This fails for the snippet code support.
            // DQ (8/23/2018): At this point the token stream has been collected (verify).
            // Note: to debug this support the token stream is always collected (the process 
            // is fast, but making it optional in the future might be helpful).
               ROSE_ASSERT (returnListOfAttributes->get_rawTokenStream() != NULL);
#else
               if (returnListOfAttributes == NULL)
                  {
#if 0
                    printf ("Found returnListOfAttributes == NULL, calling getPreprocessorDirectives() \n");
#endif
#ifdef ROSE_BUILD_CPP_LANGUAGE_SUPPORT
                    returnListOfAttributes = getPreprocessorDirectives(fileNameForDirectivesAndComments);
#endif
                  }
#endif
             }
        }
       else
        {
       // This is the case of: (use_Wave == true). This mode does NOT work for Fortran code!
          ROSE_ASSERT(sourceFile->get_Fortran_only() == false);

       // AS(011306) fetch the list of attributes from the Wave output
       // int currentFileNameId = currentFilePtr->get_file_info()->get_file_id();
       // std::string currentStringFilename = Sg_File_Info::getFilenameFromID(currentFileNameId);

          delete returnListOfAttributes;
          returnListOfAttributes = new ROSEAttributesList();

       // Copy the ROSEAttributesList from the global mapFilenameToAttributes as the elments that are attached to
       // the AST from the ROSEAttributesList is set to NULL by the attachment process 

          std::map<std::string,ROSEAttributesList* >::iterator currentFileItr = mapFilenameToAttributes.find(fileNameForTokenStream);
          if (currentFileItr != mapFilenameToAttributes.end())
             {
            // If there already exists a list for the current file then get that list.
               ROSE_ASSERT( currentFileItr->second != NULL);

               ROSEAttributesList* existingReturnListOfAttributes = currentFileItr->second;

               for (std::vector<PreprocessingInfo*>::iterator it_1 = existingReturnListOfAttributes->getList().begin(); it_1 != existingReturnListOfAttributes->getList().end(); ++it_1)
                  {
                    returnListOfAttributes->addElement(**it_1);
                  }
             }
        }

     ROSE_ASSERT(returnListOfAttributes != NULL);

  // DQ (12/15/2012): Generate the list of file ids to be considered equivalent to the input source file's filename.
     returnListOfAttributes->generateFileIdListFromLineDirectives();

  // DQ (9/29/2013): Check the generated returnListOfAttributes for tokens.
     if (returnListOfAttributes->get_rawTokenStream() != NULL)
        {
#if 0
          printf ("Found the raw token stream in ROSE! returnListOfAttributes->get_rawTokenStream() = %p \n",returnListOfAttributes->get_rawTokenStream());
#endif
#if 0
          LexTokenStreamType & tokenList = *(returnListOfAttributes->get_rawTokenStream());
          printf ("In AttachPreprocessingInfoTreeTrav::buildCommentAndCppDirectiveList(): Output token list (number of CPP directives and comments = %d): \n",returnListOfAttributes->size());
          printf ("In AttachPreprocessingInfoTreeTrav::buildCommentAndCppDirectiveList(): Output token list (number of tokens = %" PRIuPTR "): \n",tokenList.size());

       // Debugging output for token handling.
          int counter = 0;
          for (LexTokenStreamType::iterator i = tokenList.begin(); i != tokenList.end(); i++)
             {
               printf ("   --- token #%d token = %p \n",counter,(*i)->p_tok_elem);
               if ((*i)->p_tok_elem != NULL)
                  {
                    printf ("   --- --- token id = %d token = %s \n",(*i)->p_tok_elem->token_id,(*i)->p_tok_elem->token_lexeme.c_str());
                  }

            // DQ (9/29/2013): Added support for reference to the PreprocessingInfo object in the token stream.
               printf ("   --- token #%d p_preprocessingInfo = %p \n",counter,(*i)->p_preprocessingInfo);

               printf ("   --- token #%d beginning_fpi line = %d column = %d \n",counter,(*i)->beginning_fpi.line_num,(*i)->beginning_fpi.column_num);
               printf ("   --- token #%d ending_fpi    line = %d column = %d \n",counter,(*i)->ending_fpi.line_num,(*i)->ending_fpi.column_num);

               counter++;
             }
#endif
#if 0
          printf ("Exiting as a test in evaluation of token list in ROSE! \n");
          ROSE_ASSERT(false);
#endif
        }

#if 0
     printf ("Leaving AttachPreprocessingInfoTreeTrav::buildCommentAndCppDirectiveList(use_Wave = %s) file = %s \n",use_Wave ? "true" : "false",fileNameForDirectivesAndComments.c_str());
#endif

  // DQ (4/29/2020): Introduce test for recursive call.
     isRecursiveCall = false;

     return returnListOfAttributes;
   }


ROSEAttributesList*
AttachPreprocessingInfoTreeTrav::getListOfAttributes ( int currentFileNameId )
   {
  // This function will get the list of CPP directives and comments if it exists, 
  // or build it if required.  The function is called each time we come to a IR 
  // node as part of the traversal. If it is a new IR node (from a file not previously 
  // visited) then the associated file will be read to gather its CPP directives and 
  // comments.

  // DQ (5/1/2020): This is now a data member.
  // ROSEAttributesList* currentListOfAttributes = NULL;

#if 0
     printf ("In AttachPreprocessingInfoTreeTrav::getListOfAttributes() currentFileNameId = %d file = %s \n",currentFileNameId,Sg_File_Info::getFilenameFromID(currentFileNameId).c_str());
#endif

  // DQ (4/29/2020): Introduce test for recursive call.
     static bool isRecursiveCall = false;
     ROSE_ASSERT(isRecursiveCall == false);

     isRecursiveCall = true;

#if 0
  // DQ (4/29/2020): Introduce test for redundant calls to this function.
     static set<int> filenameIdSet;
     if (filenameIdSet.find(currentFileNameId) == filenameIdSet.end())
        {
          filenameIdSet.insert(currentFileNameId);
        }
       else
        {
          printf ("Redundant call to AttachPreprocessingInfoTreeTrav::getListOfAttributes() for currentFileNameId = %d file = %s \n",currentFileNameId,Sg_File_Info::getFilenameFromID(currentFileNameId).c_str());

#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }
#endif

  // DQ (6/12/2020): This should always be true in the new design. This is not always true.
  // ROSE_ASSERT(currentFileNameId >= 0);

  // DQ (5/19/2013): Added test... only valid for specific test codes with appropriate CPP directives.
  // ROSE_ASSERT(currentListOfAttributes != NULL);

#if 0
     ROSE_ASSERT(currentListOfAttributes != NULL);
     printf ("Leaving AttachPreprocessingInfoTreeTrav::getListOfAttributes(): currentListOfAttributes = %p currentListOfAttributes->size() = %d \n",
          currentListOfAttributes,currentListOfAttributes != NULL ? currentListOfAttributes->size() : -1);
#endif

  // DQ (4/29/2020): Introduce test for recursive call.
     isRecursiveCall = false;

     return currentListOfAttributes;
   }


// Member function: evaluateInheritedAttribute
AttachPreprocessingInfoTreeTraversalInheritedAttrribute
AttachPreprocessingInfoTreeTrav::evaluateInheritedAttribute ( SgNode *n, AttachPreprocessingInfoTreeTraversalInheritedAttrribute inheritedAttribute)
   {
  // This is this inherited attribute evaluation.  It is executed as a preorder traversal 
  // of the AST.  We don't use anything in the inherited attribute at present, however,
  // some actions have to be executed as we first visit an IR node and some have to be
  // executed as we last vist an IR node (post-order; see the evaluateSynthezidedAttribute()
  // member function).

  // DQ (11/20/2019): Check this (should be set in constructor).
     ROSE_ASSERT(sourceFile != NULL);

#if 0
     printf ("TOP of AttachPreprocessingInfoTreeTrav::evaluateInheritedAttribute(): n = %p = %s name = %s \n",n,n->class_name().c_str(),SageInterface::get_name(n).c_str());
     printf (" --- previousLocatedNode = %p = %s \n",previousLocatedNode,(previousLocatedNode != NULL) ? previousLocatedNode->class_name().c_str() : "null");
#endif
#if 0
     printf (" --- sourceFile->get_globalScope() = %p \n",sourceFile->get_globalScope());
#endif

#if 0
     printf ("\n\n**********************************************************\n");
     printf ("In AttachPreprocessingInfoTreeTrav::evaluateInheritedAttribute(): n = %p = %s \n",n,n->class_name().c_str());
     SgStatement* currentStatement = isSgStatement(n);
  // if (isSgStatement(n) && (isSgStatement(n)->get_parent() != NULL) )
     if (currentStatement != NULL && (currentStatement->get_parent() != NULL) )
        {
          printf ("     parent = %s \n",currentStatement->get_parent()->class_name().c_str());
          ROSE_ASSERT(currentStatement->get_file_info() != NULL);
#if 0
          currentStatement->get_startOfConstruct()->display("In AttachPreprocessingInfoTreeTrav::evaluateInheritedAttribute(): (START) debug");
#endif
#if 0
          currentStatement->get_endOfConstruct()->display("In AttachPreprocessingInfoTreeTrav::evaluateInheritedAttribute(): (END) debug");
#endif
        }
#endif

     ROSE_ASSERT(n != NULL);
  // printf ("In AttachPreprocessingInfoTreeTrav::evaluateInheritedAttribute(): n = %p = %s \n",n,n->class_name().c_str());
  // SgTemplateFunctionDeclaration* templateDeclaration = isSgTemplateFunctionDeclaration(n);
     SgDeclarationStatement* templateDeclaration              = isSgTemplateFunctionDeclaration(n);
     SgDeclarationStatement* templateInstantiationDeclaration = isSgTemplateInstantiationFunctionDecl(n);

     if (templateDeclaration == NULL) templateDeclaration = isSgTemplateMemberFunctionDeclaration(n);
     if (templateDeclaration == NULL) templateDeclaration = isSgTemplateClassDeclaration(n);
     if (templateDeclaration == NULL) templateDeclaration = isSgTemplateVariableDeclaration(n);

     if (templateDeclaration != NULL)
        {
       // Set the flag in the inherited attribute.
       // printf ("Set the flag for this to be in a template declaration n = %p = %s \n",n,n->class_name().c_str());
          inheritedAttribute.isPartOfTemplateDeclaration = true;
        }
       else
        {
       // DQ (7/1/2014): Added support for detecting when we are in a template instantation.
          if (templateInstantiationDeclaration == NULL) templateInstantiationDeclaration = isSgTemplateInstantiationMemberFunctionDecl(n);
          if (templateInstantiationDeclaration == NULL) templateInstantiationDeclaration = isSgTemplateInstantiationDecl(n);
       // if (templateInstantiationDeclaration == NULL) templateInstantiationDeclaration = isSgTemplateInstantiationVariableDecl(n);
          if (templateInstantiationDeclaration != NULL)
             {
               inheritedAttribute.isPartOfTemplateInstantiationDeclaration = true;
             }
#if 0
          if (inheritedAttribute.isPartOfTemplateDeclaration == true)
             {
            // printf ("This is a part of a template declaration (suppress attachment of comments and CPP directves to template declarations, since they are unparsed as strings for the moment) n = %p = %s \n",n,n->class_name().c_str());
             }
            else
             {
            // printf ("This is not part of a template declaration n = %p = %s \n",n,n->class_name().c_str());
             }
#endif
#if 0
          if (inheritedAttribute.isPartOfTemplateInstantiationDeclaration == true)
             {
               printf ("This is a part of a template Instantiation declaration (suppress attachment of comments and CPP directves to template declarations, since they might not be unparsed) n = %p = %s \n",n,n->class_name().c_str());
             }
            else
             {
            // printf ("This is not part of a template Instantiation declaration n = %p = %s \n",n,n->class_name().c_str());
             }
#endif
        }

  // DQ (8/6/2012): Allow those associated with the declaration and not inside of the template declaration.
  // if (inheritedAttribute.isPartOfTemplateDeclaration == true && templateDeclaration == NULL)
     if ( (inheritedAttribute.isPartOfTemplateDeclaration              == true && templateDeclaration              == NULL) || 
          (inheritedAttribute.isPartOfTemplateInstantiationDeclaration == true && templateInstantiationDeclaration == NULL) )
        {
// #if DEBUG_ATTACH_PREPROCESSING_INFO
#if 0
          printf ("Returning without further processing if we are a part of a template declaration or template instantiation declaration \n");
#endif
          return inheritedAttribute;
        }

  // Check if current AST node is an SgFile object
     SgFile* currentFilePtr = isSgFile(n);
     if (currentFilePtr != NULL)
        {
       // Current AST node is an SgFile object, generate the corresponding list of attributes
#if 0
          printf ("In AttachPreprocessingInfoTreeTrav::evaluateInheritedAttribute(): case of SgFile: n = %p = %s \n",n,n->class_name().c_str());
#endif

#if DEBUG_ATTACH_PREPROCESSING_INFO
          printf ("=== Visiting SgSourceFile node and building current list of attributes === \n");
#endif

       // This entry should not be present, so generate the list.
       // If this is a preprocessed file then change the name so that we generate the correct list for the correct file.
       // int currentFileNameId = currentFilePtr->get_file_info()->get_file_id();
          Sg_File_Info* currentFileInfo = currentFilePtr->get_file_info();
          ROSE_ASSERT(currentFileInfo != NULL);
#if 0
          printf ("(SgSourceFile) sourceFile->get_requires_C_preprocessor()                = %s \n",sourceFile->get_requires_C_preprocessor() == true ? "true" : "false");
          printf ("(SgSourceFile) sourceFile->get_file_info()->get_filename()              = %s \n",sourceFile->get_file_info()->get_filename());
          printf ("(SgSourceFile) sourceFile->get_file_info()->get_filenameString()        = %s \n",sourceFile->get_file_info()->get_filenameString().c_str());
          printf ("(SgSourceFile) sourceFile->get_file_info()->get_physical_filename()     = %s \n",sourceFile->get_file_info()->get_physical_filename().c_str());

          printf ("(SgSourceFile) currentFilePtr->get_requires_C_preprocessor()            = %s \n",currentFilePtr->get_requires_C_preprocessor() == true ? "true" : "false");
          printf ("(SgSourceFile) currentFilePtr->get_file_info()->get_filename()          = %s \n",currentFilePtr->get_file_info()->get_filename());
          printf ("(SgSourceFile) currentFilePtr->get_file_info()->get_filenameString()    = %s \n",currentFilePtr->get_file_info()->get_filenameString().c_str());
          printf ("(SgSourceFile) currentFilePtr->get_file_info()->get_physical_filename() = %s \n",currentFilePtr->get_file_info()->get_physical_filename().c_str());
#endif

       // DQ (11/2/2019): Commenting out this assertion so that we can support attaching comments to header files.
       // ROSE_ASSERT(sourceFile == currentFilePtr);
          if (sourceFile != currentFilePtr)
             {
               printf ("NOTE: sourceFile = %p currentFilePtr = %p \n",sourceFile,currentFilePtr);
             }
#if 0
          printf ("(SgSourceFile) currentFilePtr->generate_C_preprocessor_intermediate_filename(sourceFile->get_file_info()->get_filename()) = %s \n",
               currentFilePtr->generate_C_preprocessor_intermediate_filename(sourceFile->get_file_info()->get_filename()).c_str());
#endif

        // DQ (10/25/2019): This is not a correct assertion for Fortran code.
        // DQ (9/23/2019): For C/C++ code this should be false (including all headers).
        // ROSE_ASSERT(sourceFile->get_requires_C_preprocessor() == false);

       // DQ (12/2/2018): This fails for the C/C++ snippet insertion tests.
       // DQ (12/2/2018): This fails for Fortran.
       // DQ (9/5/2018): We should have already set the preprocessorDirectivesAndCommentsList, checked in getTokenStream().
       // ROSE_ASSERT(currentFilePtr->get_preprocessorDirectivesAndCommentsList() != NULL);
       // if (SageInterface::is_Fortran_language() == false)
          if (SageInterface::is_C_language() == true || SageInterface::is_Cxx_language() == true)
             {
            // ROSE_ASSERT(currentFilePtr->get_preprocessorDirectivesAndCommentsList() != NULL);
             }

#if 0
          int currentFileNameId = (currentFilePtr->get_requires_C_preprocessor() == true) ? 
                                  Sg_File_Info::getIDFromFilename(currentFilePtr->generate_C_preprocessor_intermediate_filename(sourceFile->get_file_info()->get_filename())) : 
                                  currentFileInfo->get_file_id();
#else
#if 0
          int currentFileNameId = (currentFilePtr->get_requires_C_preprocessor() == true) ? 
                                  Sg_File_Info::getIDFromFilename(currentFilePtr->generate_C_preprocessor_intermediate_filename(sourceFile->get_file_info()->get_filename())) : 
                                  currentFileInfo->get_physical_file_id();
#else
       // DQ (9/7/2018): Actually the default for C/C++ code should be that get_requires_C_preprocessor() == false, the other case is for C preprocessed fortran code.
          int currentFileNameId = (currentFilePtr->get_requires_C_preprocessor() == true) ? 
                               // Sg_File_Info::getIDFromFilename(sourceFile->get_file_info()->get_filenameString()) : 
                                  Sg_File_Info::getIDFromFilename(currentFilePtr->generate_C_preprocessor_intermediate_filename(sourceFile->get_file_info()->get_filename())) : 
                                  currentFileInfo->get_physical_file_id(source_file_id);
#endif
#endif

#if 0
          printf ("(SgSourceFile) currentFileNameId                                          = %d \n",currentFileNameId);
          printf ("(SgSourceFile) currentFileName for currentFileNameId: filename            = %s \n",Sg_File_Info::getFilenameFromID(currentFileNameId).c_str());
          printf (" --- currentFilePtr->get_header_file_unparsing_optimization_header_file() = %s \n",currentFilePtr->get_header_file_unparsing_optimization_header_file() ? "true" : "false");
          printf (" --- source_file_id                                                       = %d \n",source_file_id);
          printf (" --- currentFilePtr->get_requires_C_preprocessor()                        = %s \n",currentFilePtr->get_requires_C_preprocessor() ? "true" : "false");
#endif

       // DQ (6/29/2020): We should not be adding comments and/or CPP directives to IR nodes that don't have a source position.
          ROSE_ASSERT(currentFileNameId >= 0);

#if 0
          if (currentFileNameId < 0)
             {
               printf ("Error (currentFileNameId < 0): currentFileNameId = %d (return from evaluateInheritedAttribute function) \n",currentFileNameId);
               ROSE_ASSERT(false);

               return inheritedAttribute;
             }
#endif
       // std::cerr << "The filename " << sourceFile->get_file_info()->get_filename() << std::endl;
       // ROSE_ASSERT(attributeMapForAllFiles.find(currentFileNameId) == attributeMapForAllFiles.end());

          if (currentFileNameId != target_source_file_id)
             {
#if 0
               printf ("currentFileNameId != target_source_file_id: currentFileNameId = %d target_source_file_id = %d \n",currentFileNameId,target_source_file_id);
#endif
               return inheritedAttribute;
             }

#if 0
          printf ("currentFilePtr->get_header_file_unparsing_optimization_header_file() = %s \n",
               currentFilePtr->get_header_file_unparsing_optimization_header_file() ? "true" : "false");
#endif

       // DQ (6/3/2020): We now handle one file at a time and the currentListOfAttributes is a member of the traversal.
       // DQ (8/19/2019): Avoid processing this if we are optimizing the header file unparsing and only processing the header files.
       // ROSEAttributesList* currentListOfAttributes = NULL;
#if 0
          printf ("In AttachPreprocessingInfoTreeTrav::evaluateInheritedAttribute(): filePreprocInfo = %p \n",filePreprocInfo);
#endif

#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif

#if 0
          printf ("Exiting as a test so that we can get the token information attached to the SgSourceFile \n");
          ROSE_ASSERT(false);
#endif

#if 0
       // DQ (11/2/2019): Force processing to exist afer a selected header file is processed.
             {
            // SgSourceFile* sourceFile = isSgSourceFile(currentFilePtr);
               bool isHeaderFile = sourceFile->get_isHeaderFile();
#if 0
               printf ("isHeaderFile = %s \n",isHeaderFile ? "true" : "false");
#endif
               if (isHeaderFile == true)
                  {
                    SgIncludeFile* includeFile = sourceFile->get_associated_include_file();
                    ROSE_ASSERT(includeFile != NULL);
                    ROSE_ASSERT(includeFile->get_source_file() == sourceFile);

                    if (includeFile->get_source_file()->getFileName() == "/home/quinlan1/ROSE/git_rose_development/tests/nonsmoke/functional/CompileTests/UnparseHeadersTests/test4/SimpleInternal.h")
                       {
                         printf ("Exiting as a test after nested traversal over the global scope in the include file \n");
                         ROSE_ASSERT(false);
                       }
                  }
             }
#endif
        }

  // Move attributes from the list of attributes into the collection of the current AST nodes,
  // we only consider statements for the moment, but this needs to be refined further on.
  // Probably we will have to consider each SgLocatedNode IR node within the AST.
  // if (dynamic_cast<SgStatement*>(n) != NULL)
     SgStatement* statement = isSgStatement(n);
  // Liao 11/2/2010, Ideally we should put all SgLocatedNode here,
  // But we start with statements and initialized names first
     SgInitializedName * i_name = isSgInitializedName (n); 
     SgAggregateInitializer * a_initor = isSgAggregateInitializer (n);

#if 0
     printf ("statement = %p i_name = %p a_initor = %p \n",statement,i_name,a_initor);
#endif
#if 0
     if (statement != NULL)
        {
          printf (" --- Statement = %p = %s \n",statement,statement->class_name().c_str());
        }
       else
        {
          if (i_name != NULL)
             {
               printf (" --- InitializedName = %p = %s \n",i_name,i_name->class_name().c_str());
             }
            else
             {
               if (a_initor != NULL)
                  {
                    printf (" --- AggregateInitializer = %p = %s \n",a_initor,a_initor->class_name().c_str());
                  }
             }
        }
#endif

     if (statement != NULL || i_name != NULL || a_initor != NULL)
        {
          SgLocatedNode* currentLocNodePtr = NULL;
          int line = 0;

       // DQ (12/9/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
          int col  = 0;

       // The following should always work since each statement is a located node
       // currentLocNodePtr = dynamic_cast<SgLocatedNode*>(n);
          currentLocNodePtr = isSgLocatedNode(n);
          ROSE_ASSERT(currentLocNodePtr != NULL);

#if 0
          printf ("&&&&&&&&&&&&&&&& currentLocNodePtr = %p = %s \n",currentLocNodePtr,currentLocNodePtr->class_name().c_str());
#endif

       // Attach the comments only to nodes from the same file
          ROSE_ASSERT(currentLocNodePtr->get_file_info() != NULL);
       // int currentFileNameId = currentLocNodePtr->get_file_info()->get_file_id();
          Sg_File_Info* currentFileInfo = currentLocNodePtr->get_file_info();
          ROSE_ASSERT(currentFileInfo != NULL);

       // DQ (12/2/2018): Oddly enough, this case does not appear to fail in the C/C++ snippet insertion tests.
       // DQ (12/2/2018): This fails for Fortran.
       // DQ (9/7/2018): Assert this as default for C/C++ file processing tests only (remove later).
       // ROSE_ASSERT(sourceFile->get_requires_C_preprocessor() == false);
       // if (SageInterface::is_Fortran_language() == false)
          if (SageInterface::is_C_language() == true || SageInterface::is_Cxx_language() == true)
             {
               ROSE_ASSERT(sourceFile->get_requires_C_preprocessor() == false);
             }

#if 0
          printf ("In AttachPreprocessingInfoTreeTrav::evaluateInheritedAttribute(): Calling Sg_File_Info::getIDFromFilename() \n");
          printf (" --- sourceFile->get_requires_C_preprocessor() = %s \n",sourceFile->get_requires_C_preprocessor() ? "true" : "false");
          printf (" --- source_file_id = %d \n",source_file_id);
#endif
#if 0
          int currentFileNameId = (sourceFile->get_requires_C_preprocessor() == true) ? 
                                   Sg_File_Info::getIDFromFilename(sourceFile->generate_C_preprocessor_intermediate_filename(sourceFile->get_file_info()->get_filename())) : 
                                   currentFileInfo->get_file_id();
#else
          int currentFileNameId = (sourceFile->get_requires_C_preprocessor() == true) ? 
                                   Sg_File_Info::getIDFromFilename(sourceFile->generate_C_preprocessor_intermediate_filename(sourceFile->get_file_info()->get_filename())) : 
                                   currentFileInfo->get_physical_file_id(source_file_id);
#endif

#if 0
          printf ("(SgStatement) currentFileNameId = %d \n",currentFileNameId);
          printf ("(SgStatement) currentFileName for currentFileNameId = %s \n",Sg_File_Info::getFilenameFromID(currentFileNameId).c_str());
          printf (" --- we should have alread collected all comments and CPP directives \n");
#endif

       // DQ (11/2/2019): Adding debugging code.
       // if (currentFileNameId == 0)
#if 0
          printf ("sourceFile->get_file_info()->get_filename() = %s \n",sourceFile->get_file_info()->get_filename());
#endif
#if 0
          if (sourceFile->get_file_info()->get_filenameString() == "/home/quinlan1/ROSE/git_rose_development/tests/nonsmoke/functional/CompileTests/UnparseHeadersTests/test4/Simple4.C")
             {
               printf ("This can be a redundant call to getListOfAttributes() \n");

               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
             }
#endif
       // DQ (4/29/2020): This is a redundnat call but it.
       // DQ (11/2/2019): This is the call that can be redundant.
          ROSEAttributesList* currentListOfAttributes = getListOfAttributes(currentFileNameId);

       // DQ (8/19/2019): The comments and CPP directives are being attached to the AST twice for those statements in the source file.
       // We need to prevent this by setting currentListOfAttributes = NULL when doing the header file optimization within the header file phase.
          ROSE_ASSERT(sourceFile != NULL);
          if (sourceFile->get_header_file_unparsing_optimization_header_file() == true)
             {
#if 0
#if 0
               printf ("&&&&& Set currentListOfAttributes = NULL for the header file phase of the header file optimization \n");
#endif
               currentListOfAttributes = NULL;
#else
#if 0
               printf ("&&&&& Commented out setting currentListOfAttributes = NULL for header file optimization \n");
#endif
#endif
             }

#if 0
          printf ("currentListOfAttributes = %p \n",currentListOfAttributes);
#endif

       // DQ (8/22/2018): This can be NULL!
       // ROSE_ASSERT(currentListOfAttributes != NULL);

       // If currentListOfAttributes == NULL then this was not an IR node from a file where we wanted to include CPP directives and comments.
          if (currentListOfAttributes != NULL)
             {
#if 0
               printf ("currentListOfAttributes = %p size = %d filename = %s \n",currentListOfAttributes,currentListOfAttributes->size(),currentListOfAttributes->getFileName().c_str());
#endif
            // DQ (6/20/2005): Compiler generated is not enough, it must be marked for output explicitly
            // bool isCompilerGenerated = currentLocNodePtr->get_file_info()->isCompilerGenerated();
               bool isCompilerGenerated = currentLocNodePtr->get_file_info()->isCompilerGeneratedNodeToBeUnparsed();

            // JJW (6/25/2008): These are always flagged as "to be unparsed", even if they are not 
            // unparsed because their corresponding declarations aren't unparsed
               if (isSgClassDefinition(currentLocNodePtr) || isSgFunctionDefinition(currentLocNodePtr))
                  {
                    SgLocatedNode* ln = isSgLocatedNode(currentLocNodePtr->get_parent());
                    Sg_File_Info* parentFi = ln ? ln->get_file_info() : NULL;
                    if (parentFi && parentFi->isCompilerGenerated() && !parentFi->isCompilerGeneratedNodeToBeUnparsed())
                       {
                         isCompilerGenerated = false;
                       }
                  }

               bool isTransformation = currentLocNodePtr->get_file_info()->isTransformation();

            // Try to not call get_filename() if it would be inappropriate (either when isCompilerGenerated || isTransformation)

#if 0
            // DQ (10/27/2007): Initialized to -1 upon suggestion by Andreas.
               int fileIdForOriginOfCurrentLocatedNode = -1;
               if ( !isCompilerGenerated && !isTransformation )
                  {
                 // fileIdForOriginOfCurrentLocatedNode = currentLocNodePtr->get_file_info()->get_file_id();
                    Sg_File_Info* currentFileInfo = currentLocNodePtr->get_file_info();
                    ROSE_ASSERT(currentFileInfo != NULL);

#if 0
                    printf ("sourceFile->get_requires_C_preprocessor() = %s \n",sourceFile->get_requires_C_preprocessor() ? "true" : "false");
                    printf (" --- currentFileInfo->get_physical_file_id(source_file_id) = %d \n",currentFileInfo->get_physical_file_id(source_file_id));
                    printf (" --- source_file_id                                                 = %d \n",source_file_id);
#endif

                 // DQ (9/20/2013): Fixing up references to get_file_id() to use get_physical_file_id().
                 // fileIdForOriginOfCurrentLocatedNode = (sourceFile->get_requires_C_preprocessor() == true) ? 
                 //                         Sg_File_Info::getIDFromFilename(sourceFile->generate_C_preprocessor_intermediate_filename(sourceFile->get_file_info()->get_filename())) : 
                 //                         currentFileInfo->get_file_id();
                    fileIdForOriginOfCurrentLocatedNode = (sourceFile->get_requires_C_preprocessor() == true) ? 
                                            Sg_File_Info::getIDFromFilename(sourceFile->generate_C_preprocessor_intermediate_filename(sourceFile->get_file_info()->get_filename())) : 
                                            currentFileInfo->get_physical_file_id(source_file_id);
                  }

               printf ("evaluateInheritedAttribute: isCompilerGenerated = %s isTransformation = %s fileIdForOriginOfCurrentLocatedNode = %d \n",
                    isCompilerGenerated ? "true" : "false",isTransformation ? "true" : "false",fileIdForOriginOfCurrentLocatedNode);
#endif
#if 0
               printf ("evaluateInheritedAttribute: isCompilerGenerated = %s isTransformation = %s \n",
                    isCompilerGenerated ? "true" : "false",isTransformation ? "true" : "false");
#endif
               int currentLocNode_physical_file_id = currentLocNodePtr->get_file_info()->get_physical_file_id();
               string currentLocNode_physical_filename_from_id = Sg_File_Info::getFilenameFromID(currentLocNode_physical_file_id);
#if 0
               printf ("currentLocNode_physical_file_id = %d \n",currentLocNode_physical_file_id);
               printf ("currentLocNode_physical_filename_from_id = %s \n",currentLocNode_physical_filename_from_id.c_str());
               printf (" --- currentFileNameId                                                    = %d \n",currentFileNameId);
               printf (" --- source_file_id                                                       = %d \n",source_file_id);
#endif
#if 0
            // DQ (4/21/2020): debugging the unparse header files tests: test8.
               if (currentFileNameId == 1 && source_file_id == 1)
                  {
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
                  }
#endif
#if 0
               printf ("Attaching to node from currentLocNodePtr->get_file_info()->get_filename() = %s \n",currentLocNodePtr->get_file_info()->get_filename());
               printf (" --- sourceFile->get_requires_C_preprocessor()                            = %s \n",sourceFile->get_requires_C_preprocessor() ? "true" : "false");
               printf (" --- currentListOfAttributes->getFileName()                               = %s \n",currentListOfAttributes->getFileName().c_str());
               printf (" --- currentFileNameId                                                    = %d \n",currentFileNameId);
               printf (" --- source_file_id                                                       = %d \n",source_file_id);
               printf (" --- currentLocNodePtr->get_file_info()->get_file_id()                    = %d \n",currentLocNodePtr->get_file_info()->get_file_id());
               printf (" --- currentLocNodePtr->get_file_info()->get_physical_file_id()           = %d \n",currentLocNodePtr->get_file_info()->get_physical_file_id());
#endif
  
            // Pei-Hung (2/25/2020): If CPP is required, then we should use currentFileNameId here to use the preprocessed
            // input file.  Otherwise, all the preprocessed information is not attached to AST.  Comments and directives
            // will not be unparsed.
            // DQ (11/3/2019): I think we want the source_file_id below, since they used to be that currentFileNameId 
            // and source_file_id had the same value, but this didn't allow us to support the header file unparsing.
            // Or perhaps it didn't allow the support of the optimization of the header file unparsing.
            // DQ (5/24/2005): Relaxed to handle compiler generated and transformed IR nodes
            // if ( isCompilerGenerated || isTransformation || currentFileNameId == fileIdForOriginOfCurrentLocatedNode )
            // if ( isCompilerGenerated || isTransformation || source_file_id == fileIdForOriginOfCurrentLocatedNode )
            // if ( source_file_id == fileIdForOriginOfCurrentLocatedNode )
#if 0
               printf ("sourceFile->get_requires_C_preprocessor() = %s \n",sourceFile->get_requires_C_preprocessor() ? "true" : "false");
               printf ("currentLocNode_physical_file_id = %d \n",currentLocNode_physical_file_id);
               printf ("currentFileNameId               = %d \n",currentFileNameId);
               printf ("source_file_id                  = %d \n",source_file_id);

               if ( ((sourceFile->get_requires_C_preprocessor() == true) ? currentFileNameId : source_file_id) == currentLocNode_physical_file_id )
                   {
                     printf ("TRUE: ((sourceFile->get_requires_C_preprocessor() == true) ? currentFileNameId : source_file_id) == currentLocNode_physical_file_id \n");
                   }
                  else
                   {
                     printf ("FALSE: ((sourceFile->get_requires_C_preprocessor() == true) ? currentFileNameId : source_file_id) == currentLocNode_physical_file_id \n");
                   }
#endif
            // DQ (4/16/2020): This is the cause of a redundant inclusion of a CPP directive and comment in test8. Basically,
            // the issue is that the evaluation of the inherited attribute is causing it to be attached and the evaluation 
            // of the synthesized attribute is also causing it to be attached. If this is a fix then I need to work with Pei-Hung.
            // Or the issue is that the Preprocessor list iterator is not being properly increments, and so this is why both 
            // attribute evaluation functions are adding the include directive in test8.
            // if ( ((sourceFile->get_requires_C_preprocessor() == true) ? currentFileNameId : source_file_id) == currentLocNode_physical_file_id )
            // if ( source_file_id == fileIdForOriginOfCurrentLocatedNode )
               if ( ((sourceFile->get_requires_C_preprocessor() == true) ? currentFileNameId : source_file_id) == currentLocNode_physical_file_id )
                  {
                 // DQ (11/3/2019): Check that the comment or CPP directive is from the same file as the locatedNode.
                 // A variation of this test might be required later, though we should only be attacheing comments and 
                 // CPP directives before possible transformations.
                 // if (currentLocNodePtr->get_file_info()->get_filename() != currentListOfAttributes->getFileName())

                 // int currentLocNode_physical_file_id = currentLocNodePtr->get_file_info()->get_physical_file_id();
                 // string currentLocNode_physical_filename_from_id = Sg_File_Info::getFilenameFromID(currentLocNode_physical_file_id);
#if 0
                    printf ("currentLocNode_physical_file_id          = %d \n",currentLocNode_physical_file_id);
                    printf ("currentLocNode_physical_filename_from_id = %s \n",currentLocNode_physical_filename_from_id.c_str());
#endif
#if 0
                    if (currentLocNodePtr->get_file_info()->get_physical_filename() != currentListOfAttributes->getFileName())
                       {
                         printf ("Error: currentLocNodePtr->get_file_info()->get_filename() != currentListOfAttributes->getFileName() \n");
                         printf (" --- currentLocNodePtr->get_file_info()->get_filename()          = %s \n",currentLocNodePtr->get_file_info()->get_filename());
                         printf (" --- currentLocNodePtr->get_file_info()->get_physical_filename() = %s \n",currentLocNodePtr->get_file_info()->get_physical_filename().c_str());
                         printf (" --- currentListOfAttributes->getFileName()                      = %s \n",currentListOfAttributes->getFileName().c_str());
                       }
#endif
                 // ROSE_ASSERT(currentLocNodePtr->get_file_info()->get_filename() == currentListOfAttributes->getFileName());
                 // ROSE_ASSERT(currentLocNodePtr->get_file_info()->get_physical_filename() == currentListOfAttributes->getFileName());
#if 0
                    printf ("isCompilerGenerated = %s \n",isCompilerGenerated ? "true" : "false");
                    printf ("isTransformation    = %s \n",isTransformation ? "true" : "false");
#endif
                 // DQ (11/4/2019): if we allow isCompilerGenerated || isTransformation above, then we need to uncomment this if statement.
                    if (!isCompilerGenerated && !isTransformation)
                       {
                      // DQ (12/4/2019): This fails for Fortran code so skip the test when using Fortran.
                      // ROSE_ASSERT(currentLocNode_physical_filename_from_id == currentListOfAttributes->getFileName());
                         if (sourceFile->get_Fortran_only() == false)
                            {
                              ROSE_ASSERT(currentLocNode_physical_filename_from_id == currentListOfAttributes->getFileName());
                            }
                           else
                            {
#if 0
                              printf ("Fortran case: currentLocNode_physical_filename_from_id != currentListOfAttributes->getFileName() \n");
                              printf (" --- currentLocNode_physical_filename_from_id = %s \n",currentLocNode_physical_filename_from_id.c_str());
                              printf (" --- currentListOfAttributes->getFileName() = %s \n",currentListOfAttributes->getFileName().c_str());
#endif
                            }
                       }

                 // DQ (2/28/2019): We need to return the line that is associated with the source file where this can be a node shared between multiple ASTs.
                 // Current node belongs to the file the name of which has been specified on the command line
                 // line = currentLocNodePtr->get_file_info()->get_line();
                 // line = currentLocNodePtr->get_file_info()->get_physical_line();
                    line = currentLocNodePtr->get_file_info()->get_physical_line(source_file_id);

                 // DQ (12/9/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
                    col  = currentLocNodePtr->get_file_info()->get_col();
#if 0
                    printf ("Insert any comment before %p = %s = %s (compilerGenerate=%s) at line = %d col = %d \n",
                          currentLocNodePtr,currentLocNodePtr->class_name().c_str(),SageInterface::get_name(currentLocNodePtr).c_str(),
                          isCompilerGenerated ? "true" : "false", line, col);
#endif
#if 0
                 // DQ (8/21/2018): Modified to exclude the column information (no longer used).
                    printf ("Insert any comment before %p = %s = %s (compilerGenerate=%s) at line = %d \n",
                          currentLocNodePtr,currentLocNodePtr->class_name().c_str(),SageInterface::get_name(currentLocNodePtr).c_str(),
                          isCompilerGenerated ? "true" : "false", line);
#endif

#if 0
                    printf ("In AttachPreprocessingInfoTreeTrav::evaluateInheritedAttribute() calling iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber(): n->class_name() = %s \n",
                         n->class_name().c_str());
#endif

                 // DQ (8/19/2019): This is not the best place to isolate the two phases of processing the source file from the processin og the headers.
                 // DQ (8/19/2019): If we want to defer the insertion of CPP directives from header files into the AST then
                 // we need to be able to call this function later. But since this is a recursive function maybe we could 
                 // just call the whole process to insert comments and CPP directives into ROSE later, however, that would 
                 // mark the nodes as transformed.

                 // Or maybe we could do it once where the comments and CPP directives are inserted into the main source file, 
                 // and then later when they are inserted into all of the header files.
#if 0
                    printf ("Saving information about where to input comments and CPP directives from header files \n");
                    printf (" --- currentLocNodePtr = %p = %s \n",currentLocNodePtr,currentLocNodePtr != NULL ? currentLocNodePtr->class_name().c_str() : "null");
                    printf (" --- line = %d \n",line);
                    printf (" --- currentListOfAttributes = %p \n",currentListOfAttributes);
#endif
                 // Iterate over the list of comments and directives and add them to the AST
                    bool reset_start_index = false;
                    iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber(
                          currentLocNodePtr,line,PreprocessingInfo::before, reset_start_index, currentListOfAttributes );
#if 0
                    printf ("Before calling setupPointerToPreviousNode() in AttachPreprocessingInfoTreeTrav::evaluateInheritedAttribute(): n = %p = %s \n",n,n->class_name().c_str());
                    printf (" --- previousLocatedNode = %p = %s \n",previousLocatedNode,(previousLocatedNode != NULL) ? previousLocatedNode->class_name().c_str() : "null");
#endif
                 // save the previous node (in an accumulator attribute), but handle some nodes differently
                 // to avoid having comments attached to them since they are not unparsed directly.
                 // printf ("currentLocNodePtr = %p = %s \n",currentLocNodePtr,currentLocNodePtr->class_name().c_str());
                 // setupPointerToPreviousNode(currentLocNodePtr);
#if 1
#if 0
                    printf ("After calling setupPointerToPreviousNode() in AttachPreprocessingInfoTreeTrav::evaluateInheritedAttribute(): n = %p = %s \n",n,n->class_name().c_str());
                    printf (" --- previousLocatedNode = %p = %s \n",previousLocatedNode,(previousLocatedNode != NULL) ? previousLocatedNode->class_name().c_str() : "null");
                    printf (" --- currentLocNodePtr = %p = %s \n",currentLocNodePtr,currentLocNodePtr->class_name().c_str());
#endif
                 // DQ (6/26/2020): Avoid setting the previousLocatedNode to a SgInitializedName in a variable declaration.
                    SgInitializedName* initializedName = isSgInitializedName(currentLocNodePtr);
                    if (initializedName != NULL)
                       {
#if 0
                         printf ("Ignore update of previousLocatedNode to initializedName = %p = %s \n",initializedName,initializedName->get_name().str());
#endif
                       }
                      else
                       {
                      // DQ (6/17/2020): Set the previousLocatedNode
                         previousLocatedNode = currentLocNodePtr;
                       }
#if 0
                    printf ("After resetting previousLocatedNode in AttachPreprocessingInfoTreeTrav::evaluateInheritedAttribute(): n = %p = %s \n",n,n->class_name().c_str());
                    printf (" --- previousLocatedNode = %p = %s \n",previousLocatedNode,(previousLocatedNode != NULL) ? previousLocatedNode->class_name().c_str() : "null");
                    printf (" --- currentLocNodePtr = %p = %s \n",currentLocNodePtr,currentLocNodePtr->class_name().c_str());
#endif
#endif

#if 0
                 // DQ (8/21/2018): As a result of adapting the physical file name to work with unparse headers, we have 
                 // a bug when the SgFunctionParameterList is selected to be the target of an inserted CPP directive.
                    if (isSgFunctionParameterList(n) != NULL)
                       {
                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
                       }
#endif
                  }
#if 0
             // Debugging output
                 else
                  {
                    printf ("Node belongs to a different file: \n");
                  }
#endif
             } // end if current list of attribute is not empty

#if 0
       // DQ (11/4/2019): Exit after where we find an error.
          if (isSgForStatement(n) != NULL && currentListOfAttributes->getFileName() == "/home/quinlan1/ROSE/git_rose_development/tests/nonsmoke/functional/CompileTests/UnparseHeadersTests/test4/SimpleInternal.h")
             {
               printf ("Exiting upon finding a SgForStatement! \n");
               ROSE_ASSERT(false);
             }
#endif

        } // end if statement or init name

#if 0
     printf ("Leaving AttachPreprocessingInfoTreeTrav::evaluateInheritedAttribute(): n = %p = %s \n",n,n->class_name().c_str());
     printf (" --- previousLocatedNode = %p = %s \n",previousLocatedNode,(previousLocatedNode != NULL) ? previousLocatedNode->class_name().c_str() : "null");
#endif

     return inheritedAttribute;
   }


// Member function: evaluateSynthesizedAttribute
AttachPreprocessingInfoTreeTraversalSynthesizedAttribute
AttachPreprocessingInfoTreeTrav::evaluateSynthesizedAttribute(
    SgNode *n,
    AttachPreprocessingInfoTreeTraversalInheritedAttrribute inheritedAttribute,
    SubTreeSynthesizedAttributes synthiziedAttributeList)
   {
  // DQ (11/29/2008): FIXME: Note that this traversal does not use its inheritedAttribute 
  // or synthiziedAttributeList attributes, so it could be expressed as a much 
  // simpler visit traversal.  We might do that later, if we decide that we REALLY
  // don't require inheritedAttribute or synthiziedAttributeList attributes.

     AttachPreprocessingInfoTreeTraversalSynthesizedAttribute returnSynthesizeAttribute;

#if 0
     printf ("\nTOP of AttachPreprocessingInfoTreeTrav::evaluateSynthesizedAttribute(): n = %p = %s name = %s \n",n,n->class_name().c_str(),SageInterface::get_name(n).c_str());
     printf (" --- previousLocatedNode = %p = %s \n",previousLocatedNode,(previousLocatedNode != NULL) ? previousLocatedNode->class_name().c_str() : "null");
#endif
#if 0
     SgStatement* currentStatement = isSgStatement(n);
  // if (isSgStatement(n) && (isSgStatement(n)->get_parent() != NULL) )
     if (currentStatement != NULL && (currentStatement->get_parent() != NULL) )
        {
          printf ("     parent = %s \n",currentStatement->get_parent()->class_name().c_str());
          ROSE_ASSERT(currentStatement->get_file_info() != NULL);
          currentStatement->get_file_info()->display("In AttachPreprocessingInfoTreeTrav::evaluateSynthesizedAttribute(): debug");
        }
#endif

  // DQ (8/6/2012): Allow those associated with the declaration and not inside of the template declaration.
     ROSE_ASSERT(n != NULL);
  // printf ("In AttachPreprocessingInfoTreeTrav::evaluateSynthesizedAttribute(): n = %p = %s \n",n,n->class_name().c_str());
     SgDeclarationStatement* templateDeclaration = isSgTemplateFunctionDeclaration(n);

     if (templateDeclaration == NULL) templateDeclaration = isSgTemplateMemberFunctionDeclaration(n);
     if (templateDeclaration == NULL) templateDeclaration = isSgTemplateClassDeclaration(n);
     if (templateDeclaration == NULL) templateDeclaration = isSgTemplateVariableDeclaration(n);

     SgDeclarationStatement* templateInstantiationDeclaration = isSgTemplateInstantiationFunctionDecl(n);
     if (templateInstantiationDeclaration == NULL) templateInstantiationDeclaration = isSgTemplateInstantiationMemberFunctionDecl(n);
     if (templateInstantiationDeclaration == NULL) templateInstantiationDeclaration = isSgTemplateInstantiationDecl(n);
  // if (templateInstantiationDeclaration == NULL) templateInstantiationDeclaration = isSgTemplateInstantiationVariableDecl(n);

  // DQ (7/1/2014): Modify to avoid use of CPP directives in both template declarations and template instantiations (which might not be unparsed).
  // if (inheritedAttribute.isPartOfTemplateDeclaration == true )
  // if (inheritedAttribute.isPartOfTemplateDeclaration == true && templateDeclaration == NULL)
     if ( (inheritedAttribute.isPartOfTemplateDeclaration              == true && templateDeclaration              == NULL) || 
          (inheritedAttribute.isPartOfTemplateInstantiationDeclaration == true && templateInstantiationDeclaration == NULL) )
        {
// #if DEBUG_ATTACH_PREPROCESSING_INFO
#if 0
          printf ("Returning without further processing if we are a part of a template declaration n = %p = %s \n",n,n->class_name().c_str());
#endif
          return returnSynthesizeAttribute;
        }

  // DQ (3/4/2016): Klocworks reports a problem with "isSgClassDeclaration(n)->get_endOfConstruct() != NULL".
  // These used to be a problem, so we can continue to test these specific cases.
  // ROSE_ASSERT (isSgCaseOptionStmt(n)   == NULL || isSgCaseOptionStmt(n)->get_body()             != NULL);
#if 0
     SgCaseOptionStmt* caseOptionStm = isSgCaseOptionStmt(n);

  // DQ (1/25/2019): A case option without a body is allowed, and comments should be allowed here..
  // DQ (11/25/2017): Added debugging info to support new switch implementation.
     if (caseOptionStm != NULL && caseOptionStm->get_body() == NULL)
        {
          printf ("Error: In AttachPreprocessingInfoTreeTrav::evaluateSynthesizedAttribute(): caseOptionStm = %p \n",caseOptionStm);
          caseOptionStm->get_file_info()->display("In AttachPreprocessingInfoTreeTrav::evaluateSynthesizedAttribute(): caseOptionStm: debug");
        }
  // DQ (1/25/2019): A case option without a body is allowed.
  // ROSE_ASSERT (caseOptionStm == NULL || caseOptionStm->get_body() != NULL);
#endif

  // DQ (3/4/2016): Klocworks reports a problem with "isSgClassDeclaration(n)->get_endOfConstruct() != NULL".
  // ROSE_ASSERT (isSgClassDeclaration(n) == NULL || isSgClassDeclaration(n)->get_endOfConstruct() != NULL);
  // ROSE_ASSERT (isSgClassDeclaration(n) == NULL || (isSgClassDeclaration(n) != NULL && isSgClassDeclaration(n)->get_endOfConstruct() != NULL) );
     SgClassDeclaration* classDeclaration = isSgClassDeclaration(n);
     ROSE_ASSERT (classDeclaration == NULL || classDeclaration->get_endOfConstruct() != NULL);

  // Only process SgLocatedNode object and the SgFile object
  // SgFile* fileNode           = dynamic_cast<SgFile*>(n);
  // SgLocatedNode* locatedNode = dynamic_cast<SgLocatedNode*>(n);
     SgFile* fileNode           = isSgFile(n);


#if 0
  // DQ (11/3/2019): Force processing to exist afer a selected header file is processed.
     if (fileNode != NULL)
        {
          SgSourceFile* sourceFile = isSgSourceFile(fileNode);
          bool isHeaderFile = sourceFile->get_isHeaderFile();
#if 0
          printf ("isHeaderFile = %s \n",isHeaderFile ? "true" : "false");
#endif
          if (isHeaderFile == true)
             {
               SgIncludeFile* includeFile = sourceFile->get_associated_include_file();
               ROSE_ASSERT(includeFile != NULL);
               ROSE_ASSERT(includeFile->get_source_file() == sourceFile);

            // if (includeFile->get_source_file()->getFileName() == "/home/quinlan1/ROSE/git_rose_development/tests/nonsmoke/functional/CompileTests/UnparseHeadersTests/test4/SimpleInternal.h")
            // if (includeFile->get_source_file()->getFileName() == "/home/quinlan1/ROSE/git_rose_development/tests/nonsmoke/functional/CompileTests/UnparseHeadersTests/test4/InnerInternal1.h")
               if (includeFile->get_source_file()->getFileName() == "/home/quinlan1/ROSE/git_rose_development/tests/nonsmoke/functional/CompileTests/UnparseHeadersTests/test4/SimpleInternal.h")
                  {
                    printf ("Exiting as a test in evaluateSynthesizedAttribute() in the include file \n");
                    ROSE_ASSERT(false);
                  }
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }
        }
#endif


     SgLocatedNode* locatedNode = isSgLocatedNode(n);

#if 0
  // DQ (6/10/2020): This might be an issue now.
     if (fileNode != NULL)
        {
          printf ("We might miss comments and CPP directives at the end of the file! \n");
        }
#endif

  // DQ (6/10/2020): We only care is the locatedNode is non-null now.
  // if ( (locatedNode != NULL) || (fileNode != NULL) )
  // if (locatedNode != NULL)
     if ( (locatedNode != NULL) || (fileNode != NULL) )
        {
       // Attach the comments only to nodes from the same file
       // int fileNameId = currentFileNameId;
       // ROSE_ASSERT(locatedNode->get_file_info() != NULL);
          int currentFileNameId = -9;
          if (locatedNode != NULL)
             {
               ROSE_ASSERT(locatedNode->get_file_info() != NULL);
#if 0
               printf ("locatedNode->get_file_info() = %p \n",locatedNode->get_file_info());
               locatedNode->get_file_info()->display("In AttachPreprocessingInfoTreeTrav::evaluateSynthesizedAttribute(): debug");
#endif
            // currentFileNameId = locatedNode->get_file_info()->get_file_id();
               currentFileNameId = locatedNode->get_file_info()->get_physical_file_id(source_file_id);
             }
            else
             {
            // ROSE_ASSERT(fileNode->get_file_info() != NULL);
            // currentFileNameId = fileNode->get_file_info()->get_file_id();
               Sg_File_Info* currentFileInfo = sourceFile->get_file_info();
               ROSE_ASSERT(currentFileInfo != NULL);
#if 0
               currentFileNameId = (sourceFile->get_requires_C_preprocessor() == true) ? 
                                   Sg_File_Info::getIDFromFilename(sourceFile->generate_C_preprocessor_intermediate_filename(sourceFile->get_file_info()->get_filename())) : 
                                   currentFileInfo->get_file_id();
#else
            // Newer version of code using the physical source code position.
               currentFileNameId = (sourceFile->get_requires_C_preprocessor() == true) ? 
                                   Sg_File_Info::getIDFromFilename(sourceFile->generate_C_preprocessor_intermediate_filename(sourceFile->get_file_info()->get_filename())) : 
                                   currentFileInfo->get_physical_file_id(source_file_id);
#endif
             }

#if 0
          printf ("##### currentFileNameId = %d target_source_file_id = %d \n",currentFileNameId,target_source_file_id);
#endif
       // DQ (6/25/2020): If this is not a node associated with the collect comments and CPP directives for the associated file then ignore this IR node.
          if (currentFileNameId != target_source_file_id)
             {
#if 0
               printf ("This IR node does not match the file where the comments and CPP dirtectives were collected: \n");
               printf (" --- currentFileNameId = %d \n",currentFileNameId);
               printf (" --- target_source_file_id = %d \n",target_source_file_id);
#endif
               return returnSynthesizeAttribute;
             }

#if 0
       // DQ (12/21/2012): This is failing...
       // ROSE_ASSERT(locatedNode != NULL);
          if (locatedNode != NULL)
             {
               printf ("In AttachPreprocessingInfoTreeTrav::evaluateSynthesizedAttribute(): currentFileNameId = %d = %s locatedNode->get_file_info()->get_physical_file_id() = %d = %s physical_line = %d \n",
                  currentFileNameId,
                  Sg_File_Info::getFilenameFromID(currentFileNameId).c_str(),
                  locatedNode->get_file_info()->get_physical_file_id(),
                  Sg_File_Info::getFilenameFromID(locatedNode->get_file_info()->get_physical_file_id()).c_str(),locatedNode->get_file_info()->get_physical_line());
             }
            else
             {
               printf ("In AttachPreprocessingInfoTreeTrav::evaluateSynthesizedAttribute(): locatedNode == NULL \n");
             }
#endif
#if 0
          printf ("In AttachPreprocessingInfoTreeTrav::evaluateSynthesizedAttribute(): currentFileNameId = %d = %s \n",currentFileNameId,Sg_File_Info::getFilenameFromID(currentFileNameId).c_str());
#endif
       // DQ (12/20/2012): Adding support for physical source position.
          if (locatedNode != NULL)
             {
               ROSE_ASSERT(locatedNode->get_file_info()->get_physical_file_id(source_file_id) == currentFileNameId);
             }

#if 0
          printf ("In AttachPreprocessingInfoTreeTrav::evaluateSynthesizedAttribute(): n = %p = %s currentFileNameId = %d \n",n,n->class_name().c_str(),currentFileNameId);
#endif
       // DQ (10/27/2007): This is a valgrind error: use of uninitialized variable below!
       // Initialized with a value that could not match a valid file_id.
          int fileIdForOriginOfCurrentLocatedNode = -99;

          bool isCompilerGeneratedOrTransformation = false;
          int lineOfClosingBrace = 0;
          if (locatedNode != NULL)
             {
               ROSE_ASSERT(locatedNode->get_file_info());
            // printf ("Calling locatedNode->get_file_info()->get_filename() \n");

            // DQ (6/20/2005): Compiler generated IR nodes to be output are now marked explicitly!
            // isCompilerGeneratedOrTransformation = locatedNode->get_file_info()->isCompilerGenerated() || 
            //                                       locatedNode->get_file_info()->isTransformation() ||
               isCompilerGeneratedOrTransformation = locatedNode->get_file_info()->isCompilerGeneratedNodeToBeUnparsed() ||
                                                     locatedNode->get_file_info()->isTransformation();

            // bool isCompilerGenerated = currentLocNodePtr->get_file_info()->isCompilerGeneratedNodeToBeUnparsed();
            // bool isTransformation    = currentLocNodePtr->get_file_info()->isTransformation();

            // DQ (6/20/2005): Notice that we use the new hasPositionInSource() member function
            // if ( isCompilerGeneratedOrTransformation == false )
               if ( locatedNode->get_file_info()->hasPositionInSource() == true )
                  {
                 // fileIdForOriginOfCurrentLocatedNode = locatedNode->get_file_info()->get_file_id();
                    fileIdForOriginOfCurrentLocatedNode = locatedNode->get_file_info()->get_physical_file_id(source_file_id);
                  }

               if (locatedNode->get_endOfConstruct() != NULL)
                  {
                    ROSE_ASSERT (locatedNode->get_endOfConstruct() != NULL);
                 // lineOfClosingBrace = locatedNode->get_endOfConstruct()->get_line();
                    lineOfClosingBrace = locatedNode->get_endOfConstruct()->get_physical_line(source_file_id);
                  }
             }
            else
             {
            // handle the trivial case of a SgFile node being from it's own file

            // fileIdForOriginOfCurrentLocatedNode = currentFileNameId;
            // fileIdForOriginOfCurrentLocatedNode = sourceFile->get_file_info()->get_file_id();

               Sg_File_Info* currentFileInfo = sourceFile->get_file_info();
               ROSE_ASSERT(currentFileInfo != NULL);
#if 0
               fileIdForOriginOfCurrentLocatedNode = (sourceFile->get_requires_C_preprocessor() == true) ? 
                                   Sg_File_Info::getIDFromFilename(sourceFile->generate_C_preprocessor_intermediate_filename(sourceFile->get_file_info()->get_filename())) : 
                                   currentFileInfo->get_file_id();
#else
               fileIdForOriginOfCurrentLocatedNode = (sourceFile->get_requires_C_preprocessor() == true) ? 
                                   Sg_File_Info::getIDFromFilename(sourceFile->generate_C_preprocessor_intermediate_filename(sourceFile->get_file_info()->get_filename())) : 
                                   currentFileInfo->get_physical_file_id(source_file_id);
#endif

            // Use one billion as the max number of lines in a file
               const int OneBillion = 1000000000;

#if 0
            // DQ (5/28/2019): debugging test2019_441.C.
               printf ("Using OneBillion figure for lineOfClosingBrace \n");
#endif
               lineOfClosingBrace = OneBillion;
             }

#if 0
          printf ("isCompilerGeneratedOrTransformation   = %s \n",isCompilerGeneratedOrTransformation ? "true" : "false");
          printf ("currentFileNameId = %d fileIdForOriginOfCurrentLocatedNode = %d \n",currentFileNameId,fileIdForOriginOfCurrentLocatedNode);
          printf ("currentFileName for currentFileNameId = %s \n",Sg_File_Info::getFilenameFromID(currentFileNameId).c_str());
#endif
       // Make sure the astNode matches the current file's list of comments and CPP directives.
       // DQ (5/24/2005): Handle cases of isCompilerGenerated or isTransformation
          if ( (isCompilerGeneratedOrTransformation == true) || (currentFileNameId == fileIdForOriginOfCurrentLocatedNode) )
             {
#if 0
               printf ("In AttachPreprocessingInfoTreeTrav::evaluateSynthesizedAttribute(): %p = %s isCompilerGeneratedOrTransformation = %s lineOfClosingBrace = %d \n",
                    n,n->class_name().c_str(),isCompilerGeneratedOrTransformation ? "true" : "false",lineOfClosingBrace);
#endif

#if 0
            // Debugging code
               if (attributeMapForAllFiles.find(currentFileNameId) == attributeMapForAllFiles.end())
                  {
                    Sg_File_Info::display_static_data("debugging in AttachPreprocessingInfoTreeTrav");

#if 0
                 // This outputs too much data to be useful.
                 // output internal data in maps...
                 // display_static_data("debugging in AttachPreprocessingInfoTreeTrav");
                    display("debugging in AttachPreprocessingInfoTreeTrav");
#endif
                  }

               printf ("currentFileName for currentFileNameId = %d = %s \n",currentFileNameId,Sg_File_Info::getFilenameFromID(currentFileNameId).c_str());
#endif

            // DQ (9/22/2013): This fails for the projects/haskellport tests (does not appear to be related to the move to physical source position information, but I can't be certain).
            // ROSE_ASSERT(processAllIncludeFiles == false || ((currentFileNameId < 0) || (attributeMapForAllFiles.find(currentFileNameId) != attributeMapForAllFiles.end())));

#if 0
               printf ("In AttachPreprocessingInfoTreeTrav::evaluateSynthesizedAttribute(): calling getListOfAttributes(): currentFileName for currentFileNameId = %d = %s \n",
                    currentFileNameId,Sg_File_Info::getFilenameFromID(currentFileNameId).c_str());
#endif
            // ROSEAttributesList* currentListOfAttributes = attributeMapForAllFiles[currentFileNameId];
               ROSEAttributesList* currentListOfAttributes = getListOfAttributes(currentFileNameId);
#if 0
               printf ("In AttachPreprocessingInfoTreeTrav::evaluateSynthesizedAttribute(): currentListOfAttributes = %p \n",currentListOfAttributes);
#endif
#if 0
               printf ("In AttachPreprocessingInfoTreeTrav::evaluateSynthesizedAttribute(): This ROSEAttributesList should also be saved into the SgFile ROSEAttributesListContainer \n");
#endif
            // ROSE_ASSERT(currentListOfAttributes != NULL);
               if (currentListOfAttributes == NULL)
                  {
                 // This case is used to handle the case of the currentFileNameId being negative (not a real file).
#if 1
                    printf ("Not supporting gathering of CPP directives and comments for this file currentFileNameId = %d \n",currentFileNameId);
#endif
                    return returnSynthesizeAttribute;
                  }

            // DQ (6/10/2020): Set the previousLocNodePtr to the locatedNode.
            // SgLocatedNode* previousLocNodePtr = previousLocatedNode;
            // SgLocatedNode* previousLocNodePtr = locatedNode;
#if 0
               printf ("previousLocatedNode = %p \n",previousLocatedNode);
               if (previousLocatedNode != NULL)
                  {
                    printf ("previousLocatedNode = %p = %s \n",previousLocatedNode,previousLocatedNode->class_name().c_str());
                  }
#endif
            // DQ (6/11/2020): We want to use previousLocatedNode, but it seems to sometimes be NULL (need to isolate this case).
               SgLocatedNode* previousLocNodePtr = previousLocatedNode;
               if (previousLocNodePtr == NULL)
                  {
                    previousLocNodePtr = locatedNode;

                 // DQ (6/12/2020): Debug where this is still NULL.
                 // ROSE_ASSERT(previousLocNodePtr != NULL);
                    if (previousLocNodePtr == NULL)
                       {
                         ROSE_ASSERT(n != NULL);
                         printf ("############### Note: previousLocNodePtr == NULL; locatedNode == NULL; n = %p = %s \n",n,n->class_name().c_str());
                       }
                    if (isSgSourceFile(n) == NULL)
                       {
                         ROSE_ASSERT(previousLocNodePtr != NULL);
                         printf ("Note: previousLocNodePtr == NULL; reset to locatedNode = %p = %s \n",previousLocNodePtr,previousLocNodePtr->class_name().c_str());
                       }
                  }

            // ROSE_ASSERT(previousLocNodePtr != NULL);
               if (isSgSourceFile(n) == NULL)
                  {
                    ROSE_ASSERT(previousLocNodePtr != NULL);
                  }

#if 0
            // DQ (6/11/2020): This appear to be NULL in some cases I am debugging currently.
            // ROSE_ASSERT(previousLocNodePtr != NULL);
               if (previousLocatedNode == NULL)
                  {
                    printf ("Before switch: previousLocatedNode == NULL \n");
                  }
                 else
                  {
                    printf ("Before switch: previousLocatedNode = %p = %s \n",previousLocatedNode,previousLocatedNode->class_name().c_str());
                  }


            // DQ (6/9/2020): This appear to be NULL in some cases I am debugging currently.
            // ROSE_ASSERT(previousLocNodePtr != NULL);
               if (previousLocNodePtr == NULL)
                  {
                    printf ("Before switch: previousLocNodePtr == NULL \n");
                  }
                 else
                  {
                    printf ("Before switch: previousLocNodePtr = %p = %s \n",previousLocNodePtr,previousLocNodePtr->class_name().c_str());
                  }
#endif

#if 0
            // DQ (6/25/2020): Debugging support.
               printf ("################## In evaluateSynthesizedAttribute(): Before switch: n = %p = %s \n",n,n->class_name().c_str());
               SgLocatedNode* tmp_locatedNode = isSgLocatedNode(n);
               printf ("tmp_locatedNode = %p \n",tmp_locatedNode);
               if (tmp_locatedNode != NULL)
                  {
                    ROSE_ASSERT(tmp_locatedNode->get_file_info() != NULL);
                    printf ("output source position information: \n");
                    printf ("tmp_locatedNode->get_file_info() = %p = %s \n",tmp_locatedNode->get_file_info(),tmp_locatedNode->get_file_info()->class_name().c_str());
                    tmp_locatedNode->get_file_info()->display("tmp_locatedNode before switch");
                    printf ("target_source_file_id = %d \n",target_source_file_id);
                    printf ("currentFileNameId = %d \n",currentFileNameId);
                  }
                 else
                  {
                    printf (" --- tmp_locatedNode == NULL \n");
                  }
               printf ("Processing switch \n");
#endif
               switch (n->variantT())
                  {
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
                 // SgBinaryComposite need not be in the switch since we don't attach CPP directives or comments to it.
                    case V_SgBinaryComposite:
                        {
                          printf ("Error: SgBinaryComposite need not be in the switch since we don't attach CPP directives or comments to it ... \n");
                          ROSE_ASSERT(false);
                          break;
                        }
#endif

                 // I wanted to leave the SgFile case in the switch statement rather 
                 // than separating it out in a conditional statement at the top of the file.
                 // case V_SgFile:
                    case V_SgSourceFile:
                       {
#if 0
                         printf ("Processing case: V_SgSourceFile \n");
#endif
                         ROSE_ASSERT(previousLocatedNode != NULL);
#if 0
                            printf ("Case SgFile: See if we can find a better target to attach these comments than %s \n",previousLocNodePtr->sage_class_name());
#endif
#if 0
                         // DQ (5/1/2020): Disabled data member for attributeMapForAllFiles.
                         // SgLocatedNode* targetNode = previousLocNodePtr;
                            ROSE_ASSERT(previousLocatedNodeMap.find(currentFileNameId) != previousLocatedNodeMap.end());
                            SgLocatedNode* targetNode = previousLocatedNodeMap[currentFileNameId];
#else
                            SgLocatedNode* targetNode = previousLocatedNode;
#endif
                            ROSE_ASSERT(targetNode != NULL);
                         // printf ("In SgFile: previousLocNodePtr = %s \n",previousLocNodePtr->sage_class_name());
                         // printf ("In SgSourceFile: initial value of targetNode = %p = %s \n",targetNode,targetNode->class_name().c_str());

                         // If the target is a SgBasicBlock then try to find its parent in the global scope
                         // if (isSgBasicBlock(previousLocNodePtr) != NULL)
                            if (isSgBasicBlock(targetNode) != NULL)
                               {
                                 while ( (targetNode != NULL) && (isSgGlobal(targetNode->get_parent()) == NULL) )
                                    {
                                      targetNode = dynamic_cast<SgLocatedNode*>(targetNode->get_parent());
                                   // printf ("loop: targetNode = %s \n",targetNode->sage_class_name());
                                    }
                               }

                            ROSE_ASSERT(targetNode->get_file_info() != NULL);
                         // This case appears for test2008_08.f90: the SgProgramHeaderStatement is not present in the source code
                         // so we can't attach a comment to it.
                         // if (targetNode->get_file_info()->get_file_id() < 0)
                            if (targetNode->get_file_info()->get_physical_file_id(source_file_id) < 0)
                               {
#if 0
                                 printf ("Error: we should not be calling iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber() using targetNode->get_file_info()->get_file_id()          = %d \n",targetNode->get_file_info()->get_file_id());
#endif
#if 0
                                 printf ("Error: we should not be calling iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber() using targetNode->get_file_info()->get_physical_file_id() = %d \n",targetNode->get_file_info()->get_physical_file_id(source_file_id));
                                 printf ("In SgFile: targetNode = %s \n",targetNode->class_name().c_str());
                                 printf ("currentFileName for currentFileNameId = %d = %s \n",currentFileNameId,Sg_File_Info::getFilenameFromID(currentFileNameId).c_str());
                                 printf ("sourceFile = %s \n",sourceFile->get_sourceFileNameWithPath().c_str());
#endif
                              // DQ (9/12/2010): This is something caught in compiling the Fortran LLNL_POP code file: prognostic.F90
                              // ROSE_ASSERT(false);
                              // printf ("Skipping abort in processing a Fortran LLNL_POP code file: prognostic.F90 (unclear how to handle this error, if it is an error) \n");

                              // DQ (9/25/2013): FIXME: I don't like this design using a break statement at this specific location (in the middle of the case implementation) in this case.
                                 break;

                              // return returnSynthesizeAttribute;
                               }

#if 0
                             printf ("In AttachPreprocessingInfoTreeTrav::evaluateSynthisizedAttribute() calling iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber(): n->class_name() = %s \n",
                                  n->class_name().c_str());
#endif
                         // Iterate over the list of comments and directives and add them to the AST
                         // negara1 (07/28/2011): Changed to false, since we might need to re-visit some header files.
                            bool reset_start_index = false;
                            iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                               ( targetNode, lineOfClosingBrace, PreprocessingInfo::after, reset_start_index, currentListOfAttributes );

#if 0
                            printf ("DONE calling iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber(): n->class_name() = %s \n",n->class_name().c_str());
#endif
#if 0
                         // DQ (4/20/2020): Testing for unparse headers support: test8.
                            if (source_file_id == 1)
                               {
                                 printf ("Exiting as a test after calling iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber() from evaluateSynthisizedAttribute(): case SgFile \n");
                                 ROSE_ASSERT(false);
                               }
#endif
                         // DQ (12/19/2008): Output debugging information (needs to be output before we reset the attributeMapForAllFiles map entries
                            if ( SgProject::get_verbose() >= 3 )
                               {
                                 bool processAllFiles = sourceFile->get_collectAllCommentsAndDirectives();
                                 if (processAllFiles == true)
                                      display("Output from collecting ALL comments and CPP directives (across source and header files)");
                                   else
                                      display("Output from collecting comments and CPP directives in source file only");
                               }

#if 0
                         // DQ (1/22/2008): This IS a problem for the AST file I/O...which tries to write out the ROSEAttributesList of PreprocessingInfo objects.
                         // This is likely because the elements of that list are shared and were already processed at an earlier step in the AST File I/O.
                         // This is not a problem for the list of PreprocessingInfo since at this point they have already been added to the AST and are no longer
                         // required.  If we want to keep the actual token stream then that will have to be addressed.  We should also consider translating the
                         // raw token stream (using the lex data structures) to use the SgToken data structure so that it could be saved with the AST.  All of 
                         // this is later work however...

                         // DQ (1/21/2008): Save the details of the token information for this file (even though at this point we are mostly done with it)
                            SgFile* file = isSgFile(n);
                            string filename = file->get_sourceFileNameWithPath();
                            ROSE_ASSERT(file->get_preprocessorDirectivesAndCommentsList() != NULL);

                         // printf ("Adding secondary lex pass information (inheritedAttribute.currentListOfAttributes = %p) to file = %s \n",inheritedAttribute.currentListOfAttributes,filename.c_str());
                            file->get_preprocessorDirectivesAndCommentsList()->addList(filename,inheritedAttribute.currentListOfAttributes);
#else
                         // DQ (1/21/2008): Original code
                         // printf ("Delete Fortran Token List Size: currentListOfAttributes->get_rawTokenStream()->size() = %" PRIuPTR " \n",currentListOfAttributes->get_rawTokenStream()->size());
                         // delete inheritedAttribute.currentListOfAttributes;
                         // delete currentListOfAttributes;
#if 0
                         // DQ (5/1/2020): Disabled data member for attributeMapForAllFiles.
                            ROSE_ASSERT(attributeMapForAllFiles.find(currentFileNameId) != attributeMapForAllFiles.end());
#endif

#if 0
                            printf ("$$$$$$$$$$$$$$$$$$$$$$$$ Setting attributeMapForAllFiles entry to NULL: currentFileNameId = %d \n",currentFileNameId);
#endif
                         // For now just reset the pointer to NULL, but later we might want to delete the lists (to avoid a memory leak).
                         // delete attributeMapForAllFiles[currentFileNameId];
#if 0
                         // DQ (5/1/2020): Disabled data member for attributeMapForAllFiles.
                            attributeMapForAllFiles[currentFileNameId] = NULL;
#endif
                            currentListOfAttributes = NULL;

#if 0
                         // DQ (4/28/2020): It might be a problem that we are resetting this.
                            printf ("currentFileNameId = %d \n",currentFileNameId);
                            printf ("currentFileNameId = %zu \n",currentFileNameId);
#endif
#if 0
                            printf ("Reset the startIndexMap         [currentFileNameId = %zu] = %zu \n",currentFileNameId,startIndexMap[currentFileNameId]);
                            printf ("Reset the previousLocatedNodeMap[currentFileNameId = %zu] = %p \n",currentFileNameId,previousLocatedNodeMap[currentFileNameId]);
#endif
#if 0
                         // Reset the pointer to the previous located node and the current list size
                            previousLocatedNodeMap[currentFileNameId] = NULL;
                            startIndexMap[currentFileNameId] = 0;
#else
#if 0
                         // DQ (4/28/2020): Test this idea for unparsing header files and test8.
                            printf ("Skipping the reset of the previousLocatedNodeMap and the startIndexMap \n");
#endif
#endif
#if 0
                         // DQ (5/1/2020): Disabled data member for attributeMapForAllFiles.
                         // DQ (12/19/2008): I think this should be true, but check it!
                            ROSE_ASSERT(previousLocatedNodeMap.size() == startIndexMap.size());
#endif
            
#endif
#if 1
                            if (statementsToInsertBefore.size() > 0)
                               {
                                 printf ("In AttachPreprocessingInfoTreeTrav::evaluateSynthesizedAttribute(): case V_SgSourceFile: process statementsToInsertBefore (size = %zu) \n",
                                      statementsToInsertBefore.size());
                               }
#endif
                         // negara1 (08/12/2011): We reached the last AST node, so its safe to insert nodes for header files bodies.
                            for (list<pair<SgIncludeDirectiveStatement*, SgStatement*> >::const_iterator it = statementsToInsertBefore.begin(); it != statementsToInsertBefore.end(); it++) 
                               {
                                 ROSE_ASSERT(it->second != NULL);
#if 0
                                 printf ("Target it->second = %p = %s \n",it->second,it->second->class_name().c_str());
                                 it->second->get_file_info()->display("it->second: debug");
#endif
                                 ROSE_ASSERT(it->first != NULL);
#if 0
                                 printf ("Target it->first = %p = %s \n",it->first,it->first->class_name().c_str());
                                 it->first->get_file_info()->display("it->first: debug");
#endif
#if 0
                                 printf ("In AttachPreprocessingInfoTreeTrav::evaluateSynthesizedAttribute(): case V_SgSourceFile: Calling SageInterface::insertStatementBefore(): it->second = %p = %s \n",it->second,it->second->class_name().c_str());
#endif
                                 SageInterface::insertStatementBefore(it->second, it->first, false);
                               }

#if 1
                            if (statementsToInsertAfter.size() > 0)
                               {
                                 printf ("In AttachPreprocessingInfoTreeTrav::evaluateSynthesizedAttribute(): case V_SgSourceFile: process statementsToInsertAfter (size = %zu) \n",
                                      statementsToInsertAfter.size());
                               }
#endif
                            for (list<pair<SgIncludeDirectiveStatement*, SgStatement*> >::const_iterator it = statementsToInsertAfter.begin(); it != statementsToInsertAfter.end(); it++) 
                               {
#if 0
                                 printf ("In AttachPreprocessingInfoTreeTrav::evaluateSynthesizedAttribute(): case V_SgSourceFile: Calling SageInterface::insertStatementAfter(): it->second = %p = %s \n",it->second,it->second->class_name().c_str());
#endif
#if 0
                                 printf ("In AttachPreprocessingInfoTreeTrav::evaluateSynthesizedAttribute(): it->first->get_directiveString() = %s \n",it->first->get_directiveString().c_str());
#endif
                                 SgClassDefinition* classDefinition = isSgClassDefinition(it -> second);
                                 if (classDefinition != NULL)
                                    {
                                    // Since the parent of SgClassDefinition is SgClassDeclaration, whose implementation for child insertion is not provided, insert after the
                                    // last statement of SgClassDefinition instead.
                                       SgDeclarationStatement* lastMember = (classDefinition -> get_members()).back();
                                       SageInterface::insertStatementAfter(lastMember, it -> first, false);
                                    } 
                                   else 
                                    {
                                      SgBasicBlock* basicBlock = isSgBasicBlock(it -> second);
                                      if (basicBlock != NULL)
                                         {
                                        // Do not insert after a basic block, but rather insert as the last statement of the basic block.
                                           SageInterface::insertStatementAfter(basicBlock -> lastStatement(), it -> first, false);
                                         }
                                        else
                                         {
                                           ROSE_ASSERT(it->second != NULL);
#if 0
                                           printf ("it->second = %p = %s \n",it->second,it->second->class_name().c_str());
#endif
                                           ROSE_ASSERT(it->first != NULL);
#if 0
                                           printf ("it->first = %p = %s \n",it->first,it->first->class_name().c_str());
#endif
                                        // Handle other scopes.
                                        // SgScopeStatement* scope = isSgScopeStatement(it->second);
                                           SgGlobal* globalScope = isSgGlobal(it->second);
                                           if (globalScope != NULL)
                                              {
                                                printf ("globalScope->get_declarations().size() = %zu \n",globalScope->get_declarations().size());
                                                if (globalScope->get_declarations().empty() == false)
                                                   {
                                                  // When there is no statement outside of the frontend (rose_edg_required_macros_and_functions.h), we want 
                                                  // to put this after the last statement from rose_edg_required_macros_and_functions.h.
                                                     SgStatement* firstStatement = globalScope->get_declarations()[0];
                                                     printf ("Addressing insertion into globa scope: firstStatement = %p = %s \n",firstStatement,firstStatement->class_name().c_str());
                                                     ROSE_ASSERT(firstStatement != NULL);

                                                     SgStatement* firstStatementAfterPreincludeStatements = SageInterface::lastFrontEndSpecificStatement(globalScope);
                                                     ROSE_ASSERT(firstStatementAfterPreincludeStatements != NULL);
#if 0
                                                     printf ("Addressing insertion into globa scope: firstStatementAfterPreincludeStatements = %p = %s \n",
                                                          firstStatementAfterPreincludeStatements,firstStatementAfterPreincludeStatements->class_name().c_str());
#endif
                                                     SageInterface::insertStatementAfter(firstStatement, it->first, false);
                                                   }
                                                  else
                                                   {
                                                  // DQ (11/21/2018): Adding.
                                                     printf ("Global scope is empty! \n");
                                                     ROSE_ASSERT(false);
                                                   }
#if 0
                                                printf ("Exiting as a test! \n");
                                                ROSE_ASSERT(false);
#endif
                                              }
                                             else
                                              {
                                                SageInterface::insertStatementAfter(it->second, it->first, false);
                                              }
                                         }
                                    }
                               }

#if 0
                         // DQ (11/3/2019): Force processing to exit afer a selected header file is processed.
                               {
                              // SgSourceFile* sourceFile = isSgSourceFile(currentFilePtr);
                                 bool isHeaderFile = sourceFile->get_isHeaderFile();
#if 0
                                 printf ("isHeaderFile = %s \n",isHeaderFile ? "true" : "false");
#endif
                                 if (isHeaderFile == true)
                                    {
                                      SgIncludeFile* includeFile = sourceFile->get_associated_include_file();
                                      ROSE_ASSERT(includeFile != NULL);
                                      ROSE_ASSERT(includeFile->get_source_file() == sourceFile);
#error "DEAD CODE!"
                                     if (includeFile->get_source_file()->getFileName() == "/home/quinlan1/ROSE/git_rose_development/tests/nonsmoke/functional/CompileTests/UnparseHeadersTests/test4/SimpleInternal.h")
                                        {
                                          printf ("Exiting as a test after nested traversal over the global scope in the include file \n");
                                          ROSE_ASSERT(false);
                                        }
#if 0
                                     printf ("Exiting as a test! \n");
                                     ROSE_ASSERT(false);
#endif
                                    }
                               }
#endif


#if 0
                         printf ("In AttachPreprocessingInfoTreeTrav::evaluateSynthesizedAttribute(): case V_SgSourceFile: end of case \n");
#endif
                         break;
                       }

                 // This case helps place the comment or directive relative to the closing brace of a SgBasicBlock.
                    case V_SgBasicBlock:
                          {
#if 0
                            printf ("Processing case: V_SgBasicBlock \n");
#endif
                            ROSE_ASSERT (locatedNode->get_endOfConstruct() != NULL);

                         // The following should always work since each statement is a located node
                            SgBasicBlock* basicBlock = dynamic_cast<SgBasicBlock*>(n);
#if 0
                            printf ("Case SgBasicBlock: lineOfClosingBrace = %d \n",lineOfClosingBrace);
                            printf ("Case SgBasicBlock: See if we can find a better target to attach these comments than %s \n",previousLocNodePtr->sage_class_name());
#endif
#if 0
                            printf ("In case V_SgBasicBlock: calling iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber() \n");
#endif
                         // DQ (6/8/2020): This appear to be NULL in some cases I am debugging currently.
                            ROSE_ASSERT(basicBlock != NULL);

                         // DQ (3/18/2005): This is a more robust process (although it introduces a new location for a comment/directive)
                            bool reset_start_index = false;
                            iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                              ( basicBlock, lineOfClosingBrace, PreprocessingInfo::inside, reset_start_index, currentListOfAttributes );

                         // DQ (4/9/2005): We need to point to the SgBasicBlock and not the last return statement (I think)
                         // Reset the previousLocNodePtr to the current node so that all 
                         // PreprocessingInfo objects will be inserted relative to the 
                         // current node next time.
                         // previousLocNodePtr = basicBlock;
#if 1
                         // DQ (6/24/2020): Set this only in the evaluateInheritedAttribute() function.
#if 0
                         // DQ (5/1/2020): Removed the previousLocatedNodeMap.
                            previousLocatedNodeMap[currentFileNameId] = basicBlock;
#else
                            previousLocatedNode = basicBlock;
#endif
#endif
                            break;
                          }
                        // Liao 11/2/2010, support #include within SgAggregateInitializer { }   
                        // e.g.
                        /*
                             static const char c_tree_code_type[] = {
                                 'x',
                                 #include "c-common.def"
                             };
                        */

                    case V_SgAggregateInitializer:
                          {
#if 0
                            printf ("Processing case: V_SgAggregateInitializer \n");
#endif
                            ROSE_ASSERT (locatedNode->get_endOfConstruct() != NULL);

                            SgAggregateInitializer* target = dynamic_cast<SgAggregateInitializer*>(n);
#if 0
                            printf ("Case SgAggregateInitializer: Calling iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber() using locatedNode->get_file_info()->get_file_id() = %d \n",
                                 locatedNode->get_file_info()->get_file_id());
#endif
#if 0
                            printf ("In case V_SgAggregateInitializer: calling iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber() \n");
#endif
                         // DQ (6/8/2020): This appear to be NULL in some cases I am debugging currently.
                            ROSE_ASSERT(target != NULL);

                            bool reset_start_index = false;
                            iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                              ( target, lineOfClosingBrace, PreprocessingInfo::inside, reset_start_index, currentListOfAttributes );

#if 1
                         // DQ (6/24/2020): Set this only in the evaluateInheritedAttribute() function.
#if 0
                         // DQ (5/1/2020): Removed the previousLocatedNodeMap.
                            previousLocatedNodeMap[currentFileNameId] = target;
#else
                            previousLocatedNode = target;
#endif
#endif
                            break;
                          }

                 // DQ (12/29/2011): Adding support for template class declarations.
                    case V_SgTemplateClassDeclaration:

                    case V_SgClassDeclaration:
                          {
#if 0
                            printf ("Processing case: V_SgClassDeclaration \n");
#endif
                            ROSE_ASSERT (locatedNode->get_endOfConstruct() != NULL);

                         // The following should always work since each statement is a located node
                            SgClassDeclaration* classDeclaration = dynamic_cast<SgClassDeclaration*>(n);
                            ROSE_ASSERT(classDeclaration != NULL);
#if 0
                            printf ("In case V_SgClassDeclaration: calling iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber() \n");
#endif
                         // DQ (6/8/2020): This appear to be NULL in some cases I am debugging currently.
                            ROSE_ASSERT(previousLocNodePtr != NULL);

                         // DQ (3/18/2005): This is a more robust process (although it introduces a new location for a comment/directive)
                            bool reset_start_index = false;
                            iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                              ( previousLocNodePtr, lineOfClosingBrace, PreprocessingInfo::after, reset_start_index,currentListOfAttributes );
                         // printf ("Adding comment/directive to base of class declaration \n");
                         // iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                         //    ( locatedNode, lineOfClosingBrace, PreprocessingInfo::inside );

                         // previousLocNodePtr = classDeclaration;
#if 1
                         // DQ (6/24/2020): Set this only in the evaluateInheritedAttribute() function.
#if 0
                         // DQ (5/1/2020): Removed the previousLocatedNodeMap.
                            previousLocatedNodeMap[currentFileNameId] = classDeclaration;
#else
                            previousLocatedNode = classDeclaration;
#endif
#endif
                            break;
                          }

                 // GB (09/18/2007): Added support for preprocessing info inside typedef declarations (e.g. after the
                 // base type, which is what the previousLocNodePtr might point to).
                    case V_SgTypedefDeclaration:
                          {
#if 0
                            printf ("Processing case: V_SgTypedefDeclaration \n");
#endif
                            ROSE_ASSERT(locatedNode->get_endOfConstruct() != NULL);

                            SgTypedefDeclaration *typedefDeclaration = isSgTypedefDeclaration(n);
                            ROSE_ASSERT(typedefDeclaration != NULL);
#if 0
                            printf ("In case V_SgTypedefDeclaration: calling iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber() \n");
#endif
                         // DQ (6/8/2020): This appear to be NULL in some cases I am debugging currently.
                            ROSE_ASSERT(previousLocNodePtr != NULL);

                            bool reset_start_index = false;
                            iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                              ( previousLocNodePtr, lineOfClosingBrace, PreprocessingInfo::after, reset_start_index,currentListOfAttributes );

                         // previousLocNodePtr = typedefDeclaration;
#if 1
                         // DQ (6/24/2020): Set this only in the evaluateInheritedAttribute() function.
#if 0
                         // DQ (5/1/2020): Removed the previousLocatedNodeMap.
                            previousLocatedNodeMap[currentFileNameId] = typedefDeclaration;
#else
                            previousLocatedNode = typedefDeclaration;
#endif
#endif
                            break;
                          }

                 // DQ (12/29/2011): Adding support for template variable declarations.
                    case V_SgTemplateVariableDeclaration:

                 // GB (09/19/2007): Added support for preprocessing info inside variable declarations (e.g. after the
                 // base type, which is what the previousLocNodePtr might point to).
                    case V_SgVariableDeclaration:
                          {
#if 0
                            printf ("Processing case: V_SgVariableDeclaration \n");
#endif
                            ROSE_ASSERT(locatedNode->get_endOfConstruct() != NULL);

                            SgVariableDeclaration *variableDeclaration = isSgVariableDeclaration(n);
                            ROSE_ASSERT(variableDeclaration != NULL);
#if 0
                            printf ("In AttachPreprocessingInfoTreeTrav::evaluateSynthesizedAttribute(): variableDeclaration = %p \n",variableDeclaration);
#endif
#if 0
                            printf ("In case V_SgVariableDeclaration: calling iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber() \n");
#endif
                         // DQ (6/9/2020): This appear to be NULL in some cases I am debugging currently.
                            ROSE_ASSERT(previousLocNodePtr != NULL);

                            bool reset_start_index = false;
                            iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                              ( previousLocNodePtr, lineOfClosingBrace, PreprocessingInfo::after, reset_start_index,currentListOfAttributes );

                         // previousLocNodePtr = variableDeclaration;
#if 1
                         // DQ (6/24/2020): Set this only in the evaluateInheritedAttribute() function.
#if 0
                         // DQ (5/1/2020): Removed the previousLocatedNodeMap.
                            previousLocatedNodeMap[currentFileNameId] = variableDeclaration;
#else
                            previousLocatedNode = variableDeclaration;
#endif
#endif
                            break;
                          }

                 // DQ (10/25/2012): Added new case.  I expect this might be important for test2012_78.c
                    case V_SgInitializedName:
                          {
#if 0
                            printf ("Processing case: V_SgInitializedName \n");
#endif
// #ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
#if 0
                            printf ("In AttachPreprocessingInfoTreeTrav::evaluateSynthesizedAttribute(): Added new support for preprocessing info to be added after the SgInitializedName. \n");
#endif
                            ROSE_ASSERT(locatedNode->get_endOfConstruct() != NULL);

                            SgInitializedName *initializedName = isSgInitializedName(n);
                            ROSE_ASSERT(initializedName != NULL);
#if 0
                            printf ("In AttachPreprocessingInfoTreeTrav::evaluateSynthesizedAttribute(): initializedName = %p name = %s \n",initializedName,initializedName->get_name().str());
#endif
#if 0
                            printf ("In case V_SgInitializedName: calling iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber() \n");
#endif
// <<<<<<< HEAD
                         // DQ (6/9/2020): This appears to be NULL in some cases I am debugging currently.
                            ROSE_ASSERT(previousLocNodePtr != NULL);
#if 0
                            printf ("case V_SgInitializedName: Before calling iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber(): previousLocNodePtr = %p \n",previousLocNodePtr);
                            if (previousLocNodePtr != NULL)
                               {
                                 printf (" --- previousLocNodePtr = %p = %s \n",previousLocNodePtr,previousLocNodePtr->class_name().c_str());
                               }
#endif
// =======
                            // Liao 6/10/2020, Fortran subroutine will have a SgInitializedName generated in AST to represent the subroutine name.
                            // It is compiler-generated and has no appearance in the original source code. 
                            // We should not attach comments to it.
                            if (SageInterface::is_Fortran_language ())
                            {
                              if (isSgProcedureHeaderStatement(initializedName->get_parent()))
                              {
                            //    cout<<"Found Fortran subroutine init name, skipping attaching comments to it..."<< initializedName <<endl;
                              }
                            }
                            else
                            {

// >>>>>>> origin/master
                            bool reset_start_index = false;
                            iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                              ( previousLocNodePtr, lineOfClosingBrace, PreprocessingInfo::after, reset_start_index, currentListOfAttributes );

#if 1
                         // DQ (6/24/2020): Set this only in the evaluateInheritedAttribute() function.
#if 0
                         // DQ (5/1/2020): Removed the previousLocatedNodeMap.
                            previousLocatedNodeMap[currentFileNameId] = initializedName;
// <<<<<<< HEAD
#else
                            previousLocatedNode = initializedName;
#endif
#endif
#if 0
                            printf ("Processing case: END OF CASE V_SgInitializedName \n");
#endif

// =======
                            }
// >>>>>>> origin/master
                            break;
                          }

                    case V_SgClassDefinition:
                          {
#if 0
                            printf ("Processing case: V_SgClassDefinition \n");
#endif
                            ROSE_ASSERT (locatedNode->get_endOfConstruct() != NULL);

                         // DQ (3/19/2005): This is a more robust process (although it introduces a new location for a comment/directive)
                         // iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                         //    ( previousLocNodePtr, lineOfClosingBrace, PreprocessingInfo::after );
                         // printf ("Adding comment/directive to base of class definition \n");
#if 0
                            printf ("In case V_SgClassDefinition: calling iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber() \n");
#endif
                         // DQ (6/9/2020): This appear to be NULL in some cases I am debugging currently.
                            ROSE_ASSERT(locatedNode != NULL);

                            bool reset_start_index = false;
                            iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                              ( locatedNode, lineOfClosingBrace, PreprocessingInfo::inside, reset_start_index,currentListOfAttributes );

                         // previousLocNodePtr = locatedNode;
                         // previousLocatedNodeMap[currentFileNameId] = locatedNode;
                            break;
                          }

                    case V_SgEnumDeclaration:
                          {
#if 0
                            printf ("Processing case: V_SgEnumDeclaration \n");
#endif
                            ROSE_ASSERT (locatedNode->get_endOfConstruct() != NULL);

                         // The following should always work since each statement is a located node
                            SgEnumDeclaration* enumDeclaration = dynamic_cast<SgEnumDeclaration*>(n);
                            ROSE_ASSERT(enumDeclaration != NULL);
#if 0
                            printf ("In case V_SgEnumDefinition: calling iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber() \n");
#endif
                         // DQ (3/18/2005): This is a more robust process (although it introduces a new location for a comment/directive)
                         // iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                         //    ( previousLocNodePtr, lineOfClosingBrace, PreprocessingInfo::after );
                         // printf ("Adding comment/directive to base of enum declaration \n");

                         // DQ (6/9/2020): This appear to be NULL in some cases I am debugging currently.
                            ROSE_ASSERT(locatedNode != NULL);

                            bool reset_start_index = false;
                            iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                              ( locatedNode, lineOfClosingBrace, PreprocessingInfo::inside, reset_start_index,currentListOfAttributes );

                         // previousLocNodePtr = enumDeclaration;
#if 1
                         // DQ (6/24/2020): Set this only in the evaluateInheritedAttribute() function.
#if 0
                         // DQ (5/1/2020): Removed the previousLocatedNodeMap.
                            previousLocatedNodeMap[currentFileNameId] = enumDeclaration;
#else
                            previousLocatedNode = enumDeclaration;
#endif
#endif
                            break;
                          }

                    // DQ (5/3/2004): Added support for namespaces
                    case V_SgNamespaceDeclarationStatement:
                         {
#if 0
                            printf ("Processing case: V_SgNamespaceDeclarationStatement \n");
#endif
                            ROSE_ASSERT (locatedNode->get_endOfConstruct() != NULL);

                         // The following should always work since each statement is a located node
                         // SgNamespaceDeclarationStatement* namespaceDeclaration = dynamic_cast<SgNamespaceDeclarationStatement*>(n);
                         // SgNamespaceDeclarationStatement* namespaceDeclaration = isSgNamespaceDeclarationStatement(n);
                            SgNamespaceDeclarationStatement* namespaceDeclaration = dynamic_cast<SgNamespaceDeclarationStatement*>(n);
                            ROSE_ASSERT(namespaceDeclaration != NULL);

                         // DQ (6/9/2020): This appear to be NULL in some cases I am debugging currently.
                            ROSE_ASSERT(previousLocNodePtr != NULL);
#if 0
                            printf ("In case V_SgNamespaceDeclarationStatement: calling iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber() \n");
#endif
                            bool reset_start_index = false;
                            iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                              ( previousLocNodePtr, lineOfClosingBrace, PreprocessingInfo::after, reset_start_index,currentListOfAttributes );

                         // previousLocNodePtr = namespaceDeclaration;
#if 1
                         // DQ (6/24/2020): Set this only in the evaluateInheritedAttribute() function.
#if 0
                         // DQ (5/1/2020): Removed the previousLocatedNodeMap.
                            previousLocatedNodeMap[currentFileNameId] = namespaceDeclaration;
#else
                            previousLocatedNode = namespaceDeclaration;
#endif
#endif
                            break;
                         }

                    // DQ (5/3/2004): Added support for namespaces
                    case V_SgNamespaceDefinitionStatement:
                       {
                         ROSE_ASSERT (locatedNode->get_endOfConstruct() != NULL);

                      // The following should always work since each statement is a located node
                         SgNamespaceDefinitionStatement* namespaceDefinition = dynamic_cast<SgNamespaceDefinitionStatement*>(n);
                         ROSE_ASSERT(namespaceDefinition != NULL);

                      // DQ (3/18/2005): This is a more robust process (although it introduces a new location for a comment/directive)
                      // iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                      //    ( previousLocNodePtr, lineOfClosingBrace, PreprocessingInfo::after );
                      // printf ("Adding comment/directive to base of namespace definition \n");

                      // DQ (6/9/2020): This appear to be NULL in some cases I am debugging currently.
                         ROSE_ASSERT(previousLocNodePtr != NULL);

#if 0
                         printf ("In case V_SgNamespaceDefinitionStatement: calling iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber() \n");
#endif
                         bool reset_start_index = false;
                         iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                              ( locatedNode, lineOfClosingBrace, PreprocessingInfo::inside, reset_start_index,currentListOfAttributes );

                      // previousLocNodePtr = namespaceDefinition;
#if 1
                         // DQ (6/24/2020): Set this only in the evaluateInheritedAttribute() function.
#if 0
                      // DQ (5/1/2020): Removed the previousLocatedNodeMap.
                         previousLocatedNodeMap[currentFileNameId] = namespaceDefinition;
#else
                         previousLocatedNode = namespaceDefinition;
#endif
#endif
                         break;
                       }

                    // DQ (4/9/2005): Added support for templates instaiations which are compiler generated
                    //                but OK to attach comments to them (just not inside them!).
                    case V_SgTemplateInstantiationMemberFunctionDecl:
                       {
                         ROSE_ASSERT (locatedNode->get_endOfConstruct() != NULL);
                      // printf ("Found a SgTemplateInstantiationMemberFunctionDecl but only record it as a previousLocNodePtr \n");

                      // DQ (6/9/2020): This appear to be NULL in some cases I am debugging currently.
                         ROSE_ASSERT(locatedNode != NULL);
#if 0
                         printf ("In case V_SgTemplateInstantiationMemberFunctionDecl: calling iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber() \n");
#endif
                      // DQ (3/11/2012): Added recursive call to insert comments.
                         bool reset_start_index = false;
                         iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                           ( locatedNode, lineOfClosingBrace, PreprocessingInfo::inside, reset_start_index,currentListOfAttributes );

                      // previousLocNodePtr = locatedNode;
#if 1
                         // DQ (6/24/2020): Set this only in the evaluateInheritedAttribute() function.
#if 0
                      // DQ (5/1/2020): Removed the previousLocatedNodeMap.
                         previousLocatedNodeMap[currentFileNameId] = locatedNode;
#else
                         previousLocatedNode = locatedNode;
#endif
#endif
                      // DQ (3/11/2012): Added break statement to prevent fall through, I think this fixes a bug.
                         break;
                       }

                 // DQ (5/13/2012): Added case.
                    case V_SgTemplateClassDefinition:

                 // DQ (3/11/2012): Added case.
                    case V_SgTemplateFunctionDefinition:

                 // DQ (8/12/2012): Added support for attaching comments after a SgFunctionDefinition.
                    case V_SgFunctionDefinition:

                 // DQ (12/29/2011): Adding support for template function and member function declarations.
                    case V_SgTemplateFunctionDeclaration:
                    case V_SgTemplateMemberFunctionDeclaration:

                 // DQ (4/21/2005): this can be the last statement and if it is we have to 
                 // record it as such so that directives/comments can be attached after it.
                    case V_SgTemplateInstantiationDirectiveStatement:
                 // case V_SgFunctionParameterList:
                    case V_SgFunctionDeclaration:   // Liao 11/8/2010, this is necessary since SgInitializedName might be a previous located node.
                                 // we don't want to attach anything after an ending initialized name,
                                 // So we give a chance to the init name's ancestor a chance. 
                                 // For preprocessing info appearing after a last init name, we attach it inside the ancestor.

                    case V_SgMemberFunctionDeclaration:
                    case V_SgTemplateInstantiationFunctionDecl:
                       {
                         ROSE_ASSERT (locatedNode->get_endOfConstruct() != NULL);

#if 0
                         printf ("Case SgFunctionDefinition or SgFunctionDeclaration: Calling iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber() using locatedNode->get_file_info()->get_file_id() = %d \n",
                                 locatedNode->get_file_info()->get_file_id());
#endif
#if 0
                         printf ("In case V_xxx: calling iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber(): PreprocessingInfo::inside: n = %p = %s \n",n,n->class_name().c_str());
#endif
                      // DQ (6/9/2020): This appear to be NULL in some cases I am debugging currently.
                         ROSE_ASSERT(locatedNode != NULL);

                      // DQ (3/11/2012): Added recursive call to insert comments.
                         bool reset_start_index = false;
                         iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber( locatedNode, lineOfClosingBrace, PreprocessingInfo::inside, reset_start_index,currentListOfAttributes );

#if 0
                         printf ("DONE: Case SgFunctionDefinition or SgFunctionDeclaration: Calling iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber() using locatedNode->get_file_info()->get_file_id() = %d \n",
                                 locatedNode->get_file_info()->get_file_id());
                         printf ("DONE: In case V_xxx: calling iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber(): PreprocessingInfo::inside: n = %p = %s \n",n,n->class_name().c_str());
#endif

                      // previousLocNodePtr = locatedNode;
#if 1
                      // DQ (6/24/2020): Set this only in the evaluateInheritedAttribute() function.
#if 0
                      // DQ (5/1/2020): Removed the previousLocatedNodeMap.
                         previousLocatedNodeMap[currentFileNameId] = locatedNode;
#else
                         previousLocatedNode = locatedNode;
#endif
#endif
                      // DQ (3/11/2012): Added break statement to prevent fall through, I think this fixes a bug.
                         break;
                       }

                    default:
                       {
                      // DQ (11/11/2012): Added assertion.
                         ROSE_ASSERT(n != NULL);

#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
                         printf ("Skipping any possability of attaching a comment/directive after a %s \n",n->class_name().c_str());
                      // ROSE_ASSERT(false);
#endif
#if DEBUG_ATTACH_PREPROCESSING_INFO
                         ROSE_ASSERT(n->get_file_info() != NULL);
                         n->get_file_info()->display("Skipping any possability of attaching a comment/directive: debug");
#endif
#if 0
                         // DQ (6/24/2020): Set this only in the evaluateInheritedAttribute() function.
                      // DQ (6/17/2020): Set the previousLocatedNode
                         if (locatedNode != NULL)
                            {
                              previousLocatedNode = locatedNode;
                            }
                           else
                            {
                              ROSE_ASSERT(previousLocatedNode != NULL);
#if 0
                              printf ("Skip setting the previousLocatedNode = %p = %s \n",previousLocatedNode,previousLocatedNode->class_name().c_str());
#endif
                            }
#endif
                       }
                  }

             // DQ (6/17/2020): Need to check for null pointer.
                ROSE_ASSERT(previousLocatedNode != NULL);
#if 0
                printf ("AFTER SWITCH: previousLocatedNode = %p = %s \n",previousLocatedNode,previousLocatedNode->class_name().c_str());
#endif

#if 0
             // DQ (6/24/2020): Make sure that that previousLocatedNode is from the file matching the collected comments and CPP directives.
                printf ("target_source_file_id = %d \n",target_source_file_id);
#endif
                ROSE_ASSERT(previousLocatedNode->get_file_info() != NULL);
#if 0
                printf ("previousLocatedNode->get_file_info()->get_file_id() = %d \n",previousLocatedNode->get_file_info()->get_file_id());
#endif
                if (previousLocatedNode->get_file_info()->get_physical_file_id() != target_source_file_id)
                   {
                     printf ("Error: previousLocatedNode->get_file_info()->get_file_id() != target_source_file_id \n");
                     previousLocatedNode->get_file_info()->display("Error: previousLocatedNode->get_file_info()->get_file_id() != target_source_file_id");
                     printf (" --- previousLocatedNode->get_file_info()->get_file_id() = %d \n",previousLocatedNode->get_file_info()->get_file_id());
                     printf (" --- target_source_file_id                               = %d \n",target_source_file_id);
                   }
                ROSE_ASSERT(previousLocatedNode->get_file_info()->get_physical_file_id() == target_source_file_id);

#if 0
            // DQ (6/9/2020): This appear to be NULL in some cases I am debugging currently.
            // ROSE_ASSERT(previousLocNodePtr != NULL);
               if (previousLocNodePtr == NULL)
                  {
                    printf ("After switch: previousLocNodePtr == NULL \n");
                  }
                 else
                  {
                    printf ("After switch: previousLocNodePtr = %p = %s \n",previousLocNodePtr,previousLocNodePtr->class_name().c_str());
                  }
#endif
             } // if compiler generated or match current file

#if 0
          if (locatedNode != NULL)
             {
               printf ("Output attached comments: \n");
               printOutComments(locatedNode);
             }
#endif
        } // end if (locatedNode) || (fileNode != NULL)

#if 0
     printf ("Leaving AttachPreprocessingInfoTreeTrav::evaluateSynthesizedAttribute(): n = %p = %s \n",n,n->class_name().c_str());
     printf (" --- previousLocatedNode = %p = %s \n",previousLocatedNode,(previousLocatedNode != NULL) ? previousLocatedNode->class_name().c_str() : "null");
#endif

  // DQ (6/15/2020): Set the previous node to be the current node as we leave evaluateSynthesizedAttribute().
     if (previousLocatedNode == NULL)
        {
          previousLocatedNode = isSgLocatedNode(n);
#if 0
          printf ("Set previousLocatedNode to %p = %s \n",previousLocatedNode,previousLocatedNode->class_name().c_str());
#endif
        }

#if 0
  // DQ (6/15/2020): This appear to be NULL in some cases I am debugging currently.
  // ROSE_ASSERT(previousLocNodePtr != NULL);
     if (previousLocatedNode == NULL)
        {
          printf (" --- previousLocatedNode == NULL \n");
        }
       else
        {
          printf (" --- previousLocatedNode = %p = %s \n",previousLocatedNode,previousLocatedNode->class_name().c_str());
        }
#endif

     return returnSynthesizeAttribute;
   }

// ifndef  CXX_IS_ROSE_CODE_GENERATION
// #endif 
