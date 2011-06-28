/**
 * \file include/rose/Exception-rose.hpp
 * \brief Rose exceptions
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#ifndef _EXCEPTION_ROSE_HPP_
#define _EXCEPTION_ROSE_HPP_

#include "rose/rose-utils.hpp"
#include "common/Exception.hpp"

#include <string>

namespace Exception {

class ExceptionRose : public ExceptionFrontend {
	protected:
		const SgNode * p_node;
	
	public:
		ExceptionRose(std::string str, const SgNode * node);
		virtual ~ExceptionRose();
		
		virtual void print(std::ostream & out) = 0;
};

/**************/
/* Management */
/**************/

class RoseAttributeMissing : public ExceptionRose {
	public:
		RoseAttributeMissing(std::string str, const SgNode * node);
		virtual ~RoseAttributeMissing();
		
		virtual void print(std::ostream & out);
};

class RoseUnknownNode : public ExceptionRose {
	public:
		RoseUnknownNode(std::string str, const SgNode * node);
		virtual ~RoseUnknownNode();
		
		virtual void print(std::ostream & out);
};

/********************/
/* Sage Translation */
/********************/

class ExceptionSageTranslation : public ExceptionRose {
	public:
		ExceptionSageTranslation(std::string str, const SgNode * node);
		virtual ~ExceptionSageTranslation();
		
		virtual void print(std::ostream & out) = 0;
};

class MisplacedVariableDeclaration : public ExceptionSageTranslation {
	public:
		MisplacedVariableDeclaration(const SgNode * node);
		virtual ~MisplacedVariableDeclaration();
		
		virtual void print(std::ostream & out);
};

class MisplacedNode : public ExceptionSageTranslation {
	public:
		MisplacedNode(const SgNode * node);
		virtual ~MisplacedNode();
		
		virtual void print(std::ostream & out);
};

class NoPragmaSCoP : public ExceptionSageTranslation {
	public:
		NoPragmaSCoP();
		virtual ~NoPragmaSCoP();
		
		virtual void print(std::ostream & out);
};

class NoPragmaEndSCoP : public ExceptionSageTranslation {
	public:
		NoPragmaEndSCoP();
		virtual ~NoPragmaEndSCoP();
		
		virtual void print(std::ostream & out);
};

/* For loop translation */

class ExceptionForLoopTranslation : public ExceptionSageTranslation {
	public:
		ExceptionForLoopTranslation(const SgNode * node, std::string str);
		virtual ~ExceptionForLoopTranslation();
		
		virtual void print(std::ostream & out);
};

/* Linear expression translation */

class ExceptionLinearExpressionTranslation : public ExceptionSageTranslation {
	public:
		ExceptionLinearExpressionTranslation(const SgNode * node, std::string str);
		virtual ~ExceptionLinearExpressionTranslation();
		
		virtual void print(std::ostream & out);
};

/* Conditionnal translation */

class ExceptionConditionnalTranslation : public ExceptionSageTranslation {
	public:
		ExceptionConditionnalTranslation(const SgNode * node, std::string str);
		virtual ~ExceptionConditionnalTranslation();
		
		virtual void print(std::ostream & out);
};

/* Data Access retrieval */

class ExceptionAccessRetrieval : public ExceptionSageTranslation {
	public:
		ExceptionAccessRetrieval(const SgNode * node, std::string str);
		virtual ~ExceptionAccessRetrieval();
		
		virtual void print(std::ostream & out);
};

}

#endif /* _EXCEPTION_ROSE_HPP_ */

