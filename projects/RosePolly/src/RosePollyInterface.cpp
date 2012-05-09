
/*
 *  Definitions of the Pollyhedral API methods.
 *	Author : Athanasios Konstantinidis
 *	Data : 12-Jan-2012
 *	Lawrence Livermore National Laboratory
 */


#include <rose.h>
#include <sageInterface.h>

using namespace std;
using namespace SageInterface;

#include <rosepoly/RosePollyInterface.h>
#include <rosepoly/traversals.h>
#include <rosepoly/error.h>

SgGlobal * GlobalScope;

vector<RosePollyModel*> live_objects;

map<int,Kernel*> kernel_map;

vector<Kernel*> findRosePollyCandidates( SgNode * root, search_type t )
{
	vector<Kernel*> list;
	
	vector<SgNode*> pragmas = NodeQuery::querySubTree(root, V_SgPragma);
	
	for ( int i = 0 ; i < pragmas.size() ; i++ ) {
		
		string pragmaName = isSgPragma(pragmas[i])->get_pragma();
		
		if ( pragmaName == annotName ) {
			
			SgPragmaDeclaration * pr_decl = isSgPragmaDeclaration(pragmas[i]->get_parent());
			SgStatement * block = getNextStatement(pr_decl);
			
			if ( isSgBasicBlock(block) != NULL ) {
				list.push_back(new Kernel(i,isSgBasicBlock(block)) );
			} else {
				block = getNextStatement(block);
				if ( isSgBasicBlock(block) != NULL )
					list.push_back(new Kernel(i,isSgBasicBlock(block)) );
			}
		}
	}
	
	return list;
}

bool RosePollyEvaluate( Kernel * k, RosePollyCustom * c )
{
	cout<<"  [Evaluating Kernel against the Pollyhedral constraints] . . . ";
	PollyEvaluateTraversal pollyEval(c);
	pollyEval.traverse( k->get_flow_graph()->get_head() );
	
	return pollyEval.result; 
}

void RosePollyDependenceAnalysis( vector<affineStatement*>& stmts )
{
	DependenceTraversal dep_trav;
	for ( int i = 0 ; i < stmts.size() ; i++ ) {
		
		int read_size = stmts[i]->get_num_reads();
		int write_size = stmts[i]->get_num_writes();
		
		/* STEP 1 : True dependencies (W_R) */
		dep_trav.init(stmts[i],W_R);
		// cout<<"[Finding True-Dependencies for STM["<<stmts[i]->get_ID()<<"] ]"<<endl;
		for ( int j = 0 ; j < read_size ; j++ ) {
			if ( stmts[i]->has_valid_read_map(j) ) {
				dep_trav.set_ref(j);
				// cout<<"-calling the traversal for ref("<<i<<")"<<endl;
				dep_trav.traverse(stmts[i]->next(BACKWARD));
			}
		}
		
		/* STEP 2 : Anti dependencies (R_W) */
		dep_trav.init(stmts[i],R_W);
		// cout<<"[Finding Anti-Dependencies for STM["<<stmts[i]->get_ID()<<"] ]"<<endl;
		for ( int j = 0 ; j < write_size ; j++ ) {
			if ( stmts[i]->has_valid_write_map(j) ) {
				dep_trav.set_ref(j);
				// cout<<"-calling the traversal for ref("<<i<<")"<<endl;
				dep_trav.traverse(stmts[i]->next(BACKWARD));
			}
		}
		
		/* STEP 3 : Output dependencies (W_W) */
		dep_trav.init(stmts[i],W_W);
		for ( int j = 0 ; j < write_size ; j++ ) {
			if ( stmts[i]->has_valid_write_map(j) ) {
				dep_trav.set_ref(j);
				dep_trav.traverse(stmts[i]->next(BACKWARD));
			}
		}
		
		/* STEP 4 : Input dependencies (R_R) */
		/* dep_trav.init(stmts[i],R_R);
		for ( int j = 0 ; j < read_size ; j++ ) {
			if ( stmts[i]->has_valid_read_map(j) ) {
				dep_trav.set_ref(j);
				dep_trav.traverse(stmts[i]->next(BACKWARD));
			}
		} */
	}
}

RosePollyModel * RosePollyBuildModel( FlowGraph * graph, RosePollyCustom * c )
{
	Kernel * k = kernel_map[graph->get_id()];
	PollyModelExtractionTraversal pollyExtr( k->get_params(), c );
	pollyExtr.traverse( graph->get_head() );
	pollyExtr.consolidate();
	
	// I use a vector of stmts for efficiency ... don't forget to comment on that
	RosePollyDependenceAnalysis( pollyExtr.stmts );
	
	return new RosePollyModel( pollyExtr.stmts, k ); 
}

RosePollyModel * RosePollyBuildModel( FlowGraph * graph )
{
	defaultRosePollyCustom * c = new defaultRosePollyCustom();
	RosePollyModel * model = RosePollyBuildModel(graph,c);
	delete(c);
	return model;
}

vector<RosePollyModel*> RosePollyBuildModel( SgNode * root, search_type t )
{
	defaultRosePollyCustom * c = new defaultRosePollyCustom();
	vector<RosePollyModel*> buffer = RosePollyBuildModel( root, c, t );
	delete(c);
	return buffer;
}

vector<RosePollyModel*> RosePollyBuildModel( SgNode * root, RosePollyCustom * c, search_type t )
{
	vector<RosePollyModel*> buffer;
	
	cout<<endl<<"[Looking for Candidates (";
	if ( t == ANNOTATED )
		cout<<"annotated";
	else
		cout<<"auto";
	
	cout<<") . . . ]"<<endl;
	
	vector<Kernel*> cdts = findRosePollyCandidates(root,t);
	int found = cdts.size();
	
	cout<<"[Found("<<found<<") ]"<<endl<<endl;
	
	for ( int i = 0 ; i < found ; i++ ) {
		
		cout<<endl<<"[Building Polyhedral Model for Kernel(<<"<<cdts[i]->get_ID()<<">>)]"<<endl<<endl;
		c->set_params(cdts[i]->get_params());
		kernel_map[cdts[i]->get_ID()] = cdts[i];
		
		// STEP 1 : Evaluate kernel
		if ( !RosePollyEvaluate(cdts[i],c) ) {
			cout<<"FAIL !"<<endl;
			front_error( cdts[i]->get_scope(), EVAL, "Not a valid polyhedral kernel", false );
		} else {
			cout<<"PASS !"<<endl;
		}
		
		// STEP 2 : Build Polyhedral Model From Flow Graph
		RosePollyModel * model = RosePollyBuildModel( cdts[i]->get_flow_graph(), c );
		
		buffer.push_back(model);
		live_objects.push_back(model);
		
		cout<<endl<<"[FINISHED]"<<endl<<endl;
	}
	return buffer;
}

RosePluto * RosePollyBuildPluto( RosePollyModel * model )
{
	RosePluto * pluto = new RosePluto( *model );
	live_objects.push_back(pluto);
	return pluto;
}

RoseCloog * RosePollyBuildCloog( RosePollyModel * model )
{
	RoseCloog * cloog = new RoseCloog( *model );
	live_objects.push_back(cloog);
	return cloog;
}

void RosePollyTerminate()
{
	for ( int i = 0 ; i < live_objects.size() ; i++ )
		delete(live_objects[i]);
	
	map<int,Kernel*>::iterator it;
	
	for ( it = kernel_map.begin() ; it != kernel_map.end() ; it++ )
		delete((*it).second);
}


