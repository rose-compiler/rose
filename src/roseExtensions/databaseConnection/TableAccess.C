/******************************************************************************
 *
 * ROSE Global Database Connection
 *
 * class handling a single table
 *
 *****************************************************************************/

#include "TableAccess.h"
#include "GlobalDatabaseConnection.h"

// mysql interface
#include <sqlplus.hh>


//-----------------------------------------------------------------------------
// we need a virtual desctructor
// FIXME - create seperate C file for this function ?
TableRowdataInterface::~TableRowdataInterface()
{
	// nothing to do here
}


