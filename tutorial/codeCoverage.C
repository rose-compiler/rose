// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// Specifically it shows the design of a transformation to instrument source code, placing source code
// at the top and bottome of each basic block.

#include "rose.h"

using namespace std;

/*
   Design of this code.
      Inputs: source code (file.C)
      Outputs: instrumented source code (rose_file.C and file.o)
 
   Properties of instrumented source code:
       1) added declaration for coverage support function
          (either forward function declaration or a #include to include a header file).
       2) Each function in the source program is instrumented to include a call to the
          coverage support function/
*/



// Global variables so that the global function declaration can be reused to build
// each function call expression in the AST traversal to instrument all functions.
SgFunctionDeclaration* globalFunctionDeclaration = NULL;
SgFunctionType*        globalFunctionType        = NULL;
SgFunctionSymbol* functionSymbol = NULL;

// Simple ROSE traversal class: This allows us to visit all the functions and add
// new code to instrument/record their use.
class SimpleInstrumentation : public SgSimpleProcessing
   {
     public:
       // required visit function to define what is to be done
          void visit ( SgNode* astNode );
   };


// Code to build function declaration: This declares Shmuel's function call which
// will be inserted (as a function call) into each function body of the input
// application.
void buildFunctionDeclaration(SgProject* project)
   {
  // *****************************************************
  // Create the functionDeclaration
  // *****************************************************

  // SgGlobal* globalScope = project->get_file(0).get_root();
     SgSourceFile* sourceFile = isSgSourceFile(project->get_fileList()[0]);
     ROSE_ASSERT(sourceFile != NULL);
     SgGlobal* globalScope = sourceFile->get_globalScope();
     ROSE_ASSERT(globalScope != NULL);

     Sg_File_Info * file_info            = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
     SgType * function_return_type       = new SgTypeVoid();

     SgName function_name                        = "coverageTraceFunc1";
     SgFunctionType * function_type              = new SgFunctionType(function_return_type,false);
     SgFunctionDeclaration * functionDeclaration = new SgFunctionDeclaration(file_info, function_name, function_type);

  // DQ (9/8/2007): Fixup the defining and non-defining declarations
     ROSE_ASSERT(functionDeclaration->get_definingDeclaration() == NULL);
     functionDeclaration->set_definingDeclaration(functionDeclaration);
     ROSE_ASSERT(functionDeclaration->get_definingDeclaration()         != NULL);
     ROSE_ASSERT(functionDeclaration->get_firstNondefiningDeclaration() != functionDeclaration);

  // DQ (9/8/2007): We have not build a non-defining declaration, so this should be NULL.
     ROSE_ASSERT(functionDeclaration->get_firstNondefiningDeclaration() == NULL);

  // DQ (9/8/2007): Need to add function symbol to global scope!
     printf ("Fixing up the symbol table in scope = %p = %s for function = %p = %s \n",globalScope,globalScope->class_name().c_str(),functionDeclaration,functionDeclaration->get_name().str());
     functionSymbol = new SgFunctionSymbol(functionDeclaration);
     globalScope->insert_symbol(functionDeclaration->get_name(),functionSymbol);
     ROSE_ASSERT(globalScope->lookup_function_symbol(functionDeclaration->get_name()) != NULL);

  // ********************************************************************
  // Create the InitializedName for a parameter within the parameter list
  // ********************************************************************
     SgName var1_name = "textString";

     SgTypeChar * var1_type            = new SgTypeChar();
     SgPointerType *pointer_type       = new SgPointerType(var1_type);
     SgInitializer * var1_initializer  = NULL;
     SgInitializedName *var1_init_name = new SgInitializedName(var1_name, pointer_type, var1_initializer, NULL);
     var1_init_name->set_file_info(Sg_File_Info::generateDefaultFileInfoForTransformationNode());

  // Insert argument in function parameter list
     ROSE_ASSERT(functionDeclaration != NULL);
     ROSE_ASSERT(functionDeclaration->get_parameterList() != NULL);

     ROSE_ASSERT(functionDeclaration->get_parameterList() != NULL);
     functionDeclaration->get_parameterList()->append_arg(var1_init_name);

  // Set the parent node in the AST (this could be done by the AstPostProcessing
     functionDeclaration->set_parent(globalScope);

  // Set the scope explicitly (since it could be different from the parent?)
  // This can't be done by the AstPostProcessing (unless we relax some constraints)
     functionDeclaration->set_scope(globalScope);

  // If it is not a forward declaration then the unparser will skip the ";" at the end (need to fix this better)
     functionDeclaration->setForward();
     ROSE_ASSERT(functionDeclaration->isForward() == true);

  // Mark function as extern "C"
     functionDeclaration->get_declarationModifier().get_storageModifier().setExtern();
     functionDeclaration->set_linkage("C");  // This mechanism could be improved!

     globalFunctionType = function_type;
     globalFunctionDeclaration = functionDeclaration;

  // Add function declaration to global scope!
     globalScope->prepend_declaration(globalFunctionDeclaration);

  // functionSymbol = new SgFunctionSymbol(globalFunctionDeclaration);
  // All any modifications to be fixed up (parents etc)
  // AstPostProcessing(project); // This is not allowed and should be fixed!
     AstPostProcessing(globalScope);

   }

