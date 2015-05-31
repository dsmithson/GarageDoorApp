#ifndef NETWORKSESSION_H
#define NETWORKSESSION_H

#include <cstdlib>
#include <iostream>
#include <functional>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class NetworkSession
{
    public:
		NetworkSession(boost::asio::io_service& service, std::string password, std::function<bool()> getIsDoorOpen, std::function<bool()> toggleDoor, std::function<bool()> toggleLight, std::function<float()> getTemperature, std::function<float()> getHumidity);
        virtual ~NetworkSession();
        tcp::socket& socket() { return socket_; }
        void start();

    private:
        void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
        void handle_write(const boost::system::error_code& error);
		void write_string(std::string msg, bool appendPrompt = true);
		void process_buffer();
		std::string password_;
		bool userHasAuthenticated;
		bool userHasRequestedShutdown;
		std::function<bool()> getIsDoorOpen_;
		std::function<bool()> toggleDoor_;
		std::function<bool()> toggleLight_;
		std::function<float()> getTemperature_;
		std::function<float()> getHumidity_;
		std::string msgBuffer;
        tcp::socket socket_;
        enum { max_length = 1024 };
        char data_[max_length];
};

#endif // NETWORKSESSION_H
