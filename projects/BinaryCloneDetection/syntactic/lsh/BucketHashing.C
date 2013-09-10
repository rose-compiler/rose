/*
 * Copyright (c) 2004-2005 Massachusetts Institute of Technology.
 * All Rights Reserved.
 *
 * MIT grants permission to use, copy, modify, and distribute this software and
 * its documentation for NON-COMMERCIAL purposes and without fee, provided that
 * this copyright notice appears in all copies.
 *
 * MIT provides this software "as is," without representations or warranties of
 * any kind, either expressed or implied, including but not limited to the
 * implied warranties of merchantability, fitness for a particular purpose, and
 * noninfringement.  MIT shall not be liable for any damages arising from any
 * use of this software.
 *
 * Author: Alexandr Andoni (andoni@mit.edu), Piotr Indyk (indyk@mit.edu)
 */

#include "headers.h"
#include <vector>
using namespace std;

// Creates a new bucket with specified fields. The new bucket contains
// only a single entry -- bucketEntry. bucketEntry->nextEntry is
// expected to be NULL.
// PI-64
inline PGBucketT newGBucket(PUHashStructureT uhash, UnsT control1, /*PPointT point, */ IntT pointIndex, PGBucketT nextGBucket){
  PGBucketT bucket;
  if (uhash != NULL && uhash->unusedPGBuckets != NULL){
    bucket = uhash->unusedPGBuckets;
    uhash->unusedPGBuckets = uhash->unusedPGBuckets->nextGBucketInChain;
  } else {
    bucket = new GBucketT;
    //FAILIF(NULL == (bucket = (PGBucketT)MALLOC(sizeof(GBucketT))));
    nAllocatedGBuckets++;
  }
  ASSERT(bucket != NULL);
  bucket->controlValue1 = control1;
  bucket->firstEntry.pointIndex = pointIndex;
  bucket->firstEntry.nextEntry = NULL;
  bucket->nextGBucketInChain = nextGBucket;

  nGBuckets++;
  return bucket;
}

// Adds the entry <bucketEntry> to the bucket <bucket>.
// PI-64
inline void addPointToGBucket(PUHashStructureT uhash, PGBucketT bucket/*, PPointT point*/ , IntT pointIndex){
  ASSERT(bucket != NULL);
  ASSERT(uhash != NULL);

  // create a new bucket entry for the point
  TIMEV_START(timeBucketCreation);
  PBucketEntryT bucketEntry;
  if (uhash->unusedPBucketEntrys != NULL){
    bucketEntry = uhash->unusedPBucketEntrys;
    uhash->unusedPBucketEntrys = uhash->unusedPBucketEntrys->nextEntry;
  }else{
    //FAILIF(NULL == (bucketEntry = (PBucketEntryT)MALLOC(sizeof(BucketEntryT))));
    bucketEntry = new BucketEntryT;
    nAllocatedBEntries++;
  }
  ASSERT(bucketEntry != NULL);
  bucketEntry->pointIndex = pointIndex;
  TIMEV_END(timeBucketCreation);
  
  bucketEntry->nextEntry = bucket->firstEntry.nextEntry;
  bucket->firstEntry.nextEntry = bucketEntry;
}

