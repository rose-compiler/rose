#ifndef ROSE_RSIM_Futex_H
#define ROSE_RSIM_Futex_H

/** Table of fast user-space mutex (futex) queues.  This table is placed in the simulator's shared memory and protected by a
 *  semaphore contained in the table.  It's essentially a hash table containing a fixed number of QueueMember objects, each of
 *  which represents a single thread that's blocked on a futex.  The QueueMember objects are linked into a free list when
 *  they're not in use. */
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

    /** Construct a new futex table.  The specified semaphore is aquired and released by this method; the simulator's global
     *  semaphore is ususually passed as this argument.  The @p name is supplied to the underlying shm_open() call and, if @p
     *  do_unlink is true, the semaphore is unlinked from the filesystem immediately after being created/opened.
     *
     *  This method always tries to open an existing shared memory file before creating a new one.  In any case, the file size
     *  is always truncated to be the size of an RSIM_FutexTable object and mapped into shared memory. */
    RSIM_FutexTable(sem_t *semaphore, const std::string &name, bool do_unlink)
        : semaphore(semaphore), fd(-1), table(NULL) {
        ctor(name, do_unlink);
    }

    /** Destroy a futex table.  The table is unmaped and the shared memory file descriptor is closed.  No attempt is made to
     *  wake threads that might be blocked on the table. */
    ~RSIM_FutexTable();

    /** Insert a new key into the wait queue and return its index.  The @p bitset vector is used during the signal operation to
     *  determine which waiting threads should be unblocked.  If @p locked is false then the simulator's global semaphore is
     *  aquired and released by this function.
     *
     *  Returns the futex table index for the new entry on success; returns a negative error number on failure.  On failure,
     *  the global semphore is posted if we had waited on it.  In addition to the usual errors returned by sem_wait and
     *  sem_init, this function may also return -EINTR if interrupted by a signal, or -EAGAIN if the futex table is full. */
    int insert(rose_addr_t key, uint32_t bitset, LockStatus locked);

    /** Wait for the specified futex to be signaled.  The @p mno argument is the return value from the insert() operation and
     *  must not be an index that has been erased from the table.  This function blocks until the futex is signaled by another
     *  thread.
     *
     *  Returns zero on success, negative error number on failure.  It is possible for this function to return -EINTR if
     *  interrupted by a signal. */
    int wait(int mno);

    /** Remove a member from the futex wait queue.  The @p member_number must be the value returned by the insert() method for
     *  the same @p key as supplied to this erase method.  If @p locked is false then the simulator's global semaphore is
     *  aquired and released by this method.
     *
     *  Returns zero on success; returns a negative error number on failure.  Besides the normal failures from sem_wait, this
     *  method might also return -EINVAL if the specified @p key, @p member_number pair does not exist in the table. */
    int erase(rose_addr_t key, size_t member_number, LockStatus locked);

    /** Signal a futex, waking up one or more threads that are blocked.  This operation wakes at most @p nprocs threads waiting
     *  on the futex having the specified @p key.  If @p locked is false, then the simulator's global semaphore is aquired and
     *  released by this method.
     *
     *  Returns the number of threads woken up on success; a negative error number on failure.  On error, no threads have been
     *  woken up. */
    int signal(rose_addr_t key, uint32_t bitset, int nprocs, LockStatus locked);

    /** Prints the contents of the futex table.  This internal information is intended for debugging and its format could
     *  change at any time. */
    void dump(FILE*, const std::string &title="", const std::string &prefix="");

private:
    void ctor(const std::string &name, bool do_unlink);                 /**< Constructor helper. */
    int init_table();                                                   /**< Initialize a new table during construction. */
    int check_table();                                                  /**< Check table consistency. */

private:
    sem_t *semaphore;                                                   /**< Semaphore protecting entire table. */
    int fd;                                                             /**< File descriptor for SYSV shared memory. */
    Table *table;                                                       /**< Address of shared memory table. */
};

#endif
