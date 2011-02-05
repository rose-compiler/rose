/******************************************************************************
 *
 * ROSE Global Database Test
 *
 * Callgraph traversal implementation
 *
 *****************************************************************************/


#include <rose.h>
// database access
#include "Callgraph.h"
USE_TABLE_FILES();


//! helper function to add .dot file position information from a ROSE located node pointer
template<class GraphType, class GraphVertex>
void addDotLocation( SgNode *node, string label, GraphVertex &vdesc, GraphType &G ) {
        SgLocatedNode *locnode = isSgLocatedNode(node);
        if(!locnode) return;
        Sg_File_Info *startinf  = locnode->get_startOfConstruct(); 
        if(startinf) {
                //cerr << " SINF " << startinf->get_filename() << " " << startinf->get_line() << endl; // debug
                Sg_File_Info *endinf            = locnode->get_endOfConstruct(); 
                ostringstream dest;
                dest << "file://" << startinf->get_filename() << ":" << startinf->get_line() << ":0";
        
                if(endinf) {
                        //cerr << " EINF " << endinf->get_filename() << " " << endinf->get_line() << endl; // debug
                        // use start & end
                        if(endinf->get_line() != startinf->get_line()) 
                                dest << ":" << endinf->get_line() << ":0";
                } else {
                        // just use start info
                }

                // set graphviz attr map
                boost::get( boost::vertex_attribute, G )[vdesc][label] = dest.str();
                //cerr << " addDotLocation: added '"<< dest.str() <<"' "<< boost::get( boost::vertex_name, G )[vdesc] << endl;
                return;
        }
        //Sg_File_Info *startinf        = funcCall->get_startOfConstruct(); if(startinf) cerr << " SINF " << startinf->get_filename() << " " << startinf->get_line() << endl; // debug
        //Sg_File_Info *endinf          = funcCall->get_endOfConstruct(); if(endinf) cerr << " EINF " << endinf->get_filename() << " " << endinf->get_line() << endl; // debug
        //cerr << " addDotLocation: added nothing "<< endl;
}


// not working...
// class definition pointer is only working for local definition:
//      class c {
//              virtual int foo( void ) {};
//      };
// not for:
//      class c {
//              virtual int foo( void );
//      };
// int c::foo( void ) { };
//


NodeQuerySynthesizedAttributeType myDeclQueryBROKEN(SgNode* astNode, SgNode* functionDefinition)
{
        ROSE_ASSERT(astNode != 0);

        NodeQuerySynthesizedAttributeType returnNodeList;

        //SgNode* sageReturnNode = NULL;
        SgMemberFunctionDeclaration* sageMemberFunctionDeclaration = isSgMemberFunctionDeclaration(astNode);

        if(sageMemberFunctionDeclaration != NULL) {
                        cerr <<" q found mfdecl "<< (unsigned long)functionDefinition << endl;

                        { SgLocatedNode *node = isSgLocatedNode( sageMemberFunctionDeclaration );
                        Sg_File_Info *startinf  = node->get_startOfConstruct(); 
                        if(startinf) cerr << "        SINF " << startinf->get_filename() << " " << startinf->get_line() << endl; } // debug

                        { SgLocatedNode *node = isSgLocatedNode( functionDefinition );
                        Sg_File_Info *startinf  = node->get_startOfConstruct(); 
                        if(startinf) cerr << "        SFDE " << startinf->get_filename() << " " << startinf->get_line() << endl; } // debug

                if(isSgClassDefinition(sageMemberFunctionDeclaration->get_parent())) {
                        cerr <<"        q found cdef "<< endl;

                        if(sageMemberFunctionDeclaration->get_definition()) {
                        { SgLocatedNode *node = isSgLocatedNode(sageMemberFunctionDeclaration->get_definition());
                        Sg_File_Info *startinf  = node->get_startOfConstruct(); 
                        if(startinf) cerr << "        SMD2 " << startinf->get_filename() << " " << startinf->get_line() << endl; } // debug
                        } else {
                                cerr << "   SDM2 NULL " << endl;
                        }

                        if(isSgNode(sageMemberFunctionDeclaration->get_definition()) == functionDefinition) {
                        cerr <<"        q comp def "<< endl;
                                returnNodeList.push_back(astNode);
                        }
                }
        }

        return returnNodeList;
}/* End function querySolverUnionFields() */

// search function declarations by name
NodeQuerySynthesizedAttributeType myDeclQuery(SgNode* astNode, SgNode* functionDecl)
{
        ROSE_ASSERT(astNode != 0);
        NodeQuerySynthesizedAttributeType returnNodeList;
        SgFunctionDeclaration *funcDec = isSgFunctionDeclaration( functionDecl );
        string fname( funcDec->get_mangled_name().str() ); // MANGLE

        //SgNode* sageReturnNode = NULL;
        SgMemberFunctionDeclaration* sageMemberFunctionDeclaration = isSgMemberFunctionDeclaration(astNode);

        if(sageMemberFunctionDeclaration != NULL) {
                if(isSgClassDefinition(sageMemberFunctionDeclaration->get_parent())) {
                        string fother( sageMemberFunctionDeclaration->get_mangled_name().str() ); // MANGLE
                        if(fother == fname) {
                                returnNodeList.push_back(astNode);
                        }
                }
        }

        return returnNodeList;
}/* End function querySolverUnionFields() */


// search class declarations by type
NodeQuerySynthesizedAttributeType myClassDeclQuery(SgNode* astNode, SgNode* type)
{
        ROSE_ASSERT(astNode != 0);
        NodeQuerySynthesizedAttributeType returnNodeList;
        SgType *classType = isSgType( type );

        SgClassDeclaration *classDec = isSgClassDeclaration( astNode );
        if( classDec ) {
                cerr << " CDS " << classDec->get_type()->get_name().str() << endl;
                if(classDec->get_type() == classType) {
                        cerr << " FOUND!!!!!!!!! ";
                        returnNodeList.push_back( astNode );
                }
        }
        return returnNodeList;
}



//! add an edge for the call to this function to the callgraph
void CallgraphTraversal::addCallEdge(SgFunctionDeclaration *funcDec, long functionScope) 
{
        GlobalDatabaseConnection *gdb = getDB();
        // get function DB entry
        functionsTableAccess functions( gdb );
        functionsRowdata data(
                        UNKNOWNID , getProjectId(), 
                        funcDec->get_mangled_name().str(), SIDEEFFECTS_UNDECIDED,  // MANGLE
                        UNKNOWNID );
        //funcId = functions.retrieveCreateByColumn( &data, "functionName", data.get_functionName(), data.get_projectId() );
        functions.retrieveCreateByColumn( &data, "functionName", data.get_functionName(), data.get_projectId() );
        // make sure declaration is in the graph with correct name
        //getCallgraph()->insertWithName( data, data.get_functionName() );
        //DatabaseGraph<functionsRowdata, EdgeTypeEmpty>::dbgVertex vdesc;
        Callgraph::dbgVertex vdesc;
        vdesc = getCallgraph()->insertVertex( data, data.get_functionName() );
        addDotLocation( funcDec,"sourcefile", vdesc, *getCallgraph() );

        //cerr << " found callexp of "<< data.get_functionName() << " at "<< getCurrentFunctionScope()  << endl; // debug
        //Sg_File_Info *startinf        = funcCall->get_startOfConstruct(); if(startinf) cerr << " SINF " << startinf->get_filename() << " " << startinf->get_line() << endl; // debug
        //Sg_File_Info *endinf          = funcCall->get_endOfConstruct(); if(endinf) cerr << " EINF " << endinf->get_filename() << " " << endinf->get_line() << endl; // debug

        functionsRowdata caller;
        int selRet = functions.selectById( functionScope, &caller );
        assert( selRet == 0);

        // add callgraph edge
        //A callgraph->addNode( data, data.get_functionName() );
        //A callgraph->addEdge( caller, data );
        getCallgraph()->insertEdge( caller, data );
}


                                
//-----------------------------------------------------------------------------
// Functions required by the tree traversal mechanism
CallgraphInhAttr
CallgraphTraversal::evaluateInheritedAttribute (
                SgNode* astNode,
                CallgraphInhAttr inheritedAttribute )
{
        GlobalDatabaseConnection *gdb;    // db connection
        //long funcId;                                                                                  // id of a function declaration
        //DatabaseGraph<functionsRowdata,EdgeTypeEmpty> *callgraph = getCallgraph();
        Callgraph *callgraph = getCallgraph();
        //DatabaseGraph<typesRowdata,EdgeTypeEmpty>     *classhier = getClasshierarchy();
        //Classhierarchy *classhier = getClasshierarchy();
        gdb = getDB();  

        switch(astNode->variantT())
        {
   // case V_SgFile:
   // case V_SgBinaryComposite:
      case V_SgSourceFile:
        {
                        cerr << " found V_SgSourceFile or SgBinaryComposite" << endl; // debug
                        // found a file node

      // DQ (9/2/2008): Modified to support SgSourceFile IR node
                        SgSourceFile *fileNode = isSgSourceFile( astNode );
         ROSE_ASSERT(fileNode != NULL);

                        filesTableAccess files( gdb );

                        // get file info FIXME?
                        //Sg_File_Info *fileinfo = fileNode->get_file_info();
                        filesRowdata filedata( UNKNOWNID, getProjectId(), fileNode->getFileName() );
                        files.retrieveCreateByColumn( &filedata, "fileName", filedata.get_fileName(), filedata.get_projectId() );
                        setFileId( filedata.get_id() );
                        getFileScopes()->push_back( filedata.get_id() );
                        
                        // clear all entries for defined functions in this table
                        // FIXME this will invalidate graph ids...????
                        // FIXME detect constructor calls?

                        /*
                        functionsRowdata funcrow;
                        ostringstream delfuncs;
                        delfuncs << "DELETE FROM " << funcrow.getName() << " WHERE definitionFileId='" << getFileId() << "' ; " ;
                        //cerr << " delete old func entries: " <<deledges.str().c_str()<< endl;
                        mpGDB->execute( deledges.str().c_str() );
                        */

                        // get filenames...
                        /*for(int i= 0;i<fileNode->get_numberOfSourceFileNames();i++) {
                                char *filename = ( fileNode->get_sourceFileNamesWithPath() )[i];
                                filesRowdata filedata( UNKNOWNID, getProjectId(), filename);
                                files.retrieveCreateByName( &filedata, filedata.get_fileName(), 2, filedata.get_id() );
                                //cerr << " found file " << filedata.id <<" "<<filedata.fileName<< endl; // debug

                                setFileId( filedata.get_id() );
                                if(i>0) {
                                        // FIXME what to do when there are multiple files??
                                        assert(false);
                                }
                        }*/
                        } break;

                case V_SgFunctionDefinition: {
                        // found a function definition
                        SgFunctionDefinition *funcDef = isSgFunctionDefinition( astNode );
                        if(! funcDef->get_body() ) {
                                // function has no body, so this isnt a real definition -> quit
                                break;
                        }

                        //cerr << " found one ... " << endl; // debug
                        SgFunctionDeclaration *funcDec = isSgFunctionDeclaration( astNode->get_parent() );
                        //if(funcDec->isVirtual()) { cerr << " VIRTUAL " ; // debug } 
                        cerr << " found V_SgFunctionDefinition of " <<funcDec->get_mangled_name().str()<< endl; // debug
                        if(funcDec) {
                                // get function DB entry
                                functionsTableAccess functions( gdb );
                                functionsRowdata data(
                                                UNKNOWNID , getProjectId(), 
                                                funcDec->get_mangled_name().str(), SIDEEFFECTS_UNDECIDED, 
                                                UNKNOWNID );
                                long funcId = functions.retrieveCreateByColumn( &data, "functionName", data.get_functionName(), data.get_projectId() );
                                data.set_definitionFileId( getFileId() );
                                functions.modify( &data );
                                //cerr << " found id " << funcId<<" " << getFileId() << endl; // debug

                                // init subgraph
                                filesTableAccess files( gdb );
                                int fileId = data.get_definitionFileId();
                                filesRowdata filedata( UNKNOWNID, UNKNOWNID, "" );
                                files.selectById( fileId, &filedata );

                                if(getCGFileSubgraphs()->find(fileId) != getCGFileSubgraphs()->end()) {
                                        string filename = filedata.get_fileName();
                                        //getCallgraph()->addSubgraph( filename );
                                        getCGFileSubgraphs()->insert( fileId );
                                }
                                                
                                // add current scope to stack
                                inheritedAttribute.pushFunctionScope( funcId );
                                // remember we encerrered this definition in this run
                                getFunctionDefinitions()->push_back( funcId );
                                // there should be a node for every function definition -> add one
                                //A getCallgraph()->addNode( data, data.get_functionName() );
                                //getCallgraph()->insertWithName( data, data.get_functionName() );
                                Callgraph::dbgVertex vdesc;
                                vdesc = getCallgraph()->insertVertex( data, data.get_functionName(), 
                                                filedata.get_id(), filedata.get_fileName() );
                                addDotLocation( funcDec,"sourcefile", vdesc, *getCallgraph() );
                                
                                // we found the function scope, so we can clear all successors in the callgraph that already exist
                                getCallgraph()->clearSuccessors( data );
                        }

                        cerr << " found V_SgFunctionDefinition done for " <<funcDec->get_mangled_name().str()<< endl; // debug
                        } break;

                case V_SgCtorInitializerList: {
                        // a constructor initializer list may also contain function calls
                        SgFunctionDeclaration *funcDec = isSgFunctionDeclaration( astNode->get_parent() );
                        cerr << " found V_SgCtorInitializerList for " <<funcDec->get_mangled_name().str()<< endl; // debug
                        // found a function definition
                        if(funcDec) {
                                // get function DB entry
                                functionsTableAccess functions( gdb );
                                functionsRowdata data( UNKNOWNID , getProjectId(), 
                                                funcDec->get_mangled_name().str(), SIDEEFFECTS_UNDECIDED, UNKNOWNID );
                                long funcId = functions.retrieveCreateByColumn( &data, "functionName", data.get_functionName(), data.get_projectId() );

                                // add current scope to stack
                                inheritedAttribute.pushFunctionScope( funcId );
                                // the successors should already be cleared from definition
                        }
                        } break;

                case V_SgFunctionCallExp: {
                        cerr << " found V_SgFunctionCallExp ... "; // debug
                        if(!inheritedAttribute.hasValidFunctionScope()) { cerr << " invalid function scope? at ";
                                Sg_File_Info *startinf  = astNode->get_startOfConstruct(); if(startinf) cerr << " start: " << startinf->get_filename() << " " << startinf->get_line();
                                Sg_File_Info *endinf            = astNode->get_endOfConstruct(); if(endinf) cerr << " end: " << endinf->get_filename() << " " << endinf->get_line(); 
                                cerr << endl;
                                break; }

                        // found a function call
                        SgFunctionCallExp *funcCall = isSgFunctionCallExp( astNode );
                        // get function call expression
                        SgExpression *funcExp = funcCall->get_function();

                        SgFunctionRefExp *funcRef = isSgFunctionRefExp( funcExp );
                        if(funcRef) {
                                // found a standard function reference 
                                //cerr << " found refcallexp " << endl; // debug
                                SgFunctionSymbol  *funcSymb = NULL;
                                funcSymb = funcRef->get_symbol_i();

                                // insert...
                                SgFunctionDeclaration *funcDec = funcSymb->get_declaration();
                                cerr << " std " << funcDec->get_mangled_name().str() << endl; // debug
                                addCallEdge( funcDec, inheritedAttribute.getCurrentFunctionScope() );
                        }

                        // standard member function call
                        SgDotExp *funcDotExp = isSgDotExp( funcExp );
                        if(funcDotExp) {
                                cerr << " dot " ; // debug
                                // found a member function call
                                SgMemberFunctionDeclaration *funcDec = NULL;
                                SgMemberFunctionSymbol  *funcSymb = NULL;
                                SgMemberFunctionRefExp *funcMemb = isSgMemberFunctionRefExp( funcDotExp->get_rhs_operand() );
                                if(funcMemb) {
                                        // found the member function reference
                                        //cerr << " found membcallexp " << endl; // debug
                                        funcSymb = funcMemb->get_symbol_i();
                                        funcDec = funcSymb->get_declaration();
                                }

                                cerr << " std memb " << funcDec->get_mangled_name().str() << endl; // debug
                                addCallEdge( funcDec, inheritedAttribute.getCurrentFunctionScope() );
                        }

                        // handle virtual function calls, this is much more complicated than a standard call, 
                        // so it is handled separately
                        SgArrowExp *funcArrowExp = isSgArrowExp( funcExp );
                        if(funcArrowExp) {
                                cerr << " arrow " ; // debug
                                // found a member function call
                                // handle virtual function calls
                                SgMemberFunctionDeclaration *funcDec = NULL;
                                SgMemberFunctionSymbol  *funcSymb = NULL;
                                SgMemberFunctionRefExp *funcMemb = isSgMemberFunctionRefExp( funcArrowExp->get_rhs_operand() );
                                if(funcMemb) {
                                        // found the member function reference
                                        //cerr << " found membcallexp " << endl; // debug
                                        funcSymb = funcMemb->get_symbol_i();
                                        funcDec = funcSymb->get_declaration();
                                }

                                // try to get the declaration from class
                                //cerr << " START Q " << endl; // debug
                                //break; // DEBUG !!!!
                                //list<SgNode*> decList = NodeQuery::querySubTree(getSgProject(), funcDec, myDeclQuery );
                                list<SgNode*> decList = NodeQuery::queryNodeList( *mpDeclQList, funcDec, myDeclQuery );
                                for(list<SgNode*>::iterator i=decList.begin(); i!=decList.end(); i++) {
                                        //SgLocatedNode *node = isSgLocatedNode( *i );
                                        //Sg_File_Info *startinf        = node->get_startOfConstruct(); if(startinf) cerr << " SINF " << startinf->get_filename() << " " << startinf->get_line() << endl; // debug
                                        SgMemberFunctionDeclaration *classFuncDec = isSgMemberFunctionDeclaration( *i );
                                        if(classFuncDec) funcDec = classFuncDec;
                                }
                                //cerr << " END Q " << endl; // debug

                                assert(funcDec);
                                if(funcDec->isStatic()) cerr << " STATIC " ; // debug
                                if(funcDec->isVirtual()) {
                                        cerr << " VIRTUAL " ; // debug
                                        // TODO treat virtual functions...

                                        vector<SgMemberFunctionDeclaration*> mfcalls;
                                        mfcalls = getClasshierarchy()->searchMemberFunctionCalls( funcDec );
                                        for(size_t i=0; i<mfcalls.size(); i++) {
                                                addCallEdge( mfcalls[i], inheritedAttribute.getCurrentFunctionScope() );
                                        }
                                }  else {
                                        // std non virtual one...
                                        addCallEdge( funcDec, inheritedAttribute.getCurrentFunctionScope() );
                                }
                                cerr << " memb " << funcDec->get_mangled_name().str() << endl; // debug
                                
                        }

                        } break;
                case V_SgConstructorInitializer: {

                        //break; // DEBUG!!!

                                //Sg_File_Info *startinf        = astNode->get_startOfConstruct(); if(startinf) cerr << " SINF " << startinf->get_filename() << " " << startinf->get_line() << endl; // debug
                                //Sg_File_Info *endinf          = astNode->get_endOfConstruct(); if(endinf) cerr << " EINF " << endinf->get_filename() << " " << endinf->get_line() << endl; // debug
                        cerr << " found V_SgConstructorInitializer of "; // debug
                        if(!inheritedAttribute.hasValidFunctionScope()) { cerr << " invalid function scope? at ";
                                Sg_File_Info *startinf  = astNode->get_startOfConstruct(); if(startinf) cerr << " start: " << startinf->get_filename() << " " << startinf->get_line();
                                Sg_File_Info *endinf            = astNode->get_endOfConstruct(); if(endinf) cerr << " end: " << endinf->get_filename() << " " << endinf->get_line(); 
                                cerr << endl;
                                break; }

                        // found a constructor call
                        SgConstructorInitializer *consCall = isSgConstructorInitializer( astNode );
                        SgFunctionDeclaration *funcDec = (SgFunctionDeclaration *)consCall->get_declaration(); // this returns a member func declaration
                        // might be a more complicated expression, if declaration not inited it's not a simple contructor call
                        if(!funcDec) break;
                        
                        SgMemberFunctionDeclaration *funcMemb = isSgMemberFunctionDeclaration(funcDec);
                        //cerr << " " << funcMemb->get_qualified_name().str() << endl; // debug
                        cerr << " " << funcDec->get_mangled_name().str() << " "; // debug
                        //this can also be an expression, handle other cases here, see testprog.c L. 60
                        //GEHT NICHT!

                        // get function DB entry
                        functionsTableAccess functions( gdb );
                        functionsRowdata data(
                                        UNKNOWNID , getProjectId(), 
                                        funcDec->get_mangled_name().str(), SIDEEFFECTS_UNDECIDED, 
                                        UNKNOWNID );
                        functions.retrieveCreateByColumn( &data, "functionName", data.get_functionName(), data.get_projectId() );
                        //cerr << " found conscall of "<< data.get_functionName() << " at "<< getCurrentFunctionScope()  << endl; // debug

                        functionsRowdata caller;
                        int selRet = functions.selectById( inheritedAttribute.getCurrentFunctionScope(), &caller );
                        assert( selRet == 0);

                        // add callgraph edge
                        //A callgraph->addNode( data, data.get_functionName() );
                        //A callgraph->addEdge( caller, data );
                        callgraph->insertEdge( caller, data );
                        // TODO add destructor call ?
                        break;

                        // add destructor call
                        //cerr << " adding V_SgConstructorInitializer of destructor call " << endl; // debug
                        //cerr << " TYPE "<<consCall->get_type()->get_mangled().str() <<endl;
                        if( isSgNamedType( consCall->get_type() )) {
                                //cerr << " TYPE "<< ((SgNamedType*)(consCall->get_type()))->get_name().str() <<endl;
                        }
                        //SgClassDeclaration *classDec = consCall->get_class_decl();
                        SgClassDeclaration *classDec = funcMemb->get_scope()->get_declaration();

                        // try to get the declaration from class
                        /*SgType *consType = consCall->get_type();
                        list<SgNode*> decList = NodeQuery::querySubTree(getSgProject(), consType, myClassDeclQuery );
                        for(list<SgNode*>::iterator i=decList.begin(); i!=decList.end(); i++) {
                                SgClassDeclaration *classDecFound = isSgClassDeclaration( *i );
                                if(classDecFound) classDec = classDecFound;
                        }*/
                        assert( classDec );

                        string tilde("~");
                        string destName = tilde + classDec->get_name().str(); // ??? FIXME null pointer?
                        //string destName = tilde + funcDec->get_name().str();
                        functionsRowdata destr(
                                        UNKNOWNID , getProjectId(), 
                                        destName, SIDEEFFECTS_UNDECIDED, UNKNOWNID );
                        functions.retrieveCreateByColumn( &destr, "functionName", destr.get_functionName(), destr.get_projectId() );
                        // add callgraph edge for destructor
                        //A callgraph->addNode( destr, destr.get_functionName() );
                        //A callgraph->addEdge( caller, destr );
                        callgraph->insertEdge( caller, destr );
                        cerr << " done " << endl;

                        } break;

                /*case V_SgMemberFunctionDeclaration: {
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
                                        Classhierarchy::dbgVertex chVert =-1; //?? init necessary
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
                                                cerr << " added! ";
                                        }
                                }

                        //}
                        cerr << " found V_SgMemberFunctionDeclaration done for " <<funcDec->get_name().str()<< " }}}}}}}}}}}}}}}}}}}} " << endl; // debug
                        } break;
                */

                /*case V_SgClassDefinition: {
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
                                typesRowdata partype( UNKNOWNID, getProjectId(), parentDecl->get_name().str() );
                                long parentId = types.retrieveCreateByColumn( &partype, "typeName", partype.get_typeName(), partype.get_projectId() );
                                cerr << parentDecl->get_name().str() << ", id: " << parentId << endl;

                                // add to class hierarchy graph, allow only one edge per inheritance
                                //A classhier->addNode( partype, partype.get_typeName() );
                                //A classhier->addEdge( newtype, partype, false );
                                classhier->insertEdge( newtype, partype );
                        }
                        
                        } break;*/

        } // switch node type


        // Note that we have to use a particular constructor (to pass on context information about source code position).
        // This allows the Rewrite mechanism to position new source code relative to the current position using a simple interface.
        CallgraphInhAttr returnAttribute(inheritedAttribute,astNode);
        // FIXME why not return inheritedAttribute???

        return returnAttribute;
}

