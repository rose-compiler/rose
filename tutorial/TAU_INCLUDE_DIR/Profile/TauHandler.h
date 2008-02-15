/****************************************************************************
**			TAU Portable Profiling Package			   **
**			http://www.cs.uoregon.edu/research/paracomp/tau    **
*****************************************************************************
**    Copyright 2004  						   	   **
**    Department of Computer and Information Science, University of Oregon **
**    Advanced Computing Laboratory, Los Alamos National Laboratory        **
****************************************************************************/
/***************************************************************************
**	File 		: TauHandler.h					  **
**	Description 	: TAU Profiling Package				  **
**	Author		: Sameer Shende					  **
**	Contact		: sameer@cs.uoregon.edu sameer@acl.lanl.gov 	  **
**	Documentation	: See http://www.acl.lanl.gov/tau	          **
***************************************************************************/

//////////////////////////////////////////////////////////////////////
// Routines
//////////////////////////////////////////////////////////////////////

#ifndef _TAU_HANDLER_H_
#define _TAU_HANDLER_H_
int  TauEnableTrackingMemory(void);
int  TauDisableTrackingMemory(void);
void TauEnableTrackingMuseEvents(void);
void TauDisableTrackingMuseEvents(void);
void TauSetInterruptInterval(int interval);
void TauTrackMemoryUtilization(bool allocated);
void TauTrackMuseEvents(void);
void TauTrackMemoryHere(void);
double TauGetMaxRSS(void);
int  TauGetFreeMemory(void);
void TauTrackMemoryHeadroomHere(void);
int TauEnableTrackingMemoryHeadroom(void);
int TauDisableTrackingMemoryHeadroom(void);

#endif /* _TAU_HANDLER_H_ */
  
/***************************************************************************
 * $RCSfile: TauHandler.h,v $   $Author: dquinlan $
 * $Revision: 1.1 $   $Date: 2006/04/24 00:22:01 $
 * POOMA_VERSION_ID: $Id: TauHandler.h,v 1.1 2006/04/24 00:22:01 dquinlan Exp $ 
 ***************************************************************************/

	





