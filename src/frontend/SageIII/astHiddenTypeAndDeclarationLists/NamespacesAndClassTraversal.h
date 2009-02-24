// Header-File for Datastructures needed to compute Hidden-Lists
// Robert Preissl
// Last modified : April 16, 2007

#ifndef DEBUG_NAMESPACESANDCLASSTRAVERSAL_H_
#define DEBUG_NAMESPACESANDCLASSTRAVERSAL_H_


 #include <iostream>
 #include <set>
 #include <algorithm>
 #include <string>
 #include <ext/hash_map>
 #include <vector>

 //#include "DEBUG_HiddenList_Output.h"
 //#include "DEBUG_HiddenList_Intersection.h"

// DQ (5/8/2007): We can't have these in header files
// using namespace std;
// using namespace __gnu_cxx;


//#define HIDDEN_LIST_DEBUG

#include <sstream>

namespace NamespacesAndClassSymbolCollection {


struct cmp_string {

        bool operator()(std::string s1, std::string s2) const {
                return (s1 == s2);
        }

};


struct equal_symbol {

        bool operator()(SgSymbol* S1, SgSymbol* S2) const {
                return (S1 == S2);
        }


};


struct eqstr3 {

        bool operator()(std::string s1, std::string s2) const {
                return (s1 == s2) ;
        }

};


// taken form sgi stl page
//The hash<T> template is only defined for template arguments of type char*, const char*, crope, wrope, and the built-in integral types.
struct HashFunction_String
{
        /*public:
                int operator()(std::string in) const {
                        int x = 0;
                        for(int i = 0; i < (int)in.size(); ++i) {
                                x += (int)in[0];
                        }

                        return x;
                }*/
        public:
                int operator()(std::string in) const
                {
                        return hash<char*>()((char*)in.c_str());
                }

};


struct SymbolInformation {

        SgSymbol* symbol_pointer;
        std::string name;
        bool scope_can_be_named;
        bool symbol_of_class; // symbols of classes are treated in a different way (in the intersection procedure the order ( if they are declared before or after the place where they are hidden) of such symbols is not considered)

};


// struct of booleans for getting class, namespace & validity information
struct SymbolHashMapValue {

        bool scope_can_be_named;
        bool symbol_of_class; // symbols of classes are treated in a different way (in the intersection procedure the order ( if they are declared before or after the place where they are hidden) of such symbols is not considered)
        bool is_already_proofed_to_be_valid; // Motivation for this, look at motivating example in the Intersection procedure

};


// for comparing SgSymbol*
struct cmp_SgSymbolPointer {

        bool operator() (SgSymbol* X, SgSymbol* Y) {

                // at first sort after names..
                if( X->get_name() != Y->get_name() ) {
                        return ( X->get_name() < Y->get_name() );
                }
                // if names are equal, sort after addresses
                else {
                        return ( ((long int)X) < ((long int)Y) );
                }
        }

};


// HashFunction for SymbolHashMap
struct HashFunction_SymbolHashMap {

        public:
                int operator()(SgSymbol* S) const
                {
                        // calculate hashfunction-value based on address of SgSymbol
                        return hash<int>()((long int)S);

                }

};



// this hash_map contains:
//  Key: pointer to SgSymbol
//  Value: struct of booleans for getting class, namespace & validity information
typedef hash_map<SgSymbol*, SymbolHashMapValue, HashFunction_SymbolHashMap /*equal_symbol*/> SymbolHashMap;

// this hash_map contains:
//  Key: symbol-name from the SymbolTable
//  Value: HashMap of Pointers & boolean if scope can be named of this symbols
typedef hash_map<std::string, SymbolHashMap, HashFunction_String, cmp_string> ScopeStackEntry;

// ad using directives: used for storing namespaces
//  Key: (qualified) Name of namespace
//  Value: std::vector of struct symbol table
typedef hash_map<std::string, std::vector<SymbolInformation>, HashFunction_String, eqstr3> StringVectorHashMap;





// object for Post-Order Traversal
class SynthesizedAttributeForNamespacesAndClassTraversal {

        public:

                SynthesizedAttributeForNamespacesAndClassTraversal() {};

};



// object for Pre-Order Traversal (TopDown)
//  -> will provide a stack (vector) of Scopes which will be (the LIFO one) intersected with the current symbol-table
class InheritedAttributeForNamespacesAndClassTraversal {

        public:

                int depth;

                // constructors
                InheritedAttributeForNamespacesAndClassTraversal(int d)  { depth = d; };

                InheritedAttributeForNamespacesAndClassTraversal(const InheritedAttributeForNamespacesAndClassTraversal &IA) : depth(IA.depth) {};

};



class NamespacesAndClassTraversal : public AstTopDownBottomUpProcessing<InheritedAttributeForNamespacesAndClassTraversal, SynthesizedAttributeForNamespacesAndClassTraversal> {

        public:

                NamespacesAndClassTraversal() {};


                // hash_map (Key: name / Value: vector of symbols) for storing namespaces (Feature: can be updated)
                //  will be used for updating the current scope if a using-directive or using-declaration is encountered
                StringVectorHashMap NamespacesHashMap;

                // hash_map (Key: name / Value: vector of symbols) for storing symbols of a class
                //  will be used for updating the current scope if an SgMemberFunctionDeclaration is encountered
                StringVectorHashMap ClassHashMap;

                virtual InheritedAttributeForNamespacesAndClassTraversal evaluateInheritedAttribute(SgNode* n, InheritedAttributeForNamespacesAndClassTraversal inheritedAttribute);
                virtual SynthesizedAttributeForNamespacesAndClassTraversal evaluateSynthesizedAttribute(SgNode* n, InheritedAttributeForNamespacesAndClassTraversal inheritedAttribute, SynthesizedAttributesList SynthesizedAttributeForNamespacesAndClassTraversalList);

                // for using directives
                void UpdateNamespace(std::string updated_namespace, std::string used_namespace);

                // for using declarations
                void UpdateNamespace(std::string updated_namespace, std::string used_namespace, std::string n);


};




// DQ (5/8/2007): Added this function to wrap up details of how to call the hidden list work
//void buildNamespacesAndClassHashMaps(SgProject* project );
void buildNamespacesAndClassHashMaps( SgProject* project, NamespacesAndClassTraversal& namespaces_and_class_traversal );

void StringVectorHashMapOutput(StringVectorHashMap &SVHM);



} // end of namespace: NamespacesAndClassSymbolCollection


#endif /*DEBUG_NAMESPACESANDCLASSTRAVERSAL_H_*/

