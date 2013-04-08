## Process this file with automake to create Makefile.in.
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

BUILDDIR=$(top_builddir)/projects/minitermite

ACLOCAL_AMFLAGS = -I m4

.PHONY: clang docs

clang: $(top_srcdir)/src/minitermite/clang/clang-termite.patch $(top_builddir)/clang/build/Makefile
	$(MAKE) -C $(top_builddir)/clang/build
	$(MAKE) install -C $(top_builddir)/clang/build

$(top_builddir)/clang/build/Makefile:
	cd $(top_builddir)/src/termite/clang && sh BUILD-clang.sh

EXTRA_DIST= README COPYING HOWTO_ADD_NEW_SGNODE_VARIANTS

bin_SCRIPTS=
include src/make_rule.inc
include regression/make_rule.inc

# ############################################
# Documentation
# ############################################

docs:
	@echo "  GEN [doxygen documentation]"
	@(doxygen && $(MAKE) MAKEFLAGS=-j1 -C doc/latex)

doc/html/index.html:
	$(MAKE) doc

#info_TEXINFOS = doc/src2term.texi doc/term2src.texi
dist_man_MANS = $(srcdir)/doc/src2term.1 $(srcdir)/doc/term2src.1

# We are distributing the compiled manpage so we don't depend on rst2man
# Insert the output of xxx --help into the MAN page.
EXTRA_DIST += doc/man_header.txt
$(srcdir)/doc/%.1: $(top_builddir)/src/% doc/man_header.txt README Makefile
	$(AM_V_GEN)
	@cat $(srcdir)/doc/man_header.txt |sed -e 's/@DATE@/$(shell date +%Y-%m-%d)/g' \
                     -e 's/\@VERSION\@/$(PACKAGE_VERSION)/g' >$<.txt
	@$< --help \
	 | sed -e 's|Usage: [^ ]*|Usage: $(patsubst $(srcdir)/doc/%.1, %, $@)|' \
	       -e 's/Usage: *\(.*\)/\nSYNOPSIS\n========\n\n\1\n\nDESCRIPTION\n===========\n\n/' \
               -e 's/pos/OPTIONS\n=======\n\npos/' >>$<.txt
	@echo >>$<.txt
	@tail -n +3 $(srcdir)/README >>$<.txt
	@if test -w $@; then \
             if which rst2man >/dev/null; then \
                true "We need to trick rst2man to recognize long options as options, too"; \
		sed 's/^  --/  -, --/g' $<.txt >$<.tmp; \
		rst2man $<.tmp $@.tmp; \
		sed 's/\\-, \\-/\\-/g' $@.tmp >$@; \
                rm -f $<.tmp $@.tmp; \
             else echo "**WARNING rst2man not installed, did not update $@"; \
             fi \
         else echo "**WARNING $@ is not writable, did not update. (This is OK during distcheck)"; \
         fi
	@rm -f $<.txt

clean-local:
	rm -rf doc/html
	rm -rf doc/latex
	rm -rf doc/man
	rm -rf src/.libs # bug in automake 1.9
	rm -f regression/*-[1357].term regression/*-[2468].[cf]* regression/*_postprocessed.f
