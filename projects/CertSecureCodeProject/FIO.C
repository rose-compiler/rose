/**
 * \file FIO.C
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

static bool isReadFn(const SgFunctionRefExp *fnRef, unsigned int * argNum) {
	if(isCallOfFunctionNamed(fnRef, "fread")) {
		*argNum = 3;
		return true;
	}
	if(isCallOfFunctionNamed(fnRef, "read")) {
		*argNum = 0;
		return true;
	}
	return false;
}

static bool isWriteFn(const SgFunctionRefExp *fnRef, unsigned int * argNum) {
	if(isCallOfFunctionNamed(fnRef, "fwrite")) {
		*argNum = 3;
		return true;
	}
	if(isCallOfFunctionNamed(fnRef, "write")) {
		*argNum = 0;
		return true;
	}
	return false;
}

/**
 * Be careful using functions that use file names for identification
 */
bool FIO01_C( const SgNode *node ) {
	const SgFunctionRefExp *fnRef = isSgFunctionRefExp(node);
	if (!fnRef)
		return false;
	if (!(isCallOfFunctionNamed(fnRef, "chown")
	|| isCallOfFunctionNamed(fnRef, "stat")
	|| isCallOfFunctionNamed(fnRef, "chmod"))) {
		return false;
	}

	const SgVarRefExp* file_name = isSgVarRefExp(getFnArg(fnRef,0));
	if (!file_name)
		return false;


	/**
	 * See if the file was opened inside this function, if so, we have a
	 * problem
	 */
	FOREACH_SUBNODE(findParentOfType(fnRef, SgFunctionDefinition), nodes, i, V_SgFunctionRefExp) {
		const SgFunctionRefExp *iFn = isSgFunctionRefExp(*i);
		assert(iFn);

		if (isCallOfFunctionNamed(iFn, "open")
		||  isCallOfFunctionNamed(iFn, "fopen")) {
			const SgVarRefExp *iVar = isSgVarRefExp(getFnArg(iFn,0));
			if (iVar && (getRefDecl(iVar) == getRefDecl(file_name))) {
				print_error(node, "FIO01-C", "Be careful using functions that use file names for identification", true);
				return true;
			}
		}
	}

	/* If the file was not opened, it's probably ok to use these functions */
	return false;
}

/**
 * Prefer fseek() to rewind()
 */
bool FIO07_C( const SgNode *node ) {
	const SgFunctionRefExp *fnRef = isSgFunctionRefExp(node);
	if (!(fnRef && isCallOfFunctionNamed(fnRef, "rewind")))
		return false;
	print_error( node, "FIO07-C", "Prefer fseek() to rewind()", true);
	return true;
}

/**
 * Take care when calling remove() on an open file
 */
bool FIO08_C( const SgNode *node ) {
	const SgFunctionRefExp *fnRef = isSgFunctionRefExp(node);
	if(!(fnRef && isCallOfFunctionNamed(fnRef, "remove")))
		return false;

	const SgExpression *fnExp = removeImplicitPromotions(getFnArg(fnRef,0));
	assert(fnExp);

	const SgVarRefExp *ref = isSgVarRefExp(fnExp);
	if(!ref)
		return false;
	const SgInitializedName *var = getRefDecl(ref);

	bool opened = false;
	bool closed = false;
	/* *open or *close */
	const SgFunctionRefExp *iFn = NULL;
	/* Argument to *open or *close */
	const SgVarRefExp *iVar = NULL;
	/* File descriptor return by *open */
	const SgInitializedName *fd = NULL;

	FOREACH_SUBNODE(findParentOfType(node, SgFunctionDefinition), nodes, i, V_SgFunctionRefExp) {
		iFn = isSgFunctionRefExp(*i);
		assert(iFn);

		if(iFn == fnRef)
			break;

		iVar = isSgVarRefExp(removeImplicitPromotions(getFnArg(iFn,0)));
		if (!iVar)
			continue;

		if(isCallOfFunctionNamed(iFn, "open")
		|| isCallOfFunctionNamed(iFn, "fopen")
		|| isCallOfFunctionNamed(iFn, "freopen")) {
			fd = getVarAssignedTo(iFn, NULL);
			assert(fd);

			if (getRefDecl(iVar) == var) {
				closed = false;
				opened = true;
			}
		}

		if (opened
		&& (isCallOfFunctionNamed(iFn, "close"))
		|| isCallOfFunctionNamed(iFn, "fclose")) {
			if (getRefDecl(iVar) == fd) {
				closed = true;
				opened = false;
			}
		}
	}


	if (opened && !closed) {
		print_error(node,"FIO08-C", "Take care when calling remove() on an open file", true);
		return true;
	}

	return false;
}

