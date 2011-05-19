#include "includes.h"
#include "nciter.h"

#ifdef ENABLE_BINARY

/**************************************************/
/* Code for generating binary data lists*/
/**************************************************/
/* For datalist constant rules: see the rules on the man page */

/* Forward */
static void bindata_primdata(Symbol*,Datasrc*,Bytebuffer*,Datalist*);
static void bindata_fieldarray(Symbol*,Datasrc*,Odometer*,int,Bytebuffer*);

/* Specialty wrappers for attributes and variables */
void
bindata_attrdata(Symbol* asym, Bytebuffer* memory)
{
    Datasrc* src;
    int typecode = asym->typ.basetype->typ.typecode;

    if(asym->data == NULL) return;
    if(typecode == NC_CHAR) {
	gen_charattr(asym,memory);
    } else {
        src = datalist2src(asym->data);
        while(srcmore(src)) {
            bindata_basetype(asym->typ.basetype,src,memory,NULL);
	}
    }
}

void
bindata_array(Symbol* vsym,
		  Bytebuffer* memory,
		  Datasrc* src,
		  Odometer* odom,
	          int index,
		  Datalist* fillsrc)
{
    int i;
    int rank = odom->rank;
    int lastdim = (index == (rank - 1)); /* last dimension*/
    size_t delta,count,subsize;
    Symbol* basetype = vsym->typ.basetype;
    ASSERT(index >= 0 && index < rank);

    /* Move to the ith element */
    subsize = subarraylength(&vsym->typ.dimset,index+1);
    delta = odom->start[index] * basetype->typ.nelems * subsize;
    srcmove(src,delta);
    count = odom->count[index];

    if(lastdim) {
        for(;count > 0; count--) {
            bindata_basetype(basetype,src,memory,fillsrc);
	}
	goto done;
    }

    /* now walk count elements and generate recursively */
    for(i=0;i<count;i++) {
	bindata_array(vsym,memory,src,odom,index+1,fillsrc);
    }
done:
    return;
}

/* Generate an instance of the basetype using datasrc */
void
bindata_basetype(Symbol* tsym, Datasrc* datasrc, Bytebuffer* memory, Datalist* fillsrc)
{
    int usecmpd;

    switch (tsym->subclass) {

    case NC_ENUM:
    case NC_OPAQUE:
    case NC_PRIM:
	if(issublist(datasrc)) {
	    semerror(srcline(datasrc),"Expected primitive found {..}");
	}
	bindata_primdata(tsym,datasrc,memory,fillsrc);
	break;

    case NC_COMPOUND: {
	int i;
        Constant* con;
	if(!isfillvalue(datasrc) && !issublist(datasrc)) {/* fail on no compound*/
	    semerror(srcline(datasrc),"Compound data must be enclosed in {..}");
        }
        con = srcnext(datasrc);
	if(con->nctype == NC_FILLVALUE) {
	    Datalist* filler = getfiller(tsym,fillsrc);
	    ASSERT(filler->length == 1);
	    con = &filler->data[0];
	    if(con->nctype != NC_COMPOUND) {
	        semerror(con->lineno,"Compound data fill value is not enclosed in {..}");
	    }
	}
        srcpushlist(datasrc,con->value.compoundv); /* enter the sublist*/
        for(i=0;i<listlength(tsym->subnodes);i++) {
            Symbol* field = (Symbol*)listget(tsym->subnodes,i);
            bindata_basetype(field,datasrc,memory,NULL);
	}
        srcpop(datasrc);
	} break;

    case NC_VLEN: {
        Constant* con;
        nc_vlen_t ptr;
	if(!isfillvalue(datasrc) && !issublist(datasrc)) {/* fail on no compound*/
	    semerror(con->lineno,"Vlen data must be enclosed in {..}");
        }
        con = srcnext(datasrc);
	if(con->nctype == NC_FILLVALUE) {
	    Datalist* filler = getfiller(tsym,fillsrc);
	    ASSERT(filler->length == 1);
	    con = &filler->data[0];
	    if(con->nctype != NC_COMPOUND) {
	        semerror(con->lineno,"Vlen data fill value is not enclosed in {..}");
	    }
	}
        /* generate the nc_vlen_t instance*/
        ptr.p = vlendata[con->value.compoundv->vlen.uid].data;
        ptr.len = vlendata[con->value.compoundv->vlen.uid].count;
        bbAppendn(memory,(char*)&ptr,sizeof(ptr));
        } break;

    case NC_FIELD:
	/* enclose in braces if and only if field is an array */
	usecmpd = (issublist(datasrc) && tsym->typ.dimset.ndims > 0);
	if(usecmpd) srcpush(datasrc);
	if(tsym->typ.dimset.ndims > 0) {
	    Odometer* fullodom = newodometer(&tsym->typ.dimset,NULL,NULL);
            bindata_fieldarray(tsym->typ.basetype,datasrc,fullodom,0,memory);
	    odometerfree(fullodom);
	} else {
	    bindata_basetype(tsym->typ.basetype,datasrc,memory,NULL);
	}
	if(usecmpd) srcpop(datasrc);
	break;

    default: PANIC1("bindata_basetype: unexpected subclass %d",tsym->subclass);
    }
}

