#ifndef NETWORKSERVER_H
#define NETWORKSERVER_H

#include "NetworkSession.h"
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <functional>

class NetworkServer
{
    public:

        NetworkServer(boost::asio::io_service& service, std::string password, std::function<bool()> getIsDoorOpen, std::function<bool()> toggleDoor, std::function<bool()> toggleLight);
        virtual ~NetworkServer();

    private:
        boost::asio::io_service& io_service_;
        tcp::acceptor acceptor_;
		std::string password_;
		std::function<bool()> getIsDoorOpen_;
		std::function<bool()> toggleLight_;
		std::function<bool()> toggleDoor_;
        void start_accept();
        void handle_accept(NetworkSession* new_session, const boost::system::error_code& error);
};

#endif // NETWORKSERVER_H
