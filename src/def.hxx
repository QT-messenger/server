#ifndef DEF_HXX
#define DEF_HXX

#include <boost/beast/http/string_body.hpp>
#include <cstdint>
#include <functional>
#include <iostream>

namespace http  = boost::beast::http;
namespace beast = boost::beast;

using http_request_handler      = std::function<void( const http::request<http::string_body> &request, http::response<http::string_body> &response )>;
using websocket_request_handler = std::function<void( const std::string &request, std::string &response )>;

inline void fail( beast::error_code ec, const std::string &text )
{
    std::cerr << text << ": " << ec.message() << std::endl;
}

inline void fail( const std::string &e )
{
    std::cerr << e << std::endl;
}

#endif