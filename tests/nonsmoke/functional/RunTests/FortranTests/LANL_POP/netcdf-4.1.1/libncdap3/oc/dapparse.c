/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

#include "dapparselex.h"

/* Forward */

static void addedges(OCnode* node);
static int isglobalname(char* name);
static OCnode* newocnode(char* name, OCtype octype, DAPparsestate* state);
static OCtype octypefor(Object etype);
static char* scopeduplicates(OClist* list);
static int check_int32(char* val, long* value);

/****************************************************/

/* Switch to DAS parsing SCAN_WORD definition */
void
dassetup(DAPparsestate* state)
{
    setwordchars(state->lexstate,1);
}

Object
datasetbody(DAPparsestate* state, Object name, Object decls)
{
    OCnode* node = newocnode((char*)name,OC_Dataset,state);
    node->subnodes = (OClist*)decls;
    OCASSERT((state->root == NULL));
    addedges(node);
    state->root = node;
    /* make sure to cross link */
    state->root->root = state->root;
    return node;
}

Object
attributebody(DAPparsestate* state, Object attrlist)
{
    OCnode* node = newocnode(NULL,OC_Attributeset,state);
    OCASSERT((state->root == NULL));
    state->root = node;
    /* make sure to cross link */
    state->root->root = state->root;
    node->subnodes = (OClist*)attrlist;
    addedges(node);
    return node;
}

Object
errorbody(DAPparsestate* state,
	  Object code, Object msg, Object ptype, Object prog)
{
    state->svcerror = 1;
    state->code     = (code != NULL?strdup((char*)code):NULL);
    state->message  = (msg != NULL?strdup((char*)msg):NULL);
    /* Ignore ptype and prog for now */
    return NULL;
}

Object
unrecognizedresponse(DAPparsestate* state)
{
    return errorbody(state,"0",state->lexstate->input,NULL,NULL);
}

Object
declarations(DAPparsestate* state, Object decls, Object decl)
{
    OClist* alist = (OClist*)decls;
    if(alist == NULL)
	 alist = oclistnew();
    else
	oclistpush(alist,(ocelem)decl);
    return alist;
}

Object
arraydecls(DAPparsestate* state, Object arraydecls, Object arraydecl)
{
    OClist* alist = (OClist*)arraydecls;
    if(alist == NULL)
	alist = oclistnew();
    else
	oclistpush(alist,(ocelem)arraydecl);
    return alist;
}

Object
arraydecl(DAPparsestate* state, Object name, Object size)
{
    long value;
    OCnode* dim;
    if(!check_int32(size,&value))
	dap_parse_error(state,"Dimension not an integer");
    if(name != NULL)
	dim = newocnode((char*)name,OC_Dimension,state);
    else
	dim = newocnode(NULL,OC_Dimension,state);
    dim->dim.declsize = value;
    return dim;
}

Object
attrlist(DAPparsestate* state, Object attrlist, Object attrtuple)
{
    OClist* alist = (OClist*)attrlist;
    if(alist == NULL)
	alist = oclistnew();
    else {
	char* dupname;
	if(attrtuple != NULL) {/* NULL=>alias encountered, ignore */
            oclistpush(alist,(ocelem)attrtuple);
            if((dupname=scopeduplicates(alist))!=NULL) {
	        dap_parse_error(state,"Duplicate attribute names in same scope: %s",dupname);
		/* Remove this attribute */
		oclistpop(alist);
	    }
	}
    }
    return alist;
}

Object
attrvalue(DAPparsestate* state, Object valuelist, Object value, Object etype)
{
    OClist* alist = (OClist*)valuelist;
    if(alist == NULL) alist = oclistnew();
    /* Watch out for null values */
    if(value == NULL) value = "";
    oclistpush(alist,(ocelem)strdup(value));
    return alist;
}

Object
attribute(DAPparsestate* state, Object name, Object values, Object etype)
{
    OCnode* att;
    att = newocnode((char*)name,OC_Attribute,state);
    att->etype = octypefor(etype);
    att->att.values = (OClist*)values;
    return att;
}

Object
attrset(DAPparsestate* state, Object name, Object attributes)
{
    OCnode* attset;
    attset = newocnode((char*)name,OC_Attributeset,state);
    /* Check var set vs global set */
    attset->att.isglobal = isglobalname(name);
    attset->subnodes = (OClist*)attributes;
    return attset;
}

