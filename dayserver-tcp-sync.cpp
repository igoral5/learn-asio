/**
 * @file dayserver-tcp-sync.cpp
 * @author igor
 * @date 28 окт. 2015 г.
 */

#include <iostream>
#include <exception>
#include <cstdlib>
#include <string>
#include <ctime>
#include <locale>

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>

std::string make_day_time_string()
{
    time_t now = time(nullptr);
    return ctime(&now);
}

int
main(int argc, char *argv[])
try
{
    std::locale::global(std::locale(""));
    tzset();
    if (argc < 2)
    {
        std::cerr << "Usage: dayserver-tcp-sync <port>" << std::endl;
        return EXIT_SUCCESS;
    }
    boost::asio::io_service io;
    boost::asio::ip::tcp::acceptor acceptor(io,
            boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),
                    boost::lexical_cast<unsigned short>(argv[1])));
    for(;;)
    {
        boost::asio::ip::tcp::socket socket(io);
        acceptor.accept(socket);
        std::string message = make_day_time_string();
        boost::system::error_code error;
        boost::asio::write(socket, boost::asio::buffer(message), error);
        std::cerr << error.message() << std::endl;
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
