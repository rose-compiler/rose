
#ifndef POLLY_MODEL_H
#define POLLY_MODEL_H

#include <rosepoly/data.h>
#include <rosepoly/FlowGraph.h>
#include <rosepoly/RosePollyMath.h>


typedef enum {
	ALL = -1,
	W_R = 0,
	R_W = 1,
	W_W = 2,
	R_R = 3
} dependence_type;

typedef enum {
	ORIGINAL = 0,
	PLUTO	 = 1,
	PLUTO_NO_FUSE = 2,
	PLUTO_SMART_FUSE = 3,
	PLUTO_MAX_FUSE = 4
} schedule_type;

typedef enum {
	SCALAR = 0,
	LOOP   = 1
} hyperplane_type;

class affineDependence;
class simple_multi_graph;
class SgCloog;
class Kernel;
class RosePollyCustom;

class affineStatement : public Statement {
	
	pollyDomain * Domain;
	pollyMap * seq_schedule;
	pollyMap * transformation;
	
	vector<hyperplane_type> h_types;
	
	vector<affineDependence*> Deps;
	
	int nestL;
	int scc_id;  // this is a temporary scc_id, if a cutting function is called then this gets into scc_vec
	vector<int> scc_vec;
	
	void set_affine_patterns( RosePollyCustom * c );
	
public:
	
	affineStatement( const Statement& s, const pollyDomain& d, 
					vector<int> seq_sch, RosePollyCustom * c );
	affineStatement( const affineStatement& s );
	
	virtual ~affineStatement();
	
	void add_dep( affineDependence * dep );
	void set_scc_id( int id );
	
	void add_scalar_dim( int num = -1 );
	void add_hyperplane( int * coeffs );
	void skew( int width, int top );
	void undo_transformation();
	
	void reset_patterns( const symbol_table& new_table );
	
	pollyDomain * get_domain()				  const;
	int get_nestL()							  const;
	vector<string> get_iterVector()			  const;
	vector<string> get_params()				  const;
	int get_num_reads()						  const;
	int get_num_writes()					  const;
	int get_scc_id( int pos = -1 )			  const;
	int scc_vec_size()						  const;
	string get_write_name( int pos )		  const;
	string get_read_name( int pos )			  const;
	bool has_valid_read_map( int pos )		  const;
	bool has_valid_write_map( int pos )	  const;
	pollyMap * get_transformation()			  const;
	int get_num_hyperplanes()				   const;
	int get_trans_entry( int dim, int pos )  const; // i=row, j=column
	bool is_in_nest( int * scc, int width )  const;
	loop_type detect_parallelism( int level ) const;  // level might be parallel for this statement but not globaly
	
	polly_iterator<affineDependence> get_deps();
	
	virtual void print( int ident )	const;
	void print_deps( int ident ) const;
	
};

class affineDependence
{
protected:
	
	affineStatement * Source;
	affineStatement * Destination;
	int s_ref;
	int d_ref;
	dependence_type type;
	bool valid;
	
	pollyMap * D_Polyhedron;
	
	bool satisfied;
	int satisfaction_level;
	
public:
	
	affineDependence( const affineDependence& dep );
	affineDependence( affineStatement * s, affineStatement * d, int sR, int dR, dependence_type t,
					 pollyMap * dP);
	
	~affineDependence();
	
	bool satisfaction_test( int level ); // test and set
	void reset_source( affineStatement * s );
	void reset_dest( affineStatement * d );
	
	int src() const;
	int dest() const;
	
	affineStatement * get_src() const;
	affineStatement * get_dest() const;
	
	bool isR_R() const;
	bool isW_R() const;
	bool isR_W() const;
	bool isW_W() const;
	bool isValid() const;
	
	pollyMap * get_polyhedron() const;
	bool isSatisfied() const;
	int get_satisfaction_level() const;
	
	void print( int ident, int * count = NULL, int src = -1, int dest = -1 ) const;
	
};

class RosePollyModel {
	
protected:
	
	vector<affineStatement*> stmts;
	vector<affineDependence*> deps;
	vector<string> parameters;
	symbol_table data;
	int ID;
	
	simple_multi_graph * ddg;
		
	void ddg_create();
	void compute_scc();
	void ddg_update();
	int get_max_dim_in_scc( int scc_id ) const;
	int get_scc_size( int scc_id ) const;
	int cut_all_sccs( int pos );
		
public:
	
	RosePollyModel();
	RosePollyModel( const RosePollyModel& model );
	RosePollyModel( vector<affineStatement*>& s, Kernel * k );
	
	polly_iterator<affineStatement> get_statements();
	polly_iterator<affineDependence> get_dependencies();
	
	int dep_satisfaction_update( int level );
	bool deps_satisfaction_check() const;
	int get_id() const;
	const symbol_table& get_data() const;
	
	void loop_fission();
	void loop_skewing( int width, bool top );
	
	void print( int ident = 0, bool print_deps=false ) const;
	
	virtual ~RosePollyModel();
};

#endif

