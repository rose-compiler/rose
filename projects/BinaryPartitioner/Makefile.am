include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
noinst_PROGRAMS =
EXTRA_DIST =

noinst_PROGRAMS += testPartitioner
testPartitioner_SOURCES = testPartitioner.C InstructionProvider.C Partitioner2.C
testPartitioner_CPPFLAGS = $(ROSE_INCLUDES)
testPartitioner_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
