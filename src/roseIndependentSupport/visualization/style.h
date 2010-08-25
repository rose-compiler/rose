
// Syntax highlighting stuff...
Fl_Text_Buffer     *glob_textbuf = 0;
Fl_Text_Buffer     *glob_stylebuf = 0;
Fl_Text_Display::Style_Table_Entry
                   styletable[] = {	// Style table
		     { FL_BLACK,      FL_COURIER,        14 }, // A - Plain
		     { FL_DARK_GREEN, FL_COURIER_ITALIC, 14 }, // B - Line comments
		     { FL_DARK_GREEN, FL_COURIER_ITALIC, 14 }, // C - Block comments
		     { FL_BLUE,       FL_COURIER,        14 }, // D - Strings
		     { FL_DARK_RED,   FL_COURIER,        14 }, // E - Directives
		     { FL_DARK_RED,   FL_COURIER_BOLD,   14 }, // F - Types
		     { FL_BLUE,       FL_COURIER_BOLD,   14 }  // G - Keywords
		   };
const char         *code_keywords[] = {	// List of known C/C++ keywords...
                     "and",
		     "and_eq",
		     "asm",
		     "bitand",
		     "bitor",
		     "break",
		     "case",
		     "catch",
		     "compl",
		     "continue",
		     "default",
		     "delete",
		     "do",
		     "else",
		     "false",
		     "for",
		     "goto",
		     "if",
		     "new",
		     "not",
		     "not_eq",
		     "operator",
		     "or",
		     "or_eq",
		     "return",
		     "switch",
		     "template",
		     "this",
		     "throw",
		     "true",
		     "try",
		     "while",
		     "xor",
		     "xor_eq"
		   };
const char         *code_types[] = {	// List of known C/C++ types...
                     "auto",
		     "bool",
		     "char",
		     "class",
		     "const",
		     "const_cast",
		     "double",
		     "dynamic_cast",
		     "enum",
		     "explicit",
		     "extern",
		     "float",
		     "friend",
		     "inline",
		     "int",
		     "long",
		     "mutable",
		     "namespace",
		     "private",
		     "protected",
		     "public",
		     "register",
		     "short",
		     "signed",
		     "sizeof",
		     "static",
		     "static_cast",
		     "struct",
		     "template",
		     "typedef",
		     "typename",
		     "union",
		     "unsigned",
		     "virtual",
		     "void",
		     "volatile"
		   };


//-----------------------------------------------------------------------------
//
// 'compare_keywords()' - Compare two keywords...
//
int
compare_keywords(const void *a,
                 const void *b) {
  return (strcmp(*((const char **)a), *((const char **)b)));
}



//-----------------------------------------------------------------------------
//
// 'style_parse()' - Parse text and produce style data.
//
void
style_parse(const char *text,
            char       *style,
	    int        length) {
  char	     current;
  int	     col;
  int	     last;
  char	     buf[255],
             *bufptr;
  const char *temp;

  for (current = *style, col = 0, last = 0; length > 0; length --, text ++) {
    if (current == 'B') current = 'A';
    if (current == 'A') {
      // Check for directives, comments, strings, and keywords...
      if (col == 0 && *text == '#') {
        // Set style to directive
        current = 'E';
      } else if (strncmp(text, "//", 2) == 0) {
        current = 'B';
	for (; length > 0 && *text != '\n'; length --, text ++) *style++ = 'B';

        if (length == 0) break;
      } else if (strncmp(text, "/*", 2) == 0) {
        current = 'C';
      } else if (strncmp(text, "\\\"", 2) == 0) {
        // Quoted quote...
	*style++ = current;
	*style++ = current;
	text ++;
	length --;
	col += 2;
	continue;
      } else if (*text == '\"') {
        current = 'D';
      } else if (!last && islower(*text)) {
        // Might be a keyword...
	for (temp = text, bufptr = buf;
             islower(*temp) && bufptr < (buf + sizeof(buf) - 1);
             *bufptr++ = *temp++);

        if (!islower(*temp)) {
	  *bufptr = '\0';

          bufptr = buf;

	  if (bsearch(&bufptr, code_types,
	              sizeof(code_types) / sizeof(code_types[0]),
		      sizeof(code_types[0]), compare_keywords)) {
	    while (text < temp) {
	      *style++ = 'F';
	      text ++;
	      length --;
	      col ++;
	    }

	    text --;
	    length ++;
	    last = 1;
	    continue;
	  } else if (bsearch(&bufptr, code_keywords,
                             sizeof(code_keywords) / sizeof(code_keywords[0]),
		             sizeof(code_keywords[0]), compare_keywords)) {
            while (text < temp) {
	      *style++ = 'G';
	      text ++;
	      length --;
	      col ++;
	    }

	    text --;
	    length ++;
	    last = 1;
	    continue;
	  }
	}
      }
    } else if (current == 'C' && strncmp(text, "*/", 2) == 0) {
      // Close a C comment...
      *style++ = current;
      *style++ = current;
      text ++;
      length --;
      current = 'A';
      col += 2;
      continue;
    } else if (current == 'D') {
      // Continuing in string...
      if (strncmp(text, "\\\"", 2) == 0) {
        // Quoted end quote...
	*style++ = current;
	*style++ = current;
	text ++;
	length --;
	col += 2;
	continue;
      } else if (*text == '\"') {
        // End quote...
	*style++ = current;
	col ++;
	current = 'A';
	continue;
      }
    }

    // Copy style info...
    if (current == 'A' && (*text == '{' || *text == '}')) *style++ = 'G';
    else *style++ = current;
    col ++;

    last = isalnum(*text) || *text == '.';

    if (*text == '\n') {
      // Reset column and possibly reset the style
      col = 0;
      if (current == 'B' || current == 'E') current = 'A';
    }
  }
}


