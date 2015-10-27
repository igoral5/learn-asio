/**
 * @file dayclient-tcp-async.cpp
 * @author igor
 * @date 27 окт. 2015 г.
 */

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
	ClientDayTime(boost::asio::io_service& io, const std::string& host_name, const std::string& port, long timeout) :
		m_io(io),
		m_resolver(io),
		m_socket(io),
		m_timer(io),
		m_timeout(timeout)

	{
		boost::asio::ip::tcp::resolver::query query(host_name, port);
		m_resolver.async_resolve(query,
				boost::bind(&ClientDayTime::handler_resolver,
				this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::iterator));
		restart_timer();

	}
	virtual ~ClientDayTime() noexcept = default;
	ClientDayTime(const ClientDayTime&) = delete;
	ClientDayTime& operator=(const ClientDayTime&) = delete;
	ClientDayTime(ClientDayTime&&) noexcept = default;
	ClientDayTime& operator=(ClientDayTime&&) noexcept = default;
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
			std::cerr << "time out" << std::endl;
			m_io.stop();
		}
		else if (e != boost::asio::error::operation_aborted)
		{
			std::cerr << "timer: " << e.message() << std::endl;
		}
	}
	void handler_resolver(const boost::system::error_code& e, boost::asio::ip::tcp::resolver::iterator it)
	{
		if (!e)
		{
			m_socket.async_connect(it -> endpoint(), boost::bind(&ClientDayTime::handler_connect,
					this,
					boost::asio::placeholders::error));
			restart_timer();
		}
		else
		{
			std::cerr << "resolve: " << e.message() << std::endl;
			m_timer.cancel();
		}
	}
	void handler_connect(const boost::system::error_code& e)
	{
		if (!e)
		{
			m_socket.async_read_some(boost::asio::buffer(m_buf), boost::bind(&ClientDayTime::handler_read,
					this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
			restart_timer();
		}
		else
		{
			std::cerr << "connect: " << e.message() << std::endl;
			m_timer.cancel();
		}
	}
	void handler_read(const boost::system::error_code& e, size_t len)
	{
		if (!e)
		{
			std::cout.write(m_buf.data(), len);
			m_socket.async_read_some(boost::asio::buffer(m_buf), boost::bind(&ClientDayTime::handler_read,
								this,
								boost::asio::placeholders::error,
								boost::asio::placeholders::bytes_transferred));
			restart_timer();
		}
		else
		{
			m_timer.cancel();
			if (e != boost::asio::error::eof)
			{
				std::cerr << "read: " << e.message() << std::endl;
			}
		}
	}
	boost::asio::io_service& m_io;
	boost::asio::ip::tcp::resolver m_resolver;
	boost::asio::ip::tcp::socket m_socket;
	boost::asio::deadline_timer m_timer;
	long m_timeout;
	boost::array<char, 128> m_buf;
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
	ClientDayTime client(io, argv[1], argv[2], 1000);
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
