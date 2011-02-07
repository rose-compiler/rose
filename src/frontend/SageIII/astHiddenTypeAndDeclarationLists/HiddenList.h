// Header-File for Datastructures needed to compute Hidden-Lists
// Robert Preissl
// Last modified : April 16, 2007

#ifndef DEBUG_HIDDENLIST_H_
#define DEBUG_HIDDENLIST_H_


 #include <iostream>
 #include <set>
 #include <algorithm>
 #include <string>
 #include <vector>
 #include <deque>

 //#include "DEBUG_HiddenList_Output.h"
 //#include "DEBUG_HiddenList_Intersection.h"

// DQ (5/8/2007): We can't have these in header files
// using namespace std;
// using namespace __gnu_cxx;


// #define HIDDEN_LIST_DEBUG

#include <sstream>

namespace Hidden_List_Computation {


struct cmp_string {

        bool operator()(const std::string & s1, const std::string & s2) const {

                return (s1 == s2);

        }

};


struct equal_symbol {

        bool operator()(SgSymbol* S1, SgSymbol* S2) const {

                return (S1 == S2);

        }


};

// CH (4/13/2010): Use boost::hash<string> instead
#if 0
struct eqstr3 {

        bool operator()(const std::string & s1, const std::string & s2) const {

                return (s1 == s2) ;

        }

};
#endif


// taken form sgi stl page
//The hash<T> template is only defined for template arguments of type char*, const char*, crope, wrope, and the built-in integral types.
#if 0
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
// CH (4/8/2010): Use boost::unordered instead      
//#if _MSC_VER
#if 0       
                                           public:
      static const size_t bucket_size = 4;
      static const size_t min_buckets = 8;
#endif
                int operator()(const std::string & in) const
                {
// CH (4/8/2010): Use boost::hash instead                   
//#if _MSC_VER
#if 0
                        return stdext::hash_compare<char*>()((char*)in.c_str());
#else
                        return rose_hash::hash<char*>()((char*)in.c_str());
#endif
                }

};
#endif


struct SymbolInformation {

        SgSymbol* symbol_pointer;
        std::string name;
        bool scope_can_be_named;
        bool symbol_of_class; // symbols of classes are treated in a different way (in the intersection procedure the order ( if they are declared before or after the place where they are hidden) of such symbols is not considered)

        // Robert Preissl, June 21 2007 : ad Using-Dir./Decl. store a flag which indicates that this symbol comes from a using dir./decl. or not
        bool symbol_is_from_using;
        int depth_of_using;
        SgUsingDirectiveStatement* si_using_dir;
        SgUsingDeclarationStatement* si_using_decl;

        // Robert Preissl, July 5 2007 : ad using decl. in classes : for a motivating example see intersection procedure!
        bool is_using_decl_in_class;
        SgUsingDeclarationStatement* si_using_decl_in_class;

        // constructor:
        SymbolInformation() {}

        SymbolInformation(SgSymbol* sp, std::string str, bool& b_name, bool& b_class, bool b_using, int depth, SgUsingDirectiveStatement* using_dir, SgUsingDeclarationStatement* using_decl, bool b_using_in_class, SgUsingDeclarationStatement* using_decl_in_class) :
                symbol_pointer(sp),
                name(str),
                scope_can_be_named(b_name),
                symbol_of_class(b_class),
                symbol_is_from_using(b_using),
                depth_of_using(depth),
                si_using_dir(using_dir),
                si_using_decl(using_decl),
                is_using_decl_in_class(b_using_in_class),
                si_using_decl_in_class(using_decl_in_class)
                {}


};

// struct of booleans for getting class, namespace & validity information
struct SymbolHashMapValue {

        bool scope_can_be_named;
        bool symbol_of_class; // symbols of classes are treated in a different way (in the intersection procedure the order ( if they are declared before or after the place where they are hidden) of such symbols is not considered)
        bool is_already_proofed_to_be_valid; // Motivation for this, look at motivating example in the Intersection procedure

        // Robert Preissl, June 21 2007 : ad Using-Dir./Decl. store a flag which indicates that this symbol comes from a using dir./decl. or not
        bool symbol_is_from_using;
        SgUsingDirectiveStatement* si_using_dir;
        SgUsingDeclarationStatement* si_using_decl;
        int depth_of_using;

        // Robert Preissl, July 5 2007 : ad using decl. in classes : for a motivating example see intersection procedure!
        bool is_using_decl_in_class;
        SgUsingDeclarationStatement* si_using_decl_in_class;

        SymbolHashMapValue() {}

        SymbolHashMapValue(bool b_name, bool b_class, bool b_valid, bool b_using, SgUsingDirectiveStatement* using_dir, SgUsingDeclarationStatement* using_decl, int depth, bool b_using_in_class, SgUsingDeclarationStatement* using_decl_in_class ) :
                scope_can_be_named(b_name),
                symbol_of_class(b_class),
                is_already_proofed_to_be_valid(b_valid),
                symbol_is_from_using(b_using),
                si_using_dir(using_dir),
                si_using_decl(using_decl),
                depth_of_using(depth),
                is_using_decl_in_class(b_using_in_class),
                si_using_decl_in_class(using_decl_in_class)
                {}

};


extern double accumulatedEqstr_SgDeclarationStatementTime;
extern double accumulatedEqstr_SgDeclarationStatementCalls;

#if 0
// DQ (9/25/2007): Jeremiah and I removed this function so that a default operator will be used (a pointer based operator<())

