
SCOPLIB_OBJS = $(obj_dir)/scoplib/Exception-ScopLib.lo

SCOPLIB_DEPS = $(inc_dir)/scoplib/Exception-ScopLib.hpp

if ROSE_USE_CLOOG
  SCOPLIB_DEPS += $(inc_dir)/scoplib/Cloog.hpp \
		$(tpl_dir)/scoplib/Cloog.tpp
endif
if ROSE_USE_SCOPLIB
  SCOPLIB_DEPS += $(inc_dir)/scoplib/ScopLib.hpp \
		$(tpl_dir)/scoplib/ScopLib.tpp
endif
if ROSE_USE_CANDL
  SCOPLIB_DEPS += $(inc_dir)/scoplib/Candl.hpp \
		$(tpl_dir)/scoplib/Candl.tpp
endif

$(obj_dir)/scoplib/Exception-ScopLib.lo: $(src_dir)/scoplib/Exception-ScopLib.cpp $(inc_dir)/scoplib/Exception-ScopLib.hpp
	$(LT_CXX) $(CXX_OPT) $(CXX_INC) -c $(src_dir)/scoplib/Exception-ScopLib.cpp -o $(obj_dir)/scoplib/Exception-ScopLib.lo

