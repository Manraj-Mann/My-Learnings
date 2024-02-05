#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <string>
#include <ctime>

using boost::asio::ip::udp;

std::string make_daytime_string()
{
    using namespace std; // For time_t, time and ctime;
    time_t now = time(0);
    return ctime(&now);
}

int main()
{

    try
    {
        boost::asio::io_service io_service;
        udp::socket socket(io_service, udp::endpoint(udp::v4(), 50000));

        // Wait for a client to initiate contact with us.
        // The remote_endpoint object will be populated by ip::udp::socket::receive_from().

        for (;;)
        {
            boost::array<char, 1> recv_buf;
            udp::endpoint remote_endpoint;
            boost::system::error_code error;
            
            std::cout<<"Listening at : "<<socket.local_endpoint()<<std::endl;
            socket.receive_from(

                boost::asio::buffer(recv_buf),
                remote_endpoint,
                0,
                error
            );

            if (error && error != boost::asio::error::message_size)
            {

                throw boost::system::system_error(error);
            }

            // Determine what we are going to send back to the client.

            std::string message = make_daytime_string();

            // Send the response to the remote_endpoint.

            boost::system::error_code ignored_error;

            socket.send_to(
                boost::asio::buffer(message),
                remote_endpoint, 
                0, 
                ignored_error
            );
        }
    }

    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}