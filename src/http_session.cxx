#include <boost/beast/core/bind_handler.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/verb.hpp>
#include <boost/beast/http/write.hpp>
#include <boost/core/ignore_unused.hpp>
#include <http_session.hxx>
#include <http_target.hxx>
#include <algorithm>

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
        auto self          = shared_from_this();
        http::verb method  = req.method();
        std::string target = req.target().data();

        http::response<http::string_body> response;

        auto targ = std::find_if( targets.begin(), targets.end(), [ method, target ]( http_target t )
                                  { return t.method == method && t.endp == target; } );

        if ( targ == targets.end() )
        {
            response = e404();
        }
        else
        {
            targ->handler( req, response );
        }

        beast::error_code ec;
        auto bt = http::write( socket, response, ec );
        on_write( ec, bt );
    }

    void http_session::on_write( beast::error_code ec, std::size_t bytes_transferred ) noexcept
    {
        boost::ignore_unused( bytes_transferred );
        socket.shutdown( tcp::socket::shutdown_send, ec );
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