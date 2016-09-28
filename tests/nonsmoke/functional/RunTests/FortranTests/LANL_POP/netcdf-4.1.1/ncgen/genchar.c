#include "includes.h"

/******************************************************/
/* Code for generating char variables etc; mostly
   language independent */
/******************************************************/

/*
Matching strings to char variables, attributes, and vlen
constants is challenging because it is desirable to mimic
the original ncgen. The "algorithms" used there have no
simple characterization (such as "abc" == {'a','b','c'}).
So, this rather ugly code is kept in this file
and a variety of heuristics are used to mimic ncgen.
*/

extern List* vlenconstants;

static void gen_chararraysuffix(Symbol* vsym,
	      Bytebuffer* databuf,
	      Datasrc* src,
	      Odometer*,
	      int);

static int stringexplode(Datasrc* src, size_t chunksize);
static int fillstring(size_t declsize, int len, Bytebuffer* databuf);

void
gen_charattr(Symbol* asym, Bytebuffer* databuf)
{
    Datasrc* src;
    Constant* con;

    if(asym->data == NULL) return;
    src = datalist2src(asym->data);
    while((con=srcnext(src))) {
	switch (con->nctype) {
	/* Following list should be consistent with isstringable */
	case NC_CHAR:
	    bbAppend(databuf,con->value.charv);
	    break;
	case NC_BYTE:
	    bbAppend(databuf,con->value.int8v);
	    break;
	case NC_UBYTE:
	    bbAppend(databuf,con->value.uint8v);
	    break;
	case NC_STRING:
	    bbCat(databuf,con->value.stringv.stringv);
	    bbNull(databuf);
	    break;
	case NC_FILL:
	    bbAppend(databuf,NC_FILL_CHAR);
	    break;
	default:
	    semerror(srcline(src),
		     "Encountered non-string constant in attribute: %s",
		     asym->name);
	    return;
	}
    }
}

void
gen_chararray(Symbol* vsym,
		Bytebuffer* databuf,
		Datasrc* src,
		Odometer* odom,
		int index)
{
    /* Assume that all dimensions from index+1 to rank-1 are !unlimited */
    int i;
    int rank = odom->rank;
    int lastdim = (index == (rank - 1)); /* last dimension*/
    int firstdim = (index == 0);
    int isunlimited = (odom->declsize[index] == NC_UNLIMITED);
    int exploded = 0;
    Constant* con;

    if(lastdim) {
	gen_chararraysuffix(vsym,databuf,src,odom,index);
	return;
    }

    ASSERT(index >= 0 && index < rank);
    odom->index[index] = odom->start[index]; /* reset */

    if(isunlimited) {
	size_t slicesize;
	Constant* con;
	if(!firstdim) {
	    if(!issublist(src)) {
		semerror(srcline(src),"Unlimited data must be enclosed in {..}");
		return;
	    }
	    srcpush(src); /* enter the unlimited data */
	}
	con=srcpeek(src);
        /* Break up the constant if it is too large */
	slicesize = odomsubarray(odom,index+1);
	if(con != NULL && con->value.stringv.len > slicesize) {
	    /* Constant is larger than just our slice */
	    /* Explode the constant into subchunks */
	    exploded = stringexplode(src,slicesize);
	}
	while((con=srcpeek(src))!=NULL) {
	    gen_chararray(vsym,databuf,src,odom,index+1);
	    odom->index[index]++;
	}
	odom->unlimitedsize[index] = odom->index[index];
	if(exploded) srcpop(src);
	if(!firstdim) srcpop(src);
    } else { /* !isunlimited*/
	size_t slicesize;
	con = srcpeek(src);
	ASSERT(!lastdim);
	/* Break up the constant if it is too large */
	slicesize = odomsubarray(odom,index+1);
	if(con != NULL && con->value.stringv.len > slicesize) {
	    /* Constant is larger than just our slice */
	    /* Explode the constant into subchunks */
	    exploded = stringexplode(src,slicesize);
	}
	for(i=0;i<odom->declsize[index];i++) {
	    gen_chararray(vsym,databuf,src,odom,index+1);
	}
	if(exploded) srcpop(src);
    }
}

