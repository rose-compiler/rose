/**
 * \file SIG.C
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


#include <list>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include "rose.h"
#include "utilities.h"

using namespace std;


#ifdef UNDEFINED
/**
 * Avoid using the same handler for multiple signals
 *
 * \note obsolete
 */
bool SIG00_C( const SgNode *node ) {
  static set<const SgFunctionSymbol*> All_Handlers;

  if (!isCallOfFunctionNamed( node, "signal")) return false;
  const SgFunctionRefExp* sig_fn = isSgFunctionRefExp( node);
  assert(sig_fn != NULL);
  const SgExpression* ref = getFnArg( sig_fn, 1);
  const SgFunctionRefExp* handler = isSgFunctionRefExp( ref);
  if (handler == NULL) return false; // no signal handler
  const SgFunctionSymbol* symbol = handler->get_symbol();

  // We can excuse any 1-arg function assigning itself as a handler
  const SgNode* encloser = findParentOfType(node, SgFunctionDefinition);
  if (encloser == handler->get_symbol()->get_declaration()->get_definition()) {
    const SgVarRefExp* signal_var = isSgVarRefExp( getFnArg( sig_fn, 0));
    const SgFunctionParameterList *plist = handler->get_symbol()->get_declaration()->get_parameterList();
    if (plist->get_args().size() >= 1) {
      const SgInitializedName* arg0 = *(plist->get_args().begin());
      if (signal_var->get_symbol()->get_declaration() == arg0)
	return false;
    }
  }

  if (All_Handlers.insert( symbol).second) return false;
  print_error( node, "SIG00-C", "Avoid using the same handler for multiple signals", true);
  return true;
}
#endif

/**
 * \note should really go into a file somewhere
 */
set<SgName> load_async_fns() {
  static const string posix_async_safe_fns =
    "_Exit _exit abort accept access aio_error aio_return aio_suspend"
    "alarm bind cfgetispeed cfgetospeed cfsetispeed cfsetospeed chdir chmod"
    "chown clock_gettime close connect creat dup dup2 execle"
    "execve fchmod fchown fcntl fdatasync fork fpathconf fstat"
    "fsync ftruncate getegid geteuid getgid getgroups getpeername getpgrp"
    "getpid getppid getsockname getsockopt getuid kill link listen"
    "lseek lstat mkdir mkfifo open pathconf pause pipe"
    "poll posix_trace_event pselect raise read readlink recv recvfrom"
    "recvmsg rename rmdir select sem_post send sendmsg sendto"
    "setgid setpgid setsid setsockopt setuid shutdown sigaction sigaddset"
    "sigdelset sigemptyset sigfillset sigismember sleep signal sigpause sigpending"
    "sigprocmask sigqueue sigset sigsuspend sockatmark socket socketpair stat"
    "symlink sysconf tcdrain tcflow tcflush tcgetattr tcgetpgrp tcsendbreak"
    "tcsetattr tcsetpgrp time timer_getoverrun timer_gettime timer_settime times umask"
    "uname unlink utime wait waitpid write";

  set<SgName> functions;
  istringstream i( posix_async_safe_fns);
  string name;
  while (!i.eof()) {
    i >> name;
    functions.insert(name);
  }
  return functions;
}

/// Set of async-safe functions we know about
set<SgName> Async_Fns = load_async_fns();

/// Current stack of functions examined by non_async_fn
vector<SgName> Async_Stack;

/**
 * Returns node of a non-async fn in the definition of handler or NULL if
 * none. Descends recursively through function calls.
 */
const SgFunctionRefExp* non_async_fn(const SgFunctionRefExp* handler) {
	const SgName name = handler->get_symbol()->get_name();

	if (Async_Fns.find( name) != Async_Fns.end())
		return NULL; // this fn is async-save

	/// Recursive functions are assumed async-safe
	if (find( Async_Stack.begin(), Async_Stack.end(), name) != Async_Stack.end())
		return NULL;

	const SgFunctionDefinition* def = handler->get_symbol()->get_declaration()->get_definition();
	/// if handler not defined or in async list, we assume it is not async-safe
	if (def == NULL) return handler;

	/// Walk through definition ensuring that all function calls are async-safe
	Async_Stack.push_back( name);
	const SgFunctionRefExp* result = NULL;
	FOREACH_SUBNODE( def, nodes, i, V_SgFunctionRefExp) {
		const SgFunctionRefExp* fn_ref = isSgFunctionRefExp(*i);
		assert( fn_ref != NULL);
		if (non_async_fn( fn_ref)) {
			result = fn_ref;
			break;
		}
	}
	Async_Stack.pop_back();

	/// If no unsafe functions called, add to async-safe set
	if (result == NULL)
		Async_Fns.insert( name);
	return result;
}


