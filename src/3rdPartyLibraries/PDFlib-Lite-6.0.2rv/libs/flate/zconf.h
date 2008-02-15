/* zconf.h -- configuration of the zlib compression library
 * Copyright (C) 1995-2002 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h 
 */


/* $Id: zconf.h,v 1.1 2005/10/11 17:18:11 vuduc2 Exp $ */

/* @(#) $Id: zconf.h,v 1.1 2005/10/11 17:18:11 vuduc2 Exp $ */

#ifndef _ZCONF_H
#define _ZCONF_H

#include "pc_config.h"

/*
 * If you *really* need a unique prefix for all types and library functions,
 * compile with -DZ_PREFIX. The "standard" zlib should be compiled without it.
 */
/* PDFlib GmbH: We use "pdf_z_".
** The original list was incomplete, by the way.
*/
#define Z_PREFIX
#ifdef	Z_PREFIX

#ifdef PDFLIB_PLOP_BUILD
/* redefine names of all functions for integrating into
   * psp library, to avoid name clashes if used together with
   * pdflib */
#define PREFIX(x)	plop_z_##x
#  define adler32			plop_z_adler32
#  define compress			plop_z_compress
#  define compress2			plop_z_compress2
#  define crc32				plop_z_crc32
#  define get_crc_table			plop_z_get_crc_table
#  define deflate			plop_z_deflate
#  define deflateCopy			plop_z_deflateCopy
#  define deflateEnd			plop_z_deflateEnd
#  define deflateInit2_			plop_z_deflateInit2_
#  define deflateInit_			plop_z_deflateInit_
#  define deflateParams			plop_z_deflateParams
#  define deflateReset			plop_z_deflateReset
#  define deflateSetDictionary		plop_z_deflateSetDictionary
#  define deflate_copyright		plop_z_deflate_copyright
#  define inflate_blocks		plop_z_inflate_blocks
#  define inflate_blocks_free		plop_z_inflate_blocks_free
#  define inflate_blocks_new		plop_z_inflate_blocks_new
#  define inflate_blocks_reset		plop_z_inflate_blocks_reset
#  define inflate_blocks_sync_point	plop_z_inflate_blocks_sync_point
#  define inflate_set_dictionary	plop_z_inflate_set_dictionary
#  define inflate_codes			plop_z_inflate_codes
#  define inflate_codes_free		plop_z_inflate_codes_free
#  define inflate_codes_new		plop_z_inflate_codes_new
#  define inflate_fast			plop_z_inflate_fast
#  define inflate			plop_z_inflate
#  define inflateEnd			plop_z_inflateEnd
#  define inflateInit2_			plop_z_inflateInit2_
#  define inflateInit_			plop_z_inflateInit_
#  define inflateReset			plop_z_inflateReset
#  define inflateSetDictionary		plop_z_inflateSetDictionary
#  define inflateSync			plop_z_inflateSync
#  define inflateSyncPoint		plop_z_inflateSyncPoint
#  define inflate_copyright		plop_z_inflate_copyright
#  define inflate_trees_bits		plop_z_inflate_trees_bits
#  define inflate_trees_dynamic		plop_z_inflate_trees_dynamic
#  define inflate_trees_fixed		plop_z_inflate_trees_fixed
#  define inflate_flush			plop_z_inflate_flush
#  define inflate_mask			plop_z_inflate_mask
#  define _dist_code			plop_z__dist_code
#  define _length_code			plop_z__length_code
#  define _tr_align			plop_z__tr_align
#  define _tr_flush_block		plop_z__tr_flush_block
#  define _tr_init			plop_z__tr_init
#  define _tr_stored_block		plop_z__tr_stored_block
#  define _tr_tally			plop_z__tr_tally
#  define uncompress			plop_z_uncompress
#  define zError			plop_z_zError
#  define z_errmsg			plop_z_z_errmsg
#  define z_error			plop_z_z_error
#  define z_verbose			plop_z_z_verbose
#  define zcalloc			plop_z_zcalloc
#  define zcfree			plop_z_zcfree
#  define zlibVersion			plop_z_zlibVersion

