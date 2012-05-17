#include <rose.h>

using namespace std;

#include <rosepoly/data.h>
#include <rosepoly/Nodes.h>
#include <rosepoly/access_pattern.h>
#include <rosepoly/error.h>
#include <rosepoly/RosePollyCustom.h>


AccessPattern::AccessPattern( const AccessPattern& ap )
: refExp(ap.get_refExp()), parent(ap.get_parent()), myIO(ap.get_IO()), ID(ap.get_id()),
pattern(NULL)
{	
	if ( ap.pattern )
		pattern = new pollyMap(*ap.pattern);
}

AccessPattern::AccessPattern( SgExpression * exp, IO i ) :
refExp(exp), parent(NULL), myIO(i), pattern(NULL), ID(-1)
{}

AccessPattern::~AccessPattern() { delete(pattern); }

string AccessPattern::get_name() const
{
	if ( parent!=NULL )
		return parent->get_name(); 
	else
		return "(no name)";
}

void AccessPattern::set_parent( Datum * p ) { parent = p; }

void AccessPattern::set_polly_map( pollyDomain * d, RosePollyCustom * c )
{
	pattern = c->add_pattern(d,this);
}

void AccessPattern::set_id( int i ) { ID = i; }

int AccessPattern::get_id() const { return ID; }

Datum * AccessPattern::get_parent() const { return parent; }

pollyMap * AccessPattern::get_pattern() const { return pattern; }

void AccessPattern::add_subscript( SgExpression * exp ) { Subs.insert(Subs.begin(),exp); }

IO AccessPattern::get_IO() const { return myIO; }

int AccessPattern::get_dim() const { return Subs.size(); }

bool AccessPattern::has_valid_map() const { return pattern->is_valid(); }

SgExpression * AccessPattern::get_subscript( int pos ) const 
{ 
	ROSE_ASSERT(pos<Subs.size());
	return Subs[pos]; 
}

SgExpression * AccessPattern::get_refExp() const { return refExp; }

void AccessPattern::print( int ident ) const
{
	for ( int i = 0 ; i < ident ; i++ )
		cout<<" ";
	
	cout<<get_name();
	for ( int i = 0 ; i < Subs.size() ; i++ )
		cout<<"["<<Subs[i]->unparseToString()<<"]";
	cout<<endl;
	if ( pattern ) {
		pattern->print(ident);
	} else {
		for ( int i = 0 ; i < ident ; i++ )
			cout<<" ";
		cout<<"no pattern."<<endl;
	}
}

void search_for_access_patterns( SgExpression * exp, vector<AccessPattern*>& pList, IO anIO )
{
	
	if ( !isRegularExp(exp->get_type()) ) {
		report_error( "Type of expression not supported", exp );
		
	} else if ( isSgPntrArrRefExp(exp) != NULL ) {
		
		AccessPattern * pat = extract_pattern( isSgPntrArrRefExp(exp), anIO );
		if ( pat == NULL )
			report_error("Unrecognized expression", exp);
		else 
			pList.push_back(pat);
			
	} else if ( isSgVarRefExp(exp) != NULL ) {
		
		pList.push_back( new AccessPattern( exp, anIO ) );
		
	} else if ( isSgBinaryOp(exp) != NULL ) {
		
		search_for_access_patterns( isSgBinaryOp(exp)->get_lhs_operand(), pList, anIO );
		search_for_access_patterns( isSgBinaryOp(exp)->get_rhs_operand(), pList, anIO );
		
	} else if ( isSgUnaryOp(exp) != NULL ) {
		
		search_for_access_patterns( isSgUnaryOp(exp)->get_operand(), pList, anIO );
		
	} else if ( isSgValueExp(exp) != NULL ) {
		return;
	} else {
		report_error( "Unrecognized expression", exp );
	}
}

AccessPattern * extract_pattern( SgPntrArrRefExp * arrRef, IO anIO )
{
	AccessPattern * pat = new AccessPattern( arrRef, anIO );
	pat->add_subscript( arrRef->get_rhs_operand() );
	
	bool result = extract_pattern_rec( pat, arrRef->get_lhs_operand() );
	
	return !result ? NULL : pat;
}

bool extract_pattern_rec( AccessPattern * p, SgExpression * exp )
{
	if ( isSgPntrArrRefExp(exp) ) {
		p->add_subscript( isSgPntrArrRefExp(exp)->get_rhs_operand() );
		return extract_pattern_rec( p, isSgPntrArrRefExp(exp)->get_lhs_operand() );
	} else {
		return isSgVarRefExp(exp) != NULL;
	}
}

bool isRegularExp( SgNode * n )
{
	if ( isSgPointerType(n) )
		return false;
	else if ( isSgReferenceType(n) )
		return isRegularExp(isSgReferenceType(n)->get_base_type());
	else if ( isSgTypedefType(n) )
		return isRegularExp( isSgTypedefType(n)->get_base_type() );
	else
		return true;
}




