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

/* p_afm.c
 *
 * PDFlib AFM parsing routines
 *
 * This source is based on Adobe's AFM parser which is available
 * via the Adobe Developers Association. It has been adapted
 * to PDFlib programming conventions, some bugs fixed, made thread-safe,
 * enhanced for newer AFM versions, and improved in several ways.
 *
 * The original file had the following notice:
 */
/*
 * Copyright (C) 1988, 1989, 1990, 1991 by Adobe Systems Incorporated. 
 * All rights reserved.
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "p_intern.h"
#include "p_font.h"
 
/* Flags that can be AND'ed together to specify exactly what
 * information from the AFM file should be saved.
 */
#define P_G	0x01	/* 0000 0001 */   /* Global Font Info      */
#define P_W	0x02	/* 0000 0010 */   /* Character Widths ONLY */
#define P_M	0x06	/* 0000 0110 */   /* All Char Metric Info  */
#define P_P	0x08	/* 0000 1000 */   /* Pair Kerning Info     */
#define P_T	0x10	/* 0001 0000 */   /* Track Kerning Info    */
#define P_C	0x20	/* 0010 0000 */   /* Composite Char Info   */

/* Possible return codes from the parseFile procedure. */
#define ok		0
#define parseError	-1
#define earlyEOF	-2

/* These  are deliberately not in ASCII */
#define CR	'\r'	/* Carriage Return */
#define NL	'\n'	/* Newline */

#define normalEOF 1	/* return code from parsing routines */
#define MAX_NAME 4096	/* max length for identifiers */

/* The values of each of these enumerated items correspond to an entry in the
 * table of strings defined below. Therefore, if you add a new string as 
 * new keyword into the keyStrings table, you must also add a corresponding
 * parseKey AND it MUST be in the same position!
 *
 * IMPORTANT: since the sorting algorithm is a binary search, the strings of
 * keywords must be placed in lexicographical order, below. [Therefore, the 
 * enumerated items are not necessarily in lexicographical order, depending 
 * on the name chosen. BUT, they must be placed in the same position as the 
 * corresponding key string.] The NOPE shall remain in the last position, 
 * since it does not correspond to any key string, and it is used in the 
 * "recognize" procedure to calculate how many possible keys there are.
 */

#ifndef PDFLIB_EBCDIC
enum parseKey {
  ASCENDER, CHARBBOX, CODE, COMPCHAR, CAPHEIGHT, CHARWIDTH, CHARACTERS,
  COMMENT, DESCENDER, ENCODINGSCHEME, ENDCHARMETRICS, ENDCOMPOSITES, 
  ENDFONTMETRICS, ENDKERNDATA, ENDKERNPAIRS, ENDTRACKKERN, 
  FAMILYNAME, FONTBBOX, FONTNAME, FULLNAME, ISFIXEDPITCH, 
  ITALICANGLE, KERNPAIR, KERNPAIRXAMT, LIGATURE, CHARNAME, 
  NOTICE, COMPCHARPIECE, STARTCHARMETRICS, STARTCOMPOSITES, 
  STARTFONTMETRICS, STARTKERNDATA, STARTKERNPAIRS, 
  STARTTRACKKERN, STDHW, STDVW, TRACKKERN, UNDERLINEPOSITION, 
  UNDERLINETHICKNESS, VERSION, XYWIDTH, XWIDTH, WEIGHT, XHEIGHT,
  NOPE };

/* keywords for the system:  
 * This a table of all of the current strings that are vaild AFM keys.
 * Each entry can be referenced by the appropriate parseKey value (an
 * enumerated data type defined above). If you add a new keyword here, 
 * a corresponding parseKey MUST be added to the enumerated data type
 * defined above, AND it MUST be added in the same position as the 
 * string is in this table.
 *
 * IMPORTANT: since the sorting algorithm is a binary search, the keywords
 * must be placed in lexicographical order. And, NULL should remain at the
 * end.
 */

static const char *keyStrings[] = {
  "Ascender", "B", "C", "CC", "CapHeight", "CharWidth", "Characters",
  "Comment", "Descender", "EncodingScheme", "EndCharMetrics", "EndComposites", 
  "EndFontMetrics", "EndKernData", "EndKernPairs", "EndTrackKern", 
  "FamilyName", "FontBBox", "FontName", "FullName", "IsFixedPitch", 
  "ItalicAngle", "KP", "KPX", "L", "N", 
  "Notice", "PCC", "StartCharMetrics", "StartComposites", 
  "StartFontMetrics", "StartKernData", "StartKernPairs", 
  "StartTrackKern", "StdHW", "StdVW", "TrackKern", "UnderlinePosition", 
  "UnderlineThickness", "Version", "W", "WX", "Weight", "XHeight",
  NULL };

#else	/* PDFLIB_EBCDIC */

