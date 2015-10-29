/**
 * @file dayserver-tcp-async.cpp
 * @author igor
 * @date 28 окт. 2015 г.
 */

#define BOOST_ASIO_ENABLE_HANDLER_TRACKING

#include <iostream>
#include <exception>
#include <cstdlib>
#include <ctime>
#include <string>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/lexical_cast.hpp>

std::string
make_daytime_string()
{
    time_t now = time(nullptr);
    return ctime(&now);
}

class tcp_connection : public boost::enable_shared_from_this<tcp_connection>
{
public:
    typedef boost::shared_ptr<tcp_connection> pointer;
    static pointer create(boost::asio::io_service& io)
    {
        return pointer(new tcp_connection(io));
    }
    virtual ~tcp_connection() noexcept = default;
    boost::asio::ip::tcp::socket& socket()
    {
        return m_socket;
    }
    void start()
    {
        m_message = make_daytime_string();
        boost::asio::async_write(m_socket, boost::asio::buffer(m_message),
                boost::bind(
                        &tcp_connection::handle_write,
                        shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    }
private:
    tcp_connection(boost::asio::io_service& io) : m_socket(io){    }
    void handle_write(const boost::system::error_code& e, size_t len)
    {
        std::cerr << "write " << len << " byte to " << m_socket.remote_endpoint() << std::endl;
    }
    boost::asio::ip::tcp::socket m_socket;
    std::string m_message;
};

class tcp_server
{
public:
    tcp_server(boost::asio::io_service& io, unsigned short port) :
        m_acceptor(io, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
    {
        start_accept();
    }
    virtual ~tcp_server() noexcept = default;
private:
    void start_accept()
    {
        tcp_connection::pointer new_connection = tcp_connection::create(m_acceptor.get_io_service());
        m_acceptor.async_accept(new_connection -> socket(), boost::bind(&tcp_server::handle_accept,
                this,
                new_connection,
                boost::asio::placeholders::error));
    }
    void handle_accept(tcp_connection::pointer new_connection, const boost::system::error_code& e)
    {
        if (!e)
        {
            std::cerr << "accept connection from " << new_connection -> socket().remote_endpoint()
                    << std::endl;
            new_connection -> start();
        }
        start_accept();
    }
    boost::asio::ip::tcp::acceptor m_acceptor;
};

int
main(int argc, char *argv[])
try
{
    if (argc < 2)
    {
        std::cerr << "Usage: dayserver-tcp-async <port>" << std::endl;
        return EXIT_FAILURE;
    }
    boost::asio::io_service io;
    tcp_server server(io, boost::lexical_cast<unsigned short>(argv[1]));
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
