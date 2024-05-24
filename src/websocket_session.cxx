#include <boost/asio/buffer.hpp>
#include <boost/beast/core/bind_handler.hpp>
#include <boost/beast/core/error.hpp>
#include <websocket_session.hxx>
#include <websocket_target.hxx>
#include <algorithm>

namespace msserver
{
    namespace net = boost::asio;

    void websocket_session::on_accept( beast::error_code ec ) noexcept
    {
        if ( ec )
        {
            fail( ec, "Acception" );
        }

        do_read();
    }

    void websocket_session::do_read() noexcept
    {
        wsstream.async_read( buffer, beast::bind_front_handler( &websocket_session::on_read, shared_from_this() ) );
    }

    void websocket_session::on_read( beast::error_code ec, std::size_t bytes_transferred ) noexcept
    {
        if ( ec == websocket::error::closed )
        {
            return;
        }

        if ( ec )
        {
            fail( ec, "Reading" );
            return;
        }

        handle_request();
    }

    void websocket_session::handle_request()
    {
        std::string response;

        auto target_ = std::find_if( targets.begin(), targets.end(), [ this ]( const websocket_target &t )
                                     { return t.ep == endp; } );

        if ( target_ != targets.end() )
        {
            target_->handler( net::buffer_cast<const char *>( buffer.data() ), response );
            wsstream.async_write( net::buffer( response ), beast::bind_front_handler( &websocket_session::on_write,
                                                                                      shared_from_this() ) );
        }
    }

    void websocket_session::on_write( beast::error_code ec, std::size_t bytes_transferred )
    {
        if ( ec )
        {
            fail( ec, "Write websocket" );
        }

        buffer.consume( buffer.size() );

        do_read();
    }

} // namespace msserver
