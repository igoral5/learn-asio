/**
 * @file timer-member.cpp
 * @author igor
 * @date 26 окт. 2015 г.
 */

#include <iostream>
#include <exception>
#include <cstdlib>

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>

class Handler
{
public:
	Handler(boost::asio::io_service& io, long beg, long step, size_t count) :
		m_io(io),
		m_timer(m_io, boost::posix_time::seconds(beg)),
		m_step(step), m_count(count), m_current(0)
	{
		m_timer.async_wait(boost::bind(&Handler::handler, this, boost::asio::placeholders::error));
	}
	virtual ~Handler() noexcept
	{
		std::cout << "timer-member final: " << m_current << std::endl;
	}
	void handler(const boost::system::error_code& e)
	{
		std::cout << "handler error=" << e << std::endl;
		if (!e && m_current < m_count)
		{
			std::cout << "timer-member: " << m_current++ << std::endl;
			m_timer.expires_at(m_timer.expires_at() + boost::posix_time::seconds(m_step));
			m_timer.async_wait(boost::bind(&Handler::handler, this, boost::asio::placeholders::error));
		}
	}
private:
	boost::asio::io_service& m_io;
	boost::asio::deadline_timer m_timer;
	long m_step;
	size_t m_count;
	size_t m_current;
};


int
main(int argc, char *argv[])
try
{
	boost::asio::io_service io;
	Handler handler(io, 0, 2, 5);
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
