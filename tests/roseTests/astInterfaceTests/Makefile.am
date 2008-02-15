# Liao, Jan. 9, 2008
# Makefile.am for a set of test AST builders and test input code
#-------------------------------------------------------------
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES =  \
        -I$(top_srcdir) -I$(top_builddir) \
        $(ROSE_INCLUDES) \
        -I$(top_srcdir)/src/frontend/SageIII/sageInterface

#not for installation
noinst_PROGRAMS = \
    buildFunctionDeclaration buildNondefiningFunction \
    findMain buildVariableDeclaration

# list of test SAGE AST builders 
buildFunctionDeclaration_SOURCES = buildFunctionDeclaration.C
buildNondefiningFunction_SOURCES = buildNondefiningFunction.C
findMain_SOURCES                 = findMain.C
buildVariableDeclaration_SOURCES = buildVariableDeclaration.C

# libsageInterface.la is included in rose.la already?
LDADD =  $(ROSE_LIBS)

#    $(top_builddir)/src/frontend/SageIII/sageInterface/libsageInterface.la 
if ROSE_USE_LONG_MAKE_CHECK_RULE

else

endif

TEST_CXXFLAGS = -rose:verbose 2 -g3

EXTRA_DIST = inputBlank1.C inputBlank2.C inputfindMain.C inputbuildVariableDeclaration.C

check-local:
	@echo "Test for ROSE SAGE Interfaces."
	./buildFunctionDeclaration $(TEST_CXXFLAGS) -c $(srcdir)/inputBlank1.C
	./buildNondefiningFunction $(TEST_CXXFLAGS) -c $(srcdir)/inputBlank2.C
	./findMain  $(TEST_CXXFLAGS) -c  $(srcdir)/inputfindMain.C
	./buildVariableDeclaration  $(TEST_CXXFLAGS) -c $(srcdir)/inputbuildVariableDeclaration.C

clean-local:
	rm -f *.o test*.C rose_*.c rose_*.C *.C.pdf *.c.pdf

