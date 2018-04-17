#include        "includes.h"
#include        "dump.h"
#include        "offsets.h"

/* Forward*/
static void filltypecodes(void);
static void processenums(void);
static void processtypes(void);
static void processtypesizes(void);
static void processvars(void);
static void processattributes(void);

static void processdatalists(void);
static size_t processdatalist(Symbol*);

static void inferattributetype(Symbol* asym);
static void checkconsistency(void);
static void validate(void);
static int tagvlentypes(Symbol* tsym);

static size_t walkdata(Symbol*, Datasrc*);
static size_t walkarray(Symbol*, Datasrc*, int, Datalist*);
static size_t walktype(Symbol*, Datasrc*, Datalist*);
static void walkfieldarray(Symbol*, Datasrc*, Dimset*, int);
static int lastunlimited(Dimset* dimset, int from);


static Symbol* uniquetreelocate(Symbol* refsym, Symbol* root);

List* vlenconstants;  /* List<Constant*>;*/
			  /* ptr to vlen instances across all datalists*/

/* Post-parse semantic checks and actions*/
void
processsemantics(void)
{
    /* Process each type and sort by dependency order*/
    processtypes();
    /* Make sure all typecodes are set if basetype is set*/
    filltypecodes();
    /* Process each type to compute its size*/
    processtypesizes();
    /* Process each var to fill in missing fields, etc*/
    processvars();
    /* Process attributes to connect to corresponding variable*/
    processattributes();
    /* Fix up enum constant values*/
    processenums();
    /* Fix up datalists*/
    processdatalists();
    /* check internal consistency*/
    checkconsistency();
    /* do any needed additional semantic checks*/
    validate();
}

/*
Given a reference symbol, produce the corresponding
definition symbol; return NULL if there is no definition
Note that this is somewhat complicated to conform to
various scoping rules, namely:
1. look into parent hierarchy for un-prefixed dimension names.
2. look in whole group tree for un-prefixed type names;
   search is depth first. MODIFIED 5/26/2009: Search is as follows:
   a. search parent hierarchy for matching type names.
   b. search whole tree for unique matching type name
   c. complain and require prefixed name.
3. look in the same group as ref for un-prefixed variable names.
4. ditto for group references
5. look in whole group tree for un-prefixed enum constants
*/

Symbol*
locate(Symbol* refsym)
{
    Symbol* sym = NULL;
    switch (refsym->objectclass) {
    case NC_DIM:
	if(refsym->is_prefixed) {
	    /* locate exact dimension specified*/
	    sym = lookup(NC_DIM,refsym);
	} else { /* Search for matching dimension in all parent groups*/
	    Symbol* parent = lookupgroup(refsym->prefix);/*get group for refsym*/
	    while(parent != NULL) {
		/* search this parent for matching name and type*/
		sym = lookupingroup(NC_DIM,refsym->name,parent);
		if(sym != NULL) break;
		parent = parent->container;
	    }
	}		
	break;
    case NC_TYPE:
	if(refsym->is_prefixed) {
	    /* locate exact type specified*/
	    sym = lookup(NC_TYPE,refsym);
	} else {
	    Symbol* parent;
	    int i; /* Search for matching type in all groups (except...)*/
	    /* Short circuit test for primitive types*/
	    for(i=NC_NAT;i<=NC_STRING;i++) {
		Symbol* prim = basetypefor(i);
		if(prim == NULL) continue;
	        if(strcmp(refsym->name,prim->name)==0) {
		    sym = prim;
		    break;
		}
	    }
	    if(sym == NULL) {
	        /* Added 5/26/09: look in parent hierarchy first */
	        parent = lookupgroup(refsym->prefix);/*get group for refsym*/
	        while(parent != NULL) {
		    /* search this parent for matching name and type*/
		    sym = lookupingroup(NC_TYPE,refsym->name,parent);
		    if(sym != NULL) break;
		    parent = parent->container;
		}
	    }
	    if(sym == NULL) {
	        sym = uniquetreelocate(refsym,rootgroup); /* want unique */
	    }
	}		
	break;
    case NC_VAR:
	if(refsym->is_prefixed) {
	    /* locate exact variable specified*/
	    sym = lookup(NC_VAR,refsym);
	} else {
	    Symbol* parent = lookupgroup(refsym->prefix);/*get group for refsym*/
   	    /* search this parent for matching name and type*/
	    sym = lookupingroup(NC_VAR,refsym->name,parent);
	}		
        break;
    case NC_GRP:
	if(refsym->is_prefixed) {
	    /* locate exact group specified*/
	    sym = lookup(NC_GRP,refsym);
	} else {
	    Symbol* parent = lookupgroup(refsym->prefix);/*get group for refsym*/
   	    /* search this parent for matching name and type*/
	    sym = lookupingroup(NC_GRP,refsym->name,parent);
	}		
	break;

    default: PANIC1("locate: bad refsym type: %d",refsym->objectclass);
    }
    if(debug > 1) {
	char* ncname;
	if(refsym->objectclass == NC_TYPE)
	    ncname = ncclassname(refsym->subclass);
	else
	    ncname = ncclassname(refsym->objectclass);
	fdebug("locate: %s: %s -> %s\n",
		ncname,fullname(refsym),(sym?fullname(sym):"NULL"));
    }   
    return sym;
}