#else
#define PREFIX(x)	pdf_z_##x
#  define adler32			pdf_z_adler32
#  define compress			pdf_z_compress
#  define compress2			pdf_z_compress2
#  define crc32				pdf_z_crc32
#  define get_crc_table			pdf_z_get_crc_table
#  define deflate			pdf_z_deflate
#  define deflateCopy			pdf_z_deflateCopy
#  define deflateEnd			pdf_z_deflateEnd
#  define deflateInit2_			pdf_z_deflateInit2_
#  define deflateInit_			pdf_z_deflateInit_
#  define deflateParams			pdf_z_deflateParams
#  define deflateReset			pdf_z_deflateReset
#  define deflateSetDictionary		pdf_z_deflateSetDictionary
#  define deflate_copyright		pdf_z_deflate_copyright
#  define inflate_blocks		pdf_z_inflate_blocks
#  define inflate_blocks_free		pdf_z_inflate_blocks_free
#  define inflate_blocks_new		pdf_z_inflate_blocks_new
#  define inflate_blocks_reset		pdf_z_inflate_blocks_reset
#  define inflate_blocks_sync_point	pdf_z_inflate_blocks_sync_point
#  define inflate_set_dictionary	pdf_z_inflate_set_dictionary
#  define inflate_codes			pdf_z_inflate_codes
#  define inflate_codes_free		pdf_z_inflate_codes_free
#  define inflate_codes_new		pdf_z_inflate_codes_new
#  define inflate_fast			pdf_z_inflate_fast
#  define inflate			pdf_z_inflate
#  define inflateEnd			pdf_z_inflateEnd
#  define inflateInit2_			pdf_z_inflateInit2_
#  define inflateInit_			pdf_z_inflateInit_
#  define inflateReset			pdf_z_inflateReset
#  define inflateSetDictionary		pdf_z_inflateSetDictionary
#  define inflateSync			pdf_z_inflateSync
#  define inflateSyncPoint		pdf_z_inflateSyncPoint
#  define inflate_copyright		pdf_z_inflate_copyright
#  define inflate_trees_bits		pdf_z_inflate_trees_bits
#  define inflate_trees_dynamic		pdf_z_inflate_trees_dynamic
#  define inflate_trees_fixed		pdf_z_inflate_trees_fixed
#  define inflate_flush			pdf_z_inflate_flush
#  define inflate_mask			pdf_z_inflate_mask
#  define _dist_code			pdf_z__dist_code
#  define _length_code			pdf_z__length_code
#  define _tr_align			pdf_z__tr_align
#  define _tr_flush_block		pdf_z__tr_flush_block
#  define _tr_init			pdf_z__tr_init
#  define _tr_stored_block		pdf_z__tr_stored_block
#  define _tr_tally			pdf_z__tr_tally
#  define uncompress			pdf_z_uncompress
#  define zError			pdf_z_zError
#  define z_errmsg			pdf_z_z_errmsg
#  define z_error			pdf_z_z_error
#  define z_verbose			pdf_z_z_verbose
#  define zcalloc			pdf_z_zcalloc
#  define zcfree			pdf_z_zcfree
#  define zlibVersion			pdf_z_zlibVersion

#endif /* PDFLIB_PLOP_BUILD */

#undef PREFIX

/* special handling required on the Mac where Byte is alread defined */
#if !(defined(MAC) || defined(MACOSX))
#  define Byte		z_Byte
#endif

#  define uInt		z_uInt
#  define uLong		z_uLong
#  define Bytef		z_Bytef
#  define charf		z_charf
#  define intf		z_intf
#  define uIntf		z_uIntf
#  define uLongf	z_uLongf
#  define voidpf	z_voidpf
#  define voidp		z_voidp
#endif

