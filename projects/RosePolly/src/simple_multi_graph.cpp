#include <assert.h>
#include <string.h>

#include <rosepoly/simple_multi_graph.h>
#include <rosepoly/simple_matrix.h>

// SIMPLE MULTI GRAPH

Vertex * vertices;
int nVertices;

/* Adjacency matrix */
simple_matrix * adj;

Scc * sccs;
int num_sccs;
simple_multi_graph::simple_multi_graph( const simple_multi_graph& g )
: nVertices(g.get_v_size()), adj(new simple_matrix(*g.get_adj())), num_sccs(g.get_num_sccs())
{
	vertices = new Vertex[nVertices];
	Vertex * original_v = g.get_vertices();
	for ( int i = 0 ; i < nVertices ; i++ )
		vertices[i] = original_v[i];
	
	sccs = new Scc[num_sccs];
	Scc * original_s = g.get_sccs();
	for ( int i = 0 ; i < num_sccs ; i++ )
		sccs[i] = original_s[i];
}

simple_multi_graph::simple_multi_graph( int n )
{
	assert(n!=0);
	
	nVertices = n;
	
	vertices = new Vertex[n];
	for ( int i = 0 ; i < n ; i++ )
		vertices[i].id = i;
	
	adj = new simple_matrix(n,n);
	
	for ( int i = 0 ; i < n ; i++ ) {
		vertices[i].vn = -1;
		vertices[i].fn = -1;
		for ( int j = 0 ; j < n ; j++ )
			adj->set_entry(i,j,0);
	}
	
	sccs = new Scc[n];
	
	/* Not computed yet */
	num_sccs = -1;
}

simple_multi_graph::~simple_multi_graph() 
{ delete [] vertices; delete [] sccs; delete(adj); }

void simple_multi_graph::dfs()
{
	int time = 0;
	
	for ( int i = 0 ; i < nVertices ; i++ )
		vertices[i].vn = 0;
	
	for ( int i = 0 ; i < nVertices ; i++ )
		if ( vertices[i].vn == 0 )
			dfs_vertex(i,&time);
}

simple_multi_graph * simple_multi_graph::graph_transpose()
{
	simple_multi_graph * gT = new simple_multi_graph(nVertices);
	
	Vertex * gT_vert = gT->get_vertices();
	for ( int i = 0 ; i < nVertices ; i++ ) {
		for ( int j = 0 ; j < nVertices ; j++ )
			gT->adj_set_entry(j,i,adj->entry(i,j));
		
		gT_vert[i].fn = vertices[i].fn;
	}
	return gT;
}

void simple_multi_graph::dfs_vertex( int pos, int * time )
{
	*time = *time + 1;
	Vertex * v = &vertices[pos];
	v->vn = *time;
	
	for ( int i = 0 ; i < nVertices ; i++ )
		if ( adj->entry(v->id,i) )
			if ( vertices[i].vn == 0 )
				dfs_vertex(i,time);
	
	*time = *time + 1;
	v->fn = *time;
}

/* Comparison function for sorting graph vertices by their finish time */
static int compar (const void *e1, const void *e2)
{
    Vertex *v1, *v2;
	
    v1 = (Vertex *) e1;
    v2 = (Vertex *) e2;
	
    if (v1->fn < v2->fn)    {
        return -1;
    }else if (v1->fn == v2->fn) {
        return 0;
    }else return 1;
}

void simple_multi_graph::dfs_for_scc()
{
	int time = 0;
	
	Vertex * vCopy = new Vertex[nVertices];
	memcpy(vCopy, vertices, nVertices*sizeof(Vertex));
	
	for ( int i = 0 ; i < nVertices ; i++ ) {
		vertices[i].vn = 0;
		vertices[i].scc_id = -1;
	}
	
	/* Sort by fn */
	qsort(vCopy, nVertices, sizeof(Vertex), compar);
	
	/* Reverse order of fn */
	int numScc = 0;
	for ( int i = nVertices-1 ; i >= 0 ; i-- ) {
		if (vertices[vCopy[i].id].vn == 0 ) {
			sccs[numScc].id = numScc;
			dfs_vertex(vCopy[i].id, &time);
			
			for ( int j = 0 ; j < nVertices ; j++ )
				if ( vertices[j].scc_id == -1 && vertices[j].vn > 0 )
					vertices[j].scc_id = numScc;
			
			numScc++;
		}
	}
	
	num_sccs = numScc;
	delete [] vCopy;
}

simple_matrix * simple_multi_graph::get_adj() const { return adj; }

void simple_multi_graph::set_num_sccs( int num ) { num_sccs = num; }

Vertex * simple_multi_graph::get_vertices() const { return vertices; }

void simple_multi_graph::adj_set_entry( int i, int j, int value ) { adj->set_entry(i,j,value); }

int simple_multi_graph::get_num_sccs() const { return num_sccs; }

int simple_multi_graph::adj_entry( int i, int j ) const { return adj->entry(i,j); }

int simple_multi_graph::get_v_size() const { return nVertices; }

Scc * simple_multi_graph::get_sccs() const { return sccs; }

int simple_multi_graph::get_scc_size() const { return num_sccs; }



