#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rose.h>
#include "TransformationBase.h"

// Static data members
// static TargetTemplate         *targetTemplateData;
// static TransformationTemplate *transformationTemplateData;
// static SgStatement *targetTemplateFunction;
// static SgStatement *transformationTemplateFunction;
// static SgStatementPtrList currentSgBasicBlockStack;

ROSE_TransformationBase::TargetTemplate*         ROSE_TransformationBase::targetTemplateData              = NULL;
ROSE_TransformationBase::TransformationTemplate* ROSE_TransformationBase::transformationTemplateData      = NULL;
SgStatement*            ROSE_TransformationBase::targetTemplateFunction          = NULL;
SgStatement*            ROSE_TransformationBase::transformationTemplateFunction  = NULL;
SgStatementPtrList      ROSE_TransformationBase::currentSgBasicBlockStack;


SgVariableDeclaration* ROSE_TransformationBase::subscriptFunctionDeclarationStatement[ROSE_MAX_ARRAY_DIMENSION];
SgFunctionCallExp*     ROSE_TransformationBase::subscriptFunctionDeclarationExpression[ROSE_MAX_ARRAY_DIMENSION];


ROSE_TransformationBase::~ROSE_TransformationBase()
   {
  // Destructor

  // We will not worry (yet) about the memory leak associated with SgFile not being properly deleted
  // delete programFile;
     programFile = NULL;

  // RoseStatementDataBase = NULL;
   }

ROSE_TransformationBase::ROSE_TransformationBase( SgFile *file )
   {
  // Constructor
     programFile = file;

  // Initialization
     targetTemplateFunction         = NULL;
     transformationTemplateFunction = NULL;

#if 0
  // It is annoying that this must be initialized with the relative path
  // but we can fix that later (I guess)!
  // headerFileName = "../src/Transform_3/ROSE_TRANSFORMATION_SOURCE_3.h";
     headerFileName = "ROSE_TRANSFORMATION_SOURCE_3.h";
#if 1
     printf ("In SimpleArrayAssignment constructor: headerFileName = %s \n",headerFileName);
#endif
#endif
   }

#if 0
ROSE_TransformationBase::ROSE_TransformationBase( RoseStatement * roseStatement )
   {
  // Constructor
     RoseStatementDataBase = roseStatement;
   }
#endif


SgFile *
ROSE_TransformationBase::getProgramFile()
   {
  // Return the program source file (the users application source file) 
  // which we will be doing transformations upon
     ROSE_ASSERT (programFile != NULL);
     return programFile;
   }

ROSE_StatementBlock*
ROSE_TransformationBase::pass ( ROSE_StatementBlock* roseProgramTree )
   {
  // We might want to make this a pure virtual function at some point!
     printf ("Virtual Function Called in Base Class: ROSE_TransformationBase::pass () \n");
     ROSE_ABORT();

     ROSE_ASSERT (roseProgramTree == NULL);

     return NULL;
   }

SgScopeStatement*
ROSE_TransformationBase::getCurrentScope ()
   {
     ROSE_ASSERT (currentSgBasicBlockStack.size() >= 0);
#if ROSE_INTERNAL_DEBUG
     if (ROSE_DEBUG > 3)
        {
          printf ("In pushCurrentBlock(): currentSgBasicBlockStack.size() = %d \n",currentSgBasicBlockStack.size());    
        }
#endif

  // return currentSgBasicBlockStack.front();
#ifdef USE_SAGE3
     return (SgScopeStatement*) (*currentSgBasicBlockStack.begin());
#else
     return (SgScopeStatement*) ((*currentSgBasicBlockStack.begin()).irep());
#endif     
   }

void
ROSE_TransformationBase::pushCurrentScope ( SgScopeStatement *scopeStatement )
   {
     ROSE_ASSERT (scopeStatement != NULL);
     ROSE_ASSERT (currentSgBasicBlockStack.size() >= 0);
     currentSgBasicBlockStack.push_front(scopeStatement);
#if ROSE_INTERNAL_DEBUG
     if (ROSE_DEBUG > 3)
        {
          printf ("In pushCurrentBlock(): currentSgBasicBlockStack.size() = %d \n",currentSgBasicBlockStack.size());    
        }
#endif
   }

void
ROSE_TransformationBase::popCurrentScope  ( SgScopeStatement *scopeStatement )
   {
     ROSE_ASSERT (scopeStatement != NULL);
     ROSE_ASSERT (currentSgBasicBlockStack.size() > 0);
#if ROSE_INTERNAL_DEBUG
     if (ROSE_DEBUG > 3)
        {
          printf ("In popCurrentBlock(): currentSgBasicBlockStack.size() = %d \n",currentSgBasicBlockStack.size());    
        }
#endif

  // Get the SgStatement from the SgStatementPrt using the irep() member function
  // ROSE_ASSERT (scopeStatement == (*currentSgBasicBlockStack.begin()).irep());
#if ROSE_INTERNAL_DEBUG
     if (ROSE_DEBUG > 3)
        {
          printf ("COMMENTED OUT IMPORTANT TEST! \n");
        }
#endif

     currentSgBasicBlockStack.pop_back();
   }


