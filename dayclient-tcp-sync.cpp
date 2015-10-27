/**
 * @file dayclient-tcp-sync.cpp
 * @author igor
 * @date 27 окт. 2015 г.
 */

#include <iostream>
#include <exception>
#include <cstdlib>

#include <boost/asio.hpp>
#include <boost/array.hpp>


int
main(int argc, char *argv[])
try
{
	if (argc != 3)
	{
		std::cerr << "Usage: dayclient-tcp-syn <host> <port>" << std::endl;
		return EXIT_FAILURE;
	}
	boost::asio::io_service io;
	boost::asio::ip::tcp::resolver resolver(io);
	boost::asio::ip::tcp::resolver::query query(argv[1], argv[2]);
	boost::asio::ip::tcp::resolver::iterator ep_iterator = resolver.resolve(query);
	boost::asio::ip::tcp::socket socket(io);
	boost::asio::connect(socket, ep_iterator);

	for(;;)
	{
		boost::array<char, 128> buf;
		boost::system::error_code error;
		size_t len = socket.read_some(boost::asio::buffer(buf), error);
		if (error == boost::asio::error::eof)
			break;
		else if (error)
			throw boost::system::system_error(error);
		std::cout.write(buf.data(), len);

	}
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