static int
isglobalname(char* name)
{
    int len = strlen(name);
    int glen = strlen("global");
    char* p;
    if(len < glen) return 0;
    p = name + (len - glen);
    if(strcasecmp(p,"global") != 0)
	return 0;
    return 1;
}

#if 0
static int
isnumber(const char* text)
{
    for(;*text;text++) {if(!isdigit(*text)) return 0;}
    return 1;
}
#endif

static void
dimension(OCnode* node, OClist* dimensions)
{
    unsigned int i;
    unsigned int rank = oclistlength(dimensions);
    node->array.dimensions = (OClist*)dimensions;
    node->array.rank = rank;
    for(i=0;i<rank;i++) {
        OCnode* dim = (OCnode*)oclistget(node->array.dimensions,i);
        dim->dim.array = node;
	dim->dim.arrayindex = i;
#if 0
	if(dim->name == NULL) {
	    dim->dim.anonymous = 1;
	    dim->name = dimnameanon(node->name,i);
	}
#endif
    }
}

char*
dimnameanon(char* basename, unsigned int index)
{
    char name[64];
    sprintf(name,"%s_%d",basename,index);
    return strdup(name);
}

Object
makebase(DAPparsestate* state, Object name, Object etype, Object dimensions)
{
    OCnode* node;
    node = newocnode((char*)name,OC_Primitive,state);
    node->etype = octypefor(etype);
    dimension(node,(OClist*)dimensions);
    return node;
}

Object
makestructure(DAPparsestate* state, Object name, Object dimensions, Object fields)
{
    OCnode* node;
    char* dupname;    
    if((dupname=scopeduplicates((OClist*)fields))!= NULL) {
        dap_parse_error(state,"Duplicate structure field names in same scope: %s.%s",(char*)name,dupname);
	return (Object)NULL;
    }
    node = newocnode(name,OC_Structure,state);
    node->subnodes = fields;
    dimension(node,(OClist*)dimensions);
    addedges(node);
    return node;
}

Object
makesequence(DAPparsestate* state, Object name, Object members)
{
    OCnode* node;
    char* dupname;    
    if((dupname=scopeduplicates((OClist*)members)) != NULL) {
        dap_parse_error(state,"Duplicate sequence member names in same scope: %s.%s",(char*)name,dupname);
	return (Object)NULL;
    }
    node = newocnode(name,OC_Sequence,state);
    node->subnodes = members;
    addedges(node);
    return node;
}

Object
makegrid(DAPparsestate* state, Object name, Object arraydecl, Object mapdecls)
{
    OCnode* node;
    /* Check for duplicate map names */
    char* dupname;    
    if((dupname=scopeduplicates((OClist*)mapdecls)) != NULL) {
        dap_parse_error(state,"Duplicate grid map names in same scope: %s.%s",(char*)name,dupname);
	return (Object)NULL;
    }
    node = newocnode(name,OC_Grid,state);
    node->subnodes = (OClist*)mapdecls;
    oclistinsert(node->subnodes,0,(ocelem)arraydecl);
    addedges(node);
    return node;
}

static void
addedges(OCnode* node)
{
    unsigned int i;
    if(node->subnodes == NULL) return;
    for(i=0;i<oclistlength(node->subnodes);i++) {
        OCnode* subnode = (OCnode*)oclistget(node->subnodes,i);
	subnode->container = node;
    }
}

int
daperror(DAPparsestate* state, const char* msg)
{
    dap_parse_error(state,msg);
    return 0;
}

static char*
flatten(char* s, char* tmp, int tlen)
{
    int c;
    char* p,*q;
    snprintf(tmp,tlen,"%s",s);
    tmp[tlen-1] = '\0';
    p = (q = tmp);
    while((c=*p++)) {
	switch (c) {
	case '\r': case '\n': break;
	case '\t': *q++ = ' '; break;
	case ' ': if(*p != ' ') *q++ = c; break;
	default: *q++ = c;
	}
    }
    *q = '\0';
    return tmp;
}

