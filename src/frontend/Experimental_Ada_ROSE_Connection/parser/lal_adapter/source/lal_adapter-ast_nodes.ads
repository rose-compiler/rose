package LAL_Adapter.AST_Nodes is
   -- Class hierarchy to organize processing of the ~ 365 different kinds of
   -- libadalang AST nodes.
   
   -- This should be a duplicate of the code starting at the definition of
   -- Ada_Node in C:\GNAT\2021\include\libadalang\libadalang-analysis.ads.
   
   ---------------
   -- AST nodes --
   ---------------

   type Ada_Node is tagged private;
   --  Root node class for the Ada syntax tree.
   procedure Process 
     (This : in Ada_Node);
   
   type Expr is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Expr);
   --  Base class for expressions.

   type Basic_Decl is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Basic_Decl);
   --  Root class for an Ada declaration (RM 3.1). A declaration associates
   --  a name with a language entity, for example a type or a variable.
   
   --  type Ada_List is new Ada_Node with private;
   --  
   --  type Ada_Node_List is new Ada_List with private
   --     with Iterable => (First       => Ada_Node_List_First,
   --                       Next        => Ada_Node_List_Next,
   --                       Has_Element => Ada_Node_List_Has_Element,
   --                       Element     => Ada_Node_List_Element);
   --  List of AdaNode.
   --
   --  This list node can contain one of the following nodes:
   --  --  * Abstract_State_Decl
   --  * Abstract_Subp_Decl
   --  * Allocator
   --  * Aspect_Clause
   --  * Attribute_Ref
   --  * Base_Aggregate
   --  * Bin_Op
   --  * Body_Node
   --  * Call_Expr
   --  * Char_Literal
   --  * Component_Clause
   --  * Component_Decl
   --  * Cond_Expr
   --  * Decl_Expr
   --  * Dotted_Name
   --  * Entry_Decl
   --  * Error_Decl
   --  * Exception_Decl
   --  * Exception_Handler
   --  * Explicit_Deref
   --  * Generic_Decl
   --  * Generic_Formal
   --  * Generic_Instantiation
   --  * Generic_Renaming_Decl
   --  * Identifier
   --  * Incomplete_Type_Decl
   --  * Membership_Expr
   --  * Null_Component_Decl
   --  * Null_Literal
   --  * Num_Literal
   --  * Number_Decl
   --  * Object_Decl
   --  * Others_Designator
   --  * Package_Decl
   --  * Package_Renaming_Decl
   --  * Paren_Abstract_State_Decl
   --  * Paren_Expr
   --  * Pragma_Node
   --  * Protected_Type_Decl
   --  * Qual_Expr
   --  * Quantified_Expr
   --  * Raise_Expr
   --  * Single_Protected_Decl
   --  * Single_Task_Decl
   --  * Stmt
   --  * String_Literal
   --  * Subp_Decl
   --  * Subtype_Decl
   --  * Subtype_Indication
   --  * Target_Name
   --  * Task_Type_Decl
   --  * Type_Decl
   --  * Un_Op
   --  * Use_Clause
   --  * With_Clause
   --  type Alternatives_List is new Ada_Node_List with private;
   --  List of alternatives in a ``when ...`` clause.
   --
   --  This list node can contain one of the following nodes:
   --  * Allocator
   --  * Attribute_Ref
   --  * Base_Aggregate
   --  * Bin_Op
   --  * Call_Expr
   --  * Char_Literal
   --  * Cond_Expr
   --  * Decl_Expr
   --  * Discrete_Subtype_Indication
   --  * Dotted_Name
   --  * Explicit_Deref
   --  * Identifier
   --  * Membership_Expr
   --  * Null_Literal
   --  * Num_Literal
   --  * Others_Designator
   --  * Paren_Expr
   --  * Qual_Expr
   --  * Quantified_Expr
   --  * Raise_Expr
   --  * String_Literal
   --  * Target_Name
   --  * Un_Op

   type Name is new Expr with private;
   overriding
   procedure Process 
     (This : in Name);
   --  Base class for names.

   type Single_Tok_Node is new Name with private;
   overriding
   procedure Process 
     (This : in Single_Tok_Node);
   --  Base class for nodes that are made up of a single token.
   
   type Base_Id is new Single_Tok_Node with private;
   overriding
   procedure Process 
     (This : in Base_Id);
   --  Base class for identifiers.
   
   type Identifier is new Base_Id with private;
   overriding
   procedure Process 
     (This : in Identifier);
   --  Regular identifier.
   
   type Abort_Node is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Abort_Node);
   --  Qualifier for the ``abort`` keyword.
   
   type Abort_Absent is new Abort_Node with private;
   overriding
   procedure Process 
     (This : in Abort_Absent);
      
   type Abort_Present is new Abort_Node with private;
   overriding
   procedure Process 
     (This : in Abort_Present);
      
   type Stmt is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Stmt);
   --  Bass class for statements.
   
   type Simple_Stmt is new Stmt with private;
   overriding
   procedure Process 
     (This : in Simple_Stmt);
   --  Base class for simple statements.
  
   type Abort_Stmt is new Simple_Stmt with private;
   overriding
   procedure Process 
     (This : in Abort_Stmt);
   --  ``abort`` statement.
  
   type Abstract_Node is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Abstract_Node);
   --  Qualifier for the ``abstract`` keyword.
  
   type Abstract_Absent is new Abstract_Node with private;
   overriding
   procedure Process 
     (This : in Abstract_Absent);

   type Basic_Subp_Decl is new Basic_Decl with private;
   overriding
   procedure Process 
     (This : in Basic_Subp_Decl);
   --  Base class for subprogram declarations.
   
   type Classic_Subp_Decl is new Basic_Subp_Decl with private;
   overriding
   procedure Process 
     (This : in Classic_Subp_Decl);
   --  This is an intermediate abstract class for subprogram declarations
   --  with a common structure: overriding indicator, ``SubpSpec``, aspects,
   --  <other fields>.
  
   type Formal_Subp_Decl is new Classic_Subp_Decl with private;
   overriding
   procedure Process 
     (This : in Formal_Subp_Decl);
   --  Formal subprogram declarations, in generic declarations formal parts.
  
   type Abstract_Formal_Subp_Decl is new Formal_Subp_Decl with private;
   overriding
   procedure Process 
     (This : in Abstract_Formal_Subp_Decl);
   --  Formal declaration for an abstract subprogram.
  
   type Abstract_Present is new Abstract_Node with private;
   overriding
   procedure Process 
     (This : in Abstract_Present);

   type Abstract_State_Decl is new Basic_Decl with private;
   overriding
   procedure Process 
     (This : in Abstract_State_Decl);
   --  Contained (directly or indirectly) in an AbstractStateDeclExpr, and
   --  is used to represent the BasicDecl associated with the abstract state
   --  introduced by the Abstract_State aspect. This node is necessary
   --  because all of our name resolution routines expect BasicDecls as
   --  environments' values.
   --
   --  The only purpose of this node is to populate the env with the
   --  abstract state declared through this node, so it can be referred in
   --  SPARK aspects such as Global, Depends, Refined_State, etc.
  
   type Abstract_State_Decl_Expr is new Expr with private;
   overriding
   procedure Process 
     (This : in Abstract_State_Decl_Expr);
   --  Directly corresponds to the right-hand side of the Abstract_State
   --  aspect. Only exists because the RHS of an AspectAssoc must be an
   --  expression: the actual logic is in AbstractStateDecl.
   --  type Abstract_State_Decl_List is new Ada_Node_List with private;
   --  --  List of AbstractStateDecls.
   --  --
   --  --  This list node can contain one of the following nodes:
   --  --  * Abstract_State_Decl
   --  --  * Paren_Abstract_State_Decl
  
   type Abstract_Subp_Decl is new Classic_Subp_Decl with private;
   overriding
   procedure Process 
     (This : in Abstract_Subp_Decl);
   --  Declaration for an abstract subprogram.
  
   type Composite_Stmt is new Stmt with private;
   overriding
   procedure Process 
     (This : in Composite_Stmt);
   --  Base class for composite statements.
  
   type Accept_Stmt is new Composite_Stmt with private;
   overriding
   procedure Process 
     (This : in Accept_Stmt);
   --  ``accept`` statement.
  
   type Accept_Stmt_With_Stmts is new Accept_Stmt with private;
   overriding
   procedure Process 
     (This : in Accept_Stmt_With_Stmts);
   --  Extended ``accept`` statement.
  
   type Type_Def is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Type_Def);
   --  Base class for type definitions.
  
   type Access_Def is new Type_Def with private;
   overriding
   procedure Process 
     (This : in Access_Def);
   --  Base class for access type definitions.
  
   type Access_To_Subp_Def is new Access_Def with private;
   overriding
   procedure Process 
     (This : in Access_To_Subp_Def);
   --  Type definition for accesses to subprograms.
  
   type Base_Aggregate is new Expr with private;
   overriding
   procedure Process 
     (This : in Base_Aggregate);
   --  Base class for aggregates.
  
   type Aggregate is new Base_Aggregate with private;
   overriding
   procedure Process 
     (This : in Aggregate);
   --  Aggregate that is not a ``null record`` aggregate.
  
   type Basic_Assoc is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Basic_Assoc);
   --  Association of one or several names to an expression.
   
   type Aggregate_Assoc is new Basic_Assoc with private;
   overriding
   procedure Process 
     (This : in Aggregate_Assoc);
   --  Assocation (X => Y) used for aggregates and parameter associations.
  
   type Aliased_Node is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Aliased_Node);
   --  Qualifier for the ``aliased`` keyword.
  
   type Aliased_Absent is new Aliased_Node with private;
   overriding
   procedure Process 
     (This : in Aliased_Absent);

      
   type Aliased_Present is new Aliased_Node with private;
   overriding
   procedure Process 
     (This : in Aliased_Present);
      
   type All_Node is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in All_Node);
   --  Qualifier for the ``all`` keyword.
   
   type All_Absent is new All_Node with private;
   overriding
   procedure Process 
     (This : in All_Absent);
      
   type All_Present is new All_Node with private;
   overriding
   procedure Process 
     (This : in All_Present);

   type Allocator is new Expr with private;
   overriding
   procedure Process 
     (This : in Allocator);
   --  Allocator expression (``new ...``).
   
   type Anonymous_Expr_Decl is new Basic_Decl with private;
   overriding
   procedure Process 
     (This : in Anonymous_Expr_Decl);

   --  Represents a anonymous declaration that holds an expression.
   --
   --  This is used to store the results of queries such as
   --  ``referenced_decl`` called on references to object formals from
   --  inside a instantiated generic in order to return the relevant actual.
   --
   --  Indeed, ``referenced_decl`` must return a ``BasicDecl``, but actuals
   --  of generic instantiations are ``Expr``s. This wrapper node is
   --  therefore a way to both satisfy the ``BasicDecl`` inteface, and
   --  provide to the user the expression of the actual through the ``expr``
   --  field.

   type Type_Expr is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Type_Expr);
   --  A type expression is an abstract node that embodies the concept of a
   --  reference to a type.
   --
   --  Since Ada has both subtype_indications and anonymous (inline) type
   --  declarations, a type expression contains one or the other.

   type Anonymous_type is new Type_Expr with private;
   overriding
   procedure Process 
     (This : in Anonymous_type);
   --  Container for inline anonymous array and access types declarations.

   type Base_Type_Access_Def is new Access_Def with private;
   overriding
   procedure Process 
     (This : in Base_Type_Access_Def);
   --  Base class for access type definitions.

   type Anonymous_Type_Access_Def is new Base_Type_Access_Def with private;
   overriding
   procedure Process 
     (This : in Anonymous_Type_Access_Def);
   --  Synthetic type access, that will directly reference a type decl. It
   --  is used to generate synthetic anonymous access types.

   type Base_Type_Decl is new Basic_Decl with private;
   overriding
   procedure Process 
     (This : in Base_Type_Decl);
   --  Base class for type declarations.

   type Type_Decl is new Base_Type_Decl with private;
   overriding
   procedure Process 
     (This : in Type_Decl);
   --  Type declarations that embed a type definition node.

   type Anonymous_Type_Decl is new Type_Decl with private;
   overriding
   procedure Process 
     (This : in Anonymous_Type_Decl);
   --  Anonymous type declaration (for anonymous array or access types).

   type Array_Indices is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Array_Indices);
   --  Specification for array indexes.

   type Array_Type_Def is new Type_Def with private;
   overriding
   procedure Process 
     (This : in Array_Type_Def);
   --  Type definition for an array.

   type Aspect_Assoc is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Aspect_Assoc);
   --  Name/expression association in an aspect.
   --  type Aspect_Assoc_List is new Ada_List with private
   --     with Iterable => (First       => Aspect_Assoc_List_First,
   --                       Next        => Aspect_Assoc_List_Next,
   --                       Has_Element => Aspect_Assoc_List_Has_Element,
   --                       Element     => Aspect_Assoc_List_Element);
   --  List of AspectAssoc.

   type Aspect_Clause is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Aspect_Clause);
   --  Base class for aspect clauses.

   type Aspect_Spec is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Aspect_Spec);
   --  List of aspects in a declaration.

   type Assign_Stmt is new Simple_Stmt with private;
   overriding
   procedure Process 
     (This : in Assign_Stmt);
   --  Statement for assignments.

   --  type Basic_Assoc_List is new Ada_List with private
   --     with Iterable => (First       => Basic_Assoc_List_First,
   --                       Next        => Basic_Assoc_List_Next,
   --                       Has_Element => Basic_Assoc_List_Has_Element,
   --                       Element     => Basic_Assoc_List_Element);
   --  List of BasicAssoc.
   --  type Assoc_List is new Basic_Assoc_List with private;
   --  List of associations.

   type At_Clause is new Aspect_Clause with private;
   overriding
   procedure Process 
     (This : in At_Clause);
   --  Representation clause (``for .. use at ...;``).

   type Attribute_Def_Clause is new Aspect_Clause with private;
   overriding
   procedure Process 
     (This : in Attribute_Def_Clause);
   --  Clause for an attribute definition (``for ...'Attribute use ...;``).

   type Attribute_Ref is new Name with private;
   overriding
   procedure Process 
     (This : in Attribute_Ref);
   --  Expression to reference an attribute.

   type Base_Assoc is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Base_Assoc);
   --  Abstract class for a key/value association, where the value is an

   --  expression.
   --  type Base_Assoc_List is new Ada_List with private
   --     with Iterable => (First       => Base_Assoc_List_First,
   --                       Next        => Base_Assoc_List_Next,
   --                       Has_Element => Base_Assoc_List_Has_Element,
   --                       Element     => Base_Assoc_List_Element);
   --  List of BaseAssoc.

   type Base_Formal_Param_Decl is new Basic_Decl with private;
   overriding
   procedure Process 
     (This : in Base_Formal_Param_Decl);
   --  Base class for formal parameter declarations. This is used both for
   --  records components and for subprogram parameters.
   --
   --  This is a Libadalang abstraction, that has no ARM existence.

   type Base_Formal_Param_Holder is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Base_Formal_Param_Holder);
   --  Base class for lists of formal parameters. This is used both for
   --  subprogram specifications and for records, so that we can share the
   --  matching and unpacking logic.

   type Base_Loop_Stmt is new Composite_Stmt with private;
   overriding
   procedure Process 
     (This : in Base_Loop_Stmt);
   --  Base class for loop statements.

   type Base_Package_Decl is new Basic_Decl with private;
   overriding
   procedure Process 
     (This : in Base_Package_Decl);
   --  Package declarations. Concrete instances of this class will be
   --  created in generic package declarations. Other non-generic package
   --  declarations will be instances of PackageDecl.
   --
   --  The behavior is the same, the only difference is that BasePackageDecl
   --  and PackageDecl have different behavior regarding lexical
   --  environments. In the case of generic package declarations, we use
   --  BasePackageDecl which has no env_spec, and the environment behavior
   --  is handled by the GenericPackageDecl instance.

   type Base_Record_Def is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Base_Record_Def);
   --  Base class for record definitions.

   type Body_Node is new Basic_Decl with private;
   overriding
   procedure Process 
     (This : in Body_Node);
   --  Base class for an Ada body (RM 3.11). A body is the completion of a
   --  declaration.

   type Base_Subp_body is new Body_Node with private;
   overriding
   procedure Process 
     (This : in Base_Subp_body);
   --  Base class for subprogram bodies.

   type Base_Subp_Spec is new Base_Formal_Param_Holder with private;
   overriding
   procedure Process 
     (This : in Base_Subp_Spec);
   --  Base class for subprogram specifications.

   type Base_Subtype_Decl is new Base_Type_Decl with private;
   overriding
   procedure Process 
     (This : in Base_Subtype_Decl);
   --  Base class for subtype declarations.

   --  type Basic_Decl_List is new Ada_List with private
   --     with Iterable => (First       => Basic_Decl_List_First,
   --                       Next        => Basic_Decl_List_Next,
   --                       Has_Element => Basic_Decl_List_Has_Element,
   --                       Element     => Basic_Decl_List_Element);
   --  List of BasicDecl.
   --
   --  This list node can contain one of the following nodes:
   --  * Number_Decl
   --  * Object_Decl
   --  * Single_Protected_Decl
   --  * Single_Task_Decl

   type Block_Stmt is new Composite_Stmt with private;
   overriding
   procedure Process 
     (This : in Block_Stmt);
   --  Base class for statement blocks.

   type Begin_Block is new Block_Stmt with private;
   overriding
   procedure Process 
     (This : in Begin_Block);
   --  Statement block with no declarative part.

   type Bin_Op is new Expr with private;
   overriding
   procedure Process 
     (This : in Bin_Op);
   --  Binary expression.

   type Body_Stub is new Body_Node with private;
   overriding
   procedure Process 
     (This : in Body_Stub);
   --  Base class for a body stub (RM 10.1.3). A body stub is meant to be
   --  completed by .

   type Box_Expr is new Expr with private;
   overriding
   procedure Process 
     (This : in Box_Expr);
   --  Box expression (``<>``).

   type Bracket_Aggregate is new Aggregate with private;
   overriding
   procedure Process 
     (This : in Bracket_Aggregate);
   --  Bracket array or container aggregate (Ada 2020).

   type Delta_Aggregate is new Base_Aggregate with private;
   overriding
   procedure Process 
     (This : in Delta_Aggregate);

   type Bracket_Delta_Aggregate is new Delta_Aggregate with private;
   overriding
   procedure Process 
     (This : in Bracket_Delta_Aggregate);
   --  Bracket delta aggregate (Ada 2020).

   type Call_Expr is new Name with private;
   overriding
   procedure Process 
     (This : in Call_Expr);
   --  Represent a syntactic call expression.
   --
   --  At the semantic level, this can be either a subprogram call, an array
   --  subcomponent access expression, an array slice or a type conversion.

   type Call_Stmt is new Simple_Stmt with private;
   overriding
   procedure Process 
     (This : in Call_Stmt);
   --  Statement for entry or procedure calls.

   type Cond_Expr is new Expr with private;
   overriding
   procedure Process 
     (This : in Cond_Expr);
   --  Base class for a conditional expressions (RM 4.5.7).

   type Case_Expr is new Cond_Expr with private;
   overriding
   procedure Process 
     (This : in Case_Expr);
   --  ``case`` expression (RM 4.5.7).

   type Case_Expr_Alternative is new Expr with private;
   overriding
   procedure Process 
     (This : in Case_Expr_Alternative);
   --  Alternative in a ``case`` expression (``when ... => ...``).

   --  type Case_Expr_Alternative_List is new Ada_List with private
   --     with Iterable => (First       => Case_Expr_Alternative_List_First,
   --                       Next        => Case_Expr_Alternative_List_Next,
   --                       Has_Element => Case_Expr_Alternative_List_Has_Element,
   --                       Element     => Case_Expr_Alternative_List_Element);
   --  List of CaseExprAlternative.

   type Case_Stmt is new Composite_Stmt with private;
   overriding
   procedure Process 
     (This : in Case_Stmt);
   --  ``case`` statement.

   type Case_Stmt_Alternative is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Case_Stmt_Alternative);
   --  Alternative in a ``case`` statement (``when ... => ...``).

   --  type Case_Stmt_Alternative_List is new Ada_List with private
   --  with Iterable => (First       => Case_Stmt_Alternative_List_First,
   --                    Next        => Case_Stmt_Alternative_List_Next,
   --                    Has_Element => Case_Stmt_Alternative_List_Has_Element,
   --                    Element     => Case_Stmt_Alternative_List_Element);
   --  List of CaseStmtAlternative.

   type Char_Literal is new Base_Id with private;
   overriding
   procedure Process 
     (This : in Char_Literal);
   --  Character literal.

   type Classwide_Type_Decl is new Base_Type_Decl with private;
   overriding
   procedure Process 
     (This : in Classwide_Type_Decl);
   --  Synthetic node (not parsed, generated from a property call). Refers
   --  to the classwide type for a given tagged type. The aim is that those
   --  be mostly equivalent to their non-classwide type, except for some
   --  resolution rules.

   type Compilation_Unit is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Compilation_Unit);
   --  Root node for all Ada analysis units.

   --  type Compilation_Unit_List is new Ada_List with private
   --  with Iterable => (First       => Compilation_Unit_List_First,
   --                    Next        => Compilation_Unit_List_Next,
   --                    Has_Element => Compilation_Unit_List_Has_Element,
   --                    Element     => Compilation_Unit_List_Element);
   --  List of CompilationUnit.

   type Component_Clause is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Component_Clause);
   --  Representation clause for a single component.

   type Component_Decl is new Base_Formal_Param_Decl with private;
   overriding
   procedure Process 
     (This : in Component_Decl);
   --  Declaration for a component.

   type Component_Def is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Component_Def);
   --  Definition for a component.

   type Component_List is new Base_Formal_Param_Holder with private;
   overriding
   procedure Process 
     (This : in Component_List);
   --  List of component declarations.

   type Concrete_Formal_Subp_Decl is new Formal_Subp_Decl with private;
   overriding
   procedure Process 
     (This : in Concrete_Formal_Subp_Decl);
   --  Formal declaration for a concrete subprogram.

   type Constant_Node is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Constant_Node);
   --  Qualifier for the ``constant`` keyword.

   type Constant_Absent is new Constant_Node with private;
   overriding
   procedure Process 
     (This : in Constant_Absent);

   type Constant_Present is new Constant_Node with private;
   overriding
   procedure Process 
     (This : in Constant_Present);

   type Constrained_Array_Indices is new Array_Indices with private;
   overriding
   procedure Process 
     (This : in Constrained_Array_Indices);
   --  Constrained specification for array indexes.

   type Subtype_Indication is new Type_Expr with private;
   overriding
   procedure Process 
     (This : in Subtype_Indication);
   --  Reference to a type by name.

   type Constrained_Subtype_Indication is new Subtype_Indication with private;
   overriding
   procedure Process 
     (This : in Constrained_Subtype_Indication);
   --  Reference to a type with a range constraint.

   type Constraint is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Constraint);
   --  Base class for type constraints.

   --  type Constraint_List is new Ada_Node_List with private;
   --  List of constraints.
   --
   --  This list node can contain one of the following nodes:
   --  * Attribute_Ref
   --  * Bin_Op
   --  * Call_Expr
   --  * Char_Literal
   --  * Dotted_Name
   --  * Explicit_Deref
   --  * Identifier
   --  * Qual_Expr
   --  * String_Literal
   --  * Subtype_Indication
   --  * Target_Name

   type Contract_Case_Assoc is new Base_Assoc with private;
   overriding
   procedure Process 
     (This : in Contract_Case_Assoc);
   --  Single association for the ``Contract_Case`` aspect.

   --  type Contract_Case_Assoc_List is new Ada_List with private
   --     with Iterable => (First       => Contract_Case_Assoc_List_First,
   --                       Next        => Contract_Case_Assoc_List_Next,
   --                       Has_Element => Contract_Case_Assoc_List_Has_Element,
   --                       Element     => Contract_Case_Assoc_List_Element);
   --  List of ContractCaseAssoc.

   type Contract_Cases is new Expr with private;
   overriding
   procedure Process 
     (This : in Contract_Cases);
   --  List of associations for the ``Contract_Case`` aspect.

   type Real_Type_Def is new Type_Def with private;
   overriding
   procedure Process 
     (This : in Real_Type_Def);
   --  Type definition for real numbers.

   type Decimal_Fixed_Point_Def is new Real_Type_Def with private;
   overriding
   procedure Process 
     (This : in Decimal_Fixed_Point_Def);
   --  Type definition for decimal fixed-point numbers.

   type Decl_Block is new Block_Stmt with private;
   overriding
   procedure Process 
     (This : in Decl_Block);
   --  Statement block with a declarative part.

   type Decl_Expr is new Expr with private;
   overriding
   procedure Process 
     (This : in Decl_Expr);
   --  Declare expression (Ada 2020).

   --  type Decl_List is new Ada_Node_List with private;
   --  List of declarations.
   --
   --  This list node can contain one of the following nodes:
   --  * Abstract_Subp_Decl
   --  * Aspect_Clause
   --  * Component_Decl
   --  * Entry_Decl
   --  * Expr_Function
   --  * Null_Subp_Decl
   --  * Pragma_Node
   --  * Subp_Decl
   --  * Subp_Renaming_Decl

   type Declarative_Part is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Declarative_Part);
   --  List of declarations.

   type Defining_Name is new Name with private;
   overriding
   procedure Process 
     (This : in Defining_Name);
   --  Name that defines an entity.

   --  type Defining_Name_List is new Ada_List with private
   --     with Iterable => (First       => Defining_Name_List_First,
   --                       Next        => Defining_Name_List_Next,
   --                       Has_Element => Defining_Name_List_Has_Element,
   --                       Element     => Defining_Name_List_Element);
   --  List of DefiningName.

   type Delay_Stmt is new Simple_Stmt with private;
   overriding
   procedure Process 
     (This : in Delay_Stmt);
   --  ``delay`` statement.

   type Delta_Constraint is new Constraint with private;
   overriding
   procedure Process 
     (This : in Delta_Constraint);
   --  Delta and range type constraint.

   type Derived_Type_Def is new Type_Def with private;
   overriding
   procedure Process 
     (This : in Derived_Type_Def);
   --  Type definition for a derived type.

   type Digits_Constraint is new Constraint with private;
   overriding
   procedure Process 
     (This : in Digits_Constraint);
   --  Digits and range type constraint.

   type Discrete_Base_Subtype_Decl is new Base_Subtype_Decl with private;
   overriding
   procedure Process 
     (This : in Discrete_Base_Subtype_Decl);
   --  Specific ``BaseSubtypeDecl`` synthetic subclass for the base type of
   --  scalar types.   type Discrete_Subtype_Indication is new Subtype_Indication with private;

   type Discrete_Subtype_Indication is new Subtype_Indication with private;
   overriding
   procedure Process 
     (This : in Discrete_Subtype_Indication);
   --  Reference to a type with a general constraint.

   type Discrete_Subtype_Name is new Name with private;
   overriding
   procedure Process 
     (This : in Discrete_Subtype_Name);
   --  Subtype name for membership test expressions.
 
   type Discriminant_Assoc is new Basic_Assoc with private;
   overriding
   procedure Process 
     (This : in Discriminant_Assoc);
   --  Association of discriminant names to an expression.
   
   --  type Identifier_List is new Ada_List with private
   --     with Iterable => (First       => Identifier_List_First,
   --                       Next        => Identifier_List_Next,
   --                       Has_Element => Identifier_List_Has_Element,
   --                       Element     => Identifier_List_Element);
   --  List of Identifier.
   --  type Discriminant_Choice_List is new Identifier_List with private;
   --  List of discriminant associations.

   type Discriminant_Constraint is new Constraint with private;
   overriding
   procedure Process 
     (This : in Discriminant_Constraint);
   --  List of constraints that relate to type discriminants.

   type Discriminant_Part is new Base_Formal_Param_Holder with private;
   overriding
   procedure Process 
     (This : in Discriminant_Part);
   --  Specification for discriminants in type declarations.

   type Discriminant_Spec is new Base_Formal_Param_Decl with private;
   overriding
   procedure Process 
     (This : in Discriminant_Spec);
   --  Known list of discriminants in type declarations.

   --  type Discriminant_Spec_List is new Ada_List with private
   --     with Iterable => (First       => Discriminant_Spec_List_First,
   --                       Next        => Discriminant_Spec_List_Next,
   --                       Has_Element => Discriminant_Spec_List_Has_Element,
   --                       Element     => Discriminant_Spec_List_Element);
   --  List of DiscriminantSpec.

   type Dotted_Name is new Name with private;
   overriding
   procedure Process 
     (This : in Dotted_Name);
   --  Name to select a suffix in a prefix.

   type Elsif_Expr_Part is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Elsif_Expr_Part);
   --  ``elsif`` block, part of an ``if`` expression.

   --  type Elsif_Expr_Part_List is new Ada_List with private
   --  with Iterable => (First       => Elsif_Expr_Part_List_First,
   --                    Next        => Elsif_Expr_Part_List_Next,
   --                    Has_Element => Elsif_Expr_Part_List_Has_Element,
   --                    Element     => Elsif_Expr_Part_List_Element);
   --  List of ElsifExprPart.

   type Elsif_Stmt_Part is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Elsif_Stmt_Part);
   --  ``elsif`` part in an ``if`` statement block.

   --  type Elsif_Stmt_Part_List is new Ada_List with private
   --  with Iterable => (First       => Elsif_Stmt_Part_List_First,
   --                    Next        => Elsif_Stmt_Part_List_Next,
   --                    Has_Element => Elsif_Stmt_Part_List_Has_Element,
   --                    Element     => Elsif_Stmt_Part_List_Element);
   --  List of ElsifStmtPart.

   type End_Name is new Name with private;
   overriding
   procedure Process 
     (This : in End_Name);
   --  Entity name in ``end ...;`` syntactic constructs.

   type Entry_body is new Body_Node with private;
   overriding
   procedure Process 
     (This : in Entry_body);
   --  Entry body.

   type Entry_Completion_Formal_Params is new Base_Formal_Param_Holder with private;
   overriding
   procedure Process 
     (This : in Entry_Completion_Formal_Params);
   --  Formal parameters for the completion of an ``EntryDecl`` (either an
   --  ``EntryBody`` or an ``AcceptStmt``).

   type Entry_Decl is new Basic_Subp_Decl with private;
   overriding
   procedure Process 
     (This : in Entry_Decl);
   --  Entry declaration.

   type Entry_Index_Spec is new Basic_Decl with private;
   overriding
   procedure Process 
     (This : in Entry_Index_Spec);
   --  Index specification for an entry body.

   type Entry_Spec is new Base_Subp_Spec with private;
   overriding
   procedure Process 
     (This : in Entry_Spec);
   --  Entry specification.

   type Enum_Lit_Synth_Type_Expr is new Type_Expr with private;
   overriding
   procedure Process 
     (This : in Enum_Lit_Synth_Type_Expr);
   --  Synthetic node. Represents the type expression for an enum literal.

   type Enum_Literal_Decl is new Basic_Subp_Decl with private;
   overriding
   procedure Process 
     (This : in Enum_Literal_Decl);
   --  Declaration for an enumeration literal.

   --  type Enum_Literal_Decl_List is new Ada_List with private
   --     with Iterable => (First       => Enum_Literal_Decl_List_First,
   --                       Next        => Enum_Literal_Decl_List_Next,
   --                       Has_Element => Enum_Literal_Decl_List_Has_Element,
   --                       Element     => Enum_Literal_Decl_List_Element);
   --  List of EnumLiteralDecl.

   type Enum_Rep_Clause is new Aspect_Clause with private;
   overriding
   procedure Process 
     (This : in Enum_Rep_Clause);
   --  Representation clause for enumeration types.

   type Enum_Subp_Spec is new Base_Subp_Spec with private;
   overriding
   procedure Process 
     (This : in Enum_Subp_Spec);
   --  Synthetic node for the abstract subprogram spec of an enum literal.
   --
   --  NOTE: This has no existence in the ARM. While enum literals are
   --  functions semantically, they're not such syntactically.

   type Enum_Type_Def is new Type_Def with private;
   overriding
   procedure Process 
     (This : in Enum_Type_Def);
   --  Type definition for enumerations.

   type Error_Decl is new Basic_Decl with private;
   overriding
   procedure Process 
     (This : in Error_Decl);
   --  Placeholder node for syntax errors in lists of declarations.

   type Error_Stmt is new Stmt with private;
   overriding
   procedure Process 
     (This : in Error_Stmt);
   --  Placeholder node for syntax errors in lists of statements.

   type Exception_Decl is new Basic_Decl with private;
   overriding
   procedure Process 
     (This : in Exception_Decl);
   --  Exception declarations.

   type Exception_Handler is new Basic_Decl with private;
   overriding
   procedure Process 
     (This : in Exception_Handler);
   --  Exception handler.

   type Exit_Stmt is new Simple_Stmt with private;
   overriding
   procedure Process 
     (This : in Exit_Stmt);
   --  ``exit`` statement.

   type Explicit_Deref is new Name with private;
   overriding
   procedure Process 
     (This : in Explicit_Deref);

   --  Explicit dereference expression (``.all``).
   --  type Expr_List is new Ada_List with private
   --     with Iterable => (First       => Expr_List_First,
   --                       Next        => Expr_List_Next,
   --                       Has_Element => Expr_List_Has_Element,
   --                       Element     => Expr_List_Element);
   --  List of Expr.
   --
   --  This list node can contain one of the following nodes:  * Allocator  * Attribute_Ref  * Base_Aggregate
   --  * Bin_Op
   --  * Call_Expr
   --  * Char_Literal
   --  * Cond_Expr
   --  * Decl_Expr
   --  * Discrete_Subtype_Name
   --  * Dotted_Name
   --  * Explicit_Deref
   --  * Identifier
   --  * Null_Literal
   --  * Num_Literal
   --  * Paren_Expr
   --  * Qual_Expr
   --  * Quantified_Expr
   --  * Raise_Expr
   --  * String_Literal
   --  * Target_Name
   --  * Un_Op
   --  type Expr_Alternatives_List is new Expr_List with private;
   --  List of alternatives in a membership test expression.
   --
   --  This list node can contain one of the following nodes:
   --  * Allocator
   --  * Attribute_Ref
   --  * Base_Aggregate
   --  * Bin_Op
   --  * Call_Expr
   --  * Char_Literal
   --  * Cond_Expr
   --  * Decl_Expr
   --  * Discrete_Subtype_Name
   --  * Dotted_Name
   --  * Explicit_Deref
   --  * Identifier
   --  * Null_Literal
   --  * Num_Literal
   --  * Paren_Expr
   --  * Qual_Expr
   --  * Quantified_Expr
   --  * Raise_Expr
   --  * String_Literal
   --  * Un_Op

   type Expr_function is new Base_Subp_body with private;
   overriding
   procedure Process 
     (This : in Expr_function);
   --  Expression function.

   type Extended_Return_Stmt is new Composite_Stmt with private;
   overriding
   procedure Process 
     (This : in Extended_Return_Stmt);
   --  Extended ``return`` statement.

   type Object_Decl is new Basic_Decl with private;
   overriding
   procedure Process 
     (This : in Object_Decl);
   --  Base class for Ada object declarations (RM 3.3.1). Ada object
   --  declarations are variables/constants declarations that can be
   --  declared in any declarative scope.

   type Extended_Return_Stmt_Object_Decl is new Object_Decl with private;
   overriding
   procedure Process 
     (This : in Extended_Return_Stmt_Object_Decl);
   --  Object declaration that is part of an extended return statement.

   type Floating_Point_Def is new Real_Type_Def with private;
   overriding
   procedure Process 
     (This : in Floating_Point_Def);
   --  Type definition for floating-point numbers.

   type Loop_Spec is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Loop_Spec);
   --  Base class for loop specifications.

   type For_Loop_Spec is new Loop_Spec with private;
   overriding
   procedure Process 
     (This : in For_Loop_Spec);
   --  Specification for a ``for`` loop.

   type For_Loop_Stmt is new Base_Loop_Stmt with private;
   overriding
   procedure Process 
     (This : in For_Loop_Stmt);
   --  Statement for ``for`` loops (``for ... loop ... end loop;``).

   type For_Loop_Var_Decl is new Basic_Decl with private;
   overriding
   procedure Process 
     (This : in For_Loop_Var_Decl);
   --  Declaration for the controlling variable in a ``for`` loop.

   type Formal_Discrete_Type_Def is new Type_Def with private;
   overriding
   procedure Process 
     (This : in Formal_Discrete_Type_Def);
   --  Type definition for discrete types in generic formals.

   type Generic_Decl is new Basic_Decl with private;
   overriding
   procedure Process 
     (This : in Generic_Decl);
   --  Base class for generic declarations.

   type Generic_Formal is new Base_Formal_Param_Decl with private;
   overriding
   procedure Process 
     (This : in Generic_Formal);
   --  Enclosing declaration for a generic formal. The real declaration is
   --  accessible via the ``decl`` field.

   type Generic_Formal_Obj_Decl is new Generic_Formal with private;
   overriding
   procedure Process 
     (This : in Generic_Formal_Obj_Decl);
   --  Formal declaration for an object.

   type Generic_Formal_package is new Generic_Formal with private;
   overriding
   procedure Process 
     (This : in Generic_Formal_package);
   --  Formal declaration for a package.

   type Generic_Formal_Part is new Base_Formal_Param_Holder with private;
   overriding
   procedure Process 
     (This : in Generic_Formal_Part);
   --  List of declaration for generic formals.

   type Generic_Formal_Subp_Decl is new Generic_Formal with private;
   overriding
   procedure Process 
     (This : in Generic_Formal_Subp_Decl);
   --  Formal declaration for a subprogram.

   type Generic_Formal_Type_Decl is new Generic_Formal with private;
   overriding
   procedure Process 
     (This : in Generic_Formal_Type_Decl);
   --  Formal declaration for a type.

   type Generic_Instantiation is new Basic_Decl with private;
   overriding
   procedure Process 
     (This : in Generic_Instantiation);
   --  Instantiations of generics.

   type Generic_Package_Decl is new Generic_Decl with private;
   overriding
   procedure Process 
     (This : in Generic_Package_Decl);
   --  Generic package declaration.

   type Generic_Package_Instantiation is new Generic_Instantiation with private;
   overriding
   procedure Process 
     (This : in Generic_Package_Instantiation);
   --  Instantiations of a generic package.

   type Generic_Package_Internal is new Base_Package_Decl with private;
   overriding
   procedure Process 
     (This : in Generic_Package_Internal);
   --  This class denotes the internal package contained by a
   --  GenericPackageDecl.

   type Generic_Renaming_Decl is new Basic_Decl with private;
   overriding
   procedure Process 
     (This : in Generic_Renaming_Decl);
   --  Base node for all generic renaming declarations.

   type Generic_Package_Renaming_Decl is new Generic_Renaming_Decl with private;
   overriding
   procedure Process 
     (This : in Generic_Package_Renaming_Decl);
   --  Declaration for a generic package renaming.

   type Generic_Subp_Decl is new Generic_Decl with private;
   overriding
   procedure Process 
     (This : in Generic_Subp_Decl);
   --  Generic subprogram declaration.

   type Generic_Subp_Instantiation is new Generic_Instantiation with private;
   overriding
   procedure Process 
     (This : in Generic_Subp_Instantiation);
   --  Instantiations of a generic subprogram.

   type Generic_Subp_Internal is new Basic_Subp_Decl with private;
   overriding
   procedure Process 
     (This : in Generic_Subp_Internal);
   --  Internal node for generic subprograms.

   type Generic_Subp_Renaming_Decl is new Generic_Renaming_Decl with private;
   overriding
   procedure Process 
     (This : in Generic_Subp_Renaming_Decl);
   --  Declaration for a generic subprogram renaming.

   type Goto_Stmt is new Simple_Stmt with private;
   overriding
   procedure Process 
     (This : in Goto_Stmt);
   --  ``goto`` statement.

   type Handled_Stmts is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Handled_Stmts);
   --  List of statements, with optional exception handlers.

   type If_Expr is new Cond_Expr with private;
   overriding
   procedure Process 
     (This : in If_Expr);
   --  ``if`` expression (RM 4.5.7).

   type If_Stmt is new Composite_Stmt with private;
   overriding
   procedure Process 
     (This : in If_Stmt);
   --  ``if`` statement block.

   type Incomplete_Type_Decl is new Base_Type_Decl with private;
   overriding
   procedure Process 
     (This : in Incomplete_Type_Decl);
   --  Incomplete declaration for a type.

   type Incomplete_Tagged_Type_Decl is new Incomplete_Type_Decl with private;
   overriding
   procedure Process 
     (This : in Incomplete_Tagged_Type_Decl);
   --  Incomplete declaration for a tagged type.
 
   type Index_Constraint is new Constraint with private;
   overriding
   procedure Process 
     (This : in Index_Constraint);
   --  TODO:? List of type constraints.

   type Num_Literal is new Single_Tok_Node with private;
   overriding
   procedure Process 
     (This : in Num_Literal);
   --  Base class for number literals.

   type Int_Literal is new Num_Literal with private;
   overriding
   procedure Process 
     (This : in Int_Literal);
   --  Literal for an integer.

   type Interface_Kind is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Interface_Kind);
   --  Kind of interface type.

   type Interface_Kind_limited is new Interface_Kind with private;
   overriding
   procedure Process 
     (This : in Interface_Kind_limited);

   type Interface_Kind_protected is new Interface_Kind with private;
   overriding
   procedure Process 
     (This : in Interface_Kind_protected);

   type Interface_Kind_synchronized is new Interface_Kind with private;
   overriding
   procedure Process 
     (This : in Interface_Kind_synchronized);
      
   type Interface_Kind_task is new Interface_Kind with private;
   overriding
   procedure Process 
     (This : in Interface_Kind_task);

   type Interface_Type_Def is new Type_Def with private;
   overriding
   procedure Process 
     (This : in Interface_Type_Def);
   --  Type definition for an interface.

   type Iter_type is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Iter_type);
   --  Iteration type for ``for`` loops.

   type Iter_Type_in is new Iter_type with private;
   overriding
   procedure Process 
     (This : in Iter_Type_in);

   type Iter_Type_of is new Iter_type with private;
   overriding
   procedure Process 
     (This : in Iter_Type_of);

   type Iterated_Assoc is new Basic_Assoc with private;
   overriding
   procedure Process 
     (This : in Iterated_Assoc);
   --  Iterated association (Ada 2020).

   type Known_Discriminant_Part is new Discriminant_Part with private;
   overriding
   procedure Process 
     (This : in Known_Discriminant_Part);
   --  Known list of discriminants in type declarations.

   type Label is new Simple_Stmt with private;
   overriding
   procedure Process 
     (This : in Label);
   --  Statement to declare a code label.

   type Label_Decl is new Basic_Decl with private;
   overriding
   procedure Process 
     (This : in Label_Decl);
   --  Declaration for a code label.

   type Library_Item is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Library_Item);
   --  Library item in a compilation unit.

   type Limited_Node is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Limited_Node);
   --  Qualifier for the ``limited`` keyword.

   type Limited_Absent is new Limited_Node with private;
   overriding
   procedure Process 
     (This : in Limited_Absent);

   type Limited_Present is new Limited_Node with private;
   overriding
   procedure Process 
     (This : in Limited_Present);

   type Loop_Stmt is new Base_Loop_Stmt with private;
   overriding
   procedure Process 
     (This : in Loop_Stmt);
   --  Statement for simple loops (``loop ... end loop;``).

   type Membership_Expr is new Expr with private;
   overriding
   procedure Process 
     (This : in Membership_Expr);
   --  Represent a membership test (in/not in operators).
   --
   --  Note that we don't consider them as binary operators since multiple
   --  expressions on the right hand side are allowed.

   type Mod_Int_Type_Def is new Type_Def with private;
   overriding
   procedure Process 
     (This : in Mod_Int_Type_Def);
   --  Type definition for a modular integer type.

   type Mode is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Mode);
   --  Syntactic indicators for passing modes in formals.

   type Mode_Default is new Mode with private;
   overriding
   procedure Process 
     (This : in Mode_Default);

   type Mode_in is new Mode with private;
   overriding
   procedure Process 
     (This : in Mode_in);

   type Mode_In_out is new Mode with private;
   overriding
   procedure Process 
     (This : in Mode_In_out);

   type Mode_out is new Mode with private;
   overriding
   procedure Process 
     (This : in Mode_out);

   type Multi_Abstract_State_Decl is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Multi_Abstract_State_Decl);
   --  Node that holds several AbstractStateDecl nodes, which is necessary
   --  when the Abstract_State aspect is associated with an aggregate in
   --  order to declare a list of abstract states.

   type Multi_Dim_Array_Assoc is new Aggregate_Assoc with private;
   overriding
   procedure Process 
     (This : in Multi_Dim_Array_Assoc);
   --  Association used for multi-dimension array aggregates.

   --  type Name_List is new Ada_List with private
   --     with Iterable => (First       => Name_List_First,
   --                       Next        => Name_List_Next,
   --                       Has_Element => Name_List_Has_Element,
   --                       Element     => Name_List_Element);
   --  List of Name.
   --
   --  This list node can contain one of the following nodes:
   --  * Attribute_Ref
   --  * Call_Expr
   --  * Char_Literal
   --  * Dotted_Name
   --  * Explicit_Deref
   --  * Identifier
   --  * Qual_Expr
   --  * String_Literal
   --  * Target_Name

   type Named_Stmt is new Composite_Stmt with private;
   overriding
   procedure Process 
     (This : in Named_Stmt);
   --  Wrapper class, used for composite statements that can be named
   --  (declare blocks, loops). This allows to both have a BasicDecl for the
   --  named entity declared, and a CompositeStmt for the statement
   --  hierarchy.

   type Named_Stmt_Decl is new Basic_Decl with private;
   overriding
   procedure Process 
     (This : in Named_Stmt_Decl);
   --  BasicDecl that is always the declaration inside a named statement.

   type Not_null is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Not_null);
   --  Qualifier for the ``not null`` keywords.

   type Not_Null_Absent is new Not_null with private;
   overriding
   procedure Process 
     (This : in Not_Null_Absent);

   type Not_Null_Present is new Not_null with private;
   overriding
   procedure Process 
     (This : in Not_Null_Present);

   type Null_Component_Decl is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Null_Component_Decl);
   --  Placeholder for the ``null`` in lists of components.

   type Null_Literal is new Single_Tok_Node with private;
   overriding
   procedure Process 
     (This : in Null_Literal);
   --  The ``null`` literal.

   type Null_Record_Aggregate is new Base_Aggregate with private;
   overriding
   procedure Process 
     (This : in Null_Record_Aggregate);
   --  Aggregate for ``null record``.

   type Null_Record_Def is new Base_Record_Def with private;
   overriding
   procedure Process 
     (This : in Null_Record_Def);
   --  Record definition for ``null record``.

   type Null_Stmt is new Simple_Stmt with private;
   overriding
   procedure Process 
     (This : in Null_Stmt);
   --  ``null;`` statement.

   type Null_Subp_Decl is new Base_Subp_body with private;
   overriding
   procedure Process 
     (This : in Null_Subp_Decl);
   --  Declaration for a null subprogram.

   type Number_Decl is new Basic_Decl with private;
   overriding
   procedure Process 
     (This : in Number_Decl);
   --  Declaration for a static constant number.

   type Op is new Base_Id with private;
   overriding
   procedure Process 
     (This : in Op);

   --  Operation in a binary expression.
   --
   --  Note that the ARM does not consider "double_dot" ("..") as a binary
   --  operator, but we process it this way here anyway to keep things
   --  simple.
   type Op_abs is new Op with private;
   overriding
   procedure Process 
     (This : in Op_abs);
      
   type Op_and is new Op with private;
   overriding
   procedure Process 
     (This : in Op_and);

   type Op_And_then is new Op with private;
   overriding
   procedure Process 
     (This : in Op_And_then);

   type Op_Concat is new Op with private;
   overriding
   procedure Process 
     (This : in Op_Concat);
      
   type Op_Div is new Op with private;
   overriding
   procedure Process 
     (This : in Op_Div);

   type Op_Double_Dot is new Op with private;
   overriding
   procedure Process 
     (This : in Op_Double_Dot);
      
   type Op_Eq is new Op with private;
   overriding
   procedure Process 
     (This : in Op_Eq);

   type Op_Gt is new Op with private;
   overriding
   procedure Process 
     (This : in Op_Gt);

   type Op_Gte is new Op with private;
   overriding
   procedure Process 
     (This : in Op_Gte);
      
   type Op_in is new Op with private;
   overriding
   procedure Process 
     (This : in Op_in);

   type Op_Lt is new Op with private;
   overriding
   procedure Process 
     (This : in Op_Lt);

   type Op_Lte is new Op with private;
   overriding
   procedure Process 
     (This : in Op_Lte);

   type Op_Minus is new Op with private;
   overriding
   procedure Process 
     (This : in Op_Minus);
      
   type Op_mod is new Op with private;
   overriding
   procedure Process 
     (This : in Op_mod);
      
   type Op_Mult is new Op with private;
   overriding
   procedure Process 
     (This : in Op_Mult);
      
   type Op_Neq is new Op with private;
   overriding
   procedure Process 
     (This : in Op_Neq);

   type Op_not is new Op with private;
   overriding
   procedure Process 
     (This : in Op_not);

   type Op_Not_in is new Op with private;
   overriding
   procedure Process 
     (This : in Op_Not_in);
      
   type Op_or is new Op with private;
   overriding
   procedure Process 
     (This : in Op_or);

   type Op_Or_else is new Op with private;
   overriding
   procedure Process 
     (This : in Op_Or_else);

   type Op_Plus is new Op with private;
   overriding
   procedure Process 
     (This : in Op_Plus);
      
   type Op_Pow is new Op with private;
   overriding
   procedure Process 
     (This : in Op_Pow);
      
   type Op_rem is new Op with private;
   overriding
   procedure Process 
     (This : in Op_rem);
      
   type Op_xor is new Op with private;
   overriding
   procedure Process 
     (This : in Op_xor);

   type Ordinary_Fixed_Point_Def is new Real_Type_Def with private;
   overriding
   procedure Process 
     (This : in Ordinary_Fixed_Point_Def);
   --  Type definition for ordinary fixed-point numbers.

   type Others_Designator is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Others_Designator);
   --  ``other`` designator.

   type Overriding_Node is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Overriding_Node);
   --  Syntactic indicators for subprogram overriding modes.

   type Overriding_Not_overriding is new Overriding_Node with private;
   overriding
   procedure Process 
     (This : in Overriding_Not_overriding);

   type Overriding_overriding is new Overriding_Node with private;
   overriding
   procedure Process 
     (This : in Overriding_overriding);
      
   type Overriding_Unspecified is new Overriding_Node with private;
   overriding
   procedure Process 
     (This : in Overriding_Unspecified);

   type Package_body is new Body_Node with private;
   overriding
   procedure Process 
     (This : in Package_body);
   --  Package body.

   type Package_Body_Stub is new Body_Stub with private;
   overriding
   procedure Process 
     (This : in Package_Body_Stub);
   --  Stub for a package body (``is separate``).

   type Package_Decl is new Base_Package_Decl with private;
   overriding
   procedure Process 
     (This : in Package_Decl);
   --  Non-generic package declarations.

   type Package_Renaming_Decl is new Basic_Decl with private;
   overriding
   procedure Process 
     (This : in Package_Renaming_Decl);
   --  Declaration for a package renaming.

   type Param_Assoc is new Basic_Assoc with private;
   overriding
   procedure Process 
     (This : in Param_Assoc);
   --  Assocation (X => Y) used for aggregates and parameter associations.

   type Param_Spec is new Base_Formal_Param_Decl with private;
   overriding
   procedure Process 
     (This : in Param_Spec);
   --  Specification for a parameter.
   
   --  type Param_Spec_List is new Ada_List with private
   --     with Iterable => (First       => Param_Spec_List_First,
   --                       Next        => Param_Spec_List_Next,
   --                       Has_Element => Param_Spec_List_Has_Element,
   --                       Element     => Param_Spec_List_Element);
   --  List of ParamSpec.

   type Params is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Params);
   --  List of parameter specifications.

   type Paren_Abstract_State_Decl is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Paren_Abstract_State_Decl);
   --  Holds an AbstractStateDecl between parentheses. Needed to support the
   --  syntax:
   --
   --  .. code:: ada
   --
   --  package Pkg with Abstract_State => (A, (B with Some_Aspect))

   type Paren_Expr is new Expr with private;
   overriding
   procedure Process 
     (This : in Paren_Expr);
   --  Parenthesized expression.

   --  type Parent_List is new Name_List with private;
   --  List of parents in a type declaration.
   --
   --  This list node can contain one of the following nodes:
   --  * Char_Literal
   --  * Dotted_Name
   --  * Identifier
   --  * String_Literal

   type Pragma_Argument_Assoc is new Base_Assoc with private;
   overriding
   procedure Process 
     (This : in Pragma_Argument_Assoc);
   --  Argument assocation in a pragma.

   type Pragma_Node is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Pragma_Node);
   --  Class for pragmas (RM 2.8). Pragmas are compiler directives, that can

   --  be language or compiler defined.
   --  type Pragma_Node_List is new Ada_List with private
   --     with Iterable => (First       => Pragma_Node_List_First,
   --                       Next        => Pragma_Node_List_Next,
   --                       Has_Element => Pragma_Node_List_Has_Element,
   --                       Element     => Pragma_Node_List_Element);
   --  List of Pragma.

   type Private_Node is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Private_Node);
   --  Qualifier for the ``private`` keyword.

   type Private_Absent is new Private_Node with private;
   overriding
   procedure Process 
     (This : in Private_Absent);
      
   type Private_Part is new Declarative_Part with private;
   overriding
   procedure Process 
     (This : in Private_Part);

   --  List of declarations in a private part.
   type Private_Present is new Private_Node with private;
   overriding
   procedure Process 
     (This : in Private_Present);

   type Private_Type_Def is new Type_Def with private;
   overriding
   procedure Process 
     (This : in Private_Type_Def);
   --  Type definition for a private type.

   type Protected_Node is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Protected_Node);
   --  Qualifier for the ``protected`` keyword.

   type Protected_Absent is new Protected_Node with private;
   overriding
   procedure Process 
     (This : in Protected_Absent);

   type Protected_body is new Body_Node with private;
   overriding
   procedure Process 
     (This : in Protected_body);
   --  Protected object body.

   type Protected_Body_Stub is new Body_Stub with private;
   overriding
   procedure Process 
     (This : in Protected_Body_Stub);
   --  Stub for a protected object body (``is separate``).

   type Protected_Def is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Protected_Def);
   --  Type definition for a protected object.

   type Protected_Present is new Protected_Node with private;
   overriding
   procedure Process 
     (This : in Protected_Present);

   type Protected_Type_Decl is new Base_Type_Decl with private;
   overriding
   procedure Process 
     (This : in Protected_Type_Decl);
   --  Declaration for a protected type.

   type Public_Part is new Declarative_Part with private;
   overriding
   procedure Process 
     (This : in Public_Part);
   --  List of declarations in a public part.

   type Qual_Expr is new Name with private;
   overriding
   procedure Process 
     (This : in Qual_Expr);
   --  Qualified expression (``...'(...)``).

   type Quantified_Expr is new Expr with private;
   overriding
   procedure Process 
     (This : in Quantified_Expr);
   --  Quantified expression.

   type Quantifier is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Quantifier);
   --  Type for quantified expressions.

   type Quantifier_all is new Quantifier with private;
   overriding
   procedure Process 
     (This : in Quantifier_all);

   type Quantifier_some is new Quantifier with private;
   overriding
   procedure Process 
     (This : in Quantifier_some);

   type Raise_Expr is new Expr with private;
   overriding
   procedure Process 
     (This : in Raise_Expr);
   --  Expression to raise an exception.

   type Raise_Stmt is new Simple_Stmt with private;
   overriding
   procedure Process 
     (This : in Raise_Stmt);
   --  ``raise`` statement.

   type Range_Constraint is new Constraint with private;
   overriding
   procedure Process 
     (This : in Range_Constraint);
   --  Range-based type constraint.

   type Range_Spec is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Range_Spec);
   --  Range specification.

   type Real_Literal is new Num_Literal with private;
   overriding
   procedure Process 
     (This : in Real_Literal);
   --  Literal for a real number.

   type Record_Def is new Base_Record_Def with private;
   overriding
   procedure Process 
     (This : in Record_Def);
   --  Record definition that contains components (``record ... end
   --  record``).

   type Record_Rep_Clause is new Aspect_Clause with private;
   overriding
   procedure Process 
     (This : in Record_Rep_Clause);
   --  Representation clause for a record type.

   type Record_Type_Def is new Type_Def with private;
   overriding
   procedure Process 
     (This : in Record_Type_Def);
   --  Type definition for a record.

   type Relation_Op is new Bin_Op with private;
   overriding
   procedure Process 
     (This : in Relation_Op);
   --  Binary operation that compares two value, producing a boolean.

   type Renaming_Clause is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Renaming_Clause);
   --  Renaming clause, used everywhere renamings are valid.

   type Requeue_Stmt is new Simple_Stmt with private;
   overriding
   procedure Process 
     (This : in Requeue_Stmt);
   --  ``requeue`` statement.

   type Return_Stmt is new Simple_Stmt with private;
   overriding
   procedure Process 
     (This : in Return_Stmt);
   --  ``return`` statement.

   type Reverse_Node is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Reverse_Node);
   --  Qualifier for the ``reverse`` keyword.

   type Reverse_Absent is new Reverse_Node with private;
   overriding
   procedure Process 
     (This : in Reverse_Absent);

   type Reverse_Present is new Reverse_Node with private;
   overriding
   procedure Process 
     (This : in Reverse_Present);

   type Select_Stmt is new Composite_Stmt with private;
   overriding
   procedure Process 
     (This : in Select_Stmt);
   --  ``select`` statements block.

   type Select_When_Part is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Select_When_Part);
   --  Alternative part in a ``select`` statements block.
   --  type Select_When_Part_List is new Ada_List with private
   --     with Iterable => (First       => Select_When_Part_List_First,
   --                       Next        => Select_When_Part_List_Next,
   --                       Has_Element => Select_When_Part_List_Has_Element,
   --                       Element     => Select_When_Part_List_Element);
   --  List of SelectWhenPart.

   type Signed_Int_Type_Def is new Type_Def with private;
   overriding
   procedure Process 
     (This : in Signed_Int_Type_Def);
   --  Type definition for a signed integer type.

   type Single_Protected_Decl is new Basic_Decl with private;
   overriding
   procedure Process 
     (This : in Single_Protected_Decl);
   --  Declaration for a single protected object.

   type Single_Task_Decl is new Basic_Decl with private;
   overriding
   procedure Process 
     (This : in Single_Task_Decl);
   --  Declaration for a single task.

   type Task_Type_Decl is new Base_Type_Decl with private;
   overriding
   procedure Process 
     (This : in Task_Type_Decl);
   --  Declaration for a task type.

   type Single_Task_Type_Decl is new Task_Type_Decl with private;
   overriding
   procedure Process 
     (This : in Single_Task_Type_Decl);
   --  Type declaration for a single task.
   --  type Stmt_List is new Ada_Node_List with private;
   --  List of statements.
   --
   --  This list node can contain one of the following nodes:
   --  * Pragma_Node
   --  * Stmt

   type String_Literal is new Base_Id with private;
   overriding
   procedure Process 
     (This : in String_Literal);
   --  String literal.

   type Subp_body is new Base_Subp_body with private;
   overriding
   procedure Process 
     (This : in Subp_body);
   --  Subprogram body.

   type Subp_Body_Stub is new Body_Stub with private;
   overriding
   procedure Process 
     (This : in Subp_Body_Stub);
   --  Stub for a subprogram body (``is separate``).

   type Subp_Decl is new Classic_Subp_Decl with private;
   overriding
   procedure Process 
     (This : in Subp_Decl);
   --  Regular subprogram declaration.

   type Subp_Kind is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Subp_Kind);
   --  Qualifier for a subprogram kind.

   type Subp_Kind_function is new Subp_Kind with private;
   overriding
   procedure Process 
     (This : in Subp_Kind_function);

   type Subp_Kind_procedure is new Subp_Kind with private;
   overriding
   procedure Process 
     (This : in Subp_Kind_procedure);

   type Subp_Renaming_Decl is new Base_Subp_body with private;
   overriding
   procedure Process 
     (This : in Subp_Renaming_Decl);
   --  Declaration for a subprogram renaming.

   type Subp_Spec is new Base_Subp_Spec with private;
   overriding
   procedure Process 
     (This : in Subp_Spec);
   --  Subprogram specification.

   type Subtype_Decl is new Base_Subtype_Decl with private;
   overriding
   procedure Process 
     (This : in Subtype_Decl);
   --  Subtype declaration.

   type Subunit is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Subunit);
   --  Subunit (``separate``).

   type Synchronized_Node is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Synchronized_Node);
   --  Qualifier for the ``synchronized`` keyword.

   type Synchronized_Absent is new Synchronized_Node with private;
   overriding
   procedure Process 
     (This : in Synchronized_Absent);

   type Synchronized_Present is new Synchronized_Node with private;
   overriding
   procedure Process 
     (This : in Synchronized_Present);

   type Synth_Anonymous_Type_Decl is new Anonymous_Type_Decl with private;
   overriding
   procedure Process 
     (This : in Synth_Anonymous_Type_Decl);
   --  Synthetic anonymous type decl. Used to generate anonymous access
   --  types.

   type Synthetic_Renaming_Clause is new Renaming_Clause with private;
   overriding
   procedure Process 
     (This : in Synthetic_Renaming_Clause);
   --  Synthetic renaming clause. Used to synthesize object decls with
   --  renamings. (See to_anonymous_object_decl).

   type Tagged_Node is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Tagged_Node);
   --  Qualifier for the ``tagged`` keyword.

   type Tagged_Absent is new Tagged_Node with private;
   overriding
   procedure Process 
     (This : in Tagged_Absent);

   type Tagged_Present is new Tagged_Node with private;
   overriding
   procedure Process 
     (This : in Tagged_Present);

   type Target_Name is new Name with private;
   overriding
   procedure Process 
     (This : in Target_Name);
   --  Name for Ada 2020 ``@``.

   type Task_body is new Body_Node with private;
   overriding
   procedure Process 
     (This : in Task_body);
   --  Task body.

   type Task_Body_Stub is new Body_Stub with private;
   overriding
   procedure Process 
     (This : in Task_Body_Stub);
   --  Stub for a task body (``is separate``).

   type Task_Def is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Task_Def);
   --  Type definition for a task type.

   type Terminate_Alternative is new Simple_Stmt with private;
   overriding
   procedure Process 
     (This : in Terminate_Alternative);
   --  ``terminate`` alternative in a ``select`` statement.

   type Type_Access_Def is new Base_Type_Access_Def with private;
   overriding
   procedure Process 
     (This : in Type_Access_Def);
   --  Syntactic type definition for accesses.

   type Un_Op is new Expr with private;
   overriding
   procedure Process 
     (This : in Un_Op);
   --  Unary expression.

   type Unconstrained_Array_Index is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Unconstrained_Array_Index);
   --  List of unconstrained array indexes.
   
   --  type Unconstrained_Array_Index_List is new Ada_List with private
   --     with Iterable => (First       => Unconstrained_Array_Index_List_First,
   --                       Next        => Unconstrained_Array_Index_List_Next,
   --                       Has_Element => Unconstrained_Array_Index_List_Has_Element,
   --                       Element     => Unconstrained_Array_Index_List_Element);
   --  List of UnconstrainedArrayIndex.

   type Unconstrained_Array_Indices is new Array_Indices with private;
   overriding
   procedure Process 
     (This : in Unconstrained_Array_Indices);
   --  Unconstrained specification for array indexes.

   type Unknown_Discriminant_Part is new Discriminant_Part with private;
   overriding
   procedure Process 
     (This : in Unknown_Discriminant_Part);
   --  Unknown list of discriminants in type declarations.

   type Until_Node is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Until_Node);
   --  Qualifier for the ``until`` keyword.

   type Until_Absent is new Until_Node with private;
   overriding
   procedure Process 
     (This : in Until_Absent);
      
   type Until_Present is new Until_Node with private;
   overriding
   procedure Process 
     (This : in Until_Present);

   type Update_Attribute_Ref is new Attribute_Ref with private;
   overriding
   procedure Process 
     (This : in Update_Attribute_Ref);
   --  Reference to the ``Update`` attribute.

   type Use_Clause is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Use_Clause);
   --  Base class for use clauses.
   
   type Use_Package_Clause is new Use_Clause with private;
   overriding
   procedure Process 
     (This : in Use_Package_Clause);
   --  Use clause for packages.

   type Use_Type_Clause is new Use_Clause with private;
   overriding
   procedure Process 
     (This : in Use_Type_Clause);
   --  Use clause for types.

   type Variant is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Variant);
   --  Single variant in a discriminated type record declaration.
   --
   --  This corresponds to a ``when ... => ...`` section in a variant part.
   
   --  type Variant_List is new Ada_List with private
   --     with Iterable => (First       => Variant_List_First,
   --                       Next        => Variant_List_Next,
   --                       Has_Element => Variant_List_Has_Element,
   --                       Element     => Variant_List_Element);
   --  List of Variant.

   type Variant_Part is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in Variant_Part);
   --  Variant part in a discriminated type record declaration.
   --
   --  This corresponds to the whole ``case ... is ... end case;`` block.

   type While_Loop_Spec is new Loop_Spec with private;
   overriding
   procedure Process 
     (This : in While_Loop_Spec);
   --  Specification for a ``while`` loop.

   type While_Loop_Stmt is new Base_Loop_Stmt with private;
   overriding
   procedure Process 
     (This : in While_Loop_Stmt);
   --  Statement for ``while`` loops (``while ... loop ... end loop;``).

   type With_Clause is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in With_Clause);
   --  With clause.

   type With_private is new Ada_Node with private;
   overriding
   procedure Process 
     (This : in With_private);
   --  Qualifier for the ``private`` keyword in ``with private`` record
   --  clauses.

   type With_Private_Absent is new With_private with private;
   overriding
   procedure Process 
     (This : in With_Private_Absent);

   type With_Private_Present is new With_private with private;
   overriding
   procedure Process 
     (This : in With_Private_Present);
   
   --------------------------------------------------
   -- End of types replicating LAL Ada_Node hierarchy
   --------------------------------------------------
   
