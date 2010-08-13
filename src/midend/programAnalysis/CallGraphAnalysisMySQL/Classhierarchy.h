/******************************************************************************
 *
 * ROSE Global Database Test
 *
 * Classhierarchy implementation
 *
 *****************************************************************************/

#ifndef CLASSHIERARCHY_H
#define CLASSHIERARCHY_H

#include "/home/thuerey1/local/include/config.h"

#include "DatabaseGraph.h"
#ifndef TABLES_DEFINED
USE_TABLE_TYPES();
#endif

//typedef set<SgMemberFunctionType *> chVertexData;
//typedef set<SgNode *> chVertexData;

class chVertexData {
        public:
                //! own and inherited methods
                set<SgNode *> defined;
                set<SgNode *> inherited;
                map<SgNode *, set<SgNode *> > multDeclarations;
};

// install boost database graph property map types
namespace boost {
        enum vertex_classhierarchy_t { vertex_classhierarchy };
        BOOST_INSTALL_PROPERTY( vertex, classhierarchy );
}


//! class hierarchy does not have multiple edges, so "setS" is used as vertex list
class Classhierarchy : 
public DatabaseGraph<typesRowdata, EdgeTypeEmpty, 
        setS, vecS, bidirectionalS, property< vertex_classhierarchy_t, chVertexData, GraphvizVertexProperty>, 
        GraphvizEdgeProperty, 
        GraphvizGraphProperty > 
{
        public:
                //! constructor
                Classhierarchy( long pid, long type, GlobalDatabaseConnection *gdb ) :
                        DatabaseGraph<typesRowdata, EdgeTypeEmpty, setS, vecS, bidirectionalS, 
                        property< vertex_classhierarchy_t, chVertexData, GraphvizVertexProperty>, 
                        GraphvizEdgeProperty, 
                        GraphvizGraphProperty >(pid,type,gdb)  { };

                //! destructor
                ~Classhierarchy() { };

                //! init virtual fucntion inheritance
                void inheritVirtualFunctions();

                //! search for all possible (virtual) function calls 
                vector<SgMemberFunctionDeclaration*> searchMemberFunctionCalls(SgMemberFunctionDeclaration*);

        private:
};




/******************************************************************************
 * traversal classes
 *****************************************************************************/


// Build an inherited attribute for the classhierarchy traversal
class ClasshierarchyInhAttr
{
        public:
                //! Specific constructors are required
                ClasshierarchyInhAttr ( SgNode* astNode ) { };

                ClasshierarchyInhAttr ( const ClasshierarchyInhAttr & X, SgNode* astNode ) { };

        private:

                //! currently nothing needed...
};


// tree traversal for class hierarchy generation
class ClasshierarchyTraversal
: public SgTopDownProcessing<ClasshierarchyInhAttr>
{
        public:
                ClasshierarchyTraversal ( SgProject & project )
                        : SgTopDownProcessing<ClasshierarchyInhAttr>() 
                {
                        mProjectId = -1;
                        mFileId = -1;
                        mClasshierarchy = NULL;
                        mpSgProject = NULL;
                };

                // Functions required by the tree traversal mechanism
                ClasshierarchyInhAttr evaluateInheritedAttribute (
                                SgNode* astNode,
                                ClasshierarchyInhAttr inheritedAttribute );


                //! remove functions row entries from DB using mpFunctionDefinitions
                void removeDeletedFunctionRows( void );


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

                //! set pointer to the Classhierarchy graph object
                void setClasshierarchy( Classhierarchy *set ) { mClasshierarchy = set; }
                //! get pointer to the Classhierarchy graph object
                Classhierarchy *getClasshierarchy( void ) { return mClasshierarchy; }

                //! set pointer to the SgProject graph object
                void setSgProject( SgProject *set ) { mpSgProject = set; }
                //! get pointer to the SgProject graph object
                SgProject *getSgProject( void ) { return mpSgProject; }


        private:
                
                // ! DB connection pointer
                GlobalDatabaseConnection *mpGDB;

                //! ID of the current project
                long mProjectId;
                
                //! ID of the current file
                long mFileId;
                
                //! graph to store the class hierarchy
                Classhierarchy *mClasshierarchy;

                //! vector of all id's of the processed files (for cleanup)
                vector<long> *mpFileScopes;

                //! pointer to the project node
                SgProject *mpSgProject;
};



#endif



