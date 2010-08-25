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
#include <libgen.h>

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
void FIO15(void);
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
//      FIO10();
//      FIO11();
        FIO12();
        FIO13();
//      FIO14();
        FIO15();
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
        printf("Error (type %d): %s\n", error_type, error_msg);
}

/* FIO01 v.148*/

void FIO01(void) {
        const char *file_name = "Awefawef";
        int fd;

        /* initialize file_name */

        fd = open(file_name, O_WRONLY | O_CREAT | O_EXCL, S_IRWXU);
        if (fd == -1) {
          /* Handle Error */
        }

        /* ... */

        if (fchmod(fd, S_IRUSR) == -1) {
          /* Handle Error */
        }

        close(fd);
}

/* FIO02 v.185*/

void FIO02(void) {
        char *realpath_res = NULL;
        char t[30];

        /* Verify argv[1] is supplied */

        realpath_res = realpath(t, NULL);
        if (realpath_res == NULL) {
          /* handle realpath() error */
        }

        /* Verify file name */

        if (fopen(realpath_res, "w") == NULL) {
                /* Handle Error */
        }

        /* ... */

        free(realpath_res);
        realpath_res = NULL;
}

/* FIO03 v.108*/

void FIO03(void) {
        const char *file_name = "Awefawef";
        const int new_file_mode = 0600;

        /* initialize file_name and new_file_mode */

        const int fd = open(file_name, O_CREAT | O_EXCL | O_WRONLY, new_file_mode);
        if (fd == -1) {
          /* Handle Error */
        }

        /* ... */
        close(fd);
}

/* FIO04 v.44 */
void FIO04(void) {
        const long offset = 19;
        FILE *file = fopen("t", "w");
        if(!file) return;

        /* initialize file and offset */

        if (fseek(file, offset, SEEK_SET) != 0) {
          /* Handle Error */
        }

        /* ... */
        fclose(file);
}

/* FIO05 v.51 */
void FIO05_A(void) {
struct stat orig_st;
struct stat new_st;
const char *file_name = "foo";

/* initialize file_name */

int fd = open(file_name, O_WRONLY);
if (fd == -1) {
  /* handle Error */
}

/* write to file */

if (fstat(fd, &orig_st) == -1) {
  /* handle error */
}
close(fd);
fd = -1;

/* ... */

fd = open(file_name, O_RDONLY);
if (fd == -1) {
  /* handle error */
}

if (fstat(fd, &new_st) == -1) {
  /* handle error */
}

if ((orig_st.st_dev != new_st.st_dev) ||
    (orig_st.st_ino != new_st.st_ino)) {
  /* file was tampered with! */
}

/* read from file */
close(fd);
fd = -1;
}

void FIO05_B(void) {
struct stat st;
const char *file_name = "foo";

/* initialize file_name */

int fd = open(file_name, O_RDONLY);
if (fd == -1) {
  /* Handle Error */
}

if ((fstat(fd, &st) == -1) ||
   (st.st_uid != getuid()) ||
   (st.st_gid != getgid())) {
  /* file does not belong to user */
}

/*... read from file ...*/

close(fd);
fd = -1;
}

void FIO05(void) {
  FIO05_A();
  FIO05_B();
}

/* FIO06 v.101*/

void FIO06(void) {
        const char *file_name = "foo";
        const int file_access_permissions = 0600;

        /* initialize file_name */

        const int fd = open( file_name, O_CREAT | O_WRONLY, file_access_permissions);
        if (fd == -1){
          /* handle Error */
        }

        /* ... */
        close(fd);
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

        if (fseek(fp, 0L, SEEK_SET) != 0) {
          /* handle repositioning error */
        }

        /* continue */
        fclose(fp);
}

/* FIO08 v.41 */

void FIO08(void) {
FILE *file;
const char *file_name = "foo";

/* initialize file_name */

file = fopen(file_name, "w+");
if (file == NULL) {
  /* handle error condition */
}

if (unlink(file_name) != 0) {
  /* handle error condition */
}

/*... continue performing I/O operations on file ...*/

fclose(file);
}

/* FIO09 v.14 */

void FIO09(void) {
        struct myData {
          char c;
          long l;
        };

        FILE *file = NULL;
        struct myData data;
        char buf[25];
        char *end_ptr;

        /* initialize file */

        if (fgets(buf, 1, file) == NULL) {
          /* Handle Error */
        }

        data.c = buf[0];

        if (fgets(buf, sizeof(buf), file) == NULL) {
          /* Handle Error */
        }

        data.l = strtol(buf, &end_ptr, 10);

        if ((ERANGE == errno)
         || (end_ptr == buf)
         || ('\n' != *end_ptr && '\0' != *end_ptr)) {
                /* Handle Error */
        }

        printf("%c %ld\n", data.c, data.l);
}

/* FIO10 v.10 */

/* no way to distinguish CS/NCCE */

/* FIO11 v.20 */

/* n/a */

/* FIO12 v.23 */

void FIO12(void) {
        FILE *file = NULL;
        char *buf = NULL;
        /* Setup file */
        if (setvbuf(file, buf, buf ? _IOFBF : _IONBF, BUFSIZ) != 0) {
          /* Handle error */
        }
        /* ... */
}

