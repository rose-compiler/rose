/*
 *  RosePollyModel.cpp
 *  
 *
 *  Created by Konstantinidis, Athanasios on 1/13/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <rose.h>

using namespace std;

#include <rosepoly/RosePollyModel.h>
#include <rosepoly/RoseCloog.h>
#include <rosepoly/data.h>
#include <rosepoly/access_pattern.h>
#include <rosepoly/traversals.h>
#include <rosepoly/RosePluto.h>
#include <rosepoly/simple_multi_graph.h>
#include <rosepoly/simple_matrix.h>
#include <rosepoly/Kernel.h>

RosePollyModel::RosePollyModel() : ddg(NULL), ID(-1) {}

RosePollyModel::RosePollyModel( const RosePollyModel& model )
: parameters(model.parameters), data(model.get_data()), ID(model.ID),
ddg(NULL)
{
	if ( model.ddg != NULL )
		ddg = new simple_multi_graph(*model.ddg);
	
	map<int,affineStatement*> temp;
	map<int,affineStatement*>::iterator it;
	for ( int i = 0 ; i < model.stmts.size() ; i++ ) {
		
		affineStatement * stm = new affineStatement( *model.stmts[i] );
		stmts.push_back( stm );
		temp[stm->get_ID()] = stm;
		
		stm->reset_patterns( data );
		
		polly_iterator<affineDependence> d_iter = stm->get_deps();
		
		if ( d_iter.empty() )
			continue;
		
		do {
			deps.push_back(*d_iter);
			d_iter->reset_dest(stm);
		} while (d_iter.next());
		
	}
	
	for ( int i = 0 ; i < deps.size() ; i++ ) {
		it = temp.find(deps[i]->src());
		deps[i]->reset_source(it->second);
	}
}

RosePollyModel::RosePollyModel( vector<affineStatement*>& s, Kernel * k )
: parameters(k->get_params()), data(k->get_data()), ddg(NULL), ID(k->get_ID())
{
	map<int,affineStatement*> temp;
	map<int,affineStatement*>::iterator it;
	for ( int i = 0 ; i < s.size() ; i++ ) {
		affineStatement * stm = new affineStatement( *s[i] );
		stmts.push_back( stm );
		temp[stm->get_ID()] = stm;
		
		stm->reset_patterns( data );
		
		polly_iterator<affineDependence> d_iter = stm->get_deps();
		
		if ( d_iter.empty() )
			continue;
		
		do {
			deps.push_back(*d_iter);
			d_iter->reset_dest(stm);
		} while (d_iter.next());
		
	}
	
	for ( int i = 0 ; i < deps.size() ; i++ ) {
		it = temp.find(deps[i]->src());
		deps[i]->reset_source(it->second);
	}
}

RosePollyModel::~RosePollyModel() 
{
	for ( int i = 0 ; i < stmts.size() ; i++ )
		delete(stmts[i]);
	delete(ddg);
}

const symbol_table& RosePollyModel::get_data() const { return data; }

polly_iterator<affineStatement> RosePollyModel::get_statements()
{
	return polly_iterator<affineStatement>(stmts.begin(),stmts.end());
}

polly_iterator<affineDependence> RosePollyModel::get_dependencies()
{
	return polly_iterator<affineDependence>(deps.begin(),deps.end());
}

int RosePollyModel::get_id() const { return ID; }

void RosePollyModel::print( int ident, bool print_deps ) const
{
	for ( int i = 0 ; i < ident ; i++ )
		cout<<" ";
	cout<<"[POLYHEDRAL MODEL of Kernel ("<<ID<<")]"<<endl;
	cout<<endl;
	
	for ( int i = 0 ; i < stmts.size() ; i++ )
		stmts[i]->print(ident);
	
	cout<<endl;
	
	if ( !print_deps ) return;
	
	for ( int i = 0 ; i < ident ; i++ )
		cout<<" ";
	cout<<"[DEPENDENCIES]"<<endl;
	cout<<endl;
	
	cout<<"TOTAL = "<<deps.size()<<endl;
	cout<<endl;
	for ( int i = 0 ; i < ident ; i++ )
		cout<<" ";
	
	cout<<"__TRUE_DEPENDENCIES__"<<endl;
	for ( int i = 0 ; i < deps.size() ; i++ )
		if ( deps[i]->isW_R() )
			deps[i]->print(ident);
	
	cout<<endl;
	for ( int i = 0 ; i < ident ; i++ )
		cout<<" ";
	
	cout<<"__ANTI_DEPENENCIES__"<<endl;
	for ( int i = 0 ; i < deps.size() ; i++ )
		if ( deps[i]->isR_W() )
			deps[i]->print(ident);
	
	cout<<endl;
	for ( int i = 0 ; i < ident ; i++ )
		cout<<" ";
	
	cout<<"__OUTPUT_DEPENENCIES__"<<endl;
	for ( int i = 0 ; i < deps.size() ; i++ )
		if ( deps[i]->isW_W() )
			deps[i]->print(ident);
	
	cout<<endl;
	for ( int i = 0 ; i < ident ; i++ )
		cout<<" ";
	
	cout<<"__INPUT_DEPENENCIES__"<<endl;
	for ( int i = 0 ; i < deps.size() ; i++ )
		if ( deps[i]->isR_R() )
			deps[i]->print(ident);
}

void RosePollyModel::loop_fission()
{
	if ( !ddg ) {
		ddg_create();
		compute_scc();
	}
	
	for ( int i = 0 ; i < stmts.size() ; i++ )
		stmts[i]->add_scalar_dim();
}

void RosePollyModel::loop_skewing( int width, bool top )
{
	for ( int i = 0 ; i < stmts.size() ; i++ )
		stmts[i]->skew(width,top);
}

void RosePollyModel::ddg_create()
{
	ddg = new simple_multi_graph(stmts.size());
	
	for ( int i = 0 ; i < deps.size() ; i++ ) {
		/* no input dep edges in the graph */
		if ( deps[i]->isR_R() || !deps[i]->isValid() ) continue;
		
		/* remember it's a multi-graph */
		int src_id = deps[i]->src();
		int dest_id = deps[i]->dest();
		
		int temp = ddg->adj_entry(src_id,dest_id);
		int value = (!deps[i]->isSatisfied()) ? temp + 1 : temp;
		
		ddg->adj_set_entry( src_id, dest_id, value );
	}
}

