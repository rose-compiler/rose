//#define ARRAY_TEST 1
#define PATH 1
#define ALLOW_UNKNOWN_VARIABLE_SYMBOL 1
#define FORLOOPONCETHROUGH 1
#include "smtQueryLib.h"

std::map<std::string, std::string> enums_defined;

void setTruthValue(SgNode* n, bool value) {
	pathNodeTruthValue[n] = value;
}



void forOnceThrough(SgForStatement* forStat) {
	SgStatementPtrList ptr = forStat->get_init_stmt();
	for (SgStatementPtrList::iterator i = ptr.begin(); i != ptr.end(); i++) {
		std::string statement_out = getSgStatement(*i);
		std::cout << ";;statement out:\n" << statement_out << std::endl;
	}
	std::string body_stat = getSgStatement(forStat->get_loop_body());
	std::cout << ";;body:\n" << body_stat << std::endl;
	return;
}	

void getSgIfStmt(SgIfStmt* ifstat) {
	bool truthBool;
	//#ifdef PATH
	truthBool= pathNodeTruthValue[ifstat];
	//#endif	
	scopeStatNum++;
	std::stringstream ifstatindexstream;
	ifstatindexstream << scopeStatNum;
	std::string ifstatindex = ifstatindexstream.str();
	SgStatement* conditional = ifstat->get_conditional();
	ROSE_ASSERT(isSgStatement(conditional));
	std::string conditionalString = getSgStatement(conditional);
	
	if (conditionalString == "") {
		std::cout << "empty conditional string!" << std::endl;
		ROSE_ASSERT(false);
	}
	SgStatement* ifBody;
	
	std::string ifBodyString;
	
	if (!truthBool) { ifBody = ifstat->get_false_body(); }
	else { ifBody = ifstat->get_true_body(); }
	if (ifBody == NULL) {
	ifBodyString = "; empty body\n";
	}
	else {
	ROSE_ASSERT(isSgBasicBlock(ifBody));
	getSgBasicBlock(isSgBasicBlock(ifBody));

	}
	
	std::string fullIfStat;
	std::string assrt1, assrt2, assrt3;
	if (truthBool) {
		assrt3 = "(assert (= " + conditionalString + " true))\n";
	}
	else {
		assrt3 = "(assert (= " + conditionalString + " false))\n";
	}
	expressions.push_back(assrt3);

	return;
}	

void getSgBasicBlock(SgBasicBlock* basicBlock) {
	std::string basicBlockString;
	std::vector<std::string> basicBlockStrings;
	SgStatementPtrList statements = basicBlock->get_statements();
	for (SgStatementPtrList::iterator i = statements.begin(); i != statements.end(); i++) {
		std::string ithStatement = getSgStatement(*i);
		basicBlockStrings.push_back(ithStatement);
	}
	if (basicBlockStrings.size() == 0) {
		basicBlockString = "";
	}
	else if (basicBlockStrings.size() == 1) {
		basicBlockString = "\n"  + basicBlockStrings[0];
	}
	else {
		basicBlockString = "\n";
	}
	return;
}


void getSgFunctionParameterList(SgFunctionParameterList* funcParamList) {
	SgInitializedNamePtrList pList = funcParamList->get_args();
	std::string paramReturnString;
	for (SgInitializedNamePtrList::iterator i = pList.begin(); i != pList.end(); i++) {
		std::string param = getSgInitializedName(*i);
		paramReturnString = paramReturnString + "\n" + param;
	}
	variables.push_back(paramReturnString);
	return;
}
	

void getSgFunctionDefinition(SgFunctionDefinition* funcDef) {
	SgFunctionDeclaration* funcDefDecl = funcDef->get_declaration();
	SgFunctionParameterList* funcParamList = funcDefDecl->get_parameterList(); 
	getSgFunctionParameterList(funcParamList);
	//might need to do more later, for now all it does is instantiate its parameters
	SgBasicBlock* f_body = funcDef->get_body();
	getSgBasicBlock(f_body);
	return;		
	
}

