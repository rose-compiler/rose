/****************************************************************************

Remarks on Unparsing and on attaching preprocessing information to AST nodes
----------------------------------------------------------------------------
Markus Kowarschik, 10/2002

There is a macro defined in sage3.h:
#define USE_OLD_MECHANISM_OF_HANDLING_PREPROCESSING_INFO {1|0}

If this macro is set to 1, then the *OLD* mechanism of unparsing
preprocessing information is used: in the beginning of unparsing the
AST, the function
ROSEAttributesList *getPreprocessorDirectives(char *fileName);
is called to build a list of preprocessor directives. This list is
being searched for directives to be unparsed in the course of unparsing
the AST.

If this macro is set to 0, the *NEW* unparsing mechmism is used.

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
#include "rose.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;
typedef boost::wave::cpplexer::lex_token<> token_type;
typedef std::vector<token_type>            token_container;
typedef std::list<token_type>              token_list_container;
typedef std::vector<std::list<token_type> >       token_container_container;


// Debug flag
#define DEBUG_ATTACH_PREPROCESSING_INFO 0

// DQ (9/24/2007): Moved function definition to source file from header file.
// AS(011306) Constructor for use of Wave Preprocessor
AttachPreprocessingInfoTreeTrav::AttachPreprocessingInfoTreeTrav( std::map<std::string,ROSEAttributesList*>* attrMap)
   {
     previousLocNodePtr            = NULL;
     currentListOfAttributes       = NULL;
     sizeOfCurrentListOfAttributes = 0;
  // currentFileName               = NULL;
	 currentFileNameId             = -1;
     ROSE_ASSERT(attrMap != NULL);
     currentMapOfAttributes        = attrMap;
     use_Wave                      = true;
     start_index                   = 0;
  }

// DQ (9/24/2007): Moved function definition to source file from header file.
// Constructor
AttachPreprocessingInfoTreeTrav::AttachPreprocessingInfoTreeTrav()
   {
     previousLocNodePtr            = NULL;
     currentListOfAttributes       = NULL;
     sizeOfCurrentListOfAttributes = 0;
  // currentFileName               = NULL;
     currentMapOfAttributes        = NULL;
     use_Wave                      = false;
     start_index                   = 0;
   }

void
AttachPreprocessingInfoTreeTrav::iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
   ( SgLocatedNode* locatedNode, int lineNumber, PreprocessingInfo::RelativePositionType location, bool reset_start_index )
   {
  // DQ (11/23/2008): Added comment.
  // This is the main function called to insert all PreprocessingInfo objects into IR nodes.  This function currently 
  // adds the PreprocessingInfo objects as attributes, but will be modified to insert the CPP directive specific
  // PreprocessingInfo objects as separate IR nodes and leave PreprocessingInfo objects that are comments inserts
  // as attributes.  Note that attributes imply PreprocessingInfo specific atributes and not the more general 
  // mechanism available in ROSE for user defined attributes to be saved into the AST.

#if 0
  // Debugging information...
     printf ("In iterateOverListAndInsertPrev... locatedNode = %s lineNumber = %d location = %d \n",locatedNode->sage_class_name(),lineNumber,(int)location);
     if ( dynamic_cast<SgLocatedNode*>(locatedNode) != NULL )
        {
          printf ("starting line number = %d \n",locatedNode->get_startOfConstruct()->get_line());
          if (locatedNode->get_endOfConstruct() != NULL)
               printf ("ending line number   = %d \n",locatedNode->get_endOfConstruct()->get_line());
        }
       else
        {
          printf ("locatedNode is not a SgLocatedNode object \n");
        }
#endif

#if DEBUG_ATTACH_PREPROCESSING_INFO
  // Debugging information...
        {
          int line = locatedNode->get_startOfConstruct()->get_line();
          int col  = locatedNode->get_startOfConstruct()->get_col();
          cout << "Visiting SgStatement node: " << line << ", " << col << " -> ";
          cout << getVariantName(locatedNode->variantT()) << endl;
        }
#if 0
     printf("-----> Address: %p\n", locatedNode);
     cout << "-----> Filename: " << locatedNode->get_file_info()->get_filename() << endl;
     if (locatedNode->getAttachedPreprocessingInfo() == NULL)
          cout << "-----> No PreprocessingInfo objects attached yet" << endl;
     else
          cout << "-----> There are already PreprocessingInfo objects attached to this AST node" << endl;
#endif
     cout << "Traversing current list of attributes of length " << sizeOfCurrentListOfAttributes << endl;
#endif

  // for ( int i = 0; i < sizeOfCurrentListOfAttributes; i++ )
  // AS(09/21/07) Because the AttachAllPreprocessingInfoTreeTrav can call the evaluateInheritedAttribute(..)
  // which calls this function the start_index can not be static for this function. Instead it is made
  // a class member variable for AttachPreprocessingInfoTreeTrav so that it can be reset by AttachAllPreprocessingInfoTreeTrav
  // when processing a new file.

  //   static int start_index = 0;
     for ( int i = start_index; i < sizeOfCurrentListOfAttributes; i++ )
        {
          PreprocessingInfo *currentPreprocessingInfoPtr = (*currentListOfAttributes)[i];
#if 0
          if ( currentPreprocessingInfoPtr != NULL )
               printf ("currentPreprocessingInfoPtr->getLineNumber() = %d lineNumber = %d \n",currentPreprocessingInfoPtr->getLineNumber(),lineNumber);
#endif
#if 0
          printf ("currentPreprocessingInfoPtr->getLineNumber() = %d lineNumber = %d internalString = %s \n",currentPreprocessingInfoPtr->getLineNumber(),lineNumber,currentPreprocessingInfoPtr->getString().c_str());
#endif

          bool attachCommentOrDirective = (currentPreprocessingInfoPtr != NULL) && (currentPreprocessingInfoPtr->getLineNumber() <= lineNumber);

          if ( attachCommentOrDirective == true )
             {
#if 0
               printf ("Attaching \"%s\" (from line# %d) to %s locatedNode = %p = %s = %s at line %d \n",
                   currentPreprocessingInfoPtr->getString().c_str(),
                   currentPreprocessingInfoPtr->getLineNumber(),
                   (locatedNode->get_file_info()->isCompilerGenerated() == true) ? "compiler-generated" : "non-compiler-generated",
                   locatedNode,
                   locatedNode->class_name().c_str(),SageInterface::get_name(locatedNode).c_str(),
                   (locatedNode->get_file_info()->isCompilerGenerated() == true) ? -1 : locatedNode->get_file_info()->get_line());
            // printf ("locatedNode->unparseToString() = %s \n",locatedNode->unparseToString().c_str());
#endif

            // Mark this PreprocessingInfo object as having been placed into the AST
            // It might make more sense to remove it from the list so it doesn't have 
            // to be traversed next time.
            // currentPreprocessingInfoPtr->setHasBeenCopied();
               currentListOfAttributes->getList()[i] = NULL;

            // DQ (4/13/2007): If we are going to invalidate the list of accumulated attributes then we can start 
            // next time at the next index (at least).  This removes the order n^2 complexity of traversing over the whole loop.
               start_index = i+1;

            // Mark the location relative to the current node where the PreprocessingInfo 
            // object should be unparsed (before or after) relative to the current locatedNode
               currentPreprocessingInfoPtr->setRelativePosition(location);
#if 1
               locatedNode->addToAttachedPreprocessingInfo(currentPreprocessingInfoPtr);
#else
            // If this is a CPP directive then add it as an IR nodes so that we can position 
            // CPP directives in the AST as new IR nodes instead of as comment.

            // I don't think we want to call get scope since we want the structural scope of the
            // statement in the file (get_parent()) and not the languages concept of scope (get_scope()).
            // SgScopeStatement* localScope = isSgScopeStatement(locatedNode->get_parent());
            // if (localScope != NULL)

               PreprocessingInfo::DirectiveType directive = currentPreprocessingInfoPtr->getTypeOfDirective();
               switch (directive)
                  {
                 // Trap this case out as an error...
                    case PreprocessingInfo::CpreprocessorUnknownDeclaration:
                       {
                      // I think this is an error...
                      // locatedNode->addToAttachedPreprocessingInfo(currentPreprocessingInfoPtr);
                         printf ("Error: directive == PreprocessingInfo::CpreprocessorUnknownDeclaration \n");
                         ROSE_ASSERT(false);
                         break;
                       }

                 // These are things that we don't have to worry about in Fortran CPP handling.                          
                    case PreprocessingInfo::C_StyleComment:
                    case PreprocessingInfo::CplusplusStyleComment:
                    case PreprocessingInfo::FortranStyleComment:
                    case PreprocessingInfo::CpreprocessorBlankLine:
                    case PreprocessingInfo::ClinkageSpecificationStart:
                    case PreprocessingInfo::ClinkageSpecificationEnd:
                            {
                              locatedNode->addToAttachedPreprocessingInfo(currentPreprocessingInfoPtr);
                              break;
                            }

                 // These are the CPP directives that I want to focus on support for in Fortran (and C/C++).
                    case PreprocessingInfo::CpreprocessorIncludeDeclaration:
                    case PreprocessingInfo::CpreprocessorIncludeNextDeclaration:
                    case PreprocessingInfo::CpreprocessorDefineDeclaration:
                    case PreprocessingInfo::CpreprocessorUndefDeclaration:
                    case PreprocessingInfo::CpreprocessorIfdefDeclaration:
                    case PreprocessingInfo::CpreprocessorIfndefDeclaration:
                    case PreprocessingInfo::CpreprocessorIfDeclaration:
                    case PreprocessingInfo::CpreprocessorDeadIfDeclaration:
                    case PreprocessingInfo::CpreprocessorElseDeclaration:
                    case PreprocessingInfo::CpreprocessorElifDeclaration:
                    case PreprocessingInfo::CpreprocessorEndifDeclaration:
                    case PreprocessingInfo::CpreprocessorLineDeclaration:
                    case PreprocessingInfo::CpreprocessorErrorDeclaration:
                    case PreprocessingInfo::CpreprocessorWarningDeclaration:
                    case PreprocessingInfo::CpreprocessorEmptyDeclaration:
                    case PreprocessingInfo::CpreprocessorIdentDeclaration:
                    case PreprocessingInfo::CpreprocessorCompilerGenerateLineDeclaration:
                       {
                      // These are CPP directives that we want to have generate IR nodes into the AST.
                         SgC_PreprocessorDirectiveStatement* cppDirective = SgC_PreprocessorDirectiveStatement::createDirective(currentPreprocessingInfoPtr);
                         ROSE_ASSERT(cppDirective != NULL);

                         ROSE_ASSERT(cppDirective->get_startOfConstruct() != NULL);
                         ROSE_ASSERT(cppDirective->get_endOfConstruct()   != NULL);

                         SgScopeStatement* localScope = isSgScopeStatement(locatedNode);
                         if (localScope != NULL)
                            {
                              printf ("Calling SageInterface::prependStatement(cppDirective = %s,localScope = %s) \n",cppDirective->class_name().c_str(),localScope->class_name().c_str());
                              SageInterface::prependStatement(cppDirective,localScope);
                            }
                           else
                            {
                           // Note that the AST traversal will not see this as the next statement since it 
                           // traverses a copy of the list of statements to avoid such side-effects.
                              SgStatement* currentStatement = isSgStatement(locatedNode);
                              if (currentStatement != NULL)
                                 {
                                   printf ("Calling SageInterface::insertStatementAfter(currentStatement = %s,cppDirective = %s) \n",currentStatement->class_name().c_str(),cppDirective->class_name().c_str());
                                   SageInterface::insertStatementAfter(currentStatement,cppDirective);
                                 }
                                else
                                 {
                                   printf ("Error: locatedNode is not a statement (locatedNode = %s) \n",locatedNode->class_name().c_str());
                                   ROSE_ASSERT(false);
                                 }
                            }

                         break;
                       }

                    case PreprocessingInfo::LineReplacement:
                       {
                         printf ("I am unclear where this PreprocessingInfo::LineReplacement is used! \n");
                         locatedNode->addToAttachedPreprocessingInfo(currentPreprocessingInfoPtr);
                         break;
                       }

                 // I think these are Wave specific so handle them as though this was C/C++ code.
                    case PreprocessingInfo::CSkippedToken:
                    case PreprocessingInfo::CMacroCall:
                    case PreprocessingInfo::CMacroCallStatement:
                       {
                         printf ("I think these are WAVE specific! directiveTypeName = %s \n",PreprocessingInfo::directiveTypeName(directive).c_str());
                         locatedNode->addToAttachedPreprocessingInfo(currentPreprocessingInfoPtr);
                         break;
                       }

                 // The default should be an error...
                    default:
                       {
                         printf ("Error: directive not handled directiveTypeName = %s \n",PreprocessingInfo::directiveTypeName(directive).c_str());
                         ROSE_ASSERT(false);
                       }
                  }
#endif

            // delete currentPreprocessingInfoPtr;
            // currentPreprocessingInfoPtr = NULL;

            // debugging info
            // printOutComments(locatedNode);
             }
        }

  // DQ (4/13/2007): The evaluation of the synthesized attribute for a SgFile will trigger the reset of the start index to 0.
     if (reset_start_index == true)
          start_index = 0;
   }


// DQ (11/23/2008): This is a static function
SgC_PreprocessorDirectiveStatement* SgC_PreprocessorDirectiveStatement::createDirective ( PreprocessingInfo* currentPreprocessingInfo )
   {
  // This is the new factory interface to build CPP directives as IR nodes.
     PreprocessingInfo::DirectiveType directive = currentPreprocessingInfo->getTypeOfDirective();

     SgC_PreprocessorDirectiveStatement* cppDirective = new SgEmptyDirectiveStatement();
     switch(directive)
        {
          case PreprocessingInfo::CpreprocessorUnknownDeclaration:
             {
            // I think this is an error...
            // locatedNode->addToAttachedPreprocessingInfo(currentPreprocessingInfoPtr);
               printf ("Error: directive == PreprocessingInfo::CpreprocessorUnknownDeclaration \n");
               ROSE_ASSERT(false);
               break;
             }

          case PreprocessingInfo::C_StyleComment:
          case PreprocessingInfo::CplusplusStyleComment:
          case PreprocessingInfo::FortranStyleComment:
          case PreprocessingInfo::CpreprocessorBlankLine:
          case PreprocessingInfo::ClinkageSpecificationStart:
          case PreprocessingInfo::ClinkageSpecificationEnd:
          case PreprocessingInfo::CpreprocessorIncludeDeclaration:
          case PreprocessingInfo::CpreprocessorIncludeNextDeclaration:
          case PreprocessingInfo::CpreprocessorDefineDeclaration:
          case PreprocessingInfo::CpreprocessorUndefDeclaration:
          case PreprocessingInfo::CpreprocessorIfdefDeclaration:
          case PreprocessingInfo::CpreprocessorIfndefDeclaration:
          case PreprocessingInfo::CpreprocessorIfDeclaration:
          case PreprocessingInfo::CpreprocessorDeadIfDeclaration:
          case PreprocessingInfo::CpreprocessorElseDeclaration:
          case PreprocessingInfo::CpreprocessorElifDeclaration:
          case PreprocessingInfo::CpreprocessorEndifDeclaration:
          case PreprocessingInfo::CpreprocessorLineDeclaration:
          case PreprocessingInfo::CpreprocessorErrorDeclaration:
          case PreprocessingInfo::CpreprocessorWarningDeclaration:
          case PreprocessingInfo::CpreprocessorEmptyDeclaration:
          case PreprocessingInfo::CpreprocessorIdentDeclaration:
          case PreprocessingInfo::CpreprocessorCompilerGenerateLineDeclaration:
             {
               cppDirective = new SgEmptyDirectiveStatement();
               break;
             }
             
          default:
             {
               printf ("Error: directive not handled directiveTypeName = %s \n",PreprocessingInfo::directiveTypeName(directive).c_str());
               ROSE_ASSERT(false);
             }
        }

     ROSE_ASSERT(cppDirective != NULL);

  // Set the defining declaration to be a self reference...
     cppDirective->set_definingDeclaration(cppDirective);

  // Build source position information...
     cppDirective->set_startOfConstruct(new Sg_File_Info(*(currentPreprocessingInfo->get_file_info())));
     cppDirective->set_endOfConstruct(new Sg_File_Info(*(currentPreprocessingInfo->get_file_info())));

     return cppDirective;
   }


void
AttachPreprocessingInfoTreeTrav::setupPointerToPreviousNode (SgLocatedNode* currentLocNodePtr )
   {
  // If we are at a SgCtorInitializerList then since it is visited last 
  // (after the definition) leave the previousLocNodePtr referenced to 
  // the function definition.
     if ( (dynamic_cast<SgForInitStatement*>     (currentLocNodePtr) == NULL) &&
          (dynamic_cast<SgTypedefSeq*>           (currentLocNodePtr) == NULL) &&
          (dynamic_cast<SgCatchStatementSeq*>    (currentLocNodePtr) == NULL) &&
          (dynamic_cast<SgFunctionParameterList*>(currentLocNodePtr) == NULL) &&
          (dynamic_cast<SgCtorInitializerList*>  (currentLocNodePtr) == NULL) )
        {
          previousLocNodePtr = currentLocNodePtr;
        }
       else
        {
          SgStatement* currentStatement = dynamic_cast<SgStatement*>(currentLocNodePtr);
          ROSE_ASSERT (currentStatement != NULL);
          SgStatement* parentStatement = isSgStatement(currentStatement->get_parent());
          ROSE_ASSERT (parentStatement != NULL);

       // printf ("parentStatement = %s \n",parentStatement->sage_class_name());
#if 1
          previousLocNodePtr = parentStatement;
#else
       // DQ (11/23/2008): I think that we can eliminate this code since we have the tests 
       // below to make sure that no IR node is in the set defined by: (SgForInitStatement, 
       // SgTypedefSeq, SgCatchStatementSeq, SgFunctionParameterList, SgCtorInitializerList).
          switch (currentLocNodePtr->variantT())
             {
               case V_SgForInitStatement:
                  previousLocNodePtr = parentStatement;
                  break;
               case V_SgTypedefSeq:
                  previousLocNodePtr = parentStatement;
                  printf ("Exiting in SgTypedefSeq case \n");
                  ROSE_ABORT();
                  break;

               case V_SgCatchStatementSeq:
                  previousLocNodePtr = parentStatement;
                  break;

               case V_SgFunctionParameterList:
                  previousLocNodePtr = parentStatement;
                  break;

               case V_SgCtorInitializerList:
                  previousLocNodePtr = parentStatement;
                  break;

               default:
                  printf ("AttachPreprocessingInfoTreeTrav::setupPointerToPreviousNode(): default found in switch statement  currentLocNodePtr = %s \n",currentLocNodePtr->class_name().c_str());
                  ROSE_ABORT();
                  break;
             }
#endif
        }

  // Nodes that should not have comments attached (since they are not unparsed directly)
     ROSE_ASSERT (dynamic_cast<SgForInitStatement*>     (previousLocNodePtr) == NULL);
     ROSE_ASSERT (dynamic_cast<SgTypedefSeq*>           (previousLocNodePtr) == NULL);
     ROSE_ASSERT (dynamic_cast<SgCatchStatementSeq*>    (previousLocNodePtr) == NULL);
     ROSE_ASSERT (dynamic_cast<SgFunctionParameterList*>(previousLocNodePtr) == NULL);
     ROSE_ASSERT (dynamic_cast<SgCtorInitializerList*>  (previousLocNodePtr) == NULL);
   }


// Member function: evaluateInheritedAttribute
AttachPreprocessingInfoTreeTraversalInheritedAttrribute
AttachPreprocessingInfoTreeTrav::evaluateInheritedAttribute (
    SgNode *n,
    AttachPreprocessingInfoTreeTraversalInheritedAttrribute inh)
   {
     SgFile *currentFilePtr           = NULL;
     SgLocatedNode *currentLocNodePtr = NULL;
     int line = 0, col = 0;
  // PreprocessingInfo *currentPreprocessingInfoPtr;

#if 0
     printf ("In AttachPreprocessingInfoTreeTrav::evaluateInheritedAttribute(): n->class_name() = %s \n",n->class_name().c_str());
#endif

  // DQ (12/10/2007): Declare Fortran specific lexical pass function explicitly.
  // extern int getFortranFixedFormatPreprocessorDirectives( std::string fileName );
  // extern int getFortranFreeFormatPreprocessorDirectives ( std::string fileName );
#ifdef USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT
     extern std::list <stream_element*>* getFortranFixedFormatPreprocessorDirectives( std::string fileName );
     extern std::list <stream_element*>* getFortranFreeFormatPreprocessorDirectives ( std::string fileName );
#endif

  // Check if current AST node is an SgFile object
  // if ((currentFilePtr = dynamic_cast<SgFile*>(n)) != NULL)
     currentFilePtr = isSgFile(n);
     if ( currentFilePtr != NULL )
        {
       // Current AST node is an SgFile object, generate the corresponding list of attributes
#if DEBUG_ATTACH_PREPROCESSING_INFO
          cout << "=== Visiting SgFile node and building current list of attributes ===" << endl;
#endif
          currentFileNameId               = currentFilePtr->get_file_info()->get_file_id();

       // currentListOfAttributes       = getPreprocessorDirectives(currentFileName);

          if (use_Wave == false)
             {
            // DQ (4/12/2007): Introduce tracking of performance of ROSE.
               TimingPerformance timer ("AST evaluateInheritedAttribute (use_Wave == false):");

            // AS(011306) don't need this with the Wave preprocessor (already done in one pass)
            // currentListOfAttributes       = getPreprocessorDirectives( Sg_File_Info::getFilenameFromID(currentFileNameId) );
               if (currentFilePtr->get_Fortran_only() == true)
                  {
#ifdef USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT
                 // This is either of two different kinds of Fortran program: fixed format or free format
                 //    * fix format is used for older Fortran code, F77 and earlier, and 
                 //    * free format is used for newer codes, F90 and later
                    if ( SgProject::get_verbose() > 1 )
                       {
                         printf ("Found a Fortran program (calling lexical pass to gather the Fortran specific token stream) \n");
                       }

                 // If it is not explicitly fixed form, then assume it is free form input.
                 // if (currentFilePtr->get_fixedFormat() == true)
                    if (currentFilePtr->get_inputFormat() == SgFile::e_fixed_form_output_format)
                       {
                         if ( SgProject::get_verbose() > 1 )
                            {
                              printf ("Fortran code assumed to be in fixed format form (skipping translation of tokens) \n");
                            }

                         ROSE_ASSERT(currentFilePtr != NULL);
                         string fileNameForDirectivesAndComments = currentFilePtr->get_sourceFileNameWithPath();

                      // For now we call the lexical pass on the fortran file, but we don't yet translate the tokens.
                      // currentListOfAttributes       = getPreprocessorDirectives( Sg_File_Info::getFilenameFromID(currentFileNameId) );
                      // getFortranFixedFormatPreprocessorDirectives( Sg_File_Info::getFilenameFromID(currentFileNameId) );
                      // LexTokenStreamTypePointer lex_token_stream = getFortranFixedFormatPreprocessorDirectives( Sg_File_Info::getFilenameFromID(currentFileNameId) );
                         LexTokenStreamTypePointer lex_token_stream = getFortranFixedFormatPreprocessorDirectives( fileNameForDirectivesAndComments );
                         ROSE_ASSERT(lex_token_stream != NULL);

                      // Build an empty list while we skip the translation of tokens
                         currentListOfAttributes = new ROSEAttributesList();

                      // Attach the token stream to the AST
                         currentListOfAttributes->set_rawTokenStream(lex_token_stream);
#if 1
                      // DQ (11/23/2008): This is the new support to collect CPP directives and comments from Fortran applications.
                      // printf ("Calling collectPreprocessorDirectivesAndCommentsForAST() to collect CPP directives for fileNameForDirectivesAndComments = %s \n",fileNameForDirectivesAndComments.c_str());
                         currentListOfAttributes->collectPreprocessorDirectivesAndCommentsForAST(fileNameForDirectivesAndComments,ROSEAttributesList::e_Fortran77_language);
                      // printf ("DONE: Calling collectPreprocessorDirectivesAndCommentsForAST() to collect CPP directives for fileNameForDirectivesAndComments = %s \n",fileNameForDirectivesAndComments.c_str());
#endif
#if 0
                      // DQ (11/19/2008): This code has been replaced by collectPreprocessorDirectivesAndCommentsForAST().
                      // Process the raw token stream into the PreprocessorDirectives and Comment list required to be inserted into the AST.
                      // currentListOfAttributes->collectFixedFormatPreprocessorDirectivesAndCommentsForAST(currentFilePtr->get_sourceFileNameWithPath());
                         currentListOfAttributes->collectFixedFormatPreprocessorDirectivesAndCommentsForAST(fileNameForDirectivesAndComments);
#endif
                       }
                      else
                       {
                         if ( SgProject::get_verbose() > 1 )
                            {
                              printf ("Fortran code assumed to be in free format form (skipping translation of tokens) \n");
                            }

                         if (currentFilePtr->get_inputFormat() != SgFile::e_free_form_output_format)
                            {
                              printf ("Note: In lexical pass, Fortran code assumed to be free form, but not marked explicitly as such! currentFilePtr->get_inputFormat() = %d \n",currentFilePtr->get_inputFormat());
                            }

                      // For now we call the lexical pass on the fortran file, but we don't yet translate the tokens.
                      // currentListOfAttributes       = getPreprocessorDirectives( Sg_File_Info::getFilenameFromID(currentFileNameId) );
                      // getFortranFreeFormatPreprocessorDirectives( Sg_File_Info::getFilenameFromID(currentFileNameId) );
                         string fileNameForTokenStream = Sg_File_Info::getFilenameFromID(currentFileNameId);
                      // printf ("Calling getFortranFreeFormatPreprocessorDirectives() for fileNameForTokenStream = %s \n",fileNameForTokenStream.c_str());
                         LexTokenStreamTypePointer lex_token_stream = getFortranFreeFormatPreprocessorDirectives( fileNameForTokenStream );
                         ROSE_ASSERT(lex_token_stream != NULL);

                      // Build an empty list while we skip the translation of tokens
                         currentListOfAttributes = new ROSEAttributesList();

                      // Attach the token stream to the AST
                         currentListOfAttributes->set_rawTokenStream(lex_token_stream);
                         ROSE_ASSERT(currentListOfAttributes->get_rawTokenStream() != NULL);

                      // printf ("Fortran Token List Size: currentListOfAttributes->get_rawTokenStream()->size() = %zu \n",currentListOfAttributes->get_rawTokenStream()->size());

                      // Process the raw token stream into the PreprocessorDirectives and Comment list required to be inserted into the AST.
                      // currentListOfAttributes->generatePreprocessorDirectivesAndCommentsForAST(currentFilePtr);
                         string fileNameForDirectivesAndComments = currentFilePtr->get_sourceFileNameWithPath();

                      // DQ (11/23/2008): This is the new support to collect CPP directives and comments from Fortran applications.
                      // printf ("Calling collectPreprocessorDirectivesAndCommentsForAST() to collect CPP directives for fileNameForDirectivesAndComments = %s \n",fileNameForDirectivesAndComments.c_str());
                         currentListOfAttributes->collectPreprocessorDirectivesAndCommentsForAST(fileNameForDirectivesAndComments,ROSEAttributesList::e_Fortran9x_language);

#if 0
                         printf ("Done with processing of separate lexical pass to gather CPP directives \n");
                         ROSE_ASSERT(false);
#endif
#if 0
                      // DQ (11/19/2008): This code has been replaced by collectPreprocessorDirectivesAndCommentsForAST().
                         printf ("Calling generatePreprocessorDirectivesAndCommentsForAST() for fileNameForDirectivesAndComments = %s \n",fileNameForDirectivesAndComments.c_str());
                         currentListOfAttributes->generatePreprocessorDirectivesAndCommentsForAST(fileNameForDirectivesAndComments);
#else
                      // printf ("Skipping the comments in the fortran file! \n");
#endif
                       }

                    if ( SgProject::get_verbose() > 1 )
                       {
                         printf ("Done with separate lexical pass to gather Fortran specific token stream \n");
                       }

                    if ( SgProject::get_verbose() > 1 )
                       {
                         printf ("Done with processing of separate lexical pass to gather Fortran specific CPP directives and comments from the token stream \n");
                       }
#if 0
                    printf ("Done with processing of separate lexical pass to gather Fortran specific CPP directives and comments from the token stream \n");
                    ROSE_ASSERT(false);
#endif

#else // for !USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT
                    fprintf(stderr, "Fortran parser not enabled\n");
                    abort();
#endif // USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT
                  }
                 else
                  {
                 // Else we assume this is a C or C++ program (for which the lexical analysis is identical)
                 // The lex token stream is now returned in the ROSEAttributesList object.

                    string fileNameForDirectivesAndComments = currentFilePtr->get_sourceFileNameWithPath();

#if 1
                 // DQ (11/23/2008): This is part of CPP handling for Fortran, but tested on C and C++ codes aditionally, (it is redundant for C and C++).
                 // This is a way of testing the extraction of CPP directives (on C and C++ codes, so that it is more agressively tested).
                 // Since this is a redundant test, it can be removed in later development (its use is only a performance issue).
                    currentListOfAttributes = new ROSEAttributesList();

                 // This call is just a test, this function is defined for use on Fortran.  For C and C++ we have alternative methods to extract the CPP directives and comments.
                 // printf ("Call collectPreprocessorDirectivesAndCommentsForAST to test C and C++ preprocessor directive collaction \n");
                    currentListOfAttributes->collectPreprocessorDirectivesAndCommentsForAST(fileNameForDirectivesAndComments,ROSEAttributesList::e_C_language);
                 // printf ("DONE: Call collectPreprocessorDirectivesAndCommentsForAST to test C and C++ preprocessor directive collaction \n");
#endif

                 // This function has been modified to clear any existing list of PreprocessingInfo*
                 // objects (so that we can test the function: collectPreprocessorDirectivesAndCommentsForAST()).
                 // currentListOfAttributes = getPreprocessorDirectives( Sg_File_Info::getFilenameFromID(currentFileNameId) );
                    currentListOfAttributes = getPreprocessorDirectives(fileNameForDirectivesAndComments);
                  }

            // printf ("AttachPreprocessingInfoTreeTrav::evaluateInheritedAttribute(): currentListOfAttributes = %p size() = %d \n",currentListOfAttributes,(int)currentListOfAttributes->size());
             }
            else
             {
            // This is the case of: (use_Wave == true).

            // AS(011306) fetch the list of attributes from the Wave output
               std::string currentStringFilename(  Sg_File_Info::getFilenameFromID(currentFileNameId) );

               ROSE_ASSERT(currentMapOfAttributes != NULL);
               if (currentMapOfAttributes->find(currentStringFilename) == currentMapOfAttributes->end())
                  {
                 // There is no existing list for this file, so build an empty list.
                    currentListOfAttributes = new ROSEAttributesList();
                  }
                 else
                  {
                 // If there already exists a list for the current file then get that list.
                    ROSE_ASSERT( currentMapOfAttributes->find(currentStringFilename)->second != NULL);
                    currentListOfAttributes       = currentMapOfAttributes->find(currentStringFilename)->second;
                  }
             }

          ROSE_ASSERT(currentListOfAttributes != NULL);
          sizeOfCurrentListOfAttributes = currentListOfAttributes->getLength();
        }
       else
        {
       // The current node is NOT a SgFile IR node.

       // Move attributes from the list of attributes into the collection of the current AST nodes,
       // we only consider statements for the moment, but this needs to be refined further on.
       // Probably we will have to consider each SgLocatedNode IR node within the AST.
          if (dynamic_cast<SgStatement*>(n) != NULL)
             {
            // The following should always work since each statement is a located node
               currentLocNodePtr = dynamic_cast<SgLocatedNode*>(n);
               ROSE_ASSERT(currentLocNodePtr != NULL);

            // Attach the comments only to nodes from the same file
               int fileNameId = currentFileNameId;
               ROSE_ASSERT(currentLocNodePtr->get_file_info());

            // DQ (6/20/2005): Compiler generated is not enough, it must be marked for output explicitly
            // bool isCompilerGenerated = currentLocNodePtr->get_file_info()->isCompilerGenerated();
               bool isCompilerGenerated = currentLocNodePtr->get_file_info()->isCompilerGeneratedNodeToBeUnparsed();
            // JJW (6/25/2008): These are always flagged as "to be
            // unparsed", even if they are not unparsed because their
            // corresponding declarations aren't unparsed
               if (isSgClassDefinition(currentLocNodePtr) ||
                   isSgFunctionDefinition(currentLocNodePtr)) {
                 SgLocatedNode* ln = isSgLocatedNode(currentLocNodePtr->get_parent());
                 Sg_File_Info* parentFi = ln ? ln->get_file_info() : NULL;
                 if (parentFi && parentFi->isCompilerGenerated() && !parentFi->isCompilerGeneratedNodeToBeUnparsed()) {
                   isCompilerGenerated = false;
                 }
               }
               bool isTransformation    = currentLocNodePtr->get_file_info()->isTransformation();

            // Try to not call get_filename() if it would be inappropriate (either when isCompilerGenerated || isTransformation)

            // DQ (10/27/2007): Initialized to -1 upon suggestion by Andreas.
               int fileIdForOriginOfCurrentLocatedNode = -1;
               if ( !isCompilerGenerated && !isTransformation )
                    fileIdForOriginOfCurrentLocatedNode = currentLocNodePtr->get_file_info()->get_file_id();
#if 0
               printf ("isCompilerGenerated = %s isTransformation = %s fileIdForOriginOfCurrentLocatedNode = %s \n",
                   isCompilerGenerated ? "true" : "false",isTransformation ? "true" : "false",fileIdForOriginOfCurrentLocatedNode.c_str());
#endif
            // DQ (5/24/2005): Relaxed to handle compiler generated and transformed IR nodes
               if ( isCompilerGenerated || isTransformation || fileNameId == fileIdForOriginOfCurrentLocatedNode )
                  {
                 // Current node belongs to the file the name of which has been specified
                 // on the command line
                    line = currentLocNodePtr->get_file_info()->get_line();
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
                    iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber(currentLocNodePtr,line,PreprocessingInfo::before, reset_start_index );

                 // save the previous node (in an accumulator attribute), but handle some nodes differently
                 // to avoid having comments attached to them since they are not unparsed directly.
                    setupPointerToPreviousNode(currentLocNodePtr);
                  }
#if 0
                 else
                  {
                    cout << "Node belongs to a different file: ";
                  }
#endif
             }
        }

     return inh;
   }


// Member function: evaluateSynthesizedAttribute
AttachPreprocessingInfoTreeTraversalSynthesizedAttribute
AttachPreprocessingInfoTreeTrav::evaluateSynthesizedAttribute(
    SgNode *n,
    AttachPreprocessingInfoTreeTraversalInheritedAttrribute inh,
    SubTreeSynthesizedAttributes st)
   {
     AttachPreprocessingInfoTreeTraversalSynthesizedAttribute syn;
  // PreprocessingInfo *currentPreprocessingInfoPtr = NULL;

#if 0
     printf ("In AttachPreprocessingInfoTreeTrav::evaluateSynthesizedAttribute(): n->sage_class_name() = %s \n",n->sage_class_name());
     if (isSgStatement(n) && (isSgStatement(n)->get_parent() != NULL) )
          printf ("     parent = %s \n",isSgStatement(n)->get_parent()->sage_class_name());

     ROSE_ASSERT(previousLocNodePtr != NULL);
     printf ("     previousLocNodePtr->sage_class_name()                   = %s \n",previousLocNodePtr->sage_class_name());
#endif

  // error checking
     if (isSgCaseOptionStmt(n) != NULL)
        {
       // make sure there is a valid body
#if 0
          if (isSgCaseOptionStmt(n)->get_body() == NULL)
             {
            // DEBUGGING: print out the location where we are failing
            // SgStatement* parent = isSgStatement(n)->get_parent();
               SgStatement* parent = isSgStatement(isSgStatement(n)->get_parent());
               if (parent != NULL)
                  {
                    Sg_File_Info* fileInfo = parent->get_file_info();
                    printf ("parent = %s filename = %s line = %d \n",
                        parent->sage_class_name(),fileInfo->get_filename(),fileInfo->get_line());
                  }
             }
#endif
          ROSE_ASSERT (isSgCaseOptionStmt(n)->get_body() != NULL);
        }

  // error checking
     if (isSgClassDeclaration(n) != NULL)
        {
       // DEBUGGING: print out the location where we are failing
#if 0
          SgStatement* parent = isSgStatement(isSgStatement(n)->get_parent());
          if (parent != NULL)
             {
            // make sure there is a valid body
               if (isSgClassDeclaration(n)->get_endOfConstruct() == NULL)
                  {
                    Sg_File_Info* fileInfo = parent->get_file_info();
                    printf ("parent = %s filename = %s line = %d \n",
                        parent->sage_class_name(),fileInfo->get_filename(),fileInfo->get_line());
                  }
             }
#endif
          if (isSgClassDeclaration(n)->get_endOfConstruct() == NULL)
             {
               ROSE_ASSERT(n->get_file_info() != NULL);
               n->get_file_info()->display("Warning: SgClassDeclaration::get_endOfConstruct() == NULL");
             }

       // DQ (2/7/2004): Need to find this error so that I can fix it correctly
       // QY: removed this assertion to pass template classes. need further fix
          ROSE_ASSERT (isSgClassDeclaration(n)->get_endOfConstruct() != NULL);
        }

  // Only process SgLocatedNode object and the SgFile object
     SgFile* fileNode           = dynamic_cast<SgFile*>(n);
     SgLocatedNode* locatedNode = dynamic_cast<SgLocatedNode*>(n);
     if ( (locatedNode != NULL) || (fileNode != NULL) )
        {
       // Attach the comments only to nodes from the same file
          int fileNameId = currentFileNameId;

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
                    fileIdForOriginOfCurrentLocatedNode = locatedNode->get_file_info()->get_file_id();
               if (locatedNode->get_endOfConstruct() != NULL)
                  {
                    ROSE_ASSERT (locatedNode->get_endOfConstruct() != NULL);
                    lineOfClosingBrace = locatedNode->get_endOfConstruct()->get_line();
                  }
             }
            else
             {
            // handle the trivial case of a SgFile node being from it's own file
               fileIdForOriginOfCurrentLocatedNode = fileNameId;
            // Use one billion as the max number of lines in a file
               const int OneBillion = 1000000000;
               lineOfClosingBrace = OneBillion;
             }

       // Make sure the astNode matches the current file's list of comments and CPP directives.
       // DQ (5/24/2005): Handle cases of isCompilerGenerated or isTransformation
          if ( (isCompilerGeneratedOrTransformation == true) || (fileNameId == fileIdForOriginOfCurrentLocatedNode) )
             {
#if 0
               printf ("In AttachPreprocessingInfoTreeTrav::evaluateSynthesizedAttribute(): %p = %s lineOfClosingBrace = %d \n",
                   n,n->sage_class_name(),lineOfClosingBrace);
#endif
               switch (n->variantT())
                  {
                 // I wanted to leave the SgFile case in the switch statement rather 
                 // than separating it out in a conditional statement at the top of the file.
                 // case V_SgFile:
                    case V_SgSourceFile:
                    case V_SgBinaryFile:
                          {
                         // printf ("Case SgFile: See if we can find a better target to attach these comments than %s \n",
                         //      previousLocNodePtr->sage_class_name());

                            SgLocatedNode* targetNode = previousLocNodePtr;

                         // printf ("In SgFile: previousLocNodePtr = %s \n",previousLocNodePtr->sage_class_name());

                         // If the target is a SgBasicBlock then try to find it's parent in the global scope
                            if (isSgBasicBlock(previousLocNodePtr) != NULL)
                               {
                                 while ( (targetNode != NULL) && (isSgGlobal(targetNode->get_parent()) == NULL) )
                                    {
                                      targetNode = dynamic_cast<SgLocatedNode*>(targetNode->get_parent());
                                   // printf ("loop: targetNode = %s \n",targetNode->sage_class_name());
                                    }
                               }

                         // printf ("In SgFile: targetNode = %s \n",targetNode->sage_class_name());

                         // Iterate over the list of comments and directives and add them to the AST
                            bool reset_start_index = true;
                            iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                              ( targetNode, lineOfClosingBrace, PreprocessingInfo::after, reset_start_index );

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

                         // printf ("Adding secondary lex pass information (currentListOfAttributes = %p) to file = %s \n",currentListOfAttributes,filename.c_str());
                            file->get_preprocessorDirectivesAndCommentsList()->addList(filename,currentListOfAttributes);
#else
                         // DQ (1/21/2008): Original code
                         // printf ("Delete Fortran Token List Size: currentListOfAttributes->get_rawTokenStream()->size() = %zu \n",currentListOfAttributes->get_rawTokenStream()->size());
                            delete currentListOfAttributes;
#endif
                            currentListOfAttributes = NULL;

                         // Reset the pointer to the previous located node and the current list size
                            previousLocNodePtr            = NULL;
                            sizeOfCurrentListOfAttributes = 0;
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
                              ( basicBlock, lineOfClosingBrace, PreprocessingInfo::inside,reset_start_index  );

                         // DQ (4/9/2005): We need to point to the SgBasicBlock and not the last return statement (I think)
                         // Reset the previousLocNodePtr to the current node so that all 
                         // PreprocessingInfo objects will be inserted relative to the 
                         // current node next time.
                            previousLocNodePtr = basicBlock;
                            break;
                          }

                    case V_SgClassDeclaration:
                          {
                            ROSE_ASSERT (locatedNode->get_endOfConstruct() != NULL);

                         // The following should always work since each statement is a located node
                            SgClassDeclaration* classDeclaration = dynamic_cast<SgClassDeclaration*>(n);

                         // DQ (3/18/2005): This is a more robust process (although it introduces a new location for a comment/directive)
                            bool reset_start_index = false;
                            iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                              ( previousLocNodePtr, lineOfClosingBrace, PreprocessingInfo::after, reset_start_index );
                         // printf ("Adding comment/directive to base of class declaration \n");
                         // iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                         //    ( locatedNode, lineOfClosingBrace, PreprocessingInfo::inside );

                            previousLocNodePtr = classDeclaration;
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
                              ( previousLocNodePtr, lineOfClosingBrace, PreprocessingInfo::after, reset_start_index );

                            previousLocNodePtr = typedefDeclaration;
                            break;
                          }

                 // GB (09/19/2007): Added support for preprocessing info inside variable declarations (e.g. after the
                 // base type, which is what the previousLocNodePtr might point to).
                    case V_SgVariableDeclaration:
                          {
                            ROSE_ASSERT(locatedNode->get_endOfConstruct() != NULL);

                            SgVariableDeclaration *variableDeclaration = isSgVariableDeclaration(n);
                            ROSE_ASSERT(variableDeclaration != NULL);

                            bool reset_start_index = false;
                            iterateOverListAndInsertPreviouslyUninsertedElementsAppearingBeforeLineNumber
                              ( previousLocNodePtr, lineOfClosingBrace, PreprocessingInfo::after, reset_start_index );

                            previousLocNodePtr = variableDeclaration;
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
                              ( locatedNode, lineOfClosingBrace, PreprocessingInfo::inside, reset_start_index );

                         // previousLocNodePtr = locatedNode;
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
                              ( locatedNode, lineOfClosingBrace, PreprocessingInfo::inside, reset_start_index );

                            previousLocNodePtr = enumDeclaration;
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
                              ( previousLocNodePtr, lineOfClosingBrace, PreprocessingInfo::after, reset_start_index );

                            previousLocNodePtr = namespaceDeclaration;
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
                              ( locatedNode, lineOfClosingBrace, PreprocessingInfo::inside, reset_start_index );

                            previousLocNodePtr = namespaceDefinition;
                            break;
                          }

                    // DQ (4/9/2005): Added support for templates instaiations which are compiler generated
                    //                but OK to attach comments to them (just not inside them!).
                    case V_SgTemplateInstantiationMemberFunctionDecl:
                          {
                            ROSE_ASSERT (locatedNode->get_endOfConstruct() != NULL);
                         // printf ("Found a SgTemplateInstantiationMemberFunctionDecl but only record it as a previousLocNodePtr \n");
                            previousLocNodePtr = locatedNode;
                          }

                    // DQ (4/21/2005): this can be the last statement and if it is we have to 
                    // record it as such so that directives/comments can be attached after it.
                    case V_SgTemplateInstantiationDirectiveStatement:
                          {
                            ROSE_ASSERT (locatedNode->get_endOfConstruct() != NULL);
                            previousLocNodePtr = locatedNode;
                          }

                    default:
                          {
#if 0
                            printf ("Skipping any possability of attaching a comment/directive after a %s \n",n->sage_class_name());
                         // ROSE_ASSERT(false);
#endif
                          }
                  }
             }

#if 0
          if (locatedNode != NULL)
             {
               printf ("Output attached comments: \n");
               printOutComments(locatedNode);
             }
#endif
        }

     return syn;
   }

#if CAN_NOT_COMPILE_WITH_ROSE != true

///////////////////////////////////////////////////////////////////////////////
//  Include Wave itself
#include <boost/wave.hpp>

///////////////////////////////////////////////////////////////////////////////
// Include the lexer stuff
#include <boost/wave/cpplexer/cpp_lex_token.hpp>    // token class
#include <boost/wave/cpplexer/cpp_lex_iterator.hpp> // lexer class


#include "advanced_preprocessing_hooks.h"
// #include "attachPreprocessingInfo.h"
#include "attributeListMap.h"

#endif


//Include files to get the current path
#include <unistd.h>
#include <sys/param.h>

// #include <iostream>
// #include <fstream>
// #include <string>



std::list<SgNode*> findNodes(SgNode* astNode){
     std::list<SgNode*> returnList;

     if(isSgFile(astNode)!=NULL)
          returnList.push_back(astNode);

     return returnList;
}



// AS (011306) Support for Wave preprocessor
  void
attachPreprocessingInfo(SgFile *sageFilePtr,  std::map<std::string,ROSEAttributesList*>* attrMap)
   {
  // DQ (7/6/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("AST Comment Processing (using Wave, inner part):");

  // Dummy attribute
     AttachPreprocessingInfoTreeTraversalInheritedAttrribute inh;

  // Make sure that the argument is not a NULL pointer
     ROSE_ASSERT(sageFilePtr);

  // Create tree traversal object for attaching the preprocessing information (using Wave)
     AttachPreprocessingInfoTreeTrav tt(attrMap);

  // Run tree traversal on specified source file
     tt.traverseWithinFile(sageFilePtr,inh);
   }


// DQ (4/5/2006): Older version not using Wave preprocessor
// This is the function to be called from the main function
// DQ: Now called by the SgFile constructor body (I think)
  void
attachPreprocessingInfo(SgFile *sageFilePtr)
   {
     ROSE_ASSERT(sageFilePtr != NULL);

//#ifdef USE_ROSE_BOOST_WAVE_SUPPORT2

     //std::cerr << "Checking to see if Wave is used " << std::endl;
     if(sageFilePtr->get_wave() == true )
     {
     std::cerr << "Using WAVE" << std::endl;

  // DQ (7/6/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("AST Comment and CPP Directive Processing (using Wave, outer part):");

  // Build the AST used by ROSE
  // SgProject* project = frontend(argc,argv);
     std::vector<std::string>  includeSpecifierlist;


  // Build the list that we will hand off to boost-wave
  //std::vector<std::string> includeSpecifierlist;
     std::vector<std::string> macroList;
     std::vector<std::string> preincludeList;

     const char* predefinedMacroListRaw[] = CXX_SPEC_DEF;
     vector<string> predefinedMacroList(predefinedMacroListRaw, predefinedMacroListRaw + sizeof(predefinedMacroListRaw) / sizeof(*predefinedMacroListRaw));
  // for (vector<string>::iterator i = predefinedMacroList.begin(); i != predefinedMacroList.end(); i++)
     vector<string>::iterator i = predefinedMacroList.begin();
     while (i != predefinedMacroList.end())
        {
 
           //AS(03/12/08) CXX_SPEC_DEF has been changed to only contain macro defs
          if (i->substr(0,2) == "-D")
             {
               string macro = i->substr(2);
	       
	       if(SgProject::get_verbose() >= 1)
                   printf ("Adding predefined macro to the macroList macro = %s \n",macro.c_str());

               macroList.push_back(macro);
             }
          else
             {
               string preincludeMarker = "--preinclude";
               if (i->substr(0,preincludeMarker.size()) == preincludeMarker)
                  {
                    i++;
                 // The following option is the file name associated with the "--preinclude" option
                    preincludeList.push_back(*i);

		    if(SgProject::get_verbose() >= 1)
                         std::cout << "Predefined macro: " << *i << std::endl;
                  }
               else if(i->empty()){}
               else
                  {
                    printf ("Found a non -D macro definition  (and non preinclude file) in the predefined macro list substring = %s *i = %s \n",i->substr(0,2).c_str(),i->c_str());
                  }
             }
          i++;
        }


  // Now add the entries specified on the commandline
     if(SgProject::get_verbose() >= 1)
        std::cout << "INCLUDES FROM COMMANDLInE" << std::endl;

     std::vector<std::string> commandLine = sageFilePtr->get_originalCommandLineArgumentList();
     for (vector<string>::iterator i = commandLine.begin(); i != commandLine.end(); i++)
        {
          ROSE_ASSERT((*i)[0] != ' ');
       // printf ("Command line argument: *i = %s \n",i->c_str());
          if (i->substr(0,2) == "-I")
             {
               includeSpecifierlist.push_back(*i);
	       if(SgProject::get_verbose() >= 1)
                    std::cout << *i << std::endl;
             }
          if (i->substr(0,2) == "-D")
             {
               string macro = i->substr(2);
	                                   
	       if(SgProject::get_verbose() >= 1)
                   printf ("Adding macro to the macroList macro = %s \n",macro.c_str());
               macroList.push_back(macro);
             }
        }
     if(SgProject::get_verbose() >= 1)
          std::cout << "DONE INCLUDES FROM COMMANDLInE" << std::endl;

     std::vector<SgNode*> accessFunctionsList;

  // Build list of value expressions
     std::vector<SgNode*> valueExp = NodeQuery::querySubTree (sageFilePtr,&queryFloatDoubleValExp);

  // Open and read in the specified input file.
     std::string sourceFileName = sageFilePtr->getFileName();

     if(SgProject::get_verbose() >= 1){
	     std::cout << "Source file name: \"" << sourceFileName << "\"" << std::endl;
	     std::cout << "Source file name: \"" << sageFilePtr->getFileName()<< "\"" <<  std::endl;
     }
      // sourceFileName=string(CurrentPath)+"/"+sourceFileName;
     std::ifstream instream(sourceFileName.c_str());
     std::string instring;

     if (!instream.is_open())
        {
          std::cerr << "Could not open input file: " << sourceFileName << std::endl;
          ROSE_ASSERT(false);
        }

     instream.unsetf(std::ios::skipws);
     instring = std::string(std::istreambuf_iterator<char>(instream.rdbuf()),
         std::istreambuf_iterator<char>());

  // The template boost::wave::cpplexer::lex_token<> is the token type to be used by the Wave library.
     ::token_type x;

  // The template boost::wave::cpplexer::lex_iterator<> is the lexer type to
  // be used by the Wave library.
     typedef boost::wave::cpplexer::lex_iterator< ::token_type> lex_iterator_type;

  // This is the resulting context type to use. The first template parameter
  // should match the iterator type to be used during construction of the
  // corresponding context object (see below).
     typedef boost::wave::context<std::string::iterator,lex_iterator_type, boost::wave::iteration_context_policies::load_file_to_string, advanced_preprocessing_hooks > context_type;

  // The preprocessor iterator shouldn't be constructed directly. It is 
  // to be generated through a wave::context<> object. This wave:context<> 
  // object is to be used additionally to initialize and define different 
  // parameters of the actual preprocessing (not done here).
  //
  // The preprocessing of the input stream is done on the fly behind the 
  // scenes during iteration over the context_type::iterator_type stream.
    
     ROSE_ASSERT(sourceFileName.size()>0);
     ROSE_ASSERT(instring.begin() != instring.end());
     context_type ctx (instring.begin(), instring.end(), sourceFileName.c_str());
  //     std::cout << "Current file name: " << get_current_filename()

// This get_hooks() member function was added by the Author of boost-wave to handle 
  // a problem pointed out by Andreas.
     AttributeListMap attributeListMap(sageFilePtr);
     ctx.get_hooks().attributeListMap = &attributeListMap;

  // printf ("specify the languge \n");

  // Preserve comments through preprocessing so that the output token-stream
  // contains the comments.
     if(SgProject::get_verbose() >= 1)
         std::cout << "BEFORE ADDING PREDEFINES" << std::endl;

     if( sageFilePtr->get_C_only() == true){
         //Tentaive support for C. For now treat it like C99 since Wave does not
         //have an option for just C.
         ctx.set_language(boost::wave::support_c99);
     }else if( sageFilePtr->get_C99_only() == true ){
         ctx.set_language(boost::wave::support_c99);
     }else{
         ctx.set_language(boost::wave::support_cpp);
     }

     ctx.set_language(boost::wave::enable_long_long(ctx.get_language()));
     ctx.set_language(boost::wave::enable_preserve_comments(ctx.get_language()));
     ctx.set_language(boost::wave::enable_variadics(ctx.get_language()));
  // Force a specific file to be included before all others

    if( sageFilePtr->get_C_only() == true){
         //Tentaive support for C. For now treat it like C99 since Wave does not
         //have an option for just C.
         ctx.add_macro_definition(std::string("ROSE_CPP_MODE=0"),true);
     }else if( sageFilePtr->get_C99_only() == true ){
         ctx.add_macro_definition(std::string("ROSE_CPP_MODE=0"),true);
     }else{
         ctx.add_macro_definition(std::string("ROSE_CPP_MODE=1"),true);


     }

     if(SgProject::get_verbose() >= 1)
         std::cout << "MIDDLE OF ADDING PREDEFINES" << std::endl;

     for(std::vector<std::string>::iterator it_beg = macroList.begin();
            it_beg != macroList.end(); ++it_beg){
	          
	     if(SgProject::get_verbose() >= 1)			  
              	    std::cout << "Predef macro:\"" << *it_beg << "\""<<std::endl;
	    if((*it_beg)!="")
            ctx.add_macro_definition(*it_beg,true);

     }
          
     if(SgProject::get_verbose() >= 1)
           std::cout << "AFTER ADDING PREDEFINES" << std::endl;


  // Add include paths specified on commandline to the context object 
     std::vector<string>::const_iterator firstInclude = includeSpecifierlist.begin();
     std::vector<std::string>::const_iterator lastInclude = includeSpecifierlist.end();

                                 
     if(SgProject::get_verbose() >= 1)
         printf ("Adding the /usr/include/ file \n");

  // DQ (4/7/2006): Not sure we want to do this, if we did want to do so then it should 
  // be in the list of EDG as well and in which case, what order should it be placed?

     string internalIncludePaths[]          = CXX_INCLUDE_STRING;
     //if(SgProject::get_verbose() >= 1)
     //      std::cout << "INTERNAL INCLUDE PATHS " << CXX_INCLUDE_STRING << std::endl;

     vector<string> internalIncludePathList(internalIncludePaths, internalIncludePaths + sizeof(internalIncludePaths)/sizeof(string));
  //internalIncludePathList.push_back("-I"+string(CurrentPath)+"/");


     string includeBase = findRoseSupportPathFromBuild("include-staging", "include");
     for (vector<string>::iterator i = internalIncludePathList.begin(); i != internalIncludePathList.end(); i++)
       {
           ROSE_ASSERT (!i->empty());
           string fullPath = (*i)[0] == '/' ? *i : (includeBase + "/" + *i);
           
           ctx.add_sysinclude_path(fullPath.c_str()); 
        }

     std::string sys_include = "/usr/include/";
     ctx.add_sysinclude_path(sys_include.c_str());

     if(SgProject::get_verbose() >= 1)
          printf ("DONE: Adding the /usr/include/ file \n");

     while(firstInclude != lastInclude)
        {
          string includeDir=(*firstInclude).substr(2,(*firstInclude).length());
          ctx.add_sysinclude_path((*firstInclude).substr(2,(*firstInclude).length()).c_str()); 
          ctx.add_include_path((*firstInclude).c_str());
          ++firstInclude;
        }



  // variable needed by the program to account for the bug in the column 
  // position of value expressions within ROSE.
     context_type::token_type lastOperatorToken(boost::wave::T_RIGHTPAREN,")",boost::wave::util::file_position_type("",0,0));
     ;
  // Attaching an attribute list to the current hooks object so that 
  // preprocessing infos can be extracted
     map<std::string,ROSEAttributesList*> currentMapOfAttributes;

    // std::string x,y;
  // current file position is saved for exception handling
     boost::wave::util::file_position_type current_position;

  // accessFunctionsList = NodeQuery::querySubTree (project,&queryFloatDoubleValExp);
     accessFunctionsList = NodeQuery::querySubTree (sageFilePtr,&queryFloatDoubleValExp);


     std::cerr << "For some reason we have " << std::endl;
  //Locate all value expression with Wave and set the string value of the
  //corresponding value expressions within the ROSE AST to the string value found
  //by Wave.
     context_type::iterator_type first = ctx.begin();
     context_type::iterator_type last = ctx.end();

  // analyze the input file, print out the preprocessed hooks
     if(SgProject::get_verbose() >= 1)
         printf ("Adding the preinclude file \n");

     ROSE_ASSERT(preincludeList.size() == 1);
     for (vector<string>::reverse_iterator i = preincludeList.rbegin(); i != preincludeList.rend(); ++i)
        {
          vector<string>::reverse_iterator copyOf_i = i;
          copyOf_i++;

          if(SgProject::get_verbose() >= 1)
               printf ("Adding preinclude file = %s \n",i->c_str());

       // DQ (4/7/2006): This currently fails
          first.force_include( i->c_str(), copyOf_i == preincludeList.rend() );
//          first.force_include( i->c_str(), false);
          
          if(SgProject::get_verbose() >= 1)
               printf ("DONE: Adding preinclude file = %s \n",i->c_str());
        }
#if 1

                                 
     if(SgProject::get_verbose() >= 1)
        printf ("DONE: Adding the preinclude file \n");

     try{ 
          while (first != last) {
               using namespace boost::wave;
               try{
               current_position = (*first).get_position();
          if(first->get_position().get_file()!="<built-in>"){
                    //std::cout << first->get_position().get_file() << " l" << first->get_position().get_line()
                      //        << " " << (*first).get_value() << std::endl;
               }


               token_id id = token_id(*first);
            //Attach comments found by Wave to the AST
               if((T_CCOMMENT == id) | (T_CPPCOMMENT == id)){
                    attributeListMap.found_directive(*first);
               }

               wave_tokenStream.push_back(*first);
               first++;
	       }
	       catch (boost::wave::cpp_exception &e) {
		       // some preprocessing error
             // This is a problem for using compass with emacs (see testEmacs.C).
		       // cerr  << "WAVE 1: " << e.file_name() << "(" << e.line_no() << "): "
				 //       << e.description() << endl;
	       }
	       catch (boost::wave::cpplexer::lexing_exception &e) {
		       // some lexing error
		       cerr 
			       << "WAVE 2:" << e.file_name() << "(" << e.line_no() << "): "
				       << e.description() << endl;
	       }

          }
     }
     catch (boost::wave::cpp_exception &e) {
       // some preprocessing error
          cerr 
            << "WAVE 3: " << e.file_name() << "(" << e.line_no() << "): "
              << e.description() << endl;
     }
     catch (boost::wave::cpplexer::lexing_exception &e) {
       // some lexing error
          cerr 
            << "WAVE 4:" << e.file_name() << "(" << e.line_no() << "): "
              << e.description() << endl;
     }
     catch (std::exception &e) {
       // use last recognized token to retrieve the error position
          cerr 
            << "WAVE 5:" << current_position.get_file() 
            << "(" << current_position.get_line() << "): "
              << "exception caught: " << e.what()
                << endl;
     }
     catch (...) {
       // use last recognized token to retrieve the error position
          cerr 
            << "WAVE 6:" << current_position.get_file() 
            << "(" << current_position.get_line() << "): "
              << "unexpected exception caught." << endl;
     }
     attributeListMap.attach_line_to_macro_call();

#if 0
  // Get all SgFile nodes in the AST so that the attributes can be attached to them
  // std::vector<SgNode*> sgFileList =     NodeQuery::querySubTree(project,&findNodes);
     std::vector<SgNode*> sgFileList =     NodeQuery::querySubTree(sageFilePtr,&findNodes);

  // Attache the map of attributes belonging to the current file to the AST
     for(std::vector<SgNode*>::iterator it = sgFileList.begin(); it != sgFileList.end(); ++it)
        {
          SgFile* sgFile = isSgFile(*it);
          attachPreprocessingInfo(sgFile,&attributeListMap.currentMapOfAttributes);
        }
#else

    //AS(01/04/07) Create a global map of filenames to PreprocessingInfo*'s as it is inefficient
    //to get this by a traversal of the AST
 
     for(AttributeListMap::attribute_map_type::iterator  it_files = attributeListMap.currentMapOfAttributes.begin();
                     it_files != attributeListMap.currentMapOfAttributes.end();
                     ++it_files){

          std::string filename2 = it_files->first;
          ROSEAttributesList*  attrList = it_files->second;
          //mapFilenameToAttributes[filename2] = attrList->getList();

          std::vector<PreprocessingInfo*>* preproc_info = new std::vector<PreprocessingInfo*>();
          mapFilenameToAttributes[filename2] = preproc_info;

          for(std::vector<PreprocessingInfo*>::iterator it_preproc = attrList->getList().begin();
                          it_preproc != attrList->getList().end();
                          ++it_preproc){
               preproc_info->push_back(*it_preproc);



          };

	  if(SgProject::get_verbose() >= 1){

		  std::cout << "Size of vector: " << preproc_info->size() << std::endl;
		  std::cout << "Iterating over filename:" << filename2 << std::endl;
	  }
     }

     if(SgProject::get_verbose() >= 1)
          std::cout << "Size of mapFilenameToAttributes:" << mapFilenameToAttributes.size() << std::endl;


  // DQ (and AS (4/6/2006): Call this for the single input file
     attachPreprocessingInfo(sageFilePtr,&attributeListMap.currentMapOfAttributes);
#endif


#if 0
     printf ("Ending at base of attachPreprocessingInfo(SgFile*) \n");
     ROSE_ABORT();
#endif

  // else for conditional use of Boost-Wave
#endif
     }else
     
//#endif
     {
//#else
  // DQ (7/6/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("AST Comment and CPP Directive Processing (not using Wave):");

  // DQ (4/7/2006): Older version of code (not using boost-wave)

  // Dummy attribute (nothing is done here since this is an empty class)
     AttachPreprocessingInfoTreeTraversalInheritedAttrribute inh;

  // Make sure that the argument is not a NULL pointer
     ROSE_ASSERT(sageFilePtr);

  // DQ (4/19/2006): Now supporting either the collection or ALL comments and CPP directives 
  // into header file AST nodes or just the collection of the comments and CPP directives 
  // into the source file.
  // printf ("sageFilePtr->get_collectAllCommentsAndDirectives() = %s \n",sageFilePtr->get_collectAllCommentsAndDirectives() ? "true" : "false");
     if (sageFilePtr->get_collectAllCommentsAndDirectives() == true)
        {
          printf ("Collect all comments and CPP directives into the AST (from header files) \n");

       // Create tree traversal object for attaching the preprocessing information
          AttachAllPreprocessingInfoTreeTrav tt(sageFilePtr);

       // Run tree traversal on specified source file
          tt.traverse(sageFilePtr, inh);

       // DQ (3/30/2006): Commented this out with help from Lingxiao.
       // This gets the list of header files using EDG's -M (--dependencies) option.
       // This list is provided in order so that header files without IR nodes
       // have have their comments and CPP directive processed for inclusion into
       // the AST (important if such a header file included "#if 0" only, for example).
       // tt.attach_left_info();
#if PRINT_DEVELOPER_WARNINGS
          printf ("Skipping possible header files that contain no IR nodes (the comments and CPP directives in them will not be extracted) \n");
#endif

#if 0
       // DQ (10/27/2007): Output debugging information
          if ( SgProject::get_verbose() >= 0 )
             {
               tt.display("Output from collecting ALL comments and CPP directives (across source and header files)");
             }
#endif
        }
       else
        {
       // DQ (4/13/2007): Introduce tracking of performance of ROSE.
          TimingPerformance timer ("AST Comment and CPP Directive Processing (traversal to attach):");

       // Make sure that the argument is not a NULL pointer
          ROSE_ASSERT(sageFilePtr);

       // Create tree traversal object for attaching the preprocessing information (not using Wave)
          AttachPreprocessingInfoTreeTrav tt;

       // Run tree traversal on specified source file
          tt.traverseWithinFile(sageFilePtr,inh);

#if 1
       // DQ (10/27/2007): Output debugging information
          if ( SgProject::get_verbose() >= 3 )
             {
               tt.display("Output from collecting comments and CPP directives in source file only");
             }
#endif
        }

     }
  // endif for USE_ROSE_BOOST_WAVE_SUPPORT
//#endif
   }



// DQ (10/27/2007): Added display function to output information gather durring the collection of 
// comments and CPP directives across all files.
void
AttachPreprocessingInfoTreeTrav::display(const std::string & label) const
   {
  // Output internal information

     printf ("Inside of AttachPreprocessingInfoTreeTrav::display(%s) \n",label.c_str());

     printf ("   previousLocNodePtr            = %p = %s \n",previousLocNodePtr,previousLocNodePtr != NULL ? previousLocNodePtr->class_name().c_str() : "NULL");
     printf ("   currentListOfAttributes       = %p \n",currentListOfAttributes);
     printf ("   sizeOfCurrentListOfAttributes = %d \n",sizeOfCurrentListOfAttributes);
     printf ("   currentFileNameId             = %d \n",currentFileNameId);
     printf ("   use_Wave                      = %s \n",use_Wave ? "true" : "false");
     printf ("   start_index                   = %d \n",start_index);
     printf ("   currentMapOfAttributes        = %p \n",currentMapOfAttributes);

     if (currentListOfAttributes != NULL)
        {
          printf ("   currentListOfAttributes.size() = %d \n",currentListOfAttributes->size());
          currentListOfAttributes->display("Called from AttachPreprocessingInfoTreeTrav::display()");
        }

     if (currentMapOfAttributes != NULL)
        {
          std::map<std::string,ROSEAttributesList*>::iterator i = currentMapOfAttributes->begin();
          while (i != currentMapOfAttributes->end())
             {
               string filename = i->first;
               size_t numberOfAttributes = i->second->size();
               printf ("      filename = %s  has %zu attributes \n",filename.c_str(),numberOfAttributes);

               i->second->display("Called from AttachPreprocessingInfoTreeTrav::display()");

               i++;
             }
        }
   }


// EOF








