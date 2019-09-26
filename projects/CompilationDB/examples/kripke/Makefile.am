
MAP_SCRIPT=$(top_srcdir)/projects/CompilationDB/scripts/comp_db_map.py
RENDER_SCRIPT=$(top_srcdir)/projects/CompilationDB/scripts/comp_db_render.py

$(abs_builddir)/kripke-src/CMakeLists.txt:
	git clone --recursive -b release/v1.2.0-CORAL2 https://github.com/LLNL/Kripke.git $(abs_builddir)/kripke-src

$(abs_builddir)/kripke-sequential/compile_commands.json: $(abs_builddir)/kripke-src/CMakeLists.txt
	( \
	  mkdir -p $(abs_builddir)/kripke-sequential ; \
	  cd $(abs_builddir)/kripke-sequential ; \
	  cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DENABLE_OPENMP=False -DKRIPKE_ARCH=Sequential $(abs_builddir)/kripke-src \
	)

$(abs_builddir)/kripke-openmp/compile_commands.json: $(abs_builddir)/kripke-src
	( \
	  mkdir -p $(abs_builddir)/kripke-openmp ; \
	  cd $(abs_builddir)/kripke-openmp ; \
	  cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DENABLE_OPENMP=True $(abs_builddir)/kripke-src \
	)

# testTranslator

$(abs_top_builddir)/tests/nonsmoke/functional/testTranslator:
	make -C $(abs_top_builddir)/tests/nonsmoke/functional testTranslator

$(abs_builddir)/kripke-sequential/testTranslator.json: $(abs_builddir)/kripke-sequential/compile_commands.json $(MAP_SCRIPT) $(abs_top_builddir)/tests/nonsmoke/functional/testTranslator
	$(MAP_SCRIPT) $(abs_builddir)/kripke-src $(abs_builddir)/kripke-sequential $(abs_top_builddir)/tests/nonsmoke/functional/testTranslator --filter r:-std=c++11:-std=c++14 -- -rose:unparse_edg_normalized_method_ROSE_1392

$(abs_builddir)/kripke-openmp/testTranslator.json: $(abs_builddir)/kripke-openmp/compile_commands.json $(MAP_SCRIPT) $(abs_top_builddir)/tests/nonsmoke/functional/testTranslator
	$(MAP_SCRIPT) $(abs_builddir)/kripke-src $(abs_builddir)/kripke-openmp $(abs_top_builddir)/tests/nonsmoke/functional/testTranslator --filter r:-std=c++11:-std=c++14 -- -rose:unparse_edg_normalized_method_ROSE_1392 -D_OPENMP

$(abs_builddir)/kripke-sequential/testTranslator.html: $(abs_builddir)/kripke-sequential/testTranslator.json $(RENDER_SCRIPT)
	$(RENDER_SCRIPT) --report $(abs_builddir)/kripke-sequential/testTranslator.json --title "Kripke with Sequential"

$(abs_builddir)/kripke-openmp/testTranslator.html: $(abs_builddir)/kripke-openmp/testTranslator.json $(RENDER_SCRIPT)
	$(RENDER_SCRIPT) --report $(abs_builddir)/kripke-openmp/testTranslator.json --title "Kripke with OpenMP"

# rajaChecker

$(abs_top_builddir)/tools/rajaChecker:
	make -C $(abs_top_builddir)/tools rajaChecker

$(abs_builddir)/kripke-sequential/rajaChecker.json: $(abs_builddir)/kripke-sequential/compile_commands.json $(MAP_SCRIPT) $(abs_top_builddir)/tools/rajaChecker
	$(MAP_SCRIPT) $(abs_builddir)/kripke-src $(abs_builddir)/kripke-sequential $(abs_top_builddir)/tools/rajaChecker --filter r:-std=c++11:-std=c++14 -- -rose:unparse_edg_normalized_method_ROSE_1392