/*
Search for an object in all groups using preorder depth-first traversal.
Return NULL if symbol is not unique or not found at all.
*/
static Symbol*
uniquetreelocate(Symbol* refsym, Symbol* root)
{
    int i;
    Symbol* sym = NULL;
    /* search the root for matching name and major type*/
    sym = lookupingroup(refsym->objectclass,refsym->name,root);
    if(sym == NULL) {
	for(i=0;i<listlength(root->subnodes);i++) {
	    Symbol* grp = (Symbol*)listget(root->subnodes,i);
	    if(grp->objectclass == NC_GRP && !grp->is_ref) {
		Symbol* nextsym = uniquetreelocate(refsym,grp);
		if(nextsym != NULL) {
		    if(sym != NULL) return NULL; /* not unique */	
		    sym = nextsym;
		}
	    }
	}
    }
    return sym;
}


/* 1. Do a topological sort of the types based on dependency*/
/*    so that the least dependent are first in the typdefs list*/
/* 2. fill in type typecodes*/
/* 3. mark types that use vlen*/
static void
processtypes(void)
{
    int i,j,keep,added;
    List* sorted = listnew(); /* hold re-ordered type set*/
    /* Prime the walk by capturing the set*/
    /*     of types that are dependent on primitive types*/
    /*     e.g. uint vlen(*) or primitive types*/
    for(i=0;i<listlength(typdefs);i++) {
        Symbol* sym = (Symbol*)listget(typdefs,i);
	keep=0;
	switch (sym->subclass) {
	case NC_PRIM: /*ignore pre-defined primitive types*/
	    sym->touched=1;
	    break;
	case NC_OPAQUE:
	case NC_ENUM:
	    keep=1;
	    break;
        case NC_VLEN: /* keep if its basetype is primitive*/
	    if(sym->typ.basetype->subclass == NC_PRIM) keep=1;
	    break;	    	
	case NC_COMPOUND: /* keep if all fields are primitive*/
	    keep=1; /*assume all fields are primitive*/
	    for(j=0;j<listlength(sym->subnodes);j++) {
		Symbol* field = (Symbol*)listget(sym->subnodes,j);
		ASSERT(field->subclass == NC_FIELD);
		if(field->typ.basetype->subclass != NC_PRIM) {keep=0;break;}
	    }	  
	    break;
	default: break;/* ignore*/
	}
	if(keep) {
	    sym->touched = 1;
	    listpush(sorted,(elem_t)sym);
	}
    }	
    /* 2. repeated walk to collect level i types*/
    do {
        added=0;
        for(i=0;i<listlength(typdefs);i++) {
	    Symbol* sym = (Symbol*)listget(typdefs,i);
	    if(sym->touched) continue; /* ignore already processed types*/
	    keep=0; /* assume not addable yet.*/
	    switch (sym->subclass) {
	    case NC_PRIM: 
	    case NC_OPAQUE:
	    case NC_ENUM:
		PANIC("type re-touched"); /* should never happen*/
	        break;
            case NC_VLEN: /* keep if its basetype is already processed*/
	        if(sym->typ.basetype->touched) keep=1;
	        break;	    	
	    case NC_COMPOUND: /* keep if all fields are processed*/
	        keep=1; /*assume all fields are touched*/
	        for(j=0;j<listlength(sym->subnodes);j++) {
		    Symbol* field = (Symbol*)listget(sym->subnodes,j);
		    ASSERT(field->subclass == NC_FIELD);
		    if(!field->typ.basetype->touched) {keep=1;break;}
	        }	  
	        break;
	    default: break;				
	    }
	    if(keep) {
		listpush(sorted,(elem_t)sym);
		sym->touched = 1;
		added++;
	    }	    
	}
    } while(added > 0);
    /* Any untouched type => circular dependency*/
    for(i=0;i<listlength(typdefs);i++) {
	Symbol* tsym = (Symbol*)listget(typdefs,i);
	if(tsym->touched) continue;
	semerror(tsym->lineno,"Circular type dependency for type: %s",fullname(tsym));
    }
    listfree(typdefs);
    typdefs = sorted;
    /* fill in type typecodes*/
    for(i=0;i<listlength(typdefs);i++) {
        Symbol* sym = (Symbol*)listget(typdefs,i);
	if(sym->typ.basetype != NULL && sym->typ.typecode == NC_NAT)
	    sym->typ.typecode = sym->typ.basetype->typ.typecode;
    }
    /* Identify types containing vlens */
    for(i=0;i<listlength(typdefs);i++) {
        Symbol* tsym = (Symbol*)listget(typdefs,i);
	tagvlentypes(tsym);
    }
}

