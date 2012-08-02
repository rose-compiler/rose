/*
 *  Author : Athanasios Konstantinidis
 *	Date : 12-Jan-2012
 *	Lawrence Livermore National Laboratory
 */

#include <rose.h>

using namespace std;

#include <rosepoly/Kernel.h>
#include <rosepoly/FlowGraph.h>
#include <rosepoly/Utilities.h>
#include <rosepoly/error.h>

Kernel::Kernel( int i, SgBasicBlock * sc ) 
: ID(i), scope(sc), myFlow(NULL) 
{
	/* STEP 1 : Initialize data */
	init_data();
	
	/* STEP 2 : Construct the graph */
	set_graph();
	
	/* STEP 3 : Extract parameters */
	parameter_scan();
}

Kernel::~Kernel() 
{ 
	delete(myFlow); 
}

FlowGraph * Kernel::get_flow_graph() const { return myFlow; }

vector<string> Kernel::get_params() const { return Parameters; }

SgBasicBlock * Kernel::get_scope() const { return scope; }

const symbol_table& Kernel::get_data() const { return Data; }

int Kernel::get_ID() const { return ID; }

void Kernel::add_Datum( SgVariableSymbol * varSym, IO anIO )
{
	Type t = utilities::extract_scalar_type( varSym->get_type() );
	
	if ( t == NOTYPE )
		front_error(varSym->get_declaration(),DATA,"Data type NOT supported. Sorry !",true);
	
	if ( anIO == LOCAL && isSgPointerType(varSym->get_type()) )
		anIO = LOCAL_POINTER;
	
	Data[varSym->get_name().getString()] = Datum( varSym, t, anIO );
}

void Kernel::init_data()
{
	cout<<"    [Initializing Data]"<<endl;
	
	vector<SgNode*> nestedScopes = NodeQuery::querySubTree(scope,V_SgScopeStatement);
	vector<SgNode*> varRefs = NodeQuery::querySubTree(scope,V_SgVarRefExp);
	set<SgVariableSymbol*> table;
	SgVariableSymbol * temp;
	
	for ( int i = 0 ; i < varRefs.size() ; i++ ) {
		
		/* Check if symbol already exists */
		temp = isSgVarRefExp(varRefs[i])->get_symbol();
		if ( !table.insert(temp).second )
			continue;
		
		/* Local Data : Data declared according to C99 standard 
		 (only at beggining of block) */
		if ( scope->symbol_exists(temp) ) {
			add_Datum(temp,LOCAL);
			continue;
		}
		
		/* Induction Variables : Only induction variable declaration allowed
		 in nested scopes */
		int j;
		int tot_scopes = nestedScopes.size();
		for ( j = 0 ; j < tot_scopes ; j++ ) {
			if ( isSgForStatement(nestedScopes[j]) != NULL &&
				isSgForStatement(nestedScopes[j])->symbol_exists(temp) ) {
				add_Datum(temp,INDUCTION);
				break;
			}
		}
		
		/* Global data : The rest are simply global data */
		if ( j == tot_scopes )
			add_Datum(temp);
	}
}

void Kernel::set_graph()
{
	cout<<"    [Putting Graph Together]"<<endl;
	myFlow = set_graph_recursive(scope);
}

FlowGraph * Kernel::set_graph_recursive( SgStatement * entry )
{
	BasicNode * aHead = NULL;
	BasicNode * aTail = NULL;
	FlowGraph * aGraph = new FlowGraph( aHead, aTail, ID );
	if ( entry == NULL )
		return aGraph;
	static int stmCounter = 0;
	
	SgStatementPtrList stmList;
	if ( isSgBasicBlock(entry) != NULL )
		stmList = isSgBasicBlock(entry)->generateStatementList();
	else
		stmList.push_back( entry );
	
	Rose_STL_Container<SgStatement*>::iterator i;
	for ( i = stmList.begin() ; i != stmList.end() ; i++ )
	{
		if ( isSgExprStatement( *i ) != NULL ) {
			
			Statement * stm = new Statement(isSgExprStatement(*i),stmCounter);
			stm->set_patterns(Data);
			front_error(*i,ACCESSPAT,"Incompatible access pattern",false);
			aGraph->insertNode( stm );
			stmCounter++;
			
		} else if ( isSgForStatement( *i ) != NULL ) {
			
			ForLoop * loop = new ForLoop( isSgForStatement(*i) );
			front_error(*i,GRAPH,"Loop is too complecated. Sorry !",false);
			aGraph->insertNode( loop );
			FlowGraph * tempGraph = set_graph_recursive( isSgForStatement(*i)->get_loop_body() );
			tempGraph->is_temp();
			loop->set_body( tempGraph );
			delete(tempGraph);
			
		} else if ( isSgIfStmt( *i ) ) {
			
			SgIfStmt * cond = isSgIfStmt(*i);
			SgStatement * trueBody = cond->get_true_body();
			SgStatement * falseBody = cond->get_false_body();
			
			Conditional * c_node = new Conditional( isSgExprStatement(cond->get_conditional())->get_expression() );
			aGraph->insertNode( c_node );
			
			FlowGraph * tempGraph = set_graph_recursive(trueBody);
			tempGraph->is_temp();
			c_node->set_true( tempGraph );
			delete(tempGraph);
			tempGraph = set_graph_recursive(falseBody);
			tempGraph->is_temp();
			c_node->set_false( tempGraph );
			delete(tempGraph);
			
		} else if ( isSgBasicBlock(*i) ) {
			
			FlowGraph * tempGraph = set_graph_recursive(*i);
			aGraph->insertSubGraph( tempGraph );
			tempGraph->is_temp();
			delete(tempGraph);
			
		} else if ( isSgVariableDeclaration(*i) ) {
			
			SgInitializedNamePtrList nameList = isSgVariableDeclaration(*i)->get_variables();
			string name;
			Rose_STL_Container<SgInitializedName*>::iterator i;
			for ( i = nameList.begin() ; i != nameList.end() ; i++ )
			{
				name = (*i)->get_symbol_from_symbol_table()->get_name().getString();
				symbol_table::iterator it = Data.find(name);
				if ( it == Data.end() || it->second.get_IO() < LOCAL )
					front_error(*i,GRAPH,"Declaration Not Allowed here !",true);
			}
			
		} else {
			front_error(*i,GRAPH,"Unknown statement",true);
		}

	}
	
	return aGraph;
}

void Kernel::parameter_scan()
{
	cout<<"    [Scanning for Structure Parameters]"<<endl;
	symbol_table::iterator it;
	for ( it = Data.begin() ; it != Data.end() ; it++ ) {
		if ( it->second.is_parameter() ) {
			Parameters.push_back(it->first);
			it->second.set_IO(PARAMETER);
		}
	}
}




