/******************************************************************************
 *
 * ROSE Global Database Test
 *
 * Classhierarchy implementation
 *
 *****************************************************************************/

#include <rose.h>
#include "Classhierarchy.h"
#include "GlobalDatabaseConnection.h"

//! compare two member function declartions, 
//  FIXME - currently only works with the name
bool compareFunctionDeclarations(SgFunctionDeclaration *f1, SgFunctionDeclaration *f2) {
        string n1 = f1->get_name().str();
        string n2 = f2->get_name().str();
        //cerr << endl;
        //cerr << " CFD n1:"<<n1<<" n2:"<<n2<< endl;
        if( n1 != n2 ) return false;

        //if( n1 == n2 ) cerr << " SAME!!!! " << endl;
        //n1 = "i";
        //n2 = "i";
        //if( n1 == n2 ) cerr << " SAME!!!! " << endl;
        //cerr << endl;
        //cerr << " CFD t1:"<<f1->get_type()->get_mangled_type().str()<<" t2:"<<f2->get_type()->get_mangled_type().str()<< endl;
        //cerr << " CFD r1:"<<f1->get_type()->get_return_type()->get_mangled().str()<<" r2:"<<f2->get_type()->get_return_type()->get_mangled().str()<< endl;
        //cerr << endl;

        SgFunctionType *t1 = f1->get_type();
        SgFunctionType *t2 = f2->get_type();

        //cerr << " CFD tt1 ";
        if( t1->isConst() != t2->isConst() ) return false;
        //cerr << " CFD tt2 ";
        //cerr << " r1:'"<<t1->get_return_type()->get_mangled().str()<<"' r2:'"<<t2->get_return_type()->get_mangled().str()<<"' "<< ( (t1->get_return_type()->get_mangled().str()) == (t2->get_return_type()->get_mangled().str()) ) << endl;
        // ??? doesnt work? char *ts1 = t1->get_return_type()->get_mangled().str(); char *ts2 = t2->get_return_type()->get_mangled().str();
        //cerr << " CFD tt2__ ";
        //if( (!ts1) || (!ts2) ) return false; // FIXME - this shouldnt be necessary?
        if( (!t1->get_return_type()->get_mangled().str()) || (!t2->get_return_type()->get_mangled().str()) ) return false; // FIXME - this shouldnt be necessary?
        //cerr << " CFD tt2_ "  ;
        //cerr << "' "<< ( strcmp(t1->get_return_type()->get_mangled().str(), t2->get_return_type()->get_mangled().str()) ) << " ;;; " << strcmp(ts1,ts2) << endl;
        //cerr << "' "<< (int)(t1->get_return_type()->get_mangled().str()) << " . " << (int)( t2->get_return_type()->get_mangled().str()) << " ;;; " << (int)(ts1) << " . "  << (int)(ts2) << endl;
        //cerr << "'" << *ts1 << "' - '" << *ts2 << "' " << endl;
        //cerr << "'" << t1->get_return_type()->get_mangled().str() <<"' - '"<< t2->get_return_type()->get_mangled().str() <<"'" << endl;
        //cerr << "' "<< (int)(t1->get_return_type()->get_mangled().str()) << " . " << (int)( t2->get_return_type()->get_mangled().str()) << " ;;; " << (int)(ts1) << " . "  << (int)(ts2) << endl;
        //if( strcmp(ts1, ts2)!=0 ) return false;
        if( strcmp(t1->get_return_type()->get_mangled().str(), t2->get_return_type()->get_mangled().str())!=0 ) return false;

        SgTypePtrList &arg1 = t1->get_arguments();
        SgTypePtrList &arg2 = t2->get_arguments();
        //cerr << " CFD tt3 ";
        if( arg1.size() != arg2.size() ) return false;
        
        SgTypePtrList::iterator i1 = arg1.begin();
        SgTypePtrList::iterator i2 = arg2.begin();
        while(i1 != arg1.end()) {
        //cerr << " CFD tt4 ";
                const char *ps1 = (*i1)->get_mangled().str();
                const char *ps2 =       (*i2)->get_mangled().str();
                if( (!ps1) || (!ps2) ) return false; // FIXME - this shouldnt be necessary too?
                if( strcmp(ps1, ps2)!=0 ) return false;
                i1++;
                i2++;
        }

        //cerr << " CFD same! " << endl;
        //cerr << endl;
        return true;
}
                
