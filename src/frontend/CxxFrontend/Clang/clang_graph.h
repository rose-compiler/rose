
// This code is to support building DOT graphs of the EDG AST ans associated nodes
// so that we can better understand issues in the translation of the EDG representation
// into ROSE.  The modivating problem has been the handling of GNU statement expressions
// where types are declared and extra source sequence points are added to the AST and
// we need to understand this better so that we can skip over them in the translation.

namespace CLANG_ROSE_Graph
   {
  // File for output for generated graph.
  // EXTERN std::ofstream file;
     extern std::ofstream file;

  // DQ (10/23/2020): We need to express similar function in terms of Clang IR nodes.

     void graph ( clang::Decl * decl );
     void graph ( clang::Stmt * stmt );
     void graph ( const clang::Type * type );
     void graph ( clang::DeclContext * decl_context );

#if 0
  // Graph functions that write DOT file nodes, any children (and edges from the node to the children) to the output file.
  // void graph ( a_scope_ptr scope, const std::string & scope_name = "" );
     void graph ( a_scope_ptr scope, const std::string & scope_name);

     void graph ( a_statement_ptr statement );
     void graph ( an_expr_node_ptr expression );
     void graph ( a_constant_ptr constant );
     void graph ( a_dynamic_init_ptr dynamic_initializer );
     void graph ( a_type_ptr type );
     void graph ( a_field_ptr field );
     void graph ( a_variable_ptr variable );
     void graph ( a_routine_ptr routine );
     void graph ( a_template_ptr template_node );
     void graph ( a_namespace_ptr namespace_node );

  // DQ (1/5/2014): Added more general support for a_source_sequence_entry_ptr.
     void graph ( a_source_sequence_entry_ptr src_seq_entry );

  // DQ (9/21/2015): Added support for function parameter types.
     void graph ( a_param_type_ptr type );

  // DQ (9/25/2015): Added support for function parameter types.
     void graph ( a_template_symbol_supplement_ptr template_sup );
     void graph ( a_template_decl_ptr template_decl );
     void graph ( a_template_param_type_supplement_ptr extra_info );
     void graph ( a_template_parameter_ptr template_parameter );

  // DQ (6/16/2016): Adding more graph support for template arguments.
     void graph ( a_template_arg_ptr template_argument );
     
     void graph ( a_using_decl_ptr udp );

  // DQ (2/10/2018): Adding more graph support for vla typed paramters.
     void graph ( a_vla_dimension_ptr vla_dimension );

  // TV (05/21/2018): Support for base classes
     void graph ( a_base_class_ptr base_class );

  // TV (03/29/2018): Support for symbols
     void graph ( a_symbol_ptr symbol );

  // DQ (1/19/2020): Adding support for new and delete operators.
     void graph ( a_new_delete_supplement_ptr new_delete_supplement );

  // Functions to filer the graph to be relevant to the input code (skips IR nodes from header files).
     bool outputInGraph ( SourceSequenceList & sse );
     bool outputInGraph( const a_source_position & pos );
     bool outputInGraph( a_statement_ptr statement );
     bool outputInGraph( a_block_ptr block );

  // DQ (1/19/2016): Adding support for preinitialization lists.
     void graph ( a_constructor_init_ptr ctor_init );

  // Main function to call to build the DOT file for the EDG AST.
     void graph_edg_ast();

  // DQ (6/16/2016): Added to support debugging.
     void insertGraphNode (void* node);

  // DQ (1/3`1/2018): Adding more support for scopes.
     void graph ( a_block_ptr block );

  // DQ (9/17/2018): Build a graph of the header files.
     void graph_headers();

  // DQ (9/17/2018): Build a graph of the header files.
     void graph_headers( a_source_file_ptr file );

  // DQ (8/19/2020): Adding more graph support for the switch_case_entry.
     void graph ( a_switch_case_entry_ptr switch_case_entry );

  // DQ (8/19/2020): Adding more graph support for the switch_stmt_descriptor.
     void graph ( a_switch_stmt_descr_ptr switch_stmt_descr );

  // DQ (8/20/2020): Adding more graph support for attributes.
     void graph ( an_attribute_ptr attributes );
#endif
   }

