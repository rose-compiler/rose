#include "mesgBuf.h"
#include <vector>

using namespace std;

// inserts the given message at the back of the message queue
void mesgBuf::addMesgExpr(const mesgExpr& mesg_arg)
{
	initialized = true;
	if(!isTop)
	{
		const OneDmesgExpr& mesg = dynamic_cast<const OneDmesgExpr&>(mesg_arg);
		mesgs.push_back(new OneDmesgExpr(mesg));
		compactIncremental(mesgs.size()-1);
	}
}

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
bool mesgBuf::match(const procSet& sendersRank, const procSet& recvrsRank, 
                    const mesgExpr& recv,
                    procSet** senders_arg, procSet** nonSenders_arg, mesgBuf** nonSendMesgs_arg,
                    procSet** receivers_arg, procSet** nonReceivers_arg/*, mesgExpr& nonRecvExpr*/)
{
	ROSE_ASSERT(recv.numCommOps()!=0 && !recv.isNULL());
		
	if(!initialized || isTop)
	{
		*senders_arg      = NULL;
		*nonSenders_arg   = NULL;
		*nonSendMesgs_arg = NULL;
      *receivers_arg    = NULL;
      *nonReceivers_arg = NULL;
      return false;
	}
	//procSet& rImage = recv.getImage(recvrsRank);
		
	//for(vector<mesgExpr&>::iterator it=mesgs.begin(); it!=mesgs.end(); it++)
	for(int i=0; i<mesgs.size(); i++)
	{
		//mesgExpr& send = *it;
		mesgExpr& send = *(mesgs[i]);
		ROSE_ASSERT(send.numCommOps()!=0 && !send.isNULL());
	
if(analysisDebugLevel>=1) 
{
	cout << "mesgBuf::match\n";
	cout << "    recv="<<recv.str()<<"\n";
	cout << "    send="<<send.str()<<"\n";
	cout << "    sendersRank="<<sendersRank.str()<<"\n";
}
		// Processes that successfully sent a message
		procSet& senders = send.getIdentityDomain(recv, send, sendersRank);
if(analysisDebugLevel>=1) cout << "    senders="<<senders.str()<<"\n";
		ROSE_ASSERT(senders.validSet());
		if(!senders.emptySet())
		{
			// Processes that did not send a message
			procSet& nonSenders = sendersRank.rem(senders);
if(analysisDebugLevel>=1) cout << "    nonSenders="<<nonSenders.str()<<"\n";
			ROSE_ASSERT(nonSenders.validSet());
			
			// Processes that successfully received the message
			procSet& receivers = send.getImage(senders);
if(analysisDebugLevel>=1) cout << "    receivers="<<receivers.str()<<"\n";
			ROSE_ASSERT(receivers.validSet());

			// Processes that did not receive the message
			procSet& nonReceivers = recvrsRank.rem(receivers);
if(analysisDebugLevel>=1) cout << "    nonReceivers="<<nonReceivers.str()<<"\n";
			ROSE_ASSERT(nonReceivers.validSet());
				
			mesgBuf* nonSendMesgs;
			if(!nonSenders.emptySet())
			{
				// Non-senders need an un-touched copy of this message buffer
				nonSendMesgs = new mesgBuf(*this);
if(analysisDebugLevel>=1) cout << "    nonSendMesgs="<<nonSendMesgs->str()<<"\n";
			}
			else
				nonSendMesgs = NULL;
			
			// Senders use the send mesgExpr without the send-receive matches
			send.removeMap(senders, receivers);
			
			// If this send message expression is empty as a result of the match
			if(send.numCommOps()==0 || send.isNULL())
			{
				// Erase it
				delete mesgs[i];
				mesgs.erase(mesgs.begin()+i);
			}
			else
				// Otherwise, see if we can compact this message buffer a little more
				compactIncremental(i);
			
			/*if(!nonReceivers.emptySet())
			{
				// Non-receivers need an un-touched copy of the origin recv mesgExpr
				nonRecvExpr = *(recv.copy());
			}
			
			// Receivers use the recv mesgExpr without the send-receive matches
			recv.removeMap(receivers, senders);*/
			
			*senders_arg      = &senders;
			*nonSenders_arg   = &nonSenders;
			*nonSendMesgs_arg = nonSendMesgs;
         *receivers_arg    = &receivers;
         *nonReceivers_arg = &nonReceivers;
			
/*const contRangeProcSet& senders_crps = dynamic_cast<const contRangeProcSet&>(senders);
cout << "cg = "<<senders_crps.getConstr()->str()<<"\n";*/
			
			return true;
		}
	}

	// If we went through all the in-flight messages and couldn't find a match
	/*nonSenders = sendersRank;
	receivers.emptify();
	nonReceivers = recvrsRank;*/
	return false;
}

