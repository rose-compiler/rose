/*---------------------------------------------------------------------------*
 |              PDFlib - A library for generating PDF on the fly             |
 +---------------------------------------------------------------------------+
 | Copyright (c) 1997-2005 Thomas Merz and PDFlib GmbH. All rights reserved. |
 +---------------------------------------------------------------------------+
 |                                                                           |
 |    This software is subject to the PDFlib license. It is NOT in the       |
 |    public domain. Extended versions and commercial licenses are           |
 |    available, please check http://www.pdflib.com.                         |
 |                                                                           |
 *---------------------------------------------------------------------------*/

/* $Id: pc_config.h,v 1.1 2005/10/11 17:18:13 vuduc2 Exp $
 *
 * PDFlib portability and configuration definitions
 *
 */

#ifndef PC_CONFIG_H
#define PC_CONFIG_H

/* ------------------------ feature configuration  ------------------- */

/* zlib compression support */
#define HAVE_LIBZ

/* ---------------------------- platform definitions ------------------------ */

/* #undef this if your platform doesn't support environment variables */
#define HAVE_ENVVARS

/* Compilers which are not strictly ANSI conforming can set PDF_VOLATILE
 * to an empty value.
 */
#ifndef PDF_VOLATILE
#define PDF_VOLATILE	volatile
#endif

/*
 * Byte order
 * WORDS_BIGENDIAN will be set by the configure script on most platforms.
 * Only on platforms where there is no configure script we must set the
 * endianness explicitly (most importantly CodeWarrior on the Mac)
 */
#undef PDC_ISBIGENDIAN
#if defined(WORDS_BIGENDIAN) || defined(__POWERPC__) || defined(__MC68K__)
#define PDC_ISBIGENDIAN 1
#if !defined(WORDS_BIGENDIAN)
#define WORDS_BIGENDIAN
#endif
#else
#define PDC_ISBIGENDIAN 0
#endif

/*
 * Define for compiler supporting file open function _wfopen
 * for Unicode filenames.
 */
#undef PDC_UNICODE_FILENAME

/* ---------------------------------- WIN32  -------------------------------- */

/* try to identify Windows compilers */

#if (defined _WIN32 || defined __WATCOMC__ || defined __BORLANDC__ ||	\
	(defined(__MWERKS__) && defined(__INTEL__))) && !defined WIN32
#define	WIN32
#endif	/* <Windows compiler>  && !defined WIN32 */

#ifdef	WIN32
#define WRITEMODE	"wb"
#define APPENDMODE	"ab"

#ifdef	_MSC_VER
#define _LARGEFILE_SOURCE
#endif

#undef PDC_PATHSEP
#define PDC_PATHSEP     "\\"

#if defined(_WIN32_WCE) && (_WIN32_WCE >= 300)
#define PDF_PLATFORM    "Windows CE"
#define WINCE
#undef HAVE_SETLOCALE
#undef HAVE_ENVVARS
#else
#if defined(WIN64)
#define PDF_PLATFORM    "Win64"
#else
#define PDF_PLATFORM    "Win32"
#endif
#endif

#define PDC_TMPDIR_ENV  "TMP"

/* file open function "_wfopen" for Unicode filenames is available.
**/
#if defined(_MSC_VER) && !defined(PDF_WIN98)
#define PDC_UNICODE_FILENAME
#endif

#endif	/* WIN32 */

/* some standard C library functions (eg. localtime()) are not reentrant
** and must be replaced with their "_r" equivalent (eg. localtime_r()).
*/
#if !defined(WIN32) && !defined(__MVS__) && \
 !(defined(__MWERKS__) && (defined(__POWERPC__) || defined(__MC68K__)))
#define PDC_NEEDS_R_FUNCTIONS
#endif

/* --------------------------------- Cygnus  -------------------------------- */

#ifdef __CYGWIN__
#define WRITEMODE	"wb"
#define APPENDMODE	"ab"
#ifdef DLL_EXPORT
    #define PDFLIB_EXPORTS
#endif

#endif /* __CYGWIN__ */

/* ---------------------------------- DJGPP  -------------------------------- */

#ifdef __DJGPP__
#define WRITEMODE	"wb"
#define APPENDMODE	"ab"
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
#define WRITEMODE	"wb"
#define APPENDMODE	"ab"
#define PDF_PLATFORM	"OS/2"
#endif	/* OS2 */

/* --------------------------------- Mac OS X ------------------------------- */

/* try to identify the Mac OS X command line compiler */

#if defined(__ppc__) && defined(__APPLE__)

#define MACOSX

/* Mac OS X 10.2 (Jaguar) defines this, but we use it for Mac OS 9 below */
#undef MAC

#endif /* Mac OS X */

/* --------------------------------- Mac OS 9 ------------------------------- */

/* try to identify Mac OS 9 compilers */

#if (defined macintosh || defined __POWERPC__ || defined __CFM68K__) && \
	!defined MAC && !defined MACOSX && !defined __BEOS__
#define MAC
#endif

#ifdef	MAC
#define WRITEMODE	"wb"
#define APPENDMODE	"ab"
#define PDC_PATHSEP     ":"

#undef HAVE_ENVVARS

#define PDF_PLATFORM	"Mac OS"
#endif	/* MAC */

/* ------------------ Carbon Handling for both Mac OS 9 and X --------------- */

