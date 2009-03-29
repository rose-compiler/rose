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


# include Makefile_standalone for srw98 and nnh99
nnh99shape:
	$(MAKE) -f Makefile_standalone nnh99shape_ ANALYSIS='nnh99' CARRIER='ShapeGraphSetLifted'

nnh99clean:
	$(MAKE) -f Makefile_standalone nnh99clean_ ANALYSIS='nnh99' CARRIER='ShapeGraphSetLifted'

srw98shape:
	$(MAKE) -f Makefile_standalone srw98shape_ ANALYSIS='srw98' CARRIER='ShapeGraphLifted'

srw98clean:
	$(MAKE) -f Makefile_standalone srw98clean_ ANALYSIS='srw98' CARRIER='ShapeGraphLifted'


# vim: noexpandtab:
