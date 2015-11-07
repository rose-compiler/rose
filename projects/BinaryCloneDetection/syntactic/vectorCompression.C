#include "vectorCompression.h"
#include <vector>
#include <stdint.h>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstring>

using namespace std;

// Coding scheme:
// A byte b is decoded as:
// <b> if 0x00 <= b < 0x40
// <(b >> 3) & 7, b & 7> if 0x40 <= b < 0x80
// <(b >> 4) & 3, (b >> 2) & 3, b & 3> if 0x80 <= b < 0xC0
// b - 0xC0 + 1 zero elements if 0xC0 <= b < 0xD0
// (b - 0xD0 + 1) * 16 zero elements if 0xD0 <= b < 0xE0
// If 0xE0 <= b < 0x100: (all remaining byte values)
//   acc = b - 0xE0
//   while (true) {
//     c = byte from input
//     acc = (acc << 7) | (c & 0x7F)
//     if (c >= 0x80) break
//   }

template <typename ResultHandler>
static inline void decompressVectorBase(const uint8_t compressedData[], size_t compressedDataSize, ResultHandler& rh) {
    size_t idx = 0;
    for (size_t i = 0; i < compressedDataSize; ++i) {
        uint8_t elt = compressedData[i];
        if ((elt & 0xF0) == 0xC0) {
            rh.zeroBlock(idx, elt - 0xC0 + 1);
            idx += (elt - 0xC0 + 1);
        } else if ((elt & 0xF0) == 0xD0) {
            rh.zeroBlock(idx, (elt - 0xD0 + 1) * 16);
            idx += (elt - 0xD0 + 1) * 16;
        } else if (elt < 0x40) {
            rh.element(idx, elt);
            ++idx;
        } else if (elt < 0x80) {
            rh.element(idx, (elt >> 3) & 7);
            rh.element(idx + 1, elt & 7);
            idx += 2;
        } else if (elt < 0xC0) {
            rh.element(idx, (elt >> 4) & 3);
            rh.element(idx + 1, (elt >> 2) & 3);
            rh.element(idx + 2, elt & 3);
            idx += 3;
        } else {
            size_t acc = (elt - 0xE0);
            // cerr << "E and F loop" << endl;
            while (true) {
                ++i;
                assert (i < compressedDataSize);
                uint8_t c = compressedData[i];
                // cerr << "At " << i << " in loop, got elt " << std::hex << (uint32_t)c << endl;
                acc = (acc << 7) | (c & 0x7F);
                if (c >= 0x80) break;
            }
            rh.element(idx, acc);
            ++idx;
        }
    }
    rh.end(idx);
}

struct OutputWriter {
    uint16_t* const resultPtr;

    OutputWriter(uint16_t* resultPtr): resultPtr(resultPtr) {}
    void element(size_t idx, size_t elt) {resultPtr[idx] = elt;}
    void zeroBlock(size_t idx, size_t size) {memset(resultPtr + idx, 0, sizeof(uint16_t) * size);}
    void end(size_t idx) {}
};

void decompressVector(const uint8_t compressedData[], size_t compressedDataSize, uint16_t result[])
{
    // Result must have the correct size -- use getUncompressedSizeOfVector to get it
    OutputWriter ow(result);
    decompressVectorBase(compressedData, compressedDataSize, ow);
}

struct SizeWriter {
    size_t size;
    void element(size_t idx, size_t elt) {}
    void zeroBlock(size_t idx, size_t size) {}
    void end(size_t idx) {size = idx;}
};

size_t getUncompressedSizeOfVector(const uint8_t compressedData[], size_t compressedDataSize) {
    SizeWriter sw;
    decompressVectorBase(compressedData, compressedDataSize, sw);
    return sw.size;
}

struct L1NormWriter {
    size_t norm;
    L1NormWriter(): norm(0) {}
    void element(size_t idx, size_t elt) {norm += elt;}
    void zeroBlock(size_t idx, size_t size) {}
    void end(size_t idx) {}
};

