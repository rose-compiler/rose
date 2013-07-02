
html: doxy.conf
	doxygen doxy.conf

tar: html
	tar czf doc.tar.gz html

clean-local:
	rm -rf html doc.tar.gz

EXTRA_DIST = \
  doxy.conf.in \
  main.doxy

