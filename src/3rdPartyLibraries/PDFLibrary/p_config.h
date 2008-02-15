/*---------------------------------------------------------------------------*
 |        PDFlib - A library for dynamically generating PDF documents        |
 +---------------------------------------------------------------------------+
 |        Copyright (c) 1997-2000 Thomas Merz. All rights reserved.          |
 +---------------------------------------------------------------------------+
 |    This software is NOT in the public domain.  It can be used under two   |
 |    substantially different licensing terms:                               |
 |                                                                           |
 |    The commercial license is available for a fee, and allows you to       |
 |    - ship a commercial product based on PDFlib                            |
 |    - implement commercial Web services with PDFlib                        |
 |    - distribute (free or commercial) software when the source code is     |
 |      not made available                                                   |
 |    Details can be found in the file PDFlib-license.pdf.                   |
 |                                                                           |
 |    The "Aladdin Free Public License" doesn't require any license fee,     |
 |    and allows you to                                                      |
 |    - develop and distribute PDFlib-based software for which the complete  |
 |      source code is made available                                        |
 |    - redistribute PDFlib non-commercially under certain conditions        |
 |    - redistribute PDFlib on digital media for a fee if the complete       |
 |      contents of the media are freely redistributable                     |
 |    Details can be found in the file aladdin-license.pdf.                  |
 |                                                                           |
 |    These conditions extend to ports to other programming languages.       |
 |    PDFlib is distributed with no warranty of any kind. Commercial users,  |
 |    however, will receive warranty and support statements in writing.      |
 *---------------------------------------------------------------------------*/

/* p_config.h
 *
 * PDFlib portability and configuration definitions
 *
 */

#ifndef P_CONFIG_H
#define P_CONFIG_H

/* undef this if you don't have setlocale() */
#define HAVE_SETLOCALE

/* ---------------------------------- WIN32  -------------------------------- */

/* try to identify Windows compilers */

#if (defined _WIN32 || defined __WATCOMC__ || defined __BORLANDC__ || (defined(__MWERKS__) && defined(__INTEL__))) && !defined WIN32
#define	WIN32
#endif	/* <Windows compiler>  && !defined WIN32 */

#ifdef	WIN32
#define READMODE	"rb"
#define WRITEMODE	"wb"
#define PDF_PLATFORM	"Win32"
#endif	/* WIN32 */

/* --------------------------------- Cygnus  -------------------------------- */

#ifdef __CYGWIN__
#define READMODE	"rb"
#define WRITEMODE	"wb"
#ifdef DLL_EXPORT
    #define PDFLIB_EXPORTS
#endif

#endif /* __CYGWIN__ */

/* ---------------------------------- DJGPP  -------------------------------- */

#ifdef __DJGPP__
#define READMODE	"rb"
#define WRITEMODE	"wb"
#define PDF_PLATFORM	"Win32/DJGPP"
#endif /* __DJGPP__ */

/* ----------------------------------- OS/2  -------------------------------- */

/*
 * Try to identify OS/2 compilers.
 */

#if (defined __OS2__ || defined __EMX__) && !defined OS2
#define OS2
#endif

#ifdef	OS2
#define READMODE	"rb"
#define WRITEMODE	"wb"
#define unlink		remove
#define PDF_PLATFORM	"OS/2"
#endif	/* OS2 */

/* --------------------------------- Macintosh ------------------------------ */

/* try to identify Mac compilers */

#if (defined macintosh || defined __POWERPC__ || defined __CFM68K__ || defined __MC68K__) && !defined MAC
#define MAC
#endif

#ifdef	MAC
#define READMODE	"rb"
#define WRITEMODE	"wb"
#define PATHSEP		":"

/* Adjust the following HAVE_LIB* according to the availability of libraries */
#define HAVE_LIBZ
#define HAVE_LIBPNG
#define HAVE_LIBTIFF

#define PDF_PLATFORM	"MacOS"
#endif	/* MAC */

/* --------------------------------- AS/400 --------------------------------- */

/* try to identify the AS/400 compiler */

#if	defined __ILEC400__ && !defined AS400
#define	AS400
#endif	/* __ILEC400__ */

#ifdef AS400

/* This must match the CCSID used for compiling PDFlib */
#define PDF_CODEPAGE 819
#define WRITEMODE	"wb"
#define READMODE	"rb"

#define PDF_PLATFORM	"AS/400"
#endif	/* AS400 */

/* ----------------------------------- OS/390  ------------------------------ */

#ifdef	OS390

/* This must match the CCSID used for compiling PDFlib */
#define PDF_CODEPAGE 1047
#define WRITEMODE	"wb"
#define PATHSEP		"("

#define PDF_PLATFORM	"OS/390"

#endif	/* OS390 */


/* ------------------------------------ VMS --------------------------------- */

/* Not much support yet */

#ifdef	VMS
#define	PATHSEP ""	/* what should we reasonably do here? */
#define READMODE	"rb", "mbc=60", "ctx=stm", "rfm=fix", "mrs=512"
#define WRITEMODE	"wb", "mbc=60", "ctx=stm", "rfm=fix", "mrs=512"
#define unlink		remove
#define PDF_PLATFORM	"VMS"
#endif	/* VMS */

/* --------------------------------- Defaults ------------------------------- */

#ifndef READMODE
#define READMODE	"r"
#endif	/* !READMODE */

#ifndef WRITEMODE
#define WRITEMODE	"w"
#endif	/* !WRITEMODE */

#ifndef M_PI
#define M_PI		3.14159265358979323846      /* pi */
#endif	/* !M_PI */

#ifndef PATHSEP
#define PATHSEP		"/"
#endif	/* !PATHSEP */

#ifdef	_DEBUG
#define DEBUG
#endif	/* !_DEBUG */

/* figure out whether or not we're running on an EBCDIC-based machine */
#define	ASCII_A			0x41
#define PLATFORM_A		'A'
#define EBCDIC_BRACKET		0x4A
#define PLATFORM_BRACKET	'['

#if (ASCII_A != PLATFORM_A && EBCDIC_BRACKET == PLATFORM_BRACKET)
#define PDFLIB_EBCDIC
#endif

#ifndef PDF_PLATFORM
#define PDF_PLATFORM	"Unknown OS"
#endif

#endif	/* P_CONFIG_H */
