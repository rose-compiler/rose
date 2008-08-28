#
# shape analysis experimentation makefile
# Viktor Pavlu, 2008
#

.PHONY: all clean nnh99shape nnh99clean srw98shape srw98clean 

all: nnh99shape srw98shape
clean: nnh99clean srw98clean


nnh99shape: nnh99/nnh99.optla nnh99/Makefile\
	common.set common_highlevel.optla common_normalisation.optla\
	common_support.optla common_transfer.optla
	$(MAKE) all -C nnh99

nnh99/nnh99.optla: nnh99/nnh99.code
	cpp -C -P -traditional-cpp nnh99/nnh99.code > nnh99/nnh99.optla

nnh99/Makefile:
	newanalysis -u nnh99

nnh99clean:
	test ! -e nnh99/Makefile || $(MAKE) clean -C nnh99
	rm -f nnh99/nnh99.optla
	rm -f nnh99/Makefile
	rm -f nnh99/main-support.h
	rm -f nnh99/main-support.C
	rm -f nnh99/runtests


srw98shape: srw98/srw98.optla srw98/Makefile\
	common.set common_highlevel.optla common_normalisation.optla\
	common_support.optla common_transfer.optla
	$(MAKE) all -C srw98

srw98/srw98.optla: srw98/srw98.code
	cpp -C -P -traditional-cpp srw98/srw98.code > srw98/srw98.optla

srw98/Makefile:
	newanalysis -u srw98

srw98clean:
	test ! -e srw98/Makefile || $(MAKE) clean -C srw98 
	rm -f srw98/srw98.optla
	rm -f srw98/Makefile
	rm -f srw98/main-support.h
	rm -f srw98/main-support.C
	rm -f srw98/runtests

# vim:set noexpandtab:
