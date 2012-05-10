

#include <rose.h>

using namespace std;

#include <rosepoly/RosePollyModel.h>
#include <rosepoly/RosePluto.h>
#include <rosepoly/simple_multi_graph.h>
#include <rosepoly/simple_matrix.h>


// PLUTO SCHEDULING ALGORITHM

RosePluto::RosePluto( const RosePollyModel& model )
: RosePollyModel(model), solSize(0), totalSols(0),
sols_found(0), sols_temp(0), depth(0)
{	
	if ( !ddg ) {
		ddg_create();
		compute_scc();
	}
	
	nvar = new int[stmts.size()];
	cst_width = parameters.size()+1+1; // +1 for w and +1 for the global constant
		
	for ( int i = 0 ; i < stmts.size() ; i++ ) {
		int nest = stmts[i]->get_nestL();
		nvar[i] = nest;
		totalSols = (totalSols >= nest) ? totalSols : nest;
		cst_width += (nest+1);
	}
}

RosePluto::~RosePluto() { delete [] nvar; }

void RosePluto::apply( fuse_type ft )
{
	cout<<"[PLUTO]"<<endl;
	// cout<<"TOTAL DEPS = "<<deps.size()<<endl;
	
	switch (ft)
	{
		case NO_FUSE:
			cut_all_sccs(1);
			h_types.push_back(SCALAR);
			solSize++;
			break;
		case SMART_FUSE:
			if ( cut_sccs_dim_based(1) ) {
				h_types.push_back(SCALAR);
				solSize++;
			}
			break;
		default :
			break;
	}
	
	do {
		find_permutable_hyperplanes( totalSols-sols_found );
		sols_found += sols_temp;
		// cout<<"solutions found = "<<sols_temp<<endl;
	
		if ( sols_temp ) {
			for ( int i = 0 ; i < sols_temp ; i++ ) {
				h_types.push_back(LOOP);
				/* cout<<"satisfied deps = "<<dep_satisfaction_update(solSize+i)<<endl;
				for ( int j = 0 ; j < deps.size() ; j++ )
					if ( deps[j]->isSatisfied() )
						deps[j]->print(2); */
				dep_satisfaction_update(solSize+i);
				ddg_update();
			}
			solSize += sols_temp;
		} else {
			// cout<<"adding a scalar"<<endl;
			h_types.push_back(SCALAR);
			solSize += 1;
			compute_scc();
			
			if ( ddg->get_num_sccs() <= 1 || depth > 10 )
				ROSE_ASSERT(false);
			
			// cout<<"deps satisfied = "<<cut_all_sccs(solSize)<<endl;
			// cut_all_sccs(solSize);
			/* cout<<"[printing deps]"<<endl;
			for ( int i = 0 ; i < deps.size() ; i++ )
				if ( !deps[i]->isSatisfied() )
					deps[i]->print(2);
			cout<<endl; */
			switch (ft)
			{
				case NO_FUSE:
					cut_all_sccs(solSize);
					break;
				case SMART_FUSE:
					cut_smart(solSize);
					break;
				case MAX_FUSE:
					cut_conservative(solSize);
					break;
			}
		}
		depth++;
		
	} while ( sols_found < totalSols || !deps_satisfaction_check() );
	
	detect_hyperplane_properties();
}

void RosePluto::find_permutable_hyperplanes( int max_sols )
{
	ROSE_ASSERT( max_sols >= 0 );
	
	sols_temp = 0;
	bool sol = false;
	
	if ( max_sols == 0 ) return;
	
	pollyDomain * base = get_permutability_constraints();
	pollyDomain * global = NULL;
	simple_matrix * ortho = NULL;
	simple_matrix * non_trivial_sol = NULL;
	// cout<<"max solutions = "<<max_sols<<endl;
	do {
		// cout<<"entering ..."<<endl;
		global = new pollyDomain(*base);
		
		non_trivial_sol = get_non_trivial_sol_constraints();
		ortho = new simple_matrix(0,cst_width);
		
		global->append_inequalities(non_trivial_sol);
		for ( int i = 0 ; i < stmts.size() ; i++ ) {
			// cout<<"ortho "<<i<<endl;
			simple_matrix * temp = get_ortho_constraints(i);
			ortho->append(*temp);
			delete(temp);
		}
		// ortho->print(2);
		global->append_inequalities(ortho);
		sol = !global->is_empty();
		if (sol) {
			change_coeff_order(global);
			global->lexmin(true);  // true for non-negative unknowns
			// global->print(2);
			change_coeff_order(global);
			distribute_solution(global);
			sols_temp++;
			// cout<<"FOUND SOLUTION "<<endl;
			// global->print(2);
		}
		
		delete(ortho);
		delete(non_trivial_sol);
		delete(global);
		// cout<<"exiting ... "<<endl;
	} while ( sols_temp < max_sols && sol );
	
	delete(base);
}

