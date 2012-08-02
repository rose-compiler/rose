#ifdef PERFCTR

#define PERF_INIT \
int err, EventSet = PAPI_NULL; \
int papi_events[4] = {PAPI_L1_DCM, PAPI_L2_TCM, PAPI_MEM_SCY, PAPI_TLB_DM}; \
long long CtrValues[4]; \
\
/* --- SETUP --- Configure PAPI events ------------------ */ \
printf("Initializing PAPI Hardware Performance Counters:\n"); \
err = PAPI_library_init(PAPI_VER_CURRENT); \
if (err != PAPI_VER_CURRENT) { \
    fprintf(stderr, "Error initializing PAPI: Version mismatch.  Expected %d, got %d\n", PAPI_VER_CURRENT, err); \
    perror("System error (mostly a perfctr device permission problem)\n"); \
    exit(1); \
} \
 \
/* Create Event Set */ \
if (PAPI_create_eventset(&EventSet) != PAPI_OK) { \
    printf("Failed to create PAPI Event Set\n"); \
    exit(1); \
} \
 \
/* Add Total Instructions Executed to our EventSet */ \
for (int i=0; i<4; i++) { \
    char out[PAPI_MAX_STR_LEN]; \
    PAPI_event_code_to_name(papi_events[i], out); \
    err = PAPI_add_event(EventSet, papi_events[i]); \
    if (err != PAPI_OK) { \
        printf("Failed to add PAPI event: %s\n", out); \
    } \
} \
PAPI_start(EventSet);

#define PERF_EXIT \
        PAPI_stop(EventSet, &CtrValues[0]); \
        printf("L1 D cache misses: %ld\n", CtrValues[0]); \
        printf("L2 (total) cache misses: %ld\n", CtrValues[1]); \
        printf("Memory access stalls: %ld\n", CtrValues[1]); \
        printf("Data TLB misses: %ld\n", CtrValues[1]);
#endif
