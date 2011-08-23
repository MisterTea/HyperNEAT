#ifndef __JGTL_EMAIL_H__
#define __JGTL_EMAIL_H__

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

//This adds a lot to compile time but must be here otherwise winsock.h gets
//included first and all hell breaks loose
#include <boost/asio.hpp>

//#define DISABLE_EMAIL

namespace JGTL
{
	using namespace boost;
	using namespace boost::asio;
	using namespace boost::asio::ip;

	inline bool SendEmail(
			const string &from,
			const string &fromName,
			const string &to,
			const string &subject,
			const string &body,
			const string &hostName
			)
	{
#ifdef DISABLE_EMAIL
		return true;
#endif

		asio::io_service* io_service = new asio::io_service();

		tcp::resolver resolver(*io_service);

		tcp::resolver::query query(hostName,"25");

		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
		tcp::resolver::iterator end;

		asio::ip::tcp::socket* ircSocket = new asio::ip::tcp::socket(*io_service);

		boost::system::error_code error = boost::asio::error::host_not_found;
		while (error && endpoint_iterator != end)
		{
			ircSocket->close();
			ircSocket->connect(*endpoint_iterator++, error);
		}
		if (error)
		{
			cout << "ERROR TRYING TO CONNECT!\n";
			return false;
		}

		{
			while(ircSocket->available()==0)
			{
				int tmp=0;
			}

			size_t bytesInBuffer = ircSocket->available();

			if(bytesInBuffer)
			{
				boost::array<char, 4096> buf;
				memset(buf.data(),0,4096);
				size_t len = ircSocket->read_some(boost::asio::buffer(buf.data(),4096), error);
				cout << "BUFFER: " << buf.data() << endl;
			}
		}

		{
			ostringstream ostr;
			ostr << "HELO server \r\n";
			cout << "Sending: " << ostr.str() << endl;
			ircSocket->write_some(boost::asio::buffer(ostr.str().c_str()),error);
		}
		if (error)
		{
			cout << "ERROR SENDING MESSAGE!\n";
			return false;
		}

		{
			while(ircSocket->available()==0)
			{
				int tmp=0;
			}

			size_t bytesInBuffer = ircSocket->available();

			if(bytesInBuffer)
			{
				boost::array<char, 4096> buf;
				memset(buf.data(),0,4096);
				size_t len = ircSocket->read_some(boost::asio::buffer(buf.data(),4096), error);
				cout << "BUFFER: " << buf.data() << endl;
			}
		}

		{
			ostringstream ostr;
			ostr << "MAIL FROM:<" << from << ">\r\n";
			cout << "Sending: " << ostr.str() << endl;
			ircSocket->write_some(boost::asio::buffer(ostr.str().c_str()),error);
		}
		if (error)
		{
			cout << "ERROR SENDING MESSAGE!\n";
			return false;
		}

		{
			while(ircSocket->available()==0)
			{
				int tmp=0;
			}

			size_t bytesInBuffer = ircSocket->available();

			if(bytesInBuffer)
			{
				boost::array<char, 4096> buf;
				memset(buf.data(),0,4096);
				size_t len = ircSocket->read_some(boost::asio::buffer(buf.data(),4096), error);
				cout << "BUFFER: " << buf.data() << endl;
			}
		}

		{
			ostringstream ostr;
			ostr << "RCPT TO:<" << to << ">\r\n";
			cout << "Sending: " << ostr.str() << endl;
			ircSocket->write_some(boost::asio::buffer(ostr.str().c_str()),error);
		}
		if (error)
		{
			cout << "ERROR SENDING MESSAGE!\n";
			return false;
		}

		{
			while(ircSocket->available()==0)
			{
				int tmp=0;
			}

			size_t bytesInBuffer = ircSocket->available();

			if(bytesInBuffer)
			{
				boost::array<char, 4096> buf;
				memset(buf.data(),0,4096);
				size_t len = ircSocket->read_some(boost::asio::buffer(buf.data(),4096), error);
				cout << "BUFFER: " << buf.data() << endl;
			}
		}

		{
			ostringstream ostr;
			ostr << "DATA " << "\r\n";
			cout << "Sending: " << ostr.str() << endl;
			ircSocket->write_some(boost::asio::buffer(ostr.str().c_str()),error);
		}
		if (error)
		{
			cout << "ERROR SENDING MESSAGE!\n";
			return false;
		}

		{
			while(ircSocket->available()==0)
			{
				int tmp=0;
			}

			size_t bytesInBuffer = ircSocket->available();

			if(bytesInBuffer)
			{
				boost::array<char, 4096> buf;
				memset(buf.data(),0,4096);
				size_t len = ircSocket->read_some(boost::asio::buffer(buf.data(),4096), error);
				cout << "BUFFER: " << buf.data() << endl;
			}
		}

		{
			ostringstream ostr;
			ostr << "FROM: " << fromName << "<" << from << ">\r\n";
			cout << "Sending: " << ostr.str() << endl;
			ircSocket->write_some(boost::asio::buffer(ostr.str().c_str()),error);
		}
		if (error)
		{
			cout << "ERROR SENDING MESSAGE!\n";
			return false;
		}
		{
			ostringstream ostr;
			ostr << "TO:<" << to << ">\r\n";
			cout << "Sending: " << ostr.str() << endl;
			ircSocket->write_some(boost::asio::buffer(ostr.str().c_str()),error);
		}
		if (error)
		{
			cout << "ERROR SENDING MESSAGE!\n";
			return false;
		}

		{
			ostringstream ostr;
			ostr << "SUBJECT:" << subject << "\r\n";
			cout << "Sending: " << ostr.str() << endl;
			ircSocket->write_some(boost::asio::buffer(ostr.str().c_str()),error);
		}
		if (error)
		{
			cout << "ERROR SENDING MESSAGE!\n";
			return false;
		}
		{
			ostringstream ostr;
			ostr << "Content-type: text/plain; charset=US-ASCII\r\nContent-Transfer-Encoding: 7bit\r\n\r\n";
			ostr << body << "\r\n";
			cout << "Sending: " << ostr.str() << endl;
			ircSocket->write_some(boost::asio::buffer(ostr.str().c_str()),error);
		}
		if (error)
		{
			cout << "ERROR SENDING MESSAGE!\n";
			return false;
		}
		{
			ostringstream ostr;
			ostr << "\r\n.\r\n";
			cout << "Sending: " << ostr.str() << endl;
			ircSocket->write_some(boost::asio::buffer(ostr.str().c_str()),error);
		}
		if (error)
		{
			cout << "ERROR SENDING MESSAGE!\n";
			return false;
		}

		{
			while(ircSocket->available()==0)
			{
				int tmp=0;
			}

			size_t bytesInBuffer = ircSocket->available();

			if(bytesInBuffer)
			{
				boost::array<char, 4096> buf;
				memset(buf.data(),0,4096);
				size_t len = ircSocket->read_some(boost::asio::buffer(buf.data(),4096), error);
				cout << "BUFFER: " << buf.data() << endl;
			}
		}

		{
			ostringstream ostr;
			ostr << "QUIT " << "\r\n";
			cout << "Sending: " << ostr.str() << endl;
			ircSocket->write_some(boost::asio::buffer(ostr.str().c_str()),error);
		}
		if (error)
		{
			cout << "ERROR SENDING MESSAGE!\n";
			return false;
		}

		{
			while(ircSocket->available()==0)
			{
				int tmp=0;
			}

			size_t bytesInBuffer = ircSocket->available();

			if(bytesInBuffer)
			{
				boost::array<char, 4096> buf;
				memset(buf.data(),0,4096);
				size_t len = ircSocket->read_some(boost::asio::buffer(buf.data(),4096), error);
				cout << "BUFFER: " << buf.data() << endl;
			}
		}

		ircSocket->close();
		delete ircSocket;

		delete io_service;

		return true;
	}
}

#endif