void RosePluto::distribute_solution( pollyDomain * solution )
{
	simple_matrix * mat = solution->get_matrix(false);
	int stm_offset = parameters.size()+1;
	
	for ( int i = 0 ; i < stmts.size() ; i++ ) {
		int * coeffs = (int*)calloc(nvar[i]+1,sizeof(int));
		for ( int j = 0 ; j < mat->get_rows() ; j++ ) {
			for ( int k = stm_offset ; k < stm_offset+nvar[i]+1 ; k++ ) {
				if ( mat->entry(j,k) )
					coeffs[k-stm_offset] = -mat->entry(j,mat->get_columns()-1);
			}
		}
		stm_offset += (nvar[i]+1);
		stmts[i]->add_hyperplane(coeffs);
	}
	delete(mat);
}

void RosePluto::change_coeff_order( pollyDomain * dom ) const
{
	simple_matrix * ineq = dom->get_matrix(true);
	simple_matrix * eq = dom->get_matrix(false);
	
	int start = parameters.size() + 1;
	int Rows = ineq->get_rows();
	for ( int i = 0 ; i < stmts.size() ; i++ ) {
		int end = start + nvar[i];
		int width = end-start;
		for ( int j = 0 ; j < Rows ; j++ ) {
			for ( int k = 0 ; k < width/2 ; k++ ) {
				int temp = ineq->entry(j,start+k);
				ineq->set_entry( j, start+k, ineq->entry(j,start+width-k-1) );
				ineq->set_entry( j, start+width-k-1, temp );
			}
		}
		start = end + 1;
	}
	
	start = parameters.size() + 1;
	Rows = eq->get_rows();
	for ( int i = 0 ; i < stmts.size() ; i++ ) {
		int end = start + nvar[i];
		int width = end-start;
		for ( int j = 0 ; j < Rows ; j++ ) {
			for ( int k = 0 ; k < width/2 ; k++ ) {
				int temp = eq->entry(j,start+k);
				eq->set_entry( j, start+k, eq->entry(j,start+width-k-1) );
				eq->set_entry( j, start+width-k-1, temp );
			}
		}
		start = end + 1;
	}
	
	dom->set_constraints(ineq,eq);

	delete(ineq);
	delete(eq);
}