//! init virtual fucntion inheritance
void Classhierarchy::inheritVirtualFunctions()
{
        // prepare LUT for multiple declarations
        //map<SgNode *, set<SgNode *> > multDec;
                
        property_map< dbgType, boost::vertex_classhierarchy_t>::type chMap = boost::get( boost::vertex_classhierarchy, *this );
        graph_traits< dbgType >::vertex_iterator vi,vend;
        tie(vi,vend) = vertices( *this );
        //graph_traits< dbgType >::vertex_descriptor par=*vi, succ=*vi;

        // output info
        for(; vi!=vend; vi++) {
                //cerr << " BCH v i"<< get(vertex_index,*this,*vi)<< " i1" << get(vertex_index1, *this, *vi)<<","<< get(vertex_name, *this, *vi) << endl;
                for( set<SgNode*>::iterator chd= chMap[*vi].defined.begin(); chd!= chMap[*vi].defined.end(); chd++) {
                        SgFunctionDeclaration *funcDec = isSgFunctionDeclaration( *chd );
                        //cerr << " BCH chd " << funcDec->get_mangled_name().str()<< endl; // debug
                        //cerr << " BCH chd " << funcDec->get_mangled_name().str()<< " "<< (int)funcDec->get_type() << endl; // debug
                }
        }

        // search for multiple declarations
        tie(vi,vend) = vertices( *this );
        for(; vi!=vend; vi++) {
                for(set<SgNode*>::iterator chd= chMap[*vi].defined.begin(); chd!= chMap[*vi].defined.end(); chd++) {
                        SgFunctionDeclaration *funcDec = isSgFunctionDeclaration( *chd );
                        bool found = true;
                        while(found) {
                                found = false;
                                for(set<SgNode*>::iterator chdComp = chMap[*vi].defined.begin(); 
                                                (chdComp!= chMap[*vi].defined.end())&&(!found); ) {
                                        SgFunctionDeclaration *compDec = isSgFunctionDeclaration( *chdComp );
                                        if(chdComp != chd) {
                                                //if( compDec->get_type() == funcDec->get_type() ) { // ??? TODO fix type comparison?
                                                if(compareFunctionDeclarations(compDec, funcDec)) {
                                                        //cerr << " BCH REM " << funcDec->get_mangled_name().str()<< " " << compDec->get_mangled_name().str() << endl; // debug
                                                        chMap[*vi].multDeclarations[ funcDec ].insert( compDec );
                                                        found = true;
                                                        chMap[*vi].defined.erase( chdComp ); // should return new iterator in newer STL standard??
                                                        //chVertexData::iterator chdComp2 = (chMap[*vi]).erase( chdComp );
                                                        //chdComp = chdComp2;
                //cerr << " BCH removing i"<< get(vertex_index,*this,*vi)<< " i1" << get(vertex_index1, *this, *vi)<<","<< get(vertex_name, *this, *vi) << endl;
                                                }
                                        }
                                        if(!found) chdComp++;
                                }
                        } // found
                        
                }
                //if( get( vertex_dbg_data,  *this , *vi).get_id() == edges[i].get_sourceId() ) {
                //par = *vi;
                //parFound = true;
                //}
        }


        //typedef std::deque< boostVert > container;
        //cerr << " TOPO START " << endl;
        std::deque< dbgVertex > torder;
        try {
                boost::topological_sort(*this, std::back_inserter(torder));
        } catch(boost::not_a_dag) {
                cerr << "CH -  BOOST ERROR: NOT A DAG!!!!!??? " << endl;
                assert( false );
                return;
        }
        //cerr << " TOPO END " << endl;

        //cerr << " -- " << endl; // debug
        
        for( std::deque< dbgVertex >::iterator vi=torder.begin(); vi!=torder.end(); vi++) {
                dbgVertex srcVert = *vi; // current vertex in the topo order
                //cerr << "XTOPO v i"<< get(vertex_index,*this,*vi)<< " i1" << get(vertex_index1, *this, *vi)<<","<< get(vertex_name, *this, *vi) << endl;

                for(set<SgNode*>::iterator chd= chMap[srcVert].defined.begin(); chd!= chMap[srcVert].defined.end(); chd++) {
                        SgFunctionDeclaration *defMF = isSgFunctionDeclaration( *chd );
                        
                        bool erased = true;
                        while(erased) {
                                erased = false;
                                for(set<SgNode*>::iterator inhd= chMap[srcVert].inherited.begin(); 
                                                inhd!= chMap[srcVert].inherited.end() && (!erased); ) {
                                        SgFunctionDeclaration *inhMF = isSgFunctionDeclaration( *inhd );
                                        if(compareFunctionDeclarations(defMF, inhMF)) {
                                                // own function overrides, so delete old one
                                                chMap[srcVert].inherited.erase( *inhd );
                                                erased = true;
                //cerr << "  TOPO MF:"<< defMF->get_mangled_name().str()<< " overrides MF:"<< inhMF->get_mangled_name().str() << endl; // debug
                                        }
                                        if(!erased) inhd++;
                                }
                        }
                        
                }               
                // add to inherited functions
                for(set<SgNode*>::iterator chd= chMap[srcVert].defined.begin(); chd!= chMap[srcVert].defined.end(); chd++) {
                        chMap[srcVert].inherited.insert( *chd );
                }

                // inherit own methods to child classes
                graph_traits<dbgType>::in_edge_iterator ii,iend;
                tie(ii,iend) = in_edges(*vi, *this);
                for(; ii!=iend; ii++) {
                        //dbgVertex srcVert = source(*ii,*this);
                        // methods inherited from this class to the other one
                        dbgVertex child = source(*ii, *this);
                //cerr << " T inherits to "<< get(vertex_index,*this,child)<< " i1" << get(vertex_index1, *this, child)<<","<< get(vertex_name, *this, child) << endl;
                        //for(set<SgNode*>::iterator chd= chMap[srcVert].defined.begin(); chd!= chMap[srcVert].defined.end(); chd++) {
                        for(set<SgNode*>::iterator chd= chMap[srcVert].inherited.begin(); chd!= chMap[srcVert].inherited.end(); chd++) {
                                SgFunctionDeclaration *inhFunc = isSgFunctionDeclaration( *chd );
                                bool virt = false;
                //cerr << "  TOPO v srch1" << endl;
                                if(inhFunc->isVirtual()) virt = true;
                                // also check multiple declarations
                //cerr << "  TOPO v srch2" << endl;
                                if(!virt) {
                                        for(set<SgNode *>::iterator si=chMap[srcVert].multDeclarations[inhFunc].begin(); 
                                                        si != chMap[srcVert].multDeclarations[inhFunc].end(); si++) {
                                                SgFunctionDeclaration *inhDup = isSgFunctionDeclaration( *si );
                                                if(inhDup->isVirtual()) virt = true;
                //cerr << "  TOPO v srch "<< inhDup->get_mangled_name().str() << endl; // debug
                                        }
                                }
                                        // TODO check virtual inheritance? other declarations?
                                if(virt) {
                                        //cerr << " VIRT " << inhFunc->get_mangled_name().str() << endl; // debug
                                        // and now... ??
                                }
                                chMap[child].inherited.insert( inhFunc );
                        }
                        //cerr << " BOOST v <<< " << get(vertex_index1, *this, boost::target(*ii,*this) )<<","<< get(vertex_name, *this, boost::target(*ii,*this) ) << endl; // debug
                }

        }

        // add own methods to all inherited ones
        for( std::deque< dbgVertex >::iterator vi=torder.begin(); vi!=torder.end(); vi++) {
        }

}


