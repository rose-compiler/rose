# This should not depend on ROSE in any way so I don't think we need this
# include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS =

# MS (1/25/21): include json-related code
SUBDIRS += json

if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
   SUBDIRS += fortran-parser
endif

if ROSE_EXPERIMENTAL_FLANG_ROSE_CONNECTION
   SUBDIRS += flang-parser
endif

if ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
   SUBDIRS += java-parser
endif

if ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
SUBDIRS += MSTL
else
SUBDIRS += MSTL antlr-jars
endif

# DQ (1/14/2017): Remove POET uniformally until it can be added to provide greater portability (working on Ubuntu systems).
# # DQ (12/18/2016): Disable use of POET on DEBIAN and UBUNTU systems because
# # its built system is not sufficently general to avoid linking issues.
# # SUBDIRS += POET
# if !OS_VENDOR_UBUNTU
# if !OS_VENDOR_DEBIAN
#    SUBDIRS += POET
# endif
# endif
# Pei-Hung (06/28/2017): add POET support
if ROSE_WITH_POET
   SUBDIRS += POET
endif

# Pei-Hung (12/17/2014): installing POCC
if ROSE_WITH_POCC
pocc-install:	pocc-installer.sh
	$(srcdir)/pocc-installer.sh
-include pocc-install
endif

EXTRA_DIST = CMakeLists.txt pocc-installer.sh
