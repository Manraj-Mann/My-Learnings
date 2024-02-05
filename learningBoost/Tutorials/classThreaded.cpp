#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

class printer
{

private:
    boost::asio::strand st;
    boost::asio::deadline_timer t1;
    boost::asio::deadline_timer t2;
    int count;

public:
    printer(boost::asio::io_service &io)
        : st(io),
          t1(io, boost::posix_time::seconds(1)),
          t2(io, boost::posix_time::seconds(1)),
          count(0)
    {
        t1.async_wait(st.wrap(boost::bind(&printer::print1, this)));
        t2.async_wait(st.wrap(boost::bind(&printer::print2, this)));
    }

    void print1()
    {

        if (count < 10)
        {

            std::cout << "Timer 1: " << count << std::endl;
            ++count;
            t1.expires_at(t1.expires_at() + boost::posix_time::seconds(1));
            t1.async_wait(st.wrap(boost::bind(&printer::print1, this)));
        }
    }

    void print2()
    {

        if (count < 10)
        {

            std::cout << "Timer 2: " << count << std::endl;
            ++count;
            t2.expires_at(t2.expires_at() + boost::posix_time::seconds(1));
            t2.async_wait(st.wrap(boost::bind(&printer::print2, this)));
        }
    }
};

int main()
{

    boost::asio::io_service io;
    printer p(io);
    boost::thread t(boost::bind(&boost::asio::io_service::run , &io));

    io.run();
    t.join();

    return 0;
}