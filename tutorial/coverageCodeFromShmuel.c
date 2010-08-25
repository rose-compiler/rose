#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#define TIMESTAMP_LEN 20

typedef enum {FALSE, TRUE} bool;

#define HASH_LOAD_FACTOR 0.75



struct tm *getIsoTm(const struct timeval *t) {
   return localtime((time_t*)(&(t->tv_sec)));
}

int getHundredthOfSeconds(const struct timeval *t) {
   return (int)(t->tv_usec / 10000);
}


/*
 * Creates a new coverage file. Expected to be called once in a program run.
 * Returns NULL if failed. change buffer options here, if desired. Note fflush below.
*/
static FILE *openCoverageFile() {
   char timestamp[TIMESTAMP_LEN];
   char name[6 + TIMESTAMP_LEN + 6 + 1];  /* synch_ = 6 + timestamp = 20 + .trace = 6 + null=1 */
   FILE *file = NULL;
   struct timeval val;

   gettimeofday(&val,NULL);
   const struct tm * const ts = getIsoTm(&val);


   sprintf(timestamp, "%02d_%02d_%02d_%02d_%02d_%02d_%02d",
               ts->tm_year % 100, ts->tm_mon, ts->tm_mday, ts->tm_hour, ts->tm_min, ts->tm_sec,
               getHundredthOfSeconds(&val));
   strcpy(name, "synch_");
   strcat(name, timestamp);
   strcat(name, ".trace");

   file = fopen(name, "w");
   if (file == NULL) {
      fprintf(stderr,"\nERROR : UNABLE TO OPEN FILE\n");
      exit(1);
   }

   return file;
}

/*
 * Print a coverage task to the file, as one line. The line is composed of the four arguments
 * separated by spaces. The format is: source file name, space, line number,
 * space, type, sp.
 * The file is then flushed. File is opened upon first invocation, and never closed explicitly.
*/
static void reportTask(const char *str) {
   static bool fileErrorOccured = FALSE; /*once it is turned on, no further attempt to print will be done.*/
   if ( ! fileErrorOccured) {
      static FILE *covFile = NULL; /*opened upon first print.*/
      if (covFile == NULL) {
         covFile = openCoverageFile();
         if (covFile == NULL) {
            fileErrorOccured = TRUE;
            return;
         }
      }
      if (fprintf(covFile, "%s \n", str) == EOF || fflush(covFile)) {
         fprintf(stderr, "fprintf or fflush failed\n");
         fileErrorOccured = TRUE;
      }
   }
}


static unsigned int hash_value(const char *str)
{
    unsigned int hash_value=0;
    for(;*str;str++)
    {
		hash_value+= *str;
    }
    return hash_value;
}

/*
 * Handle a newly found coverage task: check if it is in the set. If not, print it (using reportTask()), and add it
 * to the set. If needed, resize the hashtable used for the set.
 * Return non-zero if some problem occured. 1 is out of memory. Assumed to be called within internal lock.
*/
static int taskEncountered(const char *str)
{
   typedef struct taskListCell
         {struct taskListCell *next; const char *str;}
         taskListCell;
   static int capacity = 64; /*initial hash table size. When testing, try setting it at very low value (e.g. 3), so that*/
                              /*resize will occur and will be tested.*/
   static taskListCell **taskSet = NULL; /*hashtable: array of lists of taskListCell. It is initialized to size*/
                              /*capacity, but can be reallocated (resize), hence it is allocated dynamically.*/
   static int setSize;

   taskListCell *iter, *newOne;
   int hashCode = hash_value(str);
   int index = hashCode % capacity;

   if (taskSet == NULL) { /*initialize hash table*/
      taskSet = (taskListCell**)calloc(capacity,sizeof(taskListCell*));
      if (taskSet == NULL)
      {
         fprintf(stderr, "error while allocating memory\n");
         return -1;
      }

      setSize = 0;
   }

   for (iter = taskSet[index]; iter != NULL; iter = iter->next) {
      if (!strcmp(iter->str, str)) {
         return 0; /*task already in set*/
      }
   }
   /*task not in set - report it, and add it as the head to the relevant list*/
   reportTask(str);

   newOne = (taskListCell*)malloc(sizeof(taskListCell));
   newOne->str = strdup(str);
   newOne->next = taskSet[index];
   taskSet[index] = newOne;

   setSize++;
   /*check if hash table resize is needed*/
   if (setSize > HASH_LOAD_FACTOR * capacity) {
      /*resize needed: double the capacity*/
      int newCapacity = capacity * 2;
      int i;
      taskListCell **newHashTable;
      newHashTable = (taskListCell**)calloc(newCapacity,sizeof(taskListCell*));
      /*go over all tasks in the old taskSet. Insert each one to the new table, according to it's hash code.*/
      for (i=0; i<capacity; i++) {
         for (iter = taskSet[i]; iter != NULL; ) {

            taskListCell* next = iter->next; /*don't do it in the for(), since iter->next will change in the*/
                                            /*iteration block!*/
            hashCode = hash_value(iter->str);

            index = hashCode % newCapacity;
            /*insert cur at the head of the appropriate list*/
            iter->next = newHashTable[index];
            newHashTable[index] = iter;
            iter = next; /*continue iteration over list*/
         }
      }
      free(taskSet);
      taskSet = newHashTable;
      capacity = newCapacity;
   }
   return 0;
}

void coverageTraceFunc1(const char* FileMethod)
{
   static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


   pthread_mutex_lock(&lock);

   // write data
   taskEncountered(FileMethod);

   pthread_mutex_unlock(&lock);
   return;

}