simple_matrix * RosePluto::get_ortho_constraints( int stm_id ) const
{
	simple_matrix * ortho = new simple_matrix(0,cst_width);
	
	int q,value;
	int hSize = stmts[stm_id]->get_num_hyperplanes();
	// cout<<"hSize = "<<hSize<<endl;
	
	if ( !need_more_hyperplanes(stm_id) || !hSize )
		return ortho;
	
	simple_matrix * Hs = new simple_matrix(hSize,nvar[stm_id]);
	
	for ( int i = 0 ; i < hSize ; i++ ) {
		for ( int j = 0 ; j < nvar[stm_id] ; j++ ) {
			value = stmts[stm_id]->get_trans_entry(i,j);
			Hs->set_entry(i,j,value);
		}
	}
	
	simple_matrix * ortho_sub = get_orthogonal_subspace(Hs);
	
	int stm_offset = parameters.size()+1;
	for ( int i = 0 ; i < stm_id ; i++ )
		stm_offset += (nvar[i]+1);
	
	/* Normilize ortho first */
	for ( int i = 0 ; i < ortho_sub->get_columns() ; i++ )    {
		if (ortho_sub->entry(0,i) == 0) continue;
		int colgcd = (ortho_sub->entry(0,i)>=0)?ortho_sub->entry(0,i):-ortho_sub->entry(0,i);
		int k;
		for ( k = 1 ; k < ortho_sub->get_rows() ; k++ )    {
			if (ortho_sub->entry(k,i) == 0)  break;
			colgcd = gcd(colgcd,(ortho_sub->entry(k,i)>=0)?ortho_sub->entry(k,i):-ortho_sub->entry(k,i));
		}
		if ( k == ortho_sub->get_rows() )   {
			if (colgcd > 1)    {
				for ( int j = 0 ; j < ortho_sub->get_rows() ; j++ )    {
					value = ortho_sub->entry(j,i);
					value /= colgcd;
					ortho_sub->set_entry(j,i,colgcd);
				}
			}
		}
	}
	
	for ( int i = 0 ; i < nvar[stm_id] ; i++ ) {
		int j;
		for ( j = 0 ; j < nvar[stm_id] ; j++ ) {
			if ( ortho_sub->entry(j,i) != 0 ) break;
		}
		/* Ignore all zero cols */
		if ( j == nvar[stm_id] ) continue;
		
		/* Ignore cols that are -ves of previous ones */
		if ( i >= 1 ){
			for ( j = 0 ; j < nvar[stm_id] ; j++ ) {
				if ( ortho_sub->entry(j,i) != -ortho_sub->entry(j,i-1) ) break;
			}
			if ( j == nvar[stm_id] ) continue;
		}
		
		/* We have a non-zero col */
		simple_matrix temp(1,cst_width);
		for ( j = 0 ; j < nvar[stm_id] ; j++ ) {
			value = ortho_sub->entry(j,i);
			temp.set_entry(0,stm_offset+j,value);
		}
		ortho->append(temp);
	}
	
	if ( ortho->get_rows() > 0 )  {
		simple_matrix temp(1,cst_width);
		/* Sum of all of the above is the last constraint */
		for( int i = 0 ; i < cst_width ; i++ )  {
			for ( int j = 0 ; j < ortho->get_rows() ; j++ ) {
				value = temp.entry(0,i);
				value += ortho->entry(j,i);
				temp.set_entry(0,i,value);
			}
		}
		temp.set_entry(0,cst_width-1,-1);
		ortho->append(temp);
	}
	
	delete(ortho_sub);
	delete(Hs);
	
	return ortho;
}

simple_matrix * RosePluto::get_orthogonal_subspace( simple_matrix * H ) const
{
	simple_matrix * Ht = H->transpose();
	simple_matrix * mat1 = H->product(Ht);
	simple_matrix * inv = mat1->inverse();
	simple_matrix * mat2 = inv->product(mat1);
	int scale = mat2->entry(0,0);
	simple_matrix * mat3 = inv->product(H);
	simple_matrix * result = Ht->product(mat3);
	
	for ( int i = 0 ; i < result->get_rows() ; i++ ) {
		for ( int j = 0 ; j < result->get_columns() ; j++ ) {
			int value = ((i==j)?scale:0) - result->entry(i,j);
			result->set_entry(i,j,value);
		}
	}
	
	delete(Ht);
	delete(mat1);
	delete(mat2);
	delete(mat3);
	delete(inv);
	
	return result;
}

bool RosePluto::need_more_hyperplanes( int stm_id ) const
{
	int hyperplanes_found = stmts[stm_id]->get_num_hyperplanes();
	return hyperplanes_found < stmts[stm_id]->get_nestL();
}

simple_matrix * RosePluto::get_non_trivial_sol_constraints() const
{
	simple_matrix * non_trivial = new simple_matrix(0,cst_width);
	int stm_offset;
	int npar = parameters.size();
	
	for ( int i = 0 ; i < stmts.size() ; i++ ) {
		simple_matrix temp(1,cst_width);
		stm_offset = npar+1;
		
		for ( int j = 0; j < i ; j++ )
			stm_offset += (nvar[j]+1);
		
		if ( !need_more_hyperplanes(i) )
			continue;
		
		for ( int j = 0 ; j < nvar[i] ; j++ )
			temp.set_entry(0,stm_offset+j,1);
		
		temp.set_entry(0,cst_width-1,-1);
		non_trivial->append(temp);
	}
	
	return non_trivial;
}

