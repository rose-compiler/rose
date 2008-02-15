/****************************************************************************
**			TAU Portable Profiling Package			   **
**			http://www.acl.lanl.gov/tau		           **
*****************************************************************************
**    Copyright 2004  						   	   **
**    Department of Computer and Information Science, University of Oregon **
**    Advanced Computing Laboratory, Los Alamos National Laboratory        **
****************************************************************************/
/***************************************************************************
**	File 		: TauCompensate.h				  **
**	Description 	: TAU Profiling Package				  **
**	Author		: Sameer Shende					  **
**	Contact		: sameer@cs.uoregon.edu 			  **
**	Flags		: Compile with				          **
**			  -DTAU_COMPENSATE for instrumentation overhead   **
**			   compensation to correct profiles               **
**			  -DPROFILING_ON to enable profiling (ESSENTIAL)  **
**	Documentation	: http://www.cs.uoregon.edu/research/paracomp/tau **
***************************************************************************/

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#ifndef _TAU_COMPENSATE_H_
#define _TAU_COMPENSATE_H_

#ifdef TAU_COMPENSATE

enum TauOverhead { TauNullTimerOverhead, TauFullTimerOverhead };
void TauCalibrateOverhead(void);
#ifndef TAU_MULTIPLE_COUNTERS
double TauGetSingleTimerOverhead(void);
double TauGetTimerOverhead(enum TauOverhead);
#else /* TAU_MULTIPLE_COUNTERS */
double* TauGetSingleTimerOverhead(void);
double* TauGetTimerOverhead(enum TauOverhead);
#endif /* TAU_MULTIPLE_COUNTERS */



#endif /* TAU_COMPENSATE */

#endif /* _TAU_COMPENSATE_H_ */



