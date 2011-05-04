
ROSE_PRAGMA_OBJS = \
$(obj_dir)/rose-pragma/PolyhedricPragma.lo\
$(obj_dir)/rose-pragma/Exception-rose-pragma.lo \
$(obj_dir)/rose-pragma/PolyhedricPragmaParser.lo

ROSE_PRAGMA_DEPS = \
$(inc_dir)/rose-pragma/PolyhedricPragma.hpp \
$(inc_dir)/rose-pragma/Exception-rose-pragma.hpp \
$(inc_dir)/rose-pragma/PolyhedricPragmaParser.hpp

$(obj_dir)/rose-pragma/Exception-rose-pragma.lo: $(src_dir)/rose-pragma/Exception-rose-pragma.cpp \
						$(EXCEPTION_DEPS) $(inc_dir)/rose/Exception-rose.hpp $(inc_dir)/rose-pragma/Exception-rose-pragma.hpp
	$(LT_CXX) $(CXX_OPT) $(CXX_INC) $(CXX_INC_ROSE) -c $(src_dir)/rose-pragma/Exception-rose-pragma.cpp -o $(obj_dir)/rose-pragma/Exception-rose-pragma.lo

$(obj_dir)/rose-pragma/PolyhedricPragma.lo: $(src_dir)/rose-pragma/PolyhedricPragma.cpp $(inc_dir)/rose-pragma/PolyhedricPragma.hpp \
						$(COMMON_DEPS) $(ROSE_DEPS) $(inc_dir)/rose-pragma/Exception-rose-pragma.hpp
	$(LT_CXX) $(CXX_OPT) $(CXX_INC) $(CXX_INC_ROSE) $(CXX_INC_PARSER) -c $(src_dir)/rose-pragma/PolyhedricPragma.cpp -o $(obj_dir)/rose-pragma/PolyhedricPragma.lo

$(obj_dir)/rose-pragma/PolyhedricPragmaParser.lo: $(src_dir)/rose-pragma/PolyhedricPragmaParser.cpp $(inc_dir)/rose-pragma/PolyhedricPragmaParser.hpp \
						$(COMMON_DEPS) $(ROSE_DEPS)
	$(LT_CXX) $(CXX_OPT) $(CXX_INC) $(CXX_INC_ROSE) $(CXX_INC_PARSER) -c $(src_dir)/rose-pragma/PolyhedricPragmaParser.cpp -o $(obj_dir)/rose-pragma/PolyhedricPragmaParser.lo