/**
 * Call only async-safe functions in a signal handler
 */
bool SIG30_C( const SgNode *node ) {
	const SgFunctionRefExp* fnRef = isSgFunctionRefExp( node);
	if (!(fnRef && isCallOfFunctionNamed(fnRef, "signal")))
		return false;
	const SgExpression* ref = getFnArg(fnRef, 1);
	if (!ref)
		return false;
	const SgFunctionRefExp* handler = isSgFunctionRefExp( ref);
	if (handler == NULL)
		return false; // no signal handler
	const SgFunctionRefExp* bad_node = non_async_fn( handler);
	if (bad_node == NULL)
		return false;
	print_error( bad_node, "SIG30-C", ("Call only asynchronous-safe functions within signal handlers: " + bad_node->get_symbol()->get_name().getString()).c_str());
	return true;
}

/**
 * Do not access or modify shared objects in a signal handler
 *
 * Specifically, this rule only ensures that any variable referenced is either
 * local or is static volatile sig_atomic_t
 */
bool SIG31_C( const SgNode *node ) {
	const SgFunctionRefExp* sig_fn = isSgFunctionRefExp( node);
	if (!(sig_fn && isCallOfFunctionNamed(sig_fn, "signal")))
		return false;

	const SgFunctionRefExp* handler = isSgFunctionRefExp(getFnArg(sig_fn,1));
	if (handler == NULL)
		return false; // no signal handler
	const SgFunctionDefinition* def = handler->get_symbol()->get_declaration()->get_definition();

	/**
	 * Added because of C++ code
	 */
	if (!def)
		return false;
	FOREACH_SUBNODE(def, nodes, i, V_SgVarRefExp) {
		const SgVarRefExp* var_ref = isSgVarRefExp(*i);
		assert( var_ref != NULL);

		SgInitializedName* var_decl = var_ref->get_symbol()->get_declaration();
		assert( var_decl != NULL);

		const SgNode* encloser = findParentOfType(var_decl, SgFunctionParameterList);
		if (encloser != NULL)
			continue; // variable is function arg

		// We only care about global variables really
		const SgInitializedName* decl = getRefDecl(var_ref);
		assert(decl);
		if (!isGlobalVar(decl))
			continue;

		const SgType* var_type = var_ref->get_type();

		// We assume that sig_atomic_t is a typedef (not a macro)
		static const string sig_atomic_typename = "sig_atomic_t";
		const SgNamedType* named_type = isSgNamedType(stripModifiers(var_type));

		bool compliant = true;
		if (!isVolatileType(var_type))
			compliant = false;
		if (named_type == NULL || named_type->get_name() != sig_atomic_typename)
			compliant = false;

		if (!isGlobalVar(decl) && !isStaticVar(decl))
			compliant = false;
#if 0
		const SgAssignOp* assignment = isSgAssignOp( var_ref->get_parent());
		if (assignment == NULL)
			compliant = false;
		else if (assignment->get_lhs_operand() != var_ref)
			compliant = false;
#endif
		if (compliant)
			continue;

		print_error( *i, "SIG31-C", "Do not access or modify shared objects in a signal handler");
		return true;
	}
	return false;
}

/**
 * Do not call longjmp() from within a signal handler
 */
bool SIG32_C( const SgNode *node ) {
	const SgFunctionRefExp* sig_fn = isSgFunctionRefExp( node);
	if (!(sig_fn && isCallOfFunctionNamed(sig_fn, "signal")))
		return false;
	const SgExpression* ref = getFnArg( sig_fn, 1);
	const SgFunctionRefExp* handler = isSgFunctionRefExp( ref);
	if (handler == NULL)
		return false; // no signal handler
	const SgFunctionDefinition* def = handler->get_symbol()->get_declaration()->get_definition();

	/**
	 * Added because of C++ code
	 */
	if (!def)
		return false;
	FOREACH_SUBNODE(def,nodes, i, V_SgFunctionRefExp ) {
		if (isCallOfFunctionNamed(isSgFunctionRefExp(*i), "longjmp")) {
			print_error( *i, "SIG32-C", "Do not call longjmp() from within a signal handler");
			return true;
		}
	}
	return false;
}

/**
 * Do not recursively invoke the raise() function
 */
