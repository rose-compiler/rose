#ifndef UNION_FIND_H
#define UNION_FIND_H

#include "common.h"
#include "variables.h"
#include "cfgUtils.h"

// encapsulates the functionality of the UnionFind algorithm, which identifies the variables that
// are or are related to the loop index variables used to access the array that we're currently 
// analyzing
class UnionFind
{
 public:
	// the map from variable ids to their associated equivalence classes as discovered
	// by UnionFind
	map<varID, varIDSet *> var2Set;
	
	UnionFind(varIDSet &vars)
	{
		// initializes the equivalence classes of all variables in vars s.t. each
		// variable's equivalence class contains only that variable.
		for (varIDSet::iterator it = vars.begin(); it != vars.end(); it++)
		{
			//printf("UnionFind::UnionFind() it=%s\n", (*it).str().c_str());
			varIDSet *new_set = new varIDSet();
			new_set->insert(*it);
			pair<varID, varIDSet *> p(*it, new_set);
			var2Set.insert(p);
		}
	}
	
	// unify the equivalence classes of variables of var1 and var2 and have the 
	// var2Set mappings for each variable point to this equivalence
	void unionSets( varID var1, varID var2 );

	// returns whether the given variable has an associated equivalence class.
	// it should only be possible for a variable to not have an equivalence class
	// if that variable is not in vars2Name.
	bool existsSet( varID var );
	
	// performs the Union Find algorithm to determine the set of variables that are
	// related to the variables in varsConsidered. The algorithm works as follows:
	// - looks over all statements in the program
	//   - for assignments (i=j+c) or conditions (i<=j+c): combines i and j into the 
	//       same set. this produces a set of equivalence classes of variables
	//   - for array accesses that touch variables in varsConsidered, adds their read
	//       and write array indexes and saves them to set xvar
	//   - unions the equivalence classes of all variables in xvar into a single 
	//       equivalence class and returns it.
	varIDSet determineInterestSet (const DataflowNode &graph, 
	                               varIDSet &varsConsidered/*,
	                               varIDSet &vars*/);

	// prints out the contents of all sets
	void showAllSets();

	// prints out the ids of all the variables in vars2Names along with the associated
	// equivalence class, as discovered by UnionFind
	void showSet( varIDSet &vars );

	// prints out the names of the variables in a given set
	void showSet( varIDSet* s );

	// prints out the names of the variables in a given variable's equivalence class
	void showSet( varID var );
	
	struct iterator_ptr {
		varID first;
		varIDSet* second;
	};
	
	class iterator
	{
		// target map of sets
		map<varID, varIDSet *> *tgt;
		// the current pointer into this map
		map<varID, varIDSet *>::iterator it;
		// the ->first, ->second pair that we need this iterator to have
		iterator_ptr firstsecond;
		// a set of varIDSets that have already been seen during this iteration
		set<varIDSet *> seenSets;
					
		public:
		// begin flag indicates whether this is to be tgt's begin iterator (=true) or end iterator (=false)
		iterator(map<varID, varIDSet *> &tgt, bool begin)
		{
			this->tgt = &tgt;
			if(begin)
				it = this->tgt->begin();
			else
				it = this->tgt->end();
			recordCurSet();
		}
			
		bool operator ==(const iterator& that);
		bool operator !=(const iterator& that);
		void operator ++(int);
		iterator_ptr* operator ->();
		
		protected:
		void recordCurSet();
	};
	iterator begin();
	iterator end();
};

#endif
