// Implementation of the Hidden List Intersection Procedure
// Robert Preissl
// Last modified : April 16, 2007
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "HiddenList.h"
#include "HiddenList_Intersection.h"
#include "HiddenList_Output.h"

// DQ (5/8/2007): These are now in the rose.h header file
// #include "DEBUG_HiddenList_Intersection.h"
// #include "DEBUG_HiddenList.h"

using namespace std;

namespace Hidden_List_Computation {


        // DQ (8/3/2007): Performance information for accumulating the performance timing reesults
#ifdef __timespec_defined
        // #error "__timespec_defined IS defined"
        // typedef timespec timer_type;
        // struct { timespec time; long accumulatedTime; } timer_type;
#else
        // #error "__timespec_defined is NOT defined"
        // struct { long startTimeTick; long endTimeTick; long accumulatedTime; } timer_type;
#endif

        // timer_type AccumulatedIntersectionTime;

        double accumulatedIntersectionTime  = 0.0;
        double accumulatedIntersectionCalls = 0.0;

        double accumulatedIntersectionFindTime  = 0.0;
        double accumulatedIntersectionFindCalls = 0.0;

        double accumulatedIntersectionAfterFindTime  = 0.0;
        double accumulatedIntersectionAfterFindCalls = 0.0;

        double accumulatedIntersectionScopeTime [6] = { 0,0,0,0,0,0 };
        double accumulatedIntersectionScopeCalls[6] = { 0,0,0,0,0,0 };

        double accumulatedIntersectionFillScopeStack_1_Time = 0.0;
        double accumulatedIntersectionFillScopeStack_1_Calls = 0.0;
        double accumulatedIntersectionFillScopeStack_2_Time = 0.0;
        double accumulatedIntersectionFillScopeStack_2_Calls = 0.0;


        // look in Hash_Map if one (or more) of the symbols in vector exists
        // -> if yes: update the Hash_Map-Value with the current pointer (will be needed in next ScopeStatement), !! elements with same Key can never have same pointer (scope)
        // -> if no: add a new Key (and also the corresponding value) to the Hash_Map

        // Robert Preissl, June 1 2007, call Vector_Of_SymbolInformation per reference