/* Recursively check for vlens*/
static int
tagvlentypes(Symbol* tsym)
{
    int tagged = 0;
    int j;
    switch (tsym->subclass) {
        case NC_VLEN: 
	    tagged = 1;
	    tagvlentypes(tsym->typ.basetype);
	    break;	    	
	case NC_COMPOUND: /* keep if all fields are primitive*/
	    for(j=0;j<listlength(tsym->subnodes);j++) {
		Symbol* field = (Symbol*)listget(tsym->subnodes,j);
		ASSERT(field->subclass == NC_FIELD);
		if(tagvlentypes(field->typ.basetype)) tagged = 1;
	    }	  
	    break;
	default: break;/* ignore*/
    }
    if(tagged) tsym->typ.hasvlen = 1;
    return tagged;
}

/* Make sure all typecodes are set if basetype is set*/
static void
filltypecodes(void)
{
    Symbol* sym;
    for(sym=symlist;sym != NULL;sym = sym->next) {    
	if(sym->typ.basetype != NULL && sym->typ.typecode == NC_NAT)
	    sym->typ.typecode = sym->typ.basetype->typ.typecode;
    }
}

static void
processenums(void)
{
    int i,j;
    List* enumids = listnew();
    for(i=0;i<listlength(typdefs);i++) {
	Symbol* sym = (Symbol*)listget(typdefs,i);
	ASSERT(sym->objectclass == NC_TYPE);
	if(sym->subclass != NC_ENUM) continue;
	for(j=0;j<listlength(sym->subnodes);j++) {
	    Symbol* esym = (Symbol*)listget(sym->subnodes,j);
	    ASSERT(esym->subclass == NC_ECONST);
	    listpush(enumids,(elem_t)esym);
	}
    }	    
    /* Now walk set of enum ids to look for duplicates with same prefix*/
    for(i=0;i<listlength(enumids);i++) {
	Symbol* sym1 = (Symbol*)listget(enumids,i);
        for(j=i+1;j<listlength(enumids);j++) {
	   Symbol* sym2 = (Symbol*)listget(enumids,j);
	   if(strcmp(sym1->name,sym2->name) != 0) continue;
	   if(!prefixeq(sym1->prefix,sym2->prefix)) continue;
	   semerror(sym1->lineno,"Duplicate enumeration ids in same scope: %s",
		   fullname(sym1));	
	}
    }    
    /* Convert enum values to match enum type*/
    for(i=0;i<listlength(typdefs);i++) {
	Symbol* tsym = (Symbol*)listget(typdefs,i);
	ASSERT(tsym->objectclass == NC_TYPE);
	if(tsym->subclass != NC_ENUM) continue;
	for(j=0;j<listlength(tsym->subnodes);j++) {
	    Symbol* esym = (Symbol*)listget(tsym->subnodes,j);
	    Constant newec;
	    ASSERT(esym->subclass == NC_ECONST);
	    newec.nctype = esym->typ.typecode;
	    convert1(&esym->typ.econst,&newec);
	    esym->typ.econst = newec;
	}	
    }
}

