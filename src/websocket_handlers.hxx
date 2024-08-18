#pragma once
#ifndef WEBSOCKET_HANDLERS
#    define WEBSOCKET_HANDLERS
#    include <string>
#    include <boost/json.hpp>
#    include <memory>
#    include <shared_state.hxx>
#    include <websocket_error.hxx>

namespace msserver
{
    namespace json = boost::json;

    json::object validate_request( const json::value &value );
    websocket_error handle( const std::string &str, std::string &req, std::shared_ptr<shared_state> state, std::shared_ptr<websocket_session> self );
    websocket_error handle_connect( json::object &req, std::string &resp, std::shared_ptr<shared_state> state, std::shared_ptr<websocket_session> self ) noexcept;
    websocket_error handle_sendmessage( json::object &req, std::string &resp, std::shared_ptr<shared_state> state, std::shared_ptr<websocket_session> self ) noexcept;

    inline json::value parse_request_string_unchecked( const std::string &req )
    {
        return json::parse( req );
    }

    inline json::object parse_request_string( const std::string &req )
    {
        return validate_request( parse_request_string_unchecked( req ) );
    }

} // namespace msserver
#endif