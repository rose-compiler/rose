#ifndef VECTORCOMPRESSION_H
#define VECTORCOMPRESSION_H

#include <vector>
#include <stdint.h>
#include <stdlib.h>

std::vector<uint8_t> compressVector(const uint16_t data[], const size_t dataSize);
void decompressVector(const uint8_t compressedData[], size_t compressedDataSize, uint16_t result[]);
size_t getUncompressedSizeOfVector(const uint8_t compressedData[], size_t compressedDataSize);
size_t l1norm(const uint8_t compressedData[], size_t compressedDataSize);
double l2normSquared(const uint8_t compressedData[], size_t compressedDataSize);
double dotProduct(const uint8_t compressedData[], size_t compressedDataSize, const float* const otherVector);
void multipleDotProducts(const uint8_t compressedData[], size_t compressedDataSize, const float* const otherVector, const size_t numVectors, const size_t otherVectorStride, double result[]);
size_t l1distance(const uint8_t compressedData[], size_t compressedDataSize, const uint16_t* const otherVector);
double l2distanceSquared(const uint8_t compressedData[], size_t compressedDataSize, const uint16_t* const otherVector);
size_t l1distanceC(const uint8_t compressedData[], const size_t compressedDataSize, const uint8_t otherVectorCompressedData[], const size_t otherVectorCompressedDataSize);
double l2distanceSquaredC(const uint8_t compressedData[], const size_t compressedDataSize, const uint8_t otherVectorCompressedData[], const size_t otherVectorCompressedDataSize);
void elementwiseMax(const uint8_t compressedData[], size_t compressedDataSize, uint16_t v[]);
size_t computeL1Hash(const uint8_t compressedData[], size_t compressedDataSize, size_t hashElementCount, const size_t indexes[], const size_t compareValues[], const size_t coeffs[], size_t moduloValue);

#endif // VECTORCOMPRESSION_H
