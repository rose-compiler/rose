/* DFI enclosed in {} is replaced with o_ShapeGraphLifted or o_ShapeGraphSetLifted
 * for srw98 or nnh99, respectively.
 * this is done by the postprocess-main script.
 **/

extern ShapeAnalyzerOptions *opt;

// dfi_write_ creates the gdl visualisation for every basic block.
// PAG creates the visualisation layout and calls this function
// that provides the layout within every basic block

void dfi_write_(FILE * fp, KFG g, char *name, char *attrib, o_{DFI} info,int id,int insnum,int ctx) {
    // We have only one instruction per basic block !
    assert((id==-1 && insnum==-1) || insnum==0);
    if (o_{DFI}_istop(info) || o_{DFI}_isbottom(info)) {
        fprintf(fp, "node: { /*single instruction*/\n");
        fprintf(fp, "  title: \"%s\"\n", name);
        fprintf(fp, "  label: \"%s\"\n", o_{DFI}_istop(info)?GDL_TOP:GDL_BOT);
        fprintf(fp, "}\n\n");
        return;
    }

    fprintf(fp, "graph: { /*carrier box*/\n");
    fprintf(fp, "  title: \"%s\"\n", name);
    fprintf(fp, "  label: \"\"\n");
    
    fprintf(fp, "status: grey\n");
    if (opt->gdlFoldGraphs()) {
        fprintf(fp, "  status: folded\n");
    } else {
        fprintf(fp, "  status: boxed\n");
    }
    
    fprintf(fp, "  orientation: left_to_right\n");
    fprintf(fp, "  color: white\n");
    fprintf(fp, "  display_edge_labels: yes\n");
    fprintf(fp, "  manhattan_edges: no\n");
    fprintf(fp, "  ignore_singles : no\n");
    fprintf(fp, "  xspace: 50\n");
    fprintf(fp, "  %s\n", attrib);

    int n_nodes = 0;
    if (!o_{DFI}_istop(info) && !o_{DFI}_isbottom(info)) {
        int n_graphs = 0;

        o_SrwNnhPair gpair = o_extract_graphs(o_{DFI}_drop(info));
        
        if (opt->gdlShowSummaryGraph()) {
			      //gstats.addGraphs(1);
            fprintf(fp, "  graph: { /*summary graph*/\n");
            fprintf(fp, "    color: lightgrey\n");
            fprintf(fp, "    label: \"\"\n");
            n_graphs++;
            n_nodes += dfi_write_ShapeGraph_fp(fp, name, n_graphs, attrib, o_SrwNnhPair_select_1(gpair));
        }

        if (opt->gdlShowIndividualGraphs()) {
            o_ShapeGraphList graphs = o_SrwNnhPair_select_2(gpair);
            while (!o_ShapeGraphList_is_empty(graphs)) {
                o_ShapeGraph sg = o_ShapeGraphList_head(graphs);
                graphs = o_ShapeGraphList_tail(graphs);
                
                fprintf(fp, "  graph: { /*shape graph*/\n");
                fprintf(fp, "    color: white\n");
                fprintf(fp, "    label: \"\"\n");
                n_graphs++;
                n_nodes += dfi_write_ShapeGraph_fp(fp, name, n_graphs, attrib, sg);
            }
        }
    }

    if (!n_nodes) {
        fprintf(fp, "  node: { /*no nodes*/ \n");
        fprintf(fp, "    title: \"%s_dummy\"\n", name);
        fprintf(fp, "    label: \"\"\n");
        fprintf(fp, "    width : 0\n");
        fprintf(fp, "    height: 0\n");
        fprintf(fp, "  }\n");
    }

    fprintf(fp, "}\n\n\n\n");
    return;
}

