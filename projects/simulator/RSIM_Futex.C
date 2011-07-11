#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR

void
RSIM_FutexTable::ctor(const std::string &name, bool do_unlink)
{
    if (fd<0) {
        /* Obtain a lock */
        int status = sem_wait(semaphore);
        assert(0==status);

        int err = 0;
        bool created = false;
        do {
            /* Try to open an existing file, or create a new one. */
            if ((fd = shm_open(name.c_str(), O_RDWR, 0666)) < 0) {
                if ((fd = shm_open(name.c_str(), O_CREAT|O_RDWR, 0666)) < 0) {
                    err = -errno;
                    break;
                }

                if (do_unlink)
                    shm_unlink(name.c_str());

                if (ftruncate(fd, sizeof(Table))<0) {
                    err = -errno;
                    break;
                }
                
                created = true;
            }

            /* Map the table to memory */
            table = (RSIM_FutexTable::Table*)mmap(NULL, sizeof(Table), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
            if (MAP_FAILED==table) {
                err = -errno;
                table = NULL;
                break;
            }
            if (created) {
                err = init_table();
            } else {
                err = check_table();
            }
        } while (0);

        /* Error cleanup */
        if (err) {
            if (table) {
                munmap(table, sizeof(Table));
                table = NULL;
            }
            if (fd>=0) {
                close(fd);
                fd = -1;
            }
        }

        /* Release the lock */
        status = sem_post(semaphore);
        assert(0==status);
    }
}

RSIM_FutexTable::~RSIM_FutexTable()
{
    if (table) {
        munmap(table, sizeof(Table));
        table = NULL;
    }
    if (fd>=0) {
        close(fd);
        fd = -1;
    }
}

/* Lock must have already been obtained. */
int
RSIM_FutexTable::init_table()
{
    assert(table);
    memset(table, 0, sizeof(Table));
    table->magic = MAGIC;
    table->free_head = 1;
    assert(NMEMBERS>1);
    for (size_t i=1; i<NMEMBERS-1; i++) {
        table->member[i].next = i+1;
        table->member[i+1].prev = i;
    }
    return 0;
}

/* Lock must have already been obtained. */
int
RSIM_FutexTable::check_table()
{
    if (!table)
        return -EFAULT;
    if (table->magic != MAGIC)
        return -EINVAL;
    
    std::vector<bool> seen(false, NMEMBERS);
    for (size_t i=0; i<=NBUCKETS; i++) {
        size_t j = i<NBUCKETS ? table->bucket[i] : table->free_head;
        size_t prev = 0;
        for (/*void*/; j!=0; j=table->member[j].next) {
            if (j>=NMEMBERS || seen[j])
                return -EINVAL;
            if (table->member[j].prev!=prev)
                return -EINVAL;
            seen[j] = true;
            prev = j;
        }
    }

    if (seen[0])
        return -EINVAL;
    for (size_t i=1; i<NMEMBERS; i++) {
        if (!seen[i])
            return -EINVAL;
    }

    return 0;
}

/* Insert a new key into a wait queue and return its index.  Lock is acquired/released by this function. */
int
RSIM_FutexTable::insert(rose_addr_t key, uint32_t bitset, LockStatus lock_state)
{
    int retval = 0;

    if (0==bitset)
        return -EINVAL;

    /* Obtain lock if necessary */
    if (UNLOCKED==lock_state) {
        int status = sem_wait(semaphore);
        assert(0==status);
    }

    assert(table && table->magic==MAGIC);
    size_t bno = key % NBUCKETS;
    size_t mno = table->free_head;
    do {
        /* Allocate a new member */
        if (mno<=0) {
            retval = -EAGAIN; // no free space
            break;
        }
        table->free_head = table->member[mno].next;
        table->member[table->free_head].prev = 0;  // might initialize member[0].prev, but that's ok

        /* Link new member into bucket. */
        size_t old_head = table->bucket[bno];
        table->member[mno].next = old_head;
        table->member[mno].prev = 0;
        if (old_head>0)
            table->member[old_head].prev = mno;

        /* Initialize the member. */
        table->member[mno].bitset = bitset;
        int status = sem_init(&table->member[mno].sem, 1, 0);
        assert(0==status);
    } while (0);

    /* Release lock if we aquired it here. */
    if (UNLOCKED==lock_state) {
        int status = sem_post(semaphore);
        assert(0==status);
    }

    return 0==retval ? mno : retval;
}

/* Wait for the specified futex to be signaled. */
int
RSIM_FutexTable::wait(int mno)
{
    int status = sem_wait(&table->member[mno].sem);
    assert(0==status);
    return 0;
}

/* Releases the specified member index from the queue with the specified key.  THe lock is acquired/released by this function. */
int
RSIM_FutexTable::erase(rose_addr_t key, size_t member_number, LockStatus lock_state)
{
    /* Obtain lock if necessary */
    if (UNLOCKED==lock_state) {
        int status = sem_wait(semaphore);
        assert(0==status);
    }
    assert(table && table->magic==MAGIC);

    size_t bno = key % NBUCKETS;
    bool erased = false;

    /* Remove from queue */
    if (table->bucket[bno]==member_number) {
        size_t new_head = table->member[member_number].next;
        table->bucket[bno] = new_head;
        table->member[new_head].prev = 0; // might initialize member[0].prev, but that's ok
        erased = true;
    } else {
        for (size_t mno=table->bucket[bno]; mno!=0 && !erased; mno=table->member[mno].next) {
            if (mno==member_number) {
                if (table->member[mno].prev)
                    table->member[table->member[mno].prev].next = table->member[mno].next;
                if (table->member[mno].next)
                    table->member[table->member[mno].next].prev = table->member[mno].prev;
                erased = true;
            }
        }
    }

    if (erased) {
        /* Add to free list */
        if (table->free_head)
            table->member[table->free_head].prev = member_number;
        table->member[member_number].next = table->free_head;
        table->member[member_number].prev = 0;
        table->free_head = member_number;

        /* Destroy semaphore. */
        int status = sem_destroy(&table->member[member_number].sem);
        assert(0==status);
    }

    /* Release lock if we acquired it here. */
    if (UNLOCKED==lock_state) {
        int status = sem_post(semaphore);
        assert(0==status);
    }

    return erased ? 0 : -EINVAL;
}

int
RSIM_FutexTable::signal(rose_addr_t key, uint32_t bitset, int nprocs, LockStatus lock_state)
{
    int retval = 0;

    if (0==bitset)
        return 0;

    /* Obtain lock if necessary */
    if (UNLOCKED==lock_state) {
        int status = sem_wait(semaphore);
        assert(0==status);
    }
    assert(table && table->magic==MAGIC);

    size_t bno = key % NBUCKETS;

    for (size_t mno=table->bucket[bno]; mno!=0 && retval>=0 && retval<nprocs; mno=table->member[mno].next) {
        if (table->member[mno].key==key && 0!=(table->member[mno].bitset & bitset)) {
            table->member[mno].key = 0;
            sem_post(&table->member[mno].sem);
            retval++;
        }
    }

    /* Release lock if we acquired it here. */
    if (UNLOCKED==lock_state) {
        int status = sem_post(semaphore);
        assert(0==status);
    }

    return retval;
}

#endif