// Creates a new UH structure (initializes the hash table and the hash
// functions used). If <typeHT>==HT_PACKED or HT_HYBRID_CHAINS, then
// <modelHT> gives the sizes of all the static arrays that are
// used. Otherwise parameter <modelHT> is not used.
// PI-64
PUHashStructureT newUHashStructure(IntT typeHT, IntT hashTableSize, IntT bucketVectorLength, BooleanT useExternalUHFs, vector<UnsT>& mainHashA, vector<UnsT>& controlHash1, PUHashStructureT modelHT){
  PUHashStructureT uhash;
  //FAILIF(NULL == (uhash = (PUHashStructureT)MALLOC(sizeof(UHashStructureT))));
  uhash = new UHashStructureT;
  uhash->typeHT = typeHT;
  uhash->hashTableSize = hashTableSize;
  uhash->nHashedBuckets = 0;
  uhash->nHashedPoints = 0;
  uhash->unusedPGBuckets = NULL;
  uhash->unusedPBucketEntrys = NULL;

  uhash->prime = UH_PRIME_DEFAULT;
  uhash->hashedDataLength = bucketVectorLength;

  uhash->chainSizes = NULL;
  uhash->bucketPoints.pointsArray = NULL;
  uhash->hybridChainsStorage = NULL;

  switch (typeHT) {
  case HT_LINKED_LIST:
    uhash->hashTable.llHashTable = new PGBucketT[hashTableSize];
    uhash->chainSizes = NULL;
    // PI-64
    for(IntT i = 0; i < hashTableSize; i++){
      uhash->hashTable.llHashTable[i] = NULL;
    }
    break;

  case HT_HYBRID_CHAINS: {
    // PI-64
    IntT indexInStorage = 0;
    IntT lastIndexInSt = 0;
    ASSERT(modelHT != NULL);
    ASSERT(modelHT->typeHT == HT_LINKED_LIST);
    //FAILIF(NULL == (uhash->hashTable.hybridHashTable = (PHybridChainEntryT*)MALLOC(hashTableSize * sizeof(PHybridChainEntryT))));
    //FAILIF(NULL == (uhash->hybridChainsStorage = (HybridChainEntryT*)MALLOC((modelHT->nHashedPoints + modelHT->nHashedBuckets) * sizeof(HybridChainEntryT))));
    uhash->hashTable.hybridHashTable = new PHybridChainEntryT[hashTableSize];
    uhash->hybridChainsStorage       = new HybridChainEntryT[ modelHT->nHashedPoints + modelHT->nHashedBuckets ];

    // the index of the first unoccupied entry in <uhash->hybridChainsStorage>.
    indexInStorage = 0; 

    // the index of the last unoccupied entry in <uhash->hybridChainsStorage>
    // (the entries of <uhash->hybridChainsStorage> are filled from start and from end:
    // at the beginning we fill the normal buckets with their points;
    // at the end we fill the "overflow" points of buckets (additional points of buckets that have
    // more than MAX_NONOVERFLOW_POINTS_PER_BUCKET points).
    lastIndexInSt = modelHT->nHashedPoints + modelHT->nHashedBuckets - 1; 

    // PI-64
    for(IntT i = 0; i < hashTableSize; i++){
      PGBucketT bucket = modelHT->hashTable.llHashTable[i];
      if (bucket != NULL){
        uhash->hashTable.hybridHashTable[i] = uhash->hybridChainsStorage + indexInStorage; // the position where the bucket starts
      }else{
        uhash->hashTable.hybridHashTable[i] = NULL;
      }
      while(bucket != NULL){
        // Compute number of points in the current bucket.
        // PI-64
        IntT nPointsInBucket = 1;
        PBucketEntryT bucketEntry = bucket->firstEntry.nextEntry;
        while(bucketEntry != NULL){
          nPointsInBucket++;
          bucketEntry = bucketEntry->nextEntry;
        }


        // Copy the points from the bucket to the new HT.
        ASSERT(nPointsInBucket > 0);

        uhash->hybridChainsStorage[indexInStorage].controlValue1 = bucket->controlValue1;
        indexInStorage++;
        uhash->hybridChainsStorage[indexInStorage].point.isLastBucket = (bucket->nextGBucketInChain == NULL ? 1 : 0);
        uhash->hybridChainsStorage[indexInStorage].point.bucketLength = (nPointsInBucket <= MAX_NONOVERFLOW_POINTS_PER_BUCKET ?
            nPointsInBucket : 
            0); // 0 means there are "overflow" points
        uhash->hybridChainsStorage[indexInStorage].point.isLastPoint = (nPointsInBucket == 1 ? 1 : 0);
        uhash->hybridChainsStorage[indexInStorage].point.pointIndex = bucket->firstEntry.pointIndex;
        indexInStorage++;

        // Store all other points in the storage
        // PI-64
        UnsT currentIndex = indexInStorage; // index where the "current" point will be stored.
        UnsT nOverflow = 0;
        UnsT overflowStart = lastIndexInSt;
        if (nPointsInBucket <= MAX_NONOVERFLOW_POINTS_PER_BUCKET){
          indexInStorage = indexInStorage + nPointsInBucket - 1;
        }else{
          // bucket too large.
          // store the overflow points at the end of the array <uhash->hybridChainsStorage>.
          nOverflow = nPointsInBucket - MAX_NONOVERFLOW_POINTS_PER_BUCKET;
          overflowStart = lastIndexInSt - nOverflow + 1;
          lastIndexInSt = overflowStart - 1;

          // specify the offset of the start of overflow points in the
          // fields <bucketLength> of points 2, 3, ... of the space
          // immediately after the bucket.
          // PI-64
          UnsT value = overflowStart - (currentIndex - 1 + MAX_NONOVERFLOW_POINTS_PER_BUCKET);
          for(IntT j = 0; j < N_FIELDS_PER_INDEX_OF_OVERFLOW; j++){
            uhash->hybridChainsStorage[currentIndex + j].point.bucketLength = value & ((1U << N_BITS_FOR_BUCKET_LENGTH) - 1);
            value = value >> N_BITS_FOR_BUCKET_LENGTH;
          }

          // update <indexInStorage>
          indexInStorage = indexInStorage + MAX_NONOVERFLOW_POINTS_PER_BUCKET - 1;
          ASSERT(indexInStorage <= lastIndexInSt + 1);

          //FAILIFWR(nPointsInBucket > MAX_NONOVERFLOW_POINTS_PER_BUCKET, "Too many points in a bucket -- feature not implemented yet. Try to lower N_BITS_PER_POINT_INDEX as much as possible.");// TODO: not implemented yet
        }

        bucketEntry = bucket->firstEntry.nextEntry;
        while(bucketEntry != NULL){
          uhash->hybridChainsStorage[currentIndex].point.pointIndex = bucketEntry->pointIndex;
          uhash->hybridChainsStorage[currentIndex].point.isLastPoint = 0;
          bucketEntry = bucketEntry->nextEntry;

          currentIndex++;
          if ((int)currentIndex == indexInStorage && nPointsInBucket > MAX_NONOVERFLOW_POINTS_PER_BUCKET){
            // finished the normal alloted space -> going to the space reserved at end of the table.
            currentIndex = overflowStart;
          }
        }

        // set the <isLastBucket> field of the last point = 1.
        uhash->hybridChainsStorage[currentIndex - 1].point.isLastPoint = 1;

        bucket = bucket->nextGBucketInChain;
        //ASSERT((uhash->hashTable.hybridHashTable[i] + 1)->point.bucketLength > 0);
      }
    }
    ASSERT(indexInStorage == lastIndexInSt + 1);
    uhash->nHashedPoints = modelHT->nHashedPoints;
    uhash->nHashedBuckets = modelHT->nHashedBuckets;
    break;
  }

  default: assert (!"Bad hash type");
  }

  // Initializing the main hash function.
  if (!useExternalUHFs){
    //FAILIF(NULL == (uhash->mainHashA = (UnsT*)MALLOC(uhash->hashedDataLength * sizeof(UnsT))));
    uhash->mainHashA.resize(uhash->hashedDataLength);
    for(IntT i = 0; i < uhash->hashedDataLength; i++){
      uhash->mainHashA[i] = genRandomUnsT(1, MAX_HASH_RND);
    }
    mainHashA = uhash->mainHashA;
  } else {
    uhash->mainHashA = mainHashA;
  }

  // Initializing the control hash functions.
  if (!useExternalUHFs){
    //FAILIF(NULL == (uhash->controlHash1 = (UnsT*)MALLOC(uhash->hashedDataLength * sizeof(UnsT))));
    uhash->controlHash1.resize(uhash->hashedDataLength);
    for(IntT i = 0; i < uhash->hashedDataLength; i++){
      uhash->controlHash1[i] = genRandomUnsT(1, MAX_HASH_RND);
    }
    controlHash1 = uhash->controlHash1;
  } else {
    uhash->controlHash1 = controlHash1;
  }

  return uhash;
}

