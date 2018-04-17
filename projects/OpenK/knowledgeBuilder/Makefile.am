include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = rosePrgKnowledgeBuilder

rosePrgKnowledgeBuilder_SOURCES = \
	rosePrgKnowledgeBuilder.h \
	rosePrgKnowledgeBuilder.cpp \
	OntoGeneratorInterface.hpp

AM_LDFLAGS = $(ROSE_LIBS) $(LIBS_WITH_RPATH)
AM_CPPFLAGS = $(ROSE_INCLUDES) 

TEST_CODES_REQUIRED_TO_PASS = \
	ut.cpp \
	is_single.cpp \
	templateMax_minimum.cpp \
	templateInheritance.cpp

TEST_CODES_PATH=$(top_srcdir)/projects/OpenK/test

PL_SCRIPT_PATH=$(top_srcdir)/projects/OpenK/projects/canonicalloop

TEST_CODES_OWL_RESULTS = $(TEST_CODES_REQUIRED_TO_PASS:.cpp=.cpp.owl) # change .c to .c.owl
TEST_CODES_TTL_RESULTS = $(TEST_CODES_REQUIRED_TO_PASS:.cpp=.cpp.ttl)
TEST_CODES_PL_RESULTS = $(TEST_CODES_REQUIRED_TO_PASS:.cpp=.cpp.pl)
TEST_CODES_LOOP_RESULTS = 

if ROSE_HAVE_SWI_PROLOG
TEST_CODES_LOOP_RESULTS +=  #$(TEST_CODES_REQUIRED_TO_PASS:.cpp=.output)
endif

# each .c.owl file depends on the corresponding .c file
$(TEST_CODES_OWL_RESULTS): %.cpp.owl:$(TEST_CODES_PATH)/%.cpp rosePrgKnowledgeBuilder
	./rosePrgKnowledgeBuilder -emit-owl $@ -c $(TEST_CODES_PATH)/$< 

$(TEST_CODES_TTL_RESULTS): %.cpp.ttl:$(TEST_CODES_PATH)/%.cpp rosePrgKnowledgeBuilder
	./rosePrgKnowledgeBuilder -emit-ttl $@ -c $(TEST_CODES_PATH)/$< 

$(TEST_CODES_PL_RESULTS): %.cpp.pl:$(TEST_CODES_PATH)/%.cpp rosePrgKnowledgeBuilder
	./rosePrgKnowledgeBuilder -emit-pl $@ -c $(TEST_CODES_PATH)/$< 

# TODO: this does not work yet since run.pl uses relative path, which will be translated into a path in build tree. 
$(TEST_CODES_LOOP_RESULTS): %.output:%.cpp.ttl 
	swipl -s $(PL_SCRIPT_PATH)/run.pl $< $@

all: rosePrgKnowledgeBuilder

check: $(TEST_CODES_OWL_RESULTS) $(TEST_CODES_TTL_RESULTS) $(TEST_CODES_PL_RESULTS) $(TEST_CODES_LOOP_RESULTS)

#check-alive:
#	./rosePrgKnowledgeBuilder -alive true -c $(TESTDIR)/$(TESTCODE)
                                                                                                                                
clean:                                                                                                                          
	rm -rf *.exe *.o *.c.pl *.dot *_report.txt *.owl *.pl *.ttl *.xml