pollyDomain * RosePluto::get_permutability_constraints() const
{
	pollyDomain * global = init_pluto_domain();
	for ( int i = 0 ; i < deps.size() ; i++ )
		if ( !deps[i]->isR_R() && !deps[i]->isSatisfied() && deps[i]->isValid() )
			append_permutability_core(global,i);
		
	return global;
}

void RosePluto::append_permutability_core( pollyDomain * base, int index ) const
{
	pollyMap * d_polly = deps[index]->get_polyhedron();
	simple_matrix * map_mat = d_polly->get_matrix(false);
	simple_matrix * dom_mat = d_polly->get_matrix(true);
	int parSize = parameters.size();
	
	simple_matrix * total = NULL;
	simple_matrix * legality = NULL;
	simple_matrix * bounding = NULL;
	
	int src_id = deps[index]->src();
	int dest_id = deps[index]->dest();
	int src_offset = parSize+1;
	int dest_offset = parSize+1;
	normalize_matrices(dom_mat,map_mat,src_id,dest_id);  // IMPORTANT: this function MUST become hidden at some point cause it is isl-specific
	int nFarkas = dom_mat->get_rows()+1; // +1 for l0
	
	int cst_rows = nvar[dest_id]+parSize+1+1+nFarkas; // +1 for sigma negative row and +1 for constants
	
	for ( int i = 0 ; i < src_id ; i++ )
		src_offset += (nvar[i]+1);
	for ( int i = 0 ; i < dest_id ; i++ )
		dest_offset += (nvar[i]+1);
	
	if ( src_id != dest_id ) {
		legality = new simple_matrix( cst_rows, nvar[dest_id]+1+nvar[src_id]+1+nFarkas+1 ); // +1 for global constant
		bounding = new simple_matrix( cst_rows, parSize+1+nvar[dest_id]+1+nvar[src_id]+1+nFarkas+1 );
	} else {
		legality = new simple_matrix( cst_rows, nvar[dest_id]+1+nFarkas+1 );
		bounding = new simple_matrix( cst_rows, parSize+1+nvar[dest_id]+1+nFarkas+1 );
	}
	
	if ( src_id != dest_id ) {
		
		/* legality constraints */
		int limit = nvar[dest_id]+parSize+1;
		for ( int i = 0 ; i < limit ; i++ ) {
			if ( i < nvar[dest_id] ) {
				legality->set_entry(i,i,1);
			} else if ( i == limit-1 ) {
				legality->set_entry(i,nvar[dest_id],1);
				legality->set_entry(i,nvar[dest_id]+1+nvar[src_id],-1);
				legality->set_entry(i,legality->get_columns()-2,-1);
			}
			
			int Start = nvar[dest_id]+1;
			int End = Start + nvar[src_id];
			for ( int j = Start ; j < End ; j++ ) {
				int value = 0;
				if ( i < nvar[dest_id] )
					value = map_mat->entry(nvar[src_id]-1-j+Start,i); // coeff already negative
				else if ( i == limit-1 )
					value = map_mat->entry(nvar[src_id]-1-j+Start,map_mat->get_columns()-1);
				else
					value = map_mat->entry(nvar[src_id]-1-j+Start,i+nvar[src_id]);
				
				legality->set_entry(i,j,value);
			}
			
			Start = nvar[dest_id]+1+nvar[src_id]+1;
			End = Start + nFarkas-1;
			for ( int j = Start ; j < End ; j++ ) {
				int value = dom_mat->entry(j-Start,i);
				legality->set_entry(i,j,-value);
			}
		}
		
		/* Add sigma negative */
		legality->add_sigma_negative(limit);
		
		/* All Farkas multipliers are non-negative */
		for ( int i = 0 ; i < nFarkas ; i++ ) {
			for ( int j = 0 ; j < nFarkas ; j++ ) {
				int row_pos = limit+1+i;
				int col_pos = nvar[dest_id]+1+nvar[src_id]+1+j;
				legality->set_entry(row_pos,col_pos,(i==j)?1:0);
			}
		}
		
		/* Bounding constraints */
		for ( int i = 0 ; i < parSize+1 ; i++ )
			bounding->set_entry(nvar[dest_id]+i,i,1);
		
		int guard = nvar[dest_id]+1+nvar[src_id]+1;
		for ( int i = 0 ; i < limit ; i++ ) {
			for ( int j = 0 ; j < legality->get_columns() ; j++ ) {
				int value = legality->entry(i,j);
				bounding->set_entry(i,parSize+1+j,(j<guard)?-value:value);
			}
		}
		
		/* Add sigma negative */
		bounding->add_sigma_negative(limit);
		
		/* All Farkas multipliers are non-negative */
		for ( int i = 0 ; i < nFarkas ; i++ ) {
			for ( int j = 0 ; j < nFarkas ; j++ ) {
				int row_pos = limit+1+i;
				int col_pos = parSize+1+nvar[dest_id]+1+nvar[src_id]+1+j;
				bounding->set_entry(row_pos,col_pos,(i==j)?1:0);
			}
		}
		
		legality->eliminate_columns(nvar[dest_id]+1+nvar[src_id]+1,legality->get_columns()-1);
		bounding->eliminate_columns(parSize+1+nvar[dest_id]+1+nvar[src_id]+1,bounding->get_columns()-1);
		
	} else {
		
		/* legality constraints */
		int limit = nvar[dest_id]+parSize+1;
		for ( int i = 0 ; i < limit ; i++ ) {
			if ( i < nvar[dest_id] )
				legality->set_entry(i,i,1);
			else if ( i == limit-1 )
				legality->set_entry(i,legality->get_columns()-2,-1);
			
			for ( int j = 0 ; j < nvar[dest_id] ; j++ ) {
				int value = 0;
				if ( i < nvar[dest_id] )
					value = map_mat->entry(nvar[dest_id]-1-j,i);
				else if ( i == limit-1 )
					value = map_mat->entry(nvar[dest_id]-1-j,map_mat->get_columns()-1);
				
				value += legality->entry(i,j);  // coeffs already negative
				legality->set_entry(i,j,value);
			}
			
			int Start = nvar[dest_id]+1;
			int End = Start + nFarkas-1;
			for ( int j = Start ; j < End ; j++ ) {
				int value = dom_mat->entry(j-Start,i);
				legality->set_entry(i,j,-value);
			}
		}
		
		/* Add sigma negative */
		legality->add_sigma_negative(limit);
		
		/* All Farkas multipliers are non-negative */
		for ( int i = 0 ; i < nFarkas ; i++ ) {
			for ( int j = 0 ; j < nFarkas ; j++ ) {
				int row_pos = limit+1+i;
				int col_pos = nvar[dest_id]+1+j;
				legality->set_entry(row_pos,col_pos,(i==j)?1:0);
			}
		}
		
		/* Bounding constraints */
		for ( int i = 0 ; i < parSize+1 ; i++ )
			bounding->set_entry(nvar[dest_id]+i,i,1);
		
		int guard = nvar[dest_id]+1;
		for ( int i = 0 ; i < limit ; i++ ) {
			for ( int j = 0 ; j < legality->get_columns() ; j++ ) {
				int value = legality->entry(i,j);
				bounding->set_entry(i,parSize+1+j,(j<guard)?-value:value);
			}
		}
		
		/* Add sigma negative */
		bounding->add_sigma_negative(limit);
		
		/* All Farkas multipliers are non-negative */
		for ( int i = 0 ; i < nFarkas ; i++ ) {
			for ( int j = 0 ; j < nFarkas ; j++ ) {
				int row_pos = limit+1+i;
				int col_pos = parSize+1+nvar[dest_id]+1+j;
				bounding->set_entry(row_pos,col_pos,(i==j)?1:0);
			}
		}
		
		legality->eliminate_columns(nvar[dest_id]+1,legality->get_columns()-1);
		bounding->eliminate_columns(parSize+1+nvar[dest_id]+1,bounding->get_columns()-1);
	}
	
	int totalRows = legality->get_rows() + bounding->get_rows();
	total = new simple_matrix(totalRows,base->get_total_size());
	
	/* Add legality */
	int value;
	for ( int i = 0 ; i < legality->get_rows() ; i++ ) {
		for ( int j = 0 ; j < legality->get_columns() ; j++ ) {
			value = legality->entry(i,j);
			if ( j < nvar[dest_id]+1 )
				total->set_entry(i,dest_offset+j,value);
			else if ( j < legality->get_columns()-1 )
				total->set_entry(i,src_offset+j-nvar[dest_id]-1,value);
		}
	}
	
	/* Add bounding csts */
	int Start = legality->get_rows();
	for ( int i = Start ; i < total->get_rows() ; i++ ) {
		for ( int j = 0 ; j < bounding->get_columns() ; j++ ) {
			value = bounding->entry(i-Start,j);
			if ( j < parSize+1 )
				total->set_entry(i,j,value);
			else if ( j < parSize+1+nvar[dest_id]+1 ) 
				total->set_entry(i,dest_offset+j-parSize-1,value);
			else if ( j < bounding->get_columns()-1 )
				total->set_entry(i,src_offset+j-parSize-1-nvar[dest_id]-1,value);
		}
	}
	
	base->append_inequalities(total);
	
	delete(map_mat);
	delete(dom_mat);
	
	delete(total);
	delete(legality);
	delete(bounding);
	
	return;
}

