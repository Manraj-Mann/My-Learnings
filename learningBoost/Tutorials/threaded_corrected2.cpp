#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <chrono>

// adding one more thread to signify its independent of strands

void time_expired(const boost::system::error_code & /*e*/, const std::string id)
{
    std::cout << "thread " << id << " enters\n";
    boost::this_thread::sleep_for(boost::chrono::seconds(3));
    std::cout << "thread " << id << " ends\n";
}

int main()
{
    boost::asio::io_service io;
    boost::asio::strand st(io);

    // wrapping the async within strand so that it wont execute out of order

    boost::asio::deadline_timer timer1(io, boost::posix_time::seconds(5));
    boost::asio::deadline_timer timer2(io, boost::posix_time::seconds(5));
    boost::asio::deadline_timer timer3(io, boost::posix_time::seconds(6));

    timer1.async_wait(

        st.wrap(
            boost::bind(time_expired, boost::asio::placeholders::error, "Timer 1"))
        );

    timer2.async_wait(
        st.wrap(

            boost::bind(time_expired, boost::asio::placeholders::error, "Timer 2"))
        );

    timer3.async_wait(

        boost::bind(time_expired , boost::asio::placeholders::error , "Timer 3" )
    );
    // as these two threads are callin the .run function  , both the functuions are there in the callback function at the same time in two different threads

    boost::thread t1([&]()
                     { io.run(); });

    boost::thread t2([&]()
                     { io.run(); });

    t1.join();
    t2.join();
    std::cout << "Done\n";

    return 0;
}
