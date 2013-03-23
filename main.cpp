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
                std::cout << data_;

                
                

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
      } private:
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
    libusb_context *ctx;
    cout << "Initializing usb..." << endl;
    if (libusb_init(&ctx) == 0) {

#ifdef DEBUG
        cout << "debugging output enabled" << endl;
        libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_DEBUG);
#endif

        libusb_device_handle *devHandle = libusb_open_device_with_vid_pid(ctx, 0x03EB, 0x204F);
        if (devHandle) {

            libusb_detach_kernel_driver(devHandle, 0);
            libusb_claim_interface(devHandle, 0);
            unsigned char buf[61];

            buf[0] = 0x01;

            for (int i = 0; i < 10; i++) {
                int idx = i * 6 + 1;
                buf[idx++] = 0x00;
                buf[idx++] = 0x00;
                buf[idx++] = 0x00;

                buf[idx++] = 0x00;
                buf[idx++] = 0x00;
                buf[idx++] = 0x00;
            }
            int transferred = 0;


            int result = libusb_control_transfer(devHandle,
                                                 LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_CLASS
                                                 | LIBUSB_RECIPIENT_INTERFACE,
                                                 0x09,
                                                 (2 << 8),
                                                 0x00,
                                                 buf, sizeof(buf), 1000);

            cout << "result: " << result << endl;

            boost::asio::io_service io_srv;
            server *serv = new server(io_srv, 7777);
            io_srv.run();

            libusb_release_interface(devHandle, 0);
            libusb_attach_kernel_driver(devHandle, 0);

            libusb_close(devHandle);
            cout << "Lightpack is found" << endl;
        } else {
            cout << "Couldn't open Lightpack" << endl;
        }
        libusb_exit(ctx);
        return 0;
    } else {
        cout << "Couldn't init libusb" << endl;
    }
    return 1;
}
