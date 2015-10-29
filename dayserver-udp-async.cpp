/**
 * @file dayserver-udp-async.cpp
 * @author igor
 * @date 28 окт. 2015 г.
 */

#define BOOST_ASIO_ENABLE_HANDLER_TRACKING

#include <iostream>
#include <exception>
#include <cstdlib>
#include <string>
#include <ctime>

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

std::string
make_daytime_string()
{
    time_t now = time(nullptr);
    return ctime(&now);
}

class udp_server
{
public:
    udp_server(boost::asio::io_service& io, unsigned short port) :
        m_socket(io, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port))
    {
        start_receive();
    }
    virtual ~udp_server() noexcept = default;
private:
    void start_receive()
    {
        m_socket.async_receive_from(
                boost::asio::buffer(m_recv_buf),
                m_remote_endpoint,
                boost::bind(
                        &udp_server::handler_receive,
                        this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    }
    void handler_receive(const boost::system::error_code& e, size_t len)
    {
        if (!e || e == boost::asio::error::message_size)
        {
            boost::shared_ptr<std::string> message(new std::string(make_daytime_string()));
            m_socket.async_send_to(
                    boost::asio::buffer(*message),
                    m_remote_endpoint,
                    boost::bind(
                            &udp_server::handler_send,
                            this,
                            message,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
            start_receive();
        }
    }
    void handler_send(boost::shared_ptr<std::string> message, const boost::system::error_code& e,
    		size_t len)
    {
        if (!e)
        {
            std::cout << "Send " << len << " bytes to " << m_remote_endpoint << std::endl;
        }
    }
    boost::asio::ip::udp::socket m_socket;
    boost::asio::ip::udp::endpoint m_remote_endpoint;
    boost::array<char, 1> m_recv_buf;
};

int
main(int argc, char *argv[])
try
{
    if (argc < 2)
    {
        std::cerr << "Usage: dayserver-udp-async <port>" << std::endl;
        return EXIT_FAILURE;
    }
    boost::asio::io_service io;
    udp_server server(io, boost::lexical_cast<unsigned short>(argv[1]));
    io.run();
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
