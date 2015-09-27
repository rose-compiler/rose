include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# Automake conditional to control use of ROSE with Maple Symbolic Algebra Package
if ROSE_USE_BOOST_WAVE

AM_CPPFLAGS = $(ROSE_INCLUDES) $(MAPLE_AM_CPPFLAGS)

bin_PROGRAMS = nameChecker

# DQ (1/15/2007): Use the correct boost library with suffix.
REGEX_LIBS = -lboost_regex-@BOOST_LIB_SUFFIX@

# DQ (3/14/2009): Don't overwrite existing values in LDFLAGS.
# LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) $(WAVE_LDFLAGS) $(REGEX_LIBS)
LDADD += $(LIBS_WITH_RPATH) $(ROSE_LIBS) $(WAVE_LDFLAGS) $(REGEX_LIBS)

nameChecker_SOURCES = nameChecker.C checkNameImpl.C
nameChecker_DEPENDENCIES = $(ROSE_LIBS_WITH_PATH)

test: nameChecker
	./nameChecker

test1:
	./nameChecker -rose:name:file $(srcdir)/example-styles/rose-style.sn -c $(top_builddir)/src/frontend/SageIII/Cxx_Grammar.C -o grammar_test.o $(ROSE_INCLUDES) -I$(top_builddir)

test2:
	./nameChecker -rose:name:file $(srcdir)/example-styles/ale3d-style.sn -c $(srcdir)/example-styles/test-Ale3D.C -o test-Ale3D.o

test3:
	./nameChecker -rose:name:file $(srcdir)/example-styles/possibly-dot-com-style.sn -c $(srcdir)/example-styles/test-possibly-dot-com-style.C -o test-possibly-dot-com-style.o

# check-local: test1 test2 test3 
check-local: test2 test3 

endif

EXTRA_DIST = checkNameImpl.h example-styles

clean-local:
	rm -f a.out rose_*.C

