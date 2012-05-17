

#ifndef SIMPLE_MULTI_GRAPH_H
#define SIMPLE_MULTI_GRAPH_H

typedef struct {
	
	int id;
	int vn;
	int fn;
	
	/* Id of the SCC this vertex belongs to */
	int scc_id;
} Vertex;

typedef struct {
	
	/* Number of vertices in it */
	int size;
	
	int max_dim;
	
	/* Id of this SCC */
	int id;
	
} Scc;

class simple_matrix;

class simple_multi_graph {
	
	Vertex * vertices;
	int nVertices;
	
	/* Adjacency matrix */
	simple_matrix * adj;
	
	Scc * sccs;
	int num_sccs;
	
public:
	
	simple_multi_graph( const simple_multi_graph& g );
	simple_multi_graph( int n );
	
	~simple_multi_graph();
	
	void adj_set_entry( int i, int j, int value );
	void dfs();
	simple_multi_graph * graph_transpose();
	void dfs_for_scc();
	void set_num_sccs( int num );
	void dfs_vertex( int pos, int * time );
	
	int get_num_sccs() const;
	int adj_entry( int i, int j ) const;
	int get_v_size() const;
	Vertex * get_vertices() const;
	Scc * get_sccs() const;
	int get_scc_size() const;
	simple_matrix * get_adj() const;
	
};

#endif

