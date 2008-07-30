include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_MYSQL_DATABASE
INCLUDES = $(ROSE_INCLUDES) -idirafter @MySQL_path@/include -idirafter @MySQL_path@/include/mysql -idirafter /home/thuerey1/local/include

# noinst_HEADERS = sideEffect.h
include_HEADERS = sideEffect.h

noinst_LTLIBRARIES = libsideEffect.la

## Specifications for building the pdf library.
libsideEffect_la_SOURCES = sideEffect.C

# libannotationLanguageParser_la_DEPENDENCES = language-lexer.cc language-parser.cc language.tab.h
# libannotationLanguageParser_la_LIBADD = 
endif

clean:
	rm -f a.out


