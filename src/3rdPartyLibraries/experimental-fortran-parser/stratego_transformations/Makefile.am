include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# all : ofp-simplify
# all : ofp-simplify ofp2fast fast2pp fast2sage
# all : fast2sage
all : fast2pp

ofp-simplify : ofp-simplify.str
#	strc -i ofp-simplify.str $(CFLAGS) $(LDFLAGS) --main io-ofp-simplify
#	strc -i $(srcdir)/ofp-simplify.str -I $(srcdir)/../sdf_syntax -la stratego-lib --main io-ofp-simplify
	strc -i $(srcdir)/ofp-simplify.str -I $(srcdir)/../sdf_syntax -la stratego-lib --main io-ofp-simplify -o ofp-simplify

no-to-none: ofp-simplify
	strc -i $(srcdir)/no-to-none.str -I $(srcdir)/../sdf_syntax -la stratego-lib -o no-to-none

ofp2fast: no-to-none
	strc -i $(srcdir)/ofp2fast.str -I $(srcdir)/../sdf_syntax -la stratego-lib -o ofp2fast

fast2pp: ofp2fast
	strc -i $(srcdir)/fast2pp.str -I $(srcdir)/../sdf_syntax -la stratego-lib -o fast2pp

# fast2sage: fast2pp
#	strc -i $(srcdir)/fast2sage.str -I $(srcdir)/../sdf_syntax -la stratego-lib -o fast2sage

clean :
	rm -f fast2pp no-to-none ofp-simplify ofp2fast
	rm -f fast2pp.c no-to-none.c ofp-simplify.c ofp2fast.c
	rm -f *.o *.lo *.dep 
	rm -rf .libs