#if (defined(_WIN32) || defined(__WIN32__)) && !defined(WIN32)
#  define WIN32
#endif
#if defined(__GNUC__) || defined(WIN32) || defined(__386__) || defined(i386)
#  ifndef __32BIT__
#    define __32BIT__
#  endif
#endif
#if defined(__MSDOS__) && !defined(MSDOS)
#  define MSDOS
#endif

/* PDFlib GmbH: Windows CE portability */
#ifdef _WIN32_WCE 
#define NO_ERRNO_H
#endif

/*
 * Compile with -DMAXSEG_64K if the alloc function cannot allocate more
 * than 64k bytes at a time (needed on systems with 16-bit int).
 */
#if defined(MSDOS) && !defined(__32BIT__)
#  define MAXSEG_64K
#endif
#ifdef MSDOS
#  define UNALIGNED_OK
#endif

#ifndef STDC	/* PDFlib GmbH: we require ANSI C anyway */
#define STDC
#endif

/* Some Mac compilers merge all .h files incorrectly: */
#if defined(__MWERKS__) || defined(applec) ||defined(THINK_C) ||defined(__SC__)
#  define NO_DUMMY_DECL
#endif

/* Old Borland C incorrectly complains about missing returns: */
#if defined(__BORLANDC__) && (__BORLANDC__ < 0x500)
#  define NEED_DUMMY_RETURN
#endif


/* Maximum value for memLevel in deflateInit2 */
#ifndef MAX_MEM_LEVEL
#  ifdef MAXSEG_64K
#    define MAX_MEM_LEVEL 8
#  else
#    define MAX_MEM_LEVEL 9
#  endif
#endif

/* Maximum value for windowBits in deflateInit2 and inflateInit2.
 * WARNING: reducing MAX_WBITS makes minigzip unable to extract .gz files
 * created by gzip. (Files created by minigzip can still be extracted by
 * gzip.)
 */
#ifndef MAX_WBITS
#  define MAX_WBITS   15 /* 32K LZ77 window */
#endif

/* The memory requirements for deflate are (in bytes):
            (1 << (windowBits+2)) +  (1 << (memLevel+9))
 that is: 128K for windowBits=15  +  128K for memLevel = 8  (default values)
 plus a few kilobytes for small objects. For example, if you want to reduce
 the default memory requirements from 256K to 128K, compile with
     make CFLAGS="-O -DMAX_WBITS=14 -DMAX_MEM_LEVEL=7"
 Of course this will generally degrade compression (there's no free lunch).

   The memory requirements for inflate are (in bytes) 1 << windowBits
 that is, 32K for windowBits=15 (default value) plus a few kilobytes
 for small objects.
*/

                        /* Type declarations */

#ifndef OF /* function prototypes */
#  ifdef STDC
#    define OF(args)  args
#  else
#    define OF(args)  ()
#  endif
#endif

/* The following definitions for FAR are needed only for MSDOS mixed
 * model programming (small or medium model with some far allocations).
 * This was tested only with MSC; for other MSDOS compilers you may have
 * to define NO_MEMCPY in zutil.h.  If you don't need the mixed model,
 * just define FAR to be empty.
 */
#if (defined(M_I86SM) || defined(M_I86MM)) && !defined(__32BIT__)
   /* MSC small or medium model */
#  define SMALL_MEDIUM
#  ifdef _MSC_VER
#    define FAR _far
#  else
#    define FAR far
#  endif
#endif
#if defined(__BORLANDC__) && (defined(__SMALL__) || defined(__MEDIUM__))
#  ifndef __32BIT__
#    define SMALL_MEDIUM
#    define FAR _far
#  endif
#endif

