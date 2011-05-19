
$(obj_dir)/misc-test/quast.lo: $(src_dir)/misc-test/quast.cpp $(inc_dir)/maths/Quast.hpp $(tpl_dir)/maths/Quast.tpp
	$(LT_CXX) $(CXX_OPT) $(CXX_INC) -c $(src_dir)/misc-test/quast.cpp -o $(obj_dir)/misc-test/quast.lo

$(bin_dir)/quast: $(obj_dir)/misc-test/quast.lo $(obj_dir)/maths/Quast.lo $(obj_dir)/common/Exception.lo $(obj_dir)/maths/Quast.lo $(obj_dir)/maths/Exception-maths.lo
	$(LT_LINK) $(CXX_OPT) $(obj_dir)/misc-test/quast.lo $(obj_dir)/common/Exception.lo $(obj_dir)/maths/Quast.lo $(obj_dir)/maths/Exception-maths.lo $(CXX_LIB) -o $(bin_dir)/quast
	
$(obj_dir)/misc-test/explorer.lo: $(src_dir)/misc-test/explorer.cpp $(inc_dir)/maths/PolyhedronExploration.hpp
	$(LT_CXX) $(CXX_OPT) $(CXX_INC) -c $(src_dir)/misc-test/explorer.cpp -o $(obj_dir)/misc-test/explorer.lo

$(bin_dir)/explorer: $(obj_dir)/misc-test/explorer.lo $(obj_dir)/maths/PolyhedronExploration.lo
	$(LT_LINK) $(CXX_OPT) $(obj_dir)/misc-test/explorer.lo $(obj_dir)/maths/PolyhedronExploration.lo $(CXX_LIB) -o $(bin_dir)/explorer