void 
ROSE_TransformationBase::makeStatementNullStatement (SgStatement* stmt )
   {
     SgExprStatement* exprStmt = isSgExprStatement( stmt );
     ROSE_ASSERT (exprStmt != NULL);

     SgExpression *expr = exprStmt->get_the_expr();
     ROSE_ASSERT (expr != NULL);

  // DQ (10/17/2001): Changed filename to default_file_name to make consistant with other transformations
  // Sg_File_Info* fileInfo = new Sg_File_Info("NULL_Statement",1,1);
  // Sg_File_Info* fileInfo = new Sg_File_Info("",0,0);
     Sg_File_Info* fileInfo = new Sg_File_Info("default_file_name",1,1);
     ROSE_ASSERT (fileInfo != NULL);

     SgExpression* newExpression = new SgIntVal(fileInfo,0);
     ROSE_ASSERT (newExpression != NULL);

  // *expr = *newExpression;
     exprStmt->set_the_expr(newExpression);

  // showSgExpression(cout,expr);
  // showSgStatement(cout,stmt,"SimpleArrayAssignment::makeStatementNullStatement(SgStatement*)");

  // printf ("Exiting at base of SimpleArrayAssignment::makeStatementNullStatement (SgStatement* stmt )! \n");
  // ROSE_ABORT();
   }


SgStatement*
ROSE_TransformationBase::buildNullStatement()
   {
     SgStatement* returnStatement = NULL;

  // DQ (10/17/2001): Changed filename to default_file_name to make consistant with other transformations
  // Sg_File_Info* fileInfo = new Sg_File_Info("NULL_Statement",1,1);
     Sg_File_Info* fileInfo = new Sg_File_Info("default_file_name",1,1);
     ROSE_ASSERT (fileInfo != NULL);

     SgExpression* newExpression = new SgIntVal(fileInfo,0);
     ROSE_ASSERT (newExpression != NULL);

     SgExprStatement* newExprStatement = new SgExprStatement (fileInfo,newExpression);
     ROSE_ASSERT (newExprStatement != NULL);

     newExprStatement->post_construction_initialization();
     returnStatement = newExprStatement;

     return returnStatement;
   }


Sg_File_Info*
ROSE_TransformationBase::buildSageFileInfo ( char* filename )
   {
  // The second and third parameters are the line number and column number of the statement
  // if the line number is zero then the "#line filename" is not output by the unparser!
  // Sg_File_Info *fileInfo = new Sg_File_Info("CUSTOM BUILT BY ROSE",1,0);
     Sg_File_Info *fileInfo = new Sg_File_Info(filename,0,0);
     ROSE_ASSERT (fileInfo != NULL);

     return fileInfo;
   }

void 
ROSE_TransformationBase::readTargetAndTransformationTemplates ()
   {
  // This function traverses the statements within the program tree,
  // identifies those statements that are a part of the ROSE input file,
  // evaluates them for the transformation represented by this 
  // class (SimpleArrayAssignment), and transforms the statements.

     SgFile *file          = getProgramFile();
     char   *inputFileName = ROSE::getFileName(file);
  // const Boolean OUTPUT_SHOWFILE_TO_FILE = TRUE;

#ifdef ROSE_INTERNAL_DEBUG
     if (ROSE_DEBUG > 0)
          printf ("Inside of ROSE_TransformationBase::readTargetAndTransformationTemplates() for file = %s \n",inputFileName);
#endif

     char outputFilename[256];
     sprintf(outputFilename,"%s.roseShow_roseTemplates",inputFileName);

     if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
          printf ("In ROSE_TransformationBase::readTargetAndTransformationTemplates: outputFilename = %s \n",outputFilename);

     fstream ROSE_ShowFile(outputFilename,ios::out);

  // Verify that the internal references to the program tree are not setup yet
     ROSE_ASSERT (templatesDataSetup() == FALSE);

  // Select an output stream for the program tree display (cout or <filename>.C.roseShow)
  // Macro OUTPUT_SHOWFILE_TO_FILE is defined in the transformation_1.h header file
     ostream & outputStream = (OUTPUT_SHOWFILE_TO_FILE ? ((ostream&) ROSE_ShowFile) : ((ostream&) cout));

  // We start by getting the SgScopeStatement and the using its iterator
  // to go through the statements in that scope.
     SgScopeStatement *globalScope = (SgScopeStatement *)(&(file->root()));
     ROSE_ASSERT (globalScope != NULL);

     for (SgStatement::iterator s_iter = globalScope->begin();
          s_iter != globalScope->end(); s_iter++)
        {
#if 0
          printf ("ROSE::getFileName(*s_iter) = %s transformationHeaderFile() = %s \n",ROSE::getFileName(*s_iter),transformationHeaderFile());
#endif

       // First find the target of our transformation and its transformation template
       // We might want this as a separate pass over the program tree!
          if (ROSE::isSameName(transformationHeaderFile(), ROSE::getFileNameWithoutPath(*s_iter)) == TRUE)
             {
            // Now setup the internal data with pointers to the SAGE program tree where the 
            // target and transformation template are located so we can use them.
            // WARNING: This function increments the s_iter iterator three times as part of its semantics!!!!
               setupInternalTargetAndTransformationstemplates (s_iter);
               ROSE_ASSERT (templatesDataSetup() == TRUE);

            // printf("EXITING: FOUND TRANSFORMATION TEMPLATES! \n");
            // ROSE_ABORT();
	     }

        } // end of "for" loop

  // Make sure this was setup at some point
     ROSE_ASSERT (templatesDataSetup() == TRUE);

     if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
          printf("Done with ROSE_TransformationBase::readTargetAndTransformationTemplates() \n");
  // ROSE_ABORT();
   }