// Compacts adjacent message expressions to ensure that each message expression
// contains the maximum amount of information
// Returns true if this causes the message buffer to change and false otherwise.
bool mesgBuf::compact()
{
	bool modified = false;
	
	if(!initialized || isTop)
		return false;
	
	//for(list<mesgExpr&>::iterator it=mesgs.begin(); it!=mesgs.end(); )
	for(int i=0; i<(mesgs.size()-1); )
	{
		mesgExpr& sendCur = *(mesgs[i]);
		ROSE_ASSERT(sendCur.numCommOps()!=0 && !sendCur.isNULL());
		mesgExpr& sendNext = *(mesgs[i+1]);
		ROSE_ASSERT(sendNext.numCommOps()!=0 && !sendNext.isNULL());
		
		// If the current message expression is empty, remove it
		if(sendCur.numCommOps()==0)
		{
			delete mesgs[i];
			mesgs.erase(mesgs.begin() + i);
		}
		// If the current pair of adjacent message expressions can be merged, 
		// do so and remove the second
		else if(sendCur.mustMergeable(sendNext))
		{
			modified = sendCur.mergeUpd(sendNext) || modified;
			delete mesgs[i+1];
			mesgs.erase(mesgs.begin()+(i+1));
		}
		// If not, move on to the next pair
		else
			i++;
	}
	
	return modified;
}

// Compacts adjacent message expressions to ensure that each message expression
// contains the maximum amount of information. Invoked after a single message expression
// is modified, with the index of this expression passed in modIndex
// Returns true if this causes the message buffer to change and false otherwise.
bool mesgBuf::compactIncremental(int modIndex)
{
	bool modified = false;
	int i;
	
	if(!initialized || isTop)
		return false;
	
	//for(list<mesgExpr&>::iterator it=mesgs.begin(); it!=mesgs.end(); )
	// The message expression at modIndex got modified, so start the compaction
	// at the preceding message expression (if possible), propagating the merges
	// first backwards, then forwards for as long as we see changes.
	
	// If the modified message expression is empty, remove it and only do the 
	// forwards propagation to merge the mesgExprs before the modified expression 
	// and the mesgExprs after the modified expression.
	if(mesgs[modIndex]->numCommOps()==0)
	{
		delete mesgs[modIndex];
		mesgs.erase(mesgs.begin() + modIndex);
		if(modIndex==0) return true;
		
		i = modIndex-1;
	}
	else
	{
		// Backwards propagation (towards the frontier)
		for(i=modIndex; i>=1; )
		{
			mesgExpr& sendCur = *(mesgs[i]);
			ROSE_ASSERT(sendCur.numCommOps()!=0 && !sendCur.isNULL());
			mesgExpr& sendPrev = *(mesgs[i-1]);
			ROSE_ASSERT(sendPrev.numCommOps()!=0 && !sendPrev.isNULL());
			
			// If the current pair of adjacent message expressions can be merged, 
			// do so and remove the second
			if(sendPrev.mustMergeable(sendPrev))
			{
				modified = sendPrev.mergeUpd(sendCur) || modified;
				delete mesgs[i];
				mesgs.erase(mesgs.begin()+i);
				i--;
			}
			// If not, we're done with the backwards pass, since the change to message 
			// expression at index modIndex will not propagate any further up the mesgs list
			else
				break;
		}
	}
	
	for(; i<(mesgs.size()-1); )
	{
		mesgExpr& sendCur = *(mesgs[i]);
		ROSE_ASSERT(sendCur.numCommOps()!=0 && !sendCur.isNULL());
		mesgExpr& sendNext = *(mesgs[i+1]);
		ROSE_ASSERT(sendNext.numCommOps()!=0 && !sendNext.isNULL());
		
		// If the current pair of adjacent message expressions can be merged, 
		// do so and remove the second
		if(sendCur.mustMergeable(sendNext))
		{
			modified = sendCur.mergeUpd(sendNext) || modified;
			delete mesgs[i+1];
			mesgs.erase(mesgs.begin()+(i+1));
		}
		// If not, we're done with the forwards pass since the change to message expression 
		// at index modIndex will not propagate any further down the mesgs list
		else
			return modified;
	}
	
	return modified;
}

