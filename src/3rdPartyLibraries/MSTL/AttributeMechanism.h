// Author: Markus Schordan
// $Id: AttributeMechanism.h,v 1.4 2006/04/24 00:21:27 dquinlan Exp $

// This class template was removed because nothing in the ROSE library, ROSE tests, or associated ROSE projects uses it. If
// you're trying to store attributes in a Sage IR node ("Sg*" class) then use the SgNode interface (SgNode::addAttribute, etc.)
// or the AstAttributeMechanism interface on which the SgNode attribute interface is built. If you're a class author trying to
// allow user-defined attributes to be stored directly (not via pointers to a base class) and without requiring the user to
// recompile ROSE, then use Sawyer::Attribute instead (the old AttributeMechanism couldn't be used for this anyway). [Robb
// Matzke 2015-11-16]

#ifndef ATTRIBUTEMECHANISM_H
#define ATTRIBUTEMECHANISM_H

// [Robb Matzke 2015-11-16]: deprecated. Do not document.
template<class Key, class Value>
class AttributeMechanism ROSE_DEPRECATED("Use AstAttributeMechanism instead") {};

#endif
