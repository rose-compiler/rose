/* Compare S1 and S2, returning less than, equal to or
   greater than zero if S1 is lexicographically less than,
   equal to or greater than S2.  */

#include <stdio.h>
#define reg_char char

int
glibc_strcmp (const char *p1, const char *p2)
{
  register const unsigned char *s1 = (const unsigned char *) p1;
  register const unsigned char *s2 = (const unsigned char *) p2;
  unsigned reg_char c1, c2;

  do
    {
      c1 = (unsigned char) *s1++;
      c2 = (unsigned char) *s2++;
      if (c1 == '\0')
	return c1 - c2;
    }
  while (c1 == c2);

  return c1 - c2;
}

int
main()
{
#if 1 /* we don't actually need to ever clal strcmp */
    const char *s1 = "xx";
    const char *s2 = "xy";
    int cmp = glibc_strcmp(s1, s2);
    printf("strcmp(\"%s\", \"%s\") = %d\n", s1, s2, cmp);
#endif
    return 0;
}
