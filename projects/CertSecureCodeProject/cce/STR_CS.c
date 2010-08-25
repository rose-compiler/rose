/*
 * Copyright (c) 2007 Carnegie Mellon University.
 * All rights reserved.

 * Permission to use this software and its documentation for any purpose is hereby granted,
 * provided that the above copyright notice appear and that both that copyright notice and
 * this permission notice appear in supporting documentation, and that the name of CMU not
 * be used in advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.
 *
 * CMU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL CMU BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, RISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "util.h"

void STR03();
void STR04();
void STR05();
void STR06();
void STR30();
void STR31();
void STR32();
void STR33();
void STR34();
void STR35();
void STR36();
void STR37();

void STR() {
  STR03();
  STR04();
  STR05();
  STR06();
  STR30();
  STR31();
  STR32();
  STR33();
  STR34();
  STR35();
  STR36();
  STR37();
}


/* STR03_A v.47 */

void STR03() {
  char *string_data = NULL;
  char a[16];

  if (string_data == NULL) {
    /* Handle null pointer error */
  }
  else if (strlen(string_data) >= sizeof(a)) {
    /* Handle overlong string error */
  }
  else {
    strcpy(a, string_data);
  }
  printf("STR03 %p\n", string_data);
}


/* STR04_A v.21 */

void STR04() {
  size_t len;
  char cstr[] = "char string";

  len = strlen(cstr);
  printf("STR04 %d\n", len);
}


/* STR05_A v.65 */

void STR05() {
  char const *c = "Hello";
  printf("STR05 %s\n", c);
}


/* STR06_A v.54 */

void STR06() {
  char *token;
  const char *path = getenv("PATH");
  
  char *copy = (char *)malloc(strlen(path) + 1);
  if (copy == NULL) {
    /* handle error */
  }
  strcpy(copy, path);
  token = strtok(copy, ":");
  
  while (token = strtok(0, ":")) {
    /* something */
    printf("STR06 %p\n", token);
  }

  free(copy);
}


/* STR30_C v.37 */

void STR30() {
  char p[] = "string literal";
  p[0] = 'S';
  printf("STR30 %s\n", p);
}


/* STR31_C v.79 */

void STR31() {
  const size_t ARRAY_SIZE = 8;

  char dest[ARRAY_SIZE];
  char src[ARRAY_SIZE];
  size_t i;

  memset(src, '\0', sizeof(src));

  for (i=0; src[i] && (i < sizeof(dest) - 1); i++) {
    dest[i] = src[i];
  }
  dest[i] = '\0';
  printf("STR31 %p %p\n", dest, src);
}


/* STR32_C v.56 */

void STR32() {
  char ntbs[12];
  char source[12];
  
  strncpy(ntbs, source, sizeof(ntbs));
  ntbs[sizeof(ntbs) - 1] = '\0';
}


/* STR33_C v.41 */

void STR33() {
  wchar_t wide_str1[] = L"0123456789";
  wchar_t *wide_str2 
    = (wchar_t *)malloc((wcslen(wide_str1) + 1) * sizeof(wchar_t));
  if (wide_str2 == NULL) {
    /* Handle Error */
  }
  else
    free(wide_str2);
  wide_str2 = NULL; 
}


/* STR34_C v.16 */

void STR34() {
  register char *string;
  register int c;

  string = "string";
  c = EOF;

  /* If the string doesn't exist, or is empty, EOF found. */
  if (string && *string) {
    c = (unsigned char) *string++;
  }
  printf("STR34 %d\n", c);
}


/* STR35_C v.79 */

void STR35() {
  char buf[BUFSIZ];
  int ch;
  char *p;

  if (fgets(buf, sizeof(buf), stdin)) {
    /* fgets succeeds, scan for newline character */
    p = strchr(buf, '\n');
    if (p) {
      *p = '\0';
    }
    else {
      /* newline not found, flush stdin to end of line */
      while (((ch = getchar()) != '\n') 
             && !feof(stdin) 
	     && !ferror(stdin) 
	     );
    }
  }
  else {
    /* fgets failed, handle error */
  }
  printf("STR35 %d\n", ch);
}

/* STR36_C v.25 */

void STR36() {
  char s[] = "abc";
  printf("STR36 %s\n", s);
}


/* STR37_C v.37 */

size_t STR37_count_whitespace(
  char const *s, 
  size_t length) 
{
  char const *t = s;
  while (isspace((unsigned char)*t) && (t - s < length))
    ++t;
  return t - s;
}

void STR37() {
  if (STR37_count_whitespace("sdf sdf", 6) == 0) {
   /* Handle Error */
  }
}