size_t l1norm(const uint8_t compressedData[], size_t compressedDataSize) {
    L1NormWriter lnw;
    decompressVectorBase(compressedData, compressedDataSize, lnw);
    return lnw.norm;
}

struct L2NormSquaredWriter {
    double normSquared;
    L2NormSquaredWriter(): normSquared(0.0) {}
    void element(size_t idx, size_t elt) {double d = elt; normSquared += d * d;}
    void zeroBlock(size_t idx, size_t size) {}
    void end(size_t idx) {}
};

double l2normSquared(const uint8_t compressedData[], size_t compressedDataSize) {
    L2NormSquaredWriter lnw;
    decompressVectorBase(compressedData, compressedDataSize, lnw);
    return lnw.normSquared;
}

struct MultipleDotProductWriter {
    double* const dotProducts;
    const float* const otherVector;
    const size_t numOtherVectors;
    const size_t otherVectorStride;

    MultipleDotProductWriter(const float* const otherVector, const size_t numOtherVectors, const size_t otherVectorStride,
                             double* const dotProducts)
        : dotProducts(dotProducts), otherVector(otherVector), numOtherVectors(numOtherVectors),
          otherVectorStride(otherVectorStride) {
        for (size_t i = 0; i < numOtherVectors; ++i) {
            dotProducts[i] = 0.0;
        }
    }
    void element(size_t idx, size_t elt) {
        double d = elt;
        for (size_t i = 0; i < numOtherVectors; ++i) {
            dotProducts[i] += d * otherVector[i * otherVectorStride + idx];
        }
    }
    void zeroBlock(size_t idx, size_t size) {}
    void end(size_t idx) {}
};

double dotProduct(const uint8_t compressedData[], size_t compressedDataSize, const float* const otherVector) {
    double dp[1];
    MultipleDotProductWriter dpw(otherVector, 1, 0, dp);
    decompressVectorBase(compressedData, compressedDataSize, dpw);
    return dp[0];
}

void multipleDotProducts(const uint8_t compressedData[], size_t compressedDataSize, const float* const otherVector,
                         const size_t numVectors, const size_t otherVectorStride, double result[]) {
    MultipleDotProductWriter dpw(otherVector, numVectors, otherVectorStride, result);
    decompressVectorBase(compressedData, compressedDataSize, dpw);
}

struct L1DistanceWriter {
    size_t dist;
    const uint16_t* const otherVector;
    L1DistanceWriter(const uint16_t* const otherVector): dist(0), otherVector(otherVector) {}
    void element(size_t idx, size_t elt) {dist += labs((long)otherVector[idx] - elt);}
    void zeroBlock(size_t idx, size_t size) {for (size_t i = 0; i < size; ++i) dist += otherVector[idx + i];}
    void end(size_t idx) {}
};

size_t l1distance(const uint8_t compressedData[], size_t compressedDataSize, const uint16_t* const otherVector) {
    L1DistanceWriter dw(otherVector);
    decompressVectorBase(compressedData, compressedDataSize, dw);
    return dw.dist;
}

struct L2DistanceSquaredWriter {
    uint64_t distSquared;
    const uint16_t* const otherVector;
    L2DistanceSquaredWriter(const uint16_t* const otherVector): distSquared(0), otherVector(otherVector) {}
    void element(size_t idx, size_t elt) {
        unsigned int d = abs((int)otherVector[idx] - (int)elt);
        distSquared += (uint64_t)(d * d);
    }
    void zeroBlock(size_t idx, size_t size) {
        for (size_t i = 0; i < size; ++i) {
            uint64_t d = otherVector[idx + i];
            distSquared += d * d;
        }
    }
    void end(size_t idx) {}
};

double l2distanceSquared(const uint8_t compressedData[], size_t compressedDataSize, const uint16_t* const otherVector) {
    L2DistanceSquaredWriter dw(otherVector);
    decompressVectorBase(compressedData, compressedDataSize, dw);
    return (double)dw.distSquared;
}

