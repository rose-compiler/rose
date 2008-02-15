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

/* $Id: pc_file.c,v 1.1 2005/10/11 17:18:13 vuduc2 Exp $
 *
 * Various file routines
 *
 */

#include <errno.h>

#include "pc_util.h"
#include "pc_md5.h"
#include "pc_file.h"


/* headers for getpid() or _getpid().
*/
#if defined(WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>
#include <io.h>
#else
#if defined(MAC)
#include <MacErrors.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif
#endif

#ifndef WINCE
#include <time.h>
#else
#include <winbase.h>
#endif

/* platform independent wrapper functions for 64-bit file handling.
*/
int
pdc__fseek(FILE *fp, pdc_off_t offset, int whence)
{
#if defined(_LARGEFILE_SOURCE)
    #if defined(WIN32)
	switch (whence)
	{
	    case SEEK_SET:
		return fsetpos(fp, &offset);

	    case SEEK_CUR:
	    {
		pdc_off_t pos;

		fgetpos(fp, &pos);
		pos += offset;
		return fsetpos(fp, &pos);
	    }

	    case SEEK_END:
	    {
		pdc_off_t pos, len;

		pos = _telli64(fileno(fp));
		_lseeki64(fileno(fp), 0, SEEK_END);
		len = _telli64(fileno(fp));
		_lseeki64(fileno(fp), pos, SEEK_SET);

		len += offset;
		return fsetpos(fp, &len);
	    }

	    default:
		return -1;
	}
    #else
	return fseeko(fp, offset, whence);
    #endif
#else
    return fseek(fp, offset, whence);
#endif
}

pdc_off_t
pdc__ftell(FILE *fp)
{
#if defined(_LARGEFILE_SOURCE)
    #if defined(WIN32)
	pdc_off_t pos;

	fgetpos(fp, &pos);
	return pos;
    #else
	return ftello(fp);
    #endif
#else
    return ftell(fp);
#endif
}

size_t
pdc__fread(void *ptr, size_t size, size_t nmemb, FILE *fp)
{
    return fread(ptr, size, nmemb, fp);
}

size_t
pdc__fwrite(void *ptr, size_t size, size_t nmemb, FILE *fp)
{
    return fwrite(ptr, size, nmemb, fp);
}

int
pdc__fgetc(FILE *fp)
{
    return fgetc(fp);
}

int
pdc__feof(FILE *fp)
{
    return feof(fp);
}

struct pdc_file_s
{
    pdc_core       *pdc;       /* pdcore struct */
    char           *filename;  /* file name */
    FILE           *fp;        /* file struct or NULL. Then data != NULL: */
    const pdc_byte *data;      /* file data or NULL. Then fp != NULL */
    const pdc_byte *end;       /* first byte above data buffer */
    const pdc_byte *pos;       /* current file position in data buffer */
};

FILE   *
pdc_get_fileptr(pdc_file *sfp)
{
    return sfp->fp;
}

