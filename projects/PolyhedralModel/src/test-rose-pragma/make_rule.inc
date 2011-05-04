
######################################
# Test Sage to Polyhedric Annotation #
######################################

$(obj_dir)/test-rose-pragma/test-polyhedric-annotation.lo: $(src_dir)/test-rose-pragma/test-polyhedric-annotation.cpp \
							$(ROSE_DEPS) \
							$(ROSE_PRAGMA_DEPS) \
							$(CONTAINER_DEPS) \
							$(SCOPTREE_DEPS) \
							$(EXCEPTION_DEPS)
	$(LT_CXX) $(CXX_OPT) $(CXX_INC) $(CXX_INC_PPL) $(CXX_INC_ROSE) \
				-c $(src_dir)/test-rose-pragma/test-polyhedric-annotation.cpp \
				-o $(obj_dir)/test-rose-pragma/test-polyhedric-annotation.lo
	
$(bin_dir)/test-rose-pragma-polyhedric-annotation: $(obj_dir)/test-rose-pragma/test-polyhedric-annotation.lo \
							$(ROSE_OBJS) \
							$(ROSE_PRAGMA_OBJS) \
							$(EXCEPTION_OBJS) \
							$(PARSER_OBJS)
	$(LT_LINK) $(CXX_OPT) $(CXX_LIB) $(CXX_LIB_PPL) $(CXX_LIB_ROSE) $(obj_dir)/test-rose-pragma/test-polyhedric-annotation.lo \
						$(ROSE_OBJS) $(ROSE_PRAGMA_OBJS) $(EXCEPTION_OBJS) $(PARSER_OBJS) \
				-o $(bin_dir)/test-rose-pragma-polyhedric-annotation

###########################
# Test Sage to Dependency #
###########################

$(obj_dir)/test-rose-pragma/test-dependency.lo: $(src_dir)/test-rose-pragma/test-dependency.cpp \
							$(ROSE_DEPS) \
							$(ROSE_PRAGMA_DEPS) \
							$(CONTAINER_DEPS) \
							$(SCOPTREE_DEPS) \
							$(EXCEPTION_DEPS) \
							$(MATHS_DEPS)
	$(LT_CXX) $(CXX_OPT) $(CXX_INC) $(CXX_INC_PPL) $(CXX_INC_ROSE) \
				-c $(src_dir)/test-rose-pragma/test-dependency.cpp \
				-o $(obj_dir)/test-rose-pragma/test-dependency.lo

$(bin_dir)/test-rose-pragma-dependency: $(obj_dir)/test-rose-pragma/test-dependency.lo \
							$(ROSE_OBJS) \
							$(ROSE_PRAGMA_OBJS) \
							$(EXCEPTION_OBJS) \
							$(MATHS_OBJS) \
							$(PARSER_OBJS)
	$(LT_LINK) $(CXX_OPT) $(CXX_LIB) $(CXX_LIB_PPL) $(CXX_LIB_ROSE) $(obj_dir)/test-rose-pragma/test-dependency.lo \
						$(ROSE_OBJS) $(ROSE_PRAGMA_OBJS) $(MATHS_OBJS) $(EXCEPTION_OBJS) $(PARSER_OBJS) \
				-o $(bin_dir)/test-rose-pragma-dependency

#############################
# Test Sage Code Generation #
#############################

$(obj_dir)/test-rose-pragma/test-code-generation.lo: $(src_dir)/test-rose-pragma/test-code-generation.cpp \
							$(ROSE_DEPS) \
							$(ROSE_PRAGMA_DEPS) \
							$(CONTAINER_DEPS) \
							$(SCOPTREE_DEPS) \
							$(CODE_GEN_DEPS) \
							$(EXCEPTION_DEPS) \
							$(MATHS_DEPS)
	$(LT_CXX) $(CXX_OPT) $(CXX_INC) $(CXX_INC_PPL) $(CXX_INC_ROSE) \
				-c $(src_dir)/test-rose-pragma/test-code-generation.cpp \
				-o $(obj_dir)/test-rose-pragma/test-code-generation.lo
	
$(bin_dir)/test-rose-pragma-code-generation: $(obj_dir)/test-rose-pragma/test-code-generation.lo \
							$(ROSE_OBJS) \
							$(ROSE_PRAGMA_OBJS) \
							$(EXCEPTION_OBJS) \
							$(MATHS_OBJS) \
							$(PARSER_OBJS)
	$(LT_LINK) $(CXX_OPT) $(CXX_LIB) $(CXX_LIB_PPL) $(CXX_LIB_ROSE) $(obj_dir)/test-rose-pragma/test-code-generation.lo \
						$(ROSE_OBJS) $(ROSE_PRAGMA_OBJS) $(MATHS_OBJS) $(EXCEPTION_OBJS) $(PARSER_OBJS) \
				-o $(bin_dir)/test-rose-pragma-code-generation

###########################################
# Test Sage All Schedules Code Generation #
###########################################

$(obj_dir)/test-rose-pragma/test-generate-all-schedules.lo: $(src_dir)/test-rose-pragma/test-generate-all-schedules.cpp \
							$(ROSE_DEPS) \
							$(ROSE_PRAGMA_DEPS) \
							$(CONTAINER_DEPS) \
							$(SCOPTREE_DEPS) \
							$(CODE_GEN_DEPS) \
							$(SCHED_DEPS) \
							$(SCHEDSPACE_DEPS) \
							$(EXCEPTION_DEPS) \
							$(MATHS_DEPS)
	$(LT_CXX) $(CXX_OPT) $(CXX_INC) $(CXX_INC_PPL) $(CXX_INC_ROSE) \
				-c $(src_dir)/test-rose-pragma/test-generate-all-schedules.cpp \
				-o $(obj_dir)/test-rose-pragma/test-generate-all-schedules.lo
	
$(bin_dir)/test-rose-pragma-generate-all-schedules: $(obj_dir)/test-rose-pragma/test-generate-all-schedules.lo \
							$(ROSE_OBJS) \
							$(ROSE_PRAGMA_OBJS) \
							$(EXCEPTION_OBJS) \
							$(SCHEDSPACE_OBJS) \
							$(MATHS_OBJS) \
							$(SYSTEM_OBJS) \
							$(PARSER_OBJS)
	$(LT_LINK) $(CXX_OPT) $(CXX_LIB) $(CXX_LIB_PPL) $(CXX_LIB_ROSE) $(obj_dir)/test-rose-pragma/test-generate-all-schedules.lo \
						$(ROSE_OBJS) $(ROSE_PRAGMA_OBJS) $(SCHEDSPACE_OBJS) $(MATHS_OBJS) $(EXCEPTION_OBJS) $(PARSER_OBJS) $(SYSTEM_OBJS) \
				-o $(bin_dir)/test-rose-pragma-generate-all-schedules

