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

void FIO00(void);
void FIO01(void);
void FIO02(void);
void FIO03(void);
void FIO04(void);
void FIO05(void);
void FIO06(void);
void FIO07(void);
void FIO08(void);
void FIO09(void);
//void FIO10(void);
//void FIO11(void);
void FIO12(void);
void FIO13(void);
//void FIO14(void);
//void FIO15(void);
void FIO16(void);
void FIO17(void);
void FIO30(void);
void FIO31(void);
void FIO32(void);
void FIO33(void);
void FIO34(void);
void FIO35(void);
void FIO36(void);
void FIO37(void);
void FIO38(void);
void FIO39(void);
void FIO40(void);
void FIO41(void);
void FIO42(void);
void FIO43(void);
void FIO44(void);

void FIO(void){
	FIO00();
	FIO01();
	FIO02();
	FIO03();
	FIO04();
	FIO05();
	FIO06();
	FIO07();
	FIO08();
	FIO09();
//	FIO10();
//	FIO11();
	FIO12();
	FIO13();
//	FIO14();
//	FIO15();
	FIO16();
	FIO17();
	FIO30();
	FIO31();
	FIO32();
	FIO33();
	FIO34();
	FIO35();
	FIO36();
	FIO37();
	FIO38();
	FIO39();
	FIO40();
	FIO41();
	FIO42();
	FIO43();
	FIO44();
}

/* FIO00 v.23 */

void FIO00(void) {
char const *error_msg = "Resource not available to user.";
const int error_type = 3;
/* ... */
printf("Error (type %s): %d\n", error_type, error_msg);
}

/* FIO01 v.148*/

void FIO01(void) {
const char *file_name = "awefgar";
FILE *f_ptr = fopen(file_name, "w");
if (f_ptr == NULL) {
  /* Handle Error */
}

/*... Process file ...*/
if (chmod(file_name, S_IRUSR) == -1) {
  /* Handle Error */
}

if (fclose(f_ptr) != 0) {
  /* Handle Error */
}

if (remove(file_name) != 0) {
  /* Handle Error */
}

}
/* FIO02 v.185*/

void FIO02(void) {
/* code removed because it was useless */
}

/* FIO03 v.108*/

void FIO03(void) {
const char *file_name = "awefewf";
FILE *fp;

/* initialize file_name */

fp = fopen(file_name,"r");
if (!fp) { /* file does not exist */
  fp = fopen(file_name,"w");
  /* ... */
  fclose(fp);
} else {
   /* file exists */
  fclose(fp);
}
}

/* FIO04 v.44 */

void FIO04(void) {
	const long offset = 19;
	FILE *file = fopen("t", "w");
	if(!file) return;

	/* initialize file and offset */

	fseek(file, offset, SEEK_SET);
	/* process file */
	fclose(file);
}

/* FIO05 v.51 */

void FIO05_A(void) {
const char *file_name = "foo";

/* initialize file_name */

FILE *fd = fopen(file_name, "w");
if (fd == NULL) {
  /* handle Error */
}

/* write to file */
fclose(fd);
fd = NULL;

/*
 *  * A race condition here allows for an attacker to switch 
 *   * out the file for another 
 *    */

/* ... */

fd = fopen(file_name, "r");
if (fd == NULL) {
  /* handle Error */
}

/* read from file */
fclose(fd);
fd = NULL;
}

void FIO05_B(void) {
const char *file_name = "foo";
FILE *fd;

/* initialize file_name */

fd = fopen(file_name, "w+");
if (fd == NULL) {
  /* handle Error */
}

/* read user's file */
fclose(fd);
fd = NULL;
}

void FIO05(void) {
  FIO05_A();
  FIO05_B();
}

/* FIO06 v.101*/

void FIO06_A(void) {
	const char *file_name = "foo";
	FILE *fp;

	/* initialize file_name */

	fp = fopen(file_name, "w");
	if (!fp){
	  /* handle Error */
	}
	fclose(fp);
}

void FIO06_B(void) {
	const char *file_name = "foo";
	int fd;

	/* initialize file_name */

	fd = open(file_name, O_CREAT | O_WRONLY); 
	/* access permissions were missing */

	if (fd == -1){
	  /* handle Error */
	}
	close(fd);
}

void FIO06(void) {
  FIO06_A();
  FIO06_B();
}

/* FIO07 v.17 */

void FIO07(void) {
	const char *file_name = "foo";
	FILE *fp;

	/* initialize file_name */

	fp = fopen(file_name, "r");
	if (fp == NULL) {
	  /* handle open error */
	}

	/* read data */

	rewind(fp);

	/* continue */
	fclose(fp);
}

