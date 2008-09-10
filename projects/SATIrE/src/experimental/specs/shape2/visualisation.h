extern "C" void dfi_write_(FILE *fp, KFG kfg, char *name, char *attrib, o_ShapeGraphSetLifted info, int id, int insnum, int ctx);
extern "C" int  dfi_write_ShapeGraph_fp(FILE *fp, char *name, int n_graphs,  char *attrib, o_ShapeGraph sg);
extern "C" void dfi_write_Node_fp(FILE *fp, o_Node node);
