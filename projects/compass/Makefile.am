include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ (10/17/2010): This seems to be problem for Java, we can fix this later.
# DQ (4/17/2016): Restrict this to requiring both C/C++ language support and Binary analysis support.
if !ROSE_USING_ROSE_CODE_GENERATION
if !ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
if ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
if !ROSE_BUILD_COMPASS2

SUBDIRS = src tools

EXTRA_DIST = CMakeLists.txt extensions

check-local:
	@echo "***************************************************************************"
	@echo "****** ROSE/projects/compass: make check rule complete (terminated normally) ******"
	@echo "***************************************************************************"

endif
endif
endif
endif
endif