/**
 * Take care when specifying the mode parameter of fopen()
 */
bool FIO11_C( const SgNode *node ) {
	const SgFunctionRefExp *fnRef = isSgFunctionRefExp(node);
	if (!(fnRef && isCallOfFunctionNamed(fnRef, "fopen")))
		return false;

	const SgStringVal* mode = isSgStringVal(getFnArg(fnRef, 1));
	if(!mode)
		return false;

	const std::string str = mode->get_value();

	if((str == "r")
	|| (str == "w")
	|| (str == "a")
	|| (str == "rb")
	|| (str == "wb")
	|| (str == "ab")
	|| (str == "r+")
	|| (str == "w+")
	|| (str == "a+")
	|| (str == "r+b")
	|| (str == "rb+")
	|| (str == "w+b")
	|| (str == "wb+")
	|| (str == "a+b")
	|| (str == "ab+"))
		return false;

	print_error(node, "FIO11-C", "Take care when specifying the mode parameter of fopen()", true);
	return true;
}

/**
 * Prefer setvbuf() to setbuf()
 */
bool FIO12_C( const SgNode *node ) {
	const SgFunctionRefExp *fnRef = isSgFunctionRefExp(node);
	if (!(fnRef && isCallOfFunctionNamed(fnRef, "setbuf")))
		return false;

	print_error( node, "FIO12-C", "Prefer setvbuf() to setbuf()", true);
	return true;
}

/**
 * Never push back anything other than one read character
 */
bool FIO13_C( const SgNode *node ) {
	const SgFunctionRefExp *fnRef = isSgFunctionRefExp(node);
	if (!fnRef)
		return false;

	if(!(isCallOfFunctionNamed(fnRef, "ungetc"))
	  ||(isCallOfFunctionNamed(fnRef, "ungetwc")))
		return false;

	std::string fn_str;

	const SgVarRefExp* fd = isSgVarRefExp(getFnArg(fnRef, 1));
	assert(fd);

	bool before = true;

	FOREACH_SUBNODE(findParentOfType(fnRef, SgBasicBlock), nodes, i, V_SgFunctionRefExp) {
		const SgFunctionRefExp *iFn = isSgFunctionRefExp(*i);
		assert(iFn);

		/* Ignore nodes before fnRef */
		if (before) {
			if (iFn == fnRef)
				before = false;
			continue;
		}

		/* If we flushed the stream, we're done */
		if (isCallOfFunctionNamed(iFn, "fflush")
		||  isCallOfFunctionNamed(iFn, "fseek")
		||  isCallOfFunctionNamed(iFn, "fsetpos")
		||  isCallOfFunctionNamed(iFn, "rewind")) {
			const SgVarRefExp *iVar = isSgVarRefExp(getFnArg(iFn, 0));
			if (iVar && (getRefDecl(iVar) == getRefDecl(fd))) {
				return false;
			}
			continue;
		}

		/* If we read from the stream, we're done */
		unsigned int argNum;
		if (isReadFn(iFn, &argNum)) {
			const SgVarRefExp *iVar = isSgVarRefExp(getFnArg(iFn, argNum));
			if (iVar && (getRefDecl(iVar) == getRefDecl(fd))) {
				return false;
			}
			continue;
		}

		/* Buf if it's another ungetc, that's no good */
		if (isCallOfFunctionNamed(iFn, "ungetc")
		||  isCallOfFunctionNamed(iFn, "ungetwc")) {
			const SgVarRefExp *iVar = isSgVarRefExp(getFnArg(iFn, 1));
			if (iVar && (getRefDecl(iVar) == getRefDecl(fd))) {
				print_error(node, "FIO13-C", "Never push back anything other than one read character", true);
				return true;
			}
		}
	}

	return false;
}

