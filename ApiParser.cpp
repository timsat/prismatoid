
#include "ApiParser.hpp"

//#define BOOST_SPIRIT_DEBUG

#include <boost/spirit/include/qi.hpp>
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

namespace prismatoid {
    namespace api {

        namespace qi = boost::spirit::qi;
        namespace ascii = boost::spirit::ascii;
        namespace phoenix = boost::phoenix;

        struct Color {
            int r;
            int g;
            int b;
        };

        typedef std::tuple<int,int,int> color_t;

        void out_int(int v) {
            std::cout << v << std::endl;
        }

        void out_color_t(color_t v) {
            std::cout << std::get<0>(v)<< ", "<< std::get<1>(v)<< ", "<< std::get<2>(v) << std::endl;
        }

        void setColors(std::vector<color_t> v) {
            std::cout << "SetColors action" << std::endl;
            for(std::vector<color_t>::iterator it = v.begin(); it != v.end(); ++it) {
                out_color_t(*it);
            }
        }

        template <typename Iterator>
        struct cmd
          : qi::grammar<Iterator, std::vector<color_t>(), ascii::space_type>
        {
            cmd()
              : cmd::base_type(start)
            {
                using qi::_1;

                start %= (qi::lit("SetColors") >> list) [bind(&setColors,_1)];

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
                std::cout << "error" << std::endl;
            
        }

    
    } /* api */
} /* prismatoid */

