DIST=satire_0.7.0

all:
	$(MAKE) -C src

docs: handwritten-docs doxygen-docs

handwritten-docs:
	$(MAKE) -C docs

doxygen-docs:
	$(MAKE) docs -C src/grato

check:
	$(MAKE) check -C src
	@echo "-----------------"
	@echo "All checks passed"
	@echo "-----------------"

install:
	$(MAKE) install -C src

clean:
	$(MAKE) clean -C docs
	$(MAKE) clean -C src
	rm -rf $(DIST)

dist:
	$(MAKE) clean -C src
	$(MAKE) handwritten-docs -C .
	rm -rf $(DIST)
	mkdir -p $(DIST)
	cp -rp src $(DIST)
	mkdir $(DIST)/docs
	cp -rp docs/*.pdf $(DIST)/docs
	rm $(DIST)/src/examples/specs/adapt_makefile
	rm $(DIST)/src/examples/specs/makefile_header
	rm -rf $(DIST)/src/experimental
	cp Makefile_dist $(DIST)/Makefile
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
