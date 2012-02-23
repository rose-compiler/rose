# Generated automatically from Makefile.in by configure.
#
# This is a sample Makefile.in.  Make sure to update configure to create
# a Makefile from this.
#
ALL: scatterv
SHELL = /bin/sh
OPT = -O
MPICC=mpicc
MPIRUN=mpirun -maxtime 5
SRCFILES=scatterv.c
OBJFILES=scatterv.o
scatterv: $(OBJFILES)
	$(MPICC) -o scatterv $(OPT) $(OBJFILES) -lm
scatterv.log: $(OBJFILES)
	$(MPICC) -o scatterv.log -mpilog $(OPT) $(OBJFILES) -lm

.c.o:
	$(MPICC) -c $(OPT) $(DEFN) $*.c
cleanlocal:
	/bin/rm -f scatterv $(OBJFILES) scatterv.log PI*
clean: cleanlocal
output:
	$(MPICC) -o scatterv $(OPT) $(SRCFILES) -lm 
	if [ -z "$(NP)" ] ; then \
	    $(MPIRUN) -np 4 scatterv ; \
	else
	    $(MPIRUN) -np $(NP) scatterv ; fi