std::string initializeVariable(SgInitializedName* initName) {
	//if array type we need to get the index expression
	std::string index_expression_string;
	std::stringstream nameStringStream;
	SgName initNameName = initName->get_qualified_name();
	SgSymbol* initNameSym = initName->search_for_symbol_from_symbol_table();
	if (variablesOfNameX.find(initNameName.getString()) == variablesOfNameX.end()) {
		nameStringStream << initNameName.getString() << "_0";
		variablesOfNameX[initNameName.getString()] = 1;
	}
	else {
		int occurrence = variablesOfNameX[initNameName.getString()];
		nameStringStream << initNameName.getString() << "_" << occurrence;
		variablesOfNameX[initNameName.getString()] = occurrence+1;
	}
	SymbolToZ3[initNameSym] = nameStringStream.str();
	SymbolToInstances[initNameSym] = 0;
	SgType* initNameType = initName->get_type();
	std::string typeZ3;
	if (initNameType->isIntegerType()) {
		typeZ3 = "Int";
	}
	else if (initNameType->isFloatType()) {
		typeZ3 = "Real";
	}
	else if (isSgArrayType(initNameType)) {
		SgArrayType* arrTyp = isSgArrayType(initNameType);
		ROSE_ASSERT(arrTyp != NULL);
		SgType* underlying_type = arrTyp->get_base_type();
		std::string array_typeZ3;
		if (underlying_type->isIntegerType()) {
			array_typeZ3 = "Int";
		}
		else if (underlying_type->isFloatType()) {
			array_typeZ3 = "Real";
		}
		else {
			std::cout << "unknown underlying type of array!" << std::endl;
			std::cout << underlying_type->class_name() << std::endl;
			ROSE_ASSERT(false);
		}
		SgExpression* ind = arrTyp->get_index();
		std::stringstream arrStr;
		index_expression_string = getSgExpressionString(ind);
		typeZ3 = "(Array Int " + array_typeZ3 + ")";
	}
	else if (isSgClassType(initNameType)) {
		
		std::cout << "structs are not yet implemented" << std::endl;
		ROSE_ASSERT(false);
	}
	else if (isSgPointerType(initNameType)) {
		std::cout << "pointers are not yet implemented" << std::endl;
		ROSE_ASSERT(false);
	}
	else if (isSgEnumType(initNameType)) {
		SgEnumType* et = isSgEnumType(initNameType);
		SgEnumDeclaration* enum_d = isSgEnumDeclaration(et->getAssociatedDeclaration());
		getSgDeclarationStatement(enum_d);	
		typeZ3 = et->get_name().getString(); 
	}
	else {
		std::cout << "unknown type: " << initNameType->class_name() << std::endl;
		ROSE_ASSERT(false);
	}		
	std::string name = nameStringStream.str() + "_0";
	std::stringstream streamZ3;
	if (isSgArrayType(initNameType)) {
	streamZ3 << "(declare-const " << name << " " << typeZ3 << ")";
	streamZ3 << "\n(declare-fun " << name << "_len () Int)";
	streamZ3 << "\n(assert (= " << name << "_len " << index_expression_string << "))"; 	
	#ifdef ARRAY_TEST
	std::cout << "arrStream: " << streamZ3.str() << std::endl;
	#endif

	}
	else if (isSgEnumType(initNameType)) {
		streamZ3 << "(declare-const " << name << " " << typeZ3 << ")";
	}
	else {
	streamZ3 << "(declare-fun " << name << " () " << typeZ3 << ")";
	}
	return streamZ3.str();
}

		
	

std::string getSgInitializedName(SgInitializedName* initName) {
	std::string exprStr;
	SgSymbol* initNameSym = initName->search_for_symbol_from_symbol_table();
	std::string varInit = initializeVariable(initName);
	std::string retString;
	if (initName->get_initptr() != NULL) {
		SgInitializer* nameInitializer = initName->get_initializer();

	        VariantT var = nameInitializer->variantT();
        
		switch (var) {
                	case V_SgAggregateInitializer:
			{
				SgAggregateInitializer* aggInit = isSgAggregateInitializer(nameInitializer);
				if (!isSgArrayType(aggInit->get_type())) {
					std::cout << "currently only arrays use aggregate initializers, you are using " << aggInit->class_name() << std::endl;
					ROSE_ASSERT(false);
				}
				SgExprListExp* members = aggInit->get_initializers();
				SgExpressionPtrList member_expressions = members->get_expressions();
				std::string symName = SymbolToZ3[initNameSym];
				ROSE_ASSERT(SymbolToInstances[initNameSym] == 0);
				int arrmem = 0;
				std::stringstream exprStream;
				for (SgExpressionPtrList::iterator i = member_expressions.begin(); i != member_expressions.end(); i++) {
			
			exprStream << "\n(assert (= (select " << symName << "_0 "  << arrmem << ") " << getSgExpressionString((isSgAssignInitializer((*i))->get_operand())) << ")";
				arrmem = arrmem+1;
				}
                       		retString = varInit + "\n" + exprStream.str(); 
				#ifdef ARRAY_TEST
				std::cout << "retString: " << retString << std::endl;
				#endif
			
			break;
			}
			case V_SgCompoundInitializer:
			{	
			std::cout << "SgCompoundInitializer not yet supported" << std::endl;
			ROSE_ASSERT(false);
			break;
			}
			case V_SgConstructorInitializer:
			{
			std::cout << "SgConstructorInitializer is not yet supported" << std::endl;
			ROSE_ASSERT(false);
			break;
			}
			case V_SgDesignatedInitializer:
			{
			std::cout << "SgDesignatedInitializer is not yet supported" << std::endl;
			ROSE_ASSERT(false);
			break;
			}
			case V_SgAssignInitializer:
			{
			SgAssignInitializer* assignInit = isSgAssignInitializer(nameInitializer);
			std::string symName = SymbolToZ3[initNameSym];
			ROSE_ASSERT(SymbolToInstances[initNameSym] == 0);
			exprStr = "(assert (= " + symName + "_0 " + getSgExpressionString(assignInit->get_operand()) + "))";
			retString = varInit + "\n" + exprStr;
			break;
			}
			default:
			{
			std::cout << "unknown initializer of type: " << nameInitializer->class_name() << std::endl;
			ROSE_ASSERT(false);
			break;
			}
	}
			
		
	}		
	else {
		retString = varInit;
	}
	
	return retString;
}
	
