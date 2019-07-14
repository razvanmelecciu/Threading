#ifndef _logging_hpp_
#define _logging_hpp_

#ifndef _DEBUG_
   #include <boost/log/trivial.hpp>
   #include <boost/format.hpp>
   #define LOG(x) BOOST_LOG_TRIVIAL(trace) << boost::format(x)
#else
   #define LOG(x)
#endif // _DEBUG_

#endif // _logging_hpp_
