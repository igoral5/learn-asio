/**
 * @file dayclient-udp-async.cpp
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
#include <boost/bind.hpp>
#include <boost/array.hpp>

class udp_client
{
public:
	udp_client(boost::asio::io_service& io, const std::string& host, const std::string& port) :
		m_resolver(io),
		m_socket(io),
		m_send_buf{{ 0 }}
	{
		boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), host, port);
		m_resolver.async_resolve(query,
				boost::bind(
						&udp_client::handler_resolver,
						this,
						boost::asio::placeholders::error,
						boost::asio::placeholders::iterator));
	}
	virtual ~udp_client() noexcept = default;
private:
	void handler_resolver(const boost::system::error_code& e, boost::asio::ip::udp::resolver::iterator it)
	{
		if (!e)
		{
			m_socket.async_connect(*it, boost::bind(
					&udp_client::handler_connect,
					this,
					boost::asio::placeholders::error));
		}
		else if (e != boost::asio::error::operation_aborted)
		{
			std::cerr << "resolve: " << e.message() << std::endl;
		}
	}
	void handler_connect(const boost::system::error_code& e)
	{
		if (!e)
		{
			m_socket.async_send(boost::asio::buffer(m_send_buf),
					boost::bind(
							&udp_client::handler_send,
							this,
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred));
		}
		else if (e != boost::asio::error::operation_aborted)
		{
			std::cerr << "connect: " << e.message() << std::endl;
		}
	}
	void handler_send(const boost::system::error_code& e, size_t len)
	{
		if (!e)
		{
			m_socket.async_receive(boost::asio::buffer(m_recv_buf),
					boost::bind(
							&udp_client::handler_receive,
							this,
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred));
		}
		else if (e != boost::asio::error::operation_aborted)
		{
			std::cerr << "send: " << e.message() << std::endl;
		}
	}
	void handler_receive(const boost::system::error_code& e, size_t len)
	{
		if (!e)
		{
			std::cout.write(m_recv_buf.data(), len);
		}
		else if (e != boost::asio::error::operation_aborted)
		{
			std::cerr << "receive: " << e.message() << std::endl;
		}
	}
	boost::asio::ip::udp::resolver m_resolver;
	boost::asio::ip::udp::socket m_socket;
	boost::array<char, 128> m_recv_buf;
	boost::array<char, 1> m_send_buf;
};

int
main(int argc, char *argv[])
try
{
	if (argc < 3)
	{
		std::cerr << "Usage: dayclient-udp-async <host> <port>" << std::endl;
		return EXIT_FAILURE;
	}
	boost::asio::io_service io;
	udp_client client(io, argv[1], argv[2]);
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
