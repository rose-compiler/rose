# Liao, Jan. 9, 2008
# Makefile.am for a set of test AST builders and test input code
#-------------------------------------------------------------
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES =  \
        $(ROSE_INCLUDES) \
        -I$(top_srcdir)/src/frontend/SageIII/sageInterface

#not for installation
noinst_PROGRAMS = \
    buildFunctionDeclaration buildNondefiningFunction \
    findMain buildVariableDeclaration buildAssignmentStmt \
    buildFunctionCalls buildPragmaDeclaration attachComment \
    insertHeader buildExpression buildStructDeclaration \
    lookupNamedType buildFile movePreprocessingInfo buildIfStmt

# list of test SAGE AST builders 
buildFunctionDeclaration_SOURCES = buildFunctionDeclaration.C
buildNondefiningFunction_SOURCES = buildNondefiningFunction.C
findMain_SOURCES                 = findMain.C
buildVariableDeclaration_SOURCES = buildVariableDeclaration.C
buildAssignmentStmt_SOURCES      = buildAssignmentStmt.C
buildFunctionCalls_SOURCES       = buildFunctionCalls.C 
buildPragmaDeclaration_SOURCES   = buildPragmaDeclaration.C
attachComment_SOURCES            = attachComment.C
insertHeader_SOURCES             = insertHeader.C
buildExpression_SOURCES          = buildExpression.C
buildStructDeclaration_SOURCES   = buildStructDeclaration.C
lookupNamedType_SOURCES          = lookupNamedType.C           
buildFile_SOURCES                = buildFile.C           
movePreprocessingInfo_SOURCES    = movePreprocessingInfo.C
buildIfStmt_SOURCES              = buildIfStmt.C

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
  rose_inputInsertHeader.C \
  rose_inputbuildExpression.C \
  rose_inputbuildStructDeclaration.C \
  rose_inputLookupNamedType.C \
  rose_inputbuildFile.C \
  rose_inputMovePreprocessingInfo.C \
  rose_inputbuildIfStmt.C

# turn off edg warnings such as "variable x declared but never referenced" for test input files
TEST_CXXFLAGS = -rose:verbose 0 --edg:no_warnings -g3

rose_inputBlank1.C:buildFunctionDeclaration
	./buildFunctionDeclaration$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputBlank1.C
rose_inputBlank2.C:buildNondefiningFunction
	./buildNondefiningFunction$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputBlank2.C
rose_inputfindMain.C:findMain
	./findMain$(EXEEXT)  $(TEST_CXXFLAGS) -c  $(srcdir)/inputfindMain.C
rose_inputbuildVariableDeclaration.C:buildVariableDeclaration
	./buildVariableDeclaration$(EXEEXT)  $(TEST_CXXFLAGS) -c $(srcdir)/inputbuildVariableDeclaration.C
rose_inputbuildAssignmentStmt.C:buildAssignmentStmt
	./buildAssignmentStmt$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputbuildAssignmentStmt.C	
rose_inputbuildIfStmt.C:buildIfStmt
	./buildIfStmt$(EXEEXT) $(TEST_CXXFLAGS) -I$(srcdir) -c $(srcdir)/inputbuildIfStmt.C 
rose_inputbuildFile.C:buildFile
	rm -rf testfile626.cpp
	./buildFile$(EXEEXT) $(TEST_CXXFLAGS) -I$(srcdir) -c $(srcdir)/inputbuildFile.C 
rose_inputbuildFunctionCalls.C:buildFunctionCalls
	./buildFunctionCalls$(EXEEXT) $(TEST_CXXFLAGS) -I$(srcdir) -c $(srcdir)/inputbuildFunctionCalls.C	
rose_inputbuildPragmaDeclaration.c:buildPragmaDeclaration
	./buildPragmaDeclaration$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputbuildPragmaDeclaration.c
rose_inputAttachComment.C:attachComment
	./attachComment$(EXEEXT)  $(TEST_CXXFLAGS) -c $(srcdir)/inputAttachComment.C	
rose_inputInsertHeader.C:insertHeader
	./insertHeader$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputInsertHeader.C
rose_inputbuildExpression.C: buildExpression
	./buildExpression$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputbuildExpression.C
rose_inputbuildStructDeclaration.C:buildStructDeclaration
	./buildStructDeclaration$(EXEEXT)  $(TEST_CXXFLAGS) -c $(srcdir)/inputbuildStructDeclaration.C
rose_inputLookupNamedType.C:lookupNamedType
	./lookupNamedType$(EXEEXT)  $(TEST_CXXFLAGS) -c $(srcdir)/inputLookupNamedType.C
rose_inputMovePreprocessingInfo.C:movePreprocessingInfo
	./movePreprocessingInfo$(EXEEXT) $(TEST_CXXFLAGS) -c $(srcdir)/inputMovePreprocessingInfo.C

# MUST keep this up-to-date!!
EXTRA_DIST = inputBlank1.C inputBlank2.C inputfindMain.C inputbuildVariableDeclaration.C \
       inputbuildAssignmentStmt.C inputbuildFunctionCalls.C inputbuildFunctionCalls.h \
       inputbuildPragmaDeclaration.c inputAttachComment.C inputInsertHeader.C \
       inputbuildExpression.C inputbuildStructDeclaration.C inputLookupNamedType.C \
       inputbuildFile.C inputMovePreprocessingInfo.C inputbuildIfStmt.C

check-local:
	@echo "Test for ROSE SAGE Interfaces."
	$(MAKE) $(generatedCodeExamples)
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/tests/roseTests/astInterfaceTests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************"

clean-local:
	rm -f *.o test*.C rose_*.c rose_*.C rose_*.cpp *.C.pdf *.c.pdf testfile525.cpp testfile626.cpp  

