# Makefile for the run-analysis.sh script.  The script uses this makefile to run the 25-run-tests in parallel since it's
# easier to use GNU make's -j switch than it is to write all the logic ourselves in a shell script.

# The following variables must be provided on the make command-line
BINDIR="."
RUN_FLAGS=
DBNAME=/dev/null
INPUTS=
REDIRECT=yes

.PHONY: all
all:
	@echo "Warning: this makefile is normally run from the run-analysis.sh script. It requires" >&2
	@echo "         that certain variables be initialized on make's command-line." >&2
	@false

###############################################################################################################################
rt_targets = $(addsuffix .rt.out, $(INPUTS))

run-tests: $(rt_targets)

$(rt_targets): %.rt.out: %
	@echo "Output was not redirected to a file" >$@.tmp
	@echo "+ $(BINDIR)/25-run-tests --progress $(RUN_FLAGS) -- $(DBNAME) < $<" >&2
	@if [ "$(REDIRECT)"  = yes ]; then							\
		$(BINDIR)/25-run-tests --progress $(RUN_FLAGS) -- $(DBNAME) < $< >$@.tmp 2>&1;	\
	else											\
		$(BINDIR)/25-run-tests --progress $(RUN_FLAGS) -- $(DBNAME) < $<;		\
	fi
	@mv $@.tmp $@
	@echo "run-tests for $< completed"

###############################################################################################################################
fs_targets=$(addsuffix .fs.out, $(INPUTS))

func-sim: $(fs_targets)

$(fs_targets): %.fs.out: %
	@echo "Output was not redirected to a file" >$@.tmp
	@echo "+ $(BINDIR)/32-func-similarity --progress $(RUN_FLAGS) -- $(DBNAME) < $<" >&2
	@if [ "$(REDIRECT)" = yes ]; then								\
		$(BINDIR)/32-func-similarity --progress $(RUN_FLAGS) -- $(DBNAME) < $< >$@.tmp 2>&1;	\
	else												\
		$(BINDIR)/32-func-similarity --progress $(RUN_FLAGS) -- $(DBNAME) < $<;			\
	fi
	@mv $@.tmp $@
	@echo "func-similarities for $< completed"

###############################################################################################################################
as_targets=$(addsuffix .as.out, $(INPUTS))

api-sim: $(as_targets)

$(as_targets): %.as.out: %
	@echo "Output was not redirected to a file" >$@.tmp
	@echo "+ $(BINDIR)/105-api-similarity --progress $(RUN_FLAGS) -- $(DBNAME) < $<" >&2
	@if [ "$(REDIRECT)" = yes ]; then								\
		$(BINDIR)/105-api-similarity --progress $(RUN_FLAGS) -- $(DBNAME) < $< >$@.tmp 2>&1;	\
	else												\
		$(BINDIR)/105-api-similarity --progress $(RUN_FLAGS) -- $(DBNAME) < $<;			\
	fi
	@mv $@.tmp $@
	@echo "api-similarities for $< completed"       

###############################################################################################################################
ca_targets=$(INPUTS)

ca-computation: $(ca_targets)

$(ca_targets):  
	@echo "+ $(BINDIR)/110_compute_aggregate $(RUN_FLAGS) -- postgresql:///$@" 
	$(BINDIR)/110_compute_aggregate $(RUN_FLAGS) -- postgresql:///$@;		
	@echo "ca-computation for $@ completed"       




