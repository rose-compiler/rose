// DQ (1/5/2014): test2014_16.c has a simpler example of this bug.

#define xstrtou(rest) xstrtoull##rest
#define xstrto(rest) xstrtoll##rest
#define xatou(rest) xatoull##rest
#define xato(rest) xatoll##rest

unsigned long xstrtou(_range_sfx)(const char *numstr, int base,
		unsigned long lower,
		unsigned long upper,
		const struct suffix_mult *suffixes)
{
}
