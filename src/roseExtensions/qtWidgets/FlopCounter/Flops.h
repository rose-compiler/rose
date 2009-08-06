
#ifndef FLOPS_H
#define FLOPS_H



#include <string>
#include <map>

// include for VariantT
#include "sage3.h"

/**
 * \brief Helper Class for FlopCounter
 *  Accumulates occurance of VariantT given in constructor
 */
class Flops : public AstAttribute
{
   public:
      Flops();
      Flops( const Flops& f );
      Flops( VariantT opName, size_t count );
      Flops& operator=( const Flops& f );
      Flops& operator+=( const Flops& f );

      void add( VariantT opType, size_t count );
      void set_inherited( VariantT opName );

      size_t operator()( VariantT opName = V_SgNode ) const;

      void print( std::ostream& out ) const;

      // AstAttribute specific:
      std::string toString();

   private:
      typedef std::map< VariantT, size_t > BinaryOpFlops;

      void swap( Flops& f ) throw();

      BinaryOpFlops count_;
      size_t inherited_;
};

const Flops   operator+ ( const Flops& lhs, const Flops& rhs );
std::ostream& operator<<( std::ostream& out, const Flops& f );

#endif
