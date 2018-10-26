
MAP_SCRIPT=$(top_srcdir)/projects/CompilationDB/scripts/comp_db_map.py
RENDER_SCRIPT=$(top_srcdir)/projects/CompilationDB/scripts/comp_db_render.py

$(abs_builddir)/doxygen-src/CMakeLists.txt:
	git clone --recursive https://github.com/doxygen/doxygen.git $(abs_builddir)/doxygen-src

$(abs_builddir)/doxygen-build/compile_commands.json: $(abs_builddir)/doxygen-src/CMakeLists.txt
	( \
	  mkdir -p $(abs_builddir)/doxygen-build ; \
	  cd $(abs_builddir)/doxygen-build ; \
	  cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON $(abs_builddir)/doxygen-src ; \
	  make -f ./src/CMakeFiles/doxycfg.dir/build.make generated_src/configvalues.h ; \
	  make -f ./src/CMakeFiles/doxycfg.dir/build.make generated_src/lang_cfg.h ; \
	  make -f ./src/CMakeFiles/doxycfg.dir/build.make generated_src/configvalues.cpp ; \
	  make -f ./src/CMakeFiles/doxycfg.dir/build.make generated_src/configimpl.cpp ; \
	  make -f ./src/CMakeFiles/doxycfg.dir/build.make generated_src/configoptions.cpp ; \
	  make -f ./src/CMakeFiles/_doxygen.dir/build.make generated_src/layout_default.xml.h ; \
	  make -f ./src/CMakeFiles/_doxygen.dir/build.make generated_src/resources.cpp ; \
	  make -f ./src/CMakeFiles/_doxygen.dir/build.make generated_src/scanner.cpp ; \
	  make -f ./src/CMakeFiles/_doxygen.dir/build.make generated_src/code.cpp ; \
	  make -f ./src/CMakeFiles/_doxygen.dir/build.make generated_src/pyscanner.cpp ; \
	  make -f ./src/CMakeFiles/_doxygen.dir/build.make generated_src/pycode.cpp ; \
	  make -f ./src/CMakeFiles/_doxygen.dir/build.make generated_src/fortranscanner.cpp ; \
	  make -f ./src/CMakeFiles/_doxygen.dir/build.make generated_src/fortrancode.cpp ; \
	  make -f ./src/CMakeFiles/_doxygen.dir/build.make generated_src/vhdlcode.cpp ; \
	  make -f ./src/CMakeFiles/_doxygen.dir/build.make generated_src/tclscanner.cpp ; \
	  make -f ./src/CMakeFiles/_doxygen.dir/build.make generated_src/pre.cpp ; \
	  make -f ./src/CMakeFiles/_doxygen.dir/build.make generated_src/declinfo.cpp ; \
	  make -f ./src/CMakeFiles/_doxygen.dir/build.make generated_src/defargs.cpp ; \
	  make -f ./src/CMakeFiles/_doxygen.dir/build.make generated_src/doctokenizer.cpp ; \
	  make -f ./src/CMakeFiles/_doxygen.dir/build.make generated_src/commentcnv.cpp ; \
	  make -f ./src/CMakeFiles/_doxygen.dir/build.make generated_src/commentscan.cpp ; \
	  make -f ./src/CMakeFiles/_doxygen.dir/build.make generated_src/constexp.cpp ; \
	  make -f ./src/CMakeFiles/_doxygen.dir/build.make generated_src/xmlcode.cpp ; \
	  make -f ./src/CMakeFiles/_doxygen.dir/build.make generated_src/sqlcode.cpp ; \
	  make -f ./src/CMakeFiles/_doxygen.dir/build.make generated_src/ce_parse.h ; \
	  make -f ./src/CMakeFiles/_doxygen.dir/build.make generated_src/ce_parse.cpp \
	)

# testTranslator

$(abs_top_builddir)/tests/nonsmoke/functional/testTranslator:
	make -C $(abs_top_builddir)/tests/nonsmoke/functional testTranslator

