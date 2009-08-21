/**
 * \file utilities-inline.h
 *
 * Copyright (c) 2007 Carnegie Mellon University.
 * All rights reserved.

 * Permission to use this software and its documentation for any purpose is
 * hereby granted, provided that the above copyright notice appear and that
 * both that copyright notice and this permission notice appear in supporting
 * documentation, and that the name of CMU not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.
 *
 * CMU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WSTRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL CMU BE
 * LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, RISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


#ifndef UTILITIES_INLINE_H
#define UTILITIES_INLINE_H

#define findParentOfType(node, t) \
	is ## t ( findParentNodeOfType(node, V_ ##t ))

/**
 * Iterate over all initializers "i" in a list of variables
 * nodes should be something like SgVariableDeclaration->get_variables()
 */
#define FOREACH_INITNAME(nodes,i) \
	for(SgInitializedNamePtrList::const_iterator (i) = (nodes).begin(); (i) < (nodes).end(); (i)++)

/** Iterate over all nodes of type type below node, name the collection nodes
 * and name the iteroator i
 * 
 * \param[in] node Root node of the tree
 * \param[out] nodes What to call the collection
 * \param[out] i What to call the iterator
 * \param[in] type Only iterate over nodes of this type
 */
#define FOREACH_SUBNODE(node, nodes, i, type) \
	assert(node);\
	Rose_STL_Container<SgNode *> (nodes) = NodeQuery::querySubTree( const_cast<SgNode*>((const SgNode *) (node)), (type) ); \
	Rose_STL_Container<SgNode *>::iterator (i) = (nodes).begin(); \
	for (; (i) != (nodes).end(); ++(i) )

/**
 * Remove all modifiers such as const or volatile, but leave the typedefs
 */
inline const SgType *stripModifiers(const SgType *type) {
	assert(type);
	return type->stripType(SgType::STRIP_MODIFIER_TYPE);
}

/**
 * Remove all typedefs, but leave the modifiers
 */
inline const SgType *stripTypedefs(const SgType *type) {
	assert(type);
	return type->stripType(SgType::STRIP_TYPEDEF_TYPE);
}

/**
 * Checks to see if the type has the const modifier
 */
inline bool isConstType(const SgType *t) {
	assert(t);
	const SgModifierType *mt = isSgModifierType(stripTypedefs(t));
	return mt && mt->get_typeModifier().get_constVolatileModifier().isConst();
}
/**
 * Checks to see if the type has the volatile modifier
 */
inline bool isVolatileType(const SgType *t) {
	assert(t);
	const SgModifierType *mt = isSgModifierType(stripTypedefs(t));
	return mt && mt->get_typeModifier().get_constVolatileModifier().isVolatile();
}

/**
 * Returns a variable's declaration, given a reference to that var
 */
inline const SgInitializedName* getRefDecl(const SgVarRefExp* ref) {
	assert(ref);
	return ref->get_symbol()->get_declaration();
}

/**
 * Checks to see if the variable was declared static
 */
inline bool isStaticVar(const SgInitializedName *var) {
	assert(var);
	return const_cast<SgInitializedName*>(var)->get_declaration()->get_declarationModifier().get_storageModifier().isStatic();
}

/**
 * Checks to see if the variable was declared extern
 */
inline bool isExternVar(const SgInitializedName * var) {
	return const_cast<SgInitializedName*>(var)->get_declaration()->get_declarationModifier().get_storageModifier().isExtern();
}

/**
 * Checks the scope of the variable to see if it is global or not
 */
inline bool isGlobalVar(const SgInitializedName *var) {
	assert(var);
	return isSgGlobal(var->get_scope());
}

/**
 * Until ROSE implements these types natively, we will have to beat the
 * names out of the types with unparseToString()
 */
inline bool isTypeWcharT(const SgType *t) {
	assert(t);
	return stripModifiers(t)->unparseToString() == "wchar_t";
}

inline bool isTypeTimeT(const SgType *t) {
	assert(t);
	return stripModifiers(t)->unparseToString() == "time_t";
}
inline bool isTypeSizeT(const SgType *t) {
	assert(t);
	return stripModifiers(t)->unparseToString() == "size_t";
}

inline bool isTypeRSizeT(const SgType *t) {
	assert(t);
	return stripModifiers(t)->unparseToString() == "rsize_t";
}

inline bool isTypeUintptrT(const SgType *t) {
	assert(t);
	return stripModifiers(t)->unparseToString() == "uintptr_t";
}

inline bool isTypeVoid(const SgType *t) {
	assert(t);
	return stripModifiers(t)->unparseToString() == "void";
}

inline bool isTypeVoidStar(const SgType *t) {
	assert(t);
	const SgType *d = const_cast<SgType *>(t)->dereference();
	return (t !=d ) && isTypeVoid(d);
}

inline bool isTypeFile(const SgType *t) {
	assert(t);
	std::string str = stripModifiers(t)->unparseToString();
	return str == "FILE" || str == "struct _IO_FILE";
}

inline bool isCompilerGeneratedNode(const SgNode *node) {
	assert(node);
	return node->get_file_info()->isCompilerGenerated();
}

#endif
