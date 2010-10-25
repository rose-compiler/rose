#include "unionFind.h"
#include "cfgUtils.h"
#include "VirtualCFGIterator.h"
#include "rwAccessLabeler.h"
#include "arrIndexLabeler.h"

// performs the Union Find algorithm to determine the set of variables that are
// related to the variables in varsConsidered. The algorithm works as follows:
// - looks over all statements in the program
//   - for assignments (i=j+c) or conditions (i<=j+c): combines i and j into the 
//       same set. this produces a set of equivalence classes of variables
//   - for array accesses that touch variables in varsConsidered, adds their read
//       and write array indexes and saves them to set xvar
//   - unions the equivalence classes of all variables in xvar into a single 
//       equivalence class and returns it.
//#define DEBUG_FLAG1_INI
varIDSet UnionFind::determineInterestSet (const DataflowNode &cfg, 
                                 varIDSet &varsConsidered/*,
                                 varIDSet &vars*/)
{
#ifdef DEBUG_FLAG1_INI
printf("UnionFind::determineInterestSet\n");
#endif

	// set of all index variables used in accesses to variables in varsConsidered
	varIDSet xvar;
	bool undefinedRd = false, undefinedWr = false;

	// combine "0" and all the variables in varsConsidered into a single equivalence class
	{			
		for ( varIDSet::iterator curVar = varsConsidered.begin(); 
				curVar != varsConsidered.end(); curVar++ )
			unionSets(*curVar, zeroVar);
	}

	// iterate over all the nodes in the CFG
	for(VirtualCFG::iterator it(cfg); 
	    it!=VirtualCFG::iterator::end(); it++)
	{
#ifdef DEBUG_FLAG1_INI
		printf("   Node: 0x%x[%s]<%s>\n", (*it).getNode(), (*it).getNode()->class_name().c_str(), (*it).getNode()->unparseToString().c_str());
#endif
   	SgNode *n = (*it).getNode();
   	
		varID i, j, k;
		long c;
		short op;
		bool negI, negJ, negK;
		bool isRead;
		map<varID, bool> rdFromExp, wrFromExp;
		map<varID, m_varID2quad> wrIndex, rdIndex;

		SgNode* fct = 0;
		AstInterface::AstNodeList args;

		/*printf("      isSgPntrArrRefExp(n)=0x%x\n", isSgPntrArrRefExp(n));
		if(isSgPntrArrRefExp(n))
			printf("      isTopArrayRefExp(n)=%d\n", arrIndexLabeler::isTopArrayRefExp(n));*/
		// if n is the top-most SgPntrArrRefExp node of a full SgPntrArrRefExp tree
		SgPntrArrRefExp* arrRef;
		SgExpression* expr;
		if((arrRef = isSgPntrArrRefExp(n)) && arrIndexLabeler::isTopArrayRefExp(n))
		{
			
			//printf("      getArrayNameExp(n)=0x%x <%s>\n", arrIndexLabeler::getArrayNameExp(n), n->unparseToString().c_str());
			ROSE_ASSERT(varID::isValidVarExp(arrIndexLabeler::getArrayNameExp(n)));
			
			varID arrayVar(arrIndexLabeler::getArrayNameExp(n));
			/*printf("   arrayVar %s access type: %s\n", arrayVar.str().c_str(), rwAccessLabeler::getAccessType(n)==rwAccessLabeler::readAccess?"readAccess":
			                                            rwAccessLabeler::getAccessType(n)==rwAccessLabeler::writeAccess?"writeAccess":
			                                            rwAccessLabeler::getAccessType(n)==rwAccessLabeler::rwAccess?"rwAccess":
			                                            "???");*/
			
			// unify all the variables used to perform this SgPntrArrRefExp into the same set
			varIDSet allArrRefVars = getVarRefsInSubtree(n);
			for(varIDSet::iterator it = allArrRefVars.begin(); it != allArrRefVars.end(); it++)
			{
				unionSets(arrayVar, *it);
			}
			                                          
			/*// iterate through all of this SgPntrArrRefExp's index expressions
			SgExpression* curArrRef = arrRef; 
			// while we're still operating on the same SgPntrArrRefExp
			while(SgPntrArrRefExp(curIndexExp))
			{
				varID j;
				long c;
				// try to parse this index operation
				bool simIndexExp = parseAddition(curndexExp->get_lhs_operand()e, j, c);
				
				// if this is a simple index expression of form c, -c, j, j + c, j - c
				if(simpIndexExp)
				{
					// record this index expression, noting whether this SgPntrArrRefExp was 
					// used as a read and/or write access
					pair<varID, long> p(j, c);
					
					if(rwAccessLabeler::getAccessType(n) == rwAccessLabeler::readAccess ||
						rwAccessLabeler::getAccessType(n) == rwAccessLabeler::rwAccess)
					{
						// add <j,c> to the set of indexes written to using a simple expression
						rdIndex[arrayVar].insert(p);
					}
					
					if(rwAccessLabeler::getAccessType(n) == rwAccessLabeler::writeAccess ||
						rwAccessLabeler::getAccessType(n) == rwAccessLabeler::rwAccess)
					{
						// add <j,c> to the set of indexes written to using a simple expression
						wrIndex[arrayVar].insert(p);
					}
				}
				// else, if this is a complex index expression 
				else
				{
					// record that this array was indexed using a complex expression, noting 
					// whether this SgPntrArrRefExp was used as a read and/or write access
					pair<varID, long> p(j, c);
					
					if(rwAccessLabeler::getAccessType(n) == rwAccessLabeler::readAccess ||
						rwAccessLabeler::getAccessType(n) == rwAccessLabeler::rwAccess)
						rdFromExp[arrayVar]=true;
					else
						rdFromExp[arrayVar]=false;
					
					if(rwAccessLabeler::getAccessType(n) == rwAccessLabeler::writeAccess ||
						rwAccessLabeler::getAccessType(n) == rwAccessLabeler::rwAccess)
						wrFromExp[arrayVar]=true;
					else
						wrFromExp[arrayVar]=false;
				}
				
				// move onto this SgPntrArrRefExp' next index expression
				curIndexExp = curIndexExp->get_lhs_operand();
			}*/
		}
		else if(expr = isSgExpression(n))
		{
			// if this is an assignment of the form (i=j+c)
			if(isSgAssignOp(expr) && cfgUtils::parseAssignment(expr, op, i, j, negJ, k, negK, c))
			{
				/*printf("expr=%s\n", expr->unparseToString().c_str());
				printf("%s = %s + %d\n", i.str().c_str(), j.str().c_str(), c);*/
				ROSE_ASSERT(i!=zeroVar);
	
				// skip i=c
				if (j!=zeroVar)
					// combine i and j into the same set
					unionSets(i, j);
				if (k!=zeroVar)
					// combine i, j and k into the same set
					unionSets(i, k);
			}
			// if this is a conditional of the form (i<=j+c)
			else if(cfgUtils::computeTermsOfIfCondition_LTEQ(expr, i, negI, j, negJ, c))
			{
				/*printf("expr=%s\n", expr->unparseToString().c_str());
				printf("%s <= %s + %d\n", i.str().c_str(), j.str().c_str(), c);*/
				ROSE_ASSERT(i!=zeroVar);
				
				// skip i=c
				if(j!=zeroVar)
					// combine i and j into the same set
					unionSets(i, j);
			}
		}
	}
/*		// parse assignment
		if(isSgAssignOp(node))
		//if ( AstInterface::IsAssignment( ast_node ) )
		{
#ifdef DEBUG_FLAG1_INI
			cout << "    is assignment\n";
#endif
			// has array access that refers to curVar
			if ( CFGUtils::findArrayAccesses( ast_node, wrIndex, rdIndex, 
				                               rdFromExp, wrFromExp, 
				                               varsConsidered, vars) )
				{
#ifdef DEBUG_FLAG1_INI
			cout << "    is an array access\n";
#endif
					// iterate over all the variables in varsConsidered to see if it contains
					// an array access to any of them
					for ( varIDSet::iterator curVar = varsConsidered.begin(); 
					      curVar != varsConsidered.end(); curVar++ )
					{
						// if some array expressions couldn't be parsed, set the undefinedRd
						// and undefinedWr appropriately
						if( rdFromExp[*curVar] && rdIndex[*curVar].size() > 0 )
								undefinedRd = true;
						if( wrFromExp[*curVar] && wrIndex[*curVar].size() > 0 )
								undefinedWr = true;
#ifdef DEBUG_FLAG1_INI
						cout << "rdFromExp: " << rdFromExp[*curVar] << "  wrFromExp: " << wrFromExp[*curVar] << "rd size: " << rdIndex[*curVar].size()
								 << "\twr size: " << wrIndex[*curVar].size() << "\n";
#endif
	
						// add all the variables used in array access expressions to xvar
						for ( m_quad2quad::iterator it = rdIndex[*curVar].begin(); 
						      it != rdIndex[*curVar].end(); it++ )
						{
#ifdef DEBUG_FLAG1_INI
cout << vars[it->first] << " used as index in read access of interesting array. Merging...\n";
#endif
							unionSets(it->first, zeroVar);
							//xvar.insert( it->first );
						}
						for ( m_quad2quad::iterator it = wrIndex[*curVar].begin(); 
						      it != wrIndex[*curVar].end(); it++ )
						{
#ifdef DEBUG_FLAG1_INI
cout << vars[it->first] << " used as index in write access of interesting array. Merging...\n"; 
#endif
							unionSets(it->first, zeroVar);
							//xvar.insert( it->first );
						}
					}
				}
				// without array access that refers to curVar
				else
				{
					// if this is an assignment of the form (i=j+c)
					if ( CFGUtils::parseAssignment( ast_node, &i, &j, &c) )
					{
						ROSE_ASSERT(i!=zeroVar);

						// skip i=c
						if ( j != zeroVar )
							// combine i and j into the same set
							unionSets(i, j);
					}
				}
			}
			// not an assignment
			else
				if ( AstInterface::IsFunctionCall( ast_node, &fct, &args ) )
				{
					SgFunctionCallExp *exp = isSgFunctionCallExp( ast_node );
					if ( exp )
					{
						SgNode *f = exp->get_function();
						// if this is an assert condition 
						if (f->unparseToString() == "assertCond")
						{
							SgExprListExp *argexp = exp->get_args();
							if (argexp)
							{
								SgExpressionPtrList l = argexp->get_expressions();
								ROSE_ASSERT ( l.size() == 1 );
								quad x, y, c;
								// union all the variables used in the condition into the same set
								if ( CFGUtils::computeTermsOfIfCondition( isSgExpression(*(l.begin())),
																											&x, &y, &c ) )
								{
#ifdef DEBUG_FLAG1_INI
cout << vars[x] << " and " << vars[y] << " used in same conditional. Merging...\n";
#endif
									ROSE_ASSERT ( existsVariable( x, vars ) &&
									              existsVariable( y, vars ) );
									unionSets(x, y);
								}
							}
						}
					}
					/*
					Rose_STL_Container<SgNode *> vars = NodeQuery::querySubTree( *(args.begin()), V_SgVarRefExp );
					Rose_STL_Container<SgNode *> dots = NodeQuery::querySubTree( *(args.begin()), V_SgDotExp );
					vars.insert( vars.begin(), dots.begin(), dots.end() );
					for ( Rose_STL_Container<SgNode *>::iterator it = vars.begin(); it != vars.end(); it++ )
					{
						ROSE_ASSERT ( isTypeConsidered( isSgExpression( *it ) ) );
						quad vr = getRefOfTypeConsidered( *it );
						if ( existsVariable( vr ) )
							unionSets(SP, vr );
					}
					* /
				}
		}
	}

	// if there were any array read accesses using complex expressions, the later analysis
	// will not be able to understand them, which will make it unable to correctly identify
	// the region of the array that is being read. Thus, the analysis may erroneously
	// produce live regions that are too narrow. If so, Yell.
	if ( undefinedRd )
	{
		cout << "Error! Program reads to our variable from an undefined index!!!\n";
		return NULL;
	}

	// We don't need to yell if we can't process some write indexes since this will
	// prevent the analysis from killing some arrays but this at least is conservative.

/*	// there are no understandable array accesses to any variables in varsConsidered
	if ( xvar.size() == 0 )
	{
		cout << "Warning! Program never reads/writes to our arrays from an i + c index!!!\n";
		return NULL;
	}

	// union together the equivalence classes of all the index variables that were used in 
	// array accesses, together with zeroVar into a single equivalence class
	for ( varIDSet::iterator it1=xvar.begin(); 
		   it1 != xvar.end(); it1++)
		unionSets(*it1, zeroVar);
*/
	// return the equivalence class of zeroVar
	return *var2Set[zeroVar];
}

