#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "pdflib.h"

static void
bookmarks(PDF *p)
{
#define UNICODEINFO    "\376\377\0001\000.\000A\0\0"
#define LEFT 50
#define FONTSIZE 24
#define LEAD ((int) (FONTSIZE * 1.5))

    unsigned char buf[64], tmp[64];
    int c, i, pos;
    float y=700;
    int level1, level2=0, level3=0, font;

 // MESSAGE("Bookmark test...");

    font = PDF_findfont(p, "Helvetica", "host", 0);

 // PDF_begin_page(p, a4_width, a4_height);
    PDF_begin_page(p, letter_width, letter_height);

 // PDF_setfont(p, font, FONTSIZE);
    PDF_setfont(p, font, 12);

    char* buffer = "abcdefghijzlmnopqrstuvwxyzabcdefghijzlmnopqrstuvwxyz";
    PDF_translate(p,0,letter_height-36);

    PDF_show(p, buffer);
#if 1
    for (i=0; i < 20; i++)
         PDF_continue_text(p, buffer);
#endif

#if 0
    PDF_show_xy(p, "The bookmarks for this page contain all", LEFT, y-=LEAD);

    PDF_show_xy(p, "The bookmarks for this page contain all", LEFT, y-=LEAD);
    PDF_show_xy(p, "Unicode characters.", LEFT, y-=LEAD);
    PDF_show_xy(p, "Depending on the available fonts,", LEFT, y-=LEAD);
    PDF_show_xy(p, "only a certain subset will be visible on", LEFT, y-=LEAD);
    PDF_show_xy(p, "your system.", LEFT, y-=LEAD);
    
    /* private Unicode info entry */
    PDF_set_info(p, "Revision", UNICODEINFO);

    /* Generate Unicode bookmarks */
    level1 = PDF_add_bookmark(p, "Unicode bookmarks", 0, 0);

    for (c = 0; c < 65535; c += 16) {
        if (c % 4096 == 0) {
            sprintf((char *) tmp, "U+%04X", c);
            level2 = PDF_add_bookmark(p, (char *) tmp, level1, 0);
        }
        if (c % 256 == 0) {
            sprintf((char *) tmp, "U+%04X", c);
            level3 = PDF_add_bookmark(p, (char *) tmp, level2, 0);
        }
        /* now comes the actual Unicode string with the BOM */
        sprintf((char *) tmp, "0x%04X: ", c);

        /* write the Unicode byte order mark */
        strcpy((char *) buf, "\376\377");

        for (pos = 0; tmp[pos/2]; /* */ ) {
            buf[2 + pos] = 0;
            pos++;
            buf[2 + pos] = tmp[pos/2];  /* construct Unicode string */
            pos++;
        }

        pos += 2;       /* account for the BOM */
        for (i = 0; i < 16; i++) {      /* continue filling buf with chars */
            buf[pos++] = (unsigned char) ((((c+i)) & 0xFF00) >> 8);
            buf[pos++] = (unsigned char) (((c+i)) & 0x00FF);
        }

        /* signal end of string with two null bytes */
        buf[pos++] = (unsigned char) 0;
        buf[pos] = (unsigned char) 0;

        (void) PDF_add_bookmark(p, (char *) buf, level3, 1);
    }
#endif

    PDF_end_page(p);

// MESSAGE("done\n");

#undef FONTSIZE
#undef LEFT
#undef UNICODEINFO
#undef LEAD
}

int
main(int argc, char *argv[])
   {
     char	filename[50];
     PDF		*p;

     strcpy(filename, "pdftest.pdf");

     fprintf(stderr, "Creating PDFlib test file '%s'!\n", filename);

     PDF_boot();

     p = PDF_new();

     if (PDF_open_file(p, filename) == -1)
        {
    	  printf("Couldn't open PDF file '%s'!\n", filename);
	  return 1;
        }
#if 0
     PDF_set_info(p, "Keywords", "image graphics text hypertext");
     PDF_set_info(p, "Subject", "Check many PDFlib function calls");
     PDF_set_info(p, "Title", "PDFlib test program");
     PDF_set_info(p, "Creator", "PDFtest");
     PDF_set_info(p, "Author", "Thomas Merz");
#endif
     bookmarks(p);

     PDF_close(p);
     PDF_delete(p);

     PDF_shutdown();

     fprintf(stderr, "\nPDFlib test file '%s' finished!\n", filename);

     return 0;
   }
