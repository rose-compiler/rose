
SCHEDSPACE_OBJS = $(obj_dir)/common/ScheduleSpace.lo

EXCEPTION_OBJS = $(obj_dir)/common/Exception.lo

COMMON_OBJS = \
$(SCHEDSPACE_OBJS) \
$(EXCEPTION_OBJS)

CONTAINER_DEPS = \
$(inc_dir)/common/PolyhedricContainer.hpp \
$(tpl_dir)/common/PolyhedricContainer.tpp

SCOPTREE_DEPS = \
$(inc_dir)/common/ScopTree.hpp \
$(tpl_dir)/common/ScopTree.tpp

DEPENDENCY_DEPS = \
$(inc_dir)/common/PolyhedricDependency.hpp \
$(tpl_dir)/common/PolyhedricDependency.tpp

SCHEDULE_DEPS = \
$(inc_dir)/common/Schedule.hpp \
$(tpl_dir)/common/Schedule.tpp

SCHEDSPACE_DEPS = \
$(inc_dir)/common/ScheduleSpace.hpp \
$(tpl_dir)/common/ScheduleSpace.tpp

EXCEPTION_DEPS = \
$(inc_dir)/common/Exception.hpp \
$(tpl_dir)/common/Exception.tpp

COMMON_DEPS = \
$(CONTAINER_DEPS) \
$(SCOPTREE_DEPS) \
$(DEPENDENCY_DEPS) \
$(SCHEDULE_DEPS) \
$(SCHEDSPACE_DEPS) \
$(EXCEPTION_DEPS)

$(obj_dir)/common/Exception.lo: $(src_dir)/common/Exception.cpp $(inc_dir)/common/Exception.hpp
	$(LT_CXX) $(CXX_OPT) $(CXX_INC) -c $(src_dir)/common/Exception.cpp -o $(obj_dir)/common/Exception.lo

$(obj_dir)/common/ScheduleSpace.lo: $(src_dir)/common/ScheduleSpace.cpp $(inc_dir)/common/ScheduleSpace.hpp
	$(LT_CXX) $(CXX_OPT) $(CXX_INC) -c $(src_dir)/common/ScheduleSpace.cpp -o $(obj_dir)/common/ScheduleSpace.lo

