#include <rose.h>

using namespace std;

#include <rosepoly/RosePollyModel.h>
#include <rosepoly/traversals.h>
#include <rosepoly/data.h>
#include <rosepoly/utilities.h>
#include <rosepoly/error.h>
#include <rosepoly/access_pattern.h>
#include <rosepoly/RosePollyCustom.h>

extern polly_context context;


FlowGraphTraversal::FlowGraphTraversal( Direction d ) 
: D(d), termination(false), iter(NULL) {}

void FlowGraphTraversal::traverse( BasicNode * start )
{
	if ( start == NULL )
		return;
	
	iter = start;
	
	do {
		this->visit(iter);
	} while ( this->next() && !termination );
}

bool FlowGraphTraversal::next()
{
	if (iter != NULL) iter = iter->next(D);
	
	return iter != NULL;
}

// EVALUATION TRAVERSAL

PollyEvaluateTraversal::PollyEvaluateTraversal( RosePollyCustom * c )
: FlowGraphTraversal(), policy(c), result(true) {}

void PollyEvaluateTraversal::visit( BasicNode * aNode )
{
	NodeType t = aNode->get_type();
	
	switch (t)
	{
		case LOOPHEAD:
		{
			ForLoop * l = (ForLoop*)aNode;
			if ( !policy->evaluate_loop(l) ) {
				report_error( "Non-affine loop bounds", l->get_loop() );
				result = false;
				termination = true;
				break;
			}
			break;
		}
		case LOOPTAIL:
			policy->evaluate_loop((ForLoop*)aNode);
			break;
		case CONDITIONHEAD:
		{
			Conditional * c = (Conditional*)aNode;
			if ( !policy->evaluate_conditional( c ) ) {
				report_error( "Non-affine conditional", c->get_exp() );
				result = false;
				termination = true;
			}
			break;
		}
		case STATEMENT:
		{
			Statement * s = (Statement*)aNode;
			polly_iterator<AccessPattern> ap_iter = s->get_reads();
			/* Check reads */
			if ( !ap_iter.empty() )
				do {
					if ( !policy->evaluate_access(*ap_iter) ) {
						report_error( "Non-affine read pattern", ap_iter->get_refExp() );
						result = false;
						termination = true;
						break;
					}
				} while ( ap_iter.next() );
			
			if ( termination )
				break;
			
			ap_iter = s->get_writes();
			/* Check writes */
			do {
				if ( !policy->evaluate_access(*ap_iter) ) {
					report_error( "Non-affine read pattern", ap_iter->get_refExp() );
					result = false;
					termination = true;
					break;
				}
			} while ( ap_iter.next() );
			break;
		}
		default:
			return;
	}
}


// MODEL EXTRACTION

PollyModelExtractionTraversal::PollyModelExtractionTraversal( vector<string> p, RosePollyCustom * c )
: FlowGraphTraversal(), params(p), policy(c)
{
	pollyDomain * temp = new pollyDomain(p);
	DomainStack.push_back(temp);
	schBuf.push_back(0);
}

PollyModelExtractionTraversal::~PollyModelExtractionTraversal()
{
	pollyDomain * toBeDeleted = DomainStack.back();
	DomainStack.pop_back();
	ROSE_ASSERT(!DomainStack.size());
	delete(toBeDeleted);
}

void PollyModelExtractionTraversal::consolidate()
{
	for ( int i = 0 ; i < stmts.size() ; i++ )
		stmts[i]->replace(toBeReplaced[i]);
}

void PollyModelExtractionTraversal::visit( BasicNode * aNode )
{
	pollyDomain * Domain = DomainStack.back();
	switch (aNode->get_type())
	{
		case LOOPHEAD:
		{
			pollyDomain * newDomain = new pollyDomain(*Domain);
			newDomain = policy->add_loop(newDomain,(ForLoop*)aNode);
			DomainStack.push_back(newDomain);
			schBuf.push_back(0);
		}
			break;
		case LOOPTAIL:
		{
			DomainStack.pop_back();
			schBuf.pop_back();
			schBuf.back()++;
			delete(Domain);
		}
			break;
		case CONDITIONHEAD:
		{
			pollyDomain * newDomain = new pollyDomain((pollyDomain&)(*Domain));
			newDomain = policy->add_conditional( newDomain, (Conditional*)aNode );
			DomainStack.push_back(newDomain);
		}
			break;
		case CONDITIONTAIL:
		{
			DomainStack.pop_back();
			delete(Domain);
		}
			break;
		case STATEMENT:
		{
			affineStatement * stm = new affineStatement((Statement&)(*aNode),*Domain,schBuf,policy);
			stmts.push_back(stm);
			toBeReplaced.push_back((Statement*)aNode);
			schBuf.back()++;
		}
			break;
	}
}


// DEPENDENCE TRAVERSAL
DependenceTraversal::DependenceTraversal()
: FlowGraphTraversal(BACKWARD), Domain(NULL), Dest(NULL), llv(NULL),
llvSize(0), liveness(false), type(ALL) {}