void RosePluto::normalize_matrices( simple_matrix * dom, simple_matrix * map, int src_id, int dest_id ) const
{
	int map_rows = map->get_rows();
	int src_dim = nvar[src_id];
	if ( map_rows > src_dim ) {
		int cols = dom->get_columns();
		int dst_dim = nvar[dest_id];
		int length = map_rows - src_dim;
		int value;
		simple_matrix temp(length+1,cols); // +1 for the sigma negative
		for ( int i = 0 ; i < length ; i++ ) {
			for ( int j = 0 ; j < cols-1 ; j++ ) {  // -1 to handle constant later
				if ( j < dst_dim )
					value = map->entry(src_dim+i,j);
				else
					value = map->entry(src_dim+i,src_dim+j);
				
				temp.set_entry(i,j,value);
			}
			temp.set_entry(i,cols-1,map->entry(src_dim+i,map->get_columns()-1));
		}
		
		temp.add_sigma_negative(length);
		dom->append(temp);
	}
}

pollyDomain * RosePluto::init_pluto_domain() const
{
	pollyDomain * base = new pollyDomain();
	for ( int i = 0 ; i < parameters.size() ; i++ )
		base->add_dim(dim_in,parameters[i]);
	base->add_dim(dim_in,"w");
	
	for ( int i = 0 ; i < stmts.size() ; i++ ) {
		stringstream out;
		for ( int j = 0 ; j < stmts[i]->get_nestL() ; j++ ) {
			stringstream out;
			out<<"a"<<i<<j;
			base->add_dim(dim_in,out.str());
		}
		out<<"c"<<i;
		base->add_dim(dim_in,out.str());
	}
	
	return base;
}

