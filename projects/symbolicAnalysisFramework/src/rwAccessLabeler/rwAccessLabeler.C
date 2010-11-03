#include "rose.h"
#include "rwAccessLabeler.h"

using namespace std;

namespace rwAccessLabeler {

// wrapper around pointers that allows for regular == matching as well as wild-card matching
class fancyPointer
{
	public:

	void* ptr;
	// indicates whether matching on this pointer will be exact (=true) or wildcard(=false)
	bool exactMatch;
	
	fancyPointer()
	{
		ptr=NULL;
		exactMatch=true;
	}
		
	fancyPointer(void* ptr, bool exactMatch)
	{
		this->ptr = ptr;
		this->exactMatch = exactMatch;
	}
	
	const bool operator == (const fancyPointer &that)
	{
		// if wildcard matching
		if(!exactMatch || !that.exactMatch)
			return true;
		else
			return (ptr == that.ptr);
	}
	
	const bool operator != (const fancyPointer &that)
	{
		return !(*this == that);
	}
	
	fancyPointer& operator=(const fancyPointer& that)
	{
		ptr = that.ptr;
		exactMatch = that.exactMatch;
	}
	
/*	const bool operator == (const SgNode* &that)
	{
		// if wildcard matching
		if(!exactMatch)
			return true;
		else
			return (ptr == that);
	}
	
	const bool operator != (const SgNode* &that)
	{
		return !(*this == that);
	}*/
	
	fancyPointer& operator=(void* ptr)
	{
		this->ptr = (void*)ptr;
		exactMatch = true;
		return *this;
	}
	
/*	fancyPointer& operator=(const unsigned long & ptr)
	{
		this->ptr = (void*)ptr;
		exactMatch = true;
	}*/
	
	void wildMatch()
	{
		exactMatch=false;
	}
};

bool operator == (fancyPointer &me, SgNode* &that)
{
	// if wildcard matching
	if(!me.exactMatch)
		return true;
	else
		return (me.ptr == that);
}

bool operator == (SgNode* &that, fancyPointer &me)
{ return me == that; }

bool operator != (fancyPointer &me, SgNode* &that)
{ return !(me == that); }

bool operator != (SgNode* &that, fancyPointer &me)
{ return me != that; }

class lrRecord
{
	public:
	// Pointers to the read, write and read-write subtrees of an assignment node. Used by its children to determine 
	// whether they lie on the node's reading, writing side or both. They are only set in SgNodes that perform, 
	// assignments, like SgAssignNode or SgPlusPlusOp, so these fields are =NULL in all other nodes.
	fancyPointer readSubtree;
	fancyPointer writeSubtree;
	fancyPointer rwSubtree;
	
	// The types of accesses performed on this side of the nearest SgAssignOp node, if any
	accessType access;
	
	// a default lrRecord (the one at the root of a tree)
	lrRecord()
	{
		readSubtree = (void*)NULL;
		writeSubtree = (void*)NULL;
		rwSubtree = (void*)NULL;
		access = readAccess;		
//		printf("Root access=%d\n", access);
	}
	
	// on other nodes
	lrRecord (lrRecord &parent, SgNode* n)
	{
		SgBinaryOp* binOp;
		SgUnaryOp* unOp;
		SgFunctionCallExp* funcCall;
		//SgPntrArrRefExp* arrRef;
		char typeStr[100];
		
		// if this node is on the read, write or read-write side of an assignment operation, set its access appropriately
		if(parent.readSubtree == n)
			access = readAccess;
		else if(n == parent.writeSubtree)
			access = writeAccess;
		else if(n == parent.rwSubtree)
			access = rwAccess;
		else
			access = parent.access;
		
		if((binOp = isSgBinaryOp(n)))
		{			
			// writeSubtree = readSubtree
			if(isSgAssignOp(binOp))
			{
				writeSubtree = binOp->get_lhs_operand();
				readSubtree = binOp->get_rhs_operand();
				rwSubtree = (void*)NULL;
				strcpy(typeStr, "SgAssignOp");
			}
			// rwSubtree op= readSubtree
			else if(isSgAndAssignOp(binOp) ||
			        isSgDivAssignOp(binOp) ||
			        isSgIorAssignOp(binOp) ||
			        isSgLshiftAssignOp(binOp) ||
			        isSgMinusAssignOp(binOp) ||
			        isSgModAssignOp(binOp) ||
			        isSgMultAssignOp(binOp) ||
			        isSgPlusAssignOp(binOp) ||
			        isSgRshiftAssignOp(binOp) ||
			        isSgXorAssignOp(binOp))
			{
				rwSubtree = binOp->get_lhs_operand();
				readSubtree = binOp->get_rhs_operand();
				writeSubtree = (void*)NULL;
				strcpy(typeStr, "Sg*AssignOp");
			}
			else if(isSgPntrArrRefExp(binOp))
			{
				// all the references involved in an array reference, whether they are used to compute the array name
				// or used in the argument, are read-only
				writeSubtree = (void*)NULL;
				readSubtree = (void*)NULL;
				readSubtree.wildMatch();
				rwSubtree = (void*)NULL;
				strcpy(typeStr, "SgPntrArrRefExp");
			}
			else 
			{
				readSubtree = (void*)NULL;
				writeSubtree = (void*)NULL;
				rwSubtree = (void*)NULL;
				strcpy(typeStr, "???");
			}
			
			//printf("SgBinaryNode 0x%x type %s access=%d: %s\n", binOp, typeStr, access, binOp->unparseToString().c_str());	
		}
		else if((unOp = isSgUnaryOp(n)))
		{
			// unary update operations have only one operand, which is read-write
			// writeSubtree
			if(isSgMinusMinusOp(unOp) ||
				isSgPlusPlusOp(unOp))
			{
				writeSubtree = (void*)NULL;
				readSubtree = (void*)NULL;
				rwSubtree = unOp->get_operand();
				strcpy(typeStr, "Sg**Op");
			}
			// dereference operations have a read-only operand
			else if(isSgPointerDerefExp(unOp))
			{
				writeSubtree = (void*)NULL;
				readSubtree = unOp->get_operand();
				rwSubtree = (void*)NULL;
				strcpy(typeStr, "isSgPointerDerefExp");
			}
			else 
			{
				readSubtree = (void*)NULL;
				writeSubtree = (void*)NULL;
				rwSubtree = (void*)NULL;
				strcpy(typeStr, "???");
			}
			//printf("SgUnaryNode 0x%x %s access=%d: %s\n", unOp, typeStr, access, unOp->unparseToString().c_str());
		}
		else if((funcCall = isSgFunctionCallExp(n)))
		{
			// all the references involved in a function call, whether they are used to compute the function pointer
			// or used in the argument, are read-only
			writeSubtree = (void*)NULL;
			readSubtree = (void*)NULL;
			readSubtree.wildMatch();
			rwSubtree = (void*)NULL;
			//printf("SgFunctionCall 0x%x access=%d: %s\n", funcCall, access, funcCall->unparseToString().c_str());
		}
		// else, if this is neither a binary, nor unary operation node
		else
		{
			// leave subtree fields of this record as NULL
			readSubtree = (void*)NULL;
			writeSubtree = (void*)NULL;
			rwSubtree = (void*)NULL;
			
			//printf("SgNode 0x%x access=%d: %s\n", n, access, n->unparseToString().c_str());
		}
	}
};

class rwAccessAttribute : public AstAttribute
{
	accessType access;

