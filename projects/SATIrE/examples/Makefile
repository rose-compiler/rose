all:
	$(MAKE) -C sl1rd
	$(MAKE) -C sl2rd
	$(MAKE) -C constprop

check:	all
	$(MAKE) check -C sl1rd
	$(MAKE) check -C sl2rd
	$(MAKE) check -C constprop

clean:
	$(MAKE) clean -C sl1rd
	$(MAKE) clean -C sl2rd
	$(MAKE) clean -C constprop


