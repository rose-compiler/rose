/******************************************************************************
 *
 * ROSE Global Database Connection
 *
 * class handling a single table
 *
 *****************************************************************************/

#include "TableAccess.h"
#include "GlobalDatabaseConnectionMYSQL.h"

// mysql interface
#include <mysql++.h>


//-----------------------------------------------------------------------------
// we need a virtual desctructor
// FIXME - create separate C file for this function ?
TableRowdataInterface::~TableRowdataInterface()
{
	// nothing to do here
}


