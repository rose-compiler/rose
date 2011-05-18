
ROSE_OBJS = \
$(obj_dir)/rose/Exception-rose.lo \
$(obj_dir)/rose/Parser.lo \
$(obj_dir)/rose/rose-utils.lo \
$(obj_dir)/rose/Variable.lo \
$(obj_dir)/rose/Quast-rose.lo

ROSE_DEPS = \
$(inc_dir)/rose/Parser.hpp \
$(tpl_dir)/rose/Parser.tpp \
$(inc_dir)/rose/Attribute.hpp \
$(tpl_dir)/rose/Attribute.tpp \
$(inc_dir)/rose/Variable.hpp \
$(inc_dir)/rose/rose-utils.hpp \
$(inc_dir)/rose/Exception-rose.hpp

if ROSE_USE_CLOOG
  ROSE_DEPS += $(inc_dir)/rose/CodeGeneration.hpp $(tpl_dir)/rose/CodeGeneration.tpp
endif

$(obj_dir)/rose/Exception-rose.lo: $(src_dir)/rose/Exception-rose.cpp\
						$(EXCEPTION_DEPS) $(inc_dir)/rose/Exception-rose.hpp
	$(LT_CXX) $(CXX_OPT) $(CXX_INC) $(CXX_INC_ROSE) -c $(src_dir)/rose/Exception-rose.cpp -o $(obj_dir)/rose/Exception-rose.lo

$(obj_dir)/rose/Parser.lo: $(src_dir)/rose/Parser.cpp $(inc_dir)/rose/Parser.hpp $(inc_dir)/rose/Attribute.hpp\
						$(SCOPTREE_DEPS) $(EXCEPTION_DEPS) $(inc_dir)/rose/Exception-rose.hpp
	$(LT_CXX) $(CXX_OPT) $(CXX_INC) $(CXX_INC_ROSE) -c $(src_dir)/rose/Parser.cpp -o $(obj_dir)/rose/Parser.lo

$(obj_dir)/rose/rose-utils.lo: $(src_dir)/rose/rose-utils.cpp $(inc_dir)/rose/rose-utils.hpp
	$(LT_CXX) $(CXX_OPT) $(CXX_INC) $(CXX_INC_ROSE) -c $(src_dir)/rose/rose-utils.cpp -o $(obj_dir)/rose/rose-utils.lo

$(obj_dir)/rose/Variable.lo: $(src_dir)/rose/Variable.cpp $(inc_dir)/rose/Variable.hpp
	$(LT_CXX) $(CXX_OPT) $(CXX_INC) $(CXX_INC_ROSE) -c $(src_dir)/rose/Variable.cpp -o $(obj_dir)/rose/Variable.lo

$(obj_dir)/rose/Quast-rose.lo: $(src_dir)/rose/Quast-rose.cpp $(DEPENDENCY_DEPS) $(MATHS_DEPS)
	$(LT_CXX) $(CXX_OPT) $(CXX_INC) $(CXX_INC_ROSE) -c $(src_dir)/rose/Quast-rose.cpp -o $(obj_dir)/rose/Quast-rose.lo