/* Compute type sizes and compound offsets*/
void
computesize(Symbol* tsym)
{
    int i;
    int offset = 0;
    unsigned long totaldimsize;
    if(tsym->touched) return;
    tsym->touched=1;
    switch (tsym->subclass) {
        case NC_VLEN: /* actually two sizes for vlen*/
	    computesize(tsym->typ.basetype); /* first size*/
	    tsym->typ.size = ncsize(tsym->typ.typecode);
	    tsym->typ.alignment = nctypealignment(tsym->typ.typecode);
	    tsym->typ.nelems = 1; /* always a single compound datalist */
	    break;
	case NC_PRIM:
	    tsym->typ.size = ncsize(tsym->typ.typecode);
	    tsym->typ.alignment = nctypealignment(tsym->typ.typecode);
	    tsym->typ.nelems = 1;
	    break;
	case NC_OPAQUE:
	    /* size and alignment already assigned*/
	    tsym->typ.nelems = 1;
	    break;
	case NC_ENUM:
	    computesize(tsym->typ.basetype); /* first size*/
	    tsym->typ.size = tsym->typ.basetype->typ.size;
	    tsym->typ.alignment = tsym->typ.basetype->typ.alignment;
	    tsym->typ.nelems = 1;
	    break;
	case NC_COMPOUND: /* keep if all fields are primitive*/
	    /* First, compute recursively, the size and alignment of fields*/
	    for(i=0;i<listlength(tsym->subnodes);i++) {
		Symbol* field = (Symbol*)listget(tsym->subnodes,i);
		ASSERT(field->subclass == NC_FIELD);
		computesize(field);
		/* alignment of struct is same as alignment of first field*/
		if(i==0) tsym->typ.alignment = field->typ.alignment;
	    }	  
	    /* now compute the size of the compound based on*/
	    /* what user specified*/
	    offset = 0;
	    for(i=0;i<listlength(tsym->subnodes);i++) {
		Symbol* field = (Symbol*)listget(tsym->subnodes,i);
		/* only support 'c' alignment for now*/
		int alignment = field->typ.alignment;
		offset += getpadding(offset,alignment);
		field->typ.offset = offset;
		offset += field->typ.size;
	    }
	    tsym->typ.size = offset;
	    break;
        case NC_FIELD: /* Compute size of all non-unlimited dimensions*/
	    if(tsym->typ.dimset.ndims > 0) {
	        computesize(tsym->typ.basetype);
	        totaldimsize = arraylength(&tsym->typ.dimset);
	        tsym->typ.size = tsym->typ.basetype->typ.size * totaldimsize;
	        tsym->typ.alignment = tsym->typ.basetype->typ.alignment;
	        tsym->typ.nelems = 1;
	    } else {
	        tsym->typ.size = tsym->typ.basetype->typ.size;
	        tsym->typ.alignment = tsym->typ.basetype->typ.alignment;
	        tsym->typ.nelems = tsym->typ.basetype->typ.nelems;
	    }
	    break;
	default:
	    PANIC1("computesize: unexpected type class: %d",tsym->subclass);
	    break;
    }
}

void
processvars(void)
{
    int i,j;
    for(i=0;i<listlength(vardefs);i++) {
	Symbol* vsym = (Symbol*)listget(vardefs,i);
	Symbol* tsym = vsym->typ.basetype;
	/* fill in the typecode*/
	vsym->typ.typecode = tsym->typ.typecode;
	for(j=0;j<tsym->typ.dimset.ndims;j++) {
	    /* deref the dimensions*/
	    tsym->typ.dimset.dimsyms[j] = tsym->typ.dimset.dimsyms[j];
#ifndef USE_NETCDF4
	    /* UNLIMITED must only be in first place*/
	    if(tsym->typ.dimset.dimsyms[j]->dim.declsize == NC_UNLIMITED) {
		if(j != 0)
		    semerror(vsym->lineno,"Variable: %s: UNLIMITED must be in first dimension only",fullname(vsym));
	    }
#endif
	}	
    }
}

void
processtypesizes(void)
{
    int i;
    /* use touch flag to avoid circularity*/
    for(i=0;i<listlength(typdefs);i++) {
	Symbol* tsym = (Symbol*)listget(typdefs,i);
	tsym->touched = 0;
    }
    for(i=0;i<listlength(typdefs);i++) {
	Symbol* tsym = (Symbol*)listget(typdefs,i);
	computesize(tsym); /* this will recurse*/
    }
}

