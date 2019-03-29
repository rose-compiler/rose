include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#SUBDIRS = compass compassVerifier sampleCompassSubset 
SUBDIRS = compassVerifier compass

if !ROSE_USING_GCC_VERSION_EQUAL_4_9_1
SUBDIRS+=sampleCompassSubset
endif

check-local:
	@echo "***************************************************************************"
	@echo "****** ROSE/projects/Compass/tools: make check rule complete (terminated normally) ******"
	@echo "***************************************************************************"