// Removes all the buckets/points from the hash table. Used only for
// HT_LINKED_LIST.
void clearUHashStructure(PUHashStructureT uhash){
  ASSERT(uhash != NULL);
  switch (uhash->typeHT) {
  case HT_LINKED_LIST:
      // PI-64
    for(IntT i = 0; i < uhash->hashTableSize; i++){
      PGBucketT bucket = uhash->hashTable.llHashTable[i];
      while(bucket != NULL){
	PGBucketT tempBucket = bucket;
	bucket = bucket->nextGBucketInChain;
	tempBucket->nextGBucketInChain = uhash->unusedPGBuckets;
	uhash->unusedPGBuckets = tempBucket;
	
	PBucketEntryT bucketEntry = tempBucket->firstEntry.nextEntry;
	while(bucketEntry != NULL){
	  PBucketEntryT tempEntry = bucketEntry;
	  bucketEntry = bucketEntry->nextEntry;
	  tempEntry->nextEntry = uhash->unusedPBucketEntrys;
	  uhash->unusedPBucketEntrys = tempEntry;
	}
      }
      
      uhash->hashTable.llHashTable[i] = NULL;
    }
    break;
  case HT_STATISTICS:
    ASSERT(FALSE); // Not supported.
//     for(IntT i = 0; i < uhash->hashTableSize; i++){
//       uhash->hashTable.linkHashTable[i][0].indexStart = INDEX_START_EMPTY;
//     }
    break;
  default:
    ASSERT(FALSE);
  }
  uhash->nHashedPoints = 0;
  uhash->nHashedBuckets = 0;
}