// compare the mangled names of SgDeclarationStatements (I don't care in which order they are!)
struct eqstr_SgDeclarationStatement {

        // TODO: will be removed because of performance bottleneck

        bool operator() (SgDeclarationStatement* X, SgDeclarationStatement* Y) {

                bool result;

             // DQ (8/3/2007): Start time for this function
                TimingPerformance::time_type startTime;
                TimingPerformance::startTimer(startTime);

                std::string mangled_name_X = X->get_mangled_name().str();
                std::string mangled_name_Y = Y->get_mangled_name().str();

                if(mangled_name_X != mangled_name_Y) {

                        result = (mangled_name_X < mangled_name_Y);

                }
                else {

                        // for comparing the addresses of SgDeclarationStatements (Problem with functions!!)
                        long int i1 = (long int)X;
                        long int i2 = (long int)Y;

                        result = (i1 < i2);

                }

       // DQ (8/3/2007): accumulate the elapsed time for this function
          TimingPerformance::accumulateTime ( startTime, accumulatedEqstr_SgDeclarationStatementTime, accumulatedEqstr_SgDeclarationStatementCalls );

          return result;
        }

};
#endif


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
// CH (4/8/2010): Use boost::unordered instead      
//#if _MSC_VER
#if 0
          static const size_t bucket_size = 4;
          static const size_t min_buckets = 8;
#endif

                int operator()(SgSymbol* S) const
                {
                        // calculate hashfunction-value based on address of SgSymbol
// CH (4/8/2010): Use boost::hash instead           
//#if _MSC_VER
#if 0
                        return stdext::hash_compare<int>()((int)S);
#else
                        return rose_hash::hash<int>()((long int)S);
#endif
                                }

};


struct UsingDirectiveWithScope {

        SgUsingDirectiveStatement* using_dir;
        SgScopeStatement* scope;

        // Robert Preissl, June 26 2007 : add a bool-variable, which stores if this SgUsingDirectiveStatement has already been encountered or not
        bool is_already_proofed_to_be_valid;

};


struct cmp_UsingDirectiveWithScope {

            // CH (4/7/2010): This function should be const.
        bool operator() (const UsingDirectiveWithScope & X, const UsingDirectiveWithScope & Y) const {

                // at first sort after scope
                if( X.scope != Y.scope ) {

                        return (((long int)X.scope) < ((long int)Y.scope));

                }
                // if scope is equal then sort after addresses of UsingDirective
                else {

                        return (((long int)X.using_dir) < ((long int)Y.using_dir));

                }

        }

};


struct UsingDeclarationWithScope {

        SgUsingDeclarationStatement* using_decl;
        SgScopeStatement* scope;

        // Robert Preissl, June 26 2007 : add a bool-variable, which stores if this SgUsingDirectiveStatement has already been encountered or not
        bool is_already_proofed_to_be_valid;

};


struct cmp_UsingDeclarationWithScope {

            // CH (4/7/2010): This function should be const.
        bool operator() (const UsingDeclarationWithScope& X, const UsingDeclarationWithScope& Y) const {

                // at first sort after scope
                if( X.scope != Y.scope ) {

                        return (((long int)X.scope) < ((long int)Y.scope));

                }
                // if scope is equal then sort after addresses of UsingDirective
                else {

                        return (((long int)X.using_decl) < ((long int)Y.using_decl));

                }

        }

};


// typedef std::vector<SymbolInformation> Vector_Of_SymbolInformation;
typedef std::deque<SymbolInformation*> Vector_Of_SymbolInformation;


// this hash_map contains:
//  Key: pointer to SgSymbol
//  Value: struct of booleans for getting class, namespace & validity information
//  
// CH (4/8/2010): Use boost::unordered instead      
//#ifdef _MSC_VER
#if 0
typedef rose_hash::unordered_map<SgSymbol*, SymbolHashMapValue*> SymbolHashMap;
#else
typedef rose_hash::unordered_map<SgSymbol*, SymbolHashMapValue*, HashFunction_SymbolHashMap /*equal_symbol*/> SymbolHashMap;
#endif
// this hash_map contains:
//  Key: symbol-name from the SymbolTable
//  Value: HashMap of Pointers & boolean if scope can be named of this symbols
//
// CH (4/8/2010): Use boost::unordered instead      
//#ifdef _MSC_VER
#if 0
// typedef rose_hash::unordered_map<std::string, SymbolHashMap, HashFunction_String> ScopeStackEntry;
typedef rose_hash::unordered_map<std::string, SymbolHashMap> ScopeStackEntry;
#else
// CH (4/13/2010): Use boost::hash<string>
//typedef rose_hash::unordered_map<std::string, SymbolHashMap, HashFunction_String, cmp_string> ScopeStackEntry;
typedef rose_hash::unordered_map<std::string, SymbolHashMap> ScopeStackEntry;
#endif

// ad using directives: used for storing namespaces
//  Key: (qualified) Name of namespace
//  Value: std::vector of struct symbol table
//
// CH (4/8/2010): Use boost::unordered instead      
//#ifdef _MSC_VER
#if 0
// typedef rose_hash::unordered_map<std::string, Vector_Of_SymbolInformation, HashFunction_String> StringVectorHashMap;
typedef rose_hash::unordered_map<std::string, Vector_Of_SymbolInformation> StringVectorHashMap;
#else
// CH (4/13/2010): Use boost::hash<string>
//typedef rose_hash::unordered_map<std::string, Vector_Of_SymbolInformation, HashFunction_String, eqstr3> StringVectorHashMap;
typedef rose_hash::unordered_map<std::string, Vector_Of_SymbolInformation> StringVectorHashMap;
#endif

// calculating the valid scope; make only an intersection of the entries of StringVectorHashMap(VScopeStack) that match with ValidScope
// typedef std::set<SgDeclarationStatement*, eqstr_SgDeclarationStatement> SetSgDeclarationStatements;
typedef std::set<SgDeclarationStatement*> SetSgDeclarationStatements;
// typedef std::vector<SgDeclarationStatement*> SetSgDeclarationStatements;


// ad dividing the hiddenlist into three different lists: for more information see the description in evaluateInheritedAttribute function in the ifSgScopestatement part!
typedef std::set<SgSymbol*> SetSgSymbolPointers;
//typedef std::set<SgSymbol*, cmp_SgSymbolPointer> SetSgSymbolPointers;

// CH (4/7/2010): After adding 'const' to the conparing functor's function, MSVC can compile the following code
//#ifndef _MSC_VER
//typedef std::set<UsingDirectiveWithScope> SetSgUsingDirectiveStatementsWithSgScopeStatement;
//typedef std::set<UsingDeclarationWithScope> SetSgUsingDeclarationWithScopeWithSgScopeStatement;
//#else
typedef std::set<UsingDirectiveWithScope, cmp_UsingDirectiveWithScope> SetSgUsingDirectiveStatementsWithSgScopeStatement;
typedef std::set<UsingDeclarationWithScope, cmp_UsingDeclarationWithScope> SetSgUsingDeclarationWithScopeWithSgScopeStatement;
//#endif



// Robert Preissl, June 15 2007: new data structure for the storage of namespace-information:
//  can't use the "StringVectorHashMap" anymore (Don't worry, for classes it's ok!) because:
// Motivating example:
//        namespace Y {
//                class X;
//                int foo();
//        }
//
//        namespace Y {
//                void X();
//                double d;
//        }
//
// -> in this case function X hides the Class X!
// BUT: Remember young man, the first traversal collects ALL namespace (& class) data, so Namespace[Y] = {X, foo, X, d},
//`        what will be used to update the second namespace Y for a hiddenlist computation!
struct NamespaceInformation {

        Vector_Of_SymbolInformation VectorOfSymbolInformation; // for each namespace with the same name make an entry (take a look at example above!)
        SgNamespaceDefinitionStatement* namespace_definition_stmt;

};

typedef std::vector<NamespaceInformation> VectorOfNamespaceInformation;
// CH (4/8/2010): Use boost::unordered instead      
//#ifdef _MSC_VER
#if 0
// typedef rose_hash::unordered_map<std::string, VectorOfNamespaceInformation, HashFunction_String> String_VectorOfNamespaceInformation_HashMap;
typedef rose_hash::unordered_map<std::string, VectorOfNamespaceInformation> String_VectorOfNamespaceInformation_HashMap;
#else
// CH (4/13/2010): Use boost::hash<string>
//typedef rose_hash::unordered_map<std::string, VectorOfNamespaceInformation, HashFunction_String, eqstr3> String_VectorOfNamespaceInformation_HashMap;
typedef rose_hash::unordered_map<std::string, VectorOfNamespaceInformation> String_VectorOfNamespaceInformation_HashMap;
#endif

typedef std::vector<NamespaceInformation>::iterator it_VectorOfNamespaceInformation;
struct it_VectorOfNamespaceInformation_boolean {
        std::vector<NamespaceInformation>::iterator it_vec_namesp_info;
        bool first_namespace_occurence;
};

// CH (4/8/2010): Use boost::unordered instead      
//#ifdef _MSC_VER
#if 0
// DQ (11/27/2009): Unclear now to fix this.
// typedef rose_hash::unordered_map<std::string, it_VectorOfNamespaceInformation_boolean, HashFunction_String> String_it_VectorOfNamespaceInformation_boolean;
typedef rose_hash::unordered_map<std::string, it_VectorOfNamespaceInformation_boolean> String_it_VectorOfNamespaceInformation_boolean;
#else
// CH (4/13/2010): Use boost::hash<string>
//typedef rose_hash::unordered_map<std::string, it_VectorOfNamespaceInformation_boolean, HashFunction_String, eqstr3> String_it_VectorOfNamespaceInformation_boolean;
typedef rose_hash::unordered_map<std::string, it_VectorOfNamespaceInformation_boolean> String_it_VectorOfNamespaceInformation_boolean;
#endif

// Robert Preissl, June 20 2007: in addition to the SetSgUsingDirectiveStatementsWithSgScopeStatement (and also for using decl.) we keep a special data structure that keeps
//  track of where using directives (and decl.) occur in the source-code relative to other declarations.
// Motivating example:
//        namespace X {int a;}
//        void foo() {float a;}
//        using namespace X;
//
// -> due to query (is there a using dir./decl. in my scope) we make an update to the scope before the hiddenlist-computation starts,
//        so float a thinks that it hides X::int a, what is wrong.
// -> BUT the following data structure knows that the using occurs in this particular case after float a is declared, and as a result
//        X::int a is not in the hiddenlist!
typedef std::vector<SetSgDeclarationStatements> StackSetSgDeclarationStatements;

struct LinkedListStackSetSgDeclarationStatements {