void
dap_parse_error(DAPparsestate* state, const char *fmt, ...)
{
    size_t len, suffixlen, prefixlen;
    va_list argv;
    char* tmp = NULL;
    va_start(argv,fmt);
    (void) vfprintf(stderr,fmt,argv) ;
    (void) fputc('\n',stderr) ;
    len = strlen(state->lexstate->input);
    suffixlen = strlen(state->lexstate->next);
    prefixlen = (len - suffixlen);
    tmp = (char*)ocmalloc(len+1);
    flatten(state->lexstate->input,tmp,prefixlen);
    tmp[prefixlen] = '\0';
    (void) fprintf(stderr,"context: %s",tmp);
    flatten(state->lexstate->next,tmp,suffixlen);
    tmp[suffixlen] = '\0';
    (void) fprintf(stderr,"^%s\n",tmp);
    (void) fflush(stderr);	/* to ensure log files are current */
    ocfree(tmp);
}

/* Create an ocnode and capture in the state->ocnode list */
static OCnode*
newocnode(char* name, OCtype octype, DAPparsestate* state)
{
    OCnode* node = makeocnode(name,octype,state->root);
    oclistpush(state->ocnodes,(ocelem)node);
    return node;
}

static int
check_int32(char* val, long* value)
{
    char* ptr;
    int ok = 1;
    long iv = strtol(val,&ptr,0); /* 0=>auto determine base */
    if((iv == 0 && val == ptr) || *ptr != '\0') {ok=0; iv=1;}
    else if(iv > OC_INT32_MAX || iv < OC_INT32_MIN) ok=0;
    if(value != NULL) *value = iv;
    return ok;
}

static char*
scopeduplicates(OClist* list)
{
    unsigned int i,j;
    for(i=0;i<oclistlength(list);i++) {
	OCnode* io = (OCnode*)oclistget(list,i);
        for(j=i+1;j<oclistlength(list);j++) {
	    OCnode* jo = (OCnode*)oclistget(list,j);
	    if(strcmp(io->name,jo->name)==0)
		return io->name;
	}
    }
    return NULL;
}

static OCtype
octypefor(Object etype)
{
    switch ((long)etype) {
    case SCAN_BYTE: return OC_Byte;
    case SCAN_INT16: return OC_Int16;
    case SCAN_UINT16: return OC_UInt16;
    case SCAN_INT32: return OC_Int32;
    case SCAN_UINT32: return OC_UInt32;
    case SCAN_FLOAT32: return OC_Float32;
    case SCAN_FLOAT64: return OC_Float64;
    case SCAN_URL: return OC_URL;
    case SCAN_STRING: return OC_String;
    default: abort();
    }
    return OC_NAT;
}

static void
dap_parse_cleanup(DAPparsestate* state)
{
    daplexcleanup(&state->lexstate);
    if(state->ocnodes != NULL) ocfreenodes(state->ocnodes);
    state->ocnodes = NULL;
    free(state);
}

static DAPparsestate*
dap_parse_init(char* buf)
{
    DAPparsestate* state = (DAPparsestate*)ocmalloc(sizeof(DAPparsestate)); /*ocmalloc zeros*/
    MEMCHECK(state,(DAPparsestate*)NULL);
    if(buf==NULL) {
        dap_parse_error(state,"dap_parse_init: no input buffer");
	dap_parse_cleanup(state);
	return NULL;
    }
    daplexinit(buf,&state->lexstate);
    return state;
}

/* Wrapper for dapparse */
OCerror
DAPparse(OCstate* conn, OCtree* tree, char* parsestring)
{
    DAPparsestate* state = dap_parse_init(parsestring);
    int parseresult;
    OCerror ocerr = OC_NOERR;
    state->ocnodes = oclistnew();
    state->conn = conn;
    if(ocdebug >= 2) dapdebug = 1;
    parseresult = dapparse(state);
    if(parseresult == 0) {/* 0 => parse ok */
	/* Check to see if we ended up parsing an error message */
	if(state->svcerror) {
            conn->error.code = nulldup(state->code);
            conn->error.message = nulldup(state->message);
	    tree->root = NULL;
	    ocerr = OC_EDAPSVC;
	} else {
	    OCASSERT((state->root != NULL));	
            tree->root = state->root;
	    state->root = NULL; /* avoid reclaim */
            tree->nodes = state->ocnodes;
	    state->ocnodes = NULL; /* avoid reclaim */
            tree->root->tree = tree;
	    ocerr = OC_NOERR;
	}
    } else { /* Parse failed */
	switch (tree->dxdclass) {
	case OCDAS: ocerr = OC_EDAS; break;
	case OCDDS: ocerr = OC_EDDS; break;
	case OCDATADDS: ocerr = OC_EDATADDS; break;
	default: ocerr = OC_EDAPSVC;
	}		
    }
    dap_parse_cleanup(state);
    return ocerr;
}

