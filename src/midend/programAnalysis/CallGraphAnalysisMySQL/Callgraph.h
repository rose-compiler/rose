/******************************************************************************
 *
 * ROSE Global Database Test
 *
 * Callgraph implementation
 *
 *****************************************************************************/


#ifndef CALLGRAPH_H
#define CALLGRAPH_H

#include "/home/thuerey1/local/include/config.h"

#include "DatabaseGraph.h"
#include "Classhierarchy.h"

#include "TableDefinitions.h"
#ifndef TABLES_DEFINED
USE_TABLE_FUNCTIONS();
#endif

//typedef set<SgMemberFunctionType *> chVertexData;
//typedef set<SgNode *> chVertexData;

class callgraphVertexData {
        public:
                //! data to store with a CG node
                int dummy;
};

// install boost database graph property map types
namespace boost {
        enum vertex_callgraph_t { vertex_callgraph };
        BOOST_INSTALL_PROPERTY( vertex, callgraph );
}


//! callgraph, implemented with a boost graph
class Callgraph : 
public DatabaseGraph<functionsRowdata, EdgeTypeEmpty, 
        vecS, vecS, bidirectionalS, property< vertex_callgraph_t, callgraphVertexData, GraphvizVertexProperty>, 
        GraphvizEdgeProperty, 
        GraphvizGraphProperty > 
{
        public:
                //! constructor
                Callgraph( long pid, long type, GlobalDatabaseConnection *gdb ) :
                        DatabaseGraph<functionsRowdata, EdgeTypeEmpty, vecS, vecS, bidirectionalS, 
                        property< vertex_callgraph_t, callgraphVertexData, GraphvizVertexProperty>, 
                        GraphvizEdgeProperty, 
                        GraphvizGraphProperty >(pid,type,gdb)  { };

                //! destructor
                ~Callgraph() { };

        private:
};




/******************************************************************************
 * callgraph generation traversal classes
 *****************************************************************************/

//! constants whether function has or has no side effects (or whether its currently unknown)
#define SIDEEFFECTS_UNDECIDED 1
#define SIDEEFFECTS_NO                          2
#define SIDEEFFECTS_YES                         3


//! constants for implicit/explicit function calls
#define FUNCCALL_IMPLICIT 1
#define FUNCCALL_EXPLICIT 2


// Build an inherited attribute for the callgraph traversal
class CallgraphInhAttr
{
        public:
                //! Specific constructors are required
                CallgraphInhAttr ( SgNode* astNode ) :
                        mFunctionScopes() { };

                CallgraphInhAttr ( const CallgraphInhAttr & X, SgNode* astNode ) { 
                        mFunctionScopes = X.mFunctionScopes;
                };

                //! is the current function scope correctly initialized?
                bool hasValidFunctionScope( void ){ 
                        return ( mFunctionScopes.size() > 0); }

                //! get current function scope
                long getCurrentFunctionScope( void ){ 
                        assert( mFunctionScopes.size() > 0);
                        return mFunctionScopes[ mFunctionScopes.size()-1 ];
                }
                //! add scope function id
                long pushFunctionScope(long id){ mFunctionScopes.push_back( id ); }

        private:

                //! current context of function scopes
                vector<long> mFunctionScopes;
                
};




// tree traversal for callgraph generation
class CallgraphTraversal
: public SgTopDownProcessing<CallgraphInhAttr>
{
        public:
                CallgraphTraversal ( SgProject & project )
                        : SgTopDownProcessing<CallgraphInhAttr>() 
                {
                        mProjectId = -1;
                        mFileId = -1;
                        mCallgraph = NULL;
                        mClasshierarchy = NULL;
                        mpFunctionDefinitions = NULL;
                        mpSgProject = NULL;
                };

                // Functions required by the tree traversal mechanism
                CallgraphInhAttr evaluateInheritedAttribute (
                                SgNode* astNode,
                                CallgraphInhAttr inheritedAttribute );


                //! remove functions row entries from DB using mpFunctionDefinitions
                void removeDeletedFunctionRows( void );


                //! set function def vector
                void setFunctionDefinitions( vector<long> *set ){ mpFunctionDefinitions = set; };
                //! get function def vector
                vector<long> *getFunctionDefinitions( void ) { return mpFunctionDefinitions; };

                //! set file scope vector
                void setFileScopes( vector<long> *set ){ mpFileScopes = set; };
                //! get file scope vector
                vector<long> *getFileScopes( void ) { return mpFileScopes; };

                //! set DB connection
                void setDB( GlobalDatabaseConnection *set ){ mpGDB = set; };
                //! get DB connection
                GlobalDatabaseConnection *getDB( void ) { return mpGDB; };

                //! set project ID
                void setProjectId( long set ){ mProjectId = set; };
                //! get project ID
                long getProjectId( void ){ return mProjectId; };

                //! set file ID
                void setFileId( long set ){ mFileId = set; };
                //! get file ID
                long getFileId( void ){ return mFileId; };

                //! set pointer to the callgraph graph object
                //void setCallgraph( DatabaseGraph<functionsRowdata, EdgeTypeEmpty> *set ) { mCallgraph = set; }
                void setCallgraph( Callgraph *set ) { mCallgraph = set; }
                //! get pointer to the callgraph graph object
                Callgraph *getCallgraph( void ) { return mCallgraph; }

                //! set pointer to the Classhierarchy graph object
                void setClasshierarchy( Classhierarchy *set ) { mClasshierarchy = set; }
                //! get pointer to the Classhierarchy graph object
                Classhierarchy *getClasshierarchy( void ) { return mClasshierarchy; }

                //! set pointer to the SgProject graph object
                void setSgProject( SgProject *set ) { mpSgProject = set; }
                //! get pointer to the SgProject graph object
                SgProject *getSgProject( void ) { return mpSgProject; }

                //! get callgraph file subgraph storage
                set<int> *getCGFileSubgraphs( void ){ return &mCGFileSubgraphs; }

                //! set query decl list
                void setDeclList( list<SgNode*> *set ) { mpDeclQList = set; }
                

        private:

                //! add an edge for the call to this function to the callgraph
                void addCallEdge(SgFunctionDeclaration *funcDec, long functionScope);
                

                // ! DB connection pointer
                GlobalDatabaseConnection *mpGDB;

                //! ID of the current project
                long mProjectId;
                
                //! ID of the current file
                long mFileId;
                
                //! graph to store the callgraph
                //DatabaseGraph<functionsRowdata, EdgeTypeEmpty> *mCallgraph;
                Callgraph *mCallgraph;
                
                //! graph to store the class hierarchy
                Classhierarchy *mClasshierarchy;

                //! vector of encoutered function definitions (needed for detection of deleted functions)
                vector<long> *mpFunctionDefinitions;

                //! vector of all id's of the processed files (for cleanup)
                vector<long> *mpFileScopes;

                //! pointer to the project node
                SgProject *mpSgProject;

                //! inited callgraph file subgraph values
                set<int> mCGFileSubgraphs;

                //! pointer to a query list of declaration nodes for quicker query
                list<SgNode*> *mpDeclQList;

};



#endif