// Frees the <uhash> structure. If <freeHashFunctions>==FALSE, then
// the hash functions are not freed (because they might be reused, and
// therefore shared by several PUHashStructureT structures).
void freeUHashStructure(PUHashStructureT uhash, BooleanT freeHashFunctions){
  if (uhash == NULL){
    return;
  }
  switch (uhash->typeHT) {
  case HT_LINKED_LIST:
    break;
    for(IntT i = 0; i < uhash->hashTableSize; i++){
      PGBucketT bucket = uhash->hashTable.llHashTable[i];
      while (bucket != NULL){
	PGBucketT tempBucket = bucket;
	bucket = bucket->nextGBucketInChain;
	PBucketEntryT bucketEntry = tempBucket->firstEntry.nextEntry;
	while (bucketEntry != NULL){
	  PBucketEntryT tempEntry = bucketEntry;
	  bucketEntry = bucketEntry->nextEntry;
	  delete tempEntry;
	}
	delete tempBucket;
      }
    }
    delete[] uhash->hashTable.llHashTable;
    if (uhash->unusedPGBuckets != NULL){
      PGBucketT bucket = uhash->unusedPGBuckets;
      while (bucket != NULL){
	PGBucketT tempBucket = bucket;
	bucket = bucket->nextGBucketInChain;
	delete tempBucket;
      }
    }
    if (uhash->unusedPBucketEntrys != NULL){
      PBucketEntryT bucketEntry = uhash->unusedPBucketEntrys;
      while (bucketEntry != NULL){
	PBucketEntryT tempEntry = bucketEntry;
	bucketEntry = bucketEntry->nextEntry;
	delete tempEntry;
      }
    }
    ASSERT(uhash->chainSizes == NULL);
    break;
  case HT_HYBRID_CHAINS:

    ASSERT (uhash->typeHT == HT_HYBRID_CHAINS);
    delete[] uhash->hashTable.hybridHashTable;
    delete[] uhash->hybridChainsStorage;
    ASSERT(uhash->chainSizes == NULL);
    break;
  default:
    ASSERT(FALSE);
  }


  delete uhash;
}

#if MACHINE_WORD_SIZE == 64

static inline uint64_t mulhi(uint64_t a, uint64_t b) {
  uint64_t z1, z2;
  asm ("mulq %3" : "=a"(z1), "=d"(z2) : "a"(a), "r"(b));
  return z2;
}

static inline uint64_t modmac(uint64_t a, uint64_t b, uint64_t s, uint64_t minusM) {
  uint64_t ab = a * b;
  uint64_t abHigh = mulhi(a, b);
  uint64_t oldAb = ab;
  ab += s;
  if (ab < oldAb) ++abHigh; // Carry
  while (abHigh != 0) {
    uint64_t newAb = abHigh * minusM + ab;
    uint64_t newAbHigh = mulhi(abHigh, minusM) + (newAb < ab);
    ab = newAb;
    abHigh = newAbHigh;
  }
  if (ab >= -minusM) {ab += minusM;}
  return ab;
}

// Computes (a.b)mod UH_PRIME_DEFAULT. b is coded as <nBPieces> blocks
// of size totaling <size>. <a> is of length <size>.
// PI-64
inline UnsT computeBlockProductModDefaultPrime(const vector<UnsT>& a, const vector<vector<UnsT> >& b, IntT nBPieces, IntT size){
  const UnsT minusPrime = (-(UnsT)UH_PRIME_DEFAULT);
  LongUns64T h = 0;
  IntT bPieceSize = size / nBPieces;
  IntT i = 0;
  for(IntT bPiece = 0; bPiece < nBPieces; bPiece++){
    for(IntT j = 0; j < bPieceSize; j++, i++){
      h = modmac(a[i], b[bPiece][j], h, minusPrime);
      // PI-64
      CR_ASSERT(h < UH_PRIME_DEFAULT);
    }
  }
  return h;
}

#else // Word size == 32

// Computes (a.b)mod UH_PRIME_DEFAULT. b is coded as <nBPieces> blocks
// of size totaling <size>. <a> is of length <size>.
// PI-64
inline UnsT computeBlockProductModDefaultPrime(const vector<UnsT>& a, const vector<vector<UnsT> >& b, IntT nBPieces, IntT size){
  LongUns64T h = 0;
  IntT j = 0;
  IntT bPiece = 0;
  IntT bPieceSize = size / nBPieces;
  IntT i = 0;
  std::cout << "bPieceSize: " << bPieceSize << " a.size: " << a.size() << "b.size: "<< b.size() << " size : " << size << std::endl;
  for(IntT bPiece = 0; bPiece < nBPieces; bPiece++){
    for(IntT j = 0; j < bPieceSize; j++, i++){
      ASSERT(i < a.size() );
      ASSERT(j < b[bPieceSize].size());
      ASSERT(bPieceSize < b.size());
      h = h + (LongUns64T)a[i] * b[bPieceSize][j];
      h %= UH_PRIME_DEFAULT;
    }
  }
  return h;
}

#endif

// Computes (a.b)mod UH_PRIME_DEFAULT.
// PI-64
inline UnsT computeProductModDefaultPrime(const vector<UnsT>& a, UnsT aOffset, const vector<UnsT>& b, IntT size){
  return computeBlockProductModDefaultPrime((aOffset == 0 ? a : vector<UnsT>(a.begin() + aOffset, a.end())), vector<vector<UnsT> >(1, b), 1, size);
}

// Compute fuction ((rndVector . data)mod prime)mod hashTableSize
// Vectors <rndVector> and <data> are assumed to have length <size>.
// PI-64
inline UnsT computeUHashFunction(const vector<UnsT>& rndVector, const vector<vector<UnsT> >& data, IntT nDataPieces, IntT size, UnsT prime, IntT hashTableSize){
  ASSERT(prime == UH_PRIME_DEFAULT);

  // PI-64
  UnsT h = computeBlockProductModDefaultPrime(rndVector, data, nDataPieces, size) % hashTableSize;

  ASSERT(h >= 0 && (int)h < hashTableSize);

  return h;
}