/* Used only for structure field arrays*/
static void
bindata_fieldarray(Symbol* basetype, Datasrc* src, Odometer* odom, int index,
		Bytebuffer* memory)
{
    int i;
    int rank = odom->rank;
    unsigned int size = odom->declsize[index];
    int lastdim = (index == (rank - 1)); /* last dimension*/
    int chartype = (basetype->typ.typecode == NC_CHAR);

    if(chartype) {
	/* Collect the char field in a separate buffer */
	Bytebuffer* fieldbuf = bbNew();
        gen_charfield(src,odom,index,fieldbuf);
        bbAppendn(memory,bbContents(fieldbuf),bbLength(fieldbuf));
	bbFree(fieldbuf);
    } else {
        ASSERT(size != 0);
        for(i=0;i<size;i++) {
            if(lastdim) {
	        bindata_basetype(basetype,src,memory,NULL);
            } else { /* !lastdim*/
	        bindata_fieldarray(basetype,src,odom,index+1,memory);
	    }
	}
    }
}

static void
bindata_primdata(Symbol* basetype, Datasrc* src, Bytebuffer* memory, Datalist* fillsrc)
{
    Constant* prim;
    Constant target;

    prim = srcnext(src);
    if(prim == NULL) prim = &fillconstant;

    ASSERT(prim->nctype != NC_COMPOUND);

    if(prim->nctype == NC_FILLVALUE) {
	Datalist* filler = getfiller(basetype,fillsrc);
	ASSERT(filler->length == 1);
	srcpushlist(src,filler);
        bindata_primdata(basetype,src,memory,NULL);
	srcpop(src);
	goto done;
    }

    target.nctype = basetype->typ.typecode;

    if(target.nctype != NC_ECONST) {
	convert1(prim,&target);
        alignbuffer(&target,memory);
    }

    switch (target.nctype) {
        case NC_ECONST:
	    if(basetype->subclass != NC_ENUM) {
	        semerror(prim->lineno,"Conversion to enum not supported (yet)");
	    } else {
		Datalist* econ = builddatalist(1);
		srcpushlist(src,econ);
		dlappend(econ,&prim->value.enumv->typ.econst);
	        bindata_primdata(prim->value.enumv->typ.basetype,src,memory,fillsrc);
		srcpop(src);
	    }
   	    break;
        case NC_OPAQUE: {
	    unsigned char* bytes;
	    size_t len;
	    setprimlength(&target,basetype->typ.size*2);
	    bytes=makebytestring(target.value.opaquev.stringv,&len);
	    bbAppendn(memory,(void*)bytes,len);
	    } break;

        case NC_CHAR:
            bbAppendn(memory,&target.value.charv,sizeof(target.value.charv));
	    break;
        case NC_BYTE:
            bbAppendn(memory,(void*)&target.value.int8v,sizeof(target.value.int8v));
	    break;
        case NC_SHORT:
            bbAppendn(memory,(void*)&target.value.int16v,sizeof(target.value.int16v));
	    break;
        case NC_INT:
            bbAppendn(memory,(void*)&target.value.int32v,sizeof(target.value.int32v));
	    break;
        case NC_FLOAT:
            bbAppendn(memory,(void*)&target.value.floatv,sizeof(target.value.floatv));
	    break;
        case NC_DOUBLE:
            bbAppendn(memory,(void*)&target.value.doublev,sizeof(target.value.doublev));
	    break;
        case NC_UBYTE:
            bbAppendn(memory,(void*)&target.value.uint8v,sizeof(target.value.uint8v));
	    break;
        case NC_USHORT:
            bbAppendn(memory,(void*)&target.value.uint16v,sizeof(target.value.uint16v));
	    break;
        case NC_UINT:
            bbAppendn(memory,(void*)&target.value.uint32v,sizeof(target.value.uint32v));
	    break;
        case NC_INT64: {
	    union SI64 { char ch[8]; long long i64;} si64;
	    si64.i64 = target.value.int64v;
            bbAppendn(memory,(void*)si64.ch,sizeof(si64.ch));
	    } break;
        case NC_UINT64: {
	    union SU64 { char ch[8]; unsigned long long i64;} su64;
	    su64.i64 = target.value.uint64v;
            bbAppendn(memory,(void*)su64.ch,sizeof(su64.ch));
	    } break;
        case NC_STRING: {
            if(usingclassic) {
                bbAppendn(memory,target.value.stringv.stringv,target.value.stringv.len);
            } else if(target.nctype == NC_CHAR) {
                bbAppendn(memory,target.value.stringv.stringv,target.value.stringv.len);
            } else {
                char* ptr;
                int len = (size_t)target.value.stringv.len;
                ptr = poolalloc(len+1); /* CAREFUL: this has short lifetime*/
                memcpy(ptr,target.value.stringv.stringv,len);
                ptr[len] = '\0';
                bbAppendn(memory,(void*)&ptr,sizeof(ptr));
            }
	    } break;

        default: PANIC1("bindata_primdata: unexpected type: %d",target.nctype);
    }
done:
    return;
}