bool SIG33_C( const SgNode *node ) {
	const SgFunctionRefExp *sigRef = isSgFunctionRefExp(node);
	if (!(sigRef && isCallOfFunctionNamed(sigRef, "signal")))
		return false;

	/**
	 * We can only handle this if we know explicitly what signal is assigned
	 * to this handler.
	 */
	const SgIntVal *sigInt = isSgIntVal(getFnArg(sigRef,0));
	if (!sigInt)
		return false;
	const int signum = sigInt->get_value();

	const SgFunctionRefExp* ref = isSgFunctionRefExp(getFnArg(sigRef, 1));
	assert(ref);

	const SgFunctionDeclaration *fnDecl = ref->get_symbol()->get_declaration();
	assert(fnDecl);

	/**
	 * See if there's a call too raise in this handler
	 */
	bool raise = false;
	FOREACH_SUBNODE(fnDecl, nodes1, i, V_SgFunctionRefExp) {
		if (isCallOfFunctionNamed(isSgFunctionRefExp(*i), "raise"))
			raise = true;
	}
	if (!raise)
		return false;

	/**
	 * See if there's another raise that can raise this signal
	 */
	const SgFunctionRefExp *fnCall;
	bool violation = false;
	FOREACH_SUBNODE(fnDecl->get_scope(), nodes, j, V_SgFunctionRefExp) {
		fnCall = isSgFunctionRefExp(*j);;
		if (isCallOfFunctionNamed(fnCall, "raise")) {
			const SgIntVal *raiseInt = isSgIntVal(getFnArg(fnCall, 0));
			if (!raiseInt
			|| (raiseInt->get_value() != signum))
				continue;
			print_error(fnCall, "SIG33-C", "Do not recursively invoke the raise() function");
			violation = true;
		}
	}

	return violation;
}

/**
 * Do not call signal() from within interruptible signal handlers
 */
bool SIG34_C( const SgNode *node ) {
	const SgFunctionRefExp *sigRef = isSgFunctionRefExp(node);
	if (!(sigRef && isCallOfFunctionNamed(sigRef, "signal")))
		return false;

	const SgFunctionCallExp *sigCall = isSgFunctionCallExp(sigRef->get_parent());
	if (!sigCall)
		return false;

	/**
	 * Usually this is a value, but we really have no idea what type the macro
	 * will expand to, so it's easier to just look at the string, which will
	 * be either a number (or a variable name)
	 */
	const std::string sigStr = getFnArg(sigCall, 0)->unparseToString();

	const SgFunctionRefExp *handlerRef = isSgFunctionRefExp(getFnArg(isSgFunctionRefExp(node), 1));
	if (!handlerRef)
		return false;

	const SgFunctionDeclaration *handlerDecl = handlerRef->get_symbol()->get_declaration();
	assert(handlerDecl);

	const SgFunctionDefinition *handlerDef = handlerDecl->get_definition();
	/**
	 * Assertion removed b/c of C++ code
	 */
	if(! handlerDef)
		return false;

	/**
	 * Because we are looking at a recursive call, we should ignore the case
	 * where the "node" is the same as the iCall that we will find later,
	 * otherwise we will report the same line twice
	 */
	if (findParentOfType(sigRef, SgFunctionDefinition) == handlerDef)
		return false;

	const SgInitializedName *signum = handlerDecl->get_args().front();
	assert(signum);

	bool violation = false;

	FOREACH_SUBNODE(handlerDef, nodes, i, V_SgFunctionCallExp) {
		const SgFunctionCallExp *iCall = isSgFunctionCallExp(*i);
		assert(iCall);
		const SgFunctionRefExp *iSig = isSgFunctionRefExp(iCall->get_function());
		assert(iSig);
		if (!isCallOfFunctionNamed(iSig, "signal"))
			continue;

		const SgFunctionRefExp *iRef = isSgFunctionRefExp(getFnArg(isSgFunctionRefExp(iSig), 1));
		if (!iRef)
			continue;

		if (iRef->get_symbol() != handlerRef->get_symbol())
			continue;

		const SgExpression *sigExp = getFnArg(iSig, 0);
		assert(sigExp);

		/**
		 * If we are not assigning to the same signal, ignore
		 */
		if (isSgVarRefExp(sigExp)) {
			if (getRefDecl(isSgVarRefExp(sigExp)) != signum)
				continue;
		} else if(sigExp->unparseToString() != sigStr) {
			continue;
		}

		print_error(iSig, "SIG34-C", "Do not call signal() from within interruptible signal handlers");
		violation = true;
	}

	return violation;
}

bool SIG(const SgNode *node) {
  bool violation = false;
  //  violation |= SIG00_C(node);
  violation |= SIG30_C(node);
  violation |= SIG31_C(node);
  violation |= SIG32_C(node);
  violation |= SIG33_C(node);
  violation |= SIG34_C(node);
  return violation;
}