enum parseKey {
  ASCENDER, CHARBBOX, CODE, COMPCHAR, CAPHEIGHT, CHARACTERS, CHARWIDTH,
  COMMENT, DESCENDER, ENCODINGSCHEME, ENDCHARMETRICS, ENDCOMPOSITES, 
  ENDFONTMETRICS, ENDKERNDATA, ENDKERNPAIRS, ENDTRACKKERN, 
  FAMILYNAME, FONTBBOX, FONTNAME, FULLNAME, ISFIXEDPITCH, 
  ITALICANGLE, KERNPAIR, KERNPAIRXAMT, LIGATURE, CHARNAME, 
  NOTICE, COMPCHARPIECE, STARTCHARMETRICS, STARTCOMPOSITES, 
  STARTFONTMETRICS, STARTKERNDATA, STARTKERNPAIRS, 
  STARTTRACKKERN, STDHW, STDVW, TRACKKERN, UNDERLINEPOSITION, 
  UNDERLINETHICKNESS, VERSION, XYWIDTH, WEIGHT, XWIDTH, XHEIGHT,
  NOPE };

static const char *keyStrings[] = {
  "Ascender", "B", "C", "CC", "CapHeight", "Characters", "CharWidth",
  "Comment", "Descender", "EncodingScheme", "EndCharMetrics", "EndComposites", 
  "EndFontMetrics", "EndKernData", "EndKernPairs", "EndTrackKern", 
  "FamilyName", "FontBBox", "FontName", "FullName", "IsFixedPitch", 
  "ItalicAngle", "KP", "KPX", "L", "N", 
  "Notice", "PCC", "StartCharMetrics", "StartComposites", 
  "StartFontMetrics", "StartKernData", "StartKernPairs", 
  "StartTrackKern", "StdHW", "StdVW", "TrackKern", "UnderlinePosition", 
  "UnderlineThickness", "Version", "W", "Weight", "WX", "XHeight",
  NULL };


#endif	/* PDFLIB_EBCDIC */
  
/*************************** token *************************/

/*  A "AFM File Conventions" tokenizer. That means that it will
 *  return the next token delimited by white space.
 */
 
static char *
token(FILE *fp, char *ident)
{
    int ch, idx;

    /* skip over white space */
    while ((ch = fgetc(fp)) == ' ' || ch == CR || ch == NL || 
            ch == ',' || ch == '\t' || ch == ';') {
	    ; /* */
    }
    
    idx = 0;
    while (ch != EOF && ch != ' ' && ch != CR && ch != NL 
           && ch != '\t' && ch != ':' && ch != ';') {
        ident[idx++] = (char) ch;
        ch = fgetc(fp);
    } /* while */

    if (ch == EOF && idx < 1)
	return ((char *)NULL);
    if (idx >= 1 && ch != ':' )
	ungetc(ch, fp);
    if (idx < 1 )
	ident[idx++] = (char) ch;	/* single-character token */

    ident[idx] = 0;
    
    return(ident);	/* returns pointer to the token */

} /* token */


/*************************** linetoken *************************/

/*  "linetoken" will read all tokens until the EOL character from
 *  the given fp.  This is used to get any arguments that can be
 *  more than one word (like Comment lines and FullName).
 */

static char *
linetoken(FILE *fp, char *ident)
{
    int ch, idx;

    while ((ch = fgetc(fp)) == ' ' || ch == '\t' ){
	/* */ ;
    }
    
    idx = 0;
    while (ch != EOF && ch != CR && ch != NL) {
        ident[idx++] = (char) ch;
        ch = fgetc(fp);
    } /* while */
    
    ungetc(ch, fp);
    ident[idx] = 0;

    return(ident);	/* returns pointer to the token */

} /* linetoken */


/*************************** recognize *************************/

/*  This function tries to match a string to a known list of
 *  valid AFM entries (check the keyStrings array above). 
 *  "word" contains everything from white space through the
 *  next space, tab, or ":" character.
 *
 *  The algorithm is a standard Knuth binary search.
 */

static enum parseKey
recognize(char *word)
{
    int lower = 0, upper = (int) NOPE, midpoint=0, cmpvalue;
    int found = pdf_false;

    while ((upper >= lower) && !found) {
        midpoint = (lower + upper)/2;
        if (keyStrings[midpoint] == NULL)
	    break;
        cmpvalue = strncmp(word, keyStrings[midpoint], MAX_NAME);
        if (cmpvalue == 0)
	    found = pdf_true;
        else if (cmpvalue < 0)
	    upper = midpoint - 1;
        else
	    lower = midpoint + 1;
    } /* while */

    if (found)
	return (enum parseKey) midpoint;
    else
	return NOPE;
    
} /* recognize */


/************************* parseGlobals *****************************/

