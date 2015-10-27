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
	ClientDayTime(boost::asio::io_service& io, const std::string& host_name, long timeout) :
		m_io(io),
		m_resolver(io),
		m_socket(io),
		m_timer(io, boost::posix_time::milliseconds(timeout)),
		m_timeout(timeout)

	{
		boost::asio::ip::tcp::resolver::query query(host_name, "daytime");
		m_resolver.async_resolve(query, boost::bind(&ClientDayTime::handler_resolver,
				this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::iterator));
		m_timer.async_wait(boost::bind(&ClientDayTime::handler_timer,
				this,
				boost::asio::placeholders::error));

	}
	virtual ~ClientDayTime() noexcept = default;
	void handler_timer(const boost::system::error_code& e)
	{
		if (!e)
		{
			std::cerr << "Превышен лимит операции" << std::endl;
			m_io.stop();
		}
		else if (e != boost::asio::error::operation_aborted)
		{
			std::cerr << "Ошибка timer: " << e.message() << std::endl;
		}
	}
	void handler_resolver(const boost::system::error_code& e, boost::asio::ip::tcp::resolver::iterator it)
	{
		if (!e)
		{
			m_socket.async_connect(it -> endpoint(), boost::bind(&ClientDayTime::handler_connect,
					this,
					boost::asio::placeholders::error));
			m_timer.expires_from_now(boost::posix_time::milliseconds(m_timeout));
			m_timer.async_wait(boost::bind(&ClientDayTime::handler_timer,
					this,
					boost::asio::placeholders::error));
		}
		else
		{
			std::cerr << "Ошибка resolv: " << e.message() << std::endl;
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
			m_timer.expires_from_now(boost::posix_time::milliseconds(m_timeout));
			m_timer.async_wait(boost::bind(&ClientDayTime::handler_timer,
								this,
								boost::asio::placeholders::error));
		}
		else
		{
			std::cerr << "Ошибка connect: " << e.message() << std::endl;
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
			m_timer.expires_from_now(boost::posix_time::milliseconds(m_timeout));
			m_timer.async_wait(boost::bind(&ClientDayTime::handler_timer,
								this,
								boost::asio::placeholders::error));
		}
		else
		{
			m_timer.cancel();
			if (e != boost::asio::error::eof)
			{
				std::cerr << "Ошибка read: " << e.message() << std::endl;
			}
		}
	}
private:
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
	if (argc != 2)
	{
		std::cerr << "Usage: dayclient-tcp-asyn <host>" << std::endl;
		return EXIT_FAILURE;
	}
	boost::asio::io_service io;
	ClientDayTime client(io, argv[1], 1000);
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