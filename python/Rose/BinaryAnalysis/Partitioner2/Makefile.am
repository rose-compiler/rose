include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
AM_CPPFLAGS = $(ROSE_INCLUDES)

if ROSE_ENABLE_PYTHON_API

pythondir = $(prefix)/python/Rose/BinaryAnalysis/Partitioner2

python_LTLIBRARIES = lib_Partitioner2.la
lib_Partitioner2_la_SOURCES = _Partitioner2.C

install-data-hook:
	cd $(pythondir) && ln -sf lib_Partitioner2.so _Partitioner2.so

python_DATA = __init__.py

endif

