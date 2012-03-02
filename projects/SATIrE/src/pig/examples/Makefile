# set OCE_DIR to point to the devsrc directory containing the compiler sources
OCE_DIR = ../../devsrc/
# set PAG to point to the pag executable
PAG = $(PAGDIR)/bin/pag
# set PIG to point to the pig executable; this should be correct for you
PIG = ../pig

check: pig compile checkpag

pig: syn c.pig
	../pig -pi syn c.pig

compile: pig
	g++ -I$(OCE_DIR) -c syntree.c

checkpag: pig

clean:
	rm -f pig_* syntree.*