        // points to the previous Stack (needed, because there will be several using dir./decl. and I don't want to store a whole stack of declarations
        //  for each using dir./decl.)
        //LinkedListStackSetSgDeclarationStatements* PreviousLinkedListStackSetSgDeclarationStatements;

        // will be a UsingDecl. or a UsingDir.
        SgDeclarationStatement* PreviousLinkedListStackSetSgDeclarationStatements;

        StackSetSgDeclarationStatements CurrentStackSetSgDeclarationStatements;

};

// HashFunction for UsingDirectiveStatement_LinkedListStackSetSgDeclarationStatements_HashMap
struct HashFunction_SgUsingDirectiveStatement {

        public:
// CH (4/8/2010): Use boost::unordered instead      
//                      #if _MSC_VER
#if 0                   
                                           public:
      static const size_t bucket_size = 4;
      static const size_t min_buckets = 8;
#endif

                int operator()(SgUsingDirectiveStatement* using_dir) const
                {
                        // calculate hashfunction-value based on address of SgUsingDirectiveStatement
// CH (4/8/2010): Use boost::hash instead           
//#if _MSC_VER
#if 0
                        return stdext::hash_compare<int>()((int)using_dir);
#else
                        return rose_hash::hash<int>()((long int)using_dir);
#endif
                                }

};

//  Key: Address of SgUsingDirectiveStatement
//  Value: LinkedListStackSetSgDeclarationStatements
//
// CH (4/8/2010): Use boost::unordered instead      
//#ifdef _MSC_VER
#if 0
typedef rose_hash::unordered_map<SgUsingDirectiveStatement*, LinkedListStackSetSgDeclarationStatements> UsingDirectiveStatement_LinkedListStackSetSgDeclarationStatements_HashMap;
#else
typedef rose_hash::unordered_map<SgUsingDirectiveStatement*, LinkedListStackSetSgDeclarationStatements, HashFunction_SgUsingDirectiveStatement> UsingDirectiveStatement_LinkedListStackSetSgDeclarationStatements_HashMap;
#endif

// HashFunction for UsingDeclarationStatement_LinkedListStackSetSgDeclarationStatements_HashMap
struct HashFunction_SgUsingDeclarationStatement {

        public:
// CH (4/8/2010): Use boost::unordered instead      
//#if _MSC_VER
#if 0
                                           public:
      static const size_t bucket_size = 4;
      static const size_t min_buckets = 8;
#endif
                int operator()(SgUsingDeclarationStatement* using_decl) const
                {
                        // calculate hashfunction-value based on address of SgUsingDeclarationStatement
// CH (4/8/2010): Use boost::hash instead           
//#if _MSC_VER
#if 0
                        return stdext::hash_compare<int>()((int)using_decl);
#else
                        return rose_hash::hash<int>()((long int)using_decl);
#endif
                                }

};

//  Key: Address of SgUsingDirectiveStatement
//  Value: LinkedListStackSetSgDeclarationStatements
//
// CH (4/8/2010): Use boost::unordered instead      
//#if _MSC_VER
#if 0
typedef rose_hash::unordered_map<SgUsingDeclarationStatement*, LinkedListStackSetSgDeclarationStatements> UsingDeclarationStatement_LinkedListStackSetSgDeclarationStatements_HashMap;
#else
typedef rose_hash::unordered_map<SgUsingDeclarationStatement*, LinkedListStackSetSgDeclarationStatements, HashFunction_SgUsingDeclarationStatement> UsingDeclarationStatement_LinkedListStackSetSgDeclarationStatements_HashMap;
#endif

//  Key: Address of SgUsingDirectiveStatement
//  Value: SetSgDeclarationStatements
//
// CH (4/8/2010): Use boost::unordered instead      
//#if _MSC_VER
#if 0
typedef rose_hash::unordered_map<SgUsingDirectiveStatement*, SetSgDeclarationStatements> UsingDirectiveStatement_SetSgDeclarationStatements_HashMap;
#else
typedef rose_hash::unordered_map<SgUsingDirectiveStatement*, SetSgDeclarationStatements, HashFunction_SgUsingDirectiveStatement> UsingDirectiveStatement_SetSgDeclarationStatements_HashMap;
#endif

//  Key: Address of SgUsingDirectiveStatement
//  Value: SetSgDeclarationStatements
//
// CH (4/8/2010): Use boost::unordered instead      
//#if _MSC_VER
#if 0
typedef rose_hash::unordered_map<SgUsingDeclarationStatement*, SetSgDeclarationStatements> UsingDeclarationStatement_SetSgDeclarationStatements_HashMap;
#else
typedef rose_hash::unordered_map<SgUsingDeclarationStatement*, SetSgDeclarationStatements, HashFunction_SgUsingDeclarationStatement> UsingDeclarationStatement_SetSgDeclarationStatements_HashMap;
#endif

/*
// object for Post-Order Traversal
class SynthesizedAttributeSgScopeStatement {

        public:

                SynthesizedAttributeSgScopeStatement() {};

};
*/

// object for Post-Order Traversal
typedef int SynthesizedAttributeSgScopeStatement;


// object for Pre-Order Traversal (TopDown)
//  -> will provide a stack (vector) of Scopes which will be (the LIFO one) intersected with the current symbol-table
class InheritedAttributeSgScopeStatement {

        public:

                int depth;
                // points to the vector which stores the Symbol-Tables of all scopes like a stack
                std::vector<ScopeStackEntry>* pointer_VectorScopeStack;

