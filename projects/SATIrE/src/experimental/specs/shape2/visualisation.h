/* {DFI} is replaced with o_ShapeGraphLifted or o_ShapeGraphSetLifted
 * for srw98 or nnh99, respectively.
 * this is done in the Makefile.
 **/
extern "C" void dfi_write_(FILE *fp, KFG kfg, char *name, char *attrib, {DFI} info, int id, int insnum, int ctx);
extern "C" int  dfi_write_ShapeGraph_fp(FILE *fp, char *name, int n_graphs,  char *attrib, o_ShapeGraph sg);
extern "C" void dfi_write_Node_fp(FILE *fp, o_Node node);
