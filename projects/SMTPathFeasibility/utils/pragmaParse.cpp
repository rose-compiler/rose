#include "pragmaParse.h"
std::string get_pragma_string(SgPragmaDeclaration* prdecl) {
        std::string prag;
	ROSE_ASSERT(prdecl!=NULL);
        SgPragma* pr = prdecl->get_pragma();
        prag = pr->get_pragma();
        return prag;
}

bool isSMTGeneralLoop(std::string pstring) {
	if (pstring.find(" loop ") != std::string::npos) {
		return true;
	}
	else {
		return false;
	}
}



std::string parseSMTGeneral(std::string pstring) {
	std::string unroll_number;
	ROSE_ASSERT(pstring.find("smt") != std::string::npos);
	// currently parseSMTGeneral has keyword smt followed by
	// loop
	// more to come
	if (pstring.find(" loop ") != std::string::npos) {
		if (pstring.find(" unroll ") != std::string::npos) {
			size_t unroll_number_itr = pstring.find(" unroll ");
			unroll_number_itr+=8;
			//std::cout << "string at unroll_number_itr: " << pstring[unroll_number_itr] << std::endl;
			size_t end_unroll_number = unroll_number_itr;
			while (pstring[end_unroll_number] != ' ' && pstring[end_unroll_number] != std::string::npos) {
				end_unroll_number++;
			}
				
			unroll_number = pstring.substr(unroll_number_itr,end_unroll_number-unroll_number_itr);
			//std::cout << ";; unroll number length: " << unroll_number.length() << std::endl;
			std::cout << ";; for loop unrolling, unroll number " <<  unroll_number << std::endl;			
			//int unroll_int = atoi(unroll_number.c_str());
			//std::cout << ";; numeric unroll number: " << unroll_int << std::endl;
	}
	else {
		ROSE_ASSERT(false);
	}
	}
	else {
		ROSE_ASSERT(false);
	}
	return unroll_number;
}

bool isSMTGeneral(std::string pstring) {
	if (pstring.find("smt") != std::string::npos) {
		return true;
	}
	else {
		return false;
	}
}

std::string get_name(std::string pstring) {
        //std::string name;
        //std::size_t begin_parse = pstring.find("pragma") + 6;
        //std::size_t curr_pos = pstring.begin();//egin_parse+1;
        int curr_pos = 0;
        if (pstring[curr_pos] == ' ') {
                while (pstring[curr_pos] == ' ') {
                        curr_pos+=1;
                }
        }
        int begin_pos = curr_pos;
        while (pstring[curr_pos] != ' ') {
                curr_pos++;
        }
        int end_pos = curr_pos;
        std::string name = pstring.substr(begin_pos, end_pos-begin_pos);
        //std::cout << "name: " << name << std::endl;

        return name;
}

std::string get_exp_operator(std::string exp) {
	if (exp.find("+")) {
		return "+";
	}
	else if (exp.find("-")) {
		return "-";
	}
	else if (exp.find("%")) {
		return "%";
	}
	else if (exp.find("*")) {
		return "*";
	}
	else if (exp.find("/")) {
		return "/";
	}
	else {
		std::cout << "can't find expression operator in " << exp <<std::endl;
		ROSE_ASSERT(false);
	}
}
	

std::string get_comp_operator(std::string exp) {
	if (exp.find(">=")) {
		return ">=";
	}
	else if (exp.find(">")) {
		return ">";
	}
	else if (exp.find("<=")) {
		return "<=";
	}
	else if (exp.find("<")) {
		return "<";
	}
	else if (exp.find("!=")) {
		return "!=";
	}
	else if (exp.find("=")) {
		return "=";
	}
	else {
		std::cout << "can't find operator in string: " << exp << std::endl;
		ROSE_ASSERT(false);
	}
}
	
/*std::string translate_to_sexp(std::string assumption) {
	std::string op = get_comp_operator(assumption);
	substr

}
*/
std::vector<std::string> get_assumptions(std::string prag_str) {
	//std::cout << "get_assumptions\n";
	std::size_t begin = prag_str.find("[");
	std::size_t end = prag_str.find("]");
	std::string assumption_string = prag_str.substr(begin,end-begin+1);
	assumption_string.erase(std::remove(assumption_string.begin(),assumption_string.end(),' '),assumption_string.end());
	int begin_assumption = 1;
	int curr_pos = 1;
	std::vector<std::string> assumptions; 
	while (assumption_string[curr_pos] != ']') {
		if (assumption_string[curr_pos] == ',') {
			std::string assumption = assumption_string.substr(begin_assumption,curr_pos-begin_assumption);
			assumptions.push_back(assumption);
			curr_pos++;
			begin_assumption=curr_pos;
		}
		else {
			curr_pos++;
		}
	}
	std::string assumption = assumption_string.substr(begin_assumption,curr_pos-begin_assumption);
	assumptions.push_back(assumption);
	//for (std::vector<std::string>::iterator i = assumptions.begin(); i != assumptions.end(); i++) {
//		std::cout << *i << std::endl;
//	}
	return assumptions;
}						

std::string get_position(std::string prag_str) {
	std::string position;
        if (prag_str.find(" begin ") != std::string::npos) {
                position = "begin";
        }
        else if (prag_str.find(" end") != std::string::npos) {
                position = "end";
        }
        else if (prag_str.find(" define ") != std::string::npos) {
                position = "define";
        }
	else if (prag_str.find(" assume ") != std::string::npos) {
		position = "assume";
	}
	else if (prag_str == "") {
		std::cout << "empty prag_str" << std::endl;
        	ROSE_ASSERT(false);
	}
	else {
		std::cout << "unknown prag_str!" << std::endl;
		std::cout << prag_str << std::endl;
		ROSE_ASSERT(false);
	}
        return position; 
}

//void remove_white_space(std::string& val) {
//	val.erase(std::remove(val.begin(),val.end(),' '),val.end());
//}

std::pair<std::string,std::string> get_type_and_name(std::string val) {
	int begin = 0;
	int end = 0;
	while (val[end] != ':') {
		end++;
	}
	//std::cout << "begin: " << begin << std::endl;
	//std::cout << "end: " << end << std::endl;
	//std::cout << "end-begin " << end-begin << std::endl;
	std::string type = val.substr(begin,end-begin);
	
	end++;
	begin = end;
	while (val[end]) {
		end++;
	}
	
	//val.substr(begin,end-begin);
	std::pair<std::string,std::string> type_and_name;
	type_and_name.first = type;
	//std::cout << "begin: " << begin << std::endl;
	//std::cout << "end: " << end << std::endl;
	//std::cout << "end-begin: " << end-begin << std::endl;
	std::string name = val.substr(begin,end-begin);
	type_and_name.second = name;
	return type_and_name;
}
std::vector<std::pair<std::string,std::string> > get_vars(std::string pstring) {
        std::vector<std::pair<std::string,std::string> > defined_vars;
	std::size_t begin_parse = pstring.find("define");
        std::size_t begin_defined = pstring.find("[");
	std::size_t end_defined = pstring.find("]");
		
	std::string white_space_string = pstring.substr(begin_defined,end_defined-begin_defined+1);
        //std::cout << white_space_string << " string\n";
	//std::string no_white_space_string = remove_white_space(white_space_string);
        //
      white_space_string.erase(std::remove(white_space_string.begin(),white_space_string.end(),' '),white_space_string.end());
	//std::cout << "white_space_string: " << white_space_string << std::endl;
//std::cout << no_white_space_string << " nostring\n";
	int begin_int = 1;
	int curr_int = 1;
        ROSE_ASSERT(white_space_string[0] == '[');
	std::vector<std::string> items;
	while (true) {
		if (white_space_string[curr_int] == ']') {
			break;
		}
		else if (white_space_string[curr_int] == ',') {
			int end = curr_int;
			std::string curr_item = white_space_string.substr(begin_int,end-begin_int);
			//std::cout << "curr item: " << curr_item << std::endl;
			curr_int++;	
			begin_int = curr_int;
			items.push_back(curr_item);
		}
		else {
			curr_int++;
		}
	}
	int end = curr_int;
	std::string curr_item = white_space_string.substr(begin_int,end-begin_int);
	items.push_back(curr_item);
	#ifdef DEBUG_OUTPUT
	std::cout << "curr item: " << curr_item << std::endl;
	std::cout << "done with while loop" << std::endl;
	#endif
	//std::vector<std::pair<std::string,std::string> > defined_vars
	for (int i = 0; i < items.size(); i++) {	
		std::string curr_item = items[i];
		std::size_t at_pos = curr_item.find(":");
		std::string type = curr_item.substr(0,at_pos);
		std::string name = curr_item.substr(at_pos+1,curr_item.length()-at_pos);
		#ifdef DEBUG_OUTPUT
		std::cout << "type: " << type << std::endl;
		std::cout << "name: " << name << std::endl;
		#endif
		std::pair<std::string,std::string> type_name;
		type_name.first = type;
		type_name.second = name;
		defined_vars.push_back(type_name);
	}
	/*	
		
		
        while (white_space_string[curr_int] != ']') {
		if (white_space_string[curr_int] != ',' && white_space_string[curr_int] != ']') {
                        curr_int++;
                }
                else {
			int len = curr_int - begin_int;
                        std::string tmp_name = white_space_string.substr( begin_int, len);
			std::pair<std::string,std::string> type_and_name = get_type_and_name(tmp_name);
                        defined_vars.push_back(type_and_name);
			//var_names.push_back(type_and_name.second);
			std::cout << ";; pragma defined var" << std::endl;
			std::cout << ";; name: " << type_and_name.second << std::endl;
			//var_types.push_back(type_and_name.first);
			std::cout << ";; type: " << type_and_name.first << std::endl;
                        if (white_space_string[curr_int] == ']') {
				break;
			}
			curr_int++;
			begin_int = curr_int;
			
                }
        }
	*/	
        //std::string last_name = white_space_string.substr(begin_int,curr_int-begin_int);
        //std::pair<std::string,std::string> last_name_type = get_type_and_name(last_name);
	//defined_vars.push_back(last_name_type); 
//	var_names.push_back(last_name_type.second);
        return defined_vars;
}