void getSgScopeStatement(SgScopeStatement* scopeStat) {
	VariantT var = scopeStat->variantT();
	std::string scopeStatStr = "";
	switch (var) {
		case V_SgBasicBlock:
			{
			getSgBasicBlock(isSgBasicBlock(scopeStat));
			break;
			}

		case V_SgCatchOptionStmt:
			{
			std::cout << "SgCatchOptionStmt is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgDoWhileStmt:
			{
			std::cout << "SgDoWhileStmt is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgForStatement:
			{
		//	std::cout << "SgForStatement is not yet implemented" << std::endl;
			#ifdef FORLOOPONCETHROUGH
			forOnceThrough(isSgForStatement(scopeStat));
				
			
			#else
			std::cout << "SgForStatement is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			#endif
			break;
			}

		case V_SgGlobal:
			{
			std::cout << "SgGlobal is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgIfStmt:
			{
			getSgIfStmt(isSgIfStmt(scopeStat));
			break;
			}

		case V_SgSwitchStatement:
			{
			std::cout << "SgSwitchStatement is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgWhileStmt:
			{
			std::cout << "SgWhileStmt is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgForAllStatement:

			{
			std::cout << "SgForAllStatement is not yet implemented" << std::endl; ;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgAssociateStatement:

			{
			std::cout << "SgAssociateStatement is not yet implemented" << std::endl; ;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgBlockDataStatement:

			{
			std::cout << "SgBlockDataStatement is not yet implemented" << std::endl; ;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgNamespaceDefinitionStatement:

			{
			std::cout << "SgNamespaceDefinitionStatement is not yet implemented" << std::endl; ;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgClassDefinition:
			{
			std::cout << "SgClassDefinition should not be found here! " << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgFunctionDefinition:
			{
			getSgFunctionDefinition(isSgFunctionDefinition(scopeStat));
			break;
			}

		case V_SgCAFWithTeamStatement:
			{
			std::cout << "SgCAFWithTeamStatement should not be found here! " << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgFortranDo:
			{
			std::cout << "SgFortranDo should not be found here! " << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgFortranNonblockedDo:
			{
			std::cout << "SgFortranNonblockedDo should not be found here! " << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgJavaForEachStatement:
			{
			std::cout << "SgJavaForEachStatement should not be found here! " << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgJavaLabelStatement:
			{
			std::cout << "SgJavaLabelStatement should not be found here! " << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgUpcForAllStatement:
			{
			std::cout << "SgUpcForAllStatement should not be found here! " << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		default:
			{
			std::cout << " Unknown node type!: " << scopeStat->class_name() << std::endl;
			ROSE_ASSERT(false);
			}
	}
	return;
}
			
void getSgDeclarationStatement(SgDeclarationStatement* declStat) {
	VariantT var = declStat->variantT();
	std::string declStatStr = "";
	switch (var) {
		case V_SgEnumDeclaration:
			{
			SgEnumDeclaration* enum_decl = isSgEnumDeclaration(declStat);
			std::string enum_str_name = enum_decl->get_name().getString();
                        std::cout << ";enum_str_name = " << enum_str_name << std::endl;
                        if (enums_defined.find(enum_str_name) == enums_defined.end()) {
                                std::cout << ";enum not yet defined" << std::endl;
                                std::string enum_name = enum_decl->get_name().getString();
                                std::stringstream enum_z3;
                                enum_z3 << "(declare-datatypes () ((" << enum_name;
                                SgInitializedNamePtrList enumerated_values = enum_decl->get_enumerators();
                                for (SgInitializedNamePtrList::iterator j = enumerated_values.begin(); j != enumerated_values.end(); j++) {
                                        SgName enum_value_name = (*j)->get_name();
                                        enum_z3 << " " << enum_value_name.getString();
                                }
                                enum_z3 << ")))";
                                std::cout << enum_z3.str() << std::endl;
                                enums_defined[enum_str_name] = enum_z3.str();
                        }
			break;
			}

		case V_SgFunctionParameterList:
			{
			std::cout << "SgFunctionParameterList is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgTypedefDeclaration:
			{
			std::cout << "SgTypedefDeclaration is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgVariableDeclaration:
			{
			SgVariableDeclaration* varDecl = isSgVariableDeclaration(declStat);
			SgInitializedNamePtrList list = varDecl->get_variables();
			std::stringstream declStatStrStream;
			int checkOne = 0;
			for (SgInitializedNamePtrList::iterator i = list.begin(); i != list.end(); i++) {
				if (checkOne == 1) {
					std::cout << "InitializedNamePtrList should have only one InitializedName" << std::endl;
					ROSE_ASSERT(false);
				}
				SgInitializedName* ithName = isSgInitializedName(*i);
				declStatStrStream << getSgInitializedName(*i) << "\n";
				checkOne++;
			}
			declStatStr = declStatStrStream.str();

			break;
			}

		case V_SgVariableDefinition:
			{
			std::cout << "SgVariableDefinition is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgAttributeSpecificationStatement:

			{
			std::cout << "SgAttributeSpecificationStatement is not yet implemented" << std::endl; ;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgCommonBlock:

			{
			std::cout << "SgCommonBlock is not yet implemented" << std::endl; ;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgEquivalenceStatement:

			{
			std::cout << "SgEquivalenceStatement is not yet implemented" << std::endl; ;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgImplicitStatement:

			{
			std::cout << "SgImplicitStatement is not yet implemented" << std::endl; ;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgNamelistStatement:

			{
			std::cout << "SgNamelistStatement is not yet implemented" << std::endl; ;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgNamespaceDeclarationStatement:

			{
			std::cout << "SgNamespaceDeclarationStatement is not yet implemented" << std::endl; ;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgTemplateInstantiationDirectiveStatement:

			{
			std::cout << "SgTemplateInstantiationDirectiveStatement is not yet implemented" << std::endl; ;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgUsingDeclarationStatement:

			{
			std::cout << "SgUsingDeclarationStatement is not yet implemented" << std::endl; ;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgUsingDirectiveStatement:

			{
			std::cout << "SgUsingDirectiveStatement is not yet implemented" << std::endl; ;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgClassDeclaration:
			{
			std::cout << "SgClassDeclaration should not be found here! " << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgContainsStatement:
			{
			std::cout << "SgContainsStatement should not be found here! " << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgFunctionDeclaration:
			{
			std::cout << "SgFunctionDeclaration should not be found here! " << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgTemplateDeclaration:
			{
			std::cout << "SgTemplateDeclaration should not be found here! " << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgAsmStmt:
			{
			std::cout << "SgAsmStmt should not be found here! " << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgC_PreprocessorDirectiveStatement:
			{
			std::cout << "SgC_PreprocessorDirectiveStatement should not be found here! " << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgClinkageDeclarationStatement:
			{
			std::cout << "SgClinkageDeclarationStatement should not be found here! " << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgCtorInitializerList:
			{
			std::cout << "SgCtorInitializerList should not be found here! " << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgFormatStatement:
			{
			std::cout << "SgFormatStatement should not be found here! " << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgFortranIncludeLine:
			{
			std::cout << "SgFortranIncludeLine should not be found here! " << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgImportStatement:
			{
			std::cout << "SgImportStatement should not be found here! " << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgInterfaceStatement:
			{
			std::cout << "SgInterfaceStatement should not be found here! " << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgJavaImportStatement:
			{
			std::cout << "SgJavaImportStatement should not be found here! " << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgNamespaceAliasDeclarationStatement:
			{
			std::cout << "SgNamespaceAliasDeclarationStatement should not be found here! " << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgStmtDeclarationStatement:
			{
			std::cout << "SgStmtDeclarationStatement should not be found here! " << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgUseStatement:
			{
			std::cout << "SgUseStatement should not be found here! " << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgPragmaDeclaration:
			{
					
			//std::cout << "pragmas skipped" << std::endl;
		//	declStatStr=";;pragma\n";
			break;
			}	
		default:
			{
			std::cout << " Unknown node type!: " << declStat->class_name() << std::endl;
			ROSE_ASSERT(false);
			}
	}
	declarations.push_back(declStatStr);
	return;
}


std::string getSgIOStatement(SgIOStatement* ioStat) {
	std::cout << "io statements are not implemented yet" << std::endl;
	ROSE_ASSERT(false);
	return "";
}

std::string getSgExprStatement(SgExprStatement* exprStat) {
	SgExpression* exprFromExprStat = exprStat->get_expression();
	std::string exprStr = getSgExpressionString(exprFromExprStat);
	return exprStr;
}
std::string getSgStatement(SgStatement* stat) {
	VariantT var = stat->variantT();
	std::string statStr = "";
	if (isSgDeclarationStatement(stat)) {
		getSgDeclarationStatement(isSgDeclarationStatement(stat));
	}
	else if (isSgScopeStatement(stat)) {
		getSgScopeStatement(isSgScopeStatement(stat));
	}
	else if (isSgIOStatement(stat)) {
		statStr = getSgIOStatement(isSgIOStatement(stat));
	}
	else {
	switch (var) {
		case V_SgAllocateStatement:
			{
			std::cout << "SgAllocateStatement is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgArithmeticIfStatement:
			{
			std::cout << "SgArithmeticIfStatement is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgAssertStmt:
			{
			std::cout << "SgAssertStmt is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgAssignedGotoStatement:
			{
			std::cout << "SgAssignedGotoStatement is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgAssignStatement:
			{
			std::cout << "SgAssignStatement is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgBreakStmt:
			{
			std::cout << "SgBreakStmt is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgCaseOptionStmt:
			{
			std::cout << "SgCaseOptionStmt is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgCatchStatementSeq:
			{
			std::cout << "SgCatchStatementSeq is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgComputedGotoStatement:
			{
			std::cout << "SgComputedGotoStatement is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgContinueStmt:
			{
			std::cout << "SgContinueStmt is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgDeallocateStatement:
			{
			std::cout << "SgDeallocateStatement is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgDefaultOptionStmt:
			{
			std::cout << "SgDefaultOptionStmt is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgExprStatement:
			{
			statStr = getSgExprStatement(isSgExprStatement(stat));
			break;
			}

		case V_SgForInitStatement:
			{
			std::cout << "SgForInitStatement is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgFunctionTypeTable:
			{
			std::cout << "SgFunctionTypeTable is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgGotoStatement:
			{
			std::cout << "SgGotoStatement is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgLabelStatement:
			{
			std::cout << "SgLabelStatement is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgReturnStmt:
			{
			SgReturnStmt* ret = isSgReturnStmt(stat);
			SgExpression* retExp = ret->get_expression();
			std::string retExpStr = getSgExpressionString(retExp);
			statStr = "; return statement not yet linked to function\n ;" + retExpStr + "\n";	
			break;
			}

		case V_SgSequenceStatement:

			{
			std::cout << "SgSequenceStatement is not yet implemented" << std::endl; ;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgPassStatement:

			{
			std::cout << "SgPassStatement is not yet implemented" << std::endl; ;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgNullStatement:

			{
			std::cout << "SgNullStatement is not yet implemented" << std::endl; ;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgNullifyStatement:

			{
			std::cout << "SgNullifyStatement is not yet implemented" << std::endl; ;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgExecStatement:

			{
			std::cout << "SgExecStatement is not yet implemented" << std::endl; ;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgElseWhereStatement:

			{
			std::cout << "SgElseWhereStatement is not yet implemented" << std::endl; ;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgScopeStatement:
			{
			std::cout << "SgScopeStatement should not be found here! " << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgDeclarationStatement:
			{
			std::cout << "SgDeclarationStatement should not be found here! " << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgIOStatement:
			{
			std::cout << "SgIOStatement should not be found here! " << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgJavaThrowStatement:
			{
			std::cout << "SgJavaThrowStatement should not be found here! " << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgJavaSynchronizedStatement:
			{
			std::cout << "SgJavaSynchronizedStatement should not be found here! " << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgOmpBarrierStatement:
			{
			std::cout << "SgOmpBarrierStatement should not be found here! " << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgOmpBodyStatement:
			{
			std::cout << "SgOmpBodyStatement should not be found here! " << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgOmpFlushStatement:
			{
			std::cout << "SgOmpFlushStatement should not be found here! " << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		case V_SgOmpTaskwaitStatement:
			{
			std::cout << "SgOmpTaskwaitStatement should not be found here! " << std::endl;
			ROSE_ASSERT(false);
			break;
			}

		default:
			std::cout << "unknown SgStatement type!: " << stat->class_name() << std::endl;
			ROSE_ASSERT(false);		
	}
	}
	return statStr;
}


std::string getSgUnaryOp(SgUnaryOp* unaryOp) {
	std::string unaryOpString;
	VariantT var = unaryOp->variantT();
	switch (var) {
		case V_SgAddressOfOp:
			std::cout << "SgAddressOfOp not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			unaryOpString = "";
			break;
		case V_SgBitComplementOp:
			std::cout << "SgBitComplementOp is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			unaryOpString = "";
			break;
		case V_SgCastExp:
			{
			SgExpression* casted = unaryOp->get_operand();
			unaryOpString = getSgExpressionString(casted);
			break;
			}
		case V_SgConjugateOp:
			std::cout << "SgConjugateOp is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			unaryOpString = "";
			break;
		case V_SgExpressionRoot:
			std::cout << "SgExpressionRoot is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			unaryOpString = "";
			break;
		case V_SgImagPartOp:
			std::cout << "SgImagPartOp is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			unaryOpString = "";
			break;
		case V_SgMinusMinusOp:
			std::cout << "SgMinusMinusOp is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			unaryOpString = "";
			break;
		case V_SgMinusOp:
			{
			SgExpression* operand = unaryOp->get_operand();
			std::string toMinus = getSgExpressionString(operand);
			std::string minusedExp;
			minusedExp = "(- " + toMinus + ")";
			unaryOpString = minusedExp;
			break;
			}
		case V_SgNotOp:
			std::cout << "SgNotOp is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			unaryOpString = "";
			break;
		case V_SgPlusPlusOp:
			std::cout << "SgPlusPlusOp is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			unaryOpString = "";
			break;
		case V_SgPointerDerefExp:
			std::cout << "SgPointerDerefExp is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			unaryOpString = "";
			break;
		case V_SgRealPartOp:
			std::cout << "SgRealPartOp is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			unaryOpString = "";
			break;
		case V_SgThrowOp:
			std::cout << "SgThrowOp is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			unaryOpString = "";
			break;
		case V_SgUnaryAddOp:
			std::cout << "SgUnaryAddOp is not yet implemented" << std::endl;
			ROSE_ASSERT(false);
			unaryOpString = "";
			break;
		case V_SgUserDefinedUnaryOp:
			std::cout << "SgUserDefinedUnaryOp is not yet implemented";
			ROSE_ASSERT(false);
			unaryOpString = "";
			break;
		default:
			std::cout << "Unknown unary op! " << unaryOp->class_name() << std::endl;
			ROSE_ASSERT(false);
			unaryOpString = "";
			break;
		}
	return unaryOpString;
} 

std::string getSgCompoundAssignOp(SgCompoundAssignOp* node) {
        VariantT var = node->variantT();
        std::string opStr = "";
        switch (var) {
                case V_SgAndAssignOp:
                        {
                        std::cout << "SgAndAssignOp is not yet implemented" << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }
                
                case V_SgDivAssignOp:
                        {
			opStr = "/";
                        //std::cout << "SgDivAssignOp is not yet implemented" << std::endl;
                        //ROSE_ASSERT(false);
                        break;
                        }
                
                case V_SgIntegerDivideAssignOp:
                        {
			opStr = "cdiv";
                        break;
                        }
                
                case V_SgMinusAssignOp:
                        {
			opStr = "-";
                        break;
                        }
                
                case V_SgModAssignOp:
                        {
			opStr = "cmod";
                        break;
                        }
                
                case V_SgMultAssignOp:
                        {
			opStr = "*";
                        break;
                        }
                
                case V_SgPlusAssignOp:
                        {
			opStr = "+";
                        break;
                        }
                
                case V_SgExponentiationAssignOp:
                        
                        {
                        std::cout << "SgExponentiationAssignOp is not yet implemented" << std::endl; ;
                        ROSE_ASSERT(false);
                        break;
                        }
                
                case V_SgIorAssignOp:
                        
                        {
                        std::cout << "SgIorAssignOp is not yet implemented" << std::endl; ;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgLshiftAssignOp:

                        {
                        std::cout << "SgLshiftAssignOp is not yet implemented" << std::endl; ;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgRshiftAssignOp:

                        {
                        std::cout << "SgRshiftAssignOp is not yet implemented" << std::endl; ;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgXorAssignOp:

                        {
                        std::cout << "SgXorAssignOp is not yet implemented" << std::endl; ;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgJavaUnsignedRshiftAssignOp:
                        {
                        std::cout << "SgJavaUnsignedRshiftAssignOp should not be found here! " << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                default:
                        {
                        std::cout << " Unknown node type!: " << node->class_name() << std::endl;
                        ROSE_ASSERT(false);
                        }
        }
        return(opStr);
}

std::string getSgBinaryOp(SgBinaryOp* op) {
	VariantT binOpVariant = op->variantT();
	std::string retStr;
	if (isSgCompoundAssignOp(op)) {
		SgCompoundAssignOp* compAssign = isSgCompoundAssignOp(op);
		retStr = getSgCompoundAssignOp(compAssign);
	}
	else {
	switch (binOpVariant) {
		case V_SgAddOp:
			retStr = "+";
			break;
		case V_SgSubtractOp:
			retStr = "-";
			break;
		case V_SgMultiplyOp:
			retStr = "*";
			break;
		case V_SgDivideOp:
			retStr = "/";
			break;
		case V_SgEqualityOp:
			retStr = "=";
			break;
		case V_SgGreaterThanOp:
			retStr = ">";
			break;
		case V_SgGreaterOrEqualOp:
			retStr = ">=";
			break;
		case V_SgLessThanOp:
			retStr = "<";
			break;
		case V_SgLessOrEqualOp:
			retStr = "<=";
			break;
		case V_SgIntegerDivideOp:
			retStr = "cdiv";
			break;
		case V_SgAssignOp:
			retStr = "assign";
			break;
		case V_SgNotEqualOp:
			retStr = "neq";
			break;
		case V_SgModOp:
			retStr = "cmod";
			break;
		case V_SgPntrArrRefExp:
			retStr = "select";
			break;
		case V_SgAndOp:
			retStr = "and";
			break;
		case V_SgOrOp:
			retStr = "or";
			break;
		default:
			retStr = "unknown";
			

		}
	}
	return retStr;
}



std::string getSgExpressionString(SgExpression* expNode) {
	//bool returnString = false;
	std::string expString;
	bool set_to_expression_val = false;
	VariantT var = expNode->variantT();
	if (isSgBinaryOp(expNode)) {
		SgBinaryOp* binOp = isSgBinaryOp(expNode);
		SgExpression* lhs_exp = binOp->get_lhs_operand();
		SgExpression* rhs_exp = binOp->get_rhs_operand();
		expString = writeSgBinaryOpZ3(binOp, lhs_exp, rhs_exp);
		if (isSgAssignOp(binOp) || isSgCompoundAssignOp(binOp)) {
			declarations.push_back(expString);
			expString = "";
		}
		else if (isSgEqualityOp(binOp) || isSgGreaterThanOp(binOp) || isSgGreaterOrEqualOp(binOp) || isSgLessThanOp(binOp) || isSgLessOrEqualOp(binOp) || isSgNotEqualOp(binOp) || isSgAndOp(binOp) || isSgOrOp(binOp)) {
		
		//set_to_expression_val = true;
		
		}
	}
	else if (isSgUnaryOp(expNode)) {
		expString = getSgUnaryOp(isSgUnaryOp(expNode));
	}
	else if (isSgValueExp(expNode)) {
		expString = getSgValueExp(isSgValueExp(expNode));
	}
	else {
	switch (var) {
		case V_SgCallExpression:
			std::cout << "SgCallExpression not yet implemented" << std::endl;
			expString = "";
			ROSE_ASSERT(false);
			break;
		case V_SgClassNameRefExp:
			std::cout << "SgClassNameRefExp not yet implemented" << std::endl;
			expString = "";
			ROSE_ASSERT(false);
			break;
		case V_SgConditionalExp:
			std::cout << "SgConditionalExp (trinary A ? B : C) not yet implemented" << std::endl;
			expString = "";
			ROSE_ASSERT(false);
			break;		
		case V_SgExprListExp:
			std::cout << "SgExprListExp is not yet implemented" << std::endl;
			expString = "";
			ROSE_ASSERT(false);
			break;
		case V_SgFunctionRefExp:
			std::cout << "SgFunctionRefExp is not yet implemented" << std::endl;
			expString = "";
			ROSE_ASSERT(false);
			break;
		case V_SgDeleteExp:
			std::cout << "SgDeleteExp is not yet implemented" << std::endl;
			expString = "";
			ROSE_ASSERT(false);
			break;
		case V_SgInitializer:
			std::cout << "SgInitializer is not yet implemented" << std::endl;
			expString = "";
			ROSE_ASSERT(false);
			break;
		case V_SgNaryOp:
			std::cout << "SgNaryOp is not yet implemented" << std::endl;
			expString = "";
			ROSE_ASSERT(false);
			break;
		case V_SgNewExp:
			std::cout << "SgNewExp is not yet implemented" << std::endl;
			expString = "";
			ROSE_ASSERT(false);
			break;
		case V_SgNullExpression:
		
			expString = "; null expression";
			break;
		case V_SgRefExp:
			std::cout << "SgRefExp is not yet implemented" << std::endl;
			expString = "";
			ROSE_ASSERT(false);
			break;
		case V_SgSizeOfOp:
			std::cout << "SgSizeOfOp is not yet implemented" << std::endl;
			expString = "";
			ROSE_ASSERT(false);
			break;
		case V_SgStatementExpression:
			std::cout << "SgStatementExpression is not yet implemented" << std::endl;
			expString = "";
			ROSE_ASSERT(false);
			break;
		case V_SgValueExp:
			std::cout << "V_SgValueExp should never be encountered" << std::endl;
			ROSE_ASSERT(false);
			expString = "";
			break;
			
		case V_SgVarRefExp:
			expString = getSgVarRefExp(isSgVarRefExp(expNode));
			break;
		default:
			std::cout << expNode->class_name() << " is not being considered for implementation";
			expString = "";
			ROSE_ASSERT(false);
		}
		}
			
		/*if (set_to_expression_val) {
		std::stringstream exp_var;
		exp_var << "e_" << expression_count;
		expression_count++;
		
		std::string exp_var_decl = "(declare-const " + exp_var.str() + " Bool)";
		variables.push_back(exp_var_decl);
		std::string exp_var_val = "(assert (= " + exp_var.str() + " " + expString + "))";	
		expressions.push_back(exp_var_val);	
return exp_var.str();
		}
		else {
			return expString;
		}*/
		return expString;
	}

std::string getSgVarRefExp(SgVarRefExp* varRefExp) {
	SgVariableSymbol* varSym = varRefExp->get_symbol();
	std::string varName;
	if (SymbolToZ3.find(varSym) == SymbolToZ3.end()) {
		#ifndef ALLOW_UNKNOWN_VARIABLE_SYMBOL
			std::cout << "ERROR, unknown variable symbol" << std::endl;
			ROSE_ASSERT(false);
		#endif
		SgInitializedName* initNam = varSym->get_declaration();
		ROSE_ASSERT(SymbolToInstances.find(varSym) == SymbolToInstances.end());
		std::string init_str = initializeVariable(initNam);
		declarations.push_back(init_str);
		std::string retStr = getSgVarRefExp(varRefExp);	
		return retStr;
		//ROSE_ASSERT(SymbolToInstances.find(varSym) == SymbolToInstan
		//sitd::cout << "ERROR, unknown variable symbol referenced!" << std::endl;
		//ROSE_ASSERT(false);
		
	}
	else {
		varName = SymbolToZ3[varSym];
	}
	int instance = SymbolToInstances[varSym];
	std::stringstream varNameStr;
	varNameStr << varName << "_" << instance;
	return varNameStr.str();
}

std::string getSgValueExp(SgValueExp* valExp) {
	VariantT var = valExp->variantT();
	std::stringstream val;
	switch (var) {
		case V_SgDoubleVal:
			val << isSgDoubleVal(valExp)->get_value();
			break;
		case V_SgFloatVal:
			val << isSgFloatVal(valExp)->get_value();
			break;
		case V_SgShortVal:
			val << isSgShortVal(valExp)->get_value();
			break;
		case V_SgLongIntVal:
			val << isSgLongIntVal(valExp)->get_value();
			break;
		case V_SgLongLongIntVal:
			val << isSgLongLongIntVal(valExp)->get_value();
			break;
		case V_SgIntVal:
			val << isSgIntVal(valExp)->get_value();
			break;
		case V_SgLongDoubleVal:
			val << isSgLongDoubleVal(valExp)->get_value();
			break;
		case V_SgEnumVal:
			{
			SgEnumVal* enumValExp = isSgEnumVal(valExp);
			val << enumValExp->get_name().getString();
			break;
			}
			
			
		default:
			std::cout << "SgValueExp : " << valExp->class_name() << std::endl;
			ROSE_ASSERT(false);
		}
		return val.str();
	}
	
std::string writeSgBinaryOpZ3(SgBinaryOp* op, SgExpression* lhs, SgExpression* rhs) {
	
	std::stringstream ss;
	std::string opStr;
	bool compAssign = false;
	if (isSgCompoundAssignOp(op)) {
		compAssign = true;
		opStr = getSgCompoundAssignOp(isSgCompoundAssignOp(op));
	
	}
	else {
		opStr = getSgBinaryOp(op);
	}
	ROSE_ASSERT(opStr != "unknown");
	std::string rhsstring;
	std::string lhsstring;
	lhsstring = getSgExpressionString(lhs);
	SgType* lhstyp;
	SgType* rhstyp;
	if (isSgArrayType(lhs->get_type())) {
	lhstyp = isSgArrayType(lhs->get_type())->get_base_type();	
	}
	else {
	lhstyp = lhs->get_type();
	}
	if (isSgArrayType(rhs->get_type())) {
	rhstyp = isSgArrayType(rhs->get_type())->get_base_type();
	}
	else {
	rhstyp = rhs->get_type();
	}
	if (isSgEnumType(lhs->get_type())) {
	}
	else {	
	ROSE_ASSERT(lhstyp == rhstyp);
	}	
	if (isSgValueExp(rhs)) {
		rhsstring = getSgValueExp(isSgValueExp(rhs));
	}
	else if (isSgUnaryOp(rhs)) {	
		rhsstring = getSgUnaryOp(isSgUnaryOp(rhs));
	}
		
	else {
		rhsstring = getSgExpressionString(rhs);
	}
	if (opStr == "/" && lhstyp->isIntegerType()) {
		opStr = "cdiv";
	} 
	if (opStr == "assign" || compAssign) {
		if (isSgVarRefExp(lhs)) {
		SgVarRefExp* lhsSgVarRefExp = isSgVarRefExp(lhs);
		int instances = SymbolToInstances[lhsSgVarRefExp->get_symbol()];
		std::stringstream instanceName;
		SymbolToInstances[lhsSgVarRefExp->get_symbol()] = instances + 1;
		std::string lhsname = SymbolToZ3[lhsSgVarRefExp->get_symbol()];
		instanceName << lhsname << "_" << (instances+1);
		SgType* varType = lhsSgVarRefExp->get_type();
		std::string typeZ3;
		if (varType->isFloatType()) {
			typeZ3 = "Real";
		}
		else if (varType->isIntegerType()) {
			typeZ3 = "Int";
		}
		else if (isSgEnumType(varType)) {
			typeZ3 = isSgEnumType(varType)->get_name().getString();
		}
		else {
			typeZ3 = "Unknown";
		}
		ss << "(declare-fun " << instanceName.str() << " () " << typeZ3 << ")\n";
		if (!compAssign) {  		
			ss << "(assert (= " << instanceName.str() << " " << rhsstring << "))";
		}
		else {
			std::stringstream oldInstanceName;
			oldInstanceName << lhsname << "_" << instances;
			ss << "(assert (= " << instanceName.str() << " (" << opStr << " " << oldInstanceName.str() << " " << rhsstring << ")))"; 
		}
		}
		
		else {
			ROSE_ASSERT(isSgPntrArrRefExp(lhs));
			std::string u_type;
			SgPntrArrRefExp* lhspntr = isSgPntrArrRefExp(lhs);
			SgVarRefExp* varlhspntr = isSgVarRefExp(lhspntr->get_lhs_operand());
			SgArrayType* arrTy = isSgArrayType(varlhspntr->get_type());
			if (arrTy->get_base_type()->isIntegerType()) {
				u_type = "Int";
			}
			else if (arrTy->get_base_type()->isFloatType()) {
				u_type = "Real";
			}
			else {
				std::cout << "unknown base type for array" << std::endl;
				ROSE_ASSERT(false);
			}
			std::stringstream oldInstanceName;
			SgVarRefExp* varexp = isSgVarRefExp((isSgPntrArrRefExp(lhs))->get_lhs_operand());
			oldInstanceName << SymbolToZ3[varexp->get_symbol()] << "_" << SymbolToInstances[varexp->get_symbol()];	
			int instances = SymbolToInstances[varexp->get_symbol()];
                	std::stringstream instanceName;
                	SymbolToInstances[varexp->get_symbol()] = instances + 1;
                	std::string lhsname = SymbolToZ3[varexp->get_symbol()];
                	instanceName << lhsname << "_" << instances+1;
			ss << "(declare-const " << instanceName.str() << " (Array Int " << u_type << "))\n ";	
			std::string indexstring = getSgExpressionString(isSgPntrArrRefExp(lhs)->get_rhs_operand());	
			ss << "(assert (= (store " << oldInstanceName.str() << " " << indexstring << " " << rhsstring << ") " << instanceName.str() << "))";
		}
	}	
	else if (opStr == "neq") {
	ss << "(not (= " << lhsstring << " " << rhsstring << "))";
	}
	else if (opStr == "or" || opStr == "and") {
		std::stringstream val_stream;	
		if (pathNodeTruthValue.find(op) != pathNodeTruthValue.end()) {
		bool logic_val = pathNodeTruthValue[op];
		//std::cout << ";and/or lhsstring " << lhsstring << "\n";
		//std::cout << ";and/or rhsstring " << rhsstring << "\n";
		if (opStr == "and") {
				
			if (logic_val) {
			
				std::string p_decl = "(assert (= " + lhsstring + " true))";
				declarations.push_back(p_decl);
				ss << rhsstring;
				//ss << "(and " << lhsstring << " " << rhsstring << ")";

			}
			else {
				std::string p_decl = "(assert (= " + lhsstring + " false))";
				declarations.push_back(p_decl);
				ss << "false";	
			}
		}
		else {
			if (logic_val) {
				std::string p_decl = "(assert (= " + lhsstring + " true))";
				declarations.push_back(p_decl);
				ss << "true"; 
			}
			else {
				std::string p_decl = "(assert (= " + lhsstring + " false))";
				declarations.push_back(p_decl);
				ss << rhsstring;
			}
		}
	}
	else {
		ss << "";
	}
	}
	else {	
	ss << "(" << opStr << " " << lhsstring << " " << rhsstring << ")";
	}
	return ss.str();
}