#if 0
// DQ (12/1/2005): This version of the visit function handles the special case of 
// instumentation at each function (a function call at the top of each function).
// At IBM we modified this to be a version which instrumented every block.

void
SimpleInstrumentation::visit ( SgNode* astNode )
   {
     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(astNode);
     SgFunctionDefinition*  functionDefinition  = functionDeclaration != NULL ? functionDeclaration->get_definition() : NULL;
     if (functionDeclaration != NULL && functionDefinition != NULL)
        {
       // It is up to the user to link the implementations of these functions link time
          string functionName = functionDeclaration->get_name().str();
          string fileName     = functionDeclaration->get_file_info()->get_filename();

       // Build a source file location object (for construction of each IR node)
       // Note that we should not be sharing the same Sg_File_Info object in multiple IR nodes.
          Sg_File_Info * file_info = Sg_File_Info::generateDefaultFileInfoForTransformationNode();

          SgFunctionSymbol* functionSymbol        = new SgFunctionSymbol(globalFunctionDeclaration);
          SgFunctionRefExp* functionRefExpression = new SgFunctionRefExp(file_info,functionSymbol,globalFunctionType);
          SgExprListExp* expressionList           = new SgExprListExp(file_info);

          string converageFunctionInput = functionName + string (" in ") + fileName;
          SgStringVal* functionNameStringValue = new SgStringVal(file_info,(char*)converageFunctionInput.c_str());
          expressionList->append_expression(functionNameStringValue);
          SgFunctionCallExp* functionCallExp   = new SgFunctionCallExp(file_info,functionRefExpression,expressionList,globalFunctionType);

       // create an expression type
          SgTypeVoid* expr_type = new SgTypeVoid();

       // create an expression root
       // SgExpressionRoot * expr_root = new SgExpressionRoot(file_info,functionCallExp,expr_type);

       // create an expression statement
          SgExprStatement* new_stmt = new SgExprStatement(file_info,functionCallExp);

       // expr_root->set_parent(new_stmt);
       // new_stmt->set_expression_root(expr_root);
       // functionCallExp->set_parent(new_stmt->get_expression_root());

          functionCallExp->set_parent(new_stmt);

       // insert a statement into the function body
          functionDefinition->get_body()->prepend_statement(new_stmt);

#if 0
       // This shows the alternative use of the ROSE Rewrite Mechanism to do the same thing!
       // However, the performance is not as good as the version written above (more directly 
       // building the IR nodes).

       // string codeAtTopOfBlock = "void printf(char*); printf (\"FUNCTION_NAME in FILE_NAME \\n\");";
          string codeAtTopOfBlock = "coverageTraceFunc1(\"FUNCTION_NAME in FILE_NAME\");";

          string functionTarget   = "FUNCTION_NAME";
          string fileTarget       = "FILE_NAME";

          codeAtTopOfBlock.replace(codeAtTopOfBlock.find(functionTarget),functionTarget.size(),functionName);
          codeAtTopOfBlock.replace(codeAtTopOfBlock.find(fileTarget),fileTarget.size(),fileName);

       // printf ("codeAtTopOfBlock = %s \n",codeAtTopOfBlock.c_str());
          printf ("%s in %s \n",functionName.c_str(),fileName.c_str());

       // Insert new code into the scope represented by the statement (applies to SgScopeStatements)
          MiddleLevelRewrite::ScopeIdentifierEnum scope = MidLevelCollectionTypedefs::StatementScope;

          SgBasicBlock* functionBody = functionDefinition->get_body();
          ROSE_ASSERT(functionBody != NULL);

       // Insert the new code at the top of the scope represented by block
          MiddleLevelRewrite::insert(functionBody,codeAtTopOfBlock,scope,MidLevelCollectionTypedefs::TopOfCurrentScope);
#endif
        }
   }
