
#ifndef GPROF_INFO_H
#define GPROF_INFO_H

#include <iostream>
#include <fstream>
#include "rosehpct/rosehpct.hh"

namespace RoseHPCT
{
  // forward declaration of gprof_info (needed for operator<<)
  class gprof_info;

  /*!
   * \brief reading gprof information into generic Container
   *
   * This function reads a file line by line and populates the
   * Container c with the gprof information found.
   * Consider resizing the Container to an appropriate size, for performance reasons.
   *
   * \param gprof_file Path to gprof output file
   * \param c          Container to hold the data 
   *                   (must provide a push_back( const gprof_info& info ) member function )
   */
  template< typename Container >
    void read_gprof_info( const char* gprof_file, Container& c );

  /*!
   * \brief operator<< for gprof_info
   *
   * outputs content of gprof_info to an arbitrary std::ostream
   *
   * \param out stream to write to
   * \param info gprof_info to output
   */
  std::ostream& operator<<( std::ostream& out, const gprof_info& info );

  /*!
   * \brief gprof_info
   *
   * Holds gprof Profiling information
   */
  class gprof_info
  {
    public:
      /*!
       * \brief Default constructor
       *
       * Sets every member to 0, and the strings to an empty string
       */
      gprof_info( )
        : time_percent_( 0.0 ),
        cumulative_seconds_( 0.0 ),
        self_seconds_( 0.0 ),
        calls_( 0 ),
        self_ns_calls_( 0.0 ),
        total_ns_calls_( 0.0 ),
        function_name_( "" ),
        file_name_( "" ),
        line_number_( 0 )
    {}

      /*!
       * \brief Copy constructor
       */
      gprof_info( const gprof_info& info )
        : time_percent_( info.time_percent_ ),
        cumulative_seconds_( info.cumulative_seconds_ ),
        self_seconds_( info.self_seconds_ ),
        calls_( info.calls_ ),
        self_ns_calls_( info.self_ns_calls_ ),
        total_ns_calls_( info.total_ns_calls_ ),
        function_name_( info.function_name_ ),
        file_name_( info.file_name_ ),
        line_number_( info.line_number_ )
    {}
      /*!
       * \brief Stringify a class object
       */
      std::string toString() const;

      /*!
       * \brief Parse constructor
       *
       * Parses information given in the input string.
       * The string hase the following format:
       *  time_precent cumulative_seconds self_seconds calls  self_ns_calls total_ns_call  name
       *  double       double             double       size_t double        double         
       *
       *  name is a compound of the following:
       *  function_name (file_name:line_number @ call_address )
       *  string         string    size_t
       *
       *  Thie fields have the name of the corresponding member variables.
       *  Throws an exception if the parsing fails.
       */
      gprof_info( const std::string& input );
      /*!
       * \brief Destructor
       *
       * noop
       */
      ~gprof_info() {}

      /*!
       * \brief Assignment operator
       */
      gprof_info& operator=( const gprof_info& info )
      {
        if (this == &info)
          return *this;
        time_percent_= info.time_percent_;
        cumulative_seconds_ = info.cumulative_seconds_;
        self_seconds_ = info.self_seconds_;
        calls_ = info.calls_;
        self_ns_calls_ = info.self_ns_calls_;
        total_ns_calls_ = info.total_ns_calls_;
        function_name_ = info.function_name_;
        file_name_ = info.file_name_;
        line_number_ = info.line_number_;
        return *this;
      }

      /*!
       * \defgroup Member access functions
       * @{
       */

      /*!
       * \see time_percent_
       */
      const double&       time_percent() const
      {
        return time_percent_;
      }
      /*!
       * \see cumulative_seconds_
       */
      const double&       cumulative_seconds() const
      {
        return cumulative_seconds_;
      }
      /*!
       * \see self_calls_
       */
      const double&       self_seconds() const
      {
        return self_seconds_;
      }
      /*!
       * \see calls_
       */
      const size_t&         calls() const
      {
        return calls_;
      }
      /*!
       * \see self_ns_calls_
       */
      const double&       self_ns_calls() const
      {
        return self_ns_calls_;
      }
      /*!
       * \see total_ns_calls_
       */
      const double&       total_ns_calls() const
      {
        return total_ns_calls_;
      }
      /*!
       * \see function_name_
       */
      const std::string& function_name() const
      {
        return function_name_;
      }
      /*!
       * \see file_name_
       */
      const std::string& file_name() const
      {
        return file_name_;
      }
      /*!
       * \see line_number_
       */
      const size_t&         line_number() const
      {
        return line_number_;
      }
      /*!
       * @}
       */

    private:
      double      time_percent_; /*! < TODO: document the meaning of this number */
      double      cumulative_seconds_; /*! < TODO: document the meaning of this number */
      double      self_seconds_; /*! < TODO: document the meaning of this number */
      size_t      calls_; /*! < TODO: document the meaning of this number */
      double      self_ns_calls_; /*! < TODO: document the meaning of this number */
      double      total_ns_calls_; /*! < TODO: document the meaning of this number */
      std::string function_name_; /*! < The function name */
      std::string file_name_; /*! < The file name in which the function is defined */
      size_t      line_number_; /*! < The line number the definition occurs in the file */
  };

  template< typename Container >
    void read_gprof_info( const char* gprof_file, Container& c )
    {
      // open file
      std::ifstream file( gprof_file );
      std::string buffer;

      // read file by line
      while( std::getline( file, buffer ) )
      {
        try {
          // create temporary object
          gprof_info tmp( buffer );
          // push it back into the container
          c.push_back( tmp );
        }
        catch (...) {}
      }
    }
   ProgramTreeList_t loadGprofVec(std::vector< gprof_info > vec);

} //end of namespace
#endif