// prints out the contents of all sets
void
UnionFind::showAllSets()
{
	int i=0;
	for(iterator it = this->begin(); it!=this->end(); it++, i++)
	{
		printf("set %d: ", i);
		showSet(it->second);
	}
}


// prints out the ids of all the variables in vars along with the associated
// equivalence class, as discovered by UnionFind
void
UnionFind::showSet(varIDSet &vars)
{
	for(varIDSet::iterator it = vars.begin(); it != vars.end(); it++)
	{
		varIDSet *s = var2Set[*it];
		ROSE_ASSERT (s);
		cout << "Element: " << ((varID)(*it)).str() << " has set with " << s->size()
		     << " members: ";
		for(varIDSet::iterator it2 = s->begin(); it2 != s->end(); it2++)
			cout << "\t" << ((varID)*it2).str();
		cout << "\n";
	}
}

// prints out the names of the variables in a given set
void
UnionFind::showSet( varIDSet* s )
{
	ROSE_ASSERT (s);
	cout << "Set size " << s->size() << " has elements:";
	for ( varIDSet::iterator it1 = s->begin(); it1 != s->end(); it1++ )
		cout << "\t" << ((varID)*it1).str();
	cout << "\n";
}

// prints out the names of the variables in a given variable's equivalence class
void
UnionFind::showSet( varID var )
{
	ROSE_ASSERT (existsSet( var ));
	showSet( var2Set[var] );
}

