/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

/* Parser actions for constraint expressions */

/* Since oc does not use the constraint parser,
   they functions all just abort if called.
*/

#include "ceparselex.h"

static Object collectlist(Object list0, Object decl);

void
projections(CEparsestate* state, Object list0)
{
    state->projections = (NClist*)list0;
#ifdef DEBUG
fprintf(stderr,"	ce.projections: %s\n",
	dumpprojections(state->projections));
#endif
}

void
selections(CEparsestate* state, Object list0)
{
    state->selections = (NClist*)list0;
#ifdef DEBUG
fprintf(stderr,"	ce.selections: %s\n",
	dumpselections(state->selections));
#endif
}


Object
projectionlist(CEparsestate* state, Object list0, Object decl)
{
    return collectlist(list0,decl);
}

Object
projection(CEparsestate* state, Object segmentlist)
{
    NCprojection* p = createncprojection();
    p->segments = (NClist*)segmentlist;    
#ifdef DEBUG
fprintf(stderr,"	ce.projection: %s\n",
	dumpprojection1(p));
#endif
    return p;
}

Object
segmentlist(CEparsestate* state, Object list0, Object decl)
{
    return collectlist(list0,decl);
}

Object
segment(CEparsestate* state, Object name, Object slices0)
{
    NCsegment* segment = createncsegment();
    NClist* slices = (NClist*)slices0;
    segment->segment = strdup((char*)name);
    segment->slicerank = nclistlength(slices);
    if(slices != NULL) {
	int i;
	ASSERT(nclistlength(slices) > 0);
	for(i=0;i<nclistlength(slices);i++) {
	    NCslice* slice = (NCslice*)nclistget(slices,i);
	    segment->slices[i] = *slice;
	    free(slice);
	}
        segment->slicesdefined = 1;
    } else
        segment->slicesdefined = 0;
#ifdef DEBUG
fprintf(stderr,"	ce.segment: %s\n",
	dumpsegment(segment));
#endif
    return segment;
}

Object
array_indices(CEparsestate* state, Object list0, Object decl)
{
    return collectlist(list0,decl);
}

Object
range(CEparsestate* state, Object sfirst, Object sstride, Object slast)
{
    NCslice* slice = createncslice();
    unsigned long first,stride,last;

    /* Note: that incoming arguments are strings; we must convert to size_t;
       but we do know they are legal integers or NULL */
    sscanf((char*)sfirst,"%lu",&first); /* always defined */
    if(slast != NULL)
        sscanf((char*)slast,"%lu",&last);
    else
	last = first;
    if(sstride != NULL)
        sscanf((char*)sstride,"%lu",&stride);
    else
	stride = 1; /* default */

    if(stride == 0)
	ceerror(state,"Illegal index for range stride");
    if(last < first)
	ceerror(state,"Illegal index for range last index");
    slice->first  = first;
    slice->stride = stride;
    slice->stop   = last + 1;
    slice->length  = slice->stop - slice->first;
    slice->count  = slice->length / slice->stride;
#ifdef DEBUG
fprintf(stderr,"	ce.slice: %s\n",
	dumpslice(slice));
#endif
    return slice;
}

/* Selection Procedures */

Object
selectionlist(CEparsestate* state, Object list0, Object decl)
{
    return collectlist(list0,decl);
}

Object
sel_clause(CEparsestate* state, int selcase,
	   Object path0, Object relop0, Object values)
{
    NCselection* sel = createncselection();
    sel->operator = (SelectionTag)relop0;
    sel->path = (NClist*)path0;
    sel->values = (NClist*)values;
    sel->node = NULL;
    return sel;
}

Object
selectionpath(CEparsestate* state, Object list0, Object text)
{
    ASSERT(text != NULL);
    return collectlist(list0,text);
}

Object
function(CEparsestate* state, Object fcnname, Object args)
{
    NCselection* sel = createncselection();
    sel->operator = ST_FCN;
    sel->path = nclistnew();
    nclistpush(sel->path,(ncelem)fcnname);
    sel->node = NULL;
    sel->values = nclistnew();
    return sel;
}

Object
arg_list(CEparsestate* state, Object list0, Object decl)
{
    return collectlist(list0,decl);
}

Object
value_list(CEparsestate* state, Object list0, Object decl)
{
    return collectlist(list0,decl);
}

Object
value(CEparsestate* state, Object text, int tag)
{
    NCvalue* value = createncvalue();
    switch (tag) {
    case SCAN_STRINGCONST:
	value->kind = ST_STR;
	value->value.text = text;
	break;
    case SCAN_NUMBERCONST:
	if(sscanf(text,"%lld",&value->value.intvalue)==1)
	    value->kind = ST_INT;
	else if(sscanf(text,"%lg",&value->value.floatvalue)==1)
	    value->kind = ST_FLOAT;
	else {
	    sscanf(text,"%lG",&value->value.floatvalue);
	    value->kind = ST_FLOAT;
	}
	break;
    case SCAN_WORD:
    default:
	/* In this case, text is actually a path list */
	value->kind = ST_VAR;
	value->value.var.path = (NClist*)text; /* fill-in cdfnode later */
        break;
    }
    return value;
}

static Object
collectlist(Object list0, Object decl)
{
    NClist* list = (NClist*)list0;
    if(list == NULL) list = nclistnew();
    nclistpush(list,(ncelem)decl);
    return list;
}

int
ceerror(CEparsestate* state, char* msg)
{
    strcpy(state->errorbuf,msg);
    state->errorcode=1;
    return 0;
}

static void
ce_parse_cleanup(CEparsestate* state)
{
    celexcleanup(&state->lexstate); /* will free */
}

static CEparsestate*
ce_parse_init(char* input, int ncconstraint)
{
    CEparsestate* state = NULL;
    if(input==NULL) {
        ceerror(state,"ce_parse_init: no input buffer");
    } else {
        state = (CEparsestate*)emalloc(sizeof(CEparsestate));
        MEMCHECK(state,(CEparsestate*)NULL);
        memset((void*)state,0,sizeof(CEparsestate)); /* Zero memory*/
        state->errorbuf[0] = '\0';
        state->errorcode = 0;
        celexinit(input,&state->lexstate);
	state->projections = NULL;
	state->selections = NULL;
	state->ncconstraint = ncconstraint;
    }
    return state;
}


/* Wrapper for ceparse */
int
ncceparse(char* input, int ncconstraint,
	  NClist** projectionsp, NClist** selectionsp, char** errmsgp)
{
    CEparsestate* state;
    int errcode = 0;

    if(input != NULL) {
#ifdef DEBUG
fprintf(stderr,"ncceparse: input=%s\n",input);
#endif
        state = ce_parse_init(input,ncconstraint);
        if(ceparse(state) == 0) {
#ifdef DEBUG
if(state->projections)
fprintf(stderr,"ncceparse: projections=%s\n",dumpprojections(state->projections));
#endif
	    if(projectionsp) *projectionsp = state->projections;
#ifdef DEBUG
if(state->selections)
fprintf(stderr,"ncceparse: selections=%s\n",dumpselections(state->selections));
#endif
	    if(selectionsp) *selectionsp = state->selections;
	} else {
	    if(errmsgp) *errmsgp = nulldup(state->errorbuf);
	}
	errcode = state->errorcode;
        ce_parse_cleanup(state);
    }
    return errcode;
}
