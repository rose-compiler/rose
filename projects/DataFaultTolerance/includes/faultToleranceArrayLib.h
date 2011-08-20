/*
 * faultToleranceLib.h
 *
 *  Created on: May 24, 2011
 *      Author: sharma9
 */


#include <string>

// Update Entire Array
void updateArray(std::string arrayName, int *arrayPtr, int LB, int UB, int chunk);
void updateArray(std::string arrayName, int *arrayPtr, int LB1, int UB1, int LB2, int UB2, int chunk);
void updateArray(std::string arrayName, int *arrayPtr, int LB1, int UB1, int LB2, int UB2, int LB3, int UB3, int chunk);

// Update an element in Array
void updateElem(std::string arrayName, int newValue, int index, int oldValue, int chunk);
void updateElem(std::string arrayName, int newValue, int index1, int index2, int oldValue, int chunk);
void updateElem(std::string arrayName, int newValue, int index1, int index2, int index3, int oldValue, int chunk);

// Validate Entire Array
void validateArray(std::string arrayName, int *arrayPtr, int LB, int UB, int chunk);
void validateArray(std::string arrayName, int *arrayPtr, int LB1, int UB1, int LB2, int UB2, int chunk);
void validateArray(std::string arrayName, int *arrayPtr, int LB1, int UB1, int LB2, int UB2, int LB3, int UB3, int chunk);

// Hash Table methods
void clearHashTable();
