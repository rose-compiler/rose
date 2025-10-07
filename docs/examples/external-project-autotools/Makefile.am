# Programs to build
bin_PROGRAMS = simple_analyzer

# Simple ROSE analyzer
simple_analyzer_SOURCES = simple_analyzer.cpp
simple_analyzer_CXXFLAGS = $(ROSE_CFLAGS) $(AM_CXXFLAGS)
simple_analyzer_LDADD = $(ROSE_LIBS)

# Clean generated files
CLEANFILES = *~

# Extra files to distribute
EXTRA_DIST = \
    README.md \
    test_input.c \
    autogen.sh
