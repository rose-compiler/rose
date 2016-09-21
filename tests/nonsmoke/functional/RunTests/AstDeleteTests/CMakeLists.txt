
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

set(memoryTranslator_SRCS test.C)

kde4_add_executable(memoryTranslator ${memoryTranslator_SRCS})

target_link_libraries(memoryTranslator ${KDE4_KDECORE_LIBS})

install(TARGETS memoryTranslator DESTINATION bin)


########### install files ###############




#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#TEST_TRANSLATOR = $(top_builddir)/tests/nonsmoke/functional/testTranslator $(ROSE_FLAGS)
#
#bin_PROGRAMS = memoryTranslator
#memoryTranslator_SOURCES = test.C
#
#INCLUDES = $(ROSE_INCLUDES)
#LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)
#
#TESTCODES_REQUIRED_TO_PASS = \
#      test1.C
#
#TESTCODES = \
#$(TESTCODES_REQUIRED_TO_PASS)
#
#TEST_Objects = ${TESTCODES:.c=.o}
#
#
#$(TEST_Objects): memoryTranslator
#	./memoryTranslator $(ROSE_FLAGS) -c $(srcdir)/$(@:.o=.c)
#
#
## This rule is run after automake's internal check rule (which we don't want to use)
#check-local:
#	@echo "#################################################"
#	@echo "    Test ROSEs Deleting of nodes                 "
#	@echo "#################################################"
#	@$(MAKE) $(TEST_Objects)
#	@echo "*****************************************************************************************************************"
#	@echo "*  ROSE/tests/nonsmoke/functional/RunTests/AstDeleteTests: make check rule complete (terminated normally)                           *"
#	@echo "*****************************************************************************************************************"
#
#
#EXTRA_DIST = $(TESTCODES_REQUIRED_TO_PASS)
#
#clean-local:
#	rm -f *.o test1 *.s rm *.out rose_*.C *.C.pdf *.dot core local_test-isystemOption.C *.dump *.new
#
