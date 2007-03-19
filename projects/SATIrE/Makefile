DIST=SATIrE-0.7.0

all:
	$(MAKE) -C src
	$(MAKE) EXAMPLE_INSTALL_PATH=$(PWD)/examples -C examples

docs: handwritten-docs doxygen-docs

handwritten-docs:
	$(MAKE) -C docs

doxygen-docs:
	$(MAKE) docs -C src/grato

check:
	$(MAKE) check -C src
	$(MAKE) check -C examples
	@echo "-----------------"
	@echo "All checks passed"
	@echo "-----------------"

install:
	$(MAKE) install -C src

clean:
	$(MAKE) clean -C docs
	$(MAKE) clean -C src
	$(MAKE) EXAMPLE_INSTALL_PATH=$(PWD)/examples clean -C examples
	rm -rf $(DIST)

dist:
	$(MAKE) clean -C src
	$(MAKE) EXAMPLE_INSTALL_PATH=$(PWD)/examples clean -C examples
	$(MAKE) handwritten-docs -C .
	rm -rf $(DIST)
	mkdir -p $(DIST)
	cp -rp src $(DIST)
	cp -rp examples $(DIST)
	mkdir $(DIST)/docs
	cp -rp docs/*.pdf $(DIST)/docs
	rm -rf $(DIST)/examples/sl2rd/tools
	rm -rf $(DIST)/examples/sl2rd/tests/costa
	rm -rf $(DIST)/src/experimental
	cp Makefile_dist $(DIST)/Makefile
	cp -p README $(DIST)
	cp -p KNOWN_BUGS $(DIST)
	cp -p LICENSE $(DIST)
	rm -f $(DIST)/*~
	rm -rf $(DIST)/CVS
	rm -f $(DIST)/*/*~
	rm -rf $(DIST)/*/CVS
	rm -f $(DIST)/*/*/*~
	rm -rf $(DIST)/*/*/CVS
	rm -f $(DIST)/*/*/*/*~
	rm -rf $(DIST)/*/*/*/CVS
	rm -f $(DIST)/*/*/*/*/*~
	rm -rf $(DIST)/*/*/*/*/CVS
	tar cvzf $(DIST).tgz $(DIST)