int dfi_write_ShapeGraph_fp(FILE *fp, char *name, int n_graphs,  char *attrib, o_ShapeGraph sg) {
    int n_nodes = 0;

    o_NodeList      is_shared   = o_extract_is_shared(sg);
    o_StackEdgeList stack_edges = o_extract_stackedges(sg);
    o_HeapEdgeList  heap_edges  = o_extract_heapedges(sg);

    o_NodeList      nodes = o_extract_heapnodes(sg);
    o_VarList        vars = o_vars_by_stackedges(o_extract_stackedge_set(sg));

    fprintf(fp, "    title: \"%s-sg%d\"\n", name, n_graphs);
    fprintf(fp, "    orientation: left_to_right\n");
    fprintf(fp, "    status: grey\n");
    fprintf(fp, "    display_edge_labels: yes\n");
    fprintf(fp, "    manhattan_edges: no\n");
    fprintf(fp, "    ignore_singles : no\n");
    fprintf(fp, "    xspace: 50\n");
    fprintf(fp, "    %s\n", attrib);

    // all heap nodes
    while (!o_NodeList_is_empty(nodes)) {
        o_Node node = o_NodeList_head(nodes);
        nodes = o_NodeList_tail(nodes);
		    //gstats.addHeapNode();
        n_nodes++;
        
        fprintf(fp, "    node: { /*heap node*/ \n");
        fprintf(fp, "      title: \"%s-sg%d", name, n_graphs);
        o_Node_print_fp(fp, node);
        fprintf(fp, "\"\n");
        fprintf(fp, "      label: \"");
        dfi_write_Node_fp(fp, node);
        fprintf(fp, "\"\n");
        if (o_NodeList_is_elem(is_shared, node))
        {
            fprintf(fp, "      shape: rhomb\n");
            // vpavlu: 2008-12-01: edge was always set to 0
            //if (!edge)
            {
                if (o_Node_is_empty(node)) 
                    fprintf(fp, "      color: darkmagenta\n");
                else
                    fprintf(fp, "      color: red\n");
            }
        }
        fprintf(fp, "    }\n\n");
    }

    // all var nodes
    while (!o_VarList_is_empty(vars)) {
        o_VariableId node = o_VarList_head(vars);
		    //gstats.addStackNode();

        vars = o_VarList_tail(vars);
        fprintf(fp, "    node: { /*var node*/ \n");
        fprintf(fp, "      title: \"%s-sg%d", name, n_graphs);
        o_VariableId_print_fp(fp, node);
        fprintf(fp, "\"\n      label: \"");
        o_VariableId_print_fp(fp, node);
        fprintf(fp, "\"\n");
        fprintf(fp, "      shape: ellipse\n");
        fprintf(fp, "      level: 0\n");
        fprintf(fp, "    }\n\n");
    }

    // all stack edges
    while (!o_StackEdgeList_is_empty(stack_edges)) {
        o_StackEdge    edge = o_StackEdgeList_head(stack_edges);
        o_VariableId    var = o_StackEdge_select_1(edge);
        o_Node    vars = o_StackEdge_select_2(edge);

        stack_edges = o_StackEdgeList_tail(stack_edges);
        fprintf(fp, "    edge: { /*stackedge*/\n");
        fprintf(fp, "      sourcename: \"%s-sg%d", name, n_graphs);
        o_VariableId_print_fp(fp, var);
        fprintf(fp, "\"\n");
        fprintf(fp, "      targetname: \"%s-sg%d", name, n_graphs);
        o_Node_print_fp(fp, vars);
        fprintf(fp, "\"\n");
        fprintf(fp, "    }\n\n");
    }

    // all heap edges
    while (!o_HeapEdgeList_is_empty(heap_edges)) {
        o_HeapEdge edge  = o_HeapEdgeList_head(heap_edges);
        o_Node node1     = o_HeapEdge_select_1(edge);
        o_VariableId var = o_HeapEdge_select_2(edge);
        o_Node node2     = o_HeapEdge_select_3(edge);

        heap_edges = o_HeapEdgeList_tail(heap_edges);
        fprintf(fp, "    edge: { /*heapedge*/\n");
        fprintf(fp, "      sourcename: \"%s-sg%d", name, n_graphs);
        o_Node_print_fp(fp, node1);
        fprintf(fp, "\"\n");
        fprintf(fp, "      targetname: \"%s-sg%d", name, n_graphs);
        o_Node_print_fp(fp, node2);
        fprintf(fp, "\"\n");
        fprintf(fp, "      label: \"");
        o_VariableId_print_fp(fp, var);
        fprintf(fp, "\"\n");
        fprintf(fp, "    }\n\n");
    }

    if (!n_nodes) {
        fprintf(fp, "  node: { /*no nodes*/ \n");
        fprintf(fp, "    title: \"%s-sg%d_dummy\"\n", name, n_graphs);
        fprintf(fp, "    label: \"\"\n");
        fprintf(fp, "    width : 0\n");
        fprintf(fp, "    height: 0\n");
        fprintf(fp, "  }\n");
    }
    
    // end graph
    fprintf(fp, "  }\n\n");
    return n_nodes;

    return 1;
}

void dfi_write_Node_fp(FILE * fp, o_Node node) {
    _o_Node_cur cur;

    std::vector<std::string> varSet;
    o_Node_cur_reset(&cur,node);
    while(!o_Node_cur_is_empty(&cur)) {
        str var   = o_varid_str(o_Node_cur_get(&cur));
        o_Node_cur_next(&cur);
        //if (var!=0) fprintf(fp,"%s",var); // MS adapted to char*
        //else fprintf(fp,"NOVAR");
        //if(!o_Node_cur_is_empty(&cur))
        //fprintf(fp,",");
        if (var!=0) {
            varSet.push_back(var);
        } else {
            varSet.push_back("NOVAR");
        }
    }
    // we need to sort the set of variable names to ensure that the same string is generated for the same set
    sort(varSet.begin(),varSet.end());
    fprintf(fp,"{");
    for(std::vector<std::string>::iterator i=varSet.begin();i!=varSet.end(); i++) {
        if(i!=varSet.begin()) {
            fprintf(fp,",");
        }
        std::string varName=*i;
        const char* c=varName.c_str();
        fprintf(fp,c);
    }
    fprintf(fp,"}");
}

// vim: ts=4 sts=2 expandtab:
