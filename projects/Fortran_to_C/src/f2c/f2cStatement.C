#include "f2c.h"
#include <algorithm>

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace Fortran_to_C;

extern bool isLinearlizeArray;
/******************************************************************************************************************************/
/* 
  Rename the output filename to .C file
  Replace the output file name to rose_f2c_*.C"
*/
/******************************************************************************************************************************/
void Fortran_to_C::translateFileName(SgFile* sourceFile)
{
  string outputFilename = sourceFile->get_sourceFileNameWithoutPath();
  size_t found;

  if (SgProject::get_verbose() > 2)
  {
      std::cout << "find file name: " << sourceFile->get_sourceFileNameWithoutPath()  << std::endl;
  }
  // Search for *.F or *.f, both are valid for Fortran.
  found = outputFilename.find(".F");
  if (found == string::npos) {
      found = outputFilename.find(".f");
      ROSE_ASSERT(found != string::npos);
    }
    
    outputFilename.replace(found, 2, ".c");
    outputFilename = "rose_f2c_" + outputFilename;
    if (SgProject::get_verbose() > 2)
    {
      std::cout << "New output name: " << outputFilename  << std::endl;
  }
  // set the output filename
  sourceFile->set_unparse_output_filename(outputFilename);
  sourceFile->set_outputLanguage(SgFile::e_C_output_language);
  sourceFile->set_C_only(true);
}



/******************************************************************************************************************************/
/* 
  Translate SgProgramHeaderStatement in Fortran into SgFunctionDeclaration in C.
  The main subroutine in Fortran will become main function in C.
*/
/******************************************************************************************************************************/
void Fortran_to_C::translateProgramHeaderStatement(SgProgramHeaderStatement* programHeaderStatement)
{
  // Get scopeStatement from SgProgramHeaderStatement
  SgScopeStatement* scopeStatement = programHeaderStatement->get_scope();
  ROSE_ASSERT(scopeStatement);
  
  // Get ParameterList and DecoratorList
  SgFunctionParameterList* functionParameterList = deepCopy(programHeaderStatement->get_parameterList());
  SgExprListExp* decoratorList = deepCopy(programHeaderStatement->get_decoratorList());
  
  // Reuse FunctionDefinition from Fortran programHeaderStatement
  SgFunctionDefinition* functionDefinition = programHeaderStatement->get_definition();
  
  // Get basicBlock from SgProgramHeaderStatement
  SgBasicBlock* basicBlock = functionDefinition->get_body();
  ROSE_ASSERT(basicBlock);

  SgSymbolTable* symbolTable = basicBlock->get_symbol_table();
  ROSE_ASSERT(symbolTable);
  
  // The main function return type is int
  SgType* mainType = SgTypeInt::createType();
  
  // Create SgFunctionDeclaration for C main function. Name must be "main".
  SgFunctionDeclaration* cFunctionDeclaration = buildDefiningFunctionDeclaration("main",
                                                                                 mainType,
                                                                                 functionParameterList,
                                                                                 scopeStatement,
                                                                                 decoratorList);
  
  // Remove original function symbol.  Keep the new function symbol with name of "main"
  SgFunctionSymbol* functionSymbol = isSgFunctionSymbol(scopeStatement->lookup_symbol(programHeaderStatement->get_name()));
  SgSymbolTable* globalSymbolTable = isSgSymbolTable(functionSymbol->get_parent());
  globalSymbolTable->remove(functionSymbol);
  functionSymbol->set_parent(NULL);
  delete(functionSymbol);
  
  // Setup the C function declaration.
  deepDelete(cFunctionDeclaration->get_definition());
  functionDefinition->set_parent(cFunctionDeclaration);
  cFunctionDeclaration->set_definition(functionDefinition);
  programHeaderStatement->set_definition(NULL);
  
  // Replace the SgProgramHeaderStatement with SgFunctionDeclaration.
  replaceStatement(programHeaderStatement,cFunctionDeclaration,true);
  
  /*
     Fortran has the implicit data type.  Variables with implicit data type will be found in the local basic block in
     AST tree. Translator has to link these variable declaration to the main basic block under function declaration. 
  */
  fixFortranSymbolTable(functionDefinition,false);
 
  programHeaderStatement->set_parent(NULL);
}  // End of Fortran_to_C::translateProgramHeaderStatement



