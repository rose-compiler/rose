include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/src/frontend/OpenFortranParser_SAGE_Connection

# This is the free format lexical analyzer
$(srcdir)/preproc-fortran.C : preproc-fortran.lex 
	$(LEX) -PRose_Fortran_free_format_ -o$(srcdir)/preproc-fortran.C $(srcdir)/preproc-fortran.lex

preproc-fortran.o : $(srcdir)/preproc-fortran.C
#	$(srcdir)/gen_rose_token_defs.pl > rose_token_defs_gen.h
	$(CXXCOMPILE) -D__EXTERN_C__ -c $(srcdir)/preproc-fortran.C -o $@

# This is the fixed format lexical analyzer
$(srcdir)/preproc-fortran-fixed.C : preproc-fortran-fixed.lex
	$(LEX) -PRose_Fortran_fixed_format_ -o$(srcdir)/preproc-fortran-fixed.C $(srcdir)/preproc-fortran-fixed.lex

preproc-fortran-fixed.o : $(srcdir)/preproc-fortran-fixed.C
	$(CXXCOMPILE) -D__EXTERN_C__ -c $(srcdir)/preproc-fortran-fixed.C -o $@

# Copy the original lex file and the C++ file generated from it to the distribution
# so that users don't have to find an appropriate version of lex (flex).  Flex should
# be used by developers so that ANSI C code is generated for g++.  This is something
# for developers to worry about in the generation of sufficently portable distributions.
EXTRA_DIST = preproc-fortran.lex preproc-fortran.C preproc-fortran-fixed.lex preproc-fortran-fixed.C
CLEANFILES = preproc-fortran.o preproc-fortran-fixed.o 

FORTRAN_LEX_SCANNERS = preproc-fortran.C preproc-fortran-fixed.C

if ROSE_USE_OPEN_FORTRAN_PARSER
if DEV_LIBS
# lib_LTLIBRARIES = libroseFortran.la libparser_java_FortranParserActionJNI.la
# lib_LTLIBRARIES = libroseFortran.la libfortran_ofp_parser_java_FortranParserActionJNI.la
lib_LTLIBRARIES = libroseFortran.la libfortran_ofp_parser_c_jni_FortranParserActionJNI.la
else
# This will cause the dynamic library to be built (as libroseFortran.so)
# noinst_LTLIBRARIES = libroseFortran.la
# lib_LTLIBRARIES = libroseFortran.la
noinst_LTLIBRARIES = libroseFortran.la

# This shared library is built in the Open Fortran Parser using the command:
#      gcc -shared -I/usr/apps/java/jdk1.5.0_11/include -I/usr/apps/java/jdk1.5.0_11/include/linux 
#                  -I../  -fPIC ../token.c jni_token_funcs.c c_actions_jni.c ../FortranParserActionPrint.c 
#                  -o ../../../../build/libparser_java_FortranParserActionJNI.so
# To build a similar library using libtool we indicate that we want an installed 
# library (which will then be made into a dynamic shared library).
# lib_LTLIBRARIES = libparser_java_FortranParserActionJNI.la
# lib_LTLIBRARIES = libfortran_ofp_parser_java_FortranParserActionJNI.la
lib_LTLIBRARIES = libfortran_ofp_parser_c_jni_FortranParserActionJNI.la
endif
else
# DQ (10/21/2007): build an empyy lib file so that ROSE/src/Makefile.am need not be conditional on Fortran.
noinst_LTLIBRARIES = libroseFortran.la
endif

if ROSE_USE_OPEN_FORTRAN_PARSER
# Compile using the function to call the Open Fortran Parser (should be a C file, since it will not compile currently as a C++ file)
openFortranParserMain = openFortranParser_main.c
# libroseFortran_la_LIBADD = -L$(JAVA_JVM_PATH) -ljvm
libroseFortran_la_LIBADD =
else
# Compile using a function that represents the API of the Open Fortran Parser
openFortranParserMain = dummy_openFortranParser_main.c
libroseFortran_la_LIBADD =
endif

# libroseFortran_la_SOURCES    = FortranParserActionROSE.C fortran_support.C token.c $(FORTRAN_LEX_SCANNERS) $(openFortranParserMain)
libroseFortran_la_SOURCES      = fortran_support.C $(FORTRAN_LEX_SCANNERS) $(openFortranParserMain)
libroseFortran_la_DEPENDENCIES =