void
processattributes(void)
{
    int i,j;
    /* process global attributes*/
    for(i=0;i<listlength(gattdefs);i++) {
	Symbol* asym = (Symbol*)listget(gattdefs,i);
	if(asym->typ.basetype == NULL) {
	    inferattributetype(asym);
	    if(asym->typ.basetype == NULL) {/* no data list */
	        asym->typ.basetype = primsymbols[NC_CHAR];
	    }
	}
        /* fill in the typecode*/
	asym->typ.typecode = asym->typ.basetype->typ.typecode;
    }
    /* process per variable attributes*/
    for(i=0;i<listlength(attdefs);i++) {
	Symbol* asym = (Symbol*)listget(attdefs,i);
	if(asym->typ.basetype == NULL) inferattributetype(asym);
	/* fill in the typecode*/
	asym->typ.typecode = asym->typ.basetype->typ.typecode;
    }
    /* collect per-variable attributes per variable*/
    for(i=0;i<listlength(vardefs);i++) {
	Symbol* vsym = (Symbol*)listget(vardefs,i);
	List* list = listnew();
        for(j=0;j<listlength(attdefs);j++) {
	    Symbol* asym = (Symbol*)listget(attdefs,j);
	    ASSERT(asym->att.var != NULL);
	    if(asym->att.var != vsym) continue;	    
            listpush(list,(elem_t)asym);
	}
	vsym->var.attributes = list;
    }
}

/*
 Look at the first primitive value of the
 attribute's datalist to infer the type of the attribute.
 There is a potential ambiguity when that value is a string.
 Is the attribute type NC_CHAR or NC_STRING?
 The answer is we always assume it is NC_CHAR in order to
 be back compatible with ncgen.
*/

static nc_type
inferattributetype1(Datasrc* src)
{
    nc_type result = NC_NAT;
    /* Recurse down any enclosing compound markers to find first non-fill "primitive"*/
    while(result == NC_NAT && srcmore(src)) {
	if(issublist(src)) {
	    srcpush(src);
	    result = inferattributetype1(src);
	    srcpop(src);
	} else {	
	    Constant* con = srcnext(src);
	    if(isprimplus(con->nctype)) result = con->nctype;
	    /* else keep looking*/
	}
    }
    return result;
}

static void
inferattributetype(Symbol* asym)
{
    Datalist* datalist;
    Datasrc* src;
    nc_type nctype;
    ASSERT(asym->data != NULL);
    datalist = asym->data;
    src = datalist2src(datalist);
    nctype = inferattributetype1(src);    
    freedatasrc(src);
    /* get the corresponding primitive type built-in symbol*/
    /* special case for string*/
    if(nctype == NC_STRING)
        asym->typ.basetype = basetypefor(NC_CHAR);
    else if(usingclassic) {
        /* If we are in classic mode, then restrict the inferred type
           to the classic types */
	switch (nctype) {
	case NC_UBYTE:
	    nctype = NC_SHORT;
	    break;	
	case NC_USHORT:
	case NC_UINT:
	case NC_INT64:
	case NC_UINT64:
	case NC_OPAQUE:
	case NC_ENUM:
	    nctype = NC_INT;
	    break;
	default: /* leave as is */
	    break;
	}
	asym->typ.basetype = basetypefor(nctype);
    } else
	asym->typ.basetype = basetypefor(nctype);


}

/* Find name within group structure*/
Symbol*
lookupgroup(List* prefix)
{
#ifdef USE_NETCDF4
    if(prefix == NULL || listlength(prefix) == 0)
	return rootgroup;
    else
	return (Symbol*)listtop(prefix);
#else
    return rootgroup;
#endif
}

/* Find name within given group*/
Symbol*
lookupingroup(nc_class objectclass, char* name, Symbol* grp)
{
    int i;
    if(name == NULL) return NULL;
    if(grp == NULL) grp = rootgroup;
dumpgroup(grp);
    for(i=0;i<listlength(grp->subnodes);i++) {
	Symbol* sym = (Symbol*)listget(grp->subnodes,i);
	if(sym->is_ref) continue;
	if(sym->objectclass != objectclass) continue;
	if(strcmp(sym->name,name)!=0) continue;
	return sym;
    }
    return NULL;
}