/******************************************************************************************************************************/
/* 
  Translate SgProcedureHeaderStatement in Fortran into SgFunctionDeclaration in C.
  The subroutine in Fortran will become function in C.
*/
/******************************************************************************************************************************/
void Fortran_to_C::translateProcedureHeaderStatement(SgProcedureHeaderStatement* procedureHeaderStatement)
{
  // We only handles Fortran function and Fortran subroutine
  ROSE_ASSERT(procedureHeaderStatement->isFunction() || procedureHeaderStatement->isSubroutine());
  
  SgScopeStatement* scopeStatement = procedureHeaderStatement->get_scope();
  ROSE_ASSERT(scopeStatement);
  
  // Get ParameterList and DecoratorList
  SgFunctionParameterList* functionParameterList = deepCopy(procedureHeaderStatement->get_parameterList());
  SgExprListExp* decoratorList = deepCopy(procedureHeaderStatement->get_decoratorList());
  
  // Get the return variable from Fortran, name is same as function name.
  SgInitializedName* fortranReturnVar = procedureHeaderStatement->get_result_name();
  ROSE_ASSERT(fortranReturnVar);
  
  // Reuse FunctionDefinition from Fortran procedureHeaderStatement
  SgFunctionDefinition* functionDefinition = procedureHeaderStatement->get_definition();
  
  // Get basicBlock from SgProcedureHeaderStatement
  SgBasicBlock* basicBlock = functionDefinition->get_body();
  ROSE_ASSERT(basicBlock);
  SgSymbolTable* symbolTable = basicBlock->get_symbol_table();
  ROSE_ASSERT(symbolTable);
  
  // Get the function name from Fortran
  SgName functionName = procedureHeaderStatement->get_name();
  
  /* 
     Get the return function type from Fortran.
     Subroutine has only void return type. 
  */
  SgType* functionType = procedureHeaderStatement->get_type()->get_return_type(); 
  
  // Create SgFunctionDeclaration for C function. 
  SgFunctionDeclaration* cFunctionDeclaration = buildDefiningFunctionDeclaration(functionName,
                                                                                 functionType,
                                                                                 functionParameterList,
                                                                                 scopeStatement,
                                                                                 decoratorList);
  
  // Fix the function symbol declaration
  SgFunctionSymbol* functionSymbol = isSgFunctionSymbol(scopeStatement->lookup_symbol(procedureHeaderStatement->get_name()));
  functionSymbol->set_declaration(cFunctionDeclaration);
  
  // Setup the C function declaration.
  deepDelete(cFunctionDeclaration->get_definition());
  functionDefinition->set_parent(cFunctionDeclaration);
  cFunctionDeclaration->set_definition(functionDefinition);
  procedureHeaderStatement->set_definition(NULL);
  
  // If it is a Fortran function, add a return statement at the end.
  if(procedureHeaderStatement->isFunction())
  {
    // Create C return variable, based on the Fortran return variable name and type.
    const SgName fortranReturnVarName = fortranReturnVar->get_name();
  
    // Build VarRefExp for the return statement.  The return varaible has same name as the Fortran function.
    SgVarRefExp* VarRefExp = buildVarRefExp(fortranReturnVarName,basicBlock);
    ROSE_ASSERT(VarRefExp);
    
    // Add return statement to the end of C function.
    SgReturnStmt* returnStmt = buildReturnStmt(VarRefExp);
    ROSE_ASSERT(returnStmt);
    basicBlock->get_statements().insert(basicBlock->get_statements().end(),returnStmt);
    returnStmt->set_parent(basicBlock);
  }

  // Replace the SgProcedureHeaderStatement with SgFunctionDeclaration.
  replaceStatement(procedureHeaderStatement,cFunctionDeclaration,true);
  
  /*
     Fortran has the implicit data type.  Variables with implicit data type will be found in the local basic block in
     AST tree. Translator has to link these variable declaration to the main basic block under function declaration. 
  */
  fixFortranSymbolTable(functionDefinition,procedureHeaderStatement->isFunction());
  
  procedureHeaderStatement->set_parent(NULL);
  procedureHeaderStatement->set_result_name(NULL);
}  // End of Fortran_to_C::translateProcedureHeaderStatement

