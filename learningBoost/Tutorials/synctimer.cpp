#include <iostream>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

int main(){

  // create io context - provide io functionality
  boost::asio::io_service io;
  // // create a deadline timer - core asio provides io functionality which takes input the two args 
  // 1. io_context 2. sets timer to expire
  
  // deadline timer has two states : 1. expired 2. not expired 
  boost::asio::deadline_timer t ( 
      io , 
      boost::posix_time::seconds(5) );

  // Example performs blocking wait on time
  t.wait();
  std::cout<<"Timer Finished !"<<std::endl;
  return 0;

}
