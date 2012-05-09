

#ifndef PLUTO_SCHEDULE_H
#define PLUTO_SCHEDULE_H

class simple_multi_graph;

typedef enum {
	NO_FUSE = 0,
	SMART_FUSE = 1,
	MAX_FUSE = 2
} fuse_type;

class RosePluto : public RosePollyModel {
		
	int solSize;
	int totalSols;
	int sols_found;
	int sols_temp;
	int depth;
	int * nvar;
	int cst_width;
	vector<hyperplane_type> h_types;
	
	void find_permutable_hyperplanes( int max_sols );
	
	pollyDomain * init_pluto_domain() const;
	pollyDomain * get_permutability_constraints() const;
	simple_matrix * get_non_trivial_sol_constraints() const;
	simple_matrix * get_ortho_constraints( int stm_id ) const;
	void distribute_solution( pollyDomain * solution );
	void change_coeff_order( pollyDomain * dom ) const;
	simple_matrix * get_orthogonal_subspace( simple_matrix * H ) const;
	void detect_hyperplane_properties() const;
	void update_loop_type( int stm_id, int level, loop_type& type ) const;
	void set_scc_state( int stm_id, int * scc_state, int width ) const;
	
	int cut_sccs_dim_based( int pos );
	int cut_smart( int pos );
	int cut_between_sccs( int pos, int sc1, int sc2 );
	int cut_conservative( int pos );
	bool sccs_direct_connected( int sc1, int sc2 ) const;
	
	bool need_more_hyperplanes( int stm_id ) const;
	
	void append_permutability_core( pollyDomain * base, int index ) const;
	void normalize_matrices( simple_matrix * dom, simple_matrix * map, int src_id, int dest_id ) const;
	
public:
	
	RosePluto( const RosePollyModel& model );
	
	void apply( fuse_type ft );
	
	virtual ~RosePluto();
	
};

#endif

