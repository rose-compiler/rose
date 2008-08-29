include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_OPENGL

INCLUDES = $(ROSE_INCLUDES) -I$(builddir) $(GL_CFLAGS)

bin_PROGRAMS = vizzBinary 

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) $(GL_LIBS)

vizzBinary_SOURCES = vizzBinary.C

check-local: vizzBinary 

else

bin_PROGRAMS =

endif

#EXTRA_DIST = buffer2.bin

#SUBDIRS = analyses graphanalyses

