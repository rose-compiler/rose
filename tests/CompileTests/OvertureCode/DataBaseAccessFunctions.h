#ifndef DATA_BASE_ACCESS_FUNCTIONS_H
#define DATA_BASE_ACCESS_FUNCTIONS_H

// Define some functions that get Grids from a database

extern const aString nullString;

int initializeMappingList();
int destructMappingList();

int getFromADataBase(CompositeGrid & cg, 
		     const aString & fileName, 
		     const aString & gridName=nullString,
                     const bool & checkTheGrid=FALSE );

int findDataBaseFile( aString & fileName, const bool & searchCommonLocations=TRUE );

#endif
