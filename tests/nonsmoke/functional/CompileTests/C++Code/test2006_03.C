#if 0
The following code:
class Node{
  public:
    Node ()  { label_count; }
    static int label_count;
  };

compiled with ROSE gives the following assertion:

identityTranslator:
../../../../NEW_ROSE/src/frontend/EDG_SAGE_Connection/sage_gen_be.C:12636:
SgExpression* sage_gen_lvalue(an_expr_node*): Assertion
"node->variant.variable->source_corresp.sg_symbol_ptr != __null" failed.
/home/saebjornsen1/links/rose-rose-compile/g++3.3.3: line 4: 32382 Aborted

/home/saebjornsen1/ROSE-October/gcc3.3.3/exampleTranslators/documentedExamples/simpleTranslatorExamples/identityTranslator -fpermissive $@

#endif


class Node
   {
     public:
          Node ()  { label_count; }
          static int label_count;
   };

