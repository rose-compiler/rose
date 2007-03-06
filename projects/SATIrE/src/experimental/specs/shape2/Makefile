#
# shape analysis experimentation makefile
# Viktor Pavlu, 2008
#

.PHONY: all clean
.PHONY: nnh99shape nnh99clean nnh99optla
.PHONY: srw98shape srw98clean srw98optla
.PHONY: shapetool shapetoolclean

info:
	@echo Targets to choose from:
	@echo
	@echo '  nnh99shape -- builds nnh99 standalone'
	@echo '  srw98shape -- builds srw98 standalone'
	@echo '  shapetool  -- both analyses in a single executable'
	@echo
	@echo '  all -- all of the above'
	@echo

all: nnh99shape srw98shape shapetool
clean: nnh99clean srw98clean shapetoolclean


# nnh99shape compiles and links the analysis as standalone binary.
# postprocess-main edits the newanalysis-generated main.[Ch]
# files to include the common code fragments like visualisation
nnh99shape: nnh99/nnh99.optla nnh99/main.C nnh99/CmdlineOptions.C nnh99/CmdlineOptions.h nnh99/visualisation.h
	$(MAKE) all -C nnh99

nnh99/nnh99.optla: nnh99/nnh99.code
	# include common_*.optla into nnh99.optla, customize DFI, remove prefix
	cpp -C -P nnh99/nnh99.code | sed -e 's/{DFI}/ShapeGraphSetLifted/g' | grep -v "prefix *: *nnh99" > nnh99/nnh99.optla

nnh99/main.C: nnh99/nnh99.optla
	newanalysis nnh99
	./postprocess-main nnh99/main.C nnh99

nnh99/visualisation.h: visualisation.h
	sed visualisation.h -e 's/{DFI}/o_ShapeGraphSetLifted/g' > nnh99/visualisation.h

nnh99/CmdlineOptions.C: CmdlineOptions.C
	cp CmdlineOptions.C nnh99/

nnh99/CmdlineOptions.h: CmdlineOptions.h
	cp CmdlineOptions.h nnh99/


nnh99clean:
	test ! -e nnh99/Makefile || $(MAKE) clean -C nnh99
	rm -f nnh99/nnh99.optla nnh99/GraphStats.h nnh99/CmdlineOptions.[Ch] nnh99/visualisation.h
	rm -f nnh99/Makefile
	rm -f nnh99/main-support.[Ch] nnh99/main.[Ch] nnh99/nnh99_implementation.[Ch]
	rm -f nnh99/runtests




# srw98shape compiles and links the analysis as standalone binary.
# postprocess-main edits the newanalysis-generated main.[Ch]
# files to include the common code fragments like visualisation
srw98shape: srw98/srw98.optla srw98/main.C srw98/CmdlineOptions.C srw98/CmdlineOptions.h srw98/visualisation.h
	$(MAKE) all -C srw98

srw98/srw98.optla: srw98/srw98.code
	# include common_*.optla into srw98.optla, customize DFI, remove prefix
	cpp -C -P srw98/srw98.code | sed -e 's/{DFI}/ShapeGraphLifted/g' | grep -v "prefix *: *srw98" > srw98/srw98.optla

srw98/main.C: srw98/srw98.optla
	newanalysis srw98
	./postprocess-main srw98/main.C srw98

srw98/visualisation.h: visualisation.h
	sed visualisation.h -e 's/{DFI}/o_ShapeGraphLifted/g' > srw98/visualisation.h

srw98/CmdlineOptions.C: CmdlineOptions.C
	cp CmdlineOptions.C srw98/

srw98/CmdlineOptions.h: CmdlineOptions.h
	cp CmdlineOptions.h srw98/


srw98clean:
	test ! -e srw98/Makefile || $(MAKE) clean -C srw98
	rm -f srw98/srw98.optla srw98/GraphStats.h srw98/CmdlineOptions.[Ch] srw98/visualisation.h
	rm -f srw98/Makefile
	rm -f srw98/main-support.[Ch] srw98/main.[Ch] srw98/srw98_implementation.[Ch]
	rm -f srw98/runtests







# shapetool compiles and links both analyses as childs of
# an empty dummy analysis. on the commandline you can then
# choose the implementation of shape analysis you want to
# run