//-----------------------------------------------------------------------------
// remove functions row entries from DB using mpFunctionDefinitions
void CallgraphTraversal::removeDeletedFunctionRows( void ) {
        functionsTableAccess functions( getDB() );
        
        // get all DB entries of funtion defs for this all files processed
        for(unsigned int f=0; f<mpFileScopes->size(); f++) {
                
        ostringstream cmd;
        cmd << " definitionFileId='" << (*mpFileScopes)[f] << "' " ;
        vector<functionsRowdata> res = functions.select( cmd.str() );
        
        // check if one of these is not there anymore
        //cerr << " removeDeletedFunctionRows - " << res.size() <<" "<< cmd.str() << endl; // debug
        for(unsigned int j=0; j<res.size(); j++) {
                long currId = res[j].get_id();
                //cerr << " removeDeletedFunctionRows - checking " << currId << endl; // debug
                bool found = false;
                for(unsigned int i=0; i<mpFunctionDefinitions->size(); i++) {
                        if( (*mpFunctionDefinitions)[i] == currId ) {
                                found = true;
                                (*mpFunctionDefinitions)[i] = -1;
                        }
                }

                if(!found) {
                        Callgraph *callgraph = getCallgraph();
                        functionsRowdata delfunc;
                  functions.selectById( currId, &delfunc );
                        // check if there are  callgraph references to it
                        if(! callgraph->nodeIsUsed( delfunc )) {
                                cerr << " removeDeletedFunctionRows - removing " << currId << endl; // debug
                                functions.remove( &delfunc );
                                callgraph->removeNode( delfunc );
                        }
                } // !found
        }
        
        } // all files
}