/* FIO13 v.29 */

void FIO13(void) {
        FILE *fp;
        fpos_t pos;
        const char *file_name = "foo";

        /* initialize file_name */

        fp = fopen(file_name, "rb");
        if (fp == NULL) {
          /* handle Error */
        }

        /* read data */

        if (fgetpos(fp, &pos)) {
          /* handle Error */
        }

        /* read the data that will be "pushed back" */

        if (fsetpos(fp, &pos)) {
          /* handle Error */
        }

        /* Continue on */
        fclose(fp);
}

/* FIO14 v.30 */

/* n/a */

/* FIO15 v.76 */

void FIO15(void) {
        const char *dir_name = "dirfoo";

        /* initialize dir_name */

        if (mkdir(dir_name, S_IRWXU) == -1) {
          /* Handle Error */
        }
}

/* FIO16 v.27 */

void FIO16_A(char *argv[]) {
        if (setuid(0) == -1) {
          /* Handle Error */
        }

        if (chroot("chroot/jail") == -1) {
          /* Handle Error */
        }

        if (chdir("/") == -1) {
          /* Handle Error */
        }

        /* Drop privileges permanently */
        if (setgid(getgid()) == -1) {
          /* Handle Error */
        }

        if (setuid(getuid()) == -1) {
          /* Handle Error */
        }

        /* Perform unprivileged operations */

        FILE *fp = fopen(argv[1], "w");
        char x[100];
        strncpy(x, argv[2], 100);
        x[sizeof(x) - 1] = '\0';

        /* Write operation safe is safe within jail */
        if (fwrite(x, sizeof(x[0]), sizeof(x)/sizeof(x[0]), fp) < sizeof(x)/sizeof(x[0])) {
                /* Handle Error */
        }
        fclose(fp);
}

void FIO16(void) {
  char *t[3] = {NULL, NULL, NULL};
  FIO16_A(t);
}

/* FIO17 v.14 */

int secure_dir(const char* path) {
  char *realpath_res = realpath(path, NULL);
  char *path_copy = NULL;
  char *dirname_res = NULL;
  char ** dirs = NULL;
  int num_of_dirs = 0;
  int secure = 1;
  int i;
  struct stat buf;
  const uid_t my_uid = geteuid();

  if (realpath_res == NULL) {
    /* Handle Error */
  }

  if (!(path_copy = strdup(realpath_res))) {
    /* Handle Error */
  }

  dirname_res = path_copy;
  /* Figure out how far it is to the root */
  while (1) {
    dirname_res = dirname(dirname_res);

    num_of_dirs++;

    if ((strcmp(dirname_res, ".") == 0) ||
        (strcmp(dirname_res, "/") == 0)) {
      break;
    }
  }
  free(path_copy);
  path_copy = NULL;

  /* Now allocate and fill the dirs array */
  if (!(dirs = (char **)malloc(num_of_dirs*sizeof(*dirs)))) {
    /* Handle Error */
  }
  if (!(dirs[num_of_dirs - 1] = strdup(realpath_res))) {
    /* Handle Error */
  }

  if(!(path_copy = strdup(realpath_res))) {
    /* Handle Error */
  }

  dirname_res = path_copy;
  for (i = 1; i < num_of_dirs; i++) {
    dirname_res = dirname(dirname_res);

    dirs[num_of_dirs - i - 1] = strdup(dirname_res);

  }
  free(path_copy);
  path_copy = NULL;

  /* Traverse from the root to the top, checking
   * permissions along the way */
  for (i = 0; i < num_of_dirs; i++) {
    if (stat(dirs[i], &buf) != 0) {
       /* Handle Error */
    }
    if ((buf.st_uid != my_uid) && (buf.st_uid != 0)) {
      /* Directory is owned by someone besides user or root */
      secure = 0;
    } else if (!(buf.st_mode & S_ISVTX) &&
               (buf.st_mode & (S_IWGRP | S_IWOTH))) {
      /* Others have permission to rename or remove files here */
      secure = 0;
    }
    free(dirs[i]);
    dirs[i] = NULL;
  }

  free(dirs);
  dirs = NULL;

  return secure;
}

void FIO17(void) {
        const char *file_name = "foo";
        FILE *fp;

        /* initialize file_name */

        if (!secure_dir(file_name)) {
          /* Handle Error */
        }

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
  if (fputs(msg, stderr) == EOF) {
    /* Handle Error */
  }
  free(msg);
  msg = NULL;
}

void FIO30(void) {
  incorrect_password("password");
}

/* FIO31 v.27 */

void do_stuff(const FILE *logfile) {
  /* Write logs pertaining to do_stuff() */

  /* ... */
}

void FIO31(void) {
  FILE *logfile = fopen("log", "a");
  if(logfile == NULL) {
    /* handle error */
  }

  /* Write logs pertaining to main() */

  do_stuff(logfile);

  /* ... */
  fclose(logfile);
}

/* FIO32 v.50 */

#ifdef O_NOFOLLOW
  #define OPEN_FLAGS O_NOFOLLOW | O_NONBLOCK