// PI-64
inline UnsT combinePrecomputedHashes(const vector<UnsT>& firstBucketVector, const vector<UnsT>& secondBucketVector, IntT nBucketVectorPieces, IntT uhfIndex){
  // CR_ASSERT(bucketVector != NULL);
//   if (nBucketVectorPieces == 1) {
//     // using normal <g> functions.
//     CR_ASSERT(bucketVector[1] != NULL);
//     return (bucketVector[1][uhfIndex] % UH_PRIME_DEFAULT);
//   } else {
//     CR_ASSERT(nBucketVectorPieces == 2); // each of the <g> functions is a pair of 2 <u> functions.
//     //CR_ASSERT(bucketVector[2] != NULL);
//     //CR_ASSERT(bucketVector[3] != NULL);
//     LongUns64T r = (LongUns64T)(bucketVector[2][uhfIndex]) + (LongUns64T)(bucketVector[3][uhfIndex + UHF_NUMBER_OF_HASHES]);
//     if (r >= UH_PRIME_DEFAULT) {
//       r -= UH_PRIME_DEFAULT;
//     }
//     CR_ASSERT(r < UH_PRIME_DEFAULT);
//     return (UnsT)r;
//   }
  if (nBucketVectorPieces == 1) {
    // using normal <g> functions.
    // PI-64
    UnsT h = firstBucketVector[uhfIndex];
    if (h > UH_PRIME_DEFAULT){
      h = h - UH_PRIME_DEFAULT;
    }
    return h;
  } else {
    CR_ASSERT(nBucketVectorPieces == 2); // each of the <g> functions is a pair of 2 <u> functions.
    //CR_ASSERT(bucketVector[2] != NULL);
    //CR_ASSERT(bucketVector[3] != NULL);
    LongUns64T r = (LongUns64T)(firstBucketVector[uhfIndex]) + (LongUns64T)(secondBucketVector[uhfIndex + UHF_NUMBER_OF_HASHES]);
    if (r >= UH_PRIME_DEFAULT) {
      r -= UH_PRIME_DEFAULT;
    }
    CR_ASSERT(r < UH_PRIME_DEFAULT);
    // PI-64
    return (UnsT)r;
  }
}

