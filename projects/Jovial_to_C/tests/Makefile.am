include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

TESTCODE_INCLUDES = 

J2C_FLAGS = -rose:C_output_language 

BASIC_TESTCODES_REQUIRED_TO_PASS = \
        tiny.jov

ALL_TESTCODES = \
$(BASIC_TESTCODES_REQUIRED_TO_PASS)

PASSING_BASIC_JOVIAL_TEST_OBJECTS = \
$(BASIC_TESTCODES_REQUIRED_TO_PASS:.jov=.o) 

if ROSE_EXPERIMENTAL_JOVIAL_ROSE_CONNECTION

translate:
	$(MAKE) -C ../src translate

$(PASSING_BASIC_JOVIAL_TEST_OBJECTS): %.o: $(srcdir)/%.jov translate
	../src/translate $(J2C_FLAGS) -c $<

conditional-check-local:
	@echo "Test for Jovial-to-C translator."	
	@$(MAKE) $(PASSING_BASIC_JOVIAL_TEST_OBJECTS)
	@echo "======== make check rule for Jovial-to-C terminated normally ========"

else
conditional-check-local:
	@echo "*********************************************************************************************"
	@echo "******************* make check not run here: configure --with-jovial ************************"
	@echo "*********************************************************************************************"
endif

check-local: conditional-check-local

clean-local:	
	rm -f $(PASSING_BASIC_JOVIAL_TEST_OBJECTS) *.c *.jov *.dot

EXTRA_DIST = $(ALL_TESTCODES) 

