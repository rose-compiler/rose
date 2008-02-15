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

/* p_util.c
 *
 * PDFlib utility routines
 *
 */

#include <math.h>
#include <string.h>
#include <stdlib.h>
#ifdef AS400
#include <qp0z1170.h>	/* for getenv() emulation */
#endif
#include <stdio.h>

#include "p_intern.h"

/*
 * Format floating point numbers in a PDF compatible way.
 * This must be used for all floating output since PDF doesn't
 * allow %g exponential format, and %f produces too many characters
 * in most cases. PDF spec says: "use four or five decimal places".
 */

/* Acrobat viewers change absolute values < 1/65536 to zero */
#define SMALLREAL	0.000015

/* Acrobat viewers have an upper limit on real numbers */
#define BIGREAL		32767

const char *
pdf_float(char *buf, float f)
{
    if (fabs(f) < SMALLREAL)
	return strcpy(buf, "0");	/* force very small numbers to zero   */

    sprintf(buf, "%.4g", f);		/* try %g first and then check output */

    if (strchr(buf, 'e')) {		/* this format is not PDF compatible  */
	if (fabs(f) < 1)
	    sprintf(buf, "%1.5f", f);	/* 5 decimal places for small numbers */
	else if (fabs(f) <= BIGREAL)
	    sprintf(buf, "%1.2f", f);	/* 2 decimal places for medium numbers*/
	else
	    sprintf(buf, "%1.0f", f);	/* skip decimal places for big numbers*/
    }

    return buf;
}

/* Unicode-related stuff */

/* strlen() for unicode strings, which are terminated by two zero bytes.
 * wstrlen() returns the number of bytes in the Unicode string,
 * not including the two terminating null bytes.
 */
static size_t
wstrlen(const char *s)
{
    size_t len;

    for(len = 0; 
	(unsigned char) (s[len++]) != 0 ||
	(unsigned char) (s[len++]) != 0; /* */ ) {
	/* */
    }

    return len-2;
}

/*
 * This function returns the length in bytes for C and Unicode strings.
 * Note that unlike strlen() it returns the length _including_ the 
 * terminator, which may be one or two null bytes.
 */
size_t
pdf_strlen(const char *text)
{
    if (pdf_is_unicode(text))
	return wstrlen(text) + 2;
    else
	return strlen(text) + 1;
}

/* Allocate a PDFlib-local buffer and copy the string including
 * the terminating sentinel. If the string starts with the Unicode BOM
 * it is considered a Unicode string, and must be terminated by
 * two null bytes. Otherwise it is considered a plain C string and
 * must be terminated by a single null byte.
 * The caller is responsible for freeing the buffer.
 */
char *
pdf_strdup(PDF *p, const char *text)
{
    char *buf;
    size_t len;

    if (text == NULL)
	pdf_error(p, PDF_SystemError, "NULL string in pdf_strdup");
	
    len = pdf_strlen(text);
    buf = (char *) p->malloc(p, len, "pdf_strdup");
    memcpy(buf, text, len);

    return buf;
}

/* ----------------- Resource handling ----------------------- */
static void pdf_init_resources(PDF *p);

char *
pdf_find_resource(PDF *p, const char *category, const char *resourcename, pdf_bool loadupr)
{
    pdf_category *cat;
    pdf_res *res;
    
    /* Resources may have been added individually with PDF_set_parameter().
     * If so, we search them; if not, we have to load the resource file.
     */
    if (p->resources == NULL && !p->resourcefile_loaded)
	pdf_init_resources(p);

    for (cat = p->resources; cat != (pdf_category *) NULL; cat = cat->next) {
	if (!strcmp(cat->category, category)) {
	    for (res = cat->kids; res != (pdf_res *)NULL; res=res->next) {
		if (!strcmp(res->name, resourcename))
		    return res->filename;
	    }
	    if (p->resourcefile_loaded)	/* all hope is lost */
		return NULL;
	}
    }

    /*
     * Up to now we may have searched the individually loaded resources,
     * but not the resource file. Since we haven't found the resource,
     * we now (very lazily) load the resource file and try again.
     *
     * However, we only do this if the caller explicitly requested it.
     * This allows for pure dynamic configuration without any UPR file.
     */

    if(!loadupr)
	return NULL;

    if (!p->resourcefile_loaded) {
	pdf_init_resources(p);
	return pdf_find_resource(p, category, resourcename, pdf_true);
    }

    pdf_error(p, PDF_SystemError, "Resource category '%s' not found", category);
    
    /* Satisfy the compiler */
    return NULL;
}

/* The UPR file handling routines are based on the publicly available
 * utility makepsres from Adobe Systems.
 */
