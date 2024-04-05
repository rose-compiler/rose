include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = SageIII

# Note that CXX and C are either both defined or both undefined.
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
   SUBDIRS += CxxFrontend
endif

# Rasmussen (01/21/2018): Adding general support for experimental languages
if ROSE_BUILD_ATERM_SUPPORT
   SUBDIRS += Experimental_General_Language_Support
endif

if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
   SUBDIRS += OpenFortranParser_SAGE_Connection
endif

# Added support for using flang/F18 for parsing for Fortran [Rasmussen 08/14/2019]
if ROSE_EXPERIMENTAL_FLANG_ROSE_CONNECTION
if ROSE_BUILD_ATERM_SUPPORT_FALSE
#  Don't add twice
   SUBDIRS += Experimental_General_Language_Support
endif
   SUBDIRS += Experimental_Flang_ROSE_Connection
endif

# Removed --enable-experimental_fortran_frontend option, replaced by flang frontend [Rasmussen 9/14/2022]
#

if ROSE_EXPERIMENTAL_CSHARP_ROSE_CONNECTION
   SUBDIRS += Experimental_Csharp_ROSE_Connection
endif

if ROSE_EXPERIMENTAL_ADA_ROSE_CONNECTION
   SUBDIRS += Experimental_Ada_ROSE_Connection
endif

if ROSE_EXPERIMENTAL_LIBADALANG_ROSE_CONNECTION
   SUBDIRS += Experimental_Libadalang_ROSE_Connection
endif

if ROSE_EXPERIMENTAL_JOVIAL_ROSE_CONNECTION
   SUBDIRS += Experimental_Jovial_ROSE_Connection
endif

if ROSE_EXPERIMENTAL_MATLAB_ROSE_CONNECTION
   SUBDIRS += Experimental_Matlab_ROSE_Connection
endif


SUBDIRS += ECJ_ROSE_Connection

SUBDIRS += Disassemblers BinaryFormats

if ROSE_BUILD_PHP_LANGUAGE_SUPPORT
   SUBDIRS += PHPFrontend
endif

if ROSE_BUILD_PYTHON_LANGUAGE_SUPPORT
   SUBDIRS += PythonFrontend
endif

# Until we revisit this this, we want to include specific documentation that has been put into this directory.
# Note that OpenFortranParser_SAGE_Connection has the newer work to support Fortran in ROSE.  This work
# is part of a contract to add Fortran support to ROSE using the Open 64 compiler.  It supports a roughly
# Fortran 77 level of details, however it uses IR nodes that have been extended or removed (in a few cases)
# as part of adding the Fortran 2003 support using the Open Fortan Parser.
EXTRA_DIST = CMakeLists.txt frontend.docs