#if defined(_MSC_VER) && defined(_MANAGED)
#pragma unmanaged
#endif
int
pdc_get_fopen_errnum(pdc_core *pdc, int errnum)
{
    int outnum = errnum, isread;

    (void) pdc;

    isread = (errnum == PDC_E_IO_RDOPEN);

#if defined(MVS)

    switch (errno)
    {
        case 49:
        outnum = isread ? PDC_E_IO_RDOPEN_NF : PDC_E_IO_WROPEN_NF;
    }
    return outnum;

#elif defined(WIN32)
    {
	DWORD lasterror = GetLastError();
        switch (lasterror)
        {
            case ERROR_FILE_NOT_FOUND:
            outnum = isread ? PDC_E_IO_RDOPEN_NF : PDC_E_IO_WROPEN_NF;
            break;

            case ERROR_ACCESS_DENIED:
            case ERROR_INVALID_PASSWORD:
            case ERROR_NETWORK_ACCESS_DENIED:
            outnum = isread ? PDC_E_IO_RDOPEN_PD : PDC_E_IO_WROPEN_PD;
            break;

            case ERROR_INVALID_NAME:
            outnum = isread ? PDC_E_IO_RDOPEN_NF : PDC_E_IO_WROPEN_IS;
            break;

            case ERROR_PATH_NOT_FOUND:
            case ERROR_INVALID_DRIVE:
            case ERROR_BAD_NETPATH:
            case ERROR_BAD_UNIT:
            outnum = isread ? PDC_E_IO_RDOPEN_NF : PDC_E_IO_WROPEN_NP;
            break;

            case ERROR_TOO_MANY_OPEN_FILES:
            case ERROR_SHARING_BUFFER_EXCEEDED:
            outnum = isread ? PDC_E_IO_RDOPEN_TM : PDC_E_IO_WROPEN_TM;
            break;

            case ERROR_FILE_EXISTS:
            outnum = PDC_E_IO_WROPEN_AE;
            break;

            case ERROR_BUFFER_OVERFLOW:
            outnum = PDC_E_IO_WROPEN_TL;
            break;

            case ERROR_WRITE_FAULT:
            case ERROR_CANNOT_MAKE:
            outnum = PDC_E_IO_WROPEN_NC;
            break;

            case ERROR_HANDLE_DISK_FULL:
            case ERROR_DISK_FULL:
            outnum = PDC_E_IO_WROPEN_NS;
            break;

            case ERROR_SHARING_VIOLATION:
            outnum = isread ? PDC_E_IO_RDOPEN_SV : PDC_E_IO_WROPEN_SV;
            break;
        }

        if (lasterror)
        {
            errno = (int) lasterror;
            return outnum;
        }

        /* if lasterror == 0 we must look for errno (see .NET) */
    }

#endif /* WIN32 */

    switch (errno)
    {
#ifdef EACCES
        case EACCES:
        outnum = isread ? PDC_E_IO_RDOPEN_PD : PDC_E_IO_WROPEN_PD;
        break;
#endif
#ifdef EMACOSERR
        case EMACOSERR:
#if defined(MAC)

        switch (__MacOSErrNo)
        {
            case fnfErr:
            case dirNFErr:
            case resFNotFound:
            case afpDirNotFound:
            outnum = isread ? PDC_E_IO_RDOPEN_NF : PDC_E_IO_WROPEN_NF;
            break;

            case permErr:
            case wrPermErr:
            case wPrErr:
            case afpAccessDenied:
            case afpVolLocked:
            outnum = isread ? PDC_E_IO_RDOPEN_PD : PDC_E_IO_WROPEN_PD;
            break;

            case nsvErr:
            case afpObjectTypeErr:
            outnum = isread ? PDC_E_IO_RDOPEN_NF : PDC_E_IO_WROPEN_IS;
            break;

            case tmfoErr:
            case afpTooManyFilesOpen:
            outnum = isread ? PDC_E_IO_RDOPEN_TM : PDC_E_IO_WROPEN_TM;
            break;

            case opWrErr:
            outnum = PDC_E_IO_WROPEN_AE;
            break;

            case dirFulErr:
            case dskFulErr:
            case afpDiskFull:
            outnum = PDC_E_IO_WROPEN_NS;
            break;

            case fLckdErr:
            case afpLockErr:
            outnum = isread ? PDC_E_IO_RDOPEN_SV : PDC_E_IO_WROPEN_SV;
            break;

            default:
            break;
        }

        if (__MacOSErrNo)
        {
            return outnum;
        }
#endif
        break;
#endif
#ifdef ENOENT
        case ENOENT:
        outnum = isread ? PDC_E_IO_RDOPEN_NF : PDC_E_IO_WROPEN_NF;
        break;
#endif
#ifdef EMFILE
        case EMFILE:
        outnum = isread ? PDC_E_IO_RDOPEN_TM : PDC_E_IO_WROPEN_TM;
        break;
#endif
#ifdef ENFILE
        case ENFILE:
        outnum = isread ? PDC_E_IO_RDOPEN_TM : PDC_E_IO_WROPEN_TM;
        break;
#endif
#ifdef EISDIR
        case EISDIR:
        outnum = isread ? PDC_E_IO_RDOPEN_ID : PDC_E_IO_WROPEN_ID;
        break;
#endif
#ifdef EEXIST
        case EEXIST:
        outnum = PDC_E_IO_WROPEN_AE;
        break;
#endif
#ifdef ENAMETOOLONG
        case ENAMETOOLONG:
        outnum = PDC_E_IO_WROPEN_TL;
        break;
#endif
#ifdef ENOSPC
        case ENOSPC:
        outnum = PDC_E_IO_WROPEN_NS;
        break;
#endif
        default:
        /* observed on Solaris */
        if (errno == 0)
        {
            outnum = isread ? PDC_E_IO_RDOPEN_TM : PDC_E_IO_WROPEN_TM;
            break;
        }
        outnum = errnum;
        break;
    }

    return outnum;
}
#if defined(_MSC_VER) && defined(_MANAGED)
#pragma managed
#endif

