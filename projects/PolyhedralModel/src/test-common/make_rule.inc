
TEST_COMMON_OBJS = $(obj_dir)/test-common/PolyhedricDependency-test.lo \
				$(obj_dir)/test-common/PolyhedricContainer-test.lo

TEST_COMMON_DEPS = $(inc_dir)/test-common/ScopTree-test.hpp \
				$(inc_dir)/test-common/PolyhedricDependency-test.hpp \
				$(inc_dir)/test-common/PolyhedricContainer-test.hpp

$(obj_dir)/test-common/PolyhedricContainer-test.lo: $(src_dir)/test-common/PolyhedricContainer-test.cpp $(TEST_COMMON_DEPS) $(COMMON_DEPS) $(MATHS_DEPS)
	$(LT_CXX) $(CXX_OPT) $(CXX_INC) -c $(src_dir)/test-common/PolyhedricContainer-test.cpp -o $(obj_dir)/test-common/PolyhedricContainer-test.lo

$(obj_dir)/test-common/PolyhedricDependency-test.lo: $(src_dir)/test-common/PolyhedricDependency-test.cpp $(TEST_COMMON_DEPS) $(COMMON_DEPS) $(MATHS_DEPS)
	$(LT_CXX) $(CXX_OPT) $(CXX_INC) -c $(src_dir)/test-common/PolyhedricDependency-test.cpp -o $(obj_dir)/test-common/PolyhedricDependency-test.lo

$(obj_dir)/test-common/matmul.lo: $(src_dir)/test-common/matmul.cpp $(TEST_COMMON_DEPS) $(COMMON_DEPS) $(MATHS_DEPS)
	$(LT_CXX) $(CXX_OPT) $(CXX_INC) -c $(src_dir)/test-common/matmul.cpp -o $(obj_dir)/test-common/matmul.lo

$(obj_dir)/test-common/matcolsum.lo: $(src_dir)/test-common/matcolsum.cpp $(TEST_COMMON_DEPS) $(COMMON_DEPS) $(MATHS_DEPS)
	$(LT_CXX) $(CXX_OPT) $(CXX_INC) -c $(src_dir)/test-common/matcolsum.cpp -o $(obj_dir)/test-common/matcolsum.lo

$(obj_dir)/test-common/matsum.lo: $(src_dir)/test-common/matsum.cpp $(TEST_COMMON_DEPS) $(COMMON_DEPS) $(MATHS_DEPS)
	$(LT_CXX) $(CXX_OPT) $(CXX_INC) -c $(src_dir)/test-common/matsum.cpp -o $(obj_dir)/test-common/matsum.lo

$(bin_dir)/matmul: $(obj_dir)/test-common/matmul.lo $(TEST_COMMON_OBJS) $(COMMON_OBJS) $(MATHS_OBJS)
	$(LT_LINK) $(CXX_OPT) $(obj_dir)/test-common/matmul.lo $(TEST_COMMON_OBJS) $(COMMON_OBJS) $(MATHS_OBJS) $(CXX_LIB) -o $(bin_dir)/matmul

$(bin_dir)/matcolsum: $(obj_dir)/test-common/matcolsum.lo $(TEST_COMMON_OBJS) $(COMMON_OBJS) $(MATHS_OBJS)
	$(LT_LINK) $(CXX_OPT) $(obj_dir)/test-common/matcolsum.lo $(TEST_COMMON_OBJS) $(COMMON_OBJS) $(MATHS_OBJS) $(CXX_LIB) -o $(bin_dir)/matcolsum

$(bin_dir)/matsum: $(obj_dir)/test-common/matsum.lo $(TEST_COMMON_OBJS) $(COMMON_OBJS) $(MATHS_OBJS)
	$(LT_LINK) $(CXX_OPT) $(obj_dir)/test-common/matsum.lo $(TEST_COMMON_OBJS) $(COMMON_OBJS) $(MATHS_OBJS) $(CXX_LIB) -o $(bin_dir)/matsum

