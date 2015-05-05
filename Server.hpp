#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/asio.hpp>

namespace prismatoid {
    using boost::asio::ip::tcp;

    class Session {
    public:
        Session(boost::asio::io_service & io_service)
            :socket_(io_service) {}

        tcp::socket & socket() {
            return socket_;
        }

        void start();

    private:
        void handle_read(const boost::system::error_code & error, size_t bytes_transferred);
        void handle_write(const boost::system::error_code & error);

        tcp::socket socket_;
        enum { max_length = 1024 };
        char data_[max_length];
    };


    class Server {
    public:
        Server(boost::asio::io_service & io_service, short port);

    private:
        void start_accept();
        void handle_accept(Session * new_session, const boost::system::error_code & error);

        boost::asio::io_service & io_service_;
        tcp::acceptor acceptor_;
    };

} /* prismatoid */

#endif /* SERVER_HPP */

