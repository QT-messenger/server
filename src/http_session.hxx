#pragma once
#ifndef HTTP_SESSION_HXX
#    define HTTP_SESSION_HXX
#    include <memory>
#    include <vector>
#    include <boost/beast/http/empty_body.hpp>
#    include <boost/asio/ip/tcp.hpp>
#    include <boost/beast/core/flat_buffer.hpp>
#    include <boost/beast/http.hpp>
#    include <boost/beast/http/message.hpp>
#    include <boost/beast/http/string_body.hpp>
#    include <http_target.hxx>

using tcp       = boost::asio::ip::tcp;
namespace beast = boost::beast;
namespace http  = beast::http;

namespace msserver
{

    class http_session : public std::enable_shared_from_this<http_session>
    {
      private:
        tcp::socket socket;
        beast::flat_buffer buffer;
        http::request<http::string_body> req;
        const std::vector<http_target> &targets;

      public:
        inline http_session( tcp::socket &sock, const std::vector<http_target> &targets ) :
            socket( std::move( sock ) ),
            targets( targets ) {}

        inline void run()
        {
            do_read();
        }

        inline void run( const http::request<http::string_body> &req )
        {
            this->req = req;
            on_read();
        }

      private:
        void do_read() noexcept;
        void on_read() noexcept;
        void on_write( beast::error_code ec, std::size_t bytes_transferred ) noexcept;

        http::response<http::string_body> e404() const noexcept;
    };

} // namespace msserver

#endif