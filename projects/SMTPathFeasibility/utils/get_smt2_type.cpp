#include<string>



// this can be changed later to allow different naming standards
std::string get_smt_name(SgType* t, std::string name) {
	return name;
}
	

void confirm_unsigned(std::string var_name) {
	return;
}

std::string write_var_declaration(SgType* t, std::string name, std::string declare_suffix, bool bvec=false);
	


std::string write_var_declaration(SgType* t, std::string name, std::string declare_suffix, bool bvec) {
	std::string smt_name = get_smt_name(t,name,declare_suffix)
	std::string declaration;
	std::string type_str;
	if (t->isIntegerType()) {
		if (t->isIntegerType()) {
			type_str = "Int";
		}
		else if (t->isFloatType()) {
			type_str = "Real";
		}
		else if (isSgTypeChar(t) || isSgTypeSignedChar(t)) {
			type_str = "(_ BitVec 8)";
		}
		else if (isSgTypeUnsignedChar(t)) {
			type_str = "(_ BitVec 8)";
			confirm_unsigned(name);
		}
		else if (isSgArrayType(t)) {
			get_array_type(t,type_str);
							
}

std::string declare_fun(SgType* t, std::