/*  This function is called by pdf_parse_afm. It will parse the AFM File
 *  up to the "StartCharMetrics" keyword, which essentially marks the
 *  end of the Global Font Information and the beginning of the character
 *  metrics information. 
 *
 *  This function returns an error code specifying whether there was 
 *  a premature EOF or a parsing error. This return value is used by 
 *  parseFile to determine if there is more file to parse.
 */
 
static int
parseGlobals(PDF *p, FILE *fp, pdf_font *font, char *ident)
{
    int cont = pdf_true;
    int error = ok;
    register char *keyword;
    
    while (cont) {
        keyword = token(fp, ident);
        
          /* Have reached an early and unexpected EOF. */
          /* Set flag and stop parsing */
        if (keyword == NULL) {
            error = earlyEOF;
            break;   /* get out of loop */
        }
	switch(recognize(keyword)) {
	    case STARTFONTMETRICS:
		(void) token(fp, ident);
		break;
	    case CHARACTERS:
		(void) token(fp, ident);	/* eat # of characters */
		break;
	    /* New AFM 4.1 keyword "CharWidth" implies fixed pitch */
	    case CHARWIDTH:
		(void) token(fp, ident);	/* eat x value */
		(void) token(fp, ident);	/* eat y value */
		font->isFixedPitch = pdf_true;
		break;
	    case COMMENT:
		(void) linetoken(fp, ident);
		break;
	    case FONTNAME:
		keyword = token(fp, ident);
		font->name = pdf_strdup(p, keyword);
		break;
	    case ENCODINGSCHEME:
		keyword = token(fp, ident);
		font->encodingScheme = pdf_strdup(p, keyword);
		break; 
	    case FULLNAME:
		keyword = linetoken(fp, ident);
		font->fullName = pdf_strdup(p, keyword);
		break; 
	    case FAMILYNAME:           
		/* recognize Multiple Master fonts by name */
	        keyword = linetoken(fp, ident);
		font->familyName = pdf_strdup(p, keyword);
		if (strlen(keyword) > 3 &&
		    !strcmp(keyword + strlen(keyword) - 3, " MM"))
		    font->multimaster = pdf_true;
		break; 
	    case WEIGHT:
		/* Weight may read "All (Multiple Master)" for MM fonts */
		keyword = linetoken(fp, ident);
		font->weight = pdf_strdup(p, keyword);
		break;
	    case ITALICANGLE:
		keyword = token(fp, ident);
		font->italicAngle = (float) atof(keyword);
		break;
	    case ISFIXEDPITCH:
		keyword = token(fp, ident);
		if (!strncmp(keyword, "false", MAX_NAME))
		    font->isFixedPitch = pdf_false;
		else 
		    font->isFixedPitch = pdf_true;
		break; 
	    case UNDERLINEPOSITION:
		keyword = token(fp, ident);
		font->underlinePosition = atoi(keyword);
		break; 
	    case UNDERLINETHICKNESS:
		keyword = token(fp, ident);
		font->underlineThickness = atoi(keyword);
		break;
	    case VERSION:
		(void) linetoken(fp, ident);
		break; 
	    case NOTICE:
		(void) linetoken(fp, ident);
		break; 
	    case FONTBBOX:
		keyword = token(fp, ident);
		font->llx = (float) atof(keyword);
		keyword = token(fp, ident);
		font->lly = (float) atof(keyword);
		keyword = token(fp, ident);
		font->urx = (float) atof(keyword);
		keyword = token(fp, ident);
		font->ury = (float) atof(keyword);
		break;
	    case CAPHEIGHT:
		keyword = token(fp, ident);
		font->capHeight = atoi(keyword);
		break;
	    case XHEIGHT:
		keyword = token(fp, ident);
		font->xHeight = atoi(keyword);
		break;

	    /* added by tm Oct 07, 1997 */
	    case STDHW:
		keyword = token(fp, ident);
		font->StdHW = atoi(keyword);
		break;
	    case STDVW:
		keyword = token(fp, ident);
		font->StdVW = atoi(keyword);
		break;
	    /* end new stuff */

	    case DESCENDER:
		keyword = token(fp, ident);
		font->descender = atoi(keyword);
		break;
	    case ASCENDER:
		keyword = token(fp, ident);
		font->ascender = atoi(keyword);
		break;
	    case STARTCHARMETRICS:
		cont = pdf_false;
		break;
	    case ENDFONTMETRICS:
		cont = pdf_false;
		error = normalEOF;
		break;
	    case NOPE:
	    default:
		error = parseError;
		break;
	} /* switch */
    } /* while */
    
    return(error);
    
} /* parseGlobals */ 

#ifdef CHARWIDTHS_SUPPORTED

/************************* parseCharWidths **************************/

