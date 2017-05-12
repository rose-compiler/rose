EXTRA_DIST=COPYING LICENSE README VERSION_HISTORY

# DQ (9/12/2013): We need a way to exclude older versions of Boost
# without accepting each specific version of boost.
if ROSE_USING_BOOST_VERSION_1_40
else
if ROSE_USING_BOOST_VERSION_1_41
else
if ROSE_USING_BOOST_VERSION_1_42
else
if ROSE_USING_BOOST_VERSION_1_43
else
if ROSE_USING_BOOST_VERSION_1_44
else

SUBDIRS=src
# demo target (for plugin demos)
demo:
	make -C src demo

endif
endif
endif
endif
endif
