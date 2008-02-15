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

/* $Id: pdfimage.c,v 1.1 2005/10/11 17:18:17 vuduc2 Exp $
 *
 * Convert BMP/PNG/TIFF/GIF/JPEG images to PDF
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

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

#include "pdflib.h"

static void
usage(void)
{
    fprintf(stderr, "pdfimage: convert images to PDF.\n");
    fprintf(stderr,
	"(c) PDFlib GmbH and Thomas Merz 1997-2005 www.pdflib.com\n\n");
    fprintf(stderr, "usage: pdfimage [options] imagefile(s)\n");
    fprintf(stderr, "Available options:\n");
    fprintf(stderr,
	    "-o <file>  output file (required)\n");
    fprintf(stderr,
	    "-p <num>   create bookmarks with page numbers starting at num\n");
    fprintf(stderr,
	    "-r <res>   force resolution overriding image settings\n");

    exit(1);
}

int
main(int argc, char *argv[])
{
    char	*pdffilename = NULL;
    PDF		*p;
    int		image;
    int		opt;
    int		resolution = 0;
    int		page_numbering = 0;
    int		frame;
    int		current_page = 1;
    char	optlist[256];
    char	buf[512];

    while ((opt = getopt(argc, argv, "r:o:p:")) != -1)
	switch (opt) {
	    case 'o':
		pdffilename = optarg;
		break;

	    case 'p':
		page_numbering = 1;
		if (optarg) {
		    current_page = atoi(optarg);
		}
		break;

	    case 'r':
		if (!optarg || (resolution = atoi(optarg)) <= 0) {
		    fprintf(stderr, "Error: non-positive resolution.\n");
		    usage();
		}

	    case '?':
	    default:
		usage();
	}

    if (optind == argc) {
	fprintf(stderr, "Error: no image files given.\n");
	usage();
    }

    if (pdffilename == NULL) {
	fprintf(stderr, "Error: no output file given.\n");
	usage();
    }

    if ((p = PDF_new()) == (PDF *) 0)
    {
        fprintf(stderr, "Couldn't create PDFlib object (out of memory)!\n");
	exit(99);
    }

    PDF_TRY(p) {
	if (PDF_begin_document(p, pdffilename, 0, "") == -1) {
	    fprintf(stderr, "Error: cannot open output file %s.\n",
		pdffilename);
	    exit(1);
	}

	PDF_set_info(p, "Creator", "pdfimage");
	PDF_set_parameter(p, "warning", "false");

	while (optind++ < argc) {
	    fprintf(stderr, "Processing image file '%s'...", argv[optind-1]);

	    /* process all frames in a multi-page image file */
	    for (frame=1; /* */; frame++)
	    {
		sprintf(optlist, "page %d", frame);
		image = PDF_load_image(p, "auto", argv[optind-1], 0, optlist);

		if (image == -1) {
		    if (frame == 1)
			fprintf(stderr, "\n%s (skipped).\n", PDF_get_errmsg(p));
		    break;
		}

		/* dummy page size, will be adjusted later */
		PDF_begin_page_ext(p, 20, 20, "");

		/* define outline with filename or page number */
		if (page_numbering) {
		    sprintf(buf, "Page %d", current_page++);
		    PDF_create_bookmark(p, buf, 0, "");
		} else {
		    if (frame > 1)
			sprintf(buf, "%s (%d)", argv[optind-1], frame);
		    else
			sprintf(buf, "%s", argv[optind-1]);
		    PDF_create_bookmark(p, buf, 0, "");
		}

		if (resolution != 0)
		    sprintf(optlist, "dpi %d", resolution);
		else
		    sprintf(optlist, "adjustpage");

		PDF_fit_image(p, image, 0.0, 0.0, optlist);

		PDF_end_page_ext(p, "");
	    }

	    if (frame > 2)
		fprintf(stderr, "(%d frames)", frame-1);
	    fprintf(stderr, "\n");
	}

	PDF_end_document(p, "");
    }

    PDF_CATCH(p) {
        printf("\npdfimage: error while creating PDF output (%s)\n",
		PDF_get_errmsg(p));
        PDF_delete(p);
        exit(99);
    }

    PDF_delete(p);

    exit(0);
}
