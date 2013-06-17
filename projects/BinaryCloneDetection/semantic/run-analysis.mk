# Makefile for the run-analysis.sh script.  The script uses this makefile to run the 03-run-tests in parallel since it's
# easier to use GNU make's -j switch than it is to write all the logic ourselves in a shell script.

# The following variables must be provided on the make command-line
BINDIR="."
RUN_FLAGS=
DBNAME=/dev/null
INPUTS=
ERROR=show_error
REDIRECT=yes

targets=$(addsuffix .out, $(INPUTS))

.PHONY: all
all: $(ERROR) $(targets)

$(targets): %.out: %
	@echo "Output was not redirected to a file" >$@.tmp
	@echo "+ $(BINDIR)/03-run-tests $(RUN_FLAGS) -- $(DBNAME) < $<" >&2
	@if [ "$(REDIRECT)"  = yes ]; then							\
		$(BINDIR)/03-run-tests --progress $(RUN_FLAGS) -- $(DBNAME) < $< >$@.tmp 2>&1;	\
	else											\
		$(BINDIR)/03-run-tests --progress $(RUN_FLAGS) -- $(DBNAME) < $<;		\
	fi
	@mv $@.tmp $@
	@echo "Tester $< completed"

.PHONY: show_error
show_error:
	@echo "Warning: this makefile is normally run from the run-analysis.sh script. It requires" >&2
	@echo "         that certain variables be initialized on make's command-line." >&2
	@false
