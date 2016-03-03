//! read gprof line-by-line performance profiling result text files

/*
 * This software was produced with support in part from the Defense Advanced
 * Research Projects Agency (DARPA) through AFRL Contract FA8650-09-C-1915.
 * Nothing in this work should be construed as reflecting the official policy
 * or position of the Defense Department, the United States government,
 * or Rice University.
 */

#include "sage3basic.h"
#include <sstream>

// include the right header for spirit classic
// need to check for the version, because there was a directory restructuring
// and a change of namespace
// introduced with boost 1.38
#include <boost/version.hpp>
#if BOOST_VERSION >= 103600
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
    //TODO: can have the same name for several functions 
    // Should look further for file and line information
    //static std::map<std::string, std::set<IRTree_t*> > func_node_map;
    static std::map<std::string, IRTree_t*> func_node_map;

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
      // file level---------------
      // We don't attach any metrics here and let propagation phase to do it
      // create file node if it does not exists 
      string file_name = info.file_name();
      IRTree_t* file_node = file_node_map[file_name];
      if (file_node == NULL)
      {
        File * f= new File(file_name);
        ROSE_ASSERT(f!=NULL);
        // insert it under prog_root
        size_t num_kids = prog_root->getNumChildren();
		num_kids++;
        file_node = prog_root->setChildValue(num_kids, f);
        ROSE_ASSERT(file_node != NULL);
        file_node_map[file_name] = file_node;
      }
      // function level---------------
      // We don't attach any metrics here and let propagation phase to do it
      //  Deal with functions with same names
      string func_name = info.function_name();
      IRTree_t* func_node = NULL;
      func_node =  func_node_map[func_name];
#if 0  // gprof does not provide function's line number, info.line_number() is for the statement     
      std::set<IRTree_t*> nodeset = func_node_map[func_name];
      for( std::set<IRTree_t*>::const_iterator iter = nodeset.begin();
          iter != nodeset.end(); iter ++)
      {
        File * existing_file = dynamic_cast<File*>((*iter)->value); 
        // match file node and line number
        if ((existing_file->getFileNode() ==file_node)&&
            (existing_file->getFirstLine() == info.line_number()))
        {
          func_node = exsting_file;
          break;
        }
      }
#endif      
      if (func_node == NULL)
      { // assume the end line is equal to the beginning line
        Procedure * p= new Procedure(info.line_number(), func_name, info.line_number());
        ROSE_ASSERT(p!=NULL);
        p->setFileNode(dynamic_cast<File*>(file_node->value));
        //insert it under a corresponding file node
        size_t num_kids= file_node->getNumChildren();
		num_kids++;
        func_node = file_node->setChildValue(num_kids,p);
        func_node_map[func_name]=func_node;
        //func_node_map[func_name].insert(func_node);
      } 
      // statement level-------------------
      // Where the line-by-line metrics should be attached.
      if (info.calls()>0)  
        // this line is about an entire function , we ignore them for now
        // since a later metric propagation phase will generate function metrics from statement level ones.
      {

      } 
      else  // this line is about a single statement only, 
        // no chance to have multiple lines of information for a single statement
      {
        // Build the Profile IR node
        // Statement* s = new Statement ("",info.line_number(),0);
        Statement* s = new Statement ("", info.line_number(), info.line_number()); // DXN
        s->setFileNode(dynamic_cast<File*>(file_node->value));
        // insert into a procedure node
        size_t num_kids = func_node->getNumChildren();
		num_kids++;
        func_node->setChildValue(num_kids,s); 

        //Attach metrics
        // Metric 1: We treat the self-seconds as wall clock time, 
        // even the original WALLCLK of hpctoolkit is an integer cycle count
        Metric *m = new Metric(m_wallclock,info.self_seconds());
        ROSE_ASSERT(m!=NULL);
        s->addMetric(*m);
        // Metric 2:  For line-by-line gprof results, the percentage is exclusive , not inclusive
        m = new Metric(m_percentage, info.time_percent()/100.0); // must divided by 100 here!!
        ROSE_ASSERT(m!=NULL);
        s->addMetric(*m);
      } 
    }
    treelist.push_back(prog_root);
    return treelist;
  }


}// end of namespace 