/* Find symbol within group structure*/
Symbol*
lookup(nc_class objectclass, Symbol* pattern)
{
    Symbol* grp;
    if(pattern == NULL) return NULL;
    grp = lookupgroup(pattern->prefix);
    if(grp == NULL) return NULL;
    return lookupingroup(objectclass,pattern->name,grp);
}

#ifndef NO_STDARG
void
semerror(const int lno, const char *fmt, ...)
#else
void
semerror(lno,fmt,va_alist) const int lno; const char* fmt; va_dcl
#endif
{
    va_list argv;
    vastart(argv,fmt);
    (void)fprintf(stderr,"%s: %s line %d: ", progname, cdlname, lno);
    vderror(fmt,argv);
    exit(1);
}


/* return internal size for values of specified netCDF type */
size_t
nctypesize(
     nc_type type)			/* netCDF type code */
{
    switch (type) {
      case NC_BYTE: return sizeof(char);
      case NC_CHAR: return sizeof(char);
      case NC_SHORT: return sizeof(short);
      case NC_INT: return sizeof(int);
      case NC_FLOAT: return sizeof(float);
      case NC_DOUBLE: return sizeof(double);
      case NC_UBYTE: return sizeof(unsigned char);
      case NC_USHORT: return sizeof(unsigned short);
      case NC_UINT: return sizeof(unsigned int);
      case NC_INT64: return sizeof(long long);
      case NC_UINT64: return sizeof(unsigned long long);
      case NC_STRING: return sizeof(char*);
      default:
	PANIC("nctypesize: bad type code");
    }
    return 0;
}

static int
sqContains(List* seq, Symbol* sym)
{
    int i;
    if(seq == NULL) return 0;
    for(i=0;i<listlength(seq);i++) {
        Symbol* sub = (Symbol*)listget(seq,i);
	if(sub == sym) return 1;
    }
    return 0;
}

static void
checkconsistency(void)
{
    int i;
    for(i=0;i<listlength(grpdefs);i++) {
	Symbol* sym = (Symbol*)listget(grpdefs,i);
	if(sym == rootgroup) {
	    if(sym->container != NULL)
	        PANIC("rootgroup has a container");
	} else if(sym->container == NULL && sym != rootgroup)
	    PANIC1("symbol with no container: %s",sym->name);
	else if(sym->container->is_ref != 0)
	    PANIC1("group with reference container: %s",sym->name);
	else if(sym != rootgroup && !sqContains(sym->container->subnodes,sym))
	    PANIC1("group not in container: %s",sym->name);
	if(sym->subnodes == NULL)
	    PANIC1("group with null subnodes: %s",sym->name);
    }
    for(i=0;i<listlength(typdefs);i++) {
	Symbol* sym = (Symbol*)listget(typdefs,i);
        if(!sqContains(sym->container->subnodes,sym))
	    PANIC1("type not in container: %s",sym->name);
    }
    for(i=0;i<listlength(dimdefs);i++) {
	Symbol* sym = (Symbol*)listget(dimdefs,i);
        if(!sqContains(sym->container->subnodes,sym))
	    PANIC1("dimension not in container: %s",sym->name);
    }
    for(i=0;i<listlength(vardefs);i++) {
	Symbol* sym = (Symbol*)listget(vardefs,i);
        if(!sqContains(sym->container->subnodes,sym))
	    PANIC1("variable not in container: %s",sym->name);
	if(!(isprimplus(sym->typ.typecode)
	     || sqContains(typdefs,sym->typ.basetype)))
	    PANIC1("variable with undefined type: %s",sym->name);
    }
}

static void
validate(void)
{
    int i;
    for(i=0;i<listlength(vardefs);i++) {
	Symbol* sym = (Symbol*)listget(vardefs,i);
	if(sym->var.special._Fillvalue != NULL) {
	}
    }
}

/*
Do any pre-processing of datalists.
1. Compute the effective size of unlimited
   dimensions vis-a-vis this data list
2. Compute the length of attribute lists
3. Collect the VLEN constants
4. add fills as needed to get lengths correct
5. make interior unlimited instances all have same length
*/

