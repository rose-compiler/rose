/*
 * This is a program implemented for testing purposes.
 *	All new features are tested here.
 *
 *	Author : Athanasios Konstantinidis
 *	Date : 12-Jan-2012
 *	Lawrence Livermore National Laboratory
 */

#include <rose.h>

using namespace std;

#include <rosepoly/RosePollyInterface.h>

int main(int argc, char * argv[]) {
	
	SgProject * proj = frontend(argc,argv);
	
	vector<RosePollyModel*> kernels = RosePollyBuildModel(proj);
	
	for ( int i = 0 ; i < kernels.size() ; i++ ) {
		
		RosePluto * pluto = RosePollyBuildPluto(kernels[i]);
		pluto->apply(SMART_FUSE);
		pluto->loop_skewing(3,false);
		pluto->print(2);
		
		RoseCloog * cloog = RosePollyBuildCloog(pluto);
		CloogOptions * cl_options = RoseCloog::init_default_options();
		cloog->apply( cl_options );
		
		// cloog->print_to_file("cloogOUT.cloog");
		cloog->print_to_screen();
		/* FlowGraph * graph = cloog->print_to_flow_graph();
		graph->print(2); */
	}
	RosePollyTerminate();
	
	proj->unparse();
	
	return 0;
}
