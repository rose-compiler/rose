#include "rose.h"

void get_pointer_type(SgType*,int);
void get_array_type(SgType*,int);

void get_pointer_type(SgType* t1, int dim) {
	std::cout << "dim: " << dim << " pointer type " << std::endl;
	
	SgType* t = isSgPointerType(t1)->get_base_type();
	dim = dim + 1;
	if (isSgPointerType(t)) {
		get_pointer_type(t,dim);
	}
	else if (isSgArrayType(t)) {
		get_array_type(t,dim);
	}
	else {
		std::cout << "base_type = " << t->class_name() << std::endl;
	}
	return;
}
void get_array_type(SgType* t1, int dim) {
	 std::cout << "dim: " << dim << " array type " << std::endl;
	SgType* t = isSgArrayType(t1)->get_base_type();
	dim = dim + 1;
	if (isSgPointerType(t)) {
		//std::cout << "dim: " << dim << " pointer type " << std::endl;
		get_pointer_type(t,dim);
	}
	else if (isSgArrayType(t)) {
		//std::cout << "dim: " << dim << " array type " << std::endl;	
		get_array_type(t, dim);
		
	}
	else {
		std::cout << "base_type = " << t->class_name() << std::endl;
	}
	return;
}
int main(int argc, char* argv[]) {
	SgProject* proj = frontend(argc,argv);
	SgFunctionDeclaration* mainDecl = SageInterface::findMain(proj);
	SgFunctionDefinition* mainDef = mainDecl->get_definition();
	std::vector<SgNode*> varRefNodes = NodeQuery::querySubTree(mainDef, V_SgVarRefExp);
	for (unsigned int i=0; i < varRefNodes.size(); i++) {
		SgVarRefExp* var_ref = isSgVarRefExp(varRefNodes[i]);
		SgType* var_type = var_ref->get_type();
		
		std::cout << "type: " << var_type->class_name() << std::endl;
		if (isSgArrayType(var_type)) {
			get_array_type(var_type,0);
			/*int dim = 0;
			SgType* tmp_type = var_type;
		
			while (isSgArrayType(tmp_type)) {
			
			if (isSgArrayType(tmp_type)) {
				dim += 1;
				tmp_type = (isSgArrayType(tmp_type))->get_base_type();
			}
			else {
				break;
			}
			}
			//tmp_type = (isSgArrayType(var_type))->get_base_type();
			//std::cout << "base type: " << base_type->class_name() << std::endl;
			
			SgType* base_type = tmp_type;
			std::cout << "base type:  " << base_type->class_name() << " dim = " << dim << std::endl;
		*/
		}
		if (isSgPointerType(var_type)) {
			get_pointer_type(var_type,0);
/*
			SgType* tmp_type = var_type;
			int dim = 0;
			while (isSgPointerType(tmp_type)) {
				dim++;
				tmp_type = isSgPointerType(tmp_type)->get_base_type();
			}
			SgType* base_type = tmp_type;
			//SgType* base_type = (isSgPointerType(var_type))->get_base_type();
			std::cout << "base type: " << base_type->class_name() << " dim: " << dim << std::endl;
		*/
		}
	}
	return 0;
}	