if ROSE_USE_OPEN_FORTRAN_PARSER
# This file (these c_action() function implementations) need to be in a dynamic 
# library so that they can be called by the Java based Open Fortran Parser.
# libparser_java_FortranParserActionJNI_la_SOURCES      = FortranParserActionROSE.C token.c jni_token_funcs.c c_actions_jni.c
# libparser_java_FortranParserActionJNI_la_DEPENDENCIES =
# libfortran_ofp_parser_java_FortranParserActionJNI_la_SOURCES      = FortranParserActionROSE.C token.c jni_token_funcs.c c_actions_jni.c
# libfortran_ofp_parser_java_FortranParserActionJNI_la_DEPENDENCIES =
libfortran_ofp_parser_c_jni_FortranParserActionJNI_la_SOURCES      = FortranParserActionROSE.C token.c jni_token_funcs.c c_actions_jni.c
libfortran_ofp_parser_c_jni_FortranParserActionJNI_la_DEPENDENCIES =

# To support the Open Fortran Parser we need to build a shared library with a specific name and which will have
# all of the c_action() function implementations.  The name to match is: XXX libparser_java_FortranParserActionJNI.so
# As of Feb 18 2008: the name is changed to libfortran_ofp_parser_c_jni_FortranParserActionJNI.so

include_HEADERS = ActionEnums.h  FortranParserAction.h  token.h rose_fortran_token_maps.h  rose_token_defs.h fortran_support.h jni_token_funcs.h

openFortranParserMain: $(top_builddir)/rose_paths.h

# Moved this header file to ../SageIII, it is not used for the lex pass for all languages in ROSE.
# general_defs.h

# DQ (9/24/2007): After updating the Open Fortran Parser, there are a number of files that should be copied over to ROSE.
# Note that only the following are used in ROSE:
#    1) token.c and token.h (definition of OFP's tokens)
#    2) FortranParserAction.h  (header file of C_action functions)
#    3) ActionEnums.h (enum values used in OFP and queried in ROSE)
# All other files copied to ROSE are used as reference within the development.
update:
#	cp $(OPEN_FORTRAN_PARSER_PATH)/src/parser/c/token.* $(srcdir)
#	cp $(OPEN_FORTRAN_PARSER_PATH)/src/parser/c/jni/jni_token_funcs.* $(srcdir)
#	cp $(OPEN_FORTRAN_PARSER_PATH)/src/parser/c/jni/c_actions_jni.c $(srcdir)
#	cp $(OPEN_FORTRAN_PARSER_PATH)/src/parser/c/FortranParserAction.h $(srcdir)
#	cp $(OPEN_FORTRAN_PARSER_PATH)/src/parser/c/ActionEnums.h $(srcdir)
#	cp $(OPEN_FORTRAN_PARSER_PATH)/src/parser/c/FortranParserActionNull.c $(srcdir)
	cp $(OPEN_FORTRAN_PARSER_PATH)/src/fortran/ofp/parser/c/token.* $(srcdir)
	cp $(OPEN_FORTRAN_PARSER_PATH)/src/fortran/ofp/parser/c/FortranParserAction.h $(srcdir)
	cp $(OPEN_FORTRAN_PARSER_PATH)/src/fortran/ofp/parser/c/ActionEnums.h $(srcdir)
	cp $(OPEN_FORTRAN_PARSER_PATH)/src/fortran/ofp/parser/c/FortranParserActionNull.c $(srcdir)
	cp $(OPEN_FORTRAN_PARSER_PATH)/src/fortran/ofp/parser/c/FortranParserActionPrint.c $(srcdir)
	cp $(OPEN_FORTRAN_PARSER_PATH)/src/fortran/ofp/parser/c/jni/jni_token_funcs.* $(srcdir)
	cp $(OPEN_FORTRAN_PARSER_PATH)/src/fortran/ofp/parser/c/jni/c_actions_jni.c $(srcdir)
	cp $(OPEN_FORTRAN_PARSER_PATH)/src/fortran/ofp/parser/java/IFortranParserAction.java $(srcdir)

endif

# EXTRA_DIST = FortranParserActionROSE.C token.c jni_token_funcs.c c_actions_jni.c 

clean-local:
	rm -rf *.o

distclean-local:
