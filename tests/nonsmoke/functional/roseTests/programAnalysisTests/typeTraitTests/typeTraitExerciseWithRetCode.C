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
    bool expectedValue;
    typedef bool (*TypeTraitFunctionP)(const SgType * const) ;
    map<string, TypeTraitFunctionP> nameToFpMap;
    
    
    void CallTypeTraitFunction(SgTypeTraitBuiltinOperator * func) {
        string str = func->get_name().getString();
        cout<<"\n calling:"<<str<<":";
        map<string, TypeTraitFunctionP> :: iterator it = nameToFpMap.find(str);
        
        if(it != nameToFpMap.end()){
            
            // found a valid type trait that we can call
            ROSE_ASSERT(isSgType(func->get_args()[0]));
            bool val = it->second(isSgType(func->get_args()[0]));
            cout<<val;
            ROSE_ASSERT(val == expectedValue);
            
            // check that the analysis returned what the test expected.
            if(val != expectedValue)
                exit(-1);
            return;
        }
        
        if (str == "__is_base_of"){
            // __is_base_of is special since it has a different signature.
            ROSE_ASSERT(isSgType(func->get_args()[0]));
            ROSE_ASSERT(isSgType(func->get_args()[1]));
            bool val =IsBaseOf(isSgType(func->get_args()[0]),isSgType(func->get_args()[1]));
            cout<<val;
            ROSE_ASSERT(val == expectedValue);
            if(val != expectedValue)
                exit(-1);
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
            // if it is a SgTypeTraitBuiltinOperator, call our analysis passing it the type
            CallTypeTraitFunction(expr);
        } else if (SgIfStmt * ifStmt = isSgIfStmt(node)) {
            ROSE_ASSERT(ifStmt->get_conditional());
            ROSE_ASSERT(isSgExprStatement(ifStmt->get_conditional()));
            ROSE_ASSERT(isSgExprStatement(ifStmt->get_conditional())->get_expression ());
            //cout<<isSgExprStatement(ifStmt->get_conditional())->get_expression ()->class_name();
            
            // if the condition in the If statement has a NOT operator expect false as the return from
            // calling SgTypeTraitBuiltinOperator. Else expect true.
            // CAUTION: Tests need to adhere to this convention.
            
            if(isSgNotOp(isSgExprStatement(ifStmt->get_conditional())->get_expression ()))
                this->expectedValue = false;
            else
                this->expectedValue = true;
        }
    }
};



//SageInterface::makeSingleStatementBodyToBlock

int main( int argc, char * argv[] ) {
    // Generate the ROSE AST.
    SgProject* project = frontend(argc,argv);
    TypeTraitChecker typeTraitChecker;
    typeTraitChecker.traverse(project, preorder);
    // don't do backend compile since this is only an analysis test
    return 0;
}
