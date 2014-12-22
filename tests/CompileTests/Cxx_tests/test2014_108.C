
// DQ (8/19/2014): This file will not compile as is with g++, so it is a meaningly test of ROSE.

#if 0
// Commented out this file (not a valid C++ file).

#include <vector>

#include "boost/foreach.hpp"
#include "boost/cstdint.hpp"

using std::vector;

namespace {

class Block 
   {
     public:
       // DQ (7/14/2014): It appears that this function is required to be defined.
       // uint64_t getSize() const;
          uint64_t getSize() const { return 0; }
   };

#if 0
uint64_t getNumberOfZones(const vector<Block>& blocks) 
   {
     uint64_t numZones = 0;

     BOOST_FOREACH(const Block& block, blocks) 
        {
          numZones += block.getSize();
        }

     return numZones;
   }
#endif


uint64_t expanded_getNumberOfZones(const vector<Block>& blocks)
   {
     uint64_t numZones = 0;

     if (boost::foreach_detail_::auto_any_t _foreach_col23 = boost::foreach_detail_::contain( (blocks) , (true ? 0 : boost::foreach_detail_::or_( boost::foreach_detail_::and_( boost::foreach_detail_::not_(boost::foreach_detail_::is_array_(blocks)) , (true ? 0 : boost::foreach_detail_::is_rvalue_( (true ? boost::foreach_detail_::make_probe(blocks) : (blocks)), 0))) , boost::foreach_detail_::and_( boost::foreach_detail_::not_(boost_foreach_is_noncopyable( boost::foreach_detail_::to_ptr(blocks) , boost_foreach_argument_dependent_lookup_hack_value)) , boost_foreach_is_lightweight_proxy( boost::foreach_detail_::to_ptr(blocks) , boost_foreach_argument_dependent_lookup_hack_value)))))) 
{} 
else 
   if (boost::foreach_detail_::auto_any_t _foreach_cur23 = boost::foreach_detail_::begin( _foreach_col23 , (true ? 0 : boost::foreach_detail_::encode_type(blocks, boost::foreach_detail_::is_const_(blocks))) , (true ? 0 : boost::foreach_detail_::or_( boost::foreach_detail_::and_( boost::foreach_detail_::not_(boost::foreach_detail_::is_array_(blocks)) , (true ? 0 : boost::foreach_detail_::is_rvalue_( (true ? boost::foreach_detail_::make_probe(blocks) : (blocks)), 0))) , boost::foreach_detail_::and_( boost::foreach_detail_::not_(boost_foreach_is_noncopyable( boost::foreach_detail_::to_ptr(blocks) , boost_foreach_argument_dependent_lookup_hack_value)) , boost_foreach_is_lightweight_proxy( boost::foreach_detail_::to_ptr(blocks) , boost_foreach_argument_dependent_lookup_hack_value)))))) 
{} 
else 
   if (boost::foreach_detail_::auto_any_t _foreach_end23 = boost::foreach_detail_::end( _foreach_col23 , (true ? 0 : boost::foreach_detail_::encode_type(blocks, boost::foreach_detail_::is_const_(blocks))) , (true ? 0 : boost::foreach_detail_::or_( boost::foreach_detail_::and_( boost::foreach_detail_::not_(boost::foreach_detail_::is_array_(blocks)) , (true ? 0 : boost::foreach_detail_::is_rvalue_( (true ? boost::foreach_detail_::make_probe(blocks) : (blocks)), 0))) , boost::foreach_detail_::and_( boost::foreach_detail_::not_(boost_foreach_is_noncopyable( boost::foreach_detail_::to_ptr(blocks) , boost_foreach_argument_dependent_lookup_hack_value)) , boost_foreach_is_lightweight_proxy( boost::foreach_detail_::to_ptr(blocks) , boost_foreach_argument_dependent_lookup_hack_value)))))) 
{} 
else 
   for (bool _foreach_continue23 = true; _foreach_continue23 && !boost::foreach_detail_::done( _foreach_cur23 , _foreach_end23 , (true ? 0 : boost::foreach_detail_::encode_type(blocks, boost::foreach_detail_::is_const_(blocks)))); _foreach_continue23 ? boost::foreach_detail_::next( _foreach_cur23 , (true ? 0 : boost::foreach_detail_::encode_type(blocks, boost::foreach_detail_::is_const_(blocks)))) : (void)0) 
        if (boost::foreach_detail_::set_false(_foreach_continue23)) 
          {} 
         else 
            for (const Block& block = boost::foreach_detail_::deref( _foreach_cur23 , (true ? 0 : boost::foreach_detail_::encode_type(blocks, boost::foreach_detail_::is_const_(blocks)))); !_foreach_continue23; _foreach_continue23 = true)
        {
          numZones += block.getSize();
        }

     return numZones;
   }

#if 1
// The function will be unparsed as (with errors):
uint64_t unparsed_expanded_getNumberOfZones(const std::vector< Block  , class std::allocator< Block  >  > &blocks)
{
uint64_t numZones = 0;
if (boost::foreach_detail_::auto_any_t _foreach_col23 = (boost::foreach_detail_::contain(blocks,(true?0 : boost::foreach_detail_::or_(boost::foreach_detail_::and_(boost::foreach_detail_::not_(boost::foreach_detail_::is_array_(blocks)),(true?0 : boost::foreach_detail_::is_rvalue_((true?(boost::foreach_detail_::make_probe(blocks) . operator std::vector< <unnamed>::Block, std::allocator< <unnamed>::Block> > &()) : blocks),0))),boost::foreach_detail_::and_(boost::foreach_detail_::not_(::boost_foreach_is_noncopyable(boost::foreach_detail_::to_ptr(blocks),boost_foreach_argument_dependent_lookup_hack_value)),::boost_foreach_is_lightweight_proxy(boost::foreach_detail_::to_ptr(blocks),boost_foreach_argument_dependent_lookup_hack_value))))))) {
}
else {
if (boost::foreach_detail_::auto_any_t _foreach_cur23 = (boost::foreach_detail_::begin(_foreach_col23,(true?0 : boost::foreach_detail_::encode_type(blocks,boost::foreach_detail_::is_const_(blocks))),(true?0 : boost::foreach_detail_::or_(boost::foreach_detail_::and_(boost::foreach_detail_::not_(boost::foreach_detail_::is_array_(blocks)),(true?0 : boost::foreach_detail_::is_rvalue_((true?(boost::foreach_detail_::make_probe(blocks) . operator std::vector< <unnamed>::Block, std::allocator< <unnamed>::Block> > &()) : blocks),0))),boost::foreach_detail_::and_(boost::foreach_detail_::not_(::boost_foreach_is_noncopyable(boost::foreach_detail_::to_ptr(blocks),boost_foreach_argument_dependent_lookup_hack_value)),::boost_foreach_is_lightweight_proxy(boost::foreach_detail_::to_ptr(blocks),boost_foreach_argument_dependent_lookup_hack_value))))))) {
}
else {
if (boost::foreach_detail_::auto_any_t _foreach_end23 = (boost::foreach_detail_::end(_foreach_col23,(true?0 : boost::foreach_detail_::encode_type(blocks,boost::foreach_detail_::is_const_(blocks))),(true?0 : boost::foreach_detail_::or_(boost::foreach_detail_::and_(boost::foreach_detail_::not_(boost::foreach_detail_::is_array_(blocks)),(true?0 : boost::foreach_detail_::is_rvalue_((true?(boost::foreach_detail_::make_probe(blocks) . operator std::vector< <unnamed>::Block, std::allocator< <unnamed>::Block> > &()) : blocks),0))),boost::foreach_detail_::and_(boost::foreach_detail_::not_(::boost_foreach_is_noncopyable(boost::foreach_detail_::to_ptr(blocks),boost_foreach_argument_dependent_lookup_hack_value)),::boost_foreach_is_lightweight_proxy(boost::foreach_detail_::to_ptr(blocks),boost_foreach_argument_dependent_lookup_hack_value))))))) {
}
else {
for (bool _foreach_continue23 = true; _foreach_continue23 && !boost::foreach_detail_::done(_foreach_cur23,_foreach_end23,(true?0 : boost::foreach_detail_::encode_type(blocks,boost::foreach_detail_::is_const_(blocks)))); _foreach_continue23?boost::foreach_detail_::next(_foreach_cur23,(true?0 : boost::foreach_detail_::encode_type(blocks,boost::foreach_detail_::is_const_(blocks)))) : ((void )0)) 
if (boost::foreach_detail_::set_false(_foreach_continue23)) 
{}
else {
  for (const class Block &block = boost::foreach_detail_::deref(_foreach_cur23,(true?0 : boost::foreach_detail_::encode_type(blocks,boost::foreach_detail_::is_const_(blocks)))); !_foreach_continue23; _foreach_continue23 = true) {
    numZones += block .  getSize ();
  }
}
}
}
}
return numZones;
}
#endif

} // namespace

#endif
