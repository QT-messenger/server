#pragma once
#ifndef DEF_HXX
#    define DEF_HXX
#    include <boost/beast/http/string_body.hpp>
#    include <boost/json.hpp>
#    include <functional>
#    include <iostream>
#    include <shared_state.hxx>
#    include <websocket_handler_result.hxx>
#    define HTTP_TARGET_HOST "0.0.0.0"
#    define HTTP_TARGET_PORT 8080

namespace http  = boost::beast::http;
namespace beast = boost::beast;
namespace json  = boost::json;

using http_request_handler             = std::function<void( const http::request<http::string_body> &request, http::response<http::string_body> &response )>;
using raw_websocket_request_handler    = std::function<msserver::websocket_handler_result( const std::string &request, std::string &response, std::shared_ptr<msserver::shared_state> state, std::shared_ptr<msserver::websocket_session> self )>;
using parsed_websocket_request_handler = std::function<msserver::websocket_handler_result( json::object &parsed_request, std::string &response, std::shared_ptr<msserver::shared_state> state, std::shared_ptr<msserver::websocket_session> self )>;

inline void fail( beast::error_code ec, const std::string &text ) noexcept
{
    std::cerr << text << ": " << ec.message() << std::endl;
}

#endif