// Adds the bucket entry (a point <point>) to the bucket defined by
// bucketVector in the uh structure with number uhsNumber. If no such
// bucket exists, then it is first created.
// PI-64
void addBucketEntry(PUHashStructureT uhash, IntT nBucketVectorPieces, const vector<UnsT>& firstBucketVector, const vector<UnsT>& secondBucketVector/*, PPointT point*/ , IntT pointIndex){
  CR_ASSERT(uhash != NULL);
  // CR_ASSERT(bucketVector != NULL);

  // PI-64
  UnsT hIndex;
  UnsT control1;

  if (!USE_PRECOMPUTED_HASHES){
    // if not using the same hash functions across multiple
    // UHashStructureT, then we need to compute explicitly the hases.
    // PI-64
    vector<vector<UnsT> > tempVector(2);
    tempVector[0] = firstBucketVector;
    tempVector[1] = secondBucketVector;
    hIndex = computeUHashFunction(uhash->mainHashA, tempVector, nBucketVectorPieces, uhash->hashedDataLength, uhash->prime, uhash->hashTableSize);
    control1 = computeBlockProductModDefaultPrime(uhash->controlHash1, tempVector, nBucketVectorPieces, uhash->hashedDataLength);
  } else {
    // if using the same hash functions across multiple
    // UHashStructureT, then we can use the (possibly partially)
    // precomputed hash values.
    CR_ASSERT(uhash->prime == UH_PRIME_DEFAULT);
    hIndex = combinePrecomputedHashes(firstBucketVector, secondBucketVector, nBucketVectorPieces, UHF_MAIN_INDEX) % uhash->hashTableSize;
    control1 = combinePrecomputedHashes(firstBucketVector, secondBucketVector, nBucketVectorPieces, UHF_CONTROL1_INDEX);
  }

  PGBucketT p;
  // PI-64
  switch (uhash->typeHT) {
  case HT_LINKED_LIST:
    p = uhash->hashTable.llHashTable[hIndex];
    while(p != NULL && 
	  (p->controlValue1 != control1)) {
      p = p->nextGBucketInChain;
    }
    if (p == NULL) {
      // new bucket to add to the hash table
      uhash->nHashedBuckets++;
      uhash->hashTable.llHashTable[hIndex] = newGBucket(uhash,
							control1, 
							pointIndex, 
							uhash->hashTable.llHashTable[hIndex]);
    } else {
      // add this bucket entry to the existing bucket
      addPointToGBucket(uhash, p, pointIndex);
    }
    break;
  case HT_PACKED:
//     // The bucket should already exist.
//     IntT i;
//     for(i = 0; i < uhash->chainSizes[hIndex] && uhash->hashTable.packedHashTable[hIndex][i].nPointsInBucket > 0; i++){
//       if (uhash->hashTable.packedHashTable[hIndex][i].controlValue1 == control1){
// 	break;
//       }
//     }
//     uhash->hashTable.packedHashTable[hIndex][i].nPointsInBucket++;
    break;
  case HT_STATISTICS:
    ASSERT(FALSE);
//     uhash->bucketPoints.pointsList[uhash->nHashedPoints].point = point;
//     found = FALSE;
//     for(j = 0; j < uhash->chainSizes[hIndex] && uhash->hashTable.linkHashTable[hIndex][j].indexStart != INDEX_START_EMPTY; j++){
//       if (uhash->hashTable.linkHashTable[hIndex][j].controlValue1 == control1){
// 	found = TRUE;
// 	break;
//       }
//     }
//     if (!found) {
//       // new bucket
//       if (j >= uhash->chainSizes[hIndex]) {
// 	// dont have enough space in pREALLOCated memory.
// 	if (uhash->chainSizes[hIndex] > 0) {
// 	  uhash->chainSizes[hIndex] = CEIL(uhash->chainSizes[hIndex] * CHAIN_RESIZE_RATIO);
// 	}else{
// 	  uhash->chainSizes[hIndex] = 1;
// 	}
// 	uhash->hashTable.linkHashTable[hIndex] = (LinkPackedGBucketT*)REALLOC(uhash->hashTable.linkHashTable[hIndex], uhash->chainSizes[hIndex] * sizeof(LinkPackedGBucketT));
// 	uhash->hashTable.linkHashTable[hIndex][j].controlValue1 = control1;
//       }
//       uhash->hashTable.linkHashTable[hIndex][j].controlValue1 = control1;
//       uhash->hashTable.linkHashTable[hIndex][j].indexStart = INDEX_START_EMPTY;
//       uhash->nHashedBuckets++;
//       if (j + 1 < uhash->chainSizes[hIndex]){
// 	uhash->hashTable.linkHashTable[hIndex][j + 1].indexStart = INDEX_START_EMPTY;
//       }
//     }
    
//     temp = uhash->hashTable.linkHashTable[hIndex][j].indexStart;
//     uhash->hashTable.linkHashTable[hIndex][j].indexStart = uhash->nHashedPoints;
//     uhash->bucketPoints.pointsList[uhash->nHashedPoints].nextPoint = temp;

    break;
  default:
    ASSERT(FALSE);
  }
  uhash->nHashedPoints++;
}

