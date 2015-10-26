/**
 * @file timer.cpp
 * @author igor
 * @date 26 окт. 2015 г.
 */

#include <iostream>
#include <exception>
#include <cstdlib>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

int
main(int argc, char *argv[])
try
{
	boost::asio::io_service io;
	boost::asio::deadline_timer timer(io, boost::posix_time::milliseconds(5000));
	timer.wait();
	std::cout << "Timer-sync" << std::endl;
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
