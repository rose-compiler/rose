/*
 * TALC_runtime.cpp
 *
 *  Created on: Oct 23, 2012
 *      Modified by Kamal Sharma
 *      Original Code by Jeff Keasler
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <map>
#include <string>

#include "Meta.hpp"
#include "Field.hpp"
#include "Parser.hpp"

using namespace std;

static bool metaInitialized = false;
static Meta *meta;

/* This structure manages the memory pool.  There is an */
/* opportunity here for deallocating a pool when no */
/* pointers exist and reallocating on demand.  Also, */
/* we could add checks that the numEntities in the pool */
/* matches the numEntities allocated, check for memory leaks, etc. */

struct AllocationInfo {
   char *basePtr ;
   int numEntities ; /* number of values in this context */
   int allocBits ; /* bit vector indicating allocation call has occured */

   AllocationInfo(char *data, int numEnt, int bitIdx):
      basePtr(data), numEntities(numEnt), allocBits(1<<bitIdx) {}

   void Allocate(int bitIdx)
   {
      int bitMask = (1<<bitIdx) ;
      if ((allocBits & bitMask) != 0) {
         printf("Memory Leak detected\n") ;
      }
      allocBits |= bitMask ;
   }

   void Deallocate(int bitIdx)
   {
      allocBits &= (~ (1<<bitIdx) ) ;
   }
} ;

static map<const void *, map<FieldBlock*, AllocationInfo *> > allocateList;

Field *GetMeta(const char *name) {

	// Meta is not initialized
	if (!metaInitialized) {
		printf("INIT META\n");
		// Parse the Meta File
		Parser *parser = new Parser("Meta");
		meta = parser->parseFile();
		trace << " Printing Meta " << endl;
		meta->print();
		metaInitialized = true;
	}

	return meta->getField(name);
}

static void *AllocateMeta(const void *context, const char *name,
		const int numEntities) {
	char *retVal = NULL;

	Field *metaField = GetMeta(name);
	if (metaField != NULL) {
		if ((allocateList.find(context) == allocateList.end())
				|| (allocateList[context].find(metaField->getFieldBlock())
						== allocateList[context].end())) {
			// bytes = sizeof(type) * (no of elems in field block )
			int bytes = metaField->getElementSize() * metaField->getFieldBlock()->getSize();
			/* char *basePtr = (char *) malloc(bytes*numEntities) ; */
			printf(" TALC name: %s bytes:%d\n", name, bytes);
			char *basePtr;
			posix_memalign((void **) &basePtr, 16, bytes * numEntities);
			allocateList[context][metaField->getFieldBlock()] = new AllocationInfo(
					basePtr, numEntities, 0);
#ifdef SHOW_LAYOUT
			printf("start = %lx  end = %lx  len = %d",
					(long) basePtr,
					(long) (basePtr + bytes*numEntities),
					bytes*numEntities);
			printf("\n");
#endif
		}
		AllocationInfo *allocateInfo = allocateList[context][metaField->getFieldBlock()];
		if (numEntities != allocateInfo->numEntities) {
			printf("improper allocation for variable %s.\n", name);
			printf("numEntities does not match group size.\n");
			exit(-1);
		}
		int offset;
		if(metaField->getFieldBlock()->isBlocked())
			offset = metaField->getPosition()*metaField->getElementSize()*metaField->getFieldBlock()->getBlockSize();
		else
			offset = metaField->getPosition()*metaField->getElementSize();
		retVal = allocateInfo->basePtr + offset;
	}

	return retVal;
}

int *AllocateInt(const void *context, const char *name, const int numEntities) {
	return (int *) AllocateMeta(context, name, numEntities);
}

float *AllocateFloat(const void *context, const char *name,
		const int numEntities) {
	return (float *) AllocateMeta(context, name, numEntities);
}

double *AllocateDouble(const void *context, const char *name,
		const int numEntities) {
	return (double *) AllocateMeta(context, name, numEntities);
}

