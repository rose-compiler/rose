#if 0
// DQ (5/21/2013): I think that we can and should remove this file.
// for the moment I will comment out the whole file.

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
// #include "HiddenList_Output.h"

#define ROSE_TRACK_PROGRESS_OF_ROSE_COMPILING_ROSE 0

using namespace std;

// using own namespace
namespace Hidden_List_Computation {

double accumulatedEvaluateInheritedAttribute_1_Time  = 0.0;
double accumulatedEvaluateInheritedAttribute_1_Calls = 0.0;

double accumulatedEvaluateInheritedAttributeCommonCaseTime  = 0.0;
double accumulatedEvaluateInheritedAttributeCommonCaseCalls = 0.0;

void NamespacesAndClassTraversal::UpdateNamespace(string updated_namespace, string used_namespace) {

        VectorOfNamespaceInformation used_namespace_VecNamespInfo;
        used_namespace_VecNamespInfo = this->NamespacesHashMap[used_namespace];

        // this entry (VectorOfNamespaceInformation) could contain of several entries for this special namespace (namespace A{int a;} .. namespace A{int b;}
        //  -> size of VectorOfNamespaceInformation = 2
        // Therefore add ALL the entries to the updated namespace entry (to the last entry!!)

        VectorOfNamespaceInformation updated_namespace_VecNamespInfo;
        updated_namespace_VecNamespInfo = this->NamespacesHashMap[ updated_namespace ];

        // Robert Preissl, July 3 2007 : find out the size of the updated namespace to get sure that the correct position will be updated!!
        // Motivating example:
        //        NS X { .. }
        //        NS X { .. }
        //        NS Y { .. }
        //        NS Y { using NS X }
        int size_of_updated_namespace_VecNamespInfo = (int)updated_namespace_VecNamespInfo.size();

        for(int i = 0; i < (int)used_namespace_VecNamespInfo.size(); ++i) {

                for(int j = 0; j < (int)(used_namespace_VecNamespInfo[i].VectorOfSymbolInformation.size() ); ++j ) {

                        updated_namespace_VecNamespInfo[size_of_updated_namespace_VecNamespInfo - 1].VectorOfSymbolInformation.push_back(used_namespace_VecNamespInfo[i].VectorOfSymbolInformation[j]);

                }

        }

        // overwrite the previous entry
        this->NamespacesHashMap[ updated_namespace ] = updated_namespace_VecNamespInfo;

}

void NamespacesAndClassTraversal::UpdateNamespace(string updated_namespace, string used_namespace, string n) {

        VectorOfNamespaceInformation used_namespace_VecNamespInfo;
        used_namespace_VecNamespInfo = this->NamespacesHashMap[used_namespace];

        // this entry (VectorOfNamespaceInformation) could contain of several entries for this special namespace (namespace A{int a;} .. namespace A{int b;}
        //  -> size of VectorOfNamespaceInformation = 2
        // Therefore add ALL the entries to the updated namespace entry (to the last entry!!)

        VectorOfNamespaceInformation updated_namespace_VecNamespInfo;
        updated_namespace_VecNamespInfo = this->NamespacesHashMap[ updated_namespace ];

        // Robert Preissl, July 3 2007 : find out the size of the updated namespace to get sure that the correct position will be updated!!
        // Motivating example:
        //        NS X { .. }
        //        NS X { .. }
        //        NS Y { .. }
        //        NS Y { using X::x }
        int size_of_updated_namespace_VecNamespInfo = (int)updated_namespace_VecNamespInfo.size();

        for(int i = 0; i < (int)used_namespace_VecNamespInfo.size(); ++i) {

                for(int j = 0; j < (int)(used_namespace_VecNamespInfo[i].VectorOfSymbolInformation.size()); ++j ) {

                        if(used_namespace_VecNamespInfo[i].VectorOfSymbolInformation[j]->name == n) {

                                updated_namespace_VecNamespInfo[size_of_updated_namespace_VecNamespInfo - 1].VectorOfSymbolInformation.push_back(used_namespace_VecNamespInfo[i].VectorOfSymbolInformation[j]);

                        }

                }

        }

        // overwrite the previous entry
        this->NamespacesHashMap[ updated_namespace ] = updated_namespace_VecNamespInfo;

}

InheritedAttributeForNamespacesAndClassTraversal NamespacesAndClassTraversal :: evaluateInheritedAttribute(SgNode* n, InheritedAttributeForNamespacesAndClassTraversal inheritedAttribute) {

     // DQ (8/3/2007): Start time for this function
        TimingPerformance::time_type startTime;
        TimingPerformance::startTimer(startTime);

        TimingPerformance::time_type startTimeCommonCase;

#if ROSE_TRACK_PROGRESS_OF_ROSE_COMPILING_ROSE
     // DQ (2/9/2010): Debugging code for ROSE compiling ROSE.
        if (isSgStatement(n) != NULL)
           {
             SgStatement* stmt = isSgStatement(n);
             printf ("In NamespacesAndClassTraversal::evaluateInheritedAttribute(): file = %s line = %d \n",stmt->get_startOfConstruct()->get_filenameString().c_str(),stmt->get_startOfConstruct()->get_line());
           }
#endif

         // Robert Preissl, June 20 2007: ad Occurence of using directives (and decl.) in the source-code relative to other declarations.
        if(isSgUsingDirectiveStatement(n) || isSgUsingDeclarationStatement(n)) {

                if( this->collection_mode == 0 ) {

                        int size_of_stack = (int)this->stack_of_declarations.size();

                        if( this->UsingDirRelativeToDeclarations.empty() ) {

                                (this->list_of_stack_of_declarations).PreviousLinkedListStackSetSgDeclarationStatements = NULL;
                                (this->list_of_stack_of_declarations).CurrentStackSetSgDeclarationStatements = this->stack_of_declarations;

                                //this->stack_of_declarations.clear();

                                // clear entries of stack:
                                for(int i = 0; i < size_of_stack; ++i ) {

                                        this->stack_of_declarations[i].clear();

                                }

                                this->last_using_dir_or_decl_stmt = (SgDeclarationStatement*)n;

                        }
                        else {

                                (this->list_of_stack_of_declarations).PreviousLinkedListStackSetSgDeclarationStatements = this->last_using_dir_or_decl_stmt;
                                (this->list_of_stack_of_declarations).CurrentStackSetSgDeclarationStatements = this->stack_of_declarations;

                                //this->stack_of_declarations.clear();
                                // clear entries of stack:
                                for(int i = 0; i < size_of_stack; ++i ) {

                                        this->stack_of_declarations[i].clear();

                                }

                                this->last_using_dir_or_decl_stmt = (SgDeclarationStatement*)n;

                        }

                        if(isSgUsingDirectiveStatement(n)) {

                                #ifdef HIDDEN_LIST_DEBUG
                                        cout << "Insert into UsingDirRelativeToDeclarations" << endl;
                                #endif

                                SgUsingDirectiveStatement* using_dir = isSgUsingDirectiveStatement(n);

                                this->UsingDirRelativeToDeclarations[using_dir] = this->list_of_stack_of_declarations;

                                #ifdef HIDDEN_LIST_DEBUG
                                        cout << "END Insert into UsingDirRelativeToDeclarations" << endl;
                                #endif

                        }
                        else {

                                SgUsingDeclarationStatement* using_decl = isSgUsingDeclarationStatement(n);

                                this->UsingDeclRelativeToDeclarations[using_decl] = this->list_of_stack_of_declarations;

                        }

                }
                else if( this->collection_mode == 1 ) {

                        if(isSgUsingDirectiveStatement(n)) {

                                #ifdef HIDDEN_LIST_DEBUG
                                        cout << "Insert into UsingDirRelativeToDeclarations" << endl;
                                #endif

                                SgUsingDirectiveStatement* using_dir = isSgUsingDirectiveStatement(n);

                                //this->UsingDirRelativeToDeclarations_2[using_dir] = this->declarations_before_using;
                                this->UsingDirRelativeToDeclarations_2[using_dir] = this->stack_of_declarations[inheritedAttribute.depth-1];

                        }
                        else {

                                SgUsingDeclarationStatement* using_decl = isSgUsingDeclarationStatement(n);

                                //this->UsingDeclRelativeToDeclarations_2[using_decl] = this->declarations_before_using;
                                this->UsingDeclRelativeToDeclarations_2[using_decl] = this->stack_of_declarations[inheritedAttribute.depth-1];

                        }

                }
                else {

                        cout << "ERROR: wrong collection mode! " << endl;

                }

        }

        // Robert Preissl, June 20 2007: ad Occurence of using directives (and decl.) in the source-code relative to other declarations:
        //  if encounter a declarationstmt, enter in the declarationset

        //  works now for variables, functions, tyepdefs, classes, enums, namespaces, templates

        // Robert Preissl, June 26 2007 : add isSgUsingDirectiveStatement to the list (reason: if you have two or more usings in the same scope, have
        //   to find out which using happens before the other one)

        if( isSgVariableDeclaration(n) || isSgFunctionDeclaration(n) || isSgTypedefDeclaration(n) || isSgClassDeclaration(n) || isSgEnumDeclaration(n) || isSgNamespaceDeclarationStatement(n) || isSgTemplateDeclaration(n) || isSgUsingDirectiveStatement(n) ||isSgUsingDeclarationStatement(n) ) {

                TimingPerformance::startTimer(startTimeCommonCase);

                #ifdef HIDDEN_LIST_DEBUG
                        cout << " -> Inserting into this->stack_of_declarations[" << (int)this->stack_of_declarations.size()-1 <<  "][" <<this->stack_of_declarations[(int)this->stack_of_declarations.size()-1].size() << "] : " << GetSgNameOutOfSgDeclarationStatement((SgDeclarationStatement*)n).str() << endl;
                #endif

                if( this->collection_mode == 0 ) {

                        int size_of_stack = (int)this->stack_of_declarations.size();
                        // insert directly into stack at its top position:
                        this->stack_of_declarations[size_of_stack - 1].insert((SgDeclarationStatement*)n);

                }
                else if( this->collection_mode == 1 ) {

                        int size_of_stack = (int)this->stack_of_declarations.size();
                        // insert directly into stack at its top position:
                        this->stack_of_declarations[size_of_stack - 1].insert((SgDeclarationStatement*)n);

                        // this->declarations_before_using.insert( (SgDeclarationStatement*)n );

                }
                else {

                        cout << "ERROR: wrong collection mode! " << endl;

                }

          // DQ (8/3/2007): accumulate the elapsed time for this function
             TimingPerformance::accumulateTime ( startTimeCommonCase, accumulatedEvaluateInheritedAttributeCommonCaseTime, accumulatedEvaluateInheritedAttributeCommonCaseCalls );

        }

        // for the usage of namespaces
        if(isSgUsingDirectiveStatement(n)) {

                SgUsingDirectiveStatement* using_dir_stat = isSgUsingDirectiveStatement(n);

                #ifdef HIDDEN_LIST_DEBUG
                        cout << "---------------" << endl;
                        cout << "Using Directive:  " << using_dir_stat->get_mangled_name().str() << endl;
                        cout << "--------------- " << endl;cout << endl;
                #endif

                SgNamespaceDeclarationStatement* namesp_decl_stat = using_dir_stat->get_namespaceDeclaration();
                string used_namespace = namesp_decl_stat->get_qualified_name().str();
                #ifdef HIDDEN_LIST_DEBUG
                        cout << "Namespace used: " << used_namespace << endl;

                        cout << "Update of SymbolStack at depth: " << inheritedAttribute.depth-1 << endl;
                #endif

                // Namespace Usement is transitive: if using happens in a Nampespace Definition Stat. -> update NamespacesHashMap
                SgNamespaceDefinitionStatement* namesp_def_stat = (SgNamespaceDefinitionStatement*)n->get_parent();
                if(isSgNamespaceDefinitionStatement(namesp_def_stat)) {

                        SgNamespaceDeclarationStatement* namesp_decl_stat_2 = namesp_def_stat->get_namespaceDeclaration();
                        string updated_namespace = namesp_decl_stat_2->get_qualified_name().str();

                        #ifdef HIDDEN_LIST_DEBUG
                                cout << "Transitive Usage of Namespaces!!" << endl;
                                cout << "Namepace: " << updated_namespace << " will be updated with the information of namespace: " << used_namespace << endl;
                        #endif

                        UpdateNamespace(updated_namespace, used_namespace);

                }

        }

        if(isSgUsingDeclarationStatement(n)) {

                bool using_decl_in_class = false;
                string class_name;

                SgUsingDeclarationStatement* using_decl_stat = isSgUsingDeclarationStatement(n);

                #ifdef HIDDEN_LIST_DEBUG
                        cout << "---------------" << endl;
                        cout << "Using Declaration!! " << endl;
                        cout << "---------------" << endl; cout << endl;
                #endif

                // using decl. in class: -> so parent of the usingdecl. should be a class-def.
                if(isSgClassDefinition(using_decl_stat->get_parent()) ) {

                        using_decl_in_class = true;

                        SgClassDefinition* class_def = isSgClassDefinition(using_decl_stat->get_parent());

                        SgClassDeclaration* class_decl = class_def->get_declaration();
                        class_name = class_decl->get_name().str();

                        #ifdef HIDDEN_LIST_DEBUG
                                cout << "Using in Class with name = " << class_name << endl;
                        #endif

                        // Motivating example, page 119 in C++Standard,
                        //
                        //        struct B {
                        //                void f(char) {  }
                        //                void g(char) {  }
                        //        };
                        //
                        //        struct D: B {
                        //                using B::f;
                        //                void f(int) { f('c');  }
                        //
                        //                void g(int) {  g('c'); }
                        //        };
                        //        -> 1.) g(int) hides g(char)
                        //        -> 2.) using B::f doesn't hide, it overloades


                }

                // Robert Preissl, May 25 2007
                // Motivating example: Cxx_tests: test2005_114.C : using declarations in classes/structs

                if(isSgDeclarationStatement(using_decl_stat->get_declaration()) ) {

                        SgDeclarationStatement* decl_stat = using_decl_stat->get_declaration();

                        ROSE_ASSERT(decl_stat != NULL);

                        //!!! using declarations works now for variables, functions, typedefs, classes, enums
                        if( isSgVariableDeclaration(decl_stat) || isSgFunctionDeclaration(decl_stat) || isSgTypedefDeclaration(decl_stat) || isSgClassDeclaration(decl_stat) || isSgEnumDeclaration(decl_stat) ) {

                                string name, scope_name, used_namespace;

                                // used_namespace is just a dummy argument in this case, will not needed later
                                GetName_And_Scopequalifiedname_AndScopename_Of_SgDeclStat_In_UsingDecl(decl_stat, name, scope_name, used_namespace);

                                //        cout << "name of used decl.: " << name << endl;

                                if( using_decl_in_class == true ) {

                                        SgDeclarationStatement* decl_stat_of_class_hashmap;

                                        // find this element in the corresponfing class and mark it as "used"

                                        Vector_Of_SymbolInformation::iterator it_vectorOfSymbolInformation;

                                        for(it_vectorOfSymbolInformation = this->ClassHashMap[class_name].begin(); it_vectorOfSymbolInformation != this->ClassHashMap[class_name].end(); ++it_vectorOfSymbolInformation) {

                                                if( (*it_vectorOfSymbolInformation)->name == name ) {

                                                        decl_stat_of_class_hashmap = GetSgDeclarationStatementOutOfSgSymbol((*it_vectorOfSymbolInformation)->symbol_pointer);
                                                        ROSE_ASSERT(decl_stat_of_class_hashmap != NULL);

                                                        if( decl_stat_of_class_hashmap->get_mangled_name() == decl_stat->get_mangled_name() ) {

                                                                // modify entries
                                                                (*it_vectorOfSymbolInformation)->is_using_decl_in_class = true;
                                                                (*it_vectorOfSymbolInformation)->si_using_decl_in_class = using_decl_stat;

                                                                // cout << "Found and marked!" << endl;


                                                        }


                                                }

                                        }

                                }
                                else {

                                        // Namespace Usement is transitive: if using happens in a Nampespace Definition Stat. -> update NamespacesHashMap
                                        SgNamespaceDefinitionStatement* namesp_def_stat = (SgNamespaceDefinitionStatement*)n->get_parent();
                                        if(isSgNamespaceDefinitionStatement(namesp_def_stat)) {
                                                SgNamespaceDeclarationStatement* namesp_decl_stat = namesp_def_stat->get_namespaceDeclaration();
                                                string updated_namespace = namesp_decl_stat->get_qualified_name().str();

                                                #ifdef HIDDEN_LIST_DEBUG
                                                        cout << "Transitive Usage of Namespaces!!" << endl;
                                                        cout << "Namepace: " << updated_namespace << " will be updated with the information of namespace: " << scope_name << endl;
                                                #endif

                                                UpdateNamespace(updated_namespace, scope_name, name);
                                        }

                                }

                        }

                }
                else {

                #ifdef HIDDEN_LIST_DEBUG
                        cout << "  NO Using in Class, NO Using in Namespace " << endl;
                #endif

                }


        }

        if(isSgGlobal(n)) {
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

                #ifdef HIDDEN_LIST_DEBUG
                        cout << "**************** SCOPE BEGIN *****************************************************"<<endl; cout << endl;

                        cout << "Scope Name: " << SgScope->get_qualified_name().str();
                        cout << " in depth: " << inheritedAttribute.depth << endl;
                #endif

                SgNamespaceDefinitionStatement* snds = isSgNamespaceDefinitionStatement(n);
                if(snds != NULL) {

                        #ifdef HIDDEN_LIST_DEBUG
                                cout << "Namespace Definition with name: " << snds->get_qualified_name().str() << endl;
                        #endif
                        namespace_definition = true;

                }

                if( this->collection_mode == 0 ) {

                        // Robert Preissl, June 20 2007: ad Occurence of using directives (and decl.) in the source-code relative to other declarations:
                        //  if enter a new scope we create a new stack frame for the declarationset
                        if( isSgGlobal(n) /*(int)this->stack_of_declarations.size() == 0 */ ) {

                                #ifdef HIDDEN_LIST_DEBUG
                                        cout << "BEGIN!! create a new frame "<< endl;
                                #endif

                                this->stack_of_declarations.push_back(this->declarations_before_using);

                        }
                        else {

                                #ifdef HIDDEN_LIST_DEBUG
                                        cout << " push_back new frame in this->stack_of_declarations " << endl;
                                #endif

                                this->stack_of_declarations.push_back(this->declarations_before_using);

                                #ifdef HIDDEN_LIST_DEBUG
                                        cout << " END push_back new frame in this->stack_of_declarations / SIZE OF STACK AFTER PUSH_BACK: " << this->stack_of_declarations.size() << endl;
                                #endif

                        }

                }
                else if( this->collection_mode == 1 ) {

                        // this->declarations_before_using.clear();

                        // Robert Preissl, June 20 2007: ad Occurence of using directives (and decl.) in the source-code relative to other declarations:
                        //  if enter a new scope we create a new stack frame for the declarationset
                        if( isSgGlobal(n) /*(int)this->stack_of_declarations.size() == 0 */ ) {

                                #ifdef HIDDEN_LIST_DEBUG
                                        cout << "BEGIN!! create a new frame "<< endl;
                                #endif

                                this->stack_of_declarations.push_back(this->declarations_before_using);

                        }
                        else {

                                #ifdef HIDDEN_LIST_DEBUG
                                        cout << " push_back new frame in this->stack_of_declarations " << endl;
                                #endif

                                this->stack_of_declarations.push_back(this->declarations_before_using);

                                #ifdef HIDDEN_LIST_DEBUG
                                        cout << " END push_back new frame in this->stack_of_declarations / SIZE OF STACK AFTER PUSH_BACK: " << this->stack_of_declarations.size() << endl;
                                #endif

                        }

                }
                else {

                        cout << "ERROR: wrong collection mode! " << endl;

                }



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

                        bool class_definition = false;
                        if(isSgClassDefinition(n)) { class_definition = true; }

                        SgSymbolTable::hash_iterator SgSymbolTable_it = hashTable->begin();
                        // Iterate through the ROSE symbol table
                        while (SgSymbolTable_it != hashTable->end()) {

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

                        }


                        if(namespace_definition) {

                                // first look-up if an entry with this namespace name already exists (e.g. namespace A { int a;} ... namespace A{int b;}
                                string namespace_name = snds->get_qualified_name().str();

                                String_VectorOfNamespaceInformation_HashMap::iterator find_it;

                                find_it = this->NamespacesHashMap.find( namespace_name );

                                NamespaceInformation namespace_information;

                                // if yes -> update
                                if(find_it != this->NamespacesHashMap.end() ) {

                                        namespace_information.VectorOfSymbolInformation = temp_symboltable;
                                        namespace_information.namespace_definition_stmt = snds;
                                        // update, means that a VectorOfNamespaceInformation already exists with this namespace-name in the NamespacesHashMap, so
                                        //  I can just push the next namespace_information entry into the vector
                                        (find_it->second).push_back(namespace_information);

                                }
                                else {

                                        // new entry to NamespacesHashMap
                                        VectorOfNamespaceInformation vector_namespace_information;

                                        namespace_information.VectorOfSymbolInformation = temp_symboltable;
                                        namespace_information.namespace_definition_stmt = snds;

                                        vector_namespace_information.push_back(namespace_information);

                                        this->NamespacesHashMap[ namespace_name ] = vector_namespace_information;

                                }

                                #ifdef HIDDEN_LIST_DEBUG
                                        cout << "NAMESPACES: " << endl;
                                        String_VectorOfNamespaceInformation_HashMapOutput(this->NamespacesHashMap);
                                #endif

                        }
                        else if(class_definition) {

                                SgClassDefinition* class_def = isSgClassDefinition(n);
                                SgClassDeclaration* class_decl = class_def->get_declaration();
                                string class_name = class_decl->get_name().str();

                                #ifdef HIDDEN_LIST_DEBUG
                                        cout << "Class Definition: name = " << class_name << endl;
                                #endif

                                // DQ (5/31/2007): This implementation might not be good enough to handle "class X{}; void foo() { class X{}; }"
                                this->ClassHashMap[ class_name ] = temp_symboltable;

                                SgBaseClassPtrList base_classes = class_def->get_inheritances();
                                #ifdef HIDDEN_LIST_DEBUG
                                        cout << "Class " << class_name << " is derived from Nr. of different classes: " << base_classes.size() << endl;
                                #endif

                             // DQ (9/25/2007): New code using the std::vector instead of std::list
                                Rose_STL_Container<SgBaseClass*> :: iterator list_baseClasses_it;
                                string inherited_class_name;

                                for(list_baseClasses_it = base_classes.begin(); list_baseClasses_it != base_classes.end(); ++list_baseClasses_it) {

                                        class_decl = (*list_baseClasses_it)->get_base_class();
                                        inherited_class_name = class_decl->get_name().str();

                                        #ifdef HIDDEN_LIST_DEBUG
                                                cout << inherited_class_name << endl;
                                        #endif

                                        Vector_Of_SymbolInformation inherited_symbol_table;
                                        inherited_symbol_table = this->ClassHashMap[inherited_class_name];
                                        bool insert_ok = false;

                                     // for each class in this list ..
                                     // for(int i = 0; i < (int)this->ClassHashMap[inherited_class_name].size(); ++i) {
                                        for(int i = 0; i < (int)(inherited_symbol_table.size()); ++i) {

                                                SgSymbol* symb = (inherited_symbol_table[i]->symbol_pointer);

                                                if(symb != NULL) {

                                                     // DQ (5/31/2007): TODO: Please consider an implementation using "switch"
#if 0
                                                        printf ("In NamespacesAndClassTraversal::evaluateInheritedAttribute(): i = %d inherited_symbol_table.size() = %" PRIuPTR " symb = %p \n",i,inherited_symbol_table.size(),symb);
                                                        printf ("In NamespacesAndClassTraversal::evaluateInheritedAttribute(): symb = %p = %s \n",symb,symb->class_name().c_str());
#endif
                                                     // check if this symbols are private, if yes -> don't make an update with them
                                                        if (isSgVariableSymbol(symb) || isSgMemberFunctionSymbol(symb) || isSgClassSymbol(symb) || isSgTypedefSymbol(symb) || isSgEnumSymbol(symb) || isSgEnumFieldSymbol(symb) ) {

                                                                SgDeclarationStatement* decl_statement = NULL;
                                                                SgAccessModifier access_modifier;

                                                                if(isSgVariableSymbol(symb)) {
                                                                        SgVariableSymbol* var_symbol = isSgVariableSymbol(symb);
                                                                        decl_statement = var_symbol->get_declaration()->get_declaration();
                                                                }
                                                                else if(isSgMemberFunctionSymbol(symb)) {
                                                                        SgMemberFunctionSymbol* member_func_symb = isSgMemberFunctionSymbol(symb);
                                                                        decl_statement = (SgDeclarationStatement*)member_func_symb->get_declaration();
                                                                }
                                                                else if(isSgTypedefSymbol(symb)) {
                                                                        SgTypedefSymbol* typedef_symb = isSgTypedefSymbol(symb);
                                                                        decl_statement = (SgDeclarationStatement*)typedef_symb->get_declaration();
                                                                }
                                                                else if(isSgClassSymbol(symb)) {
                                                                        SgClassSymbol* class_symb = isSgClassSymbol(symb);
                                                                        decl_statement = (SgDeclarationStatement*)class_symb->get_declaration();
                                                                }
                                                                else if(isSgEnumSymbol(symb)) {
                                                                        SgEnumSymbol* enum_symb = isSgEnumSymbol(symb);
                                                                        decl_statement = (SgDeclarationStatement*)enum_symb->get_declaration();
                                                                }
                                                                else if(isSgEnumFieldSymbol(symb)) {
                                                                        SgEnumFieldSymbol* enumfield_symb = isSgEnumFieldSymbol(symb);
                                                                        decl_statement = enumfield_symb->get_declaration()->get_declaration();
                                                                }

                                                                if(decl_statement != NULL) {
                                                                        SgDeclarationModifier decl_modifier = decl_statement->get_declarationModifier();
                                                                        access_modifier = decl_modifier.get_accessModifier();
                                                                }

                                                                if(access_modifier.isPrivate()) {
                                                                        #ifdef HIDDEN_LIST_DEBUG
                                                                                cout << "!! Private !!" << endl;
                                                                        #endif
                                                                        insert_ok = false; // no insert for private members
                                                                }
                                                                else { insert_ok = true; }

                                                        }
                                                        else { insert_ok = true; }

                                                        if(insert_ok) {

                                                                // .. make an update to ClassHashMap..
                                                                //        old code:
                                                                //        (this->ClassHashMap[ class_name ]).push_back(inherited_symbol_table[i] );

                                                                // Robert Preissl, July 5 2007 : uses push_front instead of push_back, because ..
                                                                // Motivatin example:
                                                                //        class A
                                                                //        {
                                                                //                public:
                                                                //                        A operator=(A a);
                                                                //        };
                                                                //
                                                                //        class B : public A
                                                                //        {
                                                                //                public:
                                                                //                        A operator=(A a);
                                                                //        };
                                                                //
                                                                // The Problem was that A's = was pushed to the end and so was HIDING B's = in the intersection
                                                                //  procedure called in the scope of class B!!!!
                                                                (this->ClassHashMap[ class_name ]).push_front( inherited_symbol_table[i] );

                                                        }

                                                }

                                        }

                                }




                        }

                }

                #ifdef HIDDEN_LIST_DEBUG
                        cout << "**************** SCOPE END *****************************************************"<<endl;
                        cout << endl;
                #endif

                return InheritedAttributeForNamespacesAndClassTraversal(inheritedAttribute.depth + 1);

        }