#if defined(MAC) || defined(MACOSX)
/*
 * By default we always build a carbonized version of the library,
 * but allow non-Carbon builds to be triggered by setting the
 * PDF_TARGET_API_MAC_CLASSIC symbol externally.
 */

#ifdef PDF_TARGET_API_MAC_CLASSIC
#undef PDF_TYPE1_HOSTFONT_SUPPORTED
#else
#define PDF_TARGET_API_MAC_CARBON
#endif

#if defined(PDF_TARGET_API_MAC_CARBON) && !defined(TARGET_API_MAC_CARBON)
#define TARGET_API_MAC_CARBON 1
#endif

#endif  /* MAC */

/* ----------------------------------- BeOS --------------------------------- */

#ifdef __BEOS__
#define PDF_PLATFORM	"BeOS"
#endif /* __BEOS__ */

/* --------------------------------- AS/400 --------------------------------- */

/* try to identify the AS/400 compiler */

#if	defined __ILEC400__ && !defined AS400
#define	AS400
#endif

#ifdef AS400

#pragma comment(copyright, \
	"(C) PDFlib GmbH, Muenchen, Germany (www.pdflib.com)")

#if (_OS400_TGTVRM__>440)
# ifndef _LARGE_FILE_API
   #error You need to compile this module with DEFINE(_LARGE_FILE_API)
# endif
# ifndef __TERASPACE__
   #error You need to compile this module with TERASPACE(*YES *TSIFC)
STGMDL(*TERASPACE)
# endif
#endif

#define READTMODE       "rb"
#define WRITEMODE	"wb"
#define APPENDMODE	"ab"

#define PDF_PLATFORM	"iSeries"

#define WORDS_BIGENDIAN
#undef PDC_ISBIGENDIAN
#define PDC_ISBIGENDIAN 1

#endif	/* AS400 */

/* --------------------- S/390 with Unix System Services -------------------- */

#ifdef	OS390

#define WRITEMODE	"wb"
#define APPENDMODE	"ab"

#undef WORDS_BIGENDIAN
#define WORDS_BIGENDIAN
#undef PDC_ISBIGENDIAN
#define PDC_ISBIGENDIAN 1

#endif	/* OS390 */

/* -------------------------------- S/390 with MVS -------------------------- */

/* try to identify MVS (__MVS__ is #defined on USS and MVS!)
 * I370 is used by SAS C
 */

#if !defined(OS390) && (defined __MVS__ || defined I370) && !defined MVS
#define	MVS
#endif

#ifdef	MVS

#if defined(I370)
#define PDC_FILEQUOT    ""
#else
#define READBMODE       "rb,byteseek"
#define READBMODE_PLUS  "rb+,byteseek"
#define PDC_FILEQUOT    "'"
#endif
#define WRITEMODE       "wb"
#define WRITEMODE_V	"wb,recfm=v"
#define APPENDMODE	"ab"

#undef PDC_PATHSEP
#define PDC_PATHSEP     "("

#undef PDC_PATHTERM
#define PDC_PATHTERM    ")"

#define PDF_PLATFORM	"zSeries MVS"
#define PDF_OS390_MVS_RESOURCE

#define WORDS_BIGENDIAN
#undef PDC_ISBIGENDIAN
#define PDC_ISBIGENDIAN 1

#endif	/* MVS */

/* ------------------------------------ VMS --------------------------------- */

/* No special handling required */

#ifdef	VMS
/* Usually this will come from the build process */
#ifndef PDF_PLATFORM
#define PDF_PLATFORM	"VMS"
#define PDC_TMPDIR_ENV  "SYS$SCRATCH"
#define PDC_PATHSEP_LOG ":"
#endif
#endif	/* VMS */

/* --------------------------------- Defaults ------------------------------- */

#ifndef READTMODE
#define READTMODE       "r"
#endif  /* !READTMODE */

#ifndef READBMODE
#define READBMODE       "rb"
#endif  /* !READBMODE */

#ifndef READBMODE_PLUS
#define READBMODE_PLUS  "rb+"
#endif  /* !READBMODE_PLUS */

#ifndef WRITEMODE
#define WRITEMODE	"w"
#endif	/* !WRITEMODE */

#ifndef APPENDMODE
#define APPENDMODE	"a"
#endif	/* !APPENDMODE */

#ifndef PDC_PATHSEP
#define PDC_PATHSEP     "/"
#endif  /* !PDC_PATHSEP */

#ifndef PDC_TMPDIR_ENV
#define PDC_TMPDIR_ENV  "TMPDIR"
#endif  /* !PDC_TMPDIR_ENV */

#ifdef	_DEBUG
#define DEBUG
#endif	/* _DEBUG */

#ifdef	DEBUG
#define	PDC_DEBUG
#endif	/* DEBUG */

#define PDC_SCHAR_MIN   (-128)
#define PDC_SCHAR_MAX   127
#define PDC_UCHAR_MAX   255
#define PDC_SHRT_MIN    (-32768)
#define PDC_SHRT_MAX    32767
#define PDC_USHRT_MAX   65535
#define PDC_INT_MIN     (-PDC_INT_MAX - 1)
#define PDC_INT_MAX     2147483647
#define PDC_UINT_MAX    4294967295U

#define PDC_FLOAT_MAX   ((double) 1e+37)
#define PDC_FLOAT_MIN   ((double) -1e+37)
#define PDC_FLOAT_PREC  ((double) 1e-6)

#define PDC_OFFSET(type, field) ((unsigned int) &(((type *)NULL)->field))

#endif	/* PC_CONFIG_H */
