#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main (int argc, char* argv[])
{
  int i;
  size_t n = 0;
  char* concat_buf = 0;

  if (argc < 2)
    {
      printf ("usage: %s <str1> [<str2> ...]\n", argv[0]);
      return 1;
    }

  /* Determine how much space we need. */
  printf ("Determine how much space we need \n");
  for (i = 1; i < argc; ++i)
    {
      printf ("[%d] '%s'\n", i, argv[i]);
      n += strlen (argv[i]);
    }

  printf ("Allocate a large buffer for all strings on command line, to be separated by a vertical bar. \n");

  /* Allocate a large buffer for all strings on command line, to be
   * separated by a vertical bar.
   */
  printf ("Allocating a buffer of size %d...\n", sizeof (char) * (n + argc - 1));
  concat_buf = (char *)malloc (sizeof (char) * (n + argc));
  if (!concat_buf)
    {
      printf ("*** Out of memory allocating buffer for all strings! ***\n");
      return 1;
    }

  printf ("Calling strcpy() \n");

  strcpy (concat_buf, argv[1]);
  for (i = 2; i < argc; ++i)
    {
      strcat (concat_buf, "|");
      strcat (concat_buf, argv[i]);
    }

  printf ("Final buffer has %d characters, excluding terminating NULL:\n"
	  "\n"
	  "  [%s]\n"
	  "\n",
	  strlen (concat_buf), concat_buf);
  return 0;
}

// eof