$(abs_builddir)/doxygen-build/testTranslator.json: $(abs_builddir)/doxygen-build/compile_commands.json $(MAP_SCRIPT) $(abs_top_builddir)/tests/nonsmoke/functional/testTranslator
	$(MAP_SCRIPT) $(abs_builddir)/doxygen-src $(abs_builddir)/doxygen-build $(abs_top_builddir)/tests/nonsmoke/functional/testTranslator

$(abs_builddir)/doxygen-build/testTranslator.html: $(abs_builddir)/doxygen-build/testTranslator.json $(RENDER_SCRIPT)
	$(RENDER_SCRIPT) --report $(abs_builddir)/doxygen-build/testTranslator.json --title "Doxygen"

# genInstFlowGraph

$(abs_top_builddir)/projects/TemplateAnalysis/src/genInstFlowGraph:
	make -C $(abs_top_builddir)/projects/TemplateAnalysis/src genInstFlowGraph

$(abs_builddir)/doxygen-build/genInstFlowGraph.json: $(abs_builddir)/doxygen-build/compile_commands.json $(MAP_SCRIPT) $(abs_top_builddir)/projects/TemplateAnalysis/src/genInstFlowGraph
	$(MAP_SCRIPT) $(abs_builddir)/doxygen-src $(abs_builddir)/doxygen-build $(abs_top_builddir)/projects/TemplateAnalysis/src/genInstFlowGraph -- --namespace-filter-for-analysis=std --namespace-filter-for-analysis=__gnu_cxx --namespace-filter-for-graphviz=std --namespace-filter-for-graphviz=__gnu_cxx

$(abs_builddir)/doxygen-build/genInstFlowGraph.html: $(abs_builddir)/doxygen-build/genInstFlowGraph.json $(RENDER_SCRIPT)
	$(RENDER_SCRIPT) --report $(abs_builddir)/doxygen-build/genInstFlowGraph.json --title "Doxygen" --addons '[ { "function" : "graphviz" , "kwargs" : { "title" : "Template Instantiation Flow" , "filename" : "%d/%fe.dot" } } ]'

# buildCallGraph

$(abs_top_builddir)/tutorial/buildCallGraph:
	make -C $(abs_top_builddir)/tutorial buildCallGraph

$(abs_builddir)/doxygen-build/buildCallGraph.json: $(abs_builddir)/doxygen-build/compile_commands.json $(MAP_SCRIPT) $(abs_top_builddir)/tutorial/buildCallGraph
	$(MAP_SCRIPT) $(abs_builddir)/doxygen-src $(abs_builddir)/doxygen-build $(abs_top_builddir)/tutorial/buildCallGraph -- --namespace-filter-for-analysis=std --namespace-filter-for-analysis=__gnu_cxx --namespace-filter-for-graphviz=std --namespace-filter-for-graphviz=__gnu_cxx

$(abs_builddir)/doxygen-build/buildCallGraph.html: $(abs_builddir)/doxygen-build/buildCallGraph.json $(RENDER_SCRIPT)
	$(RENDER_SCRIPT) --report $(abs_builddir)/doxygen-build/buildCallGraph.json --title "Doxygen" --addons '[ { "function" : "graphviz" , "kwargs" : { "title" : "Call Graph" , "filename" : "%d/%fe_callGraph.dot" } } ]'

########

check-local: \
  $(abs_builddir)/doxygen-build/testTranslator.html \
  $(abs_builddir)/doxygen-build/buildCallGraph.html \
  $(abs_builddir)/doxygen-build/genInstFlowGraph.html

clean-local:
#	rm -rf $(abs_builddir)/doxygen-src
	rm -rf $(abs_builddir)/doxygen-build
	rm -rf $(abs_builddir)/doxygen-build/testTranslator.json
	rm -rf $(abs_builddir)/doxygen-build/genInstFlowGraph.json