/*
This walk of the data lists collects
vlen sublists and constructs separate C constants
for each of them. The "id" of each list is then
recorded in the containing datalist.
*/
void
bindata_vlenconstants(List* vlenconstants)
{
    int i,nvlen;
    Datasrc* vlensrc;
    Bytebuffer* memory = bbNew();

    /* Prepare a place to store vlen constants */
    nvlen = listlength(vlenconstants);
    if(nvlen == 0) return;
    vlendata = (struct Vlendata*)emalloc(sizeof(struct Vlendata)*nvlen+1);
    memset((void*)vlendata,0,sizeof(struct Vlendata)*nvlen+1);

    for(i=0;i<nvlen;i++) {
	Constant* cmpd = (Constant*)listget(vlenconstants,i);
	int chartype;
	Symbol* tsym = cmpd->value.compoundv->vlen.schema;
	unsigned long uid = cmpd->value.compoundv->vlen.uid;
	unsigned long count;
        ASSERT(tsym != NULL);
        chartype = (tsym->typ.basetype->typ.typecode == NC_CHAR);

	vlensrc = datalist2src(cmpd->value.compoundv);

	bbClear(memory);
	count = 0;
	if(chartype) {
   	    /* Collect the char vlen in a separate buffer */
            gen_charvlen(vlensrc,memory);
	    count = bbLength(memory);
	} else {
  	    while(srcmore(vlensrc)) {
                bindata_basetype(tsym->typ.basetype,vlensrc,memory,NULL);
		count++;
	    }
	    ASSERT(count == cmpd->value.compoundv->vlen.count);
        }
	vlendata[uid].data = bbDup(memory);
	vlendata[uid].count = count;
    }
    bbFree(memory);
}

#endif /*ENABLE_BINARY*/
