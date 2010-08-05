#include <rose.h>
#include <iostream>
#include "GlobalDatabaseConnectionMYSQL.h"
#include "TableDefinitions.h"
DEFINE_TABLE_PROJECTS();
DEFINE_TABLE_GRAPHDATA();
DEFINE_TABLE_GRAPHNODE();
DEFINE_TABLE_GRAPHEDGE();
CREATE_TABLE_2( simpleFuncTable,  int,projectId, string,functionName);
DEFINE_TABLE_2( simpleFuncTable,  int,projectId, string,functionName);
#define TABLES_DEFINED 1

#include "DatabaseGraph.h"
typedef DatabaseGraph<simpleFuncTableRowdata, EdgeTypeEmpty> CallGraph;
bool debugOut = true; // output information about traversal to stderr

//-----------------------------------------------------------------------------
// define traversal classes 

// Build an inherited attribute storing the current function defintion scope
class MyInheritedAttribute
{
  public:
    MyInheritedAttribute () { mFunctionDefinitionId=-1; };
    MyInheritedAttribute ( SgNode* astNode ) { mFunctionDefinitionId=-1; }; // FIXME why is this necessary?
    MyInheritedAttribute (const MyInheritedAttribute &X) { 
      mFunctionDefinitionId = X.mFunctionDefinitionId; };

    void setFunctionDefinitionId(int set) { mFunctionDefinitionId = set; }
    int getFunctionDefinitionId()         { return mFunctionDefinitionId; }
    
  protected:

    // id of the current function definition scope
    int mFunctionDefinitionId;
};

// tree traversal, search for function calls in side definitions
class MyTraversal
: public SgTopDownProcessing<MyInheritedAttribute>
{
  public:
    MyTraversal() 
      : SgTopDownProcessing<MyInheritedAttribute>()
      { };

    MyTraversal (
        int setPID,
        GlobalDatabaseConnection *setGdb,
        CallGraph *setGraph) 
      : SgTopDownProcessing<MyInheritedAttribute>(),
        mpGDB( setGdb ), mProjectId( setPID), mpCallgraph( setGraph )
      { };

    // Functions required by the tree traversal mechanism
    MyInheritedAttribute evaluateInheritedAttribute (
        SgNode* astNode,
        MyInheritedAttribute inheritedAttribute ) 
    {
      switch(astNode->variantT())
      {
        case V_SgFunctionDefinition: 
          {
            // found a function definition, remember it as the source for future calls
            SgFunctionDeclaration *funcDec = isSgFunctionDeclaration( astNode->get_parent() );
            if(debugOut) cerr << " found V_SgFunctionDefinition of " <<funcDec->get_name().str()<< endl; 
            if(funcDec) {
              // get function DB entry
              TableAccess< simpleFuncTableRowdata > simpleFunc( mpGDB );
              simpleFuncTableRowdata data( UNKNOWNID , mProjectId, funcDec->get_name().str() );
              long funcId = simpleFunc.retrieveCreateByColumn( 
                &data, "functionName", data.get_functionName(), data.get_projectId() );
              inheritedAttribute.setFunctionDefinitionId( funcId );
              mpCallgraph->insertVertex( data, data.get_functionName() );
            }
          } break;
        case V_SgFunctionCallExp: 
          {
            // found a function call
            if(debugOut) cerr << " found V_SgFunctionCallExp ... "; 
            if(inheritedAttribute.getFunctionDefinitionId() <= 0) break; // not in a recognized definition
            SgFunctionDeclaration *funcDec = NULL;
            SgFunctionRefExp *funcRef = isSgFunctionRefExp( ((SgFunctionCallExp *)astNode)->get_function() );
            if(funcRef) {
              // found a standard function reference 
              funcDec = funcRef->get_symbol_i()->get_declaration();
              if(debugOut) cerr << " std " ; if(funcDec) cerr << funcDec->get_name().str(); 
            }

            // see if its a member function call
            SgDotExp *funcDotExp = isSgDotExp( ((SgFunctionCallExp *)astNode)->get_function() );
            if(funcDotExp) {
	      SgMemberFunctionRefExp *membFunc = isSgMemberFunctionRefExp( 
		funcDotExp->get_rhs_operand() );
	      funcDec = membFunc->get_symbol_i()->get_declaration();
              if(debugOut) cerr << " memb " ; if(funcDec) cerr << funcDec->get_name().str() ; 
            }

            if(funcDec) {
              // get function DB entry
              TableAccess< simpleFuncTableRowdata > simpleFunc( mpGDB );
              simpleFuncTableRowdata data( UNKNOWNID , mProjectId, funcDec->get_name().str() );
              simpleFunc.retrieveCreateByColumn( 
                &data, "functionName", data.get_functionName(), data.get_projectId() );
              // make sure declaration is in the graph with correct name
              mpCallgraph->insertVertex( data, data.get_functionName() );

              simpleFuncTableRowdata caller; // retrieve information about calling function
              int selRet = simpleFunc.selectById( inheritedAttribute.getFunctionDefinitionId(), &caller );
              assert( selRet == 0);

              // add callgraph edge
              mpCallgraph->insertEdge( caller, data );
              if(debugOut) cerr << " edge "; 
            }
            if(debugOut) cerr << " , done." << endl; 
          } break;
        default:
          break;
      }

      MyInheritedAttribute returnAttribute(inheritedAttribute);
      return returnAttribute;
    };
    
  protected:

    // keep a connection to the database for the callgraph
    GlobalDatabaseConnection *mpGDB;

    // the id of the current project
    int mProjectId;

    // the current callgaph
    CallGraph *mpCallgraph;
};


//-----------------------------------------------------------------------------
// build a simple callgraph - restrictions: e.g. function pointers, virtual 
// functions, recursive calls etc.
int main(int argc, char *argv[]) {
  GlobalDatabaseConnection db;
  int initOk =  db.initialize();
  assert( initOk==0 );

  CREATE_TABLE(db, projects);
  CREATE_TABLE(db, graphdata);
  CREATE_TABLE(db, graphnode);
  CREATE_TABLE(db, graphedge);

  TableAccess< simpleFuncTableRowdata > simpleFunc( &db );
  simpleFunc.initialize();

  // initialize project
  string projectName = "testProject"; 
  projectsRowdata prow( UNKNOWNID ,projectName, UNKNOWNID );
  projects.retrieveCreateByColumn( &prow, "name", 
      projectName );
  long projectId = prow.get_id();

  // init graph
  CallGraph *callgraph = new CallGraph( projectId, GTYPE_SIMPLECALLGRAPH, &db );
  callgraph->loadFromDatabase( );

  // traverse the source code... 
  SgProject* project = frontend(argc,argv);
  MyTraversal treeTraversal( projectId, &db, callgraph);
  MyInheritedAttribute inheritedAttribute( project );
  treeTraversal.traverse( project,inheritedAttribute );

  // save graph to dot file, and to database
  callgraph->writeToDOTFile( "simplecallgraph_example.dot" );
  callgraph->writeToDatabase( );
  delete callgraph;

  db.shutdown();
  return( 0 );
}