/**
 * Exclude user input from format strings
 *
 * We make sure that the format argument to *printf family of functions is
 * either const or a string
 */
bool FIO30_C( const SgNode *node) {
	const SgFunctionRefExp *fnRef = isSgFunctionRefExp(node);
	if(!fnRef)
		return false;

	int argNum;
	if(((argNum = getScanfFormatString(fnRef)) == -1) &&
		 ((argNum = getPrintfFormatString(fnRef)) == -1)) {
		return false;
	}

	const SgExpression *frmt = removeImplicitPromotions(getFnArg(fnRef,argNum));
	assert(frmt);
	const SgType *frmtType = frmt->get_type();
	assert(frmtType);

	/**
	 * for some reason we can't find the const version of dereference
	 */
	bool isConst = isConstType((const_cast<SgType *>(frmtType))->dereference());
	if(!(isConst || isSgTypeString(frmtType))) {
		print_error(node, "FIO30-C", "Exclude user input from format strings");
		return true;
	}

	return false;
}

/**
 * Use int to capture the return value of character IO functions
 */
bool FIO34_C( const SgNode *node) {
	const SgFunctionRefExp *fnRef = isSgFunctionRefExp(node);
	if (!fnRef)
		return false;
	if (!(isCallOfFunctionNamed(fnRef, "fputc")
		||isCallOfFunctionNamed(fnRef, "putc")
		||isCallOfFunctionNamed(fnRef, "putchar")
		||isCallOfFunctionNamed(fnRef, "ungetc")
		||isCallOfFunctionNamed(fnRef, "fgetc")
		||isCallOfFunctionNamed(fnRef, "getc")
		||isCallOfFunctionNamed(fnRef, "getchar")))
		return false;

	assert(node->get_parent());
	assert(node->get_parent()->get_parent());

	SgNode const *parent = node->get_parent()->get_parent();

	SgCastExp const *cast;
	if(!(cast = isSgCastExp(parent)))
		return false;

	const SgType *t = cast->get_type();
	if (isSgTypeInt(t)
	  ||isSgTypeSignedInt(t)
	  ||isSgTypeUnsignedInt(t)
	  ||isSgTypeLong(t)
	  ||isSgTypeSignedLong(t)
	  ||isSgTypeUnsignedLong(t)
	  ||isSgTypeLongLong(t)
	  ||isSgTypeUnsignedLongLong(t))
		return false;

	print_error( node, "FIO34-C", "Use int to capture the return value of character I/O functions");
	return true;
}

/**
 * Do not use a copy of a FILE object for input and output
 */
bool FIO38_C( const SgNode *node) {
	const SgExpression *rhs = NULL;
	if (isAnyAssignOp(node)) {
		rhs = isSgBinaryOp(node)->get_rhs_operand();
	} else if (isSgAssignInitializer(node)) {
		rhs = isSgAssignInitializer(node)->get_operand();
	}
	if (!rhs)
		return false;

	if (isTypeFile(rhs->get_type())) {
		print_error(node, "FIO38-C", "Do not use a copy of a FILE object for input and output");
		return true;
	}

	return false;
}

/**
 * Do not alternately input and output from a stream without an intervening
 * flush or positioning call
 */
