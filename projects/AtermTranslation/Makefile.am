include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# The Stratego and A-Term support is outside of ROSE
# This needs a configure flag so that the location outside 
# of ROSE can be easily specificed at configure time.
# TERM_INCLUDE = -I/home/dquinlan/ROSE/Stratego-tux49/include
# A_TERM_INCLUDE = -I/home/willcock2/sw/include
A_TERM_INCLUDE = -I$(ATERM_INSTALL_PATH)/include

INCLUDES = $(ROSE_INCLUDES) $(A_TERM_INCLUDE)

# DQ (10/27/2004): Removed because it has dependence upon Stratego (which may not be installed)
# This will be made a option in the future and the location of the Stratego package will
# have to be specified on the configure command line!
bin_PROGRAMS = termToStratego strategoToTerm

termToStratego_SOURCES = termToStratego.C astToAtermImpl.C atermTranslation.h

strategoToTerm_SOURCES = strategoToTerm.C atermToAstImpl.C atermTranslation.h

include_HEADERS = atermTranslation.h

# There is a difference between the two versions of Linux
# such that the A-Term library cannot be mixed with a version
# of ROSE that is compiled with the wrong version.  I have 
# verified that Red Hat Enterprise Linux WS release 3 (Taroon Update 2)
# works with the A-Term library and that the older OS on tux49
# works with the A-Term library (but only with the one compiled there).
# tux40: Red Hat Enterprise Linux WS release 3 (Taroon Update 2)
# tux40: ATERM_LIB_DIR = /home/dquinlan/ROSE/Stratego/lib

# tux49: Red Hat Linux release 7.2 (Enigma)
# tux49: ATERM_LIB_DIR = /home/dquinlan/ROSE/Stratego-tux49/lib

# DQ (10/27/2004): This explicit path will be later set at configure time.
# TERM_LIB_DIR = /home/dquinlan/ROSE/Stratego-tux49/lib
# ATERM_LIB_DIR = /home/dquinlan/ROSE/Stratego/lib
# ATERM_LIB_DIR = /home/willcock2/sw/lib
ATERM_LIB_DIR = $(ATERM_INSTALL_PATH)/lib

# LDADD = $(ATERM_LIB_DIR)/libATerm.a $(LIBS_WITH_RPATH) $(ROSE_LIBS) 
# LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) $(ATERM_LIB_DIR)/libATerm.a 
# LDADD = -L/home/dquinlan2/ROSE/LINUX-3.3.2/src /home/dquinlan2/ROSE/LINUX-3.3.2/src/librose.so \
#         /home/dquinlan2/ROSE/LINUX-3.3.2/src/libedg.so $(ATERM_LIB_DIR)/libATerm-gcc.a
#        -L/usr/apps/gcc/3.3.2/lib /usr/apps/gcc/3.3.2/lib/libgc.a -lc
LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) $(ATERM_LIB_DIR)/libATerm.a 


TESTCODES_REQUIRED_TO_PASS = test2013_01.C

TESTCODE_CURRENTLY_FAILING =

# Automake's testing mechanism (which defines the "make check" rule) requires passing tests.
TESTCODES = $(TESTCODES_REQUIRED_TO_PASS)

ALL_TESTCODES = $(TESTCODES) $(TESTCODE_CURRENTLY_FAILING)

PASSING_TEST_Output = $(addprefix rose_,${TESTCODES})
TEST_Output = $(addprefix rose_,${ALL_TESTCODES})

rose_%: $(srcdir)/% termToStratego
	./termToStratego $(ROSE_FLAGS) -c $< -o $@


# $(TEST_Output): ../../testTranslator $(srcdir)/$(@:rose_%=%)
# ../../testTranslator -rose:UPC $(ROSE_FLAGS) -c $(srcdir)/$(@:rose_%=%)
../../testTranslator:
	cd ../..; $(MAKE) testTranslator



check-local:
	@echo "Tests for ATerm examples."
	@$(MAKE) $(PASSING_TEST_Output)
	@echo "********************************************************************************************"
	@echo "****** ROSE/projects/AtermTranslation: make check rule complete (terminated normally) ******"
	@echo "********************************************************************************************"


EXTRA_DIST = 
CLEANFILES = 

clean-local:
	rm -rf Templates.DB ii_files ti_files rose_*.C *.pdf

# rule to compile stretego program to generate a C file 
# which stretego compiles to generate an executable.
strtest1$(EXEEXT): strtest1.str
	strc -i strtest1