void RosePluto::update_loop_type( int stm_id, int level, loop_type& type ) const
{
	loop_type temp;
	temp = stmts[stm_id]->detect_parallelism( level );
	if ( temp > type )
		type = temp;
}

void RosePluto::set_scc_state( int stm_id, int * scc_state, int width ) const
{
	for ( int i = 0 ; i < width ; i++ )
		scc_state[i] = stmts[stm_id]->get_scc_id(i);
}

void RosePluto::detect_hyperplane_properties() const
{
	int h_size = h_types.size();
	int scc_width = 0;
	int scc_size = 0;
	vector<affineStatement*> buffer;
	
	for ( int i = 0 ; i < h_size ; i++ )
		if ( h_types[i] == SCALAR )
			scc_size++;
	
	int * scc_state = (int*)calloc(scc_size,sizeof(int));
	
	for ( int i = 0 ; i < h_size ; i++ ) {
		
		if ( h_types[i] == SCALAR ) {
			scc_width++;
		} else {
			loop_type type = PARALLEL;
			for ( int j = 0 ; j < stmts.size() ; j++ ) {
				if ( stmts[j]->is_in_nest(scc_state,scc_width) ) {
					update_loop_type(j,i,type);
				} else {
					for ( int k = 0 ; k < buffer.size() ; k++ )
						buffer[k]->push_loop_type(type);
					buffer.clear();
					type = PARALLEL;
					update_loop_type(j,i,type);
					set_scc_state(j,scc_state,scc_width);
				}
				buffer.push_back(stmts[j]);
			}
			
			for ( int j = 0 ; j < buffer.size() ; j++ )
				buffer[j]->push_loop_type(type);
			
			for ( int j = 0 ; j < scc_width ; j++ )
				scc_state[j] = 0;
			
			buffer.clear();
		}
	}
	free(scc_state);
}

