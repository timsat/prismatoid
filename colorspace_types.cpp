#include "colorspace_types.hpp"
#include <cmath>
#include <algorithm>

namespace prismatoid {
    namespace types {
        const char kBytesPerPixel = 4;

        //Observer= 2°, Illuminant= D65
        const float kRefX = 95.047;
        const float kRefY = 100.000;
        const float kRefZ = 108.883;

        template<typename A, typename T>
        T WithinRange(A x, T min, T max) {
            T result;
            if (x < min)
                result = min;
            else if (x > max)
                result = max;
            else
                result = x;
            return result;
        }

        Rgb12::Rgb12(unsigned char r, unsigned char g, unsigned char b) {
            float k = 4095.0 / 255.0;
            r_ = r * k;
            g_ = g * k;
            b_ = b * k;
        }

        void Rgb12::correctGamma(double gamma) {
            r_ = 4095 * pow(r() / 4095.0, gamma);
            g_ = 4095 * pow(g() / 4095.0, gamma);
            b_ = 4095 * pow(b() / 4095.0, gamma);
        }

        void Rgb12::correctBrightness(unsigned char brightness) {
            // brightness -- must be in percents (0..100%)
            r_ = (brightness / 100.0) * r();
            g_ = (brightness / 100.0) * g();
            b_ = (brightness / 100.0) * b();
        }

        void Rgb12::trim(unsigned int max) {
            if (r() > max) r_ = max;
            if (g() > max) g_ = max;
            if (b() > max) b_ = max;
        }

        Xyz * Rgb12::toXyz(Xyz * dest) {
            //12bit RGB from 0 to 4095
            double r = ( this->r() / 4095.0 );
            double g = ( this->g() / 4095.0 );
            double b = ( this->b() / 4095.0 );

            if ( r > 0.04045 )
                r = pow( (r + 0.055)/1.055, 2.4);
            else
                r = r / 12.92;

            if ( g > 0.04045 )
                g = pow( (g + 0.055)/1.055, 2.4);
            else
                g = g / 12.92;

            if ( b > 0.04045 )
                b = pow( (b + 0.055)/1.055, 2.4);
            else
                b = b / 12.92;

            r = r * 100;
            g = g * 100;
            b = b * 100;

            //Observer. = 2°, Illuminant = D65
            dest->setX(r * 0.4124 + g * 0.3576 + b * 0.1805);
            dest->setY(r * 0.2126 + g * 0.7152 + b * 0.0722);
            dest->setZ(r * 0.0193 + g * 0.1192 + b * 0.9505);

            return dest;
        }

        Lab * Rgb12::toLab(Lab * dest) {
            Xyz xyz;
            this->toXyz(&xyz);
            return xyz.toLab(dest);
        }

        Lab Rgb12::toLab() {
            Xyz xyz;
            Lab lab;
            this->toXyz(&xyz);
            xyz.toLab(&lab);
            return lab;
        }

        Xyz * Lab::toXyz(Xyz * dest) {
            double y = (l_ + 16.0) / 116.0;
            double x = a_ / 500.0 + y;
            double z = y - b_ / 200.0;

            if ( y*y*y > 0.008856 )
                y = y*y*y;
            else
                y = ( y - 16.0 / 116 ) / 7.787;

            if ( x*x*x > 0.008856 )
                x = x*x*x;
            else
                x = ( x - 16.0 / 116 ) / 7.787;

            if ( z*z*z > 0.008856 )
                z = z*z*z;
            else
                z = ( z - 16.0 / 116 ) / 7.787;

            dest->setX(kRefX * x);     //ref_X =  95.047     Observer= 2°, Illuminant= D65
            dest->setY(kRefY * y);     //ref_Y = 100.000
            dest->setZ(kRefZ * z);     //ref_Z = 108.883

            return dest;
        }

        Rgb12 * Lab::toRgb12(Rgb12 * dest) {
            Xyz xyz;
            this->toXyz(&xyz);
            return xyz.toRgb12(dest);
        }

        Lab * Xyz::toLab(Lab * dest) {

            double x = x_ / kRefX;          //ref_X =  95.047   Observer= 2°, Illuminant= D65
            double y = y_ / kRefY;          //ref_Y = 100.000
            double z = z_ / kRefZ;          //ref_Z = 108.883

            if ( x > 0.008856 )
                x = pow(x, 1.0/3);
            else
                x = (7.787 * x) + ( 16.0 / 116 );

            if ( y > 0.008856 )
                y = pow(y, 1.0/3);
            else
                y = (7.787 * y) + (16.0 / 116);

            if ( z > 0.008856 )
                z = pow(z, 1.0/3);
            else
                z = (7.787 * z) + (16.0 / 116);

            dest->setL(round((116 * y) - 16));
            dest->setA(WithinRange<double, char>(round(500 * ( x - y )), -128, 127));
            dest->setB(WithinRange<double, char>(round(200 * ( y - z )), -128, 127));

            return dest;
        }


        Rgb12 * Xyz::toRgb12(Rgb12 * dest) {
            double x = x_ / 100;        //X from 0 to  95.047      (Observer = 2°, Illuminant = D65)
            double y = y_ / 100;        //Y from 0 to 100.000
            double z = z_ / 100;        //Z from 0 to 108.883

            double r = x *  3.2406 + y * -1.5372 + z * -0.4986;
            double g = x * -0.9689 + y *  1.8758 + z *  0.0415;
            double b = x *  0.0557 + y * -0.2040 + z *  1.0570;

            if ( r > 0.0031308 )
                r = 1.055 * pow(r, (1 / 2.4)) - 0.055;
            else
                r = 12.92 * r;

            if ( g > 0.0031308 )
                g = 1.055 * pow(g, (1 / 2.4)) - 0.055;
            else
                g = 12.92 * g;

            if ( b > 0.0031308 )
                b = 1.055 * pow(b, (1 / 2.4)) - 0.055;
            else
                b = 12.92 * b;

            r = WithinRange<double, double>(r, 0, 1);
            g = WithinRange<double, double>(g, 0, 1);
            b = WithinRange<double, double>(b, 0, 1);

            dest->setR(round(r*4095));
            dest->setG(round(g*4095));
            dest->setB(round(b*4095));

            return dest;
        }

    } /* types */
} /* prismatoid */
