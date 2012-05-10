
/*
 *  This file defines the Kernel interface.
 *	Author : Athanasios Konstantinidis
 *	Data :	12-Jan-2012
 *	Lawrenve Livermore National Laboratory
 */

#ifndef KERNEL_H
#define KERNEL_H

#include <rosepoly/data.h>

class FlowGraph;

/*
 *  This class represent a specific program block
 *	of interest. It only contains information related
 *	with the possition of this block in the program
 *	as well as some naming/symbolic information related
 *	to the local and global data accessed inside
 *	the block.
 */
class Kernel {
	
	int ID;
	
	SgBasicBlock *  scope;
	FlowGraph *	    myFlow;
	symbol_table  Data;
	vector<string>  Parameters;
	
	/* Flow Graph construction operations */
	void init_data();
	void set_graph();
	FlowGraph * set_graph_recursive( SgStatement * entry );
	void parameter_scan();
	void add_Datum( SgVariableSymbol * varSym, IO anIO=NOVALUE );
	
public:
	
	Kernel( int i, SgBasicBlock * sc );
	~Kernel();
	
	FlowGraph * get_flow_graph() const;
	int get_ID()				 const;
	const symbol_table& get_data()	 const;
	vector<string> get_params()	 const;
	SgBasicBlock * get_scope()   const;
	
};

#endif

