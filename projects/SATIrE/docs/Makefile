all: satire_manual.pdf
	$(MAKE) -C cfgconstruction
	$(MAKE) -C pig-doc
	$(MAKE) -C pag-interface
	cp pig-doc/pig-doc.pdf pag-interface/pag-interface.pdf .
	cp cfgconstruction/cfgconstruction.pdf satire_cfgconstruction.pdf

	$(MAKE) clean -C cfgconstruction
	$(MAKE) clean -C pig-doc
	$(MAKE) clean -C pag-interface

.PHONY: satire_manual.pdf
satire_manual.pdf:
	$(MAKE) -C manual
	cp manual/satire_manual.pdf .

clean:
	$(MAKE) clean -C cfgconstruction
	$(MAKE) clean -C pig-doc
	$(MAKE) clean -C pag-interface
	$(MAKE) clean -C manual
	rm -f cfgconstruction.pdf pig-doc.pdf pag-interface.pdf satire_manual.pdf