     // DQ (8/3/2007): accumulate the elapsed time for this function
        TimingPerformance::accumulateTime ( startTime, accumulatedEvaluateInheritedAttribute_1_Time, accumulatedEvaluateInheritedAttribute_1_Calls );

        return InheritedAttributeForNamespacesAndClassTraversal(inheritedAttribute.depth);

}

//  -> will be used to pop the stack of Scopes
SynthesizedAttributeForNamespacesAndClassTraversal NamespacesAndClassTraversal :: evaluateSynthesizedAttribute(SgNode* n, InheritedAttributeForNamespacesAndClassTraversal inheritedAttribute, SynthesizedAttributesList SynthesizedAttributeForNamespacesAndClassTraversalList) {

        // if scope statement reached -> pop stack of scopes
        SgScopeStatement* SgScope = isSgScopeStatement(n);
             if (SgScope != NULL) {

                // Robert Preissl, June 20 2007: ad Occurence of using directives (and decl.) in the source-code relative to other declarations:
                //  if leaving a scope we pop the stack frame for the declarationset
                if( isSgGlobal(n) /*(int)this->stack_of_declarations.size() == 0 */ ) {

                        #ifdef HIDDEN_LIST_DEBUG
                                cout << ">> in evaluateSynthesizedAttribute: isSgGlobal!!"<< endl;
                        #endif

                }
                else {

                        if( this->collection_mode == 0 ) {

                                #ifdef HIDDEN_LIST_DEBUG
                                        cout << ">> in evaluateSynthesizedAttribute: stack_of_declarations.pop_back at size: "<< this->stack_of_declarations.size() << endl;
                                #endif

                                this->stack_of_declarations.pop_back();

                                //this->declarations_before_using.clear();
                                #ifdef HIDDEN_LIST_DEBUG
                                        cout << ">> END in evaluateSynthesizedAttribute: stack_of_declarations.pop_back!!"<< endl;
                                #endif

                        }
                        else if( this->collection_mode == 1 ) {

                                // Robert Preissl, June 26 2007 : clear will be done when a new scope is encountered
                                // this->declarations_before_using.clear();
                                this->stack_of_declarations.pop_back();

                                #ifdef HIDDEN_LIST_DEBUG
                                        cout << ">> END in evaluateSynthesizedAttribute: stack_of_declarations.pop_back!!"<< endl;
                                #endif

                        }
                        else {

                                cout << "ERROR: wrong collection mode! " << endl;

                        }

                }


        }

        /*// returned objects are not important for the computation of the hiddenlist
        if (SynthesizedAttributeForNamespacesAndClassTraversalList.size() == 0) {

                SynthesizedAttributeForNamespacesAndClassTraversal test;
                return test;

        }
        else {

                return SynthesizedAttributeForNamespacesAndClassTraversalList[0];
                
        }*/

        return 42;

}



