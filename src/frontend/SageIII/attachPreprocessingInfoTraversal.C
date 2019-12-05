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
     std::map<std::string, SgIncludeFile*> edg_include_file_map;
#endif
   }

// DQ (11/30/2008): Refactored this code out of the simpler function to isolate 
// the Wave specific handling.  


AttachPreprocessingInfoTreeTrav::AttachPreprocessingInfoTreeTrav( SgSourceFile* file, bool includeDirectivesAndCommentsFromAllFiles )
   {
     use_Wave = file->get_wave();

  // Wave will get all Preprocessor Diretives by default and it is therefore reasonable
  // that it will attach all
#if 0
     if(use_Wave)
          processAllIncludeFiles = true;
       else
#endif

     processAllIncludeFiles = includeDirectivesAndCommentsFromAllFiles;

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

  // DQ (2/28/2019): We need to return the line that is associated with the source file where this can be a ode shared between multiple ASTs.
     ROSE_ASSERT(sourceFile != NULL);
     ROSE_ASSERT(sourceFile->get_file_info() != NULL);
     source_file_id = sourceFile->get_file_info()->get_physical_file_id();

  // DQ (11/20/2019): Check this.
  // ROSE_ASSERT(sourceFile->get_globalScope() != NULL);

#if 0
     printf ("In AttachPreprocessingInfoTreeTrav() constructor: sourceFile = %p sourceFile->getFileName() = %s \n",sourceFile,sourceFile->getFileName().c_str());
     printf (" --- sourceFile->get_globalScope() = %p \n",sourceFile->get_globalScope());
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

  // printf ("   previousLocNodePtr            = %p = %s \n",previousLocNodePtr,previousLocNodePtr != NULL ? previousLocNodePtr->class_name().c_str() : "NULL");
  // printf ("   currentListOfAttributes       = %p \n",currentListOfAttributes);
  // printf ("   sizeOfCurrentListOfAttributes = %d \n",sizeOfCurrentListOfAttributes);
  // printf ("   currentFileNameId             = %d \n",currentFileNameId);
  // printf ("   start_index                   = %d \n",start_index);
  // printf ("   currentMapOfAttributes        = %p \n",currentMapOfAttributes);

 //  Call the separate support for output of the static data
 //  AttachPreprocessingInfoTreeTrav::display_static_data(label);

     printf ("attributeMapForAllFiles: \n");
     for (AttributeMapType::const_iterator i = attributeMapForAllFiles.begin(); i != attributeMapForAllFiles.end(); i++)
        {
          printf ("   file id = %d list pointer = %p list size = %d filename = %s \n",
               i->first,i->second,(int)((i->second != NULL) ? i->second->size() : -1),Sg_File_Info::getFilenameFromID(i->first).c_str());

       // After the traversal, the last action in the evaluateSynthesizedAttribute() function for a SgSourceFile,
       // is to set the pointer to NULL. So this is OK to be NULL if this function is called after the traversal.
       // ROSE_ASSERT(i->second != NULL);
          if (i->second != NULL)
             {
               i->second->display("Called from AttachPreprocessingInfoTreeTrav::display_static_data()");
             }
        }

     printf ("previousLocatedNodeMap: \n");
     for (previousLocatedNodeInFileType::const_iterator i = previousLocatedNodeMap.begin(); i != previousLocatedNodeMap.end(); i++)
        {
          printf ("   id = %d previous node = %p = %s file = %s \n",
               i->first,i->second,(i->second != NULL) ? i->second->class_name().c_str() : "NULL",Sg_File_Info::getFilenameFromID(i->first).c_str());

       // After the traversal, the last action in the evaluateSynthesizedAttribute() function for a SgSourceFile,
       // is to set the pointer to NULL. So this is OK to be NULL if this function is called after the traversal.
       // ROSE_ASSERT(i->second != NULL);
        }

     printf ("startIndexMap: \n");
     for (StartingIndexAttributeMapType::const_iterator i = startIndexMap.begin(); i != startIndexMap.end(); i++)
        {
          printf ("   id = %d starting index = %d file = %s \n",
               i->first,i->second,Sg_File_Info::getFilenameFromID(i->first).c_str());
        }
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

#if 0
  // DQ (8/22/2018): Added debugging information.
     printf ("In AttachPreprocessingInfoTreeTrav::iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber(): currentListOfAttributes->size() = %d \n",currentListOfAttributes->size());
#endif
#if 0
     currentListOfAttributes->display("Top of iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber()");
#endif

#if 0
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

#if 0
     int currentFileId = (sourceFile->get_requires_C_preprocessor() == true) ? 
                         Sg_File_Info::getIDFromFilename(sourceFile->generate_C_preprocessor_intermediate_filename(sourceFile->get_file_info()->get_filename())) : 
                         locatedFileInfo->get_file_id();
#else
  // DQ (12/18/2012): Switch to using the physical file id now that we support this feature.
     int currentFileId = (sourceFile->get_requires_C_preprocessor() == true) ? 
                         Sg_File_Info::getIDFromFilename(sourceFile->generate_C_preprocessor_intermediate_filename(sourceFile->get_file_info()->get_filename())) : 
                         locatedFileInfo->get_physical_file_id(source_file_id);
#endif

#if 0
     printf ("In iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber(): currentFileId = %d currentListOfAttributes->get_filenameIdSet().size() = %" PRIuPTR " \n",currentFileId,currentListOfAttributes->get_filenameIdSet().size());

     set<int>::iterator i = currentListOfAttributes->get_filenameIdSet().begin();
     while (i != currentListOfAttributes->get_filenameIdSet().end())
        {
          printf ("equivalent file = %d = %s \n",*i,Sg_File_Info::getFilenameFromID(*i).c_str());
          i++;
        }
#endif
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

     int start_index = startIndexMap[currentFileId];

     if (attributeMapForAllFiles.find(currentFileId) == attributeMapForAllFiles.end())
        {
          printf ("Error: locatedNode = %p = %s currentFileId = %d file = %s \n",locatedNode,locatedNode->class_name().c_str(),currentFileId,Sg_File_Info::getFilenameFromID(currentFileId).c_str());
          locatedFileInfo->display("In AttachPreprocessingInfoTreeTrav::iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber()");
        }
     ROSE_ASSERT(attributeMapForAllFiles.find(currentFileId) != attributeMapForAllFiles.end());
     int sizeOfCurrentListOfAttributes = attributeMapForAllFiles[currentFileId]->size();

#if 0
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
               isFortranBlockAndBeforePoisition = true; 
        }

  // DQ (12/23/2008): Note: I think that this should be turned into a while loop (starting at start_index,
  // to lineNumber when location == PreprocessingInfo::before, and to the sizeOfCurrentListOfAttributes 
  // when location == PreprocessingInfo::after).
     if (!isFortranBlockAndBeforePoisition)
        {
#if 0
       // DQ (12/23/2008): This is tighter control over the number of iterations required.
          int i = start_index;

       // We might want this to be tighter in the cases of (location != PreprocessingInfo::before)
       // Not that here we use a billion to represent a large number.
          int bound = (location == PreprocessingInfo::before) ? lineNumber : 1000000000;

          PreprocessingInfo *currentPreprocessingInfoPtr = (*currentListOfAttributes)[i];
       // while (currentPreprocessingInfoPtr->getLineNumber() < bound && i < sizeOfCurrentListOfAttributes)
          while ( (i < sizeOfCurrentListOfAttributes) && (currentPreprocessingInfoPtr->getLineNumber() < bound) )
#endif
#if 0
            printf ("start_index = %d sizeOfCurrentListOfAttributes = %d \n",start_index,sizeOfCurrentListOfAttributes);
#endif
          list<pair<SgIncludeDirectiveStatement*, SgStatement*> > localStatementsToInsertAfter;
          for ( int i = start_index; i < sizeOfCurrentListOfAttributes; i++ )
             {
               PreprocessingInfo *currentPreprocessingInfoPtr = (*currentListOfAttributes)[i];
#if 0
               printf ("Processing (*currentListOfAttributes)[%3d] = %p string = %s \n",i,currentPreprocessingInfoPtr,currentPreprocessingInfoPtr->getString().c_str());
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
#if 0
               printf ("currentPreprocessingInfoLineNumber = %d lineNumber = %d \n",currentPreprocessingInfoLineNumber,lineNumber);
#endif

#if 1
            // DQ (12/23/2008): So far this is the most reliable way to break out of the loop.
               ROSE_ASSERT(currentPreprocessingInfoPtr != NULL);
               if ( (currentPreprocessingInfoLineNumber > lineNumber) && (location == PreprocessingInfo::before) )
                  {
                 // DQ (12/23/2008): I think that under this constraint we could exit this loop!
#if 0
                    printf ("Warning: Why are we searching this list of PreprocessingInfo beyond the line number of the current statement (using break) \n");
#endif
                 // DQ (12/23/2008): I don't like the design which forces an exit from the loop here, but this is the most robust implementation so far.
                    break;
                  }
#endif
            // bool attachCommentOrDirective = (currentPreprocessingInfoPtr != NULL) && (currentPreprocessingInfoPtr->getLineNumber() <= lineNumber);
               bool attachCommentOrDirective = (currentPreprocessingInfoLineNumber <= lineNumber);

            // DQ (1/7/2019): Supress comments and CPP directives onto member functions of the generated labda function class.
               SgLambdaExp* lambdaExpression = isSgLambdaExp(locatedNode->get_parent());
               if (lambdaExpression != NULL)
                  {
#if 0
                    printf ("NOTE: Detected lambda expression as parent of the located node = %p = %s: suppress attachment of comments and CPP directives \n",locatedNode,locatedNode->class_name().c_str());
#endif
                    attachCommentOrDirective = false;
                  }
#if 0
               printf ("attachCommentOrDirective = %s currentPreprocessingInfoLineNumber = %d lineNumber = %d \n",attachCommentOrDirective ? "true" : "false",currentPreprocessingInfoLineNumber,lineNumber);
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
                    printf ("Attaching to nod from locatedNode->get_file_info()->get_filename() = %s \n",locatedNode->get_file_info()->get_filename());
                    printf (" --- currentListOfAttributes->getFileName()                        = %s \n",currentListOfAttributes->getFileName().c_str());
#endif

                 // DQ (11/4/2019): If we want this assertion then it likely should be based on physical filenames (derived from physical fid ids).
                 // DQ (11/3/2019): Check that the comment or CPP directive is from the same file as the locatedNode.
                 // A variation of this test might be required later, though we should only be attacheing comments and 
                 // CPP directives before possible transformations.
                    printf ("In iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber(): locatedNode->get_file_info()->get_filename() = %s \n",locatedNode->get_file_info()->get_filename());
                    printf ("In iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber(): currentListOfAttributes->getFileName()       = %s \n",currentListOfAttributes->getFileName().c_str());
                 // ROSE_ASSERT(locatedNode->get_file_info()->get_filename() == currentListOfAttributes->getFileName());

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

                 // DQ (4/13/2007): If we are going to invalidate the list of accumulated attributes then we can start 
                 // next time at the next index (at least).  This removes the order n^2 complexity of traversing over the whole loop.
                 // start_index = i+1;
                    ROSE_ASSERT(startIndexMap.find(currentFileId) != startIndexMap.end());
                    startIndexMap[currentFileId] = i+1;
                 // printf ("Incremented start_index to be %d \n",startIndexMap[currentFileId]);

                 // Mark the location relative to the current node where the PreprocessingInfo 
                 // object should be unparsed (before or after) relative to the current locatedNode
                    currentPreprocessingInfoPtr->setRelativePosition(location);
#if 0
                    printf ("Attaching CPP directives %s to IR nodes as attributes. \n",PreprocessingInfo::directiveTypeName(currentPreprocessingInfoPtr->getTypeOfDirective()).c_str());
#endif
#if 1
                 // This uses the old code to attach comments and CPP directives to the AST as attributes.
                    locatedNode->addToAttachedPreprocessingInfo(currentPreprocessingInfoPtr);

                 // DQ (12/2/2018): This fails for the C/C++ snippet insertion tests.
                 // DQ (12/2/2018): This fails for Fortran.
                 // DQ (9/5/2018): We should have already set the preprocessorDirectivesAndCommentsList, checked in getTokenStream().
                 // ROSE_ASSERT(sourceFile->get_preprocessorDirectivesAndCommentsList() != NULL);
                 // if (SageInterface::is_Fortran_language() == false)
                    if (SageInterface::is_C_language() == true || SageInterface::is_Cxx_language() == true)
                       {
                      // ROSE_ASSERT(sourceFile->get_preprocessorDirectivesAndCommentsList() != NULL);
                       }
#if 0
                    printf ("sourceFile->getFileName()                            = %s \n",sourceFile->getFileName().c_str());
                    printf ("sourceFile->get_unparseHeaderFiles()                 = %s \n",sourceFile->get_unparseHeaderFiles() ? "true" : "false");
                    printf ("sourceFile->get_header_file_unparsing_optimization() = %s \n",sourceFile->get_header_file_unparsing_optimization() ? "true" : "false");
                    printf ("currentPreprocessingInfoPtr->getTypeOfDirective() == PreprocessingInfo::CpreprocessorIncludeDeclaration = %s \n",
                            currentPreprocessingInfoPtr->getTypeOfDirective() == PreprocessingInfo::CpreprocessorIncludeDeclaration ? "true" : "false");
#endif

                 // DQ (11/3/2019): Adding test for if sourceFile->get_header_file_unparsing_optimization() is true.
                 // negara1 (08/05/2011): If currentPreprocessingInfoPtr is an include directive, get the included file.
                 // If the included file exists, append all its trailing preprocessor directives to its last node and reset its 
                 // start index to the first preprocessor directive.
                 // Proceed only if header files unparsing is enabled.
                 // if (sourceFile->get_unparseHeaderFiles() == true)
                    if (sourceFile->get_unparseHeaderFiles() == true && sourceFile->get_header_file_unparsing_optimization() == false)
                       {
                         if (currentPreprocessingInfoPtr->getTypeOfDirective() == PreprocessingInfo::CpreprocessorIncludeDeclaration)
                            {
                              string includedFileName = sourceFile->get_project()->findIncludedFile(currentPreprocessingInfoPtr);
#if 1
                              printf ("))))))))))))))))))))))))))) sourceFile->get_unparseHeaderFiles() == true: includedFileName = %s \n",includedFileName.c_str());
                           // printf ("   --- includedFileName.length() = %zu \n",includedFileName.length());
#endif
#if 0
                              printf ("sourceFile->get_include_file_list().size() = %zu \n",sourceFile->get_include_file_list().size());
                              for (size_t i = 0; i < sourceFile->get_include_file_list().size(); i++)
                                 {
                                   printf ("Saved include file information: filename = %s \n",sourceFile->get_include_file_list()[i]->get_filename().str());
                                 }
#endif
#if 1
                              printf ("Exiting as a test! \n");
                              ROSE_ASSERT(false);
#endif

#if 0
                              printf ("sourceFile->getFileName()                                        = %s \n",sourceFile->getFileName().c_str());
                              printf ("sourceFile->get_header_file_unparsing_optimization()             = %s \n",sourceFile->get_header_file_unparsing_optimization() ? "true" : "false");
                              printf ("sourceFile->get_header_file_unparsing_optimization_source_file() = %s \n",sourceFile->get_header_file_unparsing_optimization_source_file() ? "true" : "false");
                              printf ("sourceFile->get_header_file_unparsing_optimization_header_file() = %s \n",sourceFile->get_header_file_unparsing_optimization_header_file() ? "true" : "false");
#endif

                              bool collectDataFromThisFile = false;
                              if (sourceFile->get_header_file_unparsing_optimization_source_file() == true)
                                 {
#if 0
                                   printf ("Collect comments and CPP directives from only source file (not header files) \n");
#endif
                                   collectDataFromThisFile = false;
                                 }
                                else
                                 {
#if 0
                                   printf ("Collect comments and CPP directives from all files (source and header files) \n");
#endif
                                   collectDataFromThisFile = true;
                                 }

#if 0
                              printf ("collectDataFromThisFile = %s \n",collectDataFromThisFile ? "true" : "false");
#endif

                           // DQ (9/18/2018): Build a map so that the CPP include directives, processed in later stages of the frontend,
                           // can initialize the pointer in the SgIncludeBody or the SgIncludeDirective so the SgInclude data structure.
                           // We only want to process included files that are a part of the translation unit (for not this will exclude 
                           // pre-include files).
                              SgIncludeFile* include_file = NULL;

                           // DQ (8/19/2019): Adding optimization to separate collection of data from source files and header file.
                           // if (EDG_ROSE_Translation::edg_include_file_map.find(includedFileName) != EDG_ROSE_Translation::edg_include_file_map.end())
                              if (collectDataFromThisFile == true && EDG_ROSE_Translation::edg_include_file_map.find(includedFileName) != EDG_ROSE_Translation::edg_include_file_map.end())
                                 {
#if 0
                                   printf ("Gather comments and CPP directives: includedFileName = %s \n",includedFileName.c_str());
#endif
                                   include_file = EDG_ROSE_Translation::edg_include_file_map[includedFileName];
                                   ROSE_ASSERT(include_file != NULL);

                                // DQ (10/29/2018): Note: FIX ME!
                                // Removing the SgIncludeFile from the EDG_ROSE_Translation::edg_include_file_map and adding 
                                // it to another map is not a really great design idea.  So this should be revisited later.
#if 0
                                   printf (" --- before erase: EDG_ROSE_Translation::edg_include_file_map.size() = %zu \n",EDG_ROSE_Translation::edg_include_file_map.size());
#endif
#if 0
                                // Remove the file from the list to avoid it being refound.
                                   EDG_ROSE_Translation::edg_include_file_map.erase(includedFileName);
#else
                                   printf ("Skipping EDG_ROSE_Translation::edg_include_file_map.erase(includedFileName) \n");
#endif
                                // Add the SgIncludeFile to another list so that we can find it again in the unparsing step, 
                                // when we know what scope needs to be unparsed.  Note that global scope is not enough since
                                // the header file may be associated with inclusion into a nested scope and we require the 
                                // outer most nested scope where it was included.
                                   if (Rose::includeFileMapForUnparsing.find(includedFileName) == Rose::includeFileMapForUnparsing.end())
                                      {
#if 0
                                        printf (" --- Adding this include file to the map to support unparsing (if modified statements are identified) \n");
#endif
                                        Rose::includeFileMapForUnparsing.insert(std::pair<string,SgIncludeFile*>(includedFileName,include_file));
#if 0
                                        printf ("ROSE::includeFileMapForUnparsing.size() = %zu \n",Rose::includeFileMapForUnparsing.size());
#endif
#if 0
                                        printf ("Exiting as a test! \n");
                                        ROSE_ASSERT(false);
#endif
                                      }
#if 0
                                   printf ("after erase: EDG_ROSE_Translation::edg_include_file_map.size() = %zu \n",EDG_ROSE_Translation::edg_include_file_map.size());
#endif
                                 }
                                else
                                 {
                                // NOTE: we want to add this information to the header file report.
#if 0
                                   printf ("COULD NOT find a SgIncludeFile (skip this include file that is not in the translation unit (was not seen by EDG)) \n");
                                   printf (" --- includedFileName = %s \n",includedFileName.c_str());
                                   printf (" --- collectDataFromThisFile = %s \n",collectDataFromThisFile ? "true" : "false");
#endif
                                   if (includedFileName == "/home/quinlan1/ROSE/git_rose_development/tests/nonsmoke/functional/CompileTests/UnparseHeadersTests/test4/SimpleInternal.h")
                                      {
                                        printf ("Found include file that is not in EDG_ROSE_Translation::edg_include_file_map: \n");

                                        std::map<std::string, SgIncludeFile*>::iterator map_itr = EDG_ROSE_Translation::edg_include_file_map.begin();
                                        while (map_itr != EDG_ROSE_Translation::edg_include_file_map.end())
                                          {
                                            printf (" --- map_itr->first = %s map_itr->second = %p = %s \n",map_itr->first.c_str(),map_itr->second,map_itr->second->get_filename().str());
                                            map_itr++;
                                          }

#if 0
                                        printf ("Exiting as a test! \n");
                                        ROSE_ASSERT(false);
#endif
                                      }

                                 }

                           // DQ (9/18/2018): Only process the include file if this matches one that was seen in the EDG/ROSE translation.
                           // Else it is just a CPP include directive that might not have been actually included (since we don't evaluate the
                           // unprocessed regions of CPP directives (e.g. not evaluated because they were contained in a #if 0 ... #endif pairing.
                           // if (includedFileName.length() > 0)
                           // if (include_file != NULL)
#if 0
                              if (include_file != NULL)
                                 {
                                   printf ("include_file->get_isIncludedMoreThanOnce() = %s \n",include_file->get_isIncludedMoreThanOnce() ? "true" : "false");
                                   printf ("include_file->get_isPrimaryUse()           = %s \n",include_file->get_isPrimaryUse() ? "true" : "false");
                                 }
#endif
                              if (include_file != NULL && (include_file->get_isIncludedMoreThanOnce() == false || (include_file->get_isIncludedMoreThanOnce() == true && include_file->get_isPrimaryUse() == true) ) )
                                 {
                                // found the included file
                                   const int OneBillion = 1000000000;
                                   int fileNameId = Sg_File_Info::getIDFromFilename(includedFileName);

                                // A header file might not be present in the map at this point if it contains only preprocessor directives and comments.
                                   if (fileNameId < 0)
                                      {
#if 0
                                        printf ("fileNameId < 0: sourceFile->get_unparseHeaderFiles() == true: addFilenameToMap(): adding includedFileName = %s \n", includedFileName.c_str());
#endif
                                        fileNameId = Sg_File_Info::addFilenameToMap(includedFileName);
                                      }
#if 0
                                   printf ("includedFileName = %s locatedNode = %p = %s \n",includedFileName.c_str(),locatedNode,locatedNode->class_name().c_str());
#endif

#if 1
                                // DQ (9/30/2019): I think this code is not executed when the header file optimization is active,
                                // and that is an issue since the AST does not include the header files required to support the 
                                // header file unparsing of a subset of a scope associated with a specific header file.
                                   printf ("Exiting as a test! \n");
                                   ROSE_ASSERT(false);
#endif

                                // Currently, ROSE supports unparsing of header files only when #include directives do not appear inside expressions.
                                // The only exception is SgHeaderFileBody, whose parent SgIncludeDirectiveStatement is used instead.
                                   SgStatement* locatedStatement;
                                   if (isSgHeaderFileBody(locatedNode) != NULL)
                                      {
                                        locatedStatement = isSgStatement(locatedNode->get_parent());
                                      }
                                     else
                                      {
                                        locatedStatement = isSgStatement(locatedNode);
                                      }

                                   if (locatedStatement == NULL)
                                      {
                                        cout << "Can not handle #include directives inside expressions for header files unpasing:" << locatedNode -> class_name() << endl;
                                        ROSE_ASSERT(false);
                                      }

                                   SgIncludeDirectiveStatement* includeDirectiveStatement = new SgIncludeDirectiveStatement();
                                   ROSE_ASSERT(includeDirectiveStatement != NULL);

                                // DQ (6/3/2019): To be consistant with other SgDeclarationStatement IR nodes, mark this as the defining declaration.
                                   includeDirectiveStatement->set_definingDeclaration(includeDirectiveStatement);
                                   ROSE_ASSERT(includeDirectiveStatement->get_definingDeclaration() != NULL);

                                // DQ (9/18/2018): Adding the connection to the include file hierarchy as generated in the EDG/ROSE translation.
                                   includeDirectiveStatement->set_include_file_heirarchy(include_file);

                                // DQ (11/9/2018): Not clear if this is the best solution.  Might be better to have 
                                // it point to the SgSourceFile or the parent include file IR node.
                                   include_file->set_parent(includeDirectiveStatement);
#if 0
                                   printf ("Attaching CPP directives: include_file->get_name_used_in_include_directive() = %s \n",include_file->get_name_used_in_include_directive().str());
#endif
                                // DQ (8/21/2018): Set a better source file position for the SgIncludeDirectiveStatement.
                                   includeDirectiveStatement->set_startOfConstruct(new Sg_File_Info(*(currentPreprocessingInfoPtr-> get_file_info())));
                                   includeDirectiveStatement->set_endOfConstruct  (new Sg_File_Info(*(currentPreprocessingInfoPtr-> get_file_info())));

                                // DQ (8/24/2018): Connect this properly into the AST.
                                   includeDirectiveStatement->set_parent(sourceFile->get_globalScope());

                                // NOTE: Set, but not used in the current implementation.
                                   includeDirectiveStatement->set_directiveString(currentPreprocessingInfoPtr -> getString());
#if 0
                                   printf ("includeDirectiveStatement->get_directiveString() = %s \n",includeDirectiveStatement->get_directiveString().c_str());
#endif
                                // DQ (11/5/2018): Pass the name that was used to the SgIncludeDirectiveStatement.
                                   includeDirectiveStatement->set_name_used_in_include_directive(include_file->get_name_used_in_include_directive());

                                // DQ (11/9/2018): It might simplify the design to eliminate this IR node since it only contains 
                                // a pointer to the SgSourceFile for the associated header file.
                                   SgHeaderFileBody* headerFileBody = new SgHeaderFileBody();

                                // DQ (8/21/2018): Set a better source file position for the SgHeaderFileBody.
                                   headerFileBody->set_startOfConstruct(new Sg_File_Info(includedFileName));
                                   headerFileBody->set_endOfConstruct  (new Sg_File_Info(includedFileName));

                                   ROSE_ASSERT(headerFileBody->get_endOfConstruct() != NULL);

                                   headerFileBody -> set_parent(includeDirectiveStatement);
                                   includeDirectiveStatement -> set_headerFileBody(headerFileBody);
                                   if (location == PreprocessingInfo::before)
                                      {
#if 0
                                        printf ("@@@@@@@@@@@@@@@@@@@ Insert into statementsToInsertBefore: includeDirectiveStatement = %p = %s at locatedStatement = %p = %s \n",
                                             includeDirectiveStatement,includeDirectiveStatement->class_name().c_str(),
                                             locatedStatement,locatedStatement->class_name().c_str());
#endif
                                        statementsToInsertBefore.push_back(pair<SgIncludeDirectiveStatement*, SgStatement*>(includeDirectiveStatement, locatedStatement));
                                      }
                                     else 
                                      {
#if 0
                                        printf ("@@@@@@@@@@@@@@@@@@@ Insert into statementsToInsertAfter: includeDirectiveStatement = %p = %s at locatedStatement = %p = %s \n",
                                             includeDirectiveStatement,includeDirectiveStatement->class_name().c_str(),
                                             locatedStatement,locatedStatement->class_name().c_str());
#endif
                                     // push_front in order to preserve the order when these include statements are inserted in the AST.
                                        localStatementsToInsertAfter.push_front(pair<SgIncludeDirectiveStatement*, SgStatement*>(includeDirectiveStatement, locatedStatement));
                                      }

                                   SgLocatedNode* targetNode = NULL;
                                   if (previousLocatedNodeMap.find(fileNameId) != previousLocatedNodeMap.end())
                                      {
                                         targetNode = previousLocatedNodeMap[fileNameId];
                                      }
                                   if (targetNode == NULL)
                                      {
                                         // Can be NULL either because it is not found or because it was previously reset.
                                         targetNode = headerFileBody;
                                      }
#if 0
                                // DQ (8/21/2018): check that the source position information is correct.
                                   includeDirectiveStatement->get_startOfConstruct()->display("includeDirectiveStatement: startOfConstruct: debug");
                                   includeDirectiveStatement->get_endOfConstruct()  ->display("includeDirectiveStatement: endOfConstruct: debug");
                                   headerFileBody->get_startOfConstruct()->display("headerFileBody: startOfConstruct: debug");
                                   headerFileBody->get_endOfConstruct()  ->display("headerFileBody: endOfConstruct: debug");
#endif
                                // DQ (9/5/2018): We should have already set the preprocessorDirectivesAndCommentsList, checked in getTokenStream().
                                   if (sourceFile->get_preprocessorDirectivesAndCommentsList() == NULL)
                                      {
                                     // DQ (6/3/2019):Output a warning so we can look into this later.
                                        printf ("WARNING: In iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber(): sourceFile->get_preprocessorDirectivesAndCommentsList() == NULL \n");
                                      }
                                // ROSE_ASSERT(sourceFile->get_preprocessorDirectivesAndCommentsList() != NULL);

                                // DQ (5/28/2019): debugging test2019_441.C.
                                // printf ("Using OneBillion figure for end of construct line number \n");
#if 0
                                   printf ("Calling iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber(): processing header file \n");
#endif
                                   bool reset_start_index = false;
                                   iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber (targetNode, OneBillion, PreprocessingInfo::after, reset_start_index, getListOfAttributes(fileNameId));

                                // Reset the pointer to the previous located node and reset the start index.
                                   previousLocatedNodeMap[fileNameId] = NULL;
                                   startIndexMap[fileNameId] = 0;
#if 0
                                // DQ (8/21/2018): We might want to set the endOfConstruct for the SgHeaderFileBody source position (if so then this might be the values to use for that) \n");
                                   printf ("We could use this information to set the end of the SgHeaderFileBody (no, since this is the location of the nested include directive): \n");
                                   printf ("   --- currentPreprocessingInfoPtr->get_file_info()->filenameString() = %s \n",currentPreprocessingInfoPtr->get_file_info()->get_filenameString().c_str());
                                   printf ("   --- currentPreprocessingInfoPtr->getLineNumber() = %d lineNumber = %d internalString = %s \n",currentPreprocessingInfoPtr->getLineNumber(),lineNumber,currentPreprocessingInfoPtr->getString().c_str());
#endif

#if 0
                                   printf ("Exiting as a test! \n");
                                   ROSE_ASSERT(false);
#endif
#if 0
                                   printf ("$$$$$$$$$$$$$$$$$$ Building new SgSourceFile (to support include file) \n");
#endif
                                // DQ (11/10/2018): This is now set in the EDG/ROSE translation.
                                // DQ (8/23/2018): Collect the comments, CPP directives, and token stream from the include file.
                                // SgSourceFile* include_sourceFile = new SgSourceFile();
                                   SgSourceFile* include_sourceFile = include_file->get_source_file();

                                // DQ (11/22/2018): EDG/ROSE translation skips building SgSourceFile for non-application header files.
                                // ROSE_ASSERT(include_sourceFile != NULL);

                                // DQ (11/21/2018): I think we can assert this here!
                                   ROSE_ASSERT(include_file != NULL);

                                // DQ (9/26/2018): Set the source file in the SgIncludeFile.
                                // if (include_file != NULL)
                                   if (include_file != NULL && include_sourceFile != NULL)
                                      {
                                     // DQ (10/27/2018): If this is an non-null include_file: START.
#if 0
                                        if (include_file->get_source_file() != NULL)
                                           {
                                             printf ("include_file->get_source_file()                = %p \n",include_file->get_source_file());
                                             printf ("include_file->get_source_file()->getFileName() = %s \n",include_file->get_source_file()->getFileName().c_str());
                                           }
#endif
#if 0
                                        if (include_file->get_source_file() != NULL)
                                           {
                                             ROSE_ASSERT(include_file->get_source_file() == NULL);
                                             include_file->set_source_file(include_sourceFile);
                                             ROSE_ASSERT(include_file->get_source_file() != NULL);
                                             ROSE_ASSERT(include_file->get_filename().is_null() == false);
                                           }
#else
                                     // This is now set in the EDG?ROSE connection (check it here).
                                        ROSE_ASSERT(include_file->get_source_file() != NULL);
                                        ROSE_ASSERT(include_file->get_filename().is_null() == false);
#endif
#if 0
                                        printf ("include_file->get_filename() = %s \n",include_file->get_filename().str());
                                        printf ("sourceFile->getFileName()    = %s \n",sourceFile->getFileName().c_str());

                                        printf ("include_file->get_source_file_of_translation_unit()                        = %p = %s \n",
                                             include_file->get_source_file_of_translation_unit(),include_file->get_source_file_of_translation_unit()->class_name().c_str());
                                        printf ("sourceFile                                                                 = %p = %s \n",sourceFile,sourceFile->class_name().c_str());
                                        ROSE_ASSERT(include_file->get_source_file_of_translation_unit() != NULL);
                                        printf ("include_file->get_source_file_of_translation_unit()->getFileName().c_str() = %s \n",include_file->get_source_file_of_translation_unit()->getFileName().c_str());
#endif
                                     // This is now set in EDG/ROSE translation.
                                     // We need to link to the source file for the translation unit, and the source file (or header file) that included this include file.
                                     // include_file->set_source_file_of_translation_unit(sourceFile);
                                        ROSE_ASSERT(include_file->get_source_file_of_translation_unit()->getFileName() == sourceFile->getFileName());
                                        ROSE_ASSERT(include_file->get_source_file_of_translation_unit() == sourceFile);
#if 0
                                        printf ("include_file->get_source_file_of_translation_unit() = %p filename = %s \n",
                                             include_file->get_source_file_of_translation_unit(),
                                             include_file->get_source_file_of_translation_unit()->getFileName().c_str());
                                        printf ("include_file->get_parent_include_file() = %p filename = %s \n",
                                             include_file->get_parent_include_file(),
                                             include_file->get_parent_include_file() != NULL ? include_file->get_parent_include_file()->get_filename().str() : "NULL");
#endif
                                     // This might not be correct yet.
                                        include_file->set_including_source_file(sourceFile);
                                     // include_sourceFile>set_including_source_file(include_file);

                                     // The parent of the SgIncludeDirectiveStatement is ??
                                        ROSE_ASSERT(includeDirectiveStatement->get_parent() != NULL);
#if 0
                                        printf ("includeDirectiveStatement->get_parent() = %p = %s \n",includeDirectiveStatement->get_parent(),includeDirectiveStatement->get_parent()->class_name().c_str());
                                        printf ("include_file->get_parent() = %p = %s \n",include_file->get_parent(),include_file->get_parent()->class_name().c_str());
#endif
                                        SgNode* tmp_parent = include_file;
                                        while (tmp_parent != NULL)
                                           {
#if 0
                                             if (tmp_parent != NULL && tmp_parent->get_parent() != NULL)
                                                {
                                                  printf ("tmp_parent->get_parent() = %p = %s \n",tmp_parent->get_parent(),tmp_parent->get_parent()->class_name().c_str());
                                                }
#endif
#if 0
                                          // DQ (11/9/2018): What is the issue with the parent pointer in SgProject?
                                             if (isSgProject(tmp_parent) != NULL)
                                                {
                                                  tmp_parent = NULL;
                                                }
                                               else
                                                {
                                                  tmp_parent = tmp_parent->get_parent();
                                                }
#else
                                             tmp_parent = tmp_parent->get_parent();
#endif
#if 0
                                             printf ("After increment! tmp_parent = %p \n",tmp_parent);
#endif
                                           }
#if 0
                                        printf ("After while loop! \n");
#endif
#if 0
                                        printf ("Exiting as a test! \n");
                                        ROSE_ASSERT(false);
#endif

                                // DQ (9/7/2018): C/C++ source files and header files should set this to false, since the C preprocessor need not be and is not explicitly called.
                                   include_sourceFile->set_requires_C_preprocessor(false);

                                // DQ (9/7/2018): For C/C++ code this should be false.
                                   ROSE_ASSERT(include_sourceFile->get_requires_C_preprocessor() == false);

                                // DQ (8/23/2018): Point to the generate SgSourceFile for the include file (so that we can reuse it in the unparser).
                                   headerFileBody->set_include_file(include_sourceFile);
                                   include_sourceFile->set_parent(headerFileBody);

                                // DQ (8/7/2018): use of new data member to explicitly mark SgSourceFile as a header file.
                                   include_sourceFile->set_isHeaderFile(true);
#if 0
                                   printf ("In iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber(): unparsedFile marked as header file: include_sourceFile->get_isHeaderFile() = %s \n",
                                        include_sourceFile->get_isHeaderFile() ? "true" : "false");
#endif
                                // DQ (8/3/2018): Set the boolean unparse token option on the new SgSourceFile built for unparsing the header file.
                                   SgProject* project = TransformationSupport::getProject(locatedNode);
                                   ROSE_ASSERT(project != NULL);
                                   bool use_token_based_unparsing = project->get_unparse_tokens();
                                   include_sourceFile->set_unparse_tokens(use_token_based_unparsing);

                                // DQ (10/30/2018): Independent of if we are unparsing via the token stream, we need to mark that the 
                                // header fie is part of the unparsing of header files (so that it will interpret the physical file 
                                // usage correctly in the unparser's statementInFile() function).
                                   include_sourceFile->set_unparseHeaderFiles(true);

                                // DQ (9/7/2018): For C/C++ code this should be false.
                                   ROSE_ASSERT(include_sourceFile->get_requires_C_preprocessor() == false);

                                // TODO: Generalize this hard coded trick.
                                   include_sourceFile->set_Cxx_only(true);
#if 0
                                   printf ("In unparseIncludedFiles(): includedFileName = %s \n",includedFileName.c_str());
#endif
                                   if (use_token_based_unparsing == true)
                                      {
                                     // Check that we have a token stream available, and build one if required.
                                        ROSE_ASSERT(include_sourceFile != NULL);
                                      }
#if 0
                                   printf ("Exiting as a test! \n");
                                   ROSE_ASSERT(false);
#endif

                                // DQ (11/10/2018): This is handled in the EDG/ROSE connection.
                                // DQ (8/6/2018): Adding assertions to support unparse_tokens option.
                                // ROSE_ASSERT(include_sourceFile->get_startOfConstruct() == NULL);
                                // ROSE_ASSERT(include_sourceFile->get_endOfConstruct()   == NULL);
                                   ROSE_ASSERT(include_sourceFile->get_startOfConstruct() != NULL);
                                // ROSE_ASSERT(include_sourceFile->get_endOfConstruct()   != NULL);
                                   ROSE_ASSERT(include_sourceFile->get_endOfConstruct()   == NULL);

                                // DQ (11/10/2018): This is now set in the EDG/ROSE translation.
                                // DQ (8/6/2018): Set the file name for the unparsedFile.
                                // include_sourceFile->set_sourceFileNameWithPath(includedFileName);

                                // DQ (11/10/2018): This is now set in the EDG/ROSE translation.
                                // DQ (8/6/2018): Set the file info for the unparsedFile.
                                // include_sourceFile->set_startOfConstruct(new Sg_File_Info(includedFileName));
                                // include_sourceFile->set_endOfConstruct(new Sg_File_Info(includedFileName));

                                // DQ (8/6/2018): Adding assertions to support unparse_tokens option.
                                   ROSE_ASSERT(include_sourceFile->get_startOfConstruct() != NULL);

                                // DQ (8/24/2018): The SgSourceFile IR node does not have an endOfConstruct.
                                // ROSE_ASSERT(include_sourceFile->get_endOfConstruct()   != NULL);
#if 0
                                // DQ (8/8/2018): The implementation of getFileName accesses the filename stored in the startOfConstruct (which might not be set yet).
                                   printf ("Attaching CPP directives: include_sourceFile->getFileName()                   = %s \n",include_sourceFile->getFileName().c_str());
                                   printf ("Attaching CPP directives: include_sourceFile->get_sourceFileNameWithPath()    = %s \n",include_sourceFile->get_sourceFileNameWithPath().c_str());
                                   printf ("Attaching CPP directives: include_sourceFile->get_sourceFileNameWithoutPath() = %s \n",include_sourceFile->get_sourceFileNameWithoutPath().c_str());
                                   printf ("Attaching CPP directives: include_sourceFile->get_unparse_output_filename()   = %s \n",include_sourceFile->get_unparse_output_filename().c_str());
#endif
#if 0
                                   printf ("$$$$$$$$$$$$$$$$$$ Building new SgSourceFile (to support include file): include_sourceFile = %p filename = %s \n",include_sourceFile,include_sourceFile->getFileName().c_str());
#endif
#if 0
                                   printf ("############## Attaching CPP directives: includedFileName = %s \n",includedFileName.c_str());
                                   printf ("############## Attaching CPP directives: include_sourceFile->getFileName() = %s \n",include_sourceFile->getFileName().c_str());
#endif
                                // DQ (8/6/2018): Check the parent on the new file (this is fixed now).
                                // ROSE_ASSERT(include_sourceFile->get_parent() == NULL);

#if 0
                                // printf ("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ \n");
                                   printf ("Attaching CPP directives: sourceFile->get_unparseHeaderFiles() == true: Calling secondaryPassOverSourceFile() \n");
#endif
#if 0
                                   printf ("sourceFile->get_unparseHeaderFiles() == true: requires_C_preprocessor = %s \n",include_sourceFile->get_requires_C_preprocessor() ? "true" : "false");
#endif
                                // DQ (9/5/2018): I think we need this to support header files.
                                // include_sourceFile->set_requires_C_preprocessor(true);

                                // DQ (9/7/2018): For C/C++ code this should be false.
                                   ROSE_ASSERT(include_sourceFile->get_requires_C_preprocessor() == false);

                                   include_sourceFile->secondaryPassOverSourceFile();
#if 0
                                   printf ("Attaching CPP directives: DONE: sourceFile->get_unparseHeaderFiles() == true: Calling secondaryPassOverSourceFile() \n");
#endif
                                // DQ (9/5/2018): We should have already set the preprocessorDirectivesAndCommentsList, checked in getTokenStream().
                                   ROSE_ASSERT(include_sourceFile->get_preprocessorDirectivesAndCommentsList() != NULL);
#if 0
                                   printf ("include_sourceFile->getFileName() = %s \n",include_sourceFile->getFileName().c_str());
                                   printf ("Calling ROSEAttributesListContainer::display() \n");
                                   include_sourceFile->get_preprocessorDirectivesAndCommentsList()->display("In AttachPreprocessingInfoTreeTrav::iterateOverListAndInsertPreviously...()");
#endif
                                // DQ (9/6/2018): This should be a non-empty list if we are using the token stream.
                                   ROSE_ASSERT (include_sourceFile->get_token_list().empty() == true);
#if 0
                                   printf ("Exiting as a test! \n");
                                   ROSE_ASSERT(false);
#endif
                                // DQ (9/25/2018): This should be NULL, but we might want to only build the token stream mapping later 
                                // after it is initialized (so that the new SgGlobal will be in the token map).
                                   ROSE_ASSERT(include_sourceFile->get_globalScope() == NULL);
#if 0
                                // void buildTokenStreamMapping(SgSourceFile* sourceFile);
                                   buildTokenStreamMapping(include_sourceFile);

                                // DQ (9/6/2018): This should be a non-empty list if we are using the token stream.
                                   ROSE_ASSERT (include_sourceFile->get_token_list().empty() == false);
#if 0
                                   SgTokenPtrList & tokenVector = include_sourceFile->get_token_list();
                                   printf ("\n\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                                   printf ("In AttachPreprocessingInfoTreeTrav::iterateOverListAndInsertPreviously...(): sourceFile->get_token_list().size() = %zu \n",sourceFile->get_token_list().size());
                                   for (size_t i = 0; i < tokenVector.size(); i++)
                                      {
                                        printf ("token #%3zu = %s \n",i,tokenVector[i]->get_lexeme_string().c_str());
                                      }
#endif
#endif
#if 0
                                   printf ("Exiting as a test! \n");
                                   ROSE_ASSERT(false);
#endif
#if 0
                                // DQ (9/6/2018): Added as a test!
                                // Actually in this case there is no include directive for this file because it is included via a pre-include command line option.
                                // Handling this header file might require an explicit step (or it might not be worth supporting since it is ROSE specific).
                                // if (include_sourceFile->getFileName() == "/data1/ROSE_CompileTree/git-LINUX-64bit-5.1.0-EDG412-BOOST_1_60-dq-development-rc-cxx-only/include-staging/g++_HEADERS/rose_edg_required_macros_and_functions.h")
                                   if (include_sourceFile->getFileName() == "/home/quinlan1/ROSE/git_rose_development/tests/nonsmoke/functional/CompileTests/UnparseHeadersTests/test0/Simple.h")
                                      {
                                        printf ("Leaving: /home/quinlan1/ROSE/git_rose_development/tests/nonsmoke/functional/CompileTests/UnparseHeadersTests/test0/Simple.h \n");
                                        ROSE_ASSERT(false);
                                      }
                                     else
                                      {
                                        printf ("Leaving: include_sourceFile->getFileName() = %s \n",include_sourceFile->getFileName().c_str());
                                      }
#else
#if 0
                                   printf ("Leaving: include_sourceFile->getFileName() = %s \n",include_sourceFile->getFileName().c_str());
#endif
#endif
                                // DQ (9/10/2018): We need to build a global scope for each file (but we can to that here or do it later).
                                // However, later in the unparser is where we heed to assign the heeader file's global scope to that of 
                                // the source file's global scope so that each source file has a representation of the global scope but 
                                // can mark it independenty to be unparsed using the token based unparser from the AST or the token stream.
                                   ROSE_ASSERT(include_sourceFile->get_globalScope() == NULL);

                                // DQ (11/10/2018): I think this may have already been set in the EDG/ROSE connection.
                                   ROSE_ASSERT(include_sourceFile->get_file_info() != NULL);

                                   string headerFileName = include_sourceFile->getFileName();
#if 0
                                   Sg_File_Info* headerFileInfo = new Sg_File_Info(headerFileName, 0,0);

                                   include_sourceFile->set_file_info(headerFileInfo);
#endif
                                // Set SgGlobal to avoid problems with checks during unparsing.
                                   SgGlobal* headerFileGlobal = new SgGlobal();
                                   include_sourceFile->set_globalScope(headerFileGlobal);

                                // headerFileGlobal->set_file_info(unparsedFileInfo);
                                   Sg_File_Info* startOfConstructFileInfo = new Sg_File_Info(headerFileName, 0,0);
                                   Sg_File_Info* endOfConstructFileInfo   = new Sg_File_Info(headerFileName, 0,0);

                                   headerFileGlobal->set_startOfConstruct(startOfConstructFileInfo);
                                   headerFileGlobal->set_endOfConstruct  (endOfConstructFileInfo);

                                   headerFileGlobal->set_parent(include_sourceFile);

#if 0
                                // DQ (10/23/2018): Output report of AST nodes marked as modified!
                                   SageInterface::reportModifiedStatements("In AttachPreprocessingInfoTreeTrav::iterateOverListAndInsertPreviously...():after calling set functions",include_sourceFile);
#endif

                                // DQ (10/23/2018): We need to reset the isModified flag for the headerFileGlobal.
                                   headerFileGlobal->set_isModified(false);

#if 0
                                // DQ (10/23/2018): Output report of AST nodes marked as modified!
                                   SageInterface::reportModifiedStatements("In AttachPreprocessingInfoTreeTrav::iterateOverListAndInsertPreviously...():after reset of isModifiedFlag",include_sourceFile);
#endif

                                   ROSE_ASSERT(headerFileGlobal->get_parent() != NULL);

                                   ROSE_ASSERT(include_sourceFile->get_globalScope() != NULL);

                                // DQ (9/14/2018): Set the declaration list in the headerFileGlobal to that of the global scope in the source file.
#if 0
                                   printf ("Assigning the global scope declarations from sourceFile to include_sourceFile \n");
                                   printf ("   --- sourceFile         = %p = %s \n",sourceFile,sourceFile->getFileName().c_str());
                                   printf ("   --- include_sourceFile = %p = %s \n",include_sourceFile,include_sourceFile->getFileName().c_str());
                                   printf ("   --- sourceFile->get_globalScope()->get_declarations().size() = %zu \n",sourceFile->get_globalScope()->get_declarations().size());
                                   printf ("   --- sourceFile->get_unparse_output_filename()                = %s \n",sourceFile->get_unparse_output_filename().c_str());
                                   printf ("   --- include_sourceFile->get_unparse_output_filename()        = %s \n",include_sourceFile->get_unparse_output_filename().c_str());
#endif
                                // This should not have been setup yet.
                                   ROSE_ASSERT(headerFileGlobal->get_declarations().empty() == true);

                                // DQ (9/25/2018): NOTE: we need to add the new SgGlobal IR node into the token mapping 
                                // (with the same entry as for the sourceFile's global scope???)
                                // Copy the list of declarations to the copy of the global scope.
                                   headerFileGlobal->get_declarations() = sourceFile->get_globalScope()->get_declarations();

                                // DQ (9/15/2018): Build report on the header file unparsing.
                                   if (project->get_reportOnHeaderFileUnparsing() == true)
                                      {
                                     // Build up the data structure required to report out the details on the header file unparsing.
#if 0
                                        printf ("Building the data structure to report on header file unparsing. \n");
#endif
                                     // Unclear what to attached the header file report data strcuture to.
                                        SgSourceFile* sourceFile = TransformationSupport::getSourceFile(locatedNode);
                                        ROSE_ASSERT(sourceFile != NULL);
#if 1
                                     // HeaderFileSupportReport* reportData = new HeaderFileSupportReport(sourceFile);
                                        SgHeaderFileReport* reportData = sourceFile->get_headerFileReport();
                                        if (reportData == NULL)
                                           {
#if 0
                                             printf ("Building new report data \n");
#endif
                                             reportData = new SgHeaderFileReport(sourceFile);
                                             sourceFile->set_headerFileReport(reportData);

                                          // DQ (11/10/2018): Double check that this is set.
                                             ROSE_ASSERT(reportData->get_source_file() != NULL);
                                           }
                                          else
                                           {
#if 0
                                             printf ("report data being reused \n");
#endif
                                           }
                                        ROSE_ASSERT(reportData != NULL);
#if 0
                                        printf ("reportData->get_include_file_list().size() = %zu \n",reportData->get_include_file_list().size());
#endif
                                        reportData->get_include_file_list().push_back(include_sourceFile);
#if 1
                                        printf ("################################################### \n");
                                        printf ("################################################### \n");
                                        reportData->display("processing includes");
                                        printf ("################################################### \n");
                                        printf ("################################################### \n");
#endif
#endif
#if 0
                                        printf ("Exiting as a test! \n");
                                        ROSE_ASSERT(false);
#endif
                                      }
#if 0
                                // DQ (9/19/2018): Always insert the include directive before the statement.
                                // Note: the default is already to insert before the next statement, so this has little effect.
                                // An option might be that the include should appear before the statements that are from the 
                                // include file, which would be slightly different semantics. This is a topic for later work.
                                   location = PreprocessingInfo::before;
#endif
#if 0
                                   printf ("Exiting as a test! \n");
                                   ROSE_ASSERT(false);
#endif

#if 1
                                // void buildTokenStreamMapping(SgSourceFile* sourceFile);
                                   buildTokenStreamMapping(include_sourceFile);

                                // DQ (9/6/2018): This should be a non-empty list if we are using the token stream.
                                   ROSE_ASSERT (include_sourceFile->get_token_list().empty() == false);
#if 0
                                   SgTokenPtrList & tokenVector = include_sourceFile->get_token_list();
                                   printf ("\n\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                                   printf ("In AttachPreprocessingInfoTreeTrav::iterateOverListAndInsertPreviously...(): sourceFile->get_token_list().size() = %zu \n",sourceFile->get_token_list().size());
                                   for (size_t i = 0; i < tokenVector.size(); i++)
                                      {
                                        printf ("token #%3zu = %s \n",i,tokenVector[i]->get_lexeme_string().c_str());
                                      }
#endif
#endif
#if 0
                                // DQ (9/25/2018): Test if the token stream mapping includes the global scope.
                                // NOTE: this is an issue for the global scope added for each include file (which maybe should be done before the mapping is built).
                                   printf ("sourceFile         = %p file name = %s \n",sourceFile,sourceFile->getFileName().c_str());
                                   printf ("   --- sourceFile->get_globalScope()                        = %p \n",sourceFile->get_globalScope());
                                   printf ("   --- sourceFile->get_tokenSubsequenceMap().size()         = %zu \n",sourceFile->get_tokenSubsequenceMap().size());
                                   printf ("include_sourceFile = %p file name = %s \n",include_sourceFile,include_sourceFile->getFileName().c_str());
                                   printf ("   --- include_sourceFile->get_tokenSubsequenceMap().size() = %zu \n",include_sourceFile->get_tokenSubsequenceMap().size());
                                   printf ("   --- include_sourceFile->get_globalScope()                = %p \n",include_sourceFile->get_globalScope());
#endif
#if 0
                                // DQ (9/26/2018): We can't enforce this because the call to buildTokenStreamMapping() on the source file (*.C file) 
                                // happens after the support for adding CPP directives and comments.
                                // ROSE_ASSERT(sourceFile->get_tokenSubsequenceMap().find(sourceFile->get_globalScope())                 != sourceFile->get_tokenSubsequenceMap().end());

                                // DQ (9/26/2018): But we should be able to enforce this for the current header file we have just processed.
                                   ROSE_ASSERT(include_sourceFile->get_tokenSubsequenceMap().find(include_sourceFile->get_globalScope()) != include_sourceFile->get_tokenSubsequenceMap().end());

                                   printf ("Calling display on token sequence for global scope \n");
                                   ROSE_ASSERT(include_sourceFile->get_tokenSubsequenceMap()[include_sourceFile->get_globalScope()] != NULL);
                                   TokenStreamSequenceToNodeMapping* tokenSequence = include_sourceFile->get_tokenSubsequenceMap()[include_sourceFile->get_globalScope()];
                                   ROSE_ASSERT(tokenSequence != NULL);
                                   tokenSequence->display("token sequence for global scope");
                                // include_sourceFile->get_tokenSubsequenceMap()[include_sourceFile->get_globalScope()]->second->display("token sequence for global scope");
#endif

#if 0
                                // DQ (10/23/2018): Output report of AST nodes marked as modified!
                                   SageInterface::reportModifiedStatements("In AttachPreprocessingInfoTreeTrav::iterateOverListAndInsertPreviously...():",include_sourceFile);
#endif
#if 0
                                   printf ("Exiting as a test! \n");
                                   ROSE_ASSERT(false);
#endif

                                // DQ (10/27/2018): If this is an non-null include_file: END.
                                      }
                                 }
                            }
                       }
                      else 
                       {
                      // Sara Royuela (Nov 9th, 2012)
                      // When collecting comment and directives, 
                      // we attach preprocessed info comming from headers without any SgLocated node to the current SgFile
                         if ( sourceFile->get_collectAllCommentsAndDirectives() )
                            {
                              if (currentPreprocessingInfoPtr -> getTypeOfDirective() == PreprocessingInfo::CpreprocessorIncludeDeclaration)
                                 {
                                   string includedFileName = sourceFile -> get_project() -> findIncludedFile(currentPreprocessingInfoPtr);
                                   if ( includedFileName.size() > 0 )
                                      {
                                        int fileNameId = Sg_File_Info::getIDFromFilename(includedFileName);
                                     // We only add the preprocessor directives and comments to file only in the case the file only contains preprocessor information
                                     // When this occurs, the file is not present in the map at this point 
                                        if (fileNameId < 0)
                                           {
                                             fileNameId = Sg_File_Info::addFilenameToMap(includedFileName);
#if 0
                                             printf ("Calling getListOfAttributes(): includedFileName = %s we attach preprocessed info comming from headers without any SgLocated node to the current SgFile \n",includedFileName.c_str());
#endif
                                             ROSEAttributesList* headerAttributes = getListOfAttributes(fileNameId);
                                             if ( headerAttributes->size() )
                                                {
                                                  string filename = sourceFile->get_sourceFileNameWithPath();
                                                  sourceFile->get_preprocessorDirectivesAndCommentsList()->addList(filename, headerAttributes);
                                                }
                                           }
                                      }
                                 }
                            }
                       }
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
             }

       // DQ (1/7/2019): This appears to be nearly always an empty list, so we can improve the performance and also simlify the debugging with this test.
          if (localStatementsToInsertAfter.empty() == false)
             {
#if 0
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
       // Reset all the start_index data members (for each associated file)
       // start_index = 0;
          for (StartingIndexAttributeMapType::iterator it = startIndexMap.begin(); it != startIndexMap.end(); it++)
             {
               it->second = 0;
             }
        }

#if 0
     string dotgraph_filename = "include_file_graph_from_attach_CPP_directives";
     SgProject* project = SageInterface::getProject(XXX);
     ROSE_ASSERT(project != NULL);
     generateGraphOfIncludeFiles(project,dotgraph_filename);
#endif

#if 0
  // DQ (10/27/2019): Added debugging information.
     printf ("Leaving AttachPreprocessingInfoTreeTrav::iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber(): currentListOfAttributes->size() = %d \n",currentListOfAttributes->size());
#endif

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

#if 0
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
          previousLocatedNodeMap[currentFileId] = currentLocNodePtr;
          ROSE_ASSERT(previousLocatedNodeMap.find(currentFileId) != previousLocatedNodeMap.end());

       // Supports assertions at end of function
          previousLocNodePtr = currentLocNodePtr;
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

       // printf ("parentStatement = %s \n",parentStatement->sage_class_name());
          previousLocatedNodeMap[currentFileId] = parentStatement;

       // Supports assertions at end of function
          previousLocNodePtr = parentStatement;
        }

  // Nodes that should not have comments attached (since they are not unparsed directly 
  // within the generation of the source code by the unparser (no associated unparse functions))
     ROSE_ASSERT (dynamic_cast<SgForInitStatement*>     (previousLocNodePtr) == NULL);
     ROSE_ASSERT (dynamic_cast<SgTypedefSeq*>           (previousLocNodePtr) == NULL);
     ROSE_ASSERT (dynamic_cast<SgCatchStatementSeq*>    (previousLocNodePtr) == NULL);
     ROSE_ASSERT (dynamic_cast<SgFunctionParameterList*>(previousLocNodePtr) == NULL);
     ROSE_ASSERT (dynamic_cast<SgCtorInitializerList*>  (previousLocNodePtr) == NULL);
   }


ROSEAttributesList* 
AttachPreprocessingInfoTreeTrav::buildCommentAndCppDirectiveList ( bool use_Wave, std::string fileNameForDirectivesAndComments )
   {
  // This function abstracts the collection of comments and CPP directives into a list.  
  // The list is then used to draw from as the AST is traversed and the list elements 
  // are woven into the AST.
#if 0
     printf ("Inside of AttachPreprocessingInfoTreeTrav::buildCommentAndCppDirectiveList(use_Wave = %s) \n",use_Wave ? "true" : "false");
#endif

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
          int currentFileNameId = sourceFile->get_file_info()->get_file_id();
#if 0
          printf ("Generating a new ROSEAttributesList: currentFileNameId = %d \n",currentFileNameId);
#endif

       // DQ (11/2/2019): A call to getListOfAttributes() will generate infinite recursion.
       // returnListOfAttributes = getListOfAttributes(currentFileNameId);
          returnListOfAttributes = getPreprocessorDirectives(fileNameForDirectivesAndComments);
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
                    returnListOfAttributes = getPreprocessorDirectives(fileNameForDirectivesAndComments);
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

     ROSEAttributesList* currentListOfAttributes = NULL;

#if 0
     printf ("In AttachPreprocessingInfoTreeTrav::getListOfAttributes() currentFileNameId = %d file = %s \n",currentFileNameId,Sg_File_Info::getFilenameFromID(currentFileNameId).c_str());
#endif

  // Check if this is a file id that is associated with a source file or a special 
  // value to represent compiler generated IR nodes, transformations, etc.
     if (currentFileNameId >= 0)
        {
       // Check if the attributes have been gathered for this file
          if (attributeMapForAllFiles.find(currentFileNameId) == attributeMapForAllFiles.end())
             {

            // If not then read the file and collect the CPP directives and comments from each file.

            // We always want to process the source file, but not always all the include files.
            // int sourceFileNameId = sourceFile->get_file_info()->get_file_id();
               Sg_File_Info* sourceFileInfo = sourceFile->get_file_info();
#if 0
               int sourceFileNameId = (sourceFile->get_requires_C_preprocessor() == true) ? 
                                  Sg_File_Info::getIDFromFilename(sourceFile->generate_C_preprocessor_intermediate_filename(sourceFileInfo->get_filename())) : 
                                  sourceFileInfo->get_file_id();

#error "DEAD CODE!"

#else
#if 0
               int sourceFileNameId = (sourceFile->get_requires_C_preprocessor() == true) ? 
                                  Sg_File_Info::getIDFromFilename(sourceFile->generate_C_preprocessor_intermediate_filename(sourceFileInfo->get_filename())) : 
                                  sourceFileInfo->get_physical_file_id();
#else
            // DQ (9/7/2018): Actually the default for C/C++ code should be that get_requires_C_preprocessor() == false, the other case is for C preprocessed fortran code.
            // DQ (9/5/2018): We must use the full file name.
               int sourceFileNameId = (sourceFile->get_requires_C_preprocessor() == true) ? 
                               // Sg_File_Info::getIDFromFilename(sourceFileInfo->get_filename()) : 
                                  Sg_File_Info::getIDFromFilename(sourceFile->generate_C_preprocessor_intermediate_filename(sourceFileInfo->get_filename())) : 
                                  sourceFileInfo->get_physical_file_id(source_file_id);
#endif
#endif

            // DQ (11/2/2019): Unparsing of header files requires that we gather the CPP directives and comments from header 
            // files in addition to the original source files, so the currentFileNameId and sourceFileNameId will not always match,
            // and yet we don't want to skip the collection of CPP directives and comments.
            // bool skipProcessFile = (processAllIncludeFiles == false) && (currentFileNameId != sourceFileNameId);
               bool skipProcessFile = (processAllIncludeFiles == true);
#if 0
               printf ("In AttachPreprocessingInfoTreeTrav::getListOfAttributes(): currentFileNameId = %d sourceFileNameId = %d skipProcessFile = %s \n",
                    currentFileNameId,sourceFileNameId,skipProcessFile ? "true" : "false");
#endif
               if (skipProcessFile == false)
                  {
#if 0
                    printf ("In AttachPreprocessingInfoTreeTrav::getListOfAttributes(): currentFileNameId = %d sourceFileNameId = %d Sg_File_Info::getFilenameFromID(currentFileNameId) = %s \n",
                         currentFileNameId,sourceFileNameId,Sg_File_Info::getFilenameFromID(currentFileNameId).c_str());
#endif

                 // DQ (11/2/2019): Make sure that we never insert a NULL pointer into the attributeMapForAllFiles.
                 // attributeMapForAllFiles[currentFileNameId] = buildCommentAndCppDirectiveList(use_Wave, Sg_File_Info::getFilenameFromID(currentFileNameId) );
                    ROSEAttributesList* commentAndCppDirectiveList = buildCommentAndCppDirectiveList(use_Wave, Sg_File_Info::getFilenameFromID(currentFileNameId) );
                    ROSE_ASSERT(commentAndCppDirectiveList != NULL);
                    attributeMapForAllFiles[currentFileNameId] = commentAndCppDirectiveList;

                    ROSE_ASSERT(attributeMapForAllFiles.find(currentFileNameId) != attributeMapForAllFiles.end());
                    currentListOfAttributes = attributeMapForAllFiles[currentFileNameId];
                    ROSE_ASSERT(currentListOfAttributes != NULL);
                  }
             }
            else
             {
               currentListOfAttributes = attributeMapForAllFiles[currentFileNameId];
               ROSE_ASSERT(currentListOfAttributes != NULL);
             }
        }

  // DQ (5/19/2013): Added test... only valid for specific test codes with appropriate CPP directives.
  // ROSE_ASSERT(currentListOfAttributes != NULL);

#if 0
     printf ("Leaving AttachPreprocessingInfoTreeTrav::getListOfAttributes(): currentListOfAttributes = %p currentListOfAttributes->size() = %d \n",
          currentListOfAttributes,currentListOfAttributes != NULL ? currentListOfAttributes->size() : -1);
#endif

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
     printf ("In AttachPreprocessingInfoTreeTrav::evaluateInheritedAttribute(): n = %p = %s \n",n,n->class_name().c_str());
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

       // Temporary code (testing this)
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

       // DQ (8/19/2019): Avoid processing this if we are optimizing the header file unparsing and only processing the header files.
          ROSEAttributesList* currentListOfAttributes = NULL;
          if (currentFilePtr->get_header_file_unparsing_optimization_header_file() == true)
             {
#if 0
               printf ("Skip processing the source file when using the headed file optimiation \n");
#endif
#if 0
               printf ("Collect comments and CPP directives: currentFileNameId = %d \n",currentFileNameId);
#endif

            // DQ (10/21/2019): This will be tested below, if it is not in place then we need to do it here.
               ROSEAttributesListContainerPtr filePreprocInfo = currentFilePtr->get_preprocessorDirectivesAndCommentsList();
            // ROSE_ASSERT(filePreprocInfo->getList().empty() == false);

            // DQ (12/3/2019): I think we need this.
               ROSE_ASSERT(filePreprocInfo != NULL);
#if 1
            // DQ (10/21/2019): This code does not change the assertion below.
            // currentListOfAttributes = getListOfAttributes(currentFileNameId);
            // if (filePreprocInfo->getList().find(sourceFile->get_file_info()->get_filename()) == filePreprocInfo->getList().end())
               if (filePreprocInfo->getList().find(currentFilePtr->get_file_info()->get_filename()) == filePreprocInfo->getList().end())
                  {
#if 0
                    printf ("Generating a new ROSEAttributesList: currentFileNameId = %d \n",currentFileNameId);
#endif
                    currentListOfAttributes = getListOfAttributes(currentFileNameId);
#if 0
                    printf ("DONE: Generating a new ROSEAttributesList \n");
#endif
                  }
                 else
                  {
#if 0
                    printf ("Using the existing ROSEAttributesList \n");
#endif
                 // currentListOfAttributes = filePreprocInfo->getList()[sourceFile->get_file_info()->get_filename()];
                    currentListOfAttributes = filePreprocInfo->getList()[currentFilePtr->get_file_info()->get_filename()];
#if 0
                    printf ("DONE: Using the existing ROSEAttributesList \n");
#endif
                  }

               ROSE_ASSERT(currentListOfAttributes != NULL);
#endif

#if 1
            // DQ (11/2/2019): Avoid redundent reading of the source file for CPP directives and comments.
            // if (filePreprocInfo->getList().find(sourceFile->get_file_info()->get_filename()) == filePreprocInfo->getList().end())
               if (filePreprocInfo->getList().find(currentFilePtr->get_file_info()->get_filename()) == filePreprocInfo->getList().end())
                  {
#if 0
                    printf ("filePreprocInfo->getList() DOES NOT have an entry for this file: adding one: filename = %s \n",sourceFile->get_file_info()->get_filename());
#endif
                 // currentListOfAttributes = filePreprocInfo->getList()[sourceFile->get_file_info()->get_filename()];
                 // filePreprocInfo->getList()[sourceFile->get_file_info()->get_filename()] = currentListOfAttributes;
                    filePreprocInfo->getList()[currentFilePtr->get_file_info()->get_filename()] = currentListOfAttributes;
                  }
                 else
                  {
                 // ROSE_ASSERT (filePreprocInfo->getList().find(sourceFile->get_file_info()->get_filename()) != filePreprocInfo->getList().end());
                 // printf ("Error: we should have seen the list of CPP directives and comments already collected \n");
                 // ROSE_ASSERT(false);
                  }

            // ROSE_ASSERT (filePreprocInfo->getList().find(sourceFile->get_file_info()->get_filename()) != filePreprocInfo->getList().end());
               ROSE_ASSERT (filePreprocInfo->getList().find(currentFilePtr->get_file_info()->get_filename()) != filePreprocInfo->getList().end());
#endif

            // currentListOfAttributes = getListOfAttributes(currentFileNameId);
            // ROSE_ASSERT(currentListOfAttributes != NULL);
#if 0
               printf ("In AttachPreprocessingInfoTreeTrav::evaluateInheritedAttribute(): filePreprocInfo->getList().empty() = %s \n",filePreprocInfo->getList().empty() ? "true" : "false");
#endif
            // ROSE_ASSERT(filePreprocInfo->getList().empty() == false);
             }
            else
             {
#if 0
               printf ("@@@@@@@@@@@@@@@@@@@@ Reading the comments and CPP directives from the current file: \n");
               printf ("   --- currentFileNameId = %d currentFileName = %s \n",currentFileNameId,Sg_File_Info::getFilenameFromID(currentFileNameId).c_str());
#endif
            // This will cause the CPP directives and comments list to be generated for the source file (plus the token stream).
            // ROSEAttributesList* currentListOfAttributes = getListOfAttributes(currentFileNameId);
            // currentListOfAttributes = getListOfAttributes(currentFileNameId);
            // ROSE_ASSERT(currentListOfAttributes != NULL);

               ROSEAttributesListContainerPtr filePreprocInfo = currentFilePtr->get_preprocessorDirectivesAndCommentsList();

            // DQ (12/1/2019): This fails for Fortran support.
            // ROSE_ASSERT(filePreprocInfo != NULL);
               if (filePreprocInfo != NULL)
                  {
                 // if (filePreprocInfo->getList().find(sourceFile->get_file_info()->get_filename()) == filePreprocInfo->getList().end())
                    if (filePreprocInfo->getList().find(currentFilePtr->get_file_info()->get_filename()) == filePreprocInfo->getList().end())
                       {
#if 0
                         printf ("Generating a new ROSEAttributesList (currentFilePtr->get_header_file_unparsing_optimization_header_file() == false) \n");
#endif
                         currentListOfAttributes = getListOfAttributes(currentFileNameId);
#if 0
                         printf ("DONE: Generating a new ROSEAttributesList (currentFilePtr->get_header_file_unparsing_optimization_header_file() == false) \n");
#endif
                       }
                      else
                       {
#if 0
                         printf ("Using the existing ROSEAttributesList (currentFilePtr->get_header_file_unparsing_optimization_header_file() == false) \n");
#endif
                      // currentListOfAttributes = filePreprocInfo->getList()[sourceFile->get_file_info()->get_filename()];
                         currentListOfAttributes = filePreprocInfo->getList()[currentFilePtr->get_file_info()->get_filename()];
                       }

                    ROSE_ASSERT(currentListOfAttributes != NULL);
                  }

#if 0
               printf ("@@@@@@@@@@@@@@@@@@@@ DONE: Reading the comments and CPP directives from the current file: \n");
               printf ("   --- currentFileNameId = %d currentFileName = %s \n",currentFileNameId,Sg_File_Info::getFilenameFromID(currentFileNameId).c_str());
#endif
             }

       // *************************************************
       // ** Save the ROSEAttributesList into the SgFile **
       // *************************************************

       // We need to extract the token list and save that in the current source file (currentFilePtr).
       // SgSourceFile* sourceFile = isSgSourceFile(currentFilePtr);
       // ROSE_ASSERT(sourceFile != NULL);

       // DQ (10/21/2013): This was reported as an error for test2008_01.F (fortran tests).
       // ROSE_ASSERT(currentFilePtr->get_preprocessorDirectivesAndCommentsList() != NULL);
          ROSEAttributesListContainerPtr filePreprocInfo = currentFilePtr->get_preprocessorDirectivesAndCommentsList();

#if 0
          printf ("In AttachPreprocessingInfoTreeTrav::evaluateInheritedAttribute(): filePreprocInfo = %p \n",filePreprocInfo);
#endif

#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif

          if (filePreprocInfo != NULL)
             {
#if 0
               printf (" --- sourceFile->get_header_file_unparsing_optimization()             = %s \n",sourceFile->get_header_file_unparsing_optimization() ? "true" : "false");
               printf (" --- sourceFile->get_header_file_unparsing_optimization_source_file() = %s \n",sourceFile->get_header_file_unparsing_optimization_source_file() ? "true" : "false");
               printf (" --- sourceFile->get_header_file_unparsing_optimization_header_file() = %s \n",sourceFile->get_header_file_unparsing_optimization_header_file() ? "true" : "false");

               printf (" --- currentFilePtr->get_header_file_unparsing_optimization()             = %s \n",currentFilePtr->get_header_file_unparsing_optimization() ? "true" : "false");
               printf (" --- currentFilePtr->get_header_file_unparsing_optimization_source_file() = %s \n",currentFilePtr->get_header_file_unparsing_optimization_source_file() ? "true" : "false");
               printf (" --- currentFilePtr->get_header_file_unparsing_optimization_header_file() = %s \n",currentFilePtr->get_header_file_unparsing_optimization_header_file() ? "true" : "false");
#endif
            // DQ (8/19/2019): This assertion should depend upon if we are supporting unparse header file optimization and which phase we are in.
            // ROSE_ASSERT(filePreprocInfo->getList().empty() == true);
               if (sourceFile->get_header_file_unparsing_optimization_header_file() == true)
                  {
#if 0
                    printf ("In AttachPreprocessingInfoTreeTrav::evaluateInheritedAttribute(): filePreprocInfo->getList().empty() = %s \n",filePreprocInfo->getList().empty() ? "true" : "false");
#endif
#if 0
                    ROSE_ASSERT(filePreprocInfo->getList().empty() == false);

                 // DQ (8/19/2019): I think we can assert this.
                    ROSE_ASSERT(filePreprocInfo->getList().find(sourceFile->get_file_info()->get_filename()) != filePreprocInfo->getList().end());
#endif
                  }
                 else
                  {
                 // DQ (11/202019): I think that we should have at least seen one file (from where getListOfAttributes() is called above).
                 // ROSE_ASSERT(filePreprocInfo->getList().empty() == true);
                 // ROSE_ASSERT(filePreprocInfo->getList().empty() == false);

                 // DQ (12/4/2019): This fails for Fortran code so skip the test when using Fortran.
                    if (sourceFile->get_Fortran_only() == false)
                       {
                         ROSE_ASSERT(filePreprocInfo->getList().empty() == false);
                       }
                  }
#if 0
               printf ("Put the ROSEAttributesList into the ROSEAttributesListContainer (an stl map) \n");
#endif
            // Put the ROSEAttributesList into the ROSEAttributesListContainer (an stl map)

            // DQ (8/19/2019): We have to allow for the currentListOfAttributes to be NULL when optimizing header 
            // file unparsing performance and header files are being processed instead of the source file.
            // filePreprocInfo->getList()[sourceFile->get_file_info()->get_filename()] = currentListOfAttributes;
               if (currentListOfAttributes != NULL)
                  {
#if 0
                    printf ("In AttachPreprocessingInfoTreeTrav::evaluateInheritedAttribute(): Adding an entry for comments and CPP directives for file = %s \n",sourceFile->get_file_info()->get_filename());
#endif
                 // DQ (10/22/2019): This should be true for us to be adding an entry below.
                 // ROSE_ASSERT(filePreprocInfo->getList().find(sourceFile->get_file_info()->get_filename()) == filePreprocInfo->getList().end());
                 // filePreprocInfo->getList()[sourceFile->get_file_info()->get_filename()] = currentListOfAttributes;
                 // if (filePreprocInfo->getList().find(sourceFile->get_file_info()->get_filename()) == filePreprocInfo->getList().end())
                    if (filePreprocInfo->getList().find(currentFilePtr->get_file_info()->get_filename()) == filePreprocInfo->getList().end())
                       {
#if 0
                      // printf ("filePreprocInfo->getList() DOES NOT have an entry for this file: adding one: filename = %s \n",sourceFile->get_file_info()->get_filename());
                         printf ("filePreprocInfo->getList() DOES NOT have an entry for this file: adding one: filename = %s \n",currentFilePtr->get_file_info()->get_filename());
#endif
                      // filePreprocInfo->getList()[sourceFile->get_file_info()->get_filename()] = currentListOfAttributes;
                         filePreprocInfo->getList()[currentFilePtr->get_file_info()->get_filename()] = currentListOfAttributes;
                       }
                      else
                       {
#if 0
                      // printf ("filePreprocInfo->getList() ALREADY HAS an entry for this file: filename = %s \n",sourceFile->get_file_info()->get_filename());
                         printf ("filePreprocInfo->getList() ALREADY HAS an entry for this file: filename = %s \n",currentFilePtr->get_file_info()->get_filename());
#endif
                       }
                  }
#if 0
               printf ("In AttachPreprocessingInfoTreeTrav::evaluateInheritedAttribute(): filePreprocInfo->getList().size() = %" PRIuPTR " \n",filePreprocInfo->getList().size());
#endif
             }
            else
             {
            // DQ (10/21/2013): I am not clear if this should be a warning, but I disabled the assertion above (required for 
            // Fortran or perhaps masking another issue).  After more investigation, I think this is OK to comment out.
#if 0
               printf ("WARNING: currentFilePtr->get_preprocessorDirectivesAndCommentsList() == NULL \n");
#endif
            // DQ (12/2/2018): This fails for the C/C++ snippet insertion tests.
            // DQ (12/2/2018): This fails for Fortran.
            // DQ (9/5/2018): We should have already set the preprocessorDirectivesAndCommentsList, checked in getTokenStream().
            // ROSE_ASSERT(currentFilePtr->get_preprocessorDirectivesAndCommentsList() != NULL);
            // if (SageInterface::is_Fortran_language() == false)
               if (SageInterface::is_C_language() == true || SageInterface::is_Cxx_language() == true)
                  {
                 // ROSE_ASSERT(currentFilePtr->get_preprocessorDirectivesAndCommentsList() != NULL);
                  }
             }

       // DQ (10/27/2019): Now make sure that we have a valid subtree for this SgSourceFile.
          ROSE_ASSERT(currentFilePtr != NULL);
          SgSourceFile* sourceFile = isSgSourceFile(currentFilePtr);
          ROSE_ASSERT(sourceFile != NULL);
#if 0
          printf ("sourceFile = %p filename = %s \n",sourceFile,sourceFile->getFileName().c_str());
#endif
          ROSE_ASSERT(sourceFile != NULL);
          SgGlobal* globalScope = sourceFile->get_globalScope();
          if (globalScope == NULL)
             {
               bool isHeaderFile = sourceFile->get_isHeaderFile();
#if 0
               printf ("isHeaderFile = %s \n",isHeaderFile ? "true" : "false");
#endif
               if (isHeaderFile == true)
                  {
                    SgIncludeFile* includeFile = sourceFile->get_associated_include_file();
                    ROSE_ASSERT(includeFile != NULL);
                    ROSE_ASSERT(includeFile->get_source_file() == sourceFile);
#if 0
                    printf ("includeFile->get_source_file() = %p filename = %s \n",includeFile->get_source_file(),includeFile->get_source_file()->getFileName().c_str());
#endif
                    SgSourceFile* translation_unit_source_file = includeFile->get_source_file_of_translation_unit();
                    ROSE_ASSERT(translation_unit_source_file != NULL);
#if 0
                    printf ("translation_unit_source_file = %p = filename = %s \n",translation_unit_source_file,translation_unit_source_file->getFileName().c_str());
#endif
#if 0
                    printf ("translation_unit_source_file->get_unparse_output_filename() = %s \n",translation_unit_source_file->get_unparse_output_filename().c_str());
#endif
                    SgGlobal* translation_unit_global_scope = translation_unit_source_file->get_globalScope();
                    ROSE_ASSERT(translation_unit_global_scope != NULL);
#if 0
                 // SgDeclarationStatementPtrList       p_declarations
                    printf ("translation_unit_global_scope->get_declarations().size() = %zu \n",translation_unit_global_scope->get_declarations().size());
#endif
#if 0
                    printf ("sourceFile = %p set_globalScope(): using translation_unit_global_scope = %p \n",sourceFile,translation_unit_global_scope);
                    printf ("sourceFile->get_globalScope() = %p \n",sourceFile->get_globalScope());
                    printf ("sourceFile->getFileName() = %s \n",sourceFile->getFileName().c_str());
#endif
                    sourceFile->set_globalScope(translation_unit_global_scope);

                 // Reset the global scope.
                    globalScope = sourceFile->get_globalScope();

                 // DQ (10/27/2019): We need a nested traversal else we will only visit the global scope of the translation_unit_source_file.
                 // This is because the external traversal is called using traverseWithinFile (for for a different file (I think). 
                    bool processAllFiles = false;

                 // Dummy attribute (nothing is done here since this is an empty class)
                    AttachPreprocessingInfoTreeTraversalInheritedAttrribute inh;

                 // DQ (11/3/2019): Should we be using the sourceFile instead of the translation_unit_source_file?
                 // AttachPreprocessingInfoTreeTrav tt(translation_unit_source_file,processAllFiles);
                    AttachPreprocessingInfoTreeTrav tt(sourceFile,processAllFiles);
#if 0
                    printf ("##### Using the sourceFile instead of the translation_unit_source_file when supporting header files ##### \n");
#endif
#if 0
                    printf ("################################################################################# \n");
                    printf ("################################################################################# \n");
                    printf ("Calling nested traverseWithinFile(): to attach CPP directives and comments to AST \n");
                    printf ("################################################################################# \n");
                    printf ("################################################################################# \n");
#endif
#if 0
                    printf (" --- sourceFile = %p filename = %s \n",sourceFile,sourceFile->getFileName().c_str());
#endif

                 // DQ (11/3/2019): Add a data member to the AttachPreprocessingInfoTreeTraversalInheritedAttrribute to hold the source file 
                 // and set it to the sourceFile so that we can reference the header files in the nested traversal.
                 // Or set the source file in the AttachPreprocessingInfoTreeTrav traversla class (which might be a better idea.
                 // Of could it be that the translation_unit_source_file parameter is the wrong argument to use here.
                 // printf ("This is where I need to pick up the current work in the morning \n");

#if 0
                    printf ("Exiting as a test before the nested traversal over the global scope in the include file \n");
                    ROSE_ASSERT(false);
#endif
                 // DQ (11/2/2019): We want this to traverse only the header file (not the whole translation unit).
                 // However the header file could be nested inside of other scopes (and is at least nested in the 
                 // global scope) so we need to traverse the whole AST.  There might be a better solution longer term
                 // if we could mark all nesting scopes so that they could be traversed using a similar (but maybe 
                 // different) traversal.
                 // tt.traverseWithinFile(translation_unit_source_file,inh);
                 // tt.traverseWithinFile(sourceFile,inh);
#if 0
                    tt.traverse(sourceFile,inh);
#else
#if 0
                    printf ("############################### \n");
                    printf ("############################### \n");
                    printf ("Commented out nested traversal! \n");
                    printf ("############################### \n");
                    printf ("############################### \n");
#else
#if 0
                    printf ("########################################## \n");
                    printf ("########################################## \n");
                    printf ("Previously Commented out nested traversal! \n");
                    printf (" --- sourceFile->get_header_file_unparsing_optimization()             = %s \n",sourceFile->get_header_file_unparsing_optimization() ? "true" : "false");
                    printf (" --- sourceFile->get_header_file_unparsing_optimization_header_file() = %s \n",sourceFile->get_header_file_unparsing_optimization_header_file() ? "true" : "false");
                    printf ("########################################## \n");
                    printf ("########################################## \n");
#endif
                 // DQ (11/5/2019): This is required for the test codes in UnparseHeadersTests, but an error for the the test codes in UnparseHeadersUsingTokenStream_tests.
                    if (sourceFile->get_header_file_unparsing_optimization_header_file() == true)
                       {
                         tt.traverse(sourceFile,inh);
                       }
#endif
#endif

#if 0
                    printf ("Exiting as a test after nested traversal over the global scope in the include file \n");
                    ROSE_ASSERT(false);
#endif
#if 0
                    if (includeFile->get_source_file()->getFileName() == "/home/quinlan1/ROSE/git_rose_development/tests/nonsmoke/functional/CompileTests/UnparseHeadersTests/test4/SimpleInternal.h")
                       {
                         printf ("Exiting as a test after nested traversal over the global scope in the include file \n");
                         ROSE_ASSERT(false);
                       }
#endif
                  }
             }
          ROSE_ASSERT(globalScope != NULL);

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
       // int col  = 0;

       // The following should always work since each statement is a located node
       // currentLocNodePtr = dynamic_cast<SgLocatedNode*>(n);
          currentLocNodePtr = isSgLocatedNode(n);
          ROSE_ASSERT(currentLocNodePtr != NULL);

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
          if (sourceFile->get_file_info()->get_filename() == "/home/quinlan1/ROSE/git_rose_development/tests/nonsmoke/functional/CompileTests/UnparseHeadersTests/test4/Simple4.C")
             {
               printf ("This can be a redundant call to getListOfAttributes() \n");

               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
             }

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
                
#if 0
               printf ("evaluateInheritedAttribute: isCompilerGenerated = %s isTransformation = %s fileIdForOriginOfCurrentLocatedNode = %d \n",
                    isCompilerGenerated ? "true" : "false",isTransformation ? "true" : "false",fileIdForOriginOfCurrentLocatedNode);
#endif
               int currentLocNode_physical_file_id = currentLocNodePtr->get_file_info()->get_physical_file_id();
               string currentLocNode_physical_filename_from_id = Sg_File_Info::getFilenameFromID(currentLocNode_physical_file_id);
#if 0
               printf ("currentLocNode_physical_file_id = %d \n",currentLocNode_physical_file_id);
               printf ("currentLocNode_physical_filename_from_id = %s \n",currentLocNode_physical_filename_from_id.c_str());
#endif
#if 0
               printf ("Attaching to node from currentLocNodePtr->get_file_info()->get_filename() = %s \n",currentLocNodePtr->get_file_info()->get_filename());
               printf (" --- currentListOfAttributes->getFileName()                               = %s \n",currentListOfAttributes->getFileName().c_str());
               printf (" --- currentFileNameId                                                    = %d \n",currentFileNameId);
               printf (" --- source_file_id                                                       = %d \n",source_file_id);
               printf (" --- currentLocNodePtr->get_file_info()->get_file_id()                    = %d \n",currentLocNodePtr->get_file_info()->get_file_id());
               printf (" --- currentLocNodePtr->get_file_info()->get_physical_file_id()           = %d \n",currentLocNodePtr->get_file_info()->get_physical_file_id());
#endif

            // DQ (11/3/2019): I think we want the source_file_id below, since they used to be that currentFileNameId 
            // and source_file_id had the same value, but this didn't allow us to support the header file unparsing.
            // Or perhaps it didn't allow the support of the optimization of the header file unparsing.
            // DQ (5/24/2005): Relaxed to handle compiler generated and transformed IR nodes
            // if ( isCompilerGenerated || isTransformation || currentFileNameId == fileIdForOriginOfCurrentLocatedNode )
            // if ( isCompilerGenerated || isTransformation || source_file_id == fileIdForOriginOfCurrentLocatedNode )
            // if ( source_file_id == fileIdForOriginOfCurrentLocatedNode )
               if ( source_file_id == currentLocNode_physical_file_id )
                  {
                 // DQ (11/3/2019): Check that the comment or CPP directive is from the same file as the locatedNode.
                 // A variation of this test might be required later, though we should only be attacheing comments and 
                 // CPP directives before possible transformations.
                 // if (currentLocNodePtr->get_file_info()->get_filename() != currentListOfAttributes->getFileName())

                 // int currentLocNode_physical_file_id = currentLocNodePtr->get_file_info()->get_physical_file_id();
                 // string currentLocNode_physical_filename_from_id = Sg_File_Info::getFilenameFromID(currentLocNode_physical_file_id);
#if 0
                    printf ("currentLocNode_physical_file_id = %d \n",currentLocNode_physical_file_id);
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
                 // col  = currentLocNodePtr->get_file_info()->get_col();
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
                          currentLocNodePtr,line,PreprocessingInfo::before, reset_start_index, 
                          currentListOfAttributes );

                 // save the previous node (in an accumulator attribute), but handle some nodes differently
                 // to avoid having comments attached to them since they are not unparsed directly.
                 // printf ("currentLocNodePtr = %p = %s \n",currentLocNodePtr,currentLocNodePtr->class_name().c_str());
                    setupPointerToPreviousNode(currentLocNodePtr);
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
     printf ("In AttachPreprocessingInfoTreeTrav::evaluateSynthesizedAttribute(): n = %p = %s \n",n,n->class_name().c_str());
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
#if 1
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

            // DQ (5/28/2019): debugging test2019_441.C.
            // printf ("Using OneBillion figure for lineOfClosingBrace \n");

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
            // Note that since this is for the original file, the list of attributes should already be in the map.
            // Note that values of currentFileNameId < 0 are for IR nodes that don't have a mapped source position
            // (e.g. compiler generated, unknown, etc.).
               if ( !(processAllIncludeFiles == false || ((currentFileNameId < 0) || (attributeMapForAllFiles.find(currentFileNameId) != attributeMapForAllFiles.end()))) )
                  {
                    std::cerr <<"node = (" <<stringifyVariantT(n->variantT(), "V_") <<"*)" <<n;
                    assert(isSgLocatedNode(n));
                    Sg_File_Info *info = isSgLocatedNode(n)->get_startOfConstruct();
                    assert(info);
                    std::cerr <<" at " <<info->get_filenameString() <<"[fileId=" <<currentFileNameId <<"]:" <<info->get_line() <<"." <<info->get_col() <<"\n";
#if 1
                 // This outputs too much data to be useful (must same the output to a file to figure out what is going on).
                    display("about to abort...");
#endif
                    printf ("processAllIncludeFiles = %s \n",processAllIncludeFiles ? "true" : "false");
                    printf ("currentFileNameId = %d \n",currentFileNameId);
                    printf ("currentFileName for currentFileNameId = %d = %s \n",currentFileNameId,Sg_File_Info::getFilenameFromID(currentFileNameId).c_str());
                    printf ("attributeMapForAllFiles.find(currentFileNameId) != attributeMapForAllFiles.end() = %s \n",attributeMapForAllFiles.find(currentFileNameId) != attributeMapForAllFiles.end() ? "true" : "false");
                  }

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
#if 0
                    printf ("Not supporting gathering of CPP directives and comments for this file currentFileNameId = %d \n",currentFileNameId);
#endif
                    return returnSynthesizeAttribute;
                  }

               if (previousLocatedNodeMap.find(currentFileNameId) == previousLocatedNodeMap.end())
                  {
#if 0
                    printf ("WARNING: Can't locate the entry for currentFileNameId = %d \n",currentFileNameId);
                    printf ("currentFileName for currentFileNameId = %d = %s \n",currentFileNameId,Sg_File_Info::getFilenameFromID(currentFileNameId).c_str());
#endif
                  }

            // DQ (9/22/2013): This is an error for the projects/haskellport tests (but only for the case of the headers included via -isystem, so for now ignore this case).
            // ROSE_ASSERT(previousLocatedNodeMap.find(currentFileNameId) != previousLocatedNodeMap.end());
               if (previousLocatedNodeMap.find(currentFileNameId) == previousLocatedNodeMap.end())
                  {
#if 0
                    printf ("ERROR: Can't locate the entry for currentFileNameId = %d (return returnSynthesizeAttribute) \n",currentFileNameId);
#endif
                    return returnSynthesizeAttribute;
                  }

               SgLocatedNode* previousLocNodePtr = previousLocatedNodeMap[currentFileNameId];

               switch (n->variantT())
                  {
                 // SgBinaryComposite need not be in the switch since we don't attach CPP directives or comments to it.
                    case V_SgBinaryComposite:
                        {
                          printf ("Error: SgBinaryComposite need not be in the switch since we don't attach CPP directives or comments to it ... \n");
                          ROSE_ASSERT(false);
                          break;
                        }

                 // I wanted to leave the SgFile case in the switch statement rather 
                 // than separating it out in a conditional statement at the top of the file.
                 // case V_SgFile:
                    case V_SgSourceFile:
                          {
#if 0
                            printf ("Case SgFile: See if we can find a better target to attach these comments than %s \n",previousLocNodePtr->sage_class_name());
#endif
                         // SgLocatedNode* targetNode = previousLocNodePtr;
                            ROSE_ASSERT(previousLocatedNodeMap.find(currentFileNameId) != previousLocatedNodeMap.end());
                            SgLocatedNode* targetNode = previousLocatedNodeMap[currentFileNameId];

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
                            ROSE_ASSERT(attributeMapForAllFiles.find(currentFileNameId) != attributeMapForAllFiles.end());
#if 0
                            printf ("$$$$$$$$$$$$$$$$$$$$$$$$ Setting attributeMapForAllFiles entry to NULL: currentFileNameId = %d \n",currentFileNameId);
#endif
                         // For now just reset the pointer to NULL, but later we might want to delete the lists (to avoid a memory leak).
                         // delete attributeMapForAllFiles[currentFileNameId];
                            attributeMapForAllFiles[currentFileNameId] = NULL;
#endif
                            currentListOfAttributes = NULL;

                         // Reset the pointer to the previous located node and the current list size
                            previousLocatedNodeMap[currentFileNameId] = NULL;
                            startIndexMap[currentFileNameId] = 0;

                         // DQ (12/19/2008): I think this should be true, but check it!
                            ROSE_ASSERT(previousLocatedNodeMap.size() == startIndexMap.size());
#if 0
                            printf ("In AttachPreprocessingInfoTreeTrav::evaluateSynthesizedAttribute(): case V_SgSourceFile: process statementsToInsertBefore \n");
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

#if 0
                            printf ("In AttachPreprocessingInfoTreeTrav::evaluateSynthesizedAttribute(): case V_SgSourceFile: process statementsToInsertAfter \n");
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
#if 1
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
                         // DQ (11/3/2019): Force processing to exist afer a selected header file is processed.
                               {
                              // SgSourceFile* sourceFile = isSgSourceFile(currentFilePtr);
                                 bool isHeaderFile = sourceFile->get_isHeaderFile();
#if 1
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
#if 1
                                     printf ("Exiting as a test! \n");
                                     ROSE_ASSERT(false);
#endif
                                    }
                               }
#endif


                            break;
                          }

                    // This case helps place the comment or directive relative
                    // to the closing brace of a SgBasicBlock.
                    case V_SgBasicBlock:
                          {
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
                         // DQ (3/18/2005): This is a more robust process (although it introduces a new location for a comment/directive)
                            bool reset_start_index = false;
                            iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                              ( basicBlock, lineOfClosingBrace, PreprocessingInfo::inside, reset_start_index, currentListOfAttributes );

                         // DQ (4/9/2005): We need to point to the SgBasicBlock and not the last return statement (I think)
                         // Reset the previousLocNodePtr to the current node so that all 
                         // PreprocessingInfo objects will be inserted relative to the 
                         // current node next time.
                         // previousLocNodePtr = basicBlock;
                            previousLocatedNodeMap[currentFileNameId] = basicBlock;
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
                            ROSE_ASSERT (locatedNode->get_endOfConstruct() != NULL);

                            SgAggregateInitializer* target = dynamic_cast<SgAggregateInitializer*>(n);
#if 0
                            printf ("Case SgAggregateInitializer: Calling iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber() using locatedNode->get_file_info()->get_file_id() = %d \n",
                                 locatedNode->get_file_info()->get_file_id());
#endif
#if 0
                            printf ("In case V_SgAggregateInitializer: calling iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber() \n");
#endif
                            bool reset_start_index = false;
                            iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                              ( target, lineOfClosingBrace, PreprocessingInfo::inside, reset_start_index, currentListOfAttributes );

                           previousLocatedNodeMap[currentFileNameId] = target;
                            break;
                          }

                 // DQ (12/29/2011): Adding support for template class declarations.
                    case V_SgTemplateClassDeclaration:

                    case V_SgClassDeclaration:
                          {
                            ROSE_ASSERT (locatedNode->get_endOfConstruct() != NULL);

                         // The following should always work since each statement is a located node
                            SgClassDeclaration* classDeclaration = dynamic_cast<SgClassDeclaration*>(n);
#if 0
                            printf ("In case V_SgClassDeclaration: calling iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber() \n");
#endif
                         // DQ (3/18/2005): This is a more robust process (although it introduces a new location for a comment/directive)
                            bool reset_start_index = false;
                            iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                              ( previousLocNodePtr, lineOfClosingBrace, PreprocessingInfo::after, reset_start_index,currentListOfAttributes );
                         // printf ("Adding comment/directive to base of class declaration \n");
                         // iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                         //    ( locatedNode, lineOfClosingBrace, PreprocessingInfo::inside );

                         // previousLocNodePtr = classDeclaration;
                            previousLocatedNodeMap[currentFileNameId] = classDeclaration;
                            break;
                          }

                 // GB (09/18/2007): Added support for preprocessing info inside typedef declarations (e.g. after the
                 // base type, which is what the previousLocNodePtr might point to).
                    case V_SgTypedefDeclaration:
                          {
                            ROSE_ASSERT(locatedNode->get_endOfConstruct() != NULL);

                            SgTypedefDeclaration *typedefDeclaration = isSgTypedefDeclaration(n);
                            ROSE_ASSERT(typedefDeclaration != NULL);
#if 0
                            printf ("In case V_SgTypedefDeclaration: calling iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber() \n");
#endif
                            bool reset_start_index = false;
                            iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                              ( previousLocNodePtr, lineOfClosingBrace, PreprocessingInfo::after, reset_start_index,currentListOfAttributes );

                         // previousLocNodePtr = typedefDeclaration;
                            previousLocatedNodeMap[currentFileNameId] = typedefDeclaration;
                            break;
                          }

                 // DQ (12/29/2011): Adding support for template variable declarations.
                    case V_SgTemplateVariableDeclaration:

                 // GB (09/19/2007): Added support for preprocessing info inside variable declarations (e.g. after the
                 // base type, which is what the previousLocNodePtr might point to).
                    case V_SgVariableDeclaration:
                          {
                            ROSE_ASSERT(locatedNode->get_endOfConstruct() != NULL);

                            SgVariableDeclaration *variableDeclaration = isSgVariableDeclaration(n);
                            ROSE_ASSERT(variableDeclaration != NULL);
#if 0
                            printf ("In AttachPreprocessingInfoTreeTrav::evaluateSynthesizedAttribute(): variableDeclaration = %p \n",variableDeclaration);
#endif
#if 0
                            printf ("In case V_SgVariableDeclaration: calling iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber() \n");
#endif
                            bool reset_start_index = false;
                            iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                              ( previousLocNodePtr, lineOfClosingBrace, PreprocessingInfo::after, reset_start_index,currentListOfAttributes );

                         // previousLocNodePtr = variableDeclaration;
                            previousLocatedNodeMap[currentFileNameId] = variableDeclaration;
                            break;
                          }

                 // DQ (10/25/2012): Added new case.  I expect this might be important for test2012_78.c
                    case V_SgInitializedName:
                          {
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
                            bool reset_start_index = false;
                            iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                              ( previousLocNodePtr, lineOfClosingBrace, PreprocessingInfo::after, reset_start_index, currentListOfAttributes );

                            previousLocatedNodeMap[currentFileNameId] = initializedName;
                            break;
                          }

                    case V_SgClassDefinition:
                          {
                            ROSE_ASSERT (locatedNode->get_endOfConstruct() != NULL);

                         // DQ (3/19/2005): This is a more robust process (although it introduces a new location for a comment/directive)
                         // iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                         //    ( previousLocNodePtr, lineOfClosingBrace, PreprocessingInfo::after );
                         // printf ("Adding comment/directive to base of class definition \n");
#if 0
                            printf ("In case V_SgClassDefinition: calling iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber() \n");
#endif
                            bool reset_start_index = false;
                            iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                              ( locatedNode, lineOfClosingBrace, PreprocessingInfo::inside, reset_start_index,currentListOfAttributes );

                         // previousLocNodePtr = locatedNode;
                         // previousLocatedNodeMap[currentFileNameId] = locatedNode;
                            break;
                          }

                    case V_SgEnumDeclaration:
                          {
                            ROSE_ASSERT (locatedNode->get_endOfConstruct() != NULL);

                         // The following should always work since each statement is a located node
                            SgEnumDeclaration* enumDeclaration = dynamic_cast<SgEnumDeclaration*>(n);

                         // DQ (3/18/2005): This is a more robust process (although it introduces a new location for a comment/directive)
                         // iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                         //    ( previousLocNodePtr, lineOfClosingBrace, PreprocessingInfo::after );
                         // printf ("Adding comment/directive to base of enum declaration \n");
                            bool reset_start_index = false;
                            iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                              ( locatedNode, lineOfClosingBrace, PreprocessingInfo::inside, reset_start_index,currentListOfAttributes );

                         // previousLocNodePtr = enumDeclaration;
                            previousLocatedNodeMap[currentFileNameId] = enumDeclaration;
                            break;
                          }

                    // DQ (5/3/2004): Added support for namespaces
                    case V_SgNamespaceDeclarationStatement:
                          {
                            ROSE_ASSERT (locatedNode->get_endOfConstruct() != NULL);

                         // The following should always work since each statement is a located node
                            SgNamespaceDeclarationStatement* namespaceDeclaration = 
                              dynamic_cast<SgNamespaceDeclarationStatement*>(n);

                            bool reset_start_index = false;
                            iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                              ( previousLocNodePtr, lineOfClosingBrace, PreprocessingInfo::after, reset_start_index,currentListOfAttributes );

                         // previousLocNodePtr = namespaceDeclaration;
                            previousLocatedNodeMap[currentFileNameId] = namespaceDeclaration;
                            break;
                          }

                    // DQ (5/3/2004): Added support for namespaces
                    case V_SgNamespaceDefinitionStatement:
                          {
                            ROSE_ASSERT (locatedNode->get_endOfConstruct() != NULL);

                         // The following should always work since each statement is a located node
                            SgNamespaceDefinitionStatement* namespaceDefinition = 
                              dynamic_cast<SgNamespaceDefinitionStatement*>(n);

                         // DQ (3/18/2005): This is a more robust process (although it introduces a new location for a comment/directive)
                         // iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                         //    ( previousLocNodePtr, lineOfClosingBrace, PreprocessingInfo::after );
                         // printf ("Adding comment/directive to base of namespace definition \n");
                            bool reset_start_index = false;
                            iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                              ( locatedNode, lineOfClosingBrace, PreprocessingInfo::inside, reset_start_index,currentListOfAttributes );

                         // previousLocNodePtr = namespaceDefinition;
                            previousLocatedNodeMap[currentFileNameId] = namespaceDefinition;
                            break;
                          }

                    // DQ (4/9/2005): Added support for templates instaiations which are compiler generated
                    //                but OK to attach comments to them (just not inside them!).
                    case V_SgTemplateInstantiationMemberFunctionDecl:
                          {
                            ROSE_ASSERT (locatedNode->get_endOfConstruct() != NULL);
                         // printf ("Found a SgTemplateInstantiationMemberFunctionDecl but only record it as a previousLocNodePtr \n");

                         // DQ (3/11/2012): Added recursive call to insert comments.
                            bool reset_start_index = false;
                            iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                              ( locatedNode, lineOfClosingBrace, PreprocessingInfo::inside, reset_start_index,currentListOfAttributes );

                         // previousLocNodePtr = locatedNode;
                            previousLocatedNodeMap[currentFileNameId] = locatedNode;

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
                                  //  we don't want to attach anything after an ending initialized name,
                                  //  So we give a chance to the init name's ancestor a chance. 
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
                         printf ("In case V_xxx: calling iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber(): n = %p = %s \n",n,n->class_name().c_str());
#endif
                      // DQ (3/11/2012): Added recursive call to insert comments.
                         bool reset_start_index = false;
                         iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber( locatedNode, lineOfClosingBrace, PreprocessingInfo::inside, reset_start_index,currentListOfAttributes );

                      // previousLocNodePtr = locatedNode;
                         previousLocatedNodeMap[currentFileNameId] = locatedNode;

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
                       }
                  }
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
#endif

     return returnSynthesizeAttribute;
   }

// ifndef  CXX_IS_ROSE_CODE_GENERATION
// #endif 
