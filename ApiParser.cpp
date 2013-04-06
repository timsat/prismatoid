
#include "ApiParser.hpp"

//#define BOOST_SPIRIT_DEBUG

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_eoi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/fusion/adapted.hpp>
#include <boost/spirit/home/phoenix/bind/bind_function.hpp>


#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include "color_types.hpp"
#include "LightpackDevice.hpp"

namespace prismatoid {
    namespace api {

        namespace dev = prismatoid::devices;
        namespace qi = boost::spirit::qi;
        namespace ascii = boost::spirit::ascii;
        namespace phoenix = boost::phoenix;

        typedef std::tuple<int,int,int> color_t;

        void out_int(int v) {
            std::cout << v << std::endl;
        }

        void out_color_t(color_t v) {
            std::cout << std::get<0>(v)<< ", "<< std::get<1>(v)<< ", "<< std::get<2>(v) << std::endl;
        }

        void set_colors(std::vector<color_t> v) {
            std::cout << "SetColors action" << std::endl;
            std::vector<types::RgbColor> colors;
            for(std::vector<color_t>::iterator it = v.begin(); it != v.end(); ++it) {
                out_color_t(*it);
                types::RgbColor color = { std::get<0>(*it), std::get<1>(*it), std::get<2>(*it), 0x0fff};
                colors.push_back(color);
            }           
            dev::LightpackDevice * lightpack = dev::LightpackDevice::instance();
            lightpack->set_colors(colors);
            
        }

        template <typename Iterator>
        struct cmd
          : qi::grammar<Iterator, std::vector<color_t>(), ascii::space_type>
        {
            cmd()
              : cmd::base_type(start)
            {
                using qi::_1;

                start %= (qi::lit("SetColors") >> list >> (qi::eol | qi::eoi)) [bind(&set_colors,_1)];

                list %= (color_val % ',');

                color_val %= (qi::lit('(') >> qi::int_ >> qi::lit(',') >> qi::int_ >> qi::lit(',') >> qi::int_ >> qi::lit(')'));

            }
            qi::rule<Iterator, std::vector<color_t>(), ascii::space_type> start, list;
            qi::rule<Iterator, color_t(), ascii::space_type> color_val;
        };

        void ApiParser::parsecmd(std::string &in) {
            namespace qi = boost::spirit::qi;

            std::vector<color_t> cmdResult;
            cmd<std::string::iterator> c;
            bool const result = qi::phrase_parse(in.begin(), in.end(), c, ascii::space, cmdResult);
            if (!result)
                std::cout << "error parsing string: " << in << std::endl;
            
        }

    
    } /* api */
} /* prismatoid */
