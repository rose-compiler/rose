#include "main-vis.h"

bool get_universal_attribute__kill_norm_temps()
{
    return true;
}

void my_VarSet_print_fp(FILE * fp, o_VarSet node);

void dfi_write_(FILE * fp, KFG g, char *name, char *attrib, o_dfi info,int id,int insnum,int ctx)
{
  int show_summarygraph = 0; //shows the combined shape graph a la Sagiv/Reps/William (0: no summary graph)
  int show_shapegraphs  = 1; //shows the shape graphs (0: no shape graphs)
  int fold_shapegraphs  = 0; //fold the shapegraphs (0: display them all initially)

	/* We have only one instruction per basic block ! */
	assert((id==-1 && insnum==-1) || insnum==0);
	if (o_dfi_istop(info) || o_dfi_isbottom(info))
  {
		fprintf(fp, "node: { /*single instruction*/\n");
		fprintf(fp, "  title: \"%s\"\n", name);
		fprintf(fp, "  label: \"%s\"\n", o_dfi_istop(info)?GDL_TOP:GDL_BOT);
		fprintf(fp, "}\n\n");
		return;
	}

	fprintf(fp, "graph: { /*carrier box*/\n");
	fprintf(fp, "  title: \"%s\"\n", name);
	fprintf(fp, "  label: \"\"\n");
  
	//fprintf(fp, "status: grey\n");
  if (fold_shapegraphs)
    fprintf(fp, "  status: folded\n");
  else
    fprintf(fp, "  status: boxed\n");
  
	fprintf(fp, "  orientation: left_to_right\n");
	fprintf(fp, "  color: white\n");
	fprintf(fp, "  display_edge_labels: yes\n");
	fprintf(fp, "  manhattan_edges: no\n");
	fprintf(fp, "  ignore_singles : no\n");
	fprintf(fp, "  xspace: 50\n");
	fprintf(fp, "  %s\n", attrib);
  
	if (!o_dfi_istop(info) && !o_dfi_isbottom(info))
  {
    // *) these lines need adaption when switching between srw and nnh analyses
    o_ShapeGraphSet carrier = o_dfi_drop(info);  //*
    o_ShapeGraphList graphs = o_emptylist();
    int n_graphs = 0;
    int n_nodes = 0;

    if (show_shapegraphs) 
    {
      graphs = o_ShapeGraphList_conc(o_nnh_extract_shapeGraphs(carrier), graphs);  //*
    }

    if (show_summarygraph)
    {
      graphs = o_ShapeGraphList_conc(o_nnh_extract_summaryGraph(carrier), graphs); //*
    }

    // all graphs
    while (!o_ShapeGraphList_is_empty(graphs)) 
    {
      o_ShapeGraph sg = o_ShapeGraphList_head(graphs);
      graphs = o_ShapeGraphList_tail(graphs);
      n_graphs++;

      o_NodeList        is_shared = o_extract_is_shared(sg);
      o_StackEdgeList stack_edges = o_extract_stackedges(sg);
      o_HeapEdgeList   heap_edges = o_extract_heapedges(sg);

      o_NodeList            nodes = o_extract_heapnodes(sg);
      o_StrList              vars = o_vars_by_stackedges(o_extract_stackedge_set(sg));

      if (show_summarygraph && n_graphs == 1 && !o_ShapeGraphList_is_empty(graphs))
      {
        fprintf(fp, "  graph: { /*summary graph*/\n");
        fprintf(fp, "    color: green\n");
      }
      else
      {
        fprintf(fp, "  graph: { /*shape graph*/\n");
        fprintf(fp, "    color: white\n");
      }
      fprintf(fp, "    title: \"%s-sg%d\"\n", name, n_graphs);
      fprintf(fp, "    label: \"\"\n");
      fprintf(fp, "    orientation: left_to_right\n");
      fprintf(fp, "    status: grey\n");
      fprintf(fp, "    display_edge_labels: yes\n");
      fprintf(fp, "    manhattan_edges: no\n");
      fprintf(fp, "    ignore_singles : no\n");
      fprintf(fp, "    xspace: 50\n");
      fprintf(fp, "    %s\n", attrib);

      // all heap nodes
      while (!o_NodeList_is_empty(nodes)) {
        o_VarSet node = o_NodeList_head(nodes);

        nodes = o_NodeList_tail(nodes);
        n_nodes++;
        fprintf(fp, "    node: { /*heap node*/ \n");
        fprintf(fp, "      title: \"%s-sg%d", name, n_graphs);
        o_VarSet_print_fp(fp, node);
        fprintf(fp, "\"\n");
        fprintf(fp, "      label: \"");
        my_VarSet_print_fp(fp, node);
        fprintf(fp, "\"\n");
        if (o_NodeList_is_elem(is_shared, node))
        {
          fprintf(fp, "      shape: rhomb\n");
          fprintf(fp, "      color: red\n");
        }
        fprintf(fp, "    }\n\n");
      }

      // all var nodes
      while (!o_StrList_is_empty(vars)) {
        str            node = o_StrList_head(vars);

        vars = o_StrList_tail(vars);
        fprintf(fp, "    node: { /*var node*/ \n");
        fprintf(fp, "      title: \"%s-sg%d", name, n_graphs);
        str_print_fp(fp, node);
        fprintf(fp, "\"\n      label: \"");
        /* snum_print_fp(fp, node); */
                          //if (node < 0) 
        //fprintf(fp, "@_%s\"\n", getstr(-1*node));
        //else fprintf(fp, "%s\"\n", getstr(node));
        fprintf(fp, "%s\"\n", str_to_charp(node));
        fprintf(fp, "      shape: ellipse\n");
        fprintf(fp, "      level: 0\n");
        fprintf(fp, "    }\n\n");
      }

      // all stack edges
      while (!o_StackEdgeList_is_empty(stack_edges)) {
        o_StackEdge edge = o_StackEdgeList_head(stack_edges);
        str          src = o_StackEdge_select_1(edge);
        o_VarSet    dest = o_StackEdge_select_2(edge);

        stack_edges = o_StackEdgeList_tail(stack_edges);
        fprintf(fp, "    edge: { /*stackedge*/\n");
        fprintf(fp, "      sourcename: \"%s-sg%d", name, n_graphs);
        str_print_fp(fp, src);
        fprintf(fp, "\"\n");
        fprintf(fp, "      targetname: \"%s-sg%d", name, n_graphs);
        o_VarSet_print_fp(fp, dest);
        fprintf(fp, "\"\n");
        fprintf(fp, "    }\n\n");
      }

      // all heap edges
      while (!o_HeapEdgeList_is_empty(heap_edges)) {
        o_HeapEdge edge = o_HeapEdgeList_head(heap_edges);
        o_VarSet  node1 = o_HeapEdge_select_1(edge);
        str         sel = o_HeapEdge_select_2(edge);
        o_VarSet  node2 = o_HeapEdge_select_3(edge);

        heap_edges = o_HeapEdgeList_tail(heap_edges);
        fprintf(fp, "    edge: { /*heapedge*/\n");
        fprintf(fp, "      sourcename: \"%s-sg%d", name, n_graphs);
        o_VarSet_print_fp(fp, node1);
        fprintf(fp, "\"\n");
        fprintf(fp, "      targetname: \"%s-sg%d", name, n_graphs);
        o_VarSet_print_fp(fp, node2);
        fprintf(fp, "\"\n");
        fprintf(fp, "      label: \"%s\"\n", str_to_charp(sel));
        fprintf(fp, "    }\n\n");
      }
      
      // end one graph
      fprintf(fp, "  }\n\n");
    }

    if (!n_nodes) {
      fprintf(fp, "  node: { /*no nodes*/ \n");
      fprintf(fp, "    title: \"%s-sg%d_dummy\"\n", name, n_graphs);
      fprintf(fp, "    label: \"\"\n");
      fprintf(fp, "    width : 0\n");
      fprintf(fp, "    height: 0\n");
      fprintf(fp, "  }\n");
    }
  }
  

	fprintf(fp, "}\n\n\n\n");
	return;
}

void my_VarSet_print_fp(FILE * fp, o_VarSet node) {
  _o_VarSet_cur cur;

  fprintf(fp,"{");
  o_VarSet_cur_reset(&cur,node);

  while (!o_VarSet_cur_is_empty(&cur)) {
    char* var = str_to_charp(o_VarSet_cur_get(&cur)); // MS adapted int to char*
    o_VarSet_cur_next(&cur);

    if (var!=0)
      fprintf(fp,"%s",var); // MS adapted to char*
    else
      fprintf(fp,"NOVAR");

    if(!o_VarSet_cur_is_empty(&cur))
      fprintf(fp,",");
  }
  fprintf(fp,"}");
}



static void print_statement_attributes(DFI_STORE, SgStatement *);

void AliasTraversal::visit(SgNode *node)
{
    if (isSgStatement(node))
    {
        SgStatement *stmt = isSgStatement(node);
        if (isSgBasicBlock(stmt->get_parent()))
        {
            std::string stmt_str = stmt->unparseToString();
            std::cout << stmt_str << std::endl;
            print_statement_attributes(store, stmt);
        }
    }
}

extern void *get_statement_pre_info(DFI_STORE store, SgStatement *stmt);
extern void *get_statement_post_info(DFI_STORE store, SgStatement *stmt);

static void print_statement_attributes(DFI_STORE store, SgStatement *stmt)
{
    StatementAttribute *end
        = (StatementAttribute *) stmt->getAttribute("PAG statement end");
    BasicBlock *endb = end->get_bb();
    enum pos eb = end->get_pos();
    
    if (endb != NULL)
    {
        // kfg_node_infolabel_print_fp(stdout, NULL, endb, 0);
        std::cout << o_test_alias((carrier_type_o(CARRIER_TYPE))get_statement_post_info(store, stmt));
        
        /*
        std::cout << "is_may_alias(" << exprA << ", " << exprB << ") = ";
        std::cout << o_is_may_alias(exprA, exprB, (carrier_type_o(CARRIER_TYPE))get_statement_post_info(store, stmt)) << std::endl;
        
        std::cout << "is_must_alias(" << exprA << ", " << exprB << ") = ";
        std::cout << o_is_must_alias(exprA, exprB, (carrier_type_o(CARRIER_TYPE))get_statement_post_info(store, stmt)) << std::endl;
        */
    }
    else
        std::cout << "0";
    std::cout << std::endl;
}
