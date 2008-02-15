
EXTRA_DIST = TestDriver Container.annot  UserDefList2b.C.out.save  UserDefList2.C.out.save   \
             UserDefList3.C.out.save  UserDefList2.C  UserDefList2d.C   UserDefList.C \
             UserDefList2c.C  UserDefList2d.C.out.save  UserDefList.C.out.save \
             UserDefList2b.C  UserDefList2c.C.out.save  UserDefList3.C  UserDefList.h

# to deactivate tests comment out check-local
check-local:
	@echo "SKIPPING TEST CODES RUN BY \"make check\" rule in projects/containerParallelizer/test directory ..."
	$(MAKE) FORCE_TEST_CODES_TO_RUN
	@echo "******************************************************************************************************"
	@echo "****** ROSE/projects/containerParallelizer/test: make check rule complete (terminated normally) ******"
	@echo "******************************************************************************************************"

FORCE_TEST_CODES_TO_RUN:
	$(srcdir)/TestDriver $(srcdir)

