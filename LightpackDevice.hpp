#ifndef LIGHTPACK_DEVICE_HPP
#define LIGHTPACK_DEVICE_HPP

#include <boost/utility.hpp>
#include <libusb-1.0/libusb.h>
#include "color_types.hpp"
#include <vector>

namespace prismatoid {
    namespace devices {

        const int kLightpackDeviceBufferSize = 61;
        
        namespace types = prismatoid::types;
        

        class LightpackDevice: public boost::noncopyable {
          public:
            static LightpackDevice * init(libusb_context *ctx);
            static LightpackDevice * instance();
             virtual ~LightpackDevice();

             bool open();
             void close();

             bool set_colors(const std::vector<types::RgbColor>& colors);

          protected:
             LightpackDevice(libusb_context *ctx);
             void write();

            static LightpackDevice *this_;

          private:
            unsigned char buf_[kLightpackDeviceBufferSize];
            libusb_context *ctx_;
            libusb_device_handle *dev_;
        };

    } /* devices */
} /* prismatoid */
#endif       /* end of include guard: LIGHTPACK_DEVICE_HPP */