//! search for all possible (virtual) function calls 
vector<SgMemberFunctionDeclaration*> 
Classhierarchy::searchMemberFunctionCalls(SgMemberFunctionDeclaration* mfCall)
{
        vector<SgMemberFunctionDeclaration*> retvec;
        property_map< dbgType, boost::vertex_classhierarchy_t>::type chMap = boost::get( boost::vertex_classhierarchy, *this );

        SgClassDefinition *classDef  = mfCall->get_scope();
        SgName                                           classname = classDef->get_qualified_name(); // MANGLE
        string                                           cnamestr  = classname.str();
        graph_traits< dbgType >::vertex_iterator vi,vend;
        dbgVertex vdesc = *vi;
        bool foundVertex = false;
        tie(vi,vend) = vertices( *this );
        for(; vi!=vend; vi++) {
                //cerr << " BCH v i"<< get(vertex_index,*this,*vi)<< " i1" << get(vertex_index1, *this, *vi)<<","<< get(vertex_name, *this, *vi) << endl;
                if( get(vertex_dbg_data, *this, *vi).get_typeName() == cnamestr ) {
                        //cerr << " SMF srch "<< cnamestr <<" vi "<< get(vertex_index,*this,*vi)<< " i1" << get(vertex_index1, *this, *vi)<<","<< get(vertex_name, *this, *vi) << endl;
                        vdesc = *vi;
                        foundVertex = true;
                        break;
                }
        }
        if(!foundVertex) { cerr << " SMF srch "<< cnamestr <<" vi "<< get(vertex_index,*this,*vi)<< " i1" << get(vertex_index1, *this, *vi)<<","<< get(vertex_name, *this, *vi) << endl; }
        assert( foundVertex );

        set<dbgVertex> treeset;
        treeset.insert( vdesc );

        // first find "highest" class in CH that still provides this MF
        dbgVertex vhighest = vdesc; // first assume its the current one
        graph_traits<dbgType>::out_edge_iterator oi,oend;
        tie(oi,oend) = out_edges( vdesc, *this);
        for(; oi!=oend; oi++) {
                //cerr << " SMF inherits from "<< get(vertex_index,*this,target(*oi,*this))<< " i1" << get(vertex_index1, *this, target(*oi,*this))<<","<< get(vertex_name, *this, target(*oi,*this)) << endl;
                
                // does any of the base classes implement the member function?
                bool noParentImpl = true; 
                
                // check if this base class also implements MF
                for(set<SgNode*>::iterator chd= chMap[target(*oi,*this)].inherited.begin(); 
                                chd!= chMap[target(*oi,*this)].inherited.end(); chd++) {
                        SgFunctionDeclaration *inhFunc = isSgFunctionDeclaration( *chd );
                        bool virt = false;
                        //cerr << "  TOPO v srch1" << endl;
                        if(inhFunc->isVirtual()) virt = true;
                        if( (virt) && (compareFunctionDeclarations(inhFunc,mfCall)) ) {
                                // remeber for traversal
                                treeset.insert( target(*oi, *this) );
                                noParentImpl = false;
                        }
                }
                if(noParentImpl) {
                        // we found it
                        vhighest = target(*oi, *this);
                        break;
                }
        }
        //cerr << " SMF high "<< cnamestr <<" vi "<< get(vertex_index,*this,vhighest)<< " i1" << get(vertex_index1, *this, vhighest)<<","<< get(vertex_name, *this, vhighest) << endl; 

        // now traverse class hierachy downwards, for all children that implement this function, add to set
        set<dbgVertex> tovisit;
        set<dbgVertex> visited;
        tovisit.insert( vhighest );
        //hier weiter
        while( tovisit.size() > 0 ) {
                dbgVertex currVert = *(tovisit.begin());
                tovisit.erase( currVert );
                visited.insert( currVert );
        //cerr << " SMF visi "<< get(vertex_index,*this,currVert)<< " i1" << get(vertex_index1, *this, currVert)<<","<< get(vertex_name, *this, currVert) << endl; 
                for(set<SgNode*>::iterator chd= chMap[currVert].defined.begin(); chd!= chMap[currVert].defined.end(); chd++) {
                        SgMemberFunctionDeclaration*inhFunc = isSgMemberFunctionDeclaration( *chd );
                        if(compareFunctionDeclarations(inhFunc,mfCall)) {
                                retvec.push_back( inhFunc );
                        }
                }
                graph_traits<dbgType>::in_edge_iterator ii,iend;
                tie(ii,iend) = in_edges( currVert, *this);
                for(; ii!=iend; ii++) {
                        dbgVertex child = source(*ii, *this);
                        // only insert of not already visited
                        set<dbgVertex>::iterator found = visited.find( child );
                        if(found == visited.end()) 
                                tovisit.insert( child );
                }
        }

        //retvec.push_back( mfCall );
        return retvec;
}