static void
gen_chararraysuffix(Symbol* vsym,
	      Bytebuffer* databuf,
	      Datasrc* src,
	      Odometer* odom,
	      int index)
{
    int i;
    int rank = odom->rank;
    int lastdim = (index == (rank - 1)); /* last dimension*/
    int firstdim = (index == 0);
    size_t declsize = odom->declsize[index];
    int isunlimited = (declsize==NC_UNLIMITED);
    Constant* con;

    ASSERT(index >= 0 && index < rank);
    odom->index[index] = odom->start[index]; /* reset*/

    con = srcpeek(src);
    if(!isunlimited) {
	if(con != NULL && !isstringable(con->nctype)) {
	    semerror(srcline(src),
		     "Encountered non-string constant in char data: %s",
		     vsym->name);
	    return;
	}
	if(lastdim) {
	    /* To mimic ncgen original, it appears we have to hack.
	       I think firstdim==lastdim may work.
	    */
	    for(i=0;i<declsize;) {
		int slen;
		con = srcnext(src);
		if(con == NULL) break;
		slen = collectstring(con,declsize,databuf);
		if(!firstdim && slen < declsize) slen=fillstring(declsize,slen,databuf);
		i += slen;
	    }
	    if(firstdim && i < declsize) i = fillstring(declsize,i,databuf);
   	    odom->index[index] = i;
	} else { /* ! lastdim*/
	    int exploded = 0;
	    size_t slicesize = odometertotal(odom,index+1);
	    if(con != NULL && con->nctype == NC_STRING
               && con->value.stringv.len > slicesize) {
		/* Constant is larger than just our slice */
		/* Explode the constant into subchunks */
		exploded = stringexplode(src,slicesize);
	    }
	    for(i=0;i<odom->declsize[index];i++) {
		gen_chararraysuffix(vsym,databuf,src,odom,index+1);
		odom->index[index]++;
	    }
	    if(exploded) srcpop(src);
	}
    } else { /* unlimited => lastdim*/
	Constant* con;
	if(!firstdim) {
	    if(!issublist(src)) {
		semerror(srcline(src),"Unlimited data must be enclosed in {..}");
		return;
	    }
	    srcpush(src); /* enter the unlimited data */
	}
	/* Basically, collect all the strings until we run out */
	i = 0;
	while((con=srcnext(src))!=NULL) {
	    i += collectstring(con,0,databuf);
	}
	odom->index[index] = i;
	odom->unlimitedsize[index] = odom->index[index];
	if(!firstdim) srcpop(src);
    }
}

/*
Since the field has fixed
dimensions, we can just
read N elements where N
is the product of the dimensions.
*/

void
gen_charfield(Datasrc* src, Odometer* odom, int index, Bytebuffer* fieldbuf)
{
    int i;
    int lastdim = (index == (odom->rank - 1));
    size_t declsize = odom->declsize[index];
    Constant* con;

    ASSERT(declsize != 0);

    if(lastdim) {
	for(i=0;i<declsize;) {
	    con = srcnext(src);
	    if(con == NULL) break;
	    if(!isstringable(con->nctype)) {
	        semerror(srcline(src),
	     		"Encountered non-string constant in compound field");
		return;
	    }
	    i += collectstring(con,declsize,fieldbuf);
	}
        if(i < declsize) i=fillstring(declsize,i,fieldbuf);
    } else { /* ! lastdim*/
	int exploded = 0;
	size_t slicesize;
	/* Compute subslice size */
        slicesize = 1;
	for(i=index+1;i<odom->rank;i++)
	    slicesize *= MAX(odom->declsize[i],odom->unlimitedsize[i]);
	con = srcpeek(src);
	if(con != NULL && !isstringable(con->nctype)) {
	    semerror(srcline(src),
	     		"Encountered non-string constant in compound field");
	    return;
	}
	if(con != NULL && con->value.stringv.len > slicesize) {
	    /* Constant is larger than just our slice */
	    /* Explode the constant into subchunks */
	    exploded = stringexplode(src,slicesize);
        }
        for(i=0;i<declsize;i++) {
	    gen_charfield(src,odom,index+1,fieldbuf);
	}
	if(exploded) srcpop(src);
    }
}

