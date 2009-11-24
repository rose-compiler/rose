#SET($ENV{BOOST_INCLUDE}
#    /home/saebjornsen1/REPOSITORY-BUILD/gcc-4.1.1/boost_1_35/include/boost-1_35/)
#SET($ENV{BOOST_LIBRARYDIR}
#    /home/saebjornsen1/REPOSITORY-BUILD/gcc-4.1.1/boost_1_35/lib)
#SET($ENV{BOOST_ROOT}
#    /home/saebjornsen1/REPOSITORY-BUILD/gcc-4.1.1/boost_1_35/)

set( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" "${CMAKE_SOURCE_DIR}/cmake/modules"  ${CMAKE_MODULE_PATH})

#include( FindKDE4Internal )
#find_package(KDE4 REQUIRED)
set( INCLUDE_INSTALL_DIR "/usr/include")
set(BIN_INSTALL_DIR      "bin"   )   # The install dir for executables
set(LIB_INSTALL_DIR      "lib" )

set(INSTALL_TARGETS_DEFAULT_ARGS  RUNTIME DESTINATION "${BIN_INSTALL_DIR}"
                                      LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
                                                                        ARCHIVE
                                                                        DESTINATION
                                                                        "${LIB_INSTALL_DIR}"
                                                                        COMPONENT
                                                                        Devel
                                                                        )



cmake_minimum_required(VERSION 2.6)
add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
#enable_language(Fortran)

#/CLR  /MDd /TP 
set( CMAKE_CXX_FLAGS " -fexceptions -DHAVE_CONFIG_H -g " )
#set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -DHAVE_CONFIG_H" )
#set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fexceptions -DHAVE_CONFIG_H" )
set( CMAKE_C_FLAGS " -fexceptions -DHAVE_CONFIG_H -g " )

#set(  CMAKE_VERBOSE_MAKEFILE  true )

set(ROSE_TOP_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(ROSE_TOP_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

#Generate our configure file

#taken from LyX
include(ConfigureChecks)
#include(KDE4Defaults)

include(MacroLibrary)
include(FindMySQL)
#include(FindOpenSSL)
#include(FindSQLITE3)
include(FindJava)

include(FindFlex)
if(NOT FLEX_FOUND)
    message(FATAL_ERROR "Could not find flex command")
endif(NOT FLEX_FOUND)

include(FindBison)
FIND_BISON()
if(NOT BISON_FOUND)
    message(FATAL_ERROR "Could not find BISON command")
endif(NOT BISON_FOUND)

include(ConvenienceLibs)

SET(Boost_ADDITIONAL_VERSIONS "1.39.0" "1.40.0" )
#Ensure that Boost is found
find_package( Boost 1.36.0 COMPONENTS date_time filesystem program_options regex system wave )
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Could not find Boost version 1.36.0 or newer command")
endif(NOT Boost_FOUND)


#find_library(M_LIB m)
#find_library(RT_LIB rt)


set( link_with_libraries ${Boost_LIBRARIES} ${M_LIB} ${RT_LIB} ${OPENSSL_LIBRARIES} )


configure_file(${ROSE_TOP_SRC_DIR}/rose_config.h.in.cmake ${ROSE_TOP_BINARY_DIR}/rose_config.h)

#include(ManualStuff.cmake)

#include(ConfigureChecks.cmake)

 set( LIBHARU_SOURCE_DIR ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/libharu-200910170404 )
 set( LIBHARU_BINARY_DIR ${ROSE_TOP_BINARY_DIR}/src/3rdPartyLibraries/libharu-200910170404 )
 set( ROSE_INCLUDES 
     ${ROSE_TOP_BINARY_DIR}
     ${ROSE_TOP_BINARY_DIR}/src/frontend/SageIII/
     ${ROSE_TOP_BINARY_DIR}/src/frontend/SageIII/astFileIO
     ${ROSE_TOP_SRC_DIR}
     ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII 
     ${ROSE_TOP_SRC_DIR}/src/ROSETTA/src/
     ${ROSE_TOP_SRC_DIR}/src 
     ${ROSE_TOP_SRC_DIR}/src/frontend/CxxFrontend/EDG_SAGE_Connection/
     ${ROSE_TOP_SRC_DIR}/src/frontend/CxxFrontend/EDG_3.3/src
     ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII 
     ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/astFixup 
     ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/astPostProcessing 
     ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/astMerge 
     ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/astVisualization 
     ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/astFileIO 
     ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/sageInterface 
     ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/virtualCFG 
     ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/astTokenStream 
     ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/astHiddenTypeAndDeclarationLists 
     ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/astFileIO 
     ${ROSE_TOP_SRC_DIR}/src/frontend/OpenFortranParser_SAGE_Connection 
     ${ROSE_TOP_SRC_DIR}/src/frontend/PHPFrontend 
     ${ROSE_TOP_SRC_DIR}/src/frontend/BinaryFormats 
     ${ROSE_TOP_SRC_DIR}/src/frontend/BinaryDisassembly 
     ${ROSE_TOP_SRC_DIR}/src/frontend/Disassemblers 
     ${ROSE_TOP_SRC_DIR}/src/frontend/ExecFormats 
     ${ROSE_TOP_SRC_DIR}/src/backend/unparser 
     ${ROSE_TOP_SRC_DIR}/src/backend/unparser/formatSupport 
     ${ROSE_TOP_SRC_DIR}/src/backend/unparser/languageIndependenceSupport 
     ${ROSE_TOP_SRC_DIR}/src/backend/unparser/CxxCodeGeneration 
     ${ROSE_TOP_SRC_DIR}/src/backend/unparser/FortranCodeGeneration 
     ${ROSE_TOP_SRC_DIR}/src/backend/unparser/PHPCodeGeneration 
     ${ROSE_TOP_SRC_DIR}/src/backend/asmUnparser 
     ${ROSE_TOP_SRC_DIR}/src/util 
     ${ROSE_TOP_SRC_DIR}/src/util/support 
     ${ROSE_TOP_SRC_DIR}/src/util/graphs  
     ${ROSE_TOP_SRC_DIR}/src/util/stringSupport 
     ${ROSE_TOP_SRC_DIR}/src/util/commandlineProcessing 
     ${ROSE_TOP_SRC_DIR}/src/midend/abstractHandle 
     ${ROSE_TOP_SRC_DIR}/src/midend/astDiagnostics 
     ${ROSE_TOP_SRC_DIR}/src/midend/astInlining 
     ${ROSE_TOP_SRC_DIR}/src/midend/astOutlining  
     ${ROSE_TOP_SRC_DIR}/src/midend/astProcessing 
     ${ROSE_TOP_SRC_DIR}/src/midend/astQuery 
     ${ROSE_TOP_SRC_DIR}/src/midend/astRewriteMechanism 
     ${ROSE_TOP_SRC_DIR}/src/midend/astUtil/annotation 
     ${ROSE_TOP_SRC_DIR}/src/midend/astUtil/astInterface 
     ${ROSE_TOP_SRC_DIR}/src/midend/astUtil/astSupport 
     ${ROSE_TOP_SRC_DIR}/src/midend/astUtil/symbolicVal 
     ${ROSE_TOP_SRC_DIR}/src/midend/binaryAnalyses 
     ${ROSE_TOP_SRC_DIR}/src/midend/binaryAnalyses/dataflowanalyses 
     ${ROSE_TOP_SRC_DIR}/src/midend/binaryAnalyses/graph 
     ${ROSE_TOP_SRC_DIR}/src/midend/binaryAnalyses/instructionSemantics 
     ${ROSE_TOP_SRC_DIR}/src/midend/loopProcessing/computation 
     ${ROSE_TOP_SRC_DIR}/src/midend/loopProcessing/depGraph 
     ${ROSE_TOP_SRC_DIR}/src/midend/loopProcessing/depInfo 
     ${ROSE_TOP_SRC_DIR}/src/midend/loopProcessing/driver 
     ${ROSE_TOP_SRC_DIR}/src/midend/loopProcessing/outsideInterface 
     ${ROSE_TOP_SRC_DIR}/src/midend/loopProcessing/prepostTransformation 
     ${ROSE_TOP_SRC_DIR}/src/midend/loopProcessing/slicing 
     ${ROSE_TOP_SRC_DIR}/src/midend/ompLowering 
     ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis 
     ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/CFG 
     ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/CallGraphAnalysis 
     ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/OAWrap 
     ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/dataflowAnalysis 
     ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/defUseAnalysis 
     ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/distributedMemoryAnalysis 
     ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/dominanceAnalysis 
     ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/graphAnalysis 
     ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/pointerAnal 
     ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing 
     ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/valuePropagation 
     ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/partialRedundancyElimination 
     ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/finiteDifferencing 
     ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/functionCallNormalization 
     ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/constantFolding 
     ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/implicitCodeGeneration 
     ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/runtimeTransformation 
     ${ROSE_TOP_SRC_DIR}/src/roseSupport 
     ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/MSTL 
     ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/libharu-2.1.0/include 
     ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/libharu-2.1.0/include 
     ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/qrose/Framework 
     ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/qrose/Widgets  
     ${ROSE_TOP_SRC_DIR}/projects/dataBase 
     ${ROSE_TOP_SRC_DIR}/src/util/graphs 
     ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/libharu-2.1.0/win32/include/
     ${ROSE_TOP_SRC_DIR}/src/midend/astUtil/astInterface )

IF(WIN32) 
set( ROSE_INCLUDES ${ROSE_INCLUDES}
     ${ROSE_TOP_SRC_DIR}/winspecific )
ENDIF()
     
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )
#include_directories( ${ROSE_INCLUDES} )

 



set(QT_INCLUDES ${ROSE_INCLUDES} ${Boost_INCLUDE_DIRS} )

add_subdirectory(config)
add_subdirectory(src)
add_subdirectory(tests)
########### install files ###############

#install(FILES  rose_config.h DESTINATION ${INCLUDE_INSTALL_DIR} )



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#include $(top_srcdir)/projects/compass/src/compassSupport/compass_dirs.inc
#
## Use automake conditional to build Public and Developer distributions
##    Public: includes all of ROSE and binary of EDG work and parts that
##            are more restricted.
##    Developer: This version includes all EDG source code and some parts that
##             might be more restricted.
## This next line will be removed for binary distributions, leaving the variable empty
#DEVELOPER_DISTRIBUTION_DIRECTORY = developersScratchSpace proposals
#
## DQ (1/18/2009): Added demo directory to support showoffs :-).
## DQ "config" and "src" must preceed any other directories, and 
## "tutorial" and "exampleTranslators" must preceed "docs" since 
## they generate data that is subsequently used in the generated 
## ROSE documentation (examples, input codes, and generated output).
## SUBDIRS = config src projects tests tutorial exampleTranslators docs developersScratchSpace tools proposals scripts
#SUBDIRS = libltdl config src projects tests tutorial exampleTranslators docs tools scripts demo binaries $(DEVELOPER_DISTRIBUTION_DIRECTORY)
#
#
#ACLOCAL_AMFLAGS = -I ./config -I ./acmacros -I ./libltdl -I /usr/share/aclocal
#
## Force rose.docs to be make by default 
## (if rose.docs.in is modified)
#all-local: rose.docs 
#
## DQ (3/21/2009): This takes several minutes under Cygwin, so skip these test there.
#if ROSE_BUILD_OS_IS_CYGWIN
#  PROJECT_SOURCE_CODE_TESTS = 
#else
#
## Liao, 6/27/2009 skip the test if PCH is not used
#if ROSE_PCH 
#PROJECT_SOURCE_CODE_TESTS = detect_roseh detect_rosec
#else  
#PROJECT_SOURCE_CODE_TESTS = 
#endif
#
#endif
#
## all: detect_roseh detect_rosec
#all: $(PROJECT_SOURCE_CODE_TESTS)
#
## tps: script that checks that no header file includes rose.h
## rpm: exclude build directories (they contain "include-staging") since ROSE itself creates some source files.
#detect_roseh:
#	touch $(top_builddir)/src/frontend/SageIII/rose.h.pch
#	rm $(top_builddir)/src/frontend/SageIII/rose.h.pch
#	@echo "***************************************************************************************"
#	@echo "***** ROSE uses pre-compiled headers and needs to check integrity. ********************"
#	@echo "***** ROSE ensures that no header files includes rose.h and that   ********************"
#	@echo "***** the first header file in a source file is rose.h (if used)   ********************"
#	@echo "***************************************************************************************"
#	@echo Testing for rose.h in header files...
#	@if test `find $(top_srcdir) \( -type d -exec test -d "{}/include-staging" \; -prune \) -o -type f \( -name "*.h" -o -name "*.hh" \) -exec grep -H "#include \"rose.h\"\|#include <rose.h>" {} \; |  grep -v "//#include \"rose.h\"\|//#include <rose.h>" | grep -v "// #include \"rose.h\"\|// #include <rose.h>" | wc | awk '{print \$$1}'` -eq 0; then echo "rose.h header check passed."; else echo "header files exist that include rose.h"; find $(top_srcdir) \( -type d -exec test -d "{}/include-staging" \; -prune \) -o -type f \( -name "*.h" -o -name "*.hh" \) -exec grep -H "#include \"rose.h\"\|#include <rose.h>" {} \; | grep -v "//#include \"rose.h\"\|//#include <rose.h>" | grep -v "// #include \"rose.h\"\|// #include <rose.h>" ; exit 1; fi
#
## tps: added script from Andreas with small modifications - find all C files that dont include rose in the top of the file 
## rpm: exclude build directories (they contain "include-staging") since ROSE itself creates some source files.
#detect_rosec:
#	@echo Testing for rose.h in source files...
#	@if test `find $(top_srcdir) \( -type d -exec test -d "{}/include-staging" \; -prune \) -o -type f \( -name "*.C" -o -name "*.cpp" -o -name "*.cc" \) -exec grep -H "#include" {} \; | grep "\"rose.h\"\|<rose.h>" | grep -v "\/\/" | cut -d ':' -f 1 | grep -v "preproc-fortran.cc" | grep -v "preproc-fortran-fixed.cc" | uniq | xargs -n 1 grep -m 1 -HP "^#include" | grep -v "rose.h\|mpi.h" | wc | awk '{print \$$1}'` -eq 0; then echo "rose.h source file check passed.";  else echo "There are source files that dont include rose.h as the first header file."; find $(top_srcdir) \( -type d -exec test -d "{}/include-staging" \; -prune \) -o -type f \( -name "*.C" -o -name "*.cpp" -o -name "*.cc" \) -exec grep -H "#include" {} \; | grep "\"rose.h\"\|<rose.h>" | grep -v "\/\/" | cut -d ':' -f 1 | grep -v "preproc-fortran.cc" | grep -v "preproc-fortran-fixed.cc" | uniq | xargs -n 1 grep -m 1 -HP "^#include" | grep -v "rose.h\|mpi.h"; exit 1; fi
#
#
## DQ (3/9/2009): Additional things that we can enforce within ROSE (written by Andreas).
## Additional rules to enforce:
##    1) Finding using declarations in header files:
##       find -name "*.h" -exec grep -nPH "^[ \t]*using[ \t]+namespace" {} \;
##    2) Finding link dependences in Makefile.am files:
##       find -name Makefile.am -exec grep -nPH "^[ \t]*[a-zA-Z]*[_]?LDADD[ \t]*" {} \;
#
#
## DQ (6/29/2004): I don't think we need this!
## LIBS = @LIBS@ ${SAGE_LIBS}
#
## It seems that it is better to handle the config directory with its own Makefile.am
## This allows us to hide autoconf accessory files (config.guess, etc.)
## Copy the config directory to the distribution: why?
## (because it has the file: config/Makefile.for.ROSE.includes.and.libs)
## EXTRA_DIST = stamp-h.in stamp-h1.in GNU_HEADERS config
## EXTRA_DIST = stamp-h.in stamp-h1.in rose.docs.in GNU_HEADERS COPYWRITE ROSE_ResearchPapers
#EXTRA_DIST = stamp-h.in stamp-h1.in rose.docs.in COPYRIGHT LicenseInformation ROSE_ResearchPapers README.OSX README.Cygwin build ChangeLog2
#
## At some point we want to put tals into the distribution (likely just PDF files, but not yet).
## ROSE_Talks
#
## clean-local explanation:
## Remove template repositories.  No source code is compiled, but configuration
##   tests may have created template repositories here.
#clean-local:
#	rm -rf Templates.DB ii_files ti_files *.csv
#
## DQ (9/8/2006): Modified to remove backend specific header files
#uninstall-local:
#	rm -rf $(DESTDIR)$(includedir)/*_HEADERS
#
#distclean-local:
#	-rm -rf $(top_builddir)/include-staging/
#
#nodist_include_HEADERS = rose_config.h
#
## DQ (4/5/2009): This rule is not called when compiling this directory, it is only
## called when compiling the /src/util directory (perhaps it should be located in 
## that directory's Makefile.am.
## DQ (12/3/2007): Added rose_paths.h so that it would be copied to the install tree.
## JJW (8/25/2008): Changed to a .c file
#include $(top_srcdir)/config/build_rose_paths.Makefile
#
#check-local:
#	@echo '************************************************************'
#	@echo '****** make check rule complete (terminated normally) ******'
#	@echo '************************************************************'
#
#install-exec-local:
#	@echo '*****************************************************************'
#	@echo '***** make install-exec rule complete (terminated normally) *****'
#	@echo '*****************************************************************'
#
#install-data-local:
#	-rm -rf $(DESTDIR)$(includedir)/*_HEADERS
#	-mkdir $(DESTDIR)$(includedir)
#	cp -R $(top_builddir)/include-staging/* $(DESTDIR)$(includedir)/
#	@echo '*****************************************************************'
#	@echo '***** make install-data rule complete (terminated normally) *****'
#	@echo '*****************************************************************'
#
## DQ (4/22/2005): Set this so that make distcheck will use the same 
## --with-edg_source_code=true/false option as were used at configure.
## JJW (5/14/2008): Add the Boost flag the same way, using an internal
## variable from AX_BOOST_* to find the argument to --with-boost when
## this copy of ROSE was originally configured.
#DISTCHECK_CONFIGURE_FLAGS = --with-boost=$(ac_boost_path)
#
## DQ (7/25/2008): If ROSE was originallly configured with Fortran (by tuning 
## on the java support) then make sure it is tested as part of the distcheck rule.
#if ROSE_USE_OPEN_FORTRAN_PARSER
#   DISTCHECK_CONFIGURE_FLAGS += --with-java
#endif
#
## DQ (7/25/2008): If ROSE was originallly configured to use the HPC Toolkit
## then make sure it is tested as part of the distcheck rule.
#if ROSE_BUILD_ROSEHPCT
#   DISTCHECK_CONFIGURE_FLAGS += --enable-rosehpct
#endif
#
## DQ (7/25/2008): If ROSE was originallly configured to use the javaport
## work (generation of Java interface functions for ROSE using SWIG) then 
## make sure it is tested as part of the distcheck rule.
#if ENABLE_JAVAPORT
#   DISTCHECK_CONFIGURE_FLAGS += --with-javaport
#endif
#
## DQ (4/22/2005): To make clear when binaries are in the distribution as 
## opposed to EDG source code we rename the final generated distribution.
## JJW (8/3/2008): This is only used for source distributions now --
## source_with_binary_edg_dist does its own renaming
#rename_distribution:
#	new_name="$(distdir)-source-with-EDG-source.tar.gz"; \
#	cp $(distdir).tar.gz $$new_name
#
## We reset distdir as shown on
## http://jcalcote.wordpress.com/2008/02/23/autoconf-macros-exposed-at3/ to add the SVN
## revision number
#distdir = $(PACKAGE)-$(VERSION)$(DOT_SVNREV)
#
## Automake hook-ish rule to be run after dist rule (but hook runs before gzip so we can't use hook)
## We could build our own ROSE specific wrapper for dist and distcheck.
#dist-after:
#	@echo "Running dist hook rule ..."
#	@$(MAKE) rename_distribution
#
## hook rule to be run after distcheck rule
#distcheck-after:
#	@echo "Running distcheck hook rule ..."
#	@$(MAKE) rename_distribution
#
## CLEANFILES = rose_binary_compatibility_signature rose_binary_compatibility_signature_src
#CLEANFILES = rose_binary_compatibility_signature
#
#find_rose_lock_files:
##	du -a | grep rose_performance_report_lockfile
#	find . -name rose_performance_report_lockfile -print
#
## New rule to simplify generation of documentation (we want to avoid using the
## automake generated "docs" rule because the generation of documentation is
## dependent upon separate tools which the user might not have available
## (true for bothe the LaTeX and html documentation).
## DQ (7/23/2004): Since there is a "docs" subdirectory we need to force the build!
#FORCE_DOCS_TO_BE_MADE:
#
## DQ (7/25/2008): Running "make docs" introduces dependences that are inappropriate
## for a distribution.
#docs: FORCE_DOCS_TO_BE_MADE
#	@if [ -f $(top_srcdir)/src/midend/midend.docs ]; then \
#	   echo "    Generate all possible documentation cd docs; make documentation;"; \
#	   $(MAKE) -Ctutorial docs && \
#	   $(MAKE) -Cprojects/haskellport docs && \
#	   $(MAKE) -Ctests/roseTests/astInterfaceTests check && \
#	   $(MAKE) -Ctests/roseTests/ompLoweringTests check && \
#	   $(MAKE) -Ctests/CompileTests/UPC_tests check && \
#	   $(MAKE) -Cprojects/UpcTranslation/tests check && \
#	   $(MAKE) -Cprojects/autoParallelization/tests check && \
#	   $(MAKE) -Cdocs docs && \
#	   $(MAKE) -C$(compass_build_tooldir)/compass docs; \
#   else \
#      echo "Please look at the ROSE web site for documentation (www.roseCompiler.org)"; \
#   fi
#
#
#clean_binary_distribution: distdir
#	rm -rf $(distdir)/src/frontend/CxxFrontend/EDG*
#	rm -rf $(distdir)/src/frontend/CxxFrontend/Makefile.am # Will be replaced with another version in the binary_edg_tarball rule
#	rm -rf $(distdir)/proposals
#	rm -rf $(distdir)/developersScratchSpace
## Dan said this is OK 5/27/2009	
##	rm -rf $(distdir)/projects/assemblyToSourceAst
#	rm -rf $(distdir)/docs/testDoxygen
#
#if BINARY_EDG_TARBALL_ENABLED
#
#rose_binary_compatibility_signature:
#	find $(srcdir)/src/ROSETTA $(srcdir)/src/frontend/CxxFrontend \( -name \*.C -o -name \*.c -o -name \*.h -o -name \*.code -o -name \*.macro -o -name astNodeList -o -name \*.yy \) -a -print | env LC_ALL=C sort | xargs cat -ve | \
#	@MD5@ | cut -f 1 -d \  > rose_binary_compatibility_signature
#	cat rose_binary_compatibility_signature
#
#.PHONY : rose_binary_compatibility_signature # Always rebuild the file when needed
#
## 1
## This rule generates the simple binary tarball for the directories: EDG and EDG_SAGE_Connection
#binary_edg_tarball: rose_binary_compatibility_signature
#	cd src/ROSETTA/src; $(MAKE)
#	cd src/frontend/CxxFrontend; $(MAKE)
##	BINARY_EDG_DIRNAME=roseBinaryEDG-$(build_triplet_without_redhat)-`cat rose_binary_compatibility_signature` &&
#	BINARY_EDG_DIRNAME=roseBinaryEDG-$(build_triplet_without_redhat)-@GENERATED_COMPILER_NAME_AND_VERSION_SUFFIX@-`cat rose_binary_compatibility_signature` && \
#	rm -rf $${BINARY_EDG_DIRNAME} && \
#	mkdir $${BINARY_EDG_DIRNAME} && \
#	sed "s@^dependency_libs.*@dependency_libs=''@" src/frontend/CxxFrontend/libroseEDG.la > $${BINARY_EDG_DIRNAME}/libroseEDG.la && \
#	cp -Rp src/frontend/CxxFrontend/.libs $${BINARY_EDG_DIRNAME}/ && \
#	tardir=$${BINARY_EDG_DIRNAME} && $(am__tar) | GZIP=$(GZIP_ENV) gzip -c >$${BINARY_EDG_DIRNAME}.tar.gz && \
#	rm -rf $${BINARY_EDG_DIRNAME}
#
## 2
## This rule will insert the binary compatability signature into the filename for the generated binary.
#copy_binary_edg_tarball_to_source_tree: binary_edg_tarball rose_binary_compatibility_signature
##	BINARY_EDG_DIRNAME=roseBinaryEDG-$(build_triplet_without_redhat)-`cat rose_binary_compatibility_signature`
##	BINARY_EDG_DIRNAME=roseBinaryEDG-$(build_triplet_without_redhat)-@GENERATED_COMPILER_NAME_AND_VERSION_SUFFIX@-`cat rose_binary_compatibility_signature` && 
##	$(top_srcdir)/scripts/copy_binary_edg_tarball_to_source_tree $(top_srcdir) "@build_triplet_without_redhat@"
#	$(top_srcdir)/scripts/copy_binary_edg_tarball_to_source_tree $(top_srcdir) "@build_triplet_without_redhat@" "@GENERATED_COMPILER_NAME_AND_VERSION_SUFFIX@"
#
## 3
## Commits the EDG binary into the SVN repository.
## copy_binary_edg_tarball_to_source_tree_svn: binary_edg_tarball rose_binary_compatibility_signature
#copy_binary_edg_tarball_to_source_tree_svn: copy_binary_edg_tarball_to_source_tree
##	BINARY_EDG_DIRNAME=roseBinaryEDG-$(build_triplet_without_redhat)-`cat rose_binary_compatibility_signature` && 
##	$(top_srcdir)/scripts/copy_binary_edg_tarball_to_source_tree_svn `sed -n 5p $(top_srcdir)/.svn/entries` "@build_triplet_without_redhat@"
#	$(top_srcdir)/scripts/copy_binary_edg_tarball_to_source_tree_svn `sed -n 5p $(top_srcdir)/.svn/entries` "@build_triplet_without_redhat@" "@GENERATED_COMPILER_NAME_AND_VERSION_SUFFIX@" `sed -n 4p $(top_srcdir)/.svn/entries`
#
## 4
## Builds source release containing the EDG binary.
## source_with_binary_edg_dist: rose_binary_compatibility_signature
## source_with_binary_edg_dist: copy_binary_edg_tarball_to_source_tree
#
## DQ (9/16/2009): I think it is an error to call the copy_binary_edg_tarball_to_source_tree_svn rule, 
##    call copy_binary_edg_tarball_to_source_tree (as it used to be). It fails to work on a distribution
##    because there in no ".svn/entries" file.
##    ../sourcetree/scripts/copy_binary_edg_tarball_to_source_tree_svn `sed -n 5p ../sourcetree/.svn/entries` "i686-apple-darwin" "GNU-4.0"
##    sed: ../sourcetree/.svn/entries: No such file or directory
## source_with_binary_edg_dist: copy_binary_edg_tarball_to_source_tree_svn
#source_with_binary_edg_dist: copy_binary_edg_tarball_to_source_tree
#	@ROSE_BINARY_COMPATIBILITY_SIGNATURE=`cat rose_binary_compatibility_signature` && \
#	if test `ls -1 $(srcdir)/src/frontend/CxxFrontend/roseBinaryEDG-*-$${ROSE_BINARY_COMPATIBILITY_SIGNATURE}.tar.gz | wc -l` -eq 0; then echo "Error: Could not find any binary versions of EDG with signature $${ROSE_BINARY_COMPATIBILITY_SIGNATURE} in $(srcdir)/src/frontend/CxxFrontend.  This distribution cannot be made because it would not be usable on any platform." >&2; exit 1; fi && \
#	if test `ls -1 $(srcdir)/src/frontend/CxxFrontend/roseBinaryEDG-*-*.tar.gz | grep -v $${ROSE_BINARY_COMPATIBILITY_SIGNATURE} | wc -l` -ne 0; then echo "Error: The following obsolete binary versions of EDG were found in $(srcdir)/src/frontend/CxxFrontend." >&2; ls -1 $(srcdir)/src/frontend/CxxFrontend/roseBinaryEDG-*-*.tar.gz | grep -v $${ROSE_BINARY_COMPATIBILITY_SIGNATURE} >&2; exit 1; fi && \
#	$(MAKE) clean_binary_distribution && \
#	cp $(srcdir)/src/frontend/CxxFrontend/roseBinaryEDG-*-$${ROSE_BINARY_COMPATIBILITY_SIGNATURE}.tar.gz $(distdir)/src/frontend/CxxFrontend && \
#	cat $(top_srcdir)/configure.in | \
#		grep -v '^developersScratchSpace/' | \
#		grep -v '^proposals/' | \
#		grep -v '^src/frontend/CxxFrontend/EDG' | \
#		grep -v '^docs/testDoxygen/' | \
#		sed s/RELEASE_BINARY_COMPATIBILITY_SIGNATURE/$${ROSE_BINARY_COMPATIBILITY_SIGNATURE}/g > \
#		$(distdir)/configure.in
#	cat $(top_srcdir)/Makefile.am | \
#		grep -v 'DEVELOPER_DISTRIBUTION_DIRECTORY *=' > \
#		$(distdir)/Makefile.am # Remove SUBDIRS references to developersScratchSpace and proposals
#	cat $(top_srcdir)/docs/Makefile.am | \
#		grep -v 'DEVELOPER_DISTRIBUTION_DIRECTORY *=' > \
#		$(distdir)/docs/Makefile.am # Remove SUBDIRS reference to testDoxygen
#	cp $(top_srcdir)/src/frontend/CxxFrontend/Makefile.am.forBinaryEDG $(distdir)/src/frontend/CxxFrontend/Makefile.am
#	echo "" >> $(distdir)/src/frontend/CxxFrontend/Makefile.am
#	cd $(distdir)/src/frontend/CxxFrontend; echo "EXTRA_DIST = `echo *.tar.gz`" >> Makefile.am
#	cd $(distdir) && ./build
#	find $(distdir)/ -name Makefile.in~ | xargs rm -rf
#	find $(distdir)/ -depth -name .svn -exec rm -rf {} \;
#	find $(distdir)/ -depth -name autom4te.cache -exec rm -rf {} \;
#	tardir=$(distdir) && $(am__tar) | GZIP=$(GZIP_ENV) gzip -c >$(PACKAGE)-$(VERSION)-source-with-EDG-binary$(DOT_SVNREV).tar.gz
#	$(am__remove_distdir)
#
## DQ (9/10/2009): Build a tarball for testing at NMI (ROSE without EDG source)
## Some of the names of these makefile rules should be changed to be more specific.
#$(PACKAGE)-$(VERSION)-source-with-EDG-binary.tar.gz: source_with_binary_edg_dist
##	$(MAKE) copy_binary_edg_tarball_to_source_tree
##	$(MAKE) copy_binary_edg_tarball_to_source_tree_svn
##	$(MAKE) source_with_binary_edg_dist
#
## DQ (9/14/2009): this can fail if the most recent binaries are not available.
#binary_tarball: $(PACKAGE)-$(VERSION)-source-with-EDG-binary.tar.gz
#
#endif
