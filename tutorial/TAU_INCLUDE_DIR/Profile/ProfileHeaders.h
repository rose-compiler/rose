/****************************************************************************
**			TAU Portable Profiling Package			   **
**			http://www.acl.lanl.gov/tau		           **
*****************************************************************************
**    Copyright 1997  						   	   **
**    Department of Computer and Information Science, University of Oregon **
**    Advanced Computing Laboratory, Los Alamos National Laboratory        **
****************************************************************************/
/***************************************************************************
**	File 		: ProfileHeaders.h			          **
**	Description 	: TAU Profiling Package include files	    	  **
**	Author		: Sameer Shende					  **
**	Contact		: sameer@cs.uoregon.edu sameer@acl.lanl.gov 	  **
**	Flags		: Compile with				          **
**			  -DPROFILING_ON to enable profiling (ESSENTIAL)  **
**			  -DPROFILE_STATS for Std. Deviation of Excl Time **
**			  -DSGI_HW_COUNTERS for using SGI counters 	  **
**			  -DPROFILE_CALLS  for trace of each invocation   **
**			  -DSGI_TIMERS  for SGI fast nanosecs timer	  **
**			  -DTULIP_TIMERS for non-sgi Platform	 	  **
**			  -DPOOMA_STDSTL for using STD STL in POOMA src   **
**			  -DPOOMA_TFLOP for Intel Teraflop at SNL/NM 	  **
**			  -DPOOMA_KAI for KCC compiler 			  **
**			  -DDEBUG_PROF  for internal debugging messages   **
**                        -DPROFILE_CALLSTACK to enable callstack traces  **
**	Documentation	: See http://www.acl.lanl.gov/tau	          **
***************************************************************************/
#ifndef _PROFILE_HEADERS_H_
#define _PROFILE_HEADERS_H_

#include <string.h>

#if (defined(TAU_DOT_H_LESS_HEADERS) || defined (TAU_STDCXXLIB))
#include <string>
#include <vector>
#include <utility>
#include <list>
#include <map>
using std::string;
#define TAU_STD_NAMESPACE std::
#ifdef TAU_LIBRARY_SOURCE
using std::vector;
using std::pair;
using std::list;
using std::map;
#endif /* TAU_LIBRARY_SOURCE */
#else
#define __BOOL_DEFINED 
#include "Profile/bstring.h"
#include <vector.h>
#include <map.h>
#include <list.h>
#include <pair.h>
#endif /* TAU_DOT_H_LESS_HEADERS  || TAU_STDCXXLIB */

#ifndef NO_RTTI /* RTTI is present */
#ifdef RTTI 
#include <typeinfo.h>
#else /* RTTI */
#include <typeinfo>
using std::type_info;
/* This is by default */ 
#endif /* RTTI */
#endif /* NO_RTTI */


#endif /* _PROFILE_HEADERS_H_ */
/***************************************************************************
 * $RCSfile: ProfileHeaders.h,v $   $Author: dquinlan $
 * $Revision: 1.1 $   $Date: 2006/04/24 00:22:00 $
 * POOMA_VERSION_ID: $Id: ProfileHeaders.h,v 1.1 2006/04/24 00:22:00 dquinlan Exp $ 
 ***************************************************************************/
