include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ (9/11/2006): Remove these subdirectories
# SUBDIRS = A++Tests P++Tests

# Removed conditional compilation for ROSE_PERFORMANCE_TESTS
# It seems that a conditional can be use to setup SUB DIRS within Makefile.am
# if ROSE_PERFORMANCE_TESTS
# #  SUBDIRS = BenchmarkBase A++Tests P++Tests
#    SUBDIRS = A++Tests P++Tests
# else
#     # Note the we need the "." to make the dist rule work!
#     # SUBDIRS = .
#     SUBDIRS = A++Tests P++Tests
# endif

# SUBDIRS = BenchmarkBase A++Tests P++Tests

# This alows us to make distributions that could exclude the PerformanceTests directory
# I don't know if this is a good idea but it also allows us to test ROSE without the
# PerformanceTests directory as well.
# DIST_SUBDIRS = $(SUBDIRS)

# clean:
# if ROSE_PERFORMANCE_TESTS
#	cd BenchmarkBase; $(MAKE) clean;
#	cd A++Tests; $(MAKE) clean;
#	cd P++Tests; $(MAKE) clean;
# endif

