#include <algorithm>
#include <websocket_session.hxx>
#include <boost/beast/core/bind_handler.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/verb.hpp>
#include <boost/beast/http/write.hpp>
#include <boost/core/ignore_unused.hpp>
#include <http_session.hxx>
#include <http_target.hxx>

namespace msserver
{
    void http_session::do_read()
    {
        parser.emplace();

        parser->body_limit( 10000 );

        stream.expires_after( std::chrono::seconds( 30 ) );

        http::async_read(
            stream, buffer, *parser,
            beast::bind_front_handler( &http_session::on_read, shared_from_this() ) );
    }

    void http_session::on_read( beast::error_code ec, std::size_t bytes_transferred ) noexcept
    {
        if ( ec == http::error::end_of_stream )
        {
            return do_close();
        }

        auto req = parser->get();

        if ( websocket::is_upgrade( req ) )
        {
            std::make_shared<websocket_session>( stream.release_socket(), std::string( req.target() ), websocket_targets, state )->run( parser->release() );
            return;
        }

        handle_request( req );
    }

    void http_session::handle_request( const http::request<http::string_body> &req )
    {
        auto self          = shared_from_this();
        http::verb method  = req.method();
        std::string target = std::string( req.target() );

        if ( target == "/connect" )
        {
            handle_connect( req );
            return;
        }

        http::response<http::string_body> response;

        auto targ = std::find_if( http_targets.begin(), http_targets.end(), [ method, target ]( http_target t )
                                  { return t.method == method && t.endp == target; } );

        if ( targ == http_targets.end() )
        {
            response = e404();
        }
        else
        {
            targ->handler( req, response );
        }

        http::write( stream, response );
        do_close();
    }

    void http_session::do_close()
    {
        beast::error_code ec;
        stream.socket().shutdown( tcp::socket::shutdown_send, ec );
    }

    http::response<http::string_body> http_session::e404() const noexcept
    {
        http::response<http::string_body> response;
        response.result( http::status::not_found );
        response.body() = "Not found!";
        response.set( http::field::content_type, "text/plain" );

        return response;
    }

    void http_session::handle_connect( const http::request<http::string_body> &req )
    {
        std::make_shared<websocket_session>( stream.release_socket(), std::string( req.target() ), websocket_targets, state )->run( parser->release() );
    }
} // namespace msserver