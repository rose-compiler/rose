/**
 * \file value.C
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
#include "value.h"

/**
 * Takes a Value node and tries to extract the numeric value from it.  The
 * need for this exists because size_t are expressed as both UnsignedIntVal's
 * and UnsignedLongVal's.
 */
bool getSizetVal(const SgExpression *node, size_t *value) {
	if(!node)
		return false;
	if (isSgUnsignedIntVal(node)) {
		*value = isSgUnsignedIntVal(node)->get_value();
	} else if (isSgUnsignedLongVal(node)) {
		*value = isSgUnsignedLongVal(node)->get_value();
	} else {
		return false;
	}
	return true;
}

bool getIntegerVal(const SgExpression *node, intmax_t *n) {
	if(!node)
		return false;
	if (isSgUnsignedIntVal(node)) {
		*n = isSgUnsignedIntVal(node)->get_value();
	} else if (isSgIntVal(node)) {
		*n = isSgIntVal(node)->get_value();
	} else if (isSgUnsignedLongVal(node)) {
		*n = (intmax_t) (isSgUnsignedLongVal(node)->get_value());
	} else if (isSgLongIntVal(node)) {
		*n = isSgLongIntVal(node)->get_value();
	} else if (isSgUnsignedLongLongIntVal(node)) {
		*n = (intmax_t) isSgUnsignedLongLongIntVal(node)->get_value();
	} else if (isSgLongLongIntVal(node)) {
		*n = isSgLongLongIntVal(node)->get_value();
	} else if (isSgUnsignedShortVal(node)) {
		*n = isSgUnsignedShortVal(node)->get_value();
	} else if (isSgShortVal(node)) {
		*n = isSgShortVal(node)->get_value();
	} else {
		return false;
	}
	return true;
}

bool getFloatingVal(const SgExpression *node, long double *n) {
	if(!node)
		return false;
	if (isSgFloatVal(node)) {
		*n = isSgFloatVal(node)->get_value();
	} else if (isSgDoubleVal(node)) {
		*n = isSgDoubleVal(node)->get_value();
	} else if (isSgLongDoubleVal(node)) {
		*n = isSgLongDoubleVal(node)->get_value();
	} else {
		return false;
	}
	return true;
}

bool isVal(const SgExpression *node, const intmax_t n) {
	if (!node)
		return false;
	intmax_t x;
	if (!getIntegerVal(node, &x))
		return false;
	return x == n;
}

/**
 * Takes a Value node and tries to make sure it is 0
 */
bool isZeroVal(const SgExpression *node) {
	if (!node)
		return false;
	if (node->get_type()->isIntegerType()) {
		return isVal(node,0);
	} else if (node->get_type()->isFloatType()) {
		long double x;
		if (!getFloatingVal(node, &x))
			return false;
		return x == 0.0l;
	} else {
		return false;
	}
}

/**
 * Takes a Value node and tries to make sure it is the minimum
 */
bool isMinVal(const SgExpression *node) {
	if(!node)
		return false;
	if (isSgUnsignedIntVal(node)) {
		return 0 == isSgUnsignedIntVal(node)->get_value();
	} else if (isSgIntVal(node)) {
		return INT_MIN == isSgIntVal(node)->get_value();
	} else if (isSgUnsignedLongVal(node)) {
		return 0 == isSgUnsignedLongVal(node)->get_value();
	} else if (isSgLongIntVal(node)) {
		return LONG_MIN == isSgLongIntVal(node)->get_value();
	} else if (isSgUnsignedLongLongIntVal(node)) {
		return 0 == isSgUnsignedLongLongIntVal(node)->get_value();
	} else if (isSgLongLongIntVal(node)) {
		return std::numeric_limits<long long>::min() == isSgLongLongIntVal(node)->get_value();
	} else if (isSgUnsignedShortVal(node)) {
		return 0 == isSgUnsignedShortVal(node)->get_value();
	} else if (isSgShortVal(node)) {
		return SHRT_MIN == isSgShortVal(node)->get_value();
	} else if (isSgUnsignedCharVal(node)) {
		return 0 == isSgUnsignedCharVal(node)->get_value();
	} else if (isSgCharVal(node)) {
		return CHAR_MIN == isSgCharVal(node)->get_value();
	} else {
		return false;
	}
}

