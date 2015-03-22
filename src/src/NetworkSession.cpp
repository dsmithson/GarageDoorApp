#include "NetworkSession.h"

using namespace std;

NetworkSession::NetworkSession(boost::asio::io_service& io_service, std::string password, function<bool()> getIsDoorOpen, function<bool()> toggleDoor, function<bool()> toggleLight)
	: socket_(io_service), password_(password), getIsDoorOpen_(getIsDoorOpen), toggleDoor_(toggleDoor), toggleLight_(toggleLight), userHasAuthenticated(false), userHasRequestedShutdown(false)
{
}

NetworkSession::~NetworkSession()
{
}

void NetworkSession::start()
{
	write_string("Login Required");
	/*socket_.async_read_some(boost::asio::buffer(data_, max_length),
							boost::bind(&NetworkSession::handle_read, this,
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred));*/
}

void NetworkSession::handle_read(const boost::system::error_code& error, size_t bytes_transferred)
{
	if (!error)
	{
		//Process received bytes, and cast them to lower characters as we fill our message buffer
		bool messageProcessed = false;

		for (int i = 0; i < bytes_transferred; i++)
		{
			char current = data_[i];

			if (current == '\n')
			{
				//Ignore
				continue;
			}
			else if (current == '\r')
			{
				if (msgBuffer.length() == 0)
				{
					write_string("");
				}
				else
				{
					process_buffer();
					messageProcessed = true;

					if (userHasRequestedShutdown)
						return;
				}
			}
			else
				msgBuffer += tolower(current);

			//Snaity check
			if (msgBuffer.length() > 1024)
			{
				cout << "Message buffer length is too long.  Resetting...";
				msgBuffer.clear();
				break;
			}
		}

		//Reset our msg buffer
		memset(data_, 0, max_length);

		//If we didn't get a full message yet, we need to handle starting a new read ourselves
		if (!messageProcessed)
		{
			socket_.async_read_some(boost::asio::buffer(data_, max_length),
				boost::bind(&NetworkSession::handle_read, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
		}
	}
	else
	{
		delete this;
	}
}

void NetworkSession::process_buffer()
{
	//Message buffer contents should be stored in lower case
	bool sendResponse = true;
	bool actionSucceeded = true;

	//Has user authenticated yet?
	if (!userHasAuthenticated)
	{
		if (msgBuffer == password_)
		{
			userHasAuthenticated = true;
			write_string("Authenticated");
		}
		else if(msgBuffer == "bye")
		{
		    //Allow user to close connection before authenticating
		    userHasRequestedShutdown = true;
		    socket_.close();
		    delete this;
		    return;
		}
		else
		{
			write_string("Invalid Password");
		}
		sendResponse = false;
	}
	else if (msgBuffer == "isdooropen?")
	{
		bool isDoorOpen = getIsDoorOpen_();
		write_string(isDoorOpen ? "yes" : "no");
		sendResponse = false;
	}
	else if (msgBuffer == "opendoor")
	{
		if (!getIsDoorOpen_())
			actionSucceeded = toggleDoor_();
	}
	else if (msgBuffer == "closedoor")
	{
		if (getIsDoorOpen_)
			actionSucceeded = toggleDoor_();
	}
	else if (msgBuffer == "toggledoor")
	{
		actionSucceeded = toggleDoor_();
	}
	else if (msgBuffer == "togglelight")
	{
		actionSucceeded = toggleLight_();
	}
	else if (msgBuffer == "bye")
	{
		userHasRequestedShutdown = true;
		socket_.close();
		delete this;
		return;
	}
	else
	{
		//Unrecognized command
		write_string("Unrecognized command: " + msgBuffer);
		sendResponse = false;
	}

	//Send response?
	if (sendResponse)
		write_string(actionSucceeded ? "OK" : "FAILED");

	//Reset message buffer
	msgBuffer.clear();
}

void NetworkSession::handle_write(const boost::system::error_code& error)
{
	if (!error)
	{
		//if (userHasRequestedShutdown)
	//	{
    //		socket_.close();
	//	}

		//Start reading more data
		socket_.async_read_some(boost::asio::buffer(data_, max_length),
			boost::bind(&NetworkSession::handle_read, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}
	else
	{
		delete this;
	}
}

void NetworkSession::write_string(std::string msg, bool appendPrompt)
{
	string fullMsg = msg;
	if (appendPrompt)
		fullMsg += ">";

	boost::asio::async_write(socket_,
		boost::asio::buffer(fullMsg.c_str(), fullMsg.length()),
		boost::bind(&NetworkSession::handle_write, this,
		boost::asio::placeholders::error));
}
