
MATHS_OBJS = \
$(obj_dir)/maths/Farkas.lo \
$(obj_dir)/maths/biLinear.lo \
$(obj_dir)/maths/Quast.lo \
$(obj_dir)/maths/PolyhedronExploration.lo \
$(obj_dir)/maths/Exception-maths.lo

MATHS_DEPS = \
$(inc_dir)/maths/Farkas.hpp \
$(inc_dir)/maths/biLinear.hpp \
$(inc_dir)/maths/Quast.hpp \
$(tpl_dir)/maths/Quast.tpp \
$(inc_dir)/maths/PolyhedronExploration.hpp \
$(inc_dir)/maths/Exception-maths.hpp

$(obj_dir)/maths/Farkas.lo: $(src_dir)/maths/Farkas.cpp $(inc_dir)/maths/Farkas.hpp
	$(LT_CXX) $(CXX_OPT) $(CXX_INC) -c $(src_dir)/maths/Farkas.cpp -o $(obj_dir)/maths/Farkas.lo

$(obj_dir)/maths/biLinear.lo: $(src_dir)/maths/biLinear.cpp $(inc_dir)/maths/biLinear.hpp
	$(LT_CXX) $(CXX_OPT) $(CXX_INC) -c $(src_dir)/maths/biLinear.cpp -o $(obj_dir)/maths/biLinear.lo

$(obj_dir)/maths/Quast.lo: $(src_dir)/maths/Quast.cpp $(inc_dir)/maths/Quast.hpp
	$(LT_CXX) $(CXX_OPT) $(CXX_INC) -c $(src_dir)/maths/Quast.cpp -o $(obj_dir)/maths/Quast.lo

$(obj_dir)/maths/PolyhedronExploration.lo: $(src_dir)/maths/PolyhedronExploration.cpp $(inc_dir)/maths/PolyhedronExploration.hpp
	$(LT_CXX) $(CXX_OPT) $(CXX_INC) -c $(src_dir)/maths/PolyhedronExploration.cpp -o $(obj_dir)/maths/PolyhedronExploration.lo

$(obj_dir)/maths/Exception-maths.lo: $(src_dir)/maths/Exception-maths.cpp $(inc_dir)/maths/Exception-maths.hpp
	$(LT_CXX) $(CXX_OPT) $(CXX_INC) -c $(src_dir)/maths/Exception-maths.cpp -o $(obj_dir)/maths/Exception-maths.lo