/******************************************************************************************************************************/
/* 
  This function creates declarations for all variables with implicit data type in Fortran.
  All local variables have to be declared in function's main basic block. 
*/
/******************************************************************************************************************************/
void Fortran_to_C::fixFortranSymbolTable(SgNode* root, bool hasReturnVariable)
{
  SgFunctionDefinition* functionDefinition = isSgFunctionDefinition(root);
  SgBasicBlock* basicBlock = functionDefinition->get_body();
  ROSE_ASSERT(basicBlock);
  SgSymbolTable* symbolTable = basicBlock->get_symbol_table();
  ROSE_ASSERT(symbolTable);
  
  // Node query for all basic blocks in functions
  Rose_STL_Container<SgNode*> basicBlockList = NodeQuery::querySubTree (root,V_SgBasicBlock);
  for (Rose_STL_Container<SgNode*>::iterator i = basicBlockList.begin(); i != basicBlockList.end(); i++)
  {
    SgBasicBlock* localBasicBlock = isSgBasicBlock(*i);
    ROSE_ASSERT(localBasicBlock);
    
    // Get the symbol table inside the basic block
    SgSymbolTable* localSymbolTable = localBasicBlock->get_symbol_table();
    ROSE_ASSERT(localSymbolTable);
    
    // Get all symbols from the local symbol table
    std::set<SgNode *> symbolList  = localSymbolTable->get_symbols();
    std::set<SgNode *>::iterator j = symbolList.begin();
    while(j != symbolList.end())
    {
      SgSymbol* symbol = isSgSymbol(*j);   
      ROSE_ASSERT(symbol);
      // Create declaration for all VariableSymbols, we might need to work on other symbols.
      SgVariableSymbol* localVariableSymbol = isSgVariableSymbol(symbol);
      if(localVariableSymbol != NULL)
      {
        // Get the initializedName from the variableSymbol
        SgInitializedName* localVariableInitailizedName = localVariableSymbol->get_declaration();
        ROSE_ASSERT(localVariableInitailizedName);
        
        /*
          This case is for the Fortran result_name.
          Fortran function by default has a return variable having the same name as the function.
          If this return variable isn't defined yet, there is no variableDeclaration for this
          return variable.  Therefore, the translator has to create one for it.
        */ 
        if(isSgVariableDeclaration(localVariableInitailizedName->get_parent()) == NULL)
        {
          // First case is the Fortran function, which has return type.
          if(hasReturnVariable)
          {
            // Create variableDeclaration based on the symbol's name and type.
            SgVariableDeclaration* variableDeclaration = buildVariableDeclaration(localVariableSymbol->get_name(), 
                                                                                  localVariableSymbol->get_type(), 
                                                                                  NULL, 
                                                                                  basicBlock);
            ROSE_ASSERT(variableDeclaration);
            // This is required for these variable declaration
            variableDeclaration->set_firstNondefiningDeclaration(variableDeclaration);
            
            // patch the required information for new variable declaration
            fixVariableDeclaration(variableDeclaration,basicBlock); 
            
            // Insert return variable declaration into beginning of basic block
            basicBlock->get_statements().insert(basicBlock->get_statements().begin(),variableDeclaration);
            variableDeclaration->set_parent(basicBlock);

            //localVariableInitailizedName->set_parent(variableDeclaration);

            /* 
              The new variableDeclaration creates a new InitializedName by default.
              It is better to take this new InitializedName to repalce the original one.
            */
            SgInitializedName* newInitializedName = variableDeclaration->get_decl_item(localVariableSymbol->get_name());
            localVariableSymbol->set_declaration(newInitializedName);
            delete(localVariableInitailizedName);
          }
          /* 
            The else case is for the Fortran subroutine, which has no return type.
            If there is a variable having name same as subroutine name, the Fortran parser will catch that syntax error.
          */
          else
          {
            if (SgProject::get_verbose() > 2)
            {
              std::cout<< "AST Fortran return_name " << localVariableSymbol->get_name() << "will not be declared" <<std::endl;
            }
            SgSymbolTable* parentSymbolTable = isSgSymbolTable(localVariableSymbol->get_parent());
            ROSE_ASSERT(parentSymbolTable);
            parentSymbolTable->remove(localVariableSymbol);
            deepDelete(localVariableSymbol);
          }
        }
        /*
          This case is for regular variables.
          In Fortran, variables with implicit data type have no declarations for them.  But you can still find the 
          variableDeclarations for them. The translator simply insert the variableDeclarations as the statements in 
          C function basic block.
          Before the insertion, check if the declaration is already in the statement list.
        */
        else
        {
          SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(localVariableInitailizedName->get_parent());
          SgStatementPtrList statementList = basicBlock->get_statements();
          Rose_STL_Container<SgStatement*>::iterator it;
          it = find(statementList.begin(),statementList.end(),variableDeclaration);
          if(it == statementList.end()){
            basicBlock->get_statements().insert(basicBlock->get_statements().begin(), variableDeclaration);
          }
        }
      }
      ++j;
    }  // end of while loop
  }   // end of i loop
}