void
pdc_set_fopen_errmsg(pdc_core *pdc, int errnum, const char *qualifier,
                     const char *filename)
{
    const char *stemp = NULL;

    errnum = pdc_get_fopen_errnum(pdc, errnum);
    if (errnum == PDC_E_IO_RDOPEN)
        errnum = PDC_E_IO_RDOPEN_CODE;
    else if (errnum == PDC_E_IO_WROPEN)
        errnum = PDC_E_IO_WROPEN_CODE;
    if (errnum == PDC_E_IO_RDOPEN_CODE || errnum == PDC_E_IO_WROPEN_CODE)
    {
#if defined(EMACOSERR) && defined(MAC)
        errno = (int) __MacOSErrNo;
#endif
        stemp = pdc_errprintf(pdc, "%d", errno);
    }

    pdc_set_errmsg(pdc, errnum, qualifier, filename, stemp, 0);
}

void *
pdc_read_file(pdc_core *pdc, FILE *fp, pdc_off_t *o_filelen, int incore)
{
    static const char fn[] = "pdc_read_file";
    pdc_off_t filelen = 0, len = 0;
    char *content = NULL;


#if !defined(MVS) || !defined(I370)

    pdc__fseek(fp, 0, SEEK_END);
    filelen = pdc__ftell(fp);
    pdc__fseek(fp, 0, SEEK_SET);

    if (incore && filelen)
    {
        content = (char *) pdc_malloc(pdc, (size_t) (filelen + 1), fn);
        len = (pdc_off_t) pdc__fread(content, 1, (size_t) filelen, fp);
        if (len != filelen)
        {
            pdc_free(pdc, content);
            filelen = 0;
            content = NULL;
        }
    }

#endif

    if (content) content[filelen] = 0;
    *o_filelen = filelen;
    return (void *) content;
}

FILE *
pdc_wfopen(pdc_core *pdc, const char *filename, const char *mode)
{
    FILE *fp = NULL;

#if defined(PDC_UNICODE_FILENAME)

    pdc_byte *outfilename = NULL;
    pdc_text_format nameformat = pdc_utf8;
    pdc_text_format targetnameformat = pdc_utf16;
    int len = (int) strlen(filename);
    int outlen = 0;

    /* convert filename from UTF-8 to UTF-16 or Latin-1 */
    pdc_convert_string(pdc, nameformat, 0, NULL, (pdc_byte *) filename, len,
                       &targetnameformat, NULL, &outfilename, &outlen,
                       PDC_CONV_TRYBYTES, pdc_true);

    if (targetnameformat == pdc_bytes)
    {
        fp = fopen((const char *) outfilename, mode);
    }
    else
    {
        wchar_t wmode[8];
        int i;

        len = (int) strlen(mode);
        for (i = 0; i < len; i++)
            wmode[i] = (wchar_t) mode[i];
        wmode[len] = 0;

        fp = _wfopen((wchar_t *) outfilename, wmode);
    }

    pdc_free(pdc, outfilename);

#else
    (void) pdc;

    fp = fopen(filename, mode);
#endif

    return fp;
}

pdc_file *
pdc_fopen(pdc_core *pdc, const char *filename, const char *qualifier,
          const pdc_byte *data, size_t size, int flags)
{
    static const char fn[] = "pdc_fopen";
    pdc_file *sfile;

    sfile = (pdc_file *) pdc_calloc(pdc, sizeof(pdc_file), fn);

    if (data)
    {
        sfile->data = data;
        sfile->pos = sfile->data;
        sfile->end = sfile->data + size;
    }
    else
    {
        sfile->fp = pdc_wfopen(pdc, filename,
                        (flags & PDC_FILE_BINARY) ? READBMODE : READTMODE);
        if (sfile->fp == NULL)
        {
            pdc_free(pdc, sfile);
            if (qualifier)
            {
                pdc_set_fopen_errmsg(pdc, PDC_E_IO_RDOPEN, qualifier, filename);
            }
            return NULL;
        }
    }

    sfile->pdc = pdc;
    sfile->filename = pdc_strdup(pdc, filename);


    return sfile;
}