/*  This function is called by "parseFile". It will parse the AFM File
 *  up to the "EndCharMetrics" keyword. It will save the character 
 *  width info (as opposed to all of the character metric information)
 *  if requested by the caller of parseFile. Otherwise, it will just
 *  parse through the section without saving any information.
 *
 *  If data is to be saved, parseCharWidths is passed in a pointer 
 *  to an array of widths that has already been initialized by the
 *  standard value for unmapped character codes. This function parses
 *  the Character Metrics section only storing the width information
 *  for the encoded characters into the array using the character code
 *  as the index into that array.
 *
 *  This function returns an error code specifying whether there was 
 *  a premature EOF or a parsing error. This return value is used by 
 *  parseFile to determine if there is more file to parse.
 */
 
static int
parseCharWidths(PDF *p, FILE *fp, int *cwi, char *ident)
{
    int cont = pdf_true, save = (cwi != NULL);
    int pos = 0, error = ok;
    register char *keyword;
    
    while (cont) {
        keyword = token(fp, ident);
          /* Have reached an early and unexpected EOF. */
          /* Set flag and stop parsing */
        if (keyword == NULL)
        {
            error = earlyEOF;
            break; /* get out of loop */
        }
        if (!save)	
          /* get tokens until the end of the Char Metrics section without */
          /* saving any of the data*/
            switch (recognize(keyword))  {				
                case ENDCHARMETRICS:
                    cont = pdf_false;
                    break; 
                case ENDFONTMETRICS:
                    cont = pdf_false;
                    error = normalEOF;
                    break;
                default: 
                    break;
            } /* switch */
        else
          /* otherwise parse entire char metrics section, saving */
          /* only the char x-width info */
            switch(recognize(keyword)) {
                case COMMENT:
                    keyword = linetoken(fp, ident);
                    break;
                case CODE:
                    keyword = token(fp, ident);
                    pos = atoi(keyword);
                    break;
                case XYWIDTH:
                /* PROBLEM: Should be no Y-WIDTH when doing "quick & dirty" */
                    keyword = token(fp, ident);
		    keyword = token(fp, ident); /* eat values */
                    error = parseError;
                    break;
                case XWIDTH:
                    keyword = token(fp, ident);
                    if (pos >= 0) /* ignore unmapped chars */
                        cwi[pos] = atoi(keyword);
                    break;
                case ENDCHARMETRICS:
                    cont = pdf_false;
                    break; 
                case ENDFONTMETRICS:
                    cont = pdf_false;
                    error = normalEOF;
                    break;
                case CHARNAME:	/* eat values (so doesn't cause parseError) */
                    keyword = token(fp, ident); 
                    break;
            	case CHARBBOX: 
                    keyword = token(fp, ident);
		    keyword = token(fp, ident);
                    keyword = token(fp, ident);
		    keyword = token(fp, ident);
		    break;
		case LIGATURE:
                    keyword = token(fp, ident);
		    keyword = token(fp, ident);
		    break;
                case NOPE:
                default: 
                    error = parseError;
                    break;
            } /* switch */
    } /* while */
    
    return(error);
    
} /* parseCharWidths */    
#endif /* CHARWIDTHS_SUPPORTED */


/************************* parseCharMetrics ************************/

/*  This function is called by parseFile if the caller of parseFile
 *  requested that all character metric information be saved
 *  (as opposed to only the character width information).
 *
 *  parseCharMetrics is passed in a pointer to an array of records
 *  to hold information on a per character basis. This function
 *  parses the Character Metrics section storing all character
 *  metric information for the ALL characters (mapped and unmapped) 
 *  into the array.
 *
 *  This function returns an error code specifying whether there was 
 *  a premature EOF or a parsing error. This return value is used by 
 *  parseFile to determine if there is more file to parse.
 */
 
static int
parseCharMetrics(PDF *p, FILE *fp, pdf_font *font, char *ident)
{
    int cont = pdf_true, firstTime = pdf_true;
    int error = ok, count = 0;
    register CharMetricInfo *temp = font->cmi;
    register char *keyword;
  
    while (cont) {
        keyword = token(fp, ident);
        if (keyword == NULL) {
            error = earlyEOF;
            break; /* get out of loop */
        }
        switch(recognize(keyword)) {
            case COMMENT:
                (void) linetoken(fp, ident);
                break; 
            case CODE:
                if (count < font->numOfChars) { 
                    if (firstTime)
			firstTime = pdf_false;
                    else
			temp++;
                    temp->code = atoi(token(fp, ident));
                    count++;
                } else {
                    error = parseError;
                    cont = pdf_false;
                }
                break;
            case XYWIDTH:
                temp->wx = atoi(token(fp, ident));
                temp->wy = atoi(token(fp, ident));
                break;                 
            case XWIDTH: 
                temp->wx = atoi(token(fp, ident));
                break;
            case CHARNAME: 
                keyword = token(fp, ident);
                temp->name = pdf_strdup(p, keyword);
                break;            
            case CHARBBOX: 
                temp->charBBox.llx = (float) atoi(token(fp, ident));
                temp->charBBox.lly = (float) atoi(token(fp, ident));
                temp->charBBox.urx = (float) atoi(token(fp, ident));
                temp->charBBox.ury = (float) atoi(token(fp, ident));
                break;
            case LIGATURE: {
                Ligature **tail = &(temp->ligs);
                Ligature *node = *tail;
                
                if (*tail != NULL) {
                    while (node->next != NULL)
                        node = node->next;
                    tail = &(node->next); 
                }
                
                *tail = (Ligature *) p->calloc(p, 
				sizeof(Ligature), "parseCharMetrics");

                keyword = token(fp, ident);
                (*tail)->succ = pdf_strdup(p, keyword);

                keyword = token(fp, ident);
                (*tail)->lig = pdf_strdup(p, keyword);
                break; }
            case ENDCHARMETRICS:
                cont = pdf_false;
                break; 
            case ENDFONTMETRICS: 
                cont = pdf_false;
                error = normalEOF;
                break; 
            case NOPE:
            default:
                error = parseError; 
                break; 
        } /* switch */
    } /* while */
    
    if ((error == ok) && (count != font->numOfChars))
        error = parseError;
    
    return(error);
    
} /* parseCharMetrics */ 


