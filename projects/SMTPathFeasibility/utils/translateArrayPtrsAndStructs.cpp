#include "rose.h"
#include "utilHeader.h"
#include <iostream>
#include <string>
#include <set>
std::string translateVarRefs(SgVarRefExp*, int);
std::string getTabbedString(std::string,int);
std::string translateClass(SgType*,int);
std::string translatePointer(SgVariableDeclaration*,int);
std::string translateArray(SgArrayType*,int,SgName);


std::string translateArray(SgArrayType* arr_typ, int tabs, SgName varName) {
	std::string retVal;
	SgType* base_typ = arr_typ->get_base_type();
	SgExpression* ind_exp = arr_typ->get_index();
	std::cout << "\tname: " << varName.getString()  << std::endl;
	std::cout << "\tbase: "<<base_typ->class_name()<<std::endl;
	std::cout << "\tindex: " << ind_exp->class_name()<<std::endl;		
	std::cout << "\tindex value: ";
	if (isSgValueExp(ind_exp)) {
		ROSE_ASSERT(isSgIntVal(ind_exp));
		std::cout << isSgIntVal(ind_exp)->get_value() << std::endl;
	}
	else {
		std::cout << "\t not a value expression" << std::endl;
	}
	SgExprListExp* dimInfo = arr_typ->get_dim_info();
	return retVal;
}

std::string translateVarRefs(SgVarRefExp* varRefExp,int tabs) {
	std::string retVal;
	std::string tb = getTabbedString("",tabs);
	SgType* varType = varRefExp->get_symbol()->get_type();
	std::cout << tb << "name: " << varRefExp->get_symbol()->get_name().getString() << std::endl;
	std::cout << tb << "type: " << varType->class_name() << std::endl;
	if (isSgPointerType(varType)) {
		SgVariableDeclaration* vD = isSgVariableDeclaration(varRefExp->get_symbol()->get_declaration()->get_declaration());	
		translatePointer(vD,tabs+1);
	}
	if (isSgClassType(varType)) {
		SgVariableDeclaration* vD = isSgVariableDeclaration(varRefExp->get_symbol()->get_declaration()->get_declaration());
		SgType* varDeclType = vD->get_definition()->get_type();
		translateClass(varDeclType,tabs+1);
	}
	if (isSgArrayType(varType)) {
		translateArray(isSgArrayType(varType),tabs+1,varRefExp->get_symbol()->get_name());
	}
	return retVal;
}

std::string getTabbedString(std::string str, int tabs) {
	std::string retVal = "";
	for (int i = 0; i < tabs; i++) {
		retVal+="\t";
	}
	retVal += str;
	return retVal;
}



std::string translateClass(SgType* varDeclType, int tabs) {	
		std::string retVal;
		SgName className = isSgClassType(varDeclType)->get_name();
                std::string tb = getTabbedString("",tabs);
                std::cout << tb << "class: " << className.getString() << std::endl;
                SgNamedType* namType = isSgNamedType(varDeclType);
                SgDeclarationStatement* namTypeDecl = namType->get_declaration();
                SgDeclarationStatement* namTypeDeclFirst = namTypeDecl->get_definingDeclaration();
                SgClassDeclaration* classDecl = isSgClassDeclaration(namTypeDeclFirst);
                SgClassDefinition* classDef = classDecl->get_definition();
                SgDeclarationStatementPtrList statLst = classDef->get_members();
                SgDeclarationStatementPtrList::iterator i;
                i = statLst.begin();
                for (; i != statLst.end(); i++) {
                        SgDeclarationStatement* ithDecl = (*i);
                        if (isSgVariableDeclaration(ithDecl)) {
                                SgVariableDefinition* varDefI = isSgVariableDeclaration(ithDecl)->get_definition();
                                SgName varName = varDefI->get_vardefn()->get_name();
                                SgType* varType = varDefI->get_type();
                                std::string varTypeStr = get_type_identifier(varType);
                                std::string tb = getTabbedString("",tabs);

                                std::cout << tb << "type: " << varTypeStr << ", name: " << varName.getString() << std::endl;
                        }
                        else {
                                std::cout << "not variable declaration" << std::endl;
                        }
                }

        return retVal; 
	}

