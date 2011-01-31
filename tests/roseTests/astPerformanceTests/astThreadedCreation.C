/* Tests that AST nodes can be created in a multi-threaded environment.
 *
 * The test makes a number of passes (NPASSES), and each pass performs these steps:
 *    1. Clone a number of threads (NTHREADS), and in each thread
 *          -- create a number of nodes (NODES_PER_THREAD)
 *          -- set an integer property of the node to a unique value
 *    2. Wait for all threads to complete, then check that
 *          -- every "new" resulted in a unique non-null pointer
 *          -- every node has the expected value for the integer property
 *    3. Clone a number of threads (NTHREADS), and in each thread
 *          -- delete some nodes
 *
 * The reason for multiple passes is because we verify that deletion is working properly by allocated
 * more nodes afterward.
 *
 * We use SgAsmGenericSection as the node type because:
 *    1. It's not a base class, and therefore might exercise more sophisticated code paths
 *    2. It has an integer property (id) that's not limit checked or used for anything during construction
 */

#include "rose.h"

#define NPASSES 3                       /* number of passes through this test, each pass creates and deletes nodes */
#define NTHREADS 2                      /* zero implies using only the main thread; >0 implies creation of sub-threads */
#define NODES_PER_THREAD 2000           /* number of nodes to create per thread */

#define thread_of(G)    ((G)/NODES_PER_THREAD)
#define node_of(G)      ((G)%NODES_PER_THREAD)

/* Set this if you want this test to protect the "new" operator with its own mutex.  This might be useful when debugging.  If
 * you get errors when this is clear but they cannot be reproduced when this is set, then the error is likely due to
 * multi-threading issues within ROSE rather than other kinds of errors. */
static bool use_my_own_mutex = false;
static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;


static SgAsmGenericFile *file;
static SgAsmGenericSection *nodes[(NTHREADS?NTHREADS:1)*NODES_PER_THREAD];

static void *create_nodes(void *_offsetp)
{
    int offset = *(int*)_offsetp;
    for (int i=0; i<NODES_PER_THREAD; i++) {
        if (use_my_own_mutex)
            pthread_mutex_lock(&m);
        /* Note: we must use a null header because insertion of the new section into a header is not thread safe at
         * this time. In other words, the node we're creating is not attached to the AST. [RPM 2011-01-27] */
        SgAsmGenericSection *node = nodes[offset+i] = new SgAsmGenericSection(file, NULL);
        if (use_my_own_mutex)
            pthread_mutex_unlock(&m);
        if (node)
            node->set_id(offset+i);
    }
    return NULL;
}

/* Deletes nodes */
static void *delete_nodes(void *_offsetp)
{
    int offset = *(int*)_offsetp;
    for (int i=0; i<NODES_PER_THREAD; i++) {
        if (use_my_own_mutex)
            pthread_mutex_lock(&m);
        delete nodes[offset+i];
        nodes[offset+i] = NULL;
        if (use_my_own_mutex)
            pthread_mutex_unlock(&m);
    }
    return NULL;
}

int main()
{
    bool had_errors = false;
    pthread_t threads[NTHREADS?NTHREADS:1];
    int offsets[NTHREADS?NTHREADS:1];
    file = new SgAsmGenericFile;

    for (int pass=0; pass<NPASSES; pass++) {
        /* Create the nodes */
        memset(nodes, 0, sizeof nodes);
        if (0==NTHREADS) {
            fprintf(stderr, "pass %d: creating %d nodes using only the main thread...\n", pass, NODES_PER_THREAD);
            offsets[0] = 0;
            create_nodes(offsets+0);
        } else {
            fprintf(stderr, "pass %d: creating %d nodes using %d threads with %s mutex...\n",
                    pass, NTHREADS*NODES_PER_THREAD, NTHREADS, use_my_own_mutex?"my own":"ROSE's");
            for (int i=0; i<NTHREADS; i++) {
                offsets[i] = i * NODES_PER_THREAD;
                pthread_create(threads+i, NULL, create_nodes, offsets+i);
            }
            for (int i=0; i<NTHREADS; i++)
                pthread_join(threads[i], NULL);
        }

#if 0 /* If you want to verify that the checking below is working... */
        int idx_lo = NODES_PER_THREAD/2;
        int idx_hi = ((NTHREADS?NTHREADS:1)-1)*NODES_PER_THREAD+NODES_PER_THREAD/2+1;
        fprintf(stderr, "DEBUG: duplicating node %d.%d to %d.%d\n",
                thread_of(idx_lo), node_of(idx_lo), thread_of(idx_hi), node_of(idx_hi));
        nodes[idx_hi] = nodes[idx_lo];
#endif

        /* Check results:
         *  1. Every node must have been allocated
         *  2. node[i].id == i
         *  3. Every node must have a unique address (checked via std::map for O(N log N))
         */
        fprintf(stderr, "pass %d: checking results...\n", pass);
        typedef std::map<SgAsmGenericSection*, int> SectionMap;
        SectionMap sections;
        for (int i=0; i<(NTHREADS?NTHREADS:1)*NODES_PER_THREAD; i++) {
            if (!nodes[i]) {
                fprintf(stderr, "    node %d.%d is null\n", thread_of(i), node_of(i));
                had_errors = true;
            } else {
                if (nodes[i]->get_id()!=i) {
                    fprintf(stderr, "    node %d.%d has unexpected value: has %d, but expected %d\n",
                            thread_of(i), node_of(i), nodes[i]->get_id(), i);
                    had_errors = true;
                }
                std::pair<SectionMap::iterator,bool> inserted = sections.insert(std::make_pair(nodes[i], i));
                if (!inserted.second) {
                    int j = inserted.first->second;
                    fprintf(stderr, "    node %d.%d is the same as node %d.%d\n",
                            thread_of(i), node_of(i), thread_of(j), node_of(j));
                    had_errors = true;
                }
            }
        }

        /* Delete the nodes using the same number of threads.  There's not a good way to test that this actually works other
         * than perhaps getting fault of some sort. When running with multiple passes, the subsequent node creations might
         * detect an error... */
        if (0==NTHREADS) {
            fprintf(stderr, "pass %d: deleting nodes using only the main thread...\n", pass);
            delete_nodes(offsets+0);
        } else {
            fprintf(stderr, "pass %d: deleting nodes using %d threads with %s mutex...\n",
                    pass, NTHREADS, use_my_own_mutex?"my own":"ROSE's");
            for (int i=0; i<NTHREADS; i++)
                pthread_create(threads+i, NULL, delete_nodes, offsets+i);
            for (int i=0; i<NTHREADS; i++)
                pthread_join(threads[i], NULL);
        }
    }

    return had_errors ? 1 : 0;
}