bool FIO39_C( const SgNode *node) {
	const SgFunctionRefExp *fnRef = isSgFunctionRefExp(node);
	if (!fnRef)
		return false;

	bool read1 = false;
	bool write1 = false;
	unsigned int argNum;
	read1 = isReadFn(fnRef, &argNum);
	write1 = isWriteFn(fnRef, &argNum);
	if (!(read1 || write1))
		return false;

	const SgVarRefExp* fd = isSgVarRefExp(getFnArg(fnRef, argNum));
	/**
	 * Disabling assertion due to C++ code
	 */
	if (!fd)
		return false;
//	assert(fd);

	bool before = true;

	FOREACH_SUBNODE(findParentOfType(fnRef, SgBasicBlock), nodes, i, V_SgFunctionRefExp) {
		const SgFunctionRefExp *iFn = isSgFunctionRefExp(*i);
		assert(iFn);

		/* Ignore nodes before fnRef */
		if (before) {
			if (iFn == fnRef)
				before = false;
			continue;
		}

		/* If we flushed the stream, we're done */
		if (isCallOfFunctionNamed(iFn, "fflush")
		||  isCallOfFunctionNamed(iFn, "fseek")
		||  isCallOfFunctionNamed(iFn, "fsetpos")
		||  isCallOfFunctionNamed(iFn, "rewind")) {
			const SgVarRefExp *iVar = isSgVarRefExp(getFnArg(iFn, 0));
			if (iVar && (getRefDecl(iVar) == getRefDecl(fd))) {
				return false;
			}
			continue;
		}

		if ((read1 && isWriteFn(iFn, &argNum))
		|| (write1 && isReadFn(iFn, &argNum))) {
			const SgVarRefExp *iVar = isSgVarRefExp(getFnArg(iFn, argNum));
			if (iVar && (getRefDecl(iVar) == getRefDecl(fd))) {
				print_error(node, "FIO39-C", "Do not alternately input and output from a stream without an intervening flush or positioning call");
				return true;
			}
		}
	}

	return false;
}

/**
 * Ensure files are properly closed when they are no longer needed
 */
bool FIO42_C( const SgNode *node ) {
	const SgFunctionRefExp *fnRef = isSgFunctionRefExp(node);
	if (!fnRef)
		return false;
	if (!(isCallOfFunctionNamed(fnRef, "open")
		||isCallOfFunctionNamed(fnRef, "fopen")
		||isCallOfFunctionNamed(fnRef, "freopen")))
		return false;

	const SgInitializedName *fd = getVarAssignedTo(fnRef, NULL);
	if (!fd)
		return false;
	/**
	 * It's much to hard to analyze these kinds of variables
	 */
	if (isGlobalVar(fd) || isStaticVar(fd))
		return false;

	bool before = true;
	FOREACH_SUBNODE(findParentOfType(node, SgFunctionDefinition), nodes, i, V_SgFunctionRefExp) {
		const SgFunctionRefExp * iFn = isSgFunctionRefExp(*i);
		assert(iFn);
		if (before) {
			if (iFn == fnRef)
				before = false;
			continue;
		}

		/**
		 * ERR04 allows using exit to close file descriptors
		 */
		if (isCallOfFunctionNamed(iFn, "exit")
		  ||isCallOfFunctionNamed(iFn, "_Exit"))
			return false;
		if (!(isCallOfFunctionNamed(iFn, "close")
			||isCallOfFunctionNamed(iFn, "fclose")
			||isCallOfFunctionNamed(iFn, "fcntl")))
			continue;

		const SgVarRefExp *iFd = isSgVarRefExp(removeImplicitPromotions(getFnArg(iFn,0)));
		if (!iFd)
			continue;

		if (getRefDecl(iFd) == fd)
			return false;
	}

	print_error(node, "FIO42-C", "Ensure files are properly closed when they are no longer needed");
	return true;
}

/**
 * Do not use tmpfile()
 */
bool FIO43_C( const SgNode *node ) {
	const SgFunctionRefExp *fnRef = isSgFunctionRefExp(node);
	if (!(fnRef && isCallOfFunctionNamed(fnRef, "tmpfile")))
		return false;

	print_error( node, "FIO43-C", "Do not use tmpfile()");
	return true;
}

