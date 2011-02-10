/******************************************************************************
 *
 * ROSE Callgraph and Classhierarchy Test
 *
 *****************************************************************************/

#include <iostream>
#include <assert.h>

// use and define table objects
#include "TableDefinitions.h"
DEFINE_TABLE_PROJECTS();
DEFINE_TABLE_FILES();
DEFINE_TABLE_FUNCTIONS();
DEFINE_TABLE_GRAPHDATA();
DEFINE_TABLE_GRAPHNODE();
DEFINE_TABLE_GRAPHEDGE();
DEFINE_TABLE_TYPES();

// "notify" other header files that all necessary tables are defined
#define TABLES_DEFINED

// include the other header files
#include "GlobalDatabaseConnection.h"
#include "Callgraph.h"
#include "Classhierarchy.h"

//-----------------------------------------------------------------------------
// main - create test instances
int main(int argc, char *argv[]) {

        GlobalDatabaseConnection db;
        bool initOk =  db.initialize();
        assert( initOk==0 );

        CREATE_TABLE(db, projects);
        CREATE_TABLE(db, files);
        CREATE_TABLE(db, functions);
        CREATE_TABLE(db, graphdata);
        CREATE_TABLE(db, graphedge);
        CREATE_TABLE(db, types);
        
        // initialize project
        long projId = UNKNOWNID; // as of yet undefined - this _HAS_ to be '0' first AUTO_INCREMENT to work!
        string projName = "testCallgraphProject";

        projectsRowdata projdata( projId ,projName, UNKNOWNID);
        projects.retrieveCreateByColumn( &projdata, "name", projName );
        projId = projdata.get_id();
        cout << " PID " << projId << endl;

        // init callgraph
        Callgraph *callgraph = new Callgraph( projId, GTYPE_CALLGRAPH, &db );
        callgraph->setSubgraphInit( 4, new filesRowdata(), 2 );
        callgraph->loadFromDatabase( );

        // init class hierarchy
        Classhierarchy *classhier = new Classhierarchy( projId, GTYPE_CLASSHIERARCHY, &db );
        classhier->loadFromDatabase( );

        // invoke frontend
        SgProject* project = frontend(argc,argv);


        // generate class hierarchy
        ClasshierarchyTraversal clhTrav(*project);
        ClasshierarchyInhAttr clhInhAttr(project);
        vector<long> clhFuncDefs;
        clhTrav.setDB( &db );
        clhTrav.setProjectId( projId );
        clhTrav.setClasshierarchy( classhier );
        clhTrav.setSgProject( project );
        // traverse the AST, build classhierarchy
        clhTrav.traverse(project,clhInhAttr);
        // prepare virtual function tables
        classhier->inheritVirtualFunctions();


        // generate callgraph object
        CallgraphTraversal cgTrav(*project);
        CallgraphInhAttr cgInhAttr(project);

        vector<long> funcDefs;
        vector<long> fileScopes;
        cgTrav.setDB( &db );
        cgTrav.setProjectId( projId );
        cgTrav.setCallgraph( callgraph );
        cgTrav.setClasshierarchy( classhier );
        cgTrav.setFunctionDefinitions( &funcDefs );
        cgTrav.setFileScopes( &fileScopes );
        cgTrav.setSgProject( project );

        list<SgNode *> declList;
        declList = NodeQuery::querySubTree( project, NodeQuery::FunctionDeclarations );
        cgTrav.setDeclList( &declList );

        // traverse the AST, build the callgraph
        cgTrav.traverse(project,cgInhAttr);


        // update database, write callgraph dot file
        cgTrav.removeDeletedFunctionRows();
        callgraph->writeToDOTFile( "callgraphExampleCallgraph.dot" );
        callgraph->writeToDatabase( );
        delete callgraph;

        // update database, write classhierarchy dot file
        classhier->writeToDOTFile( "callgraphExampleClassHierarchy.dot" );
        classhier->writeToDatabase( );
        delete classhier;

        db.shutdown();
        return 0;
}