void DependenceTraversal::init( affineStatement * s, dependence_type t )
{
	if ( llv != NULL)
		delete [] llv;
	if ( !params.size() )
		params = s->get_params();
	
	Dest = s;
	llv = new int[s->get_nestL()];
	llvSize = s->get_nestL();
	type = t;
}

DependenceTraversal::~DependenceTraversal()
{
	delete(Domain); delete [] llv;
}

void DependenceTraversal::set_ref( int ref )
{
	destRef = ref;
	destName = (type==W_W||type==R_W) ? Dest->get_write_name(ref) : Dest->get_read_name(ref);
	llvIndex = 0;
	
	for ( int i = 0 ; i < llvSize ; i++ )
		llv[i] = 2;
	
	if (Domain) delete(Domain);
	Domain = new pollyDomain((*Dest->get_domain()));
}

void DependenceTraversal::visit( BasicNode * Node )
{
	switch ( Node->get_type() )
	{
		case LOOPHEAD:
		{
			ForLoop * loop = (ForLoop*)Node;
			if ( regions.empty() ) {
				llv[llvIndex] = 1;
				loop->iterate();
				regions.push_back(HEAD);
			} else if ( regions.back() == COMPLETE ) {
				regions.pop_back();
				llv[llvIndex] = 0;
				llvIndex++;
			} else { regions.pop_back(); }
		}
			break;
		case LOOPTAIL:
			if ( !regions.empty() && regions.back() == HEAD )
				regions.back() = COMPLETE;
			else
				regions.push_back(TAIL);
			break;
		case STATEMENT:
		{
			if ( Domain->is_empty() )
				return;
			
			affineStatement * source = (affineStatement*)Node;
			int num_refs = (type==W_W||type==W_R) ? source->get_num_writes() : source->get_num_reads();
			
			/* Liveness analysis */
			if ( type==R_R||type==R_W) {
				// cout<<"-liveness ENTER"<<endl;
				
				liveness = true;
				
				for ( int i = 0 ; i < source->get_num_writes() ; i++ ) {
					string srcName = source->get_write_name(i);
					if ( destName == srcName ) {
						// cout<<"-match found -> stm("<<source->get_ID()<<") and ref("<<i<<")"<<endl;
						dependence_test(source,i);
					}
				}
				
				// cout<<"-liveness EXIT"<<endl;
				liveness = false;
			}
			
			for ( int i = 0 ; i < num_refs ; i++ ) {
				string srcName = (type==W_W||type==W_R) ? source->get_write_name(i) : source->get_read_name(i);
				if ( destName == srcName ) {
					/* cout<<"-match found -> stm("<<source->get_ID()<<") and ref("<<i<<")"<<endl;
					for ( int j = 0 ; j < llvSize ; j++ )
						cout<<llv[j]<<" ";
					cout<<endl; */
					dependence_test(source,i);
				}
			}
		}
	}
}

void DependenceTraversal::dependence_test( affineStatement * source, int ref )
{
	/* Sanity check */
	if ( type == W_R || type == W_W ) {
		if ( !source->has_valid_write_map(ref) )
			return;
	} else {
		if ( !source->has_valid_read_map(ref) )
			return;
	}
	
	/* Step 1 : Initiate the dependence mapping with the order constraints */
	pollyMap * depMap = new pollyMap(source->get_domain(),Domain,llv);
	// depMap->print(2);
	
	/* Step 2 : Add the remaining domain of the destination statement */
	depMap->intersect_domain(Domain);
	// depMap->print(2);
	
	/* Step 3 : Add the domain of the source statement */
	depMap->intersect_range(source->get_domain());
	
	/* Step 4 : Add conflict constraints */
	AccessPattern * d_pat = (type==W_W||type==R_W) ? Dest->get_write(destRef) : Dest->get_read(destRef);
	AccessPattern * s_pat;
	
	if ( liveness )
		s_pat = source->get_write(ref);
	else
		s_pat = (type==W_W||type==W_R) ? source->get_write(ref) : source->get_read(ref);
	
	depMap->add_conflict_constraint( s_pat->get_pattern(), d_pat->get_pattern() );
	// depMap->print(2);
	
	/* Step 5 : Get the solution if needed */
	if ( depMap->is_empty() ) {
		delete(depMap);
		return;
	} else {
		// cout<<"DEP FOUND !!"<<endl;
		depMap->lexmax();
		if ( !liveness ) {
			vector<pollyMap*> counter_parts = depMap->get_disjoints();
			for ( int i = 0 ; i < counter_parts.size() ; i++ ) {
				// cout<<"source("<<source->get_ID()<<") and ref("<<ref<<")"<<endl;
				Dest->add_dep(new affineDependence(source,Dest,ref,destRef,type,counter_parts[i]));
				delete(counter_parts[i]);
			}
		}
		
		if ( liveness || type != R_W )
			Domain->subtract(depMap->get_map_domain());
		
		delete(depMap);
	}
}
