void
gen_charvlen(Datasrc* vlensrc, Bytebuffer* databuf)
{
    int count;
    Bytebuffer* vlenbuf = bbNew();
    Constant* con;

    count = 0;
    while((con=srcnext(vlensrc)) != NULL) {
	if(!isstringable(con->nctype)) {
	    semerror(srcline(vlensrc),
		     "Encountered non-string constant in vlen constant");
	    goto done;
        }
	count += collectstring(con,0,vlenbuf);
    }

done:
    bbFree(vlenbuf);
}

/**************************************************/

static Datalist*
buildstringlist(char* s, size_t chunksize, int lineno)
{
    size_t slen,div,rem;
    Datalist* charlist;
    Constant* chars;

    if(s == NULL) s = "";
    slen = strlen(s);
    ASSERT(chunksize > 0);
    div = slen / chunksize;
    rem = slen % chunksize;
    if(rem > 0) div++;

    charlist = builddatalist(div);
    if(!charlist) return NULL;
    charlist->readonly = 0;
    charlist->length = div;
    chars=charlist->data;
    if(slen == 0) {
	/* Special case for null string */
	charlist->length = 1;
	chars->nctype = NC_STRING;
	chars->lineno = lineno;
        chars->value.stringv.len = 1;
        chars->value.stringv.stringv = nulldup("");
    } else {
	int i;
	for(i=0;i<(div-1);i++,chars++) {
	    chars->nctype = NC_STRING;
	    chars->lineno = lineno;
            chars->value.stringv.len = chunksize;
	    chars->value.stringv.stringv = (char*)emalloc(chunksize+1);
	    memcpy(chars->value.stringv.stringv,s,chunksize);
	    chars->value.stringv.stringv[chunksize] = '\0';
	    s += chunksize;
	}
	/* Do last chunk */
        chars->nctype = NC_STRING;
	chars->lineno = lineno;
        chars->value.stringv.len = strlen(s);
        chars->value.stringv.stringv = nulldup(s);
    }
    return charlist;
}

static int
stringexplode(Datasrc* src, size_t chunksize)
{
    Constant* con;
    Datalist* charlist;

    if(!isstring(src)) return 0;
    con = srcnext(src);
    charlist = buildstringlist(con->value.stringv.stringv,chunksize,srcline(src));
    srcpushlist(src,charlist);
    return 1;
}

int
collectstring(Constant* con, size_t declsize, Bytebuffer* databuf)
{
    int i = 0;
    if(con != NULL) {
	ASSERT(isstringable(con->nctype));
        if(con->nctype == NC_STRING) {
            if(declsize > 0 && con->value.stringv.len >= (declsize-i)) {
                bbAppendn(databuf,con->value.stringv.stringv,con->value.stringv.len);
                i = declsize;
            } else if(con->value.stringv.len == 0) {
                i = 0;
            } else {
                /* Append */
                bbCat(databuf,con->value.stringv.stringv);
                i = con->value.stringv.len;
	    }
	} else if(con->nctype == NC_FILLVALUE) {
            bbAppend(databuf,NC_FILL_CHAR);
	    i = 1;
        } else {
            /* Append */
            bbAppend(databuf,con->value.charv);
            i = 1;
	}                   
    }
    return i;
}

/* Fill */
static int
fillstring(size_t declsize, int len, Bytebuffer* databuf)
{
    for(;len<declsize;len++)
        bbAppend(databuf,NC_FILL_CHAR);
    return len;
}
