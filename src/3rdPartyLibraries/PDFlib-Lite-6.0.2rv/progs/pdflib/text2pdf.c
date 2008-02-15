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

/* $Id: text2pdf.c,v 1.1 2005/10/11 17:18:17 vuduc2 Exp $
 * 
 * Convert text files to PDF
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined(__CYGWIN32__)
#include <getopt.h>
#elif defined(WIN32)
int getopt(int argc, char * const argv[], const char *optstring);
extern char *optarg;
extern int optind;
#elif !defined(WIN32) && !defined(MAC)
#include <unistd.h>
#endif


#ifdef WIN32
#include <process.h>
#endif

#ifdef NeXT
#include <libc.h>	/* for getopt(), optind, optarg */
#endif

#ifdef __CYGWIN32__
#include <getopt.h>	/* for getopt(), optind, optarg */
#endif

#if defined WIN32 || defined __DJGPP__ || \
    defined __OS2__ || defined __IBMC__ || defined __IBMCPP__ || \
    defined __POWERPC__ || defined __CFM68K__ || defined __MC68K__ || \
    defined AS400 || defined __ILEC400__

#define READMODE	"rb"

#else

#define READMODE	"r"

#endif	/* Mac, Windows, and OS/2 platforms */

/* figure out whether or not we're running on an EBCDIC-based machine */
#define ASCII_A                 0x41
#define PLATFORM_A              'A'
#define EBCDIC_BRACKET          0x4A
#define PLATFORM_BRACKET        '['

#if (ASCII_A != PLATFORM_A && EBCDIC_BRACKET == PLATFORM_BRACKET)
#define PDFLIB_EBCDIC
#endif

#include "pdflib.h"

static void
usage(void)
{
    fprintf(stderr, "text2pdf - convert text files to PDF.\n");
    fprintf(stderr,
	"(c) PDFlib GmbH and Thomas Merz 1997-2005 www.pdflib.com\n\n");
    fprintf(stderr, "usage: text2pdf [options] [textfile]\n");
    fprintf(stderr, "Available options:\n");
    fprintf(stderr,
	"-e encoding   font encoding to use. Common encoding names:\n");
    fprintf(stderr,
	"              winansi, macroman, ebcdic, or user-defined\n");
    fprintf(stderr, "              host = default encoding of this platform\n");
    fprintf(stderr, "-f fontname   name of font to use\n");
    fprintf(stderr, "-h height     page height in points\n");
    fprintf(stderr, "-m margin     margin size in points\n");
    fprintf(stderr, "-o filename   PDF output file name\n");
    fprintf(stderr, "-s size       font size\n");
    fprintf(stderr, "-w width      page width in points\n");

    exit(1);
}

#define BUFLEN 		512

int
main(int argc, char *argv[])
{
    char	buf[BUFLEN], *s;
    char	*pdffilename = NULL;
    FILE	*textfile = stdin;
    PDF		*p;
    int		opt;
    int		font;
    char	*fontname, *encoding;
    double	fontsize;
    double	x, y, width = a4_width, height = a4_height, margin = 20;
    char	ff, nl;
    
    fontname	= "Courier";
    fontsize	= 12.0;
    encoding	= "host";
    nl		= '\n';
    ff		= '\f';

    while ((opt = getopt(argc, argv, "e:f:h:m:o:s:w:")) != -1)
	switch (opt) {
	    case 'e':
		encoding = optarg;
		break;

	    case 'f':
		fontname = optarg;
		break;

	    case 'h':
		height = atof(optarg);
		if (height < 0) {
		    fprintf(stderr, "Error: bad page height %f!\n", height);
		    usage();
		}
		break;

	    case 'm':
		margin = atof(optarg);
		if (margin < 0) {
		    fprintf(stderr, "Error: bad margin %f!\n", margin);
		    usage();
		}
		break;

	    case 'o':
		pdffilename = optarg;
		break;

	    case 's':
		fontsize = atof(optarg);
		if (fontsize < 0) {
		    fprintf(stderr, "Error: bad font size %f!\n", fontsize);
		    usage();
		}
		break;

	    case 'w':
		width = atof(optarg);
		if (width < 0) {
		    fprintf(stderr, "Error: bad page width %f!\n", width);
		    usage();
		}
		break;

	    case '?':
	    default:
		usage();
	}

    if (!strcmp(encoding, "ebcdic")) {
	/* form feed is 0x0C in both ASCII and EBCDIC */
	nl = 0x15;
    }

    if (pdffilename == NULL)
	usage();

    if (optind < argc) {
	if ((textfile = fopen(argv[optind], READMODE)) == NULL) {
	    fprintf(stderr, "Error: cannot open input file %s.\n",argv[optind]);
	    exit(2);
	}
    } else
	textfile = stdin;

    p = PDF_new();
    if (p == NULL) {
	fprintf(stderr, "Error: cannot open output file %s.\n", pdffilename);
	exit(1);
    }

    PDF_begin_document(p, pdffilename, 0, "");

    PDF_set_info(p, "Title", "Converted text");
    PDF_set_info(p, "Creator", "text2pdf");

    x = margin;
    y = height - margin;

    while ((s = fgets(buf, BUFLEN, textfile)) != NULL) {
	if (s[0] == ff) {
	    if (y == height - margin)
		PDF_begin_page_ext(p, width, height, "");
	    PDF_end_page_ext(p, "");
	    y = height - margin;
	    continue;
	}

	if (s[0] != '\0' && s[strlen(s) - 1] == nl)
	    s[strlen(s) - 1] = '\0';	/* remove newline character */

	if (y < margin) {		/* page break necessary? */
	    y = height - margin;
	    PDF_end_page_ext(p, "");
	}

	if (y == height - margin) {
	    PDF_begin_page_ext(p, width, height, "");
	    font = PDF_load_font(p, fontname, 0, encoding, "");
	    PDF_setfont(p, font, fontsize);
	    PDF_set_text_pos(p, x, y);
	    y -= fontsize;
	}

	PDF_continue_text(p, s);
	y -= fontsize;

    }

    if (y != height - margin)
	PDF_end_page_ext(p, "");

    PDF_end_document(p, "");
    PDF_delete(p);

    exit(0);
}
