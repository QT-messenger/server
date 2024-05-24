#pragma once
#ifndef HTTP_SESSION_HXX
#    define HTTP_SESSION_HXX

#    include "websocket_target.hxx"
#    include <boost/asio/dispatch.hpp>
#    include <boost/beast/core/tcp_stream.hpp>
#    include <boost/beast/http/empty_body.hpp>
#    include <http_target.hxx>
#    include <boost/asio/ip/tcp.hpp>
#    include <boost/beast/core/flat_buffer.hpp>
#    include <boost/beast/http.hpp>
#    include <boost/beast/http/message.hpp>
#    include <boost/beast/http/string_body.hpp>
#    include <memory>
#    include <vector>

using tcp       = boost::asio::ip::tcp;
namespace beast = boost::beast;
namespace http  = beast::http;
namespace net   = boost::asio;

namespace msserver
{

    class http_session : public std::enable_shared_from_this<http_session>
    {
      private:
        beast::tcp_stream stream;
        beast::flat_buffer buffer;
        const std::vector<http_target> http_targets;
        std::vector<websocket_target> websocket_targets;
        boost::optional<http::request_parser<http::string_body>>
            parser;

      public:
        inline http_session( tcp::socket &sock, const std::vector<http_target> &http_targets, const std::vector<websocket_target> &websocket_targets ) :
            stream( std::move( sock ) ),
            http_targets( http_targets ),
            websocket_targets( websocket_targets )
        {
        }

        inline void run()
        {
            net::dispatch( stream.get_executor(),
                           beast::bind_front_handler( &http_session::do_read,
                                                      this->shared_from_this() ) );
        }

      private:
        void do_read();
        void on_read( beast::error_code ec, std::size_t bytes_transferred ) noexcept;
        void handle_request( const http::request<http::string_body> &req );
        void do_close();

        http::response<http::string_body> e404() const noexcept;
    };

} // namespace msserver

#endif