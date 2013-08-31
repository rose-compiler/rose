#
# This file is part of Canviz. See http://www.canviz.org/
# $Id: Makefile 259 2009-01-20 22:52:24Z ryandesign.com $
#

AWK=awk
FIND=find
SED=sed
SORT=sort
TAIL=tail
XARGS=xargs
DOT=$(shell which dot)
GRAPHVIZ_PREFIX=$(shell dirname $(shell dirname $(DOT)))
GRAPHVIZ_SRC:=$(shell $(FIND) . -type d -name 'graphviz-*' | $(TAIL) -n 1 | $(SED) s%^\./%%)
GRAPHVIZ_LAYOUTS=circo dot fdp neato twopi
GRAPHVIZ_FIRST_LAYOUT=$(firstword $(GRAPHVIZ_LAYOUTS))
EXAMPLES_DIR=examples
EXAMPLE_GRAPHS_SRC_DIR:=$(GRAPHVIZ_PREFIX)/share/graphviz/graphs
#EXAMPLE_GRAPHS_SRC_DIR:=$(GRAPHVIZ_SRC)/rtest/graphs
EXAMPLE_GRAPHS_SRC:=$(shell $(FIND) $(EXAMPLE_GRAPHS_SRC_DIR) -type f -name '*.dot' -or -name '*.gv')
EXAMPLE_GRAPHS_DIR=./graphs
EXAMPLE_GRAPH_IMAGES_DIR=$(EXAMPLE_GRAPHS_DIR)/images
EXAMPLE_GRAPHS=$(patsubst $(EXAMPLE_GRAPHS_SRC_DIR)/%,$(EXAMPLE_GRAPHS_DIR)/$(GRAPHVIZ_FIRST_LAYOUT)/%.txt,$(EXAMPLE_GRAPHS_SRC))

.PHONY: all examples hello-world-example examples-images colorschemes clean

all: examples

colorschemes: x11colors.js brewercolors.js

examples: hello-world-example $(EXAMPLE_GRAPHS_DIR)/graphlist.js $(EXAMPLE_GRAPHS_DIR)/layoutlist.js $(EXAMPLE_GRAPHS) examples-images

hello-world-example: $(EXAMPLES_DIR)/hello_world/new.html $(EXAMPLES_DIR)/hello_world/old.html

$(EXAMPLES_DIR)/hello_world/new.html: $(EXAMPLES_DIR)/hello_world/new.html.in $(EXAMPLES_DIR)/hello_world/graph.gv $(EXAMPLES_DIR)/hello_world/graph-xdot.gv insert_file.awk
	$(AWK) -f insert_file.awk -v placeholder=@GRAPH@ $(EXAMPLES_DIR)/hello_world/graph.gv $< > $@

$(EXAMPLES_DIR)/hello_world/old.html: $(EXAMPLES_DIR)/hello_world/old.html.in $(EXAMPLES_DIR)/hello_world/graph.gv $(EXAMPLES_DIR)/hello_world/graph-cmapx.html $(EXAMPLES_DIR)/hello_world/graph.png insert_file.awk
	$(AWK) -f insert_file.awk -v placeholder=@GRAPH@ $(EXAMPLES_DIR)/hello_world/graph.gv $< > $@.tmp
	$(AWK) -f insert_file.awk -v placeholder=@CMAPX@ $(EXAMPLES_DIR)/hello_world/graph-cmapx.html $@.tmp > $@
	rm -f $@.tmp

$(EXAMPLES_DIR)/hello_world/graph-xdot.gv: $(EXAMPLES_DIR)/hello_world/graph.gv
	$(DOT) $< -Txdot -o$@

$(EXAMPLES_DIR)/hello_world/graph-cmapx.html: $(EXAMPLES_DIR)/hello_world/graph.gv
	$(DOT) $< -Tcmapx -o$@

$(EXAMPLES_DIR)/hello_world/graph.png: $(EXAMPLES_DIR)/hello_world/graph.gv
	$(DOT) $< -Tpng -o$@

$(EXAMPLE_GRAPHS_DIR):
	mkdir $(EXAMPLE_GRAPHS_DIR)

$(EXAMPLE_GRAPH_IMAGES_DIR):
	mkdir $(EXAMPLE_GRAPH_IMAGES_DIR)

$(EXAMPLE_GRAPHS_DIR)/graphlist.js: graphlist.awk $(EXAMPLE_GRAPHS_DIR)
	@echo "Generating $@"
	@echo $(patsubst $(EXAMPLE_GRAPHS_SRC_DIR)/%,%,$(EXAMPLE_GRAPHS_SRC)) | $(AWK) -f graphlist.awk > $@;

$(EXAMPLE_GRAPHS_DIR)/layoutlist.js: layoutlist.awk $(EXAMPLE_GRAPHS_DIR)
	@echo "Generating $@"
	@echo $(GRAPHVIZ_LAYOUTS) | $(AWK) -f layoutlist.awk > $@;

$(EXAMPLE_GRAPHS_DIR)/$(GRAPHVIZ_FIRST_LAYOUT)/%.dot.txt: $(EXAMPLE_GRAPHS_SRC_DIR)/%.dot $(EXAMPLE_GRAPHS_DIR)
	$(render-example-graph)

$(EXAMPLE_GRAPHS_DIR)/$(GRAPHVIZ_FIRST_LAYOUT)/%.gv.txt: $(EXAMPLE_GRAPHS_SRC_DIR)/%.gv $(EXAMPLE_GRAPHS_DIR)
	$(render-example-graph)

define render-example-graph
@echo "Rendering $(subst /$(GRAPHVIZ_FIRST_LAYOUT)/,/*/,$@)"
@./render_example_graph.sh $(EXAMPLE_GRAPHS_SRC_DIR) $(patsubst $(EXAMPLE_GRAPHS_SRC_DIR)/%,%,$<) $(EXAMPLE_GRAPHS_DIR) $(GRAPHVIZ_PREFIX) $(GRAPHVIZ_LAYOUTS)
endef

examples-images: $(EXAMPLE_GRAPH_IMAGES_DIR)
	$(FIND) $(EXAMPLE_GRAPHS_SRC_DIR) -type f -name '*.gif' -or -name '*.jpg' -or -name '*.png' -print0 | $(XARGS) -0 -t -n 1 -I % cp % $(EXAMPLE_GRAPH_IMAGES_DIR)


x11colors.js: gvcolors.awk $(GRAPHVIZ_SRC)/lib/common/color_names
	$(AWK) -f gvcolors.awk < $(GRAPHVIZ_SRC)/lib/common/color_names > $@

brewercolors.js: gvcolors.awk $(GRAPHVIZ_SRC)/lib/common/brewer_lib
	$(AWK) -f gvcolors.awk < $(GRAPHVIZ_SRC)/lib/common/brewer_lib > $@

/lib/common/color_names /lib/common/brewer_lib:
	@echo 'Unpack the Graphviz source in this directory first.' 1>&2
	@exit 1

clean:
	-rm -rf $(EXAMPLES_DIR)/hello_world/graph-xdot.gv $(EXAMPLES_DIR)/hello_world/graph-cmapx.html $(EXAMPLES_DIR)/hello_world/graph.png $(EXAMPLES_DIR)/hello_world/new.html $(EXAMPLES_DIR)/hello_world/old.html $(EXAMPLES_DIR)/hello_world/old.html.tmp $(EXAMPLE_GRAPHS_DIR)
