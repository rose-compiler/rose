# This should not depend on ROSE in any way so I don't think we need this
# include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# SUBDIRS = MSTL libharu-2.1.0 fortran-parser java-parser antlr-jars qrose
# checkPointLibrary

SUBDIRS =

if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
   SUBDIRS += fortran-parser
endif

if ROSE_EXPERIMENTAL_OFP_ROSE_CONNECTION
   SUBDIRS += experimental-fortran-parser
endif

if ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
   SUBDIRS += java-parser
endif

if ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
SUBDIRS += MSTL
else
SUBDIRS += MSTL libharu-2.1.0 antlr-jars qrose POET
endif

# TV (08/03/12): Need to finish integration before
SUBDIRS += UPR

pocc-install:	pocc-installer.sh
	$(srcdir)/pocc-installer.sh

-include pocc-install
EXTRA_DIST = CMakeLists.txt pocc-installer.sh
