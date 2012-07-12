#include <stdlib.h>
#include <stdio.h>

typedef enum {
  UNKNOWN, DOS_BINARY, DOS_TEXT, UNIX_TEXT
} File_type;


/* This is the function we analyze. */
File_type
guess_type (char *buf, size_t buflen)
{
  int crlf_seen = 0;
  char *bp = buf;

  while (buflen--)
    {
      /* Treat a file as binary if it has a NUL character.  */
      if (!*bp)
        return DOS_BINARY;

      /* CR before LF means DOS text file (unless we later see
         binary characters).  */
      else if (*bp == '\r' && buflen && bp[1] == '\n')
        crlf_seen = 1;

      bp++;
    }

  return crlf_seen ? DOS_TEXT : UNIX_TEXT;
}

/* The rest of this file is optional for the purposes of analysis, but we make some calls to guess_type() */
void
show_result(char *buf, size_t n)
{
    File_type ft = guess_type(buf, n);
    printf("guess_type(\"%s\", %d) = %d", buf, n, (int)ft);
    switch (ft) {
        case UNKNOWN:
            fputs(" (UNKNOWN)\n", stdout);
            break;
        case DOS_BINARY:
            fputs(" (DOS_BINARY)\n", stdout);
            break;
        case DOS_TEXT:
            fputs(" (DOS_TEXT)\n", stdout);
            break;
        case UNIX_TEXT:
            fputs(" (UNIX_TEXT)\n", stdout);
            break;
        default:
            fputc('\n', stdout);
            break;
    }
}

int
main()
{
    show_result("xx", 2);
    return 0;
}
