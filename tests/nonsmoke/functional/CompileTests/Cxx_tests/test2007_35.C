/*
The preprocessed version of this code produces many other ROSE errors, but the following (run using Boost 1.33.1):

#include <boost/smart_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

class t: public boost::enable_shared_from_this<t> {};

produces an assertion failure about a NULL file info.  Also, when it is able to be unparsed, the boost:: qualification is missing on enable_shared_from_this, which causes the generated code to be incorrect.

-- Jeremiah Willcock 
*/

#include <boost/smart_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

class t: public boost::enable_shared_from_this<t> {};
