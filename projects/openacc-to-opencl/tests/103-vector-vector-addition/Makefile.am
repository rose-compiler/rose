
TEST_ID=103

NBR_GANG=8
NBR_WORKER=64
NBR_VECTOR=1

TEST_PARAMS=1048576

include ../100-rules.mk

eval: 103-libopenacc-tile_0 103-libopenacc-tile_1 103-libopenacc-tile_2 103-reference
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(OPENACC_LIB_DIR) ../100-eval.sh $(srcdir) $(CURRDIR)/../../datas/ 103 libopenacc-tile_0
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(OPENACC_LIB_DIR) ../100-eval.sh $(srcdir) $(CURRDIR)/../../datas/ 103 libopenacc-tile_1
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(OPENACC_LIB_DIR) ../100-eval.sh $(srcdir) $(CURRDIR)/../../datas/ 103 libopenacc-tile_2
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):$(OPENACC_LIB_DIR) ../100-eval.sh $(srcdir) $(CURRDIR)/../../datas/ 103 reference

$(TEST_ID)-host-data-v1.o: $(srcdir)/$(TEST_ID)-host-data.c $(ACC_INTERNAL_DEPS)
	$(CC) $(CFLAGS) $(INCLUDES) -DKERNEL_VERSION=1 $(HOST_ENV_MACRO) -c $(srcdir)/$(TEST_ID)-host-data.c -o $@

$(TEST_ID)-libopenacc-tile_0: $(TEST_ID)-libopenacc.o $(TEST_ID)-host-data-v1.o $(DEPS)
	$(LD) $(LDFLAGS) $(LIBS) $(TEST_ID)-libopenacc.o $(TEST_ID)-host-data-v1.o -o $(TEST_ID)-libopenacc-tile_0

$(TEST_ID)-host-data-v2.o: $(srcdir)/$(TEST_ID)-host-data.c $(ACC_INTERNAL_DEPS)
	$(CC) $(CFLAGS) $(INCLUDES) -DKERNEL_VERSION=2 $(HOST_ENV_MACRO) -c $(srcdir)/$(TEST_ID)-host-data.c -o $@

$(TEST_ID)-libopenacc-tile_1: $(TEST_ID)-libopenacc.o $(TEST_ID)-host-data-v2.o $(DEPS)
	$(LD) $(LDFLAGS) $(LIBS) $(TEST_ID)-libopenacc.o $(TEST_ID)-host-data-v2.o -o $(TEST_ID)-libopenacc-tile_1

$(TEST_ID)-host-data-v3.o: $(srcdir)/$(TEST_ID)-host-data.c $(ACC_INTERNAL_DEPS)
	$(CC) $(CFLAGS) $(INCLUDES) -DKERNEL_VERSION=3 $(HOST_ENV_MACRO) -c $(srcdir)/$(TEST_ID)-host-data.c -o $@

$(TEST_ID)-libopenacc-tile_2: $(TEST_ID)-libopenacc.o $(TEST_ID)-host-data-v3.o $(DEPS)
	$(LD) $(LDFLAGS) $(LIBS) $(TEST_ID)-libopenacc.o $(TEST_ID)-host-data-v3.o -o $(TEST_ID)-libopenacc-tile_2