// returns whether the given variable has an associated equivalence class.
// it should only be possible for a variable to not have an equivalence class
// if that variable is not in vars.
bool
UnionFind::existsSet( varID var )
{
	return ( var2Set.find( var ) != var2Set.end() );
}

// unify the equivalence classes of variables of var1 and var2 and have the 
// var2Set mappings for each variable point to this equivalence
void
UnionFind::unionSets(varID var1, varID var2 )
{
	varIDSet *sA, *sB;
	
	// if the two variables are the same, do nothing
	if ( var1 == var2 )
		return;
	
	// if either variable has no associated equivalence class, Yell
	if ( !( existsSet( var1 ) && existsSet( var2 ) ) )
	{
		cout << "Error! Var1 = " << var1.str() << "\tVar2 = " << var2.str() << "\n";
		ROSE_ASSERT (0);
	}
	
	// Place the union into the set of the minimal element
	
	// make sure that var1 contains the minimal element
	if ( var1 < var2 )
	{
		sA = var2Set[var1];
		sB = var2Set[var2];
	}
	else
	{
		sB = var2Set[var1];
		sA = var2Set[var2];
	}		
	// if the two variables already belong to the same equivalence class, do nothing
	if (sA == sB)
		return;
	
	// copy all the variables from the sB equivalence class to the sA equivalence class
	// and link all the variables in the sB class to the sA class
	for (varIDSet::iterator it = sB->begin(); it != sB->end(); it++)
	{
		var2Set[*it] = sA;
		sA->insert(*it);
	}
	
	// deallocate the sB equivalence class
	delete (sB);
}

/***************************
 *** UnionFind::iterator ***
 ***************************/

bool UnionFind::iterator::operator ==(const iterator& that)
{
	return (it == that.it);
}

bool UnionFind::iterator::operator !=(const iterator& that)
{
	return !(*this == that);
}
		
void UnionFind::iterator::operator ++(int)
{
	do
	{
		it++;
		// if the current set has not been seen before
		if(seenSets.find(it->second)==seenSets.end())
		{
			recordCurSet();
			return;
		}
	}
	while(it!=tgt->end());
	// we reached the end of the map without seeing another set
}

UnionFind::iterator_ptr* UnionFind::iterator::operator ->()
{
	return &firstsecond;
}

// record the fact that we've seen the current set
void UnionFind::iterator::recordCurSet()
{
	seenSets.insert(it->second);
	if(it!=tgt->end())
	{
		firstsecond.first  = it->first;
		firstsecond.second = it->second;
	}
}

UnionFind::iterator UnionFind::begin()
{
	iterator it(var2Set, true);
	return it;
}

UnionFind::iterator UnionFind::end()
{
	iterator it(var2Set, false);
	return it;
}
