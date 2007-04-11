#include "main.h"

// GRAPH OUTPUT
#include <stdio.h>
//#include <str.h>
#include <assert.h>

#include "iface.h"
#include "iterate.h"
#include "mapping.h"
#include "shape-out/src/o_dfi.h"

#include <string>
#include <vector>
#include <algorithm>

void my_o_VarSet_print_fp(FILE * fp, o_VarSet node);

// declared in iterate.h
//extern "C" void *doit(ANALYSIS)(void *);

extern "C" void gdl_create(char *, int);
static int      edge = 0; // global variable for setting edges on/off

int main(int argc, char **argv)
{

  /* parse the command line and extract analyzer options */
  CommandLineParser clp;
  AnalyzerOptions opt=clp.parse(argc,argv);

  /* set the PAG options as specified on the command line */
  setPagOptions(opt);
  
  /* Run the frontend to construct an abstract syntax tree from
   * the files specified on the command line (which has been processed
   * and commands only relevant to the analyzer have been removed). */
  SgProject* ast_root = frontend(opt.getCommandLineNum(), opt.getCommandLineCarray());
  
  /* Make sure everything is OK... */
  AstTests::runAllTests(ast_root);

  /* Construct a control-flow graph from the AST, make sure it has
   * the structrure expected by PAG, and run the analysis on it,
   * attributing the statements of the AST with analysis
   * information. Use the StatementAttributeTraversal class for accessing
   * the analysis information on each statement */
  char* outputfile=(char*)opt.getGdlFileName().c_str();
  DFI_STORE analysis_info = perform_pag_analysis(ANALYSIS)(ast_root,outputfile,!opt.animationGeneration());

  /* Handle command line option --textoutput */
  if(opt.analysisResultsTextOutput()) {
    PagDfiTextPrinter<DFI_STORE> p(analysis_info);
    p.traverseInputFiles(ast_root, preorder);
  }
  
  /* Handle command line option --sourceoutput 
   * The source code (i.e. the AST) is annotated with comments showing
   * the analysis results and by calling the backend an annotated C/C++
   * file is generated (named rose_<inputfilename>) */
  if(opt.analysisResultsSourceOutput()) {
    PagDfiCommentAnnotator<DFI_STORE> ca(analysis_info);
    ca.traverseInputFiles(ast_root, preorder);
    ast_root->unparse();
  }

  /* Free all memory allocated by the PAG garbage collection */
  GC_finish();
  
  return 0;

  /*
  debug_stat=1; // 1 or 2 for debugging, 2 means more detailed stats
  verbose=0; // prints PAG-info during analysis (see section 14.3.3.1 in User Manual)

  animation="anim-out";
    SgProject *root = frontend(argc, argv);
    AstTests::runAllTests(root);
    std::cout << "collecting functions... ";
    ProcTraversal s;
    s.traverseInputFiles(root, preorder);
    std::cout << "done" << std::endl;

    std::cout << "generating cfg... ";
    CFGTraversal t(s.get_procedures());
    t.traverseInputFiles(root, preorder);
    std::cout << "done" << std::endl;

    std::cout << "testing cfg... " << std::endl;
    int test_result = kfg_testit(t.getCFG(), 0, "cfg_dump.gdl");
    if (test_result == 0)
    {
        std::cout << std::endl
            << "Warning: There are problems in the CFG."
            << std::endl
            << "Do not rely on the analysis results." << std::endl;
    }
    else
        std::cout << "no problems found" << std::endl;

    std::cout << "performing analysis " str(ANALYSIS) "... ";
    doit(ANALYSIS)(t.getCFG());
    std::cout << "done" << std::endl;
    std::cout << "generating visualization... ";
    gdl_create(str(ANALYSIS) "_result.gdl", 0);
    std::cout << "done" << std::endl;

    return 0;
  */
}

