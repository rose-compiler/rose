#include "rose.h"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>

using namespace std;
using namespace SageBuilder;
using namespace SageInterface;

// a pair of names: class name and function name.
typedef vector<pair<string, string> > class_function_pairs;

/* 
struct MemberFunctionInfo
{
    string class_name;
    string func_name;
    vector<SgType*> para_types;
};
*/

// This function detects macro calls which contains event function 
// and name of the class it belongs to as parameters, and save those names.
class_function_pairs event_detect()
{
    class_function_pairs class_func;

    typedef std::map<std::string, std::vector<PreprocessingInfo*>* > attached_attr_type;
    typedef  std::vector<PreprocessingInfo*> rose_attr_list_type;

    for( std::map<std::string,ROSEAttributesList* > ::iterator  it_files = mapFilenameToAttributes.begin();
	    it_files != mapFilenameToAttributes.end();
	    ++it_files)
    {
	std::string filename = it_files->first;

	for (rose_attr_list_type::iterator it_1 =
		it_files->second->getList().begin(); 
		it_1 != it_files->second->getList().end();
		++it_1)
	{
	    PreprocessingInfo *currentInfo = *it_1;
	    ROSE_ASSERT(currentInfo != NULL);
	    PreprocessingInfo::DirectiveType typeOfDirective =
		currentInfo->getTypeOfDirective (); 

	    std::string macroDefFilename = currentInfo->getString();
	    macroDefFilename = StringUtility::stripPathFromFileName(macroDefFilename);

	    if (typeOfDirective == PreprocessingInfo::CMacroCall)
	    {
		PreprocessingInfo::rose_macro_call* macro =  currentInfo->get_macro_call();
		if (macro->is_functionlike)
		{
		    if (macro->macro_call.get_value() == "DEFINE_SIMOBJ_EVENT_0_ARG" ||
			    macro->macro_call.get_value() == "DEFINE_SIMOBJ_EVENT_1_ARG" ||
			    macro->macro_call.get_value() == "DEFINE_SIMOBJ_EVENT_2_ARG" ||
			    macro->macro_call.get_value() == "DEFINE_SIMOBJ_EVENT_3_ARG" ||
			    macro->macro_call.get_value() == "DEFINE_SIMOBJ_EVENT_4_ARG" ||
			    macro->macro_call.get_value() == "DEFINE_SIMOBJ_EVENT_5_ARG")
		    {
			string class_name, func_name;
			//int args_num = macro->arguments.size() - 3;
			for(size_t i = 1; i < macro->arguments.size(); ++i)
			{
			    for(token_list_container::iterator tk = macro->arguments[i].begin();
				    tk != macro->arguments[i].end(); ++tk)
			    {
				string str = tk->get_value().c_str();
				boost::trim(str);
				if (str == "") continue;
				if (i == 1) class_name = str;
				if (i == 2) func_name = str;
			    }
			}
			if(class_name != "" && func_name != "")
			{
			    class_func.push_back(make_pair(class_name, func_name));
			}
		    }	
		}
	    }
	}
    }
    return class_func;
}


class visitorTraversal : public AstSimpleProcessing
{
    public:
	visitorTraversal(const class_function_pairs cf) : class_func(cf) {}
	virtual void visit(SgNode* n); 

    private:
	SgFunctionRefExp* trackVarRefToFindFunction(SgVarRefExp* ref);
	class_function_pairs class_func;
};

void visitorTraversal::visit(SgNode* n)
{
    // for a member function, get its name and the name of the class
    if(SgMemberFunctionDeclaration* memfunc_decl = isSgMemberFunctionDeclaration(n))
    {
	string func_name = memfunc_decl->get_name().getString();
	if(SgClassDefinition* class_def = memfunc_decl->get_class_scope())
	{
	    string class_name = class_def->get_declaration()->get_name().getString();
	    for(size_t i = 0; i < class_func.size(); ++i)
	    {
		// check if the class name and function name are both the same to names detected
		if(class_func[i].first == class_name && 
			class_func[i].second == func_name)
		    cout << class_name << "::" << func_name << endl;
	    }
	}
    }
}

// Typical main function for ROSE translator
int main( int argc, char * argv[] )
{
    // Build the AST used by ROSE
    std::vector<std::string> newArgv(argv,argv+argc);
    newArgv.push_back("-rose:wave");

    SgProject* project = frontend(newArgv);

    // Build the traversal object
    visitorTraversal event_detector(event_detect());

    // Call the traversal starting at the project node of the AST
    event_detector.traverseInputFiles(project,preorder);

    return backend(project);
}

