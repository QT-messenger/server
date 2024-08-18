#include <boost/asio/buffer.hpp>
#include <boost/beast/core/bind_handler.hpp>
#include <boost/beast/core/buffers_to_string.hpp>
#include <boost/beast/core/error.hpp>
#include <websocket_session.hxx>
#include <websocket_target.hxx>
#include <algorithm>

namespace msserver
{
    // websocket_session *websocket_session::find_session( uint64_t id ) noexcept
    // {
    //     auto sessions = state->get_sessions();
    //     if ( id >= sessions.size() )
    //         return nullptr;
    //     return sessions[ id ];
    // }

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
            fail( ec, "closed" );
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
            auto result = target_->handler( beast::buffers_to_string( buffer.data() ), response, state, shared_from_this() );

            // if ( result == websocket_error::Ok )
            // {
            //     auto session = find_session( id );
            //     if ( session == nullptr )
            //     {
            //     }
            // }
        }

        beast::error_code ec;
        on_write( ec );
    }

    void websocket_session::on_write( beast::error_code ec )
    {
        if ( ec )
        {
            fail( ec, "Write" );
        }

        buffer = beast::flat_buffer();

        do_read();
    }

} // namespace msserver
