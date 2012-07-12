/*
 * utility.h
 *
 *  Created on: Jul 15, 2011
 *      Author: sharma9
 */

#ifndef UTILITY_H_
#define UTILITY_H_

class ArrayRef
{
	public:
		ArrayRef(string _name, string _type, string _dim)
		{
			name = _name;
			type = _type;
			dimensions = _dim;
		}
		string name;
		string type;
		string dimensions;
};


#endif /* UTILITY_H_ */
