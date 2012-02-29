#
# shape analysis experimentation makefile
# Viktor Pavlu, 2008
#

.PHONY: all clean clean_
.PHONY: nnh99shape nnh99clean
.PHONY: srw98shape srw98clean
.PHONY: diagrams

BUILD=build

all: nnh99shape srw98shape
clean: nnh99clean srw98clean clean_

MAKE=make CC="gcc" CXX="g++-4.2.0"

# include Makefile_template for srw98 and nnh99
nnh99shape:
	@$(MAKE) -f Makefile_template shape ANALYSIS='nnh99' CARRIER='ShapeGraphSetLifted' LOWEREDCARRIER='ShapeGraphSet' BUILD="$(BUILD)"
nnh99clean:
	@$(MAKE) -f Makefile_template clean ANALYSIS='nnh99' CARRIER='ShapeGraphSetLifted' LOWEREDCARRIER='ShapeGraphSet' BUILD="$(BUILD)"

srw98shape:
	@$(MAKE) -f Makefile_template shape ANALYSIS='srw98' CARRIER='ShapeGraphLifted' LOWEREDCARRIER='ShapeGraph' BUILD="$(BUILD)"
srw98clean:
	@$(MAKE) -f Makefile_template clean ANALYSIS='srw98' CARRIER='ShapeGraphLifted' LOWEREDCARRIER='ShapeGraph' BUILD="$(BUILD)"



$(BUILD)/nnh99/nnh99: nnh99shape
$(BUILD)/srw98/srw98: srw98shape

report_prec_nokeep_aliases.txt: $(BUILD)/nnh99/nnh99 $(BUILD)/srw98/srw98
	./measurements _prec_nokeep_aliases --var-alias-from-graphset --var-delete-tmpvars

report_prec_keep_aliases.txt: $(BUILD)/nnh99/nnh99 $(BUILD)/srw98/srw98
	./measurements _prec_keep_aliases --var-alias-from-graphset --var-keep-tmpvars

report_fast_nokeep_aliases.txt: $(BUILD)/nnh99/nnh99 $(BUILD)/srw98/srw98
	./measurements _fast_nokeep_aliases --var-alias-from-summary-graph --var-delete-tmpvars

report_fast_keep_aliases.txt: $(BUILD)/nnh99/nnh99 $(BUILD)/srw98/srw98
	./measurements _fast_keep_aliases --var-alias-from-summary-graph --var-keep-tmpvars

diagrams: report_fast_keep_aliases.txt \
	        report_fast_nokeep_aliases.txt \
	        report_prec_keep_aliases.txt \
	        report_prec_nokeep_aliases.txt
#	/usr/local/mstools/bin/gnuplot < diagrams.gp


clean_:
	-rm -rf $(BUILD)
	-rm -rf tmp_*
	-rm -rf srw98.gdl nnh99.gdl
	-rm -rf gen_*

# vim: noexpandtab:


