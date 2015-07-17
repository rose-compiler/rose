std::map<SgVariableSymbol*, int> getEncodedVariable
std::map<int,SgVariableSymbol*> getVariableFromEncoding;

std::string returnDereferenceString(std::string loc, std::string result) {
	std::string s_exp;
	s_exp = "(!S " + loc + " !D " + result + ")";
	return s_exp;
}

std::string returnDirectFieldAccessString(std::string loc, std::string field, std::string result) {
	std::string s_exp;
	s_exp = "(!S " + loc + " " + field + " " + result + ")";
	return s_exp;
}

std::string returnIndirectFieldAccessString(std::string loc, std::string field, std::string result) {
	std::string s_exp;
	s_exp = "(!S " + loc + " !D !tmpRes)\n";
	s_exp += "(!S !tmpRes !D " + field + " " + result + ")";
	return s_exp;
}

std::string returnAddressFunctionString(std::string loc, std::string address) {
	std::string s_exp;
	s_exp = "(!A " + loc + " " + address + ")";
	return s_exp;
}

std::string returnVariableFunctionString(std::string var, std::string value) {
	std::string s_exp;
	s_exp = "(!V " + var + " "  + value + ")";
	return s_exp;
}

std::string returnNthArrayAddress(std::string loc, std::string field, std::string fieldLen, std::string n, std::string result) {
	std::string s_exp;
	s_exp = "(!S " + loc + " "  + field + " !tmpRes)\n";
	s_exp += "(!T " + loc + " " + fieldLen + " !tmpRes2)\n";
	s_exp += "(= (+ !tmpRes (* " + n  + " !tmpRes2)) + " + result + ")\n"; 
	return s_exp;
}

std::string nextFieldAddress(std::string loc, std::string field, std::string fieldLen, std::string result) {
	std::string s_exp;
	s_exp = "(!S " + loc + " " + field + " !tmpRes)\n";
	s_exp += "(!T " + loc + " " + fieldLen + " !tmpRes2)\n";
	s_exp += "(= (+ !tmpRes !tmpRes2) " + result + ")";
}	


	
