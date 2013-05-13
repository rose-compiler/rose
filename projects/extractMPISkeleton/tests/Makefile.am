include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

EXEC=../../src/extractMPISkeleton

include Makefile_variables

check-local: ../src/extractMPISkeleton
	for f in ${EXTRA_DIST} ; do mkdir -p `dirname $$f` ; cp $(asrcdir)/$$f $$f ; done
	${MAKE} ${MARKS}
	${MAKE} ${ALL_OBJS}
	${MAKE} ${ALL_EXPECTED:.expected=.comp}
	@echo "***********************************************************************************************"
	@echo "*** ROSE/projects/extractMPISkeleton/tests: make check rule complete (terminated normally)"
	@echo "***********************************************************************************************"

clean-local:
	rm -rf ${DIRS}
