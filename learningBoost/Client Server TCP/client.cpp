#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/thread/thread.hpp>
int main(){

    int argc = 2;
    char *argv[] = {"client", "127.0.0.1"  , "50000"};

    // specify the server as command line arg
    try {

        if(argc != 2){

            std::cerr<<"Usage : client <host>"<<std::endl;
            return 1;
        }

        // creating the io service
        boost::asio::io_service io_service;

        // use resolver to get the end points for the specified address 
        boost::asio::ip::tcp::resolver resolver(io_service);

        // resolver takes the query object and turns it into list of endpoints
        // in out case the service is "daytime"

        boost::asio::ip::tcp::resolver::query query(argv[1] , argv[2]);

        // The list of end points can be iterated using an iterator

        boost::asio::ip::tcp::resolver::iterator end_points_iterator = resolver.resolve(query);

        // now we connect to the endpoints by creating a socket but it may be ipv4 or ipv6 so we try both
        boost::asio::ip::tcp::socket socket(io_service);
        boost::asio::connect(socket , end_points_iterator);

        // once the connection is open , all we need is to read the data from server

        // to hold data we use boost:array or std::vector and use boost::buffer() automatically determines the size of array and prevent buffer to overrun

        for(; ; ){

            boost::array<char , 128> buf;
            boost::system::error_code error;
            
            size_t len = socket.read_some(boost::asio::buffer(buf) , error);

            // if server server closes the connection safely it gives eof error
            if( error == boost::asio::error::eof){

                // clean connection closure
                break;
            }
            else if (error){

                throw boost::system::system_error(error);
            }

            std::cout.write(buf.data() , len);
        }



    }
    catch(std::exception &e){
        std::cerr<<e.what()<<std::endl;
    }
    return 0;
}