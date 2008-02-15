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

/* $Id: pc_file.h,v 1.2 2007/01/16 05:14:37 dquinlan Exp $
 *
 * Definitions for file routines
 *
 */

#ifndef PC_FILE_H
#define PC_FILE_H

#if (defined(MAC) || defined(MACOSX))

#include <Files.h>

#ifdef PDF_TARGET_API_MAC_CARBON

OSStatus FSMakePath(SInt16 volRefNum, SInt32 dirID, ConstStr255Param name,
           UInt8 *path, UInt32 maxPathSize);

#if 0
OSStatus FSPathMakeFSSpec(const UInt8 *path, FSSpec *spec);
#endif

#else

#include <Aliases.h>

OSErr FSpGetFullPath(const FSSpec *spec, short *fullPathLength,
        Handle *fullPath);

OSErr FSpLocationFromFullPath(short fullPathLength,
		const void *fullPath, FSSpec *spec);

#endif /* !PDF_TARGET_API_MAC_CARBON */
#endif /* (defined(MAC) || defined(MACOSX)) */

#define PDC_FILENAMELEN  1024    /* maximum file name length */

#define PDC_FILE_BINARY (1L<<0)  /* open as binary file,
                                    otherwise as text file */

#define PDC_OK_FREAD(file, buffer, len) \
    (pdc_fread(buffer, 1, len, file) == len)

typedef struct pdc_file_s pdc_file;

int		pdc__fseek(FILE *fp, pdc_off_t offset, int whence);
pdc_off_t	pdc__ftell(FILE *fp);
size_t		pdc__fread(void *ptr, size_t size, size_t nmemb, FILE *fp);
size_t		pdc__fwrite(void *ptr, size_t size, size_t nmemb, FILE *fp);
int		pdc__fgetc(FILE *fp);
int		pdc__feof(FILE *fp);

FILE   *pdc_get_fileptr(pdc_file *sfp);
int     pdc_get_fopen_errnum(pdc_core *pdc, int errnum);
void    pdc_set_fopen_errmsg(pdc_core *pdc, int errnum, const char *qualifier,
                            const char *filename);
void    *pdc_read_file(pdc_core *pdc, FILE *fp, pdc_off_t *o_filelen,
			    int incore);
int     pdc_read_textfile(pdc_core *pdc, pdc_file *sfp, char ***linelist);
char *	pdc_temppath(pdc_core *pdc, char *outbuf, const char *inbuf,
			    size_t inlen, const char *dirname);

FILE   *pdc_wfopen(pdc_core *pdc, const char *filename, const char *mode);

pdc_file *	pdc_fopen(pdc_core *pdc, const char *filename,
		    const char *qualifier, const pdc_byte *data,
		    size_t size, int flags);
pdc_core *	pdc_file_getpdc(pdc_file *sfp);
char   *	pdc_file_name(pdc_file *sfp);
pdc_off_t	pdc_file_size(pdc_file *sfp);
pdc_bool	pdc_file_isvirtual(pdc_file *sfp);
char   *	pdc_fgetline(char *s, int size, pdc_file *sfp);
pdc_off_t	pdc_ftell(pdc_file *sfp);
int		pdc_fseek(pdc_file *sfp, pdc_off_t offset, int whence);
size_t		pdc_fread(void *ptr, size_t size, size_t nmemb, pdc_file *sfp);
const void *	pdc_freadall(pdc_file *sfp, size_t *filelen,
		    pdc_bool *ismem);

int     pdc_ungetc(int c, pdc_file *sfp);
int     pdc_fgetc(pdc_file *sfp);
#if 0
char   *pdc_fgets(char *s, int size, pdc_file *sfp);
#endif
int     pdc_feof(pdc_file *sfp);
void    pdc_fclose(pdc_file *sfp);
void    pdc_file_fullname(const char *dirname, const char *basename,
                          char *fullname);
char   *pdc_file_fullname_mem(pdc_core *pdc, const char *dirname,
                              const char *basename);



#endif  /* PC_FILE_H */
