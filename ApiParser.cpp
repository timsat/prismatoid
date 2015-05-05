
#include "ApiParser.hpp"

//#define BOOST_SPIRIT_DEBUG

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_eoi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/fusion/adapted.hpp>


#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include "colorspace_types.hpp"
#include "LightpackDevice.hpp"

namespace prismatoid {
    namespace api {

        namespace dev = prismatoid::devices;
        namespace qi = boost::spirit::qi;
        namespace ascii = boost::spirit::ascii;
        namespace phoenix = boost::phoenix;
        using namespace std;

        typedef tuple<int,int,int> color_t;

        void print_int(int v) {
            cout << v << endl;
        }

        void print_color_t(color_t v) {
            cout << get<0>(v)<< ", "<< get<1>(v)<< ", "<< get<2>(v) << endl;
        }

        void set_colors(vector<color_t> v) {
            cout << "SetColors action" << endl;
            vector<types::Rgb12> colors;
            for(vector<color_t>::iterator it = v.begin(); it != v.end(); ++it) {
                print_color_t(*it);
                types::Rgb12 color((unsigned char)get<0>(*it), (unsigned char)get<1>(*it), (unsigned char)get<2>(*it));
                colors.push_back(color);
            }           
            dev::LightpackDevice * lightpack = dev::LightpackDevice::instance();
            lightpack->set_colors(colors);
            
        }

        void set_smoothness(int smoothness) {
            cout << "SetSmoothness action " << smoothness << endl;
            dev::LightpackDevice * lightpack = dev::LightpackDevice::instance();
            lightpack->set_smoothness(smoothness);
        }

        void construct_list(vector<color_t> &rule_val, int length, color_t color)  {
            for (int i = 0; i < length; i++) {
                rule_val.push_back(color);
            }
        }

        template <typename Iterator>
        struct cmd
          : qi::grammar<Iterator, vector<color_t>(), ascii::space_type>
        {
            cmd()
              : cmd::base_type(start)
            {
                start =  (qi::lit("SetColors") >> list >> (qi::eol | qi::eoi)) [bind(&set_colors,qi::_1)]

                        | (qi::lit("SetSmoothness") >> qi::int_ >> (qi::eol | qi::eoi)) [bind(&set_smoothness,qi::_1)];


                list %= (color_val % ',') | list_short;

                list_short = (qi::lit('[') >> qi::int_ >> qi::lit(']') >> color_val)[bind(&construct_list, qi::_val, qi::_1, qi::_2)];

                color_val %= (qi::lit('(') >> qi::int_ >> qi::lit(',') >> qi::int_ >> qi::lit(',') >> qi::int_ >> qi::lit(')'));

            }
            qi::rule<Iterator, vector<color_t>(), ascii::space_type> start, list, list_short;
            qi::rule<Iterator, color_t(), ascii::space_type> color_val;
        };

        void ApiParser::parsecmd(string &in) {
            vector<color_t> cmdResult;
            cmd<string::iterator> c;
            bool const result = qi::phrase_parse(in.begin(), in.end(), c, ascii::space, cmdResult);
            if (!result)
                cout << "error parsing string: " << in << endl;
            
        }

    
    } /* api */
} /* prismatoid */

