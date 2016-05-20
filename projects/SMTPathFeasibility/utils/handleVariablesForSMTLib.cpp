#include "utilHeader.h"
//#include "handleVariablesForSMTLib.h"

std::map<SgSymbol*,int> SymbolToInstances;

std::map<SgVariableSymbol*, std::string> varExpressionValue;

std::set<SgVariableSymbol*> varSymbolList;
std::map<SgSymbol*,std::string> SymbolToZ3;
/*
*/
std::map<SgScopeStatement*,int> scopeStatementToInteger;

/*
*/
std::map<SgVariableSymbol*,int> variableSymbolToInstanceIdentifier;

std::map<SgVariableSymbol*,std::string> symbolToConvertedString;

std::map<std::string,int> variablesOfNameX;
/*
std::string get_type_identifier(SgType* typ);
std::string getVarName(SgInitializedName* nam);
std::string getVarName(SgVarRefExp*);
std::string getVarName(SgVariableSymbol*);
void getSgDeclarationStatement(SgDeclarationStatement*);
std::string getSExpr(SgExpression*);
std::string getNewVar(SgVarRefExp*);
std::string getNewVar(SgInitializedName*);
*/

std::map<std::string,std::string> origNameToSMTName;

DFAnalysis* defuse;


std::string constructConditional(SgExpression* conditional) {
        std::string conditionalSExpr = getSExpr(conditional);
        return conditionalSExpr;
}


void initDefUseAnalysis(SgProject* proj) {
        defuse = new DefUseAnalysis(proj);
        /*bool debug = false;
        defuse->run(debug);*/
        defuse->run(false);
        return;
}
//string output if analyzeVariableReference is not working
// correctly
void debugAnalyzeVariableReference(SgVarRefExp* varRef) {
        SgInitializedName* initName = isSgInitializedName(varRef->get_symbol()->get_declaration());
        std::vector<SgNode*> vec = defuse->getDefFor(varRef,initName);
        ROSE_ASSERT(vec.size() > 0);
        std::vector<SgNode*>::iterator i = vec.begin();
        for (; i != vec.end(); i++) {
                SgStatement* currDefStmt = SageInterface::getEnclosingStatement(*i);
                std::cout << "vec entry class name: " << (*i)->class_name() << std::endl;
                std::cout << "def statement class name: " << currDefStmt->class_name();
                std::cout << "def statement string: " << currDefStmt->unparseToString() << std::endl;
        }
        return;
}
std::vector<SgStatement*> analyzeVariableDefUse(SgVarRefExp* varRef) {
        SgInitializedName* initName = isSgInitializedName(varRef->get_symbol()->get_declaration());
        std::string name = initName->get_qualified_name().str();
        std::vector<SgNode*> vec = defuse->getDefFor(varRef,initName);
        std::vector<SgStatement*> retVec;
        ROSE_ASSERT(vec.size() > 0);
        for (int i = 0; i < vec.size(); i++) {
                SgStatement* currStat = SageInterface::getEnclosingStatement(vec[i]);
                retVec.push_back(currStat);
        }
        return retVec;
}




std::string getAssignOp(SgBinaryOp* binOp,SgExpression* lhs,SgExpression* rhs) {
        std::string assignString = "";
        if (isSgVarRefExp(lhs)) {
                //std::string varName = getVarName(vAssign);
                
                std::string rhs_expr = getSExpr(rhs);
                convertAssignOp(binOp,lhs,rhs);
                std::string varName = getNewVar(isSgVarRefExp(lhs));
                assignString += "(= " + varName + " " + rhs_expr + ")";
        }
        else if (isSgPntrArrRefExp(lhs)) {
                assignString += "Currently cannot handle Pointer Array Reference Expressions";
        }
        else {
                assignString += "Currently can't handle left hand side of type: " + lhs->class_name();
        }
        return assignString;            
}

std::string convertCompoundAssignOp(SgCompoundAssignOp* binOp, SgExpression*lhs, SgExpression* rhs) {
        SgVarRefExp* exp = isSgVarRefExp(lhs);
        std::string retStr;
        if (exp) {
                        varExpressionValue[exp->get_symbol()] = getSExpr(rhs);
                        retStr = varExpressionValue[exp->get_symbol()];


        }
        else {
                retStr = "cannot convert given lhs" + lhs->class_name() + "\n";
                //ROSE_ASSERT(false);
        }
	std::cout << "convertCompoundAssignOp: " << retStr << std::endl;
        return retStr;
}


