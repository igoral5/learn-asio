/**
 * @file dayserver-comb-async.cpp
 * @author igor
 * @date 29 окт. 2015 г.
 */

#define BOOST_ASIO_ENABLE_HANDLER_TRACKING

#include <iostream>
#include <exception>
#include <cstdlib>
#include <string>
#include <ctime>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

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
    tcp_connection(const tcp_connection&) = delete;
    tcp_connection& operator=(const tcp_connection&) = delete;
    tcp_connection(tcp_connection&&) noexcept = default;
    tcp_connection& operator=(tcp_connection&&) noexcept = default;
    boost::asio::ip::tcp::socket& socket()
    {
        return m_socket;
    }
    void start()
    {
        m_message = make_daytime_string();
        boost::asio::async_write(m_socket, boost::asio::buffer(m_message), boost::bind(
                &tcp_connection::handle_write,
                shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }
private:
    tcp_connection(boost::asio::io_service& io) : m_socket(io) {}
    void handle_write(const boost::system::error_code& e, size_t len)
    {
        if (!e)
        {
            std::cout << "tcp write " << len << " bytes to " << m_socket.remote_endpoint()
                    << std::endl;
        }
        else if (e != boost::asio::error::operation_aborted)
        {
            std::cerr << "error tcp write: " << e.message() << std::endl;
        }
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
    tcp_server(const tcp_server&) = delete;
    tcp_server& operator=(const tcp_server&) = delete;
    tcp_server(tcp_server&&) noexcept = default;
    tcp_server& operator=(tcp_server&&) noexcept = default;
private:
    void start_accept()
    {
        tcp_connection::pointer new_connection = tcp_connection::create(m_acceptor.get_io_service());
        m_acceptor.async_accept(new_connection -> socket(), boost::bind(
                &tcp_server::handle_accept,
                this,
                new_connection,
                boost::asio::placeholders::error));
    }
    void handle_accept(tcp_connection::pointer new_connection, const boost::system::error_code& e)
    {
        if (!e)
        {
            new_connection -> start();
        }
        else if (e != boost::asio::error::operation_aborted)
        {
            std::cerr << "error tcp accept: " << e.message() << std::endl;
        }
        start_accept();
    }
    boost::asio::ip::tcp::acceptor m_acceptor;
};

class udp_server
{
public:
    udp_server(boost::asio::io_service& io, unsigned short port) :
        m_socket(io, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port))
    {
        start_receive();
    }
    virtual ~udp_server() noexcept = default;
    udp_server(const udp_server&) = delete;
    udp_server& operator=(const udp_server&) = delete;
    udp_server(udp_server&&) noexcept = default;
    udp_server& operator=(udp_server&&) noexcept = default;
private:
    void start_receive()
    {
        m_socket.async_receive_from(boost::asio::buffer(m_recv_buf),
                m_remote_endpoint,
                boost::bind(
                        &udp_server::handle_receive,
                        this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
    }
    void handle_receive(const boost::system::error_code& e, size_t len)
    {
        if (!e || e == boost::asio::error::message_size)
        {
            boost::shared_ptr<std::string> message(new std::string(make_daytime_string()));
            m_socket.async_send_to(boost::asio::buffer(*message),
                    m_remote_endpoint,
                    boost::bind(
                            &udp_server::handle_send,
                            this,
                            message,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
        }
        else if (e != boost::asio::error::operation_aborted)
        {
            std::cerr << "error udp receive: " << e.message() << std::endl;
        }
        start_receive();
    }
    void handle_send(boost::shared_ptr<std::string> message, const boost::system::error_code& e,
            size_t len)
    {
        if (!e || e == boost::asio::error::message_size)
        {
            std::cout << "udp send " << len  << " bytes to " << m_remote_endpoint
                    << std::endl;
        }
        else if (e != boost::asio::error::operation_aborted)
        {
            std::cerr << "error udp send: " << e.message() << std::endl;
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
        std::cerr << "Usage: dayserver-comb-async <port>" << std::endl;
        return EXIT_FAILURE;
    }
    boost::asio::io_service io;
    unsigned short port = boost::lexical_cast<unsigned short>(argv[1]);
    udp_server server1(io, port);
    tcp_server server2(io, port);
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
