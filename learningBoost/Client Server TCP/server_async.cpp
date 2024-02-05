#include <boost/asio.hpp>
#include <iostream>
#include <boost/bind.hpp>
#include <ctime>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <string>

using boost::asio::ip::tcp;

std::string make_daytime_string()
{
    using namespace std; // For time_t, time and ctime;
    time_t now = time(0);
    return ctime(&now);
}

class tcp_connection : public boost::enable_shared_from_this<tcp_connection>
{
private:
    // create the socket
    tcp::socket socket_;
    std::string message_;

public:
    // creates socket using the ioservice which is already accepted
    tcp_connection(boost::asio::io_service &io_service)
        : socket_(io_service)
    {
    }
    // shared_ptr and enable_shared_from_this 
    // because we want to keep the tcp_connection object alive as long as there is an operation that refers to it.
    typedef boost::shared_ptr<tcp_connection> pointer;
    static pointer create(boost::asio::io_service &io_service)
    {

        return pointer(new tcp_connection(io_service));
    }

    tcp::socket &socket()
    {

        return socket_;
    }

    void start(){

        message_ = make_daytime_string();
        // boost::asio::async_write() to serve the data to the client
        // boost::asio::async_write(), rather than ip::tcp::socket::async_write_some(), to ensure that the entire block of data is sent.
        boost::asio::async_write(
            socket_, 
            boost::asio::buffer(message_),
            boost::bind(
                &tcp_connection::handle_write, 
                shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred
                
            )
        );

    }

    void handle_write(const boost::system::error_code & /*error*/,
                      size_t /*bytes_transferred*/)
    {
        std::cout<<"Fulfilled Request\n";
    }
};

class tcp_server
{

private:
    tcp::acceptor acceptor_;

    // creates a socket and
    // initiates an asynchronous accept operation to wait for a new connection.
    void start_accept()
    {
        tcp_connection::pointer new_connection = tcp_connection::create(acceptor_.get_io_service());
        acceptor_.async_accept(

            new_connection->socket(), boost::bind(&tcp_server::handle_accept, this, new_connection, boost::asio::placeholders::error));
    }
    // The function handle_accept() is called when the asynchronous accept operation initiated by start_accept() finishes. 
    // It services the client request, and then calls start_accept() to initiate the next accept operation.
    void handle_accept(tcp_connection::pointer new_connection, const boost::system::error_code &error)
    {
        if (!error)
        {   
            // async write to the connection made
            new_connection->start();
        }
        // start to accet the new connection
        start_accept();
    }

public:
    // constructor inializes an acceptor
    tcp_server(boost::asio::io_service &io_service) : acceptor_(io_service, tcp::endpoint(tcp::v4(), 50000))
    {
        start_accept();
    }
};


int main()
{

    try
    {
        boost::asio::io_service io_service;
        // created a class of tcp_server
        tcp_server server(io_service);
        // using .run
        io_service.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}