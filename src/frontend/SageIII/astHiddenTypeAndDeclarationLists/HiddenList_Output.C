#if 0
// DQ (5/21/2013): I think that we can and should remove this file.
// for the moment I will comment out the whole file.

// Implementation of the Hidden List Output routines
// Robert Preissl
// Last modified : April 16, 2007

// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "HiddenList.h"
#include "HiddenList_Intersection.h"
#include "HiddenList_Output.h"

// DQ (5/8/2007): This is now in the rose.h header file
// #include "HiddenList.h"


using namespace std;

namespace Hidden_List_Computation {

void VectorOutput(Vector_Of_SymbolInformation X)
{
        Vector_Of_SymbolInformation::iterator it;

        for(it = X.begin(); it != X.end(); ++it) {

                cout << (*it)->symbol_pointer << " / name: " << (*it)->name << " / sage-class-name: " << ((*it)->symbol_pointer)->sage_class_name() << endl;

        }

        cout << endl;
}


void StackOfSymbolTableOutput(vector<ScopeStackEntry>& VectorScopeStack, int depth) {

        ScopeStackEntry::iterator it;
        SymbolHashMap::iterator value_it;
        int counter = 0;

        for(int v = 0; v < depth; ++v ) {

                cout << "Scope-Stack[" << v << "]: " << endl;

                //cout << "NR. OF DIFFERENT HIDDEN SYMBOLS: " << SSE.size() << endl;

                for(it = VectorScopeStack[v].begin(); it != VectorScopeStack[v].end(); ++it) {

                        // cout << "Symbol: " << it->first << " with adress(es) : ";
                        for(value_it = (it->second).begin(); value_it != (it->second).end(); ++value_it) {

                                ROSE_ASSERT(value_it->first != NULL);

                                //if( (*value_it).scope_can_be_named) {
                                if( /*(value_it->second).scope_can_be_named */ true ) {
                                        cout << "Symbol " << it->first << "; with SgSymbol-ClassName " << ( (value_it)->first)->class_name() << " with SgSymbol-Address: " << ((value_it)->first) << " / Valid: " << (value_it)->second->is_already_proofed_to_be_valid << endl;
                                        counter++;
                                }
                                else {
                                        cout << "Unaccessible entry " << it->first << "; with SgSymbol-ClassName " << ((value_it)->first)->class_name() << " with SgSymbol-Address: " << ((value_it)->first) << " / Valid: " << (value_it)->second->is_already_proofed_to_be_valid << endl;
                                        counter++;
                                }

                                #ifdef HIDDEN_LIST_DEBUG
                                        /*if(isSgTemplateSymbol(value_it->first)) {

                                                SgTemplateSymbol* template_symbol = isSgTemplateSymbol(value_it->first);

                                                SgTemplateDeclaration* template_decl = template_symbol->get_declaration();

                                                if(template_decl !=  NULL) {

                                                        string file_name;
                                                        int source_line;

                                                        file_name = template_decl->get_file_info()->get_filename();
                                                        source_line = template_decl->get_file_info()->get_line();

                                                        cout << " >> isSgTemplateSymbol: FileName:"  << file_name << " / line: "  << source_line << endl;

                                                }

                                        }*/
                                #endif

                                #ifdef HIDDEN_LIST_DEBUG
                                        /*if(isSgFunctionSymbol(value_it->first)) {

                                                SgFunctionSymbol* func_symbol = isSgFunctionSymbol(value_it->first);

                                                SgFunctionDeclaration* func_decl = func_symbol->get_declaration();

                                                if(func_decl !=  NULL) {

                                                        string file_name;
                                                        int source_line;

                                                        file_name = func_decl->get_file_info()->get_filename();
                                                        source_line = func_decl->get_file_info()->get_line();

                                                        cout << " >> isSgFunctionSymbol: FileName:"  << file_name << " / line: "  << source_line << endl;

                                                }

                                        }*/
                                #endif

                        }

                }

                cout << endl;


        }

}


void HiddenListOutput(ScopeStackEntry SSE) {

        ScopeStackEntry::iterator it;
        SymbolHashMap::iterator value_it;
        int counter = 0;

        cout << "NR. OF DIFFERENT HIDDEN SYMBOLS: " << SSE.size() << endl;
        for(it = SSE.begin(); it != SSE.end(); ++it) {

        // DQ (5/8/2007): Added assertion for valid pointer
        //ROSE_ASSERT(it->first != NULL);

            cout << "Symbol: " << it->first << " with adress(es) : ";
            for(value_it = (it->second).begin(); value_it != (it->second).end(); ++value_it) {

                    ROSE_ASSERT(value_it->first != NULL);

                    //if( (*value_it).scope_can_be_named) {
                    if( (value_it->second)->scope_can_be_named ) {
                                cout << "Hidden entry " << it->first << "; with SgSymbol-ClassName " << ((value_it)->first)->class_name() << " with SgSymbol-Address: " << ((value_it)->first) << " / Valid: " << (value_it)->second->is_already_proofed_to_be_valid << endl;
                                counter++;
                        }
                        else {
                                cout << "Unaccessible entry " << it->first << "; with SgSymbol-ClassName " << ((value_it)->first)->class_name() << " with SgSymbol-Address: " << ((value_it)->first) << " / Valid: " << (value_it)->second->is_already_proofed_to_be_valid << endl;
                                counter++;
                        }

                        #ifdef HIDDEN_LIST_DEBUG
                                if(isSgTemplateSymbol(value_it->first)) {

                                        SgTemplateSymbol* template_symbol = isSgTemplateSymbol(value_it->first);

                                        SgTemplateDeclaration* template_decl = template_symbol->get_declaration();

                                        if(template_decl !=  NULL) {

                                                string file_name;
                                                int source_line;

                                                file_name = template_decl->get_file_info()->get_filename();
                                                source_line = template_decl->get_file_info()->get_line();

                                                cout << " >> isSgTemplateSymbol: FileName:"  << file_name << " / line: "  << source_line << endl;

                                        }

                                }
                        #endif

                        #ifdef HIDDEN_LIST_DEBUG
                                if(isSgFunctionSymbol(value_it->first)) {

                                        SgFunctionSymbol* func_symbol = isSgFunctionSymbol(value_it->first);

                                        SgFunctionDeclaration* func_decl = func_symbol->get_declaration();

                                        if(func_decl !=  NULL) {

                                                string file_name;
                                                int source_line;

                                                file_name = func_decl->get_file_info()->get_filename();
                                                source_line = func_decl->get_file_info()->get_line();

                                                cout << " >> isSgFunctionSymbol: FileName:"  << file_name << " / line: "  << source_line << endl;

                                        }

                                }
                        #endif

                }

        }

        cout << " -> Number of entries in hidden list: " << counter << endl;
        cout << endl;

}

// Robert Preissl, May, 10 2007 : added this function to divide the hiddenlist into subsets
void OutputSubsetsOfHiddenList(SetSgSymbolPointers AvailableHidden_Functions_Variables, SetSgSymbolPointers AvailableHidden_Types, SetSgSymbolPointers ElaboratingForcingHidden_Types) {

        SetSgSymbolPointers::const_iterator it;

        if(!AvailableHidden_Functions_Variables.empty()) {
                cout << "    AvailableHidden_Functions_Variables: " << endl;
                for(it = AvailableHidden_Functions_Variables.begin(); it != AvailableHidden_Functions_Variables.end(); ++it) {
                
                        cout << "    SgSymbol: " << (*it)->get_name().str() << " / with address: " << *it << endl;
                        //#ifdef HIDDEN_LIST_DEBUG
                                if(isSgTemplateSymbol(*it)) {

                                        SgTemplateSymbol* template_symbol = isSgTemplateSymbol(*it);

                                        SgTemplateDeclaration* template_decl = template_symbol->get_declaration();

                                        if(template_decl !=  NULL) {

                                                string file_name;
                                                int source_line;

                                                file_name = template_decl->get_file_info()->get_filename();
                                                source_line = template_decl->get_file_info()->get_line();

                                                cout << " >> isSgTemplateSymbol: FileName:"  << file_name << " / line: "  << source_line << endl;

                                        }

                                }
                        //#endif

                        //#ifdef HIDDEN_LIST_DEBUG
                                if(isSgFunctionSymbol(*it)) {

                                        SgFunctionSymbol* func_symbol = isSgFunctionSymbol(*it);

                                        SgFunctionDeclaration* func_decl = func_symbol->get_declaration();

                                        if(func_decl !=  NULL) {

                                                string file_name;
                                                int source_line;

                                                file_name = func_decl->get_file_info()->get_filename();
                                                source_line = func_decl->get_file_info()->get_line();

                                                cout << " >> isSgFunctionSymbol: FileName:"  << file_name << " / line: "  << source_line << endl;

                                        }

                                }
                        //#endif
                }
                cout << " ********* "<< endl;
        }

        if(!AvailableHidden_Types.empty()) {
                cout << "    AvailableHidden_Types: " << endl;
                for(it = AvailableHidden_Types.begin(); it != AvailableHidden_Types.end(); ++it) {
                        cout << "    SgSymbol: " << (*it)->get_name().str() << " / with address: " << *it << endl;
                        //#ifdef HIDDEN_LIST_DEBUG
                                if(isSgTemplateSymbol(*it)) {

                                        SgTemplateSymbol* template_symbol = isSgTemplateSymbol(*it);

                                        SgTemplateDeclaration* template_decl = template_symbol->get_declaration();

                                        if(template_decl !=  NULL) {

                                                string file_name;
                                                int source_line;

                                                file_name = template_decl->get_file_info()->get_filename();
                                                source_line = template_decl->get_file_info()->get_line();

                                                cout << " >> isSgTemplateSymbol: FileName:"  << file_name << " / line: "  << source_line << endl;

                                        }

                                }
                        //#endif

                        //#ifdef HIDDEN_LIST_DEBUG
                                if(isSgFunctionSymbol(*it)) {

                                        SgFunctionSymbol* func_symbol = isSgFunctionSymbol(*it);

                                        SgFunctionDeclaration* func_decl = func_symbol->get_declaration();

                                        if(func_decl !=  NULL) {

                                                string file_name;
                                                int source_line;

                                                file_name = func_decl->get_file_info()->get_filename();
                                                source_line = func_decl->get_file_info()->get_line();

                                                cout << " >> isSgFunctionSymbol: FileName:"  << file_name << " / line: "  << source_line << endl;

                                        }

                                }
                        //#endif
                }
                cout << " ********* "<< endl;
        }

        if(!ElaboratingForcingHidden_Types.empty()) {
                cout << "    ElaboratingForcingHidden_Typess: " << endl;
                for(it = ElaboratingForcingHidden_Types.begin(); it != ElaboratingForcingHidden_Types.end(); ++it) {
                        cout << "    SgSymbol: " << (*it)->get_name().str() << " / with address: " << *it << endl;
                        //#ifdef HIDDEN_LIST_DEBUG
                                if(isSgTemplateSymbol(*it)) {

                                        SgTemplateSymbol* template_symbol = isSgTemplateSymbol(*it);

                                        SgTemplateDeclaration* template_decl = template_symbol->get_declaration();

                                        if(template_decl !=  NULL) {

                                                string file_name;
                                                int source_line;

                                                file_name = template_decl->get_file_info()->get_filename();
                                                source_line = template_decl->get_file_info()->get_line();

                                                cout << " >> isSgTemplateSymbol: FileName:"  << file_name << " / line: "  << source_line << endl;

                                        }

                                }
                        //#endif

                        //#ifdef HIDDEN_LIST_DEBUG
                                if(isSgFunctionSymbol(*it)) {

                                        SgFunctionSymbol* func_symbol = isSgFunctionSymbol(*it);

                                        SgFunctionDeclaration* func_decl = func_symbol->get_declaration();

                                        if(func_decl !=  NULL) {

                                                string file_name;
                                                int source_line;

                                                file_name = func_decl->get_file_info()->get_filename();
                                                source_line = func_decl->get_file_info()->get_line();

                                                cout << " >> isSgFunctionSymbol: FileName:"  << file_name << " / line: "  << source_line << endl;

                                        }

                                }
                        //#endif
                }
        }

}

void OutputVectorStringSetHashMap(vector<ScopeStackEntry> VSSE) {
        for(int i = 0; i < (int)VSSE.size(); ++i) {
                cout << "VectorScopeStack["<<i<<"] : "<< endl;
                HiddenListOutput(VSSE[i]);
        }
        cout << endl;
        cout << endl;
}

void StringVectorHashMapOutput(StringVectorHashMap &SVHM) {

        StringVectorHashMap::const_iterator c_it;

        cout << "Size: " << SVHM.size() << endl;

        for(c_it = SVHM.begin(); c_it != SVHM.end(); ++c_it) {
                cout << "Class Name: " << c_it->first << " [";
                Vector_Of_SymbolInformation v = c_it->second;
                for(int i = 0; i < (int)v.size(); ++i) {
                        cout << v[i]->name << ", ";
                }
                cout << "]" << endl;
        }
}

void String_VectorOfNamespaceInformation_HashMapOutput(String_VectorOfNamespaceInformation_HashMap &Out) {

        String_VectorOfNamespaceInformation_HashMap::iterator it_1;
        VectorOfNamespaceInformation::iterator it_2;

        for(it_1 = Out.begin(); it_1 != Out.end(); ++it_1) {

                cout << "Namespace [" << it_1->first << "] : " << endl;

                for(it_2 = (it_1->second).begin(); it_2 != (it_1->second).end(); ++it_2 ) {

                        Vector_Of_SymbolInformation v = (*it_2).VectorOfSymbolInformation;
                        cout << "        { ";
                        for(int i = 0; i < (int)v.size(); ++i) {
                                cout << v[i]->name << ",";
                        }
                        cout << " }" << endl;

                }

        }

}

void OutputSetSgDeclarationStatements(SetSgDeclarationStatements& X) {

        SetSgDeclarationStatements::iterator set_it;

        for(set_it = X.begin(); set_it != X.end(); ++set_it) {

                cout << "        Name: " << GetSgNameOutOfSgDeclarationStatement((*set_it)).str() << " | address: " << (*set_it) << endl;

        }

        cout << endl;

}


void UsingDirectiveStatement_SetSgDeclarationStatements_HashMap__Output(UsingDirectiveStatement_SetSgDeclarationStatements_HashMap& X) {

        UsingDirectiveStatement_SetSgDeclarationStatements_HashMap::iterator it;
        SgNamespaceDeclarationStatement* namesp_decl_stat;

        cout << "Size of UsingDirRelativeToDeclarations: " << X.size() << endl;

        for( it = X.begin(); it != X.end(); ++it ) {

                namesp_decl_stat = it->first->get_namespaceDeclaration();
                string used_namespace = namesp_decl_stat->get_qualified_name().str();

                cout << "        Declarations before using-namespace-directive : " << used_namespace << endl;

                OutputSetSgDeclarationStatements( it->second );

        }

}


void UsingDeclarationStatement_SetSgDeclarationStatements_HashMap__Output(UsingDeclarationStatement_SetSgDeclarationStatements_HashMap& X) {

        UsingDeclarationStatement_SetSgDeclarationStatements_HashMap::iterator it;

        cout << "Size of UsingDeclRelativeToDeclarations: " << X.size() << endl;

        for( it = X.begin(); it != X.end(); ++it ) {

                cout << "        Declarations before using-namespace-declaration : "  << endl;

                OutputSetSgDeclarationStatements( it->second );

        }

}


void OutputVectorSetSgDeclarationStatements (vector<SetSgDeclarationStatements>* VSS) {

        SetSgDeclarationStatements::iterator set_it;

        for(int i = 0; i < (int)((*VSS).size()); ++i) {
                cout << "Stack Frame of valid scopes at depth: " << i << ": ";
                for(set_it = (*VSS)[i].begin(); set_it != (*VSS)[i].end(); ++set_it) {
                        cout << "Name: " << (*set_it)->get_mangled_name().str() << " | address: " << (*set_it) << " *** ";
                }
                cout << endl;
        }
        cout << endl;cout << endl;

}

void StackSetSgDeclarationStatements__Output(const StackSetSgDeclarationStatements& X ) {

        int size_of_stack = 0;
        SetSgDeclarationStatements::const_iterator set_it;


        size_of_stack = (int)X.size();

        for(int i = 0; i < size_of_stack; ++i ) {

                cout << "        Stack-Frame [" << i << "] : {";

                // print set
                for( set_it = X[i].begin(); set_it != X[i].end(); ++set_it ) {

                        cout << "(" << GetSgNameOutOfSgDeclarationStatement( *set_it ).str() << "/" << *set_it<< ")" << " ,  ";

                }

                cout << "}" << endl;

        }

}


void StackSetSgDeclarationStatements__Output(const StackSetSgDeclarationStatements& X, SgNamespaceDeclarationStatement* namesp_decl_stat ) {

        int size_of_stack = 0;
        SetSgDeclarationStatements::const_iterator set_it;

        string used_namespace = namesp_decl_stat->get_qualified_name().str();

        cout << "Declarations before using-namespace : " << used_namespace << endl;

        size_of_stack = (int)X.size();

        for(int i = 0; i < size_of_stack; ++i ) {

                cout << "        Stack-Frame [" << i << "] : {";

                // print set
                for( set_it = X[i].begin(); set_it != X[i].end(); ++set_it ) {

                        cout << GetSgNameOutOfSgDeclarationStatement( *set_it ).str() << ", ";

                }

                cout << "}" << endl;

        }

}


void UsingDirectiveStatement_LinkedListStackSetSgDeclarationStatements_HashMap__Output (UsingDirectiveStatement_LinkedListStackSetSgDeclarationStatements_HashMap& X) {

        UsingDirectiveStatement_LinkedListStackSetSgDeclarationStatements_HashMap::const_iterator hashmap_it;
        SgDeclarationStatement* temp_PreviousLinkedListStackSetSgDeclarationStatements;
        SgUsingDirectiveStatement* using_dir_stmt = 0;
        SgUsingDeclarationStatement* using_decl_stmt = 0;
        bool using_dir = false;


        cout << "Size of UsingDirRelativeToDeclarations: " << X.size() << endl;

        for( hashmap_it = X.begin(); hashmap_it != X.end(); ++hashmap_it ) {

                StackSetSgDeclarationStatements__Output(hashmap_it->second.CurrentStackSetSgDeclarationStatements, hashmap_it->first->get_namespaceDeclaration());

                // if there are more than one using in the code:
                if( hashmap_it->second.PreviousLinkedListStackSetSgDeclarationStatements != NULL ) {

                        cout << " + (linked with other usings..) " << endl;

                        temp_PreviousLinkedListStackSetSgDeclarationStatements = hashmap_it->second.PreviousLinkedListStackSetSgDeclarationStatements;

                        // I could also do this recursively, but I think its faster to do not
                        while( temp_PreviousLinkedListStackSetSgDeclarationStatements != NULL ) {

                                if( isSgUsingDirectiveStatement(temp_PreviousLinkedListStackSetSgDeclarationStatements) ) {

                                        using_dir_stmt = isSgUsingDirectiveStatement(temp_PreviousLinkedListStackSetSgDeclarationStatements);
                                        using_dir = true;

                                }
                                else if( isSgUsingDeclarationStatement(temp_PreviousLinkedListStackSetSgDeclarationStatements) ) {

                                        using_decl_stmt = isSgUsingDeclarationStatement(temp_PreviousLinkedListStackSetSgDeclarationStatements);
                                        using_dir = false;

                                }
                                else { cout << "ERROR in  UsingDirectiveStatement_LinkedListStackSetSgDeclarationStatements_HashMap__Output" << endl; }

                                if(using_dir) {

                                        StackSetSgDeclarationStatements__Output( X[using_dir_stmt].CurrentStackSetSgDeclarationStatements, using_dir_stmt->get_namespaceDeclaration() );

                                }
                                /*else {

                                        StackSetSgDeclarationStatements__Output( X[temp_PreviousLinkedListStackSetSgDeclarationStatements].CurrentStackSetSgDeclarationStatements, using_decl_stmt->get_namespaceDeclaration() );

                                }*/

                                cout << " + " << endl;

                                temp_PreviousLinkedListStackSetSgDeclarationStatements = X[using_dir_stmt].PreviousLinkedListStackSetSgDeclarationStatements;

                        }

                }

                cout << endl;

        }

}

} // namespace


// DQ (5/21/2013): I think that we can and should remove this file.
// for the moment I will comment out the whole file.
#endif