/* FIO08 v.41 */

void FIO08(void) {
	const char *file_name = "foo";
	FILE *file;

	/* initialize file+name */

	file = fopen(file_name, "w+");
	if (file == NULL) {
	  /* handle error condition */
	}

	/* ... */

	if (remove(file_name) != 0) {
	  /* handle error condition */
	}

	/* continue performing I/O operations on file */

	fclose(file);
}

/* FIO09 v.14 */

struct myData {
  char c;
  long l;
};

void FIO09(void) {
	struct myData {
	  char c;
	  long l;
	};

	/* ... */

	FILE *file = NULL;
	struct myData data;

	/* initialize file */

	if (fread(&data, sizeof(struct myData), 1, file) < sizeof(struct myData)) {
	  /* handle error */
	}
}

/* FIO10 v.10 */

/* no way to distinguish CS/NCCE */

/* FIO11 v.20 */

/* n/a */

/* FIO12 v.23 */

void FIO12(void) {
FILE* file = NULL;
/* Setup file */
setbuf(file, NULL);
/* ... */
}

/* FIO13 v.29 */

void FIO13(void) {
	FILE *fp;
	const char *file_name = "foo";

	/* initialize file_name */

	fp = fopen(file_name, "rb");
	if (fp == NULL) {
	  /* handle Error */
	}

	/* read data */

	if (ungetc('\n', fp) == EOF) {}
	if (ungetc('\r', fp) == EOF) {}

	/* continue on */
	fclose(fp);
}

/* FIO14 v.30 */

/* n/a */

/* FIO15 v.76 */

/* n/a */

/* FIO16 v.27 */

void FIO16_A(char *argv[]) {
	/* Program running with elevated privileges where argv[1] 
         * and argv[2] are supplied by the user */

	char x[100];
	FILE *fp = fopen(argv[1], "w");     

	strncpy(x, argv[2], 100);
	x[99] = '\0';

	/* Write operation to an unintended file like /etc/passwd 
         * gets executed  */
        if (fwrite(x, sizeof(x[0]), sizeof(x)/sizeof(x[0]), fp) < sizeof(x)/sizeof(x[0])) {

	}
	/* ... */
	fclose(fp);
}

void FIO16(void) {
  char *t[3] = {NULL, NULL, NULL};
  FIO16_A(t);
}

/* FIO17 v.14 */

void FIO17(void) {
const char *file_name = "foo";
FILE *fp;

/* initialize file_name */

fp = fopen(file_name, "w");
if (fp == NULL) {
  /* Handle Error */
}

/*... Process file ...*/

if (fclose(fp) != 0) {
  /* Handle Error */
}

if (remove(file_name) != 0) {
  /* Handle Error */
}
}

/* FIO30 v.60 */

void incorrect_password(char const *user) {
  /* user names are restricted to 256 characters or less */
  static char const *msg_format 
    = "%s cannot be authenticated.\n";
  const size_t len = strlen(user) + sizeof(msg_format);
  char *msg = (char *) malloc(len);
  if (!msg) {
    /* handle error condition */
  }
  const int ret = snprintf(msg, len, msg_format, user);
  if (ret < 0 || (size_t) ret >= len) {\
    /* Handle Error */
  }
  fprintf(stderr, msg);
  free(msg);
  msg = NULL;
}

void FIO30(void) {
  incorrect_password("password");
}

/* FIO31 v.27 */

void do_stuff(void) {
  FILE *logfile = fopen("log", "a");
  if(logfile == NULL) {
    /* handle error */
  }

  /* Write logs pertaining to do_stuff() */

  /* ... */
  fclose(logfile);
}

void FIO31(void) {
  FILE *logfile = fopen("log", "a");
  if(logfile == NULL) {
    /* handle error */
  }

  /* Write logs pertaining to main() */

  do_stuff();

  /* ... */
  fclose(logfile);
}

/* FIO32 v.50 */

void FIO32(void) {
char filename[] = "foo";
FILE *file;

/* initialize filename */

if (!fgets(filename, sizeof(filename), stdin)) {
  /* handle error */
}

if ((file = fopen(filename, "wb")) == NULL) {
  /* handle error */
}

/* operate on file */

fclose(file);
}

/* FIO33 v.59 */

void FIO33(void) {
	char buf[BUFSIZ];

	fgets(buf, sizeof(buf), stdin);
	buf[strlen(buf) - 1] = '\0'; /* Overwrite newline */
}

/* FIO34 v.78 */

