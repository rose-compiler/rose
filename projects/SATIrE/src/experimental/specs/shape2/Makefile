#
# shape analysis experimentation makefile
# Viktor Pavlu, 2008
#

.PHONY: all clean clean_
.PHONY: nnh99shape nnh99clean
.PHONY: srw98shape srw98clean

all: nnh99shape srw98shape
clean: nnh99clean srw98clean clean_

# include Makefile_template for srw98 and nnh99
nnh99shape:
	@$(MAKE) -f Makefile_template shape ANALYSIS='nnh99' CARRIER='ShapeGraphSetLifted'
nnh99clean:
	@$(MAKE) -f Makefile_template clean ANALYSIS='nnh99' CARRIER='ShapeGraphSetLifted'

srw98shape:
	@$(MAKE) -f Makefile_template shape ANALYSIS='srw98' CARRIER='ShapeGraphLifted'
srw98clean:
	@$(MAKE) -f Makefile_template clean ANALYSIS='srw98' CARRIER='ShapeGraphLifted'

clean_:
	-rm -rf build/* build tmp_alias-stats tmp_output

# vim: noexpandtab:


