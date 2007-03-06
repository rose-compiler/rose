all:
	$(MAKE) -C ../pig
	$(MAKE) -C icfgbuilder
	$(MAKE) -C astaccess
	$(MAKE) -C commandlineinterface  
#	$(MAKE) -C annotation
	cp astaccess/syntree.o .
	ar -csr libsatire.a icfgbuilder/*.o astaccess/*.o commandlineinterface/*.o #annotation/*.o
	ar -d libsatire.a syntree.o

clean: 
	$(MAKE) clean -C icfgbuilder
	$(MAKE) clean -C astaccess
	$(MAKE) clean -C commandlineinterface  
#	$(MAKE) clean -C annotation
	rm -f libsatire.a