#ifdef DFI_WRITE
void dfi_write_(FILE * fp, KFG g, char *name, char *attrib, o_dfi info,int id,int insnum,int ctx) {
	int result = 0;

	//printf("dfi_write: name:%s, attrib:%s id:%d insum:%d ctx:%d.\n",name,attrib,id,insnum,ctx);
	// id ist die KnotenID
	//printf("name:%s, attrib:%s\n",name,attrib);
	

	/* We have only one instruction per basic block ! */
	assert((id==-1 && insnum==-1) || insnum==0);
	if (o_dfi_istop(info) || o_dfi_isbottom(info)) {
		fprintf(fp,"node: {\n");
		fprintf(fp, "title: \"%s\"\n", name);
		fprintf(fp, "label: \"%s\"\n", o_dfi_istop(info)?GDL_TOP:GDL_BOT);
		fprintf(fp,"}\n\n");
		return;
	}

	fprintf(fp, "graph: {\n");
	fprintf(fp, "title: \"%s\"\n", name);
	fprintf(fp, "label: \"\"\n");
	fprintf(fp, "orientation : left_to_right\n");
	fprintf(fp, "color       : white\n");
	fprintf(fp, "status      : grey\n");
	fprintf(fp, "display_edge_labels: yes\n");
	fprintf(fp, "manhattan_edges: no\n");
	fprintf(fp, "ignore_singles : no\n");
	fprintf(fp, "xspace: 50\n");
	fprintf(fp, "%s\n", attrib);
	if (!o_dfi_istop(info) && !o_dfi_isbottom(info)) {
		o_dfii          flow = o_dfi_drop(info);
		o_graph         graph = o_dfii_select_1(flow);
		o_printtype        share = o_getlist3(o_dfii_select_2(flow));
		o_duo_l1         exter = o_getlist1(graph);
		o_TripleList      inter = o_getlist2(graph);
		o_printtype        nodes;
		//o_snuml         extn = o_extract4(exter);
		o_strl         extn = o_extract4(exter);

		nodes = o_extract_nodes(graph);

		while (!o_printtype_is_empty(nodes)) {
			o_VarSet          node = o_printtype_head(nodes);

			nodes = o_printtype_tail(nodes);
			result++;
			fprintf(fp, "node:{\n");
			fprintf(fp, "title: \"%s", name);
			my_o_VarSet_print_fp(fp, node);
			fprintf(fp, "\"\n");
			fprintf(fp, "label: \"");
			my_o_VarSet_print_fp(fp, node);
			fprintf(fp, "\"\n");
			if (o_printtype_is_elem(share, node)) {
                   	fprintf(fp, "shape:rhomb\n");
				if (!edge) {
			if (o_VarSet_is_empty(node)) 
                   		fprintf(fp, "color:darkmagenta\n");
                   		else fprintf(fp, "color:red\n");
                        }}
			fprintf(fp, "}\n\n");
		}

		while (!o_strl_is_empty(extn)) {
			str            node = o_strl_head(extn);

			extn = o_strl_tail(extn);
			fprintf(fp, "node:{\n");
			fprintf(fp, "title: \"%s", name);
			str_print_fp(fp, node);
			fprintf(fp, "\"\nlabel: \"");
			/* snum_print_fp(fp, node); */
                        //if (node < 0) 
			//fprintf(fp, "@_%s\"\n", getstr(-1*node));
			//else fprintf(fp, "%s\"\n", getstr(node));
			fprintf(fp, "%s\"\n", str_to_charp(node));
			fprintf(fp, "shape:ellipse\n");
			fprintf(fp, "level:0\n");
			fprintf(fp, "}\n\n");
		}

		while (!o_duo_l1_is_empty(exter)) {
			o_duo           edge = o_duo_l1_head(exter);
			str             var = o_duo_select_1(edge);
			o_VarSet          vars = o_duo_select_2(edge);

			exter = o_duo_l1_tail(exter);
			fprintf(fp, "edge:{\n");
			fprintf(fp, "sourcename: \"%s", name);
			str_print_fp(fp, var);
			fprintf(fp, "\"\n");
			fprintf(fp, "targetname: \"%s", name);
			my_o_VarSet_print_fp(fp, vars);
			fprintf(fp, "\"\n");
			fprintf(fp, "}\n\n");
		}

		while (!o_TripleList_is_empty(inter)) {
			o_triple        edge = o_TripleList_head(inter);
			o_VarSet          node1 = o_triple_select_1(edge);
			str             var = o_triple_select_2(edge);
			o_VarSet          node2 = o_triple_select_3(edge);

			inter = o_TripleList_tail(inter);
			fprintf(fp, "edge:{\n");
			fprintf(fp, "sourcename: \"%s", name);
			my_o_VarSet_print_fp(fp, node1);
			fprintf(fp, "\"\n");
			fprintf(fp, "targetname: \"%s", name);
			my_o_VarSet_print_fp(fp, node2);
			fprintf(fp, "\"\n");
			fprintf(fp, "label: \"");
			/* snum_print_fp(fp, var); */
			//	if (var < 0) 
			//fprintf(fp, "%s\"\n", getstr(-1*var));
			//else fprintf(fp, "%s\"\n", getstr(var));
			fprintf(fp, "%s\"\n", str_to_charp(var));
			fprintf(fp, "}\n\n");
		}
	}
    if (!result) {
        fprintf(fp, "node:{\n");
        fprintf(fp, "title: \"%s_dummy\"\n", name);
        fprintf(fp, "label: \"\"\n");
        fprintf(fp, "width : 0\n");
        fprintf(fp, "height: 0\n");
        fprintf(fp, "     }\n");
    }

	fprintf(fp, "}\n\n");
	return;
}

 
void my_o_VarSet_print_fp(FILE * fp, o_VarSet node) {
	_o_VarSet_cur cur;

	std::vector<std::string> varSet;
	o_VarSet_cur_reset(&cur,node);
	while(!o_VarSet_cur_is_empty(&cur)) {
	        char* var = str_to_charp(o_VarSet_cur_get(&cur)); // MS adapted int to char*
		o_VarSet_cur_next(&cur);
		//if (var!=0) fprintf(fp,"%s",var); // MS adapted to char*
		//else fprintf(fp,"NOVAR");
		//if(!o_VarSet_cur_is_empty(&cur))
		//fprintf(fp,",");
		if (var!=0)
		  varSet.push_back(var);
		else
		  varSet.push_back("NOVAR");
	}
	// we need to sort the set of variable names to ensure that the same string is generated for the same set
	sort(varSet.begin(),varSet.end());
	fprintf(fp,"{");
	for(std::vector<std::string>::iterator i=varSet.begin();i!=varSet.end(); i++) {
	  if(i!=varSet.begin())
	    fprintf(fp,",");
	  std::string varName=*i;
	  const char* c=varName.c_str();
	  fprintf(fp,c);

	}
	fprintf(fp,"}");
}
#endif
