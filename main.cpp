#include <libusb-1.0/libusb.h>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "LightpackDevice.hpp"
#include "ApiParser.hpp"

namespace prismatoid {
    using boost::asio::ip::tcp;

    class session {
      public:
        session(boost::asio::io_service & io_service)
        :socket_(io_service) {
        } tcp::socket & socket() {
            return socket_;
        }
        void start() {
            socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                    boost::bind(&session::handle_read, this,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred));
        }

      private:
        void handle_read(const boost::system::error_code & error, size_t bytes_transferred) {
            if (!error) {
                std::string in(data_, bytes_transferred);
                prismatoid::api::ApiParser::parsecmd(in);
                
                

                socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                        boost::bind(&session::handle_read, this,
                                                    boost::asio::placeholders::error,
                                                    boost::asio::placeholders::bytes_transferred));
            } else {
                delete this;
            }
        }

        void handle_write(const boost::system::error_code & error) {
            if (!error) {
                socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                        boost::bind(&session::handle_read, this,
                                                    boost::asio::placeholders::error,
                                                    boost::asio::placeholders::bytes_transferred));
            } else {
                delete this;
            }
        }

        tcp::socket socket_;
        enum { max_length = 1024 };
        char data_[max_length];
    };

    class server {
      public:
        server(boost::asio::io_service & io_service, short port)
        :io_service_(io_service), acceptor_(io_service, tcp::endpoint(tcp::v4(), port)) {
            start_accept();
      }
      private:
        void start_accept() {
            session *new_session = new session(io_service_);
            acceptor_.async_accept(new_session->socket(),
                                   boost::bind(&server::handle_accept, this, new_session,
                                               boost::asio::placeholders::error));
        }

        void handle_accept(session * new_session, const boost::system::error_code & error) {
            if (!error) {
                new_session->start();
            } else {
                delete new_session;
            }

            start_accept();
        }

        boost::asio::io_service & io_service_;
        tcp::acceptor acceptor_;
    };

}

int main() {
    using namespace std;
    using namespace prismatoid;

    const int kMaxCmdLength = 1024;
    char buf[kMaxCmdLength];
    libusb_context *ctx;
    cout << "Initializing usb... ";
    if (libusb_init(&ctx) == 0) {
        cout << "ok" << endl;

#ifdef DEBUG
        cout << "debugging output enabled" << endl;
        libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_DEBUG);
#endif
        devices::LightpackDevice * dev = devices::LightpackDevice::init(ctx);
        
        
        cout << "Opening lightpack... ";
        if (dev->open()) {
            cout << "ok" << endl;
        } else {
            cout << "couldn't open lightpack" << endl;
            return -1;
        }

        int commandsHandled = 0;
        while (0 && !cin.eof()) {
            memset(buf, 0, sizeof(buf));
            cin.getline(buf, kMaxCmdLength);
            std::string in(buf);
            if (in.size() > 0) {
                commandsHandled++;
                prismatoid::api::ApiParser::parsecmd(in);
            }
        }
        if (commandsHandled == 0) {
            cout << "running server at port 7777..." << endl;
            boost::asio::io_service io_srv;
            server *serv = new server(io_srv, 7777);
            io_srv.run();
        }

        dev->close();

        libusb_exit(ctx);
        return 0;
    } else {
        cout << "Couldn't init libusb" << endl;
    }
    return 1;
}