/************************* parseTrackKernData ***********************/

/*  This function is called by "parseFile". It will parse the AFM File 
 *  up to the "EndTrackKern" or "EndKernData" keywords. It will save the
 *  track kerning data if requested by the caller of parseFile.
 *
 *  parseTrackKernData is passed in a pointer to the font record.
 *  If data is to be saved, the font record will already contain 
 *  a valid pointer to storage for the track kerning data.
 *
 *  This function returns an error code specifying whether there was 
 *  a premature EOF or a parsing error. This return value is used by 
 *  parseFile to determine if there is more file to parse.
 */
 
static int
parseTrackKernData(FILE *fp, pdf_font *font, char *ident)
{
    int cont = pdf_true, save = (font->tkd != NULL);
    int pos = 0, error = ok, tcount = 0;
    register char *keyword;
  
    while (cont) {
        keyword = token(fp, ident);
        
        if (keyword == NULL) {
            error = earlyEOF;
            break; /* get out of loop */
        }
        if (!save)
          /* get tokens until the end of the Track Kerning Data */
          /* section without saving any of the data */
            switch(recognize(keyword)) {
                case ENDTRACKKERN:
                case ENDKERNDATA:
                    cont = pdf_false;
                    break;
                case ENDFONTMETRICS:
                    cont = pdf_false;
                    error = normalEOF;
                    break;
                default:
                    break;
            } /* switch */
	else
          /* otherwise parse entire Track Kerning Data section, */
          /* saving the data */
            switch(recognize(keyword)) {
                case COMMENT:
                    (void) linetoken(fp, ident);
                    break;
                case TRACKKERN:
                    if (tcount < font->numOfTracks) {
                        keyword = token(fp, ident);
                        font->tkd[pos].degree = atoi(keyword);
                        keyword = token(fp, ident);
                        font->tkd[pos].minPtSize = (float) atof(keyword);
                        keyword = token(fp, ident);
                        font->tkd[pos].minKernAmt = (float) atof(keyword);
                        keyword = token(fp, ident);
                        font->tkd[pos].maxPtSize = (float) atof(keyword);
                        keyword = token(fp, ident);
                        font->tkd[pos++].maxKernAmt = (float) atof(keyword);
                        tcount++;
                    } else {
                        error = parseError;
                        cont = pdf_false;
                    }
                    break;
                case ENDTRACKKERN:
                case ENDKERNDATA:
                    cont = pdf_false;
                    break;
                case ENDFONTMETRICS:
                    cont = pdf_false;
                    error = normalEOF;
                    break;
                case NOPE:
                default:
                    error = parseError;
                    break;
            } /* switch */
    } /* while */
    
    if (error == ok && tcount != font->numOfTracks)
        error = parseError;
        
    return(error);
    
} /* parseTrackKernData */    


/************************* parsePairKernData ************************/

/*  This function is called by "parseFile". It will parse the AFM File 
 *  up to the "EndKernPairs" or "EndKernData" keywords. It will save
 *  the pair kerning data if requested by the caller of parseFile.
 *
 *  parsePairKernData is passed in a pointer to the font record.
 *  If data is to be saved, the font record will already contain 
 *  a valid pointer to storage for the pair kerning data.
 *
 *  This function returns an error code specifying whether there was 
 *  a premature EOF or a parsing error. This return value is used by 
 *  parseFile to determine if there is more file to parse.
 */
 
