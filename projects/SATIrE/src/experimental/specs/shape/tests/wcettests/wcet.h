/***************************************************
 *                                                 *
 *  Header file for the use of wcetC/wcetCc, a     *
 *  language to enable static WCET analyis         *
 *  on standard ANSI C.                            *
 *  (wcetCc is like wcetC, but with the prefix     *
 *   'wcet_' before each wcet key word.            *
 *                                                 *
 *  (c) 2001 Raimund Kirner, TU-Wien,              *
 *           raimund@vmars.tuwien.ac.at            *
 *                                                 *
 ***************************************************/

/* Constants for LANG_WCET to select the right     *
 * keywords.                                       */

#define OTHER   0x00  /* no annotations */
#define WCETC   0x01  /* wcetC          */
#define WCETCC  0x02  /* wcetCc         */

/* If LANG_WCET is not defined, the default        *
 * language is allways WCETC                       */

#ifndef LANG_WCET
#define LANG_WCET WCETC
#endif

/* Definitions of wrappers for the wcetC grammar   */

#if defined(LANG_WCET) && ((LANG_WCET)==WCETC || (LANG_WCET)==WCETCC)
#if (LANG_WCET) == WCETC  /* WCETC */
#define WCET_LOOP_BOUND(x)    maximum (x) iterations
#define WCET_SCOPE(x)         scope x 
#define WCET_MARKER(x)        marker x
#define WCET_RESTRICTION(x)   restriction x
#define WCET_ADD_CYCLES(x)    addcycles(x)
#else   /* WCETCC */
#define WCET_LOOP_BOUND(x)    wcet_maximum (x) wcet_iterations
#define WCET_SCOPE(x)         wcet_scope x 
#define WCET_MARKER(x)        wcet_marker x
#define WCET_RESTRICTION(x)   wcet_restriction x
#define WCET_ADD_CYCLES(x)    wcet_addcycles(x)
#endif  
#define WCET_BLOCK_BEGIN(x,y) wcet_blockbegin(x,y)
#define WCET_BLOCK_END(x)     wcet_blockend(x)
#define WCET_BUILD_INFO(x)    wcet_buildinfo(x)
#else   /* ANSI C */
#define WCET_LOOP_BOUND(x)
#define WCET_SCOPE(x)
#define WCET_MARKER(x)
#define WCET_RESTRICTION(x)
#define WCET_ADD_CYCLES(x)
#define WCET_BLOCK_BEGIN(x,y) 
#define WCET_BLOCK_END(x)
#define WCET_BUILD_INFO(x)
#endif