/******************************************************************************
 * traversal implementation
 *****************************************************************************/


//-----------------------------------------------------------------------------
// Functions required by the tree traversal mechanism
ClasshierarchyInhAttr
ClasshierarchyTraversal::evaluateInheritedAttribute (
                SgNode* astNode,
                ClasshierarchyInhAttr inheritedAttribute )
{
        GlobalDatabaseConnection *gdb;    // db connection
        //long funcId;                                                                                  // id of a function declaration
        Classhierarchy *classhier = getClasshierarchy();
        gdb = getDB();  

        switch(astNode->variantT())
        {

                case V_SgMemberFunctionDeclaration: {
                        SgMemberFunctionDeclaration *funcDec = isSgMemberFunctionDeclaration( astNode );
                        //funcDec = funcDef->get_declaration();
                        //if(isSgMemberFunctionDeclaration(funcDec)) {
                                // add to class hierarchy if member function definition
                                //if(isSgMemberFunctionDeclaration()) {
                                //cerr << " adding CHvinf for MembFunc " << endl;
                                SgClassDefinition *classDef = isSgClassDefinition( funcDec->get_scope() );
                                //assert(classDef);
                                if(classDef) {
                                        string classname = classDef->get_qualified_name().str();
                                        // get the classhier. vertex
                                        Classhierarchy::dbgVertex chVert = 0; //?? init necessary
                                        bool foundClass = false;
                                        Classhierarchy::dbgVertexIterator chvi,chvend;
                                        boost::tie(chvi,chvend) = boost::vertices( *getClasshierarchy() );
                                        for(; chvi!=chvend; chvi++) {
                                                if( boost::get( vertex_dbg_data,  *getClasshierarchy() , *chvi).get_typeName() == classname ) {
                                                        chVert = *chvi;
                                                        foundClass = true;
                                                }
                                        }
                                        if(foundClass) {
                                                property_map< Classhierarchy::dbgType, boost::vertex_classhierarchy_t>::type chMap = boost::get( boost::vertex_classhierarchy, *getClasshierarchy() );
                                                chMap[ chVert ].defined.insert( funcDec );
                                                //get type?
                                                //cerr << " added! "; // debug
                                        }
                                }

                        //}
                        cerr << " found V_SgMemberFunctionDeclaration done for " <<funcDec->get_mangled_name().str()<< " " << endl; // debug
                        } break;

                case V_SgClassDefinition: {
                        cerr << " found V_SgClassDef of "; // debug
                        SgClassDefinition *classDef = isSgClassDefinition( astNode );
                        assert( classDef );
                        SgName classname = classDef->get_qualified_name();

                        // make db entry
                        long typeId = UNKNOWNID;
                        typesTableAccess types( gdb );
                        typesRowdata newtype( typeId, getProjectId(), classname.str() );
                        typeId = types.retrieveCreateByColumn( &newtype, "typeName", newtype.get_typeName(), newtype.get_projectId() );
                        cerr << classname.str()<< ", id:" << newtype.get_id() << endl; // debug
                        //classhier->addNode( newtype, newtype.get_typeName() );
                        //classhier->insertWithName( newtype, newtype.get_typeName() );
                        classhier->insertVertex( newtype, newtype.get_typeName() );

                        SgBaseClassList inherits = classDef->get_inheritances();
                        for( SgBaseClassList::iterator i=inherits.begin(); i!=inherits.end(); i++) {
                                SgClassDeclaration *parentDecl = (*i).get_base_class();
                                cerr << " found inheritance from " ; // debug
                                assert( parentDecl );

                                // add new edge
                                typesRowdata partype( UNKNOWNID, getProjectId(), parentDecl->get_name().str() ); // MANGLE
                                long parentId = types.retrieveCreateByColumn( &partype, "typeName", partype.get_typeName(), partype.get_projectId() );
                                cerr << parentDecl->get_name().str() << ", id: " << parentId << endl;

                                // add to class hierarchy graph, allow only one edge per inheritance
                                //A classhier->addNode( partype, partype.get_typeName() );
                                //A classhier->addEdge( newtype, partype, false );
                                classhier->insertEdge( newtype, partype );
                        }
                        
                        } break;

        } // switch node type


        // Note that we have to use a particular constructor (to pass on context information about source code position).
        // This allows the Rewrite mechanism to position new source code relative to the current position using a simple interface.
        ClasshierarchyInhAttr returnAttribute(inheritedAttribute,astNode);
        // FIXME why not return inheritedAttribute???

        return returnAttribute;
}