/* Compile with -DZLIB_DLL for Windows DLL support */
#if defined(ZLIB_DLL)
#  if defined(_WINDOWS) || defined(WINDOWS)
#    ifdef FAR
#      undef FAR
#    endif
#    include <windows.h>
#    define ZEXPORT  WINAPI
#    ifdef WIN32
#      define ZEXPORTVA  WINAPIV
#    else
#      define ZEXPORTVA  FAR _cdecl _export
#    endif
#  endif
#  if defined (__BORLANDC__)
#    if (__BORLANDC__ >= 0x0500) && defined (WIN32)
#      include <windows.h>
#      define ZEXPORT __declspec(dllexport) WINAPI
#      define ZEXPORTRVA __declspec(dllexport) WINAPIV
#    else
#      if defined (_Windows) && defined (__DLL__)
#        define ZEXPORT _export
#        define ZEXPORTVA _export
#      endif
#    endif
#  endif
#endif

#if defined (__BEOS__)
#  if defined (ZLIB_DLL)
#    define ZEXTERN extern __declspec(dllexport)
#  else
#    define ZEXTERN extern __declspec(dllimport)
#  endif
#endif

#ifndef ZEXPORT
#  define ZEXPORT
#endif
#ifndef ZEXPORTVA
#  define ZEXPORTVA
#endif
#ifndef ZEXTERN
#  define ZEXTERN extern
#endif

#ifndef FAR
#   define FAR
#endif

/* PDFlib GmbH: also do the typedef on the Mac */
#if defined(MAC) || defined(MACOSX)
#include <MacTypes.h>
#else
typedef unsigned char  Byte;  /* 8 bits */
#endif

typedef unsigned int   uInt;  /* 16 bits or more */
typedef unsigned long  uLong; /* 32 bits or more */

#ifdef SMALL_MEDIUM
   /* Borland C/C++ and some old MSC versions ignore FAR inside typedef */
#  define Bytef Byte FAR
#else
   typedef Byte  FAR Bytef;
#endif
typedef char  FAR charf;
typedef int   FAR intf;
typedef uInt  FAR uIntf;
typedef uLong FAR uLongf;

#ifdef STDC
   typedef void FAR *voidpf;
   typedef void     *voidp;
#else
   typedef Byte FAR *voidpf;
   typedef Byte     *voidp;
#endif

/* PDFlib GmbH: Windows portability */
#if !defined(WIN32) && !defined(OS2) && !defined(MAC)
#  include <sys/types.h> /* for off_t */
#  include <unistd.h>    /* for SEEK_* and off_t */
#  define z_off_t  off_t
#endif

#ifndef SEEK_SET
#  define SEEK_SET        0       /* Seek from beginning of file.  */
#  define SEEK_CUR        1       /* Seek from current position.  */
#  define SEEK_END        2       /* Set file pointer to EOF plus "offset" */
#endif
#ifndef z_off_t
#  define  z_off_t long
#endif

/* MVS linker does not support external names larger than 8 bytes */
#if defined(__MVS__)
#   pragma map(deflateInit_,"DEIN")
#   pragma map(deflateInit2_,"DEIN2")
#   pragma map(deflateEnd,"DEEND")
#   pragma map(inflateInit_,"ININ")
#   pragma map(inflateInit2_,"ININ2")
#   pragma map(inflateEnd,"INEND")
#   pragma map(inflateSync,"INSY")
#   pragma map(inflateSetDictionary,"INSEDI")
#   pragma map(inflate_blocks,"INBL")
#   pragma map(inflate_blocks_new,"INBLNE")
#   pragma map(inflate_blocks_free,"INBLFR")
#   pragma map(inflate_blocks_reset,"INBLRE")
#   pragma map(inflate_codes_free,"INCOFR")
#   pragma map(inflate_codes,"INCO")
#   pragma map(inflate_fast,"INFA")
#   pragma map(inflate_flush,"INFLU")
#   pragma map(inflate_mask,"INMA")
#   pragma map(inflate_set_dictionary,"INSEDI2")
#   pragma map(inflate_copyright,"INCOPY")
#   pragma map(inflate_trees_bits,"INTRBI")
#   pragma map(inflate_trees_dynamic,"INTRDY")
#   pragma map(inflate_trees_fixed,"INTRFI")
#   pragma map(inflate_trees_free,"INTRFR")
#endif

#endif /* _ZCONF_H */