/**
 * Do not use fopen() on the results of tmpnam()
 */
bool FIO43_C_2( const SgNode *node ) {
	const SgFunctionRefExp *fnRef = isSgFunctionRefExp(node);
	if (!(fnRef && isCallOfFunctionNamed(fnRef, "tmpnam")))
		return false;

	if (!isVarUsedByFunction("fopen", isSgVarRefExp(getFnArg(fnRef, 0))))
		return false;

	print_error( node, "FIO43-C", "Do not use fopen() on the results of tmpnam()");
	return true;
}

/**
 * Do not use open() on the results of mktemp()
 */
bool FIO43_C_3( const SgNode *node ) {
	const SgFunctionRefExp *fnRef = isSgFunctionRefExp(node);
	if (!(fnRef && isCallOfFunctionNamed(fnRef, "mktemp")))
		return false;

	if (!isVarUsedByFunction("open", isSgVarRefExp( getFnArg(fnRef, 0))))
		return false;

	print_error( node, "FIO43-C", "Do not use open() on the results of mktemp()");
	return true;
}

/**
 * Do not use fopen_s() on the results of tmpnam_s()
 */
bool FIO43_C_4( const SgNode *node ) {
	const SgFunctionRefExp *fnRef = isSgFunctionRefExp(node);
	if (!(fnRef && isCallOfFunctionNamed(fnRef, "tmpnam_s")))
		return false;

	if (!isVarUsedByFunction("fopen_s", isSgVarRefExp( getFnArg(fnRef, 1))))
		return false;

	print_error( node, "FIO43-C", "Do not use fopen_s() on the results of tmpnam_s()");
	return true;
}

/**
 * Only use values for fsetpos() that are returned from fgetpos()
 */
bool FIO44_C( const SgNode *node) {
	const SgFunctionRefExp * setRef = isSgFunctionRefExp(node);
	if (!(setRef && isCallOfFunctionNamed(setRef,"fsetpos")))
		return false;

	const SgExpression * arg = removeImplicitPromotions(getFnArg(setRef, 1));
	assert(arg);
	const SgAddressOfOp *setAddr = isSgAddressOfOp(arg);
	assert(setAddr);
	const SgVarRefExp * posRef = isSgVarRefExp(setAddr->get_operand());
	if (!posRef)
		return false;
	const SgInitializedName *posName = getRefDecl(posRef);
	assert(posName);

	/* We can't really handle global variables, too complicated */
	if (isGlobalVar(posName))
		return false;

	FOREACH_SUBNODE(posName->get_scope(), nodes, i, V_SgVarRefExp) {
		const SgVarRefExp *iVar = isSgVarRefExp(*i);
		assert(iVar);

		if (getRefDecl(iVar) != posName)
			continue;

		const SgAddressOfOp *addrOp = isSgAddressOfOp(iVar->get_parent());
		if (!addrOp)
			continue;
		const SgFunctionCallExp *iFn = isSgFunctionCallExp(addrOp->get_parent()->get_parent());
		if (!iFn)
			continue;
		if (isCallOfFunctionNamed(isSgFunctionRefExp(iFn->get_function()), "fgetpos"))
			return false;
	}

	print_error(node, "FIO44-C", "Only use values for fsetpos() that are returned from fgetpos()");
	return true;
}

bool FIO(const SgNode *node) {
  bool violation = false;
  violation |= FIO01_C(node);
  violation |= FIO07_C(node);
  violation |= FIO08_C(node);
  violation |= FIO11_C(node);
  violation |= FIO12_C(node);
  violation |= FIO13_C(node);
  violation |= FIO30_C(node);
  violation |= FIO34_C(node);
  violation |= FIO38_C(node);
  violation |= FIO39_C(node);
  violation |= FIO42_C(node);
  violation |= FIO43_C(node);
  violation |= FIO43_C_2(node);
  violation |= FIO43_C_3(node);
  violation |= FIO43_C_4(node);
  violation |= FIO44_C(node);
  return violation;
}