static int
parsePairKernData(PDF *p, FILE *fp, pdf_font *font, char *ident)
{  
    int cont = pdf_true, save = (font->pkd != NULL);
    int pos = 0, error = ok, pcount = 0;
    register char *keyword;
  
    while (cont) {
        keyword = token(fp, ident);
        
        if (keyword == NULL) {
            error = earlyEOF;
            break; /* get out of loop */
        }
        if (!save)
          /* get tokens until the end of the Pair Kerning Data */
          /* section without saving any of the data */
            switch(recognize(keyword)) {
                case ENDKERNPAIRS:
                case ENDKERNDATA:
                    cont = pdf_false;
                    break;
                case ENDFONTMETRICS:
                    cont = pdf_false;
                    error = normalEOF;
                    break;
                default:
                    break;
            } /* switch */
	else
          /* otherwise parse entire Pair Kerning Data section, */
          /* saving the data */
            switch(recognize(keyword)) {
                case COMMENT:
                    (void) linetoken(fp, ident);
                    break;
                case KERNPAIR:
                    if (pcount < font->numOfPairs) {
                        keyword = token(fp, ident);
                        font->pkd[pos].name1 = pdf_strdup(p, keyword);
                        keyword = token(fp, ident);
                        font->pkd[pos].name2 = pdf_strdup(p, keyword);
                        keyword = token(fp, ident);
                        font->pkd[pos].xamt = atoi(keyword);
                        keyword = token(fp, ident);
                        font->pkd[pos++].yamt = atoi(keyword);
                        pcount++;
                    } else {
                        error = parseError;
                        cont = pdf_false;
                    }
                    break;
                case KERNPAIRXAMT:
                    if (pcount < font->numOfPairs) {
                        keyword = token(fp, ident);
                        font->pkd[pos].name1 = pdf_strdup(p, keyword);
                        keyword = token(fp, ident);
                        font->pkd[pos].name2 = pdf_strdup(p, keyword);
                        keyword = token(fp, ident);
                        font->pkd[pos++].xamt = atoi(keyword);
                        pcount++;
                    } else {
                        error = parseError;
                        cont = pdf_false;
                    }
                    break;
                case ENDKERNPAIRS:
                case ENDKERNDATA:
                    cont = pdf_false;
                    break;
                case ENDFONTMETRICS:
                    cont = pdf_false;
                    error = normalEOF;
                    break;
                case NOPE:
                default:
                    error = parseError;
                    break;
            } /* switch */
    } /* while */
    
    if (error == ok && pcount != font->numOfPairs)
        error = parseError;
        
    return(error);
    
} /* parsePairKernData */ 


/************************* parseCompCharData **************************/

/*  This function is called by "parseFile". It will parse the AFM File 
 *  up to the "EndComposites" keyword. It will save the composite 
 *  character data if requested by the caller of parseFile.
 *
 *  parseCompCharData is passed in a pointer to the font record, and 
 *  a boolean representing if the data should be saved.
 *
 *  This function will create the appropriate amount of storage for
 *  the composite character data and store a pointer to the storage
 *  in the font record.
 *
 *  This function returns an error code specifying whether there was 
 *  a premature EOF or a parsing error. This return value is used by 
 *  parseFile to determine if there is more file to parse.
 */
 
static int
parseCompCharData(PDF *p, FILE *fp, pdf_font *font, char *ident)
{  
    int cont = pdf_true, firstTime = pdf_true, save = (font->ccd != NULL);
    int pos = 0, j = 0, error = ok, ccount = 0, pcount = 0;
    register char *keyword;
  
    while (cont) {
        keyword = token(fp, ident);
        if (keyword == NULL)
          /* Have reached an early and unexpected EOF. */
          /* Set flag and stop parsing */
        {
            error = earlyEOF;
            break; /* get out of loop */
        }
        if (ccount > font->numOfComps) {
            error = parseError;
            break; /* get out of loop */
        }
        if (!save)
          /* get tokens until the end of the Composite Character info */
          /* section without saving any of the data */
            switch(recognize(keyword)) {
                case ENDCOMPOSITES:
                    cont = pdf_false;
                    break;
                case ENDFONTMETRICS:
                    cont = pdf_false;
                    error = normalEOF;
                    break;
                default:
                    break;
            } /* switch */
	else
          /* otherwise parse entire Composite Character info section, */
          /* saving the data */
            switch(recognize(keyword)) {
                case COMMENT:
                    (void) linetoken(fp, ident);
                    break;
                case COMPCHAR:
                    if (ccount < font->numOfComps) {
                        keyword = token(fp, ident);
                        if (pcount != font->ccd[pos].numOfPieces)
                            error = parseError;
                        pcount = 0;
                        if (firstTime)
			    firstTime = pdf_false;
                        else
			    pos++;
                        font->ccd[pos].ccName = pdf_strdup(p, keyword);

                        keyword = token(fp, ident);
                        font->ccd[pos].numOfPieces = atoi(keyword);
                        font->ccd[pos].pieces = (Pcc *) p->calloc(p, 
				    font->ccd[pos].numOfPieces * sizeof(Pcc),
				    "parseCompCharData");
                        j = 0;
                        ccount++;
                    } else {
                        error = parseError;
                        cont = pdf_false;
                    }
                    break;
                case COMPCHARPIECE:
                    if (pcount < font->ccd[pos].numOfPieces) {
                        keyword = token(fp, ident);
                        font->ccd[pos].pieces[j].pccName = 
				pdf_strdup(p, keyword);
                        keyword = token(fp, ident);
                        font->ccd[pos].pieces[j].deltax = atoi(keyword);

                        keyword = token(fp, ident);
                        font->ccd[pos].pieces[j++].deltay = atoi(keyword);
                        pcount++;
                    }
                    else
                        error = parseError;
                    break;
                case ENDCOMPOSITES:
                    cont = pdf_false;
                    break;
                case ENDFONTMETRICS:
                    cont = pdf_false;
                    error = normalEOF;
                    break;
                case NOPE:
                default:
                    error = parseError;
                    break;
            } /* switch */
    } /* while */
    
    if (error == ok && ccount != font->numOfComps)
        error = parseError;
    
    return(error);
    
} /* parseCompCharData */    


/*************************** pdf_parse_afm *****************************/
/*
 *  Storage for the font structure (but not the structure itself) will
 *  be allocated in parseFile and the structure will be filled in
 *  with the requested data from the AFM File.
 */
static int
pdf_parse_afm (PDF *p, FILE *fp, pdf_font *font)
{
    /*  flags is a mask with bits set representing what data should be saved. */
    int flags =  P_G | P_M;
    
    int code; 		/* return code from each of the parsing routines */
    char ident[MAX_NAME];
    
    register char *keyword; /* used to store a token */	 
    
    pdf_init_font_struct(p, font);
    font->afm = pdf_true;

    code = parseGlobals(p, fp, font, ident); 
    

    /* The Global Font Information is followed by the Character Metrics */
    /* section. Which procedure is used to parse this section depends on */
    /* how much information should be saved. If all of the metrics info */
    /* is wanted, parseCharMetrics is called. If only the character widths */
    /* is wanted, parseCharWidths is called. parseCharWidths will also */
    /* be called in the case that no character data is to be saved, just */
    /* to parse through the section. */
  
    if ((code != normalEOF) && (code != earlyEOF)) {
        font->numOfChars = atoi(token(fp, ident));
	font->cmi = (CharMetricInfo *) p->calloc(p,
	    font->numOfChars * sizeof(CharMetricInfo), "pdf_parse_afm");
	code = parseCharMetrics(p, fp, font, ident);
    }
    
    /* The remaining sections of the AFM are optional. This code will */
    /* look at the next keyword in the file to determine what section */
    /* is next, and then allocate the appropriate amount of storage */
    /* for the data (if the data is to be saved) and call the */
    /* appropriate parsing routine to parse the section. */
    
    while ((code != normalEOF) && (code != earlyEOF)) {
        keyword = token(fp, ident);
        if (keyword == NULL)
          /* Have reached an early and unexpected EOF. */
          /* Set flag and stop parsing */
        {
            code = earlyEOF;
            break; /* get out of loop */
        }
        switch(recognize(keyword)) {
            case STARTKERNDATA:
                break;
            case ENDKERNDATA:
                break;
            case STARTTRACKKERN:
                keyword = token(fp, ident);
                if (flags & P_T) {
                    font->numOfTracks = atoi(keyword);
                    font->tkd = (TrackKernData *) p->calloc(p, 
				    font->numOfTracks * sizeof(TrackKernData),
				    "pdf_parse_afm");
                } /* if */
                code = parseTrackKernData(fp, font, ident);
                break;
            case STARTKERNPAIRS:
                keyword = token(fp, ident);
                if (flags & P_P) {
                    font->numOfPairs = atoi(keyword);
                    font->pkd = (PairKernData *) p->calloc(p,
				    font->numOfPairs * sizeof(PairKernData),
				    "pdf_parse_afm");
                } /* if */
                code = parsePairKernData(p, fp, font, ident);
                break;
            case STARTCOMPOSITES:
                keyword = token(fp, ident);
                if (flags & P_C) { 
                    font->numOfComps = atoi(keyword);
                    font->ccd = (CompCharData *) p->calloc(p, 
				    font->numOfComps * sizeof(CompCharData),
				    "pdf_parse_afm");
                } /* if */
                code = parseCompCharData(p, fp, font, ident);
                break;    
            case ENDFONTMETRICS:
                code = normalEOF;
                break;
            case NOPE:
            default:
                code = parseError;
                break;
        } /* switch */
    } /* while */
  
    /* fix some entries */
    if (code >= 0) {
	/* 
	 * If we don't know the exact stem width we use default values
	 * according to the font weight (regular, semi, or bold).
	 */

	if (font->StdVW == 0) {
	    if (font->weight != NULL && !strcmp(font->weight, "Semibold"))
		font->StdVW = DEFAULT_STEMWIDTH_SEMI;
	    else if (font->weight != NULL && !strcmp(font->weight, "Bold"))
		font->StdVW = DEFAULT_STEMWIDTH_BOLD;
	    else
		font->StdVW = DEFAULT_STEMWIDTH;
	}

#define DEFAULT_ENCODING "FontSpecific"
	/* Deal with buggy AFMs without an encoding entry */
	if (font->encodingScheme == NULL)
	    font->encodingScheme = pdf_strdup(p, DEFAULT_ENCODING);
    }

    return(code < 0 ? pdf_false : pdf_true);
  
} /* pdf_parse_afm */

void
pdf_cleanup_font_struct(PDF *p, pdf_font *font)
{
    if (font == NULL)
	return;

    p->free(p, font->name);

    if (font->fullName)
	p->free(p, font->fullName);

    if (font->encodingScheme)
	p->free(p, font->encodingScheme);

    if (font->familyName)
	p->free(p, font->familyName);
    
    if (font->weight)
	p->free(p, font->weight);

    if (font->afm) {
	if (font->cmi != NULL) { 
	    int i;
	    CharMetricInfo *temp = font->cmi;
	    Ligature *node, *node1;
	    
	    for (i = 0; i < font->numOfChars; ++i) {
		for (node = temp->ligs; node != NULL; /* */) {
		    p->free(p, node->succ);
		    p->free(p, node->lig);
		    node1 = node->next;
		    p->free(p, node);
		    node = node1;
		}
		p->free(p, temp->name);
		temp++;
	    }
	    
	    p->free(p, font->cmi);
	}

	if (font->tkd != NULL)
	    p->free(p, font->tkd);

	if (font->pkd != NULL) { 
	    p->free(p, font->pkd->name1);
	    p->free(p, font->pkd->name2);
	    p->free(p, font->pkd);
	}

	if (font->ccd != NULL) {
	    int i, j;
	    CompCharData *ccd = font->ccd;
	    
	    for (i = 0; i < font->numOfComps; ++i) {
		for (j = 0; j < ccd[i].numOfPieces; ++j)
		    p->free(p, ccd[i].pieces[j].pccName); 

		p->free(p, ccd[i].ccName);
	    }
	    p->free(p, font->ccd);
	}
    }
}

pdf_bool
pdf_get_metrics_afm(PDF *p, pdf_font *font, const char *fontname, int enc, const char *filename)
{
    FILE	*afmfile;
    int		*widths;
    int		i, j;
    const char	*charname;
    CharMetricInfo *cmi;

    /* open AFM file */
    if ((afmfile = fopen(filename, "r")) == NULL) {
	return pdf_false;
    }

    /* parse AFM file */
    if (pdf_parse_afm(p, afmfile, font) == pdf_false) {
	fclose(afmfile);
	pdf_error(p, PDF_RuntimeError, "Error parsing AFM file '%s'",
		filename);
    }

    fclose(afmfile);

    /* The AFM exists but is bad. */
    if (font->cmi == NULL) {
	pdf_error(p, PDF_RuntimeError, 
		"Couldn't parse character metrics in AFM file '%s'", filename);
    }

    /*
     * The user may have changed the AFM in order to have sort of a
     * font aliasing mechanism. Since this will only rarely happen
     * deliberately we issue a warning message.
     */
    if (fontname && strcmp(font->name, fontname)) {
	pdf_error(p, PDF_NonfatalError,
		"Font name mismatch in AFM file '%s'", filename);
    }

    /* 
     * Generate character width array according to the chosen encoding vector
     * or the font's default encoding vector.
     */
    if (enc != builtin && 
    	strcmp(font->encodingScheme, "AdobeStandardEncoding") &&
    	strcmp(font->encodingScheme, "StandardEncoding")) {
	pdf_error(p, PDF_NonfatalError,
		"Can't reencode Symbol font '%s' (using builtin)", font->name);
	enc = builtin;
    }

    font->encoding = enc;

    widths = font->widths;		/* shortcut */

    if (enc != builtin && p->encodings[enc]) {	/* text font */
	for (i = 0; i < 256; i++) {
	    charname = p->encodings[enc]->chars[i];
	    widths[i] = 250;		/* some reasonable default */
	    if (charname == NULL)	/* unencoded character */
		continue;
	    for (j = 0, cmi = font->cmi; j < font->numOfChars; ++j, ++cmi) {
		if (!strcmp(cmi->name, charname)) {
		    widths[i] = cmi->wx;
		    break;
		}
	    }
	}
    } else {					/* symbol or pi font */
	for (i = 0; i < 256; i++)
		widths[i] = 250;	/* some reasonable default */
	for (i = 0, cmi = font->cmi; i < font->numOfChars; i++, cmi++) {
	    if (cmi->code >= 0 && cmi->code < 256)
		widths[cmi->code] = cmi->wx;
	}
    }

    pdf_make_fontflags(p, font);

    return pdf_true;
}
