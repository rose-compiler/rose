include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

all : ofp-simplify

ofp-simplify : ofp-simplify.str
#	strc -i ofp-simplify.str $(CFLAGS) $(LDFLAGS) --main io-ofp-simplify
#	strc -i $(srcdir)/ofp-simplify.str -I $(srcdir)/../sdf_syntax -la stratego-lib --main io-ofp-simplify
	strc -i $(srcdir)/ofp-simplify.str -I $(srcdir)/../sdf_syntax -la stratego-lib --main io-ofp-simplify -o ofp-simplify

clean :
	rm -f ofp-simplify ofp-simplify.c
	rm -f *.o *.lo *.dep
	rm -rf .libs

