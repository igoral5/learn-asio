/**
 * @file timer-bind.cpp
 * @author igor
 * @date 26 окт. 2015 г.
 */

#include <iostream>
#include <exception>
#include <cstdlib>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

void
handler_dead_timer(const boost::system::error_code& e, boost::asio::deadline_timer& timer,
        int& count)
{
    if (!e && count < 5)
    {
        std::cout << "Timer-bind: " << count++ << std::endl;
        timer.expires_at(timer.expires_at() + boost::posix_time::seconds(1));
        timer.async_wait(boost::bind(
                handler_dead_timer,
                boost::asio::placeholders::error,
                boost::ref(timer),
                boost::ref(count)));
    }
}


int
main(int argc, char *argv[])
try
{
    boost::asio::io_service io;
    boost::asio::deadline_timer timer(io, boost::posix_time::seconds(1));
    int count = 0;
    timer.async_wait(boost::bind(
            handler_dead_timer,
            boost::asio::placeholders::error,
            boost::ref(timer),
            boost::ref(count)));
    io.run();
    std::cout << "Final timer-bind: " << count << std::endl;
    return EXIT_SUCCESS;
}
catch (const std::exception& e)
{
    std::cerr << "Exception: " << e.what() << std::endl;
    return EXIT_FAILURE;
}
catch(...)
{
    std::cerr << "Unknown exceprion" << std::endl;
    return EXIT_FAILURE;
}