$(abs_builddir)/kripke-openmp/rajaChecker.json: $(abs_builddir)/kripke-openmp/compile_commands.json $(MAP_SCRIPT) $(abs_top_builddir)/tools/rajaChecker
	$(MAP_SCRIPT) $(abs_builddir)/kripke-src $(abs_builddir)/kripke-openmp $(abs_top_builddir)/tools/rajaChecker --filter r:-std=c++11:-std=c++14 -- -rose:unparse_edg_normalized_method_ROSE_1392 -D_OPENMP

$(abs_builddir)/kripke-sequential/rajaChecker.html: $(abs_builddir)/kripke-sequential/rajaChecker.json $(RENDER_SCRIPT)
	$(RENDER_SCRIPT) --report $(abs_builddir)/kripke-sequential/rajaChecker.json --title "Kripke with Sequential"

$(abs_builddir)/kripke-openmp/rajaChecker.html: $(abs_builddir)/kripke-openmp/rajaChecker.json $(RENDER_SCRIPT)
	$(RENDER_SCRIPT) --report $(abs_builddir)/kripke-openmp/rajaChecker.json --title "Kripke with OpenMP"

# genInstFlowGraph

$(abs_top_builddir)/projects/TemplateAnalysis/src/genInstFlowGraph:
	make -C $(abs_top_builddir)/projects/TemplateAnalysis/src genInstFlowGraph

$(abs_builddir)/kripke-sequential/genInstFlowGraph.json: $(abs_builddir)/kripke-sequential/compile_commands.json $(MAP_SCRIPT) $(abs_top_builddir)/projects/TemplateAnalysis/src/genInstFlowGraph
	$(MAP_SCRIPT) $(abs_builddir)/kripke-src $(abs_builddir)/kripke-sequential $(abs_top_builddir)/projects/TemplateAnalysis/src/genInstFlowGraph --filter r:-std=c++11:-std=c++14 -- -rose:unparse_edg_normalized_method_ROSE_1392 --namespace-filter-for-analysis=std  --namespace-filter-for-analysis=camp  --namespace-filter-for-analysis=RAJA  --namespace-filter-for-analysis=__gnu_cxx --namespace-filter-for-graphviz=std --namespace-filter-for-graphviz=__gnu_cxx


$(abs_builddir)/kripke-openmp/genInstFlowGraph.json: $(abs_builddir)/kripke-openmp/compile_commands.json $(MAP_SCRIPT) $(abs_top_builddir)/projects/TemplateAnalysis/src/genInstFlowGraph
	$(MAP_SCRIPT) $(abs_builddir)/kripke-src $(abs_builddir)/kripke-openmp $(abs_top_builddir)/projects/TemplateAnalysis/src/genInstFlowGraph --filter r:-std=c++11:-std=c++14 -- -rose:unparse_edg_normalized_method_ROSE_1392 -D_OPENMP --namespace-filter-for-analysis=std  --namespace-filter-for-analysis=camp  --namespace-filter-for-analysis=RAJA  --namespace-filter-for-analysis=__gnu_cxx --namespace-filter-for-graphviz=std --namespace-filter-for-graphviz=__gnu_cxx

$(abs_builddir)/kripke-sequential/genInstFlowGraph.html: $(abs_builddir)/kripke-sequential/genInstFlowGraph.json $(RENDER_SCRIPT)
	$(RENDER_SCRIPT) --report $(abs_builddir)/kripke-sequential/genInstFlowGraph.json --title "Kripke with Sequential" --addons '[ { "function" : "graphviz" , "kwargs" : { "title" : "Template Instantiation Flow" , "filename" : "%d/%fe.dot" } } ]'