void
processdatalists(void)
{
    int i;
    if(debug > 0) fdebug("processdatalists:\n");
    vlenconstants = listnew();

    listsetalloc(vlenconstants,1024);

    /* process global attributes*/
    for(i=0;i<listlength(gattdefs);i++) {
	Symbol* asym = (Symbol*)listget(gattdefs,i);
	if(asym->data != NULL)
            asym->data->nelems = processdatalist(asym);
        if(debug > 0 && asym->data != NULL) {
	    fdebug(":%s.datalist: ",asym->name);
	    dumpdatalist(asym->data,"");
	    fdebug("\n");
	}
    }
    /* process per variable attributes*/
    for(i=0;i<listlength(attdefs);i++) {
	Symbol* asym = (Symbol*)listget(attdefs,i);
	if(asym->data != NULL)
	    asym->data->nelems = processdatalist(asym);
        if(debug > 0 && asym->data != NULL) {
	    fdebug("%s:%s.datalist: ",asym->att.var->name,asym->name);
	    dumpdatalist(asym->data,"");
	    fdebug("\n");
	}
    }
    /* process all variable data lists */
    for(i=0;i<listlength(vardefs);i++) {
	Symbol* vsym = (Symbol*)listget(vardefs,i);
	if(vsym->data != NULL)
            vsym->data->nelems = processdatalist(vsym);
        if(debug > 0 && vsym->data != NULL) {
	    fdebug("%s.datalist: ",vsym->name);
	    dumpdatalist(vsym->data,"");
	    fdebug("\n");
	}
    }
}

static size_t
processdatalist(Symbol* sym)
{
    Datasrc* src;
    size_t total = 0;
    src = datalist2src(sym->data);
    total = walkdata(sym,src);
    freedatasrc(src);
    return total;
}

/*
Recursively walk the variable/basetype and
simultaneously walk the datasrc.
Uses separate code for:
1. variables
2. types
3. field arrays
This set of procedures is an example of the
canonical way to simultaneously walk a variable
and a datalist.
*/

static size_t
walkdata(Symbol* sym, Datasrc* src)
{
    int rank = sym->typ.dimset.ndims;
    size_t total = 0;
    Datalist* fillsrc = sym->var.special._Fillvalue;

    switch (sym->objectclass) {
    case NC_VAR:
	if(rank == 0) /*scalar*/
	    total = walktype(sym->typ.basetype,src,fillsrc);
	else
	    total = walkarray(sym,src,0,fillsrc);
	break;
    case NC_ATT:
	for(total=0;srcpeek(src) != NULL;total++)
	    walktype(sym->typ.basetype,src,NULL);	
	break;
    default:
	PANIC1("walkdata: illegal objectclass: %d",(int)sym->objectclass);
	break;	
    }
    return total;
}

static size_t
walkarray(Symbol* vsym, Datasrc* src, int dimindex, Datalist* fillsrc)
{
    int i;
    Dimset* dimset = &vsym->typ.dimset;
    int rank = dimset->ndims;
    int lastdim = (dimindex == (rank-1));
    Symbol* dim = dimset->dimsyms[dimindex];
    int isunlimited = (dim->dim.declsize == NC_UNLIMITED);
    int islastunlimited = lastunlimited(dimset,dimindex+1);
    int ischartype = (vsym->typ.basetype->typ.typecode == NC_CHAR);
    size_t total = 1;
    size_t count = 0;

    ASSERT(rank > 0);

    if(isunlimited) {
	if(islastunlimited && ischartype) {
	    /* the remainder of src must be all stringables */
	    int checkpoint = src->index; /* save */
	    size_t subsize, slen;
	    Bytebuffer* buf = bbNew();
	    /* test and collect the complete string */
	    for(;;) {	    
	        Constant* con = srcnext(src);
		if(con == NULL) break;
		if(!isstringable(con->nctype)) {
		    semerror(srcline(src),"Illegal string constant");
		} else
		    collectstring(con,0,buf);
	    }
	    src->index = checkpoint;
	    /* Compute the subslice size */
	    subsize = subarraylength(dimset,dimindex+1);
	    /* pad the string */
	    slen = bbLength(buf);
	    slen += (subsize-1);
	    /* Compute the presumed size of this unlimited. */
	    count = slen / subsize;	    
	    /* compute unlimited max */
	    dim->dim.unlimitedsize = MAX(count,dim->dim.unlimitedsize);
	    bbFree(buf);
	} else {
	    for(count=0;srcpeek(src) != NULL;count++) {
                if(lastdim)
                    walktype(vsym->typ.basetype,src,fillsrc);
	        else
	            total *= walkarray(vsym,src,dimindex+1,fillsrc);
	    }
	    /* compute unlimited max */
	    dim->dim.unlimitedsize = MAX(count,dim->dim.unlimitedsize);
	}
    } else {
	count = dim->dim.declsize;
	for(i=0;i<dim->dim.declsize;i++) {
            if(lastdim)
                walktype(vsym->typ.basetype,src,fillsrc);
	    else
	        total *= walkarray(vsym,src,dimindex+1,fillsrc);
	}
    }
    return total;    
}