std::string convertAssignOp(SgBinaryOp* binOp, SgExpression* lhs, SgExpression* rhs) {
        SgVarRefExp* exp = isSgVarRefExp(lhs);
        std::string retStr;
        if (exp) {
                        varExpressionValue[exp->get_symbol()] = getSExpr(rhs);
                        retStr = varExpressionValue[exp->get_symbol()];


        }
        else {
                retStr = "cannot convert given lhs" + lhs->class_name() + "\n";
                //ROSE_ASSERT(false);
        }
        return retStr;
}


std::string getCompoundAssignOp(SgCompoundAssignOp* binOp,SgExpression* lhs, SgExpression* rhs) {
        std::string rhsString = getSExpr(rhs);
        VariantT var = binOp->variantT();
        std::string opStr = "";
        std::string retstr = "";
        switch (var) {
                case V_SgAndAssignOp:
                        {
                        opStr = "cand";
                        break;
                        }

                case V_SgDivAssignOp:
                        {
                        opStr = "/";
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
                        opStr = "expontentiate";
                        break;
                        }
             case V_SgIorAssignOp:

                        {
                        opStr = "Ior";
                        break;
                        }

                case V_SgLshiftAssignOp:

                        {
                        opStr = "Lshift";
                        break;
                        }

                case V_SgRshiftAssignOp:

                        {
                        opStr = "Rshift";
                        break;
                        }

                case V_SgXorAssignOp:

                        {
                        opStr = "Xor";
                        break;
                        }


                case V_SgJavaUnsignedRshiftAssignOp:
                        {
                        opStr = "JavaUnsignedRshiftAssignOp";
                        break;
                        }

                default:
                        {
                        opStr = "UnknownOperator" + binOp->class_name();
                        break;
                        }
        }
        if (isSgVarRefExp(lhs)) {
                SgVarRefExp* varRefExp = isSgVarRefExp(lhs);
              	
		//convertCompoundAssignOp(binOp,lhs, rhs);
		std::string varStr = getVarName(varRefExp);
		std::string rhs_plus_compound = "(" + opStr + " " + varStr + " " + getSExpr(rhs) + ")\n";
		varExpressionValue[varRefExp->get_symbol()] = rhs_plus_compound;
		//std::string varStr = getVarName(varRefExp);
                std::string varNew = getNewVar(varRefExp);
		
                retstr += "(= " + varNew + "(" + opStr + " " + varStr + " " + rhsString + "))";
        	
	//	std::cout << "retstr: " << retstr << std::endl;
	}
        else {
		ROSE_ASSERT(false);
                retstr += "(= newVarVersion (" + opStr + " newVarVersion " + rhsString + "))";
        }
        return(retstr);
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
	origNameToSMTName[initNameName.getString()] = nameStringStream.str();
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
                index_expression_string = getSExpr(ind);
                typeZ3 = "(Array Int " + array_typeZ3 + ")";
        }
        else if (isSgClassType(initNameType)) {

                std::cout << "structs are not yet implemented" << std::endl;
                ROSE_ASSERT(false);
  }
        else if (isSgPointerType(initNameType)) {
		typeZ3 = "Real";
                //std::cout << "pointers are not yet implemented" << std::endl;
                //ROSE_ASSERT(false);
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
	streamZ3 << "(declare-var " << name << " " << typeZ3 << ")";
//        streamZ3 << "(declare-fun " << name << " () " << typeZ3 << ")";
        }
        return streamZ3.str();
}



std::string getNewVar(SgVarRefExp* exp) {

        std::string retStr = "";
	
        SgVariableSymbol* expVarSymbol = exp->get_symbol();
        if (varSymbolList.find(expVarSymbol) == varSymbolList.end()) {
                varSymbolList.insert(expVarSymbol);
        }
        if (variableSymbolToInstanceIdentifier.find(expVarSymbol) == variableSymbolToInstanceIdentifier.end()) {
                std::string varNam = getVarName(exp);
        }
//      ROSE_ASSERT(variableSymbolToInstanceIdentifier.find(expVarSymbol) != variableSymbolToInstanceIdentifier.end());
        variableSymbolToInstanceIdentifier[expVarSymbol]++;
        retStr += getVarName(exp);
        return retStr;
}