                // constructors
                InheritedAttributeSgScopeStatement(int d, std::vector<ScopeStackEntry>* p_VSS)  {
                        depth = d;
                        pointer_VectorScopeStack = p_VSS;
                };

                InheritedAttributeSgScopeStatement(const InheritedAttributeSgScopeStatement &IASSS) : depth(IASSS.depth), pointer_VectorScopeStack(IASSS.pointer_VectorScopeStack) {};

};



class HiddenListComputationTraversal : public AstTopDownBottomUpProcessing<InheritedAttributeSgScopeStatement, SynthesizedAttributeSgScopeStatement> {

        public:

                int xy; // just for debugging purpose

                // Robert Preissl, June 4 2007: Call the intersection procedure only when we are in one of the input-files
                //  (all other files will also be traversed (to collect namespace & class information), but no hiddenlist will be calculated!
                SgSourceFile* sg_file_pointer;

                // hash_map (Key: name / Value: vector of symbols) for storing namespaces (Feature: can be updated)
                //  will be used for updating the current scope if a using-directive or using-declaration is encountered

                // Robert Preissl, June 15 2007: new data structure for the storage of namespace-information:
                // StringVectorHashMap NamespacesHashMap;
                String_VectorOfNamespaceInformation_HashMap NamespacesHashMap;

                // Robert Preissl, June 18 2007: in addition to the NamespacesHashMap
#ifndef _MSC_VER
// tps (11/25/2009) : FIXME; This will not work on windows right now
                String_it_VectorOfNamespaceInformation_boolean NamespacesIteratorHashMap;
#else
// #pragma message ("WARNING: HiddenList : Change implementation to work under windows.")
             // DQ (11/27/2009): I think this is fixed now.
                String_it_VectorOfNamespaceInformation_boolean NamespacesIteratorHashMap;
#endif
                // hash_map (Key: name / Value: vector of symbols) for storing symbols of a class
                //  will be used for updating the current scope if an SgMemberFunctionDeclaration is encountered
                StringVectorHashMap ClassHashMap;

                // query the input files if there are Using Decl. or Directives and make an update of the Scope before the intersection procedure starts
                // Robert Preissl, June 7 2007 Use of sets because faster to find elements (will be done in every Scope)

// CH (4/7/2010) : There is no problem now.
//#ifdef _MSC_VER
// tps (12/07/2009) : FIXME; This will not work on windows right now
                SetSgUsingDirectiveStatementsWithSgScopeStatement UsingDirectivesSet;
                                SetSgUsingDeclarationWithScopeWithSgScopeStatement UsingDeclarationsSet;
//#else
//#pragma message ("WARNING: HiddenList : HiddenListComputationTraversal : Change implementation to work under windows. Does not work with Release (MODE)")
//#endif

                //  For collection_mode 0 of NamespacesAndClassTraversal: a UsingDirectiveStatement_LinkedListStackSetSgDeclarationStatements_HashMap will be built up
                UsingDirectiveStatement_LinkedListStackSetSgDeclarationStatements_HashMap UsingDirRelativeToDeclarations;
                UsingDeclarationStatement_LinkedListStackSetSgDeclarationStatements_HashMap UsingDeclRelativeToDeclarations;

                // For collection_mode 1 of NamespacesAndClassTraversal: just a UsingDirectiveStatement_SetSgDeclarationStatements_HashMap
                //  will be built up (means that only declarations before the using in the same scope are stored)
                UsingDirectiveStatement_SetSgDeclarationStatements_HashMap UsingDirRelativeToDeclarations_2;
                UsingDeclarationStatement_SetSgDeclarationStatements_HashMap UsingDeclRelativeToDeclarations_2;

                // Robert Preissl, June 25 2007, change parameter "vector<ScopeStackEntry>* p_VSS" to "Vector_Of_SymbolInformation temp_symboltable"
                //  -> so we don't update the scope itself, we update the symboltable of the scope which we use for the intersection
                void UsingDirective_in_Scope(SgUsingDirectiveStatement* using_dir_stat, Vector_Of_SymbolInformation& temp_symboltable, int depth, bool scope_can_be_named, int namespace_counter);
                void UsingDeclaration_in_Scope(SgUsingDeclarationStatement* using_decl_stat, Vector_Of_SymbolInformation& temp_symboltable, int depth, bool scope_can_be_named, int namespace_counter, std::string name, std::string scope_qualified_name);

                virtual InheritedAttributeSgScopeStatement evaluateInheritedAttribute(SgNode* n, InheritedAttributeSgScopeStatement inheritedAttribute);
                virtual SynthesizedAttributeSgScopeStatement evaluateSynthesizedAttribute(SgNode* n, InheritedAttributeSgScopeStatement inheritedAttribute, SynthesizedAttributesList SynthesizedAttributeSgScopeStatementList);

                // for using directives
                // Robert Preissl, June 25 2007, change parameter "vector<ScopeStackEntry>* p_VSS" to "Vector_Of_SymbolInformation temp_symboltable"
                //  -> so we don't update the scope itself, we update the symboltable of the scope which we use for the intersection
                void UpdateScope_WithNamespace(Vector_Of_SymbolInformation& temp_symboltable, int depth, VectorOfNamespaceInformation &X, std::string used_namespace, SgUsingDirectiveStatement* using_dir_stat, bool scope_can_be_named, int namespace_counter);
                void UpdateNamespace(std::string updated_namespace, std::string used_namespace);

