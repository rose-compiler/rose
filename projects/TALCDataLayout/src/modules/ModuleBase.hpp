/*
 * ModuleBase.hpp
 *
 *  Created on: Oct 17, 2012
 *      Author: Kamal Sharma
 */

#ifndef MODULEBASE_HPP_
#define MODULEBASE_HPP_

#include "rose.h"
#include "Common.hpp"
#include "PrintUtil.hpp"
#include "Meta.hpp"

class ModuleBase
{
public:
	virtual void visit(SgProject * project) = 0;

	void setMeta(Meta *meta) { this->meta = meta; };
	Meta* getMeta() { return meta; };
protected:
	Meta *meta;
};


#endif /* MODULEBASE_HPP_ */
