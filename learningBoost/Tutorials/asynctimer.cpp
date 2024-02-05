#include <iostream>
#include <boost/asio.hpp>

void print(const boost::system::error_code & /*e*/){

  std::cout<<"Hello World\n";
}

int main(){
  
  boost::asio::io_service io;
  boost::asio::deadline_timer t(io , boost::posix_time::seconds(5));
  
  // now we are going to use async wait , which will not block or make blocking call 
  // async wait returns immediately and does it work
  t.async_wait(&print);

  for(int i=0; i < 10;i++){
    std::cout<<"loop running\n";
  }

  // .run() is a blocking function which stops the execution of main code or block it and let the async wait finish ist work if not
  io.run();
  return 0;
}