                // for using declarations
                void UpdateScope_WithNamespace(Vector_Of_SymbolInformation& temp_symboltable, int depth, VectorOfNamespaceInformation &X, std::string used_namespace, SgUsingDeclarationStatement* using_dir_stat, bool scope_can_be_named, std::string n, int namespace_counter);
                void UpdateNamespace(std::string updated_namespace, std::string used_namespace, std::string n);

                void UpdateScope_WithClass(std::vector<ScopeStackEntry>* p_VSS, int depth, Vector_Of_SymbolInformation &X);

                //void CalculateIfIsSameFile(SgNode* n, Sg_File_Info* sg_file_info);



                // CONSTRUCTORS:

                HiddenListComputationTraversal() { };

                HiddenListComputationTraversal(int i, String_VectorOfNamespaceInformation_HashMap& A, StringVectorHashMap& B, UsingDirectiveStatement_LinkedListStackSetSgDeclarationStatements_HashMap& C, UsingDeclarationStatement_LinkedListStackSetSgDeclarationStatements_HashMap& D ) :
                        xy(i),
                        NamespacesHashMap(A),
                        ClassHashMap(B),
                        UsingDirRelativeToDeclarations(C),
                        UsingDeclRelativeToDeclarations(D)
                        {

                        // and let each entry (has same keys of course) of NamespacesIteratorHashMap pointing to the begin of NamespacesHashMap-value
                        String_VectorOfNamespaceInformation_HashMap::iterator it;
                        for(it = NamespacesHashMap.begin(); it != NamespacesHashMap.end(); ++it) {

                                it_VectorOfNamespaceInformation_boolean it_VecOfNamespInfo;
                                it_VecOfNamespInfo.it_vec_namesp_info = (it->second).begin();
                                it_VecOfNamespInfo.first_namespace_occurence = false;

                             // NamespacesIteratorHashMap[it->first] = it_VecOfNamespInfo;

// #ifndef _MSC_VER
// tps (11/25/2009) : FIXME; This will not work on windows right now
                                String_it_VectorOfNamespaceInformation_boolean::iterator foundValue = NamespacesIteratorHashMap.find(it->first);
                                if (foundValue == NamespacesIteratorHashMap.end())
                                    {
                                      NamespacesIteratorHashMap.insert(std::make_pair(it->first,it_VecOfNamespInfo));
                                    }
                                   else
                                    {
                                      foundValue->second = it_VecOfNamespInfo;
                                    }
// #else
// #pragma message ("WARNING HiddenList : Change implementation to work under windows.")
// #endif
                        }

                };

                HiddenListComputationTraversal(int i, String_VectorOfNamespaceInformation_HashMap& A, StringVectorHashMap& B, UsingDirectiveStatement_SetSgDeclarationStatements_HashMap& C, UsingDeclarationStatement_SetSgDeclarationStatements_HashMap& D ) :
                        xy(i),
                        NamespacesHashMap(A),
                        ClassHashMap(B),
                        UsingDirRelativeToDeclarations_2(C),
                        UsingDeclRelativeToDeclarations_2(D)
                        {

                        // and let each entry (has same keys of course) of NamespacesIteratorHashMap pointing to the begin of NamespacesHashMap-value
                        String_VectorOfNamespaceInformation_HashMap::iterator it;
                        for(it = this->NamespacesHashMap.begin(); it != this->NamespacesHashMap.end(); ++it) {

                                it_VectorOfNamespaceInformation_boolean it_VecOfNamespInfo;
                                it_VecOfNamespInfo.it_vec_namesp_info = (it->second).begin();
                                it_VecOfNamespInfo.first_namespace_occurence = false;

// #ifndef _MSC_VER
// tps (11/25/2009) : FIXME; This will not work on windows right now
                             // DQ (9/25/2007): This is a problem piece of code that is caught by STL debugging.
                             // NamespacesIteratorHashMap[it->first] = it_VecOfNamespInfo;
                                String_it_VectorOfNamespaceInformation_boolean::iterator foundValue = NamespacesIteratorHashMap.find(it->first);
                                if (foundValue == NamespacesIteratorHashMap.end())
                                    {
                                      NamespacesIteratorHashMap.insert(std::make_pair(it->first,it_VecOfNamespInfo));
                                    }
                                   else
                                    {
                                      foundValue->second = it_VecOfNamespInfo;
                                   }
// #else
// #pragma message ("WARNING HiddenList : Change implementation to work under windows.")
// #endif                                    
                        }

                };

};





// Robert Preissl, May, 10 2007 : added this function to divide the hiddenlist into subsets
void GetSubsetsOfHiddenList(ScopeStackEntry hiddenlist, SetSgSymbolPointers &AvailableHidden_Functions_Variables, SetSgSymbolPointers &AvailableHidden_Types, SetSgSymbolPointers &ElaboratingForcingHidden_Types);


// DQ (5/8/2007): Added this function to wrap up details of how to call the hidden list work
void buildHiddenTypeAndDeclarationLists( SgNode* node /*SgProject* project*/ );


// Robert Preissl, May 25 2007 : works for Variables, Functions, Typedefs, Classes, Enums, EnumFields
SgDeclarationStatement* GetSgDeclarationStatementOutOfSgSymbol(SgSymbol* p_symbol);


// Robert Preissl, May 25 2007 : works for Variables, Functions, Typedefs, Classes, Enums,
SgName GetSgNameOutOfSgDeclarationStatement(SgDeclarationStatement* decl_stat);