static void
StripComments(char *buf)
{
    register char *ch = buf;

    for (/* */ ; /* */ ; /* */) {
	while (*ch != '%' && *ch != '\0')
	    ch++;
	if (*ch == '\0')
	    break;
	if (ch == buf || *(ch-1) != '\\') {
	    *ch = '\0';
	    break;
	}
	ch++;
    }

    /* ch points to '\0' right now */

    if (ch == buf)
	return;
    ch--;

    while (ch > buf && (*ch == ' ' || *ch == '\t' || *ch == '\n' || *ch == '\r')) {
	*ch = '\0';
	ch--;
    }

    if (ch == buf && (*ch == ' ' || *ch == '\t' || *ch == '\n' || *ch == '\r'))
	*ch = '\0';
}

/* Caller must free returned line */

#define BUFSIZE 1024

static char *
GetWholeLine(PDF *p, FILE *fp)
{
    char *line;
    char buf[BUFSIZE];
    size_t len, oldlen;

    do {
	if (fgets(buf, BUFSIZE, fp) == NULL)
	    return NULL;
	StripComments(buf);	/* Strip comments and blank lines */
    } while (buf[0] == 0);

    StripComments(buf);
    
    len = strlen(buf); 

    if (buf[len-1] == '\n' && buf[len-2] == '\r') {
        buf[len-2] = '\n';                /* adjust DOS-style line ends */
        buf[len-1] = '\0';
        len--;
    } else if (buf[len-1] == '\r') {
        buf[len-1] = '\n';                /* adjust Mac-style line ends */
    }

    line = (char *) p->malloc(p, len+1, "GetWholeLine in UPR parser");
    strcpy(line, buf);

    if (line[len-1] == '\\') {	/* Continued... */
	line[len-1] = '\0';
	oldlen = len-1;
	for (;;) {
	    if (fgets(buf, BUFSIZE, fp) == NULL)
		return line;

	    StripComments(buf);
	    if (buf[0] == '\0') return line;

	    len = strlen(buf);
	    line = (char *) p->realloc(p, line, oldlen+len+1,
			     "GetWholeLine in UPR parser");
	    strcat(line, buf);

	    oldlen += len;
	    if (line[oldlen-1] != '\\')
		break;
	    line[oldlen-1] = '\0';
	    oldlen--;
	}
    }
    return line;
}

void
pdf_add_resource(PDF *p, const char *category, const char *resource,
		const char *filename, const char *prefix)
{
    pdf_category *cat, *lastcat = NULL;
    pdf_res *res, *lastres = NULL;

    /* All other (unknown) categories raise an error */
    if (strcmp("FontOutline", category) &&
	strcmp("FontAFM", category) &&
	strcmp("Encoding", category) &&
	strcmp("FontPFM", category)) {
	    return;
    }

    /* find start of this category's resource list, if the category exists */
    for (cat = p->resources; cat != (pdf_category *) NULL; cat = cat->next) {
	lastcat = cat;
	if (!strcmp(cat->category, category))
	    break;
    }

    if (cat == NULL) {
	cat = (pdf_category *) p->malloc(p, 
			sizeof(pdf_category), "pdf_add_resource");
	cat->category = pdf_strdup(p, category);
	cat->kids = NULL;
	cat->next = NULL;

	if (lastcat) {
	    lastcat->next = cat;
	}else {
	    p->resources = cat;
	}
    }

    if (prefix == NULL) {
	if (p->prefix)
	    prefix = p->prefix;
	else
	    prefix = "";
	if (filename[0] == '.' && filename[1] == '/')
	    filename += 2;
    } else {
	prefix++; /* Skip over leading / */
	if (prefix[0] == '.' && prefix[1] == '/')
	    prefix += 2;
    }

    /* Go to the end of the resource list */
    for (res = cat->kids; res != (pdf_res *) NULL; res = res->next)
	lastres = res;

    res = (pdf_res *) p->malloc(p, sizeof(pdf_res), "pdf_add_resource");

    if (lastres)
	lastres->next = res;
    else
	cat->kids = res;

    res->next = NULL;
    res->name = pdf_strdup(p, resource);

    /* we need two extra bytes for the OS/390's closing ")'" */
    res->filename = (char *) p->malloc(p, strlen(filename) + strlen(prefix) + 4,
		"pdf_add_resource");

    if (prefix != NULL && prefix[0] != '\0') {
	strcpy (res->filename, prefix);
	strcat (res->filename, PATHSEP);
	strcat (res->filename, filename);
#ifdef  OS390
        strcat (res->filename, ")'");
#endif  /* OS390 */
    } else
	strcpy (res->filename, filename);
}