std::string translatePointer(SgVariableDeclaration* varDecl, int tabs) {
        std::string retVal;
	SgType* varDeclType = varDecl->get_definition()->get_type();
	SgType* base_type = isSgPointerType(varDeclType)->get_base_type();
        std::string tb = getTabbedString("",tabs);
        std::cout << tb << "base type: " << base_type->class_name() << std::endl;
	if (isSgClassType(base_type)) {
		translateClass(base_type,tabs+1);
	}			
	return retVal;		
}

std::string getDirectFieldReference(SgDotExp* dot_exp, std::string soln) {
	SgExpression* lExp = dot_exp->get_lhs_operand();
	SgExpression* rExp = dot_exp->get_rhs_operand();
        std::string translation;
	if (isSgVarRefExp(lExp) && isSgVarRefExp(rExp)) {
            SgName lhs_name = isSgVarRefExp(lExp)->get_symbol()->get_name();
            SgName rhs_name = isSgVarRefExp(rExp)->get_symbol()->get_name();
            std::string field = rhs_name.getString();
            std::string location = lhs_name.getString();
            std::cout << ";direct reference" << std::endl;
            std::string soln = "soln";
            translation = "(S "+location+"' " + field+"' " + soln +")";
        }
	else {
		std::cout << "cannot handle direct field reference with lhs: " << lExp->class_name() << " and rhs: " << rExp->class_name() << std::endl;
	}
	std::cout << translation << std::endl;
	return translation;
}



std::string getIndirectFieldReference(SgArrowExp* arrow_exp, std::string soln1, std::string soln2) {
	std::cout << ";indirect reference" << std::endl;
       	std::string translation = "";
	SgExpression* lExp = arrow_exp->get_lhs_operand();
	SgExpression* rExp = arrow_exp->get_rhs_operand();
	if (isSgVarRefExp(lExp) && isSgVarRefExp(rExp)) {
            SgName lhs_name = isSgVarRefExp(lExp)->get_symbol()->get_name();
            SgName rhs_name = isSgVarRefExp(rExp)->get_symbol()->get_name();
            std::string field = rhs_name.getString();
            std::string location = lhs_name.getString();
            translation = "(and (S " + location + "' D " + soln1 + ")" + "(S " + soln1 + " " + field + "' " + soln2 + "))";                                 
        }
	std::cout << translation << std::endl;
	return translation;	
}

int main(int argc, char** argv) {
	SgProject* proj = frontend(argc,argv);
	SgFunctionDeclaration* mainDecl = SageInterface::findMain(proj);
	SgFunctionDefinition* mainDef = mainDecl->get_definition();
	std::vector<SgNode*> variableDeclarations;
	variableDeclarations = NodeQuery::querySubTree(mainDef, V_SgVariableDeclaration);
	//for (int i = 0; i < variableDeclarations.size(); i++) {
//		translateArrayDeclaration(isSgVariableDeclaration(variableDeclarations[i]),0);
//	}
	std::set<SgVariableSymbol*> varSymbols;
	std::vector<SgNode*> varRefs = NodeQuery::querySubTree(mainDef, V_SgVarRefExp);
	for (int i = 0; i < varRefs.size(); i++) {
		if (varSymbols.find(isSgVarRefExp(varRefs[i])->get_symbol()) == varSymbols.end()) {
		translateVarRefs(isSgVarRefExp(varRefs[i]),0);
		varSymbols.insert(isSgVarRefExp(varRefs[i])->get_symbol());
		}
	}
	std::vector<SgNode*> forLoops = NodeQuery::querySubTree(mainDef, V_SgForStatement);
	for (int i = 0; i < forLoops.size(); i++) {
		std::vector<SgNode*> refsInFor = NodeQuery::querySubTree(forLoops[i],V_SgVarRefExp);
		for (int j = 0; j < refsInFor.size(); j++) {
			translateVarRefs(isSgVarRefExp(varRefs[j]),0);
		}
		std::vector<SgNode*> binaryOpsInFor = NodeQuery::querySubTree(forLoops[i], V_SgBinaryOp);
		for (int j = 0; j < binaryOpsInFor.size(); j++) {
			if (isSgArrowExp(binaryOpsInFor[j]) || isSgDotExp(binaryOpsInFor[j])) {
				if (isSgArrowExp(binaryOpsInFor[j])) {
					std::string indirectRef = getIndirectFieldReference(isSgArrowExp(binaryOpsInFor[j]),"soln1", "soln2");	
				}
				else {
					std::string directRef = getDirectFieldReference(isSgDotExp(binaryOpsInFor[j]),"soln");
				}
			}	
		}
	}
	return 0;
}	
