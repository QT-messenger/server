#include <boost/beast/http/read.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/verb.hpp>
#include <boost/core/ignore_unused.hpp>
#include <http_session.hxx>
#include <http_target.hxx>
#include <algorithm>
#include <iostream>

namespace msserver
{
    void http_session::do_read() noexcept
    {
        auto self = shared_from_this();
        http::async_read( socket, buffer, req, [ self ]( beast::error_code ec, std::size_t bytes_transferred )
                          {
          boost::ignore_unused(bytes_transferred);
          if (!ec) {
            self->on_read();
          } } );
    }

    void http_session::on_read() noexcept
    {
        auto self = shared_from_this();

        http::verb method  = req.method();
        std::string target = req.target().data();

        http::response<http::string_body> response;

        auto targ = std::find_if( targets.begin(), targets.end(), [ method, target ]( http_target t )
                                  { return t.method == method && t.endp == target; } );

        if ( targ == targets.end() )
        {
            response = e404();
        }

        targ->handler( req, response );

        http::async_write( socket, response,
                           [ self ]( beast::error_code ec, std::size_t bytes_transferred )
                           {
                               boost::ignore_unused( bytes_transferred );
                               self->socket.shutdown( tcp::socket::shutdown_send, ec );
                           } );
    }

    http::response<http::string_body> http_session::e404() const noexcept
    {
        http::response<http::string_body> response;
        response.result( http::status::not_found );
        response.body() = "Not found!";
        response.set( http::field::content_type, "text/plain" );

        return response;
    }
} // namespace msserver