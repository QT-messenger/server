#include <boost/asio/buffer.hpp>
#include <boost/beast/core/bind_handler.hpp>
#include <boost/beast/core/error.hpp>
#include <websocket_session.hxx>
#include <websocket_target.hxx>
#include <algorithm>

namespace msserver
{
    void websocket_session::on_run() noexcept
    {
        wsstream.set_option(
            websocket::stream_base::timeout::suggested( beast::role_type::server ) );

        wsstream.set_option(
            websocket::stream_base::decorator( []( websocket::response_type &res )
                                               { res.set( http::field::server, std::string( BOOST_BEAST_VERSION_STRING ) +
                                                                                   " websocket-server-async" ); } ) );

        beast::error_code ec;
        wsstream.accept( req, ec );
        on_accept( ec );
        // wsstream.async_accept(
        // beast::bind_front_handler( &websocket_session::on_accept, shared_from_this() ) );
    }

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
        if ( ec )
        {
            fail( ec, "Reading" );
        }

        std::string response;

        auto target_ = std::find_if( targets.begin(), targets.end(), [ this ]( const websocket_target &t )
                                     { return t.ep == endp; } );

        if ( target_ != targets.end() )
        {
            target_->handler( net::buffer_cast<const char *>( buffer.data() ), response );
            wsstream.write( net::buffer( response ) );
            buffer.consume( buffer.size() );
        }

        do_read();
    }
} // namespace msserver