// Robert Preissl, May, 17 2007 : added this function for updating the subsets of the hiddenlist; will be called in SgFunctionDeclaration
void UpdateSubsetOfHiddenList(SetSgSymbolPointers &SubsetOfHiddenList, SetSgSymbolPointers UpdatingSet);


// Robert Preissl, June 1, 2007 : make a new entry of the vector which stores the Symbol-Tables of all scopes like a stack
void CreateNewStackFrame(std::vector<ScopeStackEntry>* p_VSHM, int depth);


void CallIntersection_F (
    std::vector<ScopeStackEntry>* p_VSHM,
    int depth,
    SgFunctionDeclaration* f_decl,
    SetSgSymbolPointers &AvailableHidden_Functions_Variables,
    SetSgSymbolPointers &AvailableHidden_Types,
    SetSgSymbolPointers &ElaboratingForcingHidden_Types,
    UsingDirectiveStatement_LinkedListStackSetSgDeclarationStatements_HashMap& UsingDirRelativeToDeclarations,
    UsingDeclarationStatement_LinkedListStackSetSgDeclarationStatements_HashMap& UsingDeclRelativeToDeclarations,
    UsingDirectiveStatement_SetSgDeclarationStatements_HashMap& UsingDirRelativeToDeclarations_2,
    UsingDeclarationStatement_SetSgDeclarationStatements_HashMap& UsingDeclRelativeToDeclarations_2,
    SetSgUsingDirectiveStatementsWithSgScopeStatement& UsingDirectivesSet,
    SetSgUsingDeclarationWithScopeWithSgScopeStatement& UsingDeclarationsSet
);


void CallIntersection(
    std::vector<ScopeStackEntry>* p_VSHM,
    int depth,
    Vector_Of_SymbolInformation &X,
    SetSgSymbolPointers &AvailableHidden_Functions_Variables,
    SetSgSymbolPointers &AvailableHidden_Types,
    SetSgSymbolPointers &ElaboratingForcingHidden_Types,
    UsingDirectiveStatement_LinkedListStackSetSgDeclarationStatements_HashMap& UsingDirRelativeToDeclarations,
    UsingDeclarationStatement_LinkedListStackSetSgDeclarationStatements_HashMap& UsingDeclRelativeToDeclarations,
    UsingDirectiveStatement_SetSgDeclarationStatements_HashMap& UsingDirRelativeToDeclarations_2,
    UsingDeclarationStatement_SetSgDeclarationStatements_HashMap& UsingDeclRelativeToDeclarations_2,
    SetSgUsingDirectiveStatementsWithSgScopeStatement& UsingDirectivesSet,
    SetSgUsingDeclarationWithScopeWithSgScopeStatement& UsingDeclarationsSet
);


// Robert Preissl, June 7 2007: For Comparing File Infos of SgSymbol*; learn more about it in HiddenList_Intersection.C where it is called!
//  third parameter is the returned SgSymbol* which will be inserted in the hiddenlist
//  functions returns 0 if it couldn't compare the file-info -> no insert into hidden-list
//  functions returns 1 for p_symbol_pointer
//  functions returns 2 for it_symbol_pointer
int CompareFileInfoOfSgSymbols(SgSymbol* p_symbol_pointer, SgSymbol* it_symbol_pointer);


// Robert Preissl, June 15 2007: For Comparing File Names of SgSymbol*; learn more about it in HiddenList_Intersection.C where it is called!
//  functions returns 0 if it couldn't compare the file-info
//  functions returns 1 for equal file names
//  functions returns 2 for different file names
int CompareFileNamesOfSgSymbols(SgSymbol* p_symbol_pointer, SgSymbol* it_symbol_pointer);


// check if there are using-directives or using-declarations in the input code
// Motivation for this, look at Function Definition to learn more!
// Robert Preissl, July 9 2007 : this function is called in astPostProcessing for SgFile
void SetUsingDirectivesSetAndUsingDeclarationsSet(HiddenListComputationTraversal& exampleTraversal, SgFile* project);


bool Find_SetSgDeclarationStatements_In_StackSetSgDeclarationStatements(SetSgDeclarationStatements& find_decl_set,StackSetSgDeclarationStatements& stack, int depth, int depth_of_using);

// Robert Preissl, June 22 2007 : return true if decl_stmt (or one of its parent-decl_stmt) occur in the UsingDirRelativeToDeclarations HashMap
//  need depth, because I don't have to look into the stack deeper as "depth" (means that we have a using in a scope deeper nested as the symbol
//  that is hiding)
bool FindDeclarationInUsingDirRelativeToDeclarations(
    SgDeclarationStatement* decl_stmt,
    int depth,
    UsingDirectiveStatement_LinkedListStackSetSgDeclarationStatements_HashMap& UsingDirRelativeToDeclarations,
    UsingDeclarationStatement_LinkedListStackSetSgDeclarationStatements_HashMap& UsingDeclRelativeToDeclarations,
    SgUsingDirectiveStatement* using_dir,
    int depth_of_using
);


// object for Post-Order Traversal
/*class SynthesizedAttributeForNamespacesAndClassTraversal {

        public:

                SynthesizedAttributeForNamespacesAndClassTraversal() {};

};*/

