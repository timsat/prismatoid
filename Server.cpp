#include "Server.hpp"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "ApiParser.hpp"


namespace prismatoid {

    using boost::asio::ip::tcp;

    void Session::start() {
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                boost::bind(&Session::handle_read, this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
    }

    void Session::handle_read(const boost::system::error_code & error, size_t bytes_transferred) {
        if (!error) {
            std::string in(data_, bytes_transferred);
            api::ApiParser::parsecmd(in);

            socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                    boost::bind(&Session::handle_read, this,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred));
        } else {
            delete this;
        }
    }

    void Session::handle_write(const boost::system::error_code & error) {
        if (!error) {
            socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                    boost::bind(&Session::handle_read, this,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred));
        } else {
            delete this;
        }
    }

    Server::Server(boost::asio::io_service & io_service, short port)
        :io_service_(io_service), acceptor_(io_service, tcp::endpoint(tcp::v4(), port)) {
        start_accept();
    }
    void Server::start_accept() {
        Session *new_session = new Session(io_service_);
        acceptor_.async_accept(new_session->socket(),
                               boost::bind(&Server::handle_accept, this, new_session,
                                           boost::asio::placeholders::error));
    }

    void Server::handle_accept(Session * new_session, const boost::system::error_code & error) {
        if (!error) {
            new_session->start();
        } else {
            delete new_session;
        }

        start_accept();
    }

} // namespace prismatoid
