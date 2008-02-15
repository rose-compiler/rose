/****************************************************************************
**			TAU Portable Profiling Package			   **
**			http://www.acl.lanl.gov/tau		           **
*****************************************************************************
**    Copyright 1997  						   	   **
**    Department of Computer and Information Science, University of Oregon **
**    Advanced Computing Laboratory, Los Alamos National Laboratory        **
****************************************************************************/
/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1994                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/

/*
 * pcxx_timer.h: timer definitions and macros
 *
 * (c) 1994 Jerry Manic Saftware
 *
 * Version 3.0
 */

/* Written by Bernd Mohr & Pete Beckman */
/* Last overhauled Jun 1994             */

/* Things are setup, so that the different machines are recognized   */
/* automatically, if this file is compiled with a decent compiler.   */
/* EXCEPTION: There is no way in a C program of distinguishing       */
/*            between a Sun and a Meiko, and a Rs6000 and a SP1.     */
/*            The user of this package has to set __MEIKO__ or       */
/*            __SP1__ respectively.                                  */
/* NOTE: This package was primarily written for parallel machines.   */
/*       However, it runs also on basically every UNIX workstation,  */
/*       but it makes no use of special high resolution timers on    */
/*       these machines (as these are VERY unportable).              */

#ifndef __PCXX_TIMERS_H__
#define __PCXX_TIMERS_H__


// Hack. For now, we've disabled PCXX_EVENT
#     define PCXX_EVENT(c, e, p)
#       define PCXX_WTIMER_CLEAR    60199
#       define PCXX_WTIMER_START    60100
#       define PCXX_WTIMER_STOP     60200
#       define PCXX_UTIMER_CLEAR    60399
#       define PCXX_UTIMER_START    60300
#       define PCXX_UTIMER_STOP     60400

#ifdef  __cplusplus
#  define LANG "C"
#else
#  define LANG
#endif /* __cplusplus */

/*********************************************************************/
/* long pcxx_GetUSecL:    get microseconds as long (for tracing)     */
/* double pcxx_GetUSecD:  get microseconds as double (for profiling) */
/* double tulip_WallClock                                             */
/*********************************************************************/

#ifdef __PARAGON__
/* -- clock resolution 100ns -- */
   extern LANG double pcxxT_dclock ();
   extern LANG double pcxxT_start;
#  define pcxx_GetUSecL()  ((pcxxT_dclock() - pcxxT_start) * 0.1)
#  define pcxx_GetUSecD()  (pcxxT_dclock() * 0.1)
#  define tulip_WallClock() (pcxxT_dclock() * 1.0e-7)
#else
#ifdef __ksr__
#  include <ksr/ksr_timers.h>
#  define pcxx_GetUSecL()  x_all_timer()
#  define pcxx_GetUSecD()  (all_seconds() * 1.0e6)
#  define tulip_WallClock() (all_seconds())
#  define tulip_UserClock() (user_seconds())
#else
#if defined(_SEQUENT_) || defined(sequent)
/* -- clock resolution 1us -- */
#  include <usclkc.h>
#  define pcxx_GetUSecL()  GETUSCLK()
#  define pcxx_GetUSecD()  ((double) GETUSCLK())
#  define tulip_WallClock() ((double) GETUSCLK() * 1.0e-6)
#else
#ifdef butterfly
   extern LANG long getusecclock ();
#  define pcxx_GetUSecL()  getusecclock()
#  define pcxx_GetUSecD()  ((double) getusecclock())
#  define tulip_WallClock() ((double) getusecclock() * 1.0e-6)
#else
/* added SP1_T instead of SP1 by elj 3/26/97 -- tb0time not available on SP2 */
#ifdef __SP1_T__
/* -- clock resolution 1ms -- */
   extern LANG double pcxxT_time;
   extern LANG double pcxxT_start;
   extern LANG void tb0time(double*);
#  define pcxx_GetUSecL()  (tb0time(&pcxxT_time),(pcxxT_time-pcxxT_start)*1.0e3)
#  define pcxx_GetUSecD()  (tb0time(&pcxxT_time),pcxxT_time*1.0e3)
#  define tulip_WallClock() (tb0time(&pcxxT_time),pcxxT_time*1.0e-3)
#else
#ifdef __MEIKO__
#  define pcxx_GetUSecL()
#  define pcxx_GetUSecD()
#  define tulip_WallClock()
#else
#ifdef __T3D__
/* -- clock speed 150MHz -- */
#  include <time.h>
#  define pcxx_GetUSecL()  (rtclock() / 150)
#  define pcxx_GetUSecD()  ((double) rtclock() * 0.6666666667e-2)
#  define tulip_WallClock() ((double) rtclock() * 0.6666666667e-8)
#  define tulip_UserClock() ((double) cpused() * 0.6666666667e-8)
#  define tulip_InitClocks()
#else
#ifdef _CRAYT3E
/* -- clock speed 300MHz -- */
#  include <time.h>
#  define pcxx_GetUSecL()  (rtclock() / 300)
#  define pcxx_GetUSecD()  ((double) rtclock() * 0.3333333333e-2)
#  define tulip_WallClock() ((double) rtclock() * 0.3333333333e-8)
#  define tulip_UserClock() ((double) cpused() * 0.3333333333e-8)
#  define tulip_InitClocks()
#else
   extern LANG unsigned long int pcxxT_GetClock ();
#  define pcxx_GetUSecL()  pcxxT_GetClock()
#  define pcxx_GetUSecD()  ((double) pcxxT_GetClock())
#  define tulip_WallClock() ((double) pcxxT_GetClock() * 1.0e-6)
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif

/*******************************************************************/
/* Declarations for the rest of the Timers and Clocks Package      */
/*******************************************************************/
#if !defined(__T3D__) && !defined(_CRAYT3E)
extern LANG void tulip_InitClocks ();
#endif
#if !defined(__ksr__) && !defined(__T3D__) && !defined(_CRAYT3E)
extern LANG double tulip_UserClock();
#endif
extern LANG int tulip_UserTimerClear(int);
extern LANG int tulip_UserTimerStart(int);
extern LANG int tulip_UserTimerStop(int);
extern LANG double tulip_UserTimerElapsed(int);
extern LANG int tulip_WallTimerClear(int);
extern LANG int tulip_WallTimerStart(int);
extern LANG int tulip_WallTimerStop(int);
extern LANG double tulip_WallTimerElapsed(int);

#endif  /* __PCXX_TIMERS_H__ */