void RosePollyModel::compute_scc() 
{
	ddg->dfs();
	
	simple_multi_graph * gT = ddg->graph_transpose();
	
	gT->dfs_for_scc();
	
	ddg->set_num_sccs( gT->get_num_sccs() );
	
	Vertex * g_vert = ddg->get_vertices();
	Vertex * gT_vert = gT->get_vertices();
	for ( int i = 0 ; i < ddg->get_v_size() ; i++ ) {
		g_vert[i].scc_id = gT_vert[i].scc_id;
		ROSE_ASSERT(gT_vert[i].id == i);
		stmts[i]->set_scc_id( g_vert[i].scc_id );
	}
	
	Scc * g_scc = ddg->get_sccs();
	Scc * gT_scc = gT->get_sccs();
	for ( int i = 0 ; i < ddg->get_scc_size() ; i++ ) {
		g_scc[i].max_dim = get_max_dim_in_scc(i);
		g_scc[i].size = get_scc_size(i);
		g_scc[i].id = gT_scc[i].id;
	}
	
	delete(gT);
}

void RosePollyModel::ddg_update()
{
	int Size = ddg->get_v_size();
	for ( int i = 0 ; i < Size ; i++ )
		for ( int j = 0 ; j < Size ; j++ )
			ddg->adj_set_entry(i,j,0);
	
	for ( int i = 0 ; i < deps.size() ; i++ ) {
		if ( deps[i]->isR_R() || !deps[i]->isValid() ) continue;
		int src_id = deps[i]->src();
		int dest_id = deps[i]->dest();
		
		int temp = ddg->adj_entry(src_id,dest_id);
		int value = (!deps[i]->isSatisfied()) ? temp+1 : temp;
		
		ddg->adj_set_entry( src_id, dest_id, value );
	}
}

int RosePollyModel::cut_all_sccs( int pos )
{
	int num_satisfied = 0;
	
	if ( ddg->get_num_sccs() == 1 )
		return 0;
	
	loop_fission();
	
	num_satisfied = dep_satisfaction_update(pos-1);
	ddg_update();
	
	return num_satisfied;
}