/******************************************************************************************************************************/
/* 
  Translate FortranDoLoop in Fortran into ForStatement in C.
*/
/******************************************************************************************************************************/
void Fortran_to_C::translateFortranDoLoop(SgFortranDo* fortranDo)
{
  Sg_File_Info* fileInfo = Sg_File_Info::generateDefaultFileInfoForTransformationNode();

  // Get FortranDo body
  SgBasicBlock* loopBody = fortranDo->get_body();
  fortranDo->set_body(NULL); 
  // Get initialization
  SgExpression* initialization = fortranDo->get_initialization();
  fortranDo->set_initialization(NULL); 
  // Get bound
  SgExpression* bound = fortranDo->get_bound();
  fortranDo->set_bound(NULL); 
  // Get increment
  SgExpression* increment = fortranDo->get_increment();
  fortranDo->set_increment(NULL); 
  // Get numerical label
  SgLabelRefExp* numericLabel = fortranDo->get_numeric_label();
  fortranDo->set_numeric_label(NULL);

  // create forInitStatement
  SgForInitStatement* forInitStatement = buildForInitStatement();
  // Add the FortranDoLoop initialization into forInitStatement
  forInitStatement->append_init_stmt(buildExprStatement(initialization));
  forInitStatement->set_file_info(fileInfo);
  // Get index variableExpression
  ROSE_ASSERT(isSgBinaryOp(initialization));
  SgExpression* variableExpression = ((SgBinaryOp*)initialization)->get_lhs_operand();
  SgVarRefExp* varRefExp = isSgVarRefExp(variableExpression);
  SgVariableSymbol* variableSymbol = varRefExp->get_symbol();

  /* 
    build C forStatement test case.
    In theory, the initialization has to be a AssignOp.
    LHS = initilized variable from initialization 
    RHS = bound from FortranDoLoop                
  */ 
  SgLessOrEqualOp* lessOrEqualOp = buildLessOrEqualOp(buildVarRefExp(variableSymbol),
                                                      bound);
  SgExprStatement* testStatement = buildExprStatement(lessOrEqualOp);

  /*
    build C increment expression
    generate "i = i + n" style of increment
    
    If increment in FortranDo is NULL, then the increment is 1 by default
  */
  SgAssignOp* cIncrementOp;
  if(isSgNullExpression(increment) == NULL)
  {
    cIncrementOp = buildAssignOp(buildVarRefExp(variableSymbol),
                                                buildAddOp(buildVarRefExp(variableSymbol),
                                                           increment));
  }
  else
  {
    cIncrementOp = buildAssignOp(buildVarRefExp(variableSymbol),
                                                buildAddOp(buildVarRefExp(variableSymbol),
                                                buildIntVal(1)));
  }
  // create C forStatement
  SgForStatement* forStatement = buildForStatement(forInitStatement,
                                                   testStatement,
                                                   cIncrementOp,
                                                   loopBody,
                                                   NULL); 

  forStatement->set_numeric_label(numericLabel);
  forInitStatement->set_parent(forStatement);
  testStatement->set_parent(forStatement);
  cIncrementOp->set_parent(forStatement);
  loopBody->set_parent(forStatement);

  // Replace the SgFortranDo with SgForStatement
  replaceStatement(fortranDo,forStatement,true);
}  // End of Fortran_to_C::translateProcedureHeaderStatement