BIN=shape

shapetool:
	# prepare dummy analysis
	mkdir -p $(BIN)
	cpp -C -P shapetool.optla > $(BIN)/$(BIN).optla
	# prepare srw98
	mkdir -p $(BIN)/srw98
	cpp -C -P srw98/srw98.code | sed -e 's/{DFI}/ShapeGraphLifted/g' > $(BIN)/srw98/srw98.optla
	cp srw98/Makefile.custom $(BIN)/srw98/Makefile.custom
	# prepare nnh99
	mkdir -p $(BIN)/nnh99
	cpp -C -P nnh99/nnh99.code | sed -e 's/{DFI}/ShapeGraphSetLifted/g' > $(BIN)/nnh99/nnh99.optla
	cp nnh99/Makefile.custom $(BIN)/nnh99/Makefile.custom
	# compile and link
	newcombinedanalysis $(BIN) srw98 nnh99
	# put custom visualisation in children's main-support.o but remove all other code as it
	# will not be needed and only produces name-clashes; the visualisation is prefixed, so
	# no name-clashes with that, compile there to xxx/main-support.o
	# --1, main-support.C receives code
	echo '// generated main-support.C with only the visualisation' > $(BIN)/nnh99/main-support.C
	echo '#ifndef CARRIER_TYPE'                                   >> $(BIN)/nnh99/main-support.C
	echo '    #define CARRIER_TYPE PLEASE_SPECIFY_CARRIER_TYPE'   >> $(BIN)/nnh99/main-support.C
	echo '#endif'                                                 >> $(BIN)/nnh99/main-support.C
	echo '#include "main-support.h"'                              >> $(BIN)/nnh99/main-support.C
	echo '#include "cfg_support.h"'                               >> $(BIN)/nnh99/main-support.C
	echo '#include "analysis_info.h"'                             >> $(BIN)/nnh99/main-support.C
	cp $(BIN)/nnh99/main-support.C $(BIN)/srw98/main-support.C
	# prefix and customize {DFI}
	sed -e 's/o_extract_/o_srw98o_extract_/g' -e 's/o_vars_by_/o_srw98o_vars_by_/g' -e 's/dfi_write_/o_srw98dfi_write_/g' -e 's/{DFI}/ShapeGraphLifted/g'    visualisation.c >> $(BIN)/srw98/main-support.C
	sed -e 's/o_extract_/o_nnh99o_extract_/g' -e 's/o_vars_by_/o_nnh99o_vars_by_/g' -e 's/dfi_write_/o_nnh99dfi_write_/g' -e 's/{DFI}/ShapeGraphSetLifted/g' visualisation.c >> $(BIN)/nnh99/main-support.C
	# --2, visualisation.h put there for inclusion
	sed -e 's/dfi_write_/o_srw98dfi_write_/g' -e 's/{DFI}/ShapeGraphLifted/g'    visualisation.h > $(BIN)/srw98/visualisation.h
	sed -e 's/dfi_write_/o_nnh99dfi_write_/g' -e 's/{DFI}/ShapeGraphSetLifted/g' visualisation.h > $(BIN)/nnh99/visualisation.h
	# --3, cmdlineoptions.[Ch] too
	cp shapetool_main.C    $(BIN)/main.C
	cp CmdlineOptions.[Ch] $(BIN)
	cp CmdlineOptions.[Ch] $(BIN)/srw98
	cp CmdlineOptions.[Ch] $(BIN)/nnh99
	# modify root Makefile so that the xxx/main-support.o's are linked as well
	sed -i -e 's|OTHERANALYSISLIBS = ${OTHERANALYSES:%=otheranalyses/%.a}|OTHERANALYSISLIBS = ${OTHERANALYSES:%=otheranalyses/%.a} nnh99/main-support.o srw98/main-support.o |g' $(BIN)/Makefile
	# finally, build the bastard
	$(MAKE) srw98.a -C $(BIN)/srw98/
	$(MAKE) nnh99.a -C $(BIN)/nnh99/
	$(MAKE) main-support.o -C $(BIN)/srw98/
	$(MAKE) main-support.o -C $(BIN)/nnh99/
	make -C $(BIN)

shapetoolclean:
	rm -rf $(BIN)

# vim:set noexpandtab:

