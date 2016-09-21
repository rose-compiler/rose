include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#===============================================================================
if ROSE_BUILD_X10_LANGUAGE_SUPPORT
#===============================================================================
ROSE_TRANSLATOR = $(top_builddir)/tests/nonsmoke/functional/testTranslator
$(ROSE_TRANSLATOR):
	@$(MAKE) testTranslator -C $(top_builddir)/tests

ROSE_FLAGS =

TESTCODES = \
	HelloWholeWorld.x10

# Warning: Use -rose:output to unparse file in build tree, rather
# than override the original file in the source tree.
$(TESTCODES:.x10=.compile): $(ROSE_TRANSLATOR)
	$(ROSE_TRANSLATOR) \
			$(ROSE_FLAGS) \
			"$(srcdir)/$(@:.compile=.x10)" \
			-rose:output "$(@:.compile=.x10)"

check-local:
	@$(MAKE) $(TESTCODES:.x10=.compile)
	@echo "***********************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/x10_tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************"

clean-local:
	rm -f *.java
	rm -f *.class
	rm -f *.x10

#===============================================================================
else
#===============================================================================
check-local:
	@echo "Skipping x10_tests because X10 language support is not enabled, see ROSE/configure --help"
	@echo "***********************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/CompileTests/x10_tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************"
#===============================================================================
endif
#===============================================================================
