/**
 * @file timer-async.cpp
 * @author igor
 * @date 26 окт. 2015 г.
 */

#include <iostream>
#include <exception>
#include <cstdlib>

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

void
handler(const boost::system::error_code& e)
{
    std::cout << "timer-async: " << e.message() << std::endl;
}

int
main(int argc, char *argv[])
try
{
    boost::asio::io_service io;
    boost::asio::deadline_timer timer(io, boost::posix_time::seconds(5));
    timer.async_wait(handler);
    io.run();
    std::cout << "Final timer-async" << std::endl;
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
