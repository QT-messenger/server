#pragma once
#ifndef WEBSOCKET_SESSION_HXX
#    define WEBSOCKET_SESSION_HXX
#    include <boost/asio/dispatch.hpp>
#    include <boost/beast/core/bind_handler.hpp>
#    include <boost/beast/core/error.hpp>
#    include <boost/beast/core/flat_buffer.hpp>
#    include <boost/asio/ip/tcp.hpp>
#    include <boost/beast/http/message.hpp>
#    include <boost/beast/websocket/stream.hpp>
#    include <def.hxx>
#    include <websocket_target.hxx>
#    include <shared_state.hxx>
#    include <memory>

namespace msserver
{

    using tcp           = boost::asio::ip::tcp;
    namespace beast     = boost::beast;
    namespace websocket = beast::websocket;
    namespace net       = boost::asio;

    class websocket_session : public std::enable_shared_from_this<websocket_session>
    {
      private:
        websocket::stream<tcp::socket> wsstream;
        beast::flat_buffer buffer;
        std::string endp;
        const std::vector<websocket_target> &targets;
        http::request<http::string_body> req;
        uint64_t id { 0 };

      public:
        friend shared_state;
        std::shared_ptr<shared_state> state;
        inline websocket_session( tcp::socket &&socket, const std::string &endp, const std::vector<websocket_target> &targets, std::shared_ptr<shared_state> state ) :
            wsstream( std::move( socket ) ),
            endp( endp ),
            targets( targets ),
            state( state )
        {
            state->join( *this );
        }

        inline ~websocket_session()
        {
            state->leave( *this );
        }

        inline void run( http::request<http::string_body> req ) noexcept
        {
            wsstream.set_option(
                websocket::stream_base::decorator( []( websocket::response_type &res )
                                                   { res.set( http::field::server,
                                                              ( BOOST_BEAST_VERSION_STRING " ms-server" ) ); } ) );

            wsstream.async_accept( req, beast::bind_front_handler( &websocket_session::on_accept, shared_from_this() ) );
        }

        inline size_t send( const std::string &message ) noexcept
        {
            beast::error_code ec;
            return wsstream.write( net::buffer( message ), ec );
        }

        constexpr inline uint64_t get_id() const noexcept
        {
            return id;
        }

      private:
        void on_accept( beast::error_code ec ) noexcept;
        void do_read() noexcept;
        void on_read( beast::error_code ec, std::size_t bytes_transferred ) noexcept;
        void handle_request();
        void on_write( beast::error_code ec );
        websocket_session *find_session( uint64_t id ) noexcept;
    };

} // namespace msserver

#endif