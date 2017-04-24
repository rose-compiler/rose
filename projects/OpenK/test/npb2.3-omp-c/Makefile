SHELL=/bin/sh
CLASS=S
SFILE=config/suite.def

default: header
	@ $(SHELL) sys/print_instructions

BT: bt
bt: header
	cd BT; $(MAKE) CLASS=$(CLASS)
		       
SP: sp		       
sp: header	       
	cd SP; $(MAKE) CLASS=$(CLASS)
		       
LU: lu		       
lu: header	       
	cd LU; $(MAKE) CLASS=$(CLASS)
		       
MG: mg		       
mg: header	       
	cd MG; $(MAKE) CLASS=$(CLASS)
		       
FT: ft		       
ft: header	       
	cd FT; $(MAKE) CLASS=$(CLASS)
		       
IS: is		       
is: header	       
	cd IS; $(MAKE) CLASS=$(CLASS)
		       
CG: cg		       
cg: header	       
	cd CG; $(MAKE) CLASS=$(CLASS)
		       
EP: ep		       
ep: header	       
	cd EP; $(MAKE) CLASS=$(CLASS)

# Awk script courtesy cmg@cray.com
suite:
	@ awk '{ if ($$1 !~ /^#/ &&  NF > 0)                              \
	printf "make %s CLASS=%s\n", $$1, $$2 }' $(SFILE)  \
	| $(SHELL)


# It would be nice to make clean in each subdirectory (the targets
# are defined) but on a really clean system this will won't work
# because those makefiles need config/make.def
clean:
	- rm -f core 
	- rm -f *~ */core */*~ */*.o */npbparams.h */*.obj */*.exe */tmp.*
	- rm -f sys/setparams sys/makesuite sys/setparams.h
	- rm -f bin/sp.* bin/lu.* bin/mg.* bin/ft.* bin/bt.* bin/is.* bin/ep.* bin/cg.*

veryclean: clean
	- rm config/make.def config/suite.def Part*
	- rm bin/sp.* bin/lu.* bin/mg.* bin/ft.* bin/bt.* bin/is.* bin/ep.* bin/cg.*

header:
	@ $(SHELL) sys/print_header

kit: 
	- makekit -s100k -k30 * */* */*/*