struct ElementwiseMaxWriter {
    uint16_t* const v;
    ElementwiseMaxWriter(uint16_t* const v): v(v) {}
    void element(size_t idx, size_t elt) {if (elt > v[idx]) v[idx] = elt;}
    void zeroBlock(size_t idx, size_t size) {}
    void end(size_t idx) {}
};

void elementwiseMax(const uint8_t compressedData[], size_t compressedDataSize, uint16_t* const v) {
    ElementwiseMaxWriter emw(v);
    decompressVectorBase(compressedData, compressedDataSize, emw);
}

struct L1HashWriter {
    size_t hashElementCount;
    const size_t* const indexes;
    const size_t* const compareValues;
    const size_t* const coeffs;
    size_t moduloValue;
    size_t hashValue;
    size_t currentIndex;
    L1HashWriter(size_t hashElementCount, const size_t indexes[], const size_t compareValues[], const size_t coeffs[],
                 size_t moduloValue)
        : hashElementCount(hashElementCount), indexes(indexes), compareValues(compareValues), coeffs(coeffs),
          moduloValue(moduloValue), hashValue(0), currentIndex(0) {}
    void element(size_t idx, size_t value) {
        while (currentIndex < hashElementCount && indexes[currentIndex] < idx) ++currentIndex;
        while (currentIndex < hashElementCount && indexes[currentIndex] == idx) {
            if (value > compareValues[currentIndex]) {
                hashValue += coeffs[currentIndex];
                if (hashValue >= moduloValue)
                    hashValue -= moduloValue;
            }
            ++currentIndex;
        }
    }
    void zeroBlock(size_t idx, size_t size) {
        while (currentIndex < hashElementCount && indexes[currentIndex] < idx + size)
            ++currentIndex;
    }
    void end(size_t idx) {}
};

size_t computeL1Hash(const uint8_t compressedData[], size_t compressedDataSize, size_t hashElementCount,
                     const size_t indexes[], const size_t compareValues[], const size_t coeffs[], size_t moduloValue) {
    L1HashWriter lhw(hashElementCount, indexes, compareValues, coeffs, moduloValue);
    decompressVectorBase(compressedData, compressedDataSize, lhw);
    return lhw.hashValue;
}

struct VectorDecompressor { // Slower, but allows decompression of two vectors to be interleaved
    size_t cachedElements[3];
    unsigned int cachedElementCount;
    unsigned int cachedElementIndex;
    size_t currentOutputIndex;
    const uint8_t * const compressedData;
    size_t compressedDataSize;
    size_t compressedDataIndex;

    VectorDecompressor(const uint8_t compressedData[], size_t compressedDataSize)
        : cachedElementCount(0), cachedElementIndex(0), currentOutputIndex(0), // One past last index output
          compressedData(compressedData), compressedDataSize(compressedDataSize), compressedDataIndex(0) {
        skipZeroElements();
    }