pdc_bool
pdc_file_isvirtual(pdc_file *sfp)
{
    return sfp->fp ? pdc_false : pdc_true;
}

char *
pdc_file_name(pdc_file *sfp)
{
    return sfp->filename;
}

pdc_core *
pdc_file_getpdc(pdc_file *sfp)
{
    return sfp->pdc;
}

pdc_off_t
pdc_file_size(pdc_file *sfp)
{
    pdc_off_t filelen;

    if (sfp->fp)
    {
        pdc_off_t pos = pdc__ftell(sfp->fp);

        pdc_read_file(sfp->pdc, sfp->fp, &filelen, 0);
        pdc__fseek(sfp->fp, pos, SEEK_SET);
    }
    else
        filelen = (pdc_off_t) (sfp->end - sfp->data);

    return filelen;
}

const void *
pdc_freadall(pdc_file *sfp, size_t *filelen, pdc_bool *ismem)
{
    pdc_off_t flen;	/* TODO2GB: >2GB on 32-bit platforms? */

    if (sfp->fp)
    {
        const void *result = pdc_read_file(sfp->pdc, sfp->fp, &flen, 1);

        if (ismem) *ismem = pdc_false;
	*filelen = (size_t) flen;
	return result;
    }

    if (ismem) *ismem = pdc_true;
    *filelen = (size_t) (sfp->end - sfp->data);
    return sfp->data;
}

static int
pdc_fgetc_e(pdc_file *sfp)
{
    int c = pdc_fgetc(sfp);
    return c;
}

char *
pdc_fgetline(char *s, int size, pdc_file *sfp)
{
    int i, c;

    c = pdc_fgetc_e(sfp);
    if (c == EOF)
        return NULL;

    size--;
    for (i = 0; i < size; i++)
    {
        if (c == '\n' || c == '\r' || c == EOF) break;
        s[i] = (char) c;
        c = pdc_fgetc_e(sfp);
    }
    s[i] = 0;

    /* Skip windows line end \r\n */
    if (c == '\r')
    {
        c = pdc_fgetc_e(sfp);

        if (c != '\n' && c != EOF)
	{
	    if (sfp->fp)
		ungetc(c, sfp->fp);
	    else
		pdc_fseek(sfp, -1, SEEK_CUR);
	}
    }
    return s;
}

/*
 * Emulation of C file functions - relevant for PDFlib
 */

pdc_off_t
pdc_ftell(pdc_file *sfp)
{
    if (sfp->fp)
        return pdc__ftell(sfp->fp);

    return (pdc_off_t) (sfp->pos - sfp->data);
}

int
pdc_fseek(pdc_file *sfp, pdc_off_t offset, int whence)
{
    if (sfp->fp)
        return pdc__fseek(sfp->fp, offset, whence);

    switch (whence)
    {
        case SEEK_SET:
        if (sfp->data + offset > sfp->end)
            return -1;
        sfp->pos = sfp->data + offset;
        break;

        case SEEK_CUR:
        if (sfp->pos + offset > sfp->end)
            return -1;
        sfp->pos += offset;
        break;

        case SEEK_END:
        if (sfp->end + offset > sfp->end)
            return -1;
        sfp->pos = sfp->end + offset;
        break;
    }
    return 0;
}

size_t
pdc_fread(void *ptr, size_t size, size_t nmemb, pdc_file *sfp)
{
    size_t nbytes = 0;

    if (sfp->fp)
        return pdc__fread(ptr, size, nmemb, sfp->fp);

    nbytes = size * nmemb;
    if (sfp->pos + nbytes > sfp->end)
    {
        nbytes = (size_t) (sfp->end - sfp->pos);
        nmemb = nbytes / size;
        nbytes = nmemb *size;
    }
    memcpy(ptr, sfp->pos, nbytes);
    sfp->pos += nbytes;

    return nmemb;
}

int
pdc_fgetc(pdc_file *sfp)
{
    int ch = 0;

    if (sfp->fp)
        return pdc__fgetc(sfp->fp);

    if (sfp->pos < sfp->end)
    {
	ch = (int) *sfp->pos;
	sfp->pos++;
    }
    else
    {
	ch = EOF;
    }

    return ch;
}

int
pdc_feof(pdc_file *sfp)
{
    if (sfp->fp)
        return pdc__feof(sfp->fp);

    return (sfp->pos >= sfp->end) ? 1 : 0;
}

