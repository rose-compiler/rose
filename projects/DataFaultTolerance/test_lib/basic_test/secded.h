/*
 * secded.h
 *
 *  Created on: May 25, 2011
 *      Author: sharma9
 */

#include <string>

/*#ifdef  __cplusplus
extern "C" {
#endif
*/
extern bool calculateParity(int data, unsigned int checkbits);
extern unsigned short int calculateIntCheckBits(int value);
extern void applySECDED(void *memLoc, std::string type, unsigned short int retrievedCheckBits, bool retrievedParity ,unsigned short int calCheckBits, bool calParity);

/*#ifdef  __cplusplus
}
#endif*/