int RosePluto::cut_sccs_dim_based( int pos )
{
	int num_satisfied = 0;
	
	if ( ddg->get_num_sccs() == 1 )
		return 0;
	
	int count = 0;
	Scc * sccs = ddg->get_sccs();
	int cur_max_dim = sccs[0].max_dim;
	
	for ( int i = 0 ; i < ddg->get_num_sccs() ; i++ ) {
        if (cur_max_dim != sccs[i].max_dim)   {
            cur_max_dim = sccs[i].max_dim;
            count++;
        }
		
        for ( int j = 0 ; j < stmts.size() ; j++ ) {
            if ( stmts[j]->get_scc_id() == i )  {
                stmts[j]->add_scalar_dim(count);
            }
        }
    }
	
	int num_new_carried;
    num_new_carried = dep_satisfaction_update(pos-1);
	
	if ( !num_new_carried ) {
		for ( int i = 0 ; i < stmts.size() ; i++ )
			stmts[i]->undo_transformation();
	} else {
		ddg_update();
	}
	return num_new_carried;
}

int RosePluto::cut_smart( int pos )
{
	int num_new_carried = 0;
	
	num_new_carried = cut_sccs_dim_based(pos);
	
	if ( num_new_carried )
		return num_new_carried;
	
	int n_sccs = ddg->get_num_sccs();
	num_new_carried = cut_between_sccs(pos,ceil(n_sccs/2.0)-1, ceil(n_sccs/2.0));
	
	if ( num_new_carried )
		return num_new_carried;
	
	for ( int i = 0 ; i < n_sccs-1 ; i++ )
		for ( int j = n_sccs-1 ; j>=i+1 ; j-- )
			num_new_carried += cut_between_sccs(pos,i,j);
	return num_new_carried;
}

int RosePluto::cut_between_sccs( int pos, int sc1, int sc2 )
{
	int num_new_carried;
	
	if ( !sccs_direct_connected(sc1,sc2) )
		return 0;
	
	for ( int i = 0 ; i < stmts.size() ; i++ ) {
		if ( stmts[i]->get_scc_id() < sc2 )
			stmts[i]->add_scalar_dim(0);
		else
			stmts[i]->add_scalar_dim(1);
	}
	num_new_carried = dep_satisfaction_update(pos-1);
	ddg_update();
	return num_new_carried;
}

bool RosePluto::sccs_direct_connected( int sc1, int sc2 ) const
{
	int nstmt = stmts.size();
	for ( int i = 0 ; i < nstmt ; i++ )
		if ( stmts[i]->get_scc_id() == sc1 )
			for ( int j = 0 ; j < nstmt ; j++ )
				if ( stmts[j]->get_scc_id() == sc2 )
					if ( ddg->adj_entry(i,j) > 0 )
						return true;
	return false;
}

int RosePluto::cut_conservative( int pos )
{
	int num_new_carried = cut_sccs_dim_based(pos);
	
	if ( num_new_carried )
		return num_new_carried;
	
	int n_sccs = ddg->get_num_sccs();
	
	num_new_carried = cut_between_sccs(pos,ceil(n_sccs/2.0)-1,ceil(n_sccs/2.0));
	
	if ( num_new_carried )
		return num_new_carried;
	
	for ( int i = 0 ; i < n_sccs-1 ; i++ )
		for ( int j = n_sccs-1 ; j>=i+1 ; j-- )
			num_new_carried = cut_between_sccs(pos,i,j);
	
	return num_new_carried;
}

