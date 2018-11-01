include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
AM_CPPFLAGS = $(ROSE_INCLUDES)

if ROSE_ENABLE_PYTHON_API

pythondir = $(prefix)/python/Rose/BinaryAnalysis/Unparser

python_LTLIBRARIES = lib_Unparser.la
lib_Unparser_la_SOURCES = _Unparser.C

install-data-hook:
	cd $(pythondir) && ln -sf lib_Unparser.so _Unparser.so

python_DATA = __init__.py

endif