// DQ (5/8/2007): Added this function to wrap up details of how to call the hidden list work
// Robert Preissl, July 9 2007 : added 1st paramter because this function is called in astPostProcessing for SgFile
void buildNamespacesAndClassHashMaps( SgFile* project  /*SgProject* project*/, NamespacesAndClassTraversal& namespaces_and_class_traversal ) {

  // DQ (8/1/2007): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("Build namespaces and class hash maps:");

        #ifdef HIDDEN_LIST_DEBUG
                cout << " >>>>>>>>>>>>  !!!  Begin of NamespacesAndClassTraversal Procedure" << endl;
        #endif

        int start_depth = 0;

        InheritedAttributeForNamespacesAndClassTraversal inheritedAttribute(start_depth);
        // SynthesizedAttributeSgScopeStatement synthesizedAttribute();

        //NamespacesAndClassTraversal namespaces_and_class_traversal;

        // Robert Preissl, June 8: Redesign of HiddenList-Computation:
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
        //    the information of namespace Y hasn't been collected yet!

        // The other motivation for this redesign was a much more clear and clean design for later usages where e.g. someone
        // is only interested in Symbols of Namespaces or classes for a special analysis; so only the first traversal would be needed to run.


        //exampleTraversal.traverseInputFiles(project, inheritedAttribute);
        namespaces_and_class_traversal.traverse(project, inheritedAttribute);

        #ifdef HIDDEN_LIST_DEBUG
                cout << endl;
                cout << "NAMESPACES: " << endl;
                String_VectorOfNamespaceInformation_HashMapOutput(namespaces_and_class_traversal.NamespacesHashMap);

                cout << endl;
                cout << "CLASSES: " << endl;
                StringVectorHashMapOutput(namespaces_and_class_traversal.ClassHashMap);
        #endif

}

} // namespace

/*
int main ( int argc, char* argv[] ) {

        SgProject* project = frontend(argc,argv);
        ROSE_ASSERT (project != NULL);

        NamespacesAndClassSymbolCollection::NamespacesAndClassTraversal namespaces_and_class_traversal;

        NamespacesAndClassSymbolCollection::buildNamespacesAndClassHashMaps( project, namespaces_and_class_traversal );


        return 0;

}
*/


// DQ (5/21/2013): I think that we can and should remove this file.
// for the moment I will comment out the whole file.
#endif
