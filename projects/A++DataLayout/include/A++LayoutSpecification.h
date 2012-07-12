/*
 * A++LayoutTransformations.h
 *
 *  Created on: Jul 11, 2011
 *      Author: sharma9
 */

#ifndef A__SPECIFICATION_H_
#define A__SPECIFICATION_H_

class doubleArray
   {
     public:
			doubleArray();
   };

class intArray
   {
     public:
			intArray();
   };

doubleArray & interleaveAcrossArrays ( doubleArray A, ...);
intArray & interleaveAcrossArrays ( intArray A, ...);


#endif /* A__LAYOUTTRANSFORMATIONS_H_ */