        void Intersection (
                std::vector<ScopeStackEntry>* p_VSHM,
                int depth,
                Vector_Of_SymbolInformation &local_scope,
                SetSgSymbolPointers &AvailableHidden_Functions_Variables,
                SetSgSymbolPointers &AvailableHidden_Types,
                SetSgSymbolPointers &ElaboratingForcingHidden_Types,
                UsingDirectiveStatement_LinkedListStackSetSgDeclarationStatements_HashMap& UsingDirRelativeToDeclarations,
                UsingDeclarationStatement_LinkedListStackSetSgDeclarationStatements_HashMap& UsingDeclRelativeToDeclarations,
                UsingDirectiveStatement_SetSgDeclarationStatements_HashMap& UsingDirRelativeToDeclarations_2,
                UsingDeclarationStatement_SetSgDeclarationStatements_HashMap& UsingDeclRelativeToDeclarations_2,
                SetSgUsingDirectiveStatementsWithSgScopeStatement& UsingDirectivesSet,
                SetSgUsingDeclarationWithScopeWithSgScopeStatement& UsingDeclarationsSet
                )
        {

                // DQ (8/3/2007): Start time for this function
                TimingPerformance::time_type startTime;
                TimingPerformance::startTimer(startTime);

                TimingPerformance::time_type startTimeScope[6];

                // Robert Preissl, August 2 2007 : save iterator that points to local_scope.begin() and to local_scope.end(), so that it doesn't need to be computed all the time
                Vector_Of_SymbolInformation::iterator it_begin = local_scope.begin();
                Vector_Of_SymbolInformation::iterator it_end = local_scope.end();

                Vector_Of_SymbolInformation::iterator it;

                ScopeStackEntry::iterator find_it;
                SymbolHashMapValue symbol_hashmap_value;

                bool no_insert_into_hiddenlist = false;

                // SymbolHashMap temp;

                SgSymbol* it_symbol_pointer;

                // Robert Preissl, July 9 2007 : Performance Tuning : we don't copy the previous stack frame (in CreateNewStackFrame) in the current frame any longer
                //  -> therefore looking into (*p_VSHM)[depth] isn't enough, have to iterate through the whole stack to find an element
                for(int depth_it = depth; depth_it >= 0; --depth_it) {


                        switch (depth_it)
                        {
                        case 0: TimingPerformance::startTimer(startTimeScope[0]); break;
                        case 1: TimingPerformance::startTimer(startTimeScope[1]); break;
                        case 2: TimingPerformance::startTimer(startTimeScope[2]); break;
                        case 3: TimingPerformance::startTimer(startTimeScope[3]); break;
                        case 4: TimingPerformance::startTimer(startTimeScope[4]); break;
                        case 5: TimingPerformance::startTimer(startTimeScope[5]); break;
                        default: {}
                        }

                        // Robert Preissl, August 2 2007 : Performance Tuning : try to optimize "find_it = (*p_VSHM)[depth_it].find( (*it)->name );"
                        //  -> so drag out the parts that are non dependent on it:
                        ScopeStackEntry& refr_Scope = (*p_VSHM)[depth_it];

                        // for each symbol from the current symbol table..
                        for(it = it_begin; it != it_end; ++it) {

                                it_symbol_pointer = (*it)->symbol_pointer;

                                // .. look in the current scope if there are symbols with the same name

                                TimingPerformance::time_type startTimeFind;
                                TimingPerformance::startTimer(startTimeFind);

                                // find_it = (*p_VSHM)[depth_it].find( (*it)->name );
                                find_it = refr_Scope.find( (*it)->name );

                                TimingPerformance::accumulateTime ( startTimeFind, accumulatedIntersectionFindTime, accumulatedIntersectionFindCalls );

                                TimingPerformance::time_type startTimeAfterFind;
                                TimingPerformance::startTimer(startTimeAfterFind);

                                // found it
                                if(find_it != (*p_VSHM)[depth_it].end()) {

                                        // Robert Preissl, August 2 2007 : per reference
                                        const SymbolHashMap& symbol_hashmap = find_it->second;;

                                        SymbolHashMap:: const_iterator it_symbol_hashmap;
                                        SgSymbol* p_symbol;
                                        bool it_symbol_hashmap__is_already_proofed_to_be_valid;
                                        bool it_symbol_hashmap__symbol_of_class;
                                        bool it_symbol_hashmap__is_using_decl_in_class;

#ifdef HIDDEN_LIST_DEBUG
                                        // just to find out (in the case of a template symbol that is hiding) what (File Info) is hiding
                                        if(isSgTemplateSymbol(it_symbol_pointer)) {

                                                SgTemplateSymbol* template_symbol = isSgTemplateSymbol(it_symbol_pointer);
                                                SgTemplateDeclaration* template_decl = template_symbol->get_declaration();

                                                if(template_decl !=  NULL) {

                                                        string file_name;
                                                        int source_line;
                                                        file_name = template_decl->get_file_info()->get_filename();
                                                        source_line = template_decl->get_file_info()->get_line();

                                                        cout << " >>>> isSgTemplateSymbol: FileName:"  << file_name << " / line: "  << source_line << endl;

                                                }

                                        }
#endif

#ifdef HIDDEN_LIST_DEBUG
                                        cout << "SIZE symbol_hashmap: " << symbol_hashmap.size() << " for finding " << (*it)->name << endl;
#endif

                                        // Robert Preissl, August 2 2007 : performance tuning
                                        SymbolHashMap:: const_iterator it_symbol_hashmap__end = symbol_hashmap.end();

                                        // iterate through all entries of the symbol_hashmap (e.g. x occured in several scopes)
                                        for(it_symbol_hashmap = symbol_hashmap.begin(); it_symbol_hashmap != it_symbol_hashmap__end; ++it_symbol_hashmap) {

                                                ROSE_ASSERT(it_symbol_hashmap->first != NULL);

                                                // Robert Preissl, July 10 2007 : moved inside the loop
                                                no_insert_into_hiddenlist = false;

                                                p_symbol = it_symbol_hashmap->first;
                                                it_symbol_hashmap__is_already_proofed_to_be_valid = (it_symbol_hashmap->second)->is_already_proofed_to_be_valid;
                                                it_symbol_hashmap__symbol_of_class = (it_symbol_hashmap->second)->symbol_of_class;
                                                it_symbol_hashmap__is_using_decl_in_class = (it_symbol_hashmap->second)->is_using_decl_in_class;


                                                if(p_symbol != NULL) {

                                                        // Robert Preissl, June 22 2007 : There is a case where I can omit the intersection -> when both symbols (hiding & hidden
                                                        //        symbols) have a "scope_can_be_named - flag = false", because there will be no entry in one of the three lists
                                                        //  Question may occur: why is not "scope_can_be_named - flag = false" at p_symbol enough for omitting?
                                                        //  Answer: because there is a case in the intersection (look down), where p_symbol can also be the symbol that is hiding!!
                                                        //    (comment from Robert Preissl, June 7 2007)
                                                        //
                                                        // Motivating example:
                                                        //        int a;
                                                        //        {
                                                        //                int x;
                                                        //                {
                                                        //                        double x;
                                                        //                }
                                                        //        }
                                                        if( !it_symbol_hashmap->second->scope_can_be_named && !(*it)->scope_can_be_named) {

                                                                //        cout << "No intersection required, because no insert into one of the three hiddenlists!!" << endl;
                                                                no_insert_into_hiddenlist = true;

                                                        }


                                                        // Robert Preissl, June 22 2007 : in addition to UsingDirRelativeToDeclarations
                                                        //   (and also for using decl.) we keep a special data structure that keeps
                                                        //   track of where using directives (and decl.) occur in the source-code relative to other declarations.
                                                        // -> due to query (is there a using dir./decl. in my scope) we update the scope before the hiddenlist-computation starts,
                                                        //        so e.g. int d (case 3) thinks that it hides (2)X::int a, what is wrong.

                                                        //        namespace Y { double y; }
                                                        //        namespace X {
                                                        //                int a, c, d, e, f; // case 0: they shouldn't hide themselves if we would have a using namespace X in global scope!
                                                        //                float y;
                                                        //        }
                                                        //        void foo(int wwwwwwwwwww) {
                                                        //                float a, b; // case 1: float a will be hidden by X::int a
                                                        //                {
                                                        //                        using namespace Y; // case 2a: Y::double y will be hidden by X::float y
                                                        //                        double c; // case 2: double c will be hidden by X::int c
                                                        //                        (1) using namespace X;
                                                        //                        {
                                                        //                                int d; // case 3: int d will hide (1)X::int d, BUT not (2)X::int d
                                                        //                                // using namespace Y; case 4: Y::double y hides (1)X::float y but NOT (2)X::float y
                                                        //                        }
                                                        //                        (2) using namespace X;
                                                        //                        double f; // ad case 2: will not be hidden by X::int f, BUT double f hides X::int f !!!!
                                                        //                }
                                                        //                double e; // ad case 1: will not be hidden by X::int e
                                                        //        }

#ifdef HIDDEN_LIST_DEBUG
                                                        if( (*it)->symbol_is_from_using || it_symbol_hashmap->second->symbol_is_from_using) {

                                                                SgDeclarationStatement* find_decl_stmt =  GetSgDeclarationStatementOutOfSgSymbol(it_symbol_pointer);

                                                                if( (*it)->symbol_is_from_using && !(it_symbol_hashmap->second->symbol_is_from_using) ) {

                                                                        cout << " <<A>> Hiding Symbol: " << GetSgNameOutOfSgDeclarationStatement(find_decl_stmt).str() << " in depth: " << (*it)->depth_of_using  << " AND hidden symbol in depth: "<< it_symbol_hashmap->second->depth_of_using << endl;

                                                                }
                                                                else if( !((*it)->symbol_is_from_using) && it_symbol_hashmap->second->symbol_is_from_using ) {

                                                                        cout << " <<B>> Hiding Symbol: " <<GetSgNameOutOfSgDeclarationStatement(find_decl_stmt).str() << " in depth: " << (*it)->depth_of_using << " AND hidden symbol in depth: "<< it_symbol_hashmap->second->depth_of_using << endl;

                                                                }
                                                                else if( (*it)->symbol_is_from_using && it_symbol_hashmap->second->symbol_is_from_using ) {

                                                                        cout << " <<C>> " << endl;

                                                                }
                                                                else {
                                                                        cout << "ERROR A" << endl;
                                                                }

                                                        }
#endif

                                                        // Robert Preissl, July 10 2007 : ad comment from Robert Preissl, June 22 2007 : don't need to do
                                                        //   anything if we already know that there will not be an insert into the hidden-list
                                                        if( !no_insert_into_hiddenlist ) {

                                                                // Robert Preissl, June 27 2007 : comment : depth is (or should be) the same as (*it).depth_of_using

                                                                // case 0 : ad example above: we have updated the global scope with int a before we start intersection
                                                                //     -> so in scope namespace X int a would hide itself what is not true!!
                                                                if( p_symbol == it_symbol_pointer ) {

                                                                        no_insert_into_hiddenlist = true;

#ifdef HIDDEN_LIST_DEBUG
                                                                        cout << "case 0: No Insert! "<< endl;
#endif

                                                                }
                                                                // case 1: (possibly) hidden symbol is not from a using stmt AND depth of hidden symbol is < as depth of hiding symbol
                                                                else if( !(it_symbol_hashmap->second->symbol_is_from_using) && (*it)->symbol_is_from_using && ( it_symbol_hashmap->second->depth_of_using  < depth) ) {

                                                                        // nothing sepcial to be done, normal intersection
                                                                        no_insert_into_hiddenlist = false;


#ifdef HIDDEN_LIST_DEBUG
                                                                        cout << "case 1: nothing special to be done " << endl;
#endif

                                                                }
                                                                // case 2: symbol that is hidden or symbol that is hiding is from a using stmt. AND depth is the same (therefore in the same scope)
                                                                else if( (it_symbol_hashmap->second->symbol_is_from_using || (*it)->symbol_is_from_using) && ( it_symbol_hashmap->second->depth_of_using  == (*it)->depth_of_using) ) {

                                                                        if(isSgFunctionSymbol(p_symbol) && isSgFunctionSymbol(it_symbol_pointer)) {

                                                                                // cout << "NO Insert, because :  isSgFunctionSymbol(p_symbol) && isSgFunctionSymbol(it_symbol_pointer " << endl;
                                                                                no_insert_into_hiddenlist = true;

                                                                        }

                                                                        // Robert Preissl, July 6 2007 : if symbol that is hiding and symbol that is hidden is in same scope &
                                                                        //  one of them is a Template & one of them included via using -> no insert into hidden-list
                                                                        // Motivating example: look at test143.C
                                                                        if( isSgTemplateSymbol(p_symbol) ) {

                                                                                no_insert_into_hiddenlist = true;


                                                                                /*SgTemplateSymbol* template_symbol = isSgTemplateSymbol(p_symbol);
                                                                                ROSE_ASSERT(template_symbol != NULL);
                                                                                SgTemplateDeclaration* template_decl = template_symbol->get_declaration();
                                                                                ROSE_ASSERT(template_decl != NULL);

                                                                                // class or nested class template
                                                                                if(template_decl->get_template_kind() == 1 || template_decl->get_template_kind() == 2) {

                                                                                if( isSgVariableSymbol(it_symbol_pointer) || isSgFunctionSymbol(it_symbol_pointer) || isSgTypedefSymbol(it_symbol_pointer) || isSgClassSymbol(it_symbol_pointer) || isSgEnumSymbol(it_symbol_pointer) || isSgEnumFieldSymbol(it_symbol_pointer) || isSgNamespaceSymbol(it_symbol_pointer) || isSgTemplateSymbol(it_symbol_pointer) ) {

                                                                                no_insert_into_hiddenlist = true;

                                                                                }

                                                                                }
                                                                                // function or member function template
                                                                                else if(template_decl->get_template_kind() == 3 || template_decl->get_template_kind() == 4 ) {

                                                                                if( isSgVariableSymbol(it_symbol_pointer) || isSgFunctionSymbol(it_symbol_pointer) || isSgTypedefSymbol(it_symbol_pointer) || isSgClassSymbol(it_symbol_pointer) || isSgEnumSymbol(it_symbol_pointer) || isSgEnumFieldSymbol(it_symbol_pointer) || isSgNamespaceSymbol(it_symbol_pointer) || isSgTemplateSymbol(it_symbol_pointer) ) {

                                                                                no_insert_into_hiddenlist = true;

                                                                                }

                                                                                }
                                                                                */

                                                                        }
                                                                        if( isSgTemplateSymbol(it_symbol_pointer) ) {

                                                                                no_insert_into_hiddenlist = true;

                                                                        }

                                                                        if(!no_insert_into_hiddenlist) {

                                                                                // Robert Preissl, June 28 2007 : !!!! Don't have to treat here the case of using-decl, because I can't have them
                                                                                //  in the same scope as something else with the same name!!!
                                                                                //  e.g.
                                                                                //        namespace X {
                                                                                //                int x;
                                                                                //                class C {
                                                                                //                        public:        int c;
                                                                                //                };
                                                                                //        }
                                                                                //        int main() {
                                                                                //                float C;
                                                                                //                using X::C; // causes an error : `C' is already declared in this scope
                                                                                //
                                                                                //                float x;
                                                                                //                // using namespace X; // would be ok
                                                                                //
                                                                                //                return 0;
                                                                                //        }

                                                                                bool case_2a = false;

                                                                                // case 2a : both symbol are from using..
                                                                                if( it_symbol_hashmap->second->symbol_is_from_using && (*it)->symbol_is_from_using ) {

                                                                                        case_2a = true;

#ifdef HIDDEN_LIST_DEBUG
                                                                                        cout << "case 2a:"<< endl;
#endif

                                                                                }

#ifdef HIDDEN_LIST_DEBUG
                                                                                cout << "case 2b:"<< endl;
#endif

                                                                                // find out which is the hiding element and which is the hidden element:

                                                                                //   so, first we find out the symbol that is NOT from a using:
                                                                                SgSymbol* temp_symbol_pointer_not_from_using;
                                                                                SgUsingDirectiveStatement* using_dir_stmt;

                                                                                if(!it_symbol_hashmap->second->symbol_is_from_using) {

                                                                                        temp_symbol_pointer_not_from_using = p_symbol;
                                                                                        using_dir_stmt = (*it)->si_using_dir;

                                                                                }
                                                                                else {

                                                                                        temp_symbol_pointer_not_from_using = it_symbol_pointer;
                                                                                        // will be taken in case 2a
                                                                                        using_dir_stmt = it_symbol_hashmap->second->si_using_dir;

                                                                                }

                                                                                //   then, find out its Decl.Stat. and take a look into the UsingDirRelativeToDeclarations_2 hash_map
                                                                                //        to find out if this decl. happens before or after the using:
                                                                                SgDeclarationStatement* find_decl_stmt = GetSgDeclarationStatementOutOfSgSymbol(temp_symbol_pointer_not_from_using);
                                                                                ROSE_ASSERT(find_decl_stmt != NULL);

                                                                                SetSgDeclarationStatements::iterator find_it_SetSgDeclarationStatements;

                                                                                if( case_2a == true ) {

                                                                                        find_it_SetSgDeclarationStatements = UsingDirRelativeToDeclarations_2[using_dir_stmt].find(it_symbol_hashmap->second->si_using_dir);

                                                                                }
                                                                                else {

                                                                                        find_it_SetSgDeclarationStatements = UsingDirRelativeToDeclarations_2[using_dir_stmt].find(find_decl_stmt);

                                                                                }

                                                                                // Ok, the following code could be a little bit tricky, that's why we have this example here:

                                                                                // Motivating example:

                                                                                //        namespace X {
                                                                                //                int a, c, d;
                                                                                //        }
                                                                                //        void foo() {
                                                                                //                float r;
                                                                                //                double c; // case c
                                                                                //
                                                                                //                using namespace X; // hides float a
                                                                                //
                                                                                //                double a; // case a
                                                                                //
                                                                                //
                                                                                //        }

                                                                                // SYMBOLTABLE FROM WHICH INTERSECTION IS CALLED, BUT CAN ALSO BE IN ANY OTHER ORDER!!!!!!
                                                                                //        0x8df7990 / name: a / sage-class-name: SgVariableSymbol
                                                                                //        0x8df7978 / name: c / sage-class-name: SgVariableSymbol
                                                                                //        0x8df7960 / name: r / sage-class-name: SgVariableSymbol
                                                                                //        0x8df7900 / name: a / sage-class-name: SgVariableSymbol // from Namespace X
                                                                                //        0x8df7918 / name: c / sage-class-name: SgVariableSymbol        // from Namespace X
                                                                                //        0x8df7930 / name: d / sage-class-name: SgVariableSymbol // from Namespace X

                                                                                //        case a: (is equivalent to declaration NOT found in UsingDirRelativeToDeclarations_2)
                                                                                //                case a 1: int a stands before double a in the symbol-table -> no swap
                                                                                //                case a 2: double a stands before int a in the symbol-table -> swap the symbol_pointers, because
                                                                                //                          otherwise it would mean, that int a hides double a, what is false!!

                                                                                //        case c: (is equivalent to declaration found in UsingDirRelativeToDeclarations_2)
                                                                                //        // analog to case a, but the other way round

                                                                                if( find_it_SetSgDeclarationStatements != UsingDirRelativeToDeclarations_2[using_dir_stmt].end() ) {

                                                                                        // if found: means that non-using element is BEFORE (in the source code!!!) using stmt

#ifdef HIDDEN_LIST_DEBUG
                                                                                        cout << " FOUND! and ";
#endif

                                                                                        if(temp_symbol_pointer_not_from_using == p_symbol) {

                                                                                                // means that the non-using element is BEFORE (in the symbol-table!!!) the element from the using
                                                                                                // -> therefore no swap!!

#ifdef HIDDEN_LIST_DEBUG
                                                                                                cout << "no swap" << endl;
#endif
                                                                                        }
                                                                                        else {

                                                                                                // swap pointers and..
                                                                                                temp_symbol_pointer_not_from_using = p_symbol;
                                                                                                p_symbol = it_symbol_pointer;
                                                                                                it_symbol_pointer = temp_symbol_pointer_not_from_using;

                                                                                                // .. relevant bool variables
                                                                                                it_symbol_hashmap__symbol_of_class = (*it)->symbol_of_class;
                                                                                                it_symbol_hashmap__is_using_decl_in_class = (*it)->is_using_decl_in_class;

#ifdef HIDDEN_LIST_DEBUG
                                                                                                cout << " swap" << endl;
#endif

                                                                                        }

                                                                                }
                                                                                else {

#ifdef HIDDEN_LIST_DEBUG
                                                                                        cout << " not FOUND!";
#endif

                                                                                        if(temp_symbol_pointer_not_from_using == p_symbol) {

                                                                                                // swap pointers and..
                                                                                                temp_symbol_pointer_not_from_using = p_symbol;
                                                                                                p_symbol = it_symbol_pointer;
                                                                                                it_symbol_pointer = temp_symbol_pointer_not_from_using;

                                                                                                // .. relevant bool variables
                                                                                                it_symbol_hashmap__symbol_of_class = (*it)->symbol_of_class;
                                                                                                it_symbol_hashmap__is_using_decl_in_class = (*it)->is_using_decl_in_class;

#ifdef HIDDEN_LIST_DEBUG
                                                                                                cout << " swap" << endl;
#endif

                                                                                        }
                                                                                        else {

#ifdef HIDDEN_LIST_DEBUG
                                                                                                cout << "no swap" << endl;
#endif

                                                                                        }

                                                                                }

                                                                                no_insert_into_hiddenlist = false;
                                                                                it_symbol_hashmap__is_already_proofed_to_be_valid = true;

                                                                        }

                                                                }
                                                                // case 3: symbol that is hidden is from a using stmt AND depth of hidden symbol is < as depth of hiding symbol
                                                                // + !! also includes case 4
                                                                else if( it_symbol_hashmap->second->symbol_is_from_using && ( it_symbol_hashmap->second->depth_of_using < depth)  ) {

#ifdef HIDDEN_LIST_DEBUG
                                                                        cout << "case 3:"<< endl;
#endif

                                                                        // here we have to distinguish between using-dirs and using-decls:
                                                                        if(it_symbol_hashmap->second->si_using_dir != NULL) {

                                                                                // take a look into this->UsingDirectivesSet to find out if the using directive that brings this symbol
                                                                                //  into the scope has already been encountered yet

// CH (4/7/2010): At least MSVC 9.0 can compile the following code
//#ifdef _MSCx_VER
                                                                                SetSgUsingDirectiveStatementsWithSgScopeStatement::iterator it_UsingDirectivesSet;
//#else
//                                                                              // tps (04/07/2009) : Problems with Hiddenlist
//#pragma message ("WARNING : HiddenList_Intersection : SetSgUsingDirectiveStatementsWithSgScopeStatement not defined")
//#endif
                                                                                UsingDirectiveWithScope dummy_UsingDirectiveWithScope;

                                                                                dummy_UsingDirectiveWithScope.using_dir = it_symbol_hashmap->second->si_using_dir;

                                                                                ROSE_ASSERT(it_symbol_hashmap->second->si_using_dir->get_parent() != NULL);

                                                                                // Robert Preissl, June 27 2007: To find an element in a SetSgUsingDirectiveStatementsWithSgScopeStatement we need to
                                                                                //  specify the scope & the SgUsingDirectiveStatement (-> look at comparator cmp_UsingDirectiveWithScope in HiddenList.h)
                                                                                dummy_UsingDirectiveWithScope.scope = isSgScopeStatement(it_symbol_hashmap->second->si_using_dir->get_parent());
// CH (4/7/2010): This issue is fixed by adding 'const' to compare function.
//#ifdef _MSCx_VER
                                                                                it_UsingDirectivesSet = UsingDirectivesSet.find( dummy_UsingDirectiveWithScope );

                                                                                if(it_UsingDirectivesSet != UsingDirectivesSet.end() ) {

#ifdef HIDDEN_LIST_DEBUG
                                                                                        cout << " A :----> isSgUsingDirectiveStatement found in UsingDirectivesSet" << endl;
                                                                                        cout << " is_already_proofed_to_be_valid: " << (*it_UsingDirectivesSet).is_already_proofed_to_be_valid << endl;
#endif

                                                                                        if( (*it_UsingDirectivesSet).is_already_proofed_to_be_valid == true ) {

                                                                                                // if the using directive that brings in p_symbol has already been encountered, this element
                                                                                                //  can be inserted into the hiddenlist
                                                                                                no_insert_into_hiddenlist = false;

                                                                                                // Robert Preissl, June 27 2007 : is already true because of constructor call with "true" at the end
                                                                                                //   of this function (when a symbol which comes from a using is being hidden and its scope-depth
                                                                                                //   is smaller than those of the hiding symbol, the constructor at the end of this function has
                                                                                                //   has already been called!!!!!)
                                                                                                //        it_symbol_hashmap__is_already_proofed_to_be_valid = true;

                                                                                        }
                                                                                        else {

                                                                                                no_insert_into_hiddenlist = true;

                                                                                        }
                                                                        }
                                                                        else 
//#else
//#pragma message ("WARNING : HiddenList_Intersection : Code does not work under Windows")
//#endif
                                                                        {
                                                                                cout << "ERROR: isSgUsingDirectiveStatement not found in UsingDirectivesSet" << endl;

                                                                        }

                                                                        }
                                                                else if(it_symbol_hashmap->second->si_using_decl != NULL) {

                                                                        // take a look into this->UsingDeclarationsSet to find out if the
                                                                        //  using directive that brings this symbol
                                                                        //  into the scope has already been encountered yet

// CH (4/7/2010): This issue is fixed by adding 'const' to compare function.
//#ifdef _MSCx_VER
                                                                        SetSgUsingDeclarationWithScopeWithSgScopeStatement::iterator it_UsingDeclarationsSet;
//#else
//                                                                      //tps (12/7/2009) : Problems in Windows in Release Mode
//#pragma message ("WARNING : HiddenList_Intersection : Code does not work under Windows")
//#endif
                                                                        UsingDeclarationWithScope dummy_UsingDeclarationWithScope;

                                                                        dummy_UsingDeclarationWithScope.using_decl = it_symbol_hashmap->second->si_using_decl;

                                                                        ROSE_ASSERT(it_symbol_hashmap->second->si_using_decl->get_parent() != NULL);

                                                                        // Robert Preissl, June 27 2007: To find an element in a
                                                                        //  SetSgUsingDirectiveStatementsWithSgScopeStatement we need to
                                                                        //  specify the scope & the SgUsingDirectiveStatement (-> look at
                                                                        //  comparator cmp_UsingDirectiveWithScope in HiddenList.h)
                                                                        dummy_UsingDeclarationWithScope.scope = isSgScopeStatement(it_symbol_hashmap->second->si_using_decl->get_parent());

// CH (4/7/2010): This issue is fixed by adding 'const' to compare function.
//#ifdef _MSCx_VER
                                                                        it_UsingDeclarationsSet = UsingDeclarationsSet.find( dummy_UsingDeclarationWithScope );

                                                                        if(it_UsingDeclarationsSet != UsingDeclarationsSet.end() ) {

#ifdef HIDDEN_LIST_DEBUG
                                                                                cout << " A :----> isSgUsingDeclarationStatement found in UsingDeclarationsSet" << endl;
                                                                                cout << " is_already_proofed_to_be_valid: " << (*it_UsingDeclarationsSet).is_already_proofed_to_be_valid << endl;
#endif

                                                                                if( (*it_UsingDeclarationsSet).is_already_proofed_to_be_valid == true ) {

                                                                                        // if the using directive that brings in p_symbol has already been encountered, this element
                                                                                        //  can be inserted into the hiddenlist
                                                                                        no_insert_into_hiddenlist = false;

                                                                                        // Robert Preissl, June 27 2007 : is already true because of constructor call with "true" at the end
                                                                                        //   of this function (when a symbol which comes from a using is being hidden and its scope-depth
                                                                                        //   is smaller than those of the hiding symbol, the constructor at the end of this function has
                                                                                        //   has already been called!!!!!)
                                                                                        //        it_symbol_hashmap__is_already_proofed_to_be_valid = true;

                                                                                }
                                                                                else {

                                                                                        no_insert_into_hiddenlist = true;

                                                                                }

                                                                        }
//#else
//                                                                      //tps (12/7/2009) : Problems in Windows in Release Mode
//#pragma message ("WARNING : HiddenList_Intersection : Code does not work under Windows")
//#endif

                                                                }
                                                                else {

                                                                        cout << "ERROR: no using-dir or using-decl" << endl;

                                                                }

                                                        }
                                                        else {  }

                                                }



                                                if(!no_insert_into_hiddenlist) {

                                                        // if the symbol from the symbol_hashmap is a Variable, Function, Typedef, Class, Enum, EnumField
                                                        if( isSgVariableSymbol(p_symbol) || isSgFunctionSymbol(p_symbol) || isSgTypedefSymbol(p_symbol) || isSgClassSymbol(p_symbol) || isSgEnumSymbol(p_symbol) || isSgEnumFieldSymbol(p_symbol) || isSgNamespaceSymbol(p_symbol) ) {

                                                                // Motivating example: int main() { int x; { double x,y,z; } int y; return 0; }
                                                                // -> in the main block x, y would be in the symbol table
                                                                // -> so in the nested block we have x, y as hidden symbols, which is wrong
                                                                // -> therefore take only those symbols from the Stack which are really valid at this time

                                                                // if is_already_proofed_to_be_valid is false, this means that this symbol is in a class or it
                                                                //  is not visible at this time (like the "int y" above)

                                                                // Robert Preissl, June 15 2007 is_already_proofed_to_be_valid is either true OR
                                                                //  (parent of p_symbol is global scope AND file infos of the the compared symbols are different);
                                                                //  in other words, global variables of included headers shouldn't be neglected!!!
                                                                // Motivating example:
                                                                // A.h : int a;
                                                                // A.C : #include "A.h"
                                                                //         int main() {
                                                                //                double a;
                                                                //                return 0;
                                                                //         }
                                                                //
                                                                // -> int a would not be considered for the hiddenlist-computation if we would just ask about the
                                                                //   "is_already_proofed_to_be_valid - flag", because it would be false
                                                                //    (Reason: HiddenList Computation just uses "traverse", so "int a"'s validity will not be proofed!!!)


                                                                SgDeclarationStatement* decl_stat_in_header = GetSgDeclarationStatementOutOfSgSymbol(p_symbol);

                                                                if(it_symbol_hashmap__is_already_proofed_to_be_valid || ( (decl_stat_in_header != NULL) && (decl_stat_in_header->get_parent() != NULL) && isSgGlobal(decl_stat_in_header->get_parent()) && (CompareFileNamesOfSgSymbols(p_symbol, it_symbol_pointer) == 2 ) ) ) {

                                                                        // Robert Preissl, May 14, 2007
                                                                        //  Motivating example: class A { A() {} }; // constructor can't hide his class name
                                                                        //    +
                                                                        // Robert Preissl, May 15, 2007
                                                                        //  Motivating example:
                                                                        //        class A { A() {} };
                                                                        //        class B:A { double A(); // is allowed to hide class A, but can't hide constructor A(), because of function overloading issue };

                                                                        if( isSgClassSymbol(p_symbol) && isSgMemberFunctionSymbol(it_symbol_pointer) && (isSgClassSymbol(p_symbol))->get_declaration()->get_name() == (isSgMemberFunctionSymbol(it_symbol_pointer))->get_scope()->get_declaration()->get_name() ) {
#ifdef HIDDEN_LIST_DEBUG
                                                                                cout << "Constructor: " << p_symbol->get_name().str() << " doesn't hide Class: No entry in   hiddenlist!! " << endl;
#endif
                                                                        }
                                                                        else {

                                                                                // Robert Preissl, July 10 2007 : only symbols whose "scope can be named"
                                                                                //   could be inserted into one of the hidden-lists
                                                                                if( it_symbol_hashmap->second->scope_can_be_named) {

                                                                                        if(isSgVariableSymbol(p_symbol)) {
                                                                                                AvailableHidden_Functions_Variables.insert(p_symbol);
#ifdef HIDDEN_LIST_DEBUG
                                                                                                cout << "VALID Variable Symbol: ";
#endif
                                                                                        }
                                                                                        else if(isSgFunctionSymbol(p_symbol)) {

                                                                                                // Robert Preissl, June 5 2007 : function hiding: 2 different ways in this algorithm:
                                                                                                // *) in classes or namespaces : function is in symbol-table
                                                                                                // **) else: not in symbol-table -> treated in function-intersection-procedure

                                                                                                // C++ Standard page 219: "A locally declared function is not
                                                                                                //  in the same scope as a function in a containing scope"
                                                                                                //
                                                                                                // Motivating example:
                                                                                                //  int f(char*);
                                                                                                //  void g() {
                                                                                                //         extern f(int); // hides f(char*)
                                                                                                //  }

                                                                                                // check if symbol that is hiding is a function:
                                                                                                if(isSgFunctionSymbol(it_symbol_pointer) ) {

                                                                                                        // -> so compare Scopes of the the (name-)matching functions,
                                                                                                        //  if they are in different scopes -> entry in hiddenlist
                                                                                                        SgFunctionDeclaration* p_symbol_f_decl = (isSgFunctionSymbol(p_symbol))->get_declaration();
                                                                                                        SgFunctionDeclaration* it_symbol_f_decl = (isSgFunctionSymbol(it_symbol_pointer))->get_declaration();
                                                                                                        ROSE_ASSERT(p_symbol_f_decl != NULL);
                                                                                                        ROSE_ASSERT(it_symbol_f_decl != NULL);

                                                                                                        if(p_symbol_f_decl->get_scope() != it_symbol_f_decl->get_scope()) {

                                                                                                                AvailableHidden_Functions_Variables.insert(p_symbol);
#ifdef HIDDEN_LIST_DEBUG
                                                                                                                cout << "VALID Function Symbol: ";
#endif

                                                                                                        }
                                                                                                        else {
                                                                                                                // know that is in a namespace, because
                                                                                                                //  it_symbol_hashmap__is_already_proofed_to_be_valid is true (so not a class)
#ifdef HIDDEN_LIST_DEBUG
                                                                                                                cout << "Function Overloading in Namespaces! -> no hiddenlist entry ";
#endif
                                                                                                        }

                                                                                                }
                                                                                                else {

                                                                                                        AvailableHidden_Functions_Variables.insert(p_symbol);
#ifdef HIDDEN_LIST_DEBUG
                                                                                                        cout << "VALID Function Symbol: ";
#endif

                                                                                                }

                                                                                        }
                                                                                        else if(isSgTypedefSymbol(p_symbol)) {
                                                                                                AvailableHidden_Types.insert(p_symbol);
#ifdef HIDDEN_LIST_DEBUG
                                                                                                cout << "VALID Typedef Symbol: ";
#endif
                                                                                        }
                                                                                        else if(isSgClassSymbol(p_symbol)) {
                                                                                                ElaboratingForcingHidden_Types.insert(p_symbol);
#ifdef HIDDEN_LIST_DEBUG
                                                                                                cout << "VALID Class Symbol: ";
#endif
                                                                                        }
                                                                                        else if(isSgEnumSymbol(p_symbol)) {
                                                                                                //todo
                                                                                                ElaboratingForcingHidden_Types.insert(p_symbol);
#ifdef HIDDEN_LIST_DEBUG
                                                                                                cout << "VALID Enum Symbol: ";
#endif
                                                                                        }
                                                                                        else if(isSgEnumFieldSymbol(p_symbol)) {
                                                                                                AvailableHidden_Functions_Variables.insert(p_symbol);
#ifdef HIDDEN_LIST_DEBUG
                                                                                                cout << "VALID EnumField Symbol: ";
#endif
                                                                                        }
                                                                                        else if(isSgNamespaceSymbol(p_symbol)) {
                                                                                                AvailableHidden_Functions_Variables.insert(p_symbol);
#ifdef HIDDEN_LIST_DEBUG
                                                                                                cout << "VALID Namespace Symbol: ";
#endif
                                                                                        }

#ifdef HIDDEN_LIST_DEBUG
                                                                                        cout << " with address: " << p_symbol << endl;
#endif

                                                                                }

                                                                        }
                                                                }
                                                                else {

                                                                        // ..or a valid class symbol (because in a class, the order in which symbols are declared doesn't count for hiding
                                                                        // Robert Preissl, July 5 2007 : added another flag that has to be checked: is_using_decl_in_class
                                                                        // Motivating example:
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
                                                                        //
                                                                        // So, in this case, only g is hidden!!!
                                                                        if(it_symbol_hashmap__symbol_of_class && !(*it)->is_using_decl_in_class ) {


                                                                                // Robert Preissl, May 30 2007
                                                                                //  ad declaration matching: see page 219 c++ standard
                                                                                //  motivating example:
                                                                                //        class B {
                                                                                //                public:
                                                                                //                        int f(int);
                                                                                //        };
                                                                                //
                                                                                //        class D : public B {
                                                                                //                public:
                                                                                //                        int f(char*); // hides, not overloades, B's f !!!!
                                                                                //
                                                                                //        };

                                                                                // p_symbol is what may be hidden and *it is what is responcible for the hidding

                                                                                // The use of mangled name may not be required since a comparison of scopes is likely enough (and faster)
                                                                                if(isSgMemberFunctionSymbol(p_symbol) && isSgMemberFunctionSymbol(it_symbol_pointer ) && ( isSgMemberFunctionSymbol(p_symbol)->get_scope() != isSgMemberFunctionSymbol(it_symbol_pointer)->get_scope() ) ) {

                                                                                        AvailableHidden_Functions_Variables.insert(p_symbol);

#ifdef HIDDEN_LIST_DEBUG
                                                                                        cout << "VALID Class-Function Symbol: "<< endl;
#endif

                                                                                        if(it_symbol_hashmap->second->symbol_is_from_using) {
                                                                                                cout << "YESY"<< endl;
                                                                                        }

                                                                                }

                                                                                // Robert Preissl, May 14, 2007
                                                                                //  ad function overloading in a class: (case: function in symbol table is a member function)
                                                                                //  (other case: function in symbol table is "normal" function, will be treated in other intersection procedure)
                                                                                //
                                                                                // Robert Preissl, May 30, comment: Since we are in the "symbol_of_class" branch we know that if
                                                                                // isSgFunctionSymbol(p_symbol) is true, also isSgMemberFunctionSymbol(p_symbol) is true!!!
                                                                                else if( isSgFunctionSymbol(p_symbol) && isSgMemberFunctionSymbol(it_symbol_pointer) ) {

#ifdef HIDDEN_LIST_DEBUG
                                                                                        cout << "Function Overloading of "<< p_symbol->get_name().str() << ": No entry in hiddenlist!! " << endl;
#endif

                                                                                }

                                                                                // Robert Preissl, May 29 2007
                                                                                //  ad templates: it_symbol_pointer can also be a (Function)TemplateSymbol
                                                                                //  (implicates to be a function template because of the same name of p_symbol which is a functionsymbol
                                                                                else if(isSgFunctionSymbol(p_symbol) && isSgTemplateSymbol(it_symbol_pointer) ) {

#ifdef HIDDEN_LIST_DEBUG
                                                                                        cout << " Function Overloading of TemplateFunction "<< p_symbol->get_name().str() << ": No entry in hiddenlist!! " << endl;
#endif

                                                                                }
                                                                                else {

                                                                                        if(isSgVariableSymbol(p_symbol)) {
                                                                                                AvailableHidden_Functions_Variables.insert(p_symbol);
#ifdef HIDDEN_LIST_DEBUG
                                                                                                cout << "VALID Class-Variable Symbol: ";
#endif
                                                                                        }
                                                                                        else if(isSgFunctionSymbol(p_symbol)) {
                                                                                                AvailableHidden_Functions_Variables.insert(p_symbol);
#ifdef HIDDEN_LIST_DEBUG
                                                                                                cout << "VALID Class-Function Symbol: ";
#endif
                                                                                        }
                                                                                        else if(isSgTypedefSymbol(p_symbol)) {
                                                                                                AvailableHidden_Types.insert(p_symbol);
#ifdef HIDDEN_LIST_DEBUG
                                                                                                cout << "VALID Class-Typedef Symbol: ";
#endif
                                                                                        }
                                                                                        else if(isSgClassSymbol(p_symbol)) {
                                                                                                //todo
                                                                                                ElaboratingForcingHidden_Types.insert(p_symbol);
#ifdef HIDDEN_LIST_DEBUG
                                                                                                cout << "VALID Class-Class Symbol: ";
#endif
                                                                                        }
                                                                                        else if(isSgEnumSymbol(p_symbol)) {
                                                                                                //todo
                                                                                                ElaboratingForcingHidden_Types.insert(p_symbol);
#ifdef HIDDEN_LIST_DEBUG
                                                                                                cout << "VALID Class-Enum Symbol: ";
#endif
                                                                                        }
                                                                                        else if(isSgEnumFieldSymbol(p_symbol)) {
                                                                                                AvailableHidden_Functions_Variables.insert(p_symbol);
#ifdef HIDDEN_LIST_DEBUG
                                                                                                cout << "VALID Class-EnumField Symbol: ";
#endif
                                                                                        }

#ifdef HIDDEN_LIST_DEBUG
                                                                                        cout << " with address: " << p_symbol << endl;
#endif

                                                                                }

                                                                        }

                                                                        // Robert Preissl, May 30 2007: ad Name hiding, C++ standard page 29: "A class name or
                                                                        //  enumeration name can be hidden by the name of an object, function or enumeration
                                                                        // motivating example:
                                                                        //        class B { public: B() {} };
                                                                        //        int B;


                                                                        // Robert Preissl, May 30 2007, don't need to ask about FunctionSymbol because this is separately treated!!!


                                                                        // Robert Preissl, May 30 2007: should also a typdef be allowed to hide classes or enums???
                                                                        // typedef struct X{} X;
                                                                        // Answer: The typedef is treated as a type in ROSE, but this over resolves
                                                                        //   the issue which in the language as an alias and not a new type. As
                                                                        //   a result the use of type elaboration is meaningless and equivalent to the
                                                                        //   unelaborated type because a typedef is an alias and NOT a new type from the
                                                                        //   language point of view.


                                                                        // Robert Preissl, June 6 2007 added isSgFunctionSymbol, because it can happen
                                                                        //   e.g. in classes or namespaces that the function is in the symbol-table and
                                                                        //   therefore has to be treated in this intersection function!!!

                                                                        // Robert Preissl, June 7 2007: look at comment of June 7 below!
                                                                        //   -> p_symbol can also be the symbol that is hiding!! (and the other way round!)

                                                                        // old code, don't remove!
                                                                        //else if( ( isSgClassSymbol(p_symbol) || isSgEnumSymbol(p_symbol) ) && ( isSgVariableSymbol(it_symbol_pointer) ||  isSgFunctionSymbol(it_symbol_pointer)  || isSgEnumSymbol(it_symbol_pointer) ) ) {

                                                                        else if( ( ( isSgClassSymbol(p_symbol) || isSgEnumSymbol(p_symbol) ) && ( isSgVariableSymbol(it_symbol_pointer) ||  isSgFunctionSymbol(it_symbol_pointer)  || isSgEnumSymbol(it_symbol_pointer) ) ) || ( ( isSgClassSymbol(it_symbol_pointer) || isSgEnumSymbol(it_symbol_pointer) ) && ( isSgVariableSymbol(p_symbol) ||  isSgFunctionSymbol(p_symbol)  || isSgEnumSymbol(p_symbol) ) ) ) {


                                                                                // ad Robert Preissl, June 6 2007 added isSgFunctionSymbol:
                                                                                //  but if it is not a function of a namespace or class,                                                                                                                //  only call the hidden-list computation once! (in the function_intersection procedure)
                                                                                if(isSgFunctionSymbol(it_symbol_pointer) ) {

                                                                                        SgFunctionDeclaration* it_symbol_f_decl = (isSgFunctionSymbol(it_symbol_pointer))->get_declaration();
                                                                                        ROSE_ASSERT(it_symbol_f_decl != NULL);
                                                                                        ROSE_ASSERT((it_symbol_f_decl->get_parent()) != NULL);

                                                                                        if( isSgNamespaceDefinitionStatement(it_symbol_f_decl->get_parent()) || isSgMemberFunctionDeclaration(it_symbol_f_decl) ) {

                                                                                                // Robert Preissl, June 7 2007: !!! There is no particular ordering
                                                                                                //  of the elements in the symbol-table!
                                                                                                // Motivating example:
                                                                                                //         class A { int a; };
                                                                                                //         void A();
                                                                                                //
                                                                                                //  -> This means that it is not always guaranteed that class A
                                                                                                //   is before function A in the symbol table, and as a result
                                                                                                //   that function A would always hide class A !!!!!
                                                                                                //  -> Therefore, and only in the case where hiding is an issue
                                                                                                //    in the same scope, have to compare the file-info of the elements
                                                                                                //    of the symbol-table to find out what is hidden and what is hiding!!!

                                                                                                int result_of_CompareFileInfoOfSgSymbols = CompareFileInfoOfSgSymbols(p_symbol, it_symbol_pointer);
                                                                                                if( result_of_CompareFileInfoOfSgSymbols == 0) {

#ifdef HIDDEN_LIST_DEBUG
                                                                                                        cout << "Could not compare file-info!! ";
#endif

                                                                                                }
                                                                                                else {

                                                                                                        //        todo

                                                                                                        if( result_of_CompareFileInfoOfSgSymbols == 1 ) {

                                                                                                                // Robert Preissl, July 10 2007 :
                                                                                                                //   only symbols whose "scope can be named"
                                                                                                                //   could be inserted into one of the hidden-lists
                                                                                                                if( it_symbol_hashmap->second->scope_can_be_named) {

                                                                                                                        ElaboratingForcingHidden_Types.insert(p_symbol);

                                                                                                                }

                                                                                                        }
                                                                                                        else {

                                                                                                                // Robert Preissl, July 10 2007 :
                                                                                                                //   only symbols whose "scope can be named"
                                                                                                                //   could be inserted into one of the hidden-lists
                                                                                                                if( (*it)->scope_can_be_named ) {

                                                                                                                        ElaboratingForcingHidden_Types.insert(it_symbol_pointer);
                                                                                                                }

                                                                                                        }

#ifdef HIDDEN_LIST_DEBUG
                                                                                                        cout << "VALID Class or Enum Symbol: " << endl;
#endif

                                                                                                }

                                                                                        }
                                                                                        else {

                                                                                                // no insert into hiddenlist, because done in other interesction procedure!

                                                                                        }

                                                                                }
                                                                                // Robert Preissl, June 7 2007: p_symbol can also be the symbol that is hiding!!
                                                                                else if(isSgFunctionSymbol(p_symbol) ) {

                                                                                        SgFunctionDeclaration* p_symbol_f_decl = (isSgFunctionSymbol(p_symbol))->get_declaration();
                                                                                        ROSE_ASSERT(p_symbol_f_decl != NULL);
                                                                                        ROSE_ASSERT((p_symbol_f_decl->get_parent()) != NULL);

                                                                                        if( isSgNamespaceDefinitionStatement(p_symbol_f_decl->get_parent()) || isSgMemberFunctionDeclaration(p_symbol_f_decl) ) {

                                                                                                // Robert Preissl, June 7 2007: !!! There is no particular ordering
                                                                                                //  of the elements in the symbol-table!
                                                                                                // Motivating example:
                                                                                                //         class A { int a; };
                                                                                                //         void A();
                                                                                                //
                                                                                                //  -> This means that it is not always guaranteed that class A
                                                                                                //   is before function A in the symbol table, and as a result
                                                                                                //   that function A would always hide class A !!!!!
                                                                                                //  -> Therefore, and only in the case where hiding is an issue
                                                                                                //    in the same scope, have to compare the file-info of the elements
                                                                                                //    of the symbol-table to find out what is hidden and what is hiding!!!


                                                                                                int result_of_CompareFileInfoOfSgSymbols = CompareFileInfoOfSgSymbols(p_symbol, it_symbol_pointer);
                                                                                                //cout << "result_of_CompareFileInfoOfSgSymbols:" << result_of_CompareFileInfoOfSgSymbols << endl;
                                                                                                if( result_of_CompareFileInfoOfSgSymbols == 0) {

#ifdef HIDDEN_LIST_DEBUG
                                                                                                        cout << "Could not compare file-info!! ";
#endif

                                                                                                }
                                                                                                else {

                                                                                                        //        todo
                                                                                                        if( result_of_CompareFileInfoOfSgSymbols == 1 ) {

                                                                                                                // Robert Preissl, July 10 2007 :
                                                                                                                //   only symbols whose "scope can be named"
                                                                                                                //   could be inserted into one of the hidden-lists
                                                                                                                if( it_symbol_hashmap->second->scope_can_be_named) {

                                                                                                                        ElaboratingForcingHidden_Types.insert(p_symbol);

                                                                                                                }

                                                                                                        }
                                                                                                        else {

                                                                                                                // Robert Preissl, July 10 2007 :
                                                                                                                //   only symbols whose "scope can be named"
                                                                                                                //   could be inserted into one of the hidden-lists
                                                                                                                if( (*it)->scope_can_be_named ) {

                                                                                                                        ElaboratingForcingHidden_Types.insert(it_symbol_pointer);
                                                                                                                }

                                                                                                        }

#ifdef HIDDEN_LIST_DEBUG
                                                                                                        cout << "VALID Class or Enum Symbol: "<< endl;
#endif

                                                                                                }

                                                                                        }
                                                                                        else {

                                                                                                // no insert into hiddenlist, because done in other interesction procedure!

                                                                                        }

                                                                                }
                                                                                else {

                                                                                        // Read comment from above written at June 7 !!!

                                                                                        int result_of_CompareFileInfoOfSgSymbols = CompareFileInfoOfSgSymbols(p_symbol, it_symbol_pointer);

                                                                                        if( result_of_CompareFileInfoOfSgSymbols == 0) {

#ifdef HIDDEN_LIST_DEBUG
                                                                                                cout << "Could not compare file-info!! ";
#endif

                                                                                        }
                                                                                        else {
                                                                                                //cout << "result_of_CompareFileInfoOfSgSymbols:" << result_of_CompareFileInfoOfSgSymbols << endl;
                                                                                                //        todo
                                                                                                if( result_of_CompareFileInfoOfSgSymbols == 1 ) {

                                                                                                        // Robert Preissl, July 10 2007 :
                                                                                                        //   only symbols whose "scope can be named"
                                                                                                        //   could be inserted into one of the hidden-lists
                                                                                                        if( it_symbol_hashmap->second->scope_can_be_named) {

                                                                                                                ElaboratingForcingHidden_Types.insert(p_symbol);

                                                                                                        }

                                                                                                }
                                                                                                else {

                                                                                                        // Robert Preissl, July 10 2007 :
                                                                                                        //   only symbols whose "scope can be named"
                                                                                                        //   could be inserted into one of the hidden-lists
                                                                                                        if( (*it)->scope_can_be_named ) {

                                                                                                                ElaboratingForcingHidden_Types.insert(it_symbol_pointer);
                                                                                                        }

                                                                                                }

#ifdef HIDDEN_LIST_DEBUG
                                                                                                cout << "VALID Class or Enum Symbol: "<< endl;
#endif

                                                                                        }

                                                                                }

                                                                        }
                                                                        else {


                                                                                if(isSgVariableSymbol(p_symbol)) {
#ifdef HIDDEN_LIST_DEBUG
                                                                                        cout << "NOT VALID Variable Symbol: ";
#endif
                                                                                }
                                                                                else if(isSgFunctionSymbol(p_symbol)) {
#ifdef HIDDEN_LIST_DEBUG
                                                                                        cout << "NOT VALID Function Symbol: ";
#endif
                                                                                }
                                                                                else if(isSgTypedefSymbol(p_symbol)) {
#ifdef HIDDEN_LIST_DEBUG
                                                                                        cout << "NOT VALID Typedef Symbol: ";
#endif
                                                                                }
                                                                                else if(isSgClassSymbol(p_symbol)) {
#ifdef HIDDEN_LIST_DEBUG
                                                                                        cout << "NOT VALID Class Symbol: ";
#endif
                                                                                }
                                                                                else if(isSgEnumSymbol(p_symbol)) {
#ifdef HIDDEN_LIST_DEBUG
                                                                                        cout << "NOT VALID Enum Symbol: ";
#endif
                                                                                }
                                                                                else if(isSgEnumFieldSymbol(p_symbol)) {

                                                                                        // Robert Preissl, July 10 2007 :
                                                                                        //   only symbols whose "scope can be named"
                                                                                        //   could be inserted into one of the hidden-lists
                                                                                        if( it_symbol_hashmap->second->scope_can_be_named) {

                                                                                                // Robert Preissl, June 12 2007
                                                                                                // Motivating example:
                                                                                                //         enum enum1 { zeroA };
                                                                                                //         void foo() { int zeroA; }
                                                                                                //
                                                                                                // -> Since an enumfield-symbol has no declaration, the
                                                                                                //    "is_already_proofed_to_be_valid-flag"
                                                                                                //     will not be true, even it should be visible at this time!
                                                                                                // -> Therefore find its enum-symbol-declaration, see if
                                                                                                //     this is valid, and then decide
                                                                                                //     whether to insert this symbol or not!

                                                                                                SgEnumFieldSymbol* enum_field_symbol = isSgEnumFieldSymbol(p_symbol);
                                                                                                SgInitializedName* enum_field_init_name = enum_field_symbol->get_declaration();

                                                                                                ROSE_ASSERT(enum_field_init_name != NULL);
                                                                                                ROSE_ASSERT((enum_field_init_name->get_parent()) != NULL);

                                                                                                SgEnumDeclaration* enum_decl = (SgEnumDeclaration*)enum_field_init_name->get_parent();

                                                                                                string enum_name = enum_decl->get_name().str();
                                                                                                //cout << "enum_decl-name:" << enum_name << endl;

                                                                                                // find it in the scope:
                                                                                                ScopeStackEntry::const_iterator find_enumsymbol_it = (*p_VSHM)[depth_it].find( enum_name );

                                                                                                // found it
                                                                                                if(find_enumsymbol_it != (*p_VSHM)[depth_it].end()) {

                                                                                                        SymbolHashMap symbol_hashmap_for_finding_enum;
                                                                                                        symbol_hashmap_for_finding_enum = find_enumsymbol_it->second;

                                                                                                        SymbolHashMap::const_iterator it_symbol_hashmap_2;

                                                                                                        // now find the correct enum-symbol
                                                                                                        for(it_symbol_hashmap_2 = symbol_hashmap_for_finding_enum.begin(); it_symbol_hashmap_2 != symbol_hashmap_for_finding_enum.end(); ++it_symbol_hashmap_2) {

                                                                                                                ROSE_ASSERT(it_symbol_hashmap_2->first != NULL);

                                                                                                                bool enum_is_valid = false;

                                                                                                                if( isSgEnumSymbol(it_symbol_hashmap_2->first) ) {

                                                                                                                        if( it_symbol_hashmap_2->second->is_already_proofed_to_be_valid ) {

                                                                                                                                enum_is_valid = true;

                                                                                                                        }

                                                                                                                }

                                                                                                                if(enum_is_valid == true) {

                                                                                                                        // insert
                                                                                                                        AvailableHidden_Functions_Variables.insert(p_symbol);

#ifdef HIDDEN_LIST_DEBUG
                                                                                                                        cout << "VALID EnumField Symbol: "<< endl;
#endif

                                                                                                                }
                                                                                                                else {

                                                                                                                        // no insert
#ifdef HIDDEN_LIST_DEBUG
                                                                                                                        cout << "NOT VALID EnumField Symbol: "<< endl;
#endif

                                                                                                                }

                                                                                                        }


                                                                                                }
                                                                                                else {

                                                                                                        cout << "ERROR: enumfield has no enum in the scope stack!!!" << endl;

                                                                                                }

                                                                                        }

                                                                                }
                                                                                else if(isSgNamespaceSymbol(p_symbol)) {
#ifdef HIDDEN_LIST_DEBUG
                                                                                        cout << "NOT VALID Namespace Symbol: "<< endl;
#endif
                                                                                }

#ifdef HIDDEN_LIST_DEBUG
                                                                                cout << " with address: " << p_symbol << endl;
#endif

                                                                        }

                                                                }

                                                        }
                                                        // Robert Preissl, May 24 2007
                                                        else if( isSgTemplateSymbol(p_symbol) ) {

                                                                // Robert Preissl, July 10 2007 :
                                                                //   only symbols whose "scope can be named"
                                                                //   could be inserted into one of the hidden-lists
                                                                if( it_symbol_hashmap->second->scope_can_be_named) {

#ifdef HIDDEN_LIST_DEBUG
                                                                        cout << it_symbol_pointer->sage_class_name() << ": " << it_symbol_pointer->get_name().str() << " would hide template symbol: " << p_symbol->get_name().str() << endl;
#endif

                                                                        if(it_symbol_hashmap__is_already_proofed_to_be_valid) {

#ifdef HIDDEN_LIST_DEBUG
                                                                                cout << "would hide valid template" << endl;
#endif

                                                                                // Let's filter out those cases where a (*it.symbol_pointer) will NOT hide a (valid) TemplateSymbol
                                                                                if( isSgFunctionSymbol( it_symbol_pointer) ) {

                                                                                        // get the function declaration and find out if it is a template function
                                                                                        SgFunctionDeclaration* func_decl = NULL;
                                                                                        if(isSgFunctionSymbol(it_symbol_pointer)) {

                                                                                                SgFunctionSymbol* func_symbol = isSgFunctionSymbol(it_symbol_pointer);
                                                                                                if(func_symbol != NULL) {

                                                                                                        func_decl = func_symbol->get_declaration();

                                                                                                }
                                                                                        }
                                                                                        else if(isSgMemberFunctionSymbol(it_symbol_pointer)) {

                                                                                                SgMemberFunctionSymbol* member_func_symbol = isSgMemberFunctionSymbol(it_symbol_pointer);
                                                                                                if(member_func_symbol != NULL) {

                                                                                                        func_decl = (SgFunctionDeclaration*)member_func_symbol->get_declaration();

                                                                                                }

                                                                                        }
                                                                                        else {

                                                                                                cout << " ERROR in isSgTemplateSymbol, Intersection.C: " << endl;

                                                                                        }

                                                                                        ROSE_ASSERT(func_decl != NULL);

                                                                                        //  case 1a: (*it.symbol_pointer) is a function created from the corresponding function template
                                                                                        if(func_decl->isTemplateFunction()) {

#ifdef HIDDEN_LIST_DEBUG
                                                                                                cout << " Template Function will not hide its own template " << endl;
#endif

                                                                                        }
                                                                                        // case 2: constructor of a template class doesn't hide its template class name
                                                                                        else if(isSgMemberFunctionSymbol(it_symbol_pointer)) {

#ifdef HIDDEN_LIST_DEBUG
                                                                                                cout << " Member-Function (Constructor) will not hide its own class-template " << endl;
#endif

                                                                                        }
                                                                                        else {

                                                                                                // Robert Preissl, June 6 2007: find out if it is a function- or class-template to fill the right list
                                                                                                SgTemplateSymbol* template_symbol = isSgTemplateSymbol(p_symbol);
                                                                                                ROSE_ASSERT(template_symbol != NULL);
                                                                                                SgTemplateDeclaration* template_decl = template_symbol->get_declaration();
                                                                                                ROSE_ASSERT(template_decl != NULL);

                                                                                                // class or nested class template
                                                                                                if(template_decl->get_template_kind() == 1 || template_decl->get_template_kind() == 2) {

                                                                                                        AvailableHidden_Types.insert(p_symbol);

                                                                                                }
                                                                                                // function or member function template
                                                                                                else if(template_decl->get_template_kind() == 3 || template_decl->get_template_kind() == 4 ) {

                                                                                                        AvailableHidden_Functions_Variables.insert(p_symbol);

                                                                                                }

                                                                                        }

                                                                                }
                                                                                // case 3: (class)template-symbol will not hide its template-symbol (of the global scope) with the same name
                                                                                //  (look at AST for further explanation)
                                                                                else if( isSgTemplateSymbol(it_symbol_pointer) ) {

#ifdef HIDDEN_LIST_DEBUG
                                                                                        cout << "No hiding: Template Symbol of Template Symbol" << endl;
#endif

                                                                                }
                                                                                else {

                                                                                        // Robert Preissl, June 6 2007: find out if it is a function- or class-template to fill the right list
                                                                                        SgTemplateSymbol* template_symbol = isSgTemplateSymbol(p_symbol);
                                                                                        ROSE_ASSERT(template_symbol != NULL);
                                                                                        SgTemplateDeclaration* template_decl = template_symbol->get_declaration();
                                                                                        ROSE_ASSERT(template_decl != NULL);

                                                                                        // class or nested class template
                                                                                        if(template_decl->get_template_kind() == 1 || template_decl->get_template_kind() == 2) {

                                                                                                AvailableHidden_Types.insert(p_symbol);

                                                                                        }
                                                                                        // function or member function template
                                                                                        else if(template_decl->get_template_kind() == 3 || template_decl->get_template_kind() == 4 ) {

                                                                                                AvailableHidden_Functions_Variables.insert(p_symbol);

                                                                                        }

                                                                                }

                                                                        }
                                                                        else {

                                                                                if(it_symbol_hashmap__symbol_of_class) {

                                                                                        // case 1b: (*it.symbol_pointer) is a member-function, p_symbol is a member-function (and a template symbol) -> no hiding
                                                                                        // case 1c: (*it.symbol_pointer) is a member-function, p_symbol is a member-function -> no hiding (treated above)

                                                                                        // example:         template<class T> class Stack { // case 3
                                                                                        //                        public:
                                                                                        //                                Stack() {} // case 2
                                                                                        //                                ~Stack() {}
                                                                                        //                                bool push(T );
                                                                                        //                                bool push(T*, T*); // case 1b
                                                                                        //                                // bool push(int in); // not allowed
                                                                                        //                                bool push(int i, double d); // case 1c
                                                                                        //                };

                                                                                        if( /*isSgFunctionSymbol(p_symbol) &&*/ isSgMemberFunctionSymbol(it_symbol_pointer) ) {

#ifdef HIDDEN_LIST_DEBUG
                                                                                                cout << " >>>>>> Function Overloading of "<< p_symbol->get_name().str() << ": No entry in hiddenlist!! " << endl;
#endif

                                                                                        }
                                                                                }
                                                                                else {

#ifdef HIDDEN_LIST_DEBUG
                                                                                        cout << " !! NOT VALID Class Template Symbol " << endl;
#endif

                                                                                }

                                                                        }

                                                                }

                                                        }
                                                        // not a variable or function, typedef, class, enum, namespace -> let's take it without checking if it is a valid symbol!!
                                                        else {
#ifdef HIDDEN_LIST_DEBUG
                                                                cout << " >> Intersection: not a variable, type, function, enum, class, namespace!! : " << p_symbol->sage_class_name() << endl;
#endif


                                                        }

                                                } // no_insert
                                                else {

#ifdef HIDDEN_LIST_DEBUG
                                                        cout << "NO INSERT!!"<< endl;
#endif

                                                }

                                        }

                                        TimingPerformance::accumulateTime ( startTimeAfterFind, accumulatedIntersectionAfterFindTime, accumulatedIntersectionAfterFindCalls );

                                } // for

                                // Robert Preissl, July 9 2007 : Performance Tuning : update the current frame only at the first iteration
                                if( depth_it == depth ) {


                                        TimingPerformance::time_type startTimeFillScopeStack_1;
                                        TimingPerformance::startTimer(startTimeFillScopeStack_1);

                                        if( !(*it)->symbol_is_from_using && !(*it)->is_using_decl_in_class ) {

                                                // update symbol-hashmap after values are inserted in hidden_list
                                                /*symbol_hashmap_value.scope_can_be_named = (*it).scope_can_be_named;
                                                symbol_hashmap_value.symbol_of_class = (*it).symbol_of_class;
                                                symbol_hashmap_value.is_already_proofed_to_be_valid = false;
                                                symbol_hashmap_value.symbol_is_from_using = false;
                                                symbol_hashmap_value.depth_of_using = (*it).depth_of_using;
                                                symbol_hashmap_value.is_using_decl_in_class = (*it).is_using_decl_in_class;
                                                symbol_hashmap_value.si_using_decl_in_class = (*it).si_using_decl_in_class;*/

                                                // SymbolHashMapValue(bool b_name, bool b_class, bool b_valid, bool b_using, SgUsingDirectiveStatement* using_dir, SgUsingDeclarationStatement* using_decl, int depth, bool b_using_in_class, SgUsingDeclarationStatement* using_decl_in_class )



                                                (find_it->second)[(*it)->symbol_pointer] = new SymbolHashMapValue(
                                                        (*it)->scope_can_be_named,     //        symbol_hashmap_value.scope_can_be_named x
                                                        (*it)->symbol_of_class,        //        symbol_hashmap_value.symbol_of_class x
                                                        false,                        //        symbol_hashmap_value.is_already_proofed_to_be_valid x
                                                        false,                        //        symbol_hashmap_value.symbol_is_from_using x
                                                        (*it)->si_using_dir,           //        symbol_hashmap_value.si_using_dir
                                                        (*it)->si_using_decl,                  //        symbol_hashmap_value.si_using_decl
                                                        (*it)->depth_of_using,                   //        symbol_hashmap_value.depth_of_using x
                                                        (*it)->is_using_decl_in_class, //        symbol_hashmap_value.is_using_decl_in_class x
                                                        (*it)->si_using_decl_in_class  //        symbol_hashmap_value.si_using_decl_in_class x
                                                        );

                                        }
                                        else {

                                                // per default a symbol from a namespace is not valid
                                                // Motivating example:
                                                //        int x;
                                                //        namespace Y { double x; }
                                                //        void foo() { float x; }
                                                //        using namespace Y;
                                                //
                                                // -> the problem is the we update the global scope before the intersection starts with the symbol double x.
                                                // -> so, float x would hide int x & double x; what is false
                                                // -> therefore set double x  valid = false and mark that it comes from a using dir. (symbol_is_from_using = true)
                                                // -> when it matches to a symbol in the intersection-procedure, take a look into the UsingDirRelativeToDeclarations HashMap to find
                                                //        out if the symbol that is hiding (here: float x) is BEFORE or AFTER the using stmt that includes the possibly hidden symbol
                                                // -> then decide whether to insert into hidden-list or not

                                                if( (*it)->si_using_dir != NULL && !(*it)->is_using_decl_in_class ) {

                                                        // constructor call of SymbolHashMapValue:
                                                        //        symbol_hashmap_value.scope_can_be_named = (*it).scope_can_be_named
                                                        //        symbol_hashmap_value.symbol_of_class = (*it_VecSymbolInfo).symbol_of_class
                                                        //        symbol_hashmap_value.is_already_proofed_to_be_valid = true // !!
                                                        //        symbol_hashmap_value.symbol_is_from_using = true // true, because is included via using-dir
                                                        //        symbol_hashmap_value.si_using_dir = (*it).si_using_dir
                                                        //        symbol_hashmap_value.si_using_decl = NULL
                                                        //        symbol_hashmap_value.depth_of_using = (*it).depth_of_using
                                                        //        symbol_hashmap_value.is_using_decl_in_class = false
                                                        //        symbol_hashmap_value.si_using_decl_in_class = NULL

                                                        // symbol_hashmap_value = SymbolHashMapValue( (*it).scope_can_be_named, (*it).symbol_of_class, true, true, (*it).si_using_dir, NULL, (*it).depth_of_using, false, NULL);

                                                        (find_it->second)[(*it)->symbol_pointer] = new SymbolHashMapValue( (*it)->scope_can_be_named, (*it)->symbol_of_class, true, true, (*it)->si_using_dir, NULL, (*it)->depth_of_using, false, NULL);
                                                        ;
                                                }
                                                else if( (*it)->si_using_decl != NULL && !(*it)->is_using_decl_in_class ) {

                                                        // constructor call of SymbolHashMapValue:
                                                        //        symbol_hashmap_value.scope_can_be_named = (*it).scope_can_be_named
                                                        //        symbol_hashmap_value.symbol_of_class = (*it_VecSymbolInfo).symbol_of_class
                                                        //        symbol_hashmap_value.is_already_proofed_to_be_valid = true // !!
                                                        //        symbol_hashmap_value.symbol_is_from_using = true; // true, because is included via using-dir
                                                        //        symbol_hashmap_value.si_using_dir = NULL
                                                        //        symbol_hashmap_value.si_using_decl = (*it).si_using_decl
                                                        //        symbol_hashmap_value.depth_of_using = (*it).depth_of_using
                                                        //        symbol_hashmap_value.is_using_decl_in_class = false
                                                        //        symbol_hashmap_value.si_using_decl_in_class = NULL

                                                        // symbol_hashmap_value = SymbolHashMapValue( (*it).scope_can_be_named, (*it).symbol_of_class, true, true, NULL, (*it).si_using_decl, (*it).depth_of_using, false, NULL);

                                                        (find_it->second)[(*it)->symbol_pointer] = new SymbolHashMapValue( (*it)->scope_can_be_named, (*it)->symbol_of_class, true, true, NULL, (*it)->si_using_decl, (*it)->depth_of_using, false, NULL);


                                                }
                                                else if( (*it)->is_using_decl_in_class ) {

                                                        // constructor call of SymbolHashMapValue:
                                                        //        symbol_hashmap_value.scope_can_be_named = (*it).scope_can_be_named
                                                        //        symbol_hashmap_value.symbol_of_class = (*it_VecSymbolInfo).symbol_of_class
                                                        //        symbol_hashmap_value.is_already_proofed_to_be_valid = true // !!
                                                        //        symbol_hashmap_value.symbol_is_from_using = true; // true, because is included via using-dir
                                                        //        symbol_hashmap_value.si_using_dir = NULL
                                                        //        symbol_hashmap_value.si_using_decl = (*it).si_using_decl
                                                        //        symbol_hashmap_value.depth_of_using = (*it).depth_of_using
                                                        //        symbol_hashmap_value.is_using_decl_in_class = true
                                                        //        symbol_hashmap_value.si_using_decl_in_class = (*it).si_using_decl_in_class

                                                        // symbol_hashmap_value = SymbolHashMapValue( (*it).scope_can_be_named, (*it).symbol_of_class, true, true, NULL, (*it).si_using_decl, (*it).depth_of_using, true, (*it).si_using_decl_in_class);

                                                        (find_it->second)[(*it)->symbol_pointer] = new SymbolHashMapValue( (*it)->scope_can_be_named, (*it)->symbol_of_class, true, true, NULL, (*it)->si_using_decl, (*it)->depth_of_using, true, (*it)->si_using_decl_in_class);


                                                }
                                                else {

                                                        cout << "ERROR: no using-dir or using-decl" << endl;

                                                }

                                        }

                                        TimingPerformance::accumulateTime ( startTimeFillScopeStack_1, accumulatedIntersectionFillScopeStack_1_Time, accumulatedIntersectionFillScopeStack_1_Calls );

                                        // Robert Preissl, August 2 2007 : performance tuning : will have this insides the if's above to reduce copying with symbol_hashmap_value
                                        /*

                                        // Robert Preissl, June 27 2007 : in case 2 of the code above (where we treat symbols from included via using) we
                                        //  may have swaped p_symbol and it_symbol_pointer, so it's safer to use here the pointer from the iterator it
                                        //        (find_it->second)[it_symbol_pointer] = symbol_hashmap_value;
                                        (find_it->second)[(*it).symbol_pointer] = symbol_hashmap_value;

                                        */

                                }

                        }
                        // not found, make new entry (that's how the stack of scopes is filled)
                                                else {

                                                        // Robert Preissl, July 9 2007 : Performance Tuning : update the current frame only at the first iteration
                                                        if( depth_it == depth ) {

                                                                TimingPerformance::time_type startTimeFillScopeStack_2;
                                                                TimingPerformance::startTimer(startTimeFillScopeStack_2);

                                                                // DQ (9/25/2007): Change suggested by Jeremiah.
                                                                // ((*p_VSHM)[depth])[ (*it)->name ] = SymbolHashMap();
                                                                // SymbolHashMap& temp = ((*p_VSHM)[depth])[ (*it)->name ];
                                                                SymbolHashMap& temp = ((*p_VSHM)[depth])[ (*it)->name ];
                                                                temp.clear();

                                                                if( !(*it)->symbol_is_from_using && !(*it)->is_using_decl_in_class ) {

                                                                        // update symbol-hashmap after values are inserted in hidden_list
                                                                        /*symbol_hashmap_value.scope_can_be_named = (*it).scope_can_be_named;
                                                                        symbol_hashmap_value.is_already_proofed_to_be_valid = false;
                                                                        symbol_hashmap_value.symbol_of_class = (*it).symbol_of_class;
                                                                        symbol_hashmap_value.symbol_is_from_using = false;
                                                                        symbol_hashmap_value.depth_of_using = (*it).depth_of_using;
                                                                        symbol_hashmap_value.is_using_decl_in_class = (*it).is_using_decl_in_class;
                                                                        symbol_hashmap_value.si_using_decl_in_class = (*it).si_using_decl_in_class;*/

                                                                        temp[(*it)->symbol_pointer] = new SymbolHashMapValue(
                                                                                (*it)->scope_can_be_named,    //        symbol_hashmap_value.scope_can_be_named x
                                                                                (*it)->symbol_of_class,       //        symbol_hashmap_value.symbol_of_class x
                                                                                false,                        //        symbol_hashmap_value.is_already_proofed_to_be_valid x
                                                                                false,                        //        symbol_hashmap_value.symbol_is_from_using x
                                                                                (*it)->si_using_dir,          //        symbol_hashmap_value.si_using_dir
                                                                                (*it)->si_using_decl,         //        symbol_hashmap_value.si_using_decl
                                                                                (*it)->depth_of_using,        //        symbol_hashmap_value.depth_of_using x
                                                                                (*it)->is_using_decl_in_class,//        symbol_hashmap_value.is_using_decl_in_class x
                                                                                (*it)->si_using_decl_in_class //        symbol_hashmap_value.si_using_decl_in_class x
                                                                                );

                                                                }
                                                                else {
                                                                        if( (*it)->si_using_dir != NULL && !(*it)->is_using_decl_in_class) {

                                                                                // constructor call of SymbolHashMapValue:
                                                                                //        symbol_hashmap_value.scope_can_be_named = (*it).scope_can_be_named
                                                                                //        symbol_hashmap_value.symbol_of_class = (*it_VecSymbolInfo).symbol_of_class
                                                                                //        symbol_hashmap_value.is_already_proofed_to_be_valid = true // !!
                                                                                //        symbol_hashmap_value.symbol_is_from_using = true // true, because is included via using-dir
                                                                                //        symbol_hashmap_value.si_using_dir = (*it).si_using_dir
                                                                                //        symbol_hashmap_value.si_using_decl = NULL
                                                                                //        symbol_hashmap_value.depth_of_using = (*it).depth_of_using
                                                                                //        symbol_hashmap_value.is_using_decl_in_class = false
                                                                                //        symbol_hashmap_value.si_using_decl_in_class = NULL

                                                                                // symbol_hashmap_value = SymbolHashMapValue( (*it).scope_can_be_named, (*it).symbol_of_class, true, true, (*it).si_using_dir, NULL, (*it).depth_of_using, false, NULL);

                                                                                temp[(*it)->symbol_pointer] = new SymbolHashMapValue( (*it)->scope_can_be_named, (*it)->symbol_of_class, true, true, (*it)->si_using_dir, NULL, (*it)->depth_of_using, false, NULL);

                                                                        }
                                                                        else if( (*it)->si_using_decl != NULL && !(*it)->is_using_decl_in_class) {

                                                                                // constructor call of SymbolHashMapValue:
                                                                                //        symbol_hashmap_value.scope_can_be_named = (*it).scope_can_be_named
                                                                                //        symbol_hashmap_value.symbol_of_class = (*it_VecSymbolInfo).symbol_of_class
                                                                                //        symbol_hashmap_value.is_already_proofed_to_be_valid = true // !!
                                                                                //        symbol_hashmap_value.symbol_is_from_using = true // true, because is included via using-dir
                                                                                //        symbol_hashmap_value.si_using_dir = NULL
                                                                                //        symbol_hashmap_value.si_using_decl = (*it).si_using_decl
                                                                                //        symbol_hashmap_value.depth_of_using = (*it).depth_of_using
                                                                                //        symbol_hashmap_value.is_using_decl_in_class = false
                                                                                //        symbol_hashmap_value.si_using_decl_in_class = NULL

                                                                                // symbol_hashmap_value = SymbolHashMapValue( (*it).scope_can_be_named, (*it).symbol_of_class, true, true, NULL, (*it).si_using_decl, (*it).depth_of_using, false, NULL);

                                                                                temp[(*it)->symbol_pointer] = new SymbolHashMapValue( (*it)->scope_can_be_named, (*it)->symbol_of_class, true, true, NULL, (*it)->si_using_decl, (*it)->depth_of_using, false, NULL);

                                                                        }
                                                                        else if( (*it)->is_using_decl_in_class ) {

                                                                                // constructor call of SymbolHashMapValue:
                                                                                //        symbol_hashmap_value.scope_can_be_named = (*it).scope_can_be_named
                                                                                //        symbol_hashmap_value.symbol_of_class = (*it_VecSymbolInfo).symbol_of_class
                                                                                //        symbol_hashmap_value.is_already_proofed_to_be_valid = true // !!
                                                                                //        symbol_hashmap_value.symbol_is_from_using = true; // true, because is included via using-dir
                                                                                //        symbol_hashmap_value.si_using_dir = NULL
                                                                                //        symbol_hashmap_value.si_using_decl = (*it).si_using_decl
                                                                                //        symbol_hashmap_value.depth_of_using = (*it).depth_of_using
                                                                                //        symbol_hashmap_value.is_using_decl_in_class = true
                                                                                //        symbol_hashmap_value.si_using_decl_in_class = (*it).si_using_decl_in_class

                                                                                // symbol_hashmap_value = SymbolHashMapValue( (*it).scope_can_be_named, (*it).symbol_of_class, true, true, NULL, (*it).si_using_decl, (*it).depth_of_using, true, (*it).si_using_decl_in_class);

                                                                                temp[(*it)->symbol_pointer] = new SymbolHashMapValue( (*it)->scope_can_be_named, (*it)->symbol_of_class, true, true, NULL, (*it)->si_using_decl, (*it)->depth_of_using, true, (*it)->si_using_decl_in_class);

                                                                        }
                                                                        else {

                                                                                cout << "ERROR: no using-dir or using-decl" << endl;

                                                                        }

                                                                }

                                                                TimingPerformance::accumulateTime ( startTimeFillScopeStack_2, accumulatedIntersectionFillScopeStack_2_Time, accumulatedIntersectionFillScopeStack_2_Calls );

                                                                // Robert Preissl, August 2 2007 : performance tuning : will have this insides the if's above to reduce copying with symbol_hashmap_value

                                                                /*
                                                                // Robert Preissl, June 27 2007 : in case 2 of the code above (where we treat symbols from included via using) we
                                                                //  may have swaped p_symbol and it_symbol_pointer, so it's safer to use here the pointer from the iterator it
                                                                //        temp[it_symbol_pointer] = symbol_hashmap_value;

                                                                temp[(*it).symbol_pointer] = symbol_hashmap_value;
                                                                */

                                                                /*

                                                                ((*p_VSHM)[depth])[ (*it).name ] = temp;

                                                                */

                                                                // temp.clear();

                                                        }

                                                }

                }

                switch (depth_it)
                {
                case 0: TimingPerformance::accumulateTime ( startTimeScope[0], accumulatedIntersectionScopeTime[0], accumulatedIntersectionScopeCalls[0] ); break;
                case 1: TimingPerformance::accumulateTime ( startTimeScope[1], accumulatedIntersectionScopeTime[1], accumulatedIntersectionScopeCalls[1] ); break;
                case 2: TimingPerformance::accumulateTime ( startTimeScope[2], accumulatedIntersectionScopeTime[2], accumulatedIntersectionScopeCalls[2] ); break;
                case 3: TimingPerformance::accumulateTime ( startTimeScope[3], accumulatedIntersectionScopeTime[3], accumulatedIntersectionScopeCalls[3] ); break;
                case 4: TimingPerformance::accumulateTime ( startTimeScope[4], accumulatedIntersectionScopeTime[4], accumulatedIntersectionScopeCalls[4] ); break;
                case 5: TimingPerformance::accumulateTime ( startTimeScope[5], accumulatedIntersectionScopeTime[5], accumulatedIntersectionScopeCalls[5] ); break;
                default: {}
                }

        }

        // DQ (8/3/2007): accumulate the elapsed time for this function
        TimingPerformance::accumulateTime ( startTime, accumulatedIntersectionTime, accumulatedIntersectionCalls );
}

// look in Hash_Map if FunctionDeclaration exists in the current scope & valid scope (look in current scope to get the symbol information)
// further information in "InheritedAttributeSgScopeStatement VisitorTraversal :: evaluateInheritedAttribute"
void Intersection_with_FunctionDeclaration (
        std::vector<ScopeStackEntry>* p_VSHM, int depth,
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
        )
{

        ScopeStackEntry::iterator find_it;

        bool no_insert_into_hiddenlist = false;

        // Robert Preissl, July 9 2007 : Performance Tuning :
        for(int depth_it = depth-1; depth_it >= 0; --depth_it) {

                // .. look in the current scope if there are symbols with the same name as the function
                find_it = (*p_VSHM)[depth_it].find( f_decl->get_name().str() );

                // found it
                if(find_it != (*p_VSHM)[depth_it].end()) {

                        no_insert_into_hiddenlist = false;

                        // Robert Preissl, August 2 2007 : per reference
                        const SymbolHashMap& symbol_hashmap = find_it->second;;

                        SymbolHashMap:: const_iterator it_symbol_hashmap;
                        SgSymbol* p_symbol;

                        string file_name;
                        int source_line;

                        file_name = f_decl->get_file_info()->get_filename();
                        source_line = f_decl->get_file_info()->get_line();

#ifdef HIDDEN_LIST_DEBUG
                        // just to find out what (File Info) is hiding
                        cout << "SIZE symbol_hashmap: " << symbol_hashmap.size() << " for finding function: " << f_decl->get_name().str() << " where ?: file-name " << file_name << " / line: " << source_line  << endl;
#endif

                        // Robert Preissl, August 2 2007 : performance tuning
                        SymbolHashMap:: const_iterator it_symbol_hashmap__end = symbol_hashmap.end();

                        // iterate through all entries of the SymbolHashMap-Value (e.g. x occured in several scopes), but don"t consider
                        //  the function symbol with the same name !!! (is the same function, and therefore will not hide it)
                        for(it_symbol_hashmap = symbol_hashmap.begin(); it_symbol_hashmap != it_symbol_hashmap__end; ++it_symbol_hashmap) {

                                ROSE_ASSERT(it_symbol_hashmap->first != NULL);
                                p_symbol = it_symbol_hashmap->first;

                                if(p_symbol != NULL) {

                                        // Robert Preissl, July 10 2007 :
                                        //   only symbols whose "scope can be named"
                                        //   could be inserted into one of the hidden-lists
                                        if( it_symbol_hashmap->second->scope_can_be_named) {

                                                // Robert Preissl, July 6 2007 : Here, a function will never hide something included into a scope via using, because
                                                //  this is alerady treated in the "normal" intersection procedure.
                                                //
                                                // Motivating example:
                                                //        namespace X {
                                                //                void foobar(int g1) {
                                                //                        int eeee;
                                                //                }
                                                //        }
                                                //
                                                //        void foobar(double h3232) {
                                                //                int tttttttt;
                                                //        }
                                                //
                                                //        using namespace X;

                                                // -> In the "normal" intersection procedure we update the global scope with X's foobar, therefore when we call the intersection
                                                //     for global scope, we find out, that ::foobar is hidden by X's foobar.
                                                // BUT: there is also a call of "Intersection_with_FunctionDeclaration" when ::foobar is encountered.
                                                // -> So, no f_intersection allowed when something from a namespace is being hidden

#ifdef HIDDEN_LIST_DEBUG
                                                if( it_symbol_hashmap->second->symbol_is_from_using ) {

                                                        cout << "Intersection_with_FunctionDeclaration: symbol_is_from_using A" << endl;

                                                }
#endif


                                                if( it_symbol_hashmap->second->symbol_is_from_using ) {

                                                        no_insert_into_hiddenlist = true;

                                                }


                                                if(!no_insert_into_hiddenlist) {

                                                        // if the symbol from the Scope-HashMap is a Variable Symbol, or Function Symbol, or a Typedef Symbol then ..
                                                        if( isSgVariableSymbol(p_symbol) || isSgTypedefSymbol(p_symbol) || isSgClassSymbol(p_symbol) || isSgEnumSymbol(p_symbol) || isSgEnumFieldSymbol(p_symbol) || isSgNamespaceSymbol(p_symbol) ) {

                                                                // .. find out its declaration statement ..
                                                                SgDeclarationStatement* decl_stat;

                                                                decl_stat = GetSgDeclarationStatementOutOfSgSymbol(p_symbol);

                                                                if(decl_stat != NULL) {

                                                                        if(it_symbol_hashmap->second->is_already_proofed_to_be_valid) {

                                                                                if(isSgVariableSymbol(p_symbol)) {
                                                                                        AvailableHidden_Functions_Variables.insert(p_symbol);
#ifdef HIDDEN_LIST_DEBUG
                                                                                        cout << "VALID Variable Symbol: ";
#endif
                                                                                }
                                                                                else if(isSgTypedefSymbol(p_symbol)) {
                                                                                        AvailableHidden_Types.insert(p_symbol);
#ifdef HIDDEN_LIST_DEBUG
                                                                                        cout << "VALID Typedef Symbol: ";
#endif
                                                                                }
                                                                                else if(isSgClassSymbol(p_symbol)) {
                                                                                        //todo
                                                                                        ElaboratingForcingHidden_Types.insert(p_symbol);
#ifdef HIDDEN_LIST_DEBUG
                                                                                        cout << "VALID Class Symbol: ";
#endif
                                                                                }
                                                                                else if(isSgEnumSymbol(p_symbol)) {
                                                                                        //todo
                                                                                        ElaboratingForcingHidden_Types.insert(p_symbol);
#ifdef HIDDEN_LIST_DEBUG
                                                                                        cout << "VALID Enum Symbol: ";
#endif
                                                                                }
                                                                                else if(isSgEnumFieldSymbol(p_symbol)) {
                                                                                        AvailableHidden_Functions_Variables.insert(p_symbol);
#ifdef HIDDEN_LIST_DEBUG
                                                                                        cout << "VALID EnumField Symbol: ";
#endif
                                                                                }
                                                                                else if(isSgNamespaceSymbol(p_symbol)) {
                                                                                        AvailableHidden_Functions_Variables.insert(p_symbol);
#ifdef HIDDEN_LIST_DEBUG
                                                                                        cout << "VALID Namespace Symbol: ";
#endif
                                                                                }
#ifdef HIDDEN_LIST_DEBUG
                                                                                cout << decl_stat->get_mangled_name().str() << " with address: " << decl_stat << endl;
#endif

                                                                        }
                                                                        else {
                                                                                // ..or a valid class symbol (because in a class, the order in which symbols are declared doesn't count for hiding
                                                                                if((it_symbol_hashmap->second)->symbol_of_class) {

                                                                                        if(isSgVariableSymbol(p_symbol)) {
                                                                                                AvailableHidden_Functions_Variables.insert(p_symbol);
#ifdef HIDDEN_LIST_DEBUG
                                                                                                cout << "VALID Class-Variable Symbol: ";
#endif
                                                                                        }
                                                                                        else if(isSgTypedefSymbol(p_symbol)) {
                                                                                                AvailableHidden_Types.insert(p_symbol);
#ifdef HIDDEN_LIST_DEBUG
                                                                                                cout << "VALID Class-Typedef Symbol: ";
#endif
                                                                                        }
                                                                                        else if(isSgClassSymbol(p_symbol)) {
                                                                                                //todo
                                                                                                ElaboratingForcingHidden_Types.insert(p_symbol);
#ifdef HIDDEN_LIST_DEBUG
                                                                                                cout << "VALID Class-Class Symbol: ";
#endif
                                                                                        }
                                                                                        else if(isSgEnumSymbol(p_symbol)) {
                                                                                                //todo
                                                                                                ElaboratingForcingHidden_Types.insert(p_symbol);
#ifdef HIDDEN_LIST_DEBUG
                                                                                                cout << "VALID Class-Enum Symbol: ";
#endif
                                                                                        }
                                                                                        else if(isSgEnumFieldSymbol(p_symbol)) {
                                                                                                AvailableHidden_Functions_Variables.insert(p_symbol);
#ifdef HIDDEN_LIST_DEBUG
                                                                                                cout << "VALID Class-EnumField Symbol: ";
#endif
                                                                                        }
#ifdef HIDDEN_LIST_DEBUG
                                                                                        cout << decl_stat->get_mangled_name().str() << " with address: " << decl_stat << endl;
#endif

                                                                                }
                                                                                else {

#ifdef HIDDEN_LIST_DEBUG
                                                                                        if(isSgVariableSymbol(p_symbol)) {
                                                                                                cout << "NOT VALID Variable Symbol: ";
                                                                                        }
                                                                                        else if(isSgTypedefSymbol(p_symbol)) {
                                                                                                cout << "NOT VALID Typedef Symbol: ";
                                                                                        }
                                                                                        else if(isSgClassSymbol(p_symbol)) {
                                                                                                cout << "NOT VALID Class Symbol: ";
                                                                                        }
                                                                                        else if(isSgEnumSymbol(p_symbol)) {
                                                                                                cout << "NOT VALID Enum Symbol: ";
                                                                                        }
                                                                                        else if(isSgEnumFieldSymbol(p_symbol)) {
                                                                                                cout << "NOT VALID EnumField Symbol: ";
                                                                                        }
                                                                                        else if(isSgNamespaceSymbol(p_symbol)) {
                                                                                                cout << "NOT VALID Namespace Symbol: ";
                                                                                        }
                                                                                        cout << decl_stat->get_mangled_name().str() << " with address: " << decl_stat << endl;
#endif

                                                                                }

                                                                        }

                                                                } // decl_stat != NULL

                                                        }

                                                        // Preissl, May 14 2007, ad function overloading: function can never hide a function with the same name (as long it is valid C++) !!!

                                                        // Preissl, May 24 2007, ad templates: function can't hide its template!!
                                                        else if(isSgFunctionSymbol(p_symbol) || isSgTemplateSymbol(p_symbol)) {

                                                                if(isSgTemplateSymbol(p_symbol)) {

#ifdef HIDDEN_LIST_DEBUG
                                                                        cout << "isSgTemplateSymbol: Function can never hide a function in valid c++ code -> function overloading! " << endl;
#endif

                                                                        if(it_symbol_hashmap->second->is_already_proofed_to_be_valid) {

#ifdef HIDDEN_LIST_DEBUG
                                                                                cout << "would hide valid template" << endl;
#endif

                                                                                // Let's filter out those cases where a (*it.symbol_pointer) will NOT hide a (valid) TemplateSymbol

                                                                                SgTemplateDeclaration* template_decl = (isSgTemplateSymbol(p_symbol))->get_declaration();
                                                                                ROSE_ASSERT(template_decl != NULL);

#ifdef HIDDEN_LIST_DEBUG
                                                                                cout << "template_kind: "  << template_decl->get_template_kind() << endl;
#endif

                                                                                // function or member function template
                                                                                if(template_decl->get_template_kind() == 3 || template_decl->get_template_kind() == 4 ) {

                                                                                        // don't hide a function template if you are a function

                                                                                }
                                                                                // don't hide your own template
                                                                                else if(f_decl->isTemplateFunction()) {

#ifdef HIDDEN_LIST_DEBUG
                                                                                        cout << " NOT HIDING YOUR OWN TEMPLATE " << endl;
#endif

                                                                                }
                                                                                else {

                                                                                        // Robert Preissl, June 6 2007: find out if it is a function- or class-template to fill the right list
                                                                                        SgTemplateSymbol* template_symbol = isSgTemplateSymbol(p_symbol);
                                                                                        ROSE_ASSERT(template_symbol != NULL);
                                                                                        SgTemplateDeclaration* template_decl = template_symbol->get_declaration();
                                                                                        ROSE_ASSERT(template_decl != NULL);

                                                                                        // class or nested class template
                                                                                        if(template_decl->get_template_kind() == 1 || template_decl->get_template_kind() == 2) {

                                                                                                AvailableHidden_Types.insert(p_symbol);

                                                                                        }
                                                                                        // function or member function template
                                                                                        else if(template_decl->get_template_kind() == 3 || template_decl->get_template_kind() == 4 ) {

                                                                                                AvailableHidden_Functions_Variables.insert(p_symbol);

                                                                                        }

                                                                                }

                                                                        }

                                                                }
                                                                // if isSgFunctionSymbol:
                                                                else {

                                                                        // C++ Standard page 219: "A locally declared function is not in the same scope as a function in a containing scope"
                                                                        // Motivating example:
                                                                        //  int f(char*);
                                                                        //  void g() {
                                                                        //         extern f(int); // hides f(char*)
                                                                        //  }

                                                                        // -> so compare Scopes of the the (name-)matching functions, if they are in different scopes -> entry in hiddenlist
                                                                        SgFunctionSymbol* func_symbol = isSgFunctionSymbol(p_symbol);
                                                                        if(func_symbol != NULL) {

                                                                                SgFunctionDeclaration* p_symbol_f_decl = func_symbol->get_declaration();
                                                                                ROSE_ASSERT(p_symbol_f_decl != NULL);

                                                                                if(f_decl->get_scope() != p_symbol_f_decl->get_scope()) {

                                                                                        AvailableHidden_Functions_Variables.insert(p_symbol);

                                                                                }
                                                                                else {

#ifdef HIDDEN_LIST_DEBUG
                                                                                        cout << "isSgFunctionSymbol: Function can never hide a function in valid c++ code -> function overloading! " << endl;
#endif

                                                                                }

                                                                        }

                                                                }

                                                        }
                                                        // not a variable, type, function, enum, class, namespace!!
                                                        else {
#ifdef HIDDEN_LIST_DEBUG
                                                                cout << " >> Intersection: not a variable, type, function, enum, class, namespace!! : " << p_symbol->sage_class_name() << endl;
#endif


                                                        }

                                                } // no_insert
                                                else {

#ifdef HIDDEN_LIST_DEBUG
                                                        cout << "NO INSERT!!"<< endl;
#endif

                                                }

                                        }

                                }

                        }


                }

        }

}

} // namespace