int RosePollyModel::get_max_dim_in_scc( int scc_id ) const
{
	int max = -1;
	for ( int i = 0 ; i < stmts.size() ; i++ )
		if ( stmts[i]->get_scc_id() == scc_id )
			max = (max>=stmts[i]->get_nestL()) ? max : stmts[i]->get_nestL();
	
	return max;
}

int RosePollyModel::get_scc_size( int scc_id ) const
{
	int num = 0;
	for ( int i = 0 ; i < stmts.size() ; i++ )
		if ( stmts[i]->get_scc_id() == scc_id )
			num++;
	
	return num;
}

int RosePollyModel::dep_satisfaction_update( int level )
{
	int num_new_carried = 0;
	
	for ( int i = 0 ; i < deps.size() ; i++ ) {
		if ( deps[i]->isValid() &&
			deps[i]->satisfaction_test(level) )
			num_new_carried++;
	}
	return num_new_carried;
}

bool RosePollyModel::deps_satisfaction_check() const
{
	for ( int i = 0 ; i < deps.size() ; i++ ) {
		if ( deps[i]->isR_R() || !deps[i]->isValid() ) continue;
		if ( !deps[i]->isSatisfied() )
			return false;
	}
	return true;
}

// AFFINE STATEMENT

affineStatement::affineStatement( const Statement& s, 
								 const pollyDomain& d, vector<int> seq_sch, RosePollyCustom * c )
: Statement(s), scc_id(0), nestL(d.get_dim_size(dim_in)), Domain(new pollyDomain(d)), 
seq_schedule(new pollyMap(Domain,seq_sch)), 
transformation(new pollyMap(d)) 
{
	set_affine_patterns(c);
}

affineStatement::affineStatement( const affineStatement& s )
: Statement(s), Domain(new pollyDomain(*s.Domain)), 
seq_schedule(new pollyMap(*s.seq_schedule)), 
transformation(new pollyMap(*s.transformation)), h_types(s.h_types),
nestL(s.nestL), scc_id(s.scc_id), scc_vec(s.scc_vec) 
{
	for ( int i = 0 ; i < s.Deps.size() ; i++ )
		Deps.push_back( new affineDependence(*s.Deps[i]) );
}


affineStatement::~affineStatement()
{
	delete(Domain);
	delete(seq_schedule);
	delete(transformation);
	
	for ( int i = 0 ; i < Deps.size() ; i++ )
		delete(Deps[i]);
}

void affineStatement::set_affine_patterns( RosePollyCustom * c )
{
	for ( int i = 0 ; i < Reads.size() ; i++ )
		Reads[i]->set_polly_map(Domain,c);
	
	for ( int i = 0 ; i < Writes.size() ; i++ )
		Writes[i]->set_polly_map(Domain,c);
}

void affineStatement::reset_patterns( const symbol_table& new_table )
{
	Datum * parent;
	AccessPattern * new_pat;
	symbol_table::const_iterator it;
	
	for ( int i = 0 ; i < Reads.size() ; i++ ) {
		parent = Reads[i]->get_parent();
		it = new_table.find(parent->get_name());
		new_pat = it->second.get_pattern(Reads[i]->get_id());
		Reads[i] = new_pat;
	}
	
	for ( int i = 0 ; i < Writes.size() ; i++ ) {
		parent = Writes[i]->get_parent();
		it = new_table.find(parent->get_name());
		new_pat = it->second.get_pattern(Writes[i]->get_id());
		Writes[i] = new_pat;
	}
}

void affineStatement::set_scc_id( int id )
{
	// scc_vec.push_back(id);
	scc_id = id;
}

void affineStatement::add_dep( affineDependence * dep )
{
	Deps.push_back(dep);
}

void affineStatement::undo_transformation()
{
	if ( !h_types.size() )
		return;
	else if ( h_types.back() == SCALAR )
		scc_vec.pop_back();
	
	transformation->undo();
}

void affineStatement::add_scalar_dim( int num )
{	
	h_types.push_back(SCALAR);
	int * coeffs = (int*)calloc(nestL+1,sizeof(int));
	coeffs[nestL] = (num==-1) ? scc_id : num;
	scc_vec.push_back(coeffs[nestL]);
	transformation->add_hyperplane(coeffs,nestL);
	delete(coeffs);
}

void affineStatement::add_hyperplane( int * coeffs )
{
	h_types.push_back(LOOP);
	transformation->add_hyperplane(coeffs,nestL);
	delete(coeffs);
}