    bool atEnd() const {return cachedElementIndex == cachedElementCount && compressedDataIndex == compressedDataSize;}
    void skipZeroElements() {
        while (true) {
            if (compressedDataIndex >= compressedDataSize) return;
            uint8_t elt = compressedData[compressedDataIndex];
            if ((elt & 0xF0) == 0xC0) {
                ++compressedDataIndex;
                currentOutputIndex += elt - 0xC0 + 1;
            } else if ((elt & 0xF0) == 0xD0) {
                ++compressedDataIndex;
                currentOutputIndex += (elt - 0xD0 + 1) * 16;
            } else {
                return;
            }
        }
    }
    pair<size_t, size_t> getNextElement() { // Return index and value of next stored element (skipping zero elements)
    top:
        if (cachedElementIndex < cachedElementCount) {
            size_t nextElt = cachedElements[cachedElementIndex];
            ++cachedElementIndex;
            size_t nextEltIndex = currentOutputIndex;
            ++currentOutputIndex;
            if (cachedElementIndex == cachedElementCount) {
                skipZeroElements(); // Only need to do this when the last element of this set is being output
            }
            return make_pair(nextEltIndex, nextElt);
        } else if (compressedDataIndex >= compressedDataSize) {
            assert (!"End of data reached -- check for this using atEnd()");
            abort();
        } else { // cachedElementIndex == cachedElementCount and compressedDataIndex not at end
            uint8_t elt = compressedData[compressedDataIndex];
            ++compressedDataIndex;
            if ((elt & 0xE0) == 0xC0) {
                assert (!"Should not get here -- skipZeroElements() should have been used to skip these codes");
            } else if (elt < 0x40) {
                cachedElements[0] = elt;
                cachedElementIndex = 0;
                cachedElementCount = 1;
            } else if (elt < 0x80) {
                cachedElements[0] = (elt >> 3) & 7;
                cachedElements[1] = elt & 7;
                cachedElementIndex = 0;
                cachedElementCount = 2;
            } else if (elt < 0xC0) {
                cachedElements[0] = (elt >> 4) & 3;
                cachedElements[1] = (elt >> 2) & 3;
                cachedElements[2] = elt & 3;
                cachedElementIndex = 0;
                cachedElementCount = 3;
            } else {
                size_t acc = (elt - 0xE0);
                while (true) {
                    assert (compressedDataIndex < compressedDataSize);
                    uint8_t c = compressedData[compressedDataIndex];
                    ++compressedDataIndex;
                    acc = (acc << 7) | (c & 0x7F);
                    if (c >= 0x80) break;
                }
                cachedElements[0] = acc;
                cachedElementIndex = 0;
                cachedElementCount = 1;
            }
            goto top;
        }
    }
};


size_t l1distanceC(const uint8_t compressedData[], const size_t compressedDataSize, const uint8_t otherVectorCompressedData[],
                   const size_t otherVectorCompressedDataSize) {
#if 0
    cerr << "l1distanceC -------------------------------" << endl;
    cerr << "a: ";
    for (size_t i = 0; i < compressedDataSize; ++i) {
        cerr << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << (uint32_t)(compressedData[i]) << ' ';
    }
    cerr << "\n";
    cerr << "b: ";
    for (size_t i = 0; i < otherVectorCompressedDataSize; ++i) {
        cerr << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << (uint32_t)(otherVectorCompressedData[i]) << ' ';
    }
    cerr << "\n";
    cerr << std::dec;
#endif
    VectorDecompressor d1(compressedData, compressedDataSize);
    VectorDecompressor d2(otherVectorCompressedData, otherVectorCompressedDataSize);
    // Scan through union of indexes of nonzero elements of d1 and d2
    bool d1eltValid, d2eltValid;
    pair<size_t, size_t> d1elt, d2elt;
    size_t dist = 0;
    if (d1.atEnd()) {
        d1eltValid = false;
    } else {
        d1elt = d1.getNextElement();
        d1eltValid = true;
    }
    if (d2.atEnd()) {
        d2eltValid = false;
    } else {
        d2elt = d2.getNextElement();
        d2eltValid = true;
    }

    while (d1eltValid || d2eltValid) {
        if (!d1eltValid || (d2eltValid && d2elt.first < d1elt.first)) {
            dist += d2elt.second;
            if (d2.atEnd()) {
                d2eltValid = false;
            } else {
                d2elt = d2.getNextElement();
                d2eltValid = true;
            }
        } else if (!d2eltValid || (d1eltValid && d1elt.first < d2elt.first)) {
            dist += d1elt.second;
            if (d1.atEnd()) {
                d1eltValid = false;
            } else {
                d1elt = d1.getNextElement();
                d1eltValid = true;
            }
        } else { // Positions are equal
            size_t e = d1elt.second > d2elt.second ? d1elt.second - d2elt.second : d2elt.second - d1elt.second;
            dist += e;
            if (d1.atEnd()) {
                d1eltValid = false;
            } else {
                d1elt = d1.getNextElement();
                d1eltValid = true;
            }
            if (d2.atEnd()) {
                d2eltValid = false;
            } else {
                d2elt = d2.getNextElement();
                d2eltValid = true;
            }
        }
    }
    return dist;
}

