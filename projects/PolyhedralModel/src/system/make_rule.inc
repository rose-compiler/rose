
SYSTEM_OBJS = $(obj_dir)/system/Utils.o

SYSTEM_DEPS = $(inc_dir)/system/Utils.hpp

$(obj_dir)/system/Utils.o: $(src_dir)/system/Utils.cpp $(inc_dir)/system/Utils.hpp
	$(LT_CXX) $(CXX_OPT) $(CXX_INC) -c $(src_dir)/system/Utils.cpp -o $(obj_dir)/system/Utils.o

