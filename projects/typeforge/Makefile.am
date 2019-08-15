EXTRA_DIST=README

.PHONY: docs
docs:
	make -C docs docs

# excluding old versions of boost
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
SUBDIRS=src tests docs
endif
endif
endif
endif
endif
