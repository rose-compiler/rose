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

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;
using namespace rose;

// Debug flag
#define DEBUG_ATTACH_PREPROCESSING_INFO 0


//It is needed because otherwise, the default destructor breaks something.

AttachPreprocessingInfoTreeTrav::~AttachPreprocessingInfoTreeTrav() {
    //do nothing
}


// DQ (11/30/2008): Refactored this code out of the simpler function to isolate 
// the Wave specific handling.  


AttachPreprocessingInfoTreeTrav::AttachPreprocessingInfoTreeTrav( SgSourceFile* file, bool includeDirectivesAndCommentsFromAllFiles )
   {
// #ifndef  CXX_IS_ROSE_CODE_GENERATION
  // previousLocNodePtr            = NULL;
  // currentListOfAttributes       = NULL;
  // sizeOfCurrentListOfAttributes = 0;
  // currentFileName               = NULL;
  // currentMapOfAttributes        = NULL;
     use_Wave                      = file->get_wave();

     //Wave will get all Preprocessor Diretives by default and it is therefore reasonable
     //that it will attach all
#if 0
     if(use_Wave)
       processAllIncludeFiles = true;
     else
#endif
       processAllIncludeFiles        = includeDirectivesAndCommentsFromAllFiles;


  // start_index                   = 0;

     sourceFile = file;
// #endif
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
  // PreprocessingInfo objects as separate IR nodes and leave PreprocessingInfo objects that are comments inserts
  // as attributes.  Note that attributes imply PreprocessingInfo specific atrributes and not the more general 
  // mechanism available in ROSE for user defined attributes to be saved into the AST.

     ROSE_ASSERT(currentListOfAttributes != NULL);

#if 0
  // Debugging information...
     printf ("In iterateOverListAndInsertPrev... locatedNode = %s lineNumber = %d location = %s \n",locatedNode->class_name().c_str(),lineNumber,PreprocessingInfo::relativePositionName(location).c_str());
  // if ( dynamic_cast<SgLocatedNode*>(locatedNode) != NULL )
  // if ( isSgLocatedNode(locatedNode) != NULL )
     if ( locatedNode != NULL )
        {
       // printf ("starting line number = %d \n",locatedNode->get_startOfConstruct()->get_line());
          printf ("starting line number = %d \n",locatedNode->get_startOfConstruct()->get_physical_line());
          if (locatedNode->get_endOfConstruct() != NULL)
             {
            // printf ("ending line number   = %d \n",locatedNode->get_endOfConstruct()->get_line());
               printf ("ending line number   = %d \n",locatedNode->get_endOfConstruct()->get_physical_line());
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
          int line        = locatedNode->get_startOfConstruct()->get_physical_line();
          int col         = locatedNode->get_startOfConstruct()->get_col();
       // int ending_line = locatedNode->get_endOfConstruct()->get_line();
          int ending_line = locatedNode->get_endOfConstruct()->get_physical_line();
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
                         locatedFileInfo->get_physical_file_id();
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
          list<pair<SgIncludeDirectiveStatement*, SgStatement*> > localStatementsToInsertAfter;
          for ( int i = start_index; i < sizeOfCurrentListOfAttributes; i++ )
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
             {
               PreprocessingInfo *currentPreprocessingInfoPtr = (*currentListOfAttributes)[i];
#if 0
               if ( currentPreprocessingInfoPtr != NULL )
                    printf ("currentPreprocessingInfoPtr->getLineNumber() = %d lineNumber = %d \n",currentPreprocessingInfoPtr->getLineNumber(),lineNumber);
#endif
#if 0
               printf ("currentPreprocessingInfoPtr->getLineNumber() = %d lineNumber = %d internalString = %s \n",currentPreprocessingInfoPtr->getLineNumber(),lineNumber,currentPreprocessingInfoPtr->getString().c_str());
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
                 // printf ("Warning: Why are we searching this list of PreprocessingInfo beyond the line number of the current statement (using break) \n");

                 // DQ (12/23/2008): I don't like the design which forces an exit from the loop here, but this is the most robust implementation so far.
                    break;
                  }
#endif
            // bool attachCommentOrDirective = (currentPreprocessingInfoPtr != NULL) && (currentPreprocessingInfoPtr->getLineNumber() <= lineNumber);
               bool attachCommentOrDirective = (currentPreprocessingInfoLineNumber <= lineNumber);

               if ( attachCommentOrDirective == true )
                  {
#if DEBUG_ATTACH_PREPROCESSING_INFO
                    printf ("Attaching \"%s\" (from line# %d) to %s locatedNode = %p = %s = %s at line %d position = %s \n",
                         currentPreprocessingInfoPtr->getString().c_str(),
                         currentPreprocessingInfoPtr->getLineNumber(),
                         (locatedNode->get_file_info()->isCompilerGenerated() == true) ? "compiler-generated" : "non-compiler-generated",
                         locatedNode,
                         locatedNode->class_name().c_str(),SageInterface::get_name(locatedNode).c_str(),
                      // (locatedNode->get_file_info()->isCompilerGenerated() == true) ? -1 : locatedNode->get_file_info()->get_line());
                         (locatedNode->get_file_info()->isCompilerGenerated() == true) ? -1 : locatedNode->get_file_info()->get_physical_line(),
                         PreprocessingInfo::relativePositionName(location).c_str());
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

                 // negara1 (08/05/2011): If currentPreprocessingInfoPtr is an include directive, get the included file.
                 // If the included file exists, append all its trailing preprocessor directives to its last node and reset its 
                 // start index to the first preprocessor directive.
                 // Proceed only if header files unparsing is enabled.
                 if (sourceFile->get_unparseHeaderFiles())
                    {
                       if (currentPreprocessingInfoPtr->getTypeOfDirective() == PreprocessingInfo::CpreprocessorIncludeDeclaration)
                          {
                             string includedFileName = sourceFile->get_project()->findIncludedFile(currentPreprocessingInfoPtr);
                             if (includedFileName.length() > 0)
                                {
                                   // found the included file
                                   const int OneBillion = 1000000000;
                                   int fileNameId = Sg_File_Info::getIDFromFilename(includedFileName);

                                   // A header file might not be present in the map at this point if it contains only preprocessor directives and comments.
                                   if (fileNameId < 0)
                                      {
                                         fileNameId = Sg_File_Info::addFilenameToMap(includedFileName);
                                      }
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
                                   includeDirectiveStatement -> set_file_info(locatedNode -> get_file_info());
                                   includeDirectiveStatement -> set_directiveString(currentPreprocessingInfoPtr -> getString()); //Set, but not used in the current implementation.
                                   SgHeaderFileBody* headerFileBody = new SgHeaderFileBody();
                                   headerFileBody -> set_file_info(new Sg_File_Info(includedFileName));
                                   headerFileBody -> set_parent(includeDirectiveStatement);
                                   includeDirectiveStatement -> set_headerFileBody(headerFileBody);
                                   if (location == PreprocessingInfo::before)
                                      {
                                         statementsToInsertBefore.push_back(pair<SgIncludeDirectiveStatement*, SgStatement*>(includeDirectiveStatement, locatedStatement));
                                      }
                                   else 
                                      {
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
                                   bool reset_start_index = false;
                                   iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber (targetNode, OneBillion, PreprocessingInfo::after, reset_start_index, getListOfAttributes(fileNameId));
                                   // Reset the pointer to the previous located node and reset the start index.
                                   previousLocatedNodeMap[fileNameId] = NULL;
                                   startIndexMap[fileNameId] = 0;
                                }
                          }
                    }
                    // Sara Royuela (Nov 9th, 2012)
                    // When collecting comment and directives, 
                    // we attach preprocessed info comming from headers without any SgLocated node to the current SgFile
                    else if( sourceFile->get_collectAllCommentsAndDirectives() )
                    {
                       if (currentPreprocessingInfoPtr -> getTypeOfDirective() == PreprocessingInfo::CpreprocessorIncludeDeclaration) {
                          string includedFileName = sourceFile -> get_project() -> findIncludedFile(currentPreprocessingInfoPtr);
                          if( includedFileName.size() > 0 )
                             {
                                int fileNameId = Sg_File_Info::getIDFromFilename(includedFileName);
                                // We only add the preprocessor directives and comments to file only in the case the file only contains preprocessor information
                                // When this occurs, the file is not present in the map at this point 
                                if (fileNameId < 0) { 
                                   fileNameId = Sg_File_Info::addFilenameToMap(includedFileName);
                                   ROSEAttributesList* headerAttributes = getListOfAttributes(fileNameId);
                                   if( headerAttributes->size() )
                                      {
                                         string filename = sourceFile->get_sourceFileNameWithPath();
                                         sourceFile->get_preprocessorDirectivesAndCommentsList()->addList(filename, headerAttributes);
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
     
       // negara1 (08/15/2011): After the iteration is over, add local list of statements to "insert after" to the global list. Two lists are used in order to
       // insert in front of the local list and then, insert the local list in front of the global list such that we preserve the relative order of inserted nodes. 
          statementsToInsertAfter.insert(statementsToInsertAfter.begin(), localStatementsToInsertAfter.begin(), localStatementsToInsertAfter.end());
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
                         locatedFileInfo->get_physical_file_id();
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
     ROSEAttributesList* returnListOfAttributes = new ROSEAttributesList();

  // Build an empty list while we skip the translation of tokens
  // returnListOfAttributes = new ROSEAttributesList();

     ROSE_ASSERT(sourceFile != NULL);
     string fileNameForTokenStream = fileNameForDirectivesAndComments;

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

     if (use_Wave == false)
        {
       // std::cerr << "Not using wave" << std::endl;
       // DQ (4/12/2007): Introduce tracking of performance of ROSE.
          TimingPerformance timer ("AST evaluateInheritedAttribute (use_Wave == false):");


          //AS(4/3/09): FIXME: We are doing this quick fix because the fileNameForDirectivesAndComments is
          //incorrect for Fortran
          //PC(08/17/2009): Now conditional on the output language, otherwise breaks -rose:collectAllCommentsAndDirectives
          if (sourceFile->get_outputLanguage() == SgFile::e_Fortran_output_language)
             {
               fileNameForDirectivesAndComments = sourceFile->get_sourceFileNameWithPath();
               fileNameForTokenStream           = fileNameForDirectivesAndComments;
             }

          if (sourceFile->get_Fortran_only() == true)
             {
            // For Fortran CPP code you need to preprocess the code into an intermediate file in order to pass it through
            // the Fortran frontend. This is because for Fortan everything is ONE file. 
               if (sourceFile->get_requires_C_preprocessor() == true)
                  {
                    fileNameForDirectivesAndComments = sourceFile->generate_C_preprocessor_intermediate_filename(fileNameForDirectivesAndComments);
                  }
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
#ifdef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
                    lex_token_stream = getFortranFreeFormatPreprocessorDirectives( fileNameForTokenStream );
#endif
                    ROSE_ASSERT(lex_token_stream != NULL);

                 // Attach the token stream to the AST
                    returnListOfAttributes->set_rawTokenStream(lex_token_stream);
                    ROSE_ASSERT(returnListOfAttributes->get_rawTokenStream() != NULL);

                 // printf ("Fortran Token List Size: returnListOfAttributes->get_rawTokenStream()->size() = %" PRIuPTR " \n",returnListOfAttributes->get_rawTokenStream()->size());

                 // DQ (11/23/2008): This is the new support to collect CPP directives and comments from Fortran applications.
                 // printf ("Calling collectPreprocessorDirectivesAndCommentsForAST() to collect CPP directives for fileNameForDirectivesAndComments = %s \n",fileNameForDirectivesAndComments.c_str());
                    returnListOfAttributes->collectPreprocessorDirectivesAndCommentsForAST(fileNameForDirectivesAndComments,ROSEAttributesList::e_Fortran9x_language);

#if 0
                    printf ("Done with processing of separate lexical pass to gather CPP directives \n");
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

#if 1
            // DQ (11/23/2008): This is part of CPP handling for Fortran, but tested on C and C++ codes additionally, (it is redundant for C and C++).
            // This is a way of testing the extraction of CPP directives (on C and C++ codes, so that it is more agressively tested).
            // Since this is a redundant test, it can be removed in later development (its use is only a performance issue).
            // returnListOfAttributes = new ROSEAttributesList();

            // This call is just a test, this function is defined for use on Fortran.  For C and C++ we have alternative methods to extract the CPP directives and comments.
#if 0
               printf ("Call collectPreprocessorDirectivesAndCommentsForAST to test C and C++ preprocessor directive collection \n");
#endif
               returnListOfAttributes->collectPreprocessorDirectivesAndCommentsForAST(fileNameForDirectivesAndComments,ROSEAttributesList::e_C_language);
#if 0
               printf ("DONE: Call collectPreprocessorDirectivesAndCommentsForAST to test C and C++ preprocessor directive collection \n");
#endif
#endif

            // This function has been modified to clear any existing list of PreprocessingInfo*
            // objects (so that we can test the function: collectPreprocessorDirectivesAndCommentsForAST()).
            // returnListOfAttributes = getPreprocessorDirectives( Sg_File_Info::getFilenameFromID(currentFileNameId) );
#if 0
               printf ("Calling lex or wave based mechanism for collecting CPP directives, comments, and token stream \n");
#endif
               returnListOfAttributes = getPreprocessorDirectives(fileNameForDirectivesAndComments);
#if 0
               printf ("DONE: Calling lex or wave based mechanism for collecting CPP directives, comments, and token stream \n");
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

          //Copy the ROSEAttributesList from the global mapFilenameToAttributes as the elments that are attached to
          //the AST from the ROSEAttributesList is set to NULL by the attachment process 

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
#endif
#if 0
          printf ("In AttachPreprocessingInfoTreeTrav::buildCommentAndCppDirectiveList(): Output token list (number of CPP directives and comments = %d): \n",returnListOfAttributes->size());
          printf ("In AttachPreprocessingInfoTreeTrav::buildCommentAndCppDirectiveList(): Output token list (number of tokens = %" PRIuPTR "): \n",tokenList.size());
#endif
#if 0
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
               int sourceFileNameId = (sourceFile->get_requires_C_preprocessor() == true) ? 
                                  Sg_File_Info::getIDFromFilename(sourceFile->generate_C_preprocessor_intermediate_filename(sourceFileInfo->get_filename())) : 
                                  sourceFileInfo->get_physical_file_id();
#endif

               bool skipProcessFile = (processAllIncludeFiles == false) && (currentFileNameId != sourceFileNameId);
#if 0
               printf ("In AttachPreprocessingInfoTreeTrav::getListOfAttributes(): currentFileNameId = %d sourceFileNameId = %d skipProcessFile = %s \n",currentFileNameId,sourceFileNameId,skipProcessFile ? "true" : "false");
#endif
               if (skipProcessFile == false)
                  {
#if 0
                    printf ("In AttachPreprocessingInfoTreeTrav::getListOfAttributes(): currentFileNameId = %d sourceFileNameId = %d Sg_File_Info::getFilenameFromID(currentFileNameId) = %s \n",
                         currentFileNameId,sourceFileNameId,Sg_File_Info::getFilenameFromID(currentFileNameId).c_str());
#endif
                    attributeMapForAllFiles[currentFileNameId] = buildCommentAndCppDirectiveList(use_Wave, Sg_File_Info::getFilenameFromID(currentFileNameId) );

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
     printf ("In AttachPreprocessingInfoTreeTrav::getListOfAttributes(): currentListOfAttributes = %p currentListOfAttributes->size() = %d \n",currentListOfAttributes,currentListOfAttributes != NULL ? currentListOfAttributes->size() : -1);
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

#if 0
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
#if DEBUG_ATTACH_PREPROCESSING_INFO
          printf ("Returning without further processing if we are a part of a template declaration or template instantiation declaration \n");
#endif
          return inheritedAttribute;
        }

  // Check if current AST node is an SgFile object
     SgFile* currentFilePtr = isSgFile(n);
     if (currentFilePtr != NULL)
        {
       // Current AST node is an SgFile object, generate the corresponding list of attributes

#if DEBUG_ATTACH_PREPROCESSING_INFO
          printf ("=== Visiting SgSourceFile node and building current list of attributes === \n");
#endif

       // This entry should not be present, so generate the list.
       // If this is a preprocessed file then change the name so that we generate the correct list for the correct file.
       // int currentFileNameId = currentFilePtr->get_file_info()->get_file_id();
          Sg_File_Info* currentFileInfo = currentFilePtr->get_file_info();
          ROSE_ASSERT(currentFileInfo != NULL);
#if 0
          printf ("(SgSourceFile) currentFilePtr->get_requires_C_preprocessor() = %s \n",currentFilePtr->get_requires_C_preprocessor() == true ? "true" : "false");
          printf ("(SgSourceFile) sourceFile->get_file_info()->get_filename() = %s \n",sourceFile->get_file_info()->get_filename());
#endif
          ROSE_ASSERT(sourceFile == currentFilePtr);
#if 0
          printf ("(SgSourceFile) currentFilePtr->generate_C_preprocessor_intermediate_filename(sourceFile->get_file_info()->get_filename()) = %s \n",currentFilePtr->generate_C_preprocessor_intermediate_filename(sourceFile->get_file_info()->get_filename()).c_str());
#endif
#if 0
          int currentFileNameId = (currentFilePtr->get_requires_C_preprocessor() == true) ? 
                                  Sg_File_Info::getIDFromFilename(currentFilePtr->generate_C_preprocessor_intermediate_filename(sourceFile->get_file_info()->get_filename())) : 
                                  currentFileInfo->get_file_id();
#else
          int currentFileNameId = (currentFilePtr->get_requires_C_preprocessor() == true) ? 
                                  Sg_File_Info::getIDFromFilename(currentFilePtr->generate_C_preprocessor_intermediate_filename(sourceFile->get_file_info()->get_filename())) : 
                                  currentFileInfo->get_physical_file_id();
#endif
#if 0
          printf ("(SgSourceFile) currentFileNameId = %d \n",currentFileNameId);
          printf ("(SgSourceFile) currentFileName for currentFileNameId = %s \n",Sg_File_Info::getFilenameFromID(currentFileNameId).c_str());
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

       // This will cause the CPP directives and comments list to be generated for the source file.
          ROSEAttributesList* currentListOfAttributes = getListOfAttributes(currentFileNameId);
          ROSE_ASSERT(currentListOfAttributes != NULL);


       // *************************************************
       // ** Save the ROSEAttributesList into the SgFile **
       // *************************************************

       // We need to extract the token list and save that in the current source file (currentFilePtr).
       // SgSourceFile* sourceFile = isSgSourceFile(currentFilePtr);
       // ROSE_ASSERT(sourceFile != NULL);

       // DQ (10/21/2013): This was reported as an error for test2008_01.F (fortran tests).
       // ROSE_ASSERT(currentFilePtr->get_preprocessorDirectivesAndCommentsList() != NULL);
          ROSEAttributesListContainerPtr filePreprocInfo = currentFilePtr->get_preprocessorDirectivesAndCommentsList();
          if (filePreprocInfo != NULL)
             {
               ROSE_ASSERT(filePreprocInfo->getList().empty() == true);
#if 0
               printf ("Put the ROSEAttributesList into the ROSEAttributesListContainer (an stl map) \n");
#endif
            // Put the ROSEAttributesList into the ROSEAttributesListContainer (an stl map)
               filePreprocInfo->getList()[sourceFile->get_file_info()->get_filename()] = currentListOfAttributes;
#if 0
               printf ("In AttachPreprocessingInfoTreeTrav::evaluateInheritedAttribute(): filePreprocInfo->getList().size() = %" PRIuPTR " \n",filePreprocInfo->getList().size());
#endif
             }
            else
             {
            // DQ (10/21/2013): I am not clear if this shuld be a warning, but I disabled the assertion above (required for 
            // Fortran or perhaps masking another issue).  After more investigation, I think this is OK to comment out.
#if 0
               printf ("WARNING: currentFilePtr->get_preprocessorDirectivesAndCommentsList() == NULL \n");
#endif
             }
#if 0
          printf ("Exiting as a test so that we can get the token information attached to the SgSourceFile \n");
          ROSE_ASSERT(false);
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
       if (statement != NULL || i_name != NULL || a_initor != NULL)
       {
          SgLocatedNode* currentLocNodePtr = NULL;
          int line = 0;
          int col  = 0;

       // The following should always work since each statement is a located node
          currentLocNodePtr = dynamic_cast<SgLocatedNode*>(n);
          ROSE_ASSERT(currentLocNodePtr != NULL);

       // Attach the comments only to nodes from the same file
          ROSE_ASSERT(currentLocNodePtr->get_file_info() != NULL);
       // int currentFileNameId = currentLocNodePtr->get_file_info()->get_file_id();
          Sg_File_Info* currentFileInfo = currentLocNodePtr->get_file_info();
          ROSE_ASSERT(currentFileInfo != NULL);
#if 0
          int currentFileNameId = (sourceFile->get_requires_C_preprocessor() == true) ? 
                                   Sg_File_Info::getIDFromFilename(sourceFile->generate_C_preprocessor_intermediate_filename(sourceFile->get_file_info()->get_filename())) : 
                                   currentFileInfo->get_file_id();
#else
          int currentFileNameId = (sourceFile->get_requires_C_preprocessor() == true) ? 
                                   Sg_File_Info::getIDFromFilename(sourceFile->generate_C_preprocessor_intermediate_filename(sourceFile->get_file_info()->get_filename())) : 
                                   currentFileInfo->get_physical_file_id();
#endif

#if 0
           printf ("(SgStatement) currentFileNameId = %d \n",currentFileNameId);
           printf ("(SgStatement) currentFileName for currentFileNameId = %s \n",Sg_File_Info::getFilenameFromID(currentFileNameId).c_str());
#endif
           
           ROSEAttributesList* currentListOfAttributes = getListOfAttributes(currentFileNameId);

           //printf ("currentListOfAttributes = %p \n",currentListOfAttributes);

           //If currentListOfAttributes == NULL then this was not an IR node from a file where we wanted 
           //to include CPP directives and comments.
           if (currentListOfAttributes != NULL)
              {
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

                  // DQ (9/20/2013): Fixing up references to get_file_id() to use get_physical_file_id().
                  // fileIdForOriginOfCurrentLocatedNode = (sourceFile->get_requires_C_preprocessor() == true) ? 
                  //                         Sg_File_Info::getIDFromFilename(sourceFile->generate_C_preprocessor_intermediate_filename(sourceFile->get_file_info()->get_filename())) : 
                  //                         currentFileInfo->get_file_id();
                     fileIdForOriginOfCurrentLocatedNode = (sourceFile->get_requires_C_preprocessor() == true) ? 
                                             Sg_File_Info::getIDFromFilename(sourceFile->generate_C_preprocessor_intermediate_filename(sourceFile->get_file_info()->get_filename())) : 
                                             currentFileInfo->get_physical_file_id();
                  }
                
#if 0
               printf ("evaluateInheritedAttribute: isCompilerGenerated = %s isTransformation = %s fileIdForOriginOfCurrentLocatedNode = %d \n",
                    isCompilerGenerated ? "true" : "false",isTransformation ? "true" : "false",fileIdForOriginOfCurrentLocatedNode);
#endif
            // DQ (5/24/2005): Relaxed to handle compiler generated and transformed IR nodes
               if ( isCompilerGenerated || isTransformation || currentFileNameId == fileIdForOriginOfCurrentLocatedNode )
                  {
                 // Current node belongs to the file the name of which has been specified
                 // on the command line
                 // line = currentLocNodePtr->get_file_info()->get_line();
                    line = currentLocNodePtr->get_file_info()->get_physical_line();
                    col  = currentLocNodePtr->get_file_info()->get_col();
#if 0
                    printf ("Insert any comment before %p = %s = %s (compilerGenerate=%s) at line = %d col = %d \n",
                          currentLocNodePtr,currentLocNodePtr->class_name().c_str(),SageInterface::get_name(currentLocNodePtr).c_str(),
                          isCompilerGenerated ? "true" : "false", line, col);
#endif
#if 0
                    printf ("In AttachPreprocessingInfoTreeTrav::evaluateInheritedAttribute() calling iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber(): n->class_name() = %s \n",n->class_name().c_str());
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
                  }
#if 0
             // Debugging output
                 else
                  {
                    printf ("Node belongs to a different file: \n");
                  }
#endif
             } // end if current list of attribute is not empty
        } // end if statement or init name

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
  // printf ("In AttachPreprocessingInfoTreeTrav::evaluateInheritedAttribute(): n = %p = %s \n",n,n->class_name().c_str());
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
#if DEBUG_ATTACH_PREPROCESSING_INFO
          printf ("Returning without further processing if we are a part of a template declaration n = %p = %s \n",n,n->class_name().c_str());
#endif
          return returnSynthesizeAttribute;
        }

  // DQ (3/4/2016): Klocworks reports a problem with "isSgClassDeclaration(n)->get_endOfConstruct() != NULL".
  // These used to be a problem, so we can continue to test these specific cases.
  // ROSE_ASSERT (isSgCaseOptionStmt(n)   == NULL || isSgCaseOptionStmt(n)->get_body()             != NULL);
     SgCaseOptionStmt* caseOptionStm = isSgCaseOptionStmt(n);
     ROSE_ASSERT (caseOptionStm == NULL || caseOptionStm->get_body() != NULL);

  // DQ (3/4/2016): Klocworks reports a problem with "isSgClassDeclaration(n)->get_endOfConstruct() != NULL".
  // ROSE_ASSERT (isSgClassDeclaration(n) == NULL || isSgClassDeclaration(n)->get_endOfConstruct() != NULL);
  // ROSE_ASSERT (isSgClassDeclaration(n) == NULL || (isSgClassDeclaration(n) != NULL && isSgClassDeclaration(n)->get_endOfConstruct() != NULL) );
     SgClassDeclaration* classDeclaration = isSgClassDeclaration(n);
     ROSE_ASSERT (classDeclaration == NULL || classDeclaration->get_endOfConstruct() != NULL);

  // Only process SgLocatedNode object and the SgFile object
  // SgFile* fileNode           = dynamic_cast<SgFile*>(n);
  // SgLocatedNode* locatedNode = dynamic_cast<SgLocatedNode*>(n);
     SgFile* fileNode           = isSgFile(n);
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
               currentFileNameId = locatedNode->get_file_info()->get_physical_file_id();
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
                                   currentFileInfo->get_physical_file_id();
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
               ROSE_ASSERT(locatedNode->get_file_info()->get_physical_file_id() == currentFileNameId);
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
                    fileIdForOriginOfCurrentLocatedNode = locatedNode->get_file_info()->get_physical_file_id();
                  }

               if (locatedNode->get_endOfConstruct() != NULL)
                  {
                    ROSE_ASSERT (locatedNode->get_endOfConstruct() != NULL);
                 // lineOfClosingBrace = locatedNode->get_endOfConstruct()->get_line();
                    lineOfClosingBrace = locatedNode->get_endOfConstruct()->get_physical_line();
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
                                   currentFileInfo->get_physical_file_id();
#endif

            // Use one billion as the max number of lines in a file
               const int OneBillion = 1000000000;

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
                    printf ("WARNING: Can't locate the entry for currentFileNameId = %d \n",currentFileNameId);
                  }

            // DQ (9/22/2013): This is an error for the projects/haskellport tests (but only for the case of the headers included via -isystem, so for now ignore this case).
            // ROSE_ASSERT(previousLocatedNodeMap.find(currentFileNameId) != previousLocatedNodeMap.end());
               if (previousLocatedNodeMap.find(currentFileNameId) == previousLocatedNodeMap.end())
                  {
                    printf ("ERROR: Can't locate the entry for currentFileNameId = %d (return returnSynthesizeAttribute) \n",currentFileNameId);
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
                         // printf ("Case SgFile: See if we can find a better target to attach these comments than %s \n",
                         //      previousLocNodePtr->sage_class_name());

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
                            if (targetNode->get_file_info()->get_physical_file_id() < 0)
                               {
#if 0
                                 printf ("Error: we should not be calling iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber() using targetNode->get_file_info()->get_file_id()          = %d \n",targetNode->get_file_info()->get_file_id());
#endif
#if 0
                                 printf ("Error: we should not be calling iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber() using targetNode->get_file_info()->get_physical_file_id() = %d \n",targetNode->get_file_info()->get_physical_file_id());
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

                         // negara1 (08/12/2011): We reached the last AST node, so its safe to insert nodes for header files bodies.
                            for (list<pair<SgIncludeDirectiveStatement*, SgStatement*> >::const_iterator it = statementsToInsertBefore.begin(); it != statementsToInsertBefore.end(); it++) 
                               {
                                 ROSE_ASSERT(it->second != NULL);
                                 printf ("Target it->second = %p = %s \n",it->second,it->second->class_name().c_str());
                                 it->second->get_file_info()->display("it->second: debug");

                                 ROSE_ASSERT(it->first != NULL);
                                 printf ("Target it->first = %p = %s \n",it->first,it->first->class_name().c_str());
                                 it->first->get_file_info()->display("it->first: debug");

                                 SageInterface::insertStatementBefore(it->second, it->first, false);
                               }
                            for (list<pair<SgIncludeDirectiveStatement*, SgStatement*> >::const_iterator it = statementsToInsertAfter.begin(); it != statementsToInsertAfter.end(); it++) {
                                SgClassDefinition* classDefinition = isSgClassDefinition(it -> second);
                                if (classDefinition != NULL) {
                                    //Since the parent of SgClassDefinition is SgClassDeclaration, whose implementation for child insertion is not provided, insert after the
                                    //last statement of SgClassDefinition instead.
                                    SgDeclarationStatement* lastMember = (classDefinition -> get_members()).back();
                                    SageInterface::insertStatementAfter(lastMember, it -> first, false);
                                } else {
                                    SgBasicBlock* basicBlock = isSgBasicBlock(it -> second);
                                    if (basicBlock != NULL) {
                                        //Do not insert after a basic block, but rather insert as the last statement of the basic block.
                                        SageInterface::insertStatementAfter(basicBlock -> lastStatement(), it -> first, false);
                                    } else {
                                        SageInterface::insertStatementAfter(it -> second, it -> first, false);
                                    }
                                }
                            }
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

     return returnSynthesizeAttribute;
   }

// ifndef  CXX_IS_ROSE_CODE_GENERATION
// #endif 
