/**
 * @file timer-thread.cpp
 * @author igor
 * @date 27 окт. 2015 г.
 */

#include <iostream>
#include <exception>
#include <cstdlib>

#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


class Handler
{
public:
	Handler(boost::asio::io_service& io) :
		m_strand(io),
		m_timer1(io, boost::posix_time::seconds(1)),
		m_timer2(io, boost::posix_time::seconds(1)),
		m_count(0)
	{
		m_timer1.async_wait(m_strand.wrap(boost::bind(&Handler::handler1, this, boost::asio::placeholders::error)));
		m_timer2.async_wait(m_strand.wrap(boost::bind(&Handler::handler2, this, boost::asio::placeholders::error)));
	}
	virtual ~Handler() noexcept
	{
		std::cout << "Destructor Handler m_count: " << m_count << std::endl;
	}
	void handler1(const boost::system::error_code& e)
	{
		if (!e && m_count < 10)
		{
			std::cout << "Timer1: " << m_count++ << " thread_id: " << boost::this_thread::get_id() << std::endl;
			m_timer1.expires_at(m_timer1.expires_at() + boost::posix_time::seconds(1));
			m_timer1.async_wait(m_strand.wrap(boost::bind(&Handler::handler1, this, boost::asio::placeholders::error)));
		}
	}
	void handler2(const boost::system::error_code& e)
	{
		if (!e && m_count < 10)
		{
			std::cout << "Timer2: " << m_count++ << " thread_id: " << boost::this_thread::get_id() << std::endl;
			m_timer2.expires_at(m_timer2.expires_at() + boost::posix_time::seconds(1));
			m_timer2.async_wait(m_strand.wrap(boost::bind(&Handler::handler2, this, boost::asio::placeholders::error)));
		}
	}
private:
	boost::asio::strand m_strand;
	boost::asio::deadline_timer m_timer1;
	boost::asio::deadline_timer m_timer2;
	size_t m_count;
};

int
main(int argc, char *argv[])
try
{
	boost::asio::io_service io;
	Handler handler(io);
	boost::thread th(boost::bind(&boost::asio::io_service::run, &io));
	io.run();
	th.join();
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