	public:	
	rwAccessAttribute(lrRecord r)
	{
		access = r.access;
	}
	
	rwAccessAttribute(const rwAccessAttribute& a)
	{
		access = a.access;
	}
	
	rwAccessAttribute(rwAccessAttribute* a)
	{
		access = a->access;
	}
	
	void setAccess(accessType access)
	{
		this->access = access;
	}
	
	accessType getAccess()
	{
		return access;
	}
	
	string getString()
	{
		return access==readAccess? "readAccess":
		       access==writeAccess? "writeAccess":
		       	                   "rwAccess";
	}
	
	bool operator == (rwAccessAttribute& that)
	{
		return access == that.access;
	}
	
	bool operator != (rwAccessAttribute& that)
	{
		return !(*this == that);
	}
};

class rwAccessLabeler : public AstTopDownProcessing<lrRecord>
{
	public:
	lrRecord evaluateInheritedAttribute(SgNode* n, lrRecord inherited )
	{
		// pass this node's left-right record down to its children
		lrRecord r(inherited, n);
		
		// label this node with its read/write access information
		rwAccessAttribute* rwAttr = new rwAccessAttribute(r);
		/*char attrName[100];
		sprintf(attrName, "rwAccess: %d", rwAttr->access);
		n->addNewAttribute(attrName, rwAttr);*/
		// if this node doesn't already have an attribute, add it
		if(!n->attributeExists("rwAccess"))
			n->addNewAttribute("rwAccess", rwAttr);
		else
		{
			rwAccessAttribute* oldAttr = (rwAccessAttribute*)n->getAttribute("rwAccess");
			// if it does, make sure that the old and the new attributes agree
			if(*oldAttr != *rwAttr)
				fprintf(stderr, "ERROR: old and new rwAttributes differ (Old: %s, New: %s)in SgNode <%s>!\n",
				        accessTypeToStr(oldAttr->getAccess()).c_str(), accessTypeToStr(rwAttr->getAccess()).c_str(), 
				        n->unparseToString().c_str());
		}
		
		return r;
	}
};

// set of SgNodes on which addRWAnnotations has already been executed
static set<SgNode*> priorRuns;

// labels the portion of the AST rooted at root with annotations that identify the various portions
// the read/write sides of SgAssignOp nodes (if they are)
void addRWAnnotations(SgNode* root)
{
	// only do the run if we haven't yet called addRWAnnotations on this SgNode
	if(priorRuns.find(root) == priorRuns.end())
	{
		rwAccessLabeler rwal;
		lrRecord	r;
	
		priorRuns.insert(root);
		rwal.traverse(root, r);
	}
}

// returns the type of access being performed at the given node
accessType getAccessType(SgNode* n)
{
	ROSE_ASSERT(n->getAttribute("rwAccess"));
	return ((rwAccessAttribute*)(n->getAttribute("rwAccess")))->getAccess();
}

// copies the access type annotation from src to tgt
void cloneAccessType(SgNode* tgt, SgNode* src)
{
	// only bother if the source node has an access attribute
	if(src->attributeExists("rwAccess"))
	{
		if(tgt->attributeExists("rwAccess"))
			((rwAccessAttribute*)tgt->getAttribute("rwAccess"))->setAccess(((rwAccessAttribute*)(src->getAttribute("rwAccess")))->getAccess());
		else
			tgt->addNewAttribute("rwAccess", new rwAccessAttribute(((rwAccessAttribute*)(src->getAttribute("rwAccess")))));
	}
}

// returns a string representation of the given accessType
string accessTypeToStr(accessType t)
{
	return (t==readAccess?"readAccess":(t==writeAccess?"writeAccess":(t==rwAccess?"rwAccess":"???")));
}

}