// Returns a string representation of this set
string mesgBuf::str(string indent) const
{
	ostringstream outs;
	outs << indent << "[mesgBuf: ";
	if(!initialized)
		outs << "Uninitialized]";
	else if(isTop)
		outs << "Top]";
	else
	{
		for(vector<mesgExpr*>::const_iterator it = mesgs.begin(); it != mesgs.end(); )
		{
			outs << indent << "    " << (*it)->str();
			it++;
		}
		if(mesgs.size()>0)
			outs << indent << "]";
		else
			outs << "]";
	}
	
	return outs.str();
}

// Returns a string representation of this set
string mesgBuf::str(string indent)
{
	ostringstream outs;
	outs << indent << "[mesgBuf: ";
	if(!initialized)
		outs << "Uninitialized]";
	else if(isTop)
		outs << "Top]";
	else
	{
		for(vector<mesgExpr*>::const_iterator it = mesgs.begin(); it != mesgs.end(); )
		{
			outs << indent << "    " << (*it)->str();
			it++;
		}
		if(mesgs.size()>0)
			outs << indent << "]";
		else
			outs << "]";
	}
	
	return outs.str();
}

// initializes this Lattice to its default state, if it is not already initialized
void mesgBuf::initialize()
{
	initialized=true;
}

// Sets this message buffer to top (unrepresentable message buffer)
// returns true if this causes this message buffer to change, false otherwise
bool mesgBuf::setToTop()
{
	bool modified = (initialized == false) ||
	                mesgs.size()>0 ||
	                isTop==false;
	initialized=true;
	deleteAllMesgs();
	isTop=true;
	
	return modified;
}

// returns a copy of this lattice
Lattice* mesgBuf::copy() const
{
	return new mesgBuf(*this);
}

// overwrites the state of this Lattice with that of that Lattice
void mesgBuf::copy(Lattice* that_arg)
{
	mesgBuf* that = dynamic_cast<mesgBuf*>(that_arg);
	
	initialized = that->initialized;
	deleteAllMesgs();
	// copy all messages from that to this
	for(vector<mesgExpr*>::iterator it=that->mesgs.begin(); 
	    it!=that->mesgs.end(); it++)
	{
		mesgs.push_back(&((*it)->copy()));
	}
}

// Deletes all messages in mesgs
void mesgBuf::deleteAllMesgs()
{
	ROSE_ASSERT(initialized || mesgs.size()==0);
	for(vector<mesgExpr*>::iterator it=mesgs.begin(); it!=mesgs.end(); it++)
		delete *it;
	mesgs.clear();
}

// computes the meet of this and that and saves the result in this
// returns true if this causes this to change and false otherwise
bool mesgBuf::meetUpdate(Lattice* that_arg)
{
	mesgBuf* that = dynamic_cast<mesgBuf*>(that_arg);
	
	bool modified=false;
	if(!initialized)
	{
		modified = !(*this == that);
		copy(that);
	}
	else if(!that->initialized)
		modified = false;
	else
	{
		if(!(*this == that))
		{
			modified = setToTop();
		}
		/*
		// Aligns mesgs and that->mesgs from the start (frontier) of both lists
		// all the way to their rear. At each point in the iteration, 
		// the current mesgExprs in both lists are either equal or mergeable.
		// They will be merged in the meet list (mesgs).
		vector<mesgExpr*>::iterator itThis, itThat
		for(itThis=mesgs.begin(), itThat=that->mesgs.begin(); 
		    itThis!=mesgs.end() && itThat!=that->mesgs.end(); 
		    itThis++, itThat++)
		{
			if(*itThis != *itThat)
			{
				
			}
		}*/
		
	}
	return modified;
}

bool mesgBuf::operator==(Lattice* that_arg)
{
	mesgBuf* that = dynamic_cast<mesgBuf*>(that_arg);
	
	if(initialized == that->initialized &&
	   isTop == that->isTop &&
	   mesgs.size() == that->mesgs.size())
	{
		// compare each element of mesgs
		vector<mesgExpr*>::iterator itThis, itThat;
		for(itThis=mesgs.begin(), itThat=that->mesgs.begin(); 
		    itThis!=mesgs.end() && itThat!=that->mesgs.end(); 
		    itThis++, itThat++)
		{
			if(*itThis != *itThat)
				return false;
		}
		return true;
	}
	return false;
}