void
pdc_fclose(pdc_file *sfp)
{
    if (sfp)
    {
        if (sfp->fp)
        {
            fclose(sfp->fp);
            sfp->fp = NULL;
        }
        if (sfp->filename)
        {
            pdc_free(sfp->pdc, sfp->filename);
            sfp->filename = NULL;
        }
        pdc_free(sfp->pdc, sfp);
    }
}

/*
 * Concatenating a directory name with a file base name to a full valid
 * file name. On MVS platforms an extension at the end of basename
 * will be discarded.
 */
void
pdc_file_fullname(const char *dirname, const char *basename, char *fullname)
{
    const char *pathsep = PDC_PATHSEP;

#ifdef MVS
    pdc_bool lastterm = pdc_false;
#endif

    if (!dirname || !dirname[0])
    {
        strcpy(fullname, basename);
    }
    else
    {
        fullname[0] = 0;
#ifdef MVS
        if (strncmp(dirname, PDC_FILEQUOT, 1))
            strcat(fullname, PDC_FILEQUOT);
#endif
        strcat(fullname, dirname);
#ifdef VMS
        /* look for logical name */
        if(getenv(dirname))
            pathsep = PDC_PATHSEP_LOG;
#endif
        strcat(fullname, pathsep);
        strcat(fullname, basename);
#ifdef MVS
        lastterm = pdc_true;
#endif
    }

#ifdef MVS
    {
        int ie, len;

        len = strlen(fullname);
        for (ie = len - 1; ie >= 0; ie--)
        {
            if (fullname[ie] == pathsep[0])
                break;

            if (fullname[ie] == '.')
            {
                fullname[ie] = 0;
                break;
            }
        }
        if (lastterm)
        {
            strcat(fullname, PDC_PATHTERM);
            strcat(fullname, PDC_FILEQUOT);
        }
    }
#endif
}

#define EXTRA_SPACE     32     /* extra space for separators, FILEQUOT etc. */

char *
pdc_file_fullname_mem(pdc_core *pdc, const char *dirname, const char *basename)
{
    static const char fn[] = "pdc_file_fullname_mem";
    char *fullname;
    size_t len;

    len = strlen(basename);
    if (dirname && dirname[0])
        len += strlen(dirname);
    len += EXTRA_SPACE;
    fullname = (char *) pdc_malloc(pdc, len, fn);

    pdc_file_fullname(dirname, basename, fullname);

    return fullname;
}

/*
 * Function reads a text file and creates a string list
 * of all no-empty and no-comment lines. The strings are stripped
 * by leading and trailing white space characters.
 *
 * The caller is responsible for freeing the resultated string list
 * by calling the function pdc_cleanup_stringlist.
 *
 * Not for unicode strings.
 *
 * Return value: Number of strings
 */

#define PDC_BUFSIZE 1024
#define PDC_ARGV_CHUNKSIZE 256

int
pdc_read_textfile(pdc_core *pdc, pdc_file *sfp, char ***linelist)
{
    static const char fn[] = "pdc_read_textfile";
    char   buf[PDC_BUFSIZE];
    char  *content = NULL;
    char **argv = NULL;
    int    nlines = 0;
    pdc_off_t filelen;
    size_t len, maxl = 0;
    int    tocont, incont = 0;
    int    i, is = 0, isb = 0;

    /* Get file length */
    filelen = pdc_file_size(sfp);
    if (filelen)
    {
        /* Allocate content array */
        content = (char *) pdc_malloc(pdc, (size_t) filelen, fn);

        /* Read loop */
        while (pdc_fgetline(buf, PDC_BUFSIZE, sfp) != NULL)
        {
            /* Strip blank and comment lines */
            pdc_str2trim(buf);
            if (buf[0] == 0 || buf[0] == '%')
                continue;

            /* Strip inline comments */
            len = strlen(buf);
            for (i = 1; i < (int) len; i++)
            {
                if (buf[i] == '%')
                {
                    if (buf[i-1] == '\\')
                    {
                        memmove(&buf[i-1], &buf[i], (size_t) (len - i));
                        len--;
                        buf[len] = 0;
                    }
                    else
                    {
                        buf[i] = 0;
                        pdc_strtrim(buf);
                        len = strlen(buf);
                        break;
                    }
                }
            }

            /* Continuation line */
            tocont = (buf[len-1] == '\\') ? 1:0;
            if (tocont)
            {
                buf[len-1] = '\0';
                len--;
            }
            if (!incont)
                isb = is;
            incont = tocont;

            /* Copy line */
            strcpy(&content[is], buf);

            /* Save whole line */
            if (!incont)
            {
                if (nlines >= (int) maxl)
                {
                    maxl += PDC_ARGV_CHUNKSIZE;
                    argv = (argv == NULL) ?
                            (char **)pdc_malloc(pdc, maxl * sizeof(char *), fn):
                            (char **)pdc_realloc(pdc, argv, maxl *
                                                 sizeof(char *), fn);
                }
                argv[nlines] = &content[isb];
                nlines++;

                pdc_trace_protocol(pdc, 2, trc_filesearch,
                        "\t\tLine %d; \"%s\"\n", nlines, argv[nlines - 1]);
            }

            /* Next index */
            is += (int) (len + 1 - incont);
        }

        if (!argv) pdc_free(pdc, content);
    }

    *linelist = argv;
    return nlines;
}


