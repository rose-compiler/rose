#include "rose.h"
#include <iostream>
#include <sstream>
#include <functional>
#include <numeric>
#include <fstream>
#include <map>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
using namespace std;
using namespace boost::algorithm;
using namespace SageInterface;


#define foreach BOOST_FOREACH

class TypeTraitChecker : public AstSimpleProcessing {
private:
    typedef bool (*TypeTraitFunctionP)(const SgType * const) ;
    map<string, TypeTraitFunctionP> nameToFpMap;
    void CallTypeTraitFunction(SgTypeTraitBuiltinOperator * func) {
        
        // Print info about the callsite so that we can debug in case of failures.
        string str = func->get_name().getString();
        cout<<"\n -----Location:"<<func->get_file_info()->get_filenameString () << " :Line: " << func->get_file_info()->get_line () <<" :Col: "<< func->get_file_info()->get_col();
        cout<<"\n -----Calling the type trait:"<<str<<":";
        
        // skip if it is a SgTemplateType
        // This allows us to process files with templates
        SgType *t1 = isSgType(func->get_args()[0]);
        SgType *t = t1->stripType(SgType::STRIP_TYPEDEF_TYPE);
        
        if (isSgTemplateType(t)){
            cout<<"skip";
            return;
        }
        
        map<string, TypeTraitFunctionP> :: iterator it = nameToFpMap.find(str);
        if(it != nameToFpMap.end()){
            // Found a type trait that we understand, let us run analysis on it.
            ROSE_ASSERT(isSgType(func->get_args()[0]));
            bool val = it->second(isSgType(func->get_args()[0]));
            cout<<val;
            return;
        }
        
        if (str == "__is_base_of"){
            // __is_base_of is special since it has a different signature.
            ROSE_ASSERT(isSgType(func->get_args()[0]));
            ROSE_ASSERT(isSgType(func->get_args()[1]));
            bool val =IsBaseOf(isSgType(func->get_args()[0]),isSgType(func->get_args()[1]));
            cout<<val;
        }
    }
    
    
public:
    TypeTraitChecker(){
        // Populate function pointers.
        nameToFpMap["__has_nothrow_assign"] = HasNoThrowAssign;
        nameToFpMap["__has_nothrow_copy"] = HasNoThrowCopy;
        nameToFpMap["__has_nothrow_constructor"] = HasNoThrowConstructor;
        nameToFpMap["__has_trivial_assign"] = HasTrivialAssign;
        nameToFpMap["__has_trivial_copy"] = HasTrivialCopy;
        nameToFpMap["__has_trivial_constructor"] = HasTrivialConstructor;
        nameToFpMap["__has_trivial_destructor"] = HasTrivialDestructor;
        nameToFpMap["__has_virtual_destructor"] = HasVirtualDestructor;
        nameToFpMap["__is_abstract"] = IsAbstract;
        nameToFpMap["__is_class"] = IsClass;
        nameToFpMap["__is_empty"] = IsEmpty;
        nameToFpMap["__is_enum"] = IsEnum;
        nameToFpMap["__is_literal_type"] = IsLiteralType;
        nameToFpMap["__is_pod"] = IsPod;
        nameToFpMap["__is_polymorphic"] = IsPolymorphic;
        nameToFpMap["__is_standard_layout"] = IsStandardLayout;
        nameToFpMap["__is_trivial"] = IsTrivial;
        nameToFpMap["__is_union"] = IsUnion;
        //        nameToFpMap["__is_base_of"] = IsBaseOf;
        //      SgType *  UnderlyingType(SgType *type);
        
    }
    
    void visit(SgNode * node) {
        if(SgTypeTraitBuiltinOperator * expr = isSgTypeTraitBuiltinOperator(node)){
            CallTypeTraitFunction(expr);
        }
    }
};



//SageInterface::makeSingleStatementBodyToBlock

int main( int argc, char * argv[] ) {
    // Generate the ROSE AST.
    SgProject* project = frontend(argc,argv);
    
    TypeTraitChecker typeTraitChecker;
    typeTraitChecker.traverse(project, preorder);
    
    // don't do back end.
    return 0;
}
