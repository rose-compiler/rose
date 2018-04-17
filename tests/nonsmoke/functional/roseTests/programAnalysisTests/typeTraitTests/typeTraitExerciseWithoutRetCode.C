#include "rose.h"

// #include <iostream>
// #include <sstream>
// #include <functional>
// #include <numeric>
// #include <fstream>
// #include <map>
// #include <boost/tokenizer.hpp>
// #include <boost/algorithm/string.hpp>
// #include <boost/foreach.hpp>

using namespace std;

// using namespace boost::algorithm;
// using namespace SageInterface;
// #define foreach BOOST_FOREACH

#define DEBUG_QUADRATIC_BEHAVIOR 0

size_t counter = 0;

class TypeTraitChecker : public AstSimpleProcessing {
private:
    typedef bool (*TypeTraitFunctionP)(const SgType * const) ;
    map<string, TypeTraitFunctionP> nameToFpMap;
    void CallTypeTraitFunction(SgTypeTraitBuiltinOperator * func) {
        
#if DEBUG_QUADRATIC_BEHAVIOR
      printf ("At end of CallTypeTraitFunction: counter = %zu \n",counter);
#endif

        // Print info about the callsite so that we can debug in case of failures.
        string str = func->get_name().getString();
        cout<<"\n -----Location:"<<func->get_file_info()->get_filenameString () << " :Line: " << func->get_file_info()->get_line () <<" :Col: "<< func->get_file_info()->get_col();
#if DEBUG_QUADRATIC_BEHAVIOR
        printf ("In CallTypeTraitFunction: after first cout \n");
#endif
        cout<<"\n -----Calling the type trait:"<<str<<":";
#if DEBUG_QUADRATIC_BEHAVIOR
        printf ("In CallTypeTraitFunction: after second cout \n");
#endif
        
        // skip if it is a SgTemplateType
        // This allows us to process files with templates
        SgType *t1 = isSgType(func->get_args()[0]);
        SgType *t = t1->stripType(SgType::STRIP_TYPEDEF_TYPE);
        
        if (isSgTemplateType(t)){
            cout<<"skip";
#if DEBUG_QUADRATIC_BEHAVIOR
            printf ("In CallTypeTraitFunction: output SKIP \n");
#endif
            return;
        }
        
        map<string, TypeTraitFunctionP> :: iterator it = nameToFpMap.find(str);
        if(it != nameToFpMap.end()){
            // Found a type trait that we understand, let us run analysis on it.
            ROSE_ASSERT(isSgType(func->get_args()[0]));
            bool val = it->second(isSgType(func->get_args()[0]));
            cout<<val;
#if DEBUG_QUADRATIC_BEHAVIOR
            printf ("In CallTypeTraitFunction: output boolean value \n");
#endif
            return;
        }
        
        if (str == "__is_base_of"){
            // __is_base_of is special since it has a different signature.
            ROSE_ASSERT(isSgType(func->get_args()[0]));
            ROSE_ASSERT(isSgType(func->get_args()[1]));
            bool val = SageInterface::IsBaseOf(isSgType(func->get_args()[0]),isSgType(func->get_args()[1]));
            cout<<val;
        }

#if DEBUG_QUADRATIC_BEHAVIOR
       printf ("In CallTypeTraitFunction: at end of function \n");
#endif
    }
    
    
public:
    TypeTraitChecker(){
        // Populate function pointers.
        nameToFpMap["__has_nothrow_assign"]      = SageInterface::HasNoThrowAssign;
        nameToFpMap["__has_nothrow_copy"]        = SageInterface::HasNoThrowCopy;
        nameToFpMap["__has_nothrow_constructor"] = SageInterface::HasNoThrowConstructor;
        nameToFpMap["__has_trivial_assign"]      = SageInterface::HasTrivialAssign;
        nameToFpMap["__has_trivial_copy"]        = SageInterface::HasTrivialCopy;
        nameToFpMap["__has_trivial_constructor"] = SageInterface::HasTrivialConstructor;
        nameToFpMap["__has_trivial_destructor"]  = SageInterface::HasTrivialDestructor;
        nameToFpMap["__has_virtual_destructor"]  = SageInterface::HasVirtualDestructor;
        nameToFpMap["__is_abstract"]             = SageInterface::IsAbstract;
        nameToFpMap["__is_class"]                = SageInterface::IsClass;
        nameToFpMap["__is_empty"]                = SageInterface::IsEmpty;
        nameToFpMap["__is_enum"]                 = SageInterface::IsEnum;
        nameToFpMap["__is_literal_type"]         = SageInterface::IsLiteralType;
        nameToFpMap["__is_pod"]                  = SageInterface::IsPod;
        nameToFpMap["__is_polymorphic"]          = SageInterface::IsPolymorphic;
        nameToFpMap["__is_standard_layout"]      = SageInterface::IsStandardLayout;
        nameToFpMap["__is_trivial"]              = SageInterface::IsTrivial;
        nameToFpMap["__is_union"]                = SageInterface::IsUnion;
        //        nameToFpMap["__is_base_of"] = IsBaseOf;
        //      SgType *  UnderlyingType(SgType *type);
    }
    
    void visit(SgNode * node) {

     // Added counter to debug performance issues in large files.
        counter++;

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
