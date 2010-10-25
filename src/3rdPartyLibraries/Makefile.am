# This should not depend on ROSE in any way so I don't think we need this
# include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# SUBDIRS = MSTL libharu-2.1.0 fortran-parser java-parser antlr-jars qrose
# checkPointLibrary

SUBDIRS =

if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
   SUBDIRS += fortran-parser
endif

if ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
   SUBDIRS += java-parser
endif

SUBDIRS += MSTL libharu-2.1.0 antlr-jars qrose

EXTRA_DIST = CMakeLists.txt