/* generate a temporary file name from the current time, pid, 'dirname',
** and the data in 'inbuf' using MD5. prepend 'dirname' to the file name.
** the result is written to 'outbuf'. if 'outbuf' is NULL, memory will be
** allocated and must be freed by the caller. otherwise, 'pdc' can be set
** to NULL.
**
** if 'dirname' isn't specified the function looks for an environment
** variable via the define PDC_TMPDIR_ENV. This define is set in
** pc_config.h. If the environment variable has a value and if the
** directory exists (check with the temporary file together) the
** directory will be used.
*/

#ifdef MVS
#define TMP_NAME_LEN	9
#define TMP_SUFFIX	""
#define TMP_SUFF_LEN	0
#else
#define TMP_NAME_LEN	14
#define TMP_SUFFIX	".TMP"
#define TMP_SUFF_LEN	4
#endif

char *
pdc_temppath(
    pdc_core *pdc,
    char *outbuf,
    const char *inbuf,
    size_t inlen,
    const char *dirname)
{
    char		name[TMP_NAME_LEN + TMP_SUFF_LEN + 1];
    MD5_CTX             md5;
    time_t              timer;
    unsigned char       digest[MD5_DIGEST_LENGTH];
    int                 i;
    size_t              dirlen;
#ifdef VMS
    char               *tmpdir = NULL;
#endif /* VMS */

#if defined(WIN32)
#if defined(__BORLANDC__)
    int pid = getpid();
#else
    int pid = _getpid();
#endif
#else
#if !defined(MAC)
    pid_t pid = getpid();
#endif
#endif

#ifdef PDC_TMPDIR_ENV
    if (!dirname)
    {
        dirname = (char *) getenv(PDC_TMPDIR_ENV);

        if (dirname)
        {
            FILE *fp;
            char *outbuf_p;

            outbuf_p = pdc_temppath(pdc, outbuf, inbuf, inlen, dirname);

            /* check */
            fp = fopen(outbuf_p, WRITEMODE);
            if (fp)
            {
                fclose(fp);
                remove(outbuf_p);
            }
            else
            {
                dirname = (char *) 0;
            }
            if (!outbuf)
               pdc_free(pdc, outbuf_p);
        }
    }
#endif /* !PDC_TMPDIR_ENV */

    time(&timer);

    MD5_Init(&md5);
#if !defined(MAC)
    MD5_Update(&md5, (unsigned char *) &pid, sizeof pid);
#endif
    MD5_Update(&md5, (unsigned char *) &timer, sizeof timer);

    if (inlen == 0 && inbuf != (const char *) 0)
	inlen = strlen(inbuf);

    if (inlen != 0)
	MD5_Update(&md5, (unsigned char *) inbuf, inlen);

    dirlen = dirname ? strlen(dirname) : 0;
    if (dirlen)
	MD5_Update(&md5, (const unsigned char *) dirname, dirlen);

    MD5_Final(digest, &md5);

    for (i = 0; i < TMP_NAME_LEN - 1; ++i)
        name[i] = (char) (PDF_A + digest[i % MD5_DIGEST_LENGTH] % 26);

    name[i] = 0;
    strcat(name, TMP_SUFFIX);

    if (!outbuf)
        outbuf = pdc_file_fullname_mem(pdc, dirname, name);
    else
        pdc_file_fullname(dirname, name, outbuf);
    return outbuf;
}

#if defined(MAC) || defined(MACOSX)


#endif /* (defined(MAC) || defined(MACOSX)) */

