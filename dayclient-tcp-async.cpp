/**
 * @file dayclient-tcp-async.cpp
 * @author igor
 * @date 27 окт. 2015 г.
 */

#define BOOST_ASIO_ENABLE_HANDLER_TRACKING

#include <iostream>
#include <exception>
#include <cstdlib>

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

class ClientDayTime
{
public:
    enum Status
    {
        resolving = 0,
        connecting,
        reading,
        success,
        failure
    };
    ClientDayTime(boost::asio::io_service& io,
            const std::string& host_name,
            const std::string& port,
            long timeout, size_t max_num) :
        m_resolver(io),
        m_socket(io),
        m_timer(io),
        m_timeout(timeout),
        m_num(0),
        m_max_num(max_num)
    {
        boost::asio::ip::tcp::resolver::query query(host_name, port);
        m_resolver.async_resolve(query,
                boost::bind(&ClientDayTime::handler_resolver,
                this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::iterator));
        restart_timer();
        m_status = resolving;
    }
    virtual ~ClientDayTime() noexcept = default;
    ClientDayTime(const ClientDayTime&) = delete;
    ClientDayTime& operator=(const ClientDayTime&) = delete;
    ClientDayTime(ClientDayTime&&) noexcept = default;
    ClientDayTime& operator=(ClientDayTime&&) noexcept = default;
    Status get_status()
    {
        return m_status;
    }
private:
    void restart_timer()
    {
        m_timer.expires_from_now(boost::posix_time::milliseconds(m_timeout));
        m_timer.async_wait(boost::bind(&ClientDayTime::handler_timer,
                this,
                boost::asio::placeholders::error));
    }
    void handler_timer(const boost::system::error_code& e)
    {
        if (!e)
        {
            switch (m_status)
            {
            case connecting:
                std::cerr << "time out connect to " << m_iterator -> endpoint() << std::endl;
                if (!next_connect())
                {
                    m_timer.get_io_service().stop();
                    m_status = failure;
                }
                break;
            case reading:
                std::cerr << "time out reading to " << m_iterator -> endpoint() << std::endl;
                reread();
                break;
            case resolving:
                std::cerr << "time out resolve" << std::endl;
                m_resolver.cancel();
                m_status = failure;
                break;
            default:
                break;
            }
        }
        else if (e != boost::asio::error::operation_aborted)
        {
            std::cerr << "timer: " << e.message() << std::endl;
            m_timer.get_io_service().stop();
            m_status = failure;
        }
    }
    void handler_resolver(const boost::system::error_code& e,
            boost::asio::ip::tcp::resolver::iterator it)
    {
        if (!e)
        {
            m_iterator = it;
            connect();
        }
        else if (e != boost::asio::error::operation_aborted)
        {
            std::cerr << "resolve: " << e.message() << std::endl;
            m_timer.cancel();
            m_status = failure;
        }
    }
    void handler_connect(const boost::system::error_code& e)
    {
        if (!e)
        {
            m_socket.async_read_some(boost::asio::buffer(m_buf),
                    boost::bind(&ClientDayTime::handler_read,
                    this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
            restart_timer();
            m_status = reading;
        }
        else if (e != boost::asio::error::operation_aborted)
        {
            std::cerr << "connect to " << m_iterator -> endpoint() << ":" << e.message()
                    << std::endl;
            if (!next_connect())
            {
                m_timer.cancel();
                m_status = failure;
            }
        }
    }
    void handler_read(const boost::system::error_code& e, size_t len)
    {
        if (!e)
        {
            std::cout.write(m_buf.data(), len);
            m_socket.async_read_some(boost::asio::buffer(m_buf), boost::bind(
                    &ClientDayTime::handler_read,
                    this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
            restart_timer();
            m_status = reading;
        }
        else if (e == boost::asio::error::eof)
        {
            m_timer.cancel();
            m_status = success;
        }
        else if (e != boost::asio::error::operation_aborted)
        {
            m_timer.cancel();
            std::cerr << "read: " << e.message() << std::endl;
            reread();
        }
    }
    bool connect()
    {
        if (m_iterator != boost::asio::ip::tcp::resolver::iterator())
        {
            std::cout << "Connect to " << m_iterator -> endpoint() << std::endl;
            m_socket.async_connect(m_iterator -> endpoint(), boost::bind(
                    &ClientDayTime::handler_connect,
                    this,
                    boost::asio::placeholders::error));
            restart_timer();
            m_status = connecting;
            return true;
        }
        return false;
    }
    bool reconnect()
    {
        m_socket.cancel();
        m_socket.close();
        return connect();
    }
    bool next_connect()
    {
        m_iterator++;
        m_num = 0;
        return reconnect();
    }
    void reread()
    {
        m_num++;
        if (m_num < m_max_num)
        {
            reconnect();
        }
        else
        {
            if(!next_connect())
            {
                m_status = failure;
            }
        }
    }
    boost::asio::ip::tcp::resolver m_resolver;
    boost::asio::ip::tcp::socket m_socket;
    boost::asio::deadline_timer m_timer;
    long m_timeout;
    boost::array<char, 128> m_buf;
    boost::asio::ip::tcp::resolver::iterator m_iterator;
    Status m_status;
    size_t m_num;
    size_t m_max_num;
};


int
main(int argc, char *argv[])
try
{
    if (argc != 3)
    {
        std::cerr << "Usage: dayclient-tcp-asyn <host> <port>" << std::endl;
        return EXIT_FAILURE;
    }
    boost::asio::io_service io;
    ClientDayTime client(io, argv[1], argv[2], 1000, 3);
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
