/*
 * faultToleranceArrayLib.C
 *
 *  Created on: May 24, 2011
 *      Author: sharma9
 */

#include <string>
#include <map>
#include <iostream>
#include <vector>
#include <sstream>

#include "faultToleranceArrayLibUtility.C"

using namespace std;

//3D
void updateArray(string arrayName, int *arrayPtr, int LB1, int UB1, int LB2,
		int UB2, int LB3, int UB3, int chunk) {
	for (int i = LB1; i < UB1; i++) {
		for (int j = LB2; j < UB2; j++) {
			stringstream out;
			out << "_" << i << "_" << j;
			string name = arrayName + out.str();

			processUpdateArray(name, arrayPtr, i, j, UB1, UB2, LB3, UB3, chunk);
		}
	}
}

// 2D
void updateArray(string arrayName, int *arrayPtr, int LB1, int UB1, int LB2,
		int UB2, int chunk) {
	for (int i = LB1; i < UB1; i++) {
		stringstream out;
		out << "_" << i;
		string name = arrayName + out.str();

		processUpdateArray(name, arrayPtr, 0, i, 0, UB1, LB2, UB2, chunk);
	}
}

/*
 * Updates a 1D Array
 */
void updateArray(string arrayName, int *arrayPtr, int LB, int UB, int chunk) {
	processUpdateArray(arrayName, arrayPtr, 0, 0, 0, 0, LB, UB, chunk);
}

/*
 * Update Element of 3D Array
 */
void updateElem(string arrayName, int newValue, int index1, int index2,
		int index3, int oldValue, int chunk) {
	stringstream out;
	out << "_" << index1 << "_" << index2;
	arrayName.append(out.str());

	processUpdateElem(arrayName, index3, newValue, oldValue, chunk);
}

/*
 * Update Element of 2D Array
 */
void updateElem(string arrayName, int newValue, int index1, int index2,
		int oldValue, int chunk) {
	stringstream out;
	out << "_" << index1;
	arrayName.append(out.str());

	processUpdateElem(arrayName, index2, newValue, oldValue, chunk);
}

/*
 * Updates a single element of an array
 * by calculating the appropriate chunk location
 */
void updateElem(string arrayName, int newValue, int index, int oldValue,
		int chunk) {
	processUpdateElem(arrayName, index, newValue, oldValue, chunk);
}

//1D
void validateArray(string arrayName, int *arrayPtr, int LB, int UB, int chunk) {
	processValidateArray(arrayName, arrayPtr, 0, 0, 0, 0, LB, UB, chunk);
}

//2D
void validateArray(string arrayName, int *arrayPtr, int LB1, int UB1, int LB2,
		int UB2, int chunk) {
	for (int i = LB1; i < UB1; i++) {
		stringstream out;
		out << "_" << i;
		string name = arrayName + out.str();

		processValidateArray(name, arrayPtr, 0, i, 0, UB1, LB2, UB2, chunk);
	}
}

//3D
void validateArray(string arrayName, int *arrayPtr, int LB1, int UB1, int LB2,
		int UB2, int LB3, int UB3, int chunk) {
	for (int i = LB1; i < UB1; i++) {
		for (int j = LB2; j < UB2; j++) {
			stringstream out;
			out << "_" << i << "_" << j;
			string name = arrayName + out.str();

			processValidateArray(name, arrayPtr, i, j, UB1, UB2, LB3, UB3,
					chunk);
		}
	}
}

void clearHashTable()
{
	hashTable.clear();
}

//int main()
//{
//	int A[100], B[100][100];
//
//	for(int i=0; i<100; i++)
//		A[i] = i+1;
//
//	updateArray("A", &A[0], 0, 100, 8);
//	updateArray("B", &B[0][0], 0, 100, 0, 100, 8);
//	for(int i=0; i<100; i++)
//	{
//		//validateElem("A", A, i, 0, 100, 8);
//		int t1 = B[i][5];
//		B[i][5] = A[i];
//		updateElem("B", B[i][5], i, 5, t1, 8);
//		B[2][5] = 23;
//	}
//	validateArray("A", A, 0, 100, 8);
//	validateArray("B", &B[0][0], 0, 100, 0, 100, 8);
//}