void
pdf_cleanup_resources(PDF *p)
{
    pdf_category *cat, *lastcat;
    pdf_res *res, *lastres;

    for (cat = p->resources; cat != (pdf_category *) NULL; /* */) {
	for (res = cat->kids; res != (pdf_res *) NULL; /* */) {
	    lastres = res;
	    res = lastres->next;
	    p->free(p, lastres->name);
	    p->free(p, lastres->filename);
	    p->free(p, lastres);
	}
	lastcat = cat;
	cat = lastcat->next;
	p->free(p, lastcat->category);
	p->free(p, lastcat);
    }

    p->resources = NULL;
}

static void
pdf_init_resources(PDF *p)
{
    char	*uprfilename;
    char	*resourceFile;
    FILE	*fp;
    char	buf[BUFSIZE];
    char	*line;
    char	*category;
    char	*prefix = NULL;

    if (p->resourcefile_loaded)
	return;
    else
	p->resourcefile_loaded = pdf_true;

#ifndef MAC
    /* user-supplied upr file */
    uprfilename = getenv(RESOURCEFILE);
#else
    uprfilename = NULL;
#endif
    /* client-supplied res file */
    if (uprfilename == NULL || *uprfilename == '\0')
	uprfilename = p->resourcefilename;

    /* default resource file */
    if (uprfilename == NULL || *uprfilename == '\0')
	uprfilename = pdf_strdup(p, DEFAULTRESOURCEFILE);

    if ((fp = fopen(uprfilename, "r")) == NULL)
	pdf_error(p, PDF_IOError, "Resource configuration file '%s' not found",
		uprfilename);

    /* Skip over list of categories */
    do {
	if (fgets(buf, BUFSIZE, fp) == NULL) {
	    fclose(fp);
	    return;
	}
    } while (buf[0] != '.');

    for (;;) {	/* process categories */
	line = GetWholeLine(p, fp);

	if (line == NULL)
	    break;
      
	if (line[0] == '/') {		/* handle optional directory prefix */
	    prefix = line;
	    continue;
	}

	category = line;

	for (;;) {	/* process lines for this category */

	    line = GetWholeLine(p, fp);
	    if (line == NULL) {
		if (prefix != NULL)
		    p->free(p, prefix);
		p->free(p, category);
		fclose(fp);
		return;
	    }

	    if (line[0] == '.') {
		p->free(p, category);
		p->free(p, line);
		break;
	    }

	    resourceFile = line;

	    while ((resourceFile = strchr(resourceFile, '=')) != NULL) {
		if (resourceFile != line && *(resourceFile-1) != '\\') {
		    *resourceFile++ = '\0';

		    if (*resourceFile == '=') {
			resourceFile++;		/* skip '=' character */
			pdf_add_resource(p, category, line, 
				resourceFile, NULL);
		    }else {
			pdf_add_resource(p, category, line, 
				resourceFile, prefix);
		    }

		    break;
		}
		resourceFile++;
	    }

	    p->free(p, line);
	}	/* process lines for this category */
    }		/* process categories */

    if (prefix != NULL)
	p->free(p, prefix);
    fclose(fp);
}

/*
 * Load a user-defined encoding from file and return a unique
 * encoding index.
 */

int
pdf_load_encoding(PDF *p, const char *filename, const char *encoding)
{
    FILE	*fp;
    char	*line, charname[128];
    int		ret, slot, l;
    pdf_encodingvector *enc;

    /* find next encoding slot */
    for (slot = 0; slot < ENC_MAX; slot++)
	if (p->encodings[slot] == NULL)
	    break;
    
    if (slot == ENC_MAX)
	pdf_error(p, PDF_SystemError, "Too many encodings");

    enc = (pdf_encodingvector *) p->malloc(p, sizeof(pdf_encodingvector),
    		"pdf_load_encoding");

    ret = slot;

    enc->apiname = pdf_strdup(p, encoding);
    enc->pdfname = NULL;

    if ((fp = fopen(filename, "r")) == NULL)
	pdf_error(p, PDF_IOError, "Encoding file '%s' not found", filename);

    for (slot = 0; slot < 256; slot++)
	enc->chars[slot] = NULL;

    l = 0;

    for (;;) {
	line = GetWholeLine(p, fp);
	l++;

	if (line == NULL)
	    break;

	if (*line == '\0' || *line == '\r') {
	    p->free(p, line);
	    continue;
	}

	if (sscanf(line, "%s %d", charname, &slot) != 2 ||
	    slot <= 0 || slot > 256) {
	    p->free(p, line);
	    fclose(fp);
	    pdf_error(p, PDF_IOError, "Bad line %d in encoding file '%s'",
	    	l, filename);
	}
      
	enc->chars[slot] = pdf_strdup(p, charname);
	p->free(p, line);
    }

    fclose(fp);
    p->encodings[ret] = enc;

    return ret;
}
