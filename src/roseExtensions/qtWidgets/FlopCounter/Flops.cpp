
#include <sstream>

#include "Flops.h"
    
    Flops::Flops()
: count_()
{
}

   Flops::Flops( const Flops& f )
: count_( f.count_ )
{
}

   Flops::Flops( VariantT opName, size_t count )
: count_( )
{
   count_.insert( std::pair<VariantT, size_t>( opName, count ) );
}

Flops& Flops::operator=( const Flops& f )
{
   Flops tmp( f );
   swap( tmp );

   return *this;
}

Flops& Flops::operator+=( const Flops& f )
{
   for( BinaryOpFlops::const_iterator i( f.count_.begin() );
         i != f.count_.end();
         ++i )
   {
      BinaryOpFlops::iterator pos( count_.find( i->first ) );

      if( pos == count_.end() )
      {
         count_.insert( *i );
         continue;
      }
      size_t newCount = pos->second + i->second;
      count_.erase( pos );
      count_.insert( std::pair<VariantT, size_t>( i->first, newCount ) );
   }

   return *this;
}

void Flops::add( VariantT opName, size_t count )
{
   BinaryOpFlops::iterator insertPos( count_.find( opName ) );

   if( insertPos == count_.end() )
   {
      count_.insert( std::pair<VariantT, size_t>( opName, count ) );
   }
   else
   {
      size_t newCount = insertPos->second + count;
      count_.erase( insertPos );
      count_.insert( std::pair<VariantT, size_t>( opName, newCount ) );
   }
}

size_t Flops::operator()(VariantT opName /*= V_SgNode*/ ) const
{
   if( opName == V_SgNode )
   {
      size_t sum = 0;
      for( BinaryOpFlops::const_iterator i( count_.begin() );
            i != count_.end();
            ++i )
      {
         sum += i->second;
      }

      return sum;
   }

   BinaryOpFlops::const_iterator pos( count_.find( opName ) );

   if( pos == count_.end() )
   {
      return 0;
   }
   else
   {
      return pos->second;
   }
}

void Flops::swap( Flops& f ) throw()
{
   std::swap( count_, f.count_ );
}

void Flops::print( std::ostream& out ) const
{
   for( BinaryOpFlops::const_iterator i( count_.begin() );
         i != count_.end();
         ++i )
   {
      out << i->first << ": " << i->second;
      if( i != count_.end()-- )
          out << "\n";
   }
}

std::string Flops::toString()
{
    std::ostringstream out_sstr;
    print( out_sstr );

    return out_sstr.str();
}

const Flops operator+( const Flops& lhs, const Flops& rhs )
{
   Flops tmp( lhs );

   return tmp += rhs;
}

std::ostream& operator<<( std::ostream& out, const Flops& f )
{
   f.print( out );
   return out;
}