typedef int SynthesizedAttributeForNamespacesAndClassTraversal;

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

                NamespacesAndClassTraversal(int mode) : collection_mode(mode) {};


                // hash_map (Key: name / Value: vector of symbols) for storing namespaces (Feature: can be updated)
                //  will be used for updating the current scope if a using-directive or using-declaration is encountered

                // Robert Preissl, June 15 2007: new data structure for the storage of namespace-information:
                // StringVectorHashMap NamespacesHashMap;
                String_VectorOfNamespaceInformation_HashMap NamespacesHashMap;

                // Robert Preissl, June 20 2007: Explanation & Motivation; look at typedef above!
                //  collection_mode 0: a UsingDirectiveStatement_LinkedListStackSetSgDeclarationStatements_HashMap will be built up
                UsingDirectiveStatement_LinkedListStackSetSgDeclarationStatements_HashMap UsingDirRelativeToDeclarations;
                UsingDeclarationStatement_LinkedListStackSetSgDeclarationStatements_HashMap UsingDeclRelativeToDeclarations;
                SetSgDeclarationStatements declarations_before_using;
                StackSetSgDeclarationStatements stack_of_declarations;
                LinkedListStackSetSgDeclarationStatements list_of_stack_of_declarations;

                //  collection_mode 1: just a UsingDirectiveStatement_SetSgDeclarationStatements_HashMap will be built up
                //   (means that only declarations before the using in the same scope are stored)
                UsingDirectiveStatement_SetSgDeclarationStatements_HashMap UsingDirRelativeToDeclarations_2;
                UsingDeclarationStatement_SetSgDeclarationStatements_HashMap UsingDeclRelativeToDeclarations_2;

                SgDeclarationStatement* last_using_dir_or_decl_stmt;

                // hash_map (Key: name / Value: vector of symbols) for storing symbols of a class
                //  will be used for updating the current scope if an SgMemberFunctionDeclaration is encountered
                StringVectorHashMap ClassHashMap;

                virtual InheritedAttributeForNamespacesAndClassTraversal evaluateInheritedAttribute(SgNode* n, InheritedAttributeForNamespacesAndClassTraversal inheritedAttribute);
                virtual SynthesizedAttributeForNamespacesAndClassTraversal evaluateSynthesizedAttribute(SgNode* n, InheritedAttributeForNamespacesAndClassTraversal inheritedAttribute, SynthesizedAttributesList SynthesizedAttributeForNamespacesAndClassTraversalList);

                // for using directives
                void UpdateNamespace(std::string updated_namespace, std::string used_namespace);

                // for using declarations
                void UpdateNamespace(std::string updated_namespace, std::string used_namespace, std::string n);

        private:

                // Robert Preissl, June 26 2007 : specifies the collection mode of using-information:
                //  collection_mode 0: a UsingDirectiveStatement_LinkedListStackSetSgDeclarationStatements_HashMap will be built up
                //  collection_mode 1: just a UsingDirectiveStatement_SetSgDeclarationStatements_HashMap will be built up (means that only declarations before the using in the same scope are stored)
                // (of course, the same applies to using-decl.)
                int collection_mode;


};


// Robert Preissl, July 2 2007 : Help-Function called when needed the name and the scope name of a declaration-stmt in a using-decl. (scope-name gives me
//        the name of the namespace)
void GetName_And_Scopequalifiedname_AndScopename_Of_SgDeclStat_In_UsingDecl(SgDeclarationStatement* decl_stat, std::string& name, std::string& scope_qualified_name, std::string& scope_name);


// DQ (5/8/2007): Added this function to wrap up details of how to call the hidden list work
//void buildNamespacesAndClassHashMaps(SgProject* project );
// Robert Preissl, July 9 2007 : added 1st paramter because this function is called in astPostProcessing for SgFile
void buildNamespacesAndClassHashMaps( SgFile* project  /*SgProject* project*/, NamespacesAndClassTraversal& namespaces_and_class_traversal ) ;


// DQ (8/3/2007): timer variables to computing elapsed time
extern double accumulatedIntersectionTime;
extern double accumulatedIntersectionCalls;
extern double accumulatedIntersectionFindTime;
extern double accumulatedIntersectionFindCalls;
extern double accumulatedIntersectionAfterFindTime;
extern double accumulatedIntersectionAfterFindCalls;
extern double accumulatedIntersectionScopeTime[6];
extern double accumulatedIntersectionScopeCalls[6];

extern double accumulatedIntersectionFillScopeStack_1_Time;
extern double accumulatedIntersectionFillScopeStack_1_Calls;
extern double accumulatedIntersectionFillScopeStack_2_Time;
extern double accumulatedIntersectionFillScopeStack_2_Calls;

extern double accumulatedEvaluateInheritedAttribute_1_Time;
extern double accumulatedEvaluateInheritedAttribute_1_Calls;
extern double accumulatedEvaluateInheritedAttributeCommonCaseTime;
extern double accumulatedEvaluateInheritedAttributeCommonCaseCalls;

extern double accumulatedEvaluateInheritedAttribute_2_Time;
extern double accumulatedEvaluateInheritedAttribute_2_Calls;
extern double accumulatedEvaluateSynthesizedAttribute_2_Time;
extern double accumulatedEvaluateSynthesizedAttribute_2_Calls;
extern double accumulatedEvaluateInheritedAttributeCaseTime[14];
extern double accumulatedEvaluateInheritedAttributeCaseCalls[14];

extern double accumulatedCreateNewStackFrameTime;
extern double accumulatedCreateNewStackFrameCalls;

extern double accumulatedUpdateScopeWithClassTime;
extern double accumulatedUpdateScopeWithClassCalls;


} // end of namespace: Hidden_List_Computation


#endif /*DEBUG_HIDDENLIST_H_*/