$(abs_builddir)/kripke-openmp/genInstFlowGraph.html: $(abs_builddir)/kripke-openmp/genInstFlowGraph.json $(RENDER_SCRIPT)
	$(RENDER_SCRIPT) --report $(abs_builddir)/kripke-openmp/genInstFlowGraph.json --title "Kripke with OpenMP" --addons '[ { "function" : "graphviz" , "kwargs" : { "title" : "Template Instantiation Flow" , "filename" : "%d/%fe.dot" } } ]'

# buildCallGraph

$(abs_top_builddir)/tutorial/buildCallGraph:
	make -C $(abs_top_builddir)/tutorial buildCallGraph

$(abs_builddir)/kripke-sequential/buildCallGraph.json: $(abs_builddir)/kripke-sequential/compile_commands.json $(MAP_SCRIPT) $(abs_top_builddir)/tutorial/buildCallGraph
	$(MAP_SCRIPT) $(abs_builddir)/kripke-src $(abs_builddir)/kripke-sequential $(abs_top_builddir)/tutorial/buildCallGraph --filter r:-std=c++11:-std=c++14 -- -rose:unparse_edg_normalized_method_ROSE_1392


$(abs_builddir)/kripke-openmp/buildCallGraph.json: $(abs_builddir)/kripke-openmp/compile_commands.json $(MAP_SCRIPT) $(abs_top_builddir)/tutorial/buildCallGraph
	$(MAP_SCRIPT) $(abs_builddir)/kripke-src $(abs_builddir)/kripke-openmp $(abs_top_builddir)/tutorial/buildCallGraph --filter r:-std=c++11:-std=c++14 -- -rose:unparse_edg_normalized_method_ROSE_1392 -D_OPENMP

$(abs_builddir)/kripke-sequential/buildCallGraph.html: $(abs_builddir)/kripke-sequential/buildCallGraph.json $(RENDER_SCRIPT)
	$(RENDER_SCRIPT) --report $(abs_builddir)/kripke-sequential/buildCallGraph.json --title "Kripke with Sequential" --addons '[ { "function" : "graphviz" , "kwargs" : { "title" : "Call Graph" , "filename" : "%d/%fe_callGraph.dot" } } ]'

$(abs_builddir)/kripke-openmp/buildCallGraph.html: $(abs_builddir)/kripke-openmp/buildCallGraph.json $(RENDER_SCRIPT)
	$(RENDER_SCRIPT) --report $(abs_builddir)/kripke-openmp/buildCallGraph.json --title "Kripke with OpenMP" --addons '[ { "function" : "graphviz" , "kwargs" : { "title" : "Call Graph" , "filename" : "%d/%fe_callGraph.dot" , "timeout" : "30s" } } ]'

########

check-local: \
  $(abs_builddir)/kripke-sequential/testTranslator.html \
  $(abs_builddir)/kripke-openmp/testTranslator.html \
  $(abs_builddir)/kripke-sequential/rajaChecker.html \
  $(abs_builddir)/kripke-openmp/rajaChecker.html \
  $(abs_builddir)/kripke-sequential/buildCallGraph.html \
  $(abs_builddir)/kripke-openmp/buildCallGraph.html

clean-local:
#	rm -rf $(abs_builddir)/kripke-src
	rm -rf $(abs_builddir)/kripke-sequential $(abs_builddir)/kripke-openmp
	rm -rf $(abs_builddir)/kripke-sequential/testTranslator.json $(abs_builddir)/kripke-openmp/testTranslator.json
	rm -rf $(abs_builddir)/kripke-sequential/testTranslator.html $(abs_builddir)/kripke-openmp/testTranslator.html
	rm -rf $(abs_builddir)/kripke-sequential/rajaChecker.json $(abs_builddir)/kripke-openmp/rajaChecker.json
	rm -rf $(abs_builddir)/kripke-sequential/rajaChecker.html $(abs_builddir)/kripke-openmp/rajaChecker.html
	rm -rf $(abs_builddir)/kripke-sequential/genInstFlowGraph.json
	rm -rf $(abs_builddir)/kripke-sequential/genInstFlowGraph.html

