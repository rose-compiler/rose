
include ../../makefile.inc

include make_rule.inc

all-am: $(MATHS_OBJS)

check:
	

docs:
	

clean:
	rm -f $(obj_dir)/maths/*
	rm -rf $(obj_dir)/maths/.libs

EXTRA_DIST= make_rule.inc Exception-maths.cpp PolyhedronExploration.cpp Quast.cpp biLinear.cpp Farkas.cpp \
../../include/maths/Farkas.hpp \
../../include/maths/PPLUtils.hpp \
../../include/maths/biLinear.hpp \
../../include/maths/PolyhedronExploration.hpp \
../../include/maths/Quast.hpp \
../../include/maths/Exception-maths.hpp \
../../tpl/maths/Quast.tpp
