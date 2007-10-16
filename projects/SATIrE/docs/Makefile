all:
	$(MAKE) -C cfgconstruction
	$(MAKE) -C pig-doc
	$(MAKE) -C pag-interface
	cp pig-doc/pig-doc.pdf pag-interface/pag-interface.pdf .
	cp cfgconstruction/cfgconstruction.pdf satire_cfgconstruction.pdf

	$(MAKE) clean -C cfgconstruction
	$(MAKE) clean -C pig-doc
	$(MAKE) clean -C pag-interface

clean:
	$(MAKE) clean -C cfgconstruction
	$(MAKE) clean -C pig-doc
	$(MAKE) clean -C pag-interface
	rm -f cfgconstruction.pdf pig-doc.pdf pag-interface.pdf