#endif

void
SimpleInstrumentation::visit ( SgNode* astNode ) {
   SgBasicBlock *block = NULL;
   block = isSgBasicBlock(astNode);
   if (block != NULL) {
       // It is up to the user to link the implementations of these functions link time
       Sg_File_Info *fileInfo = block->get_file_info();
       string fileName = fileInfo->get_filename();
       int lineNum = fileInfo->get_line();

       // Build a source file location object (for construction of each IR node)
       // Note that we should not be sharing the same Sg_File_Info object in multiple IR nodes.
       Sg_File_Info * newCallfileInfo = Sg_File_Info::generateDefaultFileInfoForTransformationNode();

       ROSE_ASSERT(functionSymbol != NULL);
       SgFunctionRefExp* functionRefExpression = new SgFunctionRefExp(newCallfileInfo,functionSymbol,globalFunctionType);
       SgExprListExp* expressionList           = new SgExprListExp(newCallfileInfo);

       string codeLocation = fileName + " " + StringUtility::numberToString(lineNum);
       SgStringVal* functionNameStringValue = new SgStringVal(newCallfileInfo,(char*)codeLocation.c_str());
       expressionList->append_expression(functionNameStringValue);
       SgFunctionCallExp* functionCallExp   = new SgFunctionCallExp(newCallfileInfo,functionRefExpression,expressionList,globalFunctionType);

    // create an expression type
    // SgTypeVoid* expr_type = new SgTypeVoid();
    // create an expression root
    // SgExpressionRoot * expr_root = new SgExpressionRoot(newCallfileInfo,functionCallExp,expr_type);

    // create an expression statement
       SgExprStatement* new_stmt = new SgExprStatement(newCallfileInfo,functionCallExp);

    // expr_root->set_parent(new_stmt);
    // new_stmt->set_expression(expr_root);
    // functionCallExp->set_parent(new_stmt->get_expression());
       functionCallExp->set_parent(new_stmt);

    // insert a statement into the function body
       block->prepend_statement(new_stmt);

   }
}

int
main ( int argc, char * argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

  // Build ROSE AST
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT(project != NULL);

  // Call function to declare function to be called to recode use of all functions in the AST
     buildFunctionDeclaration(project);

  // Call traversal to do instrumentation (put instumentation into the AST).
     SimpleInstrumentation treeTraversal;
     treeTraversal.traverseInputFiles ( project, preorder );

  // Generate Code and compile it with backend (vendor) compiler to generate object code
  // or executable (as specified on commandline using vendor compiler's command line).
  // Returns error code form compilation using vendor's compiler.
     return backend(project);
   }