#else
  #define OPEN_FLAGS O_NONBLOCK
#endif

/* ... */
void FIO32(void) {
        struct stat orig_st;
        struct stat open_st;
        int fd;
        int flags;
        char filename[] = "foo";

        /* initialize filename */

        if (!fgets(filename, sizeof(filename), stdin)) {
          /* handle error */
        }

        if ((lstat(filename, &orig_st) != 0) 
         || (!S_ISREG(orig_st.st_mode))) 
        {
          /* handle error */
        }

        /* A TOCTOU race condition exists here, see below */

        fd = open(filename, OPEN_FLAGS | O_WRONLY);
        if (fd == -1) {
          /* handle error */
        }

        if (fstat(fd, &open_st) != 0) {
          /* handle error */
        }

        if ((orig_st.st_mode != open_st.st_mode) ||
                (orig_st.st_ino  != open_st.st_ino) ||
                (orig_st.st_dev  != open_st.st_dev)) {
          /* file was tampered with */
        }

        /* Optional: drop the O_NONBLOCK now that we are sure 
         *  * this is a good file */
        if ((flags = fcntl(fd, F_GETFL)) == -1) {
          /* handle error */
        }

        if (fcntl(fd, F_SETFL, flags & ~O_NONBLOCK) != 0) {
          /* handle error */
        }

        /* operate on file */

        close(fd);
}

/* operate on file */

/* FIO33 v.59 */

void FIO33(void) {
  char buf[BUFSIZ];
  char *p;

  if (fgets(buf, sizeof(buf), stdin)) {
    /* fgets succeeds, scan for newline character */
    p = strchr(buf, '\n');
    if (p) {
      *p = '\0';
    }
    else {
      /* newline not found, flush stdin to end of line */
      while ((getchar() != '\n') 
           && !feof(stdin) 
           && !ferror(stdin) 
      );
    }
  }
  else {
    /* fgets failed, handle error */
  }
}

/* FIO34 v.78 */

void FIO34(void) {
        char buf[BUFSIZ];
        int c;
        int i = 0;

        while ( ((c = getchar()) != '\n') 
                   && !feof(stdin) 
                   && !ferror(stdin)) 
        {
          if (i < BUFSIZ-1) {
                buf[i++] = c;
          }
        }
        buf[i] = '\0'; /* terminate NTBS */
        printf("FIO34 %s\n", buf);
}

/* FIO35 v.49 */
#define FIOJOIN(x, y) FIOJOIN_AGAIN(x, y)
#define FIOJOIN_AGAIN(x, y) x ## y
#define FIO_static_assert(e) \
  typedef char FIOJOIN(assertion_failed_at_line_, _LINE_) \
  [(e) ? 1 : -1]

void FIO35(void) {
  int c;

  FIO_static_assert(sizeof(char) < sizeof(int));
  /* EOF may not be distinguishable from a character */
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

/* FIO38 v.24 */

void FIO38(void) {
  FILE *my_stdout = stdout;
  if (fputs("Hello, World!\n", my_stdout) == EOF) {
    /* Handle Error */
  }
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
          /* Handle error */
        }

        if (fseek(file, 0L, SEEK_SET) != 0) {
          /* Handle Error */
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
          *buf = '\0';
        }
}

/* FIO41 v.34 */

void FIO41(void) {
        int c;
        const char *filename = "foo";
        FILE *fptr;

        /* initialize filename */

        fptr = fopen(filename, "r");
        if (fptr == NULL) {
          /* Handle Error */
        }

        c = getc(fptr);
        if (c == EOF) {
          /* Handle Error */
        }
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
        fclose(f);
        f = NULL;
        editor = getenv("EDITOR");
        if (editor == NULL) {
          /* Handle getenv() error */
        }
        /* Sanitize environment before calling system()! */
        if (system(editor) == -1) {
                /* Handle Error */
        }
}

/* FIO43 v.110*/

void FIO43(void) {
  char sfn[] = "temp-XXXXXX";
  FILE *sfp;
  const int fd = mkstemp(sfn);
  if (fd == -1) {
    /* Handle Error */
  }

  /* 
   * Unlink immediately to allow the name to be recycled.
   * The race condition here is inconsequential if the file
   * is created with exclusive permissions (glibc >= 2.0.7) 
   */

  if (unlink(sfn) == -1) {
    /* Handle Error */
  }

  sfp = fdopen(fd, "w+");
  if (sfp == NULL) {
    close(fd);
    /* Handle Error */
  }

  /* use temporary file */

  fclose(sfp); /* also closes fd */
}

/* FIO44 v.46 */

enum { NO_FILE_POS_VALUES = 3 };

int opener(FILE* file)  {
  int rc;
  fpos_t offset;

  if (file == NULL) { return EINVAL; }

  rc = fgetpos(file, &offset);
  if (rc != 0 ) { return rc; }

  rc = fsetpos(file, &offset);
  if (rc != 0 ) { return rc; }

  return 0;
}

void FIO44(void) {
        if (opener(NULL) != 0) {
                /* Handle Error */
        }
}
