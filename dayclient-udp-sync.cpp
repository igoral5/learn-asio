/**
 * @file dayclient-udp-sync.cpp
 * @author igor
 * @date 28 окт. 2015 г.
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
    if (argc < 3)
    {
        std::cerr << "Usage: dayclient-udp-sync <host> <port>" << std::endl;
        return EXIT_FAILURE;
    }
    boost::asio::io_service io;
    boost::asio::ip::udp::resolver resolver(io);
    boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), argv[1], argv[2]);
    boost::asio::ip::udp::endpoint receiver_endpoint = *resolver.resolve(query);
    boost::asio::ip::udp::socket socket(io);
    socket.connect(receiver_endpoint);
    boost::array<char, 1> send_buf{{ 0 }};
    socket.send(boost::asio::buffer(send_buf));
    boost::array<char, 128> recv_buf;
    size_t len = socket.receive(boost::asio::buffer(recv_buf));
    std::cout.write(recv_buf.data(), len);
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
