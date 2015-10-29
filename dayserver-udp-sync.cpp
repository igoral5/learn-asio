/**
 * @file dayserver-udp-sync.cpp
 * @author igor
 * @date 28 окт. 2015 г.
 */

#include <iostream>
#include <exception>
#include <cstdlib>
#include <string>
#include <ctime>

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/lexical_cast.hpp>

std::string
make_daytime_string()
{
    time_t now = time(nullptr);
    return ctime(&now);
}


int
main(int argc, char *argv[])
try
{
    if (argc < 2)
    {
        std::cerr << "Usage: dayserver-udp-sync <port>" << std::endl;
        return EXIT_FAILURE;
    }
    boost::asio::io_service io;
    boost::asio::ip::udp::socket socket(io,
            boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(),
                    boost::lexical_cast<unsigned short>(argv[1])));
    for(;;)
    {
        boost::array<char, 1> recv_buf;
        boost::asio::ip::udp::endpoint remote_endpoint;
        boost::system::error_code error;
        socket.receive_from(boost::asio::buffer(recv_buf),
                remote_endpoint,
                0,
                error);
        if (error && error != boost::asio::error::message_size)
        {
            std::cerr << "error receive: " << error.message() << std::endl;
        }
        std::string message = make_daytime_string();
        size_t len = socket.send_to(boost::asio::buffer(message),
                remote_endpoint,
                0,
                error);
        if (error)
        {
            std::cerr << "error send: " << error.message() << std::endl;
        }
        else
        {
            std::cout << "Send " << len << " bytes to " << remote_endpoint << std::endl;
        }
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
