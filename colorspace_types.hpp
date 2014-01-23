/*
 * colorspace_types.hpp
 *
 *  Created on: 2013-07-22
 *     Project: Lightpack
 *
 *  Copyright (c) 2013 Timur Sattarov tim.helloworld at gmail.com
 *
 *  Lightpack a USB content-driving ambient lighting system
 *
 *  Lightpack is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Lightpack is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef COLORSPACE_TYPES_HPP
#define COLORSPACE_TYPES_HPP
#include <string>
namespace prismatoid
{
namespace types
{
    class Lab;
    class Xyz;

    /*!
      36bit RGB, 12 bit per channel!
      */
    class Rgb12 {
    public:
        Rgb12() { r_ = g_ = b_ = 0; }
        Rgb12(unsigned int r, unsigned int g, unsigned int b): r_(r), g_(g), b_(b){}
        Rgb12(unsigned char r, unsigned char g, unsigned char b);

        void correctGamma(double gamma);
        void correctBrightness(unsigned char brightness);
        void trim(unsigned int max);

        Xyz * toXyz(Xyz *dest);
        Lab * toLab(Lab *dest);
        Lab toLab();
        const std::string toString() const { return std::string(); }

        void setR(unsigned int r) { r_ = r & 0x0fff; }
        unsigned int r() const { return (r_ & 0x0fff); }

        void setG(unsigned int g) { g_ = g & 0x0fff; }
        unsigned int g() const { return g_ & 0x0fff; }

        void setB(unsigned int b) { b_ = b & 0x0fff; }
        unsigned int b() const { return b_ & 0x0fff; }

        unsigned char r8() const { return (r_ >> 4)& 0xff; }
        unsigned char g8() const { return (g_ >> 4)& 0xff; }
        unsigned char b8() const { return (b_ >> 4)& 0xff; }

    protected:
        unsigned int r_, g_, b_;
    };

    class Xyz {
    public:
        Xyz() { x_ = y_ = z_ = 0; }
        Xyz(double x, double y, double z): x_(x), y_(y), z_(z){}
        Lab * toLab(Lab *dest);
        Rgb12 * toRgb12(Rgb12 *dest);

        void setX(double x) { x_ = x; }
        double x() const { return x_; }

        void setY(double y) { y_ = y; }
        double y() const { return y_; }

        void setZ(double z) { z_ = z; }
        double z() const { return z_; }

    protected:
        double x_, y_, z_;
    };

    class Lab {
    public:
        Lab() { }
        Lab(unsigned int l, int a, int b): l_(l), a_(a), b_(b){}
        Xyz * toXyz(Xyz *dest);
        Rgb12 * toRgb12(Rgb12 *dest);

        void setL(unsigned char l) { l_ = l; }
        unsigned char l() const { return l_; }

        void setA(char a) { a_ = a; }
        char a() const { return a_; }

        void setB(char b) { b_ = b; }
        char b() const { return b_; }

    protected:
        unsigned char l_;
        char a_,b_;
    };
}
}
#endif // COLORSPACE_TYPES_HPP
