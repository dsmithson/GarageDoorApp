#include "NetworkServer.h"

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;

using boost::asio::ip::tcp;

NetworkServer::NetworkServer(io_service& io_service, std::string password, function<bool()> getIsDoorOpen, function<bool()> toggleDoor, function<bool()> toggleLight, function<float()> getTemperature, function<float()> getHumidity)
	: io_service_(io_service), acceptor_(io_service, tcp::endpoint(tcp::v4(), 5174)), password_(password), getIsDoorOpen_(getIsDoorOpen), toggleDoor_(toggleDoor), toggleLight_(toggleLight), getTemperature_(getTemperature), getHumidity_(getHumidity)
{
    start_accept();
}

NetworkServer::~NetworkServer()
{
    //dtor
}

void NetworkServer::start_accept()
{
    NetworkSession* new_session = new NetworkSession(io_service_, password_, getIsDoorOpen_, toggleDoor_, toggleLight_, getTemperature_, getHumidity_);
    acceptor_.async_accept(new_session->socket(),
                          boost::bind(&NetworkServer::handle_accept, this, new_session,
                                      boost::asio::placeholders::error));

}

void NetworkServer::handle_accept(NetworkSession* new_session, const boost::system::error_code& error)
{
    if(!error)
    {
        new_session->start();
    }
    else
    {
        delete new_session;
    }

    start_accept();
}

/*
const short Port = 5174;

        NetworkServer(boost::asio::io_service& service);
        virtual ~NetworkServer();

    private:
        void beginAcceptConnection();
        void beginReceive();
        void handleReceive(const boost::system::error_code& error, std::size_t bytesTransferred);
        void processReceivedData(char* data, std::size_t count);
*/
