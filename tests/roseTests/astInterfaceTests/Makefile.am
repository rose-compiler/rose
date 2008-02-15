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
    findMain buildVariableDeclaration buildAssignmentStmt \
    buildFunctionCalls buildPragmaDeclaration attachComment \
    insertHeader 

# list of test SAGE AST builders 
buildFunctionDeclaration_SOURCES = buildFunctionDeclaration.C
buildNondefiningFunction_SOURCES = buildNondefiningFunction.C
findMain_SOURCES                 = findMain.C
buildVariableDeclaration_SOURCES = buildVariableDeclaration.C
buildAssignmentStmt_SOURCES      = buildAssignmentStmt.C
buildFunctionCalls_SOURCES       = buildFunctionCalls.C  buildFunctionCalls.h
buildPragmaDeclaration_SOURCES   = buildPragmaDeclaration.C
attachComment_SOURCES            = attachComment.C
insertHeader_SOURCES             = insertHeader.C


# libsageInterface.la is included in rose.la already?
LDADD =  $(ROSE_LIBS)

#    $(top_builddir)/src/frontend/SageIII/sageInterface/libsageInterface.la 
if ROSE_USE_LONG_MAKE_CHECK_RULE

else

endif
generatedCodeExamples = \
  rose_inputBlank1.C \
  rose_inputBlank2.C \
  rose_inputfindMain.C \
  rose_inputbuildVariableDeclaration.C \
  rose_inputbuildAssignmentStmt.C \
  rose_inputbuildFunctionCalls.C \
  rose_inputbuildPragmaDeclaration.c \
  rose_inputAttachComment.C \
  rose_inputInsertHeader.C

rose_inputBlank1.C:buildFunctionDeclaration
	./buildFunctionDeclaration $(TEST_CXXFLAGS) -c $(srcdir)/inputBlank1.C
rose_inputBlank2.C:buildNondefiningFunction
	./buildNondefiningFunction $(TEST_CXXFLAGS) -c $(srcdir)/inputBlank2.C
rose_inputfindMain.C:findMain
	./findMain  $(TEST_CXXFLAGS) -c  $(srcdir)/inputfindMain.C
rose_inputbuildVariableDeclaration.C:buildVariableDeclaration
	./buildVariableDeclaration  $(TEST_CXXFLAGS) -c $(srcdir)/inputbuildVariableDeclaration.C
rose_inputbuildAssignmentStmt.C:buildAssignmentStmt
	./buildAssignmentStmt $(TEST_CXXFLAGS) -c $(srcdir)/inputbuildAssignmentStmt.C	

rose_inputbuildFunctionCalls.C:buildFunctionCalls
	./buildFunctionCalls $(TEST_CXXFLAGS) -I$(srcdir) -c $(srcdir)/inputbuildFunctionCalls.C	
rose_inputbuildPragmaDeclaration.c:buildPragmaDeclaration
	./buildPragmaDeclaration $(TEST_CXXFLAGS) -c $(srcdir)/inputbuildPragmaDeclaration.c
rose_inputAttachComment.C:attachComment
	./attachComment  $(TEST_CXXFLAGS) -c $(srcdir)/inputAttachComment.C	
rose_inputInsertHeader.C:insertHeader
	./insertHeader $(TEST_CXXFLAGS) -c $(srcdir)/inputInsertHeader.C

TEST_CXXFLAGS = -rose:verbose 0 -g3

EXTRA_DIST = inputBlank1.C inputBlank2.C inputfindMain.C inputbuildVariableDeclaration.C \
       inputbuildAssignmentStmt.C inputbuildFunctionCalls.C inputbuildPragmaDeclaration.c \
       inputAttachComment.C

check-local:
	@echo "Test for ROSE SAGE Interfaces."
	$(MAKE) $(generatedCodeExamples)
clean-local:
	rm -f *.o test*.C rose_*.c rose_*.C *.C.pdf *.c.pdf