private
   type Ada_Node is tagged record
      null;
   end record;

   type Expr is new Ada_Node with null record;

   type Basic_Decl is new Ada_Node with null record;

   type Ada_List is new Ada_Node with null record;

   type Ada_Node_List is new Ada_List with null record;

   type Alternatives_List is new Ada_Node_List with null record;

   type Name is new Expr with null record;

   type Single_Tok_Node is new Name with null record;

   type Base_Id is new Single_Tok_Node with null record;

   type Identifier is new Base_Id with null record;

   type Abort_Node is new Ada_Node with null record;

   type Abort_Absent is new Abort_Node with null record;

   type Abort_Present is new Abort_Node with null record;

   type Stmt is new Ada_Node with null record;

   type Simple_Stmt is new Stmt with null record;

   type Abort_Stmt is new Simple_Stmt with null record;

   type Abstract_Node is new Ada_Node with null record;

   type Abstract_Absent is new Abstract_Node with null record;

   type Basic_Subp_Decl is new Basic_Decl with null record;

   type Classic_Subp_Decl is new Basic_Subp_Decl with null record;

   type Formal_Subp_Decl is new Classic_Subp_Decl with null record;

   type Abstract_Formal_Subp_Decl is new Formal_Subp_Decl with null record;

   type Abstract_Present is new Abstract_Node with null record;

   type Abstract_State_Decl is new Basic_Decl with null record;

   type Abstract_State_Decl_Expr is new Expr with null record;

   type Abstract_State_Decl_List is new Ada_Node_List with null record;

   type Abstract_Subp_Decl is new Classic_Subp_Decl with null record;

   type Composite_Stmt is new Stmt with null record;

   type Accept_Stmt is new Composite_Stmt with null record;

   type Accept_Stmt_With_Stmts is new Accept_Stmt with null record;

   type Type_Def is new Ada_Node with null record;

   type Access_Def is new Type_Def with null record;

   type Access_To_Subp_Def is new Access_Def with null record;

   type Base_Aggregate is new Expr with null record;

   type Aggregate is new Base_Aggregate with null record;

   type Basic_Assoc is new Ada_Node with null record;

   type Aggregate_Assoc is new Basic_Assoc with null record;

   type Aliased_Node is new Ada_Node with null record;

   type Aliased_Absent is new Aliased_Node with null record;

   type Aliased_Present is new Aliased_Node with null record;

   type All_Node is new Ada_Node with null record;

   type All_Absent is new All_Node with null record;

   type All_Present is new All_Node with null record;

   type Allocator is new Expr with null record;

   type Anonymous_Expr_Decl is new Basic_Decl with null record;

   type Type_Expr is new Ada_Node with null record;

   type Anonymous_Type is new Type_Expr with null record;

   type Base_Type_Access_Def is new Access_Def with null record;

   type Anonymous_Type_Access_Def is new Base_Type_Access_Def with null record;

   type Base_Type_Decl is new Basic_Decl with null record;

   type Type_Decl is new Base_Type_Decl with null record;

   type Anonymous_Type_Decl is new Type_Decl with null record;

   type Array_Indices is new Ada_Node with null record;

   type Array_Type_Def is new Type_Def with null record;

   type Aspect_Assoc is new Ada_Node with null record;

   type Aspect_Assoc_List is new Ada_List with null record;

   type Aspect_Clause is new Ada_Node with null record;

   type Aspect_Spec is new Ada_Node with null record;

   type Assign_Stmt is new Simple_Stmt with null record;

   type Basic_Assoc_List is new Ada_List with null record;

   type Assoc_List is new Basic_Assoc_List with null record;

   type At_Clause is new Aspect_Clause with null record;

   type Attribute_Def_Clause is new Aspect_Clause with null record;

   type Attribute_Ref is new Name with null record;

   type Base_Assoc is new Ada_Node with null record;

   type Base_Assoc_List is new Ada_List with null record;

   type Base_Formal_Param_Decl is new Basic_Decl with null record;

   type Base_Formal_Param_Holder is new Ada_Node with null record;

   type Base_Loop_Stmt is new Composite_Stmt with null record;

   type Base_Package_Decl is new Basic_Decl with null record;

   type Base_Record_Def is new Ada_Node with null record;

   type Body_Node is new Basic_Decl with null record;

   type Base_Subp_Body is new Body_Node with null record;

   type Base_Subp_Spec is new Base_Formal_Param_Holder with null record;

   type Base_Subtype_Decl is new Base_Type_Decl with null record;

   type Basic_Decl_List is new Ada_List with null record;

   type Block_Stmt is new Composite_Stmt with null record;

   type Begin_Block is new Block_Stmt with null record;

   type Bin_Op is new Expr with null record;

   type Body_Stub is new Body_Node with null record;

   type Box_Expr is new Expr with null record;

   type Bracket_Aggregate is new Aggregate with null record;

   type Delta_Aggregate is new Base_Aggregate with null record;

   type Bracket_Delta_Aggregate is new Delta_Aggregate with null record;

   type Call_Expr is new Name with null record;

   type Call_Stmt is new Simple_Stmt with null record;

   type Cond_Expr is new Expr with null record;

   type Case_Expr is new Cond_Expr with null record;

   type Case_Expr_Alternative is new Expr with null record;

   type Case_Expr_Alternative_List is new Ada_List with null record;

   type Case_Stmt is new Composite_Stmt with null record;

   type Case_Stmt_Alternative is new Ada_Node with null record;

   type Case_Stmt_Alternative_List is new Ada_List with null record;

   type Char_Literal is new Base_Id with null record;

   type Classwide_Type_Decl is new Base_Type_Decl with null record;

   type Compilation_Unit is new Ada_Node with null record;

   type Compilation_Unit_List is new Ada_List with null record;

   type Component_Clause is new Ada_Node with null record;

   type Component_Decl is new Base_Formal_Param_Decl with null record;

   type Component_Def is new Ada_Node with null record;

   type Component_List is new Base_Formal_Param_Holder with null record;

   type Concrete_Formal_Subp_Decl is new Formal_Subp_Decl with null record;

   type Constant_Node is new Ada_Node with null record;

   type Constant_Absent is new Constant_Node with null record;

   type Constant_Present is new Constant_Node with null record;

   type Constrained_Array_Indices is new Array_Indices with null record;

   type Subtype_Indication is new Type_Expr with null record;

   type Constrained_Subtype_Indication is new Subtype_Indication with null record;

   type Constraint is new Ada_Node with null record;

   type Constraint_List is new Ada_Node_List with null record;

   type Contract_Case_Assoc is new Base_Assoc with null record;

   type Contract_Case_Assoc_List is new Ada_List with null record;

   type Contract_Cases is new Expr with null record;

   type Real_Type_Def is new Type_Def with null record;

   type Decimal_Fixed_Point_Def is new Real_Type_Def with null record;

   type Decl_Block is new Block_Stmt with null record;

   type Decl_Expr is new Expr with null record;

   type Decl_List is new Ada_Node_List with null record;

   type Declarative_Part is new Ada_Node with null record;

   type Defining_Name is new Name with null record;

   type Defining_Name_List is new Ada_List with null record;

   type Delay_Stmt is new Simple_Stmt with null record;

   type Delta_Constraint is new Constraint with null record;

   type Derived_Type_Def is new Type_Def with null record;

   type Digits_Constraint is new Constraint with null record;

   type Discrete_Base_Subtype_Decl is new Base_Subtype_Decl with null record;

   type Discrete_Subtype_Indication is new Subtype_Indication with null record;

   type Discrete_Subtype_Name is new Name with null record;

   type Discriminant_Assoc is new Basic_Assoc with null record;

   type Identifier_List is new Ada_List with null record;

   type Discriminant_Choice_List is new Identifier_List with null record;

   type Discriminant_Constraint is new Constraint with null record;

   type Discriminant_Part is new Base_Formal_Param_Holder with null record;

   type Discriminant_Spec is new Base_Formal_Param_Decl with null record;

   type Discriminant_Spec_List is new Ada_List with null record;

   type Dotted_Name is new Name with null record;

   type Elsif_Expr_Part is new Ada_Node with null record;

   type Elsif_Expr_Part_List is new Ada_List with null record;

   type Elsif_Stmt_Part is new Ada_Node with null record;

   type Elsif_Stmt_Part_List is new Ada_List with null record;

   type End_Name is new Name with null record;

   type Entry_Body is new Body_Node with null record;

   type Entry_Completion_Formal_Params is new Base_Formal_Param_Holder with null record;

   type Entry_Decl is new Basic_Subp_Decl with null record;

   type Entry_Index_Spec is new Basic_Decl with null record;

   type Entry_Spec is new Base_Subp_Spec with null record;

   type Enum_Lit_Synth_Type_Expr is new Type_Expr with null record;

   type Enum_Literal_Decl is new Basic_Subp_Decl with null record;

   type Enum_Literal_Decl_List is new Ada_List with null record;

   type Enum_Rep_Clause is new Aspect_Clause with null record;

   type Enum_Subp_Spec is new Base_Subp_Spec with null record;

   type Enum_Type_Def is new Type_Def with null record;

   type Error_Decl is new Basic_Decl with null record;

   type Error_Stmt is new Stmt with null record;

   type Exception_Decl is new Basic_Decl with null record;

   type Exception_Handler is new Basic_Decl with null record;

   type Exit_Stmt is new Simple_Stmt with null record;

   type Explicit_Deref is new Name with null record;

   type Expr_List is new Ada_List with null record;

   type Expr_Alternatives_List is new Expr_List with null record;

   type Expr_Function is new Base_Subp_Body with null record;

   type Extended_Return_Stmt is new Composite_Stmt with null record;

   type Object_Decl is new Basic_Decl with null record;

   type Extended_Return_Stmt_Object_Decl is new Object_Decl with null record;

   type Floating_Point_Def is new Real_Type_Def with null record;

   type Loop_Spec is new Ada_Node with null record;

   type For_Loop_Spec is new Loop_Spec with null record;

   type For_Loop_Stmt is new Base_Loop_Stmt with null record;

   type For_Loop_Var_Decl is new Basic_Decl with null record;

   type Formal_Discrete_Type_Def is new Type_Def with null record;

   type Generic_Decl is new Basic_Decl with null record;

   type Generic_Formal is new Base_Formal_Param_Decl with null record;

   type Generic_Formal_Obj_Decl is new Generic_Formal with null record;

   type Generic_Formal_Package is new Generic_Formal with null record;

   type Generic_Formal_Part is new Base_Formal_Param_Holder with null record;

   type Generic_Formal_Subp_Decl is new Generic_Formal with null record;

   type Generic_Formal_Type_Decl is new Generic_Formal with null record;

   type Generic_Instantiation is new Basic_Decl with null record;

   type Generic_Package_Decl is new Generic_Decl with null record;

   type Generic_Package_Instantiation is new Generic_Instantiation with null record;

   type Generic_Package_Internal is new Base_Package_Decl with null record;

   type Generic_Renaming_Decl is new Basic_Decl with null record;

   type Generic_Package_Renaming_Decl is new Generic_Renaming_Decl with null record;

   type Generic_Subp_Decl is new Generic_Decl with null record;

   type Generic_Subp_Instantiation is new Generic_Instantiation with null record;

   type Generic_Subp_Internal is new Basic_Subp_Decl with null record;

   type Generic_Subp_Renaming_Decl is new Generic_Renaming_Decl with null record;

   type Goto_Stmt is new Simple_Stmt with null record;

   type Handled_Stmts is new Ada_Node with null record;

   type If_Expr is new Cond_Expr with null record;

   type If_Stmt is new Composite_Stmt with null record;

   type Incomplete_Type_Decl is new Base_Type_Decl with null record;

   type Incomplete_Tagged_Type_Decl is new Incomplete_Type_Decl with null record;

   type Index_Constraint is new Constraint with null record;

   type Num_Literal is new Single_Tok_Node with null record;

   type Int_Literal is new Num_Literal with null record;

   type Interface_Kind is new Ada_Node with null record;

   type Interface_Kind_Limited is new Interface_Kind with null record;

   type Interface_Kind_Protected is new Interface_Kind with null record;

   type Interface_Kind_Synchronized is new Interface_Kind with null record;

   type Interface_Kind_Task is new Interface_Kind with null record;

   type Interface_Type_Def is new Type_Def with null record;

   type Iter_Type is new Ada_Node with null record;

   type Iter_Type_In is new Iter_Type with null record;

   type Iter_Type_Of is new Iter_Type with null record;

   type Iterated_Assoc is new Basic_Assoc with null record;

   type Known_Discriminant_Part is new Discriminant_Part with null record;

   type Label is new Simple_Stmt with null record;

   type Label_Decl is new Basic_Decl with null record;

   type Library_Item is new Ada_Node with null record;

   type Limited_Node is new Ada_Node with null record;

   type Limited_Absent is new Limited_Node with null record;

   type Limited_Present is new Limited_Node with null record;

   type Loop_Stmt is new Base_Loop_Stmt with null record;

   type Membership_Expr is new Expr with null record;

   type Mod_Int_Type_Def is new Type_Def with null record;

   type Mode is new Ada_Node with null record;

   type Mode_Default is new Mode with null record;

   type Mode_In is new Mode with null record;

   type Mode_In_Out is new Mode with null record;

   type Mode_Out is new Mode with null record;

   type Multi_Abstract_State_Decl is new Ada_Node with null record;

   type Multi_Dim_Array_Assoc is new Aggregate_Assoc with null record;

   type Name_List is new Ada_List with null record;

   type Named_Stmt is new Composite_Stmt with null record;

   type Named_Stmt_Decl is new Basic_Decl with null record;

   type Not_Null is new Ada_Node with null record;

   type Not_Null_Absent is new Not_Null with null record;

   type Not_Null_Present is new Not_Null with null record;

   type Null_Component_Decl is new Ada_Node with null record;

   type Null_Literal is new Single_Tok_Node with null record;

   type Null_Record_Aggregate is new Base_Aggregate with null record;

   type Null_Record_Def is new Base_Record_Def with null record;

   type Null_Stmt is new Simple_Stmt with null record;

   type Null_Subp_Decl is new Base_Subp_Body with null record;

   type Number_Decl is new Basic_Decl with null record;

   type Op is new Base_Id with null record;

   type Op_Abs is new Op with null record;

   type Op_And is new Op with null record;

   type Op_And_Then is new Op with null record;

   type Op_Concat is new Op with null record;

   type Op_Div is new Op with null record;

   type Op_Double_Dot is new Op with null record;

   type Op_Eq is new Op with null record;

   type Op_Gt is new Op with null record;

   type Op_Gte is new Op with null record;

   type Op_In is new Op with null record;

   type Op_Lt is new Op with null record;

   type Op_Lte is new Op with null record;

   type Op_Minus is new Op with null record;

   type Op_Mod is new Op with null record;

   type Op_Mult is new Op with null record;

   type Op_Neq is new Op with null record;

   type Op_Not is new Op with null record;

   type Op_Not_In is new Op with null record;

   type Op_Or is new Op with null record;

   type Op_Or_Else is new Op with null record;

   type Op_Plus is new Op with null record;

   type Op_Pow is new Op with null record;

   type Op_Rem is new Op with null record;

   type Op_Xor is new Op with null record;

   type Ordinary_Fixed_Point_Def is new Real_Type_Def with null record;

   type Others_Designator is new Ada_Node with null record;

   type Overriding_Node is new Ada_Node with null record;

   type Overriding_Not_Overriding is new Overriding_Node with null record;

   type Overriding_Overriding is new Overriding_Node with null record;

   type Overriding_Unspecified is new Overriding_Node with null record;

   type Package_Body is new Body_Node with null record;

   type Package_Body_Stub is new Body_Stub with null record;

   type Package_Decl is new Base_Package_Decl with null record;

   type Package_Renaming_Decl is new Basic_Decl with null record;

   type Param_Assoc is new Basic_Assoc with null record;

   type Param_Spec is new Base_Formal_Param_Decl with null record;

   type Param_Spec_List is new Ada_List with null record;

   type Params is new Ada_Node with null record;

   type Paren_Abstract_State_Decl is new Ada_Node with null record;

   type Paren_Expr is new Expr with null record;

   type Parent_List is new Name_List with null record;

   type Pragma_Argument_Assoc is new Base_Assoc with null record;

   type Pragma_Node is new Ada_Node with null record;

   type Pragma_Node_List is new Ada_List with null record;

   type Private_Node is new Ada_Node with null record;

   type Private_Absent is new Private_Node with null record;

   type Private_Part is new Declarative_Part with null record;

   type Private_Present is new Private_Node with null record;

   type Private_Type_Def is new Type_Def with null record;

   type Protected_Node is new Ada_Node with null record;

   type Protected_Absent is new Protected_Node with null record;

   type Protected_Body is new Body_Node with null record;

   type Protected_Body_Stub is new Body_Stub with null record;

   type Protected_Def is new Ada_Node with null record;

   type Protected_Present is new Protected_Node with null record;

   type Protected_Type_Decl is new Base_Type_Decl with null record;

   type Public_Part is new Declarative_Part with null record;

   type Qual_Expr is new Name with null record;

   type Quantified_Expr is new Expr with null record;

   type Quantifier is new Ada_Node with null record;

   type Quantifier_All is new Quantifier with null record;

   type Quantifier_Some is new Quantifier with null record;

   type Raise_Expr is new Expr with null record;

   type Raise_Stmt is new Simple_Stmt with null record;

   type Range_Constraint is new Constraint with null record;

   type Range_Spec is new Ada_Node with null record;

   type Real_Literal is new Num_Literal with null record;

   type Record_Def is new Base_Record_Def with null record;

   type Record_Rep_Clause is new Aspect_Clause with null record;

   type Record_Type_Def is new Type_Def with null record;

   type Relation_Op is new Bin_Op with null record;

   type Renaming_Clause is new Ada_Node with null record;

   type Requeue_Stmt is new Simple_Stmt with null record;

   type Return_Stmt is new Simple_Stmt with null record;

   type Reverse_Node is new Ada_Node with null record;

   type Reverse_Absent is new Reverse_Node with null record;

   type Reverse_Present is new Reverse_Node with null record;

   type Select_Stmt is new Composite_Stmt with null record;

   type Select_When_Part is new Ada_Node with null record;

   type Select_When_Part_List is new Ada_List with null record;

   type Signed_Int_Type_Def is new Type_Def with null record;

   type Single_Protected_Decl is new Basic_Decl with null record;

   type Single_Task_Decl is new Basic_Decl with null record;

   type Task_Type_Decl is new Base_Type_Decl with null record;

   type Single_Task_Type_Decl is new Task_Type_Decl with null record;

   type Stmt_List is new Ada_Node_List with null record;

   type String_Literal is new Base_Id with null record;

   type Subp_Body is new Base_Subp_Body with null record;

   type Subp_Body_Stub is new Body_Stub with null record;

   type Subp_Decl is new Classic_Subp_Decl with null record;

   type Subp_Kind is new Ada_Node with null record;

   type Subp_Kind_Function is new Subp_Kind with null record;

   type Subp_Kind_Procedure is new Subp_Kind with null record;

   type Subp_Renaming_Decl is new Base_Subp_Body with null record;

   type Subp_Spec is new Base_Subp_Spec with null record;

   type Subtype_Decl is new Base_Subtype_Decl with null record;

   type Subunit is new Ada_Node with null record;

   type Synchronized_Node is new Ada_Node with null record;

   type Synchronized_Absent is new Synchronized_Node with null record;

   type Synchronized_Present is new Synchronized_Node with null record;

   type Synth_Anonymous_Type_Decl is new Anonymous_Type_Decl with null record;

   type Synthetic_Renaming_Clause is new Renaming_Clause with null record;

   type Tagged_Node is new Ada_Node with null record;

   type Tagged_Absent is new Tagged_Node with null record;

   type Tagged_Present is new Tagged_Node with null record;

   type Target_Name is new Name with null record;

   type Task_Body is new Body_Node with null record;

   type Task_Body_Stub is new Body_Stub with null record;

   type Task_Def is new Ada_Node with null record;

   type Terminate_Alternative is new Simple_Stmt with null record;

   type Type_Access_Def is new Base_Type_Access_Def with null record;

   type Un_Op is new Expr with null record;

   type Unconstrained_Array_Index is new Ada_Node with null record;

   type Unconstrained_Array_Index_List is new Ada_List with null record;

   type Unconstrained_Array_Indices is new Array_Indices with null record;

   type Unknown_Discriminant_Part is new Discriminant_Part with null record;

   type Until_Node is new Ada_Node with null record;

   type Until_Absent is new Until_Node with null record;

   type Until_Present is new Until_Node with null record;

   type Update_Attribute_Ref is new Attribute_Ref with null record;

   type Use_Clause is new Ada_Node with null record;

   type Use_Package_Clause is new Use_Clause with null record;

   type Use_Type_Clause is new Use_Clause with null record;

   type Variant is new Ada_Node with null record;

   type Variant_List is new Ada_List with null record;

   type Variant_Part is new Ada_Node with null record;

   type While_Loop_Spec is new Loop_Spec with null record;

   type While_Loop_Stmt is new Base_Loop_Stmt with null record;

   type With_Clause is new Ada_Node with null record;

   type With_Private is new Ada_Node with null record;

   type With_Private_Absent is new With_Private with null record;

   type With_Private_Present is new With_Private with null record;

   --------------------------------------------------
   -- End of types replicating LAL Ada_Node hierarchy
   --------------------------------------------------

   type Ada_Node_Access_Class is access Ada_Node'Class;
   
   function To_Ada_Node
     (This : in LAL.Ada_Node'Class)
     return Ada_Node_Access_Class;

end LAL_Adapter.AST_Nodes;
