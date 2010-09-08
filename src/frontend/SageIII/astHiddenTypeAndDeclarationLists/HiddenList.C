// Implementation of Class Members of HiddenList.h
// Robert Preissl
// Last modified : April 16, 2007

// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "HiddenList.h"
#include "HiddenList_Intersection.h"
#include "HiddenList_Output.h"

// DQ (5/8/2007): This is now in the rose.h header file
// #include "HiddenList.h"

// DQ (5/8/2007): These are now in the HiddenList.h header file
// #include "HiddenList_Output.h"
// #include "HiddenList_Intersection.h"

#define ROSE_TRACK_PROGRESS_OF_ROSE_COMPILING_ROSE 0

using namespace std;

// using own namespace

#include <utility>

namespace Hidden_List_Computation {


double accumulatedEvaluateInheritedAttribute_2_Time  = 0.0;
double accumulatedEvaluateInheritedAttribute_2_Calls = 0.0;

double accumulatedEvaluateSynthesizedAttribute_2_Time  = 0.0;
double accumulatedEvaluateSynthesizedAttribute_2_Calls = 0.0;

double accumulatedEvaluateInheritedAttributeCaseTime[14]  = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
double accumulatedEvaluateInheritedAttributeCaseCalls[14] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

double accumulatedEqstr_SgDeclarationStatementTime  = 0.0;
double accumulatedEqstr_SgDeclarationStatementCalls = 0.0;

double accumulatedCreateNewStackFrameTime  = 0.0;
double accumulatedCreateNewStackFrameCalls = 0.0;

double accumulatedUpdateScopeWithClassTime  = 0.0;
double accumulatedUpdateScopeWithClassCalls = 0.0;
   
//std::ostringstream hidden_list_debug;

// Robert Preissl, June 25 2007, change parameter "vector<ScopeStackEntry>* p_VSS" to "Vector_Of_SymbolInformation temp_symboltable"
//  -> so we don't update the scope itself, we update the symboltable of the scope which we use for the intersection
void HiddenListComputationTraversal::UpdateScope_WithNamespace(Vector_Of_SymbolInformation& temp_symboltable, int depth, VectorOfNamespaceInformation &X, string used_namespace, SgUsingDirectiveStatement* using_dir_stat, bool scope_can_be_named, int namespace_counter) {

        VectorOfNamespaceInformation::iterator it_VecNamespaceInfo;
        Vector_Of_SymbolInformation::iterator it_VecSymbolInfo;
        ScopeStackEntry::iterator find_it;
        SymbolHashMapValue symbol_hashmap_value;
        bool global_scope = false;

        // e.g. using namespace std; in SgGlobal-Scope -> depth would be 0, therefore update at stack position -1
        if(depth < 0) {

                #ifdef HIDDEN_LIST_DEBUG
                        cout << "Using directive in global scope!" << endl;
                #endif

                global_scope = true;

        }

        #ifdef HIDDEN_LIST_DEBUG
                // helpful to see the entries of the last namespace which is used to update temp_symboltable
                //        cout << "size of this->NamespacesIteratorHashMap[used_namespace] : name : " << used_namespace << " size: " <<  ( (*this->NamespacesIteratorHashMap[used_namespace].it_vec_namesp_info).VectorOfSymbolInformation ).size()<< endl;
                //        cout << "(*this->NamespacesIteratorHashMap[used_namespace].it_vec_namesp_info).VectorOfSymbolInformation: " << endl;
                //        VectorOutput( (*this->NamespacesIteratorHashMap[used_namespace].it_vec_namesp_info).VectorOfSymbolInformation );
        #endif

        // Robert Preissl, June 29 2007 : iterator goes too far (until X.end() ) !!
        // Motivating example:
        //        namespace X { int a; }
        //        int main() {
        //                float b;
        //                using namespace X;
        //        }
        //        namespace X { int b; }
        //
        // -> if we would update (the scope where the using occurs) with the whole namespace X, X::int b would hide float b, what is wrong
        // Therefore only update as far as the relevant iterator-entry in NamespacesIteratorHashMap points!!
        //        old code: for(it_VecNamespaceInfo = X.begin(); it_VecNamespaceInfo != X.end(); ++it_VecNamespaceInfo) {

        // Robert Preissl, July 3 2007 : global scope and using

        int bound = 0;

        if(namespace_counter > 0) {

                bound = namespace_counter-1;

        }
        else {

             // bound = this->NamespacesIteratorHashMap[used_namespace].it_vec_namesp_info - X.begin();
                String_it_VectorOfNamespaceInformation_boolean::iterator locationOfIteratorForUsedNamespace = this->NamespacesIteratorHashMap.find(used_namespace);
#if 0
#if 0
                ROSE_ASSERT(locationOfIteratorForUsedNamespace != this->NamespacesIteratorHashMap.end());
                bound = locationOfIteratorForUsedNamespace->second.it_vec_namesp_info - X.begin();
#else
             // DQ (10/5/2007): Is this a reasonable fix?
                if (locationOfIteratorForUsedNamespace != this->NamespacesIteratorHashMap.end())
                     bound = locationOfIteratorForUsedNamespace->second.it_vec_namesp_info - X.begin();
                  else
                     bound = -1;
#endif
#else
            // DQ (10/12/2007): Fix in Liao's patch (suggested by Jeremiah).
            // JJW 10/10/2007 -- this was just an assertion that the
            // namespace was found, but std does not need to be defined
            // before it can be used.
               if (locationOfIteratorForUsedNamespace != this->NamespacesIteratorHashMap.end())
                  {
                    bound = locationOfIteratorForUsedNamespace->second.it_vec_namesp_info - X.begin();
                  }
                 else
                  {
                    if (used_namespace == "std" || used_namespace == "::std")
                       {
                         bound = -1; // Special case for std
                       }
                  }
#endif
             // printf ("In UpdateScope_WithNamespace(): bound = %d \n",bound);
        }

        int b = 0;

        for(it_VecNamespaceInfo = X.begin(); b <= bound; ++it_VecNamespaceInfo) {

                for(it_VecSymbolInfo = (*it_VecNamespaceInfo).VectorOfSymbolInformation.begin(); it_VecSymbolInfo != (*it_VecNamespaceInfo).VectorOfSymbolInformation.end(); ++it_VecSymbolInfo ) {

                        // constructor call of SymbolInformation:
                        //        symbol_information.symbol_pointer = (*it_VecSymbolInfo).symbol_pointer
                        //        symbol_information.name = (*it_VecSymbolInfo).name
                        //        symbol_information.scope_can_be_named = (*it_VecSymbolInfo).scope_can_be_named
                        //        symbol_information.symbol_of_class = (*it_VecSymbolInfo).symbol_of_class
                        //        symbol_information.symbol_is_from_using = true
                        //        symbol_information.depth_of_using = depth
                        //        symbol_information.si_using_dir = using_dir_stat
                        //        symbol_information.si_using_decl = NULL
                        //        symbol_information.is_using_decl_in_class = false
                        //        symbol_information.si_using_decl_in_class = NULL
                        ROSE_ASSERT((*it_VecSymbolInfo) != NULL);

                     // DQ (7/23/2010): Debugging this code be first refactoring it to make it simpler.
                     // temp_symboltable.push_back( new SymbolInformation( (*it_VecSymbolInfo)->symbol_pointer, (*it_VecSymbolInfo)->name, (*it_VecSymbolInfo)->scope_can_be_named, (*it_VecSymbolInfo)->symbol_of_class, true, depth, using_dir_stat, NULL, false, NULL ) );
                        SymbolInformation* temp = new SymbolInformation( (*it_VecSymbolInfo)->symbol_pointer, (*it_VecSymbolInfo)->name, (*it_VecSymbolInfo)->scope_can_be_named, (*it_VecSymbolInfo)->symbol_of_class, true, depth, using_dir_stat, NULL, false, NULL );
                        temp_symboltable.push_back( temp );

                }

                b++;

        }

}


void HiddenListComputationTraversal::UpdateScope_WithNamespace(Vector_Of_SymbolInformation& temp_symboltable, int depth, VectorOfNamespaceInformation &X, string used_namespace, SgUsingDeclarationStatement* using_decl_stat, bool scope_can_be_named, string n, int namespace_counter) {

        VectorOfNamespaceInformation::iterator it_VecNamespaceInfo;
        Vector_Of_SymbolInformation::iterator it_VecSymbolInfo;
        ScopeStackEntry::iterator find_it;
        SymbolHashMapValue symbol_hashmap_value;
        bool global_scope = false;

        // e.g. using namespace std; in SgGlobal-Scope -> depth would be 0, therefore update at stack position -1
        if(depth < 0) {

                #ifdef HIDDEN_LIST_DEBUG
                        cout << "Using declaration in global scope!" << endl;
                #endif

                global_scope = true;

        }

        // Robert Preissl, June 29 2007 : Optimization can be done, because iterator goes too far (until X.end() ) !!
        // Motivating example:
        //        namespace X { int a; }
        //        int main() {
        //                using X::a;
        //                {
        //                        float a; // hides X::int a
        //                }
        //        }
        //        namespace X { int b; }
        //
        // Don't have to walk through all namespaces X to find a symbol that is included via a using-decl.
        // Therefore only search as far as the relevant iterator-entry in NamespacesIteratorHashMap points!!
        //        old code: for(it_VecNamespaceInfo = X.begin(); it_VecNamespaceInfo != X.end(); ++it_VecNamespaceInfo) {


        int bound = 0;

        if(namespace_counter > 0) {

                bound = namespace_counter-1;

        }
        else {

             // bound = this->NamespacesIteratorHashMap[used_namespace].it_vec_namesp_info - X.begin();
                String_it_VectorOfNamespaceInformation_boolean::iterator locationOfIteratorForUsedNamespace = this->NamespacesIteratorHashMap.find(used_namespace);
                ROSE_ASSERT(locationOfIteratorForUsedNamespace != this->NamespacesIteratorHashMap.end());
                bound = locationOfIteratorForUsedNamespace->second.it_vec_namesp_info - X.begin();
        }
        int b = 0;

        // loop, because n could exist several times in vector<symbol_table> (e.g. typedef struct X {} X)
        for(it_VecNamespaceInfo = X.begin(); b <= bound; ++it_VecNamespaceInfo) {

                for(it_VecSymbolInfo = (*it_VecNamespaceInfo).VectorOfSymbolInformation.begin(); it_VecSymbolInfo != (*it_VecNamespaceInfo).VectorOfSymbolInformation.end(); ++it_VecSymbolInfo ) {

                        if( (*it_VecSymbolInfo)->name == n) {

                                // constructor call of SymbolInformation:
                                //        symbol_information.symbol_pointer = (*it_VecSymbolInfo).symbol_pointer
                                //        symbol_information.name = (*it_VecSymbolInfo).name
                                //        symbol_information.scope_can_be_named = (*it_VecSymbolInfo).scope_can_be_named
                                //        symbol_information.symbol_of_class = (*it_VecSymbolInfo).symbol_of_class
                                //        symbol_information.symbol_is_from_using = true
                                //        symbol_information.depth_of_using = depth
                                //        symbol_information.si_using_dir = NULL
                                //        symbol_information.si_using_decl = using_decl_stat
                                //        symbol_information.is_using_decl_in_class = false
                                //        symbol_information.si_using_decl_in_class = NULL
                                temp_symboltable.push_back( new SymbolInformation( (*it_VecSymbolInfo)->symbol_pointer, (*it_VecSymbolInfo)->name, (*it_VecSymbolInfo)->scope_can_be_named, (*it_VecSymbolInfo)->symbol_of_class, true, depth, NULL, using_decl_stat, false, NULL ) );

                        }

                }

                b++;

        }

}


void HiddenListComputationTraversal::UpdateScope_WithClass(vector<ScopeStackEntry>* p_VSS, int depth, Vector_Of_SymbolInformation &X) {

     // DQ (8/3/2007): Start time for this function
        TimingPerformance::time_type startTime;
        TimingPerformance::startTimer(startTime);

        Vector_Of_SymbolInformation::iterator it;

        SymbolHashMapValue symbol_hashmap_value;

        // bool global_scope = false;

     // DQ (9/25/2007): Added assertions!
        ROSE_ASSERT(p_VSS != NULL);
        ROSE_ASSERT(depth >= 0);

#if 0
     // Since depth is >= 0, we don't need this case!

     // e.g. using namespace std; in SgGlobal-Scope -> depth would be 0, therefore update at stack position -1
        if(depth < 0) {

                #ifdef HIDDEN_LIST_DEBUG
                        cout << "Using directive in global scope!" << endl;
                #endif

                global_scope = true;

        }
#endif

        for(it = X.begin(); it != X.end(); ++it) {

#if 0
                ScopeStackEntry::iterator find_it = (*p_VSS)[depth].find( (*it)->name );
                // found it
                if(!global_scope && (find_it != (*p_VSS)[depth].end()) ) {

                    // Robert Preissl: Performance Tuning: use constructor call that passes object directly
                    // update scope:
                    /*
                        symbol_hashmap_value.scope_can_be_named = (*it)->scope_can_be_named;
                        symbol_hashmap_value.is_already_proofed_to_be_valid = true;
                        symbol_hashmap_value.symbol_of_class = (*it)->symbol_of_class;
                        symbol_hashmap_value.is_using_decl_in_class = (*it)->is_using_decl_in_class;
                        symbol_hashmap_value.si_using_decl_in_class = (*it)->si_using_decl_in_class;
                    */

                    (find_it->second)[(*it)->symbol_pointer] = new SymbolHashMapValue(
                        (*it)->scope_can_be_named,     //        symbol_hashmap_value.scope_can_be_named x
                        (*it)->symbol_of_class,        //        symbol_hashmap_value.symbol_of_class x
                        true,                        //        symbol_hashmap_value.is_already_proofed_to_be_valid x
                        false,                        //        symbol_hashmap_value.symbol_is_from_using x
                        (*it)->si_using_dir,           //        symbol_hashmap_value.si_using_dir
                        (*it)->si_using_decl,                  //        symbol_hashmap_value.si_using_decl
                        (*it)->depth_of_using,                   //        symbol_hashmap_value.depth_of_using x
                        (*it)->is_using_decl_in_class, //        symbol_hashmap_value.is_using_decl_in_class x
                        (*it)->si_using_decl_in_class  //        symbol_hashmap_value.si_using_decl_in_class x
                    );

                }
                else {

                    // Robert Preissl: Performance Tuning: use constructor call that passes object directly
                    // update scope:

                    SymbolHashMap temp;

                    temp[(*it)->symbol_pointer] = new SymbolHashMapValue(
                            (*it)->scope_can_be_named,     //        symbol_hashmap_value.scope_can_be_named x
                            (*it)->symbol_of_class,        //        symbol_hashmap_value.symbol_of_class x
                            true,                          //        symbol_hashmap_value.is_already_proofed_to_be_valid x
                            false,                         //        symbol_hashmap_value.symbol_is_from_using x
                            (*it)->si_using_dir,           //        symbol_hashmap_value.si_using_dir
                            (*it)->si_using_decl,          //        symbol_hashmap_value.si_using_decl
                            (*it)->depth_of_using,         //        symbol_hashmap_value.depth_of_using x
                            (*it)->is_using_decl_in_class, //        symbol_hashmap_value.is_using_decl_in_class x
                            (*it)->si_using_decl_in_class  //        symbol_hashmap_value.si_using_decl_in_class x
                    );

                   ((*p_VSS)[depth])[ (*it)->name ] = temp;
               }
#else
                 // DQ (9/25/2007): Change suggested by Jeremiah.
                    SymbolHashMap & temp = ((*p_VSS)[depth])[ (*it)->name ];
                 // if (global_scope) temp.clear();

                    temp[(*it)->symbol_pointer] = new SymbolHashMapValue(
                            (*it)->scope_can_be_named,     //        symbol_hashmap_value.scope_can_be_named x
                            (*it)->symbol_of_class,        //        symbol_hashmap_value.symbol_of_class x
                            true,                          //        symbol_hashmap_value.is_already_proofed_to_be_valid x
                            false,                         //        symbol_hashmap_value.symbol_is_from_using x
                            (*it)->si_using_dir,           //        symbol_hashmap_value.si_using_dir
                            (*it)->si_using_decl,          //        symbol_hashmap_value.si_using_decl
                            (*it)->depth_of_using,         //        symbol_hashmap_value.depth_of_using x
                            (*it)->is_using_decl_in_class, //        symbol_hashmap_value.is_using_decl_in_class x
                            (*it)->si_using_decl_in_class  //        symbol_hashmap_value.si_using_decl_in_class x
                    );
#endif
        }

     // DQ (8/3/2007): accumulate the elapsed time for this function
        TimingPerformance::accumulateTime ( startTime, accumulatedUpdateScopeWithClassTime, accumulatedUpdateScopeWithClassCalls );
}




// Robert Preissl, June 25 2007, change parameter "vector<ScopeStackEntry>* p_VSS" to "Vector_Of_SymbolInformation temp_symboltable"
//  -> so we don't update the scope itself, we update the symboltable of the scope which we use for the intersection
void HiddenListComputationTraversal::UsingDirective_in_Scope(SgUsingDirectiveStatement* using_dir_stat, Vector_Of_SymbolInformation& temp_symboltable, int depth, bool scope_can_be_named, int namespace_counter) {

        if(isSgUsingDirectiveStatement(using_dir_stat)) {

                // Robert Preissl, June 4 2007: Call the intersection procedure only when we are in one of the input-files
                //  (all other files will also be traversed (to collect namespace & class information), but no hiddenlist will be calculated!
                //        Sg_File_Info* sg_file_info = using_dir_stat->get_file_info();
                //        #ifdef HIDDEN_LIST_DEBUG
                //                //cout << " CalculateIfIsSameFile in [isSgUsingDirectiveStatement] " << endl;
                //        #endif
                //        CalculateIfIsSameFile(using_dir_stat, sg_file_info);
                //        //cout << using_dir_stat->unparseToString() << ", " << using_dir_stat->sage_class_name() << endl;


                #ifdef HIDDEN_LIST_DEBUG
                        cout << "---------------" << endl;
                        cout << "Using Directive:  " << using_dir_stat->get_mangled_name().str() << endl;
                        cout << "--------------- " << endl;cout << endl;
                #endif

                SgNamespaceDeclarationStatement* namesp_decl_stat = using_dir_stat->get_namespaceDeclaration();
                string used_namespace = namesp_decl_stat->get_qualified_name().str();

                #ifdef HIDDEN_LIST_DEBUG
                        cout << "Namespace used: " << used_namespace << endl;

                        cout << "Update of temp_symboltable at depth: " << depth << endl;
                #endif

                //        // Namespace Usement is transitive: if using happens in a Nampespace Definition Stat. -> update NamespacesHashMap
                //        SgNamespaceDefinitionStatement* namesp_def_stat = (SgNamespaceDefinitionStatement*)using_dir_stat->get_parent();
                //        if(isSgNamespaceDefinitionStatement(namesp_def_stat)) {
                //                SgNamespaceDeclarationStatement* namesp_decl_stat_2 = namesp_def_stat->get_namespaceDeclaration();
                //                string updated_namespace = namesp_decl_stat_2->get_qualified_name().str();
                //                #ifdef HIDDEN_LIST_DEBUG
                //                        cout << "Transitive Usage of Namespaces!!" << endl;
                //                        cout << "Namepace: " << updated_namespace << " will be updated with the information of namespace: " << used_namespace << endl;
                //                #endif
                //                UpdateNamespace(updated_namespace, used_namespace);
                //        }

                // Robert: April,4 : Update current scope with the symbols of the used namespace:
                UpdateScope_WithNamespace(temp_symboltable, depth, this->NamespacesHashMap[used_namespace], used_namespace, using_dir_stat, scope_can_be_named, namespace_counter) ;

        }

}


void HiddenListComputationTraversal::UsingDeclaration_in_Scope(SgUsingDeclarationStatement* using_decl_stat, Vector_Of_SymbolInformation& temp_symboltable, int depth, bool scope_can_be_named, int namespace_counter, string name, string scope_qualified_name) {

        if( isSgUsingDeclarationStatement(using_decl_stat) ) {

                #ifdef HIDDEN_LIST_DEBUG
                        cout << "---------------" << endl;
                        cout << "Using Declaration!! " << endl;
                        cout << "---------------" << endl; cout << endl;
                #endif

                // Robert Preissl, May 25 2007
                // Motivating example: Cxx_tests: test2005_114.C : using declarations in classes/structs

                // means that using decl. occurs in global scope -> dont' have to calculate name and scope_qualified_name again (already been done where function is called)
                if(namespace_counter > 0) {

                        UpdateScope_WithNamespace(temp_symboltable, depth, this->NamespacesHashMap[scope_qualified_name], scope_qualified_name, using_decl_stat, scope_can_be_named, name, namespace_counter);

                }
                else {

                        if(isSgDeclarationStatement(using_decl_stat->get_declaration()) ) {

                                SgDeclarationStatement* decl_stat = using_decl_stat->get_declaration();

                                ROSE_ASSERT(decl_stat != NULL);

                                //!!! using declarations works now for variables, functions, typedefs, classes, enums
                                if( isSgVariableDeclaration(decl_stat) || isSgFunctionDeclaration(decl_stat) || isSgTypedefDeclaration(decl_stat) || isSgClassDeclaration(decl_stat) || isSgEnumDeclaration(decl_stat) ) {

                                        // hide the names from the paramter list (int this case ok, because they are just dummies)
                                        string name, scope_name, scope_qualified_name;

                                        GetName_And_Scopequalifiedname_AndScopename_Of_SgDeclStat_In_UsingDecl(decl_stat, name, scope_qualified_name, scope_name);

                                        // Robert: April,4 : Update current scope with the symbols of the used namespace:
                                                UpdateScope_WithNamespace(temp_symboltable, depth, this->NamespacesHashMap[scope_qualified_name], scope_qualified_name, using_decl_stat, scope_can_be_named, name, namespace_counter);

                                }

                        }
                        // using decl. in class: -> so parent of the usingdecl. should be a class-def.
                        else if(isSgClassDefinition(using_decl_stat->get_parent()) ) {

                                // SgClassDefinition* class_def = isSgClassDefinition(using_decl_stat->get_parent());

                                // nothing special to be done, because the elements "used" are already in the symbol table

                        }

                }

        }

}


// implemented visitor function for AST-Top-Down Traversal:
// There are several types of nodes which have to be treated differently
//  1. central point of interest is the case of SgScopeStatements, where the symbol table is used to build up a stack of scopes. Also the intersection
//     procedure is called in this if-statement, where the current symbol table is intersected with the current scope
//     Moreover, Namespace- and Class-Information is stored, for later usage, in special data structures
//  2. case of function declarations have also to be treated. Read more about this in the explanation below!
//  3. Usage of namespaces is treated in two different forms: UsingDirectives & Using-Declarations
InheritedAttributeSgScopeStatement HiddenListComputationTraversal :: evaluateInheritedAttribute(SgNode* n, InheritedAttributeSgScopeStatement inheritedAttribute) {

        // DQ (8/3/2007): Start time for this function
        TimingPerformance::time_type startTime;
        TimingPerformance::startTimer(startTime);

#if ROSE_TRACK_PROGRESS_OF_ROSE_COMPILING_ROSE
     // DQ (2/9/2010): Debugging code for ROSE compiling ROSE.
        if (isSgStatement(n) != NULL)
           {
             SgStatement* stmt = isSgStatement(n);
             printf ("In HiddenListComputationTraversal::evaluateInheritedAttribute(): file = %s line = %d \n",stmt->get_startOfConstruct()->get_filenameString().c_str(),stmt->get_startOfConstruct()->get_line());
           }
#endif

        if( isSgSourceFile(n) ) {
                this->sg_file_pointer = isSgSourceFile(n);
             // cout << " >> File-Name: " << this->sg_file_pointer->getFileName() << endl;
        }


        // Robert Preissl, June 1 2007 : Only call the hiddenlist computation for the specified input-files!
        //  Motivation: Reducing the overhead caused by compuation of hiddenlist for the whole AST (included headers,..)
        //  -> find out the file-info and see if its flag "isOutputInCodeGeneration" is true which returns true only if required to be unparsed in generated code.

        // compute it for node here, but this info will be used in cases below..
        //Sg_File_Info* sg_file_info = n->get_file_info();

        // for calculating a set of valid declarations in the current scope
        SgDeclarationStatement* decl_stat = isSgDeclarationStatement(n);
        if(decl_stat != NULL) {

                TimingPerformance::time_type startTimeCase_1;
                TimingPerformance::startTimer(startTimeCase_1);

                #ifdef HIDDEN_LIST_DEBUG
                        cout << "DECLARATION STAT.: ";
                #endif

                // Robert: May 8
                // Motivating example: class A { public: int a; void foo(); A() {} }; // so a, A and foo would be in the symbol table !!!
                //  -> therfore don't make this additional intersection (done below) when it is a memberfunction decl.

                // Robert Preissl, May 14, 2007
                //  ad function overloading: look at motivating example above: -> there are 2 different forms of function overloading here:
                //   *) in a class : function is in the symbol table -> have to treat function overloading in "normal" intersection procedure
                //                   e.g. class A { void foo(); void foo(int x); };  : hiddenlist = 0
                //   **) outside a class: function is NOT in the symbol table -> have to treat function overloading in "function-intersection procedure

                // Robert Preissl, June 5 2007, ad function overloading:
                //  ad *) also in namespaces, the function is visible in the symbol table of the namespace
                //  -> also treat function overloading in "normal" intersection procedure

                if(isSgFunctionDeclaration(decl_stat) && !isSgMemberFunctionDeclaration(decl_stat) && !isSgNamespaceDefinitionStatement(decl_stat->get_parent()) ) {

                        // Robert Preissl, June 4 2007: Call the intersection procedure only when we are in one of the input-files
                        //  (all other files will also be traversed (to collect namespace & class information), but no hiddenlist will be calculated!
                        //        Sg_File_Info* sg_file_info = decl_stat->get_file_info();
                        //        #ifdef HIDDEN_LIST_DEBUG
                        //                //cout << " CalculateIfIsSameFile in [if(isSgFunctionDeclaration(decl_stat) && !isSgMemberFunctionDeclaration(decl_stat) && !isSgNamespaceDefinitionStatement(decl_stat->get_parent()) )] " << endl;
                        //        #endif
                        //        CalculateIfIsSameFile(n, sg_file_info);

                        // Robert: April 5
                        // Due to the fact that function decl. don't appear in the symbol table, they are not treated in the intersection
                        //  procedure in the scope statement -> therefore have to look for every function decl. if its NAME appears in the
                        //  current scope!
                        //  !! regarding the motivating example below, doo() need not be defined to be hidden !! of course the statement
                        //     int ed = doo() wouldn't be correct, but doo() would again hide int doo = 9
                        //
                        // motivating example:
                        //   int main() { int doo = 9; { int doo(); int ed = doo(); } return 0; }
                        //   int doo() { return 0; }
                        SgFunctionDeclaration* func_decl = isSgFunctionDeclaration(decl_stat);
                        #ifdef HIDDEN_LIST_DEBUG
                                cout << "Function Declaration: ( with function name: " << func_decl->get_name().str() << ")";
                                cout << " .. name: " << decl_stat->get_mangled_name().str() << " / class name: " << decl_stat->sage_class_name() << " *** address (of SgDecl.Stat.): " << decl_stat << endl;
                                cout << endl;
                        #endif


                        SetSgSymbolPointers AvailableHidden_Functions_Variables;
                        SetSgSymbolPointers AvailableHidden_Types;
                        SetSgSymbolPointers ElaboratingForcingHidden_Types;

#ifndef _MSC_VER
                        // for gprof
                        CallIntersection_F(
                            inheritedAttribute.pointer_VectorScopeStack,
                            inheritedAttribute.depth,
                            func_decl,
                            AvailableHidden_Functions_Variables,
                            AvailableHidden_Types,
                            ElaboratingForcingHidden_Types,
                            this->UsingDirRelativeToDeclarations,
                            this->UsingDeclRelativeToDeclarations,
                            this->UsingDirRelativeToDeclarations_2,
                            this->UsingDeclRelativeToDeclarations_2,
							this->UsingDirectivesSet,
							this->UsingDeclarationsSet
                        );
#else
// tps (12/7/2009) This is currently not defined since it fails in Release mode
#pragma message ("WARNING: HiddenList: InheritedAttributeSgScopeStatement: UsingDirectivesSet commented out right now.")
ROSE_ASSERT(false);

#endif
                        if(!AvailableHidden_Functions_Variables.empty() || !AvailableHidden_Types.empty() || !ElaboratingForcingHidden_Types.empty()  ) {

                                #ifdef HIDDEN_LIST_DEBUG
                                        cout << "additional List of Symbols hidden by a FUNCTION (Scope: )" << endl; //SgScope->get_qualified_name().str() << endl;

                                        OutputSubsetsOfHiddenList(AvailableHidden_Functions_Variables, AvailableHidden_Types, ElaboratingForcingHidden_Types);
                                        cout << " /////////////////////////////////////////////////////////////////////////////////// " << endl; cout <<  endl;
                                #endif



                                SetSgSymbolPointers UpdatingSet;

                                // Robert Preissl, May 11, 2007
                                // update the three lists of hidden-symbols of SgScopeStatement
                                SgScopeStatement* SgScope = NULL;

                                if(isSgScopeStatement(func_decl->get_parent() ) ) {

                                        SgScope = isSgScopeStatement(func_decl->get_parent() );

                                        UpdatingSet = SgScope->get_hidden_declaration_list();
                                        UpdateSubsetOfHiddenList(AvailableHidden_Functions_Variables, UpdatingSet);
                                        SgScope->set_hidden_declaration_list(AvailableHidden_Functions_Variables);

                                        UpdatingSet = SgScope->get_hidden_type_list();
                                        UpdateSubsetOfHiddenList(AvailableHidden_Types, UpdatingSet);
                                        SgScope->set_hidden_type_list(AvailableHidden_Types);

                                        UpdatingSet = SgScope->get_type_elaboration_list();
                                        UpdateSubsetOfHiddenList(ElaboratingForcingHidden_Types, UpdatingSet);
                                        SgScope->set_type_elaboration_list(ElaboratingForcingHidden_Types);

                                        #ifdef HIDDEN_LIST_DEBUG
                                                cout << " >>> SgFunctionDeclaration:  Output of updated hiddenlists: " << endl;
                                                // OutputSubsetsOfHiddenList(SgScope->get_hidden_declaration_list(), SgScope->get_hidden_type_list(),SgScope->get_type_elaboration_list() );
                                                // OutputSubsetsOfHiddenList(AvailableHidden_Functions_Variables, AvailableHidden_Types, ElaboratingForcingHidden_Types );
                                        #endif

                                }
                                else if(isSgTemplateInstantiationDirectiveStatement(func_decl->get_parent()) ) {
                                        SgTemplateInstantiationDirectiveStatement* template_inst_dir_stat = isSgTemplateInstantiationDirectiveStatement(func_decl->get_parent());
                                        if(isSgScopeStatement(template_inst_dir_stat->get_parent()) ) {

                                                SgScope = isSgScopeStatement(template_inst_dir_stat->get_parent());

                                                UpdatingSet = SgScope->get_hidden_declaration_list();

                                                UpdateSubsetOfHiddenList(AvailableHidden_Functions_Variables, UpdatingSet);

                                                SgScope->set_hidden_declaration_list(AvailableHidden_Functions_Variables);

                                                UpdatingSet = SgScope->get_hidden_type_list();
                                                UpdateSubsetOfHiddenList(AvailableHidden_Types, UpdatingSet);
                                                SgScope->set_hidden_type_list(AvailableHidden_Types);

                                                UpdatingSet = SgScope->get_type_elaboration_list();
                                                UpdateSubsetOfHiddenList(ElaboratingForcingHidden_Types, UpdatingSet);
                                                SgScope->set_type_elaboration_list(ElaboratingForcingHidden_Types);

                                        }
                                        else {
                                                cout << "ERROR 2" << endl;
                                        }
                                }
                                else {
                                        cout << "ERROR 1" << endl;
                                }

                        }

                }


                // Robert Preissl, May 15 2007: Mark those elements in Stack of current scope which are valid:
                //  1st: find the HashMap-Value in the current stack entry which has a SgDeclarationStatement "decl_stat"
                //  works now for variables, functions, typedefs, classes, enums

                // Robert Preissl, May 30 2007, update for namespaces, templates

                // Robert Preissl, May 25 2007 added SgTemplateDeclaration

                // Robert Preissl, June 4 2007: Call the intersection procedure only when we are in one of the input-files
                //  (all other files will also be traversed (to collect namespace & class information), but no hiddenlist will be calculated!
                // -> therefore don't have to mark declarations as visible when not in one of the input-files, exception: if global scope, then also mark them if visible!

                if( isSgVariableDeclaration(decl_stat) || isSgFunctionDeclaration(decl_stat) || isSgTypedefDeclaration(decl_stat) || isSgClassDeclaration(decl_stat) || isSgEnumDeclaration(decl_stat) || isSgNamespaceDeclarationStatement(decl_stat) || isSgTemplateDeclaration(decl_stat) ) {

                        #ifdef HIDDEN_LIST_DEBUG
                                cout << " .. name: " << decl_stat->sage_class_name() << " *** address (of SgDecl.Stat.): " << decl_stat << endl;
                                cout << endl;
                        #endif

                        // Robert Preissl, May 11, 2007
                        //  Motivating Example, for Marking the entries of SymbolHashMap as valid if they were proofed to be valid once
                        //  int main() {
                        //        int a;
                        //        {
                        //                int x; // -> x number 2
                        //                {
                        //                        // after intersecting the current symbol table (b,x,y) with the current (not valid!!) scope (a,x,x,y)
                        //                        //  we find out that only x number 2 is valid by running through the valid scope (set<SgDeclarationsStatement)
                        //                        //   -> !! mark it as valid for the intersection
                        //                        int b, x, y;
                        //                }
                        //                int y;
                        //                {
                        //                        int x; // after interecting need not verify validity of x number 2 !!! (so don't need to run through the set again)
                        //                        double b;
                        //                }
                        //        }
                        //        int x; // -> x number 1
                        //        return 0;
                        //  }

                        SgName decl_stat_name = GetSgNameOutOfSgDeclarationStatement(decl_stat);

                        // Robert Preissl, May 22 2007, ad testcode Cxx_tests: 2003_30.C : unsigned int :16; -> no name!! therefore decl_stat_name == NULL
                        //ROSE_ASSERT(decl_stat_name != NULL);

                        ScopeStackEntry::iterator find_it;

                        find_it = (*inheritedAttribute.pointer_VectorScopeStack)[inheritedAttribute.depth-1].find( decl_stat_name.str() );

                        // found it
                        if(find_it != (*inheritedAttribute.pointer_VectorScopeStack)[inheritedAttribute.depth-1].end()) {

                                SymbolHashMap:: iterator it_symbol_hashmap;
                                SgSymbol* p_symbol;

                                pair<string const, SymbolHashMap> &Ref_to_ScopeStackEntry = *find_it;

                                // iterate through all entries of the HashMap-Value
                                for(it_symbol_hashmap = Ref_to_ScopeStackEntry.second.begin(); it_symbol_hashmap != Ref_to_ScopeStackEntry.second.end(); ++it_symbol_hashmap) {

                                        // 2nd: find the corresponding SgDeclarationStatement
                                        ROSE_ASSERT(it_symbol_hashmap->first != NULL);
                                        p_symbol = it_symbol_hashmap->first;

                                        // DQ (5/22/2007): Added assertion
                                        ROSE_ASSERT(p_symbol != NULL);

                                        // .. find out its declaration statement ..
                                        SgDeclarationStatement* decl_stat_symbol_hashmap;

                                        decl_stat_symbol_hashmap = GetSgDeclarationStatementOutOfSgSymbol(p_symbol);
                                        ROSE_ASSERT(decl_stat != NULL);

                                        // Robert Preissl, May 23 2007, : TestCode: test2001_11.C _S_ios_base_init / SgVariableSymbol had an initalized name
                                        //  not NULL nut the parent (SgVariableDeclaration* was NULL)
                                        //ROSE_ASSERT(decl_stat_symbol_hashmap != NULL);
                                        if(decl_stat_symbol_hashmap != NULL) {

                                                if( decl_stat_symbol_hashmap->get_mangled_name() == decl_stat->get_mangled_name() ) {

                                                        pair<SgSymbol* const, SymbolHashMapValue*> &Ref_to_symbol_hashmap = *it_symbol_hashmap;
                                                        (Ref_to_symbol_hashmap.second)->is_already_proofed_to_be_valid = true;

                                                }

                                        }
                                        else {
                                                cout << "In hiddenlist analysis WARNING: -> decl_stat_symbol_hashmap == NULL, means that to a non-NULL SgSymbol* doesn't exist a non-NULL SgDeclarationStatement* " << endl;
                                        }

                                }

                        }
                        else {
                                // Don't panic!!!!! Means that a declaration doesn't appear in the symbol table yet. (e.g. in classes is order of declarations not important
                        }

                }
                else if( isSgUsingDirectiveStatement(decl_stat) ) {

                        SgUsingDirectiveStatement* using_dir_stmt = isSgUsingDirectiveStatement(decl_stat);

                        // ad comment "Robert Preissl, June 26 2007 : add a bool-variable, which stores if
                        //   this SgUsingDirectiveStatement has already been encountered or not"
                        SetSgUsingDirectiveStatementsWithSgScopeStatement::iterator it_UsingDirectivesSet;

                        UsingDirectiveWithScope dummy_UsingDirectiveWithScope;
                        dummy_UsingDirectiveWithScope.using_dir = using_dir_stmt;

                        ROSE_ASSERT(using_dir_stmt->get_parent() != NULL);
                        dummy_UsingDirectiveWithScope.scope = isSgScopeStatement(using_dir_stmt->get_parent());
                        dummy_UsingDirectiveWithScope.is_already_proofed_to_be_valid = false;

// CH (4/7/2010) : Issue fixed as above
//#ifndef _MSCx_VER
                        it_UsingDirectivesSet = this->UsingDirectivesSet.find( dummy_UsingDirectiveWithScope );

                        if(it_UsingDirectivesSet != this->UsingDirectivesSet.end() ) {

                                #ifdef HIDDEN_LIST_DEBUG
                                        cout << " ----> isSgUsingDirectiveStatement found in this->UsingDirectivesSet" << endl;
                                #endif

                                // erase the entry with SgUsingDirectiveStatement* = using_dir_stmt and..
                                this->UsingDirectivesSet.erase(it_UsingDirectivesSet);

                                // .. replace it with
                                dummy_UsingDirectiveWithScope.is_already_proofed_to_be_valid = true;
                                this->UsingDirectivesSet.insert(dummy_UsingDirectiveWithScope);

                        }
                        else {

                                cout << "ERROR: it_UsingDirectivesSet != this->UsingDirectivesSet.end()" << endl;

                        }

//#else
//							// tps (12/7/2009) This is currently not defines since it fails in Release mode
//#pragma message ("WARNING: HiddenList: InheritedAttributeSgScopeStatement2 : UsingDirectivesSet commented out right now.")
//#endif
                }

                if( isSgUsingDeclarationStatement(decl_stat) ) {

                        SgUsingDeclarationStatement* using_decl_stmt = isSgUsingDeclarationStatement(decl_stat);

                        // ad comment "Robert Preissl, June 26 2007 : add a bool-variable, which stores if
                        //   this SgUsingDeclarationStatement has already been encountered or not"
                        SetSgUsingDeclarationWithScopeWithSgScopeStatement::iterator it_UsingDeclarationSet;

                        UsingDeclarationWithScope dummy_UsingDeclarationWithScope;
                        dummy_UsingDeclarationWithScope.using_decl = using_decl_stmt;

                        ROSE_ASSERT(using_decl_stmt->get_parent() != NULL);
                        dummy_UsingDeclarationWithScope.scope = isSgScopeStatement(using_decl_stmt->get_parent());
                        dummy_UsingDeclarationWithScope.is_already_proofed_to_be_valid = false;

// CH (4/7/2010) : Issue fixed as above
//#ifndef _MSCx_VER
                        it_UsingDeclarationSet = this->UsingDeclarationsSet.find( dummy_UsingDeclarationWithScope );

                        if(it_UsingDeclarationSet != this->UsingDeclarationsSet.end() ) {

                                #ifdef HIDDEN_LIST_DEBUG
                                        cout << " ----> isSgUsingDeclarationStatement found in this->UsingDeclarationsSet" << endl;
                                #endif

                                // erase the entry with SgUsingDeclarationStatement* = using_decl_stmt and..
                                this->UsingDeclarationsSet.erase(it_UsingDeclarationSet);

                                // .. replace it with
                                dummy_UsingDeclarationWithScope.is_already_proofed_to_be_valid = true;
                                this->UsingDeclarationsSet.insert(dummy_UsingDeclarationWithScope);

                        }
                        else {

                                cout << "ERROR: it_UsingDeclarationSet != this->UsingDeclarationsSet.end()" << endl;

                        }
//#else
//							// tps (12/7/2009) This is currently not defines since it fails in Release mode
//#pragma message ("WARNING: HiddenList: InheritedAttributeSgScopeStatement : UsingDeclarationSet commented out right now.")
//#endif

                }
                else {
                        #ifdef HIDDEN_LIST_DEBUG
                                cout << "OTHER Declaration ";
                                cout << " .. name: " << decl_stat->get_mangled_name().str() << " / class-name: " << decl_stat->sage_class_name() << " *** address (of SgDecl.Stat.): " << decl_stat << endl;
                                cout << endl;
                        #endif
                }



                // DQ (8/3/2007): accumulate the elapsed time for this function
                TimingPerformance::accumulateTime ( startTimeCase_1, accumulatedEvaluateInheritedAttributeCaseTime[1], accumulatedEvaluateInheritedAttributeCaseCalls[1] );

        }

        bool global_scope = false;

        if(isSgGlobal(n)) {

                global_scope = true;

                #ifdef HIDDEN_LIST_DEBUG
                        cout << "---------------" << endl;
                        cout << "GLOBAL!!!!!!!!!!!!" << endl;
                        cout << " ---------------" << endl;
                        cout << endl;
                #endif

        }

        // ad using directives:
        //  if isSgScopeStatement is also an isSgNamespaceDefinitionStatement -> store in StringSetHashMap Class-Attribute of class InheritedAttributeVisitorTraversal all the names of the namespace.
        //  if this namespace will be used later -> copy the entries to the valid scope
        bool namespace_definition = false;

        SgScopeStatement* SgScope = isSgScopeStatement(n);
        if (SgScope != NULL) {

                TimingPerformance::time_type startTimeCase_2;
                TimingPerformance::startTimer(startTimeCase_2);

                #ifdef HIDDEN_LIST_DEBUG
                        cout << "**************** SCOPE BEGIN *****************************************************"<<endl; cout << endl;

                        cout << "Scope Name: " << SgScope->get_qualified_name().str();
                        cout << " in depth: " << inheritedAttribute.depth << endl;
                #endif

                SgNamespaceDefinitionStatement* namespace_def_stmt = isSgNamespaceDefinitionStatement(n);
                if(namespace_def_stmt != NULL) {

                        #ifdef HIDDEN_LIST_DEBUG
                                cout << "Namespace Definition with name: " << namespace_def_stmt->get_qualified_name().str() << endl;
                        #endif
                        namespace_definition = true;

                }


                // Robert Preissl, June 4 2007: Call the intersection procedure only when we are in one of the input-files
                //  (all other files will also be traversed (to collect namespace & class information), but no hiddenlist will be calculated!

                // Robert Preissl, June 8 2007: Redesign of HiddenList-Computation: Don't need this enay more because only traverseInputFiles is chosen
                //  for the hiddenlist-computation; for more information look at "void buildHiddenTypeAndDeclarationLists"

                //        Sg_File_Info* sg_file_info = SgScope->get_file_info();
                //        // cout << SgScope->unparseToString() << ", " << SgScope->sage_class_name() << endl;
                //        #ifdef HIDDEN_LIST_DEBUG
                //                //cout << " CalculateIfIsSameFile in [isSgScopeStatement] " << endl;
                //        #endif
                //        CalculateIfIsSameFile(n, sg_file_info);


                // Robert Preissl: May, 10, 2007
                //  There are 3 different kinds of hiding (available Hiding, Unavailable Hiding, Elaborating-Forcing Hiding)
                //   and we have to distinguish between Variables/Functions and Types to be hidden!!
                //  Elaborating-Forcing Hiding for Functions/Variables is impossible and also a Unavailable-hidden-List would not
                //   be very interesting
                //   -> Therefore have to supply 3 sets of SgSymbol* for every SgScopeStatement :
                //        *) Set of available Hiding for Functions/Variables
                //        **) Set of available Hiding for Types
                //        ***) Set of Elaborating-Forcing Hiding for Types (for classes, struct & enums)

                SetSgSymbolPointers AvailableHidden_Functions_Variables;
                SetSgSymbolPointers AvailableHidden_Types;
                SetSgSymbolPointers ElaboratingForcingHidden_Types;

                Vector_Of_SymbolInformation temp_symboltable;

                SgSymbolTable::BaseHashType* hashTable = SgScope->get_symbol_table()->get_table();

                // DQ (5/31/2007): Modified to call equivalent function (then removed)
                // bool can_be_named = (bool)SgScope->supportsDefiningFunctionDeclaration();
                bool can_be_named = (bool)SgScope->containsOnlyDeclarations();

                if(can_be_named) {

                        #ifdef HIDDEN_LIST_DEBUG
                                cout << "containsOnlyDeclarations -> can be named! " << endl;
                        #endif

                }

                if(hashTable != NULL) {

                        TimingPerformance::time_type startTimeCase_3;
                        TimingPerformance::startTimer(startTimeCase_3);

                        SymbolInformation symbol_information;

                        bool class_definition = false;
                        if(isSgClassDefinition(n)) {

                                class_definition = true;

                        }

                        SgSymbolTable::hash_iterator SgSymbolTable_it = hashTable->begin();

                        // Iterate through the ROSE symbol table

                        // Robert Preissl, July 3 2007 : don't have to iterate through the ROSE-symbol-table if we are in a class, because
                        //   this information has already been collected by the 1st traversal

                        // Robert Preissl, July 5 2007 : the same applies to namespaces
                        while ( !namespace_definition && !class_definition && (SgSymbolTable_it != hashTable->end() ) ) {

                                TimingPerformance::time_type startTimeCase_4;
                                TimingPerformance::startTimer(startTimeCase_4);

                                // constructor call of SymbolInformation:
                                //        symbol_information.symbol_pointer = (*it_VecSymbolInfo).symbol_pointer
                                //        symbol_information.name = (*it_VecSymbolInfo).name
                                //        symbol_information.scope_can_be_named = (*it_VecSymbolInfo).scope_can_be_named
                                //        symbol_information.symbol_of_class = (*it_VecSymbolInfo).symbol_of_class
                                //        symbol_information.symbol_is_from_using = true
                                //        symbol_information.depth_of_using = depth
                                //        symbol_information.si_using_dir = using_dir_stat
                                //        symbol_information.si_using_decl = NULL
                                //        symbol_information.is_using_decl_in_class = false
                                //        symbol_information.si_using_decl_in_class = NULL

                                temp_symboltable.push_back( new SymbolInformation( SgSymbolTable_it->second, SgSymbolTable_it->first.getString(), can_be_named, class_definition, false, inheritedAttribute.depth, NULL, NULL, false, NULL ) );

                                SgSymbolTable_it++;

                                TimingPerformance::accumulateTime ( startTimeCase_4, accumulatedEvaluateInheritedAttributeCaseTime[4], accumulatedEvaluateInheritedAttributeCaseCalls[4] );
                        }

                        if(namespace_definition) {

                                TimingPerformance::time_type startTimeCase_5;
                                TimingPerformance::startTimer(startTimeCase_5);

                                // first look-up if an entry with this namespace name already exists (e.g. namespace A { int a;} ... namespace A{int b;}
                                string namespace_name = namespace_def_stmt->get_qualified_name().str();

                                String_VectorOfNamespaceInformation_HashMap::iterator find_it;

                                find_it = this->NamespacesHashMap.find( namespace_name );
                                // if yes -> update
                                if(find_it != this->NamespacesHashMap.end() ) {

                                        // check if its the first time that we are meeting this namespace (in the constructor will be set to false)
                                        // if( (this->NamespacesIteratorHashMap[ namespace_name ]).first_namespace_occurence == false ) {
                                        String_it_VectorOfNamespaceInformation_boolean::iterator foundInIteratorHashMap = this->NamespacesIteratorHashMap.find(namespace_name);
                                        if (foundInIteratorHashMap == this->NamespacesIteratorHashMap.end() || foundInIteratorHashMap->second.first_namespace_occurence == false) {
                                           
                                                // if yes, then jump over those entries which are from header-files! (header file namespace-entries are always at the beginning)
                                                // Motivating example: let's assume that there was a namespace Y {int t;} in a header before those two namespace Y above
                                                it_VectorOfNamespaceInformation it_vectorofnamespaceinformation;

                                                bool first_non_header_namespace = true;

                                                for(it_vectorofnamespaceinformation = find_it->second.begin(); (it_vectorofnamespaceinformation != find_it->second.end() ) && first_non_header_namespace; ++it_vectorofnamespaceinformation ) {

                                                        TimingPerformance::time_type startTimeCase_11;
                                                        TimingPerformance::startTimer(startTimeCase_11);

                                                        // calculate the file-info from the namespace-definition-stmt and..
                                                        Sg_File_Info* sg_file_info = (*it_vectorofnamespaceinformation).namespace_definition_stmt->get_file_info();
                                                        ROSE_ASSERT(sg_file_info != NULL);

                                                        // ..compare it to the current file-info
                                                        if( sg_file_info->isSameFile(this->sg_file_pointer) ) {

                                                                // overwrite the entry in the NamespacesIteratorHashMap when we are pointing to the first time to a
                                                                //  namespace entry of the same file
                                                                it_VectorOfNamespaceInformation_boolean it_VecOfNamespInfo;
                                                                it_VecOfNamespInfo.it_vec_namesp_info = it_vectorofnamespaceinformation;
                                                                it_VecOfNamespInfo.first_namespace_occurence = true;

                                                                #ifdef HIDDEN_LIST_DEBUG
                                                                        //        VectorOutput( (*it_VecOfNamespInfo.it_vec_namesp_info).VectorOfSymbolInformation );
                                                                #endif

                                                             // this->NamespacesIteratorHashMap[ namespace_name ] = it_VecOfNamespInfo;
                                                                if (foundInIteratorHashMap == NamespacesIteratorHashMap.end())
                                                                   {
                                                                     NamespacesIteratorHashMap.insert(std::make_pair(namespace_name,it_VecOfNamespInfo));
                                                                   }
                                                                  else
                                                                   {
                                                                     foundInIteratorHashMap->second = it_VecOfNamespInfo;
                                                                   }

                                                                first_non_header_namespace = false;

                                                        }

                                                        TimingPerformance::accumulateTime ( startTimeCase_11, accumulatedEvaluateInheritedAttributeCaseTime[11], accumulatedEvaluateInheritedAttributeCaseCalls[11] );
                                                }

                                                //if no non_header_namespace entry with this name, let him show to the end:
                                                // (means that there are only namespace occurences of this name in the header)
                                                if(first_non_header_namespace) {

                                                        //TODO: end nicht ganz richtig?? oder doch?? testen!!!, auf letztem eintrag!!

                                                        it_VectorOfNamespaceInformation_boolean it_VecOfNamespInfo;
                                                        it_VecOfNamespInfo.it_vec_namesp_info = find_it->second.end();
                                                        it_VecOfNamespInfo.first_namespace_occurence = true;

                                                     // this->NamespacesIteratorHashMap[ namespace_name ] = it_VecOfNamespInfo;
                                                        if (foundInIteratorHashMap == NamespacesIteratorHashMap.end())
                                                           {
                                                             NamespacesIteratorHashMap.insert(std::make_pair(namespace_name,it_VecOfNamespInfo));
                                                           }
                                                          else
                                                           {
                                                             foundInIteratorHashMap->second = it_VecOfNamespInfo;
                                                           }
                                                }

                                        }
                                        else {

                                                // means that there was already an occurence of namespace "namespace_name", so let adequate NamespacesIteratorHashMap
                                                //  entry point to the next
                                             // it_VectorOfNamespaceInformation_boolean it_VecOfNamespInfo = this->NamespacesIteratorHashMap[ namespace_name ];
                                                it_VectorOfNamespaceInformation_boolean & it_VecOfNamespInfo = foundInIteratorHashMap->second;
                                                ++it_VecOfNamespInfo.it_vec_namesp_info;
                                                it_VecOfNamespInfo.first_namespace_occurence = true;

                                             // this->NamespacesIteratorHashMap[ namespace_name ] = it_VecOfNamespInfo;
                                             // foundInIteratorHashMap->second = it_VecOfNamespInfo;

                                        }

                                        // Robert Preissl, June 6 : if namespace already exists, also have to update the scope
                                        //  Motivating example:
                                        //        namespace Y {
                                        //                class B;
                                        //        }
                                        //        namespace Y {
                                        //                // This function declaration A hides class B and type elaboration is enough to resolve class A.
                                        //                void B();
                                        //                //B* b1; // not allowed
                                        //                class  B* b1;
                                        //        }

                                        it_VectorOfNamespaceInformation it_VecOfNamespInfo;
                                        Vector_Of_SymbolInformation::iterator it_VecSymbolInfo;

                                     // printf ("this->NamespacesHashMap[namespace_name].size() = %zu \n",this->NamespacesHashMap[namespace_name].size());
                                     // int bound = this->NamespacesIteratorHashMap[ namespace_name ].it_vec_namesp_info - this->NamespacesHashMap[namespace_name].begin();
                                     // int bound = foundInIteratorHashMap->second.it_vec_namesp_info - find_it->second.begin();
                                     // int bound = this->NamespacesIteratorHashMap[ namespace_name ].it_vec_namesp_info - this->NamespacesHashMap[namespace_name].begin();
                                        int bound = foundInIteratorHashMap->second.it_vec_namesp_info - find_it->second.begin();
                                        int b = 0;

#ifdef HIDDEN_LIST_DEBUG
                                        cout << " VectorOutput( (*it_VecOfNamespInfo.it_vec_namesp_info).VectorOfSymbolInformation: " << endl;
                                        VectorOutput( (*(this->NamespacesIteratorHashMap[ namespace_name ]).it_vec_namesp_info).VectorOfSymbolInformation );
#endif

                                     // DQ (3/5/2009): This is the original code (which allows all the C++ test codes to pass).
                                     // This is required for test2005_206.C to pass, however it had to be modified to allow the outlining test to a separate file to pass.

                                     // printf ("b = %d bound = %d find_it->second.size() = %zu \n",b,bound, find_it->second.size());

                                     // for(it_VecOfNamespInfo = this->NamespacesHashMap[namespace_name].begin(); (b <= bound); ++it_VecOfNamespInfo) {
                                     // for(it_VecOfNamespInfo = find_it->second.begin(); (b <= bound); ++it_VecOfNamespInfo)
                                        for(it_VecOfNamespInfo = find_it->second.begin(); (b <= bound); ++it_VecOfNamespInfo)
                                           {
                                          // if ((*it_VecOfNamespInfo).VectorOfSymbolInformation.empty() == false)
                                          // printf ("(*it_VecOfNamespInfo).VectorOfSymbolInformation.size() = %zu \n",(*it_VecOfNamespInfo).VectorOfSymbolInformation.size());

                                          // DQ (3/5/2009): Make sure that the test of "b <= bound" does not cause us to iterate past the last element of the list.
                                             if (it_VecOfNamespInfo != find_it->second.end())
                                                {
                                                  for(it_VecSymbolInfo = (*it_VecOfNamespInfo).VectorOfSymbolInformation.begin(); it_VecSymbolInfo != (*it_VecOfNamespInfo).VectorOfSymbolInformation.end(); ++it_VecSymbolInfo )
                                                     {
                                                    // cout << "Update with NAME: " << (*it_VecSymbolInfo).name << endl;
                                                       temp_symboltable.push_back( (*it_VecSymbolInfo) );
#ifdef HIDDEN_LIST_DEBUG
                                                       cout << "Update of temp_symboltable" << endl;
                                                       VectorOutput(temp_symboltable);
#endif
                                                     }
                                                }
                                                b++;
                                           }
                                }
                                else {

                                        // this->NamespacesHashMap[ namespace_name ] = temp_symboltable;
                                        cout << " ERROR 1: Namespace should have been collected by NamespaceAndClassTraversal!! " << endl;

                                }

                                #ifdef HIDDEN_LIST_DEBUG
                                        //cout << "NAMESPACES: " << endl;
                                        //StringVectorHashMapOutput(this->NamespacesHashMap);
                                #endif


                                TimingPerformance::accumulateTime ( startTimeCase_5, accumulatedEvaluateInheritedAttributeCaseTime[5], accumulatedEvaluateInheritedAttributeCaseCalls[5] );
                        }
                        else if(class_definition) {

                                TimingPerformance::time_type startTimeCase_6;
                                TimingPerformance::startTimer(startTimeCase_6);

                                SgClassDefinition* class_def = isSgClassDefinition(n);
                                SgClassDeclaration* class_decl = class_def->get_declaration();
                                string class_name = class_decl->get_name().str();

                                #ifdef HIDDEN_LIST_DEBUG
                                        cout << "Class Definition: name = " << class_name << endl;
                                #endif

                                // DQ (5/31/2007): This implementation might not be good enough to handle "class X{}; void foo() { class X{}; }"

                                // Robert Preissl, July 03 2007 : ClassHashMap filled by 1st traversal (includes inheritance)
                                temp_symboltable = this->ClassHashMap[ class_name ];

                                TimingPerformance::accumulateTime ( startTimeCase_6, accumulatedEvaluateInheritedAttributeCaseTime[6], accumulatedEvaluateInheritedAttributeCaseCalls[6] );
                        }

                        TimingPerformance::accumulateTime ( startTimeCase_3, accumulatedEvaluateInheritedAttributeCaseTime[3], accumulatedEvaluateInheritedAttributeCaseCalls[3] );
                }

                // now, do the intersection:

                // Robert Preissl, June 1, 2007 : Splitting the intersection function into parts:
                //  *) Creating new stack frame
                //  **) Intersection

                CreateNewStackFrame(inheritedAttribute.pointer_VectorScopeStack, inheritedAttribute.depth);

                // Robert Preissl, June 8 2007 : ad "check if there are using-directives or using-declarations in the input code"
                //  Take a look into the future (or better said in the two sets of class VisitorTraversal) to see if there will be using decl/direct. in this
                //  SgScopeStatement.
                //  If Yes -> update the scope BEFORE the interesection starts, so changes take place and can be considered during the hiddenlist computation
                //  If No -> nothing

                SetSgUsingDirectiveStatementsWithSgScopeStatement::iterator it_UsingDirectivesSet;

// CH (4/7/2010) : Issue fixed as above
//#ifdef _MSCx_VER
                // Robert Preissl, June 26 2007: has to be a for-loop and can't use find because there can be more than one using in a scope
                for( it_UsingDirectivesSet = this->UsingDirectivesSet.begin(); it_UsingDirectivesSet != this->UsingDirectivesSet.end(); ++it_UsingDirectivesSet ) {

                        TimingPerformance::time_type startTimeCase_7;
                        TimingPerformance::startTimer(startTimeCase_7);

                        // compare the address of the current Scope with those from the UsingDirectivesSet
                        if( SgScope == (*it_UsingDirectivesSet).scope ) {

                                // Robert Preissl, July 3 2007 : Problems with global scope:
                                // Motivating example:
                                //        NS X { int x }
                                //        using NS X;
                                //        NS X { int y }
                                //        int main ..
                                // -> we haven't set the iterator of NS X yet, so how should we update the global scope??

                                // way out of the dilemma : take a look into the corresponding UsingDirRelativeToDeclarations_2 data structure
                                //        to find out how many NS X are before the using: then update, in this case with "int x"

                                if(global_scope == true) {

                                        int namespace_counter = 0;

                                        SetSgDeclarationStatements::iterator it_setSgDecl;
                                        SgNamespaceDeclarationStatement* namesp_decl_stat = (*it_UsingDirectivesSet).using_dir->get_namespaceDeclaration();
                                        // string used_namespace = namesp_decl_stat->get_qualified_name().str();
                                        string used_namespace = namesp_decl_stat->get_name().str();

                                        for( it_setSgDecl = this->UsingDirRelativeToDeclarations_2[(*it_UsingDirectivesSet).using_dir].begin(); it_setSgDecl != this->UsingDirRelativeToDeclarations_2[(*it_UsingDirectivesSet).using_dir].end(); ++it_setSgDecl ) {

                                                if( isSgNamespaceDeclarationStatement( *it_setSgDecl ) && ( used_namespace == GetSgNameOutOfSgDeclarationStatement( *it_setSgDecl ).str() ) ) {

                                                        namespace_counter++;

                                                }

                                        }

                                        //  -> so we don't update the scope itself, we update the symboltable of the scope which we use for the intersection
                                        UsingDirective_in_Scope((*it_UsingDirectivesSet).using_dir, temp_symboltable, inheritedAttribute.depth, can_be_named, namespace_counter);

                                        #ifdef HIDDEN_LIST_DEBUG
                                                cout << " UsingDirective in Scope: " << SgScope->get_qualified_name().str() << " in depth: " << inheritedAttribute.depth << endl;
                                        #endif

                                }

                                // Robert Preissl, July 3 2007 : no using will be executed in a namespace, because already collected by 1st traversal
                                //  and global scope treated above
                                if(!namespace_definition && !global_scope) {

                                        // Robert Preissl, June 25 2007, change parameter "vector<ScopeStackEntry>* p_VSS" to "Vector_Of_SymbolInformation temp_symboltable"
                                        //  -> so we don't update the scope itself, we update the symboltable of the scope which we use for the intersection
                                        UsingDirective_in_Scope((*it_UsingDirectivesSet).using_dir, temp_symboltable, inheritedAttribute.depth, can_be_named, -1);

                                        #ifdef HIDDEN_LIST_DEBUG
                                                cout << " UsingDirective in Scope: " << SgScope->get_qualified_name().str() << " in depth: " << inheritedAttribute.depth << endl;
                                        #endif

                                }

                        }

                        TimingPerformance::accumulateTime ( startTimeCase_7, accumulatedEvaluateInheritedAttributeCaseTime[7], accumulatedEvaluateInheritedAttributeCaseCalls[7] );
                }

//#else
//							// tps (12/7/2009) This is currently not defines since it fails in Release mode
//#pragma message ("WARNING: HiddenList: InheritedAttributeSgScopeStatement3: UsingDirectivesSet commented out right now.")
//#endif

                SetSgUsingDeclarationWithScopeWithSgScopeStatement::iterator it_UsingDeclarationsSet;

// CH (4/7/2010) : Problem fixed as above.
//#ifdef _MSCx_VER
                // Robert Preissl, June 28 2007: doing (nearly) the same as we have done above with using directives:
                for( it_UsingDeclarationsSet = this->UsingDeclarationsSet.begin(); it_UsingDeclarationsSet != this->UsingDeclarationsSet.end(); ++it_UsingDeclarationsSet ) {

                        TimingPerformance::time_type startTimeCase_8;
                        TimingPerformance::startTimer(startTimeCase_8);

                        // compare the address of the current Scope with those from the UsingDirectivesSet
                        if( SgScope == (*it_UsingDeclarationsSet).scope ) {

                                // not a using class decl. (will be treated in Namespac)
                                if(! (isSgClassDefinition(((*it_UsingDeclarationsSet).using_decl)->get_parent()) ) ) {

                                        // Robert Preissl, July 3 2007 : Problems with global scope:
                                        // Motivating example:
                                        //        NS X { int x }
                                        //        using X::x;
                                        //        NS X { int y }
                                        //        int main ..
                                        // -> we haven't set the iterator of NS X yet, so how should we update the global scope??

                                        // way out of the dilemma : take a look into the corresponding UsingDirRelativeToDeclarations_2 data structure
                                        //        to find out how many NS X are before the using: then update, in this case with "int x"

                                        string name, used_namespace, used_namespace_qualifiedname;

                                        if(global_scope == true) {

                                                TimingPerformance::time_type startTimeCase_12;
                                                TimingPerformance::startTimer(startTimeCase_12);

                                                int namespace_counter = 0;

                                                SetSgDeclarationStatements::iterator it_setSgDecl;

                                                SgDeclarationStatement* decl_stat = ((*it_UsingDeclarationsSet).using_decl)->get_declaration();

                                                ROSE_ASSERT(decl_stat != NULL);

                                                //!!! using declarations works now for variables, functions, typedefs, classes, enums
                                                if( isSgVariableDeclaration(decl_stat) || isSgFunctionDeclaration(decl_stat) || isSgTypedefDeclaration(decl_stat) || isSgClassDeclaration(decl_stat) || isSgEnumDeclaration(decl_stat) ) {

                                                        GetName_And_Scopequalifiedname_AndScopename_Of_SgDeclStat_In_UsingDecl(decl_stat, name, used_namespace_qualifiedname, used_namespace);

                                                        // cout << "used_namespace: " << used_namespace << endl;

                                                        for( it_setSgDecl = this->UsingDeclRelativeToDeclarations_2[(*it_UsingDeclarationsSet).using_decl].begin(); it_setSgDecl != this->UsingDeclRelativeToDeclarations_2[(*it_UsingDeclarationsSet).using_decl].end(); ++it_setSgDecl ) {

                                                                if( isSgNamespaceDeclarationStatement( *it_setSgDecl ) && ( used_namespace == GetSgNameOutOfSgDeclarationStatement( *it_setSgDecl ).str() ) ) {

                                                                        namespace_counter++;

                                                                }

                                                        }

                                                        //  -> so we don't update the scope itself, we update the symboltable of the scope which we use for the intersection
                                                        UsingDeclaration_in_Scope((*it_UsingDeclarationsSet).using_decl, temp_symboltable, inheritedAttribute.depth, can_be_named, namespace_counter, name, used_namespace_qualifiedname);

                                                        #ifdef HIDDEN_LIST_DEBUG
                                                                cout << " UsingDeclaration in Scope: " << SgScope->get_qualified_name().str() << " in depth: " << inheritedAttribute.depth << endl;
                                                        #endif

                                                }

                                                TimingPerformance::accumulateTime ( startTimeCase_12, accumulatedEvaluateInheritedAttributeCaseTime[12], accumulatedEvaluateInheritedAttributeCaseCalls[12] );
                                        }


                                        // Robert Preissl, July 3 2007 : no using will be executed in a namespace, because already collected by 1st traversal
                                        //  and global scope treated above
                                        if(!namespace_definition && !global_scope) {

                                                TimingPerformance::time_type startTimeCase_13;
                                                TimingPerformance::startTimer(startTimeCase_13);

                                                // Robert Preissl, June 25 2007, change parameter "vector<ScopeStackEntry>* p_VSS" to "Vector_Of_SymbolInformation temp_symboltable"
                                                //  -> so we don't update the scope itself, we update the symboltable of the scope which we use for the intersection
                                                // name, used_namespace_qualifiedname are just dummies her
                                                UsingDeclaration_in_Scope((*it_UsingDeclarationsSet).using_decl, temp_symboltable, inheritedAttribute.depth, can_be_named, -1, name, used_namespace_qualifiedname);
                                                #ifdef HIDDEN_LIST_DEBUG
                                                        cout << " UsingDeclaration in Scope: " << SgScope->get_qualified_name().str() << " in depth: " << inheritedAttribute.depth << endl;
                                                #endif

                                                TimingPerformance::accumulateTime ( startTimeCase_13, accumulatedEvaluateInheritedAttributeCaseTime[13], accumulatedEvaluateInheritedAttributeCaseCalls[13] );
                                        }

                                }

                        }

                        TimingPerformance::accumulateTime ( startTimeCase_8, accumulatedEvaluateInheritedAttributeCaseTime[8], accumulatedEvaluateInheritedAttributeCaseCalls[8] );
				}
//#else
//							// tps (12/7/2009) This is currently not defines since it fails in Release mode
//#pragma message ("WARNING: HiddenList: InheritedAttributeSgScopeStatement2: UsingDeclarationSet commented out right now.")
//#endif

                #ifdef HIDDEN_LIST_DEBUG
                        cout << "INTERSECTION of the current symbol-table: "<< endl;
                        VectorOutput(temp_symboltable);

                        if(inheritedAttribute.depth >= 0) {

                                cout << "with the current scope at depth: " << inheritedAttribute.depth << endl;

                                // Robert Preissl, July 9 2007 : ad performance tuning: the whole information isn't stored any longer in one entry
                                //         old code: HiddenListOutput((*inheritedAttribute.pointer_VectorScopeStack)[inheritedAttribute.depth]);

                                StackOfSymbolTableOutput( *inheritedAttribute.pointer_VectorScopeStack, inheritedAttribute.depth);

                        }
                #endif

                // start intersection process of the current symbol table with the current scope
#ifndef _MSC_VER
                CallIntersection(
                    inheritedAttribute.pointer_VectorScopeStack,
                    inheritedAttribute.depth,
                    temp_symboltable,
                    AvailableHidden_Functions_Variables,
                    AvailableHidden_Types,
                    ElaboratingForcingHidden_Types,
                    this->UsingDirRelativeToDeclarations,
                    this->UsingDeclRelativeToDeclarations,
                    this->UsingDirRelativeToDeclarations_2,
                    this->UsingDeclRelativeToDeclarations_2,
                    this->UsingDirectivesSet,
                    this->UsingDeclarationsSet
                );
#else
							// tps (12/7/2009) This is currently not defines since it fails in Release mode
#pragma message ("WARNING: HiddenList: InheritedAttributeSgScopeStatement4: UsingDirectivesSet commented out right now.")
 ROSE_ASSERT(false);

#endif

                if(!AvailableHidden_Functions_Variables.empty() || !AvailableHidden_Types.empty() || !ElaboratingForcingHidden_Types.empty()  ) {

                        TimingPerformance::time_type startTimeCase_9;
                        TimingPerformance::startTimer(startTimeCase_9);

                        #ifdef HIDDEN_LIST_DEBUG
                                cout << " >>> Hidden List of Scope: " << SgScope->get_qualified_name().str() << endl;
                        #endif

                        // Filling the three sets of hidden-symbols of SgScopeStatement
                        SgScope->set_type_elaboration_list(ElaboratingForcingHidden_Types);
                        SgScope->set_hidden_type_list(AvailableHidden_Types);
                        SgScope->set_hidden_declaration_list(AvailableHidden_Functions_Variables);

                        #ifdef HIDDEN_LIST_DEBUG
                                OutputSubsetsOfHiddenList(AvailableHidden_Functions_Variables, AvailableHidden_Types, ElaboratingForcingHidden_Types);
                                cout << " /////////////////////////////////////////////////////////////////////////////////// " << endl; cout <<  endl;
                        #endif
                        // testing:
                        //    OutputSubsetsOfHiddenList(SgScope->get_hidden_declaration_list(), SgScope->get_hidden_type_list(), SgScope->get_type_elaboration_list() );

                        TimingPerformance::accumulateTime ( startTimeCase_9, accumulatedEvaluateInheritedAttributeCaseTime[9], accumulatedEvaluateInheritedAttributeCaseCalls[9] );
                }


                #ifdef HIDDEN_LIST_DEBUG
                        cout << "XY: " << this->xy << endl;
                #endif
                this->xy++;

                // if Member-Function-Declaration: Update current scope with the symbol from the class
                //  Motivating example: class A { public: int x; void foo(); } void A::foo() { int x; // hides x from class }
                //  -> in AST: SgGlobal -> SgMemberFunctionDeclaration -> SgFunctionDefinition -> SgBasicBlock (here should be the update used)
                //  !! -> make update AFTER new stack frame is opened (which is done above at the intersection procedure) and do this in the
                //        function definition statement (only if we had a previous MemberFunctionDecl.); now changes take place in BasicBlock

                if(isSgFunctionDefinition(n)) {

                        TimingPerformance::time_type startTimeCase_10;
                        TimingPerformance::startTimer(startTimeCase_10);

                        SgFunctionDefinition* func_def = isSgFunctionDefinition(n);

                        #ifdef HIDDEN_LIST_DEBUG
                                cout << endl;cout << endl;cout << endl;
                                cout << " Function Definition" << endl;
                        #endif

                        //if(member_func_decl != NULL) {
                        if(isSgMemberFunctionDeclaration(func_def->get_parent())) {

                                SgMemberFunctionDeclaration*  member_func_decl = isSgMemberFunctionDeclaration(func_def->get_parent());

                                #ifdef HIDDEN_LIST_DEBUG
                                        cout << " !!!!!!!!!!!  Function Definition before MemberFuncDecl" << endl;
                                        cout << "Size of pointer_VectorScopeStack: " << (*inheritedAttribute.pointer_VectorScopeStack).size() << endl;
                                        cout << "depth: " << inheritedAttribute.depth << endl;

                                        cout << endl;
                                #endif

                                // only make update if the definition is outside the class def. (inside: would have the right symbols)
                                if(isSgGlobal(member_func_decl->get_parent())) {
                                        #ifdef HIDDEN_LIST_DEBUG
                                                cout << "Update of VectorScopeStack with symbols of Class: ";
                                        #endif
                                        SgClassDefinition* class_def = member_func_decl->get_class_scope();
                                        SgClassDeclaration* class_decl = class_def->get_declaration();
                                        string class_name = class_decl->get_name().str();
                                        #ifdef HIDDEN_LIST_DEBUG
                                                cout << class_name << endl;
                                        #endif

                                        UpdateScope_WithClass(inheritedAttribute.pointer_VectorScopeStack, inheritedAttribute.depth, this->ClassHashMap[ class_name ]);

                                }

                        }

                        TimingPerformance::accumulateTime ( startTimeCase_10, accumulatedEvaluateInheritedAttributeCaseTime[10], accumulatedEvaluateInheritedAttributeCaseCalls[10] );
                }


                #ifdef HIDDEN_LIST_DEBUG
                        cout << "**************** SCOPE END *****************************************************"<<endl;
                        cout << endl;
                #endif

             // DQ (8/3/2007): accumulate the elapsed time for this function
                TimingPerformance::accumulateTime ( startTimeCase_2, accumulatedEvaluateInheritedAttributeCaseTime[2], accumulatedEvaluateInheritedAttributeCaseCalls[2] );

                return InheritedAttributeSgScopeStatement(inheritedAttribute.depth + 1, inheritedAttribute.pointer_VectorScopeStack);
		}

     // DQ (8/3/2007): accumulate the elapsed time for this function
        TimingPerformance::accumulateTime ( startTime, accumulatedEvaluateInheritedAttribute_2_Time, accumulatedEvaluateInheritedAttribute_2_Calls );

        return InheritedAttributeSgScopeStatement(inheritedAttribute.depth, inheritedAttribute.pointer_VectorScopeStack);

}

//  -> will be used to pop the stack of Scopes
SynthesizedAttributeSgScopeStatement HiddenListComputationTraversal :: evaluateSynthesizedAttribute(SgNode* n, InheritedAttributeSgScopeStatement inheritedAttribute, SynthesizedAttributesList SynthesizedAttributeSgScopeStatementList) {

     // DQ (8/3/2007): Start time for this function
        TimingPerformance::time_type startTime;
        TimingPerformance::startTimer(startTime);

        // if scope statement reached -> pop stack of scopes
        SgScopeStatement* SgScope = isSgScopeStatement(n);
             if (SgScope != NULL) {

                #ifdef HIDDEN_LIST_DEBUG
                        cout << "SynthesizedAttribute -> pop stack" << endl;
                #endif
                //hidden_list_debug << "S (before pop) with stacksize: " << (*inheritedAttribute.pointer_VectorScopeStack).size() << endl;
                (*inheritedAttribute.pointer_VectorScopeStack).pop_back();

        }

        /*// returned objects are not important for the computation of the hiddenlist
        if (SynthesizedAttributeSgScopeStatementList.size() == 0) {

                SynthesizedAttributeSgScopeStatement test;
                return test;

        }
        else {

                return SynthesizedAttributeSgScopeStatementList[0];

        }*/

     // DQ (8/3/2007): accumulate the elapsed time for this function
        TimingPerformance::accumulateTime ( startTime, accumulatedEvaluateSynthesizedAttribute_2_Time, accumulatedEvaluateSynthesizedAttribute_2_Calls );

        return 42;

}

bool Find_SetSgDeclarationStatements_In_StackSetSgDeclarationStatements(SetSgDeclarationStatements& find_decl_set, StackSetSgDeclarationStatements& stack, int depth, int depth_of_using) {

        SetSgDeclarationStatements::iterator set_it;

        SetSgDeclarationStatements result_set;
        int size_of_stack = (int)stack.size();

        cout << "stack in Find_SetSgDeclarationStatements_In_StackSetSgDeclarationStatements: " << endl;
        StackSetSgDeclarationStatements__Output(stack);

        cout << " intersect with SetSgDeclarationStatements: " << endl;
        OutputSetSgDeclarationStatements(find_decl_set);

        for(int i = 0; ( (i < size_of_stack) /*&& (i <= depth) */); ++i ) {

             // set_intersection( find_decl_set.begin(), find_decl_set.end(), stack[i].begin(),  stack[i].end(), inserter(result_set, result_set.begin()), eqstr_SgDeclarationStatement() );
                set_intersection( find_decl_set.begin(), find_decl_set.end(), stack[i].begin(),  stack[i].end(), inserter(result_set, result_set.begin()) );

                // Robert Preissl: Performance Tuning : don't use size()
                //        result_size = result_set.size();

                // cout << "RESULT_SET size: " << result_size << endl;

                //        result_it = result_set.begin();
                //        while(result_it != result_set.end() ) {
                //                result_it++;
                //        }

                if( !result_set.empty() ) {

                        // cout << ">>>>> result_size > 0 // depth of using: " << depth_of_using << " AND depth of hiding symbol: " << depth << endl;

                        return true;

                }

        }

        return false;

}

// Robert Preissl, June 22 2007 : return true if decl_stmt (or one of its parent-decl_stmt) occur in the UsingDirRelativeToDeclarations HashMap
//  need depth, because I don't have to look into the stack deeper as "depth" (means that we have a using in a scope deeper nested as the symbol
//  that is hiding)
bool FindDeclarationInUsingDirRelativeToDeclarations(SgDeclarationStatement* decl_stmt, int depth, UsingDirectiveStatement_LinkedListStackSetSgDeclarationStatements_HashMap& UsingDirRelativeToDeclarations, UsingDeclarationStatement_LinkedListStackSetSgDeclarationStatements_HashMap& UsingDeclRelativeToDeclarations, SgUsingDirectiveStatement* using_dir, int depth_of_using) {

        bool found = false;
        bool is_using_dir = false;
        SgDeclarationStatement* temp_PreviousLinkedListStackSetSgDeclarationStatements;
        SgUsingDirectiveStatement* using_dir_stmt = 0;
        SgUsingDeclarationStatement* using_decl_stmt = 0;

        // at first create a set of set of SgDeclarationStatements that contains the decl_stmt with its parent decl_stmt we are searching for:
        SetSgDeclarationStatements find_decl_set;
        SgNode* temp_node = decl_stmt;

        // at least the set contains the SgDeclarationStatement decl_stmt
        find_decl_set.insert(decl_stmt);
        // find_decl_set.push_back(decl_stmt);

        while( !(isSgGlobal(temp_node) != NULL) ) {

                temp_node = temp_node->get_parent();
                ROSE_ASSERT(temp_node != NULL);

                if( isSgDeclarationStatement(temp_node) ) {

                        find_decl_set.insert((SgDeclarationStatement*)temp_node);
                     // find_decl_set.push_back((SgDeclarationStatement*)temp_node);

                }

        }

        // now see if one of these decl. occurs in the UsingDirRelativeToDeclarations hashmap..

        found = Find_SetSgDeclarationStatements_In_StackSetSgDeclarationStatements( find_decl_set, UsingDirRelativeToDeclarations[using_dir].CurrentStackSetSgDeclarationStatements, depth, depth_of_using );

        if(found == true) {

                return true;

        }
        // if there are more than one using in the code:
        else if( !found && UsingDirRelativeToDeclarations[using_dir].PreviousLinkedListStackSetSgDeclarationStatements != NULL ) {

                temp_PreviousLinkedListStackSetSgDeclarationStatements = UsingDirRelativeToDeclarations[using_dir].PreviousLinkedListStackSetSgDeclarationStatements;

                // I could also do this recursively, but I think its faster to do not
                while( temp_PreviousLinkedListStackSetSgDeclarationStatements != NULL ) {

                        if( isSgUsingDirectiveStatement(temp_PreviousLinkedListStackSetSgDeclarationStatements) ) {

                                using_dir_stmt = isSgUsingDirectiveStatement(temp_PreviousLinkedListStackSetSgDeclarationStatements);
                                is_using_dir = true;

                        }
                        else if( isSgUsingDeclarationStatement(temp_PreviousLinkedListStackSetSgDeclarationStatements) ) {

                                using_decl_stmt = isSgUsingDeclarationStatement(temp_PreviousLinkedListStackSetSgDeclarationStatements);
                                is_using_dir = false;

                        }
                        else { cout << "ERROR in  FindDeclarationInUsingDirRelativeToDeclarations" << endl; }

                        if(is_using_dir) {

                                found = Find_SetSgDeclarationStatements_In_StackSetSgDeclarationStatements( find_decl_set, UsingDirRelativeToDeclarations[using_dir_stmt].CurrentStackSetSgDeclarationStatements, depth, depth_of_using );

                                if(found) {

                                        return true;

                                }

                        }
                        //else {

                        //        StackSetSgDeclarationStatements__Output( X[temp_PreviousLinkedListStackSetSgDeclarationStatements].CurrentStackSetSgDeclarationStatements, using_decl_stmt->get_namespaceDeclaration() );

                        //}

                        temp_PreviousLinkedListStackSetSgDeclarationStatements = UsingDirRelativeToDeclarations[using_dir_stmt].PreviousLinkedListStackSetSgDeclarationStatements;

                }

        }

        return false;

}

// Robert Preissl, May, 10 2007 : added this function to divide the hiddenlist into subsets
void GetSubsetsOfHiddenList(ScopeStackEntry hiddenlist, SetSgSymbolPointers &AvailableHidden_Functions_Variables, SetSgSymbolPointers &AvailableHidden_Types, SetSgSymbolPointers &ElaboratingForcingHidden_Types) {

        ScopeStackEntry::iterator it;
        SymbolHashMap::iterator value_it;
        SymbolHashMap temp_symbol_hashmap;

        for(it = hiddenlist.begin(); it != hiddenlist.end(); ++it) {

                temp_symbol_hashmap = it->second;

                for(value_it = temp_symbol_hashmap.begin(); value_it != temp_symbol_hashmap.end(); ++value_it) {

                        ROSE_ASSERT(value_it->first != NULL);

                        // filter out the unavailable entries of hiddenlist
                        if( (value_it->second)->scope_can_be_named ) {

                                // ElaboratingForcing HiddenList entries:
                                if(isSgClassSymbol(value_it->first)) {
                                        //cout << "ElaboratingForcing HiddenList entry, Class/Struct: " << it->first << endl;
                                        ElaboratingForcingHidden_Types.insert(value_it->first);
                                }
                                else if(isSgEnumSymbol(value_it->first)) {
                                        //cout << "ElaboratingForcing HiddenList entry, Enum: " << it->first << endl;
                                        ElaboratingForcingHidden_Types.insert(value_it->first);
                                }
                                // ???? not quite sure if EnumField can be elaborated
                                else if(isSgEnumFieldSymbol(value_it->first)) {
                                        //cout << "ElaboratingForcing HiddenList entry, EnumField: " << it->first << endl;
                                        ElaboratingForcingHidden_Types.insert(value_it->first);
                                }

                                // available HiddenList entries:
                                else if(isSgTypedefSymbol(value_it->first)) {
                                        //cout << "available HiddenList entry, typedef: " << it->first << endl;
                                        AvailableHidden_Types.insert(value_it->first);
                                }

                                // availableHidden Functions or Variables
                                else if(isSgVariableSymbol(value_it->first)) {
                                        //cout << "available HiddenList entry, Variable: " << it->first << endl;
                                        AvailableHidden_Functions_Variables.insert(value_it->first);
                                }
                                else if(isSgFunctionSymbol(value_it->first))  {
                                        //cout << "available HiddenList entry, Function: " << it->first << endl;
                                        AvailableHidden_Functions_Variables.insert(value_it->first);
                                }

                                else if(isSgNamespaceSymbol(value_it->first)) {
                                        //cout << "available HiddenList entry, Namespace: " << it->first << endl;
                                        AvailableHidden_Functions_Variables.insert(value_it->first);
                                }

                                // Robert Preissl, May 29 2007, ad templates
                                else if(isSgTemplateSymbol(value_it->first)) {

                                        SgTemplateSymbol* template_symbol = isSgTemplateSymbol(value_it->first);
                                        if(template_symbol != NULL) {

                                                SgTemplateDeclaration* template_decl = template_symbol->get_declaration();

                                                #ifdef HIDDEN_LIST_DEBUG
                                                        cout << "template_kind: "  << template_decl->get_template_kind() << endl;
                                                #endif

                                                // class or nested class template
                                                if(template_decl->get_template_kind() == 1 || template_decl->get_template_kind() == 2) {

                                                        ElaboratingForcingHidden_Types.insert(value_it->first);

                                                }
                                                // function or member function template
                                                else if(template_decl->get_template_kind() == 3 || template_decl->get_template_kind() == 4 ) {

                                                        AvailableHidden_Functions_Variables.insert(value_it->first);

                                                }

                                        }

                                }

                        }
                        else {
                                //cout << "Unaccessible entry " << it->first << "; with SgSymbol-ClassName " << ((*l_it).symbol_pointer)->class_name() << " with SgSymbol-Address: " << (*l_it).symbol_pointer << endl;

                        }
                }
        }

}


// Robert Preissl, May, 17 2007 : added this function for updating the subsets of the hiddenlist; will be called in SgFunctionDeclaration
void UpdateSubsetOfHiddenList(SetSgSymbolPointers &SubsetOfHiddenList, SetSgSymbolPointers UpdatingSet) {

        SetSgSymbolPointers::iterator value_it;

        for(value_it = UpdatingSet.begin(); value_it != UpdatingSet.end(); value_it++) {

                SubsetOfHiddenList.insert(*value_it);

        }

}


// Robert Preissl, May 25 2007 : works for Variables, Functions, Typedefs, Classes, Enums,
SgName GetSgNameOutOfSgDeclarationStatement(SgDeclarationStatement* decl_stat) {

        SgName name = "";

        // variables
        if(isSgVariableDeclaration(decl_stat))
        {
                SgVariableDeclaration* var_decl = isSgVariableDeclaration(decl_stat);
#if 0
             // DQ (9/9/2007): Added assertion to test inlining example in tutorial
                if (var_decl->get_definition() == NULL)
                   {
                     printf ("Error in hidden list computation GetSgNameOutOfSgDeclarationStatement(): var_decl->get_definition() == NULL \n");
                     var_decl->get_startOfConstruct()->display("Error: var_decl->get_definition() == NULL (debug)");
                     ROSE_ASSERT(var_decl->get_variables().empty() == false);
                     printf ("variable name = %s \n",var_decl->get_variables().front()->get_name().str());
                     ROSE_ASSERT(var_decl->get_variables().front()->get_definition() != NULL);
                     printf ("var_decl->get_variables().front()->get_definition() = %s \n",var_decl->get_variables().front()->get_definition()->class_name().c_str());
                   }
                ROSE_ASSERT(var_decl->get_definition() != NULL);
#endif
             // DQ (9/9/2007): To get the name we don't have to do through the definition (and we should not).
             // SgInitializedName* initialized_name_variable = var_decl->get_definition()->get_vardefn();
                ROSE_ASSERT(var_decl->get_variables().empty() == false);
                SgInitializedName* initialized_name_variable = var_decl->get_variables().front();

                if(initialized_name_variable != NULL)
                {

                        name = initialized_name_variable->get_name();
                        //         cout << "InitializedName 1: " << name << endl;

                }

        }
        // functions
        else if(isSgFunctionDeclaration(decl_stat)) {

                SgFunctionDeclaration* func_decl = isSgFunctionDeclaration(decl_stat);
                name = func_decl->get_name();

                //        cout << "isSgFunctionDeclaration: " << name << endl;

        }
        // typedefs
        else if(isSgTypedefDeclaration(decl_stat)) {

                SgTypedefDeclaration* type_decl = isSgTypedefDeclaration(decl_stat);
                name = type_decl->get_name();

                //        cout << "isSgTypedefDeclaration: " << name << endl;

        }
        // classes
        else if(isSgClassDeclaration(decl_stat)) {

                SgClassDeclaration* class_decl = isSgClassDeclaration(decl_stat);
                name = class_decl->get_name();

                //        cout << "isSgClassDeclaration: " << name << endl;

        }
        // enums
        else if(isSgEnumDeclaration(decl_stat)) {

                SgEnumDeclaration* enum_decl = isSgEnumDeclaration(decl_stat);
                name = enum_decl->get_name();

                //        cout << "isSgEnumDeclaration: " << name << endl;

        }
        // namespaces
        else if(isSgNamespaceDeclarationStatement(decl_stat)) {

                SgNamespaceDeclarationStatement* namespace_decl = isSgNamespaceDeclarationStatement(decl_stat);
                name = namespace_decl->get_name();

                //        cout << "isSgNamespaceDeclarationStatement: " << name << endl;

        }
        // templates
        else if(isSgTemplateDeclaration(decl_stat)) {

                SgTemplateDeclaration* template_decl = isSgTemplateDeclaration(decl_stat);
                name = template_decl->get_name();

                //        cout << "isSgTemplateDeclaration: " << name << endl;

        }
        // using dir
        else if(isSgUsingDirectiveStatement(decl_stat)) {

                SgUsingDirectiveStatement* uds = isSgUsingDirectiveStatement(decl_stat);
                SgNamespaceDeclarationStatement* namesp_decl_stat = uds->get_namespaceDeclaration();
                string used_namespace = namesp_decl_stat->get_qualified_name().str();

                name = "using_decl: "+used_namespace;

                //        cout << "isSgUsingDirectiveStatement: " << name << endl;

        }
        // using decl
        else if(isSgUsingDeclarationStatement(decl_stat)) {

                SgUsingDeclarationStatement* uds = isSgUsingDeclarationStatement(decl_stat);

                name = "mangled name of using_decl: " + string(((uds->get_mangled_name()).str()));

                //        cout << "isSgUsingDeclarationStatement: " << name << endl;

        }
        else {

                cout << "ERROR: " <<decl_stat->sage_class_name() << endl;

        }

        return name;

}

// Robert Preissl, May 25 2007 : works for Variables, Functions, Typedefs, Classes, Enums, EnumFields
SgDeclarationStatement* GetSgDeclarationStatementOutOfSgSymbol(SgSymbol* p_symbol) {

        SgDeclarationStatement* decl_stat = NULL;

        if(isSgFunctionSymbol(p_symbol)) {
                // cout << " isSgFunctionSymbol " << endl;
                SgFunctionSymbol* func_symbol = isSgFunctionSymbol(p_symbol);
                if(func_symbol != NULL) {
                        decl_stat = (SgDeclarationStatement*)func_symbol->get_declaration();
                }
        }
        else if(isSgVariableSymbol(p_symbol)) {
                // cout << " isSgVariableSymbol: " << p_symbol->get_name().str() << " / " << p_symbol->class_name() << endl;
                SgVariableSymbol* var_symbol = isSgVariableSymbol(p_symbol);
                if(var_symbol != NULL) {
                        SgInitializedName* init_name = var_symbol->get_declaration();
                        if(init_name != NULL) {
                                // decl_stat = init_name->get_declaration();
                                SgVariableDeclaration* var_decl = (isSgVariableDeclaration(init_name->get_parent()));
                                if(var_decl != NULL) {
                                        // cout << " var_decl NOT NULL" << endl;
                                        decl_stat  = (SgDeclarationStatement*)var_decl;
                                }
                                else {
                                        //Sg_File_Info* fi = (init_name->get_parent())->get_file_info();
                                        //cout << "KRANK!!: " << fi->get_filename() << "/ " << fi->get_line() << endl;
                                        decl_stat = NULL;
                                }
                        }
                        else {
                                  // cout << " -> init_name = NULL " << endl;
                        }
                }
                else {
                          // cout << " -> var_symbol = NULL " << endl;
                }
        }
        else if(isSgTypedefSymbol(p_symbol)) {
                // cout << " isSgTypedefSymbol " << endl;
                SgTypedefSymbol* type_symbol = isSgTypedefSymbol(p_symbol);
                if(type_symbol != NULL) {
                        decl_stat = (SgDeclarationStatement*)type_symbol->get_declaration();
                }
        }
        else if(isSgClassSymbol(p_symbol)) {
                // cout << " isSgClassSymbol " << endl;
                SgClassSymbol* class_symbol = isSgClassSymbol(p_symbol);
                if(class_symbol != NULL) {
                        decl_stat = (SgDeclarationStatement*)class_symbol->get_declaration();
                }
        }
        else if(isSgEnumSymbol(p_symbol)) {
                // cout << " isSgEnumSymbol " << endl;
                SgEnumSymbol* enum_symbol = isSgEnumSymbol(p_symbol);
                if(enum_symbol != NULL) {
                        decl_stat = (SgDeclarationStatement*)enum_symbol->get_declaration();
                }
        }
        else if(isSgEnumFieldSymbol(p_symbol)) {
                // cout << " isSgEnumFieldSymbol " << endl;
                SgEnumFieldSymbol* enumfield_symbol = isSgEnumFieldSymbol(p_symbol);
                if(enumfield_symbol != NULL) {
                        SgInitializedName* init_name = enumfield_symbol->get_declaration();
                        if(init_name != NULL) {
                                decl_stat = init_name->get_declaration();
                        }
                }
        }
        else if(isSgNamespaceSymbol(p_symbol)) {
                // cout << " isSgNamespaceSymbol " << endl;
                SgNamespaceSymbol* namespace_symbol = isSgNamespaceSymbol(p_symbol);
                if(namespace_symbol != NULL) {
                        decl_stat = (SgDeclarationStatement*)namespace_symbol->get_declaration();
                }
        }
     // DQ (4/14/2010): Added support for Alias symbols
        else if(isSgAliasSymbol(p_symbol)) {
                // cout << " isSgAliasSymbol " << endl;
                SgAliasSymbol* alias_symbol = isSgAliasSymbol(p_symbol);
                if(alias_symbol != NULL) {
                        decl_stat = (SgDeclarationStatement*)alias_symbol->get_declaration();
                }
        }
        else if(isSgTemplateSymbol(p_symbol)) {
                // cout << " isSgTemplateSymbol " << endl;
                SgTemplateSymbol* template_symbol = isSgTemplateSymbol(p_symbol);
                if(template_symbol != NULL ) {
                        SgTemplateDeclaration* template_decl = template_symbol->get_declaration();
                        if(template_decl != NULL) {
                                decl_stat = (SgDeclarationStatement*)template_decl;
                        }
                        else {
                                // cout << " -> template_decl = NULL " << endl;
                        }
                        // cout << " -> isSgTemplateSymbol: " << endl;
                }
                else {
                        // cout << " -> template_symbol = NULL " << endl;
                }
        }
        else {
                std::cout << ">>> ERROR : " << p_symbol->get_name().str() << " / " << p_symbol->class_name() << std::endl;
        }

        return decl_stat;
}

void CreateNewStackFrame(std::vector<ScopeStackEntry>* p_VSHM, int depth) {


        TimingPerformance::time_type startTimeCreateNewStackFrame;
        TimingPerformance::startTimer(startTimeCreateNewStackFrame);
                
        // for every scope make new stack frame
        if(depth >= 0 ) {
                ScopeStackEntry hidden_list;

                (*p_VSHM).push_back(hidden_list);
        }

        /*
        else {

                //  and copy entries of previous frame into new one
                (*p_VSHM).push_back((*p_VSHM)[depth-1]);
        }
        */

        TimingPerformance::accumulateTime ( startTimeCreateNewStackFrame, accumulatedCreateNewStackFrameTime, accumulatedCreateNewStackFrameCalls);
}


void CallIntersection_F(std::vector<ScopeStackEntry>* p_VSHM, int depth, SgFunctionDeclaration* f_decl, SetSgSymbolPointers &AvailableHidden_Functions_Variables, SetSgSymbolPointers &AvailableHidden_Types, SetSgSymbolPointers &ElaboratingForcingHidden_Types, UsingDirectiveStatement_LinkedListStackSetSgDeclarationStatements_HashMap& UsingDirRelativeToDeclarations, UsingDeclarationStatement_LinkedListStackSetSgDeclarationStatements_HashMap& UsingDeclRelativeToDeclarations, UsingDirectiveStatement_SetSgDeclarationStatements_HashMap& UsingDirRelativeToDeclarations_2, UsingDeclarationStatement_SetSgDeclarationStatements_HashMap& UsingDeclRelativeToDeclarations_2, SetSgUsingDirectiveStatementsWithSgScopeStatement& UsingDirectivesSet, SetSgUsingDeclarationWithScopeWithSgScopeStatement& UsingDeclarationsSet) {

        Intersection_with_FunctionDeclaration(p_VSHM, depth, f_decl, AvailableHidden_Functions_Variables, AvailableHidden_Types, ElaboratingForcingHidden_Types, UsingDirRelativeToDeclarations, UsingDeclRelativeToDeclarations, UsingDirRelativeToDeclarations_2, UsingDeclRelativeToDeclarations_2, UsingDirectivesSet, UsingDeclarationsSet);


}


void CallIntersection(std::vector<ScopeStackEntry>* p_VSHM, int depth, Vector_Of_SymbolInformation &X, SetSgSymbolPointers &AvailableHidden_Functions_Variables, SetSgSymbolPointers &AvailableHidden_Types, SetSgSymbolPointers &ElaboratingForcingHidden_Types, UsingDirectiveStatement_LinkedListStackSetSgDeclarationStatements_HashMap& UsingDirRelativeToDeclarations, UsingDeclarationStatement_LinkedListStackSetSgDeclarationStatements_HashMap& UsingDeclRelativeToDeclarations, UsingDirectiveStatement_SetSgDeclarationStatements_HashMap& UsingDirRelativeToDeclarations_2, UsingDeclarationStatement_SetSgDeclarationStatements_HashMap& UsingDeclRelativeToDeclarations_2, SetSgUsingDirectiveStatementsWithSgScopeStatement& UsingDirectivesSet, SetSgUsingDeclarationWithScopeWithSgScopeStatement& UsingDeclarationsSet) {

        Intersection(p_VSHM, depth, X, AvailableHidden_Functions_Variables, AvailableHidden_Types, ElaboratingForcingHidden_Types, UsingDirRelativeToDeclarations, UsingDeclRelativeToDeclarations, UsingDirRelativeToDeclarations_2, UsingDeclRelativeToDeclarations_2, UsingDirectivesSet, UsingDeclarationsSet);

}

/*
void HiddenListComputationTraversal::CalculateIfIsSameFile(SgNode* n, Sg_File_Info* sg_file_info) {

        if(sg_file_info != NULL) {

                if( sg_file_info->isSameFile(this->sg_file_pointer) ) {

                        this->do_intersection = true;
                        #ifdef HIDDEN_LIST_DEBUG
                                cout << " this->do_intersection = true " << endl;
                        #endif

                }
                else {

                        // Robert Preissl, June 4 2007, Motivating example:
                        //         if(x == 5)
                        //                if(x < 6)
                        //                        y = x;
                        // -> compiler adds basic block
                        // That sucks because this block is now "not in the same file" because it was compiler generated and this->do_intersection would be false!!

                        if(isSgBasicBlock(n)) {

                                SgBasicBlock* basic_block = isSgBasicBlock(n);

                                ROSE_ASSERT(basic_block->get_parent() != NULL);

                                // cases where a basic block can be added: Under a..
                                // (or in other words, cases where a "{", "}" can be neglected to write valid c++ code
                                if( isSgIfStmt(basic_block->get_parent() ) ) {

                                        Sg_File_Info* sg_file_info = isSgIfStmt(basic_block->get_parent())->get_file_info();

                                        // this means that the parent of the basic-block (here an if-stmt) is also (also, because we are in the else part) not in the same file
                                        if( !sg_file_info->isSameFile(this->sg_file_pointer) ) {
                                                this->do_intersection = false;
                                                #ifdef HIDDEN_LIST_DEBUG
                                                        cout << "isSgIfStmt this->do_intersection = false " << endl;
                                                #endif
                                        }
                                        else {
                                                this->do_intersection = true;
                                                #ifdef HIDDEN_LIST_DEBUG
                                                        cout << "isSgIfStmt this->do_intersection = true " << endl;
                                                #endif
                                        }

                                }
                                else if(isSgForStatement(basic_block->get_parent() ) ) {

                                        Sg_File_Info* sg_file_info = isSgForStatement(basic_block->get_parent())->get_file_info();

                                        // this means that the parent of the basic-block (here a for-stmt) is also (also, because we are in the else part) not in the same file
                                        if( !sg_file_info->isSameFile(this->sg_file_pointer) ) {
                                                this->do_intersection = false;
                                                #ifdef HIDDEN_LIST_DEBUG
                                                        cout << "isSgForStatement this->do_intersection = false " << endl;
                                                #endif
                                        }
                                        else {
                                                this->do_intersection = true;
                                                #ifdef HIDDEN_LIST_DEBUG
                                                        cout << "isSgForStatement this->do_intersection = true " << endl;
                                                #endif
                                        }

                                }
                                else if(isSgWhileStmt(basic_block->get_parent() ) ) {

                                        Sg_File_Info* sg_file_info = isSgWhileStmt(basic_block->get_parent())->get_file_info();

                                        // this means that the parent of the basic-block (here a While-stmt) is also (also, because we are in the else part) not in the same file
                                        if( !sg_file_info->isSameFile(this->sg_file_pointer) ) {
                                                this->do_intersection = false;
                                                #ifdef HIDDEN_LIST_DEBUG
                                                        cout << "isSgWhileStmt this->do_intersection = false " << endl;
                                                #endif
                                        }
                                        else {
                                                this->do_intersection = true;
                                                #ifdef HIDDEN_LIST_DEBUG
                                                        cout << "isSgWhileStmt this->do_intersection = true " << endl;
                                                #endif
                                        }

                                }
                                else if(isSgDoWhileStmt(basic_block->get_parent() ) ) {

                                        Sg_File_Info* sg_file_info = isSgDoWhileStmt(basic_block->get_parent())->get_file_info();

                                        // this means that the parent of the basic-block (here a DoWhile-stmt) is also (also, because we are in the else part) not in the same file
                                        if( !sg_file_info->isSameFile(this->sg_file_pointer) ) {
                                                this->do_intersection = false;
                                                #ifdef HIDDEN_LIST_DEBUG
                                                        cout << "isSgDoWhileStmt this->do_intersection = false " << endl;
                                                #endif
                                        }
                                        else {
                                                this->do_intersection = true;
                                                #ifdef HIDDEN_LIST_DEBUG
                                                        cout << "isSgDoWhileStmt this->do_intersection = true " << endl;
                                                #endif
                                        }

                                }
                                else if(isSgSwitchStatement(basic_block->get_parent() ) ) {

                                        Sg_File_Info* sg_file_info = isSgSwitchStatement(basic_block->get_parent())->get_file_info();

                                        // this means that the parent of the basic-block (here a isSgSwitchStatement-stmt) is also (also, because we are in the else part) not in the same file
                                        if( !sg_file_info->isSameFile(this->sg_file_pointer) ) {
                                                this->do_intersection = false;
                                                #ifdef HIDDEN_LIST_DEBUG
                                                        cout << "isSgSwitchStatement this->do_intersection = false " << endl;
                                                #endif
                                        }
                                        else {
                                                this->do_intersection = true;
                                                #ifdef HIDDEN_LIST_DEBUG
                                                        cout << "isSgSwitchStatement this->do_intersection = true " << endl;
                                                #endif
                                        }

                                }
                                else {

                                        this->do_intersection = false;
                                        #ifdef HIDDEN_LIST_DEBUG
                                                cout << " this->do_intersection = false " << endl;
                                        #endif
                                }

                        }
                        else {

                                this->do_intersection = false;
                                #ifdef HIDDEN_LIST_DEBUG
                                        cout << " this->do_intersection = false " << endl;
                                #endif

                        }

                }

        }
        else {

                cout << "ERROR!! >> sg_file_info is NULL" << endl;

        }
}
*/


// Robert Preissl, June 7 2007: For Comparing File Infos of SgSymbol*; learn more about it in HiddenList_Intersection.C where it is called!
//  functions returns 0 if it couldn't compare the file-info -> no insert into hidden-list
//  functions returns 1 for p_symbol_pointer
//  functions returns 2 for it_symbol_pointer
int CompareFileInfoOfSgSymbols(SgSymbol* p_symbol_pointer, SgSymbol* it_symbol_pointer) {

        // ok, get the declaration of these symbols and then compare the file-info:
        SgDeclarationStatement* decl_stat_p_symbol_pointer = NULL;
        SgDeclarationStatement* decl_stat_it_symbol_pointer = NULL;

        // as we can find out in HiddenList_Intersection.C p_symbol_pointer should be a class or an enum..
        decl_stat_p_symbol_pointer = GetSgDeclarationStatementOutOfSgSymbol(p_symbol_pointer);
        decl_stat_it_symbol_pointer = GetSgDeclarationStatementOutOfSgSymbol(it_symbol_pointer);

        if(decl_stat_p_symbol_pointer != NULL && decl_stat_it_symbol_pointer != NULL) {

                Sg_File_Info* p_file_info;
                Sg_File_Info* it_file_info;

                p_file_info = decl_stat_p_symbol_pointer->get_file_info();
                it_file_info = decl_stat_it_symbol_pointer->get_file_info();

                if(p_file_info != NULL && it_file_info != NULL) {

                        // Robert Preissl, July 5 2007 : what happens if the files are not the same -> < would return false in any way!
                        // BUT: don't care because of comment of Robert Preissl, June 15 2007 in intersection-procedure
                        //  example : A.h : class A
                        //            A.cpp : int A

                        if(p_file_info < it_file_info) {

                                return 1;

                        }
                        else {

                                return 2;

                        }

                }

                else {

                        cout << "In hiddenlist computation: ERROR in CompareFileInfoOfSgSymbols!!! At least one of symbols has no file info." << endl;
                        return 0;

                }


        }
        else {

                cout << "In hiddenlist computation: ERROR in CompareFileInfoOfSgSymbols!!! At least one of symbols has no corresponding declaration. " << endl;
                return 0;

        }


}

// Robert Preissl, June 15 2007: For Comparing File Names of SgSymbol*; learn more about it in HiddenList_Intersection.C where it is called!
//  functions returns 0 if it couldn't compare the file-info
//  functions returns 1 for equal file names
//  functions returns 2 for different file names
int CompareFileNamesOfSgSymbols(SgSymbol* p_symbol_pointer, SgSymbol* it_symbol_pointer) {

  // ok, get the declaration of these symbols and then compare the file-info:
  SgDeclarationStatement* decl_stat_p_symbol_pointer = NULL;
  SgDeclarationStatement* decl_stat_it_symbol_pointer = NULL;

  // as we can find out in HiddenList_Intersection.C p_symbol_pointer should be a class or an enum..
  decl_stat_p_symbol_pointer = GetSgDeclarationStatementOutOfSgSymbol(p_symbol_pointer);
  decl_stat_it_symbol_pointer = GetSgDeclarationStatementOutOfSgSymbol(it_symbol_pointer);

  if(decl_stat_p_symbol_pointer != NULL && decl_stat_it_symbol_pointer != NULL) {

    Sg_File_Info* p_file_info;
    Sg_File_Info* it_file_info;

    p_file_info = decl_stat_p_symbol_pointer->get_file_info();
    it_file_info = decl_stat_it_symbol_pointer->get_file_info();

    if(p_file_info != NULL && it_file_info != NULL) {

      if(p_file_info->get_filename() == it_file_info->get_filename()) {

        return 1;

      }
      else {

        return 2;

      }

    }

    else {

      cout << "In hiddenlist computation: ERROR in CompareFileNamesOfSgSymbols()!!! At least one of them has NULL file info" << endl;
      return 0;

    }


  }
  else {

    cout << "In hiddenlist computation: ERROR in CompareFileNamesOfSgSymbols()!!! At least one of them has NULL declaration" << endl;
    return 0;

  }


}



// check if there are using-directives or using-declarations in the input code
// Motivation for this, look at Function Definition to learn more!

// Robert Preissl, July 9 2007 : this function is called in astPostProcessing for SgFile
void SetUsingDirectivesSetAndUsingDeclarationsSet(HiddenListComputationTraversal& exampleTraversal, SgFile* project) {

        // Robert Preissl, June 7 2007:
        // Motivating example:
        //         namespace X {
        //                class X1;
        //        }
        //        using namespace X;
        //        void X1();
        //
        // -> Function X1 should "hide" (not really hiding, but let's say yes) the class X1, but the update of the scope will take place when
        //    the interesction has already been finished! So query my input files if there are Using Decl. or Directives and make an update
        //    of the Scope before the intersection procedure starts!

        // Robert Preissl, July 9 2007 : this function is called in astPostProcessing for SgFile

        /* OLD CODE for SgProject* project : don't remove!!!

        SgFilePtrList* file_list_pointer = project->get_fileList();
        SgFilePtrList::iterator it_file_list_pointer;

        list<SgNode*> temp_UsingDirectivesList, UsingDirectivesList;
        list<SgNode*> temp_UsingDeclarationsList, UsingDeclarationsList;

        for( it_file_list_pointer = file_list_pointer->begin(); it_file_list_pointer != file_list_pointer->end(); ++it_file_list_pointer ) {

                temp_UsingDirectivesList = NodeQuery::querySubTree(*it_file_list_pointer, V_SgUsingDirectiveStatement);
                UsingDirectivesList.merge(temp_UsingDirectivesList);

                temp_UsingDeclarationsList = NodeQuery::querySubTree(*it_file_list_pointer, V_SgUsingDeclarationStatement);
                UsingDeclarationsList.merge(temp_UsingDeclarationsList);

        }

        */

//        SgFilePtrList* file_list_pointer = project->get_fileList();
//        SgFilePtrList::iterator it_file_list_pointer;

#if 0
        Rose_STL_Container<SgNode*> temp_UsingDirectivesList, UsingDirectivesList;
        Rose_STL_Container<SgNode*> temp_UsingDeclarationsList, UsingDeclarationsList;

//      for( it_file_list_pointer = file_list_pointer->begin(); it_file_list_pointer != file_list_pointer->end(); ++it_file_list_pointer ) {

                temp_UsingDirectivesList = NodeQuery::querySubTree(project, V_SgUsingDirectiveStatement);
                UsingDirectivesList.merge(temp_UsingDirectivesList);

                temp_UsingDeclarationsList = NodeQuery::querySubTree(project, V_SgUsingDeclarationStatement);
                UsingDeclarationsList.merge(temp_UsingDeclarationsList);

//      }
#else
     // DQ (9/25/2007): New code using the std::vector instead of std::list
        Rose_STL_Container<SgNode*> UsingDirectivesList   = NodeQuery::querySubTree(project, V_SgUsingDirectiveStatement);
        Rose_STL_Container<SgNode*> UsingDeclarationsList = NodeQuery::querySubTree(project, V_SgUsingDeclarationStatement);
#endif

        // filling sets of (Using Directives/Declaration, Scope) because have to look in every scope statement if there is an using dir/decl
        if( !UsingDirectivesList.empty() ) {

                UsingDirectiveWithScope using_directive_with_scope;
                SgUsingDirectiveStatement* using_directive_stmt;
                SgScopeStatement* scope_stmt;

             // DQ (9/25/2007): New code using the std::vector instead of std::list
                Rose_STL_Container<SgNode*>::iterator it_list_nodes;

                // filling the UsingDirectivesSet
                for( it_list_nodes = UsingDirectivesList.begin(); it_list_nodes != UsingDirectivesList.end(); ++it_list_nodes ) {

                        using_directive_stmt = isSgUsingDirectiveStatement(*it_list_nodes);
                        ROSE_ASSERT(using_directive_stmt != NULL);

                        scope_stmt = isSgScopeStatement(using_directive_stmt->get_parent());
                        ROSE_ASSERT(scope_stmt != NULL);

                        using_directive_with_scope.using_dir = using_directive_stmt;
                        using_directive_with_scope.scope = scope_stmt;
                        using_directive_with_scope.is_already_proofed_to_be_valid = false;

// CH (4/7/2010) : The coresponding member has been added.
//#ifndef _MSCx_VER
						(exampleTraversal.UsingDirectivesSet).insert(using_directive_with_scope);
//#else
//							// tps (12/7/2009) This is currently not defines since it fails in Release mode
//#pragma message ("WARNING: HiddenList: SetUsingDirectivesSetAndUsingDeclarationsSet: UsingDirectivesSet commented out right now.")
//#endif

                }

        }

        if( !UsingDeclarationsList.empty() ) {

                UsingDeclarationWithScope using_declaration_with_scope;
                SgUsingDeclarationStatement* using_declaration_stmt;
                SgScopeStatement* scope_stmt;

             // DQ (9/25/2007): New code using the std::vector instead of std::list
                Rose_STL_Container<SgNode*>::iterator it_list_nodes;

                // filling the UsingDeclarationsSet
                for( it_list_nodes = UsingDeclarationsList.begin(); it_list_nodes != UsingDeclarationsList.end(); ++it_list_nodes ) {

                        using_declaration_stmt = isSgUsingDeclarationStatement(*it_list_nodes);
                        ROSE_ASSERT(using_declaration_stmt != NULL);

                        scope_stmt = isSgScopeStatement(using_declaration_stmt->get_parent());
                        ROSE_ASSERT(scope_stmt != NULL);

                        using_declaration_with_scope.using_decl = using_declaration_stmt;
                        using_declaration_with_scope.scope = scope_stmt;
                        using_declaration_with_scope.is_already_proofed_to_be_valid = false;

// CH (4/7/2010) : The coresponding member has been added.
//#ifdef _MSCx_VER
                        (exampleTraversal.UsingDeclarationsSet).insert(using_declaration_with_scope);
//#else
//							// tps (12/7/2009) This is currently not defines since it fails in Release mode
//#pragma message ("WARNING: HiddenList: SetUsingDirectivesSetAndUsingDeclarationsSet: UsingDeclarationSet commented out right now.")
//#endif

                }

        }

        #ifdef HIDDEN_LIST_DEBUG
                cout << " Size of UsingDirectivesList " << exampleTraversal.UsingDirectivesSet.size() << endl;
                cout << " Size of UsingDeclarationsList " << exampleTraversal.UsingDeclarationsSet.size() << endl;
        #endif

}

// Robert Preissl, July 2 2007 : Help-Function called when needed the name and the scope name of a declaration-stmt in a using-decl. (scope-name gives me
//        the name of the namespace)
void GetName_And_Scopequalifiedname_AndScopename_Of_SgDeclStat_In_UsingDecl(SgDeclarationStatement* decl_stat, string& name, string& scope_qualified_name, string& scope_name) {

        SgNamespaceDeclarationStatement* namespace_decl_stmt;

        // variables
        if(isSgVariableDeclaration(decl_stat))
        {
                SgVariableDeclaration* var_decl = isSgVariableDeclaration(decl_stat);
                SgInitializedName* initialized_name_variable = var_decl->get_definition()->get_vardefn();
                if(initialized_name_variable != NULL)
                {
                        name = initialized_name_variable->get_name().str();
                        // cout << "InitializedName: " << name << endl;
                        SgScopeStatement* scope_stat = initialized_name_variable->get_scope();
                        scope_qualified_name = scope_stat->get_qualified_name().str();

                        if(isSgNamespaceDefinitionStatement(scope_stat)) {

                                namespace_decl_stmt = isSgNamespaceDefinitionStatement(scope_stat)->get_namespaceDeclaration();
                                scope_name = namespace_decl_stmt->get_name();

                        }
                        else {

                                scope_name = "";

                        }

                        // cout << "SCOPE: " << scope_name << endl;
                }
        }

        // functions
        else if(isSgFunctionDeclaration(decl_stat)) {
                SgFunctionDeclaration* func_decl = isSgFunctionDeclaration(decl_stat);
                SgScopeStatement* scope_stat = func_decl->get_scope();
                scope_qualified_name = scope_stat->get_qualified_name().str();
                // cout << "SCOPE: " << scope_qualified_name << endl;
                name = func_decl->get_name().str();
                // cout << "InitializedName: " << name << endl;

                if(isSgNamespaceDefinitionStatement(scope_stat)) {

                        namespace_decl_stmt = isSgNamespaceDefinitionStatement(scope_stat)->get_namespaceDeclaration();
                        scope_name = namespace_decl_stmt->get_name();

                }
                else {

                        scope_name = "";

                }
        }
        // typedefs
        else if(isSgTypedefDeclaration(decl_stat)) {
                SgTypedefDeclaration* type_decl = isSgTypedefDeclaration(decl_stat);
                SgScopeStatement* scope_stat = type_decl->get_scope();
                scope_qualified_name = scope_stat->get_qualified_name().str();
                name = type_decl->get_name().str();

                if(isSgNamespaceDefinitionStatement(scope_stat)) {

                        namespace_decl_stmt = isSgNamespaceDefinitionStatement(scope_stat)->get_namespaceDeclaration();
                        scope_name = namespace_decl_stmt->get_name();

                }
                else {

                        scope_name = "";

                }
        }
        // classes
        else if(isSgClassDeclaration(decl_stat)) {
                SgClassDeclaration* class_decl = isSgClassDeclaration(decl_stat);
                SgScopeStatement* scope_stat = class_decl->get_scope();
                scope_qualified_name = scope_stat->get_qualified_name().str();
                name = class_decl->get_name().str();

                if(isSgNamespaceDefinitionStatement(scope_stat)) {

                        namespace_decl_stmt = isSgNamespaceDefinitionStatement(scope_stat)->get_namespaceDeclaration();
                        scope_name = namespace_decl_stmt->get_name();

                }
                else {

                        scope_name = "";

                }
        }
        // enums
        else if(isSgEnumDeclaration(decl_stat)) {
                SgEnumDeclaration* enum_decl = isSgEnumDeclaration(decl_stat);
                SgScopeStatement* scope_stat = enum_decl->get_scope();
                scope_qualified_name = scope_stat->get_qualified_name().str();
                name = enum_decl->get_name().str();

                if(isSgNamespaceDefinitionStatement(scope_stat)) {

                        namespace_decl_stmt = isSgNamespaceDefinitionStatement(scope_stat)->get_namespaceDeclaration();
                        scope_name = namespace_decl_stmt->get_name();

                }
                else {

                        scope_name = "";

                }
        }

        // not allowed by EDG, but allowed under g++ -ansi
        //else if(isSgNamespaceDeclarationStatement(decl_stat)) {
        //        SgNamespaceDeclarationStatement* namesp_decl = isSgNamespaceDeclarationStatement(decl_stat);
                // ...
        //}
        else { cout << "ERROR in GetNameAndScopenameOfSgDeclStat" << endl; }

}


// DQ (5/8/2007): Added this function to wrap up details of how to call the hidden list work
void buildHiddenTypeAndDeclarationLists( SgNode* node /*SgProject* project*/ )
   {
  // DQ (8/1/2007): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("Hidden type and hidden declaration list generation:");

  // cout << " >> Class name: " << node->sage_class_name() << endl;

        //SgProject* project = isSgProject(node);
        //ROSE_ASSERT(project != NULL);

        // Robert Preissl, July 9 2007 : this function is called in astPostProcessing for SgFile
        SgSourceFile* project = isSgSourceFile(node);

        if (project == NULL)
           {
             printf ("Can't process buildHiddenTypeAndDeclarationLists for AST fragment = %p = %s \n",node,node->class_name().c_str());
             return;
           }
        ROSE_ASSERT(project != NULL);

        #ifdef HIDDEN_LIST_DEBUG
                cout << " >>>  Begin of buildHiddenTypeAndDeclarationLists Procedure" << endl;
        #endif

        vector<ScopeStackEntry> VSSE;
        vector<ScopeStackEntry>* pointer_VSSE;
        pointer_VSSE = &VSSE;

        int start_depth = 0;

        InheritedAttributeSgScopeStatement inheritedAttribute(start_depth, pointer_VSSE);
        // SynthesizedAttributeSgScopeStatement synthesizedAttribute();


        // Robert Preissl, June 8 2007: Redesign of HiddenList-Computation:
        //  Seperating the existing traversal into two traversal, where the first traversal traverses the whole AST to
        //  collect namespace & class information, and the second traversal computes based upon the data of the first
        //  traversal the hiddenlists only for the specified input files!

        // Motivating example:
        //         namespace Z {
        //                int z;
        //        }
        //        using namespace Z;
        //
        //        int main() {
        //                double z;
        //                return 0;
        //        }

        // -> at the time where I would make an update to global scope because of the using-directive,
        //    the information of namespace Z hasn't been collected yet!

        // The other motivation for this redesign was a much more clear and clean design for later usages where e.g. someone
        // is only interested in Symbols of Namespaces or classes for a special analysis; so only the first traversal would be needed to run.

        // Robert Preissl, June 26 2007 :  parameter specifies the collection mode of using-information:
        //  case 0: a UsingDirectiveStatement_LinkedListStackSetSgDeclarationStatements_HashMap will be built up
        //  case 1: just a UsingDirectiveStatement_SetSgDeclarationStatements_HashMap will be built up (means that only declarations
        //     before the using in the same scope are stored)
        // (of course, the same applies to using-decl.)

        int collection_mode_of_NamespacesAndClassTraversal = 1;
        NamespacesAndClassTraversal namespaces_and_class_traversal(collection_mode_of_NamespacesAndClassTraversal);

        buildNamespacesAndClassHashMaps( project, namespaces_and_class_traversal );

     // DQ (10/22/2007): These are hard to find and remove from the output!!!
     // cout << endl;

     // cout << " >>> Using Information relative to other declaration: " << endl;

        if(collection_mode_of_NamespacesAndClassTraversal == 0) {

                UsingDirectiveStatement_LinkedListStackSetSgDeclarationStatements_HashMap__Output(namespaces_and_class_traversal.UsingDirRelativeToDeclarations);

        }
        else if(collection_mode_of_NamespacesAndClassTraversal == 1) {

                #ifdef HIDDEN_LIST_DEBUG
                        UsingDirectiveStatement_SetSgDeclarationStatements_HashMap__Output(namespaces_and_class_traversal.UsingDirRelativeToDeclarations_2);

                        cout << "                << >>                " << endl;

                        UsingDeclarationStatement_SetSgDeclarationStatements_HashMap__Output(namespaces_and_class_traversal.UsingDeclRelativeToDeclarations_2);
                #endif

        }
        else {

                cout << "ERROR: wrong collection mode! " << endl;

        }

     // DQ (10/22/2007): These are hard to find and remove from the output!!!
     // cout << endl;

        HiddenListComputationTraversal hiddenlist_traversal;

        if(collection_mode_of_NamespacesAndClassTraversal == 0) {

                // constructor call
                //        hiddenlist_traversal.xy = 0;
                //        hiddenlist_traversal.NamespacesHashMap(namespaces_and_class_traversal.NamespacesHashMap);
                //        hiddenlist_traversal.ClassHashMap = namespaces_and_class_traversal.ClassHashMap;
                //        hiddenlist_traversal.UsingDirRelativeToDeclarations = namespaces_and_class_traversal.UsingDirRelativeToDeclarations;
                //        hiddenlist_traversal.UsingDeclRelativeToDeclarations = namespaces_and_class_traversal.UsingDeclRelativeToDeclarations;
                hiddenlist_traversal = HiddenListComputationTraversal(0, namespaces_and_class_traversal.NamespacesHashMap, namespaces_and_class_traversal.ClassHashMap, namespaces_and_class_traversal.UsingDirRelativeToDeclarations, namespaces_and_class_traversal.UsingDeclRelativeToDeclarations );

        }
        else if(collection_mode_of_NamespacesAndClassTraversal == 1) {

                // constructor call
                //        hiddenlist_traversal.xy = 0;
                //        hiddenlist_traversal.NamespacesHashMap(namespaces_and_class_traversal.NamespacesHashMap);
                //        hiddenlist_traversal.ClassHashMap = namespaces_and_class_traversal.ClassHashMap;
                //        hiddenlist_traversal.UsingDirRelativeToDeclarations_2 = namespaces_and_class_traversal.UsingDirRelativeToDeclarations_2;
                //        hiddenlist_traversal.UsingDeclRelativeToDeclarations_2 = namespaces_and_class_traversal.UsingDeclRelativeToDeclarations_2;
                hiddenlist_traversal = HiddenListComputationTraversal(0, namespaces_and_class_traversal.NamespacesHashMap, namespaces_and_class_traversal.ClassHashMap, namespaces_and_class_traversal.UsingDirRelativeToDeclarations_2, namespaces_and_class_traversal.UsingDeclRelativeToDeclarations_2 );

        }
        else {

                cout << "ERROR: wrong collection mode! " << endl;

        }


        #ifdef HIDDEN_LIST_DEBUG

                cout << endl;
                cout << " >>>  Namespace-Information of whole AST collected by NamespacesAndClassTraversal: " << endl;
                String_VectorOfNamespaceInformation_HashMapOutput(hiddenlist_traversal.NamespacesHashMap);

                cout << endl;

                cout << " >>>  Class-Information of whole AST collected by NamespacesAndClassTraversal: " << endl;
                StringVectorHashMapOutput(hiddenlist_traversal.ClassHashMap);
                cout << endl;

        #endif


        // check if there are using-directives or using-declarations in the input code
        // Motivation for this, look at Function Definition to learn more!
        SetUsingDirectivesSetAndUsingDeclarationsSet(hiddenlist_traversal, project);

        // ad comment above from June 8 2007 : now we call the hidden-list computation only for the specified input-files! -> traverseImputFile instead of traverse
        //hiddenlist_traversal.traverse(project, inheritedAttribute);

        // Robert Preissl, July 9 2007 : this function is called in astPostProcessing for SgFile (changed from traverseInputFiles to traverseWithinFile)
        hiddenlist_traversal.traverseWithinFile(project, inheritedAttribute);
}

} // namespace

