//! read gprof line-by-line performance profiling result text files
#include "rose.h"
#include <sstream>

// include the right header for spirit classic
// need to check for the version, because there was a directory restructuring
// and a change of namespace
// introduced with boost 1.38
#include <boost/version.hpp>
#if BOOST_VERSION >= 103800
 #include <boost/spirit/include/classic_core.hpp>
 namespace boost_spirit = BOOST_SPIRIT_CLASSIC_NS;
#else
 #include <boost/spirit.hpp>
 namespace boost_spirit = boost::spirit;
#endif

#include "rosehpct/gprof/gprof_info.hh"

using namespace boost_spirit;
using namespace std;

#include <vector>
namespace RoseHPCT
{
  gprof_info::gprof_info( const std::string& input )
    // set variables to default values
    : time_percent_( 0.0 ),
    cumulative_seconds_( 0.0 ),
    self_seconds_( 0.0 ),
    calls_( 0 ),
    self_ns_calls_( 0.0 ),
    total_ns_calls_( 0.0 ),
    function_name_( "" ),
    file_name_( "" ),
    line_number_( 0 )
  {
    // This function parses the string with the help of boost::spirit
    // see http://spirit.sourceforge.net/distrib/spirit_1_8_5/libs/spirit/index.html for more information
    // deklaration of the different rules
    rule<> grammar,
    time_percent_p,
    cumulative_seconds_p,
    self_seconds_p,
    calls_p,
    self_ns_calls_p,
    total_ns_calls_p,
    function_name_p,
    file_name_p,
    line_number_p;

    // Setting up the grammar
    // reminder of how the line looks:
    // time_precent cumulative_seconds self_seconds calls  self_ns_calls total_ns_call  name
    // double       double             double       size_t double        double
    //
    // name is a compound of the following:
    // function_name (file_name:line_number @ call_address )
    // string         string    size_t
    //
    // in between the data fields at least one space can be found
    grammar = +ch_p( ' ' ) >> time_percent_p >>
      +ch_p( ' ' ) >> cumulative_seconds_p >>
      +ch_p( ' ' ) >> self_seconds_p >>
      !( +ch_p( ' ' ) >> calls_p ) >>               // calls are optional
      !( +ch_p( ' ' ) >> self_ns_calls_p ) >>       // self_ns_calls are optional
      !( +ch_p( ' ' ) >> total_ns_calls_p ) >>      // total_ns_calls are optional
      +ch_p( ' ' ) >> function_name_p >>
      ch_p( ' ' ) >> ch_p( '(' ) >> file_name_p >>
      ch_p( ':' ) >> line_number_p >> *anychar_p;  // *anychar_p reads in the remaining part of the string,
    // but dismisses it
    // definition of the different parts we want to safe.
    //                                 format of the input                       assignment action, if found
    time_percent_p       =             real_p                                  [ assign_a( time_percent_ ) ];
    cumulative_seconds_p =             real_p                                  [ assign_a( cumulative_seconds_ ) ];
    self_seconds_p       =             real_p                                  [ assign_a( self_seconds_ ) ];
    calls_p              =             int_p                                   [ assign_a( calls_ ) ];
    self_ns_calls_p      =             real_p                                  [ assign_a( self_ns_calls_ ) ];
    total_ns_calls_p     =             real_p                                  [ assign_a( total_ns_calls_ ) ];
    function_name_p      = lexeme_d[ ( *( print_p - blank_p ) )                [ assign_a( function_name_ ) ] ];
    file_name_p          = lexeme_d[ ( *( print_p - blank_p - ch_p( ':' )  ) ) [ assign_a( file_name_ )] ];
    line_number_p        =             int_p                                   [ assign_a( line_number_ ) ];

    // actual parsing
    bool success = parse( input.c_str(), grammar ).full;

    if( !success ) throw "gprofInfo: Parsing not successful";
  }

  std::string gprof_info::toString() const
  {
    std::ostringstream os;
    os << time_percent() << ""
      << cumulative_seconds() << " "
      << self_seconds() << " "
      << calls() << " "
      << self_ns_calls() << " "
      << total_ns_calls() << " "
      << function_name() << " "
      << file_name() << " "
      << line_number()<< " "
      << std::endl;
    return os.str();
  }

  std::ostream& operator<<( std::ostream& out, const gprof_info& info )
  {
    out << "Function " << info.function_name() << " " << info.file_name() << " " << info.line_number() << std::endl;
    out << info.time_percent() << " "
      << info.cumulative_seconds() << " "
      << info.self_seconds() << " "
      << info.calls() << " "
      << info.self_ns_calls() << " "
      << info.total_ns_calls() << std::endl;
    return out;   
  }

  // 4 fixed levels of IR for gprof resutls: program, file, function, and statements
  // Convert gprof line-by-line profiling information to a tree-like Profile IR used in rosehpct
  ProgramTreeList_t loadGprofVec( const std::vector< gprof_info >& vec)
  {
    ProgramTreeList_t treelist;
    // root node, 
    static IRTree_t* prog_root=NULL;
    // a file and file node map
    static std::map<std::string , IRTree_t* > file_node_map;
    // TODO how about duplicated function name? 
    static std::map<std::string, IRTree_t* > func_node_map;

    if (vec.size()==0)
      return treelist;

    if (prog_root ==NULL)
    {
      prog_root =  new IRTree_t (new Program("PGM"));
      ROSE_ASSERT(prog_root !=NULL);
    }
    std::vector< gprof_info >::const_iterator iter=vec.begin();
    for (;iter != vec.end(); iter++)
    {
      gprof_info info = *iter;
      // create file node if not done
      string file_name = info.file_name();
      string func_name = info.function_name();

      IRTree_t* file_node = file_node_map[file_name];
      // TODO how about duplicated function name? 
      IRTree_t* func_node = func_node_map[file_name];
       // file level
      if (file_node == NULL)
      {
        File * value = new File(file_name);
        ROSE_ASSERT(value !=NULL);
        // insert it under prog_root
        size_t num_kids = prog_root->getNumChildren();
        file_node = prog_root->setChildValue(num_kids+1, value);
        ROSE_ASSERT(file_node != NULL);
        file_node_map[file_name] = file_node;
      }
      // function level
      if (func_node == NULL)
      { // assume the end line is equal to the beginning line
        Procedure * value = new Procedure(func_name,info.line_number(),info.line_number());
        ROSE_ASSERT(value!=NULL);
        //insert it under a corresponding file node
        size_t num_kids= file_node->getNumChildren();
        func_node = file_node->setChildValue(num_kids+1,value);
        dynamic_cast<Located*>(func_node->value)->setFileNode(dynamic_cast<File*>(file_node->value));
        func_node_map[func_name]= func_node;
      } 
      // statement level
      if (info.calls()>0)  
        // this line is about an entire function , we ignore them for now
        // since a later metric propagation phase will generate function metrics from statement level ones.
       {
         
       } 
      else  // this line is about a single statement only
       {
         Statement* s = new Statement ("",info.line_number(),0);
         // We treat the self-seconds as wall clock time
         Metric *m = new Metric("WALLCLK",info.self_seconds());
         ROSE_ASSERT(m!=NULL);
         s->addMetric(*m);
         // insert into a procedure node
         size_t num_kids = func_node->getNumChildren();
         //IRTree_t* stmt_node = 
         func_node->setChildValue(num_kids+1,s); 
       } 
    }
    treelist.push_back(prog_root);
    return treelist;
  }


}// end of namespace 
