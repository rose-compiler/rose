/* MDH WCET BENCHMARK SUITE. File version $Id: loop3.c,v 1.1 2005/11/11 10:17:03 ael01 Exp $ */


/* Changes:
 * JG 2005/12/12: Indented program. Removed argument to main.
 */
 
#define PLOOP(A,B,I) do                \
{                                      \
  for (i = (A); i <  (B); i += (I))    \
    s += i;                            \
                                       \
  for (i = (A); i <= (B); i += (I))    \
    s += i;                            \
                                       \
  for (i = (A); i <  (B); i -= (-(I))) \
    s += i;                            \
                                       \
  for (i = (A); i <= (B); i -= (-(I))) \
    s += i;                            \
} while (0)

#define MLOOP(A,B,D) do                \
{                                      \
  for (i = (A); i >  (B); i -= (D))    \
    s += i;                            \
                                       \
  for (i = (A); i >= (B); i -= (D))    \
    s += i;                            \
                                       \
  for (i = (A); i >  (B); i += (-(D))) \
    s += i;                            \
                                       \
  for (i = (A); i >= (B); i += (-(D))) \
    s += i;                            \
} while (0)


int 
main(int argc)
{
	int             s = 0;
	int             i;
	argc = 2;


	PLOOP(0, 3, 1);
	PLOOP(1, 2, 1);
	PLOOP(-3, -2, 1);
	PLOOP(-4, -1, 1);
	PLOOP(-5, 0, 1);
	PLOOP(-6, 4, 1);

	PLOOP(0, 3, argc);
	PLOOP(1, 2, argc);
	PLOOP(-3, -2, argc);
	PLOOP(-4, -1, argc);
	PLOOP(-5, 0, argc);
	PLOOP(-6, 4, argc);

	MLOOP(5, 2, 1);
	MLOOP(6, 1, 1);
	MLOOP(7, 0, 1);
	MLOOP(8, -1, 1);
	MLOOP(9, -2, 1);
	MLOOP(0, -3, 1);
	MLOOP(-1, -5, 1);
	MLOOP(-2, -7, 1);
	MLOOP(16, -8, 1);

	MLOOP(5, 2, argc);
	MLOOP(6, 1, argc);
	MLOOP(7, 0, argc);
	MLOOP(8, -1, argc);
	MLOOP(9, -2, argc);
	MLOOP(0, -3, argc);
	MLOOP(-1, -5, argc);
	MLOOP(-2, -7, argc);
	MLOOP(16, -8, argc);

	return s;
}
