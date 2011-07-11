#ifndef ROSE_RSIM_Futex_H
#define ROSE_RSIM_Futex_H

class RSIM_FutexTable {
private:
    static const uint16_t MAGIC = 0x4f52;       /**< Magic number at start of shared memory. */
    static const size_t NBUCKETS = 2039;        /**< Number of buckets. Prime. */
    static const size_t NMEMBERS = 768;         /**< Number of members. */

    struct QueueMember {
        uint64_t key;                           /**< Futex key; i.e., the underlying address.  Zero implies wake up. */
        uint16_t next;                          /**< Index of next entry in the list. Zero implies end of list. */
        uint16_t prev;                          /**< Index of previous entry in the list.  List is not circular. */
        uint32_t bitset;                        /**< Bit vector provided when member was added to queue. */
        sem_t sem;                              /**< Underlying real semaphore to implement wait and wake. */
    };

    struct Table {
        uint16_t magic;                         /**< Magic number. */
        uint16_t free_head;                     /**< Free list head. */
        uint16_t bucket[NBUCKETS];              /**< Array of futex queues (2039 is prime) */
        QueueMember member[NMEMBERS];           /**< Members, initially all on the free list. member[0] is unused.*/
    };
    
public:
    enum LockStatus {                           /**< State of the semaphore associated with this futex table. */
        LOCKED,                                 /**< Semaphore has been acquired. */
        UNLOCKED,                               /**< Semaphore has not yet been acquired. */
    };

    RSIM_FutexTable(sem_t *semaphore, const std::string &name, bool do_unlink)
        : semaphore(semaphore), fd(-1), table(NULL) {
        ctor(name, do_unlink);
    }
    ~RSIM_FutexTable();

    int insert(rose_addr_t key, uint32_t bitset, LockStatus locked);
    int wait(int mno);
    int erase(rose_addr_t key, size_t member_number, LockStatus locked);
    int signal(rose_addr_t key, uint32_t bitset, int nprocs, LockStatus locked);

private:
    void ctor(const std::string &name, bool do_unlink);
    int init_table();
    int check_table();

private:
    sem_t *semaphore;
    int fd;
    Table *table;
};

#endif
