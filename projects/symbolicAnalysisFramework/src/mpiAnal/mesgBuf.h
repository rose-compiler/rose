#ifndef MESG_BUF_H
#define MESG_BUF_H

#include "mesgExpr.h"
#include "procSet.h"
#include "common.h"
#include "cfgUtils.h"
#include "variables.h"
#include "lattice.h"

class mesgBuf : public FiniteLattice
{
	// queue of message expressions, with the oldest at the front of mesgs and the newest at the back.
	// The message frontier is computer from front to back of mesgs.
	vector<mesgExpr*> mesgs;
	bool initialized;
	// top = unrepresentable message buffer;
	bool isTop;

	public:
	mesgBuf()
	{
		initialized = false;
		isTop = false;
	}
	
	mesgBuf(const mesgBuf& that)
	{
		mesgs = that.mesgs;
		initialized = that.initialized;
		isTop = that.isTop;
	}

	// inserts the given message at the back of the message queue
	void addMesgExpr(const mesgExpr& mesg);
	
	// Match the outgoing messages (this) from the set of sender processes (sendersRank) to the 
	// message expression (recv) of the set of receiver processes (recvrsRank). 
	// If the match was successful, returns true and
	//     senders - assigned to the set of processes in sendersRank that succeeded in sending a message;
	//               this mesgBuf is modified to correspond their new message buffer
	//     nonSenders - assigned of the set of processes in sendersRank that did not send a message;
	//               nonSendMesgs is set to a copy of this that corresponds to these processes' 
	//               message buffer
	//        senders and nonSenders must be non-overlapping and their union must = sendersRank
	//     receivers - assigned to the set of processes in recvrsRank that succeeded in receiving 
	//               a message from a process in senders; all these processes should be unblocked
	//     nonReceivers - assigned of the set of processes in sendersRank that did not receive a 
	//               message from a process in senders; these processes should remain blocked
	//        receivers and nonReceivers must be non-overlapping and their union must = recvrsRank
	bool match(const procSet& sendersRank, const procSet& recvrsRank, 
	           const mesgExpr& recv,
	           procSet** senders,   procSet** nonSenders, mesgBuf** nonSendMesgs,
	           procSet** receivers, procSet** nonReceivers/*, mesgExpr& nonRecvExpr*/);
	
	// Compacts adjacent message expressions to ensure that each message expression
	// contains the maximum amount of information
	// Returns true if this causes the message buffer to change and false otherwise.
	bool compact();
	
	// Compacts adjacent message expressions to ensure that each message expression
	// contains the maximum amount of information. Invoked after a single message expression
	// is modified, with the index of this expression passed in modIndex
	// Returns true if this causes the message buffer to change and false otherwise.
	bool compactIncremental(int modIndex);
	
	// Returns a string representation of this set
	string str(string indent="") const;
	
	/*********************************
	 * Functions required by Lattice *
	 *********************************/
	// initializes this Lattice to its default state, if it is not already initialized
	void initialize();
	
	// Sets this message buffer to top (unrepresentable message buffer)
	bool setToTop();
	
	// returns a copy of this lattice
	Lattice* copy() const;
	
	// overwrites the state of this Lattice with that of that Lattice
	void copy(Lattice* that);
	
	protected:
	// Deletes all messages in mesgs
	void deleteAllMesgs();
	
	public:
	/*
	// Called by analyses to create a copy of this lattice. However, if this lattice maintains any 
	//    information on a per-variable basis, these per-variable mappings must be converted from 
	//    the current set of variables to another set. This may be needed during function calls, 
	//    when dataflow information from the caller/callee needs to be transferred to the callee/calleer.
	// We do not force child classes to define their own versions of this function since not all
	//    Lattices have per-variable information.
	// varNameMap - maps all variable names that have changed, in each mapping pair, pair->first is the 
	//              old variable and pair->second is the new variable
	// func - the function that the copy Lattice will now be associated with
	virtual void remapVars(const map<varID, varID>& varNameMap, const Function& newFunc) {} 
	
	// Called by analyses to copy over from the that Lattice dataflow information into this Lattice.
	// that contains data for a set of variables and incorporateVars must overwrite the state of just
	// those variables, while leaving its state for other variables alone.
	// We do not force child classes to define their own versions of this function since not all
	//    Lattices have per-variable information.
	virtual void incorporateVars(Lattice* that) {}*/
	
	// computes the meet of this and that and saves the result in this
	// returns true if this causes this to change and false otherwise
	bool meetUpdate(Lattice* that);
	
	bool operator==(Lattice* that);
		
	// The string that represents this object
	// If indent!="", every line of this string must be prefixed by indent
	// The last character of the returned string should not be '\n', even if it is a multi-line string.
	string str(string indent="");
};

#endif
