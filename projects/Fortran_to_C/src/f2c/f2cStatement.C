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
  SgFunctionParameterList* functionParameterList = buildFunctionParameterList(); 
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
  
  translateFunctionParameterList(functionParameterList,programHeaderStatement->get_parameterList(),functionDefinition);

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
  Convert to a new functionParameterList
*/
/******************************************************************************************************************************/
void Fortran_to_C::translateFunctionParameterList(SgFunctionParameterList* newList, SgFunctionParameterList* oldList, SgFunctionDefinition* funcDef)
{
  SgInitializedNamePtrList argList = oldList->get_args();
  for(SgInitializedNamePtrList::iterator i=argList.begin(); i != argList.end(); ++i)
  {
    SgType* newType = (*i)->get_type();
    /*
       We need to handle multi-dimensional array argument.
       We have performed the arrayType translation in the earlier process, 
       therefore, the arrayType AST is alreay in C-style.
    */
    if(isSgArrayType(newType))
    {
      SgArrayType* rootType = isSgArrayType(newType);
      SgType* baseType = rootType->get_base_type();
      newType = buildPointerType(baseType);
      rootType->set_base_type(NULL);
      deepDelete(rootType);
    }
    SgInitializedName* newName = buildInitializedName((*i)->get_name(), newType);
    newName->set_scope(funcDef);
    newList->append_arg(newName);
  }
  
}

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
  SgFunctionParameterList* functionParameterList = buildFunctionParameterList(); 
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

  translateFunctionParameterList(functionParameterList,procedureHeaderStatement->get_parameterList(),functionDefinition);
  
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
  SgSymbolTable* symbolTable = functionDefinition->get_symbol_table();
  ROSE_ASSERT(symbolTable);

  map<SgLabelSymbol*, SgLabelStatement*> labelStmtList;
  std::set<SgNode *> symbolList  = symbolTable->get_symbols();
  std::set<SgNode *>::iterator i = symbolList.begin();
  for(; i != symbolList.end(); ++i)
  {
    SgLabelSymbol* localLabelSymbol = isSgLabelSymbol(*i);
    if(localLabelSymbol != NULL)
    {
      string cLabelName = "FortranLabel" + localLabelSymbol->get_name().getString();
      SgLabelStatement* fortranLabelStmt = isSgLabelStatement(localLabelSymbol->get_fortran_statement());
      ROSE_ASSERT(fortranLabelStmt);

      // building new lableStmt
      SgLabelStatement* newLabelStmt = buildLabelStatement(cLabelName, NULL, functionDefinition);
      newLabelStmt->set_scope(fortranLabelStmt->get_scope());
      SgLabelSymbol* newSymbol = new SgLabelSymbol(newLabelStmt);
      symbolTable->insert(cLabelName,newSymbol);
      // make the list of oldLabelSymbol and newLabelStatement
      labelStmtList.insert(pair<SgLabelSymbol*,SgLabelStatement*>(localLabelSymbol,newLabelStmt));

      replaceStatement(fortranLabelStmt,newLabelStmt,true);
      deepDelete(fortranLabelStmt->get_numeric_label()); 
      deepDelete(fortranLabelStmt);
    }
  }
  
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
    std::set<SgNode *> localSymbolList  = localSymbolTable->get_symbols();
    std::set<SgNode *>::iterator j = localSymbolList.begin();
    for(; j != localSymbolList.end(); ++j)
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
    }  
  }   // end of i loop

  // Replace the target of Fortran gotoStatement
  Rose_STL_Container<SgNode*> gotoList = NodeQuery::querySubTree (root,V_SgGotoStatement);
  for (Rose_STL_Container<SgNode*>::iterator j = gotoList.begin(); j != gotoList.end(); j++)
  {
    SgGotoStatement* gotoStmt = isSgGotoStatement(*j);
    map<SgLabelSymbol*, SgLabelStatement*>::iterator i = labelStmtList.find(gotoStmt->get_label_expression()->get_symbol());
    if(i != labelStmtList.end())
    {
      delete(gotoStmt->get_label_expression());
      gotoStmt->set_label_expression(NULL);
      gotoStmt->set_label(i->second);
    }
  }
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
        deepDelete(attributeSpecificationStatement->get_parameter_list());        
        break;
      }
    case SgAttributeSpecificationStatement::e_parameterStatement:
      {
        break;
      }
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
  SgScopeStatement* scope = equivalenceStatement->get_scope();
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
      case 1: equivalence (a,b) , or equivalence(a(c),b)
      Transformation:
        1. create pointer b
        2. b points to the beginning address of a, no matter a is scalar or array
    */
    if(isSgVarRefExp(expr2) != NULL)
    {
      bool isEquivToVarRef = false;
      SgVarRefExp* varRefExp1 = NULL;
      
      if(isSgVarRefExp(expr1) != NULL)
      {
        isEquivToVarRef = true;
        varRefExp1 = isSgVarRefExp(expr1);
      }
      else if(isSgPntrArrRefExp(expr1) != NULL)
      {
        SgPntrArrRefExp* tmpArrayType = isSgPntrArrRefExp(expr1);
        varRefExp1 = isSgVarRefExp(tmpArrayType->get_lhs_operand());
        while((tmpArrayType = isSgPntrArrRefExp(tmpArrayType->get_lhs_operand())) != NULL)
        {
          varRefExp1 = isSgVarRefExp(tmpArrayType->get_lhs_operand());
        }
      }
      SgVariableSymbol* symbol1 = varRefExp1->get_symbol();
      SgType* var1OriginalType = symbol1->get_type();


      SgVarRefExp* varRefExp2 = isSgVarRefExp(expr2);
      SgVariableSymbol* symbol2 = varRefExp2->get_symbol();
      SgType* var2OriginalType = symbol2->get_type();
      SgInitializedName* sym2InitializedName = symbol2->get_declaration();
      SgDeclarationStatement* sym2OldDecl = sym2InitializedName->get_declaration();

      if(isSgArrayType(var2OriginalType) != NULL)
      {
        SgArrayType* tmpArrayType = isSgArrayType(var2OriginalType);
        vector<SgExpression*> arrayInfo;
        arrayInfo.insert(arrayInfo.begin(),tmpArrayType->get_index());
        SgType* baseType = tmpArrayType->get_base_type();
        while((tmpArrayType = isSgArrayType(tmpArrayType->get_base_type())) != NULL)
        {
          baseType = tmpArrayType->get_base_type();
          arrayInfo.insert(arrayInfo.begin(),tmpArrayType->get_index());
        }
        // Now baseType should have the base type of multi-dimensional array.
        SgType* newType = NULL;
        for(vector<SgExpression*>::iterator i=arrayInfo.begin(); i <arrayInfo.end()-1;++i)
        {
          SgArrayType* tmpType;
          tmpType = buildArrayType(baseType,*i);
          baseType = isSgType(tmpType);
        }
        // build the new PointerType
        newType = buildPointerType(baseType);

        SgAssignInitializer* assignInitializer = NULL;
        // VariableDeclaration for this pointer, that points to the array space
        if(isEquivToVarRef)
        {
          /* 
            This following case is tested in equivalence2.F
            real*4 a(4,16), b(4,4)
            equivalence(a,b)
          */ 
          assignInitializer = buildAssignInitializer(buildCastExp(buildAddressOfOp(buildVarRefExp(symbol1)),
                                                                  newType,
                                                                  SgCastExp::e_C_style_cast));
        }
        else
        {
          /* 
            This following case is tested in equivalence3.F
            real*4 a(4,16), b(4,4)
           equivalence(a(1,4),b)
          */ 
          SgPntrArrRefExp* var1PntrArrRef = isSgPntrArrRefExp(expr1);
          SgExpression* newVar1Exp = buildAddressOfOp(deepCopy(var1PntrArrRef));
          assignInitializer = buildAssignInitializer(buildCastExp(newVar1Exp,
                                                                  newType,
                                                                  SgCastExp::e_C_style_cast));
        }

        SgVariableDeclaration* sym2NewDecl = buildVariableDeclaration(symbol2->get_name(),
                                                                      newType,
                                                                      assignInitializer,
                                                                      scope);        
        SgInitializedName* sym2NewIntializedName = *((sym2NewDecl->get_variables()).begin());
        sym2NewIntializedName->set_prev_decl_item(NULL);
        symbol2->set_declaration(sym2NewIntializedName);
        replaceStatement(sym2OldDecl,sym2NewDecl,true);
        deepDelete(sym2OldDecl);

      }
      else
      {
        /*
          This is the type when b is just a scalar variable.
          the rest of hte usage of b has to be a derefence.
          b = ...   ====> *b = ...
          c = b     ====> c  = *b
        */
        SgPointerType* newType = buildPointerType(var2OriginalType);
        SgAssignInitializer* assignInitializer = NULL;
        if(isEquivToVarRef)
        {
          /* 
            This following case is tested in equivalence1.F
            real*4 a, b
           equivalence(a,b)
          */ 
          /*
            if both variable are same type, then no cast is needed. Otherwise, we cast its type.
          */
          if(var1OriginalType == var2OriginalType)
              assignInitializer = buildAssignInitializer(buildAddressOfOp(buildVarRefExp(symbol1)));
          else
              assignInitializer = buildAssignInitializer(buildCastExp(buildAddressOfOp(buildVarRefExp(symbol1)),
                                                                      newType, 
                                                                      SgCastExp::e_C_style_cast));
        }
        else
        {
          /* 
            This following case is tested in equivalence4.F
            real*4 a(4,16), b
           equivalence(a(1,4),b)
          */ 
          SgPntrArrRefExp* var1PntrArrRef = isSgPntrArrRefExp(expr1);
          SgExpression* newVar1Exp = buildAddressOfOp(deepCopy(var1PntrArrRef));
          assignInitializer = buildAssignInitializer(buildCastExp(newVar1Exp,
                                                                  newType, 
                                                                  SgCastExp::e_C_style_cast));
        }    
        SgVariableDeclaration* sym2NewDecl = buildVariableDeclaration(symbol2->get_name(),
                                                                      newType,
                                                                      assignInitializer,
                                                                      scope);
        SgInitializedName* sym2NewIntializedName = *((sym2NewDecl->get_variables()).begin());
        sym2NewIntializedName->set_prev_decl_item(NULL);
        symbol2->set_declaration(sym2NewIntializedName);
        replaceStatement(sym2OldDecl,sym2NewDecl,true);
        deepDelete(sym2OldDecl);
  
        // Node query for all SgVarRefExp pointing to variable2 
        Rose_STL_Container<SgNode*> varList = NodeQuery::querySubTree (getEnclosingFunctionDefinition(equivalenceStatement),V_SgVarRefExp);
        for (Rose_STL_Container<SgNode*>::iterator i = varList.begin(); i != varList.end(); i++)
        {
          SgVarRefExp* varRef = isSgVarRefExp(*i);
          if(varRef->get_symbol()->get_declaration() == sym2NewIntializedName)
          {
              replaceExpression(varRef,
                                buildPointerDerefExp(buildVarRefExp(symbol2)),
                                false);
          }
        }
      }
    }
    /*
      case 2: equivalence (a,b(c)) , or equivalence(a(c),b(d))
      Transformation:
    */
    else if(isSgPntrArrRefExp(expr2) != NULL)
    {
      bool isEquivToVarRef = false;
      SgVarRefExp* varRefExp1 = NULL;
      
      if(isSgVarRefExp(expr1) != NULL)
      {
        isEquivToVarRef = true;
        varRefExp1 = isSgVarRefExp(expr1);
      }
      else if(isSgPntrArrRefExp(expr1) != NULL)
      {
        SgPntrArrRefExp* tmpArrayType = isSgPntrArrRefExp(expr1);
        varRefExp1 = isSgVarRefExp(tmpArrayType->get_lhs_operand());
        while((tmpArrayType = isSgPntrArrRefExp(tmpArrayType->get_lhs_operand())) != NULL)
        {
          varRefExp1 = isSgVarRefExp(tmpArrayType->get_lhs_operand());
        }
      }
      // fetch information of var1
      SgVariableSymbol* symbol1 = varRefExp1->get_symbol();
      SgType* var1OriginalType = symbol1->get_type();
      SgInitializedName* sym1InitializedName = symbol1->get_declaration();
      SgDeclarationStatement* sym1OldDecl = sym1InitializedName->get_declaration();

      // fetch information of var2
      SgPntrArrRefExp* var2PntrArrRef = isSgPntrArrRefExp(expr2);
      SgVarRefExp* varRefExp2 = isSgVarRefExp(var2PntrArrRef->get_lhs_operand());
      while((var2PntrArrRef = isSgPntrArrRefExp(var2PntrArrRef->get_lhs_operand())) != NULL)
      {
        varRefExp2 = isSgVarRefExp(var2PntrArrRef->get_lhs_operand());
      }
      SgVariableSymbol* symbol2 = varRefExp2->get_symbol();
      SgType* var2OriginalType = symbol2->get_type();
      SgInitializedName* sym2InitializedName = symbol2->get_declaration();
      SgDeclarationStatement* sym2OldDecl = sym2InitializedName->get_declaration();

      if(isEquivToVarRef)
      {
        if(isSgArrayType(var1OriginalType) != NULL)
        {
          /*
          real*4 a(4)
          equivalence(a, b(c)) 
          */
          SgArrayType* tmpArrayType = isSgArrayType(var1OriginalType);
          vector<SgExpression*> arrayInfo;
          arrayInfo.insert(arrayInfo.begin(),tmpArrayType->get_index());
          SgType* baseType = tmpArrayType->get_base_type();
          while((tmpArrayType = isSgArrayType(tmpArrayType->get_base_type())) != NULL)
          {
            baseType = tmpArrayType->get_base_type();
            arrayInfo.insert(arrayInfo.begin(),tmpArrayType->get_index());
          }
          // Now baseType should have the base type of multi-dimensional array.
          SgType* newType = NULL;
          for(vector<SgExpression*>::iterator i=arrayInfo.begin(); i <arrayInfo.end()-1;++i)
          {
            SgArrayType* tmpType;
            tmpType = buildArrayType(baseType,*i);
            baseType = isSgType(tmpType);
          }
          // build the new PointerType
          newType = buildPointerType(baseType);

          SgAssignInitializer* assignInitializer = NULL;
          var2PntrArrRef = isSgPntrArrRefExp(expr2);
          SgExpression* newVar2Exp = buildAddressOfOp(deepCopy(var2PntrArrRef));
          assignInitializer = buildAssignInitializer(buildCastExp(newVar2Exp,
                                                                  newType, 
                                                                  SgCastExp::e_C_style_cast));
          SgVariableDeclaration* sym1NewDecl = buildVariableDeclaration(symbol1->get_name(),
                                                                        newType,
                                                                        assignInitializer,
                                                                        scope);
          SgInitializedName* sym1NewIntializedName = *((sym1NewDecl->get_variables()).begin());
          sym1NewIntializedName->set_prev_decl_item(NULL);
          symbol1->set_declaration(sym1NewIntializedName);
          insertStatementAfterLastDeclaration(sym1NewDecl,scope);
          removeStatement(sym1OldDecl);
          deepDelete(sym1OldDecl);

        }
        else
        {
          /*
          real*4 a
          equivalence(a, b(c)) 
          */
          SgPointerType* newType = buildPointerType(var1OriginalType);
          SgAssignInitializer* assignInitializer = NULL;
          var2PntrArrRef = isSgPntrArrRefExp(expr2);
          SgExpression* newVar2Exp = buildAddressOfOp(deepCopy(var2PntrArrRef));
          assignInitializer = buildAssignInitializer(buildCastExp(newVar2Exp,
                                                                  newType, 
                                                                  SgCastExp::e_C_style_cast));
          SgVariableDeclaration* sym1NewDecl = buildVariableDeclaration(symbol1->get_name(),
                                                                        newType,
                                                                        assignInitializer,
                                                                        scope);
          SgInitializedName* sym1NewIntializedName = *((sym1NewDecl->get_variables()).begin());
          sym1NewIntializedName->set_prev_decl_item(NULL);
          symbol1->set_declaration(sym1NewIntializedName);
          insertStatementAfterLastDeclaration(sym1NewDecl,scope);
          removeStatement(sym1OldDecl);
          deepDelete(sym1OldDecl);
          // Node query for all SgVarRefExp pointing to variable1 
          Rose_STL_Container<SgNode*> varList = NodeQuery::querySubTree (getEnclosingFunctionDefinition(equivalenceStatement),V_SgVarRefExp);
          for (Rose_STL_Container<SgNode*>::iterator i = varList.begin(); i != varList.end(); i++)
          {
            SgVarRefExp* varRef = isSgVarRefExp(*i);
            if(varRef->get_symbol()->get_declaration() == sym1NewIntializedName)
            {
                replaceExpression(varRef,
                                  buildPointerDerefExp(buildVarRefExp(symbol1)),
                                  false);
            }
          }
        }
      }
      else
      {
/*
!!!!!IMPORTANT!!!!!!
This case, we would need Fortran user to follow this rule.
equivalence(largeArray, smallArray)

Example:
dimension a(4,4),b(4)
equivalence(a(1,4),b(1))

Then, b will become a pointer points to a(1,4), and b will never reference out of bound.
Having the reverse order would cause a to reference out of bound.
*/
        /*
        real*4 a(4)
        equivalence(a(d), b(c)) 
        */
        SgArrayType* tmpArrayType = isSgArrayType(var2OriginalType);
        vector<SgExpression*> arrayInfo;
        arrayInfo.insert(arrayInfo.begin(),tmpArrayType->get_index());
        SgType* baseType = tmpArrayType->get_base_type();
        while((tmpArrayType = isSgArrayType(tmpArrayType->get_base_type())) != NULL)
        {
          baseType = tmpArrayType->get_base_type();
          arrayInfo.insert(arrayInfo.begin(),tmpArrayType->get_index());
        }
        // Now baseType should have the base type of multi-dimensional array.
        SgType* newType = NULL;
        for(vector<SgExpression*>::iterator i=arrayInfo.begin(); i <arrayInfo.end()-1;++i)
        {
          SgArrayType* tmpType;
          tmpType = buildArrayType(baseType,*i);
          baseType = isSgType(tmpType);
        }
        // build the new PointerType
        newType = buildPointerType(baseType);

        SgAssignInitializer* assignInitializer = NULL;
        SgPntrArrRefExp* var1PntrArrRef = isSgPntrArrRefExp(expr1);
        SgExpression* newVar1Exp = buildAddressOfOp(deepCopy(var1PntrArrRef));
        assignInitializer = buildAssignInitializer(buildCastExp(newVar1Exp,
                                                                newType,
                                                                SgCastExp::e_C_style_cast));

        SgVariableDeclaration* sym2NewDecl = buildVariableDeclaration(symbol2->get_name(),
                                                                      newType,
                                                                      assignInitializer,
                                                                      scope);        
        SgInitializedName* sym2NewIntializedName = *((sym2NewDecl->get_variables()).begin());
        sym2NewIntializedName->set_prev_decl_item(NULL);
        symbol2->set_declaration(sym2NewIntializedName);
        replaceStatement(sym2OldDecl,sym2NewDecl,true);
        deepDelete(sym2OldDecl);
      }
    }
    else
    {
      cerr<<"warning, unhandled equivalence case"<<endl;
    }
  }
  deepDelete(equivalenceStatement->get_equivalence_set_list());
}


void Fortran_to_C::removeFortranMaxMinFunction(SgGlobal* global)
{
  vector<string> functionName;
  functionName.push_back("min");
  functionName.push_back("max");
  functionName.push_back("dmin");
  functionName.push_back("dmin");
  for(vector<string>::iterator i = functionName.begin(); i != functionName.end(); ++i){
    SgFunctionSymbol* funcSymbol = lookupFunctionSymbolInParentScopes((*i), global); 
    if(funcSymbol != NULL)
    {
      ROSE_ASSERT(funcSymbol);
      SgSymbolTable* symTable = isSgSymbolTable(funcSymbol->get_parent());
      ROSE_ASSERT(symTable);
      symTable->remove(funcSymbol);
      SgFunctionDeclaration* decl = funcSymbol->get_declaration();
      funcSymbol->set_declaration(NULL);
      deepDelete(decl);
      deepDelete(funcSymbol);
    }
  }
}