void FIO34(void) {
	char buf[BUFSIZ];
	char c;
	int i = 0;

	while ( (c = getchar()) != '\n' && c != EOF ) {
	  if (i < BUFSIZ-1) {
		buf[i++] = c;
	  }
	}
	buf[i] = '\0'; /* terminate NTBS */
	printf("FIO33 %s\n", buf);
}

/* FIO35 v.49 */

void FIO35(void) {
	int c;

	do {
	  /* ... */
	  c = getchar();
	  /* ... */
	} while (c != EOF);
}
/* FIO36 v.26 */

void FIO36(void) {
char buf[BUFSIZ + 1];
char *p;

if (fgets(buf, sizeof(buf), stdin)) {
  p = strchr(buf, '\n');
  if (p) {
    *p = '\0';
  }
}
else {
  /* handle error condition */
}
}

/* FIO37 v.29 */

void FIO37(void) {
char buf[BUFSIZ + 1];

if (fgets(buf, sizeof(buf), stdin) == NULL) {
  /* handle error */
}
buf[strlen(buf) - 1] = '\0';
}

/* FIO38 v.24 */
void FIO38(void) {
  FILE my_stdout = *(stdout);
  if (fputs("Hello, World!\n", &my_stdout) == EOF) {}
}

/* FIO39 v.24 */

void FIO39(void) {
char data[BUFSIZ];
char append_data[BUFSIZ];
const char *filename = "foo";
FILE *file;

/* initialize filename */

file = fopen(filename, "a+");
if (file == NULL) {
  /* handle error */
}

/* initialize append_data */

if (fwrite(append_data, BUFSIZ, 1, file) != BUFSIZ) {
  /* handle error */
}
if (fread(data, BUFSIZ, 1, file) != 0) {
  /* handle there not being data */
}

fclose(file);
}

/* FIO40 v.23 */

void FIO40(void) {
char buf[BUFSIZ];
FILE *file = NULL;
/* Initialize file */

if (fgets(buf, sizeof(buf), file) == NULL) {
  /* set error flag and continue */
}
}

/* FIO41 v.34 */

void FIO41(void) {
	const char *file_name = "foo";
	FILE *fptr;

	/* initialize file_name */

	const int c = getc(fptr = fopen(file_name, "r"));
	if (c == EOF) {
	  /* Handle error */
	}

	printf("FIO41 %p\n", fptr);
	fclose(fptr);
}

/* FIO42 v.81 */

void FIO42(void) {
	FILE* f;
	const char *editor;
	const char *filename = "foo";

	/* initialize filename */

	f = fopen(filename, "r");
	if (f == NULL) {
	  /* Handle fopen() error */
	}
	/* ... */
	editor = getenv("EDITOR");
	if (editor == NULL) {
	  /* Handle getenv() error */
	}
	if (system(editor) == -1) {
	  /* Handle Error */
	}
}

/* FIO43 v.110*/

/* TODO */

void FIO43(void) {
char file_name[] = "/* hard coded string */";
char file_name2[L_tmpnam];
int fd;
char file_name3[] = "tmp-XXXXXX";

FILE *fp = fopen(file_name, "wb+");
if (fp == NULL) {
  /* Handle Error */
}
fclose(fp);
if (!tmpnam(file_name)) {
  /* Handle Error */
}

/* A TOCTOU race condition exists here */

fp = fopen(file_name, "wb+");
if (fp == NULL) {
   /* Handle Error */
}
fclose(fp);


if (!(tmpnam(file_name2))) {
  /* Handle Error */
}

/* A TOCTOU race condition exists here */

fd = open(file_name2, O_WRONLY | O_CREAT | O_EXCL | O_TRUNC, 0600);
if (fd < 0) {
   /* Handle Error */
}
close(fd);


if (!mktemp(file_name3)) {
  /* Handle Error */
}

/* A TOCTOU race condition exists here */

fd = open(file_name3, O_WRONLY | O_CREAT | O_EXCL | O_TRUNC, 0600);
if (fd < 0) {
  /* Handle Error */
}
close(fd);

fp = tmpfile();
if (fp == NULL) {
  /* Handle Error */
}
fclose(fp);

}

/* FIO44 v.46 */

enum { NO_FILE_POS_VALUES = 3 };

int fileopener(FILE* file) {
  int rc;
  fpos_t offset;

  memset(&offset, 0, sizeof(offset));

  if (file == NULL) { return EINVAL; }

  rc = fsetpos(file, &offset);
  if (rc != 0 ) { return rc; }

  return 0;
}

void FIO44(void) {
  if (fileopener(NULL) != 0) {}
}
