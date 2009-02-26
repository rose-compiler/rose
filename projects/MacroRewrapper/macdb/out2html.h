#ifndef _OUT_2_HTML_H_
#define _OUT_2_HTML_H_

#include <cstdio>
#include <regex.h>
#include <string>

typedef enum {
	ENUM_PATTERN_FILENAME,
	ENUM_PATTERN_LINE_BEGIN,
	ENUM_PATTERN_LINE_END,
	ENUM_PATTERN_COL_BEGIN,
	ENUM_PATTERN_COL_END,
	ENUM_PATTERN_NOT_USED
} file_pattern_enum_t;

typedef struct macro_location_tag_ {
	std::string filename;
	int lineno, colno;
} macro_location_t;

regex_t file_patterns[ENUM_PATTERN_NOT_USED];

bool init_file_patterns();
bool file_pattern_parse(macro_location_t & info, char * line, regex_t patterns[], int dim=ENUM_PATTERN_NOT_USED);


#endif