//-----------------------------------------------------------------------------
//
// 'style_init()' - Initialize the style buffer...
//
void style_init( Fl_Text_Buffer* buf) {
	glob_textbuf = buf;
  char *style = new char[glob_textbuf->length() + 1];
  char *text = glob_textbuf->text();
  

  memset(style, 'A', glob_textbuf->length());
  style[glob_textbuf->length()] = '\0';

  if (!glob_stylebuf) glob_stylebuf = new Fl_Text_Buffer(glob_textbuf->length());

  style_parse(text, style, glob_textbuf->length());

  glob_stylebuf->text(style);
  delete[] style;
  free(text);
}


//-----------------------------------------------------------------------------
//
// 'style_unfinished_cb()' - Update unfinished styles.
//
void style_unfinished_cb(int, void*) {
}


//-----------------------------------------------------------------------------
//
// 'style_update()' - Update the style buffer...
//
void
style_update(int        pos,		// I - Position of update
             int        nInserted,	// I - Number of inserted chars
	     int        nDeleted,	// I - Number of deleted chars
             int        /*nRestyled*/,	// I - Number of restyled chars
	     const char * /*deletedText*/,// I - Text that was deleted
             void       *cbArg) {	// I - Callback data
  int	start,				// Start of text
	end;				// End of text
  char	last,				// Last style on line
	*style,				// Style data
	*text;				// Text data


  // If this is just a selection change, just unselect the style buffer...
  if (nInserted == 0 && nDeleted == 0) {
    glob_stylebuf->unselect();
    return;
  }

  // Track changes in the text buffer...
  if (nInserted > 0) {
    // Insert characters into the style buffer...
    style = new char[nInserted + 1];
    memset(style, 'A', nInserted);
    style[nInserted] = '\0';

    glob_stylebuf->replace(pos, pos + nDeleted, style);
    delete[] style;
  } else {
    // Just delete characters in the style buffer...
    glob_stylebuf->remove(pos, pos + nDeleted);
  }

  // Select the area that was just updated to avoid unnecessary
  // callbacks...
  glob_stylebuf->select(pos, pos + nInserted - nDeleted);

  // Re-parse the changed region; we do this by parsing from the
  // beginning of the line of the changed region to the end of
  // the line of the changed region...  Then we check the last
  // style character and keep updating if we have a multi-line
  // comment character...
  start = glob_textbuf->line_start(pos);
  end   = glob_textbuf->line_end(pos + nInserted);
  text  = glob_textbuf->text_range(start, end);
  style = glob_stylebuf->text_range(start, end);
  last  = style[end - start - 1];

//  printf("start = %d, end = %d, text = \"%s\", style = \"%s\"...\n",
//         start, end, text, style);

  style_parse(text, style, end - start);

//  printf("new style = \"%s\"...\n", style);

  glob_stylebuf->replace(start, end, style);
  ((Fl_Text_Editor *)cbArg)->redisplay_range(start, end);

  if (last != style[end - start - 1]) {
    // The last character on the line changed styles, so reparse the
    // remainder of the buffer...
    free(text);
    free(style);

    end   = glob_textbuf->length();
    text  = glob_textbuf->text_range(start, end);
    style = glob_stylebuf->text_range(start, end);

    style_parse(text, style, end - start);

    glob_stylebuf->replace(start, end, style);
    ((Fl_Text_Editor *)cbArg)->redisplay_range(start, end);
  }

  free(text);
  free(style);
}