double l2distanceSquaredC(const uint8_t compressedData[], const size_t compressedDataSize,
                          const uint8_t otherVectorCompressedData[], const size_t otherVectorCompressedDataSize) {
    VectorDecompressor d1(compressedData, compressedDataSize);
    VectorDecompressor d2(otherVectorCompressedData, otherVectorCompressedDataSize);
    // Scan through union of indexes of nonzero elements of d1 and d2
    bool d1eltValid, d2eltValid;
    pair<size_t, size_t> d1elt, d2elt;
    uint64_t dist = 0;
    if (d1.atEnd()) {
        d1eltValid = false;
    } else {
        d1elt = d1.getNextElement();
        d1eltValid = true;
    }
    if (d2.atEnd()) {
        d2eltValid = false;
    } else {
        d2elt = d2.getNextElement();
        d2eltValid = true;
    }

    while (d1eltValid || d2eltValid) {
        if (!d1eltValid || (d2eltValid && d2elt.first < d1elt.first)) {
            uint64_t e = d2elt.second;
            dist += e * e;
            if (d2.atEnd()) {
                d2eltValid = false;
            } else {
                d2elt = d2.getNextElement();
                d2eltValid = true;
            }
        } else if (!d2eltValid || (d1eltValid && d1elt.first < d2elt.first)) {
            uint64_t e = d1elt.second;
            dist += e * e;
            if (d1.atEnd()) {
                d1eltValid = false;
            } else {
                d1elt = d1.getNextElement();
                d1eltValid = true;
            }
        } else { // Positions are equal
            unsigned int e = abs((int)d1elt.second - (int)d2elt.second);
            dist += (uint64_t)(e * e);
            if (d1.atEnd()) {
                d1eltValid = false;
            } else {
                d1elt = d1.getNextElement();
                d1eltValid = true;
            }
            if (d2.atEnd()) {
                d2eltValid = false;
            } else {
                d2elt = d2.getNextElement();
                d2eltValid = true;
            }
        }
    }
    return (double)dist;
}

vector<uint8_t> compressVector(const uint16_t data[], const size_t dataSize) {
    vector<uint8_t> result;
    for (size_t i = 0; i < dataSize; /* Increment done within loop */ ) {
        uint16_t elt = data[i];
        if (elt == 0) {
            size_t zeroCount = 0;
            while (i < dataSize && data[i] == 0) {
                ++zeroCount;
                ++i;
            }
            while (zeroCount != 0) {
                if (zeroCount >= 16) {
                    size_t num16zeros = zeroCount / 16;
                    if (num16zeros > 16)
                        num16zeros = 16;
                    result.push_back(0xD0 - 1 + num16zeros);
                    zeroCount -= num16zeros * 16;
                } else {
                    result.push_back(0xC0 - 1 + zeroCount);
                    zeroCount = 0;
                }
            }
        } else if (elt <= 3 && i + 2 < dataSize && data[i + 1] <= 3 && data[i + 2] <= 3) {
            result.push_back(0x80 + (elt << 4) + (data[i + 1] << 2) + data[i + 2]);
            i += 3;
        } else if (elt <= 7 && i + 1 < dataSize && data[i + 1] <= 7) {
            result.push_back(0x40 + (elt << 3) + data[i + 1]);
            i += 2;
        } else if (elt < 64) {
            result.push_back(elt);
            i += 1;
        } else if (elt < 32 * 128) {
            result.push_back(0xE0 + (elt >> 7));
            result.push_back(0x80 + (elt & 0x7F));
            i += 1;
        } else /* if (elt < 32 * 128 * 128) -- always true for this data size */ {
            result.push_back(0xE0 + (elt >> 14));
            result.push_back(0x00 + ((elt >> 7) & 0x7F));
            result.push_back(0x80 + (elt & 0x7F));
            i += 1;
        }
    }
    return result;
}
