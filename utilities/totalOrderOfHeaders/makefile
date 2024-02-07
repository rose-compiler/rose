totalOrderGeneration:totalOrderGeneration.cpp
ifndef ROSE_HOME
	$(error ROSE_HOME is not set, it is needed to find nlohmann/json.hpp)
endif
	g++ -I$(ROSE_HOME)/include/rose -O0 -g -std=c++11 $^ -o $@

.PHONY: install check test	
clean:
	rm -rf *.o totalOrderGeneration total_header.hh log.txt myheader.h log2.txt

install: totalOrderGeneration
ifndef ROSE_HOME
	$(error ROSE_HOME is not set)
endif
	@if [ -d "$(ROSE_HOME)/bin" ]; then \
		echo "Installing totalOrderGeneration to $(ROSE_HOME)/bin"; \
		cp $< $(ROSE_HOME)/bin/; \
	else \
		echo "Error: $(ROSE_HOME)/bin does not exist"; \
		exit 1; \
	fi	
# this is the test using builtin test data inside of the tool
test:totalOrderGeneration
	./totalOrderGeneration --test
# run the program using some sample json files under json/ folder
check:totalOrderGeneration
	make test # run builtin tests
	./totalOrderGeneration --input=json/ &>log.txt # run tests with .json input files
	diff total_header.hh reference/total_header.hh
	./totalOrderGeneration --input=json/ --output=myheader.h&>log2.txt # run tests with .json input files
	diff myheader.h reference/total_header.hh
