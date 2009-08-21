/**
 * \file ERR.C
 *
 * \note As written, these tests catch template declarations only if
 * instantiated.
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

#include "rose.h"
#include "utilities.h"

/**
 * Understand the termination behavior of assert() and abort()
 */
bool ERR06_C( const SgNode *node ) {
	const SgFunctionRefExp *fnRef = isSgFunctionRefExp(node);
	if (!(fnRef && isCallOfFunctionNamed(fnRef, "abort")))
		return false;

	std::set<const SgInitializedName *> fds;

	FOREACH_SUBNODE(findParentOfType(fnRef, SgFunctionDefinition), nodes, i, V_SgFunctionRefExp) {
		const SgFunctionRefExp *iFn = isSgFunctionRefExp(*i);
		assert(iFn);
		if (iFn == fnRef)
			break;

		if (isCallOfFunctionNamed(iFn, "open")
		  ||isCallOfFunctionNamed(iFn, "fopen")
		  ||isCallOfFunctionNamed(iFn, "mkstemp")) {
			const SgInitializedName *var = getVarAssignedTo(iFn, NULL);
			if (var && !(isGlobalVar(var) || isStaticVar(var))) {
				fds.insert(var);
			}
		} else if (isCallOfFunctionNamed(iFn, "close")
		  ||isCallOfFunctionNamed(iFn, "open")) {
			const SgVarRefExp *varRef = isSgVarRefExp(removeImplicitPromotions(getFnArg(iFn, 0)));
			if (!varRef)
				continue;
			const SgInitializedName *var = getRefDecl(varRef);
			assert(var);
			fds.erase(fds.find(var));
		}
	}

	if (fds.size() > 0) {
		print_error(node, "ERR06-C", "Understand the termination behavior of assert() and abort()", true);
		return true;
	}

	return false;
}

/**
 * Don't redefine errno
 *
 * \note If the program includes errno.h than errno is a macro, so we won't be
 * able to flag this declaration
 */
bool ERR31_C( const SgNode *node ) {
	const SgVariableDeclaration *varDecl = isSgVariableDeclaration(node);
	if (!varDecl)
		return false;
	bool violation = false;
	FOREACH_INITNAME(varDecl->get_variables(), i) {
		const SgInitializedName *var = isSgInitializedName(*i);
		assert(var);
		if (var->get_name().getString() == "errno") {
			print_error(node, "ERR31-C", "Don't redefine errno");
			violation = true;
		}
	}
	return violation;
}

bool ERR(const SgNode *node) {
  bool violation = false;
  violation |= ERR06_C(node);
  violation |= ERR31_C(node);
  return violation;
}