/******************************************************************************************************************************/
/* 
  Translate the  AttributeSpecificationStatement
*/
/******************************************************************************************************************************/
void Fortran_to_C::translateAttributeSpecificationStatement(SgAttributeSpecificationStatement* attributeSpecificationStatement)
{
  switch(attributeSpecificationStatement->get_attribute_kind())
  {
    case SgAttributeSpecificationStatement::e_dimensionStatement:
      {
        SgExpressionPtrList dimensionList = attributeSpecificationStatement->get_parameter_list()->get_expressions();
        Rose_STL_Container<SgExpression*>::iterator i =  dimensionList.begin();
        while(i != dimensionList.end())
        {
          SgPntrArrRefExp* pntrArrRefExp = isSgPntrArrRefExp(*i);
          ROSE_ASSERT(pntrArrRefExp);
          if(isLinearlizeArray)
          {
            linearizeArraySubscript(pntrArrRefExp);
          }
          else
          {
            translateArraySubscript(pntrArrRefExp);
          }
          ++i;
        }
        
      }
      break;
    case SgAttributeSpecificationStatement::e_parameterStatement:
      {
      }
      break;
    case SgAttributeSpecificationStatement::e_accessStatement_private:
    case SgAttributeSpecificationStatement::e_accessStatement_public:
    case SgAttributeSpecificationStatement::e_allocatableStatement:
    case SgAttributeSpecificationStatement::e_asynchronousStatement:
    case SgAttributeSpecificationStatement::e_bindStatement:
    case SgAttributeSpecificationStatement::e_dataStatement:
    case SgAttributeSpecificationStatement::e_externalStatement:
    case SgAttributeSpecificationStatement::e_intentStatement:
    case SgAttributeSpecificationStatement::e_intrinsicStatement:
    case SgAttributeSpecificationStatement::e_optionalStatement:
    case SgAttributeSpecificationStatement::e_pointerStatement:
    case SgAttributeSpecificationStatement::e_protectedStatement:
    case SgAttributeSpecificationStatement::e_saveStatement:
    case SgAttributeSpecificationStatement::e_targetStatement:
    case SgAttributeSpecificationStatement::e_valueStatement:
    case SgAttributeSpecificationStatement::e_volatileStatement:
    case SgAttributeSpecificationStatement::e_last_attribute_spec:
    case SgAttributeSpecificationStatement::e_unknown_attribute_spec:
    default:
      break;
  }
}

/******************************************************************************************************************************/
/* 
  Translate the  SgEquivalenceStatement
*/
/******************************************************************************************************************************/
void Fortran_to_C::translateEquivalenceStatement(SgEquivalenceStatement* equivalenceStatement)
{
  SgExpressionPtrList equivalentList = equivalenceStatement->get_equivalence_set_list()->get_expressions();
  for(vector<SgExpression*>::iterator i=equivalentList.begin(); i<equivalentList.end(); ++i)
  {
    SgExprListExp* equivalentPairExp = isSgExprListExp(*i);
    ROSE_ASSERT(equivalentPairExp);
    SgExpressionPtrList equivalentPair = equivalentPairExp->get_expressions(); 
    ROSE_ASSERT(equivalentPair.size() == 2);

    SgExpression* expr1 = equivalentPair[0];
    SgExpression* expr2 = equivalentPair[1];
    /*
      case 1: equivalence (a,b)
    */
    if()
    {
    }
    /*
      case 3: equivalence (a(i),b(j))
    */
    else if()
    {
    }
    /*
      case 2: equivalence (a,b(i))
    */
    else
    {
    }

  }
}
