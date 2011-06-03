/**
 * \file src/rose/Exception-rose.cpp
 * \brief Implementation of Rose's specific exceptions.
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#include "rose/Exception-rose.hpp"
#include "rose/rose-utils.hpp"

namespace Exception {

ExceptionRose::ExceptionRose(std::string str, const SgNode * node) :
	ExceptionFrontend(str),
	p_node(node)
{}

ExceptionRose::~ExceptionRose() {}

/**************/
/* Management */
/**************/

RoseAttributeMissing::RoseAttributeMissing(std::string str, const SgNode * node) :
	ExceptionRose(str, node)
{}

RoseAttributeMissing::~RoseAttributeMissing() {}
		
void RoseAttributeMissing::print(std::ostream & out) {
	out << "No " << p_str << " is attached to the node " << p_node << std::endl;
}

RoseUnknownNode::RoseUnknownNode(std::string str, const SgNode * node) :
	ExceptionRose(str, node)
{}

RoseUnknownNode::~RoseUnknownNode() {}
		
void RoseUnknownNode::print(std::ostream & out) {
	Sg_File_Info * info = p_node->get_file_info();
	out << info->get_filenameString() << ":" << info->get_line() << ": " << p_str << " does not accept " << p_node->class_name() << std::endl;
}

/********************/
/* Sage Translation */
/********************/

ExceptionSageTranslation::ExceptionSageTranslation(std::string str, const SgNode * node) :
	ExceptionRose(str, node)
{}

ExceptionSageTranslation::~ExceptionSageTranslation() {}

MisplacedVariableDeclaration::MisplacedVariableDeclaration(const SgNode * node) :
	ExceptionSageTranslation("", node)
{}

MisplacedVariableDeclaration::~MisplacedVariableDeclaration() {}
		
void MisplacedVariableDeclaration::print(std::ostream & out) {
	Sg_File_Info * info = p_node->get_file_info();
	out << info->get_filenameString() << ":" << info->get_line() << ": Variable declaration can only append at the begining of a function." << std::endl;
}

MisplacedNode::MisplacedNode(const SgNode * node) :
	ExceptionSageTranslation("", node)
{}

MisplacedNode::~MisplacedNode() {}
		
void MisplacedNode::print(std::ostream & out) {
	Sg_File_Info * info = p_node->get_file_info();
	out << info->get_filenameString() << ":" << info->get_line() << ": " << p_node->class_name() << " can not be inside a SCoP." << std::endl;
}

NoPragmaSCoP::NoPragmaSCoP() :
	ExceptionSageTranslation("", NULL)
{}

NoPragmaSCoP::~NoPragmaSCoP() {}
		
void NoPragmaSCoP::print(std::ostream & out) {
	out << "Need a pragma \"scop\" to know where  SCoP start." << std::endl;
}

NoPragmaEndSCoP::NoPragmaEndSCoP() :
	ExceptionSageTranslation("", NULL)
{}

NoPragmaEndSCoP::~NoPragmaEndSCoP() {}
		
void NoPragmaEndSCoP::print(std::ostream & out) {
	out << "Need a pragma \"endscop\" to know where  SCoP end." << std::endl;
}

/* For loop translation */

ExceptionForLoopTranslation::ExceptionForLoopTranslation(const SgNode * node, std::string str) :
	ExceptionSageTranslation(str, node)
{}

ExceptionForLoopTranslation::~ExceptionForLoopTranslation() {}
		
void ExceptionForLoopTranslation::print(std::ostream & out) {
	Sg_File_Info * info = p_node->get_file_info();
	out << info->get_filenameString() << ":" << info->get_line() << ": " << p_str << std::endl;
}

/* Linear expression translation */

ExceptionLinearExpressionTranslation::ExceptionLinearExpressionTranslation(const SgNode * node, std::string str) :
	ExceptionSageTranslation(str, node)
{}

ExceptionLinearExpressionTranslation::~ExceptionLinearExpressionTranslation() {}
		
void ExceptionLinearExpressionTranslation::print(std::ostream & out) {
	Sg_File_Info * info = p_node->get_file_info();
	out << info->get_filenameString() << ":" << info->get_line() << ": " << p_str << std::endl;
}

/* Conditionnal translation */

ExceptionConditionnalTranslation::ExceptionConditionnalTranslation(const SgNode * node, std::string str) :
	ExceptionSageTranslation(str, node)
{}

ExceptionConditionnalTranslation::~ExceptionConditionnalTranslation() {}
		
void ExceptionConditionnalTranslation::print(std::ostream & out) {
	Sg_File_Info * info = p_node->get_file_info();
	out << info->get_filenameString() << ":" << info->get_line() << ": " << p_str << std::endl;
}

/* Data Access retrieval */

ExceptionAccessRetrieval::ExceptionAccessRetrieval(const SgNode * node, std::string str) :
	ExceptionSageTranslation(str, node)
{}

ExceptionAccessRetrieval::~ExceptionAccessRetrieval() {}
		
void ExceptionAccessRetrieval::print(std::ostream & out) {
	Sg_File_Info * info = p_node->get_file_info();
	out << info->get_filenameString() << ":" << info->get_line() << ": " << p_str << std::endl;
}

}