static size_t
walktype(Symbol* tsym, Datasrc* src, Datalist* fillsrc)
{
    int i;
    int count;
    Constant* con;
    Datalist* dl;

    ASSERT(tsym->objectclass == NC_TYPE);

    switch (tsym->subclass) {

    case NC_ENUM: case NC_OPAQUE: case NC_PRIM: 
	srcnext(src);
	break;

    case NC_COMPOUND:
	if(!isfillvalue(src) && !issublist(src)) {/* fail on no compound*/
           semerror(srcline(src),"Compound constants must be enclosed in {..}");
        }
        con = srcnext(src);
	if(con->nctype == NC_FILLVALUE) {
	    dl = getfiller(tsym,fillsrc);
	    ASSERT(dl->length == 1);
	    con = &dl->data[0];
	    if(con->nctype != NC_COMPOUND) {
	        semerror(srcline(src),"Vlen fill constants must be enclosed in {..}");
	    }
	}
        dl = con->value.compoundv;
	srcpushlist(src,dl); /* enter the sublist*/
	for(count=0,i=0;i<listlength(tsym->subnodes) && srcmore(src);i++,count++) {
	    Symbol* field = (Symbol*)listget(tsym->subnodes,i);
	    walktype(field,src,NULL);
	}
        srcpop(src);
	dl->nelems = count;
	break;

    case NC_VLEN:
        if(!isfillvalue(src) && !issublist(src)) {/* fail on no compound*/
           semerror(srcline(src),"Vlen constants must be enclosed in {..}");
        }
	con = srcnext(src);
	if(con->nctype == NC_FILLVALUE) {
	    dl = getfiller(tsym,fillsrc);
	    ASSERT(dl->length == 1);
	    con = &dl->data[0];
	    if(con->nctype != NC_COMPOUND) {
	        semerror(srcline(src),"Vlen fill constants must be enclosed in {..}");
	    }
	}
        if(!listcontains(vlenconstants,(elem_t)con)) {
            dl = con->value.compoundv;
	    /* Process list only if new */
	    srcpushlist(src,dl); /* enter the sublist*/
	    for(count = 0;srcmore(src);count++) {
                walktype(tsym->typ.basetype,src,NULL);
            }
            srcpop(src);
     	    dl->nelems = count;
	    dl->vlen.count = count;	
	    dl->vlen.uid = listlength(vlenconstants);
	    dl->vlen.schema = tsym;
 	    listpush(vlenconstants,(elem_t)con);
	}
	break;

    case NC_FIELD:
        if(tsym->typ.dimset.ndims > 0) {
	    walkfieldarray(tsym->typ.basetype,src,&tsym->typ.dimset,0);
	} else
	    walktype(tsym->typ.basetype,src,NULL);
	break;

    default: PANIC1("processdatalist: unexpected subclass %d",tsym->subclass);
    }
    return 1;
}

/* Used only for structure field arrays*/
static void
walkfieldarray(Symbol* basetype, Datasrc* src, Dimset* dimset, int index)
{
    int i;
    int rank = dimset->ndims;
    int lastdim = (index == (rank-1));
    Symbol* dim = dimset->dimsyms[index];
    size_t datasize = dim->dim.declsize;
    size_t count = 0;

    ASSERT(datasize != 0);
    count = datasize;
    for(i=0;i<datasize;i++) {
        if(lastdim)
	    walktype(basetype,src,NULL);
	else
	    walkfieldarray(basetype,src,dimset,index+1);
    }
}

/* Return 1 if the set of dimensions from..rank-1 are not unlimited */
static int
lastunlimited(Dimset* dimset, int from)
{
    int i;
    for(i=from;i<dimset->ndims;i++) {
        Symbol* dim = dimset->dimsyms[i];
        if(dim->dim.declsize == NC_UNLIMITED) return 0;
    }
    return 1;
}
