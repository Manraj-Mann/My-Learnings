#include <iostream> 
#include <boost/asio.hpp>
#include <boost/bind.hpp>
class printer{

  private:
    boost::asio::deadline_timer timer_;
    int count_;

  public : 

    printer(boost::asio::io_service &io): timer_(io , boost::posix_time::seconds(4)) , count_(0)
    {
      timer_.async_wait(boost::bind(&printer::print , this ));
    }

    void print( ){
      
      if(count_ < 5){
        
        std::cout<<"Count : " <<count_<<std::endl;
        ++count_;

        timer_.expires_at( timer_.expires_at() + boost::posix_time::seconds(1));
        timer_.async_wait( boost::bind(&printer::print , this));
      }

    }

    ~printer(){
      
      std::cout<<"Final Count : "<<count_<<std::endl;

    }

};

int main(){

  boost::asio::io_service io;
  printer p(io);
  io.run();
  return 0;
}


