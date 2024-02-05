#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
using boost::asio::ip::tcp;

std::string make_daytime_string(){

    std::time_t now = std::time(0);
    return std::ctime(&now);
}

int main(){

    try
    {
        boost::asio::io_service io_service;
        // create an acceptor to listen connections
        tcp::acceptor acceptor(io_service , tcp::endpoint(tcp::v4() ,50000));
        // this is an iterative server which handles one connection at a time

        std::cout<<"Server Started ...\n";
         std::cout << "Server is listening on: " << acceptor.local_endpoint() << std::endl;
        for(; ;){
        
            tcp::socket socket(io_service);
            acceptor.accept(socket);

            // providing service
            std::string message = make_daytime_string();
            boost::system::error_code ignored_error;
            boost::asio::write(socket , boost::asio::buffer(message) , ignored_error);

            std::cout<<"FulFilled Req of : "<<socket.remote_endpoint()<<std::endl;
        
        }
        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    

    return 0;
}