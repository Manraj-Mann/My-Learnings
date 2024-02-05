// this code will call the print function 5 times at stop at 6th time


#include <iostream>
#include <boost/asio.hpp>
// using .bind to pass the parameters to tyhe handler function
#include <boost/bind.hpp>

void print(const boost::system::error_code & /*e*/ , boost::asio::deadline_timer *t , int * count){


    // if count reaches 5 stop incrementing
    if(*count < 5){
      std::cout<<"Call Count : "<<(*count)<<std::endl;
      ++(*count);
      // if the count is less than 6 then we can increase the timer expiry by one second 
      t->expires_at(t->expires_at() + boost::posix_time::seconds(1));
      // start a new async wait on timer 
      t->async_wait(boost::bind(print ,boost::asio::placeholders::error, t , count));
    }

}

int main(){

  boost::asio::io_service io;
  int count=0;
  boost::asio::deadline_timer t(io , boost::posix_time::seconds(5));

  // now we are going to use async wait , which will not block or make blocking call
  //   // async wait returns immediately and does it work
  t.async_wait(boost::bind( print , boost::asio::placeholders::error , &t , &count));
  
  // .run() is a blocking function which stops the execution of main code or block it and let the async wait finish ist work if not
  io.run();

  std::cout<<"Final Count is : "<<count<<std::endl;

  return 0;
}

