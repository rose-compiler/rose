/******************************************************************************
 *
 * ROSE Global Database Test
 *
 * macros for table defintion object creation
 *
 *****************************************************************************/


#include "TableAccess.h"


//! register a table to the global database for init
#define CREATE_TABLE(DB,TABLE) \
		TableAccess< TABLE##Rowdata > TABLE; \
		TABLE.setGlobalDatabaseConnection( &(DB) ); \
		TABLE.initialize(); \


//! macros to create default rose tables

#define USE_TABLE_PROJECTS() \
	CREATE_TABLE_2( projects, \
			string,name , \
			int,callgraphRootId ); \
	typedef TableAccess< projectsRowdata > projectsTableAccess; \


#define DEFINE_TABLE_PROJECTS() \
	USE_TABLE_PROJECTS(); \
	DEFINE_TABLE_2( projects, \
			string,name , \
			int,callgraphRootId ); 



#define USE_TABLE_FILES() \
	CREATE_TABLE_2( files, \
			int,projectId, \
			textString,fileName ); \
	typedef TableAccess< filesRowdata > filesTableAccess;\
	

#define DEFINE_TABLE_FILES() \
	USE_TABLE_FILES(); \
	DEFINE_TABLE_2( files, \
			int,projectId, \
			textString,fileName ); 



#define USE_TABLE_FUNCTIONS() \
	CREATE_TABLE_4( functions, \
			int,projectId, \
			textString,functionName, \
			short,hasSideEffects, \
			int, definitionFileId ); \
	typedef TableAccess< functionsRowdata > functionsTableAccess; \

#define DEFINE_TABLE_FUNCTIONS() \
	USE_TABLE_FUNCTIONS(); \
	DEFINE_TABLE_4( functions, \
			int,projectId, \
			textString,functionName, \
			short,hasSideEffects, \
			int,definitionFileId ); 



#define USE_TABLE_TYPES() \
	CREATE_TABLE_2( types, \
			int,projectId, \
			textString,typeName ); \
	typedef TableAccess< typesRowdata > typesTableAccess; \

#define DEFINE_TABLE_TYPES() \
	USE_TABLE_TYPES(); \
	DEFINE_TABLE_2( types, \
			int,projectId, \
			textString,typeName ); 



#define USE_TABLE_GRAPHDATA() \
	CREATE_TABLE_2( graphdata, \
			int,projectId, \
			int,graphType ); \
	typedef TableAccess< graphdataRowdata > graphdataTableAccess; \

#define DEFINE_TABLE_GRAPHDATA() \
	USE_TABLE_GRAPHDATA(); \
	DEFINE_TABLE_2( graphdata, \
			int,projectId, \
			int,graphType ); 



#define USE_TABLE_GRAPHNODE() \
	CREATE_TABLE_3( graphnode, \
			int,graphId, \
			int,nodeId, \
		 	string,name	); \
	typedef TableAccess< graphnodeRowdata > graphnodeTableAccess; \

#define DEFINE_TABLE_GRAPHNODE() \
	USE_TABLE_GRAPHNODE(); \
	DEFINE_TABLE_3( graphnode, \
			int,graphId, \
			int,nodeId, \
		 	string,name	); 



#define USE_TABLE_GRAPHEDGE() \
	CREATE_TABLE_4( graphedge, \
			int,graphId, \
                        int,edgeId, \
			int,sourceId, \
			int,targetId ); \
	typedef TableAccess< graphedgeRowdata > graphedgeTableAccess; \

#define DEFINE_TABLE_GRAPHEDGE() \
	USE_TABLE_GRAPHEDGE(); \
	DEFINE_TABLE_4( graphedge, \
			int,graphId, \
                        int,edgeId, \
			int,sourceId, \
			int,targetId ); 




#define USE_TABLE_METATABLE() \
	CREATE_TABLE_1( metatable, string,tableName ); \
	typedef TableAccess< metatableRowdata > metatableTableAccess; \

#define DEFINE_TABLE_METATABLE() \
	USE_TABLE_METATABLE(); \
	DEFINE_TABLE_1( metatable, string,tableName ); 




