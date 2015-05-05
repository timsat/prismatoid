#include <libusb-1.0/libusb.h>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "LightpackDevice.hpp"
#include "ApiParser.hpp"
#include "Server.hpp"

int main(int argc, char ** argv) {
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
            int commandsHandled = 0;
            bool isProcessStdin = argc > 1 && strcmp(argv[1],"-f") == 0;
            while (isProcessStdin && !cin.eof()) {
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
                Server *serv = new Server(io_srv, 7777);
                io_srv.run();
            }

            dev->close();
        } else {
            libusb_exit(ctx);
            cout << "couldn't open lightpack" << endl;
            return -1;
        }

        libusb_exit(ctx);
        return 0;
    } else {
        cout << "Couldn't init libusb" << endl;
    }
    return 1;
}
