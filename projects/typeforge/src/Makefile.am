include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = typeforge

AM_LDFLAGS = $(ROSE_LIBS) $(LIBS_WITH_RPATH)
AM_CPPFLAGS = $(ROSE_INCLUDES)

TF_REL_PATH=projects/typeforge
TF_INC_PATH=

TF_LIB=$(abs_top_builddir)/$(TF_REL_PATH)/lib/libTypeforge.la
$(TF_LIB):
	$(MAKE) -C $(abs_top_builddir)/$(TF_REL_PATH)/lib libTypeforge.la

TF_TRANSFORM_LIB=$(abs_top_builddir)/$(TF_REL_PATH)/lib/libTypeforge-transform.la
$(TF_TRANSFORM_LIB):
	$(MAKE) -C $(abs_top_builddir)/$(TF_REL_PATH)/lib libTypeforge-transform.la

TF_CAST_LIB=$(abs_top_builddir)/$(TF_REL_PATH)/lib/libTypeforge-cast.la
$(TF_CAST_LIB):
	$(MAKE) -C $(abs_top_builddir)/$(TF_REL_PATH)/lib libTypeforge-cast.la

TF_UTILS_LIB=$(abs_top_builddir)/$(TF_REL_PATH)/lib/libTypeforge-utils.la
$(TF_UTILS_LIB):
	$(MAKE) -C $(abs_top_builddir)/$(TF_REL_PATH)/lib libTypeforge-utils.la

TF_OPNET_LIB=$(abs_top_builddir)/$(TF_REL_PATH)/lib/OpNet/libTypeforge-opnet.la
$(TF_OPNET_LIB):
	$(MAKE) -C $(abs_top_builddir)/$(TF_REL_PATH)/lib/OpNet libTypeforge-opnet.la

typeforge_CXXFLAGS= \
  -I$(abs_top_srcdir)/$(TF_REL_PATH)/include \
  -g -Wall -O3 -march=native -ftree-vectorize \
  -rdynamic

typeforge_SOURCES=typeforge.cpp
typeforge_LDADD= \
  $(TF_LIB) \
  $(TF_OPNET_LIB) \
  $(TF_TRANSFORM_LIB) \
  $(TF_CAST_LIB) \
  $(TF_UTILS_LIB)