pollyDomain * affineStatement::get_domain() const { return Domain; }

int affineStatement::get_nestL() const { return nestL; }

int affineStatement::get_num_reads() const { return Reads.size(); }

int affineStatement::get_num_writes() const { return Writes.size(); }

vector<string> affineStatement::get_iterVector() const { return Domain->get_dims(dim_in); }

int affineStatement::scc_vec_size() const { return scc_vec.size(); }

int affineStatement::get_scc_id( int pos ) const 
{ 
	/* assert(scc_vec.size());
	if ( pos != -1 ) {
		assert(pos<scc_vec.size());
		return scc_vec[pos];
	} else {
		return scc_vec.back();
	} */
	
	return (pos==-1) ? scc_id : scc_vec[pos];
}

void affineStatement::skew( int width, int top )
{
	vector<int> skew_dims;
	int level = 0;
	int h_size = h_types.size();
	for ( int i = 0 ; i < h_size ; i++ ) {
		if ( h_types[i] == LOOP ) {
			if ( l_types[level] == PIP_PARALLEL )
				skew_dims.push_back(i);
			level++;
		}
	}
	
	int depth = skew_dims.size();
	
	if ( depth < width )
		return;
	
	h_size = (top && depth > width) ? width : depth;
	int start = (top||h_size<width) ? 0 : h_size-width;
	
	for ( int i = start+1 ; i < h_size ; i++ )
		l_types[i] = PARALLEL;
	
	for ( int i = 0 ; i < l_types.size() ; i++ )
		if ( l_types[i] != PARALLEL )
			l_types[i] = SEQ;
	
	transformation->skew(skew_dims,width,top);
}

vector<string> affineStatement::get_params() const { return Domain->get_dims(dim_param); }

bool affineStatement::has_valid_read_map( int pos ) const { return Reads[pos]->has_valid_map(); }

bool affineStatement::has_valid_write_map( int pos ) const { return Writes[pos]->has_valid_map(); }

pollyMap * affineStatement::get_transformation() const { return transformation; }

string affineStatement::get_write_name( int pos ) const 
{
	ROSE_ASSERT( pos < Writes.size() );
	return Writes[pos]->get_name();
}

int affineStatement::get_num_hyperplanes() const
{
	int count = 0;
	for ( int i = 0 ; i < h_types.size() ; i++ )
		if ( h_types[i] == LOOP )
			count++;
	return count;
}

bool affineStatement::is_in_nest( int * scc, int width ) const
{
	for ( int i = 0 ; i < width ; i++ )
		if ( scc[i] != scc_vec[i] )
			return false;
	
	return true;
}

loop_type affineStatement::detect_parallelism( int level ) const
{
	for ( int i = 0 ; i < Deps.size() ; i++ ) {
		if ( !Deps[i]->isR_R() ) {
			assert(Deps[i]->isSatisfied());
			if ( Deps[i]->get_satisfaction_level() == level )
				return PIP_PARALLEL;
		}
	}
	return PARALLEL;
}

string affineStatement::get_read_name( int pos ) const
{
	ROSE_ASSERT( pos < Reads.size() );
	return Reads[pos]->get_name();
}

int affineStatement::get_trans_entry( int dim, int pos ) const
{
	return transformation->get_entry(dim,pos);
}

polly_iterator<affineDependence> affineStatement::get_deps()
{
	
	return polly_iterator<affineDependence>(Deps.begin(),Deps.end());
}

void affineStatement::print( int ident ) const
{
	for ( int i = 0 ; i < ident ; i++ )
		cout<<" ";
	
	cout<<"[STATEMENT : "<<myStatement->unparseToString()<<"]"<<endl;
	
	for ( int i = 0 ; i < ident ; i++ )
		cout<<" ";
	
	cout<<"__domain__"<<endl;
	Domain->print( ident );
	cout<<endl;
	
	for ( int i = 0 ; i < ident ; i++ )
		cout<<" ";
	
	cout<<"__reads__"<<endl;
	for ( int i = 0 ; i < Reads.size() ; i++ )
		Reads[i]->print(ident);
	cout<<endl;
	
	for ( int i = 0 ; i < ident ; i++ )
		cout<<" ";
	
	cout<<"__writes__"<<endl;
	for ( int i = 0 ; i < Writes.size() ; i++ )
		Writes[i]->print(ident);
	cout<<endl;
	
	if ( !seq_schedule )
		return;
	
	for ( int i = 0 ; i < ident ; i++ )
		cout<<" ";
	cout<<"__seq_schedule__"<<endl;
	seq_schedule->print(ident);
	cout<<endl;
	
	for ( int i = 0 ; i < ident ; i++ )
		cout<<" ";
	cout<<"__transformation__"<<endl;
	transformation->print(ident);
	cout<<endl;
	
	for ( int i = 0 ; i < ident ; i++ )
		cout<<" ";
	cout<<"__loop_types__"<<endl;
	for ( int i = 0 ; i < l_types.size() ; i++ ) {
		for ( int j = 0 ; j < ident+2 ; j++ )
			cout<<" ";
		switch (l_types[i])
		{
			case PARALLEL:
				cout<<"PARALLEL"<<endl;
				break;
			case PIP_PARALLEL:
				cout<<"PIPELINE"<<endl;
				break;
			case SEQ:
				cout<<"SEQUENTIAL"<<endl;
				break;
			default:
				cout<<"UNDEFINED"<<endl;
				break;
		}
	}
	cout<<endl;
}

void affineStatement::print_deps( int ident ) const
{
	for ( int i = 0 ; i < Deps.size() ; i++ )
		Deps[i]->print(ident);
}


// AFFINE DEPENDENCE

affineDependence::affineDependence( const affineDependence& dep )
: Source(dep.Source), Destination(dep.Destination), s_ref(dep.s_ref), 
d_ref(dep.d_ref), type(dep.type), valid(dep.valid), satisfied(dep.satisfied),
satisfaction_level(dep.satisfaction_level), D_Polyhedron(new pollyMap(*dep.D_Polyhedron)) 
{}

affineDependence::affineDependence( 
			affineStatement * s, affineStatement * d, int sR, int dR, dependence_type t,
			pollyMap * dP )
: Source(s), Destination(d), s_ref(sR), d_ref(dR), type(t), valid(true),
satisfied(false), satisfaction_level(-1) 
{
	D_Polyhedron = new pollyMap(*dP);
}

affineDependence::~affineDependence() { delete(D_Polyhedron); }

bool affineDependence::satisfaction_test( int level ) 
{
	if ( satisfied )
		return false;

	pollyMap * t_src = Source->get_transformation();
	pollyMap * t_dest = Destination->get_transformation();
	
	if ( D_Polyhedron->satisfaction_test( level, t_src, t_dest ) ) {
		satisfied = true;
		satisfaction_level = level;
		return true;
	}
	return false;
}

void affineDependence::reset_source( affineStatement * s ) { Source = s; }

void affineDependence::reset_dest( affineStatement * d ) { Destination = d; }

int affineDependence::src() const { return Source->get_ID(); }

int affineDependence::dest() const { return Destination->get_ID(); }

affineStatement * affineDependence::get_src() const { return Source; }

affineStatement * affineDependence::get_dest() const { return Destination; }

bool affineDependence::isR_R() const { return type==R_R; }

bool affineDependence::isW_R() const { return type==W_R; }

bool affineDependence::isR_W() const { return type==R_W; }

bool affineDependence::isW_W() const { return type==W_W; }

bool affineDependence::isValid() const { return valid; }

pollyMap * affineDependence::get_polyhedron() const { return D_Polyhedron; }

bool affineDependence::isSatisfied() const { return satisfied; }

int affineDependence::get_satisfaction_level() const { return satisfaction_level; }

void affineDependence::print( int ident, int * count, int src, int dest ) const
{
	if (!valid)
		return;
	
	if ( src >= 0 || dest >= 0 ) {
		if ( src != Source->get_ID() && dest != Destination->get_ID() )
			return;
	}
	
	if ( count ) {
		(*count)++;
		cout<<(*count)<<".";
	}
	
	string s_id = (type==W_W||type==W_R) ? "write" : "read";
	string d_id = (type==W_W||type==R_W) ? "write" : "read";
	
	for ( int i = 0 ; i < ident ; i++ )
		cout<<" ";
	cout<<"["<<s_id<<"("<<Source->get_ID()<<":"<<s_ref<<") --> "<<d_id<<"("<<Destination->get_ID()<<":"<<d_ref<<")]"<<endl;
	
	D_Polyhedron->print(ident);
}



