// Returns the bucket defined by the vector <bucketVector> in the UH
// structure number <uhsNumber>.
// PI-64
GeneralizedPGBucket getGBucket(PUHashStructureT uhash, IntT nBucketVectorPieces, const vector<UnsT>& firstBucketVector, const vector<UnsT>& secondBucketVector){
  UnsT hIndex;
  UnsT control1;

  //TIMEV_START(timeGBHash);
  if (!USE_PRECOMPUTED_HASHES){
    // if not using the same hash functions across multiple
    // UHashStructureT, then we need to compute explicitly the hases.
    // PI-64
    vector<vector<UnsT> > tempVector(2);
    tempVector[0] = firstBucketVector;
    tempVector[1] = secondBucketVector;
    hIndex = computeUHashFunction(uhash->mainHashA, tempVector, nBucketVectorPieces, uhash->hashedDataLength, uhash->prime, uhash->hashTableSize);
    control1 = computeBlockProductModDefaultPrime(uhash->controlHash1, tempVector, nBucketVectorPieces, uhash->hashedDataLength);
  } else {
    // if using the same hash functions across multiple
    // UHashStructureT, then we can use the (possibly partially)
    // precomputed hash values.
    CR_ASSERT(uhash->prime == UH_PRIME_DEFAULT);
    hIndex = combinePrecomputedHashes(firstBucketVector, secondBucketVector, nBucketVectorPieces, UHF_MAIN_INDEX) % uhash->hashTableSize;
    control1 = combinePrecomputedHashes(firstBucketVector, secondBucketVector, nBucketVectorPieces, UHF_CONTROL1_INDEX);
  }
  //TIMEV_END(timeGBHash);

  GeneralizedPGBucket result;
  PGBucketT p;
  PHybridChainEntryT indexHybrid = NULL;
  //TIMEV_START(timeChainTraversal);
  switch(uhash->typeHT) {
  case HT_LINKED_LIST:
    p = uhash->hashTable.llHashTable[hIndex];
    while(p != NULL && 
	  (p->controlValue1 != control1)) {
      p = p->nextGBucketInChain;
      nBucketsInChains++;
    }
    result.llGBucket = p;
    return result;
  case HT_STATISTICS:
    ASSERT(FALSE); // HT_STATISTICS not supported anymore
    for(IntT j = 0; j < uhash->chainSizes[hIndex] && uhash->hashTable.linkHashTable[hIndex][j].indexStart != INDEX_START_EMPTY; j++){
      if (uhash->hashTable.linkHashTable[hIndex][j].controlValue1 == control1){
	result.linkGBucket = &(uhash->hashTable.linkHashTable[hIndex][j]);
	return result;
      }
    }
    result.linkGBucket = NULL;
    return result;
  case HT_PACKED:
    ASSERT(FALSE); // HT_PACKED not supported anymore
    for(IntT j = 0; j < uhash->chainSizes[hIndex]; j++){
      if (uhash->hashTable.packedHashTable[hIndex][j].controlValue1 == control1){
	result.packedGBucket = &(uhash->hashTable.packedHashTable[hIndex][j]);
	return result;
      }
    }
    result.packedGBucket = NULL;
    return result;
  case HT_HYBRID_CHAINS:
    indexHybrid = uhash->hashTable.hybridHashTable[hIndex];
    while (indexHybrid != NULL){ 
      if (indexHybrid->controlValue1 == control1){
	result.hybridGBucket = indexHybrid + 1;
	return result;
      }else{
	indexHybrid = indexHybrid + 1;
	if (indexHybrid->point.isLastBucket != 0){
	  result.hybridGBucket = NULL;
	  return result;
	}
	indexHybrid = indexHybrid + indexHybrid->point.bucketLength;
      }
    }
    result.hybridGBucket = NULL;
    return result;
    break;
  default:
    ASSERT(FALSE);
  }
  //TIMEV_END(timeChainTraversal);

}

// PI-64
void precomputeUHFsForULSH(PUHashStructureT uhash, const std::vector<UnsT>& uVector, IntT length, std::vector<UnsT>& result){
  ASSERT((int)uVector.size() == length);
  ASSERT((int)uhash->mainHashA.size() == uhash->hashedDataLength);
  ASSERT((int)uhash->controlHash1.size() == uhash->hashedDataLength);
  if (length == uhash->hashedDataLength){
    result[UHF_MAIN_INDEX] = computeProductModDefaultPrime(uhash->mainHashA, 0, uVector, length);
    result[UHF_CONTROL1_INDEX] = computeProductModDefaultPrime(uhash->controlHash1, 0, uVector, length);
  } else {
    ASSERT(2 * length == uhash->hashedDataLength); // the length is 1/2 of the bucket length
    result[UHF_MAIN_INDEX] = computeProductModDefaultPrime(uhash->mainHashA, 0, uVector, length);
    result[UHF_CONTROL1_INDEX] = computeProductModDefaultPrime(uhash->controlHash1, 0, uVector, length);
    result[UHF_MAIN_INDEX + UHF_NUMBER_OF_HASHES] = computeProductModDefaultPrime(uhash->mainHashA, length, uVector, length);
    result[UHF_CONTROL1_INDEX + UHF_NUMBER_OF_HASHES] = computeProductModDefaultPrime(uhash->controlHash1, length, uVector, length);
  }
}