std::string getNewVar(SgInitializedName* nam) {
        std::string retStr = "";
        ROSE_ASSERT(nam->search_for_symbol_from_symbol_table());
        SgSymbol* sym = nam->search_for_symbol_from_symbol_table();
        ROSE_ASSERT(isSgVariableSymbol(sym));
        SgVariableSymbol* varSym = isSgVariableSymbol(sym);
        if (varSymbolList.find(varSym) == varSymbolList.end()) {
                varSymbolList.insert(varSym);
        }
        variableSymbolToInstanceIdentifier[varSym]++;
        retStr += getVarName(nam);
        return retStr;
}


std::string getVarName(SgVarRefExp* exp) {
        std::string retStr = getVarName(exp->get_symbol());
        return retStr;
}

std::string getVarName(SgVariableSymbol* expVarSymbol) {
//      SgVariableSymbol* expVarSymbol = exp->get_symbol();
        SgScopeStatement* symbolScope = expVarSymbol->get_scope();
        ROSE_ASSERT(scopeStatementToInteger.find(symbolScope) != scopeStatementToInteger.end());
        int scopeId = scopeStatementToInteger[symbolScope];
        SgType* symbolType = expVarSymbol->get_type();
        std::string typeId = get_type_identifier(symbolType);
        if (varSymbolList.find(expVarSymbol) == varSymbolList.end()) {
                varSymbolList.insert(expVarSymbol);
        }
        if (variableSymbolToInstanceIdentifier.find(expVarSymbol) == variableSymbolToInstanceIdentifier.end()) {
                variableSymbolToInstanceIdentifier[expVarSymbol] = 0;
        }
        std::stringstream instancestream;
        int instance = variableSymbolToInstanceIdentifier[expVarSymbol];
        instancestream << instance;
        std::stringstream scopestream;
        scopestream << scopeId;
        std::string name = expVarSymbol->get_name().getString() + "_" + typeId + "_s" + scopestream.str();
                symbolToConvertedString[expVarSymbol] = name;
		if (origNameToSMTName.find(expVarSymbol->get_name().getString()) == origNameToSMTName.end()) {
			origNameToSMTName[expVarSymbol->get_name().getString()] = name;
		}
        return name;
}



std::string getVarName(SgInitializedName* nam) {
        SgVariableSymbol* expVarSymbol = isSgVariableSymbol(nam->search_for_symbol_from_symbol_table());//get_symbol();
        SgScopeStatement* symbolScope = nam->search_for_symbol_from_symbol_table()->get_scope();
        ROSE_ASSERT(scopeStatementToInteger.find(symbolScope) != scopeStatementToInteger.end());
        int scopeId = scopeStatementToInteger[symbolScope];
        SgType* symbolType = expVarSymbol->get_type();
        std::string typeId = get_type_identifier(symbolType);
        if (varSymbolList.find(expVarSymbol) == varSymbolList.end()) {
                varSymbolList.insert(expVarSymbol);
        }
        if (variableSymbolToInstanceIdentifier.find(expVarSymbol) == variableSymbolToInstanceIdentifier.end()) {
                variableSymbolToInstanceIdentifier[expVarSymbol] = 0;
        }
        std::stringstream instancestream;
        int instance = variableSymbolToInstanceIdentifier[expVarSymbol];
        instancestream << instance;
        std::stringstream scopestream;
        scopestream << scopeId;
        std::string name = nam->search_for_symbol_from_symbol_table()->get_name().getString() + "_" + typeId + "_s" + scopestream.str();
                symbolToConvertedString[expVarSymbol] = name;
        return name;
}


std::string convertVarRefExp(SgVarRefExp* exp) {
        std::string retStr;
        if (varExpressionValue.find(exp->get_symbol()) == varExpressionValue.end()) {
                varExpressionValue[exp->get_symbol()] = getVarName(exp);
                retStr = varExpressionValue[exp->get_symbol()];

        }
        else {
                retStr = varExpressionValue[exp->get_symbol()];
        }
        return retStr;
}

