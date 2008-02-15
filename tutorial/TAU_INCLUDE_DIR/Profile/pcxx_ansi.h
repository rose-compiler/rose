/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1994                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/

/*
 * pcxx_ansi.h: ANSI C compatibility macros
 *
 * (c) 1994 Jerry Manic Saftware
 *
 * Version 3.0
 */

#ifndef	__PCXX_ANSI_H__
#define	__PCXX_ANSI_H__

#if defined (__cplusplus) || defined (__STDC__) || defined (_AIX) || (defined (__mips) && defined (_SYSTYPE_SVR4))

#define SIGNAL_TYPE	void
#define SIGNAL_ARG_TYPE	int

#else	/* Not ANSI C.  */

#define SIGNAL_TYPE	int
#define SIGNAL_ARG_TYPE

#endif	/* ANSI C */

#endif	/* __PCXX_ANSI_H__ */
