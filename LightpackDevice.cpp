#include "LightpackDevice.hpp"
#include <cstring>
#include <iostream>

namespace prismatoid {
    namespace devices {

        LightpackDevice *LightpackDevice::this_ = NULL;

        LightpackDevice::LightpackDevice(libusb_context * ctx) {
            dev_ = NULL;
            ctx_ = ctx;
        }

        LightpackDevice::~LightpackDevice() {
            if (LightpackDevice::this_)
                delete LightpackDevice::this_;
        }

        LightpackDevice *LightpackDevice::init(libusb_context * ctx) {
            if (!LightpackDevice::this_)
                LightpackDevice::this_ = new LightpackDevice(ctx);
            return LightpackDevice::this_;
        }

        LightpackDevice *LightpackDevice::instance() {
            return LightpackDevice::this_;
        }

        bool LightpackDevice::open() {
            if (! dev_) {
                dev_ = libusb_open_device_with_vid_pid(ctx_, 0x03EB, 0x204F);
                if (dev_) {

                    libusb_detach_kernel_driver(dev_, 0);
                    libusb_claim_interface(dev_, 0);
                }
            }
            return dev_ != NULL;
        }

        void LightpackDevice::close() {
            if (dev_) {
                libusb_release_interface(dev_, 0);
                libusb_attach_kernel_driver(dev_, 0);

                dev_ = NULL;
            }
        }

        bool LightpackDevice::set_colors(const std::vector<types::RgbColor>& colors) {

            buf_[0] = 0x01;
            int idx = 1;

            for(std::vector<types::RgbColor>::const_iterator it = colors.begin(); it != colors.end(); ++it) {
                buf_[idx++] = (*it).r;
                buf_[idx++] = (*it).g;
                buf_[idx++] = (*it).b;

                buf_[idx++] = 0;// (*it).r & 0x0f;
                buf_[idx++] = 0;// (*it).g & 0x0f;
                buf_[idx++] = 0;// (*it).b & 0x0f;
            }
            
            if (idx < kLightpackDeviceBufferSize) {
                memset(buf_ + idx, 0, kLightpackDeviceBufferSize - idx);
            }
            size_t bytes_to_transfer = kLightpackDeviceBufferSize;

            int result = libusb_control_transfer(dev_,
                                                 LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_CLASS
                                                 | LIBUSB_RECIPIENT_INTERFACE,
                                                 0x09,
                                                 (2 << 8),
                                                 0x00,
                                                 buf_, bytes_to_transfer, 1000);
            return result == bytes_to_transfer;
        }
    }                           /* devices */
}                               /* prismatoid */
