#pragma once
#ifndef WEBSOCKET_SESSION_HXX
#    define WEBSOCKET_SESSION_HXX
#    include <memory>
#    include <boost/asio/dispatch.hpp>
#    include <boost/beast/core/bind_handler.hpp>
#    include <boost/beast/core/error.hpp>
#    include <boost/beast/core/flat_buffer.hpp>
#    include <boost/asio/ip/tcp.hpp>
#    include <boost/beast/http/message.hpp>
#    include <boost/beast/websocket/stream.hpp>
#    include <websocket_target.hxx>
#    include <def.hxx>

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
        std::vector<websocket_target> targets;
        http::request<http::string_body> req;

      public:
        inline websocket_session( tcp::socket &socket, const std::string &endp, const std::vector<websocket_target> &targets ) :
            wsstream( std::move( socket ) ),
            endp( endp ),
            targets( targets )
        {
        }

        inline void run( http::request<http::string_body> req ) noexcept
        {
            wsstream.async_accept( req, beast::bind_front_handler( &websocket_session::on_accept, shared_from_this() ) );
        }

      private:
        void on_run() noexcept;
        void on_accept( beast::error_code ec ) noexcept;
        void do_read() noexcept;
        void on_read( beast::error_code ec, std::size_t bytes_transferred ) noexcept;
        void on_write( beast::error_code ec, std::size_t bytes_transferred ) noexcept;
    };

} // namespace msserver

#endif